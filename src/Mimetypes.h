/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef MIME_H
#define MIME_H

#include <SupportDefs.h>
#include <TypeConstants.h>


class BMessage;


bool installMimeTypes();
bool feedMimeType();
bool feedEntryMimeType();
void addIndices();

static void addAttribute(BMessage&, const char*, const char*,
	int32 type = B_STRING_TYPE, int32 width = 200);


#endif

