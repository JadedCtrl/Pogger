/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
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


enum
{
	kIntervalChanged	= 'intu',
	kNotifyNewCheckbox	= 'chnn',
	kNotifyFailCheckbox	= 'chnf'
};


class UpdatesView : public BGroupView {
public:
	UpdatesView(const char* name);

	void MessageReceived(BMessage* msg);
	void AttachedToWindow();

private:
	void _InitInterface();

	void _UpdateIntervalPreference();
	void _UpdateIntervalLabel();

	BBox* fNotificationsBox;
	BBox* fSchedulingBox;

	BCheckBox*	fNotifyNewCheck;
	BCheckBox*	fNotifyFailCheck;
	BSlider*	fIntervalSlider;
};


#endif // UPDATESVIEW_H

