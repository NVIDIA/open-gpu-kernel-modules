/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * OpenRISC Linux
 *
 * Linux architectural port borrowing liberally from similar works of
 * others.  All original copyrights apply as per the original source
 * declaration.
 *
 * OpenRISC implementation:
 * Copyright (C) 2003 Matjaz Breskvar <phoenix@bsemi.com>
 * Copyright (C) 2010-2011 Jonas Bonn <jonas@southpole.se>
 * et al.
 */

#ifndef __ASM_OPENRISC_CPUINFO_H
#define __ASM_OPENRISC_CPUINFO_H

struct cpuinfo_or1k {
	u32 clock_frequency;

	u32 icache_size;
	u32 icache_block_size;
	u32 icache_ways;

	u32 dcache_size;
	u32 dcache_block_size;
	u32 dcache_ways;

	u16 coreid;
};

extern struct cpuinfo_or1k cpuinfo_or1k[NR_CPUS];
extern void setup_cpuinfo(void);

#endif /* __ASM_OPENRISC_CPUINFO_H */
