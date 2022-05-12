/*
 * Copyright (c) 2013-2015, Mellanox Technologies. All rights reserved.
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

#include <linux/kref.h>
#include <rdma/ib_umem.h>
#include <rdma/ib_user_verbs.h>
#include <rdma/ib_cache.h>
#include "mlx5_ib.h"
#include "srq.h"
#include "qp.h"

static void mlx5_ib_cq_comp(struct mlx5_core_cq *cq, struct mlx5_eqe *eqe)
{
	struct ib_cq *ibcq = &to_mibcq(cq)->ibcq;

	ibcq->comp_handler(ibcq, ibcq->cq_context);
}

static void mlx5_ib_cq_event(struct mlx5_core_cq *mcq, enum mlx5_event type)
{
	struct mlx5_ib_cq *cq = container_of(mcq, struct mlx5_ib_cq, mcq);
	struct mlx5_ib_dev *dev = to_mdev(cq->ibcq.device);
	struct ib_cq *ibcq = &cq->ibcq;
	struct ib_event event;

	if (type != MLX5_EVENT_TYPE_CQ_ERROR) {
		mlx5_ib_warn(dev, "Unexpected event type %d on CQ %06x\n",
			     type, mcq->cqn);
		return;
	}

	if (ibcq->event_handler) {
		event.device     = &dev->ib_dev;
		event.event      = IB_EVENT_CQ_ERR;
		event.element.cq = ibcq;
		ibcq->event_handler(&event, ibcq->cq_context);
	}
}

static void *get_cqe(struct mlx5_ib_cq *cq, int n)
{
	return mlx5_frag_buf_get_wqe(&cq->buf.fbc, n);
}

static u8 sw_ownership_bit(int n, int nent)
{
	return (n & nent) ? 1 : 0;
}

static void *get_sw_cqe(struct mlx5_ib_cq *cq, int n)
{
	void *cqe = get_cqe(cq, n & cq->ibcq.cqe);
	struct mlx5_cqe64 *cqe64;

	cqe64 = (cq->mcq.cqe_sz == 64) ? cqe : cqe + 64;

	if (likely(get_cqe_opcode(cqe64) != MLX5_CQE_INVALID) &&
	    !((cqe64->op_own & MLX5_CQE_OWNER_MASK) ^ !!(n & (cq->ibcq.cqe + 1)))) {
		return cqe;
	} else {
		return NULL;
	}
}

static void *next_cqe_sw(struct mlx5_ib_cq *cq)
{
	return get_sw_cqe(cq, cq->mcq.cons_index);
}

static enum ib_wc_opcode get_umr_comp(struct mlx5_ib_wq *wq, int idx)
{
	switch (wq->wr_data[idx]) {
	case MLX5_IB_WR_UMR:
		return 0;

	case IB_WR_LOCAL_INV:
		return IB_WC_LOCAL_INV;

	case IB_WR_REG_MR:
		return IB_WC_REG_MR;

	default:
		pr_warn("unknown completion status\n");
		return 0;
	}
}

static void handle_good_req(struct ib_wc *wc, struct mlx5_cqe64 *cqe,
			    struct mlx5_ib_wq *wq, int idx)
{
	wc->wc_flags = 0;
	switch (be32_to_cpu(cqe->sop_drop_qpn) >> 24) {
	case MLX5_OPCODE_RDMA_WRITE_IMM:
		wc->wc_flags |= IB_WC_WITH_IMM;
		fallthrough;
	case MLX5_OPCODE_RDMA_WRITE:
		wc->opcode    = IB_WC_RDMA_WRITE;
		break;
	case MLX5_OPCODE_SEND_IMM:
		wc->wc_flags |= IB_WC_WITH_IMM;
		fallthrough;
	case MLX5_OPCODE_SEND:
	case MLX5_OPCODE_SEND_INVAL:
		wc->opcode    = IB_WC_SEND;
		break;
	case MLX5_OPCODE_RDMA_READ:
		wc->opcode    = IB_WC_RDMA_READ;
		wc->byte_len  = be32_to_cpu(cqe->byte_cnt);
		break;
	case MLX5_OPCODE_ATOMIC_CS:
		wc->opcode    = IB_WC_COMP_SWAP;
		wc->byte_len  = 8;
		break;
	case MLX5_OPCODE_ATOMIC_FA:
		wc->opcode    = IB_WC_FETCH_ADD;
		wc->byte_len  = 8;
		break;
	case MLX5_OPCODE_ATOMIC_MASKED_CS:
		wc->opcode    = IB_WC_MASKED_COMP_SWAP;
		wc->byte_len  = 8;
		break;
	case MLX5_OPCODE_ATOMIC_MASKED_FA:
		wc->opcode    = IB_WC_MASKED_FETCH_ADD;
		wc->byte_len  = 8;
		break;
	case MLX5_OPCODE_UMR:
		wc->opcode = get_umr_comp(wq, idx);
		break;
	}
}

enum {
	MLX5_GRH_IN_BUFFER = 1,
	MLX5_GRH_IN_CQE	   = 2,
};

static void handle_responder(struct ib_wc *wc, struct mlx5_cqe64 *cqe,
			     struct mlx5_ib_qp *qp)
{
	enum rdma_link_layer ll = rdma_port_get_link_layer(qp->ibqp.device, 1);
	struct mlx5_ib_dev *dev = to_mdev(qp->ibqp.device);
	struct mlx5_ib_srq *srq = NULL;
	struct mlx5_ib_wq *wq;
	u16 wqe_ctr;
	u8  roce_packet_type;
	bool vlan_present;
	u8 g;

	if (qp->ibqp.srq || qp->ibqp.xrcd) {
		struct mlx5_core_srq *msrq = NULL;

		if (qp->ibqp.xrcd) {
			msrq = mlx5_cmd_get_srq(dev, be32_to_cpu(cqe->srqn));
			if (msrq)
				srq = to_mibsrq(msrq);
		} else {
			srq = to_msrq(qp->ibqp.srq);
		}
		if (srq) {
			wqe_ctr = be16_to_cpu(cqe->wqe_counter);
			wc->wr_id = srq->wrid[wqe_ctr];
			mlx5_ib_free_srq_wqe(srq, wqe_ctr);
			if (msrq)
				mlx5_core_res_put(&msrq->common);
		}
	} else {
		wq	  = &qp->rq;
		wc->wr_id = wq->wrid[wq->tail & (wq->wqe_cnt - 1)];
		++wq->tail;
	}
	wc->byte_len = be32_to_cpu(cqe->byte_cnt);

	switch (get_cqe_opcode(cqe)) {
	case MLX5_CQE_RESP_WR_IMM:
		wc->opcode	= IB_WC_RECV_RDMA_WITH_IMM;
		wc->wc_flags	= IB_WC_WITH_IMM;
		wc->ex.imm_data = cqe->immediate;
		break;
	case MLX5_CQE_RESP_SEND:
		wc->opcode   = IB_WC_RECV;
		wc->wc_flags = IB_WC_IP_CSUM_OK;
		if (unlikely(!((cqe->hds_ip_ext & CQE_L3_OK) &&
			       (cqe->hds_ip_ext & CQE_L4_OK))))
			wc->wc_flags = 0;
		break;
	case MLX5_CQE_RESP_SEND_IMM:
		wc->opcode	= IB_WC_RECV;
		wc->wc_flags	= IB_WC_WITH_IMM;
		wc->ex.imm_data = cqe->immediate;
		break;
	case MLX5_CQE_RESP_SEND_INV:
		wc->opcode	= IB_WC_RECV;
		wc->wc_flags	= IB_WC_WITH_INVALIDATE;
		wc->ex.invalidate_rkey = be32_to_cpu(cqe->inval_rkey);
		break;
	}
	wc->src_qp	   = be32_to_cpu(cqe->flags_rqpn) & 0xffffff;
	wc->dlid_path_bits = cqe->ml_path;
	g = (be32_to_cpu(cqe->flags_rqpn) >> 28) & 3;
	wc->wc_flags |= g ? IB_WC_GRH : 0;
	if (unlikely(is_qp1(qp->ibqp.qp_type))) {
		u16 pkey = be32_to_cpu(cqe->pkey) & 0xffff;

		ib_find_cached_pkey(&dev->ib_dev, qp->port, pkey,
				    &wc->pkey_index);
	} else {
		wc->pkey_index = 0;
	}

	if (ll != IB_LINK_LAYER_ETHERNET) {
		wc->slid = be16_to_cpu(cqe->slid);
		wc->sl = (be32_to_cpu(cqe->flags_rqpn) >> 24) & 0xf;
		return;
	}

	wc->slid = 0;
	vlan_present = cqe->l4_l3_hdr_type & 0x1;
	roce_packet_type   = (be32_to_cpu(cqe->flags_rqpn) >> 24) & 0x3;
	if (vlan_present) {
		wc->vlan_id = (be16_to_cpu(cqe->vlan_info)) & 0xfff;
		wc->sl = (be16_to_cpu(cqe->vlan_info) >> 13) & 0x7;
		wc->wc_flags |= IB_WC_WITH_VLAN;
	} else {
		wc->sl = 0;
	}

	switch (roce_packet_type) {
	case MLX5_CQE_ROCE_L3_HEADER_TYPE_GRH:
		wc->network_hdr_type = RDMA_NETWORK_ROCE_V1;
		break;
	case MLX5_CQE_ROCE_L3_HEADER_TYPE_IPV6:
		wc->network_hdr_type = RDMA_NETWORK_IPV6;
		break;
	case MLX5_CQE_ROCE_L3_HEADER_TYPE_IPV4:
		wc->network_hdr_type = RDMA_NETWORK_IPV4;
		break;
	}
	wc->wc_flags |= IB_WC_WITH_NETWORK_HDR_TYPE;
}

static void dump_cqe(struct mlx5_ib_dev *dev, struct mlx5_err_cqe *cqe)
{
	mlx5_ib_warn(dev, "dump error cqe\n");
	mlx5_dump_err_cqe(dev->mdev, cqe);
}

static void mlx5_handle_error_cqe(struct mlx5_ib_dev *dev,
				  struct mlx5_err_cqe *cqe,
				  struct ib_wc *wc)
{
	int dump = 1;

	switch (cqe->syndrome) {
	case MLX5_CQE_SYNDROME_LOCAL_LENGTH_ERR:
		wc->status = IB_WC_LOC_LEN_ERR;
		break;
	case MLX5_CQE_SYNDROME_LOCAL_QP_OP_ERR:
		wc->status = IB_WC_LOC_QP_OP_ERR;
		break;
	case MLX5_CQE_SYNDROME_LOCAL_PROT_ERR:
		wc->status = IB_WC_LOC_PROT_ERR;
		break;
	case MLX5_CQE_SYNDROME_WR_FLUSH_ERR:
		dump = 0;
		wc->status = IB_WC_WR_FLUSH_ERR;
		break;
	case MLX5_CQE_SYNDROME_MW_BIND_ERR:
		wc->status = IB_WC_MW_BIND_ERR;
		break;
	case MLX5_CQE_SYNDROME_BAD_RESP_ERR:
		wc->status = IB_WC_BAD_RESP_ERR;
		break;
	case MLX5_CQE_SYNDROME_LOCAL_ACCESS_ERR:
		wc->status = IB_WC_LOC_ACCESS_ERR;
		break;
	case MLX5_CQE_SYNDROME_REMOTE_INVAL_REQ_ERR:
		wc->status = IB_WC_REM_INV_REQ_ERR;
		break;
	case MLX5_CQE_SYNDROME_REMOTE_ACCESS_ERR:
		wc->status = IB_WC_REM_ACCESS_ERR;
		break;
	case MLX5_CQE_SYNDROME_REMOTE_OP_ERR:
		wc->status = IB_WC_REM_OP_ERR;
		break;
	case MLX5_CQE_SYNDROME_TRANSPORT_RETRY_EXC_ERR:
		wc->status = IB_WC_RETRY_EXC_ERR;
		dump = 0;
		break;
	case MLX5_CQE_SYNDROME_RNR_RETRY_EXC_ERR:
		wc->status = IB_WC_RNR_RETRY_EXC_ERR;
		dump = 0;
		break;
	case MLX5_CQE_SYNDROME_REMOTE_ABORTED_ERR:
		wc->status = IB_WC_REM_ABORT_ERR;
		break;
	default:
		wc->status = IB_WC_GENERAL_ERR;
		break;
	}

	wc->vendor_err = cqe->vendor_err_synd;
	if (dump)
		dump_cqe(dev, cqe);
}

static void handle_atomics(struct mlx5_ib_qp *qp, struct mlx5_cqe64 *cqe64,
			   u16 tail, u16 head)
{
	u16 idx;

	do {
		idx = tail & (qp->sq.wqe_cnt - 1);
		if (idx == head)
			break;

		tail = qp->sq.w_list[idx].next;
	} while (1);
	tail = qp->sq.w_list[idx].next;
	qp->sq.last_poll = tail;
}

static void free_cq_buf(struct mlx5_ib_dev *dev, struct mlx5_ib_cq_buf *buf)
{
	mlx5_frag_buf_free(dev->mdev, &buf->frag_buf);
}

static void get_sig_err_item(struct mlx5_sig_err_cqe *cqe,
			     struct ib_sig_err *item)
{
	u16 syndrome = be16_to_cpu(cqe->syndrome);

#define GUARD_ERR   (1 << 13)
#define APPTAG_ERR  (1 << 12)
#define REFTAG_ERR  (1 << 11)

	if (syndrome & GUARD_ERR) {
		item->err_type = IB_SIG_BAD_GUARD;
		item->expected = be32_to_cpu(cqe->expected_trans_sig) >> 16;
		item->actual = be32_to_cpu(cqe->actual_trans_sig) >> 16;
	} else
	if (syndrome & REFTAG_ERR) {
		item->err_type = IB_SIG_BAD_REFTAG;
		item->expected = be32_to_cpu(cqe->expected_reftag);
		item->actual = be32_to_cpu(cqe->actual_reftag);
	} else
	if (syndrome & APPTAG_ERR) {
		item->err_type = IB_SIG_BAD_APPTAG;
		item->expected = be32_to_cpu(cqe->expected_trans_sig) & 0xffff;
		item->actual = be32_to_cpu(cqe->actual_trans_sig) & 0xffff;
	} else {
		pr_err("Got signature completion error with bad syndrome %04x\n",
		       syndrome);
	}

	item->sig_err_offset = be64_to_cpu(cqe->err_offset);
	item->key = be32_to_cpu(cqe->mkey);
}

static void sw_comp(struct mlx5_ib_qp *qp, int num_entries, struct ib_wc *wc,
		    int *npolled, bool is_send)
{
	struct mlx5_ib_wq *wq;
	unsigned int cur;
	int np;
	int i;

	wq = (is_send) ? &qp->sq : &qp->rq;
	cur = wq->head - wq->tail;
	np = *npolled;

	if (cur == 0)
		return;

	for (i = 0;  i < cur && np < num_entries; i++) {
		unsigned int idx;

		idx = (is_send) ? wq->last_poll : wq->tail;
		idx &= (wq->wqe_cnt - 1);
		wc->wr_id = wq->wrid[idx];
		wc->status = IB_WC_WR_FLUSH_ERR;
		wc->vendor_err = MLX5_CQE_SYNDROME_WR_FLUSH_ERR;
		wq->tail++;
		if (is_send)
			wq->last_poll = wq->w_list[idx].next;
		np++;
		wc->qp = &qp->ibqp;
		wc++;
	}
	*npolled = np;
}

static void mlx5_ib_poll_sw_comp(struct mlx5_ib_cq *cq, int num_entries,
				 struct ib_wc *wc, int *npolled)
{
	struct mlx5_ib_qp *qp;

	*npolled = 0;
	/* Find uncompleted WQEs belonging to that cq and return mmics ones */
	list_for_each_entry(qp, &cq->list_send_qp, cq_send_list) {
		sw_comp(qp, num_entries, wc + *npolled, npolled, true);
		if (*npolled >= num_entries)
			return;
	}

	list_for_each_entry(qp, &cq->list_recv_qp, cq_recv_list) {
		sw_comp(qp, num_entries, wc + *npolled, npolled, false);
		if (*npolled >= num_entries)
			return;
	}
}

