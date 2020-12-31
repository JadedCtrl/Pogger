/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "Mimetypes.h"

#include <Application.h>
#include <DateTime.h>
#include <Message.h>
#include <MimeType.h>


// install the Feed Entry mimetype, if need be
bool
feedMimeType()
{
	BMessage info;
	BMimeType mime("text/x-feed-entry");
	if (mime.IsInstalled())
		return true;

	mime.GetAttrInfo(&info);

	mime.SetShortDescription("Feed Entry");
	mime.SetLongDescription("Atom/RSS Feed Entry");

	addAttribute(info, "FEED:name", "Name");
	addAttribute(info, "FEED:description", "Description");
	addAttribute(info, "META:url", "URL");
	addAttribute(info, "FEED:source", "Source");
	addAttribute(info, "FEED:when", "When", B_TIME_TYPE, 150);

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


