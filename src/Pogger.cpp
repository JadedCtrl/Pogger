#include <StorageKit.h>
#include <String.h>
#include <getopt.h>
#include "Channel.h"
#include "Item.h"
#include "parsing.h"
#include "Config.h"
#include "Pogger.h"

#include <StorageKit.h>

Config* main_cfg;

int
main ( int argc, char** argv )
{
	main_cfg = new Config;
	usageMsg.ReplaceAll("%app%", "Pogger");

	invocation( argc, argv, &main_cfg );
	main_cfg->targetFeeds.DoForEach(&processFeed);

	return 0;
}

// ----------------------------------------------------------------------------

int
usage ()
{
	fprintf(stderr, "%s", usageMsg.String());
	return 2;
}

int
invocation ( int argc, char** argv, Config** cfgPtr )
{
	Config* cfg = *(cfgPtr);
	bool suicide = false;
	static struct option sLongOptions[] = {
		{ "help", no_argument, 0, 'h' },
		{ "verbose", no_argument, 0, 'v' },
		{ "output", required_argument, 0, 'O' },
		{ "mimetype", required_argument, 0, 'm' },
		{ "foreground", no_argument, 0, 'D' },
		{ 0, 0, 0, 0 }
	};

	while (true) {
		opterr = 0;
		int c = getopt_long(argc, argv, "+hvDm:O:", sLongOptions, NULL);

		switch (c) {
			case -1:
				freeargInvocation( argc, argv, optind, cfgPtr );
				return 0;
			case 'h':
				return usage();
			case 'm':
				cfg->mimetype = BString(optarg);
				break;
			case 'O':
				cfg->outDir = BString(optarg);
				break;
			case 'v':
				cfg->verbose = true;
				break;
			case 'D':
				cfg->daemon = false;
				break;
			case '?':
				if ( optopt == 'O' || optopt == 'm' )
					fprintf( stderr, "Option `-%c` requires an argument.\n\n",
					         optopt );
				else
					fprintf( stderr, "Unknown option `-%c`.\n\n", optopt );
				return 2;
		}
	}
}

// ―――――――――――――――――

void
freeargInvocation ( int argc, char** argv, int optind, Config** cfgPtr )
{
	Config* cfg = *(cfgPtr);
	if ( optind < argc ) {
		int freeargc = argc - optind;
		cfg->targetFeeds =  BList( freeargc );

		for ( int i = 0; i < freeargc; i++ ) {
			BString* newFeed = new BString( argv[optind + i] );
			cfg->targetFeeds.AddItem( newFeed );
		}
	}
}

// ----------------------------------------------------------------------------

bool
processItem ( void* item )
{
	Item* itemPtr  = (Item*)item;
	itemPtr->Filetize( main_cfg, false );
	return false;
}

bool
processFeed ( void* feed )
{
	BString* feedStr = (BString*)feed;
				
	Channel* chan = (Channel*)malloc( sizeof(Channel) );
	chan = new Channel(*(feedStr), main_cfg->outDir);
	chan->Parse(main_cfg);
	BList items = chan->items;
	items.DoForEach(&processItem);
	free(chan);

	return false;
}
