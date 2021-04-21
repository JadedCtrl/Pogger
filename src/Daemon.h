/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef SHADOWPOG_H
#define SHADOWPOG_H

#include <Application.h>

class FeedController;
class Notifier;
class Preferences;


class App : public BApplication {
public:
	App();

	void MessageReceived(BMessage* msg);
	bool QuitRequested();

	void ArgvReceived(int32 argc, char** argv);
	void RefsReceived(BMessage* message);

	Preferences* fPreferences;
	BMessageRunner* fUpdateRunner;

private:
	void _OpenEntryFile(BMessage* refMessage);
	void _OpenEntryFileAsHtml(entry_ref ref);
	void _OpenEntryFileAsUrl(entry_ref ref);
	void _OpenSourceFile(BMessage* refMessage);

	void _ReloadPreferences();

	FeedController* fFeedController;
	Notifier* fNotifier;
};


#endif // SHADOWPOG_H
