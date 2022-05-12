/*
 * Copyright (c) 2009-2010 Chelsio, Inc. All rights reserved.
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
 *	  copyright notice, this list of conditions and the following
 *	  disclaimer.
 *      - Redistributions in binary form must reproduce the above
 *	  copyright notice, this list of conditions and the following
 *	  disclaimer in the documentation and/or other materials
 *	  provided with the distribution.
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
#ifndef __IW_CXGB4_H__
#define __IW_CXGB4_H__

#include <linux/mutex.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/xarray.h>
#include <linux/completion.h>
#include <linux/netdevice.h>
#include <linux/sched/mm.h>
#include <linux/pci.h>
#include <linux/dma-mapping.h>
#include <linux/inet.h>
#include <linux/wait.h>
#include <linux/kref.h>
#include <linux/timer.h>
#include <linux/io.h>
#include <linux/workqueue.h>

#include <asm/byteorder.h>

#include <net/net_namespace.h>

#include <rdma/ib_verbs.h>
#include <rdma/iw_cm.h>
#include <rdma/rdma_netlink.h>
#include <rdma/iw_portmap.h>
#include <rdma/restrack.h>

#include "cxgb4.h"
#include "cxgb4_uld.h"
#include "l2t.h"
#include <rdma/cxgb4-abi.h>

#define DRV_NAME "iw_cxgb4"
#define MOD DRV_NAME ":"

#ifdef pr_fmt
#undef pr_fmt
#endif

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include "t4.h"

#define PBL_OFF(rdev_p, a) ((a) - (rdev_p)->lldi.vr->pbl.start)
#define RQT_OFF(rdev_p, a) ((a) - (rdev_p)->lldi.vr->rq.start)

static inline void *cplhdr(struct sk_buff *skb)
{
	return skb->data;
}

#define C4IW_ID_TABLE_F_RANDOM 1       /* Pseudo-randomize the id's returned */
#define C4IW_ID_TABLE_F_EMPTY  2       /* Table is initially empty */

struct c4iw_id_table {
	u32 flags;
	u32 start;              /* logical minimal id */
	u32 last;               /* hint for find */
	u32 max;
	spinlock_t lock;
	unsigned long *table;
};

struct c4iw_resource {
	struct c4iw_id_table tpt_table;
	struct c4iw_id_table qid_table;
	struct c4iw_id_table pdid_table;
	struct c4iw_id_table srq_table;
};

struct c4iw_qid_list {
	struct list_head entry;
	u32 qid;
};

struct c4iw_dev_ucontext {
	struct list_head qpids;
	struct list_head cqids;
	struct mutex lock;
	struct kref kref;
};

enum c4iw_rdev_flags {
	T4_FATAL_ERROR = (1<<0),
	T4_STATUS_PAGE_DISABLED = (1<<1),
};

struct c4iw_stat {
	u64 total;
	u64 cur;
	u64 max;
	u64 fail;
};

struct c4iw_stats {
	struct mutex lock;
	struct c4iw_stat qid;
	struct c4iw_stat pd;
	struct c4iw_stat stag;
	struct c4iw_stat pbl;
	struct c4iw_stat rqt;
	struct c4iw_stat srqt;
	struct c4iw_stat srq;
	struct c4iw_stat ocqp;
	u64  db_full;
	u64  db_empty;
	u64  db_drop;
	u64  db_state_transitions;
	u64  db_fc_interruptions;
	u64  tcam_full;
	u64  act_ofld_conn_fails;
	u64  pas_ofld_conn_fails;
	u64  neg_adv;
};

struct c4iw_hw_queue {
	int t4_eq_status_entries;
	int t4_max_eq_size;
	int t4_max_iq_size;
	int t4_max_rq_size;
	int t4_max_sq_size;
	int t4_max_qp_depth;
	int t4_max_cq_depth;
	int t4_stat_len;
};

struct wr_log_entry {
	ktime_t post_host_time;
	ktime_t poll_host_time;
	u64 post_sge_ts;
	u64 cqe_sge_ts;
	u64 poll_sge_ts;
	u16 qid;
	u16 wr_id;
	u8 opcode;
	u8 valid;
};

struct c4iw_rdev {
	struct c4iw_resource resource;
	u32 qpmask;
	u32 cqmask;
	struct c4iw_dev_ucontext uctx;
	struct gen_pool *pbl_pool;
	struct gen_pool *rqt_pool;
	struct gen_pool *ocqp_pool;
	u32 flags;
	struct cxgb4_lld_info lldi;
	unsigned long bar2_pa;
	void __iomem *bar2_kva;
	unsigned long oc_mw_pa;
	void __iomem *oc_mw_kva;
	struct c4iw_stats stats;
	struct c4iw_hw_queue hw_queue;
	struct t4_dev_status_page *status_page;
	atomic_t wr_log_idx;
	struct wr_log_entry *wr_log;
	int wr_log_size;
	struct workqueue_struct *free_workq;
	struct completion rqt_compl;
	struct completion pbl_compl;
	struct kref rqt_kref;
	struct kref pbl_kref;
};

