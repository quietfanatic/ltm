
inline void LTM_start_MAlt (Match* m, MStr_t str) {
	m->Alt.matched = malloc(sizeof(Match));
	if (!m->Alt.matched) die("Could not malloc m.Alt.matched");
	for (m->Alt.alti = 0; m->Alt.alti < m->spec->Alt.nalts; m->Alt.alti++) {
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



inline void LTM_backtrack_MAlt (Match* m, MStr_t str) {
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
