/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef ATOM_FEED_H
#define ATOM_FEED_H


#include <tinyxml2.h>

#include "Feed.h"


class AtomFeed: public Feed {
public:
	AtomFeed();
	AtomFeed(Feed*);

	void Parse();
	void RootParse(tinyxml2::XMLElement*);
	void EntryParse(tinyxml2::XMLElement*);
	void ParseEntries(tinyxml2::XMLElement*);
};


#endif

