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


class Entry {
public:
	
	Entry();
	~Entry();

	bool Filetize();

	BString Title();
	bool SetTitle(const char*);
	bool SetTitle(tinyxml2::XMLElement*);

	BString Description();
	bool SetDescription(const char*);
	bool SetDescription(tinyxml2::XMLElement*);

	BString FeedTitle();
	bool SetFeedTitle(BString);

	BString	Content();
	bool SetContent(const char*);
	bool SetContent(tinyxml2::XMLElement*);

	BString	PostUrl();
	bool	SetPostUrl(const char*);
	bool	SetPostUrl(tinyxml2::XMLElement*);

	BDateTime	Date();
	bool	SetDate(const char*);
	bool	SetDate(tinyxml2::XMLElement*);


private:
	BString  fTitle;
	BString  fDescription;
	BString  fFeedTitle;
	BDateTime fDate;
	BString  fPostUrl;
	BString  fContent;
};


#endif

