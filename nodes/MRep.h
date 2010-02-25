
static inline void walk_repmax (Match* m, MStr_t str) {
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

inline void LTM_start_MRepMax (Match* m, MStr_t str) {
	m->Rep.nmatches   = 1;
	m->Rep.matches    = malloc(sizeof(Match));
	if (!m->Rep.matches) die("Could not malloc m.Rep.matches");
	m->Rep.matches[0] = make_match(m->spec->Rep.child, str, m->start);
	walk_repmax(m, str);
	return;
}


inline void LTM_backtrack_MRepMax (Match* m, MStr_t str) {
	backtrack(&m->Rep.matches[m->Rep.nmatches-1], str);
	walk_repmax(m, str);
	return;
}


