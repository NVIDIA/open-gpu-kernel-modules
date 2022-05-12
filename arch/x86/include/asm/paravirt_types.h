/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_PARAVIRT_TYPES_H
#define _ASM_X86_PARAVIRT_TYPES_H

/* Bitmask of what can be clobbered: usually at least eax. */
#define CLBR_EAX  (1 << 0)
#define CLBR_ECX  (1 << 1)
#define CLBR_EDX  (1 << 2)
#define CLBR_EDI  (1 << 3)

#ifdef CONFIG_X86_32
/* CLBR_ANY should match all regs platform has. For i386, that's just it */
#define CLBR_ANY  ((1 << 4) - 1)

#define CLBR_ARG_REGS	(CLBR_EAX | CLBR_EDX | CLBR_ECX)
#define CLBR_RET_REG	(CLBR_EAX | CLBR_EDX)
#else
#define CLBR_RAX  CLBR_EAX
#define CLBR_RCX  CLBR_ECX
#define CLBR_RDX  CLBR_EDX
#define CLBR_RDI  CLBR_EDI
#define CLBR_RSI  (1 << 4)
#define CLBR_R8   (1 << 5)
#define CLBR_R9   (1 << 6)
#define CLBR_R10  (1 << 7)
#define CLBR_R11  (1 << 8)

#define CLBR_ANY  ((1 << 9) - 1)

#define CLBR_ARG_REGS	(CLBR_RDI | CLBR_RSI | CLBR_RDX | \
			 CLBR_RCX | CLBR_R8 | CLBR_R9)
#define CLBR_RET_REG	(CLBR_RAX)

#endif /* X86_64 */

#ifndef __ASSEMBLY__

#include <asm/desc_defs.h>
#include <asm/pgtable_types.h>
#include <asm/nospec-branch.h>

struct page;
struct thread_struct;
struct desc_ptr;
struct tss_struct;
struct mm_struct;
struct desc_struct;
struct task_struct;
struct cpumask;
struct flush_tlb_info;
struct mmu_gather;
struct vm_area_struct;

/*
 * Wrapper type for pointers to code which uses the non-standard
 * calling convention.  See PV_CALL_SAVE_REGS_THUNK below.
 */
struct paravirt_callee_save {
	void *func;
};

/* general info */
struct pv_info {
#ifdef CONFIG_PARAVIRT_XXL
	u16 extra_user_64bit_cs;  /* __USER_CS if none */
#endif

	const char *name;
};

#ifdef CONFIG_PARAVIRT_XXL
struct pv_lazy_ops {
	/* Set deferred update mode, used for batching operations. */
	void (*enter)(void);
	void (*leave)(void);
	void (*flush)(void);
} __no_randomize_layout;
#endif

struct pv_cpu_ops {
	/* hooks for various privileged instructions */
	void (*io_delay)(void);

#ifdef CONFIG_PARAVIRT_XXL
	unsigned long (*get_debugreg)(int regno);
	void (*set_debugreg)(int regno, unsigned long value);

	unsigned long (*read_cr0)(void);
	void (*write_cr0)(unsigned long);

	void (*write_cr4)(unsigned long);

	/* Segment descriptor handling */
	void (*load_tr_desc)(void);
	void (*load_gdt)(const struct desc_ptr *);
	void (*load_idt)(const struct desc_ptr *);
	void (*set_ldt)(const void *desc, unsigned entries);
	unsigned long (*store_tr)(void);
	void (*load_tls)(struct thread_struct *t, unsigned int cpu);
	void (*load_gs_index)(unsigned int idx);
	void (*write_ldt_entry)(struct desc_struct *ldt, int entrynum,
				const void *desc);
	void (*write_gdt_entry)(struct desc_struct *,
				int entrynum, const void *desc, int size);
	void (*write_idt_entry)(gate_desc *,
				int entrynum, const gate_desc *gate);
	void (*alloc_ldt)(struct desc_struct *ldt, unsigned entries);
	void (*free_ldt)(struct desc_struct *ldt, unsigned entries);

	void (*load_sp0)(unsigned long sp0);

#ifdef CONFIG_X86_IOPL_IOPERM
	void (*invalidate_io_bitmap)(void);
	void (*update_io_bitmap)(void);
#endif

	void (*wbinvd)(void);

	/* cpuid emulation, mostly so that caps bits can be disabled */
	void (*cpuid)(unsigned int *eax, unsigned int *ebx,
		      unsigned int *ecx, unsigned int *edx);

