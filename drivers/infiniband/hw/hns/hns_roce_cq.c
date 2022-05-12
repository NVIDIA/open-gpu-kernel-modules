/*
 * Copyright (c) 2016 Hisilicon Limited.
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

#include <linux/platform_device.h>
#include <rdma/ib_umem.h>
#include <rdma/uverbs_ioctl.h>
#include "hns_roce_device.h"
#include "hns_roce_cmd.h"
#include "hns_roce_hem.h"
#include "hns_roce_common.h"

static u8 get_least_load_bankid_for_cq(struct hns_roce_bank *bank)
{
	u32 least_load = bank[0].inuse;
	u8 bankid = 0;
	u32 bankcnt;
	u8 i;

	for (i = 1; i < HNS_ROCE_CQ_BANK_NUM; i++) {
		bankcnt = bank[i].inuse;
		if (bankcnt < least_load) {
			least_load = bankcnt;
			bankid = i;
		}
	}

	return bankid;
}

static int alloc_cqn(struct hns_roce_dev *hr_dev, struct hns_roce_cq *hr_cq)
{
	struct hns_roce_cq_table *cq_table = &hr_dev->cq_table;
	struct hns_roce_bank *bank;
	u8 bankid;
	int id;

	mutex_lock(&cq_table->bank_mutex);
	bankid = get_least_load_bankid_for_cq(cq_table->bank);
	bank = &cq_table->bank[bankid];

	id = ida_alloc_range(&bank->ida, bank->min, bank->max, GFP_KERNEL);
	if (id < 0) {
		mutex_unlock(&cq_table->bank_mutex);
		return id;
	}

	/* the lower 2 bits is bankid */
	hr_cq->cqn = (id << CQ_BANKID_SHIFT) | bankid;
	bank->inuse++;
	mutex_unlock(&cq_table->bank_mutex);

	return 0;
}

static inline u8 get_cq_bankid(unsigned long cqn)
{
	/* The lower 2 bits of CQN are used to hash to different banks */
	return (u8)(cqn & GENMASK(1, 0));
}

static void free_cqn(struct hns_roce_dev *hr_dev, unsigned long cqn)
{
	struct hns_roce_cq_table *cq_table = &hr_dev->cq_table;
	struct hns_roce_bank *bank;

	bank = &cq_table->bank[get_cq_bankid(cqn)];

	ida_free(&bank->ida, cqn >> CQ_BANKID_SHIFT);

	mutex_lock(&cq_table->bank_mutex);
	bank->inuse--;
	mutex_unlock(&cq_table->bank_mutex);
}

