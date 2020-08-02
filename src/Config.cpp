#include <iostream>
#include <String.h>
#include "Config.h"

Config::Config () {
	verbose     = false;
	daemon      = true;
	will_save   = false;
	updateFeeds = false;
}

// !! handle file status
void
Config::Load ()
{
	if ( configDir == NULL )
		configDir = BString( "/boot/home/config/settings/Pogger/" );

	BString filename = BString(configDir);
	filename.Append("settings");
	BFile* file = new BFile( filename.String(), B_READ_ONLY );
	status_t result = file->InitCheck();

	BMessage storage;
	storage.Unflatten( file );

	if ( outDir == NULL)
		outDir   = BString( storage.GetString("outDir", "/boot/home/feeds/") );
	if ( cacheDir == NULL)
		cacheDir = BString( storage.GetString("cacheDir", "/boot/home/config/cache/Pogger/") );
	delete file;
}

// !! handle file status
void
Config::Save ()
{
	if ( configDir == NULL )
		configDir = BString( "/boot/home/config/settings/Pogger/" );

	BPath*      cfgPath  = new BPath( configDir.String(), NULL, true );
	BEntry*     cfgEntry = new BEntry( cfgPath->Path() );
	BDirectory* cfgDir   = new BDirectory;

	cfgDir->CreateDirectory(cfgPath->Path(), NULL);
	
	if ( cfgEntry->Exists() == false )
		cfgDir->CreateDirectory( cfgPath->Path(), NULL );

	BMessage storage;
	BString filename = BString( configDir ).Append("/settings");

	BFile* file = new BFile( filename.String(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE );
	status_t result = file->InitCheck();

	storage.AddString( "outDir", outDir.String() );
	storage.AddString( "cacheDir", cacheDir.String() );

	storage.Flatten( file );
}

