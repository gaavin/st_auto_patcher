#include <stdio.h>

#include "patch_finder.h"
#include "types.h"


patch *find_patches(char *path) {
	patch *patches = NULL;

	patches = find_patches(path);
	if (!(patches)) {
		fprintf(stderr, "couldn't find patches\n");
	}
	return patches;
}
