/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2003, 04, 07 Ralf Baechle (ralf@linux-mips.org)
 *
 * SNI RM200 C apparently was only shipped with R4600 V2.0 and R5000 processors.
 */
#ifndef __ASM_MACH_RM200_CPU_FEATURE_OVERRIDES_H
#define __ASM_MACH_RM200_CPU_FEATURE_OVERRIDES_H

#define cpu_has_tlb		1
#define cpu_has_4kex		1
#define cpu_has_4k_cache	1
#define cpu_has_32fpr		1
#define cpu_has_counter		1
#define cpu_has_watch		0
#define cpu_has_mips16		0
#define cpu_has_mips16e2	0
#define cpu_has_divec		0
#define cpu_has_cache_cdex_p	1
#define cpu_has_prefetch	0
#define cpu_has_mcheck		0
#define cpu_has_ejtag		0
#define cpu_has_llsc		1
#define cpu_has_vtag_icache	0
#define cpu_has_dc_aliases	(PAGE_SIZE < 0x4000)
#define cpu_has_ic_fills_f_dc	0
#define cpu_has_dsp		0
#define cpu_has_dsp2		0
#define cpu_has_nofpuex		0
#define cpu_has_64bits		1
#define cpu_has_mipsmt		0
#define cpu_has_userlocal	0

#define cpu_has_mips32r1	0
#define cpu_has_mips32r2	0
#define cpu_has_mips64r1	0
#define cpu_has_mips64r2	0

#endif /* __ASM_MACH_RM200_CPU_FEATURE_OVERRIDES_H */
