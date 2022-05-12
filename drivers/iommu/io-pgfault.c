// SPDX-License-Identifier: GPL-2.0
/*
 * Handle device page faults
 *
 * Copyright (C) 2020 ARM Ltd.
 */

#include <linux/iommu.h>
#include <linux/list.h>
#include <linux/sched/mm.h>
#include <linux/slab.h>
#include <linux/workqueue.h>

#include "iommu-sva-lib.h"

/**
 * struct iopf_queue - IO Page Fault queue
 * @wq: the fault workqueue
 * @devices: devices attached to this queue
 * @lock: protects the device list
 */
struct iopf_queue {
	struct workqueue_struct		*wq;
	struct list_head		devices;
	struct mutex			lock;
};

/**
 * struct iopf_device_param - IO Page Fault data attached to a device
 * @dev: the device that owns this param
 * @queue: IOPF queue
 * @queue_list: index into queue->devices
 * @partial: faults that are part of a Page Request Group for which the last
 *           request hasn't been submitted yet.
 */
struct iopf_device_param {
	struct device			*dev;
	struct iopf_queue		*queue;
	struct list_head		queue_list;
	struct list_head		partial;
};

struct iopf_fault {
	struct iommu_fault		fault;
	struct list_head		list;
};

struct iopf_group {
	struct iopf_fault		last_fault;
	struct list_head		faults;
	struct work_struct		work;
	struct device			*dev;
};

static int iopf_complete_group(struct device *dev, struct iopf_fault *iopf,
			       enum iommu_page_response_code status)
{
	struct iommu_page_response resp = {
		.version		= IOMMU_PAGE_RESP_VERSION_1,
		.pasid			= iopf->fault.prm.pasid,
		.grpid			= iopf->fault.prm.grpid,
		.code			= status,
	};

	if ((iopf->fault.prm.flags & IOMMU_FAULT_PAGE_REQUEST_PASID_VALID) &&
	    (iopf->fault.prm.flags & IOMMU_FAULT_PAGE_RESPONSE_NEEDS_PASID))
		resp.flags = IOMMU_PAGE_RESP_PASID_VALID;

	return iommu_page_response(dev, &resp);
}

static enum iommu_page_response_code
iopf_handle_single(struct iopf_fault *iopf)
{
	vm_fault_t ret;
	struct mm_struct *mm;
	struct vm_area_struct *vma;
	unsigned int access_flags = 0;
	unsigned int fault_flags = FAULT_FLAG_REMOTE;
	struct iommu_fault_page_request *prm = &iopf->fault.prm;
	enum iommu_page_response_code status = IOMMU_PAGE_RESP_INVALID;

	if (!(prm->flags & IOMMU_FAULT_PAGE_REQUEST_PASID_VALID))
		return status;

	mm = iommu_sva_find(prm->pasid);
	if (IS_ERR_OR_NULL(mm))
		return status;

	mmap_read_lock(mm);

	vma = find_extend_vma(mm, prm->addr);
	if (!vma)
		/* Unmapped area */
		goto out_put_mm;

	if (prm->perm & IOMMU_FAULT_PERM_READ)
		access_flags |= VM_READ;

	if (prm->perm & IOMMU_FAULT_PERM_WRITE) {
		access_flags |= VM_WRITE;
		fault_flags |= FAULT_FLAG_WRITE;
	}

	if (prm->perm & IOMMU_FAULT_PERM_EXEC) {
		access_flags |= VM_EXEC;
		fault_flags |= FAULT_FLAG_INSTRUCTION;
	}

	if (!(prm->perm & IOMMU_FAULT_PERM_PRIV))
		fault_flags |= FAULT_FLAG_USER;

	if (access_flags & ~vma->vm_flags)
		/* Access fault */
		goto out_put_mm;

	ret = handle_mm_fault(vma, prm->addr, fault_flags, NULL);
	status = ret & VM_FAULT_ERROR ? IOMMU_PAGE_RESP_INVALID :
		IOMMU_PAGE_RESP_SUCCESS;

out_put_mm:
	mmap_read_unlock(mm);
	mmput(mm);

	return status;
}

static void iopf_handle_group(struct work_struct *work)
{
	struct iopf_group *group;
	struct iopf_fault *iopf, *next;
	enum iommu_page_response_code status = IOMMU_PAGE_RESP_SUCCESS;

	group = container_of(work, struct iopf_group, work);

	list_for_each_entry_safe(iopf, next, &group->faults, list) {
		/*
		 * For the moment, errors are sticky: don't handle subsequent
		 * faults in the group if there is an error.
		 */
		if (status == IOMMU_PAGE_RESP_SUCCESS)
			status = iopf_handle_single(iopf);

		if (!(iopf->fault.prm.flags &
		      IOMMU_FAULT_PAGE_REQUEST_LAST_PAGE))
			kfree(iopf);
	}

	iopf_complete_group(group->dev, &group->last_fault, status);
	kfree(group);
}

