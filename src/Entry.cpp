/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "Entry.h"

#include <iostream>
#include <fstream>

#include <StorageKit.h>

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
Entry::Filetize(BDirectory outDir)
{
	BFile file(fTitle.String(), B_READ_WRITE);
	BEntry outDirEntry;
	time_t tt_date = fDate.Time_t();

	outDir.GetEntry(&outDirEntry);
	if (outDir.InitCheck() == B_ENTRY_NOT_FOUND) {
		outDir.CreateDirectory(BPath(&outDirEntry).Path(), &outDir);
	}
	outDir.CreateFile(fTitle.String(), &file);

	BString betype = BString("text/x-feed-entry");
	file.WriteAttr("BEOS:TYPE", B_MIME_STRING_TYPE, 0, betype.String(),
		betype.CountChars() + 1);

	file.WriteAttr("Feed:name", B_STRING_TYPE, 0,
		fTitle.String(), fTitle.CountChars());
	file.WriteAttr("Feed:description", B_STRING_TYPE, 0,
		fDescription.String(), fDescription.CountChars());
	file.WriteAttr("Feed:source", B_STRING_TYPE, 0,
		fFeedTitle.String(), fFeedTitle.CountChars());
	file.WriteAttr("META:url", B_STRING_TYPE, 0, fPostUrl.String(),
		fPostUrl.CountChars());

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
Entry::SetDate(const char* dateStr)
{
	if (dateStr == NULL)
		return false;
	BDateTime newDate = feedDateToBDate(dateStr);
	if (newDate == NULL)
		return false;
	fDate = newDate;
	return true;
}


bool
Entry::SetDate(tinyxml2::XMLElement* elem)
{
	if (elem != NULL)
		return SetDate(elem->GetText());
	return false;
}


