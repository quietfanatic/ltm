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

// Size of capture id.  Shorter uses less space, but limits number of captures in one scope to 65536.
#ifndef MCapID_t
#define MCapID_t uint16_t
#endif

// Our two main type here.
typedef union MSpec MSpec;
typedef union Match Match;

// Node types, to be used both for MSpec and for Match
enum MType {
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
	MREP,
	MSCOPE,
	MCAP,
	MNAMECAP,
	MMULTICAP,
	MREF,
	MCALL,
};
typedef uint8_t MType_t;

const char*const LTM_MType[] = {
	"NoMatch",
	"MNull",
	"MBegin",
	"MEnd",
	"MAny",
	"MChar",
	"MCharClass",
	"MAlt",
	"MGroup",
	"MOpt",
	"MRepMax",
	"MRepMin",
	"MScope",
	"MCap",
	"MNameCap",
	"MMultiCap",
	"MRef",
	"MCustom"
};


// Flags to be used for various things.  Most are NYI.

enum MFlags {
	MF_independent = 1,  // Don't descend on destruction (Possibly obsoleted by the MRef type)
	MF_minimal     = 2,  // Match as little as possible (for MRep, or possibly MGroup too)
	MF_nobacktrack = 4,  // Don't backtrack.  Should be set automatically on atomic nodes.
	MF_nocaps      = 8,  // Doesn't have captures.  With MF_nobacktrack, means result can be thrown away.
	MF_throwaway   = 12, // MF_nobacktrack and MF_nocaps.
};  // More to come later, including backtracking control.
typedef uint16_t MFlags_t;


// Utility structs

typedef struct MCharRange {
	MChar_t from;
	MChar_t to;
} MCharRange;

typedef struct MCustomSpec {
	void (* start ) (Match*, MStr_t, Match*);
	void (* backtrack ) (Match*, MStr_t, Match*);
	void (* abort ) (Match*, MStr_t, Match*);
	void (* finish ) (Match*);
	void (* destroy ) (Match*);
} MCustomSpec;


// MSpec structs

#define MSPEC_STRUCT_COMMON \
	MType_t type; \
	MFlags_t flags;

struct MSpecNoMatch { MSPEC_STRUCT_COMMON };
struct MSpecNull    { MSPEC_STRUCT_COMMON };
struct MSpecBegin   { MSPEC_STRUCT_COMMON };
struct MSpecEnd     { MSPEC_STRUCT_COMMON };
struct MSpecAny     { MSPEC_STRUCT_COMMON };
struct MSpecChar    { MSPEC_STRUCT_COMMON
	MChar_t c;
};
struct MSpecCharClass { MSPEC_STRUCT_COMMON
	int negative;
	size_t nranges;
	MCharRange* ranges;
};
struct MSpecAlt { MSPEC_STRUCT_COMMON
	size_t nalts;
	MSpec* alts;
};
struct MSpecGroup { MSPEC_STRUCT_COMMON
	size_t nelements;
	MSpec* elements;
};
struct MSpecOpt { MSPEC_STRUCT_COMMON
	MSpec* possible;
};
struct MSpecRep { MSPEC_STRUCT_COMMON
	MSpec* child;
	size_t min;
	size_t max;
};
struct MSpecScope { MSPEC_STRUCT_COMMON
	MSpec* child;
	MCapID_t ncaps;
	MCapID_t nnamecaps;
	char** names;
};
struct MSpecCap { MSPEC_STRUCT_COMMON
	MSpec* child;
	MCapID_t id;
};
struct MSpecNameCap { MSPEC_STRUCT_COMMON
	MSpec* child;
	MCapID_t id;
	char* name;
};
struct MSpecRef { MSPEC_STRUCT_COMMON
	MSpec* ref;
};
struct MSpecCustom { MSPEC_STRUCT_COMMON
	MCustomSpec* call;
};

union MSpec {
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
	struct MSpecRef Ref;
	struct MSpecCustom Custom;
};



// Match structs

#define MATCH_STRUCT_COMMON \
	MType_t type; \
	MFlags_t flags; \
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
	Match* matched;
};
struct MatchGroup { MATCH_STRUCT_COMMON
	size_t nelements;
	Match* elements;
};
struct MatchOpt { MATCH_STRUCT_COMMON
	Match* possible;
};
struct MatchRep { MATCH_STRUCT_COMMON
	size_t nmatches;
	Match** matches;  // Variable length array needs more indirection.
};
struct MatchScope { MATCH_STRUCT_COMMON
	Match* child;
	Match** caps;  // Numbered Caps first, then Named Caps.
};
struct MatchCap { MATCH_STRUCT_COMMON
	Match* child;
};
struct MatchNameCap { MATCH_STRUCT_COMMON
	Match* child;
};
struct MatchMultiCap { MATCH_STRUCT_COMMON
	size_t nplaces;
	Match** places;
};
struct MatchCustom { MATCH_STRUCT_COMMON
	void* data1;
	void* data2;
};

union Match {
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
	struct MatchCustom Custom;
};

#endif
