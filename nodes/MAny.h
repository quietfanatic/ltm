
MSpec create_MAny () {
	MSpec r;
	r.type = MANY;
	return r;
}

static inline void LTM_start_MAny (Match* m, MStr_t str, Match* scope) {
	if (MStr_endat(str, m->start)) {  // Fails only at end of string
		DEBUGLOG7(" ## Not matching MAny (end of string)\n");
		m->type = NOMATCH;
		return;
	}
	DEBUGLOG7(" ## Matching MAny\n");
	m->end = m->start + 1;
	return;
}

