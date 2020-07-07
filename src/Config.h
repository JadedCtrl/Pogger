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
	BList targetFeeds; // file or url

	BDateTime minDate;
	BDateTime maxDate;

	Config ( );
};

#endif
