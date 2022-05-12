// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) Freescale Semicondutor, Inc. 2007, 2008.
 * Copyright (C) Semihalf 2009
 * Copyright (C) Ilya Yanok, Emcraft Systems 2010
 * Copyright (C) Alexander Popov, Promcontroller 2014
 * Copyright (C) Mario Six, Guntermann & Drunck GmbH, 2016
 *
 * Written by Piotr Ziecik <kosmo@semihalf.com>. Hardware description
 * (defines, structures and comments) was taken from MPC5121 DMA driver
 * written by Hongjun Chen <hong-jun.chen@freescale.com>.
 *
 * Approved as OSADL project by a majority of OSADL members and funded
 * by OSADL membership fees in 2009;  for details see www.osadl.org.
 */

/*
 * MPC512x and MPC8308 DMA driver. It supports memory to memory data transfers
 * (tested using dmatest module) and data transfers between memory and
 * peripheral I/O memory by means of slave scatter/gather with these
 * limitations:
 *  - chunked transfers (described by s/g lists with more than one item) are
 *     refused as long as proper support for scatter/gather is missing
 *  - transfers on MPC8308 always start from software as this SoC does not have
 *     external request lines for peripheral flow control
 *  - memory <-> I/O memory transfer chunks of sizes of 1, 2, 4, 16 (for
 *     MPC512x), and 32 bytes are supported, and, consequently, source
 *     addresses and destination addresses must be aligned accordingly;
 *     furthermore, for MPC512x SoCs, the transfer size must be aligned on
 *     (chunk size * maxburst)
 */

#include <linux/module.h>
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <linux/of_dma.h>
#include <linux/of_platform.h>

#include <linux/random.h>

#include "dmaengine.h"

/* Number of DMA Transfer descriptors allocated per channel */
#define MPC_DMA_DESCRIPTORS	64

/* Macro definitions */
#define MPC_DMA_TCD_OFFSET	0x1000

/*
 * Maximum channel counts for individual hardware variants
 * and the maximum channel count over all supported controllers,
 * used for data structure size
 */
#define MPC8308_DMACHAN_MAX	16
#define MPC512x_DMACHAN_MAX	64
#define MPC_DMA_CHANNELS	64

/* Arbitration mode of group and channel */
#define MPC_DMA_DMACR_EDCG	(1 << 31)
#define MPC_DMA_DMACR_ERGA	(1 << 3)
#define MPC_DMA_DMACR_ERCA	(1 << 2)

/* Error codes */
#define MPC_DMA_DMAES_VLD	(1 << 31)
#define MPC_DMA_DMAES_GPE	(1 << 15)
#define MPC_DMA_DMAES_CPE	(1 << 14)
#define MPC_DMA_DMAES_ERRCHN(err) \
				(((err) >> 8) & 0x3f)
#define MPC_DMA_DMAES_SAE	(1 << 7)
#define MPC_DMA_DMAES_SOE	(1 << 6)
#define MPC_DMA_DMAES_DAE	(1 << 5)
#define MPC_DMA_DMAES_DOE	(1 << 4)
#define MPC_DMA_DMAES_NCE	(1 << 3)
#define MPC_DMA_DMAES_SGE	(1 << 2)
#define MPC_DMA_DMAES_SBE	(1 << 1)
#define MPC_DMA_DMAES_DBE	(1 << 0)

#define MPC_DMA_DMAGPOR_SNOOP_ENABLE	(1 << 6)

#define MPC_DMA_TSIZE_1		0x00
#define MPC_DMA_TSIZE_2		0x01
#define MPC_DMA_TSIZE_4		0x02
#define MPC_DMA_TSIZE_16	0x04
#define MPC_DMA_TSIZE_32	0x05

/* MPC5121 DMA engine registers */
struct __attribute__ ((__packed__)) mpc_dma_regs {
	/* 0x00 */
	u32 dmacr;		/* DMA control register */
	u32 dmaes;		/* DMA error status */
	/* 0x08 */
	u32 dmaerqh;		/* DMA enable request high(channels 63~32) */
	u32 dmaerql;		/* DMA enable request low(channels 31~0) */
	u32 dmaeeih;		/* DMA enable error interrupt high(ch63~32) */
	u32 dmaeeil;		/* DMA enable error interrupt low(ch31~0) */
	/* 0x18 */
	u8 dmaserq;		/* DMA set enable request */
	u8 dmacerq;		/* DMA clear enable request */
	u8 dmaseei;		/* DMA set enable error interrupt */
	u8 dmaceei;		/* DMA clear enable error interrupt */
	/* 0x1c */
	u8 dmacint;		/* DMA clear interrupt request */
	u8 dmacerr;		/* DMA clear error */
	u8 dmassrt;		/* DMA set start bit */
	u8 dmacdne;		/* DMA clear DONE status bit */
	/* 0x20 */
	u32 dmainth;		/* DMA interrupt request high(ch63~32) */
	u32 dmaintl;		/* DMA interrupt request low(ch31~0) */
	u32 dmaerrh;		/* DMA error high(ch63~32) */
	u32 dmaerrl;		/* DMA error low(ch31~0) */
	/* 0x30 */
	u32 dmahrsh;		/* DMA hw request status high(ch63~32) */
	u32 dmahrsl;		/* DMA hardware request status low(ch31~0) */
	union {
		u32 dmaihsa;	/* DMA interrupt high select AXE(ch63~32) */
		u32 dmagpor;	/* (General purpose register on MPC8308) */
	};
	u32 dmailsa;		/* DMA interrupt low select AXE(ch31~0) */
	/* 0x40 ~ 0xff */
	u32 reserve0[48];	/* Reserved */
	/* 0x100 */
	u8 dchpri[MPC_DMA_CHANNELS];
	/* DMA channels(0~63) priority */
};

