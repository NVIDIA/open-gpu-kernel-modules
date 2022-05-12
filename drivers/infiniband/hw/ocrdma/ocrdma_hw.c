/* This file is part of the Emulex RoCE Device Driver for
 * RoCE (RDMA over Converged Ethernet) adapters.
 * Copyright (C) 2012-2015 Emulex. All rights reserved.
 * EMULEX and SLI are trademarks of Emulex.
 * www.emulex.com
 *
 * This software is available to you under a choice of one of two licenses.
 * You may choose to be licensed under the terms of the GNU General Public
 * License (GPL) Version 2, available from the file COPYING in the main
 * directory of this source tree, or the BSD license below:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contact Information:
 * linux-drivers@emulex.com
 *
 * Emulex
 * 3333 Susan Street
 * Costa Mesa, CA 92626
 */

#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/log2.h>
#include <linux/dma-mapping.h>
#include <linux/if_ether.h>

#include <rdma/ib_verbs.h>
#include <rdma/ib_user_verbs.h>
#include <rdma/ib_cache.h>

#include "ocrdma.h"
#include "ocrdma_hw.h"
#include "ocrdma_verbs.h"
#include "ocrdma_ah.h"

enum mbx_status {
	OCRDMA_MBX_STATUS_FAILED		= 1,
	OCRDMA_MBX_STATUS_ILLEGAL_FIELD		= 3,
	OCRDMA_MBX_STATUS_OOR			= 100,
	OCRDMA_MBX_STATUS_INVALID_PD		= 101,
	OCRDMA_MBX_STATUS_PD_INUSE		= 102,
	OCRDMA_MBX_STATUS_INVALID_CQ		= 103,
	OCRDMA_MBX_STATUS_INVALID_QP		= 104,
	OCRDMA_MBX_STATUS_INVALID_LKEY		= 105,
	OCRDMA_MBX_STATUS_ORD_EXCEEDS		= 106,
	OCRDMA_MBX_STATUS_IRD_EXCEEDS		= 107,
	OCRDMA_MBX_STATUS_SENDQ_WQE_EXCEEDS	= 108,
	OCRDMA_MBX_STATUS_RECVQ_RQE_EXCEEDS	= 109,
	OCRDMA_MBX_STATUS_SGE_SEND_EXCEEDS	= 110,
	OCRDMA_MBX_STATUS_SGE_WRITE_EXCEEDS	= 111,
	OCRDMA_MBX_STATUS_SGE_RECV_EXCEEDS	= 112,
	OCRDMA_MBX_STATUS_INVALID_STATE_CHANGE	= 113,
	OCRDMA_MBX_STATUS_MW_BOUND		= 114,
	OCRDMA_MBX_STATUS_INVALID_VA		= 115,
	OCRDMA_MBX_STATUS_INVALID_LENGTH	= 116,
	OCRDMA_MBX_STATUS_INVALID_FBO		= 117,
	OCRDMA_MBX_STATUS_INVALID_ACC_RIGHTS	= 118,
	OCRDMA_MBX_STATUS_INVALID_PBE_SIZE	= 119,
	OCRDMA_MBX_STATUS_INVALID_PBL_ENTRY	= 120,
	OCRDMA_MBX_STATUS_INVALID_PBL_SHIFT	= 121,
	OCRDMA_MBX_STATUS_INVALID_SRQ_ID	= 129,
	OCRDMA_MBX_STATUS_SRQ_ERROR		= 133,
	OCRDMA_MBX_STATUS_RQE_EXCEEDS		= 134,
	OCRDMA_MBX_STATUS_MTU_EXCEEDS		= 135,
	OCRDMA_MBX_STATUS_MAX_QP_EXCEEDS	= 136,
	OCRDMA_MBX_STATUS_SRQ_LIMIT_EXCEEDS	= 137,
	OCRDMA_MBX_STATUS_SRQ_SIZE_UNDERUNS	= 138,
	OCRDMA_MBX_STATUS_QP_BOUND		= 130,
	OCRDMA_MBX_STATUS_INVALID_CHANGE	= 139,
	OCRDMA_MBX_STATUS_ATOMIC_OPS_UNSUP	= 140,
	OCRDMA_MBX_STATUS_INVALID_RNR_NAK_TIMER	= 141,
	OCRDMA_MBX_STATUS_MW_STILL_BOUND	= 142,
	OCRDMA_MBX_STATUS_PKEY_INDEX_INVALID	= 143,
	OCRDMA_MBX_STATUS_PKEY_INDEX_EXCEEDS	= 144
};

enum additional_status {
	OCRDMA_MBX_ADDI_STATUS_INSUFFICIENT_RESOURCES = 22
};

enum cqe_status {
	OCRDMA_MBX_CQE_STATUS_INSUFFICIENT_PRIVILEDGES	= 1,
	OCRDMA_MBX_CQE_STATUS_INVALID_PARAMETER		= 2,
	OCRDMA_MBX_CQE_STATUS_INSUFFICIENT_RESOURCES	= 3,
	OCRDMA_MBX_CQE_STATUS_QUEUE_FLUSHING		= 4,
	OCRDMA_MBX_CQE_STATUS_DMA_FAILED		= 5
};

static inline void *ocrdma_get_eqe(struct ocrdma_eq *eq)
{
	return eq->q.va + (eq->q.tail * sizeof(struct ocrdma_eqe));
}

static inline void ocrdma_eq_inc_tail(struct ocrdma_eq *eq)
{
	eq->q.tail = (eq->q.tail + 1) & (OCRDMA_EQ_LEN - 1);
}

static inline void *ocrdma_get_mcqe(struct ocrdma_dev *dev)
{
	struct ocrdma_mcqe *cqe = (struct ocrdma_mcqe *)
	    (dev->mq.cq.va + (dev->mq.cq.tail * sizeof(struct ocrdma_mcqe)));

	if (!(le32_to_cpu(cqe->valid_ae_cmpl_cons) & OCRDMA_MCQE_VALID_MASK))
		return NULL;
	return cqe;
}

static inline void ocrdma_mcq_inc_tail(struct ocrdma_dev *dev)
{
	dev->mq.cq.tail = (dev->mq.cq.tail + 1) & (OCRDMA_MQ_CQ_LEN - 1);
}

static inline struct ocrdma_mqe *ocrdma_get_mqe(struct ocrdma_dev *dev)
{
	return dev->mq.sq.va + (dev->mq.sq.head * sizeof(struct ocrdma_mqe));
}

static inline void ocrdma_mq_inc_head(struct ocrdma_dev *dev)
{
	dev->mq.sq.head = (dev->mq.sq.head + 1) & (OCRDMA_MQ_LEN - 1);
}

static inline void *ocrdma_get_mqe_rsp(struct ocrdma_dev *dev)
{
	return dev->mq.sq.va + (dev->mqe_ctx.tag * sizeof(struct ocrdma_mqe));
}

enum ib_qp_state get_ibqp_state(enum ocrdma_qp_state qps)
{
	switch (qps) {
	case OCRDMA_QPS_RST:
		return IB_QPS_RESET;
	case OCRDMA_QPS_INIT:
		return IB_QPS_INIT;
	case OCRDMA_QPS_RTR:
		return IB_QPS_RTR;
	case OCRDMA_QPS_RTS:
		return IB_QPS_RTS;
	case OCRDMA_QPS_SQD:
	case OCRDMA_QPS_SQ_DRAINING:
		return IB_QPS_SQD;
	case OCRDMA_QPS_SQE:
		return IB_QPS_SQE;
	case OCRDMA_QPS_ERR:
		return IB_QPS_ERR;
	}
	return IB_QPS_ERR;
}

static enum ocrdma_qp_state get_ocrdma_qp_state(enum ib_qp_state qps)
{
	switch (qps) {
	case IB_QPS_RESET:
		return OCRDMA_QPS_RST;
	case IB_QPS_INIT:
		return OCRDMA_QPS_INIT;
	case IB_QPS_RTR:
		return OCRDMA_QPS_RTR;
	case IB_QPS_RTS:
		return OCRDMA_QPS_RTS;
	case IB_QPS_SQD:
		return OCRDMA_QPS_SQD;
	case IB_QPS_SQE:
		return OCRDMA_QPS_SQE;
	case IB_QPS_ERR:
		return OCRDMA_QPS_ERR;
	}
	return OCRDMA_QPS_ERR;
}

static int ocrdma_get_mbx_errno(u32 status)
{
	int err_num;
	u8 mbox_status = (status & OCRDMA_MBX_RSP_STATUS_MASK) >>
					OCRDMA_MBX_RSP_STATUS_SHIFT;
	u8 add_status = (status & OCRDMA_MBX_RSP_ASTATUS_MASK) >>
					OCRDMA_MBX_RSP_ASTATUS_SHIFT;

	switch (mbox_status) {
	case OCRDMA_MBX_STATUS_OOR:
	case OCRDMA_MBX_STATUS_MAX_QP_EXCEEDS:
		err_num = -EAGAIN;
		break;

	case OCRDMA_MBX_STATUS_INVALID_PD:
	case OCRDMA_MBX_STATUS_INVALID_CQ:
	case OCRDMA_MBX_STATUS_INVALID_SRQ_ID:
	case OCRDMA_MBX_STATUS_INVALID_QP:
	case OCRDMA_MBX_STATUS_INVALID_CHANGE:
	case OCRDMA_MBX_STATUS_MTU_EXCEEDS:
	case OCRDMA_MBX_STATUS_INVALID_RNR_NAK_TIMER:
	case OCRDMA_MBX_STATUS_PKEY_INDEX_INVALID:
	case OCRDMA_MBX_STATUS_PKEY_INDEX_EXCEEDS:
	case OCRDMA_MBX_STATUS_ILLEGAL_FIELD:
	case OCRDMA_MBX_STATUS_INVALID_PBL_ENTRY:
	case OCRDMA_MBX_STATUS_INVALID_LKEY:
	case OCRDMA_MBX_STATUS_INVALID_VA:
	case OCRDMA_MBX_STATUS_INVALID_LENGTH:
	case OCRDMA_MBX_STATUS_INVALID_FBO:
	case OCRDMA_MBX_STATUS_INVALID_ACC_RIGHTS:
	case OCRDMA_MBX_STATUS_INVALID_PBE_SIZE:
	case OCRDMA_MBX_STATUS_ATOMIC_OPS_UNSUP:
	case OCRDMA_MBX_STATUS_SRQ_ERROR:
	case OCRDMA_MBX_STATUS_SRQ_SIZE_UNDERUNS:
		err_num = -EINVAL;
		break;

	case OCRDMA_MBX_STATUS_PD_INUSE:
	case OCRDMA_MBX_STATUS_QP_BOUND:
	case OCRDMA_MBX_STATUS_MW_STILL_BOUND:
	case OCRDMA_MBX_STATUS_MW_BOUND:
		err_num = -EBUSY;
		break;

	case OCRDMA_MBX_STATUS_RECVQ_RQE_EXCEEDS:
	case OCRDMA_MBX_STATUS_SGE_RECV_EXCEEDS:
	case OCRDMA_MBX_STATUS_RQE_EXCEEDS:
	case OCRDMA_MBX_STATUS_SRQ_LIMIT_EXCEEDS:
	case OCRDMA_MBX_STATUS_ORD_EXCEEDS:
	case OCRDMA_MBX_STATUS_IRD_EXCEEDS:
	case OCRDMA_MBX_STATUS_SENDQ_WQE_EXCEEDS:
	case OCRDMA_MBX_STATUS_SGE_SEND_EXCEEDS:
	case OCRDMA_MBX_STATUS_SGE_WRITE_EXCEEDS:
		err_num = -ENOBUFS;
		break;

	case OCRDMA_MBX_STATUS_FAILED:
		switch (add_status) {
		case OCRDMA_MBX_ADDI_STATUS_INSUFFICIENT_RESOURCES:
			err_num = -EAGAIN;
			break;
		default:
			err_num = -EFAULT;
		}
		break;
	default:
		err_num = -EFAULT;
	}
	return err_num;
}

char *port_speed_string(struct ocrdma_dev *dev)
{
	char *str = "";
	u16 speeds_supported;

	speeds_supported = dev->phy.fixed_speeds_supported |
				dev->phy.auto_speeds_supported;
	if (speeds_supported & OCRDMA_PHY_SPEED_40GBPS)
		str = "40Gbps ";
	else if (speeds_supported & OCRDMA_PHY_SPEED_10GBPS)
		str = "10Gbps ";
	else if (speeds_supported & OCRDMA_PHY_SPEED_1GBPS)
		str = "1Gbps ";

	return str;
}

static int ocrdma_get_mbx_cqe_errno(u16 cqe_status)
{
	int err_num = -EINVAL;

	switch (cqe_status) {
	case OCRDMA_MBX_CQE_STATUS_INSUFFICIENT_PRIVILEDGES:
		err_num = -EPERM;
		break;
	case OCRDMA_MBX_CQE_STATUS_INVALID_PARAMETER:
		err_num = -EINVAL;
		break;
	case OCRDMA_MBX_CQE_STATUS_INSUFFICIENT_RESOURCES:
	case OCRDMA_MBX_CQE_STATUS_QUEUE_FLUSHING:
		err_num = -EINVAL;
		break;
	case OCRDMA_MBX_CQE_STATUS_DMA_FAILED:
	default:
		err_num = -EINVAL;
		break;
	}
	return err_num;
}

void ocrdma_ring_cq_db(struct ocrdma_dev *dev, u16 cq_id, bool armed,
		       bool solicited, u16 cqe_popped)
{
	u32 val = cq_id & OCRDMA_DB_CQ_RING_ID_MASK;

	val |= ((cq_id & OCRDMA_DB_CQ_RING_ID_EXT_MASK) <<
	     OCRDMA_DB_CQ_RING_ID_EXT_MASK_SHIFT);

	if (armed)
		val |= (1 << OCRDMA_DB_CQ_REARM_SHIFT);
	if (solicited)
		val |= (1 << OCRDMA_DB_CQ_SOLICIT_SHIFT);
	val |= (cqe_popped << OCRDMA_DB_CQ_NUM_POPPED_SHIFT);
	iowrite32(val, dev->nic_info.db + OCRDMA_DB_CQ_OFFSET);
}

static void ocrdma_ring_mq_db(struct ocrdma_dev *dev)
{
	u32 val = 0;

	val |= dev->mq.sq.id & OCRDMA_MQ_ID_MASK;
	val |= 1 << OCRDMA_MQ_NUM_MQE_SHIFT;
	iowrite32(val, dev->nic_info.db + OCRDMA_DB_MQ_OFFSET);
}

static void ocrdma_ring_eq_db(struct ocrdma_dev *dev, u16 eq_id,
			      bool arm, bool clear_int, u16 num_eqe)
{
	u32 val = 0;

	val |= eq_id & OCRDMA_EQ_ID_MASK;
	val |= ((eq_id & OCRDMA_EQ_ID_EXT_MASK) << OCRDMA_EQ_ID_EXT_MASK_SHIFT);
	if (arm)
		val |= (1 << OCRDMA_REARM_SHIFT);
	if (clear_int)
		val |= (1 << OCRDMA_EQ_CLR_SHIFT);
	val |= (1 << OCRDMA_EQ_TYPE_SHIFT);
	val |= (num_eqe << OCRDMA_NUM_EQE_SHIFT);
	iowrite32(val, dev->nic_info.db + OCRDMA_DB_EQ_OFFSET);
}

static void ocrdma_init_mch(struct ocrdma_mbx_hdr *cmd_hdr,
			    u8 opcode, u8 subsys, u32 cmd_len)
{
	cmd_hdr->subsys_op = (opcode | (subsys << OCRDMA_MCH_SUBSYS_SHIFT));
	cmd_hdr->timeout = 20; /* seconds */
	cmd_hdr->cmd_len = cmd_len - sizeof(struct ocrdma_mbx_hdr);
}

static void *ocrdma_init_emb_mqe(u8 opcode, u32 cmd_len)
{
	struct ocrdma_mqe *mqe;

	mqe = kzalloc(sizeof(struct ocrdma_mqe), GFP_KERNEL);
	if (!mqe)
		return NULL;
	mqe->hdr.spcl_sge_cnt_emb |=
		(OCRDMA_MQE_EMBEDDED << OCRDMA_MQE_HDR_EMB_SHIFT) &
					OCRDMA_MQE_HDR_EMB_MASK;
	mqe->hdr.pyld_len = cmd_len - sizeof(struct ocrdma_mqe_hdr);

	ocrdma_init_mch(&mqe->u.emb_req.mch, opcode, OCRDMA_SUBSYS_ROCE,
			mqe->hdr.pyld_len);
	return mqe;
}

static void ocrdma_free_q(struct ocrdma_dev *dev, struct ocrdma_queue_info *q)
{
	dma_free_coherent(&dev->nic_info.pdev->dev, q->size, q->va, q->dma);
}

static int ocrdma_alloc_q(struct ocrdma_dev *dev,
			  struct ocrdma_queue_info *q, u16 len, u16 entry_size)
{
	memset(q, 0, sizeof(*q));
	q->len = len;
	q->entry_size = entry_size;
	q->size = len * entry_size;
	q->va = dma_alloc_coherent(&dev->nic_info.pdev->dev, q->size, &q->dma,
				   GFP_KERNEL);
	if (!q->va)
		return -ENOMEM;
	return 0;
}

static void ocrdma_build_q_pages(struct ocrdma_pa *q_pa, int cnt,
					dma_addr_t host_pa, int hw_page_size)
{
	int i;

	for (i = 0; i < cnt; i++) {
		q_pa[i].lo = (u32) (host_pa & 0xffffffff);
		q_pa[i].hi = (u32) upper_32_bits(host_pa);
		host_pa += hw_page_size;
	}
}

static int ocrdma_mbx_delete_q(struct ocrdma_dev *dev,
			       struct ocrdma_queue_info *q, int queue_type)
{
	u8 opcode = 0;
	int status;
	struct ocrdma_delete_q_req *cmd = dev->mbx_cmd;

