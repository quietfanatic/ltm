#include "ltm.h"
#include "tostr.h"


MSpec SR_transform(Match m);

int main () {
	MSpec* SR_expr = malloc(sizeof(MSpec));
	MSpec* SR_group = malloc(sizeof(MSpec));
	MSpec* SR_class = malloc(sizeof(MSpec));
	MSpec* SR_char = malloc(sizeof(MSpec));
	MSpec* SR_atom = malloc(sizeof(MSpec));
	MSpec* SR_quant = malloc(sizeof(MSpec));

	// In perl6-ish notation
	// expr { ( <atom> <quant> )* }
	// group { '(' (expr) ')' }
	// class { '[' [ $<one> = <char> || $<range> = <char>\-<char> ] ']' }
	// char { $<lit> = . || \\ $<esc> = . }
	// atom { <group> || <class> || <char> || $<any> = '.' }
	// quant { <[*+?]> }

	*SR_expr =
	create_MScope(
		create_MRep(0, 0, 2000000000,
			create_MNameCap("unit",
				create_MScope(
					create_MGroup(2,
						create_MNameCap("atom",
							create_MRef(SR_atom)
						),
						create_MOpt(
							create_MNameCap("quant",
								create_MRef(SR_quant)
							)
						)
					)
				)
			)
		)
	);

	*SR_group =
	create_MScope(
		create_MGroup(3,
			create_MChar('('),
			create_MCap(
				create_MRef(SR_expr)
			),
			create_MChar(')')
		)
	);

	*SR_class =
	create_MScope(
		create_MGroup(3,
			create_MChar('['),
			create_MRep(0, 0, 2000000000,
				create_MCap(
					create_MScope(
						create_MAlt(2,
							create_MNameCap("one",
								create_MRef(SR_char)
							),
							create_MNameCap("range",
								create_MScope(
									create_MGroup(3,
										create_MCap(create_MRef(SR_char)),
										create_MChar('-'),
										create_MCap(create_MRef(SR_char))
									)
								)
							)
						)
					)
				)
			),
			create_MChar(']')
		)
	);
	
	*SR_char = 
	create_MScope(
		create_MAlt(2,
			create_MGroup(2,
				create_MChar('\\'),
				create_MNameCap("esc",
					create_MAny()
				)
			),
			create_MNameCap("lit",
				create_MCharClass_s(1, 10, "\\\\(())[[]]**++\?\?{{}}")
			)
		)
	);

	*SR_atom =
	create_MScope(
		create_MAlt(4,
			create_MNameCap("group",
				create_MRef(SR_group)
			),
			create_MNameCap("class",
				create_MRef(SR_class)
			),
			create_MNameCap("char",
				create_MRef(SR_char)
			),
			create_MNameCap("any",
				create_MChar('.')
			)
		)
	);

	*SR_quant = 
	create_MScope(
		create_MCharClass_s(0, 3, "**++\?\?")
	);
	finish_MSpec(SR_expr);
	puts(mspec_to_str(*SR_expr));
	finish_MSpec(SR_group);
	puts(mspec_to_str(*SR_group));
	finish_MSpec(SR_class);
	puts(mspec_to_str(*SR_class));
	finish_MSpec(SR_char);
	puts(mspec_to_str(*SR_char));
	finish_MSpec(SR_atom);
	puts(mspec_to_str(*SR_atom));
	finish_MSpec(SR_quant);
	puts(mspec_to_str(*SR_quant));

	Match SR_test_1 = LTM_match_at(*SR_expr, "abc", 0);
	Match SR_test_2 = LTM_match_at(*SR_expr, "abc(abc)abc", 0);
	Match SR_test_3 = LTM_match_at(*SR_expr, "abc[abc]abc", 0);
	Match SR_test_4 = LTM_match_at(*SR_expr, "a(a|ab)(a|ab|abc)*", 0);
	Match SR_test_5 = LTM_match_at(*SR_expr, "[a-zA-Z0-9]", 0);
	printf("%d\n", SR_test_1.end);
	printf("%d\n", SR_test_2.end);
	printf("%d\n", SR_test_3.end);
	printf("%d\n", SR_test_4.end);
	printf("%d\n", SR_test_5.end);
	destroy_Match(SR_test_1);
	destroy_Match(SR_test_2);
	destroy_Match(SR_test_3);
	destroy_Match(SR_test_4);
	destroy_Match(SR_test_5);


	return 0;
}
