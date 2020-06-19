#include <UrlProtocolListener.h>
#include <Url.h>
#include "ProtocolListener.h"

ProtocolListener::ProtocolListener ( bool traceLogging )
		:
		fDownloadIO(NULL),
		fTraceLogging(traceLogging)
{ }

ProtocolListener::~ProtocolListener ( )
{ }

void
ProtocolListener::DataReceived ( BUrlRequest* caller, const char* data, off_t position, ssize_t size )
{
	if (fDownloadIO != NULL)
		fDownloadIO->Write(data, size);
}

void
ProtocolListener::SetDownloadIO ( BDataIO* downloadIO )
{
	fDownloadIO = downloadIO;
}

BDataIO*
ProtocolListener::GetDownloadIO ( )
{
	return fDownloadIO;
}
