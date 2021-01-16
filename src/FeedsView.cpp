/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "FeedsView.h"

#include <Message.h>
#include <GroupView.h>
#include <LayoutBuilder.h>
#include <ListView.h>
#include <ScrollView.h>
#include <SeparatorView.h>

#include <cstdio>

#include "App.h"
#include "Feed.h"
#include "FeedController.h"
#include "FeedEditWindow.h"
#include "FeedListItem.h"


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
			break;
		}
		case kFeedsEditButton:
		{
			_EditSelectedFeed();
			break;
		}
		case kFeedsSelected:
		{
			fEditButton->SetEnabled(true);
			fRemoveButton->SetEnabled(true);
			break;
		}
		case kFeedsEdited:
		{
			break;
		}
		default:
		{
//			BWindow::MessageReceived(msg);
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

	BList feeds = FeedController::SubscribedFeeds();
	for (int i = 0; i < feeds.CountItems(); i++) {
		FeedListItem* item = new FeedListItem((Feed*)feeds.ItemAt(i));
		fFeedsListView->AddItem(item);
	}

	// Add, Remove, Edit
	fAddButton = new BButton("addFeed", "+", new BMessage(kFeedsAddButton));
	fRemoveButton = new BButton("removeFeed", "-", new BMessage(kFeedsRemoveButton));
	fEditButton = new BButton("editFeed", "Edit…", new BMessage(kFeedsEditButton));

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
	int32 selIndex = fFeedsListView->CurrentSelection();
	FeedListItem* selected = (FeedListItem*)fFeedsListView->ItemAt(selIndex);
	Feed delFeed = Feed(BEntry(selected->GetFeedPath()));

	delFeed.Unfiletize();
}


