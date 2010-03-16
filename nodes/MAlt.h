
MSpec create_MAlt (MFlags_t flags, int nalts, ...) {
	MSpec r;
	r.type      = MALT;
	r.flags     = flags;
	r.Alt.nalts = nalts;
	r.Alt.alts  = malloc(nalts * sizeof(MSpec));
	va_list alts;
	va_start (alts, nalts);
	int i;
	for (i=0; i < nalts; i++)
		r.Alt.alts[i] = va_arg(alts, MSpec);
	va_end(alts);
	return r;
}

static inline void LTM_finish_MAlt (MSpec* spec, MSpec* scope) {
	int i;
	for (i=0; i < spec->Alt.nalts; i++)
		LTM_finish_MSpec(&spec->Alt.alts[i], scope);
	return;  // Optimization stuff to go here
}

static inline void LTM_destroy_MSpecAlt (MSpec spec) {
	int i;
	for (i=0; i < spec.Alt.nalts; i++)
		destroy_MSpec(spec.Alt.alts[i]);
	free(spec.Alt.alts);
	return;
}

static inline void LTM_destroy_MatchAlt (Match m) {
	destroy_Match(*m.Alt.matched);
	free(m.Alt.matched);
	return;
}


static inline void LTM_start_MAlt (Match* m, MStr_t str, Match* scope) {
	m->Alt.matched = malloc(sizeof(Match));
	if (!m->Alt.matched) die("Could not malloc m.Alt.matched");
	for (m->Alt.alti = 0; m->Alt.alti < m->spec->Alt.nalts; m->Alt.alti++) {
		LTM_init_Match(m->Alt.matched, &m->spec->Alt.alts[m->Alt.alti], m->start);
		LTM_start(m->Alt.matched, str, scope);
		if (m->Alt.matched->type != NOMATCH) {
			DEBUGLOG7(" ## Matching MAlt at %d\n", m->Alt.matched->end);
			m->end = m->Alt.matched->end;
			return;
		}
	}  // None matched  (An empty Alt always fails, too)
	DEBUGLOG7(" ## Not matching MAlt\n");
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
				DEBUGLOG7(" ## Matching MAlt at %d\n", m->Alt.matched->end);
				m->end = m->Alt.matched->end;
				return;
			}
		}  // Out of options.
		DEBUGLOG7(" ## Not matching MAlt\n");
		free(m->Alt.matched);
		m->type = NOMATCH;
		return;
	}
	DEBUGLOG7(" ## Matching MAlt at %d\n", m->Alt.matched->end);
	m->end = m->Alt.matched->end;
	return;
}

static inline void LTM_abort_MAlt (Match* m, MStr_t str, Match* scope) {
	LTM_abort(m->Alt.matched, str, scope);
	free(m->Alt.matched);
	return;
}
