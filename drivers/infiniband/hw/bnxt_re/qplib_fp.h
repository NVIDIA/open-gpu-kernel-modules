/*
 * Broadcom NetXtreme-E RoCE driver.
 *
 * Copyright (c) 2016 - 2017, Broadcom. All rights reserved.  The term
 * Broadcom refers to Broadcom Limited and/or its subsidiaries.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * BSD license below:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Description: Fast Path Operators (header)
 */

#ifndef __BNXT_QPLIB_FP_H__
#define __BNXT_QPLIB_FP_H__

/* Few helper structures temporarily defined here
 * should get rid of these when roce_hsi.h is updated
 * in original code base
 */
struct sq_ud_ext_hdr {
	__le32 dst_qp;
	__le32 avid;
	__le64 rsvd;
};

struct sq_raw_ext_hdr {
	__le32 cfa_meta;
	__le32 rsvd0;
	__le64 rsvd1;
};

struct sq_rdma_ext_hdr {
	__le64 remote_va;
	__le32 remote_key;
	__le32 rsvd;
};

struct sq_atomic_ext_hdr {
	__le64 swap_data;
	__le64 cmp_data;
};

struct sq_fr_pmr_ext_hdr {
	__le64 pblptr;
	__le64 va;
};

struct sq_bind_ext_hdr {
	__le64 va;
	__le32 length_lo;
	__le32 length_hi;
};

struct rq_ext_hdr {
	__le64 rsvd1;
	__le64 rsvd2;
};

/* Helper structures end */

struct bnxt_qplib_srq {
	struct bnxt_qplib_pd		*pd;
	struct bnxt_qplib_dpi		*dpi;
	struct bnxt_qplib_db_info	dbinfo;
	u64				srq_handle;
	u32				id;
	u16				wqe_size;
	u32				max_wqe;
	u32				max_sge;
	u32				threshold;
	bool				arm_req;
	struct bnxt_qplib_cq		*cq;
	struct bnxt_qplib_hwq		hwq;
	struct bnxt_qplib_swq		*swq;
	int				start_idx;
	int				last_idx;
	struct bnxt_qplib_sg_info	sg_info;
	u16				eventq_hw_ring_id;
	spinlock_t			lock; /* protect SRQE link list */
};

struct bnxt_qplib_sge {
	u64				addr;
	u32				lkey;
	u32				size;
};

#define BNXT_QPLIB_QP_MAX_SGL	6
struct bnxt_qplib_swq {
	u64				wr_id;
	int				next_idx;
	u8				type;
	u8				flags;
	u32				start_psn;
	u32				next_psn;
	u32				slot_idx;
	u8				slots;
	struct sq_psn_search		*psn_search;
	struct sq_psn_search_ext	*psn_ext;
};

struct bnxt_qplib_swqe {
	/* General */
#define	BNXT_QPLIB_FENCE_WRID	0x46454E43	/* "FENC" */
	u64				wr_id;
	u8				reqs_type;
	u8				type;
#define BNXT_QPLIB_SWQE_TYPE_SEND			0
#define BNXT_QPLIB_SWQE_TYPE_SEND_WITH_IMM		1
#define BNXT_QPLIB_SWQE_TYPE_SEND_WITH_INV		2
#define BNXT_QPLIB_SWQE_TYPE_RDMA_WRITE			4
#define BNXT_QPLIB_SWQE_TYPE_RDMA_WRITE_WITH_IMM	5
#define BNXT_QPLIB_SWQE_TYPE_RDMA_READ			6
#define BNXT_QPLIB_SWQE_TYPE_ATOMIC_CMP_AND_SWP		8
#define BNXT_QPLIB_SWQE_TYPE_ATOMIC_FETCH_AND_ADD	11
#define BNXT_QPLIB_SWQE_TYPE_LOCAL_INV			12
#define BNXT_QPLIB_SWQE_TYPE_FAST_REG_MR		13
#define BNXT_QPLIB_SWQE_TYPE_REG_MR			13
#define BNXT_QPLIB_SWQE_TYPE_BIND_MW			14
#define BNXT_QPLIB_SWQE_TYPE_RECV			128
#define BNXT_QPLIB_SWQE_TYPE_RECV_RDMA_IMM		129
	u8				flags;
#define BNXT_QPLIB_SWQE_FLAGS_SIGNAL_COMP		BIT(0)
#define BNXT_QPLIB_SWQE_FLAGS_RD_ATOMIC_FENCE		BIT(1)
#define BNXT_QPLIB_SWQE_FLAGS_UC_FENCE			BIT(2)
#define BNXT_QPLIB_SWQE_FLAGS_SOLICIT_EVENT		BIT(3)
#define BNXT_QPLIB_SWQE_FLAGS_INLINE			BIT(4)
	struct bnxt_qplib_sge		sg_list[BNXT_QPLIB_QP_MAX_SGL];
	int				num_sge;
	/* Max inline data is 96 bytes */
	u32				inline_len;
#define BNXT_QPLIB_SWQE_MAX_INLINE_LENGTH		96
	u8		inline_data[BNXT_QPLIB_SWQE_MAX_INLINE_LENGTH];

