/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <SupportDefs.h>

class BString;
class BMessage;


enum
{
	kProgress	= 'npro'
};


class Notifier {
public:
	Notifier();

	void MessageReceived(BMessage* msg);

private:
	void _NewEntryNotification(BString feedName, int32 feedCount);
	void _ParseFailNotification(BString feedUrl);
	void _DownloadFailNotification(BString feedUrl);

	void _UpdateProgress();

	int32 fEnqueuedFeeds;
	int32 fFeedsInProgress;
};


#endif // NOTIFIER_H

