// SPDX-License-Identifier: GPL-2.0
/*
 *  linux/arch/sh/kernel/signal.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 *  1997-11-28  Modified for POSIX.1b signals by Richard Henderson
 *
 *  SuperH version:  Copyright (C) 1999, 2000  Niibe Yutaka & Kaz Kojima
 *
 */
#include <linux/sched.h>
#include <linux/sched/task_stack.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/kernel.h>
#include <linux/signal.h>
#include <linux/errno.h>
#include <linux/wait.h>
#include <linux/ptrace.h>
#include <linux/unistd.h>
#include <linux/stddef.h>
#include <linux/tty.h>
#include <linux/elf.h>
#include <linux/personality.h>
#include <linux/binfmts.h>
#include <linux/io.h>
#include <linux/tracehook.h>
#include <asm/ucontext.h>
#include <linux/uaccess.h>
#include <asm/cacheflush.h>
#include <asm/syscalls.h>
#include <asm/fpu.h>

struct fdpic_func_descriptor {
	unsigned long	text;
	unsigned long	GOT;
};

/*
 * The following define adds a 64 byte gap between the signal
 * stack frame and previous contents of the stack.  This allows
 * frame unwinding in a function epilogue but only if a frame
 * pointer is used in the function.  This is necessary because
 * current gcc compilers (<4.3) do not generate unwind info on
 * SH for function epilogues.
 */
#define UNWINDGUARD 64

/*
 * Do a signal return; undo the signal stack.
 */

#define MOVW(n)	 (0x9300|((n)-2))	/* Move mem word at PC+n to R3 */
#if defined(CONFIG_CPU_SH2)
#define TRAP_NOARG 0xc320		/* Syscall w/no args (NR in R3) */
#else
#define TRAP_NOARG 0xc310		/* Syscall w/no args (NR in R3) */
#endif
#define OR_R0_R0 0x200b			/* or r0,r0 (insert to avoid hardware bug) */

struct sigframe
{
	struct sigcontext sc;
	unsigned long extramask[_NSIG_WORDS-1];
	u16 retcode[8];
};

struct rt_sigframe
{
	struct siginfo info;
	struct ucontext uc;
	u16 retcode[8];
};

#ifdef CONFIG_SH_FPU
static inline int restore_sigcontext_fpu(struct sigcontext __user *sc)
{
	struct task_struct *tsk = current;

	if (!(boot_cpu_data.flags & CPU_HAS_FPU))
		return 0;

	set_used_math();
	return __copy_from_user(&tsk->thread.xstate->hardfpu, &sc->sc_fpregs[0],
				sizeof(long)*(16*2+2));
}

static inline int save_sigcontext_fpu(struct sigcontext __user *sc,
				      struct pt_regs *regs)
{
	struct task_struct *tsk = current;

	if (!(boot_cpu_data.flags & CPU_HAS_FPU))
		return 0;

	if (!used_math())
		return __put_user(0, &sc->sc_ownedfp);

	if (__put_user(1, &sc->sc_ownedfp))
		return -EFAULT;

	/* This will cause a "finit" to be triggered by the next
	   attempted FPU operation by the 'current' process.
	   */
	clear_used_math();

	unlazy_fpu(tsk, regs);
	return __copy_to_user(&sc->sc_fpregs[0], &tsk->thread.xstate->hardfpu,
			      sizeof(long)*(16*2+2));
}
#endif /* CONFIG_SH_FPU */

static int
restore_sigcontext(struct pt_regs *regs, struct sigcontext __user *sc, int *r0_p)
{
	unsigned int err = 0;

#define COPY(x)		err |= __get_user(regs->x, &sc->sc_##x)
			COPY(regs[1]);
	COPY(regs[2]);	COPY(regs[3]);
	COPY(regs[4]);	COPY(regs[5]);
	COPY(regs[6]);	COPY(regs[7]);
	COPY(regs[8]);	COPY(regs[9]);
	COPY(regs[10]);	COPY(regs[11]);
	COPY(regs[12]);	COPY(regs[13]);
	COPY(regs[14]);	COPY(regs[15]);
	COPY(gbr);	COPY(mach);
	COPY(macl);	COPY(pr);
	COPY(sr);	COPY(pc);
#undef COPY

#ifdef CONFIG_SH_FPU
	if (boot_cpu_data.flags & CPU_HAS_FPU) {
		int owned_fp;
		struct task_struct *tsk = current;

		regs->sr |= SR_FD; /* Release FPU */
		clear_fpu(tsk, regs);
		clear_used_math();
		err |= __get_user (owned_fp, &sc->sc_ownedfp);
		if (owned_fp)
			err |= restore_sigcontext_fpu(sc);
	}
#endif

	regs->tra = -1;		/* disable syscall checks */
	err |= __get_user(*r0_p, &sc->sc_regs[0]);
	return err;
}

