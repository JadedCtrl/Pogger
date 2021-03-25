/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "Notifier.h"

#include <Catalog.h>
#include <List.h>
#include <Message.h>
#include <StringFormat.h>
#include <Notification.h>

#include "App.h"
#include "FeedController.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Notifier"


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
	BString notifyLabel(B_TRANSLATE("Feed updates"));
	BString notifyText;

	static BStringFormat oneSourceFormat(B_TRANSLATE("{0, plural,"
	"=1{One new entry from %source%.}"
	"other{# new entries from %source%.}}"));

	static BStringFormat multiSourceFormat(B_TRANSLATE("{0, plural,"
	"=1{%n% new entries from %source% and one other.}"
	"other{%n% new entries from %source% and # others.}}"));

	if (fUpdatedFeeds->CountItems() > 1)
		multiSourceFormat.Format(notifyText, fUpdatedFeeds->CountItems() - 1);
	else
		oneSourceFormat.Format(notifyText, fTotalEntries);

	BString entryNum = "";
	entryNum << fTotalEntries;

	BString* feedTitle = (BString*)fUpdatedFeeds->ItemAt(0);
	if (feedTitle->IsEmpty())
		feedTitle->SetTo(B_TRANSLATE("Untitled feed"));

	notifyText.ReplaceAll("%n%", entryNum);
	notifyText.ReplaceAll("%source%", feedTitle->String());

	notifyNew.SetTitle(notifyLabel);
	notifyNew.SetContent(notifyText);

	entry_ref feedsRef;
	BEntry(((App*)be_app)->fPreferences->EntryDir()).GetRef(&feedsRef);
	notifyNew.SetOnClickFile(&feedsRef);

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
	BString notifyLabel(B_TRANSLATE("Update failure"));
	BString notifyText;

	static BStringFormat multiSourceFormat(B_TRANSLATE("{0, plural,"
	"=1{Failed to update %source% and one other.}"
	"other{Failed to update %source% and # others.}}"));

	if (fFailedFeeds->CountItems() > 1) 
		multiSourceFormat.Format(notifyText, fFailedFeeds->CountItems() - 1);
	else
		notifyText = B_TRANSLATE("Failed to update %source%.");

	BString* feedTitle = (BString*)fFailedFeeds->ItemAt(0);
	if (feedTitle->IsEmpty())
		feedTitle->SetTo(B_TRANSLATE("Untitled feed"));

	notifyText.ReplaceAll("%source%", feedTitle->String());

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


