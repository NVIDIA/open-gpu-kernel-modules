// SPDX-License-Identifier: (GPL-2.0+ OR BSD-3-Clause)
/* Copyright 2017-2019 NXP */

#include "enetc.h"
#include <linux/bpf_trace.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/vmalloc.h>
#include <linux/ptp_classify.h>
#include <net/pkt_sched.h>

static int enetc_num_stack_tx_queues(struct enetc_ndev_priv *priv)
{
	int num_tx_rings = priv->num_tx_rings;
	int i;

	for (i = 0; i < priv->num_rx_rings; i++)
		if (priv->rx_ring[i]->xdp.prog)
			return num_tx_rings - num_possible_cpus();

	return num_tx_rings;
}

static struct enetc_bdr *enetc_rx_ring_from_xdp_tx_ring(struct enetc_ndev_priv *priv,
							struct enetc_bdr *tx_ring)
{
	int index = &priv->tx_ring[tx_ring->index] - priv->xdp_tx_ring;

	return priv->rx_ring[index];
}

static struct sk_buff *enetc_tx_swbd_get_skb(struct enetc_tx_swbd *tx_swbd)
{
	if (tx_swbd->is_xdp_tx || tx_swbd->is_xdp_redirect)
		return NULL;

	return tx_swbd->skb;
}

static struct xdp_frame *
enetc_tx_swbd_get_xdp_frame(struct enetc_tx_swbd *tx_swbd)
{
	if (tx_swbd->is_xdp_redirect)
		return tx_swbd->xdp_frame;

	return NULL;
}

static void enetc_unmap_tx_buff(struct enetc_bdr *tx_ring,
				struct enetc_tx_swbd *tx_swbd)
{
	/* For XDP_TX, pages come from RX, whereas for the other contexts where
	 * we have is_dma_page_set, those come from skb_frag_dma_map. We need
	 * to match the DMA mapping length, so we need to differentiate those.
	 */
	if (tx_swbd->is_dma_page)
		dma_unmap_page(tx_ring->dev, tx_swbd->dma,
			       tx_swbd->is_xdp_tx ? PAGE_SIZE : tx_swbd->len,
			       tx_swbd->dir);
	else
		dma_unmap_single(tx_ring->dev, tx_swbd->dma,
				 tx_swbd->len, tx_swbd->dir);
	tx_swbd->dma = 0;
}

static void enetc_free_tx_frame(struct enetc_bdr *tx_ring,
				struct enetc_tx_swbd *tx_swbd)
{
	struct xdp_frame *xdp_frame = enetc_tx_swbd_get_xdp_frame(tx_swbd);
	struct sk_buff *skb = enetc_tx_swbd_get_skb(tx_swbd);

	if (tx_swbd->dma)
		enetc_unmap_tx_buff(tx_ring, tx_swbd);

	if (xdp_frame) {
		xdp_return_frame(tx_swbd->xdp_frame);
		tx_swbd->xdp_frame = NULL;
	} else if (skb) {
		dev_kfree_skb_any(skb);
		tx_swbd->skb = NULL;
	}
}

/* Let H/W know BD ring has been updated */
static void enetc_update_tx_ring_tail(struct enetc_bdr *tx_ring)
{
	/* includes wmb() */
	enetc_wr_reg_hot(tx_ring->tpir, tx_ring->next_to_use);
}

static int enetc_ptp_parse(struct sk_buff *skb, u8 *udp,
			   u8 *msgtype, u8 *twostep,
			   u16 *correction_offset, u16 *body_offset)
{
	unsigned int ptp_class;
	struct ptp_header *hdr;
	unsigned int type;
	u8 *base;

	ptp_class = ptp_classify_raw(skb);
	if (ptp_class == PTP_CLASS_NONE)
		return -EINVAL;

	hdr = ptp_parse_header(skb, ptp_class);
	if (!hdr)
		return -EINVAL;

	type = ptp_class & PTP_CLASS_PMASK;
	if (type == PTP_CLASS_IPV4 || type == PTP_CLASS_IPV6)
		*udp = 1;
	else
		*udp = 0;

	*msgtype = ptp_get_msgtype(hdr, ptp_class);
	*twostep = hdr->flag_field[0] & 0x2;

	base = skb_mac_header(skb);
	*correction_offset = (u8 *)&hdr->correction - base;
	*body_offset = (u8 *)hdr + sizeof(struct ptp_header) - base;

	return 0;
}

static int enetc_map_tx_buffs(struct enetc_bdr *tx_ring, struct sk_buff *skb)
{
	bool do_vlan, do_onestep_tstamp = false, do_twostep_tstamp = false;
	struct enetc_ndev_priv *priv = netdev_priv(tx_ring->ndev);
	struct enetc_hw *hw = &priv->si->hw;
	struct enetc_tx_swbd *tx_swbd;
	int len = skb_headlen(skb);
	union enetc_tx_bd temp_bd;
	u8 msgtype, twostep, udp;
	union enetc_tx_bd *txbd;
	u16 offset1, offset2;
	int i, count = 0;
	skb_frag_t *frag;
	unsigned int f;
	dma_addr_t dma;
	u8 flags = 0;

	i = tx_ring->next_to_use;
	txbd = ENETC_TXBD(*tx_ring, i);
	prefetchw(txbd);

	dma = dma_map_single(tx_ring->dev, skb->data, len, DMA_TO_DEVICE);
	if (unlikely(dma_mapping_error(tx_ring->dev, dma)))
		goto dma_err;

	temp_bd.addr = cpu_to_le64(dma);
	temp_bd.buf_len = cpu_to_le16(len);
	temp_bd.lstatus = 0;

	tx_swbd = &tx_ring->tx_swbd[i];
	tx_swbd->dma = dma;
	tx_swbd->len = len;
	tx_swbd->is_dma_page = 0;
	tx_swbd->dir = DMA_TO_DEVICE;
	count++;

	do_vlan = skb_vlan_tag_present(skb);
	if (skb->cb[0] & ENETC_F_TX_ONESTEP_SYNC_TSTAMP) {
		if (enetc_ptp_parse(skb, &udp, &msgtype, &twostep, &offset1,
				    &offset2) ||
		    msgtype != PTP_MSGTYPE_SYNC || twostep)
			WARN_ONCE(1, "Bad packet for one-step timestamping\n");
		else
			do_onestep_tstamp = true;
	} else if (skb->cb[0] & ENETC_F_TX_TSTAMP) {
		do_twostep_tstamp = true;
	}

	tx_swbd->do_twostep_tstamp = do_twostep_tstamp;
	tx_swbd->check_wb = tx_swbd->do_twostep_tstamp;

	if (do_vlan || do_onestep_tstamp || do_twostep_tstamp)
		flags |= ENETC_TXBD_FLAGS_EX;

	if (tx_ring->tsd_enable)
		flags |= ENETC_TXBD_FLAGS_TSE | ENETC_TXBD_FLAGS_TXSTART;

	/* first BD needs frm_len and offload flags set */
	temp_bd.frm_len = cpu_to_le16(skb->len);
	temp_bd.flags = flags;

	if (flags & ENETC_TXBD_FLAGS_TSE)
		temp_bd.txstart = enetc_txbd_set_tx_start(skb->skb_mstamp_ns,
							  flags);

	if (flags & ENETC_TXBD_FLAGS_EX) {
		u8 e_flags = 0;
		*txbd = temp_bd;
		enetc_clear_tx_bd(&temp_bd);

		/* add extension BD for VLAN and/or timestamping */
		flags = 0;
		tx_swbd++;
		txbd++;
		i++;
		if (unlikely(i == tx_ring->bd_count)) {
			i = 0;
			tx_swbd = tx_ring->tx_swbd;
			txbd = ENETC_TXBD(*tx_ring, 0);
		}
		prefetchw(txbd);

		if (do_vlan) {
			temp_bd.ext.vid = cpu_to_le16(skb_vlan_tag_get(skb));
			temp_bd.ext.tpid = 0; /* < C-TAG */
			e_flags |= ENETC_TXBD_E_FLAGS_VLAN_INS;
		}

		if (do_onestep_tstamp) {
			u32 lo, hi, val;
			u64 sec, nsec;
			u8 *data;

			lo = enetc_rd_hot(hw, ENETC_SICTR0);
			hi = enetc_rd_hot(hw, ENETC_SICTR1);
			sec = (u64)hi << 32 | lo;
			nsec = do_div(sec, 1000000000);

			/* Configure extension BD */
			temp_bd.ext.tstamp = cpu_to_le32(lo & 0x3fffffff);
			e_flags |= ENETC_TXBD_E_FLAGS_ONE_STEP_PTP;

			/* Update originTimestamp field of Sync packet
			 * - 48 bits seconds field
			 * - 32 bits nanseconds field
			 */
			data = skb_mac_header(skb);
			*(__be16 *)(data + offset2) =
				htons((sec >> 32) & 0xffff);
			*(__be32 *)(data + offset2 + 2) =
				htonl(sec & 0xffffffff);
			*(__be32 *)(data + offset2 + 6) = htonl(nsec);

			/* Configure single-step register */
			val = ENETC_PM0_SINGLE_STEP_EN;
			val |= ENETC_SET_SINGLE_STEP_OFFSET(offset1);
			if (udp)
				val |= ENETC_PM0_SINGLE_STEP_CH;

			enetc_port_wr(hw, ENETC_PM0_SINGLE_STEP, val);
			enetc_port_wr(hw, ENETC_PM1_SINGLE_STEP, val);
		} else if (do_twostep_tstamp) {
			skb_shinfo(skb)->tx_flags |= SKBTX_IN_PROGRESS;
			e_flags |= ENETC_TXBD_E_FLAGS_TWO_STEP_PTP;
		}

		temp_bd.ext.e_flags = e_flags;
		count++;
	}

	frag = &skb_shinfo(skb)->frags[0];
	for (f = 0; f < skb_shinfo(skb)->nr_frags; f++, frag++) {
		len = skb_frag_size(frag);
		dma = skb_frag_dma_map(tx_ring->dev, frag, 0, len,
				       DMA_TO_DEVICE);
		if (dma_mapping_error(tx_ring->dev, dma))
			goto dma_err;

		*txbd = temp_bd;
		enetc_clear_tx_bd(&temp_bd);

		flags = 0;
		tx_swbd++;
		txbd++;
		i++;
		if (unlikely(i == tx_ring->bd_count)) {
			i = 0;
			tx_swbd = tx_ring->tx_swbd;
			txbd = ENETC_TXBD(*tx_ring, 0);
		}
		prefetchw(txbd);

		temp_bd.addr = cpu_to_le64(dma);
		temp_bd.buf_len = cpu_to_le16(len);

		tx_swbd->dma = dma;
		tx_swbd->len = len;
		tx_swbd->is_dma_page = 1;
		tx_swbd->dir = DMA_TO_DEVICE;
		count++;
	}

	/* last BD needs 'F' bit set */
	flags |= ENETC_TXBD_FLAGS_F;
	temp_bd.flags = flags;
	*txbd = temp_bd;

	tx_ring->tx_swbd[i].is_eof = true;
	tx_ring->tx_swbd[i].skb = skb;

	enetc_bdr_idx_inc(tx_ring, &i);
	tx_ring->next_to_use = i;

	skb_tx_timestamp(skb);

	enetc_update_tx_ring_tail(tx_ring);

	return count;

dma_err:
	dev_err(tx_ring->dev, "DMA map error");

	do {
		tx_swbd = &tx_ring->tx_swbd[i];
		enetc_free_tx_frame(tx_ring, tx_swbd);
		if (i == 0)
			i = tx_ring->bd_count;
		i--;
	} while (count--);

	return 0;
}

static netdev_tx_t enetc_start_xmit(struct sk_buff *skb,
				    struct net_device *ndev)
{
	struct enetc_ndev_priv *priv = netdev_priv(ndev);
	struct enetc_bdr *tx_ring;
	int count;

