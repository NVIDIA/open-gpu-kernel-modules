// SPDX-License-Identifier: GPL-2.0 OR MIT
/*
 * Copyright 2014-2022 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <linux/mutex.h>
#include <linux/log2.h>
#include <linux/sched.h>
#include <linux/sched/mm.h>
#include <linux/sched/task.h>
#include <linux/mmu_context.h>
#include <linux/slab.h>
#include <linux/amd-iommu.h>
#include <linux/notifier.h>
#include <linux/compat.h>
#include <linux/mman.h>
#include <linux/file.h>
#include <linux/pm_runtime.h>
#include "amdgpu_amdkfd.h"
#include "amdgpu.h"

struct mm_struct;

#include "kfd_priv.h"
#include "kfd_device_queue_manager.h"
#include "kfd_iommu.h"
#include "kfd_svm.h"
#include "kfd_trace.h"
#include "kfd_debug.h"

/*
 * List of struct kfd_process (field kfd_process).
 * Unique/indexed by mm_struct*
 */
DEFINE_HASHTABLE(kfd_processes_table, KFD_PROCESS_TABLE_SIZE);
static DEFINE_MUTEX(kfd_processes_mutex);

#ifndef DEFINE_SRCU
struct srcu_struct kfd_processes_srcu;
void kfd_init_processes_srcu(void)
{
	init_srcu_struct(&kfd_processes_srcu);
}

void kfd_cleanup_processes_srcu(void)
{
	cleanup_srcu_struct(&kfd_processes_srcu);
}
#else
DEFINE_SRCU(kfd_processes_srcu);
#endif

/* For process termination handling */
static struct workqueue_struct *kfd_process_wq;

/* Ordered, single-threaded workqueue for restoring evicted
 * processes. Restoring multiple processes concurrently under memory
 * pressure can lead to processes blocking each other from validating
 * their BOs and result in a live-lock situation where processes
 * remain evicted indefinitely.
 */
static struct workqueue_struct *kfd_restore_wq;

static struct kfd_process *find_process(const struct task_struct *thread,
		bool ref);
static void kfd_process_ref_release(struct kref *ref);
static struct kfd_process *create_process(const struct task_struct *thread);

static void evict_process_worker(struct work_struct *work);
static void restore_process_worker(struct work_struct *work);

static void kfd_process_device_destroy_cwsr_dgpu(struct kfd_process_device *pdd);
static void kfd_sysfs_create_file(struct kobject *kobj, struct attribute *attr,
				char *name);

struct kfd_procfs_tree {
	struct kobject *kobj;
};

static struct kfd_procfs_tree procfs;

/*
 * Structure for SDMA activity tracking
 */
struct kfd_sdma_activity_handler_workarea {
	struct work_struct sdma_activity_work;
	struct kfd_process_device *pdd;
	uint64_t sdma_activity_counter;
};

struct temp_sdma_queue_list {
	uint64_t __user *rptr;
	uint64_t sdma_val;
	unsigned int queue_id;
	struct list_head list;
};

static void kfd_sdma_activity_worker(struct work_struct *work)
{
	struct kfd_sdma_activity_handler_workarea *workarea;
	struct kfd_process_device *pdd;
	uint64_t val;
	struct mm_struct *mm;
	struct queue *q;
	struct qcm_process_device *qpd;
	struct device_queue_manager *dqm;
	int ret = 0;
	struct temp_sdma_queue_list sdma_q_list;
	struct temp_sdma_queue_list *sdma_q, *next;

	workarea = container_of(work, struct kfd_sdma_activity_handler_workarea,
				sdma_activity_work);

	pdd = workarea->pdd;
	if (!pdd)
		return;
	dqm = pdd->dev->dqm;
	qpd = &pdd->qpd;
	if (!dqm || !qpd)
		return;
	/*
	 * Total SDMA activity is current SDMA activity + past SDMA activity
	 * Past SDMA count is stored in pdd.
	 * To get the current activity counters for all active SDMA queues,
	 * we loop over all SDMA queues and get their counts from user-space.
	 *
	 * We cannot call get_user() with dqm_lock held as it can cause
	 * a circular lock dependency situation. To read the SDMA stats,
	 * we need to do the following:
	 *
	 * 1. Create a temporary list of SDMA queue nodes from the qpd->queues_list,
	 *    with dqm_lock/dqm_unlock().
	 * 2. Call get_user() for each node in temporary list without dqm_lock.
	 *    Save the SDMA count for each node and also add the count to the total
	 *    SDMA count counter.
	 *    Its possible, during this step, a few SDMA queue nodes got deleted
	 *    from the qpd->queues_list.
	 * 3. Do a second pass over qpd->queues_list to check if any nodes got deleted.
	 *    If any node got deleted, its SDMA count would be captured in the sdma
	 *    past activity counter. So subtract the SDMA counter stored in step 2
	 *    for this node from the total SDMA count.
	 */
	INIT_LIST_HEAD(&sdma_q_list.list);

	/*
	 * Create the temp list of all SDMA queues
	 */
	dqm_lock(dqm);

	list_for_each_entry(q, &qpd->queues_list, list) {
		if ((q->properties.type != KFD_QUEUE_TYPE_SDMA) &&
		    (q->properties.type != KFD_QUEUE_TYPE_SDMA_XGMI))
			continue;

		sdma_q = kzalloc(sizeof(struct temp_sdma_queue_list), GFP_KERNEL);
		if (!sdma_q) {
			dqm_unlock(dqm);
			goto cleanup;
		}

		INIT_LIST_HEAD(&sdma_q->list);
		sdma_q->rptr = (uint64_t __user *)q->properties.read_ptr;
		sdma_q->queue_id = q->properties.queue_id;
		list_add_tail(&sdma_q->list, &sdma_q_list.list);
	}

	/*
	 * If the temp list is empty, then no SDMA queues nodes were found in
	 * qpd->queues_list. Return the past activity count as the total sdma
	 * count
	 */
	if (list_empty(&sdma_q_list.list)) {
		workarea->sdma_activity_counter = pdd->sdma_past_activity_counter;
		dqm_unlock(dqm);
		return;
	}

	dqm_unlock(dqm);

	/*
	 * Get the usage count for each SDMA queue in temp_list.
	 */
	mm = get_task_mm(pdd->process->lead_thread);
	if (!mm)
		goto cleanup;

	kthread_use_mm(mm);

	list_for_each_entry(sdma_q, &sdma_q_list.list, list) {
		val = 0;
		ret = read_sdma_queue_counter(sdma_q->rptr, &val);
		if (ret) {
			pr_debug("Failed to read SDMA queue active counter for queue id: %d",
				 sdma_q->queue_id);
		} else {
			sdma_q->sdma_val = val;
			workarea->sdma_activity_counter += val;
		}
	}

	kthread_unuse_mm(mm);
	mmput(mm);

	/*
	 * Do a second iteration over qpd_queues_list to check if any SDMA
	 * nodes got deleted while fetching SDMA counter.
	 */
	dqm_lock(dqm);

	workarea->sdma_activity_counter += pdd->sdma_past_activity_counter;

	list_for_each_entry(q, &qpd->queues_list, list) {
		if (list_empty(&sdma_q_list.list))
			break;

		if ((q->properties.type != KFD_QUEUE_TYPE_SDMA) &&
		    (q->properties.type != KFD_QUEUE_TYPE_SDMA_XGMI))
			continue;

		list_for_each_entry_safe(sdma_q, next, &sdma_q_list.list, list) {
			if (((uint64_t __user *)q->properties.read_ptr == sdma_q->rptr) &&
			     (sdma_q->queue_id == q->properties.queue_id)) {
				list_del(&sdma_q->list);
				kfree(sdma_q);
				break;
			}
		}
	}

	dqm_unlock(dqm);

	/*
	 * If temp list is not empty, it implies some queues got deleted
	 * from qpd->queues_list during SDMA usage read. Subtract the SDMA
	 * count for each node from the total SDMA count.
	 */
	list_for_each_entry_safe(sdma_q, next, &sdma_q_list.list, list) {
		workarea->sdma_activity_counter -= sdma_q->sdma_val;
		list_del(&sdma_q->list);
		kfree(sdma_q);
	}

	return;

cleanup:
	list_for_each_entry_safe(sdma_q, next, &sdma_q_list.list, list) {
		list_del(&sdma_q->list);
		kfree(sdma_q);
	}
}

/**
 * kfd_get_cu_occupancy - Collect number of waves in-flight on this device
 * by current process. Translates acquired wave count into number of compute units
 * that are occupied.
 *
 * @attr: Handle of attribute that allows reporting of wave count. The attribute
 * handle encapsulates GPU device it is associated with, thereby allowing collection
 * of waves in flight, etc
 * @buffer: Handle of user provided buffer updated with wave count
 *
 * Return: Number of bytes written to user buffer or an error value
 */
static int kfd_get_cu_occupancy(struct attribute *attr, char *buffer)
{
	int cu_cnt;
	int wave_cnt;
	int max_waves_per_cu;
	struct kfd_dev *dev = NULL;
	struct kfd_process *proc = NULL;
	struct kfd_process_device *pdd = NULL;

	pdd = container_of(attr, struct kfd_process_device, attr_cu_occupancy);
	dev = pdd->dev;
	if (dev->kfd2kgd->get_cu_occupancy == NULL)
		return -EINVAL;

	cu_cnt = 0;
	proc = pdd->process;
	if (pdd->qpd.queue_count == 0) {
		pr_debug("Gpu-Id: %d has no active queues for process %d\n",
			 dev->id, proc->pasid);
		return snprintf(buffer, PAGE_SIZE, "%d\n", cu_cnt);
	}

	/* Collect wave count from device if it supports */
	wave_cnt = 0;
	max_waves_per_cu = 0;
	dev->kfd2kgd->get_cu_occupancy(dev->adev, proc->pasid, &wave_cnt,
			&max_waves_per_cu);

	/* Translate wave count to number of compute units */
	cu_cnt = (wave_cnt + (max_waves_per_cu - 1)) / max_waves_per_cu;
	return snprintf(buffer, PAGE_SIZE, "%d\n", cu_cnt);
}