struct __attribute__ ((__packed__)) mpc_dma_tcd {
	/* 0x00 */
	u32 saddr;		/* Source address */

	u32 smod:5;		/* Source address modulo */
	u32 ssize:3;		/* Source data transfer size */
	u32 dmod:5;		/* Destination address modulo */
	u32 dsize:3;		/* Destination data transfer size */
	u32 soff:16;		/* Signed source address offset */

	/* 0x08 */
	u32 nbytes;		/* Inner "minor" byte count */
	u32 slast;		/* Last source address adjustment */
	u32 daddr;		/* Destination address */

	/* 0x14 */
	u32 citer_elink:1;	/* Enable channel-to-channel linking on
				 * minor loop complete
				 */
	u32 citer_linkch:6;	/* Link channel for minor loop complete */
	u32 citer:9;		/* Current "major" iteration count */
	u32 doff:16;		/* Signed destination address offset */

	/* 0x18 */
	u32 dlast_sga;		/* Last Destination address adjustment/scatter
				 * gather address
				 */

	/* 0x1c */
	u32 biter_elink:1;	/* Enable channel-to-channel linking on major
				 * loop complete
				 */
	u32 biter_linkch:6;
	u32 biter:9;		/* Beginning "major" iteration count */
	u32 bwc:2;		/* Bandwidth control */
	u32 major_linkch:6;	/* Link channel number */
	u32 done:1;		/* Channel done */
	u32 active:1;		/* Channel active */
	u32 major_elink:1;	/* Enable channel-to-channel linking on major
				 * loop complete
				 */
	u32 e_sg:1;		/* Enable scatter/gather processing */
	u32 d_req:1;		/* Disable request */
	u32 int_half:1;		/* Enable an interrupt when major counter is
				 * half complete
				 */
	u32 int_maj:1;		/* Enable an interrupt when major iteration
				 * count completes
				 */
	u32 start:1;		/* Channel start */
};

struct mpc_dma_desc {
	struct dma_async_tx_descriptor	desc;
	struct mpc_dma_tcd		*tcd;
	dma_addr_t			tcd_paddr;
	int				error;
	struct list_head		node;
	int				will_access_peripheral;
};

struct mpc_dma_chan {
	struct dma_chan			chan;
	struct list_head		free;
	struct list_head		prepared;
	struct list_head		queued;
	struct list_head		active;
	struct list_head		completed;
	struct mpc_dma_tcd		*tcd;
	dma_addr_t			tcd_paddr;

	/* Settings for access to peripheral FIFO */
	dma_addr_t			src_per_paddr;
	u32				src_tcd_nunits;
	u8				swidth;
	dma_addr_t			dst_per_paddr;
	u32				dst_tcd_nunits;
	u8				dwidth;

	/* Lock for this structure */
	spinlock_t			lock;
};

struct mpc_dma {
	struct dma_device		dma;
	struct tasklet_struct		tasklet;
	struct mpc_dma_chan		channels[MPC_DMA_CHANNELS];
	struct mpc_dma_regs __iomem	*regs;
	struct mpc_dma_tcd __iomem	*tcd;
	int				irq;
	int				irq2;
	uint				error_status;
	int				is_mpc8308;

	/* Lock for error_status field in this structure */
	spinlock_t			error_status_lock;
};

#define DRV_NAME	"mpc512x_dma"

/* Convert struct dma_chan to struct mpc_dma_chan */
static inline struct mpc_dma_chan *dma_chan_to_mpc_dma_chan(struct dma_chan *c)
{
	return container_of(c, struct mpc_dma_chan, chan);
}

/* Convert struct dma_chan to struct mpc_dma */
static inline struct mpc_dma *dma_chan_to_mpc_dma(struct dma_chan *c)
{
	struct mpc_dma_chan *mchan = dma_chan_to_mpc_dma_chan(c);

	return container_of(mchan, struct mpc_dma, channels[c->chan_id]);
}

/*
 * Execute all queued DMA descriptors.
 *
 * Following requirements must be met while calling mpc_dma_execute():
 *	a) mchan->lock is acquired,
 *	b) mchan->active list is empty,
 *	c) mchan->queued list contains at least one entry.
 */
