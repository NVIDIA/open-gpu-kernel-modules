// SPDX-License-Identifier: GPL-2.0-only
/*
 * Cavium Networks CNS3420 Validation Board
 *
 * Copyright 2000 Deep Blue Solutions Ltd
 * Copyright 2008 ARM Limited
 * Copyright 2008 Cavium Networks
 *		  Scott Shu
 * Copyright 2010 MontaVista Software, LLC.
 *		  Anton Vorontsov <avorontsov@mvista.com>
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/compiler.h>
#include <linux/io.h>
#include <linux/dma-mapping.h>
#include <linux/serial_core.h>
#include <linux/serial_8250.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/physmap.h>
#include <linux/mtd/partitions.h>
#include <linux/usb/ehci_pdriver.h>
#include <linux/usb/ohci_pdriver.h>
#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/time.h>
#include "cns3xxx.h"
#include "pm.h"
#include "core.h"
#include "devices.h"

/*
 * NOR Flash
 */
static struct mtd_partition cns3420_nor_partitions[] = {
	{
		.name		= "uboot",
		.size		= 0x00040000,
		.offset		= 0,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "kernel",
		.size		= 0x004C0000,
		.offset		= MTDPART_OFS_APPEND,
	}, {
		.name		= "filesystem",
		.size		= 0x7000000,
		.offset		= MTDPART_OFS_APPEND,
	}, {
		.name		= "filesystem2",
		.size		= 0x0AE0000,
		.offset		= MTDPART_OFS_APPEND,
	}, {
		.name		= "ubootenv",
		.size		= MTDPART_SIZ_FULL,
		.offset		= MTDPART_OFS_APPEND,
	},
};

static struct physmap_flash_data cns3420_nor_pdata = {
	.width = 2,
	.parts = cns3420_nor_partitions,
	.nr_parts = ARRAY_SIZE(cns3420_nor_partitions),
};

static struct resource cns3420_nor_res = {
	.start = CNS3XXX_FLASH_BASE,
	.end = CNS3XXX_FLASH_BASE + SZ_128M - 1,
	.flags = IORESOURCE_MEM | IORESOURCE_MEM_32BIT,
};

static struct platform_device cns3420_nor_pdev = {
	.name = "physmap-flash",
	.id = 0,
	.resource = &cns3420_nor_res,
	.num_resources = 1,
	.dev = {
		.platform_data = &cns3420_nor_pdata,
	},
};

/*
 * UART
 */
static void __init cns3420_early_serial_setup(void)
{
#ifdef CONFIG_SERIAL_8250_CONSOLE
	static struct uart_port cns3420_serial_port = {
		.membase        = (void __iomem *)CNS3XXX_UART0_BASE_VIRT,
		.mapbase        = CNS3XXX_UART0_BASE,
		.irq            = IRQ_CNS3XXX_UART0,
		.iotype         = UPIO_MEM,
		.flags          = UPF_BOOT_AUTOCONF | UPF_FIXED_TYPE,
		.regshift       = 2,
		.uartclk        = 24000000,
		.line           = 0,
		.type           = PORT_16550A,
		.fifosize       = 16,
	};

	early_serial_setup(&cns3420_serial_port);
#endif
}

/*
 * USB
 */
static struct resource cns3xxx_usb_ehci_resources[] = {
	[0] = {
		.start = CNS3XXX_USB_BASE,
		.end   = CNS3XXX_USB_BASE + SZ_16M - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_CNS3XXX_USB_EHCI,
		.flags = IORESOURCE_IRQ,
	},
};

static u64 cns3xxx_usb_ehci_dma_mask = DMA_BIT_MASK(32);

