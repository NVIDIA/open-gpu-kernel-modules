// SPDX-License-Identifier: GPL-2.0
/*  arch/sparc64/kernel/signal32.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *  Copyright (C) 1995 David S. Miller (davem@caip.rutgers.edu)
 *  Copyright (C) 1996 Miguel de Icaza (miguel@nuclecu.unam.mx)
 *  Copyright (C) 1997 Eddie C. Dost   (ecd@skynet.be)
 *  Copyright (C) 1997,1998 Jakub Jelinek   (jj@sunsite.mff.cuni.cz)
 */

#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/signal.h>
#include <linux/errno.h>
#include <linux/wait.h>
#include <linux/ptrace.h>
#include <linux/unistd.h>
#include <linux/mm.h>
#include <linux/tty.h>
#include <linux/binfmts.h>
#include <linux/compat.h>
#include <linux/bitops.h>
#include <linux/tracehook.h>

#include <linux/uaccess.h>
#include <asm/ptrace.h>
#include <asm/psrcompat.h>
#include <asm/fpumacro.h>
#include <asm/visasm.h>
#include <asm/compat_signal.h>
#include <asm/switch_to.h>

#include "sigutil.h"
#include "kernel.h"

/* This magic should be in g_upper[0] for all upper parts
 * to be valid.
 */
#define SIGINFO_EXTRA_V8PLUS_MAGIC	0x130e269
typedef struct {
	unsigned int g_upper[8];
	unsigned int o_upper[8];
	unsigned int asi;
} siginfo_extra_v8plus_t;

struct signal_frame32 {
	struct sparc_stackf32	ss;
	__siginfo32_t		info;
	/* __siginfo_fpu_t * */ u32 fpu_save;
	unsigned int		insns[2];
	unsigned int		extramask[_COMPAT_NSIG_WORDS - 1];
	unsigned int		extra_size; /* Should be sizeof(siginfo_extra_v8plus_t) */
	/* Only valid if (info.si_regs.psr & (PSR_VERS|PSR_IMPL)) == PSR_V8PLUS */
	siginfo_extra_v8plus_t	v8plus;
	/* __siginfo_rwin_t * */u32 rwin_save;
} __attribute__((aligned(8)));

struct rt_signal_frame32 {
	struct sparc_stackf32	ss;
	compat_siginfo_t	info;
	struct pt_regs32	regs;
	compat_sigset_t		mask;
	/* __siginfo_fpu_t * */ u32 fpu_save;
	unsigned int		insns[2];
	compat_stack_t		stack;
	unsigned int		extra_size; /* Should be sizeof(siginfo_extra_v8plus_t) */
	/* Only valid if (regs.psr & (PSR_VERS|PSR_IMPL)) == PSR_V8PLUS */
	siginfo_extra_v8plus_t	v8plus;
	/* __siginfo_rwin_t * */u32 rwin_save;
} __attribute__((aligned(8)));

/* Checks if the fp is valid.  We always build signal frames which are
 * 16-byte aligned, therefore we can always enforce that the restore
 * frame has that property as well.
 */
static bool invalid_frame_pointer(void __user *fp, int fplen)
{
	if ((((unsigned long) fp) & 15) ||
	    ((unsigned long)fp) > 0x100000000ULL - fplen)
		return true;
	return false;
}

