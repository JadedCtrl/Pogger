/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "MainWindow.h"

#include <Button.h>
#include <Catalog.h>
#include <GroupView.h>
#include <LayoutBuilder.h>
#include <SeparatorView.h>
#include <StatusBar.h>
#include <String.h>
#include <TabView.h>

#include "App.h"
#include "EntriesView.h"
#include "FeedController.h"
#include "FeedListView.h"
#include "FeedsView.h"
#include "UpdatesView.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "MainWindow"


MainWindow::MainWindow()
	:
	BWindow(((App*)be_app)->fPreferences->fMainWindowRect,
		B_TRANSLATE_SYSTEM_NAME("Pogger"), B_TITLED_WINDOW, 0)
{
	_InitInterface();
	MoveOnScreen();
}


MainWindow::~MainWindow()
{
	((App*)be_app)->fPreferences->fMainWindowRect = ConvertToScreen(Bounds());
	((App*)be_app)->fPreferences->fTabSelection = fTabView->Selection();
	be_app->PostMessage(B_QUIT_REQUESTED);
}


void
MainWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case kFeedsAddNew:
		case kFeedsEnqueueSelected:
		case kFeedsRemoveSelected:
		case kFeedsEditSelected:
		case kFeedsSelected:
		case kFeedsEdited:
		case kDownloadStart:
		case kDownloadComplete:
		case kDownloadFail:
		case kParseComplete:
		case kParseFail:
		{
			fFeedsView->MessageReceived(msg);
			break;
		}
		case kOpenWithSelect:
		{
			fEntriesView->MessageReceived(msg);
			break;
		}
		case kProgress:
		{
			fFeedsView->MessageReceived(msg);
			int32 total,current = 0;

			if (msg->FindInt32("total", &total) == B_OK
				&& msg->FindInt32("current", &current) == B_OK)
			{
				_UpdateProgress(total, current);
			}
			break;
		}
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}


void
MainWindow::_InitInterface()
{
	// Tabs
	fBaseView = new BGroupView("baseView");
	fTabView = new BTabView("tabView", B_WIDTH_FROM_WIDEST);
	fFeedsView = new FeedsView(B_TRANSLATE("Feeds"));
	fEntriesView = new EntriesView(B_TRANSLATE("Entries"));
	fUpdatesView = new UpdatesView(B_TRANSLATE("Updates"));

	fTabView->AddTab(fFeedsView);
	fTabView->AddTab(fEntriesView);
	fTabView->AddTab(fUpdatesView);
	fTabView->SetBorder(B_NO_BORDER);
	fTabView->Select(((App*)be_app)->fPreferences->fTabSelection);
	fBaseView->AddChild(fTabView);

	// Bottom bar
	fStatusBar = new BStatusBar("feedProgress");
	fUpdateButton = new BButton("updateNow", "", NULL);
	fUpdateButton->SetTarget((App*)be_app);
	fUpdateButton->SetExplicitAlignment(
		BAlignment(B_ALIGN_RIGHT, B_ALIGN_MIDDLE));
	_SetUpdateButton(false);

	// Window size
	font_height fontHeight;
	fTabView->GetFontHeight(&fontHeight);
	int16 fsize = fontHeight.ascent + fontHeight.descent;
	float minWidth = 36 * fsize;
	float minHeight = 25 * fsize;
	SetSizeLimits(minWidth, 100000, minHeight, 100000);
	if (Size().Height() < minHeight || Size().Width() < minWidth)
		ResizeTo(minWidth, minHeight);

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.SetInsets(0, B_USE_DEFAULT_SPACING, 0, 0)
		.Add(fBaseView)
		.Add(new BSeparatorView(B_HORIZONTAL))
		.AddGroup(B_HORIZONTAL)
			.Add(fStatusBar)
			.Add(fUpdateButton)
			.SetInsets(B_USE_WINDOW_SPACING, B_USE_DEFAULT_SPACING,
				B_USE_DEFAULT_SPACING, B_USE_WINDOW_SPACING)
		.End()
	 .End();
}


void
MainWindow::_UpdateProgress(int32 total, int32 current)
{
	if (current < fStatusBar->CurrentValue())
		fStatusBar->Reset();
	fStatusBar->SetMaxValue(total);
	fStatusBar->SetTo(current);

	if (total == current)
		_SetUpdateButton(false);
	else
		_SetUpdateButton(true);
}


void
MainWindow::_SetUpdateButton(bool cancel)
{
	if (cancel == true) {
		fUpdateButton->SetLabel(B_TRANSLATE("Cancel"));
		fUpdateButton->SetMessage(new BMessage(kClearQueue));
	} else {
		fUpdateButton->SetLabel(B_TRANSLATE("Update now"));
		fUpdateButton->SetMessage(new BMessage(kUpdateSubscribed));
	}
}


