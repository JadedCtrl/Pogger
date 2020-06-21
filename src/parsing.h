#ifndef PARSE_H
#define PARSE_H

#include <iostream>
#include <raptor2/raptor2.h>
#include "Channel.h"


void	feedParser		(Channel**);
void	feedHandler		( void*, raptor_statement* );
void	handleFeedStatement	( Channel**, raptor_statement* );
void	handleChannelStatement	( Channel**, BString, BString );
void	handleItemStatement	( Channel**, BString, BString, std::string );

int	countItemParser		( const char* );
void	countItemHandler	( void*, raptor_statement* );

void	printStatementParser	( const char* );
void	printStatementHandler	( void*, raptor_statement* );

BString	getPredicateTag		( const char* );
BString	getPredicateTag		( BString );
BString	getPredicateTag		( std::string );
std::string	to_utf		( uint32 );
std::string	unescape	( std::string, std::string );
std::string	unescape	( const char* );

#endif
