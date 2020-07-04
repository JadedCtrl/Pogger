#include <iostream>
#include <sstream>
#include <raptor2/raptor2.h>
#include "Channel.h"
#include "Item.h"
#include "parsing.h"


// ============================================================================
// PARSERS
void
feedParser ( Channel** chanPtr )
{
	Channel* chan = *(chanPtr);
	raptor_parser* rss_parser = NULL;
	raptor_world* world;
	world = raptor_new_world();

	unsigned char *uri_string;
	raptor_uri *uri, *base_uri;

	rss_parser = raptor_new_parser( world, "rss-tag-soup" );
	uri_string = raptor_uri_filename_to_uri_string( chan->filePath.String() );
	uri = raptor_new_uri( world, uri_string );
	base_uri = raptor_uri_copy( uri );

	raptor_parser_set_statement_handler( rss_parser, &chan, feedHandler );
	raptor_parser_parse_file( rss_parser, uri, base_uri );

	raptor_free_parser( rss_parser );
	raptor_free_uri( base_uri );
	raptor_free_uri( uri );
	raptor_free_memory( uri_string );	
	raptor_free_world( world );
}

// -------------------------------------
int
countItemParser ( const char* filePath )
{
	raptor_parser* rss_parser = NULL;
	raptor_world* world;
	world = raptor_new_world();

	unsigned char *uri_string;
	raptor_uri *uri, *base_uri;

	rss_parser = raptor_new_parser(world, "rss-tag-soup");
	uri_string = raptor_uri_filename_to_uri_string( filePath );
	uri = raptor_new_uri( world, uri_string );
	base_uri = raptor_uri_copy( uri );

	int* itemCount = (int*)malloc( sizeof(int) );
	*itemCount = 0;
	raptor_parser_set_statement_handler( rss_parser, &itemCount, countItemHandler );
	raptor_parser_parse_file( rss_parser, uri, base_uri );

	free( itemCount );
	raptor_free_parser( rss_parser );
	raptor_free_uri( base_uri );
	raptor_free_uri( uri );
	raptor_free_memory( uri_string );	
	raptor_free_world( world );

	return *(itemCount);
}

void
printStatementParser ( const char* filePath )
{
	raptor_parser* rss_parser = NULL;
	raptor_world* world;
	world = raptor_new_world();

	unsigned char *uri_string;
	raptor_uri *uri, *base_uri;

	rss_parser = raptor_new_parser(world, "rss-tag-soup");
	uri_string = raptor_uri_filename_to_uri_string( filePath );
	uri = raptor_new_uri( world, uri_string );
	base_uri = raptor_uri_copy( uri );

	raptor_parser_set_statement_handler( rss_parser, NULL, printStatementHandler );
	raptor_parser_parse_file( rss_parser, uri, base_uri );

	raptor_free_parser( rss_parser );
	raptor_free_uri( base_uri );
	raptor_free_uri( uri );
	raptor_free_memory( uri_string );	
	raptor_free_world( world );
}


// ============================================================================
// HANDLERS
void
feedHandler ( void* user_data, raptor_statement* statement ) 
{
	if ( user_data != NULL ) {
		Channel** chanPtr = (Channel**)user_data;
		handleFeedStatement( chanPtr, statement );
	}
}

void
countItemHandler ( void* user_data, raptor_statement* statement ) 
{
	int** countPtr =  ( int** )user_data;
	int* count     = *(countPtr);

	const char* object    = ( const char* )raptor_term_to_string( statement->object );
	const char* predicate = ( const char* )raptor_term_to_string( statement->predicate );

	if (getPredicateTag(predicate) == "type"
	    && getPredicateTag(object) == "item")
		*count += 1;
}

void
printStatementHandler ( void* user_data, raptor_statement* statement ) 
{
	int** countPtr = (int**)user_data;
	int* count = *(countPtr);

	const char* subject   = ( const char* )raptor_term_to_string( statement->subject );
	const char* predicate = ( const char* )raptor_term_to_string( statement->predicate );
	const char* object    = ( const char* )raptor_term_to_string( statement->object );

	printf("%s\t-%s\n%.50s\n", subject, predicate, object);
}

// ----------------------------------------------------------------------------
// FEEDHANDLER HELPERS
void
handleFeedStatement ( Channel** chanPtr, raptor_statement* statement )
{
	Channel* chan = *(chanPtr);
	const char* cpredicate = (const char*)raptor_term_to_string( statement->predicate );
	const char* csubject   = (const char*)raptor_term_to_string( statement->subject );
	const char* cobject    = (const char*)raptor_term_to_string( statement->object );
	
	BString predicate = BString(cpredicate);
	BString subject   = BString(csubject);
	BString bobject    = BString(cobject);

	bobject.ReplaceAll("\\\"","\"");
	bobject.ReplaceFirst("\"","");
	bobject.ReplaceLast("\"","");

	std::string object  = unescape(bobject.String());

	predicate = getPredicateTag( predicate );

	if ( predicate == "type" && getPredicateTag( object ) == "channel" )
		chan->topLevelSubject = subject;

	if ( subject != chan->topLevelSubject )
//		handleChannelStatement( chanPtr, predicate, object );
//	else
		handleItemStatement( chanPtr, subject, predicate, object );
}