	switch (queue_type) {
	case QTYPE_MCCQ:
		opcode = OCRDMA_CMD_DELETE_MQ;
		break;
	case QTYPE_CQ:
		opcode = OCRDMA_CMD_DELETE_CQ;
		break;
	case QTYPE_EQ:
		opcode = OCRDMA_CMD_DELETE_EQ;
		break;
	default:
		BUG();
	}
	memset(cmd, 0, sizeof(*cmd));
	ocrdma_init_mch(&cmd->req, opcode, OCRDMA_SUBSYS_COMMON, sizeof(*cmd));
	cmd->id = q->id;

	status = be_roce_mcc_cmd(dev->nic_info.netdev,
				 cmd, sizeof(*cmd), NULL, NULL);
	if (!status)
		q->created = false;
	return status;
}

static int ocrdma_mbx_create_eq(struct ocrdma_dev *dev, struct ocrdma_eq *eq)
{
	int status;
	struct ocrdma_create_eq_req *cmd = dev->mbx_cmd;
	struct ocrdma_create_eq_rsp *rsp = dev->mbx_cmd;

	memset(cmd, 0, sizeof(*cmd));
	ocrdma_init_mch(&cmd->req, OCRDMA_CMD_CREATE_EQ, OCRDMA_SUBSYS_COMMON,
			sizeof(*cmd));

	cmd->req.rsvd_version = 2;
	cmd->num_pages = 4;
	cmd->valid = OCRDMA_CREATE_EQ_VALID;
	cmd->cnt = 4 << OCRDMA_CREATE_EQ_CNT_SHIFT;

	ocrdma_build_q_pages(&cmd->pa[0], cmd->num_pages, eq->q.dma,
			     PAGE_SIZE_4K);
	status = be_roce_mcc_cmd(dev->nic_info.netdev, cmd, sizeof(*cmd), NULL,
				 NULL);
	if (!status) {
		eq->q.id = rsp->vector_eqid & 0xffff;
		eq->vector = (rsp->vector_eqid >> 16) & 0xffff;
		eq->q.created = true;
	}
	return status;
}

static int ocrdma_create_eq(struct ocrdma_dev *dev,
			    struct ocrdma_eq *eq, u16 q_len)
{
	int status;

	status = ocrdma_alloc_q(dev, &eq->q, OCRDMA_EQ_LEN,
				sizeof(struct ocrdma_eqe));
	if (status)
		return status;

	status = ocrdma_mbx_create_eq(dev, eq);
	if (status)
		goto mbx_err;
	eq->dev = dev;
	ocrdma_ring_eq_db(dev, eq->q.id, true, true, 0);

	return 0;
mbx_err:
	ocrdma_free_q(dev, &eq->q);
	return status;
}

int ocrdma_get_irq(struct ocrdma_dev *dev, struct ocrdma_eq *eq)
{
	int irq;

	if (dev->nic_info.intr_mode == BE_INTERRUPT_MODE_INTX)
		irq = dev->nic_info.pdev->irq;
	else
		irq = dev->nic_info.msix.vector_list[eq->vector];
	return irq;
}

static void _ocrdma_destroy_eq(struct ocrdma_dev *dev, struct ocrdma_eq *eq)
{
	if (eq->q.created) {
		ocrdma_mbx_delete_q(dev, &eq->q, QTYPE_EQ);
		ocrdma_free_q(dev, &eq->q);
	}
}

static void ocrdma_destroy_eq(struct ocrdma_dev *dev, struct ocrdma_eq *eq)
{
	int irq;

	/* disarm EQ so that interrupts are not generated
	 * during freeing and EQ delete is in progress.
	 */
	ocrdma_ring_eq_db(dev, eq->q.id, false, false, 0);

	irq = ocrdma_get_irq(dev, eq);
	free_irq(irq, eq);
	_ocrdma_destroy_eq(dev, eq);
}

static void ocrdma_destroy_eqs(struct ocrdma_dev *dev)
{
	int i;

	for (i = 0; i < dev->eq_cnt; i++)
		ocrdma_destroy_eq(dev, &dev->eq_tbl[i]);
}

static int ocrdma_mbx_mq_cq_create(struct ocrdma_dev *dev,
				   struct ocrdma_queue_info *cq,
				   struct ocrdma_queue_info *eq)
{
	struct ocrdma_create_cq_cmd *cmd = dev->mbx_cmd;
	struct ocrdma_create_cq_cmd_rsp *rsp = dev->mbx_cmd;
	int status;

	memset(cmd, 0, sizeof(*cmd));
	ocrdma_init_mch(&cmd->req, OCRDMA_CMD_CREATE_CQ,
			OCRDMA_SUBSYS_COMMON, sizeof(*cmd));

	cmd->req.rsvd_version = OCRDMA_CREATE_CQ_VER2;
	cmd->pgsz_pgcnt = (cq->size / OCRDMA_MIN_Q_PAGE_SIZE) <<
		OCRDMA_CREATE_CQ_PAGE_SIZE_SHIFT;
	cmd->pgsz_pgcnt |= PAGES_4K_SPANNED(cq->va, cq->size);

	cmd->ev_cnt_flags = OCRDMA_CREATE_CQ_DEF_FLAGS;
	cmd->eqn = eq->id;
	cmd->pdid_cqecnt = cq->size / sizeof(struct ocrdma_mcqe);

	ocrdma_build_q_pages(&cmd->pa[0], cq->size / OCRDMA_MIN_Q_PAGE_SIZE,
			     cq->dma, PAGE_SIZE_4K);
	status = be_roce_mcc_cmd(dev->nic_info.netdev,
				 cmd, sizeof(*cmd), NULL, NULL);
	if (!status) {
		cq->id = (u16) (rsp->cq_id & OCRDMA_CREATE_CQ_RSP_CQ_ID_MASK);
		cq->created = true;
	}
	return status;
}

static u32 ocrdma_encoded_q_len(int q_len)
{
	u32 len_encoded = fls(q_len);	/* log2(len) + 1 */

	if (len_encoded == 16)
		len_encoded = 0;
	return len_encoded;
}

static int ocrdma_mbx_create_mq(struct ocrdma_dev *dev,
				struct ocrdma_queue_info *mq,
				struct ocrdma_queue_info *cq)
{
	int num_pages, status;
	struct ocrdma_create_mq_req *cmd = dev->mbx_cmd;
	struct ocrdma_create_mq_rsp *rsp = dev->mbx_cmd;
	struct ocrdma_pa *pa;

	memset(cmd, 0, sizeof(*cmd));
	num_pages = PAGES_4K_SPANNED(mq->va, mq->size);

	ocrdma_init_mch(&cmd->req, OCRDMA_CMD_CREATE_MQ_EXT,
			OCRDMA_SUBSYS_COMMON, sizeof(*cmd));
	cmd->req.rsvd_version = 1;
	cmd->cqid_pages = num_pages;
	cmd->cqid_pages |= (cq->id << OCRDMA_CREATE_MQ_CQ_ID_SHIFT);
	cmd->async_cqid_valid = OCRDMA_CREATE_MQ_ASYNC_CQ_VALID;

	cmd->async_event_bitmap = BIT(OCRDMA_ASYNC_GRP5_EVE_CODE);
	cmd->async_event_bitmap |= BIT(OCRDMA_ASYNC_RDMA_EVE_CODE);
	/* Request link events on this  MQ. */
	cmd->async_event_bitmap |= BIT(OCRDMA_ASYNC_LINK_EVE_CODE);

	cmd->async_cqid_ringsize = cq->id;
	cmd->async_cqid_ringsize |= (ocrdma_encoded_q_len(mq->len) <<
				OCRDMA_CREATE_MQ_RING_SIZE_SHIFT);
	cmd->valid = OCRDMA_CREATE_MQ_VALID;
	pa = &cmd->pa[0];

	ocrdma_build_q_pages(pa, num_pages, mq->dma, PAGE_SIZE_4K);
	status = be_roce_mcc_cmd(dev->nic_info.netdev,
				 cmd, sizeof(*cmd), NULL, NULL);
	if (!status) {
		mq->id = rsp->id;
		mq->created = true;
	}
	return status;
}

static int ocrdma_create_mq(struct ocrdma_dev *dev)
{
	int status;

	/* Alloc completion queue for Mailbox queue */
	status = ocrdma_alloc_q(dev, &dev->mq.cq, OCRDMA_MQ_CQ_LEN,
				sizeof(struct ocrdma_mcqe));
	if (status)
		goto alloc_err;

	dev->eq_tbl[0].cq_cnt++;
	status = ocrdma_mbx_mq_cq_create(dev, &dev->mq.cq, &dev->eq_tbl[0].q);
	if (status)
		goto mbx_cq_free;

	memset(&dev->mqe_ctx, 0, sizeof(dev->mqe_ctx));
	init_waitqueue_head(&dev->mqe_ctx.cmd_wait);
	mutex_init(&dev->mqe_ctx.lock);

	/* Alloc Mailbox queue */
	status = ocrdma_alloc_q(dev, &dev->mq.sq, OCRDMA_MQ_LEN,
				sizeof(struct ocrdma_mqe));
	if (status)
		goto mbx_cq_destroy;
	status = ocrdma_mbx_create_mq(dev, &dev->mq.sq, &dev->mq.cq);
	if (status)
		goto mbx_q_free;
	ocrdma_ring_cq_db(dev, dev->mq.cq.id, true, false, 0);
	return 0;

mbx_q_free:
	ocrdma_free_q(dev, &dev->mq.sq);
mbx_cq_destroy:
	ocrdma_mbx_delete_q(dev, &dev->mq.cq, QTYPE_CQ);
mbx_cq_free:
	ocrdma_free_q(dev, &dev->mq.cq);
alloc_err:
	return status;
}

static void ocrdma_destroy_mq(struct ocrdma_dev *dev)
{
	struct ocrdma_queue_info *mbxq, *cq;

	/* mqe_ctx lock synchronizes with any other pending cmds. */
	mutex_lock(&dev->mqe_ctx.lock);
	mbxq = &dev->mq.sq;
	if (mbxq->created) {
		ocrdma_mbx_delete_q(dev, mbxq, QTYPE_MCCQ);
		ocrdma_free_q(dev, mbxq);
	}
	mutex_unlock(&dev->mqe_ctx.lock);

	cq = &dev->mq.cq;
	if (cq->created) {
		ocrdma_mbx_delete_q(dev, cq, QTYPE_CQ);
		ocrdma_free_q(dev, cq);
	}
}

static void ocrdma_process_qpcat_error(struct ocrdma_dev *dev,
				       struct ocrdma_qp *qp)
{
	enum ib_qp_state new_ib_qps = IB_QPS_ERR;
	enum ib_qp_state old_ib_qps;

	if (qp == NULL)
		BUG();
	ocrdma_qp_state_change(qp, new_ib_qps, &old_ib_qps);
}

static void ocrdma_dispatch_ibevent(struct ocrdma_dev *dev,
				    struct ocrdma_ae_mcqe *cqe)
{
	struct ocrdma_qp *qp = NULL;
	struct ocrdma_cq *cq = NULL;
	struct ib_event ib_evt;
	int cq_event = 0;
	int qp_event = 1;
	int srq_event = 0;
	int dev_event = 0;
	int type = (cqe->valid_ae_event & OCRDMA_AE_MCQE_EVENT_TYPE_MASK) >>
	    OCRDMA_AE_MCQE_EVENT_TYPE_SHIFT;
	u16 qpid = cqe->qpvalid_qpid & OCRDMA_AE_MCQE_QPID_MASK;
	u16 cqid = cqe->cqvalid_cqid & OCRDMA_AE_MCQE_CQID_MASK;

	/*
	 * Some FW version returns wrong qp or cq ids in CQEs.
	 * Checking whether the IDs are valid
	 */

	if (cqe->qpvalid_qpid & OCRDMA_AE_MCQE_QPVALID) {
		if (qpid < dev->attr.max_qp)
			qp = dev->qp_tbl[qpid];
		if (qp == NULL) {
			pr_err("ocrdma%d:Async event - qpid %u is not valid\n",
			       dev->id, qpid);
			return;
		}
	}

	if (cqe->cqvalid_cqid & OCRDMA_AE_MCQE_CQVALID) {
		if (cqid < dev->attr.max_cq)
			cq = dev->cq_tbl[cqid];
		if (cq == NULL) {
			pr_err("ocrdma%d:Async event - cqid %u is not valid\n",
			       dev->id, cqid);
			return;
		}
	}

	memset(&ib_evt, 0, sizeof(ib_evt));

	ib_evt.device = &dev->ibdev;

	switch (type) {
	case OCRDMA_CQ_ERROR:
		ib_evt.element.cq = &cq->ibcq;
		ib_evt.event = IB_EVENT_CQ_ERR;
		cq_event = 1;
		qp_event = 0;
		break;
	case OCRDMA_CQ_OVERRUN_ERROR:
		ib_evt.element.cq = &cq->ibcq;
		ib_evt.event = IB_EVENT_CQ_ERR;
		cq_event = 1;
		qp_event = 0;
		break;
	case OCRDMA_CQ_QPCAT_ERROR:
		ib_evt.element.qp = &qp->ibqp;
		ib_evt.event = IB_EVENT_QP_FATAL;
		ocrdma_process_qpcat_error(dev, qp);
		break;
	case OCRDMA_QP_ACCESS_ERROR:
		ib_evt.element.qp = &qp->ibqp;
		ib_evt.event = IB_EVENT_QP_ACCESS_ERR;
		break;
	case OCRDMA_QP_COMM_EST_EVENT:
		ib_evt.element.qp = &qp->ibqp;
		ib_evt.event = IB_EVENT_COMM_EST;
		break;
	case OCRDMA_SQ_DRAINED_EVENT:
		ib_evt.element.qp = &qp->ibqp;
		ib_evt.event = IB_EVENT_SQ_DRAINED;
		break;
	case OCRDMA_DEVICE_FATAL_EVENT:
		ib_evt.element.port_num = 1;
		ib_evt.event = IB_EVENT_DEVICE_FATAL;
		qp_event = 0;
		dev_event = 1;
		break;
	case OCRDMA_SRQCAT_ERROR:
		ib_evt.element.srq = &qp->srq->ibsrq;
		ib_evt.event = IB_EVENT_SRQ_ERR;
		srq_event = 1;
		qp_event = 0;
		break;
	case OCRDMA_SRQ_LIMIT_EVENT:
		ib_evt.element.srq = &qp->srq->ibsrq;
		ib_evt.event = IB_EVENT_SRQ_LIMIT_REACHED;
		srq_event = 1;
		qp_event = 0;
		break;
	case OCRDMA_QP_LAST_WQE_EVENT:
		ib_evt.element.qp = &qp->ibqp;
		ib_evt.event = IB_EVENT_QP_LAST_WQE_REACHED;
		break;
	default:
		cq_event = 0;
		qp_event = 0;
		srq_event = 0;
		dev_event = 0;
		pr_err("%s() unknown type=0x%x\n", __func__, type);
		break;
	}

	if (type < OCRDMA_MAX_ASYNC_ERRORS)
		atomic_inc(&dev->async_err_stats[type]);

	if (qp_event) {
		if (qp->ibqp.event_handler)
			qp->ibqp.event_handler(&ib_evt, qp->ibqp.qp_context);
	} else if (cq_event) {
		if (cq->ibcq.event_handler)
			cq->ibcq.event_handler(&ib_evt, cq->ibcq.cq_context);
	} else if (srq_event) {
		if (qp->srq->ibsrq.event_handler)
			qp->srq->ibsrq.event_handler(&ib_evt,
						     qp->srq->ibsrq.
						     srq_context);
	} else if (dev_event) {
		dev_err(&dev->ibdev.dev, "Fatal event received\n");
		ib_dispatch_event(&ib_evt);
	}

}

static void ocrdma_process_grp5_aync(struct ocrdma_dev *dev,
					struct ocrdma_ae_mcqe *cqe)
{
	struct ocrdma_ae_pvid_mcqe *evt;
	int type = (cqe->valid_ae_event & OCRDMA_AE_MCQE_EVENT_TYPE_MASK) >>
			OCRDMA_AE_MCQE_EVENT_TYPE_SHIFT;

	switch (type) {
	case OCRDMA_ASYNC_EVENT_PVID_STATE:
		evt = (struct ocrdma_ae_pvid_mcqe *)cqe;
		if ((evt->tag_enabled & OCRDMA_AE_PVID_MCQE_ENABLED_MASK) >>
			OCRDMA_AE_PVID_MCQE_ENABLED_SHIFT)
			dev->pvid = ((evt->tag_enabled &
					OCRDMA_AE_PVID_MCQE_TAG_MASK) >>
					OCRDMA_AE_PVID_MCQE_TAG_SHIFT);
		break;

	case OCRDMA_ASYNC_EVENT_COS_VALUE:
		atomic_set(&dev->update_sl, 1);
		break;
	default:
		/* Not interested evts. */
		break;
	}
}

static void ocrdma_process_link_state(struct ocrdma_dev *dev,
				      struct ocrdma_ae_mcqe *cqe)
{
	struct ocrdma_ae_lnkst_mcqe *evt;
	u8 lstate;

	evt = (struct ocrdma_ae_lnkst_mcqe *)cqe;
	lstate = ocrdma_get_ae_link_state(evt->speed_state_ptn);

	if (!(lstate & OCRDMA_AE_LSC_LLINK_MASK))
		return;

	if (dev->flags & OCRDMA_FLAGS_LINK_STATUS_INIT)
		ocrdma_update_link_state(dev, (lstate & OCRDMA_LINK_ST_MASK));
}