static int alloc_cqc(struct hns_roce_dev *hr_dev, struct hns_roce_cq *hr_cq)
{
	struct hns_roce_cq_table *cq_table = &hr_dev->cq_table;
	struct ib_device *ibdev = &hr_dev->ib_dev;
	struct hns_roce_cmd_mailbox *mailbox;
	u64 mtts[MTT_MIN_COUNT] = { 0 };
	dma_addr_t dma_handle;
	int ret;

	ret = hns_roce_mtr_find(hr_dev, &hr_cq->mtr, 0, mtts, ARRAY_SIZE(mtts),
				&dma_handle);
	if (!ret) {
		ibdev_err(ibdev, "failed to find CQ mtr, ret = %d.\n", ret);
		return -EINVAL;
	}

	/* Get CQC memory HEM(Hardware Entry Memory) table */
	ret = hns_roce_table_get(hr_dev, &cq_table->table, hr_cq->cqn);
	if (ret) {
		ibdev_err(ibdev, "failed to get CQ(0x%lx) context, ret = %d.\n",
			  hr_cq->cqn, ret);
		goto err_out;
	}

	ret = xa_err(xa_store(&cq_table->array, hr_cq->cqn, hr_cq, GFP_KERNEL));
	if (ret) {
		ibdev_err(ibdev, "failed to xa_store CQ, ret = %d.\n", ret);
		goto err_put;
	}

	/* Allocate mailbox memory */
	mailbox = hns_roce_alloc_cmd_mailbox(hr_dev);
	if (IS_ERR(mailbox)) {
		ret = PTR_ERR(mailbox);
		goto err_xa;
	}

	hr_dev->hw->write_cqc(hr_dev, hr_cq, mailbox->buf, mtts, dma_handle);

	/* Send mailbox to hw */
	ret = hns_roce_cmd_mbox(hr_dev, mailbox->dma, 0, hr_cq->cqn, 0,
			HNS_ROCE_CMD_CREATE_CQC, HNS_ROCE_CMD_TIMEOUT_MSECS);
	hns_roce_free_cmd_mailbox(hr_dev, mailbox);
	if (ret) {
		ibdev_err(ibdev,
			  "failed to send create cmd for CQ(0x%lx), ret = %d.\n",
			  hr_cq->cqn, ret);
		goto err_xa;
	}

	hr_cq->cons_index = 0;
	hr_cq->arm_sn = 1;

	atomic_set(&hr_cq->refcount, 1);
	init_completion(&hr_cq->free);

	return 0;

err_xa:
	xa_erase(&cq_table->array, hr_cq->cqn);

err_put:
	hns_roce_table_put(hr_dev, &cq_table->table, hr_cq->cqn);

err_out:
	return ret;
}

static void free_cqc(struct hns_roce_dev *hr_dev, struct hns_roce_cq *hr_cq)
{
	struct hns_roce_cq_table *cq_table = &hr_dev->cq_table;
	struct device *dev = hr_dev->dev;
	int ret;

	ret = hns_roce_cmd_mbox(hr_dev, 0, 0, hr_cq->cqn, 1,
				HNS_ROCE_CMD_DESTROY_CQC,
				HNS_ROCE_CMD_TIMEOUT_MSECS);
	if (ret)
		dev_err(dev, "DESTROY_CQ failed (%d) for CQN %06lx\n", ret,
			hr_cq->cqn);

	xa_erase(&cq_table->array, hr_cq->cqn);

	/* Waiting interrupt process procedure carried out */
	synchronize_irq(hr_dev->eq_table.eq[hr_cq->vector].irq);

	/* wait for all interrupt processed */
	if (atomic_dec_and_test(&hr_cq->refcount))
		complete(&hr_cq->free);
	wait_for_completion(&hr_cq->free);

	hns_roce_table_put(hr_dev, &cq_table->table, hr_cq->cqn);
}

static int alloc_cq_buf(struct hns_roce_dev *hr_dev, struct hns_roce_cq *hr_cq,
			struct ib_udata *udata, unsigned long addr)
{
	struct ib_device *ibdev = &hr_dev->ib_dev;
	struct hns_roce_buf_attr buf_attr = {};
	int ret;

	buf_attr.page_shift = hr_dev->caps.cqe_buf_pg_sz + HNS_HW_PAGE_SHIFT;
	buf_attr.region[0].size = hr_cq->cq_depth * hr_cq->cqe_size;
	buf_attr.region[0].hopnum = hr_dev->caps.cqe_hop_num;
	buf_attr.region_count = 1;

	ret = hns_roce_mtr_create(hr_dev, &hr_cq->mtr, &buf_attr,
				  hr_dev->caps.cqe_ba_pg_sz + HNS_HW_PAGE_SHIFT,
				  udata, addr);
	if (ret)
		ibdev_err(ibdev, "failed to alloc CQ mtr, ret = %d.\n", ret);

	return ret;
}

static void free_cq_buf(struct hns_roce_dev *hr_dev, struct hns_roce_cq *hr_cq)
{
	hns_roce_mtr_destroy(hr_dev, &hr_cq->mtr);
}

