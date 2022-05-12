/* SPDX-License-Identifier: GPL-2.0 OR Linux-OpenIB */
/*
 * Copyright (c) 2016 Mellanox Technologies Ltd. All rights reserved.
 * Copyright (c) 2015 System Fabric Works, Inc. All rights reserved.
 */

#ifndef RXE_LOC_H
#define RXE_LOC_H

/* rxe_av.c */
void rxe_init_av(struct rdma_ah_attr *attr, struct rxe_av *av);

int rxe_av_chk_attr(struct rxe_dev *rxe, struct rdma_ah_attr *attr);

void rxe_av_from_attr(u8 port_num, struct rxe_av *av,
		     struct rdma_ah_attr *attr);

void rxe_av_to_attr(struct rxe_av *av, struct rdma_ah_attr *attr);

void rxe_av_fill_ip_info(struct rxe_av *av, struct rdma_ah_attr *attr);

struct rxe_av *rxe_get_av(struct rxe_pkt_info *pkt);

/* rxe_cq.c */
int rxe_cq_chk_attr(struct rxe_dev *rxe, struct rxe_cq *cq,
		    int cqe, int comp_vector);

int rxe_cq_from_init(struct rxe_dev *rxe, struct rxe_cq *cq, int cqe,
		     int comp_vector, struct ib_udata *udata,
		     struct rxe_create_cq_resp __user *uresp);

int rxe_cq_resize_queue(struct rxe_cq *cq, int new_cqe,
			struct rxe_resize_cq_resp __user *uresp,
			struct ib_udata *udata);

int rxe_cq_post(struct rxe_cq *cq, struct rxe_cqe *cqe, int solicited);

void rxe_cq_disable(struct rxe_cq *cq);

void rxe_cq_cleanup(struct rxe_pool_entry *arg);

/* rxe_mcast.c */
int rxe_mcast_get_grp(struct rxe_dev *rxe, union ib_gid *mgid,
		      struct rxe_mc_grp **grp_p);

int rxe_mcast_add_grp_elem(struct rxe_dev *rxe, struct rxe_qp *qp,
			   struct rxe_mc_grp *grp);

int rxe_mcast_drop_grp_elem(struct rxe_dev *rxe, struct rxe_qp *qp,
			    union ib_gid *mgid);

void rxe_drop_all_mcast_groups(struct rxe_qp *qp);

void rxe_mc_cleanup(struct rxe_pool_entry *arg);

/* rxe_mmap.c */
struct rxe_mmap_info {
	struct list_head	pending_mmaps;
	struct ib_ucontext	*context;
	struct kref		ref;
	void			*obj;

	struct mminfo info;
};

void rxe_mmap_release(struct kref *ref);

struct rxe_mmap_info *rxe_create_mmap_info(struct rxe_dev *dev, u32 size,
					   struct ib_udata *udata, void *obj);

int rxe_mmap(struct ib_ucontext *context, struct vm_area_struct *vma);

/* rxe_mr.c */
enum copy_direction {
	to_mr_obj,
	from_mr_obj,
};

void rxe_mr_init_dma(struct rxe_pd *pd, int access, struct rxe_mr *mr);

int rxe_mr_init_user(struct rxe_pd *pd, u64 start, u64 length, u64 iova,
		     int access, struct ib_udata *udata, struct rxe_mr *mr);

int rxe_mr_init_fast(struct rxe_pd *pd, int max_pages, struct rxe_mr *mr);

int rxe_mr_copy(struct rxe_mr *mr, u64 iova, void *addr, int length,
		enum copy_direction dir, u32 *crcp);

int copy_data(struct rxe_pd *pd, int access,
	      struct rxe_dma_info *dma, void *addr, int length,
	      enum copy_direction dir, u32 *crcp);

void *iova_to_vaddr(struct rxe_mr *mr, u64 iova, int length);

enum lookup_type {
	lookup_local,
	lookup_remote,
};

struct rxe_mr *lookup_mr(struct rxe_pd *pd, int access, u32 key,
			 enum lookup_type type);

int mr_check_range(struct rxe_mr *mr, u64 iova, size_t length);

void rxe_mr_cleanup(struct rxe_pool_entry *arg);

int advance_dma_data(struct rxe_dma_info *dma, unsigned int length);

/* rxe_net.c */
void rxe_loopback(struct sk_buff *skb);
int rxe_send(struct rxe_pkt_info *pkt, struct sk_buff *skb);
struct sk_buff *rxe_init_packet(struct rxe_dev *rxe, struct rxe_av *av,
				int paylen, struct rxe_pkt_info *pkt);
int rxe_prepare(struct rxe_pkt_info *pkt, struct sk_buff *skb, u32 *crc);
const char *rxe_parent_name(struct rxe_dev *rxe, unsigned int port_num);
int rxe_mcast_add(struct rxe_dev *rxe, union ib_gid *mgid);
int rxe_mcast_delete(struct rxe_dev *rxe, union ib_gid *mgid);

/* rxe_qp.c */
int rxe_qp_chk_init(struct rxe_dev *rxe, struct ib_qp_init_attr *init);

