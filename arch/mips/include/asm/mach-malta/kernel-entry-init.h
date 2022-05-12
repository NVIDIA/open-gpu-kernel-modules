/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Chris Dearman (chris@mips.com)
 * Copyright (C) 2007 Mips Technologies, Inc.
 * Copyright (C) 2014 Imagination Technologies Ltd.
 */
#ifndef __ASM_MACH_MIPS_KERNEL_ENTRY_INIT_H
#define __ASM_MACH_MIPS_KERNEL_ENTRY_INIT_H

#include <asm/regdef.h>
#include <asm/mipsregs.h>

	/*
	 * Prepare segments for EVA boot:
	 *
	 * This is in case the processor boots in legacy configuration
	 * (SI_EVAReset is de-asserted and CONFIG5.K == 0)
	 *
	 * ========================= Mappings =============================
	 * Virtual memory           Physical memory           Mapping
	 * 0x00000000 - 0x7fffffff  0x80000000 - 0xfffffffff   MUSUK (kuseg)
	 *                          Flat 2GB physical memory
	 *
	 * 0x80000000 - 0x9fffffff  0x00000000 - 0x1ffffffff   MUSUK (kseg0)
	 * 0xa0000000 - 0xbf000000  0x00000000 - 0x1ffffffff   MUSUK (kseg1)
	 * 0xc0000000 - 0xdfffffff             -                 MK  (kseg2)
	 * 0xe0000000 - 0xffffffff             -                 MK  (kseg3)
	 *
	 *
	 * Lowmem is expanded to 2GB
	 *
	 * The following code uses the t0, t1, t2 and ra registers without
	 * previously preserving them.
	 *
	 */
	.macro	platform_eva_init

	.set	push
	.set	reorder
	/*
	 * Get Config.K0 value and use it to program
	 * the segmentation registers
	 */
	mfc0    t1, CP0_CONFIG
	andi	t1, 0x7 /* CCA */
	move	t2, t1
	ins	t2, t1, 16, 3
	/* SegCtl0 */
	li      t0, ((MIPS_SEGCFG_MK << MIPS_SEGCFG_AM_SHIFT) |		\
		(0 << MIPS_SEGCFG_PA_SHIFT) |				\
		(1 << MIPS_SEGCFG_EU_SHIFT)) |				\
		(((MIPS_SEGCFG_MK << MIPS_SEGCFG_AM_SHIFT) |		\
		(0 << MIPS_SEGCFG_PA_SHIFT) |				\
		(1 << MIPS_SEGCFG_EU_SHIFT)) << 16)
	or	t0, t2
	mtc0	t0, CP0_SEGCTL0

	/* SegCtl1 */
	li      t0, ((MIPS_SEGCFG_MUSUK << MIPS_SEGCFG_AM_SHIFT) |	\
		(0 << MIPS_SEGCFG_PA_SHIFT) |				\
		(2 << MIPS_SEGCFG_C_SHIFT) |				\
		(1 << MIPS_SEGCFG_EU_SHIFT)) |				\
		(((MIPS_SEGCFG_MUSUK << MIPS_SEGCFG_AM_SHIFT) |		\
		(0 << MIPS_SEGCFG_PA_SHIFT) |				\
		(1 << MIPS_SEGCFG_EU_SHIFT)) << 16)
	ins	t0, t1, 16, 3
	mtc0	t0, CP0_SEGCTL1

	/* SegCtl2 */
	li	t0, ((MIPS_SEGCFG_MUSUK << MIPS_SEGCFG_AM_SHIFT) |	\
		(6 << MIPS_SEGCFG_PA_SHIFT) |				\
		(1 << MIPS_SEGCFG_EU_SHIFT)) |				\
		(((MIPS_SEGCFG_MUSUK << MIPS_SEGCFG_AM_SHIFT) |		\
		(4 << MIPS_SEGCFG_PA_SHIFT) |				\
		(1 << MIPS_SEGCFG_EU_SHIFT)) << 16)
	or	t0, t2
	mtc0	t0, CP0_SEGCTL2

	jal	mips_ihb
	mfc0    t0, $16, 5
	li      t2, 0x40000000      /* K bit */
	or      t0, t0, t2
	mtc0    t0, $16, 5
	sync
	jal	mips_ihb

	.set	pop
	.endm

	.macro	kernel_entry_setup

#ifdef CONFIG_EVA
	sync
	ehb

	mfc0    t1, CP0_CONFIG
	bgez    t1, 9f
	mfc0	t0, CP0_CONFIG, 1
	bgez	t0, 9f
	mfc0	t0, CP0_CONFIG, 2
	bgez	t0, 9f
	mfc0	t0, CP0_CONFIG, 3
	sll     t0, t0, 6   /* SC bit */
	bgez    t0, 9f

	platform_eva_init
	b       0f
9:
	/* Assume we came from YAMON... */
	PTR_LA	v0, 0x9fc00534	/* YAMON print */
	lw	v0, (v0)
	move	a0, zero
	PTR_LA  a1, nonsc_processor
	jal	v0

	PTR_LA	v0, 0x9fc00520	/* YAMON exit */
	lw	v0, (v0)
	li	a0, 1
	jal	v0

1:	b	1b
	nop
	__INITDATA
nonsc_processor:
	.asciz  "EVA kernel requires a MIPS core with Segment Control implemented\n"
	__FINIT
#endif /* CONFIG_EVA */
0:
	.endm

/*
 * Do SMP slave processor setup necessary before we can safely execute C code.
 */
	.macro	smp_slave_setup
#ifdef CONFIG_EVA
	sync
	ehb
	platform_eva_init
#endif
	.endm

#endif /* __ASM_MACH_MIPS_KERNEL_ENTRY_INIT_H */
