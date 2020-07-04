#ifndef CONFIG_H
#define CONFIG_H

#include <StorageKit.h>

class Config {
public:
	bool verbose;
	bool daemon;
	BString mimetype;
	BString outDir;
	BString targetFeed; // file or url

	Config ( );
};

#endif