static void ocrdma_process_acqe(struct ocrdma_dev *dev, void *ae_cqe)
{
	/* async CQE processing */
	struct ocrdma_ae_mcqe *cqe = ae_cqe;
	u32 evt_code = (cqe->valid_ae_event & OCRDMA_AE_MCQE_EVENT_CODE_MASK) >>
			OCRDMA_AE_MCQE_EVENT_CODE_SHIFT;
	switch (evt_code) {
	case OCRDMA_ASYNC_LINK_EVE_CODE:
		ocrdma_process_link_state(dev, cqe);
		break;
	case OCRDMA_ASYNC_RDMA_EVE_CODE:
		ocrdma_dispatch_ibevent(dev, cqe);
		break;
	case OCRDMA_ASYNC_GRP5_EVE_CODE:
		ocrdma_process_grp5_aync(dev, cqe);
		break;
	default:
		pr_err("%s(%d) invalid evt code=0x%x\n", __func__,
		       dev->id, evt_code);
	}
}

static void ocrdma_process_mcqe(struct ocrdma_dev *dev, struct ocrdma_mcqe *cqe)
{
	if (dev->mqe_ctx.tag == cqe->tag_lo && dev->mqe_ctx.cmd_done == false) {
		dev->mqe_ctx.cqe_status = (cqe->status &
		     OCRDMA_MCQE_STATUS_MASK) >> OCRDMA_MCQE_STATUS_SHIFT;
		dev->mqe_ctx.ext_status =
		    (cqe->status & OCRDMA_MCQE_ESTATUS_MASK)
		    >> OCRDMA_MCQE_ESTATUS_SHIFT;
		dev->mqe_ctx.cmd_done = true;
		wake_up(&dev->mqe_ctx.cmd_wait);
	} else
		pr_err("%s() cqe for invalid tag0x%x.expected=0x%x\n",
		       __func__, cqe->tag_lo, dev->mqe_ctx.tag);
}

static int ocrdma_mq_cq_handler(struct ocrdma_dev *dev, u16 cq_id)
{
	u16 cqe_popped = 0;
	struct ocrdma_mcqe *cqe;

	while (1) {
		cqe = ocrdma_get_mcqe(dev);
		if (cqe == NULL)
			break;
		ocrdma_le32_to_cpu(cqe, sizeof(*cqe));
		cqe_popped += 1;
		if (cqe->valid_ae_cmpl_cons & OCRDMA_MCQE_AE_MASK)
			ocrdma_process_acqe(dev, cqe);
		else if (cqe->valid_ae_cmpl_cons & OCRDMA_MCQE_CMPL_MASK)
			ocrdma_process_mcqe(dev, cqe);
		memset(cqe, 0, sizeof(struct ocrdma_mcqe));
		ocrdma_mcq_inc_tail(dev);
	}
	ocrdma_ring_cq_db(dev, dev->mq.cq.id, true, false, cqe_popped);
	return 0;
}

static struct ocrdma_cq *_ocrdma_qp_buddy_cq_handler(struct ocrdma_dev *dev,
				struct ocrdma_cq *cq, bool sq)
{
	struct ocrdma_qp *qp;
	struct list_head *cur;
	struct ocrdma_cq *bcq = NULL;
	struct list_head *head = sq?(&cq->sq_head):(&cq->rq_head);

	list_for_each(cur, head) {
		if (sq)
			qp = list_entry(cur, struct ocrdma_qp, sq_entry);
		else
			qp = list_entry(cur, struct ocrdma_qp, rq_entry);

		if (qp->srq)
			continue;
		/* if wq and rq share the same cq, than comp_handler
		 * is already invoked.
		 */
		if (qp->sq_cq == qp->rq_cq)
			continue;
		/* if completion came on sq, rq's cq is buddy cq.
		 * if completion came on rq, sq's cq is buddy cq.
		 */
		if (qp->sq_cq == cq)
			bcq = qp->rq_cq;
		else
			bcq = qp->sq_cq;
		return bcq;
	}
	return NULL;
}

static void ocrdma_qp_buddy_cq_handler(struct ocrdma_dev *dev,
				       struct ocrdma_cq *cq)
{
	unsigned long flags;
	struct ocrdma_cq *bcq = NULL;

	/* Go through list of QPs in error state which are using this CQ
	 * and invoke its callback handler to trigger CQE processing for
	 * error/flushed CQE. It is rare to find more than few entries in
	 * this list as most consumers stops after getting error CQE.
	 * List is traversed only once when a matching buddy cq found for a QP.
	 */
	spin_lock_irqsave(&dev->flush_q_lock, flags);
	/* Check if buddy CQ is present.
	 * true - Check for  SQ CQ
	 * false - Check for RQ CQ
	 */
	bcq = _ocrdma_qp_buddy_cq_handler(dev, cq, true);
	if (bcq == NULL)
		bcq = _ocrdma_qp_buddy_cq_handler(dev, cq, false);
	spin_unlock_irqrestore(&dev->flush_q_lock, flags);

	/* if there is valid buddy cq, look for its completion handler */
	if (bcq && bcq->ibcq.comp_handler) {
		spin_lock_irqsave(&bcq->comp_handler_lock, flags);
		(*bcq->ibcq.comp_handler) (&bcq->ibcq, bcq->ibcq.cq_context);
		spin_unlock_irqrestore(&bcq->comp_handler_lock, flags);
	}
}

static void ocrdma_qp_cq_handler(struct ocrdma_dev *dev, u16 cq_idx)
{
	unsigned long flags;
	struct ocrdma_cq *cq;

	if (cq_idx >= OCRDMA_MAX_CQ)
		BUG();

	cq = dev->cq_tbl[cq_idx];
	if (cq == NULL)
		return;

	if (cq->ibcq.comp_handler) {
		spin_lock_irqsave(&cq->comp_handler_lock, flags);
		(*cq->ibcq.comp_handler) (&cq->ibcq, cq->ibcq.cq_context);
		spin_unlock_irqrestore(&cq->comp_handler_lock, flags);
	}
	ocrdma_qp_buddy_cq_handler(dev, cq);
}

static void ocrdma_cq_handler(struct ocrdma_dev *dev, u16 cq_id)
{
	/* process the MQ-CQE. */
	if (cq_id == dev->mq.cq.id)
		ocrdma_mq_cq_handler(dev, cq_id);
	else
		ocrdma_qp_cq_handler(dev, cq_id);
}

static irqreturn_t ocrdma_irq_handler(int irq, void *handle)
{
	struct ocrdma_eq *eq = handle;
	struct ocrdma_dev *dev = eq->dev;
	struct ocrdma_eqe eqe;
	struct ocrdma_eqe *ptr;
	u16 cq_id;
	u8 mcode;
	int budget = eq->cq_cnt;

	do {
		ptr = ocrdma_get_eqe(eq);
		eqe = *ptr;
		ocrdma_le32_to_cpu(&eqe, sizeof(eqe));
		mcode = (eqe.id_valid & OCRDMA_EQE_MAJOR_CODE_MASK)
				>> OCRDMA_EQE_MAJOR_CODE_SHIFT;
		if (mcode == OCRDMA_MAJOR_CODE_SENTINAL)
			pr_err("EQ full on eqid = 0x%x, eqe = 0x%x\n",
			       eq->q.id, eqe.id_valid);
		if ((eqe.id_valid & OCRDMA_EQE_VALID_MASK) == 0)
			break;

		ptr->id_valid = 0;
		/* ring eq doorbell as soon as its consumed. */
		ocrdma_ring_eq_db(dev, eq->q.id, false, true, 1);
		/* check whether its CQE or not. */
		if ((eqe.id_valid & OCRDMA_EQE_FOR_CQE_MASK) == 0) {
			cq_id = eqe.id_valid >> OCRDMA_EQE_RESOURCE_ID_SHIFT;
			ocrdma_cq_handler(dev, cq_id);
		}
		ocrdma_eq_inc_tail(eq);

		/* There can be a stale EQE after the last bound CQ is
		 * destroyed. EQE valid and budget == 0 implies this.
		 */
		if (budget)
			budget--;

	} while (budget);

	eq->aic_obj.eq_intr_cnt++;
	ocrdma_ring_eq_db(dev, eq->q.id, true, true, 0);
	return IRQ_HANDLED;
}

static void ocrdma_post_mqe(struct ocrdma_dev *dev, struct ocrdma_mqe *cmd)
{
	struct ocrdma_mqe *mqe;

	dev->mqe_ctx.tag = dev->mq.sq.head;
	dev->mqe_ctx.cmd_done = false;
	mqe = ocrdma_get_mqe(dev);
	cmd->hdr.tag_lo = dev->mq.sq.head;
	ocrdma_copy_cpu_to_le32(mqe, cmd, sizeof(*mqe));
	/* make sure descriptor is written before ringing doorbell */
	wmb();
	ocrdma_mq_inc_head(dev);
	ocrdma_ring_mq_db(dev);
}

static int ocrdma_wait_mqe_cmpl(struct ocrdma_dev *dev)
{
	long status;
	/* 30 sec timeout */
	status = wait_event_timeout(dev->mqe_ctx.cmd_wait,
				    (dev->mqe_ctx.cmd_done != false),
				    msecs_to_jiffies(30000));
	if (status)
		return 0;
	else {
		dev->mqe_ctx.fw_error_state = true;
		pr_err("%s(%d) mailbox timeout: fw not responding\n",
		       __func__, dev->id);
		return -1;
	}
}

/* issue a mailbox command on the MQ */
static int ocrdma_mbx_cmd(struct ocrdma_dev *dev, struct ocrdma_mqe *mqe)
{
	int status = 0;
	u16 cqe_status, ext_status;
	struct ocrdma_mqe *rsp_mqe;
	struct ocrdma_mbx_rsp *rsp = NULL;

	mutex_lock(&dev->mqe_ctx.lock);
	if (dev->mqe_ctx.fw_error_state)
		goto mbx_err;
	ocrdma_post_mqe(dev, mqe);
	status = ocrdma_wait_mqe_cmpl(dev);
	if (status)
		goto mbx_err;
	cqe_status = dev->mqe_ctx.cqe_status;
	ext_status = dev->mqe_ctx.ext_status;
	rsp_mqe = ocrdma_get_mqe_rsp(dev);
	ocrdma_copy_le32_to_cpu(mqe, rsp_mqe, (sizeof(*mqe)));
	if ((mqe->hdr.spcl_sge_cnt_emb & OCRDMA_MQE_HDR_EMB_MASK) >>
				OCRDMA_MQE_HDR_EMB_SHIFT)
		rsp = &mqe->u.rsp;

	if (cqe_status || ext_status) {
		pr_err("%s() cqe_status=0x%x, ext_status=0x%x,\n",
		       __func__, cqe_status, ext_status);
		if (rsp) {
			/* This is for embedded cmds. */
			pr_err("opcode=0x%x, subsystem=0x%x\n",
			       (rsp->subsys_op & OCRDMA_MBX_RSP_OPCODE_MASK) >>
				OCRDMA_MBX_RSP_OPCODE_SHIFT,
				(rsp->subsys_op & OCRDMA_MBX_RSP_SUBSYS_MASK) >>
				OCRDMA_MBX_RSP_SUBSYS_SHIFT);
		}
		status = ocrdma_get_mbx_cqe_errno(cqe_status);
		goto mbx_err;
	}
	/* For non embedded, rsp errors are handled in ocrdma_nonemb_mbx_cmd */
	if (rsp && (mqe->u.rsp.status & OCRDMA_MBX_RSP_STATUS_MASK))
		status = ocrdma_get_mbx_errno(mqe->u.rsp.status);
mbx_err:
	mutex_unlock(&dev->mqe_ctx.lock);
	return status;
}

static int ocrdma_nonemb_mbx_cmd(struct ocrdma_dev *dev, struct ocrdma_mqe *mqe,
				 void *payload_va)
{
	int status;
	struct ocrdma_mbx_rsp *rsp = payload_va;

	if ((mqe->hdr.spcl_sge_cnt_emb & OCRDMA_MQE_HDR_EMB_MASK) >>
				OCRDMA_MQE_HDR_EMB_SHIFT)
		BUG();

	status = ocrdma_mbx_cmd(dev, mqe);
	if (!status)
		/* For non embedded, only CQE failures are handled in
		 * ocrdma_mbx_cmd. We need to check for RSP errors.
		 */
		if (rsp->status & OCRDMA_MBX_RSP_STATUS_MASK)
			status = ocrdma_get_mbx_errno(rsp->status);

	if (status)
		pr_err("opcode=0x%x, subsystem=0x%x\n",
		       (rsp->subsys_op & OCRDMA_MBX_RSP_OPCODE_MASK) >>
			OCRDMA_MBX_RSP_OPCODE_SHIFT,
			(rsp->subsys_op & OCRDMA_MBX_RSP_SUBSYS_MASK) >>
			OCRDMA_MBX_RSP_SUBSYS_SHIFT);
	return status;
}

static void ocrdma_get_attr(struct ocrdma_dev *dev,
			      struct ocrdma_dev_attr *attr,
			      struct ocrdma_mbx_query_config *rsp)
{
	attr->max_pd =
	    (rsp->max_pd_ca_ack_delay & OCRDMA_MBX_QUERY_CFG_MAX_PD_MASK) >>
	    OCRDMA_MBX_QUERY_CFG_MAX_PD_SHIFT;
	attr->udp_encap = (rsp->max_pd_ca_ack_delay &
			   OCRDMA_MBX_QUERY_CFG_L3_TYPE_MASK) >>
			   OCRDMA_MBX_QUERY_CFG_L3_TYPE_SHIFT;
	attr->max_dpp_pds =
	   (rsp->max_dpp_pds_credits & OCRDMA_MBX_QUERY_CFG_MAX_DPP_PDS_MASK) >>
	    OCRDMA_MBX_QUERY_CFG_MAX_DPP_PDS_OFFSET;
	attr->max_qp =
	    (rsp->qp_srq_cq_ird_ord & OCRDMA_MBX_QUERY_CFG_MAX_QP_MASK) >>
	    OCRDMA_MBX_QUERY_CFG_MAX_QP_SHIFT;
	attr->max_srq =
		(rsp->max_srq_rpir_qps & OCRDMA_MBX_QUERY_CFG_MAX_SRQ_MASK) >>
		OCRDMA_MBX_QUERY_CFG_MAX_SRQ_OFFSET;
	attr->max_send_sge = ((rsp->max_recv_send_sge &
			       OCRDMA_MBX_QUERY_CFG_MAX_SEND_SGE_MASK) >>
			      OCRDMA_MBX_QUERY_CFG_MAX_SEND_SGE_SHIFT);
	attr->max_recv_sge = (rsp->max_recv_send_sge &
			      OCRDMA_MBX_QUERY_CFG_MAX_RECV_SGE_MASK) >>
	    OCRDMA_MBX_QUERY_CFG_MAX_RECV_SGE_SHIFT;
	attr->max_srq_sge = (rsp->max_srq_rqe_sge &
			      OCRDMA_MBX_QUERY_CFG_MAX_SRQ_SGE_MASK) >>
	    OCRDMA_MBX_QUERY_CFG_MAX_SRQ_SGE_OFFSET;
	attr->max_rdma_sge = (rsp->max_wr_rd_sge &
			      OCRDMA_MBX_QUERY_CFG_MAX_RD_SGE_MASK) >>
	    OCRDMA_MBX_QUERY_CFG_MAX_RD_SGE_SHIFT;
	attr->max_ord_per_qp = (rsp->max_ird_ord_per_qp &
				OCRDMA_MBX_QUERY_CFG_MAX_ORD_PER_QP_MASK) >>
	    OCRDMA_MBX_QUERY_CFG_MAX_ORD_PER_QP_SHIFT;
	attr->max_ird_per_qp = (rsp->max_ird_ord_per_qp &
				OCRDMA_MBX_QUERY_CFG_MAX_IRD_PER_QP_MASK) >>
	    OCRDMA_MBX_QUERY_CFG_MAX_IRD_PER_QP_SHIFT;
	attr->cq_overflow_detect = (rsp->qp_srq_cq_ird_ord &
				    OCRDMA_MBX_QUERY_CFG_CQ_OVERFLOW_MASK) >>
	    OCRDMA_MBX_QUERY_CFG_CQ_OVERFLOW_SHIFT;
	attr->srq_supported = (rsp->qp_srq_cq_ird_ord &
			       OCRDMA_MBX_QUERY_CFG_SRQ_SUPPORTED_MASK) >>
	    OCRDMA_MBX_QUERY_CFG_SRQ_SUPPORTED_SHIFT;
	attr->local_ca_ack_delay = (rsp->max_pd_ca_ack_delay &
				    OCRDMA_MBX_QUERY_CFG_CA_ACK_DELAY_MASK) >>
	    OCRDMA_MBX_QUERY_CFG_CA_ACK_DELAY_SHIFT;
	attr->max_mw = rsp->max_mw;
	attr->max_mr = rsp->max_mr;
	attr->max_mr_size = ((u64)rsp->max_mr_size_hi << 32) |
			      rsp->max_mr_size_lo;
	attr->max_pages_per_frmr = rsp->max_pages_per_frmr;
	attr->max_num_mr_pbl = rsp->max_num_mr_pbl;
	attr->max_cqe = rsp->max_cq_cqes_per_cq &
			OCRDMA_MBX_QUERY_CFG_MAX_CQES_PER_CQ_MASK;
	attr->max_cq = (rsp->max_cq_cqes_per_cq &
			OCRDMA_MBX_QUERY_CFG_MAX_CQ_MASK) >>
			OCRDMA_MBX_QUERY_CFG_MAX_CQ_OFFSET;
	attr->wqe_size = ((rsp->wqe_rqe_stride_max_dpp_cqs &
		OCRDMA_MBX_QUERY_CFG_MAX_WQE_SIZE_MASK) >>
		OCRDMA_MBX_QUERY_CFG_MAX_WQE_SIZE_OFFSET) *
		OCRDMA_WQE_STRIDE;
	attr->rqe_size = ((rsp->wqe_rqe_stride_max_dpp_cqs &
		OCRDMA_MBX_QUERY_CFG_MAX_RQE_SIZE_MASK) >>
		OCRDMA_MBX_QUERY_CFG_MAX_RQE_SIZE_OFFSET) *
		OCRDMA_WQE_STRIDE;
	attr->max_inline_data =
	    attr->wqe_size - (sizeof(struct ocrdma_hdr_wqe) +
			      sizeof(struct ocrdma_sge));
	if (ocrdma_get_asic_type(dev) == OCRDMA_ASIC_GEN_SKH_R) {
		attr->ird = 1;
		attr->ird_page_size = OCRDMA_MIN_Q_PAGE_SIZE;
		attr->num_ird_pages = MAX_OCRDMA_IRD_PAGES;
	}
	dev->attr.max_wqe = rsp->max_wqes_rqes_per_q >>
		 OCRDMA_MBX_QUERY_CFG_MAX_WQES_PER_WQ_OFFSET;
	dev->attr.max_rqe = rsp->max_wqes_rqes_per_q &
		OCRDMA_MBX_QUERY_CFG_MAX_RQES_PER_RQ_MASK;
}