void do_sigreturn32(struct pt_regs *regs)
{
	struct signal_frame32 __user *sf;
	compat_uptr_t fpu_save;
	compat_uptr_t rwin_save;
	unsigned int psr, ufp;
	unsigned int pc, npc;
	sigset_t set;
	compat_sigset_t seta;
	int err, i;
	
	/* Always make any pending restarted system calls return -EINTR */
	current->restart_block.fn = do_no_restart_syscall;

	synchronize_user_stack();

	regs->u_regs[UREG_FP] &= 0x00000000ffffffffUL;
	sf = (struct signal_frame32 __user *) regs->u_regs[UREG_FP];

	/* 1. Make sure we are not getting garbage from the user */
	if (invalid_frame_pointer(sf, sizeof(*sf)))
		goto segv;

	if (get_user(ufp, &sf->info.si_regs.u_regs[UREG_FP]))
		goto segv;

	if (ufp & 0x7)
		goto segv;

	if (__get_user(pc, &sf->info.si_regs.pc) ||
	    __get_user(npc, &sf->info.si_regs.npc))
		goto segv;

	if ((pc | npc) & 3)
		goto segv;

	if (test_thread_flag(TIF_32BIT)) {
		pc &= 0xffffffff;
		npc &= 0xffffffff;
	}
	regs->tpc = pc;
	regs->tnpc = npc;

	/* 2. Restore the state */
	err = __get_user(regs->y, &sf->info.si_regs.y);
	err |= __get_user(psr, &sf->info.si_regs.psr);

	for (i = UREG_G1; i <= UREG_I7; i++)
		err |= __get_user(regs->u_regs[i], &sf->info.si_regs.u_regs[i]);
	if ((psr & (PSR_VERS|PSR_IMPL)) == PSR_V8PLUS) {
		err |= __get_user(i, &sf->v8plus.g_upper[0]);
		if (i == SIGINFO_EXTRA_V8PLUS_MAGIC) {
			unsigned long asi;

			for (i = UREG_G1; i <= UREG_I7; i++)
				err |= __get_user(((u32 *)regs->u_regs)[2*i], &sf->v8plus.g_upper[i]);
			err |= __get_user(asi, &sf->v8plus.asi);
			regs->tstate &= ~TSTATE_ASI;
			regs->tstate |= ((asi & 0xffUL) << 24UL);
		}
	}

	/* User can only change condition codes in %tstate. */
	regs->tstate &= ~(TSTATE_ICC|TSTATE_XCC);
	regs->tstate |= psr_to_tstate_icc(psr);

	/* Prevent syscall restart.  */
	pt_regs_clear_syscall(regs);

	err |= __get_user(fpu_save, &sf->fpu_save);
	if (!err && fpu_save)
		err |= restore_fpu_state(regs, compat_ptr(fpu_save));
	err |= __get_user(rwin_save, &sf->rwin_save);
	if (!err && rwin_save) {
		if (restore_rwin_state(compat_ptr(rwin_save)))
			goto segv;
	}
	err |= __get_user(seta.sig[0], &sf->info.si_mask);
	err |= copy_from_user(&seta.sig[1], &sf->extramask,
			      (_COMPAT_NSIG_WORDS - 1) * sizeof(unsigned int));
	if (err)
	    	goto segv;

	set.sig[0] = seta.sig[0] + (((long)seta.sig[1]) << 32);
	set_current_blocked(&set);
	return;

segv:
	force_sig(SIGSEGV);
}

