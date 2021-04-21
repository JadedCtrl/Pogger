/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef FEEDSOURCE_H
#define FEEDSOURCE_H

#include <StringList.h>


// Hierarchy: FeedSource → Feed → Entry
// TODO: RemoveFeed(), AddFeed(), etc.
class FeedSource {
public:
	BStringList Feeds();

	const char* name = "";
};


#endif // FEEDSOURCE_H

