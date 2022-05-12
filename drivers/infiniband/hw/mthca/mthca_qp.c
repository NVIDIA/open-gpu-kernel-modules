/*
 * Copyright (c) 2004 Topspin Communications.  All rights reserved.
 * Copyright (c) 2005 Cisco Systems. All rights reserved.
 * Copyright (c) 2005 Mellanox Technologies. All rights reserved.
 * Copyright (c) 2004 Voltaire, Inc. All rights reserved.
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

#include <linux/string.h>
#include <linux/slab.h>
#include <linux/sched.h>

#include <asm/io.h>

#include <rdma/ib_verbs.h>
#include <rdma/ib_cache.h>
#include <rdma/ib_pack.h>
#include <rdma/uverbs_ioctl.h>

#include "mthca_dev.h"
#include "mthca_cmd.h"
#include "mthca_memfree.h"
#include "mthca_wqe.h"

enum {
	MTHCA_MAX_DIRECT_QP_SIZE = 4 * PAGE_SIZE,
	MTHCA_ACK_REQ_FREQ       = 10,
	MTHCA_FLIGHT_LIMIT       = 9,
	MTHCA_UD_HEADER_SIZE     = 72, /* largest UD header possible */
	MTHCA_INLINE_HEADER_SIZE = 4,  /* data segment overhead for inline */
	MTHCA_INLINE_CHUNK_SIZE  = 16  /* inline data segment chunk */
};

enum {
	MTHCA_QP_STATE_RST  = 0,
	MTHCA_QP_STATE_INIT = 1,
	MTHCA_QP_STATE_RTR  = 2,
	MTHCA_QP_STATE_RTS  = 3,
	MTHCA_QP_STATE_SQE  = 4,
	MTHCA_QP_STATE_SQD  = 5,
	MTHCA_QP_STATE_ERR  = 6,
	MTHCA_QP_STATE_DRAINING = 7
};

enum {
	MTHCA_QP_ST_RC 	= 0x0,
	MTHCA_QP_ST_UC 	= 0x1,
	MTHCA_QP_ST_RD 	= 0x2,
	MTHCA_QP_ST_UD 	= 0x3,
	MTHCA_QP_ST_MLX = 0x7
};

enum {
	MTHCA_QP_PM_MIGRATED = 0x3,
	MTHCA_QP_PM_ARMED    = 0x0,
	MTHCA_QP_PM_REARM    = 0x1
};

enum {
	/* qp_context flags */
	MTHCA_QP_BIT_DE  = 1 <<  8,
	/* params1 */
	MTHCA_QP_BIT_SRE = 1 << 15,
	MTHCA_QP_BIT_SWE = 1 << 14,
	MTHCA_QP_BIT_SAE = 1 << 13,
	MTHCA_QP_BIT_SIC = 1 <<  4,
	MTHCA_QP_BIT_SSC = 1 <<  3,
	/* params2 */
	MTHCA_QP_BIT_RRE = 1 << 15,
	MTHCA_QP_BIT_RWE = 1 << 14,
	MTHCA_QP_BIT_RAE = 1 << 13,
	MTHCA_QP_BIT_RIC = 1 <<  4,
	MTHCA_QP_BIT_RSC = 1 <<  3
};

enum {
	MTHCA_SEND_DOORBELL_FENCE = 1 << 5
};

struct mthca_qp_path {
	__be32 port_pkey;
	u8     rnr_retry;
	u8     g_mylmc;
	__be16 rlid;
	u8     ackto;
	u8     mgid_index;
	u8     static_rate;
	u8     hop_limit;
	__be32 sl_tclass_flowlabel;
	u8     rgid[16];
} __packed;

struct mthca_qp_context {
	__be32 flags;
	__be32 tavor_sched_queue; /* Reserved on Arbel */
	u8     mtu_msgmax;
	u8     rq_size_stride;	/* Reserved on Tavor */
	u8     sq_size_stride;	/* Reserved on Tavor */
	u8     rlkey_arbel_sched_queue;	/* Reserved on Tavor */
	__be32 usr_page;
	__be32 local_qpn;
	__be32 remote_qpn;
	u32    reserved1[2];
	struct mthca_qp_path pri_path;
	struct mthca_qp_path alt_path;
	__be32 rdd;
	__be32 pd;
	__be32 wqe_base;
	__be32 wqe_lkey;
	__be32 params1;
	__be32 reserved2;
	__be32 next_send_psn;
	__be32 cqn_snd;
	__be32 snd_wqe_base_l;	/* Next send WQE on Tavor */
	__be32 snd_db_index;	/* (debugging only entries) */
	__be32 last_acked_psn;
	__be32 ssn;
	__be32 params2;
	__be32 rnr_nextrecvpsn;
	__be32 ra_buff_indx;
	__be32 cqn_rcv;
	__be32 rcv_wqe_base_l;	/* Next recv WQE on Tavor */
	__be32 rcv_db_index;	/* (debugging only entries) */
	__be32 qkey;
	__be32 srqn;
	__be32 rmsn;
	__be16 rq_wqe_counter;	/* reserved on Tavor */
	__be16 sq_wqe_counter;	/* reserved on Tavor */
	u32    reserved3[18];
} __packed;

struct mthca_qp_param {
	__be32 opt_param_mask;
	u32    reserved1;
	struct mthca_qp_context context;
	u32    reserved2[62];
} __packed;

enum {
	MTHCA_QP_OPTPAR_ALT_ADDR_PATH     = 1 << 0,
	MTHCA_QP_OPTPAR_RRE               = 1 << 1,
	MTHCA_QP_OPTPAR_RAE               = 1 << 2,
	MTHCA_QP_OPTPAR_RWE               = 1 << 3,
	MTHCA_QP_OPTPAR_PKEY_INDEX        = 1 << 4,
	MTHCA_QP_OPTPAR_Q_KEY             = 1 << 5,
	MTHCA_QP_OPTPAR_RNR_TIMEOUT       = 1 << 6,
	MTHCA_QP_OPTPAR_PRIMARY_ADDR_PATH = 1 << 7,
	MTHCA_QP_OPTPAR_SRA_MAX           = 1 << 8,
	MTHCA_QP_OPTPAR_RRA_MAX           = 1 << 9,
	MTHCA_QP_OPTPAR_PM_STATE          = 1 << 10,
	MTHCA_QP_OPTPAR_PORT_NUM          = 1 << 11,
	MTHCA_QP_OPTPAR_RETRY_COUNT       = 1 << 12,
	MTHCA_QP_OPTPAR_ALT_RNR_RETRY     = 1 << 13,
	MTHCA_QP_OPTPAR_ACK_TIMEOUT       = 1 << 14,
	MTHCA_QP_OPTPAR_RNR_RETRY         = 1 << 15,
	MTHCA_QP_OPTPAR_SCHED_QUEUE       = 1 << 16
};

static const u8 mthca_opcode[] = {
	[IB_WR_SEND]                 = MTHCA_OPCODE_SEND,
	[IB_WR_SEND_WITH_IMM]        = MTHCA_OPCODE_SEND_IMM,
	[IB_WR_RDMA_WRITE]           = MTHCA_OPCODE_RDMA_WRITE,
	[IB_WR_RDMA_WRITE_WITH_IMM]  = MTHCA_OPCODE_RDMA_WRITE_IMM,
	[IB_WR_RDMA_READ]            = MTHCA_OPCODE_RDMA_READ,
	[IB_WR_ATOMIC_CMP_AND_SWP]   = MTHCA_OPCODE_ATOMIC_CS,
	[IB_WR_ATOMIC_FETCH_AND_ADD] = MTHCA_OPCODE_ATOMIC_FA,
};

static int is_sqp(struct mthca_dev *dev, struct mthca_qp *qp)
{
	return qp->qpn >= dev->qp_table.sqp_start &&
		qp->qpn <= dev->qp_table.sqp_start + 3;
}

static int is_qp0(struct mthca_dev *dev, struct mthca_qp *qp)
{
	return qp->qpn >= dev->qp_table.sqp_start &&
		qp->qpn <= dev->qp_table.sqp_start + 1;
}

static void *get_recv_wqe(struct mthca_qp *qp, int n)
{
	if (qp->is_direct)
		return qp->queue.direct.buf + (n << qp->rq.wqe_shift);
	else
		return qp->queue.page_list[(n << qp->rq.wqe_shift) >> PAGE_SHIFT].buf +
			((n << qp->rq.wqe_shift) & (PAGE_SIZE - 1));
}

static void *get_send_wqe(struct mthca_qp *qp, int n)
{
	if (qp->is_direct)
		return qp->queue.direct.buf + qp->send_wqe_offset +
			(n << qp->sq.wqe_shift);
	else
		return qp->queue.page_list[(qp->send_wqe_offset +
					    (n << qp->sq.wqe_shift)) >>
					   PAGE_SHIFT].buf +
			((qp->send_wqe_offset + (n << qp->sq.wqe_shift)) &
			 (PAGE_SIZE - 1));
}

static void mthca_wq_reset(struct mthca_wq *wq)
{
	wq->next_ind  = 0;
	wq->last_comp = wq->max - 1;
	wq->head      = 0;
	wq->tail      = 0;
}

void mthca_qp_event(struct mthca_dev *dev, u32 qpn,
		    enum ib_event_type event_type)
{
	struct mthca_qp *qp;
	struct ib_event event;

	spin_lock(&dev->qp_table.lock);
	qp = mthca_array_get(&dev->qp_table.qp, qpn & (dev->limits.num_qps - 1));
	if (qp)
		++qp->refcount;
	spin_unlock(&dev->qp_table.lock);

	if (!qp) {
		mthca_warn(dev, "Async event %d for bogus QP %08x\n",
			   event_type, qpn);
		return;
	}

	if (event_type == IB_EVENT_PATH_MIG)
		qp->port = qp->alt_port;

	event.device      = &dev->ib_dev;
	event.event       = event_type;
	event.element.qp  = &qp->ibqp;
	if (qp->ibqp.event_handler)
		qp->ibqp.event_handler(&event, qp->ibqp.qp_context);

	spin_lock(&dev->qp_table.lock);
	if (!--qp->refcount)
		wake_up(&qp->wait);
	spin_unlock(&dev->qp_table.lock);
}

static int to_mthca_state(enum ib_qp_state ib_state)
{
	switch (ib_state) {
	case IB_QPS_RESET: return MTHCA_QP_STATE_RST;
	case IB_QPS_INIT:  return MTHCA_QP_STATE_INIT;
	case IB_QPS_RTR:   return MTHCA_QP_STATE_RTR;
	case IB_QPS_RTS:   return MTHCA_QP_STATE_RTS;
	case IB_QPS_SQD:   return MTHCA_QP_STATE_SQD;
	case IB_QPS_SQE:   return MTHCA_QP_STATE_SQE;
	case IB_QPS_ERR:   return MTHCA_QP_STATE_ERR;
	default:                return -1;
	}
}

enum { RC, UC, UD, RD, RDEE, MLX, NUM_TRANS };

