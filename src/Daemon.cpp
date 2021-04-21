/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "Daemon.h"

#include <iostream>

#include <MessageRunner.h>
#include <Roster.h>
#include <Url.h>

#include "DeskbarView.h"
#include "FeedController.h"
#include "Notifier.h"
#include "Preferences.h"
#include "Util.h"


App::App() : BApplication("application/x-vnd.PoggerDaemon")
{
	fPreferences = new Preferences;
	fPreferences->Load();

	fFeedController = new FeedController();
	fNotifier = new Notifier();

	BMessage updateMessage(kUpdateSubscribed);
	int64 interval = fPreferences->UpdateInterval();
	int32 count = -1;

	if (interval == -1)
		count = 0;
	else
		MessageReceived(&updateMessage);
	fUpdateRunner = new BMessageRunner(this, updateMessage, interval, count);

	installDeskbar();
}


void
App::MessageReceived(BMessage* msg)
{
	switch(msg->what)
	{
		case kPreferencesUpdated:
		{
			_ReloadPreferences();
			break;
		}	
		case kEnqueueFeed:
		case kUpdateSubscribed:
		case kControllerCheck:
		case kClearQueue:
		{
			fFeedController->MessageReceived(msg);
			break;
		}
		case kProgress:
		case kParseComplete:
		case kParseFail:
		case kDownloadFail:
			fNotifier->MessageReceived(msg);
		case kDownloadComplete:
		case kDownloadStart:
		{
			BMessenger toApp("application/x-vnd.Pogger");
			if (toApp.IsValid())
				toApp.SendMessage(msg);
			break;
		}
		default:
			BApplication::MessageReceived(msg);
	}
}


bool
App::QuitRequested()
{
	delete fUpdateRunner, fFeedController, fPreferences;
	removeDeskbar();
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

		if (BString(type) == "application/x-feed-entry"
			|| BString(type) == "text/x-feed-entry")
			_OpenEntryFile(&msg);
		else if (BString(type) == "application/x-feed-source")
			_OpenSourceFile(&msg);
	}
}


void
App::_OpenEntryFile(BMessage* refMessage)
{
	entry_ref entryRef;
	refMessage->FindRef("refs", &entryRef);
	BFile entryFile(&entryRef, B_WRITE_ONLY);

	BString readStatus("Read");
	entryFile.WriteAttrString("Feed:status", &readStatus);

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
	BRoster roster;
	roster.Launch("application/x-vnd.Pogger", refMessage);
}


void
App::_ReloadPreferences()
{
	fPreferences->Load();
	int64 interval = fPreferences->UpdateInterval();

	if (interval == -1)
		fUpdateRunner->SetCount(0);
	else {
		fUpdateRunner->SetCount(-1);
		fUpdateRunner->SetInterval(interval);
	}
}


int
main(int argc, char** arv)
{
	App app;
	app.Run();
	return 0;
}


