#ifndef ATOM_FEED_H
#define ATOM_FEED_H

#include <tinyxml2.h>
#include <DateTime.h>
#include <String.h>
#include <List.h>
#include <Url.h>
#include "Config.h"
#include "Feed.h"

class AtomFeed: public Feed {
public:
	AtomFeed ( BString, Config* );

	void	Parse        ( Config* );
	void	RootParse    ( Config*, tinyxml2::XMLElement* );
	void	EntryParse   ( Config*, tinyxml2::XMLElement* );
	void	ParseEntries ( Config*, tinyxml2::XMLElement* );
};

#endif
