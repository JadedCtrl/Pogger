/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <SupportDefs.h>

class BString;


class Notifier {
public:
	void MessageReceived(BMessage* msg);

private:
	void _NewEntryNotification(BString feedName, int32 feedCount);
	void _ParseFailNotification(BString feedUrl);
	void _DownloadFailNotification(BString feedUrl);

};


#endif // NOTIFIER_H