	/* Queue one-step Sync packet if already locked */
	if (skb->cb[0] & ENETC_F_TX_ONESTEP_SYNC_TSTAMP) {
		if (test_and_set_bit_lock(ENETC_TX_ONESTEP_TSTAMP_IN_PROGRESS,
					  &priv->flags)) {
			skb_queue_tail(&priv->tx_skbs, skb);
			return NETDEV_TX_OK;
		}
	}

	tx_ring = priv->tx_ring[skb->queue_mapping];

	if (unlikely(skb_shinfo(skb)->nr_frags > ENETC_MAX_SKB_FRAGS))
		if (unlikely(skb_linearize(skb)))
			goto drop_packet_err;

	count = skb_shinfo(skb)->nr_frags + 1; /* fragments + head */
	if (enetc_bd_unused(tx_ring) < ENETC_TXBDS_NEEDED(count)) {
		netif_stop_subqueue(ndev, tx_ring->index);
		return NETDEV_TX_BUSY;
	}

	enetc_lock_mdio();
	count = enetc_map_tx_buffs(tx_ring, skb);
	enetc_unlock_mdio();

	if (unlikely(!count))
		goto drop_packet_err;

	if (enetc_bd_unused(tx_ring) < ENETC_TXBDS_MAX_NEEDED)
		netif_stop_subqueue(ndev, tx_ring->index);

	return NETDEV_TX_OK;

drop_packet_err:
	dev_kfree_skb_any(skb);
	return NETDEV_TX_OK;
}

netdev_tx_t enetc_xmit(struct sk_buff *skb, struct net_device *ndev)
{
	struct enetc_ndev_priv *priv = netdev_priv(ndev);
	u8 udp, msgtype, twostep;
	u16 offset1, offset2;

	/* Mark tx timestamp type on skb->cb[0] if requires */
	if ((skb_shinfo(skb)->tx_flags & SKBTX_HW_TSTAMP) &&
	    (priv->active_offloads & ENETC_F_TX_TSTAMP_MASK)) {
		skb->cb[0] = priv->active_offloads & ENETC_F_TX_TSTAMP_MASK;
	} else {
		skb->cb[0] = 0;
	}

	/* Fall back to two-step timestamp if not one-step Sync packet */
	if (skb->cb[0] & ENETC_F_TX_ONESTEP_SYNC_TSTAMP) {
		if (enetc_ptp_parse(skb, &udp, &msgtype, &twostep,
				    &offset1, &offset2) ||
		    msgtype != PTP_MSGTYPE_SYNC || twostep != 0)
			skb->cb[0] = ENETC_F_TX_TSTAMP;
	}

	return enetc_start_xmit(skb, ndev);
}

static irqreturn_t enetc_msix(int irq, void *data)
{
	struct enetc_int_vector	*v = data;
	int i;

	enetc_lock_mdio();

	/* disable interrupts */
	enetc_wr_reg_hot(v->rbier, 0);
	enetc_wr_reg_hot(v->ricr1, v->rx_ictt);

	for_each_set_bit(i, &v->tx_rings_map, ENETC_MAX_NUM_TXQS)
		enetc_wr_reg_hot(v->tbier_base + ENETC_BDR_OFF(i), 0);

	enetc_unlock_mdio();

	napi_schedule(&v->napi);

	return IRQ_HANDLED;
}

static void enetc_rx_dim_work(struct work_struct *w)
{
	struct dim *dim = container_of(w, struct dim, work);
	struct dim_cq_moder moder =
		net_dim_get_rx_moderation(dim->mode, dim->profile_ix);
	struct enetc_int_vector	*v =
		container_of(dim, struct enetc_int_vector, rx_dim);

	v->rx_ictt = enetc_usecs_to_cycles(moder.usec);
	dim->state = DIM_START_MEASURE;
}

static void enetc_rx_net_dim(struct enetc_int_vector *v)
{
	struct dim_sample dim_sample;

	v->comp_cnt++;

	if (!v->rx_napi_work)
		return;

	dim_update_sample(v->comp_cnt,
			  v->rx_ring.stats.packets,
			  v->rx_ring.stats.bytes,
			  &dim_sample);
	net_dim(&v->rx_dim, dim_sample);
}

static int enetc_bd_ready_count(struct enetc_bdr *tx_ring, int ci)
{
	int pi = enetc_rd_reg_hot(tx_ring->tcir) & ENETC_TBCIR_IDX_MASK;

	return pi >= ci ? pi - ci : tx_ring->bd_count - ci + pi;
}

static bool enetc_page_reusable(struct page *page)
{
	return (!page_is_pfmemalloc(page) && page_ref_count(page) == 1);
}

static void enetc_reuse_page(struct enetc_bdr *rx_ring,
			     struct enetc_rx_swbd *old)
{
	struct enetc_rx_swbd *new;

	new = &rx_ring->rx_swbd[rx_ring->next_to_alloc];

	/* next buf that may reuse a page */
	enetc_bdr_idx_inc(rx_ring, &rx_ring->next_to_alloc);

	/* copy page reference */
	*new = *old;
}

static void enetc_get_tx_tstamp(struct enetc_hw *hw, union enetc_tx_bd *txbd,
				u64 *tstamp)
{
	u32 lo, hi, tstamp_lo;

	lo = enetc_rd_hot(hw, ENETC_SICTR0);
	hi = enetc_rd_hot(hw, ENETC_SICTR1);
	tstamp_lo = le32_to_cpu(txbd->wb.tstamp);
	if (lo <= tstamp_lo)
		hi -= 1;
	*tstamp = (u64)hi << 32 | tstamp_lo;
}

static void enetc_tstamp_tx(struct sk_buff *skb, u64 tstamp)
{
	struct skb_shared_hwtstamps shhwtstamps;

	if (skb_shinfo(skb)->tx_flags & SKBTX_IN_PROGRESS) {
		memset(&shhwtstamps, 0, sizeof(shhwtstamps));
		shhwtstamps.hwtstamp = ns_to_ktime(tstamp);
		skb_txtime_consumed(skb);
		skb_tstamp_tx(skb, &shhwtstamps);
	}
}

static void enetc_recycle_xdp_tx_buff(struct enetc_bdr *tx_ring,
				      struct enetc_tx_swbd *tx_swbd)
{
	struct enetc_ndev_priv *priv = netdev_priv(tx_ring->ndev);
	struct enetc_rx_swbd rx_swbd = {
		.dma = tx_swbd->dma,
		.page = tx_swbd->page,
		.page_offset = tx_swbd->page_offset,
		.dir = tx_swbd->dir,
		.len = tx_swbd->len,
	};
	struct enetc_bdr *rx_ring;

	rx_ring = enetc_rx_ring_from_xdp_tx_ring(priv, tx_ring);

	if (likely(enetc_swbd_unused(rx_ring))) {
		enetc_reuse_page(rx_ring, &rx_swbd);

		/* sync for use by the device */
		dma_sync_single_range_for_device(rx_ring->dev, rx_swbd.dma,
						 rx_swbd.page_offset,
						 ENETC_RXB_DMA_SIZE_XDP,
						 rx_swbd.dir);

		rx_ring->stats.recycles++;
	} else {
		/* RX ring is already full, we need to unmap and free the
		 * page, since there's nothing useful we can do with it.
		 */
		rx_ring->stats.recycle_failures++;

		dma_unmap_page(rx_ring->dev, rx_swbd.dma, PAGE_SIZE,
			       rx_swbd.dir);
		__free_page(rx_swbd.page);
	}

	rx_ring->xdp.xdp_tx_in_flight--;
}

static bool enetc_clean_tx_ring(struct enetc_bdr *tx_ring, int napi_budget)
{
	struct net_device *ndev = tx_ring->ndev;
	struct enetc_ndev_priv *priv = netdev_priv(ndev);
	int tx_frm_cnt = 0, tx_byte_cnt = 0;
	struct enetc_tx_swbd *tx_swbd;
	int i, bds_to_clean;
	bool do_twostep_tstamp;
	u64 tstamp = 0;

	i = tx_ring->next_to_clean;
	tx_swbd = &tx_ring->tx_swbd[i];

	bds_to_clean = enetc_bd_ready_count(tx_ring, i);

	do_twostep_tstamp = false;

	while (bds_to_clean && tx_frm_cnt < ENETC_DEFAULT_TX_WORK) {
		struct xdp_frame *xdp_frame = enetc_tx_swbd_get_xdp_frame(tx_swbd);
		struct sk_buff *skb = enetc_tx_swbd_get_skb(tx_swbd);
		bool is_eof = tx_swbd->is_eof;

		if (unlikely(tx_swbd->check_wb)) {
			struct enetc_ndev_priv *priv = netdev_priv(ndev);
			union enetc_tx_bd *txbd;

			txbd = ENETC_TXBD(*tx_ring, i);

			if (txbd->flags & ENETC_TXBD_FLAGS_W &&
			    tx_swbd->do_twostep_tstamp) {
				enetc_get_tx_tstamp(&priv->si->hw, txbd,
						    &tstamp);
				do_twostep_tstamp = true;
			}
		}

		if (tx_swbd->is_xdp_tx)
			enetc_recycle_xdp_tx_buff(tx_ring, tx_swbd);
		else if (likely(tx_swbd->dma))
			enetc_unmap_tx_buff(tx_ring, tx_swbd);

		if (xdp_frame) {
			xdp_return_frame(xdp_frame);
		} else if (skb) {
			if (unlikely(tx_swbd->skb->cb[0] &
				     ENETC_F_TX_ONESTEP_SYNC_TSTAMP)) {
				/* Start work to release lock for next one-step
				 * timestamping packet. And send one skb in
				 * tx_skbs queue if has.
				 */
				schedule_work(&priv->tx_onestep_tstamp);
			} else if (unlikely(do_twostep_tstamp)) {
				enetc_tstamp_tx(skb, tstamp);
				do_twostep_tstamp = false;
			}
			napi_consume_skb(skb, napi_budget);
		}

		tx_byte_cnt += tx_swbd->len;
		/* Scrub the swbd here so we don't have to do that
		 * when we reuse it during xmit
		 */
		memset(tx_swbd, 0, sizeof(*tx_swbd));

		bds_to_clean--;
		tx_swbd++;
		i++;
		if (unlikely(i == tx_ring->bd_count)) {
			i = 0;
			tx_swbd = tx_ring->tx_swbd;
		}

		/* BD iteration loop end */
		if (is_eof) {
			tx_frm_cnt++;
			/* re-arm interrupt source */
			enetc_wr_reg_hot(tx_ring->idr, BIT(tx_ring->index) |
					 BIT(16 + tx_ring->index));
		}

		if (unlikely(!bds_to_clean))
			bds_to_clean = enetc_bd_ready_count(tx_ring, i);
	}

	tx_ring->next_to_clean = i;
	tx_ring->stats.packets += tx_frm_cnt;
	tx_ring->stats.bytes += tx_byte_cnt;

	if (unlikely(tx_frm_cnt && netif_carrier_ok(ndev) &&
		     __netif_subqueue_stopped(ndev, tx_ring->index) &&
		     (enetc_bd_unused(tx_ring) >= ENETC_TXBDS_MAX_NEEDED))) {
		netif_wake_subqueue(ndev, tx_ring->index);
	}

	return tx_frm_cnt != ENETC_DEFAULT_TX_WORK;
}

static bool enetc_new_page(struct enetc_bdr *rx_ring,
			   struct enetc_rx_swbd *rx_swbd)
{
	bool xdp = !!(rx_ring->xdp.prog);
	struct page *page;
	dma_addr_t addr;

	page = dev_alloc_page();
	if (unlikely(!page))
		return false;

	/* For XDP_TX, we forgo dma_unmap -> dma_map */
	rx_swbd->dir = xdp ? DMA_BIDIRECTIONAL : DMA_FROM_DEVICE;

	addr = dma_map_page(rx_ring->dev, page, 0, PAGE_SIZE, rx_swbd->dir);
	if (unlikely(dma_mapping_error(rx_ring->dev, addr))) {
		__free_page(page);

		return false;
	}

	rx_swbd->dma = addr;
	rx_swbd->page = page;
	rx_swbd->page_offset = rx_ring->buffer_offset;

