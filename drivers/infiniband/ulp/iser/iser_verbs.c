/*
 * Copyright (c) 2004, 2005, 2006 Voltaire, Inc. All rights reserved.
 * Copyright (c) 2005, 2006 Cisco Systems.  All rights reserved.
 * Copyright (c) 2013-2014 Mellanox Technologies. All rights reserved.
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
 *	- Redistributions of source code must retain the above
 *	  copyright notice, this list of conditions and the following
 *	  disclaimer.
 *
 *	- Redistributions in binary form must reproduce the above
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
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/delay.h>

#include "iscsi_iser.h"

#define ISCSI_ISER_MAX_CONN	8
#define ISER_MAX_RX_LEN		(ISER_QP_MAX_RECV_DTOS * ISCSI_ISER_MAX_CONN)
#define ISER_MAX_TX_LEN		(ISER_QP_MAX_REQ_DTOS  * ISCSI_ISER_MAX_CONN)
#define ISER_MAX_CQ_LEN		(ISER_MAX_RX_LEN + ISER_MAX_TX_LEN + \
				 ISCSI_ISER_MAX_CONN)

static void iser_qp_event_callback(struct ib_event *cause, void *context)
{
	iser_err("qp event %s (%d)\n",
		 ib_event_msg(cause->event), cause->event);
}

static void iser_event_handler(struct ib_event_handler *handler,
				struct ib_event *event)
{
	iser_err("async event %s (%d) on device %s port %d\n",
		 ib_event_msg(event->event), event->event,
		dev_name(&event->device->dev), event->element.port_num);
}

/*
 * iser_create_device_ib_res - creates Protection Domain (PD), Completion
 * Queue (CQ), DMA Memory Region (DMA MR) with the device associated with
 * the adaptor.
 *
 * Return: 0 on success, -1 on failure
 */
static int iser_create_device_ib_res(struct iser_device *device)
{
	struct ib_device *ib_dev = device->ib_device;

	if (!(ib_dev->attrs.device_cap_flags & IB_DEVICE_MEM_MGT_EXTENSIONS)) {
		iser_err("IB device does not support memory registrations\n");
		return -1;
	}

	device->pd = ib_alloc_pd(ib_dev,
		iser_always_reg ? 0 : IB_PD_UNSAFE_GLOBAL_RKEY);
	if (IS_ERR(device->pd))
		goto pd_err;

	INIT_IB_EVENT_HANDLER(&device->event_handler, ib_dev,
			      iser_event_handler);
	ib_register_event_handler(&device->event_handler);
	return 0;

pd_err:
	iser_err("failed to allocate an IB resource\n");
	return -1;
}

/*
 * iser_free_device_ib_res - destroy/dealloc/dereg the DMA MR,
 * CQ and PD created with the device associated with the adaptor.
 */
static void iser_free_device_ib_res(struct iser_device *device)
{
	ib_unregister_event_handler(&device->event_handler);
	ib_dealloc_pd(device->pd);

	device->pd = NULL;
}

static struct iser_fr_desc *
iser_create_fastreg_desc(struct iser_device *device,
			 struct ib_pd *pd,
			 bool pi_enable,
			 unsigned int size)
{
	struct iser_fr_desc *desc;
	struct ib_device *ib_dev = device->ib_device;
	enum ib_mr_type mr_type;
	int ret;

	desc = kzalloc(sizeof(*desc), GFP_KERNEL);
	if (!desc)
		return ERR_PTR(-ENOMEM);

	if (ib_dev->attrs.device_cap_flags & IB_DEVICE_SG_GAPS_REG)
		mr_type = IB_MR_TYPE_SG_GAPS;
	else
		mr_type = IB_MR_TYPE_MEM_REG;

	desc->rsc.mr = ib_alloc_mr(pd, mr_type, size);
	if (IS_ERR(desc->rsc.mr)) {
		ret = PTR_ERR(desc->rsc.mr);
		iser_err("Failed to allocate ib_fast_reg_mr err=%d\n", ret);
		goto err_alloc_mr;
	}

	if (pi_enable) {
		desc->rsc.sig_mr = ib_alloc_mr_integrity(pd, size, size);
		if (IS_ERR(desc->rsc.sig_mr)) {
			ret = PTR_ERR(desc->rsc.sig_mr);
			iser_err("Failed to allocate sig_mr err=%d\n", ret);
			goto err_alloc_mr_integrity;
		}
	}
	desc->rsc.mr_valid = 0;

	return desc;

err_alloc_mr_integrity:
	ib_dereg_mr(desc->rsc.mr);
err_alloc_mr:
	kfree(desc);

	return ERR_PTR(ret);
}

