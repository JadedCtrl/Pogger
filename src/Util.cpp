#include <iostream>
#include <sstream>
#include <locale>
#include <iomanip>
#include <DateTime.h>
#include <HttpRequest.h>
#include "ProtocolListener.h"
#include "Util.h"

// ----------------------------------------------------------------------------

BDateTime
feedDateToBDate ( const char* dateCStr )
{
	BDateTime date = dateRfc822ToBDate( dateCStr );
	if ( date == NULL )   date = dateRfc3339ToBDate( dateCStr );
	return date;
}

BDateTime
dateRfc3339ToBDate ( const char* dateCStr )
{
	return stringDateToBDate( dateCStr, "%Y-%m-%dT%H:%M:%S" );
}

BDateTime
dateRfc822ToBDate ( const char* dateCStr )
{
	return stringDateToBDate( dateCStr, "%a, %d %b %Y %H:%M:%S" );
}

BDateTime
stringDateToBDate ( const char* dateCStr, const char* templateCStr )
{
	std::istringstream dateStream( dateCStr );
	std::tm time = {};

	if ( dateStream >> std::get_time( &time, templateCStr ) ) {
		BTime newTime = BTime( time.tm_hour, time.tm_min, time.tm_sec, 0 );
		BDate newDate = BDate( time.tm_year + 1900, time.tm_mon + 1, time.tm_mday );
		return BDateTime( newDate, newTime );
	}
	return NULL;
}

// ----------------------------------------------------------------------------

BString
dateTo3339String ( BDateTime dt )
{
	char buffer[18];
	sprintf( buffer, "%i-%02i-%02iT%02i:%02i:%02i",
		 dt.Date().Year(), dt.Date().Month(), dt.Date().Day(),
		 dt.Time().Hour(), dt.Time().Minute(), dt.Time().Second() );
	
	return BString( buffer );
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
