// SPDX-License-Identifier: GPL-2.0
/*
 * usage.c
 *
 * Various reporting routines.
 * Originally copied from GIT source.
 *
 * Copyright (C) Linus Torvalds, 2005
 */
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <linux/compiler.h>

static __noreturn void usage_builtin(const char *err)
{
	fprintf(stderr, "\n Usage: %s\n", err);
	exit(129);
}

/* If we are in a dlopen()ed .so write to a global variable would segfault
 * (ugh), so keep things static. */
static void (*usage_routine)(const char *err) __noreturn = usage_builtin;

void usage(const char *err)
{
	usage_routine(err);
}
