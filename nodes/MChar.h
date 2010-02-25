

void inline LTM_start_MChar (Match* m, MStr_t str) {
	if (MStr_at(str, m->start) == m->spec->Char.c) {
		DEBUGLOG(" ## Matching MChar\n");
		m->end = m->start + 1;
		return;
	}
	DEBUGLOG(" ## Not Matching MChar\n");
	m->type = NOMATCH;
	return;
}





