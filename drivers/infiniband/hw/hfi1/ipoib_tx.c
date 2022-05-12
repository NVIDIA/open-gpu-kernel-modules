// SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause)
/*
 * Copyright(c) 2020 Intel Corporation.
 *
 */

/*
 * This file contains HFI1 support for IPOIB SDMA functionality
 */

#include <linux/log2.h>
#include <linux/circ_buf.h>

#include "sdma.h"
#include "verbs.h"
#include "trace_ibhdrs.h"
#include "ipoib.h"
#include "trace_tx.h"

/* Add a convenience helper */
#define CIRC_ADD(val, add, size) (((val) + (add)) & ((size) - 1))
#define CIRC_NEXT(val, size) CIRC_ADD(val, 1, size)
#define CIRC_PREV(val, size) CIRC_ADD(val, -1, size)

/**
 * struct ipoib_txreq - IPOIB transmit descriptor
 * @txreq: sdma transmit request
 * @sdma_hdr: 9b ib headers
 * @sdma_status: status returned by sdma engine
 * @priv: ipoib netdev private data
 * @txq: txq on which skb was output
 * @skb: skb to send
 */
struct ipoib_txreq {
	struct sdma_txreq           txreq;
	struct hfi1_sdma_header     sdma_hdr;
	int                         sdma_status;
	struct hfi1_ipoib_dev_priv *priv;
	struct hfi1_ipoib_txq      *txq;
	struct sk_buff             *skb;
};

struct ipoib_txparms {
	struct hfi1_devdata        *dd;
	struct rdma_ah_attr        *ah_attr;
	struct hfi1_ibport         *ibp;
	struct hfi1_ipoib_txq      *txq;
	union hfi1_ipoib_flow       flow;
	u32                         dqpn;
	u8                          hdr_dwords;
	u8                          entropy;
};

static u64 hfi1_ipoib_txreqs(const u64 sent, const u64 completed)
{
	return sent - completed;
}

static u64 hfi1_ipoib_used(struct hfi1_ipoib_txq *txq)
{
	return hfi1_ipoib_txreqs(txq->sent_txreqs,
				 atomic64_read(&txq->complete_txreqs));
}

static void hfi1_ipoib_stop_txq(struct hfi1_ipoib_txq *txq)
{
	trace_hfi1_txq_stop(txq);
	if (atomic_inc_return(&txq->stops) == 1)
		netif_stop_subqueue(txq->priv->netdev, txq->q_idx);
}

static void hfi1_ipoib_wake_txq(struct hfi1_ipoib_txq *txq)
{
	trace_hfi1_txq_wake(txq);
	if (atomic_dec_and_test(&txq->stops))
		netif_wake_subqueue(txq->priv->netdev, txq->q_idx);
}

static uint hfi1_ipoib_ring_hwat(struct hfi1_ipoib_txq *txq)
{
	return min_t(uint, txq->priv->netdev->tx_queue_len,
		     txq->tx_ring.max_items - 1);
}

static uint hfi1_ipoib_ring_lwat(struct hfi1_ipoib_txq *txq)
{
	return min_t(uint, txq->priv->netdev->tx_queue_len,
		     txq->tx_ring.max_items) >> 1;
}

static void hfi1_ipoib_check_queue_depth(struct hfi1_ipoib_txq *txq)
{
	++txq->sent_txreqs;
	if (hfi1_ipoib_used(txq) >= hfi1_ipoib_ring_hwat(txq) &&
	    !atomic_xchg(&txq->ring_full, 1)) {
		trace_hfi1_txq_full(txq);
		hfi1_ipoib_stop_txq(txq);
	}
}

static void hfi1_ipoib_check_queue_stopped(struct hfi1_ipoib_txq *txq)
{
	struct net_device *dev = txq->priv->netdev;

	/* If shutting down just return as queue state is irrelevant */
	if (unlikely(dev->reg_state != NETREG_REGISTERED))
		return;

	/*
	 * When the queue has been drained to less than half full it will be
	 * restarted.
	 * The size of the txreq ring is fixed at initialization.
	 * The tx queue len can be adjusted upward while the interface is
	 * running.
	 * The tx queue len can be large enough to overflow the txreq_ring.
	 * Use the minimum of the current tx_queue_len or the rings max txreqs
	 * to protect against ring overflow.
	 */
	if (hfi1_ipoib_used(txq) < hfi1_ipoib_ring_lwat(txq) &&
	    atomic_xchg(&txq->ring_full, 0)) {
		trace_hfi1_txq_xmit_unstopped(txq);
		hfi1_ipoib_wake_txq(txq);
	}
}