static inline int c4iw_fatal_error(struct c4iw_rdev *rdev)
{
	return rdev->flags & T4_FATAL_ERROR;
}

static inline int c4iw_num_stags(struct c4iw_rdev *rdev)
{
	return (int)(rdev->lldi.vr->stag.size >> 5);
}

#define C4IW_WR_TO (60*HZ)

struct c4iw_wr_wait {
	struct completion completion;
	int ret;
	struct kref kref;
};

void _c4iw_free_wr_wait(struct kref *kref);

static inline void c4iw_put_wr_wait(struct c4iw_wr_wait *wr_waitp)
{
	pr_debug("wr_wait %p ref before put %u\n", wr_waitp,
		 kref_read(&wr_waitp->kref));
	WARN_ON(kref_read(&wr_waitp->kref) == 0);
	kref_put(&wr_waitp->kref, _c4iw_free_wr_wait);
}

static inline void c4iw_get_wr_wait(struct c4iw_wr_wait *wr_waitp)
{
	pr_debug("wr_wait %p ref before get %u\n", wr_waitp,
		 kref_read(&wr_waitp->kref));
	WARN_ON(kref_read(&wr_waitp->kref) == 0);
	kref_get(&wr_waitp->kref);
}

static inline void c4iw_init_wr_wait(struct c4iw_wr_wait *wr_waitp)
{
	wr_waitp->ret = 0;
	init_completion(&wr_waitp->completion);
}

static inline void _c4iw_wake_up(struct c4iw_wr_wait *wr_waitp, int ret,
				 bool deref)
{
	wr_waitp->ret = ret;
	complete(&wr_waitp->completion);
	if (deref)
		c4iw_put_wr_wait(wr_waitp);
}

static inline void c4iw_wake_up_noref(struct c4iw_wr_wait *wr_waitp, int ret)
{
	_c4iw_wake_up(wr_waitp, ret, false);
}

static inline void c4iw_wake_up_deref(struct c4iw_wr_wait *wr_waitp, int ret)
{
	_c4iw_wake_up(wr_waitp, ret, true);
}

static inline int c4iw_wait_for_reply(struct c4iw_rdev *rdev,
				 struct c4iw_wr_wait *wr_waitp,
				 u32 hwtid, u32 qpid,
				 const char *func)
{
	int ret;

	if (c4iw_fatal_error(rdev)) {
		wr_waitp->ret = -EIO;
		goto out;
	}

	ret = wait_for_completion_timeout(&wr_waitp->completion, C4IW_WR_TO);
	if (!ret) {
		pr_err("%s - Device %s not responding (disabling device) - tid %u qpid %u\n",
		       func, pci_name(rdev->lldi.pdev), hwtid, qpid);
		rdev->flags |= T4_FATAL_ERROR;
		wr_waitp->ret = -EIO;
		goto out;
	}
	if (wr_waitp->ret)
		pr_debug("%s: FW reply %d tid %u qpid %u\n",
			 pci_name(rdev->lldi.pdev), wr_waitp->ret, hwtid, qpid);
out:
	return wr_waitp->ret;
}

int c4iw_ofld_send(struct c4iw_rdev *rdev, struct sk_buff *skb);

static inline int c4iw_ref_send_wait(struct c4iw_rdev *rdev,
				     struct sk_buff *skb,
				     struct c4iw_wr_wait *wr_waitp,
				     u32 hwtid, u32 qpid,
				     const char *func)
{
	int ret;

	pr_debug("%s wr_wait %p hwtid %u qpid %u\n", func, wr_waitp, hwtid,
		 qpid);
	c4iw_get_wr_wait(wr_waitp);
	ret = c4iw_ofld_send(rdev, skb);
	if (ret) {
		c4iw_put_wr_wait(wr_waitp);
		return ret;
	}
	return c4iw_wait_for_reply(rdev, wr_waitp, hwtid, qpid, func);
}

enum db_state {
	NORMAL = 0,
	FLOW_CONTROL = 1,
	RECOVERY = 2,
	STOPPED = 3
};

struct c4iw_dev {
	struct ib_device ibdev;
	struct c4iw_rdev rdev;
	u32 device_cap_flags;
	struct xarray cqs;
	struct xarray qps;
	struct xarray mrs;
	struct mutex db_mutex;
	struct dentry *debugfs_root;
	enum db_state db_state;
	struct xarray hwtids;
	struct xarray atids;
	struct xarray stids;
	struct list_head db_fc_list;
	u32 avail_ird;
	wait_queue_head_t wait;
};

struct uld_ctx {
	struct list_head entry;
	struct cxgb4_lld_info lldi;
	struct c4iw_dev *dev;
	struct work_struct reg_work;
};

static inline struct c4iw_dev *to_c4iw_dev(struct ib_device *ibdev)
{
	return container_of(ibdev, struct c4iw_dev, ibdev);
}

