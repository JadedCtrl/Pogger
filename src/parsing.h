#ifndef PARSE_H
#define PARSE_H

#include <tinyxml2.h>
#include "Config.h"
#include "Channel.h"

void feedParser       ( Channel**, Config* );
void rssParser        ( Channel**, Config*, tinyxml2::XMLDocument* );
void rssRootParse     ( Channel**, Config*, tinyxml2::XMLElement* );
void rssItemParse     ( Channel**, Config*, tinyxml2::XMLElement* );
void rssParseItems    ( Channel**, Config*, tinyxml2::XMLElement* );
void atomParser       ( Channel**, Config*, tinyxml2::XMLDocument* );
void atomRootParse    ( Channel** chanPtr, Config*, tinyxml2::XMLElement* );
void atomEntryParse   ( Channel**, Config*, tinyxml2::XMLElement* );
void atomParseEntries ( Channel**, Config*, tinyxml2::XMLElement* );
int  xmlCountSiblings ( tinyxml2::XMLElement*, const char* );

#endif
