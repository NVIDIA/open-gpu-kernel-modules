// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2018 Intel Corporation. */

#include <linux/bpf_trace.h>
#include <linux/stringify.h>
#include <net/xdp_sock_drv.h>
#include <net/xdp.h>

#include "i40e.h"
#include "i40e_txrx_common.h"
#include "i40e_xsk.h"

int i40e_alloc_rx_bi_zc(struct i40e_ring *rx_ring)
{
	unsigned long sz = sizeof(*rx_ring->rx_bi_zc) * rx_ring->count;

	rx_ring->rx_bi_zc = kzalloc(sz, GFP_KERNEL);
	return rx_ring->rx_bi_zc ? 0 : -ENOMEM;
}

void i40e_clear_rx_bi_zc(struct i40e_ring *rx_ring)
{
	memset(rx_ring->rx_bi_zc, 0,
	       sizeof(*rx_ring->rx_bi_zc) * rx_ring->count);
}

static struct xdp_buff **i40e_rx_bi(struct i40e_ring *rx_ring, u32 idx)
{
	return &rx_ring->rx_bi_zc[idx];
}

/**
 * i40e_xsk_pool_enable - Enable/associate an AF_XDP buffer pool to a
 * certain ring/qid
 * @vsi: Current VSI
 * @pool: buffer pool
 * @qid: Rx ring to associate buffer pool with
 *
 * Returns 0 on success, <0 on failure
 **/
static int i40e_xsk_pool_enable(struct i40e_vsi *vsi,
				struct xsk_buff_pool *pool,
				u16 qid)
{
	struct net_device *netdev = vsi->netdev;
	bool if_running;
	int err;

	if (vsi->type != I40E_VSI_MAIN)
		return -EINVAL;

	if (qid >= vsi->num_queue_pairs)
		return -EINVAL;

	if (qid >= netdev->real_num_rx_queues ||
	    qid >= netdev->real_num_tx_queues)
		return -EINVAL;

	err = xsk_pool_dma_map(pool, &vsi->back->pdev->dev, I40E_RX_DMA_ATTR);
	if (err)
		return err;

	set_bit(qid, vsi->af_xdp_zc_qps);

	if_running = netif_running(vsi->netdev) && i40e_enabled_xdp_vsi(vsi);

	if (if_running) {
		err = i40e_queue_pair_disable(vsi, qid);
		if (err)
			return err;

		err = i40e_queue_pair_enable(vsi, qid);
		if (err)
			return err;

		/* Kick start the NAPI context so that receiving will start */
		err = i40e_xsk_wakeup(vsi->netdev, qid, XDP_WAKEUP_RX);
		if (err)
			return err;
	}

	return 0;
}

/**
 * i40e_xsk_pool_disable - Disassociate an AF_XDP buffer pool from a
 * certain ring/qid
 * @vsi: Current VSI
 * @qid: Rx ring to associate buffer pool with
 *
 * Returns 0 on success, <0 on failure
 **/
static int i40e_xsk_pool_disable(struct i40e_vsi *vsi, u16 qid)
{
	struct net_device *netdev = vsi->netdev;
	struct xsk_buff_pool *pool;
	bool if_running;
	int err;

	pool = xsk_get_pool_from_qid(netdev, qid);
	if (!pool)
		return -EINVAL;

	if_running = netif_running(vsi->netdev) && i40e_enabled_xdp_vsi(vsi);

	if (if_running) {
		err = i40e_queue_pair_disable(vsi, qid);
		if (err)
			return err;
	}

	clear_bit(qid, vsi->af_xdp_zc_qps);
	xsk_pool_dma_unmap(pool, I40E_RX_DMA_ATTR);

	if (if_running) {
		err = i40e_queue_pair_enable(vsi, qid);
		if (err)
			return err;
	}

	return 0;
}

/**
 * i40e_xsk_pool_setup - Enable/disassociate an AF_XDP buffer pool to/from
 * a ring/qid
 * @vsi: Current VSI
 * @pool: Buffer pool to enable/associate to a ring, or NULL to disable
 * @qid: Rx ring to (dis)associate buffer pool (from)to
 *
 * This function enables or disables a buffer pool to a certain ring.
 *
 * Returns 0 on success, <0 on failure
 **/
