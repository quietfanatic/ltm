
MSpec create_MNull () {
	MSpec r;
	r.type = MNULL;
	return r;
}

MSpec create_MBegin () {
	MSpec r;
	r.type = MBEGIN;
	return r;
}

MSpec create_MEnd () {
	MSpec r;
	r.type = MEND;
	return r;
}


static inline void LTM_start_MNull (Match* m, MStr_t str, Match* scope) {
	DEBUGLOG7(" ## Matching MNull\n");
	m->end = m->start;
	return;
}

static inline void LTM_start_MBegin (Match* m, MStr_t str, Match* scope) {
	if (MStr_begin_at(str, m->start)) {
		DEBUGLOG7(" ## Matching MBegin\n");
		m->end = m->start;
		return;
	}
	DEBUGLOG7(" ## Not matching MBegin\n");
	m->type = NOMATCH;
	return;
}

static inline void LTM_start_MEnd (Match* m, MStr_t str, Match* scope) {
	if (MStr_end_at(str, m->start)) {
		DEBUGLOG7(" ## Matching MEnd\n");
		m->end = m->start;
		return;
	}
	DEBUGLOG7(" ## Not matching MEnd\n");
	m->type = NOMATCH;
	return;
}


// NoMatch

MSpec create_NoMatch () {
	MSpec r;
	r.type = NOMATCH;
	return r;
}

static inline void LTM_start_NoMatch (Match* m, MStr_t str, Match* scope) {
	return;
}

static inline void LTM_backtrack_NoMatch (Match* m, MStr_t str, Match* scope) {
	die("Error: Tried to backtrack into a failed match.\n");
}

