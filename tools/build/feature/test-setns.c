// SPDX-License-Identifier: GPL-2.0
#define _GNU_SOURCE
#include <sched.h>

int main(void)
{
	return setns(0, 0);
}
#undef _GNU_SOURCE