	/* Unsafe MSR operations.  These will warn or panic on failure. */
	u64 (*read_msr)(unsigned int msr);
	void (*write_msr)(unsigned int msr, unsigned low, unsigned high);

	/*
	 * Safe MSR operations.
	 * read sets err to 0 or -EIO.  write returns 0 or -EIO.
	 */
	u64 (*read_msr_safe)(unsigned int msr, int *err);
	int (*write_msr_safe)(unsigned int msr, unsigned low, unsigned high);

	u64 (*read_pmc)(int counter);

	void (*start_context_switch)(struct task_struct *prev);
	void (*end_context_switch)(struct task_struct *next);
#endif
} __no_randomize_layout;

struct pv_irq_ops {
#ifdef CONFIG_PARAVIRT_XXL
	/*
	 * Get/set interrupt state.  save_fl is expected to use X86_EFLAGS_IF;
	 * all other bits returned from save_fl are undefined.
	 *
	 * NOTE: These functions callers expect the callee to preserve
	 * more registers than the standard C calling convention.
	 */
	struct paravirt_callee_save save_fl;
	struct paravirt_callee_save irq_disable;
	struct paravirt_callee_save irq_enable;

	void (*safe_halt)(void);
	void (*halt)(void);
#endif
} __no_randomize_layout;

struct pv_mmu_ops {
	/* TLB operations */
	void (*flush_tlb_user)(void);
	void (*flush_tlb_kernel)(void);
	void (*flush_tlb_one_user)(unsigned long addr);
	void (*flush_tlb_multi)(const struct cpumask *cpus,
				const struct flush_tlb_info *info);

	void (*tlb_remove_table)(struct mmu_gather *tlb, void *table);

	/* Hook for intercepting the destruction of an mm_struct. */
	void (*exit_mmap)(struct mm_struct *mm);

#ifdef CONFIG_PARAVIRT_XXL
	struct paravirt_callee_save read_cr2;
	void (*write_cr2)(unsigned long);

	unsigned long (*read_cr3)(void);
	void (*write_cr3)(unsigned long);

	/* Hooks for intercepting the creation/use of an mm_struct. */
	void (*activate_mm)(struct mm_struct *prev,
			    struct mm_struct *next);
	void (*dup_mmap)(struct mm_struct *oldmm,
			 struct mm_struct *mm);

	/* Hooks for allocating and freeing a pagetable top-level */
	int  (*pgd_alloc)(struct mm_struct *mm);
	void (*pgd_free)(struct mm_struct *mm, pgd_t *pgd);

	/*
	 * Hooks for allocating/releasing pagetable pages when they're
	 * attached to a pagetable
	 */
	void (*alloc_pte)(struct mm_struct *mm, unsigned long pfn);
	void (*alloc_pmd)(struct mm_struct *mm, unsigned long pfn);
	void (*alloc_pud)(struct mm_struct *mm, unsigned long pfn);
	void (*alloc_p4d)(struct mm_struct *mm, unsigned long pfn);
	void (*release_pte)(unsigned long pfn);
	void (*release_pmd)(unsigned long pfn);
	void (*release_pud)(unsigned long pfn);
	void (*release_p4d)(unsigned long pfn);

	/* Pagetable manipulation functions */
	void (*set_pte)(pte_t *ptep, pte_t pteval);
	void (*set_pmd)(pmd_t *pmdp, pmd_t pmdval);

	pte_t (*ptep_modify_prot_start)(struct vm_area_struct *vma, unsigned long addr,
					pte_t *ptep);
	void (*ptep_modify_prot_commit)(struct vm_area_struct *vma, unsigned long addr,
					pte_t *ptep, pte_t pte);

	struct paravirt_callee_save pte_val;
	struct paravirt_callee_save make_pte;

	struct paravirt_callee_save pgd_val;
	struct paravirt_callee_save make_pgd;

	void (*set_pud)(pud_t *pudp, pud_t pudval);

	struct paravirt_callee_save pmd_val;
	struct paravirt_callee_save make_pmd;

	struct paravirt_callee_save pud_val;
	struct paravirt_callee_save make_pud;

	void (*set_p4d)(p4d_t *p4dp, p4d_t p4dval);

#if CONFIG_PGTABLE_LEVELS >= 5
	struct paravirt_callee_save p4d_val;
	struct paravirt_callee_save make_p4d;

