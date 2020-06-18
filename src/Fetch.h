#include <UrlProtocolListener.h>
#include <Url.h>

class ProtocolListener : public BUrlProtocolListener
{
public:
	ProtocolListener(bool traceLogging)
		:
		fDownloadIO(NULL),
		fTraceLogging(traceLogging)
	{
	}

	virtual ~ProtocolListener()
	{
	}

	virtual void DataReceived(BUrlRequest* caller, const char* data,
		off_t position, ssize_t size)
	{
		if (fDownloadIO != NULL) {
			fDownloadIO->Write(data, size);
		}
	}

	void
	SetDownloadIO ( BDataIO* downloadIO )
	{
		fDownloadIO = downloadIO;
	}

	BDataIO*
	GetDownloadIO ( )
	{
		return fDownloadIO;
	}

private:
	BDataIO*	fDownloadIO;
	bool		fTraceLogging;
};

// ----------------------------------------------------------------------------

int32 fetch ( BUrl,  BDataIO* );
int32 fetch ( char*, BDataIO* );
