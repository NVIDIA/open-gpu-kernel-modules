// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Driver for the Atmel AHB DMA Controller (aka HDMA or DMAC on AT91 systems)
 *
 * Copyright (C) 2008 Atmel Corporation
 *
 * This supports the Atmel AHB DMA Controller found in several Atmel SoCs.
 * The only Atmel DMA Controller that is not covered by this driver is the one
 * found on AT91SAM9263.
 */

#include <dt-bindings/dma/at91.h>
#include <linux/clk.h>
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <linux/dmapool.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_dma.h>

#include "at_hdmac_regs.h"
#include "dmaengine.h"

/*
 * Glossary
 * --------
 *
 * at_hdmac		: Name of the ATmel AHB DMA Controller
 * at_dma_ / atdma	: ATmel DMA controller entity related
 * atc_	/ atchan	: ATmel DMA Channel entity related
 */

#define	ATC_DEFAULT_CFG		(ATC_FIFOCFG_HALFFIFO)
#define	ATC_DEFAULT_CTRLB	(ATC_SIF(AT_DMA_MEM_IF) \
				|ATC_DIF(AT_DMA_MEM_IF))
#define ATC_DMA_BUSWIDTHS\
	(BIT(DMA_SLAVE_BUSWIDTH_UNDEFINED) |\
	BIT(DMA_SLAVE_BUSWIDTH_1_BYTE) |\
	BIT(DMA_SLAVE_BUSWIDTH_2_BYTES) |\
	BIT(DMA_SLAVE_BUSWIDTH_4_BYTES))

#define ATC_MAX_DSCR_TRIALS	10

/*
 * Initial number of descriptors to allocate for each channel. This could
 * be increased during dma usage.
 */
static unsigned int init_nr_desc_per_channel = 64;
module_param(init_nr_desc_per_channel, uint, 0644);
MODULE_PARM_DESC(init_nr_desc_per_channel,
		 "initial descriptors per channel (default: 64)");

/**
 * struct at_dma_platform_data - Controller configuration parameters
 * @nr_channels: Number of channels supported by hardware (max 8)
 * @cap_mask: dma_capability flags supported by the platform
 */
struct at_dma_platform_data {
	unsigned int	nr_channels;
	dma_cap_mask_t  cap_mask;
};

/**
 * struct at_dma_slave - Controller-specific information about a slave
 * @dma_dev: required DMA master device
 * @cfg: Platform-specific initializer for the CFG register
 */
struct at_dma_slave {
	struct device		*dma_dev;
	u32			cfg;
};

/* prototypes */
static dma_cookie_t atc_tx_submit(struct dma_async_tx_descriptor *tx);
static void atc_issue_pending(struct dma_chan *chan);


/*----------------------------------------------------------------------*/

static inline unsigned int atc_get_xfer_width(dma_addr_t src, dma_addr_t dst,
						size_t len)
{
	unsigned int width;

	if (!((src | dst  | len) & 3))
		width = 2;
	else if (!((src | dst | len) & 1))
		width = 1;
	else
		width = 0;

	return width;
}

static struct at_desc *atc_first_active(struct at_dma_chan *atchan)
{
	return list_first_entry(&atchan->active_list,
				struct at_desc, desc_node);
}

static struct at_desc *atc_first_queued(struct at_dma_chan *atchan)
{
	return list_first_entry(&atchan->queue,
				struct at_desc, desc_node);
}

/**
 * atc_alloc_descriptor - allocate and return an initialized descriptor
 * @chan: the channel to allocate descriptors for
 * @gfp_flags: GFP allocation flags
 *
 * Note: The ack-bit is positioned in the descriptor flag at creation time
 *       to make initial allocation more convenient. This bit will be cleared
 *       and control will be given to client at usage time (during
 *       preparation functions).
 */
static struct at_desc *atc_alloc_descriptor(struct dma_chan *chan,
					    gfp_t gfp_flags)
{
	struct at_desc	*desc = NULL;
	struct at_dma	*atdma = to_at_dma(chan->device);
	dma_addr_t phys;

	desc = dma_pool_zalloc(atdma->dma_desc_pool, gfp_flags, &phys);
	if (desc) {
		INIT_LIST_HEAD(&desc->tx_list);
		dma_async_tx_descriptor_init(&desc->txd, chan);
		/* txd.flags will be overwritten in prep functions */
		desc->txd.flags = DMA_CTRL_ACK;
		desc->txd.tx_submit = atc_tx_submit;
		desc->txd.phys = phys;
	}

	return desc;
}

/**
 * atc_desc_get - get an unused descriptor from free_list
 * @atchan: channel we want a new descriptor for
 */
static struct at_desc *atc_desc_get(struct at_dma_chan *atchan)
{
	struct at_desc *desc, *_desc;
	struct at_desc *ret = NULL;
	unsigned long flags;
	unsigned int i = 0;

	spin_lock_irqsave(&atchan->lock, flags);
	list_for_each_entry_safe(desc, _desc, &atchan->free_list, desc_node) {
		i++;
		if (async_tx_test_ack(&desc->txd)) {
			list_del(&desc->desc_node);
			ret = desc;
			break;
		}
		dev_dbg(chan2dev(&atchan->chan_common),
				"desc %p not ACKed\n", desc);
	}
	spin_unlock_irqrestore(&atchan->lock, flags);
	dev_vdbg(chan2dev(&atchan->chan_common),
		"scanned %u descriptors on freelist\n", i);

	/* no more descriptor available in initial pool: create one more */
	if (!ret)
		ret = atc_alloc_descriptor(&atchan->chan_common, GFP_NOWAIT);

	return ret;
}

/**
 * atc_desc_put - move a descriptor, including any children, to the free list
 * @atchan: channel we work on
 * @desc: descriptor, at the head of a chain, to move to free list
 */
static void atc_desc_put(struct at_dma_chan *atchan, struct at_desc *desc)
{
	if (desc) {
		struct at_desc *child;
		unsigned long flags;

		spin_lock_irqsave(&atchan->lock, flags);
		list_for_each_entry(child, &desc->tx_list, desc_node)
			dev_vdbg(chan2dev(&atchan->chan_common),
					"moving child desc %p to freelist\n",
					child);
		list_splice_init(&desc->tx_list, &atchan->free_list);
		dev_vdbg(chan2dev(&atchan->chan_common),
			 "moving desc %p to freelist\n", desc);
		list_add(&desc->desc_node, &atchan->free_list);
		spin_unlock_irqrestore(&atchan->lock, flags);
	}
}

/**
 * atc_desc_chain - build chain adding a descriptor
 * @first: address of first descriptor of the chain
 * @prev: address of previous descriptor of the chain
 * @desc: descriptor to queue
 *
 * Called from prep_* functions
 */
static void atc_desc_chain(struct at_desc **first, struct at_desc **prev,
			   struct at_desc *desc)
{
	if (!(*first)) {
		*first = desc;
	} else {
		/* inform the HW lli about chaining */
		(*prev)->lli.dscr = desc->txd.phys;
		/* insert the link descriptor to the LD ring */
		list_add_tail(&desc->desc_node,
				&(*first)->tx_list);
	}
	*prev = desc;
}

/**
 * atc_dostart - starts the DMA engine for real
 * @atchan: the channel we want to start
 * @first: first descriptor in the list we want to begin with
 *
 * Called with atchan->lock held and bh disabled
 */
static void atc_dostart(struct at_dma_chan *atchan, struct at_desc *first)
{
	struct at_dma	*atdma = to_at_dma(atchan->chan_common.device);

	/* ASSERT:  channel is idle */
	if (atc_chan_is_enabled(atchan)) {
		dev_err(chan2dev(&atchan->chan_common),
			"BUG: Attempted to start non-idle channel\n");
		dev_err(chan2dev(&atchan->chan_common),
			"  channel: s0x%x d0x%x ctrl0x%x:0x%x l0x%x\n",
			channel_readl(atchan, SADDR),
			channel_readl(atchan, DADDR),
			channel_readl(atchan, CTRLA),
			channel_readl(atchan, CTRLB),
			channel_readl(atchan, DSCR));

		/* The tasklet will hopefully advance the queue... */
		return;
	}

	vdbg_dump_regs(atchan);

	channel_writel(atchan, SADDR, 0);
	channel_writel(atchan, DADDR, 0);
	channel_writel(atchan, CTRLA, 0);
	channel_writel(atchan, CTRLB, 0);
	channel_writel(atchan, DSCR, first->txd.phys);
	channel_writel(atchan, SPIP, ATC_SPIP_HOLE(first->src_hole) |
		       ATC_SPIP_BOUNDARY(first->boundary));
	channel_writel(atchan, DPIP, ATC_DPIP_HOLE(first->dst_hole) |
		       ATC_DPIP_BOUNDARY(first->boundary));
	dma_writel(atdma, CHER, atchan->mask);

	vdbg_dump_regs(atchan);
}

/*
 * atc_get_desc_by_cookie - get the descriptor of a cookie
 * @atchan: the DMA channel
 * @cookie: the cookie to get the descriptor for
 */
static struct at_desc *atc_get_desc_by_cookie(struct at_dma_chan *atchan,
						dma_cookie_t cookie)
{
	struct at_desc *desc, *_desc;

	list_for_each_entry_safe(desc, _desc, &atchan->queue, desc_node) {
		if (desc->txd.cookie == cookie)
			return desc;
	}

	list_for_each_entry_safe(desc, _desc, &atchan->active_list, desc_node) {
		if (desc->txd.cookie == cookie)
			return desc;
	}

	return NULL;
}

