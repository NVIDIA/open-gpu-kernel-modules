/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2012 ARM Ltd.
 */
#ifndef __ASM_DEBUG_MONITORS_H
#define __ASM_DEBUG_MONITORS_H

#include <linux/errno.h>
#include <linux/types.h>
#include <asm/brk-imm.h>
#include <asm/esr.h>
#include <asm/insn.h>
#include <asm/ptrace.h>

/* Low-level stepping controls. */
#define DBG_MDSCR_SS		(1 << 0)
#define DBG_SPSR_SS		(1 << 21)

/* MDSCR_EL1 enabling bits */
#define DBG_MDSCR_KDE		(1 << 13)
#define DBG_MDSCR_MDE		(1 << 15)
#define DBG_MDSCR_MASK		~(DBG_MDSCR_KDE | DBG_MDSCR_MDE)

#define	DBG_ESR_EVT(x)		(((x) >> 27) & 0x7)

/* AArch64 */
#define DBG_ESR_EVT_HWBP	0x0
#define DBG_ESR_EVT_HWSS	0x1
#define DBG_ESR_EVT_HWWP	0x2
#define DBG_ESR_EVT_BRK		0x6

/*
 * Break point instruction encoding
 */
#define BREAK_INSTR_SIZE		AARCH64_INSN_SIZE

/*
 * BRK instruction encoding
 * The #imm16 value should be placed at bits[20:5] within BRK ins
 */
#define AARCH64_BREAK_MON	0xd4200000

/*
 * BRK instruction for provoking a fault on purpose
 * Unlike kgdb, #imm16 value with unallocated handler is used for faulting.
 */
#define AARCH64_BREAK_FAULT	(AARCH64_BREAK_MON | (FAULT_BRK_IMM << 5))

#define AARCH64_BREAK_KGDB_DYN_DBG	\
	(AARCH64_BREAK_MON | (KGDB_DYN_DBG_BRK_IMM << 5))

#define CACHE_FLUSH_IS_SAFE		1

/* kprobes BRK opcodes with ESR encoding  */
#define BRK64_OPCODE_KPROBES	(AARCH64_BREAK_MON | (KPROBES_BRK_IMM << 5))
#define BRK64_OPCODE_KPROBES_SS	(AARCH64_BREAK_MON | (KPROBES_BRK_SS_IMM << 5))
/* uprobes BRK opcodes with ESR encoding  */
#define BRK64_OPCODE_UPROBES	(AARCH64_BREAK_MON | (UPROBES_BRK_IMM << 5))

/* AArch32 */
#define DBG_ESR_EVT_BKPT	0x4
#define DBG_ESR_EVT_VECC	0x5

#define AARCH32_BREAK_ARM	0x07f001f0
#define AARCH32_BREAK_THUMB	0xde01
#define AARCH32_BREAK_THUMB2_LO	0xf7f0
#define AARCH32_BREAK_THUMB2_HI	0xa000

#ifndef __ASSEMBLY__
struct task_struct;

#define DBG_ARCH_ID_RESERVED	0	/* In case of ptrace ABI updates. */

#define DBG_HOOK_HANDLED	0
#define DBG_HOOK_ERROR		1

struct step_hook {
	struct list_head node;
	int (*fn)(struct pt_regs *regs, unsigned int esr);
};

void register_user_step_hook(struct step_hook *hook);
void unregister_user_step_hook(struct step_hook *hook);

void register_kernel_step_hook(struct step_hook *hook);
void unregister_kernel_step_hook(struct step_hook *hook);

struct break_hook {
	struct list_head node;
	int (*fn)(struct pt_regs *regs, unsigned int esr);
	u16 imm;
	u16 mask; /* These bits are ignored when comparing with imm */
};

void register_user_break_hook(struct break_hook *hook);
void unregister_user_break_hook(struct break_hook *hook);

void register_kernel_break_hook(struct break_hook *hook);
void unregister_kernel_break_hook(struct break_hook *hook);

u8 debug_monitors_arch(void);

enum dbg_active_el {
	DBG_ACTIVE_EL0 = 0,
	DBG_ACTIVE_EL1,
};

void enable_debug_monitors(enum dbg_active_el el);
void disable_debug_monitors(enum dbg_active_el el);

void user_rewind_single_step(struct task_struct *task);
void user_fastforward_single_step(struct task_struct *task);
void user_regs_reset_single_step(struct user_pt_regs *regs,
				 struct task_struct *task);

void kernel_enable_single_step(struct pt_regs *regs);
void kernel_disable_single_step(void);
int kernel_active_single_step(void);

#ifdef CONFIG_HAVE_HW_BREAKPOINT
int reinstall_suspended_bps(struct pt_regs *regs);
#else
static inline int reinstall_suspended_bps(struct pt_regs *regs)
{
	return -ENODEV;
}
#endif

int aarch32_break_handler(struct pt_regs *regs);

void debug_traps_init(void);

#endif	/* __ASSEMBLY */
#endif	/* __ASM_DEBUG_MONITORS_H */
