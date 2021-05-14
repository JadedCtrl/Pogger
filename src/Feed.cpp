/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "Feed.h"

#include <File.h>
#include <FindDirectory.h>

#include "Entry.h"
#include "Util.h"


Feed::Feed()
	: fTitle(BString(""))
{
	fLastDate = BDateTime::CurrentDateTime(B_LOCAL_TIME);
}


Feed::Feed(const char* identifier, const char* source, const char* title,
	const char* url)
	: fTitle(BString(title)),
	fSource(BString(source)),
	fIdentifier(BString(identifier)),
	fUrl(BUrl(url))
{
}


Feed::Feed(Feed* feed)
	: Feed()
{
	SetTitle(feed->Title());
	SetUrl(feed->Url());
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


void
Feed::SetEntries(BObjectList<Entry> entries)
{
	fEntries = entries;
}


BString
Feed::Title()
{
	return fTitle;
}


bool
Feed::SetTitle(const char* titleStr)
{
	if (titleStr != NULL)
		fTitle = BString(titleStr);
	else return false;
	return true;
}


BUrl
Feed::Url()
{
	return fUrl;
}


bool
Feed::SetUrl(BUrl newUrl)
{
	fUrl = newUrl;
	return true;
}


BDateTime
Feed::Date()
{
	return fDate;
}


// Set the latest date given by feed (from entry or from metadata)
bool
Feed::SetDate(BDateTime newDate)
{
	if (newDate == NULL)
		return false;
	fDate = newDate;
	return true;
}


BDateTime
Feed::LastDate()
{
	return fLastDate;
}


// Set the latest time the feed was fetched and parsed
bool
Feed::SetLastDate(BDateTime newDate)
{
	if (newDate == NULL)
		return false;
	fLastDate = newDate;
	return true;
}


BString
Feed::Hash()
{
	return fHash;
}


bool
Feed::SetHash(BString hash)
{
	fHash = hash;
	return true;
}


BString
Feed::LastHash()
{
	return fLastHash;
}


bool
Feed::SetLastHash(BString hash)
{
	fLastHash = hash;
	return true;
}


const char*
Feed::Identifier()
{
	return fIdentifier.String();
}


bool
Feed::SetIdentifier(const char* id)
{
	fIdentifier = BString(id);
	return true;
}


const char*
Feed::Source()
{
	return fSource.String();
}


bool
Feed::SetSource(const char* source)
{
	fSource = BString(source);
	return true;
}


