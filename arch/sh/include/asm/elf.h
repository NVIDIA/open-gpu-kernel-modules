/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_SH_ELF_H
#define __ASM_SH_ELF_H

#include <linux/utsname.h>
#include <asm/auxvec.h>
#include <asm/ptrace.h>
#include <asm/user.h>

/* ELF header e_flags defines */
#define EF_SH_PIC		0x100	/* -fpic */
#define EF_SH_FDPIC		0x8000	/* -mfdpic */

/* SH (particularly SHcompact) relocation types  */
#define	R_SH_NONE		0
#define	R_SH_DIR32		1
#define	R_SH_REL32		2
#define	R_SH_DIR8WPN		3
#define	R_SH_IND12W		4
#define	R_SH_DIR8WPL		5
#define	R_SH_DIR8WPZ		6
#define	R_SH_DIR8BP		7
#define	R_SH_DIR8W		8
#define	R_SH_DIR8L		9
#define	R_SH_SWITCH16		25
#define	R_SH_SWITCH32		26
#define	R_SH_USES		27
#define	R_SH_COUNT		28
#define	R_SH_ALIGN		29
#define	R_SH_CODE		30
#define	R_SH_DATA		31
#define	R_SH_LABEL		32
#define	R_SH_SWITCH8		33
#define	R_SH_GNU_VTINHERIT	34
#define	R_SH_GNU_VTENTRY	35
#define	R_SH_TLS_GD_32		144
#define	R_SH_TLS_LD_32		145
#define	R_SH_TLS_LDO_32		146
#define	R_SH_TLS_IE_32		147
#define	R_SH_TLS_LE_32		148
#define	R_SH_TLS_DTPMOD32	149
#define	R_SH_TLS_DTPOFF32	150
#define	R_SH_TLS_TPOFF32	151
#define	R_SH_GOT32		160
#define	R_SH_PLT32		161
#define	R_SH_COPY		162
#define	R_SH_GLOB_DAT		163
#define	R_SH_JMP_SLOT		164
#define	R_SH_RELATIVE		165
#define	R_SH_GOTOFF		166
#define	R_SH_GOTPC		167

/* FDPIC relocs */
#define R_SH_GOT20		201
#define R_SH_GOTOFF20		202
#define R_SH_GOTFUNCDESC	203
#define R_SH_GOTFUNCDESC20	204
#define R_SH_GOTOFFFUNCDESC	205
#define R_SH_GOTOFFFUNCDESC20	206
#define R_SH_FUNCDESC		207
#define R_SH_FUNCDESC_VALUE	208

/* SHmedia relocs */
#define R_SH_IMM_LOW16		246
#define R_SH_IMM_LOW16_PCREL	247
#define R_SH_IMM_MEDLOW16	248
#define R_SH_IMM_MEDLOW16_PCREL	249
/* Keep this the last entry.  */
#define	R_SH_NUM		256

/*
 * ELF register definitions..
 */

typedef unsigned long elf_greg_t;

#define ELF_NGREG (sizeof (struct pt_regs) / sizeof(elf_greg_t))
typedef elf_greg_t elf_gregset_t[ELF_NGREG];

typedef struct user_fpu_struct elf_fpregset_t;

/*
 * These are used to set parameters in the core dumps.
 */
#define ELF_CLASS	ELFCLASS32
#ifdef __LITTLE_ENDIAN__
#define ELF_DATA	ELFDATA2LSB
#else
#define ELF_DATA	ELFDATA2MSB
#endif
#define ELF_ARCH	EM_SH

/*
 * This is used to ensure we don't load something for the wrong architecture.
 */
#define elf_check_arch(x)		((x)->e_machine == EM_SH)
#define elf_check_fdpic(x)		((x)->e_flags & EF_SH_FDPIC)
#define elf_check_const_displacement(x)	((x)->e_flags & EF_SH_PIC)

/*
 * Enable dump using regset.
 * This covers all of general/DSP/FPU regs.
 */
#define CORE_DUMP_USE_REGSET

#define ELF_FDPIC_CORE_EFLAGS	EF_SH_FDPIC
#define ELF_EXEC_PAGESIZE	PAGE_SIZE

/* This is the location that an ET_DYN program is loaded if exec'ed.  Typical
   use of this is to invoke "./ld.so someprog" to test out a new version of
   the loader.  We need to make sure that it is out of the way of the program
   that it will "exec", and that there is sufficient room for the brk.  */

