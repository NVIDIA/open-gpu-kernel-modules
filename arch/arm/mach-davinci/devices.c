// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * mach-davinci/devices.c
 *
 * DaVinci platform device setup/initialization
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/platform_data/i2c-davinci.h>
#include <linux/platform_data/mmc-davinci.h>
#include <linux/platform_data/edma.h>
#include <linux/dma-mapping.h>
#include <linux/io.h>
#include <linux/reboot.h>

#include <mach/hardware.h>
#include <mach/cputype.h>
#include <mach/mux.h>

#include "davinci.h"
#include "irqs.h"

#define DAVINCI_I2C_BASE	     0x01C21000
#define DAVINCI_ATA_BASE	     0x01C66000
#define DAVINCI_MMCSD0_BASE	     0x01E10000
#define DM355_MMCSD0_BASE	     0x01E11000
#define DM355_MMCSD1_BASE	     0x01E00000
#define DM365_MMCSD0_BASE	     0x01D11000
#define DM365_MMCSD1_BASE	     0x01D00000

void __iomem  *davinci_sysmod_base;

void davinci_map_sysmod(void)
{
	davinci_sysmod_base = ioremap(DAVINCI_SYSTEM_MODULE_BASE,
					      0x800);
	/*
	 * Throw a bug since a lot of board initialization code depends
	 * on system module availability. ioremap() failing this early
	 * need careful looking into anyway.
	 */
	BUG_ON(!davinci_sysmod_base);
}

static struct resource i2c_resources[] = {
	{
		.start		= DAVINCI_I2C_BASE,
		.end		= DAVINCI_I2C_BASE + 0x40,
		.flags		= IORESOURCE_MEM,
	},
	{
		.start		= DAVINCI_INTC_IRQ(IRQ_I2C),
		.flags		= IORESOURCE_IRQ,
	},
};

static struct platform_device davinci_i2c_device = {
	.name           = "i2c_davinci",
	.id             = 1,
	.num_resources	= ARRAY_SIZE(i2c_resources),
	.resource	= i2c_resources,
};

void __init davinci_init_i2c(struct davinci_i2c_platform_data *pdata)
{
	if (cpu_is_davinci_dm644x())
		davinci_cfg_reg(DM644X_I2C);

	davinci_i2c_device.dev.platform_data = pdata;
	(void) platform_device_register(&davinci_i2c_device);
}

static struct resource ide_resources[] = {
	{
		.start		= DAVINCI_ATA_BASE,
		.end		= DAVINCI_ATA_BASE + 0x7ff,
		.flags		= IORESOURCE_MEM,
	},
	{
		.start		= DAVINCI_INTC_IRQ(IRQ_IDE),
		.end		= DAVINCI_INTC_IRQ(IRQ_IDE),
		.flags		= IORESOURCE_IRQ,
	},
};

static u64 ide_dma_mask = DMA_BIT_MASK(32);

static struct platform_device ide_device = {
	.name           = "palm_bk3710",
	.id             = -1,
	.resource       = ide_resources,
	.num_resources  = ARRAY_SIZE(ide_resources),
	.dev = {
		.dma_mask		= &ide_dma_mask,
		.coherent_dma_mask      = DMA_BIT_MASK(32),
	},
};

void __init davinci_init_ide(void)
{
	if (cpu_is_davinci_dm644x()) {
		davinci_cfg_reg(DM644X_HPIEN_DISABLE);
		davinci_cfg_reg(DM644X_ATAEN);
		davinci_cfg_reg(DM644X_HDIREN);
	} else if (cpu_is_davinci_dm646x()) {
		/* IRQ_DM646X_IDE is the same as IRQ_IDE */
		davinci_cfg_reg(DM646X_ATAEN);
	} else {
		WARN_ON(1);
		return;
	}

	platform_device_register(&ide_device);
}

#if IS_ENABLED(CONFIG_MMC_DAVINCI)

static u64 mmcsd0_dma_mask = DMA_BIT_MASK(32);

static struct resource mmcsd0_resources[] = {
	{
		/* different on dm355 */
		.start = DAVINCI_MMCSD0_BASE,
		.end   = DAVINCI_MMCSD0_BASE + SZ_4K - 1,
		.flags = IORESOURCE_MEM,
	},
	/* IRQs:  MMC/SD, then SDIO */
	{
		.start = DAVINCI_INTC_IRQ(IRQ_MMCINT),
		.flags = IORESOURCE_IRQ,
	}, {
		/* different on dm355 */
		.start = DAVINCI_INTC_IRQ(IRQ_SDIOINT),
		.flags = IORESOURCE_IRQ,
	},
};

static struct platform_device davinci_mmcsd0_device = {
	.name = "dm6441-mmc",
	.id = 0,
	.dev = {
		.dma_mask = &mmcsd0_dma_mask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
	},
	.num_resources = ARRAY_SIZE(mmcsd0_resources),
	.resource = mmcsd0_resources,
};

static u64 mmcsd1_dma_mask = DMA_BIT_MASK(32);

