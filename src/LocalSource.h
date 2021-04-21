/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef LOCALSOURCE_H
#define LOCALSOURCE_H

#include "FeedSource.h"

class LocalSource : public FeedSource {
public:
	static BStringList Feeds();

	const char* name = "Local";
};


#endif // LOCALSOURCE_H

