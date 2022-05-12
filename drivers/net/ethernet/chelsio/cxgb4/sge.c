/*
 * This file is part of the Chelsio T4 Ethernet driver for Linux.
 *
 * Copyright (c) 2003-2014 Chelsio Communications, Inc. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <linux/dma-mapping.h>
#include <linux/jiffies.h>
#include <linux/prefetch.h>
#include <linux/export.h>
#include <net/xfrm.h>
#include <net/ipv6.h>
#include <net/tcp.h>
#include <net/busy_poll.h>
#ifdef CONFIG_CHELSIO_T4_FCOE
#include <scsi/fc/fc_fcoe.h>
#endif /* CONFIG_CHELSIO_T4_FCOE */
#include "cxgb4.h"
#include "t4_regs.h"
#include "t4_values.h"
#include "t4_msg.h"
#include "t4fw_api.h"
#include "cxgb4_ptp.h"
#include "cxgb4_uld.h"
#include "cxgb4_tc_mqprio.h"
#include "sched.h"

/*
 * Rx buffer size.  We use largish buffers if possible but settle for single
 * pages under memory shortage.
 */
#if PAGE_SHIFT >= 16
# define FL_PG_ORDER 0
#else
# define FL_PG_ORDER (16 - PAGE_SHIFT)
#endif

/* RX_PULL_LEN should be <= RX_COPY_THRES */
#define RX_COPY_THRES    256
#define RX_PULL_LEN      128

/*
 * Main body length for sk_buffs used for Rx Ethernet packets with fragments.
 * Should be >= RX_PULL_LEN but possibly bigger to give pskb_may_pull some room.
 */
#define RX_PKT_SKB_LEN   512

/*
 * Max number of Tx descriptors we clean up at a time.  Should be modest as
 * freeing skbs isn't cheap and it happens while holding locks.  We just need
 * to free packets faster than they arrive, we eventually catch up and keep
 * the amortized cost reasonable.  Must be >= 2 * TXQ_STOP_THRES.  It should
 * also match the CIDX Flush Threshold.
 */
#define MAX_TX_RECLAIM 32

/*
 * Max number of Rx buffers we replenish at a time.  Again keep this modest,
 * allocating buffers isn't cheap either.
 */
#define MAX_RX_REFILL 16U

/*
 * Period of the Rx queue check timer.  This timer is infrequent as it has
 * something to do only when the system experiences severe memory shortage.
 */
#define RX_QCHECK_PERIOD (HZ / 2)

/*
 * Period of the Tx queue check timer.
 */
#define TX_QCHECK_PERIOD (HZ / 2)

/*
 * Max number of Tx descriptors to be reclaimed by the Tx timer.
 */
#define MAX_TIMER_TX_RECLAIM 100

/*
 * Timer index used when backing off due to memory shortage.
 */
#define NOMEM_TMR_IDX (SGE_NTIMERS - 1)

/*
 * Suspension threshold for non-Ethernet Tx queues.  We require enough room
 * for a full sized WR.
 */
#define TXQ_STOP_THRES (SGE_MAX_WR_LEN / sizeof(struct tx_desc))

/*
 * Max Tx descriptor space we allow for an Ethernet packet to be inlined
 * into a WR.
 */
#define MAX_IMM_TX_PKT_LEN 256

/*
 * Max size of a WR sent through a control Tx queue.
 */
#define MAX_CTRL_WR_LEN SGE_MAX_WR_LEN

struct rx_sw_desc {                /* SW state per Rx descriptor */
	struct page *page;
	dma_addr_t dma_addr;
};

/*
 * Rx buffer sizes for "useskbs" Free List buffers (one ingress packet pe skb
 * buffer).  We currently only support two sizes for 1500- and 9000-byte MTUs.
 * We could easily support more but there doesn't seem to be much need for
 * that ...
 */
#define FL_MTU_SMALL 1500
#define FL_MTU_LARGE 9000

static inline unsigned int fl_mtu_bufsize(struct adapter *adapter,
					  unsigned int mtu)
{
	struct sge *s = &adapter->sge;

	return ALIGN(s->pktshift + ETH_HLEN + VLAN_HLEN + mtu, s->fl_align);
}

#define FL_MTU_SMALL_BUFSIZE(adapter) fl_mtu_bufsize(adapter, FL_MTU_SMALL)
#define FL_MTU_LARGE_BUFSIZE(adapter) fl_mtu_bufsize(adapter, FL_MTU_LARGE)

/*
 * Bits 0..3 of rx_sw_desc.dma_addr have special meaning.  The hardware uses
 * these to specify the buffer size as an index into the SGE Free List Buffer
 * Size register array.  We also use bit 4, when the buffer has been unmapped
 * for DMA, but this is of course never sent to the hardware and is only used
 * to prevent double unmappings.  All of the above requires that the Free List
 * Buffers which we allocate have the bottom 5 bits free (0) -- i.e. are
 * 32-byte or or a power of 2 greater in alignment.  Since the SGE's minimal
 * Free List Buffer alignment is 32 bytes, this works out for us ...
 */
enum {
	RX_BUF_FLAGS     = 0x1f,   /* bottom five bits are special */
	RX_BUF_SIZE      = 0x0f,   /* bottom three bits are for buf sizes */
	RX_UNMAPPED_BUF  = 0x10,   /* buffer is not mapped */

	/*
	 * XXX We shouldn't depend on being able to use these indices.
	 * XXX Especially when some other Master PF has initialized the
	 * XXX adapter or we use the Firmware Configuration File.  We
	 * XXX should really search through the Host Buffer Size register
	 * XXX array for the appropriately sized buffer indices.
	 */
	RX_SMALL_PG_BUF  = 0x0,   /* small (PAGE_SIZE) page buffer */
	RX_LARGE_PG_BUF  = 0x1,   /* buffer large (FL_PG_ORDER) page buffer */

	RX_SMALL_MTU_BUF = 0x2,   /* small MTU buffer */
	RX_LARGE_MTU_BUF = 0x3,   /* large MTU buffer */
};

static int timer_pkt_quota[] = {1, 1, 2, 3, 4, 5};
#define MIN_NAPI_WORK  1

static inline dma_addr_t get_buf_addr(const struct rx_sw_desc *d)
{
	return d->dma_addr & ~(dma_addr_t)RX_BUF_FLAGS;
}

static inline bool is_buf_mapped(const struct rx_sw_desc *d)
{
	return !(d->dma_addr & RX_UNMAPPED_BUF);
}

/**
 *	txq_avail - return the number of available slots in a Tx queue
 *	@q: the Tx queue
 *
 *	Returns the number of descriptors in a Tx queue available to write new
 *	packets.
 */
static inline unsigned int txq_avail(const struct sge_txq *q)
{
	return q->size - 1 - q->in_use;
}

/**
 *	fl_cap - return the capacity of a free-buffer list
 *	@fl: the FL
 *
 *	Returns the capacity of a free-buffer list.  The capacity is less than
 *	the size because one descriptor needs to be left unpopulated, otherwise
 *	HW will think the FL is empty.
 */
static inline unsigned int fl_cap(const struct sge_fl *fl)
{
	return fl->size - 8;   /* 1 descriptor = 8 buffers */
}

/**
 *	fl_starving - return whether a Free List is starving.
 *	@adapter: pointer to the adapter
 *	@fl: the Free List
 *
 *	Tests specified Free List to see whether the number of buffers
 *	available to the hardware has falled below our "starvation"
 *	threshold.
 */
static inline bool fl_starving(const struct adapter *adapter,
			       const struct sge_fl *fl)
{
	const struct sge *s = &adapter->sge;

	return fl->avail - fl->pend_cred <= s->fl_starve_thres;
}

int cxgb4_map_skb(struct device *dev, const struct sk_buff *skb,
		  dma_addr_t *addr)
{
	const skb_frag_t *fp, *end;
	const struct skb_shared_info *si;

	*addr = dma_map_single(dev, skb->data, skb_headlen(skb), DMA_TO_DEVICE);
	if (dma_mapping_error(dev, *addr))
		goto out_err;

	si = skb_shinfo(skb);
	end = &si->frags[si->nr_frags];

	for (fp = si->frags; fp < end; fp++) {
		*++addr = skb_frag_dma_map(dev, fp, 0, skb_frag_size(fp),
					   DMA_TO_DEVICE);
		if (dma_mapping_error(dev, *addr))
			goto unwind;
	}
	return 0;

unwind:
	while (fp-- > si->frags)
		dma_unmap_page(dev, *--addr, skb_frag_size(fp), DMA_TO_DEVICE);

	dma_unmap_single(dev, addr[-1], skb_headlen(skb), DMA_TO_DEVICE);
out_err:
	return -ENOMEM;
}
EXPORT_SYMBOL(cxgb4_map_skb);

static void unmap_skb(struct device *dev, const struct sk_buff *skb,
		      const dma_addr_t *addr)
{
	const skb_frag_t *fp, *end;
	const struct skb_shared_info *si;

	dma_unmap_single(dev, *addr++, skb_headlen(skb), DMA_TO_DEVICE);

	si = skb_shinfo(skb);
	end = &si->frags[si->nr_frags];
	for (fp = si->frags; fp < end; fp++)
		dma_unmap_page(dev, *addr++, skb_frag_size(fp), DMA_TO_DEVICE);
}

#ifdef CONFIG_NEED_DMA_MAP_STATE
/**
 *	deferred_unmap_destructor - unmap a packet when it is freed
 *	@skb: the packet
 *
 *	This is the packet destructor used for Tx packets that need to remain
 *	mapped until they are freed rather than until their Tx descriptors are
 *	freed.
 */
static void deferred_unmap_destructor(struct sk_buff *skb)
{
	unmap_skb(skb->dev->dev.parent, skb, (dma_addr_t *)skb->head);
}
#endif

/**
 *	free_tx_desc - reclaims Tx descriptors and their buffers
 *	@adap: the adapter
 *	@q: the Tx queue to reclaim descriptors from
 *	@n: the number of descriptors to reclaim
 *	@unmap: whether the buffers should be unmapped for DMA
 *
 *	Reclaims Tx descriptors from an SGE Tx queue and frees the associated
 *	Tx buffers.  Called with the Tx queue lock held.
 */
void free_tx_desc(struct adapter *adap, struct sge_txq *q,
		  unsigned int n, bool unmap)
{
	unsigned int cidx = q->cidx;
	struct tx_sw_desc *d;

	d = &q->sdesc[cidx];
	while (n--) {
		if (d->skb) {                       /* an SGL is present */
			if (unmap && d->addr[0]) {
				unmap_skb(adap->pdev_dev, d->skb, d->addr);
				memset(d->addr, 0, sizeof(d->addr));
			}
			dev_consume_skb_any(d->skb);
			d->skb = NULL;
		}
		++d;
		if (++cidx == q->size) {
			cidx = 0;
			d = q->sdesc;
		}
	}
	q->cidx = cidx;
}

/*
 * Return the number of reclaimable descriptors in a Tx queue.
 */
static inline int reclaimable(const struct sge_txq *q)
{
	int hw_cidx = ntohs(READ_ONCE(q->stat->cidx));
	hw_cidx -= q->cidx;
	return hw_cidx < 0 ? hw_cidx + q->size : hw_cidx;
}

/**
 *	reclaim_completed_tx - reclaims completed TX Descriptors
 *	@adap: the adapter
 *	@q: the Tx queue to reclaim completed descriptors from
 *	@maxreclaim: the maximum number of TX Descriptors to reclaim or -1
 *	@unmap: whether the buffers should be unmapped for DMA
 *
 *	Reclaims Tx Descriptors that the SGE has indicated it has processed,
 *	and frees the associated buffers if possible.  If @max == -1, then
 *	we'll use a defaiult maximum.  Called with the TX Queue locked.
 */
static inline int reclaim_completed_tx(struct adapter *adap, struct sge_txq *q,
				       int maxreclaim, bool unmap)
{
	int reclaim = reclaimable(q);

	if (reclaim) {
		/*
		 * Limit the amount of clean up work we do at a time to keep
		 * the Tx lock hold time O(1).
		 */
		if (maxreclaim < 0)
			maxreclaim = MAX_TX_RECLAIM;
		if (reclaim > maxreclaim)
			reclaim = maxreclaim;

		free_tx_desc(adap, q, reclaim, unmap);
		q->in_use -= reclaim;
	}

	return reclaim;
}

/**
 *	cxgb4_reclaim_completed_tx - reclaims completed Tx descriptors
 *	@adap: the adapter
 *	@q: the Tx queue to reclaim completed descriptors from
 *	@unmap: whether the buffers should be unmapped for DMA
 *
 *	Reclaims Tx descriptors that the SGE has indicated it has processed,
 *	and frees the associated buffers if possible.  Called with the Tx
 *	queue locked.
 */
void cxgb4_reclaim_completed_tx(struct adapter *adap, struct sge_txq *q,
				bool unmap)
{
	(void)reclaim_completed_tx(adap, q, -1, unmap);
}
EXPORT_SYMBOL(cxgb4_reclaim_completed_tx);

static inline int get_buf_size(struct adapter *adapter,
			       const struct rx_sw_desc *d)
{
	struct sge *s = &adapter->sge;
	unsigned int rx_buf_size_idx = d->dma_addr & RX_BUF_SIZE;
	int buf_size;

	switch (rx_buf_size_idx) {
	case RX_SMALL_PG_BUF:
		buf_size = PAGE_SIZE;
		break;

	case RX_LARGE_PG_BUF:
		buf_size = PAGE_SIZE << s->fl_pg_order;
		break;

	case RX_SMALL_MTU_BUF:
		buf_size = FL_MTU_SMALL_BUFSIZE(adapter);
		break;

	case RX_LARGE_MTU_BUF:
		buf_size = FL_MTU_LARGE_BUFSIZE(adapter);
		break;

	default:
		BUG();
	}

	return buf_size;
}

/**
 *	free_rx_bufs - free the Rx buffers on an SGE free list
 *	@adap: the adapter
 *	@q: the SGE free list to free buffers from
 *	@n: how many buffers to free
 *
 *	Release the next @n buffers on an SGE free-buffer Rx queue.   The
 *	buffers must be made inaccessible to HW before calling this function.
 */
static void free_rx_bufs(struct adapter *adap, struct sge_fl *q, int n)
{
	while (n--) {
		struct rx_sw_desc *d = &q->sdesc[q->cidx];

		if (is_buf_mapped(d))
			dma_unmap_page(adap->pdev_dev, get_buf_addr(d),
				       get_buf_size(adap, d),
				       PCI_DMA_FROMDEVICE);
		put_page(d->page);
		d->page = NULL;
		if (++q->cidx == q->size)
			q->cidx = 0;
		q->avail--;
	}
}

/**
 *	unmap_rx_buf - unmap the current Rx buffer on an SGE free list
 *	@adap: the adapter
 *	@q: the SGE free list
 *
 *	Unmap the current buffer on an SGE free-buffer Rx queue.   The
 *	buffer must be made inaccessible to HW before calling this function.
 *
 *	This is similar to @free_rx_bufs above but does not free the buffer.
 *	Do note that the FL still loses any further access to the buffer.
 */
static void unmap_rx_buf(struct adapter *adap, struct sge_fl *q)
{
	struct rx_sw_desc *d = &q->sdesc[q->cidx];

	if (is_buf_mapped(d))
		dma_unmap_page(adap->pdev_dev, get_buf_addr(d),
			       get_buf_size(adap, d), PCI_DMA_FROMDEVICE);
	d->page = NULL;
	if (++q->cidx == q->size)
		q->cidx = 0;
	q->avail--;
}

static inline void ring_fl_db(struct adapter *adap, struct sge_fl *q)
{
	if (q->pend_cred >= 8) {
		u32 val = adap->params.arch.sge_fl_db;

		if (is_t4(adap->params.chip))
			val |= PIDX_V(q->pend_cred / 8);
		else
			val |= PIDX_T5_V(q->pend_cred / 8);

		/* Make sure all memory writes to the Free List queue are
		 * committed before we tell the hardware about them.
		 */
		wmb();

		/* If we don't have access to the new User Doorbell (T5+), use
		 * the old doorbell mechanism; otherwise use the new BAR2
		 * mechanism.
		 */
		if (unlikely(q->bar2_addr == NULL)) {
			t4_write_reg(adap, MYPF_REG(SGE_PF_KDOORBELL_A),
				     val | QID_V(q->cntxt_id));
		} else {
			writel(val | QID_V(q->bar2_qid),
			       q->bar2_addr + SGE_UDB_KDOORBELL);

			/* This Write memory Barrier will force the write to
			 * the User Doorbell area to be flushed.
			 */
			wmb();
		}
		q->pend_cred &= 7;
	}
}

static inline void set_rx_sw_desc(struct rx_sw_desc *sd, struct page *pg,
				  dma_addr_t mapping)
{
	sd->page = pg;
	sd->dma_addr = mapping;      /* includes size low bits */
}

/**
 *	refill_fl - refill an SGE Rx buffer ring
 *	@adap: the adapter
 *	@q: the ring to refill
 *	@n: the number of new buffers to allocate
 *	@gfp: the gfp flags for the allocations
 *
 *	(Re)populate an SGE free-buffer queue with up to @n new packet buffers,
 *	allocated with the supplied gfp flags.  The caller must assure that
 *	@n does not exceed the queue's capacity.  If afterwards the queue is
 *	found critically low mark it as starving in the bitmap of starving FLs.
 *
 *	Returns the number of buffers allocated.
 */
static unsigned int refill_fl(struct adapter *adap, struct sge_fl *q, int n,
			      gfp_t gfp)
{
	struct sge *s = &adap->sge;
	struct page *pg;
	dma_addr_t mapping;
	unsigned int cred = q->avail;
	__be64 *d = &q->desc[q->pidx];
	struct rx_sw_desc *sd = &q->sdesc[q->pidx];
	int node;

#ifdef CONFIG_DEBUG_FS
	if (test_bit(q->cntxt_id - adap->sge.egr_start, adap->sge.blocked_fl))
		goto out;
#endif

	gfp |= __GFP_NOWARN;
	node = dev_to_node(adap->pdev_dev);

	if (s->fl_pg_order == 0)
		goto alloc_small_pages;

	/*
	 * Prefer large buffers
	 */
	while (n) {
		pg = alloc_pages_node(node, gfp | __GFP_COMP, s->fl_pg_order);
		if (unlikely(!pg)) {
			q->large_alloc_failed++;
			break;       /* fall back to single pages */
		}

		mapping = dma_map_page(adap->pdev_dev, pg, 0,
				       PAGE_SIZE << s->fl_pg_order,
				       PCI_DMA_FROMDEVICE);
		if (unlikely(dma_mapping_error(adap->pdev_dev, mapping))) {
			__free_pages(pg, s->fl_pg_order);
			q->mapping_err++;
			goto out;   /* do not try small pages for this error */
		}
		mapping |= RX_LARGE_PG_BUF;
		*d++ = cpu_to_be64(mapping);

		set_rx_sw_desc(sd, pg, mapping);
		sd++;

		q->avail++;
		if (++q->pidx == q->size) {
			q->pidx = 0;
			sd = q->sdesc;
			d = q->desc;
		}
		n--;
	}

alloc_small_pages:
	while (n--) {
		pg = alloc_pages_node(node, gfp, 0);
		if (unlikely(!pg)) {
			q->alloc_failed++;
			break;
		}

		mapping = dma_map_page(adap->pdev_dev, pg, 0, PAGE_SIZE,
				       PCI_DMA_FROMDEVICE);
		if (unlikely(dma_mapping_error(adap->pdev_dev, mapping))) {
			put_page(pg);
			q->mapping_err++;
			goto out;
		}
		*d++ = cpu_to_be64(mapping);

		set_rx_sw_desc(sd, pg, mapping);
		sd++;

		q->avail++;
		if (++q->pidx == q->size) {
			q->pidx = 0;
			sd = q->sdesc;
			d = q->desc;
		}
	}

out:	cred = q->avail - cred;
	q->pend_cred += cred;
	ring_fl_db(adap, q);

	if (unlikely(fl_starving(adap, q))) {
		smp_wmb();
		q->low++;
		set_bit(q->cntxt_id - adap->sge.egr_start,
			adap->sge.starving_fl);
	}

	return cred;
}

static inline void __refill_fl(struct adapter *adap, struct sge_fl *fl)
{
	refill_fl(adap, fl, min(MAX_RX_REFILL, fl_cap(fl) - fl->avail),
		  GFP_ATOMIC);
}

/**
 *	alloc_ring - allocate resources for an SGE descriptor ring
 *	@dev: the PCI device's core device
 *	@nelem: the number of descriptors
 *	@elem_size: the size of each descriptor
 *	@sw_size: the size of the SW state associated with each ring element
 *	@phys: the physical address of the allocated ring
 *	@metadata: address of the array holding the SW state for the ring
 *	@stat_size: extra space in HW ring for status information
 *	@node: preferred node for memory allocations
 *
 *	Allocates resources for an SGE descriptor ring, such as Tx queues,
 *	free buffer lists, or response queues.  Each SGE ring requires
 *	space for its HW descriptors plus, optionally, space for the SW state
 *	associated with each HW entry (the metadata).  The function returns
 *	three values: the virtual address for the HW ring (the return value
 *	of the function), the bus address of the HW ring, and the address
 *	of the SW ring.
 */
static void *alloc_ring(struct device *dev, size_t nelem, size_t elem_size,
			size_t sw_size, dma_addr_t *phys, void *metadata,
			size_t stat_size, int node)
{
	size_t len = nelem * elem_size + stat_size;
	void *s = NULL;
	void *p = dma_alloc_coherent(dev, len, phys, GFP_KERNEL);

	if (!p)
		return NULL;
	if (sw_size) {
		s = kcalloc_node(sw_size, nelem, GFP_KERNEL, node);

		if (!s) {
			dma_free_coherent(dev, len, p, *phys);
			return NULL;
		}
	}
	if (metadata)
		*(void **)metadata = s;
	return p;
}

/**
 *	sgl_len - calculates the size of an SGL of the given capacity
 *	@n: the number of SGL entries
 *
 *	Calculates the number of flits needed for a scatter/gather list that
 *	can hold the given number of entries.
 */
static inline unsigned int sgl_len(unsigned int n)
{
	/* A Direct Scatter Gather List uses 32-bit lengths and 64-bit PCI DMA
	 * addresses.  The DSGL Work Request starts off with a 32-bit DSGL
	 * ULPTX header, then Length0, then Address0, then, for 1 <= i <= N,
	 * repeated sequences of { Length[i], Length[i+1], Address[i],
	 * Address[i+1] } (this ensures that all addresses are on 64-bit
	 * boundaries).  If N is even, then Length[N+1] should be set to 0 and
	 * Address[N+1] is omitted.
	 *
	 * The following calculation incorporates all of the above.  It's
	 * somewhat hard to follow but, briefly: the "+2" accounts for the
	 * first two flits which include the DSGL header, Length0 and
	 * Address0; the "(3*(n-1))/2" covers the main body of list entries (3
	 * flits for every pair of the remaining N) +1 if (n-1) is odd; and
	 * finally the "+((n-1)&1)" adds the one remaining flit needed if
	 * (n-1) is odd ...
	 */
	n--;
	return (3 * n) / 2 + (n & 1) + 2;
}

/**
 *	flits_to_desc - returns the num of Tx descriptors for the given flits
 *	@n: the number of flits
 *
 *	Returns the number of Tx descriptors needed for the supplied number
 *	of flits.
 */
static inline unsigned int flits_to_desc(unsigned int n)
{
	BUG_ON(n > SGE_MAX_WR_LEN / 8);
	return DIV_ROUND_UP(n, 8);
}

/**
 *	is_eth_imm - can an Ethernet packet be sent as immediate data?
 *	@skb: the packet
 *	@chip_ver: chip version
 *
 *	Returns whether an Ethernet packet is small enough to fit as
 *	immediate data. Return value corresponds to headroom required.
 */
static inline int is_eth_imm(const struct sk_buff *skb, unsigned int chip_ver)
{
	int hdrlen = 0;

	if (skb->encapsulation && skb_shinfo(skb)->gso_size &&
	    chip_ver > CHELSIO_T5) {
		hdrlen = sizeof(struct cpl_tx_tnl_lso);
		hdrlen += sizeof(struct cpl_tx_pkt_core);
	} else if (skb_shinfo(skb)->gso_type & SKB_GSO_UDP_L4) {
		return 0;
	} else {
		hdrlen = skb_shinfo(skb)->gso_size ?
			 sizeof(struct cpl_tx_pkt_lso_core) : 0;
		hdrlen += sizeof(struct cpl_tx_pkt);
	}
	if (skb->len <= MAX_IMM_TX_PKT_LEN - hdrlen)
		return hdrlen;
	return 0;
}

/**
 *	calc_tx_flits - calculate the number of flits for a packet Tx WR
 *	@skb: the packet
 *	@chip_ver: chip version
 *
 *	Returns the number of flits needed for a Tx WR for the given Ethernet
 *	packet, including the needed WR and CPL headers.
 */
