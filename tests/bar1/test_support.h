/* SPDX-License-Identifier: MIT */
#ifndef BAR1_TEST_SUPPORT_H
#define BAR1_TEST_SUPPORT_H

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* Release-build assertion behavior, with allocation failures injected below. */
#define PORT_BREAKPOINT() abort()
#define PORT_BREAKPOINT_CHECKED() ((void)0)
#define PORT_COVERAGE_PUSH_OFF() ((void)0)
#define PORT_COVERAGE_POP() ((void)0)

#endif
