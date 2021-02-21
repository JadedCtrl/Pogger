/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "AtomFeed.h"

#include <tinyxml2.h>

#include <iostream>

#include "App.h"
#include "Entry.h"
#include "Util.h"


AtomFeed::AtomFeed()
{
	title = BString("Untitled Feed");
	cachePath = BString("");
}


AtomFeed::AtomFeed(Feed* feed)
	: AtomFeed::AtomFeed()
{
	SetXmlUrl(feed->GetXmlUrl());
	SetCachePath(feed->GetCachePath());
}


void
AtomFeed::Parse ()
{
	entries = BList();
	tinyxml2::XMLDocument xml;
	xml.LoadFile(GetCachePath().String());

	Feed::Parse();

	tinyxml2::XMLElement* xfeed = xml.FirstChildElement("feed");

	RootParse(xfeed);
	ParseEntries(xfeed);

	Filetize();
}


void
AtomFeed::RootParse(tinyxml2::XMLElement* xfeed)
{
	tinyxml2::XMLElement* xauthor = xfeed->FirstChildElement("author");
	tinyxml2::XMLElement* xentry = xfeed->FirstChildElement("entry");
	tinyxml2::XMLElement* xlink = xfeed->FirstChildElement("link");
	tinyxml2::XMLElement* xauthlink = xauthor->FirstChildElement("link");
	
	bool set = false;

	SetTitle(xfeed->FirstChildElement("title"));

	set = SetDate(xfeed->FirstChildElement("updated"));
	if (!set)
		set = SetDate(xfeed->FirstChildElement("published"));
	if (!set && xentry)
		set = SetDate(xentry->FirstChildElement("updated"));
	if (!set && xentry)
		set = SetDate(xentry->FirstChildElement("published"));

	std::cout << "Channel '" << title << "' at '" << xmlUrl.UrlString()
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
	newEntry->SetFeedTitle(title);

	bool set = false;
	set = newEntry->SetDesc(xentry->FirstChildElement("summary"));
	if (!set)
		set = newEntry->SetDesc(xentry->FirstChildElement("description")); 
	if (!set && xmedia)
		set = newEntry->SetDesc(xmedia->FirstChildElement("media:description")); 

	set = newEntry->SetDate(xentry->FirstChildElement("updated"));
	if (!set)
		set = newEntry->SetDate(xentry->FirstChildElement("published"));

	if (date == NULL || date < newEntry->GetDate())
		SetDate(newEntry->GetDate());

	if (xcontent) {
		xcontent->Accept(&xprinter);
		newEntry->SetContent(xprinter.CStr());
	}

	AddEntry(newEntry);
}


void
AtomFeed::ParseEntries(tinyxml2::XMLElement* xfeed)
{
	tinyxml2::XMLElement* xentry;

	xentry = xfeed->FirstChildElement("entry");

	int entryCount = xmlCountSiblings(xentry, "entry");
	entries = BList(entryCount);

	std::cout << "\t-" << entryCount << "-\n";

	while (xentry) {
		EntryParse(xentry);
		xentry = xentry->NextSiblingElement("entry");
	}
}
