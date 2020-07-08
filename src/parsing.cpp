#include <iostream>
#include <sstream>
#include <tinyxml2.h>
#include "Feed.h"
#include "Entry.h"
#include "Util.h"
#include "parsing.h"


// ============================================================================
// PARSERS
void
feedParser ( Feed** feedPtr, Config* cfg )
{
	Feed* feed = *(feedPtr);

	tinyxml2::XMLDocument xml;
	xml.LoadFile( feed->filePath.String() );

	if ( xml.FirstChildElement("rss") )
		rssParser( feedPtr, cfg, &xml );
	else if ( xml.FirstChildElement("feed") )
		atomParser( feedPtr, cfg, &xml );
}

// ----------------------------------------------------------------------------

void
rssParser ( Feed** feedPtr, Config* cfg, tinyxml2::XMLDocument* xml )
{
	Feed* chan = *(feedPtr);

	tinyxml2::XMLElement* xchan = xml->FirstChildElement("rss")->FirstChildElement("channel");

	rssRootParse( feedPtr, cfg, xchan );
	rssParseEntries( feedPtr, cfg, xchan );
}

void
rssRootParse( Feed** feedPtr, Config* cfg, tinyxml2::XMLElement* xchan )
{
	Feed* feed = *(feedPtr);

	feed->SetTitle   ( xchan->FirstChildElement("title") );
	feed->SetDesc    ( xchan->FirstChildElement("description") );
	feed->SetHomeUrl ( xchan->FirstChildElement("link") );
	feed->SetDate    ( xchan->FirstChildElement("lastBuildDate") );

	if ( cfg->verbose )
		printf("Channel '%s' at '%s':\n", feed->title.String(), feed->homeUrl.String());
}

void
rssEntryParse ( Feed** feedPtr, Config* cfg, tinyxml2::XMLElement* xitem )
{
	Feed* feed = *(feedPtr);

	Entry* newEntry = (Entry*)malloc( sizeof(Entry) );
	newEntry = new Entry( feed->outputDir );

	newEntry->SetTitle   ( xitem->FirstChildElement("title") );
	newEntry->SetDesc    ( xitem->FirstChildElement("description") );
	newEntry->SetDate    ( xitem->FirstChildElement("pubDate") );
	newEntry->SetPostUrl ( xitem->FirstChildElement("link") );
	newEntry->SetContent ( xitem->FirstChildElement("content:encoded") );

	if (cfg->verbose )
		printf("\t%s\n", newEntry->title.String());

	if ( withinDateRange( cfg->minDate, newEntry->date, cfg->maxDate ) )
		feed->entries.AddItem( newEntry );
}

void
rssParseEntries ( Feed** feedPtr, Config* cfg, tinyxml2::XMLElement* xchan )
{
	Feed* feed = *(feedPtr);
	tinyxml2::XMLElement* xitem;

	xitem = xchan->FirstChildElement("item");

	int entryCount = xmlCountSiblings( xitem, "item" );
	feed->entries = BList(entryCount);

	if ( cfg->verbose )
		printf("\t-%i entries-\n", entryCount);

	while ( xitem ) {
		rssEntryParse( feedPtr, cfg, xitem );
		xitem = xitem->NextSiblingElement("item");
	}
}

// ----------------------------------------------------------------------------

void
atomParser ( Feed** feedPtr, Config* cfg, tinyxml2::XMLDocument* xml )
{
	Feed* feed = *(feedPtr);

	tinyxml2::XMLElement* xfeed = xml->FirstChildElement("feed");

	atomRootParse( feedPtr, cfg, xfeed );
	atomParseEntries( feedPtr, cfg, xfeed );
}

void
atomRootParse( Feed** feedPtr, Config* cfg, tinyxml2::XMLElement* xfeed )
{
	Feed* feed = *(feedPtr);

	tinyxml2::XMLElement* xauthor = xfeed->FirstChildElement("author");
	tinyxml2::XMLElement* xentry = xfeed->FirstChildElement("entry");
	tinyxml2::XMLElement* xlink = xfeed->FirstChildElement("link");
	tinyxml2::XMLElement* xauthlink = xauthor->FirstChildElement("link");
	
	bool set = false;

	feed->SetTitle( xfeed->FirstChildElement("title") );
	feed->SetDesc( xfeed->FirstChildElement("description") );

	set = feed->SetDate( xfeed->FirstChildElement("updated") );
	if ( !set )             set = feed->SetDate( xfeed->FirstChildElement("published") );
	if ( !set && xentry )   set = feed->SetDate( xentry->FirstChildElement("updated") );
	if ( !set && xentry )   set = feed->SetDate( xentry->FirstChildElement("published") );

	set = feed->SetHomeUrl( xlink->Attribute( "href" ) );
	if ( !set && xauthor )    set = feed->SetHomeUrl( xauthor->FirstChildElement("uri") );
	if ( !set && xauthlink )  set = feed->SetHomeUrl( xauthlink->Attribute( "href" ) );

	if ( cfg->verbose )
		printf("Channel '%s' at '%s':\n", feed->title.String(), feed->homeUrl.String());
}

void
atomEntryParse ( Feed** feedPtr, Config* cfg, tinyxml2::XMLElement* xentry )
{
	Feed* feed = *(feedPtr);
	Entry* newEntry= (Entry*)malloc( sizeof(Entry) );
	newEntry = new Entry( feed->outputDir );

	tinyxml2::XMLElement* xcontent = xentry->FirstChildElement("content");
	tinyxml2::XMLElement* xmedia   = xentry->FirstChildElement("media:group");
	tinyxml2::XMLPrinter  xprinter;

	newEntry->SetTitle( xentry->FirstChildElement("title") );
	newEntry->SetPostUrl( xentry->FirstChildElement("link")->Attribute("href") );

	bool set = false;
	set = newEntry->SetDesc( xentry->FirstChildElement("summary") );
	if ( !set )           set = newEntry->SetDesc( xentry->FirstChildElement("description")); 
	if ( !set && xmedia ) set = newEntry->SetDesc( xmedia->FirstChildElement("media:description")); 

	set = newEntry->SetDate( xentry->FirstChildElement("updated") );
	if ( !set )   set = newEntry->SetDate( xentry->FirstChildElement("published") );

	if ( xcontent ) {
		xcontent->Accept( &xprinter );
		newEntry->SetContent( xprinter.CStr() );
	}

	if ( cfg->verbose )
		printf("\t%s\n", newEntry->title.String());

	if ( withinDateRange( cfg->minDate, newEntry->date, cfg->maxDate ) )
		feed->entries.AddItem( newEntry );
}

void
atomParseEntries ( Feed** feedPtr, Config* cfg, tinyxml2::XMLElement* xfeed )
{
	Feed* feed = *(feedPtr);
	tinyxml2::XMLElement* xentry;

	xentry = xfeed->FirstChildElement("entry");

	int entryCount = xmlCountSiblings( xentry, "entry" );
	feed->entries = BList(entryCount);

	if ( cfg->verbose )
		printf("\t-%i entries-\n", entryCount);

	while ( xentry ) {
		atomEntryParse( feedPtr, cfg, xentry );
		xentry = xentry->NextSiblingElement("entry");
	}
}

// ----------------------------------------------------------------------------

int
xmlCountSiblings ( tinyxml2::XMLElement* xsibling, const char* sibling_name )
{
	int count = 0;
	while ( xsibling ) {
		count++;
		xsibling = xsibling->NextSiblingElement(sibling_name);
	}
	return count;
}