	return true;
}

static int enetc_refill_rx_ring(struct enetc_bdr *rx_ring, const int buff_cnt)
{
	struct enetc_rx_swbd *rx_swbd;
	union enetc_rx_bd *rxbd;
	int i, j;

	i = rx_ring->next_to_use;
	rx_swbd = &rx_ring->rx_swbd[i];
	rxbd = enetc_rxbd(rx_ring, i);

	for (j = 0; j < buff_cnt; j++) {
		/* try reuse page */
		if (unlikely(!rx_swbd->page)) {
			if (unlikely(!enetc_new_page(rx_ring, rx_swbd))) {
				rx_ring->stats.rx_alloc_errs++;
				break;
			}
		}

		/* update RxBD */
		rxbd->w.addr = cpu_to_le64(rx_swbd->dma +
					   rx_swbd->page_offset);
		/* clear 'R" as well */
		rxbd->r.lstatus = 0;

		enetc_rxbd_next(rx_ring, &rxbd, &i);
		rx_swbd = &rx_ring->rx_swbd[i];
	}

	if (likely(j)) {
		rx_ring->next_to_alloc = i; /* keep track from page reuse */
		rx_ring->next_to_use = i;

		/* update ENETC's consumer index */
		enetc_wr_reg_hot(rx_ring->rcir, rx_ring->next_to_use);
	}

	return j;
}

#ifdef CONFIG_FSL_ENETC_PTP_CLOCK
static void enetc_get_rx_tstamp(struct net_device *ndev,
				union enetc_rx_bd *rxbd,
				struct sk_buff *skb)
{
	struct skb_shared_hwtstamps *shhwtstamps = skb_hwtstamps(skb);
	struct enetc_ndev_priv *priv = netdev_priv(ndev);
	struct enetc_hw *hw = &priv->si->hw;
	u32 lo, hi, tstamp_lo;
	u64 tstamp;

	if (le16_to_cpu(rxbd->r.flags) & ENETC_RXBD_FLAG_TSTMP) {
		lo = enetc_rd_reg_hot(hw->reg + ENETC_SICTR0);
		hi = enetc_rd_reg_hot(hw->reg + ENETC_SICTR1);
		rxbd = enetc_rxbd_ext(rxbd);
		tstamp_lo = le32_to_cpu(rxbd->ext.tstamp);
		if (lo <= tstamp_lo)
			hi -= 1;

		tstamp = (u64)hi << 32 | tstamp_lo;
		memset(shhwtstamps, 0, sizeof(*shhwtstamps));
		shhwtstamps->hwtstamp = ns_to_ktime(tstamp);
	}
}
#endif

static void enetc_get_offloads(struct enetc_bdr *rx_ring,
			       union enetc_rx_bd *rxbd, struct sk_buff *skb)
{
	struct enetc_ndev_priv *priv = netdev_priv(rx_ring->ndev);

	/* TODO: hashing */
	if (rx_ring->ndev->features & NETIF_F_RXCSUM) {
		u16 inet_csum = le16_to_cpu(rxbd->r.inet_csum);

		skb->csum = csum_unfold((__force __sum16)~htons(inet_csum));
		skb->ip_summed = CHECKSUM_COMPLETE;
	}

	if (le16_to_cpu(rxbd->r.flags) & ENETC_RXBD_FLAG_VLAN) {
		__be16 tpid = 0;

		switch (le16_to_cpu(rxbd->r.flags) & ENETC_RXBD_FLAG_TPID) {
		case 0:
			tpid = htons(ETH_P_8021Q);
			break;
		case 1:
			tpid = htons(ETH_P_8021AD);
			break;
		case 2:
			tpid = htons(enetc_port_rd(&priv->si->hw,
						   ENETC_PCVLANR1));
			break;
		case 3:
			tpid = htons(enetc_port_rd(&priv->si->hw,
						   ENETC_PCVLANR2));
			break;
		default:
			break;
		}

		__vlan_hwaccel_put_tag(skb, tpid, le16_to_cpu(rxbd->r.vlan_opt));
	}

#ifdef CONFIG_FSL_ENETC_PTP_CLOCK
	if (priv->active_offloads & ENETC_F_RX_TSTAMP)
		enetc_get_rx_tstamp(rx_ring->ndev, rxbd, skb);
#endif
}

/* This gets called during the non-XDP NAPI poll cycle as well as on XDP_PASS,
 * so it needs to work with both DMA_FROM_DEVICE as well as DMA_BIDIRECTIONAL
 * mapped buffers.
 */
static struct enetc_rx_swbd *enetc_get_rx_buff(struct enetc_bdr *rx_ring,
					       int i, u16 size)
{
	struct enetc_rx_swbd *rx_swbd = &rx_ring->rx_swbd[i];

	dma_sync_single_range_for_cpu(rx_ring->dev, rx_swbd->dma,
				      rx_swbd->page_offset,
				      size, rx_swbd->dir);
	return rx_swbd;
}

/* Reuse the current page without performing half-page buffer flipping */
static void enetc_put_rx_buff(struct enetc_bdr *rx_ring,
			      struct enetc_rx_swbd *rx_swbd)
{
	size_t buffer_size = ENETC_RXB_TRUESIZE - rx_ring->buffer_offset;

	enetc_reuse_page(rx_ring, rx_swbd);

	dma_sync_single_range_for_device(rx_ring->dev, rx_swbd->dma,
					 rx_swbd->page_offset,
					 buffer_size, rx_swbd->dir);

	rx_swbd->page = NULL;
}

/* Reuse the current page by performing half-page buffer flipping */
static void enetc_flip_rx_buff(struct enetc_bdr *rx_ring,
			       struct enetc_rx_swbd *rx_swbd)
{
	if (likely(enetc_page_reusable(rx_swbd->page))) {
		rx_swbd->page_offset ^= ENETC_RXB_TRUESIZE;
		page_ref_inc(rx_swbd->page);

		enetc_put_rx_buff(rx_ring, rx_swbd);
	} else {
		dma_unmap_page(rx_ring->dev, rx_swbd->dma, PAGE_SIZE,
			       rx_swbd->dir);
		rx_swbd->page = NULL;
	}
}

static struct sk_buff *enetc_map_rx_buff_to_skb(struct enetc_bdr *rx_ring,
						int i, u16 size)
{
	struct enetc_rx_swbd *rx_swbd = enetc_get_rx_buff(rx_ring, i, size);
	struct sk_buff *skb;
	void *ba;

	ba = page_address(rx_swbd->page) + rx_swbd->page_offset;
	skb = build_skb(ba - rx_ring->buffer_offset, ENETC_RXB_TRUESIZE);
	if (unlikely(!skb)) {
		rx_ring->stats.rx_alloc_errs++;
		return NULL;
	}

	skb_reserve(skb, rx_ring->buffer_offset);
	__skb_put(skb, size);

	enetc_flip_rx_buff(rx_ring, rx_swbd);

	return skb;
}

static void enetc_add_rx_buff_to_skb(struct enetc_bdr *rx_ring, int i,
				     u16 size, struct sk_buff *skb)
{
	struct enetc_rx_swbd *rx_swbd = enetc_get_rx_buff(rx_ring, i, size);

	skb_add_rx_frag(skb, skb_shinfo(skb)->nr_frags, rx_swbd->page,
			rx_swbd->page_offset, size, ENETC_RXB_TRUESIZE);

	enetc_flip_rx_buff(rx_ring, rx_swbd);
}

static bool enetc_check_bd_errors_and_consume(struct enetc_bdr *rx_ring,
					      u32 bd_status,
					      union enetc_rx_bd **rxbd, int *i)
{
	if (likely(!(bd_status & ENETC_RXBD_LSTATUS(ENETC_RXBD_ERR_MASK))))
		return false;

	enetc_put_rx_buff(rx_ring, &rx_ring->rx_swbd[*i]);
	enetc_rxbd_next(rx_ring, rxbd, i);

	while (!(bd_status & ENETC_RXBD_LSTATUS_F)) {
		dma_rmb();
		bd_status = le32_to_cpu((*rxbd)->r.lstatus);

		enetc_put_rx_buff(rx_ring, &rx_ring->rx_swbd[*i]);
		enetc_rxbd_next(rx_ring, rxbd, i);
	}

	rx_ring->ndev->stats.rx_dropped++;
	rx_ring->ndev->stats.rx_errors++;

	return true;
}

static struct sk_buff *enetc_build_skb(struct enetc_bdr *rx_ring,
				       u32 bd_status, union enetc_rx_bd **rxbd,
				       int *i, int *cleaned_cnt, int buffer_size)
{
	struct sk_buff *skb;
	u16 size;

	size = le16_to_cpu((*rxbd)->r.buf_len);
	skb = enetc_map_rx_buff_to_skb(rx_ring, *i, size);
	if (!skb)
		return NULL;

	enetc_get_offloads(rx_ring, *rxbd, skb);

	(*cleaned_cnt)++;

	enetc_rxbd_next(rx_ring, rxbd, i);

	/* not last BD in frame? */
	while (!(bd_status & ENETC_RXBD_LSTATUS_F)) {
		bd_status = le32_to_cpu((*rxbd)->r.lstatus);
		size = buffer_size;

		if (bd_status & ENETC_RXBD_LSTATUS_F) {
			dma_rmb();
			size = le16_to_cpu((*rxbd)->r.buf_len);
		}

		enetc_add_rx_buff_to_skb(rx_ring, *i, size, skb);

		(*cleaned_cnt)++;

		enetc_rxbd_next(rx_ring, rxbd, i);
	}

	skb_record_rx_queue(skb, rx_ring->index);
	skb->protocol = eth_type_trans(skb, rx_ring->ndev);

	return skb;
}

#define ENETC_RXBD_BUNDLE 16 /* # of BDs to update at once */

static int enetc_clean_rx_ring(struct enetc_bdr *rx_ring,
			       struct napi_struct *napi, int work_limit)
{
	int rx_frm_cnt = 0, rx_byte_cnt = 0;
	int cleaned_cnt, i;

	cleaned_cnt = enetc_bd_unused(rx_ring);
	/* next descriptor to process */
	i = rx_ring->next_to_clean;

	while (likely(rx_frm_cnt < work_limit)) {
		union enetc_rx_bd *rxbd;
		struct sk_buff *skb;
		u32 bd_status;

		if (cleaned_cnt >= ENETC_RXBD_BUNDLE)
			cleaned_cnt -= enetc_refill_rx_ring(rx_ring,
							    cleaned_cnt);

		rxbd = enetc_rxbd(rx_ring, i);
		bd_status = le32_to_cpu(rxbd->r.lstatus);
		if (!bd_status)
			break;

		enetc_wr_reg_hot(rx_ring->idr, BIT(rx_ring->index));
		dma_rmb(); /* for reading other rxbd fields */

		if (enetc_check_bd_errors_and_consume(rx_ring, bd_status,
						      &rxbd, &i))
			break;

		skb = enetc_build_skb(rx_ring, bd_status, &rxbd, &i,
				      &cleaned_cnt, ENETC_RXB_DMA_SIZE);
		if (!skb)
			break;

		rx_byte_cnt += skb->len;
		rx_frm_cnt++;

		napi_gro_receive(napi, skb);
	}

	rx_ring->next_to_clean = i;

	rx_ring->stats.packets += rx_frm_cnt;
	rx_ring->stats.bytes += rx_byte_cnt;

	return rx_frm_cnt;
}

static void enetc_xdp_map_tx_buff(struct enetc_bdr *tx_ring, int i,
				  struct enetc_tx_swbd *tx_swbd,
				  int frm_len)
{
	union enetc_tx_bd *txbd = ENETC_TXBD(*tx_ring, i);

	prefetchw(txbd);

	enetc_clear_tx_bd(txbd);
	txbd->addr = cpu_to_le64(tx_swbd->dma + tx_swbd->page_offset);
	txbd->buf_len = cpu_to_le16(tx_swbd->len);
	txbd->frm_len = cpu_to_le16(frm_len);

	memcpy(&tx_ring->tx_swbd[i], tx_swbd, sizeof(*tx_swbd));
}