static int to_mthca_st(int transport)
{
	switch (transport) {
	case RC:  return MTHCA_QP_ST_RC;
	case UC:  return MTHCA_QP_ST_UC;
	case UD:  return MTHCA_QP_ST_UD;
	case RD:  return MTHCA_QP_ST_RD;
	case MLX: return MTHCA_QP_ST_MLX;
	default:  return -1;
	}
}

static void store_attrs(struct mthca_sqp *sqp, const struct ib_qp_attr *attr,
			int attr_mask)
{
	if (attr_mask & IB_QP_PKEY_INDEX)
		sqp->pkey_index = attr->pkey_index;
	if (attr_mask & IB_QP_QKEY)
		sqp->qkey = attr->qkey;
	if (attr_mask & IB_QP_SQ_PSN)
		sqp->send_psn = attr->sq_psn;
}

static void init_port(struct mthca_dev *dev, int port)
{
	int err;
	struct mthca_init_ib_param param;

	memset(&param, 0, sizeof param);

	param.port_width = dev->limits.port_width_cap;
	param.vl_cap     = dev->limits.vl_cap;
	param.mtu_cap    = dev->limits.mtu_cap;
	param.gid_cap    = dev->limits.gid_table_len;
	param.pkey_cap   = dev->limits.pkey_table_len;

	err = mthca_INIT_IB(dev, &param, port);
	if (err)
		mthca_warn(dev, "INIT_IB failed, return code %d.\n", err);
}

static __be32 get_hw_access_flags(struct mthca_qp *qp, const struct ib_qp_attr *attr,
				  int attr_mask)
{
	u8 dest_rd_atomic;
	u32 access_flags;
	u32 hw_access_flags = 0;

	if (attr_mask & IB_QP_MAX_DEST_RD_ATOMIC)
		dest_rd_atomic = attr->max_dest_rd_atomic;
	else
		dest_rd_atomic = qp->resp_depth;

	if (attr_mask & IB_QP_ACCESS_FLAGS)
		access_flags = attr->qp_access_flags;
	else
		access_flags = qp->atomic_rd_en;

	if (!dest_rd_atomic)
		access_flags &= IB_ACCESS_REMOTE_WRITE;

	if (access_flags & IB_ACCESS_REMOTE_READ)
		hw_access_flags |= MTHCA_QP_BIT_RRE;
	if (access_flags & IB_ACCESS_REMOTE_ATOMIC)
		hw_access_flags |= MTHCA_QP_BIT_RAE;
	if (access_flags & IB_ACCESS_REMOTE_WRITE)
		hw_access_flags |= MTHCA_QP_BIT_RWE;

	return cpu_to_be32(hw_access_flags);
}

static inline enum ib_qp_state to_ib_qp_state(int mthca_state)
{
	switch (mthca_state) {
	case MTHCA_QP_STATE_RST:      return IB_QPS_RESET;
	case MTHCA_QP_STATE_INIT:     return IB_QPS_INIT;
	case MTHCA_QP_STATE_RTR:      return IB_QPS_RTR;
	case MTHCA_QP_STATE_RTS:      return IB_QPS_RTS;
	case MTHCA_QP_STATE_DRAINING:
	case MTHCA_QP_STATE_SQD:      return IB_QPS_SQD;
	case MTHCA_QP_STATE_SQE:      return IB_QPS_SQE;
	case MTHCA_QP_STATE_ERR:      return IB_QPS_ERR;
	default:                      return -1;
	}
}

static inline enum ib_mig_state to_ib_mig_state(int mthca_mig_state)
{
	switch (mthca_mig_state) {
	case 0:  return IB_MIG_ARMED;
	case 1:  return IB_MIG_REARM;
	case 3:  return IB_MIG_MIGRATED;
	default: return -1;
	}
}

static int to_ib_qp_access_flags(int mthca_flags)
{
	int ib_flags = 0;

	if (mthca_flags & MTHCA_QP_BIT_RRE)
		ib_flags |= IB_ACCESS_REMOTE_READ;
	if (mthca_flags & MTHCA_QP_BIT_RWE)
		ib_flags |= IB_ACCESS_REMOTE_WRITE;
	if (mthca_flags & MTHCA_QP_BIT_RAE)
		ib_flags |= IB_ACCESS_REMOTE_ATOMIC;

	return ib_flags;
}

static void to_rdma_ah_attr(struct mthca_dev *dev,
			    struct rdma_ah_attr *ah_attr,
			    struct mthca_qp_path *path)
{
	u8 port_num = (be32_to_cpu(path->port_pkey) >> 24) & 0x3;

	memset(ah_attr, 0, sizeof(*ah_attr));

	if (port_num == 0 || port_num > dev->limits.num_ports)
		return;
	ah_attr->type = rdma_ah_find_type(&dev->ib_dev, port_num);
	rdma_ah_set_port_num(ah_attr, port_num);

	rdma_ah_set_dlid(ah_attr, be16_to_cpu(path->rlid));
	rdma_ah_set_sl(ah_attr, be32_to_cpu(path->sl_tclass_flowlabel) >> 28);
	rdma_ah_set_path_bits(ah_attr, path->g_mylmc & 0x7f);
	rdma_ah_set_static_rate(ah_attr,
				mthca_rate_to_ib(dev,
						 path->static_rate & 0xf,
						 port_num));
	if (path->g_mylmc & (1 << 7)) {
		u32 tc_fl = be32_to_cpu(path->sl_tclass_flowlabel);

		rdma_ah_set_grh(ah_attr, NULL,
				tc_fl & 0xfffff,
				path->mgid_index &
				(dev->limits.gid_table_len - 1),
				path->hop_limit,
				(tc_fl >> 20) & 0xff);
		rdma_ah_set_dgid_raw(ah_attr, path->rgid);
	}
}

int mthca_query_qp(struct ib_qp *ibqp, struct ib_qp_attr *qp_attr, int qp_attr_mask,
		   struct ib_qp_init_attr *qp_init_attr)
{
	struct mthca_dev *dev = to_mdev(ibqp->device);
	struct mthca_qp *qp = to_mqp(ibqp);
	int err = 0;
	struct mthca_mailbox *mailbox = NULL;
	struct mthca_qp_param *qp_param;
	struct mthca_qp_context *context;
	int mthca_state;

	mutex_lock(&qp->mutex);

	if (qp->state == IB_QPS_RESET) {
		qp_attr->qp_state = IB_QPS_RESET;
		goto done;
	}

	mailbox = mthca_alloc_mailbox(dev, GFP_KERNEL);
	if (IS_ERR(mailbox)) {
		err = PTR_ERR(mailbox);
		goto out;
	}

	err = mthca_QUERY_QP(dev, qp->qpn, 0, mailbox);
	if (err) {
		mthca_warn(dev, "QUERY_QP failed (%d)\n", err);
		goto out_mailbox;
	}

	qp_param    = mailbox->buf;
	context     = &qp_param->context;
	mthca_state = be32_to_cpu(context->flags) >> 28;

	qp->state		     = to_ib_qp_state(mthca_state);
	qp_attr->qp_state	     = qp->state;
	qp_attr->path_mtu 	     = context->mtu_msgmax >> 5;
	qp_attr->path_mig_state      =
		to_ib_mig_state((be32_to_cpu(context->flags) >> 11) & 0x3);
	qp_attr->qkey 		     = be32_to_cpu(context->qkey);
	qp_attr->rq_psn 	     = be32_to_cpu(context->rnr_nextrecvpsn) & 0xffffff;
	qp_attr->sq_psn 	     = be32_to_cpu(context->next_send_psn) & 0xffffff;
	qp_attr->dest_qp_num 	     = be32_to_cpu(context->remote_qpn) & 0xffffff;
	qp_attr->qp_access_flags     =
		to_ib_qp_access_flags(be32_to_cpu(context->params2));

	if (qp->transport == RC || qp->transport == UC) {
		to_rdma_ah_attr(dev, &qp_attr->ah_attr, &context->pri_path);
		to_rdma_ah_attr(dev, &qp_attr->alt_ah_attr, &context->alt_path);
		qp_attr->alt_pkey_index =
			be32_to_cpu(context->alt_path.port_pkey) & 0x7f;
		qp_attr->alt_port_num	=
			rdma_ah_get_port_num(&qp_attr->alt_ah_attr);
	}

	qp_attr->pkey_index = be32_to_cpu(context->pri_path.port_pkey) & 0x7f;
	qp_attr->port_num   =
		(be32_to_cpu(context->pri_path.port_pkey) >> 24) & 0x3;

	/* qp_attr->en_sqd_async_notify is only applicable in modify qp */
	qp_attr->sq_draining = mthca_state == MTHCA_QP_STATE_DRAINING;

	qp_attr->max_rd_atomic = 1 << ((be32_to_cpu(context->params1) >> 21) & 0x7);

	qp_attr->max_dest_rd_atomic =
		1 << ((be32_to_cpu(context->params2) >> 21) & 0x7);
	qp_attr->min_rnr_timer 	    =
		(be32_to_cpu(context->rnr_nextrecvpsn) >> 24) & 0x1f;
	qp_attr->timeout 	    = context->pri_path.ackto >> 3;
	qp_attr->retry_cnt 	    = (be32_to_cpu(context->params1) >> 16) & 0x7;
	qp_attr->rnr_retry 	    = context->pri_path.rnr_retry >> 5;
	qp_attr->alt_timeout 	    = context->alt_path.ackto >> 3;

done:
	qp_attr->cur_qp_state	     = qp_attr->qp_state;
	qp_attr->cap.max_send_wr     = qp->sq.max;
	qp_attr->cap.max_recv_wr     = qp->rq.max;
	qp_attr->cap.max_send_sge    = qp->sq.max_gs;
	qp_attr->cap.max_recv_sge    = qp->rq.max_gs;
	qp_attr->cap.max_inline_data = qp->max_inline_data;

	qp_init_attr->cap	     = qp_attr->cap;
	qp_init_attr->sq_sig_type    = qp->sq_policy;

out_mailbox:
	mthca_free_mailbox(dev, mailbox);

out:
	mutex_unlock(&qp->mutex);
	return err;
}

static int mthca_path_set(struct mthca_dev *dev, const struct rdma_ah_attr *ah,
			  struct mthca_qp_path *path, u8 port)
{
	path->g_mylmc     = rdma_ah_get_path_bits(ah) & 0x7f;
	path->rlid        = cpu_to_be16(rdma_ah_get_dlid(ah));
	path->static_rate = mthca_get_rate(dev, rdma_ah_get_static_rate(ah),
					   port);

	if (rdma_ah_get_ah_flags(ah) & IB_AH_GRH) {
		const struct ib_global_route *grh = rdma_ah_read_grh(ah);

		if (grh->sgid_index >= dev->limits.gid_table_len) {
			mthca_dbg(dev, "sgid_index (%u) too large. max is %d\n",
				  grh->sgid_index,
				  dev->limits.gid_table_len - 1);
			return -1;
		}

		path->g_mylmc   |= 1 << 7;
		path->mgid_index = grh->sgid_index;
		path->hop_limit  = grh->hop_limit;
		path->sl_tclass_flowlabel =
			cpu_to_be32((rdma_ah_get_sl(ah) << 28) |
				    (grh->traffic_class << 20) |
				    (grh->flow_label));
		memcpy(path->rgid, grh->dgid.raw, 16);
	} else {
		path->sl_tclass_flowlabel = cpu_to_be32(rdma_ah_get_sl(ah) <<
							28);
	}

