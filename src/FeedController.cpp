/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "FeedController.h"

#include <Directory.h>
#include <Message.h>
#include <MessageRunner.h>
#include <Notification.h>

#include <iostream>

#include "App.h"
#include "AtomFeed.h"
#include "Entry.h"
#include "RssFeed.h"


FeedController::FeedController()
	:
	fMainThread(find_thread(NULL)),
	fDownloadThread(0),
	fParseThread(0),
	fDownloadQueue(new BList()),
	fMessageRunner(new BMessageRunner(be_app, BMessage(kControllerCheck), 50000, -1))
{
	fDownloadThread = spawn_thread(_DownloadLoop, "here, eat this",
		B_NORMAL_PRIORITY, &fMainThread);
	fParseThread = spawn_thread(_ParseLoop, "oki tnx nomnomnom",
		B_NORMAL_PRIORITY, &fMainThread);
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
				_EnqueueFeed((Feed*)data);
				i++;
			}
			break;
		}
		case kUpdateSubscribed:
		{
			BList subFeeds = SubscribedFeeds();
			for (int i = 0; i < subFeeds.CountItems(); i++) {
				_EnqueueFeed((Feed*)subFeeds.ItemAt(i));
			}
			break;
		}
		case kClearQueue:
		{
			break;
		}
		case kControllerCheck:
		{
			_ProcessQueueItem();
			_CheckStatus();
			break;
		}
		default:
		{
//			BWindow::MessageReceived(msg);
			break;
		}
	}
}


BList
FeedController::SubscribedFeeds()
{
	BDirectory subDir("/boot/home/config/settings/Pogger/Subscriptions");
	BEntry feedEntry;
	BList feeds;

	while (subDir.GetNextEntry(&feedEntry) == B_OK)
		feeds.AddItem(new Feed(feedEntry));
	return feeds;
}


void
FeedController::_EnqueueFeed(Feed* feed)
{
	fMessageRunner->SetCount(-1);
	fDownloadQueue->AddItem(feed);
}


void
FeedController::_ProcessQueueItem()
{
	if (has_data(fDownloadThread) && !fDownloadQueue->IsEmpty()) {
		Feed* buffer = (Feed*)(fDownloadQueue->RemoveItem(0));
		send_data(fDownloadThread, 0, (void*)buffer, sizeof(Feed));

		BMessage downloadInit = BMessage(kDownloadStart);
		downloadInit.AddString("feed_url", buffer->GetXmlUrl().UrlString());
		downloadInit.AddString("feed_name", buffer->GetTitle());
		((App*)be_app)->MessageReceived(&downloadInit);
	}
}


void
FeedController::_CheckStatus()
{
	thread_id sender;

	while (has_data(find_thread(NULL))) {
		Feed* feedBuffer = (Feed*)malloc(sizeof(Feed));
		int32 code = receive_data(&sender, (void*)feedBuffer, sizeof(Feed));

		switch (code)
		{
			case kDownloadComplete: 
			{
				BMessage complete = BMessage(kDownloadComplete);
				complete.AddString("feed_url",
					feedBuffer->GetXmlUrl().UrlString());
				((App*)be_app)->MessageReceived(&complete);

				send_data(fParseThread, 0, (void*)feedBuffer, sizeof(Feed));
				break;
			}
			case kDownloadFail:
			{
				BMessage failure = BMessage(kDownloadFail);
				failure.AddString("feed_url",
					feedBuffer->GetXmlUrl().UrlString());
				((App*)be_app)->MessageReceived(&failure);
				break;
			}
			case kParseComplete:
			{
				BMessage complete = BMessage(kParseComplete);
				complete.AddString("feed_url", feedBuffer->GetXmlUrl().UrlString());
				complete.AddInt32("entry_count", feedBuffer->GetNewEntries().CountItems());
				((App*)be_app)->MessageReceived(&complete);
				break;
			}
			case kParseFail:
			{
				BMessage failure = BMessage(kParseFail);
				failure.AddString("feed_url", feedBuffer->GetXmlUrl().UrlString());
				((App*)be_app)->MessageReceived(&failure);
				break;
			}
		}
	}
}


int32
FeedController::_DownloadLoop(void* data)
{
	thread_id main = *((thread_id*)data);
	thread_id sender;
	Feed* feedBuffer = (Feed*)malloc(sizeof(Feed));


	while (true) {
		int32 code = receive_data(&sender, (void*)feedBuffer, sizeof(Feed));

		std::cout << "Downloading feed from "
			<< feedBuffer->GetXmlUrl().UrlString() << "…\n";

		if (feedBuffer->Fetch()) {
			send_data(main, kDownloadComplete, (void*)feedBuffer, sizeof(Feed));
		}
		else {
			send_data(main, kDownloadFail, (void*)feedBuffer, sizeof(Feed));
		}
	}
	delete(feedBuffer);
	return 0;
}


int32
FeedController::_ParseLoop(void* data)
{
	thread_id main = *((thread_id*)data);
	thread_id sender;
	Feed* feedBuffer = (Feed*)malloc(sizeof(Feed));

	while (true) {
		int32 code = receive_data(&sender, (void*)feedBuffer, sizeof(Feed));

		BList entries;
		BString feedTitle;
		BUrl feedUrl = feedBuffer->GetXmlUrl();
		BDirectory outDir = BDirectory(((App*)be_app)->fPreferences->EntryDir());

		if (feedBuffer->IsAtom()) {
			AtomFeed* feed = (AtomFeed*)malloc(sizeof(AtomFeed));
			feed = new AtomFeed(feedBuffer);
			feed->Parse();
			entries = feed->GetNewEntries();
			feedTitle = feed->GetTitle();

			for (int i = 0; i < entries.CountItems(); i++)
				((Entry*)entries.ItemAt(i))->Filetize(outDir);
			delete feed;
		}
		else if (feedBuffer->IsRss()) {
			RssFeed* feed = (RssFeed*)malloc(sizeof(RssFeed));
			feed = new RssFeed(feedBuffer);
			feed->Parse();
			entries = feed->GetNewEntries();
			feedTitle = feed->GetTitle();

			for (int i = 0; i < entries.CountItems(); i++)
				((Entry*)entries.ItemAt(i))->Filetize(outDir);
			delete feed;
		}


		if (feedBuffer->IsAtom() || feedBuffer->IsRss()) {
			send_data(main, kParseComplete, (void*)feedBuffer, sizeof(Feed));
		}
		else {
			send_data(main, kParseFail, (void*)feedBuffer, sizeof(Feed));
		}
	}

	delete (feedBuffer);
	return 0;
}


