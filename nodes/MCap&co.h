
MSpec create_MCap (MFlags_t flags, MSpec child) {
	MSpec r;
	r.type = MCAP;
	r.flags = flags;
	r.Cap.child = malloc(sizeof(MSpec));
	*r.Cap.child = child;
	return r;
}

MSpec create_MNameCap (MFlags_t flags, char* name, MSpec child) {
	MSpec r;
	r.type = MNAMECAP;
	r.flags = flags;
	r.NameCap.child = malloc(sizeof(MSpec));
	*r.NameCap.child = child;
	r.NameCap.name = malloc(strlen(name));
	strcpy(r.NameCap.name, name);
	return r;
}

static inline void LTM_finish_MCap (MSpec* spec, MSpec* scope) {
	if (scope == NULL) die("MCap is not inside MScope.\n");
	spec->Cap.id = scope->Scope.ncaps++;
	LTM_finish_MSpec(spec->Cap.child, scope);
	return;
}

static inline void LTM_finish_MNameCap (MSpec* spec, MSpec* scope) {
	if (scope == NULL) die("MNameCap is not inside MScope.\n");
	int i = LTM_indexof_NameCap(scope, spec->NameCap.name);
	if (i != -1) {  // We can have multiple namecaps of the same name.
		spec->NameCap.id = i;
	}
	else {
		spec->NameCap.id = scope->Scope.nnamecaps++;
		scope->Scope.names = realloc(scope->Scope.names, scope->Scope.nnamecaps * sizeof(char*));
		scope->Scope.names[spec->NameCap.id] = spec->NameCap.name;
	}
	LTM_finish_MSpec(spec->NameCap.child, scope);
	return;
}

static inline void LTM_destroy_MSpecCap (MSpec spec) {
	destroy_MSpec(*spec.Cap.child);
	free(spec.Cap.child);
	return;
}

static inline void LTM_destroy_MSpecNameCap (MSpec spec) {
	destroy_MSpec(*spec.NameCap.child);
	free(spec.NameCap.child);
	free(spec.NameCap.name);
	return;
}

static inline void LTM_destroy_MatchCap (Match m) {
	destroy_Match(*m.Cap.child);
	free(m.Cap.child);
	return;
}

static inline void LTM_destroy_MatchNameCap (Match m) {
	destroy_Match(*m.NameCap.child);
	free(m.NameCap.child);
	return;
}



// Passing id to this means both MCap and MNameCap can use it.
static inline void LTM_link_MCap (Match* m, MCapID_t id, Match* scope) {
	DEBUGLOG4(" ## Linking capture id:%d at %08x to scope at %08x\n", id, (int) m, (int) scope);
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
		(multi->nplaces)++;
		multi->places = realloc(multi->places, multi->nplaces * sizeof(Match*));
		if (!multi->places) die("Could not malloc multi->places.");
		multi->places[multi->nplaces-1] = m;
		return;
	}
	 // Gotta create our own
	multi = malloc(sizeof(struct MatchMultiCap));
	if (!multi) die("Could not malloc multi.");
	multi->type = MMULTICAP;  // Important!
	multi->nplaces = 2;  // One for you, one for me
	multi->places = malloc(2 * sizeof(Match*));
	if (!multi->places) die("Could not malloc multi->places");
	multi->places[0] = scope->Scope.caps[id];
	multi->places[1] = m;
	 // Insert our multi
	scope->Scope.caps[id] = (Match*) multi;
	return;
}