static int mlx5_poll_one(struct mlx5_ib_cq *cq,
			 struct mlx5_ib_qp **cur_qp,
			 struct ib_wc *wc)
{
	struct mlx5_ib_dev *dev = to_mdev(cq->ibcq.device);
	struct mlx5_err_cqe *err_cqe;
	struct mlx5_cqe64 *cqe64;
	struct mlx5_core_qp *mqp;
	struct mlx5_ib_wq *wq;
	uint8_t opcode;
	uint32_t qpn;
	u16 wqe_ctr;
	void *cqe;
	int idx;

repoll:
	cqe = next_cqe_sw(cq);
	if (!cqe)
		return -EAGAIN;

	cqe64 = (cq->mcq.cqe_sz == 64) ? cqe : cqe + 64;

	++cq->mcq.cons_index;

	/* Make sure we read CQ entry contents after we've checked the
	 * ownership bit.
	 */
	rmb();

	opcode = get_cqe_opcode(cqe64);
	if (unlikely(opcode == MLX5_CQE_RESIZE_CQ)) {
		if (likely(cq->resize_buf)) {
			free_cq_buf(dev, &cq->buf);
			cq->buf = *cq->resize_buf;
			kfree(cq->resize_buf);
			cq->resize_buf = NULL;
			goto repoll;
		} else {
			mlx5_ib_warn(dev, "unexpected resize cqe\n");
		}
	}

