/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef APP_H
#define APP_H

#include <Application.h>

#include "Preferences.h"

class BMessage;
class BMessageRunner;
class FeedController;
class MainWindow;


class App : public BApplication
{
public:
	App();

	void Pulse();
	void MessageReceived(BMessage* msg);
	bool QuitRequested();

	void ArgvReceived(int32 argc, char** argv);
	void RefsReceived(BMessage* message);

	MainWindow* fMainWindow;
	Preferences* fPreferences;

private:
	void _OpenEntryFile(BMessage* refMessage);
	void _OpenSourceFile(BMessage* refMessage);
};


int main(int argc, char** argv);


#endif // APP_H

