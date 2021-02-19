POGGER                                                       feed parser/daemon
===============================================================================

Pogger is an Atom/RSS feed parser and daemon, intending to be a sorta-clone of
the Haiku mail daemon.

It can be run as a daemon, or as a simple XML feed parser for turning a feed's
URL or file into a set of post-files, with appropriate metadata attributes.

When run as a daemon, it'll check your configured feeds periodically, and
place any new posts into your ~/feeds/ folder, along with notifying you about
them.

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


LIES
--------------------------------------------------
This README is bonkers out-of-date, and has several lies. I'll update it at some
point soon.


BORING INFO
--------------------------------------------------
Pogger is under the MIT license.
https://github.com/JadedCtrl/Pogger
jadedctrl@teknik.io
