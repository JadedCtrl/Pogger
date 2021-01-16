/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef FEED_H
#define FEED_H


#include <tinyxml2.h>

#include "Entry.h"


class BDateTime;
class BEntry;
class BString;
class BList;
class BUrl;


class Feed {
public:
	Feed(BUrl);
	Feed(BUrl, BString);
	Feed(BEntry);
	Feed(BUrl xml, BEntry entry);
	Feed(Feed*);
	Feed();

	virtual void Parse();

	BList	GetEntries();
	BList	GetNewEntries();

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

	BList	entries;
	bool	fetched;
	bool	updated;
};


#endif

