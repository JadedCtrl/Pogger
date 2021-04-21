/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "LocalSource.h"

#include <Directory.h>
#include <Entry.h>
#include <FindDirectory.h>
#include <Path.h>

#include "Feed.h"


BStringList
LocalSource::Feeds()
{
	BPath subPath;
	find_directory(B_USER_SETTINGS_DIRECTORY, &subPath);
	subPath.Append("Pogger");
	subPath.Append("Subscriptions");
	BDirectory subDir(subPath.Path());
	if (subDir.InitCheck() == B_ENTRY_NOT_FOUND) {
		subDir.CreateDirectory(subPath.Path(), &subDir);

		subPath.Append("Haiku Project");
		Feed defaultSub(BUrl("https://www.haiku-os.org/blog/index.xml"),
			BEntry(subPath.Path()));
		defaultSub.SetTitle("Haiku Project");
		defaultSub.Filetize();
	}

	BEntry feedEntry;
	BPath feedPath;
	BStringList feeds;

	while (subDir.GetNextEntry(&feedEntry) == B_OK
		&& feedEntry.GetPath(&feedPath) == B_OK)
		feeds.Add(feedPath.Path());
	return feeds;
}


