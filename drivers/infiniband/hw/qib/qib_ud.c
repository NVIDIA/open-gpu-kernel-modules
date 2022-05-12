/*
 * Copyright (c) 2012 - 2019 Intel Corporation.  All rights reserved.
 * Copyright (c) 2006, 2007, 2008, 2009 QLogic Corporation. All rights reserved.
 * Copyright (c) 2005, 2006 PathScale, Inc. All rights reserved.
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

#include <rdma/ib_smi.h>
#include <rdma/ib_verbs.h>

#include "qib.h"
#include "qib_mad.h"

/**
 * qib_ud_loopback - handle send on loopback QPs
 * @sqp: the sending QP
 * @swqe: the send work request
 *
 * This is called from qib_make_ud_req() to forward a WQE addressed
 * to the same HCA.
 * Note that the receive interrupt handler may be calling qib_ud_rcv()
 * while this is being called.
 */
static void qib_ud_loopback(struct rvt_qp *sqp, struct rvt_swqe *swqe)
{
	struct qib_ibport *ibp = to_iport(sqp->ibqp.device, sqp->port_num);
	struct qib_pportdata *ppd = ppd_from_ibp(ibp);
	struct qib_devdata *dd = ppd->dd;
	struct rvt_dev_info *rdi = &dd->verbs_dev.rdi;
	struct rvt_qp *qp;
	struct rdma_ah_attr *ah_attr;
	unsigned long flags;
	struct rvt_sge_state ssge;
	struct rvt_sge *sge;
	struct ib_wc wc;
	u32 length;
	enum ib_qp_type sqptype, dqptype;

	rcu_read_lock();
	qp = rvt_lookup_qpn(rdi, &ibp->rvp, rvt_get_swqe_remote_qpn(swqe));
	if (!qp) {
		ibp->rvp.n_pkt_drops++;
		goto drop;
	}

	sqptype = sqp->ibqp.qp_type == IB_QPT_GSI ?
			IB_QPT_UD : sqp->ibqp.qp_type;
	dqptype = qp->ibqp.qp_type == IB_QPT_GSI ?
			IB_QPT_UD : qp->ibqp.qp_type;

	if (dqptype != sqptype ||
	    !(ib_rvt_state_ops[qp->state] & RVT_PROCESS_RECV_OK)) {
		ibp->rvp.n_pkt_drops++;
		goto drop;
	}

	ah_attr = rvt_get_swqe_ah_attr(swqe);
	ppd = ppd_from_ibp(ibp);

	if (qp->ibqp.qp_num > 1) {
		u16 pkey1;
		u16 pkey2;
		u16 lid;

		pkey1 = qib_get_pkey(ibp, sqp->s_pkey_index);
		pkey2 = qib_get_pkey(ibp, qp->s_pkey_index);
		if (unlikely(!qib_pkey_ok(pkey1, pkey2))) {
			lid = ppd->lid | (rdma_ah_get_path_bits(ah_attr) &
					  ((1 << ppd->lmc) - 1));
			qib_bad_pkey(ibp, pkey1,
				     rdma_ah_get_sl(ah_attr),
				     sqp->ibqp.qp_num, qp->ibqp.qp_num,
				     cpu_to_be16(lid),
				     cpu_to_be16(rdma_ah_get_dlid(ah_attr)));
			goto drop;
		}
	}

	/*
	 * Check that the qkey matches (except for QP0, see 9.6.1.4.1).
	 * Qkeys with the high order bit set mean use the
	 * qkey from the QP context instead of the WR (see 10.2.5).
	 */
	if (qp->ibqp.qp_num) {
		u32 qkey;

		qkey = (int)rvt_get_swqe_remote_qkey(swqe) < 0 ?
			sqp->qkey : rvt_get_swqe_remote_qkey(swqe);
		if (unlikely(qkey != qp->qkey))
			goto drop;
	}

	/*
	 * A GRH is expected to precede the data even if not
	 * present on the wire.
	 */
	length = swqe->length;
	memset(&wc, 0, sizeof(wc));
	wc.byte_len = length + sizeof(struct ib_grh);

	if (swqe->wr.opcode == IB_WR_SEND_WITH_IMM) {
		wc.wc_flags = IB_WC_WITH_IMM;
		wc.ex.imm_data = swqe->wr.ex.imm_data;
	}

	spin_lock_irqsave(&qp->r_lock, flags);

	/*
	 * Get the next work request entry to find where to put the data.
	 */
	if (qp->r_flags & RVT_R_REUSE_SGE)
		qp->r_flags &= ~RVT_R_REUSE_SGE;
	else {
		int ret;

		ret = rvt_get_rwqe(qp, false);
		if (ret < 0) {
			rvt_rc_error(qp, IB_WC_LOC_QP_OP_ERR);
			goto bail_unlock;
		}
		if (!ret) {
			if (qp->ibqp.qp_num == 0)
				ibp->rvp.n_vl15_dropped++;
			goto bail_unlock;
		}
	}
	/* Silently drop packets which are too big. */
	if (unlikely(wc.byte_len > qp->r_len)) {
		qp->r_flags |= RVT_R_REUSE_SGE;
		ibp->rvp.n_pkt_drops++;
		goto bail_unlock;
	}

	if (rdma_ah_get_ah_flags(ah_attr) & IB_AH_GRH) {
		struct ib_grh grh;
		const struct ib_global_route *grd = rdma_ah_read_grh(ah_attr);

		qib_make_grh(ibp, &grh, grd, 0, 0);
		rvt_copy_sge(qp, &qp->r_sge, &grh,
			     sizeof(grh), true, false);
		wc.wc_flags |= IB_WC_GRH;
	} else
		rvt_skip_sge(&qp->r_sge, sizeof(struct ib_grh), true);
	ssge.sg_list = swqe->sg_list + 1;
	ssge.sge = *swqe->sg_list;
	ssge.num_sge = swqe->wr.num_sge;
	sge = &ssge.sge;
	while (length) {
		u32 len = rvt_get_sge_length(sge, length);

		rvt_copy_sge(qp, &qp->r_sge, sge->vaddr, len, true, false);
		sge->vaddr += len;
		sge->length -= len;
		sge->sge_length -= len;
		if (sge->sge_length == 0) {
			if (--ssge.num_sge)
				*sge = *ssge.sg_list++;
		} else if (sge->length == 0 && sge->mr->lkey) {
			if (++sge->n >= RVT_SEGSZ) {
				if (++sge->m >= sge->mr->mapsz)
					break;
				sge->n = 0;
			}
			sge->vaddr =
				sge->mr->map[sge->m]->segs[sge->n].vaddr;
			sge->length =
				sge->mr->map[sge->m]->segs[sge->n].length;
		}
		length -= len;
	}
	rvt_put_ss(&qp->r_sge);
	if (!test_and_clear_bit(RVT_R_WRID_VALID, &qp->r_aflags))
		goto bail_unlock;
	wc.wr_id = qp->r_wr_id;
	wc.status = IB_WC_SUCCESS;
	wc.opcode = IB_WC_RECV;
	wc.qp = &qp->ibqp;
	wc.src_qp = sqp->ibqp.qp_num;
	wc.pkey_index = qp->ibqp.qp_type == IB_QPT_GSI ?
		rvt_get_swqe_pkey_index(swqe) : 0;
	wc.slid = ppd->lid | (rdma_ah_get_path_bits(ah_attr) &
				((1 << ppd->lmc) - 1));
	wc.sl = rdma_ah_get_sl(ah_attr);
	wc.dlid_path_bits = rdma_ah_get_dlid(ah_attr) & ((1 << ppd->lmc) - 1);
	wc.port_num = qp->port_num;
	/* Signal completion event if the solicited bit is set. */
	rvt_recv_cq(qp, &wc, swqe->wr.send_flags & IB_SEND_SOLICITED);
	ibp->rvp.n_loop_pkts++;
bail_unlock:
	spin_unlock_irqrestore(&qp->r_lock, flags);
drop:
	rcu_read_unlock();
}

