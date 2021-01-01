#ifndef MAINWINDOW_H 
#define MAINWINDOW_H


#include <Window.h>

class BButton;
class BGroupView;
class BTabView;
class EntriesView;
class FeedsView;
class UpdatesView;


class MainWindow : public BWindow {
public:
	MainWindow();

	void MessageReceived(BMessage*);

private:
	void _InitInterface();

	BGroupView*	 fBaseView;
	BTabView*		fTabView;
	EntriesView*	fEntriesView;
	FeedsView*		fFeedsView;
	UpdatesView*	fUpdatesView;

	BButton*	fUpdateNowButton;
};


#endif // MAINWINDOW_H