static int ocrdma_check_fw_config(struct ocrdma_dev *dev,
				   struct ocrdma_fw_conf_rsp *conf)
{
	u32 fn_mode;

	fn_mode = conf->fn_mode & OCRDMA_FN_MODE_RDMA;
	if (fn_mode != OCRDMA_FN_MODE_RDMA)
		return -EINVAL;
	dev->base_eqid = conf->base_eqid;
	dev->max_eq = conf->max_eq;
	return 0;
}

/* can be issued only during init time. */
static int ocrdma_mbx_query_fw_ver(struct ocrdma_dev *dev)
{
	int status = -ENOMEM;
	struct ocrdma_mqe *cmd;
	struct ocrdma_fw_ver_rsp *rsp;

	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_GET_FW_VER, sizeof(*cmd));
	if (!cmd)
		return -ENOMEM;
	ocrdma_init_mch((struct ocrdma_mbx_hdr *)&cmd->u.cmd[0],
			OCRDMA_CMD_GET_FW_VER,
			OCRDMA_SUBSYS_COMMON, sizeof(*cmd));

	status = ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);
	if (status)
		goto mbx_err;
	rsp = (struct ocrdma_fw_ver_rsp *)cmd;
	memset(&dev->attr.fw_ver[0], 0, sizeof(dev->attr.fw_ver));
	memcpy(&dev->attr.fw_ver[0], &rsp->running_ver[0],
	       sizeof(rsp->running_ver));
	ocrdma_le32_to_cpu(dev->attr.fw_ver, sizeof(rsp->running_ver));
mbx_err:
	kfree(cmd);
	return status;
}

/* can be issued only during init time. */
static int ocrdma_mbx_query_fw_config(struct ocrdma_dev *dev)
{
	int status = -ENOMEM;
	struct ocrdma_mqe *cmd;
	struct ocrdma_fw_conf_rsp *rsp;

	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_GET_FW_CONFIG, sizeof(*cmd));
	if (!cmd)
		return -ENOMEM;
	ocrdma_init_mch((struct ocrdma_mbx_hdr *)&cmd->u.cmd[0],
			OCRDMA_CMD_GET_FW_CONFIG,
			OCRDMA_SUBSYS_COMMON, sizeof(*cmd));
	status = ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);
	if (status)
		goto mbx_err;
	rsp = (struct ocrdma_fw_conf_rsp *)cmd;
	status = ocrdma_check_fw_config(dev, rsp);
mbx_err:
	kfree(cmd);
	return status;
}

int ocrdma_mbx_rdma_stats(struct ocrdma_dev *dev, bool reset)
{
	struct ocrdma_rdma_stats_req *req = dev->stats_mem.va;
	struct ocrdma_mqe *mqe = &dev->stats_mem.mqe;
	struct ocrdma_rdma_stats_resp *old_stats;
	int status;

	old_stats = kmalloc(sizeof(*old_stats), GFP_KERNEL);
	if (old_stats == NULL)
		return -ENOMEM;

	memset(mqe, 0, sizeof(*mqe));
	mqe->hdr.pyld_len = dev->stats_mem.size;
	mqe->hdr.spcl_sge_cnt_emb |=
			(1 << OCRDMA_MQE_HDR_SGE_CNT_SHIFT) &
				OCRDMA_MQE_HDR_SGE_CNT_MASK;
	mqe->u.nonemb_req.sge[0].pa_lo = (u32) (dev->stats_mem.pa & 0xffffffff);
	mqe->u.nonemb_req.sge[0].pa_hi = (u32) upper_32_bits(dev->stats_mem.pa);
	mqe->u.nonemb_req.sge[0].len = dev->stats_mem.size;

	/* Cache the old stats */
	memcpy(old_stats, req, sizeof(struct ocrdma_rdma_stats_resp));
	memset(req, 0, dev->stats_mem.size);

	ocrdma_init_mch((struct ocrdma_mbx_hdr *)req,
			OCRDMA_CMD_GET_RDMA_STATS,
			OCRDMA_SUBSYS_ROCE,
			dev->stats_mem.size);
	if (reset)
		req->reset_stats = reset;

	status = ocrdma_nonemb_mbx_cmd(dev, mqe, dev->stats_mem.va);
	if (status)
		/* Copy from cache, if mbox fails */
		memcpy(req, old_stats, sizeof(struct ocrdma_rdma_stats_resp));
	else
		ocrdma_le32_to_cpu(req, dev->stats_mem.size);

	kfree(old_stats);
	return status;
}

static int ocrdma_mbx_get_ctrl_attribs(struct ocrdma_dev *dev)
{
	int status = -ENOMEM;
	struct ocrdma_dma_mem dma;
	struct ocrdma_mqe *mqe;
	struct ocrdma_get_ctrl_attribs_rsp *ctrl_attr_rsp;
	struct mgmt_hba_attribs *hba_attribs;

	mqe = kzalloc(sizeof(struct ocrdma_mqe), GFP_KERNEL);
	if (!mqe)
		return status;

	dma.size = sizeof(struct ocrdma_get_ctrl_attribs_rsp);
	dma.va	 = dma_alloc_coherent(&dev->nic_info.pdev->dev,
					dma.size, &dma.pa, GFP_KERNEL);
	if (!dma.va)
		goto free_mqe;

	mqe->hdr.pyld_len = dma.size;
	mqe->hdr.spcl_sge_cnt_emb |=
			(1 << OCRDMA_MQE_HDR_SGE_CNT_SHIFT) &
			OCRDMA_MQE_HDR_SGE_CNT_MASK;
	mqe->u.nonemb_req.sge[0].pa_lo = (u32) (dma.pa & 0xffffffff);
	mqe->u.nonemb_req.sge[0].pa_hi = (u32) upper_32_bits(dma.pa);
	mqe->u.nonemb_req.sge[0].len = dma.size;

	ocrdma_init_mch((struct ocrdma_mbx_hdr *)dma.va,
			OCRDMA_CMD_GET_CTRL_ATTRIBUTES,
			OCRDMA_SUBSYS_COMMON,
			dma.size);

	status = ocrdma_nonemb_mbx_cmd(dev, mqe, dma.va);
	if (!status) {
		ctrl_attr_rsp = (struct ocrdma_get_ctrl_attribs_rsp *)dma.va;
		hba_attribs = &ctrl_attr_rsp->ctrl_attribs.hba_attribs;

		dev->hba_port_num = (hba_attribs->ptpnum_maxdoms_hbast_cv &
					OCRDMA_HBA_ATTRB_PTNUM_MASK)
					>> OCRDMA_HBA_ATTRB_PTNUM_SHIFT;
		strlcpy(dev->model_number,
			hba_attribs->controller_model_number,
			sizeof(dev->model_number));
	}
	dma_free_coherent(&dev->nic_info.pdev->dev, dma.size, dma.va, dma.pa);
free_mqe:
	kfree(mqe);
	return status;
}

static int ocrdma_mbx_query_dev(struct ocrdma_dev *dev)
{
	int status = -ENOMEM;
	struct ocrdma_mbx_query_config *rsp;
	struct ocrdma_mqe *cmd;

	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_QUERY_CONFIG, sizeof(*cmd));
	if (!cmd)
		return status;
	status = ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);
	if (status)
		goto mbx_err;
	rsp = (struct ocrdma_mbx_query_config *)cmd;
	ocrdma_get_attr(dev, &dev->attr, rsp);
mbx_err:
	kfree(cmd);
	return status;
}

int ocrdma_mbx_get_link_speed(struct ocrdma_dev *dev, u8 *lnk_speed,
			      u8 *lnk_state)
{
	int status = -ENOMEM;
	struct ocrdma_get_link_speed_rsp *rsp;
	struct ocrdma_mqe *cmd;

	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_QUERY_NTWK_LINK_CONFIG_V1,
				  sizeof(*cmd));
	if (!cmd)
		return status;
	ocrdma_init_mch((struct ocrdma_mbx_hdr *)&cmd->u.cmd[0],
			OCRDMA_CMD_QUERY_NTWK_LINK_CONFIG_V1,
			OCRDMA_SUBSYS_COMMON, sizeof(*cmd));

	((struct ocrdma_mbx_hdr *)cmd->u.cmd)->rsvd_version = 0x1;

	status = ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);
	if (status)
		goto mbx_err;

	rsp = (struct ocrdma_get_link_speed_rsp *)cmd;
	if (lnk_speed)
		*lnk_speed = (rsp->pflt_pps_ld_pnum & OCRDMA_PHY_PS_MASK)
			      >> OCRDMA_PHY_PS_SHIFT;
	if (lnk_state)
		*lnk_state = (rsp->res_lnk_st & OCRDMA_LINK_ST_MASK);

mbx_err:
	kfree(cmd);
	return status;
}

static int ocrdma_mbx_get_phy_info(struct ocrdma_dev *dev)
{
	int status = -ENOMEM;
	struct ocrdma_mqe *cmd;
	struct ocrdma_get_phy_info_rsp *rsp;

	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_PHY_DETAILS, sizeof(*cmd));
	if (!cmd)
		return status;

	ocrdma_init_mch((struct ocrdma_mbx_hdr *)&cmd->u.cmd[0],
			OCRDMA_CMD_PHY_DETAILS, OCRDMA_SUBSYS_COMMON,
			sizeof(*cmd));

	status = ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);
	if (status)
		goto mbx_err;

	rsp = (struct ocrdma_get_phy_info_rsp *)cmd;
	dev->phy.phy_type =
			(rsp->ityp_ptyp & OCRDMA_PHY_TYPE_MASK);
	dev->phy.interface_type =
			(rsp->ityp_ptyp & OCRDMA_IF_TYPE_MASK)
				>> OCRDMA_IF_TYPE_SHIFT;
	dev->phy.auto_speeds_supported  =
			(rsp->fspeed_aspeed & OCRDMA_ASPEED_SUPP_MASK);
	dev->phy.fixed_speeds_supported =
			(rsp->fspeed_aspeed & OCRDMA_FSPEED_SUPP_MASK)
				>> OCRDMA_FSPEED_SUPP_SHIFT;
mbx_err:
	kfree(cmd);
	return status;
}

int ocrdma_mbx_alloc_pd(struct ocrdma_dev *dev, struct ocrdma_pd *pd)
{
	int status = -ENOMEM;
	struct ocrdma_alloc_pd *cmd;
	struct ocrdma_alloc_pd_rsp *rsp;

	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_ALLOC_PD, sizeof(*cmd));
	if (!cmd)
		return status;
	if (pd->dpp_enabled)
		cmd->enable_dpp_rsvd |= OCRDMA_ALLOC_PD_ENABLE_DPP;
	status = ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);
	if (status)
		goto mbx_err;
	rsp = (struct ocrdma_alloc_pd_rsp *)cmd;
	pd->id = rsp->dpp_page_pdid & OCRDMA_ALLOC_PD_RSP_PDID_MASK;
	if (rsp->dpp_page_pdid & OCRDMA_ALLOC_PD_RSP_DPP) {
		pd->dpp_enabled = true;
		pd->dpp_page = rsp->dpp_page_pdid >>
				OCRDMA_ALLOC_PD_RSP_DPP_PAGE_SHIFT;
	} else {
		pd->dpp_enabled = false;
		pd->num_dpp_qp = 0;
	}
mbx_err:
	kfree(cmd);
	return status;
}

int ocrdma_mbx_dealloc_pd(struct ocrdma_dev *dev, struct ocrdma_pd *pd)
{
	int status = -ENOMEM;
	struct ocrdma_dealloc_pd *cmd;

	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_DEALLOC_PD, sizeof(*cmd));
	if (!cmd)
		return status;
	cmd->id = pd->id;
	status = ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);
	kfree(cmd);
	return status;
}


static int ocrdma_mbx_alloc_pd_range(struct ocrdma_dev *dev)
{
	int status = -ENOMEM;
	size_t pd_bitmap_size;
	struct ocrdma_alloc_pd_range *cmd;
	struct ocrdma_alloc_pd_range_rsp *rsp;

	/* Pre allocate the DPP PDs */
	if (dev->attr.max_dpp_pds) {
		cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_ALLOC_PD_RANGE,
					  sizeof(*cmd));
		if (!cmd)
			return -ENOMEM;
		cmd->pd_count = dev->attr.max_dpp_pds;
		cmd->enable_dpp_rsvd |= OCRDMA_ALLOC_PD_ENABLE_DPP;
		status = ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);
		rsp = (struct ocrdma_alloc_pd_range_rsp *)cmd;

		if (!status && (rsp->dpp_page_pdid & OCRDMA_ALLOC_PD_RSP_DPP) &&
		    rsp->pd_count) {
			dev->pd_mgr->dpp_page_index = rsp->dpp_page_pdid >>
					OCRDMA_ALLOC_PD_RSP_DPP_PAGE_SHIFT;
			dev->pd_mgr->pd_dpp_start = rsp->dpp_page_pdid &
					OCRDMA_ALLOC_PD_RNG_RSP_START_PDID_MASK;
			dev->pd_mgr->max_dpp_pd = rsp->pd_count;
			pd_bitmap_size =
				BITS_TO_LONGS(rsp->pd_count) * sizeof(long);
			dev->pd_mgr->pd_dpp_bitmap = kzalloc(pd_bitmap_size,
							     GFP_KERNEL);
		}
		kfree(cmd);
	}

	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_ALLOC_PD_RANGE, sizeof(*cmd));
	if (!cmd)
		return -ENOMEM;

	cmd->pd_count = dev->attr.max_pd - dev->attr.max_dpp_pds;
	status = ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);
	rsp = (struct ocrdma_alloc_pd_range_rsp *)cmd;
	if (!status && rsp->pd_count) {
		dev->pd_mgr->pd_norm_start = rsp->dpp_page_pdid &
					OCRDMA_ALLOC_PD_RNG_RSP_START_PDID_MASK;
		dev->pd_mgr->max_normal_pd = rsp->pd_count;
		pd_bitmap_size = BITS_TO_LONGS(rsp->pd_count) * sizeof(long);
		dev->pd_mgr->pd_norm_bitmap = kzalloc(pd_bitmap_size,
						      GFP_KERNEL);
	}
	kfree(cmd);

	if (dev->pd_mgr->pd_norm_bitmap || dev->pd_mgr->pd_dpp_bitmap) {
		/* Enable PD resource manager */
		dev->pd_mgr->pd_prealloc_valid = true;
		return 0;
	}
	return status;
}

static void ocrdma_mbx_dealloc_pd_range(struct ocrdma_dev *dev)
{
	struct ocrdma_dealloc_pd_range *cmd;

	/* return normal PDs to firmware */
	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_DEALLOC_PD_RANGE, sizeof(*cmd));
	if (!cmd)
		goto mbx_err;

	if (dev->pd_mgr->max_normal_pd) {
		cmd->start_pd_id = dev->pd_mgr->pd_norm_start;
		cmd->pd_count = dev->pd_mgr->max_normal_pd;
		ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);
	}

	if (dev->pd_mgr->max_dpp_pd) {
		kfree(cmd);
		/* return DPP PDs to firmware */
		cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_DEALLOC_PD_RANGE,
					  sizeof(*cmd));
		if (!cmd)
			goto mbx_err;

		cmd->start_pd_id = dev->pd_mgr->pd_dpp_start;
		cmd->pd_count = dev->pd_mgr->max_dpp_pd;
		ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);
	}
mbx_err:
	kfree(cmd);
}

void ocrdma_alloc_pd_pool(struct ocrdma_dev *dev)
{
	int status;

	dev->pd_mgr = kzalloc(sizeof(struct ocrdma_pd_resource_mgr),
			      GFP_KERNEL);
	if (!dev->pd_mgr)
		return;

	status = ocrdma_mbx_alloc_pd_range(dev);
	if (status) {
		pr_err("%s(%d) Unable to initialize PD pool, using default.\n",
			 __func__, dev->id);
	}
}

static void ocrdma_free_pd_pool(struct ocrdma_dev *dev)
{
	ocrdma_mbx_dealloc_pd_range(dev);
	kfree(dev->pd_mgr->pd_norm_bitmap);
	kfree(dev->pd_mgr->pd_dpp_bitmap);
	kfree(dev->pd_mgr);
}

static int ocrdma_build_q_conf(u32 *num_entries, int entry_size,
			       int *num_pages, int *page_size)
{
	int i;
	int mem_size;

	*num_entries = roundup_pow_of_two(*num_entries);
	mem_size = *num_entries * entry_size;
	/* find the possible lowest possible multiplier */
	for (i = 0; i < OCRDMA_MAX_Q_PAGE_SIZE_CNT; i++) {
		if (mem_size <= (OCRDMA_Q_PAGE_BASE_SIZE << i))
			break;
	}
	if (i >= OCRDMA_MAX_Q_PAGE_SIZE_CNT)
		return -EINVAL;
	mem_size = roundup(mem_size,
		       ((OCRDMA_Q_PAGE_BASE_SIZE << i) / OCRDMA_MAX_Q_PAGES));
	*num_pages =
	    mem_size / ((OCRDMA_Q_PAGE_BASE_SIZE << i) / OCRDMA_MAX_Q_PAGES);
	*page_size = ((OCRDMA_Q_PAGE_BASE_SIZE << i) / OCRDMA_MAX_Q_PAGES);
	*num_entries = mem_size / entry_size;
	return 0;
}

