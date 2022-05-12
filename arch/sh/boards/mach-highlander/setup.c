// SPDX-License-Identifier: GPL-2.0
/*
 * arch/sh/boards/renesas/r7780rp/setup.c
 *
 * Renesas Solutions Highlander Support.
 *
 * Copyright (C) 2002 Atom Create Engineering Co., Ltd.
 * Copyright (C) 2005 - 2008 Paul Mundt
 *
 * This contains support for the R7780RP-1, R7780MP, and R7785RP
 * Highlander modules.
 */
#include <linux/init.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/ata_platform.h>
#include <linux/types.h>
#include <linux/mtd/physmap.h>
#include <linux/i2c.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/usb/r8a66597.h>
#include <linux/usb/m66592.h>
#include <linux/clkdev.h>
#include <net/ax88796.h>
#include <asm/machvec.h>
#include <mach/highlander.h>
#include <asm/clock.h>
#include <asm/heartbeat.h>
#include <asm/io.h>
#include <asm/io_trapped.h>

static struct r8a66597_platdata r8a66597_data = {
	.xtal = R8A66597_PLATDATA_XTAL_12MHZ,
	.vif = 1,
};

static struct resource r8a66597_usb_host_resources[] = {
	[0] = {
		.start	= 0xA4200000,
		.end	= 0xA42000FF,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= IRQ_EXT1,		/* irq number */
		.end	= IRQ_EXT1,
		.flags	= IORESOURCE_IRQ | IRQF_TRIGGER_LOW,
	},
};

static struct platform_device r8a66597_usb_host_device = {
	.name		= "r8a66597_hcd",
	.id		= -1,
	.dev = {
		.dma_mask		= NULL,		/* don't use dma */
		.coherent_dma_mask	= 0xffffffff,
		.platform_data		= &r8a66597_data,
	},
	.num_resources	= ARRAY_SIZE(r8a66597_usb_host_resources),
	.resource	= r8a66597_usb_host_resources,
};

static struct m66592_platdata usbf_platdata = {
	.xtal = M66592_PLATDATA_XTAL_24MHZ,
	.vif = 1,
};

