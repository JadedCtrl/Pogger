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
	configDir = BString("/boot/home/config/settings/Pogger/");

	BString filename = BString(configDir);
	filename.Append("settings");
	BFile file(filename.String(), B_READ_ONLY);
	status_t result = file.InitCheck();

	BMessage storage;
	storage.Unflatten(&file);

	updateInterval = storage.GetInt64("updateInterval", 3600000000);
	outDir   = BString(storage.GetString("outDir", "/boot/home/feeds/"));
	cacheDir = BString(storage.GetString("cacheDir",
		"/boot/home/config/cache/Pogger/"));
}


// !! handle file status
void
Preferences::Save ()
{
	if (configDir == NULL)
		configDir = BString("/boot/home/config/settings/Pogger/");

	BPath*		cfgPath		= new BPath(configDir.String(), NULL, true);
	BEntry*		cfgEntry	= new BEntry(cfgPath->Path());
	BDirectory*	cfgDir		= new BDirectory;

	cfgDir->CreateDirectory(cfgPath->Path(), NULL);
	
	if (cfgEntry->Exists() == false)
		cfgDir->CreateDirectory(cfgPath->Path(), NULL);

	BMessage storage;
	BString filename = BString(configDir).Append("/settings");

	BFile* file = new BFile(filename.String(), B_WRITE_ONLY | B_CREATE_FILE
		| B_ERASE_FILE);
	status_t result = file->InitCheck();

	storage.AddFloat("updateInterval", updateInterval);
	storage.AddString("outDir", outDir.String());
	storage.AddString("cacheDir", cacheDir.String());

	storage.Flatten(file);
}