static void iser_destroy_fastreg_desc(struct iser_fr_desc *desc)
{
	struct iser_reg_resources *res = &desc->rsc;

	ib_dereg_mr(res->mr);
	if (res->sig_mr) {
		ib_dereg_mr(res->sig_mr);
		res->sig_mr = NULL;
	}
	kfree(desc);
}

/**
 * iser_alloc_fastreg_pool - Creates pool of fast_reg descriptors
 * for fast registration work requests.
 * @ib_conn: connection RDMA resources
 * @cmds_max: max number of SCSI commands for this connection
 * @size: max number of pages per map request
 *
 * Return: 0 on success, or errno code on failure
 */
int iser_alloc_fastreg_pool(struct ib_conn *ib_conn,
			    unsigned cmds_max,
			    unsigned int size)
{
	struct iser_device *device = ib_conn->device;
	struct iser_fr_pool *fr_pool = &ib_conn->fr_pool;
	struct iser_fr_desc *desc;
	int i, ret;

	INIT_LIST_HEAD(&fr_pool->list);
	INIT_LIST_HEAD(&fr_pool->all_list);
	spin_lock_init(&fr_pool->lock);
	fr_pool->size = 0;
	for (i = 0; i < cmds_max; i++) {
		desc = iser_create_fastreg_desc(device, device->pd,
						ib_conn->pi_support, size);
		if (IS_ERR(desc)) {
			ret = PTR_ERR(desc);
			goto err;
		}

		list_add_tail(&desc->list, &fr_pool->list);
		list_add_tail(&desc->all_list, &fr_pool->all_list);
		fr_pool->size++;
	}

	return 0;

err:
	iser_free_fastreg_pool(ib_conn);
	return ret;
}

/**
 * iser_free_fastreg_pool - releases the pool of fast_reg descriptors
 * @ib_conn: connection RDMA resources
 */
void iser_free_fastreg_pool(struct ib_conn *ib_conn)
{
	struct iser_fr_pool *fr_pool = &ib_conn->fr_pool;
	struct iser_fr_desc *desc, *tmp;
	int i = 0;

	if (list_empty(&fr_pool->all_list))
		return;

	iser_info("freeing conn %p fr pool\n", ib_conn);

	list_for_each_entry_safe(desc, tmp, &fr_pool->all_list, all_list) {
		list_del(&desc->all_list);
		iser_destroy_fastreg_desc(desc);
		++i;
	}

	if (i < fr_pool->size)
		iser_warn("pool still has %d regions registered\n",
			  fr_pool->size - i);
}

/*
 * iser_create_ib_conn_res - Queue-Pair (QP)
 *
 * Return: 0 on success, -1 on failure
 */
