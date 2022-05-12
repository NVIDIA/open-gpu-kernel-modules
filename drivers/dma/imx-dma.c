// SPDX-License-Identifier: GPL-2.0+
//
// drivers/dma/imx-dma.c
//
// This file contains a driver for the Freescale i.MX DMA engine
// found on i.MX1/21/27
//
// Copyright 2010 Sascha Hauer, Pengutronix <s.hauer@pengutronix.de>
// Copyright 2012 Javier Martin, Vista Silicon <javier.martin@vista-silicon.com>

#include <linux/err.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/dmaengine.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/of_dma.h>

#include <asm/irq.h>
#include <linux/platform_data/dma-imx.h>

#include "dmaengine.h"
#define IMXDMA_MAX_CHAN_DESCRIPTORS	16
#define IMX_DMA_CHANNELS  16

#define IMX_DMA_2D_SLOTS	2
#define IMX_DMA_2D_SLOT_A	0
#define IMX_DMA_2D_SLOT_B	1

#define IMX_DMA_LENGTH_LOOP	((unsigned int)-1)
#define IMX_DMA_MEMSIZE_32	(0 << 4)
#define IMX_DMA_MEMSIZE_8	(1 << 4)
#define IMX_DMA_MEMSIZE_16	(2 << 4)
#define IMX_DMA_TYPE_LINEAR	(0 << 10)
#define IMX_DMA_TYPE_2D		(1 << 10)
#define IMX_DMA_TYPE_FIFO	(2 << 10)

#define IMX_DMA_ERR_BURST     (1 << 0)
#define IMX_DMA_ERR_REQUEST   (1 << 1)
#define IMX_DMA_ERR_TRANSFER  (1 << 2)
#define IMX_DMA_ERR_BUFFER    (1 << 3)
#define IMX_DMA_ERR_TIMEOUT   (1 << 4)

#define DMA_DCR     0x00		/* Control Register */
#define DMA_DISR    0x04		/* Interrupt status Register */
#define DMA_DIMR    0x08		/* Interrupt mask Register */
#define DMA_DBTOSR  0x0c		/* Burst timeout status Register */
#define DMA_DRTOSR  0x10		/* Request timeout Register */
#define DMA_DSESR   0x14		/* Transfer Error Status Register */
#define DMA_DBOSR   0x18		/* Buffer overflow status Register */
#define DMA_DBTOCR  0x1c		/* Burst timeout control Register */
#define DMA_WSRA    0x40		/* W-Size Register A */
#define DMA_XSRA    0x44		/* X-Size Register A */
#define DMA_YSRA    0x48		/* Y-Size Register A */
#define DMA_WSRB    0x4c		/* W-Size Register B */
#define DMA_XSRB    0x50		/* X-Size Register B */
#define DMA_YSRB    0x54		/* Y-Size Register B */
#define DMA_SAR(x)  (0x80 + ((x) << 6))	/* Source Address Registers */
#define DMA_DAR(x)  (0x84 + ((x) << 6))	/* Destination Address Registers */
#define DMA_CNTR(x) (0x88 + ((x) << 6))	/* Count Registers */
#define DMA_CCR(x)  (0x8c + ((x) << 6))	/* Control Registers */
#define DMA_RSSR(x) (0x90 + ((x) << 6))	/* Request source select Registers */
#define DMA_BLR(x)  (0x94 + ((x) << 6))	/* Burst length Registers */
#define DMA_RTOR(x) (0x98 + ((x) << 6))	/* Request timeout Registers */
#define DMA_BUCR(x) (0x98 + ((x) << 6))	/* Bus Utilization Registers */
#define DMA_CCNR(x) (0x9C + ((x) << 6))	/* Channel counter Registers */

#define DCR_DRST           (1<<1)
#define DCR_DEN            (1<<0)
#define DBTOCR_EN          (1<<15)
#define DBTOCR_CNT(x)      ((x) & 0x7fff)
#define CNTR_CNT(x)        ((x) & 0xffffff)
#define CCR_ACRPT          (1<<14)
#define CCR_DMOD_LINEAR    (0x0 << 12)
#define CCR_DMOD_2D        (0x1 << 12)
#define CCR_DMOD_FIFO      (0x2 << 12)
#define CCR_DMOD_EOBFIFO   (0x3 << 12)
#define CCR_SMOD_LINEAR    (0x0 << 10)
#define CCR_SMOD_2D        (0x1 << 10)
#define CCR_SMOD_FIFO      (0x2 << 10)
#define CCR_SMOD_EOBFIFO   (0x3 << 10)
#define CCR_MDIR_DEC       (1<<9)
#define CCR_MSEL_B         (1<<8)
#define CCR_DSIZ_32        (0x0 << 6)
#define CCR_DSIZ_8         (0x1 << 6)
#define CCR_DSIZ_16        (0x2 << 6)
#define CCR_SSIZ_32        (0x0 << 4)
#define CCR_SSIZ_8         (0x1 << 4)
#define CCR_SSIZ_16        (0x2 << 4)
#define CCR_REN            (1<<3)
#define CCR_RPT            (1<<2)
#define CCR_FRC            (1<<1)
#define CCR_CEN            (1<<0)
#define RTOR_EN            (1<<15)
#define RTOR_CLK           (1<<14)
#define RTOR_PSC           (1<<13)

enum  imxdma_prep_type {
	IMXDMA_DESC_MEMCPY,
	IMXDMA_DESC_INTERLEAVED,
	IMXDMA_DESC_SLAVE_SG,
	IMXDMA_DESC_CYCLIC,
};

struct imx_dma_2d_config {
	u16		xsr;
	u16		ysr;
	u16		wsr;
	int		count;
};

struct imxdma_desc {
	struct list_head		node;
	struct dma_async_tx_descriptor	desc;
	enum dma_status			status;
	dma_addr_t			src;
	dma_addr_t			dest;
	size_t				len;
	enum dma_transfer_direction	direction;
	enum imxdma_prep_type		type;
	/* For memcpy and interleaved */
	unsigned int			config_port;
	unsigned int			config_mem;
	/* For interleaved transfers */
	unsigned int			x;
	unsigned int			y;
	unsigned int			w;
	/* For slave sg and cyclic */
	struct scatterlist		*sg;
	unsigned int			sgcount;
};

