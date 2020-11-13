#ifndef MIME_H
#define MIME_H

#include <Message.h>
#include <String.h>

bool feedMimeType ( );

static void addAttribute
( BMessage&, const char*, const char*, int32 type = B_STRING_TYPE, int32 width = 200);


#endif
