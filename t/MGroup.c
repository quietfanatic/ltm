#include "t/test.h"
#include "ltm.h"


int main () {
	plan(11);
	
	MSpec group1 = { .Group = {
		.type = MGROUP,
		.flags = 0,
		.nelements = 3,
		.elements = (MSpec[]) {
			{ .Char = {
				.type = MCHAR,
				.flags = 0,
				.c = 'a'
			}},
			{ .Char = {
				.type = MCHAR,
				.flags = 0,
				.c = 'b'
			}},
			{ .Char = {
				.type = MCHAR,
				.flags = 0,
				.c = 'c'
			}}
		}
	}};
	finish_MSpec(&group1);
	
	char* teststr = "abcdef";
	
	Match result = LTM_match_at(&group1, teststr, 0);
	isnt(result.type, NOMATCH, "MGroup(abc) matched beginning of \"abcdef\"");
	is(result.start, 0, "MGroup.start is correct");
	is(result.end, 3, "MGroup.end is correct");
	destroy_Match(result);

	result = LTM_match_at(&group1, teststr, 1);
	is(result.type, NOMATCH, "MGroup(abc) doesn't match later in \"abcdef\"");
	destroy_Match(result);

	MSpec group2 = { .Group = {
		.type = MGROUP,
		.flags = 0,
		.nelements = 3,
		.elements = (MSpec[]) {
			{ .Group = {
				.type = MGROUP,
				.flags = 0,
				.nelements = 2,
				.elements = (MSpec[]) {
					{ .Char = {
						.type = MCHAR,
						.flags = 0,
						.c = 'a'
					}},
					{ .Char = {
						.type = MCHAR,
						.flags = 0,
						.c = 'b'
					}},
				}
			}},
			{ .Char = {
				.type = MCHAR,
				.flags = 0,
				.c = 'c'
			}},
			{ .Group = {
				.type = MGROUP,
				.flags = 0,
				.nelements = 3,
				.elements = (MSpec[]) {
					{ .Char = {
						.type = MCHAR,
						.flags = 0,
						.c = 'd'
					}},
					{ .Char = {
						.type = MCHAR,
						.flags = 0,
						.c = 'e'
					}},
					{ .Group = {
						.type = MGROUP,
						.flags = 0,
						.nelements = 1,
						.elements = (MSpec[]) {
							{ .Char = {
								.type = MCHAR,
								.flags = 0,
								.c = 'f'
							}}
						}
					}}
				}
			}}
		}
	}};
	finish_MSpec(&group2);

	result = LTM_match_at(&group2, teststr, 0);
	isnt(result.type, NOMATCH, "nested MGroup((ab)c(de(f))) matched beginning of \"abcdef\"");
	is(result.start, 0, "nested MGroup.start is correct");
	is(result.end, 6, "nested MGroup.end is correct");
	destroy_Match(result);

	result = LTM_match_at(&group2, teststr, 1);
	is(result.type, NOMATCH, "nested MGroup did not match middle");
	destroy_Match(result);

	MSpec group3 = { .Group = {
		.type = MGROUP,
		.flags = 0,
		.nelements = 0,
		.elements = NULL  // Should not be accessed.
	}};
	finish_MSpec(&group3);

	result = LTM_match_at(&group3, teststr, 0);
	isnt(result.type, NOMATCH, "empty MGroup matches");
	is(result.start, 0, "empty MGroup.start is correct");
	is(result.end, 0, "empty MGroup.end has zero width");
	destroy_Match(result);



	return 0;
}

