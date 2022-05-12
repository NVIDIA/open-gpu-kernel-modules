// SPDX-License-Identifier: GPL-2.0-only
/*
 * linux/arch/arm/mach-sa1100/cerf.c
 *
 * Apr-2003 : Removed some old PDA crud [FB]
 * Oct-2003 : Added uart2 resource [FB]
 * Jan-2004 : Removed io map for flash [FB]
 */

#include <linux/init.h>
#include <linux/gpio/machine.h>
#include <linux/kernel.h>
#include <linux/tty.h>
#include <linux/platform_data/sa11x0-serial.h>
#include <linux/platform_device.h>
#include <linux/irq.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/gpio.h>
#include <linux/leds.h>

#include <mach/hardware.h>
#include <asm/setup.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/flash.h>
#include <asm/mach/map.h>

#include <mach/cerf.h>
#include <linux/platform_data/mfd-mcp-sa11x0.h>
#include <mach/irqs.h>
#include "generic.h"

static struct resource cerfuart2_resources[] = {
	[0] = DEFINE_RES_MEM(0x80030000, SZ_64K),
};

static struct platform_device cerfuart2_device = {
	.name		= "sa11x0-uart",
	.id		= 2,
	.num_resources	= ARRAY_SIZE(cerfuart2_resources),
	.resource	= cerfuart2_resources,
};

/* Compact Flash */
static struct gpiod_lookup_table cerf_cf_gpio_table = {
	.dev_id = "sa11x0-pcmcia.1",
	.table = {
		GPIO_LOOKUP("gpio", 19, "bvd2", GPIO_ACTIVE_HIGH),
		GPIO_LOOKUP("gpio", 20, "bvd1", GPIO_ACTIVE_HIGH),
		GPIO_LOOKUP("gpio", 21, "reset", GPIO_ACTIVE_HIGH),
		GPIO_LOOKUP("gpio", 22, "ready", GPIO_ACTIVE_HIGH),
		GPIO_LOOKUP("gpio", 23, "detect", GPIO_ACTIVE_LOW),
		{ },
	},
};

/* LEDs */
struct gpio_led cerf_gpio_leds[] = {
	{
		.name			= "cerf:d0",
		.default_trigger	= "heartbeat",
		.gpio			= 0,
	},
	{
		.name			= "cerf:d1",
		.default_trigger	= "cpu0",
		.gpio			= 1,
	},
	{
		.name			= "cerf:d2",
		.default_trigger	= "default-on",
		.gpio			= 2,
	},
	{
		.name			= "cerf:d3",
		.default_trigger	= "default-on",
		.gpio			= 3,
	},

};

static struct gpio_led_platform_data cerf_gpio_led_info = {
	.leds		= cerf_gpio_leds,
	.num_leds	= ARRAY_SIZE(cerf_gpio_leds),
};

static struct platform_device *cerf_devices[] __initdata = {
	&cerfuart2_device,
};

#ifdef CONFIG_SA1100_CERF_FLASH_32MB
#  define CERF_FLASH_SIZE	0x02000000
#elif defined CONFIG_SA1100_CERF_FLASH_16MB
#  define CERF_FLASH_SIZE	0x01000000
#elif defined CONFIG_SA1100_CERF_FLASH_8MB
#  define CERF_FLASH_SIZE	0x00800000
#else
#  error "Undefined flash size for CERF"
#endif

static struct mtd_partition cerf_partitions[] = {
	{
		.name		= "Bootloader",
		.size		= 0x00020000,
		.offset		= 0x00000000,
	}, {
		.name		= "Params",
		.size		= 0x00040000,
		.offset		= 0x00020000,
	}, {
		.name		= "Kernel",
		.size		= 0x00100000,
		.offset		= 0x00060000,
	}, {
		.name		= "Filesystem",
		.size		= CERF_FLASH_SIZE-0x00160000,
		.offset		= 0x00160000,
	}
};

static struct flash_platform_data cerf_flash_data = {
	.map_name	= "cfi_probe",
	.parts		= cerf_partitions,
	.nr_parts	= ARRAY_SIZE(cerf_partitions),
};

static struct resource cerf_flash_resource =
	DEFINE_RES_MEM(SA1100_CS0_PHYS, SZ_32M);

static void __init cerf_init_irq(void)
{
	sa1100_init_irq();
	irq_set_irq_type(CERF_ETH_IRQ, IRQ_TYPE_EDGE_RISING);
}

static struct map_desc cerf_io_desc[] __initdata = {
  	{	/* Crystal Ethernet Chip */
		.virtual	=  0xf0000000,
		.pfn		= __phys_to_pfn(0x08000000),
		.length		= 0x00100000,
		.type		= MT_DEVICE
	}
};

static void __init cerf_map_io(void)
{
	sa1100_map_io();
	iotable_init(cerf_io_desc, ARRAY_SIZE(cerf_io_desc));

	sa1100_register_uart(0, 3);
	sa1100_register_uart(1, 2); /* disable this and the uart2 device for sa1100_fir */
	sa1100_register_uart(2, 1);
}

static struct mcp_plat_data cerf_mcp_data = {
	.mccr0		= MCCR0_ADM,
	.sclk_rate	= 11981000,
};

static void __init cerf_init(void)
{
	sa11x0_ppc_configure_mcp();
	platform_add_devices(cerf_devices, ARRAY_SIZE(cerf_devices));
	gpio_led_register_device(-1, &cerf_gpio_led_info);
	sa11x0_register_mtd(&cerf_flash_data, &cerf_flash_resource, 1);
	sa11x0_register_mcp(&cerf_mcp_data);
	sa11x0_register_pcmcia(1, &cerf_cf_gpio_table);
}

MACHINE_START(CERF, "Intrinsyc CerfBoard/CerfCube")
	/* Maintainer: support@intrinsyc.com */
	.map_io		= cerf_map_io,
	.nr_irqs	= SA1100_NR_IRQS,
	.init_irq	= cerf_init_irq,
	.init_time	= sa1100_timer_init,
	.init_machine	= cerf_init,
	.init_late	= sa11x0_init_late,
	.restart	= sa11x0_restart,
MACHINE_END
