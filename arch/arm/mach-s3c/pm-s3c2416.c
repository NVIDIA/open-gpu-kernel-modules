// SPDX-License-Identifier: GPL-2.0
//
// Copyright (c) 2010 Samsung Electronics Co., Ltd.
//		http://www.samsung.com
//
// S3C2416 - PM support (Based on Ben Dooks' S3C2412 PM support)

#include <linux/device.h>
#include <linux/syscore_ops.h>
#include <linux/io.h>

#include <asm/cacheflush.h>

#include "regs-s3c2443-clock.h"

#include "cpu.h"
#include "pm.h"

#include "s3c2412-power.h"

#ifdef CONFIG_PM_SLEEP
extern void s3c2412_sleep_enter(void);

static int s3c2416_cpu_suspend(unsigned long arg)
{
	/* enable wakeup sources regardless of battery state */
	__raw_writel(S3C2443_PWRCFG_SLEEP, S3C2443_PWRCFG);

	/* set the mode as sleep, 2BED represents "Go to BED" */
	__raw_writel(0x2BED, S3C2443_PWRMODE);

	s3c2412_sleep_enter();

	pr_info("Failed to suspend the system\n");
	return 1; /* Aborting suspend */
}

static void s3c2416_pm_prepare(void)
{
	/*
	 * write the magic value u-boot uses to check for resume into
	 * the INFORM0 register, and ensure INFORM1 is set to the
	 * correct address to resume from.
	 */
	__raw_writel(0x2BED, S3C2412_INFORM0);
	__raw_writel(__pa_symbol(s3c_cpu_resume), S3C2412_INFORM1);
}

static int s3c2416_pm_add(struct device *dev, struct subsys_interface *sif)
{
	pm_cpu_prep = s3c2416_pm_prepare;
	pm_cpu_sleep = s3c2416_cpu_suspend;

	return 0;
}

static struct subsys_interface s3c2416_pm_interface = {
	.name		= "s3c2416_pm",
	.subsys		= &s3c2416_subsys,
	.add_dev	= s3c2416_pm_add,
};

static __init int s3c2416_pm_init(void)
{
	return subsys_interface_register(&s3c2416_pm_interface);
}

arch_initcall(s3c2416_pm_init);
#endif

static void s3c2416_pm_resume(void)
{
	/* unset the return-from-sleep amd inform flags */
	__raw_writel(0x0, S3C2443_PWRMODE);
	__raw_writel(0x0, S3C2412_INFORM0);
	__raw_writel(0x0, S3C2412_INFORM1);
}

struct syscore_ops s3c2416_pm_syscore_ops = {
	.resume		= s3c2416_pm_resume,
};
