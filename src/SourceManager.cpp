/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "SourceManager.h"

#include <iostream>

#include <Catalog.h>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <FindDirectory.h>
#include <Path.h>

#include "Feed.h"
#include "Source.h"
#include "sources/RssAtom.h"
#include "Util.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "SourceManager"


BObjectList<Source>
SourceManager::Sources()
{
	BDirectory sourceDir = BDirectory(_SourceConfigPath().Path());
	BEntry sourceEntry;
	BPath sourcePath;

	BObjectList<Source> sources;

	while (sourceDir.GetNextEntry(&sourceEntry) == B_OK
		&& sourceEntry.GetPath(&sourcePath) == B_OK)
	{
		Source* source = GetSource(sourcePath.Path());
		if (source != NULL)
			sources.AddItem(source);
	}

	// If no sources, create a default RssAtom one
	if (sources.CountItems() == 0) {
		sourcePath = _SourceConfigPath();
		sourcePath.Append("RSS-Atom");
		RssAtom(sourcePath.Path());
		sources.AddItem(GetSource(sourcePath.Path()));
	}

	return sources;
}


Source*
SourceManager::GetSource(const char* configPath)
{
	BFile config(configPath, B_READ_ONLY);
	BMessage storage;
	storage.Unflatten(&config);
	BString sourceType;

	if (storage.FindString("source_type", &sourceType) == B_OK)
		if (sourceType == BString("RssAtom"))
			return new RssAtom(configPath);
	return NULL;
}


Source*
SourceManager::GetSource(Feed* feed)
{
	return GetSource(feed->Source());
}


Source*
SourceManager::GetSourceOfType(const char* sourceType)
{
	BObjectList<Source> sources = Sources();

	for (int i = 0; i < sources.CountItems(); i++) {
		if (sources.ItemAt(i)->Type() == BString(sourceType))
			return sources.ItemAt(i);
	}
	return NULL;
}


BObjectList<Feed>
SourceManager::Feeds()
{
	BObjectList<Source> sources = Sources();
	BObjectList<Feed> feeds;

	for (int i = 0; i < sources.CountItems(); i++) {
		Source* sauce = sources.ItemAt(i);
		BObjectList<Feed> newFeeds = sources.ItemAt(i)->Feeds();
		feeds.AddList(&newFeeds);
	}

	return feeds;
}


Feed*
SourceManager::GetFeed(const char* identifier, const char* source)
{
	return GetSource(source)->GetFeed(identifier);
}


bool
SourceManager::Fetch(Feed* feed)
{
	return GetSource(feed)->Fetch(feed);
}


bool
SourceManager::Parse(Feed* feed)
{
	return GetSource(feed)->Parse(feed);
}


void
SourceManager::AddFeed(Feed* newFeed)
{
	return GetSource(newFeed)->AddFeed(newFeed);
}


void
SourceManager::EditFeed(Feed* updated)
{
	return GetSource(updated)->EditFeed(updated);
}


void
SourceManager::RemoveFeed(Feed* mortonta)
{
	return GetSource(mortonta)->RemoveFeed(mortonta);
}


bool
SourceManager::IsUpdated(Feed* feed)
{
	return GetSource(feed)->IsUpdated(feed);
}


BPath
SourceManager::_SourceConfigPath()
{
	BPath sourcePath;
	find_directory(B_USER_SETTINGS_DIRECTORY, &sourcePath);
	sourcePath.Append("Pogger");
	sourcePath.Append("Sources");

	BDirectory sourceDir = BDirectory(sourcePath.Path());
	if (sourceDir.InitCheck() == B_ENTRY_NOT_FOUND)
		sourceDir.CreateDirectory(sourcePath.Path(), &sourceDir);
	
	return sourcePath;
}


