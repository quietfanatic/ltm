
#include "ltm.h"
#include "tostr.h"
#include <stdio.h>

int main () {
	MSpec t1;
	t1 = create_MGroup(3,
		create_MBegin(),
		create_MAlt(2,
			create_MGroup(3,
				create_MChar('a'),
				create_MChar('b'),
				create_MChar('c')
			),
			create_MRep(0, 0, 52,
				create_MChar('a')
			)
		),
		create_MEnd()
	);
	char* t1s = mspec_to_str(t1);
	char* t1c = "MGroup(MBegin, MAlt(MGroup(MChar(a), MChar(b), MChar(c)), MRep(MChar(a), 0..52)), MEnd)";
	puts(t1s);
	puts(t1c);
	puts(strcmp(t1s, t1c) ? "Failure" : "Success");
	free(t1s);
	destroy_MSpec(t1);  // No way to test memory freeing :(
	return 0;
}