	return 0;
}

static int __mthca_modify_qp(struct ib_qp *ibqp,
			     const struct ib_qp_attr *attr, int attr_mask,
			     enum ib_qp_state cur_state,
			     enum ib_qp_state new_state,
			     struct ib_udata *udata)
{
	struct mthca_dev *dev = to_mdev(ibqp->device);
	struct mthca_qp *qp = to_mqp(ibqp);
	struct mthca_ucontext *context = rdma_udata_to_drv_context(
		udata, struct mthca_ucontext, ibucontext);
	struct mthca_mailbox *mailbox;
	struct mthca_qp_param *qp_param;
	struct mthca_qp_context *qp_context;
	u32 sqd_event = 0;
	int err = -EINVAL;

	mailbox = mthca_alloc_mailbox(dev, GFP_KERNEL);
	if (IS_ERR(mailbox)) {
		err = PTR_ERR(mailbox);
		goto out;
	}
	qp_param = mailbox->buf;
	qp_context = &qp_param->context;
	memset(qp_param, 0, sizeof *qp_param);

	qp_context->flags      = cpu_to_be32((to_mthca_state(new_state) << 28) |
					     (to_mthca_st(qp->transport) << 16));
	qp_context->flags     |= cpu_to_be32(MTHCA_QP_BIT_DE);
	if (!(attr_mask & IB_QP_PATH_MIG_STATE))
		qp_context->flags |= cpu_to_be32(MTHCA_QP_PM_MIGRATED << 11);
	else {
		qp_param->opt_param_mask |= cpu_to_be32(MTHCA_QP_OPTPAR_PM_STATE);
		switch (attr->path_mig_state) {
		case IB_MIG_MIGRATED:
			qp_context->flags |= cpu_to_be32(MTHCA_QP_PM_MIGRATED << 11);
			break;
		case IB_MIG_REARM:
			qp_context->flags |= cpu_to_be32(MTHCA_QP_PM_REARM << 11);
			break;
		case IB_MIG_ARMED:
			qp_context->flags |= cpu_to_be32(MTHCA_QP_PM_ARMED << 11);
			break;
		}
	}

	/* leave tavor_sched_queue as 0 */

	if (qp->transport == MLX || qp->transport == UD)
		qp_context->mtu_msgmax = (IB_MTU_2048 << 5) | 11;
	else if (attr_mask & IB_QP_PATH_MTU) {
		if (attr->path_mtu < IB_MTU_256 || attr->path_mtu > IB_MTU_2048) {
			mthca_dbg(dev, "path MTU (%u) is invalid\n",
				  attr->path_mtu);
			goto out_mailbox;
		}
		qp_context->mtu_msgmax = (attr->path_mtu << 5) | 31;
	}

	if (mthca_is_memfree(dev)) {
		if (qp->rq.max)
			qp_context->rq_size_stride = ilog2(qp->rq.max) << 3;
		qp_context->rq_size_stride |= qp->rq.wqe_shift - 4;

		if (qp->sq.max)
			qp_context->sq_size_stride = ilog2(qp->sq.max) << 3;
		qp_context->sq_size_stride |= qp->sq.wqe_shift - 4;
	}

	/* leave arbel_sched_queue as 0 */

	if (qp->ibqp.uobject)
		qp_context->usr_page = cpu_to_be32(context->uar.index);
	else
		qp_context->usr_page = cpu_to_be32(dev->driver_uar.index);
	qp_context->local_qpn  = cpu_to_be32(qp->qpn);
	if (attr_mask & IB_QP_DEST_QPN) {
		qp_context->remote_qpn = cpu_to_be32(attr->dest_qp_num);
	}

	if (qp->transport == MLX)
		qp_context->pri_path.port_pkey |=
			cpu_to_be32(qp->port << 24);
	else {
		if (attr_mask & IB_QP_PORT) {
			qp_context->pri_path.port_pkey |=
				cpu_to_be32(attr->port_num << 24);
			qp_param->opt_param_mask |= cpu_to_be32(MTHCA_QP_OPTPAR_PORT_NUM);
		}
	}

	if (attr_mask & IB_QP_PKEY_INDEX) {
		qp_context->pri_path.port_pkey |=
			cpu_to_be32(attr->pkey_index);
		qp_param->opt_param_mask |= cpu_to_be32(MTHCA_QP_OPTPAR_PKEY_INDEX);
	}

	if (attr_mask & IB_QP_RNR_RETRY) {
		qp_context->alt_path.rnr_retry = qp_context->pri_path.rnr_retry =
			attr->rnr_retry << 5;
		qp_param->opt_param_mask |= cpu_to_be32(MTHCA_QP_OPTPAR_RNR_RETRY |
							MTHCA_QP_OPTPAR_ALT_RNR_RETRY);
	}

	if (attr_mask & IB_QP_AV) {
		if (mthca_path_set(dev, &attr->ah_attr, &qp_context->pri_path,
				   attr_mask & IB_QP_PORT ? attr->port_num : qp->port))
			goto out_mailbox;

		qp_param->opt_param_mask |= cpu_to_be32(MTHCA_QP_OPTPAR_PRIMARY_ADDR_PATH);
	}

	if (ibqp->qp_type == IB_QPT_RC &&
	    cur_state == IB_QPS_INIT && new_state == IB_QPS_RTR) {
		u8 sched_queue = ibqp->uobject ? 0x2 : 0x1;

		if (mthca_is_memfree(dev))
			qp_context->rlkey_arbel_sched_queue |= sched_queue;
		else
			qp_context->tavor_sched_queue |= cpu_to_be32(sched_queue);

		qp_param->opt_param_mask |=
			cpu_to_be32(MTHCA_QP_OPTPAR_SCHED_QUEUE);
	}

	if (attr_mask & IB_QP_TIMEOUT) {
		qp_context->pri_path.ackto = attr->timeout << 3;
		qp_param->opt_param_mask |= cpu_to_be32(MTHCA_QP_OPTPAR_ACK_TIMEOUT);
	}

	if (attr_mask & IB_QP_ALT_PATH) {
		if (attr->alt_pkey_index >= dev->limits.pkey_table_len) {
			mthca_dbg(dev, "Alternate P_Key index (%u) too large. max is %d\n",
				  attr->alt_pkey_index, dev->limits.pkey_table_len-1);
			goto out_mailbox;
		}

		if (attr->alt_port_num == 0 || attr->alt_port_num > dev->limits.num_ports) {
			mthca_dbg(dev, "Alternate port number (%u) is invalid\n",
				attr->alt_port_num);
			goto out_mailbox;
		}

		if (mthca_path_set(dev, &attr->alt_ah_attr, &qp_context->alt_path,
				   rdma_ah_get_port_num(&attr->alt_ah_attr)))
			goto out_mailbox;

		qp_context->alt_path.port_pkey |= cpu_to_be32(attr->alt_pkey_index |
							      attr->alt_port_num << 24);
		qp_context->alt_path.ackto = attr->alt_timeout << 3;
		qp_param->opt_param_mask |= cpu_to_be32(MTHCA_QP_OPTPAR_ALT_ADDR_PATH);
	}

	/* leave rdd as 0 */
	qp_context->pd         = cpu_to_be32(to_mpd(ibqp->pd)->pd_num);
	/* leave wqe_base as 0 (we always create an MR based at 0 for WQs) */
	qp_context->wqe_lkey   = cpu_to_be32(qp->mr.ibmr.lkey);
	qp_context->params1    = cpu_to_be32((MTHCA_ACK_REQ_FREQ << 28) |
					     (MTHCA_FLIGHT_LIMIT << 24) |
					     MTHCA_QP_BIT_SWE);
	if (qp->sq_policy == IB_SIGNAL_ALL_WR)
		qp_context->params1 |= cpu_to_be32(MTHCA_QP_BIT_SSC);
	if (attr_mask & IB_QP_RETRY_CNT) {
		qp_context->params1 |= cpu_to_be32(attr->retry_cnt << 16);
		qp_param->opt_param_mask |= cpu_to_be32(MTHCA_QP_OPTPAR_RETRY_COUNT);
	}

	if (attr_mask & IB_QP_MAX_QP_RD_ATOMIC) {
		if (attr->max_rd_atomic) {
			qp_context->params1 |=
				cpu_to_be32(MTHCA_QP_BIT_SRE |
					    MTHCA_QP_BIT_SAE);
			qp_context->params1 |=
				cpu_to_be32(fls(attr->max_rd_atomic - 1) << 21);
		}
		qp_param->opt_param_mask |= cpu_to_be32(MTHCA_QP_OPTPAR_SRA_MAX);
	}

	if (attr_mask & IB_QP_SQ_PSN)
		qp_context->next_send_psn = cpu_to_be32(attr->sq_psn);
	qp_context->cqn_snd = cpu_to_be32(to_mcq(ibqp->send_cq)->cqn);

	if (mthca_is_memfree(dev)) {
		qp_context->snd_wqe_base_l = cpu_to_be32(qp->send_wqe_offset);
		qp_context->snd_db_index   = cpu_to_be32(qp->sq.db_index);
	}

	if (attr_mask & IB_QP_MAX_DEST_RD_ATOMIC) {
		if (attr->max_dest_rd_atomic)
			qp_context->params2 |=
				cpu_to_be32(fls(attr->max_dest_rd_atomic - 1) << 21);

		qp_param->opt_param_mask |= cpu_to_be32(MTHCA_QP_OPTPAR_RRA_MAX);
	}

	if (attr_mask & (IB_QP_ACCESS_FLAGS | IB_QP_MAX_DEST_RD_ATOMIC)) {
		qp_context->params2      |= get_hw_access_flags(qp, attr, attr_mask);
		qp_param->opt_param_mask |= cpu_to_be32(MTHCA_QP_OPTPAR_RWE |
							MTHCA_QP_OPTPAR_RRE |
							MTHCA_QP_OPTPAR_RAE);
	}

	qp_context->params2 |= cpu_to_be32(MTHCA_QP_BIT_RSC);

	if (ibqp->srq)
		qp_context->params2 |= cpu_to_be32(MTHCA_QP_BIT_RIC);

	if (attr_mask & IB_QP_MIN_RNR_TIMER) {
		qp_context->rnr_nextrecvpsn |= cpu_to_be32(attr->min_rnr_timer << 24);
		qp_param->opt_param_mask |= cpu_to_be32(MTHCA_QP_OPTPAR_RNR_TIMEOUT);
	}
	if (attr_mask & IB_QP_RQ_PSN)
		qp_context->rnr_nextrecvpsn |= cpu_to_be32(attr->rq_psn);

	qp_context->ra_buff_indx =
		cpu_to_be32(dev->qp_table.rdb_base +
			    ((qp->qpn & (dev->limits.num_qps - 1)) * MTHCA_RDB_ENTRY_SIZE <<
			     dev->qp_table.rdb_shift));

	qp_context->cqn_rcv = cpu_to_be32(to_mcq(ibqp->recv_cq)->cqn);

