
MCharRange create_MCharRange (MChar_t from, MChar_t to) {
	MCharRange r;
	r.from = from;
	r.to   = to;
	return r;
}

MSpec create_MCharClass (int negative, int nranges, ...) {
	va_list ranges;
	va_start(ranges, nranges);
	MSpec r;
	r.type               = MCHARCLASS;
	r.CharClass.negative = negative;
	r.CharClass.nranges  = nranges;
	r.CharClass.ranges   = malloc(nranges * sizeof(struct MCharRange));
	if (!r.CharClass.ranges) die("Couldn't malloc r.CharClass.ranges.");
	int i;
	for (i=0; i < nranges; i++)
		r.CharClass.ranges[i] = va_arg(ranges, MCharRange);
	va_end(ranges);
	return r;
}

MSpec create_MCharClass_s (int negative, int nranges, char* ranges) {
	MSpec r;
	r.type               = MCHARCLASS;
	r.CharClass.negative = negative;
	r.CharClass.nranges  = nranges;
	r.CharClass.ranges   = malloc(nranges * sizeof(struct MCharRange));
	if (!r.CharClass.ranges) die("Couldn't malloc r.CharClass.ranges.");
	memcpy(r.CharClass.ranges, ranges, nranges * sizeof(struct MCharRange));
	return r;
}

static inline void LTM_destroy_MSpecCharClass (MSpec spec) {
	free(spec.CharClass.ranges);
	return;
}

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