/**
 * iommu_queue_iopf - IO Page Fault handler
 * @fault: fault event
 * @cookie: struct device, passed to iommu_register_device_fault_handler.
 *
 * Add a fault to the device workqueue, to be handled by mm.
 *
 * This module doesn't handle PCI PASID Stop Marker; IOMMU drivers must discard
 * them before reporting faults. A PASID Stop Marker (LRW = 0b100) doesn't
 * expect a response. It may be generated when disabling a PASID (issuing a
 * PASID stop request) by some PCI devices.
 *
 * The PASID stop request is issued by the device driver before unbind(). Once
 * it completes, no page request is generated for this PASID anymore and
 * outstanding ones have been pushed to the IOMMU (as per PCIe 4.0r1.0 - 6.20.1
 * and 10.4.1.2 - Managing PASID TLP Prefix Usage). Some PCI devices will wait
 * for all outstanding page requests to come back with a response before
 * completing the PASID stop request. Others do not wait for page responses, and
 * instead issue this Stop Marker that tells us when the PASID can be
 * reallocated.
 *
 * It is safe to discard the Stop Marker because it is an optimization.
 * a. Page requests, which are posted requests, have been flushed to the IOMMU
 *    when the stop request completes.
 * b. The IOMMU driver flushes all fault queues on unbind() before freeing the
 *    PASID.
 *
 * So even though the Stop Marker might be issued by the device *after* the stop
 * request completes, outstanding faults will have been dealt with by the time
 * the PASID is freed.
 *
 * Return: 0 on success and <0 on error.
 */
int iommu_queue_iopf(struct iommu_fault *fault, void *cookie)
{
	int ret;
	struct iopf_group *group;
	struct iopf_fault *iopf, *next;
	struct iopf_device_param *iopf_param;

	struct device *dev = cookie;
	struct dev_iommu *param = dev->iommu;

	lockdep_assert_held(&param->lock);

	if (fault->type != IOMMU_FAULT_PAGE_REQ)
		/* Not a recoverable page fault */
		return -EOPNOTSUPP;

	/*
	 * As long as we're holding param->lock, the queue can't be unlinked
	 * from the device and therefore cannot disappear.
	 */
	iopf_param = param->iopf_param;
	if (!iopf_param)
		return -ENODEV;

	if (!(fault->prm.flags & IOMMU_FAULT_PAGE_REQUEST_LAST_PAGE)) {
		iopf = kzalloc(sizeof(*iopf), GFP_KERNEL);
		if (!iopf)
			return -ENOMEM;

		iopf->fault = *fault;

		/* Non-last request of a group. Postpone until the last one */
		list_add(&iopf->list, &iopf_param->partial);

		return 0;
	}

	group = kzalloc(sizeof(*group), GFP_KERNEL);
	if (!group) {
		/*
		 * The caller will send a response to the hardware. But we do
		 * need to clean up before leaving, otherwise partial faults
		 * will be stuck.
		 */
		ret = -ENOMEM;
		goto cleanup_partial;
	}

	group->dev = dev;
	group->last_fault.fault = *fault;
	INIT_LIST_HEAD(&group->faults);
	list_add(&group->last_fault.list, &group->faults);
	INIT_WORK(&group->work, iopf_handle_group);

	/* See if we have partial faults for this group */
	list_for_each_entry_safe(iopf, next, &iopf_param->partial, list) {
		if (iopf->fault.prm.grpid == fault->prm.grpid)
			/* Insert *before* the last fault */
			list_move(&iopf->list, &group->faults);
	}

	queue_work(iopf_param->queue->wq, &group->work);
	return 0;

cleanup_partial:
	list_for_each_entry_safe(iopf, next, &iopf_param->partial, list) {
		if (iopf->fault.prm.grpid == fault->prm.grpid) {
			list_del(&iopf->list);
			kfree(iopf);
		}
	}
	return ret;
}
EXPORT_SYMBOL_GPL(iommu_queue_iopf);

/**
 * iopf_queue_flush_dev - Ensure that all queued faults have been processed
 * @dev: the endpoint whose faults need to be flushed.
 *
 * The IOMMU driver calls this before releasing a PASID, to ensure that all
 * pending faults for this PASID have been handled, and won't hit the address
 * space of the next process that uses this PASID. The driver must make sure
 * that no new fault is added to the queue. In particular it must flush its
 * low-level queue before calling this function.
 *
 * Return: 0 on success and <0 on error.
 */
int iopf_queue_flush_dev(struct device *dev)
{
	int ret = 0;
	struct iopf_device_param *iopf_param;
	struct dev_iommu *param = dev->iommu;

	if (!param)
		return -ENODEV;

	mutex_lock(&param->lock);
	iopf_param = param->iopf_param;
	if (iopf_param)
		flush_workqueue(iopf_param->queue->wq);
	else
		ret = -ENODEV;
	mutex_unlock(&param->lock);

	return ret;
}
EXPORT_SYMBOL_GPL(iopf_queue_flush_dev);