struct imxdma_channel {
	int				hw_chaining;
	struct timer_list		watchdog;
	struct imxdma_engine		*imxdma;
	unsigned int			channel;

	struct tasklet_struct		dma_tasklet;
	struct list_head		ld_free;
	struct list_head		ld_queue;
	struct list_head		ld_active;
	int				descs_allocated;
	enum dma_slave_buswidth		word_size;
	dma_addr_t			per_address;
	u32				watermark_level;
	struct dma_chan			chan;
	struct dma_async_tx_descriptor	desc;
	enum dma_status			status;
	int				dma_request;
	struct scatterlist		*sg_list;
	u32				ccr_from_device;
	u32				ccr_to_device;
	bool				enabled_2d;
	int				slot_2d;
	unsigned int			irq;
	struct dma_slave_config		config;
};

enum imx_dma_type {
	IMX1_DMA,
	IMX21_DMA,
	IMX27_DMA,
};

struct imxdma_engine {
	struct device			*dev;
	struct dma_device		dma_device;
	void __iomem			*base;
	struct clk			*dma_ahb;
	struct clk			*dma_ipg;
	spinlock_t			lock;
	struct imx_dma_2d_config	slots_2d[IMX_DMA_2D_SLOTS];
	struct imxdma_channel		channel[IMX_DMA_CHANNELS];
	enum imx_dma_type		devtype;
	unsigned int			irq;
	unsigned int			irq_err;

};

struct imxdma_filter_data {
	struct imxdma_engine	*imxdma;
	int			 request;
};

static const struct of_device_id imx_dma_of_dev_id[] = {
	{
		.compatible = "fsl,imx1-dma", .data = (const void *)IMX1_DMA,
	}, {
		.compatible = "fsl,imx21-dma", .data = (const void *)IMX21_DMA,
	}, {
		.compatible = "fsl,imx27-dma", .data = (const void *)IMX27_DMA,
	}, {
		/* sentinel */
	}
};
MODULE_DEVICE_TABLE(of, imx_dma_of_dev_id);

static inline int is_imx1_dma(struct imxdma_engine *imxdma)
{
	return imxdma->devtype == IMX1_DMA;
}

static inline int is_imx27_dma(struct imxdma_engine *imxdma)
{
	return imxdma->devtype == IMX27_DMA;
}

static struct imxdma_channel *to_imxdma_chan(struct dma_chan *chan)
{
	return container_of(chan, struct imxdma_channel, chan);
}

static inline bool imxdma_chan_is_doing_cyclic(struct imxdma_channel *imxdmac)
{
	struct imxdma_desc *desc;

	if (!list_empty(&imxdmac->ld_active)) {
		desc = list_first_entry(&imxdmac->ld_active, struct imxdma_desc,
					node);
		if (desc->type == IMXDMA_DESC_CYCLIC)
			return true;
	}
	return false;
}



static void imx_dmav1_writel(struct imxdma_engine *imxdma, unsigned val,
			     unsigned offset)
{
	__raw_writel(val, imxdma->base + offset);
}

static unsigned imx_dmav1_readl(struct imxdma_engine *imxdma, unsigned offset)
{
	return __raw_readl(imxdma->base + offset);
}

static int imxdma_hw_chain(struct imxdma_channel *imxdmac)
{
	struct imxdma_engine *imxdma = imxdmac->imxdma;

	if (is_imx27_dma(imxdma))
		return imxdmac->hw_chaining;
	else
		return 0;
}

/*
 * imxdma_sg_next - prepare next chunk for scatter-gather DMA emulation
 */
static inline void imxdma_sg_next(struct imxdma_desc *d)
{
	struct imxdma_channel *imxdmac = to_imxdma_chan(d->desc.chan);
	struct imxdma_engine *imxdma = imxdmac->imxdma;
	struct scatterlist *sg = d->sg;
	size_t now;

	now = min_t(size_t, d->len, sg_dma_len(sg));
	if (d->len != IMX_DMA_LENGTH_LOOP)
		d->len -= now;

	if (d->direction == DMA_DEV_TO_MEM)
		imx_dmav1_writel(imxdma, sg->dma_address,
				 DMA_DAR(imxdmac->channel));
	else
		imx_dmav1_writel(imxdma, sg->dma_address,
				 DMA_SAR(imxdmac->channel));

	imx_dmav1_writel(imxdma, now, DMA_CNTR(imxdmac->channel));

	dev_dbg(imxdma->dev, " %s channel: %d dst 0x%08x, src 0x%08x, "
		"size 0x%08x\n", __func__, imxdmac->channel,
		 imx_dmav1_readl(imxdma, DMA_DAR(imxdmac->channel)),
		 imx_dmav1_readl(imxdma, DMA_SAR(imxdmac->channel)),
		 imx_dmav1_readl(imxdma, DMA_CNTR(imxdmac->channel)));
}

static void imxdma_enable_hw(struct imxdma_desc *d)
{
	struct imxdma_channel *imxdmac = to_imxdma_chan(d->desc.chan);
	struct imxdma_engine *imxdma = imxdmac->imxdma;
	int channel = imxdmac->channel;
	unsigned long flags;

	dev_dbg(imxdma->dev, "%s channel %d\n", __func__, channel);

	local_irq_save(flags);

	imx_dmav1_writel(imxdma, 1 << channel, DMA_DISR);
	imx_dmav1_writel(imxdma, imx_dmav1_readl(imxdma, DMA_DIMR) &
			 ~(1 << channel), DMA_DIMR);
	imx_dmav1_writel(imxdma, imx_dmav1_readl(imxdma, DMA_CCR(channel)) |
			 CCR_CEN | CCR_ACRPT, DMA_CCR(channel));

	if (!is_imx1_dma(imxdma) &&
			d->sg && imxdma_hw_chain(imxdmac)) {
		d->sg = sg_next(d->sg);
		if (d->sg) {
			u32 tmp;
			imxdma_sg_next(d);
			tmp = imx_dmav1_readl(imxdma, DMA_CCR(channel));
			imx_dmav1_writel(imxdma, tmp | CCR_RPT | CCR_ACRPT,
					 DMA_CCR(channel));
		}
	}

	local_irq_restore(flags);
}