static int ocrdma_mbx_create_ah_tbl(struct ocrdma_dev *dev)
{
	int i;
	int status = -ENOMEM;
	int max_ah;
	struct ocrdma_create_ah_tbl *cmd;
	struct ocrdma_create_ah_tbl_rsp *rsp;
	struct pci_dev *pdev = dev->nic_info.pdev;
	dma_addr_t pa;
	struct ocrdma_pbe *pbes;

	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_CREATE_AH_TBL, sizeof(*cmd));
	if (!cmd)
		return status;

	max_ah = OCRDMA_MAX_AH;
	dev->av_tbl.size = sizeof(struct ocrdma_av) * max_ah;

	/* number of PBEs in PBL */
	cmd->ah_conf = (OCRDMA_AH_TBL_PAGES <<
				OCRDMA_CREATE_AH_NUM_PAGES_SHIFT) &
				OCRDMA_CREATE_AH_NUM_PAGES_MASK;

	/* page size */
	for (i = 0; i < OCRDMA_MAX_Q_PAGE_SIZE_CNT; i++) {
		if (PAGE_SIZE == (OCRDMA_MIN_Q_PAGE_SIZE << i))
			break;
	}
	cmd->ah_conf |= (i << OCRDMA_CREATE_AH_PAGE_SIZE_SHIFT) &
				OCRDMA_CREATE_AH_PAGE_SIZE_MASK;

	/* ah_entry size */
	cmd->ah_conf |= (sizeof(struct ocrdma_av) <<
				OCRDMA_CREATE_AH_ENTRY_SIZE_SHIFT) &
				OCRDMA_CREATE_AH_ENTRY_SIZE_MASK;

	dev->av_tbl.pbl.va = dma_alloc_coherent(&pdev->dev, PAGE_SIZE,
						&dev->av_tbl.pbl.pa,
						GFP_KERNEL);
	if (dev->av_tbl.pbl.va == NULL)
		goto mem_err;

	dev->av_tbl.va = dma_alloc_coherent(&pdev->dev, dev->av_tbl.size,
					    &pa, GFP_KERNEL);
	if (dev->av_tbl.va == NULL)
		goto mem_err_ah;
	dev->av_tbl.pa = pa;
	dev->av_tbl.num_ah = max_ah;

	pbes = (struct ocrdma_pbe *)dev->av_tbl.pbl.va;
	for (i = 0; i < dev->av_tbl.size / OCRDMA_MIN_Q_PAGE_SIZE; i++) {
		pbes[i].pa_lo = (u32)cpu_to_le32(pa & 0xffffffff);
		pbes[i].pa_hi = (u32)cpu_to_le32(upper_32_bits(pa));
		pa += PAGE_SIZE;
	}
	cmd->tbl_addr[0].lo = (u32)(dev->av_tbl.pbl.pa & 0xFFFFFFFF);
	cmd->tbl_addr[0].hi = (u32)upper_32_bits(dev->av_tbl.pbl.pa);
	status = ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);
	if (status)
		goto mbx_err;
	rsp = (struct ocrdma_create_ah_tbl_rsp *)cmd;
	dev->av_tbl.ahid = rsp->ahid & 0xFFFF;
	kfree(cmd);
	return 0;

mbx_err:
	dma_free_coherent(&pdev->dev, dev->av_tbl.size, dev->av_tbl.va,
			  dev->av_tbl.pa);
	dev->av_tbl.va = NULL;
mem_err_ah:
	dma_free_coherent(&pdev->dev, PAGE_SIZE, dev->av_tbl.pbl.va,
			  dev->av_tbl.pbl.pa);
	dev->av_tbl.pbl.va = NULL;
	dev->av_tbl.size = 0;
mem_err:
	kfree(cmd);
	return status;
}

static void ocrdma_mbx_delete_ah_tbl(struct ocrdma_dev *dev)
{
	struct ocrdma_delete_ah_tbl *cmd;
	struct pci_dev *pdev = dev->nic_info.pdev;

	if (dev->av_tbl.va == NULL)
		return;

	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_DELETE_AH_TBL, sizeof(*cmd));
	if (!cmd)
		return;
	cmd->ahid = dev->av_tbl.ahid;

	ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);
	dma_free_coherent(&pdev->dev, dev->av_tbl.size, dev->av_tbl.va,
			  dev->av_tbl.pa);
	dev->av_tbl.va = NULL;
	dma_free_coherent(&pdev->dev, PAGE_SIZE, dev->av_tbl.pbl.va,
			  dev->av_tbl.pbl.pa);
	kfree(cmd);
}

/* Multiple CQs uses the EQ. This routine returns least used
 * EQ to associate with CQ. This will distributes the interrupt
 * processing and CPU load to associated EQ, vector and so to that CPU.
 */
static u16 ocrdma_bind_eq(struct ocrdma_dev *dev)
{
	int i, selected_eq = 0, cq_cnt = 0;
	u16 eq_id;

	mutex_lock(&dev->dev_lock);
	cq_cnt = dev->eq_tbl[0].cq_cnt;
	eq_id = dev->eq_tbl[0].q.id;
	/* find the EQ which is has the least number of
	 * CQs associated with it.
	 */
	for (i = 0; i < dev->eq_cnt; i++) {
		if (dev->eq_tbl[i].cq_cnt < cq_cnt) {
			cq_cnt = dev->eq_tbl[i].cq_cnt;
			eq_id = dev->eq_tbl[i].q.id;
			selected_eq = i;
		}
	}
	dev->eq_tbl[selected_eq].cq_cnt += 1;
	mutex_unlock(&dev->dev_lock);
	return eq_id;
}

static void ocrdma_unbind_eq(struct ocrdma_dev *dev, u16 eq_id)
{
	int i;

	mutex_lock(&dev->dev_lock);
	i = ocrdma_get_eq_table_index(dev, eq_id);
	if (i == -EINVAL)
		BUG();
	dev->eq_tbl[i].cq_cnt -= 1;
	mutex_unlock(&dev->dev_lock);
}

int ocrdma_mbx_create_cq(struct ocrdma_dev *dev, struct ocrdma_cq *cq,
			 int entries, int dpp_cq, u16 pd_id)
{
	int status = -ENOMEM; int max_hw_cqe;
	struct pci_dev *pdev = dev->nic_info.pdev;
	struct ocrdma_create_cq *cmd;
	struct ocrdma_create_cq_rsp *rsp;
	u32 hw_pages, cqe_size, page_size, cqe_count;

	if (entries > dev->attr.max_cqe) {
		pr_err("%s(%d) max_cqe=0x%x, requester_cqe=0x%x\n",
		       __func__, dev->id, dev->attr.max_cqe, entries);
		return -EINVAL;
	}
	if (dpp_cq && (ocrdma_get_asic_type(dev) != OCRDMA_ASIC_GEN_SKH_R))
		return -EINVAL;

	if (dpp_cq) {
		cq->max_hw_cqe = 1;
		max_hw_cqe = 1;
		cqe_size = OCRDMA_DPP_CQE_SIZE;
		hw_pages = 1;
	} else {
		cq->max_hw_cqe = dev->attr.max_cqe;
		max_hw_cqe = dev->attr.max_cqe;
		cqe_size = sizeof(struct ocrdma_cqe);
		hw_pages = OCRDMA_CREATE_CQ_MAX_PAGES;
	}

	cq->len = roundup(max_hw_cqe * cqe_size, OCRDMA_MIN_Q_PAGE_SIZE);

	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_CREATE_CQ, sizeof(*cmd));
	if (!cmd)
		return -ENOMEM;
	ocrdma_init_mch(&cmd->cmd.req, OCRDMA_CMD_CREATE_CQ,
			OCRDMA_SUBSYS_COMMON, sizeof(*cmd));
	cq->va = dma_alloc_coherent(&pdev->dev, cq->len, &cq->pa, GFP_KERNEL);
	if (!cq->va) {
		status = -ENOMEM;
		goto mem_err;
	}
	page_size = cq->len / hw_pages;
	cmd->cmd.pgsz_pgcnt = (page_size / OCRDMA_MIN_Q_PAGE_SIZE) <<
					OCRDMA_CREATE_CQ_PAGE_SIZE_SHIFT;
	cmd->cmd.pgsz_pgcnt |= hw_pages;
	cmd->cmd.ev_cnt_flags = OCRDMA_CREATE_CQ_DEF_FLAGS;

	cq->eqn = ocrdma_bind_eq(dev);
	cmd->cmd.req.rsvd_version = OCRDMA_CREATE_CQ_VER3;
	cqe_count = cq->len / cqe_size;
	cq->cqe_cnt = cqe_count;
	if (cqe_count > 1024) {
		/* Set cnt to 3 to indicate more than 1024 cq entries */
		cmd->cmd.ev_cnt_flags |= (0x3 << OCRDMA_CREATE_CQ_CNT_SHIFT);
	} else {
		u8 count = 0;
		switch (cqe_count) {
		case 256:
			count = 0;
			break;
		case 512:
			count = 1;
			break;
		case 1024:
			count = 2;
			break;
		default:
			goto mbx_err;
		}
		cmd->cmd.ev_cnt_flags |= (count << OCRDMA_CREATE_CQ_CNT_SHIFT);
	}
	/* shared eq between all the consumer cqs. */
	cmd->cmd.eqn = cq->eqn;
	if (ocrdma_get_asic_type(dev) == OCRDMA_ASIC_GEN_SKH_R) {
		if (dpp_cq)
			cmd->cmd.pgsz_pgcnt |= OCRDMA_CREATE_CQ_DPP <<
				OCRDMA_CREATE_CQ_TYPE_SHIFT;
		cq->phase_change = false;
		cmd->cmd.pdid_cqecnt = (cq->len / cqe_size);
	} else {
		cmd->cmd.pdid_cqecnt = (cq->len / cqe_size) - 1;
		cmd->cmd.ev_cnt_flags |= OCRDMA_CREATE_CQ_FLAGS_AUTO_VALID;
		cq->phase_change = true;
	}

	/* pd_id valid only for v3 */
	cmd->cmd.pdid_cqecnt |= (pd_id <<
		OCRDMA_CREATE_CQ_CMD_PDID_SHIFT);
	ocrdma_build_q_pages(&cmd->cmd.pa[0], hw_pages, cq->pa, page_size);
	status = ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);
	if (status)
		goto mbx_err;

	rsp = (struct ocrdma_create_cq_rsp *)cmd;
	cq->id = (u16) (rsp->rsp.cq_id & OCRDMA_CREATE_CQ_RSP_CQ_ID_MASK);
	kfree(cmd);
	return 0;
mbx_err:
	ocrdma_unbind_eq(dev, cq->eqn);
	dma_free_coherent(&pdev->dev, cq->len, cq->va, cq->pa);
mem_err:
	kfree(cmd);
	return status;
}

void ocrdma_mbx_destroy_cq(struct ocrdma_dev *dev, struct ocrdma_cq *cq)
{
	struct ocrdma_destroy_cq *cmd;

	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_DELETE_CQ, sizeof(*cmd));
	if (!cmd)
		return;
	ocrdma_init_mch(&cmd->req, OCRDMA_CMD_DELETE_CQ,
			OCRDMA_SUBSYS_COMMON, sizeof(*cmd));

	cmd->bypass_flush_qid |=
	    (cq->id << OCRDMA_DESTROY_CQ_QID_SHIFT) &
	    OCRDMA_DESTROY_CQ_QID_MASK;

	ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);
	ocrdma_unbind_eq(dev, cq->eqn);
	dma_free_coherent(&dev->nic_info.pdev->dev, cq->len, cq->va, cq->pa);
	kfree(cmd);
}

int ocrdma_mbx_alloc_lkey(struct ocrdma_dev *dev, struct ocrdma_hw_mr *hwmr,
			  u32 pdid, int addr_check)
{
	int status = -ENOMEM;
	struct ocrdma_alloc_lkey *cmd;
	struct ocrdma_alloc_lkey_rsp *rsp;

	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_ALLOC_LKEY, sizeof(*cmd));
	if (!cmd)
		return status;
	cmd->pdid = pdid;
	cmd->pbl_sz_flags |= addr_check;
	cmd->pbl_sz_flags |= (hwmr->fr_mr << OCRDMA_ALLOC_LKEY_FMR_SHIFT);
	cmd->pbl_sz_flags |=
	    (hwmr->remote_wr << OCRDMA_ALLOC_LKEY_REMOTE_WR_SHIFT);
	cmd->pbl_sz_flags |=
	    (hwmr->remote_rd << OCRDMA_ALLOC_LKEY_REMOTE_RD_SHIFT);
	cmd->pbl_sz_flags |=
	    (hwmr->local_wr << OCRDMA_ALLOC_LKEY_LOCAL_WR_SHIFT);
	cmd->pbl_sz_flags |=
	    (hwmr->remote_atomic << OCRDMA_ALLOC_LKEY_REMOTE_ATOMIC_SHIFT);
	cmd->pbl_sz_flags |=
	    (hwmr->num_pbls << OCRDMA_ALLOC_LKEY_PBL_SIZE_SHIFT);

	status = ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);
	if (status)
		goto mbx_err;
	rsp = (struct ocrdma_alloc_lkey_rsp *)cmd;
	hwmr->lkey = rsp->lrkey;
mbx_err:
	kfree(cmd);
	return status;
}

int ocrdma_mbx_dealloc_lkey(struct ocrdma_dev *dev, int fr_mr, u32 lkey)
{
	int status;
	struct ocrdma_dealloc_lkey *cmd;

	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_DEALLOC_LKEY, sizeof(*cmd));
	if (!cmd)
		return -ENOMEM;
	cmd->lkey = lkey;
	cmd->rsvd_frmr = fr_mr ? 1 : 0;
	status = ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);

	kfree(cmd);
	return status;
}

static int ocrdma_mbx_reg_mr(struct ocrdma_dev *dev, struct ocrdma_hw_mr *hwmr,
			     u32 pdid, u32 pbl_cnt, u32 pbe_size, u32 last)
{
	int status = -ENOMEM;
	int i;
	struct ocrdma_reg_nsmr *cmd;
	struct ocrdma_reg_nsmr_rsp *rsp;
	u64 fbo = hwmr->va & (hwmr->pbe_size - 1);

	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_REGISTER_NSMR, sizeof(*cmd));
	if (!cmd)
		return -ENOMEM;
	cmd->num_pbl_pdid =
	    pdid | (hwmr->num_pbls << OCRDMA_REG_NSMR_NUM_PBL_SHIFT);
	cmd->fr_mr = hwmr->fr_mr;

	cmd->flags_hpage_pbe_sz |= (hwmr->remote_wr <<
				    OCRDMA_REG_NSMR_REMOTE_WR_SHIFT);
	cmd->flags_hpage_pbe_sz |= (hwmr->remote_rd <<
				    OCRDMA_REG_NSMR_REMOTE_RD_SHIFT);
	cmd->flags_hpage_pbe_sz |= (hwmr->local_wr <<
				    OCRDMA_REG_NSMR_LOCAL_WR_SHIFT);
	cmd->flags_hpage_pbe_sz |= (hwmr->remote_atomic <<
				    OCRDMA_REG_NSMR_REMOTE_ATOMIC_SHIFT);
	cmd->flags_hpage_pbe_sz |= (hwmr->mw_bind <<
				    OCRDMA_REG_NSMR_BIND_MEMWIN_SHIFT);
	cmd->flags_hpage_pbe_sz |= (last << OCRDMA_REG_NSMR_LAST_SHIFT);

	cmd->flags_hpage_pbe_sz |= (hwmr->pbe_size / OCRDMA_MIN_HPAGE_SIZE);
	cmd->flags_hpage_pbe_sz |= (hwmr->pbl_size / OCRDMA_MIN_HPAGE_SIZE) <<
					OCRDMA_REG_NSMR_HPAGE_SIZE_SHIFT;
	cmd->totlen_low = hwmr->len;
	cmd->totlen_high = upper_32_bits(hwmr->len);
	cmd->fbo_low = lower_32_bits(fbo);
	cmd->fbo_high = upper_32_bits(fbo);
	cmd->va_loaddr = (u32) hwmr->va;
	cmd->va_hiaddr = (u32) upper_32_bits(hwmr->va);

	for (i = 0; i < pbl_cnt; i++) {
		cmd->pbl[i].lo = (u32) (hwmr->pbl_table[i].pa & 0xffffffff);
		cmd->pbl[i].hi = upper_32_bits(hwmr->pbl_table[i].pa);
	}
	status = ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);
	if (status)
		goto mbx_err;
	rsp = (struct ocrdma_reg_nsmr_rsp *)cmd;
	hwmr->lkey = rsp->lrkey;
mbx_err:
	kfree(cmd);
	return status;
}

static int ocrdma_mbx_reg_mr_cont(struct ocrdma_dev *dev,
				  struct ocrdma_hw_mr *hwmr, u32 pbl_cnt,
				  u32 pbl_offset, u32 last)
{
	int status;
	int i;
	struct ocrdma_reg_nsmr_cont *cmd;

	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_REGISTER_NSMR_CONT, sizeof(*cmd));
	if (!cmd)
		return -ENOMEM;
	cmd->lrkey = hwmr->lkey;
	cmd->num_pbl_offset = (pbl_cnt << OCRDMA_REG_NSMR_CONT_NUM_PBL_SHIFT) |
	    (pbl_offset & OCRDMA_REG_NSMR_CONT_PBL_SHIFT_MASK);
	cmd->last = last << OCRDMA_REG_NSMR_CONT_LAST_SHIFT;

	for (i = 0; i < pbl_cnt; i++) {
		cmd->pbl[i].lo =
		    (u32) (hwmr->pbl_table[i + pbl_offset].pa & 0xffffffff);
		cmd->pbl[i].hi =
		    upper_32_bits(hwmr->pbl_table[i + pbl_offset].pa);
	}
	status = ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);

	kfree(cmd);
	return status;
}

int ocrdma_reg_mr(struct ocrdma_dev *dev,
		  struct ocrdma_hw_mr *hwmr, u32 pdid, int acc)
{
	int status;
	u32 last = 0;
	u32 cur_pbl_cnt, pbl_offset;
	u32 pending_pbl_cnt = hwmr->num_pbls;