static ssize_t kfd_procfs_show(struct kobject *kobj, struct attribute *attr,
			       char *buffer)
{
	if (strcmp(attr->name, "pasid") == 0) {
		struct kfd_process *p = container_of(attr, struct kfd_process,
						     attr_pasid);

		return snprintf(buffer, PAGE_SIZE, "%d\n", p->pasid);
	} else if (strncmp(attr->name, "vram_", 5) == 0) {
		struct kfd_process_device *pdd = container_of(attr, struct kfd_process_device,
							      attr_vram);
		return snprintf(buffer, PAGE_SIZE, "%llu\n", READ_ONCE(pdd->vram_usage));
	} else if (strncmp(attr->name, "sdma_", 5) == 0) {
		struct kfd_process_device *pdd = container_of(attr, struct kfd_process_device,
							      attr_sdma);
		struct kfd_sdma_activity_handler_workarea sdma_activity_work_handler;

		INIT_WORK(&sdma_activity_work_handler.sdma_activity_work,
					kfd_sdma_activity_worker);

		sdma_activity_work_handler.pdd = pdd;
		sdma_activity_work_handler.sdma_activity_counter = 0;

		schedule_work(&sdma_activity_work_handler.sdma_activity_work);

		flush_work(&sdma_activity_work_handler.sdma_activity_work);

		return snprintf(buffer, PAGE_SIZE, "%llu\n",
				(sdma_activity_work_handler.sdma_activity_counter)/
				 SDMA_ACTIVITY_DIVISOR);
	} else {
		pr_err("Invalid attribute");
		return -EINVAL;
	}

	return 0;
}

static void kfd_procfs_kobj_release(struct kobject *kobj)
{
	kfree(kobj);
}

static const struct sysfs_ops kfd_procfs_ops = {
	.show = kfd_procfs_show,
};

static struct kobj_type procfs_type = {
	.release = kfd_procfs_kobj_release,
	.sysfs_ops = &kfd_procfs_ops,
};

void kfd_procfs_init(void)
{
	int ret = 0;

	procfs.kobj = kfd_alloc_struct(procfs.kobj);
	if (!procfs.kobj)
		return;

	ret = kobject_init_and_add(procfs.kobj, &procfs_type,
				   &kfd_device->kobj, "proc");
	if (ret) {
		pr_warn("Could not create procfs proc folder");
		/* If we fail to create the procfs, clean up */
		kfd_procfs_shutdown();
	}
}

void kfd_procfs_shutdown(void)
{
	if (procfs.kobj) {
		kobject_del(procfs.kobj);
		kobject_put(procfs.kobj);
		procfs.kobj = NULL;
	}
}

static ssize_t kfd_procfs_queue_show(struct kobject *kobj,
				     struct attribute *attr, char *buffer)
{
	struct queue *q = container_of(kobj, struct queue, kobj);

	if (!strcmp(attr->name, "size"))
		return snprintf(buffer, PAGE_SIZE, "%llu",
				q->properties.queue_size);
	else if (!strcmp(attr->name, "type"))
		return snprintf(buffer, PAGE_SIZE, "%d", q->properties.type);
	else if (!strcmp(attr->name, "gpuid"))
		return snprintf(buffer, PAGE_SIZE, "%u", q->device->id);
	else
		pr_err("Invalid attribute");

	return 0;
}

static ssize_t kfd_procfs_stats_show(struct kobject *kobj,
				     struct attribute *attr, char *buffer)
{
	if (strcmp(attr->name, "evicted_ms") == 0) {
		struct kfd_process_device *pdd = container_of(attr,
				struct kfd_process_device,
				attr_evict);
		uint64_t evict_jiffies;

		evict_jiffies = atomic64_read(&pdd->evict_duration_counter);

		return snprintf(buffer,
				PAGE_SIZE,
				"%llu\n",
				jiffies64_to_msecs(evict_jiffies));

	/* Sysfs handle that gets CU occupancy is per device */
	} else if (strcmp(attr->name, "cu_occupancy") == 0) {
		return kfd_get_cu_occupancy(attr, buffer);
	} else {
		pr_err("Invalid attribute");
	}

	return 0;
}

static ssize_t kfd_sysfs_counters_show(struct kobject *kobj,
				       struct attribute *attr, char *buf)
{
	struct kfd_process_device *pdd;

	if (!strcmp(attr->name, "faults")) {
		pdd = container_of(attr, struct kfd_process_device,
				   attr_faults);
		return sysfs_emit(buf, "%llu\n", READ_ONCE(pdd->faults));
	}
	if (!strcmp(attr->name, "page_in")) {
		pdd = container_of(attr, struct kfd_process_device,
				   attr_page_in);
		return sysfs_emit(buf, "%llu\n", READ_ONCE(pdd->page_in));
	}
	if (!strcmp(attr->name, "page_out")) {
		pdd = container_of(attr, struct kfd_process_device,
				   attr_page_out);
		return sysfs_emit(buf, "%llu\n", READ_ONCE(pdd->page_out));
	}
	return 0;
}

static const struct sysfs_ops procfs_queue_ops = {
	.show = kfd_procfs_queue_show,
};

static struct kobj_type procfs_queue_type = {
	.sysfs_ops = &procfs_queue_ops,
};

static const struct sysfs_ops procfs_stats_ops = {
	.show = kfd_procfs_stats_show,
};

static struct kobj_type procfs_stats_type = {
	.sysfs_ops = &procfs_stats_ops,
	.release = kfd_procfs_kobj_release,
};

static const struct sysfs_ops sysfs_counters_ops = {
	.show = kfd_sysfs_counters_show,
};

static struct kobj_type sysfs_counters_type = {
	.sysfs_ops = &sysfs_counters_ops,
	.release = kfd_procfs_kobj_release,
};

int kfd_procfs_add_queue(struct queue *q)
{
	struct kfd_process *proc;
	int ret;

	if (!q || !q->process)
		return -EINVAL;
	proc = q->process;

	/* Create proc/<pid>/queues/<queue id> folder */
	if (!proc->kobj_queues)
		return -EFAULT;
	ret = kobject_init_and_add(&q->kobj, &procfs_queue_type,
			proc->kobj_queues, "%u", q->properties.queue_id);
	if (ret < 0) {
		pr_warn("Creating proc/<pid>/queues/%u failed",
			q->properties.queue_id);
		kobject_put(&q->kobj);
		return ret;
	}

	kfd_sysfs_create_file(&q->kobj, &q->attr_guid, "guid");
	kfd_sysfs_create_file(&q->kobj, &q->attr_size, "size");
	kfd_sysfs_create_file(&q->kobj, &q->attr_type, "type");

	return 0;
}

static void kfd_sysfs_create_file(struct kobject *kobj, struct attribute *attr,
				 char *name)
{
	int ret;

	if (!kobj || !attr || !name)
		return;

	attr->name = name;
	attr->mode = KFD_SYSFS_FILE_MODE;
	sysfs_attr_init(attr);

	ret = sysfs_create_file(kobj, attr);
	if (ret)
		pr_warn("Create sysfs %s/%s failed %d", kobj->name, name, ret);
}

static void kfd_procfs_add_sysfs_stats(struct kfd_process *p)
{
	int ret;
	int i;
	char stats_dir_filename[MAX_SYSFS_FILENAME_LEN];

	if (!p || !p->kobj)
		return;

	/*
	 * Create sysfs files for each GPU:
	 * - proc/<pid>/stats_<gpuid>/
	 * - proc/<pid>/stats_<gpuid>/evicted_ms
	 * - proc/<pid>/stats_<gpuid>/cu_occupancy
	 */
	for (i = 0; i < p->n_pdds; i++) {
		struct kfd_process_device *pdd = p->pdds[i];

		snprintf(stats_dir_filename, MAX_SYSFS_FILENAME_LEN,
				"stats_%u", pdd->dev->id);
		pdd->kobj_stats = kfd_alloc_struct(pdd->kobj_stats);
		if (!pdd->kobj_stats)
			return;

		ret = kobject_init_and_add(pdd->kobj_stats,
					   &procfs_stats_type,
					   p->kobj,
					   stats_dir_filename);

		if (ret) {
			pr_warn("Creating KFD proc/stats_%s folder failed",
				stats_dir_filename);
			kobject_put(pdd->kobj_stats);
			pdd->kobj_stats = NULL;
			return;
		}

		kfd_sysfs_create_file(pdd->kobj_stats, &pdd->attr_evict,
				      "evicted_ms");
		/* Add sysfs file to report compute unit occupancy */
		if (pdd->dev->kfd2kgd->get_cu_occupancy)
			kfd_sysfs_create_file(pdd->kobj_stats,
					      &pdd->attr_cu_occupancy,
					      "cu_occupancy");
	}
}

static void kfd_procfs_add_sysfs_counters(struct kfd_process *p)
{
	int ret = 0;
	int i;
	char counters_dir_filename[MAX_SYSFS_FILENAME_LEN];

	if (!p || !p->kobj)
		return;

	/*
	 * Create sysfs files for each GPU which supports SVM
	 * - proc/<pid>/counters_<gpuid>/
	 * - proc/<pid>/counters_<gpuid>/faults
	 * - proc/<pid>/counters_<gpuid>/page_in
	 * - proc/<pid>/counters_<gpuid>/page_out
	 */
	for_each_set_bit(i, p->svms.bitmap_supported, p->n_pdds) {
		struct kfd_process_device *pdd = p->pdds[i];
		struct kobject *kobj_counters;

		snprintf(counters_dir_filename, MAX_SYSFS_FILENAME_LEN,
			"counters_%u", pdd->dev->id);
		kobj_counters = kfd_alloc_struct(kobj_counters);
		if (!kobj_counters)
			return;

		ret = kobject_init_and_add(kobj_counters, &sysfs_counters_type,
					   p->kobj, counters_dir_filename);
		if (ret) {
			pr_warn("Creating KFD proc/%s folder failed",
				counters_dir_filename);
			kobject_put(kobj_counters);
			return;
		}

		pdd->kobj_counters = kobj_counters;
		kfd_sysfs_create_file(kobj_counters, &pdd->attr_faults,
				      "faults");
		kfd_sysfs_create_file(kobj_counters, &pdd->attr_page_in,
				      "page_in");
		kfd_sysfs_create_file(kobj_counters, &pdd->attr_page_out,
				      "page_out");
	}
}