static inline unsigned int calc_tx_flits(const struct sk_buff *skb,
					 unsigned int chip_ver)
{
	unsigned int flits;
	int hdrlen = is_eth_imm(skb, chip_ver);

	/* If the skb is small enough, we can pump it out as a work request
	 * with only immediate data.  In that case we just have to have the
	 * TX Packet header plus the skb data in the Work Request.
	 */

	if (hdrlen)
		return DIV_ROUND_UP(skb->len + hdrlen, sizeof(__be64));

	/* Otherwise, we're going to have to construct a Scatter gather list
	 * of the skb body and fragments.  We also include the flits necessary
	 * for the TX Packet Work Request and CPL.  We always have a firmware
	 * Write Header (incorporated as part of the cpl_tx_pkt_lso and
	 * cpl_tx_pkt structures), followed by either a TX Packet Write CPL
	 * message or, if we're doing a Large Send Offload, an LSO CPL message
	 * with an embedded TX Packet Write CPL message.
	 */
	flits = sgl_len(skb_shinfo(skb)->nr_frags + 1);
	if (skb_shinfo(skb)->gso_size) {
		if (skb->encapsulation && chip_ver > CHELSIO_T5) {
			hdrlen = sizeof(struct fw_eth_tx_pkt_wr) +
				 sizeof(struct cpl_tx_tnl_lso);
		} else if (skb_shinfo(skb)->gso_type & SKB_GSO_UDP_L4) {
			u32 pkt_hdrlen;

			pkt_hdrlen = eth_get_headlen(skb->dev, skb->data,
						     skb_headlen(skb));
			hdrlen = sizeof(struct fw_eth_tx_eo_wr) +
				 round_up(pkt_hdrlen, 16);
		} else {
			hdrlen = sizeof(struct fw_eth_tx_pkt_wr) +
				 sizeof(struct cpl_tx_pkt_lso_core);
		}

		hdrlen += sizeof(struct cpl_tx_pkt_core);
		flits += (hdrlen / sizeof(__be64));
	} else {
		flits += (sizeof(struct fw_eth_tx_pkt_wr) +
			  sizeof(struct cpl_tx_pkt_core)) / sizeof(__be64);
	}
	return flits;
}

/**
 *	calc_tx_descs - calculate the number of Tx descriptors for a packet
 *	@skb: the packet
 *	@chip_ver: chip version
 *
 *	Returns the number of Tx descriptors needed for the given Ethernet
 *	packet, including the needed WR and CPL headers.
 */
static inline unsigned int calc_tx_descs(const struct sk_buff *skb,
					 unsigned int chip_ver)
{
	return flits_to_desc(calc_tx_flits(skb, chip_ver));
}

/**
 *	cxgb4_write_sgl - populate a scatter/gather list for a packet
 *	@skb: the packet
 *	@q: the Tx queue we are writing into
 *	@sgl: starting location for writing the SGL
 *	@end: points right after the end of the SGL
 *	@start: start offset into skb main-body data to include in the SGL
 *	@addr: the list of bus addresses for the SGL elements
 *
 *	Generates a gather list for the buffers that make up a packet.
 *	The caller must provide adequate space for the SGL that will be written.
 *	The SGL includes all of the packet's page fragments and the data in its
 *	main body except for the first @start bytes.  @sgl must be 16-byte
 *	aligned and within a Tx descriptor with available space.  @end points
 *	right after the end of the SGL but does not account for any potential
 *	wrap around, i.e., @end > @sgl.
 */
void cxgb4_write_sgl(const struct sk_buff *skb, struct sge_txq *q,
		     struct ulptx_sgl *sgl, u64 *end, unsigned int start,
		     const dma_addr_t *addr)
{
	unsigned int i, len;
	struct ulptx_sge_pair *to;
	const struct skb_shared_info *si = skb_shinfo(skb);
	unsigned int nfrags = si->nr_frags;
	struct ulptx_sge_pair buf[MAX_SKB_FRAGS / 2 + 1];

	len = skb_headlen(skb) - start;
	if (likely(len)) {
		sgl->len0 = htonl(len);
		sgl->addr0 = cpu_to_be64(addr[0] + start);
		nfrags++;
	} else {
		sgl->len0 = htonl(skb_frag_size(&si->frags[0]));
		sgl->addr0 = cpu_to_be64(addr[1]);
	}

	sgl->cmd_nsge = htonl(ULPTX_CMD_V(ULP_TX_SC_DSGL) |
			      ULPTX_NSGE_V(nfrags));
	if (likely(--nfrags == 0))
		return;
	/*
	 * Most of the complexity below deals with the possibility we hit the
	 * end of the queue in the middle of writing the SGL.  For this case
	 * only we create the SGL in a temporary buffer and then copy it.
	 */
	to = (u8 *)end > (u8 *)q->stat ? buf : sgl->sge;

	for (i = (nfrags != si->nr_frags); nfrags >= 2; nfrags -= 2, to++) {
		to->len[0] = cpu_to_be32(skb_frag_size(&si->frags[i]));
		to->len[1] = cpu_to_be32(skb_frag_size(&si->frags[++i]));
		to->addr[0] = cpu_to_be64(addr[i]);
		to->addr[1] = cpu_to_be64(addr[++i]);
	}
	if (nfrags) {
		to->len[0] = cpu_to_be32(skb_frag_size(&si->frags[i]));
		to->len[1] = cpu_to_be32(0);
		to->addr[0] = cpu_to_be64(addr[i + 1]);
	}
	if (unlikely((u8 *)end > (u8 *)q->stat)) {
		unsigned int part0 = (u8 *)q->stat - (u8 *)sgl->sge, part1;

		if (likely(part0))
			memcpy(sgl->sge, buf, part0);
		part1 = (u8 *)end - (u8 *)q->stat;
		memcpy(q->desc, (u8 *)buf + part0, part1);
		end = (void *)q->desc + part1;
	}
	if ((uintptr_t)end & 8)           /* 0-pad to multiple of 16 */
		*end = 0;
}
EXPORT_SYMBOL(cxgb4_write_sgl);

/*	cxgb4_write_partial_sgl - populate SGL for partial packet
 *	@skb: the packet
 *	@q: the Tx queue we are writing into
 *	@sgl: starting location for writing the SGL
 *	@end: points right after the end of the SGL
 *	@addr: the list of bus addresses for the SGL elements
 *	@start: start offset in the SKB where partial data starts
 *	@len: length of data from @start to send out
 *
 *	This API will handle sending out partial data of a skb if required.
 *	Unlike cxgb4_write_sgl, @start can be any offset into the skb data,
 *	and @len will decide how much data after @start offset to send out.
 */
void cxgb4_write_partial_sgl(const struct sk_buff *skb, struct sge_txq *q,
			     struct ulptx_sgl *sgl, u64 *end,
			     const dma_addr_t *addr, u32 start, u32 len)
{
	struct ulptx_sge_pair buf[MAX_SKB_FRAGS / 2 + 1] = {0}, *to;
	u32 frag_size, skb_linear_data_len = skb_headlen(skb);
	struct skb_shared_info *si = skb_shinfo(skb);
	u8 i = 0, frag_idx = 0, nfrags = 0;
	skb_frag_t *frag;

	/* Fill the first SGL either from linear data or from partial
	 * frag based on @start.
	 */
	if (unlikely(start < skb_linear_data_len)) {
		frag_size = min(len, skb_linear_data_len - start);
		sgl->len0 = htonl(frag_size);
		sgl->addr0 = cpu_to_be64(addr[0] + start);
		len -= frag_size;
		nfrags++;
	} else {
		start -= skb_linear_data_len;
		frag = &si->frags[frag_idx];
		frag_size = skb_frag_size(frag);
		/* find the first frag */
		while (start >= frag_size) {
			start -= frag_size;
			frag_idx++;
			frag = &si->frags[frag_idx];
			frag_size = skb_frag_size(frag);
		}

		frag_size = min(len, skb_frag_size(frag) - start);
		sgl->len0 = cpu_to_be32(frag_size);
		sgl->addr0 = cpu_to_be64(addr[frag_idx + 1] + start);
		len -= frag_size;
		nfrags++;
		frag_idx++;
	}

	/* If the entire partial data fit in one SGL, then send it out
	 * now.
	 */
	if (!len)
		goto done;

	/* Most of the complexity below deals with the possibility we hit the
	 * end of the queue in the middle of writing the SGL.  For this case
	 * only we create the SGL in a temporary buffer and then copy it.
	 */
	to = (u8 *)end > (u8 *)q->stat ? buf : sgl->sge;

	/* If the skb couldn't fit in first SGL completely, fill the
	 * rest of the frags in subsequent SGLs. Note that each SGL
	 * pair can store 2 frags.
	 */
	while (len) {
		frag_size = min(len, skb_frag_size(&si->frags[frag_idx]));
		to->len[i & 1] = cpu_to_be32(frag_size);
		to->addr[i & 1] = cpu_to_be64(addr[frag_idx + 1]);
		if (i && (i & 1))
			to++;
		nfrags++;
		frag_idx++;
		i++;
		len -= frag_size;
	}

	/* If we ended in an odd boundary, then set the second SGL's
	 * length in the pair to 0.
	 */
	if (i & 1)
		to->len[1] = cpu_to_be32(0);

	/* Copy from temporary buffer to Tx ring, in case we hit the
	 * end of the queue in the middle of writing the SGL.
	 */
	if (unlikely((u8 *)end > (u8 *)q->stat)) {
		u32 part0 = (u8 *)q->stat - (u8 *)sgl->sge, part1;

		if (likely(part0))
			memcpy(sgl->sge, buf, part0);
		part1 = (u8 *)end - (u8 *)q->stat;
		memcpy(q->desc, (u8 *)buf + part0, part1);
		end = (void *)q->desc + part1;
	}

	/* 0-pad to multiple of 16 */
	if ((uintptr_t)end & 8)
		*end = 0;
done:
	sgl->cmd_nsge = htonl(ULPTX_CMD_V(ULP_TX_SC_DSGL) |
			ULPTX_NSGE_V(nfrags));
}
EXPORT_SYMBOL(cxgb4_write_partial_sgl);

/* This function copies 64 byte coalesced work request to
 * memory mapped BAR2 space. For coalesced WR SGE fetches
 * data from the FIFO instead of from Host.
 */
static void cxgb_pio_copy(u64 __iomem *dst, u64 *src)
{
	int count = 8;

	while (count) {
		writeq(*src, dst);
		src++;
		dst++;
		count--;
	}
}

/**
 *	cxgb4_ring_tx_db - check and potentially ring a Tx queue's doorbell
 *	@adap: the adapter
 *	@q: the Tx queue
 *	@n: number of new descriptors to give to HW
 *
 *	Ring the doorbel for a Tx queue.
 */
inline void cxgb4_ring_tx_db(struct adapter *adap, struct sge_txq *q, int n)
{
	/* Make sure that all writes to the TX Descriptors are committed
	 * before we tell the hardware about them.
	 */
	wmb();

	/* If we don't have access to the new User Doorbell (T5+), use the old
	 * doorbell mechanism; otherwise use the new BAR2 mechanism.
	 */
	if (unlikely(q->bar2_addr == NULL)) {
		u32 val = PIDX_V(n);
		unsigned long flags;

		/* For T4 we need to participate in the Doorbell Recovery
		 * mechanism.
		 */
		spin_lock_irqsave(&q->db_lock, flags);
		if (!q->db_disabled)
			t4_write_reg(adap, MYPF_REG(SGE_PF_KDOORBELL_A),
				     QID_V(q->cntxt_id) | val);
		else
			q->db_pidx_inc += n;
		q->db_pidx = q->pidx;
		spin_unlock_irqrestore(&q->db_lock, flags);
	} else {
		u32 val = PIDX_T5_V(n);

		/* T4 and later chips share the same PIDX field offset within
		 * the doorbell, but T5 and later shrank the field in order to
		 * gain a bit for Doorbell Priority.  The field was absurdly
		 * large in the first place (14 bits) so we just use the T5
		 * and later limits and warn if a Queue ID is too large.
		 */
		WARN_ON(val & DBPRIO_F);

		/* If we're only writing a single TX Descriptor and we can use
		 * Inferred QID registers, we can use the Write Combining
		 * Gather Buffer; otherwise we use the simple doorbell.
		 */
		if (n == 1 && q->bar2_qid == 0) {
			int index = (q->pidx
				     ? (q->pidx - 1)
				     : (q->size - 1));
			u64 *wr = (u64 *)&q->desc[index];

			cxgb_pio_copy((u64 __iomem *)
				      (q->bar2_addr + SGE_UDB_WCDOORBELL),
				      wr);
		} else {
			writel(val | QID_V(q->bar2_qid),
			       q->bar2_addr + SGE_UDB_KDOORBELL);
		}

		/* This Write Memory Barrier will force the write to the User
		 * Doorbell area to be flushed.  This is needed to prevent
		 * writes on different CPUs for the same queue from hitting
		 * the adapter out of order.  This is required when some Work
		 * Requests take the Write Combine Gather Buffer path (user
		 * doorbell area offset [SGE_UDB_WCDOORBELL..+63]) and some
		 * take the traditional path where we simply increment the
		 * PIDX (User Doorbell area SGE_UDB_KDOORBELL) and have the
		 * hardware DMA read the actual Work Request.
		 */
		wmb();
	}
}
EXPORT_SYMBOL(cxgb4_ring_tx_db);

/**
 *	cxgb4_inline_tx_skb - inline a packet's data into Tx descriptors
 *	@skb: the packet
 *	@q: the Tx queue where the packet will be inlined
 *	@pos: starting position in the Tx queue where to inline the packet
 *
 *	Inline a packet's contents directly into Tx descriptors, starting at
 *	the given position within the Tx DMA ring.
 *	Most of the complexity of this operation is dealing with wrap arounds
 *	in the middle of the packet we want to inline.
 */
void cxgb4_inline_tx_skb(const struct sk_buff *skb,
			 const struct sge_txq *q, void *pos)
{
	int left = (void *)q->stat - pos;
	u64 *p;

	if (likely(skb->len <= left)) {
		if (likely(!skb->data_len))
			skb_copy_from_linear_data(skb, pos, skb->len);
		else
			skb_copy_bits(skb, 0, pos, skb->len);
		pos += skb->len;
	} else {
		skb_copy_bits(skb, 0, pos, left);
		skb_copy_bits(skb, left, q->desc, skb->len - left);
		pos = (void *)q->desc + (skb->len - left);
	}

	/* 0-pad to multiple of 16 */
	p = PTR_ALIGN(pos, 8);
	if ((uintptr_t)p & 8)
		*p = 0;
}
EXPORT_SYMBOL(cxgb4_inline_tx_skb);

static void *inline_tx_skb_header(const struct sk_buff *skb,
				  const struct sge_txq *q,  void *pos,
				  int length)
{
	u64 *p;
	int left = (void *)q->stat - pos;

	if (likely(length <= left)) {
		memcpy(pos, skb->data, length);
		pos += length;
	} else {
		memcpy(pos, skb->data, left);
		memcpy(q->desc, skb->data + left, length - left);
		pos = (void *)q->desc + (length - left);
	}
	/* 0-pad to multiple of 16 */
	p = PTR_ALIGN(pos, 8);
	if ((uintptr_t)p & 8) {
		*p = 0;
		return p + 1;
	}
	return p;
}

/*
 * Figure out what HW csum a packet wants and return the appropriate control
 * bits.
 */
static u64 hwcsum(enum chip_type chip, const struct sk_buff *skb)
{
	int csum_type;
	bool inner_hdr_csum = false;
	u16 proto, ver;

	if (skb->encapsulation &&
	    (CHELSIO_CHIP_VERSION(chip) > CHELSIO_T5))
		inner_hdr_csum = true;

	if (inner_hdr_csum) {
		ver = inner_ip_hdr(skb)->version;
		proto = (ver == 4) ? inner_ip_hdr(skb)->protocol :
			inner_ipv6_hdr(skb)->nexthdr;
	} else {
		ver = ip_hdr(skb)->version;
		proto = (ver == 4) ? ip_hdr(skb)->protocol :
			ipv6_hdr(skb)->nexthdr;
	}

	if (ver == 4) {
		if (proto == IPPROTO_TCP)
			csum_type = TX_CSUM_TCPIP;
		else if (proto == IPPROTO_UDP)
			csum_type = TX_CSUM_UDPIP;
		else {
nocsum:			/*
			 * unknown protocol, disable HW csum
			 * and hope a bad packet is detected
			 */
			return TXPKT_L4CSUM_DIS_F;
		}
	} else {
		/*
		 * this doesn't work with extension headers
		 */
		if (proto == IPPROTO_TCP)
			csum_type = TX_CSUM_TCPIP6;
		else if (proto == IPPROTO_UDP)
			csum_type = TX_CSUM_UDPIP6;
		else
			goto nocsum;
	}

	if (likely(csum_type >= TX_CSUM_TCPIP)) {
		int eth_hdr_len, l4_len;
		u64 hdr_len;

		if (inner_hdr_csum) {
			/* This allows checksum offload for all encapsulated
			 * packets like GRE etc..
			 */
			l4_len = skb_inner_network_header_len(skb);
			eth_hdr_len = skb_inner_network_offset(skb) - ETH_HLEN;
		} else {
			l4_len = skb_network_header_len(skb);
			eth_hdr_len = skb_network_offset(skb) - ETH_HLEN;
		}
		hdr_len = TXPKT_IPHDR_LEN_V(l4_len);

		if (CHELSIO_CHIP_VERSION(chip) <= CHELSIO_T5)
			hdr_len |= TXPKT_ETHHDR_LEN_V(eth_hdr_len);
		else
			hdr_len |= T6_TXPKT_ETHHDR_LEN_V(eth_hdr_len);
		return TXPKT_CSUM_TYPE_V(csum_type) | hdr_len;
	} else {
		int start = skb_transport_offset(skb);

		return TXPKT_CSUM_TYPE_V(csum_type) |
			TXPKT_CSUM_START_V(start) |
			TXPKT_CSUM_LOC_V(start + skb->csum_offset);
	}
}

static void eth_txq_stop(struct sge_eth_txq *q)
{
	netif_tx_stop_queue(q->txq);
	q->q.stops++;
}

static inline void txq_advance(struct sge_txq *q, unsigned int n)
{
	q->in_use += n;
	q->pidx += n;
	if (q->pidx >= q->size)
		q->pidx -= q->size;
}

#ifdef CONFIG_CHELSIO_T4_FCOE
static inline int
cxgb_fcoe_offload(struct sk_buff *skb, struct adapter *adap,
		  const struct port_info *pi, u64 *cntrl)
{
	const struct cxgb_fcoe *fcoe = &pi->fcoe;

	if (!(fcoe->flags & CXGB_FCOE_ENABLED))
		return 0;

	if (skb->protocol != htons(ETH_P_FCOE))
		return 0;

	skb_reset_mac_header(skb);
	skb->mac_len = sizeof(struct ethhdr);

	skb_set_network_header(skb, skb->mac_len);
	skb_set_transport_header(skb, skb->mac_len + sizeof(struct fcoe_hdr));

	if (!cxgb_fcoe_sof_eof_supported(adap, skb))
		return -ENOTSUPP;

	/* FC CRC offload */
	*cntrl = TXPKT_CSUM_TYPE_V(TX_CSUM_FCOE) |
		     TXPKT_L4CSUM_DIS_F | TXPKT_IPCSUM_DIS_F |
		     TXPKT_CSUM_START_V(CXGB_FCOE_TXPKT_CSUM_START) |
		     TXPKT_CSUM_END_V(CXGB_FCOE_TXPKT_CSUM_END) |
		     TXPKT_CSUM_LOC_V(CXGB_FCOE_TXPKT_CSUM_END);
	return 0;
}
#endif /* CONFIG_CHELSIO_T4_FCOE */

/* Returns tunnel type if hardware supports offloading of the same.
 * It is called only for T5 and onwards.
 */
enum cpl_tx_tnl_lso_type cxgb_encap_offload_supported(struct sk_buff *skb)
{
	u8 l4_hdr = 0;
	enum cpl_tx_tnl_lso_type tnl_type = TX_TNL_TYPE_OPAQUE;
	struct port_info *pi = netdev_priv(skb->dev);
	struct adapter *adapter = pi->adapter;

	if (skb->inner_protocol_type != ENCAP_TYPE_ETHER ||
	    skb->inner_protocol != htons(ETH_P_TEB))
		return tnl_type;

	switch (vlan_get_protocol(skb)) {
	case htons(ETH_P_IP):
		l4_hdr = ip_hdr(skb)->protocol;
		break;
	case htons(ETH_P_IPV6):
		l4_hdr = ipv6_hdr(skb)->nexthdr;
		break;
	default:
		return tnl_type;
	}

	switch (l4_hdr) {
	case IPPROTO_UDP:
		if (adapter->vxlan_port == udp_hdr(skb)->dest)
			tnl_type = TX_TNL_TYPE_VXLAN;
		else if (adapter->geneve_port == udp_hdr(skb)->dest)
			tnl_type = TX_TNL_TYPE_GENEVE;
		break;
	default:
		return tnl_type;
	}

	return tnl_type;
}

static inline void t6_fill_tnl_lso(struct sk_buff *skb,
				   struct cpl_tx_tnl_lso *tnl_lso,
				   enum cpl_tx_tnl_lso_type tnl_type)
{
	u32 val;
	int in_eth_xtra_len;
	int l3hdr_len = skb_network_header_len(skb);
	int eth_xtra_len = skb_network_offset(skb) - ETH_HLEN;
	const struct skb_shared_info *ssi = skb_shinfo(skb);
	bool v6 = (ip_hdr(skb)->version == 6);

	val = CPL_TX_TNL_LSO_OPCODE_V(CPL_TX_TNL_LSO) |
	      CPL_TX_TNL_LSO_FIRST_F |
	      CPL_TX_TNL_LSO_LAST_F |
	      (v6 ? CPL_TX_TNL_LSO_IPV6OUT_F : 0) |
	      CPL_TX_TNL_LSO_ETHHDRLENOUT_V(eth_xtra_len / 4) |
	      CPL_TX_TNL_LSO_IPHDRLENOUT_V(l3hdr_len / 4) |
	      (v6 ? 0 : CPL_TX_TNL_LSO_IPHDRCHKOUT_F) |
	      CPL_TX_TNL_LSO_IPLENSETOUT_F |
	      (v6 ? 0 : CPL_TX_TNL_LSO_IPIDINCOUT_F);
	tnl_lso->op_to_IpIdSplitOut = htonl(val);

	tnl_lso->IpIdOffsetOut = 0;

	/* Get the tunnel header length */
	val = skb_inner_mac_header(skb) - skb_mac_header(skb);
	in_eth_xtra_len = skb_inner_network_header(skb) -
			  skb_inner_mac_header(skb) - ETH_HLEN;

	switch (tnl_type) {
	case TX_TNL_TYPE_VXLAN:
	case TX_TNL_TYPE_GENEVE:
		tnl_lso->UdpLenSetOut_to_TnlHdrLen =
			htons(CPL_TX_TNL_LSO_UDPCHKCLROUT_F |
			CPL_TX_TNL_LSO_UDPLENSETOUT_F);
		break;
	default:
		tnl_lso->UdpLenSetOut_to_TnlHdrLen = 0;
		break;
	}

	tnl_lso->UdpLenSetOut_to_TnlHdrLen |=
		 htons(CPL_TX_TNL_LSO_TNLHDRLEN_V(val) |
		       CPL_TX_TNL_LSO_TNLTYPE_V(tnl_type));

	tnl_lso->r1 = 0;

	val = CPL_TX_TNL_LSO_ETHHDRLEN_V(in_eth_xtra_len / 4) |
	      CPL_TX_TNL_LSO_IPV6_V(inner_ip_hdr(skb)->version == 6) |
	      CPL_TX_TNL_LSO_IPHDRLEN_V(skb_inner_network_header_len(skb) / 4) |
	      CPL_TX_TNL_LSO_TCPHDRLEN_V(inner_tcp_hdrlen(skb) / 4);
	tnl_lso->Flow_to_TcpHdrLen = htonl(val);

	tnl_lso->IpIdOffset = htons(0);

	tnl_lso->IpIdSplit_to_Mss = htons(CPL_TX_TNL_LSO_MSS_V(ssi->gso_size));
	tnl_lso->TCPSeqOffset = htonl(0);
	tnl_lso->EthLenOffset_Size = htonl(CPL_TX_TNL_LSO_SIZE_V(skb->len));
}

static inline void *write_tso_wr(struct adapter *adap, struct sk_buff *skb,
				 struct cpl_tx_pkt_lso_core *lso)
{
	int eth_xtra_len = skb_network_offset(skb) - ETH_HLEN;
	int l3hdr_len = skb_network_header_len(skb);
	const struct skb_shared_info *ssi;
	bool ipv6 = false;

	ssi = skb_shinfo(skb);
	if (ssi->gso_type & SKB_GSO_TCPV6)
		ipv6 = true;

	lso->lso_ctrl = htonl(LSO_OPCODE_V(CPL_TX_PKT_LSO) |
			      LSO_FIRST_SLICE_F | LSO_LAST_SLICE_F |
			      LSO_IPV6_V(ipv6) |
			      LSO_ETHHDR_LEN_V(eth_xtra_len / 4) |
			      LSO_IPHDR_LEN_V(l3hdr_len / 4) |
			      LSO_TCPHDR_LEN_V(tcp_hdr(skb)->doff));
	lso->ipid_ofst = htons(0);
	lso->mss = htons(ssi->gso_size);
	lso->seqno_offset = htonl(0);
	if (is_t4(adap->params.chip))
		lso->len = htonl(skb->len);
	else
		lso->len = htonl(LSO_T5_XFER_SIZE_V(skb->len));