	if (mthca_is_memfree(dev))
		qp_context->rcv_db_index   = cpu_to_be32(qp->rq.db_index);

	if (attr_mask & IB_QP_QKEY) {
		qp_context->qkey = cpu_to_be32(attr->qkey);
		qp_param->opt_param_mask |= cpu_to_be32(MTHCA_QP_OPTPAR_Q_KEY);
	}

	if (ibqp->srq)
		qp_context->srqn = cpu_to_be32(1 << 24 |
					       to_msrq(ibqp->srq)->srqn);

	if (cur_state == IB_QPS_RTS && new_state == IB_QPS_SQD	&&
	    attr_mask & IB_QP_EN_SQD_ASYNC_NOTIFY		&&
	    attr->en_sqd_async_notify)
		sqd_event = 1 << 31;

	err = mthca_MODIFY_QP(dev, cur_state, new_state, qp->qpn, 0,
			      mailbox, sqd_event);
	if (err) {
		mthca_warn(dev, "modify QP %d->%d returned %d.\n",
			   cur_state, new_state, err);
		goto out_mailbox;
	}

	qp->state = new_state;
	if (attr_mask & IB_QP_ACCESS_FLAGS)
		qp->atomic_rd_en = attr->qp_access_flags;
	if (attr_mask & IB_QP_MAX_DEST_RD_ATOMIC)
		qp->resp_depth = attr->max_dest_rd_atomic;
	if (attr_mask & IB_QP_PORT)
		qp->port = attr->port_num;
	if (attr_mask & IB_QP_ALT_PATH)
		qp->alt_port = attr->alt_port_num;

	if (is_sqp(dev, qp))
		store_attrs(qp->sqp, attr, attr_mask);

	/*
	 * If we moved QP0 to RTR, bring the IB link up; if we moved
	 * QP0 to RESET or ERROR, bring the link back down.
	 */
	if (is_qp0(dev, qp)) {
		if (cur_state != IB_QPS_RTR &&
		    new_state == IB_QPS_RTR)
			init_port(dev, qp->port);

		if (cur_state != IB_QPS_RESET &&
		    cur_state != IB_QPS_ERR &&
		    (new_state == IB_QPS_RESET ||
		     new_state == IB_QPS_ERR))
			mthca_CLOSE_IB(dev, qp->port);
	}

	/*
	 * If we moved a kernel QP to RESET, clean up all old CQ
	 * entries and reinitialize the QP.
	 */
	if (new_state == IB_QPS_RESET && !qp->ibqp.uobject) {
		mthca_cq_clean(dev, to_mcq(qp->ibqp.recv_cq), qp->qpn,
			       qp->ibqp.srq ? to_msrq(qp->ibqp.srq) : NULL);
		if (qp->ibqp.send_cq != qp->ibqp.recv_cq)
			mthca_cq_clean(dev, to_mcq(qp->ibqp.send_cq), qp->qpn, NULL);

		mthca_wq_reset(&qp->sq);
		qp->sq.last = get_send_wqe(qp, qp->sq.max - 1);

		mthca_wq_reset(&qp->rq);
		qp->rq.last = get_recv_wqe(qp, qp->rq.max - 1);

		if (mthca_is_memfree(dev)) {
			*qp->sq.db = 0;
			*qp->rq.db = 0;
		}
	}

out_mailbox:
	mthca_free_mailbox(dev, mailbox);
out:
	return err;
}

int mthca_modify_qp(struct ib_qp *ibqp, struct ib_qp_attr *attr, int attr_mask,
		    struct ib_udata *udata)
{
	struct mthca_dev *dev = to_mdev(ibqp->device);
	struct mthca_qp *qp = to_mqp(ibqp);
	enum ib_qp_state cur_state, new_state;
	int err = -EINVAL;

	if (attr_mask & ~IB_QP_ATTR_STANDARD_BITS)
		return -EOPNOTSUPP;

	mutex_lock(&qp->mutex);
	if (attr_mask & IB_QP_CUR_STATE) {
		cur_state = attr->cur_qp_state;
	} else {
		spin_lock_irq(&qp->sq.lock);
		spin_lock(&qp->rq.lock);
		cur_state = qp->state;
		spin_unlock(&qp->rq.lock);
		spin_unlock_irq(&qp->sq.lock);
	}

	new_state = attr_mask & IB_QP_STATE ? attr->qp_state : cur_state;

	if (!ib_modify_qp_is_ok(cur_state, new_state, ibqp->qp_type,
				attr_mask)) {
		mthca_dbg(dev, "Bad QP transition (transport %d) "
			  "%d->%d with attr 0x%08x\n",
			  qp->transport, cur_state, new_state,
			  attr_mask);
		goto out;
	}

	if ((attr_mask & IB_QP_PKEY_INDEX) &&
	     attr->pkey_index >= dev->limits.pkey_table_len) {
		mthca_dbg(dev, "P_Key index (%u) too large. max is %d\n",
			  attr->pkey_index, dev->limits.pkey_table_len-1);
		goto out;
	}

	if ((attr_mask & IB_QP_PORT) &&
	    (attr->port_num == 0 || attr->port_num > dev->limits.num_ports)) {
		mthca_dbg(dev, "Port number (%u) is invalid\n", attr->port_num);
		goto out;
	}

	if (attr_mask & IB_QP_MAX_QP_RD_ATOMIC &&
	    attr->max_rd_atomic > dev->limits.max_qp_init_rdma) {
		mthca_dbg(dev, "Max rdma_atomic as initiator %u too large (max is %d)\n",
			  attr->max_rd_atomic, dev->limits.max_qp_init_rdma);
		goto out;
	}

	if (attr_mask & IB_QP_MAX_DEST_RD_ATOMIC &&
	    attr->max_dest_rd_atomic > 1 << dev->qp_table.rdb_shift) {
		mthca_dbg(dev, "Max rdma_atomic as responder %u too large (max %d)\n",
			  attr->max_dest_rd_atomic, 1 << dev->qp_table.rdb_shift);
		goto out;
	}

	if (cur_state == new_state && cur_state == IB_QPS_RESET) {
		err = 0;
		goto out;
	}

	err = __mthca_modify_qp(ibqp, attr, attr_mask, cur_state, new_state,
				udata);

out:
	mutex_unlock(&qp->mutex);
	return err;
}

static int mthca_max_data_size(struct mthca_dev *dev, struct mthca_qp *qp, int desc_sz)
{
	/*
	 * Calculate the maximum size of WQE s/g segments, excluding
	 * the next segment and other non-data segments.
	 */
	int max_data_size = desc_sz - sizeof (struct mthca_next_seg);

	switch (qp->transport) {
	case MLX:
		max_data_size -= 2 * sizeof (struct mthca_data_seg);
		break;

	case UD:
		if (mthca_is_memfree(dev))
			max_data_size -= sizeof (struct mthca_arbel_ud_seg);
		else
			max_data_size -= sizeof (struct mthca_tavor_ud_seg);
		break;

	default:
		max_data_size -= sizeof (struct mthca_raddr_seg);
		break;
	}

	return max_data_size;
}

static inline int mthca_max_inline_data(struct mthca_pd *pd, int max_data_size)
{
	/* We don't support inline data for kernel QPs (yet). */
	return pd->ibpd.uobject ? max_data_size - MTHCA_INLINE_HEADER_SIZE : 0;
}

static void mthca_adjust_qp_caps(struct mthca_dev *dev,
				 struct mthca_pd *pd,
				 struct mthca_qp *qp)
{
	int max_data_size = mthca_max_data_size(dev, qp,
						min(dev->limits.max_desc_sz,
						    1 << qp->sq.wqe_shift));

	qp->max_inline_data = mthca_max_inline_data(pd, max_data_size);

	qp->sq.max_gs = min_t(int, dev->limits.max_sg,
			      max_data_size / sizeof (struct mthca_data_seg));
	qp->rq.max_gs = min_t(int, dev->limits.max_sg,
			       (min(dev->limits.max_desc_sz, 1 << qp->rq.wqe_shift) -
				sizeof (struct mthca_next_seg)) /
			       sizeof (struct mthca_data_seg));
}

/*
 * Allocate and register buffer for WQEs.  qp->rq.max, sq.max,
 * rq.max_gs and sq.max_gs must all be assigned.
 * mthca_alloc_wqe_buf will calculate rq.wqe_shift and
 * sq.wqe_shift (as well as send_wqe_offset, is_direct, and
 * queue)
 */
static int mthca_alloc_wqe_buf(struct mthca_dev *dev,
			       struct mthca_pd *pd,
			       struct mthca_qp *qp,
			       struct ib_udata *udata)
{
	int size;
	int err = -ENOMEM;

	size = sizeof (struct mthca_next_seg) +
		qp->rq.max_gs * sizeof (struct mthca_data_seg);

	if (size > dev->limits.max_desc_sz)
		return -EINVAL;

	for (qp->rq.wqe_shift = 6; 1 << qp->rq.wqe_shift < size;
	     qp->rq.wqe_shift++)
		; /* nothing */

	size = qp->sq.max_gs * sizeof (struct mthca_data_seg);
	switch (qp->transport) {
	case MLX:
		size += 2 * sizeof (struct mthca_data_seg);
		break;

	case UD:
		size += mthca_is_memfree(dev) ?
			sizeof (struct mthca_arbel_ud_seg) :
			sizeof (struct mthca_tavor_ud_seg);
		break;

	case UC:
		size += sizeof (struct mthca_raddr_seg);
		break;

	case RC:
		size += sizeof (struct mthca_raddr_seg);
		/*
		 * An atomic op will require an atomic segment, a
		 * remote address segment and one scatter entry.
		 */
		size = max_t(int, size,
			     sizeof (struct mthca_atomic_seg) +
			     sizeof (struct mthca_raddr_seg) +
			     sizeof (struct mthca_data_seg));
		break;

	default:
		break;
	}

	/* Make sure that we have enough space for a bind request */
	size = max_t(int, size, sizeof (struct mthca_bind_seg));

	size += sizeof (struct mthca_next_seg);

	if (size > dev->limits.max_desc_sz)
		return -EINVAL;

	for (qp->sq.wqe_shift = 6; 1 << qp->sq.wqe_shift < size;
	     qp->sq.wqe_shift++)
		; /* nothing */

	qp->send_wqe_offset = ALIGN(qp->rq.max << qp->rq.wqe_shift,
				    1 << qp->sq.wqe_shift);

	/*
	 * If this is a userspace QP, we don't actually have to
	 * allocate anything.  All we need is to calculate the WQE
	 * sizes and the send_wqe_offset, so we're done now.
	 */
	if (udata)
		return 0;

	size = PAGE_ALIGN(qp->send_wqe_offset +
			  (qp->sq.max << qp->sq.wqe_shift));

	qp->wrid = kmalloc_array(qp->rq.max + qp->sq.max, sizeof(u64),
				 GFP_KERNEL);
	if (!qp->wrid)
		goto err_out;

	err = mthca_buf_alloc(dev, size, MTHCA_MAX_DIRECT_QP_SIZE,
			      &qp->queue, &qp->is_direct, pd, 0, &qp->mr);
	if (err)
		goto err_out;

	return 0;

err_out:
	kfree(qp->wrid);
	return err;
}

