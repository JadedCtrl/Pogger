/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef ENTRIESVIEW_H 
#define ENTRIESVIEW_H

#include <SupportDefs.h>
#include <GroupView.h>

class BBox;
class BButton;
class BMenuField;
class BMessage;
class BPopUpMenu;
class BRadioButton;
class BStringView;
class BTextControl;


class EntriesView : public BGroupView {
public:
	EntriesView(const char* name);

	void MessageReceived(BMessage* msg);
	void AttachedToWindow();

private:
	void _InitInterface();


	BBox*			fSavingBox;
	BStringView*	fEntryFolderLabel;
	BTextControl*	fEntryFolderText;
	BButton*		fEntryFolderBrowseButton;
	BStringView*	fFileExtLabel;
	BTextControl*	fFileExtText;

	BBox*			fOpeningBox;
	BStringView*	fOpenAsLabel;
	BRadioButton*	fOpenAsHtmlRadio;
	BRadioButton*	fOpenAsUrlRadio;
	BStringView*	fOpenWithLabel;
	BPopUpMenu*		fOpenWithMenu;
	BMenuField*		fOpenWithMenuField;
	BButton*		fOpenWithSelectButton;
};


#endif // ENTRIESVIEW_H

