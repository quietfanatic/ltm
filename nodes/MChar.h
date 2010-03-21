
MSpec create_MChar (MFlags_t flags, char c) {
	MSpec r;
	r.type   = MCHAR;
	r.flags  = flags & MF_nobacktrack;
	r.Char.c = c;
	return r;
}

static inline void LTM_start_MChar (Match* m, MStr_t str, Match* scope) {
	MPos_t pos = m->start;
	char c;
	MStr_read(str, pos, c);
	if (c == m->spec->Char.c) {
		DEBUGLOG7(" ## Matching MChar at %d\n", MStr_after(str, m->start));
		m->end = pos;
		return;
	}
	DEBUGLOG7(" ## Not Matching MChar\n");
	m->type = NOMATCH;
	return;
}