static void mthca_free_wqe_buf(struct mthca_dev *dev,
			       struct mthca_qp *qp)
{
	mthca_buf_free(dev, PAGE_ALIGN(qp->send_wqe_offset +
				       (qp->sq.max << qp->sq.wqe_shift)),
		       &qp->queue, qp->is_direct, &qp->mr);
	kfree(qp->wrid);
}

static int mthca_map_memfree(struct mthca_dev *dev,
			     struct mthca_qp *qp)
{
	int ret;

	if (mthca_is_memfree(dev)) {
		ret = mthca_table_get(dev, dev->qp_table.qp_table, qp->qpn);
		if (ret)
			return ret;

		ret = mthca_table_get(dev, dev->qp_table.eqp_table, qp->qpn);
		if (ret)
			goto err_qpc;

		ret = mthca_table_get(dev, dev->qp_table.rdb_table,
				      qp->qpn << dev->qp_table.rdb_shift);
		if (ret)
			goto err_eqpc;

	}

	return 0;

err_eqpc:
	mthca_table_put(dev, dev->qp_table.eqp_table, qp->qpn);

err_qpc:
	mthca_table_put(dev, dev->qp_table.qp_table, qp->qpn);

	return ret;
}

static void mthca_unmap_memfree(struct mthca_dev *dev,
				struct mthca_qp *qp)
{
	mthca_table_put(dev, dev->qp_table.rdb_table,
			qp->qpn << dev->qp_table.rdb_shift);
	mthca_table_put(dev, dev->qp_table.eqp_table, qp->qpn);
	mthca_table_put(dev, dev->qp_table.qp_table, qp->qpn);
}

static int mthca_alloc_memfree(struct mthca_dev *dev,
			       struct mthca_qp *qp)
{
	if (mthca_is_memfree(dev)) {
		qp->rq.db_index = mthca_alloc_db(dev, MTHCA_DB_TYPE_RQ,
						 qp->qpn, &qp->rq.db);
		if (qp->rq.db_index < 0)
			return -ENOMEM;

		qp->sq.db_index = mthca_alloc_db(dev, MTHCA_DB_TYPE_SQ,
						 qp->qpn, &qp->sq.db);
		if (qp->sq.db_index < 0) {
			mthca_free_db(dev, MTHCA_DB_TYPE_RQ, qp->rq.db_index);
			return -ENOMEM;
		}
	}

	return 0;
}

static void mthca_free_memfree(struct mthca_dev *dev,
			       struct mthca_qp *qp)
{
	if (mthca_is_memfree(dev)) {
		mthca_free_db(dev, MTHCA_DB_TYPE_SQ, qp->sq.db_index);
		mthca_free_db(dev, MTHCA_DB_TYPE_RQ, qp->rq.db_index);
	}
}

static int mthca_alloc_qp_common(struct mthca_dev *dev,
				 struct mthca_pd *pd,
				 struct mthca_cq *send_cq,
				 struct mthca_cq *recv_cq,
				 enum ib_sig_type send_policy,
				 struct mthca_qp *qp,
				 struct ib_udata *udata)
{
	int ret;
	int i;
	struct mthca_next_seg *next;

	qp->refcount = 1;
	init_waitqueue_head(&qp->wait);
	mutex_init(&qp->mutex);
	qp->state    	 = IB_QPS_RESET;
	qp->atomic_rd_en = 0;
	qp->resp_depth   = 0;
	qp->sq_policy    = send_policy;
	mthca_wq_reset(&qp->sq);
	mthca_wq_reset(&qp->rq);

	spin_lock_init(&qp->sq.lock);
	spin_lock_init(&qp->rq.lock);

	ret = mthca_map_memfree(dev, qp);
	if (ret)
		return ret;

	ret = mthca_alloc_wqe_buf(dev, pd, qp, udata);
	if (ret) {
		mthca_unmap_memfree(dev, qp);
		return ret;
	}

	mthca_adjust_qp_caps(dev, pd, qp);

	/*
	 * If this is a userspace QP, we're done now.  The doorbells
	 * will be allocated and buffers will be initialized in
	 * userspace.
	 */
	if (udata)
		return 0;

	ret = mthca_alloc_memfree(dev, qp);
	if (ret) {
		mthca_free_wqe_buf(dev, qp);
		mthca_unmap_memfree(dev, qp);
		return ret;
	}

	if (mthca_is_memfree(dev)) {
		struct mthca_data_seg *scatter;
		int size = (sizeof (struct mthca_next_seg) +
			    qp->rq.max_gs * sizeof (struct mthca_data_seg)) / 16;

		for (i = 0; i < qp->rq.max; ++i) {
			next = get_recv_wqe(qp, i);
			next->nda_op = cpu_to_be32(((i + 1) & (qp->rq.max - 1)) <<
						   qp->rq.wqe_shift);
			next->ee_nds = cpu_to_be32(size);

			for (scatter = (void *) (next + 1);
			     (void *) scatter < (void *) next + (1 << qp->rq.wqe_shift);
			     ++scatter)
				scatter->lkey = cpu_to_be32(MTHCA_INVAL_LKEY);
		}

		for (i = 0; i < qp->sq.max; ++i) {
			next = get_send_wqe(qp, i);
			next->nda_op = cpu_to_be32((((i + 1) & (qp->sq.max - 1)) <<
						    qp->sq.wqe_shift) +
						   qp->send_wqe_offset);
		}
	} else {
		for (i = 0; i < qp->rq.max; ++i) {
			next = get_recv_wqe(qp, i);
			next->nda_op = htonl((((i + 1) % qp->rq.max) <<
					      qp->rq.wqe_shift) | 1);
		}

	}

	qp->sq.last = get_send_wqe(qp, qp->sq.max - 1);
	qp->rq.last = get_recv_wqe(qp, qp->rq.max - 1);

	return 0;
}

static int mthca_set_qp_size(struct mthca_dev *dev, struct ib_qp_cap *cap,
			     struct mthca_pd *pd, struct mthca_qp *qp)
{
	int max_data_size = mthca_max_data_size(dev, qp, dev->limits.max_desc_sz);

	/* Sanity check QP size before proceeding */
	if (cap->max_send_wr  	 > dev->limits.max_wqes ||
	    cap->max_recv_wr  	 > dev->limits.max_wqes ||
	    cap->max_send_sge 	 > dev->limits.max_sg   ||
	    cap->max_recv_sge 	 > dev->limits.max_sg   ||
	    cap->max_inline_data > mthca_max_inline_data(pd, max_data_size))
		return -EINVAL;

	/*
	 * For MLX transport we need 2 extra send gather entries:
	 * one for the header and one for the checksum at the end
	 */
	if (qp->transport == MLX && cap->max_send_sge + 2 > dev->limits.max_sg)
		return -EINVAL;

	if (mthca_is_memfree(dev)) {
		qp->rq.max = cap->max_recv_wr ?
			roundup_pow_of_two(cap->max_recv_wr) : 0;
		qp->sq.max = cap->max_send_wr ?
			roundup_pow_of_two(cap->max_send_wr) : 0;
	} else {
		qp->rq.max = cap->max_recv_wr;
		qp->sq.max = cap->max_send_wr;
	}

	qp->rq.max_gs = cap->max_recv_sge;
	qp->sq.max_gs = max_t(int, cap->max_send_sge,
			      ALIGN(cap->max_inline_data + MTHCA_INLINE_HEADER_SIZE,
				    MTHCA_INLINE_CHUNK_SIZE) /
			      sizeof (struct mthca_data_seg));

	return 0;
}

int mthca_alloc_qp(struct mthca_dev *dev,
		   struct mthca_pd *pd,
		   struct mthca_cq *send_cq,
		   struct mthca_cq *recv_cq,
		   enum ib_qp_type type,
		   enum ib_sig_type send_policy,
		   struct ib_qp_cap *cap,
		   struct mthca_qp *qp,
		   struct ib_udata *udata)
{
	int err;

	switch (type) {
	case IB_QPT_RC: qp->transport = RC; break;
	case IB_QPT_UC: qp->transport = UC; break;
	case IB_QPT_UD: qp->transport = UD; break;
	default: return -EINVAL;
	}

	err = mthca_set_qp_size(dev, cap, pd, qp);
	if (err)
		return err;

	qp->qpn = mthca_alloc(&dev->qp_table.alloc);
	if (qp->qpn == -1)
		return -ENOMEM;

	/* initialize port to zero for error-catching. */
	qp->port = 0;

	err = mthca_alloc_qp_common(dev, pd, send_cq, recv_cq,
				    send_policy, qp, udata);
	if (err) {
		mthca_free(&dev->qp_table.alloc, qp->qpn);
		return err;
	}

	spin_lock_irq(&dev->qp_table.lock);
	mthca_array_set(&dev->qp_table.qp,
			qp->qpn & (dev->limits.num_qps - 1), qp);
	spin_unlock_irq(&dev->qp_table.lock);

	return 0;
}

static void mthca_lock_cqs(struct mthca_cq *send_cq, struct mthca_cq *recv_cq)
	__acquires(&send_cq->lock) __acquires(&recv_cq->lock)
{
	if (send_cq == recv_cq) {
		spin_lock_irq(&send_cq->lock);
		__acquire(&recv_cq->lock);
	} else if (send_cq->cqn < recv_cq->cqn) {
		spin_lock_irq(&send_cq->lock);
		spin_lock_nested(&recv_cq->lock, SINGLE_DEPTH_NESTING);
	} else {
		spin_lock_irq(&recv_cq->lock);
		spin_lock_nested(&send_cq->lock, SINGLE_DEPTH_NESTING);
	}
}

static void mthca_unlock_cqs(struct mthca_cq *send_cq, struct mthca_cq *recv_cq)
	__releases(&send_cq->lock) __releases(&recv_cq->lock)
{
	if (send_cq == recv_cq) {
		__release(&recv_cq->lock);
		spin_unlock_irq(&send_cq->lock);
	} else if (send_cq->cqn < recv_cq->cqn) {
		spin_unlock(&recv_cq->lock);
		spin_unlock_irq(&send_cq->lock);
	} else {
		spin_unlock(&send_cq->lock);
		spin_unlock_irq(&recv_cq->lock);
	}
}

int mthca_alloc_sqp(struct mthca_dev *dev,
		    struct mthca_pd *pd,
		    struct mthca_cq *send_cq,
		    struct mthca_cq *recv_cq,
		    enum ib_sig_type send_policy,
		    struct ib_qp_cap *cap,
		    int qpn,
		    u32 port,
		    struct mthca_qp *qp,
		    struct ib_udata *udata)
{
	u32 mqpn = qpn * 2 + dev->qp_table.sqp_start + port - 1;
	int err;

	qp->transport = MLX;
	err = mthca_set_qp_size(dev, cap, pd, qp);
	if (err)
		return err;

	qp->sqp->header_buf_size = qp->sq.max * MTHCA_UD_HEADER_SIZE;
	qp->sqp->header_buf =
		dma_alloc_coherent(&dev->pdev->dev, qp->sqp->header_buf_size,
				   &qp->sqp->header_dma, GFP_KERNEL);
	if (!qp->sqp->header_buf)
		return -ENOMEM;

