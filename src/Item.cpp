#include <iostream>
#include <fstream>
#include <tinyxml2.h>
#include <StorageKit.h>
#include "Config.h"
#include "Item.h"

Item::Item ( BString outputPath )
{
	title = BString("");
	description = BString("");
	homePage = BString("");
	postUrl  = BString("");
	content  = BString("");
	pubDate  = BString("");
	outputDir = outputPath;
}

bool
Item::Filetize ( Config* cfg, bool onlyIfNew = false )
{
	BDirectory* dir = new BDirectory( outputDir );
	BFile* file = new BFile( title.String(), B_READ_WRITE );

	dir->CreateFile( title.String(), file );

	BString betype = cfg->mimetype;

	file->WriteAttr( "META:title", B_STRING_TYPE, 0,
			 title.String(), title.CountChars() );
	file->WriteAttr( "description", B_STRING_TYPE, 0,
			 description.String(), description.CountChars() );
	file->WriteAttr( "pubDate", B_STRING_TYPE, 0,
			 pubDate.String(), pubDate.CountChars() );
	file->WriteAttr( "META:url", B_STRING_TYPE, 0,
			 postUrl.String(), postUrl.CountChars() );
	file->WriteAttr( "BEOS:TYPE", B_STRING_TYPE, 0,
			 betype.String(), betype.CountChars() );

	file->Write(content.String(), content.Length());
	// using file->Write with content converted to C string messes up length ofc
	// this is required to preserve length (because of UTF char substitutions in parsing.cpp)
//	const char* strPath = outputDir.String();
//	std::string path(strPath);
//	path += std::string(title.String());
//	std::cout << path << std::endl;
//
//	std::ofstream pFile(path);
//	pFile << content;
//	pFile.close();
	return false;
}

void Item::SetTitle ( const char* titleStr ) {
	if ( titleStr != NULL )	title = BString( titleStr );
}
void Item::SetTitle ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	SetTitle( elem->GetText() );
}

void Item::SetDesc ( const char* descStr ) {
	if ( descStr != NULL )	description = BString( descStr );
}
void Item::SetDesc ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	SetDesc( elem->GetText() );
}

void Item::SetContent ( const char* contentStr ) {
	if ( contentStr != NULL )	content = BString( contentStr );
}
void Item::SetContent ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	SetContent( elem->GetText() );
}

void Item::SetPostUrl ( const char* urlStr ) {
	if ( urlStr != NULL )
		postUrl = BString( urlStr );
}
void Item::SetPostUrl ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	SetPostUrl( elem->GetText() );
}

void Item::SetPubDate ( const char* dateStr ) {
	if ( dateStr != NULL )
		pubDate = BString( dateStr );
}
void Item::SetPubDate ( tinyxml2::XMLElement* elem ) {
	if ( elem != NULL )	SetPubDate( elem->GetText() );
}
