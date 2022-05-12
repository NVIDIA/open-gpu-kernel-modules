/*
 * Copyright (c) 2018, Mellanox Technologies. All rights reserved.
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

#include <linux/bpf_trace.h>
#include <net/xdp_sock_drv.h>
#include "en/xdp.h"
#include "en/params.h"

int mlx5e_xdp_max_mtu(struct mlx5e_params *params, struct mlx5e_xsk_param *xsk)
{
	int hr = mlx5e_get_linear_rq_headroom(params, xsk);

	/* Let S := SKB_DATA_ALIGN(sizeof(struct skb_shared_info)).
	 * The condition checked in mlx5e_rx_is_linear_skb is:
	 *   SKB_DATA_ALIGN(sw_mtu + hard_mtu + hr) + S <= PAGE_SIZE         (1)
	 *   (Note that hw_mtu == sw_mtu + hard_mtu.)
	 * What is returned from this function is:
	 *   max_mtu = PAGE_SIZE - S - hr - hard_mtu                         (2)
	 * After assigning sw_mtu := max_mtu, the left side of (1) turns to
	 * SKB_DATA_ALIGN(PAGE_SIZE - S) + S, which is equal to PAGE_SIZE,
	 * because both PAGE_SIZE and S are already aligned. Any number greater
	 * than max_mtu would make the left side of (1) greater than PAGE_SIZE,
	 * so max_mtu is the maximum MTU allowed.
	 */

	return MLX5E_HW2SW_MTU(params, SKB_MAX_HEAD(hr));
}

static inline bool
mlx5e_xmit_xdp_buff(struct mlx5e_xdpsq *sq, struct mlx5e_rq *rq,
		    struct mlx5e_dma_info *di, struct xdp_buff *xdp)
{
	struct mlx5e_xmit_data xdptxd;
	struct mlx5e_xdp_info xdpi;
	struct xdp_frame *xdpf;
	dma_addr_t dma_addr;

	xdpf = xdp_convert_buff_to_frame(xdp);
	if (unlikely(!xdpf))
		return false;

	xdptxd.data = xdpf->data;
	xdptxd.len  = xdpf->len;

	if (xdp->rxq->mem.type == MEM_TYPE_XSK_BUFF_POOL) {
		/* The xdp_buff was in the UMEM and was copied into a newly
		 * allocated page. The UMEM page was returned via the ZCA, and
		 * this new page has to be mapped at this point and has to be
		 * unmapped and returned via xdp_return_frame on completion.
		 */

		/* Prevent double recycling of the UMEM page. Even in case this
		 * function returns false, the xdp_buff shouldn't be recycled,
		 * as it was already done in xdp_convert_zc_to_xdp_frame.
		 */
		__set_bit(MLX5E_RQ_FLAG_XDP_XMIT, rq->flags); /* non-atomic */

		xdpi.mode = MLX5E_XDP_XMIT_MODE_FRAME;

		dma_addr = dma_map_single(sq->pdev, xdptxd.data, xdptxd.len,
					  DMA_TO_DEVICE);
		if (dma_mapping_error(sq->pdev, dma_addr)) {
			xdp_return_frame(xdpf);
			return false;
		}

		xdptxd.dma_addr     = dma_addr;
		xdpi.frame.xdpf     = xdpf;
		xdpi.frame.dma_addr = dma_addr;
	} else {
		/* Driver assumes that xdp_convert_buff_to_frame returns
		 * an xdp_frame that points to the same memory region as
		 * the original xdp_buff. It allows to map the memory only
		 * once and to use the DMA_BIDIRECTIONAL mode.
		 */

		xdpi.mode = MLX5E_XDP_XMIT_MODE_PAGE;

		dma_addr = di->addr + (xdpf->data - (void *)xdpf);
		dma_sync_single_for_device(sq->pdev, dma_addr, xdptxd.len,
					   DMA_TO_DEVICE);

		xdptxd.dma_addr = dma_addr;
		xdpi.page.rq    = rq;
		xdpi.page.di    = *di;
	}

	return INDIRECT_CALL_2(sq->xmit_xdp_frame, mlx5e_xmit_xdp_frame_mpwqe,
			       mlx5e_xmit_xdp_frame, sq, &xdptxd, &xdpi, 0);
}

