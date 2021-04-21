/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef PREFS_H
#define PREFS_H

#include <DateTime.h>
#include <String.h>
#include <StorageKit.h>


static const int64 HOUR_IN_MICROSECONDS = 3600000000;

enum
{
	kPreferencesUpdated = 'pfup',
	kOpenAsHtml = 1,
	kOpenAsUrl = 2
};


class Preferences {
public:
	Preferences();
	Preferences(Preferences*);

	void Load();
	void Save();

	int64 UpdateInterval();
	int UpdateIntervalIndex();
	void SetUpdateIntervalIndex(int8 index);

	BString EntryDir();
	status_t SetEntryDir(const char* path);

	BString EntryOpenWith();
	status_t SetEntryOpenWith(const char* binPath);

	bool EntryOpenAsHtml();
	bool SetEntryOpenAsHtml(bool asHtml);

	bool	fNewNotify;
	bool	fFailureNotify;
	int8	fOpenAs;

	BRect	fMainWindowRect;
	BRect	fFeedEditRect;
	int32	fTabSelection;

private:
	void _FileError(status_t result);

	int8 fUpdateInterval;

	BString fEntryDir;
	BString fOpenWith;
};


#endif // PREFS_H