asmlinkage void do_rt_sigreturn32(struct pt_regs *regs)
{
	struct rt_signal_frame32 __user *sf;
	unsigned int psr, pc, npc, ufp;
	compat_uptr_t fpu_save;
	compat_uptr_t rwin_save;
	sigset_t set;
	int err, i;
	
	/* Always make any pending restarted system calls return -EINTR */
	current->restart_block.fn = do_no_restart_syscall;

	synchronize_user_stack();
	regs->u_regs[UREG_FP] &= 0x00000000ffffffffUL;
	sf = (struct rt_signal_frame32 __user *) regs->u_regs[UREG_FP];

	/* 1. Make sure we are not getting garbage from the user */
	if (invalid_frame_pointer(sf, sizeof(*sf)))
		goto segv;

	if (get_user(ufp, &sf->regs.u_regs[UREG_FP]))
		goto segv;

	if (ufp & 0x7)
		goto segv;

	if (__get_user(pc, &sf->regs.pc) || 
	    __get_user(npc, &sf->regs.npc))
		goto segv;

	if ((pc | npc) & 3)
		goto segv;

	if (test_thread_flag(TIF_32BIT)) {
		pc &= 0xffffffff;
		npc &= 0xffffffff;
	}
	regs->tpc = pc;
	regs->tnpc = npc;

	/* 2. Restore the state */
	err = __get_user(regs->y, &sf->regs.y);
	err |= __get_user(psr, &sf->regs.psr);
	
	for (i = UREG_G1; i <= UREG_I7; i++)
		err |= __get_user(regs->u_regs[i], &sf->regs.u_regs[i]);
	if ((psr & (PSR_VERS|PSR_IMPL)) == PSR_V8PLUS) {
		err |= __get_user(i, &sf->v8plus.g_upper[0]);
		if (i == SIGINFO_EXTRA_V8PLUS_MAGIC) {
			unsigned long asi;

			for (i = UREG_G1; i <= UREG_I7; i++)
				err |= __get_user(((u32 *)regs->u_regs)[2*i], &sf->v8plus.g_upper[i]);
			err |= __get_user(asi, &sf->v8plus.asi);
			regs->tstate &= ~TSTATE_ASI;
			regs->tstate |= ((asi & 0xffUL) << 24UL);
		}
	}

	/* User can only change condition codes in %tstate. */
	regs->tstate &= ~(TSTATE_ICC|TSTATE_XCC);
	regs->tstate |= psr_to_tstate_icc(psr);

	/* Prevent syscall restart.  */
	pt_regs_clear_syscall(regs);

	err |= __get_user(fpu_save, &sf->fpu_save);
	if (!err && fpu_save)
		err |= restore_fpu_state(regs, compat_ptr(fpu_save));
	err |= get_compat_sigset(&set, &sf->mask);
	err |= compat_restore_altstack(&sf->stack);
	if (err)
		goto segv;
		
	err |= __get_user(rwin_save, &sf->rwin_save);
	if (!err && rwin_save) {
		if (restore_rwin_state(compat_ptr(rwin_save)))
			goto segv;
	}

	set_current_blocked(&set);
	return;
segv:
	force_sig(SIGSEGV);
}

static void __user *get_sigframe(struct ksignal *ksig, struct pt_regs *regs, unsigned long framesize)
{
	unsigned long sp;
	
	regs->u_regs[UREG_FP] &= 0x00000000ffffffffUL;
	sp = regs->u_regs[UREG_FP];
	
	/*
	 * If we are on the alternate signal stack and would overflow it, don't.
	 * Return an always-bogus address instead so we will die with SIGSEGV.
	 */
	if (on_sig_stack(sp) && !likely(on_sig_stack(sp - framesize)))
		return (void __user *) -1L;

	/* This is the X/Open sanctioned signal stack switching.  */
	sp = sigsp(sp, ksig) - framesize;

	/* Always align the stack frame.  This handles two cases.  First,
	 * sigaltstack need not be mindful of platform specific stack
	 * alignment.  Second, if we took this signal because the stack
	 * is not aligned properly, we'd like to take the signal cleanly
	 * and report that.
	 */
	sp &= ~15UL;

	return (void __user *) sp;
}

/* The I-cache flush instruction only works in the primary ASI, which
 * right now is the nucleus, aka. kernel space.
 *
 * Therefore we have to kick the instructions out using the kernel
 * side linear mapping of the physical address backing the user
 * instructions.
 */
static void flush_signal_insns(unsigned long address)
{
	unsigned long pstate, paddr;
	pte_t *ptep, pte;
	pgd_t *pgdp;
	p4d_t *p4dp;
	pud_t *pudp;
	pmd_t *pmdp;

	/* Commit all stores of the instructions we are about to flush.  */
	wmb();

	/* Disable cross-call reception.  In this way even a very wide
	 * munmap() on another cpu can't tear down the page table
	 * hierarchy from underneath us, since that can't complete
	 * until the IPI tlb flush returns.
	 */

	__asm__ __volatile__("rdpr %%pstate, %0" : "=r" (pstate));
	__asm__ __volatile__("wrpr %0, %1, %%pstate"
				: : "r" (pstate), "i" (PSTATE_IE));

	pgdp = pgd_offset(current->mm, address);
	if (pgd_none(*pgdp))
		goto out_irqs_on;
	p4dp = p4d_offset(pgdp, address);
	if (p4d_none(*p4dp))
		goto out_irqs_on;
	pudp = pud_offset(p4dp, address);
	if (pud_none(*pudp))
		goto out_irqs_on;
	pmdp = pmd_offset(pudp, address);
	if (pmd_none(*pmdp))
		goto out_irqs_on;

	ptep = pte_offset_map(pmdp, address);
	pte = *ptep;
	if (!pte_present(pte))
		goto out_unmap;

	paddr = (unsigned long) page_address(pte_page(pte));

	__asm__ __volatile__("flush	%0 + %1"
			     : /* no outputs */
			     : "r" (paddr),
			       "r" (address & (PAGE_SIZE - 1))
			     : "memory");

out_unmap:
	pte_unmap(ptep);
out_irqs_on:
	__asm__ __volatile__("wrpr %0, 0x0, %%pstate" : : "r" (pstate));

}

