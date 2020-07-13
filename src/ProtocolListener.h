#ifndef PROTOCOL_LISTENER_H
#define PROTOCOL_LISTENER_H

#include <UrlProtocolListener.h>
#include <Url.h>

class ProtocolListener : public BUrlProtocolListener
{
public:
	ProtocolListener ( bool );

	virtual ~ProtocolListener();
	virtual void DataReceived(BUrlRequest*, const char*, off_t, ssize_t);
	void SetDownloadIO ( BDataIO* );
	BDataIO* GetDownloadIO ( );

private:
	BDataIO*	fDownloadIO;
	bool		fTraceLogging;
};

#endif