static int alloc_cq_db(struct hns_roce_dev *hr_dev, struct hns_roce_cq *hr_cq,
		       struct ib_udata *udata, unsigned long addr,
		       struct hns_roce_ib_create_cq_resp *resp)
{
	bool has_db = hr_dev->caps.flags & HNS_ROCE_CAP_FLAG_CQ_RECORD_DB;
	struct hns_roce_ucontext *uctx;
	int err;

	if (udata) {
		if (has_db &&
		    udata->outlen >= offsetofend(typeof(*resp), cap_flags)) {
			uctx = rdma_udata_to_drv_context(udata,
					struct hns_roce_ucontext, ibucontext);
			err = hns_roce_db_map_user(uctx, udata, addr,
						   &hr_cq->db);
			if (err)
				return err;
			hr_cq->flags |= HNS_ROCE_CQ_FLAG_RECORD_DB;
			resp->cap_flags |= HNS_ROCE_CQ_FLAG_RECORD_DB;
		}
	} else {
		if (has_db) {
			err = hns_roce_alloc_db(hr_dev, &hr_cq->db, 1);
			if (err)
				return err;
			hr_cq->set_ci_db = hr_cq->db.db_record;
			*hr_cq->set_ci_db = 0;
			hr_cq->flags |= HNS_ROCE_CQ_FLAG_RECORD_DB;
		}
		hr_cq->db_reg = hr_dev->reg_base + hr_dev->odb_offset +
				DB_REG_OFFSET * hr_dev->priv_uar.index;
	}

	return 0;
}

static void free_cq_db(struct hns_roce_dev *hr_dev, struct hns_roce_cq *hr_cq,
		       struct ib_udata *udata)
{
	struct hns_roce_ucontext *uctx;

	if (!(hr_cq->flags & HNS_ROCE_CQ_FLAG_RECORD_DB))
		return;

	hr_cq->flags &= ~HNS_ROCE_CQ_FLAG_RECORD_DB;
	if (udata) {
		uctx = rdma_udata_to_drv_context(udata,
						 struct hns_roce_ucontext,
						 ibucontext);
		hns_roce_db_unmap_user(uctx, &hr_cq->db);
	} else {
		hns_roce_free_db(hr_dev, &hr_cq->db);
	}
}

static int verify_cq_create_attr(struct hns_roce_dev *hr_dev,
				 const struct ib_cq_init_attr *attr)
{
	struct ib_device *ibdev = &hr_dev->ib_dev;

	if (!attr->cqe || attr->cqe > hr_dev->caps.max_cqes) {
		ibdev_err(ibdev, "failed to check CQ count %u, max = %u.\n",
			  attr->cqe, hr_dev->caps.max_cqes);
		return -EINVAL;
	}

	if (attr->comp_vector >= hr_dev->caps.num_comp_vectors) {
		ibdev_err(ibdev, "failed to check CQ vector = %u, max = %d.\n",
			  attr->comp_vector, hr_dev->caps.num_comp_vectors);
		return -EINVAL;
	}

	return 0;
}

static int get_cq_ucmd(struct hns_roce_cq *hr_cq, struct ib_udata *udata,
		       struct hns_roce_ib_create_cq *ucmd)
{
	struct ib_device *ibdev = hr_cq->ib_cq.device;
	int ret;

	ret = ib_copy_from_udata(ucmd, udata, min(udata->inlen, sizeof(*ucmd)));
	if (ret) {
		ibdev_err(ibdev, "failed to copy CQ udata, ret = %d.\n", ret);
		return ret;
	}

	return 0;
}

static void set_cq_param(struct hns_roce_cq *hr_cq, u32 cq_entries, int vector,
			 struct hns_roce_ib_create_cq *ucmd)
{
	struct hns_roce_dev *hr_dev = to_hr_dev(hr_cq->ib_cq.device);

