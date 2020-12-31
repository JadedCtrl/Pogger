/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef CONFIG_H
#define CONFIG_H


#include <DateTime.h>
#include <String.h>
#include <StorageKit.h>


class Config {
public:
	Config();
	Config(Config*);

	void Load();
	void Save();
	
	bool verbose;
	bool daemon;
	BString outDir;
	BList targetFeeds; // strings of files or urls

	BDateTime minDate;
	BDateTime maxDate;

	BString configDir;
	BString cacheDir;
	bool will_save;

	bool updateFeeds;
};


#endif

