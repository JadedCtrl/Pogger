#include <iostream>
#include <sstream>
#include <tinyxml2.h>
#include "Channel.h"
#include "Item.h"
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
		printf("has atom\n");
}

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
