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
#define MStr_endat(str, i) ((str)[i] == 0)
#endif

// Size of capture id.  Shorter uses less space, but limits number of captures in one scope.
#ifndef MCapID_t
#define MCapID_t uint16_t
#endif



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


typedef struct MSpec {
	Match_type type;
	union {
		struct MSpecNoMatch {} No;
		struct MSpecNull {} Null;
		struct MSpecBegin {} Begin;
		struct MSpecEnd {} End;
		struct MSpecAny {} Any;
		struct MSpecChar {
			MChar_t c;
		} Char;
		struct MSpecCharClass {
			size_t nranges;
			struct MSpecCharRange {
				MChar_t from;
				MChar_t to;
			}* ranges;
		} CharClass;
		struct MSpecAlt {
			size_t nalts;
			struct MSpec* alts;
		} Alt;
		struct MSpecGroup {
			size_t nelements;
			struct MSpec* elements;
		} Group;
		struct MSpecOpt {
			struct MSpec* possible;
		} Opt;
		struct MSpecRep {
			struct MSpec* child;
			size_t min;
			size_t max;
		} Rep;
		struct MSpecScope {
			struct MSpec* child;
			MCapID_t ncaps;
			MCapID_t nnamecaps;
			char** names;
		} Scope;
		struct MSpecCap {
			struct MSpec* child;
			MCapID_t id;
		} Cap;
		struct MSpecNameCap {
			struct MSpec* child;
			char* name;
			MCapID_t id;
		} NameCap;
	};
} MSpec;

typedef struct Match {
	Match_type type;
	MSpec* spec;
	size_t start;
	size_t end;
	union {
		struct NoMatch {} No;
		struct MatchNull {} Null;
		struct MatchBegin {} Begin;
		struct MatchEnd {} End;
		struct MatchAny {} Any;
		struct MatchChar {} Char;
		struct MatchCharClass {} CharClass;
		struct MatchAlt {
			size_t alti;
			struct Match* matched;
		} Alt;
		struct MatchGroup {
			size_t nelements;
			struct Match* elements;
		} Group;
		struct MatchOpt {
			struct Match* possible;
		} Opt;
		struct MatchRep {
			size_t nmatches;
			struct Match* matches;
		} Rep;
		struct MatchScope {
			struct MSpec* child;
			struct MSpec* caps; // Numbered Caps first, then Named Caps.
		} Scope;
		struct MatchCap {
			struct MSpec* child;
		} Cap;
		struct MatchNameCap {
			struct MSpec* child;
		} NameCap;
	};
} Match;

#endif
