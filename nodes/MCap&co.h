













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
	if (scope->Scope.caps[m->spec->Cap.id]) {
		struct MatchMultiCap* multi;
		 // Already got one of me
		if (scope->Scope.caps[m->spec->Cap.id]->type == MMULTICAP) {
			 // And it's already a MultiCap
			multi = (struct MatchMultiCap*) scope->Scope.caps[m->spec->Cap.id];
			multi->nplaces++;
			multi->places = realloc(multi->places, multi->nplaces * sizeof(Match*));
			if (!multi->places) die("Could not malloc multi.places.");
			multi->places[multi->nplaces-1] = m;
		}
		else {
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
		}
	}
	else {  // No link yet
		scope->Scope.caps[m->spec->Cap.id] = m;
	}
	 // done
	m->end = m->Scope.child->end;
	return;
}






















