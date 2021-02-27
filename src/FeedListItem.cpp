/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "FeedListItem.h"

#include <View.h>

#include "Feed.h"


FeedListItem::FeedListItem(Feed* feed)
	:
	BStringItem(feed->GetTitle().String(), 0, false),
	fStatus(kClearStatus),
	fFeedUrl(feed->GetXmlUrl()),
	fFeedPath(feed->GetCachePath())
{
}


void
FeedListItem::DrawItem(BView* owner, BRect frame, bool complete)
{
	BStringItem::DrawItem(owner, frame, complete);

	owner->MovePenTo(frame.right - 20, frame.top + BaselineOffset());

	switch (fStatus) {
		case kDownloadingStatus:
		{
			owner->DrawString("…");
			break;
		}
		case kParsingStatus:
		{
			owner->DrawString("―");
			break;
		}
		case kErrorStatus:
		{
			owner->DrawString("X");
			break;
		}
	}
}


BString
FeedListItem::GetFeedPath()
{
	return fFeedPath;
}


BUrl
FeedListItem::GetFeedUrl()
{
	return fFeedUrl;
}


void
FeedListItem::SetStatus(int8 status)
{
	fStatus = status;
}