	union {
		/* Send, with imm, inval key */
		struct {
			union {
				__be32	imm_data;
				u32	inv_key;
			};
			u32		q_key;
			u32		dst_qp;
			u16		avid;
		} send;

		/* Send Raw Ethernet and QP1 */
		struct {
			u16		lflags;
			u16		cfa_action;
			u32		cfa_meta;
		} rawqp1;

		/* RDMA write, with imm, read */
		struct {
			union {
				__be32	imm_data;
				u32	inv_key;
			};
			u64		remote_va;
			u32		r_key;
		} rdma;

		/* Atomic cmp/swap, fetch/add */
		struct {
			u64		remote_va;
			u32		r_key;
			u64		swap_data;
			u64		cmp_data;
		} atomic;

		/* Local Invalidate */
		struct {
			u32		inv_l_key;
		} local_inv;

		/* FR-PMR */
		struct {
			u8		access_cntl;
			u8		pg_sz_log;
			bool		zero_based;
			u32		l_key;
			u32		length;
			u8		pbl_pg_sz_log;
#define BNXT_QPLIB_SWQE_PAGE_SIZE_4K			0
#define BNXT_QPLIB_SWQE_PAGE_SIZE_8K			1
#define BNXT_QPLIB_SWQE_PAGE_SIZE_64K			4
#define BNXT_QPLIB_SWQE_PAGE_SIZE_256K			6
#define BNXT_QPLIB_SWQE_PAGE_SIZE_1M			8
#define BNXT_QPLIB_SWQE_PAGE_SIZE_2M			9
#define BNXT_QPLIB_SWQE_PAGE_SIZE_4M			10
#define BNXT_QPLIB_SWQE_PAGE_SIZE_1G			18
			u8		levels;
#define PAGE_SHIFT_4K	12
			__le64		*pbl_ptr;
			dma_addr_t	pbl_dma_ptr;
			u64		*page_list;
			u16		page_list_len;
			u64		va;
		} frmr;

		/* Bind */
		struct {
			u8		access_cntl;
#define BNXT_QPLIB_BIND_SWQE_ACCESS_LOCAL_WRITE		BIT(0)
#define BNXT_QPLIB_BIND_SWQE_ACCESS_REMOTE_READ		BIT(1)
#define BNXT_QPLIB_BIND_SWQE_ACCESS_REMOTE_WRITE	BIT(2)
#define BNXT_QPLIB_BIND_SWQE_ACCESS_REMOTE_ATOMIC	BIT(3)
#define BNXT_QPLIB_BIND_SWQE_ACCESS_WINDOW_BIND		BIT(4)
			bool		zero_based;
			u8		mw_type;
			u32		parent_l_key;
			u32		r_key;
			u64		va;
			u32		length;
		} bind;
	};
};

struct bnxt_qplib_q {
	struct bnxt_qplib_hwq		hwq;
	struct bnxt_qplib_swq		*swq;
	struct bnxt_qplib_db_info	dbinfo;
	struct bnxt_qplib_sg_info	sg_info;
	u32				max_wqe;
	u16				wqe_size;
	u16				q_full_delta;
	u16				max_sge;
	u32				psn;
	bool				condition;
	bool				single;
	bool				send_phantom;
	u32				phantom_wqe_cnt;
	u32				phantom_cqe_cnt;
	u32				next_cq_cons;
	bool				flushed;
	u32				swq_start;
	u32				swq_last;
};