static int iser_create_ib_conn_res(struct ib_conn *ib_conn)
{
	struct iser_conn *iser_conn = to_iser_conn(ib_conn);
	struct iser_device	*device;
	struct ib_device	*ib_dev;
	struct ib_qp_init_attr	init_attr;
	int			ret = -ENOMEM;
	unsigned int max_send_wr, cq_size;

	BUG_ON(ib_conn->device == NULL);

	device = ib_conn->device;
	ib_dev = device->ib_device;

	if (ib_conn->pi_support)
		max_send_wr = ISER_QP_SIG_MAX_REQ_DTOS + 1;
	else
		max_send_wr = ISER_QP_MAX_REQ_DTOS + 1;
	max_send_wr = min_t(unsigned int, max_send_wr,
			    (unsigned int)ib_dev->attrs.max_qp_wr);

	cq_size = max_send_wr + ISER_QP_MAX_RECV_DTOS;
	ib_conn->cq = ib_cq_pool_get(ib_dev, cq_size, -1, IB_POLL_SOFTIRQ);
	if (IS_ERR(ib_conn->cq)) {
		ret = PTR_ERR(ib_conn->cq);
		goto cq_err;
	}
	ib_conn->cq_size = cq_size;

	memset(&init_attr, 0, sizeof(init_attr));

	init_attr.event_handler = iser_qp_event_callback;
	init_attr.qp_context	= (void *)ib_conn;
	init_attr.send_cq	= ib_conn->cq;
	init_attr.recv_cq	= ib_conn->cq;
	init_attr.cap.max_recv_wr  = ISER_QP_MAX_RECV_DTOS;
	init_attr.cap.max_send_sge = 2;
	init_attr.cap.max_recv_sge = 1;
	init_attr.sq_sig_type	= IB_SIGNAL_REQ_WR;
	init_attr.qp_type	= IB_QPT_RC;
	init_attr.cap.max_send_wr = max_send_wr;
	if (ib_conn->pi_support)
		init_attr.create_flags |= IB_QP_CREATE_INTEGRITY_EN;
	iser_conn->max_cmds = ISER_GET_MAX_XMIT_CMDS(max_send_wr - 1);

	ret = rdma_create_qp(ib_conn->cma_id, device->pd, &init_attr);
	if (ret)
		goto out_err;

	ib_conn->qp = ib_conn->cma_id->qp;
	iser_info("setting conn %p cma_id %p qp %p max_send_wr %d\n",
		  ib_conn, ib_conn->cma_id,
		  ib_conn->cma_id->qp, max_send_wr);
	return ret;

out_err:
	ib_cq_pool_put(ib_conn->cq, ib_conn->cq_size);
cq_err:
	iser_err("unable to alloc mem or create resource, err %d\n", ret);

	return ret;
}

/*
 * based on the resolved device node GUID see if there already allocated
 * device for this device. If there's no such, create one.
 */
static
struct iser_device *iser_device_find_by_ib_device(struct rdma_cm_id *cma_id)
{
	struct iser_device *device;

	mutex_lock(&ig.device_list_mutex);

	list_for_each_entry(device, &ig.device_list, ig_list)
		/* find if there's a match using the node GUID */
		if (device->ib_device->node_guid == cma_id->device->node_guid)
			goto inc_refcnt;

	device = kzalloc(sizeof *device, GFP_KERNEL);
	if (device == NULL)
		goto out;

	/* assign this device to the device */
	device->ib_device = cma_id->device;
	/* init the device and link it into ig device list */
	if (iser_create_device_ib_res(device)) {
		kfree(device);
		device = NULL;
		goto out;
	}
	list_add(&device->ig_list, &ig.device_list);

inc_refcnt:
	device->refcount++;
out:
	mutex_unlock(&ig.device_list_mutex);
	return device;
}

/* if there's no demand for this device, release it */
static void iser_device_try_release(struct iser_device *device)
{
	mutex_lock(&ig.device_list_mutex);
	device->refcount--;
	iser_info("device %p refcount %d\n", device, device->refcount);
	if (!device->refcount) {
		iser_free_device_ib_res(device);
		list_del(&device->ig_list);
		kfree(device);
	}
	mutex_unlock(&ig.device_list_mutex);
}

/*
 * Called with state mutex held
 */
static int iser_conn_state_comp_exch(struct iser_conn *iser_conn,
				     enum iser_conn_state comp,
				     enum iser_conn_state exch)
{
	int ret;

	ret = (iser_conn->state == comp);
	if (ret)
		iser_conn->state = exch;

	return ret;
}

void iser_release_work(struct work_struct *work)
{
	struct iser_conn *iser_conn;

	iser_conn = container_of(work, struct iser_conn, release_work);

	/* Wait for conn_stop to complete */
	wait_for_completion(&iser_conn->stop_completion);
	/* Wait for IB resouces cleanup to complete */
	wait_for_completion(&iser_conn->ib_completion);

	mutex_lock(&iser_conn->state_mutex);
	iser_conn->state = ISER_CONN_DOWN;
	mutex_unlock(&iser_conn->state_mutex);

	iser_conn_release(iser_conn);
}

/**
 * iser_free_ib_conn_res - release IB related resources
 * @iser_conn: iser connection struct
 * @destroy: indicator if we need to try to release the
 *     iser device and memory regoins pool (only iscsi
 *     shutdown and DEVICE_REMOVAL will use this).
 *
 * This routine is called with the iser state mutex held
 * so the cm_id removal is out of here. It is Safe to
 * be invoked multiple times.
 */
