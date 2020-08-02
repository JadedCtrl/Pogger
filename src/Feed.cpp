#include <tinyxml2.h>
#include "Entry.h"
#include "Config.h"
#include "Util.h"
#include "Feed.h"

Feed::Feed ( BString path, Config* cfg )
{
	title = BString( "Untitled Feed" );
	description = BString( "Nondescript, N/A." );
	homeUrl = BString("");
	xmlUrl = BString("");
	updated = true;
	filePath = GetCachePath( path, cfg );
}

Feed::Feed ( ) {
	title = BString("");
	description = BString("");
	homeUrl = BString("");
	xmlUrl = BString("");
}

// ----------------------------------------------------------------------------

BString
Feed::GetCachePath ( BString givenPath, Config* cfg )
{
	BUrl givenUrl = BUrl( givenPath );
	BString protocol = givenUrl.Protocol().String();

	if ( protocol == NULL && givenUrl.UrlString() != NULL )
		return givenPath;
	if ( protocol != BString("http")  &&  protocol != BString("https") )
		return NULL;

	return FetchRemoteFeed( givenPath, cfg );
}

BString
Feed::FetchRemoteFeed ( BString givenPath, Config* cfg )
{
	BUrl givenUrl = BUrl( givenPath );
	BString* newHash = new BString();
	char oldHash[41];

	BString splitName = givenUrl.Host( );
	splitName.Append( givenUrl.Path() );
	splitName.ReplaceAll("/", "_");

	BString filename = cfg->cacheDir;
	filename.Append(splitName);
	BFile* cacheFile = new BFile( filename, B_READ_WRITE | B_CREATE_FILE );

	cacheFile->ReadAttr( "FeedSum", B_STRING_TYPE, 0,
			     oldHash, 41 );

	if ( cfg->verbose )
		printf( "Saving %s...\n", givenPath.String() );

	webFetch( givenUrl, cacheFile, newHash );

	cacheFile->WriteAttr( "FeedSum", B_STRING_TYPE, 0,
			      newHash->String(), newHash->CountChars() );

	if ( *(newHash) == BString(oldHash) )
		updated = false;

	return filename;
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

