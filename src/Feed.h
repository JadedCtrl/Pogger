/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef FEED_H
#define FEED_H


#include <tinyxml2.h>

#include <ObjectList.h>

#include "Entry.h"


class BDateTime;
class BEntry;
class BString;
class BUrl;


class Feed {
public:
	Feed(BUrl);
	Feed(BUrl, BString);
	Feed(BEntry);
	Feed(BUrl xml, BEntry entry);
	Feed(Feed*);
	Feed();

	~Feed();

	virtual void Parse();

	BObjectList<Entry>	GetEntries();
	BObjectList<Entry>	GetNewEntries();

	bool	Fetch();

	void	Filetize();
	void	Unfiletize();

	bool	IsRss();
	bool	IsAtom();
	bool	IsUpdated();

	BString	GetTitle();
	bool	SetDate(BDateTime);

	BUrl	GetXmlUrl();
	BDateTime GetDate();

	bool	SetTitle(const char*);
	bool	SetXmlUrl(BUrl newUrl);
	bool	SetCachePath(BString path);

	BString	GetCachePath();

protected:
	bool	SetTitle(tinyxml2::XMLElement*);
	bool	SetDate(const char*);
	bool	SetDate(tinyxml2::XMLElement*);

	bool	AddEntry(Entry*);

	int		xmlCountSiblings(tinyxml2::XMLElement*, const char*);

	BString	title;
	BDateTime date;
	BDateTime lastDate;
	BUrl	xmlUrl;
	BString	cachePath;
	BString hash;
	BString lastHash;

	BObjectList<Entry> entries;
	bool	fetched;
	bool	updated;
};


#endif

