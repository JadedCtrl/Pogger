/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "Preferences.h"

#include <String.h>

#include <iostream>


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
	fEntryFileExt	= BString(storage.GetString("entryExt", ""));
	fOpenAsHtml		= storage.GetBool("openAsHtml", false);
	fOpenWith		= BString(storage.GetString("openWith", "WebPositive"));

	fNewNotify		= storage.GetBool("notifyNew", true);
	fFailureNotify	= storage.GetBool("notifyFailure", true);
	fUpdateInterval	= storage.GetInt8("updateInterval", 1);
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
	storage.AddString("entryExt", fEntryFileExt.String());
	storage.AddBool("openAsHtml", fOpenAsHtml);
	storage.AddString("openWith", fOpenWith.String());

	storage.AddBool("notifyNew", fNewNotify);
	storage.AddBool("notifyFailure", fFailureNotify);
	storage.AddInt8("updateInterval", fUpdateInterval);

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


bool
Preferences::NotifyOnFailure()
{
	return fFailureNotify;
}


bool
Preferences::NotifyOnNew()
{
	return fNewNotify;
}


void
Preferences::SetNotifyOnFailure(bool value)
{
	fFailureNotify = value;
}


void
Preferences::SetNotifyOnNew(bool value)
{
	fNewNotify = value;
}