/**
 * atc_calc_bytes_left - calculates the number of bytes left according to the
 * value read from CTRLA.
 *
 * @current_len: the number of bytes left before reading CTRLA
 * @ctrla: the value of CTRLA
 */
static inline int atc_calc_bytes_left(int current_len, u32 ctrla)
{
	u32 btsize = (ctrla & ATC_BTSIZE_MAX);
	u32 src_width = ATC_REG_TO_SRC_WIDTH(ctrla);

	/*
	 * According to the datasheet, when reading the Control A Register
	 * (ctrla), the Buffer Transfer Size (btsize) bitfield refers to the
	 * number of transfers completed on the Source Interface.
	 * So btsize is always a number of source width transfers.
	 */
	return current_len - (btsize << src_width);
}

/**
 * atc_get_bytes_left - get the number of bytes residue for a cookie
 * @chan: DMA channel
 * @cookie: transaction identifier to check status of
 */
static int atc_get_bytes_left(struct dma_chan *chan, dma_cookie_t cookie)
{
	struct at_dma_chan      *atchan = to_at_dma_chan(chan);
	struct at_desc *desc_first = atc_first_active(atchan);
	struct at_desc *desc;
	int ret;
	u32 ctrla, dscr, trials;

	/*
	 * If the cookie doesn't match to the currently running transfer then
	 * we can return the total length of the associated DMA transfer,
	 * because it is still queued.
	 */
	desc = atc_get_desc_by_cookie(atchan, cookie);
	if (desc == NULL)
		return -EINVAL;
	else if (desc != desc_first)
		return desc->total_len;

	/* cookie matches to the currently running transfer */
	ret = desc_first->total_len;

	if (desc_first->lli.dscr) {
		/* hardware linked list transfer */

		/*
		 * Calculate the residue by removing the length of the child
		 * descriptors already transferred from the total length.
		 * To get the current child descriptor we can use the value of
		 * the channel's DSCR register and compare it against the value
		 * of the hardware linked list structure of each child
		 * descriptor.
		 *
		 * The CTRLA register provides us with the amount of data
		 * already read from the source for the current child
		 * descriptor. So we can compute a more accurate residue by also
		 * removing the number of bytes corresponding to this amount of
		 * data.
		 *
		 * However, the DSCR and CTRLA registers cannot be read both
		 * atomically. Hence a race condition may occur: the first read
		 * register may refer to one child descriptor whereas the second
		 * read may refer to a later child descriptor in the list
		 * because of the DMA transfer progression inbetween the two
		 * reads.
		 *
		 * One solution could have been to pause the DMA transfer, read
		 * the DSCR and CTRLA then resume the DMA transfer. Nonetheless,
		 * this approach presents some drawbacks:
		 * - If the DMA transfer is paused, RX overruns or TX underruns
		 *   are more likey to occur depending on the system latency.
		 *   Taking the USART driver as an example, it uses a cyclic DMA
		 *   transfer to read data from the Receive Holding Register
		 *   (RHR) to avoid RX overruns since the RHR is not protected
		 *   by any FIFO on most Atmel SoCs. So pausing the DMA transfer
		 *   to compute the residue would break the USART driver design.
		 * - The atc_pause() function masks interrupts but we'd rather
		 *   avoid to do so for system latency purpose.
		 *
		 * Then we'd rather use another solution: the DSCR is read a
		 * first time, the CTRLA is read in turn, next the DSCR is read
		 * a second time. If the two consecutive read values of the DSCR
		 * are the same then we assume both refers to the very same
		 * child descriptor as well as the CTRLA value read inbetween
		 * does. For cyclic tranfers, the assumption is that a full loop
		 * is "not so fast".
		 * If the two DSCR values are different, we read again the CTRLA
		 * then the DSCR till two consecutive read values from DSCR are
		 * equal or till the maxium trials is reach.
		 * This algorithm is very unlikely not to find a stable value for
		 * DSCR.
		 */

		dscr = channel_readl(atchan, DSCR);
		rmb(); /* ensure DSCR is read before CTRLA */
		ctrla = channel_readl(atchan, CTRLA);
		for (trials = 0; trials < ATC_MAX_DSCR_TRIALS; ++trials) {
			u32 new_dscr;

			rmb(); /* ensure DSCR is read after CTRLA */
			new_dscr = channel_readl(atchan, DSCR);

			/*
			 * If the DSCR register value has not changed inside the
			 * DMA controller since the previous read, we assume
			 * that both the dscr and ctrla values refers to the
			 * very same descriptor.
			 */
			if (likely(new_dscr == dscr))
				break;

			/*
			 * DSCR has changed inside the DMA controller, so the
			 * previouly read value of CTRLA may refer to an already
			 * processed descriptor hence could be outdated.
			 * We need to update ctrla to match the current
			 * descriptor.
			 */
			dscr = new_dscr;
			rmb(); /* ensure DSCR is read before CTRLA */
			ctrla = channel_readl(atchan, CTRLA);
		}
		if (unlikely(trials >= ATC_MAX_DSCR_TRIALS))
			return -ETIMEDOUT;

		/* for the first descriptor we can be more accurate */
		if (desc_first->lli.dscr == dscr)
			return atc_calc_bytes_left(ret, ctrla);

		ret -= desc_first->len;
		list_for_each_entry(desc, &desc_first->tx_list, desc_node) {
			if (desc->lli.dscr == dscr)
				break;

			ret -= desc->len;
		}

		/*
		 * For the current descriptor in the chain we can calculate
		 * the remaining bytes using the channel's register.
		 */
		ret = atc_calc_bytes_left(ret, ctrla);
	} else {
		/* single transfer */
		ctrla = channel_readl(atchan, CTRLA);
		ret = atc_calc_bytes_left(ret, ctrla);
	}

	return ret;
}

/**
 * atc_chain_complete - finish work for one transaction chain
 * @atchan: channel we work on
 * @desc: descriptor at the head of the chain we want do complete
 */
static void
atc_chain_complete(struct at_dma_chan *atchan, struct at_desc *desc)
{
	struct dma_async_tx_descriptor	*txd = &desc->txd;
	struct at_dma			*atdma = to_at_dma(atchan->chan_common.device);
	unsigned long flags;

	dev_vdbg(chan2dev(&atchan->chan_common),
		"descriptor %u complete\n", txd->cookie);

	spin_lock_irqsave(&atchan->lock, flags);

	/* mark the descriptor as complete for non cyclic cases only */
	if (!atc_chan_is_cyclic(atchan))
		dma_cookie_complete(txd);

	/* If the transfer was a memset, free our temporary buffer */
	if (desc->memset_buffer) {
		dma_pool_free(atdma->memset_pool, desc->memset_vaddr,
			      desc->memset_paddr);
		desc->memset_buffer = false;
	}

	/* move children to free_list */
	list_splice_init(&desc->tx_list, &atchan->free_list);
	/* move myself to free_list */
	list_move(&desc->desc_node, &atchan->free_list);

	spin_unlock_irqrestore(&atchan->lock, flags);

	dma_descriptor_unmap(txd);
	/* for cyclic transfers,
	 * no need to replay callback function while stopping */
	if (!atc_chan_is_cyclic(atchan))
		dmaengine_desc_get_callback_invoke(txd, NULL);

	dma_run_dependencies(txd);
}

/**
 * atc_complete_all - finish work for all transactions
 * @atchan: channel to complete transactions for
 *
 * Eventually submit queued descriptors if any
 *
 * Assume channel is idle while calling this function
 * Called with atchan->lock held and bh disabled
 */
static void atc_complete_all(struct at_dma_chan *atchan)
{
	struct at_desc *desc, *_desc;
	LIST_HEAD(list);
	unsigned long flags;

	dev_vdbg(chan2dev(&atchan->chan_common), "complete all\n");

	spin_lock_irqsave(&atchan->lock, flags);

	/*
	 * Submit queued descriptors ASAP, i.e. before we go through
	 * the completed ones.
	 */
	if (!list_empty(&atchan->queue))
		atc_dostart(atchan, atc_first_queued(atchan));
	/* empty active_list now it is completed */
	list_splice_init(&atchan->active_list, &list);
	/* empty queue list by moving descriptors (if any) to active_list */
	list_splice_init(&atchan->queue, &atchan->active_list);

	spin_unlock_irqrestore(&atchan->lock, flags);

	list_for_each_entry_safe(desc, _desc, &list, desc_node)
		atc_chain_complete(atchan, desc);
}

/**
 * atc_advance_work - at the end of a transaction, move forward
 * @atchan: channel where the transaction ended
 */
static void atc_advance_work(struct at_dma_chan *atchan)
{
	unsigned long flags;
	int ret;

	dev_vdbg(chan2dev(&atchan->chan_common), "advance_work\n");

	spin_lock_irqsave(&atchan->lock, flags);
	ret = atc_chan_is_enabled(atchan);
	spin_unlock_irqrestore(&atchan->lock, flags);
	if (ret)
		return;

	if (list_empty(&atchan->active_list) ||
	    list_is_singular(&atchan->active_list))
		return atc_complete_all(atchan);

	atc_chain_complete(atchan, atc_first_active(atchan));

	/* advance work */
	spin_lock_irqsave(&atchan->lock, flags);
	atc_dostart(atchan, atc_first_active(atchan));
	spin_unlock_irqrestore(&atchan->lock, flags);
}


