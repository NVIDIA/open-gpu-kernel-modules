/*******************************************************************************
*
* Copyright (c) 2015-2016 Intel Corporation.  All rights reserved.
*
* This software is available to you under a choice of one of two
* licenses.  You may choose to be licensed under the terms of the GNU
* General Public License (GPL) Version 2, available from the file
* COPYING in the main directory of this source tree, or the
* OpenFabrics.org BSD license below:
*
*   Redistribution and use in source and binary forms, with or
*   without modification, are permitted provided that the following
*   conditions are met:
*
*    - Redistributions of source code must retain the above
*	copyright notice, this list of conditions and the following
*	disclaimer.
*
*    - Redistributions in binary form must reproduce the above
*	copyright notice, this list of conditions and the following
*	disclaimer in the documentation and/or other materials
*	provided with the distribution.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
* BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
* ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*******************************************************************************/

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/random.h>
#include <linux/highmem.h>
#include <linux/time.h>
#include <linux/hugetlb.h>
#include <linux/irq.h>
#include <asm/byteorder.h>
#include <net/ip.h>
#include <rdma/ib_verbs.h>
#include <rdma/iw_cm.h>
#include <rdma/ib_user_verbs.h>
#include <rdma/ib_umem.h>
#include <rdma/uverbs_ioctl.h>
#include "i40iw.h"

/**
 * i40iw_query_device - get device attributes
 * @ibdev: device pointer from stack
 * @props: returning device attributes
 * @udata: user data
 */
static int i40iw_query_device(struct ib_device *ibdev,
			      struct ib_device_attr *props,
			      struct ib_udata *udata)
{
	struct i40iw_device *iwdev = to_iwdev(ibdev);

	if (udata->inlen || udata->outlen)
		return -EINVAL;
	memset(props, 0, sizeof(*props));
	ether_addr_copy((u8 *)&props->sys_image_guid, iwdev->netdev->dev_addr);
	props->fw_ver = i40iw_fw_major_ver(&iwdev->sc_dev) << 32 |
			i40iw_fw_minor_ver(&iwdev->sc_dev);
	props->device_cap_flags = iwdev->device_cap_flags;
	props->vendor_id = iwdev->ldev->pcidev->vendor;
	props->vendor_part_id = iwdev->ldev->pcidev->device;
	props->hw_ver = (u32)iwdev->sc_dev.hw_rev;
	props->max_mr_size = I40IW_MAX_OUTBOUND_MESSAGE_SIZE;
	props->max_qp = iwdev->max_qp - iwdev->used_qps;
	props->max_qp_wr = I40IW_MAX_QP_WRS;
	props->max_send_sge = I40IW_MAX_WQ_FRAGMENT_COUNT;
	props->max_recv_sge = I40IW_MAX_WQ_FRAGMENT_COUNT;
	props->max_cq = iwdev->max_cq - iwdev->used_cqs;
	props->max_cqe = iwdev->max_cqe;
	props->max_mr = iwdev->max_mr - iwdev->used_mrs;
	props->max_pd = iwdev->max_pd - iwdev->used_pds;
	props->max_sge_rd = I40IW_MAX_SGE_RD;
	props->max_qp_rd_atom = I40IW_MAX_IRD_SIZE;
	props->max_qp_init_rd_atom = props->max_qp_rd_atom;
	props->atomic_cap = IB_ATOMIC_NONE;
	props->max_fast_reg_page_list_len = I40IW_MAX_PAGES_PER_FMR;
	return 0;
}

/**
 * i40iw_query_port - get port attrubutes
 * @ibdev: device pointer from stack
 * @port: port number for query
 * @props: returning device attributes
 */
static int i40iw_query_port(struct ib_device *ibdev,
			    u32 port,
			    struct ib_port_attr *props)
{
	props->lid = 1;
	props->port_cap_flags = IB_PORT_CM_SUP | IB_PORT_REINIT_SUP |
		IB_PORT_VENDOR_CLASS_SUP | IB_PORT_BOOT_MGMT_SUP;
	props->gid_tbl_len = 1;
	props->active_width = IB_WIDTH_4X;
	props->active_speed = 1;
	props->max_msg_sz = I40IW_MAX_OUTBOUND_MESSAGE_SIZE;
	return 0;
}

/**
 * i40iw_alloc_ucontext - Allocate the user context data structure
 * @uctx: Uverbs context pointer from stack
 * @udata: user data
 *
 * This keeps track of all objects associated with a particular
 * user-mode client.
 */
static int i40iw_alloc_ucontext(struct ib_ucontext *uctx,
				struct ib_udata *udata)
{
	struct ib_device *ibdev = uctx->device;
	struct i40iw_device *iwdev = to_iwdev(ibdev);
	struct i40iw_alloc_ucontext_req req;
	struct i40iw_alloc_ucontext_resp uresp = {};
	struct i40iw_ucontext *ucontext = to_ucontext(uctx);

	if (ib_copy_from_udata(&req, udata, sizeof(req)))
		return -EINVAL;

	if (req.userspace_ver < 4 || req.userspace_ver > I40IW_ABI_VER) {
		i40iw_pr_err("Unsupported provider library version %u.\n", req.userspace_ver);
		return -EINVAL;
	}

	uresp.max_qps = iwdev->max_qp;
	uresp.max_pds = iwdev->max_pd;
	uresp.wq_size = iwdev->max_qp_wr * 2;
	uresp.kernel_ver = req.userspace_ver;

	ucontext->iwdev = iwdev;
	ucontext->abi_ver = req.userspace_ver;

	if (ib_copy_to_udata(udata, &uresp, sizeof(uresp)))
		return -EFAULT;

	INIT_LIST_HEAD(&ucontext->cq_reg_mem_list);
	spin_lock_init(&ucontext->cq_reg_mem_list_lock);
	INIT_LIST_HEAD(&ucontext->qp_reg_mem_list);
	spin_lock_init(&ucontext->qp_reg_mem_list_lock);

	return 0;
}

/**
 * i40iw_dealloc_ucontext - deallocate the user context data structure
 * @context: user context created during alloc
 */
static void i40iw_dealloc_ucontext(struct ib_ucontext *context)
{
	return;
}

/**
 * i40iw_mmap - user memory map
 * @context: context created during alloc
 * @vma: kernel info for user memory map
 */
static int i40iw_mmap(struct ib_ucontext *context, struct vm_area_struct *vma)
{
	struct i40iw_ucontext *ucontext = to_ucontext(context);
	u64 dbaddr;

	if (vma->vm_pgoff || vma->vm_end - vma->vm_start != PAGE_SIZE)
		return -EINVAL;

	dbaddr = I40IW_DB_ADDR_OFFSET + pci_resource_start(ucontext->iwdev->ldev->pcidev, 0);

	return rdma_user_mmap_io(context, vma, dbaddr >> PAGE_SHIFT, PAGE_SIZE,
				 pgprot_noncached(vma->vm_page_prot), NULL);
}

/**
 * i40iw_alloc_pd - allocate protection domain
 * @pd: PD pointer
 * @udata: user data
 */
static int i40iw_alloc_pd(struct ib_pd *pd, struct ib_udata *udata)
{
	struct i40iw_pd *iwpd = to_iwpd(pd);
	struct i40iw_device *iwdev = to_iwdev(pd->device);
	struct i40iw_sc_dev *dev = &iwdev->sc_dev;
	struct i40iw_alloc_pd_resp uresp;
	struct i40iw_sc_pd *sc_pd;
	u32 pd_id = 0;
	int err;

	if (iwdev->closing)
		return -ENODEV;

	err = i40iw_alloc_resource(iwdev, iwdev->allocated_pds,
				   iwdev->max_pd, &pd_id, &iwdev->next_pd);
	if (err) {
		i40iw_pr_err("alloc resource failed\n");
		return err;
	}

	sc_pd = &iwpd->sc_pd;

	if (udata) {
		struct i40iw_ucontext *ucontext = rdma_udata_to_drv_context(
			udata, struct i40iw_ucontext, ibucontext);
		dev->iw_pd_ops->pd_init(dev, sc_pd, pd_id, ucontext->abi_ver);
		memset(&uresp, 0, sizeof(uresp));
		uresp.pd_id = pd_id;
		if (ib_copy_to_udata(udata, &uresp, sizeof(uresp))) {
			err = -EFAULT;
			goto error;
		}
	} else {
		dev->iw_pd_ops->pd_init(dev, sc_pd, pd_id, -1);
	}

	i40iw_add_pdusecount(iwpd);
	return 0;

error:
	i40iw_free_resource(iwdev, iwdev->allocated_pds, pd_id);
	return err;
}

/**
 * i40iw_dealloc_pd - deallocate pd
 * @ibpd: ptr of pd to be deallocated
 * @udata: user data or null for kernel object
 */
static int i40iw_dealloc_pd(struct ib_pd *ibpd, struct ib_udata *udata)
{
	struct i40iw_pd *iwpd = to_iwpd(ibpd);
	struct i40iw_device *iwdev = to_iwdev(ibpd->device);

	i40iw_rem_pdusecount(iwpd, iwdev);
	return 0;
}

/**
 * i40iw_get_pbl - Retrieve pbl from a list given a virtual
 * address
 * @va: user virtual address
 * @pbl_list: pbl list to search in (QP's or CQ's)
 */
static struct i40iw_pbl *i40iw_get_pbl(unsigned long va,
				       struct list_head *pbl_list)
{
	struct i40iw_pbl *iwpbl;

	list_for_each_entry(iwpbl, pbl_list, list) {
		if (iwpbl->user_base == va) {
			iwpbl->on_list = false;
			list_del(&iwpbl->list);
			return iwpbl;
		}
	}
	return NULL;
}

/**
 * i40iw_free_qp_resources - free up memory resources for qp
 * @iwqp: qp ptr (user or kernel)
 */
void i40iw_free_qp_resources(struct i40iw_qp *iwqp)
{
	struct i40iw_pbl *iwpbl = &iwqp->iwpbl;
	struct i40iw_device *iwdev = iwqp->iwdev;
	u32 qp_num = iwqp->ibqp.qp_num;

	i40iw_ieq_cleanup_qp(iwdev->vsi.ieq, &iwqp->sc_qp);
	if (qp_num)
		i40iw_free_resource(iwdev, iwdev->allocated_qps, qp_num);
	if (iwpbl->pbl_allocated)
		i40iw_free_pble(iwdev->pble_rsrc, &iwpbl->pble_alloc);
	i40iw_free_dma_mem(iwdev->sc_dev.hw, &iwqp->q2_ctx_mem);
	i40iw_free_dma_mem(iwdev->sc_dev.hw, &iwqp->kqp.dma_mem);
	kfree(iwqp->kqp.wrid_mem);
	iwqp->kqp.wrid_mem = NULL;
	kfree(iwqp);
}

/**
 * i40iw_clean_cqes - clean cq entries for qp
 * @iwqp: qp ptr (user or kernel)
 * @iwcq: cq ptr
 */
static void i40iw_clean_cqes(struct i40iw_qp *iwqp, struct i40iw_cq *iwcq)
{
	struct i40iw_cq_uk *ukcq = &iwcq->sc_cq.cq_uk;

	ukcq->ops.iw_cq_clean(&iwqp->sc_qp.qp_uk, ukcq);
}

/**
 * i40iw_destroy_qp - destroy qp
 * @ibqp: qp's ib pointer also to get to device's qp address
 * @udata: user data
 */
static int i40iw_destroy_qp(struct ib_qp *ibqp, struct ib_udata *udata)
{
	struct i40iw_qp *iwqp = to_iwqp(ibqp);
	struct ib_qp_attr attr;
	struct i40iw_device *iwdev = iwqp->iwdev;

	memset(&attr, 0, sizeof(attr));

	iwqp->destroyed = 1;

	if (iwqp->ibqp_state >= IB_QPS_INIT && iwqp->ibqp_state < IB_QPS_RTS)
		i40iw_next_iw_state(iwqp, I40IW_QP_STATE_ERROR, 0, 0, 0);

	if (!iwqp->user_mode) {
		if (iwqp->iwscq) {
			i40iw_clean_cqes(iwqp, iwqp->iwscq);
			if (iwqp->iwrcq != iwqp->iwscq)
				i40iw_clean_cqes(iwqp, iwqp->iwrcq);
		}
	}

	attr.qp_state = IB_QPS_ERR;
	i40iw_modify_qp(&iwqp->ibqp, &attr, IB_QP_STATE, NULL);
	i40iw_qp_rem_ref(&iwqp->ibqp);
	wait_for_completion(&iwqp->free_qp);
	i40iw_cqp_qp_destroy_cmd(&iwdev->sc_dev, &iwqp->sc_qp);
	i40iw_rem_pdusecount(iwqp->iwpd, iwdev);
	i40iw_free_qp_resources(iwqp);
	i40iw_rem_devusecount(iwdev);

	return 0;
}

/**
 * i40iw_setup_virt_qp - setup for allocation of virtual qp
 * @iwdev: iwarp device
 * @iwqp: qp ptr
 * @init_info: initialize info to return
 */
static int i40iw_setup_virt_qp(struct i40iw_device *iwdev,
			       struct i40iw_qp *iwqp,
			       struct i40iw_qp_init_info *init_info)
{
	struct i40iw_pbl *iwpbl = &iwqp->iwpbl;
	struct i40iw_qp_mr *qpmr = &iwpbl->qp_mr;