static int setup_frame32(struct ksignal *ksig, struct pt_regs *regs,
			 sigset_t *oldset)
{
	struct signal_frame32 __user *sf;
	int i, err, wsaved;
	void __user *tail;
	int sigframe_size;
	u32 psr;
	compat_sigset_t seta;

	/* 1. Make sure everything is clean */
	synchronize_user_stack();
	save_and_clear_fpu();
	
	wsaved = get_thread_wsaved();

	sigframe_size = sizeof(*sf);
	if (current_thread_info()->fpsaved[0] & FPRS_FEF)
		sigframe_size += sizeof(__siginfo_fpu_t);
	if (wsaved)
		sigframe_size += sizeof(__siginfo_rwin_t);

	sf = (struct signal_frame32 __user *)
		get_sigframe(ksig, regs, sigframe_size);
	
	if (invalid_frame_pointer(sf, sigframe_size)) {
		if (show_unhandled_signals)
			pr_info("%s[%d] bad frame in setup_frame32: %08lx TPC %08lx O7 %08lx\n",
				current->comm, current->pid, (unsigned long)sf,
				regs->tpc, regs->u_regs[UREG_I7]);
		force_sigsegv(ksig->sig);
		return -EINVAL;
	}

	tail = (sf + 1);

	/* 2. Save the current process state */
	if (test_thread_flag(TIF_32BIT)) {
		regs->tpc &= 0xffffffff;
		regs->tnpc &= 0xffffffff;
	}
	err  = put_user(regs->tpc, &sf->info.si_regs.pc);
	err |= __put_user(regs->tnpc, &sf->info.si_regs.npc);
	err |= __put_user(regs->y, &sf->info.si_regs.y);
	psr = tstate_to_psr(regs->tstate);
	if (current_thread_info()->fpsaved[0] & FPRS_FEF)
		psr |= PSR_EF;
	err |= __put_user(psr, &sf->info.si_regs.psr);
	for (i = 0; i < 16; i++)
		err |= __put_user(regs->u_regs[i], &sf->info.si_regs.u_regs[i]);
	err |= __put_user(sizeof(siginfo_extra_v8plus_t), &sf->extra_size);
	err |= __put_user(SIGINFO_EXTRA_V8PLUS_MAGIC, &sf->v8plus.g_upper[0]);
	for (i = 1; i < 16; i++)
		err |= __put_user(((u32 *)regs->u_regs)[2*i],
				  &sf->v8plus.g_upper[i]);
	err |= __put_user((regs->tstate & TSTATE_ASI) >> 24UL,
			  &sf->v8plus.asi);

	if (psr & PSR_EF) {
		__siginfo_fpu_t __user *fp = tail;
		tail += sizeof(*fp);
		err |= save_fpu_state(regs, fp);
		err |= __put_user((u64)fp, &sf->fpu_save);
	} else {
		err |= __put_user(0, &sf->fpu_save);
	}
	if (wsaved) {
		__siginfo_rwin_t __user *rwp = tail;
		tail += sizeof(*rwp);
		err |= save_rwin_state(wsaved, rwp);
		err |= __put_user((u64)rwp, &sf->rwin_save);
		set_thread_wsaved(0);
	} else {
		err |= __put_user(0, &sf->rwin_save);
	}

	/* If these change we need to know - assignments to seta relies on these sizes */
	BUILD_BUG_ON(_NSIG_WORDS != 1);
	BUILD_BUG_ON(_COMPAT_NSIG_WORDS != 2);
	seta.sig[1] = (oldset->sig[0] >> 32);
	seta.sig[0] = oldset->sig[0];

	err |= __put_user(seta.sig[0], &sf->info.si_mask);
	err |= __copy_to_user(sf->extramask, &seta.sig[1],
			      (_COMPAT_NSIG_WORDS - 1) * sizeof(unsigned int));

	if (!wsaved) {
		err |= copy_in_user((u32 __user *)sf,
				    (u32 __user *)(regs->u_regs[UREG_FP]),
				    sizeof(struct reg_window32));
	} else {
		struct reg_window *rp;

		rp = &current_thread_info()->reg_window[wsaved - 1];
		for (i = 0; i < 8; i++)
			err |= __put_user(rp->locals[i], &sf->ss.locals[i]);
		for (i = 0; i < 6; i++)
			err |= __put_user(rp->ins[i], &sf->ss.ins[i]);
		err |= __put_user(rp->ins[6], &sf->ss.fp);
		err |= __put_user(rp->ins[7], &sf->ss.callers_pc);
	}	
	if (err)
		return err;

	/* 3. signal handler back-trampoline and parameters */
	regs->u_regs[UREG_FP] = (unsigned long) sf;
	regs->u_regs[UREG_I0] = ksig->sig;
	regs->u_regs[UREG_I1] = (unsigned long) &sf->info;
	regs->u_regs[UREG_I2] = (unsigned long) &sf->info;

	/* 4. signal handler */
	regs->tpc = (unsigned long) ksig->ka.sa.sa_handler;
	regs->tnpc = (regs->tpc + 4);
	if (test_thread_flag(TIF_32BIT)) {
		regs->tpc &= 0xffffffff;
		regs->tnpc &= 0xffffffff;
	}

	/* 5. return to kernel instructions */
	if (ksig->ka.ka_restorer) {
		regs->u_regs[UREG_I7] = (unsigned long)ksig->ka.ka_restorer;
	} else {
		unsigned long address = ((unsigned long)&(sf->insns[0]));

		regs->u_regs[UREG_I7] = (unsigned long) (&(sf->insns[0]) - 2);
	
		err  = __put_user(0x821020d8, &sf->insns[0]); /*mov __NR_sigreturn, %g1*/
		err |= __put_user(0x91d02010, &sf->insns[1]); /*t 0x10*/
		if (err)
			return err;
		flush_signal_insns(address);
	}
	return 0;
}

