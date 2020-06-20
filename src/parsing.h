#ifndef PARSE_H
#define PARSE_H

#include <raptor2/raptor2.h>
#include "Channel.h"


void	feedParser		(Channel**);
void	feedHandler		( void*, raptor_statement* );
void	handleFeedStatement	( Channel**, raptor_statement* );
void	handleChannelStatement	( Channel**, BString, BString );
void	handleItemStatement	( Channel**, BString, BString, BString );

int	countItemParser		( const char* );
void	countItemHandler	( void*, raptor_statement* );

void	printStatementParser	( const char* );
void	printStatementHandler	( void*, raptor_statement* );

BString	getPredicateTag		( char* );
BString	getPredicateTag		( BString );

#endif
