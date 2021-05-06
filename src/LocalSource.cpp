/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "LocalSource.h"

#include <iostream>

#include <Catalog.h>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <FindDirectory.h>
#include <Path.h>

#include "Feed.h"
#include "Util.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "LocalSource"


LocalSource::LocalSource()
{
	BDirectory subDir = BDirectory(_SubscriptionPath().Path());
	if (subDir.InitCheck() == B_ENTRY_NOT_FOUND) {
//		subDir.CreateDirectory(subPath.Path(), &subDir);

//		subPath.Append("Haiku Project");
//		Feed defaultSub(BUrl("https://www.haiku-os.org/blog/index.xml"),
//			BEntry(subPath.Path()));
//		defaultSub.SetTitle("Haiku Project");
//		defaultSub.Filetize();
	}
}


BObjectList<Feed>
LocalSource::Feeds()
{
	BDirectory subDir = BDirectory(_SubscriptionPath().Path());
	BEntry feedEntry;
	BPath feedPath;

	BObjectList<Feed> feeds;

	while (subDir.GetNextEntry(&feedEntry) == B_OK
		&& feedEntry.GetPath(&feedPath) == B_OK)
	{
		Feed* feed = GetFeed(feedPath.Path());
		feeds.AddItem(feed);
	}

	return feeds;
}


Feed*
LocalSource::GetFeed(const char* identifier)
{
	BFile file(identifier, B_READ_ONLY);
	time_t tt_lastDate = 0;
	BDateTime lastDate;
	BString title;
	BString url;
	BString hash;

	file.ReadAttrString("Feed:name", &title);
	file.ReadAttrString("META:url", &url);
	file.ReadAttrString("Feed:hash", &hash);
	file.ReadAttr("Feed:when", B_TIME_TYPE, 0, &tt_lastDate, sizeof(time_t));
	lastDate.SetTime_t(tt_lastDate);

	Feed* feed = new Feed(identifier, title.String(), url.String());
	feed->SetHash(hash);
	feed->SetLastDate(lastDate);
	return feed;
}


bool
LocalSource::Fetch(Feed* feed)
{
	BFile cacheFile = BFile(feed->Identifier(), B_READ_WRITE | B_CREATE_FILE);
	BString hash;

	int32 result = fetch(feed->Url(), &cacheFile, &hash, 30);
	feed->SetHash(hash);

	if (result == 0)
		return true;
	return false;
}


bool
LocalSource::Parse(Feed* feed)
{
	if (IsUpdated(feed) == false)
		return true;

	if (_IsAtom(feed) == true)
		return _AtomParse(feed);
	else if (_IsRss(feed) == true)
		return _RssParse(feed);
	return false;
}


void
LocalSource::AddFeed(Feed* newFeed)
{
	if (newFeed->Identifier().IsEmpty() == true) {
		BPath subPath = _SubscriptionPath();
		subPath.Append(urlToFilename(newFeed->Url()));
		newFeed->SetIdentifier(subPath.Path());
	}
	EditFeed(newFeed);
}


void
LocalSource::EditFeed(Feed* updated)
{
	Feed* old = GetFeed(updated->Identifier());
	BFile file(updated->Identifier(), B_READ_WRITE | B_CREATE_FILE);

	if (old->Title() != updated->Title()) {
		BString title = updated->Title();
		file.WriteAttrString("Feed:name", &title);
	}
	if (old->Url() != updated->Url()) {
		BString url = updated->Url().UrlString();
		file.WriteAttrString("META:url", &url);
	}
	if (old->Hash() != updated->Hash()) {
		BString hash = updated->Hash();
		file.WriteAttrString("Feed:hash", &hash);
	}
	if (old->Date() != updated->Date()) {
		time_t tt_date = updated->Date().Time_t();
		file.WriteAttr("Feed:when", B_TIME_TYPE, 0, &tt_date, sizeof(time_t));
	}

	BString type("application/x-feed-source");
	file.WriteAttr("BEOS:TYPE", B_MIME_STRING_TYPE, 0, type.String(),
		type.CountChars() + 1);
}


