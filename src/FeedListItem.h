/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef FEEDITEM_H
#define FEEDITEM_H

#include <String.h>
#include <StringItem.h>
#include <Url.h>

//class BString;
//class BUrl;
class Feed;


class FeedListItem : public BStringItem
{
public:
	FeedListItem(Feed* feed);

private:
	BUrl fFeedUrl;
	BString fFeedPath;
};


#endif // FEEDITEM_H