	void (*set_pgd)(pgd_t *pgdp, pgd_t pgdval);
#endif	/* CONFIG_PGTABLE_LEVELS >= 5 */

	struct pv_lazy_ops lazy_mode;

	/* dom0 ops */

	/* Sometimes the physical address is a pfn, and sometimes its
	   an mfn.  We can tell which is which from the index. */
	void (*set_fixmap)(unsigned /* enum fixed_addresses */ idx,
			   phys_addr_t phys, pgprot_t flags);
#endif
} __no_randomize_layout;

struct arch_spinlock;
#ifdef CONFIG_SMP
#include <asm/spinlock_types.h>
#endif

struct qspinlock;

struct pv_lock_ops {
	void (*queued_spin_lock_slowpath)(struct qspinlock *lock, u32 val);
	struct paravirt_callee_save queued_spin_unlock;

	void (*wait)(u8 *ptr, u8 val);
	void (*kick)(int cpu);

	struct paravirt_callee_save vcpu_is_preempted;
} __no_randomize_layout;

/* This contains all the paravirt structures: we get a convenient
 * number for each function using the offset which we use to indicate
 * what to patch. */
struct paravirt_patch_template {
	struct pv_cpu_ops	cpu;
	struct pv_irq_ops	irq;
	struct pv_mmu_ops	mmu;
	struct pv_lock_ops	lock;
} __no_randomize_layout;

extern struct pv_info pv_info;
extern struct paravirt_patch_template pv_ops;
extern void (*paravirt_iret)(void);

#define PARAVIRT_PATCH(x)					\
	(offsetof(struct paravirt_patch_template, x) / sizeof(void *))

#define paravirt_type(op)				\
	[paravirt_typenum] "i" (PARAVIRT_PATCH(op)),	\
	[paravirt_opptr] "i" (&(pv_ops.op))
#define paravirt_clobber(clobber)		\
	[paravirt_clobber] "i" (clobber)

/*
 * Generate some code, and mark it as patchable by the
 * apply_paravirt() alternate instruction patcher.
 */
#define _paravirt_alt(insn_string, type, clobber)	\
	"771:\n\t" insn_string "\n" "772:\n"		\
	".pushsection .parainstructions,\"a\"\n"	\
	_ASM_ALIGN "\n"					\
	_ASM_PTR " 771b\n"				\
	"  .byte " type "\n"				\
	"  .byte 772b-771b\n"				\
	"  .short " clobber "\n"			\
	".popsection\n"

/* Generate patchable code, with the default asm parameters. */
#define paravirt_alt(insn_string)					\
	_paravirt_alt(insn_string, "%c[paravirt_typenum]", "%c[paravirt_clobber]")

/* Simple instruction patching code. */
#define NATIVE_LABEL(a,x,b) "\n\t.globl " a #x "_" #b "\n" a #x "_" #b ":\n\t"

unsigned int paravirt_patch(u8 type, void *insn_buff, unsigned long addr, unsigned int len);

int paravirt_disable_iospace(void);

/*
 * This generates an indirect call based on the operation type number.
 * The type number, computed in PARAVIRT_PATCH, is derived from the
 * offset into the paravirt_patch_template structure, and can therefore be
 * freely converted back into a structure offset.
 */
#define PARAVIRT_CALL					\
	ANNOTATE_RETPOLINE_SAFE				\
	"call *%c[paravirt_opptr];"

