#include <raptor2/raptor2.h>
#include <StorageKit.h>
#include "Channel.h"
#include "Item.h"
#include "parsing.h" //


bool
create_item ( void* item )
{
	Item* itemPtr  = (Item*)item;
	itemPtr->Filetize( false );
	return false;
}

int
main ( int argc, char** argv )
{
	BString outputDir("/boot/home/feeds/");
	Channel* chan = (Channel*)malloc( sizeof(Channel) );

	chan = new Channel(argv[1], outputDir);
	chan->Parse();

	BList items = chan->items;
	items.DoForEach(&create_item);
	return 0;
}