static int setup_rt_frame32(struct ksignal *ksig, struct pt_regs *regs,
			    sigset_t *oldset)
{
	struct rt_signal_frame32 __user *sf;
	int i, err, wsaved;
	void __user *tail;
	int sigframe_size;
	u32 psr;

	/* 1. Make sure everything is clean */
	synchronize_user_stack();
	save_and_clear_fpu();
	
	wsaved = get_thread_wsaved();

	sigframe_size = sizeof(*sf);
	if (current_thread_info()->fpsaved[0] & FPRS_FEF)
		sigframe_size += sizeof(__siginfo_fpu_t);
	if (wsaved)
		sigframe_size += sizeof(__siginfo_rwin_t);

	sf = (struct rt_signal_frame32 __user *)
		get_sigframe(ksig, regs, sigframe_size);
	
	if (invalid_frame_pointer(sf, sigframe_size)) {
		if (show_unhandled_signals)
			pr_info("%s[%d] bad frame in setup_rt_frame32: %08lx TPC %08lx O7 %08lx\n",
				current->comm, current->pid, (unsigned long)sf,
				regs->tpc, regs->u_regs[UREG_I7]);
		force_sigsegv(ksig->sig);
		return -EINVAL;
	}

	tail = (sf + 1);

	/* 2. Save the current process state */
	if (test_thread_flag(TIF_32BIT)) {
		regs->tpc &= 0xffffffff;
		regs->tnpc &= 0xffffffff;
	}
	err  = put_user(regs->tpc, &sf->regs.pc);
	err |= __put_user(regs->tnpc, &sf->regs.npc);
	err |= __put_user(regs->y, &sf->regs.y);
	psr = tstate_to_psr(regs->tstate);
	if (current_thread_info()->fpsaved[0] & FPRS_FEF)
		psr |= PSR_EF;
	err |= __put_user(psr, &sf->regs.psr);
	for (i = 0; i < 16; i++)
		err |= __put_user(regs->u_regs[i], &sf->regs.u_regs[i]);
	err |= __put_user(sizeof(siginfo_extra_v8plus_t), &sf->extra_size);
	err |= __put_user(SIGINFO_EXTRA_V8PLUS_MAGIC, &sf->v8plus.g_upper[0]);
	for (i = 1; i < 16; i++)
		err |= __put_user(((u32 *)regs->u_regs)[2*i],
				  &sf->v8plus.g_upper[i]);
	err |= __put_user((regs->tstate & TSTATE_ASI) >> 24UL,
			  &sf->v8plus.asi);

	if (psr & PSR_EF) {
		__siginfo_fpu_t __user *fp = tail;
		tail += sizeof(*fp);
		err |= save_fpu_state(regs, fp);
		err |= __put_user((u64)fp, &sf->fpu_save);
	} else {
		err |= __put_user(0, &sf->fpu_save);
	}
	if (wsaved) {
		__siginfo_rwin_t __user *rwp = tail;
		tail += sizeof(*rwp);
		err |= save_rwin_state(wsaved, rwp);
		err |= __put_user((u64)rwp, &sf->rwin_save);
		set_thread_wsaved(0);
	} else {
		err |= __put_user(0, &sf->rwin_save);
	}

	/* Update the siginfo structure.  */
	err |= copy_siginfo_to_user32(&sf->info, &ksig->info);
	
	/* Setup sigaltstack */
	err |= __compat_save_altstack(&sf->stack, regs->u_regs[UREG_FP]);

	err |= put_compat_sigset(&sf->mask, oldset, sizeof(compat_sigset_t));

	if (!wsaved) {
		err |= copy_in_user((u32 __user *)sf,
				    (u32 __user *)(regs->u_regs[UREG_FP]),
				    sizeof(struct reg_window32));
	} else {
		struct reg_window *rp;

		rp = &current_thread_info()->reg_window[wsaved - 1];
		for (i = 0; i < 8; i++)
			err |= __put_user(rp->locals[i], &sf->ss.locals[i]);
		for (i = 0; i < 6; i++)
			err |= __put_user(rp->ins[i], &sf->ss.ins[i]);
		err |= __put_user(rp->ins[6], &sf->ss.fp);
		err |= __put_user(rp->ins[7], &sf->ss.callers_pc);
	}
	if (err)
		return err;
	
	/* 3. signal handler back-trampoline and parameters */
	regs->u_regs[UREG_FP] = (unsigned long) sf;
	regs->u_regs[UREG_I0] = ksig->sig;
	regs->u_regs[UREG_I1] = (unsigned long) &sf->info;
	regs->u_regs[UREG_I2] = (unsigned long) &sf->regs;

	/* 4. signal handler */
	regs->tpc = (unsigned long) ksig->ka.sa.sa_handler;
	regs->tnpc = (regs->tpc + 4);
	if (test_thread_flag(TIF_32BIT)) {
		regs->tpc &= 0xffffffff;
		regs->tnpc &= 0xffffffff;
	}

	/* 5. return to kernel instructions */
	if (ksig->ka.ka_restorer)
		regs->u_regs[UREG_I7] = (unsigned long)ksig->ka.ka_restorer;
	else {
		unsigned long address = ((unsigned long)&(sf->insns[0]));

		regs->u_regs[UREG_I7] = (unsigned long) (&(sf->insns[0]) - 2);
	
		/* mov __NR_rt_sigreturn, %g1 */
		err |= __put_user(0x82102065, &sf->insns[0]);

		/* t 0x10 */
		err |= __put_user(0x91d02010, &sf->insns[1]);
		if (err)
			return err;

		flush_signal_insns(address);
	}
	return 0;
}