static inline struct c4iw_cq *get_chp(struct c4iw_dev *rhp, u32 cqid)
{
	return xa_load(&rhp->cqs, cqid);
}

static inline struct c4iw_qp *get_qhp(struct c4iw_dev *rhp, u32 qpid)
{
	return xa_load(&rhp->qps, qpid);
}

extern uint c4iw_max_read_depth;

static inline int cur_max_read_depth(struct c4iw_dev *dev)
{
	return min(dev->rdev.lldi.max_ordird_qp, c4iw_max_read_depth);
}

struct c4iw_pd {
	struct ib_pd ibpd;
	u32 pdid;
	struct c4iw_dev *rhp;
};

static inline struct c4iw_pd *to_c4iw_pd(struct ib_pd *ibpd)
{
	return container_of(ibpd, struct c4iw_pd, ibpd);
}

struct tpt_attributes {
	u64 len;
	u64 va_fbo;
	enum fw_ri_mem_perms perms;
	u32 stag;
	u32 pdid;
	u32 qpid;
	u32 pbl_addr;
	u32 pbl_size;
	u32 state:1;
	u32 type:2;
	u32 rsvd:1;
	u32 remote_invaliate_disable:1;
	u32 zbva:1;
	u32 mw_bind_enable:1;
	u32 page_size:5;
};

struct c4iw_mr {
	struct ib_mr ibmr;
	struct ib_umem *umem;
	struct c4iw_dev *rhp;
	struct sk_buff *dereg_skb;
	u64 kva;
	struct tpt_attributes attr;
	u64 *mpl;
	dma_addr_t mpl_addr;
	u32 max_mpl_len;
	u32 mpl_len;
	struct c4iw_wr_wait *wr_waitp;
};

static inline struct c4iw_mr *to_c4iw_mr(struct ib_mr *ibmr)
{
	return container_of(ibmr, struct c4iw_mr, ibmr);
}

struct c4iw_mw {
	struct ib_mw ibmw;
	struct c4iw_dev *rhp;
	struct sk_buff *dereg_skb;
	u64 kva;
	struct tpt_attributes attr;
	struct c4iw_wr_wait *wr_waitp;
};

static inline struct c4iw_mw *to_c4iw_mw(struct ib_mw *ibmw)
{
	return container_of(ibmw, struct c4iw_mw, ibmw);
}

struct c4iw_cq {
	struct ib_cq ibcq;
	struct c4iw_dev *rhp;
	struct sk_buff *destroy_skb;
	struct t4_cq cq;
	spinlock_t lock;
	spinlock_t comp_handler_lock;
	atomic_t refcnt;
	wait_queue_head_t wait;
	struct c4iw_wr_wait *wr_waitp;
};

static inline struct c4iw_cq *to_c4iw_cq(struct ib_cq *ibcq)
{
	return container_of(ibcq, struct c4iw_cq, ibcq);
}

struct c4iw_mpa_attributes {
	u8 initiator;
	u8 recv_marker_enabled;
	u8 xmit_marker_enabled;
	u8 crc_enabled;
	u8 enhanced_rdma_conn;
	u8 version;
	u8 p2p_type;
};

struct c4iw_qp_attributes {
	u32 scq;
	u32 rcq;
	u32 sq_num_entries;
	u32 rq_num_entries;
	u32 sq_max_sges;
	u32 sq_max_sges_rdma_write;
	u32 rq_max_sges;
	u32 state;
	u8 enable_rdma_read;
	u8 enable_rdma_write;
	u8 enable_bind;
	u8 enable_mmid0_fastreg;
	u32 max_ord;
	u32 max_ird;
	u32 pd;
	u32 next_state;
	char terminate_buffer[52];
	u32 terminate_msg_len;
	u8 is_terminate_local;
	struct c4iw_mpa_attributes mpa_attr;
	struct c4iw_ep *llp_stream_handle;
	u8 layer_etype;
	u8 ecode;
	u16 sq_db_inc;
	u16 rq_db_inc;
	u8 send_term;
};

struct c4iw_qp {
	struct ib_qp ibqp;
	struct list_head db_fc_entry;
	struct c4iw_dev *rhp;
	struct c4iw_ep *ep;
	struct c4iw_qp_attributes attr;
	struct t4_wq wq;
	spinlock_t lock;
	struct mutex mutex;
	wait_queue_head_t wait;
	int sq_sig_all;
	struct c4iw_srq *srq;
	struct c4iw_ucontext *ucontext;
	struct c4iw_wr_wait *wr_waitp;
	struct completion qp_rel_comp;
	refcount_t qp_refcnt;
};

static inline struct c4iw_qp *to_c4iw_qp(struct ib_qp *ibqp)
{
	return container_of(ibqp, struct c4iw_qp, ibqp);
}

struct c4iw_srq {
	struct ib_srq ibsrq;
	struct list_head db_fc_entry;
	struct c4iw_dev *rhp;
	struct t4_srq wq;
	struct sk_buff *destroy_skb;
	u32 srq_limit;
	u32 pdid;
	int idx;
	u32 flags;
	spinlock_t lock; /* protects srq */
	struct c4iw_wr_wait *wr_waitp;
	bool armed;
};

