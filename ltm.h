#include <stdlib.h>
#include <stdio.h>
#include "match_types.h"
#define DEBUGLOG(x) //(fprintf(stderr, x))


void walk_group_from (Match* m, MStr_t str, size_t i);
void backtrack (Match* m, MStr_t str);
Match make_match (MSpec* m, MStr_t str, size_t start);

void die (char* mess) {
	fputs(mess, stderr);
	abort();
}


// On match success:
//     DEBUGLOG()
//     make sure .nmatches (or such) is set correctly
//     set .end
//     return
// On match failure:
//     DEBUGLOG()
//     free any used memory
//         destroy_match() need not be called, as all inner nodes will already be NOMATCH.
//     set .type to NOMATCH
//     return


void walk_group_from (Match* m, MStr_t str, size_t i) {
	for (;;) {
		if (m->Group.elements[i].type == NOMATCH) {
			if (i == 0) {
				DEBUGLOG(" ## Not matching MGroup (Out of tokens to backtrack)\n");
				free(m->Group.elements);
				m->type = NOMATCH;
				return;
			}
			i--;
			backtrack(&(m->Group.elements[i]), str);
		}
		else {
			i++;
			if (i == m->Group.nelements) {
				DEBUGLOG(" ## Matching MGroup\n");
				m->end = m->Group.elements[i-1].end;
				return;
			}
			m->Group.elements[i] = make_match(&m->spec->Group.elements[i], str, m->Group.elements[i-1].end);
		}
	}
}

void walk_repmax (Match* m, MStr_t str) {
	for (;;) {
		if (m->Rep.matches[m->Rep.nmatches-1].type == NOMATCH) {
			if (m->Rep.nmatches == 0) {
				DEBUGLOG(" ## Not matching MRepMax (Out of tokens to backtrack)\n");
				free(m->Rep.matches);
				return;
			}
			m->Rep.nmatches--;
			if (m->Rep.nmatches >= m->spec->Rep.min) {
				DEBUGLOG(" ## Matching MRepMax (enough matches)\n");
				m->end = m->Rep.matches[m->Rep.nmatches-1].end;
				return;
			}
			backtrack(&m->Rep.matches[m->Rep.nmatches-1], str);
		}
		else {
			if (m->Rep.nmatches == m->spec->Rep.max) {
				DEBUGLOG(" ## Matching MRepMax (maximum matches)\n");
				m->end = m->Rep.matches[m->Rep.nmatches-1].end;
				return;
			}
			m->Rep.nmatches++;
			m->Rep.matches = realloc(m->Rep.matches, m->Rep.nmatches * sizeof(Match));
			if (!m->Rep.matches) die("Could not realloc m->Rep.matches");
			m->Rep.matches[m->Rep.nmatches-1] = make_match(
				m->spec->Rep.child, str, m->Rep.matches[m->Rep.nmatches-2].end
			);
		}
	}
}



void backtrack (Match* m, MStr_t str) {
	switch (m->type) {
		case NOMATCH: {
			die("Error: Tried to backtrack into a failed match.\n");
		}
		case MGROUP: {
			if (m->Group.nelements == 0) {
				m->type = NOMATCH;
				return;
			}
			backtrack(&m->Group.elements[m->Group.nelements-1], str);
			walk_group_from(m, str, m->Group.nelements-1);
			return;
		}
		case MOPT: {
			DEBUGLOG(" ## Matching MOpt (as null)\n");
			free(m->Opt.possible);
			m->end = m->start;
			m->type = MNULL;
			return;
		}
		case MALT: {
			m->Alt.alti++;
			for (; m->Alt.alti < m->spec->Alt.nalts; m->Alt.alti++) {
				*m->Alt.matched = make_match(&m->spec->Alt.alts[m->Alt.alti], str, m->start);
				if (m->Alt.matched->type != NOMATCH) {
					DEBUGLOG(" ## Matching MAlt\n");
					m->end = m->Alt.matched->end;
					return;
				}
			}
			DEBUGLOG(" ## Not matching MAlt\n");
			free(m->Alt.matched);
			m->type = NOMATCH;
			return;
		}
		case MREPMAX: {
			backtrack(&m->Rep.matches[m->Rep.nmatches-1], str);
			walk_repmax(m, str);
			return;
		}
		default: {
			DEBUGLOG(" ## Backtracking past a token\n");
			m->type = NOMATCH;
			return;
		}
	}
}



