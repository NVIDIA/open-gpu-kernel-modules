// SPDX-License-Identifier: GPL-2.0-only
/*
 * Kernel traps/events for Hexagon processor
 *
 * Copyright (c) 2010-2014, The Linux Foundation. All rights reserved.
 */

#include <linux/init.h>
#include <linux/sched/signal.h>
#include <linux/sched/debug.h>
#include <linux/sched/task_stack.h>
#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/kdebug.h>
#include <linux/syscalls.h>
#include <linux/signal.h>
#include <linux/tracehook.h>
#include <asm/traps.h>
#include <asm/vm_fault.h>
#include <asm/syscall.h>
#include <asm/registers.h>
#include <asm/unistd.h>
#include <asm/sections.h>
#ifdef CONFIG_KGDB
# include <linux/kgdb.h>
#endif

#define TRAP_SYSCALL	1
#define TRAP_DEBUG	0xdb

void __init trap_init(void)
{
}

#ifdef CONFIG_GENERIC_BUG
/* Maybe should resemble arch/sh/kernel/traps.c ?? */
int is_valid_bugaddr(unsigned long addr)
{
	return 1;
}
#endif /* CONFIG_GENERIC_BUG */

static const char *ex_name(int ex)
{
	switch (ex) {
	case HVM_GE_C_XPROT:
	case HVM_GE_C_XUSER:
		return "Execute protection fault";
	case HVM_GE_C_RPROT:
	case HVM_GE_C_RUSER:
		return "Read protection fault";
	case HVM_GE_C_WPROT:
	case HVM_GE_C_WUSER:
		return "Write protection fault";
	case HVM_GE_C_XMAL:
		return "Misaligned instruction";
	case HVM_GE_C_WREG:
		return "Multiple writes to same register in packet";
	case HVM_GE_C_PCAL:
		return "Program counter values that are not properly aligned";
	case HVM_GE_C_RMAL:
		return "Misaligned data load";
	case HVM_GE_C_WMAL:
		return "Misaligned data store";
	case HVM_GE_C_INVI:
	case HVM_GE_C_PRIVI:
		return "Illegal instruction";
	case HVM_GE_C_BUS:
		return "Precise bus error";
	case HVM_GE_C_CACHE:
		return "Cache error";

	case 0xdb:
		return "Debugger trap";

	default:
		return "Unrecognized exception";
	}
}

static void do_show_stack(struct task_struct *task, unsigned long *fp,
			  unsigned long ip, const char *loglvl)
{
	int kstack_depth_to_print = 24;
	unsigned long offset, size;
	const char *name = NULL;
	unsigned long *newfp;
	unsigned long low, high;
	char tmpstr[128];
	char *modname;
	int i;

	if (task == NULL)
		task = current;

	printk("%sCPU#%d, %s/%d, Call Trace:\n", loglvl, raw_smp_processor_id(),
		task->comm, task_pid_nr(task));

	if (fp == NULL) {
		if (task == current) {
			asm("%0 = r30" : "=r" (fp));
		} else {
			fp = (unsigned long *)
			     ((struct hexagon_switch_stack *)
			     task->thread.switch_sp)->fp;
		}
	}

	if ((((unsigned long) fp) & 0x3) || ((unsigned long) fp < 0x1000)) {
		printk("%s-- Corrupt frame pointer %p\n", loglvl, fp);
		return;
	}

	/* Saved link reg is one word above FP */
	if (!ip)
		ip = *(fp+1);

	/* Expect kernel stack to be in-bounds */
	low = (unsigned long)task_stack_page(task);
	high = low + THREAD_SIZE - 8;
	low += sizeof(struct thread_info);

	for (i = 0; i < kstack_depth_to_print; i++) {

		name = kallsyms_lookup(ip, &size, &offset, &modname, tmpstr);

		printk("%s[%p] 0x%lx: %s + 0x%lx", loglvl, fp, ip, name, offset);
		if (((unsigned long) fp < low) || (high < (unsigned long) fp))
			printk(KERN_CONT " (FP out of bounds!)");
		if (modname)
			printk(KERN_CONT " [%s] ", modname);
		printk(KERN_CONT "\n");

		newfp = (unsigned long *) *fp;

		if (((unsigned long) newfp) & 0x3) {
			printk("%s-- Corrupt frame pointer %p\n", loglvl, newfp);
			break;
		}

		/* Attempt to continue past exception. */
		if (0 == newfp) {
			struct pt_regs *regs = (struct pt_regs *) (((void *)fp)
						+ 8);

			if (regs->syscall_nr != -1) {
				printk("%s-- trap0 -- syscall_nr: %ld", loglvl,
					regs->syscall_nr);
				printk(KERN_CONT "  psp: %lx  elr: %lx\n",
					 pt_psp(regs), pt_elr(regs));
				break;
			} else {
				/* really want to see more ... */
				kstack_depth_to_print += 6;
				printk("%s-- %s (0x%lx)  badva: %lx\n", loglvl,
					ex_name(pt_cause(regs)), pt_cause(regs),
					pt_badva(regs));
			}

			newfp = (unsigned long *) regs->r30;
			ip = pt_elr(regs);
		} else {
			ip = *(newfp + 1);
		}

		/* If link reg is null, we are done. */
		if (ip == 0x0)
			break;

		/* If newfp isn't larger, we're tracing garbage. */
		if (newfp > fp)
			fp = newfp;
		else
			break;
	}
}

