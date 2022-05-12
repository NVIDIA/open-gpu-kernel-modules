// SPDX-License-Identifier: GPL-2.0
/*
 * fault.c:  Page fault handlers for the Sparc.
 *
 * Copyright (C) 1995 David S. Miller (davem@caip.rutgers.edu)
 * Copyright (C) 1996 Eddie C. Dost (ecd@skynet.be)
 * Copyright (C) 1997 Jakub Jelinek (jj@sunsite.mff.cuni.cz)
 */

#include <asm/head.h>

#include <linux/string.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/ptrace.h>
#include <linux/mman.h>
#include <linux/threads.h>
#include <linux/kernel.h>
#include <linux/signal.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/perf_event.h>
#include <linux/interrupt.h>
#include <linux/kdebug.h>
#include <linux/uaccess.h>
#include <linux/extable.h>

#include <asm/page.h>
#include <asm/openprom.h>
#include <asm/oplib.h>
#include <asm/setup.h>
#include <asm/smp.h>
#include <asm/traps.h>

#include "mm_32.h"

int show_unhandled_signals = 1;

static void __noreturn unhandled_fault(unsigned long address,
				       struct task_struct *tsk,
				       struct pt_regs *regs)
{
	if ((unsigned long) address < PAGE_SIZE) {
		printk(KERN_ALERT
		    "Unable to handle kernel NULL pointer dereference\n");
	} else {
		printk(KERN_ALERT "Unable to handle kernel paging request at virtual address %08lx\n",
		       address);
	}
	printk(KERN_ALERT "tsk->{mm,active_mm}->context = %08lx\n",
		(tsk->mm ? tsk->mm->context : tsk->active_mm->context));
	printk(KERN_ALERT "tsk->{mm,active_mm}->pgd = %08lx\n",
		(tsk->mm ? (unsigned long) tsk->mm->pgd :
			(unsigned long) tsk->active_mm->pgd));
	die_if_kernel("Oops", regs);
}

static inline void
show_signal_msg(struct pt_regs *regs, int sig, int code,
		unsigned long address, struct task_struct *tsk)
{
	if (!unhandled_signal(tsk, sig))
		return;

	if (!printk_ratelimit())
		return;

	printk("%s%s[%d]: segfault at %lx ip %px (rpc %px) sp %px error %x",
	       task_pid_nr(tsk) > 1 ? KERN_INFO : KERN_EMERG,
	       tsk->comm, task_pid_nr(tsk), address,
	       (void *)regs->pc, (void *)regs->u_regs[UREG_I7],
	       (void *)regs->u_regs[UREG_FP], code);

	print_vma_addr(KERN_CONT " in ", regs->pc);

	printk(KERN_CONT "\n");
}

static void __do_fault_siginfo(int code, int sig, struct pt_regs *regs,
			       unsigned long addr)
{
	if (unlikely(show_unhandled_signals))
		show_signal_msg(regs, sig, code,
				addr, current);

	force_sig_fault(sig, code, (void __user *) addr, 0);
}

static unsigned long compute_si_addr(struct pt_regs *regs, int text_fault)
{
	unsigned int insn;

	if (text_fault)
		return regs->pc;

	if (regs->psr & PSR_PS)
		insn = *(unsigned int *) regs->pc;
	else
		__get_user(insn, (unsigned int *) regs->pc);

	return safe_compute_effective_address(regs, insn);
}

static noinline void do_fault_siginfo(int code, int sig, struct pt_regs *regs,
				      int text_fault)
{
	unsigned long addr = compute_si_addr(regs, text_fault);

	__do_fault_siginfo(code, sig, regs, addr);
}

