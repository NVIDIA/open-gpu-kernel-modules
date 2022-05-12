// SPDX-License-Identifier: GPL-2.0
/*
 * SH7723 Setup
 *
 *  Copyright (C) 2008  Paul Mundt
 */
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/serial.h>
#include <linux/mm.h>
#include <linux/serial_sci.h>
#include <linux/uio_driver.h>
#include <linux/usb/r8a66597.h>
#include <linux/sh_timer.h>
#include <linux/sh_intc.h>
#include <linux/io.h>
#include <asm/clock.h>
#include <asm/mmzone.h>
#include <asm/platform_early.h>
#include <cpu/sh7723.h>

/* Serial */
static struct plat_sci_port scif0_platform_data = {
	.scscr		= SCSCR_REIE,
	.type           = PORT_SCIF,
	.regtype	= SCIx_SH4_SCIF_NO_SCSPTR_REGTYPE,
};

static struct resource scif0_resources[] = {
	DEFINE_RES_MEM(0xffe00000, 0x100),
	DEFINE_RES_IRQ(evt2irq(0xc00)),
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
	.type           = PORT_SCIF,
	.regtype	= SCIx_SH4_SCIF_NO_SCSPTR_REGTYPE,
};

static struct resource scif1_resources[] = {
	DEFINE_RES_MEM(0xffe10000, 0x100),
	DEFINE_RES_IRQ(evt2irq(0xc20)),
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
	.type           = PORT_SCIF,
	.regtype	= SCIx_SH4_SCIF_NO_SCSPTR_REGTYPE,
};