/* returns true if packet was consumed by xdp */
bool mlx5e_xdp_handle(struct mlx5e_rq *rq, struct mlx5e_dma_info *di,
		      u32 *len, struct xdp_buff *xdp)
{
	struct bpf_prog *prog = rcu_dereference(rq->xdp_prog);
	u32 act;
	int err;

	if (!prog)
		return false;

	act = bpf_prog_run_xdp(prog, xdp);
	switch (act) {
	case XDP_PASS:
		*len = xdp->data_end - xdp->data;
		return false;
	case XDP_TX:
		if (unlikely(!mlx5e_xmit_xdp_buff(rq->xdpsq, rq, di, xdp)))
			goto xdp_abort;
		__set_bit(MLX5E_RQ_FLAG_XDP_XMIT, rq->flags); /* non-atomic */
		return true;
	case XDP_REDIRECT:
		/* When XDP enabled then page-refcnt==1 here */
		err = xdp_do_redirect(rq->netdev, xdp, prog);
		if (unlikely(err))
			goto xdp_abort;
		__set_bit(MLX5E_RQ_FLAG_XDP_XMIT, rq->flags);
		__set_bit(MLX5E_RQ_FLAG_XDP_REDIRECT, rq->flags);
		if (xdp->rxq->mem.type != MEM_TYPE_XSK_BUFF_POOL)
			mlx5e_page_dma_unmap(rq, di);
		rq->stats->xdp_redirect++;
		return true;
	default:
		bpf_warn_invalid_xdp_action(act);
		fallthrough;
	case XDP_ABORTED:
xdp_abort:
		trace_xdp_exception(rq->netdev, prog, act);
		fallthrough;
	case XDP_DROP:
		rq->stats->xdp_drop++;
		return true;
	}
}

static u16 mlx5e_xdpsq_get_next_pi(struct mlx5e_xdpsq *sq, u16 size)
{
	struct mlx5_wq_cyc *wq = &sq->wq;
	u16 pi, contig_wqebbs;

	pi = mlx5_wq_cyc_ctr2ix(wq, sq->pc);
	contig_wqebbs = mlx5_wq_cyc_get_contig_wqebbs(wq, pi);
	if (unlikely(contig_wqebbs < size)) {
		struct mlx5e_xdp_wqe_info *wi, *edge_wi;

		wi = &sq->db.wqe_info[pi];
		edge_wi = wi + contig_wqebbs;

		/* Fill SQ frag edge with NOPs to avoid WQE wrapping two pages. */
		for (; wi < edge_wi; wi++) {
			*wi = (struct mlx5e_xdp_wqe_info) {
				.num_wqebbs = 1,
				.num_pkts = 0,
			};
			mlx5e_post_nop(wq, sq->sqn, &sq->pc);
		}
		sq->stats->nops += contig_wqebbs;

		pi = mlx5_wq_cyc_ctr2ix(wq, sq->pc);
	}

	return pi;
}

static void mlx5e_xdp_mpwqe_session_start(struct mlx5e_xdpsq *sq)
{
	struct mlx5e_tx_mpwqe *session = &sq->mpwqe;
	struct mlx5e_xdpsq_stats *stats = sq->stats;
	struct mlx5e_tx_wqe *wqe;
	u16 pi;

	pi = mlx5e_xdpsq_get_next_pi(sq, MLX5E_TX_MPW_MAX_WQEBBS);
	wqe = MLX5E_TX_FETCH_WQE(sq, pi);
	net_prefetchw(wqe->data);

	*session = (struct mlx5e_tx_mpwqe) {
		.wqe = wqe,
		.bytes_count = 0,
		.ds_count = MLX5E_TX_WQE_EMPTY_DS_COUNT,
		.pkt_count = 0,
		.inline_on = mlx5e_xdp_get_inline_state(sq, session->inline_on),
	};

	stats->mpwqe++;
}

void mlx5e_xdp_mpwqe_complete(struct mlx5e_xdpsq *sq)
{
	struct mlx5_wq_cyc       *wq    = &sq->wq;
	struct mlx5e_tx_mpwqe *session = &sq->mpwqe;
	struct mlx5_wqe_ctrl_seg *cseg = &session->wqe->ctrl;
	u16 ds_count = session->ds_count;
	u16 pi = mlx5_wq_cyc_ctr2ix(wq, sq->pc);
	struct mlx5e_xdp_wqe_info *wi = &sq->db.wqe_info[pi];

	cseg->opmod_idx_opcode =
		cpu_to_be32((sq->pc << 8) | MLX5_OPCODE_ENHANCED_MPSW);
	cseg->qpn_ds = cpu_to_be32((sq->sqn << 8) | ds_count);

	wi->num_wqebbs = DIV_ROUND_UP(ds_count, MLX5_SEND_WQEBB_NUM_DS);
	wi->num_pkts   = session->pkt_count;

	sq->pc += wi->num_wqebbs;

	sq->doorbell_cseg = cseg;

	session->wqe = NULL; /* Close session */
}