asmlinkage void do_sparc_fault(struct pt_regs *regs, int text_fault, int write,
			       unsigned long address)
{
	struct vm_area_struct *vma;
	struct task_struct *tsk = current;
	struct mm_struct *mm = tsk->mm;
	int from_user = !(regs->psr & PSR_PS);
	int code;
	vm_fault_t fault;
	unsigned int flags = FAULT_FLAG_DEFAULT;

	if (text_fault)
		address = regs->pc;

	/*
	 * We fault-in kernel-space virtual memory on-demand. The
	 * 'reference' page table is init_mm.pgd.
	 *
	 * NOTE! We MUST NOT take any locks for this case. We may
	 * be in an interrupt or a critical region, and should
	 * only copy the information from the master page table,
	 * nothing more.
	 */
	code = SEGV_MAPERR;
	if (address >= TASK_SIZE)
		goto vmalloc_fault;

	/*
	 * If we're in an interrupt or have no user
	 * context, we must not take the fault..
	 */
	if (pagefault_disabled() || !mm)
		goto no_context;

	perf_sw_event(PERF_COUNT_SW_PAGE_FAULTS, 1, regs, address);

retry:
	mmap_read_lock(mm);

	if (!from_user && address >= PAGE_OFFSET)
		goto bad_area;

	vma = find_vma(mm, address);
	if (!vma)
		goto bad_area;
	if (vma->vm_start <= address)
		goto good_area;
	if (!(vma->vm_flags & VM_GROWSDOWN))
		goto bad_area;
	if (expand_stack(vma, address))
		goto bad_area;
	/*
	 * Ok, we have a good vm_area for this memory access, so
	 * we can handle it..
	 */
good_area:
	code = SEGV_ACCERR;
	if (write) {
		if (!(vma->vm_flags & VM_WRITE))
			goto bad_area;
	} else {
		/* Allow reads even for write-only mappings */
		if (!(vma->vm_flags & (VM_READ | VM_EXEC)))
			goto bad_area;
	}

	if (from_user)
		flags |= FAULT_FLAG_USER;
	if (write)
		flags |= FAULT_FLAG_WRITE;

	/*
	 * If for any reason at all we couldn't handle the fault,
	 * make sure we exit gracefully rather than endlessly redo
	 * the fault.
	 */
	fault = handle_mm_fault(vma, address, flags, regs);

	if (fault_signal_pending(fault, regs))
		return;

	if (unlikely(fault & VM_FAULT_ERROR)) {
		if (fault & VM_FAULT_OOM)
			goto out_of_memory;
		else if (fault & VM_FAULT_SIGSEGV)
			goto bad_area;
		else if (fault & VM_FAULT_SIGBUS)
			goto do_sigbus;
		BUG();
	}

	if (flags & FAULT_FLAG_ALLOW_RETRY) {
		if (fault & VM_FAULT_RETRY) {
			flags |= FAULT_FLAG_TRIED;

			/* No need to mmap_read_unlock(mm) as we would
			 * have already released it in __lock_page_or_retry
			 * in mm/filemap.c.
			 */

			goto retry;
		}
	}

	mmap_read_unlock(mm);
	return;

	/*
	 * Something tried to access memory that isn't in our memory map..
	 * Fix it, but check if it's kernel or user first..
	 */
bad_area:
	mmap_read_unlock(mm);

bad_area_nosemaphore:
	/* User mode accesses just cause a SIGSEGV */
	if (from_user) {
		do_fault_siginfo(code, SIGSEGV, regs, text_fault);
		return;
	}

	/* Is this in ex_table? */
no_context:
	if (!from_user) {
		const struct exception_table_entry *entry;

		entry = search_exception_tables(regs->pc);
#ifdef DEBUG_EXCEPTIONS
		printk("Exception: PC<%08lx> faddr<%08lx>\n",
		       regs->pc, address);
		printk("EX_TABLE: insn<%08lx> fixup<%08x>\n",
			regs->pc, entry->fixup);
#endif
		regs->pc = entry->fixup;
		regs->npc = regs->pc + 4;
		return;
	}

	unhandled_fault(address, tsk, regs);
	do_exit(SIGKILL);

/*
 * We ran out of memory, or some other thing happened to us that made
 * us unable to handle the page fault gracefully.
 */
out_of_memory:
	mmap_read_unlock(mm);
	if (from_user) {
		pagefault_out_of_memory();
		return;
	}
	goto no_context;

do_sigbus:
	mmap_read_unlock(mm);
	do_fault_siginfo(BUS_ADRERR, SIGBUS, regs, text_fault);
	if (!from_user)
		goto no_context;

vmalloc_fault:
	{
		/*
		 * Synchronize this task's top level page-table
		 * with the 'reference' page table.
		 */
		int offset = pgd_index(address);
		pgd_t *pgd, *pgd_k;
		p4d_t *p4d, *p4d_k;
		pud_t *pud, *pud_k;
		pmd_t *pmd, *pmd_k;

		pgd = tsk->active_mm->pgd + offset;
		pgd_k = init_mm.pgd + offset;

		if (!pgd_present(*pgd)) {
			if (!pgd_present(*pgd_k))
				goto bad_area_nosemaphore;
			pgd_val(*pgd) = pgd_val(*pgd_k);
			return;
		}

		p4d = p4d_offset(pgd, address);
		pud = pud_offset(p4d, address);
		pmd = pmd_offset(pud, address);

		p4d_k = p4d_offset(pgd_k, address);
		pud_k = pud_offset(p4d_k, address);
		pmd_k = pmd_offset(pud_k, address);

		if (pmd_present(*pmd) || !pmd_present(*pmd_k))
			goto bad_area_nosemaphore;

		*pmd = *pmd_k;
		return;
	}
}

