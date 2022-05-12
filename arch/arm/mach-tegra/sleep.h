/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2010-2013, NVIDIA Corporation. All rights reserved.
 */

#ifndef __MACH_TEGRA_SLEEP_H
#define __MACH_TEGRA_SLEEP_H

#include "iomap.h"
#include "irammap.h"

#define TEGRA_ARM_PERIF_VIRT (TEGRA_ARM_PERIF_BASE - IO_CPU_PHYS \
					+ IO_CPU_VIRT)
#define TEGRA_FLOW_CTRL_VIRT (TEGRA_FLOW_CTRL_BASE - IO_PPSB_PHYS \
					+ IO_PPSB_VIRT)
#define TEGRA_CLK_RESET_VIRT (TEGRA_CLK_RESET_BASE - IO_PPSB_PHYS \
					+ IO_PPSB_VIRT)
#define TEGRA_APB_MISC_VIRT (TEGRA_APB_MISC_BASE - IO_APB_PHYS \
					+ IO_APB_VIRT)
#define TEGRA_PMC_VIRT	(TEGRA_PMC_BASE - IO_APB_PHYS + IO_APB_VIRT)

#define TEGRA_IRAM_RESET_BASE_VIRT (IO_IRAM_VIRT + \
				TEGRA_IRAM_RESET_HANDLER_OFFSET)

/* PMC_SCRATCH37-39 and 41 are used for tegra_pen_lock and idle */
#define PMC_SCRATCH37	0x130
#define PMC_SCRATCH38	0x134
#define PMC_SCRATCH39	0x138
#define PMC_SCRATCH41	0x140

#ifdef CONFIG_ARCH_TEGRA_2x_SOC
#define CPU_RESETTABLE		2
#define CPU_RESETTABLE_SOON	1
#define CPU_NOT_RESETTABLE	0
#endif

/* flag of tegra_disable_clean_inv_dcache to do LoUIS or all */
#define TEGRA_FLUSH_CACHE_LOUIS	0
#define TEGRA_FLUSH_CACHE_ALL	1

#ifdef __ASSEMBLY__
/* waits until the microsecond counter (base) is > rn */
.macro wait_until, rn, base, tmp
	add	\rn, \rn, #1
1001:	ldr	\tmp, [\base]
	cmp	\tmp, \rn
	bmi	1001b
.endm

/* returns the offset of the flow controller halt register for a cpu */
.macro cpu_to_halt_reg rd, rcpu
	cmp	\rcpu, #0
	subne	\rd, \rcpu, #1
	movne	\rd, \rd, lsl #3
	addne	\rd, \rd, #0x14
	moveq	\rd, #0
.endm

/* returns the offset of the flow controller csr register for a cpu */
.macro cpu_to_csr_reg rd, rcpu
	cmp	\rcpu, #0
	subne	\rd, \rcpu, #1
	movne	\rd, \rd, lsl #3
	addne	\rd, \rd, #0x18
	moveq	\rd, #8
.endm

/* returns the ID of the current processor */
.macro cpu_id, rd
	mrc	p15, 0, \rd, c0, c0, 5
	and	\rd, \rd, #0xF
.endm

/* loads a 32-bit value into a register without a data access */
.macro mov32, reg, val
	movw	\reg, #:lower16:\val
	movt	\reg, #:upper16:\val
.endm

/* Marco to check CPU part num */
.macro check_cpu_part_num part_num, tmp1, tmp2
	mrc	p15, 0, \tmp1, c0, c0, 0
	ubfx	\tmp1, \tmp1, #4, #12
	mov32	\tmp2, \part_num
	cmp	\tmp1, \tmp2
.endm

/* Macro to exit SMP coherency. */
.macro exit_smp, tmp1, tmp2
	mrc	p15, 0, \tmp1, c1, c0, 1	@ ACTLR
	bic	\tmp1, \tmp1, #(1<<6) | (1<<0)	@ clear ACTLR.SMP | ACTLR.FW
	mcr	p15, 0, \tmp1, c1, c0, 1	@ ACTLR
	isb
#ifdef CONFIG_HAVE_ARM_SCU
	check_cpu_part_num 0xc09, \tmp1, \tmp2
	mrceq	p15, 0, \tmp1, c0, c0, 5
	andeq	\tmp1, \tmp1, #0xF
	moveq	\tmp1, \tmp1, lsl #2
	moveq	\tmp2, #0xf
	moveq	\tmp2, \tmp2, lsl \tmp1
	ldreq	\tmp1, =(TEGRA_ARM_PERIF_VIRT + 0xC)
	streq	\tmp2, [\tmp1]			@ invalidate SCU tags for CPU
	dsb
#endif
.endm

/* Macro to check Tegra revision */
#define APB_MISC_GP_HIDREV	0x804
.macro tegra_get_soc_id base, tmp1
	mov32	\tmp1, \base
	ldr	\tmp1, [\tmp1, #APB_MISC_GP_HIDREV]
	and	\tmp1, \tmp1, #0xff00
	mov	\tmp1, \tmp1, lsr #8
.endm

#else
void tegra_resume(void);
int tegra_sleep_cpu_finish(unsigned long);
void tegra_disable_clean_inv_dcache(u32 flag);

void tegra20_hotplug_shutdown(void);
void tegra30_hotplug_shutdown(void);

void tegra20_tear_down_cpu(void);
void tegra30_tear_down_cpu(void);

#endif
#endif
