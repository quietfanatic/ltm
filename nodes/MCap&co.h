





// Passing id to this means both MCap and MNameCap can use it.
static inline void LTM_link_MCap (Match* m, MCapID_t id, Match* scope) {
	if (!scope->Scope.caps[id]) {
		// No link yet
		scope->Scope.caps[id] = m;
		return;
	}
	 // Already got one of me
	struct MatchMultiCap* multi;
	if (scope->Scope.caps[id]->type == MMULTICAP) {
		 // And it's already a MultiCap
		multi = (struct MatchMultiCap*) scope->Scope.caps[id];
		multi->nplaces++;
		multi->places = realloc(multi->places, multi->nplaces * sizeof(Match*));
		if (!multi->places) die("Could not malloc multi.places.");
		multi->places[multi->nplaces-1] = m;
		return;
	}
	 // Gotta create our own
	multi = malloc(sizeof(struct MatchMultiCap));
	if (!multi) die("Could not malloc multi.");
	multi->nplaces = 2;  // One for you, one for me
	multi->places = malloc(2 * sizeof(Match*));
	if (!multi->places) die("Could not malloc multi.");
	multi->places[0] = scope->Scope.caps[id];
	multi->places[1] = m;
	 // Insert our multi
	scope->Scope.caps[id] = (Match*) multi;
	return;
}

static inline void LTM_unlink_MCap (Match* m, MCapID_t id, Match* scope) {
	if (!scope->Scope.caps[id])
		die("Error: tried to unlink a non-linked capture.\n");
	if (scope->Scope.caps[id] == m) {
		scope->Scope.caps[id] = NULL;
		return;  // Real simple
	}
	 // Oh no it's a MultiCap
	 // We ought to be the latest entry in it.
	struct MatchMultiCap* multi;
	multi = (struct MatchMultiCap*) scope->Scope.caps[id];
	if (multi->type != MMULTICAP) die("Error: tried to unlink a capture, but a different one was linked in its place.\n");
	multi->nplaces--;
	if (multi->places[multi->nplaces] != m)
		die("Error: tried to unlink a capture, but it wasn't the last of its ID.\n");
	if (multi->nplaces == 0) {  // We're the last one left.
		scope->Scope.caps[id] = NULL;
		free(multi);
		return;
	}
	multi->places[multi->nplaces] = NULL;
	return;
}




static inline void LTM_start_MCap (Match* m, MStr_t str, Match* scope) {
	m->Cap.child = malloc(sizeof(Match));
	if (!m->Cap.child) die("Could not malloc m->Cap.child.");
	LTM_init_Match(m->Cap.child, m->spec->Cap.child, m->start);
	LTM_start(m->Cap.child, str, scope);
	if (m->Cap.child->type == NOMATCH) {
		DEBUGLOG(" ## Not matching MCap\n");
		free(m->Cap.child);
		m->type = NOMATCH;
		return;
	}
	DEBUGLOG(" ## Matching MCap\n");
	 // Now set link to Scope
	LTM_link_MCap(m, m->spec->Cap.id, scope);
	m->end = m->Cap.child->end;
	return;
}

static inline void LTM_backtrack_MCap (Match* m, MStr_t str, Match* scope) {
	LTM_backtrack(m->Cap.child, str, scope);
	if (m->Cap.child->type == NOMATCH) {
		DEBUGLOG(" ## Not matching MCap\n");
		LTM_unlink_MCap(m, m->spec->Cap.id, scope);
		free(m->Cap.child);
		m->type = NOMATCH;
		return;
	}
	DEBUGLOG(" ## Matching MCap\n");
	m->end = m->Cap.child->end;
	return;
}

static inline void LTM_start_MNameCap (Match* m, MStr_t str, Match* scope) {
	m->NameCap.child = malloc(sizeof(Match));
	if (!m->NameCap.child) die("Could not malloc m->NameCap.child.\n");
	LTM_init_Match(m->NameCap.child, m->spec->NameCap.child, m->start);
	LTM_start(m->NameCap.child, str, scope);
	if (m->NameCap.child->type == NOMATCH) {
		DEBUGLOG(" ## Not matching MNAMECAP\n");
		free(m->Cap.child);
		m->type = NOMATCH;
		return;
	}
	DEBUGLOG(" ## Matching MNameCap\n");
	LTM_link_MCap(m, m->spec->NameCap.id + scope->spec->Scope.ncaps, scope);
	m->end = m->NameCap.child->end;
	return;
}

static inline void LTM_backtrack_MNameCap (Match* m, MStr_t str, Match* scope) {
	LTM_backtrack(m->NameCap.child, str, scope);
	if (m->NameCap.child->type == NOMATCH) {
		DEBUGLOG(" ## Not matching MNameCap\n");
		LTM_unlink_MCap(m, m->spec->NameCap.id + scope->spec->Scope.ncaps, scope);
		free(m->NameCap.child);
		m->type = NOMATCH;
		return;
	}
	DEBUGLOG(" ## Matching MCap\n");
	m->end = m->NameCap.child->end;
	return;
}



















