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

#include "App.h"


enum { M_BUTTON_CLICKED = 'btcl' };


MainWindow::MainWindow (void)
	:
	BWindow(BRect(0,0,0,0), "Pogger", B_TITLED_WINDOW, B_NOT_RESIZABLE
		| B_NOT_ZOOMABLE | B_AUTO_UPDATE_SIZE_LIMITS | B_QUIT_ON_WINDOW_CLOSE)
{
	_InitInterface();
	MoveTo(BPoint(-1000.0,-1000.0));
	MoveOnScreen();
}


void
MainWindow::MessageReceived(BMessage *msg)
{
       switch (msg->what)
       {
              case M_BUTTON_CLICKED:
              {
                     BString labelString("Clicks: ");
                     labelString << "DAD";
                     SetTitle(labelString.String());
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
	fFeedView = new FeedView("Feeds");

	fTabView->AddTab(fFeedView);
	fTabView->SetBorder(B_NO_BORDER);
	fBaseView->AddChild(fTabView);

	fServiceToggleButton = new BButton("stoggle", "Start Service",
		new BMessage('pppp'));
//	fRevertButton->SetEnabled(false);
	fServiceToggleButton->SetTarget(this);
	fServiceToggleButton->SetExplicitAlignment(
		BAlignment(B_ALIGN_LEFT, B_ALIGN_MIDDLE));

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.SetInsets(0, B_USE_DEFAULT_SPACING, 0, 0)
		.Add(fBaseView)
		.Add(new BSeparatorView(B_HORIZONTAL))
		.AddGroup(B_HORIZONTAL)
			.Add(fServiceToggleButton)
			.SetInsets(B_USE_WINDOW_SPACING, B_USE_DEFAULT_SPACING,
				B_USE_DEFAULT_SPACING, B_USE_WINDOW_SPACING);
}
