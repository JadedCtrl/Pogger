#include <iostream>
#include <fstream>
#include <raptor2/raptor2.h>
#include <StorageKit.h>
#include "Item.h"

Item::Item ( BString localSubject, BString outputPath )
{
	subject	= localSubject;
	title = BString("");
	description = BString("");
	homePage = BString("");
	postUrl  = BString("");
	content  = "";
	pubDate  = BString("");
	outputDir = outputPath;
}

bool
Item::Filetize ( bool onlyIfNew = false )
{
	BDirectory* dir = new BDirectory( outputDir );
	BFile* file = new BFile( title.String(), B_READ_WRITE );

	dir->CreateFile( title.String(), file );

	BString betype = "text/html";

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

	// using file->Write with content converted to C string messes up length ofc
	// this is required to preserve length (because of UTF char substitutions in parsing.cpp)
	const char* strPath = outputDir.String();
	std::string path(strPath);
	path += std::string(title.String());
	std::cout << path << std::endl;

	std::ofstream pFile(path);
	pFile << content;
	pFile.close();
	return false;
}