	return (void *)(lso + 1);
}

/**
 *	t4_sge_eth_txq_egress_update - handle Ethernet TX Queue update
 *	@adap: the adapter
 *	@eq: the Ethernet TX Queue
 *	@maxreclaim: the maximum number of TX Descriptors to reclaim or -1
 *
 *	We're typically called here to update the state of an Ethernet TX
 *	Queue with respect to the hardware's progress in consuming the TX
 *	Work Requests that we've put on that Egress Queue.  This happens
 *	when we get Egress Queue Update messages and also prophylactically
 *	in regular timer-based Ethernet TX Queue maintenance.
 */
int t4_sge_eth_txq_egress_update(struct adapter *adap, struct sge_eth_txq *eq,
				 int maxreclaim)
{
	unsigned int reclaimed, hw_cidx;
	struct sge_txq *q = &eq->q;
	int hw_in_use;

	if (!q->in_use || !__netif_tx_trylock(eq->txq))
		return 0;

	/* Reclaim pending completed TX Descriptors. */
	reclaimed = reclaim_completed_tx(adap, &eq->q, maxreclaim, true);

	hw_cidx = ntohs(READ_ONCE(q->stat->cidx));
	hw_in_use = q->pidx - hw_cidx;
	if (hw_in_use < 0)
		hw_in_use += q->size;

	/* If the TX Queue is currently stopped and there's now more than half
	 * the queue available, restart it.  Otherwise bail out since the rest
	 * of what we want do here is with the possibility of shipping any
	 * currently buffered Coalesced TX Work Request.
	 */
	if (netif_tx_queue_stopped(eq->txq) && hw_in_use < (q->size / 2)) {
		netif_tx_wake_queue(eq->txq);
		eq->q.restarts++;
	}

	__netif_tx_unlock(eq->txq);
	return reclaimed;
}

static inline int cxgb4_validate_skb(struct sk_buff *skb,
				     struct net_device *dev,
				     u32 min_pkt_len)
{
	u32 max_pkt_len;

	/* The chip min packet length is 10 octets but some firmware
	 * commands have a minimum packet length requirement. So, play
	 * safe and reject anything shorter than @min_pkt_len.
	 */
	if (unlikely(skb->len < min_pkt_len))
		return -EINVAL;

	/* Discard the packet if the length is greater than mtu */
	max_pkt_len = ETH_HLEN + dev->mtu;

	if (skb_vlan_tagged(skb))
		max_pkt_len += VLAN_HLEN;

	if (!skb_shinfo(skb)->gso_size && (unlikely(skb->len > max_pkt_len)))
		return -EINVAL;

	return 0;
}

static void *write_eo_udp_wr(struct sk_buff *skb, struct fw_eth_tx_eo_wr *wr,
			     u32 hdr_len)
{
	wr->u.udpseg.type = FW_ETH_TX_EO_TYPE_UDPSEG;
	wr->u.udpseg.ethlen = skb_network_offset(skb);
	wr->u.udpseg.iplen = cpu_to_be16(skb_network_header_len(skb));
	wr->u.udpseg.udplen = sizeof(struct udphdr);
	wr->u.udpseg.rtplen = 0;
	wr->u.udpseg.r4 = 0;
	if (skb_shinfo(skb)->gso_size)
		wr->u.udpseg.mss = cpu_to_be16(skb_shinfo(skb)->gso_size);
	else
		wr->u.udpseg.mss = cpu_to_be16(skb->len - hdr_len);
	wr->u.udpseg.schedpktsize = wr->u.udpseg.mss;
	wr->u.udpseg.plen = cpu_to_be32(skb->len - hdr_len);

	return (void *)(wr + 1);
}

/**
 *	cxgb4_eth_xmit - add a packet to an Ethernet Tx queue
 *	@skb: the packet
 *	@dev: the egress net device
 *
 *	Add a packet to an SGE Ethernet Tx queue.  Runs with softirqs disabled.
 */
static netdev_tx_t cxgb4_eth_xmit(struct sk_buff *skb, struct net_device *dev)
{
	enum cpl_tx_tnl_lso_type tnl_type = TX_TNL_TYPE_OPAQUE;
	bool ptp_enabled = is_ptp_enabled(skb, dev);
	unsigned int last_desc, flits, ndesc;
	u32 wr_mid, ctrl0, op, sgl_off = 0;
	const struct skb_shared_info *ssi;
	int len, qidx, credits, ret, left;
	struct tx_sw_desc *sgl_sdesc;
	struct fw_eth_tx_eo_wr *eowr;
	struct fw_eth_tx_pkt_wr *wr;
	struct cpl_tx_pkt_core *cpl;
	const struct port_info *pi;
	bool immediate = false;
	u64 cntrl, *end, *sgl;
	struct sge_eth_txq *q;
	unsigned int chip_ver;
	struct adapter *adap;

	ret = cxgb4_validate_skb(skb, dev, ETH_HLEN);
	if (ret)
		goto out_free;

	pi = netdev_priv(dev);
	adap = pi->adapter;
	ssi = skb_shinfo(skb);
#if IS_ENABLED(CONFIG_CHELSIO_IPSEC_INLINE)
	if (xfrm_offload(skb) && !ssi->gso_size)
		return adap->uld[CXGB4_ULD_IPSEC].tx_handler(skb, dev);
#endif /* CHELSIO_IPSEC_INLINE */

#if IS_ENABLED(CONFIG_CHELSIO_TLS_DEVICE)
	if (cxgb4_is_ktls_skb(skb) &&
	    (skb->len - (skb_transport_offset(skb) + tcp_hdrlen(skb))))
		return adap->uld[CXGB4_ULD_KTLS].tx_handler(skb, dev);
#endif /* CHELSIO_TLS_DEVICE */

	qidx = skb_get_queue_mapping(skb);
	if (ptp_enabled) {
		if (!(adap->ptp_tx_skb)) {
			skb_shinfo(skb)->tx_flags |= SKBTX_IN_PROGRESS;
			adap->ptp_tx_skb = skb_get(skb);
		} else {
			goto out_free;
		}
		q = &adap->sge.ptptxq;
	} else {
		q = &adap->sge.ethtxq[qidx + pi->first_qset];
	}
	skb_tx_timestamp(skb);

	reclaim_completed_tx(adap, &q->q, -1, true);
	cntrl = TXPKT_L4CSUM_DIS_F | TXPKT_IPCSUM_DIS_F;

#ifdef CONFIG_CHELSIO_T4_FCOE
	ret = cxgb_fcoe_offload(skb, adap, pi, &cntrl);
	if (unlikely(ret == -EOPNOTSUPP))
		goto out_free;
#endif /* CONFIG_CHELSIO_T4_FCOE */

	chip_ver = CHELSIO_CHIP_VERSION(adap->params.chip);
	flits = calc_tx_flits(skb, chip_ver);
	ndesc = flits_to_desc(flits);
	credits = txq_avail(&q->q) - ndesc;

	if (unlikely(credits < 0)) {
		eth_txq_stop(q);
		dev_err(adap->pdev_dev,
			"%s: Tx ring %u full while queue awake!\n",
			dev->name, qidx);
		return NETDEV_TX_BUSY;
	}

	if (is_eth_imm(skb, chip_ver))
		immediate = true;

	if (skb->encapsulation && chip_ver > CHELSIO_T5)
		tnl_type = cxgb_encap_offload_supported(skb);

	last_desc = q->q.pidx + ndesc - 1;
	if (last_desc >= q->q.size)
		last_desc -= q->q.size;
	sgl_sdesc = &q->q.sdesc[last_desc];

	if (!immediate &&
	    unlikely(cxgb4_map_skb(adap->pdev_dev, skb, sgl_sdesc->addr) < 0)) {
		memset(sgl_sdesc->addr, 0, sizeof(sgl_sdesc->addr));
		q->mapping_err++;
		goto out_free;
	}

	wr_mid = FW_WR_LEN16_V(DIV_ROUND_UP(flits, 2));
	if (unlikely(credits < ETHTXQ_STOP_THRES)) {
		/* After we're done injecting the Work Request for this
		 * packet, we'll be below our "stop threshold" so stop the TX
		 * Queue now and schedule a request for an SGE Egress Queue
		 * Update message. The queue will get started later on when
		 * the firmware processes this Work Request and sends us an
		 * Egress Queue Status Update message indicating that space
		 * has opened up.
		 */
		eth_txq_stop(q);
		if (chip_ver > CHELSIO_T5)
			wr_mid |= FW_WR_EQUEQ_F | FW_WR_EQUIQ_F;
	}

	wr = (void *)&q->q.desc[q->q.pidx];
	eowr = (void *)&q->q.desc[q->q.pidx];
	wr->equiq_to_len16 = htonl(wr_mid);
	wr->r3 = cpu_to_be64(0);
	if (skb_shinfo(skb)->gso_type & SKB_GSO_UDP_L4)
		end = (u64 *)eowr + flits;
	else
		end = (u64 *)wr + flits;

	len = immediate ? skb->len : 0;
	len += sizeof(*cpl);
	if (ssi->gso_size && !(ssi->gso_type & SKB_GSO_UDP_L4)) {
		struct cpl_tx_pkt_lso_core *lso = (void *)(wr + 1);
		struct cpl_tx_tnl_lso *tnl_lso = (void *)(wr + 1);

		if (tnl_type)
			len += sizeof(*tnl_lso);
		else
			len += sizeof(*lso);

		wr->op_immdlen = htonl(FW_WR_OP_V(FW_ETH_TX_PKT_WR) |
				       FW_WR_IMMDLEN_V(len));
		if (tnl_type) {
			struct iphdr *iph = ip_hdr(skb);

			t6_fill_tnl_lso(skb, tnl_lso, tnl_type);
			cpl = (void *)(tnl_lso + 1);
			/* Driver is expected to compute partial checksum that
			 * does not include the IP Total Length.
			 */
			if (iph->version == 4) {
				iph->check = 0;
				iph->tot_len = 0;
				iph->check = ~ip_fast_csum((u8 *)iph, iph->ihl);
			}
			if (skb->ip_summed == CHECKSUM_PARTIAL)
				cntrl = hwcsum(adap->params.chip, skb);
		} else {
			cpl = write_tso_wr(adap, skb, lso);
			cntrl = hwcsum(adap->params.chip, skb);
		}
		sgl = (u64 *)(cpl + 1); /* sgl start here */
		q->tso++;
		q->tx_cso += ssi->gso_segs;
	} else if (ssi->gso_size) {
		u64 *start;
		u32 hdrlen;

		hdrlen = eth_get_headlen(dev, skb->data, skb_headlen(skb));
		len += hdrlen;
		wr->op_immdlen = cpu_to_be32(FW_WR_OP_V(FW_ETH_TX_EO_WR) |
					     FW_ETH_TX_EO_WR_IMMDLEN_V(len));
		cpl = write_eo_udp_wr(skb, eowr, hdrlen);
		cntrl = hwcsum(adap->params.chip, skb);

		start = (u64 *)(cpl + 1);
		sgl = (u64 *)inline_tx_skb_header(skb, &q->q, (void *)start,
						  hdrlen);
		if (unlikely(start > sgl)) {
			left = (u8 *)end - (u8 *)q->q.stat;
			end = (void *)q->q.desc + left;
		}
		sgl_off = hdrlen;
		q->uso++;
		q->tx_cso += ssi->gso_segs;
	} else {
		if (ptp_enabled)
			op = FW_PTP_TX_PKT_WR;
		else
			op = FW_ETH_TX_PKT_WR;
		wr->op_immdlen = htonl(FW_WR_OP_V(op) |
				       FW_WR_IMMDLEN_V(len));
		cpl = (void *)(wr + 1);
		sgl = (u64 *)(cpl + 1);
		if (skb->ip_summed == CHECKSUM_PARTIAL) {
			cntrl = hwcsum(adap->params.chip, skb) |
				TXPKT_IPCSUM_DIS_F;
			q->tx_cso++;
		}
	}

	if (unlikely((u8 *)sgl >= (u8 *)q->q.stat)) {
		/* If current position is already at the end of the
		 * txq, reset the current to point to start of the queue
		 * and update the end ptr as well.
		 */
		left = (u8 *)end - (u8 *)q->q.stat;
		end = (void *)q->q.desc + left;
		sgl = (void *)q->q.desc;
	}

	if (skb_vlan_tag_present(skb)) {
		q->vlan_ins++;
		cntrl |= TXPKT_VLAN_VLD_F | TXPKT_VLAN_V(skb_vlan_tag_get(skb));
#ifdef CONFIG_CHELSIO_T4_FCOE
		if (skb->protocol == htons(ETH_P_FCOE))
			cntrl |= TXPKT_VLAN_V(
				 ((skb->priority & 0x7) << VLAN_PRIO_SHIFT));
#endif /* CONFIG_CHELSIO_T4_FCOE */
	}

	ctrl0 = TXPKT_OPCODE_V(CPL_TX_PKT_XT) | TXPKT_INTF_V(pi->tx_chan) |
		TXPKT_PF_V(adap->pf);
	if (ptp_enabled)
		ctrl0 |= TXPKT_TSTAMP_F;
#ifdef CONFIG_CHELSIO_T4_DCB
	if (is_t4(adap->params.chip))
		ctrl0 |= TXPKT_OVLAN_IDX_V(q->dcb_prio);
	else
		ctrl0 |= TXPKT_T5_OVLAN_IDX_V(q->dcb_prio);
#endif
	cpl->ctrl0 = htonl(ctrl0);
	cpl->pack = htons(0);
	cpl->len = htons(skb->len);
	cpl->ctrl1 = cpu_to_be64(cntrl);

	if (immediate) {
		cxgb4_inline_tx_skb(skb, &q->q, sgl);
		dev_consume_skb_any(skb);
	} else {
		cxgb4_write_sgl(skb, &q->q, (void *)sgl, end, sgl_off,
				sgl_sdesc->addr);
		skb_orphan(skb);
		sgl_sdesc->skb = skb;
	}

	txq_advance(&q->q, ndesc);

	cxgb4_ring_tx_db(adap, &q->q, ndesc);
	return NETDEV_TX_OK;

out_free:
	dev_kfree_skb_any(skb);
	return NETDEV_TX_OK;
}

/* Constants ... */
enum {
	/* Egress Queue sizes, producer and consumer indices are all in units
	 * of Egress Context Units bytes.  Note that as far as the hardware is
	 * concerned, the free list is an Egress Queue (the host produces free
	 * buffers which the hardware consumes) and free list entries are
	 * 64-bit PCI DMA addresses.
	 */
	EQ_UNIT = SGE_EQ_IDXSIZE,
	FL_PER_EQ_UNIT = EQ_UNIT / sizeof(__be64),
	TXD_PER_EQ_UNIT = EQ_UNIT / sizeof(__be64),

	T4VF_ETHTXQ_MAX_HDR = (sizeof(struct fw_eth_tx_pkt_vm_wr) +
			       sizeof(struct cpl_tx_pkt_lso_core) +
			       sizeof(struct cpl_tx_pkt_core)) / sizeof(__be64),
};

/**
 *	t4vf_is_eth_imm - can an Ethernet packet be sent as immediate data?
 *	@skb: the packet
 *
 *	Returns whether an Ethernet packet is small enough to fit completely as
 *	immediate data.
 */
static inline int t4vf_is_eth_imm(const struct sk_buff *skb)
{
	/* The VF Driver uses the FW_ETH_TX_PKT_VM_WR firmware Work Request
	 * which does not accommodate immediate data.  We could dike out all
	 * of the support code for immediate data but that would tie our hands
	 * too much if we ever want to enhace the firmware.  It would also
	 * create more differences between the PF and VF Drivers.
	 */
	return false;
}

/**
 *	t4vf_calc_tx_flits - calculate the number of flits for a packet TX WR
 *	@skb: the packet
 *
 *	Returns the number of flits needed for a TX Work Request for the
 *	given Ethernet packet, including the needed WR and CPL headers.
 */
static inline unsigned int t4vf_calc_tx_flits(const struct sk_buff *skb)
{
	unsigned int flits;

	/* If the skb is small enough, we can pump it out as a work request
	 * with only immediate data.  In that case we just have to have the
	 * TX Packet header plus the skb data in the Work Request.
	 */
	if (t4vf_is_eth_imm(skb))
		return DIV_ROUND_UP(skb->len + sizeof(struct cpl_tx_pkt),
				    sizeof(__be64));

	/* Otherwise, we're going to have to construct a Scatter gather list
	 * of the skb body and fragments.  We also include the flits necessary
	 * for the TX Packet Work Request and CPL.  We always have a firmware
	 * Write Header (incorporated as part of the cpl_tx_pkt_lso and
	 * cpl_tx_pkt structures), followed by either a TX Packet Write CPL
	 * message or, if we're doing a Large Send Offload, an LSO CPL message
	 * with an embedded TX Packet Write CPL message.
	 */
	flits = sgl_len(skb_shinfo(skb)->nr_frags + 1);
	if (skb_shinfo(skb)->gso_size)
		flits += (sizeof(struct fw_eth_tx_pkt_vm_wr) +
			  sizeof(struct cpl_tx_pkt_lso_core) +
			  sizeof(struct cpl_tx_pkt_core)) / sizeof(__be64);
	else
		flits += (sizeof(struct fw_eth_tx_pkt_vm_wr) +
			  sizeof(struct cpl_tx_pkt_core)) / sizeof(__be64);
	return flits;
}

/**
 *	cxgb4_vf_eth_xmit - add a packet to an Ethernet TX queue
 *	@skb: the packet
 *	@dev: the egress net device
 *
 *	Add a packet to an SGE Ethernet TX queue.  Runs with softirqs disabled.
 */
static netdev_tx_t cxgb4_vf_eth_xmit(struct sk_buff *skb,
				     struct net_device *dev)
{
	unsigned int last_desc, flits, ndesc;
	const struct skb_shared_info *ssi;
	struct fw_eth_tx_pkt_vm_wr *wr;
	struct tx_sw_desc *sgl_sdesc;
	struct cpl_tx_pkt_core *cpl;
	const struct port_info *pi;
	struct sge_eth_txq *txq;
	struct adapter *adapter;
	int qidx, credits, ret;
	size_t fw_hdr_copy_len;
	unsigned int chip_ver;
	u64 cntrl, *end;
	u32 wr_mid;

	/* The chip minimum packet length is 10 octets but the firmware
	 * command that we are using requires that we copy the Ethernet header
	 * (including the VLAN tag) into the header so we reject anything
	 * smaller than that ...
	 */
	fw_hdr_copy_len = sizeof(wr->ethmacdst) + sizeof(wr->ethmacsrc) +
			  sizeof(wr->ethtype) + sizeof(wr->vlantci);
	ret = cxgb4_validate_skb(skb, dev, fw_hdr_copy_len);
	if (ret)
		goto out_free;

	/* Figure out which TX Queue we're going to use. */
	pi = netdev_priv(dev);
	adapter = pi->adapter;
	qidx = skb_get_queue_mapping(skb);
	WARN_ON(qidx >= pi->nqsets);
	txq = &adapter->sge.ethtxq[pi->first_qset + qidx];

	/* Take this opportunity to reclaim any TX Descriptors whose DMA
	 * transfers have completed.
	 */
	reclaim_completed_tx(adapter, &txq->q, -1, true);

	/* Calculate the number of flits and TX Descriptors we're going to
	 * need along with how many TX Descriptors will be left over after
	 * we inject our Work Request.
	 */
	flits = t4vf_calc_tx_flits(skb);
	ndesc = flits_to_desc(flits);
	credits = txq_avail(&txq->q) - ndesc;

	if (unlikely(credits < 0)) {
		/* Not enough room for this packet's Work Request.  Stop the
		 * TX Queue and return a "busy" condition.  The queue will get
		 * started later on when the firmware informs us that space
		 * has opened up.
		 */
		eth_txq_stop(txq);
		dev_err(adapter->pdev_dev,
			"%s: TX ring %u full while queue awake!\n",
			dev->name, qidx);
		return NETDEV_TX_BUSY;
	}

	last_desc = txq->q.pidx + ndesc - 1;
	if (last_desc >= txq->q.size)
		last_desc -= txq->q.size;
	sgl_sdesc = &txq->q.sdesc[last_desc];

	if (!t4vf_is_eth_imm(skb) &&
	    unlikely(cxgb4_map_skb(adapter->pdev_dev, skb,
				   sgl_sdesc->addr) < 0)) {
		/* We need to map the skb into PCI DMA space (because it can't
		 * be in-lined directly into the Work Request) and the mapping
		 * operation failed.  Record the error and drop the packet.
		 */
		memset(sgl_sdesc->addr, 0, sizeof(sgl_sdesc->addr));
		txq->mapping_err++;
		goto out_free;
	}

	chip_ver = CHELSIO_CHIP_VERSION(adapter->params.chip);
	wr_mid = FW_WR_LEN16_V(DIV_ROUND_UP(flits, 2));
	if (unlikely(credits < ETHTXQ_STOP_THRES)) {
		/* After we're done injecting the Work Request for this
		 * packet, we'll be below our "stop threshold" so stop the TX
		 * Queue now and schedule a request for an SGE Egress Queue
		 * Update message.  The queue will get started later on when
		 * the firmware processes this Work Request and sends us an
		 * Egress Queue Status Update message indicating that space
		 * has opened up.
		 */
		eth_txq_stop(txq);
		if (chip_ver > CHELSIO_T5)
			wr_mid |= FW_WR_EQUEQ_F | FW_WR_EQUIQ_F;
	}

	/* Start filling in our Work Request.  Note that we do _not_ handle
	 * the WR Header wrapping around the TX Descriptor Ring.  If our
	 * maximum header size ever exceeds one TX Descriptor, we'll need to
	 * do something else here.
	 */
	WARN_ON(DIV_ROUND_UP(T4VF_ETHTXQ_MAX_HDR, TXD_PER_EQ_UNIT) > 1);
	wr = (void *)&txq->q.desc[txq->q.pidx];
	wr->equiq_to_len16 = cpu_to_be32(wr_mid);
	wr->r3[0] = cpu_to_be32(0);
	wr->r3[1] = cpu_to_be32(0);
	skb_copy_from_linear_data(skb, (void *)wr->ethmacdst, fw_hdr_copy_len);
	end = (u64 *)wr + flits;

	/* If this is a Large Send Offload packet we'll put in an LSO CPL
	 * message with an encapsulated TX Packet CPL message.  Otherwise we
	 * just use a TX Packet CPL message.
	 */
	ssi = skb_shinfo(skb);
	if (ssi->gso_size) {
		struct cpl_tx_pkt_lso_core *lso = (void *)(wr + 1);
		bool v6 = (ssi->gso_type & SKB_GSO_TCPV6) != 0;
		int l3hdr_len = skb_network_header_len(skb);
		int eth_xtra_len = skb_network_offset(skb) - ETH_HLEN;

		wr->op_immdlen =
			cpu_to_be32(FW_WR_OP_V(FW_ETH_TX_PKT_VM_WR) |
				    FW_WR_IMMDLEN_V(sizeof(*lso) +
						    sizeof(*cpl)));
		 /* Fill in the LSO CPL message. */
		lso->lso_ctrl =
			cpu_to_be32(LSO_OPCODE_V(CPL_TX_PKT_LSO) |
				    LSO_FIRST_SLICE_F |
				    LSO_LAST_SLICE_F |
				    LSO_IPV6_V(v6) |
				    LSO_ETHHDR_LEN_V(eth_xtra_len / 4) |
				    LSO_IPHDR_LEN_V(l3hdr_len / 4) |
				    LSO_TCPHDR_LEN_V(tcp_hdr(skb)->doff));
		lso->ipid_ofst = cpu_to_be16(0);
		lso->mss = cpu_to_be16(ssi->gso_size);
		lso->seqno_offset = cpu_to_be32(0);
		if (is_t4(adapter->params.chip))
			lso->len = cpu_to_be32(skb->len);
		else
			lso->len = cpu_to_be32(LSO_T5_XFER_SIZE_V(skb->len));

		/* Set up TX Packet CPL pointer, control word and perform
		 * accounting.
		 */
		cpl = (void *)(lso + 1);

		if (chip_ver <= CHELSIO_T5)
			cntrl = TXPKT_ETHHDR_LEN_V(eth_xtra_len);
		else
			cntrl = T6_TXPKT_ETHHDR_LEN_V(eth_xtra_len);

		cntrl |= TXPKT_CSUM_TYPE_V(v6 ?
					   TX_CSUM_TCPIP6 : TX_CSUM_TCPIP) |
			 TXPKT_IPHDR_LEN_V(l3hdr_len);
		txq->tso++;
		txq->tx_cso += ssi->gso_segs;
	} else {
		int len;

		len = (t4vf_is_eth_imm(skb)
		       ? skb->len + sizeof(*cpl)
		       : sizeof(*cpl));
		wr->op_immdlen =
			cpu_to_be32(FW_WR_OP_V(FW_ETH_TX_PKT_VM_WR) |
				    FW_WR_IMMDLEN_V(len));

		/* Set up TX Packet CPL pointer, control word and perform
		 * accounting.
		 */
		cpl = (void *)(wr + 1);
		if (skb->ip_summed == CHECKSUM_PARTIAL) {
			cntrl = hwcsum(adapter->params.chip, skb) |
				TXPKT_IPCSUM_DIS_F;
			txq->tx_cso++;
		} else {
			cntrl = TXPKT_L4CSUM_DIS_F | TXPKT_IPCSUM_DIS_F;
		}
	}