static void kfd_procfs_add_sysfs_files(struct kfd_process *p)
{
	int i;

	if (!p || !p->kobj)
		return;

	/*
	 * Create sysfs files for each GPU:
	 * - proc/<pid>/vram_<gpuid>
	 * - proc/<pid>/sdma_<gpuid>
	 */
	for (i = 0; i < p->n_pdds; i++) {
		struct kfd_process_device *pdd = p->pdds[i];

		snprintf(pdd->vram_filename, MAX_SYSFS_FILENAME_LEN, "vram_%u",
			 pdd->dev->id);
		kfd_sysfs_create_file(p->kobj, &pdd->attr_vram,
				      pdd->vram_filename);

		snprintf(pdd->sdma_filename, MAX_SYSFS_FILENAME_LEN, "sdma_%u",
			 pdd->dev->id);
		kfd_sysfs_create_file(p->kobj, &pdd->attr_sdma,
					    pdd->sdma_filename);
	}
}

void kfd_procfs_del_queue(struct queue *q)
{
	if (!q)
		return;

	sysfs_remove_file(&q->kobj, &q->attr_guid);
	sysfs_remove_file(&q->kobj, &q->attr_size);
	sysfs_remove_file(&q->kobj, &q->attr_type);

	kobject_del(&q->kobj);
	kobject_put(&q->kobj);
}

int kfd_process_create_wq(void)
{
	if (!kfd_process_wq)
		kfd_process_wq = alloc_workqueue("kfd_process_wq", 0, 0);
	if (!kfd_restore_wq)
		kfd_restore_wq = alloc_ordered_workqueue("kfd_restore_wq", 0);

	if (!kfd_process_wq || !kfd_restore_wq) {
		kfd_process_destroy_wq();
		return -ENOMEM;
	}

	return 0;
}

void kfd_process_destroy_wq(void)
{
	if (kfd_process_wq) {
		destroy_workqueue(kfd_process_wq);
		kfd_process_wq = NULL;
	}
	if (kfd_restore_wq) {
		destroy_workqueue(kfd_restore_wq);
		kfd_restore_wq = NULL;
	}
}

static void kfd_process_free_gpuvm(struct kgd_mem *mem,
			struct kfd_process_device *pdd, void *kptr)
{
	struct kfd_dev *dev = pdd->dev;

	if (kptr) {
		amdgpu_amdkfd_gpuvm_unmap_gtt_bo_from_kernel(dev->adev, mem);
		kptr = NULL;
	}

	amdgpu_amdkfd_gpuvm_unmap_memory_from_gpu(dev->adev, mem, pdd->drm_priv);
	amdgpu_amdkfd_gpuvm_free_memory_of_gpu(dev->adev, mem, pdd->drm_priv,
					       NULL);
}

/* kfd_process_alloc_gpuvm - Allocate GPU VM for the KFD process
 *	This function should be only called right after the process
 *	is created and when kfd_processes_mutex is still being held
 *	to avoid concurrency. Because of that exclusiveness, we do
 *	not need to take p->mutex.
 */
static int kfd_process_alloc_gpuvm(struct kfd_process_device *pdd,
				   uint64_t gpu_va, uint32_t size,
				   uint32_t flags, struct kgd_mem **mem, void **kptr)
{
	struct kfd_dev *kdev = pdd->dev;
	int err;

	err = amdgpu_amdkfd_gpuvm_alloc_memory_of_gpu(kdev->adev, gpu_va, size,
						 pdd->drm_priv, NULL, mem, NULL,
						 flags, false);
	if (err)
		goto err_alloc_mem;

	err = amdgpu_amdkfd_gpuvm_map_memory_to_gpu(kdev->adev, *mem,
			pdd->drm_priv, NULL);
	if (err)
		goto err_map_mem;

	err = amdgpu_amdkfd_gpuvm_sync_memory(kdev->adev, *mem, true);
	if (err) {
		pr_debug("Sync memory failed, wait interrupted by user signal\n");
		goto sync_memory_failed;
	}

	if (kptr) {
		err = amdgpu_amdkfd_gpuvm_map_gtt_bo_to_kernel(kdev->adev,
				(struct kgd_mem *)*mem, kptr, NULL);
		if (err) {
			pr_debug("Map GTT BO to kernel failed\n");
			goto sync_memory_failed;
		}
	}

	return err;

sync_memory_failed:
	amdgpu_amdkfd_gpuvm_unmap_memory_from_gpu(kdev->adev, *mem, pdd->drm_priv);

err_map_mem:
	amdgpu_amdkfd_gpuvm_free_memory_of_gpu(kdev->adev, *mem, pdd->drm_priv,
					       NULL);
err_alloc_mem:
	*mem = NULL;
	*kptr = NULL;
	return err;
}

/* kfd_process_device_reserve_ib_mem - Reserve memory inside the
 *	process for IB usage The memory reserved is for KFD to submit
 *	IB to AMDGPU from kernel.  If the memory is reserved
 *	successfully, ib_kaddr will have the CPU/kernel
 *	address. Check ib_kaddr before accessing the memory.
 */
static int kfd_process_device_reserve_ib_mem(struct kfd_process_device *pdd)
{
	struct qcm_process_device *qpd = &pdd->qpd;
	uint32_t flags = KFD_IOC_ALLOC_MEM_FLAGS_GTT |
			KFD_IOC_ALLOC_MEM_FLAGS_NO_SUBSTITUTE |
			KFD_IOC_ALLOC_MEM_FLAGS_WRITABLE |
			KFD_IOC_ALLOC_MEM_FLAGS_EXECUTABLE;
	struct kgd_mem *mem;
	void *kaddr;
	int ret;

	if (qpd->ib_kaddr || !qpd->ib_base)
		return 0;

	/* ib_base is only set for dGPU */
	ret = kfd_process_alloc_gpuvm(pdd, qpd->ib_base, PAGE_SIZE, flags,
				      &mem, &kaddr);
	if (ret)
		return ret;

	qpd->ib_mem = mem;
	qpd->ib_kaddr = kaddr;

	return 0;
}

static void kfd_process_device_destroy_ib_mem(struct kfd_process_device *pdd)
{
	struct qcm_process_device *qpd = &pdd->qpd;

	if (!qpd->ib_kaddr || !qpd->ib_base)
		return;

	kfd_process_free_gpuvm(qpd->ib_mem, pdd, qpd->ib_kaddr);
}

struct kfd_process *kfd_create_process(struct task_struct *thread)
{
	struct kfd_process *process;
	int ret;

	if (!(thread->mm && mmget_not_zero(thread->mm)))
		return ERR_PTR(-EINVAL);

	/* Only the pthreads threading model is supported. */
	if (thread->group_leader->mm != thread->mm) {
		mmput(thread->mm);
		return ERR_PTR(-EINVAL);
	}

	/*
	 * take kfd processes mutex before starting of process creation
	 * so there won't be a case where two threads of the same process
	 * create two kfd_process structures
	 */
	mutex_lock(&kfd_processes_mutex);

	/* A prior open of /dev/kfd could have already created the process. */
	process = find_process(thread, false);
	if (process) {
		pr_debug("Process already found\n");
	} else {
		process = create_process(thread);
		if (IS_ERR(process))
			goto out;

		if (!procfs.kobj)
			goto out;

		process->kobj = kfd_alloc_struct(process->kobj);
		if (!process->kobj) {
			pr_warn("Creating procfs kobject failed");
			goto out;
		}
		ret = kobject_init_and_add(process->kobj, &procfs_type,
					   procfs.kobj, "%d",
					   (int)process->lead_thread->pid);
		if (ret) {
			pr_warn("Creating procfs pid directory failed");
			kobject_put(process->kobj);
			goto out;
		}

		kfd_sysfs_create_file(process->kobj, &process->attr_pasid,
				      "pasid");

		process->kobj_queues = kobject_create_and_add("queues",
							process->kobj);
		if (!process->kobj_queues)
			pr_warn("Creating KFD proc/queues folder failed");

		kfd_procfs_add_sysfs_stats(process);
		kfd_procfs_add_sysfs_files(process);
		kfd_procfs_add_sysfs_counters(process);

		init_waitqueue_head(&process->wait_irq_drain);
	}
out:
	if (!IS_ERR(process))
		kref_get(&process->ref);
	mutex_unlock(&kfd_processes_mutex);
	mmput(thread->mm);

	return process;
}

struct kfd_process *kfd_get_process(const struct task_struct *thread)
{
	struct kfd_process *process;

	if (!thread->mm)
		return ERR_PTR(-EINVAL);

	/* Only the pthreads threading model is supported. */
	if (thread->group_leader->mm != thread->mm)
		return ERR_PTR(-EINVAL);

	process = find_process(thread, false);
	if (!process)
		return ERR_PTR(-EINVAL);

	return process;
}

static struct kfd_process *find_process_by_mm(const struct mm_struct *mm)
{
	struct kfd_process *process;

	hash_for_each_possible_rcu(kfd_processes_table, process,
					kfd_processes, (uintptr_t)mm)
		if (process->mm == mm)
			return process;

	return NULL;
}

static struct kfd_process *find_process(const struct task_struct *thread,
		bool ref)
{
	struct kfd_process *p;
	int idx;

	idx = srcu_read_lock(&kfd_processes_srcu);
	p = find_process_by_mm(thread->mm);
	if (p && ref)
		kref_get(&p->ref);
	srcu_read_unlock(&kfd_processes_srcu, idx);

	return p;
}

void kfd_unref_process(struct kfd_process *p)
{
	kref_put(&p->ref, kfd_process_ref_release);
}

/* This increments the process->ref counter. */
struct kfd_process *kfd_lookup_process_by_pid(struct pid *pid)
{
	struct task_struct *task = NULL;
	struct kfd_process *p    = NULL;

	if (!pid) {
		task = current;
		get_task_struct(task);
	} else {
		task = get_pid_task(pid, PIDTYPE_PID);
	}

	if (task) {
		p = find_process(task, true);
		put_task_struct(task);
	}