static void imxdma_disable_hw(struct imxdma_channel *imxdmac)
{
	struct imxdma_engine *imxdma = imxdmac->imxdma;
	int channel = imxdmac->channel;
	unsigned long flags;

	dev_dbg(imxdma->dev, "%s channel %d\n", __func__, channel);

	if (imxdma_hw_chain(imxdmac))
		del_timer(&imxdmac->watchdog);

	local_irq_save(flags);
	imx_dmav1_writel(imxdma, imx_dmav1_readl(imxdma, DMA_DIMR) |
			 (1 << channel), DMA_DIMR);
	imx_dmav1_writel(imxdma, imx_dmav1_readl(imxdma, DMA_CCR(channel)) &
			 ~CCR_CEN, DMA_CCR(channel));
	imx_dmav1_writel(imxdma, 1 << channel, DMA_DISR);
	local_irq_restore(flags);
}

static void imxdma_watchdog(struct timer_list *t)
{
	struct imxdma_channel *imxdmac = from_timer(imxdmac, t, watchdog);
	struct imxdma_engine *imxdma = imxdmac->imxdma;
	int channel = imxdmac->channel;

	imx_dmav1_writel(imxdma, 0, DMA_CCR(channel));

	/* Tasklet watchdog error handler */
	tasklet_schedule(&imxdmac->dma_tasklet);
	dev_dbg(imxdma->dev, "channel %d: watchdog timeout!\n",
		imxdmac->channel);
}

static irqreturn_t imxdma_err_handler(int irq, void *dev_id)
{
	struct imxdma_engine *imxdma = dev_id;
	unsigned int err_mask;
	int i, disr;
	int errcode;

	disr = imx_dmav1_readl(imxdma, DMA_DISR);

	err_mask = imx_dmav1_readl(imxdma, DMA_DBTOSR) |
		   imx_dmav1_readl(imxdma, DMA_DRTOSR) |
		   imx_dmav1_readl(imxdma, DMA_DSESR)  |
		   imx_dmav1_readl(imxdma, DMA_DBOSR);

	if (!err_mask)
		return IRQ_HANDLED;

	imx_dmav1_writel(imxdma, disr & err_mask, DMA_DISR);

	for (i = 0; i < IMX_DMA_CHANNELS; i++) {
		if (!(err_mask & (1 << i)))
			continue;
		errcode = 0;

		if (imx_dmav1_readl(imxdma, DMA_DBTOSR) & (1 << i)) {
			imx_dmav1_writel(imxdma, 1 << i, DMA_DBTOSR);
			errcode |= IMX_DMA_ERR_BURST;
		}
		if (imx_dmav1_readl(imxdma, DMA_DRTOSR) & (1 << i)) {
			imx_dmav1_writel(imxdma, 1 << i, DMA_DRTOSR);
			errcode |= IMX_DMA_ERR_REQUEST;
		}
		if (imx_dmav1_readl(imxdma, DMA_DSESR) & (1 << i)) {
			imx_dmav1_writel(imxdma, 1 << i, DMA_DSESR);
			errcode |= IMX_DMA_ERR_TRANSFER;
		}
		if (imx_dmav1_readl(imxdma, DMA_DBOSR) & (1 << i)) {
			imx_dmav1_writel(imxdma, 1 << i, DMA_DBOSR);
			errcode |= IMX_DMA_ERR_BUFFER;
		}
		/* Tasklet error handler */
		tasklet_schedule(&imxdma->channel[i].dma_tasklet);

		dev_warn(imxdma->dev,
			 "DMA timeout on channel %d -%s%s%s%s\n", i,
			 errcode & IMX_DMA_ERR_BURST ?    " burst" : "",
			 errcode & IMX_DMA_ERR_REQUEST ?  " request" : "",
			 errcode & IMX_DMA_ERR_TRANSFER ? " transfer" : "",
			 errcode & IMX_DMA_ERR_BUFFER ?   " buffer" : "");
	}
	return IRQ_HANDLED;
}

static void dma_irq_handle_channel(struct imxdma_channel *imxdmac)
{
	struct imxdma_engine *imxdma = imxdmac->imxdma;
	int chno = imxdmac->channel;
	struct imxdma_desc *desc;
	unsigned long flags;

	spin_lock_irqsave(&imxdma->lock, flags);
	if (list_empty(&imxdmac->ld_active)) {
		spin_unlock_irqrestore(&imxdma->lock, flags);
		goto out;
	}

	desc = list_first_entry(&imxdmac->ld_active,
				struct imxdma_desc,
				node);
	spin_unlock_irqrestore(&imxdma->lock, flags);

	if (desc->sg) {
		u32 tmp;
		desc->sg = sg_next(desc->sg);

		if (desc->sg) {
			imxdma_sg_next(desc);

			tmp = imx_dmav1_readl(imxdma, DMA_CCR(chno));

			if (imxdma_hw_chain(imxdmac)) {
				/* FIXME: The timeout should probably be
				 * configurable
				 */
				mod_timer(&imxdmac->watchdog,
					jiffies + msecs_to_jiffies(500));

				tmp |= CCR_CEN | CCR_RPT | CCR_ACRPT;
				imx_dmav1_writel(imxdma, tmp, DMA_CCR(chno));
			} else {
				imx_dmav1_writel(imxdma, tmp & ~CCR_CEN,
						 DMA_CCR(chno));
				tmp |= CCR_CEN;
			}

			imx_dmav1_writel(imxdma, tmp, DMA_CCR(chno));

			if (imxdma_chan_is_doing_cyclic(imxdmac))
				/* Tasklet progression */
				tasklet_schedule(&imxdmac->dma_tasklet);

			return;
		}

		if (imxdma_hw_chain(imxdmac)) {
			del_timer(&imxdmac->watchdog);
			return;
		}
	}

out:
	imx_dmav1_writel(imxdma, 0, DMA_CCR(chno));
	/* Tasklet irq */
	tasklet_schedule(&imxdmac->dma_tasklet);
}

