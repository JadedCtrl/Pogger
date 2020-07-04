#ifndef CHANNEL_H
#define CHANNEL_H

#include <tinyxml2.h>
#include <DateTime.h>
#include <String.h>
#include <List.h>
#include <Url.h>
#include "Config.h"

class Channel {
public:
	char	lang[3];
	BString	title;
	BString	description;
	BString	lastDate;
	BString	homePage;
	BString	xmlUrl;
	BList	items;

	BString topLevelSubject;
	BString lastSubject;
	BString	filePath;
	BString outputDir;


	Channel	( BString, BString );
//	Channel	( BEntry );
//	Channel	( BUrl );
	void	Parse	( Config* );

	bool	SetTitle ( const char* );
	bool	SetTitle ( tinyxml2::XMLElement* );
	bool	SetDesc ( const char* );
	bool	SetDesc ( tinyxml2::XMLElement* );
	bool	SetLastDate ( const char* );
	bool	SetLastDate ( tinyxml2::XMLElement* );
	bool	SetHomePage ( const char* );
	bool	SetHomePage ( tinyxml2::XMLElement* );
};

#endif