static int csn3xxx_usb_power_on(struct platform_device *pdev)
{
	/*
	 * EHCI and OHCI share the same clock and power,
	 * resetting twice would cause the 1st controller been reset.
	 * Therefore only do power up  at the first up device, and
	 * power down at the last down device.
	 *
	 * Set USB AHB INCR length to 16
	 */
	if (atomic_inc_return(&usb_pwr_ref) == 1) {
		cns3xxx_pwr_power_up(1 << PM_PLL_HM_PD_CTRL_REG_OFFSET_PLL_USB);
		cns3xxx_pwr_clk_en(1 << PM_CLK_GATE_REG_OFFSET_USB_HOST);
		cns3xxx_pwr_soft_rst(1 << PM_SOFT_RST_REG_OFFST_USB_HOST);
		__raw_writel((__raw_readl(MISC_CHIP_CONFIG_REG) | (0X2 << 24)),
			MISC_CHIP_CONFIG_REG);
	}

	return 0;
}

static void csn3xxx_usb_power_off(struct platform_device *pdev)
{
	/*
	 * EHCI and OHCI share the same clock and power,
	 * resetting twice would cause the 1st controller been reset.
	 * Therefore only do power up  at the first up device, and
	 * power down at the last down device.
	 */
	if (atomic_dec_return(&usb_pwr_ref) == 0)
		cns3xxx_pwr_clk_dis(1 << PM_CLK_GATE_REG_OFFSET_USB_HOST);
}

static struct usb_ehci_pdata cns3xxx_usb_ehci_pdata = {
	.power_on	= csn3xxx_usb_power_on,
	.power_off	= csn3xxx_usb_power_off,
};

static struct platform_device cns3xxx_usb_ehci_device = {
	.name          = "ehci-platform",
	.num_resources = ARRAY_SIZE(cns3xxx_usb_ehci_resources),
	.resource      = cns3xxx_usb_ehci_resources,
	.dev           = {
		.dma_mask          = &cns3xxx_usb_ehci_dma_mask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
		.platform_data     = &cns3xxx_usb_ehci_pdata,
	},
};

static struct resource cns3xxx_usb_ohci_resources[] = {
	[0] = {
		.start = CNS3XXX_USB_OHCI_BASE,
		.end   = CNS3XXX_USB_OHCI_BASE + SZ_16M - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_CNS3XXX_USB_OHCI,
		.flags = IORESOURCE_IRQ,
	},
};

static u64 cns3xxx_usb_ohci_dma_mask = DMA_BIT_MASK(32);

static struct usb_ohci_pdata cns3xxx_usb_ohci_pdata = {
	.num_ports	= 1,
	.power_on	= csn3xxx_usb_power_on,
	.power_off	= csn3xxx_usb_power_off,
};

static struct platform_device cns3xxx_usb_ohci_device = {
	.name          = "ohci-platform",
	.num_resources = ARRAY_SIZE(cns3xxx_usb_ohci_resources),
	.resource      = cns3xxx_usb_ohci_resources,
	.dev           = {
		.dma_mask          = &cns3xxx_usb_ohci_dma_mask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
		.platform_data	   = &cns3xxx_usb_ohci_pdata,
	},
};

/*
 * Initialization
 */
static struct platform_device *cns3420_pdevs[] __initdata = {
	&cns3420_nor_pdev,
	&cns3xxx_usb_ehci_device,
	&cns3xxx_usb_ohci_device,
};

static void __init cns3420_init(void)
{
	cns3xxx_l2x0_init();

	platform_add_devices(cns3420_pdevs, ARRAY_SIZE(cns3420_pdevs));

	cns3xxx_ahci_init();
	cns3xxx_sdhci_init();

	pm_power_off = cns3xxx_power_off;
}

static struct map_desc cns3420_io_desc[] __initdata = {
	{
		.virtual	= CNS3XXX_UART0_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS3XXX_UART0_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE,
	},
};

static void __init cns3420_map_io(void)
{
	cns3xxx_map_io();
	iotable_init(cns3420_io_desc, ARRAY_SIZE(cns3420_io_desc));

	cns3420_early_serial_setup();
}

MACHINE_START(CNS3420VB, "Cavium Networks CNS3420 Validation Board")
	.atag_offset	= 0x100,
	.map_io		= cns3420_map_io,
	.init_irq	= cns3xxx_init_irq,
	.init_time	= cns3xxx_timer_init,
	.init_machine	= cns3420_init,
	.init_late      = cns3xxx_pcie_init_late,
	.restart	= cns3xxx_restart,
MACHINE_END
