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
	BString cache("/boot/home/config/cache/Pogger/");
	cache.Append(urlToFilename(xmlUrl));
	SetCachePath(cache);
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
	SetTitle(feed->GetTitle());
	SetXmlUrl(feed->GetXmlUrl());
}
	

Feed::Feed()
	:
	title(BString("Untitled Feed"))
{
	lastDate = BDateTime::CurrentDateTime(B_LOCAL_TIME);
}


void
Feed::Parse()
{
	BFile feedFile = BFile(cachePath, B_READ_ONLY);
	time_t tt_lastDate = 0;

	feedFile.ReadAttr("Feed:when", B_TIME_TYPE, 0, &tt_lastDate, sizeof(time_t));

	if (tt_lastDate > 0)
		lastDate.SetTime_t(tt_lastDate);
}


// Download a remote feed's XML to the cache path.
bool
Feed::Fetch()
{
	BFile cacheFile = BFile(cachePath, B_READ_WRITE | B_CREATE_FILE);

	cacheFile.ReadAttrString("Feed:hash", &lastHash);

	int32 result = fetch(xmlUrl, &cacheFile, &hash, 30);
	cacheFile.WriteAttrString("Feed:hash", &hash);

	if (result == 0)
		return true;
	return false;
}


void
Feed::Filetize()
{
	BFile feedFile(cachePath, B_WRITE_ONLY | B_CREATE_FILE);
	time_t tt_date = date.Time_t();
	BString url = xmlUrl.UrlString();
	BString name = GetTitle();
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
	BEntry entry(GetCachePath().String());
	entry.Remove();
}


bool
Feed::IsRss()
{
	tinyxml2::XMLDocument xml;
	xml.LoadFile(cachePath.String());

	if (xml.FirstChildElement("rss"))
		return true;
	return false;
}


bool
Feed::IsAtom()
{
	tinyxml2::XMLDocument xml;
	xml.LoadFile(cachePath.String());

	if (xml.FirstChildElement("feed"))
		return true;
	return false;
}


bool
Feed::IsUpdated()
{
	return lastHash != hash;
}


// Count the amount of siblings to an element of given type name
int
Feed::xmlCountSiblings (tinyxml2::XMLElement* xsibling, const char* sibling_name)
{
	int count = 0;
	while (xsibling) {
		count++;
		xsibling = xsibling->NextSiblingElement(sibling_name);
	}
	return count;
}


bool
Feed::AddEntry (Entry* newEntry)
{
	entries.AddItem(newEntry);
	return true;
}


BList
Feed::GetEntries()
{
	return entries;
}


BList
Feed::GetNewEntries()
{
	BList newEntries;
	for (int i = 0; i < entries.CountItems(); i++) {
		Entry* entry = ((Entry*)entries.ItemAt(i));
		if (entry->GetDate() > lastDate)
			newEntries.AddItem(entry);
	}	

	return newEntries;
}


bool
Feed::SetTitle(const char* titleStr)
{
	if (titleStr != NULL)
		title = BString(titleStr);
	else return false;
	return true;
}


bool
Feed::SetTitle(tinyxml2::XMLElement* elem)
{
	if (elem != NULL)
		return SetTitle(elem->GetText());
	else return false;
}


BString
Feed::GetTitle()
{
	return title;
}


bool
Feed::SetXmlUrl(BUrl newUrl)
{
	xmlUrl = newUrl;
	return true;
}


BUrl
Feed::GetXmlUrl()
{
	return xmlUrl;
}


bool
Feed::SetCachePath(BString path)
{
	cachePath = path;
	return true;
}


BString
Feed::GetCachePath()
{
	return cachePath;
}


// Set the latest date given by feed (from entry or from root)
bool
Feed::SetDate(BDateTime newDate)
{
	if (newDate == NULL)
		return false;
	date = newDate;
	return true;
}


bool
Feed::SetDate(const char* dateCStr)
{
	if (dateCStr == NULL)
		return false;
	SetDate(feedDateToBDate(dateCStr));
	return true;
}


bool
Feed::SetDate(tinyxml2::XMLElement* elem)
{
	if (elem != NULL)
		return SetDate(elem->GetText());
	else return false;
}


BDateTime
Feed::GetDate()
{
	return date;
}