static struct resource scif2_resources[] = {
	DEFINE_RES_MEM(0xffe20000, 0x100),
	DEFINE_RES_IRQ(evt2irq(0xc40)),
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

static struct plat_sci_port scif3_platform_data = {
	.sampling_rate	= 8,
	.type           = PORT_SCIFA,
};

static struct resource scif3_resources[] = {
	DEFINE_RES_MEM(0xa4e30000, 0x100),
	DEFINE_RES_IRQ(evt2irq(0x900)),
};

static struct platform_device scif3_device = {
	.name		= "sh-sci",
	.id		= 3,
	.resource	= scif3_resources,
	.num_resources	= ARRAY_SIZE(scif3_resources),
	.dev		= {
		.platform_data	= &scif3_platform_data,
	},
};

static struct plat_sci_port scif4_platform_data = {
	.sampling_rate	= 8,
	.type           = PORT_SCIFA,
};

static struct resource scif4_resources[] = {
	DEFINE_RES_MEM(0xa4e40000, 0x100),
	DEFINE_RES_IRQ(evt2irq(0xd00)),
};

static struct platform_device scif4_device = {
	.name		= "sh-sci",
	.id		= 4,
	.resource	= scif4_resources,
	.num_resources	= ARRAY_SIZE(scif4_resources),
	.dev		= {
		.platform_data	= &scif4_platform_data,
	},
};

static struct plat_sci_port scif5_platform_data = {
	.sampling_rate	= 8,
	.type           = PORT_SCIFA,
};

static struct resource scif5_resources[] = {
	DEFINE_RES_MEM(0xa4e50000, 0x100),
	DEFINE_RES_IRQ(evt2irq(0xfa0)),
};

static struct platform_device scif5_device = {
	.name		= "sh-sci",
	.id		= 5,
	.resource	= scif5_resources,
	.num_resources	= ARRAY_SIZE(scif5_resources),
	.dev		= {
		.platform_data	= &scif5_platform_data,
	},
};

static struct uio_info vpu_platform_data = {
	.name = "VPU5",
	.version = "0",
	.irq = evt2irq(0x980),
};

static struct resource vpu_resources[] = {
	[0] = {
		.name	= "VPU",
		.start	= 0xfe900000,
		.end	= 0xfe902807,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		/* place holder for contiguous memory */
	},
};

static struct platform_device vpu_device = {
	.name		= "uio_pdrv_genirq",
	.id		= 0,
	.dev = {
		.platform_data	= &vpu_platform_data,
	},
	.resource	= vpu_resources,
	.num_resources	= ARRAY_SIZE(vpu_resources),
};

static struct uio_info veu0_platform_data = {
	.name = "VEU2H",
	.version = "0",
	.irq = evt2irq(0x8c0),
};

static struct resource veu0_resources[] = {
	[0] = {
		.name	= "VEU2H0",
		.start	= 0xfe920000,
		.end	= 0xfe92027b,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		/* place holder for contiguous memory */
	},
};

static struct platform_device veu0_device = {
	.name		= "uio_pdrv_genirq",
	.id		= 1,
	.dev = {
		.platform_data	= &veu0_platform_data,
	},
	.resource	= veu0_resources,
	.num_resources	= ARRAY_SIZE(veu0_resources),
};

static struct uio_info veu1_platform_data = {
	.name = "VEU2H",
	.version = "0",
	.irq = evt2irq(0x560),
};

static struct resource veu1_resources[] = {
	[0] = {
		.name	= "VEU2H1",
		.start	= 0xfe924000,
		.end	= 0xfe92427b,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		/* place holder for contiguous memory */
	},
};

static struct platform_device veu1_device = {
	.name		= "uio_pdrv_genirq",
	.id		= 2,
	.dev = {
		.platform_data	= &veu1_platform_data,
	},
	.resource	= veu1_resources,
	.num_resources	= ARRAY_SIZE(veu1_resources),
};

static struct sh_timer_config cmt_platform_data = {
	.channels_mask = 0x20,
};

static struct resource cmt_resources[] = {
	DEFINE_RES_MEM(0x044a0000, 0x70),
	DEFINE_RES_IRQ(evt2irq(0xf00)),
};

static struct platform_device cmt_device = {
	.name		= "sh-cmt-32",
	.id		= 0,
	.dev = {
		.platform_data	= &cmt_platform_data,
	},
	.resource	= cmt_resources,
	.num_resources	= ARRAY_SIZE(cmt_resources),
};

static struct sh_timer_config tmu0_platform_data = {
	.channels_mask = 7,
};

static struct resource tmu0_resources[] = {
	DEFINE_RES_MEM(0xffd80000, 0x2c),
	DEFINE_RES_IRQ(evt2irq(0x400)),
	DEFINE_RES_IRQ(evt2irq(0x420)),
	DEFINE_RES_IRQ(evt2irq(0x440)),
};

static struct platform_device tmu0_device = {
	.name		= "sh-tmu",
	.id		= 0,
	.dev = {
		.platform_data	= &tmu0_platform_data,
	},
	.resource	= tmu0_resources,
	.num_resources	= ARRAY_SIZE(tmu0_resources),
};

static struct sh_timer_config tmu1_platform_data = {
	.channels_mask = 7,
};

static struct resource tmu1_resources[] = {
	DEFINE_RES_MEM(0xffd90000, 0x2c),
	DEFINE_RES_IRQ(evt2irq(0x920)),
	DEFINE_RES_IRQ(evt2irq(0x940)),
	DEFINE_RES_IRQ(evt2irq(0x960)),
};

static struct platform_device tmu1_device = {
	.name		= "sh-tmu",
	.id		= 1,
	.dev = {
		.platform_data	= &tmu1_platform_data,
	},
	.resource	= tmu1_resources,
	.num_resources	= ARRAY_SIZE(tmu1_resources),
};

static struct resource rtc_resources[] = {
	[0] = {
		.start	= 0xa465fec0,
		.end	= 0xa465fec0 + 0x58 - 1,
		.flags	= IORESOURCE_IO,
	},
	[1] = {
		/* Period IRQ */
		.start	= evt2irq(0xaa0),
		.flags	= IORESOURCE_IRQ,
	},
	[2] = {
		/* Carry IRQ */
		.start	= evt2irq(0xac0),
		.flags	= IORESOURCE_IRQ,
	},
	[3] = {
		/* Alarm IRQ */
		.start	= evt2irq(0xa80),
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device rtc_device = {
	.name		= "sh-rtc",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(rtc_resources),
	.resource	= rtc_resources,
};

static struct r8a66597_platdata r8a66597_data = {
	.on_chip = 1,
};

static struct resource sh7723_usb_host_resources[] = {
	[0] = {
		.start	= 0xa4d80000,
		.end	= 0xa4d800ff,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= evt2irq(0xa20),
		.end	= evt2irq(0xa20),
		.flags	= IORESOURCE_IRQ | IRQF_TRIGGER_LOW,
	},
};

static struct platform_device sh7723_usb_host_device = {
	.name		= "r8a66597_hcd",
	.id		= 0,
	.dev = {
		.dma_mask		= NULL,         /*  not use dma */
		.coherent_dma_mask	= 0xffffffff,
		.platform_data		= &r8a66597_data,
	},
	.num_resources	= ARRAY_SIZE(sh7723_usb_host_resources),
	.resource	= sh7723_usb_host_resources,
};

static struct resource iic_resources[] = {
	[0] = {
		.name	= "IIC",
		.start  = 0x04470000,
		.end    = 0x04470017,
		.flags  = IORESOURCE_MEM,
	},
	[1] = {
		.start  = evt2irq(0xe00),
		.end    = evt2irq(0xe60),
		.flags  = IORESOURCE_IRQ,
       },
};

static struct platform_device iic_device = {
	.name           = "i2c-sh_mobile",
	.id             = 0, /* "i2c0" clock */
	.num_resources  = ARRAY_SIZE(iic_resources),
	.resource       = iic_resources,
};

static struct platform_device *sh7723_devices[] __initdata = {
	&scif0_device,
	&scif1_device,
	&scif2_device,
	&scif3_device,
	&scif4_device,
	&scif5_device,
	&cmt_device,
	&tmu0_device,
	&tmu1_device,
	&rtc_device,
	&iic_device,
	&sh7723_usb_host_device,
	&vpu_device,
	&veu0_device,
	&veu1_device,
};

static int __init sh7723_devices_setup(void)
{
	platform_resource_setup_memory(&vpu_device, "vpu", 2 << 20);
	platform_resource_setup_memory(&veu0_device, "veu0", 2 << 20);
	platform_resource_setup_memory(&veu1_device, "veu1", 2 << 20);

	return platform_add_devices(sh7723_devices,
				    ARRAY_SIZE(sh7723_devices));
}
arch_initcall(sh7723_devices_setup);

static struct platform_device *sh7723_early_devices[] __initdata = {
	&scif0_device,
	&scif1_device,
	&scif2_device,
	&scif3_device,
	&scif4_device,
	&scif5_device,
	&cmt_device,
	&tmu0_device,
	&tmu1_device,
};

void __init plat_early_device_setup(void)
{
	sh_early_platform_add_devices(sh7723_early_devices,
				   ARRAY_SIZE(sh7723_early_devices));
}

#define RAMCR_CACHE_L2FC	0x0002
#define RAMCR_CACHE_L2E		0x0001
#define L2_CACHE_ENABLE		(RAMCR_CACHE_L2E|RAMCR_CACHE_L2FC)

void l2_cache_init(void)
{
	/* Enable L2 cache */
	__raw_writel(L2_CACHE_ENABLE, RAMCR);
}

enum {
	UNUSED=0,
	ENABLED,
	DISABLED,

	/* interrupt sources */
	IRQ0, IRQ1, IRQ2, IRQ3, IRQ4, IRQ5, IRQ6, IRQ7,
	HUDI,
	DMAC1A_DEI0,DMAC1A_DEI1,DMAC1A_DEI2,DMAC1A_DEI3,
	_2DG_TRI,_2DG_INI,_2DG_CEI,
	DMAC0A_DEI0,DMAC0A_DEI1,DMAC0A_DEI2,DMAC0A_DEI3,
	VIO_CEUI,VIO_BEUI,VIO_VEU2HI,VIO_VOUI,
	SCIFA_SCIFA0,
	VPU_VPUI,
	TPU_TPUI,
	ADC_ADI,
	USB_USI0,
	RTC_ATI,RTC_PRI,RTC_CUI,
	DMAC1B_DEI4,DMAC1B_DEI5,DMAC1B_DADERR,
	DMAC0B_DEI4,DMAC0B_DEI5,DMAC0B_DADERR,
	KEYSC_KEYI,
	SCIF_SCIF0,SCIF_SCIF1,SCIF_SCIF2,
	MSIOF_MSIOFI0,MSIOF_MSIOFI1,
	SCIFA_SCIFA1,
	FLCTL_FLSTEI,FLCTL_FLTENDI,FLCTL_FLTREQ0I,FLCTL_FLTREQ1I,
	I2C_ALI,I2C_TACKI,I2C_WAITI,I2C_DTEI,
	CMT_CMTI,
	TSIF_TSIFI,
	SIU_SIUI,
	SCIFA_SCIFA2,
	TMU0_TUNI0, TMU0_TUNI1, TMU0_TUNI2,
	IRDA_IRDAI,
	ATAPI_ATAPII,
	VEU2H1_VEU2HI,
	LCDC_LCDCI,
	TMU1_TUNI0,TMU1_TUNI1,TMU1_TUNI2,

	/* interrupt groups */
	DMAC1A, DMAC0A, VIO, DMAC0B, FLCTL, I2C, _2DG,
	SDHI1, RTC, DMAC1B, SDHI0,
};

static struct intc_vect vectors[] __initdata = {
	INTC_VECT(IRQ0, 0x600), INTC_VECT(IRQ1, 0x620),
	INTC_VECT(IRQ2, 0x640), INTC_VECT(IRQ3, 0x660),
	INTC_VECT(IRQ4, 0x680), INTC_VECT(IRQ5, 0x6a0),
	INTC_VECT(IRQ6, 0x6c0), INTC_VECT(IRQ7, 0x6e0),

	INTC_VECT(DMAC1A_DEI0,0x700),
	INTC_VECT(DMAC1A_DEI1,0x720),
	INTC_VECT(DMAC1A_DEI2,0x740),
	INTC_VECT(DMAC1A_DEI3,0x760),

	INTC_VECT(_2DG_TRI, 0x780),
	INTC_VECT(_2DG_INI, 0x7A0),
	INTC_VECT(_2DG_CEI, 0x7C0),

	INTC_VECT(DMAC0A_DEI0,0x800),
	INTC_VECT(DMAC0A_DEI1,0x820),
	INTC_VECT(DMAC0A_DEI2,0x840),
	INTC_VECT(DMAC0A_DEI3,0x860),

	INTC_VECT(VIO_CEUI,0x880),
	INTC_VECT(VIO_BEUI,0x8A0),
	INTC_VECT(VIO_VEU2HI,0x8C0),
	INTC_VECT(VIO_VOUI,0x8E0),

	INTC_VECT(SCIFA_SCIFA0,0x900),
	INTC_VECT(VPU_VPUI,0x980),
	INTC_VECT(TPU_TPUI,0x9A0),
	INTC_VECT(ADC_ADI,0x9E0),
	INTC_VECT(USB_USI0,0xA20),

	INTC_VECT(RTC_ATI,0xA80),
	INTC_VECT(RTC_PRI,0xAA0),
	INTC_VECT(RTC_CUI,0xAC0),

	INTC_VECT(DMAC1B_DEI4,0xB00),
	INTC_VECT(DMAC1B_DEI5,0xB20),
	INTC_VECT(DMAC1B_DADERR,0xB40),

	INTC_VECT(DMAC0B_DEI4,0xB80),
	INTC_VECT(DMAC0B_DEI5,0xBA0),
	INTC_VECT(DMAC0B_DADERR,0xBC0),

	INTC_VECT(KEYSC_KEYI,0xBE0),
	INTC_VECT(SCIF_SCIF0,0xC00),
	INTC_VECT(SCIF_SCIF1,0xC20),
	INTC_VECT(SCIF_SCIF2,0xC40),
	INTC_VECT(MSIOF_MSIOFI0,0xC80),
	INTC_VECT(MSIOF_MSIOFI1,0xCA0),
	INTC_VECT(SCIFA_SCIFA1,0xD00),

	INTC_VECT(FLCTL_FLSTEI,0xD80),
	INTC_VECT(FLCTL_FLTENDI,0xDA0),
	INTC_VECT(FLCTL_FLTREQ0I,0xDC0),
	INTC_VECT(FLCTL_FLTREQ1I,0xDE0),

	INTC_VECT(I2C_ALI,0xE00),
	INTC_VECT(I2C_TACKI,0xE20),
	INTC_VECT(I2C_WAITI,0xE40),
	INTC_VECT(I2C_DTEI,0xE60),

	INTC_VECT(SDHI0, 0xE80),
	INTC_VECT(SDHI0, 0xEA0),
	INTC_VECT(SDHI0, 0xEC0),

	INTC_VECT(CMT_CMTI,0xF00),
	INTC_VECT(TSIF_TSIFI,0xF20),
	INTC_VECT(SIU_SIUI,0xF80),
	INTC_VECT(SCIFA_SCIFA2,0xFA0),

	INTC_VECT(TMU0_TUNI0,0x400),
	INTC_VECT(TMU0_TUNI1,0x420),
	INTC_VECT(TMU0_TUNI2,0x440),

	INTC_VECT(IRDA_IRDAI,0x480),
	INTC_VECT(ATAPI_ATAPII,0x4A0),

	INTC_VECT(SDHI1, 0x4E0),
	INTC_VECT(SDHI1, 0x500),
	INTC_VECT(SDHI1, 0x520),

	INTC_VECT(VEU2H1_VEU2HI,0x560),
	INTC_VECT(LCDC_LCDCI,0x580),

	INTC_VECT(TMU1_TUNI0,0x920),
	INTC_VECT(TMU1_TUNI1,0x940),
	INTC_VECT(TMU1_TUNI2,0x960),

};

static struct intc_group groups[] __initdata = {
	INTC_GROUP(DMAC1A,DMAC1A_DEI0,DMAC1A_DEI1,DMAC1A_DEI2,DMAC1A_DEI3),
	INTC_GROUP(DMAC0A,DMAC0A_DEI0,DMAC0A_DEI1,DMAC0A_DEI2,DMAC0A_DEI3),
	INTC_GROUP(VIO, VIO_CEUI,VIO_BEUI,VIO_VEU2HI,VIO_VOUI),
	INTC_GROUP(DMAC0B, DMAC0B_DEI4,DMAC0B_DEI5,DMAC0B_DADERR),
	INTC_GROUP(FLCTL,FLCTL_FLSTEI,FLCTL_FLTENDI,FLCTL_FLTREQ0I,FLCTL_FLTREQ1I),
	INTC_GROUP(I2C,I2C_ALI,I2C_TACKI,I2C_WAITI,I2C_DTEI),
	INTC_GROUP(_2DG, _2DG_TRI,_2DG_INI,_2DG_CEI),
	INTC_GROUP(RTC, RTC_ATI,RTC_PRI,RTC_CUI),
	INTC_GROUP(DMAC1B, DMAC1B_DEI4,DMAC1B_DEI5,DMAC1B_DADERR),
};

static struct intc_mask_reg mask_registers[] __initdata = {
	{ 0xa4080080, 0xa40800c0, 8, /* IMR0 / IMCR0 */
	  { 0, TMU1_TUNI2, TMU1_TUNI1, TMU1_TUNI0,
	    0, ENABLED, ENABLED, ENABLED } },
	{ 0xa4080084, 0xa40800c4, 8, /* IMR1 / IMCR1 */
	  { VIO_VOUI, VIO_VEU2HI,VIO_BEUI,VIO_CEUI,DMAC0A_DEI3,DMAC0A_DEI2,DMAC0A_DEI1,DMAC0A_DEI0 } },
	{ 0xa4080088, 0xa40800c8, 8, /* IMR2 / IMCR2 */
	  { 0, 0, 0, VPU_VPUI,0,0,0,SCIFA_SCIFA0 } },
	{ 0xa408008c, 0xa40800cc, 8, /* IMR3 / IMCR3 */
	  { DMAC1A_DEI3,DMAC1A_DEI2,DMAC1A_DEI1,DMAC1A_DEI0,0,0,0,IRDA_IRDAI } },
	{ 0xa4080090, 0xa40800d0, 8, /* IMR4 / IMCR4 */
	  { 0,TMU0_TUNI2,TMU0_TUNI1,TMU0_TUNI0,VEU2H1_VEU2HI,0,0,LCDC_LCDCI } },
	{ 0xa4080094, 0xa40800d4, 8, /* IMR5 / IMCR5 */
	  { KEYSC_KEYI,DMAC0B_DADERR,DMAC0B_DEI5,DMAC0B_DEI4,0,SCIF_SCIF2,SCIF_SCIF1,SCIF_SCIF0 } },
	{ 0xa4080098, 0xa40800d8, 8, /* IMR6 / IMCR6 */
	  { 0,0,0,SCIFA_SCIFA1,ADC_ADI,0,MSIOF_MSIOFI1,MSIOF_MSIOFI0 } },
	{ 0xa408009c, 0xa40800dc, 8, /* IMR7 / IMCR7 */
	  { I2C_DTEI, I2C_WAITI, I2C_TACKI, I2C_ALI,
	    FLCTL_FLTREQ1I, FLCTL_FLTREQ0I, FLCTL_FLTENDI, FLCTL_FLSTEI } },
	{ 0xa40800a0, 0xa40800e0, 8, /* IMR8 / IMCR8 */
	  { 0, ENABLED, ENABLED, ENABLED,
	    0, 0, SCIFA_SCIFA2, SIU_SIUI } },
	{ 0xa40800a4, 0xa40800e4, 8, /* IMR9 / IMCR9 */
	  { 0, 0, 0, CMT_CMTI, 0, 0, USB_USI0,0 } },
	{ 0xa40800a8, 0xa40800e8, 8, /* IMR10 / IMCR10 */
	  { 0, DMAC1B_DADERR,DMAC1B_DEI5,DMAC1B_DEI4,0,RTC_ATI,RTC_PRI,RTC_CUI } },
	{ 0xa40800ac, 0xa40800ec, 8, /* IMR11 / IMCR11 */
	  { 0,_2DG_CEI,_2DG_INI,_2DG_TRI,0,TPU_TPUI,0,TSIF_TSIFI } },
	{ 0xa40800b0, 0xa40800f0, 8, /* IMR12 / IMCR12 */
	  { 0,0,0,0,0,0,0,ATAPI_ATAPII } },
	{ 0xa4140044, 0xa4140064, 8, /* INTMSK00 / INTMSKCLR00 */
	  { IRQ0, IRQ1, IRQ2, IRQ3, IRQ4, IRQ5, IRQ6, IRQ7 } },
};

static struct intc_prio_reg prio_registers[] __initdata = {
	{ 0xa4080000, 0, 16, 4, /* IPRA */ { TMU0_TUNI0, TMU0_TUNI1, TMU0_TUNI2, IRDA_IRDAI } },
	{ 0xa4080004, 0, 16, 4, /* IPRB */ { VEU2H1_VEU2HI, LCDC_LCDCI, DMAC1A, 0} },
	{ 0xa4080008, 0, 16, 4, /* IPRC */ { TMU1_TUNI0, TMU1_TUNI1, TMU1_TUNI2, 0} },
	{ 0xa408000c, 0, 16, 4, /* IPRD */ { } },
	{ 0xa4080010, 0, 16, 4, /* IPRE */ { DMAC0A, VIO, SCIFA_SCIFA0, VPU_VPUI } },
	{ 0xa4080014, 0, 16, 4, /* IPRF */ { KEYSC_KEYI, DMAC0B, USB_USI0, CMT_CMTI } },
	{ 0xa4080018, 0, 16, 4, /* IPRG */ { SCIF_SCIF0, SCIF_SCIF1, SCIF_SCIF2,0 } },
	{ 0xa408001c, 0, 16, 4, /* IPRH */ { MSIOF_MSIOFI0,MSIOF_MSIOFI1, FLCTL, I2C } },
	{ 0xa4080020, 0, 16, 4, /* IPRI */ { SCIFA_SCIFA1,0,TSIF_TSIFI,_2DG } },
	{ 0xa4080024, 0, 16, 4, /* IPRJ */ { ADC_ADI,0,SIU_SIUI,SDHI1 } },
	{ 0xa4080028, 0, 16, 4, /* IPRK */ { RTC,DMAC1B,0,SDHI0 } },
	{ 0xa408002c, 0, 16, 4, /* IPRL */ { SCIFA_SCIFA2,0,TPU_TPUI,ATAPI_ATAPII } },
	{ 0xa4140010, 0, 32, 4, /* INTPRI00 */
	  { IRQ0, IRQ1, IRQ2, IRQ3, IRQ4, IRQ5, IRQ6, IRQ7 } },
};

static struct intc_sense_reg sense_registers[] __initdata = {
	{ 0xa414001c, 16, 2, /* ICR1 */
	  { IRQ0, IRQ1, IRQ2, IRQ3, IRQ4, IRQ5, IRQ6, IRQ7 } },
};

static struct intc_mask_reg ack_registers[] __initdata = {
	{ 0xa4140024, 0, 8, /* INTREQ00 */
	  { IRQ0, IRQ1, IRQ2, IRQ3, IRQ4, IRQ5, IRQ6, IRQ7 } },
};

static struct intc_desc intc_desc __initdata = {
	.name = "sh7723",
	.force_enable = ENABLED,
	.force_disable = DISABLED,
	.hw = INTC_HW_DESC(vectors, groups, mask_registers,
			   prio_registers, sense_registers, ack_registers),
};

void __init plat_irq_setup(void)
{
	register_intc_controller(&intc_desc);
}
