/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef ENTRIESVIEW_H 
#define ENTRIESVIEW_H

#include <GroupView.h>

class BBox;
class BButton;
class BFilePanel;
class BMenuField;
class BMessage;
class BRadioButton;
class BStringView;
class BTextControl;
class OpenWithMenu;


enum
{
	kEntryFolderText	= 'txef',
	kEntryFolderBrowse	= 'tbef',
	kEntryFolderPath	= 'txep',
	kOpenHtmlRadio		= 'rdow',
	kOpenUrlRadio		= 'roow',
	kOpenWithSelect		= 'mnow',
	kOpenWithPath		= 'pbow',
	kOpenWithBrowse		= 'tbow'
};


class EntriesView : public BGroupView {
public:
	EntriesView(const char* name);

	void MessageReceived(BMessage* msg);
	void AttachedToWindow();

private:
	void _InitInterface();

	status_t _SetEntryOpenWith(const char* entry);
	void _FileError(status_t result);

	BBox*			fSavingBox;
	BStringView*	fEntryFolderLabel;
	BTextControl*	fEntryFolderText;
	BButton*		fEntryFolderBrowseButton;
	BFilePanel*		fEntryFolderPanel;

	BBox*			fOpeningBox;
	BStringView*	fOpenAsLabel;
	BRadioButton*	fOpenAsHtmlRadio;
	BRadioButton*	fOpenAsUrlRadio;
	BStringView*	fOpenWithLabel;
	OpenWithMenu*	fOpenWithMenu;
	BMenuField*		fOpenWithMenuField;
	BButton*		fOpenWithSelectButton;
	BFilePanel*		fOpenWithPanel;
};


#endif // ENTRIESVIEW_H

