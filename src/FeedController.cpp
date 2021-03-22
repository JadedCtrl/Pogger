/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "FeedController.h"

#include <iostream>

#include <Catalog.h>
#include <Directory.h>
#include <Message.h>
#include <MessageRunner.h>
#include <Notification.h>
#include <StringList.h>

#include "App.h"
#include "AtomFeed.h"
#include "Entry.h"
#include "RssFeed.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "FeedController"


FeedController::FeedController()
	:
	fEnqueuedTotal(0),
	fMainThread(find_thread(NULL)),
	fDownloadThread(0),
	fParseThread(0),
	fDownloadQueue(new BStringList()),
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
			ssize_t size = sizeof(BString);

			BStringList paths;
			msg->FindStrings("feedPaths", &paths);
			fDownloadQueue->Add(paths);

			fMessageRunner->SetCount(-1);
			_SendProgress();
			break;
		}
		case kUpdateSubscribed:
		{
			BStringList subFeeds = SubscribedFeeds();
			fDownloadQueue->Add(subFeeds);

			_SendProgress();
			break;
		}
		case kClearQueue:
		{
			fDownloadQueue->MakeEmpty();
			break;
		}
		case kControllerCheck:
		{
			_ProcessQueueItem();
			_ReceiveStatus();
			break;
		}
	}
}


BStringList
FeedController::SubscribedFeeds()
{
	BPath subPath;
	find_directory(B_USER_SETTINGS_DIRECTORY, &subPath);
	subPath.Append("Pogger");
	subPath.Append("Subscriptions");
	BDirectory subDir(subPath.Path());
	if (subDir.InitCheck() == B_ENTRY_NOT_FOUND) {
		subDir.CreateDirectory(subPath.Path(), &subDir);

		subPath.Append("Haiku Project");
		Feed defaultSub(BUrl("https://www.haiku-os.org/blog/index.xml"),
			BEntry(subPath.Path()));
		defaultSub.SetTitle("Haiku Project");
		defaultSub.Filetize();
	}

	BEntry feedEntry;
	BPath feedPath;
	BStringList feeds;

	while (subDir.GetNextEntry(&feedEntry) == B_OK
		&& feedEntry.GetPath(&feedPath) == B_OK)
		feeds.Add(feedPath.Path());
	return feeds;
}


void
FeedController::_SendProgress()
{
	int32 dqCount = fDownloadQueue->CountStrings();

	if (fEnqueuedTotal < dqCount)
		fEnqueuedTotal = dqCount;

	BMessage progress(kProgress);
	progress.AddInt32("total", fEnqueuedTotal);
	progress.AddInt32("current", fEnqueuedTotal - dqCount);
	be_app->MessageReceived(&progress);

	if (dqCount == 0)
		fEnqueuedTotal = 0;
}


void
FeedController::_ProcessQueueItem()
{
	if (has_data(fDownloadThread) && !fDownloadQueue->IsEmpty()) {
		BString feed = fDownloadQueue->Remove(0);
		Feed* buffer;

		if (BUrl(feed.String()).IsValid() == true)
			buffer = new Feed(BUrl(feed.String()));
		else
			buffer = new Feed(feed.String());

		send_data(fDownloadThread, 0, (void*)buffer, sizeof(Feed));

		BMessage downloadInit = BMessage(kDownloadStart);
		downloadInit.AddString("feed_name", buffer->Title());
		downloadInit.AddString("feed_url", buffer->XmlUrl().UrlString());
		((App*)be_app)->MessageReceived(&downloadInit);
	}
}


void
FeedController::_ReceiveStatus()
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
				complete.AddString("feed_name", feedBuffer->Title());
				complete.AddString("feed_url",
					feedBuffer->XmlUrl().UrlString());
				((App*)be_app)->MessageReceived(&complete);

				send_data(fParseThread, 0, (void*)feedBuffer, sizeof(Feed));
				break;
			}
			case kDownloadFail:
			{
				BMessage failure = BMessage(kDownloadFail);
				failure.AddString("feed_name", feedBuffer->Title());
				failure.AddString("feed_url",
					feedBuffer->XmlUrl().UrlString());
				((App*)be_app)->MessageReceived(&failure);
				_SendProgress();
				break;
			}
			case kParseFail:
			{
				BMessage failure = BMessage(kParseFail);
				failure.AddString("feed_name", feedBuffer->Title());
				failure.AddString("feed_url", feedBuffer->XmlUrl().UrlString());
				((App*)be_app)->MessageReceived(&failure);
				_SendProgress();
				break;
			}
			// If parse was successful, the code is the amount of new entries
			default:
				BMessage complete = BMessage(kParseComplete);
				complete.AddString("feed_name", feedBuffer->Title());
				complete.AddString("feed_url", feedBuffer->XmlUrl().UrlString());
				complete.AddInt32("entry_count", code);
				((App*)be_app)->MessageReceived(&complete);
				_SendProgress();
				break;
		}
		free(feedBuffer);
	}
}


int32
FeedController::_DownloadLoop(void* data)
{
	thread_id main = *((thread_id*)data);
	thread_id sender;

	while (true) {
		Feed* feedBuffer = (Feed*)malloc(sizeof(Feed));
		receive_data(&sender, (void*)feedBuffer, sizeof(Feed));

		std::cout << B_TRANSLATE("Downloading feed from ")
			<< feedBuffer->XmlUrl().UrlString() << "…\n";

		if (feedBuffer->Fetch()) {
			send_data(main, kDownloadComplete, (void*)feedBuffer, sizeof(Feed));
		}
		else {
			send_data(main, kDownloadFail, (void*)feedBuffer, sizeof(Feed));
		}
		free(feedBuffer);
	}
	return 0;
}


int32
FeedController::_ParseLoop(void* data)
{
	thread_id main = *((thread_id*)data);
	thread_id sender;

	while (true) {
		Feed* feedBuffer = (Feed*)malloc(sizeof(Feed));
		receive_data(&sender, (void*)feedBuffer, sizeof(Feed));

		BObjectList<Entry> entries;
		int32 entriesCount = 0;
		BString feedTitle;
		BUrl feedUrl = feedBuffer->XmlUrl();

		if (feedBuffer->IsAtom() && feedBuffer->IsUpdated()) {
			AtomFeed feed(feedBuffer);
			feed.Parse();
			entries = feed.NewEntries();
			entriesCount = entries.CountItems();
			feedTitle = feed.Title();

			for (int i = 0; i < entriesCount; i++)
				entries.ItemAt(i)->Filetize();
			entries.MakeEmpty();
		}
		else if (feedBuffer->IsRss() && feedBuffer->IsUpdated()) {
			RssFeed feed(feedBuffer);
			feed.Parse();
			entries = feed.NewEntries();
			entriesCount = entries.CountItems();
			feedTitle = feed.Title();

			for (int i = 0; i < entriesCount; i++)
				entries.ItemAt(i)->Filetize();
			entries.MakeEmpty();
		}


		if (feedBuffer->IsAtom() || feedBuffer->IsRss()) {
			send_data(main, entriesCount, (void*)feedBuffer, sizeof(Feed));
		}
		else {
			send_data(main, entriesCount, (void*)feedBuffer, sizeof(Feed));
		}

		free(feedBuffer);
	}

	return 0;
}


