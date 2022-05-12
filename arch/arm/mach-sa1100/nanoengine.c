// SPDX-License-Identifier: GPL-2.0-only
/*
 * linux/arch/arm/mach-sa1100/nanoengine.c
 *
 * Bright Star Engineering's nanoEngine board init code.
 *
 * Copyright (C) 2010 Marcelo Roberto Jimenez <mroberto@cpti.cetuc.puc-rio.br>
 */

#include <linux/init.h>
#include <linux/gpio/machine.h>
#include <linux/kernel.h>
#include <linux/platform_data/sa11x0-serial.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/root_dev.h>

#include <asm/mach-types.h>
#include <asm/setup.h>
#include <asm/page.h>

#include <asm/mach/arch.h>
#include <asm/mach/flash.h>
#include <asm/mach/map.h>

#include <mach/hardware.h>
#include <mach/nanoengine.h>
#include <mach/irqs.h>

#include "generic.h"

/* Flash bank 0 */
static struct mtd_partition nanoengine_partitions[] = {
	{
		.name	= "nanoEngine boot firmware and parameter table",
		.size		= 0x00010000,  /* 32K */
		.offset		= 0,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "kernel/initrd reserved",
		.size		= 0x002f0000,
		.offset		= 0x00010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "experimental filesystem allocation",
		.size		= 0x00100000,
		.offset		= 0x00300000,
		.mask_flags	= MTD_WRITEABLE,
	}
};

static struct flash_platform_data nanoengine_flash_data = {
	.map_name	= "jedec_probe",
	.parts		= nanoengine_partitions,
	.nr_parts	= ARRAY_SIZE(nanoengine_partitions),
};

static struct resource nanoengine_flash_resources[] = {
	DEFINE_RES_MEM(SA1100_CS0_PHYS, SZ_32M),
	DEFINE_RES_MEM(SA1100_CS1_PHYS, SZ_32M),
};

static struct map_desc nanoengine_io_desc[] __initdata = {
	{
		/* System Registers */
		.virtual	= 0xf0000000,
		.pfn		= __phys_to_pfn(0x10000000),
		.length		= 0x00100000,
		.type		= MT_DEVICE
	}, {
		/* Internal PCI Memory Read/Write */
		.virtual	= NANO_PCI_MEM_RW_VIRT,
		.pfn		= __phys_to_pfn(NANO_PCI_MEM_RW_PHYS),
		.length		= NANO_PCI_MEM_RW_SIZE,
		.type		= MT_DEVICE
	}, {
		/* Internal PCI Config Space */
		.virtual	= NANO_PCI_CONFIG_SPACE_VIRT,
		.pfn		= __phys_to_pfn(NANO_PCI_CONFIG_SPACE_PHYS),
		.length		= NANO_PCI_CONFIG_SPACE_SIZE,
		.type		= MT_DEVICE
	}
};

static void __init nanoengine_map_io(void)
{
	sa1100_map_io();
	iotable_init(nanoengine_io_desc, ARRAY_SIZE(nanoengine_io_desc));

	sa1100_register_uart(0, 1);
	sa1100_register_uart(1, 2);
	sa1100_register_uart(2, 3);
	Ser1SDCR0 |= SDCR0_UART;
	/* disable IRDA -- UART2 is used as a normal serial port */
	Ser2UTCR4 = 0;
	Ser2HSCR0 = 0;
}

static struct gpiod_lookup_table nanoengine_pcmcia0_gpio_table = {
	.dev_id = "sa11x0-pcmcia.0",
	.table = {
		GPIO_LOOKUP("gpio", 11, "ready", GPIO_ACTIVE_HIGH),
		GPIO_LOOKUP("gpio", 13, "detect", GPIO_ACTIVE_LOW),
		GPIO_LOOKUP("gpio", 15, "reset", GPIO_ACTIVE_HIGH),
		{ },
	},
};

static struct gpiod_lookup_table nanoengine_pcmcia1_gpio_table = {
	.dev_id = "sa11x0-pcmcia.1",
	.table = {
		GPIO_LOOKUP("gpio", 12, "ready", GPIO_ACTIVE_HIGH),
		GPIO_LOOKUP("gpio", 14, "detect", GPIO_ACTIVE_LOW),
		GPIO_LOOKUP("gpio", 16, "reset", GPIO_ACTIVE_HIGH),
		{ },
	},
};

static void __init nanoengine_init(void)
{
	sa11x0_register_pcmcia(0, &nanoengine_pcmcia0_gpio_table);
	sa11x0_register_pcmcia(1, &nanoengine_pcmcia1_gpio_table);
	sa11x0_register_mtd(&nanoengine_flash_data, nanoengine_flash_resources,
		ARRAY_SIZE(nanoengine_flash_resources));
}

MACHINE_START(NANOENGINE, "BSE nanoEngine")
	.atag_offset	= 0x100,
	.map_io		= nanoengine_map_io,
	.nr_irqs	= SA1100_NR_IRQS,
	.init_irq	= sa1100_init_irq,
	.init_time	= sa1100_timer_init,
	.init_machine	= nanoengine_init,
	.init_late	= sa11x0_init_late,
	.restart	= sa11x0_restart,
MACHINE_END
