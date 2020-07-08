#ifndef RSS_FEED_H
#define RSS_FEED_H

#include <tinyxml2.h>
#include <DateTime.h>
#include <String.h>
#include <List.h>
#include <Url.h>
#include "Config.h"
#include "Feed.h"

class RssFeed: public Feed {
public:
	RssFeed ( BString, BString );

	void	Parse        ( Config* );
	void	RootParse    ( Config*, tinyxml2::XMLElement* );
	void	EntryParse   ( Config*, tinyxml2::XMLElement* );
	void	ParseEntries ( Config*, tinyxml2::XMLElement* );
};

#endif
