#include <cstdio>
#include <tinyxml2.h>
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
	lastDate = BString("");
	topLevelSubject = "";
	lastSubject = "";
	outputDir = outputPath;
}

void
Channel::Parse ( Config* cfg )
{
	items = BList();
	Channel* chan = this;
	feedParser(&chan, cfg);
}

void Channel::SetTitle ( const char* titleStr ) {
	if ( titleStr != NULL )	title = BString( titleStr );
}
void Channel::SetTitle ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	SetTitle( elem->GetText() );
}

void Channel::SetDesc ( const char* descStr ) {
	if ( descStr != NULL )	description = BString( descStr );
}
void Channel::SetDesc ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	SetDesc( elem->GetText() );
}

void Channel::SetHomePage ( const char* homepageStr ) {
	if ( homepageStr != NULL )
		homePage = BString( homepageStr );
}
void Channel::SetHomePage ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	SetHomePage( elem->GetText() );
}

void Channel::SetLastDate ( const char* dateStr ) {
	if ( dateStr != NULL )	lastDate = BString( dateStr );
}
void Channel::SetLastDate ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	SetLastDate( elem->GetText() );
}
