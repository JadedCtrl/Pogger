/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * Copyright 2007-2016, Haiku, Inc. All rights reserved.
 * Copyright 2011, Clemens Zeidler <haiku@clemens-zeidler.de>
 * Copyright 2001-2002 Dr. Zoidberg Enterprises. All rights reserved.
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "Mimetypes.h"

#include <Application.h>
#include <DateTime.h>
#include <Message.h>
#include <MimeType.h>
#include <Resources.h>


bool installMimeTypes()
{
	return (feedMimeType() && feedEntryMimeType());
}


bool
feedMimeType()
{
	BMessage info;
	BMimeType mime("application/x-feed-source");
	if (mime.IsInstalled())
		return true;

	BResources* res = BApplication::AppResources();
	size_t length = 0;
	const void* data = res->LoadResource(B_VECTOR_ICON_TYPE, "feed-type", &length);
	mime.SetIcon((uint8*)data, length);

	mime.SetPreferredApp("application/x-vnd.Pogger");

	mime.GetAttrInfo(&info);

	mime.SetShortDescription("Feed");
	mime.SetLongDescription("Atom/RSS Feed");

	addAttribute(info, "Feed:name", "Name");
	addAttribute(info, "Feed:description", "Description");
	addAttribute(info, "META:url", "URL");
	addAttribute(info, "Feed:when", "Updated", B_TIME_TYPE, 150);
	addAttribute(info, "Feed:hash", "Hash");

	return mime.SetAttrInfo(&info);
}


bool
feedEntryMimeType()
{
	BMessage info;
	BMimeType mime("text/x-feed-entry");
	if (mime.IsInstalled())
		return true;

	BResources* res = BApplication::AppResources();
	size_t length = 0;
	const void* data = res->LoadResource(B_VECTOR_ICON_TYPE, "entry-type", &length);
	mime.SetIcon((uint8*)data, length);

	mime.SetPreferredApp("application/x-vnd.Pogger");

	mime.GetAttrInfo(&info);

	mime.SetShortDescription("Feed Entry");
	mime.SetLongDescription("Atom/RSS Feed Entry");

	addAttribute(info, "Feed:name", "Name");
	addAttribute(info, "Feed:description", "Description");
	addAttribute(info, "META:url", "URL");
	addAttribute(info, "Feed:source", "Source");
	addAttribute(info, "Feed:when", "When", B_TIME_TYPE, 150);

	return mime.SetAttrInfo(&info);
}


// add the given attribute to a BMessage for use as attr info
// borrowed from mailserver (thanks!)
static void
addAttribute(BMessage& msg, const char* name, const char* publicName,
	int32 type, int32 width)
{
	msg.AddString("attr:name", name);
	msg.AddString("attr:public_name", publicName);
	msg.AddInt32("attr:type", type);
	msg.AddInt32("attr:width", width);
	msg.AddInt32("attr:alignment", B_ALIGN_LEFT);
	msg.AddBool("attr:extra", false);
	msg.AddBool("attr:viewable", true);
	msg.AddBool("attr:editable", true);
}


