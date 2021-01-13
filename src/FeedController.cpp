/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "FeedController.h"

#include <Message.h>
#include <Notification.h>

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
		case kUpdateSubscribed:
		{
			BDirectory subDir("/boot/home/config/settings/Pogger/Subscriptions");
			BEntry feedEntry;
			Feed* feed;

			while (subDir.GetNextEntry(&feedEntry) == B_OK) {
				feed = new Feed(feedEntry);
				BMessage* getFeed = new BMessage(kEnqueueFeed);
				getFeed->AddData("feeds", B_RAW_TYPE, feed, sizeof(Feed));
				((App*)be_app)->MessageReceived(getFeed);
			}
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
		printf( "Downloading feed from %s...\n",
			feedBuffer->GetXmlUrl().UrlString().String());

		if (feedBuffer->Fetch()) {
			BMessage* downloaded = new BMessage(kDownloadComplete);
			downloaded->AddData("feeds", B_RAW_TYPE, feedBuffer, sizeof(Feed));
			((App*)be_app)->MessageReceived(downloaded);
		}
		else {
			BMessage* failure = new BMessage(kDownloadFail);
			failure->AddString("feed_url", feedBuffer->GetXmlUrl().UrlString());
			((App*)be_app)->MessageReceived(failure);
		}
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
		BString feedTitle;
		BUrl feedUrl = feedBuffer->GetXmlUrl();
		BDirectory outDir = BDirectory(((App*)be_app)->cfg->outDir);

		if (feedBuffer->IsAtom()) {
			AtomFeed* feed = (AtomFeed*)malloc(sizeof(AtomFeed));
			feed = new AtomFeed(feedBuffer);
			feed->Parse();
			entries = feed->GetNewEntries();
			feedTitle = feed->GetTitle();
			delete(feed);
		}
		else if (feedBuffer->IsRss()) {
			RssFeed* feed = (RssFeed*)malloc(sizeof(RssFeed));
			feed = new RssFeed(feedBuffer);
			feed->Parse();
			entries = feed->GetNewEntries();
			feedTitle = feed->GetTitle();
			delete(feed);
		}


		if (feedBuffer->IsAtom() || feedBuffer->IsRss()) {
			for (int i = 0; i < entries.CountItems(); i++)
				((Entry*)entries.ItemAt(i))->Filetize(outDir);

			BMessage* complete = new BMessage(kParseComplete);
			complete->AddString("feed_name", feedTitle);
			complete->AddInt32("entry_count", entries.CountItems());
			((App*)be_app)->MessageReceived(complete);
		}
		else {
			BMessage* failure = new BMessage(kParseFail);
			failure->AddString("feed_url", feedUrl.UrlString());
			((App*)be_app)->MessageReceived(failure);
		}
	}
	
	delete (feedBuffer);
	return 0;
}


