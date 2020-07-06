#include <StorageKit.h>

int  main ( int, char** );
int  usage       ( );
int  invocation  ( int, char**, Config** );
void freeargInvocation  ( int, char**, int, Config** );
bool processItem ( void* );
bool processFeed ( void* );

// ----------------------------------------------------------------------------

BString usageMsg =
	"Usage: %app% [-hvD] [-m mimetype] [-tT datetime] [-O directory] \n"
	"       %app% [-hv] [-mtTO] ( <text/xml file> |  <META:url file> | <url> )\n"
	"\n"
	"%app%, a RSS and Atom feed parser/daemon.\n"
	"\n"
	"Options:\n"
	"  -h, --help       - Print this usage info.\n"
	"  -v, --verbose    - Print verbose (debug) info.\n"
	"  -m, --mimetype   - Mimetype of new item files. (Default: text/xml)\n"
	"  -O, --output     - Output dir for item files. (Default: ~/feeds/)\n"
	"  -t, --before     - Only return items published before this datetime.\n"
	"  -T, --after      - Only return items published after this datetime.\n"
	"  -D, --foreground - Run in the foreground, do not daemonize.\n"
	"                     Only applies when running without file/url arg.\n"
	"\n"
	"When invoked without a file or URL, will search for any new feed items\n"
	"published since last check by by any 'feed file' placed in the config\n"
	"directory (default: ~/config/settings/Rifen/feeds/) and create their\n"
	"corresponding files.\n"
	"\n"
	"When invoked with a file or URL, will create files from items contained\n"
	"within the given file or URL.\n"
	"\n"
	"'Feed files' are files with a 'META:url' attribute containing the\n"
	"corresponding URL to the feed's XML.\n"
	"\n"
	"Both -t and -T use the ISO 8601 format for specifying datetimes:\n"
	"      YYYY-MM-DDTHH:MM:SS - 2020-01-01T07:07:07\n"
	"You can leave out seconds, minutes, or hours, but YMD are required.\n"
	"\n"
	"NOTE: This message doesn't reflect reality. This is more of a spec of\n"
	"      what I hope this program will be. As of now -t and -T aren't\n"
	"      implemented, and running %app% without a file/url free-argument\n"
	"      is invalid, as the daemon isn't implemented at all. As such,\n"
	"      -D is also non-functional.\n"
	"      But it sure can turn an XML feed into files! Lol.\n"
;

