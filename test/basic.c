#include "ltm.h"
#include <stdio.h>

void printmatch (Match m) {
	switch (m.type) {
		case NOMATCH: {
			printf("NoMatch");
			return;
		}
		case MNULL: {
			printf("MNull[%d..%d]", m.start, m.end);
			return;
		}
		case MBEGIN: {
			printf("MBegin[%d..%d]", m.start, m.end);
			return;
		}
		case MEND: {
			printf("MEnd[%d..%d]", m.start, m.end);
			return;
		}
		case MANY: {
			printf("MAny[%d..%d]", m.start, m.end);
			return;
		}
		case MCHAR: {
			printf("MChar(%c)[%d..%d]", m.spec->Char.c, m.start, m.end);
			return;
		}
		case MCHARCLASS: {
			printf("MCharClass(");
			int i;
			for (i=0; i < m.spec->CharClass.nranges; i++) {
				printf("%c", m.spec->CharClass.ranges[i].from);
				if (m.spec->CharClass.ranges[i].from < m.spec->CharClass.ranges[i].to)
					printf("..%c", m.spec->CharClass.ranges[i].to);
			}
			printf(")[%d..%d]", m.start, m.end);
			return;
		}
		case MGROUP: {
			if (m.Group.nelements == 0) {
				printf("MGroup()[%d..%d]", m.start, m.end);
				return;
			}
			printf("MGroup(");
			printmatch(m.Group.elements[0]);
			int i;
			for (i=1; i < m.Group.nelements; i++) {
				printf(", ");
				printmatch(m.Group.elements[i]);
			}
			printf(")[%d..%d]", m.start, m.end);
			return;
		}
		case MREPMAX: {
			if (m.Rep.nmatches == 0) {
				printf("MRepMax()[%d..%d]", m.start, m.end);
				return;
			}
			printf("MRepMax(");
			printmatch(m.Rep.matches[0]);
			int i;
			for (i=1; i < m.Rep.nmatches; i++) {
				printf(", ");
				printmatch(m.Rep.matches[i]);
			}
			printf(")[%d..%d]", m.start, m.end);
			return;
		}
		case MOPT: {
			printf("MOpt(");
			printmatch(*m.Opt.possible);
			printf(")[%d..%d]", m.start, m.end);
			return;
		}
		case MSCOPE: {
			printf("MScope(");
			printmatch(*m.Scope.child);
			printf(")[%d..%d]", m.start, m.end);
			return;
		}
		case MCAP: {
			printf("MCap(");
			printmatch(*m.Cap.child);
			printf(")[%d..%d]", m.start, m.end);
			return;
		}
		case MNAMECAP: {
			printf("MNameCap(");
			printmatch(*m.NameCap.child);
			printf(", \"%s\")[%d..%d]", m.spec->NameCap.name, m.start, m.end);
			return;
		}
		case MALT: {
			printf("MAlt(");
			printmatch(*m.Alt.matched);
			printf(")[%d..%d]", m.start, m.end);
			return;
		}
		default: {
			printf("UnknownMatch");
			return;
		}
	}
}


Match test_match (MSpec spec, char* str, int start, const char* comparison) {
	Match m;
	LTM_init_Match(&m, &spec, start);
	LTM_start(&m, str, NULL);
	printmatch(m);
	printf("\n");
	puts(comparison);
	return m;
};

