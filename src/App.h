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
	App(void);
	void MessageReceived(BMessage* msg);
	void ArgvReceived(int32 argc, char** argv);
	

	MainWindow* fMainWindow;
	Preferences* fPreferences;
	Notifier* fNotifier;
	BMessageRunner* fUpdateRunner;

private:
	FeedController* fFeedController;
};

int  main	( int, char** );

extern const char* configPath;


#endif // APP_H