static void hfi1_ipoib_free_tx(struct ipoib_txreq *tx, int budget)
{
	struct hfi1_ipoib_dev_priv *priv = tx->priv;

	if (likely(!tx->sdma_status)) {
		dev_sw_netstats_tx_add(priv->netdev, 1, tx->skb->len);
	} else {
		++priv->netdev->stats.tx_errors;
		dd_dev_warn(priv->dd,
			    "%s: Status = 0x%x pbc 0x%llx txq = %d sde = %d\n",
			    __func__, tx->sdma_status,
			    le64_to_cpu(tx->sdma_hdr.pbc), tx->txq->q_idx,
			    tx->txq->sde->this_idx);
	}

	napi_consume_skb(tx->skb, budget);
	sdma_txclean(priv->dd, &tx->txreq);
	kmem_cache_free(priv->txreq_cache, tx);
}

static int hfi1_ipoib_drain_tx_ring(struct hfi1_ipoib_txq *txq, int budget)
{
	struct hfi1_ipoib_circ_buf *tx_ring = &txq->tx_ring;
	unsigned long head;
	unsigned long tail;
	unsigned int max_tx;
	int work_done;
	int tx_count;

	spin_lock_bh(&tx_ring->consumer_lock);

	/* Read index before reading contents at that index. */
	head = smp_load_acquire(&tx_ring->head);
	tail = tx_ring->tail;
	max_tx = tx_ring->max_items;

	work_done = min_t(int, CIRC_CNT(head, tail, max_tx), budget);

	for (tx_count = work_done; tx_count; tx_count--) {
		hfi1_ipoib_free_tx(tx_ring->items[tail], budget);
		tail = CIRC_NEXT(tail, max_tx);
	}

	atomic64_add(work_done, &txq->complete_txreqs);

	/* Finished freeing tx items so store the tail value. */
	smp_store_release(&tx_ring->tail, tail);

	spin_unlock_bh(&tx_ring->consumer_lock);

	hfi1_ipoib_check_queue_stopped(txq);

	return work_done;
}

static int hfi1_ipoib_process_tx_ring(struct napi_struct *napi, int budget)
{
	struct hfi1_ipoib_dev_priv *priv = hfi1_ipoib_priv(napi->dev);
	struct hfi1_ipoib_txq *txq = &priv->txqs[napi - priv->tx_napis];

	int work_done = hfi1_ipoib_drain_tx_ring(txq, budget);

	if (work_done < budget)
		napi_complete_done(napi, work_done);

	return work_done;
}

static void hfi1_ipoib_add_tx(struct ipoib_txreq *tx)
{
	struct hfi1_ipoib_circ_buf *tx_ring = &tx->txq->tx_ring;
	unsigned long head;
	unsigned long tail;
	size_t max_tx;

	spin_lock(&tx_ring->producer_lock);

	head = tx_ring->head;
	tail = READ_ONCE(tx_ring->tail);
	max_tx = tx_ring->max_items;

	if (likely(CIRC_SPACE(head, tail, max_tx))) {
		tx_ring->items[head] = tx;

		/* Finish storing txreq before incrementing head. */
		smp_store_release(&tx_ring->head, CIRC_ADD(head, 1, max_tx));
		napi_schedule_irqoff(tx->txq->napi);
	} else {
		struct hfi1_ipoib_txq *txq = tx->txq;
		struct hfi1_ipoib_dev_priv *priv = tx->priv;

		/* Ring was full */
		hfi1_ipoib_free_tx(tx, 0);
		atomic64_inc(&txq->complete_txreqs);
		dd_dev_dbg(priv->dd, "txq %d full.\n", txq->q_idx);
	}

	spin_unlock(&tx_ring->producer_lock);
}

