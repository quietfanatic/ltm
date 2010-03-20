#include "t/test.h"
#include "ltm.h"



int main () {
	plan(5);

	MSpec mchar_a = { .Char = {
		.type = MCHAR,
		.flags = 0,
		.c = 'a'
	}};
	
	char* teststr = "abcdef";

	Match result = LTM_match_at(&mchar_a, teststr, 0);
	is(result.type, MCHAR, "MChar(a) matches a");
	is(result.start, 0, "MChar.start is correct");
	is(result.end, 1, "MChar match has width 1");
	destroy_Match(result);

	result = LTM_match_at(&mchar_a, teststr, 3);
	is(result.type, NOMATCH, "MChar(a) doesn't match c");
	destroy_Match(result);

	result = LTM_match_at(&mchar_a, teststr, 6);
	is(result.type, NOMATCH, "MChar(a) doesn't match at end");
	destroy_Match(result);

	return 0;
}








