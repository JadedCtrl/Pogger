// borrowed significantly from mailserver. thanks! <3
#include <Application.h>
#include <DateTime.h>
#include <Message.h>
#include <MimeType.h>
#include "Mimetypes.h"

bool
feedMimeType ( )
{
	BMessage info;
	BMimeType mime( "text/x-feed-entry" );
	mime.GetAttrInfo( &info );

	mime.SetShortDescription( "Feed Entry" );
	mime.SetLongDescription( "Atom/RSS Feed Entry" );

	addAttribute( info, "FEED:name", "Name" );
	addAttribute( info, "FEED:description", "Description" );
	addAttribute( info, "META:url", "URL" );
	addAttribute( info, "FEED:source", "Source" );
	addAttribute( info, "FEED:when", "When", B_TIME_TYPE, 150 );

	mime.SetAttrInfo( &info );

	return true;
}

// -------------------------------------

static void
addAttribute
( BMessage& msg, const char* name, const char* publicName, int32 type, int32 width )
{
	msg.AddString( "attr:name", name );
	msg.AddString( "attr:public_name", publicName );
	msg.AddInt32( "attr:type", type );
	msg.AddInt32( "attr:width", width );
	msg.AddInt32( "attr:alignment", B_ALIGN_LEFT );
	msg.AddBool( "attr:extra", false );
	msg.AddBool( "attr:viewable", true );
	msg.AddBool( "attr:editable", false );
}