	return p;
}

static void kfd_process_device_free_bos(struct kfd_process_device *pdd)
{
	struct kfd_process *p = pdd->process;
	struct kfd_bo *buf_obj;
	int id;
	int i;

	/*
	 * Remove all handles from idr and release appropriate
	 * local memory object
	 */
	idr_for_each_entry(&pdd->alloc_idr, buf_obj, id) {

		for (i = 0; i < p->n_pdds; i++) {
			struct kfd_process_device *peer_pdd = p->pdds[i];

			if (!peer_pdd->drm_priv)
				continue;
			amdgpu_read_lock(peer_pdd->dev->ddev, false);
			amdgpu_amdkfd_gpuvm_unmap_memory_from_gpu(
				peer_pdd->dev->adev, buf_obj->mem, peer_pdd->drm_priv);
			amdgpu_read_unlock(peer_pdd->dev->ddev);
		}

		amdgpu_read_lock(pdd->dev->ddev, false);
		amdgpu_amdkfd_gpuvm_free_memory_of_gpu(pdd->dev->adev,
						      buf_obj->mem, pdd->drm_priv, NULL);
		amdgpu_read_unlock(pdd->dev->ddev);
		kfd_process_device_remove_obj_handle(pdd, id);
	}
}

/*
 * Just kunmap and unpin signal BO here. It will be freed in
 * kfd_process_free_outstanding_kfd_bos()
 */
static void kfd_process_kunmap_signal_bo(struct kfd_process *p)
{
	struct kfd_process_device *pdd;
	struct kfd_dev *kdev;
	void *mem;

	kdev = kfd_device_by_id(GET_GPU_ID(p->signal_handle));
	if (!kdev)
		return;

	mutex_lock(&p->mutex);

	pdd = kfd_get_process_device_data(kdev, p);
	if (!pdd)
		goto out;

	mem = kfd_process_device_translate_handle(
		pdd, GET_IDR_HANDLE(p->signal_handle));
	if (!mem)
		goto out;

	amdgpu_amdkfd_gpuvm_unmap_gtt_bo_from_kernel(kdev->adev, mem);

out:
	mutex_unlock(&p->mutex);
}

static void kfd_process_free_outstanding_kfd_bos(struct kfd_process *p)
{
	int i;

	for (i = 0; i < p->n_pdds; i++)
		kfd_process_device_free_bos(p->pdds[i]);
}

static void kfd_process_destroy_pdds(struct kfd_process *p)
{
	int i;

	for (i = 0; i < p->n_pdds; i++) {
		struct kfd_process_device *pdd = p->pdds[i];

		pr_debug("Releasing pdd (topology id %d) for process (pasid 0x%x)\n",
				pdd->dev->id, p->pasid);

		kfd_process_device_destroy_cwsr_dgpu(pdd);
		kfd_process_device_destroy_ib_mem(pdd);

		if (pdd->drm_file) {
			amdgpu_amdkfd_gpuvm_release_process_vm(
					pdd->dev->adev, pdd->drm_priv);
			fput(pdd->drm_file);
		}

		if (pdd->qpd.cwsr_kaddr && !pdd->qpd.cwsr_base)
			free_pages((unsigned long)pdd->qpd.cwsr_kaddr,
				get_order(KFD_CWSR_TBA_TMA_SIZE));
#if defined(HAVE_BITMAP_FUNCS)
		bitmap_free(pdd->qpd.doorbell_bitmap);
#else
		bitmap_free(pdd->qpd.doorbell_bitmap);
#endif
		idr_destroy(&pdd->alloc_idr);
		mutex_destroy(&pdd->qpd.doorbell_lock);

		kfd_free_process_doorbells(pdd->dev, pdd->doorbell_index);

		/*
		 * before destroying pdd, make sure to report availability
		 * for auto suspend
		 */
		if (pdd->runtime_inuse) {
			pm_runtime_mark_last_busy(pdd->dev->ddev->dev);
			pm_runtime_put_autosuspend(pdd->dev->ddev->dev);
			pdd->runtime_inuse = false;
		}

		kfree(pdd);
		p->pdds[i] = NULL;
	}
	p->n_pdds = 0;
}

static void kfd_process_remove_sysfs(struct kfd_process *p)
{
	struct kfd_process_device *pdd;
	int i;

	if (!p->kobj)
		return;

	sysfs_remove_file(p->kobj, &p->attr_pasid);
	kobject_del(p->kobj_queues);
	kobject_put(p->kobj_queues);
	p->kobj_queues = NULL;

	for (i = 0; i < p->n_pdds; i++) {
		pdd = p->pdds[i];

		sysfs_remove_file(p->kobj, &pdd->attr_vram);
		sysfs_remove_file(p->kobj, &pdd->attr_sdma);

		sysfs_remove_file(pdd->kobj_stats, &pdd->attr_evict);
		if (pdd->dev->kfd2kgd->get_cu_occupancy)
			sysfs_remove_file(pdd->kobj_stats,
					  &pdd->attr_cu_occupancy);
		kobject_del(pdd->kobj_stats);
		kobject_put(pdd->kobj_stats);
		pdd->kobj_stats = NULL;
	}

	for_each_set_bit(i, p->svms.bitmap_supported, p->n_pdds) {
		pdd = p->pdds[i];

		sysfs_remove_file(pdd->kobj_counters, &pdd->attr_faults);
		sysfs_remove_file(pdd->kobj_counters, &pdd->attr_page_in);
		sysfs_remove_file(pdd->kobj_counters, &pdd->attr_page_out);
		kobject_del(pdd->kobj_counters);
		kobject_put(pdd->kobj_counters);
		pdd->kobj_counters = NULL;
	}

	kobject_del(p->kobj);
	kobject_put(p->kobj);
	p->kobj = NULL;
}

/* No process locking is needed in this function, because the process
 * is not findable any more. We must assume that no other thread is
 * using it any more, otherwise we couldn't safely free the process
 * structure in the end.
 */
static void kfd_process_wq_release(struct work_struct *work)
{
	struct kfd_process *p = container_of(work, struct kfd_process,
					     release_work);

	kfd_process_remove_sysfs(p);
	kfd_iommu_unbind_process(p);

	kfd_process_kunmap_signal_bo(p);
	kfd_process_free_outstanding_kfd_bos(p);
	svm_range_list_fini(p);

	kfd_process_destroy_pdds(p);
	dma_fence_put(p->ef);

	kfd_event_free_process(p);

	kfd_pasid_free(p->pasid);
	mutex_destroy(&p->mutex);

	put_task_struct(p->lead_thread);

	kfree(p);
}

static void kfd_process_ref_release(struct kref *ref)
{
	struct kfd_process *p = container_of(ref, struct kfd_process, ref);

	INIT_WORK(&p->release_work, kfd_process_wq_release);
	queue_work(kfd_process_wq, &p->release_work);
}

#ifdef HAVE_MMU_NOTIFIER_PUT
static struct mmu_notifier *kfd_process_alloc_notifier(struct mm_struct *mm)
{
	int idx = srcu_read_lock(&kfd_processes_srcu);
	struct kfd_process *p = find_process_by_mm(mm);

	srcu_read_unlock(&kfd_processes_srcu, idx);

	return p ? &p->mmu_notifier : ERR_PTR(-ESRCH);
}

static void kfd_process_free_notifier(struct mmu_notifier *mn)
{
	kfd_unref_process(container_of(mn, struct kfd_process, mmu_notifier));
}
#else
static void kfd_process_destroy_delayed(struct rcu_head *rcu)
{
	struct kfd_process *p = container_of(rcu, struct kfd_process, rcu);

	kfd_unref_process(p);
}
#endif

static void kfd_process_notifier_release(struct mmu_notifier *mn,
					struct mm_struct *mm)
{
	struct kfd_process *p;
	int i;

	/*
	 * The kfd_process structure can not be free because the
	 * mmu_notifier srcu is read locked
	 */
	p = container_of(mn, struct kfd_process, mmu_notifier);
	if (WARN_ON(p->mm != mm))
		return;

	mutex_lock(&kfd_processes_mutex);
	hash_del_rcu(&p->kfd_processes);
	mutex_unlock(&kfd_processes_mutex);
	synchronize_srcu(&kfd_processes_srcu);

	cancel_delayed_work_sync(&p->eviction_work);
	cancel_delayed_work_sync(&p->restore_work);

	mutex_lock(&p->mutex);

	/* Iterate over all process device data structures and if the
	 * pdd is in debug mode, we should first force unregistration,
	 * then we will be able to destroy the queues. Also invalidate
	 * doorbell_vma private data because the pdds are about to be
	 * destroyed, which can race with the kfd_doorbell_close
	 * vm_ops callback.
	 */
	for (i = 0; i < p->n_pdds; i++) {
		struct kfd_process_device *pdd = p->pdds[i];
		struct kfd_dev *dev = pdd->dev;

		if (pdd->qpd.doorbell_vma)
			pdd->qpd.doorbell_vma->vm_private_data = NULL;

		/* re-enable GFX OFF since runtime enable with ttmp setup disabled it. */
		if (!kfd_dbg_is_rlc_restore_supported(pdd->dev) && p->runtime_info.ttmp_setup)
			amdgpu_amdkfd_gfx_off_ctrl(pdd->dev->adev, true);
	}

	/* New debugger for GFXv9 and later */
	if (p->debug_trap_enabled) {
		kfd_dbg_trap_disable(p);
	}

	/* If we are the debugger, we need to clean up the debugged process.
	 * We should disable any debugging options enabled, and resume
	 * any suspended queues.
	 */
	if (atomic_read(&p->debugged_process_count) > 0) {
		struct kfd_process *target;
		unsigned int temp;
		int idx = srcu_read_lock(&kfd_processes_srcu);

		hash_for_each_rcu(kfd_processes_table, temp, target, kfd_processes) {
			if (target->debugger_process &&
				target->debugger_process == p) {
				mutex_lock_nested(&target->mutex, 1);
				if (target->debug_trap_enabled)
					kfd_dbg_trap_disable(target);
				mutex_unlock(&target->mutex);
				if (atomic_read(&p->debugged_process_count) == 0)
					break;
			}
		}
		srcu_read_unlock(&kfd_processes_srcu, idx);
	}

