/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef FEEDEDIT_H
#define FEEDEDIT_H

#include <Window.h>

#include "Feed.h"

class BButton;
class BStringView;
class BTextControl;
class Feed;
class FeedListItem;


enum
{
	kOkButton		= 'edok',
	kCancelButton	= 'ecan'
};


class FeedEditWindow : public BWindow {
public:
	FeedEditWindow();
	FeedEditWindow(const char* identifier, const char* source);
	~FeedEditWindow();

	void MessageReceived(BMessage* msg);

private:
	void _InitInterface();

	void _SaveFeed();

	BString fFeedPath;
	Feed* fFeed;

	BStringView*	fFeedNameLabel;
	BTextControl*	fFeedNameText;
	BStringView*	fFeedUrlLabel;
	BTextControl*	fFeedUrlText;

	BButton* fOkButton;
	BButton* fCancelButton;
};


#endif // FEEDEDIT_H

