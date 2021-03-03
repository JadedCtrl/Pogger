/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "FeedEditWindow.h"

#include <Button.h>
#include <Message.h>
#include <LayoutBuilder.h>
#include <SeparatorView.h>
#include <StringView.h>
#include <TextControl.h>

#include "App.h"
#include "Feed.h"
#include "FeedController.h"
#include "FeedListItem.h"
#include "FeedsView.h"


FeedEditWindow::FeedEditWindow()
	:
	BWindow(((App*)be_app)->fPreferences->fFeedEditRect, "New Feed",
		B_TITLED_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
{
	_InitInterface();
	MoveOnScreen();
	fDeleteButton->SetEnabled(false);

	fFeed = new Feed();
}


FeedEditWindow::FeedEditWindow(BEntry feedEntry)
	:
	FeedEditWindow()
{
	SetTitle("Edit Feed");
	fFeed = new Feed(feedEntry);

	fFeedNameText->SetText(fFeed->GetTitle().String());
	fFeedUrlText->SetText(fFeed->GetXmlUrl().UrlString().String());

	fDeleteButton->SetEnabled(true);
}


FeedEditWindow::FeedEditWindow(FeedListItem* feedItem)
	:
	FeedEditWindow(BEntry(feedItem->GetFeedPath()))
{
}


FeedEditWindow::~FeedEditWindow()
{
	((App*)be_app)->fPreferences->fFeedEditRect = ConvertToScreen(Bounds());
}


void
FeedEditWindow::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case kSaveButton:
		{
			_SaveFeed();
			break;
		}
		case kDeleteButton:
		{
			_DeleteFeed();
			break;
		}
	}
}


void
FeedEditWindow::_InitInterface()
{
	// Name and URL
	fFeedNameLabel = new BStringView("feedNameLabel", "Feed name:");
	fFeedNameText = new BTextControl("feedName", "", "", NULL);
	fFeedUrlLabel = new BStringView("feedUrlLabel", "Feed URL:");
	fFeedUrlText = new BTextControl("feedUrl", "", "", NULL);

	// Save/Delete
	fSaveButton = new BButton("save", "Save",
		new BMessage(kSaveButton));
	fDeleteButton = new BButton("delete", "Delete",
		new BMessage(kDeleteButton));

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.SetInsets(0, B_USE_DEFAULT_SPACING, 0, 0)

		.AddGroup(B_HORIZONTAL, B_USE_DEFAULT_SPACING)
			.AddGroup(B_VERTICAL, B_USE_DEFAULT_SPACING)
				.SetInsets(B_USE_ITEM_INSETS, 0, 0, B_USE_ITEM_INSETS)
				.Add(fFeedNameLabel)
				.Add(fFeedUrlLabel)
			.End()
			.AddGroup(B_VERTICAL, B_USE_DEFAULT_SPACING)
				.SetInsets(B_USE_ITEM_INSETS, 0, B_USE_ITEM_INSETS,
					B_USE_ITEM_INSETS)
				.Add(fFeedNameText)
				.Add(fFeedUrlText)
			.End()
		.End()

		.Add(new BSeparatorView(B_HORIZONTAL))
		.AddGroup(B_HORIZONTAL)
			.AddGlue()
			.Add(fDeleteButton)
			.Add(fSaveButton)
			.SetInsets(B_USE_WINDOW_SPACING, B_USE_DEFAULT_SPACING,
				B_USE_DEFAULT_SPACING, B_USE_WINDOW_SPACING)
		.End()
	.End();
}


void
FeedEditWindow::_SaveFeed()
{
	BString subLocation("/boot/home/config/settings/Pogger/Subscriptions/");
	BString title(fFeedNameText->Text());
	const char* urlString = fFeedUrlText->Text();
	BString filename;
	if (title.IsEmpty())
		filename = BString(urlString);
	else
		filename = BString(title);
	subLocation.Append(filename);

	if (fFeed->GetCachePath().IsEmpty())
		fFeed->SetCachePath(subLocation);

	if (!title.IsEmpty())
		fFeed->SetTitle(title.String());
	fFeed->SetXmlUrl(BUrl(urlString));
	fFeed->Filetize();

	BMessage edited(kFeedsEdited);
	BMessage enqueueUpdated(kEnqueueFeed);
	enqueueUpdated.AddData("feeds", B_RAW_TYPE, (void*)fFeed, sizeof(Feed));

	((App*)be_app)->MessageReceived(&enqueueUpdated);
	((App*)be_app)->PostMessage(&edited);
	Quit();
}


void
FeedEditWindow::_DeleteFeed()
{
	fFeed->Unfiletize();
	BMessage edited(kFeedsEdited);
	((App*)be_app)->PostMessage(&edited);
	Quit();
}


