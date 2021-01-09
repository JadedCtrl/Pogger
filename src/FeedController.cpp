/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "FeedController.h"

#include <Message.h>

#include <cstdio>

#include "App.h"
#include "AtomFeed.h"
#include "Entry.h"
#include "RssFeed.h"


FeedController::FeedController()
	:
	fDownloadThread(0),
	fParseThread(0)
{
	fDownloadThread = spawn_thread(_DownloadLoop, "here, eat this",
		B_NORMAL_PRIORITY, NULL);
	fParseThread = spawn_thread(_ParseLoop, "oki tnx nomnomnom",
		B_NORMAL_PRIORITY, NULL);
	resume_thread(fDownloadThread);
	resume_thread(fParseThread);
}


FeedController::~FeedController()
{
	kill_thread(fDownloadThread);
	kill_thread(fParseThread);
}


void
FeedController::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case kEnqueueFeed:
		{
			int i = 0;
			const void* data;
			ssize_t size = sizeof(Feed);

			while (msg->HasData("feeds", B_RAW_TYPE, i)) {
				msg->FindData("feeds", B_RAW_TYPE, i, &data, &size);
				send_data(fDownloadThread, msg->what, data, size);
				i++;
			}
			break;
		}
		case kClearQueue:
		{
			break;
		}
		case kDownloadComplete:
		{
			int i = 0;
			const void* data;
			ssize_t size = sizeof(Feed);

			while (msg->HasData("feeds", B_RAW_TYPE, i)) {
				msg->FindData("feeds", B_RAW_TYPE, i, &data, &size);
				if (((Feed*)data)->IsUpdated() == true)
					send_data(fParseThread, msg->what, data, size);
				i++;
			}
			break;
		}
		default:
		{
//			BWindow::MessageReceived(msg);
			break;
		}
	}
}


int32
FeedController::_DownloadLoop(void* ignored)
{
	thread_id sender;
	Feed* feedBuffer = new Feed();

	while (receive_data(&sender, (void*)feedBuffer, sizeof(Feed)) != 0) {
		feedBuffer->FetchRemoteFeed();

		BMessage* downloaded = new BMessage(kDownloadComplete);
		downloaded->AddData("feeds", B_RAW_TYPE, feedBuffer, sizeof(Feed));


		((App*)be_app)->MessageReceived(downloaded);
	}

	delete(feedBuffer);
	return 0;
}


int32
FeedController::_ParseLoop(void* ignored)
{
	thread_id sender;
	Feed* feedBuffer = new Feed();

	while (receive_data(&sender, (void*)feedBuffer, sizeof(Feed)) != 0) {
		BList entries;

		if (feedBuffer->IsAtom()) {
			AtomFeed* feed = (AtomFeed*)malloc(sizeof(AtomFeed));
			feed = new AtomFeed(feedBuffer);
			feed->Parse();
			entries = feed->GetEntries();
			delete(feed);
		}
		if (feedBuffer->IsRss()) {
			RssFeed* feed = (RssFeed*)malloc(sizeof(RssFeed));
			feed = new RssFeed(feedBuffer);
			feed->Parse();
			entries = feed->GetEntries();
			delete(feed);
		}

		for (int i = 0; i < entries.CountItems(); i++)
			((Entry*)entries.ItemAt(i))->Filetize(true);
	}
	
	delete (feedBuffer);
	return 0;
}