static inline struct c4iw_srq *to_c4iw_srq(struct ib_srq *ibsrq)
{
	return container_of(ibsrq, struct c4iw_srq, ibsrq);
}

struct c4iw_ucontext {
	struct ib_ucontext ibucontext;
	struct c4iw_dev_ucontext uctx;
	u32 key;
	spinlock_t mmap_lock;
	struct list_head mmaps;
	bool is_32b_cqe;
};

static inline struct c4iw_ucontext *to_c4iw_ucontext(struct ib_ucontext *c)
{
	return container_of(c, struct c4iw_ucontext, ibucontext);
}

struct c4iw_mm_entry {
	struct list_head entry;
	u64 addr;
	u32 key;
	unsigned len;
};

static inline struct c4iw_mm_entry *remove_mmap(struct c4iw_ucontext *ucontext,
						u32 key, unsigned len)
{
	struct list_head *pos, *nxt;
	struct c4iw_mm_entry *mm;

	spin_lock(&ucontext->mmap_lock);
	list_for_each_safe(pos, nxt, &ucontext->mmaps) {

		mm = list_entry(pos, struct c4iw_mm_entry, entry);
		if (mm->key == key && mm->len == len) {
			list_del_init(&mm->entry);
			spin_unlock(&ucontext->mmap_lock);
			pr_debug("key 0x%x addr 0x%llx len %d\n", key,
				 (unsigned long long)mm->addr, mm->len);
			return mm;
		}
	}
	spin_unlock(&ucontext->mmap_lock);
	return NULL;
}

static inline void insert_mmap(struct c4iw_ucontext *ucontext,
			       struct c4iw_mm_entry *mm)
{
	spin_lock(&ucontext->mmap_lock);
	pr_debug("key 0x%x addr 0x%llx len %d\n",
		 mm->key, (unsigned long long)mm->addr, mm->len);
	list_add_tail(&mm->entry, &ucontext->mmaps);
	spin_unlock(&ucontext->mmap_lock);
}

enum c4iw_qp_attr_mask {
	C4IW_QP_ATTR_NEXT_STATE = 1 << 0,
	C4IW_QP_ATTR_SQ_DB = 1<<1,
	C4IW_QP_ATTR_RQ_DB = 1<<2,
	C4IW_QP_ATTR_ENABLE_RDMA_READ = 1 << 7,
	C4IW_QP_ATTR_ENABLE_RDMA_WRITE = 1 << 8,
	C4IW_QP_ATTR_ENABLE_RDMA_BIND = 1 << 9,
	C4IW_QP_ATTR_MAX_ORD = 1 << 11,
	C4IW_QP_ATTR_MAX_IRD = 1 << 12,
	C4IW_QP_ATTR_LLP_STREAM_HANDLE = 1 << 22,
	C4IW_QP_ATTR_STREAM_MSG_BUFFER = 1 << 23,
	C4IW_QP_ATTR_MPA_ATTR = 1 << 24,
	C4IW_QP_ATTR_QP_CONTEXT_ACTIVATE = 1 << 25,
	C4IW_QP_ATTR_VALID_MODIFY = (C4IW_QP_ATTR_ENABLE_RDMA_READ |
				     C4IW_QP_ATTR_ENABLE_RDMA_WRITE |
				     C4IW_QP_ATTR_MAX_ORD |
				     C4IW_QP_ATTR_MAX_IRD |
				     C4IW_QP_ATTR_LLP_STREAM_HANDLE |
				     C4IW_QP_ATTR_STREAM_MSG_BUFFER |
				     C4IW_QP_ATTR_MPA_ATTR |
				     C4IW_QP_ATTR_QP_CONTEXT_ACTIVATE)
};

int c4iw_modify_qp(struct c4iw_dev *rhp,
				struct c4iw_qp *qhp,
				enum c4iw_qp_attr_mask mask,
				struct c4iw_qp_attributes *attrs,
				int internal);

enum c4iw_qp_state {
	C4IW_QP_STATE_IDLE,
	C4IW_QP_STATE_RTS,
	C4IW_QP_STATE_ERROR,
	C4IW_QP_STATE_TERMINATE,
	C4IW_QP_STATE_CLOSING,
	C4IW_QP_STATE_TOT
};

static inline int c4iw_convert_state(enum ib_qp_state ib_state)
{
	switch (ib_state) {
	case IB_QPS_RESET:
	case IB_QPS_INIT:
		return C4IW_QP_STATE_IDLE;
	case IB_QPS_RTS:
		return C4IW_QP_STATE_RTS;
	case IB_QPS_SQD:
		return C4IW_QP_STATE_CLOSING;
	case IB_QPS_SQE:
		return C4IW_QP_STATE_TERMINATE;
	case IB_QPS_ERR:
		return C4IW_QP_STATE_ERROR;
	default:
		return -1;
	}
}

