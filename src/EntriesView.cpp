/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "EntriesView.h"

#include <Alert.h>
#include <Box.h>
#include <Button.h>
#include <Catalog.h>
#include <Message.h>
#include <LayoutBuilder.h>
#include <PopUpMenu.h>
#include <StringList.h>
#include <RadioButton.h>
#include <StringView.h>
#include <TextControl.h>

#include "App.h"
#include "OpenWithMenu.h"
#include "Util.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "EntriesView"


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
			status_t result = _SetEntryOpenWith(fEntryFolderText->Text());
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
			BString signature;
			if (msg->FindString("signature", &signature) == B_OK)
				_SetEntryOpenWith(signature.String());
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
				&& _SetEntryOpenWith(BPath(&ref).Path()) == B_OK)
			{
				BMessage* prefMsg = new BMessage(kOpenWithSelect);
				prefMsg->AddString("signature", BPath(&ref).Path());

				BMenuItem* prefItem = new BMenuItem(BPath(&ref).Path(), prefMsg);
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
	fSavingBox->SetLabel(B_TRANSLATE("Saving"));

	fEntryFolderLabel = new BStringView("entryFolderLabel",
		B_TRANSLATE("Entry folder:"));
	fEntryFolderText = new BTextControl("entryFolder", "", "",
		new BMessage(kEntryFolderText));
	fEntryFolderBrowseButton = new BButton("entryFolderBrowse",
		B_TRANSLATE("Browse…"), new BMessage(kEntryFolderBrowse));

	// Opening
	fOpeningBox = new BBox("opening");
	fOpeningBox->SetLabel(B_TRANSLATE("Opening"));

	fOpenAsLabel = new BStringView("openAsLabel", B_TRANSLATE("Open as:"));
	fOpenAsHtmlRadio = new BRadioButton("asHtml", B_TRANSLATE("HTML"),
		new BMessage(kOpenHtmlRadio));
	fOpenAsUrlRadio = new BRadioButton("asUrl", B_TRANSLATE("URL"),
		new BMessage(kOpenUrlRadio));

	fOpenWithLabel = new BStringView("openWithLabel",
		B_TRANSLATE("Open with:"));
	fOpenWithMenu = new OpenWithMenu("openWith", "text/html",
		((App*)be_app)->fPreferences->EntryOpenWith());
	fOpenWithMenuField = new BMenuField("openWithMenu", NULL, fOpenWithMenu);
	fOpenWithSelectButton = new BButton("openWithSelect",
		B_TRANSLATE("Select…"), new BMessage(kOpenWithBrowse));


	// Display current settings
	Preferences* prefs = ((App*)be_app)->fPreferences;
	if (prefs->fOpenAs == kOpenAsHtml)
		fOpenAsHtmlRadio->SetValue(B_CONTROL_ON);
	else 
		fOpenAsUrlRadio->SetValue(B_CONTROL_ON);

	fEntryFolderText->SetText(prefs->EntryDir());

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

	BLayoutBuilder::Group<>(this, B_VERTICAL, B_USE_ITEM_SPACING)
		.SetInsets(B_USE_DEFAULT_SPACING)
		.Add(fSavingBox)
		.Add(fOpeningBox)
		.AddGlue()
	.End();
}


status_t
EntriesView::_SetEntryOpenWith(const char* entry)
{
	char systemSig[B_MIME_TYPE_LENGTH];
	BMimeType entryType("application/x-feed-entry");


	if (entryType.GetPreferredApp(systemSig) == B_OK
		&& (BString(systemSig) != BString("application/x-vnd.Pogger")
			&& BString(systemSig) != "application/x-vnd.PoggerDaemon"))
	{
		BAlert* alert = new BAlert(B_TRANSLATE("Default entry program"),
			B_TRANSLATE("This program will only be used to open entry files "
			"if they are first opened with Pogger.\nCurrently, Pogger isn't set "
			"as the default to open feed entries."),
		    B_TRANSLATE("Ignore"), B_TRANSLATE("Make default"), "",
			B_WIDTH_AS_USUAL, B_EVEN_SPACING, B_IDEA_ALERT);

		alert->SetShortcut(0, B_ESCAPE);
		int32 button_index = alert->Go();

		if (button_index == 1)
			entryType.SetPreferredApp("application/x-vnd.Pogger");
	}

	return ((App*)be_app)->fPreferences->SetEntryOpenWith(entry);
}


void
EntriesView::_FileError(status_t result)
{
	BPath cfgPath;
	find_directory(B_USER_SETTINGS_DIRECTORY, &cfgPath);

	if (result == B_NOT_A_DIRECTORY) {
		BAlert* alert = new BAlert(B_TRANSLATE("Entries directory"),
			B_TRANSLATE("The path you selected isn't a folder― please choose "
			"another path."), B_TRANSLATE("OK"), NULL, NULL,
			B_WIDTH_AS_USUAL, B_WARNING_ALERT);
			alert->Go();
		return;
	}

	userFileError(result, B_TRANSLATE("Entries directory"),
		B_TRANSLATE("Couldn't open this folder because no path was specified.\n"
	"Please select a new folder."),
		B_TRANSLATE("Couldn't open this folder because permission was denied.\n"
	"This usually means that you don't have read permissions.\nPlease make "
	"sure that your user has read-access to this folder.\nCheck your OS "
	"documentation for more information."),
		B_TRANSLATE("There is not enough memory available on your system to "
	"access the given path.\nPlease try closing a few applications and "
	"restarting Pogger."));
}


