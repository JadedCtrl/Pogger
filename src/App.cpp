/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "App.h"

#include <MessageRunner.h>
#include <StorageKit.h>
#include <String.h>

#include <iostream>

#include "AtomFeed.h"
#include "Entry.h"
#include "Feed.h"
#include "FeedController.h"
#include "FeedsView.h"
#include "MainWindow.h"
#include "Mimetypes.h"
#include "Notifier.h"
#include "Preferences.h"
#include "RssFeed.h"
#include "Util.h"


int
main(int argc, char** argv)
{
	installMimeTypes();

	App* app = new App();
	app->Run();
	app->fPreferences->Save();
	return 0;
}


App::App() : BApplication("application/x-vnd.Pogger")
{
	fPreferences = new Preferences;
	fPreferences->Load();

	fMainWindow = new MainWindow();
	fMainWindow->Show();

	fNotifier = new Notifier();
	fFeedController = new FeedController();

	BMessage* updateMessage = new BMessage(kUpdateSubscribed);
	int64 interval = fPreferences->UpdateInterval();
	int32 count = -1;

	if (interval == -1)
		count = 0;
//	else
//		MessageReceived(updateMessage);

	fUpdateRunner = new BMessageRunner(this, updateMessage, interval, count);
}


void
App::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case kUpdateSubscribed:
		{
			fFeedController->MessageReceived(msg);
			break;
		}
		case kEnqueueFeed:
		case kDownloadComplete:
		{
			fNotifier->MessageReceived(msg);
			fFeedController->MessageReceived(msg);
			break;
		}
		case kFeedsEdited:
		case kProgress:
		{
			fMainWindow->MessageReceived(msg);
			break;
		}
		case kClearQueue:
		{
			break;
		}
		case kParseComplete:
		case kParseFail:
		case kDownloadFail:
		{
			fNotifier->MessageReceived(msg);
			break;
		}
		default:
		{
			BApplication::MessageReceived(msg);
			break;
		}
	}
}


void
App::ArgvReceived(int32 argc, char** argv)
{
	for (int i = 1; i < argc; i++) {
		if (BUrl(argv[i]).IsValid()) {
			Feed* newFeed = new Feed(BUrl(argv[i]));

			BMessage* enqueue = new BMessage(kEnqueueFeed);
			enqueue->AddData("feeds", B_RAW_TYPE, (void*)newFeed, sizeof(Feed));

			MessageReceived(enqueue);
		}
	}
}
		

const char* configPath = "/boot/home/config/settings/Pogger/";


