#include <tinyxml2.h>
#include "Entry.h"
#include "Config.h"
#include "Util.h"
#include "RssFeed.h"

RssFeed::RssFeed ( BString path, Config* cfg )
{
	title = BString("Untitled Feed");
	description = BString("");
	homeUrl = BString("");
	xmlUrl = BString("");
	filePath = path;
	outputDir = cfg->outDir;
}

// ----------------------------------------------------------------------------

void
RssFeed::Parse ( Config* cfg )
{
	BFile* feedFile = new BFile( filePath.String(), B_READ_ONLY );
        tinyxml2::XMLDocument xml;
	entries = BList();
	time_t tt_lastDate = 0;
	BDateTime attrLastDate = BDateTime();


	feedFile->ReadAttr( "LastDate", B_TIME_TYPE, 0, &tt_lastDate, sizeof(time_t) );
	if ( tt_lastDate > 0 && cfg->minDate == NULL && cfg->updateFeeds == true ) {
		attrLastDate.SetTime_t( tt_lastDate );
		cfg->minDate = attrLastDate;
	}

        xml.LoadFile( filePath.String() );
	tinyxml2::XMLElement* xchan = xml.FirstChildElement("rss")->FirstChildElement("channel");

	RootParse( cfg, xchan );
	ParseEntries( cfg, xchan );

	tt_lastDate = lastDate.Time_t();
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
