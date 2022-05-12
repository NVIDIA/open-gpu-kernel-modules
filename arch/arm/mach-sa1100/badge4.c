// SPDX-License-Identifier: GPL-2.0-only
/*
 * linux/arch/arm/mach-sa1100/badge4.c
 *
 * BadgePAD 4 specific initialization
 *
 *   Tim Connors <connors@hpl.hp.com>
 *   Christopher Hoover <ch@hpl.hp.com>
 *
 * Copyright (C) 2002 Hewlett-Packard Company
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/platform_data/sa11x0-serial.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/tty.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/leds.h>

#include <mach/hardware.h>
#include <asm/mach-types.h>
#include <asm/setup.h>
#include <mach/irqs.h>

#include <asm/mach/arch.h>
#include <asm/mach/flash.h>
#include <asm/mach/map.h>
#include <asm/hardware/sa1111.h>

#include <mach/badge4.h>

#include "generic.h"

static struct resource sa1111_resources[] = {
	[0] = DEFINE_RES_MEM(BADGE4_SA1111_BASE, 0x2000),
	[1] = DEFINE_RES_IRQ(BADGE4_IRQ_GPIO_SA1111),
};

static int badge4_sa1111_enable(void *data, unsigned devid)
{
	if (devid == SA1111_DEVID_USB)
		badge4_set_5V(BADGE4_5V_USB, 1);
	return 0;
}

static void badge4_sa1111_disable(void *data, unsigned devid)
{
	if (devid == SA1111_DEVID_USB)
		badge4_set_5V(BADGE4_5V_USB, 0);
}

static struct sa1111_platform_data sa1111_info = {
	.disable_devs	= SA1111_DEVID_PS2_MSE,
	.enable		= badge4_sa1111_enable,
	.disable	= badge4_sa1111_disable,
};

static u64 sa1111_dmamask = 0xffffffffUL;

static struct platform_device sa1111_device = {
	.name		= "sa1111",
	.id		= 0,
	.dev		= {
		.dma_mask = &sa1111_dmamask,
		.coherent_dma_mask = 0xffffffff,
		.platform_data = &sa1111_info,
	},
	.num_resources	= ARRAY_SIZE(sa1111_resources),
	.resource	= sa1111_resources,
};

/* LEDs */
struct gpio_led badge4_gpio_leds[] = {
	{
		.name			= "badge4:red",
		.default_trigger	= "heartbeat",
		.gpio			= 7,
	},
	{
		.name			= "badge4:green",
		.default_trigger	= "cpu0",
		.gpio			= 9,
	},
};

static struct gpio_led_platform_data badge4_gpio_led_info = {
	.leds		= badge4_gpio_leds,
	.num_leds	= ARRAY_SIZE(badge4_gpio_leds),
};

static struct platform_device badge4_leds = {
	.name	= "leds-gpio",
	.id	= -1,
	.dev	= {
		.platform_data	= &badge4_gpio_led_info,
	}
};

static struct platform_device *devices[] __initdata = {
	&sa1111_device,
	&badge4_leds,
};

static int __init badge4_sa1111_init(void)
{
	/*
	 * Ensure that the memory bus request/grant signals are setup,
	 * and the grant is held in its inactive state
	 */
	sa1110_mb_disable();

	/*
	 * Probe for SA1111.
	 */
	return platform_add_devices(devices, ARRAY_SIZE(devices));
}


/*
 * 1 x Intel 28F320C3 Advanced+ Boot Block Flash (32 Mi bit)
 *   Eight 4 KiW Parameter Bottom Blocks (64 KiB)
 *   Sixty-three 32 KiW Main Blocks (4032 Ki b)
 *
 * <or>
 *
 * 1 x Intel 28F640C3 Advanced+ Boot Block Flash (64 Mi bit)
 *   Eight 4 KiW Parameter Bottom Blocks (64 KiB)
 *   One-hundred-twenty-seven 32 KiW Main Blocks (8128 Ki b)
 */
