#ifndef WEBFETCH_H
#define WEBFETCH_H
#include <Url.h>
#include "ProtocolListener.h"

int32 webfetch ( BUrl,  BDataIO* );
int32 webfetch ( char*, BDataIO* );

#endif
