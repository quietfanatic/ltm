#include "t/test.h"
#include "ltm.h"



int main () {
	plan(9);

	MSpec rep1 = { .Rep = {
		.type = MREP,
		.flags = 0,
		.min = 0,
		.max = 4,
		.child = (MSpec[]) {{ .Char = {
			.type = MCHAR,
			.flags = 0,
			.c = 'a'
		}}}
	}};
	finish_MSpec(&rep1);
	
	char* teststr = "aaaaaabcd";
	
	Match result = LTM_match_at(&rep1, teststr, 0);
	isnt(result.type, NOMATCH, "MRep matches");
	is(result.start, 0, "MRep.start is correct");
	is(result.end, 4, "MRep only matches up to max");
	destroy_Match(result);

	result = LTM_match_at(&rep1, teststr, 3);
	isnt(result.type, NOMATCH, "MRep matches with less than max");
	is(result.start, 3, "MRep.start is correct");
	is(result.end, 6, "MRep only goes as far as it can");
	destroy_Match(result);

	result = LTM_match_at(&rep1, teststr, 6);
	isnt(result.type, NOMATCH, "MRep can match nothing if min is 0");
	is(result.end, 6, "MRep can have zero width");
	destroy_Match(result);

	rep1.Rep.min = 2;

	result = LTM_match_at(&rep1, teststr, 5);
	is(result.type, NOMATCH, "MRep won't match less than min");
	destroy_Match(result);


	return 0;
}








