/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_EXTABLE_H
#define _ASM_X86_EXTABLE_H
/*
 * The exception table consists of triples of addresses relative to the
 * exception table entry itself. The first address is of an instruction
 * that is allowed to fault, the second is the target at which the program
 * should continue. The third is a handler function to deal with the fault
 * caused by the instruction in the first field.
 *
 * All the routines below use bits of fixup code that are out of line
 * with the main instruction path.  This means when everything is well,
 * we don't even have to jump over them.  Further, they do not intrude
 * on our cache or tlb entries.
 */

struct exception_table_entry {
	int insn, fixup, handler;
};
struct pt_regs;

#define ARCH_HAS_RELATIVE_EXTABLE

#define swap_ex_entry_fixup(a, b, tmp, delta)			\
	do {							\
		(a)->fixup = (b)->fixup + (delta);		\
		(b)->fixup = (tmp).fixup - (delta);		\
		(a)->handler = (b)->handler + (delta);		\
		(b)->handler = (tmp).handler - (delta);		\
	} while (0)

enum handler_type {
	EX_HANDLER_NONE,
	EX_HANDLER_FAULT,
	EX_HANDLER_UACCESS,
	EX_HANDLER_OTHER
};

extern int fixup_exception(struct pt_regs *regs, int trapnr,
			   unsigned long error_code, unsigned long fault_addr);
extern int fixup_bug(struct pt_regs *regs, int trapnr);
extern enum handler_type ex_get_fault_handler_type(unsigned long ip);
extern void early_fixup_exception(struct pt_regs *regs, int trapnr);

#endif
