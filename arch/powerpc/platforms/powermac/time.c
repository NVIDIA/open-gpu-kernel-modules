// SPDX-License-Identifier: GPL-2.0
/*
 * Support for periodic interrupts (100 per second) and for getting
 * the current time from the RTC on Power Macintoshes.
 *
 * We use the decrementer register for our periodic interrupts.
 *
 * Paul Mackerras	August 1996.
 * Copyright (C) 1996 Paul Mackerras.
 * Copyright (C) 2003-2005 Benjamin Herrenschmidt.
 *
 */
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/param.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/time.h>
#include <linux/adb.h>
#include <linux/cuda.h>
#include <linux/pmu.h>
#include <linux/interrupt.h>
#include <linux/hardirq.h>
#include <linux/rtc.h>

#include <asm/sections.h>
#include <asm/prom.h>
#include <asm/io.h>
#include <asm/machdep.h>
#include <asm/time.h>
#include <asm/nvram.h>
#include <asm/smu.h>

#include "pmac.h"

#undef DEBUG

#ifdef DEBUG
#define DBG(x...) printk(x)
#else
#define DBG(x...)
#endif

/*
 * Calibrate the decrementer frequency with the VIA timer 1.
 */
#define VIA_TIMER_FREQ_6	4700000	/* time 1 frequency * 6 */

/* VIA registers */
#define RS		0x200		/* skip between registers */
#define T1CL		(4*RS)		/* Timer 1 ctr/latch (low 8 bits) */
#define T1CH		(5*RS)		/* Timer 1 counter (high 8 bits) */
#define T1LL		(6*RS)		/* Timer 1 latch (low 8 bits) */
#define T1LH		(7*RS)		/* Timer 1 latch (high 8 bits) */
#define ACR		(11*RS)		/* Auxiliary control register */
#define IFR		(13*RS)		/* Interrupt flag register */

/* Bits in ACR */
#define T1MODE		0xc0		/* Timer 1 mode */
#define T1MODE_CONT	0x40		/*  continuous interrupts */

/* Bits in IFR and IER */
#define T1_INT		0x40		/* Timer 1 interrupt */

long __init pmac_time_init(void)
{
	s32 delta = 0;
#if defined(CONFIG_NVRAM) && defined(CONFIG_PPC32)
	int dst;
	
	delta = ((s32)pmac_xpram_read(PMAC_XPRAM_MACHINE_LOC + 0x9)) << 16;
	delta |= ((s32)pmac_xpram_read(PMAC_XPRAM_MACHINE_LOC + 0xa)) << 8;
	delta |= pmac_xpram_read(PMAC_XPRAM_MACHINE_LOC + 0xb);
	if (delta & 0x00800000UL)
		delta |= 0xFF000000UL;
	dst = ((pmac_xpram_read(PMAC_XPRAM_MACHINE_LOC + 0x8) & 0x80) != 0);
	printk("GMT Delta read from XPRAM: %d minutes, DST: %s\n", delta/60,
		dst ? "on" : "off");
#endif
	return delta;
}

#ifdef CONFIG_PMAC_SMU
static time64_t smu_get_time(void)
{
	struct rtc_time tm;

	if (smu_get_rtc_time(&tm, 1))
		return 0;
	return rtc_tm_to_time64(&tm);
}
#endif

/* Can't be __init, it's called when suspending and resuming */
time64_t pmac_get_boot_time(void)
{
	/* Get the time from the RTC, used only at boot time */
	switch (sys_ctrler) {
#ifdef CONFIG_ADB_CUDA
	case SYS_CTRLER_CUDA:
		return cuda_get_time();
#endif
#ifdef CONFIG_ADB_PMU
	case SYS_CTRLER_PMU:
		return pmu_get_time();
#endif
#ifdef CONFIG_PMAC_SMU
	case SYS_CTRLER_SMU:
		return smu_get_time();
#endif
	default:
		return 0;
	}
}

