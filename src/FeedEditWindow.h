/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef FEEDEDIT_H
#define FEEDEDIT_H

#include <Window.h>

class BButton;
class BStringView;
class BTextControl;
class Feed;
class FeedListItem;


enum
{
	kSaveButton		= 'esav',
	kDeleteButton	= 'edel'
};


class FeedEditWindow : public BWindow {
public:
	FeedEditWindow();
	FeedEditWindow(BEntry feedEntry);
	FeedEditWindow(FeedListItem* feedItem);
	~FeedEditWindow();

	void MessageReceived(BMessage* msg);

private:
	void _InitInterface();

	void _SaveFeed();
	void _DeleteFeed();

	BString fFeedPath;
	Feed* fFeed;

	BStringView*	fFeedNameLabel;
	BTextControl*	fFeedNameText;
	BStringView*	fFeedUrlLabel;
	BTextControl*	fFeedUrlText;

	BButton* fSaveButton;
	BButton* fDeleteButton;
};


#endif // FEEDEDIT_H

