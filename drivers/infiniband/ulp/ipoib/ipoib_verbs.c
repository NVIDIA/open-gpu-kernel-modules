/*
 * Copyright (c) 2004, 2005 Topspin Communications.  All rights reserved.
 * Copyright (c) 2005 Mellanox Technologies. All rights reserved.
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

#include <linux/slab.h>

#include "ipoib.h"

int ipoib_mcast_attach(struct net_device *dev, struct ib_device *hca,
		       union ib_gid *mgid, u16 mlid, int set_qkey, u32 qkey)
{
	struct ipoib_dev_priv *priv = ipoib_priv(dev);
	struct ib_qp_attr *qp_attr = NULL;
	int ret;
	u16 pkey_index;

	if (ib_find_pkey(priv->ca, priv->port, priv->pkey, &pkey_index)) {
		clear_bit(IPOIB_PKEY_ASSIGNED, &priv->flags);
		ret = -ENXIO;
		goto out;
	}
	set_bit(IPOIB_PKEY_ASSIGNED, &priv->flags);

	if (set_qkey) {
		ret = -ENOMEM;
		qp_attr = kmalloc(sizeof(*qp_attr), GFP_KERNEL);
		if (!qp_attr)
			goto out;

		/* set correct QKey for QP */
		qp_attr->qkey = qkey;
		ret = ib_modify_qp(priv->qp, qp_attr, IB_QP_QKEY);
		if (ret) {
			ipoib_warn(priv, "failed to modify QP, ret = %d\n", ret);
			goto out;
		}
	}

	/* attach QP to multicast group */
	ret = ib_attach_mcast(priv->qp, mgid, mlid);
	if (ret)
		ipoib_warn(priv, "failed to attach to multicast group, ret = %d\n", ret);

out:
	kfree(qp_attr);
	return ret;
}

int ipoib_mcast_detach(struct net_device *dev, struct ib_device *hca,
		       union ib_gid *mgid, u16 mlid)
{
	struct ipoib_dev_priv *priv = ipoib_priv(dev);
	int ret;

	ret = ib_detach_mcast(priv->qp, mgid, mlid);

	return ret;
}

int ipoib_init_qp(struct net_device *dev)
{
	struct ipoib_dev_priv *priv = ipoib_priv(dev);
	int ret;
	struct ib_qp_attr qp_attr;
	int attr_mask;

	if (!test_bit(IPOIB_PKEY_ASSIGNED, &priv->flags))
		return -1;

	qp_attr.qp_state = IB_QPS_INIT;
	qp_attr.qkey = 0;
	qp_attr.port_num = priv->port;
	qp_attr.pkey_index = priv->pkey_index;
	attr_mask =
	    IB_QP_QKEY |
	    IB_QP_PORT |
	    IB_QP_PKEY_INDEX |
	    IB_QP_STATE;
	ret = ib_modify_qp(priv->qp, &qp_attr, attr_mask);
	if (ret) {
		ipoib_warn(priv, "failed to modify QP to init, ret = %d\n", ret);
		goto out_fail;
	}

	qp_attr.qp_state = IB_QPS_RTR;
	/* Can't set this in a INIT->RTR transition */
	attr_mask &= ~IB_QP_PORT;
	ret = ib_modify_qp(priv->qp, &qp_attr, attr_mask);
	if (ret) {
		ipoib_warn(priv, "failed to modify QP to RTR, ret = %d\n", ret);
		goto out_fail;
	}

	qp_attr.qp_state = IB_QPS_RTS;
	qp_attr.sq_psn = 0;
	attr_mask |= IB_QP_SQ_PSN;
	attr_mask &= ~IB_QP_PKEY_INDEX;
	ret = ib_modify_qp(priv->qp, &qp_attr, attr_mask);
	if (ret) {
		ipoib_warn(priv, "failed to modify QP to RTS, ret = %d\n", ret);
		goto out_fail;
	}

	return 0;

out_fail:
	qp_attr.qp_state = IB_QPS_RESET;
	if (ib_modify_qp(priv->qp, &qp_attr, IB_QP_STATE))
		ipoib_warn(priv, "Failed to modify QP to RESET state\n");

	return ret;
}

