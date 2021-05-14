/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "FeedListView.h"

#include <Catalog.h>
#include <MenuItem.h>
#include <OutlineListView.h>
#include <PopUpMenu.h>
#include <Window.h>

#include "App.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "FeedListView"


FeedListView::FeedListView(const char* name)
	: BOutlineListView(name)
{
}


void
FeedListView::MouseDown(BPoint where)
{
	BOutlineListView::MouseDown(where);

	uint32 buttons = 0;
	Window()->CurrentMessage()->FindInt32("buttons", (int32*)&buttons);

	if (!(buttons & B_SECONDARY_MOUSE_BUTTON))
		return;
	
	BMenuItem* item;
	int32 selIndex = CurrentSelection();
	if (Superitem(ItemAt(selIndex)) == NULL)
		item = _SourcePopUp()->Go(ConvertToScreen(where));
	else
		item = _FeedPopUp()->Go(ConvertToScreen(where));

	if (item != NULL)
		Window()->MessageReceived(item->Message());
	return;
}


BPopUpMenu*
FeedListView::_FeedPopUp()
{
	BPopUpMenu* menu = new BPopUpMenu("feedPopUp");
	menu->SetTargetForItems(Window());
	menu->AddItem(new BMenuItem(B_TRANSLATE("Update feed"),
		new BMessage(kFeedsEnqueueSelected)));
	menu->AddItem(new BMenuItem(B_TRANSLATE("Edit feed…"),
		new BMessage(kFeedsEditSelected)));
	menu->AddItem(new BMenuItem(B_TRANSLATE("Remove feed…"), 
		new BMessage(kFeedsRemoveSelected)));

	return menu;
}


BPopUpMenu*
FeedListView::_SourcePopUp()
{
	BMessage enqueue(kFeedsEnqueueSelected);

	BPopUpMenu* menu = new BPopUpMenu("sourcePopUp");
	menu->AddItem(new BMenuItem(B_TRANSLATE("Update source"),
		new BMessage(kFeedsEnqueueSelected)));
//	menu->AddItem(BMenuItem("Configure source…", BMessage('')));
//	menu->AddItem(BMenuItem("Remove source…", BMessage('')));
	return menu;
}


