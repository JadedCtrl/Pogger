/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "FeedView.h"

#include <Message.h>
#include <GroupView.h>
#include <LayoutBuilder.h>


FeedView::FeedView(const char* name)
	:
	BGroupView(name, B_VERTICAL, B_USE_DEFAULT_SPACING)
{
	_InitInterface();
}


void
FeedView::MessageReceived(BMessage* msg)
{
}


void
FeedView::_InitInterface()
{
	fTestButton = new BButton("stest", "Do Nothing!",
		new BMessage('pppq'));
	fTestButton->SetTarget(this);
	fTestButton->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_MIDDLE));

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.SetInsets(0, B_USE_DEFAULT_SPACING, 0, 0)
		.Add(fTestButton)
		.SetInsets(B_USE_WINDOW_SPACING, B_USE_DEFAULT_SPACING,
			B_USE_DEFAULT_SPACING, B_USE_WINDOW_SPACING);
}


