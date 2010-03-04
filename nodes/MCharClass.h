

static inline void LTM_start_MCharClass (Match* m, MStr_t str, Match* scope) {
	int i;  // Ranges must be sorted to work.
	if (MStr_endat(str, m->start)) {
		DEBUGLOG(" ## Not matching MCharClass (end of string)");
		m->type = NOMATCH;
		return;
	}
	for (i=0; i < m->spec->CharClass.nranges; i++) {
		if (MStr_at(str, m->start) < m->spec->CharClass.ranges[i].from) {
			if (m->spec->CharClass.negative) {
				DEBUGLOG(" ## Matching negated MCharClass at %d\n", m->start + 1);
				m->end = m->start + 1;
				return;
			}		
			DEBUGLOG(" ## Not matching MCharClass\n");
			m->type = NOMATCH;
			return;  // Not in range
		}
		if (MStr_at(str, m->start) <= m->spec->CharClass.ranges[i].to) {
			if (m->spec->CharClass.negative) {
				DEBUGLOG(" ## Not matching negated MCharClass\n");
				m->type = NOMATCH;
				return;
			}
			DEBUGLOG(" ## Matching MCharClass at %d\n", m->start + 1);
			m->end = m->start + 1;
			return;  // In range
		}
	}
	if (m->spec->CharClass.negative) {
		DEBUGLOG(" ## Matching negated MCharClass at %d\n", m->start + 1);
		m->end = m->start + 1;
		return;
	}
	DEBUGLOG(" ## Not matching MCharClass\n");
	m->type = NOMATCH;
	return;  // Out of ranges
}