struct bnxt_qplib_qp {
	struct bnxt_qplib_pd		*pd;
	struct bnxt_qplib_dpi		*dpi;
	struct bnxt_qplib_chip_ctx	*cctx;
	u64				qp_handle;
#define BNXT_QPLIB_QP_ID_INVALID        0xFFFFFFFF
	u32				id;
	u8				type;
	u8				sig_type;
	u8				wqe_mode;
	u8				state;
	u8				cur_qp_state;
	u64				modify_flags;
	u32				max_inline_data;
	u32				mtu;
	u8				path_mtu;
	bool				en_sqd_async_notify;
	u16				pkey_index;
	u32				qkey;
	u32				dest_qp_id;
	u8				access;
	u8				timeout;
	u8				retry_cnt;
	u8				rnr_retry;
	u64				wqe_cnt;
	u32				min_rnr_timer;
	u32				max_rd_atomic;
	u32				max_dest_rd_atomic;
	u32				dest_qpn;
	u8				smac[6];
	u16				vlan_id;
	u8				nw_type;
	struct bnxt_qplib_ah		ah;

#define BTH_PSN_MASK			((1 << 24) - 1)
	/* SQ */
	struct bnxt_qplib_q		sq;
	/* RQ */
	struct bnxt_qplib_q		rq;
	/* SRQ */
	struct bnxt_qplib_srq		*srq;
	/* CQ */
	struct bnxt_qplib_cq		*scq;
	struct bnxt_qplib_cq		*rcq;
	/* IRRQ and ORRQ */
	struct bnxt_qplib_hwq		irrq;
	struct bnxt_qplib_hwq		orrq;
	/* Header buffer for QP1 */
	int				sq_hdr_buf_size;
	int				rq_hdr_buf_size;
/*
 * Buffer space for ETH(14), IP or GRH(40), UDP header(8)
 * and ib_bth + ib_deth (20).
 * Max required is 82 when RoCE V2 is enabled
 */
#define BNXT_QPLIB_MAX_QP1_SQ_HDR_SIZE_V2	86
	/* Ethernet header	=  14 */
	/* ib_grh		=  40 (provided by MAD) */
	/* ib_bth + ib_deth	=  20 */
	/* MAD			= 256 (provided by MAD) */
	/* iCRC			=   4 */
#define BNXT_QPLIB_MAX_QP1_RQ_ETH_HDR_SIZE	14
#define BNXT_QPLIB_MAX_QP1_RQ_HDR_SIZE_V2	512
#define BNXT_QPLIB_MAX_GRH_HDR_SIZE_IPV4	20
#define BNXT_QPLIB_MAX_GRH_HDR_SIZE_IPV6	40
#define BNXT_QPLIB_MAX_QP1_RQ_BDETH_HDR_SIZE	20
	void				*sq_hdr_buf;
	dma_addr_t			sq_hdr_buf_map;
	void				*rq_hdr_buf;
	dma_addr_t			rq_hdr_buf_map;
	struct list_head		sq_flush;
	struct list_head		rq_flush;
};

#define BNXT_QPLIB_MAX_CQE_ENTRY_SIZE	sizeof(struct cq_base)

#define CQE_CNT_PER_PG		(PAGE_SIZE / BNXT_QPLIB_MAX_CQE_ENTRY_SIZE)
#define CQE_MAX_IDX_PER_PG	(CQE_CNT_PER_PG - 1)
#define CQE_PG(x)		(((x) & ~CQE_MAX_IDX_PER_PG) / CQE_CNT_PER_PG)
#define CQE_IDX(x)		((x) & CQE_MAX_IDX_PER_PG)

#define ROCE_CQE_CMP_V			0
#define CQE_CMP_VALID(hdr, raw_cons, cp_bit)			\
	(!!((hdr)->cqe_type_toggle & CQ_BASE_TOGGLE) ==		\
	   !((raw_cons) & (cp_bit)))

static inline bool bnxt_qplib_queue_full(struct bnxt_qplib_q *que,
					 u8 slots)
{
	struct bnxt_qplib_hwq *hwq;
	int avail;

	hwq = &que->hwq;
	/* False full is possible, retrying post-send makes sense */
	avail = hwq->cons - hwq->prod;
	if (hwq->cons <= hwq->prod)
		avail += hwq->depth;
	return avail <= slots;
}

