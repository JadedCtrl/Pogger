#include <cstdio>
#include "Channel.h"

Channel::Channel ( int itemCount )
{
	title = BString("Untitled Feed");
	description = BString("Nondescript, N/A.");
	homePage = BString("");
	xmlUrl = BString("");
	items = BList(itemCount);
	topLevelSubject = "";
	lastSubject = "";
}

// ============================================================================

Item::Item ( BString localSubject )
{
	subject	= localSubject;
	title = BString("");
	description = BString("");
	homePage = BString("");
	postUrl  = BString("");
	content  = BString("");
}

void
Item::Print ()
{
//	printf("%s\t%s\n%s\n\n", subject.String(), title.String(), content.String());
	printf("%s\t%s\n", subject.String(), title.String());
}