void
LocalSource::RemoveFeed(Feed* mortonta)
{
	BEntry entry(mortonta->Identifier().String());
	entry.Remove();
}


bool
LocalSource::IsUpdated(Feed* feed)
{
	return (GetFeed(feed->Identifier())->Hash() != feed->Hash());
}


bool
LocalSource::_IsAtom(Feed* feed)
{
	tinyxml2::XMLDocument xml;
	xml.LoadFile(feed->Identifier().String());

	if (xml.FirstChildElement("feed"))
		return true;
	return false;
}


bool
LocalSource::_IsRss(Feed* feed)
{
	tinyxml2::XMLDocument xml;
	xml.LoadFile(feed->Identifier().String());

	if (xml.FirstChildElement("rss"))
		return true;
	return false;
}


bool
LocalSource::_AtomParse(Feed* atomFeed)
{
	tinyxml2::XMLDocument xml;
	xml.LoadFile(atomFeed->Identifier().String());

	tinyxml2::XMLElement* xfeed = xml.FirstChildElement("feed");

	_AtomRootParse(atomFeed, xfeed);
	_AtomEntriesParse(atomFeed, xfeed);
	return true;
}


void
LocalSource::_AtomRootParse(Feed* feed, tinyxml2::XMLElement* xfeed)
{
	tinyxml2::XMLElement* xentry = xfeed->FirstChildElement("entry");

	bool set = false;

	_SetTitle(feed, xfeed->FirstChildElement("title"));

	set = _SetDate(feed, xfeed->FirstChildElement("updated"));
	if (!set)
		set = _SetDate(feed, xfeed->FirstChildElement("published"));
	if (!set && xentry)
		set = _SetDate(feed, xentry->FirstChildElement("updated"));
	if (!set && xentry)
		set = _SetDate(feed, xentry->FirstChildElement("published"));

	BString logString(B_TRANSLATE("Channel '%source%' at %url%:\n"));
	logString.ReplaceAll("%source%", feed->Title().String());
	logString.ReplaceAll("%url%", feed->Url().UrlString());

	std::cout << logString.String();
}


void
LocalSource::_AtomEntriesParse(Feed* feed, tinyxml2::XMLElement* xfeed)
{
	tinyxml2::XMLElement* xentry;

	xentry = xfeed->FirstChildElement("entry");

	int entryCount = _XmlCountSiblings(xentry, "entry");
	BObjectList<Entry> entries(entryCount, true);

	BString logString(B_TRANSLATE("\t-%count% entries-\n"));
	BString entryStr;
	entryStr << entryCount;
	logString.ReplaceAll("%count%", entryStr);

	std::cout << logString.String();

	while (xentry) {
		Entry* newEntry = _AtomEntryParse(feed, xentry);
		if (newEntry != NULL)
			entries.AddItem(newEntry);
		xentry = xentry->NextSiblingElement("entry");
	}

	feed->SetEntries(entries);
}


Entry*
LocalSource::_AtomEntryParse(Feed* feed, tinyxml2::XMLElement* xentry)
{
	Entry* newEntry = new Entry();

	tinyxml2::XMLElement* xcontent = xentry->FirstChildElement("content");
	tinyxml2::XMLElement* xmedia   = xentry->FirstChildElement("media:group");
	tinyxml2::XMLPrinter  xprinter;

	newEntry->SetTitle(xentry->FirstChildElement("title"));
	newEntry->SetPostUrl(xentry->FirstChildElement("link")->Attribute("href"));
	newEntry->SetFeedTitle(feed->Title());

	bool set = false;
	set = newEntry->SetDescription(xentry->FirstChildElement("summary"));
	if (!set)
		set = newEntry->SetDescription(xentry->FirstChildElement("description")); 
	if (!set && xmedia)
		set = newEntry->SetDescription(
			xmedia->FirstChildElement("media:description")); 

	set = _SetDate(newEntry, xentry->FirstChildElement("updated"));
	if (!set)
		set = _SetDate(newEntry, xentry->FirstChildElement("published"));

	if (feed->Date() == NULL || feed->Date() < newEntry->Date())
		feed->SetDate(newEntry->Date());

	if (xcontent) {
		xcontent->Accept(&xprinter);
		newEntry->SetContent(xprinter.CStr());
	}

	return newEntry;
}


