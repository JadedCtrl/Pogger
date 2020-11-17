#ifndef APPP_H
#define APPP_H

#include <SupportDefs.h>
#include <Application.h>
#include "Config.h"


class App : public BApplication
{
public:
       App(void);
       Config* cfg;
};

// ----------------------------------------------------------------------------

int  main	( int, char** );
void cliStart	( int, char** );

// ----------------------------------------------------------------------------

extern const char* configPath;

extern BString usageMsg;
#endif