/* Puts in the TX ring one XDP frame, mapped as an array of TX software buffer
 * descriptors.
 */
static bool enetc_xdp_tx(struct enetc_bdr *tx_ring,
			 struct enetc_tx_swbd *xdp_tx_arr, int num_tx_swbd)
{
	struct enetc_tx_swbd *tmp_tx_swbd = xdp_tx_arr;
	int i, k, frm_len = tmp_tx_swbd->len;

	if (unlikely(enetc_bd_unused(tx_ring) < ENETC_TXBDS_NEEDED(num_tx_swbd)))
		return false;

	while (unlikely(!tmp_tx_swbd->is_eof)) {
		tmp_tx_swbd++;
		frm_len += tmp_tx_swbd->len;
	}

	i = tx_ring->next_to_use;

	for (k = 0; k < num_tx_swbd; k++) {
		struct enetc_tx_swbd *xdp_tx_swbd = &xdp_tx_arr[k];

		enetc_xdp_map_tx_buff(tx_ring, i, xdp_tx_swbd, frm_len);

		/* last BD needs 'F' bit set */
		if (xdp_tx_swbd->is_eof) {
			union enetc_tx_bd *txbd = ENETC_TXBD(*tx_ring, i);

			txbd->flags = ENETC_TXBD_FLAGS_F;
		}

		enetc_bdr_idx_inc(tx_ring, &i);
	}

	tx_ring->next_to_use = i;

	return true;
}

static int enetc_xdp_frame_to_xdp_tx_swbd(struct enetc_bdr *tx_ring,
					  struct enetc_tx_swbd *xdp_tx_arr,
					  struct xdp_frame *xdp_frame)
{
	struct enetc_tx_swbd *xdp_tx_swbd = &xdp_tx_arr[0];
	struct skb_shared_info *shinfo;
	void *data = xdp_frame->data;
	int len = xdp_frame->len;
	skb_frag_t *frag;
	dma_addr_t dma;
	unsigned int f;
	int n = 0;

	dma = dma_map_single(tx_ring->dev, data, len, DMA_TO_DEVICE);
	if (unlikely(dma_mapping_error(tx_ring->dev, dma))) {
		netdev_err(tx_ring->ndev, "DMA map error\n");
		return -1;
	}

	xdp_tx_swbd->dma = dma;
	xdp_tx_swbd->dir = DMA_TO_DEVICE;
	xdp_tx_swbd->len = len;
	xdp_tx_swbd->is_xdp_redirect = true;
	xdp_tx_swbd->is_eof = false;
	xdp_tx_swbd->xdp_frame = NULL;

	n++;
	xdp_tx_swbd = &xdp_tx_arr[n];

	shinfo = xdp_get_shared_info_from_frame(xdp_frame);

	for (f = 0, frag = &shinfo->frags[0]; f < shinfo->nr_frags;
	     f++, frag++) {
		data = skb_frag_address(frag);
		len = skb_frag_size(frag);

		dma = dma_map_single(tx_ring->dev, data, len, DMA_TO_DEVICE);
		if (unlikely(dma_mapping_error(tx_ring->dev, dma))) {
			/* Undo the DMA mapping for all fragments */
			while (--n >= 0)
				enetc_unmap_tx_buff(tx_ring, &xdp_tx_arr[n]);

			netdev_err(tx_ring->ndev, "DMA map error\n");
			return -1;
		}

		xdp_tx_swbd->dma = dma;
		xdp_tx_swbd->dir = DMA_TO_DEVICE;
		xdp_tx_swbd->len = len;
		xdp_tx_swbd->is_xdp_redirect = true;
		xdp_tx_swbd->is_eof = false;
		xdp_tx_swbd->xdp_frame = NULL;

		n++;
		xdp_tx_swbd = &xdp_tx_arr[n];
	}

	xdp_tx_arr[n - 1].is_eof = true;
	xdp_tx_arr[n - 1].xdp_frame = xdp_frame;

	return n;
}

int enetc_xdp_xmit(struct net_device *ndev, int num_frames,
		   struct xdp_frame **frames, u32 flags)
{
	struct enetc_tx_swbd xdp_redirect_arr[ENETC_MAX_SKB_FRAGS] = {0};
	struct enetc_ndev_priv *priv = netdev_priv(ndev);
	struct enetc_bdr *tx_ring;
	int xdp_tx_bd_cnt, i, k;
	int xdp_tx_frm_cnt = 0;

	enetc_lock_mdio();

	tx_ring = priv->xdp_tx_ring[smp_processor_id()];

	prefetchw(ENETC_TXBD(*tx_ring, tx_ring->next_to_use));

	for (k = 0; k < num_frames; k++) {
		xdp_tx_bd_cnt = enetc_xdp_frame_to_xdp_tx_swbd(tx_ring,
							       xdp_redirect_arr,
							       frames[k]);
		if (unlikely(xdp_tx_bd_cnt < 0))
			break;

		if (unlikely(!enetc_xdp_tx(tx_ring, xdp_redirect_arr,
					   xdp_tx_bd_cnt))) {
			for (i = 0; i < xdp_tx_bd_cnt; i++)
				enetc_unmap_tx_buff(tx_ring,
						    &xdp_redirect_arr[i]);
			tx_ring->stats.xdp_tx_drops++;
			break;
		}

		xdp_tx_frm_cnt++;
	}

	if (unlikely((flags & XDP_XMIT_FLUSH) || k != xdp_tx_frm_cnt))
		enetc_update_tx_ring_tail(tx_ring);

	tx_ring->stats.xdp_tx += xdp_tx_frm_cnt;

	enetc_unlock_mdio();

	return xdp_tx_frm_cnt;
}

static void enetc_map_rx_buff_to_xdp(struct enetc_bdr *rx_ring, int i,
				     struct xdp_buff *xdp_buff, u16 size)
{
	struct enetc_rx_swbd *rx_swbd = enetc_get_rx_buff(rx_ring, i, size);
	void *hard_start = page_address(rx_swbd->page) + rx_swbd->page_offset;
	struct skb_shared_info *shinfo;

	/* To be used for XDP_TX */
	rx_swbd->len = size;

	xdp_prepare_buff(xdp_buff, hard_start - rx_ring->buffer_offset,
			 rx_ring->buffer_offset, size, false);

	shinfo = xdp_get_shared_info_from_buff(xdp_buff);
	shinfo->nr_frags = 0;
}

static void enetc_add_rx_buff_to_xdp(struct enetc_bdr *rx_ring, int i,
				     u16 size, struct xdp_buff *xdp_buff)
{
	struct skb_shared_info *shinfo = xdp_get_shared_info_from_buff(xdp_buff);
	struct enetc_rx_swbd *rx_swbd = enetc_get_rx_buff(rx_ring, i, size);
	skb_frag_t *frag = &shinfo->frags[shinfo->nr_frags];

	/* To be used for XDP_TX */
	rx_swbd->len = size;

	skb_frag_off_set(frag, rx_swbd->page_offset);
	skb_frag_size_set(frag, size);
	__skb_frag_set_page(frag, rx_swbd->page);

	shinfo->nr_frags++;
}

static void enetc_build_xdp_buff(struct enetc_bdr *rx_ring, u32 bd_status,
				 union enetc_rx_bd **rxbd, int *i,
				 int *cleaned_cnt, struct xdp_buff *xdp_buff)
{
	u16 size = le16_to_cpu((*rxbd)->r.buf_len);

	xdp_init_buff(xdp_buff, ENETC_RXB_TRUESIZE, &rx_ring->xdp.rxq);

	enetc_map_rx_buff_to_xdp(rx_ring, *i, xdp_buff, size);
	(*cleaned_cnt)++;
	enetc_rxbd_next(rx_ring, rxbd, i);

	/* not last BD in frame? */
	while (!(bd_status & ENETC_RXBD_LSTATUS_F)) {
		bd_status = le32_to_cpu((*rxbd)->r.lstatus);
		size = ENETC_RXB_DMA_SIZE_XDP;

		if (bd_status & ENETC_RXBD_LSTATUS_F) {
			dma_rmb();
			size = le16_to_cpu((*rxbd)->r.buf_len);
		}

		enetc_add_rx_buff_to_xdp(rx_ring, *i, size, xdp_buff);
		(*cleaned_cnt)++;
		enetc_rxbd_next(rx_ring, rxbd, i);
	}
}

/* Convert RX buffer descriptors to TX buffer descriptors. These will be
 * recycled back into the RX ring in enetc_clean_tx_ring.
 */
static int enetc_rx_swbd_to_xdp_tx_swbd(struct enetc_tx_swbd *xdp_tx_arr,
					struct enetc_bdr *rx_ring,
					int rx_ring_first, int rx_ring_last)
{
	int n = 0;

	for (; rx_ring_first != rx_ring_last;
	     n++, enetc_bdr_idx_inc(rx_ring, &rx_ring_first)) {
		struct enetc_rx_swbd *rx_swbd = &rx_ring->rx_swbd[rx_ring_first];
		struct enetc_tx_swbd *tx_swbd = &xdp_tx_arr[n];

		/* No need to dma_map, we already have DMA_BIDIRECTIONAL */
		tx_swbd->dma = rx_swbd->dma;
		tx_swbd->dir = rx_swbd->dir;
		tx_swbd->page = rx_swbd->page;
		tx_swbd->page_offset = rx_swbd->page_offset;
		tx_swbd->len = rx_swbd->len;
		tx_swbd->is_dma_page = true;
		tx_swbd->is_xdp_tx = true;
		tx_swbd->is_eof = false;
	}

	/* We rely on caller providing an rx_ring_last > rx_ring_first */
	xdp_tx_arr[n - 1].is_eof = true;

	return n;
}

static void enetc_xdp_drop(struct enetc_bdr *rx_ring, int rx_ring_first,
			   int rx_ring_last)
{
	while (rx_ring_first != rx_ring_last) {
		enetc_put_rx_buff(rx_ring,
				  &rx_ring->rx_swbd[rx_ring_first]);
		enetc_bdr_idx_inc(rx_ring, &rx_ring_first);
	}
	rx_ring->stats.xdp_drops++;
}

static void enetc_xdp_free(struct enetc_bdr *rx_ring, int rx_ring_first,
			   int rx_ring_last)
{
	while (rx_ring_first != rx_ring_last) {
		struct enetc_rx_swbd *rx_swbd = &rx_ring->rx_swbd[rx_ring_first];

		if (rx_swbd->page) {
			dma_unmap_page(rx_ring->dev, rx_swbd->dma, PAGE_SIZE,
				       rx_swbd->dir);
			__free_page(rx_swbd->page);
			rx_swbd->page = NULL;
		}
		enetc_bdr_idx_inc(rx_ring, &rx_ring_first);
	}
	rx_ring->stats.xdp_redirect_failures++;
}

static int enetc_clean_rx_ring_xdp(struct enetc_bdr *rx_ring,
				   struct napi_struct *napi, int work_limit,
				   struct bpf_prog *prog)
{
	int xdp_tx_bd_cnt, xdp_tx_frm_cnt = 0, xdp_redirect_frm_cnt = 0;
	struct enetc_tx_swbd xdp_tx_arr[ENETC_MAX_SKB_FRAGS] = {0};
	struct enetc_ndev_priv *priv = netdev_priv(rx_ring->ndev);
	int rx_frm_cnt = 0, rx_byte_cnt = 0;
	struct enetc_bdr *tx_ring;
	int cleaned_cnt, i;
	u32 xdp_act;

	cleaned_cnt = enetc_bd_unused(rx_ring);
	/* next descriptor to process */
	i = rx_ring->next_to_clean;