bool
LocalSource::_RssParse(Feed* feed)
{
	tinyxml2::XMLDocument xml;
	xml.LoadFile(feed->Identifier().String());

	tinyxml2::XMLElement* xchan = xml.FirstChildElement("rss")->FirstChildElement("channel");

	_RssRootParse(feed, xchan);
	_RssEntriesParse(feed, xchan);
	return true;
}


void
LocalSource::_RssRootParse(Feed* feed, tinyxml2::XMLElement* xchan)
{
	_SetTitle(feed, xchan->FirstChildElement("title"));
	_SetDate(feed, xchan->FirstChildElement("lastBuildDate"));

	BString logString(B_TRANSLATE("Channel '%source%' at %url%:\n"));
	logString.ReplaceAll("%source%", feed->Title().String());
	logString.ReplaceAll("%url%", feed->Url().UrlString());

	std::cout << logString.String();
}


void
LocalSource::_RssEntriesParse(Feed* feed, tinyxml2::XMLElement* xchan)
{
	tinyxml2::XMLElement* xitem;

	xitem = xchan->FirstChildElement("item");

	int entryCount = _XmlCountSiblings(xitem, "item");
	BObjectList<Entry> entries = BObjectList<Entry>(entryCount, true);

	BString logString(B_TRANSLATE("\t-%count% entries-\n"));
	BString entryStr;
	entryStr << entryCount;
	logString.ReplaceAll("%count%", entryStr);

	std::cout << logString.String();

	while (xitem) {
		entries.AddItem(_RssEntryParse(feed, xitem));
		xitem = xitem->NextSiblingElement("item");
	}
	feed->SetEntries(entries);
}


Entry*
LocalSource::_RssEntryParse(Feed* feed, tinyxml2::XMLElement* xitem)
{
	Entry* newEntry = new Entry();

	newEntry->SetTitle(xitem->FirstChildElement("title"));
	newEntry->SetDescription(xitem->FirstChildElement("description"));
	_SetDate(newEntry, xitem->FirstChildElement("pubDate"));
	newEntry->SetPostUrl(xitem->FirstChildElement("link"));
	newEntry->SetContent(xitem->FirstChildElement("content:encoded"));
	newEntry->SetFeedTitle(feed->Title());

	if (feed->Date() == NULL || feed->Date() < newEntry->Date())
		feed->SetDate(newEntry->Date());

	return newEntry;
}


bool
LocalSource::_SetTitle(Feed* feed, tinyxml2::XMLElement* elem)
{
	if (elem != NULL && feed->Title().IsEmpty() == true)
		return feed->SetTitle(elem->GetText());
	return false;
}

bool
LocalSource::_SetDate(Feed* feed, const char* dateCStr)
{
	if (dateCStr == NULL)
		return false;
	return feed->SetDate(feedDateToBDate(dateCStr));
}


bool
LocalSource::_SetDate(Feed* feed, tinyxml2::XMLElement* elem)
{
	if (elem == NULL)
		return false;
	return _SetDate(feed, elem->GetText());
}


bool
LocalSource::_SetDate(Entry* entry, const char* dateStr)
{
	if (dateStr == NULL)
		return false;
	return entry->SetDate(feedDateToBDate(dateStr));
}


bool
LocalSource::_SetDate(Entry* entry, tinyxml2::XMLElement* elem)
{
	if (elem == NULL)
		return false;

	return _SetDate(entry, elem->GetText());
}


// Count the amount of siblings to an element of given type name
int
LocalSource::_XmlCountSiblings (tinyxml2::XMLElement* xsibling,
	const char* sibling_name)
{
	int count = 0;
	while (xsibling) {
		count++;
		xsibling = xsibling->NextSiblingElement(sibling_name);
	}
	return count;
}


BPath
LocalSource::_SubscriptionPath()
{
	BPath subPath;
	find_directory(B_USER_SETTINGS_DIRECTORY, &subPath);
	subPath.Append("Pogger");
	subPath.Append("Subscriptions");
	return subPath;
}


