#include <tinyxml2.h>
#include "Entry.h"
#include "App.h"
#include "Config.h"
#include "Util.h"
#include "RssFeed.h"

RssFeed::RssFeed ( )
{
	title = BString("Untitled Feed");
	description = BString("");
	homeUrl = BString("");
	xmlUrl = BString("");
	outputDir = ((App*)be_app)->cfg->outDir;
}

RssFeed::RssFeed ( Feed* feed ) : RssFeed::RssFeed()
{	SetCachePath( feed->GetCachePath() ); }

// ----------------------------------------------------------------------------

void
RssFeed::Parse ( )
{
        tinyxml2::XMLDocument xml;
	entries = BList();

	Feed::Parse();

        xml.LoadFile( GetCachePath().String() );
	tinyxml2::XMLElement* xchan = xml.FirstChildElement("rss")->FirstChildElement("channel");

	RootParse( xchan );
	ParseEntries( xchan );

	time_t tt_lastDate = lastDate.Time_t();
	BFile* feedFile = new BFile( GetCachePath().String(), B_READ_ONLY );
	feedFile->WriteAttr( "LastDate", B_TIME_TYPE, 0, &tt_lastDate, sizeof(time_t) );
}

// -------------------------------------

void
RssFeed::RootParse ( tinyxml2::XMLElement* xchan )
{
	SetTitle   ( xchan->FirstChildElement("title") );
	SetDesc    ( xchan->FirstChildElement("description") );
	SetHomeUrl ( xchan->FirstChildElement("link") );
	SetDate    ( xchan->FirstChildElement("lastBuildDate") );

	if ( ((App*)be_app)->cfg->verbose )
		printf("Channel '%s' at '%s':\n", title.String(), homeUrl.String());
}

void
RssFeed::EntryParse ( tinyxml2::XMLElement* xitem )
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

	AddEntry( newEntry );
}

void
RssFeed::ParseEntries ( tinyxml2::XMLElement* xchan )
{
	tinyxml2::XMLElement* xitem;

	xitem = xchan->FirstChildElement("item");

	int entryCount = xmlCountSiblings( xitem, "item" );
	entries = BList(entryCount);

	if ( ((App*)be_app)->cfg->verbose )
		printf("\t-%i entries-\n", entryCount);

	while ( xitem ) {
		EntryParse( xitem );
		xitem = xitem->NextSiblingElement("item");
	}
}
