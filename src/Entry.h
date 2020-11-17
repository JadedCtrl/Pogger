#ifndef ENTRY_H
#define ENTRY_H

#include <DateTime.h>
#include <String.h>
#include <List.h>
#include <Url.h>
#include "Config.h"

class Entry {
public:
	BString  title;
	BString  description;
	BString  feedTitle;
	BDateTime date;
	BString  postUrl;
	BString  content;
	BString  outputDir;
	
	Entry	( BString );

	bool	Filetize ( bool );

	bool	SetTitle ( const char* );
	bool	SetTitle ( tinyxml2::XMLElement* );
	bool	SetDesc ( const char* );
	bool	SetDesc ( tinyxml2::XMLElement* );
	bool	SetFeedTitle ( BString );
	bool	SetContent ( const char* );
	bool	SetContent ( tinyxml2::XMLElement* );
	bool	SetPostUrl ( const char* );
	bool	SetPostUrl ( tinyxml2::XMLElement* );
	bool	SetDate ( const char* );
	bool	SetDate ( tinyxml2::XMLElement* );
};

#endif