static void mpc_dma_execute(struct mpc_dma_chan *mchan)
{
	struct mpc_dma *mdma = dma_chan_to_mpc_dma(&mchan->chan);
	struct mpc_dma_desc *first = NULL;
	struct mpc_dma_desc *prev = NULL;
	struct mpc_dma_desc *mdesc;
	int cid = mchan->chan.chan_id;

	while (!list_empty(&mchan->queued)) {
		mdesc = list_first_entry(&mchan->queued,
						struct mpc_dma_desc, node);
		/*
		 * Grab either several mem-to-mem transfer descriptors
		 * or one peripheral transfer descriptor,
		 * don't mix mem-to-mem and peripheral transfer descriptors
		 * within the same 'active' list.
		 */
		if (mdesc->will_access_peripheral) {
			if (list_empty(&mchan->active))
				list_move_tail(&mdesc->node, &mchan->active);
			break;
		} else {
			list_move_tail(&mdesc->node, &mchan->active);
		}
	}

	/* Chain descriptors into one transaction */
	list_for_each_entry(mdesc, &mchan->active, node) {
		if (!first)
			first = mdesc;

		if (!prev) {
			prev = mdesc;
			continue;
		}

		prev->tcd->dlast_sga = mdesc->tcd_paddr;
		prev->tcd->e_sg = 1;
		mdesc->tcd->start = 1;

		prev = mdesc;
	}

	prev->tcd->int_maj = 1;

	/* Send first descriptor in chain into hardware */
	memcpy_toio(&mdma->tcd[cid], first->tcd, sizeof(struct mpc_dma_tcd));

	if (first != prev)
		mdma->tcd[cid].e_sg = 1;

	if (mdma->is_mpc8308) {
		/* MPC8308, no request lines, software initiated start */
		out_8(&mdma->regs->dmassrt, cid);
	} else if (first->will_access_peripheral) {
		/* Peripherals involved, start by external request signal */
		out_8(&mdma->regs->dmaserq, cid);
	} else {
		/* Memory to memory transfer, software initiated start */
		out_8(&mdma->regs->dmassrt, cid);
	}
}

/* Handle interrupt on one half of DMA controller (32 channels) */
static void mpc_dma_irq_process(struct mpc_dma *mdma, u32 is, u32 es, int off)
{
	struct mpc_dma_chan *mchan;
	struct mpc_dma_desc *mdesc;
	u32 status = is | es;
	int ch;

	while ((ch = fls(status) - 1) >= 0) {
		status &= ~(1 << ch);
		mchan = &mdma->channels[ch + off];

		spin_lock(&mchan->lock);

		out_8(&mdma->regs->dmacint, ch + off);
		out_8(&mdma->regs->dmacerr, ch + off);

		/* Check error status */
		if (es & (1 << ch))
			list_for_each_entry(mdesc, &mchan->active, node)
				mdesc->error = -EIO;

		/* Execute queued descriptors */
		list_splice_tail_init(&mchan->active, &mchan->completed);
		if (!list_empty(&mchan->queued))
			mpc_dma_execute(mchan);

		spin_unlock(&mchan->lock);
	}
}

/* Interrupt handler */
static irqreturn_t mpc_dma_irq(int irq, void *data)
{
	struct mpc_dma *mdma = data;
	uint es;

	/* Save error status register */
	es = in_be32(&mdma->regs->dmaes);
	spin_lock(&mdma->error_status_lock);
	if ((es & MPC_DMA_DMAES_VLD) && mdma->error_status == 0)
		mdma->error_status = es;
	spin_unlock(&mdma->error_status_lock);

	/* Handle interrupt on each channel */
	if (mdma->dma.chancnt > 32) {
		mpc_dma_irq_process(mdma, in_be32(&mdma->regs->dmainth),
					in_be32(&mdma->regs->dmaerrh), 32);
	}
	mpc_dma_irq_process(mdma, in_be32(&mdma->regs->dmaintl),
					in_be32(&mdma->regs->dmaerrl), 0);

	/* Schedule tasklet */
	tasklet_schedule(&mdma->tasklet);

	return IRQ_HANDLED;
}

/* process completed descriptors */
static void mpc_dma_process_completed(struct mpc_dma *mdma)
{
	dma_cookie_t last_cookie = 0;
	struct mpc_dma_chan *mchan;
	struct mpc_dma_desc *mdesc;
	struct dma_async_tx_descriptor *desc;
	unsigned long flags;
	LIST_HEAD(list);
	int i;

	for (i = 0; i < mdma->dma.chancnt; i++) {
		mchan = &mdma->channels[i];

		/* Get all completed descriptors */
		spin_lock_irqsave(&mchan->lock, flags);
		if (!list_empty(&mchan->completed))
			list_splice_tail_init(&mchan->completed, &list);
		spin_unlock_irqrestore(&mchan->lock, flags);

		if (list_empty(&list))
			continue;

		/* Execute callbacks and run dependencies */
		list_for_each_entry(mdesc, &list, node) {
			desc = &mdesc->desc;

			dmaengine_desc_get_callback_invoke(desc, NULL);

			last_cookie = desc->cookie;
			dma_run_dependencies(desc);
		}

		/* Free descriptors */
		spin_lock_irqsave(&mchan->lock, flags);
		list_splice_tail_init(&list, &mchan->free);
		mchan->chan.completed_cookie = last_cookie;
		spin_unlock_irqrestore(&mchan->lock, flags);
	}
}

