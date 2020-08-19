// borrowed significantly from mailserver. thanks! <3
#include <DateTime.h>
#include <Message.h>
#include <MimeType.h>
#include "Mimetypes.h"

bool
feedMimeType ( )
{
	BMessage info;
	BMimeType mime( "text/x-feed-entry" );

	if ( mime.InitCheck() != B_OK ) {
		return false;
	}

	if ( !mime.IsInstalled() ) {
		mime.Delete();
		mime.Install();

		addAttribute( info, "FEED:name", "Name" );
		addAttribute( info, "FEED:description", "Description" );
		addAttribute( info, "META:url", "URL" );
		addAttribute( info, "FEED:source", "Source" );
		addAttribute( info, "FEED:when", "When", B_TIME_TYPE, true, false, 150 );

		mime.SetAttrInfo(&info);

		mime.SetShortDescription("Feed Entry");
		mime.SetLongDescription("RSS/Atom Feed Entry");
	}
	return true;
}

// -------------------------------------

static void
addAttribute
( BMessage& msg, const char* name, const char* publicName, int32 type, bool viewable, bool editable,
  int32 width)
{
	msg.AddString( "attr:name", name );
	msg.AddString( "attr:public_name", publicName );
	msg.AddInt32( "attr:type", type );
	msg.AddBool( "attr:viewable", viewable );
	msg.AddBool( "attr:editable", editable );
	msg.AddInt32( "attr:width", width );
}
