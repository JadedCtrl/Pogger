/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef FEEDITEM_H
#define FEEDITEM_H

#include <String.h>
#include <StringItem.h>
#include <Url.h>

class Feed;


enum
{
	kClearStatus = 0,
	kDownloadingStatus = 1,
	kParsingStatus = 2,
	kErrorStatus = 3
};


class FeedListItem : public BStringItem
{
public:
	FeedListItem(Feed* feed);

	void DrawItem(BView* owner, BRect frame, bool complete);

	BUrl FeedUrl();
	const char* FeedIdentifier();
	const char* FeedSource();
	Feed* GetFeed();

	void SetStatus(int8 status);

private:
	int8 fStatus;
	BUrl fFeedUrl;
	BString fFeedIdentifier;
	BString fFeedSource;
	Feed* fFeed;
};


#endif // FEEDITEM_H