/* DMA Tasklet */
static void mpc_dma_tasklet(struct tasklet_struct *t)
{
	struct mpc_dma *mdma = from_tasklet(mdma, t, tasklet);
	unsigned long flags;
	uint es;

	spin_lock_irqsave(&mdma->error_status_lock, flags);
	es = mdma->error_status;
	mdma->error_status = 0;
	spin_unlock_irqrestore(&mdma->error_status_lock, flags);

	/* Print nice error report */
	if (es) {
		dev_err(mdma->dma.dev,
			"Hardware reported following error(s) on channel %u:\n",
						      MPC_DMA_DMAES_ERRCHN(es));

		if (es & MPC_DMA_DMAES_GPE)
			dev_err(mdma->dma.dev, "- Group Priority Error\n");
		if (es & MPC_DMA_DMAES_CPE)
			dev_err(mdma->dma.dev, "- Channel Priority Error\n");
		if (es & MPC_DMA_DMAES_SAE)
			dev_err(mdma->dma.dev, "- Source Address Error\n");
		if (es & MPC_DMA_DMAES_SOE)
			dev_err(mdma->dma.dev, "- Source Offset Configuration Error\n");
		if (es & MPC_DMA_DMAES_DAE)
			dev_err(mdma->dma.dev, "- Destination Address Error\n");
		if (es & MPC_DMA_DMAES_DOE)
			dev_err(mdma->dma.dev, "- Destination Offset Configuration Error\n");
		if (es & MPC_DMA_DMAES_NCE)
			dev_err(mdma->dma.dev, "- NBytes/Citter Configuration Error\n");
		if (es & MPC_DMA_DMAES_SGE)
			dev_err(mdma->dma.dev, "- Scatter/Gather Configuration Error\n");
		if (es & MPC_DMA_DMAES_SBE)
			dev_err(mdma->dma.dev, "- Source Bus Error\n");
		if (es & MPC_DMA_DMAES_DBE)
			dev_err(mdma->dma.dev, "- Destination Bus Error\n");
	}

	mpc_dma_process_completed(mdma);
}

/* Submit descriptor to hardware */
static dma_cookie_t mpc_dma_tx_submit(struct dma_async_tx_descriptor *txd)
{
	struct mpc_dma_chan *mchan = dma_chan_to_mpc_dma_chan(txd->chan);
	struct mpc_dma_desc *mdesc;
	unsigned long flags;
	dma_cookie_t cookie;

	mdesc = container_of(txd, struct mpc_dma_desc, desc);

	spin_lock_irqsave(&mchan->lock, flags);

	/* Move descriptor to queue */
	list_move_tail(&mdesc->node, &mchan->queued);

	/* If channel is idle, execute all queued descriptors */
	if (list_empty(&mchan->active))
		mpc_dma_execute(mchan);

	/* Update cookie */
	cookie = dma_cookie_assign(txd);
	spin_unlock_irqrestore(&mchan->lock, flags);

	return cookie;
}

/* Alloc channel resources */
static int mpc_dma_alloc_chan_resources(struct dma_chan *chan)
{
	struct mpc_dma *mdma = dma_chan_to_mpc_dma(chan);
	struct mpc_dma_chan *mchan = dma_chan_to_mpc_dma_chan(chan);
	struct mpc_dma_desc *mdesc;
	struct mpc_dma_tcd *tcd;
	dma_addr_t tcd_paddr;
	unsigned long flags;
	LIST_HEAD(descs);
	int i;

	/* Alloc DMA memory for Transfer Control Descriptors */
	tcd = dma_alloc_coherent(mdma->dma.dev,
			MPC_DMA_DESCRIPTORS * sizeof(struct mpc_dma_tcd),
							&tcd_paddr, GFP_KERNEL);
	if (!tcd)
		return -ENOMEM;

	/* Alloc descriptors for this channel */
	for (i = 0; i < MPC_DMA_DESCRIPTORS; i++) {
		mdesc = kzalloc(sizeof(struct mpc_dma_desc), GFP_KERNEL);
		if (!mdesc) {
			dev_notice(mdma->dma.dev,
				"Memory allocation error. Allocated only %u descriptors\n", i);
			break;
		}

		dma_async_tx_descriptor_init(&mdesc->desc, chan);
		mdesc->desc.flags = DMA_CTRL_ACK;
		mdesc->desc.tx_submit = mpc_dma_tx_submit;

		mdesc->tcd = &tcd[i];
		mdesc->tcd_paddr = tcd_paddr + (i * sizeof(struct mpc_dma_tcd));

		list_add_tail(&mdesc->node, &descs);
	}

	/* Return error only if no descriptors were allocated */
	if (i == 0) {
		dma_free_coherent(mdma->dma.dev,
			MPC_DMA_DESCRIPTORS * sizeof(struct mpc_dma_tcd),
								tcd, tcd_paddr);
		return -ENOMEM;
	}

	spin_lock_irqsave(&mchan->lock, flags);
	mchan->tcd = tcd;
	mchan->tcd_paddr = tcd_paddr;
	list_splice_tail_init(&descs, &mchan->free);
	spin_unlock_irqrestore(&mchan->lock, flags);

	/* Enable Error Interrupt */
	out_8(&mdma->regs->dmaseei, chan->chan_id);

	return 0;
}

