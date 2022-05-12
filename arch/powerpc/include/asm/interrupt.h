/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_POWERPC_INTERRUPT_H
#define _ASM_POWERPC_INTERRUPT_H

/* BookE/4xx */
#define INTERRUPT_CRITICAL_INPUT  0x100

/* BookE */
#define INTERRUPT_DEBUG           0xd00
#ifdef CONFIG_BOOKE
#define INTERRUPT_PERFMON         0x260
#define INTERRUPT_DOORBELL        0x280
#endif

/* BookS/4xx/8xx */
#define INTERRUPT_MACHINE_CHECK   0x200

/* BookS/8xx */
#define INTERRUPT_SYSTEM_RESET    0x100

/* BookS */
#define INTERRUPT_DATA_SEGMENT    0x380
#define INTERRUPT_INST_SEGMENT    0x480
#define INTERRUPT_TRACE           0xd00
#define INTERRUPT_H_DATA_STORAGE  0xe00
#define INTERRUPT_HMI			0xe60
#define INTERRUPT_H_FAC_UNAVAIL   0xf80
#ifdef CONFIG_PPC_BOOK3S
#define INTERRUPT_DOORBELL        0xa00
#define INTERRUPT_PERFMON         0xf00
#define INTERRUPT_ALTIVEC_UNAVAIL	0xf20
#endif

/* BookE/BookS/4xx/8xx */
#define INTERRUPT_DATA_STORAGE    0x300
#define INTERRUPT_INST_STORAGE    0x400
#define INTERRUPT_EXTERNAL		0x500
#define INTERRUPT_ALIGNMENT       0x600
#define INTERRUPT_PROGRAM         0x700
#define INTERRUPT_SYSCALL         0xc00
#define INTERRUPT_TRACE			0xd00

/* BookE/BookS/44x */
#define INTERRUPT_FP_UNAVAIL      0x800

/* BookE/BookS/44x/8xx */
#define INTERRUPT_DECREMENTER     0x900

#ifndef INTERRUPT_PERFMON
#define INTERRUPT_PERFMON         0x0
#endif

/* 8xx */
#define INTERRUPT_SOFT_EMU_8xx		0x1000
#define INTERRUPT_INST_TLB_MISS_8xx	0x1100
#define INTERRUPT_DATA_TLB_MISS_8xx	0x1200
#define INTERRUPT_INST_TLB_ERROR_8xx	0x1300
#define INTERRUPT_DATA_TLB_ERROR_8xx	0x1400
#define INTERRUPT_DATA_BREAKPOINT_8xx	0x1c00
#define INTERRUPT_INST_BREAKPOINT_8xx	0x1d00

/* 603 */
#define INTERRUPT_INST_TLB_MISS_603		0x1000
#define INTERRUPT_DATA_LOAD_TLB_MISS_603	0x1100
#define INTERRUPT_DATA_STORE_TLB_MISS_603	0x1200

#ifndef __ASSEMBLY__

#include <linux/context_tracking.h>
#include <linux/hardirq.h>
#include <asm/cputime.h>
#include <asm/ftrace.h>
#include <asm/kprobes.h>
#include <asm/runlatch.h>

static inline void nap_adjust_return(struct pt_regs *regs)
{
#ifdef CONFIG_PPC_970_NAP
	if (unlikely(test_thread_local_flags(_TLF_NAPPING))) {
		/* Can avoid a test-and-clear because NMIs do not call this */
		clear_thread_local_flags(_TLF_NAPPING);
		regs->nip = (unsigned long)power4_idle_nap_return;
	}
#endif
}

struct interrupt_state {
};

static inline void booke_restore_dbcr0(void)
{
#ifdef CONFIG_PPC_ADV_DEBUG_REGS
	unsigned long dbcr0 = current->thread.debug.dbcr0;

	if (IS_ENABLED(CONFIG_PPC32) && unlikely(dbcr0 & DBCR0_IDM)) {
		mtspr(SPRN_DBSR, -1);
		mtspr(SPRN_DBCR0, global_dbcr0[smp_processor_id()]);
	}
#endif
}

