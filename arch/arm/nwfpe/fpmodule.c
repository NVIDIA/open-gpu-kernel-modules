// SPDX-License-Identifier: GPL-2.0-or-later

/*
    NetWinder Floating Point Emulator
    (c) Rebel.com, 1998-1999
    (c) Philip Blundell, 1998-1999

    Direct questions, comments to Scott Bambrough <scottb@netwinder.org>

*/

#include "fpa11.h"

#include <linux/module.h>
#include <linux/moduleparam.h>

/* XXX */
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/signal.h>
#include <linux/sched/signal.h>
#include <linux/init.h>

#include <asm/thread_notify.h>

#include "softfloat.h"
#include "fpopcode.h"
#include "fpmodule.h"
#include "fpa11.inl"

/* kernel symbols required for signal handling */
#ifdef CONFIG_FPE_NWFPE_XP
#define NWFPE_BITS "extended"
#else
#define NWFPE_BITS "double"
#endif

#ifdef MODULE
void fp_send_sig(unsigned long sig, struct task_struct *p, int priv);
#else
#define fp_send_sig	send_sig
#define kern_fp_enter	fp_enter

extern char fpe_type[];
#endif

static int nwfpe_notify(struct notifier_block *self, unsigned long cmd, void *v)
{
	struct thread_info *thread = v;

	if (cmd == THREAD_NOTIFY_FLUSH)
		nwfpe_init_fpa(&thread->fpstate);

	return NOTIFY_DONE;
}

static struct notifier_block nwfpe_notifier_block = {
	.notifier_call = nwfpe_notify,
};

/* kernel function prototypes required */
void fp_setup(void);

/* external declarations for saved kernel symbols */
extern void (*kern_fp_enter)(void);

/* Original value of fp_enter from kernel before patched by fpe_init. */
static void (*orig_fp_enter)(void);

/* forward declarations */
extern void nwfpe_enter(void);

static int __init fpe_init(void)
{
	if (sizeof(FPA11) > sizeof(union fp_state)) {
		pr_err("nwfpe: bad structure size\n");
		return -EINVAL;
	}

	if (sizeof(FPREG) != 12) {
		pr_err("nwfpe: bad register size\n");
		return -EINVAL;
	}
	if (fpe_type[0] && strcmp(fpe_type, "nwfpe"))
		return 0;

	/* Display title, version and copyright information. */
	pr_info("NetWinder Floating Point Emulator V0.97 ("
	        NWFPE_BITS " precision)\n");

	thread_register_notifier(&nwfpe_notifier_block);

	/* Save pointer to the old FP handler and then patch ourselves in */
	orig_fp_enter = kern_fp_enter;
	kern_fp_enter = nwfpe_enter;

	return 0;
}

static void __exit fpe_exit(void)
{
	thread_unregister_notifier(&nwfpe_notifier_block);
	/* Restore the values we saved earlier. */
	kern_fp_enter = orig_fp_enter;
}

/*
ScottB:  November 4, 1998

Moved this function out of softfloat-specialize into fpmodule.c.
This effectively isolates all the changes required for integrating with the
Linux kernel into fpmodule.c.  Porting to NetBSD should only require modifying
fpmodule.c to integrate with the NetBSD kernel (I hope!).

[1/1/99: Not quite true any more unfortunately.  There is Linux-specific
code to access data in user space in some other source files at the 
moment (grep for get_user / put_user calls).  --philb]

This function is called by the SoftFloat routines to raise a floating
point exception.  We check the trap enable byte in the FPSR, and raise
a SIGFPE exception if necessary.  If not the relevant bits in the 
cumulative exceptions flag byte are set and we return.
*/

#ifdef CONFIG_DEBUG_USER
/* By default, ignore inexact errors as there are far too many of them to log */
static int debug = ~BIT_IXC;
#endif

void float_raise(signed char flags)
{
	register unsigned int fpsr, cumulativeTraps;

#ifdef CONFIG_DEBUG_USER
	if (flags & debug)
 		printk(KERN_DEBUG
		       "NWFPE: %s[%d] takes exception %08x at %ps from %08lx\n",
		       current->comm, current->pid, flags,
		       __builtin_return_address(0), GET_USERREG()->ARM_pc);
#endif

	/* Read fpsr and initialize the cumulativeTraps.  */
	fpsr = readFPSR();
	cumulativeTraps = 0;

	/* For each type of exception, the cumulative trap exception bit is only
	   set if the corresponding trap enable bit is not set.  */
	if ((!(fpsr & BIT_IXE)) && (flags & BIT_IXC))
		cumulativeTraps |= BIT_IXC;
	if ((!(fpsr & BIT_UFE)) && (flags & BIT_UFC))
		cumulativeTraps |= BIT_UFC;
	if ((!(fpsr & BIT_OFE)) && (flags & BIT_OFC))
		cumulativeTraps |= BIT_OFC;
	if ((!(fpsr & BIT_DZE)) && (flags & BIT_DZC))
		cumulativeTraps |= BIT_DZC;
	if ((!(fpsr & BIT_IOE)) && (flags & BIT_IOC))
		cumulativeTraps |= BIT_IOC;

	/* Set the cumulative exceptions flags.  */
	if (cumulativeTraps)
		writeFPSR(fpsr | cumulativeTraps);

	/* Raise an exception if necessary.  */
	if (fpsr & (flags << 16))
		fp_send_sig(SIGFPE, current, 1);
}

module_init(fpe_init);
module_exit(fpe_exit);

MODULE_AUTHOR("Scott Bambrough <scottb@rebel.com>");
MODULE_DESCRIPTION("NWFPE floating point emulator (" NWFPE_BITS " precision)");
MODULE_LICENSE("GPL");

#ifdef CONFIG_DEBUG_USER
module_param(debug, int, 0644);
#endif
