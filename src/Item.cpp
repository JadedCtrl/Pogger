#include <iostream>
#include <fstream>
#include <tinyxml2.h>
#include <StorageKit.h>
#include "Config.h"
#include "Item.h"
#include "Util.h"

Item::Item ( BString outputPath )
{
	title = BString("");
	description = BString("");
	homePage = BString("");
	postUrl  = BString("");
	content  = BString("");
//	pubDate  = NULL;
	outputDir = outputPath;
}

bool
Item::Filetize ( Config* cfg, bool onlyIfNew = false )
{
	BDirectory* dir = new BDirectory( outputDir );
	BFile* file = new BFile( title.String(), B_READ_WRITE );

	dir->CreateFile( title.String(), file );

	BString betype = cfg->mimetype;
	if ( pubDate != NULL ) {
		int32 unixDate = (int32)pubDate.Time_t();
		file->WriteAttr( "unixDate", B_INT32_TYPE, 0,
				 &unixDate, sizeof(int32) );
		file->WriteAttr( "pubDate", B_STRING_TYPE, 0,
				 dateTo3339String(pubDate).String(),
				 dateTo3339String(pubDate).CountChars() );
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

bool Item::SetTitle ( const char* titleStr ) {
	if ( titleStr != NULL )	title = BString( titleStr );
	else return false;
	return true;
}
bool Item::SetTitle ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	return SetTitle( elem->GetText() );
	return false;
}

bool Item::SetDesc ( const char* descStr ) {
	if ( descStr != NULL )	description = BString( descStr );
	else return false;
	return true;
}
bool Item::SetDesc ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	return SetDesc( elem->GetText() );
	return false;
}

bool Item::SetContent ( const char* contentStr ) {
	if ( contentStr != NULL )	content = BString( contentStr );
	else return false;
	return true;
}
bool Item::SetContent ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	return SetContent( elem->GetText() );
	return false;
}

bool Item::SetPostUrl ( const char* urlStr ) {
	if ( urlStr != NULL )	postUrl = BString( urlStr );
	else return false;
	return true;
}
bool Item::SetPostUrl ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	return SetPostUrl( elem->GetText() );
	return false;
}

bool Item::SetPubDate ( const char* dateStr ) {
	if ( dateStr == NULL )
		return false;
	BDateTime date = feedDateToBDate( dateStr );
	if ( date == NULL )
		return false;
	pubDate = date;
	return true;
}
bool Item::SetPubDate ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	return SetPubDate( elem->GetText() );
	return false;
}