/**
 * atc_handle_error - handle errors reported by DMA controller
 * @atchan: channel where error occurs
 */
static void atc_handle_error(struct at_dma_chan *atchan)
{
	struct at_desc *bad_desc;
	struct at_desc *child;
	unsigned long flags;

	spin_lock_irqsave(&atchan->lock, flags);
	/*
	 * The descriptor currently at the head of the active list is
	 * broked. Since we don't have any way to report errors, we'll
	 * just have to scream loudly and try to carry on.
	 */
	bad_desc = atc_first_active(atchan);
	list_del_init(&bad_desc->desc_node);

	/* As we are stopped, take advantage to push queued descriptors
	 * in active_list */
	list_splice_init(&atchan->queue, atchan->active_list.prev);

	/* Try to restart the controller */
	if (!list_empty(&atchan->active_list))
		atc_dostart(atchan, atc_first_active(atchan));

	/*
	 * KERN_CRITICAL may seem harsh, but since this only happens
	 * when someone submits a bad physical address in a
	 * descriptor, we should consider ourselves lucky that the
	 * controller flagged an error instead of scribbling over
	 * random memory locations.
	 */
	dev_crit(chan2dev(&atchan->chan_common),
			"Bad descriptor submitted for DMA!\n");
	dev_crit(chan2dev(&atchan->chan_common),
			"  cookie: %d\n", bad_desc->txd.cookie);
	atc_dump_lli(atchan, &bad_desc->lli);
	list_for_each_entry(child, &bad_desc->tx_list, desc_node)
		atc_dump_lli(atchan, &child->lli);

	spin_unlock_irqrestore(&atchan->lock, flags);

	/* Pretend the descriptor completed successfully */
	atc_chain_complete(atchan, bad_desc);
}

/**
 * atc_handle_cyclic - at the end of a period, run callback function
 * @atchan: channel used for cyclic operations
 */
static void atc_handle_cyclic(struct at_dma_chan *atchan)
{
	struct at_desc			*first = atc_first_active(atchan);
	struct dma_async_tx_descriptor	*txd = &first->txd;

	dev_vdbg(chan2dev(&atchan->chan_common),
			"new cyclic period llp 0x%08x\n",
			channel_readl(atchan, DSCR));

	dmaengine_desc_get_callback_invoke(txd, NULL);
}

/*--  IRQ & Tasklet  ---------------------------------------------------*/

static void atc_tasklet(struct tasklet_struct *t)
{
	struct at_dma_chan *atchan = from_tasklet(atchan, t, tasklet);

	if (test_and_clear_bit(ATC_IS_ERROR, &atchan->status))
		return atc_handle_error(atchan);

	if (atc_chan_is_cyclic(atchan))
		return atc_handle_cyclic(atchan);

	atc_advance_work(atchan);
}

static irqreturn_t at_dma_interrupt(int irq, void *dev_id)
{
	struct at_dma		*atdma = (struct at_dma *)dev_id;
	struct at_dma_chan	*atchan;
	int			i;
	u32			status, pending, imr;
	int			ret = IRQ_NONE;

	do {
		imr = dma_readl(atdma, EBCIMR);
		status = dma_readl(atdma, EBCISR);
		pending = status & imr;

		if (!pending)
			break;

		dev_vdbg(atdma->dma_common.dev,
			"interrupt: status = 0x%08x, 0x%08x, 0x%08x\n",
			 status, imr, pending);

		for (i = 0; i < atdma->dma_common.chancnt; i++) {
			atchan = &atdma->chan[i];
			if (pending & (AT_DMA_BTC(i) | AT_DMA_ERR(i))) {
				if (pending & AT_DMA_ERR(i)) {
					/* Disable channel on AHB error */
					dma_writel(atdma, CHDR,
						AT_DMA_RES(i) | atchan->mask);
					/* Give information to tasklet */
					set_bit(ATC_IS_ERROR, &atchan->status);
				}
				tasklet_schedule(&atchan->tasklet);
				ret = IRQ_HANDLED;
			}
		}

	} while (pending);

	return ret;
}


/*--  DMA Engine API  --------------------------------------------------*/

/**
 * atc_tx_submit - set the prepared descriptor(s) to be executed by the engine
 * @tx: descriptor at the head of the transaction chain
 *
 * Queue chain if DMA engine is working already
 *
 * Cookie increment and adding to active_list or queue must be atomic
 */
static dma_cookie_t atc_tx_submit(struct dma_async_tx_descriptor *tx)
{
	struct at_desc		*desc = txd_to_at_desc(tx);
	struct at_dma_chan	*atchan = to_at_dma_chan(tx->chan);
	dma_cookie_t		cookie;
	unsigned long		flags;

	spin_lock_irqsave(&atchan->lock, flags);
	cookie = dma_cookie_assign(tx);

	if (list_empty(&atchan->active_list)) {
		dev_vdbg(chan2dev(tx->chan), "tx_submit: started %u\n",
				desc->txd.cookie);
		atc_dostart(atchan, desc);
		list_add_tail(&desc->desc_node, &atchan->active_list);
	} else {
		dev_vdbg(chan2dev(tx->chan), "tx_submit: queued %u\n",
				desc->txd.cookie);
		list_add_tail(&desc->desc_node, &atchan->queue);
	}

	spin_unlock_irqrestore(&atchan->lock, flags);

	return cookie;
}

/**
 * atc_prep_dma_interleaved - prepare memory to memory interleaved operation
 * @chan: the channel to prepare operation on
 * @xt: Interleaved transfer template
 * @flags: tx descriptor status flags
 */
static struct dma_async_tx_descriptor *
atc_prep_dma_interleaved(struct dma_chan *chan,
			 struct dma_interleaved_template *xt,
			 unsigned long flags)
{
	struct at_dma_chan	*atchan = to_at_dma_chan(chan);
	struct data_chunk	*first;
	struct at_desc		*desc = NULL;
	size_t			xfer_count;
	unsigned int		dwidth;
	u32			ctrla;
	u32			ctrlb;
	size_t			len = 0;
	int			i;

	if (unlikely(!xt || xt->numf != 1 || !xt->frame_size))
		return NULL;

	first = xt->sgl;

	dev_info(chan2dev(chan),
		 "%s: src=%pad, dest=%pad, numf=%d, frame_size=%d, flags=0x%lx\n",
		__func__, &xt->src_start, &xt->dst_start, xt->numf,
		xt->frame_size, flags);

	/*
	 * The controller can only "skip" X bytes every Y bytes, so we
	 * need to make sure we are given a template that fit that
	 * description, ie a template with chunks that always have the
	 * same size, with the same ICGs.
	 */
	for (i = 0; i < xt->frame_size; i++) {
		struct data_chunk *chunk = xt->sgl + i;

		if ((chunk->size != xt->sgl->size) ||
		    (dmaengine_get_dst_icg(xt, chunk) != dmaengine_get_dst_icg(xt, first)) ||
		    (dmaengine_get_src_icg(xt, chunk) != dmaengine_get_src_icg(xt, first))) {
			dev_err(chan2dev(chan),
				"%s: the controller can transfer only identical chunks\n",
				__func__);
			return NULL;
		}

		len += chunk->size;
	}

	dwidth = atc_get_xfer_width(xt->src_start,
				    xt->dst_start, len);

	xfer_count = len >> dwidth;
	if (xfer_count > ATC_BTSIZE_MAX) {
		dev_err(chan2dev(chan), "%s: buffer is too big\n", __func__);
		return NULL;
	}

	ctrla = ATC_SRC_WIDTH(dwidth) |
		ATC_DST_WIDTH(dwidth);

	ctrlb =   ATC_DEFAULT_CTRLB | ATC_IEN
		| ATC_SRC_ADDR_MODE_INCR
		| ATC_DST_ADDR_MODE_INCR
		| ATC_SRC_PIP
		| ATC_DST_PIP
		| ATC_FC_MEM2MEM;

	/* create the transfer */
	desc = atc_desc_get(atchan);
	if (!desc) {
		dev_err(chan2dev(chan),
			"%s: couldn't allocate our descriptor\n", __func__);
		return NULL;
	}

	desc->lli.saddr = xt->src_start;
	desc->lli.daddr = xt->dst_start;
	desc->lli.ctrla = ctrla | xfer_count;
	desc->lli.ctrlb = ctrlb;

	desc->boundary = first->size >> dwidth;
	desc->dst_hole = (dmaengine_get_dst_icg(xt, first) >> dwidth) + 1;
	desc->src_hole = (dmaengine_get_src_icg(xt, first) >> dwidth) + 1;

	desc->txd.cookie = -EBUSY;
	desc->total_len = desc->len = len;

	/* set end-of-link to the last link descriptor of list*/
	set_desc_eol(desc);

	desc->txd.flags = flags; /* client is in control of this ack */

	return &desc->txd;
}

/**
 * atc_prep_dma_memcpy - prepare a memcpy operation
 * @chan: the channel to prepare operation on
 * @dest: operation virtual destination address
 * @src: operation virtual source address
 * @len: operation length
 * @flags: tx descriptor status flags
 */
