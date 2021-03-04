/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * Copyight 2017 Akshay Agarwal, agarwal.akshay.akshay8@gmail.com
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef UTIL_H 
#define UTIL_H

#include <DateTime.h>
#include <Entry.h>

#include "ProtocolListener.h"

class BBitmap;
class BUrl;


BDateTime feedDateToBDate(const char*);
BDateTime dateRfc3339ToBDate(const char*);
BDateTime dateRfc822ToBDate(const char*);
BDateTime stringDateToBDate(const char*, const char*);

BString dateTo3339String(BDateTime);

bool withinDateRange(BDateTime, BDateTime, BDateTime);

bool isRemotePath(BString);

BString urlToFilename(BUrl url);
const char* tempFileName(const char* dir, const char* name, const char* suffix);

int32 fetch(BUrl url, BDataIO* reply, BString* hash, int timeout);

// The strings passed to this function are adapted from those in
// Calendar (utils/Preferences.cpp).
void userFileError(status_t status, const char* title, const char* bad_value,
	const char* perm_denied, const char* no_memory);

// Takes a (probably invalid) bit of HTML and adds necessary elements in a
// temporary copy. Returns an entry_ref to said temporary copy.
entry_ref tempHtmlFile(entry_ref* ref, const char* title);

// Also ripped from Calendar (utils/ResourceLoader.cpp)
BBitmap* loadVectorIcon(const char* name, int32 iconSize = 32,
	int32 cropSize = 22);


#endif // UTIL_H

