/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "AtomFeed.h"

#include <iostream>

#include <tinyxml2.h>

#include "App.h"
#include "Entry.h"
#include "Util.h"


AtomFeed::AtomFeed()
{
	fTitle = BString("");
	fCachePath = BString("");
}


AtomFeed::AtomFeed(Feed* feed)
	: AtomFeed::AtomFeed()
{
	SetTitle(feed->Title());
	SetXmlUrl(feed->XmlUrl());
	SetCachePath(feed->CachePath());
}


void
AtomFeed::Parse()
{
	fEntries = BObjectList<Entry>(5, true);
	tinyxml2::XMLDocument xml;
	xml.LoadFile(CachePath().String());

	Feed::Parse();

	tinyxml2::XMLElement* xfeed = xml.FirstChildElement("feed");

	RootParse(xfeed);
	ParseEntries(xfeed);

	Filetize();
}


void
AtomFeed::RootParse(tinyxml2::XMLElement* xfeed)
{
	tinyxml2::XMLElement* xentry = xfeed->FirstChildElement("entry");
	
	bool set = false;

	_SetTitle(xfeed->FirstChildElement("title"));

	set = _SetDate(xfeed->FirstChildElement("updated"));
	if (!set)
		set = _SetDate(xfeed->FirstChildElement("published"));
	if (!set && xentry)
		set = _SetDate(xentry->FirstChildElement("updated"));
	if (!set && xentry)
		set = _SetDate(xentry->FirstChildElement("published"));

	std::cout << "Channel '" << fTitle << "' at '" << fXmlUrl.UrlString()
		<< "':\n";
}


void
AtomFeed::EntryParse(tinyxml2::XMLElement* xentry)
{
	Entry* newEntry = new Entry();

	tinyxml2::XMLElement* xcontent = xentry->FirstChildElement("content");
	tinyxml2::XMLElement* xmedia   = xentry->FirstChildElement("media:group");
	tinyxml2::XMLPrinter  xprinter;

	newEntry->SetTitle(xentry->FirstChildElement("title"));
	newEntry->SetPostUrl(xentry->FirstChildElement("link")->Attribute("href"));
	newEntry->SetFeedTitle(fTitle);

	bool set = false;
	set = newEntry->SetDescription(xentry->FirstChildElement("summary"));
	if (!set)
		set = newEntry->SetDescription(xentry->FirstChildElement("description")); 
	if (!set && xmedia)
		set = newEntry->SetDescription(
			xmedia->FirstChildElement("media:description")); 

	set = newEntry->SetDate(xentry->FirstChildElement("updated"));
	if (!set)
		set = newEntry->SetDate(xentry->FirstChildElement("published"));

	if (fDate == NULL || fDate < newEntry->Date())
		_SetDate(newEntry->Date());

	if (xcontent) {
		xcontent->Accept(&xprinter);
		newEntry->SetContent(xprinter.CStr());
	}

	_AddEntry(newEntry);
}


void
AtomFeed::ParseEntries(tinyxml2::XMLElement* xfeed)
{
	tinyxml2::XMLElement* xentry;

	xentry = xfeed->FirstChildElement("entry");

	int entryCount = _XmlCountSiblings(xentry, "entry");
	fEntries = BObjectList<Entry>(entryCount, true);

	std::cout << "\t-" << entryCount << "-\n";

	while (xentry) {
		EntryParse(xentry);
		xentry = xentry->NextSiblingElement("entry");
	}
}
