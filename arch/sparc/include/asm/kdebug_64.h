/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _SPARC64_KDEBUG_H
#define _SPARC64_KDEBUG_H

struct pt_regs;

void bad_trap(struct pt_regs *, long);

/* Grossly misnamed. */
enum die_val {
	DIE_OOPS = 1,
	DIE_DEBUG,	/* ta 0x70 */
	DIE_DEBUG_2,	/* ta 0x71 */
	DIE_BPT,	/* ta 0x73 */
	DIE_SSTEP,	/* ta 0x74 */
	DIE_DIE,
	DIE_TRAP,
	DIE_TRAP_TL1,
	DIE_CALL,
	DIE_NMI,
	DIE_NMIWATCHDOG,
};

#endif