	/* If there's a VLAN tag present, add that to the list of things to
	 * do in this Work Request.
	 */
	if (skb_vlan_tag_present(skb)) {
		txq->vlan_ins++;
		cntrl |= TXPKT_VLAN_VLD_F | TXPKT_VLAN_V(skb_vlan_tag_get(skb));
	}

	 /* Fill in the TX Packet CPL message header. */
	cpl->ctrl0 = cpu_to_be32(TXPKT_OPCODE_V(CPL_TX_PKT_XT) |
				 TXPKT_INTF_V(pi->port_id) |
				 TXPKT_PF_V(0));
	cpl->pack = cpu_to_be16(0);
	cpl->len = cpu_to_be16(skb->len);
	cpl->ctrl1 = cpu_to_be64(cntrl);

	/* Fill in the body of the TX Packet CPL message with either in-lined
	 * data or a Scatter/Gather List.
	 */
	if (t4vf_is_eth_imm(skb)) {
		/* In-line the packet's data and free the skb since we don't
		 * need it any longer.
		 */
		cxgb4_inline_tx_skb(skb, &txq->q, cpl + 1);
		dev_consume_skb_any(skb);
	} else {
		/* Write the skb's Scatter/Gather list into the TX Packet CPL
		 * message and retain a pointer to the skb so we can free it
		 * later when its DMA completes.  (We store the skb pointer
		 * in the Software Descriptor corresponding to the last TX
		 * Descriptor used by the Work Request.)
		 *
		 * The retained skb will be freed when the corresponding TX
		 * Descriptors are reclaimed after their DMAs complete.
		 * However, this could take quite a while since, in general,
		 * the hardware is set up to be lazy about sending DMA
		 * completion notifications to us and we mostly perform TX
		 * reclaims in the transmit routine.
		 *
		 * This is good for performamce but means that we rely on new
		 * TX packets arriving to run the destructors of completed
		 * packets, which open up space in their sockets' send queues.
		 * Sometimes we do not get such new packets causing TX to
		 * stall.  A single UDP transmitter is a good example of this
		 * situation.  We have a clean up timer that periodically
		 * reclaims completed packets but it doesn't run often enough
		 * (nor do we want it to) to prevent lengthy stalls.  A
		 * solution to this problem is to run the destructor early,
		 * after the packet is queued but before it's DMAd.  A con is
		 * that we lie to socket memory accounting, but the amount of
		 * extra memory is reasonable (limited by the number of TX
		 * descriptors), the packets do actually get freed quickly by
		 * new packets almost always, and for protocols like TCP that
		 * wait for acks to really free up the data the extra memory
		 * is even less.  On the positive side we run the destructors
		 * on the sending CPU rather than on a potentially different
		 * completing CPU, usually a good thing.
		 *
		 * Run the destructor before telling the DMA engine about the
		 * packet to make sure it doesn't complete and get freed
		 * prematurely.
		 */
		struct ulptx_sgl *sgl = (struct ulptx_sgl *)(cpl + 1);
		struct sge_txq *tq = &txq->q;

		/* If the Work Request header was an exact multiple of our TX
		 * Descriptor length, then it's possible that the starting SGL
		 * pointer lines up exactly with the end of our TX Descriptor
		 * ring.  If that's the case, wrap around to the beginning
		 * here ...
		 */
		if (unlikely((void *)sgl == (void *)tq->stat)) {
			sgl = (void *)tq->desc;
			end = (void *)((void *)tq->desc +
				       ((void *)end - (void *)tq->stat));
		}

		cxgb4_write_sgl(skb, tq, sgl, end, 0, sgl_sdesc->addr);
		skb_orphan(skb);
		sgl_sdesc->skb = skb;
	}

	/* Advance our internal TX Queue state, tell the hardware about
	 * the new TX descriptors and return success.
	 */
	txq_advance(&txq->q, ndesc);

	cxgb4_ring_tx_db(adapter, &txq->q, ndesc);
	return NETDEV_TX_OK;

out_free:
	/* An error of some sort happened.  Free the TX skb and tell the
	 * OS that we've "dealt" with the packet ...
	 */
	dev_kfree_skb_any(skb);
	return NETDEV_TX_OK;
}

/**
 * reclaim_completed_tx_imm - reclaim completed control-queue Tx descs
 * @q: the SGE control Tx queue
 *
 * This is a variant of cxgb4_reclaim_completed_tx() that is used
 * for Tx queues that send only immediate data (presently just
 * the control queues) and	thus do not have any sk_buffs to release.
 */
static inline void reclaim_completed_tx_imm(struct sge_txq *q)
{
	int hw_cidx = ntohs(READ_ONCE(q->stat->cidx));
	int reclaim = hw_cidx - q->cidx;

	if (reclaim < 0)
		reclaim += q->size;

	q->in_use -= reclaim;
	q->cidx = hw_cidx;
}

static inline void eosw_txq_advance_index(u32 *idx, u32 n, u32 max)
{
	u32 val = *idx + n;

	if (val >= max)
		val -= max;

	*idx = val;
}

void cxgb4_eosw_txq_free_desc(struct adapter *adap,
			      struct sge_eosw_txq *eosw_txq, u32 ndesc)
{
	struct tx_sw_desc *d;

	d = &eosw_txq->desc[eosw_txq->last_cidx];
	while (ndesc--) {
		if (d->skb) {
			if (d->addr[0]) {
				unmap_skb(adap->pdev_dev, d->skb, d->addr);
				memset(d->addr, 0, sizeof(d->addr));
			}
			dev_consume_skb_any(d->skb);
			d->skb = NULL;
		}
		eosw_txq_advance_index(&eosw_txq->last_cidx, 1,
				       eosw_txq->ndesc);
		d = &eosw_txq->desc[eosw_txq->last_cidx];
	}
}

static inline void eosw_txq_advance(struct sge_eosw_txq *eosw_txq, u32 n)
{
	eosw_txq_advance_index(&eosw_txq->pidx, n, eosw_txq->ndesc);
	eosw_txq->inuse += n;
}

static inline int eosw_txq_enqueue(struct sge_eosw_txq *eosw_txq,
				   struct sk_buff *skb)
{
	if (eosw_txq->inuse == eosw_txq->ndesc)
		return -ENOMEM;

	eosw_txq->desc[eosw_txq->pidx].skb = skb;
	return 0;
}

static inline struct sk_buff *eosw_txq_peek(struct sge_eosw_txq *eosw_txq)
{
	return eosw_txq->desc[eosw_txq->last_pidx].skb;
}

static inline u8 ethofld_calc_tx_flits(struct adapter *adap,
				       struct sk_buff *skb, u32 hdr_len)
{
	u8 flits, nsgl = 0;
	u32 wrlen;

	wrlen = sizeof(struct fw_eth_tx_eo_wr) + sizeof(struct cpl_tx_pkt_core);
	if (skb_shinfo(skb)->gso_size &&
	    !(skb_shinfo(skb)->gso_type & SKB_GSO_UDP_L4))
		wrlen += sizeof(struct cpl_tx_pkt_lso_core);

	wrlen += roundup(hdr_len, 16);

	/* Packet headers + WR + CPLs */
	flits = DIV_ROUND_UP(wrlen, 8);

	if (skb_shinfo(skb)->nr_frags > 0) {
		if (skb_headlen(skb) - hdr_len)
			nsgl = sgl_len(skb_shinfo(skb)->nr_frags + 1);
		else
			nsgl = sgl_len(skb_shinfo(skb)->nr_frags);
	} else if (skb->len - hdr_len) {
		nsgl = sgl_len(1);
	}

	return flits + nsgl;
}

static void *write_eo_wr(struct adapter *adap, struct sge_eosw_txq *eosw_txq,
			 struct sk_buff *skb, struct fw_eth_tx_eo_wr *wr,
			 u32 hdr_len, u32 wrlen)
{
	const struct skb_shared_info *ssi = skb_shinfo(skb);
	struct cpl_tx_pkt_core *cpl;
	u32 immd_len, wrlen16;
	bool compl = false;
	u8 ver, proto;

	ver = ip_hdr(skb)->version;
	proto = (ver == 6) ? ipv6_hdr(skb)->nexthdr : ip_hdr(skb)->protocol;

	wrlen16 = DIV_ROUND_UP(wrlen, 16);
	immd_len = sizeof(struct cpl_tx_pkt_core);
	if (skb_shinfo(skb)->gso_size &&
	    !(skb_shinfo(skb)->gso_type & SKB_GSO_UDP_L4))
		immd_len += sizeof(struct cpl_tx_pkt_lso_core);
	immd_len += hdr_len;

	if (!eosw_txq->ncompl ||
	    (eosw_txq->last_compl + wrlen16) >=
	    (adap->params.ofldq_wr_cred / 2)) {
		compl = true;
		eosw_txq->ncompl++;
		eosw_txq->last_compl = 0;
	}

	wr->op_immdlen = cpu_to_be32(FW_WR_OP_V(FW_ETH_TX_EO_WR) |
				     FW_ETH_TX_EO_WR_IMMDLEN_V(immd_len) |
				     FW_WR_COMPL_V(compl));
	wr->equiq_to_len16 = cpu_to_be32(FW_WR_LEN16_V(wrlen16) |
					 FW_WR_FLOWID_V(eosw_txq->hwtid));
	wr->r3 = 0;
	if (proto == IPPROTO_UDP) {
		cpl = write_eo_udp_wr(skb, wr, hdr_len);
	} else {
		wr->u.tcpseg.type = FW_ETH_TX_EO_TYPE_TCPSEG;
		wr->u.tcpseg.ethlen = skb_network_offset(skb);
		wr->u.tcpseg.iplen = cpu_to_be16(skb_network_header_len(skb));
		wr->u.tcpseg.tcplen = tcp_hdrlen(skb);
		wr->u.tcpseg.tsclk_tsoff = 0;
		wr->u.tcpseg.r4 = 0;
		wr->u.tcpseg.r5 = 0;
		wr->u.tcpseg.plen = cpu_to_be32(skb->len - hdr_len);

		if (ssi->gso_size) {
			struct cpl_tx_pkt_lso_core *lso = (void *)(wr + 1);

			wr->u.tcpseg.mss = cpu_to_be16(ssi->gso_size);
			cpl = write_tso_wr(adap, skb, lso);
		} else {
			wr->u.tcpseg.mss = cpu_to_be16(0xffff);
			cpl = (void *)(wr + 1);
		}
	}

	eosw_txq->cred -= wrlen16;
	eosw_txq->last_compl += wrlen16;
	return cpl;
}

static int ethofld_hard_xmit(struct net_device *dev,
			     struct sge_eosw_txq *eosw_txq)
{
	struct port_info *pi = netdev2pinfo(dev);
	struct adapter *adap = netdev2adap(dev);
	u32 wrlen, wrlen16, hdr_len, data_len;
	enum sge_eosw_state next_state;
	u64 cntrl, *start, *end, *sgl;
	struct sge_eohw_txq *eohw_txq;
	struct cpl_tx_pkt_core *cpl;
	struct fw_eth_tx_eo_wr *wr;
	bool skip_eotx_wr = false;
	struct tx_sw_desc *d;
	struct sk_buff *skb;
	int left, ret = 0;
	u8 flits, ndesc;

	eohw_txq = &adap->sge.eohw_txq[eosw_txq->hwqid];
	spin_lock(&eohw_txq->lock);
	reclaim_completed_tx_imm(&eohw_txq->q);

	d = &eosw_txq->desc[eosw_txq->last_pidx];
	skb = d->skb;
	skb_tx_timestamp(skb);

	wr = (struct fw_eth_tx_eo_wr *)&eohw_txq->q.desc[eohw_txq->q.pidx];
	if (unlikely(eosw_txq->state != CXGB4_EO_STATE_ACTIVE &&
		     eosw_txq->last_pidx == eosw_txq->flowc_idx)) {
		hdr_len = skb->len;
		data_len = 0;
		flits = DIV_ROUND_UP(hdr_len, 8);
		if (eosw_txq->state == CXGB4_EO_STATE_FLOWC_OPEN_SEND)
			next_state = CXGB4_EO_STATE_FLOWC_OPEN_REPLY;
		else
			next_state = CXGB4_EO_STATE_FLOWC_CLOSE_REPLY;
		skip_eotx_wr = true;
	} else {
		hdr_len = eth_get_headlen(dev, skb->data, skb_headlen(skb));
		data_len = skb->len - hdr_len;
		flits = ethofld_calc_tx_flits(adap, skb, hdr_len);
	}
	ndesc = flits_to_desc(flits);
	wrlen = flits * 8;
	wrlen16 = DIV_ROUND_UP(wrlen, 16);

	left = txq_avail(&eohw_txq->q) - ndesc;

	/* If there are no descriptors left in hardware queues or no
	 * CPL credits left in software queues, then wait for them
	 * to come back and retry again. Note that we always request
	 * for credits update via interrupt for every half credits
	 * consumed. So, the interrupt will eventually restore the
	 * credits and invoke the Tx path again.
	 */
	if (unlikely(left < 0 || wrlen16 > eosw_txq->cred)) {
		ret = -ENOMEM;
		goto out_unlock;
	}

	if (unlikely(skip_eotx_wr)) {
		start = (u64 *)wr;
		eosw_txq->state = next_state;
		eosw_txq->cred -= wrlen16;
		eosw_txq->ncompl++;
		eosw_txq->last_compl = 0;
		goto write_wr_headers;
	}

	cpl = write_eo_wr(adap, eosw_txq, skb, wr, hdr_len, wrlen);
	cntrl = hwcsum(adap->params.chip, skb);
	if (skb_vlan_tag_present(skb))
		cntrl |= TXPKT_VLAN_VLD_F | TXPKT_VLAN_V(skb_vlan_tag_get(skb));

	cpl->ctrl0 = cpu_to_be32(TXPKT_OPCODE_V(CPL_TX_PKT_XT) |
				 TXPKT_INTF_V(pi->tx_chan) |
				 TXPKT_PF_V(adap->pf));
	cpl->pack = 0;
	cpl->len = cpu_to_be16(skb->len);
	cpl->ctrl1 = cpu_to_be64(cntrl);

	start = (u64 *)(cpl + 1);

write_wr_headers:
	sgl = (u64 *)inline_tx_skb_header(skb, &eohw_txq->q, (void *)start,
					  hdr_len);
	if (data_len) {
		ret = cxgb4_map_skb(adap->pdev_dev, skb, d->addr);
		if (unlikely(ret)) {
			memset(d->addr, 0, sizeof(d->addr));
			eohw_txq->mapping_err++;
			goto out_unlock;
		}

		end = (u64 *)wr + flits;
		if (unlikely(start > sgl)) {
			left = (u8 *)end - (u8 *)eohw_txq->q.stat;
			end = (void *)eohw_txq->q.desc + left;
		}

		if (unlikely((u8 *)sgl >= (u8 *)eohw_txq->q.stat)) {
			/* If current position is already at the end of the
			 * txq, reset the current to point to start of the queue
			 * and update the end ptr as well.
			 */
			left = (u8 *)end - (u8 *)eohw_txq->q.stat;

			end = (void *)eohw_txq->q.desc + left;
			sgl = (void *)eohw_txq->q.desc;
		}

		cxgb4_write_sgl(skb, &eohw_txq->q, (void *)sgl, end, hdr_len,
				d->addr);
	}

	if (skb_shinfo(skb)->gso_size) {
		if (skb_shinfo(skb)->gso_type & SKB_GSO_UDP_L4)
			eohw_txq->uso++;
		else
			eohw_txq->tso++;
		eohw_txq->tx_cso += skb_shinfo(skb)->gso_segs;
	} else if (skb->ip_summed == CHECKSUM_PARTIAL) {
		eohw_txq->tx_cso++;
	}

	if (skb_vlan_tag_present(skb))
		eohw_txq->vlan_ins++;

	txq_advance(&eohw_txq->q, ndesc);
	cxgb4_ring_tx_db(adap, &eohw_txq->q, ndesc);
	eosw_txq_advance_index(&eosw_txq->last_pidx, 1, eosw_txq->ndesc);

out_unlock:
	spin_unlock(&eohw_txq->lock);
	return ret;
}

static void ethofld_xmit(struct net_device *dev, struct sge_eosw_txq *eosw_txq)
{
	struct sk_buff *skb;
	int pktcount, ret;

	switch (eosw_txq->state) {
	case CXGB4_EO_STATE_ACTIVE:
	case CXGB4_EO_STATE_FLOWC_OPEN_SEND:
	case CXGB4_EO_STATE_FLOWC_CLOSE_SEND:
		pktcount = eosw_txq->pidx - eosw_txq->last_pidx;
		if (pktcount < 0)
			pktcount += eosw_txq->ndesc;
		break;
	case CXGB4_EO_STATE_FLOWC_OPEN_REPLY:
	case CXGB4_EO_STATE_FLOWC_CLOSE_REPLY:
	case CXGB4_EO_STATE_CLOSED:
	default:
		return;
	}

	while (pktcount--) {
		skb = eosw_txq_peek(eosw_txq);
		if (!skb) {
			eosw_txq_advance_index(&eosw_txq->last_pidx, 1,
					       eosw_txq->ndesc);
			continue;
		}

		ret = ethofld_hard_xmit(dev, eosw_txq);
		if (ret)
			break;
	}
}

static netdev_tx_t cxgb4_ethofld_xmit(struct sk_buff *skb,
				      struct net_device *dev)
{
	struct cxgb4_tc_port_mqprio *tc_port_mqprio;
	struct port_info *pi = netdev2pinfo(dev);
	struct adapter *adap = netdev2adap(dev);
	struct sge_eosw_txq *eosw_txq;
	u32 qid;
	int ret;

	ret = cxgb4_validate_skb(skb, dev, ETH_HLEN);
	if (ret)
		goto out_free;

	tc_port_mqprio = &adap->tc_mqprio->port_mqprio[pi->port_id];
	qid = skb_get_queue_mapping(skb) - pi->nqsets;
	eosw_txq = &tc_port_mqprio->eosw_txq[qid];
	spin_lock_bh(&eosw_txq->lock);
	if (eosw_txq->state != CXGB4_EO_STATE_ACTIVE)
		goto out_unlock;

	ret = eosw_txq_enqueue(eosw_txq, skb);
	if (ret)
		goto out_unlock;

	/* SKB is queued for processing until credits are available.
	 * So, call the destructor now and we'll free the skb later
	 * after it has been successfully transmitted.
	 */
	skb_orphan(skb);

	eosw_txq_advance(eosw_txq, 1);
	ethofld_xmit(dev, eosw_txq);
	spin_unlock_bh(&eosw_txq->lock);
	return NETDEV_TX_OK;

out_unlock:
	spin_unlock_bh(&eosw_txq->lock);
out_free:
	dev_kfree_skb_any(skb);
	return NETDEV_TX_OK;
}

netdev_tx_t t4_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct port_info *pi = netdev_priv(dev);
	u16 qid = skb_get_queue_mapping(skb);

	if (unlikely(pi->eth_flags & PRIV_FLAG_PORT_TX_VM))
		return cxgb4_vf_eth_xmit(skb, dev);

	if (unlikely(qid >= pi->nqsets))
		return cxgb4_ethofld_xmit(skb, dev);

	if (is_ptp_enabled(skb, dev)) {
		struct adapter *adap = netdev2adap(dev);
		netdev_tx_t ret;

		spin_lock(&adap->ptp_lock);
		ret = cxgb4_eth_xmit(skb, dev);
		spin_unlock(&adap->ptp_lock);
		return ret;
	}

	return cxgb4_eth_xmit(skb, dev);
}

static void eosw_txq_flush_pending_skbs(struct sge_eosw_txq *eosw_txq)
{
	int pktcount = eosw_txq->pidx - eosw_txq->last_pidx;
	int pidx = eosw_txq->pidx;
	struct sk_buff *skb;

	if (!pktcount)
		return;

	if (pktcount < 0)
		pktcount += eosw_txq->ndesc;

	while (pktcount--) {
		pidx--;
		if (pidx < 0)
			pidx += eosw_txq->ndesc;

		skb = eosw_txq->desc[pidx].skb;
		if (skb) {
			dev_consume_skb_any(skb);
			eosw_txq->desc[pidx].skb = NULL;
			eosw_txq->inuse--;
		}
	}

	eosw_txq->pidx = eosw_txq->last_pidx + 1;
}

/**
 * cxgb4_ethofld_send_flowc - Send ETHOFLD flowc request to bind eotid to tc.
 * @dev: netdevice
 * @eotid: ETHOFLD tid to bind/unbind
 * @tc: traffic class. If set to FW_SCHED_CLS_NONE, then unbinds the @eotid
 *
 * Send a FLOWC work request to bind an ETHOFLD TID to a traffic class.
 * If @tc is set to FW_SCHED_CLS_NONE, then the @eotid is unbound from
 * a traffic class.
 */
int cxgb4_ethofld_send_flowc(struct net_device *dev, u32 eotid, u32 tc)
{
	struct port_info *pi = netdev2pinfo(dev);
	struct adapter *adap = netdev2adap(dev);
	enum sge_eosw_state next_state;
	struct sge_eosw_txq *eosw_txq;
	u32 len, len16, nparams = 6;
	struct fw_flowc_wr *flowc;
	struct eotid_entry *entry;
	struct sge_ofld_rxq *rxq;
	struct sk_buff *skb;
	int ret = 0;

	len = struct_size(flowc, mnemval, nparams);
	len16 = DIV_ROUND_UP(len, 16);

	entry = cxgb4_lookup_eotid(&adap->tids, eotid);
	if (!entry)
		return -ENOMEM;

	eosw_txq = (struct sge_eosw_txq *)entry->data;
	if (!eosw_txq)
		return -ENOMEM;

	if (!(adap->flags & CXGB4_FW_OK)) {
		/* Don't stall caller when access to FW is lost */
		complete(&eosw_txq->completion);
		return -EIO;
	}

	skb = alloc_skb(len, GFP_KERNEL);
	if (!skb)
		return -ENOMEM;

	spin_lock_bh(&eosw_txq->lock);
	if (tc != FW_SCHED_CLS_NONE) {
		if (eosw_txq->state != CXGB4_EO_STATE_CLOSED)
			goto out_free_skb;

		next_state = CXGB4_EO_STATE_FLOWC_OPEN_SEND;
	} else {
		if (eosw_txq->state != CXGB4_EO_STATE_ACTIVE)
			goto out_free_skb;

		next_state = CXGB4_EO_STATE_FLOWC_CLOSE_SEND;
	}

	flowc = __skb_put(skb, len);
	memset(flowc, 0, len);

	rxq = &adap->sge.eohw_rxq[eosw_txq->hwqid];
	flowc->flowid_len16 = cpu_to_be32(FW_WR_LEN16_V(len16) |
					  FW_WR_FLOWID_V(eosw_txq->hwtid));
	flowc->op_to_nparams = cpu_to_be32(FW_WR_OP_V(FW_FLOWC_WR) |
					   FW_FLOWC_WR_NPARAMS_V(nparams) |
					   FW_WR_COMPL_V(1));
	flowc->mnemval[0].mnemonic = FW_FLOWC_MNEM_PFNVFN;
	flowc->mnemval[0].val = cpu_to_be32(FW_PFVF_CMD_PFN_V(adap->pf));
	flowc->mnemval[1].mnemonic = FW_FLOWC_MNEM_CH;
	flowc->mnemval[1].val = cpu_to_be32(pi->tx_chan);
	flowc->mnemval[2].mnemonic = FW_FLOWC_MNEM_PORT;
	flowc->mnemval[2].val = cpu_to_be32(pi->tx_chan);
	flowc->mnemval[3].mnemonic = FW_FLOWC_MNEM_IQID;
	flowc->mnemval[3].val = cpu_to_be32(rxq->rspq.abs_id);
	flowc->mnemval[4].mnemonic = FW_FLOWC_MNEM_SCHEDCLASS;
	flowc->mnemval[4].val = cpu_to_be32(tc);
	flowc->mnemval[5].mnemonic = FW_FLOWC_MNEM_EOSTATE;
	flowc->mnemval[5].val = cpu_to_be32(tc == FW_SCHED_CLS_NONE ?
					    FW_FLOWC_MNEM_EOSTATE_CLOSING :
					    FW_FLOWC_MNEM_EOSTATE_ESTABLISHED);

	/* Free up any pending skbs to ensure there's room for
	 * termination FLOWC.
	 */
	if (tc == FW_SCHED_CLS_NONE)
		eosw_txq_flush_pending_skbs(eosw_txq);

	ret = eosw_txq_enqueue(eosw_txq, skb);
	if (ret)
		goto out_free_skb;

	eosw_txq->state = next_state;
	eosw_txq->flowc_idx = eosw_txq->pidx;
	eosw_txq_advance(eosw_txq, 1);
	ethofld_xmit(dev, eosw_txq);

	spin_unlock_bh(&eosw_txq->lock);
	return 0;

out_free_skb:
	dev_consume_skb_any(skb);
	spin_unlock_bh(&eosw_txq->lock);
	return ret;
}