static irqreturn_t dma_irq_handler(int irq, void *dev_id)
{
	struct imxdma_engine *imxdma = dev_id;
	int i, disr;

	if (!is_imx1_dma(imxdma))
		imxdma_err_handler(irq, dev_id);

	disr = imx_dmav1_readl(imxdma, DMA_DISR);

	dev_dbg(imxdma->dev, "%s called, disr=0x%08x\n", __func__, disr);

	imx_dmav1_writel(imxdma, disr, DMA_DISR);
	for (i = 0; i < IMX_DMA_CHANNELS; i++) {
		if (disr & (1 << i))
			dma_irq_handle_channel(&imxdma->channel[i]);
	}

	return IRQ_HANDLED;
}

static int imxdma_xfer_desc(struct imxdma_desc *d)
{
	struct imxdma_channel *imxdmac = to_imxdma_chan(d->desc.chan);
	struct imxdma_engine *imxdma = imxdmac->imxdma;
	int slot = -1;
	int i;

	/* Configure and enable */
	switch (d->type) {
	case IMXDMA_DESC_INTERLEAVED:
		/* Try to get a free 2D slot */
		for (i = 0; i < IMX_DMA_2D_SLOTS; i++) {
			if ((imxdma->slots_2d[i].count > 0) &&
			((imxdma->slots_2d[i].xsr != d->x) ||
			(imxdma->slots_2d[i].ysr != d->y) ||
			(imxdma->slots_2d[i].wsr != d->w)))
				continue;
			slot = i;
			break;
		}
		if (slot < 0)
			return -EBUSY;

		imxdma->slots_2d[slot].xsr = d->x;
		imxdma->slots_2d[slot].ysr = d->y;
		imxdma->slots_2d[slot].wsr = d->w;
		imxdma->slots_2d[slot].count++;

		imxdmac->slot_2d = slot;
		imxdmac->enabled_2d = true;

		if (slot == IMX_DMA_2D_SLOT_A) {
			d->config_mem &= ~CCR_MSEL_B;
			d->config_port &= ~CCR_MSEL_B;
			imx_dmav1_writel(imxdma, d->x, DMA_XSRA);
			imx_dmav1_writel(imxdma, d->y, DMA_YSRA);
			imx_dmav1_writel(imxdma, d->w, DMA_WSRA);
		} else {
			d->config_mem |= CCR_MSEL_B;
			d->config_port |= CCR_MSEL_B;
			imx_dmav1_writel(imxdma, d->x, DMA_XSRB);
			imx_dmav1_writel(imxdma, d->y, DMA_YSRB);
			imx_dmav1_writel(imxdma, d->w, DMA_WSRB);
		}
		/*
		 * We fall-through here intentionally, since a 2D transfer is
		 * similar to MEMCPY just adding the 2D slot configuration.
		 */
		fallthrough;
	case IMXDMA_DESC_MEMCPY:
		imx_dmav1_writel(imxdma, d->src, DMA_SAR(imxdmac->channel));
		imx_dmav1_writel(imxdma, d->dest, DMA_DAR(imxdmac->channel));
		imx_dmav1_writel(imxdma, d->config_mem | (d->config_port << 2),
			 DMA_CCR(imxdmac->channel));

		imx_dmav1_writel(imxdma, d->len, DMA_CNTR(imxdmac->channel));

		dev_dbg(imxdma->dev,
			"%s channel: %d dest=0x%08llx src=0x%08llx dma_length=%zu\n",
			__func__, imxdmac->channel,
			(unsigned long long)d->dest,
			(unsigned long long)d->src, d->len);

		break;
	/* Cyclic transfer is the same as slave_sg with special sg configuration. */
	case IMXDMA_DESC_CYCLIC:
	case IMXDMA_DESC_SLAVE_SG:
		if (d->direction == DMA_DEV_TO_MEM) {
			imx_dmav1_writel(imxdma, imxdmac->per_address,
					 DMA_SAR(imxdmac->channel));
			imx_dmav1_writel(imxdma, imxdmac->ccr_from_device,
					 DMA_CCR(imxdmac->channel));

			dev_dbg(imxdma->dev,
				"%s channel: %d sg=%p sgcount=%d total length=%zu dev_addr=0x%08llx (dev2mem)\n",
				__func__, imxdmac->channel,
				d->sg, d->sgcount, d->len,
				(unsigned long long)imxdmac->per_address);
		} else if (d->direction == DMA_MEM_TO_DEV) {
			imx_dmav1_writel(imxdma, imxdmac->per_address,
					 DMA_DAR(imxdmac->channel));
			imx_dmav1_writel(imxdma, imxdmac->ccr_to_device,
					 DMA_CCR(imxdmac->channel));

			dev_dbg(imxdma->dev,
				"%s channel: %d sg=%p sgcount=%d total length=%zu dev_addr=0x%08llx (mem2dev)\n",
				__func__, imxdmac->channel,
				d->sg, d->sgcount, d->len,
				(unsigned long long)imxdmac->per_address);
		} else {
			dev_err(imxdma->dev, "%s channel: %d bad dma mode\n",
				__func__, imxdmac->channel);
			return -EINVAL;
		}

		imxdma_sg_next(d);

		break;
	default:
		return -EINVAL;
	}
	imxdma_enable_hw(d);
	return 0;
}