	qpn = ntohl(cqe64->sop_drop_qpn) & 0xffffff;
	if (!*cur_qp || (qpn != (*cur_qp)->ibqp.qp_num)) {
		/* We do not have to take the QP table lock here,
		 * because CQs will be locked while QPs are removed
		 * from the table.
		 */
		mqp = radix_tree_lookup(&dev->qp_table.tree, qpn);
		*cur_qp = to_mibqp(mqp);
	}

	wc->qp  = &(*cur_qp)->ibqp;
	switch (opcode) {
	case MLX5_CQE_REQ:
		wq = &(*cur_qp)->sq;
		wqe_ctr = be16_to_cpu(cqe64->wqe_counter);
		idx = wqe_ctr & (wq->wqe_cnt - 1);
		handle_good_req(wc, cqe64, wq, idx);
		handle_atomics(*cur_qp, cqe64, wq->last_poll, idx);
		wc->wr_id = wq->wrid[idx];
		wq->tail = wq->wqe_head[idx] + 1;
		wc->status = IB_WC_SUCCESS;
		break;
	case MLX5_CQE_RESP_WR_IMM:
	case MLX5_CQE_RESP_SEND:
	case MLX5_CQE_RESP_SEND_IMM:
	case MLX5_CQE_RESP_SEND_INV:
		handle_responder(wc, cqe64, *cur_qp);
		wc->status = IB_WC_SUCCESS;
		break;
	case MLX5_CQE_RESIZE_CQ:
		break;
	case MLX5_CQE_REQ_ERR:
	case MLX5_CQE_RESP_ERR:
		err_cqe = (struct mlx5_err_cqe *)cqe64;
		mlx5_handle_error_cqe(dev, err_cqe, wc);
		mlx5_ib_dbg(dev, "%s error cqe on cqn 0x%x:\n",
			    opcode == MLX5_CQE_REQ_ERR ?
			    "Requestor" : "Responder", cq->mcq.cqn);
		mlx5_ib_dbg(dev, "syndrome 0x%x, vendor syndrome 0x%x\n",
			    err_cqe->syndrome, err_cqe->vendor_err_synd);
		if (opcode == MLX5_CQE_REQ_ERR) {
			wq = &(*cur_qp)->sq;
			wqe_ctr = be16_to_cpu(cqe64->wqe_counter);
			idx = wqe_ctr & (wq->wqe_cnt - 1);
			wc->wr_id = wq->wrid[idx];
			wq->tail = wq->wqe_head[idx] + 1;
		} else {
			struct mlx5_ib_srq *srq;

			if ((*cur_qp)->ibqp.srq) {
				srq = to_msrq((*cur_qp)->ibqp.srq);
				wqe_ctr = be16_to_cpu(cqe64->wqe_counter);
				wc->wr_id = srq->wrid[wqe_ctr];
				mlx5_ib_free_srq_wqe(srq, wqe_ctr);
			} else {
				wq = &(*cur_qp)->rq;
				wc->wr_id = wq->wrid[wq->tail & (wq->wqe_cnt - 1)];
				++wq->tail;
			}
		}
		break;
	case MLX5_CQE_SIG_ERR: {
		struct mlx5_sig_err_cqe *sig_err_cqe =
			(struct mlx5_sig_err_cqe *)cqe64;
		struct mlx5_core_sig_ctx *sig;

		xa_lock(&dev->sig_mrs);
		sig = xa_load(&dev->sig_mrs,
				mlx5_base_mkey(be32_to_cpu(sig_err_cqe->mkey)));
		get_sig_err_item(sig_err_cqe, &sig->err_item);
		sig->sig_err_exists = true;
		sig->sigerr_count++;

		mlx5_ib_warn(dev, "CQN: 0x%x Got SIGERR on key: 0x%x err_type %x err_offset %llx expected %x actual %x\n",
			     cq->mcq.cqn, sig->err_item.key,
			     sig->err_item.err_type,
			     sig->err_item.sig_err_offset,
			     sig->err_item.expected,
			     sig->err_item.actual);

		xa_unlock(&dev->sig_mrs);
		goto repoll;
	}
	}