struct bnxt_qplib_cqe {
	u8				status;
	u8				type;
	u8				opcode;
	u32				length;
	u16				cfa_meta;
	u64				wr_id;
	union {
		__be32			immdata;
		u32			invrkey;
	};
	u64				qp_handle;
	u64				mr_handle;
	u16				flags;
	u8				smac[6];
	u32				src_qp;
	u16				raweth_qp1_flags;
	u16				raweth_qp1_errors;
	u16				raweth_qp1_cfa_code;
	u32				raweth_qp1_flags2;
	u32				raweth_qp1_metadata;
	u8				raweth_qp1_payload_offset;
	u16				pkey_index;
};

#define BNXT_QPLIB_QUEUE_START_PERIOD		0x01
struct bnxt_qplib_cq {
	struct bnxt_qplib_dpi		*dpi;
	struct bnxt_qplib_db_info	dbinfo;
	u32				max_wqe;
	u32				id;
	u16				count;
	u16				period;
	struct bnxt_qplib_hwq		hwq;
	u32				cnq_hw_ring_id;
	struct bnxt_qplib_nq		*nq;
	bool				resize_in_progress;
	struct bnxt_qplib_sg_info	sg_info;
	u64				cq_handle;

#define CQ_RESIZE_WAIT_TIME_MS		500
	unsigned long			flags;
#define CQ_FLAGS_RESIZE_IN_PROG		1
	wait_queue_head_t		waitq;
	struct list_head		sqf_head, rqf_head;
	atomic_t			arm_state;
	spinlock_t			compl_lock; /* synch CQ handlers */
/* Locking Notes:
 * QP can move to error state from modify_qp, async error event or error
 * CQE as part of poll_cq. When QP is moved to error state, it gets added
 * to two flush lists, one each for SQ and RQ.
 * Each flush list is protected by qplib_cq->flush_lock. Both scq and rcq
 * flush_locks should be acquired when QP is moved to error. The control path
 * operations(modify_qp and async error events) are synchronized with poll_cq
 * using upper level CQ locks (bnxt_re_cq->cq_lock) of both SCQ and RCQ.
 * The qplib_cq->flush_lock is required to synchronize two instances of poll_cq
 * of the same QP while manipulating the flush list.
 */
	spinlock_t			flush_lock; /* QP flush management */
	u16				cnq_events;
};

#define BNXT_QPLIB_MAX_IRRQE_ENTRY_SIZE	sizeof(struct xrrq_irrq)
#define BNXT_QPLIB_MAX_ORRQE_ENTRY_SIZE	sizeof(struct xrrq_orrq)
#define IRD_LIMIT_TO_IRRQ_SLOTS(x)	(2 * (x) + 2)
#define IRRQ_SLOTS_TO_IRD_LIMIT(s)	(((s) >> 1) - 1)
#define ORD_LIMIT_TO_ORRQ_SLOTS(x)	((x) + 1)
#define ORRQ_SLOTS_TO_ORD_LIMIT(s)	((s) - 1)

#define BNXT_QPLIB_MAX_NQE_ENTRY_SIZE	sizeof(struct nq_base)

#define NQE_CNT_PER_PG		(PAGE_SIZE / BNXT_QPLIB_MAX_NQE_ENTRY_SIZE)
#define NQE_MAX_IDX_PER_PG	(NQE_CNT_PER_PG - 1)
#define NQE_PG(x)		(((x) & ~NQE_MAX_IDX_PER_PG) / NQE_CNT_PER_PG)
#define NQE_IDX(x)		((x) & NQE_MAX_IDX_PER_PG)

#define NQE_CMP_VALID(hdr, raw_cons, cp_bit)			\
	(!!(le32_to_cpu((hdr)->info63_v[0]) & NQ_BASE_V) ==	\
	   !((raw_cons) & (cp_bit)))

#define BNXT_QPLIB_NQE_MAX_CNT		(128 * 1024)

