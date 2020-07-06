#ifndef ITEM_H
#define ITEM_H

#include <iostream>
#include <DateTime.h>
#include <String.h>
#include <List.h>
#include <Url.h>

class Item {
public:
	BString  title;
	BString  description;
	BDateTime pubDate;
	BString  homePage;
	BString  postUrl;
	BString  content;
	BString outputDir;
	
	Item	( BString );

	bool	Filetize ( Config*, bool );

	bool	SetTitle ( const char* );
	bool	SetTitle ( tinyxml2::XMLElement* );
	bool	SetDesc ( const char* );
	bool	SetDesc ( tinyxml2::XMLElement* );
	bool	SetContent ( const char* );
	bool	SetContent ( tinyxml2::XMLElement* );
	bool	SetPostUrl ( const char* );
	bool	SetPostUrl ( tinyxml2::XMLElement* );
	bool	SetPubDate ( const char* );
	bool	SetPubDate ( tinyxml2::XMLElement* );
};



#endif