#define ELF_ET_DYN_BASE         (2 * TASK_SIZE / 3)

#define ELF_CORE_COPY_REGS(_dest,_regs)				\
	memcpy((char *) &_dest, (char *) _regs,			\
	       sizeof(struct pt_regs));

/* This yields a mask that user programs can use to figure out what
   instruction set this CPU supports.  This could be done in user space,
   but it's not easy, and we've already done it here.  */

#define ELF_HWCAP	(boot_cpu_data.flags)

/* This yields a string that ld.so will use to load implementation
   specific libraries for optimization.  This is more specific in
   intent than poking at uname or /proc/cpuinfo.

   For the moment, we have only optimizations for the Intel generations,
   but that could change... */

#define ELF_PLATFORM	(utsname()->machine)

#define ELF_PLAT_INIT(_r, load_addr) \
  do { _r->regs[0]=0; _r->regs[1]=0; _r->regs[2]=0; _r->regs[3]=0; \
       _r->regs[4]=0; _r->regs[5]=0; _r->regs[6]=0; _r->regs[7]=0; \
       _r->regs[8]=0; _r->regs[9]=0; _r->regs[10]=0; _r->regs[11]=0; \
       _r->regs[12]=0; _r->regs[13]=0; _r->regs[14]=0; \
       _r->sr = SR_FD; } while (0)

#define ELF_FDPIC_PLAT_INIT(_r, _exec_map_addr, _interp_map_addr,	\
			    _dynamic_addr)				\
do {									\
	_r->regs[0]	= 0;						\
	_r->regs[1]	= 0;						\
	_r->regs[2]	= 0;						\
	_r->regs[3]	= 0;						\
	_r->regs[4]	= 0;						\
	_r->regs[5]	= 0;						\
	_r->regs[6]	= 0;						\
	_r->regs[7]	= 0;						\
	_r->regs[8]	= _exec_map_addr;				\
	_r->regs[9]	= _interp_map_addr;				\
	_r->regs[10]	= _dynamic_addr;				\
	_r->regs[11]	= 0;						\
	_r->regs[12]	= 0;						\
	_r->regs[13]	= 0;						\
	_r->regs[14]	= 0;						\
	_r->sr		= SR_FD;					\
} while (0)

#define SET_PERSONALITY(ex) \
	set_personality(PER_LINUX_32BIT | (current->personality & (~PER_MASK)))

#ifdef CONFIG_VSYSCALL
/* vDSO has arch_setup_additional_pages */
#define ARCH_HAS_SETUP_ADDITIONAL_PAGES
struct linux_binprm;
extern int arch_setup_additional_pages(struct linux_binprm *bprm,
				       int uses_interp);

extern unsigned int vdso_enabled;
extern void __kernel_vsyscall;

#define VDSO_BASE		((unsigned long)current->mm->context.vdso)
#define VDSO_SYM(x)		(VDSO_BASE + (unsigned long)(x))

#define VSYSCALL_AUX_ENT					\
	if (vdso_enabled)					\
		NEW_AUX_ENT(AT_SYSINFO_EHDR, VDSO_BASE);	\
	else							\
		NEW_AUX_ENT(AT_IGNORE, 0)
#else
#define VSYSCALL_AUX_ENT	NEW_AUX_ENT(AT_IGNORE, 0)
#endif /* CONFIG_VSYSCALL */

#ifdef CONFIG_SH_FPU
#define FPU_AUX_ENT	NEW_AUX_ENT(AT_FPUCW, FPSCR_INIT)
#else
#define FPU_AUX_ENT	NEW_AUX_ENT(AT_IGNORE, 0)
#endif

extern int l1i_cache_shape, l1d_cache_shape, l2_cache_shape;

/* update AT_VECTOR_SIZE_ARCH if the number of NEW_AUX_ENT entries changes */
#define ARCH_DLINFO						\
do {								\
	/* Optional FPU initialization */			\
	FPU_AUX_ENT;						\
								\
	/* Optional vsyscall entry */				\
	VSYSCALL_AUX_ENT;					\
								\
	/* Cache desc */					\
	NEW_AUX_ENT(AT_L1I_CACHESHAPE, l1i_cache_shape);	\
	NEW_AUX_ENT(AT_L1D_CACHESHAPE, l1d_cache_shape);	\
	NEW_AUX_ENT(AT_L2_CACHESHAPE, l2_cache_shape);		\
} while (0)

#endif /* __ASM_SH_ELF_H */
