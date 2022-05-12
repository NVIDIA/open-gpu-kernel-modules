// SPDX-License-Identifier: GPL-2.0
/*
 * SH7343 Setup
 *
 *  Copyright (C) 2006  Paul Mundt
 */
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/serial.h>
#include <linux/serial_sci.h>
#include <linux/uio_driver.h>
#include <linux/sh_timer.h>
#include <linux/sh_intc.h>
#include <asm/clock.h>
#include <asm/platform_early.h>

/* Serial */
static struct plat_sci_port scif0_platform_data = {
	.scscr		= SCSCR_CKE1,
	.type           = PORT_SCIF,
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
	.scscr		= SCSCR_CKE1,
	.type           = PORT_SCIF,
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
	.scscr		= SCSCR_CKE1,
	.type           = PORT_SCIF,
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
	.scscr		= SCSCR_CKE1,
	.type           = PORT_SCIF,
};

static struct resource scif3_resources[] = {
	DEFINE_RES_MEM(0xffe30000, 0x100),
	DEFINE_RES_IRQ(evt2irq(0xc60)),
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

static struct resource iic0_resources[] = {
	[0] = {
		.name	= "IIC0",
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

static struct platform_device iic0_device = {
	.name           = "i2c-sh_mobile",
	.id             = 0, /* "i2c0" clock */
	.num_resources  = ARRAY_SIZE(iic0_resources),
	.resource       = iic0_resources,
};

static struct resource iic1_resources[] = {
	[0] = {
		.name	= "IIC1",
		.start  = 0x04750000,
		.end    = 0x04750017,
		.flags  = IORESOURCE_MEM,
	},
	[1] = {
		.start  = evt2irq(0x780),
		.end    = evt2irq(0x7e0),
		.flags  = IORESOURCE_IRQ,
       },
};

static struct platform_device iic1_device = {
	.name           = "i2c-sh_mobile",
	.id             = 1, /* "i2c1" clock */
	.num_resources  = ARRAY_SIZE(iic1_resources),
	.resource       = iic1_resources,
};

static struct uio_info vpu_platform_data = {
	.name = "VPU4",
	.version = "0",
	.irq = evt2irq(0x980),
};

static struct resource vpu_resources[] = {
	[0] = {
		.name	= "VPU",
		.start	= 0xfe900000,
		.end	= 0xfe9022eb,
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

static struct uio_info veu_platform_data = {
	.name = "VEU",
	.version = "0",
	.irq = evt2irq(0x8c0),
};

static struct resource veu_resources[] = {
	[0] = {
		.name	= "VEU",
		.start	= 0xfe920000,
		.end	= 0xfe9200b7,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		/* place holder for contiguous memory */
	},
};

static struct platform_device veu_device = {
	.name		= "uio_pdrv_genirq",
	.id		= 1,
	.dev = {
		.platform_data	= &veu_platform_data,
	},
	.resource	= veu_resources,
	.num_resources	= ARRAY_SIZE(veu_resources),
};

static struct uio_info jpu_platform_data = {
	.name = "JPU",
	.version = "0",
	.irq = evt2irq(0x560),
};

static struct resource jpu_resources[] = {
	[0] = {
		.name	= "JPU",
		.start	= 0xfea00000,
		.end	= 0xfea102d3,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		/* place holder for contiguous memory */
	},
};

static struct platform_device jpu_device = {
	.name		= "uio_pdrv_genirq",
	.id		= 2,
	.dev = {
		.platform_data	= &jpu_platform_data,
	},
	.resource	= jpu_resources,
	.num_resources	= ARRAY_SIZE(jpu_resources),
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

static struct platform_device *sh7343_devices[] __initdata = {
	&scif0_device,
	&scif1_device,
	&scif2_device,
	&scif3_device,
	&cmt_device,
	&tmu0_device,
	&iic0_device,
	&iic1_device,
	&vpu_device,
	&veu_device,
	&jpu_device,
};

static int __init sh7343_devices_setup(void)
{
	platform_resource_setup_memory(&vpu_device, "vpu", 1 << 20);
	platform_resource_setup_memory(&veu_device, "veu", 2 << 20);
	platform_resource_setup_memory(&jpu_device, "jpu", 2 << 20);

	return platform_add_devices(sh7343_devices,
				    ARRAY_SIZE(sh7343_devices));
}
arch_initcall(sh7343_devices_setup);

static struct platform_device *sh7343_early_devices[] __initdata = {
	&scif0_device,
	&scif1_device,
	&scif2_device,
	&scif3_device,
	&cmt_device,
	&tmu0_device,
};

void __init plat_early_device_setup(void)
{
	sh_early_platform_add_devices(sh7343_early_devices,
				   ARRAY_SIZE(sh7343_early_devices));
}

enum {
	UNUSED = 0,
	ENABLED,
	DISABLED,

	/* interrupt sources */
	IRQ0, IRQ1, IRQ2, IRQ3, IRQ4, IRQ5, IRQ6, IRQ7,
	DMAC0, DMAC1, DMAC2, DMAC3,
	VIO_CEUI, VIO_BEUI, VIO_VEUI, VOU,
	MFI, VPU, TPU, Z3D4, USBI0, USBI1,
	MMC_ERR, MMC_TRAN, MMC_FSTAT, MMC_FRDY,
	DMAC4, DMAC5, DMAC_DADERR,
	KEYSC,
	SCIF, SCIF1, SCIF2, SCIF3,
	SIOF0, SIOF1, SIO,
	FLCTL_FLSTEI, FLCTL_FLENDI, FLCTL_FLTREQ0I, FLCTL_FLTREQ1I,
	I2C0_ALI, I2C0_TACKI, I2C0_WAITI, I2C0_DTEI,
	I2C1_ALI, I2C1_TACKI, I2C1_WAITI, I2C1_DTEI,
	SIM_TEI, SIM_TXI, SIM_RXI, SIM_ERI,
	IRDA, SDHI, CMT, TSIF, SIU,
	TMU0, TMU1, TMU2,
	JPU, LCDC,

	/* interrupt groups */

	DMAC0123, VIOVOU, MMC, DMAC45, FLCTL, I2C0, I2C1, SIM, USB,
};

static struct intc_vect vectors[] __initdata = {
	INTC_VECT(IRQ0, 0x600), INTC_VECT(IRQ1, 0x620),
	INTC_VECT(IRQ2, 0x640), INTC_VECT(IRQ3, 0x660),
	INTC_VECT(IRQ4, 0x680), INTC_VECT(IRQ5, 0x6a0),
	INTC_VECT(IRQ6, 0x6c0), INTC_VECT(IRQ7, 0x6e0),
	INTC_VECT(I2C1_ALI, 0x780), INTC_VECT(I2C1_TACKI, 0x7a0),
	INTC_VECT(I2C1_WAITI, 0x7c0), INTC_VECT(I2C1_DTEI, 0x7e0),
	INTC_VECT(DMAC0, 0x800), INTC_VECT(DMAC1, 0x820),
	INTC_VECT(DMAC2, 0x840), INTC_VECT(DMAC3, 0x860),
	INTC_VECT(VIO_CEUI, 0x880), INTC_VECT(VIO_BEUI, 0x8a0),
	INTC_VECT(VIO_VEUI, 0x8c0), INTC_VECT(VOU, 0x8e0),
	INTC_VECT(MFI, 0x900), INTC_VECT(VPU, 0x980),
	INTC_VECT(TPU, 0x9a0), INTC_VECT(Z3D4, 0x9e0),
	INTC_VECT(USBI0, 0xa20), INTC_VECT(USBI1, 0xa40),
	INTC_VECT(MMC_ERR, 0xb00), INTC_VECT(MMC_TRAN, 0xb20),
	INTC_VECT(MMC_FSTAT, 0xb40), INTC_VECT(MMC_FRDY, 0xb60),
	INTC_VECT(DMAC4, 0xb80), INTC_VECT(DMAC5, 0xba0),
	INTC_VECT(DMAC_DADERR, 0xbc0), INTC_VECT(KEYSC, 0xbe0),
	INTC_VECT(SCIF, 0xc00), INTC_VECT(SCIF1, 0xc20),
	INTC_VECT(SCIF2, 0xc40), INTC_VECT(SCIF3, 0xc60),
	INTC_VECT(SIOF0, 0xc80), INTC_VECT(SIOF1, 0xca0),
	INTC_VECT(SIO, 0xd00),
	INTC_VECT(FLCTL_FLSTEI, 0xd80), INTC_VECT(FLCTL_FLENDI, 0xda0),
	INTC_VECT(FLCTL_FLTREQ0I, 0xdc0), INTC_VECT(FLCTL_FLTREQ1I, 0xde0),
	INTC_VECT(I2C0_ALI, 0xe00), INTC_VECT(I2C0_TACKI, 0xe20),
	INTC_VECT(I2C0_WAITI, 0xe40), INTC_VECT(I2C0_DTEI, 0xe60),
	INTC_VECT(SDHI, 0xe80), INTC_VECT(SDHI, 0xea0),
	INTC_VECT(SDHI, 0xec0), INTC_VECT(SDHI, 0xee0),
	INTC_VECT(CMT, 0xf00), INTC_VECT(TSIF, 0xf20),
	INTC_VECT(SIU, 0xf80),
	INTC_VECT(TMU0, 0x400), INTC_VECT(TMU1, 0x420),
	INTC_VECT(TMU2, 0x440),
	INTC_VECT(JPU, 0x560), INTC_VECT(LCDC, 0x580),
};

static struct intc_group groups[] __initdata = {
	INTC_GROUP(DMAC0123, DMAC0, DMAC1, DMAC2, DMAC3),
	INTC_GROUP(VIOVOU, VIO_CEUI, VIO_BEUI, VIO_VEUI, VOU),
	INTC_GROUP(MMC, MMC_FRDY, MMC_FSTAT, MMC_TRAN, MMC_ERR),
	INTC_GROUP(DMAC45, DMAC4, DMAC5, DMAC_DADERR),
	INTC_GROUP(FLCTL, FLCTL_FLSTEI, FLCTL_FLENDI,
		   FLCTL_FLTREQ0I, FLCTL_FLTREQ1I),
	INTC_GROUP(I2C0, I2C0_ALI, I2C0_TACKI, I2C0_WAITI, I2C0_DTEI),
	INTC_GROUP(I2C1, I2C1_ALI, I2C1_TACKI, I2C1_WAITI, I2C1_DTEI),
	INTC_GROUP(SIM, SIM_TEI, SIM_TXI, SIM_RXI, SIM_ERI),
	INTC_GROUP(USB, USBI0, USBI1),
};

static struct intc_mask_reg mask_registers[] __initdata = {
	{ 0xa4080084, 0xa40800c4, 8, /* IMR1 / IMCR1 */
	  { VOU, VIO_VEUI, VIO_BEUI, VIO_CEUI, DMAC3, DMAC2, DMAC1, DMAC0 } },
	{ 0xa4080088, 0xa40800c8, 8, /* IMR2 / IMCR2 */
	  { 0, 0, 0, VPU, 0, 0, 0, MFI } },
	{ 0xa408008c, 0xa40800cc, 8, /* IMR3 / IMCR3 */
	  { SIM_TEI, SIM_TXI, SIM_RXI, SIM_ERI, 0, 0, 0, IRDA } },
	{ 0xa4080090, 0xa40800d0, 8, /* IMR4 / IMCR4 */
	  { 0, TMU2, TMU1, TMU0, JPU, 0, 0, LCDC } },
	{ 0xa4080094, 0xa40800d4, 8, /* IMR5 / IMCR5 */
	  { KEYSC, DMAC_DADERR, DMAC5, DMAC4, SCIF3, SCIF2, SCIF1, SCIF } },
	{ 0xa4080098, 0xa40800d8, 8, /* IMR6 / IMCR6 */
	  { 0, 0, 0, SIO, Z3D4, 0, SIOF1, SIOF0 } },
	{ 0xa408009c, 0xa40800dc, 8, /* IMR7 / IMCR7 */
	  { I2C0_DTEI, I2C0_WAITI, I2C0_TACKI, I2C0_ALI,
	    FLCTL_FLTREQ1I, FLCTL_FLTREQ0I, FLCTL_FLENDI, FLCTL_FLSTEI } },
	{ 0xa40800a0, 0xa40800e0, 8, /* IMR8 / IMCR8 */
	  { DISABLED, ENABLED, ENABLED, ENABLED, 0, 0, 0, SIU } },
	{ 0xa40800a4, 0xa40800e4, 8, /* IMR9 / IMCR9 */
	  { 0, 0, 0, CMT, 0, USBI1, USBI0 } },
	{ 0xa40800a8, 0xa40800e8, 8, /* IMR10 / IMCR10 */
	  { MMC_FRDY, MMC_FSTAT, MMC_TRAN, MMC_ERR } },
	{ 0xa40800ac, 0xa40800ec, 8, /* IMR11 / IMCR11 */
	  { I2C1_DTEI, I2C1_WAITI, I2C1_TACKI, I2C1_ALI, TPU, 0, 0, TSIF } },
	{ 0xa4140044, 0xa4140064, 8, /* INTMSK00 / INTMSKCLR00 */
	  { IRQ0, IRQ1, IRQ2, IRQ3, IRQ4, IRQ5, IRQ6, IRQ7 } },
};

static struct intc_prio_reg prio_registers[] __initdata = {
	{ 0xa4080000, 0, 16, 4, /* IPRA */ { TMU0, TMU1, TMU2 } },
	{ 0xa4080004, 0, 16, 4, /* IPRB */ { JPU, LCDC, SIM } },
	{ 0xa4080010, 0, 16, 4, /* IPRE */ { DMAC0123, VIOVOU, MFI, VPU } },
	{ 0xa4080014, 0, 16, 4, /* IPRF */ { KEYSC, DMAC45, USB, CMT } },
	{ 0xa4080018, 0, 16, 4, /* IPRG */ { SCIF, SCIF1, SCIF2, SCIF3 } },
	{ 0xa408001c, 0, 16, 4, /* IPRH */ { SIOF0, SIOF1, FLCTL, I2C0 } },
	{ 0xa4080020, 0, 16, 4, /* IPRI */ { SIO, 0, TSIF, I2C1 } },
	{ 0xa4080024, 0, 16, 4, /* IPRJ */ { Z3D4, 0, SIU } },
	{ 0xa4080028, 0, 16, 4, /* IPRK */ { 0, MMC, 0, SDHI } },
	{ 0xa408002c, 0, 16, 4, /* IPRL */ { 0, 0, TPU } },
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
	.name = "sh7343",
	.force_enable = ENABLED,
	.force_disable = DISABLED,
	.hw = INTC_HW_DESC(vectors, groups, mask_registers,
			   prio_registers, sense_registers, ack_registers),
};

void __init plat_irq_setup(void)
{
	register_intc_controller(&intc_desc);
}
