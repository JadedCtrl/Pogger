/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "FeedListItem.h"

#include "Feed.h"


FeedListItem::FeedListItem(Feed* feed)
	:
	BStringItem(feed->GetTitle().String(), 0, false),
	fFeedUrl(feed->GetXmlUrl()),
	fFeedPath(feed->GetCachePath())
{
}