static void hfi1_ipoib_sdma_complete(struct sdma_txreq *txreq, int status)
{
	struct ipoib_txreq *tx = container_of(txreq, struct ipoib_txreq, txreq);

	tx->sdma_status = status;

	hfi1_ipoib_add_tx(tx);
}

static int hfi1_ipoib_build_ulp_payload(struct ipoib_txreq *tx,
					struct ipoib_txparms *txp)
{
	struct hfi1_devdata *dd = txp->dd;
	struct sdma_txreq *txreq = &tx->txreq;
	struct sk_buff *skb = tx->skb;
	int ret = 0;
	int i;

	if (skb_headlen(skb)) {
		ret = sdma_txadd_kvaddr(dd, txreq, skb->data, skb_headlen(skb));
		if (unlikely(ret))
			return ret;
	}

	for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
		const skb_frag_t *frag = &skb_shinfo(skb)->frags[i];

		ret = sdma_txadd_page(dd,
				      txreq,
				      skb_frag_page(frag),
				      frag->bv_offset,
				      skb_frag_size(frag));
		if (unlikely(ret))
			break;
	}

	return ret;
}

static int hfi1_ipoib_build_tx_desc(struct ipoib_txreq *tx,
				    struct ipoib_txparms *txp)
{
	struct hfi1_devdata *dd = txp->dd;
	struct sdma_txreq *txreq = &tx->txreq;
	struct hfi1_sdma_header *sdma_hdr = &tx->sdma_hdr;
	u16 pkt_bytes =
		sizeof(sdma_hdr->pbc) + (txp->hdr_dwords << 2) + tx->skb->len;
	int ret;

	ret = sdma_txinit(txreq, 0, pkt_bytes, hfi1_ipoib_sdma_complete);
	if (unlikely(ret))
		return ret;

	/* add pbc + headers */
	ret = sdma_txadd_kvaddr(dd,
				txreq,
				sdma_hdr,
				sizeof(sdma_hdr->pbc) + (txp->hdr_dwords << 2));
	if (unlikely(ret))
		return ret;

	/* add the ulp payload */
	return hfi1_ipoib_build_ulp_payload(tx, txp);
}

static void hfi1_ipoib_build_ib_tx_headers(struct ipoib_txreq *tx,
					   struct ipoib_txparms *txp)
{
	struct hfi1_ipoib_dev_priv *priv = tx->priv;
	struct hfi1_sdma_header *sdma_hdr = &tx->sdma_hdr;
	struct sk_buff *skb = tx->skb;
	struct hfi1_pportdata *ppd = ppd_from_ibp(txp->ibp);
	struct rdma_ah_attr *ah_attr = txp->ah_attr;
	struct ib_other_headers *ohdr;
	struct ib_grh *grh;
	u16 dwords;
	u16 slid;
	u16 dlid;
	u16 lrh0;
	u32 bth0;
	u32 sqpn = (u32)(priv->netdev->dev_addr[1] << 16 |
			 priv->netdev->dev_addr[2] << 8 |
			 priv->netdev->dev_addr[3]);
	u16 payload_dwords;
	u8 pad_cnt;

	pad_cnt = -skb->len & 3;

	/* Includes ICRC */
	payload_dwords = ((skb->len + pad_cnt) >> 2) + SIZE_OF_CRC;

	/* header size in dwords LRH+BTH+DETH = (8+12+8)/4. */
	txp->hdr_dwords = 7;

	if (rdma_ah_get_ah_flags(ah_attr) & IB_AH_GRH) {
		grh = &sdma_hdr->hdr.ibh.u.l.grh;
		txp->hdr_dwords +=
			hfi1_make_grh(txp->ibp,
				      grh,
				      rdma_ah_read_grh(ah_attr),
				      txp->hdr_dwords - LRH_9B_DWORDS,
				      payload_dwords);
		lrh0 = HFI1_LRH_GRH;
		ohdr = &sdma_hdr->hdr.ibh.u.l.oth;
	} else {
		lrh0 = HFI1_LRH_BTH;
		ohdr = &sdma_hdr->hdr.ibh.u.oth;
	}

	lrh0 |= (rdma_ah_get_sl(ah_attr) & 0xf) << 4;
	lrh0 |= (txp->flow.sc5 & 0xf) << 12;

