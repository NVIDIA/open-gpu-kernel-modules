// SPDX-License-Identifier: GPL-2.0
//
// Copyright (c) 2004-2006 Simtec Electronics
//   Ben Dooks <ben@simtec.co.uk>
//
// Samsung S3C2440 and S3C2442 Mobile CPU support (not S3C2443)

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/serial_core.h>
#include <linux/serial_s3c.h>
#include <linux/platform_device.h>
#include <linux/reboot.h>
#include <linux/device.h>
#include <linux/syscore_ops.h>
#include <linux/clk.h>
#include <linux/io.h>

#include <asm/system_misc.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>

#include "map.h"
#include <asm/irq.h>

#include "regs-clock.h"
#include "regs-gpio.h"

#include "devs.h"
#include "cpu.h"
#include "pm.h"

#include "s3c24xx.h"
#include "nand-core-s3c24xx.h"
#include "regs-dsc-s3c24xx.h"

static struct map_desc s3c244x_iodesc[] __initdata __maybe_unused = {
	IODESC_ENT(CLKPWR),
	IODESC_ENT(TIMER),
	IODESC_ENT(WATCHDOG),
};

/* uart initialisation */

void __init s3c244x_init_uarts(struct s3c2410_uartcfg *cfg, int no)
{
	s3c24xx_init_uartdevs("s3c2440-uart", s3c2410_uart_resources, cfg, no);
}

void __init s3c244x_map_io(void)
{
	/* register our io-tables */

	iotable_init(s3c244x_iodesc, ARRAY_SIZE(s3c244x_iodesc));

	/* rename any peripherals used differing from the s3c2410 */

	s3c_device_sdi.name  = "s3c2440-sdi";
	s3c_device_i2c0.name  = "s3c2440-i2c";
	s3c_nand_setname("s3c2440-nand");
	s3c_device_ts.name = "s3c2440-ts";
	s3c_device_usbgadget.name = "s3c2440-usbgadget";
	s3c2410_device_dclk.name = "s3c2440-dclk";
}

/* Since the S3C2442 and S3C2440 share items, put both subsystems here */

struct bus_type s3c2440_subsys = {
	.name		= "s3c2440-core",
	.dev_name	= "s3c2440-core",
};

struct bus_type s3c2442_subsys = {
	.name		= "s3c2442-core",
	.dev_name	= "s3c2442-core",
};

/* need to register the subsystem before we actually register the device, and
 * we also need to ensure that it has been initialised before any of the
 * drivers even try to use it (even if not on an s3c2440 based system)
 * as a driver which may support both 2410 and 2440 may try and use it.
*/

static int __init s3c2440_core_init(void)
{
	return subsys_system_register(&s3c2440_subsys, NULL);
}

core_initcall(s3c2440_core_init);

static int __init s3c2442_core_init(void)
{
	return subsys_system_register(&s3c2442_subsys, NULL);
}

core_initcall(s3c2442_core_init);


#ifdef CONFIG_PM_SLEEP
static struct sleep_save s3c244x_sleep[] = {
	SAVE_ITEM(S3C2440_DSC0),
	SAVE_ITEM(S3C2440_DSC1),
	SAVE_ITEM(S3C2440_GPJDAT),
	SAVE_ITEM(S3C2440_GPJCON),
	SAVE_ITEM(S3C2440_GPJUP)
};

static int s3c244x_suspend(void)
{
	s3c_pm_do_save(s3c244x_sleep, ARRAY_SIZE(s3c244x_sleep));
	return 0;
}

static void s3c244x_resume(void)
{
	s3c_pm_do_restore(s3c244x_sleep, ARRAY_SIZE(s3c244x_sleep));
}

struct syscore_ops s3c244x_pm_syscore_ops = {
	.suspend	= s3c244x_suspend,
	.resume		= s3c244x_resume,
};
#endif
