#ifndef PROTOCOL_LISTENER_H
#define PROTOCOL_LISTENER_H

#include <UrlProtocolListener.h>
#include <Url.h>
#include <boost/uuid/detail/sha1.hpp>

class ProtocolListener : public BUrlProtocolListener
{
public:
	ProtocolListener ( bool );

	virtual ~ProtocolListener();
	virtual void DataReceived(BUrlRequest*, const char*, off_t, ssize_t);

	void SetDownloadIO ( BDataIO* );
	BDataIO* GetDownloadIO ( );

	void SetSha1 ( boost::uuids::detail::sha1* );
	boost::uuids::detail::sha1* GetSha1 ( );
	BString GetHash ( );

private:
	BDataIO*	fDownloadIO;
	boost::uuids::detail::sha1* fSha1;
	bool		fTraceLogging;
};

#endif
