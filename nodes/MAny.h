
inline void LTM_start_MAny (Match* m, MStr_t str) {
	if (MStr_endat(str, m->start)) {
		DEBUGLOG(" ## Not matching MAny (end of string)\n");
		m->type = NOMATCH;
		return;
	}
	DEBUGLOG(" ## Matching MAny\n");
	m->end = m->start + 1;
	return;
}