	kfd_process_dequeue_from_all_devices(p);
	pqm_uninit(&p->pqm);

	/* Indicate to other users that MM is no longer valid */
	p->mm = NULL;
	/* Signal the eviction fence after user mode queues are
	 * destroyed. This allows any BOs to be freed without
	 * triggering pointless evictions or waiting for fences.
	 */
	dma_fence_signal(p->ef);

	mutex_unlock(&p->mutex);

#ifdef HAVE_MMU_NOTIFIER_PUT
	mmu_notifier_put(&p->mmu_notifier);
#else
	mmu_notifier_unregister_no_release(&p->mmu_notifier, mm);
	mmu_notifier_call_srcu(&p->rcu, &kfd_process_destroy_delayed);
#endif
}

static const struct mmu_notifier_ops kfd_process_mmu_notifier_ops = {
	.release = kfd_process_notifier_release,
#ifdef HAVE_MMU_NOTIFIER_PUT
	.alloc_notifier = kfd_process_alloc_notifier,
	.free_notifier = kfd_process_free_notifier,
#endif
};

int kfd_process_init_cwsr_apu(struct kfd_process *p, struct file *filep)
{
	unsigned long  offset;
	int i;

	if (p->has_cwsr)
		return 0;

	for (i = 0; i < p->n_pdds; i++) {
		struct kfd_dev *dev = p->pdds[i]->dev;
		struct qcm_process_device *qpd = &p->pdds[i]->qpd;

		if (!dev->cwsr_enabled || qpd->cwsr_kaddr || qpd->cwsr_base)
			continue;

		offset = KFD_MMAP_TYPE_RESERVED_MEM | KFD_MMAP_GPU_ID(dev->id);
		qpd->tba_addr = (int64_t)vm_mmap(filep, 0,
			KFD_CWSR_TBA_TMA_SIZE, PROT_READ | PROT_EXEC,
			MAP_SHARED, offset);

		if (IS_ERR_VALUE(qpd->tba_addr)) {
			int err = qpd->tba_addr;

			pr_err("Failure to set tba address. error %d.\n", err);
			qpd->tba_addr = 0;
			qpd->cwsr_kaddr = NULL;
			return err;
		}

		memcpy(qpd->cwsr_kaddr, dev->cwsr_isa, dev->cwsr_isa_size);

		kfd_process_set_trap_debug_flag(qpd, p->debug_trap_enabled);

		qpd->tma_addr = qpd->tba_addr + KFD_CWSR_TMA_OFFSET;
		pr_debug("set tba :0x%llx, tma:0x%llx, cwsr_kaddr:%p for pqm.\n",
			qpd->tba_addr, qpd->tma_addr, qpd->cwsr_kaddr);
	}

	p->has_cwsr = true;

	return 0;
}

static int kfd_process_device_init_cwsr_dgpu(struct kfd_process_device *pdd)
{
	struct kfd_dev *dev = pdd->dev;
	struct qcm_process_device *qpd = &pdd->qpd;
	uint32_t flags = KFD_IOC_ALLOC_MEM_FLAGS_GTT
			| KFD_IOC_ALLOC_MEM_FLAGS_NO_SUBSTITUTE
			| KFD_IOC_ALLOC_MEM_FLAGS_EXECUTABLE;
	struct kgd_mem *mem;
	void *kaddr;
	int ret;

	if (!dev->cwsr_enabled || qpd->cwsr_kaddr || !qpd->cwsr_base)
		return 0;

	/* cwsr_base is only set for dGPU */
	ret = kfd_process_alloc_gpuvm(pdd, qpd->cwsr_base,
				      KFD_CWSR_TBA_TMA_SIZE, flags, &mem, &kaddr);
	if (ret)
		return ret;

	qpd->cwsr_mem = mem;
	qpd->cwsr_kaddr = kaddr;
	qpd->tba_addr = qpd->cwsr_base;

	memcpy(qpd->cwsr_kaddr, dev->cwsr_isa, dev->cwsr_isa_size);

	kfd_process_set_trap_debug_flag(&pdd->qpd,
					pdd->process->debug_trap_enabled);

	qpd->tma_addr = qpd->tba_addr + KFD_CWSR_TMA_OFFSET;
	pr_debug("set tba :0x%llx, tma:0x%llx, cwsr_kaddr:%p for pqm.\n",
		 qpd->tba_addr, qpd->tma_addr, qpd->cwsr_kaddr);

	return 0;
}

static void kfd_process_device_destroy_cwsr_dgpu(struct kfd_process_device *pdd)
{
	struct kfd_dev *dev = pdd->dev;
	struct qcm_process_device *qpd = &pdd->qpd;

	if (!dev->cwsr_enabled || !qpd->cwsr_kaddr || !qpd->cwsr_base)
		return;

	kfd_process_free_gpuvm(qpd->cwsr_mem, pdd, qpd->cwsr_kaddr);
}

void kfd_process_set_trap_handler(struct qcm_process_device *qpd,
				  uint64_t tba_addr,
				  uint64_t tma_addr)
{
	if (qpd->cwsr_kaddr) {
		/* KFD trap handler is bound, record as second-level TBA/TMA
		 * in first-level TMA. First-level trap will jump to second.
		 */
		uint64_t *tma =
			(uint64_t *)(qpd->cwsr_kaddr + KFD_CWSR_TMA_OFFSET);
		tma[0] = tba_addr;
		tma[1] = tma_addr;
	} else {
		/* No trap handler bound, bind as first-level TBA/TMA. */
		qpd->tba_addr = tba_addr;
		qpd->tma_addr = tma_addr;
	}
}

bool kfd_process_xnack_mode(struct kfd_process *p, bool supported)
{
	int i;

	/* On most GFXv9 GPUs, the retry mode in the SQ must match the
	 * boot time retry setting. Mixing processes with different
	 * XNACK/retry settings can hang the GPU.
	 *
	 * Different GPUs can have different noretry settings depending
	 * on HW bugs or limitations. We need to find at least one
	 * XNACK mode for this process that's compatible with all GPUs.
	 * Fortunately GPUs with retry enabled (noretry=0) can run code
	 * built for XNACK-off. On GFXv9 it may perform slower.
	 *
	 * Therefore applications built for XNACK-off can always be
	 * supported and will be our fallback if any GPU does not
	 * support retry.
	 */
	for (i = 0; i < p->n_pdds; i++) {
		struct kfd_dev *dev = p->pdds[i]->dev;

		/* Only consider GFXv9 and higher GPUs. Older GPUs don't
		 * support the SVM APIs and don't need to be considered
		 * for the XNACK mode selection.
		 */
		if (!KFD_IS_SOC15(dev))
			continue;
		/* Aldebaran can always support XNACK because it can support
		 * per-process XNACK mode selection. But let the dev->noretry
		 * setting still influence the default XNACK mode.
		 */
		if (supported && KFD_GC_VERSION(dev) == IP_VERSION(9, 4, 2))
			continue;

		/* GFXv10 and later GPUs do not support shader preemption
		 * during page faults. This can lead to poor QoS for queue
		 * management and memory-manager-related preemptions or
		 * even deadlocks.
		 */
		if (KFD_GC_VERSION(dev) >= IP_VERSION(10, 1, 1))
			return false;

		if (dev->noretry)
			return false;
	}

	return true;
}

void kfd_process_set_trap_debug_flag(struct qcm_process_device *qpd,
				     bool enabled)
{
	/* If TMA doesn't exist then flag will be set during allocation. */
	if (qpd->cwsr_kaddr) {
		uint64_t *tma =
			(uint64_t *)(qpd->cwsr_kaddr + KFD_CWSR_TMA_OFFSET);
		tma[2] = enabled;
	}
}

/*
 * On return the kfd_process is fully operational and will be freed when the
 * mm is released
 */
static struct kfd_process *create_process(const struct task_struct *thread)
{
	struct kfd_process *process;
#ifdef HAVE_MMU_NOTIFIER_PUT
	struct mmu_notifier *mn;
#endif
	int err = -ENOMEM;

	process = kzalloc(sizeof(*process), GFP_KERNEL);
	if (!process)
		goto err_alloc_process;

	kref_init(&process->ref);
	mutex_init(&process->mutex);
	process->mm = thread->mm;
	process->lead_thread = thread->group_leader;
	process->n_pdds = 0;
	process->queues_paused = false;
	INIT_DELAYED_WORK(&process->eviction_work, evict_process_worker);
	INIT_DELAYED_WORK(&process->restore_work, restore_process_worker);
	process->last_restore_timestamp = get_jiffies_64();
	kfd_event_init_process(process);
	process->is_32bit_user_mode = in_compat_syscall();
	process->debug_trap_enabled = false;
	process->debugger_process = NULL;
	atomic_set(&process->debugged_process_count, 0);
	sema_init(&process->runtime_enable_sema, 0);

	process->pasid = kfd_pasid_alloc();
	if (process->pasid == 0)
		goto err_alloc_pasid;

	err = pqm_init(&process->pqm, process);
	if (err != 0)
		goto err_process_pqm_init;

	/* init process apertures*/
	err = kfd_init_apertures(process);
	if (err != 0)
		goto err_init_apertures;

	/* Check XNACK support after PDDs are created in kfd_init_apertures */
	process->xnack_enabled = kfd_process_xnack_mode(process, false);

	err = svm_range_list_init(process);
	if (err)
		goto err_init_svm_range_list;

	/* alloc_notifier needs to find the process in the hash table */
	hash_add_rcu(kfd_processes_table, &process->kfd_processes,
			(uintptr_t)process->mm);

#ifdef HAVE_MMU_NOTIFIER_PUT
	/* MMU notifier registration must be the last call that can fail
	 * because after this point we cannot unwind the process creation.
	 * After this point, mmu_notifier_put will trigger the cleanup by
	 * dropping the last process reference in the free_notifier.
	 */
	mn = mmu_notifier_get(&kfd_process_mmu_notifier_ops, process->mm);
	if (IS_ERR(mn)) {
		err = PTR_ERR(mn);
		goto err_register_notifier;
	}
	BUG_ON(mn != &process->mmu_notifier);
#else
	/* Must be last, have to use release destruction after this */
	process->mmu_notifier.ops = &kfd_process_mmu_notifier_ops;
	err = mmu_notifier_register(&process->mmu_notifier, process->mm);
	if (err)
		goto err_register_notifier;
#endif

