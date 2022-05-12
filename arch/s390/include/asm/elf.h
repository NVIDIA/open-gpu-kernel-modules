/* SPDX-License-Identifier: GPL-2.0 */
/*
 *  S390 version
 *
 *  Derived from "include/asm-i386/elf.h"
 */

#ifndef __ASMS390_ELF_H
#define __ASMS390_ELF_H

/* s390 relocations defined by the ABIs */
#define R_390_NONE		0	/* No reloc.  */
#define R_390_8			1	/* Direct 8 bit.  */
#define R_390_12		2	/* Direct 12 bit.  */
#define R_390_16		3	/* Direct 16 bit.  */
#define R_390_32		4	/* Direct 32 bit.  */
#define R_390_PC32		5	/* PC relative 32 bit.	*/
#define R_390_GOT12		6	/* 12 bit GOT offset.  */
#define R_390_GOT32		7	/* 32 bit GOT offset.  */
#define R_390_PLT32		8	/* 32 bit PC relative PLT address.  */
#define R_390_COPY		9	/* Copy symbol at runtime.  */
#define R_390_GLOB_DAT		10	/* Create GOT entry.  */
#define R_390_JMP_SLOT		11	/* Create PLT entry.  */
#define R_390_RELATIVE		12	/* Adjust by program base.  */
#define R_390_GOTOFF32		13	/* 32 bit offset to GOT.	 */
#define R_390_GOTPC		14	/* 32 bit PC rel. offset to GOT.  */
#define R_390_GOT16		15	/* 16 bit GOT offset.  */
#define R_390_PC16		16	/* PC relative 16 bit.	*/
#define R_390_PC16DBL		17	/* PC relative 16 bit shifted by 1.  */
#define R_390_PLT16DBL		18	/* 16 bit PC rel. PLT shifted by 1.  */
#define R_390_PC32DBL		19	/* PC relative 32 bit shifted by 1.  */
#define R_390_PLT32DBL		20	/* 32 bit PC rel. PLT shifted by 1.  */
#define R_390_GOTPCDBL		21	/* 32 bit PC rel. GOT shifted by 1.  */
#define R_390_64		22	/* Direct 64 bit.  */
#define R_390_PC64		23	/* PC relative 64 bit.	*/
#define R_390_GOT64		24	/* 64 bit GOT offset.  */
#define R_390_PLT64		25	/* 64 bit PC relative PLT address.  */
#define R_390_GOTENT		26	/* 32 bit PC rel. to GOT entry >> 1. */
#define R_390_GOTOFF16		27	/* 16 bit offset to GOT. */
#define R_390_GOTOFF64		28	/* 64 bit offset to GOT. */
#define R_390_GOTPLT12		29	/* 12 bit offset to jump slot.	*/
#define R_390_GOTPLT16		30	/* 16 bit offset to jump slot.	*/
#define R_390_GOTPLT32		31	/* 32 bit offset to jump slot.	*/
#define R_390_GOTPLT64		32	/* 64 bit offset to jump slot.	*/
#define R_390_GOTPLTENT		33	/* 32 bit rel. offset to jump slot.  */
#define R_390_PLTOFF16		34	/* 16 bit offset from GOT to PLT. */
#define R_390_PLTOFF32		35	/* 32 bit offset from GOT to PLT. */
#define R_390_PLTOFF64		36	/* 16 bit offset from GOT to PLT. */
#define R_390_TLS_LOAD		37	/* Tag for load insn in TLS code. */
#define R_390_TLS_GDCALL	38	/* Tag for function call in general
                                           dynamic TLS code.  */
#define R_390_TLS_LDCALL	39	/* Tag for function call in local
                                           dynamic TLS code.  */
#define R_390_TLS_GD32		40	/* Direct 32 bit for general dynamic
                                           thread local data.  */
#define R_390_TLS_GD64		41	/* Direct 64 bit for general dynamic
                                           thread local data.  */
#define R_390_TLS_GOTIE12	42	/* 12 bit GOT offset for static TLS
                                           block offset.  */
#define R_390_TLS_GOTIE32	43	/* 32 bit GOT offset for static TLS
                                           block offset.  */
#define R_390_TLS_GOTIE64	44	/* 64 bit GOT offset for static TLS
                                           block offset.  */
#define R_390_TLS_LDM32		45	/* Direct 32 bit for local dynamic
                                           thread local data in LD code.  */
