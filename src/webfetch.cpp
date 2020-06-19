#include <HttpRequest.h>
#include "ProtocolListener.h"
#include "webfetch.h"

// ----------------------------------------------------------------------------

int32
webfetch ( char* strUrl, BDataIO* reply )
{
	return webfetch( BUrl(strUrl), reply );
}


int32
webfetch ( BUrl url, BDataIO* reply )
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