	pbl_offset = 0;
	cur_pbl_cnt = min(pending_pbl_cnt, MAX_OCRDMA_NSMR_PBL);
	if (cur_pbl_cnt == pending_pbl_cnt)
		last = 1;

	status = ocrdma_mbx_reg_mr(dev, hwmr, pdid,
				   cur_pbl_cnt, hwmr->pbe_size, last);
	if (status) {
		pr_err("%s() status=%d\n", __func__, status);
		return status;
	}
	/* if there is no more pbls to register then exit. */
	if (last)
		return 0;

	while (!last) {
		pbl_offset += cur_pbl_cnt;
		pending_pbl_cnt -= cur_pbl_cnt;
		cur_pbl_cnt = min(pending_pbl_cnt, MAX_OCRDMA_NSMR_PBL);
		/* if we reach the end of the pbls, then need to set the last
		 * bit, indicating no more pbls to register for this memory key.
		 */
		if (cur_pbl_cnt == pending_pbl_cnt)
			last = 1;

		status = ocrdma_mbx_reg_mr_cont(dev, hwmr, cur_pbl_cnt,
						pbl_offset, last);
		if (status)
			break;
	}
	if (status)
		pr_err("%s() err. status=%d\n", __func__, status);

	return status;
}

bool ocrdma_is_qp_in_sq_flushlist(struct ocrdma_cq *cq, struct ocrdma_qp *qp)
{
	struct ocrdma_qp *tmp;
	bool found = false;
	list_for_each_entry(tmp, &cq->sq_head, sq_entry) {
		if (qp == tmp) {
			found = true;
			break;
		}
	}
	return found;
}

bool ocrdma_is_qp_in_rq_flushlist(struct ocrdma_cq *cq, struct ocrdma_qp *qp)
{
	struct ocrdma_qp *tmp;
	bool found = false;
	list_for_each_entry(tmp, &cq->rq_head, rq_entry) {
		if (qp == tmp) {
			found = true;
			break;
		}
	}
	return found;
}

void ocrdma_flush_qp(struct ocrdma_qp *qp)
{
	bool found;
	unsigned long flags;
	struct ocrdma_dev *dev = get_ocrdma_dev(qp->ibqp.device);

	spin_lock_irqsave(&dev->flush_q_lock, flags);
	found = ocrdma_is_qp_in_sq_flushlist(qp->sq_cq, qp);
	if (!found)
		list_add_tail(&qp->sq_entry, &qp->sq_cq->sq_head);
	if (!qp->srq) {
		found = ocrdma_is_qp_in_rq_flushlist(qp->rq_cq, qp);
		if (!found)
			list_add_tail(&qp->rq_entry, &qp->rq_cq->rq_head);
	}
	spin_unlock_irqrestore(&dev->flush_q_lock, flags);
}

static void ocrdma_init_hwq_ptr(struct ocrdma_qp *qp)
{
	qp->sq.head = 0;
	qp->sq.tail = 0;
	qp->rq.head = 0;
	qp->rq.tail = 0;
}

int ocrdma_qp_state_change(struct ocrdma_qp *qp, enum ib_qp_state new_ib_state,
			   enum ib_qp_state *old_ib_state)
{
	unsigned long flags;
	enum ocrdma_qp_state new_state;
	new_state = get_ocrdma_qp_state(new_ib_state);

	/* sync with wqe and rqe posting */
	spin_lock_irqsave(&qp->q_lock, flags);

	if (old_ib_state)
		*old_ib_state = get_ibqp_state(qp->state);
	if (new_state == qp->state) {
		spin_unlock_irqrestore(&qp->q_lock, flags);
		return 1;
	}


	if (new_state == OCRDMA_QPS_INIT) {
		ocrdma_init_hwq_ptr(qp);
		ocrdma_del_flush_qp(qp);
	} else if (new_state == OCRDMA_QPS_ERR) {
		ocrdma_flush_qp(qp);
	}

	qp->state = new_state;

	spin_unlock_irqrestore(&qp->q_lock, flags);
	return 0;
}

static u32 ocrdma_set_create_qp_mbx_access_flags(struct ocrdma_qp *qp)
{
	u32 flags = 0;
	if (qp->cap_flags & OCRDMA_QP_INB_RD)
		flags |= OCRDMA_CREATE_QP_REQ_INB_RDEN_MASK;
	if (qp->cap_flags & OCRDMA_QP_INB_WR)
		flags |= OCRDMA_CREATE_QP_REQ_INB_WREN_MASK;
	if (qp->cap_flags & OCRDMA_QP_MW_BIND)
		flags |= OCRDMA_CREATE_QP_REQ_BIND_MEMWIN_MASK;
	if (qp->cap_flags & OCRDMA_QP_LKEY0)
		flags |= OCRDMA_CREATE_QP_REQ_ZERO_LKEYEN_MASK;
	if (qp->cap_flags & OCRDMA_QP_FAST_REG)
		flags |= OCRDMA_CREATE_QP_REQ_FMR_EN_MASK;
	return flags;
}

static int ocrdma_set_create_qp_sq_cmd(struct ocrdma_create_qp_req *cmd,
					struct ib_qp_init_attr *attrs,
					struct ocrdma_qp *qp)
{
	int status;
	u32 len, hw_pages, hw_page_size;
	dma_addr_t pa;
	struct ocrdma_pd *pd = qp->pd;
	struct ocrdma_dev *dev = get_ocrdma_dev(pd->ibpd.device);
	struct pci_dev *pdev = dev->nic_info.pdev;
	u32 max_wqe_allocated;
	u32 max_sges = attrs->cap.max_send_sge;

	/* QP1 may exceed 127 */
	max_wqe_allocated = min_t(u32, attrs->cap.max_send_wr + 1,
				dev->attr.max_wqe);

	status = ocrdma_build_q_conf(&max_wqe_allocated,
		dev->attr.wqe_size, &hw_pages, &hw_page_size);
	if (status) {
		pr_err("%s() req. max_send_wr=0x%x\n", __func__,
		       max_wqe_allocated);
		return -EINVAL;
	}
	qp->sq.max_cnt = max_wqe_allocated;
	len = (hw_pages * hw_page_size);

	qp->sq.va = dma_alloc_coherent(&pdev->dev, len, &pa, GFP_KERNEL);
	if (!qp->sq.va)
		return -EINVAL;
	qp->sq.len = len;
	qp->sq.pa = pa;
	qp->sq.entry_size = dev->attr.wqe_size;
	ocrdma_build_q_pages(&cmd->wq_addr[0], hw_pages, pa, hw_page_size);

	cmd->type_pgsz_pdn |= (ilog2(hw_page_size / OCRDMA_MIN_Q_PAGE_SIZE)
				<< OCRDMA_CREATE_QP_REQ_SQ_PAGE_SIZE_SHIFT);
	cmd->num_wq_rq_pages |= (hw_pages <<
				 OCRDMA_CREATE_QP_REQ_NUM_WQ_PAGES_SHIFT) &
	    OCRDMA_CREATE_QP_REQ_NUM_WQ_PAGES_MASK;
	cmd->max_sge_send_write |= (max_sges <<
				    OCRDMA_CREATE_QP_REQ_MAX_SGE_SEND_SHIFT) &
	    OCRDMA_CREATE_QP_REQ_MAX_SGE_SEND_MASK;
	cmd->max_sge_send_write |= (max_sges <<
				    OCRDMA_CREATE_QP_REQ_MAX_SGE_WRITE_SHIFT) &
					OCRDMA_CREATE_QP_REQ_MAX_SGE_WRITE_MASK;
	cmd->max_wqe_rqe |= (ilog2(qp->sq.max_cnt) <<
			     OCRDMA_CREATE_QP_REQ_MAX_WQE_SHIFT) &
				OCRDMA_CREATE_QP_REQ_MAX_WQE_MASK;
	cmd->wqe_rqe_size |= (dev->attr.wqe_size <<
			      OCRDMA_CREATE_QP_REQ_WQE_SIZE_SHIFT) &
				OCRDMA_CREATE_QP_REQ_WQE_SIZE_MASK;
	return 0;
}

static int ocrdma_set_create_qp_rq_cmd(struct ocrdma_create_qp_req *cmd,
					struct ib_qp_init_attr *attrs,
					struct ocrdma_qp *qp)
{
	int status;
	u32 len, hw_pages, hw_page_size;
	dma_addr_t pa = 0;
	struct ocrdma_pd *pd = qp->pd;
	struct ocrdma_dev *dev = get_ocrdma_dev(pd->ibpd.device);
	struct pci_dev *pdev = dev->nic_info.pdev;
	u32 max_rqe_allocated = attrs->cap.max_recv_wr + 1;

	status = ocrdma_build_q_conf(&max_rqe_allocated, dev->attr.rqe_size,
				     &hw_pages, &hw_page_size);
	if (status) {
		pr_err("%s() req. max_recv_wr=0x%x\n", __func__,
		       attrs->cap.max_recv_wr + 1);
		return status;
	}
	qp->rq.max_cnt = max_rqe_allocated;
	len = (hw_pages * hw_page_size);

	qp->rq.va = dma_alloc_coherent(&pdev->dev, len, &pa, GFP_KERNEL);
	if (!qp->rq.va)
		return -ENOMEM;
	qp->rq.pa = pa;
	qp->rq.len = len;
	qp->rq.entry_size = dev->attr.rqe_size;

	ocrdma_build_q_pages(&cmd->rq_addr[0], hw_pages, pa, hw_page_size);
	cmd->type_pgsz_pdn |= (ilog2(hw_page_size / OCRDMA_MIN_Q_PAGE_SIZE) <<
		OCRDMA_CREATE_QP_REQ_RQ_PAGE_SIZE_SHIFT);
	cmd->num_wq_rq_pages |=
	    (hw_pages << OCRDMA_CREATE_QP_REQ_NUM_RQ_PAGES_SHIFT) &
	    OCRDMA_CREATE_QP_REQ_NUM_RQ_PAGES_MASK;
	cmd->max_sge_recv_flags |= (attrs->cap.max_recv_sge <<
				OCRDMA_CREATE_QP_REQ_MAX_SGE_RECV_SHIFT) &
				OCRDMA_CREATE_QP_REQ_MAX_SGE_RECV_MASK;
	cmd->max_wqe_rqe |= (ilog2(qp->rq.max_cnt) <<
				OCRDMA_CREATE_QP_REQ_MAX_RQE_SHIFT) &
				OCRDMA_CREATE_QP_REQ_MAX_RQE_MASK;
	cmd->wqe_rqe_size |= (dev->attr.rqe_size <<
			OCRDMA_CREATE_QP_REQ_RQE_SIZE_SHIFT) &
			OCRDMA_CREATE_QP_REQ_RQE_SIZE_MASK;
	return 0;
}

static void ocrdma_set_create_qp_dpp_cmd(struct ocrdma_create_qp_req *cmd,
					 struct ocrdma_pd *pd,
					 struct ocrdma_qp *qp,
					 u8 enable_dpp_cq, u16 dpp_cq_id)
{
	pd->num_dpp_qp--;
	qp->dpp_enabled = true;
	cmd->max_sge_recv_flags |= OCRDMA_CREATE_QP_REQ_ENABLE_DPP_MASK;
	if (!enable_dpp_cq)
		return;
	cmd->max_sge_recv_flags |= OCRDMA_CREATE_QP_REQ_ENABLE_DPP_MASK;
	cmd->dpp_credits_cqid = dpp_cq_id;
	cmd->dpp_credits_cqid |= OCRDMA_CREATE_QP_REQ_DPP_CREDIT_LIMIT <<
					OCRDMA_CREATE_QP_REQ_DPP_CREDIT_SHIFT;
}

static int ocrdma_set_create_qp_ird_cmd(struct ocrdma_create_qp_req *cmd,
					struct ocrdma_qp *qp)
{
	struct ocrdma_pd *pd = qp->pd;
	struct ocrdma_dev *dev = get_ocrdma_dev(pd->ibpd.device);
	struct pci_dev *pdev = dev->nic_info.pdev;
	dma_addr_t pa = 0;
	int ird_page_size = dev->attr.ird_page_size;
	int ird_q_len = dev->attr.num_ird_pages * ird_page_size;
	struct ocrdma_hdr_wqe *rqe;
	int i  = 0;

	if (dev->attr.ird == 0)
		return 0;

	qp->ird_q_va = dma_alloc_coherent(&pdev->dev, ird_q_len, &pa,
					  GFP_KERNEL);
	if (!qp->ird_q_va)
		return -ENOMEM;
	ocrdma_build_q_pages(&cmd->ird_addr[0], dev->attr.num_ird_pages,
			     pa, ird_page_size);
	for (; i < ird_q_len / dev->attr.rqe_size; i++) {
		rqe = (struct ocrdma_hdr_wqe *)(qp->ird_q_va +
			(i * dev->attr.rqe_size));
		rqe->cw = 0;
		rqe->cw |= 2;
		rqe->cw |= (OCRDMA_TYPE_LKEY << OCRDMA_WQE_TYPE_SHIFT);
		rqe->cw |= (8 << OCRDMA_WQE_SIZE_SHIFT);
		rqe->cw |= (8 << OCRDMA_WQE_NXT_WQE_SIZE_SHIFT);
	}
	return 0;
}

static void ocrdma_get_create_qp_rsp(struct ocrdma_create_qp_rsp *rsp,
				     struct ocrdma_qp *qp,
				     struct ib_qp_init_attr *attrs,
				     u16 *dpp_offset, u16 *dpp_credit_lmt)
{
	u32 max_wqe_allocated, max_rqe_allocated;
	qp->id = rsp->qp_id & OCRDMA_CREATE_QP_RSP_QP_ID_MASK;
	qp->rq.dbid = rsp->sq_rq_id & OCRDMA_CREATE_QP_RSP_RQ_ID_MASK;
	qp->sq.dbid = rsp->sq_rq_id >> OCRDMA_CREATE_QP_RSP_SQ_ID_SHIFT;
	qp->max_ird = rsp->max_ord_ird & OCRDMA_CREATE_QP_RSP_MAX_IRD_MASK;
	qp->max_ord = (rsp->max_ord_ird >> OCRDMA_CREATE_QP_RSP_MAX_ORD_SHIFT);
	qp->dpp_enabled = false;
	if (rsp->dpp_response & OCRDMA_CREATE_QP_RSP_DPP_ENABLED_MASK) {
		qp->dpp_enabled = true;
		*dpp_credit_lmt = (rsp->dpp_response &
				OCRDMA_CREATE_QP_RSP_DPP_CREDITS_MASK) >>
				OCRDMA_CREATE_QP_RSP_DPP_CREDITS_SHIFT;
		*dpp_offset = (rsp->dpp_response &
				OCRDMA_CREATE_QP_RSP_DPP_PAGE_OFFSET_MASK) >>
				OCRDMA_CREATE_QP_RSP_DPP_PAGE_OFFSET_SHIFT;
	}
	max_wqe_allocated =
		rsp->max_wqe_rqe >> OCRDMA_CREATE_QP_RSP_MAX_WQE_SHIFT;
	max_wqe_allocated = 1 << max_wqe_allocated;
	max_rqe_allocated = 1 << ((u16)rsp->max_wqe_rqe);

	qp->sq.max_cnt = max_wqe_allocated;
	qp->sq.max_wqe_idx = max_wqe_allocated - 1;

	if (!attrs->srq) {
		qp->rq.max_cnt = max_rqe_allocated;
		qp->rq.max_wqe_idx = max_rqe_allocated - 1;
	}
}

int ocrdma_mbx_create_qp(struct ocrdma_qp *qp, struct ib_qp_init_attr *attrs,
			 u8 enable_dpp_cq, u16 dpp_cq_id, u16 *dpp_offset,
			 u16 *dpp_credit_lmt)
{
	int status = -ENOMEM;
	u32 flags = 0;
	struct ocrdma_pd *pd = qp->pd;
	struct ocrdma_dev *dev = get_ocrdma_dev(pd->ibpd.device);
	struct pci_dev *pdev = dev->nic_info.pdev;
	struct ocrdma_cq *cq;
	struct ocrdma_create_qp_req *cmd;
	struct ocrdma_create_qp_rsp *rsp;
	int qptype;

	switch (attrs->qp_type) {
	case IB_QPT_GSI:
		qptype = OCRDMA_QPT_GSI;
		break;
	case IB_QPT_RC:
		qptype = OCRDMA_QPT_RC;
		break;
	case IB_QPT_UD:
		qptype = OCRDMA_QPT_UD;
		break;
	default:
		return -EINVAL;
	}

	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_CREATE_QP, sizeof(*cmd));
	if (!cmd)
		return status;
	cmd->type_pgsz_pdn |= (qptype << OCRDMA_CREATE_QP_REQ_QPT_SHIFT) &
						OCRDMA_CREATE_QP_REQ_QPT_MASK;
	status = ocrdma_set_create_qp_sq_cmd(cmd, attrs, qp);
	if (status)
		goto sq_err;

	if (attrs->srq) {
		struct ocrdma_srq *srq = get_ocrdma_srq(attrs->srq);
		cmd->max_sge_recv_flags |= OCRDMA_CREATE_QP_REQ_USE_SRQ_MASK;
		cmd->rq_addr[0].lo = srq->id;
		qp->srq = srq;
	} else {
		status = ocrdma_set_create_qp_rq_cmd(cmd, attrs, qp);
		if (status)
			goto rq_err;
	}

	status = ocrdma_set_create_qp_ird_cmd(cmd, qp);
	if (status)
		goto mbx_err;

	cmd->type_pgsz_pdn |= (pd->id << OCRDMA_CREATE_QP_REQ_PD_ID_SHIFT) &
				OCRDMA_CREATE_QP_REQ_PD_ID_MASK;

	flags = ocrdma_set_create_qp_mbx_access_flags(qp);

	cmd->max_sge_recv_flags |= flags;
	cmd->max_ord_ird |= (dev->attr.max_ord_per_qp <<
			     OCRDMA_CREATE_QP_REQ_MAX_ORD_SHIFT) &
				OCRDMA_CREATE_QP_REQ_MAX_ORD_MASK;
	cmd->max_ord_ird |= (dev->attr.max_ird_per_qp <<
			     OCRDMA_CREATE_QP_REQ_MAX_IRD_SHIFT) &
				OCRDMA_CREATE_QP_REQ_MAX_IRD_MASK;
	cq = get_ocrdma_cq(attrs->send_cq);
	cmd->wq_rq_cqid |= (cq->id << OCRDMA_CREATE_QP_REQ_WQ_CQID_SHIFT) &
				OCRDMA_CREATE_QP_REQ_WQ_CQID_MASK;
	qp->sq_cq = cq;
	cq = get_ocrdma_cq(attrs->recv_cq);
	cmd->wq_rq_cqid |= (cq->id << OCRDMA_CREATE_QP_REQ_RQ_CQID_SHIFT) &
				OCRDMA_CREATE_QP_REQ_RQ_CQID_MASK;
	qp->rq_cq = cq;

	if (pd->dpp_enabled && attrs->cap.max_inline_data && pd->num_dpp_qp &&
	    (attrs->cap.max_inline_data <= dev->attr.max_inline_data)) {
		ocrdma_set_create_qp_dpp_cmd(cmd, pd, qp, enable_dpp_cq,
					     dpp_cq_id);
	}

	status = ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);
	if (status)
		goto mbx_err;
	rsp = (struct ocrdma_create_qp_rsp *)cmd;
	ocrdma_get_create_qp_rsp(rsp, qp, attrs, dpp_offset, dpp_credit_lmt);
	qp->state = OCRDMA_QPS_RST;
	kfree(cmd);
	return 0;