static inline int to_ib_qp_state(int c4iw_qp_state)
{
	switch (c4iw_qp_state) {
	case C4IW_QP_STATE_IDLE:
		return IB_QPS_INIT;
	case C4IW_QP_STATE_RTS:
		return IB_QPS_RTS;
	case C4IW_QP_STATE_CLOSING:
		return IB_QPS_SQD;
	case C4IW_QP_STATE_TERMINATE:
		return IB_QPS_SQE;
	case C4IW_QP_STATE_ERROR:
		return IB_QPS_ERR;
	}
	return IB_QPS_ERR;
}

static inline u32 c4iw_ib_to_tpt_access(int a)
{
	return (a & IB_ACCESS_REMOTE_WRITE ? FW_RI_MEM_ACCESS_REM_WRITE : 0) |
	       (a & IB_ACCESS_REMOTE_READ ? FW_RI_MEM_ACCESS_REM_READ : 0) |
	       (a & IB_ACCESS_LOCAL_WRITE ? FW_RI_MEM_ACCESS_LOCAL_WRITE : 0) |
	       FW_RI_MEM_ACCESS_LOCAL_READ;
}

enum c4iw_mmid_state {
	C4IW_STAG_STATE_VALID,
	C4IW_STAG_STATE_INVALID
};

#define C4IW_NODE_DESC "cxgb4 Chelsio Communications"

#define MPA_KEY_REQ "MPA ID Req Frame"
#define MPA_KEY_REP "MPA ID Rep Frame"

#define MPA_MAX_PRIVATE_DATA	256
#define MPA_ENHANCED_RDMA_CONN	0x10
#define MPA_REJECT		0x20
#define MPA_CRC			0x40
#define MPA_MARKERS		0x80
#define MPA_FLAGS_MASK		0xE0

#define MPA_V2_PEER2PEER_MODEL          0x8000
#define MPA_V2_ZERO_LEN_FPDU_RTR        0x4000
#define MPA_V2_RDMA_WRITE_RTR           0x8000
#define MPA_V2_RDMA_READ_RTR            0x4000
#define MPA_V2_IRD_ORD_MASK             0x3FFF

#define c4iw_put_ep(ep) {						\
	pr_debug("put_ep ep %p refcnt %d\n",		\
		 ep, kref_read(&((ep)->kref)));				\
	WARN_ON(kref_read(&((ep)->kref)) < 1);				\
	kref_put(&((ep)->kref), _c4iw_free_ep);				\
}

#define c4iw_get_ep(ep) {						\
	pr_debug("get_ep ep %p, refcnt %d\n",		\
		 ep, kref_read(&((ep)->kref)));				\
	kref_get(&((ep)->kref));					\
}
void _c4iw_free_ep(struct kref *kref);

struct mpa_message {
	u8 key[16];
	u8 flags;
	u8 revision;
	__be16 private_data_size;
	u8 private_data[];
};

struct mpa_v2_conn_params {
	__be16 ird;
	__be16 ord;
};

struct terminate_message {
	u8 layer_etype;
	u8 ecode;
	__be16 hdrct_rsvd;
	u8 len_hdrs[];
};

#define TERM_MAX_LENGTH (sizeof(struct terminate_message) + 2 + 18 + 28)

enum c4iw_layers_types {
	LAYER_RDMAP		= 0x00,
	LAYER_DDP		= 0x10,
	LAYER_MPA		= 0x20,
	RDMAP_LOCAL_CATA	= 0x00,
	RDMAP_REMOTE_PROT	= 0x01,
	RDMAP_REMOTE_OP		= 0x02,
	DDP_LOCAL_CATA		= 0x00,
	DDP_TAGGED_ERR		= 0x01,
	DDP_UNTAGGED_ERR	= 0x02,
	DDP_LLP			= 0x03
};

enum c4iw_rdma_ecodes {
	RDMAP_INV_STAG		= 0x00,
	RDMAP_BASE_BOUNDS	= 0x01,
	RDMAP_ACC_VIOL		= 0x02,
	RDMAP_STAG_NOT_ASSOC	= 0x03,
	RDMAP_TO_WRAP		= 0x04,
	RDMAP_INV_VERS		= 0x05,
	RDMAP_INV_OPCODE	= 0x06,
	RDMAP_STREAM_CATA	= 0x07,
	RDMAP_GLOBAL_CATA	= 0x08,
	RDMAP_CANT_INV_STAG	= 0x09,
	RDMAP_UNSPECIFIED	= 0xff
};

enum c4iw_ddp_ecodes {
	DDPT_INV_STAG		= 0x00,
	DDPT_BASE_BOUNDS	= 0x01,
	DDPT_STAG_NOT_ASSOC	= 0x02,
	DDPT_TO_WRAP		= 0x03,
	DDPT_INV_VERS		= 0x04,
	DDPU_INV_QN		= 0x01,
	DDPU_INV_MSN_NOBUF	= 0x02,
	DDPU_INV_MSN_RANGE	= 0x03,
	DDPU_INV_MO		= 0x04,
	DDPU_MSG_TOOBIG		= 0x05,
	DDPU_INV_VERS		= 0x06
};