static inline void interrupt_enter_prepare(struct pt_regs *regs, struct interrupt_state *state)
{
#ifdef CONFIG_PPC32
	if (!arch_irq_disabled_regs(regs))
		trace_hardirqs_off();

	if (user_mode(regs)) {
		kuep_lock();
		account_cpu_user_entry();
	} else {
		kuap_save_and_lock(regs);
	}
#endif

#ifdef CONFIG_PPC64
	if (irq_soft_mask_set_return(IRQS_ALL_DISABLED) == IRQS_ENABLED)
		trace_hardirqs_off();
	local_paca->irq_happened |= PACA_IRQ_HARD_DIS;

	if (user_mode(regs)) {
		CT_WARN_ON(ct_state() != CONTEXT_USER);
		user_exit_irqoff();

		account_cpu_user_entry();
		account_stolen_time();
	} else {
		/*
		 * CT_WARN_ON comes here via program_check_exception,
		 * so avoid recursion.
		 */
		if (TRAP(regs) != INTERRUPT_PROGRAM)
			CT_WARN_ON(ct_state() != CONTEXT_KERNEL);
	}
#endif

	booke_restore_dbcr0();
}

/*
 * Care should be taken to note that interrupt_exit_prepare and
 * interrupt_async_exit_prepare do not necessarily return immediately to
 * regs context (e.g., if regs is usermode, we don't necessarily return to
 * user mode). Other interrupts might be taken between here and return,
 * context switch / preemption may occur in the exit path after this, or a
 * signal may be delivered, etc.
 *
 * The real interrupt exit code is platform specific, e.g.,
 * interrupt_exit_user_prepare / interrupt_exit_kernel_prepare for 64s.
 *
 * However interrupt_nmi_exit_prepare does return directly to regs, because
 * NMIs do not do "exit work" or replay soft-masked interrupts.
 */
static inline void interrupt_exit_prepare(struct pt_regs *regs, struct interrupt_state *state)
{
}

static inline void interrupt_async_enter_prepare(struct pt_regs *regs, struct interrupt_state *state)
{
#ifdef CONFIG_PPC_BOOK3S_64
	if (cpu_has_feature(CPU_FTR_CTRL) &&
	    !test_thread_local_flags(_TLF_RUNLATCH))
		__ppc64_runlatch_on();
#endif

	interrupt_enter_prepare(regs, state);
	irq_enter();
}

static inline void interrupt_async_exit_prepare(struct pt_regs *regs, struct interrupt_state *state)
{
	/*
	 * Adjust at exit so the main handler sees the true NIA. This must
	 * come before irq_exit() because irq_exit can enable interrupts, and
	 * if another interrupt is taken before nap_adjust_return has run
	 * here, then that interrupt would return directly to idle nap return.
	 */
	nap_adjust_return(regs);

	irq_exit();
	interrupt_exit_prepare(regs, state);
}

struct interrupt_nmi_state {
#ifdef CONFIG_PPC64
	u8 irq_soft_mask;
	u8 irq_happened;
	u8 ftrace_enabled;
#endif
};

static inline bool nmi_disables_ftrace(struct pt_regs *regs)
{
	/* Allow DEC and PMI to be traced when they are soft-NMI */
	if (IS_ENABLED(CONFIG_PPC_BOOK3S_64)) {
		if (TRAP(regs) == INTERRUPT_DECREMENTER)
		       return false;
		if (TRAP(regs) == INTERRUPT_PERFMON)
		       return false;
	}
	if (IS_ENABLED(CONFIG_PPC_BOOK3E)) {
		if (TRAP(regs) == INTERRUPT_PERFMON)
			return false;
	}

	return true;
}