	dlid = opa_get_lid(rdma_ah_get_dlid(ah_attr), 9B);
	if (dlid == be16_to_cpu(IB_LID_PERMISSIVE)) {
		slid = be16_to_cpu(IB_LID_PERMISSIVE);
	} else {
		u16 lid = (u16)ppd->lid;

		if (lid) {
			lid |= rdma_ah_get_path_bits(ah_attr) &
				((1 << ppd->lmc) - 1);
			slid = lid;
		} else {
			slid = be16_to_cpu(IB_LID_PERMISSIVE);
		}
	}

	/* Includes ICRC */
	dwords = txp->hdr_dwords + payload_dwords;

	/* Build the lrh */
	sdma_hdr->hdr.hdr_type = HFI1_PKT_TYPE_9B;
	hfi1_make_ib_hdr(&sdma_hdr->hdr.ibh, lrh0, dwords, dlid, slid);

	/* Build the bth */
	bth0 = (IB_OPCODE_UD_SEND_ONLY << 24) | (pad_cnt << 20) | priv->pkey;

	ohdr->bth[0] = cpu_to_be32(bth0);
	ohdr->bth[1] = cpu_to_be32(txp->dqpn);
	ohdr->bth[2] = cpu_to_be32(mask_psn((u32)txp->txq->sent_txreqs));

	/* Build the deth */
	ohdr->u.ud.deth[0] = cpu_to_be32(priv->qkey);
	ohdr->u.ud.deth[1] = cpu_to_be32((txp->entropy <<
					  HFI1_IPOIB_ENTROPY_SHIFT) | sqpn);

	/* Construct the pbc. */
	sdma_hdr->pbc =
		cpu_to_le64(create_pbc(ppd,
				       ib_is_sc5(txp->flow.sc5) <<
							      PBC_DC_INFO_SHIFT,
				       0,
				       sc_to_vlt(priv->dd, txp->flow.sc5),
				       dwords - SIZE_OF_CRC +
						(sizeof(sdma_hdr->pbc) >> 2)));
}

static struct ipoib_txreq *hfi1_ipoib_send_dma_common(struct net_device *dev,
						      struct sk_buff *skb,
						      struct ipoib_txparms *txp)
{
	struct hfi1_ipoib_dev_priv *priv = hfi1_ipoib_priv(dev);
	struct ipoib_txreq *tx;
	int ret;

	tx = kmem_cache_alloc_node(priv->txreq_cache,
				   GFP_ATOMIC,
				   priv->dd->node);
	if (unlikely(!tx))
		return ERR_PTR(-ENOMEM);

	/* so that we can test if the sdma descriptors are there */
	tx->txreq.num_desc = 0;
	tx->priv = priv;
	tx->txq = txp->txq;
	tx->skb = skb;
	INIT_LIST_HEAD(&tx->txreq.list);

	hfi1_ipoib_build_ib_tx_headers(tx, txp);

	ret = hfi1_ipoib_build_tx_desc(tx, txp);
	if (likely(!ret)) {
		if (txp->txq->flow.as_int != txp->flow.as_int) {
			txp->txq->flow.tx_queue = txp->flow.tx_queue;
			txp->txq->flow.sc5 = txp->flow.sc5;
			txp->txq->sde =
				sdma_select_engine_sc(priv->dd,
						      txp->flow.tx_queue,
						      txp->flow.sc5);
			trace_hfi1_flow_switch(txp->txq);
		}

		return tx;
	}

	sdma_txclean(priv->dd, &tx->txreq);
	kmem_cache_free(priv->txreq_cache, tx);

	return ERR_PTR(ret);
}

static int hfi1_ipoib_submit_tx_list(struct net_device *dev,
				     struct hfi1_ipoib_txq *txq)
{
	int ret;
	u16 count_out;

	ret = sdma_send_txlist(txq->sde,
			       iowait_get_ib_work(&txq->wait),
			       &txq->tx_list,
			       &count_out);
	if (likely(!ret) || ret == -EBUSY || ret == -ECOMM)
		return ret;

	dd_dev_warn(txq->priv->dd, "cannot send skb tx list, err %d.\n", ret);

	return ret;
}