	get_task_struct(process->lead_thread);

	/* If PeerDirect interface was not detected try to detect it again
	 * in case if network driver was loaded later.
	 */
	kfd_init_peer_direct();

	INIT_WORK(&process->debug_event_workarea,
			debug_event_write_work_handler);

	return process;

err_register_notifier:
	hash_del_rcu(&process->kfd_processes);
	svm_range_list_fini(process);
err_init_svm_range_list:
	kfd_process_free_outstanding_kfd_bos(process);
	kfd_process_destroy_pdds(process);
err_init_apertures:
	pqm_uninit(&process->pqm);
err_process_pqm_init:
	kfd_pasid_free(process->pasid);
err_alloc_pasid:
	mutex_destroy(&process->mutex);
	kfree(process);
err_alloc_process:
	return ERR_PTR(err);
}

static int init_doorbell_bitmap(struct qcm_process_device *qpd,
			struct kfd_dev *dev)
{
	unsigned int i;
	int range_start = dev->shared_resources.non_cp_doorbells_start;
	int range_end = dev->shared_resources.non_cp_doorbells_end;

	if (!KFD_IS_SOC15(dev))
		return 0;

#if defined(HAVE_BITMAP_FUNCS)
	qpd->doorbell_bitmap = bitmap_zalloc(KFD_MAX_NUM_OF_QUEUES_PER_PROCESS,
					     GFP_KERNEL);
#else
	qpd->doorbell_bitmap = bitmap_zalloc(KFD_MAX_NUM_OF_QUEUES_PER_PROCESS,
					     GFP_KERNEL);
#endif
	if (!qpd->doorbell_bitmap)
		return -ENOMEM;

	/* Mask out doorbells reserved for SDMA, IH, and VCN on SOC15. */
	pr_debug("reserved doorbell 0x%03x - 0x%03x\n", range_start, range_end);
	pr_debug("reserved doorbell 0x%03x - 0x%03x\n",
			range_start + KFD_QUEUE_DOORBELL_MIRROR_OFFSET,
			range_end + KFD_QUEUE_DOORBELL_MIRROR_OFFSET);

	for (i = 0; i < KFD_MAX_NUM_OF_QUEUES_PER_PROCESS / 2; i++) {
		if (i >= range_start && i <= range_end) {
			__set_bit(i, qpd->doorbell_bitmap);
			__set_bit(i + KFD_QUEUE_DOORBELL_MIRROR_OFFSET,
				  qpd->doorbell_bitmap);
		}
	}

	return 0;
}

struct kfd_process_device *kfd_get_process_device_data(struct kfd_dev *dev,
							struct kfd_process *p)
{
	int i;

	for (i = 0; i < p->n_pdds; i++)
		if (p->pdds[i]->dev == dev)
			return p->pdds[i];

	return NULL;
}

struct kfd_process_device *kfd_create_process_device_data(struct kfd_dev *dev,
							struct kfd_process *p)
{
	struct kfd_process_device *pdd = NULL;

	if (WARN_ON_ONCE(p->n_pdds >= MAX_GPU_INSTANCE))
		return NULL;
	pdd = kzalloc(sizeof(*pdd), GFP_KERNEL);
	if (!pdd)
		return NULL;

	if (kfd_alloc_process_doorbells(dev, &pdd->doorbell_index) < 0) {
		pr_err("Failed to alloc doorbell for pdd\n");
		goto err_free_pdd;
	}

	if (init_doorbell_bitmap(&pdd->qpd, dev)) {
		pr_err("Failed to init doorbell for process\n");
		goto err_free_pdd;
	}

	pdd->dev = dev;
	INIT_LIST_HEAD(&pdd->qpd.queues_list);
	INIT_LIST_HEAD(&pdd->qpd.priv_queue_list);
	pdd->qpd.dqm = dev->dqm;
	pdd->qpd.pqm = &p->pqm;
	pdd->qpd.evicted = 0;
	pdd->qpd.mapped_gws_queue = false;
	mutex_init(&pdd->qpd.doorbell_lock);
	pdd->process = p;
	pdd->bound = PDD_UNBOUND;
	pdd->already_dequeued = false;
	pdd->runtime_inuse = false;
	pdd->vram_usage = 0;
	pdd->sdma_past_activity_counter = 0;
	pdd->user_gpu_id = dev->id;
	atomic64_set(&pdd->evict_duration_counter, 0);
	kfd_spm_init_process_device(pdd);
	p->pdds[p->n_pdds++] = pdd;
	if (kfd_dbg_is_per_vmid_supported(pdd->dev))
		pdd->spi_dbg_override = pdd->dev->kfd2kgd->disable_debug_trap(
							pdd->dev->adev,
							false,
							0);

	/* Init idr used for memory handle translation */
	idr_init(&pdd->alloc_idr);

	return pdd;

err_free_pdd:
	kfree(pdd);
	return NULL;
}

/**
 * kfd_process_device_init_vm - Initialize a VM for a process-device
 *
 * @pdd: The process-device
 * @drm_file: Optional pointer to a DRM file descriptor
 *
 * If @drm_file is specified, it will be used to acquire the VM from
 * that file descriptor. If successful, the @pdd takes ownership of
 * the file descriptor.
 *
 * If @drm_file is NULL, a new VM is created.
 *
 * Returns 0 on success, -errno on failure.
 */
int kfd_process_device_init_vm(struct kfd_process_device *pdd,
			       struct file *drm_file)
{
	struct kfd_process *p;
	struct kfd_dev *dev;
	int ret;

	if (!drm_file)
		return -EINVAL;

	if (pdd->drm_priv)
		return -EBUSY;

	p = pdd->process;
	dev = pdd->dev;

	ret = amdgpu_amdkfd_gpuvm_acquire_process_vm(
		dev->adev, drm_file, p->pasid,
		&p->kgd_process_info, &p->ef);
	if (ret) {
		pr_err("Failed to create process VM object\n");
		return ret;
	}
	pdd->drm_priv = drm_file->private_data;

	ret = kfd_process_device_reserve_ib_mem(pdd);
	if (ret)
		goto err_reserve_ib_mem;
	ret = kfd_process_device_init_cwsr_dgpu(pdd);
	if (ret)
		goto err_init_cwsr;

	pdd->drm_file = drm_file;

	return 0;

err_init_cwsr:
err_reserve_ib_mem:
	kfd_process_device_free_bos(pdd);
	pdd->drm_priv = NULL;

	return ret;
}

/*
 * Direct the IOMMU to bind the process (specifically the pasid->mm)
 * to the device.
 * Unbinding occurs when the process dies or the device is removed.
 *
 * Assumes that the process lock is held.
 */
struct kfd_process_device *kfd_bind_process_to_device(struct kfd_dev *dev,
							struct kfd_process *p)
{
	struct kfd_process_device *pdd;
	int err;

	pdd = kfd_get_process_device_data(dev, p);
	if (!pdd) {
		pr_err("Process device data doesn't exist\n");
		return ERR_PTR(-ENOMEM);
	}

	if (!pdd->drm_priv)
		return ERR_PTR(-ENODEV);

	/*
	 * signal runtime-pm system to auto resume and prevent
	 * further runtime suspend once device pdd is created until
	 * pdd is destroyed.
	 */
	if (!pdd->runtime_inuse) {
		err = pm_runtime_get_sync(dev->ddev->dev);
		if (err < 0) {
			pm_runtime_put_autosuspend(dev->ddev->dev);
			return ERR_PTR(err);
		}
	}

	err = kfd_iommu_bind_process_to_device(pdd);
	if (err)
		goto out;

	/*
	 * make sure that runtime_usage counter is incremented just once
	 * per pdd
	 */
	pdd->runtime_inuse = true;

	return pdd;

out:
	/* balance runpm reference count and exit with error */
	if (!pdd->runtime_inuse) {
		pm_runtime_mark_last_busy(dev->ddev->dev);
		pm_runtime_put_autosuspend(dev->ddev->dev);
	}

	return ERR_PTR(err);
}

/* Create specific handle mapped to mem from process local memory idr
 * Assumes that the process lock is held.
 */
int kfd_process_device_create_obj_handle(struct kfd_process_device *pdd,
					void *mem, uint64_t start,
					uint64_t length, uint64_t cpuva,
					unsigned int mem_type,
					int preferred_id)
{
	int handle;
	struct kfd_bo *buf_obj;
	struct kfd_process *p;

	p = pdd->process;

	buf_obj = kzalloc(sizeof(*buf_obj), GFP_KERNEL);

	if (!buf_obj)
		return -ENOMEM;

	buf_obj->it.start = start;
	buf_obj->it.last = start + length - 1;
	interval_tree_insert(&buf_obj->it, &p->bo_interval_tree);

	buf_obj->mem = mem;
	buf_obj->dev = pdd->dev;
	buf_obj->cpuva = cpuva;
	buf_obj->mem_type = mem_type;

	if (preferred_id < 0)
		handle = idr_alloc(&pdd->alloc_idr, buf_obj, 0, 0, GFP_KERNEL);
	else
		handle = idr_alloc(&pdd->alloc_idr, buf_obj, preferred_id,
						preferred_id + 1, GFP_KERNEL);

	if (handle < 0)
		kfree(buf_obj);

	return handle;
}

struct kfd_bo *kfd_process_device_find_bo(struct kfd_process_device *pdd,
					int handle)
{
	if (handle < 0)
		return NULL;

	return (struct kfd_bo *)idr_find(&pdd->alloc_idr, handle);
}

/* Translate specific handle from process local memory idr
 * Assumes that the process lock is held.
 */
void *kfd_process_device_translate_handle(struct kfd_process_device *pdd,
					int handle)
{
	struct kfd_bo *buf_obj;

	buf_obj = kfd_process_device_find_bo(pdd, handle);

	return buf_obj->mem;
}