static void imxdma_tasklet(struct tasklet_struct *t)
{
	struct imxdma_channel *imxdmac = from_tasklet(imxdmac, t, dma_tasklet);
	struct imxdma_engine *imxdma = imxdmac->imxdma;
	struct imxdma_desc *desc, *next_desc;
	unsigned long flags;

	spin_lock_irqsave(&imxdma->lock, flags);

	if (list_empty(&imxdmac->ld_active)) {
		/* Someone might have called terminate all */
		spin_unlock_irqrestore(&imxdma->lock, flags);
		return;
	}
	desc = list_first_entry(&imxdmac->ld_active, struct imxdma_desc, node);

	/* If we are dealing with a cyclic descriptor, keep it on ld_active
	 * and dont mark the descriptor as complete.
	 * Only in non-cyclic cases it would be marked as complete
	 */
	if (imxdma_chan_is_doing_cyclic(imxdmac))
		goto out;
	else
		dma_cookie_complete(&desc->desc);

	/* Free 2D slot if it was an interleaved transfer */
	if (imxdmac->enabled_2d) {
		imxdma->slots_2d[imxdmac->slot_2d].count--;
		imxdmac->enabled_2d = false;
	}

	list_move_tail(imxdmac->ld_active.next, &imxdmac->ld_free);

	if (!list_empty(&imxdmac->ld_queue)) {
		next_desc = list_first_entry(&imxdmac->ld_queue,
					     struct imxdma_desc, node);
		list_move_tail(imxdmac->ld_queue.next, &imxdmac->ld_active);
		if (imxdma_xfer_desc(next_desc) < 0)
			dev_warn(imxdma->dev, "%s: channel: %d couldn't xfer desc\n",
				 __func__, imxdmac->channel);
	}
out:
	spin_unlock_irqrestore(&imxdma->lock, flags);

	dmaengine_desc_get_callback_invoke(&desc->desc, NULL);
}

static int imxdma_terminate_all(struct dma_chan *chan)
{
	struct imxdma_channel *imxdmac = to_imxdma_chan(chan);
	struct imxdma_engine *imxdma = imxdmac->imxdma;
	unsigned long flags;

	imxdma_disable_hw(imxdmac);

	spin_lock_irqsave(&imxdma->lock, flags);
	list_splice_tail_init(&imxdmac->ld_active, &imxdmac->ld_free);
	list_splice_tail_init(&imxdmac->ld_queue, &imxdmac->ld_free);
	spin_unlock_irqrestore(&imxdma->lock, flags);
	return 0;
}

static int imxdma_config_write(struct dma_chan *chan,
			       struct dma_slave_config *dmaengine_cfg,
			       enum dma_transfer_direction direction)
{
	struct imxdma_channel *imxdmac = to_imxdma_chan(chan);
	struct imxdma_engine *imxdma = imxdmac->imxdma;
	unsigned int mode = 0;

	if (direction == DMA_DEV_TO_MEM) {
		imxdmac->per_address = dmaengine_cfg->src_addr;
		imxdmac->watermark_level = dmaengine_cfg->src_maxburst;
		imxdmac->word_size = dmaengine_cfg->src_addr_width;
	} else {
		imxdmac->per_address = dmaengine_cfg->dst_addr;
		imxdmac->watermark_level = dmaengine_cfg->dst_maxburst;
		imxdmac->word_size = dmaengine_cfg->dst_addr_width;
	}

	switch (imxdmac->word_size) {
	case DMA_SLAVE_BUSWIDTH_1_BYTE:
		mode = IMX_DMA_MEMSIZE_8;
		break;
	case DMA_SLAVE_BUSWIDTH_2_BYTES:
		mode = IMX_DMA_MEMSIZE_16;
		break;
	default:
	case DMA_SLAVE_BUSWIDTH_4_BYTES:
		mode = IMX_DMA_MEMSIZE_32;
		break;
	}

	imxdmac->hw_chaining = 0;

	imxdmac->ccr_from_device = (mode | IMX_DMA_TYPE_FIFO) |
		((IMX_DMA_MEMSIZE_32 | IMX_DMA_TYPE_LINEAR) << 2) |
		CCR_REN;
	imxdmac->ccr_to_device =
		(IMX_DMA_MEMSIZE_32 | IMX_DMA_TYPE_LINEAR) |
		((mode | IMX_DMA_TYPE_FIFO) << 2) | CCR_REN;
	imx_dmav1_writel(imxdma, imxdmac->dma_request,
			 DMA_RSSR(imxdmac->channel));

	/* Set burst length */
	imx_dmav1_writel(imxdma, imxdmac->watermark_level *
			 imxdmac->word_size, DMA_BLR(imxdmac->channel));

	return 0;
}

static int imxdma_config(struct dma_chan *chan,
			 struct dma_slave_config *dmaengine_cfg)
{
	struct imxdma_channel *imxdmac = to_imxdma_chan(chan);

	memcpy(&imxdmac->config, dmaengine_cfg, sizeof(*dmaengine_cfg));

	return 0;
}

static enum dma_status imxdma_tx_status(struct dma_chan *chan,
					    dma_cookie_t cookie,
					    struct dma_tx_state *txstate)
{
	return dma_cookie_status(chan, cookie, txstate);
}

static dma_cookie_t imxdma_tx_submit(struct dma_async_tx_descriptor *tx)
{
	struct imxdma_channel *imxdmac = to_imxdma_chan(tx->chan);
	struct imxdma_engine *imxdma = imxdmac->imxdma;
	dma_cookie_t cookie;
	unsigned long flags;

	spin_lock_irqsave(&imxdma->lock, flags);
	list_move_tail(imxdmac->ld_free.next, &imxdmac->ld_queue);
	cookie = dma_cookie_assign(tx);
	spin_unlock_irqrestore(&imxdma->lock, flags);

	return cookie;
}

static int imxdma_alloc_chan_resources(struct dma_chan *chan)
{
	struct imxdma_channel *imxdmac = to_imxdma_chan(chan);
	struct imx_dma_data *data = chan->private;

	if (data != NULL)
		imxdmac->dma_request = data->dma_request;

	while (imxdmac->descs_allocated < IMXDMA_MAX_CHAN_DESCRIPTORS) {
		struct imxdma_desc *desc;

		desc = kzalloc(sizeof(*desc), GFP_KERNEL);
		if (!desc)
			break;
		memset(&desc->desc, 0, sizeof(struct dma_async_tx_descriptor));
		dma_async_tx_descriptor_init(&desc->desc, chan);
		desc->desc.tx_submit = imxdma_tx_submit;
		/* txd.flags will be overwritten in prep funcs */
		desc->desc.flags = DMA_CTRL_ACK;
		desc->status = DMA_COMPLETE;

		list_add_tail(&desc->node, &imxdmac->ld_free);
		imxdmac->descs_allocated++;
	}

	if (!imxdmac->descs_allocated)
		return -ENOMEM;

	return imxdmac->descs_allocated;
}

