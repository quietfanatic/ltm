#include "ltm.h"
#include "tostr.h"


int main () {
	 // Let's start with a balanced paren matcher
	MSpec* BP_expr = malloc(sizeof(MSpec));
	MSpec* BP_string = malloc(sizeof(MSpec));
	*BP_expr =
	create_MRep(0, 0, 2000000000,
		create_MAlt(0, 2,
			create_MGroup(0, 3,
				create_MChar(0, '('),
				create_MRef(0, BP_expr),  // Recurse here
				create_MChar(0, ')')
			),
			create_MCharClass_s(0, 1, 2, "(())")
		)
	);
	*BP_string = create_MGroup(0, 3,
		create_MBegin(0),
		create_MRef(0, BP_expr),
		create_MEnd(0)
	);
	char* BP_expr_str = mspec_to_str(*BP_expr);
	puts(BP_expr_str);
	 //                12345   6789012345678                                                                901234
	char* BP_test_1 = "asdc(ba)cdsdsacacsac(dsacdsa(AScsadc)()(()cdsac)(cdsacdsacca(((ascdsa)cadscascsad))))asdca()";
	Match BP_test_1_match = LTM_match_at(*BP_string, BP_test_1, 0);
	if (BP_test_1_match.type == NOMATCH)
		puts("BP test 1 failed to match");
	else
		puts("BP test 1 succeeded in matching");
	char* BP_test_2 = "asdc(ba)cdsdsacacsac(dsacdsa(AScsadc)()(()cd(sac)(cdsacdsacca(((ascdsa)cadscascsad))))asdca()";
	Match BP_test_2_match = LTM_match_at(*BP_string, BP_test_2, 0);
	if (BP_test_2_match.type == NOMATCH)
		puts("BP test 2 succeeded in not matching");
	else
		puts("BP test 2 failed to not match");
	destroy_MSpec(*BP_expr);
	free(BP_expr);
	destroy_MSpec(*BP_string);
	free(BP_string);

	 // Quantized captures?
	MSpec* BPC_expr = malloc(sizeof(MSpec));
	MSpec* BPC_string = malloc(sizeof(MSpec));
	*BPC_expr =
	create_MScope(0,
		create_MRep(0, 0, 2000000000,
			create_MCap(0, 
				create_MAlt(0, 2,
					create_MGroup(0, 3,
						create_MChar(0, '('),
						create_MRef(0, BPC_expr),
						create_MChar(0, ')')
					),
					create_MCharClass_s(0, 1, 2, "(())")
				)
			)
		)
	);
	*BPC_string =
	create_MScope(0, 
		create_MGroup(0, 3,
			create_MBegin(0),
			create_MCap(0, create_MRef(0, BPC_expr)),
			create_MEnd(0)
		)
	);
	finish_MSpec(BPC_expr);
	finish_MSpec(BPC_string);
	Match BPCt1 = LTM_match_at(*BPC_string, BP_test_1, 0);
	if (BPCt1.type == NOMATCH)
		puts("BPC test 1 failed to match");
	else
		puts("BPC test 1 succeeded in matching");
	struct MatchMultiCap got = BPCt1.Scope.caps[0]->Cap.child->Scope.caps[0]->MultiCap;
	printf("Got %d / 24 caps\n", got.nplaces);
	int i;
	for (i=0; i < got.nplaces; i++) {
		printf("  %d: %08x -> %d..%d :: %d\n", i, (int) got.places[i], got.places[i]->start, got.places[i]->end, got.places[i]->type);
	}


	return 0;
}