static struct resource m66592_usb_peripheral_resources[] = {
	[0] = {
		.name	= "m66592_udc",
		.start	= 0xb0000000,
		.end	= 0xb00000FF,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.name	= "m66592_udc",
		.start	= IRQ_EXT4,		/* irq number */
		.end	= IRQ_EXT4,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device m66592_usb_peripheral_device = {
	.name		= "m66592_udc",
	.id		= -1,
	.dev = {
		.dma_mask		= NULL,		/* don't use dma */
		.coherent_dma_mask	= 0xffffffff,
		.platform_data		= &usbf_platdata,
	},
	.num_resources	= ARRAY_SIZE(m66592_usb_peripheral_resources),
	.resource	= m66592_usb_peripheral_resources,
};

static struct resource cf_ide_resources[] = {
	[0] = {
		.start	= PA_AREA5_IO + 0x1000,
		.end	= PA_AREA5_IO + 0x1000 + 0x08 - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= PA_AREA5_IO + 0x80c,
		.end	= PA_AREA5_IO + 0x80c + 0x16 - 1,
		.flags	= IORESOURCE_MEM,
	},
	[2] = {
		.start	= IRQ_CF,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct pata_platform_info pata_info = {
	.ioport_shift	= 1,
};

static struct platform_device cf_ide_device  = {
	.name		= "pata_platform",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(cf_ide_resources),
	.resource	= cf_ide_resources,
	.dev	= {
		.platform_data	= &pata_info,
	},
};

static struct resource heartbeat_resources[] = {
	[0] = {
		.start	= PA_OBLED,
		.end	= PA_OBLED,
		.flags	= IORESOURCE_MEM,
	},
};

#ifndef CONFIG_SH_R7785RP
static unsigned char heartbeat_bit_pos[] = { 2, 1, 0, 3, 6, 5, 4, 7 };

static struct heartbeat_data heartbeat_data = {
	.bit_pos	= heartbeat_bit_pos,
	.nr_bits	= ARRAY_SIZE(heartbeat_bit_pos),
};
#endif

static struct platform_device heartbeat_device = {
	.name		= "heartbeat",
	.id		= -1,

	/* R7785RP has a slightly more sensible FPGA.. */
#ifndef CONFIG_SH_R7785RP
	.dev	= {
		.platform_data	= &heartbeat_data,
	},
#endif
	.num_resources	= ARRAY_SIZE(heartbeat_resources),
	.resource	= heartbeat_resources,
};

static struct ax_plat_data ax88796_platdata = {
	.flags          = AXFLG_HAS_93CX6,
	.wordlength     = 2,
	.dcr_val        = 0x1,
	.rcr_val        = 0x40,
};

static struct resource ax88796_resources[] = {
	{
#ifdef CONFIG_SH_R7780RP
		.start  = 0xa5800400,
		.end    = 0xa5800400 + (0x20 * 0x2) - 1,
#else
		.start  = 0xa4100400,
		.end    = 0xa4100400 + (0x20 * 0x2) - 1,
#endif
		.flags  = IORESOURCE_MEM,
	},
	{
		.start  = IRQ_AX88796,
		.end    = IRQ_AX88796,
		.flags  = IORESOURCE_IRQ,
	},
};

static struct platform_device ax88796_device = {
	.name           = "ax88796",
	.id             = 0,

	.dev    = {
		.platform_data = &ax88796_platdata,
	},

	.num_resources  = ARRAY_SIZE(ax88796_resources),
	.resource       = ax88796_resources,
};

static struct mtd_partition nor_flash_partitions[] = {
	{
		.name		= "loader",
		.offset		= 0x00000000,
		.size		= 512 * 1024,
	},
	{
		.name		= "bootenv",
		.offset		= MTDPART_OFS_APPEND,
		.size		= 512 * 1024,
	},
	{
		.name		= "kernel",
		.offset		= MTDPART_OFS_APPEND,
		.size		= 4 * 1024 * 1024,
	},
	{
		.name		= "data",
		.offset		= MTDPART_OFS_APPEND,
		.size		= MTDPART_SIZ_FULL,
	},
};

static struct physmap_flash_data nor_flash_data = {
	.width		= 4,
	.parts		= nor_flash_partitions,
	.nr_parts	= ARRAY_SIZE(nor_flash_partitions),
};

/* This config is flash board for mass production. */
static struct resource nor_flash_resources[] = {
	[0]	= {
		.start	= PA_NORFLASH_ADDR,
		.end	= PA_NORFLASH_ADDR + PA_NORFLASH_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	}
};

static struct platform_device nor_flash_device = {
	.name		= "physmap-flash",
	.dev		= {
		.platform_data	= &nor_flash_data,
	},
	.num_resources	= ARRAY_SIZE(nor_flash_resources),
	.resource	= nor_flash_resources,
};

static struct resource smbus_resources[] = {
	[0] = {
		.start	= PA_SMCR,
		.end	= PA_SMCR + 0x100 - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= IRQ_SMBUS,
		.end	= IRQ_SMBUS,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device smbus_device = {
	.name		= "i2c-highlander",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(smbus_resources),
	.resource	= smbus_resources,
};

static struct i2c_board_info __initdata highlander_i2c_devices[] = {
	{
		I2C_BOARD_INFO("r2025sd", 0x32),
	},
};

static struct platform_device *r7780rp_devices[] __initdata = {
	&r8a66597_usb_host_device,
	&m66592_usb_peripheral_device,
	&heartbeat_device,
	&smbus_device,
	&nor_flash_device,
#ifndef CONFIG_SH_R7780RP
	&ax88796_device,
#endif
};

/*
 * The CF is connected using a 16-bit bus where 8-bit operations are
 * unsupported. The linux ata driver is however using 8-bit operations, so
 * insert a trapped io filter to convert 8-bit operations into 16-bit.
 */
static struct trapped_io cf_trapped_io = {
	.resource		= cf_ide_resources,
	.num_resources		= 2,
	.minimum_bus_width	= 16,
};

static int __init r7780rp_devices_setup(void)
{
	int ret = 0;

#ifndef CONFIG_SH_R7780RP
	if (register_trapped_io(&cf_trapped_io) == 0)
		ret |= platform_device_register(&cf_ide_device);
#endif

	ret |= platform_add_devices(r7780rp_devices,
				    ARRAY_SIZE(r7780rp_devices));

	ret |= i2c_register_board_info(0, highlander_i2c_devices,
				       ARRAY_SIZE(highlander_i2c_devices));

	return ret;
}
device_initcall(r7780rp_devices_setup);

/*
 * Platform specific clocks
 */
static int ivdr_clk_enable(struct clk *clk)
{
	__raw_writew(__raw_readw(PA_IVDRCTL) | (1 << IVDR_CK_ON), PA_IVDRCTL);
	return 0;
}

static void ivdr_clk_disable(struct clk *clk)
{
	__raw_writew(__raw_readw(PA_IVDRCTL) & ~(1 << IVDR_CK_ON), PA_IVDRCTL);
}

static struct sh_clk_ops ivdr_clk_ops = {
	.enable		= ivdr_clk_enable,
	.disable	= ivdr_clk_disable,
};

static struct clk ivdr_clk = {
	.ops		= &ivdr_clk_ops,
};

static struct clk *r7780rp_clocks[] = {
	&ivdr_clk,
};

static struct clk_lookup lookups[] = {
	/* main clocks */
	CLKDEV_CON_ID("ivdr_clk", &ivdr_clk),
};

static void r7780rp_power_off(void)
{
	if (mach_is_r7780mp() || mach_is_r7785rp())
		__raw_writew(0x0001, PA_POFF);
}

/*
 * Initialize the board
 */
static void __init highlander_setup(char **cmdline_p)
{
	u16 ver = __raw_readw(PA_VERREG);
	int i;

	printk(KERN_INFO "Renesas Solutions Highlander %s support.\n",
			 mach_is_r7780rp() ? "R7780RP-1" :
			 mach_is_r7780mp() ? "R7780MP"	 :
					     "R7785RP");

	printk(KERN_INFO "Board version: %d (revision %d), "
			 "FPGA version: %d (revision %d)\n",
			 (ver >> 12) & 0xf, (ver >> 8) & 0xf,
			 (ver >>  4) & 0xf, ver & 0xf);

	highlander_plat_pinmux_setup();

	/*
	 * Enable the important clocks right away..
	 */
	for (i = 0; i < ARRAY_SIZE(r7780rp_clocks); i++) {
		struct clk *clk = r7780rp_clocks[i];

		clk_register(clk);
		clk_enable(clk);
	}

	clkdev_add_table(lookups, ARRAY_SIZE(lookups));

	__raw_writew(0x0000, PA_OBLED);	/* Clear LED. */

	if (mach_is_r7780rp())
		__raw_writew(0x0001, PA_SDPOW);	/* SD Power ON */

	__raw_writew(__raw_readw(PA_IVDRCTL) | 0x01, PA_IVDRCTL);	/* Si13112 */

	pm_power_off = r7780rp_power_off;
}

static unsigned char irl2irq[HL_NR_IRL];

static int highlander_irq_demux(int irq)
{
	if (irq >= HL_NR_IRL || irq < 0 || !irl2irq[irq])
		return irq;

	return irl2irq[irq];
}

static void __init highlander_init_irq(void)
{
	unsigned char *ucp = highlander_plat_irq_setup();

	if (ucp) {
		plat_irq_setup_pins(IRQ_MODE_IRL3210);
		memcpy(irl2irq, ucp, HL_NR_IRL);
	}
}

/*
 * The Machine Vector
 */
static struct sh_machine_vector mv_highlander __initmv = {
	.mv_name		= "Highlander",
	.mv_setup		= highlander_setup,
	.mv_init_irq		= highlander_init_irq,
	.mv_irq_demux		= highlander_irq_demux,
};