void *kfd_process_find_bo_from_interval(struct kfd_process *p,
					uint64_t start_addr,
					uint64_t last_addr)
{
	struct interval_tree_node *it_node;
	struct kfd_bo *buf_obj;

	it_node = interval_tree_iter_first(&p->bo_interval_tree,
			start_addr, last_addr);
	if (!it_node) {
		pr_err("0x%llx-0x%llx does not relate to an existing buffer\n",
				start_addr, last_addr);
		return NULL;
	}

	if (interval_tree_iter_next(it_node, start_addr, last_addr)) {
		pr_err("0x%llx-0x%llx spans more than a single BO\n",
				start_addr, last_addr);
		return NULL;
	}

	buf_obj = container_of(it_node, struct kfd_bo, it);

	return buf_obj;
}

/* Remove specific handle from process local memory idr
 * Assumes that the process lock is held.
 */
void kfd_process_device_remove_obj_handle(struct kfd_process_device *pdd,
					int handle)
{
	struct kfd_bo *buf_obj;
	struct kfd_process *p;

	p = pdd->process;

	if (handle < 0)
		return;

	buf_obj = kfd_process_device_find_bo(pdd, handle);

	idr_remove(&pdd->alloc_idr, handle);

	interval_tree_remove(&buf_obj->it, &p->bo_interval_tree);

	kfree(buf_obj);
}

/* This increments the process->ref counter. */
struct kfd_process *kfd_lookup_process_by_pasid(u32 pasid)
{
	struct kfd_process *p, *ret_p = NULL;
	unsigned int temp;

	int idx = srcu_read_lock(&kfd_processes_srcu);

	hash_for_each_rcu(kfd_processes_table, temp, p, kfd_processes) {
		if (p->pasid == pasid) {
			kref_get(&p->ref);
			ret_p = p;
			break;
		}
	}

	srcu_read_unlock(&kfd_processes_srcu, idx);

	return ret_p;
}

/* This increments the process->ref counter. */
struct kfd_process *kfd_lookup_process_by_mm(const struct mm_struct *mm)
{
	struct kfd_process *p;

	int idx = srcu_read_lock(&kfd_processes_srcu);

	p = find_process_by_mm(mm);
	if (p)
		kref_get(&p->ref);

	srcu_read_unlock(&kfd_processes_srcu, idx);

	return p;
}

/* kfd_process_evict_queues - Evict all user queues of a process
 *
 * Eviction is reference-counted per process-device. This means multiple
 * evictions from different sources can be nested safely.
 */
int kfd_process_evict_queues(struct kfd_process *p, bool force)
{
	int r = 0, r_tmp = 0;
	int i;
	unsigned int n_evicted = 0;

	for (i = 0; i < p->n_pdds; i++) {
		struct kfd_process_device *pdd = p->pdds[i];

		r_tmp = pdd->dev->dqm->ops.evict_process_queues(pdd->dev->dqm,
							    &pdd->qpd);
		/* evict return -EIO if HWS is hang or asic is resetting, in this case
		 * we would like to set all the queues to be in evicted state to prevent
		 * them been add back since they actually not be saved right now.
		 */
		if (r_tmp && r_tmp != -EIO) {
			pr_err("Failed to evict process queues\n");
			r = r_tmp;
			if (!force)
				goto fail;
		}
		n_evicted++;
	}

	return r;

fail:
	/* To keep state consistent, roll back partial eviction by
	 * restoring queues
	 */
	for (i = 0; i < p->n_pdds; i++) {
		struct kfd_process_device *pdd = p->pdds[i];

		if (n_evicted == 0)
			break;
		if (pdd->dev->dqm->ops.restore_process_queues(pdd->dev->dqm,
							      &pdd->qpd))
			pr_err("Failed to restore queues\n");

		n_evicted--;
	}

	return r;
}

/* kfd_process_restore_queues - Restore all user queues of a process */
int kfd_process_restore_queues(struct kfd_process *p)
{
	int r, ret = 0;
	int i;

	for (i = 0; i < p->n_pdds; i++) {
		struct kfd_process_device *pdd = p->pdds[i];

		r = pdd->dev->dqm->ops.restore_process_queues(pdd->dev->dqm,
							      &pdd->qpd);
		if (r) {
			pr_err("Failed to restore process queues\n");
			if (!ret)
				ret = r;
		}
	}

	return ret;
}

int kfd_process_gpuidx_from_gpuid(struct kfd_process *p, uint32_t gpu_id)
{
	int i;

	for (i = 0; i < p->n_pdds; i++)
		if (p->pdds[i] && gpu_id == p->pdds[i]->user_gpu_id)
			return i;
	return -EINVAL;
}

int
kfd_process_gpuid_from_adev(struct kfd_process *p, struct amdgpu_device *adev,
			   uint32_t *gpuid, uint32_t *gpuidx)
{
	int i;

	for (i = 0; i < p->n_pdds; i++)
		if (p->pdds[i] && p->pdds[i]->dev->adev == adev) {
			*gpuid = p->pdds[i]->user_gpu_id;
			*gpuidx = i;
			return 0;
		}
	return -EINVAL;
}

void kfd_process_schedule_restore(struct kfd_process *p)
{
	int ret;
	unsigned long evicted_jiffies;
	unsigned long delay_jiffies = msecs_to_jiffies(PROCESS_RESTORE_TIME_MS);

	/* wait at least PROCESS_RESTORE_TIME_MS before attempting to restore
	 */
	evicted_jiffies = get_jiffies_64() - p->last_evict_timestamp;
	if (delay_jiffies > evicted_jiffies)
		delay_jiffies -= evicted_jiffies;
	else
		delay_jiffies = 0;

	pr_debug("Process %d schedule restore work\n", p->pasid);
	ret = queue_delayed_work(kfd_restore_wq, &p->restore_work,
				delay_jiffies);
	WARN(!ret, "Schedule restore work failed\n");
}

static void kfd_process_unmap_doorbells(struct kfd_process *p)
{
	struct mm_struct *mm = p->mm;
	int i;

	mmap_write_lock(mm);

	for (i = 0; i < p->n_pdds; i++)
		kfd_doorbell_unmap(p->pdds[i]);

	mmap_write_unlock(mm);
}

int kfd_process_remap_doorbells_locked(struct kfd_process *p)
{
	int ret = 0;
	int i;

	for (i = 0; i < p->n_pdds; i++)
		ret = kfd_doorbell_remap(p->pdds[i]);

	return ret;
}

static int kfd_process_remap_doorbells(struct kfd_process *p)
{
	struct mm_struct *mm = p->mm;
	int ret = 0;

	mmap_write_lock(mm);
	ret = kfd_process_remap_doorbells_locked(p);
	mmap_write_unlock(mm);

	return ret;
}

/**
 * kfd_process_unmap_doorbells_if_idle - Check if queues are active
 *
 * Returns true if queues are idle, and unmap doorbells.
 * Returns false if queues are active
 */
static bool kfd_process_unmap_doorbells_if_idle(struct kfd_process *p)
{
	bool busy = false;
	int i;

	if (!keep_idle_process_evicted)
		return false;

	/* Unmap doorbell first to avoid race conditions. Otherwise while the
	 * second queue is checked, the first queue may get more work, but we
	 * won't detect that since it has been checked
	 */
	kfd_process_unmap_doorbells(p);

	for (i = 0; i < p->n_pdds; i++) {
		struct kfd_process_device *pdd = p->pdds[i];

		busy = check_if_queues_active(pdd->qpd.dqm, &pdd->qpd);
		if (busy)
			break;
	}

	/* Remap doorbell if process queue is not idle */
	if (busy)
		kfd_process_remap_doorbells(p);

	return !busy;
}

static void evict_process_worker(struct work_struct *work)
{
	int ret;
	struct kfd_process *p;
	struct delayed_work *dwork;

	dwork = to_delayed_work(work);

	/* Process termination destroys this worker thread. So during the
	 * lifetime of this thread, kfd_process p will be valid
	 */
	p = container_of(dwork, struct kfd_process, eviction_work);
	trace_kfd_evict_process_worker_start(p);
	WARN_ONCE(p->last_eviction_seqno != p->ef->seqno,
		  "Eviction fence mismatch\n");

	/* Narrow window of overlap between restore and evict work
	 * item is possible. Once amdgpu_amdkfd_gpuvm_restore_process_bos
	 * unreserves KFD BOs, it is possible to evicted again. But
	 * restore has few more steps of finish. So lets wait for any
	 * previous restore work to complete
	 */
	flush_delayed_work(&p->restore_work);

	p->last_evict_timestamp = get_jiffies_64();

	pr_debug("Started evicting pasid 0x%x\n", p->pasid);
	ret = kfd_process_evict_queues(p, false);
	if (!ret) {
		dma_fence_signal(p->ef);
		dma_fence_put(p->ef);
		p->ef = NULL;

		if (!kfd_process_unmap_doorbells_if_idle(p))
			kfd_process_schedule_restore(p);
		else
			pr_debug("Process %d queues idle, doorbell unmapped\n",
				p->pasid);

		pr_debug("Finished evicting pasid 0x%x\n", p->pasid);
	} else
		pr_err("Failed to evict queues of pasid 0x%x\n", p->pasid);
	trace_kfd_evict_process_worker_end(p, ret ? "Failed" : "Success");
}

