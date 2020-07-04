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
	BString  pubDate;
	BString  homePage;
	BString  postUrl;
	BString  content;
	BString outputDir;
	
	Item	( BString );

	bool	Filetize ( Config*, bool );

	void	SetTitle ( const char* );
	void	SetTitle ( tinyxml2::XMLElement* );
	void	SetDesc ( const char* );
	void	SetDesc ( tinyxml2::XMLElement* );
	void	SetContent ( const char* );
	void	SetContent ( tinyxml2::XMLElement* );
	void	SetPostUrl ( const char* );
	void	SetPostUrl ( tinyxml2::XMLElement* );
	void	SetPubDate ( const char* );
	void	SetPubDate ( tinyxml2::XMLElement* );
};



#endif
