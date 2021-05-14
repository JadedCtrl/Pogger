/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "FeedController.h"

#include <iostream>

#include <Catalog.h>
#include <Directory.h>
#include <FindDirectory.h>
#include <Message.h>
#include <MessageRunner.h>
#include <Notification.h>
#include <StringList.h>

#include "Daemon.h"
#include "Entry.h"
#include "Preferences.h"
#include "SourceManager.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "FeedController"


FeedController::FeedController()
	:
	fEnqueuedTotal(0),
	fMainThread(find_thread(NULL)),
	fDownloadThread(0),
	fParseThread(0),
	fDownloadQueue(new BObjectList<Feed>()),
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
			BString feedID;
			BString feedSource;
			ssize_t size = sizeof(Feed);

			while (msg->HasString("feed_identifiers", i)) {
				msg->FindString("feed_identifiers", i, &feedID);
				msg->FindString("feed_sources", i, &feedSource);

				Feed* feed = SourceManager::GetFeed(feedID.String(),
					feedSource.String());
				fDownloadQueue->AddItem(feed);
				_SendProgress();
				i++;
			}
			fMessageRunner->SetCount(-1);
			break;
		}
		case kUpdateSubscribed:
		{
			BObjectList<Feed> list = SourceManager::Feeds();
			fDownloadQueue->AddList(&list);
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


void
FeedController::_SendProgress()
{
	int32 dqCount = fDownloadQueue->CountItems();

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
		Feed* feed = fDownloadQueue->ItemAt(0);
		fDownloadQueue->RemoveItemAt(0);
		send_data(fDownloadThread, 0, (void*)feed, sizeof(Feed));

		BMessage downloadInit = BMessage(kDownloadStart);
		downloadInit.AddString("feed_name", feed->Title());
		downloadInit.AddString("feed_url", feed->Url().UrlString());
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
				complete.AddString("feed_url", feedBuffer->Url().UrlString());
				((App*)be_app)->MessageReceived(&complete);

				send_data(fParseThread, 0, (void*)feedBuffer, sizeof(Feed));
				break;
			}
			case kDownloadFail:
			{
				BMessage failure = BMessage(kDownloadFail);
				failure.AddString("feed_name", feedBuffer->Title());
				failure.AddString("feed_url",
					feedBuffer->Url().UrlString());
				((App*)be_app)->MessageReceived(&failure);
				_SendProgress();
				break;
			}
			case kParseFail:
			{
				BMessage failure = BMessage(kParseFail);
				failure.AddString("feed_name", feedBuffer->Title());
				failure.AddString("feed_url", feedBuffer->Url().UrlString());
				((App*)be_app)->MessageReceived(&failure);
				_SendProgress();
				break;
			}
			// If parse was successful, the code is the amount of new entries
			default:
				BMessage complete = BMessage(kParseComplete);
				complete.AddString("feed_name", feedBuffer->Title());
				complete.AddString("feed_url", feedBuffer->Url().UrlString());
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
			<< feedBuffer->Url().UrlString() << "…\n";

		if (SourceManager::Fetch(feedBuffer)) {
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
		BUrl feedUrl = feedBuffer->Url();

		SourceManager::Parse(feedBuffer);
		entries = feedBuffer->NewEntries();
		entriesCount = entries.CountItems();
		feedTitle = feedBuffer->Title();

		for (int i = 0; i < entriesCount; i++)
			entries.ItemAt(i)->Filetize(((App*)be_app)->fPreferences->EntryDir());
		entries.MakeEmpty();

		SourceManager::EditFeed(feedBuffer);
		send_data(main, entriesCount, (void*)feedBuffer, sizeof(Feed));
		free(feedBuffer);
	}

	return 0;
}