static inline void handle_signal32(struct ksignal *ksig, 
				  struct pt_regs *regs)
{
	sigset_t *oldset = sigmask_to_save();
	int err;

	if (ksig->ka.sa.sa_flags & SA_SIGINFO)
		err = setup_rt_frame32(ksig, regs, oldset);
	else
		err = setup_frame32(ksig, regs, oldset);

	signal_setup_done(err, ksig, 0);
}

static inline void syscall_restart32(unsigned long orig_i0, struct pt_regs *regs,
				     struct sigaction *sa)
{
	switch (regs->u_regs[UREG_I0]) {
	case ERESTART_RESTARTBLOCK:
	case ERESTARTNOHAND:
	no_system_call_restart:
		regs->u_regs[UREG_I0] = EINTR;
		regs->tstate |= TSTATE_ICARRY;
		break;
	case ERESTARTSYS:
		if (!(sa->sa_flags & SA_RESTART))
			goto no_system_call_restart;
		fallthrough;
	case ERESTARTNOINTR:
		regs->u_regs[UREG_I0] = orig_i0;
		regs->tpc -= 4;
		regs->tnpc -= 4;
	}
}

/* Note that 'init' is a special process: it doesn't get signals it doesn't
 * want to handle. Thus you cannot kill init even with a SIGKILL even by
 * mistake.
 */