void show_stack(struct task_struct *task, unsigned long *fp, const char *loglvl)
{
	/* Saved link reg is one word above FP */
	do_show_stack(task, fp, 0, loglvl);
}

int die(const char *str, struct pt_regs *regs, long err)
{
	static struct {
		spinlock_t lock;
		int counter;
	} die = {
		.lock = __SPIN_LOCK_UNLOCKED(die.lock),
		.counter = 0
	};

	console_verbose();
	oops_enter();

	spin_lock_irq(&die.lock);
	bust_spinlocks(1);
	printk(KERN_EMERG "Oops: %s[#%d]:\n", str, ++die.counter);

	if (notify_die(DIE_OOPS, str, regs, err, pt_cause(regs), SIGSEGV) ==
	    NOTIFY_STOP)
		return 1;

	print_modules();
	show_regs(regs);
	do_show_stack(current, &regs->r30, pt_elr(regs), KERN_EMERG);

	bust_spinlocks(0);
	add_taint(TAINT_DIE, LOCKDEP_NOW_UNRELIABLE);

	spin_unlock_irq(&die.lock);

	if (in_interrupt())
		panic("Fatal exception in interrupt");

	if (panic_on_oops)
		panic("Fatal exception");

	oops_exit();
	do_exit(err);
	return 0;
}

int die_if_kernel(char *str, struct pt_regs *regs, long err)
{
	if (!user_mode(regs))
		return die(str, regs, err);
	else
		return 0;
}

/*
 * It's not clear that misaligned fetches are ever recoverable.
 */
static void misaligned_instruction(struct pt_regs *regs)
{
	die_if_kernel("Misaligned Instruction", regs, 0);
	force_sig(SIGBUS);
}

/*
 * Misaligned loads and stores, on the other hand, can be
 * emulated, and probably should be, some day.  But for now
 * they will be considered fatal.
 */
static void misaligned_data_load(struct pt_regs *regs)
{
	die_if_kernel("Misaligned Data Load", regs, 0);
	force_sig(SIGBUS);
}

static void misaligned_data_store(struct pt_regs *regs)
{
	die_if_kernel("Misaligned Data Store", regs, 0);
	force_sig(SIGBUS);
}

static void illegal_instruction(struct pt_regs *regs)
{
	die_if_kernel("Illegal Instruction", regs, 0);
	force_sig(SIGILL);
}

/*
 * Precise bus errors may be recoverable with a a retry,
 * but for now, treat them as irrecoverable.
 */
static void precise_bus_error(struct pt_regs *regs)
{
	die_if_kernel("Precise Bus Error", regs, 0);
	force_sig(SIGBUS);
}

/*
 * If anything is to be done here other than panic,
 * it will probably be complex and migrate to another
 * source module.  For now, just die.
 */
static void cache_error(struct pt_regs *regs)
{
	die("Cache Error", regs, 0);
}

/*
 * General exception handler
 */