int i40e_xsk_pool_setup(struct i40e_vsi *vsi, struct xsk_buff_pool *pool,
			u16 qid)
{
	return pool ? i40e_xsk_pool_enable(vsi, pool, qid) :
		i40e_xsk_pool_disable(vsi, qid);
}

/**
 * i40e_run_xdp_zc - Executes an XDP program on an xdp_buff
 * @rx_ring: Rx ring
 * @xdp: xdp_buff used as input to the XDP program
 *
 * Returns any of I40E_XDP_{PASS, CONSUMED, TX, REDIR}
 **/
static int i40e_run_xdp_zc(struct i40e_ring *rx_ring, struct xdp_buff *xdp)
{
	int err, result = I40E_XDP_PASS;
	struct i40e_ring *xdp_ring;
	struct bpf_prog *xdp_prog;
	u32 act;

	rcu_read_lock();
	/* NB! xdp_prog will always be !NULL, due to the fact that
	 * this path is enabled by setting an XDP program.
	 */
	xdp_prog = READ_ONCE(rx_ring->xdp_prog);
	act = bpf_prog_run_xdp(xdp_prog, xdp);

	if (likely(act == XDP_REDIRECT)) {
		err = xdp_do_redirect(rx_ring->netdev, xdp, xdp_prog);
		if (err)
			goto out_failure;
		rcu_read_unlock();
		return I40E_XDP_REDIR;
	}

	switch (act) {
	case XDP_PASS:
		break;
	case XDP_TX:
		xdp_ring = rx_ring->vsi->xdp_rings[rx_ring->queue_index];
		result = i40e_xmit_xdp_tx_ring(xdp, xdp_ring);
		if (result == I40E_XDP_CONSUMED)
			goto out_failure;
		break;
	default:
		bpf_warn_invalid_xdp_action(act);
		fallthrough;
	case XDP_ABORTED:
out_failure:
		trace_xdp_exception(rx_ring->netdev, xdp_prog, act);
		fallthrough; /* handle aborts by dropping packet */
	case XDP_DROP:
		result = I40E_XDP_CONSUMED;
		break;
	}
	rcu_read_unlock();
	return result;
}

bool i40e_alloc_rx_buffers_zc(struct i40e_ring *rx_ring, u16 count)
{
	u16 ntu = rx_ring->next_to_use;
	union i40e_rx_desc *rx_desc;
	struct xdp_buff **bi, *xdp;
	dma_addr_t dma;
	bool ok = true;

	rx_desc = I40E_RX_DESC(rx_ring, ntu);
	bi = i40e_rx_bi(rx_ring, ntu);
	do {
		xdp = xsk_buff_alloc(rx_ring->xsk_pool);
		if (!xdp) {
			ok = false;
			goto no_buffers;
		}
		*bi = xdp;
		dma = xsk_buff_xdp_get_dma(xdp);
		rx_desc->read.pkt_addr = cpu_to_le64(dma);
		rx_desc->read.hdr_addr = 0;

		rx_desc++;
		bi++;
		ntu++;

		if (unlikely(ntu == rx_ring->count)) {
			rx_desc = I40E_RX_DESC(rx_ring, 0);
			bi = i40e_rx_bi(rx_ring, 0);
			ntu = 0;
		}
	} while (--count);

no_buffers:
	if (rx_ring->next_to_use != ntu) {
		/* clear the status bits for the next_to_use descriptor */
		rx_desc->wb.qword1.status_error_len = 0;
		i40e_release_rx_desc(rx_ring, ntu);
	}

	return ok;
}

/**
 * i40e_construct_skb_zc - Create skbuff from zero-copy Rx buffer
 * @rx_ring: Rx ring
 * @xdp: xdp_buff
 *
 * This functions allocates a new skb from a zero-copy Rx buffer.
 *
 * Returns the skb, or NULL on failure.
 **/
static struct sk_buff *i40e_construct_skb_zc(struct i40e_ring *rx_ring,
					     struct xdp_buff *xdp)
{
	unsigned int metasize = xdp->data - xdp->data_meta;
	unsigned int datasize = xdp->data_end - xdp->data;
	struct sk_buff *skb;