Match make_match (MSpec* spec, MStr_t str, size_t start) {
	Match m;
	 m.type  = spec->type;
	 m.spec  = spec;
	 m.start = start;
	switch (m.type) {
		case NOMATCH: {
			DEBUGLOG(" ## Not matching NoMatch\n");
			return m;
		}
		case MNULL: {
			DEBUGLOG(" ## Matching MNull\n");
			m.end = m.start;
			return m;
		}
		case MBEGIN: {
			if (start == 0) {
				DEBUGLOG(" ## Matching MBEGIN\n");
				m.end = 0;
				return m;
			}
			DEBUGLOG(" ## Not matching MBEGIN\n");
			m.type = NOMATCH;
			return m;
		}
		case MEND: {
			if (MStr_endat(str, start)) {
				DEBUGLOG(" ## Matching MEND\n");
				m.end = m.start;
				return m;
			}
			DEBUGLOG(" ## Not matching MEND\n");
			m.type = NOMATCH;
			return m;
		}
		case MANY: {
			if (MStr_endat(str, start)) {
				DEBUGLOG(" ## Not matching MAny (end of string)\n");
				m.type = NOMATCH;
				return m;
			}
			DEBUGLOG(" ## Matching MAny\n");
			m.end = m.start + 1;
			return m;
		}
		case MCHAR: {
			if (MStr_at(str, start) == m.spec->Char.c) {
				DEBUGLOG(" ## Matching MChar\n");
				m.end = m.start + 1;
				return m;
			}
			DEBUGLOG(" ## Not Matching MChar\n");
			m.type = NOMATCH;
			return m;
		}
		case MCHARCLASS: {
			int i;
			for (i=0; i < m.spec->CharClass.nranges; i++) {
				if (MStr_at(str, start) < m.spec->CharClass.ranges[i].from) {
					DEBUGLOG(" ## Not matching MCharClass\n");
					m.type = NOMATCH;
					return m;
				}
				if (MStr_at(str, start) <= m.spec->CharClass.ranges[i].to) {
					DEBUGLOG(" ## Matching MCharClass\n");
					m.end = m.start + 1;
					return m;
				}
			}
			DEBUGLOG(" ## Not matching MCharClass\n");
			m.type = NOMATCH;
			return m;
		}
		case MGROUP: {
			m.Group.nelements = m.spec->Group.nelements;
			if (m.Group.nelements == 0) {
				DEBUGLOG(" ## Matching MGroup (empty group)\n");
				m.end = m.start;
				return m;
			}
			m.Group.elements = malloc(m.Group.nelements * sizeof(Match));
			if (!m.Group.elements) die("Could not malloc m.Group.elements");
			m.Group.elements[0] = make_match(&m.spec->Group.elements[0], str, m.start);
			walk_group_from(&m, str, 0);
			return m;
		}
		case MOPT: {
			m.Opt.possible = malloc(sizeof(Match));
			if (!m.Opt.possible) die("Could not malloc m.Opt.elements");
			*m.Opt.possible = make_match(m.spec->Opt.possible, str, m.start);
			if (m.Opt.possible->type == NOMATCH) {
				DEBUGLOG(" ## Matching MOpt (as null)\n");
				free(m.Opt.possible);
				m.type = MNULL;
				m.end  = m.start;
				return m;
			}
			DEBUGLOG(" ## Matching MOpt\n");
			m.end = m.Opt.possible->end;
			return m;
		}
		case MALT: {
			m.Alt.matched = malloc(sizeof(Match));
			if (!m.Alt.matched) die("Could not malloc m.Alt.matched");
			for (m.Alt.alti = 0; m.Alt.alti < m.spec->Alt.nalts; m.Alt.alti++) {
				*m.Alt.matched = make_match(&m.spec->Alt.alts[m.Alt.alti], str, start);
				if (m.Alt.matched->type != NOMATCH) {
					DEBUGLOG(" ## Matching MAlt\n");
					m.end = m.Alt.matched->end;
					return m;
				}
			}
			DEBUGLOG(" ## Not matching MAlt\n");
			free(m.Alt.matched);
			m.type = NOMATCH;
			return m;
		}
		case MREPMAX: {
			m.Rep.nmatches   = 1;
			m.Rep.matches    = malloc(sizeof(Match));
			if (!m.Rep.matches) die("Could not malloc m.Rep.matches");
			m.Rep.matches[0] = make_match(m.spec->Rep.child, str, m.start);
			walk_repmax(&m, str);
			return m;
		}
		default: {
			fprintf(stderr, "Error: Tried to associate unknown match type %d.\n", spec->type);
			abort();
		}
	}
}





