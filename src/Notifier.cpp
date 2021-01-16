/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include <Message.h>
#include <Notification.h>

#include "FeedController.h"

#include "Notifier.h"


void
Notifier::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
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
			break;
		}
		case kParseFail:
		{
			BString feedUrl = msg->GetString("feed_url", "");
			_ParseFailNotification(feedUrl);
			break;
		}
		case kDownloadFail:
		{
			BString feedUrl = msg->GetString("feed_url", "");
			_DownloadFailNotification(feedUrl);
		}
	}
}


void
Notifier::_NewEntryNotification(BString feedName, int32 entryCount)
{
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
	BNotification notifyError(B_ERROR_NOTIFICATION);
	BString notifyText("Failed to download feed from %url%");

	if (feedUrl.IsEmpty())
		notifyText = "Failed to download a feed";

	notifyText.ReplaceAll("%url%", feedUrl);

	notifyError.SetTitle("Download Failure");
	notifyError.SetContent(notifyText);
	notifyError.Send();
}


