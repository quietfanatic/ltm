#include "t/test.h"
#include "ltm.h"


int main () {
	plan(1);
	
	MSpec nm = {{{
		.type = NOMATCH
	}}};
	
	char* teststr = "abcdef";
	
	Match res1 = LTM_match_at(&nm, teststr, 0);
	is(res1.type, NOMATCH, "NoMatch doesn't match");

	return 0;
}

