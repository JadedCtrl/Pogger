#include <String.h>
#include "Config.h"

Config::Config () {
	verbose    = false;
	daemon     = true;
	mimetype   = BString("text/xml");
	outDir     = BString("/boot/home/feeds/");
}
