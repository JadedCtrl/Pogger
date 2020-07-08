#ifndef PARSE_H
#define PARSE_H

#include <tinyxml2.h>
#include "Config.h"
#include "Feed.h"

void feedParser       ( Feed**, Config* );
void rssParser        ( Feed**, Config*, tinyxml2::XMLDocument* );
void rssRootParse     ( Feed**, Config*, tinyxml2::XMLElement* );
void rssEntryParse     ( Feed**, Config*, tinyxml2::XMLElement* );
void rssParseEntries    ( Feed**, Config*, tinyxml2::XMLElement* );
void atomParser       ( Feed**, Config*, tinyxml2::XMLDocument* );
void atomRootParse    ( Feed**, Config*, tinyxml2::XMLElement* );
void atomEntryParse   ( Feed**, Config*, tinyxml2::XMLElement* );
void atomParseEntries ( Feed**, Config*, tinyxml2::XMLElement* );
int  xmlCountSiblings ( tinyxml2::XMLElement*, const char* );

#endif
