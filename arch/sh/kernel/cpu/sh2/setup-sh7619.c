// SPDX-License-Identifier: GPL-2.0
/*
 * SH7619 Setup
 *
 *  Copyright (C) 2006  Yoshinori Sato
 *  Copyright (C) 2009  Paul Mundt
 */
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/serial.h>
#include <linux/serial_sci.h>
#include <linux/sh_eth.h>
#include <linux/sh_timer.h>
#include <linux/io.h>
#include <asm/platform_early.h>

enum {
	UNUSED = 0,

	/* interrupt sources */
	IRQ0, IRQ1, IRQ2, IRQ3, IRQ4, IRQ5, IRQ6, IRQ7,
	WDT, EDMAC, CMT0, CMT1,
	SCIF0, SCIF1, SCIF2,
	HIF_HIFI, HIF_HIFBI,
	DMAC0, DMAC1, DMAC2, DMAC3,
	SIOF,
};

static struct intc_vect vectors[] __initdata = {
	INTC_IRQ(IRQ0, 64), INTC_IRQ(IRQ1, 65),
	INTC_IRQ(IRQ2, 66), INTC_IRQ(IRQ3, 67),
	INTC_IRQ(IRQ4, 80), INTC_IRQ(IRQ5, 81),
	INTC_IRQ(IRQ6, 82), INTC_IRQ(IRQ7, 83),
	INTC_IRQ(WDT, 84), INTC_IRQ(EDMAC, 85),
	INTC_IRQ(CMT0, 86), INTC_IRQ(CMT1, 87),
	INTC_IRQ(SCIF0, 88), INTC_IRQ(SCIF0, 89),
	INTC_IRQ(SCIF0, 90), INTC_IRQ(SCIF0, 91),
	INTC_IRQ(SCIF1, 92), INTC_IRQ(SCIF1, 93),
	INTC_IRQ(SCIF1, 94), INTC_IRQ(SCIF1, 95),
	INTC_IRQ(SCIF2, 96), INTC_IRQ(SCIF2, 97),
	INTC_IRQ(SCIF2, 98), INTC_IRQ(SCIF2, 99),
	INTC_IRQ(HIF_HIFI, 100), INTC_IRQ(HIF_HIFBI, 101),
	INTC_IRQ(DMAC0, 104), INTC_IRQ(DMAC1, 105),
	INTC_IRQ(DMAC2, 106), INTC_IRQ(DMAC3, 107),
	INTC_IRQ(SIOF, 108),
};

static struct intc_prio_reg prio_registers[] __initdata = {
	{ 0xf8140006, 0, 16, 4, /* IPRA */ { IRQ0, IRQ1, IRQ2, IRQ3 } },
	{ 0xf8140008, 0, 16, 4, /* IPRB */ { IRQ4, IRQ5, IRQ6, IRQ7 } },
	{ 0xf8080000, 0, 16, 4, /* IPRC */ { WDT, EDMAC, CMT0, CMT1 } },
	{ 0xf8080002, 0, 16, 4, /* IPRD */ { SCIF0, SCIF1, SCIF2 } },
	{ 0xf8080004, 0, 16, 4, /* IPRE */ { HIF_HIFI, HIF_HIFBI } },
	{ 0xf8080006, 0, 16, 4, /* IPRF */ { DMAC0, DMAC1, DMAC2, DMAC3 } },
	{ 0xf8080008, 0, 16, 4, /* IPRG */ { SIOF } },
};

static DECLARE_INTC_DESC(intc_desc, "sh7619", vectors, NULL,
			 NULL, prio_registers, NULL);

static struct plat_sci_port scif0_platform_data = {
	.scscr		= SCSCR_REIE,
	.type		= PORT_SCIF,
};

static struct resource scif0_resources[] = {
	DEFINE_RES_MEM(0xf8400000, 0x100),
	DEFINE_RES_IRQ(88),
};

static struct platform_device scif0_device = {
	.name		= "sh-sci",
	.id		= 0,
	.resource	= scif0_resources,
	.num_resources	= ARRAY_SIZE(scif0_resources),
	.dev		= {
		.platform_data	= &scif0_platform_data,
	},
};

static struct plat_sci_port scif1_platform_data = {
	.scscr		= SCSCR_REIE,
	.type		= PORT_SCIF,
};

static struct resource scif1_resources[] = {
	DEFINE_RES_MEM(0xf8410000, 0x100),
	DEFINE_RES_IRQ(92),
};

static struct platform_device scif1_device = {
	.name		= "sh-sci",
	.id		= 1,
	.resource	= scif1_resources,
	.num_resources	= ARRAY_SIZE(scif1_resources),
	.dev		= {
		.platform_data	= &scif1_platform_data,
	},
};

static struct plat_sci_port scif2_platform_data = {
	.scscr		= SCSCR_REIE,
	.type		= PORT_SCIF,
};

static struct resource scif2_resources[] = {
	DEFINE_RES_MEM(0xf8420000, 0x100),
	DEFINE_RES_IRQ(96),
};

static struct platform_device scif2_device = {
	.name		= "sh-sci",
	.id		= 2,
	.resource	= scif2_resources,
	.num_resources	= ARRAY_SIZE(scif2_resources),
	.dev		= {
		.platform_data	= &scif2_platform_data,
	},
};

static struct sh_eth_plat_data eth_platform_data = {
	.phy		= 1,
	.phy_interface	= PHY_INTERFACE_MODE_MII,
};

static struct resource eth_resources[] = {
	[0] = {
		.start = 0xfb000000,
		.end = 0xfb0001c7,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = 85,
		.end = 85,
		.flags = IORESOURCE_IRQ,
	},
};

static struct platform_device eth_device = {
	.name = "sh7619-ether",
	.id = -1,
	.dev = {
		.platform_data = &eth_platform_data,
	},
	.num_resources = ARRAY_SIZE(eth_resources),
	.resource = eth_resources,
};

static struct sh_timer_config cmt_platform_data = {
	.channels_mask = 3,
};

static struct resource cmt_resources[] = {
	DEFINE_RES_MEM(0xf84a0070, 0x10),
	DEFINE_RES_IRQ(86),
	DEFINE_RES_IRQ(87),
};

static struct platform_device cmt_device = {
	.name		= "sh-cmt-16",
	.id		= 0,
	.dev = {
		.platform_data	= &cmt_platform_data,
	},
	.resource	= cmt_resources,
	.num_resources	= ARRAY_SIZE(cmt_resources),
};

static struct platform_device *sh7619_devices[] __initdata = {
	&scif0_device,
	&scif1_device,
	&scif2_device,
	&eth_device,
	&cmt_device,
};

static int __init sh7619_devices_setup(void)
{
	return platform_add_devices(sh7619_devices,
				    ARRAY_SIZE(sh7619_devices));
}
arch_initcall(sh7619_devices_setup);

void __init plat_irq_setup(void)
{
	register_intc_controller(&intc_desc);
}

static struct platform_device *sh7619_early_devices[] __initdata = {
	&scif0_device,
	&scif1_device,
	&scif2_device,
	&cmt_device,
};

#define STBCR3 0xf80a0000

void __init plat_early_device_setup(void)
{
	/* enable CMT clock */
	__raw_writeb(__raw_readb(STBCR3) & ~0x10, STBCR3);

	sh_early_platform_add_devices(sh7619_early_devices,
				   ARRAY_SIZE(sh7619_early_devices));
}
