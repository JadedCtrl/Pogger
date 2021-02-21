/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "Entry.h"

#include <StorageKit.h>

#include <iostream>
#include <fstream>

#include "Util.h"


Entry::Entry()
{
	title = BString("");
	description = BString("");
	feedTitle = BString("");
	postUrl  = BString("");
	content  = BString("");
}


Entry::~Entry()
{}


bool
Entry::Filetize(BDirectory outDir)
{
	BFile file(title.String(), B_READ_WRITE);
	time_t tt_date = date.Time_t();

	outDir.CreateFile(title.String(), &file);

	BString betype = BString("text/x-feed-entry");
	file.WriteAttr("BEOS:TYPE", B_MIME_STRING_TYPE, 0, betype.String(),
		betype.CountChars() + 1);

	file.WriteAttr("Feed:name", B_STRING_TYPE, 0,
		title.String(), title.CountChars());
	file.WriteAttr("Feed:description", B_STRING_TYPE, 0,
		description.String(), description.CountChars());
	file.WriteAttr("Feed:source", B_STRING_TYPE, 0,
		feedTitle.String(), feedTitle.CountChars());
	file.WriteAttr("META:url", B_STRING_TYPE, 0, postUrl.String(),
		postUrl.CountChars());

	if (date != NULL) {
		file.WriteAttr("Feed:when", B_TIME_TYPE, 0, &tt_date, sizeof(time_t));
	}

	file.Write(content.String(), content.Length());
	return false;
}


bool
Entry::SetTitle(const char* titleStr)
{
	if (titleStr != NULL)
		title = BString(titleStr);
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


bool
Entry::SetDesc(const char* descStr)
{
	if (descStr != NULL)
		description = BString(descStr);
	else return false;
	return true;
}


bool
Entry::SetDesc(tinyxml2::XMLElement* elem)
{
	if (elem != NULL)
		return SetDesc(elem->GetText());
	return false;
}


bool
Entry::SetFeedTitle(BString titleStr)
{
	if (titleStr != NULL)
		feedTitle = titleStr;
	else return false;
	return true;
}


bool
Entry::SetContent(const char* contentStr)
{
	if (contentStr != NULL)
		content = BString(contentStr);
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


bool
Entry::SetPostUrl(const char* urlStr)
{
	if (urlStr != NULL)
		postUrl = BString(urlStr);
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


bool
Entry::SetDate(const char* dateStr)
{
	if (dateStr == NULL)
		return false;
	BDateTime newDate = feedDateToBDate(dateStr);
	if (newDate == NULL)
		return false;
	date = newDate;
	return true;
}


bool
Entry::SetDate(tinyxml2::XMLElement* elem)
{
	if (elem != NULL)
		return SetDate(elem->GetText());
	return false;
}


BDateTime
Entry::GetDate()
{
	return date;
}


