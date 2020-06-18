#include <raptor2/raptor2.h>
//#include "Channel.h"
#include "Parse.h"

// predicate == sweet https version of tag (e.g. <http://purl.org/rss/1.0/modules/rss091#language> )
// subject == parent
// object == data

int
countAllShit ( const char* filePath )
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

Channel*
parseRssFile ( const char* filePath )
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

	int itemCount = countAllShit( filePath );

//	int* itemCount = (int*)malloc( sizeof(int) );
//	*itemCount = 0;
//	raptor_parser_set_statement_handler( rss_parser, &itemCount, countItemHandler );
//	raptor_parser_parse_file( rss_parser, uri, base_uri );

	Channel* chan = (Channel*)malloc( sizeof(Channel) );
	chan = new Channel(itemCount);

	raptor_parser_set_statement_handler( rss_parser, &chan, channelHandler );
	raptor_parser_parse_file( rss_parser, uri, base_uri );

	raptor_free_parser(rss_parser);
	raptor_free_uri(base_uri);
	raptor_free_uri(uri);
	raptor_free_memory(uri_string);	
	raptor_free_world( world );

	return chan;
}

void
channelHandler ( void* user_data, raptor_statement* statement ) 
{
	if ( user_data != NULL ) {
		Channel** chanPtr = (Channel**)user_data;
		parseRssStatement( chanPtr, statement );
	}
}

void
countItemHandler ( void* user_data, raptor_statement* statement ) 
{
	int** countPtr = (int**)user_data;
	int* count = *(countPtr);

	const char* object = (const char*)raptor_term_to_string(statement->object);
	const char* predicate = (const char*)raptor_term_to_string(statement->predicate);

	if (getPredicateTag(predicate) == "type"
	    && getPredicateTag(object) == "item")
		*count += 1;
}


// ============================================================================

void
parseRssStatement ( Channel** chanPtr, raptor_statement* statement )
{
	Channel* chan = *(chanPtr);
	BString predicate = BString( (const char*)raptor_term_to_string(statement->predicate) );
	BString subject   = BString( (const char*)raptor_term_to_string(statement->subject) );
	BString object    = BString( (const char*)raptor_term_to_string(statement->object) );
	predicate = getPredicateTag( predicate );

	if (predicate == "type" && getPredicateTag(object) == "channel")
		chan->topLevelSubject = subject;

	if ( subject != chan->topLevelSubject )
//		parseChannelStatement( chanPtr, predicate, object );
//	else
		parseItemStatement( chanPtr, subject, predicate, object );
}

void
parseChannelStatement ( Channel** chanPtr, BString predicate, BString object )
{
	Channel* chan = *(chanPtr);
}

void
parseItemStatement ( Channel** chanPtr, BString subject, BString predicate, BString object )
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
