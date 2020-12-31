/*
 * Copyright 2020, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef INVOCATION_H
#define INVOCATION_H


int  usage       ( );
int  invocation  ( int, char** );
void freeargInvocation  ( int, char**, int );
bool processItem ( void* );
bool processFeed ( void* );


#endif