	cq_entries = max(cq_entries, hr_dev->caps.min_cqes);
	cq_entries = roundup_pow_of_two(cq_entries);
	hr_cq->ib_cq.cqe = cq_entries - 1; /* used as cqe index */
	hr_cq->cq_depth = cq_entries;
	hr_cq->vector = vector;

	spin_lock_init(&hr_cq->lock);
	INIT_LIST_HEAD(&hr_cq->sq_list);
	INIT_LIST_HEAD(&hr_cq->rq_list);
}

static void set_cqe_size(struct hns_roce_cq *hr_cq, struct ib_udata *udata,
			 struct hns_roce_ib_create_cq *ucmd)
{
	struct hns_roce_dev *hr_dev = to_hr_dev(hr_cq->ib_cq.device);

	if (udata) {
		if (udata->inlen >= offsetofend(typeof(*ucmd), cqe_size))
			hr_cq->cqe_size = ucmd->cqe_size;
		else
			hr_cq->cqe_size = HNS_ROCE_V2_CQE_SIZE;
	} else {
		hr_cq->cqe_size = hr_dev->caps.cqe_sz;
	}
}

int hns_roce_create_cq(struct ib_cq *ib_cq, const struct ib_cq_init_attr *attr,
		       struct ib_udata *udata)
{
	struct hns_roce_dev *hr_dev = to_hr_dev(ib_cq->device);
	struct hns_roce_ib_create_cq_resp resp = {};
	struct hns_roce_cq *hr_cq = to_hr_cq(ib_cq);
	struct ib_device *ibdev = &hr_dev->ib_dev;
	struct hns_roce_ib_create_cq ucmd = {};
	int ret;

	if (attr->flags)
		return -EOPNOTSUPP;

	ret = verify_cq_create_attr(hr_dev, attr);
	if (ret)
		return ret;

	if (udata) {
		ret = get_cq_ucmd(hr_cq, udata, &ucmd);
		if (ret)
			return ret;
	}

	set_cq_param(hr_cq, attr->cqe, attr->comp_vector, &ucmd);

	set_cqe_size(hr_cq, udata, &ucmd);

	ret = alloc_cq_buf(hr_dev, hr_cq, udata, ucmd.buf_addr);
	if (ret) {
		ibdev_err(ibdev, "failed to alloc CQ buf, ret = %d.\n", ret);
		return ret;
	}

	ret = alloc_cq_db(hr_dev, hr_cq, udata, ucmd.db_addr, &resp);
	if (ret) {
		ibdev_err(ibdev, "failed to alloc CQ db, ret = %d.\n", ret);
		goto err_cq_buf;
	}

	ret = alloc_cqn(hr_dev, hr_cq);
	if (ret) {
		ibdev_err(ibdev, "failed to alloc CQN, ret = %d.\n", ret);
		goto err_cq_db;
	}

	ret = alloc_cqc(hr_dev, hr_cq);
	if (ret) {
		ibdev_err(ibdev,
			  "failed to alloc CQ context, ret = %d.\n", ret);
		goto err_cqn;
	}

	/*
	 * For the QP created by kernel space, tptr value should be initialized
	 * to zero; For the QP created by user space, it will cause synchronous
	 * problems if tptr is set to zero here, so we initialize it in user
	 * space.
	 */
	if (!udata && hr_cq->tptr_addr)
		*hr_cq->tptr_addr = 0;

	if (udata) {
		resp.cqn = hr_cq->cqn;
		ret = ib_copy_to_udata(udata, &resp,
				       min(udata->outlen, sizeof(resp)));
		if (ret)
			goto err_cqc;
	}

	return 0;

err_cqc:
	free_cqc(hr_dev, hr_cq);
err_cqn:
	free_cqn(hr_dev, hr_cq->cqn);
err_cq_db:
	free_cq_db(hr_dev, hr_cq, udata);
err_cq_buf:
	free_cq_buf(hr_dev, hr_cq);
	return ret;
}