#define R_390_TLS_LDM64		46	/* Direct 64 bit for local dynamic
                                           thread local data in LD code.  */
#define R_390_TLS_IE32		47	/* 32 bit address of GOT entry for
                                           negated static TLS block offset.  */
#define R_390_TLS_IE64		48	/* 64 bit address of GOT entry for
                                           negated static TLS block offset.  */
#define R_390_TLS_IEENT		49	/* 32 bit rel. offset to GOT entry for
                                           negated static TLS block offset.  */
#define R_390_TLS_LE32		50	/* 32 bit negated offset relative to
                                           static TLS block.  */
#define R_390_TLS_LE64		51	/* 64 bit negated offset relative to
                                           static TLS block.  */
#define R_390_TLS_LDO32		52	/* 32 bit offset relative to TLS
                                           block.  */
#define R_390_TLS_LDO64		53	/* 64 bit offset relative to TLS
                                           block.  */
#define R_390_TLS_DTPMOD	54	/* ID of module containing symbol.  */
#define R_390_TLS_DTPOFF	55	/* Offset in TLS block.  */
#define R_390_TLS_TPOFF		56	/* Negate offset in static TLS
                                           block.  */
#define R_390_20		57	/* Direct 20 bit.  */
#define R_390_GOT20		58	/* 20 bit GOT offset.  */
#define R_390_GOTPLT20		59	/* 20 bit offset to jump slot.  */
#define R_390_TLS_GOTIE20	60	/* 20 bit GOT offset for static TLS
					   block offset.  */
/* Keep this the last entry.  */
#define R_390_NUM	61

/* Bits present in AT_HWCAP. */
#define HWCAP_S390_ESAN3	1
#define HWCAP_S390_ZARCH	2
#define HWCAP_S390_STFLE	4
#define HWCAP_S390_MSA		8
#define HWCAP_S390_LDISP	16
#define HWCAP_S390_EIMM		32
#define HWCAP_S390_DFP		64
#define HWCAP_S390_HPAGE	128
#define HWCAP_S390_ETF3EH	256
#define HWCAP_S390_HIGH_GPRS	512
#define HWCAP_S390_TE		1024
#define HWCAP_S390_VXRS		2048
#define HWCAP_S390_VXRS_BCD	4096
#define HWCAP_S390_VXRS_EXT	8192
#define HWCAP_S390_GS		16384
#define HWCAP_S390_VXRS_EXT2	32768
#define HWCAP_S390_VXRS_PDE	65536
#define HWCAP_S390_SORT		131072
#define HWCAP_S390_DFLT		262144

/* Internal bits, not exposed via elf */
#define HWCAP_INT_SIE		1UL

/*
 * These are used to set parameters in the core dumps.
 */
#define ELF_CLASS	ELFCLASS64
#define ELF_DATA	ELFDATA2MSB
#define ELF_ARCH	EM_S390

/* s390 specific phdr types */
#define PT_S390_PGSTE	0x70000000

/*
 * ELF register definitions..
 */

#include <linux/compat.h>

#include <asm/ptrace.h>
#include <asm/syscall.h>
#include <asm/user.h>

typedef s390_fp_regs elf_fpregset_t;
typedef s390_regs elf_gregset_t;

typedef s390_fp_regs compat_elf_fpregset_t;
typedef s390_compat_regs compat_elf_gregset_t;

#include <linux/sched/mm.h>	/* for task_struct */
#include <asm/mmu_context.h>

#include <asm/vdso.h>

extern unsigned int vdso_enabled;

/*
 * This is used to ensure we don't load something for the wrong architecture.
 */
#define elf_check_arch(x) \
	(((x)->e_machine == EM_S390 || (x)->e_machine == EM_S390_OLD) \
         && (x)->e_ident[EI_CLASS] == ELF_CLASS) 
#define compat_elf_check_arch(x) \
	(((x)->e_machine == EM_S390 || (x)->e_machine == EM_S390_OLD) \
	 && (x)->e_ident[EI_CLASS] == ELF_CLASS)
#define compat_start_thread	start_thread31

struct arch_elf_state {
	int rc;
};

#define INIT_ARCH_ELF_STATE { .rc = 0 }

