#include <tinyxml2.h>
#include "Entry.h"
#include "Config.h"
#include "Util.h"
#include "AtomFeed.h"

AtomFeed::AtomFeed ( BString path, BString outputPath )
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
AtomFeed::Parse ( Config* cfg )
{
	entries = BList();
        tinyxml2::XMLDocument xml;
        xml.LoadFile( filePath.String() );

	tinyxml2::XMLElement* xfeed = xml.FirstChildElement("feed");

	RootParse( cfg, xfeed );
	ParseEntries( cfg, xfeed );
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

	if ( xcontent ) {
		xcontent->Accept( &xprinter );
		newEntry->SetContent( xprinter.CStr() );
	}

	if ( cfg->verbose )
		printf("\t%s\n", newEntry->title.String());

	if ( withinDateRange( cfg->minDate, newEntry->date, cfg->maxDate ) )
		entries.AddItem( newEntry );
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