	iwqp->page = qpmr->sq_page;
	init_info->shadow_area_pa = cpu_to_le64(qpmr->shadow);
	if (iwpbl->pbl_allocated) {
		init_info->virtual_map = true;
		init_info->sq_pa = qpmr->sq_pbl.idx;
		init_info->rq_pa = qpmr->rq_pbl.idx;
	} else {
		init_info->sq_pa = qpmr->sq_pbl.addr;
		init_info->rq_pa = qpmr->rq_pbl.addr;
	}
	return 0;
}

/**
 * i40iw_setup_kmode_qp - setup initialization for kernel mode qp
 * @iwdev: iwarp device
 * @iwqp: qp ptr (user or kernel)
 * @info: initialize info to return
 */
static int i40iw_setup_kmode_qp(struct i40iw_device *iwdev,
				struct i40iw_qp *iwqp,
				struct i40iw_qp_init_info *info)
{
	struct i40iw_dma_mem *mem = &iwqp->kqp.dma_mem;
	u32 sqdepth, rqdepth;
	u8 sqshift;
	u32 size;
	enum i40iw_status_code status;
	struct i40iw_qp_uk_init_info *ukinfo = &info->qp_uk_init_info;

	i40iw_get_wqe_shift(ukinfo->max_sq_frag_cnt, ukinfo->max_inline_data, &sqshift);
	status = i40iw_get_sqdepth(ukinfo->sq_size, sqshift, &sqdepth);
	if (status)
		return -ENOMEM;

	status = i40iw_get_rqdepth(ukinfo->rq_size, I40IW_MAX_RQ_WQE_SHIFT, &rqdepth);
	if (status)
		return -ENOMEM;

	size = sqdepth * sizeof(struct i40iw_sq_uk_wr_trk_info) + (rqdepth << 3);
	iwqp->kqp.wrid_mem = kzalloc(size, GFP_KERNEL);

	ukinfo->sq_wrtrk_array = (struct i40iw_sq_uk_wr_trk_info *)iwqp->kqp.wrid_mem;
	if (!ukinfo->sq_wrtrk_array)
		return -ENOMEM;

	ukinfo->rq_wrid_array = (u64 *)&ukinfo->sq_wrtrk_array[sqdepth];

	size = (sqdepth + rqdepth) * I40IW_QP_WQE_MIN_SIZE;
	size += (I40IW_SHADOW_AREA_SIZE << 3);

	status = i40iw_allocate_dma_mem(iwdev->sc_dev.hw, mem, size, 256);
	if (status) {
		kfree(ukinfo->sq_wrtrk_array);
		ukinfo->sq_wrtrk_array = NULL;
		return -ENOMEM;
	}

	ukinfo->sq = mem->va;
	info->sq_pa = mem->pa;

	ukinfo->rq = &ukinfo->sq[sqdepth];
	info->rq_pa = info->sq_pa + (sqdepth * I40IW_QP_WQE_MIN_SIZE);

	ukinfo->shadow_area = ukinfo->rq[rqdepth].elem;
	info->shadow_area_pa = info->rq_pa + (rqdepth * I40IW_QP_WQE_MIN_SIZE);

	ukinfo->sq_size = sqdepth >> sqshift;
	ukinfo->rq_size = rqdepth >> I40IW_MAX_RQ_WQE_SHIFT;
	ukinfo->qp_id = iwqp->ibqp.qp_num;
	return 0;
}

/**
 * i40iw_create_qp - create qp
 * @ibpd: ptr of pd
 * @init_attr: attributes for qp
 * @udata: user data for create qp
 */
static struct ib_qp *i40iw_create_qp(struct ib_pd *ibpd,
				     struct ib_qp_init_attr *init_attr,
				     struct ib_udata *udata)
{
	struct i40iw_pd *iwpd = to_iwpd(ibpd);
	struct i40iw_device *iwdev = to_iwdev(ibpd->device);
	struct i40iw_cqp *iwcqp = &iwdev->cqp;
	struct i40iw_qp *iwqp;
	struct i40iw_ucontext *ucontext = rdma_udata_to_drv_context(
		udata, struct i40iw_ucontext, ibucontext);
	struct i40iw_create_qp_req req;
	struct i40iw_create_qp_resp uresp;
	u32 qp_num = 0;
	enum i40iw_status_code ret;
	int err_code;
	int sq_size;
	int rq_size;
	struct i40iw_sc_qp *qp;
	struct i40iw_sc_dev *dev = &iwdev->sc_dev;
	struct i40iw_qp_init_info init_info;
	struct i40iw_create_qp_info *qp_info;
	struct i40iw_cqp_request *cqp_request;
	struct cqp_commands_info *cqp_info;

	struct i40iw_qp_host_ctx_info *ctx_info;
	struct i40iwarp_offload_info *iwarp_info;
	unsigned long flags;

	if (iwdev->closing)
		return ERR_PTR(-ENODEV);

	if (init_attr->create_flags)
		return ERR_PTR(-EOPNOTSUPP);
	if (init_attr->cap.max_inline_data > I40IW_MAX_INLINE_DATA_SIZE)
		init_attr->cap.max_inline_data = I40IW_MAX_INLINE_DATA_SIZE;

	if (init_attr->cap.max_send_sge > I40IW_MAX_WQ_FRAGMENT_COUNT)
		init_attr->cap.max_send_sge = I40IW_MAX_WQ_FRAGMENT_COUNT;

	if (init_attr->cap.max_recv_sge > I40IW_MAX_WQ_FRAGMENT_COUNT)
		init_attr->cap.max_recv_sge = I40IW_MAX_WQ_FRAGMENT_COUNT;

	memset(&init_info, 0, sizeof(init_info));

	sq_size = init_attr->cap.max_send_wr;
	rq_size = init_attr->cap.max_recv_wr;

	init_info.vsi = &iwdev->vsi;
	init_info.qp_uk_init_info.sq_size = sq_size;
	init_info.qp_uk_init_info.rq_size = rq_size;
	init_info.qp_uk_init_info.max_sq_frag_cnt = init_attr->cap.max_send_sge;
	init_info.qp_uk_init_info.max_rq_frag_cnt = init_attr->cap.max_recv_sge;
	init_info.qp_uk_init_info.max_inline_data = init_attr->cap.max_inline_data;

	iwqp = kzalloc(sizeof(*iwqp), GFP_KERNEL);
	if (!iwqp)
		return ERR_PTR(-ENOMEM);

	qp = &iwqp->sc_qp;
	qp->back_qp = (void *)iwqp;
	iwqp->iwdev = iwdev;
	iwqp->ctx_info.iwarp_info = &iwqp->iwarp_info;

	if (i40iw_allocate_dma_mem(dev->hw,
				   &iwqp->q2_ctx_mem,
				   I40IW_Q2_BUFFER_SIZE + I40IW_QP_CTX_SIZE,
				   256)) {
		i40iw_pr_err("dma_mem failed\n");
		err_code = -ENOMEM;
		goto error;
	}

	init_info.q2 = iwqp->q2_ctx_mem.va;
	init_info.q2_pa = iwqp->q2_ctx_mem.pa;

	init_info.host_ctx = (void *)init_info.q2 + I40IW_Q2_BUFFER_SIZE;
	init_info.host_ctx_pa = init_info.q2_pa + I40IW_Q2_BUFFER_SIZE;

	err_code = i40iw_alloc_resource(iwdev, iwdev->allocated_qps, iwdev->max_qp,
					&qp_num, &iwdev->next_qp);
	if (err_code) {
		i40iw_pr_err("qp resource\n");
		goto error;
	}

	iwqp->iwpd = iwpd;
	iwqp->ibqp.qp_num = qp_num;
	qp = &iwqp->sc_qp;
	iwqp->iwscq = to_iwcq(init_attr->send_cq);
	iwqp->iwrcq = to_iwcq(init_attr->recv_cq);

	iwqp->host_ctx.va = init_info.host_ctx;
	iwqp->host_ctx.pa = init_info.host_ctx_pa;
	iwqp->host_ctx.size = I40IW_QP_CTX_SIZE;

	init_info.pd = &iwpd->sc_pd;
	init_info.qp_uk_init_info.qp_id = iwqp->ibqp.qp_num;
	iwqp->ctx_info.qp_compl_ctx = (uintptr_t)qp;

	if (init_attr->qp_type != IB_QPT_RC) {
		err_code = -EOPNOTSUPP;
		goto error;
	}
	if (udata) {
		err_code = ib_copy_from_udata(&req, udata, sizeof(req));
		if (err_code) {
			i40iw_pr_err("ib_copy_from_data\n");
			goto error;
		}
		iwqp->ctx_info.qp_compl_ctx = req.user_compl_ctx;
		iwqp->user_mode = 1;

		if (req.user_wqe_buffers) {
			struct i40iw_pbl *iwpbl;

			spin_lock_irqsave(
			    &ucontext->qp_reg_mem_list_lock, flags);
			iwpbl = i40iw_get_pbl(
			    (unsigned long)req.user_wqe_buffers,
			    &ucontext->qp_reg_mem_list);
			spin_unlock_irqrestore(
			    &ucontext->qp_reg_mem_list_lock, flags);

			if (!iwpbl) {
				err_code = -ENODATA;
				i40iw_pr_err("no pbl info\n");
				goto error;
			}
			memcpy(&iwqp->iwpbl, iwpbl, sizeof(iwqp->iwpbl));
		}
		err_code = i40iw_setup_virt_qp(iwdev, iwqp, &init_info);
	} else {
		err_code = i40iw_setup_kmode_qp(iwdev, iwqp, &init_info);
	}

	if (err_code) {
		i40iw_pr_err("setup qp failed\n");
		goto error;
	}

	init_info.type = I40IW_QP_TYPE_IWARP;
	ret = dev->iw_priv_qp_ops->qp_init(qp, &init_info);
	if (ret) {
		err_code = -EPROTO;
		i40iw_pr_err("qp_init fail\n");
		goto error;
	}
	ctx_info = &iwqp->ctx_info;
	iwarp_info = &iwqp->iwarp_info;
	iwarp_info->rd_enable = true;
	iwarp_info->wr_rdresp_en = true;
	if (!iwqp->user_mode) {
		iwarp_info->fast_reg_en = true;
		iwarp_info->priv_mode_en = true;
	}
	iwarp_info->ddp_ver = 1;
	iwarp_info->rdmap_ver = 1;

	ctx_info->iwarp_info_valid = true;
	ctx_info->send_cq_num = iwqp->iwscq->sc_cq.cq_uk.cq_id;
	ctx_info->rcv_cq_num = iwqp->iwrcq->sc_cq.cq_uk.cq_id;
	ret = dev->iw_priv_qp_ops->qp_setctx(&iwqp->sc_qp,
					     (u64 *)iwqp->host_ctx.va,
					     ctx_info);
	ctx_info->iwarp_info_valid = false;
	cqp_request = i40iw_get_cqp_request(iwcqp, true);
	if (!cqp_request) {
		err_code = -ENOMEM;
		goto error;
	}
	cqp_info = &cqp_request->info;
	qp_info = &cqp_request->info.in.u.qp_create.info;

	memset(qp_info, 0, sizeof(*qp_info));

	qp_info->cq_num_valid = true;
	qp_info->next_iwarp_state = I40IW_QP_STATE_IDLE;

	cqp_info->cqp_cmd = OP_QP_CREATE;
	cqp_info->post_sq = 1;
	cqp_info->in.u.qp_create.qp = qp;
	cqp_info->in.u.qp_create.scratch = (uintptr_t)cqp_request;
	ret = i40iw_handle_cqp_op(iwdev, cqp_request);
	if (ret) {
		i40iw_pr_err("CQP-OP QP create fail");
		err_code = -EACCES;
		goto error;
	}

	refcount_set(&iwqp->refcount, 1);
	spin_lock_init(&iwqp->lock);
	iwqp->sig_all = (init_attr->sq_sig_type == IB_SIGNAL_ALL_WR) ? 1 : 0;
	iwdev->qp_table[qp_num] = iwqp;
	i40iw_add_pdusecount(iwqp->iwpd);
	i40iw_add_devusecount(iwdev);
	if (udata) {
		memset(&uresp, 0, sizeof(uresp));
		uresp.actual_sq_size = sq_size;
		uresp.actual_rq_size = rq_size;
		uresp.qp_id = qp_num;
		uresp.push_idx = I40IW_INVALID_PUSH_PAGE_INDEX;
		err_code = ib_copy_to_udata(udata, &uresp, sizeof(uresp));
		if (err_code) {
			i40iw_pr_err("copy_to_udata failed\n");
			i40iw_destroy_qp(&iwqp->ibqp, udata);
			/* let the completion of the qp destroy free the qp */
			return ERR_PTR(err_code);
		}
	}
	init_completion(&iwqp->sq_drained);
	init_completion(&iwqp->rq_drained);
	init_completion(&iwqp->free_qp);

	return &iwqp->ibqp;
error:
	i40iw_free_qp_resources(iwqp);
	return ERR_PTR(err_code);
}

/**
 * i40iw_query_qp - query qp attributes
 * @ibqp: qp pointer
 * @attr: attributes pointer
 * @attr_mask: Not used
 * @init_attr: qp attributes to return
 */
