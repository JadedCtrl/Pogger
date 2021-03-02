/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <SupportDefs.h>

class BList;
class BString;
class BMessage;


class Notifier {
public:
	Notifier();

	void MessageReceived(BMessage* msg);

private:
	void _SendNotifications();
	void _SendUpdatedNotification();
	void _SendFailedNotification();

	void _SaveUpdated(BString feedName, BString feedUrl, int32 feedCount);
	void _SaveFailed(BString feedName, BString feedUrl);

	BList*	fFailedFeeds;
	BList*	fUpdatedFeeds;
	int32	fTotalEntries;
};


#endif // NOTIFIER_H

