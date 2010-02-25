#include <stddef.h>
#include <stdint.h>

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
	struct MSpec* child;
	struct MSpec* caps; // Numbered Caps first, then Named Caps.
};
struct MatchCap { MATCH_STRUCT_COMMON
	struct MSpec* child;
};
struct MatchNameCap { MATCH_STRUCT_COMMON
	struct MSpec* child;
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
	};
} Match;

#endif