	return 0;
}

static int poll_soft_wc(struct mlx5_ib_cq *cq, int num_entries,
			struct ib_wc *wc, bool is_fatal_err)
{
	struct mlx5_ib_dev *dev = to_mdev(cq->ibcq.device);
	struct mlx5_ib_wc *soft_wc, *next;
	int npolled = 0;

	list_for_each_entry_safe(soft_wc, next, &cq->wc_list, list) {
		if (npolled >= num_entries)
			break;

		mlx5_ib_dbg(dev, "polled software generated completion on CQ 0x%x\n",
			    cq->mcq.cqn);

		if (unlikely(is_fatal_err)) {
			soft_wc->wc.status = IB_WC_WR_FLUSH_ERR;
			soft_wc->wc.vendor_err = MLX5_CQE_SYNDROME_WR_FLUSH_ERR;
		}
		wc[npolled++] = soft_wc->wc;
		list_del(&soft_wc->list);
		kfree(soft_wc);
	}

	return npolled;
}

int mlx5_ib_poll_cq(struct ib_cq *ibcq, int num_entries, struct ib_wc *wc)
{
	struct mlx5_ib_cq *cq = to_mcq(ibcq);
	struct mlx5_ib_qp *cur_qp = NULL;
	struct mlx5_ib_dev *dev = to_mdev(cq->ibcq.device);
	struct mlx5_core_dev *mdev = dev->mdev;
	unsigned long flags;
	int soft_polled = 0;
	int npolled;

	spin_lock_irqsave(&cq->lock, flags);
	if (mdev->state == MLX5_DEVICE_STATE_INTERNAL_ERROR) {
		/* make sure no soft wqe's are waiting */
		if (unlikely(!list_empty(&cq->wc_list)))
			soft_polled = poll_soft_wc(cq, num_entries, wc, true);

		mlx5_ib_poll_sw_comp(cq, num_entries - soft_polled,
				     wc + soft_polled, &npolled);
		goto out;
	}

	if (unlikely(!list_empty(&cq->wc_list)))
		soft_polled = poll_soft_wc(cq, num_entries, wc, false);

	for (npolled = 0; npolled < num_entries - soft_polled; npolled++) {
		if (mlx5_poll_one(cq, &cur_qp, wc + soft_polled + npolled))
			break;
	}

	if (npolled)
		mlx5_cq_set_ci(&cq->mcq);
out:
	spin_unlock_irqrestore(&cq->lock, flags);

	return soft_polled + npolled;
}

int mlx5_ib_arm_cq(struct ib_cq *ibcq, enum ib_cq_notify_flags flags)
{
	struct mlx5_core_dev *mdev = to_mdev(ibcq->device)->mdev;
	struct mlx5_ib_cq *cq = to_mcq(ibcq);
	void __iomem *uar_page = mdev->priv.uar->map;
	unsigned long irq_flags;
	int ret = 0;

	spin_lock_irqsave(&cq->lock, irq_flags);
	if (cq->notify_flags != IB_CQ_NEXT_COMP)
		cq->notify_flags = flags & IB_CQ_SOLICITED_MASK;

	if ((flags & IB_CQ_REPORT_MISSED_EVENTS) && !list_empty(&cq->wc_list))
		ret = 1;
	spin_unlock_irqrestore(&cq->lock, irq_flags);

	mlx5_cq_arm(&cq->mcq,
		    (flags & IB_CQ_SOLICITED_MASK) == IB_CQ_SOLICITED ?
		    MLX5_CQ_DB_REQ_NOT_SOL : MLX5_CQ_DB_REQ_NOT,
		    uar_page, to_mcq(ibcq)->mcq.cons_index);

	return ret;
}

static int alloc_cq_frag_buf(struct mlx5_ib_dev *dev,
			     struct mlx5_ib_cq_buf *buf,
			     int nent,
			     int cqe_size)
{
	struct mlx5_frag_buf *frag_buf = &buf->frag_buf;
	u8 log_wq_stride = 6 + (cqe_size == 128 ? 1 : 0);
	u8 log_wq_sz     = ilog2(cqe_size);
	int err;

	err = mlx5_frag_buf_alloc_node(dev->mdev,
				       nent * cqe_size,
				       frag_buf,
				       dev->mdev->priv.numa_node);
	if (err)
		return err;

	mlx5_init_fbc(frag_buf->frags, log_wq_stride, log_wq_sz, &buf->fbc);

	buf->cqe_size = cqe_size;
	buf->nent = nent;

	return 0;
}

enum {
	MLX5_CQE_RES_FORMAT_HASH = 0,
	MLX5_CQE_RES_FORMAT_CSUM = 1,
	MLX5_CQE_RES_FORMAT_CSUM_STRIDX = 3,
};

static int mini_cqe_res_format_to_hw(struct mlx5_ib_dev *dev, u8 format)
{
	switch (format) {
	case MLX5_IB_CQE_RES_FORMAT_HASH:
		return MLX5_CQE_RES_FORMAT_HASH;
	case MLX5_IB_CQE_RES_FORMAT_CSUM:
		return MLX5_CQE_RES_FORMAT_CSUM;
	case MLX5_IB_CQE_RES_FORMAT_CSUM_STRIDX:
		if (MLX5_CAP_GEN(dev->mdev, mini_cqe_resp_stride_index))
			return MLX5_CQE_RES_FORMAT_CSUM_STRIDX;
		return -EOPNOTSUPP;
	default:
		return -EINVAL;
	}
}

