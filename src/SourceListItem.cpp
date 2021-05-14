/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "SourceListItem.h"

#include <Catalog.h>
#include <View.h>

#include "Source.h"
#include "Util.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "SourceListItem"


SourceListItem::SourceListItem(Source* source)
	: BStringItem(source->Name(), 0, false),
	fSource(source)
{
}


Source*
SourceListItem::FeedSource()
{
	return fSource;
}


