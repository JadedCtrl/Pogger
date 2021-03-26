/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * Copyright 2004-2018, Haiku, Inc. All Rights Reserved.
 * Copyright 2001 Dr. Zoidberg Enterprises. All rights reserved.
 * All rights reserved. Distributed under the terms of the MIT license.
 *
 * Authors:
 *		Michael Weirauch, dev@m-phasis.de
 */

#ifndef DBVIEW_H 
#define DBVIEW_H

#include <View.h>

class BPopUpMenu;


enum
{
	kOpenSettings	= 'dbos',
	kQuitPogger		= 'dbkp',
	kUpdateNow		= 'dbup',
	kEntryFolder	= 'dbef',
	kQueryNew		= 'dbqn'
};


// This view has a large amount of code from the mail and bluetooth servers
// (src/servers/mail/DeskbarView.cpp src/servers/bluetooth/DeskbarReplicant.cpp)

class DeskbarView : public BView {
public:
	DeskbarView(BRect rect);
	DeskbarView(BMessage* msg);

	status_t Archive(BMessage *data, bool deep = true) const;
	static DeskbarView* Instantiate(BMessage *data);
	void AttachedToWindow();

	void MessageReceived(BMessage* msg);

	void Draw(BRect rect);

	void MouseUp(BPoint point);
	void MouseDown(BPoint point);

	BPopUpMenu* _BuildMenu();
	void _GetEntryDir();
	status_t _GetNewEntryQuery(entry_ref* ref);
	status_t _CreateNewEntryQuery(BEntry query);
	void _InitIcon();


private:
	BBitmap*	fIcon;
	BString		fEntryDir;
};


int32 installDeskbar();
void removeDeskbar();

status_t our_image(image_info& image);


#endif // DBVIEW_H

