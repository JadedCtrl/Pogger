/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef APP_H
#define APP_H


#include <SupportDefs.h>
#include <Application.h>


class Config;


class App : public BApplication
{
public:
       App(void);
       Config* cfg;
};


int  main	( int, char** );
void cliStart	( int, char** );


extern const char* configPath;
extern BString usageMsg;


#endif