enum c4iw_mpa_ecodes {
	MPA_CRC_ERR		= 0x02,
	MPA_MARKER_ERR          = 0x03,
	MPA_LOCAL_CATA          = 0x05,
	MPA_INSUFF_IRD          = 0x06,
	MPA_NOMATCH_RTR         = 0x07,
};

enum c4iw_ep_state {
	IDLE = 0,
	LISTEN,
	CONNECTING,
	MPA_REQ_WAIT,
	MPA_REQ_SENT,
	MPA_REQ_RCVD,
	MPA_REP_SENT,
	FPDU_MODE,
	ABORTING,
	CLOSING,
	MORIBUND,
	DEAD,
};

enum c4iw_ep_flags {
	PEER_ABORT_IN_PROGRESS	= 0,
	ABORT_REQ_IN_PROGRESS	= 1,
	RELEASE_RESOURCES	= 2,
	CLOSE_SENT		= 3,
	TIMEOUT                 = 4,
	QP_REFERENCED           = 5,
	STOP_MPA_TIMER		= 7,
};

enum c4iw_ep_history {
	ACT_OPEN_REQ            = 0,
	ACT_OFLD_CONN           = 1,
	ACT_OPEN_RPL            = 2,
	ACT_ESTAB               = 3,
	PASS_ACCEPT_REQ         = 4,
	PASS_ESTAB              = 5,
	ABORT_UPCALL            = 6,
	ESTAB_UPCALL            = 7,
	CLOSE_UPCALL            = 8,
	ULP_ACCEPT              = 9,
	ULP_REJECT              = 10,
	TIMEDOUT                = 11,
	PEER_ABORT              = 12,
	PEER_CLOSE              = 13,
	CONNREQ_UPCALL          = 14,
	ABORT_CONN              = 15,
	DISCONN_UPCALL          = 16,
	EP_DISC_CLOSE           = 17,
	EP_DISC_ABORT           = 18,
	CONN_RPL_UPCALL         = 19,
	ACT_RETRY_NOMEM         = 20,
	ACT_RETRY_INUSE         = 21,
	CLOSE_CON_RPL		= 22,
	EP_DISC_FAIL		= 24,
	QP_REFED		= 25,
	QP_DEREFED		= 26,
	CM_ID_REFED		= 27,
	CM_ID_DEREFED		= 28,
};

enum conn_pre_alloc_buffers {
	CN_ABORT_REQ_BUF,
	CN_ABORT_RPL_BUF,
	CN_CLOSE_CON_REQ_BUF,
	CN_DESTROY_BUF,
	CN_FLOWC_BUF,
	CN_MAX_CON_BUF
};

enum {
	FLOWC_LEN = offsetof(struct fw_flowc_wr, mnemval[FW_FLOWC_MNEM_MAX])
};

union cpl_wr_size {
	struct cpl_abort_req abrt_req;
	struct cpl_abort_rpl abrt_rpl;
	struct fw_ri_wr ri_req;
	struct cpl_close_con_req close_req;
	char flowc_buf[FLOWC_LEN];
};

struct c4iw_ep_common {
	struct iw_cm_id *cm_id;
	struct c4iw_qp *qp;
	struct c4iw_dev *dev;
	struct sk_buff_head ep_skb_list;
	enum c4iw_ep_state state;
	struct kref kref;
	struct mutex mutex;
	struct sockaddr_storage local_addr;
	struct sockaddr_storage remote_addr;
	struct c4iw_wr_wait *wr_waitp;
	unsigned long flags;
	unsigned long history;
};

struct c4iw_listen_ep {
	struct c4iw_ep_common com;
	unsigned int stid;
	int backlog;
};

struct c4iw_ep_stats {
	unsigned connect_neg_adv;
	unsigned abort_neg_adv;
};

struct c4iw_ep {
	struct c4iw_ep_common com;
	struct c4iw_ep *parent_ep;
	struct timer_list timer;
	struct list_head entry;
	unsigned int atid;
	u32 hwtid;
	u32 snd_seq;
	u32 rcv_seq;
	struct l2t_entry *l2t;
	struct dst_entry *dst;
	struct sk_buff *mpa_skb;
	struct c4iw_mpa_attributes mpa_attr;
	u8 mpa_pkt[sizeof(struct mpa_message) + MPA_MAX_PRIVATE_DATA];
	unsigned int mpa_pkt_len;
	u32 ird;
	u32 ord;
	u32 smac_idx;
	u32 tx_chan;
	u32 mtu;
	u16 mss;
	u16 emss;
	u16 plen;
	u16 rss_qid;
	u16 txq_idx;
	u16 ctrlq_idx;
	u8 tos;
	u8 retry_with_mpa_v1;
	u8 tried_with_mpa_v1;
	unsigned int retry_count;
	int snd_win;
	int rcv_win;
	u32 snd_wscale;
	struct c4iw_ep_stats stats;
	u32 srqe_idx;
	u32 rx_pdu_out_cnt;
	struct sk_buff *peer_abort_skb;
};