/**
 * qib_make_ud_req - construct a UD request packet
 * @qp: the QP
 * @flags: flags to modify and pass back to caller
 *
 * Assumes the s_lock is held.
 *
 * Return 1 if constructed; otherwise, return 0.
 */
int qib_make_ud_req(struct rvt_qp *qp, unsigned long *flags)
{
	struct qib_qp_priv *priv = qp->priv;
	struct ib_other_headers *ohdr;
	struct rdma_ah_attr *ah_attr;
	struct qib_pportdata *ppd;
	struct qib_ibport *ibp;
	struct rvt_swqe *wqe;
	u32 nwords;
	u32 extra_bytes;
	u32 bth0;
	u16 lrh0;
	u16 lid;
	int ret = 0;
	int next_cur;

	if (!(ib_rvt_state_ops[qp->state] & RVT_PROCESS_NEXT_SEND_OK)) {
		if (!(ib_rvt_state_ops[qp->state] & RVT_FLUSH_SEND))
			goto bail;
		/* We are in the error state, flush the work request. */
		if (qp->s_last == READ_ONCE(qp->s_head))
			goto bail;
		/* If DMAs are in progress, we can't flush immediately. */
		if (atomic_read(&priv->s_dma_busy)) {
			qp->s_flags |= RVT_S_WAIT_DMA;
			goto bail;
		}
		wqe = rvt_get_swqe_ptr(qp, qp->s_last);
		rvt_send_complete(qp, wqe, IB_WC_WR_FLUSH_ERR);
		goto done;
	}

	/* see post_one_send() */
	if (qp->s_cur == READ_ONCE(qp->s_head))
		goto bail;

	wqe = rvt_get_swqe_ptr(qp, qp->s_cur);
	next_cur = qp->s_cur + 1;
	if (next_cur >= qp->s_size)
		next_cur = 0;

	/* Construct the header. */
	ibp = to_iport(qp->ibqp.device, qp->port_num);
	ppd = ppd_from_ibp(ibp);
	ah_attr = rvt_get_swqe_ah_attr(wqe);
	if (rdma_ah_get_dlid(ah_attr) >= be16_to_cpu(IB_MULTICAST_LID_BASE)) {
		if (rdma_ah_get_dlid(ah_attr) !=
				be16_to_cpu(IB_LID_PERMISSIVE))
			this_cpu_inc(ibp->pmastats->n_multicast_xmit);
		else
			this_cpu_inc(ibp->pmastats->n_unicast_xmit);
	} else {
		this_cpu_inc(ibp->pmastats->n_unicast_xmit);
		lid = rdma_ah_get_dlid(ah_attr) & ~((1 << ppd->lmc) - 1);
		if (unlikely(lid == ppd->lid)) {
			unsigned long tflags = *flags;
			/*
			 * If DMAs are in progress, we can't generate
			 * a completion for the loopback packet since
			 * it would be out of order.
			 * XXX Instead of waiting, we could queue a
			 * zero length descriptor so we get a callback.
			 */
			if (atomic_read(&priv->s_dma_busy)) {
				qp->s_flags |= RVT_S_WAIT_DMA;
				goto bail;
			}
			qp->s_cur = next_cur;
			spin_unlock_irqrestore(&qp->s_lock, tflags);
			qib_ud_loopback(qp, wqe);
			spin_lock_irqsave(&qp->s_lock, tflags);
			*flags = tflags;
			rvt_send_complete(qp, wqe, IB_WC_SUCCESS);
			goto done;
		}
	}

	qp->s_cur = next_cur;
	extra_bytes = -wqe->length & 3;
	nwords = (wqe->length + extra_bytes) >> 2;

	/* header size in 32-bit words LRH+BTH+DETH = (8+12+8)/4. */
	qp->s_hdrwords = 7;
	qp->s_cur_size = wqe->length;
	qp->s_cur_sge = &qp->s_sge;
	qp->s_srate = rdma_ah_get_static_rate(ah_attr);
	qp->s_wqe = wqe;
	qp->s_sge.sge = wqe->sg_list[0];
	qp->s_sge.sg_list = wqe->sg_list + 1;
	qp->s_sge.num_sge = wqe->wr.num_sge;
	qp->s_sge.total_len = wqe->length;

	if (rdma_ah_get_ah_flags(ah_attr) & IB_AH_GRH) {
		/* Header size in 32-bit words. */
		qp->s_hdrwords += qib_make_grh(ibp, &priv->s_hdr->u.l.grh,
					       rdma_ah_read_grh(ah_attr),
					       qp->s_hdrwords, nwords);
		lrh0 = QIB_LRH_GRH;
		ohdr = &priv->s_hdr->u.l.oth;
		/*
		 * Don't worry about sending to locally attached multicast
		 * QPs.  It is unspecified by the spec. what happens.
		 */
	} else {
		/* Header size in 32-bit words. */
		lrh0 = QIB_LRH_BTH;
		ohdr = &priv->s_hdr->u.oth;
	}
	if (wqe->wr.opcode == IB_WR_SEND_WITH_IMM) {
		qp->s_hdrwords++;
		ohdr->u.ud.imm_data = wqe->wr.ex.imm_data;
		bth0 = IB_OPCODE_UD_SEND_ONLY_WITH_IMMEDIATE << 24;
	} else
		bth0 = IB_OPCODE_UD_SEND_ONLY << 24;
	lrh0 |= rdma_ah_get_sl(ah_attr) << 4;
	if (qp->ibqp.qp_type == IB_QPT_SMI)
		lrh0 |= 0xF000; /* Set VL (see ch. 13.5.3.1) */
	else
		lrh0 |= ibp->sl_to_vl[rdma_ah_get_sl(ah_attr)] << 12;
	priv->s_hdr->lrh[0] = cpu_to_be16(lrh0);
	priv->s_hdr->lrh[1] =
			cpu_to_be16(rdma_ah_get_dlid(ah_attr));  /* DEST LID */
	priv->s_hdr->lrh[2] =
			cpu_to_be16(qp->s_hdrwords + nwords + SIZE_OF_CRC);
	lid = ppd->lid;
	if (lid) {
		lid |= rdma_ah_get_path_bits(ah_attr) &
			((1 << ppd->lmc) - 1);
		priv->s_hdr->lrh[3] = cpu_to_be16(lid);
	} else
		priv->s_hdr->lrh[3] = IB_LID_PERMISSIVE;
	if (wqe->wr.send_flags & IB_SEND_SOLICITED)
		bth0 |= IB_BTH_SOLICITED;
	bth0 |= extra_bytes << 20;
	bth0 |= qp->ibqp.qp_type == IB_QPT_SMI ? QIB_DEFAULT_P_KEY :
		qib_get_pkey(ibp, qp->ibqp.qp_type == IB_QPT_GSI ?
			     rvt_get_swqe_pkey_index(wqe) : qp->s_pkey_index);
	ohdr->bth[0] = cpu_to_be32(bth0);
	/*
	 * Use the multicast QP if the destination LID is a multicast LID.
	 */
	ohdr->bth[1] = rdma_ah_get_dlid(ah_attr) >=
			be16_to_cpu(IB_MULTICAST_LID_BASE) &&
		rdma_ah_get_dlid(ah_attr) != be16_to_cpu(IB_LID_PERMISSIVE) ?
		cpu_to_be32(QIB_MULTICAST_QPN) :
		cpu_to_be32(rvt_get_swqe_remote_qpn(wqe));
	ohdr->bth[2] = cpu_to_be32(wqe->psn & QIB_PSN_MASK);
	/*
	 * Qkeys with the high order bit set mean use the
	 * qkey from the QP context instead of the WR (see 10.2.5).
	 */
	ohdr->u.ud.deth[0] =
		cpu_to_be32((int)rvt_get_swqe_remote_qkey(wqe) < 0 ? qp->qkey :
			    rvt_get_swqe_remote_qkey(wqe));
	ohdr->u.ud.deth[1] = cpu_to_be32(qp->ibqp.qp_num);

done:
	return 1;
bail:
	qp->s_flags &= ~RVT_S_BUSY;
	return ret;
}

