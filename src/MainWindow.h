#ifndef PREFWINDOW_H
#define PREFWINDOW_H


#include <Window.h>

#include "FeedView.h"

class BButton;
class BGroupView;
class BTabView;

class MainWindow : public BWindow {
public:
	MainWindow();

	void MessageReceived(BMessage*);

private:
	void _InitInterface();

	BGroupView* fBaseView;
	BTabView*	fTabView;
	FeedView*	fFeedView;

	BButton*	fServiceToggleButton;
};


#endif