static int create_cq_user(struct mlx5_ib_dev *dev, struct ib_udata *udata,
			  struct mlx5_ib_cq *cq, int entries, u32 **cqb,
			  int *cqe_size, int *index, int *inlen)
{
	struct mlx5_ib_create_cq ucmd = {};
	unsigned long page_size;
	unsigned int page_offset_quantized;
	size_t ucmdlen;
	__be64 *pas;
	int ncont;
	void *cqc;
	int err;
	struct mlx5_ib_ucontext *context = rdma_udata_to_drv_context(
		udata, struct mlx5_ib_ucontext, ibucontext);

	ucmdlen = min(udata->inlen, sizeof(ucmd));
	if (ucmdlen < offsetof(struct mlx5_ib_create_cq, flags))
		return -EINVAL;

	if (ib_copy_from_udata(&ucmd, udata, ucmdlen))
		return -EFAULT;

	if ((ucmd.flags & ~(MLX5_IB_CREATE_CQ_FLAGS_CQE_128B_PAD |
			    MLX5_IB_CREATE_CQ_FLAGS_UAR_PAGE_INDEX)))
		return -EINVAL;

	if ((ucmd.cqe_size != 64 && ucmd.cqe_size != 128) ||
	    ucmd.reserved0 || ucmd.reserved1)
		return -EINVAL;

	*cqe_size = ucmd.cqe_size;

	cq->buf.umem =
		ib_umem_get(&dev->ib_dev, ucmd.buf_addr,
			    entries * ucmd.cqe_size, IB_ACCESS_LOCAL_WRITE);
	if (IS_ERR(cq->buf.umem)) {
		err = PTR_ERR(cq->buf.umem);
		return err;
	}

	page_size = mlx5_umem_find_best_cq_quantized_pgoff(
		cq->buf.umem, cqc, log_page_size, MLX5_ADAPTER_PAGE_SHIFT,
		page_offset, 64, &page_offset_quantized);
	if (!page_size) {
		err = -EINVAL;
		goto err_umem;
	}

	err = mlx5_ib_db_map_user(context, udata, ucmd.db_addr, &cq->db);
	if (err)
		goto err_umem;

	ncont = ib_umem_num_dma_blocks(cq->buf.umem, page_size);
	mlx5_ib_dbg(
		dev,
		"addr 0x%llx, size %u, npages %zu, page_size %lu, ncont %d\n",
		ucmd.buf_addr, entries * ucmd.cqe_size,
		ib_umem_num_pages(cq->buf.umem), page_size, ncont);

	*inlen = MLX5_ST_SZ_BYTES(create_cq_in) +
		 MLX5_FLD_SZ_BYTES(create_cq_in, pas[0]) * ncont;
	*cqb = kvzalloc(*inlen, GFP_KERNEL);
	if (!*cqb) {
		err = -ENOMEM;
		goto err_db;
	}

	pas = (__be64 *)MLX5_ADDR_OF(create_cq_in, *cqb, pas);
	mlx5_ib_populate_pas(cq->buf.umem, page_size, pas, 0);

	cqc = MLX5_ADDR_OF(create_cq_in, *cqb, cq_context);
	MLX5_SET(cqc, cqc, log_page_size,
		 order_base_2(page_size) - MLX5_ADAPTER_PAGE_SHIFT);
	MLX5_SET(cqc, cqc, page_offset, page_offset_quantized);

	if (ucmd.flags & MLX5_IB_CREATE_CQ_FLAGS_UAR_PAGE_INDEX) {
		*index = ucmd.uar_page_index;
	} else if (context->bfregi.lib_uar_dyn) {
		err = -EINVAL;
		goto err_cqb;
	} else {
		*index = context->bfregi.sys_pages[0];
	}

	if (ucmd.cqe_comp_en == 1) {
		int mini_cqe_format;

		if (!((*cqe_size == 128 &&
		       MLX5_CAP_GEN(dev->mdev, cqe_compression_128)) ||
		      (*cqe_size == 64  &&
		       MLX5_CAP_GEN(dev->mdev, cqe_compression)))) {
			err = -EOPNOTSUPP;
			mlx5_ib_warn(dev, "CQE compression is not supported for size %d!\n",
				     *cqe_size);
			goto err_cqb;
		}

		mini_cqe_format =
			mini_cqe_res_format_to_hw(dev,
						  ucmd.cqe_comp_res_format);
		if (mini_cqe_format < 0) {
			err = mini_cqe_format;
			mlx5_ib_dbg(dev, "CQE compression res format %d error: %d\n",
				    ucmd.cqe_comp_res_format, err);
			goto err_cqb;
		}

		MLX5_SET(cqc, cqc, cqe_comp_en, 1);
		MLX5_SET(cqc, cqc, mini_cqe_res_format, mini_cqe_format);
	}

	if (ucmd.flags & MLX5_IB_CREATE_CQ_FLAGS_CQE_128B_PAD) {
		if (*cqe_size != 128 ||
		    !MLX5_CAP_GEN(dev->mdev, cqe_128_always)) {
			err = -EOPNOTSUPP;
			mlx5_ib_warn(dev,
				     "CQE padding is not supported for CQE size of %dB!\n",
				     *cqe_size);
			goto err_cqb;
		}

		cq->private_flags |= MLX5_IB_CQ_PR_FLAGS_CQE_128_PAD;
	}

	MLX5_SET(create_cq_in, *cqb, uid, context->devx_uid);
	return 0;

err_cqb:
	kvfree(*cqb);

err_db:
	mlx5_ib_db_unmap_user(context, &cq->db);

err_umem:
	ib_umem_release(cq->buf.umem);
	return err;
}

static void destroy_cq_user(struct mlx5_ib_cq *cq, struct ib_udata *udata)
{
	struct mlx5_ib_ucontext *context = rdma_udata_to_drv_context(
		udata, struct mlx5_ib_ucontext, ibucontext);

	mlx5_ib_db_unmap_user(context, &cq->db);
	ib_umem_release(cq->buf.umem);
}

static void init_cq_frag_buf(struct mlx5_ib_cq_buf *buf)
{
	int i;
	void *cqe;
	struct mlx5_cqe64 *cqe64;

	for (i = 0; i < buf->nent; i++) {
		cqe = mlx5_frag_buf_get_wqe(&buf->fbc, i);
		cqe64 = buf->cqe_size == 64 ? cqe : cqe + 64;
		cqe64->op_own = MLX5_CQE_INVALID << 4;
	}
}