static void iser_free_ib_conn_res(struct iser_conn *iser_conn,
				  bool destroy)
{
	struct ib_conn *ib_conn = &iser_conn->ib_conn;
	struct iser_device *device = ib_conn->device;

	iser_info("freeing conn %p cma_id %p qp %p\n",
		  iser_conn, ib_conn->cma_id, ib_conn->qp);

	if (ib_conn->qp != NULL) {
		rdma_destroy_qp(ib_conn->cma_id);
		ib_cq_pool_put(ib_conn->cq, ib_conn->cq_size);
		ib_conn->qp = NULL;
	}

	if (destroy) {
		if (iser_conn->rx_descs)
			iser_free_rx_descriptors(iser_conn);

		if (device != NULL) {
			iser_device_try_release(device);
			ib_conn->device = NULL;
		}
	}
}

/**
 * iser_conn_release - Frees all conn objects and deallocs conn descriptor
 * @iser_conn: iSER connection context
 */
void iser_conn_release(struct iser_conn *iser_conn)
{
	struct ib_conn *ib_conn = &iser_conn->ib_conn;

	mutex_lock(&ig.connlist_mutex);
	list_del(&iser_conn->conn_list);
	mutex_unlock(&ig.connlist_mutex);

	mutex_lock(&iser_conn->state_mutex);
	/* In case we endup here without ep_disconnect being invoked. */
	if (iser_conn->state != ISER_CONN_DOWN) {
		iser_warn("iser conn %p state %d, expected state down.\n",
			  iser_conn, iser_conn->state);
		iscsi_destroy_endpoint(iser_conn->ep);
		iser_conn->state = ISER_CONN_DOWN;
	}
	/*
	 * In case we never got to bind stage, we still need to
	 * release IB resources (which is safe to call more than once).
	 */
	iser_free_ib_conn_res(iser_conn, true);
	mutex_unlock(&iser_conn->state_mutex);

	if (ib_conn->cma_id != NULL) {
		rdma_destroy_id(ib_conn->cma_id);
		ib_conn->cma_id = NULL;
	}

	kfree(iser_conn);
}

/**
 * iser_conn_terminate - triggers start of the disconnect procedures and
 * waits for them to be done
 * @iser_conn: iSER connection context
 *
 * Called with state mutex held
 */
int iser_conn_terminate(struct iser_conn *iser_conn)
{
	struct ib_conn *ib_conn = &iser_conn->ib_conn;
	int err = 0;

	/* terminate the iser conn only if the conn state is UP */
	if (!iser_conn_state_comp_exch(iser_conn, ISER_CONN_UP,
				       ISER_CONN_TERMINATING))
		return 0;

	iser_info("iser_conn %p state %d\n", iser_conn, iser_conn->state);

	/* suspend queuing of new iscsi commands */
	if (iser_conn->iscsi_conn)
		iscsi_suspend_queue(iser_conn->iscsi_conn);

	/*
	 * In case we didn't already clean up the cma_id (peer initiated
	 * a disconnection), we need to Cause the CMA to change the QP
	 * state to ERROR.
	 */
	if (ib_conn->cma_id) {
		err = rdma_disconnect(ib_conn->cma_id);
		if (err)
			iser_err("Failed to disconnect, conn: 0x%p err %d\n",
				 iser_conn, err);

		/* block until all flush errors are consumed */
		ib_drain_sq(ib_conn->qp);
	}

	return 1;
}

/*
 * Called with state mutex held
 */
static void iser_connect_error(struct rdma_cm_id *cma_id)
{
	struct iser_conn *iser_conn;

	iser_conn = (struct iser_conn *)cma_id->context;
	iser_conn->state = ISER_CONN_TERMINATING;
}

