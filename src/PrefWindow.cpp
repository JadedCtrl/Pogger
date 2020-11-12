#include "PrefWindow.h"
#include <Button.h>
#include <View.h>
#include <String.h>

enum
{

       M_BUTTON_CLICKED = 'btcl'
};


PrefWindow::PrefWindow ( void )
       : BWindow( BRect(100,100,300,200),"Pogger",B_TITLED_WINDOW,
                       B_ASYNCHRONOUS_CONTROLS | B_QUIT_ON_WINDOW_CLOSE )
{
	BButton *button = new BButton( BRect(10,10,11,11),"button", "Click Me!",
                                       new BMessage(M_BUTTON_CLICKED) );
	AddChild(button);
	button->ResizeToPreferred();

BView *view = new BView(BRect(100,100,300,200), "colorview", B_FOLLOW_ALL, B_WILL_DRAW);
//view->AddChild(button);
//AddChild(view);
view->SetViewColor(255,255,255);
view->SetViewUIColor(B_PANEL_BACKGROUND_COLOR);
view->Invalidate();
}

void
PrefWindow::MessageReceived ( BMessage *msg )
{

       // The way that BMessages are identified is by the public property
       // 'what'.
       switch (msg->what)
       {

              // If the message was the one sent to the window by the
              // button
              case M_BUTTON_CLICKED:
              {

                     BString labelString("Clicks: ");

                     // This converts fCount to a string and appends it to
                     // the end of labelString. More on this next lesson.
                     labelString << "DAD";

                     // Set the window's title to the new string we've made
                     SetTitle(labelString.String());
                     break;
              }
              default:
              {
                     // If the message doesn't match one of the ones we
                     // explicitly define, it must be some sort of system
                     // message, so we will call the BWindow version of
                     // MessageReceived() so that it can handle them. THIS
                     // IS REQUIRED if you want your window to act
                     // the way that you expect it to.
                     BWindow::MessageReceived(msg);
                     break;
              }
       }
}
