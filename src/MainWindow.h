#ifndef MAINWINDOW_H 
#define MAINWINDOW_H

#include <Window.h>

class BButton;
class BGroupView;
class BStatusBar;
class BTabView;
class EntriesView;
class FeedsView;
class UpdatesView;


class MainWindow : public BWindow {
public:
	MainWindow();
	~MainWindow();

	void MessageReceived(BMessage*);
	bool QuitRequested();

private:
	void _InitInterface();

	void _UpdateProgress(int32 total, int32 current);

	void _SetUpdateButton(bool cancel);

	BGroupView*		fBaseView;
	BTabView*		fTabView;
	EntriesView*	fEntriesView;
	FeedsView*		fFeedsView;
	UpdatesView*	fUpdatesView;

	BStatusBar*		fStatusBar;
	BButton*		fUpdateButton;
};


#endif // MAINWINDOW_H