/**
 *	is_imm - check whether a packet can be sent as immediate data
 *	@skb: the packet
 *
 *	Returns true if a packet can be sent as a WR with immediate data.
 */
static inline int is_imm(const struct sk_buff *skb)
{
	return skb->len <= MAX_CTRL_WR_LEN;
}

/**
 *	ctrlq_check_stop - check if a control queue is full and should stop
 *	@q: the queue
 *	@wr: most recent WR written to the queue
 *
 *	Check if a control queue has become full and should be stopped.
 *	We clean up control queue descriptors very lazily, only when we are out.
 *	If the queue is still full after reclaiming any completed descriptors
 *	we suspend it and have the last WR wake it up.
 */
static void ctrlq_check_stop(struct sge_ctrl_txq *q, struct fw_wr_hdr *wr)
{
	reclaim_completed_tx_imm(&q->q);
	if (unlikely(txq_avail(&q->q) < TXQ_STOP_THRES)) {
		wr->lo |= htonl(FW_WR_EQUEQ_F | FW_WR_EQUIQ_F);
		q->q.stops++;
		q->full = 1;
	}
}

#define CXGB4_SELFTEST_LB_STR "CHELSIO_SELFTEST"

int cxgb4_selftest_lb_pkt(struct net_device *netdev)
{
	struct port_info *pi = netdev_priv(netdev);
	struct adapter *adap = pi->adapter;
	struct cxgb4_ethtool_lb_test *lb;
	int ret, i = 0, pkt_len, credits;
	struct fw_eth_tx_pkt_wr *wr;
	struct cpl_tx_pkt_core *cpl;
	u32 ctrl0, ndesc, flits;
	struct sge_eth_txq *q;
	u8 *sgl;

	pkt_len = ETH_HLEN + sizeof(CXGB4_SELFTEST_LB_STR);

	flits = DIV_ROUND_UP(pkt_len + sizeof(*cpl) + sizeof(*wr),
			     sizeof(__be64));
	ndesc = flits_to_desc(flits);

	lb = &pi->ethtool_lb;
	lb->loopback = 1;

	q = &adap->sge.ethtxq[pi->first_qset];
	__netif_tx_lock(q->txq, smp_processor_id());

	reclaim_completed_tx(adap, &q->q, -1, true);
	credits = txq_avail(&q->q) - ndesc;
	if (unlikely(credits < 0)) {
		__netif_tx_unlock(q->txq);
		return -ENOMEM;
	}

	wr = (void *)&q->q.desc[q->q.pidx];
	memset(wr, 0, sizeof(struct tx_desc));

	wr->op_immdlen = htonl(FW_WR_OP_V(FW_ETH_TX_PKT_WR) |
			       FW_WR_IMMDLEN_V(pkt_len +
			       sizeof(*cpl)));
	wr->equiq_to_len16 = htonl(FW_WR_LEN16_V(DIV_ROUND_UP(flits, 2)));
	wr->r3 = cpu_to_be64(0);

	cpl = (void *)(wr + 1);
	sgl = (u8 *)(cpl + 1);

	ctrl0 = TXPKT_OPCODE_V(CPL_TX_PKT_XT) | TXPKT_PF_V(adap->pf) |
		TXPKT_INTF_V(pi->tx_chan + 4);

	cpl->ctrl0 = htonl(ctrl0);
	cpl->pack = htons(0);
	cpl->len = htons(pkt_len);
	cpl->ctrl1 = cpu_to_be64(TXPKT_L4CSUM_DIS_F | TXPKT_IPCSUM_DIS_F);

	eth_broadcast_addr(sgl);
	i += ETH_ALEN;
	ether_addr_copy(&sgl[i], netdev->dev_addr);
	i += ETH_ALEN;

	snprintf(&sgl[i], sizeof(CXGB4_SELFTEST_LB_STR), "%s",
		 CXGB4_SELFTEST_LB_STR);

	init_completion(&lb->completion);
	txq_advance(&q->q, ndesc);
	cxgb4_ring_tx_db(adap, &q->q, ndesc);
	__netif_tx_unlock(q->txq);

	/* wait for the pkt to return */
	ret = wait_for_completion_timeout(&lb->completion, 10 * HZ);
	if (!ret)
		ret = -ETIMEDOUT;
	else
		ret = lb->result;

	lb->loopback = 0;

	return ret;
}

/**
 *	ctrl_xmit - send a packet through an SGE control Tx queue
 *	@q: the control queue
 *	@skb: the packet
 *
 *	Send a packet through an SGE control Tx queue.  Packets sent through
 *	a control queue must fit entirely as immediate data.
 */
static int ctrl_xmit(struct sge_ctrl_txq *q, struct sk_buff *skb)
{
	unsigned int ndesc;
	struct fw_wr_hdr *wr;

	if (unlikely(!is_imm(skb))) {
		WARN_ON(1);
		dev_kfree_skb(skb);
		return NET_XMIT_DROP;
	}

	ndesc = DIV_ROUND_UP(skb->len, sizeof(struct tx_desc));
	spin_lock(&q->sendq.lock);

	if (unlikely(q->full)) {
		skb->priority = ndesc;                  /* save for restart */
		__skb_queue_tail(&q->sendq, skb);
		spin_unlock(&q->sendq.lock);
		return NET_XMIT_CN;
	}

	wr = (struct fw_wr_hdr *)&q->q.desc[q->q.pidx];
	cxgb4_inline_tx_skb(skb, &q->q, wr);

	txq_advance(&q->q, ndesc);
	if (unlikely(txq_avail(&q->q) < TXQ_STOP_THRES))
		ctrlq_check_stop(q, wr);

	cxgb4_ring_tx_db(q->adap, &q->q, ndesc);
	spin_unlock(&q->sendq.lock);

	kfree_skb(skb);
	return NET_XMIT_SUCCESS;
}

/**
 *	restart_ctrlq - restart a suspended control queue
 *	@t: pointer to the tasklet associated with this handler
 *
 *	Resumes transmission on a suspended Tx control queue.
 */
static void restart_ctrlq(struct tasklet_struct *t)
{
	struct sk_buff *skb;
	unsigned int written = 0;
	struct sge_ctrl_txq *q = from_tasklet(q, t, qresume_tsk);

	spin_lock(&q->sendq.lock);
	reclaim_completed_tx_imm(&q->q);
	BUG_ON(txq_avail(&q->q) < TXQ_STOP_THRES);  /* q should be empty */

	while ((skb = __skb_dequeue(&q->sendq)) != NULL) {
		struct fw_wr_hdr *wr;
		unsigned int ndesc = skb->priority;     /* previously saved */

		written += ndesc;
		/* Write descriptors and free skbs outside the lock to limit
		 * wait times.  q->full is still set so new skbs will be queued.
		 */
		wr = (struct fw_wr_hdr *)&q->q.desc[q->q.pidx];
		txq_advance(&q->q, ndesc);
		spin_unlock(&q->sendq.lock);

		cxgb4_inline_tx_skb(skb, &q->q, wr);
		kfree_skb(skb);

		if (unlikely(txq_avail(&q->q) < TXQ_STOP_THRES)) {
			unsigned long old = q->q.stops;

			ctrlq_check_stop(q, wr);
			if (q->q.stops != old) {          /* suspended anew */
				spin_lock(&q->sendq.lock);
				goto ringdb;
			}
		}
		if (written > 16) {
			cxgb4_ring_tx_db(q->adap, &q->q, written);
			written = 0;
		}
		spin_lock(&q->sendq.lock);
	}
	q->full = 0;
ringdb:
	if (written)
		cxgb4_ring_tx_db(q->adap, &q->q, written);
	spin_unlock(&q->sendq.lock);
}

/**
 *	t4_mgmt_tx - send a management message
 *	@adap: the adapter
 *	@skb: the packet containing the management message
 *
 *	Send a management message through control queue 0.
 */
int t4_mgmt_tx(struct adapter *adap, struct sk_buff *skb)
{
	int ret;

	local_bh_disable();
	ret = ctrl_xmit(&adap->sge.ctrlq[0], skb);
	local_bh_enable();
	return ret;
}

/**
 *	is_ofld_imm - check whether a packet can be sent as immediate data
 *	@skb: the packet
 *
 *	Returns true if a packet can be sent as an offload WR with immediate
 *	data.
 *	FW_OFLD_TX_DATA_WR limits the payload to 255 bytes due to 8-bit field.
 *      However, FW_ULPTX_WR commands have a 256 byte immediate only
 *      payload limit.
 */
static inline int is_ofld_imm(const struct sk_buff *skb)
{
	struct work_request_hdr *req = (struct work_request_hdr *)skb->data;
	unsigned long opcode = FW_WR_OP_G(ntohl(req->wr_hi));

	if (unlikely(opcode == FW_ULPTX_WR))
		return skb->len <= MAX_IMM_ULPTX_WR_LEN;
	else if (opcode == FW_CRYPTO_LOOKASIDE_WR)
		return skb->len <= SGE_MAX_WR_LEN;
	else
		return skb->len <= MAX_IMM_OFLD_TX_DATA_WR_LEN;
}

/**
 *	calc_tx_flits_ofld - calculate # of flits for an offload packet
 *	@skb: the packet
 *
 *	Returns the number of flits needed for the given offload packet.
 *	These packets are already fully constructed and no additional headers
 *	will be added.
 */
static inline unsigned int calc_tx_flits_ofld(const struct sk_buff *skb)
{
	unsigned int flits, cnt;

	if (is_ofld_imm(skb))
		return DIV_ROUND_UP(skb->len, 8);

	flits = skb_transport_offset(skb) / 8U;   /* headers */
	cnt = skb_shinfo(skb)->nr_frags;
	if (skb_tail_pointer(skb) != skb_transport_header(skb))
		cnt++;
	return flits + sgl_len(cnt);
}

/**
 *	txq_stop_maperr - stop a Tx queue due to I/O MMU exhaustion
 *	@q: the queue to stop
 *
 *	Mark a Tx queue stopped due to I/O MMU exhaustion and resulting
 *	inability to map packets.  A periodic timer attempts to restart
 *	queues so marked.
 */
static void txq_stop_maperr(struct sge_uld_txq *q)
{
	q->mapping_err++;
	q->q.stops++;
	set_bit(q->q.cntxt_id - q->adap->sge.egr_start,
		q->adap->sge.txq_maperr);
}

/**
 *	ofldtxq_stop - stop an offload Tx queue that has become full
 *	@q: the queue to stop
 *	@wr: the Work Request causing the queue to become full
 *
 *	Stops an offload Tx queue that has become full and modifies the packet
 *	being written to request a wakeup.
 */
static void ofldtxq_stop(struct sge_uld_txq *q, struct fw_wr_hdr *wr)
{
	wr->lo |= htonl(FW_WR_EQUEQ_F | FW_WR_EQUIQ_F);
	q->q.stops++;
	q->full = 1;
}

/**
 *	service_ofldq - service/restart a suspended offload queue
 *	@q: the offload queue
 *
 *	Services an offload Tx queue by moving packets from its Pending Send
 *	Queue to the Hardware TX ring.  The function starts and ends with the
 *	Send Queue locked, but drops the lock while putting the skb at the
 *	head of the Send Queue onto the Hardware TX Ring.  Dropping the lock
 *	allows more skbs to be added to the Send Queue by other threads.
 *	The packet being processed at the head of the Pending Send Queue is
 *	left on the queue in case we experience DMA Mapping errors, etc.
 *	and need to give up and restart later.
 *
 *	service_ofldq() can be thought of as a task which opportunistically
 *	uses other threads execution contexts.  We use the Offload Queue
 *	boolean "service_ofldq_running" to make sure that only one instance
 *	is ever running at a time ...
 */
static void service_ofldq(struct sge_uld_txq *q)
	__must_hold(&q->sendq.lock)
{
	u64 *pos, *before, *end;
	int credits;
	struct sk_buff *skb;
	struct sge_txq *txq;
	unsigned int left;
	unsigned int written = 0;
	unsigned int flits, ndesc;

	/* If another thread is currently in service_ofldq() processing the
	 * Pending Send Queue then there's nothing to do. Otherwise, flag
	 * that we're doing the work and continue.  Examining/modifying
	 * the Offload Queue boolean "service_ofldq_running" must be done
	 * while holding the Pending Send Queue Lock.
	 */
	if (q->service_ofldq_running)
		return;
	q->service_ofldq_running = true;

	while ((skb = skb_peek(&q->sendq)) != NULL && !q->full) {
		/* We drop the lock while we're working with the skb at the
		 * head of the Pending Send Queue.  This allows more skbs to
		 * be added to the Pending Send Queue while we're working on
		 * this one.  We don't need to lock to guard the TX Ring
		 * updates because only one thread of execution is ever
		 * allowed into service_ofldq() at a time.
		 */
		spin_unlock(&q->sendq.lock);

		cxgb4_reclaim_completed_tx(q->adap, &q->q, false);

		flits = skb->priority;                /* previously saved */
		ndesc = flits_to_desc(flits);
		credits = txq_avail(&q->q) - ndesc;
		BUG_ON(credits < 0);
		if (unlikely(credits < TXQ_STOP_THRES))
			ofldtxq_stop(q, (struct fw_wr_hdr *)skb->data);

		pos = (u64 *)&q->q.desc[q->q.pidx];
		if (is_ofld_imm(skb))
			cxgb4_inline_tx_skb(skb, &q->q, pos);
		else if (cxgb4_map_skb(q->adap->pdev_dev, skb,
				       (dma_addr_t *)skb->head)) {
			txq_stop_maperr(q);
			spin_lock(&q->sendq.lock);
			break;
		} else {
			int last_desc, hdr_len = skb_transport_offset(skb);

			/* The WR headers  may not fit within one descriptor.
			 * So we need to deal with wrap-around here.
			 */
			before = (u64 *)pos;
			end = (u64 *)pos + flits;
			txq = &q->q;
			pos = (void *)inline_tx_skb_header(skb, &q->q,
							   (void *)pos,
							   hdr_len);
			if (before > (u64 *)pos) {
				left = (u8 *)end - (u8 *)txq->stat;
				end = (void *)txq->desc + left;
			}

			/* If current position is already at the end of the
			 * ofld queue, reset the current to point to
			 * start of the queue and update the end ptr as well.
			 */
			if (pos == (u64 *)txq->stat) {
				left = (u8 *)end - (u8 *)txq->stat;
				end = (void *)txq->desc + left;
				pos = (void *)txq->desc;
			}

			cxgb4_write_sgl(skb, &q->q, (void *)pos,
					end, hdr_len,
					(dma_addr_t *)skb->head);
#ifdef CONFIG_NEED_DMA_MAP_STATE
			skb->dev = q->adap->port[0];
			skb->destructor = deferred_unmap_destructor;
#endif
			last_desc = q->q.pidx + ndesc - 1;
			if (last_desc >= q->q.size)
				last_desc -= q->q.size;
			q->q.sdesc[last_desc].skb = skb;
		}

		txq_advance(&q->q, ndesc);
		written += ndesc;
		if (unlikely(written > 32)) {
			cxgb4_ring_tx_db(q->adap, &q->q, written);
			written = 0;
		}

		/* Reacquire the Pending Send Queue Lock so we can unlink the
		 * skb we've just successfully transferred to the TX Ring and
		 * loop for the next skb which may be at the head of the
		 * Pending Send Queue.
		 */
		spin_lock(&q->sendq.lock);
		__skb_unlink(skb, &q->sendq);
		if (is_ofld_imm(skb))
			kfree_skb(skb);
	}
	if (likely(written))
		cxgb4_ring_tx_db(q->adap, &q->q, written);

	/*Indicate that no thread is processing the Pending Send Queue
	 * currently.
	 */
	q->service_ofldq_running = false;
}

/**
 *	ofld_xmit - send a packet through an offload queue
 *	@q: the Tx offload queue
 *	@skb: the packet
 *
 *	Send an offload packet through an SGE offload queue.
 */
static int ofld_xmit(struct sge_uld_txq *q, struct sk_buff *skb)
{
	skb->priority = calc_tx_flits_ofld(skb);       /* save for restart */
	spin_lock(&q->sendq.lock);

	/* Queue the new skb onto the Offload Queue's Pending Send Queue.  If
	 * that results in this new skb being the only one on the queue, start
	 * servicing it.  If there are other skbs already on the list, then
	 * either the queue is currently being processed or it's been stopped
	 * for some reason and it'll be restarted at a later time.  Restart
	 * paths are triggered by events like experiencing a DMA Mapping Error
	 * or filling the Hardware TX Ring.
	 */
	__skb_queue_tail(&q->sendq, skb);
	if (q->sendq.qlen == 1)
		service_ofldq(q);

	spin_unlock(&q->sendq.lock);
	return NET_XMIT_SUCCESS;
}

/**
 *	restart_ofldq - restart a suspended offload queue
 *	@t: pointer to the tasklet associated with this handler
 *
 *	Resumes transmission on a suspended Tx offload queue.
 */
static void restart_ofldq(struct tasklet_struct *t)
{
	struct sge_uld_txq *q = from_tasklet(q, t, qresume_tsk);

	spin_lock(&q->sendq.lock);
	q->full = 0;            /* the queue actually is completely empty now */
	service_ofldq(q);
	spin_unlock(&q->sendq.lock);
}

/**
 *	skb_txq - return the Tx queue an offload packet should use
 *	@skb: the packet
 *
 *	Returns the Tx queue an offload packet should use as indicated by bits
 *	1-15 in the packet's queue_mapping.
 */
static inline unsigned int skb_txq(const struct sk_buff *skb)
{
	return skb->queue_mapping >> 1;
}

/**
 *	is_ctrl_pkt - return whether an offload packet is a control packet
 *	@skb: the packet
 *
 *	Returns whether an offload packet should use an OFLD or a CTRL
 *	Tx queue as indicated by bit 0 in the packet's queue_mapping.
 */
static inline unsigned int is_ctrl_pkt(const struct sk_buff *skb)
{
	return skb->queue_mapping & 1;
}

static inline int uld_send(struct adapter *adap, struct sk_buff *skb,
			   unsigned int tx_uld_type)
{
	struct sge_uld_txq_info *txq_info;
	struct sge_uld_txq *txq;
	unsigned int idx = skb_txq(skb);

	if (unlikely(is_ctrl_pkt(skb))) {
		/* Single ctrl queue is a requirement for LE workaround path */
		if (adap->tids.nsftids)
			idx = 0;
		return ctrl_xmit(&adap->sge.ctrlq[idx], skb);
	}

	txq_info = adap->sge.uld_txq_info[tx_uld_type];
	if (unlikely(!txq_info)) {
		WARN_ON(true);
		kfree_skb(skb);
		return NET_XMIT_DROP;
	}

	txq = &txq_info->uldtxq[idx];
	return ofld_xmit(txq, skb);
}

/**
 *	t4_ofld_send - send an offload packet
 *	@adap: the adapter
 *	@skb: the packet
 *
 *	Sends an offload packet.  We use the packet queue_mapping to select the
 *	appropriate Tx queue as follows: bit 0 indicates whether the packet
 *	should be sent as regular or control, bits 1-15 select the queue.
 */
int t4_ofld_send(struct adapter *adap, struct sk_buff *skb)
{
	int ret;

	local_bh_disable();
	ret = uld_send(adap, skb, CXGB4_TX_OFLD);
	local_bh_enable();
	return ret;
}

/**
 *	cxgb4_ofld_send - send an offload packet
 *	@dev: the net device
 *	@skb: the packet
 *
 *	Sends an offload packet.  This is an exported version of @t4_ofld_send,
 *	intended for ULDs.
 */
int cxgb4_ofld_send(struct net_device *dev, struct sk_buff *skb)
{
	return t4_ofld_send(netdev2adap(dev), skb);
}
EXPORT_SYMBOL(cxgb4_ofld_send);

static void *inline_tx_header(const void *src,
			      const struct sge_txq *q,
			      void *pos, int length)
{
	int left = (void *)q->stat - pos;
	u64 *p;

	if (likely(length <= left)) {
		memcpy(pos, src, length);
		pos += length;
	} else {
		memcpy(pos, src, left);
		memcpy(q->desc, src + left, length - left);
		pos = (void *)q->desc + (length - left);
	}
	/* 0-pad to multiple of 16 */
	p = PTR_ALIGN(pos, 8);
	if ((uintptr_t)p & 8) {
		*p = 0;
		return p + 1;
	}
	return p;
}

/**
 *      ofld_xmit_direct - copy a WR into offload queue
 *      @q: the Tx offload queue
 *      @src: location of WR
 *      @len: WR length
 *
 *      Copy an immediate WR into an uncontended SGE offload queue.
 */
static int ofld_xmit_direct(struct sge_uld_txq *q, const void *src,
			    unsigned int len)
{
	unsigned int ndesc;
	int credits;
	u64 *pos;

	/* Use the lower limit as the cut-off */
	if (len > MAX_IMM_OFLD_TX_DATA_WR_LEN) {
		WARN_ON(1);
		return NET_XMIT_DROP;
	}

	/* Don't return NET_XMIT_CN here as the current
	 * implementation doesn't queue the request
	 * using an skb when the following conditions not met
	 */
	if (!spin_trylock(&q->sendq.lock))
		return NET_XMIT_DROP;

	if (q->full || !skb_queue_empty(&q->sendq) ||
	    q->service_ofldq_running) {
		spin_unlock(&q->sendq.lock);
		return NET_XMIT_DROP;
	}
	ndesc = flits_to_desc(DIV_ROUND_UP(len, 8));
	credits = txq_avail(&q->q) - ndesc;
	pos = (u64 *)&q->q.desc[q->q.pidx];

	/* ofldtxq_stop modifies WR header in-situ */
	inline_tx_header(src, &q->q, pos, len);
	if (unlikely(credits < TXQ_STOP_THRES))
		ofldtxq_stop(q, (struct fw_wr_hdr *)pos);
	txq_advance(&q->q, ndesc);
	cxgb4_ring_tx_db(q->adap, &q->q, ndesc);

	spin_unlock(&q->sendq.lock);
	return NET_XMIT_SUCCESS;
}

int cxgb4_immdata_send(struct net_device *dev, unsigned int idx,
		       const void *src, unsigned int len)
{
	struct sge_uld_txq_info *txq_info;
	struct sge_uld_txq *txq;
	struct adapter *adap;
	int ret;

	adap = netdev2adap(dev);

	local_bh_disable();
	txq_info = adap->sge.uld_txq_info[CXGB4_TX_OFLD];
	if (unlikely(!txq_info)) {
		WARN_ON(true);
		local_bh_enable();
		return NET_XMIT_DROP;
	}
	txq = &txq_info->uldtxq[idx];

	ret = ofld_xmit_direct(txq, src, len);
	local_bh_enable();
	return net_xmit_eval(ret);
}
EXPORT_SYMBOL(cxgb4_immdata_send);

/**
 *	t4_crypto_send - send crypto packet
 *	@adap: the adapter
 *	@skb: the packet
 *
 *	Sends crypto packet.  We use the packet queue_mapping to select the
 *	appropriate Tx queue as follows: bit 0 indicates whether the packet
 *	should be sent as regular or control, bits 1-15 select the queue.
 */
static int t4_crypto_send(struct adapter *adap, struct sk_buff *skb)
{
	int ret;

	local_bh_disable();
	ret = uld_send(adap, skb, CXGB4_TX_CRYPTO);
	local_bh_enable();
	return ret;
}

/**
 *	cxgb4_crypto_send - send crypto packet
 *	@dev: the net device
 *	@skb: the packet
 *
 *	Sends crypto packet.  This is an exported version of @t4_crypto_send,
 *	intended for ULDs.
 */
int cxgb4_crypto_send(struct net_device *dev, struct sk_buff *skb)
{
	return t4_crypto_send(netdev2adap(dev), skb);
}
EXPORT_SYMBOL(cxgb4_crypto_send);

static inline void copy_frags(struct sk_buff *skb,
			      const struct pkt_gl *gl, unsigned int offset)
{
	int i;

	/* usually there's just one frag */
	__skb_fill_page_desc(skb, 0, gl->frags[0].page,
			     gl->frags[0].offset + offset,
			     gl->frags[0].size - offset);
	skb_shinfo(skb)->nr_frags = gl->nfrags;
	for (i = 1; i < gl->nfrags; i++)
		__skb_fill_page_desc(skb, i, gl->frags[i].page,
				     gl->frags[i].offset,
				     gl->frags[i].size);

	/* get a reference to the last page, we don't own it */
	get_page(gl->frags[gl->nfrags - 1].page);
}

/**
 *	cxgb4_pktgl_to_skb - build an sk_buff from a packet gather list
 *	@gl: the gather list
 *	@skb_len: size of sk_buff main body if it carries fragments
 *	@pull_len: amount of data to move to the sk_buff's main body
 *
 *	Builds an sk_buff from the given packet gather list.  Returns the
 *	sk_buff or %NULL if sk_buff allocation failed.
 */
