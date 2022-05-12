/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_POWERPC_PROBES_H
#define _ASM_POWERPC_PROBES_H
#ifdef __KERNEL__
/*
 * Definitions common to probes files
 *
 * Copyright IBM Corporation, 2012
 */
#include <linux/types.h>

typedef u32 ppc_opcode_t;
#define BREAKPOINT_INSTRUCTION	0x7fe00008	/* trap */

/* Trap definitions per ISA */
#define IS_TW(instr)		(((instr) & 0xfc0007fe) == 0x7c000008)
#define IS_TD(instr)		(((instr) & 0xfc0007fe) == 0x7c000088)
#define IS_TDI(instr)		(((instr) & 0xfc000000) == 0x08000000)
#define IS_TWI(instr)		(((instr) & 0xfc000000) == 0x0c000000)

#ifdef CONFIG_PPC64
#define is_trap(instr)		(IS_TW(instr) || IS_TD(instr) || \
				IS_TWI(instr) || IS_TDI(instr))
#else
#define is_trap(instr)		(IS_TW(instr) || IS_TWI(instr))
#endif /* CONFIG_PPC64 */

#ifdef CONFIG_PPC_ADV_DEBUG_REGS
#define MSR_SINGLESTEP	(MSR_DE)
#else
#define MSR_SINGLESTEP	(MSR_SE)
#endif

/* Enable single stepping for the current task */
static inline void enable_single_step(struct pt_regs *regs)
{
	regs->msr |= MSR_SINGLESTEP;
#ifdef CONFIG_PPC_ADV_DEBUG_REGS
	/*
	 * We turn off Critical Input Exception(CE) to ensure that the single
	 * step will be for the instruction we have the probe on; if we don't,
	 * it is possible we'd get the single step reported for CE.
	 */
	regs->msr &= ~MSR_CE;
	mtspr(SPRN_DBCR0, mfspr(SPRN_DBCR0) | DBCR0_IC | DBCR0_IDM);
#ifdef CONFIG_PPC_47x
	isync();
#endif
#endif
}


#endif /* __KERNEL__ */
#endif	/* _ASM_POWERPC_PROBES_H */