static int i40iw_query_qp(struct ib_qp *ibqp,
			  struct ib_qp_attr *attr,
			  int attr_mask,
			  struct ib_qp_init_attr *init_attr)
{
	struct i40iw_qp *iwqp = to_iwqp(ibqp);
	struct i40iw_sc_qp *qp = &iwqp->sc_qp;

	attr->qp_state = iwqp->ibqp_state;
	attr->cur_qp_state = attr->qp_state;
	attr->qp_access_flags = 0;
	attr->cap.max_send_wr = qp->qp_uk.sq_size;
	attr->cap.max_recv_wr = qp->qp_uk.rq_size;
	attr->cap.max_inline_data = I40IW_MAX_INLINE_DATA_SIZE;
	attr->cap.max_send_sge = I40IW_MAX_WQ_FRAGMENT_COUNT;
	attr->cap.max_recv_sge = I40IW_MAX_WQ_FRAGMENT_COUNT;
	attr->port_num = 1;
	init_attr->event_handler = iwqp->ibqp.event_handler;
	init_attr->qp_context = iwqp->ibqp.qp_context;
	init_attr->send_cq = iwqp->ibqp.send_cq;
	init_attr->recv_cq = iwqp->ibqp.recv_cq;
	init_attr->srq = iwqp->ibqp.srq;
	init_attr->cap = attr->cap;
	init_attr->port_num = 1;
	return 0;
}

/**
 * i40iw_hw_modify_qp - setup cqp for modify qp
 * @iwdev: iwarp device
 * @iwqp: qp ptr (user or kernel)
 * @info: info for modify qp
 * @wait: flag to wait or not for modify qp completion
 */
void i40iw_hw_modify_qp(struct i40iw_device *iwdev, struct i40iw_qp *iwqp,
			struct i40iw_modify_qp_info *info, bool wait)
{
	struct i40iw_cqp_request *cqp_request;
	struct cqp_commands_info *cqp_info;
	struct i40iw_modify_qp_info *m_info;
	struct i40iw_gen_ae_info ae_info;

	cqp_request = i40iw_get_cqp_request(&iwdev->cqp, wait);
	if (!cqp_request)
		return;

	cqp_info = &cqp_request->info;
	m_info = &cqp_info->in.u.qp_modify.info;
	memcpy(m_info, info, sizeof(*m_info));
	cqp_info->cqp_cmd = OP_QP_MODIFY;
	cqp_info->post_sq = 1;
	cqp_info->in.u.qp_modify.qp = &iwqp->sc_qp;
	cqp_info->in.u.qp_modify.scratch = (uintptr_t)cqp_request;
	if (!i40iw_handle_cqp_op(iwdev, cqp_request))
		return;

	switch (m_info->next_iwarp_state) {
	case I40IW_QP_STATE_RTS:
		if (iwqp->iwarp_state == I40IW_QP_STATE_IDLE)
			i40iw_send_reset(iwqp->cm_node);
		fallthrough;
	case I40IW_QP_STATE_IDLE:
	case I40IW_QP_STATE_TERMINATE:
	case I40IW_QP_STATE_CLOSING:
		ae_info.ae_code = I40IW_AE_BAD_CLOSE;
		ae_info.ae_source = 0;
		i40iw_gen_ae(iwdev, &iwqp->sc_qp, &ae_info, false);
		break;
	case I40IW_QP_STATE_ERROR:
	default:
		break;
	}
}

/**
 * i40iw_modify_qp - modify qp request
 * @ibqp: qp's pointer for modify
 * @attr: access attributes
 * @attr_mask: state mask
 * @udata: user data
 */
int i40iw_modify_qp(struct ib_qp *ibqp, struct ib_qp_attr *attr,
		    int attr_mask, struct ib_udata *udata)
{
	struct i40iw_qp *iwqp = to_iwqp(ibqp);
	struct i40iw_device *iwdev = iwqp->iwdev;
	struct i40iw_qp_host_ctx_info *ctx_info;
	struct i40iwarp_offload_info *iwarp_info;
	struct i40iw_modify_qp_info info;
	u8 issue_modify_qp = 0;
	u8 dont_wait = 0;
	u32 err;
	unsigned long flags;

	if (attr_mask & ~IB_QP_ATTR_STANDARD_BITS)
		return -EOPNOTSUPP;

	memset(&info, 0, sizeof(info));
	ctx_info = &iwqp->ctx_info;
	iwarp_info = &iwqp->iwarp_info;

	spin_lock_irqsave(&iwqp->lock, flags);

	if (attr_mask & IB_QP_STATE) {
		if (iwdev->closing && attr->qp_state != IB_QPS_ERR) {
			err = -EINVAL;
			goto exit;
		}

		switch (attr->qp_state) {
		case IB_QPS_INIT:
		case IB_QPS_RTR:
			if (iwqp->iwarp_state > (u32)I40IW_QP_STATE_IDLE) {
				err = -EINVAL;
				goto exit;
			}
			if (iwqp->iwarp_state == I40IW_QP_STATE_INVALID) {
				info.next_iwarp_state = I40IW_QP_STATE_IDLE;
				issue_modify_qp = 1;
			}
			break;
		case IB_QPS_RTS:
			if ((iwqp->iwarp_state > (u32)I40IW_QP_STATE_RTS) ||
			    (!iwqp->cm_id)) {
				err = -EINVAL;
				goto exit;
			}

			issue_modify_qp = 1;
			iwqp->hw_tcp_state = I40IW_TCP_STATE_ESTABLISHED;
			iwqp->hte_added = 1;
			info.next_iwarp_state = I40IW_QP_STATE_RTS;
			info.tcp_ctx_valid = true;
			info.ord_valid = true;
			info.arp_cache_idx_valid = true;
			info.cq_num_valid = true;
			break;
		case IB_QPS_SQD:
			if (iwqp->hw_iwarp_state > (u32)I40IW_QP_STATE_RTS) {
				err = 0;
				goto exit;
			}
			if ((iwqp->iwarp_state == (u32)I40IW_QP_STATE_CLOSING) ||
			    (iwqp->iwarp_state < (u32)I40IW_QP_STATE_RTS)) {
				err = 0;
				goto exit;
			}
			if (iwqp->iwarp_state > (u32)I40IW_QP_STATE_CLOSING) {
				err = -EINVAL;
				goto exit;
			}
			info.next_iwarp_state = I40IW_QP_STATE_CLOSING;
			issue_modify_qp = 1;
			break;
		case IB_QPS_SQE:
			if (iwqp->iwarp_state >= (u32)I40IW_QP_STATE_TERMINATE) {
				err = -EINVAL;
				goto exit;
			}
			info.next_iwarp_state = I40IW_QP_STATE_TERMINATE;
			issue_modify_qp = 1;
			break;
		case IB_QPS_ERR:
		case IB_QPS_RESET:
			if (iwqp->iwarp_state == (u32)I40IW_QP_STATE_ERROR) {
				err = -EINVAL;
				goto exit;
			}
			if (iwqp->sc_qp.term_flags)
				i40iw_terminate_del_timer(&iwqp->sc_qp);
			info.next_iwarp_state = I40IW_QP_STATE_ERROR;
			if ((iwqp->hw_tcp_state > I40IW_TCP_STATE_CLOSED) &&
			    iwdev->iw_status &&
			    (iwqp->hw_tcp_state != I40IW_TCP_STATE_TIME_WAIT))
				info.reset_tcp_conn = true;
			else
				dont_wait = 1;
			issue_modify_qp = 1;
			info.next_iwarp_state = I40IW_QP_STATE_ERROR;
			break;
		default:
			err = -EINVAL;
			goto exit;
		}

		iwqp->ibqp_state = attr->qp_state;

	}
	if (attr_mask & IB_QP_ACCESS_FLAGS) {
		ctx_info->iwarp_info_valid = true;
		if (attr->qp_access_flags & IB_ACCESS_LOCAL_WRITE)
			iwarp_info->wr_rdresp_en = true;
		if (attr->qp_access_flags & IB_ACCESS_REMOTE_WRITE)
			iwarp_info->wr_rdresp_en = true;
		if (attr->qp_access_flags & IB_ACCESS_REMOTE_READ)
			iwarp_info->rd_enable = true;
		if (attr->qp_access_flags & IB_ACCESS_MW_BIND)
			iwarp_info->bind_en = true;

		if (iwqp->user_mode) {
			iwarp_info->rd_enable = true;
			iwarp_info->wr_rdresp_en = true;
			iwarp_info->priv_mode_en = false;
		}
	}

	if (ctx_info->iwarp_info_valid) {
		struct i40iw_sc_dev *dev = &iwdev->sc_dev;
		int ret;

		ctx_info->send_cq_num = iwqp->iwscq->sc_cq.cq_uk.cq_id;
		ctx_info->rcv_cq_num = iwqp->iwrcq->sc_cq.cq_uk.cq_id;
		ret = dev->iw_priv_qp_ops->qp_setctx(&iwqp->sc_qp,
						     (u64 *)iwqp->host_ctx.va,
						     ctx_info);
		if (ret) {
			i40iw_pr_err("setting QP context\n");
			err = -EINVAL;
			goto exit;
		}
	}

	spin_unlock_irqrestore(&iwqp->lock, flags);

	if (issue_modify_qp) {
		i40iw_hw_modify_qp(iwdev, iwqp, &info, true);

		spin_lock_irqsave(&iwqp->lock, flags);
		iwqp->iwarp_state = info.next_iwarp_state;
		spin_unlock_irqrestore(&iwqp->lock, flags);
	}

	if (issue_modify_qp && (iwqp->ibqp_state > IB_QPS_RTS)) {
		if (dont_wait) {
			if (iwqp->cm_id && iwqp->hw_tcp_state) {
				spin_lock_irqsave(&iwqp->lock, flags);
				iwqp->hw_tcp_state = I40IW_TCP_STATE_CLOSED;
				iwqp->last_aeq = I40IW_AE_RESET_SENT;
				spin_unlock_irqrestore(&iwqp->lock, flags);
				i40iw_cm_disconn(iwqp);
			}
		} else {
			spin_lock_irqsave(&iwqp->lock, flags);
			if (iwqp->cm_id) {
				if (atomic_inc_return(&iwqp->close_timer_started) == 1) {
					iwqp->cm_id->add_ref(iwqp->cm_id);
					i40iw_schedule_cm_timer(iwqp->cm_node,
								(struct i40iw_puda_buf *)iwqp,
								 I40IW_TIMER_TYPE_CLOSE, 1, 0);
				}
			}
			spin_unlock_irqrestore(&iwqp->lock, flags);
		}
	}
	return 0;
exit:
	spin_unlock_irqrestore(&iwqp->lock, flags);
	return err;
}

/**
 * cq_free_resources - free up recources for cq
 * @iwdev: iwarp device
 * @iwcq: cq ptr
 */
static void cq_free_resources(struct i40iw_device *iwdev, struct i40iw_cq *iwcq)
{
	struct i40iw_sc_cq *cq = &iwcq->sc_cq;

	if (!iwcq->user_mode)
		i40iw_free_dma_mem(iwdev->sc_dev.hw, &iwcq->kmem);
	i40iw_free_resource(iwdev, iwdev->allocated_cqs, cq->cq_uk.cq_id);
}

/**
 * i40iw_cq_wq_destroy - send cq destroy cqp
 * @iwdev: iwarp device
 * @cq: hardware control cq
 */
void i40iw_cq_wq_destroy(struct i40iw_device *iwdev, struct i40iw_sc_cq *cq)
{
	enum i40iw_status_code status;
	struct i40iw_cqp_request *cqp_request;
	struct cqp_commands_info *cqp_info;

	cqp_request = i40iw_get_cqp_request(&iwdev->cqp, true);
	if (!cqp_request)
		return;

	cqp_info = &cqp_request->info;

	cqp_info->cqp_cmd = OP_CQ_DESTROY;
	cqp_info->post_sq = 1;
	cqp_info->in.u.cq_destroy.cq = cq;
	cqp_info->in.u.cq_destroy.scratch = (uintptr_t)cqp_request;
	status = i40iw_handle_cqp_op(iwdev, cqp_request);
	if (status)
		i40iw_pr_err("CQP-OP Destroy QP fail");
}

/**
 * i40iw_destroy_cq - destroy cq
 * @ib_cq: cq pointer
 * @udata: user data or NULL for kernel object
 */
static int i40iw_destroy_cq(struct ib_cq *ib_cq, struct ib_udata *udata)
{
	struct i40iw_cq *iwcq;
	struct i40iw_device *iwdev;
	struct i40iw_sc_cq *cq;

	iwcq = to_iwcq(ib_cq);
	iwdev = to_iwdev(ib_cq->device);
	cq = &iwcq->sc_cq;
	i40iw_cq_wq_destroy(iwdev, cq);
	cq_free_resources(iwdev, iwcq);
	i40iw_rem_devusecount(iwdev);
	return 0;
}

/**
 * i40iw_create_cq - create cq
 * @ibcq: CQ allocated
 * @attr: attributes for cq
 * @udata: user data
 */