	while (likely(rx_frm_cnt < work_limit)) {
		union enetc_rx_bd *rxbd, *orig_rxbd;
		int orig_i, orig_cleaned_cnt;
		struct xdp_buff xdp_buff;
		struct sk_buff *skb;
		int tmp_orig_i, err;
		u32 bd_status;

		rxbd = enetc_rxbd(rx_ring, i);
		bd_status = le32_to_cpu(rxbd->r.lstatus);
		if (!bd_status)
			break;

		enetc_wr_reg_hot(rx_ring->idr, BIT(rx_ring->index));
		dma_rmb(); /* for reading other rxbd fields */

		if (enetc_check_bd_errors_and_consume(rx_ring, bd_status,
						      &rxbd, &i))
			break;

		orig_rxbd = rxbd;
		orig_cleaned_cnt = cleaned_cnt;
		orig_i = i;

		enetc_build_xdp_buff(rx_ring, bd_status, &rxbd, &i,
				     &cleaned_cnt, &xdp_buff);

		xdp_act = bpf_prog_run_xdp(prog, &xdp_buff);

		switch (xdp_act) {
		default:
			bpf_warn_invalid_xdp_action(xdp_act);
			fallthrough;
		case XDP_ABORTED:
			trace_xdp_exception(rx_ring->ndev, prog, xdp_act);
			fallthrough;
		case XDP_DROP:
			enetc_xdp_drop(rx_ring, orig_i, i);
			break;
		case XDP_PASS:
			rxbd = orig_rxbd;
			cleaned_cnt = orig_cleaned_cnt;
			i = orig_i;

			skb = enetc_build_skb(rx_ring, bd_status, &rxbd,
					      &i, &cleaned_cnt,
					      ENETC_RXB_DMA_SIZE_XDP);
			if (unlikely(!skb))
				goto out;

			napi_gro_receive(napi, skb);
			break;
		case XDP_TX:
			tx_ring = priv->xdp_tx_ring[rx_ring->index];
			xdp_tx_bd_cnt = enetc_rx_swbd_to_xdp_tx_swbd(xdp_tx_arr,
								     rx_ring,
								     orig_i, i);

			if (!enetc_xdp_tx(tx_ring, xdp_tx_arr, xdp_tx_bd_cnt)) {
				enetc_xdp_drop(rx_ring, orig_i, i);
				tx_ring->stats.xdp_tx_drops++;
			} else {
				tx_ring->stats.xdp_tx += xdp_tx_bd_cnt;
				rx_ring->xdp.xdp_tx_in_flight += xdp_tx_bd_cnt;
				xdp_tx_frm_cnt++;
				/* The XDP_TX enqueue was successful, so we
				 * need to scrub the RX software BDs because
				 * the ownership of the buffers no longer
				 * belongs to the RX ring, and we must prevent
				 * enetc_refill_rx_ring() from reusing
				 * rx_swbd->page.
				 */
				while (orig_i != i) {
					rx_ring->rx_swbd[orig_i].page = NULL;
					enetc_bdr_idx_inc(rx_ring, &orig_i);
				}
			}
			break;
		case XDP_REDIRECT:
			/* xdp_return_frame does not support S/G in the sense
			 * that it leaks the fragments (__xdp_return should not
			 * call page_frag_free only for the initial buffer).
			 * Until XDP_REDIRECT gains support for S/G let's keep
			 * the code structure in place, but dead. We drop the
			 * S/G frames ourselves to avoid memory leaks which
			 * would otherwise leave the kernel OOM.
			 */
			if (unlikely(cleaned_cnt - orig_cleaned_cnt != 1)) {
				enetc_xdp_drop(rx_ring, orig_i, i);
				rx_ring->stats.xdp_redirect_sg++;
				break;
			}

			tmp_orig_i = orig_i;

			while (orig_i != i) {
				enetc_flip_rx_buff(rx_ring,
						   &rx_ring->rx_swbd[orig_i]);
				enetc_bdr_idx_inc(rx_ring, &orig_i);
			}

			err = xdp_do_redirect(rx_ring->ndev, &xdp_buff, prog);
			if (unlikely(err)) {
				enetc_xdp_free(rx_ring, tmp_orig_i, i);
			} else {
				xdp_redirect_frm_cnt++;
				rx_ring->stats.xdp_redirect++;
			}
		}

		rx_frm_cnt++;
	}

out:
	rx_ring->next_to_clean = i;

	rx_ring->stats.packets += rx_frm_cnt;
	rx_ring->stats.bytes += rx_byte_cnt;

	if (xdp_redirect_frm_cnt)
		xdp_do_flush_map();

	if (xdp_tx_frm_cnt)
		enetc_update_tx_ring_tail(tx_ring);

	if (cleaned_cnt > rx_ring->xdp.xdp_tx_in_flight)
		enetc_refill_rx_ring(rx_ring, enetc_bd_unused(rx_ring) -
				     rx_ring->xdp.xdp_tx_in_flight);

	return rx_frm_cnt;
}

static int enetc_poll(struct napi_struct *napi, int budget)
{
	struct enetc_int_vector
		*v = container_of(napi, struct enetc_int_vector, napi);
	struct enetc_bdr *rx_ring = &v->rx_ring;
	struct bpf_prog *prog;
	bool complete = true;
	int work_done;
	int i;

	enetc_lock_mdio();

	for (i = 0; i < v->count_tx_rings; i++)
		if (!enetc_clean_tx_ring(&v->tx_ring[i], budget))
			complete = false;

	prog = rx_ring->xdp.prog;
	if (prog)
		work_done = enetc_clean_rx_ring_xdp(rx_ring, napi, budget, prog);
	else
		work_done = enetc_clean_rx_ring(rx_ring, napi, budget);
	if (work_done == budget)
		complete = false;
	if (work_done)
		v->rx_napi_work = true;

	if (!complete) {
		enetc_unlock_mdio();
		return budget;
	}

	napi_complete_done(napi, work_done);

	if (likely(v->rx_dim_en))
		enetc_rx_net_dim(v);

	v->rx_napi_work = false;

	/* enable interrupts */
	enetc_wr_reg_hot(v->rbier, ENETC_RBIER_RXTIE);

	for_each_set_bit(i, &v->tx_rings_map, ENETC_MAX_NUM_TXQS)
		enetc_wr_reg_hot(v->tbier_base + ENETC_BDR_OFF(i),
				 ENETC_TBIER_TXTIE);

	enetc_unlock_mdio();

	return work_done;
}

/* Probing and Init */
#define ENETC_MAX_RFS_SIZE 64
void enetc_get_si_caps(struct enetc_si *si)
{
	struct enetc_hw *hw = &si->hw;
	u32 val;

	/* find out how many of various resources we have to work with */
	val = enetc_rd(hw, ENETC_SICAPR0);
	si->num_rx_rings = (val >> 16) & 0xff;
	si->num_tx_rings = val & 0xff;

	val = enetc_rd(hw, ENETC_SIRFSCAPR);
	si->num_fs_entries = ENETC_SIRFSCAPR_GET_NUM_RFS(val);
	si->num_fs_entries = min(si->num_fs_entries, ENETC_MAX_RFS_SIZE);

	si->num_rss = 0;
	val = enetc_rd(hw, ENETC_SIPCAPR0);
	if (val & ENETC_SIPCAPR0_RSS) {
		u32 rss;

		rss = enetc_rd(hw, ENETC_SIRSSCAPR);
		si->num_rss = ENETC_SIRSSCAPR_GET_NUM_RSS(rss);
	}

	if (val & ENETC_SIPCAPR0_QBV)
		si->hw_features |= ENETC_SI_F_QBV;

	if (val & ENETC_SIPCAPR0_PSFP)
		si->hw_features |= ENETC_SI_F_PSFP;
}

static int enetc_dma_alloc_bdr(struct enetc_bdr *r, size_t bd_size)
{
	r->bd_base = dma_alloc_coherent(r->dev, r->bd_count * bd_size,
					&r->bd_dma_base, GFP_KERNEL);
	if (!r->bd_base)
		return -ENOMEM;

	/* h/w requires 128B alignment */
	if (!IS_ALIGNED(r->bd_dma_base, 128)) {
		dma_free_coherent(r->dev, r->bd_count * bd_size, r->bd_base,
				  r->bd_dma_base);
		return -EINVAL;
	}

	return 0;
}

static int enetc_alloc_txbdr(struct enetc_bdr *txr)
{
	int err;

	txr->tx_swbd = vzalloc(txr->bd_count * sizeof(struct enetc_tx_swbd));
	if (!txr->tx_swbd)
		return -ENOMEM;

	err = enetc_dma_alloc_bdr(txr, sizeof(union enetc_tx_bd));
	if (err) {
		vfree(txr->tx_swbd);
		return err;
	}

	txr->next_to_clean = 0;
	txr->next_to_use = 0;

	return 0;
}

static void enetc_free_txbdr(struct enetc_bdr *txr)
{
	int size, i;

	for (i = 0; i < txr->bd_count; i++)
		enetc_free_tx_frame(txr, &txr->tx_swbd[i]);

	size = txr->bd_count * sizeof(union enetc_tx_bd);

	dma_free_coherent(txr->dev, size, txr->bd_base, txr->bd_dma_base);
	txr->bd_base = NULL;

	vfree(txr->tx_swbd);
	txr->tx_swbd = NULL;
}

static int enetc_alloc_tx_resources(struct enetc_ndev_priv *priv)
{
	int i, err;

	for (i = 0; i < priv->num_tx_rings; i++) {
		err = enetc_alloc_txbdr(priv->tx_ring[i]);

		if (err)
			goto fail;
	}

	return 0;

fail:
	while (i-- > 0)
		enetc_free_txbdr(priv->tx_ring[i]);

	return err;
}

static void enetc_free_tx_resources(struct enetc_ndev_priv *priv)
{
	int i;

	for (i = 0; i < priv->num_tx_rings; i++)
		enetc_free_txbdr(priv->tx_ring[i]);
}

static int enetc_alloc_rxbdr(struct enetc_bdr *rxr, bool extended)
{
	size_t size = sizeof(union enetc_rx_bd);
	int err;

	rxr->rx_swbd = vzalloc(rxr->bd_count * sizeof(struct enetc_rx_swbd));
	if (!rxr->rx_swbd)
		return -ENOMEM;

	if (extended)
		size *= 2;

	err = enetc_dma_alloc_bdr(rxr, size);
	if (err) {
		vfree(rxr->rx_swbd);
		return err;
	}

	rxr->next_to_clean = 0;
	rxr->next_to_use = 0;
	rxr->next_to_alloc = 0;
	rxr->ext_en = extended;

	return 0;
}

static void enetc_free_rxbdr(struct enetc_bdr *rxr)
{
	int size;

	size = rxr->bd_count * sizeof(union enetc_rx_bd);

	dma_free_coherent(rxr->dev, size, rxr->bd_base, rxr->bd_dma_base);
	rxr->bd_base = NULL;

	vfree(rxr->rx_swbd);
	rxr->rx_swbd = NULL;
}

static int enetc_alloc_rx_resources(struct enetc_ndev_priv *priv)
{
	bool extended = !!(priv->active_offloads & ENETC_F_RX_TSTAMP);
	int i, err;

	for (i = 0; i < priv->num_rx_rings; i++) {
		err = enetc_alloc_rxbdr(priv->rx_ring[i], extended);

		if (err)
			goto fail;
	}

	return 0;

fail:
	while (i-- > 0)
		enetc_free_rxbdr(priv->rx_ring[i]);

	return err;
}

static void enetc_free_rx_resources(struct enetc_ndev_priv *priv)
{
	int i;

	for (i = 0; i < priv->num_rx_rings; i++)
		enetc_free_rxbdr(priv->rx_ring[i]);
}

static void enetc_free_tx_ring(struct enetc_bdr *tx_ring)
{
	int i;

	if (!tx_ring->tx_swbd)
		return;

	for (i = 0; i < tx_ring->bd_count; i++) {
		struct enetc_tx_swbd *tx_swbd = &tx_ring->tx_swbd[i];

		enetc_free_tx_frame(tx_ring, tx_swbd);
	}

	tx_ring->next_to_clean = 0;
	tx_ring->next_to_use = 0;
}