#define arch_check_elf(ehdr, interp, interp_ehdr, state) (0)
#ifdef CONFIG_PGSTE
#define arch_elf_pt_proc(ehdr, phdr, elf, interp, state)	\
({								\
	struct arch_elf_state *_state = state;			\
	if ((phdr)->p_type == PT_S390_PGSTE &&			\
	    !page_table_allocate_pgste &&			\
	    !test_thread_flag(TIF_PGSTE) &&			\
	    !current->mm->context.alloc_pgste) {		\
		set_thread_flag(TIF_PGSTE);			\
		set_pt_regs_flag(task_pt_regs(current),		\
				 PIF_SYSCALL_RESTART);		\
		_state->rc = -EAGAIN;				\
	}							\
	_state->rc;						\
})
#else
#define arch_elf_pt_proc(ehdr, phdr, elf, interp, state)	\
({								\
	(state)->rc;						\
})
#endif

/* For SVR4/S390 the function pointer to be registered with `atexit` is
   passed in R14. */
#define ELF_PLAT_INIT(_r, load_addr) \
	do { \
		_r->gprs[14] = 0; \
	} while (0)

#define CORE_DUMP_USE_REGSET
#define ELF_EXEC_PAGESIZE	PAGE_SIZE

/* This is the location that an ET_DYN program is loaded if exec'ed.  Typical
   use of this is to invoke "./ld.so someprog" to test out a new version of
   the loader.  We need to make sure that it is out of the way of the program
   that it will "exec", and that there is sufficient room for the brk. 64-bit
   tasks are aligned to 4GB. */
#define ELF_ET_DYN_BASE (is_compat_task() ? \
				(STACK_TOP / 3 * 2) : \
				(STACK_TOP / 3 * 2) & ~((1UL << 32) - 1))

/* This yields a mask that user programs can use to figure out what
   instruction set this CPU supports. */

extern unsigned long elf_hwcap;
#define ELF_HWCAP (elf_hwcap)

/* Internal hardware capabilities, not exposed via elf */

extern unsigned long int_hwcap;

/* This yields a string that ld.so will use to load implementation
   specific libraries for optimization.  This is more specific in
   intent than poking at uname or /proc/cpuinfo.

   For the moment, we have only optimizations for the Intel generations,
   but that could change... */

#define ELF_PLATFORM_SIZE 8
extern char elf_platform[];
#define ELF_PLATFORM (elf_platform)

#ifndef CONFIG_COMPAT
#define SET_PERSONALITY(ex) \
do {								\
	set_personality(PER_LINUX |				\
		(current->personality & (~PER_MASK)));		\
	current->thread.sys_call_table = sys_call_table;	\
} while (0)
#else /* CONFIG_COMPAT */
#define SET_PERSONALITY(ex)					\
do {								\
	if (personality(current->personality) != PER_LINUX32)	\
		set_personality(PER_LINUX |			\
			(current->personality & ~PER_MASK));	\
	if ((ex).e_ident[EI_CLASS] == ELFCLASS32) {		\
		set_thread_flag(TIF_31BIT);			\
		current->thread.sys_call_table =		\
			sys_call_table_emu;			\
	} else {						\
		clear_thread_flag(TIF_31BIT);			\
		current->thread.sys_call_table =		\
			sys_call_table;				\
	}							\
} while (0)
#endif /* CONFIG_COMPAT */

/*
 * Cache aliasing on the latest machines calls for a mapping granularity
 * of 512KB for the anonymous mapping base. For 64-bit processes use a
 * 512KB alignment and a randomization of up to 1GB. For 31-bit processes
 * the virtual address space is limited, use no alignment and limit the
 * randomization to 8MB.
 * For the additional randomization of the program break use 32MB for
 * 64-bit and 8MB for 31-bit.
 */
#define BRK_RND_MASK	(is_compat_task() ? 0x7ffUL : 0x1fffUL)
#define MMAP_RND_MASK	(is_compat_task() ? 0x7ffUL : 0x3ff80UL)
#define MMAP_ALIGN_MASK	(is_compat_task() ? 0 : 0x7fUL)
#define STACK_RND_MASK	MMAP_RND_MASK

/* update AT_VECTOR_SIZE_ARCH if the number of NEW_AUX_ENT entries changes */
#define ARCH_DLINFO							    \
do {									    \
	if (vdso_enabled)						    \
		NEW_AUX_ENT(AT_SYSINFO_EHDR,				    \
			    (unsigned long)current->mm->context.vdso_base); \
} while (0)

struct linux_binprm;

#define ARCH_HAS_SETUP_ADDITIONAL_PAGES 1
int arch_setup_additional_pages(struct linux_binprm *, int);

#endif