static int hfi1_ipoib_flush_tx_list(struct net_device *dev,
				    struct hfi1_ipoib_txq *txq)
{
	int ret = 0;

	if (!list_empty(&txq->tx_list)) {
		/* Flush the current list */
		ret = hfi1_ipoib_submit_tx_list(dev, txq);

		if (unlikely(ret))
			if (ret != -EBUSY)
				++dev->stats.tx_carrier_errors;
	}

	return ret;
}

static int hfi1_ipoib_submit_tx(struct hfi1_ipoib_txq *txq,
				struct ipoib_txreq *tx)
{
	int ret;

	ret = sdma_send_txreq(txq->sde,
			      iowait_get_ib_work(&txq->wait),
			      &tx->txreq,
			      txq->pkts_sent);
	if (likely(!ret)) {
		txq->pkts_sent = true;
		iowait_starve_clear(txq->pkts_sent, &txq->wait);
	}

	return ret;
}

static int hfi1_ipoib_send_dma_single(struct net_device *dev,
				      struct sk_buff *skb,
				      struct ipoib_txparms *txp)
{
	struct hfi1_ipoib_dev_priv *priv = hfi1_ipoib_priv(dev);
	struct hfi1_ipoib_txq *txq = txp->txq;
	struct ipoib_txreq *tx;
	int ret;

	tx = hfi1_ipoib_send_dma_common(dev, skb, txp);
	if (IS_ERR(tx)) {
		int ret = PTR_ERR(tx);

		dev_kfree_skb_any(skb);

		if (ret == -ENOMEM)
			++dev->stats.tx_errors;
		else
			++dev->stats.tx_carrier_errors;

		return NETDEV_TX_OK;
	}

	ret = hfi1_ipoib_submit_tx(txq, tx);
	if (likely(!ret)) {
tx_ok:
		trace_sdma_output_ibhdr(tx->priv->dd,
					&tx->sdma_hdr.hdr,
					ib_is_sc5(txp->flow.sc5));
		hfi1_ipoib_check_queue_depth(txq);
		return NETDEV_TX_OK;
	}

	txq->pkts_sent = false;

	if (ret == -EBUSY || ret == -ECOMM)
		goto tx_ok;

	sdma_txclean(priv->dd, &tx->txreq);
	dev_kfree_skb_any(skb);
	kmem_cache_free(priv->txreq_cache, tx);
	++dev->stats.tx_carrier_errors;

	return NETDEV_TX_OK;
}

static int hfi1_ipoib_send_dma_list(struct net_device *dev,
				    struct sk_buff *skb,
				    struct ipoib_txparms *txp)
{
	struct hfi1_ipoib_txq *txq = txp->txq;
	struct ipoib_txreq *tx;

	/* Has the flow change ? */
	if (txq->flow.as_int != txp->flow.as_int) {
		int ret;

		trace_hfi1_flow_flush(txq);
		ret = hfi1_ipoib_flush_tx_list(dev, txq);
		if (unlikely(ret)) {
			if (ret == -EBUSY)
				++dev->stats.tx_dropped;
			dev_kfree_skb_any(skb);
			return NETDEV_TX_OK;
		}
	}
	tx = hfi1_ipoib_send_dma_common(dev, skb, txp);
	if (IS_ERR(tx)) {
		int ret = PTR_ERR(tx);

		dev_kfree_skb_any(skb);

		if (ret == -ENOMEM)
			++dev->stats.tx_errors;
		else
			++dev->stats.tx_carrier_errors;

		return NETDEV_TX_OK;
	}

	list_add_tail(&tx->txreq.list, &txq->tx_list);

	hfi1_ipoib_check_queue_depth(txq);

	trace_sdma_output_ibhdr(tx->priv->dd,
				&tx->sdma_hdr.hdr,
				ib_is_sc5(txp->flow.sc5));

	if (!netdev_xmit_more())
		(void)hfi1_ipoib_flush_tx_list(dev, txq);

	return NETDEV_TX_OK;
}

static u8 hfi1_ipoib_calc_entropy(struct sk_buff *skb)
{
	if (skb_transport_header_was_set(skb)) {
		u8 *hdr = (u8 *)skb_transport_header(skb);

		return (hdr[0] ^ hdr[1] ^ hdr[2] ^ hdr[3]);
	}

	return (u8)skb_get_queue_mapping(skb);
}

