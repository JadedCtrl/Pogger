/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "Preferences.h"

#include <iostream>

#include <String.h>


Preferences::Preferences() {
}


Preferences::Preferences(Preferences* pref) {
}


// !! handle file status
void
Preferences::Load()
{
	BString configDir("/boot/home/config/settings/Pogger/");

	BString filename = BString(configDir);
	filename.Append("Settings");
	BFile file(filename.String(), B_READ_ONLY);
	status_t result = file.InitCheck();

	BMessage storage;
	storage.Unflatten(&file);

	fEntryDir		= BString(storage.GetString("entryDir", "/boot/home/feeds/"));
	fOpenAs			= storage.GetInt8("openAs", kOpenAsUrl);
	fOpenWith		= BString(storage.GetString("openWith",
		"application/x-vnd.Haiku-WebPositive"));

	fNewNotify		= storage.GetBool("notifyNew", true);
	fFailureNotify	= storage.GetBool("notifyFailure", true);
	fUpdateInterval	= storage.GetInt8("updateInterval", 1);

	fMainWindowRect	= storage.GetRect("mainWindow",
		BRect(BPoint(-1000.0, -1000.0), BSize(520, 380)));
	fFeedEditRect	= storage.GetRect("feedEditWindow",
		BRect(BPoint(-1000.0, -1000.0), BSize(500, 150)));
	fTabSelection	= storage.GetInt32("tabSelection", 0);
}


// !! handle file status
void
Preferences::Save()
{
	BString configDir = BString("/boot/home/config/settings/Pogger/");

	BPath*		cfgPath		= new BPath(configDir.String(), NULL, true);
	BEntry*		cfgEntry	= new BEntry(cfgPath->Path());
	BDirectory*	cfgDir		= new BDirectory;

	cfgDir->CreateDirectory(cfgPath->Path(), NULL);
	
	if (cfgEntry->Exists() == false)
		cfgDir->CreateDirectory(cfgPath->Path(), NULL);

	BMessage storage;
	BString filename = BString(configDir).Append("/Settings");

	BFile* file = new BFile(filename.String(), B_WRITE_ONLY | B_CREATE_FILE
		| B_ERASE_FILE);
	status_t result = file->InitCheck();

	storage.AddString("entryDir", fEntryDir.String());
	storage.AddString("openWith", fOpenWith.String());
	storage.AddInt8("openAs", fOpenAs);

	storage.AddBool("notifyNew", fNewNotify);
	storage.AddBool("notifyFailure", fFailureNotify);
	storage.AddInt8("updateInterval", fUpdateInterval);

	storage.AddRect("mainWindow", fMainWindowRect);
	storage.AddRect("feedEditWindow", fFeedEditRect);
	storage.AddInt32("tabSelection", fTabSelection);

	storage.Flatten(file);
}


int64
Preferences::UpdateInterval()
{
	int hours = fUpdateInterval - 1;

	switch (hours)
	{
		case -1:
			return -1;
		case 0:
			return HOUR_IN_MICROSECONDS / 2;
		default:
			return HOUR_IN_MICROSECONDS * hours;
	}
}


int
Preferences::UpdateIntervalIndex()
{
	return fUpdateInterval;
}


void
Preferences::SetUpdateIntervalIndex(int8 index)
{
	fUpdateInterval = index;
}


BString
Preferences::EntryDir()
{
	return fEntryDir;
}


status_t
Preferences::SetEntryDir(const char* path)
{
	status_t testStatus = BEntry(path).InitCheck();
	if (testStatus == B_OK)
		fEntryDir = BString(path);
	return testStatus;
}


BString
Preferences::EntryOpenWith()
{
	return fOpenWith;
}


status_t
Preferences::SetEntryOpenWith(const char* binPath)
{
//	status_t testStatus = BEntry(binPath).InitCheck();
//	if (testStatus == B_OK)
		fOpenWith = BString(binPath);
		return B_OK;
//	return testStatus;
}


bool
Preferences::EntryOpenAsHtml()
{
	return fOpenAs == kOpenAsHtml;
}


bool
Preferences::SetEntryOpenAsHtml(bool asHtml)
{
	if (asHtml == true)
		fOpenAs = kOpenAsHtml;
	else
		fOpenAs = kOpenAsUrl;
	return asHtml;
}