	/* allocate a skb to store the frags */
	skb = __napi_alloc_skb(&rx_ring->q_vector->napi,
			       xdp->data_end - xdp->data_hard_start,
			       GFP_ATOMIC | __GFP_NOWARN);
	if (unlikely(!skb))
		goto out;

	skb_reserve(skb, xdp->data - xdp->data_hard_start);
	memcpy(__skb_put(skb, datasize), xdp->data, datasize);
	if (metasize)
		skb_metadata_set(skb, metasize);

out:
	xsk_buff_free(xdp);
	return skb;
}

static void i40e_handle_xdp_result_zc(struct i40e_ring *rx_ring,
				      struct xdp_buff *xdp_buff,
				      union i40e_rx_desc *rx_desc,
				      unsigned int *rx_packets,
				      unsigned int *rx_bytes,
				      unsigned int size,
				      unsigned int xdp_res)
{
	struct sk_buff *skb;

	*rx_packets = 1;
	*rx_bytes = size;

	if (likely(xdp_res == I40E_XDP_REDIR) || xdp_res == I40E_XDP_TX)
		return;

	if (xdp_res == I40E_XDP_CONSUMED) {
		xsk_buff_free(xdp_buff);
		return;
	}

	if (xdp_res == I40E_XDP_PASS) {
		/* NB! We are not checking for errors using
		 * i40e_test_staterr with
		 * BIT(I40E_RXD_QW1_ERROR_SHIFT). This is due to that
		 * SBP is *not* set in PRT_SBPVSI (default not set).
		 */
		skb = i40e_construct_skb_zc(rx_ring, xdp_buff);
		if (!skb) {
			rx_ring->rx_stats.alloc_buff_failed++;
			*rx_packets = 0;
			*rx_bytes = 0;
			return;
		}

		if (eth_skb_pad(skb)) {
			*rx_packets = 0;
			*rx_bytes = 0;
			return;
		}

		*rx_bytes = skb->len;
		i40e_process_skb_fields(rx_ring, rx_desc, skb);
		napi_gro_receive(&rx_ring->q_vector->napi, skb);
		return;
	}

	/* Should never get here, as all valid cases have been handled already.
	 */
	WARN_ON_ONCE(1);
}

/**
 * i40e_clean_rx_irq_zc - Consumes Rx packets from the hardware ring
 * @rx_ring: Rx ring
 * @budget: NAPI budget
 *
 * Returns amount of work completed
 **/
int i40e_clean_rx_irq_zc(struct i40e_ring *rx_ring, int budget)
{
	unsigned int total_rx_bytes = 0, total_rx_packets = 0;
	u16 cleaned_count = I40E_DESC_UNUSED(rx_ring);
	u16 next_to_clean = rx_ring->next_to_clean;
	u16 count_mask = rx_ring->count - 1;
	unsigned int xdp_res, xdp_xmit = 0;
	bool failure = false;

	while (likely(total_rx_packets < (unsigned int)budget)) {
		union i40e_rx_desc *rx_desc;
		unsigned int rx_packets;
		unsigned int rx_bytes;
		struct xdp_buff *bi;
		unsigned int size;
		u64 qword;

		rx_desc = I40E_RX_DESC(rx_ring, next_to_clean);
		qword = le64_to_cpu(rx_desc->wb.qword1.status_error_len);

		/* This memory barrier is needed to keep us from reading
		 * any other fields out of the rx_desc until we have
		 * verified the descriptor has been written back.
		 */
		dma_rmb();

		if (i40e_rx_is_programming_status(qword)) {
			i40e_clean_programming_status(rx_ring,
						      rx_desc->raw.qword[0],
						      qword);
			bi = *i40e_rx_bi(rx_ring, next_to_clean);
			xsk_buff_free(bi);
			next_to_clean = (next_to_clean + 1) & count_mask;
			continue;
		}

		size = (qword & I40E_RXD_QW1_LENGTH_PBUF_MASK) >>
		       I40E_RXD_QW1_LENGTH_PBUF_SHIFT;
		if (!size)
			break;

		bi = *i40e_rx_bi(rx_ring, next_to_clean);
		bi->data_end = bi->data + size;
		xsk_buff_dma_sync_for_cpu(bi, rx_ring->xsk_pool);

		xdp_res = i40e_run_xdp_zc(rx_ring, bi);
		i40e_handle_xdp_result_zc(rx_ring, bi, rx_desc, &rx_packets,
					  &rx_bytes, size, xdp_res);
		total_rx_packets += rx_packets;
		total_rx_bytes += rx_bytes;
		xdp_xmit |= xdp_res & (I40E_XDP_TX | I40E_XDP_REDIR);
		next_to_clean = (next_to_clean + 1) & count_mask;
	}

	rx_ring->next_to_clean = next_to_clean;
	cleaned_count = (next_to_clean - rx_ring->next_to_use - 1) & count_mask;

	if (cleaned_count >= I40E_RX_BUFFER_WRITE)
		failure = !i40e_alloc_rx_buffers_zc(rx_ring, cleaned_count);

	i40e_finalize_xdp_rx(rx_ring, xdp_xmit);
	i40e_update_rx_stats(rx_ring, total_rx_bytes, total_rx_packets);

	if (xsk_uses_need_wakeup(rx_ring->xsk_pool)) {
		if (failure || next_to_clean == rx_ring->next_to_use)
			xsk_set_rx_need_wakeup(rx_ring->xsk_pool);
		else
			xsk_clear_rx_need_wakeup(rx_ring->xsk_pool);

		return (int)total_rx_packets;
	}
	return failure ? budget : (int)total_rx_packets;
}