int hfi1_ipoib_send(struct net_device *dev,
		    struct sk_buff *skb,
		    struct ib_ah *address,
		    u32 dqpn)
{
	struct hfi1_ipoib_dev_priv *priv = hfi1_ipoib_priv(dev);
	struct ipoib_txparms txp;
	struct rdma_netdev *rn = netdev_priv(dev);

	if (unlikely(skb->len > rn->mtu + HFI1_IPOIB_ENCAP_LEN)) {
		dd_dev_warn(priv->dd, "packet len %d (> %d) too long to send, dropping\n",
			    skb->len,
			    rn->mtu + HFI1_IPOIB_ENCAP_LEN);
		++dev->stats.tx_dropped;
		++dev->stats.tx_errors;
		dev_kfree_skb_any(skb);
		return NETDEV_TX_OK;
	}

	txp.dd = priv->dd;
	txp.ah_attr = &ibah_to_rvtah(address)->attr;
	txp.ibp = to_iport(priv->device, priv->port_num);
	txp.txq = &priv->txqs[skb_get_queue_mapping(skb)];
	txp.dqpn = dqpn;
	txp.flow.sc5 = txp.ibp->sl_to_sc[rdma_ah_get_sl(txp.ah_attr)];
	txp.flow.tx_queue = (u8)skb_get_queue_mapping(skb);
	txp.entropy = hfi1_ipoib_calc_entropy(skb);

	if (netdev_xmit_more() || !list_empty(&txp.txq->tx_list))
		return hfi1_ipoib_send_dma_list(dev, skb, &txp);

	return hfi1_ipoib_send_dma_single(dev, skb,  &txp);
}

/*
 * hfi1_ipoib_sdma_sleep - ipoib sdma sleep function
 *
 * This function gets called from sdma_send_txreq() when there are not enough
 * sdma descriptors available to send the packet. It adds Tx queue's wait
 * structure to sdma engine's dmawait list to be woken up when descriptors
 * become available.
 */
static int hfi1_ipoib_sdma_sleep(struct sdma_engine *sde,
				 struct iowait_work *wait,
				 struct sdma_txreq *txreq,
				 uint seq,
				 bool pkts_sent)
{
	struct hfi1_ipoib_txq *txq =
		container_of(wait->iow, struct hfi1_ipoib_txq, wait);

	write_seqlock(&sde->waitlock);

	if (likely(txq->priv->netdev->reg_state == NETREG_REGISTERED)) {
		if (sdma_progress(sde, seq, txreq)) {
			write_sequnlock(&sde->waitlock);
			return -EAGAIN;
		}

		if (list_empty(&txreq->list))
			/* came from non-list submit */
			list_add_tail(&txreq->list, &txq->tx_list);
		if (list_empty(&txq->wait.list)) {
			if (!atomic_xchg(&txq->no_desc, 1)) {
				trace_hfi1_txq_queued(txq);
				hfi1_ipoib_stop_txq(txq);
			}
			iowait_queue(pkts_sent, wait->iow, &sde->dmawait);
		}

		write_sequnlock(&sde->waitlock);
		return -EBUSY;
	}

	write_sequnlock(&sde->waitlock);
	return -EINVAL;
}

/*
 * hfi1_ipoib_sdma_wakeup - ipoib sdma wakeup function
 *
 * This function gets called when SDMA descriptors becomes available and Tx
 * queue's wait structure was previously added to sdma engine's dmawait list.
 */
static void hfi1_ipoib_sdma_wakeup(struct iowait *wait, int reason)
{
	struct hfi1_ipoib_txq *txq =
		container_of(wait, struct hfi1_ipoib_txq, wait);

	trace_hfi1_txq_wakeup(txq);
	if (likely(txq->priv->netdev->reg_state == NETREG_REGISTERED))
		iowait_schedule(wait, system_highpri_wq, WORK_CPU_UNBOUND);
}

