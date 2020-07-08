#ifndef FEED_H
#define FEED_H

#include <tinyxml2.h>
#include <DateTime.h>
#include <String.h>
#include <List.h>
#include <Url.h>
#include "Config.h"

class Feed {
public:
	char	lang[3];
	BString	title;
	BString	description;
	BDateTime date;
	BString	homeUrl;
	BString	xmlUrl;
	BList	entries;

	BString topLevelSubject;
	BString lastSubject;
	BString	filePath;
	BString outputDir;


	Feed	( BString, BString );

	void	Parse	( Config* );

	bool	SetTitle ( const char* );
	bool	SetTitle ( tinyxml2::XMLElement* );
	bool	SetDesc ( const char* );
	bool	SetDesc ( tinyxml2::XMLElement* );
	bool	SetDate ( const char* );
	bool	SetDate ( tinyxml2::XMLElement* );
	bool	SetHomeUrl ( const char* );
	bool	SetHomeUrl ( tinyxml2::XMLElement* );
};

#endif