static int i40iw_create_cq(struct ib_cq *ibcq,
			   const struct ib_cq_init_attr *attr,
			   struct ib_udata *udata)
{
	struct ib_device *ibdev = ibcq->device;
	struct i40iw_device *iwdev = to_iwdev(ibdev);
	struct i40iw_cq *iwcq = to_iwcq(ibcq);
	struct i40iw_pbl *iwpbl;
	u32 cq_num = 0;
	struct i40iw_sc_cq *cq;
	struct i40iw_sc_dev *dev = &iwdev->sc_dev;
	struct i40iw_cq_init_info info = {};
	enum i40iw_status_code status;
	struct i40iw_cqp_request *cqp_request;
	struct cqp_commands_info *cqp_info;
	struct i40iw_cq_uk_init_info *ukinfo = &info.cq_uk_init_info;
	unsigned long flags;
	int err_code;
	int entries = attr->cqe;

	if (attr->flags)
		return -EOPNOTSUPP;

	if (iwdev->closing)
		return -ENODEV;

	if (entries > iwdev->max_cqe)
		return -EINVAL;

	err_code = i40iw_alloc_resource(iwdev, iwdev->allocated_cqs,
					iwdev->max_cq, &cq_num,
					&iwdev->next_cq);
	if (err_code)
		return err_code;

	cq = &iwcq->sc_cq;
	cq->back_cq = (void *)iwcq;
	spin_lock_init(&iwcq->lock);

	info.dev = dev;
	ukinfo->cq_size = max(entries, 4);
	ukinfo->cq_id = cq_num;
	iwcq->ibcq.cqe = info.cq_uk_init_info.cq_size;
	info.ceqe_mask = 0;
	if (attr->comp_vector < iwdev->ceqs_count)
		info.ceq_id = attr->comp_vector;
	info.ceq_id_valid = true;
	info.ceqe_mask = 1;
	info.type = I40IW_CQ_TYPE_IWARP;
	if (udata) {
		struct i40iw_ucontext *ucontext = rdma_udata_to_drv_context(
			udata, struct i40iw_ucontext, ibucontext);
		struct i40iw_create_cq_req req;
		struct i40iw_cq_mr *cqmr;

		memset(&req, 0, sizeof(req));
		iwcq->user_mode = true;
		if (ib_copy_from_udata(&req, udata, sizeof(struct i40iw_create_cq_req))) {
			err_code = -EFAULT;
			goto cq_free_resources;
		}

		spin_lock_irqsave(&ucontext->cq_reg_mem_list_lock, flags);
		iwpbl = i40iw_get_pbl((unsigned long)req.user_cq_buffer,
				      &ucontext->cq_reg_mem_list);
		spin_unlock_irqrestore(&ucontext->cq_reg_mem_list_lock, flags);
		if (!iwpbl) {
			err_code = -EPROTO;
			goto cq_free_resources;
		}

		iwcq->iwpbl = iwpbl;
		iwcq->cq_mem_size = 0;
		cqmr = &iwpbl->cq_mr;
		info.shadow_area_pa = cpu_to_le64(cqmr->shadow);
		if (iwpbl->pbl_allocated) {
			info.virtual_map = true;
			info.pbl_chunk_size = 1;
			info.first_pm_pbl_idx = cqmr->cq_pbl.idx;
		} else {
			info.cq_base_pa = cqmr->cq_pbl.addr;
		}
	} else {
		/* Kmode allocations */
		int rsize;
		int shadow;

		rsize = info.cq_uk_init_info.cq_size * sizeof(struct i40iw_cqe);
		rsize = round_up(rsize, 256);
		shadow = I40IW_SHADOW_AREA_SIZE << 3;
		status = i40iw_allocate_dma_mem(dev->hw, &iwcq->kmem,
						rsize + shadow, 256);
		if (status) {
			err_code = -ENOMEM;
			goto cq_free_resources;
		}
		ukinfo->cq_base = iwcq->kmem.va;
		info.cq_base_pa = iwcq->kmem.pa;
		info.shadow_area_pa = info.cq_base_pa + rsize;
		ukinfo->shadow_area = iwcq->kmem.va + rsize;
	}

	if (dev->iw_priv_cq_ops->cq_init(cq, &info)) {
		i40iw_pr_err("init cq fail\n");
		err_code = -EPROTO;
		goto cq_free_resources;
	}

	cqp_request = i40iw_get_cqp_request(&iwdev->cqp, true);
	if (!cqp_request) {
		err_code = -ENOMEM;
		goto cq_free_resources;
	}

	cqp_info = &cqp_request->info;
	cqp_info->cqp_cmd = OP_CQ_CREATE;
	cqp_info->post_sq = 1;
	cqp_info->in.u.cq_create.cq = cq;
	cqp_info->in.u.cq_create.scratch = (uintptr_t)cqp_request;
	status = i40iw_handle_cqp_op(iwdev, cqp_request);
	if (status) {
		i40iw_pr_err("CQP-OP Create QP fail");
		err_code = -EPROTO;
		goto cq_free_resources;
	}

	if (udata) {
		struct i40iw_create_cq_resp resp;

		memset(&resp, 0, sizeof(resp));
		resp.cq_id = info.cq_uk_init_info.cq_id;
		resp.cq_size = info.cq_uk_init_info.cq_size;
		if (ib_copy_to_udata(udata, &resp, sizeof(resp))) {
			i40iw_pr_err("copy to user data\n");
			err_code = -EPROTO;
			goto cq_destroy;
		}
	}

	i40iw_add_devusecount(iwdev);
	return 0;

cq_destroy:
	i40iw_cq_wq_destroy(iwdev, cq);
cq_free_resources:
	cq_free_resources(iwdev, iwcq);
	return err_code;
}

/**
 * i40iw_get_user_access - get hw access from IB access
 * @acc: IB access to return hw access
 */
static inline u16 i40iw_get_user_access(int acc)
{
	u16 access = 0;

	access |= (acc & IB_ACCESS_LOCAL_WRITE) ? I40IW_ACCESS_FLAGS_LOCALWRITE : 0;
	access |= (acc & IB_ACCESS_REMOTE_WRITE) ? I40IW_ACCESS_FLAGS_REMOTEWRITE : 0;
	access |= (acc & IB_ACCESS_REMOTE_READ) ? I40IW_ACCESS_FLAGS_REMOTEREAD : 0;
	access |= (acc & IB_ACCESS_MW_BIND) ? I40IW_ACCESS_FLAGS_BIND_WINDOW : 0;
	return access;
}

/**
 * i40iw_free_stag - free stag resource
 * @iwdev: iwarp device
 * @stag: stag to free
 */
static void i40iw_free_stag(struct i40iw_device *iwdev, u32 stag)
{
	u32 stag_idx;

	stag_idx = (stag & iwdev->mr_stagmask) >> I40IW_CQPSQ_STAG_IDX_SHIFT;
	i40iw_free_resource(iwdev, iwdev->allocated_mrs, stag_idx);
	i40iw_rem_devusecount(iwdev);
}

/**
 * i40iw_create_stag - create random stag
 * @iwdev: iwarp device
 */
static u32 i40iw_create_stag(struct i40iw_device *iwdev)
{
	u32 stag = 0;
	u32 stag_index = 0;
	u32 next_stag_index;
	u32 driver_key;
	u32 random;
	u8 consumer_key;
	int ret;

	get_random_bytes(&random, sizeof(random));
	consumer_key = (u8)random;

	driver_key = random & ~iwdev->mr_stagmask;
	next_stag_index = (random & iwdev->mr_stagmask) >> 8;
	next_stag_index %= iwdev->max_mr;

	ret = i40iw_alloc_resource(iwdev,
				   iwdev->allocated_mrs, iwdev->max_mr,
				   &stag_index, &next_stag_index);
	if (!ret) {
		stag = stag_index << I40IW_CQPSQ_STAG_IDX_SHIFT;
		stag |= driver_key;
		stag += (u32)consumer_key;
		i40iw_add_devusecount(iwdev);
	}
	return stag;
}

/**
 * i40iw_next_pbl_addr - Get next pbl address
 * @pbl: pointer to a pble
 * @pinfo: info pointer
 * @idx: index
 */
static inline u64 *i40iw_next_pbl_addr(u64 *pbl,
				       struct i40iw_pble_info **pinfo,
				       u32 *idx)
{
	*idx += 1;
	if ((!(*pinfo)) || (*idx != (*pinfo)->cnt))
		return ++pbl;
	*idx = 0;
	(*pinfo)++;
	return (u64 *)(*pinfo)->addr;
}

/**
 * i40iw_copy_user_pgaddrs - copy user page address to pble's os locally
 * @iwmr: iwmr for IB's user page addresses
 * @pbl: ple pointer to save 1 level or 0 level pble
 * @level: indicated level 0, 1 or 2
 */
static void i40iw_copy_user_pgaddrs(struct i40iw_mr *iwmr,
				    u64 *pbl,
				    enum i40iw_pble_level level)
{
	struct ib_umem *region = iwmr->region;
	struct i40iw_pbl *iwpbl = &iwmr->iwpbl;
	struct i40iw_pble_alloc *palloc = &iwpbl->pble_alloc;
	struct i40iw_pble_info *pinfo;
	struct ib_block_iter biter;
	u32 idx = 0;

	pinfo = (level == I40IW_LEVEL_1) ? NULL : palloc->level2.leaf;

	if (iwmr->type == IW_MEMREG_TYPE_QP)
		iwpbl->qp_mr.sq_page = sg_page(region->sg_head.sgl);

	rdma_umem_for_each_dma_block(region, &biter, iwmr->page_size) {
		*pbl = rdma_block_iter_dma_address(&biter);
		pbl = i40iw_next_pbl_addr(pbl, &pinfo, &idx);
	}
}

/**
 * i40iw_check_mem_contiguous - check if pbls stored in arr are contiguous
 * @arr: lvl1 pbl array
 * @npages: page count
 * @pg_size: page size
 *
 */
static bool i40iw_check_mem_contiguous(u64 *arr, u32 npages, u32 pg_size)
{
	u32 pg_idx;

	for (pg_idx = 0; pg_idx < npages; pg_idx++) {
		if ((*arr + (pg_size * pg_idx)) != arr[pg_idx])
			return false;
	}
	return true;
}

/**
 * i40iw_check_mr_contiguous - check if MR is physically contiguous
 * @palloc: pbl allocation struct
 * @pg_size: page size
 */
static bool i40iw_check_mr_contiguous(struct i40iw_pble_alloc *palloc, u32 pg_size)
{
	struct i40iw_pble_level2 *lvl2 = &palloc->level2;
	struct i40iw_pble_info *leaf = lvl2->leaf;
	u64 *arr = NULL;
	u64 *start_addr = NULL;
	int i;
	bool ret;

	if (palloc->level == I40IW_LEVEL_1) {
		arr = (u64 *)palloc->level1.addr;
		ret = i40iw_check_mem_contiguous(arr, palloc->total_cnt, pg_size);
		return ret;
	}

	start_addr = (u64 *)leaf->addr;

	for (i = 0; i < lvl2->leaf_cnt; i++, leaf++) {
		arr = (u64 *)leaf->addr;
		if ((*start_addr + (i * pg_size * PBLE_PER_PAGE)) != *arr)
			return false;
		ret = i40iw_check_mem_contiguous(arr, leaf->cnt, pg_size);
		if (!ret)
			return false;
	}

	return true;
}

/**
 * i40iw_setup_pbles - copy user pg address to pble's
 * @iwdev: iwarp device
 * @iwmr: mr pointer for this memory registration
 * @use_pbles: flag if to use pble's
 */
static int i40iw_setup_pbles(struct i40iw_device *iwdev,
			     struct i40iw_mr *iwmr,
			     bool use_pbles)
{
	struct i40iw_pbl *iwpbl = &iwmr->iwpbl;
	struct i40iw_pble_alloc *palloc = &iwpbl->pble_alloc;
	struct i40iw_pble_info *pinfo;
	u64 *pbl;
	enum i40iw_status_code status;
	enum i40iw_pble_level level = I40IW_LEVEL_1;

	if (use_pbles) {
		mutex_lock(&iwdev->pbl_mutex);
		status = i40iw_get_pble(&iwdev->sc_dev, iwdev->pble_rsrc, palloc, iwmr->page_cnt);
		mutex_unlock(&iwdev->pbl_mutex);
		if (status)
			return -ENOMEM;

		iwpbl->pbl_allocated = true;
		level = palloc->level;
		pinfo = (level == I40IW_LEVEL_1) ? &palloc->level1 : palloc->level2.leaf;
		pbl = (u64 *)pinfo->addr;
	} else {
		pbl = iwmr->pgaddrmem;
	}

	i40iw_copy_user_pgaddrs(iwmr, pbl, level);

	if (use_pbles)
		iwmr->pgaddrmem[0] = *pbl;

	return 0;
}

/**
 * i40iw_handle_q_mem - handle memory for qp and cq
 * @iwdev: iwarp device
 * @req: information for q memory management
 * @iwpbl: pble struct
 * @use_pbles: flag to use pble
 */
