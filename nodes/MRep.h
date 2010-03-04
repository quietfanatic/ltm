
MSpec create_MRepMax (MSpec child, size_t min, size_t max) {
	MSpec r;
	r.type       = MREPMAX;
	r.Rep.child  = malloc(sizeof(MSpec));
	*r.Rep.child = child;
	r.Rep.min    = min;
	r.Rep.max    = max;
	return r;
}

static inline void LTM_destroy_MSpecRep (MSpec spec) {
	destroy_MSpec(*spec.Rep.child);
	free(spec.Rep.child);
	return;
}

static inline void LTM_destroy_MatchRep (Match m) {
	int i;
	for (i=0; i < m.Rep.nmatches; i++)
		destroy_Match(m.Rep.matches[i]);
	free(m.Rep.matches);
	return;
}



static inline void LTM_fail_MRep (Match* m) {
	free(m->Rep.matches);
	m->type = NOMATCH;
	return;
}

static inline void LTM_succeed_MRep (Match* m) {
	m->end = m->Rep.matches[m->Rep.nmatches-1].end;
	return;
}

static inline void LTM_walk_MRep (Match* m, MStr_t str, Match* scope) {
	for (;;) {  // If child didn't match, backtrack left
		if (m->Rep.matches[m->Rep.nmatches-1].type == NOMATCH) {
			if (m->Rep.nmatches == 0) {  // All the way left
				DEBUGLOG(" ## Not matching MRepMax (Out of tokens to backtrack)\n");
				return LTM_fail_MRep(m);
			}
			m->Rep.nmatches--;
			if (m->Rep.nmatches == 0) {
				DEBUGLOG(" ## Matching MRepMax (with 0 matches) at %d\n", m->start);
				m->end = m->start;
				return;
			}
			if (m->Rep.nmatches >= m->spec->Rep.min) {  // We've got enough
				DEBUGLOG(" ## Matching MRepMax (with %d matches) at %d\n", m->Rep.nmatches, m->Rep.matches[m->Rep.nmatches-1].end);
				return LTM_succeed_MRep(m);
			}
			LTM_backtrack(&m->Rep.matches[m->Rep.nmatches-1], str, scope);
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
			LTM_start(&m->Rep.matches[m->Rep.nmatches-1], str, scope);
		}
	}
}

static inline void LTM_start_MRepMax (Match* m, MStr_t str, Match* scope) {
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
	LTM_start(&m->Rep.matches[0], str, scope);
	LTM_walk_MRep(m, str, scope);
	return;
}


static inline void LTM_backtrack_MRepMax (Match* m, MStr_t str, Match* scope) {
	if (m->Rep.nmatches == 0) {
		DEBUGLOG(" ## Not matching MRepMax\n");
		return LTM_fail_MRep(m);
	}
	LTM_backtrack(&m->Rep.matches[m->Rep.nmatches-1], str, scope);
	LTM_walk_MRep(m, str, scope);
	return;
}


