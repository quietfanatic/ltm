
MSpec create_MOpt (MSpec possible) {
	MSpec r;
	r.type          = MOPT;
	r.Opt.possible  = malloc(sizeof(MSpec));
	*r.Opt.possible = possible;
	return r;
}

static inline void LTM_finish_MOpt (MSpec* spec, MSpec* scope) {
	LTM_finish_MSpec(spec->Opt.possible, scope);
	return;
}

static inline void LTM_destroy_MSpecOpt (MSpec spec) {
	free(spec.Opt.possible);
	return;
}

static inline void LTM_destroy_MatchOpt (Match m) {
	destroy_Match(*m.Opt.possible);
	free(m.Opt.possible);
	return;
}


static inline void LTM_start_MOpt (Match* m, MStr_t str, Match* scope) {
	m->Opt.possible = malloc(sizeof(Match));
	if (!m->Opt.possible) die("Could not malloc m.Opt.elements");
	LTM_init_Match(m->Opt.possible, m->spec->Opt.possible, m->start);
	LTM_start(m->Opt.possible, str, scope);
	if (m->Opt.possible->type == NOMATCH) {
		DEBUGLOG7(" ## Matching MOpt (as null)\n");
		free(m->Opt.possible);
		m->type = MNULL;
		m->end  = m->start;
		return;
	}
	DEBUGLOG7(" ## Matching MOpt\n");
	m->end = m->Opt.possible->end;
	return;
}

static inline void LTM_backtrack_MOpt (Match* m, MStr_t str, Match* scope) {
	LTM_backtrack(m->Opt.possible, str, scope);
	if (m->Opt.possible->type == NOMATCH) {
		DEBUGLOG7(" ## Matching MOpt (as null)\n");
		free(m->Opt.possible);
		m->end = m->start;
		m->type = MNULL;
		return;
	}
	DEBUGLOG7(" ## Matching MOpt\n");
	m->end = m->Opt.possible->end;
	return;
}