struct sk_buff *cxgb4_pktgl_to_skb(const struct pkt_gl *gl,
				   unsigned int skb_len, unsigned int pull_len)
{
	struct sk_buff *skb;

	/*
	 * Below we rely on RX_COPY_THRES being less than the smallest Rx buffer
	 * size, which is expected since buffers are at least PAGE_SIZEd.
	 * In this case packets up to RX_COPY_THRES have only one fragment.
	 */
	if (gl->tot_len <= RX_COPY_THRES) {
		skb = dev_alloc_skb(gl->tot_len);
		if (unlikely(!skb))
			goto out;
		__skb_put(skb, gl->tot_len);
		skb_copy_to_linear_data(skb, gl->va, gl->tot_len);
	} else {
		skb = dev_alloc_skb(skb_len);
		if (unlikely(!skb))
			goto out;
		__skb_put(skb, pull_len);
		skb_copy_to_linear_data(skb, gl->va, pull_len);

		copy_frags(skb, gl, pull_len);
		skb->len = gl->tot_len;
		skb->data_len = skb->len - pull_len;
		skb->truesize += skb->data_len;
	}
out:	return skb;
}
EXPORT_SYMBOL(cxgb4_pktgl_to_skb);

/**
 *	t4_pktgl_free - free a packet gather list
 *	@gl: the gather list
 *
 *	Releases the pages of a packet gather list.  We do not own the last
 *	page on the list and do not free it.
 */
static void t4_pktgl_free(const struct pkt_gl *gl)
{
	int n;
	const struct page_frag *p;

	for (p = gl->frags, n = gl->nfrags - 1; n--; p++)
		put_page(p->page);
}

/*
 * Process an MPS trace packet.  Give it an unused protocol number so it won't
 * be delivered to anyone and send it to the stack for capture.
 */
static noinline int handle_trace_pkt(struct adapter *adap,
				     const struct pkt_gl *gl)
{
	struct sk_buff *skb;

	skb = cxgb4_pktgl_to_skb(gl, RX_PULL_LEN, RX_PULL_LEN);
	if (unlikely(!skb)) {
		t4_pktgl_free(gl);
		return 0;
	}

	if (is_t4(adap->params.chip))
		__skb_pull(skb, sizeof(struct cpl_trace_pkt));
	else
		__skb_pull(skb, sizeof(struct cpl_t5_trace_pkt));

	skb_reset_mac_header(skb);
	skb->protocol = htons(0xffff);
	skb->dev = adap->port[0];
	netif_receive_skb(skb);
	return 0;
}

/**
 * cxgb4_sgetim_to_hwtstamp - convert sge time stamp to hw time stamp
 * @adap: the adapter
 * @hwtstamps: time stamp structure to update
 * @sgetstamp: 60bit iqe timestamp
 *
 * Every ingress queue entry has the 60-bit timestamp, convert that timestamp
 * which is in Core Clock ticks into ktime_t and assign it
 **/
static void cxgb4_sgetim_to_hwtstamp(struct adapter *adap,
				     struct skb_shared_hwtstamps *hwtstamps,
				     u64 sgetstamp)
{
	u64 ns;
	u64 tmp = (sgetstamp * 1000 * 1000 + adap->params.vpd.cclk / 2);

	ns = div_u64(tmp, adap->params.vpd.cclk);

	memset(hwtstamps, 0, sizeof(*hwtstamps));
	hwtstamps->hwtstamp = ns_to_ktime(ns);
}

static void do_gro(struct sge_eth_rxq *rxq, const struct pkt_gl *gl,
		   const struct cpl_rx_pkt *pkt, unsigned long tnl_hdr_len)
{
	struct adapter *adapter = rxq->rspq.adap;
	struct sge *s = &adapter->sge;
	struct port_info *pi;
	int ret;
	struct sk_buff *skb;

	skb = napi_get_frags(&rxq->rspq.napi);
	if (unlikely(!skb)) {
		t4_pktgl_free(gl);
		rxq->stats.rx_drops++;
		return;
	}

	copy_frags(skb, gl, s->pktshift);
	if (tnl_hdr_len)
		skb->csum_level = 1;
	skb->len = gl->tot_len - s->pktshift;
	skb->data_len = skb->len;
	skb->truesize += skb->data_len;
	skb->ip_summed = CHECKSUM_UNNECESSARY;
	skb_record_rx_queue(skb, rxq->rspq.idx);
	pi = netdev_priv(skb->dev);
	if (pi->rxtstamp)
		cxgb4_sgetim_to_hwtstamp(adapter, skb_hwtstamps(skb),
					 gl->sgetstamp);
	if (rxq->rspq.netdev->features & NETIF_F_RXHASH)
		skb_set_hash(skb, (__force u32)pkt->rsshdr.hash_val,
			     PKT_HASH_TYPE_L3);

	if (unlikely(pkt->vlan_ex)) {
		__vlan_hwaccel_put_tag(skb, htons(ETH_P_8021Q), ntohs(pkt->vlan));
		rxq->stats.vlan_ex++;
	}
	ret = napi_gro_frags(&rxq->rspq.napi);
	if (ret == GRO_HELD)
		rxq->stats.lro_pkts++;
	else if (ret == GRO_MERGED || ret == GRO_MERGED_FREE)
		rxq->stats.lro_merged++;
	rxq->stats.pkts++;
	rxq->stats.rx_cso++;
}

enum {
	RX_NON_PTP_PKT = 0,
	RX_PTP_PKT_SUC = 1,
	RX_PTP_PKT_ERR = 2
};

/**
 *     t4_systim_to_hwstamp - read hardware time stamp
 *     @adapter: the adapter
 *     @skb: the packet
 *
 *     Read Time Stamp from MPS packet and insert in skb which
 *     is forwarded to PTP application
 */
static noinline int t4_systim_to_hwstamp(struct adapter *adapter,
					 struct sk_buff *skb)
{
	struct skb_shared_hwtstamps *hwtstamps;
	struct cpl_rx_mps_pkt *cpl = NULL;
	unsigned char *data;
	int offset;

	cpl = (struct cpl_rx_mps_pkt *)skb->data;
	if (!(CPL_RX_MPS_PKT_TYPE_G(ntohl(cpl->op_to_r1_hi)) &
	     X_CPL_RX_MPS_PKT_TYPE_PTP))
		return RX_PTP_PKT_ERR;

	data = skb->data + sizeof(*cpl);
	skb_pull(skb, 2 * sizeof(u64) + sizeof(struct cpl_rx_mps_pkt));
	offset = ETH_HLEN + IPV4_HLEN(skb->data) + UDP_HLEN;
	if (skb->len < offset + OFF_PTP_SEQUENCE_ID + sizeof(short))
		return RX_PTP_PKT_ERR;

	hwtstamps = skb_hwtstamps(skb);
	memset(hwtstamps, 0, sizeof(*hwtstamps));
	hwtstamps->hwtstamp = ns_to_ktime(get_unaligned_be64(data));

	return RX_PTP_PKT_SUC;
}

/**
 *     t4_rx_hststamp - Recv PTP Event Message
 *     @adapter: the adapter
 *     @rsp: the response queue descriptor holding the RX_PKT message
 *     @rxq: the response queue holding the RX_PKT message
 *     @skb: the packet
 *
 *     PTP enabled and MPS packet, read HW timestamp
 */
static int t4_rx_hststamp(struct adapter *adapter, const __be64 *rsp,
			  struct sge_eth_rxq *rxq, struct sk_buff *skb)
{
	int ret;

	if (unlikely((*(u8 *)rsp == CPL_RX_MPS_PKT) &&
		     !is_t4(adapter->params.chip))) {
		ret = t4_systim_to_hwstamp(adapter, skb);
		if (ret == RX_PTP_PKT_ERR) {
			kfree_skb(skb);
			rxq->stats.rx_drops++;
		}
		return ret;
	}
	return RX_NON_PTP_PKT;
}

/**
 *      t4_tx_hststamp - Loopback PTP Transmit Event Message
 *      @adapter: the adapter
 *      @skb: the packet
 *      @dev: the ingress net device
 *
 *      Read hardware timestamp for the loopback PTP Tx event message
 */
static int t4_tx_hststamp(struct adapter *adapter, struct sk_buff *skb,
			  struct net_device *dev)
{
	struct port_info *pi = netdev_priv(dev);

	if (!is_t4(adapter->params.chip) && adapter->ptp_tx_skb) {
		cxgb4_ptp_read_hwstamp(adapter, pi);
		kfree_skb(skb);
		return 0;
	}
	return 1;
}

/**
 *	t4_tx_completion_handler - handle CPL_SGE_EGR_UPDATE messages
 *	@rspq: Ethernet RX Response Queue associated with Ethernet TX Queue
 *	@rsp: Response Entry pointer into Response Queue
 *	@gl: Gather List pointer
 *
 *	For adapters which support the SGE Doorbell Queue Timer facility,
 *	we configure the Ethernet TX Queues to send CIDX Updates to the
 *	Associated Ethernet RX Response Queue with CPL_SGE_EGR_UPDATE
 *	messages.  This adds a small load to PCIe Link RX bandwidth and,
 *	potentially, higher CPU Interrupt load, but allows us to respond
 *	much more quickly to the CIDX Updates.  This is important for
 *	Upper Layer Software which isn't willing to have a large amount
 *	of TX Data outstanding before receiving DMA Completions.
 */
static void t4_tx_completion_handler(struct sge_rspq *rspq,
				     const __be64 *rsp,
				     const struct pkt_gl *gl)
{
	u8 opcode = ((const struct rss_header *)rsp)->opcode;
	struct port_info *pi = netdev_priv(rspq->netdev);
	struct adapter *adapter = rspq->adap;
	struct sge *s = &adapter->sge;
	struct sge_eth_txq *txq;

	/* skip RSS header */
	rsp++;

	/* FW can send EGR_UPDATEs encapsulated in a CPL_FW4_MSG.
	 */
	if (unlikely(opcode == CPL_FW4_MSG &&
		     ((const struct cpl_fw4_msg *)rsp)->type ==
							FW_TYPE_RSSCPL)) {
		rsp++;
		opcode = ((const struct rss_header *)rsp)->opcode;
		rsp++;
	}

	if (unlikely(opcode != CPL_SGE_EGR_UPDATE)) {
		pr_info("%s: unexpected FW4/CPL %#x on Rx queue\n",
			__func__, opcode);
		return;
	}

	txq = &s->ethtxq[pi->first_qset + rspq->idx];

	/* We've got the Hardware Consumer Index Update in the Egress Update
	 * message. These Egress Update messages will be our sole CIDX Updates
	 * we get since we don't want to chew up PCIe bandwidth for both Ingress
	 * Messages and Status Page writes.  However, The code which manages
	 * reclaiming successfully DMA'ed TX Work Requests uses the CIDX value
	 * stored in the Status Page at the end of the TX Queue.  It's easiest
	 * to simply copy the CIDX Update value from the Egress Update message
	 * to the Status Page.  Also note that no Endian issues need to be
	 * considered here since both are Big Endian and we're just copying
	 * bytes consistently ...
	 */
	if (CHELSIO_CHIP_VERSION(adapter->params.chip) <= CHELSIO_T5) {
		struct cpl_sge_egr_update *egr;

		egr = (struct cpl_sge_egr_update *)rsp;
		WRITE_ONCE(txq->q.stat->cidx, egr->cidx);
	}

	t4_sge_eth_txq_egress_update(adapter, txq, -1);
}

static int cxgb4_validate_lb_pkt(struct port_info *pi, const struct pkt_gl *si)
{
	struct adapter *adap = pi->adapter;
	struct cxgb4_ethtool_lb_test *lb;
	struct sge *s = &adap->sge;
	struct net_device *netdev;
	u8 *data;
	int i;

	netdev = adap->port[pi->port_id];
	lb = &pi->ethtool_lb;
	data = si->va + s->pktshift;

	i = ETH_ALEN;
	if (!ether_addr_equal(data + i, netdev->dev_addr))
		return -1;

	i += ETH_ALEN;
	if (strcmp(&data[i], CXGB4_SELFTEST_LB_STR))
		lb->result = -EIO;

	complete(&lb->completion);
	return 0;
}

/**
 *	t4_ethrx_handler - process an ingress ethernet packet
 *	@q: the response queue that received the packet
 *	@rsp: the response queue descriptor holding the RX_PKT message
 *	@si: the gather list of packet fragments
 *
 *	Process an ingress ethernet packet and deliver it to the stack.
 */
int t4_ethrx_handler(struct sge_rspq *q, const __be64 *rsp,
		     const struct pkt_gl *si)
{
	bool csum_ok;
	struct sk_buff *skb;
	const struct cpl_rx_pkt *pkt;
	struct sge_eth_rxq *rxq = container_of(q, struct sge_eth_rxq, rspq);
	struct adapter *adapter = q->adap;
	struct sge *s = &q->adap->sge;
	int cpl_trace_pkt = is_t4(q->adap->params.chip) ?
			    CPL_TRACE_PKT : CPL_TRACE_PKT_T5;
	u16 err_vec, tnl_hdr_len = 0;
	struct port_info *pi;
	int ret = 0;

	pi = netdev_priv(q->netdev);
	/* If we're looking at TX Queue CIDX Update, handle that separately
	 * and return.
	 */
	if (unlikely((*(u8 *)rsp == CPL_FW4_MSG) ||
		     (*(u8 *)rsp == CPL_SGE_EGR_UPDATE))) {
		t4_tx_completion_handler(q, rsp, si);
		return 0;
	}

	if (unlikely(*(u8 *)rsp == cpl_trace_pkt))
		return handle_trace_pkt(q->adap, si);

	pkt = (const struct cpl_rx_pkt *)rsp;
	/* Compressed error vector is enabled for T6 only */
	if (q->adap->params.tp.rx_pkt_encap) {
		err_vec = T6_COMPR_RXERR_VEC_G(be16_to_cpu(pkt->err_vec));
		tnl_hdr_len = T6_RX_TNLHDR_LEN_G(ntohs(pkt->err_vec));
	} else {
		err_vec = be16_to_cpu(pkt->err_vec);
	}

	csum_ok = pkt->csum_calc && !err_vec &&
		  (q->netdev->features & NETIF_F_RXCSUM);

	if (err_vec)
		rxq->stats.bad_rx_pkts++;

	if (unlikely(pi->ethtool_lb.loopback && pkt->iff >= NCHAN)) {
		ret = cxgb4_validate_lb_pkt(pi, si);
		if (!ret)
			return 0;
	}

	if (((pkt->l2info & htonl(RXF_TCP_F)) ||
	     tnl_hdr_len) &&
	    (q->netdev->features & NETIF_F_GRO) && csum_ok && !pkt->ip_frag) {
		do_gro(rxq, si, pkt, tnl_hdr_len);
		return 0;
	}

	skb = cxgb4_pktgl_to_skb(si, RX_PKT_SKB_LEN, RX_PULL_LEN);
	if (unlikely(!skb)) {
		t4_pktgl_free(si);
		rxq->stats.rx_drops++;
		return 0;
	}

	/* Handle PTP Event Rx packet */
	if (unlikely(pi->ptp_enable)) {
		ret = t4_rx_hststamp(adapter, rsp, rxq, skb);
		if (ret == RX_PTP_PKT_ERR)
			return 0;
	}
	if (likely(!ret))
		__skb_pull(skb, s->pktshift); /* remove ethernet header pad */

	/* Handle the PTP Event Tx Loopback packet */
	if (unlikely(pi->ptp_enable && !ret &&
		     (pkt->l2info & htonl(RXF_UDP_F)) &&
		     cxgb4_ptp_is_ptp_rx(skb))) {
		if (!t4_tx_hststamp(adapter, skb, q->netdev))
			return 0;
	}

	skb->protocol = eth_type_trans(skb, q->netdev);
	skb_record_rx_queue(skb, q->idx);
	if (skb->dev->features & NETIF_F_RXHASH)
		skb_set_hash(skb, (__force u32)pkt->rsshdr.hash_val,
			     PKT_HASH_TYPE_L3);

	rxq->stats.pkts++;

	if (pi->rxtstamp)
		cxgb4_sgetim_to_hwtstamp(q->adap, skb_hwtstamps(skb),
					 si->sgetstamp);
	if (csum_ok && (pkt->l2info & htonl(RXF_UDP_F | RXF_TCP_F))) {
		if (!pkt->ip_frag) {
			skb->ip_summed = CHECKSUM_UNNECESSARY;
			rxq->stats.rx_cso++;
		} else if (pkt->l2info & htonl(RXF_IP_F)) {
			__sum16 c = (__force __sum16)pkt->csum;
			skb->csum = csum_unfold(c);

			if (tnl_hdr_len) {
				skb->ip_summed = CHECKSUM_UNNECESSARY;
				skb->csum_level = 1;
			} else {
				skb->ip_summed = CHECKSUM_COMPLETE;
			}
			rxq->stats.rx_cso++;
		}
	} else {
		skb_checksum_none_assert(skb);
#ifdef CONFIG_CHELSIO_T4_FCOE
#define CPL_RX_PKT_FLAGS (RXF_PSH_F | RXF_SYN_F | RXF_UDP_F | \
			  RXF_TCP_F | RXF_IP_F | RXF_IP6_F | RXF_LRO_F)

		if (!(pkt->l2info & cpu_to_be32(CPL_RX_PKT_FLAGS))) {
			if ((pkt->l2info & cpu_to_be32(RXF_FCOE_F)) &&
			    (pi->fcoe.flags & CXGB_FCOE_ENABLED)) {
				if (q->adap->params.tp.rx_pkt_encap)
					csum_ok = err_vec &
						  T6_COMPR_RXERR_SUM_F;
				else
					csum_ok = err_vec & RXERR_CSUM_F;
				if (!csum_ok)
					skb->ip_summed = CHECKSUM_UNNECESSARY;
			}
		}

#undef CPL_RX_PKT_FLAGS
#endif /* CONFIG_CHELSIO_T4_FCOE */
	}

	if (unlikely(pkt->vlan_ex)) {
		__vlan_hwaccel_put_tag(skb, htons(ETH_P_8021Q), ntohs(pkt->vlan));
		rxq->stats.vlan_ex++;
	}
	skb_mark_napi_id(skb, &q->napi);
	netif_receive_skb(skb);
	return 0;
}

/**
 *	restore_rx_bufs - put back a packet's Rx buffers
 *	@si: the packet gather list
 *	@q: the SGE free list
 *	@frags: number of FL buffers to restore
 *
 *	Puts back on an FL the Rx buffers associated with @si.  The buffers
 *	have already been unmapped and are left unmapped, we mark them so to
 *	prevent further unmapping attempts.
 *
 *	This function undoes a series of @unmap_rx_buf calls when we find out
 *	that the current packet can't be processed right away afterall and we
 *	need to come back to it later.  This is a very rare event and there's
 *	no effort to make this particularly efficient.
 */
static void restore_rx_bufs(const struct pkt_gl *si, struct sge_fl *q,
			    int frags)
{
	struct rx_sw_desc *d;

	while (frags--) {
		if (q->cidx == 0)
			q->cidx = q->size - 1;
		else
			q->cidx--;
		d = &q->sdesc[q->cidx];
		d->page = si->frags[frags].page;
		d->dma_addr |= RX_UNMAPPED_BUF;
		q->avail++;
	}
}

/**
 *	is_new_response - check if a response is newly written
 *	@r: the response descriptor
 *	@q: the response queue
 *
 *	Returns true if a response descriptor contains a yet unprocessed
 *	response.
 */
static inline bool is_new_response(const struct rsp_ctrl *r,
				   const struct sge_rspq *q)
{
	return (r->type_gen >> RSPD_GEN_S) == q->gen;
}

/**
 *	rspq_next - advance to the next entry in a response queue
 *	@q: the queue
 *
 *	Updates the state of a response queue to advance it to the next entry.
 */
static inline void rspq_next(struct sge_rspq *q)
{
	q->cur_desc = (void *)q->cur_desc + q->iqe_len;
	if (unlikely(++q->cidx == q->size)) {
		q->cidx = 0;
		q->gen ^= 1;
		q->cur_desc = q->desc;
	}
}

/**
 *	process_responses - process responses from an SGE response queue
 *	@q: the ingress queue to process
 *	@budget: how many responses can be processed in this round
 *
 *	Process responses from an SGE response queue up to the supplied budget.
 *	Responses include received packets as well as control messages from FW
 *	or HW.
 *
 *	Additionally choose the interrupt holdoff time for the next interrupt
 *	on this queue.  If the system is under memory shortage use a fairly
 *	long delay to help recovery.
 */
static int process_responses(struct sge_rspq *q, int budget)
{
	int ret, rsp_type;
	int budget_left = budget;
	const struct rsp_ctrl *rc;
	struct sge_eth_rxq *rxq = container_of(q, struct sge_eth_rxq, rspq);
	struct adapter *adapter = q->adap;
	struct sge *s = &adapter->sge;

	while (likely(budget_left)) {
		rc = (void *)q->cur_desc + (q->iqe_len - sizeof(*rc));
		if (!is_new_response(rc, q)) {
			if (q->flush_handler)
				q->flush_handler(q);
			break;
		}

		dma_rmb();
		rsp_type = RSPD_TYPE_G(rc->type_gen);
		if (likely(rsp_type == RSPD_TYPE_FLBUF_X)) {
			struct page_frag *fp;
			struct pkt_gl si;
			const struct rx_sw_desc *rsd;
			u32 len = ntohl(rc->pldbuflen_qid), bufsz, frags;

			if (len & RSPD_NEWBUF_F) {
				if (likely(q->offset > 0)) {
					free_rx_bufs(q->adap, &rxq->fl, 1);
					q->offset = 0;
				}
				len = RSPD_LEN_G(len);
			}
			si.tot_len = len;

			/* gather packet fragments */
			for (frags = 0, fp = si.frags; ; frags++, fp++) {
				rsd = &rxq->fl.sdesc[rxq->fl.cidx];
				bufsz = get_buf_size(adapter, rsd);
				fp->page = rsd->page;
				fp->offset = q->offset;
				fp->size = min(bufsz, len);
				len -= fp->size;
				if (!len)
					break;
				unmap_rx_buf(q->adap, &rxq->fl);
			}

			si.sgetstamp = SGE_TIMESTAMP_G(
					be64_to_cpu(rc->last_flit));
			/*
			 * Last buffer remains mapped so explicitly make it
			 * coherent for CPU access.
			 */
			dma_sync_single_for_cpu(q->adap->pdev_dev,
						get_buf_addr(rsd),
						fp->size, DMA_FROM_DEVICE);

			si.va = page_address(si.frags[0].page) +
				si.frags[0].offset;
			prefetch(si.va);

			si.nfrags = frags + 1;
			ret = q->handler(q, q->cur_desc, &si);
			if (likely(ret == 0))
				q->offset += ALIGN(fp->size, s->fl_align);
			else
				restore_rx_bufs(&si, &rxq->fl, frags);
		} else if (likely(rsp_type == RSPD_TYPE_CPL_X)) {
			ret = q->handler(q, q->cur_desc, NULL);
		} else {
			ret = q->handler(q, (const __be64 *)rc, CXGB4_MSG_AN);
		}

		if (unlikely(ret)) {
			/* couldn't process descriptor, back off for recovery */
			q->next_intr_params = QINTR_TIMER_IDX_V(NOMEM_TMR_IDX);
			break;
		}

		rspq_next(q);
		budget_left--;
	}

	if (q->offset >= 0 && fl_cap(&rxq->fl) - rxq->fl.avail >= 16)
		__refill_fl(q->adap, &rxq->fl);
	return budget - budget_left;
}

/**
 *	napi_rx_handler - the NAPI handler for Rx processing
 *	@napi: the napi instance
 *	@budget: how many packets we can process in this round
 *
 *	Handler for new data events when using NAPI.  This does not need any
 *	locking or protection from interrupts as data interrupts are off at
 *	this point and other adapter interrupts do not interfere (the latter
 *	in not a concern at all with MSI-X as non-data interrupts then have
 *	a separate handler).
 */
static int napi_rx_handler(struct napi_struct *napi, int budget)
{
	unsigned int params;
	struct sge_rspq *q = container_of(napi, struct sge_rspq, napi);
	int work_done;
	u32 val;

	work_done = process_responses(q, budget);
	if (likely(work_done < budget)) {
		int timer_index;

		napi_complete_done(napi, work_done);
		timer_index = QINTR_TIMER_IDX_G(q->next_intr_params);

		if (q->adaptive_rx) {
			if (work_done > max(timer_pkt_quota[timer_index],
					    MIN_NAPI_WORK))
				timer_index = (timer_index + 1);
			else
				timer_index = timer_index - 1;

			timer_index = clamp(timer_index, 0, SGE_TIMERREGS - 1);
			q->next_intr_params =
					QINTR_TIMER_IDX_V(timer_index) |
					QINTR_CNT_EN_V(0);
			params = q->next_intr_params;
		} else {
			params = q->next_intr_params;
			q->next_intr_params = q->intr_params;
		}
	} else
		params = QINTR_TIMER_IDX_V(7);

	val = CIDXINC_V(work_done) | SEINTARM_V(params);

	/* If we don't have access to the new User GTS (T5+), use the old
	 * doorbell mechanism; otherwise use the new BAR2 mechanism.
	 */
	if (unlikely(q->bar2_addr == NULL)) {
		t4_write_reg(q->adap, MYPF_REG(SGE_PF_GTS_A),
			     val | INGRESSQID_V((u32)q->cntxt_id));
	} else {
		writel(val | INGRESSQID_V(q->bar2_qid),
		       q->bar2_addr + SGE_UDB_GTS);
		wmb();
	}
	return work_done;
}