	spin_lock_irq(&dev->qp_table.lock);
	if (mthca_array_get(&dev->qp_table.qp, mqpn))
		err = -EBUSY;
	else
		mthca_array_set(&dev->qp_table.qp, mqpn, qp->sqp);
	spin_unlock_irq(&dev->qp_table.lock);

	if (err)
		goto err_out;

	qp->port      = port;
	qp->qpn       = mqpn;
	qp->transport = MLX;

	err = mthca_alloc_qp_common(dev, pd, send_cq, recv_cq,
				    send_policy, qp, udata);
	if (err)
		goto err_out_free;

	atomic_inc(&pd->sqp_count);

	return 0;

 err_out_free:
	/*
	 * Lock CQs here, so that CQ polling code can do QP lookup
	 * without taking a lock.
	 */
	mthca_lock_cqs(send_cq, recv_cq);

	spin_lock(&dev->qp_table.lock);
	mthca_array_clear(&dev->qp_table.qp, mqpn);
	spin_unlock(&dev->qp_table.lock);

	mthca_unlock_cqs(send_cq, recv_cq);

err_out:
	dma_free_coherent(&dev->pdev->dev, qp->sqp->header_buf_size,
			  qp->sqp->header_buf, qp->sqp->header_dma);
	return err;
}

static inline int get_qp_refcount(struct mthca_dev *dev, struct mthca_qp *qp)
{
	int c;

	spin_lock_irq(&dev->qp_table.lock);
	c = qp->refcount;
	spin_unlock_irq(&dev->qp_table.lock);

	return c;
}

void mthca_free_qp(struct mthca_dev *dev,
		   struct mthca_qp *qp)
{
	struct mthca_cq *send_cq;
	struct mthca_cq *recv_cq;

	send_cq = to_mcq(qp->ibqp.send_cq);
	recv_cq = to_mcq(qp->ibqp.recv_cq);

	/*
	 * Lock CQs here, so that CQ polling code can do QP lookup
	 * without taking a lock.
	 */
	mthca_lock_cqs(send_cq, recv_cq);

	spin_lock(&dev->qp_table.lock);
	mthca_array_clear(&dev->qp_table.qp,
			  qp->qpn & (dev->limits.num_qps - 1));
	--qp->refcount;
	spin_unlock(&dev->qp_table.lock);

	mthca_unlock_cqs(send_cq, recv_cq);

	wait_event(qp->wait, !get_qp_refcount(dev, qp));

	if (qp->state != IB_QPS_RESET)
		mthca_MODIFY_QP(dev, qp->state, IB_QPS_RESET, qp->qpn, 0,
				NULL, 0);

	/*
	 * If this is a userspace QP, the buffers, MR, CQs and so on
	 * will be cleaned up in userspace, so all we have to do is
	 * unref the mem-free tables and free the QPN in our table.
	 */
	if (!qp->ibqp.uobject) {
		mthca_cq_clean(dev, recv_cq, qp->qpn,
			       qp->ibqp.srq ? to_msrq(qp->ibqp.srq) : NULL);
		if (send_cq != recv_cq)
			mthca_cq_clean(dev, send_cq, qp->qpn, NULL);

		mthca_free_memfree(dev, qp);
		mthca_free_wqe_buf(dev, qp);
	}

	mthca_unmap_memfree(dev, qp);

	if (is_sqp(dev, qp)) {
		atomic_dec(&(to_mpd(qp->ibqp.pd)->sqp_count));
		dma_free_coherent(&dev->pdev->dev, qp->sqp->header_buf_size,
				  qp->sqp->header_buf, qp->sqp->header_dma);
	} else
		mthca_free(&dev->qp_table.alloc, qp->qpn);
}

/* Create UD header for an MLX send and build a data segment for it */
static int build_mlx_header(struct mthca_dev *dev, struct mthca_qp *qp, int ind,
			    const struct ib_ud_wr *wr,
			    struct mthca_mlx_seg *mlx,
			    struct mthca_data_seg *data)
{
	struct mthca_sqp *sqp = qp->sqp;
	int header_size;
	int err;
	u16 pkey;

	ib_ud_header_init(256, /* assume a MAD */ 1, 0, 0,
			  mthca_ah_grh_present(to_mah(wr->ah)), 0, 0, 0,
			  &sqp->ud_header);

	err = mthca_read_ah(dev, to_mah(wr->ah), &sqp->ud_header);
	if (err)
		return err;
	mlx->flags &= ~cpu_to_be32(MTHCA_NEXT_SOLICIT | 1);
	mlx->flags |= cpu_to_be32((!qp->ibqp.qp_num ? MTHCA_MLX_VL15 : 0) |
				  (sqp->ud_header.lrh.destination_lid ==
				   IB_LID_PERMISSIVE ? MTHCA_MLX_SLR : 0) |
				  (sqp->ud_header.lrh.service_level << 8));
	mlx->rlid = sqp->ud_header.lrh.destination_lid;
	mlx->vcrc = 0;

	switch (wr->wr.opcode) {
	case IB_WR_SEND:
		sqp->ud_header.bth.opcode = IB_OPCODE_UD_SEND_ONLY;
		sqp->ud_header.immediate_present = 0;
		break;
	case IB_WR_SEND_WITH_IMM:
		sqp->ud_header.bth.opcode = IB_OPCODE_UD_SEND_ONLY_WITH_IMMEDIATE;
		sqp->ud_header.immediate_present = 1;
		sqp->ud_header.immediate_data = wr->wr.ex.imm_data;
		break;
	default:
		return -EINVAL;
	}

	sqp->ud_header.lrh.virtual_lane    = !qp->ibqp.qp_num ? 15 : 0;
	if (sqp->ud_header.lrh.destination_lid == IB_LID_PERMISSIVE)
		sqp->ud_header.lrh.source_lid = IB_LID_PERMISSIVE;
	sqp->ud_header.bth.solicited_event = !!(wr->wr.send_flags & IB_SEND_SOLICITED);
	if (!qp->ibqp.qp_num)
		ib_get_cached_pkey(&dev->ib_dev, qp->port, sqp->pkey_index,
				   &pkey);
	else
		ib_get_cached_pkey(&dev->ib_dev, qp->port, wr->pkey_index,
				   &pkey);
	sqp->ud_header.bth.pkey = cpu_to_be16(pkey);
	sqp->ud_header.bth.destination_qpn = cpu_to_be32(wr->remote_qpn);
	sqp->ud_header.bth.psn = cpu_to_be32((sqp->send_psn++) & ((1 << 24) - 1));
	sqp->ud_header.deth.qkey = cpu_to_be32(wr->remote_qkey & 0x80000000 ?
					       sqp->qkey : wr->remote_qkey);
	sqp->ud_header.deth.source_qpn = cpu_to_be32(qp->ibqp.qp_num);

	header_size = ib_ud_header_pack(&sqp->ud_header,
					sqp->header_buf +
					ind * MTHCA_UD_HEADER_SIZE);

	data->byte_count = cpu_to_be32(header_size);
	data->lkey       = cpu_to_be32(to_mpd(qp->ibqp.pd)->ntmr.ibmr.lkey);
	data->addr       = cpu_to_be64(sqp->header_dma +
				       ind * MTHCA_UD_HEADER_SIZE);

	return 0;
}

static inline int mthca_wq_overflow(struct mthca_wq *wq, int nreq,
				    struct ib_cq *ib_cq)
{
	unsigned cur;
	struct mthca_cq *cq;

	cur = wq->head - wq->tail;
	if (likely(cur + nreq < wq->max))
		return 0;

	cq = to_mcq(ib_cq);
	spin_lock(&cq->lock);
	cur = wq->head - wq->tail;
	spin_unlock(&cq->lock);

	return cur + nreq >= wq->max;
}

static __always_inline void set_raddr_seg(struct mthca_raddr_seg *rseg,
					  u64 remote_addr, u32 rkey)
{
	rseg->raddr    = cpu_to_be64(remote_addr);
	rseg->rkey     = cpu_to_be32(rkey);
	rseg->reserved = 0;
}

static __always_inline void set_atomic_seg(struct mthca_atomic_seg *aseg,
					   const struct ib_atomic_wr *wr)
{
	if (wr->wr.opcode == IB_WR_ATOMIC_CMP_AND_SWP) {
		aseg->swap_add = cpu_to_be64(wr->swap);
		aseg->compare  = cpu_to_be64(wr->compare_add);
	} else {
		aseg->swap_add = cpu_to_be64(wr->compare_add);
		aseg->compare  = 0;
	}

}

static void set_tavor_ud_seg(struct mthca_tavor_ud_seg *useg,
			     const struct ib_ud_wr *wr)
{
	useg->lkey    = cpu_to_be32(to_mah(wr->ah)->key);
	useg->av_addr =	cpu_to_be64(to_mah(wr->ah)->avdma);
	useg->dqpn    =	cpu_to_be32(wr->remote_qpn);
	useg->qkey    =	cpu_to_be32(wr->remote_qkey);

}

static void set_arbel_ud_seg(struct mthca_arbel_ud_seg *useg,
			     const struct ib_ud_wr *wr)
{
	memcpy(useg->av, to_mah(wr->ah)->av, MTHCA_AV_SIZE);
	useg->dqpn = cpu_to_be32(wr->remote_qpn);
	useg->qkey = cpu_to_be32(wr->remote_qkey);
}

