/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "Feed.h"

#include <File.h>

#include <tinyxml2.h>

#include "App.h"
#include "Entry.h"
#include "Util.h"


Feed::Feed(BUrl xml, BString path)
	: Feed()
{
	SetXmlUrl(xml);
	SetCachePath(path);
}


Feed::Feed(BUrl xml)
	: Feed()
{
	SetXmlUrl(xml);

	BPath cache;
	find_directory(B_USER_CACHE_DIRECTORY, &cache);
	cache.Append("Pogger");

	cache.Append(urlToFilename(fXmlUrl));
	SetCachePath(cache.Path());
}


// For pre-existing feed
Feed::Feed(BEntry entry)
	: Feed()
{
	BFile file(&entry, B_READ_ONLY);
	BPath path;
	entry.GetPath(&path);
	SetCachePath(BString(path.Path()));

	BString name;
	BString url;
	file.ReadAttrString("META:url", &url);
	file.ReadAttrString("Feed:name", &name);

	if (!url.IsEmpty())
		SetXmlUrl(BUrl(url));
	if (!name.IsEmpty())
		SetTitle(name);
}


Feed::Feed(const char* pathStr)
	: Feed(BEntry(pathStr))
{
}


// For new feed
Feed::Feed(BUrl xml, BEntry entry)
	: Feed()
{
	BPath path;
	BString pathString;
	entry.GetPath(&path);
	pathString = path.Path();
	
	if (entry.IsDirectory())
		pathString += BString(urlToFilename(xml));

	SetCachePath(pathString);
	SetXmlUrl(xml);
}


Feed::Feed(Feed* feed)
	: Feed()
{
	SetTitle(feed->Title());
	SetXmlUrl(feed->XmlUrl());
}
	

Feed::Feed()
	:
	fTitle(BString("Untitled Feed"))
{
	fLastDate = BDateTime::CurrentDateTime(B_LOCAL_TIME);
}


Feed::~Feed()
{
}


void
Feed::Parse()
{
	BFile feedFile = BFile(fCachePath, B_READ_ONLY);
	time_t tt_lastDate = 0;

	feedFile.ReadAttr("Feed:when", B_TIME_TYPE, 0, &tt_lastDate, sizeof(time_t));

	if (tt_lastDate > 0)
		fLastDate.SetTime_t(tt_lastDate);
}


// Download a remote feed's XML to the cache path.
bool
Feed::Fetch()
{
	BFile cacheFile = BFile(fCachePath, B_READ_WRITE | B_CREATE_FILE);

	cacheFile.ReadAttrString("Feed:hash", &fLastHash);

	int32 result = fetch(fXmlUrl, &cacheFile, &fHash, 30);
	cacheFile.WriteAttrString("Feed:hash", &fHash);

	if (result == 0)
		return true;
	return false;
}


void
Feed::Filetize()
{
	BFile feedFile(fCachePath, B_WRITE_ONLY | B_CREATE_FILE);
	time_t tt_date = fDate.Time_t();
	BString url = fXmlUrl.UrlString();
	BString name = Title();
	BString type("application/x-feed-source");

	feedFile.WriteAttrString("Feed:name", &name);
	feedFile.WriteAttrString("META:url", &url);
	feedFile.WriteAttrString("BEOS:TYPE", &type);
	feedFile.WriteAttr("Feed:when", B_TIME_TYPE, 0, &tt_date, sizeof(time_t));
	feedFile.WriteAttr("BEOS:TYPE", B_MIME_STRING_TYPE, 0, type.String(),
	type.CountChars() + 1);
}


void
Feed::Unfiletize()
{
	BEntry entry(CachePath().String());
	entry.Remove();
}


bool
Feed::IsRss()
{
	tinyxml2::XMLDocument xml;
	xml.LoadFile(fCachePath.String());

	if (xml.FirstChildElement("rss"))
		return true;
	return false;
}


bool
Feed::IsAtom()
{
	tinyxml2::XMLDocument xml;
	xml.LoadFile(fCachePath.String());

	if (xml.FirstChildElement("feed"))
		return true;
	return false;
}


bool
Feed::IsUpdated()
{
	return fLastHash != fHash;
}


// Count the amount of siblings to an element of given type name
int
Feed::_XmlCountSiblings (tinyxml2::XMLElement* xsibling, const char* sibling_name)
{
	int count = 0;
	while (xsibling) {
		count++;
		xsibling = xsibling->NextSiblingElement(sibling_name);
	}
	return count;
}


bool
Feed::_AddEntry (Entry* newEntry)
{
	fEntries.AddItem(newEntry);
	return true;
}


BObjectList<Entry>
Feed::Entries()
{
	return fEntries;
}


BObjectList<Entry>
Feed::NewEntries()
{
	BObjectList<Entry> newEntries;
	for (int i = 0; i < fEntries.CountItems(); i++) {
		Entry* entry = fEntries.ItemAt(i);
		if (entry->Date() > fLastDate)
			newEntries.AddItem(entry);
	}	

	return newEntries;
}


bool
Feed::SetTitle(const char* titleStr)
{
	if (titleStr != NULL)
		fTitle = BString(titleStr);
	else return false;
	return true;
}


bool
Feed::_SetTitle(tinyxml2::XMLElement* elem)
{
	if (elem != NULL)
		return SetTitle(elem->GetText());
	else return false;
}


BString
Feed::Title()
{
	return fTitle;
}


bool
Feed::SetXmlUrl(BUrl newUrl)
{
	fXmlUrl = newUrl;
	return true;
}


BUrl
Feed::XmlUrl()
{
	return fXmlUrl;
}


bool
Feed::SetCachePath(BString path)
{
	fCachePath = path;
	return true;
}


BString
Feed::CachePath()
{
	return fCachePath;
}


// Set the latest date given by feed (from entry or from root)
bool
Feed::_SetDate(BDateTime newDate)
{
	if (newDate == NULL)
		return false;
	fDate = newDate;
	return true;
}


bool
Feed::_SetDate(const char* dateCStr)
{
	if (dateCStr == NULL)
		return false;
	_SetDate(feedDateToBDate(dateCStr));
	return true;
}


bool
Feed::_SetDate(tinyxml2::XMLElement* elem)
{
	if (elem != NULL)
		return _SetDate(elem->GetText());
	else return false;
}


BDateTime
Feed::Date()
{
	return fDate;
}