static int i40iw_handle_q_mem(struct i40iw_device *iwdev,
			      struct i40iw_mem_reg_req *req,
			      struct i40iw_pbl *iwpbl,
			      bool use_pbles)
{
	struct i40iw_pble_alloc *palloc = &iwpbl->pble_alloc;
	struct i40iw_mr *iwmr = iwpbl->iwmr;
	struct i40iw_qp_mr *qpmr = &iwpbl->qp_mr;
	struct i40iw_cq_mr *cqmr = &iwpbl->cq_mr;
	struct i40iw_hmc_pble *hmc_p;
	u64 *arr = iwmr->pgaddrmem;
	u32 pg_size;
	int err;
	int total;
	bool ret = true;

	total = req->sq_pages + req->rq_pages + req->cq_pages;
	pg_size = iwmr->page_size;

	err = i40iw_setup_pbles(iwdev, iwmr, use_pbles);
	if (err)
		return err;

	if (use_pbles && (palloc->level != I40IW_LEVEL_1)) {
		i40iw_free_pble(iwdev->pble_rsrc, palloc);
		iwpbl->pbl_allocated = false;
		return -ENOMEM;
	}

	if (use_pbles)
		arr = (u64 *)palloc->level1.addr;

	if (iwmr->type == IW_MEMREG_TYPE_QP) {
		hmc_p = &qpmr->sq_pbl;
		qpmr->shadow = (dma_addr_t)arr[total];

		if (use_pbles) {
			ret = i40iw_check_mem_contiguous(arr, req->sq_pages, pg_size);
			if (ret)
				ret = i40iw_check_mem_contiguous(&arr[req->sq_pages], req->rq_pages, pg_size);
		}

		if (!ret) {
			hmc_p->idx = palloc->level1.idx;
			hmc_p = &qpmr->rq_pbl;
			hmc_p->idx = palloc->level1.idx + req->sq_pages;
		} else {
			hmc_p->addr = arr[0];
			hmc_p = &qpmr->rq_pbl;
			hmc_p->addr = arr[req->sq_pages];
		}
	} else {		/* CQ */
		hmc_p = &cqmr->cq_pbl;
		cqmr->shadow = (dma_addr_t)arr[total];

		if (use_pbles)
			ret = i40iw_check_mem_contiguous(arr, req->cq_pages, pg_size);

		if (!ret)
			hmc_p->idx = palloc->level1.idx;
		else
			hmc_p->addr = arr[0];
	}

	if (use_pbles && ret) {
		i40iw_free_pble(iwdev->pble_rsrc, palloc);
		iwpbl->pbl_allocated = false;
	}

	return err;
}

/**
 * i40iw_hw_alloc_stag - cqp command to allocate stag
 * @iwdev: iwarp device
 * @iwmr: iwarp mr pointer
 */
static int i40iw_hw_alloc_stag(struct i40iw_device *iwdev, struct i40iw_mr *iwmr)
{
	struct i40iw_allocate_stag_info *info;
	struct i40iw_pd *iwpd = to_iwpd(iwmr->ibmr.pd);
	enum i40iw_status_code status;
	int err = 0;
	struct i40iw_cqp_request *cqp_request;
	struct cqp_commands_info *cqp_info;

	cqp_request = i40iw_get_cqp_request(&iwdev->cqp, true);
	if (!cqp_request)
		return -ENOMEM;

	cqp_info = &cqp_request->info;
	info = &cqp_info->in.u.alloc_stag.info;
	memset(info, 0, sizeof(*info));
	info->page_size = PAGE_SIZE;
	info->stag_idx = iwmr->stag >> I40IW_CQPSQ_STAG_IDX_SHIFT;
	info->pd_id = iwpd->sc_pd.pd_id;
	info->total_len = iwmr->length;
	info->remote_access = true;
	cqp_info->cqp_cmd = OP_ALLOC_STAG;
	cqp_info->post_sq = 1;
	cqp_info->in.u.alloc_stag.dev = &iwdev->sc_dev;
	cqp_info->in.u.alloc_stag.scratch = (uintptr_t)cqp_request;

	status = i40iw_handle_cqp_op(iwdev, cqp_request);
	if (status) {
		err = -ENOMEM;
		i40iw_pr_err("CQP-OP MR Reg fail");
	}
	return err;
}

/**
 * i40iw_alloc_mr - register stag for fast memory registration
 * @pd: ibpd pointer
 * @mr_type: memory for stag registrion
 * @max_num_sg: man number of pages
 */
static struct ib_mr *i40iw_alloc_mr(struct ib_pd *pd, enum ib_mr_type mr_type,
				    u32 max_num_sg)
{
	struct i40iw_pd *iwpd = to_iwpd(pd);
	struct i40iw_device *iwdev = to_iwdev(pd->device);
	struct i40iw_pble_alloc *palloc;
	struct i40iw_pbl *iwpbl;
	struct i40iw_mr *iwmr;
	enum i40iw_status_code status;
	u32 stag;
	int err_code = -ENOMEM;

	iwmr = kzalloc(sizeof(*iwmr), GFP_KERNEL);
	if (!iwmr)
		return ERR_PTR(-ENOMEM);

	stag = i40iw_create_stag(iwdev);
	if (!stag) {
		err_code = -EOVERFLOW;
		goto err;
	}
	stag &= ~I40IW_CQPSQ_STAG_KEY_MASK;
	iwmr->stag = stag;
	iwmr->ibmr.rkey = stag;
	iwmr->ibmr.lkey = stag;
	iwmr->ibmr.pd = pd;
	iwmr->ibmr.device = pd->device;
	iwpbl = &iwmr->iwpbl;
	iwpbl->iwmr = iwmr;
	iwmr->type = IW_MEMREG_TYPE_MEM;
	palloc = &iwpbl->pble_alloc;
	iwmr->page_cnt = max_num_sg;
	mutex_lock(&iwdev->pbl_mutex);
	status = i40iw_get_pble(&iwdev->sc_dev, iwdev->pble_rsrc, palloc, iwmr->page_cnt);
	mutex_unlock(&iwdev->pbl_mutex);
	if (status)
		goto err1;

	if (palloc->level != I40IW_LEVEL_1)
		goto err2;
	err_code = i40iw_hw_alloc_stag(iwdev, iwmr);
	if (err_code)
		goto err2;
	iwpbl->pbl_allocated = true;
	i40iw_add_pdusecount(iwpd);
	return &iwmr->ibmr;
err2:
	i40iw_free_pble(iwdev->pble_rsrc, palloc);
err1:
	i40iw_free_stag(iwdev, stag);
err:
	kfree(iwmr);
	return ERR_PTR(err_code);
}

/**
 * i40iw_set_page - populate pbl list for fmr
 * @ibmr: ib mem to access iwarp mr pointer
 * @addr: page dma address fro pbl list
 */
static int i40iw_set_page(struct ib_mr *ibmr, u64 addr)
{
	struct i40iw_mr *iwmr = to_iwmr(ibmr);
	struct i40iw_pbl *iwpbl = &iwmr->iwpbl;
	struct i40iw_pble_alloc *palloc = &iwpbl->pble_alloc;
	u64 *pbl;

	if (unlikely(iwmr->npages == iwmr->page_cnt))
		return -ENOMEM;

	pbl = (u64 *)palloc->level1.addr;
	pbl[iwmr->npages++] = cpu_to_le64(addr);
	return 0;
}

/**
 * i40iw_map_mr_sg - map of sg list for fmr
 * @ibmr: ib mem to access iwarp mr pointer
 * @sg: scatter gather list for fmr
 * @sg_nents: number of sg pages
 * @sg_offset: scatter gather offset
 */
static int i40iw_map_mr_sg(struct ib_mr *ibmr, struct scatterlist *sg,
			   int sg_nents, unsigned int *sg_offset)
{
	struct i40iw_mr *iwmr = to_iwmr(ibmr);

	iwmr->npages = 0;
	return ib_sg_to_pages(ibmr, sg, sg_nents, sg_offset, i40iw_set_page);
}

/**
 * i40iw_drain_sq - drain the send queue
 * @ibqp: ib qp pointer
 */
static void i40iw_drain_sq(struct ib_qp *ibqp)
{
	struct i40iw_qp *iwqp = to_iwqp(ibqp);
	struct i40iw_sc_qp *qp = &iwqp->sc_qp;

	if (I40IW_RING_MORE_WORK(qp->qp_uk.sq_ring))
		wait_for_completion(&iwqp->sq_drained);
}

/**
 * i40iw_drain_rq - drain the receive queue
 * @ibqp: ib qp pointer
 */
static void i40iw_drain_rq(struct ib_qp *ibqp)
{
	struct i40iw_qp *iwqp = to_iwqp(ibqp);
	struct i40iw_sc_qp *qp = &iwqp->sc_qp;

	if (I40IW_RING_MORE_WORK(qp->qp_uk.rq_ring))
		wait_for_completion(&iwqp->rq_drained);
}

/**
 * i40iw_hwreg_mr - send cqp command for memory registration
 * @iwdev: iwarp device
 * @iwmr: iwarp mr pointer
 * @access: access for MR
 */
static int i40iw_hwreg_mr(struct i40iw_device *iwdev,
			  struct i40iw_mr *iwmr,
			  u16 access)
{
	struct i40iw_pbl *iwpbl = &iwmr->iwpbl;
	struct i40iw_reg_ns_stag_info *stag_info;
	struct i40iw_pd *iwpd = to_iwpd(iwmr->ibmr.pd);
	struct i40iw_pble_alloc *palloc = &iwpbl->pble_alloc;
	enum i40iw_status_code status;
	int err = 0;
	struct i40iw_cqp_request *cqp_request;
	struct cqp_commands_info *cqp_info;

	cqp_request = i40iw_get_cqp_request(&iwdev->cqp, true);
	if (!cqp_request)
		return -ENOMEM;

	cqp_info = &cqp_request->info;
	stag_info = &cqp_info->in.u.mr_reg_non_shared.info;
	memset(stag_info, 0, sizeof(*stag_info));
	stag_info->va = (void *)(unsigned long)iwpbl->user_base;
	stag_info->stag_idx = iwmr->stag >> I40IW_CQPSQ_STAG_IDX_SHIFT;
	stag_info->stag_key = (u8)iwmr->stag;
	stag_info->total_len = iwmr->length;
	stag_info->access_rights = access;
	stag_info->pd_id = iwpd->sc_pd.pd_id;
	stag_info->addr_type = I40IW_ADDR_TYPE_VA_BASED;
	stag_info->page_size = iwmr->page_size;

	if (iwpbl->pbl_allocated) {
		if (palloc->level == I40IW_LEVEL_1) {
			stag_info->first_pm_pbl_index = palloc->level1.idx;
			stag_info->chunk_size = 1;
		} else {
			stag_info->first_pm_pbl_index = palloc->level2.root.idx;
			stag_info->chunk_size = 3;
		}
	} else {
		stag_info->reg_addr_pa = iwmr->pgaddrmem[0];
	}

	cqp_info->cqp_cmd = OP_MR_REG_NON_SHARED;
	cqp_info->post_sq = 1;
	cqp_info->in.u.mr_reg_non_shared.dev = &iwdev->sc_dev;
	cqp_info->in.u.mr_reg_non_shared.scratch = (uintptr_t)cqp_request;

	status = i40iw_handle_cqp_op(iwdev, cqp_request);
	if (status) {
		err = -ENOMEM;
		i40iw_pr_err("CQP-OP MR Reg fail");
	}
	return err;
}

/**
 * i40iw_reg_user_mr - Register a user memory region
 * @pd: ptr of pd
 * @start: virtual start address
 * @length: length of mr
 * @virt: virtual address
 * @acc: access of mr
 * @udata: user data
 */
static struct ib_mr *i40iw_reg_user_mr(struct ib_pd *pd,
				       u64 start,
				       u64 length,
				       u64 virt,
				       int acc,
				       struct ib_udata *udata)
{
	struct i40iw_pd *iwpd = to_iwpd(pd);
	struct i40iw_device *iwdev = to_iwdev(pd->device);
	struct i40iw_ucontext *ucontext = rdma_udata_to_drv_context(
		udata, struct i40iw_ucontext, ibucontext);
	struct i40iw_pble_alloc *palloc;
	struct i40iw_pbl *iwpbl;
	struct i40iw_mr *iwmr;
	struct ib_umem *region;
	struct i40iw_mem_reg_req req;
	u32 stag = 0;
	u16 access;
	bool use_pbles = false;
	unsigned long flags;
	int err = -ENOSYS;
	int ret;

	if (!udata)
		return ERR_PTR(-EOPNOTSUPP);

	if (iwdev->closing)
		return ERR_PTR(-ENODEV);

	if (length > I40IW_MAX_MR_SIZE)
		return ERR_PTR(-EINVAL);
	region = ib_umem_get(pd->device, start, length, acc);
	if (IS_ERR(region))
		return (struct ib_mr *)region;

	if (ib_copy_from_udata(&req, udata, sizeof(req))) {
		ib_umem_release(region);
		return ERR_PTR(-EFAULT);
	}

	iwmr = kzalloc(sizeof(*iwmr), GFP_KERNEL);
	if (!iwmr) {
		ib_umem_release(region);
		return ERR_PTR(-ENOMEM);
	}

	iwpbl = &iwmr->iwpbl;
	iwpbl->iwmr = iwmr;
	iwmr->region = region;
	iwmr->ibmr.pd = pd;
	iwmr->ibmr.device = pd->device;

	iwmr->page_size = PAGE_SIZE;
	if (req.reg_type == IW_MEMREG_TYPE_MEM)
		iwmr->page_size = ib_umem_find_best_pgsz(region, SZ_4K | SZ_2M,
							 virt);
	iwmr->length = region->length;

	iwpbl->user_base = virt;
	palloc = &iwpbl->pble_alloc;

