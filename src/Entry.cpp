/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "Entry.h"

#include <iostream>
#include <fstream>

#include <StorageKit.h>

#include "Daemon.h"
#include "Preferences.h"
#include "Util.h"


Entry::Entry()
	:
	fTitle(BString()),
	fDescription(BString()),
	fFeedTitle(BString()),
	fPostUrl(BString()),
	fContent(BString())
{
}


Entry::~Entry()
{}


bool
Entry::Filetize()
{
	BFile file(fTitle.String(), B_READ_WRITE);
	BString outPath = ((App*)be_app)->fPreferences->EntryDir();
	BDirectory outDir(outPath.String());
	time_t tt_date = fDate.Time_t();

	if (outDir.InitCheck() == B_ENTRY_NOT_FOUND) {
		BDirectory().CreateDirectory(outPath.String(), &outDir);
	}
	outDir.CreateFile(fTitle.String(), &file);

	BString betype = BString("application/x-feed-entry");
	file.WriteAttr("BEOS:TYPE", B_MIME_STRING_TYPE, 0, betype.String(),
		betype.CountChars() + 1);

	BString readStatus("New");
	file.WriteAttrString("Feed:name", &fTitle);
	file.WriteAttrString("Feed:description", &fDescription);
	file.WriteAttrString("Feed:source", &fFeedTitle);
	file.WriteAttrString("META:url", &fPostUrl);
	file.WriteAttrString("Feed:status", &readStatus);

	if (fDate != NULL) {
		file.WriteAttr("Feed:when", B_TIME_TYPE, 0, &tt_date, sizeof(time_t));
	}

	file.Write(fContent.String(), fContent.Length());
	return false;
}


BString
Entry::Title()
{
	return fTitle;
}


bool
Entry::SetTitle(const char* titleStr)
{
	if (titleStr != NULL)
		fTitle = BString(titleStr);
	else return false;
	return true;
}


bool
Entry::SetTitle(tinyxml2::XMLElement* elem)
{
	if (elem != NULL)
		return SetTitle(elem->GetText());
	return false;
}


BString
Entry::Description()
{
	return fDescription;
}


bool
Entry::SetDescription(const char* descStr)
{
	if (descStr != NULL)
		fDescription = BString(descStr);
	else return false;
	return true;
}


bool
Entry::SetDescription(tinyxml2::XMLElement* elem)
{
	if (elem != NULL)
		return SetDescription(elem->GetText());
	return false;
}


BString
Entry::FeedTitle()
{
	return fFeedTitle;
}


bool
Entry::SetFeedTitle(BString titleStr)
{
	if (titleStr != NULL)
		fFeedTitle = titleStr;
	else return false;
	return true;
}


BString
Entry::Content()
{
	return fContent;
}


bool
Entry::SetContent(const char* contentStr)
{
	if (contentStr != NULL)
		fContent = BString(contentStr);
	else return false;
	return true;
}


bool
Entry::SetContent(tinyxml2::XMLElement* elem)
{
	if (elem != NULL)
		return SetContent(elem->GetText());
	return false;
}


BString
Entry::PostUrl()
{
	return fPostUrl;
}


bool
Entry::SetPostUrl(const char* urlStr)
{
	if (urlStr != NULL)
		fPostUrl = BString(urlStr);
	else return false;
	return true;
}


bool
Entry::SetPostUrl(tinyxml2::XMLElement* elem)
{
	if (elem != NULL)
		return SetPostUrl(elem->GetText());
	return false;
}


BDateTime
Entry::Date()
{
	return fDate;
}


bool
Entry::SetDate(BDateTime date)
{
	fDate = date;
	return true;
}


