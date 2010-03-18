#include "ltm.h"
#include "tostr.h"


MSpec SR_transform(Match* m, MStr_t str);
MSpec SR_transform_expr(Match* m, MStr_t str);
MSpec SR_transform_group(Match* m, MStr_t str);
MSpec SR_transform_class(Match* m, MStr_t str);
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
	create_MScope(0,
		create_MRep(0, 0, SIZE_MAX,
			create_MNameCap(0, "unit",
				create_MScope(0,
					create_MGroup(0, 2,
						create_MNameCap(0, "atom",
							create_MRef(0, SR_atom)
						),
						create_MOpt(0, 
							create_MNameCap(0, "quant",
								create_MRef(0, SR_quant)
							)
						)
					)
				)
			)
		)
	);

	*SR_group =
	create_MScope(0, 
		create_MGroup(0, 3,
			create_MChar(0, '('),
			create_MCap(0, 
				create_MRef(0, SR_expr)
			),
			create_MChar(0, ')')
		)
	);

	*SR_class =
	create_MScope(0,
		create_MGroup(0, 3,
			create_MChar(0, '['),
			create_MRep(0, 0, SIZE_MAX,
				create_MCap(0,
					create_MScope(0,
						create_MAlt(0, 2,
							create_MNameCap(0, "one",
								create_MRef(0, SR_char)
							),
							create_MNameCap(0, "range",
								create_MScope(0,
									create_MGroup(0, 3,
										create_MCap(0, create_MRef(0, SR_char)),
										create_MChar(0, '-'),
										create_MCap(0, create_MRef(0, SR_char))
									)
								)
							)
						)
					)
				)
			),
			create_MChar(0, ']')
		)
	);
	
	*SR_char = 
	create_MScope(0,
		create_MAlt(0, 2,
			create_MGroup(0, 2,
				create_MChar(0, '\\'),
				create_MNameCap(0, "esc",
					create_MAny(0)
				)
			),
			create_MNameCap(0, "lit",
				create_MCharClass_s(0, 1, 10, "\\\\(())[[]]**++\?\?{{}}")
			)
		)
	);

	*SR_atom =
	create_MScope(0, 
		create_MAlt(0, 4,
			create_MNameCap(0, "group",
				create_MRef(0, SR_group)
			),
			create_MNameCap(0, "class",
				create_MRef(0, SR_class)
			),
			create_MNameCap(0, "char",
				create_MRef(0, SR_char)
			),
			create_MNameCap(0, "any",
				create_MChar(0, '.')
			)
		)
	);

	*SR_quant = 
	create_MScope(0,
		create_MCharClass_s(0, 0, 3, "**++\?\?")
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
	struct MatchMultiCap units = LTM_lookup_all_NameCaps(m, "unit")
	MSpec* elements = malloc(caps.nplaces * sizeof(MSpec));
	int i;
	for (i=0; i < units.nplaces; i++)
		elements[i] = SR_transform_unit(units.places[i]->Cap.child);
	MSpec r;
	r.flags = flags;
	r.type = MGROUP;
	r.nelements = caps.nplaces;
	r.elements = units;
	return r;
}

MSpec SR_transform_unit (Match* m, MStr_t str) {
	Match* quant = LTM_lookup_NameCap(m, "quant");
	if (quant == NULL)
		return SR_transform_atom(LTM_lookup_NameCap(m, "atom")->NameCap.child);
	switch (MStr_at(str, quant->start)) {
		case '?' {
			return create_MOpt(0, SR_transform_atom(LTM_lookup_NameCap(m, "atom")->NameCap.child));
		}
		case '+' {
			return create_MRep(0,
				1,
				SIZE_MAX,
				SR_transform_atom(LTM_lookup_NameCap(m, "atom")->NameCap.child),
			);
		}
		case '*' {
			return create_MRep(0,
				0,
				SIZE_MAX,
				SR_transform_atom(LTM_lookup_NameCap(m, "atom")->NameCap.child),
			);
		}
		default: die("Unknown quantifier character");
	}
}

MSpec SR_transform_atom (Match* m, MStr_t str) {
	Match* atom;
	return
		  (atom = LTM_lookup_NameCap(m, "group"))
			? SR_transform_group(atom->Cap.child, str)
		: (atom = LTM_lookup_NameCap(m, "class"))
			? SR_transform_class(atom->Cap.child, str)
		: (atom = LTM_lookup_NameCap(m, "char"))
			? SR_transform_char(atom->Cap.child, str)
		: (atom = LTM_lookup_NameCap(m, "any"))
			? create_MAny(0)
		: die("Unknown atom type");
}

MSpec SR_transform_group (Match* m, MStr_t str) {
	return
	create_MScope(0,
		SR_transform_expr(LTM_lookup_Cap(m, 0)->Cap.child);
	);
}

MSpec SR_transform_class (Match* m, MStr_t str) {
	struct MatchMultiCap units = LTM_lookup_all_Caps(m, 0);


