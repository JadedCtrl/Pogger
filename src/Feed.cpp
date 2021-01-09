/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "Feed.h"

#include <tinyxml2.h>

#include "App.h"
#include "Entry.h"
#include "Config.h"
#include "Util.h"


Feed::Feed(BString path)
{
	title = BString("Untitled Feed");
	description = BString("Nondescript, N/A.");
	homeUrl = BString("");
	xmlUrl = BString("");
	updated = true;
	fetched = false;
	inputPath = path;
	SetCachePath(path);
}


Feed::Feed(Feed* feed)
	: Feed()
{
	SetCachePath(feed->GetCachePath());
}
	

Feed::Feed()
{
	title = BString("");
	description = BString("");
	homeUrl = BString("");
	xmlUrl = BString("");
}


void
Feed::Parse()
{
	BFile* feedFile = new BFile(GetCachePath().String(), B_READ_ONLY);
	BDateTime attrLastDate = BDateTime();
	time_t tt_lastDate = 0;

	feedFile->ReadAttr("LastDate", B_TIME_TYPE, 0, &tt_lastDate,
		sizeof(time_t));

	if (tt_lastDate > 0 && ((App*)be_app)->cfg->updateFeeds == true) {
		attrLastDate.SetTime_t(tt_lastDate);
		minDate = attrLastDate;
	}
}


// Download a remote feed's XML to the cache path.
BString
Feed::FetchRemoteFeed()
{
	BUrl givenUrl = BUrl(inputPath);
	time_t tt_lastDate = 0;
	BDateTime* lastDate = new BDateTime();
	BString* newHash = new BString();
	char oldHash[41];

	BFile* cacheFile = new BFile(GetCachePath(), B_READ_WRITE | B_CREATE_FILE);

	cacheFile->ReadAttr("LastHash", B_STRING_TYPE, 0, oldHash, 41);

	if (((App*)be_app)->cfg->verbose)
		printf("Saving %s...\n", inputPath.String());

	webFetch(BUrl(inputPath), cacheFile, newHash);

	cacheFile->WriteAttr("LastHash", B_STRING_TYPE, 0,
		newHash->String(), newHash->CountChars());

	if (*(newHash) == BString(oldHash))
		updated = false;

	fetched = true;
	return GetCachePath();
}


// return whether or not the feed's given URI/path is remote.
bool
Feed::IsRemote ()
{
	return isRemotePath(inputPath);
}


// return whether or not the feed seems to have been updated
bool
Feed::IsUpdated ()
{
	return updated;
}


// return whether or not feed is RSS
bool
Feed::IsRss ()
{
	EnsureCached();
	tinyxml2::XMLDocument xml;
	xml.LoadFile(GetCachePath().String());

	if (xml.FirstChildElement("rss"))
		return true;
	return false;
}


// return whether or not feed is Atom
bool
Feed::IsAtom ()
{
	tinyxml2::XMLDocument xml;
	xml.LoadFile(GetCachePath().String());

	if (xml.FirstChildElement("feed"))
		return true;
	return false;
}


// ensure the feed XML is available at the cache path.
// if necessary, download it.
void
Feed::EnsureCached ()
{
	if (IsRemote() && fetched == false)
		FetchRemoteFeed();
}


// Return the 'cachePath' (location of XML file locally)
BString
Feed::GetCachePath ()
{
	if (cachePath == NULL)
		SetCachePath(inputPath);
	return cachePath;
}


// Select a 'cachePath' (location of XML file locally)
// For remote files, a cache file is created in ~/config/cache/Pogger/ by default
// For local files, the same local file is used.
BString
Feed::SetCachePath (BString givenPath)
{
	BUrl givenUrl = BUrl(givenPath);
	BString protocol = givenUrl.Protocol().String();

	if (protocol == NULL && givenUrl.UrlString() != NULL) {
		cachePath = givenPath;
		return givenPath;
	}

	BString splitName = givenUrl.Host();
	splitName.Append(givenUrl.Path());
	splitName.ReplaceAll("/", "_");

	BString filename = ((App*)be_app)->cfg->cacheDir;
	filename.Append(splitName);

	cachePath = filename;
	return filename;
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


// Add the given entry to the feed, if appropriate
bool
Feed::AddEntry (Entry* newEntry)
{
	Config* cfg = ((App*)be_app)->cfg;
	if (!withinDateRange(cfg->minDate, newEntry->date, cfg->maxDate) ||
		!withinDateRange(minDate, newEntry->date, maxDate))
		return false;

	if (cfg->verbose == true)
		printf("\t%s\n", newEntry->title.String());
	entries.AddItem(newEntry);
	return true;
}


BList
Feed::GetEntries()
{
	return entries;
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
Feed::SetDesc(const char* descStr)
{
	if (descStr != NULL)
		description = BString(descStr);
	else return false;
	return true;
}


bool
Feed::SetDesc(tinyxml2::XMLElement* elem)
{
	if (elem != NULL)
		return SetDesc(elem->GetText());
	else return false;
}


BString
Feed::GetDesc()
{
	return description;
}


// set a feed's «home URL»
bool
Feed::SetHomeUrl(const char* homepageStr)
{
	if (homepageStr != NULL)
		homeUrl = BString(homepageStr);
	else return false;
	return true;
}


bool
Feed::SetHomeUrl(tinyxml2::XMLElement* elem)
{
	if (elem != NULL)
		return SetHomeUrl(elem->GetText());
	else return false;
}


BString
Feed::GetHomeUrl()
{
	return homeUrl;
}


BString
Feed::GetXmlUrl()
{
	return xmlUrl;
}


// set the update time for a feed
bool
Feed::SetDate(const char* dateCStr)
{
	if (dateCStr == NULL)
		return false;
	BDateTime newDate = feedDateToBDate(dateCStr);
	if (newDate == NULL)
		return false;
	date = newDate;
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