int mthca_tavor_post_send(struct ib_qp *ibqp, const struct ib_send_wr *wr,
			  const struct ib_send_wr **bad_wr)
{
	struct mthca_dev *dev = to_mdev(ibqp->device);
	struct mthca_qp *qp = to_mqp(ibqp);
	void *wqe;
	void *prev_wqe;
	unsigned long flags;
	int err = 0;
	int nreq;
	int i;
	int size;
	/*
	 * f0 and size0 are only used if nreq != 0, and they will
	 * always be initialized the first time through the main loop
	 * before nreq is incremented.  So nreq cannot become non-zero
	 * without initializing f0 and size0, and they are in fact
	 * never used uninitialized.
	 */
	int size0;
	u32 f0;
	int ind;
	u8 op0 = 0;

	spin_lock_irqsave(&qp->sq.lock, flags);

	/* XXX check that state is OK to post send */

	ind = qp->sq.next_ind;

	for (nreq = 0; wr; ++nreq, wr = wr->next) {
		if (mthca_wq_overflow(&qp->sq, nreq, qp->ibqp.send_cq)) {
			mthca_err(dev, "SQ %06x full (%u head, %u tail,"
					" %d max, %d nreq)\n", qp->qpn,
					qp->sq.head, qp->sq.tail,
					qp->sq.max, nreq);
			err = -ENOMEM;
			*bad_wr = wr;
			goto out;
		}

		wqe = get_send_wqe(qp, ind);
		prev_wqe = qp->sq.last;
		qp->sq.last = wqe;

		((struct mthca_next_seg *) wqe)->nda_op = 0;
		((struct mthca_next_seg *) wqe)->ee_nds = 0;
		((struct mthca_next_seg *) wqe)->flags =
			((wr->send_flags & IB_SEND_SIGNALED) ?
			 cpu_to_be32(MTHCA_NEXT_CQ_UPDATE) : 0) |
			((wr->send_flags & IB_SEND_SOLICITED) ?
			 cpu_to_be32(MTHCA_NEXT_SOLICIT) : 0)   |
			cpu_to_be32(1);
		if (wr->opcode == IB_WR_SEND_WITH_IMM ||
		    wr->opcode == IB_WR_RDMA_WRITE_WITH_IMM)
			((struct mthca_next_seg *) wqe)->imm = wr->ex.imm_data;

		wqe += sizeof (struct mthca_next_seg);
		size = sizeof (struct mthca_next_seg) / 16;

		switch (qp->transport) {
		case RC:
			switch (wr->opcode) {
			case IB_WR_ATOMIC_CMP_AND_SWP:
			case IB_WR_ATOMIC_FETCH_AND_ADD:
				set_raddr_seg(wqe, atomic_wr(wr)->remote_addr,
					      atomic_wr(wr)->rkey);
				wqe += sizeof (struct mthca_raddr_seg);

				set_atomic_seg(wqe, atomic_wr(wr));
				wqe += sizeof (struct mthca_atomic_seg);
				size += (sizeof (struct mthca_raddr_seg) +
					 sizeof (struct mthca_atomic_seg)) / 16;
				break;

			case IB_WR_RDMA_WRITE:
			case IB_WR_RDMA_WRITE_WITH_IMM:
			case IB_WR_RDMA_READ:
				set_raddr_seg(wqe, rdma_wr(wr)->remote_addr,
					      rdma_wr(wr)->rkey);
				wqe  += sizeof (struct mthca_raddr_seg);
				size += sizeof (struct mthca_raddr_seg) / 16;
				break;

			default:
				/* No extra segments required for sends */
				break;
			}

			break;

		case UC:
			switch (wr->opcode) {
			case IB_WR_RDMA_WRITE:
			case IB_WR_RDMA_WRITE_WITH_IMM:
				set_raddr_seg(wqe, rdma_wr(wr)->remote_addr,
					      rdma_wr(wr)->rkey);
				wqe  += sizeof (struct mthca_raddr_seg);
				size += sizeof (struct mthca_raddr_seg) / 16;
				break;

			default:
				/* No extra segments required for sends */
				break;
			}

			break;

		case UD:
			set_tavor_ud_seg(wqe, ud_wr(wr));
			wqe  += sizeof (struct mthca_tavor_ud_seg);
			size += sizeof (struct mthca_tavor_ud_seg) / 16;
			break;

		case MLX:
			err = build_mlx_header(
				dev, qp, ind, ud_wr(wr),
				wqe - sizeof(struct mthca_next_seg), wqe);
			if (err) {
				*bad_wr = wr;
				goto out;
			}
			wqe += sizeof (struct mthca_data_seg);
			size += sizeof (struct mthca_data_seg) / 16;
			break;
		}

		if (wr->num_sge > qp->sq.max_gs) {
			mthca_err(dev, "too many gathers\n");
			err = -EINVAL;
			*bad_wr = wr;
			goto out;
		}

		for (i = 0; i < wr->num_sge; ++i) {
			mthca_set_data_seg(wqe, wr->sg_list + i);
			wqe  += sizeof (struct mthca_data_seg);
			size += sizeof (struct mthca_data_seg) / 16;
		}

		/* Add one more inline data segment for ICRC */
		if (qp->transport == MLX) {
			((struct mthca_data_seg *) wqe)->byte_count =
				cpu_to_be32((1 << 31) | 4);
			((u32 *) wqe)[1] = 0;
			wqe += sizeof (struct mthca_data_seg);
			size += sizeof (struct mthca_data_seg) / 16;
		}

		qp->wrid[ind + qp->rq.max] = wr->wr_id;

		if (wr->opcode >= ARRAY_SIZE(mthca_opcode)) {
			mthca_err(dev, "opcode invalid\n");
			err = -EINVAL;
			*bad_wr = wr;
			goto out;
		}

		((struct mthca_next_seg *) prev_wqe)->nda_op =
			cpu_to_be32(((ind << qp->sq.wqe_shift) +
				     qp->send_wqe_offset) |
				    mthca_opcode[wr->opcode]);
		wmb();
		((struct mthca_next_seg *) prev_wqe)->ee_nds =
			cpu_to_be32((nreq ? 0 : MTHCA_NEXT_DBD) | size |
				    ((wr->send_flags & IB_SEND_FENCE) ?
				    MTHCA_NEXT_FENCE : 0));

		if (!nreq) {
			size0 = size;
			op0   = mthca_opcode[wr->opcode];
			f0    = wr->send_flags & IB_SEND_FENCE ?
				MTHCA_SEND_DOORBELL_FENCE : 0;
		}

		++ind;
		if (unlikely(ind >= qp->sq.max))
			ind -= qp->sq.max;
	}

out:
	if (likely(nreq)) {
		wmb();

		mthca_write64(((qp->sq.next_ind << qp->sq.wqe_shift) +
			       qp->send_wqe_offset) | f0 | op0,
			      (qp->qpn << 8) | size0,
			      dev->kar + MTHCA_SEND_DOORBELL,
			      MTHCA_GET_DOORBELL_LOCK(&dev->doorbell_lock));
	}

	qp->sq.next_ind = ind;
	qp->sq.head    += nreq;

	spin_unlock_irqrestore(&qp->sq.lock, flags);
	return err;
}

int mthca_tavor_post_receive(struct ib_qp *ibqp, const struct ib_recv_wr *wr,
			     const struct ib_recv_wr **bad_wr)
{
	struct mthca_dev *dev = to_mdev(ibqp->device);
	struct mthca_qp *qp = to_mqp(ibqp);
	unsigned long flags;
	int err = 0;
	int nreq;
	int i;
	int size;
	/*
	 * size0 is only used if nreq != 0, and it will always be
	 * initialized the first time through the main loop before
	 * nreq is incremented.  So nreq cannot become non-zero
	 * without initializing size0, and it is in fact never used
	 * uninitialized.
	 */
	int size0;
	int ind;
	void *wqe;
	void *prev_wqe;

	spin_lock_irqsave(&qp->rq.lock, flags);

	/* XXX check that state is OK to post receive */

	ind = qp->rq.next_ind;

	for (nreq = 0; wr; wr = wr->next) {
		if (mthca_wq_overflow(&qp->rq, nreq, qp->ibqp.recv_cq)) {
			mthca_err(dev, "RQ %06x full (%u head, %u tail,"
					" %d max, %d nreq)\n", qp->qpn,
					qp->rq.head, qp->rq.tail,
					qp->rq.max, nreq);
			err = -ENOMEM;
			*bad_wr = wr;
			goto out;
		}

		wqe = get_recv_wqe(qp, ind);
		prev_wqe = qp->rq.last;
		qp->rq.last = wqe;

		((struct mthca_next_seg *) wqe)->ee_nds =
			cpu_to_be32(MTHCA_NEXT_DBD);
		((struct mthca_next_seg *) wqe)->flags = 0;

		wqe += sizeof (struct mthca_next_seg);
		size = sizeof (struct mthca_next_seg) / 16;

		if (unlikely(wr->num_sge > qp->rq.max_gs)) {
			err = -EINVAL;
			*bad_wr = wr;
			goto out;
		}

		for (i = 0; i < wr->num_sge; ++i) {
			mthca_set_data_seg(wqe, wr->sg_list + i);
			wqe  += sizeof (struct mthca_data_seg);
			size += sizeof (struct mthca_data_seg) / 16;
		}

		qp->wrid[ind] = wr->wr_id;

		((struct mthca_next_seg *) prev_wqe)->ee_nds =
			cpu_to_be32(MTHCA_NEXT_DBD | size);

		if (!nreq)
			size0 = size;

		++ind;
		if (unlikely(ind >= qp->rq.max))
			ind -= qp->rq.max;

		++nreq;
		if (unlikely(nreq == MTHCA_TAVOR_MAX_WQES_PER_RECV_DB)) {
			nreq = 0;

			wmb();

			mthca_write64((qp->rq.next_ind << qp->rq.wqe_shift) | size0,
				      qp->qpn << 8, dev->kar + MTHCA_RECEIVE_DOORBELL,
				      MTHCA_GET_DOORBELL_LOCK(&dev->doorbell_lock));

			qp->rq.next_ind = ind;
			qp->rq.head += MTHCA_TAVOR_MAX_WQES_PER_RECV_DB;
		}
	}

out:
	if (likely(nreq)) {
		wmb();

		mthca_write64((qp->rq.next_ind << qp->rq.wqe_shift) | size0,
			      qp->qpn << 8 | nreq, dev->kar + MTHCA_RECEIVE_DOORBELL,
			      MTHCA_GET_DOORBELL_LOCK(&dev->doorbell_lock));
	}

	qp->rq.next_ind = ind;
	qp->rq.head    += nreq;

	spin_unlock_irqrestore(&qp->rq.lock, flags);
	return err;
}