mbx_err:
	if (qp->rq.va)
		dma_free_coherent(&pdev->dev, qp->rq.len, qp->rq.va, qp->rq.pa);
rq_err:
	pr_err("%s(%d) rq_err\n", __func__, dev->id);
	dma_free_coherent(&pdev->dev, qp->sq.len, qp->sq.va, qp->sq.pa);
sq_err:
	pr_err("%s(%d) sq_err\n", __func__, dev->id);
	kfree(cmd);
	return status;
}

int ocrdma_mbx_query_qp(struct ocrdma_dev *dev, struct ocrdma_qp *qp,
			struct ocrdma_qp_params *param)
{
	int status = -ENOMEM;
	struct ocrdma_query_qp *cmd;
	struct ocrdma_query_qp_rsp *rsp;

	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_QUERY_QP, sizeof(*rsp));
	if (!cmd)
		return status;
	cmd->qp_id = qp->id;
	status = ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);
	if (status)
		goto mbx_err;
	rsp = (struct ocrdma_query_qp_rsp *)cmd;
	memcpy(param, &rsp->params, sizeof(struct ocrdma_qp_params));
mbx_err:
	kfree(cmd);
	return status;
}

static int ocrdma_set_av_params(struct ocrdma_qp *qp,
				struct ocrdma_modify_qp *cmd,
				struct ib_qp_attr *attrs,
				int attr_mask)
{
	int status;
	struct rdma_ah_attr *ah_attr = &attrs->ah_attr;
	const struct ib_gid_attr *sgid_attr;
	u16 vlan_id = 0xFFFF;
	u8 mac_addr[6], hdr_type;
	union {
		struct sockaddr_in  _sockaddr_in;
		struct sockaddr_in6 _sockaddr_in6;
	} sgid_addr, dgid_addr;
	struct ocrdma_dev *dev = get_ocrdma_dev(qp->ibqp.device);
	const struct ib_global_route *grh;

	if ((rdma_ah_get_ah_flags(ah_attr) & IB_AH_GRH) == 0)
		return -EINVAL;
	grh = rdma_ah_read_grh(ah_attr);
	if (atomic_cmpxchg(&dev->update_sl, 1, 0))
		ocrdma_init_service_level(dev);
	cmd->params.tclass_sq_psn |=
	    (grh->traffic_class << OCRDMA_QP_PARAMS_TCLASS_SHIFT);
	cmd->params.rnt_rc_sl_fl |=
	    (grh->flow_label & OCRDMA_QP_PARAMS_FLOW_LABEL_MASK);
	cmd->params.rnt_rc_sl_fl |= (rdma_ah_get_sl(ah_attr) <<
				     OCRDMA_QP_PARAMS_SL_SHIFT);
	cmd->params.hop_lmt_rq_psn |=
	    (grh->hop_limit << OCRDMA_QP_PARAMS_HOP_LMT_SHIFT);
	cmd->flags |= OCRDMA_QP_PARA_FLOW_LBL_VALID;

	/* GIDs */
	memcpy(&cmd->params.dgid[0], &grh->dgid.raw[0],
	       sizeof(cmd->params.dgid));

	sgid_attr = ah_attr->grh.sgid_attr;
	status = rdma_read_gid_l2_fields(sgid_attr, &vlan_id, &mac_addr[0]);
	if (status)
		return status;

	qp->sgid_idx = grh->sgid_index;
	memcpy(&cmd->params.sgid[0], &sgid_attr->gid.raw[0],
	       sizeof(cmd->params.sgid));
	status = ocrdma_resolve_dmac(dev, ah_attr, &mac_addr[0]);
	if (status)
		return status;

	cmd->params.dmac_b0_to_b3 = mac_addr[0] | (mac_addr[1] << 8) |
				(mac_addr[2] << 16) | (mac_addr[3] << 24);

	hdr_type = rdma_gid_attr_network_type(sgid_attr);
	if (hdr_type == RDMA_NETWORK_IPV4) {
		rdma_gid2ip((struct sockaddr *)&sgid_addr, &sgid_attr->gid);
		rdma_gid2ip((struct sockaddr *)&dgid_addr, &grh->dgid);
		memcpy(&cmd->params.dgid[0],
		       &dgid_addr._sockaddr_in.sin_addr.s_addr, 4);
		memcpy(&cmd->params.sgid[0],
		       &sgid_addr._sockaddr_in.sin_addr.s_addr, 4);
	}
	/* convert them to LE format. */
	ocrdma_cpu_to_le32(&cmd->params.dgid[0], sizeof(cmd->params.dgid));
	ocrdma_cpu_to_le32(&cmd->params.sgid[0], sizeof(cmd->params.sgid));
	cmd->params.vlan_dmac_b4_to_b5 = mac_addr[4] | (mac_addr[5] << 8);

	if (vlan_id == 0xFFFF)
		vlan_id = 0;
	if (vlan_id || dev->pfc_state) {
		if (!vlan_id) {
			pr_err("ocrdma%d:Using VLAN with PFC is recommended\n",
			       dev->id);
			pr_err("ocrdma%d:Using VLAN 0 for this connection\n",
			       dev->id);
		}
		cmd->params.vlan_dmac_b4_to_b5 |=
		    vlan_id << OCRDMA_QP_PARAMS_VLAN_SHIFT;
		cmd->flags |= OCRDMA_QP_PARA_VLAN_EN_VALID;
		cmd->params.rnt_rc_sl_fl |=
			(dev->sl & 0x07) << OCRDMA_QP_PARAMS_SL_SHIFT;
	}
	cmd->params.max_sge_recv_flags |= ((hdr_type <<
					OCRDMA_QP_PARAMS_FLAGS_L3_TYPE_SHIFT) &
					OCRDMA_QP_PARAMS_FLAGS_L3_TYPE_MASK);
	return 0;
}

static int ocrdma_set_qp_params(struct ocrdma_qp *qp,
				struct ocrdma_modify_qp *cmd,
				struct ib_qp_attr *attrs, int attr_mask)
{
	int status = 0;
	struct ocrdma_dev *dev = get_ocrdma_dev(qp->ibqp.device);

	if (attr_mask & IB_QP_PKEY_INDEX) {
		cmd->params.path_mtu_pkey_indx |= (attrs->pkey_index &
					    OCRDMA_QP_PARAMS_PKEY_INDEX_MASK);
		cmd->flags |= OCRDMA_QP_PARA_PKEY_VALID;
	}
	if (attr_mask & IB_QP_QKEY) {
		qp->qkey = attrs->qkey;
		cmd->params.qkey = attrs->qkey;
		cmd->flags |= OCRDMA_QP_PARA_QKEY_VALID;
	}
	if (attr_mask & IB_QP_AV) {
		status = ocrdma_set_av_params(qp, cmd, attrs, attr_mask);
		if (status)
			return status;
	} else if (qp->qp_type == IB_QPT_GSI || qp->qp_type == IB_QPT_UD) {
		/* set the default mac address for UD, GSI QPs */
		cmd->params.dmac_b0_to_b3 = dev->nic_info.mac_addr[0] |
			(dev->nic_info.mac_addr[1] << 8) |
			(dev->nic_info.mac_addr[2] << 16) |
			(dev->nic_info.mac_addr[3] << 24);
		cmd->params.vlan_dmac_b4_to_b5 = dev->nic_info.mac_addr[4] |
					(dev->nic_info.mac_addr[5] << 8);
	}
	if ((attr_mask & IB_QP_EN_SQD_ASYNC_NOTIFY) &&
	    attrs->en_sqd_async_notify) {
		cmd->params.max_sge_recv_flags |=
			OCRDMA_QP_PARAMS_FLAGS_SQD_ASYNC;
		cmd->flags |= OCRDMA_QP_PARA_DST_QPN_VALID;
	}
	if (attr_mask & IB_QP_DEST_QPN) {
		cmd->params.ack_to_rnr_rtc_dest_qpn |= (attrs->dest_qp_num &
				OCRDMA_QP_PARAMS_DEST_QPN_MASK);
		cmd->flags |= OCRDMA_QP_PARA_DST_QPN_VALID;
	}
	if (attr_mask & IB_QP_PATH_MTU) {
		if (attrs->path_mtu < IB_MTU_512 ||
		    attrs->path_mtu > IB_MTU_4096) {
			pr_err("ocrdma%d: IB MTU %d is not supported\n",
			       dev->id, ib_mtu_enum_to_int(attrs->path_mtu));
			status = -EINVAL;
			goto pmtu_err;
		}
		cmd->params.path_mtu_pkey_indx |=
		    (ib_mtu_enum_to_int(attrs->path_mtu) <<
		     OCRDMA_QP_PARAMS_PATH_MTU_SHIFT) &
		    OCRDMA_QP_PARAMS_PATH_MTU_MASK;
		cmd->flags |= OCRDMA_QP_PARA_PMTU_VALID;
	}
	if (attr_mask & IB_QP_TIMEOUT) {
		cmd->params.ack_to_rnr_rtc_dest_qpn |= attrs->timeout <<
		    OCRDMA_QP_PARAMS_ACK_TIMEOUT_SHIFT;
		cmd->flags |= OCRDMA_QP_PARA_ACK_TO_VALID;
	}
	if (attr_mask & IB_QP_RETRY_CNT) {
		cmd->params.rnt_rc_sl_fl |= (attrs->retry_cnt <<
				      OCRDMA_QP_PARAMS_RETRY_CNT_SHIFT) &
		    OCRDMA_QP_PARAMS_RETRY_CNT_MASK;
		cmd->flags |= OCRDMA_QP_PARA_RETRY_CNT_VALID;
	}
	if (attr_mask & IB_QP_MIN_RNR_TIMER) {
		cmd->params.rnt_rc_sl_fl |= (attrs->min_rnr_timer <<
				      OCRDMA_QP_PARAMS_RNR_NAK_TIMER_SHIFT) &
		    OCRDMA_QP_PARAMS_RNR_NAK_TIMER_MASK;
		cmd->flags |= OCRDMA_QP_PARA_RNT_VALID;
	}
	if (attr_mask & IB_QP_RNR_RETRY) {
		cmd->params.ack_to_rnr_rtc_dest_qpn |= (attrs->rnr_retry <<
			OCRDMA_QP_PARAMS_RNR_RETRY_CNT_SHIFT)
			& OCRDMA_QP_PARAMS_RNR_RETRY_CNT_MASK;
		cmd->flags |= OCRDMA_QP_PARA_RRC_VALID;
	}
	if (attr_mask & IB_QP_SQ_PSN) {
		cmd->params.tclass_sq_psn |= (attrs->sq_psn & 0x00ffffff);
		cmd->flags |= OCRDMA_QP_PARA_SQPSN_VALID;
	}
	if (attr_mask & IB_QP_RQ_PSN) {
		cmd->params.hop_lmt_rq_psn |= (attrs->rq_psn & 0x00ffffff);
		cmd->flags |= OCRDMA_QP_PARA_RQPSN_VALID;
	}
	if (attr_mask & IB_QP_MAX_QP_RD_ATOMIC) {
		if (attrs->max_rd_atomic > dev->attr.max_ord_per_qp) {
			status = -EINVAL;
			goto pmtu_err;
		}
		qp->max_ord = attrs->max_rd_atomic;
		cmd->flags |= OCRDMA_QP_PARA_MAX_ORD_VALID;
	}
	if (attr_mask & IB_QP_MAX_DEST_RD_ATOMIC) {
		if (attrs->max_dest_rd_atomic > dev->attr.max_ird_per_qp) {
			status = -EINVAL;
			goto pmtu_err;
		}
		qp->max_ird = attrs->max_dest_rd_atomic;
		cmd->flags |= OCRDMA_QP_PARA_MAX_IRD_VALID;
	}
	cmd->params.max_ord_ird = (qp->max_ord <<
				OCRDMA_QP_PARAMS_MAX_ORD_SHIFT) |
				(qp->max_ird & OCRDMA_QP_PARAMS_MAX_IRD_MASK);
pmtu_err:
	return status;
}

int ocrdma_mbx_modify_qp(struct ocrdma_dev *dev, struct ocrdma_qp *qp,
			 struct ib_qp_attr *attrs, int attr_mask)
{
	int status = -ENOMEM;
	struct ocrdma_modify_qp *cmd;

	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_MODIFY_QP, sizeof(*cmd));
	if (!cmd)
		return status;

	cmd->params.id = qp->id;
	cmd->flags = 0;
	if (attr_mask & IB_QP_STATE) {
		cmd->params.max_sge_recv_flags |=
		    (get_ocrdma_qp_state(attrs->qp_state) <<
		     OCRDMA_QP_PARAMS_STATE_SHIFT) &
		    OCRDMA_QP_PARAMS_STATE_MASK;
		cmd->flags |= OCRDMA_QP_PARA_QPS_VALID;
	} else {
		cmd->params.max_sge_recv_flags |=
		    (qp->state << OCRDMA_QP_PARAMS_STATE_SHIFT) &
		    OCRDMA_QP_PARAMS_STATE_MASK;
	}

	status = ocrdma_set_qp_params(qp, cmd, attrs, attr_mask);
	if (status)
		goto mbx_err;
	status = ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);
	if (status)
		goto mbx_err;

mbx_err:
	kfree(cmd);
	return status;
}

int ocrdma_mbx_destroy_qp(struct ocrdma_dev *dev, struct ocrdma_qp *qp)
{
	int status = -ENOMEM;
	struct ocrdma_destroy_qp *cmd;
	struct pci_dev *pdev = dev->nic_info.pdev;

	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_DELETE_QP, sizeof(*cmd));
	if (!cmd)
		return status;
	cmd->qp_id = qp->id;
	status = ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);
	if (status)
		goto mbx_err;

mbx_err:
	kfree(cmd);
	if (qp->sq.va)
		dma_free_coherent(&pdev->dev, qp->sq.len, qp->sq.va, qp->sq.pa);
	if (!qp->srq && qp->rq.va)
		dma_free_coherent(&pdev->dev, qp->rq.len, qp->rq.va, qp->rq.pa);
	if (qp->dpp_enabled)
		qp->pd->num_dpp_qp++;
	return status;
}

int ocrdma_mbx_create_srq(struct ocrdma_dev *dev, struct ocrdma_srq *srq,
			  struct ib_srq_init_attr *srq_attr,
			  struct ocrdma_pd *pd)
{
	int status = -ENOMEM;
	int hw_pages, hw_page_size;
	int len;
	struct ocrdma_create_srq_rsp *rsp;
	struct ocrdma_create_srq *cmd;
	dma_addr_t pa;
	struct pci_dev *pdev = dev->nic_info.pdev;
	u32 max_rqe_allocated;

	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_CREATE_SRQ, sizeof(*cmd));
	if (!cmd)
		return status;

	cmd->pgsz_pdid = pd->id & OCRDMA_CREATE_SRQ_PD_ID_MASK;
	max_rqe_allocated = srq_attr->attr.max_wr + 1;
	status = ocrdma_build_q_conf(&max_rqe_allocated,
				dev->attr.rqe_size,
				&hw_pages, &hw_page_size);
	if (status) {
		pr_err("%s() req. max_wr=0x%x\n", __func__,
		       srq_attr->attr.max_wr);
		status = -EINVAL;
		goto ret;
	}
	len = hw_pages * hw_page_size;
	srq->rq.va = dma_alloc_coherent(&pdev->dev, len, &pa, GFP_KERNEL);
	if (!srq->rq.va) {
		status = -ENOMEM;
		goto ret;
	}
	ocrdma_build_q_pages(&cmd->rq_addr[0], hw_pages, pa, hw_page_size);

	srq->rq.entry_size = dev->attr.rqe_size;
	srq->rq.pa = pa;
	srq->rq.len = len;
	srq->rq.max_cnt = max_rqe_allocated;

	cmd->max_sge_rqe = ilog2(max_rqe_allocated);
	cmd->max_sge_rqe |= srq_attr->attr.max_sge <<
				OCRDMA_CREATE_SRQ_MAX_SGE_RECV_SHIFT;

	cmd->pgsz_pdid |= (ilog2(hw_page_size / OCRDMA_MIN_Q_PAGE_SIZE)
		<< OCRDMA_CREATE_SRQ_PG_SZ_SHIFT);
	cmd->pages_rqe_sz |= (dev->attr.rqe_size
		<< OCRDMA_CREATE_SRQ_RQE_SIZE_SHIFT)
		& OCRDMA_CREATE_SRQ_RQE_SIZE_MASK;
	cmd->pages_rqe_sz |= hw_pages << OCRDMA_CREATE_SRQ_NUM_RQ_PAGES_SHIFT;

	status = ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);
	if (status)
		goto mbx_err;
	rsp = (struct ocrdma_create_srq_rsp *)cmd;
	srq->id = rsp->id;
	srq->rq.dbid = rsp->id;
	max_rqe_allocated = ((rsp->max_sge_rqe_allocated &
		OCRDMA_CREATE_SRQ_RSP_MAX_RQE_ALLOCATED_MASK) >>
		OCRDMA_CREATE_SRQ_RSP_MAX_RQE_ALLOCATED_SHIFT);
	max_rqe_allocated = (1 << max_rqe_allocated);
	srq->rq.max_cnt = max_rqe_allocated;
	srq->rq.max_wqe_idx = max_rqe_allocated - 1;
	srq->rq.max_sges = (rsp->max_sge_rqe_allocated &
		OCRDMA_CREATE_SRQ_RSP_MAX_SGE_RECV_ALLOCATED_MASK) >>
		OCRDMA_CREATE_SRQ_RSP_MAX_SGE_RECV_ALLOCATED_SHIFT;
	goto ret;
