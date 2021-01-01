/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "EntriesView.h"

#include <Box.h>
#include <Button.h>
#include <Message.h>
#include <LayoutBuilder.h>
#include <PopUpMenu.h>
#include <RadioButton.h>
#include <StringView.h>
#include <TextControl.h>


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
	fFileExtText->SetTarget(this);

	fOpenAsHtmlRadio->SetTarget(this);
	fOpenAsUrlRadio->SetTarget(this);
	fOpenWithSelectButton->SetTarget(this);
}


void
EntriesView::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		default:
		{
//			BWindow::MessageReceived(msg);
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
	fEntryFolderText = new BTextControl("entryFolder", "",
		"/boot/home/feeds/", new BMessage('ssss'));
	fEntryFolderBrowseButton = new BButton("entryFolderBrowse", "Browse…",
		new BMessage('mmmm'));

	fFileExtLabel = new BStringView("fileExtLabel", "File extension:");
	fFileExtText = new BTextControl("fileExt", "", "", new BMessage('ffff'));

	// Opening
	fOpeningBox = new BBox("opening");
	fOpeningBox->SetLabel("Opening");

	fOpenAsLabel = new BStringView("openAsLabel", "Open as:");
	fOpenAsHtmlRadio = new BRadioButton("asHtml", "HTML", new BMessage('ii'));
	fOpenAsUrlRadio = new BRadioButton("asUrl", "URL", new BMessage('ii'));

	fOpenWithLabel = new BStringView("openWithLabel", "Open with:");
	fOpenWithMenu = new BPopUpMenu("openWith");
	fOpenWithMenuField = new BMenuField("openWithMenu", NULL, fOpenWithMenu);
	fOpenWithMenu->AddItem(new BMenuItem("WebPositive", new BMessage('wwww')));
	fOpenWithSelectButton = new BButton("openWithSelect", "Select…",
		new BMessage('mmmm'));


	BLayoutBuilder::Group<>(fSavingBox, B_HORIZONTAL)
		.SetInsets(B_USE_ITEM_INSETS)
		.AddGroup(B_VERTICAL, B_USE_DEFAULT_SPACING)
			.SetInsets(0, 20, B_USE_ITEM_INSETS, 0)
			.Add(fEntryFolderLabel)
			.Add(fFileExtLabel)
		.End()
		.AddGroup(B_VERTICAL, B_USE_DEFAULT_SPACING)
			.SetInsets(0, 20, B_USE_ITEM_INSETS, 0)
			.AddGroup(B_HORIZONTAL, B_USE_DEFAULT_SPACING)
				.Add(fEntryFolderText)
				.Add(fEntryFolderBrowseButton)
			.End()
			.Add(fFileExtText)
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


