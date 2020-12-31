/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef FEEDVIEW_H
#define FEEDVIEW_H


#include <Button.h>
#include <SupportDefs.h>
#include <GroupView.h>


class BMessage;


class FeedView : public BGroupView {
public:
	FeedView(const char* name);

	void MessageReceived(BMessage* msg);
private:
	void _InitInterface();

	BButton* fTestButton;
};


#endif // FEEDVIEW_H