int hns_roce_destroy_cq(struct ib_cq *ib_cq, struct ib_udata *udata)
{
	struct hns_roce_dev *hr_dev = to_hr_dev(ib_cq->device);
	struct hns_roce_cq *hr_cq = to_hr_cq(ib_cq);

	if (hr_dev->hw->destroy_cq)
		hr_dev->hw->destroy_cq(ib_cq, udata);

	free_cqc(hr_dev, hr_cq);
	free_cqn(hr_dev, hr_cq->cqn);
	free_cq_db(hr_dev, hr_cq, udata);
	free_cq_buf(hr_dev, hr_cq);

	return 0;
}

void hns_roce_cq_completion(struct hns_roce_dev *hr_dev, u32 cqn)
{
	struct hns_roce_cq *hr_cq;
	struct ib_cq *ibcq;

	hr_cq = xa_load(&hr_dev->cq_table.array,
			cqn & (hr_dev->caps.num_cqs - 1));
	if (!hr_cq) {
		dev_warn(hr_dev->dev, "Completion event for bogus CQ 0x%06x\n",
			 cqn);
		return;
	}

	++hr_cq->arm_sn;
	ibcq = &hr_cq->ib_cq;
	if (ibcq->comp_handler)
		ibcq->comp_handler(ibcq, ibcq->cq_context);
}

void hns_roce_cq_event(struct hns_roce_dev *hr_dev, u32 cqn, int event_type)
{
	struct device *dev = hr_dev->dev;
	struct hns_roce_cq *hr_cq;
	struct ib_event event;
	struct ib_cq *ibcq;

	hr_cq = xa_load(&hr_dev->cq_table.array,
			cqn & (hr_dev->caps.num_cqs - 1));
	if (!hr_cq) {
		dev_warn(dev, "Async event for bogus CQ 0x%06x\n", cqn);
		return;
	}

	if (event_type != HNS_ROCE_EVENT_TYPE_CQ_ID_INVALID &&
	    event_type != HNS_ROCE_EVENT_TYPE_CQ_ACCESS_ERROR &&
	    event_type != HNS_ROCE_EVENT_TYPE_CQ_OVERFLOW) {
		dev_err(dev, "Unexpected event type 0x%x on CQ 0x%06x\n",
			event_type, cqn);
		return;
	}

	atomic_inc(&hr_cq->refcount);

	ibcq = &hr_cq->ib_cq;
	if (ibcq->event_handler) {
		event.device = ibcq->device;
		event.element.cq = ibcq;
		event.event = IB_EVENT_CQ_ERR;
		ibcq->event_handler(&event, ibcq->cq_context);
	}

	if (atomic_dec_and_test(&hr_cq->refcount))
		complete(&hr_cq->free);
}

void hns_roce_init_cq_table(struct hns_roce_dev *hr_dev)
{
	struct hns_roce_cq_table *cq_table = &hr_dev->cq_table;
	unsigned int reserved_from_bot;
	unsigned int i;

	mutex_init(&cq_table->bank_mutex);
	xa_init(&cq_table->array);

	reserved_from_bot = hr_dev->caps.reserved_cqs;

	for (i = 0; i < reserved_from_bot; i++) {
		cq_table->bank[get_cq_bankid(i)].inuse++;
		cq_table->bank[get_cq_bankid(i)].min++;
	}

	for (i = 0; i < HNS_ROCE_CQ_BANK_NUM; i++) {
		ida_init(&cq_table->bank[i].ida);
		cq_table->bank[i].max = hr_dev->caps.num_cqs /
					HNS_ROCE_CQ_BANK_NUM - 1;
	}
}

void hns_roce_cleanup_cq_table(struct hns_roce_dev *hr_dev)
{
	int i;

	for (i = 0; i < HNS_ROCE_CQ_BANK_NUM; i++)
		ida_destroy(&hr_dev->cq_table.bank[i].ida);
}
