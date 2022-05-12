/* SPDX-License-Identifier: GPL-2.0-only */
/*
 *  arch/arm/mach-footbridge/include/mach/uncompress.h
 *
 *  Copyright (C) 1996-1999 Russell King
 */
#include <asm/mach-types.h>

/*
 * Note! This could cause problems on the NetWinder
 */
#define DC21285_BASE ((volatile unsigned int *)0x42000160)
#define SER0_BASE    ((volatile unsigned char *)0x7c0003f8)

static inline void putc(char c)
{
	if (machine_is_netwinder()) {
		while ((SER0_BASE[5] & 0x60) != 0x60)
			barrier();
		SER0_BASE[0] = c;
	} else {
		while (DC21285_BASE[6] & 8);
		DC21285_BASE[0] = c;
	}
}

static inline void flush(void)
{
}

/*
 * nothing to do
 */
#define arch_decomp_setup()
