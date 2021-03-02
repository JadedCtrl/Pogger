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
class BMessageRunner;
class Feed;


enum
{
	kProgress			= 'npro',
	kEnqueueFeed		= 'fenq',
	kClearQueue			= 'frmq',
	kDownloadStart		= 'fdst',
	kDownloadComplete	= 'fdlc',
	kDownloadFail		= 'fdlf',
	kParseComplete		= 'fpec',
	kParseFail			= 'fpef',
	kUpdateSubscribed	= 'fups',
	kControllerCheck	= 'coch'
};


class FeedController{
public:
	FeedController();
	~FeedController();

	void MessageReceived(BMessage* msg);

	static BList SubscribedFeeds();

private:
	void _SendProgress();

	void _EnqueueFeed(Feed* feed);

	void _ProcessQueueItem();
	void _ReceiveStatus();

	static int32 _DownloadLoop(void* data);
	static int32 _ParseLoop(void* data);

	int32 fEnqueuedTotal;

	thread_id	fMainThread;
	thread_id	fDownloadThread;
	thread_id	fParseThread;

	BList*			fDownloadQueue;
	BMessageRunner*	fMessageRunner;
};


#endif // FEEDCONTROLLER_H

