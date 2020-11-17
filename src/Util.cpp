#include <iostream>
#include <sstream>
#include <locale>
#include <iomanip>
#include <DateTime.h>
#include <HttpRequest.h>
#include <boost/uuid/detail/sha1.hpp>
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

bool
withinDateRange ( BDateTime minDate, BDateTime nowDate, BDateTime maxDate )
{
	if ( minDate != NULL && nowDate != NULL  && maxDate != NULL )
		return ( minDate < nowDate && nowDate < maxDate );
	if ( minDate != NULL && nowDate != NULL )
		return ( minDate < nowDate );
	if ( maxDate != NULL && nowDate != NULL )
		return ( nowDate < maxDate );
	return true;
}

// ----------------------------------------------------------------------------

// return whether or not the given path is remote
bool
isRemotePath ( BString path ) {
	BUrl givenUrl = BUrl( path );
	BString protocol = givenUrl.Protocol().String();

	if ( protocol == NULL  ||  protocol == BString("file")  ||  givenUrl.UrlString() == NULL )
		return false;
	return true;
}

// ----------------------------------------------------------------------------

int32
webFetch ( BUrl url, BDataIO* reply )
{
	BString* ignored = new BString();
	return webFetch( url, reply, ignored );
}


int32
webFetch ( BUrl url, BDataIO* reply, BString* hash )
{
	ProtocolListener listener(true);
	BUrlContext context;
	boost::uuids::detail::sha1 sha1;

	BHttpRequest request( url, true, "HTTP", &listener, &context );

	listener.SetDownloadIO( reply );
	listener.SetSha1( &sha1 );

	thread_id thread = request.Run();
	wait_for_thread( thread, NULL );

	*(hash) = listener.GetHash();

	const BHttpResult& result = dynamic_cast<const BHttpResult&>( request.Result() );
	return result.StatusCode();
}
