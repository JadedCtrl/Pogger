#include <iostream>
#include <sstream>
#include <UrlProtocolListener.h>
#include <Url.h>
#include <boost/uuid/detail/sha1.hpp>
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
	if ( fDownloadIO != NULL )
		fDownloadIO->Write( data, size );
	if ( fSha1 != NULL )
		fSha1->process_bytes( data,  size );
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

void
ProtocolListener::SetSha1 ( boost::uuids::detail::sha1* sha1 )
{
	fSha1 = sha1;
}

boost::uuids::detail::sha1*
ProtocolListener::GetSha1 ( )
{
	return fSha1;
}

BString
ProtocolListener::GetHash ( )
{
	unsigned int hashInt[5];
	fSha1->get_digest( hashInt );

	std::ostringstream hashStr;
	for(std::size_t i=0; i<sizeof(hashInt)/sizeof(hashInt[0]); ++i) {
		hashStr << std::hex <<hashInt[i];
	}

	return BString( hashStr.str().c_str() );
}

