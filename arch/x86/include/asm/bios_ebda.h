/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_BIOS_EBDA_H
#define _ASM_X86_BIOS_EBDA_H

#include <asm/io.h>

/*
 * Returns physical address of EBDA.  Returns 0 if there is no EBDA.
 */
static inline unsigned int get_bios_ebda(void)
{
	/*
	 * There is a real-mode segmented pointer pointing to the
	 * 4K EBDA area at 0x40E.
	 */
	unsigned int address = *(unsigned short *)phys_to_virt(0x40E);
	address <<= 4;
	return address;	/* 0 means none */
}

void reserve_bios_regions(void);

#ifdef CONFIG_X86_CHECK_BIOS_CORRUPTION
/*
 * This is obviously not a great place for this, but we want to be
 * able to scatter it around anywhere in the kernel.
 */
void check_for_bios_corruption(void);
void start_periodic_check_for_corruption(void);
#else
static inline void check_for_bios_corruption(void)
{
}

static inline void start_periodic_check_for_corruption(void)
{
}
#endif

#endif /* _ASM_X86_BIOS_EBDA_H */
