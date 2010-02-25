
inline void LTM_start_NoMatch (Match* m, MStr_t str) {
	return;
}

inline void LTM_backtrack_NoMatch (Match* m, MStr_t str) {
	die("Error: Tried to backtrack into a failed match.\n");
}


