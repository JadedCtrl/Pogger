/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "Preferences.h"

#include <String.h>

#include "Util.h"


Preferences::Preferences() {
}


Preferences::Preferences(Preferences* pref) {
}


void
Preferences::Load()
{
	BPath cfgPath;
	find_directory(B_USER_SETTINGS_DIRECTORY, &cfgPath);
	cfgPath.Append("Pogger");
	if (BDirectory(cfgPath.Path()).InitCheck() == B_ENTRY_NOT_FOUND) {
		BDirectory(cfgPath.Path()).CreateDirectory(cfgPath.Path(), NULL);
	}

	BString filename = BString(cfgPath.Path()).Append("/Settings");

	BFile file = BFile(filename.String(), B_READ_ONLY);
	status_t result = file.InitCheck();

	if (result == B_PERMISSION_DENIED || result == B_NO_MEMORY)
		_FileError(result);

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


void
Preferences::Save()
{
	BPath cfgPath;
	find_directory(B_USER_SETTINGS_DIRECTORY, &cfgPath);
	cfgPath.Append("Pogger");
	BEntry		cfgEntry	= BEntry(cfgPath.Path());
	BDirectory	cfgDir;

	cfgDir.CreateDirectory(cfgPath.Path(), NULL);
	
	if (cfgEntry.Exists() == false)
		cfgDir.CreateDirectory(cfgPath.Path(), NULL);

	BMessage storage;
	BString filename = BString(cfgPath.Path()).Append("/Settings");

	BFile file = BFile(filename.String(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
	status_t result = file.InitCheck();
	if (result != B_OK) {
		_FileError(result);
		return;
	}

	storage.AddString("entryDir", fEntryDir.String());
	storage.AddString("openWith", fOpenWith.String());
	storage.AddInt8("openAs", fOpenAs);

	storage.AddBool("notifyNew", fNewNotify);
	storage.AddBool("notifyFailure", fFailureNotify);
	storage.AddInt8("updateInterval", fUpdateInterval);

	storage.AddRect("mainWindow", fMainWindowRect);
	storage.AddRect("feedEditWindow", fFeedEditRect);
	storage.AddInt32("tabSelection", fTabSelection);

	storage.Flatten(&file);
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
	status_t testStatus = BDirectory(path).InitCheck();
	if (BDirectory(path).IsDirectory() == false)
		return B_NOT_A_DIRECTORY;

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


void
Preferences::_FileError(status_t result)
{
	BPath cfgPath;
	find_directory(B_USER_SETTINGS_DIRECTORY, &cfgPath);

	BString permLabel("Couldn't open the preferences file because permission "
	"was denied.\nThis usually means that you don't have read permissions to "
	"your settings directory.\nPlease make sure that your user has "
	"read-access to your settings directory― likely %path%.\nCheck your OS "
	"documentation for more information.");
	permLabel.ReplaceAll("%path%", cfgPath.Path());

	userFileError(result, "Preferences file",
		"Couldn't open the preferences file because the path is not "
	"specified.\nThis usually means that the programmer made a mistake.\n"
	"Please submit a bug report to the Pogger repository on GitHub.\n"
	"Your personal settings will not be loaded.",
		permLabel.String(),
		"There is not enough memory available on your system to load the "
	"preferences file.\nPlease try closing a few applications and restarting "
	"Pogger.");
}


