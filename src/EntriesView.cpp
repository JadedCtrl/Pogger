/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "EntriesView.h"

#include <iostream>

#include <Alert.h>
#include <Box.h>
#include <Button.h>
#include <Message.h>
#include <LayoutBuilder.h>
#include <PopUpMenu.h>
#include <StringList.h>
#include <RadioButton.h>
#include <StringView.h>
#include <TextControl.h>

#include "App.h"
#include "Util.h"


EntriesView::EntriesView(const char* name)
	:
	BGroupView(name, B_VERTICAL, B_USE_DEFAULT_SPACING)
{
	_InitInterface();
}


void
EntriesView::AttachedToWindow()
{
	fEntryFolderText->SetTarget(this);
	fEntryFolderBrowseButton->SetTarget(this);

	fOpenAsHtmlRadio->SetTarget(this);
	fOpenAsUrlRadio->SetTarget(this);
	fOpenWithSelectButton->SetTarget(this);
}


void
EntriesView::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case kEntryFolderText:
		{
			status_t result = ((App*)be_app)->fPreferences->SetEntryDir(
				fEntryFolderText->Text());
			if (result != B_OK)
				_FileError(result);

			fEntryFolderText->SetText(((App*)be_app)->fPreferences->EntryDir());
			break;
		}
		case kEntryFolderBrowse:
		{
			entry_ref appsRef;
			fEntryFolderPanel = new BFilePanel(B_OPEN_PANEL, NULL, NULL,
				B_DIRECTORY_NODE, false, new BMessage(kEntryFolderPath));
			fEntryFolderPanel->Show();
			fEntryFolderPanel->SetTarget(this);
			break;
		}
		case kEntryFolderPath:
		{
			entry_ref ref;
			if (msg->HasRef("refs") && msg->FindRef("refs", &ref) == B_OK) {
				status_t result = ((App*)be_app)->fPreferences->SetEntryDir(
					BPath(&ref).Path());
				if (result != B_OK)
					_FileError(result);
			}

			fEntryFolderText->SetText(((App*)be_app)->fPreferences->EntryDir());
			delete fEntryFolderPanel;
			break;
		}
		case kOpenHtmlRadio:
		{
			((App*)be_app)->fPreferences->fOpenAs = kOpenAsHtml;
			break;
		}
		case kOpenUrlRadio:
		{
			((App*)be_app)->fPreferences->fOpenAs = kOpenAsUrl;
			break;
		}
		case kOpenWithSelect:
		{
			((App*)be_app)->fPreferences->SetEntryOpenWith(
				fOpenWithMenuField->MenuItem()->Label());
			break;
		}
		case kOpenWithBrowse:
		{
			entry_ref appsRef;
			BEntry("/boot/system/apps/").GetRef(&appsRef);

			fOpenWithPanel = new BFilePanel(B_OPEN_PANEL, NULL, &appsRef,
				B_FILE_NODE, false, new BMessage(kOpenWithPath));
			fOpenWithPanel->Show();
			fOpenWithPanel->SetTarget(this);
			break;
		}
		case kOpenWithPath:
		{
			entry_ref ref;
			if (msg->HasRef("refs") && msg->FindRef("refs", &ref) == B_OK
				&& ((App*)be_app)->fPreferences->SetEntryOpenWith(
					BPath(&ref).Path()) == B_OK)
			{
				BMenuItem* prefItem = new BMenuItem(BPath(&ref).Path(),
					new BMessage(kOpenWithSelect));
				prefItem->SetMarked(true);
				fOpenWithMenu->AddItem(prefItem);

			}

			delete fOpenWithPanel;
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
EntriesView::_InitInterface()
{
	// Saving
	fSavingBox = new BBox("saving");
	fSavingBox->SetLabel("Saving");

	fEntryFolderLabel = new BStringView("entryFolderLabel", "Entry folder:");
	fEntryFolderText = new BTextControl("entryFolder", "", "",
		new BMessage(kEntryFolderText));
	fEntryFolderBrowseButton = new BButton("entryFolderBrowse", "Browse…",
		new BMessage(kEntryFolderBrowse));

	// Opening
	fOpeningBox = new BBox("opening");
	fOpeningBox->SetLabel("Opening");

	fOpenAsLabel = new BStringView("openAsLabel", "Open as:");
	fOpenAsHtmlRadio = new BRadioButton("asHtml", "HTML",
		new BMessage(kOpenHtmlRadio));
	fOpenAsUrlRadio = new BRadioButton("asUrl", "URL",
		new BMessage(kOpenUrlRadio));

	fOpenWithLabel = new BStringView("openWithLabel", "Open with:");
	fOpenWithMenu = new BPopUpMenu("openWith");
	fOpenWithMenuField = new BMenuField("openWithMenu", NULL, fOpenWithMenu);
	fOpenWithSelectButton = new BButton("openWithSelect", "Select…",
		new BMessage(kOpenWithBrowse));


	// Display current settings
	Preferences* prefs = ((App*)be_app)->fPreferences;
	if (prefs->fOpenAs == kOpenAsHtml)
		fOpenAsHtmlRadio->SetValue(B_CONTROL_ON);
	else 
		fOpenAsUrlRadio->SetValue(B_CONTROL_ON);

	fEntryFolderText->SetText(prefs->EntryDir());

	_PopulateOpenWithMenu();


	BLayoutBuilder::Group<>(fSavingBox, B_HORIZONTAL)
		.SetInsets(B_USE_ITEM_INSETS)
		.AddGroup(B_VERTICAL, B_USE_DEFAULT_SPACING)
			.SetInsets(0, 20, B_USE_ITEM_INSETS, 0)
			.Add(fEntryFolderLabel)
		.End()
		.AddGroup(B_VERTICAL, B_USE_DEFAULT_SPACING)
			.SetInsets(0, 20, B_USE_ITEM_INSETS, 0)
			.AddGroup(B_HORIZONTAL, B_USE_DEFAULT_SPACING)
				.Add(fEntryFolderText)
				.Add(fEntryFolderBrowseButton)
			.End()
		.End()
	.End();

	BLayoutBuilder::Group<>(fOpeningBox, B_HORIZONTAL, B_USE_HALF_ITEM_SPACING)
		.SetInsets(B_USE_ITEM_INSETS)
		.AddGroup(B_VERTICAL, B_USE_DEFAULT_SPACING)
			.SetInsets(0, 20, B_USE_ITEM_INSETS, 0)
			.Add(fOpenAsLabel)
			.Add(fOpenWithLabel)
		.End()
		.AddGroup(B_VERTICAL, B_USE_DEFAULT_SPACING)
			.SetInsets(0, 20, B_USE_ITEM_INSETS, 0)
			.AddGroup(B_HORIZONTAL, B_USE_DEFAULT_SPACING)
				.Add(fOpenAsHtmlRadio)
				.Add(fOpenAsUrlRadio)
				.AddGlue()
			.End()
			.AddGroup(B_HORIZONTAL, B_USE_DEFAULT_SPACING)
				.Add(fOpenWithMenuField)
				.Add(fOpenWithSelectButton)
			.End()
		.End()
	.End();

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.SetInsets(B_USE_DEFAULT_SPACING)
		.Add(fSavingBox)
		.AddGlue()
		.Add(fOpeningBox)
		.AddGlue()
	.End();
}


void
EntriesView::_PopulateOpenWithMenu()
{
	BString preferred = ((App*)be_app)->fPreferences->EntryOpenWith();
	BMimeType html("text/html");
	BStringList signatures;
	BMessage types;
	
	BMenuItem* prefItem = new BMenuItem(preferred, new BMessage(kOpenWithSelect));
	prefItem->SetMarked(true);
	fOpenWithMenu->AddItem(prefItem);

	html.GetSupportingApps(&types);
	if (types.FindStrings("applications", &signatures) != B_OK)
		return;

	for (int i = 0; i < signatures.CountStrings(); i++) {
		BString string = signatures.StringAt(i);
		BMenuItem* item = new BMenuItem(string, new BMessage(kOpenWithSelect));
		if (string != preferred)
			fOpenWithMenu->AddItem(item);
	}
}


void
EntriesView::_FileError(status_t result)
{
	BPath cfgPath;
	find_directory(B_USER_SETTINGS_DIRECTORY, &cfgPath);

	if (result == B_NOT_A_DIRECTORY) {
		BAlert* alert = new BAlert("Entries Directory", "The path you "
		"selected isn't a folder― please choose another path.", "OK",
		NULL, NULL, B_WIDTH_AS_USUAL, B_WARNING_ALERT);
		alert->Go();
		return;
	}

	userFileError(result, "Entries Directory",
		"Couldn't open this folder because no path was specified.\n"
	"Please select a new folder.",
		"Couldn't open this folder because permission was denied.\n"
	"This usually means that you don't have read permissions.\nPlease make "
	"sure that your user has read-access to this folder.\nCheck your OS "
	"documentation for more information.",
		"There is not enough memory available on your system to access the "
	"given path.\nPlease try closing a few applications and restarting Pogger.");
}


