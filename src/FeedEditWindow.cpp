/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "FeedEditWindow.h"

#include <Alert.h>
#include <Button.h>
#include <Catalog.h>
#include <Message.h>
#include <LayoutBuilder.h>
#include <SeparatorView.h>
#include <StringView.h>
#include <TextControl.h>

#include "App.h"
#include "FeedController.h"
#include "FeedListItem.h"
#include "SourceManager.h"
#include "FeedsView.h"
#include "Util.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "FeedEditWindow"


FeedEditWindow::FeedEditWindow()
	:
	BWindow(((App*)be_app)->fPreferences->fFeedEditRect,
		B_TRANSLATE("New Feed"), B_TITLED_WINDOW,
		B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
{
	_InitInterface();
	MoveOnScreen();
	fFeed = new Feed();
}


FeedEditWindow::FeedEditWindow(const char* identifier, const char* source)
	: FeedEditWindow()
{
	SetTitle(B_TRANSLATE("Edit feed"));
	fFeed = SourceManager::GetFeed(identifier, source);

	fFeedNameText->SetText(fFeed->Title().String());
	fFeedUrlText->SetText(fFeed->Url().UrlString().String());
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
		case kOkButton:
		{
			_SaveFeed();
			break;
		}
		case kCancelButton:
		{
			Quit();
			break;
		}
	}
}


void
FeedEditWindow::_InitInterface()
{
	// Name and URL
	fFeedNameLabel = new BStringView("feedNameLabel", B_TRANSLATE("Feed name:"));
	fFeedNameText = new BTextControl("feedName", "", "", NULL);
	fFeedUrlLabel = new BStringView("feedUrlLabel", B_TRANSLATE("Feed URL:"));
	fFeedUrlText = new BTextControl("feedUrl", "", "", NULL);

	// OK/Cancel
	fOkButton = new BButton("ok", B_TRANSLATE("OK"),
		new BMessage(kOkButton));
	fCancelButton = new BButton("cancel", B_TRANSLATE("Cancel"),
		new BMessage(kCancelButton));

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
			.Add(fCancelButton)
			.AddGlue()
			.Add(fOkButton)
			.SetInsets(B_USE_WINDOW_SPACING, B_USE_DEFAULT_SPACING,
				B_USE_DEFAULT_SPACING, B_USE_WINDOW_SPACING)
		.End()
	.End();
}


void
FeedEditWindow::_SaveFeed()
{
	BString title(fFeedNameText->Text());
	const char* urlString = fFeedUrlText->Text();
	BUrl url = BUrl(urlString);

	if (BString(urlString).IsEmpty() == true) {
		BAlert* emptyAlert = new BAlert(B_TRANSLATE("Invalid feed"),
			B_TRANSLATE("Please enter a URL."), B_TRANSLATE("OK"), NULL, NULL,
			B_WIDTH_AS_USUAL, B_OFFSET_SPACING, B_WARNING_ALERT);
		emptyAlert->Go();
		return;
	}

	if (url.IsValid() == false) {
		BAlert* invAlert = new BAlert(B_TRANSLATE("Invalid feed"),
			B_TRANSLATE("The given URL is invalid. Please make sure you typed "
			"it in correctly."), B_TRANSLATE("OK"), NULL, NULL,
			B_WIDTH_AS_USUAL, B_OFFSET_SPACING, B_WARNING_ALERT);
		invAlert->Go();
		return;
	}

	if (!title.IsEmpty())
		fFeed->SetTitle(title.String());
	fFeed->SetUrl(BUrl(urlString));

	if (BString(fFeed->Identifier()).IsEmpty() == true)
		SourceManager::AddFeed(fFeed);
	else
		SourceManager::EditFeed(fFeed);

	BMessage edited(kFeedsEdited);
	BMessage enqueueUpdated(kEnqueueFeed);
	enqueueUpdated.AddString("feed_identifiers", fFeed->Identifier());
	enqueueUpdated.AddString("feed_sources", fFeed->Source());

	((App*)be_app)->MessageReceived(&enqueueUpdated);
	((App*)be_app)->PostMessage(&edited);
	Quit();
}


