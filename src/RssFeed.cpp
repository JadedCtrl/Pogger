#include <tinyxml2.h>
#include "Entry.h"
#include "Config.h"
#include "Util.h"
#include "RssFeed.h"

RssFeed::RssFeed ( )
{
	title = BString("Untitled Feed");
	description = BString("");
	homeUrl = BString("");
	xmlUrl = BString("");
}

RssFeed::RssFeed ( Feed* feed ) : RssFeed::RssFeed()
{	filePath = feed->filePath; }
RssFeed::RssFeed ( Feed* feed, Config* cfg ) : RssFeed::RssFeed( feed )
{	outputDir = cfg->outDir; }
RssFeed::RssFeed ( Config* cfg ) : RssFeed::RssFeed( )
{	outputDir = cfg->outDir; }


// ----------------------------------------------------------------------------

void
RssFeed::Parse ( Config* cfg )
{
        tinyxml2::XMLDocument xml;
	entries = BList();

	Feed::Parse( cfg );

        xml.LoadFile( filePath.String() );
	tinyxml2::XMLElement* xchan = xml.FirstChildElement("rss")->FirstChildElement("channel");

	RootParse( cfg, xchan );
	ParseEntries( cfg, xchan );

	time_t tt_lastDate = lastDate.Time_t();
	BFile* feedFile = new BFile( filePath.String(), B_READ_ONLY );
	feedFile->WriteAttr( "LastDate", B_TIME_TYPE, 0, &tt_lastDate, sizeof(time_t) );
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
	newEntry->SetFeedTitle( title );

	if ( lastDate == NULL || lastDate < newEntry->date )
		lastDate = newEntry->date;

	AddEntry( cfg, newEntry );
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
