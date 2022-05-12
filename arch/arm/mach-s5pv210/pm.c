// SPDX-License-Identifier: GPL-2.0
//
// Copyright (c) 2010-2014 Samsung Electronics Co., Ltd.
//		http://www.samsung.com
//
// S5PV210 - Power Management support
//
// Based on arch/arm/mach-s3c2410/pm.c
// Copyright (c) 2006 Simtec Electronics
//	Ben Dooks <ben@simtec.co.uk>

#include <linux/init.h>
#include <linux/suspend.h>
#include <linux/syscore_ops.h>
#include <linux/io.h>
#include <linux/soc/samsung/s3c-pm.h>

#include <asm/cacheflush.h>
#include <asm/suspend.h>

#include "common.h"
#include "regs-clock.h"

/* helper functions to save and restore register state */
struct sleep_save {
	void __iomem	*reg;
	unsigned long	val;
};

#define SAVE_ITEM(x) \
	{ .reg = (x) }

/**
 * s3c_pm_do_save() - save a set of registers for restoration on resume.
 * @ptr: Pointer to an array of registers.
 * @count: Size of the ptr array.
 *
 * Run through the list of registers given, saving their contents in the
 * array for later restoration when we wakeup.
 */
static void s3c_pm_do_save(struct sleep_save *ptr, int count)
{
	for (; count > 0; count--, ptr++) {
		ptr->val = readl_relaxed(ptr->reg);
		S3C_PMDBG("saved %p value %08lx\n", ptr->reg, ptr->val);
	}
}

/**
 * s3c_pm_do_restore() - restore register values from the save list.
 * @ptr: Pointer to an array of registers.
 * @count: Size of the ptr array.
 *
 * Restore the register values saved from s3c_pm_do_save().
 *
 * WARNING: Do not put any debug in here that may effect memory or use
 * peripherals, as things may be changing!
*/

static void s3c_pm_do_restore_core(const struct sleep_save *ptr, int count)
{
	for (; count > 0; count--, ptr++)
		writel_relaxed(ptr->val, ptr->reg);
}

static struct sleep_save s5pv210_core_save[] = {
	/* Clock ETC */
	SAVE_ITEM(S5P_MDNIE_SEL),
};

/*
 * VIC wake-up support (TODO)
 */
static u32 s5pv210_irqwake_intmask = 0xffffffff;

static u32 s5pv210_read_eint_wakeup_mask(void)
{
	return __raw_readl(S5P_EINT_WAKEUP_MASK);
}

/*
 * Suspend helpers.
 */
static int s5pv210_cpu_suspend(unsigned long arg)
{
	unsigned long tmp;

	/* issue the standby signal into the pm unit. Note, we
	 * issue a write-buffer drain just in case */

	tmp = 0;

	asm("b 1f\n\t"
	    ".align 5\n\t"
	    "1:\n\t"
	    "mcr p15, 0, %0, c7, c10, 5\n\t"
	    "mcr p15, 0, %0, c7, c10, 4\n\t"
	    "wfi" : : "r" (tmp));

	pr_info("Failed to suspend the system\n");
	return 1; /* Aborting suspend */
}

static void s5pv210_pm_prepare(void)
{
	unsigned int tmp;

	/*
	 * Set wake-up mask registers
	 * S5P_EINT_WAKEUP_MASK is set by pinctrl driver in late suspend.
	 */
	__raw_writel(s5pv210_irqwake_intmask, S5P_WAKEUP_MASK);

	/* ensure at least INFORM0 has the resume address */
	__raw_writel(__pa_symbol(s5pv210_cpu_resume), S5P_INFORM0);

	tmp = __raw_readl(S5P_SLEEP_CFG);
	tmp &= ~(S5P_SLEEP_CFG_OSC_EN | S5P_SLEEP_CFG_USBOSC_EN);
	__raw_writel(tmp, S5P_SLEEP_CFG);

	/* WFI for SLEEP mode configuration by SYSCON */
	tmp = __raw_readl(S5P_PWR_CFG);
	tmp &= S5P_CFG_WFI_CLEAN;
	tmp |= S5P_CFG_WFI_SLEEP;
	__raw_writel(tmp, S5P_PWR_CFG);

	/* SYSCON interrupt handling disable */
	tmp = __raw_readl(S5P_OTHERS);
	tmp |= S5P_OTHER_SYSC_INTOFF;
	__raw_writel(tmp, S5P_OTHERS);

	s3c_pm_do_save(s5pv210_core_save, ARRAY_SIZE(s5pv210_core_save));
}

/*
 * Suspend operations.
 */
static int s5pv210_suspend_enter(suspend_state_t state)
{
	u32 eint_wakeup_mask = s5pv210_read_eint_wakeup_mask();
	int ret;

	S3C_PMDBG("%s: suspending the system...\n", __func__);

	S3C_PMDBG("%s: wakeup masks: %08x,%08x\n", __func__,
			s5pv210_irqwake_intmask, eint_wakeup_mask);

	if (s5pv210_irqwake_intmask == -1U
	    && eint_wakeup_mask == -1U) {
		pr_err("%s: No wake-up sources!\n", __func__);
		pr_err("%s: Aborting sleep\n", __func__);
		return -EINVAL;
	}

	s3c_pm_save_uarts(false);
	s5pv210_pm_prepare();
	flush_cache_all();
	s3c_pm_check_store();

	ret = cpu_suspend(0, s5pv210_cpu_suspend);
	if (ret)
		return ret;

	s3c_pm_restore_uarts(false);

	S3C_PMDBG("%s: wakeup stat: %08x\n", __func__,
			__raw_readl(S5P_WAKEUP_STAT));

	s3c_pm_check_restore();

	S3C_PMDBG("%s: resuming the system...\n", __func__);

	return 0;
}

static int s5pv210_suspend_prepare(void)
{
	s3c_pm_check_prepare();

	return 0;
}

static void s5pv210_suspend_finish(void)
{
	s3c_pm_check_cleanup();
}

static const struct platform_suspend_ops s5pv210_suspend_ops = {
	.enter		= s5pv210_suspend_enter,
	.prepare	= s5pv210_suspend_prepare,
	.finish		= s5pv210_suspend_finish,
	.valid		= suspend_valid_only_mem,
};

/*
 * Syscore operations used to delay restore of certain registers.
 */
static void s5pv210_pm_resume(void)
{
	s3c_pm_do_restore_core(s5pv210_core_save, ARRAY_SIZE(s5pv210_core_save));
}

static struct syscore_ops s5pv210_pm_syscore_ops = {
	.resume		= s5pv210_pm_resume,
};

/*
 * Initialization entry point.
 */
void __init s5pv210_pm_init(void)
{
	register_syscore_ops(&s5pv210_pm_syscore_ops);
	suspend_set_ops(&s5pv210_suspend_ops);
}