static void enetc_free_rx_ring(struct enetc_bdr *rx_ring)
{
	int i;

	if (!rx_ring->rx_swbd)
		return;

	for (i = 0; i < rx_ring->bd_count; i++) {
		struct enetc_rx_swbd *rx_swbd = &rx_ring->rx_swbd[i];

		if (!rx_swbd->page)
			continue;

		dma_unmap_page(rx_ring->dev, rx_swbd->dma, PAGE_SIZE,
			       rx_swbd->dir);
		__free_page(rx_swbd->page);
		rx_swbd->page = NULL;
	}

	rx_ring->next_to_clean = 0;
	rx_ring->next_to_use = 0;
	rx_ring->next_to_alloc = 0;
}

static void enetc_free_rxtx_rings(struct enetc_ndev_priv *priv)
{
	int i;

	for (i = 0; i < priv->num_rx_rings; i++)
		enetc_free_rx_ring(priv->rx_ring[i]);

	for (i = 0; i < priv->num_tx_rings; i++)
		enetc_free_tx_ring(priv->tx_ring[i]);
}

static int enetc_setup_default_rss_table(struct enetc_si *si, int num_groups)
{
	int *rss_table;
	int i;

	rss_table = kmalloc_array(si->num_rss, sizeof(*rss_table), GFP_KERNEL);
	if (!rss_table)
		return -ENOMEM;

	/* Set up RSS table defaults */
	for (i = 0; i < si->num_rss; i++)
		rss_table[i] = i % num_groups;

	enetc_set_rss_table(si, rss_table, si->num_rss);

	kfree(rss_table);

	return 0;
}

int enetc_configure_si(struct enetc_ndev_priv *priv)
{
	struct enetc_si *si = priv->si;
	struct enetc_hw *hw = &si->hw;
	int err;

	/* set SI cache attributes */
	enetc_wr(hw, ENETC_SICAR0,
		 ENETC_SICAR_RD_COHERENT | ENETC_SICAR_WR_COHERENT);
	enetc_wr(hw, ENETC_SICAR1, ENETC_SICAR_MSI);
	/* enable SI */
	enetc_wr(hw, ENETC_SIMR, ENETC_SIMR_EN);

	if (si->num_rss) {
		err = enetc_setup_default_rss_table(si, priv->num_rx_rings);
		if (err)
			return err;
	}

	return 0;
}

void enetc_init_si_rings_params(struct enetc_ndev_priv *priv)
{
	struct enetc_si *si = priv->si;
	int cpus = num_online_cpus();

	priv->tx_bd_count = ENETC_TX_RING_DEFAULT_SIZE;
	priv->rx_bd_count = ENETC_RX_RING_DEFAULT_SIZE;

	/* Enable all available TX rings in order to configure as many
	 * priorities as possible, when needed.
	 * TODO: Make # of TX rings run-time configurable
	 */
	priv->num_rx_rings = min_t(int, cpus, si->num_rx_rings);
	priv->num_tx_rings = si->num_tx_rings;
	priv->bdr_int_num = cpus;
	priv->ic_mode = ENETC_IC_RX_ADAPTIVE | ENETC_IC_TX_MANUAL;
	priv->tx_ictt = ENETC_TXIC_TIMETHR;
}

int enetc_alloc_si_resources(struct enetc_ndev_priv *priv)
{
	struct enetc_si *si = priv->si;

	priv->cls_rules = kcalloc(si->num_fs_entries, sizeof(*priv->cls_rules),
				  GFP_KERNEL);
	if (!priv->cls_rules)
		return -ENOMEM;

	return 0;
}

void enetc_free_si_resources(struct enetc_ndev_priv *priv)
{
	kfree(priv->cls_rules);
}

static void enetc_setup_txbdr(struct enetc_hw *hw, struct enetc_bdr *tx_ring)
{
	int idx = tx_ring->index;
	u32 tbmr;

	enetc_txbdr_wr(hw, idx, ENETC_TBBAR0,
		       lower_32_bits(tx_ring->bd_dma_base));

	enetc_txbdr_wr(hw, idx, ENETC_TBBAR1,
		       upper_32_bits(tx_ring->bd_dma_base));

	WARN_ON(!IS_ALIGNED(tx_ring->bd_count, 64)); /* multiple of 64 */
	enetc_txbdr_wr(hw, idx, ENETC_TBLENR,
		       ENETC_RTBLENR_LEN(tx_ring->bd_count));

	/* clearing PI/CI registers for Tx not supported, adjust sw indexes */
	tx_ring->next_to_use = enetc_txbdr_rd(hw, idx, ENETC_TBPIR);
	tx_ring->next_to_clean = enetc_txbdr_rd(hw, idx, ENETC_TBCIR);

	/* enable Tx ints by setting pkt thr to 1 */
	enetc_txbdr_wr(hw, idx, ENETC_TBICR0, ENETC_TBICR0_ICEN | 0x1);

	tbmr = ENETC_TBMR_EN;
	if (tx_ring->ndev->features & NETIF_F_HW_VLAN_CTAG_TX)
		tbmr |= ENETC_TBMR_VIH;

	/* enable ring */
	enetc_txbdr_wr(hw, idx, ENETC_TBMR, tbmr);

	tx_ring->tpir = hw->reg + ENETC_BDR(TX, idx, ENETC_TBPIR);
	tx_ring->tcir = hw->reg + ENETC_BDR(TX, idx, ENETC_TBCIR);
	tx_ring->idr = hw->reg + ENETC_SITXIDR;
}

static void enetc_setup_rxbdr(struct enetc_hw *hw, struct enetc_bdr *rx_ring)
{
	int idx = rx_ring->index;
	u32 rbmr;

	enetc_rxbdr_wr(hw, idx, ENETC_RBBAR0,
		       lower_32_bits(rx_ring->bd_dma_base));

	enetc_rxbdr_wr(hw, idx, ENETC_RBBAR1,
		       upper_32_bits(rx_ring->bd_dma_base));

	WARN_ON(!IS_ALIGNED(rx_ring->bd_count, 64)); /* multiple of 64 */
	enetc_rxbdr_wr(hw, idx, ENETC_RBLENR,
		       ENETC_RTBLENR_LEN(rx_ring->bd_count));

	if (rx_ring->xdp.prog)
		enetc_rxbdr_wr(hw, idx, ENETC_RBBSR, ENETC_RXB_DMA_SIZE_XDP);
	else
		enetc_rxbdr_wr(hw, idx, ENETC_RBBSR, ENETC_RXB_DMA_SIZE);

	enetc_rxbdr_wr(hw, idx, ENETC_RBPIR, 0);

	/* enable Rx ints by setting pkt thr to 1 */
	enetc_rxbdr_wr(hw, idx, ENETC_RBICR0, ENETC_RBICR0_ICEN | 0x1);

	rbmr = ENETC_RBMR_EN;

	if (rx_ring->ext_en)
		rbmr |= ENETC_RBMR_BDS;

	if (rx_ring->ndev->features & NETIF_F_HW_VLAN_CTAG_RX)
		rbmr |= ENETC_RBMR_VTE;

	rx_ring->rcir = hw->reg + ENETC_BDR(RX, idx, ENETC_RBCIR);
	rx_ring->idr = hw->reg + ENETC_SIRXIDR;

	enetc_lock_mdio();
	enetc_refill_rx_ring(rx_ring, enetc_bd_unused(rx_ring));
	enetc_unlock_mdio();

	/* enable ring */
	enetc_rxbdr_wr(hw, idx, ENETC_RBMR, rbmr);
}

static void enetc_setup_bdrs(struct enetc_ndev_priv *priv)
{
	int i;

	for (i = 0; i < priv->num_tx_rings; i++)
		enetc_setup_txbdr(&priv->si->hw, priv->tx_ring[i]);

	for (i = 0; i < priv->num_rx_rings; i++)
		enetc_setup_rxbdr(&priv->si->hw, priv->rx_ring[i]);
}

static void enetc_clear_rxbdr(struct enetc_hw *hw, struct enetc_bdr *rx_ring)
{
	int idx = rx_ring->index;

	/* disable EN bit on ring */
	enetc_rxbdr_wr(hw, idx, ENETC_RBMR, 0);
}

static void enetc_clear_txbdr(struct enetc_hw *hw, struct enetc_bdr *tx_ring)
{
	int delay = 8, timeout = 100;
	int idx = tx_ring->index;

	/* disable EN bit on ring */
	enetc_txbdr_wr(hw, idx, ENETC_TBMR, 0);

	/* wait for busy to clear */
	while (delay < timeout &&
	       enetc_txbdr_rd(hw, idx, ENETC_TBSR) & ENETC_TBSR_BUSY) {
		msleep(delay);
		delay *= 2;
	}

	if (delay >= timeout)
		netdev_warn(tx_ring->ndev, "timeout for tx ring #%d clear\n",
			    idx);
}

static void enetc_clear_bdrs(struct enetc_ndev_priv *priv)
{
	int i;

	for (i = 0; i < priv->num_tx_rings; i++)
		enetc_clear_txbdr(&priv->si->hw, priv->tx_ring[i]);

	for (i = 0; i < priv->num_rx_rings; i++)
		enetc_clear_rxbdr(&priv->si->hw, priv->rx_ring[i]);

	udelay(1);
}

static int enetc_setup_irqs(struct enetc_ndev_priv *priv)
{
	struct pci_dev *pdev = priv->si->pdev;
	cpumask_t cpu_mask;
	int i, j, err;

	for (i = 0; i < priv->bdr_int_num; i++) {
		int irq = pci_irq_vector(pdev, ENETC_BDR_INT_BASE_IDX + i);
		struct enetc_int_vector *v = priv->int_vector[i];
		int entry = ENETC_BDR_INT_BASE_IDX + i;
		struct enetc_hw *hw = &priv->si->hw;

		snprintf(v->name, sizeof(v->name), "%s-rxtx%d",
			 priv->ndev->name, i);
		err = request_irq(irq, enetc_msix, 0, v->name, v);
		if (err) {
			dev_err(priv->dev, "request_irq() failed!\n");
			goto irq_err;
		}
		disable_irq(irq);

		v->tbier_base = hw->reg + ENETC_BDR(TX, 0, ENETC_TBIER);
		v->rbier = hw->reg + ENETC_BDR(RX, i, ENETC_RBIER);
		v->ricr1 = hw->reg + ENETC_BDR(RX, i, ENETC_RBICR1);

		enetc_wr(hw, ENETC_SIMSIRRV(i), entry);

		for (j = 0; j < v->count_tx_rings; j++) {
			int idx = v->tx_ring[j].index;

			enetc_wr(hw, ENETC_SIMSITRV(idx), entry);
		}
		cpumask_clear(&cpu_mask);
		cpumask_set_cpu(i % num_online_cpus(), &cpu_mask);
		irq_set_affinity_hint(irq, &cpu_mask);
	}

	return 0;

irq_err:
	while (i--) {
		int irq = pci_irq_vector(pdev, ENETC_BDR_INT_BASE_IDX + i);

		irq_set_affinity_hint(irq, NULL);
		free_irq(irq, priv->int_vector[i]);
	}

	return err;
}

static void enetc_free_irqs(struct enetc_ndev_priv *priv)
{
	struct pci_dev *pdev = priv->si->pdev;
	int i;

	for (i = 0; i < priv->bdr_int_num; i++) {
		int irq = pci_irq_vector(pdev, ENETC_BDR_INT_BASE_IDX + i);

		irq_set_affinity_hint(irq, NULL);
		free_irq(irq, priv->int_vector[i]);
	}
}