static int create_cq_kernel(struct mlx5_ib_dev *dev, struct mlx5_ib_cq *cq,
			    int entries, int cqe_size,
			    u32 **cqb, int *index, int *inlen)
{
	__be64 *pas;
	void *cqc;
	int err;

	err = mlx5_db_alloc(dev->mdev, &cq->db);
	if (err)
		return err;

	cq->mcq.set_ci_db  = cq->db.db;
	cq->mcq.arm_db     = cq->db.db + 1;
	cq->mcq.cqe_sz = cqe_size;

	err = alloc_cq_frag_buf(dev, &cq->buf, entries, cqe_size);
	if (err)
		goto err_db;

	init_cq_frag_buf(&cq->buf);

	*inlen = MLX5_ST_SZ_BYTES(create_cq_in) +
		 MLX5_FLD_SZ_BYTES(create_cq_in, pas[0]) *
		 cq->buf.frag_buf.npages;
	*cqb = kvzalloc(*inlen, GFP_KERNEL);
	if (!*cqb) {
		err = -ENOMEM;
		goto err_buf;
	}

	pas = (__be64 *)MLX5_ADDR_OF(create_cq_in, *cqb, pas);
	mlx5_fill_page_frag_array(&cq->buf.frag_buf, pas);

	cqc = MLX5_ADDR_OF(create_cq_in, *cqb, cq_context);
	MLX5_SET(cqc, cqc, log_page_size,
		 cq->buf.frag_buf.page_shift -
		 MLX5_ADAPTER_PAGE_SHIFT);

	*index = dev->mdev->priv.uar->index;

	return 0;

err_buf:
	free_cq_buf(dev, &cq->buf);

err_db:
	mlx5_db_free(dev->mdev, &cq->db);
	return err;
}

static void destroy_cq_kernel(struct mlx5_ib_dev *dev, struct mlx5_ib_cq *cq)
{
	free_cq_buf(dev, &cq->buf);
	mlx5_db_free(dev->mdev, &cq->db);
}

static void notify_soft_wc_handler(struct work_struct *work)
{
	struct mlx5_ib_cq *cq = container_of(work, struct mlx5_ib_cq,
					     notify_work);

	cq->ibcq.comp_handler(&cq->ibcq, cq->ibcq.cq_context);
}

int mlx5_ib_create_cq(struct ib_cq *ibcq, const struct ib_cq_init_attr *attr,
		      struct ib_udata *udata)
{
	struct ib_device *ibdev = ibcq->device;
	int entries = attr->cqe;
	int vector = attr->comp_vector;
	struct mlx5_ib_dev *dev = to_mdev(ibdev);
	struct mlx5_ib_cq *cq = to_mcq(ibcq);
	u32 out[MLX5_ST_SZ_DW(create_cq_out)];
	int index;
	int inlen;
	u32 *cqb = NULL;
	void *cqc;
	int cqe_size;
	unsigned int irqn;
	int eqn;
	int err;

	if (entries < 0 ||
	    (entries > (1 << MLX5_CAP_GEN(dev->mdev, log_max_cq_sz))))
		return -EINVAL;

	if (check_cq_create_flags(attr->flags))
		return -EOPNOTSUPP;

	entries = roundup_pow_of_two(entries + 1);
	if (entries > (1 << MLX5_CAP_GEN(dev->mdev, log_max_cq_sz)))
		return -EINVAL;

	cq->ibcq.cqe = entries - 1;
	mutex_init(&cq->resize_mutex);
	spin_lock_init(&cq->lock);
	cq->resize_buf = NULL;
	cq->resize_umem = NULL;
	cq->create_flags = attr->flags;
	INIT_LIST_HEAD(&cq->list_send_qp);
	INIT_LIST_HEAD(&cq->list_recv_qp);

	if (udata) {
		err = create_cq_user(dev, udata, cq, entries, &cqb, &cqe_size,
				     &index, &inlen);
		if (err)
			return err;
	} else {
		cqe_size = cache_line_size() == 128 ? 128 : 64;
		err = create_cq_kernel(dev, cq, entries, cqe_size, &cqb,
				       &index, &inlen);
		if (err)
			return err;

		INIT_WORK(&cq->notify_work, notify_soft_wc_handler);
	}

	err = mlx5_vector2eqn(dev->mdev, vector, &eqn, &irqn);
	if (err)
		goto err_cqb;

	cq->cqe_size = cqe_size;

	cqc = MLX5_ADDR_OF(create_cq_in, cqb, cq_context);
	MLX5_SET(cqc, cqc, cqe_sz,
		 cqe_sz_to_mlx_sz(cqe_size,
				  cq->private_flags &
				  MLX5_IB_CQ_PR_FLAGS_CQE_128_PAD));
	MLX5_SET(cqc, cqc, log_cq_size, ilog2(entries));
	MLX5_SET(cqc, cqc, uar_page, index);
	MLX5_SET(cqc, cqc, c_eqn, eqn);
	MLX5_SET64(cqc, cqc, dbr_addr, cq->db.dma);
	if (cq->create_flags & IB_UVERBS_CQ_FLAGS_IGNORE_OVERRUN)
		MLX5_SET(cqc, cqc, oi, 1);

	err = mlx5_core_create_cq(dev->mdev, &cq->mcq, cqb, inlen, out, sizeof(out));
	if (err)
		goto err_cqb;

	mlx5_ib_dbg(dev, "cqn 0x%x\n", cq->mcq.cqn);
	cq->mcq.irqn = irqn;
	if (udata)
		cq->mcq.tasklet_ctx.comp = mlx5_ib_cq_comp;
	else
		cq->mcq.comp  = mlx5_ib_cq_comp;
	cq->mcq.event = mlx5_ib_cq_event;

	INIT_LIST_HEAD(&cq->wc_list);

	if (udata)
		if (ib_copy_to_udata(udata, &cq->mcq.cqn, sizeof(__u32))) {
			err = -EFAULT;
			goto err_cmd;
		}


	kvfree(cqb);
	return 0;

err_cmd:
	mlx5_core_destroy_cq(dev->mdev, &cq->mcq);

err_cqb:
	kvfree(cqb);
	if (udata)
		destroy_cq_user(cq, udata);
	else
		destroy_cq_kernel(dev, cq);
	return err;
}

int mlx5_ib_destroy_cq(struct ib_cq *cq, struct ib_udata *udata)
{
	struct mlx5_ib_dev *dev = to_mdev(cq->device);
	struct mlx5_ib_cq *mcq = to_mcq(cq);
	int ret;

	ret = mlx5_core_destroy_cq(dev->mdev, &mcq->mcq);
	if (ret)
		return ret;

	if (udata)
		destroy_cq_user(mcq, udata);
	else
		destroy_cq_kernel(dev, mcq);
	return 0;
}

static int is_equal_rsn(struct mlx5_cqe64 *cqe64, u32 rsn)
{
	return rsn == (ntohl(cqe64->sop_drop_qpn) & 0xffffff);
}

