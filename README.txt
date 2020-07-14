POGGER                                                       feed parser/daemon
===============================================================================

Pogger is an Atom/RSS feed parser and daemon, intending to be a sorta-clone of
the Haiku mail daemon.

It can be run as a daemon, or as a simple XML feed parser for turning a feed's
URL or file into a set of post-files, with appropriate metadata attributes.

When run as a daemon, it'll check your configured feeds periodically, and
place any new posts into your ~/feeds/ folder, along with notifying you about
them.


INSTALL
--------------------------------------------------
Just build it, `$ make`, and you're good.
Depends on tinyxml2.


CONFIGURATION
--------------------------------------------------
To add feeds to Pogger, create HTTPS URL files as a link to the RSS/Atom feed 
(as long as the attribute META:url is used you're good), and put them in
`~/config/settings/Pogger/feeds/`.

To configure defaults (i.e., mimetype, output dir, etc), use the `-s` option
when invoking Pogger.


POST FILES
--------------------------------------------------
Each feed's post is represented as a file (much like the mail daemon does with
e-mails), with the following attributes:
	Int-32	unixDate	- contains the date in epoch time
	Text	date    	- Publication/update datetime for the post
	Text	META:title	- Post-title, ofc
	Text	description	- Description/summary of post
	Text	META:url	- URL of the post
	Text	BEOS:TYPE	- Defaults to text/xml, but you can config

The file's content will be the <content> attribute from RSS, or the 
from Atom. If there's no content, then the file will be empty.

Since the default mimetype is text/xml, the default behavior of Tracker is
to open post files in a web-browser, reading them as HTML.

However, you might find it more useful to open post's URLs rather than HTML
content (many don't have content whatsoever, or you might prefer to read from
the source): in that case, you have two options:

	* setting the default mimetype to `application/x-vnd.Be.Url.https`
	  (HTTPS URL), so that your web-browser will open the URL rather than
	  file-content
	* enable auto-mimetype; this will automatically set the type to HTTPS
	  URL
	  you don't specify a default mimetype. It will set all posts to
	  text/xml unless they lack content, in which case they will be an
	  HTTPS URL.


USAGE
--------------------------------------------------
Usage: Pogger [-hvDus] [-m mimetype] [-tT datetime] [-cCO path] 
       Pogger [-hvs] [-mtTcCO] ( <text/xml file> |  <META:url file> | <url> )

Pogger, a RSS and Atom feed parser/daemon.

Options:
  -h, --help       - Print this usage info.
  -v, --verbose    - Print verbose (debug) info.
  -m, --mimetype   - Mimetype of new item files. (Default: text/xml)
  -O, --output     - Output dir for item files. (Default: ~/feeds/)
  -t, --before     - Only return items published before this datetime.
  -T, --after      - Only return items published after this datetime.
  -c, --config     - Path to config dir. (Default: ~/config/settings/Pogger/)
  -C, --cache      - Path to cache.  (Default: ~/config/cache/Pogger/)
  -s, --save       - Save the args of `-m`, `-C`, and `-O` to config.
  -u, --update     - Update all feeds, but don't start daemon.
  -D, --foreground - Run in the foreground, do not daemonize.
                     `-u` and `-D` only apply when running without file/url arg.

When invoked without a file or URL, will search for any new feed items
published since last check by by any 'feed file' placed in the config
directory (default: ~/config/settings/Rifen/feeds/) and create their
corresponding files.

When invoked with a file or URL, will create files from items contained
within the given file or URL.

'Feed files' are files with a 'META:url' attribute containing the
corresponding URL to the feed's XML.

Both -t and -T use the ISO 8601 format for specifying datetimes:
      YYYY-MM-DDTHH:MM:SS - 2020-01-01T07:07:07

NOTE: This message doesn't reflect reality. This is more of a spec of
      what I hope this program will be. As of now, running Pogger
      without a file/url free-argument is invalid, as the daemon
      isn't implemented at all. As such, -D -u and -C are non-functional.
      But it sure can turn an XML feed into files! Lol.


LIES
--------------------------------------------------
Pogger has no daemon yet, it can only be invoked as a standalone feed parser.
Daemon-related args are ignored.
There's no GUI yet, and no Messaging either.
Sry bby, i'm trying <3


BORING INFO
--------------------------------------------------
Pogger is under the MIT license.
https://git.xwx.moe/pogger.git
jadedctrl@teknik.io
