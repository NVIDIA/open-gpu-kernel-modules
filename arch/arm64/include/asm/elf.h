/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2012 ARM Ltd.
 */
#ifndef __ASM_ELF_H
#define __ASM_ELF_H

#include <asm/hwcap.h>

/*
 * ELF register definitions..
 */
#include <asm/ptrace.h>
#include <asm/user.h>

/*
 * AArch64 static relocation types.
 */

/* Miscellaneous. */
#define R_ARM_NONE			0
#define R_AARCH64_NONE			256

/* Data. */
#define R_AARCH64_ABS64			257
#define R_AARCH64_ABS32			258
#define R_AARCH64_ABS16			259
#define R_AARCH64_PREL64		260
#define R_AARCH64_PREL32		261
#define R_AARCH64_PREL16		262

/* Instructions. */
#define R_AARCH64_MOVW_UABS_G0		263
#define R_AARCH64_MOVW_UABS_G0_NC	264
#define R_AARCH64_MOVW_UABS_G1		265
#define R_AARCH64_MOVW_UABS_G1_NC	266
#define R_AARCH64_MOVW_UABS_G2		267
#define R_AARCH64_MOVW_UABS_G2_NC	268
#define R_AARCH64_MOVW_UABS_G3		269

#define R_AARCH64_MOVW_SABS_G0		270
#define R_AARCH64_MOVW_SABS_G1		271
#define R_AARCH64_MOVW_SABS_G2		272

#define R_AARCH64_LD_PREL_LO19		273
#define R_AARCH64_ADR_PREL_LO21		274
#define R_AARCH64_ADR_PREL_PG_HI21	275
#define R_AARCH64_ADR_PREL_PG_HI21_NC	276
#define R_AARCH64_ADD_ABS_LO12_NC	277
#define R_AARCH64_LDST8_ABS_LO12_NC	278

#define R_AARCH64_TSTBR14		279
#define R_AARCH64_CONDBR19		280
#define R_AARCH64_JUMP26		282
#define R_AARCH64_CALL26		283
#define R_AARCH64_LDST16_ABS_LO12_NC	284
#define R_AARCH64_LDST32_ABS_LO12_NC	285
#define R_AARCH64_LDST64_ABS_LO12_NC	286
#define R_AARCH64_LDST128_ABS_LO12_NC	299

#define R_AARCH64_MOVW_PREL_G0		287
#define R_AARCH64_MOVW_PREL_G0_NC	288
#define R_AARCH64_MOVW_PREL_G1		289
#define R_AARCH64_MOVW_PREL_G1_NC	290
#define R_AARCH64_MOVW_PREL_G2		291
#define R_AARCH64_MOVW_PREL_G2_NC	292
#define R_AARCH64_MOVW_PREL_G3		293

#define R_AARCH64_RELATIVE		1027

/*
 * These are used to set parameters in the core dumps.
 */
#define ELF_CLASS	ELFCLASS64
#ifdef __AARCH64EB__
#define ELF_DATA	ELFDATA2MSB
#else
#define ELF_DATA	ELFDATA2LSB
#endif
#define ELF_ARCH	EM_AARCH64

/*
 * This yields a string that ld.so will use to load implementation
 * specific libraries for optimization.  This is more specific in
 * intent than poking at uname or /proc/cpuinfo.
 */
#define ELF_PLATFORM_SIZE	16
#ifdef __AARCH64EB__
#define ELF_PLATFORM		("aarch64_be")
#else
#define ELF_PLATFORM		("aarch64")
#endif

/*
 * This is used to ensure we don't load something for the wrong architecture.
 */
#define elf_check_arch(x)		((x)->e_machine == EM_AARCH64)

/*
 * An executable for which elf_read_implies_exec() returns TRUE will
 * have the READ_IMPLIES_EXEC personality flag set automatically.
 *
 * The decision process for determining the results are:
 *
 *                CPU*: | arm32      | arm64      |
 * ELF:                 |            |            |
 * ---------------------|------------|------------|
 * missing PT_GNU_STACK | exec-all   | exec-none  |
 * PT_GNU_STACK == RWX  | exec-stack | exec-stack |
 * PT_GNU_STACK == RW   | exec-none  | exec-none  |
 *
 *  exec-all  : all PROT_READ user mappings are executable, except when
 *              backed by files on a noexec-filesystem.
 *  exec-none : only PROT_EXEC user mappings are executable.
 *  exec-stack: only the stack and PROT_EXEC user mappings are executable.
 *
 *  *all arm64 CPUs support NX, so there is no "lacks NX" column.
 *
 */
#define compat_elf_read_implies_exec(ex, stk)	(stk == EXSTACK_DEFAULT)

#define CORE_DUMP_USE_REGSET
#define ELF_EXEC_PAGESIZE	PAGE_SIZE

/*
 * This is the base location for PIE (ET_DYN with INTERP) loads. On
 * 64-bit, this is above 4GB to leave the entire 32-bit address
 * space open for things that want to use the area for 32-bit pointers.
 */
#ifdef CONFIG_ARM64_FORCE_52BIT
#define ELF_ET_DYN_BASE		(2 * TASK_SIZE_64 / 3)
#else
#define ELF_ET_DYN_BASE		(2 * DEFAULT_MAP_WINDOW_64 / 3)
#endif /* CONFIG_ARM64_FORCE_52BIT */

#ifndef __ASSEMBLY__

#include <uapi/linux/elf.h>
#include <linux/bug.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <asm/processor.h> /* for signal_minsigstksz, used by ARCH_DLINFO */

typedef unsigned long elf_greg_t;

