#include <raptor2/raptor2.h>
#include "Parse.h"
#include <StorageKit.h>

bool
create_item ( void* item )
{
	Item* itemPtr = (Item*)item;

	BDirectory* dir    = new BDirectory("./test/test/");
	BFile*      file   = new BFile(itemPtr->title.String(), B_READ_WRITE);

	dir->CreateFile(itemPtr->title.String(), file);

	file->WriteAttr("title",B_STRING_TYPE,0,
			itemPtr->title.String(),itemPtr->title.CountChars());
	file->WriteAttr("description",B_STRING_TYPE,0,
			itemPtr->description.String(),itemPtr->description.CountChars());

//	const char* buf;
//	buf = itemPtr->title.String();
	file->Write(itemPtr->title.String(), itemPtr->title.CountChars());

	return false;
}

int
main ( int argc, char** argv )
{
	Channel* chan = parseRssFile( argv[1] );
	BList items = chan->items;
	printf("%s\n", chan->title.String());
	items.DoForEach(&create_item);

	
	return 0;
}
