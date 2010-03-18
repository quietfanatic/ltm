
MSpec create_MRef (MFlags_t flags, MSpec* ref) {
	MSpec r;
	r.type = MREF;
	r.flags = flags;
	r.Ref.ref = ref;
	return r;
}



static inline void LTM_start_MRef (Match* m, MStr_t str, Match* scope) {
	LTM_init_Match(m, m->spec->Ref.ref, m->start);
	DEBUGLOG10(" ## Ref type: %d\n", m->spec->Ref.ref->type);
	return LTM_start(m, str, scope);
}
