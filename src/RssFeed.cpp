/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "RssFeed.h"

#include "App.h"
#include "Config.h"
#include "Entry.h"
#include "Util.h"


RssFeed::RssFeed()
{
	title = BString("Untitled Feed");
	xmlUrl = BString("");
}


RssFeed::RssFeed(Feed* feed)
	: RssFeed::RssFeed()
{
	SetXmlUrl(feed->GetXmlUrl());
	SetCachePath(feed->GetCachePath());
}


void
RssFeed::Parse()
{
	tinyxml2::XMLDocument xml;
	entries = BList();

	Feed::Parse();

	xml.LoadFile(GetCachePath().String());
	tinyxml2::XMLElement* xchan = xml.FirstChildElement("rss")->FirstChildElement("channel");

	RootParse(xchan);
	ParseEntries(xchan);

	Filetize();
}


void
RssFeed::RootParse(tinyxml2::XMLElement* xchan)
{
	SetTitle(xchan->FirstChildElement("title"));
	SetDate(xchan->FirstChildElement("lastBuildDate"));

	printf("Channel '%s' at '%s':\n", title.String(),
		xmlUrl.UrlString().String());
}


void
RssFeed::EntryParse(tinyxml2::XMLElement* xitem)
{
	Entry* newEntry = (Entry*)malloc(sizeof(Entry));
	newEntry = new Entry();

	newEntry->SetTitle(xitem->FirstChildElement("title"));
	newEntry->SetDesc(xitem->FirstChildElement("description"));
	newEntry->SetDate(xitem->FirstChildElement("pubDate"));
	newEntry->SetPostUrl(xitem->FirstChildElement("link"));
	newEntry->SetContent(xitem->FirstChildElement("content:encoded"));
	newEntry->SetFeedTitle(title);

	if (date == NULL || date < newEntry->GetDate())
		date = newEntry->date;

	AddEntry(newEntry);
}


void
RssFeed::ParseEntries(tinyxml2::XMLElement* xchan)
{
	tinyxml2::XMLElement* xitem;

	xitem = xchan->FirstChildElement("item");

	int entryCount = xmlCountSiblings(xitem, "item");
	entries = BList(entryCount);

	printf("\t-%i entries-\n", entryCount);

	while (xitem) {
		EntryParse(xitem);
		xitem = xitem->NextSiblingElement("item");
	}
}


