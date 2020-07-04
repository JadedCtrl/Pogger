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

	void	SetTitle ( const char* );
	void	SetTitle ( tinyxml2::XMLElement* );
	void	SetDesc ( const char* );
	void	SetDesc ( tinyxml2::XMLElement* );
	void	SetLastDate ( const char* );
	void	SetLastDate ( tinyxml2::XMLElement* );
	void	SetHomePage ( const char* );
	void	SetHomePage ( tinyxml2::XMLElement* );
};

#endif
