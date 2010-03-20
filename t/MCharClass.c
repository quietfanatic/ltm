#include "t/test.h"
#include "ltm.h"



int main () {
	plan(12);
	
	MCharRange* ranges = malloc(3 * sizeof(MCharRange));
	ranges[0].from = 'a';
	ranges[0].to = 'c';
	ranges[1].from = 'x';
	ranges[1].to = 'y';
	ranges[2].from = '|';
	ranges[2].to = '|';

	MSpec mcc = { .CharClass = {
		.type = MCHARCLASS,
		.flags = 0,
		.negative = 0,
		.nranges = 3,
		.ranges = ranges
//		.ranges = (MCharRange[]) {
//			{ .from = 'a', .to = 'c' },
//			{ .from = 'x', .to = 'y' },
//			{ .from = '|', .to = '|' }
//		}
	}};
	finish_MSpec(&mcc);
	
	char* teststr = "abcdefxyz|~";

	Match result = LTM_match_at(&mcc, teststr, 0);
	is(result.type, MCHARCLASS, "MCharClass[a-cx-y|] matches a");
	is(result.start, 0, "MCharClass.start is correct");
	is(result.end, 1, "MCharClass match has width 1");
	destroy_Match(result);

	result = LTM_match_at(&mcc, teststr, 1);
	is(result.type, MCHARCLASS, "MCharClass[a-cx-y|] matches b");
	destroy_Match(result);

	result = LTM_match_at(&mcc, teststr, 2);
	is(result.type, MCHARCLASS, "MCharClass[a-cx-y|] matches c");
	destroy_Match(result);

	result = LTM_match_at(&mcc, teststr, 3);
	is(result.type, NOMATCH, "MCharClass[a-cx-y|] doesn't match d");
	destroy_Match(result);

	result = LTM_match_at(&mcc, teststr, 5);
	is(result.type, NOMATCH, "MCharClass[a-cx-y|] doesn't match f");
	destroy_Match(result);

	result = LTM_match_at(&mcc, teststr, 6);
	is(result.type, MCHARCLASS, "MCharClass[a-cx-y|] matches x");
	destroy_Match(result);

	result = LTM_match_at(&mcc, teststr, 7);
	is(result.type, MCHARCLASS, "MCharClass[a-cx-y|] matches y");
	destroy_Match(result);

	result = LTM_match_at(&mcc, teststr, 8);
	is(result.type, NOMATCH, "MCharClass[a-cx-y|] doesn't match z");
	destroy_Match(result);

	result = LTM_match_at(&mcc, teststr, 9);
	is(result.type, MCHARCLASS, "MCharClass[a-cx-y|] matches |");
	destroy_Match(result);

	result = LTM_match_at(&mcc, teststr, 10);
	is(result.type, NOMATCH, "MCharClass[a-cx-y|] doesn't match ~");
	destroy_Match(result);


	return 0;
}








