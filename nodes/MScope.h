static inline void LTM_destroy_MScope_MultiCaps (Match* m) {
	int i;
	for (i=0; i < m->spec->Scope.ncaps + m->spec->Scope.nnamecaps; i++) {
		if (m->Scope.caps[i]->type == MMULTICAP)
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
	m->Scope.caps = malloc(
		  (m->spec->Scope.ncaps + m->spec->Scope.nnamecaps)
		* sizeof(Match*)
	);  // Make array of pointers, both to Caps and NameCaps.
	if (!m->Scope.caps) die("Could not malloc m->Scope.caps.\n");
	LTM_init_Match(m->Scope.child, m->spec->Scope.child, m->start);
	LTM_start(m->Scope.child, str, m);  // Replace the outer scope.
	if (m->Scope.child->type == NOMATCH) {
		DEBUGLOG(" ## Not matching MScope\n");
		return LTM_fail_MScope(m);
	}
	DEBUGLOG(" ## Matching MScope\n");
	m->start = m->Scope.child->end;
	return;
}

static inline void LTM_backtrack_MScope (Match* m, MStr_t str, Match* scope) {
	LTM_backtrack(m->Scope.child, str, m);  // in this scope
	if (m->Scope.child->type == NOMATCH) {
		DEBUGLOG(" ## Not matching MScope\n");
		return LTM_fail_MScope(m);
	}
	DEBUGLOG(" ## Matching MScope\n");
	m->start = m->Scope.child->end;
	return;
}

