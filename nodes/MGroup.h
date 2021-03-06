
MSpec create_MGroup (MFlags_t flags, int nelements, ...) {
	MSpec r;
	r.type            = MGROUP;
	r.flags           = flags;
	r.Group.nelements = nelements;
	r.Group.elements  = malloc(nelements * sizeof(MSpec));
	if (!r.Group.elements) die("Couldn't malloc r.Group.elements.");
	va_list elements;
	va_start(elements, nelements);
	int i;
	for (i=0; i < nelements; i++)
		r.Group.elements[i] = va_arg(elements, MSpec);
	va_end(elements);
	return r;
}

static inline void LTM_finish_MGroup (MSpec* spec, MSpec* scope) {
	int i;
	for (i=0; i < spec->Group.nelements; i++)
		LTM_finish_MSpec(&spec->Group.elements[i], scope);
	return;  // Optimization stuff to go here
}

static inline void LTM_destroy_MSpecGroup (MSpec spec) {
	int i;
	for (i=0; i < spec.Group.nelements; i++)
		destroy_MSpec(spec.Group.elements[i]);
	free(spec.Group.elements);
	return;
}

static inline void LTM_destroy_MatchGroup (Match m) {
	int i;
	for (i=0; i < m.Group.nelements; i++) 
		destroy_Match(m.Group.elements[i]);
	free(m.Group.elements);
	return;
}



static inline void LTM_fail_MGroup (Match* m) {
	free(m->Group.elements);
	m->type = NOMATCH;
	return;
}

static inline void LTM_succeed_MGroup (Match* m) {
	m->end = m->Group.elements[m->spec->Group.nelements-1].end;
	return;
}


static inline void LTM_walk_MGroup (Match* m, MStr_t str, Match* scope, size_t i) {
	for (;;) {  // If element didn't match, backtrack left
		if (m->Group.elements[i].type == NOMATCH) {
			if (i == 0) {  // all the way left
				DEBUGLOG7(" ## Not matching MGroup (Out of tokens to backtrack)\n");
				return LTM_fail_MGroup(m);
			}
			i--;
			LTM_backtrack(&(m->Group.elements[i]), str, scope);
		}  // If element did match, advance right
		else {
			i++;
			if (i == m->spec->Group.nelements) {  // all the way right
				DEBUGLOG7(" ## Matching MGroup at %d\n", m->Group.elements[m->spec->Group.nelements-1].end);
				return LTM_succeed_MGroup(m);
			}
			LTM_init_Match(&m->Group.elements[i], &m->spec->Group.elements[i], m->Group.elements[i-1].end);
			LTM_start(&m->Group.elements[i], str, scope);
		}
	}
}

static inline void LTM_start_MGroup (Match* m, MStr_t str, Match* scope) {
	m->Group.nelements = m->spec->Group.nelements;
	if (m->spec->Group.nelements == 0) {  // Empty group
		DEBUGLOG7(" ## Matching MGroup (as null)\n");
		m->end = m->start;
		m->type = MNULL;
		return;
	}
	m->Group.elements = malloc(m->Group.nelements * sizeof(Match));
	if (!m->Group.elements) die("Could not malloc m.Group.elements");
	LTM_init_Match(&m->Group.elements[0], &m->spec->Group.elements[0], m->start);
	LTM_start(&m->Group.elements[0], str, scope);  // Prime first element
	LTM_walk_MGroup(m, str, scope, 0);
	return;
}

static inline void LTM_backtrack_MGroup (Match* m, MStr_t str, Match* scope) {
	if (m->spec->Group.nelements == 0) {  // Empty group
		m->type = NOMATCH;
		return;
	}
	LTM_backtrack(&m->Group.elements[m->spec->Group.nelements-1], str, scope);  // Backtrack one element
	LTM_walk_MGroup(m, str, scope, m->spec->Group.nelements-1);
	return;
}

static inline void LTM_abort_MGroup (Match* m, MStr_t str, Match* scope) {
	int i;  // Have to go through them backwards.
	for (i = m->Group.nelements-1; i >= 0; i--)
		LTM_abort(m->Group.elements+i, str, scope);
	free(m->Group.elements);
	return;
}