void do_signal32(struct pt_regs * regs)
{
	struct ksignal ksig;
	unsigned long orig_i0 = 0;
	int restart_syscall = 0;
	bool has_handler = get_signal(&ksig);

	if (pt_regs_is_syscall(regs) &&
	    (regs->tstate & (TSTATE_XCARRY | TSTATE_ICARRY))) {
		restart_syscall = 1;
		orig_i0 = regs->u_regs[UREG_G6];
	}

	if (has_handler) {
		if (restart_syscall)
			syscall_restart32(orig_i0, regs, &ksig.ka.sa);
		handle_signal32(&ksig, regs);
	} else {
		if (restart_syscall) {
			switch (regs->u_regs[UREG_I0]) {
			case ERESTARTNOHAND:
	     		case ERESTARTSYS:
			case ERESTARTNOINTR:
				/* replay the system call when we are done */
				regs->u_regs[UREG_I0] = orig_i0;
				regs->tpc -= 4;
				regs->tnpc -= 4;
				pt_regs_clear_syscall(regs);
				fallthrough;
			case ERESTART_RESTARTBLOCK:
				regs->u_regs[UREG_G1] = __NR_restart_syscall;
				regs->tpc -= 4;
				regs->tnpc -= 4;
				pt_regs_clear_syscall(regs);
			}
		}
		restore_saved_sigmask();
	}
}

struct sigstack32 {
	u32 the_stack;
	int cur_status;
};

asmlinkage int do_sys32_sigstack(u32 u_ssptr, u32 u_ossptr, unsigned long sp)
{
	struct sigstack32 __user *ssptr =
		(struct sigstack32 __user *)((unsigned long)(u_ssptr));
	struct sigstack32 __user *ossptr =
		(struct sigstack32 __user *)((unsigned long)(u_ossptr));
	int ret = -EFAULT;

	/* First see if old state is wanted. */
	if (ossptr) {
		if (put_user(current->sas_ss_sp + current->sas_ss_size,
			     &ossptr->the_stack) ||
		    __put_user(on_sig_stack(sp), &ossptr->cur_status))
			goto out;
	}
	
	/* Now see if we want to update the new state. */
	if (ssptr) {
		u32 ss_sp;

		if (get_user(ss_sp, &ssptr->the_stack))
			goto out;

		/* If the current stack was set with sigaltstack, don't
		 * swap stacks while we are on it.
		 */
		ret = -EPERM;
		if (current->sas_ss_sp && on_sig_stack(sp))
			goto out;
			
		/* Since we don't know the extent of the stack, and we don't
		 * track onstack-ness, but rather calculate it, we must
		 * presume a size.  Ho hum this interface is lossy.
		 */
		current->sas_ss_sp = (unsigned long)ss_sp - SIGSTKSZ;
		current->sas_ss_size = SIGSTKSZ;
	}
	
	ret = 0;
out:
	return ret;
}
