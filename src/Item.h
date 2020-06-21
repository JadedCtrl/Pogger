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
	std::string  content;
	BString outputDir;
	
	BString  subject;

	Item	( BString, BString );

	bool	Filetize ( bool );
};

#endif