static void imxdma_free_chan_resources(struct dma_chan *chan)
{
	struct imxdma_channel *imxdmac = to_imxdma_chan(chan);
	struct imxdma_engine *imxdma = imxdmac->imxdma;
	struct imxdma_desc *desc, *_desc;
	unsigned long flags;

	spin_lock_irqsave(&imxdma->lock, flags);

	imxdma_disable_hw(imxdmac);
	list_splice_tail_init(&imxdmac->ld_active, &imxdmac->ld_free);
	list_splice_tail_init(&imxdmac->ld_queue, &imxdmac->ld_free);

	spin_unlock_irqrestore(&imxdma->lock, flags);

	list_for_each_entry_safe(desc, _desc, &imxdmac->ld_free, node) {
		kfree(desc);
		imxdmac->descs_allocated--;
	}
	INIT_LIST_HEAD(&imxdmac->ld_free);

	kfree(imxdmac->sg_list);
	imxdmac->sg_list = NULL;
}

static struct dma_async_tx_descriptor *imxdma_prep_slave_sg(
		struct dma_chan *chan, struct scatterlist *sgl,
		unsigned int sg_len, enum dma_transfer_direction direction,
		unsigned long flags, void *context)
{
	struct imxdma_channel *imxdmac = to_imxdma_chan(chan);
	struct scatterlist *sg;
	int i, dma_length = 0;
	struct imxdma_desc *desc;

	if (list_empty(&imxdmac->ld_free) ||
	    imxdma_chan_is_doing_cyclic(imxdmac))
		return NULL;

	desc = list_first_entry(&imxdmac->ld_free, struct imxdma_desc, node);

	for_each_sg(sgl, sg, sg_len, i) {
		dma_length += sg_dma_len(sg);
	}

	switch (imxdmac->word_size) {
	case DMA_SLAVE_BUSWIDTH_4_BYTES:
		if (sg_dma_len(sgl) & 3 || sgl->dma_address & 3)
			return NULL;
		break;
	case DMA_SLAVE_BUSWIDTH_2_BYTES:
		if (sg_dma_len(sgl) & 1 || sgl->dma_address & 1)
			return NULL;
		break;
	case DMA_SLAVE_BUSWIDTH_1_BYTE:
		break;
	default:
		return NULL;
	}

	desc->type = IMXDMA_DESC_SLAVE_SG;
	desc->sg = sgl;
	desc->sgcount = sg_len;
	desc->len = dma_length;
	desc->direction = direction;
	if (direction == DMA_DEV_TO_MEM) {
		desc->src = imxdmac->per_address;
	} else {
		desc->dest = imxdmac->per_address;
	}
	desc->desc.callback = NULL;
	desc->desc.callback_param = NULL;

	return &desc->desc;
}

static struct dma_async_tx_descriptor *imxdma_prep_dma_cyclic(
		struct dma_chan *chan, dma_addr_t dma_addr, size_t buf_len,
		size_t period_len, enum dma_transfer_direction direction,
		unsigned long flags)
{
	struct imxdma_channel *imxdmac = to_imxdma_chan(chan);
	struct imxdma_engine *imxdma = imxdmac->imxdma;
	struct imxdma_desc *desc;
	int i;
	unsigned int periods = buf_len / period_len;

	dev_dbg(imxdma->dev, "%s channel: %d buf_len=%zu period_len=%zu\n",
			__func__, imxdmac->channel, buf_len, period_len);

	if (list_empty(&imxdmac->ld_free) ||
	    imxdma_chan_is_doing_cyclic(imxdmac))
		return NULL;

	desc = list_first_entry(&imxdmac->ld_free, struct imxdma_desc, node);

	kfree(imxdmac->sg_list);

	imxdmac->sg_list = kcalloc(periods + 1,
			sizeof(struct scatterlist), GFP_ATOMIC);
	if (!imxdmac->sg_list)
		return NULL;

	sg_init_table(imxdmac->sg_list, periods);

	for (i = 0; i < periods; i++) {
		sg_assign_page(&imxdmac->sg_list[i], NULL);
		imxdmac->sg_list[i].offset = 0;
		imxdmac->sg_list[i].dma_address = dma_addr;
		sg_dma_len(&imxdmac->sg_list[i]) = period_len;
		dma_addr += period_len;
	}

	/* close the loop */
	sg_chain(imxdmac->sg_list, periods + 1, imxdmac->sg_list);

	desc->type = IMXDMA_DESC_CYCLIC;
	desc->sg = imxdmac->sg_list;
	desc->sgcount = periods;
	desc->len = IMX_DMA_LENGTH_LOOP;
	desc->direction = direction;
	if (direction == DMA_DEV_TO_MEM) {
		desc->src = imxdmac->per_address;
	} else {
		desc->dest = imxdmac->per_address;
	}
	desc->desc.callback = NULL;
	desc->desc.callback_param = NULL;

	imxdma_config_write(chan, &imxdmac->config, direction);

	return &desc->desc;
}

static struct dma_async_tx_descriptor *imxdma_prep_dma_memcpy(
	struct dma_chan *chan, dma_addr_t dest,
	dma_addr_t src, size_t len, unsigned long flags)
{
	struct imxdma_channel *imxdmac = to_imxdma_chan(chan);
	struct imxdma_engine *imxdma = imxdmac->imxdma;
	struct imxdma_desc *desc;

	dev_dbg(imxdma->dev, "%s channel: %d src=0x%llx dst=0x%llx len=%zu\n",
		__func__, imxdmac->channel, (unsigned long long)src,
		(unsigned long long)dest, len);

	if (list_empty(&imxdmac->ld_free) ||
	    imxdma_chan_is_doing_cyclic(imxdmac))
		return NULL;

	desc = list_first_entry(&imxdmac->ld_free, struct imxdma_desc, node);

	desc->type = IMXDMA_DESC_MEMCPY;
	desc->src = src;
	desc->dest = dest;
	desc->len = len;
	desc->direction = DMA_MEM_TO_MEM;
	desc->config_port = IMX_DMA_MEMSIZE_32 | IMX_DMA_TYPE_LINEAR;
	desc->config_mem = IMX_DMA_MEMSIZE_32 | IMX_DMA_TYPE_LINEAR;
	desc->desc.callback = NULL;
	desc->desc.callback_param = NULL;

	return &desc->desc;
}

