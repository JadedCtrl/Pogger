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
	Feed(BUrl);
	Feed(BUrl, BString);
	Feed(Feed*);
	Feed();

	virtual void Parse();

	BList	GetEntries();
	BList	GetNewEntries();

	BString	Fetch();

	bool	IsRss();
	bool	IsAtom();
	bool	IsUpdated();

	BString	GetTitle();
	BUrl	GetXmlUrl();
	BDateTime GetDate();

	BString	GetCachePath();

protected:
	bool	SetTitle(const char*);
	bool	SetTitle(tinyxml2::XMLElement*);
	bool	SetDate(BDateTime);
	bool	SetDate(const char*);
	bool	SetDate(tinyxml2::XMLElement*);
	bool	SetXmlUrl(BUrl newUrl);
	bool	SetCachePath(BString path);

	bool	AddEntry(Entry*);

	int		xmlCountSiblings(tinyxml2::XMLElement*, const char*);

	BString	title;
	BDateTime date;
	BDateTime lastDate;
	BUrl	xmlUrl;
	BString	cachePath;
	BString hash;
	BString lastHash;

	BList	entries;
	bool	fetched;
	bool	updated;
};


#endif

