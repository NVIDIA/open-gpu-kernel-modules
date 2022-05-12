// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2014, Michael Ellerman, IBM Corp.
 */

#include <stdio.h>
#include <stdlib.h>

#include "event.h"
#include "utils.h"

#define MALLOC_SIZE     (0x10000 * 10)  /* Ought to be enough .. */

/*
 * Tests that the L3 bank handling is correct. We fixed it in commit e9aaac1.
 */
static int l3_bank_test(void)
{
	struct event event;
	char *p;
	int i;

	// The L3 bank logic is only used on Power8 or later
	SKIP_IF(!have_hwcap2(PPC_FEATURE2_ARCH_2_07));

	p = malloc(MALLOC_SIZE);
	FAIL_IF(!p);

	event_init(&event, 0x84918F);

	FAIL_IF(event_open(&event));

	for (i = 0; i < MALLOC_SIZE; i += 0x10000)
		p[i] = i;

	event_read(&event);
	event_report(&event);

	FAIL_IF(event.result.running == 0);
	FAIL_IF(event.result.enabled == 0);

	event_close(&event);
	free(p);

	return 0;
}

int main(void)
{
	return test_harness(l3_bank_test, "l3_bank_test");
}