static struct resource mmcsd1_resources[] = {
	{
		.start = DM355_MMCSD1_BASE,
		.end   = DM355_MMCSD1_BASE + SZ_4K - 1,
		.flags = IORESOURCE_MEM,
	},
	/* IRQs:  MMC/SD, then SDIO */
	{
		.start = DAVINCI_INTC_IRQ(IRQ_DM355_MMCINT1),
		.flags = IORESOURCE_IRQ,
	}, {
		.start = DAVINCI_INTC_IRQ(IRQ_DM355_SDIOINT1),
		.flags = IORESOURCE_IRQ,
	},
};

static struct platform_device davinci_mmcsd1_device = {
	.name = "dm6441-mmc",
	.id = 1,
	.dev = {
		.dma_mask = &mmcsd1_dma_mask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
	},
	.num_resources = ARRAY_SIZE(mmcsd1_resources),
	.resource = mmcsd1_resources,
};


void __init davinci_setup_mmc(int module, struct davinci_mmc_config *config)
{
	struct platform_device	*pdev = NULL;

	if (WARN_ON(cpu_is_davinci_dm646x()))
		return;

	/* REVISIT: update PINMUX, ARM_IRQMUX, and EDMA_EVTMUX here too;
	 * for example if MMCSD1 is used for SDIO, maybe DAT2 is unused.
	 *
	 * FIXME dm6441 (no MMC/SD), dm357 (one), and dm335 (two) are
	 * not handled right here ...
	 */
	switch (module) {
	case 1:
		if (cpu_is_davinci_dm355()) {
			/* REVISIT we may not need all these pins if e.g. this
			 * is a hard-wired SDIO device...
			 */
			davinci_cfg_reg(DM355_SD1_CMD);
			davinci_cfg_reg(DM355_SD1_CLK);
			davinci_cfg_reg(DM355_SD1_DATA0);
			davinci_cfg_reg(DM355_SD1_DATA1);
			davinci_cfg_reg(DM355_SD1_DATA2);
			davinci_cfg_reg(DM355_SD1_DATA3);
		} else if (cpu_is_davinci_dm365()) {
			/* Configure pull down control */
			unsigned v;

			v = __raw_readl(DAVINCI_SYSMOD_VIRT(SYSMOD_PUPDCTL1));
			__raw_writel(v & ~0xfc0,
					DAVINCI_SYSMOD_VIRT(SYSMOD_PUPDCTL1));

			mmcsd1_resources[0].start = DM365_MMCSD1_BASE;
			mmcsd1_resources[0].end = DM365_MMCSD1_BASE +
							SZ_4K - 1;
			mmcsd1_resources[2].start = DAVINCI_INTC_IRQ(
							IRQ_DM365_SDIOINT1);
			davinci_mmcsd1_device.name = "da830-mmc";
		} else
			break;

		pdev = &davinci_mmcsd1_device;
		break;
	case 0:
		if (cpu_is_davinci_dm355()) {
			mmcsd0_resources[0].start = DM355_MMCSD0_BASE;
			mmcsd0_resources[0].end = DM355_MMCSD0_BASE + SZ_4K - 1;
			mmcsd0_resources[2].start = DAVINCI_INTC_IRQ(
							IRQ_DM355_SDIOINT0);

			/* expose all 6 MMC0 signals:  CLK, CMD, DATA[0..3] */
			davinci_cfg_reg(DM355_MMCSD0);

			/* enable RX EDMA */
			davinci_cfg_reg(DM355_EVT26_MMC0_RX);
		} else if (cpu_is_davinci_dm365()) {
			mmcsd0_resources[0].start = DM365_MMCSD0_BASE;
			mmcsd0_resources[0].end = DM365_MMCSD0_BASE +
							SZ_4K - 1;
			mmcsd0_resources[2].start = DAVINCI_INTC_IRQ(
							IRQ_DM365_SDIOINT0);
			davinci_mmcsd0_device.name = "da830-mmc";
		} else if (cpu_is_davinci_dm644x()) {
			/* REVISIT: should this be in board-init code? */
			/* Power-on 3.3V IO cells */
			__raw_writel(0,
				DAVINCI_SYSMOD_VIRT(SYSMOD_VDD3P3VPWDN));
			/*Set up the pull regiter for MMC */
			davinci_cfg_reg(DM644X_MSTK);
		}

		pdev = &davinci_mmcsd0_device;
		break;
	}

	if (WARN_ON(!pdev))
		return;

	pdev->dev.platform_data = config;
	platform_device_register(pdev);
}

#else

void __init davinci_setup_mmc(int module, struct davinci_mmc_config *config)
{
}

#endif

/*-------------------------------------------------------------------------*/

static struct resource wdt_resources[] = {
	{
		.start	= DAVINCI_WDOG_BASE,
		.end	= DAVINCI_WDOG_BASE + SZ_1K - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device davinci_wdt_device = {
	.name		= "davinci-wdt",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(wdt_resources),
	.resource	= wdt_resources,
};

int davinci_init_wdt(void)
{
	return platform_device_register(&davinci_wdt_device);
}

static struct platform_device davinci_gpio_device = {
	.name	= "davinci_gpio",
	.id	= -1,
};

int davinci_gpio_register(struct resource *res, int size, void *pdata)
{
	davinci_gpio_device.resource = res;
	davinci_gpio_device.num_resources = size;
	davinci_gpio_device.dev.platform_data = pdata;
	return platform_device_register(&davinci_gpio_device);
}
