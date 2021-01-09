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
#include "Invocation.h"
#include "MainWindow.h"
#include "Mimetypes.h"
#include "RssFeed.h"
#include "Util.h"


int
main(int argc, char** argv)
{
	App* app = new App();
	usageMsg.ReplaceAll("%app%", "Pogger");
	feedMimeType();

	app->cfg = new Config;
	app->cfg->Load();

	if (argc == 1)
		app->Run();
	else
		cliStart(argc, argv);

		
	if ( app->cfg->will_save == true )
		app->cfg->Save();

	return 0;
}


void
cliStart(int argc, char** argv)
{
	invocation(argc, argv);
	BList targetFeeds = ((App*)be_app)->cfg->targetFeeds;

	for (int i = 0; i < targetFeeds.CountItems(); i++) {
		Feed* newFeed = new Feed(((BString*)targetFeeds.ItemAt(i))->String());
		BMessage* enqueue = new BMessage(kEnqueueFeed);

		enqueue->AddData("feeds", B_RAW_TYPE, newFeed, sizeof(Feed));

		((App*)be_app)->MessageReceived(enqueue);
	}
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


const char* configPath = "/boot/home/config/settings/Pogger/";
BString usageMsg =
	"Usage: %app% [-hvDus] [-tT datetime] [-cCO path] \n"
	"       %app% [-hvs] [-tTcCO] ( <text/xml file> |  <META:url file> | <url> )\n"
	"\n"
	"%app%, a RSS and Atom feed parser/daemon.\n"
	"\n"
	"Options:\n"
	"  -h, --help       - Print this usage info.\n"
	"  -v, --verbose    - Print verbose (debug) info.\n"
	"  -O, --output     - Output dir for item files. (Default: ~/feeds/)\n"
	"  -t, --before     - Only return items published before this datetime.\n"
	"  -T, --after      - Only return items published after this datetime.\n"
	"  -c, --config     - Path to config dir. (Default: ~/config/settings/Pogger/)\n"
	"  -C, --cache      - Path to cache.  (Default: ~/config/cache/Pogger/)\n"
	"  -s, --save       - Save the args of `-m`, `-C`, and `-O` to config.\n"
	"  -u, --update     - Update all feeds, but don't start daemon.\n"
	"  -D, --foreground - Run in the foreground, do not daemonize.\n"
	"                     `-u` and `-D` only apply when running without file/url arg.\n"
	"\n"
	"When invoked without a file or URL, will search for any new feed items\n"
	"published since last check by by any 'feed file' placed in the config\n"
	"directory (default: ~/config/settings/Rifen/feeds/) and create their\n"
	"corresponding files.\n"
	"\n"
	"When invoked with a file or URL, will create files from items contained\n"
	"within the given file or URL.\n"
	"\n"
	"'Feed files' are files with a 'META:url' attribute containing the\n"
	"corresponding URL to the feed's XML.\n"
	"\n"
	"Both -t and -T use the ISO 8601 format for specifying datetimes:\n"
	"      YYYY-MM-DDTHH:MM:SS - 2020-01-01T07:07:07\n"
	"\n"
	"NOTE: This message doesn't reflect reality. This is more of a spec of\n"
	"      what I hope this program will be. As of now, running %app%\n"
	"      without a file/url free-argument is invalid, as the daemon\n"
	"      isn't implemented at all. As such, -D -u and -C are non-functional.\n"
	"      But it sure can turn an XML feed into files! Lol.\n"
;
