#ifndef CHANNEL_H
#define CHANNEL_H

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


	Channel	( BString );
//	Channel	( BEntry );
//	Channel	( BUrl );
	void	Parse	( void );
};

#endif
