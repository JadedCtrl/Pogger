/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "FeedsView.h"

#include <Alert.h>
#include <Catalog.h>
#include <Message.h>
#include <GroupView.h>
#include <LayoutBuilder.h>
#include <ListView.h>
#include <ScrollView.h>
#include <SeparatorView.h>
#include <StringList.h>
#include <StringView.h>

#include "App.h"
#include "Feed.h"
#include "FeedController.h"
#include "FeedEditWindow.h"
#include "FeedListItem.h"
#include "Notifier.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "FeedsView"


FeedsView::FeedsView(const char* name)
	:
	BGroupView(name, B_VERTICAL, B_USE_DEFAULT_SPACING)
{
	_InitInterface();
}


void
FeedsView::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case kFeedsAddButton:
		{
			FeedEditWindow* edit = new FeedEditWindow();
			edit->Show();
			edit->Activate();
			break;
		}
		case kFeedsRemoveButton:
		{
			_RemoveSelectedFeed();
			_PopulateFeedList();
			break;
		}
		case kFeedsEditButton:
		{
			if (fFeedsListView->CurrentSelection() >= 0)
				_EditSelectedFeed();
			break;
		}
		case kFeedsSelected:
		{
			bool enabled = msg->GetInt32("index", -1) >= 0;
			fEditButton->SetEnabled(enabled);
			fRemoveButton->SetEnabled(enabled);
			break;
		}
		case kFeedsEdited:
		{
			_PopulateFeedList();
		}
		case kDownloadStart:
		{
			_UpdateProgress(msg, kDownloadingStatus);
			break;
		}
		case kDownloadComplete:
		{
			_UpdateProgress(msg, kParsingStatus);
			break;
		}
		case kDownloadFail:
		case kParseFail:
		{
			_UpdateProgress(msg, kErrorStatus);
			break;
		}
		case kParseComplete:
		{
			_UpdateProgress(msg, kClearStatus);
			break;
		}
		case kProgress:
		{
			int32 total,current = 0;

			if (msg->FindInt32("total", &total) == B_OK
				&& msg->FindInt32("current", &current) == B_OK)
			{
				if (total == current) {
					fProgressLabel->SetText("");
					_PopulateFeedList();
				}
			}
			break;
		}
	}
}


void
FeedsView::_InitInterface()
{
	// Feeds list
	fFeedsListView = new BListView("feedsList");
	fFeedsScrollView = new BScrollView("feedsScroll", fFeedsListView,
		B_WILL_DRAW, false, true);
	fFeedsListView->SetSelectionMessage(new BMessage(kFeedsSelected));
	fFeedsListView->SetInvocationMessage(new BMessage(kFeedsEditButton));

	_PopulateFeedList();

	fProgressLabel = new BStringView("progressLabel", "");

	// Add, Remove, Edit
	fAddButton = new BButton("addFeed", "+", new BMessage(kFeedsAddButton));
	fRemoveButton = new BButton("removeFeed", "-", new BMessage(kFeedsRemoveButton));
	fAddButton->SetToolTip(B_TRANSLATE("Add new feed"));
	fRemoveButton->SetToolTip(B_TRANSLATE("Remove selected feed"));
	fEditButton = new BButton("editFeed", B_TRANSLATE("Edit…"),
		new BMessage(kFeedsEditButton));

	font_height fontHeight;
	GetFontHeight(&fontHeight);
	int16 buttonHeight = int16(fontHeight.ascent + fontHeight.descent + 12);
	BSize charButtonSize(buttonHeight, buttonHeight);

	fAddButton->SetExplicitSize(charButtonSize);
	fAddButton->SetEnabled(true);
	fRemoveButton->SetExplicitSize(charButtonSize);
	fRemoveButton->SetEnabled(false);
	fEditButton->SetExplicitSize(
		BSize(fEditButton->ExplicitPreferredSize().Width(),
			charButtonSize.Height()));
	fEditButton->SetEnabled(false);


	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.SetInsets(B_USE_DEFAULT_SPACING)
		.Add(fFeedsScrollView)

		.AddGroup(B_HORIZONTAL, 0, 0.0)

			// Add and Remove buttons
			.Add(new BSeparatorView(B_VERTICAL))
			.AddGroup(B_VERTICAL, 0, 0.0)
				.AddGroup(B_HORIZONTAL, 1, 0.0)
					.SetInsets(1)
					.Add(fAddButton)
					.Add(fRemoveButton)
				.End()
				.Add(new BSeparatorView(B_HORIZONTAL))
			.End()
			.Add(new BSeparatorView(B_VERTICAL))

			.AddGlue()
			.Add(fProgressLabel)
			.AddGlue()

			// Edit button
			.Add(new BSeparatorView(B_VERTICAL))
			.AddGroup(B_VERTICAL, 0, 0.0)
				.AddGroup(B_HORIZONTAL, 1, 0.0)
					.SetInsets(1)
					.Add(fEditButton)
				.End()
				.Add(new BSeparatorView(B_HORIZONTAL))
			.End()
			.Add(new BSeparatorView(B_VERTICAL))
		.End();
}