static struct dma_async_tx_descriptor *imxdma_prep_dma_interleaved(
	struct dma_chan *chan, struct dma_interleaved_template *xt,
	unsigned long flags)
{
	struct imxdma_channel *imxdmac = to_imxdma_chan(chan);
	struct imxdma_engine *imxdma = imxdmac->imxdma;
	struct imxdma_desc *desc;

	dev_dbg(imxdma->dev, "%s channel: %d src_start=0x%llx dst_start=0x%llx\n"
		"   src_sgl=%s dst_sgl=%s numf=%zu frame_size=%zu\n", __func__,
		imxdmac->channel, (unsigned long long)xt->src_start,
		(unsigned long long) xt->dst_start,
		xt->src_sgl ? "true" : "false", xt->dst_sgl ? "true" : "false",
		xt->numf, xt->frame_size);

	if (list_empty(&imxdmac->ld_free) ||
	    imxdma_chan_is_doing_cyclic(imxdmac))
		return NULL;

	if (xt->frame_size != 1 || xt->numf <= 0 || xt->dir != DMA_MEM_TO_MEM)
		return NULL;

	desc = list_first_entry(&imxdmac->ld_free, struct imxdma_desc, node);

	desc->type = IMXDMA_DESC_INTERLEAVED;
	desc->src = xt->src_start;
	desc->dest = xt->dst_start;
	desc->x = xt->sgl[0].size;
	desc->y = xt->numf;
	desc->w = xt->sgl[0].icg + desc->x;
	desc->len = desc->x * desc->y;
	desc->direction = DMA_MEM_TO_MEM;
	desc->config_port = IMX_DMA_MEMSIZE_32;
	desc->config_mem = IMX_DMA_MEMSIZE_32;
	if (xt->src_sgl)
		desc->config_mem |= IMX_DMA_TYPE_2D;
	if (xt->dst_sgl)
		desc->config_port |= IMX_DMA_TYPE_2D;
	desc->desc.callback = NULL;
	desc->desc.callback_param = NULL;

	return &desc->desc;
}

static void imxdma_issue_pending(struct dma_chan *chan)
{
	struct imxdma_channel *imxdmac = to_imxdma_chan(chan);
	struct imxdma_engine *imxdma = imxdmac->imxdma;
	struct imxdma_desc *desc;
	unsigned long flags;

	spin_lock_irqsave(&imxdma->lock, flags);
	if (list_empty(&imxdmac->ld_active) &&
	    !list_empty(&imxdmac->ld_queue)) {
		desc = list_first_entry(&imxdmac->ld_queue,
					struct imxdma_desc, node);

		if (imxdma_xfer_desc(desc) < 0) {
			dev_warn(imxdma->dev,
				 "%s: channel: %d couldn't issue DMA xfer\n",
				 __func__, imxdmac->channel);
		} else {
			list_move_tail(imxdmac->ld_queue.next,
				       &imxdmac->ld_active);
		}
	}
	spin_unlock_irqrestore(&imxdma->lock, flags);
}

static bool imxdma_filter_fn(struct dma_chan *chan, void *param)
{
	struct imxdma_filter_data *fdata = param;
	struct imxdma_channel *imxdma_chan = to_imxdma_chan(chan);

	if (chan->device->dev != fdata->imxdma->dev)
		return false;

	imxdma_chan->dma_request = fdata->request;
	chan->private = NULL;

	return true;
}

static struct dma_chan *imxdma_xlate(struct of_phandle_args *dma_spec,
						struct of_dma *ofdma)
{
	int count = dma_spec->args_count;
	struct imxdma_engine *imxdma = ofdma->of_dma_data;
	struct imxdma_filter_data fdata = {
		.imxdma = imxdma,
	};

	if (count != 1)
		return NULL;

	fdata.request = dma_spec->args[0];

	return dma_request_channel(imxdma->dma_device.cap_mask,
					imxdma_filter_fn, &fdata);
}

