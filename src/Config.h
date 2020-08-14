#ifndef CONFIG_H
#define CONFIG_H

#include <DateTime.h>
#include <String.h>
#include <StorageKit.h>

class Config {
public:
	Config ( );
	Config ( Config* );
	
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

	void Load ( );
	void Save ( );
};

#endif