static void
iser_calc_scsi_params(struct iser_conn *iser_conn,
		      unsigned int max_sectors)
{
	struct iser_device *device = iser_conn->ib_conn.device;
	struct ib_device_attr *attr = &device->ib_device->attrs;
	unsigned short sg_tablesize, sup_sg_tablesize;
	unsigned short reserved_mr_pages;
	u32 max_num_sg;

	/*
	 * FRs without SG_GAPS can only map up to a (device) page per entry,
	 * but if the first entry is misaligned we'll end up using two entries
	 * (head and tail) for a single page worth data, so one additional
	 * entry is required.
	 */
	if (attr->device_cap_flags & IB_DEVICE_SG_GAPS_REG)
		reserved_mr_pages = 0;
	else
		reserved_mr_pages = 1;

	if (iser_conn->ib_conn.pi_support)
		max_num_sg = attr->max_pi_fast_reg_page_list_len;
	else
		max_num_sg = attr->max_fast_reg_page_list_len;

	sg_tablesize = DIV_ROUND_UP(max_sectors * SECTOR_SIZE, SZ_4K);
	sup_sg_tablesize = min_t(uint, ISCSI_ISER_MAX_SG_TABLESIZE,
				 max_num_sg - reserved_mr_pages);
	iser_conn->scsi_sg_tablesize = min(sg_tablesize, sup_sg_tablesize);
	iser_conn->pages_per_mr =
		iser_conn->scsi_sg_tablesize + reserved_mr_pages;
}

/*
 * Called with state mutex held
 */
static void iser_addr_handler(struct rdma_cm_id *cma_id)
{
	struct iser_device *device;
	struct iser_conn   *iser_conn;
	struct ib_conn   *ib_conn;
	int    ret;

	iser_conn = (struct iser_conn *)cma_id->context;
	if (iser_conn->state != ISER_CONN_PENDING)
		/* bailout */
		return;

	ib_conn = &iser_conn->ib_conn;
	device = iser_device_find_by_ib_device(cma_id);
	if (!device) {
		iser_err("device lookup/creation failed\n");
		iser_connect_error(cma_id);
		return;
	}

	ib_conn->device = device;

	/* connection T10-PI support */
	if (iser_pi_enable) {
		if (!(device->ib_device->attrs.device_cap_flags &
		      IB_DEVICE_INTEGRITY_HANDOVER)) {
			iser_warn("T10-PI requested but not supported on %s, "
				  "continue without T10-PI\n",
				  dev_name(&ib_conn->device->ib_device->dev));
			ib_conn->pi_support = false;
		} else {
			ib_conn->pi_support = true;
		}
	}

	iser_calc_scsi_params(iser_conn, iser_max_sectors);

	ret = rdma_resolve_route(cma_id, 1000);
	if (ret) {
		iser_err("resolve route failed: %d\n", ret);
		iser_connect_error(cma_id);
		return;
	}
}

/*
 * Called with state mutex held
 */
static void iser_route_handler(struct rdma_cm_id *cma_id)
{
	struct rdma_conn_param conn_param;
	int    ret;
	struct iser_cm_hdr req_hdr;
	struct iser_conn *iser_conn = (struct iser_conn *)cma_id->context;
	struct ib_conn *ib_conn = &iser_conn->ib_conn;
	struct ib_device *ib_dev = ib_conn->device->ib_device;

	if (iser_conn->state != ISER_CONN_PENDING)
		/* bailout */
		return;

	ret = iser_create_ib_conn_res(ib_conn);
	if (ret)
		goto failure;

	memset(&conn_param, 0, sizeof conn_param);
	conn_param.responder_resources = ib_dev->attrs.max_qp_rd_atom;
	conn_param.initiator_depth     = 1;
	conn_param.retry_count	       = 7;
	conn_param.rnr_retry_count     = 6;

	memset(&req_hdr, 0, sizeof(req_hdr));
	req_hdr.flags = ISER_ZBVA_NOT_SUP;
	if (!iser_always_reg)
		req_hdr.flags |= ISER_SEND_W_INV_NOT_SUP;
	conn_param.private_data	= (void *)&req_hdr;
	conn_param.private_data_len = sizeof(struct iser_cm_hdr);

	ret = rdma_connect_locked(cma_id, &conn_param);
	if (ret) {
		iser_err("failure connecting: %d\n", ret);
		goto failure;
	}

	return;
failure:
	iser_connect_error(cma_id);
}

