#include <tinyxml2.h>
#include "Entry.h"
#include "Config.h"
#include "Util.h"
#include "RssFeed.h"

RssFeed::RssFeed ( BString path, BString outputPath )
{
	title = BString("Untitled Feed");
	description = BString("");
	homeUrl = BString("");
	xmlUrl = BString("");
	filePath = path;
	outputDir = outputPath;
}

// ----------------------------------------------------------------------------

void
RssFeed::Parse ( Config* cfg )
{
	entries = BList();
        tinyxml2::XMLDocument xml;
        xml.LoadFile( filePath.String() );

	tinyxml2::XMLElement* xchan = xml.FirstChildElement("rss")->FirstChildElement("channel");

	RootParse( cfg, xchan );
	ParseEntries( cfg, xchan );
}

// -------------------------------------

void
RssFeed::RootParse ( Config* cfg, tinyxml2::XMLElement* xchan )
{
	SetTitle   ( xchan->FirstChildElement("title") );
	SetDesc    ( xchan->FirstChildElement("description") );
	SetHomeUrl ( xchan->FirstChildElement("link") );
	SetDate    ( xchan->FirstChildElement("lastBuildDate") );

	if ( cfg->verbose )
		printf("Channel '%s' at '%s':\n", title.String(), homeUrl.String());
}

void
RssFeed::EntryParse ( Config* cfg, tinyxml2::XMLElement* xitem )
{
	Entry* newEntry = (Entry*)malloc( sizeof(Entry) );
	newEntry = new Entry( outputDir );

	newEntry->SetTitle   ( xitem->FirstChildElement("title") );
	newEntry->SetDesc    ( xitem->FirstChildElement("description") );
	newEntry->SetDate    ( xitem->FirstChildElement("pubDate") );
	newEntry->SetPostUrl ( xitem->FirstChildElement("link") );
	newEntry->SetContent ( xitem->FirstChildElement("content:encoded") );

	if (cfg->verbose )
		printf("\t%s\n", newEntry->title.String());

	if ( withinDateRange( cfg->minDate, newEntry->date, cfg->maxDate ) )
		entries.AddItem( newEntry );
}

void
RssFeed::ParseEntries ( Config* cfg, tinyxml2::XMLElement* xchan )
{
	tinyxml2::XMLElement* xitem;

	xitem = xchan->FirstChildElement("item");

	int entryCount = xmlCountSiblings( xitem, "item" );
	entries = BList(entryCount);

	if ( cfg->verbose )
		printf("\t-%i entries-\n", entryCount);

	while ( xitem ) {
		EntryParse( cfg, xitem );
		xitem = xitem->NextSiblingElement("item");
	}
}
