/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "Notifier.h"

#include <Message.h>
#include <Notification.h>

#include "App.h"
#include "FeedController.h"


Notifier::Notifier()
	:
	fEnqueuedFeeds(0),
	fFeedsInProgress(0)
{
}


void
Notifier::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case kEnqueueFeed:
		{
			fEnqueuedFeeds++;
			fFeedsInProgress++;
			_UpdateProgress();
			break;
		}
		case kParseComplete:
		{
			BString feedName;
			int32 entryCount;

			if (msg->FindString("feed_name", &feedName) == B_OK
				&& msg->FindInt32("entry_count", &entryCount) == B_OK)
			{
				if (entryCount > 0)
					_NewEntryNotification(feedName, entryCount);
			}

			fFeedsInProgress--;
			_UpdateProgress();
			break;
		}
		case kParseFail:
		{
			BString feedUrl = msg->GetString("feed_url", "");
			_ParseFailNotification(feedUrl);

			fFeedsInProgress--;
			_UpdateProgress();
			break;
		}
		case kDownloadFail:
		{
			BString feedUrl = msg->GetString("feed_url", "");
			_DownloadFailNotification(feedUrl);

			fFeedsInProgress--;
			_UpdateProgress();
			break;
		}
	}
}


void
Notifier::_NewEntryNotification(BString feedName, int32 entryCount)
{
	if (((App*)be_app)->fPreferences->fNewNotify == false)
		return;

	BNotification notifyNew(B_INFORMATION_NOTIFICATION);
	BString notifyLabel("New Feed Entries");
	BString notifyText("%n% new entries from %source%");

	BString numStr("");
	numStr << entryCount;
	notifyText.ReplaceAll("%source%", feedName);
	notifyText.ReplaceAll("%n%", numStr);

	notifyNew.SetTitle(notifyLabel);
	notifyNew.SetContent(notifyText);
	notifyNew.Send();
}


void
Notifier::_ParseFailNotification(BString feedUrl)
{
	if (((App*)be_app)->fPreferences->fFailureNotify == false)
		return;

	BNotification notifyError(B_ERROR_NOTIFICATION);
	BString notifyText("Failed to parse feed from %url%");

	if (feedUrl.IsEmpty())
		notifyText = "Failed to parse a feed";

	notifyText.ReplaceAll("%url%", feedUrl);

	notifyError.SetTitle("Parse Failure");
	notifyError.SetContent(notifyText);
	notifyError.Send();
}


void
Notifier::_DownloadFailNotification(BString feedUrl)
{
	if (((App*)be_app)->fPreferences->fFailureNotify == false)
		return;

	BNotification notifyError(B_ERROR_NOTIFICATION);
	BString notifyText("Failed to download feed from %url%");

	if (feedUrl.IsEmpty())
		notifyText = "Failed to download a feed";

	notifyText.ReplaceAll("%url%", feedUrl);

	notifyError.SetTitle("Download Failure");
	notifyError.SetContent(notifyText);
	notifyError.Send();
}


void
Notifier::_UpdateProgress()
{
	BMessage progress = BMessage(kProgress);
	progress.AddInt32("max", fEnqueuedFeeds);
	progress.AddInt32("current", fFeedsInProgress);

	((App*)be_app)->MessageReceived(&progress);

	if (fFeedsInProgress == 0)
		fEnqueuedFeeds = 0;
}


