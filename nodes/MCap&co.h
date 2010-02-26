






static inline void LTM_link_MCap (Match* m, Match* scope) {
	if (!scope->Scope.caps[m->spec->Cap.id]) {
		// No link yet
		scope->Scope.caps[m->spec->Cap.id] = m;
		return;
	}
	 // Already got one of me
	struct MatchMultiCap* multi;
	if (scope->Scope.caps[m->spec->Cap.id]->type == MMULTICAP) {
		 // And it's already a MultiCap
		multi = (struct MatchMultiCap*) scope->Scope.caps[m->spec->Cap.id];
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
	multi->places[0] = scope->Scope.caps[m->spec->Cap.id];
	multi->places[1] = m;
	 // Insert our multi
	scope->Scope.caps[m->spec->Cap.id] = (Match*) multi;
	return;
}

static inline void LTM_unlink_MCap (Match* m, Match* scope) {
	if (!scope->Scope.caps[m->spec->Cap.id])
		die("Error: tried to unlink a non-linked capture.\n");
	if (scope->Scope.caps[m->spec->Cap.id] == m) {
		scope->Scope.caps[m->spec->Cap.id] = NULL;
		return;  // Real simple
	}
	 // Oh no it's a MultiCap
	 // We ought to be the latest entry in it.
	struct MatchMultiCap* multi;
	multi = (struct MatchMultiCap*) scope->Scope.caps[m->spec->Cap.id];
	if (multi->type != MMULTICAP) die("Error: tried to unlink a capture, but a different one was linked in its place.\n");
	multi->nplaces--;
	if (multi->places[multi->nplaces] != m)
		die("Error: tried to unlink a capture, but it wasn't the last of its ID.\n");
	if (multi->nplaces == 0) {  // We're the last one left.
		scope->Scope.caps[m->spec->Cap.id] = NULL;
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
	LTM_start(m->Scope.child, str, scope);
	if (m->Cap.child->type == NOMATCH) {
		DEBUGLOG(" ## Not matching MCap\n");
		free(m->Cap.child);
		m->type = NOMATCH;
		return;
	}
	DEBUGLOG(" ## Matching MCap\n");
	 // Now set link to Scope
	LTM_link_MCap(m, scope);
	m->end = m->Scope.child->end;
	return;
}

static inline void LTM_backtrack_MCap (Match* m, MStr_t str, Match* scope) {
	LTM_backtrack(m->Cap.child, str, scope);
	if (m->Cap.child->type == NOMATCH) {
		DEBUGLOG(" ## Not matching MCap\n");
		LTM_unlink_MCap(m, scope);
		free(m->Cap.child);
		m->type = NOMATCH;
		return;
	}
	DEBUGLOG(" ## Matching MCap\n");
	m->end = m->Cap.child->end;
	return;
}





















