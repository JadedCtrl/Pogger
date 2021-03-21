/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "UpdatesView.h"

#include <Box.h>
#include <Catalog.h>
#include <CheckBox.h>
#include <LayoutBuilder.h>
#include <Message.h>
#include <MessageRunner.h>
#include <Slider.h>
#include <StringFormat.h>

#include "App.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "UpdatesView"


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
	fNotificationsBox->SetLabel(B_TRANSLATE("Notifications"));

	fNotifyNewCheck = new BCheckBox("newNotify",
		B_TRANSLATE("Notify about new entries"),
		new BMessage(kNotifyNewCheckbox));
	fNotifyFailCheck = new BCheckBox("errorNotify",
		B_TRANSLATE("Notify about update failures"),
		new BMessage(kNotifyFailCheckbox));

	// Update scheduling
	fSchedulingBox = new BBox("scheduling");
	fSchedulingBox->SetLabel(B_TRANSLATE("Scheduling"));

	fIntervalSlider =
		new BSlider("interval", B_TRANSLATE("Never automatically update"),
			new BMessage(kIntervalChanged), 0, 25, B_HORIZONTAL);

	fIntervalSlider->SetHashMarkCount(26);
	fIntervalSlider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	fIntervalSlider->SetLimitLabels(B_TRANSLATE("Never"),
		B_TRANSLATE("24 hours"));
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

	BLayoutBuilder::Group<>(this, B_VERTICAL, B_USE_ITEM_SPACING)
		.SetInsets(B_USE_DEFAULT_SPACING)
		.Add(fNotificationsBox)
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
			newLabel = B_TRANSLATE("Never automatically update");
			break;
		}
		case 0:
		{
			newLabel = B_TRANSLATE("Update every 30 minutes");
			break;
		}
		case 24:
		{
			newLabel = B_TRANSLATE("Update daily");
			break;
		}
		default:
		{
			static BStringFormat format(B_TRANSLATE("{0, plural,"
				"=1{Update every hour}"
				"other{Update every # hours}}"));
			format.Format(newLabel, hours);
		}
	}

	fIntervalSlider->SetLabel(newLabel.String());
}


