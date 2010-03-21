#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "match_types.h"
#define DEBUGLOG0(...)  //(fprintf(stderr, __VA_ARGS__))
#define DEBUGLOG1(...)  //(fprintf(stderr, __VA_ARGS__))
#define DEBUGLOG2(...)  //(fprintf(stderr, __VA_ARGS__))
#define DEBUGLOG3(...)  //(fprintf(stderr, __VA_ARGS__))
#define DEBUGLOG4(...)  //(fprintf(stderr, __VA_ARGS__))
#define DEBUGLOG5(...)  //(fprintf(stderr, __VA_ARGS__))
#define DEBUGLOG6(...)  //(fprintf(stderr, __VA_ARGS__))
#define DEBUGLOG7(...)  //(fprintf(stderr, __VA_ARGS__))
#define DEBUGLOG8(...)  //(fprintf(stderr, __VA_ARGS__))
#define DEBUGLOG9(...)  //(fprintf(stderr, __VA_ARGS__))
#define DEBUGLOG10(...) //(fprintf(stderr, __VA_ARGS__))


//#define free(...) // don't free



static inline void LTM_finish_MSpec(MSpec* spec, MSpec* scope);
static inline void destroy_MSpec(MSpec spec);
static inline void destroy_Match(Match m);
void LTM_start (Match* m, MStr_t str, Match* scope);
void LTM_backtrack (Match* m, MStr_t str, Match* scope);
void LTM_abort (Match* m, MStr_t str, Match* scope);



void die (char* mess) {
	fputs(mess, stderr);
	abort();
}

// For convenience
static inline void LTM_init_Match (Match* m, MSpec* spec, size_t start) {
	m->type  = spec->type;
	m->spec  = spec;
	m->start = start;
	return;
}

#include "nodes/MAny.h"
#include "nodes/MChar.h"
#include "nodes/MNull&co.h"
#include "nodes/MCharClass.h"
#include "nodes/MGroup.h"
#include "nodes/MOpt.h"
#include "nodes/MRep.h"
#include "nodes/MAlt.h"
#include "nodes/MScope.h"
#include "nodes/MCap&co.h"
#include "nodes/MRef.h"

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


Match LTM_match_at (MSpec* spec, MStr_t str, size_t start) {
	Match r;
	LTM_init_Match(&r, spec, start);
	LTM_start(&r, str, NULL);
	DEBUGLOG1((r.type == NOMATCH ? " # Pattern did not match \"%s\"\n" : " # Finished matching \"%s\"\n"), str);
	return r;
}




void LTM_start (Match* m, MStr_t str, Match* scope) {
	DEBUGLOG10(" ## About to start %s.\n", LTM_MType[m->type]);
	switch (m->type) {
		case NOMATCH:    return LTM_start_NoMatch(m, str, scope);
		case MNULL:      return LTM_start_MNull(m, str, scope);
		case MBEGIN:     return LTM_start_MBegin(m, str, scope);
		case MEND:       return LTM_start_MEnd(m, str, scope);
		case MANY:       return LTM_start_MAny(m, str, scope);
		case MCHAR:      return LTM_start_MChar(m, str, scope);
		case MCHARCLASS: return LTM_start_MCharClass(m, str, scope);
		case MGROUP:     return LTM_start_MGroup(m, str, scope);
		case MOPT:       return LTM_start_MOpt(m, str, scope);
		case MALT:       return LTM_start_MAlt(m, str, scope);
		case MREP:       return LTM_start_MRep(m, str, scope);
		case MSCOPE:     return LTM_start_MScope(m, str, scope);
		case MCAP:       return LTM_start_MCap(m, str, scope);
		case MNAMECAP:   return LTM_start_MNameCap(m, str, scope);
		case MREF:       return LTM_start_MRef(m, str, scope);
		default: {
			fprintf(stderr, "Error: Tried to match with unknown match type %d.\n", m->type);
			abort();
		}
	}
}


