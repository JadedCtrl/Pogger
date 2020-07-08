#include <tinyxml2.h>
#include "Entry.h"
#include "Config.h"
#include "Util.h"
#include "Feed.h"

Feed::Feed ( BString path )
{
	title = BString("Untitled Feed");
	description = BString("Nondescript, N/A.");
	homeUrl = BString("");
	xmlUrl = BString("");
	filePath = path;
}

Feed::Feed () {
	title = BString("");
	description = BString("");
	homeUrl = BString("");
	xmlUrl = BString("");
}

// ----------------------------------------------------------------------------

bool
Feed::IsRss ( )
{
	tinyxml2::XMLDocument xml;
	xml.LoadFile( filePath.String() );

	if ( xml.FirstChildElement("rss") )
		return true;
	return false;
}

bool
Feed::IsAtom ( )
{
	tinyxml2::XMLDocument xml;
	xml.LoadFile( filePath.String() );

	if ( xml.FirstChildElement("feed") )
		return true;
	return false;
}

// ----------------------------------------------------------------------------

int
Feed::xmlCountSiblings ( tinyxml2::XMLElement* xsibling, const char* sibling_name )
{
	int count = 0;
	while ( xsibling ) {
		count++;
		xsibling = xsibling->NextSiblingElement(sibling_name);
	}
	return count;
}

// ----------------------------------------------------------------------------

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

