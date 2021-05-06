/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef LOCALSOURCE_H
#define LOCALSOURCE_H

#include "FeedSource.h"

class LocalSource : public FeedSource {
public:
	LocalSource();

	static BObjectList<Feed> Feeds();

	static Feed* GetFeed(const char* identifier);

	static bool Fetch(Feed* feed);
	static bool Parse(Feed* feed);

	static void AddFeed(Feed* newFeed);
	static void EditFeed(Feed* updated);
	static void RemoveFeed(Feed* mortonta);

	static bool IsUpdated(Feed* feed);

private:
	static bool _IsAtom(Feed* feed);
	static bool _IsRss(Feed* feed);

	static bool		_AtomParse(Feed* feed);
	static void		_AtomRootParse(Feed* feed, tinyxml2::XMLElement*);
	static void		_AtomEntriesParse(Feed* feed, tinyxml2::XMLElement*);
	static Entry*	_AtomEntryParse(Feed* feed, tinyxml2::XMLElement*);

	static bool		_RssParse(Feed* feed);
	static void		_RssRootParse(Feed* feed, tinyxml2::XMLElement*);
	static void		_RssEntriesParse(Feed* feed, tinyxml2::XMLElement*);
	static Entry*	_RssEntryParse(Feed* feed, tinyxml2::XMLElement*);

	static bool _SetTitle(Feed* feed, tinyxml2::XMLElement*);

	static bool _SetDate(Feed* feed, const char*);
	static bool _SetDate(Feed* feed, tinyxml2::XMLElement*);
	static bool _SetDate(Entry* entry, const char*);
	static bool _SetDate(Entry* entry, tinyxml2::XMLElement*);

	static int _XmlCountSiblings(tinyxml2::XMLElement*, const char*);
	static BPath _SubscriptionPath();
};


#endif // LOCALSOURCE_H