static struct dma_async_tx_descriptor *
atc_prep_dma_memcpy(struct dma_chan *chan, dma_addr_t dest, dma_addr_t src,
		size_t len, unsigned long flags)
{
	struct at_dma_chan	*atchan = to_at_dma_chan(chan);
	struct at_desc		*desc = NULL;
	struct at_desc		*first = NULL;
	struct at_desc		*prev = NULL;
	size_t			xfer_count;
	size_t			offset;
	unsigned int		src_width;
	unsigned int		dst_width;
	u32			ctrla;
	u32			ctrlb;

	dev_vdbg(chan2dev(chan), "prep_dma_memcpy: d%pad s%pad l0x%zx f0x%lx\n",
			&dest, &src, len, flags);

	if (unlikely(!len)) {
		dev_dbg(chan2dev(chan), "prep_dma_memcpy: length is zero!\n");
		return NULL;
	}

	ctrlb =   ATC_DEFAULT_CTRLB | ATC_IEN
		| ATC_SRC_ADDR_MODE_INCR
		| ATC_DST_ADDR_MODE_INCR
		| ATC_FC_MEM2MEM;

	/*
	 * We can be a lot more clever here, but this should take care
	 * of the most common optimization.
	 */
	src_width = dst_width = atc_get_xfer_width(src, dest, len);

	ctrla = ATC_SRC_WIDTH(src_width) |
		ATC_DST_WIDTH(dst_width);

	for (offset = 0; offset < len; offset += xfer_count << src_width) {
		xfer_count = min_t(size_t, (len - offset) >> src_width,
				ATC_BTSIZE_MAX);

		desc = atc_desc_get(atchan);
		if (!desc)
			goto err_desc_get;

		desc->lli.saddr = src + offset;
		desc->lli.daddr = dest + offset;
		desc->lli.ctrla = ctrla | xfer_count;
		desc->lli.ctrlb = ctrlb;

		desc->txd.cookie = 0;
		desc->len = xfer_count << src_width;

		atc_desc_chain(&first, &prev, desc);
	}

	/* First descriptor of the chain embedds additional information */
	first->txd.cookie = -EBUSY;
	first->total_len = len;

	/* set end-of-link to the last link descriptor of list*/
	set_desc_eol(desc);

	first->txd.flags = flags; /* client is in control of this ack */

	return &first->txd;

err_desc_get:
	atc_desc_put(atchan, first);
	return NULL;
}

static struct at_desc *atc_create_memset_desc(struct dma_chan *chan,
					      dma_addr_t psrc,
					      dma_addr_t pdst,
					      size_t len)
{
	struct at_dma_chan *atchan = to_at_dma_chan(chan);
	struct at_desc *desc;
	size_t xfer_count;

	u32 ctrla = ATC_SRC_WIDTH(2) | ATC_DST_WIDTH(2);
	u32 ctrlb = ATC_DEFAULT_CTRLB | ATC_IEN |
		ATC_SRC_ADDR_MODE_FIXED |
		ATC_DST_ADDR_MODE_INCR |
		ATC_FC_MEM2MEM;

	xfer_count = len >> 2;
	if (xfer_count > ATC_BTSIZE_MAX) {
		dev_err(chan2dev(chan), "%s: buffer is too big\n",
			__func__);
		return NULL;
	}

	desc = atc_desc_get(atchan);
	if (!desc) {
		dev_err(chan2dev(chan), "%s: can't get a descriptor\n",
			__func__);
		return NULL;
	}

	desc->lli.saddr = psrc;
	desc->lli.daddr = pdst;
	desc->lli.ctrla = ctrla | xfer_count;
	desc->lli.ctrlb = ctrlb;

	desc->txd.cookie = 0;
	desc->len = len;

	return desc;
}

/**
 * atc_prep_dma_memset - prepare a memcpy operation
 * @chan: the channel to prepare operation on
 * @dest: operation virtual destination address
 * @value: value to set memory buffer to
 * @len: operation length
 * @flags: tx descriptor status flags
 */
static struct dma_async_tx_descriptor *
atc_prep_dma_memset(struct dma_chan *chan, dma_addr_t dest, int value,
		    size_t len, unsigned long flags)
{
	struct at_dma		*atdma = to_at_dma(chan->device);
	struct at_desc		*desc;
	void __iomem		*vaddr;
	dma_addr_t		paddr;

	dev_vdbg(chan2dev(chan), "%s: d%pad v0x%x l0x%zx f0x%lx\n", __func__,
		&dest, value, len, flags);

	if (unlikely(!len)) {
		dev_dbg(chan2dev(chan), "%s: length is zero!\n", __func__);
		return NULL;
	}

	if (!is_dma_fill_aligned(chan->device, dest, 0, len)) {
		dev_dbg(chan2dev(chan), "%s: buffer is not aligned\n",
			__func__);
		return NULL;
	}

	vaddr = dma_pool_alloc(atdma->memset_pool, GFP_NOWAIT, &paddr);
	if (!vaddr) {
		dev_err(chan2dev(chan), "%s: couldn't allocate buffer\n",
			__func__);
		return NULL;
	}
	*(u32*)vaddr = value;

	desc = atc_create_memset_desc(chan, paddr, dest, len);
	if (!desc) {
		dev_err(chan2dev(chan), "%s: couldn't get a descriptor\n",
			__func__);
		goto err_free_buffer;
	}

	desc->memset_paddr = paddr;
	desc->memset_vaddr = vaddr;
	desc->memset_buffer = true;

	desc->txd.cookie = -EBUSY;
	desc->total_len = len;

	/* set end-of-link on the descriptor */
	set_desc_eol(desc);

	desc->txd.flags = flags;

	return &desc->txd;

err_free_buffer:
	dma_pool_free(atdma->memset_pool, vaddr, paddr);
	return NULL;
}

static struct dma_async_tx_descriptor *
atc_prep_dma_memset_sg(struct dma_chan *chan,
		       struct scatterlist *sgl,
		       unsigned int sg_len, int value,
		       unsigned long flags)
{
	struct at_dma_chan	*atchan = to_at_dma_chan(chan);
	struct at_dma		*atdma = to_at_dma(chan->device);
	struct at_desc		*desc = NULL, *first = NULL, *prev = NULL;
	struct scatterlist	*sg;
	void __iomem		*vaddr;
	dma_addr_t		paddr;
	size_t			total_len = 0;
	int			i;

	dev_vdbg(chan2dev(chan), "%s: v0x%x l0x%zx f0x%lx\n", __func__,
		 value, sg_len, flags);

	if (unlikely(!sgl || !sg_len)) {
		dev_dbg(chan2dev(chan), "%s: scatterlist is empty!\n",
			__func__);
		return NULL;
	}

	vaddr = dma_pool_alloc(atdma->memset_pool, GFP_NOWAIT, &paddr);
	if (!vaddr) {
		dev_err(chan2dev(chan), "%s: couldn't allocate buffer\n",
			__func__);
		return NULL;
	}
	*(u32*)vaddr = value;

	for_each_sg(sgl, sg, sg_len, i) {
		dma_addr_t dest = sg_dma_address(sg);
		size_t len = sg_dma_len(sg);

		dev_vdbg(chan2dev(chan), "%s: d%pad, l0x%zx\n",
			 __func__, &dest, len);

		if (!is_dma_fill_aligned(chan->device, dest, 0, len)) {
			dev_err(chan2dev(chan), "%s: buffer is not aligned\n",
				__func__);
			goto err_put_desc;
		}

		desc = atc_create_memset_desc(chan, paddr, dest, len);
		if (!desc)
			goto err_put_desc;

		atc_desc_chain(&first, &prev, desc);

		total_len += len;
	}

	/*
	 * Only set the buffer pointers on the last descriptor to
	 * avoid free'ing while we have our transfer still going
	 */
	desc->memset_paddr = paddr;
	desc->memset_vaddr = vaddr;
	desc->memset_buffer = true;

	first->txd.cookie = -EBUSY;
	first->total_len = total_len;

	/* set end-of-link on the descriptor */
	set_desc_eol(desc);

	first->txd.flags = flags;

	return &first->txd;

err_put_desc:
	atc_desc_put(atchan, first);
	return NULL;
}

/**
 * atc_prep_slave_sg - prepare descriptors for a DMA_SLAVE transaction
 * @chan: DMA channel
 * @sgl: scatterlist to transfer to/from
 * @sg_len: number of entries in @scatterlist
 * @direction: DMA direction
 * @flags: tx descriptor status flags
 * @context: transaction context (ignored)
 */
