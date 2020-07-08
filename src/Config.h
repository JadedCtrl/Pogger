#ifndef CONFIG_H
#define CONFIG_H

#include <DateTime.h>
#include <String.h>
#include <StorageKit.h>

class Config {
public:
	bool verbose;
	bool daemon;
	BString mimetype;
	BString outDir;
	BList targetFeeds; // strings of files or urls

	BDateTime minDate;
	BDateTime maxDate;

	BString configPath;
	bool will_save;

	Config ( );

	void Load ( );
	void Save ( );
};

#endif
