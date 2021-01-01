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

	font_height fontHeight;
	GetFontHeight(&fontHeight);
	int16 buttonHeight = int16(fontHeight.ascent + fontHeight.descent + 12);
	BSize charButtonSize(buttonHeight, buttonHeight);

	// Add, Remove, Edit
	fAddButton = new BButton("addFeed", "+",
		new BMessage('fadd'));
	fRemoveButton = new BButton("removeFeed", "-",
		new BMessage('frem'));

	fAddButton->SetTarget(this);
	fAddButton->SetExplicitSize(charButtonSize);
	fRemoveButton->SetTarget(this);
	fRemoveButton->SetExplicitSize(charButtonSize);


	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.SetInsets(B_USE_DEFAULT_SPACING)
		.Add(fFeedsScrollView)

		// Add, Remove, and Edit buttons
		.AddGroup(B_HORIZONTAL, 0, 0.0)
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
		.End();
}


