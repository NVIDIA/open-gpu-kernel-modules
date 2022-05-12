// SPDX-License-Identifier: GPL-2.0
//
// Copyright (c) 2011 Wolfson Microelectronics, plc
// Copyright (c) 2011 Samsung Electronics Co., Ltd.
//		http://www.samsung.com

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cpuidle.h>
#include <linux/io.h>
#include <linux/export.h>
#include <linux/time.h>

#include <asm/cpuidle.h>

#include "cpu.h"
#include "map.h"

#include "regs-sys-s3c64xx.h"
#include "regs-syscon-power-s3c64xx.h"

static int s3c64xx_enter_idle(struct cpuidle_device *dev,
			      struct cpuidle_driver *drv,
			      int index)
{
	unsigned long tmp;

	/* Setup PWRCFG to enter idle mode */
	tmp = __raw_readl(S3C64XX_PWR_CFG);
	tmp &= ~S3C64XX_PWRCFG_CFG_WFI_MASK;
	tmp |= S3C64XX_PWRCFG_CFG_WFI_IDLE;
	__raw_writel(tmp, S3C64XX_PWR_CFG);

	cpu_do_idle();

	return index;
}

static struct cpuidle_driver s3c64xx_cpuidle_driver = {
	.name	= "s3c64xx_cpuidle",
	.owner  = THIS_MODULE,
	.states = {
		{
			.enter            = s3c64xx_enter_idle,
			.exit_latency     = 1,
			.target_residency = 1,
			.name             = "IDLE",
			.desc             = "System active, ARM gated",
		},
	},
	.state_count = 1,
};

static int __init s3c64xx_init_cpuidle(void)
{
	if (soc_is_s3c64xx())
		return cpuidle_register(&s3c64xx_cpuidle_driver, NULL);
	return 0;
}
device_initcall(s3c64xx_init_cpuidle);