void
handleChannelStatement ( Channel** chanPtr, BString predicate, BString object )
{
	Channel* chan = *(chanPtr);
}

void
handleItemStatement ( Channel** chanPtr, BString subject, BString predicate, std::string object )
{
	Channel* chan = *(chanPtr);
	if ( subject.StartsWith("_:genid") )
		return;

	chan->title = BString("dad");

	if ( subject != chan->lastSubject )  {
		chan->lastSubject = subject;

		Item* newItem = (Item*)malloc( sizeof(Item) );
		newItem = new Item( subject, chan->outputDir );

		chan->items.AddItem( newItem );
	}

	Item* nowItem = (Item*)chan->items.LastItem();
	
	if ( predicate == "title" ) 
		nowItem->title = BString(object.c_str());
	if ( predicate == "encoded" || predicate == "Atomcontent" ) 
		nowItem->content = object;
	if ( predicate == "description" )
		nowItem->description = BString(object.c_str());
	if ( predicate == "link" || predicate == "Atomlink" )
		nowItem->postUrl = BString(object.c_str());
	if ( predicate == "Atomhref" )
		nowItem->postUrl = BString(object.c_str());
	if ( predicate == "date" || predicate == "Atompublished" ) // 2019-02-18T01:43:43Z
		nowItem->pubDate = BString(object.c_str());
	if ( predicate == "pubDate" )  // Sun, 17 Feb 2019 19:43:43 -0600
		nowItem->pubDate = BString(object.c_str());
}


// ============================================================================
// UTIL
BString
getPredicateTag ( BString spec )
{
	int32 lastSlash = spec.FindLast( '/' );
	spec.RemoveChars( 0, lastSlash + 1 );
	int32 lastHash = spec.FindLast( '#' );
	spec.RemoveChars( 0, lastHash + 1 );
	spec.RemoveLast( ">" );

	return spec;
}
BString
getPredicateTag ( const char* spec )
{
	return getPredicateTag( BString(spec) );
}
BString
getPredicateTag ( std::string spec )
{
	return getPredicateTag( spec.c_str() );
}

// ----------------------------------------------------------------------------

/* What ensues is a terrifying violation of the human form.
 * Just atrotious. I deserve to be impaled by by an ice-pick.
 * ... something (unfortunately), directly ripped from StackOverflow.
 * So when getting a raptor_statement's object, it's a char array filled
 * with escaped characters (\U2901, etc).
 * I'm really not sure how to best manage this, so SO.
 * Thanks remy-lebeau, I owe you.
 * https://stackoverflow.com/questions/28534221 */
std::string
toUtf8 ( uint32_t cp )
{
    std::string result;

    int count;
    if (cp <= 0x007F)
        count = 1;
    else if (cp <= 0x07FF)
        count = 2;
    else if (cp <= 0xFFFF)
        count = 3;
    else if (cp <= 0x10FFFF)
        count = 4;
    else
        return result; // or throw an exception

    result.resize(count);

    if (count > 1) {
        for (int i = count-1; i > 0; --i) {
            result[i] = (char) (0x80 | (cp & 0x3F));
            cp >>= 6;
        }

        for (int i = 0; i < count; ++i)
            cp |= (1 << (7-i));
    }

    result[0] = (char) cp;
    return result;
}

std::string
unescape ( std::string str, std::string escape )
{
	std::string::size_type startIdx = 0;
	do
	{
		startIdx = str.find(escape, startIdx);
		if (startIdx == std::string::npos) break;

		std::string::size_type endIdx = str.find_first_not_of("0123456789abcdefABCDEF",
								      startIdx+2);
		if (endIdx == std::string::npos) break;
	
		std::string tmpStr = str.substr(startIdx+2, endIdx-(startIdx+2));
		std::istringstream iss(tmpStr);

		uint32_t cp;
		if (iss >> std::hex >> cp)
		{
		        std::string utf8 = toUtf8(cp);
		        str.replace(startIdx, 2+tmpStr.length(), utf8);
		        startIdx += utf8.length();
		}
		else
			startIdx += 2;
	}
	while (true);	

	return str;
}

std::string
unescape (const char* str )
{
	return unescape(std::string( unescape(std::string(str), "\\u") ), "\\U");
}
