/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "Notifier.h"

#include <List.h>
#include <Message.h>
#include <Notification.h>

#include "App.h"
#include "FeedController.h"


Notifier::Notifier()
	:
	fFailedFeeds(new BList()),
	fUpdatedFeeds(new BList()),
	fTotalEntries(0)
{
}


void
Notifier::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case kProgress:
		{
			int32 total, current = 0;

			if (msg->FindInt32("total", &total) == B_OK
				&& msg->FindInt32("current", &current) == B_OK
				&& total == current)
			{
				_SendNotifications();
			}
			break;
		}
		case kParseComplete:
		{
			BString feedName;
			BString feedUrl;
			int32 entryCount;

			if ((msg->FindString("feed_name", &feedName) == B_OK
					 || msg->FindString("feed_url", &feedUrl) == B_OK)
				&& msg->FindInt32("entry_count", &entryCount) == B_OK)
			{
				if (entryCount > 0)
					_SaveUpdated(feedName, feedUrl, entryCount);
			}
			break;
		}
		case kDownloadFail:
		case kParseFail:
		{
			BString feedName;
			BString feedUrl;

			if (msg->FindString("feed_name", &feedName) == B_OK
				|| msg->FindString("feed_url", &feedUrl) == B_OK)
			{
				_SaveFailed(feedName, feedUrl);
			}
			break;
		}
	}
}


void
Notifier::_SendNotifications()
{
	_SendUpdatedNotification();
	_SendFailedNotification();

	fFailedFeeds->MakeEmpty();
	fUpdatedFeeds->MakeEmpty();
	fTotalEntries = 0;
}


void
Notifier::_SendUpdatedNotification()
{
	if (((App*)be_app)->fPreferences->fNewNotify == false
		|| fTotalEntries == 0) {
		return;
	}

	BNotification notifyNew(B_INFORMATION_NOTIFICATION);
	BString notifyLabel("Feed Updates");
	BString notifyText("%n% new entries from %source%");

	if (fUpdatedFeeds->CountItems() > 1) 
		notifyText = "%n% new entries from %source% and %m% others";

	BString entryNum,feedNum = "";
	entryNum << fTotalEntries;
	feedNum << fUpdatedFeeds->CountItems();

	notifyText.ReplaceAll("%n%", entryNum);
	notifyText.ReplaceAll("%m%", feedNum);
	notifyText.ReplaceAll("%source%",
		((BString*)fUpdatedFeeds->ItemAt(0))->String());

	notifyNew.SetTitle(notifyLabel);
	notifyNew.SetContent(notifyText);
	notifyNew.Send();
}


void
Notifier::_SendFailedNotification()
{
	if (((App*)be_app)->fPreferences->fFailureNotify == false
		|| fFailedFeeds->CountItems() == 0) {
		return;
	}

	BNotification notifyError(B_ERROR_NOTIFICATION);
	BString notifyLabel("Update Failure");
	BString notifyText("Failed to update %source%");

	if (fFailedFeeds->CountItems() > 1) 
		notifyText = "Failed to update %source% and %m% others";

	BString feedNum = "";
	feedNum << fFailedFeeds->CountItems();

	notifyText.ReplaceAll("%m%", feedNum);
	notifyText.ReplaceAll("%source%",
		((BString*)fFailedFeeds->ItemAt(0))->String());

	notifyError.SetTitle(notifyLabel);
	notifyError.SetContent(notifyText);
	notifyError.Send();
}


void
Notifier::_SaveUpdated(BString feedName, BString feedUrl, int32 entryCount)
{
	BString name = feedName;
	if (feedName.IsEmpty())
		name = feedUrl;

	fTotalEntries += entryCount;
	fUpdatedFeeds->AddItem(new BString(feedName));
}


void
Notifier::_SaveFailed(BString feedName, BString feedUrl)
{
	BString name = feedName;
	if (feedName.IsEmpty())
		name = feedUrl;

	fFailedFeeds->AddItem(new BString(feedName));
}


