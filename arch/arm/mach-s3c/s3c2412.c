// SPDX-License-Identifier: GPL-2.0
//
// Copyright (c) 2006 Simtec Electronics
//	Ben Dooks <ben@simtec.co.uk>
//
// http://armlinux.simtec.co.uk/.

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/syscore_ops.h>
#include <linux/serial_core.h>
#include <linux/serial_s3c.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/reboot.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>

#include <asm/proc-fns.h>
#include <asm/irq.h>
#include <asm/system_misc.h>

#include "map.h"
#include "regs-clock.h"
#include "regs-gpio.h"

#include "cpu.h"
#include "devs.h"
#include "pm.h"

#include "s3c24xx.h"
#include "nand-core-s3c24xx.h"
#include "regs-dsc-s3c24xx.h"
#include "s3c2412-power.h"

#ifndef CONFIG_CPU_S3C2412_ONLY
void __iomem *s3c24xx_va_gpio2 = S3C24XX_VA_GPIO;

static inline void s3c2412_init_gpio2(void)
{
	s3c24xx_va_gpio2 = S3C24XX_VA_GPIO + 0x10;
}
#else
#define s3c2412_init_gpio2() do { } while(0)
#endif

/* Initial IO mappings */

static struct map_desc s3c2412_iodesc[] __initdata __maybe_unused = {
	IODESC_ENT(CLKPWR),
	IODESC_ENT(TIMER),
	IODESC_ENT(WATCHDOG),
	{
		.virtual = (unsigned long)S3C2412_VA_SSMC,
		.pfn	 = __phys_to_pfn(S3C2412_PA_SSMC),
		.length	 = SZ_1M,
		.type	 = MT_DEVICE,
	},
	{
		.virtual = (unsigned long)S3C2412_VA_EBI,
		.pfn	 = __phys_to_pfn(S3C2412_PA_EBI),
		.length	 = SZ_1M,
		.type	 = MT_DEVICE,
	},
};

/* uart registration process */

void __init s3c2412_init_uarts(struct s3c2410_uartcfg *cfg, int no)
{
	s3c24xx_init_uartdevs("s3c2412-uart", s3c2410_uart_resources, cfg, no);

	/* rename devices that are s3c2412/s3c2413 specific */
	s3c_device_sdi.name  = "s3c2412-sdi";
	s3c_device_lcd.name  = "s3c2412-lcd";
	s3c_nand_setname("s3c2412-nand");

	/* alter IRQ of SDI controller */

	s3c_device_sdi.resource[1].start = IRQ_S3C2412_SDI;
	s3c_device_sdi.resource[1].end   = IRQ_S3C2412_SDI;

	/* spi channel related changes, s3c2412/13 specific */
	s3c_device_spi0.name = "s3c2412-spi";
	s3c_device_spi0.resource[0].end = S3C24XX_PA_SPI + 0x24;
	s3c_device_spi1.name = "s3c2412-spi";
	s3c_device_spi1.resource[0].start = S3C24XX_PA_SPI + S3C2412_SPI1;
	s3c_device_spi1.resource[0].end = S3C24XX_PA_SPI + S3C2412_SPI1 + 0x24;

}

/* s3c2412_idle
 *
 * use the standard idle call by ensuring the idle mode
 * in power config, then issuing the idle co-processor
 * instruction
*/

static void s3c2412_idle(void)
{
	unsigned long tmp;

	/* ensure our idle mode is to go to idle */

	tmp = __raw_readl(S3C2412_PWRCFG);
	tmp &= ~S3C2412_PWRCFG_STANDBYWFI_MASK;
	tmp |= S3C2412_PWRCFG_STANDBYWFI_IDLE;
	__raw_writel(tmp, S3C2412_PWRCFG);

	cpu_do_idle();
}

/* s3c2412_map_io
 *
 * register the standard cpu IO areas, and any passed in from the
 * machine specific initialisation.
*/

void __init s3c2412_map_io(void)
{
	/* move base of IO */

	s3c2412_init_gpio2();

	/* set our idle function */

	arm_pm_idle = s3c2412_idle;

	/* register our io-tables */

	iotable_init(s3c2412_iodesc, ARRAY_SIZE(s3c2412_iodesc));
}

/* need to register the subsystem before we actually register the device, and
 * we also need to ensure that it has been initialised before any of the
 * drivers even try to use it (even if not on an s3c2412 based system)
 * as a driver which may support both 2410 and 2440 may try and use it.
*/

struct bus_type s3c2412_subsys = {
	.name = "s3c2412-core",
	.dev_name = "s3c2412-core",
};

static int __init s3c2412_core_init(void)
{
	return subsys_system_register(&s3c2412_subsys, NULL);
}

core_initcall(s3c2412_core_init);

static struct device s3c2412_dev = {
	.bus		= &s3c2412_subsys,
};

int __init s3c2412_init(void)
{
	printk("S3C2412: Initialising architecture\n");

#ifdef CONFIG_PM_SLEEP
	register_syscore_ops(&s3c2412_pm_syscore_ops);
	register_syscore_ops(&s3c24xx_irq_syscore_ops);
#endif

	return device_register(&s3c2412_dev);
}
