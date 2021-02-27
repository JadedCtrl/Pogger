/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
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
class BStringView;


enum
{
	kFeedsAddButton		= 'sadd',
	kFeedsRemoveButton	= 'srem',
	kFeedsEditButton	= 'sedt',
	kFeedsSelected		= 'flsl',
	kFeedsEdited		= 'fedd'
};


class FeedsView : public BGroupView {
public:
	FeedsView(const char* name);

	virtual void MessageReceived(BMessage* msg);

private:
	void _InitInterface();

	void _EditSelectedFeed();
	void _RemoveSelectedFeed();

	void _PopulateFeedList();

	void _UpdateProgress(BMessage* msg, int8 status);

	BButton* fAddButton;
	BButton* fRemoveButton;
	BButton* fEditButton;
	BStringView* fProgressLabel;
	BListView* fFeedsListView;
	BScrollView* fFeedsScrollView;
};


#endif // FEEDDVIEW_H
