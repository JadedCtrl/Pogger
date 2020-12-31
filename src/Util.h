/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef UTIL_H 
#define UTIL_H

#include <DateTime.h>

#include "ProtocolListener.h"

class BUrl;


BDateTime feedDateToBDate(const char*);
BDateTime dateRfc3339ToBDate(const char*);
BDateTime dateRfc822ToBDate(const char*);
BDateTime stringDateToBDate(const char*, const char*);

BString dateTo3339String(BDateTime);

bool withinDateRange(BDateTime, BDateTime, BDateTime);

bool isRemotePath(BString);

int32 webFetch(BUrl, BDataIO*, BString*);
int32 webFetch(BUrl, BDataIO*);


#endif