static void enetc_setup_interrupts(struct enetc_ndev_priv *priv)
{
	struct enetc_hw *hw = &priv->si->hw;
	u32 icpt, ictt;
	int i;

	/* enable Tx & Rx event indication */
	if (priv->ic_mode &
	    (ENETC_IC_RX_MANUAL | ENETC_IC_RX_ADAPTIVE)) {
		icpt = ENETC_RBICR0_SET_ICPT(ENETC_RXIC_PKTTHR);
		/* init to non-0 minimum, will be adjusted later */
		ictt = 0x1;
	} else {
		icpt = 0x1; /* enable Rx ints by setting pkt thr to 1 */
		ictt = 0;
	}

	for (i = 0; i < priv->num_rx_rings; i++) {
		enetc_rxbdr_wr(hw, i, ENETC_RBICR1, ictt);
		enetc_rxbdr_wr(hw, i, ENETC_RBICR0, ENETC_RBICR0_ICEN | icpt);
		enetc_rxbdr_wr(hw, i, ENETC_RBIER, ENETC_RBIER_RXTIE);
	}

	if (priv->ic_mode & ENETC_IC_TX_MANUAL)
		icpt = ENETC_TBICR0_SET_ICPT(ENETC_TXIC_PKTTHR);
	else
		icpt = 0x1; /* enable Tx ints by setting pkt thr to 1 */

	for (i = 0; i < priv->num_tx_rings; i++) {
		enetc_txbdr_wr(hw, i, ENETC_TBICR1, priv->tx_ictt);
		enetc_txbdr_wr(hw, i, ENETC_TBICR0, ENETC_TBICR0_ICEN | icpt);
		enetc_txbdr_wr(hw, i, ENETC_TBIER, ENETC_TBIER_TXTIE);
	}
}

static void enetc_clear_interrupts(struct enetc_ndev_priv *priv)
{
	int i;

	for (i = 0; i < priv->num_tx_rings; i++)
		enetc_txbdr_wr(&priv->si->hw, i, ENETC_TBIER, 0);

	for (i = 0; i < priv->num_rx_rings; i++)
		enetc_rxbdr_wr(&priv->si->hw, i, ENETC_RBIER, 0);
}

static int enetc_phylink_connect(struct net_device *ndev)
{
	struct enetc_ndev_priv *priv = netdev_priv(ndev);
	struct ethtool_eee edata;
	int err;

	if (!priv->phylink)
		return 0; /* phy-less mode */

	err = phylink_of_phy_connect(priv->phylink, priv->dev->of_node, 0);
	if (err) {
		dev_err(&ndev->dev, "could not attach to PHY\n");
		return err;
	}

	/* disable EEE autoneg, until ENETC driver supports it */
	memset(&edata, 0, sizeof(struct ethtool_eee));
	phylink_ethtool_set_eee(priv->phylink, &edata);

	return 0;
}

static void enetc_tx_onestep_tstamp(struct work_struct *work)
{
	struct enetc_ndev_priv *priv;
	struct sk_buff *skb;

	priv = container_of(work, struct enetc_ndev_priv, tx_onestep_tstamp);

	netif_tx_lock(priv->ndev);

	clear_bit_unlock(ENETC_TX_ONESTEP_TSTAMP_IN_PROGRESS, &priv->flags);
	skb = skb_dequeue(&priv->tx_skbs);
	if (skb)
		enetc_start_xmit(skb, priv->ndev);

	netif_tx_unlock(priv->ndev);
}

static void enetc_tx_onestep_tstamp_init(struct enetc_ndev_priv *priv)
{
	INIT_WORK(&priv->tx_onestep_tstamp, enetc_tx_onestep_tstamp);
	skb_queue_head_init(&priv->tx_skbs);
}

void enetc_start(struct net_device *ndev)
{
	struct enetc_ndev_priv *priv = netdev_priv(ndev);
	int i;

	enetc_setup_interrupts(priv);

	for (i = 0; i < priv->bdr_int_num; i++) {
		int irq = pci_irq_vector(priv->si->pdev,
					 ENETC_BDR_INT_BASE_IDX + i);

		napi_enable(&priv->int_vector[i]->napi);
		enable_irq(irq);
	}

	if (priv->phylink)
		phylink_start(priv->phylink);
	else
		netif_carrier_on(ndev);

	netif_tx_start_all_queues(ndev);
}

int enetc_open(struct net_device *ndev)
{
	struct enetc_ndev_priv *priv = netdev_priv(ndev);
	int num_stack_tx_queues;
	int err;

	err = enetc_setup_irqs(priv);
	if (err)
		return err;

	err = enetc_phylink_connect(ndev);
	if (err)
		goto err_phy_connect;

	err = enetc_alloc_tx_resources(priv);
	if (err)
		goto err_alloc_tx;

	err = enetc_alloc_rx_resources(priv);
	if (err)
		goto err_alloc_rx;

	num_stack_tx_queues = enetc_num_stack_tx_queues(priv);

	err = netif_set_real_num_tx_queues(ndev, num_stack_tx_queues);
	if (err)
		goto err_set_queues;

	err = netif_set_real_num_rx_queues(ndev, priv->num_rx_rings);
	if (err)
		goto err_set_queues;

	enetc_tx_onestep_tstamp_init(priv);
	enetc_setup_bdrs(priv);
	enetc_start(ndev);

	return 0;

err_set_queues:
	enetc_free_rx_resources(priv);
err_alloc_rx:
	enetc_free_tx_resources(priv);
err_alloc_tx:
	if (priv->phylink)
		phylink_disconnect_phy(priv->phylink);
err_phy_connect:
	enetc_free_irqs(priv);

	return err;
}

void enetc_stop(struct net_device *ndev)
{
	struct enetc_ndev_priv *priv = netdev_priv(ndev);
	int i;

	netif_tx_stop_all_queues(ndev);

	for (i = 0; i < priv->bdr_int_num; i++) {
		int irq = pci_irq_vector(priv->si->pdev,
					 ENETC_BDR_INT_BASE_IDX + i);

		disable_irq(irq);
		napi_synchronize(&priv->int_vector[i]->napi);
		napi_disable(&priv->int_vector[i]->napi);
	}

	if (priv->phylink)
		phylink_stop(priv->phylink);
	else
		netif_carrier_off(ndev);

	enetc_clear_interrupts(priv);
}

int enetc_close(struct net_device *ndev)
{
	struct enetc_ndev_priv *priv = netdev_priv(ndev);

	enetc_stop(ndev);
	enetc_clear_bdrs(priv);

	if (priv->phylink)
		phylink_disconnect_phy(priv->phylink);
	enetc_free_rxtx_rings(priv);
	enetc_free_rx_resources(priv);
	enetc_free_tx_resources(priv);
	enetc_free_irqs(priv);

	return 0;
}

static int enetc_setup_tc_mqprio(struct net_device *ndev, void *type_data)
{
	struct enetc_ndev_priv *priv = netdev_priv(ndev);
	struct tc_mqprio_qopt *mqprio = type_data;
	struct enetc_bdr *tx_ring;
	int num_stack_tx_queues;
	u8 num_tc;
	int i;

	num_stack_tx_queues = enetc_num_stack_tx_queues(priv);
	mqprio->hw = TC_MQPRIO_HW_OFFLOAD_TCS;
	num_tc = mqprio->num_tc;

	if (!num_tc) {
		netdev_reset_tc(ndev);
		netif_set_real_num_tx_queues(ndev, num_stack_tx_queues);

		/* Reset all ring priorities to 0 */
		for (i = 0; i < priv->num_tx_rings; i++) {
			tx_ring = priv->tx_ring[i];
			enetc_set_bdr_prio(&priv->si->hw, tx_ring->index, 0);
		}

		return 0;
	}

	/* Check if we have enough BD rings available to accommodate all TCs */
	if (num_tc > num_stack_tx_queues) {
		netdev_err(ndev, "Max %d traffic classes supported\n",
			   priv->num_tx_rings);
		return -EINVAL;
	}

	/* For the moment, we use only one BD ring per TC.
	 *
	 * Configure num_tc BD rings with increasing priorities.
	 */
	for (i = 0; i < num_tc; i++) {
		tx_ring = priv->tx_ring[i];
		enetc_set_bdr_prio(&priv->si->hw, tx_ring->index, i);
	}

	/* Reset the number of netdev queues based on the TC count */
	netif_set_real_num_tx_queues(ndev, num_tc);

	netdev_set_num_tc(ndev, num_tc);

	/* Each TC is associated with one netdev queue */
	for (i = 0; i < num_tc; i++)
		netdev_set_tc_queue(ndev, i, 1, i);

	return 0;
}

int enetc_setup_tc(struct net_device *ndev, enum tc_setup_type type,
		   void *type_data)
{
	switch (type) {
	case TC_SETUP_QDISC_MQPRIO:
		return enetc_setup_tc_mqprio(ndev, type_data);
	case TC_SETUP_QDISC_TAPRIO:
		return enetc_setup_tc_taprio(ndev, type_data);
	case TC_SETUP_QDISC_CBS:
		return enetc_setup_tc_cbs(ndev, type_data);
	case TC_SETUP_QDISC_ETF:
		return enetc_setup_tc_txtime(ndev, type_data);
	case TC_SETUP_BLOCK:
		return enetc_setup_tc_psfp(ndev, type_data);
	default:
		return -EOPNOTSUPP;
	}
}

static int enetc_setup_xdp_prog(struct net_device *dev, struct bpf_prog *prog,
				struct netlink_ext_ack *extack)
{
	struct enetc_ndev_priv *priv = netdev_priv(dev);
	struct bpf_prog *old_prog;
	bool is_up;
	int i;

	/* The buffer layout is changing, so we need to drain the old
	 * RX buffers and seed new ones.
	 */
	is_up = netif_running(dev);
	if (is_up)
		dev_close(dev);

	old_prog = xchg(&priv->xdp_prog, prog);
	if (old_prog)
		bpf_prog_put(old_prog);

	for (i = 0; i < priv->num_rx_rings; i++) {
		struct enetc_bdr *rx_ring = priv->rx_ring[i];

		rx_ring->xdp.prog = prog;

		if (prog)
			rx_ring->buffer_offset = XDP_PACKET_HEADROOM;
		else
			rx_ring->buffer_offset = ENETC_RXB_PAD;
	}

	if (is_up)
		return dev_open(dev, extack);

	return 0;
}

int enetc_setup_bpf(struct net_device *dev, struct netdev_bpf *xdp)
{
	switch (xdp->command) {
	case XDP_SETUP_PROG:
		return enetc_setup_xdp_prog(dev, xdp->prog, xdp->extack);
	default:
		return -EINVAL;
	}

	return 0;
}

struct net_device_stats *enetc_get_stats(struct net_device *ndev)
{
	struct enetc_ndev_priv *priv = netdev_priv(ndev);
	struct net_device_stats *stats = &ndev->stats;
	unsigned long packets = 0, bytes = 0;
	int i;

	for (i = 0; i < priv->num_rx_rings; i++) {
		packets += priv->rx_ring[i]->stats.packets;
		bytes	+= priv->rx_ring[i]->stats.bytes;
	}

	stats->rx_packets = packets;
	stats->rx_bytes = bytes;
	bytes = 0;
	packets = 0;

	for (i = 0; i < priv->num_tx_rings; i++) {
		packets += priv->tx_ring[i]->stats.packets;
		bytes	+= priv->tx_ring[i]->stats.bytes;
	}

	stats->tx_packets = packets;
	stats->tx_bytes = bytes;

	return stats;
}

static int enetc_set_rss(struct net_device *ndev, int en)
{
	struct enetc_ndev_priv *priv = netdev_priv(ndev);
	struct enetc_hw *hw = &priv->si->hw;
	u32 reg;

	enetc_wr(hw, ENETC_SIRBGCR, priv->num_rx_rings);

	reg = enetc_rd(hw, ENETC_SIMR);
	reg &= ~ENETC_SIMR_RSSE;
	reg |= (en) ? ENETC_SIMR_RSSE : 0;
	enetc_wr(hw, ENETC_SIMR, reg);

	return 0;
}

static int enetc_set_psfp(struct net_device *ndev, int en)
{
	struct enetc_ndev_priv *priv = netdev_priv(ndev);
	int err;

	if (en) {
		err = enetc_psfp_enable(priv);
		if (err)
			return err;

		priv->active_offloads |= ENETC_F_QCI;
		return 0;
	}

	err = enetc_psfp_disable(priv);
	if (err)
		return err;

	priv->active_offloads &= ~ENETC_F_QCI;

	return 0;
}

static void enetc_enable_rxvlan(struct net_device *ndev, bool en)
{
	struct enetc_ndev_priv *priv = netdev_priv(ndev);
	int i;

	for (i = 0; i < priv->num_rx_rings; i++)
		enetc_bdr_enable_rxvlan(&priv->si->hw, i, en);
}

