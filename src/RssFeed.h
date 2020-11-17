#ifndef RSS_FEED_H
#define RSS_FEED_H

#include <tinyxml2.h>
#include <DateTime.h>
#include <String.h>
#include <List.h>
#include <Url.h>
#include "Feed.h"

class RssFeed: public Feed {
public:
	RssFeed ( );
	RssFeed ( Feed* );

	void	Parse        ( );
	void	RootParse    ( tinyxml2::XMLElement* );
	void	EntryParse   ( tinyxml2::XMLElement* );
	void	ParseEntries ( tinyxml2::XMLElement* );
};

#endif
