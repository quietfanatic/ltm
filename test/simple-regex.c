#include "ltm.h"
#include "tostr.h"


MSpec SR_transform(Match* m, MStr_t str);
MSpec SR_transform_expr(Match* m, MStr_t str);
MSpec SR_transform_group(Match* m, MStr_t str);
MSpec SR_transform_class(Match* m, MStr_t str);
MSpec SR_transform_char(Match* m, MStr_t str);
MSpec SR_transform_atom(Match* m, MStr_t str);
MSpec SR_transform_unit(Match* m, MStr_t str);

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
		create_MRep(0, 0, SIZE_MAX,
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
			create_MRep(0, 0, SIZE_MAX,
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




MSpec SR_transform (Match* m, MStr_t str) {
	return SR_transform_expr(m);
}

MSpec SR_transform_expr (Match* m, MStr_t str) {
	MatchMultiCap units = LTM_lookup_all_NameCaps(m, "unit")
	MSpec* elements = malloc(caps.nplaces * sizeof(MSpec));
	int i;
	for (i=0; i < units.nplaces; i++)
		elements[i] = SR_transform_unit(units.places[i]);
	MSpec r;
	r.type = MGROUP;
	r.nelements = caps.nplaces;
	r.elements = units;
	return r;
}

MSpec SR_transform_unit (Match* m, MStr_t str) {
	Match* quant = LTM_lookup_NameCap(m, "quant");
	if (quant == NULL)
		return SR_transform_atom(LTM_lookup_NameCap(m, "atom"));
	MSpec r;
	switch (MStr_at(str, quant->start)) {
		case '?' {
			r.type = MOPT;
			r.Opt.possible = SR_transform_atom(LTM_lookup_NameCap(m, "atom"));
			return r;
		}
		case '+' {
			r.type = MREP;
			r.Rep.min = 1;
			r.Rep.max = SIZE_MAX;
			r.Rep.child = SR_transform_atom(LTM_lookup_NameCap(m, "atom"));
			return r;
		}
		case '*' {
			r.type = MREP;
			r.Rep.min = 0;
			r.Rep.max = SIZE_MAX;
			r.Rep.child = SR_transform_atom(LTM_lookup_NameCap(m, "atom"));
			return r;
		}
		default: die("Unknown quantifier character");
	}
}

	