	iwmr->type = req.reg_type;
	iwmr->page_cnt = ib_umem_num_dma_blocks(region, iwmr->page_size);

	switch (req.reg_type) {
	case IW_MEMREG_TYPE_QP:
		use_pbles = ((req.sq_pages + req.rq_pages) > 2);
		err = i40iw_handle_q_mem(iwdev, &req, iwpbl, use_pbles);
		if (err)
			goto error;
		spin_lock_irqsave(&ucontext->qp_reg_mem_list_lock, flags);
		list_add_tail(&iwpbl->list, &ucontext->qp_reg_mem_list);
		iwpbl->on_list = true;
		spin_unlock_irqrestore(&ucontext->qp_reg_mem_list_lock, flags);
		break;
	case IW_MEMREG_TYPE_CQ:
		use_pbles = (req.cq_pages > 1);
		err = i40iw_handle_q_mem(iwdev, &req, iwpbl, use_pbles);
		if (err)
			goto error;

		spin_lock_irqsave(&ucontext->cq_reg_mem_list_lock, flags);
		list_add_tail(&iwpbl->list, &ucontext->cq_reg_mem_list);
		iwpbl->on_list = true;
		spin_unlock_irqrestore(&ucontext->cq_reg_mem_list_lock, flags);
		break;
	case IW_MEMREG_TYPE_MEM:
		use_pbles = (iwmr->page_cnt != 1);
		access = I40IW_ACCESS_FLAGS_LOCALREAD;

		err = i40iw_setup_pbles(iwdev, iwmr, use_pbles);
		if (err)
			goto error;

		if (use_pbles) {
			ret = i40iw_check_mr_contiguous(palloc, iwmr->page_size);
			if (ret) {
				i40iw_free_pble(iwdev->pble_rsrc, palloc);
				iwpbl->pbl_allocated = false;
			}
		}

		access |= i40iw_get_user_access(acc);
		stag = i40iw_create_stag(iwdev);
		if (!stag) {
			err = -ENOMEM;
			goto error;
		}

		iwmr->stag = stag;
		iwmr->ibmr.rkey = stag;
		iwmr->ibmr.lkey = stag;

		err = i40iw_hwreg_mr(iwdev, iwmr, access);
		if (err) {
			i40iw_free_stag(iwdev, stag);
			goto error;
		}

		break;
	default:
		goto error;
	}

	iwmr->type = req.reg_type;
	if (req.reg_type == IW_MEMREG_TYPE_MEM)
		i40iw_add_pdusecount(iwpd);
	return &iwmr->ibmr;

error:
	if (palloc->level != I40IW_LEVEL_0 && iwpbl->pbl_allocated)
		i40iw_free_pble(iwdev->pble_rsrc, palloc);
	ib_umem_release(region);
	kfree(iwmr);
	return ERR_PTR(err);
}

/**
 * i40iw_reg_phys_mr - register kernel physical memory
 * @pd: ibpd pointer
 * @addr: physical address of memory to register
 * @size: size of memory to register
 * @acc: Access rights
 * @iova_start: start of virtual address for physical buffers
 */
struct ib_mr *i40iw_reg_phys_mr(struct ib_pd *pd,
				u64 addr,
				u64 size,
				int acc,
				u64 *iova_start)
{
	struct i40iw_pd *iwpd = to_iwpd(pd);
	struct i40iw_device *iwdev = to_iwdev(pd->device);
	struct i40iw_pbl *iwpbl;
	struct i40iw_mr *iwmr;
	enum i40iw_status_code status;
	u32 stag;
	u16 access = I40IW_ACCESS_FLAGS_LOCALREAD;
	int ret;

	iwmr = kzalloc(sizeof(*iwmr), GFP_KERNEL);
	if (!iwmr)
		return ERR_PTR(-ENOMEM);
	iwmr->ibmr.pd = pd;
	iwmr->ibmr.device = pd->device;
	iwpbl = &iwmr->iwpbl;
	iwpbl->iwmr = iwmr;
	iwmr->type = IW_MEMREG_TYPE_MEM;
	iwpbl->user_base = *iova_start;
	stag = i40iw_create_stag(iwdev);
	if (!stag) {
		ret = -EOVERFLOW;
		goto err;
	}
	access |= i40iw_get_user_access(acc);
	iwmr->stag = stag;
	iwmr->ibmr.rkey = stag;
	iwmr->ibmr.lkey = stag;
	iwmr->page_cnt = 1;
	iwmr->pgaddrmem[0]  = addr;
	iwmr->length = size;
	status = i40iw_hwreg_mr(iwdev, iwmr, access);
	if (status) {
		i40iw_free_stag(iwdev, stag);
		ret = -ENOMEM;
		goto err;
	}

	i40iw_add_pdusecount(iwpd);
	return &iwmr->ibmr;
 err:
	kfree(iwmr);
	return ERR_PTR(ret);
}

/**
 * i40iw_get_dma_mr - register physical mem
 * @pd: ptr of pd
 * @acc: access for memory
 */
static struct ib_mr *i40iw_get_dma_mr(struct ib_pd *pd, int acc)
{
	u64 kva = 0;

	return i40iw_reg_phys_mr(pd, 0, 0, acc, &kva);
}

/**
 * i40iw_del_memlist - Deleting pbl list entries for CQ/QP
 * @iwmr: iwmr for IB's user page addresses
 * @ucontext: ptr to user context
 */
static void i40iw_del_memlist(struct i40iw_mr *iwmr,
			      struct i40iw_ucontext *ucontext)
{
	struct i40iw_pbl *iwpbl = &iwmr->iwpbl;
	unsigned long flags;

	switch (iwmr->type) {
	case IW_MEMREG_TYPE_CQ:
		spin_lock_irqsave(&ucontext->cq_reg_mem_list_lock, flags);
		if (iwpbl->on_list) {
			iwpbl->on_list = false;
			list_del(&iwpbl->list);
		}
		spin_unlock_irqrestore(&ucontext->cq_reg_mem_list_lock, flags);
		break;
	case IW_MEMREG_TYPE_QP:
		spin_lock_irqsave(&ucontext->qp_reg_mem_list_lock, flags);
		if (iwpbl->on_list) {
			iwpbl->on_list = false;
			list_del(&iwpbl->list);
		}
		spin_unlock_irqrestore(&ucontext->qp_reg_mem_list_lock, flags);
		break;
	default:
		break;
	}
}

/**
 * i40iw_dereg_mr - deregister mr
 * @ib_mr: mr ptr for dereg
 * @udata: user data
 */
static int i40iw_dereg_mr(struct ib_mr *ib_mr, struct ib_udata *udata)
{
	struct ib_pd *ibpd = ib_mr->pd;
	struct i40iw_pd *iwpd = to_iwpd(ibpd);
	struct i40iw_mr *iwmr = to_iwmr(ib_mr);
	struct i40iw_device *iwdev = to_iwdev(ib_mr->device);
	enum i40iw_status_code status;
	struct i40iw_dealloc_stag_info *info;
	struct i40iw_pbl *iwpbl = &iwmr->iwpbl;
	struct i40iw_pble_alloc *palloc = &iwpbl->pble_alloc;
	struct i40iw_cqp_request *cqp_request;
	struct cqp_commands_info *cqp_info;
	u32 stag_idx;

	ib_umem_release(iwmr->region);

	if (iwmr->type != IW_MEMREG_TYPE_MEM) {
		/* region is released. only test for userness. */
		if (iwmr->region) {
			struct i40iw_ucontext *ucontext =
				rdma_udata_to_drv_context(
					udata,
					struct i40iw_ucontext,
					ibucontext);

			i40iw_del_memlist(iwmr, ucontext);
		}
		if (iwpbl->pbl_allocated && iwmr->type != IW_MEMREG_TYPE_QP)
			i40iw_free_pble(iwdev->pble_rsrc, palloc);
		kfree(iwmr);
		return 0;
	}

	cqp_request = i40iw_get_cqp_request(&iwdev->cqp, true);
	if (!cqp_request)
		return -ENOMEM;

	cqp_info = &cqp_request->info;
	info = &cqp_info->in.u.dealloc_stag.info;
	memset(info, 0, sizeof(*info));

	info->pd_id = cpu_to_le32(iwpd->sc_pd.pd_id & 0x00007fff);
	info->stag_idx = RS_64_1(ib_mr->rkey, I40IW_CQPSQ_STAG_IDX_SHIFT);
	stag_idx = info->stag_idx;
	info->mr = true;
	if (iwpbl->pbl_allocated)
		info->dealloc_pbl = true;

	cqp_info->cqp_cmd = OP_DEALLOC_STAG;
	cqp_info->post_sq = 1;
	cqp_info->in.u.dealloc_stag.dev = &iwdev->sc_dev;
	cqp_info->in.u.dealloc_stag.scratch = (uintptr_t)cqp_request;
	status = i40iw_handle_cqp_op(iwdev, cqp_request);
	if (status)
		i40iw_pr_err("CQP-OP dealloc failed for stag_idx = 0x%x\n", stag_idx);
	i40iw_rem_pdusecount(iwpd, iwdev);
	i40iw_free_stag(iwdev, iwmr->stag);
	if (iwpbl->pbl_allocated)
		i40iw_free_pble(iwdev->pble_rsrc, palloc);
	kfree(iwmr);
	return 0;
}

/*
 * hw_rev_show
 */
static ssize_t hw_rev_show(struct device *dev,
			   struct device_attribute *attr, char *buf)
{
	struct i40iw_ib_device *iwibdev =
		rdma_device_to_drv_device(dev, struct i40iw_ib_device, ibdev);
	u32 hw_rev = iwibdev->iwdev->sc_dev.hw_rev;

	return sysfs_emit(buf, "%x\n", hw_rev);
}
static DEVICE_ATTR_RO(hw_rev);

/*
 * hca_type_show
 */
static ssize_t hca_type_show(struct device *dev,
			     struct device_attribute *attr, char *buf)
{
	return sysfs_emit(buf, "I40IW\n");
}
static DEVICE_ATTR_RO(hca_type);

/*
 * board_id_show
 */
static ssize_t board_id_show(struct device *dev,
			     struct device_attribute *attr, char *buf)
{
	return sysfs_emit(buf, "%.*s\n", 32, "I40IW Board ID");
}
static DEVICE_ATTR_RO(board_id);

static struct attribute *i40iw_dev_attributes[] = {
	&dev_attr_hw_rev.attr,
	&dev_attr_hca_type.attr,
	&dev_attr_board_id.attr,
	NULL
};

static const struct attribute_group i40iw_attr_group = {
	.attrs = i40iw_dev_attributes,
};

/**
 * i40iw_copy_sg_list - copy sg list for qp
 * @sg_list: copied into sg_list
 * @sgl: copy from sgl
 * @num_sges: count of sg entries
 */
static void i40iw_copy_sg_list(struct i40iw_sge *sg_list, struct ib_sge *sgl, int num_sges)
{
	unsigned int i;

	for (i = 0; (i < num_sges) && (i < I40IW_MAX_WQ_FRAGMENT_COUNT); i++) {
		sg_list[i].tag_off = sgl[i].addr;
		sg_list[i].len = sgl[i].length;
		sg_list[i].stag = sgl[i].lkey;
	}
}

/**
 * i40iw_post_send -  kernel application wr
 * @ibqp: qp ptr for wr
 * @ib_wr: work request ptr
 * @bad_wr: return of bad wr if err
 */