static inline void interrupt_nmi_enter_prepare(struct pt_regs *regs, struct interrupt_nmi_state *state)
{
#ifdef CONFIG_PPC64
	state->irq_soft_mask = local_paca->irq_soft_mask;
	state->irq_happened = local_paca->irq_happened;

	/*
	 * Set IRQS_ALL_DISABLED unconditionally so irqs_disabled() does
	 * the right thing, and set IRQ_HARD_DIS. We do not want to reconcile
	 * because that goes through irq tracing which we don't want in NMI.
	 */
	local_paca->irq_soft_mask = IRQS_ALL_DISABLED;
	local_paca->irq_happened |= PACA_IRQ_HARD_DIS;

	if (IS_ENABLED(CONFIG_PPC_BOOK3S_64) && !(regs->msr & MSR_PR) &&
				regs->nip < (unsigned long)__end_interrupts) {
		// Kernel code running below __end_interrupts is
		// implicitly soft-masked.
		regs->softe = IRQS_ALL_DISABLED;
	}

	/* Don't do any per-CPU operations until interrupt state is fixed */

	if (nmi_disables_ftrace(regs)) {
		state->ftrace_enabled = this_cpu_get_ftrace_enabled();
		this_cpu_set_ftrace_enabled(0);
	}
#endif

	/*
	 * Do not use nmi_enter() for pseries hash guest taking a real-mode
	 * NMI because not everything it touches is within the RMA limit.
	 */
	if (!IS_ENABLED(CONFIG_PPC_BOOK3S_64) ||
			!firmware_has_feature(FW_FEATURE_LPAR) ||
			radix_enabled() || (mfmsr() & MSR_DR))
		nmi_enter();
}

static inline void interrupt_nmi_exit_prepare(struct pt_regs *regs, struct interrupt_nmi_state *state)
{
	if (!IS_ENABLED(CONFIG_PPC_BOOK3S_64) ||
			!firmware_has_feature(FW_FEATURE_LPAR) ||
			radix_enabled() || (mfmsr() & MSR_DR))
		nmi_exit();

	/*
	 * nmi does not call nap_adjust_return because nmi should not create
	 * new work to do (must use irq_work for that).
	 */

#ifdef CONFIG_PPC64
	if (nmi_disables_ftrace(regs))
		this_cpu_set_ftrace_enabled(state->ftrace_enabled);

	/* Check we didn't change the pending interrupt mask. */
	WARN_ON_ONCE((state->irq_happened | PACA_IRQ_HARD_DIS) != local_paca->irq_happened);
	local_paca->irq_happened = state->irq_happened;
	local_paca->irq_soft_mask = state->irq_soft_mask;
#endif
}

/*
 * Don't use noinstr here like x86, but rather add NOKPROBE_SYMBOL to each
 * function definition. The reason for this is the noinstr section is placed
 * after the main text section, i.e., very far away from the interrupt entry
 * asm. That creates problems with fitting linker stubs when building large
 * kernels.
 */
#define interrupt_handler __visible noinline notrace __no_kcsan __no_sanitize_address

/**
 * DECLARE_INTERRUPT_HANDLER_RAW - Declare raw interrupt handler function
 * @func:	Function name of the entry point
 * @returns:	Returns a value back to asm caller
 */
#define DECLARE_INTERRUPT_HANDLER_RAW(func)				\
	__visible long func(struct pt_regs *regs)

/**
 * DEFINE_INTERRUPT_HANDLER_RAW - Define raw interrupt handler function
 * @func:	Function name of the entry point
 * @returns:	Returns a value back to asm caller
 *
 * @func is called from ASM entry code.
 *
 * This is a plain function which does no tracing, reconciling, etc.
 * The macro is written so it acts as function definition. Append the
 * body with a pair of curly brackets.
 *
 * raw interrupt handlers must not enable or disable interrupts, or
 * schedule, tracing and instrumentation (ftrace, lockdep, etc) would
 * not be advisable either, although may be possible in a pinch, the
 * trace will look odd at least.
 *
 * A raw handler may call one of the other interrupt handler functions
 * to be converted into that interrupt context without these restrictions.
 *
 * On PPC64, _RAW handlers may return with fast_interrupt_return.
 *
 * Specific handlers may have additional restrictions.
 */
#define DEFINE_INTERRUPT_HANDLER_RAW(func)				\
static __always_inline long ____##func(struct pt_regs *regs);		\
									\
interrupt_handler long func(struct pt_regs *regs)			\
{									\
	long ret;							\
									\
	ret = ____##func (regs);					\
									\
	return ret;							\
}									\
NOKPROBE_SYMBOL(func);							\
									\
static __always_inline long ____##func(struct pt_regs *regs)

/**
 * DECLARE_INTERRUPT_HANDLER - Declare synchronous interrupt handler function
 * @func:	Function name of the entry point
 */
#define DECLARE_INTERRUPT_HANDLER(func)					\
	__visible void func(struct pt_regs *regs)

