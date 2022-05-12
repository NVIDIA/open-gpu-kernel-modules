// SPDX-License-Identifier: GPL-2.0
/* Copyright (c) 2020 Facebook */
#include <test_progs.h>
#include <time.h>
#include "test_subprogs.skel.h"
#include "test_subprogs_unused.skel.h"

static int duration;

void test_subprogs(void)
{
	struct test_subprogs *skel;
	struct test_subprogs_unused *skel2;
	int err;

	skel = test_subprogs__open_and_load();
	if (CHECK(!skel, "skel_open", "failed to open skeleton\n"))
		return;

	err = test_subprogs__attach(skel);
	if (CHECK(err, "skel_attach", "failed to attach skeleton: %d\n", err))
		goto cleanup;

	usleep(1);

	CHECK(skel->bss->res1 != 12, "res1", "got %d, exp %d\n", skel->bss->res1, 12);
	CHECK(skel->bss->res2 != 17, "res2", "got %d, exp %d\n", skel->bss->res2, 17);
	CHECK(skel->bss->res3 != 19, "res3", "got %d, exp %d\n", skel->bss->res3, 19);
	CHECK(skel->bss->res4 != 36, "res4", "got %d, exp %d\n", skel->bss->res4, 36);

	skel2 = test_subprogs_unused__open_and_load();
	ASSERT_OK_PTR(skel2, "unused_progs_skel");
	test_subprogs_unused__destroy(skel2);

cleanup:
	test_subprogs__destroy(skel);
}
