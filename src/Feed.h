/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef FEED_H
#define FEED_H

#include <ObjectList.h>

#include "Entry.h"


class BDateTime;
class BEntry;
class BString;
class BUrl;


class Feed {
public:
	Feed();
	Feed(const char* identifier, const char* source, const char* title,
		const char* url);
	Feed(Feed*);

	BObjectList<Entry>	Entries();
	BObjectList<Entry>	NewEntries();

	void	SetEntries(BObjectList<Entry> entries);

	BString	Title();
	bool	SetTitle(const char*);

	BUrl	Url();
	bool	SetUrl(BUrl newUrl);

	BDateTime Date();
	bool	SetDate(BDateTime);

	BDateTime LastDate();
	bool	SetLastDate(BDateTime);

	BString	Hash();
	bool	SetHash(BString hash);

	BString	LastHash();
	bool	SetLastHash(BString hash);

	const char*	Identifier();
	bool		SetIdentifier(const char* id);

	const char*	Source();
	bool		SetSource(const char* source);

protected:
	BString	fTitle;
	BDateTime fDate;		// Lastest date (of post/update) given by feed
	BDateTime fLastDate;	// Last time feed was parsed
	BUrl	fUrl;
	BString	fIdentifier;
	BString	fSource;
	BString fHash;
	BString fLastHash;

	BObjectList<Entry> fEntries;
};


#endif

