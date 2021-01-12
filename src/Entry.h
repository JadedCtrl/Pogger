/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef ENTRY_H
#define ENTRY_H


#include <DateTime.h>
#include <List.h>
#include <String.h>
#include <Url.h>

#include <tinyxml2.h>

#include "Config.h"


class Entry {
public:
	
	Entry();

	bool Filetize(BDirectory outDir);

	bool SetTitle(const char*);
	bool SetTitle(tinyxml2::XMLElement*);
	bool SetDesc(const char*);
	bool SetDesc(tinyxml2::XMLElement*);
	bool SetFeedTitle(BString);
	bool SetContent(const char*);
	bool SetContent(tinyxml2::XMLElement*);
	bool SetPostUrl(const char*);
	bool SetPostUrl(tinyxml2::XMLElement*);
	bool SetDate(const char*);
	bool SetDate(tinyxml2::XMLElement*);
	BDateTime GetDate();

	BString  title;
	BString  description;
	BString  feedTitle;
	BDateTime date;
	BString  postUrl;
	BString  content;
};


#endif

