/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef OPENWITHMENU_H
#define OPENWITHMENU_H

#include <Menu.h>
#include <MenuItem.h>
#include <MimeType.h>
#include <PopUpMenu.h>


class OpenWithMenu : public BPopUpMenu {
public:
	OpenWithMenu(const char* name, const char* type, BString preferred);

private:
	void _PopulateMenu();

	BMimeType fType;
	BString fPreferredApp;
};


#endif // OPENWITHMENU_H

