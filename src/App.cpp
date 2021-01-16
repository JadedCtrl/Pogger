/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "App.h"

#include <MessageRunner.h>
#include <StorageKit.h>
#include <String.h>

#include <getopt.h>

#include "AtomFeed.h"
#include "Config.h"
#include "Entry.h"
#include "Feed.h"
#include "FeedController.h"
#include "MainWindow.h"
#include "Mimetypes.h"
#include "Notifier.h"
#include "RssFeed.h"
#include "Util.h"


int
main(int argc, char** argv)
{
	installMimeTypes();

	App* app = new App();
	app->Run();
	app->cfg->Save();
	return 0;
}


App::App() : BApplication("application/x-vnd.Pogger")
{
	cfg = new Config;
	cfg->Load();

	fMainWindow = new MainWindow();
	fNotifier = new Notifier();
	fFeedController = new FeedController();
	fMainWindow->Show();

	BMessage* updateMessage = new BMessage(kUpdateSubscribed);
//	MessageReceived(updateMessage);
	fUpdateRunner = new BMessageRunner(this, updateMessage,
		cfg->updateInterval);
}


void
App::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case kEnqueueFeed:
		case kUpdateSubscribed:
		case kDownloadComplete:
		{
			fFeedController->MessageReceived(msg);
			break;
		}
		case kClearQueue:
		{
			break;
		}
		case kParseComplete:
		{
			fNotifier->MessageReceived(msg);
			break;
		}
		case kParseFail:
		case kDownloadFail:
		{
			fNotifier->MessageReceived(msg);
			fFeedController->MessageReceived(msg);
			break;
		}
		default:
		{
//			BApplication::MessageReceived(msg);
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


