/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 *
 * Amit Bhor, Sameer Dhavale: Codito Technologies 2004
 */
#ifndef __ASM_ARC_PTRACE_H
#define __ASM_ARC_PTRACE_H

#include <uapi/asm/ptrace.h>

#ifndef __ASSEMBLY__

/* THE pt_regs: Defines how regs are saved during entry into kernel */

#ifdef CONFIG_ISA_ARCOMPACT
struct pt_regs {

	/* Real registers */
	unsigned long bta;	/* bta_l1, bta_l2, erbta */

	unsigned long lp_start, lp_end, lp_count;

	unsigned long status32;	/* status32_l1, status32_l2, erstatus */
	unsigned long ret;	/* ilink1, ilink2 or eret */
	unsigned long blink;
	unsigned long fp;
	unsigned long r26;	/* gp */

	unsigned long r12, r11, r10, r9, r8, r7, r6, r5, r4, r3, r2, r1, r0;

	unsigned long sp;	/* User/Kernel depending on where we came from */
	unsigned long orig_r0;

	/*
	 * To distinguish bet excp, syscall, irq
	 * For traps and exceptions, Exception Cause Register.
	 * 	ECR: <00> <VV> <CC> <PP>
	 * 	Last word used by Linux for extra state mgmt (syscall-restart)
	 * For interrupts, use artificial ECR values to note current prio-level
	 */
	union {
		struct {
#ifdef CONFIG_CPU_BIG_ENDIAN
			unsigned long state:8, ecr_vec:8,
				      ecr_cause:8, ecr_param:8;
#else
			unsigned long ecr_param:8, ecr_cause:8,
				      ecr_vec:8, state:8;
#endif
		};
		unsigned long event;
	};

	unsigned long user_r25;
};
#else

struct pt_regs {

	unsigned long orig_r0;

	union {
		struct {
#ifdef CONFIG_CPU_BIG_ENDIAN
			unsigned long state:8, ecr_vec:8,
				      ecr_cause:8, ecr_param:8;
#else
			unsigned long ecr_param:8, ecr_cause:8,
				      ecr_vec:8, state:8;
#endif
		};
		unsigned long event;
	};

	unsigned long bta;	/* bta_l1, bta_l2, erbta */

	unsigned long user_r25;

	unsigned long r26;	/* gp */
	unsigned long fp;
	unsigned long sp;	/* user/kernel sp depending on where we came from  */

	unsigned long r12, r30;

#ifdef CONFIG_ARC_HAS_ACCL_REGS
	unsigned long r58, r59;	/* ACCL/ACCH used by FPU / DSP MPY */
#endif
#ifdef CONFIG_ARC_DSP_SAVE_RESTORE_REGS
	unsigned long DSP_CTRL;
#endif

	/*------- Below list auto saved by h/w -----------*/
	unsigned long r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11;

	unsigned long blink;
	unsigned long lp_end, lp_start, lp_count;

	unsigned long ei, ldi, jli;

	unsigned long ret;
	unsigned long status32;
};

#endif

/* Callee saved registers - need to be saved only when you are scheduled out */

struct callee_regs {
	unsigned long r25, r24, r23, r22, r21, r20, r19, r18, r17, r16, r15, r14, r13;
};

#define instruction_pointer(regs)	((regs)->ret)
#define profile_pc(regs)		instruction_pointer(regs)

/* return 1 if user mode or 0 if kernel mode */
#define user_mode(regs) (regs->status32 & STATUS_U_MASK)

#define user_stack_pointer(regs)\
({  unsigned int sp;		\
	if (user_mode(regs))	\
		sp = (regs)->sp;\
	else			\
		sp = -1;	\
	sp;			\
})

/* return 1 if PC in delay slot */
#define delay_mode(regs) ((regs->status32 & STATUS_DE_MASK) == STATUS_DE_MASK)

#define in_syscall(regs)    ((regs->ecr_vec == ECR_V_TRAP) && !regs->ecr_param)
#define in_brkpt_trap(regs) ((regs->ecr_vec == ECR_V_TRAP) && regs->ecr_param)

#define STATE_SCALL_RESTARTED	0x01

#define syscall_wont_restart(reg) (reg->state |= STATE_SCALL_RESTARTED)
#define syscall_restartable(reg) !(reg->state &  STATE_SCALL_RESTARTED)

#define current_pt_regs()					\
({								\
	/* open-coded current_thread_info() */			\
	register unsigned long sp asm ("sp");			\
	unsigned long pg_start = (sp & ~(THREAD_SIZE - 1));	\
	(struct pt_regs *)(pg_start + THREAD_SIZE) - 1;	\
})

static inline long regs_return_value(struct pt_regs *regs)
{
	return (long)regs->r0;
}

#endif /* !__ASSEMBLY__ */

#endif /* __ASM_PTRACE_H */
