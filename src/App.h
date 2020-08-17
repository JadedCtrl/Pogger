#ifndef APPP_H
#define APPP_H

#include <SupportDefs.h>
#include <Application.h>


class App : public BApplication
{
public:
       App(void);
};

// ----------------------------------------------------------------------------

int  main	( int, char** );
void cliStart	( int, char** );

// ----------------------------------------------------------------------------

extern Config* main_cfg;

extern const char* configPath;

extern BString usageMsg;
#endif
