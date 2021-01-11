/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef APP_H
#define APP_H


#include <SupportDefs.h>
#include <Application.h>


class BMessage;
class Config;
class FeedController;
class MainWindow;


class App : public BApplication
{
public:
	App(void);
	void MessageReceived(BMessage* msg);
	void ArgvReceived(int32 argc, char** argv);
	

	Config* cfg;
	MainWindow* fMainWindow;

private:
	FeedController* fFeedController;
};

int  main	( int, char** );

extern const char* configPath;


#endif // APP_H

