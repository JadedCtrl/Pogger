/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * Copyight 2017 Akshay Agarwal, agarwal.akshay.akshay8@gmail.com
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "Util.h"

#include <iostream>
#include <iomanip>

#include <Alert.h>
#include <Application.h>
#include <Bitmap.h>
#include <Directory.h>
#include <File.h>
#include <IconUtils.h>
#include <OS.h>
#include <Resources.h>
#include <Url.h>
#include <UrlProtocolRoster.h>
#include <UrlRequest.h>

#include <boost/uuid/detail/sha1.hpp>

#include "ProtocolListener.h"


BDateTime
feedDateToBDate(const char* dateCStr)
{
	BDateTime date = dateRfc822ToBDate(dateCStr);
	if (date == NULL)
		date = dateRfc3339ToBDate(dateCStr);
	return date;
}


BDateTime
dateRfc3339ToBDate(const char* dateCStr)
{
	return stringDateToBDate(dateCStr, "%Y-%m-%dT%H:%M:%S");
}


BDateTime
dateRfc822ToBDate(const char* dateCStr)
{
	return stringDateToBDate(dateCStr, "%a, %d %b %Y %H:%M:%S");
}


BDateTime
stringDateToBDate(const char* dateCStr, const char* templateCStr)
{
	std::istringstream dateStream(dateCStr);
	std::tm time = {};

	if (dateStream >> std::get_time(&time, templateCStr)) {
		BTime newTime = BTime(time.tm_hour, time.tm_min, time.tm_sec, 0);
		BDate newDate = BDate(time.tm_year + 1900, time.tm_mon + 1,
			time.tm_mday);
		return BDateTime(newDate, newTime);
	}
	return NULL;
}


BString
dateTo3339String(BDateTime dt)
{
	char buffer[18];
	sprintf(buffer, "%i-%02i-%02iT%02i:%02i:%02i",
		dt.Date().Year(), dt.Date().Month(), dt.Date().Day(),
		dt.Time().Hour(), dt.Time().Minute(), dt.Time().Second());
	
	return BString(buffer);
}


bool
withinDateRange(BDateTime minDate, BDateTime nowDate, BDateTime maxDate)
{
	if (minDate != NULL && nowDate != NULL && maxDate != NULL)
		return (minDate < nowDate && nowDate < maxDate);
	if (minDate != NULL && nowDate != NULL)
		return (minDate < nowDate);
	if (maxDate != NULL && nowDate != NULL)
		return (nowDate < maxDate);
	return true;
}


// return whether or not the given path is remote
bool
isRemotePath(BString path)
{
	BUrl givenUrl = BUrl(path);
	BString protocol = givenUrl.Protocol().String();

	if (protocol == NULL || protocol == BString("file")
		|| givenUrl.UrlString() == NULL)
		return false;
	return true;
}


BString
urlToFilename(BUrl url)
{
	BString protocol = url.Protocol().String();

	if (protocol == NULL && url.UrlString() != NULL) {
		return BString("");
	}

	BString filename = url.Host();
	filename.Append(url.Path());
	filename.ReplaceAll("/", "_");

	return filename;
}


const char*
tempFileName(const char* dir, const char* name, const char* suffix)
{
	BString tmpDir("/tmp/");
	tmpDir << dir;
	BDirectory().CreateDirectory(tmpDir.String(), NULL);

	tmpDir << "/" << name << "-" << (rand() % 1000) << "." << suffix;
	return tmpDir.String();
}


int32
fetch(BUrl url, BDataIO* reply, BString* hash, int timeout)
{
	ProtocolListener listener(true);
	boost::uuids::detail::sha1 sha1;

	#ifdef LIBNETAPI_DEPRECATED
	BUrlRequest* request = BUrlProtocolRoster::MakeRequest(url, &listener);
	listener.SetDownloadIO(reply);
	#else
	BUrlRequest* request = BUrlProtocolRoster::MakeRequest(url, reply, &listener);
	listener.SetDownloadIO(NULL);
	#endif

	listener.SetSha1(&sha1);

	time_t startTime = time(0);
	thread_id thread = request->Run();
	thread_info t_info;

	while (time(0) - startTime < timeout
		&& get_thread_info(thread, &t_info) == B_OK)
		snooze(100000);

	kill_thread(thread);

	*(hash) = listener.GetHash();
	return request->Status();
}


void
userFileError(status_t status, const char* title, const char* bad_value,
	const char* perm_denied, const char* no_memory)
{
	BString label;

	switch (status) {
		case B_BAD_VALUE:
		{
			label = bad_value;
			break;
		}
		case B_READ_ONLY_DEVICE:
		case B_PERMISSION_DENIED:
		{
			label = perm_denied;
			break;
		}
		case B_NO_MEMORY:
		{
			label = no_memory;
			break;
		}
		default:
			return;
	}

	BAlert* alert = new BAlert(title, label.String(), "OK", NULL, NULL,
		B_WIDTH_AS_USUAL, B_WARNING_ALERT);
	alert->Go();
}


entry_ref
tempHtmlFile(entry_ref* ref, const char* title)
{
	entry_ref tempRef;
	BEntry(tempFileName("Pogger", title, "html")).GetRef(&tempRef);

	BFile ogFile(ref, B_READ_ONLY);
	BFile htmlFile(&tempRef, B_WRITE_ONLY | B_CREATE_FILE);

	char readBuf[100] = {'\0'};
	ssize_t readLen = 1;

	BString head("<html>\n<head>\n<title>");
	head << title << "</title>\n</head>\n<body>\n";
	BString tail("</body>\n</html>\n");
	const void* headBuf = head.String();
	const void* tailBuf = tail.String();

	htmlFile.Write(headBuf, head.Length());

	while (readLen > 0) {
		readLen = ogFile.Read(readBuf, 100);
		htmlFile.Write(readBuf, readLen);
	}

	htmlFile.Write(tailBuf, tail.Length());

	return tempRef;
}


BBitmap*
loadVectorIcon(const char* name, int32 iconSize, int32 cropSize)
{
	BResources* res = BApplication::AppResources();
	size_t length = 0;
	const void* data = res->LoadResource(B_VECTOR_ICON_TYPE, name, &length);
	BBitmap* temp = new BBitmap(BRect(0, 0, iconSize - 1, iconSize - 1),
		B_BITMAP_NO_SERVER_LINK, B_RGBA32);
	BBitmap* dest = new BBitmap(BRect(0, 0, cropSize - 1, cropSize - 1),
		B_RGBA32);
	if (data != NULL
		&& BIconUtils::GetVectorIcon((uint8*)data, length, temp)
			== B_OK
		&& dest->ImportBits(temp, BPoint(0, 0), BPoint(0, 0),
			cropSize, cropSize) == B_OK)  {
			delete temp;
			return dest;
	}

	delete temp;
	delete dest;
	return NULL;
}