/* Free channel resources */
static void mpc_dma_free_chan_resources(struct dma_chan *chan)
{
	struct mpc_dma *mdma = dma_chan_to_mpc_dma(chan);
	struct mpc_dma_chan *mchan = dma_chan_to_mpc_dma_chan(chan);
	struct mpc_dma_desc *mdesc, *tmp;
	struct mpc_dma_tcd *tcd;
	dma_addr_t tcd_paddr;
	unsigned long flags;
	LIST_HEAD(descs);

	spin_lock_irqsave(&mchan->lock, flags);

	/* Channel must be idle */
	BUG_ON(!list_empty(&mchan->prepared));
	BUG_ON(!list_empty(&mchan->queued));
	BUG_ON(!list_empty(&mchan->active));
	BUG_ON(!list_empty(&mchan->completed));

	/* Move data */
	list_splice_tail_init(&mchan->free, &descs);
	tcd = mchan->tcd;
	tcd_paddr = mchan->tcd_paddr;

	spin_unlock_irqrestore(&mchan->lock, flags);

	/* Free DMA memory used by descriptors */
	dma_free_coherent(mdma->dma.dev,
			MPC_DMA_DESCRIPTORS * sizeof(struct mpc_dma_tcd),
								tcd, tcd_paddr);

	/* Free descriptors */
	list_for_each_entry_safe(mdesc, tmp, &descs, node)
		kfree(mdesc);

	/* Disable Error Interrupt */
	out_8(&mdma->regs->dmaceei, chan->chan_id);
}

/* Send all pending descriptor to hardware */
static void mpc_dma_issue_pending(struct dma_chan *chan)
{
	/*
	 * We are posting descriptors to the hardware as soon as
	 * they are ready, so this function does nothing.
	 */
}

/* Check request completion status */
static enum dma_status
mpc_dma_tx_status(struct dma_chan *chan, dma_cookie_t cookie,
	       struct dma_tx_state *txstate)
{
	return dma_cookie_status(chan, cookie, txstate);
}

/* Prepare descriptor for memory to memory copy */
static struct dma_async_tx_descriptor *
mpc_dma_prep_memcpy(struct dma_chan *chan, dma_addr_t dst, dma_addr_t src,
					size_t len, unsigned long flags)
{
	struct mpc_dma *mdma = dma_chan_to_mpc_dma(chan);
	struct mpc_dma_chan *mchan = dma_chan_to_mpc_dma_chan(chan);
	struct mpc_dma_desc *mdesc = NULL;
	struct mpc_dma_tcd *tcd;
	unsigned long iflags;

	/* Get free descriptor */
	spin_lock_irqsave(&mchan->lock, iflags);
	if (!list_empty(&mchan->free)) {
		mdesc = list_first_entry(&mchan->free, struct mpc_dma_desc,
									node);
		list_del(&mdesc->node);
	}
	spin_unlock_irqrestore(&mchan->lock, iflags);

	if (!mdesc) {
		/* try to free completed descriptors */
		mpc_dma_process_completed(mdma);
		return NULL;
	}

	mdesc->error = 0;
	mdesc->will_access_peripheral = 0;
	tcd = mdesc->tcd;

	/* Prepare Transfer Control Descriptor for this transaction */
	memset(tcd, 0, sizeof(struct mpc_dma_tcd));

	if (IS_ALIGNED(src | dst | len, 32)) {
		tcd->ssize = MPC_DMA_TSIZE_32;
		tcd->dsize = MPC_DMA_TSIZE_32;
		tcd->soff = 32;
		tcd->doff = 32;
	} else if (!mdma->is_mpc8308 && IS_ALIGNED(src | dst | len, 16)) {
		/* MPC8308 doesn't support 16 byte transfers */
		tcd->ssize = MPC_DMA_TSIZE_16;
		tcd->dsize = MPC_DMA_TSIZE_16;
		tcd->soff = 16;
		tcd->doff = 16;
	} else if (IS_ALIGNED(src | dst | len, 4)) {
		tcd->ssize = MPC_DMA_TSIZE_4;
		tcd->dsize = MPC_DMA_TSIZE_4;
		tcd->soff = 4;
		tcd->doff = 4;
	} else if (IS_ALIGNED(src | dst | len, 2)) {
		tcd->ssize = MPC_DMA_TSIZE_2;
		tcd->dsize = MPC_DMA_TSIZE_2;
		tcd->soff = 2;
		tcd->doff = 2;
	} else {
		tcd->ssize = MPC_DMA_TSIZE_1;
		tcd->dsize = MPC_DMA_TSIZE_1;
		tcd->soff = 1;
		tcd->doff = 1;
	}

	tcd->saddr = src;
	tcd->daddr = dst;
	tcd->nbytes = len;
	tcd->biter = 1;
	tcd->citer = 1;

	/* Place descriptor in prepared list */
	spin_lock_irqsave(&mchan->lock, iflags);
	list_add_tail(&mdesc->node, &mchan->prepared);
	spin_unlock_irqrestore(&mchan->lock, iflags);

	return &mdesc->desc;
}

inline u8 buswidth_to_dmatsize(u8 buswidth)
{
	u8 res;

	for (res = 0; buswidth > 1; buswidth /= 2)
		res++;
	return res;
}

