#include <raptor2/raptor2.h>
#include "Channel.h"
#include "Item.h"
#include "parsing.h"

/* predicate == sweet http version of tag
   subject   == parent
   object    == data */

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

	rss_parser = raptor_new_parser(world, "rss-tag-soup");
	uri_string = raptor_uri_filename_to_uri_string( chan->filePath.String() );
	uri = raptor_new_uri( world, uri_string );
	base_uri = raptor_uri_copy( uri );

	raptor_parser_set_statement_handler( rss_parser, &chan, feedHandler );
	raptor_parser_parse_file( rss_parser, uri, base_uri );

	raptor_free_parser(rss_parser);
	raptor_free_uri(base_uri);
	raptor_free_uri(uri);
	raptor_free_memory(uri_string);	
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

	free(itemCount);
	raptor_free_parser(rss_parser);
	raptor_free_uri(base_uri);
	raptor_free_uri(uri);
	raptor_free_memory(uri_string);	
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

	raptor_free_parser(rss_parser);
	raptor_free_uri(base_uri);
	raptor_free_uri(uri);
	raptor_free_memory(uri_string);	
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

	printf("%s\t-%s\n%.5s\n", subject, predicate, object);
}

// ----------------------------------------------------------------------------
// FEEDHANDLER HELPERS
void
handleFeedStatement ( Channel** chanPtr, raptor_statement* statement )
{
	Channel* chan = *(chanPtr);
	BString predicate = BString(( const char* )raptor_term_to_string( statement->predicate ));
	BString subject   = BString(( const char* )raptor_term_to_string( statement->subject ));
	BString object    = BString(( const char* )raptor_term_to_string( statement->object ));
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
handleItemStatement ( Channel** chanPtr, BString subject, BString predicate, BString object )
{
	Channel* chan = *(chanPtr);
	if ( subject.StartsWith("_:genid") )
		return;

	chan->title = BString("dad");

	if ( subject != chan->lastSubject )  {
		chan->lastSubject = subject;

		Item* newItem = (Item*)malloc( sizeof(Item) );
		newItem = new Item( subject );

		chan->items.AddItem( newItem );
	}

	Item* nowItem = (Item*)chan->items.LastItem();
	
	if ( predicate == "title" ) 
		nowItem->title = object;

	if ( predicate == "encoded" || predicate == "Atomcontent" ) 
		nowItem->content = object;
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
getPredicateTag ( char* spec )
{
	return getPredicateTag( BString(spec) );
}
