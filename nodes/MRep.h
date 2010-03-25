
MSpec create_MRep (MFlags_t flags, size_t min, size_t max, MSpec child) {
	MSpec r;
	r.type       = MREP;
	r.flags      = flags;
	r.Rep.child  = malloc(sizeof(MSpec));
	*r.Rep.child = child;
	r.Rep.min    = min;
	r.Rep.max    = max;
	return r;
}

static inline void LTM_finish_MRep (MSpec* spec, MSpec* scope) {
	LTM_finish_MSpec(spec->Rep.child, scope);
	return;
}

static inline void LTM_destroy_MSpecRep (MSpec spec) {
	destroy_MSpec(*spec.Rep.child);
	free(spec.Rep.child);
	return;
}

static inline void LTM_destroy_MatchRep (Match m) {
	int i;
	for (i=0; i < m.Rep.nmatches; i++)
		destroy_Match(*m.Rep.matches[i]);
	free(m.Rep.matches);
	return;
}



static inline void LTM_fail_MRep (Match* m) {
	free(m->Rep.matches);
	m->type = NOMATCH;
	return;
}

static inline void LTM_succeed_MRep (Match* m) {
	m->end = m->Rep.matches[m->Rep.nmatches-1]->end;
	return;
}

static inline void LTM_walk_MRep (Match* m, MStr_t str, Match* scope) {
	for (;;) {  // If child didn't match, backtrack left
		if (m->Rep.matches[m->Rep.nmatches-1]->type == NOMATCH) {
			m->Rep.nmatches--;
			free(m->Rep.matches[m->Rep.nmatches]);
			if (m->Rep.nmatches >= m->spec->Rep.min) {  // We've got enough
				DEBUGLOG7(" ## Matching MRep (with %d matches) at %d\n", m->Rep.nmatches, m->Rep.matches[m->Rep.nmatches-1]->end);
				if (m->Rep.nmatches == 0) {
					m->end = m->start;
					return;
				}
				return LTM_succeed_MRep(m);
			}
			if (m->Rep.nmatches == 0) {  // All the way left
				DEBUGLOG7(" ## Not matching MRep (Out of tokens to backtrack)\n");
				return LTM_fail_MRep(m);
			}
			LTM_backtrack(m->Rep.matches[m->Rep.nmatches-1], str, scope);
		}  // If child did match, advance right
		else {
			if (m->Rep.nmatches == m->spec->Rep.max) {  // We're all full
				DEBUGLOG7(" ## Matching MRep (maximum matches)\n");
				return LTM_succeed_MRep(m);
			}
			m->Rep.nmatches++;
			m->Rep.matches = realloc(m->Rep.matches, m->Rep.nmatches * sizeof(Match));
			if (!m->Rep.matches) die("Could not realloc m->Rep.matches");
			m->Rep.matches[m->Rep.nmatches-1] = malloc(sizeof(Match));
			if (!m->Rep.matches[m->Rep.nmatches-1]) die("Could not realloc m->Rep.matches[m->Rep.nmatches-1]");
			LTM_init_Match(m->Rep.matches[m->Rep.nmatches-1], m->spec->Rep.child, m->Rep.matches[m->Rep.nmatches-2]->end);
			LTM_start(m->Rep.matches[m->Rep.nmatches-1], str, scope);
		}
	}
}

static inline void LTM_start_MRep (Match* m, MStr_t str, Match* scope) {
	if (m->spec->Rep.max == 0) {  // Why do you want to repeat 0 times?
		DEBUGLOG7(" ## Matching MRep (as null)\n");
		m->end = m->start;
		m->type = MNULL;
		return;
	}
	m->Rep.nmatches   = 1;
	m->Rep.matches    = malloc(sizeof(Match*));
	if (!m->Rep.matches) die("Could not malloc m->Rep.matches");
	m->Rep.matches[0] = malloc(sizeof(Match));
	if (!m->Rep.matches[0]) die("Could not malloc m->Rep.matches[0]");
	LTM_init_Match(m->Rep.matches[0], m->spec->Rep.child, m->start);
	LTM_start(m->Rep.matches[0], str, scope);
	LTM_walk_MRep(m, str, scope);
	return;
}


static inline void LTM_backtrack_MRep (Match* m, MStr_t str, Match* scope) {
	if (m->Rep.nmatches == 0) {
		DEBUGLOG7(" ## Not matching MRep\n");
		return LTM_fail_MRep(m);
	}
	LTM_backtrack(m->Rep.matches[m->Rep.nmatches-1], str, scope);
	LTM_walk_MRep(m, str, scope);
	return;
}

static inline void LTM_abort_MRep (Match* m, MStr_t str, Match* scope) {
	int i;  // Have to go through them backwards.
	for (i = m->Rep.nmatches-1; i >= 0; i--) {
		LTM_abort(m->Rep.matches[i], str, scope);
		free(m->Rep.matches[i]);
	}
	free(m->Rep.matches);
	return;
}