static void iser_connected_handler(struct rdma_cm_id *cma_id,
				   const void *private_data)
{
	struct iser_conn *iser_conn;
	struct ib_qp_attr attr;
	struct ib_qp_init_attr init_attr;

	iser_conn = (struct iser_conn *)cma_id->context;
	if (iser_conn->state != ISER_CONN_PENDING)
		/* bailout */
		return;

	(void)ib_query_qp(cma_id->qp, &attr, ~0, &init_attr);
	iser_info("remote qpn:%x my qpn:%x\n", attr.dest_qp_num, cma_id->qp->qp_num);

	if (private_data) {
		u8 flags = *(u8 *)private_data;

		iser_conn->snd_w_inv = !(flags & ISER_SEND_W_INV_NOT_SUP);
	}

	iser_info("conn %p: negotiated %s invalidation\n",
		  iser_conn, iser_conn->snd_w_inv ? "remote" : "local");

	iser_conn->state = ISER_CONN_UP;
	complete(&iser_conn->up_completion);
}

static void iser_disconnected_handler(struct rdma_cm_id *cma_id)
{
	struct iser_conn *iser_conn = (struct iser_conn *)cma_id->context;

	if (iser_conn_terminate(iser_conn)) {
		if (iser_conn->iscsi_conn)
			iscsi_conn_failure(iser_conn->iscsi_conn,
					   ISCSI_ERR_CONN_FAILED);
		else
			iser_err("iscsi_iser connection isn't bound\n");
	}
}

static void iser_cleanup_handler(struct rdma_cm_id *cma_id,
				 bool destroy)
{
	struct iser_conn *iser_conn = (struct iser_conn *)cma_id->context;

	/*
	 * We are not guaranteed that we visited disconnected_handler
	 * by now, call it here to be safe that we handle CM drep
	 * and flush errors.
	 */
	iser_disconnected_handler(cma_id);
	iser_free_ib_conn_res(iser_conn, destroy);
	complete(&iser_conn->ib_completion);
}

static int iser_cma_handler(struct rdma_cm_id *cma_id, struct rdma_cm_event *event)
{
	struct iser_conn *iser_conn;
	int ret = 0;

	iser_conn = (struct iser_conn *)cma_id->context;
	iser_info("%s (%d): status %d conn %p id %p\n",
		  rdma_event_msg(event->event), event->event,
		  event->status, cma_id->context, cma_id);

	mutex_lock(&iser_conn->state_mutex);
	switch (event->event) {
	case RDMA_CM_EVENT_ADDR_RESOLVED:
		iser_addr_handler(cma_id);
		break;
	case RDMA_CM_EVENT_ROUTE_RESOLVED:
		iser_route_handler(cma_id);
		break;
	case RDMA_CM_EVENT_ESTABLISHED:
		iser_connected_handler(cma_id, event->param.conn.private_data);
		break;
	case RDMA_CM_EVENT_REJECTED:
		iser_info("Connection rejected: %s\n",
			 rdma_reject_msg(cma_id, event->status));
		fallthrough;
	case RDMA_CM_EVENT_ADDR_ERROR:
	case RDMA_CM_EVENT_ROUTE_ERROR:
	case RDMA_CM_EVENT_CONNECT_ERROR:
	case RDMA_CM_EVENT_UNREACHABLE:
		iser_connect_error(cma_id);
		break;
	case RDMA_CM_EVENT_DISCONNECTED:
	case RDMA_CM_EVENT_ADDR_CHANGE:
	case RDMA_CM_EVENT_TIMEWAIT_EXIT:
		iser_cleanup_handler(cma_id, false);
		break;
	case RDMA_CM_EVENT_DEVICE_REMOVAL:
		/*
		 * we *must* destroy the device as we cannot rely
		 * on iscsid to be around to initiate error handling.
		 * also if we are not in state DOWN implicitly destroy
		 * the cma_id.
		 */
		iser_cleanup_handler(cma_id, true);
		if (iser_conn->state != ISER_CONN_DOWN) {
			iser_conn->ib_conn.cma_id = NULL;
			ret = 1;
		}
		break;
	default:
		iser_err("Unexpected RDMA CM event: %s (%d)\n",
			 rdma_event_msg(event->event), event->event);
		break;
	}
	mutex_unlock(&iser_conn->state_mutex);

	return ret;
}

void iser_conn_init(struct iser_conn *iser_conn)
{
	struct ib_conn *ib_conn = &iser_conn->ib_conn;

	iser_conn->state = ISER_CONN_INIT;
	init_completion(&iser_conn->stop_completion);
	init_completion(&iser_conn->ib_completion);
	init_completion(&iser_conn->up_completion);
	INIT_LIST_HEAD(&iser_conn->conn_list);
	mutex_init(&iser_conn->state_mutex);

	ib_conn->post_recv_buf_count = 0;
	ib_conn->reg_cqe.done = iser_reg_comp;
}

 /**
 * starts the process of connecting to the target
 * sleeps until the connection is established or rejected
 */
