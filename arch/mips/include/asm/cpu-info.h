/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1994 Waldorf GMBH
 * Copyright (C) 1995, 1996, 1997, 1998, 1999, 2001, 2002, 2003 Ralf Baechle
 * Copyright (C) 1996 Paul M. Antoine
 * Copyright (C) 1999, 2000 Silicon Graphics, Inc.
 * Copyright (C) 2004  Maciej W. Rozycki
 */
#ifndef __ASM_CPU_INFO_H
#define __ASM_CPU_INFO_H

#include <linux/cache.h>
#include <linux/types.h>

#include <asm/mipsregs.h>

/*
 * Descriptor for a cache
 */
struct cache_desc {
	unsigned int waysize;	/* Bytes per way */
	unsigned short sets;	/* Number of lines per set */
	unsigned char ways;	/* Number of ways */
	unsigned char linesz;	/* Size of line in bytes */
	unsigned char waybit;	/* Bits to select in a cache set */
	unsigned char flags;	/* Flags describing cache properties */
};

struct guest_info {
	unsigned long		ases;
	unsigned long		ases_dyn;
	unsigned long long	options;
	unsigned long long	options_dyn;
	int			tlbsize;
	u8			conf;
	u8			kscratch_mask;
};

/*
 * Flag definitions
 */
#define MIPS_CACHE_NOT_PRESENT	0x00000001
#define MIPS_CACHE_VTAG		0x00000002	/* Virtually tagged cache */
#define MIPS_CACHE_ALIASES	0x00000004	/* Cache could have aliases */
#define MIPS_CACHE_IC_F_DC	0x00000008	/* Ic can refill from D-cache */
#define MIPS_IC_SNOOPS_REMOTE	0x00000010	/* Ic snoops remote stores */
#define MIPS_CACHE_PINDEX	0x00000020	/* Physically indexed cache */

struct cpuinfo_mips {
	u64			asid_cache;
#ifdef CONFIG_MIPS_ASID_BITS_VARIABLE
	unsigned long		asid_mask;
#endif

	/*
	 * Capability and feature descriptor structure for MIPS CPU
	 */
	unsigned long		ases;
	unsigned long long	options;
	unsigned int		udelay_val;
	unsigned int		processor_id;
	unsigned int		fpu_id;
	unsigned int		fpu_csr31;
	unsigned int		fpu_msk31;
	unsigned int		msa_id;
	unsigned int		cputype;
	int			isa_level;
	int			tlbsize;
	int			tlbsizevtlb;
	int			tlbsizeftlbsets;
	int			tlbsizeftlbways;
	struct cache_desc	icache; /* Primary I-cache */
	struct cache_desc	dcache; /* Primary D or combined I/D cache */
	struct cache_desc	vcache; /* Victim cache, between pcache and scache */
	struct cache_desc	scache; /* Secondary cache */
	struct cache_desc	tcache; /* Tertiary/split secondary cache */
	int			srsets; /* Shadow register sets */
	int			package;/* physical package number */
	unsigned int		globalnumber;
#ifdef CONFIG_64BIT
	int			vmbits; /* Virtual memory size in bits */
#endif
	void			*data;	/* Additional data */
	unsigned int		watch_reg_count;   /* Number that exist */
	unsigned int		watch_reg_use_cnt; /* Usable by ptrace */
#define NUM_WATCH_REGS 4
	u16			watch_reg_masks[NUM_WATCH_REGS];
	unsigned int		kscratch_mask; /* Usable KScratch mask. */
	/*
	 * Cache Coherency attribute for write-combine memory writes.
	 * (shifted by _CACHE_SHIFT)
	 */
	unsigned int		writecombine;
	/*
	 * Simple counter to prevent enabling HTW in nested
	 * htw_start/htw_stop calls
	 */
	unsigned int		htw_seq;

	/* VZ & Guest features */
	struct guest_info	guest;
	unsigned int		gtoffset_mask;
	unsigned int		guestid_mask;
	unsigned int		guestid_cache;

#ifdef CONFIG_CPU_LOONGSON3_CPUCFG_EMULATION
	/* CPUCFG data for this CPU, synthesized at probe time.
	 *
	 * CPUCFG select 0 is PRId, 4 and above are unimplemented for now.
	 * So the only stored values are for CPUCFG selects 1-3 inclusive.
	 */
	u32 loongson3_cpucfg_data[3];
#endif
} __attribute__((aligned(SMP_CACHE_BYTES)));

