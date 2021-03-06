
MSpec create_MScope (MFlags_t flags, MSpec child) {
	MSpec r;
	r.type = MSCOPE;
	r.flags = flags;
	r.Scope.child = malloc(sizeof(MSpec));
	*r.Scope.child = child;
	return r;
}

static inline void LTM_finish_MScope (MSpec* spec, MSpec* scope) {
	spec->Scope.ncaps = 0;
	spec->Scope.nnamecaps = 0;
	spec->Scope.names = malloc(0);
	LTM_finish_MSpec(spec->Scope.child, spec);  // Replace the outer scope
	return;
}


static inline void LTM_destroy_MSpecScope (MSpec spec) {
	destroy_MSpec(*spec.Scope.child);
	free(spec.Scope.child);
	// No need to free names, as they're referencing inner MNameCaps.
	return;
}

static inline void LTM_destroy_MatchScope (Match m) {
	destroy_Match(*m.Scope.child);
	free(m.Scope.child);
	return;
}

static inline void LTM_destroy_MScope_MultiCaps (Match* m) {
	int i;
	for (i=0; i < m->spec->Scope.ncaps + m->spec->Scope.nnamecaps; i++)
		if (m->Scope.caps[i] != NULL)
			if (m->Scope.caps[i]->type == MMULTICAP) {
				free(m->Scope.caps[i]->MultiCap.places);
				free(m->Scope.caps[i]);
			}
	return;
}

static inline void LTM_fail_MScope (Match* m) {
	free(m->Scope.child);
	LTM_destroy_MScope_MultiCaps(m);
	free(m->Scope.caps);
	m->type = NOMATCH;
	return;
}



static inline void LTM_start_MScope (Match* m, MStr_t str, Match* scope) {
	m->Scope.child = malloc(sizeof(Match));
	if (!m->Scope.child) die("Could not malloc m->Scope.child.\n");
	m->Scope.caps = calloc(  // Initialize these to NULL!
		(m->spec->Scope.ncaps + m->spec->Scope.nnamecaps),
		sizeof(Match*)
	);  // Make array of pointers, both to Caps and NameCaps.
	if (!m->Scope.caps) die("Could not malloc m->Scope.caps.\n");
	LTM_init_Match(m->Scope.child, m->spec->Scope.child, m->start);
	LTM_start(m->Scope.child, str, m);  // Replace the outer scope.
	if (m->Scope.child->type == NOMATCH) {
		DEBUGLOG6(" ## Not matching MScope at %08x\n", (int) m);
		return LTM_fail_MScope(m);
	}
	DEBUGLOG6(" ## Matching MScope at %08x\n", (int) m);
	m->end = m->Scope.child->end;
	return;
}

static inline void LTM_backtrack_MScope (Match* m, MStr_t str, Match* scope) {
	LTM_backtrack(m->Scope.child, str, m);  // in this scope
	if (m->Scope.child->type == NOMATCH) {
		DEBUGLOG6(" ## Not matching MScope at %08x\n", (int) m);
		return LTM_fail_MScope(m);
	}
	DEBUGLOG6(" ## Matching MScope at %08x\n", (int) m);
	m->end = m->Scope.child->end;
	return;
}

static inline void LTM_abort_MScope (Match* m, MStr_t str, Match* scope) {
	LTM_abort(m->Scope.child, str, m);
	free(m->Scope.child);
	return;
}

static inline int LTM_indexof_NameCap (MSpec* spec, MName_t name) {
	 // To be replaced with generic macro
	int i;
	for (i=0; i < spec->Scope.nnamecaps; i++) {
		if (MName_cmp(spec->Scope.names[i], name) == 0)
			return i;
	}
	return -1;
}

Match* LTM_lookup_Cap (const Match* m, int ind) {
	if (ind >= m->spec->Scope.ncaps)
		return NULL;
	return m->Scope.caps[ind];
}

struct MatchMultiCap LTM_lookup_all_Caps (const Match* m, int ind) {
	if (ind >= m->spec->Scope.ncaps) {
		struct MatchMultiCap r;
		r.type = MMULTICAP;
		r.nplaces = 0;
		return r;
	}
	if (m->Scope.caps[ind]->type == MMULTICAP)
		return m->Scope.caps[ind]->MultiCap;
	struct MatchMultiCap r;
	r.type = MMULTICAP;
	r.nplaces = 1;
	r.places = m->Scope.caps+ind;
	return r;
}

struct MatchMultiCap LTM_lookup_all_NameCaps (Match* m, MName_t name) {
	int ind = LTM_indexof_NameCap (m->spec, name);
	if (ind == -1 || m->Scope.caps[ind] == NULL) {
		struct MatchMultiCap r;
		r.type = MMULTICAP;
		r.nplaces = 0;
		return r;
	}
	if (m->Scope.caps[ind]->type == MMULTICAP)
		return m->Scope.caps[ind]->MultiCap;
	struct MatchMultiCap r;
	r.type = MMULTICAP;
	r.nplaces = 1;
	r.places = m->Scope.caps+ind;
	return r;
}

Match* LTM_lookup_NameCap (const Match* m, MName_t name) {
	 // To be replaced with generic macro
	int i;
	for (i=0; i < m->spec->Scope.nnamecaps; i++) {
		DEBUGLOG6("looking at name %d; it's at %d.\n", i, (int) m->Scope.caps[m->spec->Scope.ncaps+i]);
		if (strcmp(m->spec->Scope.names[i], name) == 0)
			return m->Scope.caps[m->spec->Scope.ncaps+i];
	}
	return NULL;
}