int iser_connect(struct iser_conn   *iser_conn,
		 struct sockaddr    *src_addr,
		 struct sockaddr    *dst_addr,
		 int                 non_blocking)
{
	struct ib_conn *ib_conn = &iser_conn->ib_conn;
	int err = 0;

	mutex_lock(&iser_conn->state_mutex);

	sprintf(iser_conn->name, "%pISp", dst_addr);

	iser_info("connecting to: %s\n", iser_conn->name);

	/* the device is known only --after-- address resolution */
	ib_conn->device = NULL;

	iser_conn->state = ISER_CONN_PENDING;

	ib_conn->cma_id = rdma_create_id(&init_net, iser_cma_handler,
					 (void *)iser_conn,
					 RDMA_PS_TCP, IB_QPT_RC);
	if (IS_ERR(ib_conn->cma_id)) {
		err = PTR_ERR(ib_conn->cma_id);
		iser_err("rdma_create_id failed: %d\n", err);
		goto id_failure;
	}

	err = rdma_resolve_addr(ib_conn->cma_id, src_addr, dst_addr, 1000);
	if (err) {
		iser_err("rdma_resolve_addr failed: %d\n", err);
		goto addr_failure;
	}

	if (!non_blocking) {
		wait_for_completion_interruptible(&iser_conn->up_completion);

		if (iser_conn->state != ISER_CONN_UP) {
			err =  -EIO;
			goto connect_failure;
		}
	}
	mutex_unlock(&iser_conn->state_mutex);

	mutex_lock(&ig.connlist_mutex);
	list_add(&iser_conn->conn_list, &ig.connlist);
	mutex_unlock(&ig.connlist_mutex);
	return 0;

id_failure:
	ib_conn->cma_id = NULL;
addr_failure:
	iser_conn->state = ISER_CONN_DOWN;
connect_failure:
	mutex_unlock(&iser_conn->state_mutex);
	iser_conn_release(iser_conn);
	return err;
}

int iser_post_recvl(struct iser_conn *iser_conn)
{
	struct ib_conn *ib_conn = &iser_conn->ib_conn;
	struct iser_login_desc *desc = &iser_conn->login_desc;
	struct ib_recv_wr wr;
	int ib_ret;

	desc->sge.addr = desc->rsp_dma;
	desc->sge.length = ISER_RX_LOGIN_SIZE;
	desc->sge.lkey = ib_conn->device->pd->local_dma_lkey;

	desc->cqe.done = iser_login_rsp;
	wr.wr_cqe = &desc->cqe;
	wr.sg_list = &desc->sge;
	wr.num_sge = 1;
	wr.next = NULL;

	ib_conn->post_recv_buf_count++;
	ib_ret = ib_post_recv(ib_conn->qp, &wr, NULL);
	if (ib_ret) {
		iser_err("ib_post_recv failed ret=%d\n", ib_ret);
		ib_conn->post_recv_buf_count--;
	}

	return ib_ret;
}

int iser_post_recvm(struct iser_conn *iser_conn, int count)
{
	struct ib_conn *ib_conn = &iser_conn->ib_conn;
	unsigned int my_rx_head = iser_conn->rx_desc_head;
	struct iser_rx_desc *rx_desc;
	struct ib_recv_wr *wr;
	int i, ib_ret;

	for (wr = ib_conn->rx_wr, i = 0; i < count; i++, wr++) {
		rx_desc = &iser_conn->rx_descs[my_rx_head];
		rx_desc->cqe.done = iser_task_rsp;
		wr->wr_cqe = &rx_desc->cqe;
		wr->sg_list = &rx_desc->rx_sg;
		wr->num_sge = 1;
		wr->next = wr + 1;
		my_rx_head = (my_rx_head + 1) & iser_conn->qp_max_recv_dtos_mask;
	}

	wr--;
	wr->next = NULL; /* mark end of work requests list */

	ib_conn->post_recv_buf_count += count;
	ib_ret = ib_post_recv(ib_conn->qp, ib_conn->rx_wr, NULL);
	if (unlikely(ib_ret)) {
		iser_err("ib_post_recv failed ret=%d\n", ib_ret);
		ib_conn->post_recv_buf_count -= count;
	} else
		iser_conn->rx_desc_head = my_rx_head;

	return ib_ret;
}


