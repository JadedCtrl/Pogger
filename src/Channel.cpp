#include <cstdio>
#include <raptor2/raptor2.h>
#include "Channel.h"
#include "Item.h"
#include "parsing.h"

Channel::Channel ( BString path )
{
	title = BString("Untitled Feed");
	description = BString("Nondescript, N/A.");
	homePage = BString("");
	xmlUrl = BString("");
	filePath = path;
	topLevelSubject = "";
	lastSubject = "";
}

void
Channel::Parse ( )
{
	int itemCount = countItemParser( filePath.String() );
	items = BList(itemCount);
	Channel* chan = this;
	feedParser(&chan);
}