void cxgb4_ethofld_restart(struct tasklet_struct *t)
{
	struct sge_eosw_txq *eosw_txq = from_tasklet(eosw_txq, t,
						     qresume_tsk);
	int pktcount;

	spin_lock(&eosw_txq->lock);
	pktcount = eosw_txq->cidx - eosw_txq->last_cidx;
	if (pktcount < 0)
		pktcount += eosw_txq->ndesc;

	if (pktcount) {
		cxgb4_eosw_txq_free_desc(netdev2adap(eosw_txq->netdev),
					 eosw_txq, pktcount);
		eosw_txq->inuse -= pktcount;
	}

	/* There may be some packets waiting for completions. So,
	 * attempt to send these packets now.
	 */
	ethofld_xmit(eosw_txq->netdev, eosw_txq);
	spin_unlock(&eosw_txq->lock);
}

/* cxgb4_ethofld_rx_handler - Process ETHOFLD Tx completions
 * @q: the response queue that received the packet
 * @rsp: the response queue descriptor holding the CPL message
 * @si: the gather list of packet fragments
 *
 * Process a ETHOFLD Tx completion. Increment the cidx here, but
 * free up the descriptors in a tasklet later.
 */
int cxgb4_ethofld_rx_handler(struct sge_rspq *q, const __be64 *rsp,
			     const struct pkt_gl *si)
{
	u8 opcode = ((const struct rss_header *)rsp)->opcode;

	/* skip RSS header */
	rsp++;

	if (opcode == CPL_FW4_ACK) {
		const struct cpl_fw4_ack *cpl;
		struct sge_eosw_txq *eosw_txq;
		struct eotid_entry *entry;
		struct sk_buff *skb;
		u32 hdr_len, eotid;
		u8 flits, wrlen16;
		int credits;

		cpl = (const struct cpl_fw4_ack *)rsp;
		eotid = CPL_FW4_ACK_FLOWID_G(ntohl(OPCODE_TID(cpl))) -
			q->adap->tids.eotid_base;
		entry = cxgb4_lookup_eotid(&q->adap->tids, eotid);
		if (!entry)
			goto out_done;

		eosw_txq = (struct sge_eosw_txq *)entry->data;
		if (!eosw_txq)
			goto out_done;

		spin_lock(&eosw_txq->lock);
		credits = cpl->credits;
		while (credits > 0) {
			skb = eosw_txq->desc[eosw_txq->cidx].skb;
			if (!skb)
				break;

			if (unlikely((eosw_txq->state ==
				      CXGB4_EO_STATE_FLOWC_OPEN_REPLY ||
				      eosw_txq->state ==
				      CXGB4_EO_STATE_FLOWC_CLOSE_REPLY) &&
				     eosw_txq->cidx == eosw_txq->flowc_idx)) {
				flits = DIV_ROUND_UP(skb->len, 8);
				if (eosw_txq->state ==
				    CXGB4_EO_STATE_FLOWC_OPEN_REPLY)
					eosw_txq->state = CXGB4_EO_STATE_ACTIVE;
				else
					eosw_txq->state = CXGB4_EO_STATE_CLOSED;
				complete(&eosw_txq->completion);
			} else {
				hdr_len = eth_get_headlen(eosw_txq->netdev,
							  skb->data,
							  skb_headlen(skb));
				flits = ethofld_calc_tx_flits(q->adap, skb,
							      hdr_len);
			}
			eosw_txq_advance_index(&eosw_txq->cidx, 1,
					       eosw_txq->ndesc);
			wrlen16 = DIV_ROUND_UP(flits * 8, 16);
			credits -= wrlen16;
		}

		eosw_txq->cred += cpl->credits;
		eosw_txq->ncompl--;

		spin_unlock(&eosw_txq->lock);

		/* Schedule a tasklet to reclaim SKBs and restart ETHOFLD Tx,
		 * if there were packets waiting for completion.
		 */
		tasklet_schedule(&eosw_txq->qresume_tsk);
	}

out_done:
	return 0;
}

/*
 * The MSI-X interrupt handler for an SGE response queue.
 */
irqreturn_t t4_sge_intr_msix(int irq, void *cookie)
{
	struct sge_rspq *q = cookie;

	napi_schedule(&q->napi);
	return IRQ_HANDLED;
}

/*
 * Process the indirect interrupt entries in the interrupt queue and kick off
 * NAPI for each queue that has generated an entry.
 */
static unsigned int process_intrq(struct adapter *adap)
{
	unsigned int credits;
	const struct rsp_ctrl *rc;
	struct sge_rspq *q = &adap->sge.intrq;
	u32 val;

	spin_lock(&adap->sge.intrq_lock);
	for (credits = 0; ; credits++) {
		rc = (void *)q->cur_desc + (q->iqe_len - sizeof(*rc));
		if (!is_new_response(rc, q))
			break;

		dma_rmb();
		if (RSPD_TYPE_G(rc->type_gen) == RSPD_TYPE_INTR_X) {
			unsigned int qid = ntohl(rc->pldbuflen_qid);

			qid -= adap->sge.ingr_start;
			napi_schedule(&adap->sge.ingr_map[qid]->napi);
		}

		rspq_next(q);
	}

	val =  CIDXINC_V(credits) | SEINTARM_V(q->intr_params);

	/* If we don't have access to the new User GTS (T5+), use the old
	 * doorbell mechanism; otherwise use the new BAR2 mechanism.
	 */
	if (unlikely(q->bar2_addr == NULL)) {
		t4_write_reg(adap, MYPF_REG(SGE_PF_GTS_A),
			     val | INGRESSQID_V(q->cntxt_id));
	} else {
		writel(val | INGRESSQID_V(q->bar2_qid),
		       q->bar2_addr + SGE_UDB_GTS);
		wmb();
	}
	spin_unlock(&adap->sge.intrq_lock);
	return credits;
}

/*
 * The MSI interrupt handler, which handles data events from SGE response queues
 * as well as error and other async events as they all use the same MSI vector.
 */
static irqreturn_t t4_intr_msi(int irq, void *cookie)
{
	struct adapter *adap = cookie;

	if (adap->flags & CXGB4_MASTER_PF)
		t4_slow_intr_handler(adap);
	process_intrq(adap);
	return IRQ_HANDLED;
}

/*
 * Interrupt handler for legacy INTx interrupts.
 * Handles data events from SGE response queues as well as error and other
 * async events as they all use the same interrupt line.
 */
static irqreturn_t t4_intr_intx(int irq, void *cookie)
{
	struct adapter *adap = cookie;

	t4_write_reg(adap, MYPF_REG(PCIE_PF_CLI_A), 0);
	if (((adap->flags & CXGB4_MASTER_PF) && t4_slow_intr_handler(adap)) |
	    process_intrq(adap))
		return IRQ_HANDLED;
	return IRQ_NONE;             /* probably shared interrupt */
}

/**
 *	t4_intr_handler - select the top-level interrupt handler
 *	@adap: the adapter
 *
 *	Selects the top-level interrupt handler based on the type of interrupts
 *	(MSI-X, MSI, or INTx).
 */
irq_handler_t t4_intr_handler(struct adapter *adap)
{
	if (adap->flags & CXGB4_USING_MSIX)
		return t4_sge_intr_msix;
	if (adap->flags & CXGB4_USING_MSI)
		return t4_intr_msi;
	return t4_intr_intx;
}

static void sge_rx_timer_cb(struct timer_list *t)
{
	unsigned long m;
	unsigned int i;
	struct adapter *adap = from_timer(adap, t, sge.rx_timer);
	struct sge *s = &adap->sge;

	for (i = 0; i < BITS_TO_LONGS(s->egr_sz); i++)
		for (m = s->starving_fl[i]; m; m &= m - 1) {
			struct sge_eth_rxq *rxq;
			unsigned int id = __ffs(m) + i * BITS_PER_LONG;
			struct sge_fl *fl = s->egr_map[id];

			clear_bit(id, s->starving_fl);
			smp_mb__after_atomic();

			if (fl_starving(adap, fl)) {
				rxq = container_of(fl, struct sge_eth_rxq, fl);
				if (napi_reschedule(&rxq->rspq.napi))
					fl->starving++;
				else
					set_bit(id, s->starving_fl);
			}
		}
	/* The remainder of the SGE RX Timer Callback routine is dedicated to
	 * global Master PF activities like checking for chip ingress stalls,
	 * etc.
	 */
	if (!(adap->flags & CXGB4_MASTER_PF))
		goto done;

	t4_idma_monitor(adap, &s->idma_monitor, HZ, RX_QCHECK_PERIOD);

done:
	mod_timer(&s->rx_timer, jiffies + RX_QCHECK_PERIOD);
}

static void sge_tx_timer_cb(struct timer_list *t)
{
	struct adapter *adap = from_timer(adap, t, sge.tx_timer);
	struct sge *s = &adap->sge;
	unsigned long m, period;
	unsigned int i, budget;

	for (i = 0; i < BITS_TO_LONGS(s->egr_sz); i++)
		for (m = s->txq_maperr[i]; m; m &= m - 1) {
			unsigned long id = __ffs(m) + i * BITS_PER_LONG;
			struct sge_uld_txq *txq = s->egr_map[id];

			clear_bit(id, s->txq_maperr);
			tasklet_schedule(&txq->qresume_tsk);
		}

	if (!is_t4(adap->params.chip)) {
		struct sge_eth_txq *q = &s->ptptxq;
		int avail;

		spin_lock(&adap->ptp_lock);
		avail = reclaimable(&q->q);

		if (avail) {
			free_tx_desc(adap, &q->q, avail, false);
			q->q.in_use -= avail;
		}
		spin_unlock(&adap->ptp_lock);
	}

	budget = MAX_TIMER_TX_RECLAIM;
	i = s->ethtxq_rover;
	do {
		budget -= t4_sge_eth_txq_egress_update(adap, &s->ethtxq[i],
						       budget);
		if (!budget)
			break;

		if (++i >= s->ethqsets)
			i = 0;
	} while (i != s->ethtxq_rover);
	s->ethtxq_rover = i;

	if (budget == 0) {
		/* If we found too many reclaimable packets schedule a timer
		 * in the near future to continue where we left off.
		 */
		period = 2;
	} else {
		/* We reclaimed all reclaimable TX Descriptors, so reschedule
		 * at the normal period.
		 */
		period = TX_QCHECK_PERIOD;
	}

	mod_timer(&s->tx_timer, jiffies + period);
}

/**
 *	bar2_address - return the BAR2 address for an SGE Queue's Registers
 *	@adapter: the adapter
 *	@qid: the SGE Queue ID
 *	@qtype: the SGE Queue Type (Egress or Ingress)
 *	@pbar2_qid: BAR2 Queue ID or 0 for Queue ID inferred SGE Queues
 *
 *	Returns the BAR2 address for the SGE Queue Registers associated with
 *	@qid.  If BAR2 SGE Registers aren't available, returns NULL.  Also
 *	returns the BAR2 Queue ID to be used with writes to the BAR2 SGE
 *	Queue Registers.  If the BAR2 Queue ID is 0, then "Inferred Queue ID"
 *	Registers are supported (e.g. the Write Combining Doorbell Buffer).
 */
static void __iomem *bar2_address(struct adapter *adapter,
				  unsigned int qid,
				  enum t4_bar2_qtype qtype,
				  unsigned int *pbar2_qid)
{
	u64 bar2_qoffset;
	int ret;

	ret = t4_bar2_sge_qregs(adapter, qid, qtype, 0,
				&bar2_qoffset, pbar2_qid);
	if (ret)
		return NULL;

	return adapter->bar2 + bar2_qoffset;
}

/* @intr_idx: MSI/MSI-X vector if >=0, -(absolute qid + 1) if < 0
 * @cong: < 0 -> no congestion feedback, >= 0 -> congestion channel map
 */
int t4_sge_alloc_rxq(struct adapter *adap, struct sge_rspq *iq, bool fwevtq,
		     struct net_device *dev, int intr_idx,
		     struct sge_fl *fl, rspq_handler_t hnd,
		     rspq_flush_handler_t flush_hnd, int cong)
{
	int ret, flsz = 0;
	struct fw_iq_cmd c;
	struct sge *s = &adap->sge;
	struct port_info *pi = netdev_priv(dev);
	int relaxed = !(adap->flags & CXGB4_ROOT_NO_RELAXED_ORDERING);

	/* Size needs to be multiple of 16, including status entry. */
	iq->size = roundup(iq->size, 16);

	iq->desc = alloc_ring(adap->pdev_dev, iq->size, iq->iqe_len, 0,
			      &iq->phys_addr, NULL, 0,
			      dev_to_node(adap->pdev_dev));
	if (!iq->desc)
		return -ENOMEM;

	memset(&c, 0, sizeof(c));
	c.op_to_vfn = htonl(FW_CMD_OP_V(FW_IQ_CMD) | FW_CMD_REQUEST_F |
			    FW_CMD_WRITE_F | FW_CMD_EXEC_F |
			    FW_IQ_CMD_PFN_V(adap->pf) | FW_IQ_CMD_VFN_V(0));
	c.alloc_to_len16 = htonl(FW_IQ_CMD_ALLOC_F | FW_IQ_CMD_IQSTART_F |
				 FW_LEN16(c));
	c.type_to_iqandstindex = htonl(FW_IQ_CMD_TYPE_V(FW_IQ_TYPE_FL_INT_CAP) |
		FW_IQ_CMD_IQASYNCH_V(fwevtq) | FW_IQ_CMD_VIID_V(pi->viid) |
		FW_IQ_CMD_IQANDST_V(intr_idx < 0) |
		FW_IQ_CMD_IQANUD_V(UPDATEDELIVERY_INTERRUPT_X) |
		FW_IQ_CMD_IQANDSTINDEX_V(intr_idx >= 0 ? intr_idx :
							-intr_idx - 1));
	c.iqdroprss_to_iqesize = htons(FW_IQ_CMD_IQPCIECH_V(pi->tx_chan) |
		FW_IQ_CMD_IQGTSMODE_F |
		FW_IQ_CMD_IQINTCNTTHRESH_V(iq->pktcnt_idx) |
		FW_IQ_CMD_IQESIZE_V(ilog2(iq->iqe_len) - 4));
	c.iqsize = htons(iq->size);
	c.iqaddr = cpu_to_be64(iq->phys_addr);
	if (cong >= 0)
		c.iqns_to_fl0congen = htonl(FW_IQ_CMD_IQFLINTCONGEN_F |
				FW_IQ_CMD_IQTYPE_V(cong ? FW_IQ_IQTYPE_NIC
							:  FW_IQ_IQTYPE_OFLD));

	if (fl) {
		unsigned int chip_ver =
			CHELSIO_CHIP_VERSION(adap->params.chip);

		/* Allocate the ring for the hardware free list (with space
		 * for its status page) along with the associated software
		 * descriptor ring.  The free list size needs to be a multiple
		 * of the Egress Queue Unit and at least 2 Egress Units larger
		 * than the SGE's Egress Congrestion Threshold
		 * (fl_starve_thres - 1).
		 */
		if (fl->size < s->fl_starve_thres - 1 + 2 * 8)
			fl->size = s->fl_starve_thres - 1 + 2 * 8;
		fl->size = roundup(fl->size, 8);
		fl->desc = alloc_ring(adap->pdev_dev, fl->size, sizeof(__be64),
				      sizeof(struct rx_sw_desc), &fl->addr,
				      &fl->sdesc, s->stat_len,
				      dev_to_node(adap->pdev_dev));
		if (!fl->desc)
			goto fl_nomem;

		flsz = fl->size / 8 + s->stat_len / sizeof(struct tx_desc);
		c.iqns_to_fl0congen |= htonl(FW_IQ_CMD_FL0PACKEN_F |
					     FW_IQ_CMD_FL0FETCHRO_V(relaxed) |
					     FW_IQ_CMD_FL0DATARO_V(relaxed) |
					     FW_IQ_CMD_FL0PADEN_F);
		if (cong >= 0)
			c.iqns_to_fl0congen |=
				htonl(FW_IQ_CMD_FL0CNGCHMAP_V(cong) |
				      FW_IQ_CMD_FL0CONGCIF_F |
				      FW_IQ_CMD_FL0CONGEN_F);
		/* In T6, for egress queue type FL there is internal overhead
		 * of 16B for header going into FLM module.  Hence the maximum
		 * allowed burst size is 448 bytes.  For T4/T5, the hardware
		 * doesn't coalesce fetch requests if more than 64 bytes of
		 * Free List pointers are provided, so we use a 128-byte Fetch
		 * Burst Minimum there (T6 implements coalescing so we can use
		 * the smaller 64-byte value there).
		 */
		c.fl0dcaen_to_fl0cidxfthresh =
			htons(FW_IQ_CMD_FL0FBMIN_V(chip_ver <= CHELSIO_T5 ?
						   FETCHBURSTMIN_128B_X :
						   FETCHBURSTMIN_64B_T6_X) |
			      FW_IQ_CMD_FL0FBMAX_V((chip_ver <= CHELSIO_T5) ?
						   FETCHBURSTMAX_512B_X :
						   FETCHBURSTMAX_256B_X));
		c.fl0size = htons(flsz);
		c.fl0addr = cpu_to_be64(fl->addr);
	}

	ret = t4_wr_mbox(adap, adap->mbox, &c, sizeof(c), &c);
	if (ret)
		goto err;

	netif_napi_add(dev, &iq->napi, napi_rx_handler, 64);
	iq->cur_desc = iq->desc;
	iq->cidx = 0;
	iq->gen = 1;
	iq->next_intr_params = iq->intr_params;
	iq->cntxt_id = ntohs(c.iqid);
	iq->abs_id = ntohs(c.physiqid);
	iq->bar2_addr = bar2_address(adap,
				     iq->cntxt_id,
				     T4_BAR2_QTYPE_INGRESS,
				     &iq->bar2_qid);
	iq->size--;                           /* subtract status entry */
	iq->netdev = dev;
	iq->handler = hnd;
	iq->flush_handler = flush_hnd;

	memset(&iq->lro_mgr, 0, sizeof(struct t4_lro_mgr));
	skb_queue_head_init(&iq->lro_mgr.lroq);

	/* set offset to -1 to distinguish ingress queues without FL */
	iq->offset = fl ? 0 : -1;

	adap->sge.ingr_map[iq->cntxt_id - adap->sge.ingr_start] = iq;

	if (fl) {
		fl->cntxt_id = ntohs(c.fl0id);
		fl->avail = fl->pend_cred = 0;
		fl->pidx = fl->cidx = 0;
		fl->alloc_failed = fl->large_alloc_failed = fl->starving = 0;
		adap->sge.egr_map[fl->cntxt_id - adap->sge.egr_start] = fl;

		/* Note, we must initialize the BAR2 Free List User Doorbell
		 * information before refilling the Free List!
		 */
		fl->bar2_addr = bar2_address(adap,
					     fl->cntxt_id,
					     T4_BAR2_QTYPE_EGRESS,
					     &fl->bar2_qid);
		refill_fl(adap, fl, fl_cap(fl), GFP_KERNEL);
	}

	/* For T5 and later we attempt to set up the Congestion Manager values
	 * of the new RX Ethernet Queue.  This should really be handled by
	 * firmware because it's more complex than any host driver wants to
	 * get involved with and it's different per chip and this is almost
	 * certainly wrong.  Firmware would be wrong as well, but it would be
	 * a lot easier to fix in one place ...  For now we do something very
	 * simple (and hopefully less wrong).
	 */
	if (!is_t4(adap->params.chip) && cong >= 0) {
		u32 param, val, ch_map = 0;
		int i;
		u16 cng_ch_bits_log = adap->params.arch.cng_ch_bits_log;

		param = (FW_PARAMS_MNEM_V(FW_PARAMS_MNEM_DMAQ) |
			 FW_PARAMS_PARAM_X_V(FW_PARAMS_PARAM_DMAQ_CONM_CTXT) |
			 FW_PARAMS_PARAM_YZ_V(iq->cntxt_id));
		if (cong == 0) {
			val = CONMCTXT_CNGTPMODE_V(CONMCTXT_CNGTPMODE_QUEUE_X);
		} else {
			val =
			    CONMCTXT_CNGTPMODE_V(CONMCTXT_CNGTPMODE_CHANNEL_X);
			for (i = 0; i < 4; i++) {
				if (cong & (1 << i))
					ch_map |= 1 << (i << cng_ch_bits_log);
			}
			val |= CONMCTXT_CNGCHMAP_V(ch_map);
		}
		ret = t4_set_params(adap, adap->mbox, adap->pf, 0, 1,
				    &param, &val);
		if (ret)
			dev_warn(adap->pdev_dev, "Failed to set Congestion"
				 " Manager Context for Ingress Queue %d: %d\n",
				 iq->cntxt_id, -ret);
	}

	return 0;

fl_nomem:
	ret = -ENOMEM;
err:
	if (iq->desc) {
		dma_free_coherent(adap->pdev_dev, iq->size * iq->iqe_len,
				  iq->desc, iq->phys_addr);
		iq->desc = NULL;
	}
	if (fl && fl->desc) {
		kfree(fl->sdesc);
		fl->sdesc = NULL;
		dma_free_coherent(adap->pdev_dev, flsz * sizeof(struct tx_desc),
				  fl->desc, fl->addr);
		fl->desc = NULL;
	}
	return ret;
}

static void init_txq(struct adapter *adap, struct sge_txq *q, unsigned int id)
{
	q->cntxt_id = id;
	q->bar2_addr = bar2_address(adap,
				    q->cntxt_id,
				    T4_BAR2_QTYPE_EGRESS,
				    &q->bar2_qid);
	q->in_use = 0;
	q->cidx = q->pidx = 0;
	q->stops = q->restarts = 0;
	q->stat = (void *)&q->desc[q->size];
	spin_lock_init(&q->db_lock);
	adap->sge.egr_map[id - adap->sge.egr_start] = q;
}

/**
 *	t4_sge_alloc_eth_txq - allocate an Ethernet TX Queue
 *	@adap: the adapter
 *	@txq: the SGE Ethernet TX Queue to initialize
 *	@dev: the Linux Network Device
 *	@netdevq: the corresponding Linux TX Queue
 *	@iqid: the Ingress Queue to which to deliver CIDX Update messages
 *	@dbqt: whether this TX Queue will use the SGE Doorbell Queue Timers
 */
int t4_sge_alloc_eth_txq(struct adapter *adap, struct sge_eth_txq *txq,
			 struct net_device *dev, struct netdev_queue *netdevq,
			 unsigned int iqid, u8 dbqt)
{
	unsigned int chip_ver = CHELSIO_CHIP_VERSION(adap->params.chip);
	struct port_info *pi = netdev_priv(dev);
	struct sge *s = &adap->sge;
	struct fw_eq_eth_cmd c;
	int ret, nentries;

	/* Add status entries */
	nentries = txq->q.size + s->stat_len / sizeof(struct tx_desc);

	txq->q.desc = alloc_ring(adap->pdev_dev, txq->q.size,
			sizeof(struct tx_desc), sizeof(struct tx_sw_desc),
			&txq->q.phys_addr, &txq->q.sdesc, s->stat_len,
			netdev_queue_numa_node_read(netdevq));
	if (!txq->q.desc)
		return -ENOMEM;

	memset(&c, 0, sizeof(c));
	c.op_to_vfn = htonl(FW_CMD_OP_V(FW_EQ_ETH_CMD) | FW_CMD_REQUEST_F |
			    FW_CMD_WRITE_F | FW_CMD_EXEC_F |
			    FW_EQ_ETH_CMD_PFN_V(adap->pf) |
			    FW_EQ_ETH_CMD_VFN_V(0));
	c.alloc_to_len16 = htonl(FW_EQ_ETH_CMD_ALLOC_F |
				 FW_EQ_ETH_CMD_EQSTART_F | FW_LEN16(c));

	/* For TX Ethernet Queues using the SGE Doorbell Queue Timer
	 * mechanism, we use Ingress Queue messages for Hardware Consumer
	 * Index Updates on the TX Queue.  Otherwise we have the Hardware
	 * write the CIDX Updates into the Status Page at the end of the
	 * TX Queue.
	 */
	c.autoequiqe_to_viid = htonl(((chip_ver <= CHELSIO_T5) ?
				      FW_EQ_ETH_CMD_AUTOEQUIQE_F :
				      FW_EQ_ETH_CMD_AUTOEQUEQE_F) |
				     FW_EQ_ETH_CMD_VIID_V(pi->viid));

	c.fetchszm_to_iqid =
		htonl(FW_EQ_ETH_CMD_HOSTFCMODE_V((chip_ver <= CHELSIO_T5) ?
						 HOSTFCMODE_INGRESS_QUEUE_X :
						 HOSTFCMODE_STATUS_PAGE_X) |
		      FW_EQ_ETH_CMD_PCIECHN_V(pi->tx_chan) |
		      FW_EQ_ETH_CMD_FETCHRO_F | FW_EQ_ETH_CMD_IQID_V(iqid));

	/* Note that the CIDX Flush Threshold should match MAX_TX_RECLAIM. */
	c.dcaen_to_eqsize =
		htonl(FW_EQ_ETH_CMD_FBMIN_V(chip_ver <= CHELSIO_T5
					    ? FETCHBURSTMIN_64B_X
					    : FETCHBURSTMIN_64B_T6_X) |
		      FW_EQ_ETH_CMD_FBMAX_V(FETCHBURSTMAX_512B_X) |
		      FW_EQ_ETH_CMD_CIDXFTHRESH_V(CIDXFLUSHTHRESH_32_X) |
		      FW_EQ_ETH_CMD_CIDXFTHRESHO_V(chip_ver == CHELSIO_T5) |
		      FW_EQ_ETH_CMD_EQSIZE_V(nentries));

	c.eqaddr = cpu_to_be64(txq->q.phys_addr);

	/* If we're using the SGE Doorbell Queue Timer mechanism, pass in the
	 * currently configured Timer Index.  THis can be changed later via an
	 * ethtool -C tx-usecs {Timer Val} command.  Note that the SGE
	 * Doorbell Queue mode is currently automatically enabled in the
	 * Firmware by setting either AUTOEQUEQE or AUTOEQUIQE ...
	 */
	if (dbqt)
		c.timeren_timerix =
			cpu_to_be32(FW_EQ_ETH_CMD_TIMEREN_F |
				    FW_EQ_ETH_CMD_TIMERIX_V(txq->dbqtimerix));

	ret = t4_wr_mbox(adap, adap->mbox, &c, sizeof(c), &c);
	if (ret) {
		kfree(txq->q.sdesc);
		txq->q.sdesc = NULL;
		dma_free_coherent(adap->pdev_dev,
				  nentries * sizeof(struct tx_desc),
				  txq->q.desc, txq->q.phys_addr);
		txq->q.desc = NULL;
		return ret;
	}

	txq->q.q_type = CXGB4_TXQ_ETH;
	init_txq(adap, &txq->q, FW_EQ_ETH_CMD_EQID_G(ntohl(c.eqid_pkd)));
	txq->txq = netdevq;
	txq->tso = 0;
	txq->uso = 0;
	txq->tx_cso = 0;
	txq->vlan_ins = 0;
	txq->mapping_err = 0;
	txq->dbqt = dbqt;

	return 0;
}

