#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>


#ifndef HAVE_MATCH_TYPES
#define HAVE_MATCH_TYPES

// Size of one character
#ifndef MChar_t
#define MChar_t char
#endif

// What kind of string to operate on and how to get characters from it.
#ifndef MStr_t
#define MStr_t MChar_t* // default: zero-padded string of MChar_t
#define MStr_at(str, i) ((str)[i])
#define MStr_beginat(str, i) ((i) == 0)
#define MStr_endat(str, i) ((str)[i] == 0)
#endif

// Size of capture id.  Shorter uses less space, but limits number of captures in one scope.
#ifndef MCapID_t
#define MCapID_t uint16_t
#endif

// Size of flags
#ifndef MFlags_t
#define MFlags_t uint16_t
#endif

// Node types, to be used both for MSpec and for Match
enum Match_type {
	NOMATCH,
	MNULL,
	MBEGIN,
	MEND,
	MANY,
	MCHAR,
	MCHARCLASS,
	MALT,
	MGROUP,
	MOPT,
	MREPMAX,
	MREPMIN,
	MSCOPE,
	MCAP,
	MNAMECAP,
	MMULTICAP,
};
typedef uint8_t Match_type;



// Utility structs

typedef struct MCharRange {
	MChar_t from;
	MChar_t to;
} MCharRange;



// MSpec structs

#define MSPEC_STRUCT_COMMON \
	Match_type type; \
	MFlags_t flag;

struct MSpecNoMatch { MSPEC_STRUCT_COMMON };
struct MSpecNull    { MSPEC_STRUCT_COMMON };
struct MSpecBegin   { MSPEC_STRUCT_COMMON };
struct MSpecEnd     { MSPEC_STRUCT_COMMON };
struct MSpecAny     { MSPEC_STRUCT_COMMON };
struct MSpecChar    { MSPEC_STRUCT_COMMON
	MChar_t c;
};
struct MSpecCharClass { MSPEC_STRUCT_COMMON
	size_t nranges;
	struct MCharRange* ranges;
};
struct MSpecAlt { MSPEC_STRUCT_COMMON
	size_t nalts;
	struct MSpec* alts;
};
struct MSpecGroup { MSPEC_STRUCT_COMMON
	size_t nelements;
	struct MSpec* elements;
};
struct MSpecOpt { MSPEC_STRUCT_COMMON
	struct MSpec* possible;
};
struct MSpecRep { MSPEC_STRUCT_COMMON
	struct MSpec* child;
	size_t min;
	size_t max;
};
struct MSpecScope { MSPEC_STRUCT_COMMON
	struct MSpec* child;
	MCapID_t ncaps;
	MCapID_t nnamecaps;
	char** names;
};
struct MSpecCap { MSPEC_STRUCT_COMMON
	struct MSpec* child;
	MCapID_t id;
};
struct MSpecNameCap { MSPEC_STRUCT_COMMON
	struct MSpec* child;
	MCapID_t id;
	char* name;
};

typedef struct MSpec {
	union {
		struct { MSPEC_STRUCT_COMMON };
		struct MSpecNoMatch No;
		struct MSpecNull Null;
		struct MSpecBegin Begin;
		struct MSpecEnd End;
		struct MSpecAny Any;
		struct MSpecChar Char;
		struct MSpecCharClass CharClass;
		struct MSpecAlt Alt;
		struct MSpecGroup Group;
		struct MSpecOpt Opt;
		struct MSpecRep Rep;
		struct MSpecScope Scope;
		struct MSpecCap Cap;
		struct MSpecNameCap NameCap;
	};
} MSpec;



// Match structs

#define MATCH_STRUCT_COMMON \
	Match_type type; \
	MSpec* spec; \
	size_t start; \
	size_t end;


struct NoMatch { MATCH_STRUCT_COMMON };
struct MatchNull { MATCH_STRUCT_COMMON };
struct MatchBegin { MATCH_STRUCT_COMMON };
struct MatchEnd { MATCH_STRUCT_COMMON };
struct MatchAny { MATCH_STRUCT_COMMON };
struct MatchChar { MATCH_STRUCT_COMMON };
struct MatchCharClass { MATCH_STRUCT_COMMON };
struct MatchAlt { MATCH_STRUCT_COMMON
	size_t alti;
	struct Match* matched;
};
struct MatchGroup { MATCH_STRUCT_COMMON
	size_t nelements;
	struct Match* elements;
};
struct MatchOpt { MATCH_STRUCT_COMMON
	struct Match* possible;
};
struct MatchRep { MATCH_STRUCT_COMMON
	size_t nmatches;
	struct Match* matches;
};
struct MatchScope { MATCH_STRUCT_COMMON
	struct Match* child;
	struct Match** caps; // Numbered Caps first, then Named Caps.
};
struct MatchCap { MATCH_STRUCT_COMMON
	struct Match* child;
};
struct MatchNameCap { MATCH_STRUCT_COMMON
	struct Match* child;
};
struct MatchMultiCap { MATCH_STRUCT_COMMON
	size_t nplaces;
	struct Match** places;
};