/**
 * DEFINE_INTERRUPT_HANDLER - Define synchronous interrupt handler function
 * @func:	Function name of the entry point
 *
 * @func is called from ASM entry code.
 *
 * The macro is written so it acts as function definition. Append the
 * body with a pair of curly brackets.
 */
#define DEFINE_INTERRUPT_HANDLER(func)					\
static __always_inline void ____##func(struct pt_regs *regs);		\
									\
interrupt_handler void func(struct pt_regs *regs)			\
{									\
	struct interrupt_state state;					\
									\
	interrupt_enter_prepare(regs, &state);				\
									\
	____##func (regs);						\
									\
	interrupt_exit_prepare(regs, &state);				\
}									\
NOKPROBE_SYMBOL(func);							\
									\
static __always_inline void ____##func(struct pt_regs *regs)

/**
 * DECLARE_INTERRUPT_HANDLER_RET - Declare synchronous interrupt handler function
 * @func:	Function name of the entry point
 * @returns:	Returns a value back to asm caller
 */
#define DECLARE_INTERRUPT_HANDLER_RET(func)				\
	__visible long func(struct pt_regs *regs)

/**
 * DEFINE_INTERRUPT_HANDLER_RET - Define synchronous interrupt handler function
 * @func:	Function name of the entry point
 * @returns:	Returns a value back to asm caller
 *
 * @func is called from ASM entry code.
 *
 * The macro is written so it acts as function definition. Append the
 * body with a pair of curly brackets.
 */
#define DEFINE_INTERRUPT_HANDLER_RET(func)				\
static __always_inline long ____##func(struct pt_regs *regs);		\
									\
interrupt_handler long func(struct pt_regs *regs)			\
{									\
	struct interrupt_state state;					\
	long ret;							\
									\
	interrupt_enter_prepare(regs, &state);				\
									\
	ret = ____##func (regs);					\
									\
	interrupt_exit_prepare(regs, &state);				\
									\
	return ret;							\
}									\
NOKPROBE_SYMBOL(func);							\
									\
static __always_inline long ____##func(struct pt_regs *regs)

/**
 * DECLARE_INTERRUPT_HANDLER_ASYNC - Declare asynchronous interrupt handler function
 * @func:	Function name of the entry point
 */
#define DECLARE_INTERRUPT_HANDLER_ASYNC(func)				\
	__visible void func(struct pt_regs *regs)

/**
 * DEFINE_INTERRUPT_HANDLER_ASYNC - Define asynchronous interrupt handler function
 * @func:	Function name of the entry point
 *
 * @func is called from ASM entry code.
 *
 * The macro is written so it acts as function definition. Append the
 * body with a pair of curly brackets.
 */
#define DEFINE_INTERRUPT_HANDLER_ASYNC(func)				\
static __always_inline void ____##func(struct pt_regs *regs);		\
									\
interrupt_handler void func(struct pt_regs *regs)			\
{									\
	struct interrupt_state state;					\
									\
	interrupt_async_enter_prepare(regs, &state);			\
									\
	____##func (regs);						\
									\
	interrupt_async_exit_prepare(regs, &state);			\
}									\
NOKPROBE_SYMBOL(func);							\
									\
static __always_inline void ____##func(struct pt_regs *regs)

/**
 * DECLARE_INTERRUPT_HANDLER_NMI - Declare NMI interrupt handler function
 * @func:	Function name of the entry point
 * @returns:	Returns a value back to asm caller
 */
#define DECLARE_INTERRUPT_HANDLER_NMI(func)				\
	__visible long func(struct pt_regs *regs)

/**
 * DEFINE_INTERRUPT_HANDLER_NMI - Define NMI interrupt handler function
 * @func:	Function name of the entry point
 * @returns:	Returns a value back to asm caller
 *
 * @func is called from ASM entry code.
 *
 * The macro is written so it acts as function definition. Append the
 * body with a pair of curly brackets.
 */
#define DEFINE_INTERRUPT_HANDLER_NMI(func)				\
static __always_inline long ____##func(struct pt_regs *regs);		\
									\
interrupt_handler long func(struct pt_regs *regs)			\
{									\
	struct interrupt_nmi_state state;				\
	long ret;							\
									\
	interrupt_nmi_enter_prepare(regs, &state);			\
									\
	ret = ____##func (regs);					\
									\
	interrupt_nmi_exit_prepare(regs, &state);			\
									\
	return ret;							\
}									\
NOKPROBE_SYMBOL(func);							\
									\
