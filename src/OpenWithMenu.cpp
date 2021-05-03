/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "OpenWithMenu.h"

#include <StringList.h>

#include "EntriesView.h"


OpenWithMenu::OpenWithMenu(const char* name, const char* type, BString preferred)
	: BPopUpMenu(name),
	fType(type),
	fPreferredApp(preferred)
{
	_PopulateMenu();
}


void
OpenWithMenu::_PopulateMenu()
{
	BStringList signatures;
	BMessage types;
	
	fType.GetSupportingApps(&types);
	if (types.FindStrings("applications", &signatures) != B_OK)
		return;

	int32 subTypeCount = types.GetInt32("be:sub", signatures.CountStrings());

	bool preferredAdded = false;
	for (int i = 0; i < signatures.CountStrings(); i++) {
		BString signature = signatures.StringAt(i);

		BMessage* msg = new BMessage(kOpenWithSelect);
		msg->AddString("signature", signature);

		BMenuItem* item = new BMenuItem(signature, msg);
		AddItem(item);

		BMimeType mime(signature.String());
		char appName[B_MIME_TYPE_LENGTH] = {'\0'};
		mime.GetShortDescription(appName);
		item->SetLabel(appName);

		if (signature == fPreferredApp) {
			item->SetMarked(true);
			preferredAdded = true;
		}

		if (i + 1 == subTypeCount)
			AddSeparatorItem();
	}

	// If the current preference isn't associated with text/html, or is a path,
	// add it to the menu.
	if (preferredAdded == false) {
		AddSeparatorItem();

		BMessage* msg = new BMessage(kOpenWithSelect);
		msg->AddString("signature", fPreferredApp);

		BMenuItem* prefItem = new BMenuItem(fPreferredApp.String(), msg);
		prefItem->SetLabel(fPreferredApp.String());
		prefItem->SetMarked(true);
		AddItem(prefItem);

		BMimeType prefMime(fPreferredApp.String());
		if (prefMime.IsValid() == true) {
			char appName[B_MIME_TYPE_LENGTH] = {'\0'};
			prefMime.GetShortDescription(appName);
			prefItem->SetLabel(appName);
		}
	}
}


