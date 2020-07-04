#ifndef UTIL_H 
#define UTIL_H

#include <Url.h>
#include "ProtocolListener.h"

int     stringDateToEpoch ( const char* dateStr );
BString stringDateToBString ( const char* dateStr );
int32   webFetch ( BUrl,  BDataIO* );
int32   webFetch ( char*, BDataIO* );


#endif
