/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef FEED_H
#define FEED_H


#include <tinyxml2.h>

#include "Entry.h"


class BDateTime;
class BString;
class BList;
class BUrl;


class Feed {
public:
	Feed(BString);
	Feed();

	virtual void Parse();

	bool IsRemote();
	bool IsUpdated();
	bool IsRss();
	bool IsAtom();

	bool	AddEntry(Entry*);
	BList	GetEntries();
	bool	SetTitle(const char*);
	bool	SetTitle(tinyxml2::XMLElement*);
	BString	GetTitle();
	bool	SetDesc(const char*);
	bool	SetDesc(tinyxml2::XMLElement*);
	BString	GetDesc();
	bool	SetHomeUrl(const char*);
	bool	SetHomeUrl(tinyxml2::XMLElement*);
	BString	GetHomeUrl();
	bool	SetDate(const char*);
	bool	SetDate(tinyxml2::XMLElement*);
	BDateTime GetDate();

	BString	SetCachePath(BString);
	BString	GetCachePath();

protected:
	void EnsureCached();
	BString FetchRemoteFeed();
	int	xmlCountSiblings(tinyxml2::XMLElement*, const char*);

	BString	title;
	BString	description;
	BDateTime date;
	BDateTime lastDate;
	BDateTime minDate;
	BDateTime maxDate;
	BString	homeUrl;
	BString inputPath;
	BString	xmlUrl;
	BString	cachePath;
	BString outputDir;
	BList	entries;
	bool	fetched;
	bool	updated;
};


#endif