mbx_err:
	dma_free_coherent(&pdev->dev, srq->rq.len, srq->rq.va, pa);
ret:
	kfree(cmd);
	return status;
}

int ocrdma_mbx_modify_srq(struct ocrdma_srq *srq, struct ib_srq_attr *srq_attr)
{
	int status = -ENOMEM;
	struct ocrdma_modify_srq *cmd;
	struct ocrdma_pd *pd = srq->pd;
	struct ocrdma_dev *dev = get_ocrdma_dev(pd->ibpd.device);

	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_MODIFY_SRQ, sizeof(*cmd));
	if (!cmd)
		return status;
	cmd->id = srq->id;
	cmd->limit_max_rqe |= srq_attr->srq_limit <<
	    OCRDMA_MODIFY_SRQ_LIMIT_SHIFT;
	status = ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);
	kfree(cmd);
	return status;
}

int ocrdma_mbx_query_srq(struct ocrdma_srq *srq, struct ib_srq_attr *srq_attr)
{
	int status = -ENOMEM;
	struct ocrdma_query_srq *cmd;
	struct ocrdma_dev *dev = get_ocrdma_dev(srq->ibsrq.device);

	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_QUERY_SRQ, sizeof(*cmd));
	if (!cmd)
		return status;
	cmd->id = srq->rq.dbid;
	status = ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);
	if (status == 0) {
		struct ocrdma_query_srq_rsp *rsp =
		    (struct ocrdma_query_srq_rsp *)cmd;
		srq_attr->max_sge =
		    rsp->srq_lmt_max_sge &
		    OCRDMA_QUERY_SRQ_RSP_MAX_SGE_RECV_MASK;
		srq_attr->max_wr =
		    rsp->max_rqe_pdid >> OCRDMA_QUERY_SRQ_RSP_MAX_RQE_SHIFT;
		srq_attr->srq_limit = rsp->srq_lmt_max_sge >>
		    OCRDMA_QUERY_SRQ_RSP_SRQ_LIMIT_SHIFT;
	}
	kfree(cmd);
	return status;
}

void ocrdma_mbx_destroy_srq(struct ocrdma_dev *dev, struct ocrdma_srq *srq)
{
	struct ocrdma_destroy_srq *cmd;
	struct pci_dev *pdev = dev->nic_info.pdev;
	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_DELETE_SRQ, sizeof(*cmd));
	if (!cmd)
		return;
	cmd->id = srq->id;
	ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);
	if (srq->rq.va)
		dma_free_coherent(&pdev->dev, srq->rq.len,
				  srq->rq.va, srq->rq.pa);
	kfree(cmd);
}

static int ocrdma_mbx_get_dcbx_config(struct ocrdma_dev *dev, u32 ptype,
				      struct ocrdma_dcbx_cfg *dcbxcfg)
{
	int status;
	dma_addr_t pa;
	struct ocrdma_mqe cmd;

	struct ocrdma_get_dcbx_cfg_req *req = NULL;
	struct ocrdma_get_dcbx_cfg_rsp *rsp = NULL;
	struct pci_dev *pdev = dev->nic_info.pdev;
	struct ocrdma_mqe_sge *mqe_sge = cmd.u.nonemb_req.sge;

	memset(&cmd, 0, sizeof(struct ocrdma_mqe));
	cmd.hdr.pyld_len = max_t (u32, sizeof(struct ocrdma_get_dcbx_cfg_rsp),
					sizeof(struct ocrdma_get_dcbx_cfg_req));
	req = dma_alloc_coherent(&pdev->dev, cmd.hdr.pyld_len, &pa, GFP_KERNEL);
	if (!req) {
		status = -ENOMEM;
		goto mem_err;
	}

	cmd.hdr.spcl_sge_cnt_emb |= (1 << OCRDMA_MQE_HDR_SGE_CNT_SHIFT) &
					OCRDMA_MQE_HDR_SGE_CNT_MASK;
	mqe_sge->pa_lo = (u32) (pa & 0xFFFFFFFFUL);
	mqe_sge->pa_hi = (u32) upper_32_bits(pa);
	mqe_sge->len = cmd.hdr.pyld_len;

	ocrdma_init_mch(&req->hdr, OCRDMA_CMD_GET_DCBX_CONFIG,
			OCRDMA_SUBSYS_DCBX, cmd.hdr.pyld_len);
	req->param_type = ptype;

	status = ocrdma_mbx_cmd(dev, &cmd);
	if (status)
		goto mbx_err;

	rsp = (struct ocrdma_get_dcbx_cfg_rsp *)req;
	ocrdma_le32_to_cpu(rsp, sizeof(struct ocrdma_get_dcbx_cfg_rsp));
	memcpy(dcbxcfg, &rsp->cfg, sizeof(struct ocrdma_dcbx_cfg));

mbx_err:
	dma_free_coherent(&pdev->dev, cmd.hdr.pyld_len, req, pa);
mem_err:
	return status;
}

#define OCRDMA_MAX_SERVICE_LEVEL_INDEX	0x08
#define OCRDMA_DEFAULT_SERVICE_LEVEL	0x05

static int ocrdma_parse_dcbxcfg_rsp(struct ocrdma_dev *dev, int ptype,
				    struct ocrdma_dcbx_cfg *dcbxcfg,
				    u8 *srvc_lvl)
{
	int status = -EINVAL, indx, slindx;
	int ventry_cnt;
	struct ocrdma_app_parameter *app_param;
	u8 valid, proto_sel;
	u8 app_prio, pfc_prio;
	u16 proto;

	if (!(dcbxcfg->tcv_aev_opv_st & OCRDMA_DCBX_STATE_MASK)) {
		pr_info("%s ocrdma%d DCBX is disabled\n",
			dev_name(&dev->nic_info.pdev->dev), dev->id);
		goto out;
	}

	if (!ocrdma_is_enabled_and_synced(dcbxcfg->pfc_state)) {
		pr_info("%s ocrdma%d priority flow control(%s) is %s%s\n",
			dev_name(&dev->nic_info.pdev->dev), dev->id,
			(ptype > 0 ? "operational" : "admin"),
			(dcbxcfg->pfc_state & OCRDMA_STATE_FLAG_ENABLED) ?
			"enabled" : "disabled",
			(dcbxcfg->pfc_state & OCRDMA_STATE_FLAG_SYNC) ?
			"" : ", not sync'ed");
		goto out;
	} else {
		pr_info("%s ocrdma%d priority flow control is enabled and sync'ed\n",
			dev_name(&dev->nic_info.pdev->dev), dev->id);
	}

	ventry_cnt = (dcbxcfg->tcv_aev_opv_st >>
				OCRDMA_DCBX_APP_ENTRY_SHIFT)
				& OCRDMA_DCBX_STATE_MASK;

	for (indx = 0; indx < ventry_cnt; indx++) {
		app_param = &dcbxcfg->app_param[indx];
		valid = (app_param->valid_proto_app >>
				OCRDMA_APP_PARAM_VALID_SHIFT)
				& OCRDMA_APP_PARAM_VALID_MASK;
		proto_sel = (app_param->valid_proto_app
				>>  OCRDMA_APP_PARAM_PROTO_SEL_SHIFT)
				& OCRDMA_APP_PARAM_PROTO_SEL_MASK;
		proto = app_param->valid_proto_app &
				OCRDMA_APP_PARAM_APP_PROTO_MASK;

		if (
			valid && proto == ETH_P_IBOE &&
			proto_sel == OCRDMA_PROTO_SELECT_L2) {
			for (slindx = 0; slindx <
				OCRDMA_MAX_SERVICE_LEVEL_INDEX; slindx++) {
				app_prio = ocrdma_get_app_prio(
						(u8 *)app_param->app_prio,
						slindx);
				pfc_prio = ocrdma_get_pfc_prio(
						(u8 *)dcbxcfg->pfc_prio,
						slindx);

				if (app_prio && pfc_prio) {
					*srvc_lvl = slindx;
					status = 0;
					goto out;
				}
			}
			if (slindx == OCRDMA_MAX_SERVICE_LEVEL_INDEX) {
				pr_info("%s ocrdma%d application priority not set for 0x%x protocol\n",
					dev_name(&dev->nic_info.pdev->dev),
					dev->id, proto);
			}
		}
	}

out:
	return status;
}

void ocrdma_init_service_level(struct ocrdma_dev *dev)
{
	int status = 0, indx;
	struct ocrdma_dcbx_cfg dcbxcfg;
	u8 srvc_lvl = OCRDMA_DEFAULT_SERVICE_LEVEL;
	int ptype = OCRDMA_PARAMETER_TYPE_OPER;

	for (indx = 0; indx < 2; indx++) {
		status = ocrdma_mbx_get_dcbx_config(dev, ptype, &dcbxcfg);
		if (status) {
			pr_err("%s(): status=%d\n", __func__, status);
			ptype = OCRDMA_PARAMETER_TYPE_ADMIN;
			continue;
		}

		status = ocrdma_parse_dcbxcfg_rsp(dev, ptype,
						  &dcbxcfg, &srvc_lvl);
		if (status) {
			ptype = OCRDMA_PARAMETER_TYPE_ADMIN;
			continue;
		}

		break;
	}

	if (status)
		pr_info("%s ocrdma%d service level default\n",
			dev_name(&dev->nic_info.pdev->dev), dev->id);
	else
		pr_info("%s ocrdma%d service level %d\n",
			dev_name(&dev->nic_info.pdev->dev), dev->id,
			srvc_lvl);

	dev->pfc_state = ocrdma_is_enabled_and_synced(dcbxcfg.pfc_state);
	dev->sl = srvc_lvl;
}

int ocrdma_alloc_av(struct ocrdma_dev *dev, struct ocrdma_ah *ah)
{
	int i;
	int status = -EINVAL;
	struct ocrdma_av *av;
	unsigned long flags;

	av = dev->av_tbl.va;
	spin_lock_irqsave(&dev->av_tbl.lock, flags);
	for (i = 0; i < dev->av_tbl.num_ah; i++) {
		if (av->valid == 0) {
			av->valid = OCRDMA_AV_VALID;
			ah->av = av;
			ah->id = i;
			status = 0;
			break;
		}
		av++;
	}
	if (i == dev->av_tbl.num_ah)
		status = -EAGAIN;
	spin_unlock_irqrestore(&dev->av_tbl.lock, flags);
	return status;
}

void ocrdma_free_av(struct ocrdma_dev *dev, struct ocrdma_ah *ah)
{
	unsigned long flags;
	spin_lock_irqsave(&dev->av_tbl.lock, flags);
	ah->av->valid = 0;
	spin_unlock_irqrestore(&dev->av_tbl.lock, flags);
}

static int ocrdma_create_eqs(struct ocrdma_dev *dev)
{
	int num_eq, i, status = 0;
	int irq;
	unsigned long flags = 0;

	num_eq = dev->nic_info.msix.num_vectors -
			dev->nic_info.msix.start_vector;
	if (dev->nic_info.intr_mode == BE_INTERRUPT_MODE_INTX) {
		num_eq = 1;
		flags = IRQF_SHARED;
	} else {
		num_eq = min_t(u32, num_eq, num_online_cpus());
	}

	if (!num_eq)
		return -EINVAL;

	dev->eq_tbl = kcalloc(num_eq, sizeof(struct ocrdma_eq), GFP_KERNEL);
	if (!dev->eq_tbl)
		return -ENOMEM;

	for (i = 0; i < num_eq; i++) {
		status = ocrdma_create_eq(dev, &dev->eq_tbl[i],
					OCRDMA_EQ_LEN);
		if (status) {
			status = -EINVAL;
			break;
		}
		sprintf(dev->eq_tbl[i].irq_name, "ocrdma%d-%d",
			dev->id, i);
		irq = ocrdma_get_irq(dev, &dev->eq_tbl[i]);
		status = request_irq(irq, ocrdma_irq_handler, flags,
				     dev->eq_tbl[i].irq_name,
				     &dev->eq_tbl[i]);
		if (status)
			goto done;
		dev->eq_cnt += 1;
	}
	/* one eq is sufficient for data path to work */
	return 0;
done:
	ocrdma_destroy_eqs(dev);
	return status;
}

static int ocrdma_mbx_modify_eqd(struct ocrdma_dev *dev, struct ocrdma_eq *eq,
				 int num)
{
	int i, status;
	struct ocrdma_modify_eqd_req *cmd;

	cmd = ocrdma_init_emb_mqe(OCRDMA_CMD_MODIFY_EQ_DELAY, sizeof(*cmd));
	if (!cmd)
		return -ENOMEM;

	ocrdma_init_mch(&cmd->cmd.req, OCRDMA_CMD_MODIFY_EQ_DELAY,
			OCRDMA_SUBSYS_COMMON, sizeof(*cmd));

	cmd->cmd.num_eq = num;
	for (i = 0; i < num; i++) {
		cmd->cmd.set_eqd[i].eq_id = eq[i].q.id;
		cmd->cmd.set_eqd[i].phase = 0;
		cmd->cmd.set_eqd[i].delay_multiplier =
				(eq[i].aic_obj.prev_eqd * 65)/100;
	}
	status = ocrdma_mbx_cmd(dev, (struct ocrdma_mqe *)cmd);

	kfree(cmd);
	return status;
}

static int ocrdma_modify_eqd(struct ocrdma_dev *dev, struct ocrdma_eq *eq,
			     int num)
{
	int num_eqs, i = 0;
	if (num > 8) {
		while (num) {
			num_eqs = min(num, 8);
			ocrdma_mbx_modify_eqd(dev, &eq[i], num_eqs);
			i += num_eqs;
			num -= num_eqs;
		}
	} else {
		ocrdma_mbx_modify_eqd(dev, eq, num);
	}
	return 0;
}

void ocrdma_eqd_set_task(struct work_struct *work)
{
	struct ocrdma_dev *dev =
		container_of(work, struct ocrdma_dev, eqd_work.work);
	struct ocrdma_eq *eq = NULL;
	int i, num = 0;
	u64 eq_intr;

	for (i = 0; i < dev->eq_cnt; i++) {
		eq = &dev->eq_tbl[i];
		if (eq->aic_obj.eq_intr_cnt > eq->aic_obj.prev_eq_intr_cnt) {
			eq_intr = eq->aic_obj.eq_intr_cnt -
				  eq->aic_obj.prev_eq_intr_cnt;
			if ((eq_intr > EQ_INTR_PER_SEC_THRSH_HI) &&
			    (eq->aic_obj.prev_eqd == EQ_AIC_MIN_EQD)) {
				eq->aic_obj.prev_eqd = EQ_AIC_MAX_EQD;
				num++;
			} else if ((eq_intr < EQ_INTR_PER_SEC_THRSH_LOW) &&
				   (eq->aic_obj.prev_eqd == EQ_AIC_MAX_EQD)) {
				eq->aic_obj.prev_eqd = EQ_AIC_MIN_EQD;
				num++;
			}
		}
		eq->aic_obj.prev_eq_intr_cnt = eq->aic_obj.eq_intr_cnt;
	}

	if (num)
		ocrdma_modify_eqd(dev, &dev->eq_tbl[0], num);
	schedule_delayed_work(&dev->eqd_work, msecs_to_jiffies(1000));
}

int ocrdma_init_hw(struct ocrdma_dev *dev)
{
	int status;

	/* create the eqs  */
	status = ocrdma_create_eqs(dev);
	if (status)
		goto qpeq_err;
	status = ocrdma_create_mq(dev);
	if (status)
		goto mq_err;
	status = ocrdma_mbx_query_fw_config(dev);
	if (status)
		goto conf_err;
	status = ocrdma_mbx_query_dev(dev);
	if (status)
		goto conf_err;
	status = ocrdma_mbx_query_fw_ver(dev);
	if (status)
		goto conf_err;
	status = ocrdma_mbx_create_ah_tbl(dev);
	if (status)
		goto conf_err;
	status = ocrdma_mbx_get_phy_info(dev);
	if (status)
		goto info_attrb_err;
	status = ocrdma_mbx_get_ctrl_attribs(dev);
	if (status)
		goto info_attrb_err;

	return 0;

info_attrb_err:
	ocrdma_mbx_delete_ah_tbl(dev);
conf_err:
	ocrdma_destroy_mq(dev);
mq_err:
	ocrdma_destroy_eqs(dev);
qpeq_err:
	pr_err("%s() status=%d\n", __func__, status);
	return status;
}

void ocrdma_cleanup_hw(struct ocrdma_dev *dev)
{
	ocrdma_free_pd_pool(dev);
	ocrdma_mbx_delete_ah_tbl(dev);

	/* cleanup the control path */
	ocrdma_destroy_mq(dev);

	/* cleanup the eqs */
	ocrdma_destroy_eqs(dev);
}
