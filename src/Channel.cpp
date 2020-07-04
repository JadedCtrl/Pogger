#include <cstdio>
#include <raptor2/raptor2.h>
#include "Channel.h"
#include "Item.h"
#include "Config.h"
#include "parsing.h"

Channel::Channel ( BString path, BString outputPath )
{
	title = BString("Untitled Feed");
	description = BString("Nondescript, N/A.");
	homePage = BString("");
	xmlUrl = BString("");
	filePath = path;
	topLevelSubject = "";
	lastSubject = "";
	outputDir = outputPath;
}

void
Channel::Parse ( Config* cfg )
{
	int itemCount = countItemParser( filePath.String() );
	items = BList(itemCount);
	Channel* chan = this;
	feedParser(&chan);
}
