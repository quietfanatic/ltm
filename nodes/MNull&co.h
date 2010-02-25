
inline void LTM_start_MNull (Match* m, MStr_t str) {
	DEBUGLOG(" ## Matching MNull\n");
	m->end = m->start;
	return;
}

inline void LTM_start_MBegin (Match* m, MStr_t str) {
	if (MStr_beginat(str, m->start)) {
		DEBUGLOG(" ## Matching MBegin\n");
		m->end = m->start;
		return;
	}
	DEBUGLOG(" ## Not matching MBegin\n");
	m->type = NOMATCH;
	return;
}

inline void LTM_start_MEnd (Match* m, MStr_t str) {
	if (MStr_endat(str, m->start)) {
		DEBUGLOG(" ## Matching MEnd\n");
		m->end = m->start;
		return;
	}
	DEBUGLOG(" ## Not matching MEnd\n");
	m->type = NOMATCH;
	return;
}
