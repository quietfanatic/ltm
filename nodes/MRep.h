static inline void LTM_fail_MRep (Match* m) {
	free(m->Rep.matches);
	m->type = NOMATCH;
	return;
}

static inline void LTM_succeed_MRep (Match* m) {
	m->end = m->Rep.matches[m->Rep.nmatches-1].end;
	return;
}

static inline void LTM_walk_MRep (Match* m, MStr_t str) {
	for (;;) {  // If child didn't match, backtrack left
		if (m->Rep.matches[m->Rep.nmatches-1].type == NOMATCH) {
			if (m->Rep.nmatches == 0) {  // All the way left
				DEBUGLOG(" ## Not matching MRepMax (Out of tokens to backtrack)\n");
				return LTM_fail_MRep(m);
			}
			m->Rep.nmatches--;
			if (m->Rep.nmatches >= m->spec->Rep.min) {  // We've got enough
				DEBUGLOG(" ## Matching MRepMax (enough matches)\n");
				return LTM_succeed_MRep(m);
			}
			backtrack(&m->Rep.matches[m->Rep.nmatches-1], str);
		}  // If child did match, advance right
		else {
			if (m->Rep.nmatches == m->spec->Rep.max) {  // We're all full
				DEBUGLOG(" ## Matching MRepMax (maximum matches)\n");
				return LTM_succeed_MRep(m);
			}
			m->Rep.nmatches++;
			m->Rep.matches = realloc(m->Rep.matches, m->Rep.nmatches * sizeof(Match));
			if (!m->Rep.matches) die("Could not realloc m->Rep.matches");
			LTM_init_Match(&m->Rep.matches[m->Rep.nmatches-1], m->spec->Rep.child, m->Rep.matches[m->Rep.nmatches-2].end);
			LTM_start(&m->Rep.matches[m->Rep.nmatches-1], str);
		}
	}
}

inline void LTM_start_MRepMax (Match* m, MStr_t str) {
	if (m->spec->Rep.max == 0) {  // Why do you want to repeat 0 times?
		DEBUGLOG(" ## Matching MRepMax (as null)\n");
		m->end = m->start;
		m->type = MNULL;
		return;
	}
	m->Rep.nmatches   = 1;
	m->Rep.matches    = malloc(sizeof(Match));
	if (!m->Rep.matches) die("Could not malloc m->Rep.matches");
	LTM_init_Match(&m->Rep.matches[0], m->spec->Rep.child, m->start);
	LTM_start(&m->Rep.matches[0], str);
	LTM_walk_MRep(m, str);
	return;
}


inline void LTM_backtrack_MRepMax (Match* m, MStr_t str) {
	if (m->Rep.nmatches == 0) {
		DEBUGLOG(" ## Not matching MRepMax\n");
		return LTM_fail_MRep(m);
	}
	backtrack(&m->Rep.matches[m->Rep.nmatches-1], str);
	LTM_walk_MRep(m, str);
	return;
}