/*
 * These macros are intended to wrap calls through one of the paravirt
 * ops structs, so that they can be later identified and patched at
 * runtime.
 *
 * Normally, a call to a pv_op function is a simple indirect call:
 * (pv_op_struct.operations)(args...).
 *
 * Unfortunately, this is a relatively slow operation for modern CPUs,
 * because it cannot necessarily determine what the destination
 * address is.  In this case, the address is a runtime constant, so at
 * the very least we can patch the call to e a simple direct call, or
 * ideally, patch an inline implementation into the callsite.  (Direct
 * calls are essentially free, because the call and return addresses
 * are completely predictable.)
 *
 * For i386, these macros rely on the standard gcc "regparm(3)" calling
 * convention, in which the first three arguments are placed in %eax,
 * %edx, %ecx (in that order), and the remaining arguments are placed
 * on the stack.  All caller-save registers (eax,edx,ecx) are expected
 * to be modified (either clobbered or used for return values).
 * X86_64, on the other hand, already specifies a register-based calling
 * conventions, returning at %rax, with parameters going on %rdi, %rsi,
 * %rdx, and %rcx. Note that for this reason, x86_64 does not need any
 * special handling for dealing with 4 arguments, unlike i386.
 * However, x86_64 also have to clobber all caller saved registers, which
 * unfortunately, are quite a bit (r8 - r11)
 *
 * The call instruction itself is marked by placing its start address
 * and size into the .parainstructions section, so that
 * apply_paravirt() in arch/i386/kernel/alternative.c can do the
 * appropriate patching under the control of the backend pv_init_ops
 * implementation.
 *
 * Unfortunately there's no way to get gcc to generate the args setup
 * for the call, and then allow the call itself to be generated by an
 * inline asm.  Because of this, we must do the complete arg setup and
 * return value handling from within these macros.  This is fairly
 * cumbersome.
 *
 * There are 5 sets of PVOP_* macros for dealing with 0-4 arguments.
 * It could be extended to more arguments, but there would be little
 * to be gained from that.  For each number of arguments, there are
 * the two VCALL and CALL variants for void and non-void functions.
 *
 * When there is a return value, the invoker of the macro must specify
 * the return type.  The macro then uses sizeof() on that type to
 * determine whether its a 32 or 64 bit value, and places the return
 * in the right register(s) (just %eax for 32-bit, and %edx:%eax for
 * 64-bit). For x86_64 machines, it just returns at %rax regardless of
 * the return value size.
 *
 * 64-bit arguments are passed as a pair of adjacent 32-bit arguments
 * i386 also passes 64-bit arguments as a pair of adjacent 32-bit arguments
 * in low,high order
 *
 * Small structures are passed and returned in registers.  The macro
 * calling convention can't directly deal with this, so the wrapper
 * functions must do this.
 *
 * These PVOP_* macros are only defined within this header.  This
 * means that all uses must be wrapped in inline functions.  This also
 * makes sure the incoming and outgoing types are always correct.
 */
#ifdef CONFIG_X86_32
#define PVOP_CALL_ARGS							\
	unsigned long __eax = __eax, __edx = __edx, __ecx = __ecx;

#define PVOP_CALL_ARG1(x)		"a" ((unsigned long)(x))
#define PVOP_CALL_ARG2(x)		"d" ((unsigned long)(x))
#define PVOP_CALL_ARG3(x)		"c" ((unsigned long)(x))

#define PVOP_VCALL_CLOBBERS		"=a" (__eax), "=d" (__edx),	\
					"=c" (__ecx)
#define PVOP_CALL_CLOBBERS		PVOP_VCALL_CLOBBERS

#define PVOP_VCALLEE_CLOBBERS		"=a" (__eax), "=d" (__edx)
#define PVOP_CALLEE_CLOBBERS		PVOP_VCALLEE_CLOBBERS

#define EXTRA_CLOBBERS
#define VEXTRA_CLOBBERS
#else  /* CONFIG_X86_64 */
/* [re]ax isn't an arg, but the return val */
#define PVOP_CALL_ARGS						\
	unsigned long __edi = __edi, __esi = __esi,		\
		__edx = __edx, __ecx = __ecx, __eax = __eax;

#define PVOP_CALL_ARG1(x)		"D" ((unsigned long)(x))
#define PVOP_CALL_ARG2(x)		"S" ((unsigned long)(x))
#define PVOP_CALL_ARG3(x)		"d" ((unsigned long)(x))
#define PVOP_CALL_ARG4(x)		"c" ((unsigned long)(x))

#define PVOP_VCALL_CLOBBERS	"=D" (__edi),				\
				"=S" (__esi), "=d" (__edx),		\
				"=c" (__ecx)
#define PVOP_CALL_CLOBBERS	PVOP_VCALL_CLOBBERS, "=a" (__eax)

/* void functions are still allowed [re]ax for scratch */
#define PVOP_VCALLEE_CLOBBERS	"=a" (__eax)
#define PVOP_CALLEE_CLOBBERS	PVOP_VCALLEE_CLOBBERS

#define EXTRA_CLOBBERS	 , "r8", "r9", "r10", "r11"
#define VEXTRA_CLOBBERS	 , "rax", "r8", "r9", "r10", "r11"
#endif	/* CONFIG_X86_32 */

#ifdef CONFIG_PARAVIRT_DEBUG
#define PVOP_TEST_NULL(op)	BUG_ON(pv_ops.op == NULL)
#else
#define PVOP_TEST_NULL(op)	((void)pv_ops.op)
#endif

