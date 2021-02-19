/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef FEEDCONTROLLER_H
#define FEEDCONTROLLER_H

#include <SupportDefs.h>
#include <OS.h>

class BList;
class BMessage;


enum
{
	kEnqueueFeed		= 'fenq',
	kClearQueue			= 'frmq',
	kDownloadStart		= 'fdst',
	kDownloadComplete	= 'fdlc',
	kDownloadFail		= 'fdlf',
	kParseComplete		= 'fpec',
	kParseFail			= 'fpef',
	kUpdateSubscribed	= 'fups'
};


class FeedController{
public:
	FeedController();
	~FeedController();
	void MessageReceived(BMessage* msg);
	static BList SubscribedFeeds();

private:
	static int32 _DownloadLoop(void* ignored);
	static int32 _ParseLoop(void* ignored);

	thread_id	fDownloadThread;
	thread_id	fParseThread;
};


#endif // FEEDCONTROLLER_H

