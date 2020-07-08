#include <tinyxml2.h>
#include "Entry.h"
#include "Config.h"
#include "parsing.h"
#include "Util.h"
#include "Feed.h"

Feed::Feed ( BString path, BString outputPath )
{
	title = BString("Untitled Feed");
	description = BString("Nondescript, N/A.");
	homeUrl = BString("");
	xmlUrl = BString("");
	filePath = path;
//	lastDate = NULL;
	outputDir = outputPath;
}

void
Feed::Parse ( Config* cfg )
{
	entries = BList();
	Feed* feed = this;
	feedParser(&feed, cfg);
}

bool Feed::SetTitle ( const char* titleStr ) {
	if ( titleStr != NULL )	title = BString( titleStr );
	else return false;
	return true;
}
bool Feed::SetTitle ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	return SetTitle( elem->GetText() );
	else return false;
}

bool Feed::SetDesc ( const char* descStr ) {
	if ( descStr != NULL )	description = BString( descStr );
	else return false;
	return true;
}
bool Feed::SetDesc ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	return SetDesc( elem->GetText() );
	else return false;
}

bool Feed::SetHomeUrl ( const char* homepageStr ) {
	if ( homepageStr != NULL ) homeUrl = BString( homepageStr );
	else return false;
	return true;
}
bool Feed::SetHomeUrl ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	return SetHomeUrl( elem->GetText() );
	else return false;
}

bool Feed::SetDate ( const char* dateCStr ) {
	if ( dateCStr == NULL )
		return false;
	BDateTime newDate = feedDateToBDate( dateCStr );
	if ( newDate == NULL )
		return false;
	date = newDate;
	return true;
}
bool Feed::SetDate ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	return SetDate( elem->GetText() );

	else return false;
}
