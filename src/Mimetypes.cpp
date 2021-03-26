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
#include <fs_index.h>
#include <fs_info.h>
#include <VolumeRoster.h>



bool installMimeTypes()
{
	addIndices();
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
	mime.SetLongDescription("Atom/RSS feed");

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
	BMimeType mime("application/x-feed-entry");
	if (mime.IsInstalled())
		return true;

	BResources* res = BApplication::AppResources();
	size_t length = 0;
	const void* data = res->LoadResource(B_VECTOR_ICON_TYPE, "entry-type", &length);
	mime.SetIcon((uint8*)data, length);

	mime.SetPreferredApp("application/x-vnd.Pogger");

	mime.GetAttrInfo(&info);

	mime.SetShortDescription("Feed entry");
	mime.SetLongDescription("Atom/RSS feed entry");

	addAttribute(info, "Feed:name", "Name");
	addAttribute(info, "Feed:description", "Description");
	addAttribute(info, "META:url", "URL");
	addAttribute(info, "Feed:source", "Source");
	addAttribute(info, "Feed:status", "Status");
	addAttribute(info, "Feed:when", "When", B_TIME_TYPE, 150);

	return mime.SetAttrInfo(&info);
}


void
addIndices()
{
	int32 cookie = 0;
	dev_t device;

	while ((device = next_dev(&cookie)) >= B_OK) {
		fs_info info;
		if (fs_stat_dev(device, &info) < 0
			|| (info.flags & B_FS_HAS_QUERY) == 0)
			continue;

		fs_create_index(device, "Feed:name",		B_STRING_TYPE, 0);
		fs_create_index(device, "Feed:source",	B_STRING_TYPE, 0);
		fs_create_index(device, "Feed:status",		B_STRING_TYPE, 0);
		fs_create_index(device, "Feed:when",		B_TIME_TYPE, 0);
	}
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