static inline void LTM_unlink_MCap (Match* m, MCapID_t id, Match* scope) {
	DEBUGLOG4(" ## Unlinking capture id:%d at %08x from scope %08x\n", id, (int)m, (int)scope);
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
	DEBUGLOG5(" ### Multicap(?) at %08x is of type %d\n", (int) multi, multi->type);
	if (multi->type != MMULTICAP) die("Error: tried to unlink a capture, but a different one was linked in its place.\n");
	(multi->nplaces)--;
	if (multi->places[multi->nplaces] != m) {
		DEBUGLOG4(" ## Error info: found %08x but we're %08x.  Multi->places was:\n", (int) multi->places[multi->nplaces], (int) m);
		int i;
		for (i=0; i <= multi->nplaces; i++) {
			DEBUGLOG4(" ##  %d: %08x -> %d\n", i, (int) multi->places[i], (multi->places[i] != NULL) ? multi->places[i]->type : -1);
		};
		die("Error: tried to unlink a capture, but it wasn't the last of its ID.\n");
	}
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
		DEBUGLOG3(" ## Not matching MCap id:%d\n", m->spec->Cap.id);
		free(m->Cap.child);
		m->type = NOMATCH;
		return;
	}
	DEBUGLOG3(" ## Matching MCap id:%d\n", m->spec->Cap.id);
	 // Now set link to Scope
	LTM_link_MCap(m, m->spec->Cap.id, scope);
	m->end = m->Cap.child->end;
	return;
}

static inline void LTM_backtrack_MCap (Match* m, MStr_t str, Match* scope) {
	LTM_backtrack(m->Cap.child, str, scope);
	if (m->Cap.child->type == NOMATCH) {
		DEBUGLOG3(" ## Not matching MCap id:%d\n", m->spec->Cap.id);
		LTM_unlink_MCap(m, m->spec->Cap.id, scope);
		free(m->Cap.child);
		m->type = NOMATCH;
		return;
	}
	DEBUGLOG3(" ## Matching MCap id:%d\n", m->spec->Cap.id);
	m->end = m->Cap.child->end;
	return;
}

static inline void LTM_abort_MCap (Match* m, MStr_t str, Match* scope) {
	LTM_abort(m->Cap.child, str, scope);
	LTM_unlink_MCap(m, m->spec->Cap.id, scope);
	free(m->Cap.child);
	return;
}

static inline void LTM_start_MNameCap (Match* m, MStr_t str, Match* scope) {
	m->NameCap.child = malloc(sizeof(Match));
	if (!m->NameCap.child) die("Could not malloc m->NameCap.child.\n");
	LTM_init_Match(m->NameCap.child, m->spec->NameCap.child, m->start);
	LTM_start(m->NameCap.child, str, scope);
	if (m->NameCap.child->type == NOMATCH) {
		DEBUGLOG3(" ## Not matching MNameCap id:%d name:\"%s\"\n", m->spec->NameCap.id, m->spec->NameCap.name);
		free(m->Cap.child);
		m->type = NOMATCH;
		return;
	}
	DEBUGLOG3(" ## Matching MNameCap id:%d name:\"%s\"\n", m->spec->NameCap.id, m->spec->NameCap.name);
	LTM_link_MCap(m, m->spec->NameCap.id + scope->spec->Scope.ncaps, scope);
	m->end = m->NameCap.child->end;
	return;
}

static inline void LTM_backtrack_MNameCap (Match* m, MStr_t str, Match* scope) {
	LTM_backtrack(m->NameCap.child, str, scope);
	if (m->NameCap.child->type == NOMATCH) {
		DEBUGLOG3(" ## Not matching MNameCap id:%d name:\"%s\"\n", m->spec->NameCap.id, m->spec->NameCap.name);
		LTM_unlink_MCap(m, m->spec->NameCap.id + scope->spec->Scope.ncaps, scope);
		free(m->NameCap.child);
		m->type = NOMATCH;
		return;
	}
	DEBUGLOG3(" ## Matching MNameCap id:%d name:\"%s\"\n", m->spec->NameCap.id, m->spec->NameCap.name);
	m->end = m->NameCap.child->end;
	return;
}

static inline void LTM_abort_MNameCap (Match* m, MStr_t str, Match* scope) {
	LTM_abort(m->Cap.child, str, scope);
	LTM_unlink_MCap(m, m->spec->NameCap.id + scope->spec->Scope.nnamecaps, scope);
	free(m->NameCap.child);
	return;
}



