asmlinkage int sys_sigreturn(void)
{
	struct pt_regs *regs = current_pt_regs();
	struct sigframe __user *frame = (struct sigframe __user *)regs->regs[15];
	sigset_t set;
	int r0;

        /* Always make any pending restarted system calls return -EINTR */
	current->restart_block.fn = do_no_restart_syscall;

	if (!access_ok(frame, sizeof(*frame)))
		goto badframe;

	if (__get_user(set.sig[0], &frame->sc.oldmask)
	    || (_NSIG_WORDS > 1
		&& __copy_from_user(&set.sig[1], &frame->extramask,
				    sizeof(frame->extramask))))
		goto badframe;

	set_current_blocked(&set);

	if (restore_sigcontext(regs, &frame->sc, &r0))
		goto badframe;
	return r0;

badframe:
	force_sig(SIGSEGV);
	return 0;
}

asmlinkage int sys_rt_sigreturn(void)
{
	struct pt_regs *regs = current_pt_regs();
	struct rt_sigframe __user *frame = (struct rt_sigframe __user *)regs->regs[15];
	sigset_t set;
	int r0;

	/* Always make any pending restarted system calls return -EINTR */
	current->restart_block.fn = do_no_restart_syscall;

	if (!access_ok(frame, sizeof(*frame)))
		goto badframe;

	if (__copy_from_user(&set, &frame->uc.uc_sigmask, sizeof(set)))
		goto badframe;

	set_current_blocked(&set);

	if (restore_sigcontext(regs, &frame->uc.uc_mcontext, &r0))
		goto badframe;

	if (restore_altstack(&frame->uc.uc_stack))
		goto badframe;

	return r0;

badframe:
	force_sig(SIGSEGV);
	return 0;
}

/*
 * Set up a signal frame.
 */

static int
setup_sigcontext(struct sigcontext __user *sc, struct pt_regs *regs,
		 unsigned long mask)
{
	int err = 0;

#define COPY(x)		err |= __put_user(regs->x, &sc->sc_##x)
	COPY(regs[0]);	COPY(regs[1]);
	COPY(regs[2]);	COPY(regs[3]);
	COPY(regs[4]);	COPY(regs[5]);
	COPY(regs[6]);	COPY(regs[7]);
	COPY(regs[8]);	COPY(regs[9]);
	COPY(regs[10]);	COPY(regs[11]);
	COPY(regs[12]);	COPY(regs[13]);
	COPY(regs[14]);	COPY(regs[15]);
	COPY(gbr);	COPY(mach);
	COPY(macl);	COPY(pr);
	COPY(sr);	COPY(pc);
#undef COPY

#ifdef CONFIG_SH_FPU
	err |= save_sigcontext_fpu(sc, regs);
#endif

	/* non-iBCS2 extensions.. */
	err |= __put_user(mask, &sc->oldmask);

	return err;
}

/*
 * Determine which stack to use..
 */
static inline void __user *
get_sigframe(struct k_sigaction *ka, unsigned long sp, size_t frame_size)
{
	if (ka->sa.sa_flags & SA_ONSTACK) {
		if (sas_ss_flags(sp) == 0)
			sp = current->sas_ss_sp + current->sas_ss_size;
	}

	return (void __user *)((sp - (frame_size+UNWINDGUARD)) & -8ul);
}

/* These symbols are defined with the addresses in the vsyscall page.
   See vsyscall-trapa.S.  */
extern void __kernel_sigreturn(void);
extern void __kernel_rt_sigreturn(void);

static int setup_frame(struct ksignal *ksig, sigset_t *set,
		       struct pt_regs *regs)
{
	struct sigframe __user *frame;
	int err = 0, sig = ksig->sig;

	frame = get_sigframe(&ksig->ka, regs->regs[15], sizeof(*frame));

	if (!access_ok(frame, sizeof(*frame)))
		return -EFAULT;

	err |= setup_sigcontext(&frame->sc, regs, set->sig[0]);

	if (_NSIG_WORDS > 1)
		err |= __copy_to_user(frame->extramask, &set->sig[1],
				      sizeof(frame->extramask));

