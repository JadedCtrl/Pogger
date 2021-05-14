/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef SOURCEMANAGER_H
#define SOURCEMANAGER_H

#include <ObjectList.h>

#include "Source.h"

class Feed;

class SourceManager {
public:

	static BObjectList<Source> Sources();

	static Source* GetSource(const char* configPath);
	static Source* GetSource(Feed* feed);
	static Source* GetSourceOfType(const char* sourceType);

	static BObjectList<Feed> Feeds();

	static Feed* GetFeed(const char* identifier, const char* source);

	static bool Fetch(Feed* feed);
	static bool Parse(Feed* feed);

	static void AddFeed(Feed* newFeed);
	static void EditFeed(Feed* updated);
	static void RemoveFeed(Feed* mortonta);

	static bool IsUpdated(Feed* feed);


private:
	static BPath _SourceConfigPath();
};


#endif // SOURCEMANAGER_H