void
FeedsView::_EditSelectedFeed()
{
	int32 selIndex = fFeedsListView->CurrentSelection();
	FeedListItem* selected = (FeedListItem*)fFeedsListView->ItemAt(selIndex);
	FeedEditWindow* edit = new FeedEditWindow(selected);

	edit->Show();
	edit->Activate();
}


void
FeedsView::_RemoveSelectedFeed()
{
	BAlert* alert = new BAlert(B_TRANSLATE("Confirm removal"),
		B_TRANSLATE("Are you sure you want to remove the selected feed?"),
		NULL, B_TRANSLATE("OK"), B_TRANSLATE("Cancel"),
		B_WIDTH_AS_USUAL, B_WARNING_ALERT);

	alert->SetShortcut(1, B_ESCAPE);
	int32 button = alert->Go();
	if (button != 0)
		return;

	int32 selIndex = fFeedsListView->CurrentSelection();
	FeedListItem* selected = (FeedListItem*)fFeedsListView->ItemAt(selIndex);
	Feed delFeed = Feed(BEntry(selected->FeedPath()));

	delFeed.Unfiletize();
}


void
FeedsView::_PopulateFeedList()
{
	BStringList feeds = FeedController::SubscribedFeeds();
	int32 selected = fFeedsListView->CurrentSelection();

	for (int i = fFeedsListView->CountItems(); i >= 0; i--)
		delete ((FeedListItem*)fFeedsListView->RemoveItem(i));

	for (int i = 0; i < feeds.CountStrings(); i++) {
		Feed feed = Feed(feeds.StringAt(i).String());
		FeedListItem* item = new FeedListItem(&feed);
		fFeedsListView->AddItem(item);
	}

	if (fFeedsListView->CountItems() < selected)
		selected = fFeedsListView->CountItems();
	fFeedsListView->Select(selected);
}


void
FeedsView::_UpdateProgress(BMessage* msg, int8 status)
{
	BString feedName, feedUrl;
	if (msg->FindString("feed_url", &feedUrl) != B_OK)
		return;
	if (msg->FindString("feed_name", &feedName) != B_OK || feedName.IsEmpty())
		feedName = feedUrl;

	if (status == kDownloadingStatus) {
		BString label(B_TRANSLATE("Fetching %source%…"));
		label.ReplaceAll("%source%", feedName);
		fProgressLabel->SetText(label);
	}

	for (int i = 0; i < fFeedsListView->CountItems(); i++) {
		FeedListItem* item  = (FeedListItem*)fFeedsListView->ItemAt(i);
		if (item->FeedUrl().UrlString() == feedUrl) {
			item->SetStatus(status);
			fFeedsListView->InvalidateItem(i);
		}
	}
}


