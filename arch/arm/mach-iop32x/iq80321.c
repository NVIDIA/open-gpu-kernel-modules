// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * arch/arm/mach-iop32x/iq80321.c
 *
 * Board support code for the Intel IQ80321 platform.
 *
 * Author: Rory Bolt <rorybolt@pacbell.net>
 * Copyright (C) 2002 Rory Bolt
 * Copyright (C) 2004 Intel Corp.
 */

#include <linux/mm.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/string.h>
#include <linux/serial_core.h>
#include <linux/serial_8250.h>
#include <linux/mtd/physmap.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/gpio/machine.h>
#include <asm/irq.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/pci.h>
#include <asm/mach/time.h>
#include <asm/mach-types.h>
#include <asm/page.h>

#include "hardware.h"
#include "irqs.h"
#include "gpio-iop32x.h"

/*
 * IQ80321 timer tick configuration.
 */
static void __init iq80321_timer_init(void)
{
	/* 33.333 MHz crystal.  */
	iop_init_time(200000000);
}


/*
 * IQ80321 I/O.
 */
static struct map_desc iq80321_io_desc[] __initdata = {
 	{	/* on-board devices */
		.virtual	= IQ80321_UART,
		.pfn		= __phys_to_pfn(IQ80321_UART),
		.length		= 0x00100000,
		.type		= MT_DEVICE,
	},
};

void __init iq80321_map_io(void)
{
	iop3xx_map_io();
	iotable_init(iq80321_io_desc, ARRAY_SIZE(iq80321_io_desc));
}


/*
 * IQ80321 PCI.
 */
static int __init
iq80321_pci_map_irq(const struct pci_dev *dev, u8 slot, u8 pin)
{
	int irq;

	if ((slot == 2 || slot == 6) && pin == 1) {
		/* PCI-X Slot INTA */
		irq = IRQ_IOP32X_XINT2;
	} else if ((slot == 2 || slot == 6) && pin == 2) {
		/* PCI-X Slot INTA */
		irq = IRQ_IOP32X_XINT3;
	} else if ((slot == 2 || slot == 6) && pin == 3) {
		/* PCI-X Slot INTA */
		irq = IRQ_IOP32X_XINT0;
	} else if ((slot == 2 || slot == 6) && pin == 4) {
		/* PCI-X Slot INTA */
		irq = IRQ_IOP32X_XINT1;
	} else if (slot == 4 || slot == 8) {
		/* Gig-E */
		irq = IRQ_IOP32X_XINT0;
	} else {
		printk(KERN_ERR "iq80321_pci_map_irq() called for unknown "
			"device PCI:%d:%d:%d\n", dev->bus->number,
			PCI_SLOT(dev->devfn), PCI_FUNC(dev->devfn));
		irq = -1;
	}

	return irq;
}

static struct hw_pci iq80321_pci __initdata = {
	.nr_controllers = 1,
	.ops		= &iop3xx_ops,
	.setup		= iop3xx_pci_setup,
	.preinit	= iop3xx_pci_preinit_cond,
	.map_irq	= iq80321_pci_map_irq,
};

static int __init iq80321_pci_init(void)
{
	if ((iop3xx_get_init_atu() == IOP3XX_INIT_ATU_ENABLE) &&
		machine_is_iq80321())
		pci_common_init(&iq80321_pci);

	return 0;
}

subsys_initcall(iq80321_pci_init);


/*
 * IQ80321 machine initialisation.
 */
static struct physmap_flash_data iq80321_flash_data = {
	.width		= 1,
};

static struct resource iq80321_flash_resource = {
	.start		= 0xf0000000,
	.end		= 0xf07fffff,
	.flags		= IORESOURCE_MEM,
};

static struct platform_device iq80321_flash_device = {
	.name		= "physmap-flash",
	.id		= 0,
	.dev		= {
		.platform_data	= &iq80321_flash_data,
	},
	.num_resources	= 1,
	.resource	= &iq80321_flash_resource,
};

static struct plat_serial8250_port iq80321_serial_port[] = {
	{
		.mapbase	= IQ80321_UART,
		.membase	= (char *)IQ80321_UART,
		.irq		= IRQ_IOP32X_XINT1,
		.flags		= UPF_SKIP_TEST,
		.iotype		= UPIO_MEM,
		.regshift	= 0,
		.uartclk	= 1843200,
	},
	{ },
};

static struct resource iq80321_uart_resource = {
	.start		= IQ80321_UART,
	.end		= IQ80321_UART + 7,
	.flags		= IORESOURCE_MEM,
};

static struct platform_device iq80321_serial_device = {
	.name		= "serial8250",
	.id		= PLAT8250_DEV_PLATFORM,
	.dev		= {
		.platform_data		= iq80321_serial_port,
	},
	.num_resources	= 1,
	.resource	= &iq80321_uart_resource,
};

static void __init iq80321_init_machine(void)
{
	register_iop32x_gpio();
	gpiod_add_lookup_table(&iop3xx_i2c0_gpio_lookup);
	gpiod_add_lookup_table(&iop3xx_i2c1_gpio_lookup);
	platform_device_register(&iop3xx_i2c0_device);
	platform_device_register(&iop3xx_i2c1_device);
	platform_device_register(&iq80321_flash_device);
	platform_device_register(&iq80321_serial_device);
	platform_device_register(&iop3xx_dma_0_channel);
	platform_device_register(&iop3xx_dma_1_channel);
	platform_device_register(&iop3xx_aau_channel);
}

MACHINE_START(IQ80321, "Intel IQ80321")
	/* Maintainer: Intel Corp. */
	.atag_offset	= 0x100,
	.map_io		= iq80321_map_io,
	.init_irq	= iop32x_init_irq,
	.init_time	= iq80321_timer_init,
	.init_machine	= iq80321_init_machine,
	.restart	= iop3xx_restart,
MACHINE_END