#define NQ_CONS_PCI_BAR_REGION		2
#define NQ_DB_KEY_CP			(0x2 << CMPL_DOORBELL_KEY_SFT)
#define NQ_DB_IDX_VALID			CMPL_DOORBELL_IDX_VALID
#define NQ_DB_IRQ_DIS			CMPL_DOORBELL_MASK
#define NQ_DB_CP_FLAGS_REARM		(NQ_DB_KEY_CP |		\
					 NQ_DB_IDX_VALID)
#define NQ_DB_CP_FLAGS			(NQ_DB_KEY_CP    |	\
					 NQ_DB_IDX_VALID |	\
					 NQ_DB_IRQ_DIS)

struct bnxt_qplib_nq_db {
	struct bnxt_qplib_reg_desc	reg;
	struct bnxt_qplib_db_info	dbinfo;
};

typedef int (*cqn_handler_t)(struct bnxt_qplib_nq *nq,
		struct bnxt_qplib_cq *cq);
typedef int (*srqn_handler_t)(struct bnxt_qplib_nq *nq,
		struct bnxt_qplib_srq *srq, u8 event);

struct bnxt_qplib_nq {
	struct pci_dev			*pdev;
	struct bnxt_qplib_res		*res;
	char				name[32];
	struct bnxt_qplib_hwq		hwq;
	struct bnxt_qplib_nq_db		nq_db;
	u16				ring_id;
	int				msix_vec;
	cpumask_t			mask;
	struct tasklet_struct		nq_tasklet;
	bool				requested;
	int				budget;

	cqn_handler_t			cqn_handler;
	srqn_handler_t			srqn_handler;
	struct workqueue_struct		*cqn_wq;
};

struct bnxt_qplib_nq_work {
	struct work_struct      work;
	struct bnxt_qplib_nq    *nq;
	struct bnxt_qplib_cq    *cq;
};

void bnxt_qplib_nq_stop_irq(struct bnxt_qplib_nq *nq, bool kill);
void bnxt_qplib_disable_nq(struct bnxt_qplib_nq *nq);
int bnxt_qplib_nq_start_irq(struct bnxt_qplib_nq *nq, int nq_indx,
			    int msix_vector, bool need_init);
int bnxt_qplib_enable_nq(struct pci_dev *pdev, struct bnxt_qplib_nq *nq,
			 int nq_idx, int msix_vector, int bar_reg_offset,
			 cqn_handler_t cqn_handler,
			 srqn_handler_t srq_handler);
int bnxt_qplib_create_srq(struct bnxt_qplib_res *res,
			  struct bnxt_qplib_srq *srq);
int bnxt_qplib_modify_srq(struct bnxt_qplib_res *res,
			  struct bnxt_qplib_srq *srq);
int bnxt_qplib_query_srq(struct bnxt_qplib_res *res,
			 struct bnxt_qplib_srq *srq);
void bnxt_qplib_destroy_srq(struct bnxt_qplib_res *res,
			    struct bnxt_qplib_srq *srq);
int bnxt_qplib_post_srq_recv(struct bnxt_qplib_srq *srq,
			     struct bnxt_qplib_swqe *wqe);
int bnxt_qplib_create_qp1(struct bnxt_qplib_res *res, struct bnxt_qplib_qp *qp);
int bnxt_qplib_create_qp(struct bnxt_qplib_res *res, struct bnxt_qplib_qp *qp);
int bnxt_qplib_modify_qp(struct bnxt_qplib_res *res, struct bnxt_qplib_qp *qp);
int bnxt_qplib_query_qp(struct bnxt_qplib_res *res, struct bnxt_qplib_qp *qp);
int bnxt_qplib_destroy_qp(struct bnxt_qplib_res *res, struct bnxt_qplib_qp *qp);
void bnxt_qplib_clean_qp(struct bnxt_qplib_qp *qp);
void bnxt_qplib_free_qp_res(struct bnxt_qplib_res *res,
			    struct bnxt_qplib_qp *qp);
void *bnxt_qplib_get_qp1_sq_buf(struct bnxt_qplib_qp *qp,
				struct bnxt_qplib_sge *sge);
void *bnxt_qplib_get_qp1_rq_buf(struct bnxt_qplib_qp *qp,
				struct bnxt_qplib_sge *sge);
u32 bnxt_qplib_get_rq_prod_index(struct bnxt_qplib_qp *qp);
dma_addr_t bnxt_qplib_get_qp_buf_from_index(struct bnxt_qplib_qp *qp,
					    u32 index);
