#include <DateTime.h>
#include <String.h>
#include <List.h>
#include <Url.h>

class Channel {
public:
	char	lang[3];
	BString	title;
	BString	description;
	BDate	lastBuildDate;
	BString	homePage;
	BString	xmlUrl;
	BList	items;

	BString topLevelSubject;
	BString lastSubject;
	BString	filePath;


	Channel	( int );
//	Channel	( BEntry );
//	Channel	( BUrl );
	void	Parse	( void );
};

class Item {
public:
	BString  title;
	BString  description;
	BDate    pubDate;
	BString  homePage;
	BString  postUrl;
	BString  content;
	
	BString  subject;

	void	Print	( void );
	Item	( BString );
};
