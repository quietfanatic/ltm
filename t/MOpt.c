#include "t/test.h"
#include "ltm.h"


int main () {
	plan(9);
	
	MSpec opt1 = { .Opt = {
		.type = MOPT,
		.flags = 0,
		.possible = (MSpec[]) {
			{ .Char = {
				.type = MCHAR,
				.flags = 0,
				.c = 'a'
			}}
		}
	}};
	finish_MSpec(&opt1);
	
	char* teststr = "abcdef";
	
	Match result = LTM_match_at(&opt1, teststr, 0);
	isnt(result.type, NOMATCH, "MOpt(a) matched beginning of \"abcdef\"");
	is(result.start, 0, "MOpt.start is correct");
	is(result.end, 1, "MOpt.end is correct");
	destroy_Match(result);

	result = LTM_match_at(&opt1, teststr, 1);
	isnt(result.type, NOMATCH, "MGroup(abc) matches later in \"abcdef\"");
	is(result.start, 1, "MOpt.start is correct");
	is(result.end, 1, "MOpt can be zero-width)");
	destroy_Match(result);

	result = LTM_match_at(&opt1, teststr, 6);
	isnt(result.type, NOMATCH, "MGroup(abc) matches at end of string");
	is(result.start, 6, "MOpt.start is correct");
	is(result.end, 6, "MOpt at end is zero-width)");
	destroy_Match(result);


	return 0;
}

