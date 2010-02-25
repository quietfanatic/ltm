

static inline void walk_group_from (Match* m, MStr_t str, size_t i) {
	for (;;) {
		if (m->Group.elements[i].type == NOMATCH) {
			if (MStr_beginat(str, i)) {
				DEBUGLOG(" ## Not matching MGroup (Out of tokens to backtrack)\n");
				free(m->Group.elements);
				m->type = NOMATCH;
				return;
			}
			i--;
			backtrack(&(m->Group.elements[i]), str);
		}
		else {
			i++;
			if (i == m->Group.nelements) {
				DEBUGLOG(" ## Matching MGroup\n");
				m->end = m->Group.elements[i-1].end;
				return;
			}
			m->Group.elements[i] = make_match(&m->spec->Group.elements[i], str, m->Group.elements[i-1].end);
		}
	}
}

inline void LTM_backtrack_MGroup (Match* m, MStr_t str) {
	if (m->Group.nelements == 0) {
		m->type = NOMATCH;
		return;
	}
	backtrack(&m->Group.elements[m->Group.nelements-1], str);
	walk_group_from(m, str, m->Group.nelements-1);
	return;
}

inline void LTM_start_MGroup (Match* m, MStr_t str) {
	m->Group.nelements = m->spec->Group.nelements;
	if (m->Group.nelements == 0) {
		DEBUGLOG(" ## Matching MGroup (empty group)\n");
		m->end = m->start;
		return;
	}
	m->Group.elements = malloc(m->Group.nelements * sizeof(Match));
	if (!m->Group.elements) die("Could not malloc m.Group.elements");
	m->Group.elements[0] = make_match(&m->spec->Group.elements[0], str, m->start);
	walk_group_from(m, str, 0);
	return;
}