void __mlx5_ib_cq_clean(struct mlx5_ib_cq *cq, u32 rsn, struct mlx5_ib_srq *srq)
{
	struct mlx5_cqe64 *cqe64, *dest64;
	void *cqe, *dest;
	u32 prod_index;
	int nfreed = 0;
	u8 owner_bit;

	if (!cq)
		return;

	/* First we need to find the current producer index, so we
	 * know where to start cleaning from.  It doesn't matter if HW
	 * adds new entries after this loop -- the QP we're worried
	 * about is already in RESET, so the new entries won't come
	 * from our QP and therefore don't need to be checked.
	 */
	for (prod_index = cq->mcq.cons_index; get_sw_cqe(cq, prod_index); prod_index++)
		if (prod_index == cq->mcq.cons_index + cq->ibcq.cqe)
			break;

	/* Now sweep backwards through the CQ, removing CQ entries
	 * that match our QP by copying older entries on top of them.
	 */
	while ((int) --prod_index - (int) cq->mcq.cons_index >= 0) {
		cqe = get_cqe(cq, prod_index & cq->ibcq.cqe);
		cqe64 = (cq->mcq.cqe_sz == 64) ? cqe : cqe + 64;
		if (is_equal_rsn(cqe64, rsn)) {
			if (srq && (ntohl(cqe64->srqn) & 0xffffff))
				mlx5_ib_free_srq_wqe(srq, be16_to_cpu(cqe64->wqe_counter));
			++nfreed;
		} else if (nfreed) {
			dest = get_cqe(cq, (prod_index + nfreed) & cq->ibcq.cqe);
			dest64 = (cq->mcq.cqe_sz == 64) ? dest : dest + 64;
			owner_bit = dest64->op_own & MLX5_CQE_OWNER_MASK;
			memcpy(dest, cqe, cq->mcq.cqe_sz);
			dest64->op_own = owner_bit |
				(dest64->op_own & ~MLX5_CQE_OWNER_MASK);
		}
	}

	if (nfreed) {
		cq->mcq.cons_index += nfreed;
		/* Make sure update of buffer contents is done before
		 * updating consumer index.
		 */
		wmb();
		mlx5_cq_set_ci(&cq->mcq);
	}
}

void mlx5_ib_cq_clean(struct mlx5_ib_cq *cq, u32 qpn, struct mlx5_ib_srq *srq)
{
	if (!cq)
		return;

	spin_lock_irq(&cq->lock);
	__mlx5_ib_cq_clean(cq, qpn, srq);
	spin_unlock_irq(&cq->lock);
}

int mlx5_ib_modify_cq(struct ib_cq *cq, u16 cq_count, u16 cq_period)
{
	struct mlx5_ib_dev *dev = to_mdev(cq->device);
	struct mlx5_ib_cq *mcq = to_mcq(cq);
	int err;

	if (!MLX5_CAP_GEN(dev->mdev, cq_moderation))
		return -EOPNOTSUPP;

	if (cq_period > MLX5_MAX_CQ_PERIOD)
		return -EINVAL;

	err = mlx5_core_modify_cq_moderation(dev->mdev, &mcq->mcq,
					     cq_period, cq_count);
	if (err)
		mlx5_ib_warn(dev, "modify cq 0x%x failed\n", mcq->mcq.cqn);

	return err;
}

static int resize_user(struct mlx5_ib_dev *dev, struct mlx5_ib_cq *cq,
		       int entries, struct ib_udata *udata,
		       int *cqe_size)
{
	struct mlx5_ib_resize_cq ucmd;
	struct ib_umem *umem;
	int err;

	err = ib_copy_from_udata(&ucmd, udata, sizeof(ucmd));
	if (err)
		return err;

	if (ucmd.reserved0 || ucmd.reserved1)
		return -EINVAL;

	/* check multiplication overflow */
	if (ucmd.cqe_size && SIZE_MAX / ucmd.cqe_size <= entries - 1)
		return -EINVAL;

	umem = ib_umem_get(&dev->ib_dev, ucmd.buf_addr,
			   (size_t)ucmd.cqe_size * entries,
			   IB_ACCESS_LOCAL_WRITE);
	if (IS_ERR(umem)) {
		err = PTR_ERR(umem);
		return err;
	}

	cq->resize_umem = umem;
	*cqe_size = ucmd.cqe_size;

	return 0;
}

static int resize_kernel(struct mlx5_ib_dev *dev, struct mlx5_ib_cq *cq,
			 int entries, int cqe_size)
{
	int err;

	cq->resize_buf = kzalloc(sizeof(*cq->resize_buf), GFP_KERNEL);
	if (!cq->resize_buf)
		return -ENOMEM;

	err = alloc_cq_frag_buf(dev, cq->resize_buf, entries, cqe_size);
	if (err)
		goto ex;

	init_cq_frag_buf(cq->resize_buf);

	return 0;

ex:
	kfree(cq->resize_buf);
	return err;
}

static int copy_resize_cqes(struct mlx5_ib_cq *cq)
{
	struct mlx5_ib_dev *dev = to_mdev(cq->ibcq.device);
	struct mlx5_cqe64 *scqe64;
	struct mlx5_cqe64 *dcqe64;
	void *start_cqe;
	void *scqe;
	void *dcqe;
	int ssize;
	int dsize;
	int i;
	u8 sw_own;

	ssize = cq->buf.cqe_size;
	dsize = cq->resize_buf->cqe_size;
	if (ssize != dsize) {
		mlx5_ib_warn(dev, "resize from different cqe size is not supported\n");
		return -EINVAL;
	}

	i = cq->mcq.cons_index;
	scqe = get_sw_cqe(cq, i);
	scqe64 = ssize == 64 ? scqe : scqe + 64;
	start_cqe = scqe;
	if (!scqe) {
		mlx5_ib_warn(dev, "expected cqe in sw ownership\n");
		return -EINVAL;
	}

	while (get_cqe_opcode(scqe64) != MLX5_CQE_RESIZE_CQ) {
		dcqe = mlx5_frag_buf_get_wqe(&cq->resize_buf->fbc,
					     (i + 1) & cq->resize_buf->nent);
		dcqe64 = dsize == 64 ? dcqe : dcqe + 64;
		sw_own = sw_ownership_bit(i + 1, cq->resize_buf->nent);
		memcpy(dcqe, scqe, dsize);
		dcqe64->op_own = (dcqe64->op_own & ~MLX5_CQE_OWNER_MASK) | sw_own;

		++i;
		scqe = get_sw_cqe(cq, i);
		scqe64 = ssize == 64 ? scqe : scqe + 64;
		if (!scqe) {
			mlx5_ib_warn(dev, "expected cqe in sw ownership\n");
			return -EINVAL;
		}

		if (scqe == start_cqe) {
			pr_warn("resize CQ failed to get resize CQE, CQN 0x%x\n",
				cq->mcq.cqn);
			return -ENOMEM;
		}
	}
	++cq->mcq.cons_index;
	return 0;
}

