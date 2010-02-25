#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

typedef unsigned char uint8;




void walk_group_from (Match* m, char* str, int i);
void backtrack (Match* m, char* str);
Match make_match (Match spec, char* str, int start);
void printmatch(Match m);

void walk_group_from (Match* m, char* str, int i) {
	for (;;) {
		if ((*m).Group.elements[i].type == NOMATCH) {
			if (i == 0) {
				free((*m).Group.elements);
				(*m).type = NOMATCH;
				return;
			}
			i--;
			backtrack(&((*m).Group.elements[i]), str);
		}
		else {
			i++;
			if (i == (*m).Group.nelements) {
				(*m).end = (*m).Group.elements[i-1].end;
				return;
			}
			(*m).Group.elements[i] = make_match((*m).Group.elements[i], str, (*m).Group.elements[i-1].end);
		}
	}
}


void backtrack(Match* m, char* str) {
	switch ((*m).type) {
		case NOMATCH: {
			fprintf(stderr, "Error: Tried to backtrack across failed match.\n");
			abort();
		}
		case MANY: {
			(*m).type = NOMATCH;
			return;
		}
		case MCHAR: {
			(*m).type = NOMATCH;
			return;
		}
		case MGROUP: {
			if ((*m).Group.nelements == 0) {
				(*m).type = NOMATCH;
				return;
			}
			backtrack(&((*m).Group.elements[(*m).Group.nelements-1]), str);
			walk_group_from(m, str, (*m).Group.nelements-1);
			return;
		}
	}
}



Match make_match (Match spec, char* str, int start) {
	switch (spec.type) {
		case NOMATCH: {
			return NoMatch;
		}
		case MANY: {
			if (str[start] == 0)
				return NoMatch;
			Match r;
			 r.type  = MANY;
			 r.start = start;
			 r.end   = start + 1;
			return r;
		}
		case MCHAR: {
			if (str[start] == spec.Char.c) {
				Match r;
				 r.type   = MCHAR;
				 r.Char.c = spec.Char.c;
				 r.start  = start;
				 r.end    = start + 1;
				return r;
			}
			return NoMatch;
		}
		case MGROUP: {
			Match r;
			 r.type            = MGROUP;
			 r.Group.nelements = spec.Group.nelements;
			 r.Group.elements  = malloc(r.Group.nelements * sizeof(Match));
			 r.start           = start;
			if (r.Group.nelements == 0) {
				r.end = r.start;
				return r;
			}
			int i;
			for (i=0; i < r.Group.nelements; i++)
				r.Group.elements[i] = spec.Group.elements[i];
			r.Group.elements[i] = make_match(r.Group.elements[i], str, r.start);
			walk_group_from(&r, str, 0);
			return r;
		}
		default: return NoMatch;
	}
}
