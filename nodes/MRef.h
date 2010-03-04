
MSpec create_MRef (MSpec* ref) {
	MSpec r;
	r.type = MREF;
	r.Ref.ref = ref;
	return r;
}



static inline void LTM_start_MRef (Match* m, MStr_t str, Match* scope) {
	LTM_init_Match(m, m->spec->Ref.ref, m->start);
	DEBUGLOG(" Ref type: %d\n", m->spec->Ref.ref->type);
	return LTM_start(m, str, scope);
}