/* This always deals with user addresses. */
static void force_user_fault(unsigned long address, int write)
{
	struct vm_area_struct *vma;
	struct task_struct *tsk = current;
	struct mm_struct *mm = tsk->mm;
	unsigned int flags = FAULT_FLAG_USER;
	int code;

	code = SEGV_MAPERR;

	mmap_read_lock(mm);
	vma = find_vma(mm, address);
	if (!vma)
		goto bad_area;
	if (vma->vm_start <= address)
		goto good_area;
	if (!(vma->vm_flags & VM_GROWSDOWN))
		goto bad_area;
	if (expand_stack(vma, address))
		goto bad_area;
good_area:
	code = SEGV_ACCERR;
	if (write) {
		if (!(vma->vm_flags & VM_WRITE))
			goto bad_area;
		flags |= FAULT_FLAG_WRITE;
	} else {
		if (!(vma->vm_flags & (VM_READ | VM_EXEC)))
			goto bad_area;
	}
	switch (handle_mm_fault(vma, address, flags, NULL)) {
	case VM_FAULT_SIGBUS:
	case VM_FAULT_OOM:
		goto do_sigbus;
	}
	mmap_read_unlock(mm);
	return;
bad_area:
	mmap_read_unlock(mm);
	__do_fault_siginfo(code, SIGSEGV, tsk->thread.kregs, address);
	return;

do_sigbus:
	mmap_read_unlock(mm);
	__do_fault_siginfo(BUS_ADRERR, SIGBUS, tsk->thread.kregs, address);
}

static void check_stack_aligned(unsigned long sp)
{
	if (sp & 0x7UL)
		force_sig(SIGILL);
}

void window_overflow_fault(void)
{
	unsigned long sp;

	sp = current_thread_info()->rwbuf_stkptrs[0];
	if (((sp + 0x38) & PAGE_MASK) != (sp & PAGE_MASK))
		force_user_fault(sp + 0x38, 1);
	force_user_fault(sp, 1);

	check_stack_aligned(sp);
}

void window_underflow_fault(unsigned long sp)
{
	if (((sp + 0x38) & PAGE_MASK) != (sp & PAGE_MASK))
		force_user_fault(sp + 0x38, 0);
	force_user_fault(sp, 0);

	check_stack_aligned(sp);
}

void window_ret_fault(struct pt_regs *regs)
{
	unsigned long sp;

	sp = regs->u_regs[UREG_FP];
	if (((sp + 0x38) & PAGE_MASK) != (sp & PAGE_MASK))
		force_user_fault(sp + 0x38, 0);
	force_user_fault(sp, 0);

	check_stack_aligned(sp);
}
