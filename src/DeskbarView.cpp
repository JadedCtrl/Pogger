/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * Copyright 2004-2018, Haiku, Inc. All Rights Reserved.
 * Copyright 2001 Dr. Zoidberg Enterprises. All rights reserved.
 * All rights reserved. Distributed under the terms of the MIT license.
 *
 * Authors:
 *		Michael Weirauch, dev@m-phasis.de
 */

#include "DeskbarView.h"

#include <Bitmap.h>
#include <Catalog.h>
#include <Deskbar.h>
#include <FindDirectory.h>
#include <IconUtils.h>
#include <MenuItem.h>
#include <NodeInfo.h>
#include <Path.h>
#include <PopUpMenu.h>
#include <Resources.h>
#include <Roster.h>

#include "FeedController.h"
#include "Preferences.h"
#include "Util.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "DeskbarView"


DeskbarView::DeskbarView(BRect rect)
	: BView(rect, "Pogger", B_FOLLOW_NONE, B_WILL_DRAW)
{
	_InitIcon();
}


DeskbarView::DeskbarView(BMessage* message)
	: BView(message)
{
	_InitIcon();
	_GetEntryDir();
}


status_t
DeskbarView::Archive(BMessage* data, bool deep) const {
	BView::Archive(data, deep);

	data->AddString("add_on", "application/x-vnd.PoggerDaemon");
	data->AddString("class", "Pogger");
	
	return B_OK;
}		


DeskbarView*
DeskbarView::Instantiate(BMessage* data) {
	if (!validate_instantiation(data, "Pogger"))
		return NULL;
	return new DeskbarView(data);
}


void
DeskbarView::AttachedToWindow()
{
	BView::AttachedToWindow();
	AdoptParentColors();

	SetLowColor(ViewColor());
}


void
DeskbarView::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case kEntryFolder:
		{
			entry_ref ref;
			BEntry entry(fEntryDir);
			entry.GetRef(&ref);
			be_roster->Launch(&ref);
			break;
		}
		case kQueryNew:
		{
			entry_ref query;
			_GetNewEntryQuery(&query);
			be_roster->Launch(&query);
			break;
		}
		case kUpdateNow:
		{
			BMessenger messenger("application/x-vnd.PoggerDaemon");
			messenger.SendMessage(kUpdateSubscribed);
			break;
		}
		case kOpenSettings:
		{
			be_roster->Launch("application/x-vnd.Pogger");
			break;
		}
		case kQuitPogger:
		{
			BMessenger messenger("application/x-vnd.PoggerDaemon");
			messenger.SendMessage(B_QUIT_REQUESTED);
			break;
		}
		default:
			BView::MessageReceived(msg);
	}
}


void
DeskbarView::Draw(BRect rect)
{
	SetDrawingMode(B_OP_ALPHA);
	DrawBitmap(fIcon);
	SetDrawingMode(B_OP_COPY);
}


void
DeskbarView::MouseUp(BPoint point)
{
	BMessage open(kQueryNew);
	MessageReceived(&open);
}


void
DeskbarView::MouseDown(BPoint point)
{
	uint32 buttons;
	GetMouse(&point, &buttons);
	if (!(buttons & B_SECONDARY_MOUSE_BUTTON))
		return;

	ConvertToScreen(&point);

	BPopUpMenu* menu = _BuildMenu();
	menu->SetTargetForItems(this);
	menu->Go(point, true, false, true, true);
}


BPopUpMenu*
DeskbarView::_BuildMenu()
{
	BPopUpMenu* menu = new BPopUpMenu(B_EMPTY_STRING, false, false);

	menu->AddItem(new BMenuItem(B_TRANSLATE("Open entry folder" B_UTF8_ELLIPSIS),
		new BMessage(kEntryFolder)));
	menu->AddItem(new BMenuItem(B_TRANSLATE("Query new entries" B_UTF8_ELLIPSIS),
		new BMessage(kQueryNew)));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem(B_TRANSLATE("Update now"),
		new BMessage(kUpdateNow)));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem(B_TRANSLATE("Settings" B_UTF8_ELLIPSIS),
		new BMessage(kOpenSettings)));
	menu->AddItem(new BMenuItem(B_TRANSLATE("Quit Pogger"),
		new BMessage(kQuitPogger)));

	return menu;
}


void
DeskbarView::_GetEntryDir()
{
	Preferences pref = Preferences();
	pref.Load();
	fEntryDir = pref.EntryDir();
}


status_t
DeskbarView::_GetNewEntryQuery(entry_ref* ref)
{
	BPath path;
	find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	path.Append("Pogger/New Entries");
	BEntry query(path.Path());
	if (query.Exists())
		return query.GetRef(ref);
	else
		return _CreateNewEntryQuery(query);
}


status_t
DeskbarView::_CreateNewEntryQuery(BEntry query)
{
	BFile file(&query, B_READ_WRITE | B_CREATE_FILE);
	if (file.InitCheck() != B_OK)
		return file.InitCheck();

	BString string("\"Feed:status\" ==\"New\"");
	file.WriteAttrString("_trk/qrystr", &string);
	file.WriteAttrString("_trk/qryinitstr", &string);
	int32 mode = 'Fbyq';
	file.WriteAttr("_trk/qryinitmode", B_INT32_TYPE, 0, &mode, sizeof(int32));
	string = "Feed entry";
	file.WriteAttrString("_trk/qryinitmime", &string);
	BNodeInfo(&file).SetType("application/x-vnd.Be-query");

	return B_OK;
}


void
DeskbarView::_InitIcon()
{
	image_info info;
	if (our_image(info) != B_OK)
		return;

	BFile file(info.name, B_READ_ONLY);
	if (file.InitCheck() < B_OK)
		return;

	BResources resources(&file);
	if (resources.InitCheck() < B_OK)
		return;

	size_t size;
	const void* data = resources.LoadResource(B_VECTOR_ICON_TYPE, "feed-type", &size);

	if (data != NULL) {
		BBitmap* icon = new BBitmap(Bounds(), B_RGBA32);
		if (icon->InitCheck() == B_OK
			&& BIconUtils::GetVectorIcon((const uint8 *)data, size, icon)
				== B_OK) {
			fIcon = icon;
		}
		else
				delete icon;
	}
}


status_t
our_image(image_info& image)
{
	int32 cookie = 0;
	while (get_next_image_info(B_CURRENT_TEAM, &cookie, &image) == B_OK) {
		if ((char *)our_image >= (char *)image.text
			&& (char *)our_image <= (char *)image.text + image.text_size)
			return B_OK;
	}

	return B_ERROR;
}

int32
installDeskbar()
{
	int32 deskbarId = -1;

	// Just in case deskbar hasn't launched yet
	if (BDeskbar().IsRunning() == false)
		sleep(3);

	removeDeskbar();

	BDeskbar deskbar;
	deskbar.AddItem(
		new DeskbarView(BRect(0,0, deskbar.MaxItemHeight() - 1,
			deskbar.MaxItemHeight() - 1)),
		&deskbarId);

	return deskbarId;
}


void
removeDeskbar()
{
	BDeskbar deskbar;
	deskbar.RemoveItem("Pogger");
}


