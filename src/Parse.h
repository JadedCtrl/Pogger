#include <raptor2/raptor2.h>
#include "Channel.h"

Channel* parseRssFile		( const char* );

void	channelHandler		( void*, raptor_statement* );
void	countItemHandler	( void*, raptor_statement* );
void	parseRssStatement	( Channel**, raptor_statement* );
BString	getPredicateTag		( char* );
BString	getPredicateTag		( BString );
void	parseChannelStatement	( Channel**, BString, BString );
void	parseItemStatement	( Channel**, BString, BString, BString );