void bnxt_qplib_post_send_db(struct bnxt_qplib_qp *qp);
int bnxt_qplib_post_send(struct bnxt_qplib_qp *qp,
			 struct bnxt_qplib_swqe *wqe);
void bnxt_qplib_post_recv_db(struct bnxt_qplib_qp *qp);
int bnxt_qplib_post_recv(struct bnxt_qplib_qp *qp,
			 struct bnxt_qplib_swqe *wqe);
int bnxt_qplib_create_cq(struct bnxt_qplib_res *res, struct bnxt_qplib_cq *cq);
int bnxt_qplib_destroy_cq(struct bnxt_qplib_res *res, struct bnxt_qplib_cq *cq);
int bnxt_qplib_poll_cq(struct bnxt_qplib_cq *cq, struct bnxt_qplib_cqe *cqe,
		       int num, struct bnxt_qplib_qp **qp);
bool bnxt_qplib_is_cq_empty(struct bnxt_qplib_cq *cq);
void bnxt_qplib_req_notify_cq(struct bnxt_qplib_cq *cq, u32 arm_type);
void bnxt_qplib_free_nq(struct bnxt_qplib_nq *nq);
int bnxt_qplib_alloc_nq(struct bnxt_qplib_res *res, struct bnxt_qplib_nq *nq);
void bnxt_qplib_add_flush_qp(struct bnxt_qplib_qp *qp);
void bnxt_qplib_acquire_cq_locks(struct bnxt_qplib_qp *qp,
				 unsigned long *flags);
void bnxt_qplib_release_cq_locks(struct bnxt_qplib_qp *qp,
				 unsigned long *flags);
int bnxt_qplib_process_flush_list(struct bnxt_qplib_cq *cq,
				  struct bnxt_qplib_cqe *cqe,
				  int num_cqes);
void bnxt_qplib_flush_cqn_wq(struct bnxt_qplib_qp *qp);

static inline void *bnxt_qplib_get_swqe(struct bnxt_qplib_q *que, u32 *swq_idx)
{
	u32 idx;

	idx = que->swq_start;
	if (swq_idx)
		*swq_idx = idx;
	return &que->swq[idx];
}

static inline void bnxt_qplib_swq_mod_start(struct bnxt_qplib_q *que, u32 idx)
{
	que->swq_start = que->swq[idx].next_idx;
}

static inline u32 bnxt_qplib_get_depth(struct bnxt_qplib_q *que)
{
	return (que->wqe_size * que->max_wqe) / sizeof(struct sq_sge);
}

static inline u32 bnxt_qplib_set_sq_size(struct bnxt_qplib_q *que, u8 wqe_mode)
{
	return (wqe_mode == BNXT_QPLIB_WQE_MODE_STATIC) ?
		que->max_wqe : bnxt_qplib_get_depth(que);
}

static inline u32 bnxt_qplib_set_sq_max_slot(u8 wqe_mode)
{
	return (wqe_mode == BNXT_QPLIB_WQE_MODE_STATIC) ?
		sizeof(struct sq_send) / sizeof(struct sq_sge) : 1;
}

static inline u32 bnxt_qplib_set_rq_max_slot(u32 wqe_size)
{
	return (wqe_size / sizeof(struct sq_sge));
}

static inline u16 __xlate_qfd(u16 delta, u16 wqe_bytes)
{
	/* For Cu/Wh delta = 128, stride = 16, wqe_bytes = 128
	 * For Gen-p5 B/C mode delta = 0, stride = 16, wqe_bytes = 128.
	 * For Gen-p5 delta = 0, stride = 16, 32 <= wqe_bytes <= 512.
	 * when 8916 is disabled.
	 */
	return (delta * wqe_bytes) / sizeof(struct sq_sge);
}

static inline u16 bnxt_qplib_calc_ilsize(struct bnxt_qplib_swqe *wqe, u16 max)
{
	u16 size = 0;
	int indx;

	for (indx = 0; indx < wqe->num_sge; indx++)
		size += wqe->sg_list[indx].size;
	if (size > max)
		size = max;

	return size;
}
#endif /* __BNXT_QPLIB_FP_H__ */