/**
 * iopf_queue_discard_partial - Remove all pending partial fault
 * @queue: the queue whose partial faults need to be discarded
 *
 * When the hardware queue overflows, last page faults in a group may have been
 * lost and the IOMMU driver calls this to discard all partial faults. The
 * driver shouldn't be adding new faults to this queue concurrently.
 *
 * Return: 0 on success and <0 on error.
 */
int iopf_queue_discard_partial(struct iopf_queue *queue)
{
	struct iopf_fault *iopf, *next;
	struct iopf_device_param *iopf_param;

	if (!queue)
		return -EINVAL;

	mutex_lock(&queue->lock);
	list_for_each_entry(iopf_param, &queue->devices, queue_list) {
		list_for_each_entry_safe(iopf, next, &iopf_param->partial,
					 list) {
			list_del(&iopf->list);
			kfree(iopf);
		}
	}
	mutex_unlock(&queue->lock);
	return 0;
}
EXPORT_SYMBOL_GPL(iopf_queue_discard_partial);

/**
 * iopf_queue_add_device - Add producer to the fault queue
 * @queue: IOPF queue
 * @dev: device to add
 *
 * Return: 0 on success and <0 on error.
 */
int iopf_queue_add_device(struct iopf_queue *queue, struct device *dev)
{
	int ret = -EBUSY;
	struct iopf_device_param *iopf_param;
	struct dev_iommu *param = dev->iommu;

	if (!param)
		return -ENODEV;

	iopf_param = kzalloc(sizeof(*iopf_param), GFP_KERNEL);
	if (!iopf_param)
		return -ENOMEM;

	INIT_LIST_HEAD(&iopf_param->partial);
	iopf_param->queue = queue;
	iopf_param->dev = dev;

	mutex_lock(&queue->lock);
	mutex_lock(&param->lock);
	if (!param->iopf_param) {
		list_add(&iopf_param->queue_list, &queue->devices);
		param->iopf_param = iopf_param;
		ret = 0;
	}
	mutex_unlock(&param->lock);
	mutex_unlock(&queue->lock);

	if (ret)
		kfree(iopf_param);

	return ret;
}
EXPORT_SYMBOL_GPL(iopf_queue_add_device);

/**
 * iopf_queue_remove_device - Remove producer from fault queue
 * @queue: IOPF queue
 * @dev: device to remove
 *
 * Caller makes sure that no more faults are reported for this device.
 *
 * Return: 0 on success and <0 on error.
 */
int iopf_queue_remove_device(struct iopf_queue *queue, struct device *dev)
{
	int ret = -EINVAL;
	struct iopf_fault *iopf, *next;
	struct iopf_device_param *iopf_param;
	struct dev_iommu *param = dev->iommu;

	if (!param || !queue)
		return -EINVAL;

	mutex_lock(&queue->lock);
	mutex_lock(&param->lock);
	iopf_param = param->iopf_param;
	if (iopf_param && iopf_param->queue == queue) {
		list_del(&iopf_param->queue_list);
		param->iopf_param = NULL;
		ret = 0;
	}
	mutex_unlock(&param->lock);
	mutex_unlock(&queue->lock);
	if (ret)
		return ret;

	/* Just in case some faults are still stuck */
	list_for_each_entry_safe(iopf, next, &iopf_param->partial, list)
		kfree(iopf);

	kfree(iopf_param);

	return 0;
}
EXPORT_SYMBOL_GPL(iopf_queue_remove_device);

/**
 * iopf_queue_alloc - Allocate and initialize a fault queue
 * @name: a unique string identifying the queue (for workqueue)
 *
 * Return: the queue on success and NULL on error.
 */
struct iopf_queue *iopf_queue_alloc(const char *name)
{
	struct iopf_queue *queue;

	queue = kzalloc(sizeof(*queue), GFP_KERNEL);
	if (!queue)
		return NULL;

	/*
	 * The WQ is unordered because the low-level handler enqueues faults by
	 * group. PRI requests within a group have to be ordered, but once
	 * that's dealt with, the high-level function can handle groups out of
	 * order.
	 */
	queue->wq = alloc_workqueue("iopf_queue/%s", WQ_UNBOUND, 0, name);
	if (!queue->wq) {
		kfree(queue);
		return NULL;
	}

	INIT_LIST_HEAD(&queue->devices);
	mutex_init(&queue->lock);

	return queue;
}
EXPORT_SYMBOL_GPL(iopf_queue_alloc);

/**
 * iopf_queue_free - Free IOPF queue
 * @queue: queue to free
 *
 * Counterpart to iopf_queue_alloc(). The driver must not be queuing faults or
 * adding/removing devices on this queue anymore.
 */
void iopf_queue_free(struct iopf_queue *queue)
{
	struct iopf_device_param *iopf_param, *next;

	if (!queue)
		return;

	list_for_each_entry_safe(iopf_param, next, &queue->devices, queue_list)
		iopf_queue_remove_device(queue, iopf_param->dev);

	destroy_workqueue(queue->wq);
	kfree(queue);
}
EXPORT_SYMBOL_GPL(iopf_queue_free);