static struct dma_async_tx_descriptor *
mpc_dma_prep_slave_sg(struct dma_chan *chan, struct scatterlist *sgl,
		unsigned int sg_len, enum dma_transfer_direction direction,
		unsigned long flags, void *context)
{
	struct mpc_dma *mdma = dma_chan_to_mpc_dma(chan);
	struct mpc_dma_chan *mchan = dma_chan_to_mpc_dma_chan(chan);
	struct mpc_dma_desc *mdesc = NULL;
	dma_addr_t per_paddr;
	u32 tcd_nunits;
	struct mpc_dma_tcd *tcd;
	unsigned long iflags;
	struct scatterlist *sg;
	size_t len;
	int iter, i;

	/* Currently there is no proper support for scatter/gather */
	if (sg_len != 1)
		return NULL;

	if (!is_slave_direction(direction))
		return NULL;

	for_each_sg(sgl, sg, sg_len, i) {
		spin_lock_irqsave(&mchan->lock, iflags);

		mdesc = list_first_entry(&mchan->free,
						struct mpc_dma_desc, node);
		if (!mdesc) {
			spin_unlock_irqrestore(&mchan->lock, iflags);
			/* Try to free completed descriptors */
			mpc_dma_process_completed(mdma);
			return NULL;
		}

		list_del(&mdesc->node);

		if (direction == DMA_DEV_TO_MEM) {
			per_paddr = mchan->src_per_paddr;
			tcd_nunits = mchan->src_tcd_nunits;
		} else {
			per_paddr = mchan->dst_per_paddr;
			tcd_nunits = mchan->dst_tcd_nunits;
		}

		spin_unlock_irqrestore(&mchan->lock, iflags);

		if (per_paddr == 0 || tcd_nunits == 0)
			goto err_prep;

		mdesc->error = 0;
		mdesc->will_access_peripheral = 1;

		/* Prepare Transfer Control Descriptor for this transaction */
		tcd = mdesc->tcd;

		memset(tcd, 0, sizeof(struct mpc_dma_tcd));

		if (direction == DMA_DEV_TO_MEM) {
			tcd->saddr = per_paddr;
			tcd->daddr = sg_dma_address(sg);

			if (!IS_ALIGNED(sg_dma_address(sg), mchan->dwidth))
				goto err_prep;

			tcd->soff = 0;
			tcd->doff = mchan->dwidth;
		} else {
			tcd->saddr = sg_dma_address(sg);
			tcd->daddr = per_paddr;

			if (!IS_ALIGNED(sg_dma_address(sg), mchan->swidth))
				goto err_prep;

			tcd->soff = mchan->swidth;
			tcd->doff = 0;
		}

		tcd->ssize = buswidth_to_dmatsize(mchan->swidth);
		tcd->dsize = buswidth_to_dmatsize(mchan->dwidth);

		if (mdma->is_mpc8308) {
			tcd->nbytes = sg_dma_len(sg);
			if (!IS_ALIGNED(tcd->nbytes, mchan->swidth))
				goto err_prep;

			/* No major loops for MPC8303 */
			tcd->biter = 1;
			tcd->citer = 1;
		} else {
			len = sg_dma_len(sg);
			tcd->nbytes = tcd_nunits * tcd->ssize;
			if (!IS_ALIGNED(len, tcd->nbytes))
				goto err_prep;

			iter = len / tcd->nbytes;
			if (iter >= 1 << 15) {
				/* len is too big */
				goto err_prep;
			}
			/* citer_linkch contains the high bits of iter */
			tcd->biter = iter & 0x1ff;
			tcd->biter_linkch = iter >> 9;
			tcd->citer = tcd->biter;
			tcd->citer_linkch = tcd->biter_linkch;
		}

		tcd->e_sg = 0;
		tcd->d_req = 1;

		/* Place descriptor in prepared list */
		spin_lock_irqsave(&mchan->lock, iflags);
		list_add_tail(&mdesc->node, &mchan->prepared);
		spin_unlock_irqrestore(&mchan->lock, iflags);
	}

	return &mdesc->desc;

err_prep:
	/* Put the descriptor back */
	spin_lock_irqsave(&mchan->lock, iflags);
	list_add_tail(&mdesc->node, &mchan->free);
	spin_unlock_irqrestore(&mchan->lock, iflags);

	return NULL;
}

inline bool is_buswidth_valid(u8 buswidth, bool is_mpc8308)
{
	switch (buswidth) {
	case 16:
		if (is_mpc8308)
			return false;
	case 1:
	case 2:
	case 4:
	case 32:
		break;
	default:
		return false;
	}

	return true;
}

static int mpc_dma_device_config(struct dma_chan *chan,
				 struct dma_slave_config *cfg)
{
	struct mpc_dma_chan *mchan = dma_chan_to_mpc_dma_chan(chan);
	struct mpc_dma *mdma = dma_chan_to_mpc_dma(&mchan->chan);
	unsigned long flags;

	/*
	 * Software constraints:
	 *  - only transfers between a peripheral device and memory are
	 *     supported
	 *  - transfer chunk sizes of 1, 2, 4, 16 (for MPC512x), and 32 bytes
	 *     are supported, and, consequently, source addresses and
	 *     destination addresses; must be aligned accordingly; furthermore,
	 *     for MPC512x SoCs, the transfer size must be aligned on (chunk
	 *     size * maxburst)
	 *  - during the transfer, the RAM address is incremented by the size
	 *     of transfer chunk
	 *  - the peripheral port's address is constant during the transfer.
	 */

