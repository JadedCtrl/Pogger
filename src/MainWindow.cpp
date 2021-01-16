/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "MainWindow.h"

#include <Button.h>
#include <GroupView.h>
#include <TabView.h>
#include <LayoutBuilder.h>
#include <SeparatorView.h>
#include <String.h>

#include <cstdio>

#include "App.h"
#include "EntriesView.h"
#include "FeedController.h"
#include "FeedsView.h"
#include "UpdatesView.h"


MainWindow::MainWindow()
	:
	BWindow(BRect(BPoint(-1000.0, -1000.0), BSize(520, 380)), "Pogger",
		B_TITLED_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
{
	_InitInterface();
	MoveOnScreen();
}


void
MainWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case kFeedsAddButton:
		case kFeedsRemoveButton:
		case kFeedsEditButton:
		case kFeedsSelected:
		{
			fFeedsView->MessageReceived(msg);
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
	fBaseView = new BGroupView("baseView");
	fTabView = new BTabView("tabView", B_WIDTH_FROM_WIDEST);
	fFeedsView = new FeedsView("Feeds");
	fEntriesView = new EntriesView("Entries");
	fUpdatesView = new UpdatesView("Updates");

	fTabView->AddTab(fFeedsView);
	fTabView->AddTab(fEntriesView);
	fTabView->AddTab(fUpdatesView);
	fTabView->SetBorder(B_NO_BORDER);
	fBaseView->AddChild(fTabView);

	fUpdateNowButton = new BButton("updateNow", "Update Now",
		new BMessage(kUpdateSubscribed));
	fUpdateNowButton->SetTarget((App*)be_app);
	fUpdateNowButton->SetExplicitAlignment(
		BAlignment(B_ALIGN_RIGHT, B_ALIGN_MIDDLE));

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.SetInsets(0, B_USE_DEFAULT_SPACING, 0, 0)
		.Add(fBaseView)
		.Add(new BSeparatorView(B_HORIZONTAL))
		.AddGroup(B_HORIZONTAL)
			.Add(fUpdateNowButton)
			.SetInsets(B_USE_WINDOW_SPACING, B_USE_DEFAULT_SPACING,
				B_USE_DEFAULT_SPACING, B_USE_WINDOW_SPACING)
		.End()
	.End();
}


