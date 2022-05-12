// SPDX-License-Identifier: GPL-2.0
/*
 * Generic VM initialization for x86-64 NUMA setups.
 * Copyright 2002,2003 Andi Kleen, SuSE Labs.
 */
#include <linux/memblock.h>

#include "numa_internal.h"

void __init initmem_init(void)
{
	x86_numa_init();
}