enum {
	MLX5E_XDP_CHECK_OK = 1,
	MLX5E_XDP_CHECK_START_MPWQE = 2,
};

INDIRECT_CALLABLE_SCOPE int mlx5e_xmit_xdp_frame_check_mpwqe(struct mlx5e_xdpsq *sq)
{
	if (unlikely(!sq->mpwqe.wqe)) {
		const u16 stop_room = mlx5e_stop_room_for_wqe(MLX5_SEND_WQE_MAX_WQEBBS);

		if (unlikely(!mlx5e_wqc_has_room_for(&sq->wq, sq->cc, sq->pc,
						     stop_room))) {
			/* SQ is full, ring doorbell */
			mlx5e_xmit_xdp_doorbell(sq);
			sq->stats->full++;
			return -EBUSY;
		}

		return MLX5E_XDP_CHECK_START_MPWQE;
	}

	return MLX5E_XDP_CHECK_OK;
}

INDIRECT_CALLABLE_SCOPE bool
mlx5e_xmit_xdp_frame_mpwqe(struct mlx5e_xdpsq *sq, struct mlx5e_xmit_data *xdptxd,
			   struct mlx5e_xdp_info *xdpi, int check_result)
{
	struct mlx5e_tx_mpwqe *session = &sq->mpwqe;
	struct mlx5e_xdpsq_stats *stats = sq->stats;

	if (unlikely(xdptxd->len > sq->hw_mtu)) {
		stats->err++;
		return false;
	}

	if (!check_result)
		check_result = mlx5e_xmit_xdp_frame_check_mpwqe(sq);
	if (unlikely(check_result < 0))
		return false;

	if (check_result == MLX5E_XDP_CHECK_START_MPWQE) {
		/* Start the session when nothing can fail, so it's guaranteed
		 * that if there is an active session, it has at least one dseg,
		 * and it's safe to complete it at any time.
		 */
		mlx5e_xdp_mpwqe_session_start(sq);
	}

	mlx5e_xdp_mpwqe_add_dseg(sq, xdptxd, stats);

	if (unlikely(mlx5e_xdp_mpqwe_is_full(session)))
		mlx5e_xdp_mpwqe_complete(sq);

	mlx5e_xdpi_fifo_push(&sq->db.xdpi_fifo, xdpi);
	stats->xmit++;
	return true;
}

INDIRECT_CALLABLE_SCOPE int mlx5e_xmit_xdp_frame_check(struct mlx5e_xdpsq *sq)
{
	if (unlikely(!mlx5e_wqc_has_room_for(&sq->wq, sq->cc, sq->pc, 1))) {
		/* SQ is full, ring doorbell */
		mlx5e_xmit_xdp_doorbell(sq);
		sq->stats->full++;
		return -EBUSY;
	}

	return MLX5E_XDP_CHECK_OK;
}

INDIRECT_CALLABLE_SCOPE bool
mlx5e_xmit_xdp_frame(struct mlx5e_xdpsq *sq, struct mlx5e_xmit_data *xdptxd,
		     struct mlx5e_xdp_info *xdpi, int check_result)
{
	struct mlx5_wq_cyc       *wq   = &sq->wq;
	u16                       pi   = mlx5_wq_cyc_ctr2ix(wq, sq->pc);
	struct mlx5e_tx_wqe      *wqe  = mlx5_wq_cyc_get_wqe(wq, pi);

	struct mlx5_wqe_ctrl_seg *cseg = &wqe->ctrl;
	struct mlx5_wqe_eth_seg  *eseg = &wqe->eth;
	struct mlx5_wqe_data_seg *dseg = wqe->data;

	dma_addr_t dma_addr = xdptxd->dma_addr;
	u32 dma_len = xdptxd->len;

	struct mlx5e_xdpsq_stats *stats = sq->stats;

	net_prefetchw(wqe);

	if (unlikely(dma_len < MLX5E_XDP_MIN_INLINE || sq->hw_mtu < dma_len)) {
		stats->err++;
		return false;
	}

	if (!check_result)
		check_result = mlx5e_xmit_xdp_frame_check(sq);
	if (unlikely(check_result < 0))
		return false;

	cseg->fm_ce_se = 0;