void LTM_backtrack (Match* m, MStr_t str, Match* scope) {
	DEBUGLOG10(" ## About to backtrack %s.\n", LTM_MType[m->type]);
	if (m->spec->flags&MF_nobacktrack) {
		DEBUGLOG8(" ## Backtracking past a node with MF_nobacktrack set");
		LTM_abort(m, str, scope);
		m->type = NOMATCH;
		return;
	}
	switch (m->type) {
		case NOMATCH:  return LTM_backtrack_NoMatch(m, str, scope);
		case MGROUP:   return LTM_backtrack_MGroup(m, str, scope);
		case MOPT:     return LTM_backtrack_MOpt(m, str, scope);
		case MALT:     return LTM_backtrack_MAlt(m, str, scope);
		case MREP:     return LTM_backtrack_MRep(m, str, scope);
		case MSCOPE:   return LTM_backtrack_MScope(m, str, scope);
		case MCAP:     return LTM_backtrack_MCap(m, str, scope);
		case MNAMECAP: return LTM_backtrack_MNameCap(m, str, scope);
		default: {
			DEBUGLOG8(" ## Backtracking past a token\n");
			m->type = NOMATCH;
			return;
		}
	}
}

void LTM_abort (Match* m, MStr_t str, Match* scope) {
	DEBUGLOG10(" ## Aborting %s.\n", LTM_MType[m->type]);
	switch (m->type) {
		case MGROUP:   return LTM_abort_MGroup(m, str, scope);
		case MOPT:     return LTM_abort_MOpt(m, str, scope);
		case MALT:     return LTM_abort_MAlt(m, str, scope);
		case MREP:     return LTM_abort_MRep(m, str, scope);
		case MSCOPE:   return LTM_abort_MScope(m, str, scope);
		case MCAP:     return LTM_abort_MCap(m, str, scope);
		case MNAMECAP: return LTM_abort_MNameCap(m, str, scope);
		default: return;
	}
}



static inline void LTM_destroy_MSpec (MSpec spec) {
	if (spec.flags&MF_independent)
		return;  // Don't destroy this.
	switch (spec.type) {
		case MCHARCLASS: return LTM_destroy_MSpecCharClass(spec);
		case MGROUP:     return LTM_destroy_MSpecGroup(spec);
		case MOPT:       return LTM_destroy_MSpecOpt(spec);
		case MALT:       return LTM_destroy_MSpecAlt(spec);
		case MREP:       return LTM_destroy_MSpecRep(spec);
		case MSCOPE:     return LTM_destroy_MSpecScope(spec);
		case MCAP:       return LTM_destroy_MSpecCap(spec);
		case MNAMECAP:   return LTM_destroy_MSpecNameCap(spec);
		default: return;
	}
}

void finish_MSpec (MSpec* spec) {
	LTM_finish_MSpec(spec, NULL);
}

static inline void LTM_finish_MSpec (MSpec* spec, MSpec* scope) {
	switch (spec->type) {
		case MCHARCLASS: return LTM_finish_MCharClass(spec, scope);
		case MGROUP:     return LTM_finish_MGroup(spec, scope);
		case MOPT:       return LTM_finish_MOpt(spec, scope);
		case MALT:       return LTM_finish_MAlt(spec, scope);
		case MREP:       return LTM_finish_MRep(spec, scope);
		case MSCOPE:     return LTM_finish_MScope(spec, scope);
		case MCAP:       return LTM_finish_MCap(spec, scope);
		case MNAMECAP:   return LTM_finish_MNameCap(spec, scope);
		default: return;
	}
}


void destroy_MSpec (MSpec spec) {
	spec.flags &=~ MF_independent;  // Remove don't-destroy flag
	return LTM_destroy_MSpec(spec);
}

void destroy_Match (Match m) {
	switch (m.type) {
		case MGROUP:    return LTM_destroy_MatchGroup(m);
		case MOPT:      return LTM_destroy_MatchOpt(m);
		case MALT:      return LTM_destroy_MatchAlt(m);
		case MREP:      return LTM_destroy_MatchRep(m);
		case MSCOPE:    return LTM_destroy_MatchScope(m);
		case MCAP:      return LTM_destroy_MatchCap(m);
		case MNAMECAP:  return LTM_destroy_MatchNameCap(m);
		default: return;
	}
}












