/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright 2016-17 IBM Corp.
 */
#include <asm/ppc-opcode.h>
#include <asm/reg.h>

/*
 * Copy/paste instructions:
 *
 *	copy RA,RB
 *		Copy contents of address (RA) + effective_address(RB)
 *		to internal copy-buffer.
 *
 *	paste RA,RB
 *		Paste contents of internal copy-buffer to the address
 *		(RA) + effective_address(RB)
 */
static inline int vas_copy(void *crb, int offset)
{
	asm volatile(PPC_COPY(%0, %1)";"
		:
		: "b" (offset), "b" (crb)
		: "memory");

	return 0;
}

static inline int vas_paste(void *paste_address, int offset)
{
	u32 cr;

	cr = 0;
	asm volatile(PPC_PASTE(%1, %2)";"
		"mfocrf %0, 0x80;"
		: "=r" (cr)
		: "b" (offset), "b" (paste_address)
		: "memory", "cr0");

	/* We mask with 0xE to ignore SO */
	return (cr >> CR0_SHIFT) & 0xE;
}