	if (!IS_ALIGNED(cfg->src_addr, cfg->src_addr_width) ||
	    !IS_ALIGNED(cfg->dst_addr, cfg->dst_addr_width)) {
		return -EINVAL;
	}

	if (!is_buswidth_valid(cfg->src_addr_width, mdma->is_mpc8308) ||
	    !is_buswidth_valid(cfg->dst_addr_width, mdma->is_mpc8308))
		return -EINVAL;

	spin_lock_irqsave(&mchan->lock, flags);

	mchan->src_per_paddr = cfg->src_addr;
	mchan->src_tcd_nunits = cfg->src_maxburst;
	mchan->swidth = cfg->src_addr_width;
	mchan->dst_per_paddr = cfg->dst_addr;
	mchan->dst_tcd_nunits = cfg->dst_maxburst;
	mchan->dwidth = cfg->dst_addr_width;

	/* Apply defaults */
	if (mchan->src_tcd_nunits == 0)
		mchan->src_tcd_nunits = 1;
	if (mchan->dst_tcd_nunits == 0)
		mchan->dst_tcd_nunits = 1;

	spin_unlock_irqrestore(&mchan->lock, flags);

	return 0;
}

static int mpc_dma_device_terminate_all(struct dma_chan *chan)
{
	struct mpc_dma_chan *mchan = dma_chan_to_mpc_dma_chan(chan);
	struct mpc_dma *mdma = dma_chan_to_mpc_dma(chan);
	unsigned long flags;

	/* Disable channel requests */
	spin_lock_irqsave(&mchan->lock, flags);

	out_8(&mdma->regs->dmacerq, chan->chan_id);
	list_splice_tail_init(&mchan->prepared, &mchan->free);
	list_splice_tail_init(&mchan->queued, &mchan->free);
	list_splice_tail_init(&mchan->active, &mchan->free);

	spin_unlock_irqrestore(&mchan->lock, flags);

	return 0;
}

