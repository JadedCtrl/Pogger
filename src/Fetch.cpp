#include <HttpRequest.h>
#include "Fetch.h"

// ----------------------------------------------------------------------------

int32
fetch ( char* strUrl, BDataIO* reply )
{
	BUrl url( strUrl );
	return fetch( url, reply );
}


int32
fetch ( BUrl url, BDataIO* reply )
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