#define PVOP_RETVAL(rettype)						\
	({	unsigned long __mask = ~0UL;				\
		BUILD_BUG_ON(sizeof(rettype) > sizeof(unsigned long));	\
		switch (sizeof(rettype)) {				\
		case 1: __mask =       0xffUL; break;			\
		case 2: __mask =     0xffffUL; break;			\
		case 4: __mask = 0xffffffffUL; break;			\
		default: break;						\
		}							\
		__mask & __eax;						\
	})


#define ____PVOP_CALL(ret, op, clbr, call_clbr, extra_clbr, ...)	\
	({								\
		PVOP_CALL_ARGS;						\
		PVOP_TEST_NULL(op);					\
		asm volatile(paravirt_alt(PARAVIRT_CALL)		\
			     : call_clbr, ASM_CALL_CONSTRAINT		\
			     : paravirt_type(op),			\
			       paravirt_clobber(clbr),			\
			       ##__VA_ARGS__				\
			     : "memory", "cc" extra_clbr);		\
		ret;							\
	})

#define ____PVOP_ALT_CALL(ret, op, alt, cond, clbr, call_clbr,		\
			  extra_clbr, ...)				\
	({								\
		PVOP_CALL_ARGS;						\
		PVOP_TEST_NULL(op);					\
		asm volatile(ALTERNATIVE(paravirt_alt(PARAVIRT_CALL),	\
					 alt, cond)			\
			     : call_clbr, ASM_CALL_CONSTRAINT		\
			     : paravirt_type(op),			\
			       paravirt_clobber(clbr),			\
			       ##__VA_ARGS__				\
			     : "memory", "cc" extra_clbr);		\
		ret;							\
	})

