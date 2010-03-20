MSpec create_MCustom (MFlags_t flags, MCustomSpec* calls) {
	MSpec r;
	r.type = MCUSTOM;
	r.flags = flags;
	r.call = calls;
	return r;
}

static inline void LTM_finish_MatchCustom (Match m) {
	return m->call->finish(m);
}

static inline void LTM_destroy_MatchCustom (Match m) {
	return m->call->destroy(m);
}

static inline void LTM_start_MCustom (Match* m, MStr_t str, Match* scope) {
	return m->call->start(m, str, scope);
}

static inline void LTM_backtrack_MCustom (Match* m, MStr_t str, Match* scope) {
	return m->call->backtrack(m, str, scope);
}

static inline void LTM_abort_MCustom (Match* m, MStr_t str, Match* scope) {
	return m->call->abort(m, str, scope);
}

