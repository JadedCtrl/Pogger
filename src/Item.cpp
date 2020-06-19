#include <cstdio>
#include <raptor2/raptor2.h>
#include "Item.h"

Item::Item ( BString localSubject )
{
	subject	= localSubject;
	title = BString("");
	description = BString("");
	homePage = BString("");
	postUrl  = BString("");
	content  = BString("");
}