void do_genex(struct pt_regs *regs)
{
	/*
	 * Decode Cause and Dispatch
	 */
	switch (pt_cause(regs)) {
	case HVM_GE_C_XPROT:
	case HVM_GE_C_XUSER:
		execute_protection_fault(regs);
		break;
	case HVM_GE_C_RPROT:
	case HVM_GE_C_RUSER:
		read_protection_fault(regs);
		break;
	case HVM_GE_C_WPROT:
	case HVM_GE_C_WUSER:
		write_protection_fault(regs);
		break;
	case HVM_GE_C_XMAL:
		misaligned_instruction(regs);
		break;
	case HVM_GE_C_WREG:
		illegal_instruction(regs);
		break;
	case HVM_GE_C_PCAL:
		misaligned_instruction(regs);
		break;
	case HVM_GE_C_RMAL:
		misaligned_data_load(regs);
		break;
	case HVM_GE_C_WMAL:
		misaligned_data_store(regs);
		break;
	case HVM_GE_C_INVI:
	case HVM_GE_C_PRIVI:
		illegal_instruction(regs);
		break;
	case HVM_GE_C_BUS:
		precise_bus_error(regs);
		break;
	case HVM_GE_C_CACHE:
		cache_error(regs);
		break;
	default:
		/* Halt and catch fire */
		panic("Unrecognized exception 0x%lx\n", pt_cause(regs));
		break;
	}
}

/* Indirect system call dispatch */
long sys_syscall(void)
{
	printk(KERN_ERR "sys_syscall invoked!\n");
	return -ENOSYS;
}

void do_trap0(struct pt_regs *regs)
{
	syscall_fn syscall;

	switch (pt_cause(regs)) {
	case TRAP_SYSCALL:
		/* System call is trap0 #1 */

		/* allow strace to catch syscall args  */
		if (unlikely(test_thread_flag(TIF_SYSCALL_TRACE) &&
			tracehook_report_syscall_entry(regs)))
			return;  /*  return -ENOSYS somewhere?  */

		/* Interrupts should be re-enabled for syscall processing */
		__vmsetie(VM_INT_ENABLE);

		/*
		 * System call number is in r6, arguments in r0..r5.
		 * Fortunately, no Linux syscall has more than 6 arguments,
		 * and Hexagon ABI passes first 6 arguments in registers.
		 * 64-bit arguments are passed in odd/even register pairs.
		 * Fortunately, we have no system calls that take more
		 * than three arguments with more than one 64-bit value.
		 * Should that change, we'd need to redesign to copy
		 * between user and kernel stacks.
		 */
		regs->syscall_nr = regs->r06;

		/*
		 * GPR R0 carries the first parameter, and is also used
		 * to report the return value.  We need a backup of
		 * the user's value in case we need to do a late restart
		 * of the system call.
		 */
		regs->restart_r0 = regs->r00;

		if ((unsigned long) regs->syscall_nr >= __NR_syscalls) {
			regs->r00 = -1;
		} else {
			syscall = (syscall_fn)
				  (sys_call_table[regs->syscall_nr]);
			regs->r00 = syscall(regs->r00, regs->r01,
				   regs->r02, regs->r03,
				   regs->r04, regs->r05);
		}

		/* allow strace to get the syscall return state  */
		if (unlikely(test_thread_flag(TIF_SYSCALL_TRACE)))
			tracehook_report_syscall_exit(regs, 0);

		break;
	case TRAP_DEBUG:
		/* Trap0 0xdb is debug breakpoint */
		if (user_mode(regs)) {
			/*
			 * Some architecures add some per-thread state
			 * to distinguish between breakpoint traps and
			 * trace traps.  We may want to do that, and
			 * set the si_code value appropriately, or we
			 * may want to use a different trap0 flavor.
			 */
			force_sig_fault(SIGTRAP, TRAP_BRKPT,
					(void __user *) pt_elr(regs));
		} else {
#ifdef CONFIG_KGDB
			kgdb_handle_exception(pt_cause(regs), SIGTRAP,
					      TRAP_BRKPT, regs);
#endif
		}
		break;
	}
	/* Ignore other trap0 codes for now, especially 0 (Angel calls) */
}

/*
 * Machine check exception handler
 */
void do_machcheck(struct pt_regs *regs)
{
	/* Halt and catch fire */
	__vmstop();
}

/*
 * Treat this like the old 0xdb trap.
 */

void do_debug_exception(struct pt_regs *regs)
{
	regs->hvmer.vmest &= ~HVM_VMEST_CAUSE_MSK;
	regs->hvmer.vmest |= (TRAP_DEBUG << HVM_VMEST_CAUSE_SFT);
	do_trap0(regs);
}
