#include "ltm.h"
#include <stdint.h>



int main () {
	 // Let's start with a balanced paren matcher
	MSpec* BP_expr = malloc(sizeof(MSpec));
	MSpec* BP_string = malloc(sizeof(MSpec));
	*BP_expr =
	create_MRepMax(
		create_MAlt(2,
			create_MGroup(3,
				create_MChar('('),
				create_MRef(BP_expr),  // Recurse here
				create_MChar(')')
			),
			create_MCharClass_s(1, 2, "(())")
		), 0, 2000000000
	);
	*BP_string = create_MGroup(3,
		create_MBegin(),
		create_MRef(BP_expr),
		create_MEnd()
	);
	char* BP_test_1 = "asdc(ba)cdsdsacacsac(dsacdsa(AScsadc)()(()cdsac)(cdsacdsacca(((ascdsa)cadscascsad))))asdca()";
	Match BP_test_1_match = LTM_match_at(*BP_string, BP_test_1, 0);
	if (BP_test_1_match.type == NOMATCH)
		puts("BP_test_1 failed to match");
	else
		puts("BP_test_1 succeeded in matching");
	char* BP_test_2 = "asdc(ba)cdsdsacacsac(dsacdsa(AScsadc)()(()cd(sac)(cdsacdsacca(((ascdsa)cadscascsad))))asdca()";
	Match BP_test_2_match = LTM_match_at(*BP_string, BP_test_2, 0);
	if (BP_test_2_match.type == NOMATCH)
		puts("BP_test_2 succeeded in not matching");
	else
		puts("BP_test_2 failed to not match");
	destroy_MSpec(*BP_expr);
	free(BP_expr);
	destroy_MSpec(*BP_string);
	free(BP_string);
	return 0;
}

