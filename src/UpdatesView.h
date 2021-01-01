/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef UPDATESVIEW_H
#define UPDATESVIEW_H

#include <Button.h>
#include <SupportDefs.h>
#include <GroupView.h>

class BBox;
class BCheckBox;
class BMessage;
class BSlider;


class UpdatesView : public BGroupView {
public:
	UpdatesView(const char* name);

	void MessageReceived(BMessage* msg);
	void AttachedToWindow();

private:
	void _InitInterface();
	void _UpdateIntervalLabel();

	BBox* fNotificationsBox;
	BBox* fSchedulingBox;

	BCheckBox*	fNotifyNewCheck;
	BCheckBox*	fNotifyErrorCheck;
	BSlider*	fIntervalSlider;
};


#endif // UPDATESVIEW_H

