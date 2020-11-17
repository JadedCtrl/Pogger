#ifndef INVOCATION_H
#define INVOCATION_H

#include <StorageKit.h>

int  usage       ( );
int  invocation  ( int, char** );
void freeargInvocation  ( int, char**, int );
bool processItem ( void* );
bool processFeed ( void* );

// ----------------------------------------------------------------------------

#endif