static struct dma_async_tx_descriptor *
atc_prep_slave_sg(struct dma_chan *chan, struct scatterlist *sgl,
		unsigned int sg_len, enum dma_transfer_direction direction,
		unsigned long flags, void *context)
{
	struct at_dma_chan	*atchan = to_at_dma_chan(chan);
	struct at_dma_slave	*atslave = chan->private;
	struct dma_slave_config	*sconfig = &atchan->dma_sconfig;
	struct at_desc		*first = NULL;
	struct at_desc		*prev = NULL;
	u32			ctrla;
	u32			ctrlb;
	dma_addr_t		reg;
	unsigned int		reg_width;
	unsigned int		mem_width;
	unsigned int		i;
	struct scatterlist	*sg;
	size_t			total_len = 0;

	dev_vdbg(chan2dev(chan), "prep_slave_sg (%d): %s f0x%lx\n",
			sg_len,
			direction == DMA_MEM_TO_DEV ? "TO DEVICE" : "FROM DEVICE",
			flags);

	if (unlikely(!atslave || !sg_len)) {
		dev_dbg(chan2dev(chan), "prep_slave_sg: sg length is zero!\n");
		return NULL;
	}

	ctrla =   ATC_SCSIZE(sconfig->src_maxburst)
		| ATC_DCSIZE(sconfig->dst_maxburst);
	ctrlb = ATC_IEN;

	switch (direction) {
	case DMA_MEM_TO_DEV:
		reg_width = convert_buswidth(sconfig->dst_addr_width);
		ctrla |=  ATC_DST_WIDTH(reg_width);
		ctrlb |=  ATC_DST_ADDR_MODE_FIXED
			| ATC_SRC_ADDR_MODE_INCR
			| ATC_FC_MEM2PER
			| ATC_SIF(atchan->mem_if) | ATC_DIF(atchan->per_if);
		reg = sconfig->dst_addr;
		for_each_sg(sgl, sg, sg_len, i) {
			struct at_desc	*desc;
			u32		len;
			u32		mem;

			desc = atc_desc_get(atchan);
			if (!desc)
				goto err_desc_get;

			mem = sg_dma_address(sg);
			len = sg_dma_len(sg);
			if (unlikely(!len)) {
				dev_dbg(chan2dev(chan),
					"prep_slave_sg: sg(%d) data length is zero\n", i);
				goto err;
			}
			mem_width = 2;
			if (unlikely(mem & 3 || len & 3))
				mem_width = 0;

			desc->lli.saddr = mem;
			desc->lli.daddr = reg;
			desc->lli.ctrla = ctrla
					| ATC_SRC_WIDTH(mem_width)
					| len >> mem_width;
			desc->lli.ctrlb = ctrlb;
			desc->len = len;

			atc_desc_chain(&first, &prev, desc);
			total_len += len;
		}
		break;
	case DMA_DEV_TO_MEM:
		reg_width = convert_buswidth(sconfig->src_addr_width);
		ctrla |=  ATC_SRC_WIDTH(reg_width);
		ctrlb |=  ATC_DST_ADDR_MODE_INCR
			| ATC_SRC_ADDR_MODE_FIXED
			| ATC_FC_PER2MEM
			| ATC_SIF(atchan->per_if) | ATC_DIF(atchan->mem_if);

		reg = sconfig->src_addr;
		for_each_sg(sgl, sg, sg_len, i) {
			struct at_desc	*desc;
			u32		len;
			u32		mem;

			desc = atc_desc_get(atchan);
			if (!desc)
				goto err_desc_get;

			mem = sg_dma_address(sg);
			len = sg_dma_len(sg);
			if (unlikely(!len)) {
				dev_dbg(chan2dev(chan),
					"prep_slave_sg: sg(%d) data length is zero\n", i);
				goto err;
			}
			mem_width = 2;
			if (unlikely(mem & 3 || len & 3))
				mem_width = 0;

			desc->lli.saddr = reg;
			desc->lli.daddr = mem;
			desc->lli.ctrla = ctrla
					| ATC_DST_WIDTH(mem_width)
					| len >> reg_width;
			desc->lli.ctrlb = ctrlb;
			desc->len = len;

			atc_desc_chain(&first, &prev, desc);
			total_len += len;
		}
		break;
	default:
		return NULL;
	}

	/* set end-of-link to the last link descriptor of list*/
	set_desc_eol(prev);

	/* First descriptor of the chain embedds additional information */
	first->txd.cookie = -EBUSY;
	first->total_len = total_len;

	/* first link descriptor of list is responsible of flags */
	first->txd.flags = flags; /* client is in control of this ack */

	return &first->txd;

err_desc_get:
	dev_err(chan2dev(chan), "not enough descriptors available\n");
err:
	atc_desc_put(atchan, first);
	return NULL;
}

/*
 * atc_dma_cyclic_check_values
 * Check for too big/unaligned periods and unaligned DMA buffer
 */
static int
atc_dma_cyclic_check_values(unsigned int reg_width, dma_addr_t buf_addr,
		size_t period_len)
{
	if (period_len > (ATC_BTSIZE_MAX << reg_width))
		goto err_out;
	if (unlikely(period_len & ((1 << reg_width) - 1)))
		goto err_out;
	if (unlikely(buf_addr & ((1 << reg_width) - 1)))
		goto err_out;

	return 0;

err_out:
	return -EINVAL;
}

/*
 * atc_dma_cyclic_fill_desc - Fill one period descriptor
 */
static int
atc_dma_cyclic_fill_desc(struct dma_chan *chan, struct at_desc *desc,
		unsigned int period_index, dma_addr_t buf_addr,
		unsigned int reg_width, size_t period_len,
		enum dma_transfer_direction direction)
{
	struct at_dma_chan	*atchan = to_at_dma_chan(chan);
	struct dma_slave_config	*sconfig = &atchan->dma_sconfig;
	u32			ctrla;

	/* prepare common CRTLA value */
	ctrla =   ATC_SCSIZE(sconfig->src_maxburst)
		| ATC_DCSIZE(sconfig->dst_maxburst)
		| ATC_DST_WIDTH(reg_width)
		| ATC_SRC_WIDTH(reg_width)
		| period_len >> reg_width;

