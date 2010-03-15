
MSpec create_MChar (char c) {
	MSpec r;
	r.type   = MCHAR;
	r.Char.c = c;
	return r;
}

static inline void LTM_start_MChar (Match* m, MStr_t str, Match* scope) {
	if (MStr_at(str, m->start) == m->spec->Char.c) {
		DEBUGLOG7(" ## Matching MChar at %d\n", m->start + 1);
		m->end = m->start + 1;
		return;
	}
	DEBUGLOG7(" ## Not Matching MChar\n");
	m->type = NOMATCH;
	return;
}