static void i40e_xmit_pkt(struct i40e_ring *xdp_ring, struct xdp_desc *desc,
			  unsigned int *total_bytes)
{
	struct i40e_tx_desc *tx_desc;
	dma_addr_t dma;

	dma = xsk_buff_raw_get_dma(xdp_ring->xsk_pool, desc->addr);
	xsk_buff_raw_dma_sync_for_device(xdp_ring->xsk_pool, dma, desc->len);

	tx_desc = I40E_TX_DESC(xdp_ring, xdp_ring->next_to_use++);
	tx_desc->buffer_addr = cpu_to_le64(dma);
	tx_desc->cmd_type_offset_bsz = build_ctob(I40E_TX_DESC_CMD_ICRC | I40E_TX_DESC_CMD_EOP,
						  0, desc->len, 0);

	*total_bytes += desc->len;
}

static void i40e_xmit_pkt_batch(struct i40e_ring *xdp_ring, struct xdp_desc *desc,
				unsigned int *total_bytes)
{
	u16 ntu = xdp_ring->next_to_use;
	struct i40e_tx_desc *tx_desc;
	dma_addr_t dma;
	u32 i;

	loop_unrolled_for(i = 0; i < PKTS_PER_BATCH; i++) {
		dma = xsk_buff_raw_get_dma(xdp_ring->xsk_pool, desc[i].addr);
		xsk_buff_raw_dma_sync_for_device(xdp_ring->xsk_pool, dma, desc[i].len);

		tx_desc = I40E_TX_DESC(xdp_ring, ntu++);
		tx_desc->buffer_addr = cpu_to_le64(dma);
		tx_desc->cmd_type_offset_bsz = build_ctob(I40E_TX_DESC_CMD_ICRC |
							  I40E_TX_DESC_CMD_EOP,
							  0, desc[i].len, 0);

		*total_bytes += desc[i].len;
	}

	xdp_ring->next_to_use = ntu;
}

static void i40e_fill_tx_hw_ring(struct i40e_ring *xdp_ring, struct xdp_desc *descs, u32 nb_pkts,
				 unsigned int *total_bytes)
{
	u32 batched, leftover, i;

	batched = nb_pkts & ~(PKTS_PER_BATCH - 1);
	leftover = nb_pkts & (PKTS_PER_BATCH - 1);
	for (i = 0; i < batched; i += PKTS_PER_BATCH)
		i40e_xmit_pkt_batch(xdp_ring, &descs[i], total_bytes);
	for (i = batched; i < batched + leftover; i++)
		i40e_xmit_pkt(xdp_ring, &descs[i], total_bytes);
}