static struct mtd_partition badge4_partitions[] = {
	{
		.name	= "BLOB boot loader",
		.offset	= 0,
		.size	= 0x0000A000
	}, {
		.name	= "params",
		.offset	= MTDPART_OFS_APPEND,
		.size	= 0x00006000
	}, {
		.name	= "root",
		.offset	= MTDPART_OFS_APPEND,
		.size	= MTDPART_SIZ_FULL
	}
};

static struct flash_platform_data badge4_flash_data = {
	.map_name	= "cfi_probe",
	.parts		= badge4_partitions,
	.nr_parts	= ARRAY_SIZE(badge4_partitions),
};

static struct resource badge4_flash_resource =
	DEFINE_RES_MEM(SA1100_CS0_PHYS, SZ_64M);

static int five_v_on __initdata = 0;

static int __init five_v_on_setup(char *ignore)
{
	five_v_on = 1;
	return 1;
}
__setup("five_v_on", five_v_on_setup);


static int __init badge4_init(void)
{
	int ret;

	if (!machine_is_badge4())
		return -ENODEV;

	/* LCD */
	GPCR  = (BADGE4_GPIO_LGP2 | BADGE4_GPIO_LGP3 |
		 BADGE4_GPIO_LGP4 | BADGE4_GPIO_LGP5 |
		 BADGE4_GPIO_LGP6 | BADGE4_GPIO_LGP7 |
		 BADGE4_GPIO_LGP8 | BADGE4_GPIO_LGP9 |
		 BADGE4_GPIO_GPA_VID | BADGE4_GPIO_GPB_VID |
		 BADGE4_GPIO_GPC_VID);
	GPDR &= ~BADGE4_GPIO_INT_VID;
	GPDR |= (BADGE4_GPIO_LGP2 | BADGE4_GPIO_LGP3 |
		 BADGE4_GPIO_LGP4 | BADGE4_GPIO_LGP5 |
		 BADGE4_GPIO_LGP6 | BADGE4_GPIO_LGP7 |
		 BADGE4_GPIO_LGP8 | BADGE4_GPIO_LGP9 |
		 BADGE4_GPIO_GPA_VID | BADGE4_GPIO_GPB_VID |
		 BADGE4_GPIO_GPC_VID);

	/* SDRAM SPD i2c */
	GPCR  = (BADGE4_GPIO_SDSDA | BADGE4_GPIO_SDSCL);
	GPDR |= (BADGE4_GPIO_SDSDA | BADGE4_GPIO_SDSCL);

	/* uart */
	GPCR  = (BADGE4_GPIO_UART_HS1 | BADGE4_GPIO_UART_HS2);
	GPDR |= (BADGE4_GPIO_UART_HS1 | BADGE4_GPIO_UART_HS2);

	/* CPLD muxsel0 input for mux/adc chip select */
	GPCR  = BADGE4_GPIO_MUXSEL0;
	GPDR |= BADGE4_GPIO_MUXSEL0;

	/* test points: J5, J6 as inputs, J7 outputs */
	GPDR &= ~(BADGE4_GPIO_TESTPT_J5 | BADGE4_GPIO_TESTPT_J6);
	GPCR  = BADGE4_GPIO_TESTPT_J7;
	GPDR |= BADGE4_GPIO_TESTPT_J7;

	/* 5V supply rail. */
	GPCR  = BADGE4_GPIO_PCMEN5V;		/* initially off */
	GPDR |= BADGE4_GPIO_PCMEN5V;

	/* CPLD sdram type inputs; set up by blob */
	//GPDR |= (BADGE4_GPIO_SDTYP1 | BADGE4_GPIO_SDTYP0);
	printk(KERN_DEBUG __FILE__ ": SDRAM CPLD typ1=%d typ0=%d\n",
		!!(GPLR & BADGE4_GPIO_SDTYP1),
		!!(GPLR & BADGE4_GPIO_SDTYP0));

	/* SA1111 reset pin; set up by blob */
	//GPSR  = BADGE4_GPIO_SA1111_NRST;
	//GPDR |= BADGE4_GPIO_SA1111_NRST;


	/* power management cruft */
	PGSR = 0;
	PWER = 0;
	PCFR = 0;
	PSDR = 0;

	PWER |= PWER_GPIO26;	/* wake up on an edge from TESTPT_J5 */
	PWER |= PWER_RTC;	/* wake up if rtc fires */

	/* drive sa1111_nrst during sleep */
	PGSR |= BADGE4_GPIO_SA1111_NRST;
	/* drive CPLD as is during sleep */
	PGSR |= (GPLR & (BADGE4_GPIO_SDTYP0|BADGE4_GPIO_SDTYP1));


	/* Now bring up the SA-1111. */
	ret = badge4_sa1111_init();
	if (ret < 0)
		printk(KERN_ERR
			"%s: SA-1111 initialization failed (%d)\n",
			__func__, ret);


	/* maybe turn on 5v0 from the start */
	badge4_set_5V(BADGE4_5V_INITIALLY, five_v_on);

	sa11x0_register_mtd(&badge4_flash_data, &badge4_flash_resource, 1);

	return 0;
}

