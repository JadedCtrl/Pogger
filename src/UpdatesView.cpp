/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "UpdatesView.h"

#include <Box.h>
#include <CheckBox.h>
#include <Message.h>
#include <LayoutBuilder.h>
#include <Slider.h>

#include <cstdio>


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
	fNotifyErrorCheck->SetTarget(this);
	fIntervalSlider->SetTarget(this);
}


void
UpdatesView::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case 'iiii':
		{
			_UpdateIntervalLabel();
			break;
		}
		default:
		{
//			BWindow::MessageReceived(msg);
			break;
		}
	}
}


void
UpdatesView::_InitInterface()
{
	fNotifyNewCheck = new BCheckBox("newNotify", "Notify about new entries",
		new BMessage('nnnc'));
	fNotifyErrorCheck = new BCheckBox("errorNotify",
		"Notify about update failures", new BMessage('nnnc'));

	fNotificationsBox = new BBox("notifications");
	fNotificationsBox->SetLabel("Notifications");

	fIntervalSlider =
		new BSlider("interval", "Never automatically update",
			new BMessage('iiii'), 0, 25, B_HORIZONTAL);

	fIntervalSlider->SetHashMarkCount(26);
	fIntervalSlider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	fIntervalSlider->SetLimitLabels("Never", "24 hours");
	fIntervalSlider->SetModificationMessage(new BMessage('iiii'));

	fSchedulingBox = new BBox("scheduling");
	fSchedulingBox->SetLabel("Scheduling");


	BLayoutBuilder::Group<>(fNotificationsBox, B_VERTICAL, B_USE_HALF_ITEM_SPACING)
		.SetInsets(B_USE_ITEM_INSETS)
		.AddStrut(B_USE_ITEM_SPACING)
		.Add(fNotifyNewCheck)
		.Add(fNotifyErrorCheck)
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

	_UpdateIntervalLabel();
}


void
UpdatesView::_UpdateIntervalLabel()
{
	int32 limit;
	fIntervalSlider->GetLimits(NULL, &limit);
	int index = fIntervalSlider->Position() / (1/(float)limit);
	int hours = index - 1;

	BString newLabel;
	BString strHour;
	strHour << (int)hours;

	if (hours == -1 )
		newLabel = "Never automatically update";
	else if (hours == 0)
		newLabel = "Update every 30 minutes";
	else
		newLabel = "Update every %hour% hours";

	newLabel.ReplaceAll("%hour%", strHour);
	fIntervalSlider->SetLabel(newLabel.String());
}


