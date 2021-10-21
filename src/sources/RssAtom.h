/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef RSSATOM_H
#define RSSATOM_H

#include <tinyxml2.h>

#include <Path.h>

#include "../Source.h"

class Entry;


class RssAtom : public Source {
public:
	RssAtom(const char* configPath);

	const char* Type();
	const char* Name();
	const char* Config();

	BObjectList<Feed> Feeds();

	Feed* GetFeed(const char* identifier);

	bool Fetch(Feed* feed);
	bool Parse(Feed* feed);

	void AddFeed(Feed* newFeed);
	void EditFeed(Feed* updated);
	void RemoveFeed(Feed* mortonta);

	bool IsUpdated(Feed* feed);

	BString fConfigPath;

private:
	bool _IsAtom(Feed* feed);
	bool _IsRss(Feed* feed);

	bool	_AtomParse(Feed* feed);
	void	_AtomRootParse(Feed* feed, tinyxml2::XMLElement*);
	void	_AtomEntriesParse(Feed* feed, tinyxml2::XMLElement*);
	Entry*	_AtomEntryParse(Feed* feed, tinyxml2::XMLElement*);

	bool	_RssParse(Feed* feed);
	void	_RssRootParse(Feed* feed, tinyxml2::XMLElement*);
	void	_RssEntriesParse(Feed* feed, tinyxml2::XMLElement*);
	Entry*	_RssEntryParse(Feed* feed, tinyxml2::XMLElement*);

	// These _Sets could probably be better served as a macro :P
	bool _SetTitle(Feed* feed, tinyxml2::XMLElement*);
	bool _SetTitle(Entry* entry, tinyxml2::XMLElement*);

	bool _SetDate(Feed* feed, const char*);
	bool _SetDate(Feed* feed, tinyxml2::XMLElement*);
	bool _SetDate(Entry* entry, const char*);
	bool _SetDate(Entry* entry, tinyxml2::XMLElement*);

	bool _SetDescription(Entry* entry, tinyxml2::XMLElement*);
	bool _SetContent(Entry* entry, tinyxml2::XMLElement*);

	bool _SetPostUrl(Entry* entry, tinyxml2::XMLElement*);

	int _XmlCountSiblings(tinyxml2::XMLElement*, const char*);

	void _LoadConfig();
	void _CreateConfig();
	void _EnsureSubscriptions();

	BPath _SubscriptionPath();

	BString fTitle;
};


#endif // RSSATOM_H

