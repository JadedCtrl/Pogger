/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef FEEDSVIEW_H
#define FEEDSVIEW_H

#include <Button.h>
#include <SupportDefs.h>
#include <GroupView.h>

class BMessage;
class BListView;
class BScrollView;


class FeedsView : public BGroupView {
public:
	FeedsView(const char* name);

	void MessageReceived(BMessage* msg);

private:
	void _InitInterface();

	BButton* fAddButton;
	BButton* fRemoveButton;
	BListView* fFeedsListView;
	BScrollView* fFeedsScrollView;
};


#endif // FEEDDVIEW_H
