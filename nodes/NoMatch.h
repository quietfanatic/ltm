
static inline void LTM_start_NoMatch (Match* m, MStr_t str, Match* scope) {
	return;
}

static inline void LTM_backtrack_NoMatch (Match* m, MStr_t str, Match* scope) {
	die("Error: Tried to backtrack into a failed match.\n");
}


