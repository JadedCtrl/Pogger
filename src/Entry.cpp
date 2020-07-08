#include <iostream>
#include <fstream>
#include <tinyxml2.h>
#include <StorageKit.h>
#include "Config.h"
#include "Entry.h"
#include "Util.h"

Entry::Entry ( BString outputPath )
{
	title = BString("");
	description = BString("");
	postUrl  = BString("");
	content  = BString("");
	outputDir = outputPath;
}

bool
Entry::Filetize ( Config* cfg, bool onlyIfNew = false )
{
	BDirectory* dir = new BDirectory( outputDir );
	BFile* file = new BFile( title.String(), B_READ_WRITE );

	dir->CreateFile( title.String(), file );

	BString betype = cfg->mimetype;
	if ( date != NULL ) {
		int32 unixDate = (int32)date.Time_t();
		file->WriteAttr( "unixDate", B_INT32_TYPE, 0,
				 &unixDate, sizeof(int32) );
		file->WriteAttr( "date", B_STRING_TYPE, 0,
				 dateTo3339String(date).String(),
				 dateTo3339String(date).CountChars() );
	}

	file->WriteAttr( "META:title", B_STRING_TYPE, 0,
			 title.String(), title.CountChars() );
	file->WriteAttr( "description", B_STRING_TYPE, 0,
			 description.String(), description.CountChars() );
	file->WriteAttr( "META:url", B_STRING_TYPE, 0,
			 postUrl.String(), postUrl.CountChars() );
	file->WriteAttr( "BEOS:TYPE", B_STRING_TYPE, 0,
			 betype.String(), betype.CountChars() );

	file->Write(content.String(), content.Length());
	return false;
}

bool Entry::SetTitle ( const char* titleStr ) {
	if ( titleStr != NULL )	title = BString( titleStr );
	else return false;
	return true;
}
bool Entry::SetTitle ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	return SetTitle( elem->GetText() );
	return false;
}

bool Entry::SetDesc ( const char* descStr ) {
	if ( descStr != NULL )	description = BString( descStr );
	else return false;
	return true;
}
bool Entry::SetDesc ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	return SetDesc( elem->GetText() );
	return false;
}

bool Entry::SetContent ( const char* contentStr ) {
	if ( contentStr != NULL )	content = BString( contentStr );
	else return false;
	return true;
}
bool Entry::SetContent ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	return SetContent( elem->GetText() );
	return false;
}

bool Entry::SetPostUrl ( const char* urlStr ) {
	if ( urlStr != NULL )	postUrl = BString( urlStr );
	else return false;
	return true;
}
bool Entry::SetPostUrl ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	return SetPostUrl( elem->GetText() );
	return false;
}

bool Entry::SetDate ( const char* dateStr ) {
	if ( dateStr == NULL )
		return false;
	BDateTime newDate = feedDateToBDate( dateStr );
	if ( newDate == NULL )
		return false;
	date = newDate;
	return true;
}
bool Entry::SetDate ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	return SetDate( elem->GetText() );
	return false;
}