/**
 * iser_post_send - Initiate a Send DTO operation
 * @ib_conn: connection RDMA resources
 * @tx_desc: iSER TX descriptor
 * @signal: true to send work request as SIGNALED
 *
 * Return: 0 on success, -1 on failure
 */
int iser_post_send(struct ib_conn *ib_conn, struct iser_tx_desc *tx_desc,
		   bool signal)
{
	struct ib_send_wr *wr = &tx_desc->send_wr;
	struct ib_send_wr *first_wr;
	int ib_ret;

	ib_dma_sync_single_for_device(ib_conn->device->ib_device,
				      tx_desc->dma_addr, ISER_HEADERS_LEN,
				      DMA_TO_DEVICE);

	wr->next = NULL;
	wr->wr_cqe = &tx_desc->cqe;
	wr->sg_list = tx_desc->tx_sg;
	wr->num_sge = tx_desc->num_sge;
	wr->opcode = IB_WR_SEND;
	wr->send_flags = signal ? IB_SEND_SIGNALED : 0;

	if (tx_desc->inv_wr.next)
		first_wr = &tx_desc->inv_wr;
	else if (tx_desc->reg_wr.wr.next)
		first_wr = &tx_desc->reg_wr.wr;
	else
		first_wr = wr;

	ib_ret = ib_post_send(ib_conn->qp, first_wr, NULL);
	if (unlikely(ib_ret))
		iser_err("ib_post_send failed, ret:%d opcode:%d\n",
			 ib_ret, wr->opcode);

	return ib_ret;
}

u8 iser_check_task_pi_status(struct iscsi_iser_task *iser_task,
			     enum iser_data_dir cmd_dir, sector_t *sector)
{
	struct iser_mem_reg *reg = &iser_task->rdma_reg[cmd_dir];
	struct iser_fr_desc *desc = reg->mem_h;
	unsigned long sector_size = iser_task->sc->device->sector_size;
	struct ib_mr_status mr_status;
	int ret;

	if (desc && desc->sig_protected) {
		desc->sig_protected = false;
		ret = ib_check_mr_status(desc->rsc.sig_mr,
					 IB_MR_CHECK_SIG_STATUS, &mr_status);
		if (ret) {
			iser_err("ib_check_mr_status failed, ret %d\n", ret);
			/* Not a lot we can do, return ambiguous guard error */
			*sector = 0;
			return 0x1;
		}

		if (mr_status.fail_status & IB_MR_CHECK_SIG_STATUS) {
			sector_t sector_off = mr_status.sig_err.sig_err_offset;

			sector_div(sector_off, sector_size + 8);
			*sector = scsi_get_lba(iser_task->sc) + sector_off;

			iser_err("PI error found type %d at sector %llx "
			       "expected %x vs actual %x\n",
			       mr_status.sig_err.err_type,
			       (unsigned long long)*sector,
			       mr_status.sig_err.expected,
			       mr_status.sig_err.actual);

			switch (mr_status.sig_err.err_type) {
			case IB_SIG_BAD_GUARD:
				return 0x1;
			case IB_SIG_BAD_REFTAG:
				return 0x3;
			case IB_SIG_BAD_APPTAG:
				return 0x2;
			}
		}
	}

	return 0;
}

void iser_err_comp(struct ib_wc *wc, const char *type)
{
	if (wc->status != IB_WC_WR_FLUSH_ERR) {
		struct iser_conn *iser_conn = to_iser_conn(wc->qp->qp_context);

		iser_err("%s failure: %s (%d) vend_err %#x\n", type,
			 ib_wc_status_msg(wc->status), wc->status,
			 wc->vendor_err);

		if (iser_conn->iscsi_conn)
			iscsi_conn_failure(iser_conn->iscsi_conn,
					   ISCSI_ERR_CONN_FAILED);
	} else {
		iser_dbg("%s failure: %s (%d)\n", type,
			 ib_wc_status_msg(wc->status), wc->status);
	}
}
