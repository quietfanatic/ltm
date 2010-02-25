#include <stdlib.h>
#include <stdio.h>
#include "match_types.h"
#define DEBUGLOG(x) //(fprintf(stderr, x))


void backtrack (Match* m, MStr_t str);
void LTM_init_Match(Match* m, MSpec* spec, size_t start);
void LTM_start (Match* m, MStr_t str);

void die (char* mess) {
	fputs(mess, stderr);
	abort();
}


#include "nodes/NoMatch.h"
#include "nodes/MAny.h"
#include "nodes/MChar.h"
#include "nodes/MNull&co.h"
#include "nodes/MCharClass.h"
#include "nodes/MGroup.h"
#include "nodes/MOpt.h"
#include "nodes/MRep.h"
#include "nodes/MAlt.h"

// On match success:
//     DEBUGLOG()
//     make sure .nmatches (or such) is set correctly
//     set .end
//     return
// On match failure:
//     DEBUGLOG()
//     free any used memory
//         destroy_match() need not be called, as all inner nodes will already be NOMATCH.
//     set .type to NOMATCH
//     return



void backtrack (Match* m, MStr_t str) {
	switch (m->type) {
		case NOMATCH: return LTM_backtrack_NoMatch(m, str);
		case MGROUP: return LTM_backtrack_MGroup(m, str);
		case MOPT: return LTM_backtrack_MOpt(m, str);
		case MALT: return LTM_backtrack_MAlt(m, str);
		case MREPMAX: return LTM_backtrack_MRepMax(m, str);
		default: {
			DEBUGLOG(" ## Backtracking past a token\n");
			m->type = NOMATCH;
			return;
		}
	}
}



inline void LTM_init_Match (Match* m, MSpec* spec, size_t start) {
	m->type  = spec->type;
	m->spec  = spec;
	m->start = start;
	return;
}

void LTM_start (Match* m, MStr_t str) {
	switch (m->type) {
		case NOMATCH:    return LTM_start_NoMatch(m, str);
		case MNULL:      return LTM_start_MNull(m, str);
		case MBEGIN:     return LTM_start_MBegin(m, str);
		case MEND:       return LTM_start_MEnd(m, str);
		case MANY:       return LTM_start_MAny(m, str);
		case MCHAR:      return LTM_start_MChar(m, str);
		case MCHARCLASS: return LTM_start_MCharClass(m, str);
		case MGROUP:     return LTM_start_MGroup(m, str);
		case MOPT:       return LTM_start_MOpt(m, str);
		case MALT:       return LTM_start_MAlt(m, str);
		case MREPMAX:    return LTM_start_MRepMax(m, str);
		default: {
			fprintf(stderr, "Error: Tried to associate unknown match type %d.\n", m->type);
			abort();
		}
	}
}