	/* copy the inline part if required */
	if (sq->min_inline_mode != MLX5_INLINE_MODE_NONE) {
		memcpy(eseg->inline_hdr.start, xdptxd->data, MLX5E_XDP_MIN_INLINE);
		eseg->inline_hdr.sz = cpu_to_be16(MLX5E_XDP_MIN_INLINE);
		dma_len  -= MLX5E_XDP_MIN_INLINE;
		dma_addr += MLX5E_XDP_MIN_INLINE;
		dseg++;
	}

	/* write the dma part */
	dseg->addr       = cpu_to_be64(dma_addr);
	dseg->byte_count = cpu_to_be32(dma_len);

	cseg->opmod_idx_opcode = cpu_to_be32((sq->pc << 8) | MLX5_OPCODE_SEND);

	sq->pc++;

	sq->doorbell_cseg = cseg;

	mlx5e_xdpi_fifo_push(&sq->db.xdpi_fifo, xdpi);
	stats->xmit++;
	return true;
}

static void mlx5e_free_xdpsq_desc(struct mlx5e_xdpsq *sq,
				  struct mlx5e_xdp_wqe_info *wi,
				  u32 *xsk_frames,
				  bool recycle,
				  struct xdp_frame_bulk *bq)
{
	struct mlx5e_xdp_info_fifo *xdpi_fifo = &sq->db.xdpi_fifo;
	u16 i;

	for (i = 0; i < wi->num_pkts; i++) {
		struct mlx5e_xdp_info xdpi = mlx5e_xdpi_fifo_pop(xdpi_fifo);

		switch (xdpi.mode) {
		case MLX5E_XDP_XMIT_MODE_FRAME:
			/* XDP_TX from the XSK RQ and XDP_REDIRECT */
			dma_unmap_single(sq->pdev, xdpi.frame.dma_addr,
					 xdpi.frame.xdpf->len, DMA_TO_DEVICE);
			xdp_return_frame_bulk(xdpi.frame.xdpf, bq);
			break;
		case MLX5E_XDP_XMIT_MODE_PAGE:
			/* XDP_TX from the regular RQ */
			mlx5e_page_release_dynamic(xdpi.page.rq, &xdpi.page.di, recycle);
			break;
		case MLX5E_XDP_XMIT_MODE_XSK:
			/* AF_XDP send */
			(*xsk_frames)++;
			break;
		default:
			WARN_ON_ONCE(true);
		}
	}
}

bool mlx5e_poll_xdpsq_cq(struct mlx5e_cq *cq)
{
	struct xdp_frame_bulk bq;
	struct mlx5e_xdpsq *sq;
	struct mlx5_cqe64 *cqe;
	u32 xsk_frames = 0;
	u16 sqcc;
	int i;

	xdp_frame_bulk_init(&bq);

	sq = container_of(cq, struct mlx5e_xdpsq, cq);

	if (unlikely(!test_bit(MLX5E_SQ_STATE_ENABLED, &sq->state)))
		return false;

	cqe = mlx5_cqwq_get_cqe(&cq->wq);
	if (!cqe)
		return false;

	/* sq->cc must be updated only after mlx5_cqwq_update_db_record(),
	 * otherwise a cq overrun may occur
	 */
	sqcc = sq->cc;

	i = 0;
	do {
		struct mlx5e_xdp_wqe_info *wi;
		u16 wqe_counter, ci;
		bool last_wqe;

		mlx5_cqwq_pop(&cq->wq);

		wqe_counter = be16_to_cpu(cqe->wqe_counter);

		do {
			last_wqe = (sqcc == wqe_counter);
			ci = mlx5_wq_cyc_ctr2ix(&sq->wq, sqcc);
			wi = &sq->db.wqe_info[ci];

			sqcc += wi->num_wqebbs;

			mlx5e_free_xdpsq_desc(sq, wi, &xsk_frames, true, &bq);
		} while (!last_wqe);

		if (unlikely(get_cqe_opcode(cqe) != MLX5_CQE_REQ)) {
			netdev_WARN_ONCE(sq->channel->netdev,
					 "Bad OP in XDPSQ CQE: 0x%x\n",
					 get_cqe_opcode(cqe));
			mlx5e_dump_error_cqe(&sq->cq, sq->sqn,
					     (struct mlx5_err_cqe *)cqe);
			mlx5_wq_cyc_wqe_dump(&sq->wq, ci, wi->num_wqebbs);
		}
	} while ((++i < MLX5E_TX_CQ_POLL_BUDGET) && (cqe = mlx5_cqwq_get_cqe(&cq->wq)));

	xdp_flush_frame_bulk(&bq);

	if (xsk_frames)
		xsk_tx_completed(sq->xsk_pool, xsk_frames);

	sq->stats->cqes += i;

	mlx5_cqwq_update_db_record(&cq->wq);

	/* ensure cq space is freed before enabling more cqes */
	wmb();

	sq->cc = sqcc;
	return (i == MLX5E_TX_CQ_POLL_BUDGET);
}