static inline struct c4iw_ep *to_ep(struct iw_cm_id *cm_id)
{
	return cm_id->provider_data;
}

static inline struct c4iw_listen_ep *to_listen_ep(struct iw_cm_id *cm_id)
{
	return cm_id->provider_data;
}

static inline int ocqp_supported(const struct cxgb4_lld_info *infop)
{
#if defined(__i386__) || defined(__x86_64__) || defined(CONFIG_PPC64)
	return infop->vr->ocq.size > 0;
#else
	return 0;
#endif
}

u32 c4iw_id_alloc(struct c4iw_id_table *alloc);
void c4iw_id_free(struct c4iw_id_table *alloc, u32 obj);
int c4iw_id_table_alloc(struct c4iw_id_table *alloc, u32 start, u32 num,
			u32 reserved, u32 flags);
void c4iw_id_table_free(struct c4iw_id_table *alloc);

typedef int (*c4iw_handler_func)(struct c4iw_dev *dev, struct sk_buff *skb);

int c4iw_ep_redirect(void *ctx, struct dst_entry *old, struct dst_entry *new,
		     struct l2t_entry *l2t);
void c4iw_put_qpid(struct c4iw_rdev *rdev, u32 qpid,
		   struct c4iw_dev_ucontext *uctx);
u32 c4iw_get_resource(struct c4iw_id_table *id_table);
void c4iw_put_resource(struct c4iw_id_table *id_table, u32 entry);
int c4iw_init_resource(struct c4iw_rdev *rdev, u32 nr_tpt,
		       u32 nr_pdid, u32 nr_srqt);
int c4iw_init_ctrl_qp(struct c4iw_rdev *rdev);
int c4iw_pblpool_create(struct c4iw_rdev *rdev);
int c4iw_rqtpool_create(struct c4iw_rdev *rdev);
int c4iw_ocqp_pool_create(struct c4iw_rdev *rdev);
void c4iw_pblpool_destroy(struct c4iw_rdev *rdev);
void c4iw_rqtpool_destroy(struct c4iw_rdev *rdev);
void c4iw_ocqp_pool_destroy(struct c4iw_rdev *rdev);
void c4iw_destroy_resource(struct c4iw_resource *rscp);
int c4iw_destroy_ctrl_qp(struct c4iw_rdev *rdev);
void c4iw_register_device(struct work_struct *work);
void c4iw_unregister_device(struct c4iw_dev *dev);
int __init c4iw_cm_init(void);
void c4iw_cm_term(void);
void c4iw_release_dev_ucontext(struct c4iw_rdev *rdev,
			       struct c4iw_dev_ucontext *uctx);
void c4iw_init_dev_ucontext(struct c4iw_rdev *rdev,
			    struct c4iw_dev_ucontext *uctx);
int c4iw_poll_cq(struct ib_cq *ibcq, int num_entries, struct ib_wc *wc);
int c4iw_post_send(struct ib_qp *ibqp, const struct ib_send_wr *wr,
		   const struct ib_send_wr **bad_wr);
int c4iw_post_receive(struct ib_qp *ibqp, const struct ib_recv_wr *wr,
		      const struct ib_recv_wr **bad_wr);
int c4iw_connect(struct iw_cm_id *cm_id, struct iw_cm_conn_param *conn_param);
int c4iw_create_listen(struct iw_cm_id *cm_id, int backlog);
int c4iw_destroy_listen(struct iw_cm_id *cm_id);
int c4iw_accept_cr(struct iw_cm_id *cm_id, struct iw_cm_conn_param *conn_param);
int c4iw_reject_cr(struct iw_cm_id *cm_id, const void *pdata, u8 pdata_len);
void c4iw_qp_add_ref(struct ib_qp *qp);
void c4iw_qp_rem_ref(struct ib_qp *qp);
struct ib_mr *c4iw_alloc_mr(struct ib_pd *pd, enum ib_mr_type mr_type,
			    u32 max_num_sg);
int c4iw_map_mr_sg(struct ib_mr *ibmr, struct scatterlist *sg, int sg_nents,
		   unsigned int *sg_offset);
void c4iw_dealloc(struct uld_ctx *ctx);
struct ib_mr *c4iw_reg_user_mr(struct ib_pd *pd, u64 start,
					   u64 length, u64 virt, int acc,
					   struct ib_udata *udata);
struct ib_mr *c4iw_get_dma_mr(struct ib_pd *pd, int acc);
int c4iw_dereg_mr(struct ib_mr *ib_mr, struct ib_udata *udata);
int c4iw_destroy_cq(struct ib_cq *ib_cq, struct ib_udata *udata);
int c4iw_create_cq(struct ib_cq *ibcq, const struct ib_cq_init_attr *attr,
		   struct ib_udata *udata);