arch_initcall(badge4_init);


static unsigned badge4_5V_bitmap = 0;

void badge4_set_5V(unsigned subsystem, int on)
{
	unsigned long flags;
	unsigned old_5V_bitmap;

	local_irq_save(flags);

	old_5V_bitmap = badge4_5V_bitmap;

	if (on) {
		badge4_5V_bitmap |= subsystem;
	} else {
		badge4_5V_bitmap &= ~subsystem;
	}

	/* detect on->off and off->on transitions */
	if ((!old_5V_bitmap) && (badge4_5V_bitmap)) {
		/* was off, now on */
		printk(KERN_INFO "%s: enabling 5V supply rail\n", __func__);
		GPSR = BADGE4_GPIO_PCMEN5V;
	} else if ((old_5V_bitmap) && (!badge4_5V_bitmap)) {
		/* was on, now off */
		printk(KERN_INFO "%s: disabling 5V supply rail\n", __func__);
		GPCR = BADGE4_GPIO_PCMEN5V;
	}

	local_irq_restore(flags);
}
EXPORT_SYMBOL(badge4_set_5V);


static struct map_desc badge4_io_desc[] __initdata = {
	{	/* SRAM  bank 1 */
		.virtual	= 0xf1000000,
		.pfn		= __phys_to_pfn(0x08000000),
		.length		= 0x00100000,
		.type		= MT_DEVICE
	}, {	/* SRAM  bank 2 */
		.virtual	= 0xf2000000,
		.pfn		= __phys_to_pfn(0x10000000),
		.length		= 0x00100000,
		.type		= MT_DEVICE
	}
};

static void
badge4_uart_pm(struct uart_port *port, u_int state, u_int oldstate)
{
	if (!state) {
		Ser1SDCR0 |= SDCR0_UART;
	}
}

static struct sa1100_port_fns badge4_port_fns __initdata = {
	.pm		= badge4_uart_pm,
};

static void __init badge4_map_io(void)
{
	sa1100_map_io();
	iotable_init(badge4_io_desc, ARRAY_SIZE(badge4_io_desc));

	sa1100_register_uart_fns(&badge4_port_fns);
	sa1100_register_uart(0, 3);
	sa1100_register_uart(1, 1);
}

MACHINE_START(BADGE4, "Hewlett-Packard Laboratories BadgePAD 4")
	.atag_offset	= 0x100,
	.map_io		= badge4_map_io,
	.nr_irqs	= SA1100_NR_IRQS,
	.init_irq	= sa1100_init_irq,
	.init_late	= sa11x0_init_late,
	.init_time	= sa1100_timer_init,
#ifdef CONFIG_SA1111
	.dma_zone_size	= SZ_1M,
#endif
	.restart	= sa11x0_restart,
MACHINE_END
