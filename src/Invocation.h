#ifndef INVOCATION_H
#define INVOCATION_H

#include <StorageKit.h>

int  usage       ( );
int  invocation  ( int, char**, Config** );
void freeargInvocation  ( int, char**, int, Config** );
bool processItem ( void* );
bool processFeed ( void* );

// ----------------------------------------------------------------------------

#endif
