#include <stdio.h>
#include <stdlib.h>
#include "match_types.h"


static long ndigits (long n) {
	return
	  n < 10 ? 1
	: n < 100 ? 2
	: n < 1000 ? 3
	: n < 10000 ? 4
	: n < 100000 ? 5
	: n < 1000000 ? 6
	: n < 10000000 ? 7
	: n < 100000000 ? 8
	: n < 1000000000 ? 9
	: 10;
}

size_t get_mspec_to_str_length (MSpec spec) {
	switch (spec.type) {
		case NOMATCH: {
			return 7;
		}
		case MNULL: {
			return 5;
		}
		case MBEGIN: {
			return 6;
		}
		case MEND: {
			return 4;
		}
		case MANY: {
			return 4;
		}
		case MCHAR: {
			return 8; // "MChar(c)"
		}
		case MCHARCLASS: {
			int r = 12;
			int i;
			for (i=0; i < spec.CharClass.nranges; i++) {
				r += 1;
				if (spec.CharClass.ranges[i].from < spec.CharClass.ranges[i].to)
					r += 3;
			}
			return r;
		}
		case MGROUP: {
			int r = 6;
			int i;
			for (i=0; i < spec.Group.nelements; i++)
				r += 2 + get_mspec_to_str_length(spec.Group.elements[i]);
			return r;
		}
		case MALT: {
			int r = 4;
			int i;
			for (i=0; i < spec.Alt.nalts; i++)
				r += 2 + get_mspec_to_str_length(spec.Alt.alts[i]);
			return r;
		}
		case MOPT: {
			return 6 + get_mspec_to_str_length(*spec.Opt.possible);
		}
		case MREPMAX: { // "MRepMax(Mwhatever, min..max)"
			return 13 + get_mspec_to_str_length(*spec.Rep.child)
			          + ndigits(spec.Rep.min) + ndigits(spec.Rep.max);
		}
		default: {
			return 8; // "MUnknown"
		}
	}
}

static size_t sprint_mspec (char* s, MSpec spec) {
	switch (spec.type) {
		case NOMATCH: {
			return sprintf(s, "NoMatch");
		}
		case MNULL: {
			return sprintf(s, "MNull");
		}
		case MBEGIN: {
			return sprintf(s, "MBegin");
		}
		case MEND: {
			return sprintf(s, "MEnd");
		}
		case MANY: {
			return sprintf(s, "MAny");
		}
		case MCHAR: {
			return sprintf(s, "MChar(%c)", spec.Char.c);
		}
		case MCHARCLASS: {
			size_t pos = sprintf(s, "MCharClass(");
			int i;
			for (i=0; i < spec.CharClass.nranges; i++) {
				s[pos++] = spec.CharClass.ranges[i].from;
				if (spec.CharClass.ranges[i].from < spec.CharClass.ranges[i].to)
					pos += sprintf(s+pos, "..%c", spec.CharClass.ranges[i].to);
			}
			return pos + sprintf(s+pos, ")");
		}
		case MGROUP: {
			size_t pos = sprintf(s, "MGroup(");
			int i;
			for (i=0; i < spec.Group.nelements; i++) {
				if (i != 0) pos += sprintf(s+pos, ", ");
				pos += sprint_mspec(s+pos, spec.Group.elements[i]);
			}
			return pos + sprintf(s+pos, ")");
		}
		case MALT: {
			size_t pos = sprintf(s, "MAlt(");
			int i;
			for (i=0; i < spec.Alt.nalts; i++) {
				if (i != 0) pos += sprintf(s+pos, ", ");
				pos += sprint_mspec(s+pos, spec.Alt.alts[i]);
			}
			return pos + sprintf(s+pos, ")");
		}
		case MOPT: {
			size_t pos = sprintf(s, "MOpt(");
			pos += sprint_mspec(s+pos, *spec.Opt.possible);
			return pos + sprintf(s+pos, ")");
		}
		case MREPMAX: {
			size_t pos = sprintf(s, "MRepMax(");
			pos += sprint_mspec(s+pos, *spec.Rep.child);
			return pos + sprintf(s+pos, ", %d..%d)", spec.Rep.min, spec.Rep.max);
		}
		default: {
			return sprintf(s, "MUnknown");
		}
	}
}




char* mspec_to_str (MSpec spec) {
	size_t len = get_mspec_to_str_length(spec);
	char* r = malloc(1 + len);
	sprint_mspec(r, spec);
	r[len] = 0;
	return r;
}





