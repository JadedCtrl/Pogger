/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef APP_H
#define APP_H

#include <SupportDefs.h>
#include <Application.h>

#include "Preferences.h"

class BMessage;
class BMessageRunner;
class FeedController;
class MainWindow;
class Notifier;


class App : public BApplication
{
public:
	App();

	void MessageReceived(BMessage* msg);
	bool QuitRequested();

	void ArgvReceived(int32 argc, char** argv);
	void RefsReceived(BMessage* message);

	MainWindow* fMainWindow;
	Preferences* fPreferences;
	Notifier* fNotifier;
	BMessageRunner* fUpdateRunner;

private:
	void _OpenEntryFile(BMessage* refMessage);
	void _OpenEntryFileAsHtml(entry_ref ref);
	void _OpenEntryFileAsUrl(entry_ref ref);
	void _OpenSourceFile(BMessage* refMessage);

	FeedController* fFeedController;
};


int main(int argc, char** argv);


#endif // APP_H

