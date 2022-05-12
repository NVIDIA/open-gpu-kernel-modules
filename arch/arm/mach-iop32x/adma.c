// SPDX-License-Identifier: GPL-2.0-only
/*
 * platform device definitions for the iop3xx dma/xor engines
 * Copyright © 2006, Intel Corporation.
 */
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/platform_data/dma-iop32x.h>

#include "iop3xx.h"
#include "irqs.h"

#define IRQ_DMA0_EOT IRQ_IOP32X_DMA0_EOT
#define IRQ_DMA0_EOC IRQ_IOP32X_DMA0_EOC
#define IRQ_DMA0_ERR IRQ_IOP32X_DMA0_ERR

#define IRQ_DMA1_EOT IRQ_IOP32X_DMA1_EOT
#define IRQ_DMA1_EOC IRQ_IOP32X_DMA1_EOC
#define IRQ_DMA1_ERR IRQ_IOP32X_DMA1_ERR

#define IRQ_AA_EOT IRQ_IOP32X_AA_EOT
#define IRQ_AA_EOC IRQ_IOP32X_AA_EOC
#define IRQ_AA_ERR IRQ_IOP32X_AA_ERR

/* AAU and DMA Channels */
static struct resource iop3xx_dma_0_resources[] = {
	[0] = {
		.start = IOP3XX_DMA_PHYS_BASE(0),
		.end = IOP3XX_DMA_UPPER_PA(0),
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_DMA0_EOT,
		.end = IRQ_DMA0_EOT,
		.flags = IORESOURCE_IRQ
	},
	[2] = {
		.start = IRQ_DMA0_EOC,
		.end = IRQ_DMA0_EOC,
		.flags = IORESOURCE_IRQ
	},
	[3] = {
		.start = IRQ_DMA0_ERR,
		.end = IRQ_DMA0_ERR,
		.flags = IORESOURCE_IRQ
	}
};

static struct resource iop3xx_dma_1_resources[] = {
	[0] = {
		.start = IOP3XX_DMA_PHYS_BASE(1),
		.end = IOP3XX_DMA_UPPER_PA(1),
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_DMA1_EOT,
		.end = IRQ_DMA1_EOT,
		.flags = IORESOURCE_IRQ
	},
	[2] = {
		.start = IRQ_DMA1_EOC,
		.end = IRQ_DMA1_EOC,
		.flags = IORESOURCE_IRQ
	},
	[3] = {
		.start = IRQ_DMA1_ERR,
		.end = IRQ_DMA1_ERR,
		.flags = IORESOURCE_IRQ
	}
};


static struct resource iop3xx_aau_resources[] = {
	[0] = {
		.start = IOP3XX_AAU_PHYS_BASE,
		.end = IOP3XX_AAU_UPPER_PA,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_AA_EOT,
		.end = IRQ_AA_EOT,
		.flags = IORESOURCE_IRQ
	},
	[2] = {
		.start = IRQ_AA_EOC,
		.end = IRQ_AA_EOC,
		.flags = IORESOURCE_IRQ
	},
	[3] = {
		.start = IRQ_AA_ERR,
		.end = IRQ_AA_ERR,
		.flags = IORESOURCE_IRQ
	}
};

static u64 iop3xx_adma_dmamask = DMA_BIT_MASK(32);

static struct iop_adma_platform_data iop3xx_dma_0_data = {
	.hw_id = DMA0_ID,
	.pool_size = PAGE_SIZE,
};

static struct iop_adma_platform_data iop3xx_dma_1_data = {
	.hw_id = DMA1_ID,
	.pool_size = PAGE_SIZE,
};

static struct iop_adma_platform_data iop3xx_aau_data = {
	.hw_id = AAU_ID,
	.pool_size = 3 * PAGE_SIZE,
};

struct platform_device iop3xx_dma_0_channel = {
	.name = "iop-adma",
	.id = 0,
	.num_resources = 4,
	.resource = iop3xx_dma_0_resources,
	.dev = {
		.dma_mask = &iop3xx_adma_dmamask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
		.platform_data = (void *) &iop3xx_dma_0_data,
	},
};

struct platform_device iop3xx_dma_1_channel = {
	.name = "iop-adma",
	.id = 1,
	.num_resources = 4,
	.resource = iop3xx_dma_1_resources,
	.dev = {
		.dma_mask = &iop3xx_adma_dmamask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
		.platform_data = (void *) &iop3xx_dma_1_data,
	},
};

struct platform_device iop3xx_aau_channel = {
	.name = "iop-adma",
	.id = 2,
	.num_resources = 4,
	.resource = iop3xx_aau_resources,
	.dev = {
		.dma_mask = &iop3xx_adma_dmamask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
		.platform_data = (void *) &iop3xx_aau_data,
	},
};

static int __init iop3xx_adma_cap_init(void)
{
	dma_cap_set(DMA_MEMCPY, iop3xx_dma_0_data.cap_mask);
	dma_cap_set(DMA_INTERRUPT, iop3xx_dma_0_data.cap_mask);

	dma_cap_set(DMA_MEMCPY, iop3xx_dma_1_data.cap_mask);
	dma_cap_set(DMA_INTERRUPT, iop3xx_dma_1_data.cap_mask);

	dma_cap_set(DMA_XOR, iop3xx_aau_data.cap_mask);
	dma_cap_set(DMA_INTERRUPT, iop3xx_aau_data.cap_mask);

	return 0;
}

arch_initcall(iop3xx_adma_cap_init);