void pmac_get_rtc_time(struct rtc_time *tm)
{
	/* Get the time from the RTC, used only at boot time */
	switch (sys_ctrler) {
#ifdef CONFIG_ADB_CUDA
	case SYS_CTRLER_CUDA:
		rtc_time64_to_tm(cuda_get_time(), tm);
		break;
#endif
#ifdef CONFIG_ADB_PMU
	case SYS_CTRLER_PMU:
		rtc_time64_to_tm(pmu_get_time(), tm);
		break;
#endif
#ifdef CONFIG_PMAC_SMU
	case SYS_CTRLER_SMU:
		smu_get_rtc_time(tm, 1);
		break;
#endif
	default:
		;
	}
}

int pmac_set_rtc_time(struct rtc_time *tm)
{
	switch (sys_ctrler) {
#ifdef CONFIG_ADB_CUDA
	case SYS_CTRLER_CUDA:
		return cuda_set_rtc_time(tm);
#endif
#ifdef CONFIG_ADB_PMU
	case SYS_CTRLER_PMU:
		return pmu_set_rtc_time(tm);
#endif
#ifdef CONFIG_PMAC_SMU
	case SYS_CTRLER_SMU:
		return smu_set_rtc_time(tm, 1);
#endif
	default:
		return -ENODEV;
	}
}

#ifdef CONFIG_PPC32
/*
 * Calibrate the decrementer register using VIA timer 1.
 * This is used both on powermacs and CHRP machines.
 */
static int __init via_calibrate_decr(void)
{
	struct device_node *vias;
	volatile unsigned char __iomem *via;
	int count = VIA_TIMER_FREQ_6 / 100;
	unsigned int dstart, dend;
	struct resource rsrc;

	vias = of_find_node_by_name(NULL, "via-cuda");
	if (vias == NULL)
		vias = of_find_node_by_name(NULL, "via-pmu");
	if (vias == NULL)
		vias = of_find_node_by_name(NULL, "via");
	if (vias == NULL || of_address_to_resource(vias, 0, &rsrc)) {
	        of_node_put(vias);
		return 0;
	}
	of_node_put(vias);
	via = ioremap(rsrc.start, resource_size(&rsrc));
	if (via == NULL) {
		printk(KERN_ERR "Failed to map VIA for timer calibration !\n");
		return 0;
	}

	/* set timer 1 for continuous interrupts */
	out_8(&via[ACR], (via[ACR] & ~T1MODE) | T1MODE_CONT);
	/* set the counter to a small value */
	out_8(&via[T1CH], 2);
	/* set the latch to `count' */
	out_8(&via[T1LL], count);
	out_8(&via[T1LH], count >> 8);
	/* wait until it hits 0 */
	while ((in_8(&via[IFR]) & T1_INT) == 0)
		;
	dstart = get_dec();
	/* clear the interrupt & wait until it hits 0 again */
	in_8(&via[T1CL]);
	while ((in_8(&via[IFR]) & T1_INT) == 0)
		;
	dend = get_dec();

	ppc_tb_freq = (dstart - dend) * 100 / 6;

	iounmap(via);

	return 1;
}
#endif

/*
 * Query the OF and get the decr frequency.
 */
void __init pmac_calibrate_decr(void)
{
	generic_calibrate_decr();

#ifdef CONFIG_PPC32
	/* We assume MacRISC2 machines have correct device-tree
	 * calibration. That's better since the VIA itself seems
	 * to be slightly off. --BenH
	 */
	if (!of_machine_is_compatible("MacRISC2") &&
	    !of_machine_is_compatible("MacRISC3") &&
	    !of_machine_is_compatible("MacRISC4"))
		if (via_calibrate_decr())
			return;

	/* Special case: QuickSilver G4s seem to have a badly calibrated
	 * timebase-frequency in OF, VIA is much better on these. We should
	 * probably implement calibration based on the KL timer on these
	 * machines anyway... -BenH
	 */
	if (of_machine_is_compatible("PowerMac3,5"))
		if (via_calibrate_decr())
			return;
#endif
}