static void enetc_enable_txvlan(struct net_device *ndev, bool en)
{
	struct enetc_ndev_priv *priv = netdev_priv(ndev);
	int i;

	for (i = 0; i < priv->num_tx_rings; i++)
		enetc_bdr_enable_txvlan(&priv->si->hw, i, en);
}

int enetc_set_features(struct net_device *ndev,
		       netdev_features_t features)
{
	netdev_features_t changed = ndev->features ^ features;
	int err = 0;

	if (changed & NETIF_F_RXHASH)
		enetc_set_rss(ndev, !!(features & NETIF_F_RXHASH));

	if (changed & NETIF_F_HW_VLAN_CTAG_RX)
		enetc_enable_rxvlan(ndev,
				    !!(features & NETIF_F_HW_VLAN_CTAG_RX));

	if (changed & NETIF_F_HW_VLAN_CTAG_TX)
		enetc_enable_txvlan(ndev,
				    !!(features & NETIF_F_HW_VLAN_CTAG_TX));

	if (changed & NETIF_F_HW_TC)
		err = enetc_set_psfp(ndev, !!(features & NETIF_F_HW_TC));

	return err;
}

#ifdef CONFIG_FSL_ENETC_PTP_CLOCK
static int enetc_hwtstamp_set(struct net_device *ndev, struct ifreq *ifr)
{
	struct enetc_ndev_priv *priv = netdev_priv(ndev);
	struct hwtstamp_config config;
	int ao;

	if (copy_from_user(&config, ifr->ifr_data, sizeof(config)))
		return -EFAULT;

	switch (config.tx_type) {
	case HWTSTAMP_TX_OFF:
		priv->active_offloads &= ~ENETC_F_TX_TSTAMP_MASK;
		break;
	case HWTSTAMP_TX_ON:
		priv->active_offloads &= ~ENETC_F_TX_TSTAMP_MASK;
		priv->active_offloads |= ENETC_F_TX_TSTAMP;
		break;
	case HWTSTAMP_TX_ONESTEP_SYNC:
		priv->active_offloads &= ~ENETC_F_TX_TSTAMP_MASK;
		priv->active_offloads |= ENETC_F_TX_ONESTEP_SYNC_TSTAMP;
		break;
	default:
		return -ERANGE;
	}

	ao = priv->active_offloads;
	switch (config.rx_filter) {
	case HWTSTAMP_FILTER_NONE:
		priv->active_offloads &= ~ENETC_F_RX_TSTAMP;
		break;
	default:
		priv->active_offloads |= ENETC_F_RX_TSTAMP;
		config.rx_filter = HWTSTAMP_FILTER_ALL;
	}

	if (netif_running(ndev) && ao != priv->active_offloads) {
		enetc_close(ndev);
		enetc_open(ndev);
	}

	return copy_to_user(ifr->ifr_data, &config, sizeof(config)) ?
	       -EFAULT : 0;
}

static int enetc_hwtstamp_get(struct net_device *ndev, struct ifreq *ifr)
{
	struct enetc_ndev_priv *priv = netdev_priv(ndev);
	struct hwtstamp_config config;

	config.flags = 0;

	if (priv->active_offloads & ENETC_F_TX_ONESTEP_SYNC_TSTAMP)
		config.tx_type = HWTSTAMP_TX_ONESTEP_SYNC;
	else if (priv->active_offloads & ENETC_F_TX_TSTAMP)
		config.tx_type = HWTSTAMP_TX_ON;
	else
		config.tx_type = HWTSTAMP_TX_OFF;

	config.rx_filter = (priv->active_offloads & ENETC_F_RX_TSTAMP) ?
			    HWTSTAMP_FILTER_ALL : HWTSTAMP_FILTER_NONE;

	return copy_to_user(ifr->ifr_data, &config, sizeof(config)) ?
	       -EFAULT : 0;
}
#endif

int enetc_ioctl(struct net_device *ndev, struct ifreq *rq, int cmd)
{
	struct enetc_ndev_priv *priv = netdev_priv(ndev);
#ifdef CONFIG_FSL_ENETC_PTP_CLOCK
	if (cmd == SIOCSHWTSTAMP)
		return enetc_hwtstamp_set(ndev, rq);
	if (cmd == SIOCGHWTSTAMP)
		return enetc_hwtstamp_get(ndev, rq);
#endif

	if (!priv->phylink)
		return -EOPNOTSUPP;

	return phylink_mii_ioctl(priv->phylink, rq, cmd);
}

int enetc_alloc_msix(struct enetc_ndev_priv *priv)
{
	struct pci_dev *pdev = priv->si->pdev;
	int first_xdp_tx_ring;
	int i, n, err, nvec;
	int v_tx_rings;

	nvec = ENETC_BDR_INT_BASE_IDX + priv->bdr_int_num;
	/* allocate MSIX for both messaging and Rx/Tx interrupts */
	n = pci_alloc_irq_vectors(pdev, nvec, nvec, PCI_IRQ_MSIX);

	if (n < 0)
		return n;

	if (n != nvec)
		return -EPERM;

	/* # of tx rings per int vector */
	v_tx_rings = priv->num_tx_rings / priv->bdr_int_num;

	for (i = 0; i < priv->bdr_int_num; i++) {
		struct enetc_int_vector *v;
		struct enetc_bdr *bdr;
		int j;

		v = kzalloc(struct_size(v, tx_ring, v_tx_rings), GFP_KERNEL);
		if (!v) {
			err = -ENOMEM;
			goto fail;
		}

		priv->int_vector[i] = v;

		bdr = &v->rx_ring;
		bdr->index = i;
		bdr->ndev = priv->ndev;
		bdr->dev = priv->dev;
		bdr->bd_count = priv->rx_bd_count;
		bdr->buffer_offset = ENETC_RXB_PAD;
		priv->rx_ring[i] = bdr;

		err = xdp_rxq_info_reg(&bdr->xdp.rxq, priv->ndev, i, 0);
		if (err) {
			kfree(v);
			goto fail;
		}

		err = xdp_rxq_info_reg_mem_model(&bdr->xdp.rxq,
						 MEM_TYPE_PAGE_SHARED, NULL);
		if (err) {
			xdp_rxq_info_unreg(&bdr->xdp.rxq);
			kfree(v);
			goto fail;
		}

		/* init defaults for adaptive IC */
		if (priv->ic_mode & ENETC_IC_RX_ADAPTIVE) {
			v->rx_ictt = 0x1;
			v->rx_dim_en = true;
		}
		INIT_WORK(&v->rx_dim.work, enetc_rx_dim_work);
		netif_napi_add(priv->ndev, &v->napi, enetc_poll,
			       NAPI_POLL_WEIGHT);
		v->count_tx_rings = v_tx_rings;

		for (j = 0; j < v_tx_rings; j++) {
			int idx;

			/* default tx ring mapping policy */
			idx = priv->bdr_int_num * j + i;
			__set_bit(idx, &v->tx_rings_map);
			bdr = &v->tx_ring[j];
			bdr->index = idx;
			bdr->ndev = priv->ndev;
			bdr->dev = priv->dev;
			bdr->bd_count = priv->tx_bd_count;
			priv->tx_ring[idx] = bdr;
		}
	}

	first_xdp_tx_ring = priv->num_tx_rings - num_possible_cpus();
	priv->xdp_tx_ring = &priv->tx_ring[first_xdp_tx_ring];

	return 0;

fail:
	while (i--) {
		struct enetc_int_vector *v = priv->int_vector[i];
		struct enetc_bdr *rx_ring = &v->rx_ring;

		xdp_rxq_info_unreg_mem_model(&rx_ring->xdp.rxq);
		xdp_rxq_info_unreg(&rx_ring->xdp.rxq);
		netif_napi_del(&v->napi);
		cancel_work_sync(&v->rx_dim.work);
		kfree(v);
	}

	pci_free_irq_vectors(pdev);

	return err;
}

void enetc_free_msix(struct enetc_ndev_priv *priv)
{
	int i;

	for (i = 0; i < priv->bdr_int_num; i++) {
		struct enetc_int_vector *v = priv->int_vector[i];
		struct enetc_bdr *rx_ring = &v->rx_ring;

		xdp_rxq_info_unreg_mem_model(&rx_ring->xdp.rxq);
		xdp_rxq_info_unreg(&rx_ring->xdp.rxq);
		netif_napi_del(&v->napi);
		cancel_work_sync(&v->rx_dim.work);
	}

	for (i = 0; i < priv->num_rx_rings; i++)
		priv->rx_ring[i] = NULL;

	for (i = 0; i < priv->num_tx_rings; i++)
		priv->tx_ring[i] = NULL;

	for (i = 0; i < priv->bdr_int_num; i++) {
		kfree(priv->int_vector[i]);
		priv->int_vector[i] = NULL;
	}

	/* disable all MSIX for this device */
	pci_free_irq_vectors(priv->si->pdev);
}

static void enetc_kfree_si(struct enetc_si *si)
{
	char *p = (char *)si - si->pad;

	kfree(p);
}

static void enetc_detect_errata(struct enetc_si *si)
{
	if (si->pdev->revision == ENETC_REV1)
		si->errata = ENETC_ERR_VLAN_ISOL | ENETC_ERR_UCMCSWP;
}

int enetc_pci_probe(struct pci_dev *pdev, const char *name, int sizeof_priv)
{
	struct enetc_si *si, *p;
	struct enetc_hw *hw;
	size_t alloc_size;
	int err, len;

	pcie_flr(pdev);
	err = pci_enable_device_mem(pdev);
	if (err) {
		dev_err(&pdev->dev, "device enable failed\n");
		return err;
	}

	/* set up for high or low dma */
	err = dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(64));
	if (err) {
		err = dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(32));
		if (err) {
			dev_err(&pdev->dev,
				"DMA configuration failed: 0x%x\n", err);
			goto err_dma;
		}
	}

	err = pci_request_mem_regions(pdev, name);
	if (err) {
		dev_err(&pdev->dev, "pci_request_regions failed err=%d\n", err);
		goto err_pci_mem_reg;
	}

	pci_set_master(pdev);

	alloc_size = sizeof(struct enetc_si);
	if (sizeof_priv) {
		/* align priv to 32B */
		alloc_size = ALIGN(alloc_size, ENETC_SI_ALIGN);
		alloc_size += sizeof_priv;
	}
	/* force 32B alignment for enetc_si */
	alloc_size += ENETC_SI_ALIGN - 1;

	p = kzalloc(alloc_size, GFP_KERNEL);
	if (!p) {
		err = -ENOMEM;
		goto err_alloc_si;
	}

	si = PTR_ALIGN(p, ENETC_SI_ALIGN);
	si->pad = (char *)si - (char *)p;

	pci_set_drvdata(pdev, si);
	si->pdev = pdev;
	hw = &si->hw;

	len = pci_resource_len(pdev, ENETC_BAR_REGS);
	hw->reg = ioremap(pci_resource_start(pdev, ENETC_BAR_REGS), len);
	if (!hw->reg) {
		err = -ENXIO;
		dev_err(&pdev->dev, "ioremap() failed\n");
		goto err_ioremap;
	}
	if (len > ENETC_PORT_BASE)
		hw->port = hw->reg + ENETC_PORT_BASE;
	if (len > ENETC_GLOBAL_BASE)
		hw->global = hw->reg + ENETC_GLOBAL_BASE;

	enetc_detect_errata(si);

	return 0;

err_ioremap:
	enetc_kfree_si(si);
err_alloc_si:
	pci_release_mem_regions(pdev);
err_pci_mem_reg:
err_dma:
	pci_disable_device(pdev);

	return err;
}

void enetc_pci_remove(struct pci_dev *pdev)
{
	struct enetc_si *si = pci_get_drvdata(pdev);
	struct enetc_hw *hw = &si->hw;

	iounmap(hw->reg);
	enetc_kfree_si(si);
	pci_release_mem_regions(pdev);
	pci_disable_device(pdev);
}