	/* Set up to return from userspace.  If provided, use a stub
	   already in userspace.  */
	if (ksig->ka.sa.sa_flags & SA_RESTORER) {
		regs->pr = (unsigned long) ksig->ka.sa.sa_restorer;
#ifdef CONFIG_VSYSCALL
	} else if (likely(current->mm->context.vdso)) {
		regs->pr = VDSO_SYM(&__kernel_sigreturn);
#endif
	} else {
		/* Generate return code (system call to sigreturn) */
		err |= __put_user(MOVW(7), &frame->retcode[0]);
		err |= __put_user(TRAP_NOARG, &frame->retcode[1]);
		err |= __put_user(OR_R0_R0, &frame->retcode[2]);
		err |= __put_user(OR_R0_R0, &frame->retcode[3]);
		err |= __put_user(OR_R0_R0, &frame->retcode[4]);
		err |= __put_user(OR_R0_R0, &frame->retcode[5]);
		err |= __put_user(OR_R0_R0, &frame->retcode[6]);
		err |= __put_user((__NR_sigreturn), &frame->retcode[7]);
		regs->pr = (unsigned long) frame->retcode;
		flush_icache_range(regs->pr, regs->pr + sizeof(frame->retcode));
	}

	if (err)
		return -EFAULT;

	/* Set up registers for signal handler */
	regs->regs[15] = (unsigned long) frame;
	regs->regs[4] = sig; /* Arg for signal handler */
	regs->regs[5] = 0;
	regs->regs[6] = (unsigned long) &frame->sc;

	if (current->personality & FDPIC_FUNCPTRS) {
		struct fdpic_func_descriptor __user *funcptr =
			(struct fdpic_func_descriptor __user *)ksig->ka.sa.sa_handler;

		err |= __get_user(regs->pc, &funcptr->text);
		err |= __get_user(regs->regs[12], &funcptr->GOT);
	} else
		regs->pc = (unsigned long)ksig->ka.sa.sa_handler;

	if (err)
		return -EFAULT;

	pr_debug("SIG deliver (%s:%d): sp=%p pc=%08lx pr=%08lx\n",
		 current->comm, task_pid_nr(current), frame, regs->pc, regs->pr);

	return 0;
}

static int setup_rt_frame(struct ksignal *ksig, sigset_t *set,
			  struct pt_regs *regs)
{
	struct rt_sigframe __user *frame;
	int err = 0, sig = ksig->sig;

	frame = get_sigframe(&ksig->ka, regs->regs[15], sizeof(*frame));

	if (!access_ok(frame, sizeof(*frame)))
		return -EFAULT;

	err |= copy_siginfo_to_user(&frame->info, &ksig->info);

	/* Create the ucontext.  */
	err |= __put_user(0, &frame->uc.uc_flags);
	err |= __put_user(NULL, &frame->uc.uc_link);
	err |= __save_altstack(&frame->uc.uc_stack, regs->regs[15]);
	err |= setup_sigcontext(&frame->uc.uc_mcontext,
			        regs, set->sig[0]);
	err |= __copy_to_user(&frame->uc.uc_sigmask, set, sizeof(*set));

	/* Set up to return from userspace.  If provided, use a stub
	   already in userspace.  */
	if (ksig->ka.sa.sa_flags & SA_RESTORER) {
		regs->pr = (unsigned long) ksig->ka.sa.sa_restorer;
#ifdef CONFIG_VSYSCALL
	} else if (likely(current->mm->context.vdso)) {
		regs->pr = VDSO_SYM(&__kernel_rt_sigreturn);
#endif
	} else {
		/* Generate return code (system call to rt_sigreturn) */
		err |= __put_user(MOVW(7), &frame->retcode[0]);
		err |= __put_user(TRAP_NOARG, &frame->retcode[1]);
		err |= __put_user(OR_R0_R0, &frame->retcode[2]);
		err |= __put_user(OR_R0_R0, &frame->retcode[3]);
		err |= __put_user(OR_R0_R0, &frame->retcode[4]);
		err |= __put_user(OR_R0_R0, &frame->retcode[5]);
		err |= __put_user(OR_R0_R0, &frame->retcode[6]);
		err |= __put_user((__NR_rt_sigreturn), &frame->retcode[7]);
		regs->pr = (unsigned long) frame->retcode;
		flush_icache_range(regs->pr, regs->pr + sizeof(frame->retcode));
	}

	if (err)
		return -EFAULT;