static void hfi1_ipoib_flush_txq(struct work_struct *work)
{
	struct iowait_work *ioww =
		container_of(work, struct iowait_work, iowork);
	struct iowait *wait = iowait_ioww_to_iow(ioww);
	struct hfi1_ipoib_txq *txq =
		container_of(wait, struct hfi1_ipoib_txq, wait);
	struct net_device *dev = txq->priv->netdev;

	if (likely(dev->reg_state == NETREG_REGISTERED) &&
	    likely(!hfi1_ipoib_flush_tx_list(dev, txq)))
		if (atomic_xchg(&txq->no_desc, 0))
			hfi1_ipoib_wake_txq(txq);
}

int hfi1_ipoib_txreq_init(struct hfi1_ipoib_dev_priv *priv)
{
	struct net_device *dev = priv->netdev;
	char buf[HFI1_IPOIB_TXREQ_NAME_LEN];
	unsigned long tx_ring_size;
	int i;

	/*
	 * Ring holds 1 less than tx_ring_size
	 * Round up to next power of 2 in order to hold at least tx_queue_len
	 */
	tx_ring_size = roundup_pow_of_two((unsigned long)dev->tx_queue_len + 1);

	snprintf(buf, sizeof(buf), "hfi1_%u_ipoib_txreq_cache", priv->dd->unit);
	priv->txreq_cache = kmem_cache_create(buf,
					      sizeof(struct ipoib_txreq),
					      0,
					      0,
					      NULL);
	if (!priv->txreq_cache)
		return -ENOMEM;

	priv->tx_napis = kcalloc_node(dev->num_tx_queues,
				      sizeof(struct napi_struct),
				      GFP_KERNEL,
				      priv->dd->node);
	if (!priv->tx_napis)
		goto free_txreq_cache;

	priv->txqs = kcalloc_node(dev->num_tx_queues,
				  sizeof(struct hfi1_ipoib_txq),
				  GFP_KERNEL,
				  priv->dd->node);
	if (!priv->txqs)
		goto free_tx_napis;

	for (i = 0; i < dev->num_tx_queues; i++) {
		struct hfi1_ipoib_txq *txq = &priv->txqs[i];

		iowait_init(&txq->wait,
			    0,
			    hfi1_ipoib_flush_txq,
			    NULL,
			    hfi1_ipoib_sdma_sleep,
			    hfi1_ipoib_sdma_wakeup,
			    NULL,
			    NULL);
		txq->priv = priv;
		txq->sde = NULL;
		INIT_LIST_HEAD(&txq->tx_list);
		atomic64_set(&txq->complete_txreqs, 0);
		atomic_set(&txq->stops, 0);
		atomic_set(&txq->ring_full, 0);
		atomic_set(&txq->no_desc, 0);
		txq->q_idx = i;
		txq->flow.tx_queue = 0xff;
		txq->flow.sc5 = 0xff;
		txq->pkts_sent = false;

		netdev_queue_numa_node_write(netdev_get_tx_queue(dev, i),
					     priv->dd->node);

		txq->tx_ring.items =
			kcalloc_node(tx_ring_size,
				     sizeof(struct ipoib_txreq *),
				     GFP_KERNEL, priv->dd->node);
		if (!txq->tx_ring.items)
			goto free_txqs;

		spin_lock_init(&txq->tx_ring.producer_lock);
		spin_lock_init(&txq->tx_ring.consumer_lock);
		txq->tx_ring.max_items = tx_ring_size;

		txq->napi = &priv->tx_napis[i];
		netif_tx_napi_add(dev, txq->napi,
				  hfi1_ipoib_process_tx_ring,
				  NAPI_POLL_WEIGHT);
	}

	return 0;

free_txqs:
	for (i--; i >= 0; i--) {
		struct hfi1_ipoib_txq *txq = &priv->txqs[i];

		netif_napi_del(txq->napi);
		kfree(txq->tx_ring.items);
	}

	kfree(priv->txqs);
	priv->txqs = NULL;

free_tx_napis:
	kfree(priv->tx_napis);
	priv->tx_napis = NULL;

free_txreq_cache:
	kmem_cache_destroy(priv->txreq_cache);
	priv->txreq_cache = NULL;
	return -ENOMEM;
}