#define __PVOP_CALL(rettype, op, ...)					\
	____PVOP_CALL(PVOP_RETVAL(rettype), op, CLBR_ANY,		\
		      PVOP_CALL_CLOBBERS, EXTRA_CLOBBERS, ##__VA_ARGS__)

#define __PVOP_ALT_CALL(rettype, op, alt, cond, ...)			\
	____PVOP_ALT_CALL(PVOP_RETVAL(rettype), op, alt, cond, CLBR_ANY,\
			  PVOP_CALL_CLOBBERS, EXTRA_CLOBBERS,		\
			  ##__VA_ARGS__)

#define __PVOP_CALLEESAVE(rettype, op, ...)				\
	____PVOP_CALL(PVOP_RETVAL(rettype), op.func, CLBR_RET_REG,	\
		      PVOP_CALLEE_CLOBBERS, , ##__VA_ARGS__)

#define __PVOP_ALT_CALLEESAVE(rettype, op, alt, cond, ...)		\
	____PVOP_ALT_CALL(PVOP_RETVAL(rettype), op.func, alt, cond,	\
			  CLBR_RET_REG, PVOP_CALLEE_CLOBBERS, , ##__VA_ARGS__)


#define __PVOP_VCALL(op, ...)						\
	(void)____PVOP_CALL(, op, CLBR_ANY, PVOP_VCALL_CLOBBERS,	\
		       VEXTRA_CLOBBERS, ##__VA_ARGS__)

#define __PVOP_ALT_VCALL(op, alt, cond, ...)				\
	(void)____PVOP_ALT_CALL(, op, alt, cond, CLBR_ANY,		\
				PVOP_VCALL_CLOBBERS, VEXTRA_CLOBBERS,	\
				##__VA_ARGS__)

#define __PVOP_VCALLEESAVE(op, ...)					\
	(void)____PVOP_CALL(, op.func, CLBR_RET_REG,			\
			    PVOP_VCALLEE_CLOBBERS, , ##__VA_ARGS__)

#define __PVOP_ALT_VCALLEESAVE(op, alt, cond, ...)			\
	(void)____PVOP_ALT_CALL(, op.func, alt, cond, CLBR_RET_REG,	\
				PVOP_VCALLEE_CLOBBERS, , ##__VA_ARGS__)


#define PVOP_CALL0(rettype, op)						\
	__PVOP_CALL(rettype, op)
#define PVOP_VCALL0(op)							\
	__PVOP_VCALL(op)
#define PVOP_ALT_CALL0(rettype, op, alt, cond)				\
	__PVOP_ALT_CALL(rettype, op, alt, cond)
#define PVOP_ALT_VCALL0(op, alt, cond)					\
	__PVOP_ALT_VCALL(op, alt, cond)

#define PVOP_CALLEE0(rettype, op)					\
	__PVOP_CALLEESAVE(rettype, op)
#define PVOP_VCALLEE0(op)						\
	__PVOP_VCALLEESAVE(op)
#define PVOP_ALT_CALLEE0(rettype, op, alt, cond)			\
	__PVOP_ALT_CALLEESAVE(rettype, op, alt, cond)
#define PVOP_ALT_VCALLEE0(op, alt, cond)				\
	__PVOP_ALT_VCALLEESAVE(op, alt, cond)


#define PVOP_CALL1(rettype, op, arg1)					\
	__PVOP_CALL(rettype, op, PVOP_CALL_ARG1(arg1))
#define PVOP_VCALL1(op, arg1)						\
	__PVOP_VCALL(op, PVOP_CALL_ARG1(arg1))
#define PVOP_ALT_VCALL1(op, arg1, alt, cond)				\
	__PVOP_ALT_VCALL(op, alt, cond, PVOP_CALL_ARG1(arg1))

#define PVOP_CALLEE1(rettype, op, arg1)					\
	__PVOP_CALLEESAVE(rettype, op, PVOP_CALL_ARG1(arg1))
#define PVOP_VCALLEE1(op, arg1)						\
	__PVOP_VCALLEESAVE(op, PVOP_CALL_ARG1(arg1))
#define PVOP_ALT_CALLEE1(rettype, op, arg1, alt, cond)			\
	__PVOP_ALT_CALLEESAVE(rettype, op, alt, cond, PVOP_CALL_ARG1(arg1))
#define PVOP_ALT_VCALLEE1(op, arg1, alt, cond)				\
	__PVOP_ALT_VCALLEESAVE(op, alt, cond, PVOP_CALL_ARG1(arg1))


#define PVOP_CALL2(rettype, op, arg1, arg2)				\
	__PVOP_CALL(rettype, op, PVOP_CALL_ARG1(arg1), PVOP_CALL_ARG2(arg2))
#define PVOP_VCALL2(op, arg1, arg2)					\
	__PVOP_VCALL(op, PVOP_CALL_ARG1(arg1), PVOP_CALL_ARG2(arg2))

#define PVOP_CALL3(rettype, op, arg1, arg2, arg3)			\
	__PVOP_CALL(rettype, op, PVOP_CALL_ARG1(arg1),			\
		    PVOP_CALL_ARG2(arg2), PVOP_CALL_ARG3(arg3))
#define PVOP_VCALL3(op, arg1, arg2, arg3)				\
	__PVOP_VCALL(op, PVOP_CALL_ARG1(arg1),				\
		     PVOP_CALL_ARG2(arg2), PVOP_CALL_ARG3(arg3))

#define PVOP_CALL4(rettype, op, arg1, arg2, arg3, arg4)			\
	__PVOP_CALL(rettype, op,					\
		    PVOP_CALL_ARG1(arg1), PVOP_CALL_ARG2(arg2),		\
		    PVOP_CALL_ARG3(arg3), PVOP_CALL_ARG4(arg4))
#define PVOP_VCALL4(op, arg1, arg2, arg3, arg4)				\
	__PVOP_VCALL(op, PVOP_CALL_ARG1(arg1), PVOP_CALL_ARG2(arg2),	\
		     PVOP_CALL_ARG3(arg3), PVOP_CALL_ARG4(arg4))

/* Lazy mode for batching updates / context switch */
enum paravirt_lazy_mode {
	PARAVIRT_LAZY_NONE,
	PARAVIRT_LAZY_MMU,
	PARAVIRT_LAZY_CPU,
};

enum paravirt_lazy_mode paravirt_get_lazy_mode(void);
void paravirt_start_context_switch(struct task_struct *prev);
void paravirt_end_context_switch(struct task_struct *next);

void paravirt_enter_lazy_mmu(void);
void paravirt_leave_lazy_mmu(void);
void paravirt_flush_lazy_mmu(void);

void _paravirt_nop(void);
u64 _paravirt_ident_64(u64);

#define paravirt_nop	((void *)_paravirt_nop)

/* These all sit in the .parainstructions section to tell us what to patch. */
struct paravirt_patch_site {
	u8 *instr;		/* original instructions */
	u8 type;		/* type of this instruction */
	u8 len;			/* length of original instruction */
};

extern struct paravirt_patch_site __parainstructions[],
	__parainstructions_end[];

#endif	/* __ASSEMBLY__ */

#endif	/* _ASM_X86_PARAVIRT_TYPES_H */