int t4_sge_alloc_ctrl_txq(struct adapter *adap, struct sge_ctrl_txq *txq,
			  struct net_device *dev, unsigned int iqid,
			  unsigned int cmplqid)
{
	unsigned int chip_ver = CHELSIO_CHIP_VERSION(adap->params.chip);
	struct port_info *pi = netdev_priv(dev);
	struct sge *s = &adap->sge;
	struct fw_eq_ctrl_cmd c;
	int ret, nentries;

	/* Add status entries */
	nentries = txq->q.size + s->stat_len / sizeof(struct tx_desc);

	txq->q.desc = alloc_ring(adap->pdev_dev, nentries,
				 sizeof(struct tx_desc), 0, &txq->q.phys_addr,
				 NULL, 0, dev_to_node(adap->pdev_dev));
	if (!txq->q.desc)
		return -ENOMEM;

	c.op_to_vfn = htonl(FW_CMD_OP_V(FW_EQ_CTRL_CMD) | FW_CMD_REQUEST_F |
			    FW_CMD_WRITE_F | FW_CMD_EXEC_F |
			    FW_EQ_CTRL_CMD_PFN_V(adap->pf) |
			    FW_EQ_CTRL_CMD_VFN_V(0));
	c.alloc_to_len16 = htonl(FW_EQ_CTRL_CMD_ALLOC_F |
				 FW_EQ_CTRL_CMD_EQSTART_F | FW_LEN16(c));
	c.cmpliqid_eqid = htonl(FW_EQ_CTRL_CMD_CMPLIQID_V(cmplqid));
	c.physeqid_pkd = htonl(0);
	c.fetchszm_to_iqid =
		htonl(FW_EQ_CTRL_CMD_HOSTFCMODE_V(HOSTFCMODE_STATUS_PAGE_X) |
		      FW_EQ_CTRL_CMD_PCIECHN_V(pi->tx_chan) |
		      FW_EQ_CTRL_CMD_FETCHRO_F | FW_EQ_CTRL_CMD_IQID_V(iqid));
	c.dcaen_to_eqsize =
		htonl(FW_EQ_CTRL_CMD_FBMIN_V(chip_ver <= CHELSIO_T5
					     ? FETCHBURSTMIN_64B_X
					     : FETCHBURSTMIN_64B_T6_X) |
		      FW_EQ_CTRL_CMD_FBMAX_V(FETCHBURSTMAX_512B_X) |
		      FW_EQ_CTRL_CMD_CIDXFTHRESH_V(CIDXFLUSHTHRESH_32_X) |
		      FW_EQ_CTRL_CMD_EQSIZE_V(nentries));
	c.eqaddr = cpu_to_be64(txq->q.phys_addr);

	ret = t4_wr_mbox(adap, adap->mbox, &c, sizeof(c), &c);
	if (ret) {
		dma_free_coherent(adap->pdev_dev,
				  nentries * sizeof(struct tx_desc),
				  txq->q.desc, txq->q.phys_addr);
		txq->q.desc = NULL;
		return ret;
	}

	txq->q.q_type = CXGB4_TXQ_CTRL;
	init_txq(adap, &txq->q, FW_EQ_CTRL_CMD_EQID_G(ntohl(c.cmpliqid_eqid)));
	txq->adap = adap;
	skb_queue_head_init(&txq->sendq);
	tasklet_setup(&txq->qresume_tsk, restart_ctrlq);
	txq->full = 0;
	return 0;
}

int t4_sge_mod_ctrl_txq(struct adapter *adap, unsigned int eqid,
			unsigned int cmplqid)
{
	u32 param, val;

	param = (FW_PARAMS_MNEM_V(FW_PARAMS_MNEM_DMAQ) |
		 FW_PARAMS_PARAM_X_V(FW_PARAMS_PARAM_DMAQ_EQ_CMPLIQID_CTRL) |
		 FW_PARAMS_PARAM_YZ_V(eqid));
	val = cmplqid;
	return t4_set_params(adap, adap->mbox, adap->pf, 0, 1, &param, &val);
}

static int t4_sge_alloc_ofld_txq(struct adapter *adap, struct sge_txq *q,
				 struct net_device *dev, u32 cmd, u32 iqid)
{
	unsigned int chip_ver = CHELSIO_CHIP_VERSION(adap->params.chip);
	struct port_info *pi = netdev_priv(dev);
	struct sge *s = &adap->sge;
	struct fw_eq_ofld_cmd c;
	u32 fb_min, nentries;
	int ret;

	/* Add status entries */
	nentries = q->size + s->stat_len / sizeof(struct tx_desc);
	q->desc = alloc_ring(adap->pdev_dev, q->size, sizeof(struct tx_desc),
			     sizeof(struct tx_sw_desc), &q->phys_addr,
			     &q->sdesc, s->stat_len, NUMA_NO_NODE);
	if (!q->desc)
		return -ENOMEM;

	if (chip_ver <= CHELSIO_T5)
		fb_min = FETCHBURSTMIN_64B_X;
	else
		fb_min = FETCHBURSTMIN_64B_T6_X;

	memset(&c, 0, sizeof(c));
	c.op_to_vfn = htonl(FW_CMD_OP_V(cmd) | FW_CMD_REQUEST_F |
			    FW_CMD_WRITE_F | FW_CMD_EXEC_F |
			    FW_EQ_OFLD_CMD_PFN_V(adap->pf) |
			    FW_EQ_OFLD_CMD_VFN_V(0));
	c.alloc_to_len16 = htonl(FW_EQ_OFLD_CMD_ALLOC_F |
				 FW_EQ_OFLD_CMD_EQSTART_F | FW_LEN16(c));
	c.fetchszm_to_iqid =
		htonl(FW_EQ_OFLD_CMD_HOSTFCMODE_V(HOSTFCMODE_STATUS_PAGE_X) |
		      FW_EQ_OFLD_CMD_PCIECHN_V(pi->tx_chan) |
		      FW_EQ_OFLD_CMD_FETCHRO_F | FW_EQ_OFLD_CMD_IQID_V(iqid));
	c.dcaen_to_eqsize =
		htonl(FW_EQ_OFLD_CMD_FBMIN_V(fb_min) |
		      FW_EQ_OFLD_CMD_FBMAX_V(FETCHBURSTMAX_512B_X) |
		      FW_EQ_OFLD_CMD_CIDXFTHRESH_V(CIDXFLUSHTHRESH_32_X) |
		      FW_EQ_OFLD_CMD_EQSIZE_V(nentries));
	c.eqaddr = cpu_to_be64(q->phys_addr);

	ret = t4_wr_mbox(adap, adap->mbox, &c, sizeof(c), &c);
	if (ret) {
		kfree(q->sdesc);
		q->sdesc = NULL;
		dma_free_coherent(adap->pdev_dev,
				  nentries * sizeof(struct tx_desc),
				  q->desc, q->phys_addr);
		q->desc = NULL;
		return ret;
	}

	init_txq(adap, q, FW_EQ_OFLD_CMD_EQID_G(ntohl(c.eqid_pkd)));
	return 0;
}

int t4_sge_alloc_uld_txq(struct adapter *adap, struct sge_uld_txq *txq,
			 struct net_device *dev, unsigned int iqid,
			 unsigned int uld_type)
{
	u32 cmd = FW_EQ_OFLD_CMD;
	int ret;

	if (unlikely(uld_type == CXGB4_TX_CRYPTO))
		cmd = FW_EQ_CTRL_CMD;

	ret = t4_sge_alloc_ofld_txq(adap, &txq->q, dev, cmd, iqid);
	if (ret)
		return ret;

	txq->q.q_type = CXGB4_TXQ_ULD;
	txq->adap = adap;
	skb_queue_head_init(&txq->sendq);
	tasklet_setup(&txq->qresume_tsk, restart_ofldq);
	txq->full = 0;
	txq->mapping_err = 0;
	return 0;
}

int t4_sge_alloc_ethofld_txq(struct adapter *adap, struct sge_eohw_txq *txq,
			     struct net_device *dev, u32 iqid)
{
	int ret;

	ret = t4_sge_alloc_ofld_txq(adap, &txq->q, dev, FW_EQ_OFLD_CMD, iqid);
	if (ret)
		return ret;

	txq->q.q_type = CXGB4_TXQ_ULD;
	spin_lock_init(&txq->lock);
	txq->adap = adap;
	txq->tso = 0;
	txq->uso = 0;
	txq->tx_cso = 0;
	txq->vlan_ins = 0;
	txq->mapping_err = 0;
	return 0;
}

void free_txq(struct adapter *adap, struct sge_txq *q)
{
	struct sge *s = &adap->sge;

	dma_free_coherent(adap->pdev_dev,
			  q->size * sizeof(struct tx_desc) + s->stat_len,
			  q->desc, q->phys_addr);
	q->cntxt_id = 0;
	q->sdesc = NULL;
	q->desc = NULL;
}

void free_rspq_fl(struct adapter *adap, struct sge_rspq *rq,
		  struct sge_fl *fl)
{
	struct sge *s = &adap->sge;
	unsigned int fl_id = fl ? fl->cntxt_id : 0xffff;

	adap->sge.ingr_map[rq->cntxt_id - adap->sge.ingr_start] = NULL;
	t4_iq_free(adap, adap->mbox, adap->pf, 0, FW_IQ_TYPE_FL_INT_CAP,
		   rq->cntxt_id, fl_id, 0xffff);
	dma_free_coherent(adap->pdev_dev, (rq->size + 1) * rq->iqe_len,
			  rq->desc, rq->phys_addr);
	netif_napi_del(&rq->napi);
	rq->netdev = NULL;
	rq->cntxt_id = rq->abs_id = 0;
	rq->desc = NULL;

	if (fl) {
		free_rx_bufs(adap, fl, fl->avail);
		dma_free_coherent(adap->pdev_dev, fl->size * 8 + s->stat_len,
				  fl->desc, fl->addr);
		kfree(fl->sdesc);
		fl->sdesc = NULL;
		fl->cntxt_id = 0;
		fl->desc = NULL;
	}
}

/**
 *      t4_free_ofld_rxqs - free a block of consecutive Rx queues
 *      @adap: the adapter
 *      @n: number of queues
 *      @q: pointer to first queue
 *
 *      Release the resources of a consecutive block of offload Rx queues.
 */
void t4_free_ofld_rxqs(struct adapter *adap, int n, struct sge_ofld_rxq *q)
{
	for ( ; n; n--, q++)
		if (q->rspq.desc)
			free_rspq_fl(adap, &q->rspq,
				     q->fl.size ? &q->fl : NULL);
}

void t4_sge_free_ethofld_txq(struct adapter *adap, struct sge_eohw_txq *txq)
{
	if (txq->q.desc) {
		t4_ofld_eq_free(adap, adap->mbox, adap->pf, 0,
				txq->q.cntxt_id);
		free_tx_desc(adap, &txq->q, txq->q.in_use, false);
		kfree(txq->q.sdesc);
		free_txq(adap, &txq->q);
	}
}

/**
 *	t4_free_sge_resources - free SGE resources
 *	@adap: the adapter
 *
 *	Frees resources used by the SGE queue sets.
 */
void t4_free_sge_resources(struct adapter *adap)
{
	int i;
	struct sge_eth_rxq *eq;
	struct sge_eth_txq *etq;

	/* stop all Rx queues in order to start them draining */
	for (i = 0; i < adap->sge.ethqsets; i++) {
		eq = &adap->sge.ethrxq[i];
		if (eq->rspq.desc)
			t4_iq_stop(adap, adap->mbox, adap->pf, 0,
				   FW_IQ_TYPE_FL_INT_CAP,
				   eq->rspq.cntxt_id,
				   eq->fl.size ? eq->fl.cntxt_id : 0xffff,
				   0xffff);
	}

	/* clean up Ethernet Tx/Rx queues */
	for (i = 0; i < adap->sge.ethqsets; i++) {
		eq = &adap->sge.ethrxq[i];
		if (eq->rspq.desc)
			free_rspq_fl(adap, &eq->rspq,
				     eq->fl.size ? &eq->fl : NULL);
		if (eq->msix) {
			cxgb4_free_msix_idx_in_bmap(adap, eq->msix->idx);
			eq->msix = NULL;
		}

		etq = &adap->sge.ethtxq[i];
		if (etq->q.desc) {
			t4_eth_eq_free(adap, adap->mbox, adap->pf, 0,
				       etq->q.cntxt_id);
			__netif_tx_lock_bh(etq->txq);
			free_tx_desc(adap, &etq->q, etq->q.in_use, true);
			__netif_tx_unlock_bh(etq->txq);
			kfree(etq->q.sdesc);
			free_txq(adap, &etq->q);
		}
	}

	/* clean up control Tx queues */
	for (i = 0; i < ARRAY_SIZE(adap->sge.ctrlq); i++) {
		struct sge_ctrl_txq *cq = &adap->sge.ctrlq[i];

		if (cq->q.desc) {
			tasklet_kill(&cq->qresume_tsk);
			t4_ctrl_eq_free(adap, adap->mbox, adap->pf, 0,
					cq->q.cntxt_id);
			__skb_queue_purge(&cq->sendq);
			free_txq(adap, &cq->q);
		}
	}

	if (adap->sge.fw_evtq.desc) {
		free_rspq_fl(adap, &adap->sge.fw_evtq, NULL);
		if (adap->sge.fwevtq_msix_idx >= 0)
			cxgb4_free_msix_idx_in_bmap(adap,
						    adap->sge.fwevtq_msix_idx);
	}

	if (adap->sge.nd_msix_idx >= 0)
		cxgb4_free_msix_idx_in_bmap(adap, adap->sge.nd_msix_idx);

	if (adap->sge.intrq.desc)
		free_rspq_fl(adap, &adap->sge.intrq, NULL);

	if (!is_t4(adap->params.chip)) {
		etq = &adap->sge.ptptxq;
		if (etq->q.desc) {
			t4_eth_eq_free(adap, adap->mbox, adap->pf, 0,
				       etq->q.cntxt_id);
			spin_lock_bh(&adap->ptp_lock);
			free_tx_desc(adap, &etq->q, etq->q.in_use, true);
			spin_unlock_bh(&adap->ptp_lock);
			kfree(etq->q.sdesc);
			free_txq(adap, &etq->q);
		}
	}

	/* clear the reverse egress queue map */
	memset(adap->sge.egr_map, 0,
	       adap->sge.egr_sz * sizeof(*adap->sge.egr_map));
}

void t4_sge_start(struct adapter *adap)
{
	adap->sge.ethtxq_rover = 0;
	mod_timer(&adap->sge.rx_timer, jiffies + RX_QCHECK_PERIOD);
	mod_timer(&adap->sge.tx_timer, jiffies + TX_QCHECK_PERIOD);
}

/**
 *	t4_sge_stop - disable SGE operation
 *	@adap: the adapter
 *
 *	Stop tasklets and timers associated with the DMA engine.  Note that
 *	this is effective only if measures have been taken to disable any HW
 *	events that may restart them.
 */
void t4_sge_stop(struct adapter *adap)
{
	int i;
	struct sge *s = &adap->sge;

	if (s->rx_timer.function)
		del_timer_sync(&s->rx_timer);
	if (s->tx_timer.function)
		del_timer_sync(&s->tx_timer);

	if (is_offload(adap)) {
		struct sge_uld_txq_info *txq_info;

		txq_info = adap->sge.uld_txq_info[CXGB4_TX_OFLD];
		if (txq_info) {
			struct sge_uld_txq *txq = txq_info->uldtxq;

			for_each_ofldtxq(&adap->sge, i) {
				if (txq->q.desc)
					tasklet_kill(&txq->qresume_tsk);
			}
		}
	}

	if (is_pci_uld(adap)) {
		struct sge_uld_txq_info *txq_info;

		txq_info = adap->sge.uld_txq_info[CXGB4_TX_CRYPTO];
		if (txq_info) {
			struct sge_uld_txq *txq = txq_info->uldtxq;

			for_each_ofldtxq(&adap->sge, i) {
				if (txq->q.desc)
					tasklet_kill(&txq->qresume_tsk);
			}
		}
	}

	for (i = 0; i < ARRAY_SIZE(s->ctrlq); i++) {
		struct sge_ctrl_txq *cq = &s->ctrlq[i];

		if (cq->q.desc)
			tasklet_kill(&cq->qresume_tsk);
	}
}

/**
 *	t4_sge_init_soft - grab core SGE values needed by SGE code
 *	@adap: the adapter
 *
 *	We need to grab the SGE operating parameters that we need to have
 *	in order to do our job and make sure we can live with them.
 */

static int t4_sge_init_soft(struct adapter *adap)
{
	struct sge *s = &adap->sge;
	u32 fl_small_pg, fl_large_pg, fl_small_mtu, fl_large_mtu;
	u32 timer_value_0_and_1, timer_value_2_and_3, timer_value_4_and_5;
	u32 ingress_rx_threshold;

	/*
	 * Verify that CPL messages are going to the Ingress Queue for
	 * process_responses() and that only packet data is going to the
	 * Free Lists.
	 */
	if ((t4_read_reg(adap, SGE_CONTROL_A) & RXPKTCPLMODE_F) !=
	    RXPKTCPLMODE_V(RXPKTCPLMODE_SPLIT_X)) {
		dev_err(adap->pdev_dev, "bad SGE CPL MODE\n");
		return -EINVAL;
	}

	/*
	 * Validate the Host Buffer Register Array indices that we want to
	 * use ...
	 *
	 * XXX Note that we should really read through the Host Buffer Size
	 * XXX register array and find the indices of the Buffer Sizes which
	 * XXX meet our needs!
	 */
	#define READ_FL_BUF(x) \
		t4_read_reg(adap, SGE_FL_BUFFER_SIZE0_A+(x)*sizeof(u32))

	fl_small_pg = READ_FL_BUF(RX_SMALL_PG_BUF);
	fl_large_pg = READ_FL_BUF(RX_LARGE_PG_BUF);
	fl_small_mtu = READ_FL_BUF(RX_SMALL_MTU_BUF);
	fl_large_mtu = READ_FL_BUF(RX_LARGE_MTU_BUF);

	/* We only bother using the Large Page logic if the Large Page Buffer
	 * is larger than our Page Size Buffer.
	 */
	if (fl_large_pg <= fl_small_pg)
		fl_large_pg = 0;

	#undef READ_FL_BUF

	/* The Page Size Buffer must be exactly equal to our Page Size and the
	 * Large Page Size Buffer should be 0 (per above) or a power of 2.
	 */
	if (fl_small_pg != PAGE_SIZE ||
	    (fl_large_pg & (fl_large_pg-1)) != 0) {
		dev_err(adap->pdev_dev, "bad SGE FL page buffer sizes [%d, %d]\n",
			fl_small_pg, fl_large_pg);
		return -EINVAL;
	}
	if (fl_large_pg)
		s->fl_pg_order = ilog2(fl_large_pg) - PAGE_SHIFT;

	if (fl_small_mtu < FL_MTU_SMALL_BUFSIZE(adap) ||
	    fl_large_mtu < FL_MTU_LARGE_BUFSIZE(adap)) {
		dev_err(adap->pdev_dev, "bad SGE FL MTU sizes [%d, %d]\n",
			fl_small_mtu, fl_large_mtu);
		return -EINVAL;
	}

	/*
	 * Retrieve our RX interrupt holdoff timer values and counter
	 * threshold values from the SGE parameters.
	 */
	timer_value_0_and_1 = t4_read_reg(adap, SGE_TIMER_VALUE_0_AND_1_A);
	timer_value_2_and_3 = t4_read_reg(adap, SGE_TIMER_VALUE_2_AND_3_A);
	timer_value_4_and_5 = t4_read_reg(adap, SGE_TIMER_VALUE_4_AND_5_A);
	s->timer_val[0] = core_ticks_to_us(adap,
		TIMERVALUE0_G(timer_value_0_and_1));
	s->timer_val[1] = core_ticks_to_us(adap,
		TIMERVALUE1_G(timer_value_0_and_1));
	s->timer_val[2] = core_ticks_to_us(adap,
		TIMERVALUE2_G(timer_value_2_and_3));
	s->timer_val[3] = core_ticks_to_us(adap,
		TIMERVALUE3_G(timer_value_2_and_3));
	s->timer_val[4] = core_ticks_to_us(adap,
		TIMERVALUE4_G(timer_value_4_and_5));
	s->timer_val[5] = core_ticks_to_us(adap,
		TIMERVALUE5_G(timer_value_4_and_5));

	ingress_rx_threshold = t4_read_reg(adap, SGE_INGRESS_RX_THRESHOLD_A);
	s->counter_val[0] = THRESHOLD_0_G(ingress_rx_threshold);
	s->counter_val[1] = THRESHOLD_1_G(ingress_rx_threshold);
	s->counter_val[2] = THRESHOLD_2_G(ingress_rx_threshold);
	s->counter_val[3] = THRESHOLD_3_G(ingress_rx_threshold);

	return 0;
}

/**
 *     t4_sge_init - initialize SGE
 *     @adap: the adapter
 *
 *     Perform low-level SGE code initialization needed every time after a
 *     chip reset.
 */
int t4_sge_init(struct adapter *adap)
{
	struct sge *s = &adap->sge;
	u32 sge_control, sge_conm_ctrl;
	int ret, egress_threshold;

	/*
	 * Ingress Padding Boundary and Egress Status Page Size are set up by
	 * t4_fixup_host_params().
	 */
	sge_control = t4_read_reg(adap, SGE_CONTROL_A);
	s->pktshift = PKTSHIFT_G(sge_control);
	s->stat_len = (sge_control & EGRSTATUSPAGESIZE_F) ? 128 : 64;

	s->fl_align = t4_fl_pkt_align(adap);
	ret = t4_sge_init_soft(adap);
	if (ret < 0)
		return ret;

	/*
	 * A FL with <= fl_starve_thres buffers is starving and a periodic
	 * timer will attempt to refill it.  This needs to be larger than the
	 * SGE's Egress Congestion Threshold.  If it isn't, then we can get
	 * stuck waiting for new packets while the SGE is waiting for us to
	 * give it more Free List entries.  (Note that the SGE's Egress
	 * Congestion Threshold is in units of 2 Free List pointers.) For T4,
	 * there was only a single field to control this.  For T5 there's the
	 * original field which now only applies to Unpacked Mode Free List
	 * buffers and a new field which only applies to Packed Mode Free List
	 * buffers.
	 */
	sge_conm_ctrl = t4_read_reg(adap, SGE_CONM_CTRL_A);
	switch (CHELSIO_CHIP_VERSION(adap->params.chip)) {
	case CHELSIO_T4:
		egress_threshold = EGRTHRESHOLD_G(sge_conm_ctrl);
		break;
	case CHELSIO_T5:
		egress_threshold = EGRTHRESHOLDPACKING_G(sge_conm_ctrl);
		break;
	case CHELSIO_T6:
		egress_threshold = T6_EGRTHRESHOLDPACKING_G(sge_conm_ctrl);
		break;
	default:
		dev_err(adap->pdev_dev, "Unsupported Chip version %d\n",
			CHELSIO_CHIP_VERSION(adap->params.chip));
		return -EINVAL;
	}
	s->fl_starve_thres = 2*egress_threshold + 1;

	t4_idma_monitor_init(adap, &s->idma_monitor);

	/* Set up timers used for recuring callbacks to process RX and TX
	 * administrative tasks.
	 */
	timer_setup(&s->rx_timer, sge_rx_timer_cb, 0);
	timer_setup(&s->tx_timer, sge_tx_timer_cb, 0);

	spin_lock_init(&s->intrq_lock);

	return 0;
}