int main(int argv, char** argc) {
	char* teststr = "abc";
	char* emptystr = "";
	printf("t1, MAny match\n");
	MSpec t1;
	 t1.type = MANY;
	test_match(t1, teststr, 0, "MAny[0..1]");
	printf("t2  MAny fail (end of string)\n");
	test_match(t1, emptystr, 0, "NoMatch");
	printf("t3  MChar match\n");
	MSpec t3;
	 t3.type   = MCHAR;
	 t3.Char.c = 'a';
	test_match(t3, teststr, 0, "MChar(a)[0..1]");
	printf("t4  MChar fail (not right char)\n");
	MSpec t4;
	 t4.type   = MCHAR;
	 t4.Char.c = 'c';
	test_match(t4, teststr, 0, "NoMatch");
	printf("t5  MGroup match (a, any, c)\n");
	MSpec t5;
	 t5.type              = MGROUP;
	 t5.Group.nelements   = 3;
	 t5.Group.elements    = malloc(3 * sizeof(Match));
	 t5.Group.elements[0] = t3;
	 t5.Group.elements[1] = t1;
	 t5.Group.elements[2] = t4;
	test_match(t5, teststr, 0, "MGroup(MChar(a)[0..1], MAny[1..2], MChar(c)[2..3])[0..3]");
	printf("t6  MGroup fail (a, any, a)\n");
	 t5.Group.elements[2] = t3;
	test_match(t5, teststr, 0, "NoMatch");
	printf("t7  MGroup nested match ((a, any), c)\n");
	MSpec t7i;
	 t7i.type              = MGROUP;
	 t7i.Group.nelements   = 2;
	 t7i.Group.elements    = malloc(2 * sizeof(Match));
	 t7i.Group.elements[0] = t3;
	 t7i.Group.elements[1] = t1;
	MSpec t7;
	 t7.type              = MGROUP;
	 t7.Group.nelements   = 2;
	 t7.Group.elements    = malloc(2 * sizeof(Match));
	 t7.Group.elements[0] = t7i;
	 t7.Group.elements[1] = t4;
	test_match(t7, teststr, 0, "MGroup(MGroup(MChar(a)[0..1], MAny[1..2])[0..2], MChar(c)[2..3])[0..3]");
	printf("t8  MGroup nested fail ((a, any), a)\n");
	 t7.Group.elements[1] = t3;
	test_match(t7, teststr, 0, "NoMatch");
	printf("t9  MOpt match (a)\n");
	MSpec t9;
	 t9.type          = MOPT;
	 t9.Opt.possible  = &t3;
	test_match(t9, teststr, 0, "MOpt(MChar(a)[0..1])[0..1]");
	printf("t10  MOpt match (null)\n");
	 t9.Opt.possible  = &t4;
	test_match(t9, teststr, 0, "MNull[0..0]");
	printf("t11  MGroup backtracking (a, opt(b), b)\n");
	 t9.Opt.possible  = &t4;
	 t4.Char.c = 'b';
	MSpec t11;
	 t11.type              = MGROUP;
	 t11.Group.nelements   = 3;
	 t11.Group.elements    = malloc(3 * sizeof(Match));
	 t11.Group.elements[0] = t3;
	 t11.Group.elements[1] = t9;
	 t11.Group.elements[2] = t4;
	test_match(t11, teststr, 0, "MGroup(MChar(a)[0..1], MNull[1..1], MChar(b)[1..2])[0..2]");
	printf("t12  MRepMax match 0..3\n");
	MSpec t12;
	 t12.type      = MREPMAX;
	 t12.Rep.child = &t1;
	 t12.Rep.min   = 0;
	 t12.Rep.max   = 3;
	test_match(t12, teststr, 0, "MRepMax(MAny[0..1], MAny[1..2], MAny[2..3])[0..3]");
	printf("t13  MRepMax match 3..*\n");
	 t12.Rep.min   = 3;
	 t12.Rep.max   = 10000;
	test_match(t12, teststr, 0, "MRepMax(MAny[0..1], MAny[1..2], MAny[2..3])[0..3]");
	printf("t14  MRepMax match 0..2\n");
	 t12.Rep.min   = 0;
	 t12.Rep.max   = 2;
	test_match(t12, teststr, 0, "MRepMax(MAny[0..1], MAny[1..2])[0..2]");
	printf("t15  MRepMax backtracking (any(0..2), b)\n");
	MSpec t15;
	 t15.type              = MGROUP;
	 t15.Group.nelements   = 2;
	 t15.Group.elements    = malloc(2*sizeof(MSpec));
	 t15.Group.elements[0] = t12;
	 t15.Group.elements[1] = t4;
	test_match(t15, teststr, 0, "MGroup(MRepMax(MAny[0..1])[0..1], MChar(b)[1..2])[0..2]");
	printf("t16  MAlt match (c | a | any)->a\n");
	MSpec t16;
	 t16.type        = MALT;
	 t16.Alt.nalts   = 3;
	 t16.Alt.alts    = malloc(3*sizeof(MSpec));
	 t16.Alt.alts[0] = t4;
	 t16.Alt.alts[1] = t3;
	 t16.Alt.alts[2] = t1;
	test_match(t16, teststr, 0, "MAlt(MChar(a)[0..1])[0..1]");
	printf("t17  MAlt backtracking (((a, any) | a | any)->a, b)\n");
	 t16.Alt.alts[0]       = t7i;
	 t15.Group.elements[0] = t16;
	test_match(t15, teststr, 0, "MGroup(MAlt(MChar(a)[0..1])[0..1], MChar(b)[1..2])[0..2]");
	printf("t18  MAlt backtrack+fail (((a, any) | a | any), a)\n");
	 t15.Group.elements[1] = t3;
	test_match(t15, teststr, 0, "NoMatch");
	printf("t19  MBegin and MEnd match\n");
	MSpec begin;
	 begin.type = MBEGIN;
	MSpec end;
	 end.type = MEND;
	MSpec t19;
	 t19.type              = MGROUP;
	 t19.Group.nelements   = 5;
	 t19.Group.elements    = malloc(5*sizeof(MSpec));
	 t19.Group.elements[0] = begin;
	 t19.Group.elements[1] =
	 t19.Group.elements[2] =
	 t19.Group.elements[3] = t1;
	 t19.Group.elements[4] = end;
	test_match(t19, teststr, 0, "MGroup(MBegin[0..0], MAny[0..1], MAny[1..2], MAny[2..3], MEnd[3..3])[0..3]");
	printf("t20  Character class match\n");
	MSpec t20;
	 t20.type                = MCHARCLASS;
	 t20.CharClass.negative  = 0;
	 t20.CharClass.nranges   = 2;
	 t20.CharClass.ranges    = malloc(2 * sizeof(struct MCharRange));
	 t20.CharClass.ranges[0].from = 'a';
	 t20.CharClass.ranges[0].to   = 'a';
	 t20.CharClass.ranges[1].from = 'c';
	 t20.CharClass.ranges[1].to   = 'g';
	test_match(t20, teststr, 0, "MCharClass(ac..g)[0..1]");
	printf("t21  Character class fail\n");
	test_match(t20, teststr, 1, "NoMatch");
	printf("t22  Character class match\n");
	test_match(t20, teststr, 2, "MCharClass(ac..g)[2..3]");
	printf("t23  Negative character class fail\n");
	 t20.CharClass.negative = 1;
	test_match(t20, teststr, 0, "NoMatch");
	printf("t24  Negative character class match\n");
	test_match(t20, teststr, 1, "MCharClass(ac..g)[1..2]");
	printf("t25  Negative character class fail\n");
	test_match(t20, teststr, 2, "NoMatch");
	printf("t26  Scope and Capture match\n");
	MSpec t26c;
	 t26c.type = MCAP;
	 t26c.Cap.id = 0;
	 t26c.Cap.child = &t3;
	MSpec t26;
	 t26.type = MSCOPE;
	 t26.Scope.ncaps = 1;
	 t26.Scope.nnamecaps = 0;
	 t26.Scope.child = &t26c;
	Match t26r;
	t26r = test_match(t26, teststr, 0, "MScope(MCap(MChar(a)[0..1])[0..1])[0..1]");
	if (t26r.Scope.caps[0] == t26r.Scope.child)
		printf("Capture succeeded\n");
	else
		printf("Capture failed: %08x != %08x\n", t26r.Scope.caps[0], t26r.Scope.child);
	printf("t27  Scope and Capture fail\n");
	t26c.Cap.child = &t4;
	t26r = test_match(t26, teststr, 0, "NoMatch");
	if (t26r.Scope.caps[0] == NULL)
		printf("Non-capture succeeded\n");
	else
		printf("Non-capture failed: %08x != 00000000\n", t26r.Scope.caps[0]);
	printf("t28  Scope and NameCapture match\n");
	 t26.Scope.nnamecaps = 1;
	 t26.Scope.names = malloc(sizeof(char*));
	 t26.Scope.names[0] = "thing";
	MSpec t28;
	 t28.type = MNAMECAP;
	 t28.NameCap.id = 0;
	 t28.NameCap.name = "thing";
	 t28.NameCap.child = &t19;
	 t26.Scope.child = &t28;
	 t26r = test_match(t26, teststr, 0, "MScope(MNameCap(MGroup(MBegin[0..0], MAny[0..1], MAny[1..2], MAny[2..3], MEnd[3..3])[0..3], \"thing\")[0..3])[0..3]");
	Match* l = LTM_lookup_NameCap(&t26r, "thing");
	if (l == t26r.Scope.child)
		printf("NameCapture succeeded\n");
	else
		printf("NameCapture failed: %08x != %08x\n", l, &t28);
	printf("t29  Ref match\n");
	MSpec t29;
	 t29.type = MREF;
	printf(" ref type is %d\n", t29.type);
	 t29.Ref.ref = &t28;
	 t26.Scope.child = &t29;
	printf(" ref ptr type is %d\n", (&t29)->type);
	printf(" scope child type is %d\n", t26.Scope.child->type);
	test_match(t26, teststr, 0, "MScope(MNameCap(MGroup(MBegin[0..0], MAny[0..1], MAny[1..2], MAny[2..3], MEnd[3..3])[0..3], \"thing\")[0..3])[0..3]");

	 // cleanup.
	printf("freeing...\n");
	free(t20.CharClass.ranges);
	printf("t20\n");
	free(t19.Group.elements);
	printf("t19\n");
	free(t16.Alt.alts);
	printf("t16\n");
	free(t15.Group.elements);
	printf("t15\n");
	free(t11.Group.elements);
	printf("t11\n");
	free(t7.Group.elements);
	printf("t7\n");
	free(t7i.Group.elements);
	printf("t7i\n");
	free(t5.Group.elements);
	printf("t5\n");
	if (argv > 1) getc(stdin);
	return 0;
}








