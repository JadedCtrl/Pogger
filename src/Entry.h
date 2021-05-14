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


class Entry {
public:
	
	Entry();
	~Entry();

	bool Filetize(const char* outDirPath);

	BString Title();
	bool SetTitle(const char*);

	BString Description();
	bool SetDescription(const char*);

	BString FeedTitle();
	bool SetFeedTitle(BString);

	BString	Content();
	bool SetContent(const char*);

	BString	PostUrl();
	bool	SetPostUrl(const char*);

	BDateTime	Date();
	bool	SetDate(BDateTime date);

private:
	BString  fTitle;
	BString  fDescription;
	BString  fFeedTitle;
	BString  fFeedAddOn;
	BDateTime fDate;
	BString  fPostUrl;
	BString  fContent;
};


#endif

