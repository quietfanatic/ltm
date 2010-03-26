#include "t/test.h"
#include "ltm.h"


int main () {
	plan(11);
	
	MSpec alt1 = { .Alt = {
		.type = MALT,
		.flags = 0,
		.nalts = 3,
		.alts = (MSpec[]) {
			{ .Char = {
				.type = MCHAR,
				.flags = 0,
				.c = 'a'
			}},
			{ .Char = {
				.type = MCHAR,
				.flags = 0,
				.c = 'd'
			}},
			{ .Char = {
				.type = MCHAR,
				.flags = 0,
				.c = 'e'
			}}
		}
	}};
	finish_MSpec(&alt1);
	
	char* teststr = "abcdef";
	
	Match result = LTM_match_at(&alt1, teststr, 0);
	isnt(result.type, NOMATCH, "MAlt(a|d|e) matches first alternative");
	is(result.start, 0, "MAlt.start is correct");
	is(result.end, 1, "MAlt.end is correct");
	destroy_Match(result);

	result = LTM_match_at(&alt1, teststr, 1);
	is(result.type, NOMATCH, "MAlt(a|d|e) doesn't match b");
	destroy_Match(result);

	result = LTM_match_at(&alt1, teststr, 3);
	isnt(result.type, NOMATCH, "MAlt(a|d|e) matches second alternative");
	is(result.start, 3, "MAlt.start is correct");
	is(result.end, 4, "MAlt.end is correct");
	destroy_Match(result);

	result = LTM_match_at(&alt1, teststr, 4);
	isnt(result.type, NOMATCH, "MAlt(a|d|e) matches third alternative");
	is(result.start, 4, "MAlt.start is correct");
	is(result.end, 5, "MAlt.end is correct");
	destroy_Match(result);

	result = LTM_match_at(&alt1, teststr, 6);
	is(result.type, NOMATCH, "MAlt(a|d|e) doesn't match end of string");
	destroy_Match(result);


	return 0;
}

