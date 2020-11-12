#ifndef PREFWINDOW_H
#define PREFWINDOW_H

#include <Window.h>

class PrefWindow : public BWindow
{
public:
	PrefWindow ( void );

//	void	MessageRecieved ( BMessage* );
void MessageReceived(BMessage*);

private:

BView* fMainView;
void	_InitInterface ( void );

};

#endif