int rxe_qp_from_init(struct rxe_dev *rxe, struct rxe_qp *qp, struct rxe_pd *pd,
		     struct ib_qp_init_attr *init,
		     struct rxe_create_qp_resp __user *uresp,
		     struct ib_pd *ibpd, struct ib_udata *udata);

int rxe_qp_to_init(struct rxe_qp *qp, struct ib_qp_init_attr *init);

int rxe_qp_chk_attr(struct rxe_dev *rxe, struct rxe_qp *qp,
		    struct ib_qp_attr *attr, int mask);

int rxe_qp_from_attr(struct rxe_qp *qp, struct ib_qp_attr *attr,
		     int mask, struct ib_udata *udata);

int rxe_qp_to_attr(struct rxe_qp *qp, struct ib_qp_attr *attr, int mask);

void rxe_qp_error(struct rxe_qp *qp);

void rxe_qp_destroy(struct rxe_qp *qp);

void rxe_qp_cleanup(struct rxe_pool_entry *arg);

static inline int qp_num(struct rxe_qp *qp)
{
	return qp->ibqp.qp_num;
}

static inline enum ib_qp_type qp_type(struct rxe_qp *qp)
{
	return qp->ibqp.qp_type;
}

static inline enum ib_qp_state qp_state(struct rxe_qp *qp)
{
	return qp->attr.qp_state;
}

static inline int qp_mtu(struct rxe_qp *qp)
{
	if (qp->ibqp.qp_type == IB_QPT_RC || qp->ibqp.qp_type == IB_QPT_UC)
		return qp->attr.path_mtu;
	else
		return IB_MTU_4096;
}

static inline int rcv_wqe_size(int max_sge)
{
	return sizeof(struct rxe_recv_wqe) +
		max_sge * sizeof(struct ib_sge);
}

void free_rd_atomic_resource(struct rxe_qp *qp, struct resp_res *res);

static inline void rxe_advance_resp_resource(struct rxe_qp *qp)
{
	qp->resp.res_head++;
	if (unlikely(qp->resp.res_head == qp->attr.max_dest_rd_atomic))
		qp->resp.res_head = 0;
}

void retransmit_timer(struct timer_list *t);
void rnr_nak_timer(struct timer_list *t);

/* rxe_srq.c */
#define IB_SRQ_INIT_MASK (~IB_SRQ_LIMIT)

int rxe_srq_chk_attr(struct rxe_dev *rxe, struct rxe_srq *srq,
		     struct ib_srq_attr *attr, enum ib_srq_attr_mask mask);

int rxe_srq_from_init(struct rxe_dev *rxe, struct rxe_srq *srq,
		      struct ib_srq_init_attr *init, struct ib_udata *udata,
		      struct rxe_create_srq_resp __user *uresp);

int rxe_srq_from_attr(struct rxe_dev *rxe, struct rxe_srq *srq,
		      struct ib_srq_attr *attr, enum ib_srq_attr_mask mask,
		      struct rxe_modify_srq_cmd *ucmd, struct ib_udata *udata);

void rxe_dealloc(struct ib_device *ib_dev);

int rxe_completer(void *arg);
int rxe_requester(void *arg);
int rxe_responder(void *arg);

u32 rxe_icrc_hdr(struct rxe_pkt_info *pkt, struct sk_buff *skb);

void rxe_resp_queue_pkt(struct rxe_qp *qp, struct sk_buff *skb);

void rxe_comp_queue_pkt(struct rxe_qp *qp, struct sk_buff *skb);

static inline unsigned int wr_opcode_mask(int opcode, struct rxe_qp *qp)
{
	return rxe_wr_opcode_info[opcode].mask[qp->ibqp.qp_type];
}

static inline int rxe_xmit_packet(struct rxe_qp *qp, struct rxe_pkt_info *pkt,
				  struct sk_buff *skb)
{
	int err;
	int is_request = pkt->mask & RXE_REQ_MASK;
	struct rxe_dev *rxe = to_rdev(qp->ibqp.device);

	if ((is_request && (qp->req.state != QP_STATE_READY)) ||
	    (!is_request && (qp->resp.state != QP_STATE_READY))) {
		pr_info("Packet dropped. QP is not in ready state\n");
		goto drop;
	}

	if (pkt->mask & RXE_LOOPBACK_MASK) {
		memcpy(SKB_TO_PKT(skb), pkt, sizeof(*pkt));
		rxe_loopback(skb);
		err = 0;
	} else {
		err = rxe_send(pkt, skb);
	}

	if (err) {
		rxe->xmit_errors++;
		rxe_counter_inc(rxe, RXE_CNT_SEND_ERR);
		return err;
	}

	if ((qp_type(qp) != IB_QPT_RC) &&
	    (pkt->mask & RXE_END_MASK)) {
		pkt->wqe->state = wqe_state_done;
		rxe_run_task(&qp->comp.task, 1);
	}

	rxe_counter_inc(rxe, RXE_CNT_SENT_PKTS);
	goto done;

drop:
	kfree_skb(skb);
	err = 0;
done:
	return err;
}

#endif /* RXE_LOC_H */
