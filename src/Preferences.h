/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef PREFS_H
#define PREFS_H


#include <DateTime.h>
#include <String.h>
#include <StorageKit.h>


const int64 HOUR_IN_MICROSECONDS = 3600000000;


class Preferences {
public:
	Preferences();
	Preferences(Preferences*);

	void Load();
	void Save();

	int64 UpdateInterval();
	int UpdateIntervalIndex();
	void SetUpdateIntervalIndex(int8 index);

	bool NotifyOnFailure();
	bool NotifyOnNew();
	void SetNotifyOnFailure(bool value);
	void SetNotifyOnNew(bool value);

	
	BString fEntryDir;
	BString fEntryFileExt;
	bool fOpenAsHtml;
	BString fOpenWith;


private:
	int8 fUpdateInterval;

	bool fNewNotify;
	bool fFailureNotify;
};


#endif // PREFS_H

