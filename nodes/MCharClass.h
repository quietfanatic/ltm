
inline void LTM_start_MCharClass (Match* m, MStr_t str) {
	int i;
	for (i=0; i < m->spec->CharClass.nranges; i++) {
		if (MStr_at(str, m->start) < m->spec->CharClass.ranges[i].from) {
			DEBUGLOG(" ## Not matching MCharClass\n");
			m->type = NOMATCH;
			return;
		}
		if (MStr_at(str, m->start) <= m->spec->CharClass.ranges[i].to) {
			DEBUGLOG(" ## Matching MCharClass\n");
			m->end = m->start + 1;
			return;
		}
	}
	DEBUGLOG(" ## Not matching MCharClass\n");
	m->type = NOMATCH;
	return;
}


