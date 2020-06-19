#ifndef ITEM_H
#define ITEM_H

#include <DateTime.h>
#include <String.h>
#include <List.h>
#include <Url.h>

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

#endif
