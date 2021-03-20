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
	Feed(const char* pathStr);
	Feed(BUrl xml, BEntry entry);
	Feed(Feed*);
	Feed();

	~Feed();

	virtual void Parse();

	BObjectList<Entry>	Entries();
	BObjectList<Entry>	NewEntries();

	bool	Fetch();

	void	Filetize();
	void	Unfiletize();

	bool	IsRss();
	bool	IsAtom();
	bool	IsUpdated();

	BString	Title();
	bool	SetDate(BDateTime);

	BUrl	XmlUrl();
	BDateTime Date();

	bool	SetTitle(const char*);
	bool	SetXmlUrl(BUrl newUrl);
	bool	SetCachePath(BString path);

	BString	CachePath();

protected:
	bool	_SetTitle(tinyxml2::XMLElement*);
	bool	_SetDate(const char*);
	bool	_SetDate(tinyxml2::XMLElement*);
	bool	_SetDate(BDateTime newDate);

	bool	_AddEntry(Entry*);

	int		_XmlCountSiblings(tinyxml2::XMLElement*, const char*);

	BString	fTitle;
	BDateTime fDate;
	BDateTime fLastDate;
	BUrl	fXmlUrl;
	BString	fCachePath;
	BString fHash;
	BString fLastHash;

	BObjectList<Entry> fEntries;
};


#endif