int mthca_arbel_post_send(struct ib_qp *ibqp, const struct ib_send_wr *wr,
			  const struct ib_send_wr **bad_wr)
{
	struct mthca_dev *dev = to_mdev(ibqp->device);
	struct mthca_qp *qp = to_mqp(ibqp);
	u32 dbhi;
	void *wqe;
	void *prev_wqe;
	unsigned long flags;
	int err = 0;
	int nreq;
	int i;
	int size;
	/*
	 * f0 and size0 are only used if nreq != 0, and they will
	 * always be initialized the first time through the main loop
	 * before nreq is incremented.  So nreq cannot become non-zero
	 * without initializing f0 and size0, and they are in fact
	 * never used uninitialized.
	 */
	int size0;
	u32 f0;
	int ind;
	u8 op0 = 0;

	spin_lock_irqsave(&qp->sq.lock, flags);

	/* XXX check that state is OK to post send */

	ind = qp->sq.head & (qp->sq.max - 1);

	for (nreq = 0; wr; ++nreq, wr = wr->next) {
		if (unlikely(nreq == MTHCA_ARBEL_MAX_WQES_PER_SEND_DB)) {
			nreq = 0;

			dbhi = (MTHCA_ARBEL_MAX_WQES_PER_SEND_DB << 24) |
				((qp->sq.head & 0xffff) << 8) | f0 | op0;

			qp->sq.head += MTHCA_ARBEL_MAX_WQES_PER_SEND_DB;

			/*
			 * Make sure that descriptors are written before
			 * doorbell record.
			 */
			wmb();
			*qp->sq.db = cpu_to_be32(qp->sq.head & 0xffff);

			/*
			 * Make sure doorbell record is written before we
			 * write MMIO send doorbell.
			 */
			wmb();

			mthca_write64(dbhi, (qp->qpn << 8) | size0,
				      dev->kar + MTHCA_SEND_DOORBELL,
				      MTHCA_GET_DOORBELL_LOCK(&dev->doorbell_lock));
		}

		if (mthca_wq_overflow(&qp->sq, nreq, qp->ibqp.send_cq)) {
			mthca_err(dev, "SQ %06x full (%u head, %u tail,"
					" %d max, %d nreq)\n", qp->qpn,
					qp->sq.head, qp->sq.tail,
					qp->sq.max, nreq);
			err = -ENOMEM;
			*bad_wr = wr;
			goto out;
		}

		wqe = get_send_wqe(qp, ind);
		prev_wqe = qp->sq.last;
		qp->sq.last = wqe;

		((struct mthca_next_seg *) wqe)->flags =
			((wr->send_flags & IB_SEND_SIGNALED) ?
			 cpu_to_be32(MTHCA_NEXT_CQ_UPDATE) : 0) |
			((wr->send_flags & IB_SEND_SOLICITED) ?
			 cpu_to_be32(MTHCA_NEXT_SOLICIT) : 0)   |
			((wr->send_flags & IB_SEND_IP_CSUM) ?
			 cpu_to_be32(MTHCA_NEXT_IP_CSUM | MTHCA_NEXT_TCP_UDP_CSUM) : 0) |
			cpu_to_be32(1);
		if (wr->opcode == IB_WR_SEND_WITH_IMM ||
		    wr->opcode == IB_WR_RDMA_WRITE_WITH_IMM)
			((struct mthca_next_seg *) wqe)->imm = wr->ex.imm_data;

		wqe += sizeof (struct mthca_next_seg);
		size = sizeof (struct mthca_next_seg) / 16;

		switch (qp->transport) {
		case RC:
			switch (wr->opcode) {
			case IB_WR_ATOMIC_CMP_AND_SWP:
			case IB_WR_ATOMIC_FETCH_AND_ADD:
				set_raddr_seg(wqe, atomic_wr(wr)->remote_addr,
					      atomic_wr(wr)->rkey);
				wqe += sizeof (struct mthca_raddr_seg);

				set_atomic_seg(wqe, atomic_wr(wr));
				wqe  += sizeof (struct mthca_atomic_seg);
				size += (sizeof (struct mthca_raddr_seg) +
					 sizeof (struct mthca_atomic_seg)) / 16;
				break;

			case IB_WR_RDMA_READ:
			case IB_WR_RDMA_WRITE:
			case IB_WR_RDMA_WRITE_WITH_IMM:
				set_raddr_seg(wqe, rdma_wr(wr)->remote_addr,
					      rdma_wr(wr)->rkey);
				wqe  += sizeof (struct mthca_raddr_seg);
				size += sizeof (struct mthca_raddr_seg) / 16;
				break;

			default:
				/* No extra segments required for sends */
				break;
			}

			break;

		case UC:
			switch (wr->opcode) {
			case IB_WR_RDMA_WRITE:
			case IB_WR_RDMA_WRITE_WITH_IMM:
				set_raddr_seg(wqe, rdma_wr(wr)->remote_addr,
					      rdma_wr(wr)->rkey);
				wqe  += sizeof (struct mthca_raddr_seg);
				size += sizeof (struct mthca_raddr_seg) / 16;
				break;

			default:
				/* No extra segments required for sends */
				break;
			}

			break;

		case UD:
			set_arbel_ud_seg(wqe, ud_wr(wr));
			wqe  += sizeof (struct mthca_arbel_ud_seg);
			size += sizeof (struct mthca_arbel_ud_seg) / 16;
			break;

		case MLX:
			err = build_mlx_header(
				dev, qp, ind, ud_wr(wr),
				wqe - sizeof(struct mthca_next_seg), wqe);
			if (err) {
				*bad_wr = wr;
				goto out;
			}
			wqe += sizeof (struct mthca_data_seg);
			size += sizeof (struct mthca_data_seg) / 16;
			break;
		}

		if (wr->num_sge > qp->sq.max_gs) {
			mthca_err(dev, "too many gathers\n");
			err = -EINVAL;
			*bad_wr = wr;
			goto out;
		}

		for (i = 0; i < wr->num_sge; ++i) {
			mthca_set_data_seg(wqe, wr->sg_list + i);
			wqe  += sizeof (struct mthca_data_seg);
			size += sizeof (struct mthca_data_seg) / 16;
		}

		/* Add one more inline data segment for ICRC */
		if (qp->transport == MLX) {
			((struct mthca_data_seg *) wqe)->byte_count =
				cpu_to_be32((1 << 31) | 4);
			((u32 *) wqe)[1] = 0;
			wqe += sizeof (struct mthca_data_seg);
			size += sizeof (struct mthca_data_seg) / 16;
		}

		qp->wrid[ind + qp->rq.max] = wr->wr_id;

		if (wr->opcode >= ARRAY_SIZE(mthca_opcode)) {
			mthca_err(dev, "opcode invalid\n");
			err = -EINVAL;
			*bad_wr = wr;
			goto out;
		}

		((struct mthca_next_seg *) prev_wqe)->nda_op =
			cpu_to_be32(((ind << qp->sq.wqe_shift) +
				     qp->send_wqe_offset) |
				    mthca_opcode[wr->opcode]);
		wmb();
		((struct mthca_next_seg *) prev_wqe)->ee_nds =
			cpu_to_be32(MTHCA_NEXT_DBD | size |
				    ((wr->send_flags & IB_SEND_FENCE) ?
				     MTHCA_NEXT_FENCE : 0));

		if (!nreq) {
			size0 = size;
			op0   = mthca_opcode[wr->opcode];
			f0    = wr->send_flags & IB_SEND_FENCE ?
				MTHCA_SEND_DOORBELL_FENCE : 0;
		}

		++ind;
		if (unlikely(ind >= qp->sq.max))
			ind -= qp->sq.max;
	}

out:
	if (likely(nreq)) {
		dbhi = (nreq << 24) | ((qp->sq.head & 0xffff) << 8) | f0 | op0;

		qp->sq.head += nreq;

		/*
		 * Make sure that descriptors are written before
		 * doorbell record.
		 */
		wmb();
		*qp->sq.db = cpu_to_be32(qp->sq.head & 0xffff);

		/*
		 * Make sure doorbell record is written before we
		 * write MMIO send doorbell.
		 */
		wmb();

		mthca_write64(dbhi, (qp->qpn << 8) | size0, dev->kar + MTHCA_SEND_DOORBELL,
			      MTHCA_GET_DOORBELL_LOCK(&dev->doorbell_lock));
	}

	spin_unlock_irqrestore(&qp->sq.lock, flags);
	return err;
}

int mthca_arbel_post_receive(struct ib_qp *ibqp, const struct ib_recv_wr *wr,
			     const struct ib_recv_wr **bad_wr)
{
	struct mthca_dev *dev = to_mdev(ibqp->device);
	struct mthca_qp *qp = to_mqp(ibqp);
	unsigned long flags;
	int err = 0;
	int nreq;
	int ind;
	int i;
	void *wqe;

	spin_lock_irqsave(&qp->rq.lock, flags);

	/* XXX check that state is OK to post receive */

	ind = qp->rq.head & (qp->rq.max - 1);

	for (nreq = 0; wr; ++nreq, wr = wr->next) {
		if (mthca_wq_overflow(&qp->rq, nreq, qp->ibqp.recv_cq)) {
			mthca_err(dev, "RQ %06x full (%u head, %u tail,"
					" %d max, %d nreq)\n", qp->qpn,
					qp->rq.head, qp->rq.tail,
					qp->rq.max, nreq);
			err = -ENOMEM;
			*bad_wr = wr;
			goto out;
		}

		wqe = get_recv_wqe(qp, ind);

		((struct mthca_next_seg *) wqe)->flags = 0;

		wqe += sizeof (struct mthca_next_seg);

		if (unlikely(wr->num_sge > qp->rq.max_gs)) {
			err = -EINVAL;
			*bad_wr = wr;
			goto out;
		}

		for (i = 0; i < wr->num_sge; ++i) {
			mthca_set_data_seg(wqe, wr->sg_list + i);
			wqe += sizeof (struct mthca_data_seg);
		}

		if (i < qp->rq.max_gs)
			mthca_set_data_seg_inval(wqe);

		qp->wrid[ind] = wr->wr_id;

		++ind;
		if (unlikely(ind >= qp->rq.max))
			ind -= qp->rq.max;
	}
out:
	if (likely(nreq)) {
		qp->rq.head += nreq;

		/*
		 * Make sure that descriptors are written before
		 * doorbell record.
		 */
		wmb();
		*qp->rq.db = cpu_to_be32(qp->rq.head & 0xffff);
	}

	spin_unlock_irqrestore(&qp->rq.lock, flags);
	return err;
}

void mthca_free_err_wqe(struct mthca_dev *dev, struct mthca_qp *qp, int is_send,
			int index, int *dbd, __be32 *new_wqe)
{
	struct mthca_next_seg *next;

	/*
	 * For SRQs, all receive WQEs generate a CQE, so we're always
	 * at the end of the doorbell chain.
	 */
	if (qp->ibqp.srq && !is_send) {
		*new_wqe = 0;
		return;
	}

	if (is_send)
		next = get_send_wqe(qp, index);
	else
		next = get_recv_wqe(qp, index);

	*dbd = !!(next->ee_nds & cpu_to_be32(MTHCA_NEXT_DBD));
	if (next->ee_nds & cpu_to_be32(0x3f))
		*new_wqe = (next->nda_op & cpu_to_be32(~0x3f)) |
			(next->ee_nds & cpu_to_be32(0x3f));
	else
		*new_wqe = 0;
}

int mthca_init_qp_table(struct mthca_dev *dev)
{
	int err;
	int i;

	spin_lock_init(&dev->qp_table.lock);

	/*
	 * We reserve 2 extra QPs per port for the special QPs.  The
	 * special QP for port 1 has to be even, so round up.
	 */
	dev->qp_table.sqp_start = (dev->limits.reserved_qps + 1) & ~1UL;
	err = mthca_alloc_init(&dev->qp_table.alloc,
			       dev->limits.num_qps,
			       (1 << 24) - 1,
			       dev->qp_table.sqp_start +
			       MTHCA_MAX_PORTS * 2);
	if (err)
		return err;

	err = mthca_array_init(&dev->qp_table.qp,
			       dev->limits.num_qps);
	if (err) {
		mthca_alloc_cleanup(&dev->qp_table.alloc);
		return err;
	}

	for (i = 0; i < 2; ++i) {
		err = mthca_CONF_SPECIAL_QP(dev, i ? IB_QPT_GSI : IB_QPT_SMI,
				    dev->qp_table.sqp_start + i * 2);
		if (err) {
			mthca_warn(dev, "CONF_SPECIAL_QP returned "
				   "%d, aborting.\n", err);
			goto err_out;
		}
	}
	return 0;

 err_out:
	for (i = 0; i < 2; ++i)
		mthca_CONF_SPECIAL_QP(dev, i, 0);

	mthca_array_cleanup(&dev->qp_table.qp, dev->limits.num_qps);
	mthca_alloc_cleanup(&dev->qp_table.alloc);

	return err;
}

void mthca_cleanup_qp_table(struct mthca_dev *dev)
{
	int i;

	for (i = 0; i < 2; ++i)
		mthca_CONF_SPECIAL_QP(dev, i, 0);

	mthca_array_cleanup(&dev->qp_table.qp, dev->limits.num_qps);
	mthca_alloc_cleanup(&dev->qp_table.alloc);
}