static int __init imxdma_probe(struct platform_device *pdev)
{
	struct imxdma_engine *imxdma;
	struct resource *res;
	int ret, i;
	int irq, irq_err;

	imxdma = devm_kzalloc(&pdev->dev, sizeof(*imxdma), GFP_KERNEL);
	if (!imxdma)
		return -ENOMEM;

	imxdma->dev = &pdev->dev;
	imxdma->devtype = (enum imx_dma_type)of_device_get_match_data(&pdev->dev);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	imxdma->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(imxdma->base))
		return PTR_ERR(imxdma->base);

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	imxdma->dma_ipg = devm_clk_get(&pdev->dev, "ipg");
	if (IS_ERR(imxdma->dma_ipg))
		return PTR_ERR(imxdma->dma_ipg);

	imxdma->dma_ahb = devm_clk_get(&pdev->dev, "ahb");
	if (IS_ERR(imxdma->dma_ahb))
		return PTR_ERR(imxdma->dma_ahb);

	ret = clk_prepare_enable(imxdma->dma_ipg);
	if (ret)
		return ret;
	ret = clk_prepare_enable(imxdma->dma_ahb);
	if (ret)
		goto disable_dma_ipg_clk;

	/* reset DMA module */
	imx_dmav1_writel(imxdma, DCR_DRST, DMA_DCR);

	if (is_imx1_dma(imxdma)) {
		ret = devm_request_irq(&pdev->dev, irq,
				       dma_irq_handler, 0, "DMA", imxdma);
		if (ret) {
			dev_warn(imxdma->dev, "Can't register IRQ for DMA\n");
			goto disable_dma_ahb_clk;
		}
		imxdma->irq = irq;

		irq_err = platform_get_irq(pdev, 1);
		if (irq_err < 0) {
			ret = irq_err;
			goto disable_dma_ahb_clk;
		}

		ret = devm_request_irq(&pdev->dev, irq_err,
				       imxdma_err_handler, 0, "DMA", imxdma);
		if (ret) {
			dev_warn(imxdma->dev, "Can't register ERRIRQ for DMA\n");
			goto disable_dma_ahb_clk;
		}
		imxdma->irq_err = irq_err;
	}

	/* enable DMA module */
	imx_dmav1_writel(imxdma, DCR_DEN, DMA_DCR);

	/* clear all interrupts */
	imx_dmav1_writel(imxdma, (1 << IMX_DMA_CHANNELS) - 1, DMA_DISR);

	/* disable interrupts */
	imx_dmav1_writel(imxdma, (1 << IMX_DMA_CHANNELS) - 1, DMA_DIMR);

	INIT_LIST_HEAD(&imxdma->dma_device.channels);

	dma_cap_set(DMA_SLAVE, imxdma->dma_device.cap_mask);
	dma_cap_set(DMA_CYCLIC, imxdma->dma_device.cap_mask);
	dma_cap_set(DMA_MEMCPY, imxdma->dma_device.cap_mask);
	dma_cap_set(DMA_INTERLEAVE, imxdma->dma_device.cap_mask);

	/* Initialize 2D global parameters */
	for (i = 0; i < IMX_DMA_2D_SLOTS; i++)
		imxdma->slots_2d[i].count = 0;

	spin_lock_init(&imxdma->lock);

	/* Initialize channel parameters */
	for (i = 0; i < IMX_DMA_CHANNELS; i++) {
		struct imxdma_channel *imxdmac = &imxdma->channel[i];

		if (!is_imx1_dma(imxdma)) {
			ret = devm_request_irq(&pdev->dev, irq + i,
					dma_irq_handler, 0, "DMA", imxdma);
			if (ret) {
				dev_warn(imxdma->dev, "Can't register IRQ %d "
					 "for DMA channel %d\n",
					 irq + i, i);
				goto disable_dma_ahb_clk;
			}

			imxdmac->irq = irq + i;
			timer_setup(&imxdmac->watchdog, imxdma_watchdog, 0);
		}

		imxdmac->imxdma = imxdma;

		INIT_LIST_HEAD(&imxdmac->ld_queue);
		INIT_LIST_HEAD(&imxdmac->ld_free);
		INIT_LIST_HEAD(&imxdmac->ld_active);

		tasklet_setup(&imxdmac->dma_tasklet, imxdma_tasklet);
		imxdmac->chan.device = &imxdma->dma_device;
		dma_cookie_init(&imxdmac->chan);
		imxdmac->channel = i;

		/* Add the channel to the DMAC list */
		list_add_tail(&imxdmac->chan.device_node,
			      &imxdma->dma_device.channels);
	}

	imxdma->dma_device.dev = &pdev->dev;

	imxdma->dma_device.device_alloc_chan_resources = imxdma_alloc_chan_resources;
	imxdma->dma_device.device_free_chan_resources = imxdma_free_chan_resources;
	imxdma->dma_device.device_tx_status = imxdma_tx_status;
	imxdma->dma_device.device_prep_slave_sg = imxdma_prep_slave_sg;
	imxdma->dma_device.device_prep_dma_cyclic = imxdma_prep_dma_cyclic;
	imxdma->dma_device.device_prep_dma_memcpy = imxdma_prep_dma_memcpy;
	imxdma->dma_device.device_prep_interleaved_dma = imxdma_prep_dma_interleaved;
	imxdma->dma_device.device_config = imxdma_config;
	imxdma->dma_device.device_terminate_all = imxdma_terminate_all;
	imxdma->dma_device.device_issue_pending = imxdma_issue_pending;

	platform_set_drvdata(pdev, imxdma);

	imxdma->dma_device.copy_align = DMAENGINE_ALIGN_4_BYTES;
	dma_set_max_seg_size(imxdma->dma_device.dev, 0xffffff);

	ret = dma_async_device_register(&imxdma->dma_device);
	if (ret) {
		dev_err(&pdev->dev, "unable to register\n");
		goto disable_dma_ahb_clk;
	}

	if (pdev->dev.of_node) {
		ret = of_dma_controller_register(pdev->dev.of_node,
				imxdma_xlate, imxdma);
		if (ret) {
			dev_err(&pdev->dev, "unable to register of_dma_controller\n");
			goto err_of_dma_controller;
		}
	}

	return 0;

err_of_dma_controller:
	dma_async_device_unregister(&imxdma->dma_device);
disable_dma_ahb_clk:
	clk_disable_unprepare(imxdma->dma_ahb);
disable_dma_ipg_clk:
	clk_disable_unprepare(imxdma->dma_ipg);
	return ret;
}

static void imxdma_free_irq(struct platform_device *pdev, struct imxdma_engine *imxdma)
{
	int i;

	if (is_imx1_dma(imxdma)) {
		disable_irq(imxdma->irq);
		disable_irq(imxdma->irq_err);
	}

	for (i = 0; i < IMX_DMA_CHANNELS; i++) {
		struct imxdma_channel *imxdmac = &imxdma->channel[i];

		if (!is_imx1_dma(imxdma))
			disable_irq(imxdmac->irq);

		tasklet_kill(&imxdmac->dma_tasklet);
	}
}

static int imxdma_remove(struct platform_device *pdev)
{
	struct imxdma_engine *imxdma = platform_get_drvdata(pdev);

	imxdma_free_irq(pdev, imxdma);

        dma_async_device_unregister(&imxdma->dma_device);

	if (pdev->dev.of_node)
		of_dma_controller_free(pdev->dev.of_node);

	clk_disable_unprepare(imxdma->dma_ipg);
	clk_disable_unprepare(imxdma->dma_ahb);

        return 0;
}

static struct platform_driver imxdma_driver = {
	.driver		= {
		.name	= "imx-dma",
		.of_match_table = imx_dma_of_dev_id,
	},
	.remove		= imxdma_remove,
};

static int __init imxdma_module_init(void)
{
	return platform_driver_probe(&imxdma_driver, imxdma_probe);
}
subsys_initcall(imxdma_module_init);

MODULE_AUTHOR("Sascha Hauer, Pengutronix <s.hauer@pengutronix.de>");
MODULE_DESCRIPTION("i.MX dma driver");
MODULE_LICENSE("GPL");
