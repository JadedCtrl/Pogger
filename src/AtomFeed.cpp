#include <tinyxml2.h>
#include "Entry.h"
#include "Config.h"
#include "Util.h"
#include "AtomFeed.h"

AtomFeed::AtomFeed ( )
{
	title = BString("Untitled Feed");
	description = BString("");
	homeUrl = BString("");
	xmlUrl = BString("");
	filePath = BString("");
}

AtomFeed::AtomFeed ( Feed* feed ) : AtomFeed::AtomFeed()
{	filePath = feed->filePath; }
AtomFeed::AtomFeed ( Feed* feed, Config* cfg ) : AtomFeed::AtomFeed( feed )
{	outputDir = cfg->outDir; }
AtomFeed::AtomFeed ( Config* cfg ) : AtomFeed::AtomFeed( )
{	outputDir = cfg->outDir; }

// ----------------------------------------------------------------------------

void
AtomFeed::Parse ( Config* cfg )
{
	BFile* feedFile = new BFile( filePath.String(), B_READ_ONLY );
	entries = BList();
        tinyxml2::XMLDocument xml;
        xml.LoadFile( filePath.String() );
	time_t tt_lastDate = 0;
	BDateTime attrLastDate = BDateTime();

	feedFile->ReadAttr( "LastDate", B_TIME_TYPE, 0, &tt_lastDate, sizeof(time_t) );
	if ( tt_lastDate > 0 && cfg->updateFeeds == true ) {
		attrLastDate.SetTime_t( tt_lastDate );
		minDate = attrLastDate;
	}

	tinyxml2::XMLElement* xfeed = xml.FirstChildElement("feed");

	RootParse( cfg, xfeed );
	ParseEntries( cfg, xfeed );

	tt_lastDate = lastDate.Time_t();
	feedFile->WriteAttr( "LastDate", B_TIME_TYPE, 0, &tt_lastDate, sizeof(time_t) );
}

void
AtomFeed::RootParse( Config* cfg, tinyxml2::XMLElement* xfeed )
{
	tinyxml2::XMLElement* xauthor = xfeed->FirstChildElement("author");
	tinyxml2::XMLElement* xentry = xfeed->FirstChildElement("entry");
	tinyxml2::XMLElement* xlink = xfeed->FirstChildElement("link");
	tinyxml2::XMLElement* xauthlink = xauthor->FirstChildElement("link");
	
	bool set = false;

	SetTitle( xfeed->FirstChildElement("title") );
	SetDesc(  xfeed->FirstChildElement("description") );

	set = SetDate( xfeed->FirstChildElement("updated") );
	if ( !set )             set = SetDate( xfeed->FirstChildElement("published") );
	if ( !set && xentry )   set = SetDate( xentry->FirstChildElement("updated") );
	if ( !set && xentry )   set = SetDate( xentry->FirstChildElement("published") );

	set = SetHomeUrl( xlink->Attribute( "href" ) );
	if ( !set && xauthor )    set = SetHomeUrl( xauthor->FirstChildElement("uri") );
	if ( !set && xauthlink )  set = SetHomeUrl( xauthlink->Attribute( "href" ) );

	if ( cfg->verbose )
		printf("Channel '%s' at '%s':\n", title.String(), homeUrl.String());
}

void
AtomFeed::EntryParse ( Config* cfg, tinyxml2::XMLElement* xentry )
{
	Entry* newEntry= (Entry*)malloc( sizeof(Entry) );
	newEntry = new Entry( outputDir );

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

	if ( lastDate == NULL || lastDate < newEntry->date )
		lastDate = newEntry->date;

	if ( xcontent ) {
		xcontent->Accept( &xprinter );
		newEntry->SetContent( xprinter.CStr() );
	}

	AddEntry( cfg, newEntry );
}

void
AtomFeed::ParseEntries ( Config* cfg, tinyxml2::XMLElement* xfeed )
{
	tinyxml2::XMLElement* xentry;

	xentry = xfeed->FirstChildElement("entry");

	int entryCount = xmlCountSiblings( xentry, "entry" );
	entries = BList(entryCount);

	if ( cfg->verbose )
		printf("\t-%i entries-\n", entryCount);

	while ( xentry ) {
		EntryParse( cfg, xentry );
		xentry = xentry->NextSiblingElement("entry");
	}
}
