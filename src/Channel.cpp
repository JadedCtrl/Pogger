#include <tinyxml2.h>
#include "Util.h"
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
//	lastDate = NULL;
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

bool Channel::SetTitle ( const char* titleStr ) {
	if ( titleStr != NULL )	title = BString( titleStr );
	else return false;
	return true;
}
bool Channel::SetTitle ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	return SetTitle( elem->GetText() );
	else return false;
}

bool Channel::SetDesc ( const char* descStr ) {
	if ( descStr != NULL )	description = BString( descStr );
	else return false;
	return true;
}
bool Channel::SetDesc ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	return SetDesc( elem->GetText() );
	else return false;
}

bool Channel::SetHomePage ( const char* homepageStr ) {
	if ( homepageStr != NULL ) homePage = BString( homepageStr );
	else return false;
	return true;
}
bool Channel::SetHomePage ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	return SetHomePage( elem->GetText() );
	else return false;
}

bool Channel::SetLastDate ( const char* dateCStr ) {
	if ( dateCStr == NULL )
		return false;
	BDateTime date = feedDateToBDate( dateCStr );
	if ( date == NULL )
		return false;
	lastDate = date;
	return true;
}
bool Channel::SetLastDate ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	return SetLastDate( elem->GetText() );

	else return false;
}