int mlx5_ib_resize_cq(struct ib_cq *ibcq, int entries, struct ib_udata *udata)
{
	struct mlx5_ib_dev *dev = to_mdev(ibcq->device);
	struct mlx5_ib_cq *cq = to_mcq(ibcq);
	void *cqc;
	u32 *in;
	int err;
	int npas;
	__be64 *pas;
	unsigned int page_offset_quantized = 0;
	unsigned int page_shift;
	int inlen;
	int cqe_size;
	unsigned long flags;

	if (!MLX5_CAP_GEN(dev->mdev, cq_resize)) {
		pr_info("Firmware does not support resize CQ\n");
		return -ENOSYS;
	}

	if (entries < 1 ||
	    entries > (1 << MLX5_CAP_GEN(dev->mdev, log_max_cq_sz))) {
		mlx5_ib_warn(dev, "wrong entries number %d, max %d\n",
			     entries,
			     1 << MLX5_CAP_GEN(dev->mdev, log_max_cq_sz));
		return -EINVAL;
	}

	entries = roundup_pow_of_two(entries + 1);
	if (entries > (1 << MLX5_CAP_GEN(dev->mdev, log_max_cq_sz)) + 1)
		return -EINVAL;

	if (entries == ibcq->cqe + 1)
		return 0;

	mutex_lock(&cq->resize_mutex);
	if (udata) {
		unsigned long page_size;

		err = resize_user(dev, cq, entries, udata, &cqe_size);
		if (err)
			goto ex;

		page_size = mlx5_umem_find_best_cq_quantized_pgoff(
			cq->resize_umem, cqc, log_page_size,
			MLX5_ADAPTER_PAGE_SHIFT, page_offset, 64,
			&page_offset_quantized);
		if (!page_size) {
			err = -EINVAL;
			goto ex_resize;
		}
		npas = ib_umem_num_dma_blocks(cq->resize_umem, page_size);
		page_shift = order_base_2(page_size);
	} else {
		struct mlx5_frag_buf *frag_buf;

		cqe_size = 64;
		err = resize_kernel(dev, cq, entries, cqe_size);
		if (err)
			goto ex;
		frag_buf = &cq->resize_buf->frag_buf;
		npas = frag_buf->npages;
		page_shift = frag_buf->page_shift;
	}

	inlen = MLX5_ST_SZ_BYTES(modify_cq_in) +
		MLX5_FLD_SZ_BYTES(modify_cq_in, pas[0]) * npas;

	in = kvzalloc(inlen, GFP_KERNEL);
	if (!in) {
		err = -ENOMEM;
		goto ex_resize;
	}

	pas = (__be64 *)MLX5_ADDR_OF(modify_cq_in, in, pas);
	if (udata)
		mlx5_ib_populate_pas(cq->resize_umem, 1UL << page_shift, pas,
				     0);
	else
		mlx5_fill_page_frag_array(&cq->resize_buf->frag_buf, pas);

	MLX5_SET(modify_cq_in, in,
		 modify_field_select_resize_field_select.resize_field_select.resize_field_select,
		 MLX5_MODIFY_CQ_MASK_LOG_SIZE  |
		 MLX5_MODIFY_CQ_MASK_PG_OFFSET |
		 MLX5_MODIFY_CQ_MASK_PG_SIZE);

	cqc = MLX5_ADDR_OF(modify_cq_in, in, cq_context);

	MLX5_SET(cqc, cqc, log_page_size,
		 page_shift - MLX5_ADAPTER_PAGE_SHIFT);
	MLX5_SET(cqc, cqc, page_offset, page_offset_quantized);
	MLX5_SET(cqc, cqc, cqe_sz,
		 cqe_sz_to_mlx_sz(cqe_size,
				  cq->private_flags &
				  MLX5_IB_CQ_PR_FLAGS_CQE_128_PAD));
	MLX5_SET(cqc, cqc, log_cq_size, ilog2(entries));

	MLX5_SET(modify_cq_in, in, op_mod, MLX5_CQ_OPMOD_RESIZE);
	MLX5_SET(modify_cq_in, in, cqn, cq->mcq.cqn);

	err = mlx5_core_modify_cq(dev->mdev, &cq->mcq, in, inlen);
	if (err)
		goto ex_alloc;

	if (udata) {
		cq->ibcq.cqe = entries - 1;
		ib_umem_release(cq->buf.umem);
		cq->buf.umem = cq->resize_umem;
		cq->resize_umem = NULL;
	} else {
		struct mlx5_ib_cq_buf tbuf;
		int resized = 0;

		spin_lock_irqsave(&cq->lock, flags);
		if (cq->resize_buf) {
			err = copy_resize_cqes(cq);
			if (!err) {
				tbuf = cq->buf;
				cq->buf = *cq->resize_buf;
				kfree(cq->resize_buf);
				cq->resize_buf = NULL;
				resized = 1;
			}
		}
		cq->ibcq.cqe = entries - 1;
		spin_unlock_irqrestore(&cq->lock, flags);
		if (resized)
			free_cq_buf(dev, &tbuf);
	}
	mutex_unlock(&cq->resize_mutex);

	kvfree(in);
	return 0;

ex_alloc:
	kvfree(in);

ex_resize:
	ib_umem_release(cq->resize_umem);
	if (!udata) {
		free_cq_buf(dev, cq->resize_buf);
		cq->resize_buf = NULL;
	}
ex:
	mutex_unlock(&cq->resize_mutex);
	return err;
}

int mlx5_ib_get_cqe_size(struct ib_cq *ibcq)
{
	struct mlx5_ib_cq *cq;

	if (!ibcq)
		return 128;

	cq = to_mcq(ibcq);
	return cq->cqe_size;
}

/* Called from atomic context */
int mlx5_ib_generate_wc(struct ib_cq *ibcq, struct ib_wc *wc)
{
	struct mlx5_ib_wc *soft_wc;
	struct mlx5_ib_cq *cq = to_mcq(ibcq);
	unsigned long flags;

	soft_wc = kmalloc(sizeof(*soft_wc), GFP_ATOMIC);
	if (!soft_wc)
		return -ENOMEM;

	soft_wc->wc = *wc;
	spin_lock_irqsave(&cq->lock, flags);
	list_add_tail(&soft_wc->list, &cq->wc_list);
	if (cq->notify_flags == IB_CQ_NEXT_COMP ||
	    wc->status != IB_WC_SUCCESS) {
		cq->notify_flags = 0;
		schedule_work(&cq->notify_work);
	}
	spin_unlock_irqrestore(&cq->lock, flags);

	return 0;
}
