/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "App.h"

#include <Messenger.h>
#include <Roster.h>
#include <StorageKit.h>
#include <String.h>

#include "Entry.h"
#include "Feed.h"
#include "FeedController.h"
#include "FeedEditWindow.h"
#include "FeedsView.h"
#include "MainWindow.h"
#include "Mimetypes.h"
#include "Notifier.h"
#include "Preferences.h"
#include "SourceManager.h"
#include "Util.h"


App::App() : BApplication("application/x-vnd.Pogger")
{
	fPreferences = new Preferences;
	fPreferences->Load();

	fMainWindow = new MainWindow();

	SetPulseRate(100000);
	BRoster roster;
	roster.Launch("application/x-vnd.PoggerDaemon");
}


void
App::Pulse()
{
	// We want to delay showing the main window, just in case Pogger is used
	// only to open a feed entry (see _OpenEntryFile)
	fMainWindow->Show();
	SetPulseRate(0);
}


void
App::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case kDownloadComplete:
		case kFeedsEdited:
		case kDownloadStart:
		case kProgress:
		case kParseComplete:
		case kParseFail:
		case kDownloadFail:
		{
			fMainWindow->PostMessage(msg);
			break;
		}
		case kClearQueue:
		case kEnqueueFeed:
		case kUpdateSubscribed:
		{
			BMessenger toDaemon("application/x-vnd.PoggerDaemon");
			toDaemon.SendMessage(msg);
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

		if (BString(type) == "application/x-feed-entry"
			|| BString(type) == "text/x-feed-entry") {
			_OpenEntryFile(&msg);
		}
		else if (BString(type) == "application/x-feed-source")
			_OpenSourceFile(&msg);
	}
}


void
App::_OpenEntryFile(BMessage* refMessage)
{
	BRoster roster;
	roster.Launch("application/x-vnd.PoggerDaemon", refMessage);
	if (IsLaunching())
		Quit();
}


void
App::_OpenSourceFile(BMessage* refMessage)
{
	entry_ref entryRef;
	refMessage->FindRef("refs", &entryRef);
	BPath path(&entryRef);

	FeedEditWindow* window = new FeedEditWindow(path.Path(),
		SourceManager::GetSourceOfType("RssAtom")->fConfigPath);
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