static void i40e_set_rs_bit(struct i40e_ring *xdp_ring)
{
	u16 ntu = xdp_ring->next_to_use ? xdp_ring->next_to_use - 1 : xdp_ring->count - 1;
	struct i40e_tx_desc *tx_desc;

	tx_desc = I40E_TX_DESC(xdp_ring, ntu);
	tx_desc->cmd_type_offset_bsz |= cpu_to_le64(I40E_TX_DESC_CMD_RS << I40E_TXD_QW1_CMD_SHIFT);
}

/**
 * i40e_xmit_zc - Performs zero-copy Tx AF_XDP
 * @xdp_ring: XDP Tx ring
 * @budget: NAPI budget
 *
 * Returns true if the work is finished.
 **/
static bool i40e_xmit_zc(struct i40e_ring *xdp_ring, unsigned int budget)
{
	struct xdp_desc *descs = xdp_ring->xsk_descs;
	u32 nb_pkts, nb_processed = 0;
	unsigned int total_bytes = 0;

	nb_pkts = xsk_tx_peek_release_desc_batch(xdp_ring->xsk_pool, descs, budget);
	if (!nb_pkts)
		return true;

	if (xdp_ring->next_to_use + nb_pkts >= xdp_ring->count) {
		nb_processed = xdp_ring->count - xdp_ring->next_to_use;
		i40e_fill_tx_hw_ring(xdp_ring, descs, nb_processed, &total_bytes);
		xdp_ring->next_to_use = 0;
	}

	i40e_fill_tx_hw_ring(xdp_ring, &descs[nb_processed], nb_pkts - nb_processed,
			     &total_bytes);

	/* Request an interrupt for the last frame and bump tail ptr. */
	i40e_set_rs_bit(xdp_ring);
	i40e_xdp_ring_update_tail(xdp_ring);

	i40e_update_tx_stats(xdp_ring, nb_pkts, total_bytes);

	return nb_pkts < budget;
}

/**
 * i40e_clean_xdp_tx_buffer - Frees and unmaps an XDP Tx entry
 * @tx_ring: XDP Tx ring
 * @tx_bi: Tx buffer info to clean
 **/
static void i40e_clean_xdp_tx_buffer(struct i40e_ring *tx_ring,
				     struct i40e_tx_buffer *tx_bi)
{
	xdp_return_frame(tx_bi->xdpf);
	tx_ring->xdp_tx_active--;
	dma_unmap_single(tx_ring->dev,
			 dma_unmap_addr(tx_bi, dma),
			 dma_unmap_len(tx_bi, len), DMA_TO_DEVICE);
	dma_unmap_len_set(tx_bi, len, 0);
}

/**
 * i40e_clean_xdp_tx_irq - Completes AF_XDP entries, and cleans XDP entries
 * @vsi: Current VSI
 * @tx_ring: XDP Tx ring
 *
 * Returns true if cleanup/tranmission is done.
 **/
bool i40e_clean_xdp_tx_irq(struct i40e_vsi *vsi, struct i40e_ring *tx_ring)
{
	struct xsk_buff_pool *bp = tx_ring->xsk_pool;
	u32 i, completed_frames, xsk_frames = 0;
	u32 head_idx = i40e_get_head(tx_ring);
	struct i40e_tx_buffer *tx_bi;
	unsigned int ntc;

	if (head_idx < tx_ring->next_to_clean)
		head_idx += tx_ring->count;
	completed_frames = head_idx - tx_ring->next_to_clean;

	if (completed_frames == 0)
		goto out_xmit;

	if (likely(!tx_ring->xdp_tx_active)) {
		xsk_frames = completed_frames;
		goto skip;
	}

	ntc = tx_ring->next_to_clean;

	for (i = 0; i < completed_frames; i++) {
		tx_bi = &tx_ring->tx_bi[ntc];

		if (tx_bi->xdpf) {
			i40e_clean_xdp_tx_buffer(tx_ring, tx_bi);
			tx_bi->xdpf = NULL;
		} else {
			xsk_frames++;
		}

		if (++ntc >= tx_ring->count)
			ntc = 0;
	}

skip:
	tx_ring->next_to_clean += completed_frames;
	if (unlikely(tx_ring->next_to_clean >= tx_ring->count))
		tx_ring->next_to_clean -= tx_ring->count;

	if (xsk_frames)
		xsk_tx_completed(bp, xsk_frames);

	i40e_arm_wb(tx_ring, vsi, completed_frames);

out_xmit:
	if (xsk_uses_need_wakeup(tx_ring->xsk_pool))
		xsk_set_tx_need_wakeup(tx_ring->xsk_pool);

	return i40e_xmit_zc(tx_ring, I40E_DESC_UNUSED(tx_ring));
}