extern struct cpuinfo_mips cpu_data[];
#define current_cpu_data cpu_data[smp_processor_id()]
#define raw_current_cpu_data cpu_data[raw_smp_processor_id()]
#define boot_cpu_data cpu_data[0]

extern void cpu_probe(void);
extern void cpu_report(void);

extern const char *__cpu_name[];
#define cpu_name_string()	__cpu_name[raw_smp_processor_id()]

struct seq_file;
struct notifier_block;

extern int register_proc_cpuinfo_notifier(struct notifier_block *nb);
extern int proc_cpuinfo_notifier_call_chain(unsigned long val, void *v);

#define proc_cpuinfo_notifier(fn, pri)					\
({									\
	static struct notifier_block fn##_nb = {			\
		.notifier_call = fn,					\
		.priority = pri						\
	};								\
									\
	register_proc_cpuinfo_notifier(&fn##_nb);			\
})

struct proc_cpuinfo_notifier_args {
	struct seq_file *m;
	unsigned long n;
};

static inline unsigned int cpu_cluster(struct cpuinfo_mips *cpuinfo)
{
	/* Optimisation for systems where multiple clusters aren't used */
	if (!IS_ENABLED(CONFIG_CPU_MIPSR5) && !IS_ENABLED(CONFIG_CPU_MIPSR6))
		return 0;

	return (cpuinfo->globalnumber & MIPS_GLOBALNUMBER_CLUSTER) >>
		MIPS_GLOBALNUMBER_CLUSTER_SHF;
}

static inline unsigned int cpu_core(struct cpuinfo_mips *cpuinfo)
{
	return (cpuinfo->globalnumber & MIPS_GLOBALNUMBER_CORE) >>
		MIPS_GLOBALNUMBER_CORE_SHF;
}

static inline unsigned int cpu_vpe_id(struct cpuinfo_mips *cpuinfo)
{
	/* Optimisation for systems where VP(E)s aren't used */
	if (!IS_ENABLED(CONFIG_MIPS_MT_SMP) && !IS_ENABLED(CONFIG_CPU_MIPSR6))
		return 0;

	return (cpuinfo->globalnumber & MIPS_GLOBALNUMBER_VP) >>
		MIPS_GLOBALNUMBER_VP_SHF;
}

extern void cpu_set_cluster(struct cpuinfo_mips *cpuinfo, unsigned int cluster);
extern void cpu_set_core(struct cpuinfo_mips *cpuinfo, unsigned int core);
extern void cpu_set_vpe_id(struct cpuinfo_mips *cpuinfo, unsigned int vpe);

static inline bool cpus_are_siblings(int cpua, int cpub)
{
	struct cpuinfo_mips *infoa = &cpu_data[cpua];
	struct cpuinfo_mips *infob = &cpu_data[cpub];
	unsigned int gnuma, gnumb;

	if (infoa->package != infob->package)
		return false;

	gnuma = infoa->globalnumber & ~MIPS_GLOBALNUMBER_VP;
	gnumb = infob->globalnumber & ~MIPS_GLOBALNUMBER_VP;
	if (gnuma != gnumb)
		return false;

	return true;
}

static inline unsigned long cpu_asid_inc(void)
{
	return 1 << CONFIG_MIPS_ASID_SHIFT;
}

static inline unsigned long cpu_asid_mask(struct cpuinfo_mips *cpuinfo)
{
#ifdef CONFIG_MIPS_ASID_BITS_VARIABLE
	return cpuinfo->asid_mask;
#endif
	return ((1 << CONFIG_MIPS_ASID_BITS) - 1) << CONFIG_MIPS_ASID_SHIFT;
}

static inline void set_cpu_asid_mask(struct cpuinfo_mips *cpuinfo,
				     unsigned long asid_mask)
{
#ifdef CONFIG_MIPS_ASID_BITS_VARIABLE
	cpuinfo->asid_mask = asid_mask;
#endif
}

#endif /* __ASM_CPU_INFO_H */
