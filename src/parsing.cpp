#include <iostream>
#include <sstream>
#include <tinyxml2.h>
#include "Channel.h"
#include "Item.h"
#include "Util.h"
#include "parsing.h"


// ============================================================================
// PARSERS
void
feedParser ( Channel** chanPtr, Config* cfg )
{
	Channel* chan = *(chanPtr);

	tinyxml2::XMLDocument xml;
	xml.LoadFile( chan->filePath.String() );

	if ( xml.FirstChildElement("rss") )
		rssParser( chanPtr, cfg, &xml );
	else if ( xml.FirstChildElement("feed") )
		atomParser( chanPtr, cfg, &xml );
}

// ----------------------------------------------------------------------------

void
rssParser ( Channel** chanPtr, Config* cfg, tinyxml2::XMLDocument* xml )
{
	Channel* chan = *(chanPtr);

	tinyxml2::XMLElement* xchan = xml->FirstChildElement("rss")->FirstChildElement("channel");

	rssRootParse( chanPtr, cfg, xchan );
	rssParseItems( chanPtr, cfg, xchan );
}

void
rssRootParse( Channel** chanPtr, Config* cfg, tinyxml2::XMLElement* xchan )
{
	Channel* chan = *(chanPtr);

	chan->SetTitle( xchan->FirstChildElement("title") );
	chan->SetDesc( xchan->FirstChildElement("description") );
	chan->SetHomePage( xchan->FirstChildElement("link") );
	chan->SetLastDate( xchan->FirstChildElement("lastBuildDate") );

	if ( cfg->verbose )
		printf("Channel '%s' at '%s':\n", chan->title.String(), chan->homePage.String());
}

void
rssItemParse ( Channel** chanPtr, Config* cfg, tinyxml2::XMLElement* xitem )
{
	Channel* chan = *(chanPtr);

	Item* newItem = (Item*)malloc( sizeof(Item) );
	newItem = new Item( chan->outputDir );

	newItem->SetTitle( xitem->FirstChildElement("title") );
	newItem->SetDesc( xitem->FirstChildElement("description") );
	newItem->SetPubDate( xitem->FirstChildElement("pubDate") );
	newItem->SetContent( xitem->FirstChildElement("content:encoded") );

	if (cfg->verbose )
		printf("\t%s\n", newItem->title.String());

	if ( withinDateRange( cfg->minDate, newItem->pubDate, cfg->maxDate ) )
		chan->items.AddItem( newItem );
}

void
rssParseItems ( Channel** chanPtr, Config* cfg, tinyxml2::XMLElement* xchan )
{
	Channel* chan = *(chanPtr);
	tinyxml2::XMLElement* xitem;

	xitem = xchan->FirstChildElement("item");

	int itemCount = xmlCountSiblings( xitem, "item" );
	chan->items = BList(itemCount);

	if ( cfg->verbose )
		printf("\t-%i items-\n", itemCount);

	while ( xitem ) {
		rssItemParse( chanPtr, cfg, xitem );
		xitem = xitem->NextSiblingElement("item");
	}
}

// ----------------------------------------------------------------------------

void
atomParser ( Channel** chanPtr, Config* cfg, tinyxml2::XMLDocument* xml )
{
	Channel* chan = *(chanPtr);

	tinyxml2::XMLElement* xfeed = xml->FirstChildElement("feed");

	atomRootParse( chanPtr, cfg, xfeed );
	atomParseEntries( chanPtr, cfg, xfeed );
}

void
atomRootParse( Channel** chanPtr, Config* cfg, tinyxml2::XMLElement* xfeed )
{
	Channel* chan = *(chanPtr);

	tinyxml2::XMLElement* xauthor = xfeed->FirstChildElement("author");
	tinyxml2::XMLElement* xentry = xfeed->FirstChildElement("entry");
	tinyxml2::XMLElement* xlink = xfeed->FirstChildElement("link");
	tinyxml2::XMLElement* xauthlink = xauthor->FirstChildElement("link");
	
	bool set = false;

	chan->SetTitle( xfeed->FirstChildElement("title") );
	chan->SetDesc( xfeed->FirstChildElement("description") );

	set = chan->SetLastDate( xfeed->FirstChildElement("updated") );
	if ( !set )             set = chan->SetLastDate( xfeed->FirstChildElement("published") );
	if ( !set && xentry )   set = chan->SetLastDate( xentry->FirstChildElement("updated") );
	if ( !set && xentry )   set = chan->SetLastDate( xentry->FirstChildElement("published") );

	set = chan->SetHomePage( xlink->Attribute( "href" ) );
	if ( !set && xauthor )    set = chan->SetHomePage( xauthor->FirstChildElement("uri") );
	if ( !set && xauthlink )  set = chan->SetHomePage( xauthlink->Attribute( "href" ) );

	if ( cfg->verbose )
		printf("Channel '%s' at '%s':\n", chan->title.String(), chan->homePage.String());
}

void
atomEntryParse ( Channel** chanPtr, Config* cfg, tinyxml2::XMLElement* xentry )
{
	Channel* chan = *(chanPtr);
	Item* newItem = (Item*)malloc( sizeof(Item) );
	newItem = new Item( chan->outputDir );

	tinyxml2::XMLElement* xcontent = xentry->FirstChildElement("content");
	tinyxml2::XMLElement* xmedia   = xentry->FirstChildElement("media:group");
	tinyxml2::XMLPrinter  xprinter;

	newItem->SetTitle( xentry->FirstChildElement("title") );
	newItem->SetPostUrl( xentry->FirstChildElement("link")->Attribute("href") );

	bool set = false;
	set = newItem->SetDesc( xentry->FirstChildElement("summary") );
	if ( !set )           set = newItem->SetDesc( xentry->FirstChildElement("description")); 
	if ( !set && xmedia ) set = newItem->SetDesc( xmedia->FirstChildElement("media:description")); 

	set = newItem->SetPubDate( xentry->FirstChildElement("updated") );
	if ( !set )   set = newItem->SetPubDate( xentry->FirstChildElement("published") );

	if ( xcontent ) {
		xcontent->Accept( &xprinter );
		newItem->SetContent( xprinter.CStr() );
	}

	if ( cfg->verbose )
		printf("\t%s\n", newItem->title.String());

	if ( withinDateRange( cfg->minDate, newItem->pubDate, cfg->maxDate ) )
		chan->items.AddItem( newItem );
}

void
atomParseEntries ( Channel** chanPtr, Config* cfg, tinyxml2::XMLElement* xfeed )
{
	Channel* chan = *(chanPtr);
	tinyxml2::XMLElement* xentry;

	xentry = xfeed->FirstChildElement("entry");

	int entryCount = xmlCountSiblings( xentry, "entry" );
	chan->items = BList(entryCount);

	if ( cfg->verbose )
		printf("\t-%i items-\n", entryCount);

	while ( xentry ) {
		atomEntryParse( chanPtr, cfg, xentry );
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
