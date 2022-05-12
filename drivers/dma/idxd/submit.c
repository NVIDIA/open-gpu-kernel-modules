// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2019 Intel Corporation. All rights rsvd. */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <uapi/linux/idxd.h>
#include "idxd.h"
#include "registers.h"

static struct idxd_desc *__get_desc(struct idxd_wq *wq, int idx, int cpu)
{
	struct idxd_desc *desc;
	struct idxd_device *idxd = wq->idxd;

	desc = wq->descs[idx];
	memset(desc->hw, 0, sizeof(struct dsa_hw_desc));
	memset(desc->completion, 0, idxd->data->compl_size);
	desc->cpu = cpu;

	if (device_pasid_enabled(idxd))
		desc->hw->pasid = idxd->pasid;

	/*
	 * Descriptor completion vectors are 1...N for MSIX. We will round
	 * robin through the N vectors.
	 */
	wq->vec_ptr = (wq->vec_ptr % idxd->num_wq_irqs) + 1;
	if (!idxd->int_handles) {
		desc->hw->int_handle = wq->vec_ptr;
	} else {
		desc->vector = wq->vec_ptr;
		/*
		 * int_handles are only for descriptor completion. However for device
		 * MSIX enumeration, vec 0 is used for misc interrupts. Therefore even
		 * though we are rotating through 1...N for descriptor interrupts, we
		 * need to acqurie the int_handles from 0..N-1.
		 */
		desc->hw->int_handle = idxd->int_handles[desc->vector - 1];
	}

	return desc;
}

struct idxd_desc *idxd_alloc_desc(struct idxd_wq *wq, enum idxd_op_type optype)
{
	int cpu, idx;
	struct idxd_device *idxd = wq->idxd;
	DEFINE_SBQ_WAIT(wait);
	struct sbq_wait_state *ws;
	struct sbitmap_queue *sbq;

	if (idxd->state != IDXD_DEV_ENABLED)
		return ERR_PTR(-EIO);

	sbq = &wq->sbq;
	idx = sbitmap_queue_get(sbq, &cpu);
	if (idx < 0) {
		if (optype == IDXD_OP_NONBLOCK)
			return ERR_PTR(-EAGAIN);
	} else {
		return __get_desc(wq, idx, cpu);
	}

	ws = &sbq->ws[0];
	for (;;) {
		sbitmap_prepare_to_wait(sbq, ws, &wait, TASK_INTERRUPTIBLE);
		if (signal_pending_state(TASK_INTERRUPTIBLE, current))
			break;
		idx = sbitmap_queue_get(sbq, &cpu);
		if (idx > 0)
			break;
		schedule();
	}

	sbitmap_finish_wait(sbq, ws, &wait);
	if (idx < 0)
		return ERR_PTR(-EAGAIN);

	return __get_desc(wq, idx, cpu);
}

void idxd_free_desc(struct idxd_wq *wq, struct idxd_desc *desc)
{
	int cpu = desc->cpu;

	desc->cpu = -1;
	sbitmap_queue_clear(&wq->sbq, desc->id, cpu);
}

int idxd_submit_desc(struct idxd_wq *wq, struct idxd_desc *desc)
{
	struct idxd_device *idxd = wq->idxd;
	void __iomem *portal;
	int rc;

	if (idxd->state != IDXD_DEV_ENABLED)
		return -EIO;

	if (!percpu_ref_tryget_live(&wq->wq_active))
		return -ENXIO;

	portal = wq->portal;

	/*
	 * The wmb() flushes writes to coherent DMA data before
	 * possibly triggering a DMA read. The wmb() is necessary
	 * even on UP because the recipient is a device.
	 */
	wmb();
	if (wq_dedicated(wq)) {
		iosubmit_cmds512(portal, desc->hw, 1);
	} else {
		/*
		 * It's not likely that we would receive queue full rejection
		 * since the descriptor allocation gates at wq size. If we
		 * receive a -EAGAIN, that means something went wrong such as the
		 * device is not accepting descriptor at all.
		 */
		rc = enqcmds(portal, desc->hw);
		if (rc < 0)
			return rc;
	}

	percpu_ref_put(&wq->wq_active);

	/*
	 * Pending the descriptor to the lockless list for the irq_entry
	 * that we designated the descriptor to.
	 */
	if (desc->hw->flags & IDXD_OP_FLAG_RCI) {
		int vec;

		/*
		 * If the driver is on host kernel, it would be the value
		 * assigned to interrupt handle, which is index for MSIX
		 * vector. If it's guest then can't use the int_handle since
		 * that is the index to IMS for the entire device. The guest
		 * device local index will be used.
		 */
		vec = !idxd->int_handles ? desc->hw->int_handle : desc->vector;
		llist_add(&desc->llnode, &idxd->irq_entries[vec].pending_llist);
	}

	return 0;
}