static void restore_process_worker(struct work_struct *work)
{
	struct delayed_work *dwork;
	struct kfd_process *p;
	int ret = 0;

	dwork = to_delayed_work(work);

	/* Process termination destroys this worker thread. So during the
	 * lifetime of this thread, kfd_process p will be valid
	 */
	p = container_of(dwork, struct kfd_process, restore_work);
	pr_debug("Started restoring pasid 0x%x\n", p->pasid);
	trace_kfd_restore_process_worker_start(p);

	/* Setting last_restore_timestamp before successful restoration.
	 * Otherwise this would have to be set by KGD (restore_process_bos)
	 * before KFD BOs are unreserved. If not, the process can be evicted
	 * again before the timestamp is set.
	 * If restore fails, the timestamp will be set again in the next
	 * attempt. This would mean that the minimum GPU quanta would be
	 * PROCESS_ACTIVE_TIME_MS - (time to execute the following two
	 * functions)
	 */

	p->last_restore_timestamp = get_jiffies_64();
	/* VMs may not have been acquired yet during debugging. */
	if (p->kgd_process_info)
		ret = amdgpu_amdkfd_gpuvm_restore_process_bos(p->kgd_process_info,
							     &p->ef);
	if (ret) {
		pr_debug("Failed to restore BOs of pasid 0x%x, retry after %d ms\n",
			 p->pasid, PROCESS_BACK_OFF_TIME_MS);
		ret = queue_delayed_work(kfd_restore_wq, &p->restore_work,
				msecs_to_jiffies(PROCESS_BACK_OFF_TIME_MS));
		WARN(!ret, "reschedule restore work failed\n");
		trace_kfd_restore_process_worker_end(p, ret ?
					"Rescheduled restore" :
					"Failed to reschedule restore");
		return;
	}

	ret = kfd_process_restore_queues(p);
	trace_kfd_restore_process_worker_end(p,	ret ? "Failed" : "Success");
	if (!ret)
		pr_debug("Finished restoring pasid 0x%x\n", p->pasid);
	else
		pr_err("Failed to restore queues of pasid 0x%x\n", p->pasid);
}

void kfd_suspend_all_processes(bool force)
{
	struct kfd_process *p;
	unsigned int temp;
	int idx = srcu_read_lock(&kfd_processes_srcu);

	WARN(debug_evictions, "Evicting all processes");
	hash_for_each_rcu(kfd_processes_table, temp, p, kfd_processes) {
		cancel_delayed_work_sync(&p->eviction_work);
		cancel_delayed_work_sync(&p->restore_work);

		if (kfd_process_evict_queues(p, force))
			pr_err("Failed to suspend process 0x%x\n", p->pasid);
		dma_fence_signal(p->ef);
		dma_fence_put(p->ef);
		p->ef = NULL;
	}
	srcu_read_unlock(&kfd_processes_srcu, idx);
}

int kfd_resume_all_processes(bool sync)
{
	struct kfd_process *p;
	unsigned int temp;
	int ret = 0, idx = srcu_read_lock(&kfd_processes_srcu);

	hash_for_each_rcu(kfd_processes_table, temp, p, kfd_processes) {
		if (!queue_delayed_work(kfd_restore_wq, &p->restore_work, 0)) {
			pr_err("Restore process %d failed during resume\n",
			       p->pasid);
			ret = -EFAULT;
		}
		/*
		 * When there are multiple calls to kfd_suspend_all_processes()
		 * and kfd_resume_all_processes(), we need to wait for the
		 * delayed sync work for kfd_resume_all_processes() to complete
		 * or else the subsequent call to kfd_suspend_all_processes()
		 * may cancel any outstanding delayed work.  This can happen
		 * when the kfd debugger is started on a multi-gpu system.
		 */
		if (sync)
			flush_delayed_work(&p->restore_work);
	}
	srcu_read_unlock(&kfd_processes_srcu, idx);
	return ret;
}

int kfd_reserved_mem_mmap(struct kfd_dev *dev, struct kfd_process *process,
			  struct vm_area_struct *vma)
{
	struct kfd_process_device *pdd;
	struct qcm_process_device *qpd;

	if ((vma->vm_end - vma->vm_start) != KFD_CWSR_TBA_TMA_SIZE) {
		pr_err("Incorrect CWSR mapping size.\n");
		return -EINVAL;
	}

	pdd = kfd_get_process_device_data(dev, process);
	if (!pdd)
		return -EINVAL;
	qpd = &pdd->qpd;

	qpd->cwsr_kaddr = (void *)__get_free_pages(GFP_KERNEL | __GFP_ZERO,
					get_order(KFD_CWSR_TBA_TMA_SIZE));
	if (!qpd->cwsr_kaddr) {
		pr_err("Error allocating per process CWSR buffer.\n");
		return -ENOMEM;
	}

	vma->vm_flags |= VM_IO | VM_DONTCOPY | VM_DONTEXPAND
		| VM_NORESERVE | VM_DONTDUMP | VM_PFNMAP;
	/* Mapping pages to user process */
	return remap_pfn_range(vma, vma->vm_start,
			       PFN_DOWN(__pa(qpd->cwsr_kaddr)),
			       KFD_CWSR_TBA_TMA_SIZE, vma->vm_page_prot);
}

void kfd_flush_tlb(struct kfd_process_device *pdd, enum TLB_FLUSH_TYPE type)
{
	struct kfd_dev *dev = pdd->dev;

	if (dev->dqm->sched_policy == KFD_SCHED_POLICY_NO_HWS) {
		/* Nothing to flush until a VMID is assigned, which
		 * only happens when the first queue is created.
		 */
		if (pdd->qpd.vmid)
			amdgpu_amdkfd_flush_gpu_tlb_vmid(dev->adev,
							pdd->qpd.vmid);
	} else {
		amdgpu_amdkfd_flush_gpu_tlb_pasid(dev->adev,
					pdd->process->pasid, type);
	}
}

/* assumes caller holds process lock. */
void kfd_process_drain_interrupts(struct kfd_process_device *pdd)
{
	uint32_t irq_drain_fence[8];

	if (!KFD_IS_SOC15(pdd->dev))
		return;

	pdd->process->irq_drain_is_open = true;

	memset(irq_drain_fence, 0, sizeof(irq_drain_fence));
	irq_drain_fence[0] = (KFD_IRQ_FENCE_SOURCEID << 8) |
							KFD_IRQ_FENCE_CLIENTID;
	irq_drain_fence[3] = pdd->process->pasid;

	/* ensure stale irqs scheduled KFD interrupts and send drain fence. */
	if (amdgpu_amdkfd_send_close_event_drain_irq(pdd->dev->adev,
							irq_drain_fence)) {
		pdd->process->irq_drain_is_open = false;
		return;
	}

	if (wait_event_interruptible(pdd->process->wait_irq_drain,
				!READ_ONCE(pdd->process->irq_drain_is_open)))
		pdd->process->irq_drain_is_open = false;
}

void kfd_process_close_interrupt_drain(unsigned int pasid)
{
	struct kfd_process *p;

	p = kfd_lookup_process_by_pasid(pasid);

	if (!p)
		return;

	WRITE_ONCE(p->irq_drain_is_open, false);
	wake_up_all(&p->wait_irq_drain);
	kfd_unref_process(p);
}

struct send_exception_work_handler_workarea {
	struct work_struct work;
	struct kfd_process *p;
	unsigned int queue_id;
	uint64_t error_reason;
};

static void send_exception_work_handler(struct work_struct *work)
{
	struct send_exception_work_handler_workarea *workarea;
	struct kfd_process *p;
	struct queue *q;
	struct mm_struct *mm;
	void __user *csa_addr;
	size_t header_offset;
	uint64_t **err_payload_ptr_addr, *err_payload_ptr;
	uint64_t payload_offset;
	uint64_t cur_err;
	uint32_t ev_id;

	workarea = container_of(work,
				struct send_exception_work_handler_workarea,
				work);
	p = workarea->p;

	mm = get_task_mm(p->lead_thread);

	if (!mm)
		return;

	kthread_use_mm(mm);

	q = pqm_get_user_queue(&p->pqm, workarea->queue_id);

	if (!q)
		goto out;

	csa_addr = (void __user *) q->properties.ctx_save_restore_area_address;
	/* KFD header is 4 DWORDS in size for control stack info. */
	header_offset = sizeof(struct mqd_user_context_save_area_header);
	/*
	 * User header payload_offset is 6 DWORDS down in the header after
	 * debugger memory info.
	 */
	payload_offset = (uint64_t)(csa_addr + header_offset) +
						(2 * sizeof(uint32_t));

	err_payload_ptr_addr = (uint64_t **)payload_offset;
	get_user(err_payload_ptr, err_payload_ptr_addr);
	get_user(cur_err, err_payload_ptr);
	cur_err |= workarea->error_reason;
	put_user(cur_err, err_payload_ptr);
	get_user(ev_id, (uint64_t *)(payload_offset + sizeof(err_payload_ptr)));

	kfd_set_event(p, ev_id);

out:
	kthread_unuse_mm(mm);
	mmput(mm);
}

int kfd_send_exception_to_runtime(struct kfd_process *p,
			unsigned int queue_id,
			uint64_t error_reason)
{
	struct send_exception_work_handler_workarea worker;

	INIT_WORK_ONSTACK(&worker.work, send_exception_work_handler);

	worker.p = p;
	worker.queue_id = queue_id;
	worker.error_reason = error_reason;

	schedule_work(&worker.work);
	flush_work(&worker.work);
	destroy_work_on_stack(&worker.work);

	return 0;
}

struct kfd_process_device *kfd_process_device_data_by_id(struct kfd_process *p, uint32_t gpu_id)
{
	int i;

	if (gpu_id) {
		for (i = 0; i < p->n_pdds; i++) {
			struct kfd_process_device *pdd = p->pdds[i];

			if (pdd->user_gpu_id == gpu_id)
				return pdd;
		}
	}
	return NULL;
}

int kfd_process_get_user_gpu_id(struct kfd_process *p, uint32_t actual_gpu_id)
{
	int i;

	if (!actual_gpu_id)
		return 0;

	for (i = 0; i < p->n_pdds; i++) {
		struct kfd_process_device *pdd = p->pdds[i];

		if (pdd->dev->id == actual_gpu_id)
			return pdd->user_gpu_id;
	}
	return -EINVAL;
}

#if defined(CONFIG_DEBUG_FS)

int kfd_debugfs_mqds_by_process(struct seq_file *m, void *data)
{
	struct kfd_process *p;
	unsigned int temp;
	int r = 0;

	int idx = srcu_read_lock(&kfd_processes_srcu);

	hash_for_each_rcu(kfd_processes_table, temp, p, kfd_processes) {
		seq_printf(m, "Process %d PASID 0x%x:\n",
			   p->lead_thread->tgid, p->pasid);

		mutex_lock(&p->mutex);
		r = pqm_debugfs_mqds(m, &p->pqm);
		mutex_unlock(&p->mutex);

		if (r)
			break;
	}

	srcu_read_unlock(&kfd_processes_srcu, idx);

	return r;
}

#endif