static int mpc_dma_probe(struct platform_device *op)
{
	struct device_node *dn = op->dev.of_node;
	struct device *dev = &op->dev;
	struct dma_device *dma;
	struct mpc_dma *mdma;
	struct mpc_dma_chan *mchan;
	struct resource res;
	ulong regs_start, regs_size;
	int retval, i;
	u8 chancnt;

	mdma = devm_kzalloc(dev, sizeof(struct mpc_dma), GFP_KERNEL);
	if (!mdma) {
		retval = -ENOMEM;
		goto err;
	}

	mdma->irq = irq_of_parse_and_map(dn, 0);
	if (!mdma->irq) {
		dev_err(dev, "Error mapping IRQ!\n");
		retval = -EINVAL;
		goto err;
	}

	if (of_device_is_compatible(dn, "fsl,mpc8308-dma")) {
		mdma->is_mpc8308 = 1;
		mdma->irq2 = irq_of_parse_and_map(dn, 1);
		if (!mdma->irq2) {
			dev_err(dev, "Error mapping IRQ!\n");
			retval = -EINVAL;
			goto err_dispose1;
		}
	}

	retval = of_address_to_resource(dn, 0, &res);
	if (retval) {
		dev_err(dev, "Error parsing memory region!\n");
		goto err_dispose2;
	}

	regs_start = res.start;
	regs_size = resource_size(&res);

	if (!devm_request_mem_region(dev, regs_start, regs_size, DRV_NAME)) {
		dev_err(dev, "Error requesting memory region!\n");
		retval = -EBUSY;
		goto err_dispose2;
	}

	mdma->regs = devm_ioremap(dev, regs_start, regs_size);
	if (!mdma->regs) {
		dev_err(dev, "Error mapping memory region!\n");
		retval = -ENOMEM;
		goto err_dispose2;
	}

	mdma->tcd = (struct mpc_dma_tcd *)((u8 *)(mdma->regs)
							+ MPC_DMA_TCD_OFFSET);

	retval = request_irq(mdma->irq, &mpc_dma_irq, 0, DRV_NAME, mdma);
	if (retval) {
		dev_err(dev, "Error requesting IRQ!\n");
		retval = -EINVAL;
		goto err_dispose2;
	}

	if (mdma->is_mpc8308) {
		retval = request_irq(mdma->irq2, &mpc_dma_irq, 0,
							DRV_NAME, mdma);
		if (retval) {
			dev_err(dev, "Error requesting IRQ2!\n");
			retval = -EINVAL;
			goto err_free1;
		}
	}

	spin_lock_init(&mdma->error_status_lock);

	dma = &mdma->dma;
	dma->dev = dev;
	dma->device_alloc_chan_resources = mpc_dma_alloc_chan_resources;
	dma->device_free_chan_resources = mpc_dma_free_chan_resources;
	dma->device_issue_pending = mpc_dma_issue_pending;
	dma->device_tx_status = mpc_dma_tx_status;
	dma->device_prep_dma_memcpy = mpc_dma_prep_memcpy;
	dma->device_prep_slave_sg = mpc_dma_prep_slave_sg;
	dma->device_config = mpc_dma_device_config;
	dma->device_terminate_all = mpc_dma_device_terminate_all;

	INIT_LIST_HEAD(&dma->channels);
	dma_cap_set(DMA_MEMCPY, dma->cap_mask);
	dma_cap_set(DMA_SLAVE, dma->cap_mask);

	if (mdma->is_mpc8308)
		chancnt = MPC8308_DMACHAN_MAX;
	else
		chancnt = MPC512x_DMACHAN_MAX;

	for (i = 0; i < chancnt; i++) {
		mchan = &mdma->channels[i];

		mchan->chan.device = dma;
		dma_cookie_init(&mchan->chan);

		INIT_LIST_HEAD(&mchan->free);
		INIT_LIST_HEAD(&mchan->prepared);
		INIT_LIST_HEAD(&mchan->queued);
		INIT_LIST_HEAD(&mchan->active);
		INIT_LIST_HEAD(&mchan->completed);

		spin_lock_init(&mchan->lock);
		list_add_tail(&mchan->chan.device_node, &dma->channels);
	}

	tasklet_setup(&mdma->tasklet, mpc_dma_tasklet);

	/*
	 * Configure DMA Engine:
	 * - Dynamic clock,
	 * - Round-robin group arbitration,
	 * - Round-robin channel arbitration.
	 */
	if (mdma->is_mpc8308) {
		/* MPC8308 has 16 channels and lacks some registers */
		out_be32(&mdma->regs->dmacr, MPC_DMA_DMACR_ERCA);

		/* enable snooping */
		out_be32(&mdma->regs->dmagpor, MPC_DMA_DMAGPOR_SNOOP_ENABLE);
		/* Disable error interrupts */
		out_be32(&mdma->regs->dmaeeil, 0);

		/* Clear interrupts status */
		out_be32(&mdma->regs->dmaintl, 0xFFFF);
		out_be32(&mdma->regs->dmaerrl, 0xFFFF);
	} else {
		out_be32(&mdma->regs->dmacr, MPC_DMA_DMACR_EDCG |
						MPC_DMA_DMACR_ERGA |
						MPC_DMA_DMACR_ERCA);

		/* Disable hardware DMA requests */
		out_be32(&mdma->regs->dmaerqh, 0);
		out_be32(&mdma->regs->dmaerql, 0);

		/* Disable error interrupts */
		out_be32(&mdma->regs->dmaeeih, 0);
		out_be32(&mdma->regs->dmaeeil, 0);

		/* Clear interrupts status */
		out_be32(&mdma->regs->dmainth, 0xFFFFFFFF);
		out_be32(&mdma->regs->dmaintl, 0xFFFFFFFF);
		out_be32(&mdma->regs->dmaerrh, 0xFFFFFFFF);
		out_be32(&mdma->regs->dmaerrl, 0xFFFFFFFF);

		/* Route interrupts to IPIC */
		out_be32(&mdma->regs->dmaihsa, 0);
		out_be32(&mdma->regs->dmailsa, 0);
	}

	/* Register DMA engine */
	dev_set_drvdata(dev, mdma);
	retval = dma_async_device_register(dma);
	if (retval)
		goto err_free2;

	/* Register with OF helpers for DMA lookups (nonfatal) */
	if (dev->of_node) {
		retval = of_dma_controller_register(dev->of_node,
						of_dma_xlate_by_chan_id, mdma);
		if (retval)
			dev_warn(dev, "Could not register for OF lookup\n");
	}

	return 0;

err_free2:
	if (mdma->is_mpc8308)
		free_irq(mdma->irq2, mdma);
err_free1:
	free_irq(mdma->irq, mdma);
err_dispose2:
	if (mdma->is_mpc8308)
		irq_dispose_mapping(mdma->irq2);
err_dispose1:
	irq_dispose_mapping(mdma->irq);
err:
	return retval;
}

static int mpc_dma_remove(struct platform_device *op)
{
	struct device *dev = &op->dev;
	struct mpc_dma *mdma = dev_get_drvdata(dev);

	if (dev->of_node)
		of_dma_controller_free(dev->of_node);
	dma_async_device_unregister(&mdma->dma);
	if (mdma->is_mpc8308) {
		free_irq(mdma->irq2, mdma);
		irq_dispose_mapping(mdma->irq2);
	}
	free_irq(mdma->irq, mdma);
	irq_dispose_mapping(mdma->irq);
	tasklet_kill(&mdma->tasklet);

	return 0;
}

static const struct of_device_id mpc_dma_match[] = {
	{ .compatible = "fsl,mpc5121-dma", },
	{ .compatible = "fsl,mpc8308-dma", },
	{},
};
MODULE_DEVICE_TABLE(of, mpc_dma_match);

static struct platform_driver mpc_dma_driver = {
	.probe		= mpc_dma_probe,
	.remove		= mpc_dma_remove,
	.driver = {
		.name = DRV_NAME,
		.of_match_table	= mpc_dma_match,
	},
};

module_platform_driver(mpc_dma_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Piotr Ziecik <kosmo@semihalf.com>");