int ipoib_transport_dev_init(struct net_device *dev, struct ib_device *ca)
{
	struct ipoib_dev_priv *priv = ipoib_priv(dev);
	struct ib_qp_init_attr init_attr = {
		.cap = {
			.max_send_wr  = ipoib_sendq_size,
			.max_recv_wr  = ipoib_recvq_size,
			.max_send_sge = min_t(u32, priv->ca->attrs.max_send_sge,
					      MAX_SKB_FRAGS + 1),
			.max_recv_sge = IPOIB_UD_RX_SG
		},
		.sq_sig_type = IB_SIGNAL_ALL_WR,
		.qp_type     = IB_QPT_UD
	};
	struct ib_cq_init_attr cq_attr = {};

	int ret, size, req_vec;
	int i;
	static atomic_t counter;

	size = ipoib_recvq_size + 1;
	ret = ipoib_cm_dev_init(dev);
	if (!ret) {
		size += ipoib_sendq_size;
		if (ipoib_cm_has_srq(dev))
			size += ipoib_recvq_size + 1; /* 1 extra for rx_drain_qp */
		else
			size += ipoib_recvq_size * ipoib_max_conn_qp;
	} else
		if (ret != -EOPNOTSUPP)
			return ret;

	req_vec = atomic_inc_return(&counter) * 2;
	cq_attr.cqe = size;
	cq_attr.comp_vector = req_vec % priv->ca->num_comp_vectors;
	priv->recv_cq = ib_create_cq(priv->ca, ipoib_ib_rx_completion, NULL,
				     priv, &cq_attr);
	if (IS_ERR(priv->recv_cq)) {
		pr_warn("%s: failed to create receive CQ\n", ca->name);
		goto out_cm_dev_cleanup;
	}

	cq_attr.cqe = ipoib_sendq_size;
	cq_attr.comp_vector = (req_vec + 1) % priv->ca->num_comp_vectors;
	priv->send_cq = ib_create_cq(priv->ca, ipoib_ib_tx_completion, NULL,
				     priv, &cq_attr);
	if (IS_ERR(priv->send_cq)) {
		pr_warn("%s: failed to create send CQ\n", ca->name);
		goto out_free_recv_cq;
	}

	if (ib_req_notify_cq(priv->recv_cq, IB_CQ_NEXT_COMP))
		goto out_free_send_cq;

	init_attr.send_cq = priv->send_cq;
	init_attr.recv_cq = priv->recv_cq;

	if (priv->hca_caps & IB_DEVICE_UD_TSO)
		init_attr.create_flags |= IB_QP_CREATE_IPOIB_UD_LSO;

	if (priv->hca_caps & IB_DEVICE_BLOCK_MULTICAST_LOOPBACK)
		init_attr.create_flags |= IB_QP_CREATE_BLOCK_MULTICAST_LOOPBACK;

	if (priv->hca_caps & IB_DEVICE_MANAGED_FLOW_STEERING)
		init_attr.create_flags |= IB_QP_CREATE_NETIF_QP;

	if (priv->hca_caps & IB_DEVICE_RDMA_NETDEV_OPA)
		init_attr.create_flags |= IB_QP_CREATE_NETDEV_USE;

	priv->qp = ib_create_qp(priv->pd, &init_attr);
	if (IS_ERR(priv->qp)) {
		pr_warn("%s: failed to create QP\n", ca->name);
		goto out_free_send_cq;
	}

	if (ib_req_notify_cq(priv->send_cq, IB_CQ_NEXT_COMP))
		goto out_free_send_cq;

	for (i = 0; i < MAX_SKB_FRAGS + 1; ++i)
		priv->tx_sge[i].lkey = priv->pd->local_dma_lkey;

	priv->tx_wr.wr.opcode		= IB_WR_SEND;
	priv->tx_wr.wr.sg_list		= priv->tx_sge;
	priv->tx_wr.wr.send_flags	= IB_SEND_SIGNALED;

	priv->rx_sge[0].lkey = priv->pd->local_dma_lkey;

	priv->rx_sge[0].length = IPOIB_UD_BUF_SIZE(priv->max_ib_mtu);
	priv->rx_wr.num_sge = 1;

	priv->rx_wr.next = NULL;
	priv->rx_wr.sg_list = priv->rx_sge;

	if (init_attr.cap.max_send_sge > 1)
		dev->features |= NETIF_F_SG;

	priv->max_send_sge = init_attr.cap.max_send_sge;

	return 0;

out_free_send_cq:
	ib_destroy_cq(priv->send_cq);

out_free_recv_cq:
	ib_destroy_cq(priv->recv_cq);

out_cm_dev_cleanup:
	ipoib_cm_dev_cleanup(dev);

	return -ENODEV;
}

void ipoib_transport_dev_cleanup(struct net_device *dev)
{
	struct ipoib_dev_priv *priv = ipoib_priv(dev);

	if (priv->qp) {
		if (ib_destroy_qp(priv->qp))
			ipoib_warn(priv, "ib_qp_destroy failed\n");

		priv->qp = NULL;
	}

	ib_destroy_cq(priv->send_cq);
	ib_destroy_cq(priv->recv_cq);
}

void ipoib_event(struct ib_event_handler *handler,
		 struct ib_event *record)
{
	struct ipoib_dev_priv *priv =
		container_of(handler, struct ipoib_dev_priv, event_handler);

	if (record->element.port_num != priv->port)
		return;

	ipoib_dbg(priv, "Event %d on device %s port %d\n", record->event,
		  dev_name(&record->device->dev), record->element.port_num);

	if (record->event == IB_EVENT_CLIENT_REREGISTER) {
		queue_work(ipoib_workqueue, &priv->flush_light);
	} else if (record->event == IB_EVENT_PORT_ERR ||
		   record->event == IB_EVENT_PORT_ACTIVE ||
		   record->event == IB_EVENT_LID_CHANGE) {
		queue_work(ipoib_workqueue, &priv->flush_normal);
	} else if (record->event == IB_EVENT_PKEY_CHANGE) {
		queue_work(ipoib_workqueue, &priv->flush_heavy);
	} else if (record->event == IB_EVENT_GID_CHANGE &&
		   !test_bit(IPOIB_FLAG_DEV_ADDR_SET, &priv->flags)) {
		queue_work(ipoib_workqueue, &priv->flush_light);
	}
}
