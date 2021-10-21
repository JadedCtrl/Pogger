/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef SOURCE_H
#define SOURCE_H

#include <ObjectList.h>

#include "Feed.h"

class BView;


class Source {
public:
	// Constructor should check if the file at configPath exists; if it doesn't,
	// then the source is likely being created, and ConfigView() is about to be
	// called.
	// Source(const char* configPath);

	virtual BObjectList<Feed> Feeds() { return BObjectList<Feed>(); };

	virtual Feed* GetFeed(const char* identifier) { return new Feed(); };

	virtual bool Fetch(Feed* feed) { return false; };
	virtual bool Parse(Feed* feed) { return false;};

	virtual void AddFeed(Feed* newFeed) { };
	virtual void EditFeed(Feed* updated) { };
	virtual void RemoveFeed(Feed* mortonta) { };

	// Check if the given feed has changed since the last update
	virtual bool IsUpdated(Feed* feed) { return false; };

	// Will be shown in config window when editing source
	virtual BView* ConfigView() { return NULL; };
	virtual bool IsConfigurable() { return false; };
	virtual int8 MaxOfSource() { return -1; };

	// Will be used in the feed edit window
	virtual BView* EditFeedView() { return NULL; };

	virtual const char* Type() { return "Source"; };
	virtual const char* Name() { return "Generic"; };
	virtual const char* Config() { return NULL; };

	BString fConfigPath;
};


#endif // SOURCE_H

