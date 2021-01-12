/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "App.h"

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
#include "RssFeed.h"
#include "Util.h"


int
main(int argc, char** argv)
{
	App* app = new App();
	installMimeTypes();

	app->cfg = new Config;
	app->cfg->Load();

	app->Run();
		
	if ( app->cfg->will_save == true )
		app->cfg->Save();

	return 0;
}


App::App() : BApplication("application/x-vnd.Pogger")
{
	fMainWindow = new MainWindow();
	fFeedController = new FeedController();
	fMainWindow->Show();
}


void
App::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case kEnqueueFeed:
		{
			fFeedController->MessageReceived(msg);
			break;
		}
		case kUpdateSubscribed:
		{
			fFeedController->MessageReceived(msg);
			break;
		}
		case kClearQueue:
		{
			break;
		}
		case kQueueProgress:
		{
			fMainWindow->MessageReceived(msg);
		}
		case kDownloadComplete:
		{
			fFeedController->MessageReceived(msg);
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


