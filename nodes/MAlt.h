
static inline void LTM_start_MAlt (Match* m, MStr_t str, Match* scope) {
	m->Alt.matched = malloc(sizeof(Match));
	if (!m->Alt.matched) die("Could not malloc m.Alt.matched");
	for (m->Alt.alti = 0; m->Alt.alti < m->spec->Alt.nalts; m->Alt.alti++) {
		LTM_init_Match(m->Alt.matched, &m->spec->Alt.alts[m->Alt.alti], m->start);
		LTM_start(m->Alt.matched, str, scope);
		if (m->Alt.matched->type != NOMATCH) {
			DEBUGLOG(" ## Matching MAlt at %d\n", m->Alt.matched->end);
			m->end = m->Alt.matched->end;
			return;
		}
	}  // None matched  (An empty Alt always fails, too)
	DEBUGLOG(" ## Not matching MAlt\n");
	free(m->Alt.matched);
	m->type = NOMATCH;
	return;
}



static inline void LTM_backtrack_MAlt (Match* m, MStr_t str, Match* scope) {
	LTM_backtrack(m->Alt.matched, str, scope);
	if (m->Alt.matched->type == NOMATCH) {
		m->Alt.alti++;  // Keep trying
		for (; m->Alt.alti < m->spec->Alt.nalts; m->Alt.alti++) {
			LTM_init_Match(m->Alt.matched, &m->spec->Alt.alts[m->Alt.alti], m->start);
			LTM_start(m->Alt.matched, str, scope);
			if (m->Alt.matched->type != NOMATCH) {
				DEBUGLOG(" ## Matching MAlt at %d\n", m->Alt.matched->end);
				m->end = m->Alt.matched->end;
				return;
			}
		}  // Out of options.
		DEBUGLOG(" ## Not matching MAlt\n");
		free(m->Alt.matched);
		m->type = NOMATCH;
		return;
	}
	DEBUGLOG(" ## Matching MAlt at %d\n", m->Alt.matched->end);
	m->end = m->Alt.matched->end;
	return;
}
