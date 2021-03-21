/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "App.h"

#include <iostream>

#include <MessageRunner.h>
#include <Roster.h>
#include <StorageKit.h>
#include <String.h>

#include "AtomFeed.h"
#include "Entry.h"
#include "Feed.h"
#include "FeedController.h"
#include "FeedEditWindow.h"
#include "FeedsView.h"
#include "MainWindow.h"
#include "Mimetypes.h"
#include "Notifier.h"
#include "Preferences.h"
#include "RssFeed.h"
#include "Util.h"


App::App() : BApplication("application/x-vnd.Pogger")
{
	fPreferences = new Preferences;
	fPreferences->Load();

	fMainWindow = new MainWindow();
	fMainWindow->Show();

	fNotifier = new Notifier();
	fFeedController = new FeedController();

	BMessage updateMessage(kUpdateSubscribed);
	int64 interval = fPreferences->UpdateInterval();
	int32 count = -1;

	if (interval == -1)
		count = 0;
//	else
//		MessageReceived(&updateMessage);

	fUpdateRunner = new BMessageRunner(this, updateMessage, interval, count);
}


void
App::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case kDownloadComplete:
			fMainWindow->PostMessage(msg);
		case kEnqueueFeed:
		case kUpdateSubscribed:
		case kControllerCheck:
		{
			fFeedController->MessageReceived(msg);
			break;
		}
		case kFeedsEdited:
		case kDownloadStart:
		{
			fMainWindow->PostMessage(msg);
			break;
		}
		case kClearQueue:
		{
			fFeedController->MessageReceived(msg);
			break;
		}
		case kProgress:
		case kParseComplete:
		case kParseFail:
		case kDownloadFail:
		{
			fNotifier->MessageReceived(msg);
			fMainWindow->PostMessage(msg);
			break;
		}
		case B_SILENT_RELAUNCH:
		{
			if (fMainWindow->IsMinimized() == true)
				fMainWindow->Minimize(false);
			break;
		}
		default:
		{
			BApplication::MessageReceived(msg);
			break;
		}
	}
}


bool
App::QuitRequested()
{
	delete fUpdateRunner, fFeedController, fNotifier;
	if (fMainWindow->Lock())
		fMainWindow->Quit();

	fPreferences->Save();
	delete fPreferences;
	return true;
}


void
App::ArgvReceived(int32 argc, char** argv)
{
	BMessage refMsg(B_REFS_RECEIVED);

	for (int i = 1; i < argc; i++) {
		entry_ref ref;
		BEntry entry(argv[i]);

		if (entry.Exists() && entry.GetRef(&ref) == B_OK) {
			refMsg.AddRef("refs", &ref);
		}
		else if (BUrl(argv[i]).IsValid()) {
			BString url = BString(argv[i]);

			BMessage enqueue = BMessage(kEnqueueFeed);
			enqueue.AddData("feedPaths", B_STRING_TYPE, &url, sizeof(BString));
			MessageReceived(&enqueue);
		}
	}
	RefsReceived(&refMsg);
}


void
App::RefsReceived(BMessage* message)
{
	int i = 0;
	entry_ref ref;
	BFile file;
	BNodeInfo info;
	char type[B_FILE_NAME_LENGTH];

	while (message->HasRef("refs", i)) {
		BMessage msg(B_REFS_RECEIVED);
		message->FindRef("refs", i++, &ref);
		msg.AddRef("refs", &ref);

		file.SetTo(&ref, B_READ_ONLY);
		info.SetTo(&file);
		info.GetType(type);

		if (BString(type) == BString("text/x-feed-entry"))
			_OpenEntryFile(&msg);
		else if (BString(type) == BString("application/x-feed-source"))
			_OpenSourceFile(&msg);
	}
}


void
App::_OpenEntryFile(BMessage* refMessage)
{
	entry_ref entryRef;
	refMessage->FindRef("refs", &entryRef);

	if (fPreferences->EntryOpenAsHtml())
		_OpenEntryFileAsHtml(entryRef);
	else
		_OpenEntryFileAsUrl(entryRef);
}


void
App::_OpenEntryFileAsHtml(entry_ref ref)
{
	const char* openWith = fPreferences->EntryOpenWith();
	entry_ref openWithRef;
	BString entryTitle("untitled");
	BFile(&ref, B_READ_ONLY).ReadAttrString("Feed:name", &entryTitle);

	entry_ref tempRef = tempHtmlFile(&ref, entryTitle.String());
	BMessage newRefMessage(B_REFS_RECEIVED);
	newRefMessage.AddRef("refs", &tempRef);

	if (BMimeType(openWith).IsValid())
		BRoster().Launch(openWith, &newRefMessage);
	else if (BEntry(openWith).GetRef(&openWithRef) == B_OK)
		BRoster().Launch(&openWithRef, &newRefMessage);
}


void
App::_OpenEntryFileAsUrl(entry_ref ref)
{
	const char* openWith = fPreferences->EntryOpenWith();
	entry_ref openWithRef;
	BString entryUrl;
	if (BFile(&ref, B_READ_ONLY).ReadAttrString("META:url", &entryUrl) != B_OK)
		return;

	const char* urlArg = entryUrl.String();

	if (BMimeType(openWith).IsValid())
		BRoster().Launch(openWith, 1, &urlArg);
	else if (BEntry(openWith).GetRef(&openWithRef) == B_OK)
		BRoster().Launch(&openWithRef, 1, &urlArg);
}


void
App::_OpenSourceFile(BMessage* refMessage)
{
	entry_ref entryRef;
	refMessage->FindRef("refs", &entryRef);
	FeedEditWindow* window = new FeedEditWindow(BEntry(&entryRef));
	window->Show();
}


int
main(int argc, char** argv)
{
	srand(time(0));
	installMimeTypes();

	App* app = new App();
	app->Run();
	return 0;
}


