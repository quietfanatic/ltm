#include "t/test.h"
#include "ltm.h"



int main () {
	plan(19);

	MSpec mnull = {{
		.type = MNULL,
		.flags = 0
	}};
	finish_MSpec(mnull);
	
	char* teststr = "abcdef";
	Match result = LTM_match_at(&mnull, teststr, 0);
	is(result.type, MNULL, "MNull matches at beginning");
	is(result.start, 0, "MNull.start is correct");
	is(result.end, 0, "MNull match has no width");
	destroy_Match(result);

	result = LTM_match_at(&mnull, teststr, 3);
	is(result.type, MNULL, "MNull matches in middle");
	is(result.start, 3, "MNull.start is correct");
	is(result.end, 3, "MNull match has no width");
	destroy_Match(result);

	result = LTM_match_at(&mnull, teststr, 6);
	is(result.type, MNULL, "MNull matches at end");
	is(result.start, 6, "MNull.start is correct");
	is(result.end, 6, "MNull match has no width");
	destroy_Match(result);

	MSpec mbegin = {{
		.type = MBEGIN,
		.flags = 0
	}};
	finish_MSpec(mbegin);

	result = LTM_match_at(&mbegin, teststr, 0);
	is(result.type, MBEGIN, "MBegin matches at beginning");
	is(result.start, 0, "MBegin.start is correct");
	is(result.end, 0, "MBegin match has no width");
	destroy_Match(result);

	result = LTM_match_at(&mbegin, teststr, 2);
	is(result.type, NOMATCH, "MBegin doesn't match in middle");
	destroy_Match(result);

	result = LTM_match_at(&mbegin, teststr, 6);
	is(result.type, NOMATCH, "MBegin doesn't match at end");
	destroy_Match(result);

	MSpec mend = {{
		.type = MEND,
		.flags = 0
	}};
	finish_MSpec(mend);

	result = LTM_match_at(&mend, teststr, 0);
	is(result.type, NOMATCH, "MEnd doesn't match at beginning");
	destroy_Match(result);

	result = LTM_match_at(&mend, teststr, 4);
	is(result.type, NOMATCH, "MEnd doesn't match in middle");
	destroy_Match(result);

	result = LTM_match_at(&mend, teststr, 6);
	is(result.type, MEND, "Mend matches at end");
	is(result.start, 6, "MEnd.start is correct");
	is(result.end, 6, "MEnd match has no width");
	destroy_Match(result);


	return 0;
}








