
inline void LTM_start_MOpt (Match* m, MStr_t str) {
	m->Opt.possible = malloc(sizeof(Match));
	if (!m->Opt.possible) die("Could not malloc m.Opt.elements");
	*m->Opt.possible = make_match(m->spec->Opt.possible, str, m->start);
	if (m->Opt.possible->type == NOMATCH) {
		DEBUGLOG(" ## Matching MOpt (as null)\n");
		free(m->Opt.possible);
		m->type = MNULL;
		m->end  = m->start;
		return;
	}
	DEBUGLOG(" ## Matching MOpt\n");
	m->end = m->Opt.possible->end;
	return;
}

inline void LTM_backtrack_MOpt (Match* m, MStr_t str) {
	DEBUGLOG(" ## Matching MOpt (as null)\n");
	free(m->Opt.possible);
	m->end = m->start;
	m->type = MNULL;
	return;
}