static void hfi1_ipoib_drain_tx_list(struct hfi1_ipoib_txq *txq)
{
	struct sdma_txreq *txreq;
	struct sdma_txreq *txreq_tmp;
	atomic64_t *complete_txreqs = &txq->complete_txreqs;

	list_for_each_entry_safe(txreq, txreq_tmp, &txq->tx_list, list) {
		struct ipoib_txreq *tx =
			container_of(txreq, struct ipoib_txreq, txreq);

		list_del(&txreq->list);
		sdma_txclean(txq->priv->dd, &tx->txreq);
		dev_kfree_skb_any(tx->skb);
		kmem_cache_free(txq->priv->txreq_cache, tx);
		atomic64_inc(complete_txreqs);
	}

	if (hfi1_ipoib_used(txq))
		dd_dev_warn(txq->priv->dd,
			    "txq %d not empty found %llu requests\n",
			    txq->q_idx,
			    hfi1_ipoib_txreqs(txq->sent_txreqs,
					      atomic64_read(complete_txreqs)));
}

void hfi1_ipoib_txreq_deinit(struct hfi1_ipoib_dev_priv *priv)
{
	int i;

	for (i = 0; i < priv->netdev->num_tx_queues; i++) {
		struct hfi1_ipoib_txq *txq = &priv->txqs[i];

		iowait_cancel_work(&txq->wait);
		iowait_sdma_drain(&txq->wait);
		hfi1_ipoib_drain_tx_list(txq);
		netif_napi_del(txq->napi);
		(void)hfi1_ipoib_drain_tx_ring(txq, txq->tx_ring.max_items);
		kfree(txq->tx_ring.items);
	}

	kfree(priv->txqs);
	priv->txqs = NULL;

	kfree(priv->tx_napis);
	priv->tx_napis = NULL;

	kmem_cache_destroy(priv->txreq_cache);
	priv->txreq_cache = NULL;
}

void hfi1_ipoib_napi_tx_enable(struct net_device *dev)
{
	struct hfi1_ipoib_dev_priv *priv = hfi1_ipoib_priv(dev);
	int i;

	for (i = 0; i < dev->num_tx_queues; i++) {
		struct hfi1_ipoib_txq *txq = &priv->txqs[i];

		napi_enable(txq->napi);
	}
}

void hfi1_ipoib_napi_tx_disable(struct net_device *dev)
{
	struct hfi1_ipoib_dev_priv *priv = hfi1_ipoib_priv(dev);
	int i;

	for (i = 0; i < dev->num_tx_queues; i++) {
		struct hfi1_ipoib_txq *txq = &priv->txqs[i];

		napi_disable(txq->napi);
		(void)hfi1_ipoib_drain_tx_ring(txq, txq->tx_ring.max_items);
	}
}

void hfi1_ipoib_tx_timeout(struct net_device *dev, unsigned int q)
{
	struct hfi1_ipoib_dev_priv *priv = hfi1_ipoib_priv(dev);
	struct hfi1_ipoib_txq *txq = &priv->txqs[q];
	u64 completed = atomic64_read(&txq->complete_txreqs);

	dd_dev_info(priv->dd, "timeout txq %llx q %u stopped %u stops %d no_desc %d ring_full %d\n",
		    (unsigned long long)txq, q,
		    __netif_subqueue_stopped(dev, txq->q_idx),
		    atomic_read(&txq->stops),
		    atomic_read(&txq->no_desc),
		    atomic_read(&txq->ring_full));
	dd_dev_info(priv->dd, "sde %llx engine %u\n",
		    (unsigned long long)txq->sde,
		    txq->sde ? txq->sde->this_idx : 0);
	dd_dev_info(priv->dd, "flow %x\n", txq->flow.as_int);
	dd_dev_info(priv->dd, "sent %llu completed %llu used %llu\n",
		    txq->sent_txreqs, completed, hfi1_ipoib_used(txq));
	dd_dev_info(priv->dd, "tx_queue_len %u max_items %lu\n",
		    dev->tx_queue_len, txq->tx_ring.max_items);
	dd_dev_info(priv->dd, "head %lu tail %lu\n",
		    txq->tx_ring.head, txq->tx_ring.tail);
	dd_dev_info(priv->dd, "wait queued %u\n",
		    !list_empty(&txq->wait.list));
	dd_dev_info(priv->dd, "tx_list empty %u\n",
		    list_empty(&txq->tx_list));
}

