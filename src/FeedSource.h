/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef FEEDSOURCE_H
#define FEEDSOURCE_H

#include <StringList.h>

#include "Feed.h"


// Hierarchy: FeedSource → Feed → Entry
// TODO: RemoveFeed(), AddFeed(), etc.
class FeedSource {
public:
	static BStringList FeedIdentifiers();

	Feed RemoveFeed(Feed feed);
	Feed RemoveFeed(const char* identifier);
};


#endif // FEEDSOURCE_H

