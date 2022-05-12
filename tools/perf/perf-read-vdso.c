// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>

#define VDSO__MAP_NAME "[vdso]"

/*
 * Include definition of find_map() also used in util/vdso.c for
 * building perf.
 */
#include "util/find-map.c"

int main(void)
{
	void *start, *end;
	size_t size, written;

	if (find_map(&start, &end, VDSO__MAP_NAME))
		return 1;

	size = end - start;

	while (size) {
		written = fwrite(start, 1, size, stdout);
		if (!written)
			return 1;
		start += written;
		size -= written;
	}

	if (fflush(stdout))
		return 1;

	return 0;
}
