/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "RssFeed.h"

#include <iostream>

#include "App.h"
#include "Entry.h"
#include "Util.h"


RssFeed::RssFeed()
{
	fTitle = BString("Untitled Feed");
	fXmlUrl = BString("");
}


RssFeed::RssFeed(Feed* feed)
	: RssFeed::RssFeed()
{
	SetXmlUrl(feed->XmlUrl());
	SetCachePath(feed->CachePath());
}


void
RssFeed::Parse()
{
	tinyxml2::XMLDocument xml;
	fEntries = BObjectList<Entry>(5, true);

	Feed::Parse();

	xml.LoadFile(CachePath().String());
	tinyxml2::XMLElement* xchan = xml.FirstChildElement("rss")->FirstChildElement("channel");

	RootParse(xchan);
	ParseEntries(xchan);

	Filetize();
}


void
RssFeed::RootParse(tinyxml2::XMLElement* xchan)
{
	_SetTitle(xchan->FirstChildElement("title"));
	_SetDate(xchan->FirstChildElement("lastBuildDate"));

	std::cout << "Channel '" << fTitle.String() << "' at '" << fXmlUrl.UrlString()
		<< ":\n";
}


void
RssFeed::EntryParse(tinyxml2::XMLElement* xitem)
{
	Entry* newEntry = new Entry();

	newEntry->SetTitle(xitem->FirstChildElement("title"));
	newEntry->SetDescription(xitem->FirstChildElement("description"));
	newEntry->SetDate(xitem->FirstChildElement("pubDate"));
	newEntry->SetPostUrl(xitem->FirstChildElement("link"));
	newEntry->SetContent(xitem->FirstChildElement("content:encoded"));
	newEntry->SetFeedTitle(fTitle);

	if (fDate == NULL || fDate < newEntry->Date())
		fDate = newEntry->Date();

	_AddEntry(newEntry);
}


void
RssFeed::ParseEntries(tinyxml2::XMLElement* xchan)
{
	tinyxml2::XMLElement* xitem;

	xitem = xchan->FirstChildElement("item");

	int entryCount = _XmlCountSiblings(xitem, "item");
	fEntries = BObjectList<Entry>(entryCount, true);

	std::cout << "\t-" << entryCount << " entries-\n";

	while (xitem) {
		EntryParse(xitem);
		xitem = xitem->NextSiblingElement("item");
	}
}