static int i40iw_post_send(struct ib_qp *ibqp,
			   const struct ib_send_wr *ib_wr,
			   const struct ib_send_wr **bad_wr)
{
	struct i40iw_qp *iwqp;
	struct i40iw_qp_uk *ukqp;
	struct i40iw_post_sq_info info;
	enum i40iw_status_code ret;
	int err = 0;
	unsigned long flags;
	bool inv_stag;

	iwqp = (struct i40iw_qp *)ibqp;
	ukqp = &iwqp->sc_qp.qp_uk;

	spin_lock_irqsave(&iwqp->lock, flags);

	if (iwqp->flush_issued) {
		err = -EINVAL;
		goto out;
	}

	while (ib_wr) {
		inv_stag = false;
		memset(&info, 0, sizeof(info));
		info.wr_id = (u64)(ib_wr->wr_id);
		if ((ib_wr->send_flags & IB_SEND_SIGNALED) || iwqp->sig_all)
			info.signaled = true;
		if (ib_wr->send_flags & IB_SEND_FENCE)
			info.read_fence = true;

		switch (ib_wr->opcode) {
		case IB_WR_SEND:
		case IB_WR_SEND_WITH_INV:
			if (ib_wr->opcode == IB_WR_SEND) {
				if (ib_wr->send_flags & IB_SEND_SOLICITED)
					info.op_type = I40IW_OP_TYPE_SEND_SOL;
				else
					info.op_type = I40IW_OP_TYPE_SEND;
			} else {
				if (ib_wr->send_flags & IB_SEND_SOLICITED)
					info.op_type = I40IW_OP_TYPE_SEND_SOL_INV;
				else
					info.op_type = I40IW_OP_TYPE_SEND_INV;
			}

			if (ib_wr->send_flags & IB_SEND_INLINE) {
				info.op.inline_send.data = (void *)(unsigned long)ib_wr->sg_list[0].addr;
				info.op.inline_send.len = ib_wr->sg_list[0].length;
				ret = ukqp->ops.iw_inline_send(ukqp, &info, ib_wr->ex.invalidate_rkey, false);
			} else {
				info.op.send.num_sges = ib_wr->num_sge;
				info.op.send.sg_list = (struct i40iw_sge *)ib_wr->sg_list;
				ret = ukqp->ops.iw_send(ukqp, &info, ib_wr->ex.invalidate_rkey, false);
			}

			if (ret) {
				if (ret == I40IW_ERR_QP_TOOMANY_WRS_POSTED)
					err = -ENOMEM;
				else
					err = -EINVAL;
			}
			break;
		case IB_WR_RDMA_WRITE:
			info.op_type = I40IW_OP_TYPE_RDMA_WRITE;

			if (ib_wr->send_flags & IB_SEND_INLINE) {
				info.op.inline_rdma_write.data = (void *)(unsigned long)ib_wr->sg_list[0].addr;
				info.op.inline_rdma_write.len = ib_wr->sg_list[0].length;
				info.op.inline_rdma_write.rem_addr.tag_off = rdma_wr(ib_wr)->remote_addr;
				info.op.inline_rdma_write.rem_addr.stag = rdma_wr(ib_wr)->rkey;
				ret = ukqp->ops.iw_inline_rdma_write(ukqp, &info, false);
			} else {
				info.op.rdma_write.lo_sg_list = (void *)ib_wr->sg_list;
				info.op.rdma_write.num_lo_sges = ib_wr->num_sge;
				info.op.rdma_write.rem_addr.tag_off = rdma_wr(ib_wr)->remote_addr;
				info.op.rdma_write.rem_addr.stag = rdma_wr(ib_wr)->rkey;
				ret = ukqp->ops.iw_rdma_write(ukqp, &info, false);
			}

			if (ret) {
				if (ret == I40IW_ERR_QP_TOOMANY_WRS_POSTED)
					err = -ENOMEM;
				else
					err = -EINVAL;
			}
			break;
		case IB_WR_RDMA_READ_WITH_INV:
			inv_stag = true;
			fallthrough;
		case IB_WR_RDMA_READ:
			if (ib_wr->num_sge > I40IW_MAX_SGE_RD) {
				err = -EINVAL;
				break;
			}
			info.op_type = I40IW_OP_TYPE_RDMA_READ;
			info.op.rdma_read.rem_addr.tag_off = rdma_wr(ib_wr)->remote_addr;
			info.op.rdma_read.rem_addr.stag = rdma_wr(ib_wr)->rkey;
			info.op.rdma_read.lo_addr.tag_off = ib_wr->sg_list->addr;
			info.op.rdma_read.lo_addr.stag = ib_wr->sg_list->lkey;
			info.op.rdma_read.lo_addr.len = ib_wr->sg_list->length;
			ret = ukqp->ops.iw_rdma_read(ukqp, &info, inv_stag, false);
			if (ret) {
				if (ret == I40IW_ERR_QP_TOOMANY_WRS_POSTED)
					err = -ENOMEM;
				else
					err = -EINVAL;
			}
			break;
		case IB_WR_LOCAL_INV:
			info.op_type = I40IW_OP_TYPE_INV_STAG;
			info.op.inv_local_stag.target_stag = ib_wr->ex.invalidate_rkey;
			ret = ukqp->ops.iw_stag_local_invalidate(ukqp, &info, true);
			if (ret)
				err = -ENOMEM;
			break;
		case IB_WR_REG_MR:
		{
			struct i40iw_mr *iwmr = to_iwmr(reg_wr(ib_wr)->mr);
			int flags = reg_wr(ib_wr)->access;
			struct i40iw_pble_alloc *palloc = &iwmr->iwpbl.pble_alloc;
			struct i40iw_sc_dev *dev = &iwqp->iwdev->sc_dev;
			struct i40iw_fast_reg_stag_info info;

			memset(&info, 0, sizeof(info));
			info.access_rights = I40IW_ACCESS_FLAGS_LOCALREAD;
			info.access_rights |= i40iw_get_user_access(flags);
			info.stag_key = reg_wr(ib_wr)->key & 0xff;
			info.stag_idx = reg_wr(ib_wr)->key >> 8;
			info.page_size = reg_wr(ib_wr)->mr->page_size;
			info.wr_id = ib_wr->wr_id;

			info.addr_type = I40IW_ADDR_TYPE_VA_BASED;
			info.va = (void *)(uintptr_t)iwmr->ibmr.iova;
			info.total_len = iwmr->ibmr.length;
			info.reg_addr_pa = *(u64 *)palloc->level1.addr;
			info.first_pm_pbl_index = palloc->level1.idx;
			info.local_fence = ib_wr->send_flags & IB_SEND_FENCE;
			info.signaled = ib_wr->send_flags & IB_SEND_SIGNALED;

			if (iwmr->npages > I40IW_MIN_PAGES_PER_FMR)
				info.chunk_size = 1;

			ret = dev->iw_priv_qp_ops->iw_mr_fast_register(&iwqp->sc_qp, &info, true);
			if (ret)
				err = -ENOMEM;
			break;
		}
		default:
			err = -EINVAL;
			i40iw_pr_err(" upost_send bad opcode = 0x%x\n",
				     ib_wr->opcode);
			break;
		}

		if (err)
			break;
		ib_wr = ib_wr->next;
	}

out:
	if (err)
		*bad_wr = ib_wr;
	else
		ukqp->ops.iw_qp_post_wr(ukqp);
	spin_unlock_irqrestore(&iwqp->lock, flags);

	return err;
}

/**
 * i40iw_post_recv - post receive wr for kernel application
 * @ibqp: ib qp pointer
 * @ib_wr: work request for receive
 * @bad_wr: bad wr caused an error
 */
static int i40iw_post_recv(struct ib_qp *ibqp, const struct ib_recv_wr *ib_wr,
			   const struct ib_recv_wr **bad_wr)
{
	struct i40iw_qp *iwqp;
	struct i40iw_qp_uk *ukqp;
	struct i40iw_post_rq_info post_recv;
	struct i40iw_sge sg_list[I40IW_MAX_WQ_FRAGMENT_COUNT];
	enum i40iw_status_code ret = 0;
	unsigned long flags;
	int err = 0;

	iwqp = (struct i40iw_qp *)ibqp;
	ukqp = &iwqp->sc_qp.qp_uk;

	memset(&post_recv, 0, sizeof(post_recv));
	spin_lock_irqsave(&iwqp->lock, flags);

	if (iwqp->flush_issued) {
		err = -EINVAL;
		goto out;
	}

	while (ib_wr) {
		post_recv.num_sges = ib_wr->num_sge;
		post_recv.wr_id = ib_wr->wr_id;
		i40iw_copy_sg_list(sg_list, ib_wr->sg_list, ib_wr->num_sge);
		post_recv.sg_list = sg_list;
		ret = ukqp->ops.iw_post_receive(ukqp, &post_recv);
		if (ret) {
			i40iw_pr_err(" post_recv err %d\n", ret);
			if (ret == I40IW_ERR_QP_TOOMANY_WRS_POSTED)
				err = -ENOMEM;
			else
				err = -EINVAL;
			*bad_wr = ib_wr;
			goto out;
		}
		ib_wr = ib_wr->next;
	}
 out:
	spin_unlock_irqrestore(&iwqp->lock, flags);
	return err;
}

/**
 * i40iw_poll_cq - poll cq for completion (kernel apps)
 * @ibcq: cq to poll
 * @num_entries: number of entries to poll
 * @entry: wr of entry completed
 */
static int i40iw_poll_cq(struct ib_cq *ibcq,
			 int num_entries,
			 struct ib_wc *entry)
{
	struct i40iw_cq *iwcq;
	int cqe_count = 0;
	struct i40iw_cq_poll_info cq_poll_info;
	enum i40iw_status_code ret;
	struct i40iw_cq_uk *ukcq;
	struct i40iw_sc_qp *qp;
	struct i40iw_qp *iwqp;
	unsigned long flags;

	iwcq = (struct i40iw_cq *)ibcq;
	ukcq = &iwcq->sc_cq.cq_uk;

	spin_lock_irqsave(&iwcq->lock, flags);
	while (cqe_count < num_entries) {
		ret = ukcq->ops.iw_cq_poll_completion(ukcq, &cq_poll_info);
		if (ret == I40IW_ERR_QUEUE_EMPTY) {
			break;
		} else if (ret == I40IW_ERR_QUEUE_DESTROYED) {
			continue;
		} else if (ret) {
			if (!cqe_count)
				cqe_count = -1;
			break;
		}
		entry->wc_flags = 0;
		entry->wr_id = cq_poll_info.wr_id;
		if (cq_poll_info.error) {
			entry->status = IB_WC_WR_FLUSH_ERR;
			entry->vendor_err = cq_poll_info.major_err << 16 | cq_poll_info.minor_err;
		} else {
			entry->status = IB_WC_SUCCESS;
		}

		switch (cq_poll_info.op_type) {
		case I40IW_OP_TYPE_RDMA_WRITE:
			entry->opcode = IB_WC_RDMA_WRITE;
			break;
		case I40IW_OP_TYPE_RDMA_READ_INV_STAG:
		case I40IW_OP_TYPE_RDMA_READ:
			entry->opcode = IB_WC_RDMA_READ;
			break;
		case I40IW_OP_TYPE_SEND_SOL:
		case I40IW_OP_TYPE_SEND_SOL_INV:
		case I40IW_OP_TYPE_SEND_INV:
		case I40IW_OP_TYPE_SEND:
			entry->opcode = IB_WC_SEND;
			break;
		case I40IW_OP_TYPE_REC:
			entry->opcode = IB_WC_RECV;
			break;
		default:
			entry->opcode = IB_WC_RECV;
			break;
		}

		entry->ex.imm_data = 0;
		qp = (struct i40iw_sc_qp *)cq_poll_info.qp_handle;
		entry->qp = (struct ib_qp *)qp->back_qp;
		entry->src_qp = cq_poll_info.qp_id;
		iwqp = (struct i40iw_qp *)qp->back_qp;
		if (iwqp->iwarp_state > I40IW_QP_STATE_RTS) {
			if (!I40IW_RING_MORE_WORK(qp->qp_uk.sq_ring))
				complete(&iwqp->sq_drained);
			if (!I40IW_RING_MORE_WORK(qp->qp_uk.rq_ring))
				complete(&iwqp->rq_drained);
		}
		entry->byte_len = cq_poll_info.bytes_xfered;
		entry++;
		cqe_count++;
	}
	spin_unlock_irqrestore(&iwcq->lock, flags);
	return cqe_count;
}

/**
 * i40iw_req_notify_cq - arm cq kernel application
 * @ibcq: cq to arm
 * @notify_flags: notofication flags
 */
static int i40iw_req_notify_cq(struct ib_cq *ibcq,
			       enum ib_cq_notify_flags notify_flags)
{
	struct i40iw_cq *iwcq;
	struct i40iw_cq_uk *ukcq;
	unsigned long flags;
	enum i40iw_completion_notify cq_notify = IW_CQ_COMPL_EVENT;

	iwcq = (struct i40iw_cq *)ibcq;
	ukcq = &iwcq->sc_cq.cq_uk;
	if (notify_flags == IB_CQ_SOLICITED)
		cq_notify = IW_CQ_COMPL_SOLICITED;
	spin_lock_irqsave(&iwcq->lock, flags);
	ukcq->ops.iw_cq_request_notification(ukcq, cq_notify);
	spin_unlock_irqrestore(&iwcq->lock, flags);
	return 0;
}

/**
 * i40iw_port_immutable - return port's immutable data
 * @ibdev: ib dev struct
 * @port_num: port number
 * @immutable: immutable data for the port return
 */
static int i40iw_port_immutable(struct ib_device *ibdev, u32 port_num,
				struct ib_port_immutable *immutable)
{
	struct ib_port_attr attr;
	int err;

	immutable->core_cap_flags = RDMA_CORE_PORT_IWARP;

	err = ib_query_port(ibdev, port_num, &attr);

	if (err)
		return err;

	immutable->gid_tbl_len = attr.gid_tbl_len;

	return 0;
}

