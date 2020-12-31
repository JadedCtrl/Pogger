/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "Util.h"

#include <Url.h>
#include <UrlProtocolRoster.h>
#include <UrlRequest.h>

#include <boost/uuid/detail/sha1.hpp>
#include <iostream>
#include <iomanip>

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
isRemotePath(BString path) {
	BUrl givenUrl = BUrl(path);
	BString protocol = givenUrl.Protocol().String();

	if (protocol == NULL || protocol == BString("file")
		|| givenUrl.UrlString() == NULL)
		return false;
	return true;
}


int32
webFetch(BUrl url, BDataIO* reply)
{
	BString* ignored = new BString();
	return webFetch(url, reply, ignored);
}


int32
webFetch(BUrl url, BDataIO* reply, BString* hash)
{
	ProtocolListener listener(true);
	boost::uuids::detail::sha1 sha1;

	BUrlRequest* request = BUrlProtocolRoster::MakeRequest(url, &listener);

	listener.SetDownloadIO(reply);
	listener.SetSha1(&sha1);

	thread_id thread = request->Run();
	wait_for_thread(thread, NULL);

	*(hash) = listener.GetHash();

	return request->Status();
	return 200;
}