static unsigned qib_lookup_pkey(struct qib_ibport *ibp, u16 pkey)
{
	struct qib_pportdata *ppd = ppd_from_ibp(ibp);
	struct qib_devdata *dd = ppd->dd;
	unsigned ctxt = ppd->hw_pidx;
	unsigned i;

	pkey &= 0x7fff;	/* remove limited/full membership bit */

	for (i = 0; i < ARRAY_SIZE(dd->rcd[ctxt]->pkeys); ++i)
		if ((dd->rcd[ctxt]->pkeys[i] & 0x7fff) == pkey)
			return i;

	/*
	 * Should not get here, this means hardware failed to validate pkeys.
	 * Punt and return index 0.
	 */
	return 0;
}

/**
 * qib_ud_rcv - receive an incoming UD packet
 * @ibp: the port the packet came in on
 * @hdr: the packet header
 * @has_grh: true if the packet has a GRH
 * @data: the packet data
 * @tlen: the packet length
 * @qp: the QP the packet came on
 *
 * This is called from qib_qp_rcv() to process an incoming UD packet
 * for the given QP.
 * Called at interrupt level.
 */
void qib_ud_rcv(struct qib_ibport *ibp, struct ib_header *hdr,
		int has_grh, void *data, u32 tlen, struct rvt_qp *qp)
{
	struct ib_other_headers *ohdr;
	int opcode;
	u32 hdrsize;
	u32 pad;
	struct ib_wc wc;
	u32 qkey;
	u32 src_qp;
	u16 dlid;

	/* Check for GRH */
	if (!has_grh) {
		ohdr = &hdr->u.oth;
		hdrsize = 8 + 12 + 8;   /* LRH + BTH + DETH */
	} else {
		ohdr = &hdr->u.l.oth;
		hdrsize = 8 + 40 + 12 + 8; /* LRH + GRH + BTH + DETH */
	}
	qkey = be32_to_cpu(ohdr->u.ud.deth[0]);
	src_qp = be32_to_cpu(ohdr->u.ud.deth[1]) & RVT_QPN_MASK;

	/*
	 * Get the number of bytes the message was padded by
	 * and drop incomplete packets.
	 */
	pad = (be32_to_cpu(ohdr->bth[0]) >> 20) & 3;
	if (unlikely(tlen < (hdrsize + pad + 4)))
		goto drop;

	tlen -= hdrsize + pad + 4;

	/*
	 * Check that the permissive LID is only used on QP0
	 * and the QKEY matches (see 9.6.1.4.1 and 9.6.1.5.1).
	 */
	if (qp->ibqp.qp_num) {
		if (unlikely(hdr->lrh[1] == IB_LID_PERMISSIVE ||
			     hdr->lrh[3] == IB_LID_PERMISSIVE))
			goto drop;
		if (qp->ibqp.qp_num > 1) {
			u16 pkey1, pkey2;

			pkey1 = be32_to_cpu(ohdr->bth[0]);
			pkey2 = qib_get_pkey(ibp, qp->s_pkey_index);
			if (unlikely(!qib_pkey_ok(pkey1, pkey2))) {
				qib_bad_pkey(ibp,
					     pkey1,
					     (be16_to_cpu(hdr->lrh[0]) >> 4) &
						0xF,
					     src_qp, qp->ibqp.qp_num,
					     hdr->lrh[3], hdr->lrh[1]);
				return;
			}
		}
		if (unlikely(qkey != qp->qkey))
			return;

		/* Drop invalid MAD packets (see 13.5.3.1). */
		if (unlikely(qp->ibqp.qp_num == 1 &&
			     (tlen != 256 ||
			      (be16_to_cpu(hdr->lrh[0]) >> 12) == 15)))
			goto drop;
	} else {
		struct ib_smp *smp;

		/* Drop invalid MAD packets (see 13.5.3.1). */
		if (tlen != 256 || (be16_to_cpu(hdr->lrh[0]) >> 12) != 15)
			goto drop;
		smp = (struct ib_smp *) data;
		if ((hdr->lrh[1] == IB_LID_PERMISSIVE ||
		     hdr->lrh[3] == IB_LID_PERMISSIVE) &&
		    smp->mgmt_class != IB_MGMT_CLASS_SUBN_DIRECTED_ROUTE)
			goto drop;
	}

	/*
	 * The opcode is in the low byte when its in network order
	 * (top byte when in host order).
	 */
	opcode = be32_to_cpu(ohdr->bth[0]) >> 24;
	if (qp->ibqp.qp_num > 1 &&
	    opcode == IB_OPCODE_UD_SEND_ONLY_WITH_IMMEDIATE) {
		wc.ex.imm_data = ohdr->u.ud.imm_data;
		wc.wc_flags = IB_WC_WITH_IMM;
	} else if (opcode == IB_OPCODE_UD_SEND_ONLY) {
		wc.ex.imm_data = 0;
		wc.wc_flags = 0;
	} else
		goto drop;

	/*
	 * A GRH is expected to precede the data even if not
	 * present on the wire.
	 */
	wc.byte_len = tlen + sizeof(struct ib_grh);

	/*
	 * Get the next work request entry to find where to put the data.
	 */
	if (qp->r_flags & RVT_R_REUSE_SGE)
		qp->r_flags &= ~RVT_R_REUSE_SGE;
	else {
		int ret;

		ret = rvt_get_rwqe(qp, false);
		if (ret < 0) {
			rvt_rc_error(qp, IB_WC_LOC_QP_OP_ERR);
			return;
		}
		if (!ret) {
			if (qp->ibqp.qp_num == 0)
				ibp->rvp.n_vl15_dropped++;
			return;
		}
	}
	/* Silently drop packets which are too big. */
	if (unlikely(wc.byte_len > qp->r_len)) {
		qp->r_flags |= RVT_R_REUSE_SGE;
		goto drop;
	}
	if (has_grh) {
		rvt_copy_sge(qp, &qp->r_sge, &hdr->u.l.grh,
			     sizeof(struct ib_grh), true, false);
		wc.wc_flags |= IB_WC_GRH;
	} else
		rvt_skip_sge(&qp->r_sge, sizeof(struct ib_grh), true);
	rvt_copy_sge(qp, &qp->r_sge, data, wc.byte_len - sizeof(struct ib_grh),
		     true, false);
	rvt_put_ss(&qp->r_sge);
	if (!test_and_clear_bit(RVT_R_WRID_VALID, &qp->r_aflags))
		return;
	wc.wr_id = qp->r_wr_id;
	wc.status = IB_WC_SUCCESS;
	wc.opcode = IB_WC_RECV;
	wc.vendor_err = 0;
	wc.qp = &qp->ibqp;
	wc.src_qp = src_qp;
	wc.pkey_index = qp->ibqp.qp_type == IB_QPT_GSI ?
		qib_lookup_pkey(ibp, be32_to_cpu(ohdr->bth[0])) : 0;
	wc.slid = be16_to_cpu(hdr->lrh[3]);
	wc.sl = (be16_to_cpu(hdr->lrh[0]) >> 4) & 0xF;
	dlid = be16_to_cpu(hdr->lrh[1]);
	/*
	 * Save the LMC lower bits if the destination LID is a unicast LID.
	 */
	wc.dlid_path_bits = dlid >= be16_to_cpu(IB_MULTICAST_LID_BASE) ? 0 :
		dlid & ((1 << ppd_from_ibp(ibp)->lmc) - 1);
	wc.port_num = qp->port_num;
	/* Signal completion event if the solicited bit is set. */
	rvt_recv_cq(qp, &wc, ib_bth_is_solicited(ohdr));
	return;

drop:
	ibp->rvp.n_pkt_drops++;
}