	/* Set up registers for signal handler */
	regs->regs[15] = (unsigned long) frame;
	regs->regs[4] = sig; /* Arg for signal handler */
	regs->regs[5] = (unsigned long) &frame->info;
	regs->regs[6] = (unsigned long) &frame->uc;

	if (current->personality & FDPIC_FUNCPTRS) {
		struct fdpic_func_descriptor __user *funcptr =
			(struct fdpic_func_descriptor __user *)ksig->ka.sa.sa_handler;

		err |= __get_user(regs->pc, &funcptr->text);
		err |= __get_user(regs->regs[12], &funcptr->GOT);
	} else
		regs->pc = (unsigned long)ksig->ka.sa.sa_handler;

	if (err)
		return -EFAULT;

	pr_debug("SIG deliver (%s:%d): sp=%p pc=%08lx pr=%08lx\n",
		 current->comm, task_pid_nr(current), frame, regs->pc, regs->pr);

	return 0;
}

static inline void
handle_syscall_restart(unsigned long save_r0, struct pt_regs *regs,
		       struct sigaction *sa)
{
	/* If we're not from a syscall, bail out */
	if (regs->tra < 0)
		return;

	/* check for system call restart.. */
	switch (regs->regs[0]) {
		case -ERESTART_RESTARTBLOCK:
		case -ERESTARTNOHAND:
		no_system_call_restart:
			regs->regs[0] = -EINTR;
			break;

		case -ERESTARTSYS:
			if (!(sa->sa_flags & SA_RESTART))
				goto no_system_call_restart;
			fallthrough;
		case -ERESTARTNOINTR:
			regs->regs[0] = save_r0;
			regs->pc -= instruction_size(__raw_readw(regs->pc - 4));
			break;
	}
}

/*
 * OK, we're invoking a handler
 */
static void
handle_signal(struct ksignal *ksig, struct pt_regs *regs, unsigned int save_r0)
{
	sigset_t *oldset = sigmask_to_save();
	int ret;

	/* Set up the stack frame */
	if (ksig->ka.sa.sa_flags & SA_SIGINFO)
		ret = setup_rt_frame(ksig, oldset, regs);
	else
		ret = setup_frame(ksig, oldset, regs);

	signal_setup_done(ret, ksig, test_thread_flag(TIF_SINGLESTEP));
}

/*
 * Note that 'init' is a special process: it doesn't get signals it doesn't
 * want to handle. Thus you cannot kill init even with a SIGKILL even by
 * mistake.
 *
 * Note that we go through the signals twice: once to check the signals that
 * the kernel can handle, and then we build all the user-level signal handling
 * stack-frames in one go after that.
 */
static void do_signal(struct pt_regs *regs, unsigned int save_r0)
{
	struct ksignal ksig;

	/*
	 * We want the common case to go fast, which
	 * is why we may in certain cases get here from
	 * kernel mode. Just return without doing anything
	 * if so.
	 */
	if (!user_mode(regs))
		return;

	if (get_signal(&ksig)) {
		handle_syscall_restart(save_r0, regs, &ksig.ka.sa);

		/* Whee!  Actually deliver the signal.  */
		handle_signal(&ksig, regs, save_r0);
		return;
	}

	/* Did we come from a system call? */
	if (regs->tra >= 0) {
		/* Restart the system call - no handlers present */
		if (regs->regs[0] == -ERESTARTNOHAND ||
		    regs->regs[0] == -ERESTARTSYS ||
		    regs->regs[0] == -ERESTARTNOINTR) {
			regs->regs[0] = save_r0;
			regs->pc -= instruction_size(__raw_readw(regs->pc - 4));
		} else if (regs->regs[0] == -ERESTART_RESTARTBLOCK) {
			regs->pc -= instruction_size(__raw_readw(regs->pc - 4));
			regs->regs[3] = __NR_restart_syscall;
		}
	}

	/*
	 * If there's no signal to deliver, we just put the saved sigmask
	 * back.
	 */
	restore_saved_sigmask();
}

asmlinkage void do_notify_resume(struct pt_regs *regs, unsigned int save_r0,
				 unsigned long thread_info_flags)
{
	/* deal with pending signal delivery */
	if (thread_info_flags & (_TIF_SIGPENDING | _TIF_NOTIFY_SIGNAL))
		do_signal(regs, save_r0);

	if (thread_info_flags & _TIF_NOTIFY_RESUME)
		tracehook_notify_resume(regs);
}