int c4iw_arm_cq(struct ib_cq *ibcq, enum ib_cq_notify_flags flags);
int c4iw_modify_srq(struct ib_srq *ib_srq, struct ib_srq_attr *attr,
		    enum ib_srq_attr_mask srq_attr_mask,
		    struct ib_udata *udata);
int c4iw_destroy_srq(struct ib_srq *ib_srq, struct ib_udata *udata);
int c4iw_create_srq(struct ib_srq *srq, struct ib_srq_init_attr *attrs,
		    struct ib_udata *udata);
int c4iw_destroy_qp(struct ib_qp *ib_qp, struct ib_udata *udata);
struct ib_qp *c4iw_create_qp(struct ib_pd *pd,
			     struct ib_qp_init_attr *attrs,
			     struct ib_udata *udata);
int c4iw_ib_modify_qp(struct ib_qp *ibqp, struct ib_qp_attr *attr,
				 int attr_mask, struct ib_udata *udata);
int c4iw_ib_query_qp(struct ib_qp *ibqp, struct ib_qp_attr *attr,
		     int attr_mask, struct ib_qp_init_attr *init_attr);
struct ib_qp *c4iw_get_qp(struct ib_device *dev, int qpn);
u32 c4iw_rqtpool_alloc(struct c4iw_rdev *rdev, int size);
void c4iw_rqtpool_free(struct c4iw_rdev *rdev, u32 addr, int size);
u32 c4iw_pblpool_alloc(struct c4iw_rdev *rdev, int size);
void c4iw_pblpool_free(struct c4iw_rdev *rdev, u32 addr, int size);
u32 c4iw_ocqp_pool_alloc(struct c4iw_rdev *rdev, int size);
void c4iw_ocqp_pool_free(struct c4iw_rdev *rdev, u32 addr, int size);
void c4iw_flush_hw_cq(struct c4iw_cq *chp, struct c4iw_qp *flush_qhp);
void c4iw_count_rcqes(struct t4_cq *cq, struct t4_wq *wq, int *count);
int c4iw_ep_disconnect(struct c4iw_ep *ep, int abrupt, gfp_t gfp);
int c4iw_flush_rq(struct t4_wq *wq, struct t4_cq *cq, int count);
int c4iw_flush_sq(struct c4iw_qp *qhp);
int c4iw_ev_handler(struct c4iw_dev *rnicp, u32 qid);
u16 c4iw_rqes_posted(struct c4iw_qp *qhp);
int c4iw_post_terminate(struct c4iw_qp *qhp, struct t4_cqe *err_cqe);
u32 c4iw_get_cqid(struct c4iw_rdev *rdev, struct c4iw_dev_ucontext *uctx);
void c4iw_put_cqid(struct c4iw_rdev *rdev, u32 qid,
		struct c4iw_dev_ucontext *uctx);
u32 c4iw_get_qpid(struct c4iw_rdev *rdev, struct c4iw_dev_ucontext *uctx);
void c4iw_put_qpid(struct c4iw_rdev *rdev, u32 qid,
		struct c4iw_dev_ucontext *uctx);
void c4iw_ev_dispatch(struct c4iw_dev *dev, struct t4_cqe *err_cqe);

extern struct cxgb4_client t4c_client;
extern c4iw_handler_func c4iw_handlers[NUM_CPL_CMDS];
void __iomem *c4iw_bar2_addrs(struct c4iw_rdev *rdev, unsigned int qid,
			      enum cxgb4_bar2_qtype qtype,
			      unsigned int *pbar2_qid, u64 *pbar2_pa);
int c4iw_alloc_srq_idx(struct c4iw_rdev *rdev);
void c4iw_free_srq_idx(struct c4iw_rdev *rdev, int idx);
extern void c4iw_log_wr_stats(struct t4_wq *wq, struct t4_cqe *cqe);
extern int c4iw_wr_log;
extern int db_fc_threshold;
extern int db_coalescing_threshold;
extern int use_dsgl;
void c4iw_invalidate_mr(struct c4iw_dev *rhp, u32 rkey);
void c4iw_dispatch_srq_limit_reached_event(struct c4iw_srq *srq);
void c4iw_copy_wr_to_srq(struct t4_srq *srq, union t4_recv_wr *wqe, u8 len16);
void c4iw_flush_srqidx(struct c4iw_qp *qhp, u32 srqidx);
int c4iw_post_srq_recv(struct ib_srq *ibsrq, const struct ib_recv_wr *wr,
		       const struct ib_recv_wr **bad_wr);
struct c4iw_wr_wait *c4iw_alloc_wr_wait(gfp_t gfp);

int c4iw_fill_res_mr_entry(struct sk_buff *msg, struct ib_mr *ibmr);
int c4iw_fill_res_cq_entry(struct sk_buff *msg, struct ib_cq *ibcq);
int c4iw_fill_res_qp_entry(struct sk_buff *msg, struct ib_qp *ibqp);
int c4iw_fill_res_cm_id_entry(struct sk_buff *msg, struct rdma_cm_id *cm_id);

#endif