typedef struct Match {
	union {
		struct { MATCH_STRUCT_COMMON };
		struct NoMatch No;
		struct MatchNull Null;
		struct MatchBegin Begin;
		struct MatchEnd End;
		struct MatchAny Any;
		struct MatchChar Char;
		struct MatchCharClass CharClass;
		struct MatchAlt Alt;
		struct MatchGroup Group;
		struct MatchOpt Opt;
		struct MatchRep Rep;
		struct MatchScope Scope;
		struct MatchCap Cap;
		struct MatchNameCap NameCap;
		struct MatchMultiCap MultiCap;
	};
} Match;


// Construction/destruction of types

MSpec create_NoMatch () {
	MSpec r;
	r.type = NOMATCH;
	return r;
}

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

MSpec create_MAny () {
	MSpec r;
	r.type = MANY;
	return r;
}

MSpec create_MChar (char c) {
	MSpec r;
	r.type   = MCHAR;
	r.Char.c = c;
	return r;
}

MSpec create_MCharClass (int nranges, char* ranges) {
	MSpec r;
	r.type              = MCHARCLASS;
	r.CharClass.nranges = nranges;
	r.CharClass.ranges  = malloc(nranges * sizeof(struct MCharRange));
	memcpy(r.CharClass.ranges, ranges, nranges * sizeof(struct MCharRange));
	return r;
}

MSpec create_MGroup (int nelements, ...) {
	MSpec r;
	r.type            = MGROUP;
	r.Group.nelements = nelements;
	r.Group.elements  = malloc(nelements * sizeof(MSpec));
	va_list elements;
	va_start(elements, nelements);
	int i;
	for (i=0; i < nelements; i++)
		r.Group.elements[i] = va_arg(elements, MSpec);
	va_end(elements);
	return r;
}

MSpec create_MOpt (MSpec possible) {
	MSpec r;
	r.type          = MOPT;
	r.Opt.possible  = malloc(sizeof(MSpec));
	*r.Opt.possible = possible;
	return r;
}

MSpec create_MAlt (int nalts, ...) {
	MSpec r;
	r.type      = MALT;
	r.Alt.nalts = nalts;
	r.Alt.alts  = malloc(nalts * sizeof(MSpec));
	va_list alts;
	va_start (alts, nalts);
	int i;
	for (i=0; i < nalts; i++)
		r.Alt.alts[i] = va_arg(alts, MSpec);
	va_end(alts);
	return r;
}

MSpec create_MRepMax (MSpec child, size_t min, size_t max) {
	MSpec r;
	r.type       = MREPMAX;
	r.Rep.child  = malloc(sizeof(MSpec));
	*r.Rep.child = child;
	r.Rep.min    = min;
	r.Rep.max    = max;
	return r;
}


void destroy_mspec (MSpec spec) {
	switch (spec.type) {
		case MCHARCLASS: {
			free(spec.CharClass.ranges);
			return;
		}
		case MGROUP: {
			int i;
			for (i=0; i < spec.Group.nelements; i++)
				destroy_mspec(spec.Group.elements[i]);
			free(spec.Group.elements);
			return;
		}
		case MOPT: {
			destroy_mspec(*spec.Opt.possible);
			free(spec.Opt.possible);
			return;
		}
		case MALT: {
			int i;
			for (i=0; i < spec.Alt.nalts; i++)
				destroy_mspec(spec.Alt.alts[i]);
			free(spec.Alt.alts);
			return;
		}
		case MREPMAX: case MREPMIN: {
			destroy_mspec(*spec.Rep.child);
			free(spec.Rep.child);
			return;
		}
		default: return;
	}
}

void destroy_match (Match m) {
	switch (m.type) {
		case MGROUP: {
			int i;
			for (i=0; i < m.Group.nelements; i++) 
				destroy_match(m.Group.elements[i]);
			free(m.Group.elements);
			return;
		}
		case MOPT: {
			destroy_match(*m.Opt.possible);
			free(m.Opt.possible);
			return;
		}
		case MALT: {
			destroy_match(*m.Alt.matched);
			free(m.Alt.matched);
			return;
		}
		case MREPMAX: case MREPMIN: {
			int i;
			for (i=0; i < m.Rep.nmatches; i++)
				destroy_match(m.Rep.matches[i]);
			free(m.Rep.matches);
			return;
		}
		default: return;
	}
}










#endif
