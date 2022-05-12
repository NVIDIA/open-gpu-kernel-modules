/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_KPROBES_H
#define _ASM_KPROBES_H
/*
 *  Kernel Probes (KProbes)
 *
 * Copyright (C) IBM Corporation, 2002, 2004
 * Copyright (C) Intel Corporation, 2005
 *
 * 2005-Apr     Rusty Lynch <rusty.lynch@intel.com> and Anil S Keshavamurthy
 *              <anil.s.keshavamurthy@intel.com> adapted from i386
 */
#include <asm-generic/kprobes.h>
#include <asm/break.h>

#define BREAK_INST	(long)(__IA64_BREAK_KPROBE << 6)

#ifdef CONFIG_KPROBES

#include <linux/types.h>
#include <linux/ptrace.h>
#include <linux/percpu.h>

#define __ARCH_WANT_KPROBES_INSN_SLOT
#define MAX_INSN_SIZE   2	/* last half is for kprobe-booster */
#define NOP_M_INST	(long)(1<<27)
#define BRL_INST(i1, i2) ((long)((0xcL << 37) |	/* brl */ \
				(0x1L << 12) |	/* many */ \
				(((i1) & 1) << 36) | ((i2) << 13))) /* imm */

typedef union cmp_inst {
	struct {
	unsigned long long qp : 6;
	unsigned long long p1 : 6;
	unsigned long long c  : 1;
	unsigned long long r2 : 7;
	unsigned long long r3 : 7;
	unsigned long long p2 : 6;
	unsigned long long ta : 1;
	unsigned long long x2 : 2;
	unsigned long long tb : 1;
	unsigned long long opcode : 4;
	unsigned long long reserved : 23;
	}f;
	unsigned long long l;
} cmp_inst_t;

struct kprobe;

typedef struct _bundle {
	struct {
		unsigned long long template : 5;
		unsigned long long slot0 : 41;
		unsigned long long slot1_p0 : 64-46;
	} quad0;
	struct {
		unsigned long long slot1_p1 : 41 - (64-46);
		unsigned long long slot2 : 41;
	} quad1;
} __attribute__((__aligned__(16)))  bundle_t;

struct prev_kprobe {
	struct kprobe *kp;
	unsigned long status;
};

#define	MAX_PARAM_RSE_SIZE	(0x60+0x60/0x3f)
/* per-cpu kprobe control block */
#define ARCH_PREV_KPROBE_SZ 2
struct kprobe_ctlblk {
	unsigned long kprobe_status;
	unsigned long *bsp;
	unsigned long cfm;
	atomic_t prev_kprobe_index;
	struct prev_kprobe prev_kprobe[ARCH_PREV_KPROBE_SZ];
};

#define kretprobe_blacklist_size 0

#define SLOT0_OPCODE_SHIFT	(37)
#define SLOT1_p1_OPCODE_SHIFT	(37 - (64-46))
#define SLOT2_OPCODE_SHIFT 	(37)

#define INDIRECT_CALL_OPCODE		(1)
#define IP_RELATIVE_CALL_OPCODE		(5)
#define IP_RELATIVE_BRANCH_OPCODE	(4)
#define IP_RELATIVE_PREDICT_OPCODE	(7)
#define LONG_BRANCH_OPCODE		(0xC)
#define LONG_CALL_OPCODE		(0xD)
#define flush_insn_slot(p)		do { } while (0)

typedef struct kprobe_opcode {
	bundle_t bundle;
} kprobe_opcode_t;

/* Architecture specific copy of original instruction*/
struct arch_specific_insn {
	/* copy of the instruction to be emulated */
	kprobe_opcode_t *insn;
 #define INST_FLAG_FIX_RELATIVE_IP_ADDR		1
 #define INST_FLAG_FIX_BRANCH_REG		2
 #define INST_FLAG_BREAK_INST			4
 #define INST_FLAG_BOOSTABLE			8
 	unsigned long inst_flag;
 	unsigned short target_br_reg;
	unsigned short slot;
};

extern int kprobe_fault_handler(struct pt_regs *regs, int trapnr);
extern int kprobe_exceptions_notify(struct notifier_block *self,
				    unsigned long val, void *data);

extern void invalidate_stacked_regs(void);
extern void flush_register_stack(void);
extern void arch_remove_kprobe(struct kprobe *p);

#endif /* CONFIG_KPROBES */
#endif /* _ASM_KPROBES_H */