void mlx5e_free_xdpsq_descs(struct mlx5e_xdpsq *sq)
{
	struct xdp_frame_bulk bq;
	u32 xsk_frames = 0;

	xdp_frame_bulk_init(&bq);

	rcu_read_lock(); /* need for xdp_return_frame_bulk */

	while (sq->cc != sq->pc) {
		struct mlx5e_xdp_wqe_info *wi;
		u16 ci;

		ci = mlx5_wq_cyc_ctr2ix(&sq->wq, sq->cc);
		wi = &sq->db.wqe_info[ci];

		sq->cc += wi->num_wqebbs;

		mlx5e_free_xdpsq_desc(sq, wi, &xsk_frames, false, &bq);
	}

	xdp_flush_frame_bulk(&bq);
	rcu_read_unlock();

	if (xsk_frames)
		xsk_tx_completed(sq->xsk_pool, xsk_frames);
}

int mlx5e_xdp_xmit(struct net_device *dev, int n, struct xdp_frame **frames,
		   u32 flags)
{
	struct mlx5e_priv *priv = netdev_priv(dev);
	struct mlx5e_xdpsq *sq;
	int nxmit = 0;
	int sq_num;
	int i;

	/* this flag is sufficient, no need to test internal sq state */
	if (unlikely(!mlx5e_xdp_tx_is_enabled(priv)))
		return -ENETDOWN;

	if (unlikely(flags & ~XDP_XMIT_FLAGS_MASK))
		return -EINVAL;

	sq_num = smp_processor_id();

	if (unlikely(sq_num >= priv->channels.num))
		return -ENXIO;

	sq = &priv->channels.c[sq_num]->xdpsq;

	for (i = 0; i < n; i++) {
		struct xdp_frame *xdpf = frames[i];
		struct mlx5e_xmit_data xdptxd;
		struct mlx5e_xdp_info xdpi;
		bool ret;

		xdptxd.data = xdpf->data;
		xdptxd.len = xdpf->len;
		xdptxd.dma_addr = dma_map_single(sq->pdev, xdptxd.data,
						 xdptxd.len, DMA_TO_DEVICE);

		if (unlikely(dma_mapping_error(sq->pdev, xdptxd.dma_addr)))
			break;

		xdpi.mode           = MLX5E_XDP_XMIT_MODE_FRAME;
		xdpi.frame.xdpf     = xdpf;
		xdpi.frame.dma_addr = xdptxd.dma_addr;

		ret = INDIRECT_CALL_2(sq->xmit_xdp_frame, mlx5e_xmit_xdp_frame_mpwqe,
				      mlx5e_xmit_xdp_frame, sq, &xdptxd, &xdpi, 0);
		if (unlikely(!ret)) {
			dma_unmap_single(sq->pdev, xdptxd.dma_addr,
					 xdptxd.len, DMA_TO_DEVICE);
			break;
		}
		nxmit++;
	}

	if (flags & XDP_XMIT_FLUSH) {
		if (sq->mpwqe.wqe)
			mlx5e_xdp_mpwqe_complete(sq);
		mlx5e_xmit_xdp_doorbell(sq);
	}

	return nxmit;
}

void mlx5e_xdp_rx_poll_complete(struct mlx5e_rq *rq)
{
	struct mlx5e_xdpsq *xdpsq = rq->xdpsq;

	if (xdpsq->mpwqe.wqe)
		mlx5e_xdp_mpwqe_complete(xdpsq);

	mlx5e_xmit_xdp_doorbell(xdpsq);

	if (test_bit(MLX5E_RQ_FLAG_XDP_REDIRECT, rq->flags)) {
		xdp_do_flush_map();
		__clear_bit(MLX5E_RQ_FLAG_XDP_REDIRECT, rq->flags);
	}
}

void mlx5e_set_xmit_fp(struct mlx5e_xdpsq *sq, bool is_mpw)
{
	sq->xmit_xdp_frame_check = is_mpw ?
		mlx5e_xmit_xdp_frame_check_mpwqe : mlx5e_xmit_xdp_frame_check;
	sq->xmit_xdp_frame = is_mpw ?
		mlx5e_xmit_xdp_frame_mpwqe : mlx5e_xmit_xdp_frame;
}
