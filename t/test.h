#include <stdio.h>



static int tests_run = 0;
static int tests_failed = 0;

static int tests_planned = -1;


// Preparation

static void plan (int num_tests) {
	tests_planned = num_tests;
	printf("1..%d\n", num_tests);
}


// Meta

static int proclaim (int cond, const char* desc) {
	tests_run++;
	if (!cond) {
		printf("not ");
		tests_failed++;
	}
	printf("ok %d - %s\n", tests_run, desc == NULL ? "" : desc);
	return cond;
}


// Primary

static int ok (int cond, const char* desc) {
	return proclaim(cond, desc);
}

static int nok (int cond, const char* desc) {
	return proclaim(!cond, desc);
}

static int is (int got, int expected, const char* desc) {
	int result = got == expected;
	proclaim(result, desc);
	if (!result) {
		printf("#      got: %d\n# expected: %d\n", got, expected);
	}
	return result;
}

static int isnt (int got, int expected, const char* desc) {
	int result = got != expected;
	return proclaim(result, desc);
}









