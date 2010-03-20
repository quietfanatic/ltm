#include "t/test.h"
#include "ltm.h"



int main () {
	plan(7);

	MSpec many = {{
		.type = MANY,
		.flags = 0
	}};
	finish_MSpec(&many);
	
	char* teststr = "abcdef";
	Match result = LTM_match_at(&many, teststr, 0);
	is(result.type, MANY, "MAny matches at beginning");
	is(result.start, 0, "MAny.start is correct");
	is(result.end, 1, "MAny match has width 1");
	destroy_Match(result);

	result = LTM_match_at(&many, teststr, 3);
	is(result.type, MANY, "MAny matches in middle");
	is(result.start, 3, "MAny.start is correct");
	is(result.end, 4, "MAny middle match has with 1");
	destroy_Match(result);

	result = LTM_match_at(&many, teststr, 6);
	is(result.type, NOMATCH, "MNull doesn't match at end");
	destroy_Match(result);


	return 0;
}