static const char * const i40iw_hw_stat_names[] = {
	// 32bit names
	[I40IW_HW_STAT_INDEX_IP4RXDISCARD] = "ip4InDiscards",
	[I40IW_HW_STAT_INDEX_IP4RXTRUNC] = "ip4InTruncatedPkts",
	[I40IW_HW_STAT_INDEX_IP4TXNOROUTE] = "ip4OutNoRoutes",
	[I40IW_HW_STAT_INDEX_IP6RXDISCARD] = "ip6InDiscards",
	[I40IW_HW_STAT_INDEX_IP6RXTRUNC] = "ip6InTruncatedPkts",
	[I40IW_HW_STAT_INDEX_IP6TXNOROUTE] = "ip6OutNoRoutes",
	[I40IW_HW_STAT_INDEX_TCPRTXSEG] = "tcpRetransSegs",
	[I40IW_HW_STAT_INDEX_TCPRXOPTERR] = "tcpInOptErrors",
	[I40IW_HW_STAT_INDEX_TCPRXPROTOERR] = "tcpInProtoErrors",
	// 64bit names
	[I40IW_HW_STAT_INDEX_IP4RXOCTS + I40IW_HW_STAT_INDEX_MAX_32] =
		"ip4InOctets",
	[I40IW_HW_STAT_INDEX_IP4RXPKTS + I40IW_HW_STAT_INDEX_MAX_32] =
		"ip4InPkts",
	[I40IW_HW_STAT_INDEX_IP4RXFRAGS + I40IW_HW_STAT_INDEX_MAX_32] =
		"ip4InReasmRqd",
	[I40IW_HW_STAT_INDEX_IP4RXMCPKTS + I40IW_HW_STAT_INDEX_MAX_32] =
		"ip4InMcastPkts",
	[I40IW_HW_STAT_INDEX_IP4TXOCTS + I40IW_HW_STAT_INDEX_MAX_32] =
		"ip4OutOctets",
	[I40IW_HW_STAT_INDEX_IP4TXPKTS + I40IW_HW_STAT_INDEX_MAX_32] =
		"ip4OutPkts",
	[I40IW_HW_STAT_INDEX_IP4TXFRAGS + I40IW_HW_STAT_INDEX_MAX_32] =
		"ip4OutSegRqd",
	[I40IW_HW_STAT_INDEX_IP4TXMCPKTS + I40IW_HW_STAT_INDEX_MAX_32] =
		"ip4OutMcastPkts",
	[I40IW_HW_STAT_INDEX_IP6RXOCTS + I40IW_HW_STAT_INDEX_MAX_32] =
		"ip6InOctets",
	[I40IW_HW_STAT_INDEX_IP6RXPKTS + I40IW_HW_STAT_INDEX_MAX_32] =
		"ip6InPkts",
	[I40IW_HW_STAT_INDEX_IP6RXFRAGS + I40IW_HW_STAT_INDEX_MAX_32] =
		"ip6InReasmRqd",
	[I40IW_HW_STAT_INDEX_IP6RXMCPKTS + I40IW_HW_STAT_INDEX_MAX_32] =
		"ip6InMcastPkts",
	[I40IW_HW_STAT_INDEX_IP6TXOCTS + I40IW_HW_STAT_INDEX_MAX_32] =
		"ip6OutOctets",
	[I40IW_HW_STAT_INDEX_IP6TXPKTS + I40IW_HW_STAT_INDEX_MAX_32] =
		"ip6OutPkts",
	[I40IW_HW_STAT_INDEX_IP6TXFRAGS + I40IW_HW_STAT_INDEX_MAX_32] =
		"ip6OutSegRqd",
	[I40IW_HW_STAT_INDEX_IP6TXMCPKTS + I40IW_HW_STAT_INDEX_MAX_32] =
		"ip6OutMcastPkts",
	[I40IW_HW_STAT_INDEX_TCPRXSEGS + I40IW_HW_STAT_INDEX_MAX_32] =
		"tcpInSegs",
	[I40IW_HW_STAT_INDEX_TCPTXSEG + I40IW_HW_STAT_INDEX_MAX_32] =
		"tcpOutSegs",
	[I40IW_HW_STAT_INDEX_RDMARXRDS + I40IW_HW_STAT_INDEX_MAX_32] =
		"iwInRdmaReads",
	[I40IW_HW_STAT_INDEX_RDMARXSNDS + I40IW_HW_STAT_INDEX_MAX_32] =
		"iwInRdmaSends",
	[I40IW_HW_STAT_INDEX_RDMARXWRS + I40IW_HW_STAT_INDEX_MAX_32] =
		"iwInRdmaWrites",
	[I40IW_HW_STAT_INDEX_RDMATXRDS + I40IW_HW_STAT_INDEX_MAX_32] =
		"iwOutRdmaReads",
	[I40IW_HW_STAT_INDEX_RDMATXSNDS + I40IW_HW_STAT_INDEX_MAX_32] =
		"iwOutRdmaSends",
	[I40IW_HW_STAT_INDEX_RDMATXWRS + I40IW_HW_STAT_INDEX_MAX_32] =
		"iwOutRdmaWrites",
	[I40IW_HW_STAT_INDEX_RDMAVBND + I40IW_HW_STAT_INDEX_MAX_32] =
		"iwRdmaBnd",
	[I40IW_HW_STAT_INDEX_RDMAVINV + I40IW_HW_STAT_INDEX_MAX_32] =
		"iwRdmaInv"
};

static void i40iw_get_dev_fw_str(struct ib_device *dev, char *str)
{
	struct i40iw_device *iwdev = to_iwdev(dev);

	snprintf(str, IB_FW_VERSION_NAME_MAX, "%llu.%llu",
		 i40iw_fw_major_ver(&iwdev->sc_dev),
		 i40iw_fw_minor_ver(&iwdev->sc_dev));
}

/**
 * i40iw_alloc_hw_stats - Allocate a hw stats structure
 * @ibdev: device pointer from stack
 * @port_num: port number
 */
static struct rdma_hw_stats *i40iw_alloc_hw_stats(struct ib_device *ibdev,
						  u32 port_num)
{
	struct i40iw_device *iwdev = to_iwdev(ibdev);
	struct i40iw_sc_dev *dev = &iwdev->sc_dev;
	int num_counters = I40IW_HW_STAT_INDEX_MAX_32 +
		I40IW_HW_STAT_INDEX_MAX_64;
	unsigned long lifespan = RDMA_HW_STATS_DEFAULT_LIFESPAN;

	BUILD_BUG_ON(ARRAY_SIZE(i40iw_hw_stat_names) !=
		     (I40IW_HW_STAT_INDEX_MAX_32 +
		      I40IW_HW_STAT_INDEX_MAX_64));

	/*
	 * PFs get the default update lifespan, but VFs only update once
	 * per second
	 */
	if (!dev->is_pf)
		lifespan = 1000;
	return rdma_alloc_hw_stats_struct(i40iw_hw_stat_names, num_counters,
					  lifespan);
}

/**
 * i40iw_get_hw_stats - Populates the rdma_hw_stats structure
 * @ibdev: device pointer from stack
 * @stats: stats pointer from stack
 * @port_num: port number
 * @index: which hw counter the stack is requesting we update
 */
static int i40iw_get_hw_stats(struct ib_device *ibdev,
			      struct rdma_hw_stats *stats,
			      u32 port_num, int index)
{
	struct i40iw_device *iwdev = to_iwdev(ibdev);
	struct i40iw_sc_dev *dev = &iwdev->sc_dev;
	struct i40iw_vsi_pestat *devstat = iwdev->vsi.pestat;
	struct i40iw_dev_hw_stats *hw_stats = &devstat->hw_stats;

	if (dev->is_pf) {
		i40iw_hw_stats_read_all(devstat, &devstat->hw_stats);
	} else {
		if (i40iw_vchnl_vf_get_pe_stats(dev, &devstat->hw_stats))
			return -ENOSYS;
	}

	memcpy(&stats->value[0], hw_stats, sizeof(*hw_stats));

	return stats->num_counters;
}

/**
 * i40iw_query_gid - Query port GID
 * @ibdev: device pointer from stack
 * @port: port number
 * @index: Entry index
 * @gid: Global ID
 */
static int i40iw_query_gid(struct ib_device *ibdev,
			   u32 port,
			   int index,
			   union ib_gid *gid)
{
	struct i40iw_device *iwdev = to_iwdev(ibdev);

	memset(gid->raw, 0, sizeof(gid->raw));
	ether_addr_copy(gid->raw, iwdev->netdev->dev_addr);
	return 0;
}

static const struct ib_device_ops i40iw_dev_ops = {
	.owner = THIS_MODULE,
	.driver_id = RDMA_DRIVER_I40IW,
	/* NOTE: Older kernels wrongly use 0 for the uverbs_abi_ver */
	.uverbs_abi_ver = I40IW_ABI_VER,

	.alloc_hw_stats = i40iw_alloc_hw_stats,
	.alloc_mr = i40iw_alloc_mr,
	.alloc_pd = i40iw_alloc_pd,
	.alloc_ucontext = i40iw_alloc_ucontext,
	.create_cq = i40iw_create_cq,
	.create_qp = i40iw_create_qp,
	.dealloc_pd = i40iw_dealloc_pd,
	.dealloc_ucontext = i40iw_dealloc_ucontext,
	.dereg_mr = i40iw_dereg_mr,
	.destroy_cq = i40iw_destroy_cq,
	.destroy_qp = i40iw_destroy_qp,
	.drain_rq = i40iw_drain_rq,
	.drain_sq = i40iw_drain_sq,
	.get_dev_fw_str = i40iw_get_dev_fw_str,
	.get_dma_mr = i40iw_get_dma_mr,
	.get_hw_stats = i40iw_get_hw_stats,
	.get_port_immutable = i40iw_port_immutable,
	.iw_accept = i40iw_accept,
	.iw_add_ref = i40iw_qp_add_ref,
	.iw_connect = i40iw_connect,
	.iw_create_listen = i40iw_create_listen,
	.iw_destroy_listen = i40iw_destroy_listen,
	.iw_get_qp = i40iw_get_qp,
	.iw_reject = i40iw_reject,
	.iw_rem_ref = i40iw_qp_rem_ref,
	.map_mr_sg = i40iw_map_mr_sg,
	.mmap = i40iw_mmap,
	.modify_qp = i40iw_modify_qp,
	.poll_cq = i40iw_poll_cq,
	.post_recv = i40iw_post_recv,
	.post_send = i40iw_post_send,
	.query_device = i40iw_query_device,
	.query_gid = i40iw_query_gid,
	.query_port = i40iw_query_port,
	.query_qp = i40iw_query_qp,
	.reg_user_mr = i40iw_reg_user_mr,
	.req_notify_cq = i40iw_req_notify_cq,
	INIT_RDMA_OBJ_SIZE(ib_pd, i40iw_pd, ibpd),
	INIT_RDMA_OBJ_SIZE(ib_cq, i40iw_cq, ibcq),
	INIT_RDMA_OBJ_SIZE(ib_ucontext, i40iw_ucontext, ibucontext),
};

/**
 * i40iw_init_rdma_device - initialization of iwarp device
 * @iwdev: iwarp device
 */
static struct i40iw_ib_device *i40iw_init_rdma_device(struct i40iw_device *iwdev)
{
	struct i40iw_ib_device *iwibdev;
	struct net_device *netdev = iwdev->netdev;
	struct pci_dev *pcidev = iwdev->hw.pcidev;

	iwibdev = ib_alloc_device(i40iw_ib_device, ibdev);
	if (!iwibdev) {
		i40iw_pr_err("iwdev == NULL\n");
		return NULL;
	}
	iwdev->iwibdev = iwibdev;
	iwibdev->iwdev = iwdev;

	iwibdev->ibdev.node_type = RDMA_NODE_RNIC;
	ether_addr_copy((u8 *)&iwibdev->ibdev.node_guid, netdev->dev_addr);

	iwibdev->ibdev.phys_port_cnt = 1;
	iwibdev->ibdev.num_comp_vectors = iwdev->ceqs_count;
	iwibdev->ibdev.dev.parent = &pcidev->dev;
	memcpy(iwibdev->ibdev.iw_ifname, netdev->name,
	       sizeof(iwibdev->ibdev.iw_ifname));
	ib_set_device_ops(&iwibdev->ibdev, &i40iw_dev_ops);

	return iwibdev;
}

/**
 * i40iw_port_ibevent - indicate port event
 * @iwdev: iwarp device
 */
void i40iw_port_ibevent(struct i40iw_device *iwdev)
{
	struct i40iw_ib_device *iwibdev = iwdev->iwibdev;
	struct ib_event event;

	event.device = &iwibdev->ibdev;
	event.element.port_num = 1;
	event.event = iwdev->iw_status ? IB_EVENT_PORT_ACTIVE : IB_EVENT_PORT_ERR;
	ib_dispatch_event(&event);
}

/**
 * i40iw_destroy_rdma_device - destroy rdma device and free resources
 * @iwibdev: IB device ptr
 */
void i40iw_destroy_rdma_device(struct i40iw_ib_device *iwibdev)
{
	ib_unregister_device(&iwibdev->ibdev);
	wait_event_timeout(iwibdev->iwdev->close_wq,
			   !atomic64_read(&iwibdev->iwdev->use_count),
			   I40IW_EVENT_TIMEOUT);
	ib_dealloc_device(&iwibdev->ibdev);
}

/**
 * i40iw_register_rdma_device - register iwarp device to IB
 * @iwdev: iwarp device
 */
int i40iw_register_rdma_device(struct i40iw_device *iwdev)
{
	int ret;
	struct i40iw_ib_device *iwibdev;

	iwdev->iwibdev = i40iw_init_rdma_device(iwdev);
	if (!iwdev->iwibdev)
		return -ENOMEM;
	iwibdev = iwdev->iwibdev;
	rdma_set_device_sysfs_group(&iwibdev->ibdev, &i40iw_attr_group);
	ret = ib_device_set_netdev(&iwibdev->ibdev, iwdev->netdev, 1);
	if (ret)
		goto error;

	dma_set_max_seg_size(&iwdev->hw.pcidev->dev, UINT_MAX);
	ret = ib_register_device(&iwibdev->ibdev, "i40iw%d", &iwdev->hw.pcidev->dev);
	if (ret)
		goto error;

	return 0;
error:
	ib_dealloc_device(&iwdev->iwibdev->ibdev);
	return ret;
}
