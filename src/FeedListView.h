/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef FEEDLISTVIEW_H
#define FEEDLISTVIEW_H

#include <OutlineListView.h>
#include <Point.h>

class BPopUpMenu;


enum
{
	kFeedsRemoveSelected	= 'srem',
	kFeedsEditSelected		= 'sedt',
	kFeedsEnqueueSelected	= 'senq'
};


class FeedListView : public BOutlineListView {
public:
	FeedListView(const char* name);

	void MouseDown(BPoint where);

private:
	BPopUpMenu* _FeedPopUp();
	BPopUpMenu* _SourcePopUp();
};


#endif // FEEDLISTVIEW_H

