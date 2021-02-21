/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "UpdatesView.h"

#include <Box.h>
#include <CheckBox.h>
#include <Message.h>
#include <MessageRunner.h>
#include <LayoutBuilder.h>
#include <Slider.h>

#include <cstdio>

#include "App.h"


UpdatesView::UpdatesView(const char* name)
	:
	BGroupView(name, B_VERTICAL, B_USE_DEFAULT_SPACING)
{
	_InitInterface();
}


void
UpdatesView::AttachedToWindow()
{
	fNotifyNewCheck->SetTarget(this);
	fNotifyFailCheck->SetTarget(this);
	fIntervalSlider->SetTarget(this);
}


void
UpdatesView::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case kNotifyNewCheckbox:
		{
			if (fNotifyNewCheck->Value() == B_CONTROL_ON)
				((App*)be_app)->fPreferences->fNewNotify = true;
			else
				((App*)be_app)->fPreferences->fNewNotify = false;
			break;
		}
		case kNotifyFailCheckbox:
		{
			if (fNotifyFailCheck->Value() == B_CONTROL_ON)
				((App*)be_app)->fPreferences->fFailureNotify = true;
			else
				((App*)be_app)->fPreferences->fFailureNotify = false;
			break;
		}
		case kIntervalChanged:
		{
			_UpdateIntervalPreference();
			_UpdateIntervalLabel();
			break;
		}
		default:
		{
			BGroupView::MessageReceived(msg);
			break;
		}
	}
}


void
UpdatesView::_InitInterface()
{
	// Notifications
	fNotificationsBox = new BBox("notifications");
	fNotificationsBox->SetLabel("Notifications");

	fNotifyNewCheck = new BCheckBox("newNotify", "Notify about new entries",
		new BMessage(kNotifyNewCheckbox));
	fNotifyFailCheck = new BCheckBox("errorNotify",
		"Notify about update failures", new BMessage(kNotifyFailCheckbox));

	// Update scheduling
	fSchedulingBox = new BBox("scheduling");
	fSchedulingBox->SetLabel("Scheduling");

	fIntervalSlider =
		new BSlider("interval", "Never automatically update",
			new BMessage(kIntervalChanged), 0, 25, B_HORIZONTAL);

	fIntervalSlider->SetHashMarkCount(26);
	fIntervalSlider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	fIntervalSlider->SetLimitLabels("Never", "24 hours");
	fIntervalSlider->SetModificationMessage(new BMessage(kIntervalChanged));


	// Display current settings
	Preferences* prefs = ((App*)be_app)->fPreferences;
	if (prefs->fNewNotify == true)
		fNotifyNewCheck->SetValue(B_CONTROL_ON);
	if (prefs->fFailureNotify == true)
		fNotifyFailCheck->SetValue(B_CONTROL_ON);

	fIntervalSlider->SetValue(prefs->UpdateIntervalIndex());
	_UpdateIntervalLabel();


	BLayoutBuilder::Group<>(fNotificationsBox, B_VERTICAL, B_USE_HALF_ITEM_SPACING)
		.SetInsets(B_USE_ITEM_INSETS)
		.AddStrut(B_USE_ITEM_SPACING)
		.Add(fNotifyNewCheck)
		.Add(fNotifyFailCheck)
	.End();

	BLayoutBuilder::Group<>(fSchedulingBox, B_VERTICAL, B_USE_HALF_ITEM_SPACING)
		.SetInsets(B_USE_ITEM_INSETS)
		.AddStrut(B_USE_ITEM_SPACING)
		.Add(fIntervalSlider)
	.End();

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.SetInsets(B_USE_DEFAULT_SPACING)
		.Add(fNotificationsBox)
		.AddGlue()
		.Add(fSchedulingBox)
		.AddGlue()
	.End();
}


void
UpdatesView::_UpdateIntervalPreference()
{
	int32 limit;
	fIntervalSlider->GetLimits(NULL, &limit);
	int8 index = fIntervalSlider->Position() / (1/(float)limit);
	int8 oldIndex = ((App*)be_app)->fPreferences->UpdateIntervalIndex();

	((App*)be_app)->fPreferences->SetUpdateIntervalIndex(index);

	if (oldIndex == 0)
		((App*)be_app)->fUpdateRunner->SetCount(-1);
	else if (index == 0)
		((App*)be_app)->fUpdateRunner->SetCount(0);

	((App*)be_app)->fUpdateRunner->SetInterval(
		((App*)be_app)->fPreferences->UpdateInterval());
}


void
UpdatesView::_UpdateIntervalLabel()
{
	int8 hours = ((App*)be_app)->fPreferences->UpdateIntervalIndex() - 1;

	BString newLabel;
	BString strHour;
	strHour << hours;

	switch (hours)
	{
		case -1:
		{
			newLabel = "Never automatically update";
			break;
		}
		case 0:
		{
			newLabel = "Update every 30 minutes";
			break;
		}
		default:
			newLabel = "Update every %hour% hours";
	}

	newLabel.ReplaceAll("%hour%", strHour);
	fIntervalSlider->SetLabel(newLabel.String());
}