/**
 * i40e_xsk_wakeup - Implements the ndo_xsk_wakeup
 * @dev: the netdevice
 * @queue_id: queue id to wake up
 * @flags: ignored in our case since we have Rx and Tx in the same NAPI.
 *
 * Returns <0 for errors, 0 otherwise.
 **/
int i40e_xsk_wakeup(struct net_device *dev, u32 queue_id, u32 flags)
{
	struct i40e_netdev_priv *np = netdev_priv(dev);
	struct i40e_vsi *vsi = np->vsi;
	struct i40e_pf *pf = vsi->back;
	struct i40e_ring *ring;

	if (test_bit(__I40E_CONFIG_BUSY, pf->state))
		return -EAGAIN;

	if (test_bit(__I40E_VSI_DOWN, vsi->state))
		return -ENETDOWN;

	if (!i40e_enabled_xdp_vsi(vsi))
		return -ENXIO;

	if (queue_id >= vsi->num_queue_pairs)
		return -ENXIO;

	if (!vsi->xdp_rings[queue_id]->xsk_pool)
		return -ENXIO;

	ring = vsi->xdp_rings[queue_id];

	/* The idea here is that if NAPI is running, mark a miss, so
	 * it will run again. If not, trigger an interrupt and
	 * schedule the NAPI from interrupt context. If NAPI would be
	 * scheduled here, the interrupt affinity would not be
	 * honored.
	 */
	if (!napi_if_scheduled_mark_missed(&ring->q_vector->napi))
		i40e_force_wb(vsi, ring->q_vector);

	return 0;
}

void i40e_xsk_clean_rx_ring(struct i40e_ring *rx_ring)
{
	u16 count_mask = rx_ring->count - 1;
	u16 ntc = rx_ring->next_to_clean;
	u16 ntu = rx_ring->next_to_use;

	for ( ; ntc != ntu; ntc = (ntc + 1)  & count_mask) {
		struct xdp_buff *rx_bi = *i40e_rx_bi(rx_ring, ntc);

		xsk_buff_free(rx_bi);
	}
}

/**
 * i40e_xsk_clean_tx_ring - Clean the XDP Tx ring on shutdown
 * @tx_ring: XDP Tx ring
 **/
void i40e_xsk_clean_tx_ring(struct i40e_ring *tx_ring)
{
	u16 ntc = tx_ring->next_to_clean, ntu = tx_ring->next_to_use;
	struct xsk_buff_pool *bp = tx_ring->xsk_pool;
	struct i40e_tx_buffer *tx_bi;
	u32 xsk_frames = 0;

	while (ntc != ntu) {
		tx_bi = &tx_ring->tx_bi[ntc];

		if (tx_bi->xdpf)
			i40e_clean_xdp_tx_buffer(tx_ring, tx_bi);
		else
			xsk_frames++;

		tx_bi->xdpf = NULL;

		ntc++;
		if (ntc >= tx_ring->count)
			ntc = 0;
	}

	if (xsk_frames)
		xsk_tx_completed(bp, xsk_frames);
}

/**
 * i40e_xsk_any_rx_ring_enabled - Checks if Rx rings have an AF_XDP
 * buffer pool attached
 * @vsi: vsi
 *
 * Returns true if any of the Rx rings has an AF_XDP buffer pool attached
 **/
bool i40e_xsk_any_rx_ring_enabled(struct i40e_vsi *vsi)
{
	struct net_device *netdev = vsi->netdev;
	int i;

	for (i = 0; i < vsi->num_queue_pairs; i++) {
		if (xsk_get_pool_from_qid(netdev, i))
			return true;
	}

	return false;
}