static __always_inline long ____##func(struct pt_regs *regs)


/* Interrupt handlers */
/* kernel/traps.c */
DECLARE_INTERRUPT_HANDLER_NMI(system_reset_exception);
#ifdef CONFIG_PPC_BOOK3S_64
DECLARE_INTERRUPT_HANDLER_ASYNC(machine_check_exception);
#else
DECLARE_INTERRUPT_HANDLER_NMI(machine_check_exception);
#endif
DECLARE_INTERRUPT_HANDLER(SMIException);
DECLARE_INTERRUPT_HANDLER(handle_hmi_exception);
DECLARE_INTERRUPT_HANDLER(unknown_exception);
DECLARE_INTERRUPT_HANDLER_ASYNC(unknown_async_exception);
DECLARE_INTERRUPT_HANDLER_NMI(unknown_nmi_exception);
DECLARE_INTERRUPT_HANDLER(instruction_breakpoint_exception);
DECLARE_INTERRUPT_HANDLER(RunModeException);
DECLARE_INTERRUPT_HANDLER(single_step_exception);
DECLARE_INTERRUPT_HANDLER(program_check_exception);
DECLARE_INTERRUPT_HANDLER(emulation_assist_interrupt);
DECLARE_INTERRUPT_HANDLER(alignment_exception);
DECLARE_INTERRUPT_HANDLER(StackOverflow);
DECLARE_INTERRUPT_HANDLER(stack_overflow_exception);
DECLARE_INTERRUPT_HANDLER(kernel_fp_unavailable_exception);
DECLARE_INTERRUPT_HANDLER(altivec_unavailable_exception);
DECLARE_INTERRUPT_HANDLER(vsx_unavailable_exception);
DECLARE_INTERRUPT_HANDLER(facility_unavailable_exception);
DECLARE_INTERRUPT_HANDLER(fp_unavailable_tm);
DECLARE_INTERRUPT_HANDLER(altivec_unavailable_tm);
DECLARE_INTERRUPT_HANDLER(vsx_unavailable_tm);
DECLARE_INTERRUPT_HANDLER_NMI(performance_monitor_exception_nmi);
DECLARE_INTERRUPT_HANDLER_ASYNC(performance_monitor_exception_async);
DECLARE_INTERRUPT_HANDLER_RAW(performance_monitor_exception);
DECLARE_INTERRUPT_HANDLER(DebugException);
DECLARE_INTERRUPT_HANDLER(altivec_assist_exception);
DECLARE_INTERRUPT_HANDLER(CacheLockingException);
DECLARE_INTERRUPT_HANDLER(SPEFloatingPointException);
DECLARE_INTERRUPT_HANDLER(SPEFloatingPointRoundException);
DECLARE_INTERRUPT_HANDLER_NMI(WatchdogException);
DECLARE_INTERRUPT_HANDLER(kernel_bad_stack);

/* slb.c */
DECLARE_INTERRUPT_HANDLER_RAW(do_slb_fault);
DECLARE_INTERRUPT_HANDLER(do_bad_slb_fault);

/* hash_utils.c */
DECLARE_INTERRUPT_HANDLER_RAW(do_hash_fault);

/* fault.c */
DECLARE_INTERRUPT_HANDLER(do_page_fault);
DECLARE_INTERRUPT_HANDLER(do_bad_page_fault_segv);

/* process.c */
DECLARE_INTERRUPT_HANDLER(do_break);

/* time.c */
DECLARE_INTERRUPT_HANDLER_ASYNC(timer_interrupt);

/* mce.c */
DECLARE_INTERRUPT_HANDLER_NMI(machine_check_early);
DECLARE_INTERRUPT_HANDLER_NMI(hmi_exception_realmode);

DECLARE_INTERRUPT_HANDLER_ASYNC(TAUException);

void __noreturn unrecoverable_exception(struct pt_regs *regs);

void replay_system_reset(void);
void replay_soft_interrupts(void);

static inline void interrupt_cond_local_irq_enable(struct pt_regs *regs)
{
	if (!arch_irq_disabled_regs(regs))
		local_irq_enable();
}

#endif /* __ASSEMBLY__ */

#endif /* _ASM_POWERPC_INTERRUPT_H */
