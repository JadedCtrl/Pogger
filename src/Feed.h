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
	Feed	( BString );
	Feed	( );

	BString	title;
	BString	description;
	BDateTime date;
	BString	homeUrl;
	BString	xmlUrl;
	BString	filePath;
	BString outputDir;
	BList	entries;

	void	Parse	( Config* );

	bool	SetTitle ( const char* );
	bool	SetTitle ( tinyxml2::XMLElement* );
	bool	SetDesc ( const char* );
	bool	SetDesc ( tinyxml2::XMLElement* );
	bool	SetDate ( const char* );
	bool	SetDate ( tinyxml2::XMLElement* );
	bool	SetHomeUrl ( const char* );
	bool	SetHomeUrl ( tinyxml2::XMLElement* );

	bool IsRss ( );
	bool IsAtom ( );

protected:
	int xmlCountSiblings ( tinyxml2::XMLElement*, const char* );
};

#endif
