#include <iostream>
#include <sstream>
#include <locale>
#include <iomanip>
#include <HttpRequest.h>
#include "ProtocolListener.h"
#include "Util.h"

// ----------------------------------------------------------------------------

int
stringDateToEpoch ( const char* dateCStr )
{
	std::istringstream ss( dateCStr );
	std::tm t = {};

	if ( ss >> std::get_time( &t, "%a, %d %b %Y %H:%M:%S" ) )
		return std::mktime( &t );
		return -1;
}

BString
stringDateToBString ( const char* dateCStr )
{
	std::istringstream ss( dateCStr );
	std::ostringstream dateStream;
	std::tm t = {};

	if ( ss >> std::get_time( &t, "%a, %d %b %Y %H:%M:%S" ) ) {
		dateStream <<  std::put_time( &t, "%c" );
		std::string dateString = dateStream.str();
		return BString( dateStream.str().c_str() );
	}
	return NULL;
}

// ----------------------------------------------------------------------------

int32
webFetch ( char* strUrl, BDataIO* reply )
{
	return webFetch( BUrl(strUrl), reply );
}


int32
webFetch ( BUrl url, BDataIO* reply )
{
	ProtocolListener listener(true);
	BUrlContext context;

	BHttpRequest request( url, true, "HTTP", &listener, &context );

	listener.SetDownloadIO( reply );

	thread_id thread = request.Run();
	wait_for_thread( thread, NULL );

	const BHttpResult& result = dynamic_cast<const BHttpResult&>( request.Result() );
	return result.StatusCode();
}