#define ELF_NGREG (sizeof(struct user_pt_regs) / sizeof(elf_greg_t))
#define ELF_CORE_COPY_REGS(dest, regs)	\
	*(struct user_pt_regs *)&(dest) = (regs)->user_regs;

typedef elf_greg_t elf_gregset_t[ELF_NGREG];
typedef struct user_fpsimd_state elf_fpregset_t;

/*
 * When the program starts, a1 contains a pointer to a function to be
 * registered with atexit, as per the SVR4 ABI.  A value of 0 means we have no
 * such handler.
 */
#define ELF_PLAT_INIT(_r, load_addr)	(_r)->regs[0] = 0

#define SET_PERSONALITY(ex)						\
({									\
	clear_thread_flag(TIF_32BIT);					\
	current->personality &= ~READ_IMPLIES_EXEC;			\
})

/* update AT_VECTOR_SIZE_ARCH if the number of NEW_AUX_ENT entries changes */
#define ARCH_DLINFO							\
do {									\
	NEW_AUX_ENT(AT_SYSINFO_EHDR,					\
		    (elf_addr_t)current->mm->context.vdso);		\
									\
	/*								\
	 * Should always be nonzero unless there's a kernel bug.	\
	 * If we haven't determined a sensible value to give to		\
	 * userspace, omit the entry:					\
	 */								\
	if (likely(signal_minsigstksz))					\
		NEW_AUX_ENT(AT_MINSIGSTKSZ, signal_minsigstksz);	\
	else								\
		NEW_AUX_ENT(AT_IGNORE, 0);				\
} while (0)

#define ARCH_HAS_SETUP_ADDITIONAL_PAGES
struct linux_binprm;
extern int arch_setup_additional_pages(struct linux_binprm *bprm,
				       int uses_interp);

/* 1GB of VA */
#ifdef CONFIG_COMPAT
#define STACK_RND_MASK			(test_thread_flag(TIF_32BIT) ? \
						0x7ff >> (PAGE_SHIFT - 12) : \
						0x3ffff >> (PAGE_SHIFT - 12))
#else
#define STACK_RND_MASK			(0x3ffff >> (PAGE_SHIFT - 12))
#endif

#ifdef __AARCH64EB__
#define COMPAT_ELF_PLATFORM		("v8b")
#else
#define COMPAT_ELF_PLATFORM		("v8l")
#endif

#ifdef CONFIG_COMPAT

/* PIE load location for compat arm. Must match ARM ELF_ET_DYN_BASE. */
#define COMPAT_ELF_ET_DYN_BASE		0x000400000UL

/* AArch32 registers. */
#define COMPAT_ELF_NGREG		18
typedef unsigned int			compat_elf_greg_t;
typedef compat_elf_greg_t		compat_elf_gregset_t[COMPAT_ELF_NGREG];

/* AArch32 EABI. */
#define EF_ARM_EABI_MASK		0xff000000
#define compat_elf_check_arch(x)	(system_supports_32bit_el0() && \
					 ((x)->e_machine == EM_ARM) && \
					 ((x)->e_flags & EF_ARM_EABI_MASK))

#define compat_start_thread		compat_start_thread
/*
 * Unlike the native SET_PERSONALITY macro, the compat version maintains
 * READ_IMPLIES_EXEC across an execve() since this is the behaviour on
 * arch/arm/.
 */
#define COMPAT_SET_PERSONALITY(ex)					\
({									\
	set_thread_flag(TIF_32BIT);					\
 })
#ifdef CONFIG_COMPAT_VDSO
#define COMPAT_ARCH_DLINFO						\
do {									\
	/*								\
	 * Note that we use Elf64_Off instead of elf_addr_t because	\
	 * elf_addr_t in compat is defined as Elf32_Addr and casting	\
	 * current->mm->context.vdso to it triggers a cast warning of	\
	 * cast from pointer to integer of different size.		\
	 */								\
	NEW_AUX_ENT(AT_SYSINFO_EHDR,					\
			(Elf64_Off)current->mm->context.vdso);		\
} while (0)
#else
#define COMPAT_ARCH_DLINFO
#endif
extern int aarch32_setup_additional_pages(struct linux_binprm *bprm,
					  int uses_interp);
#define compat_arch_setup_additional_pages \
					aarch32_setup_additional_pages

#endif /* CONFIG_COMPAT */

struct arch_elf_state {
	int flags;
};

#define ARM64_ELF_BTI		(1 << 0)

#define INIT_ARCH_ELF_STATE {			\
	.flags = 0,				\
}

static inline int arch_parse_elf_property(u32 type, const void *data,
					  size_t datasz, bool compat,
					  struct arch_elf_state *arch)
{
	/* No known properties for AArch32 yet */
	if (IS_ENABLED(CONFIG_COMPAT) && compat)
		return 0;

	if (type == GNU_PROPERTY_AARCH64_FEATURE_1_AND) {
		const u32 *p = data;

		if (datasz != sizeof(*p))
			return -ENOEXEC;

		if (system_supports_bti() &&
		    (*p & GNU_PROPERTY_AARCH64_FEATURE_1_BTI))
			arch->flags |= ARM64_ELF_BTI;
	}

	return 0;
}

static inline int arch_elf_pt_proc(void *ehdr, void *phdr,
				   struct file *f, bool is_interp,
				   struct arch_elf_state *state)
{
	return 0;
}

static inline int arch_check_elf(void *ehdr, bool has_interp,
				 void *interp_ehdr,
				 struct arch_elf_state *state)
{
	return 0;
}

#endif /* !__ASSEMBLY__ */

#endif
