
static inline void LTM_fail_MGroup (Match* m) {
	free(m->Group.elements);
	m->type = NOMATCH;
	return;
}

static inline void LTM_succeed_MGroup (Match* m) {
	m->end = m->Group.elements[m->spec->Group.nelements-1].end;
	return;
}


static inline void LTM_walk_MGroup (Match* m, MStr_t str, size_t i) {
	for (;;) {  // If element didn't match, backtrack left
		if (m->Group.elements[i].type == NOMATCH) {
			if (i == 0) {  // all the way left
				DEBUGLOG(" ## Not matching MGroup (Out of tokens to backtrack)\n");
				return LTM_fail_MGroup(m);
			}
			i--;
			backtrack(&(m->Group.elements[i]), str);
		}  // If element did match, advance right
		else {
			i++;
			if (i == m->spec->Group.nelements) {  // all the way right
				DEBUGLOG(" ## Matching MGroup\n");
				return LTM_succeed_MGroup(m);
			}
			LTM_init_Match(&m->Group.elements[i], &m->spec->Group.elements[i], m->Group.elements[i-1].end);
			LTM_start(&m->Group.elements[i], str);
		}
	}
}

inline void LTM_start_MGroup (Match* m, MStr_t str) {
	m->Group.nelements = m->spec->Group.nelements;
	if (m->spec->Group.nelements == 0) {  // Empty group
		DEBUGLOG(" ## Matching MGroup (as null)\n");
		m->end = m->start;
		m->type = MNULL;
		return;
	}
	m->Group.elements = malloc(m->Group.nelements * sizeof(Match));
	if (!m->Group.elements) die("Could not malloc m.Group.elements");
	LTM_init_Match(&m->Group.elements[0], &m->spec->Group.elements[0], m->start);
	LTM_start(&m->Group.elements[0], str);  // Prime first element
	LTM_walk_MGroup(m, str, 0);
	return;
}

inline void LTM_backtrack_MGroup (Match* m, MStr_t str) {
	if (m->spec->Group.nelements == 0) {  // Empty group
		m->type = NOMATCH;
		return;
	}
	backtrack(&m->Group.elements[m->spec->Group.nelements-1], str);  // Backtrack one element
	LTM_walk_MGroup(m, str, m->spec->Group.nelements-1);
	return;
}


