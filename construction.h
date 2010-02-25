#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "match_types.h"


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
	r.CharClass.ranges  = malloc(nranges * sizeof(struct MSpecCharRange));
	memcpy(r.CharClass.ranges, ranges, nranges * sizeof(struct MSpecCharRange));
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