	switch (direction) {
	case DMA_MEM_TO_DEV:
		desc->lli.saddr = buf_addr + (period_len * period_index);
		desc->lli.daddr = sconfig->dst_addr;
		desc->lli.ctrla = ctrla;
		desc->lli.ctrlb = ATC_DST_ADDR_MODE_FIXED
				| ATC_SRC_ADDR_MODE_INCR
				| ATC_FC_MEM2PER
				| ATC_SIF(atchan->mem_if)
				| ATC_DIF(atchan->per_if);
		desc->len = period_len;
		break;

	case DMA_DEV_TO_MEM:
		desc->lli.saddr = sconfig->src_addr;
		desc->lli.daddr = buf_addr + (period_len * period_index);
		desc->lli.ctrla = ctrla;
		desc->lli.ctrlb = ATC_DST_ADDR_MODE_INCR
				| ATC_SRC_ADDR_MODE_FIXED
				| ATC_FC_PER2MEM
				| ATC_SIF(atchan->per_if)
				| ATC_DIF(atchan->mem_if);
		desc->len = period_len;
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * atc_prep_dma_cyclic - prepare the cyclic DMA transfer
 * @chan: the DMA channel to prepare
 * @buf_addr: physical DMA address where the buffer starts
 * @buf_len: total number of bytes for the entire buffer
 * @period_len: number of bytes for each period
 * @direction: transfer direction, to or from device
 * @flags: tx descriptor status flags
 */
static struct dma_async_tx_descriptor *
atc_prep_dma_cyclic(struct dma_chan *chan, dma_addr_t buf_addr, size_t buf_len,
		size_t period_len, enum dma_transfer_direction direction,
		unsigned long flags)
{
	struct at_dma_chan	*atchan = to_at_dma_chan(chan);
	struct at_dma_slave	*atslave = chan->private;
	struct dma_slave_config	*sconfig = &atchan->dma_sconfig;
	struct at_desc		*first = NULL;
	struct at_desc		*prev = NULL;
	unsigned long		was_cyclic;
	unsigned int		reg_width;
	unsigned int		periods = buf_len / period_len;
	unsigned int		i;

	dev_vdbg(chan2dev(chan), "prep_dma_cyclic: %s buf@%pad - %d (%d/%d)\n",
			direction == DMA_MEM_TO_DEV ? "TO DEVICE" : "FROM DEVICE",
			&buf_addr,
			periods, buf_len, period_len);

	if (unlikely(!atslave || !buf_len || !period_len)) {
		dev_dbg(chan2dev(chan), "prep_dma_cyclic: length is zero!\n");
		return NULL;
	}

	was_cyclic = test_and_set_bit(ATC_IS_CYCLIC, &atchan->status);
	if (was_cyclic) {
		dev_dbg(chan2dev(chan), "prep_dma_cyclic: channel in use!\n");
		return NULL;
	}

	if (unlikely(!is_slave_direction(direction)))
		goto err_out;

	if (direction == DMA_MEM_TO_DEV)
		reg_width = convert_buswidth(sconfig->dst_addr_width);
	else
		reg_width = convert_buswidth(sconfig->src_addr_width);

	/* Check for too big/unaligned periods and unaligned DMA buffer */
	if (atc_dma_cyclic_check_values(reg_width, buf_addr, period_len))
		goto err_out;

	/* build cyclic linked list */
	for (i = 0; i < periods; i++) {
		struct at_desc	*desc;

		desc = atc_desc_get(atchan);
		if (!desc)
			goto err_desc_get;

		if (atc_dma_cyclic_fill_desc(chan, desc, i, buf_addr,
					     reg_width, period_len, direction))
			goto err_desc_get;

		atc_desc_chain(&first, &prev, desc);
	}

	/* lets make a cyclic list */
	prev->lli.dscr = first->txd.phys;

	/* First descriptor of the chain embedds additional information */
	first->txd.cookie = -EBUSY;
	first->total_len = buf_len;

	return &first->txd;

err_desc_get:
	dev_err(chan2dev(chan), "not enough descriptors available\n");
	atc_desc_put(atchan, first);
err_out:
	clear_bit(ATC_IS_CYCLIC, &atchan->status);
	return NULL;
}

static int atc_config(struct dma_chan *chan,
		      struct dma_slave_config *sconfig)
{
	struct at_dma_chan	*atchan = to_at_dma_chan(chan);

	dev_vdbg(chan2dev(chan), "%s\n", __func__);

	/* Check if it is chan is configured for slave transfers */
	if (!chan->private)
		return -EINVAL;

	memcpy(&atchan->dma_sconfig, sconfig, sizeof(*sconfig));

	convert_burst(&atchan->dma_sconfig.src_maxburst);
	convert_burst(&atchan->dma_sconfig.dst_maxburst);

	return 0;
}

static int atc_pause(struct dma_chan *chan)
{
	struct at_dma_chan	*atchan = to_at_dma_chan(chan);
	struct at_dma		*atdma = to_at_dma(chan->device);
	int			chan_id = atchan->chan_common.chan_id;
	unsigned long		flags;

	dev_vdbg(chan2dev(chan), "%s\n", __func__);

	spin_lock_irqsave(&atchan->lock, flags);

	dma_writel(atdma, CHER, AT_DMA_SUSP(chan_id));
	set_bit(ATC_IS_PAUSED, &atchan->status);

	spin_unlock_irqrestore(&atchan->lock, flags);

	return 0;
}

static int atc_resume(struct dma_chan *chan)
{
	struct at_dma_chan	*atchan = to_at_dma_chan(chan);
	struct at_dma		*atdma = to_at_dma(chan->device);
	int			chan_id = atchan->chan_common.chan_id;
	unsigned long		flags;

	dev_vdbg(chan2dev(chan), "%s\n", __func__);

	if (!atc_chan_is_paused(atchan))
		return 0;

	spin_lock_irqsave(&atchan->lock, flags);

	dma_writel(atdma, CHDR, AT_DMA_RES(chan_id));
	clear_bit(ATC_IS_PAUSED, &atchan->status);

	spin_unlock_irqrestore(&atchan->lock, flags);

	return 0;
}

static int atc_terminate_all(struct dma_chan *chan)
{
	struct at_dma_chan	*atchan = to_at_dma_chan(chan);
	struct at_dma		*atdma = to_at_dma(chan->device);
	int			chan_id = atchan->chan_common.chan_id;
	struct at_desc		*desc, *_desc;
	unsigned long		flags;

	LIST_HEAD(list);

	dev_vdbg(chan2dev(chan), "%s\n", __func__);

	/*
	 * This is only called when something went wrong elsewhere, so
	 * we don't really care about the data. Just disable the
	 * channel. We still have to poll the channel enable bit due
	 * to AHB/HSB limitations.
	 */
	spin_lock_irqsave(&atchan->lock, flags);

	/* disabling channel: must also remove suspend state */
	dma_writel(atdma, CHDR, AT_DMA_RES(chan_id) | atchan->mask);

	/* confirm that this channel is disabled */
	while (dma_readl(atdma, CHSR) & atchan->mask)
		cpu_relax();

	/* active_list entries will end up before queued entries */
	list_splice_init(&atchan->queue, &list);
	list_splice_init(&atchan->active_list, &list);

	spin_unlock_irqrestore(&atchan->lock, flags);

	/* Flush all pending and queued descriptors */
	list_for_each_entry_safe(desc, _desc, &list, desc_node)
		atc_chain_complete(atchan, desc);

	clear_bit(ATC_IS_PAUSED, &atchan->status);
	/* if channel dedicated to cyclic operations, free it */
	clear_bit(ATC_IS_CYCLIC, &atchan->status);

	return 0;
}

/**
 * atc_tx_status - poll for transaction completion
 * @chan: DMA channel
 * @cookie: transaction identifier to check status of
 * @txstate: if not %NULL updated with transaction state
 *
 * If @txstate is passed in, upon return it reflect the driver
 * internal state and can be used with dma_async_is_complete() to check
 * the status of multiple cookies without re-checking hardware state.
 */
static enum dma_status
atc_tx_status(struct dma_chan *chan,
		dma_cookie_t cookie,
		struct dma_tx_state *txstate)
{
	struct at_dma_chan	*atchan = to_at_dma_chan(chan);
	unsigned long		flags;
	enum dma_status		ret;
	int bytes = 0;

	ret = dma_cookie_status(chan, cookie, txstate);
	if (ret == DMA_COMPLETE)
		return ret;
	/*
	 * There's no point calculating the residue if there's
	 * no txstate to store the value.
	 */
	if (!txstate)
		return DMA_ERROR;

	spin_lock_irqsave(&atchan->lock, flags);

	/*  Get number of bytes left in the active transactions */
	bytes = atc_get_bytes_left(chan, cookie);

	spin_unlock_irqrestore(&atchan->lock, flags);

	if (unlikely(bytes < 0)) {
		dev_vdbg(chan2dev(chan), "get residual bytes error\n");
		return DMA_ERROR;
	} else {
		dma_set_residue(txstate, bytes);
	}

	dev_vdbg(chan2dev(chan), "tx_status %d: cookie = %d residue = %d\n",
		 ret, cookie, bytes);

	return ret;
}

/**
 * atc_issue_pending - try to finish work
 * @chan: target DMA channel
 */
static void atc_issue_pending(struct dma_chan *chan)
{
	struct at_dma_chan	*atchan = to_at_dma_chan(chan);

	dev_vdbg(chan2dev(chan), "issue_pending\n");

	/* Not needed for cyclic transfers */
	if (atc_chan_is_cyclic(atchan))
		return;

	atc_advance_work(atchan);
}

/**
 * atc_alloc_chan_resources - allocate resources for DMA channel
 * @chan: allocate descriptor resources for this channel
 *
 * return - the number of allocated descriptors
 */
static int atc_alloc_chan_resources(struct dma_chan *chan)
{
	struct at_dma_chan	*atchan = to_at_dma_chan(chan);
	struct at_dma		*atdma = to_at_dma(chan->device);
	struct at_desc		*desc;
	struct at_dma_slave	*atslave;
	int			i;
	u32			cfg;

	dev_vdbg(chan2dev(chan), "alloc_chan_resources\n");

	/* ASSERT:  channel is idle */
	if (atc_chan_is_enabled(atchan)) {
		dev_dbg(chan2dev(chan), "DMA channel not idle ?\n");
		return -EIO;
	}

	if (!list_empty(&atchan->free_list)) {
		dev_dbg(chan2dev(chan), "can't allocate channel resources (channel not freed from a previous use)\n");
		return -EIO;
	}

	cfg = ATC_DEFAULT_CFG;

	atslave = chan->private;
	if (atslave) {
		/*
		 * We need controller-specific data to set up slave
		 * transfers.
		 */
		BUG_ON(!atslave->dma_dev || atslave->dma_dev != atdma->dma_common.dev);

		/* if cfg configuration specified take it instead of default */
		if (atslave->cfg)
			cfg = atslave->cfg;
	}

	/* Allocate initial pool of descriptors */
	for (i = 0; i < init_nr_desc_per_channel; i++) {
		desc = atc_alloc_descriptor(chan, GFP_KERNEL);
		if (!desc) {
			dev_err(atdma->dma_common.dev,
				"Only %d initial descriptors\n", i);
			break;
		}
		list_add_tail(&desc->desc_node, &atchan->free_list);
	}

	dma_cookie_init(chan);

	/* channel parameters */
	channel_writel(atchan, CFG, cfg);

	dev_dbg(chan2dev(chan),
		"alloc_chan_resources: allocated %d descriptors\n", i);

	return i;
}

/**
 * atc_free_chan_resources - free all channel resources
 * @chan: DMA channel
 */
static void atc_free_chan_resources(struct dma_chan *chan)
{
	struct at_dma_chan	*atchan = to_at_dma_chan(chan);
	struct at_dma		*atdma = to_at_dma(chan->device);
	struct at_desc		*desc, *_desc;
	LIST_HEAD(list);

	/* ASSERT:  channel is idle */
	BUG_ON(!list_empty(&atchan->active_list));
	BUG_ON(!list_empty(&atchan->queue));
	BUG_ON(atc_chan_is_enabled(atchan));

	list_for_each_entry_safe(desc, _desc, &atchan->free_list, desc_node) {
		dev_vdbg(chan2dev(chan), "  freeing descriptor %p\n", desc);
		list_del(&desc->desc_node);
		/* free link descriptor */
		dma_pool_free(atdma->dma_desc_pool, desc, desc->txd.phys);
	}
	list_splice_init(&atchan->free_list, &list);
	atchan->status = 0;

	/*
	 * Free atslave allocated in at_dma_xlate()
	 */
	kfree(chan->private);
	chan->private = NULL;

	dev_vdbg(chan2dev(chan), "free_chan_resources: done\n");
}

#ifdef CONFIG_OF
static bool at_dma_filter(struct dma_chan *chan, void *slave)
{
	struct at_dma_slave *atslave = slave;

	if (atslave->dma_dev == chan->device->dev) {
		chan->private = atslave;
		return true;
	} else {
		return false;
	}
}

static struct dma_chan *at_dma_xlate(struct of_phandle_args *dma_spec,
				     struct of_dma *of_dma)
{
	struct dma_chan *chan;
	struct at_dma_chan *atchan;
	struct at_dma_slave *atslave;
	dma_cap_mask_t mask;
	unsigned int per_id;
	struct platform_device *dmac_pdev;

	if (dma_spec->args_count != 2)
		return NULL;

	dmac_pdev = of_find_device_by_node(dma_spec->np);
	if (!dmac_pdev)
		return NULL;

	dma_cap_zero(mask);
	dma_cap_set(DMA_SLAVE, mask);

	atslave = kmalloc(sizeof(*atslave), GFP_KERNEL);
	if (!atslave) {
		put_device(&dmac_pdev->dev);
		return NULL;
	}

	atslave->cfg = ATC_DST_H2SEL_HW | ATC_SRC_H2SEL_HW;
	/*
	 * We can fill both SRC_PER and DST_PER, one of these fields will be
	 * ignored depending on DMA transfer direction.
	 */
	per_id = dma_spec->args[1] & AT91_DMA_CFG_PER_ID_MASK;
	atslave->cfg |= ATC_DST_PER_MSB(per_id) | ATC_DST_PER(per_id)
		     | ATC_SRC_PER_MSB(per_id) | ATC_SRC_PER(per_id);
	/*
	 * We have to translate the value we get from the device tree since
	 * the half FIFO configuration value had to be 0 to keep backward
	 * compatibility.
	 */
	switch (dma_spec->args[1] & AT91_DMA_CFG_FIFOCFG_MASK) {
	case AT91_DMA_CFG_FIFOCFG_ALAP:
		atslave->cfg |= ATC_FIFOCFG_LARGESTBURST;
		break;
	case AT91_DMA_CFG_FIFOCFG_ASAP:
		atslave->cfg |= ATC_FIFOCFG_ENOUGHSPACE;
		break;
	case AT91_DMA_CFG_FIFOCFG_HALF:
	default:
		atslave->cfg |= ATC_FIFOCFG_HALFFIFO;
	}
	atslave->dma_dev = &dmac_pdev->dev;

	chan = dma_request_channel(mask, at_dma_filter, atslave);
	if (!chan) {
		put_device(&dmac_pdev->dev);
		kfree(atslave);
		return NULL;
	}

	atchan = to_at_dma_chan(chan);
	atchan->per_if = dma_spec->args[0] & 0xff;
	atchan->mem_if = (dma_spec->args[0] >> 16) & 0xff;

	return chan;
}
#else
static struct dma_chan *at_dma_xlate(struct of_phandle_args *dma_spec,
				     struct of_dma *of_dma)
{
	return NULL;
}
#endif

/*--  Module Management  -----------------------------------------------*/

/* cap_mask is a multi-u32 bitfield, fill it with proper C code. */
static struct at_dma_platform_data at91sam9rl_config = {
	.nr_channels = 2,
};
static struct at_dma_platform_data at91sam9g45_config = {
	.nr_channels = 8,
};

#if defined(CONFIG_OF)
static const struct of_device_id atmel_dma_dt_ids[] = {
	{
		.compatible = "atmel,at91sam9rl-dma",
		.data = &at91sam9rl_config,
	}, {
		.compatible = "atmel,at91sam9g45-dma",
		.data = &at91sam9g45_config,
	}, {
		/* sentinel */
	}
};

MODULE_DEVICE_TABLE(of, atmel_dma_dt_ids);
#endif

static const struct platform_device_id atdma_devtypes[] = {
	{
		.name = "at91sam9rl_dma",
		.driver_data = (unsigned long) &at91sam9rl_config,
	}, {
		.name = "at91sam9g45_dma",
		.driver_data = (unsigned long) &at91sam9g45_config,
	}, {
		/* sentinel */
	}
};

static inline const struct at_dma_platform_data * __init at_dma_get_driver_data(
						struct platform_device *pdev)
{
	if (pdev->dev.of_node) {
		const struct of_device_id *match;
		match = of_match_node(atmel_dma_dt_ids, pdev->dev.of_node);
		if (match == NULL)
			return NULL;
		return match->data;
	}
	return (struct at_dma_platform_data *)
			platform_get_device_id(pdev)->driver_data;
}

/**
 * at_dma_off - disable DMA controller
 * @atdma: the Atmel HDAMC device
 */
static void at_dma_off(struct at_dma *atdma)
{
	dma_writel(atdma, EN, 0);

	/* disable all interrupts */
	dma_writel(atdma, EBCIDR, -1L);

	/* confirm that all channels are disabled */
	while (dma_readl(atdma, CHSR) & atdma->all_chan_mask)
		cpu_relax();
}

static int __init at_dma_probe(struct platform_device *pdev)
{
	struct resource		*io;
	struct at_dma		*atdma;
	size_t			size;
	int			irq;
	int			err;
	int			i;
	const struct at_dma_platform_data *plat_dat;

	/* setup platform data for each SoC */
	dma_cap_set(DMA_MEMCPY, at91sam9rl_config.cap_mask);
	dma_cap_set(DMA_INTERLEAVE, at91sam9g45_config.cap_mask);
	dma_cap_set(DMA_MEMCPY, at91sam9g45_config.cap_mask);
	dma_cap_set(DMA_MEMSET, at91sam9g45_config.cap_mask);
	dma_cap_set(DMA_MEMSET_SG, at91sam9g45_config.cap_mask);
	dma_cap_set(DMA_PRIVATE, at91sam9g45_config.cap_mask);
	dma_cap_set(DMA_SLAVE, at91sam9g45_config.cap_mask);

	/* get DMA parameters from controller type */
	plat_dat = at_dma_get_driver_data(pdev);
	if (!plat_dat)
		return -ENODEV;

	io = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!io)
		return -EINVAL;

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	size = sizeof(struct at_dma);
	size += plat_dat->nr_channels * sizeof(struct at_dma_chan);
	atdma = kzalloc(size, GFP_KERNEL);
	if (!atdma)
		return -ENOMEM;

	/* discover transaction capabilities */
	atdma->dma_common.cap_mask = plat_dat->cap_mask;
	atdma->all_chan_mask = (1 << plat_dat->nr_channels) - 1;

	size = resource_size(io);
	if (!request_mem_region(io->start, size, pdev->dev.driver->name)) {
		err = -EBUSY;
		goto err_kfree;
	}

	atdma->regs = ioremap(io->start, size);
	if (!atdma->regs) {
		err = -ENOMEM;
		goto err_release_r;
	}

	atdma->clk = clk_get(&pdev->dev, "dma_clk");
	if (IS_ERR(atdma->clk)) {
		err = PTR_ERR(atdma->clk);
		goto err_clk;
	}
	err = clk_prepare_enable(atdma->clk);
	if (err)
		goto err_clk_prepare;

	/* force dma off, just in case */
	at_dma_off(atdma);

	err = request_irq(irq, at_dma_interrupt, 0, "at_hdmac", atdma);
	if (err)
		goto err_irq;

	platform_set_drvdata(pdev, atdma);

	/* create a pool of consistent memory blocks for hardware descriptors */
	atdma->dma_desc_pool = dma_pool_create("at_hdmac_desc_pool",
			&pdev->dev, sizeof(struct at_desc),
			4 /* word alignment */, 0);
	if (!atdma->dma_desc_pool) {
		dev_err(&pdev->dev, "No memory for descriptors dma pool\n");
		err = -ENOMEM;
		goto err_desc_pool_create;
	}

	/* create a pool of consistent memory blocks for memset blocks */
	atdma->memset_pool = dma_pool_create("at_hdmac_memset_pool",
					     &pdev->dev, sizeof(int), 4, 0);
	if (!atdma->memset_pool) {
		dev_err(&pdev->dev, "No memory for memset dma pool\n");
		err = -ENOMEM;
		goto err_memset_pool_create;
	}

	/* clear any pending interrupt */
	while (dma_readl(atdma, EBCISR))
		cpu_relax();

	/* initialize channels related values */
	INIT_LIST_HEAD(&atdma->dma_common.channels);
	for (i = 0; i < plat_dat->nr_channels; i++) {
		struct at_dma_chan	*atchan = &atdma->chan[i];

		atchan->mem_if = AT_DMA_MEM_IF;
		atchan->per_if = AT_DMA_PER_IF;
		atchan->chan_common.device = &atdma->dma_common;
		dma_cookie_init(&atchan->chan_common);
		list_add_tail(&atchan->chan_common.device_node,
				&atdma->dma_common.channels);

		atchan->ch_regs = atdma->regs + ch_regs(i);
		spin_lock_init(&atchan->lock);
		atchan->mask = 1 << i;

		INIT_LIST_HEAD(&atchan->active_list);
		INIT_LIST_HEAD(&atchan->queue);
		INIT_LIST_HEAD(&atchan->free_list);

		tasklet_setup(&atchan->tasklet, atc_tasklet);
		atc_enable_chan_irq(atdma, i);
	}

	/* set base routines */
	atdma->dma_common.device_alloc_chan_resources = atc_alloc_chan_resources;
	atdma->dma_common.device_free_chan_resources = atc_free_chan_resources;
	atdma->dma_common.device_tx_status = atc_tx_status;
	atdma->dma_common.device_issue_pending = atc_issue_pending;
	atdma->dma_common.dev = &pdev->dev;

	/* set prep routines based on capability */
	if (dma_has_cap(DMA_INTERLEAVE, atdma->dma_common.cap_mask))
		atdma->dma_common.device_prep_interleaved_dma = atc_prep_dma_interleaved;

	if (dma_has_cap(DMA_MEMCPY, atdma->dma_common.cap_mask))
		atdma->dma_common.device_prep_dma_memcpy = atc_prep_dma_memcpy;

	if (dma_has_cap(DMA_MEMSET, atdma->dma_common.cap_mask)) {
		atdma->dma_common.device_prep_dma_memset = atc_prep_dma_memset;
		atdma->dma_common.device_prep_dma_memset_sg = atc_prep_dma_memset_sg;
		atdma->dma_common.fill_align = DMAENGINE_ALIGN_4_BYTES;
	}

	if (dma_has_cap(DMA_SLAVE, atdma->dma_common.cap_mask)) {
		atdma->dma_common.device_prep_slave_sg = atc_prep_slave_sg;
		/* controller can do slave DMA: can trigger cyclic transfers */
		dma_cap_set(DMA_CYCLIC, atdma->dma_common.cap_mask);
		atdma->dma_common.device_prep_dma_cyclic = atc_prep_dma_cyclic;
		atdma->dma_common.device_config = atc_config;
		atdma->dma_common.device_pause = atc_pause;
		atdma->dma_common.device_resume = atc_resume;
		atdma->dma_common.device_terminate_all = atc_terminate_all;
		atdma->dma_common.src_addr_widths = ATC_DMA_BUSWIDTHS;
		atdma->dma_common.dst_addr_widths = ATC_DMA_BUSWIDTHS;
		atdma->dma_common.directions = BIT(DMA_DEV_TO_MEM) | BIT(DMA_MEM_TO_DEV);
		atdma->dma_common.residue_granularity = DMA_RESIDUE_GRANULARITY_BURST;
	}

	dma_writel(atdma, EN, AT_DMA_ENABLE);

	dev_info(&pdev->dev, "Atmel AHB DMA Controller ( %s%s%s), %d channels\n",
	  dma_has_cap(DMA_MEMCPY, atdma->dma_common.cap_mask) ? "cpy " : "",
	  dma_has_cap(DMA_MEMSET, atdma->dma_common.cap_mask) ? "set " : "",
	  dma_has_cap(DMA_SLAVE, atdma->dma_common.cap_mask)  ? "slave " : "",
	  plat_dat->nr_channels);

	dma_async_device_register(&atdma->dma_common);

	/*
	 * Do not return an error if the dmac node is not present in order to
	 * not break the existing way of requesting channel with
	 * dma_request_channel().
	 */
	if (pdev->dev.of_node) {
		err = of_dma_controller_register(pdev->dev.of_node,
						 at_dma_xlate, atdma);
		if (err) {
			dev_err(&pdev->dev, "could not register of_dma_controller\n");
			goto err_of_dma_controller_register;
		}
	}

	return 0;

err_of_dma_controller_register:
	dma_async_device_unregister(&atdma->dma_common);
	dma_pool_destroy(atdma->memset_pool);
err_memset_pool_create:
	dma_pool_destroy(atdma->dma_desc_pool);
err_desc_pool_create:
	free_irq(platform_get_irq(pdev, 0), atdma);
err_irq:
	clk_disable_unprepare(atdma->clk);
err_clk_prepare:
	clk_put(atdma->clk);
err_clk:
	iounmap(atdma->regs);
	atdma->regs = NULL;
err_release_r:
	release_mem_region(io->start, size);
err_kfree:
	kfree(atdma);
	return err;
}

static int at_dma_remove(struct platform_device *pdev)
{
	struct at_dma		*atdma = platform_get_drvdata(pdev);
	struct dma_chan		*chan, *_chan;
	struct resource		*io;

	at_dma_off(atdma);
	if (pdev->dev.of_node)
		of_dma_controller_free(pdev->dev.of_node);
	dma_async_device_unregister(&atdma->dma_common);

	dma_pool_destroy(atdma->memset_pool);
	dma_pool_destroy(atdma->dma_desc_pool);
	free_irq(platform_get_irq(pdev, 0), atdma);

	list_for_each_entry_safe(chan, _chan, &atdma->dma_common.channels,
			device_node) {
		struct at_dma_chan	*atchan = to_at_dma_chan(chan);

		/* Disable interrupts */
		atc_disable_chan_irq(atdma, chan->chan_id);

		tasklet_kill(&atchan->tasklet);
		list_del(&chan->device_node);
	}

	clk_disable_unprepare(atdma->clk);
	clk_put(atdma->clk);

	iounmap(atdma->regs);
	atdma->regs = NULL;

	io = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	release_mem_region(io->start, resource_size(io));

	kfree(atdma);

	return 0;
}

static void at_dma_shutdown(struct platform_device *pdev)
{
	struct at_dma	*atdma = platform_get_drvdata(pdev);

	at_dma_off(platform_get_drvdata(pdev));
	clk_disable_unprepare(atdma->clk);
}

static int at_dma_prepare(struct device *dev)
{
	struct at_dma *atdma = dev_get_drvdata(dev);
	struct dma_chan *chan, *_chan;

	list_for_each_entry_safe(chan, _chan, &atdma->dma_common.channels,
			device_node) {
		struct at_dma_chan *atchan = to_at_dma_chan(chan);
		/* wait for transaction completion (except in cyclic case) */
		if (atc_chan_is_enabled(atchan) && !atc_chan_is_cyclic(atchan))
			return -EAGAIN;
	}
	return 0;
}

static void atc_suspend_cyclic(struct at_dma_chan *atchan)
{
	struct dma_chan	*chan = &atchan->chan_common;

	/* Channel should be paused by user
	 * do it anyway even if it is not done already */
	if (!atc_chan_is_paused(atchan)) {
		dev_warn(chan2dev(chan),
		"cyclic channel not paused, should be done by channel user\n");
		atc_pause(chan);
	}

	/* now preserve additional data for cyclic operations */
	/* next descriptor address in the cyclic list */
	atchan->save_dscr = channel_readl(atchan, DSCR);

	vdbg_dump_regs(atchan);
}

static int at_dma_suspend_noirq(struct device *dev)
{
	struct at_dma *atdma = dev_get_drvdata(dev);
	struct dma_chan *chan, *_chan;

	/* preserve data */
	list_for_each_entry_safe(chan, _chan, &atdma->dma_common.channels,
			device_node) {
		struct at_dma_chan *atchan = to_at_dma_chan(chan);

		if (atc_chan_is_cyclic(atchan))
			atc_suspend_cyclic(atchan);
		atchan->save_cfg = channel_readl(atchan, CFG);
	}
	atdma->save_imr = dma_readl(atdma, EBCIMR);

	/* disable DMA controller */
	at_dma_off(atdma);
	clk_disable_unprepare(atdma->clk);
	return 0;
}

static void atc_resume_cyclic(struct at_dma_chan *atchan)
{
	struct at_dma	*atdma = to_at_dma(atchan->chan_common.device);

	/* restore channel status for cyclic descriptors list:
	 * next descriptor in the cyclic list at the time of suspend */
	channel_writel(atchan, SADDR, 0);
	channel_writel(atchan, DADDR, 0);
	channel_writel(atchan, CTRLA, 0);
	channel_writel(atchan, CTRLB, 0);
	channel_writel(atchan, DSCR, atchan->save_dscr);
	dma_writel(atdma, CHER, atchan->mask);

	/* channel pause status should be removed by channel user
	 * We cannot take the initiative to do it here */

	vdbg_dump_regs(atchan);
}

static int at_dma_resume_noirq(struct device *dev)
{
	struct at_dma *atdma = dev_get_drvdata(dev);
	struct dma_chan *chan, *_chan;

	/* bring back DMA controller */
	clk_prepare_enable(atdma->clk);
	dma_writel(atdma, EN, AT_DMA_ENABLE);

	/* clear any pending interrupt */
	while (dma_readl(atdma, EBCISR))
		cpu_relax();

	/* restore saved data */
	dma_writel(atdma, EBCIER, atdma->save_imr);
	list_for_each_entry_safe(chan, _chan, &atdma->dma_common.channels,
			device_node) {
		struct at_dma_chan *atchan = to_at_dma_chan(chan);

		channel_writel(atchan, CFG, atchan->save_cfg);
		if (atc_chan_is_cyclic(atchan))
			atc_resume_cyclic(atchan);
	}
	return 0;
}

static const struct dev_pm_ops at_dma_dev_pm_ops = {
	.prepare = at_dma_prepare,
	.suspend_noirq = at_dma_suspend_noirq,
	.resume_noirq = at_dma_resume_noirq,
};

static struct platform_driver at_dma_driver = {
	.remove		= at_dma_remove,
	.shutdown	= at_dma_shutdown,
	.id_table	= atdma_devtypes,
	.driver = {
		.name	= "at_hdmac",
		.pm	= &at_dma_dev_pm_ops,
		.of_match_table	= of_match_ptr(atmel_dma_dt_ids),
	},
};

static int __init at_dma_init(void)
{
	return platform_driver_probe(&at_dma_driver, at_dma_probe);
}
subsys_initcall(at_dma_init);

static void __exit at_dma_exit(void)
{
	platform_driver_unregister(&at_dma_driver);
}
module_exit(at_dma_exit);

MODULE_DESCRIPTION("Atmel AHB DMA Controller driver");
MODULE_AUTHOR("Nicolas Ferre <nicolas.ferre@atmel.com>");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:at_hdmac");
