#include <raptor2/raptor2.h>
#include <StorageKit.h>
#include <String.h>
#include <getopt.h>
#include "Channel.h"
#include "Item.h"
#include "parsing.h"
#include "Config.h"
#include "Rifen.h"

int
usage ()
{
	fprintf(stderr, "%s", usageMsg.String());
	return 2;
}

bool
create_item ( void* item )
{
	printf("hi");
	Item* itemPtr  = (Item*)item;
	itemPtr->Filetize( false );
	return false;
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
				if ( optind < argc )
					cfg->targetFeed = BString(argv[optind]);
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
					fprintf( stderr, "Option `-%c` requires an argument.\n\n", optopt );
				else
					fprintf( stderr, "Unknown option `-%c`.\n\n", optopt );
				return 2;
		}
	}

	return 0;
}
	

int
main ( int argc, char** argv )
{
//
//
	Config* cfg = new Config;
	usageMsg.ReplaceAll("%app%", "Rifen");

	invocation( argc, argv, &cfg );

	Channel* chan = (Channel*)malloc( sizeof(Channel) );
	chan = new Channel(cfg->targetFeed, cfg->outDir);
	chan->Parse(cfg);

	BList items = chan->items;
	items.DoForEach(&create_item);
	return 0;
}
