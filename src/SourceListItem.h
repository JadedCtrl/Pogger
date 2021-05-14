/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef SOURCEITEM_H
#define SOURCEITEM_H

#include <StringItem.h>

class Source;


class SourceListItem : public BStringItem
{
public:
	SourceListItem(Source* source);

	Source* FeedSource();

private:
	Source* fSource;
};


#endif // SOURCEITEM_H

