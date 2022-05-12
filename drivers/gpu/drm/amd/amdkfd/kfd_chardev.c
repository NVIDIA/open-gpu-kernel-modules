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

#include <linux/device.h>
#include <linux/export.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/compat.h>
#include <uapi/linux/kfd_ioctl.h>
#include <linux/time.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/ptrace.h>
#include <linux/dma-buf.h>
#include <linux/fdtable.h>
#include <linux/processor.h>
#include <linux/ptrace.h>
#include "kfd_priv.h"
#include "kfd_device_queue_manager.h"
#include "kfd_svm.h"
#include "kfd_debug.h"
#include "kfd_ipc.h"
#include "kfd_trace.h"

#include "amdgpu_amdkfd.h"
#include "kfd_smi_events.h"
#include "amdgpu.h"
#include "amdgpu_object.h"
#include "amdgpu_dma_buf.h"

static long kfd_ioctl(struct file *, unsigned int, unsigned long);
static int kfd_open(struct inode *, struct file *);
static int kfd_release(struct inode *, struct file *);
static int kfd_mmap(struct file *, struct vm_area_struct *);

static const char kfd_dev_name[] = "kfd";

static const struct file_operations kfd_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = kfd_ioctl,
	.compat_ioctl = compat_ptr_ioctl,
	.open = kfd_open,
	.release = kfd_release,
	.mmap = kfd_mmap,
};

static int kfd_char_dev_major = -1;
static struct class *kfd_class;
struct device *kfd_device;

int kfd_chardev_init(void)
{
	int err = 0;

	kfd_char_dev_major = register_chrdev(0, kfd_dev_name, &kfd_fops);
	err = kfd_char_dev_major;
	if (err < 0)
		goto err_register_chrdev;

	kfd_class = class_create(THIS_MODULE, kfd_dev_name);
	err = PTR_ERR(kfd_class);
	if (IS_ERR(kfd_class))
		goto err_class_create;

	kfd_device = device_create(kfd_class, NULL,
					MKDEV(kfd_char_dev_major, 0),
					NULL, kfd_dev_name);
	err = PTR_ERR(kfd_device);
	if (IS_ERR(kfd_device))
		goto err_device_create;

	return 0;

err_device_create:
	class_destroy(kfd_class);
err_class_create:
	unregister_chrdev(kfd_char_dev_major, kfd_dev_name);
err_register_chrdev:
	return err;
}

void kfd_chardev_exit(void)
{
	device_destroy(kfd_class, MKDEV(kfd_char_dev_major, 0));
	class_destroy(kfd_class);
	unregister_chrdev(kfd_char_dev_major, kfd_dev_name);
	kfd_device = NULL;
}

struct device *kfd_chardev(void)
{
	return kfd_device;
}


static int kfd_open(struct inode *inode, struct file *filep)
{
	struct kfd_process *process;
	bool is_32bit_user_mode;

	if (iminor(inode) != 0)
		return -ENODEV;

	is_32bit_user_mode = in_compat_syscall();

	if (is_32bit_user_mode) {
		dev_warn(kfd_device,
			"Process %d (32-bit) failed to open /dev/kfd\n"
			"32-bit processes are not supported by amdkfd\n",
			current->pid);
		return -EPERM;
	}

	process = kfd_create_process(current);
	if (IS_ERR(process))
		return PTR_ERR(process);

	if (kfd_process_init_cwsr_apu(process, filep)) {
		kfd_unref_process(process);
		return -EFAULT;
	}

	if (kfd_is_locked()) {
		dev_dbg(kfd_device, "kfd is locked!\n"
				"process %d unreferenced", process->pasid);
		kfd_unref_process(process);
		return -EAGAIN;
	}

	/* filep now owns the reference returned by kfd_create_process */
	filep->private_data = process;

	dev_dbg(kfd_device, "process %d opened, compat mode (32 bit) - %d\n",
		process->pasid, process->is_32bit_user_mode);

	return 0;
}

static int kfd_release(struct inode *inode, struct file *filep)
{
	struct kfd_process *process = filep->private_data;

	if (process)
		kfd_unref_process(process);

	return 0;
}

static int kfd_ioctl_get_version(struct file *filep, struct kfd_process *p,
					void *data)
{
	struct kfd_ioctl_get_version_args *args = data;

	args->major_version = KFD_IOCTL_MAJOR_VERSION;
	args->minor_version = KFD_IOCTL_MINOR_VERSION;

	return 0;
}

static int set_queue_properties_from_user(struct queue_properties *q_properties,
				struct kfd_ioctl_create_queue_args *args)
{
	if (args->queue_percentage > KFD_MAX_QUEUE_PERCENTAGE) {
		pr_err("Queue percentage must be between 0 to KFD_MAX_QUEUE_PERCENTAGE\n");
		return -EINVAL;
	}

	if (args->queue_priority > KFD_MAX_QUEUE_PRIORITY) {
		pr_err("Queue priority must be between 0 to KFD_MAX_QUEUE_PRIORITY\n");
		return -EINVAL;
	}

	if ((args->ring_base_address) &&
		(!kcl_access_ok((const void __user *) args->ring_base_address,
			sizeof(uint64_t)))) {
		pr_err("Can't access ring base address\n");
		return -EFAULT;
	}

	if (!is_power_of_2(args->ring_size) && (args->ring_size != 0)) {
		pr_err("Ring size must be a power of 2 or 0\n");
		return -EINVAL;
	}

	if (!kcl_access_ok((const void __user *) args->read_pointer_address,
			sizeof(uint32_t))) {
		pr_err("Can't access read pointer\n");
		return -EFAULT;
	}

	if (!kcl_access_ok((const void __user *) args->write_pointer_address,
			sizeof(uint32_t))) {
		pr_err("Can't access write pointer\n");
		return -EFAULT;
	}

	if (args->eop_buffer_address &&
		!kcl_access_ok((const void __user *) args->eop_buffer_address,
			sizeof(uint32_t))) {
		pr_debug("Can't access eop buffer");
		return -EFAULT;
	}

	if (args->ctx_save_restore_address &&
		!kcl_access_ok((const void __user *) args->ctx_save_restore_address,
			sizeof(uint32_t))) {
		pr_debug("Can't access ctx save restore buffer");
		return -EFAULT;
	}

	q_properties->is_interop = false;
	q_properties->is_gws = false;
	q_properties->queue_percent = args->queue_percentage;
	q_properties->priority = args->queue_priority;
	q_properties->queue_address = args->ring_base_address;
	q_properties->queue_size = args->ring_size;
	q_properties->read_ptr = (uint32_t *) args->read_pointer_address;
	q_properties->write_ptr = (uint32_t *) args->write_pointer_address;
	q_properties->eop_ring_buffer_address = args->eop_buffer_address;
	q_properties->eop_ring_buffer_size = args->eop_buffer_size;
	q_properties->ctx_save_restore_area_address =
			args->ctx_save_restore_address;
	q_properties->ctx_save_restore_area_size = args->ctx_save_restore_size;
	q_properties->ctl_stack_size = args->ctl_stack_size;
	if (args->queue_type == KFD_IOC_QUEUE_TYPE_COMPUTE ||
		args->queue_type == KFD_IOC_QUEUE_TYPE_COMPUTE_AQL)
		q_properties->type = KFD_QUEUE_TYPE_COMPUTE;
	else if (args->queue_type == KFD_IOC_QUEUE_TYPE_SDMA)
		q_properties->type = KFD_QUEUE_TYPE_SDMA;
	else if (args->queue_type == KFD_IOC_QUEUE_TYPE_SDMA_XGMI)
		q_properties->type = KFD_QUEUE_TYPE_SDMA_XGMI;
	else
		return -ENOTSUPP;

	if (args->queue_type == KFD_IOC_QUEUE_TYPE_COMPUTE_AQL)
		q_properties->format = KFD_QUEUE_FORMAT_AQL;
	else
		q_properties->format = KFD_QUEUE_FORMAT_PM4;

	pr_debug("Queue Percentage: %d, %d\n",
			q_properties->queue_percent, args->queue_percentage);

	pr_debug("Queue Priority: %d, %d\n",
			q_properties->priority, args->queue_priority);

	pr_debug("Queue Address: 0x%llX, 0x%llX\n",
			q_properties->queue_address, args->ring_base_address);

	pr_debug("Queue Size: 0x%llX, %u\n",
			q_properties->queue_size, args->ring_size);

	pr_debug("Queue r/w Pointers: %px, %px\n",
			q_properties->read_ptr,
			q_properties->write_ptr);

	pr_debug("Queue Format: %d\n", q_properties->format);

	pr_debug("Queue EOP: 0x%llX\n", q_properties->eop_ring_buffer_address);

	pr_debug("Queue CTX save area: 0x%llX\n",
			q_properties->ctx_save_restore_area_address);

	return 0;
}

static int kfd_ioctl_create_queue(struct file *filep, struct kfd_process *p,
					void *data)
{
	struct kfd_ioctl_create_queue_args *args = data;
	struct kfd_dev *dev;
	int err = 0;
	unsigned int queue_id;
	struct kfd_process_device *pdd;
	struct queue_properties q_properties;
	uint32_t doorbell_offset_in_process = 0;

	memset(&q_properties, 0, sizeof(struct queue_properties));

	pr_debug("Creating queue ioctl\n");

	err = set_queue_properties_from_user(&q_properties, args);
	if (err)
		return err;

	pr_debug("Looking for gpu id 0x%x\n", args->gpu_id);

	mutex_lock(&p->mutex);

	pdd = kfd_process_device_data_by_id(p, args->gpu_id);
	if (!pdd) {
		pr_debug("Could not find gpu id 0x%x\n", args->gpu_id);
		err = -EINVAL;
		goto err_pdd;
	}
	dev = pdd->dev;

	err = amdgpu_read_lock(dev->ddev, true);
	if (err)
		goto err_read_lock;

	pdd = kfd_bind_process_to_device(dev, p);
	if (IS_ERR(pdd)) {
		err = -ESRCH;
		goto err_pdd_bind;
	}

	pr_debug("Creating queue for PASID 0x%x on gpu 0x%x\n",
			p->pasid,
			dev->id);

	err = pqm_create_queue(&p->pqm, dev, filep, &q_properties, &queue_id, NULL, NULL, NULL,
			&doorbell_offset_in_process);
	if (err != 0)
		goto err_create_queue;

	args->queue_id = queue_id;


	/* Return gpu_id as doorbell offset for mmap usage */
	args->doorbell_offset = KFD_MMAP_TYPE_DOORBELL;
	args->doorbell_offset |= KFD_MMAP_GPU_ID(args->gpu_id);
	if (KFD_IS_SOC15(dev))
		/* On SOC15 ASICs, include the doorbell offset within the
		 * process doorbell frame, which is 2 pages.
		 */
		args->doorbell_offset |= doorbell_offset_in_process;

	amdgpu_read_unlock(dev->ddev);
	mutex_unlock(&p->mutex);

	pr_debug("Queue id %d was created successfully\n", args->queue_id);

	pr_debug("Ring buffer address == 0x%016llX\n",
			args->ring_base_address);

	pr_debug("Read ptr address    == 0x%016llX\n",
			args->read_pointer_address);

	pr_debug("Write ptr address   == 0x%016llX\n",
			args->write_pointer_address);

	kfd_dbg_ev_raise(KFD_EC_MASK(EC_QUEUE_NEW), p, dev, queue_id, false, NULL, 0);
	return 0;

err_create_queue:
err_pdd_bind:
	amdgpu_read_unlock(dev->ddev);
err_pdd:
err_read_lock:
	mutex_unlock(&p->mutex);
	return err;
}

static int kfd_ioctl_destroy_queue(struct file *filp, struct kfd_process *p,
					void *data)
{
	int retval;
	struct kfd_ioctl_destroy_queue_args *args = data;
	struct kfd_dev *dev;

	pr_debug("Destroying queue id %d for pasid 0x%x\n",
				args->queue_id,
				p->pasid);

	mutex_lock(&p->mutex);

	dev = pqm_query_dev_by_qid(&p->pqm, args->queue_id);
	if (!dev) {
		retval = -EINVAL;
		goto err_query_dev;
	}

	retval = amdgpu_read_lock(dev->ddev, true);
	if (retval)
		goto err_read_lock;
	retval = pqm_destroy_queue(&p->pqm, args->queue_id);
	amdgpu_read_unlock(dev->ddev);

err_read_lock:
err_query_dev:
	mutex_unlock(&p->mutex);
	return retval;
}

static int kfd_ioctl_update_queue(struct file *filp, struct kfd_process *p,
					void *data)
{
	int retval;
	struct kfd_ioctl_update_queue_args *args = data;
	struct queue_properties properties;
	struct kfd_dev *dev;

	if (args->queue_percentage > KFD_MAX_QUEUE_PERCENTAGE) {
		pr_err("Queue percentage must be between 0 to KFD_MAX_QUEUE_PERCENTAGE\n");
		return -EINVAL;
	}

	if (args->queue_priority > KFD_MAX_QUEUE_PRIORITY) {
		pr_err("Queue priority must be between 0 to KFD_MAX_QUEUE_PRIORITY\n");
		return -EINVAL;
	}

	if ((args->ring_base_address) &&
		(!kcl_access_ok((const void __user *) args->ring_base_address,
			sizeof(uint64_t)))) {
		pr_err("Can't access ring base address\n");
		return -EFAULT;
	}

	if (!is_power_of_2(args->ring_size) && (args->ring_size != 0)) {
		pr_err("Ring size must be a power of 2 or 0\n");
		return -EINVAL;
	}

	properties.queue_address = args->ring_base_address;
	properties.queue_size = args->ring_size;
	properties.queue_percent = args->queue_percentage;
	properties.priority = args->queue_priority;

	pr_debug("Updating queue id %d for pasid 0x%x\n",
			args->queue_id, p->pasid);

	mutex_lock(&p->mutex);

	dev = pqm_query_dev_by_qid(&p->pqm, args->queue_id);
	if (!dev) {
		retval = -EINVAL;
		goto err_query_dev;
	}

	retval = amdgpu_read_lock(dev->ddev, true);
	if (retval)
		goto err_read_lock;
	retval = pqm_update_queue_properties(&p->pqm, args->queue_id, &properties);
	amdgpu_read_unlock(dev->ddev);

err_read_lock:
err_query_dev:
	mutex_unlock(&p->mutex);
	return retval;
}

static int kfd_ioctl_set_cu_mask(struct file *filp, struct kfd_process *p,
					void *data)
{
	int retval;
	const int max_num_cus = 1024;
	struct kfd_ioctl_set_cu_mask_args *args = data;
	struct mqd_update_info minfo = {0};
	uint32_t __user *cu_mask_ptr = (uint32_t __user *)args->cu_mask_ptr;
	size_t cu_mask_size = sizeof(uint32_t) * (args->num_cu_mask / 32);
	struct kfd_dev *dev;

	if ((args->num_cu_mask % 32) != 0) {
		pr_debug("num_cu_mask 0x%x must be a multiple of 32",
				args->num_cu_mask);
		return -EINVAL;
	}

	minfo.cu_mask.count = args->num_cu_mask;
	if (minfo.cu_mask.count == 0) {
		pr_debug("CU mask cannot be 0");
		return -EINVAL;
	}

	/* To prevent an unreasonably large CU mask size, set an arbitrary
	 * limit of max_num_cus bits.  We can then just drop any CU mask bits
	 * past max_num_cus bits and just use the first max_num_cus bits.
	 */
	if (minfo.cu_mask.count > max_num_cus) {
		pr_debug("CU mask cannot be greater than 1024 bits");
		minfo.cu_mask.count = max_num_cus;
		cu_mask_size = sizeof(uint32_t) * (max_num_cus/32);
	}

	minfo.cu_mask.ptr = kzalloc(cu_mask_size, GFP_KERNEL);
	if (!minfo.cu_mask.ptr)
		return -ENOMEM;

	retval = copy_from_user(minfo.cu_mask.ptr, cu_mask_ptr, cu_mask_size);
	if (retval) {
		pr_debug("Could not copy CU mask from userspace");
		retval = -EFAULT;
		goto out;
	}

	minfo.update_flag = UPDATE_FLAG_CU_MASK;

	mutex_lock(&p->mutex);

	dev = pqm_query_dev_by_qid(&p->pqm, args->queue_id);
	if (!dev) {
		retval = -EINVAL;
		goto err_query_dev;
	}

	retval = amdgpu_read_lock(dev->ddev, true);
	if (retval)
		goto err_read_lock;
	retval = pqm_update_mqd(&p->pqm, args->queue_id, &minfo);
	amdgpu_read_unlock(dev->ddev);

err_read_lock:
err_query_dev:
	mutex_unlock(&p->mutex);

out:
	kfree(minfo.cu_mask.ptr);
	return retval;
}

static int kfd_ioctl_get_queue_wave_state(struct file *filep,
					  struct kfd_process *p, void *data)
{
	struct kfd_ioctl_get_queue_wave_state_args *args = data;
	int r;
	struct kfd_dev *dev;

	mutex_lock(&p->mutex);

	dev = pqm_query_dev_by_qid(&p->pqm, args->queue_id);
	if (!dev) {
		r = -EINVAL;
		goto err_query_dev;
	}

	r = amdgpu_read_lock(dev->ddev, true);
	if (r)
		goto err_read_lock;
	r = pqm_get_wave_state(&p->pqm, args->queue_id,
			       (void __user *)args->ctl_stack_address,
			       &args->ctl_stack_used_size,
			       &args->save_area_used_size);
	amdgpu_read_unlock(dev->ddev);

err_read_lock:
err_query_dev:
	mutex_unlock(&p->mutex);

	return r;
}

static int kfd_ioctl_set_memory_policy(struct file *filep,
					struct kfd_process *p, void *data)
{
	struct kfd_ioctl_set_memory_policy_args *args = data;
	int err = 0;
	struct kfd_process_device *pdd;
	enum cache_policy default_policy, alternate_policy;

	if (args->default_policy != KFD_IOC_CACHE_POLICY_COHERENT
	    && args->default_policy != KFD_IOC_CACHE_POLICY_NONCOHERENT) {
		return -EINVAL;
	}

	if (args->alternate_policy != KFD_IOC_CACHE_POLICY_COHERENT
	    && args->alternate_policy != KFD_IOC_CACHE_POLICY_NONCOHERENT) {
		return -EINVAL;
	}

	mutex_lock(&p->mutex);
	pdd = kfd_process_device_data_by_id(p, args->gpu_id);
	if (!pdd) {
		pr_debug("Could not find gpu id 0x%x\n", args->gpu_id);
		err = -EINVAL;
		goto err_pdd;
	}

	err = amdgpu_read_lock(pdd->dev->ddev, true);
	if (err)
		goto err_read_lock;

	pdd = kfd_bind_process_to_device(pdd->dev, p);
	if (IS_ERR(pdd)) {
		err = -ESRCH;
		goto out;
	}

	default_policy = (args->default_policy == KFD_IOC_CACHE_POLICY_COHERENT)
			 ? cache_policy_coherent : cache_policy_noncoherent;

	alternate_policy =
		(args->alternate_policy == KFD_IOC_CACHE_POLICY_COHERENT)
		   ? cache_policy_coherent : cache_policy_noncoherent;

	if (!pdd->dev->dqm->ops.set_cache_memory_policy(pdd->dev->dqm,
				&pdd->qpd,
				default_policy,
				alternate_policy,
				(void __user *)args->alternate_aperture_base,
				args->alternate_aperture_size))
		err = -EINVAL;

out:
	amdgpu_read_unlock(pdd->dev->ddev);

err_read_lock:
err_pdd:
	mutex_unlock(&p->mutex);

	return err;
}

static int kfd_ioctl_set_trap_handler(struct file *filep,
					struct kfd_process *p, void *data)
{
	struct kfd_ioctl_set_trap_handler_args *args = data;
	int err = 0;
	struct kfd_process_device *pdd;

	mutex_lock(&p->mutex);

	pdd = kfd_process_device_data_by_id(p, args->gpu_id);
	if (!pdd) {
		err = -EINVAL;
		goto err_pdd;
	}

	err = amdgpu_read_lock(pdd->dev->ddev, true);
	if (err)
		goto err_read_lock;

	pdd = kfd_bind_process_to_device(pdd->dev, p);
	if (IS_ERR(pdd)) {
		err = -ESRCH;
		goto out;
	}

	kfd_process_set_trap_handler(&pdd->qpd, args->tba_addr, args->tma_addr);

out:
	amdgpu_read_unlock(pdd->dev->ddev);
err_read_lock:
err_pdd:
	mutex_unlock(&p->mutex);

	return err;
}

static int kfd_ioctl_dbg_register(struct file *filep,
				struct kfd_process *p, void *data)
{
	return -EPERM;
}

static int kfd_ioctl_dbg_unregister(struct file *filep,
				struct kfd_process *p, void *data)
{
	return -EPERM;
}

static int kfd_ioctl_dbg_address_watch(struct file *filep,
					struct kfd_process *p, void *data)
{
	return -EPERM;
}

/* Parse and generate fixed size data structure for wave control */
static int kfd_ioctl_dbg_wave_control(struct file *filep,
					struct kfd_process *p, void *data)
{
	return -EPERM;
}

static int kfd_ioctl_get_clock_counters(struct file *filep,
				struct kfd_process *p, void *data)
{
	struct kfd_ioctl_get_clock_counters_args *args = data;
	struct kfd_process_device *pdd;

	mutex_lock(&p->mutex);
	pdd = kfd_process_device_data_by_id(p, args->gpu_id);
	mutex_unlock(&p->mutex);
	if (pdd && !amdgpu_read_lock(pdd->dev->ddev, true)) {
		/* Reading GPU clock counter from KGD */
		args->gpu_clock_counter = amdgpu_amdkfd_get_gpu_clock_counter(pdd->dev->adev);
		amdgpu_read_unlock(pdd->dev->ddev);
	} else {
		/* Node without GPU resource */
		args->gpu_clock_counter = 0;
	}

	/* No access to rdtsc. Using raw monotonic time */
	args->cpu_clock_counter = ktime_get_raw_ns();
	args->system_clock_counter = ktime_get_boottime_ns();

	/* Since the counter is in nano-seconds we use 1GHz frequency */
	args->system_clock_freq = 1000000000;

	return 0;
}


static int kfd_ioctl_get_process_apertures(struct file *filp,
				struct kfd_process *p, void *data)
{
	struct kfd_ioctl_get_process_apertures_args *args = data;
	struct kfd_process_device_apertures *pAperture;
	int i;

	dev_dbg(kfd_device, "get apertures for PASID 0x%x", p->pasid);

	args->num_of_nodes = 0;

	mutex_lock(&p->mutex);
	/* Run over all pdd of the process */
	for (i = 0; i < p->n_pdds; i++) {
		struct kfd_process_device *pdd = p->pdds[i];

		pAperture =
			&args->process_apertures[args->num_of_nodes];
		pAperture->gpu_id = pdd->dev->id;
		pAperture->lds_base = pdd->lds_base;
		pAperture->lds_limit = pdd->lds_limit;
		pAperture->gpuvm_base = pdd->gpuvm_base;
		pAperture->gpuvm_limit = pdd->gpuvm_limit;
		pAperture->scratch_base = pdd->scratch_base;
		pAperture->scratch_limit = pdd->scratch_limit;

		dev_dbg(kfd_device,
			"node id %u\n", args->num_of_nodes);
		dev_dbg(kfd_device,
			"gpu id %u\n", pdd->dev->id);
		dev_dbg(kfd_device,
			"lds_base %llX\n", pdd->lds_base);
		dev_dbg(kfd_device,
			"lds_limit %llX\n", pdd->lds_limit);
		dev_dbg(kfd_device,
			"gpuvm_base %llX\n", pdd->gpuvm_base);
		dev_dbg(kfd_device,
			"gpuvm_limit %llX\n", pdd->gpuvm_limit);
		dev_dbg(kfd_device,
			"scratch_base %llX\n", pdd->scratch_base);
		dev_dbg(kfd_device,
			"scratch_limit %llX\n", pdd->scratch_limit);

		if (++args->num_of_nodes >= NUM_OF_SUPPORTED_GPUS)
			break;
	}
	mutex_unlock(&p->mutex);

	return 0;
}

static int kfd_ioctl_get_process_apertures_new(struct file *filp,
				struct kfd_process *p, void *data)
{
	struct kfd_ioctl_get_process_apertures_new_args *args = data;
	struct kfd_process_device_apertures *pa;
	int ret;
	int i;

	dev_dbg(kfd_device, "get apertures for PASID 0x%x", p->pasid);

	if (args->num_of_nodes == 0) {
		/* Return number of nodes, so that user space can alloacate
		 * sufficient memory
		 */
		mutex_lock(&p->mutex);
		args->num_of_nodes = p->n_pdds;
		goto out_unlock;
	}

	/* Fill in process-aperture information for all available
	 * nodes, but not more than args->num_of_nodes as that is
	 * the amount of memory allocated by user
	 */
	pa = kzalloc((sizeof(struct kfd_process_device_apertures) *
				args->num_of_nodes), GFP_KERNEL);
	if (!pa)
		return -ENOMEM;

	mutex_lock(&p->mutex);

	if (!p->n_pdds) {
		args->num_of_nodes = 0;
		kfree(pa);
		goto out_unlock;
	}

	/* Run over all pdd of the process */
	for (i = 0; i < min(p->n_pdds, args->num_of_nodes); i++) {
		struct kfd_process_device *pdd = p->pdds[i];

		pa[i].gpu_id = pdd->dev->id;
		pa[i].lds_base = pdd->lds_base;
		pa[i].lds_limit = pdd->lds_limit;
		pa[i].gpuvm_base = pdd->gpuvm_base;
		pa[i].gpuvm_limit = pdd->gpuvm_limit;
		pa[i].scratch_base = pdd->scratch_base;
		pa[i].scratch_limit = pdd->scratch_limit;

		dev_dbg(kfd_device,
			"gpu id %u\n", pdd->dev->id);
		dev_dbg(kfd_device,
			"lds_base %llX\n", pdd->lds_base);
		dev_dbg(kfd_device,
			"lds_limit %llX\n", pdd->lds_limit);
		dev_dbg(kfd_device,
			"gpuvm_base %llX\n", pdd->gpuvm_base);
		dev_dbg(kfd_device,
			"gpuvm_limit %llX\n", pdd->gpuvm_limit);
		dev_dbg(kfd_device,
			"scratch_base %llX\n", pdd->scratch_base);
		dev_dbg(kfd_device,
			"scratch_limit %llX\n", pdd->scratch_limit);
	}
	mutex_unlock(&p->mutex);

	args->num_of_nodes = i;
	ret = copy_to_user(
			(void __user *)args->kfd_process_device_apertures_ptr,
			pa,
			(i * sizeof(struct kfd_process_device_apertures)));
	kfree(pa);
	return ret ? -EFAULT : 0;

out_unlock:
	mutex_unlock(&p->mutex);
	return 0;
}

static int kfd_ioctl_create_event(struct file *filp, struct kfd_process *p,
					void *data)
{
	struct kfd_ioctl_create_event_args *args = data;
	int err;

	/* For dGPUs the event page is allocated in user mode. The
	 * handle is passed to KFD with the first call to this IOCTL
	 * through the event_page_offset field.
	 */
	if (args->event_page_offset) {
		mutex_lock(&p->mutex);
		err = kfd_kmap_event_page(p, args->event_page_offset);
		mutex_unlock(&p->mutex);
		if (err)
			return err;
	}

	err = kfd_event_create(filp, p, args->event_type,
				args->auto_reset != 0, args->node_id,
				&args->event_id, &args->event_trigger_data,
				&args->event_page_offset,
				&args->event_slot_index);

	pr_debug("Created event (id:0x%08x) (%s)\n", args->event_id, __func__);
	return err;
}

static int kfd_ioctl_destroy_event(struct file *filp, struct kfd_process *p,
					void *data)
{
	struct kfd_ioctl_destroy_event_args *args = data;

	return kfd_event_destroy(p, args->event_id);
}

static int kfd_ioctl_set_event(struct file *filp, struct kfd_process *p,
				void *data)
{
	struct kfd_ioctl_set_event_args *args = data;

	return kfd_set_event(p, args->event_id);
}

static int kfd_ioctl_reset_event(struct file *filp, struct kfd_process *p,
				void *data)
{
	struct kfd_ioctl_reset_event_args *args = data;

	return kfd_reset_event(p, args->event_id);
}

static int kfd_ioctl_wait_events(struct file *filp, struct kfd_process *p,
				void *data)
{
	struct kfd_ioctl_wait_events_args *args = data;
	int err;

	err = kfd_wait_on_events(p, args->num_events,
			(void __user *)args->events_ptr,
			(args->wait_for_all != 0),
			args->timeout, &args->wait_result);

	return err;
}
static int kfd_ioctl_set_scratch_backing_va(struct file *filep,
					struct kfd_process *p, void *data)
{
	struct kfd_ioctl_set_scratch_backing_va_args *args = data;
	struct kfd_process_device *pdd;
	struct kfd_dev *dev;
	long err;

	mutex_lock(&p->mutex);
	pdd = kfd_process_device_data_by_id(p, args->gpu_id);
	if (!pdd) {
		err = -EINVAL;
		goto err_pdd;
	}
	dev = pdd->dev;

	pdd = kfd_bind_process_to_device(dev, p);
	if (IS_ERR(pdd)) {
		err = PTR_ERR(pdd);
		goto bind_process_to_device_fail;
	}

	pdd->qpd.sh_hidden_private_base = args->va_addr;

	mutex_unlock(&p->mutex);

	err = amdgpu_read_lock(dev->ddev, true);
	if (err)
		return err;

	if (dev->dqm->sched_policy == KFD_SCHED_POLICY_NO_HWS &&
	    pdd->qpd.vmid != 0 && dev->kfd2kgd->set_scratch_backing_va)
		dev->kfd2kgd->set_scratch_backing_va(
			dev->adev, args->va_addr, pdd->qpd.vmid);

	amdgpu_read_unlock(dev->ddev);

	return 0;

bind_process_to_device_fail:
err_pdd:
	mutex_unlock(&p->mutex);
	return err;
}

static int kfd_ioctl_get_tile_config(struct file *filep,
		struct kfd_process *p, void *data)
{
	struct kfd_ioctl_get_tile_config_args *args = data;
	struct kfd_process_device *pdd;
	struct tile_config config;
	int err = 0;

	mutex_lock(&p->mutex);
	pdd = kfd_process_device_data_by_id(p, args->gpu_id);
	mutex_unlock(&p->mutex);
	if (!pdd)
		return -EINVAL;

	amdgpu_amdkfd_get_tile_config(pdd->dev->adev, &config);

	args->gb_addr_config = config.gb_addr_config;
	args->num_banks = config.num_banks;
	args->num_ranks = config.num_ranks;

	if (args->num_tile_configs > config.num_tile_configs)
		args->num_tile_configs = config.num_tile_configs;
	err = copy_to_user((void __user *)args->tile_config_ptr,
			config.tile_config_ptr,
			args->num_tile_configs * sizeof(uint32_t));
	if (err) {
		args->num_tile_configs = 0;
		return -EFAULT;
	}

	if (args->num_macro_tile_configs > config.num_macro_tile_configs)
		args->num_macro_tile_configs =
				config.num_macro_tile_configs;
	err = copy_to_user((void __user *)args->macro_tile_config_ptr,
			config.macro_tile_config_ptr,
			args->num_macro_tile_configs * sizeof(uint32_t));
	if (err) {
		args->num_macro_tile_configs = 0;
		return -EFAULT;
	}

	return 0;
}

static int kfd_ioctl_acquire_vm(struct file *filep, struct kfd_process *p,
				void *data)
{
	struct kfd_ioctl_acquire_vm_args *args = data;
	struct kfd_process_device *pdd;
	struct file *drm_file;
	int ret;

	drm_file = fget(args->drm_fd);
	if (!drm_file)
		return -EINVAL;

	mutex_lock(&p->mutex);
	pdd = kfd_process_device_data_by_id(p, args->gpu_id);
	if (!pdd) {
		ret = -EINVAL;
		goto err_pdd;
	}

	if (pdd->drm_file) {
		ret = pdd->drm_file == drm_file ? 0 : -EBUSY;
		goto err_drm_file;
	}

	ret = amdgpu_read_lock(pdd->dev->ddev, true);
	if (ret)
		goto err_read_lock;

	ret = kfd_process_device_init_vm(pdd, drm_file);
	if (ret)
		goto err_unlock;

	amdgpu_read_unlock(pdd->dev->ddev);
	/* On success, the PDD keeps the drm_file reference */
	mutex_unlock(&p->mutex);

	return 0;

err_unlock:
	amdgpu_read_unlock(pdd->dev->ddev);
err_pdd:
err_drm_file:
err_read_lock:
	mutex_unlock(&p->mutex);
	fput(drm_file);
	return ret;
}

bool kfd_dev_is_large_bar(struct kfd_dev *dev)
{
	struct kfd_local_mem_info mem_info;

	if (debug_largebar) {
		pr_debug("Simulate large-bar allocation on non large-bar machine\n");
		return true;
	}

	if (dev->use_iommu_v2)
		return false;

	amdgpu_amdkfd_get_local_mem_info(dev->adev, &mem_info);
	if (mem_info.local_mem_size_private == 0 &&
			mem_info.local_mem_size_public > 0)
		return true;
	return false;
}

static int kfd_ioctl_alloc_memory_of_gpu(struct file *filep,
					struct kfd_process *p, void *data)
{
	struct kfd_ioctl_alloc_memory_of_gpu_args *args = data;
	struct kfd_process_device *pdd;
	void *mem;
	struct kfd_dev *dev;
	int idr_handle;
	long err;
	uint64_t offset = args->mmap_offset;
	uint32_t flags = args->flags;
	struct vm_area_struct *vma;
	uint64_t cpuva = 0;
	unsigned int mem_type = 0;

	if (args->size == 0)
		return -EINVAL;

#if IS_ENABLED(CONFIG_HSA_AMD_SVM)
	/* Flush pending deferred work to avoid racing with deferred actions
	 * from previous memory map changes (e.g. munmap).
	 */
	svm_range_list_lock_and_flush_work(&p->svms, current->mm);
	mutex_lock(&p->svms.lock);
	mmap_write_unlock(current->mm);
	if (interval_tree_iter_first(&p->svms.objects,
				     args->va_addr >> PAGE_SHIFT,
				     (args->va_addr + args->size - 1) >> PAGE_SHIFT)) {
		pr_err("Address: 0x%llx already allocated by SVM\n",
			args->va_addr);
		mutex_unlock(&p->svms.lock);
		return -EADDRINUSE;
	}
	mutex_unlock(&p->svms.lock);
#endif
	mutex_lock(&p->mutex);
	pdd = kfd_process_device_data_by_id(p, args->gpu_id);
	if (!pdd) {
		err = -EINVAL;
		goto err_pdd;
	}

	dev = pdd->dev;

	if ((flags & KFD_IOC_ALLOC_MEM_FLAGS_PUBLIC) &&
		(flags & KFD_IOC_ALLOC_MEM_FLAGS_VRAM) &&
		!kfd_dev_is_large_bar(dev)) {
		pr_err("Alloc host visible vram on small bar is not allowed\n");
		err = -EINVAL;
		goto err_large_bar;
	}

	err = amdgpu_read_lock(dev->ddev, true);
	if (err)
		goto err_read_lock;

	pdd = kfd_bind_process_to_device(dev, p);
	if (IS_ERR(pdd)) {
		err = PTR_ERR(pdd);
		goto err_unlock;
	}

	if (flags & KFD_IOC_ALLOC_MEM_FLAGS_USERPTR) {
		/* Check if the userptr corresponds to another (or third-party)
		 * device local memory. If so treat is as a doorbell. User
		 * space will be oblivious of this and will use this doorbell
		 * BO as a regular userptr BO
		 */
		vma = find_vma(current->mm, args->mmap_offset);
		if (vma && args->mmap_offset >= vma->vm_start &&
		    (vma->vm_flags & VM_IO)) {
			unsigned long pfn;

			err = follow_pfn(vma, args->mmap_offset, &pfn);
			if (err) {
				pr_debug("Failed to get PFN: %ld\n", err);
				goto err_unlock;
			}
			flags |= KFD_IOC_ALLOC_MEM_FLAGS_DOORBELL;
			flags &= ~KFD_IOC_ALLOC_MEM_FLAGS_USERPTR;
			offset = (pfn << PAGE_SHIFT);
		} else {
			if (offset & (PAGE_SIZE - 1)) {
				pr_debug("Unaligned userptr address:%llx\n",
					 offset);
				err = -EINVAL;
				goto err_unlock;
			}
			cpuva = offset;
		}
	} else if (flags & KFD_IOC_ALLOC_MEM_FLAGS_DOORBELL) {
		if (args->size != kfd_doorbell_process_slice(dev)) {
			err = -EINVAL;
			goto err_unlock;
		}
		offset = kfd_get_process_doorbells(pdd);
	} else if (flags & KFD_IOC_ALLOC_MEM_FLAGS_MMIO_REMAP) {
		if (args->size != PAGE_SIZE) {
			err = -EINVAL;
			goto err_unlock;
		}
		offset = dev->adev->rmmio_remap.bus_addr;
		if (!offset) {
			err = -ENOMEM;
			goto err_unlock;
		}
	}

	err = amdgpu_amdkfd_gpuvm_alloc_memory_of_gpu(
		dev->adev, args->va_addr, args->size,
		pdd->drm_priv, NULL, (struct kgd_mem **) &mem, &offset,
		flags, false);

	if (err)
		goto err_unlock;

	mem_type = flags & (KFD_IOC_ALLOC_MEM_FLAGS_VRAM |
			    KFD_IOC_ALLOC_MEM_FLAGS_GTT |
			    KFD_IOC_ALLOC_MEM_FLAGS_USERPTR |
			    KFD_IOC_ALLOC_MEM_FLAGS_DOORBELL |
			    KFD_IOC_ALLOC_MEM_FLAGS_MMIO_REMAP);
	idr_handle = kfd_process_device_create_obj_handle(pdd, mem,
			args->va_addr, args->size, cpuva, mem_type, -1);
	if (idr_handle < 0) {
		err = -EFAULT;
		goto err_free;
	}

	/* Update the VRAM usage count */
	if (flags & KFD_IOC_ALLOC_MEM_FLAGS_VRAM)
		WRITE_ONCE(pdd->vram_usage, pdd->vram_usage + args->size);

	amdgpu_read_unlock(dev->ddev);
	mutex_unlock(&p->mutex);

	args->handle = MAKE_HANDLE(args->gpu_id, idr_handle);
	args->mmap_offset = offset;

	/* MMIO is mapped through kfd device
	 * Generate a kfd mmap offset
	 */
	if (flags & KFD_IOC_ALLOC_MEM_FLAGS_MMIO_REMAP)
		args->mmap_offset = KFD_MMAP_TYPE_MMIO
					| KFD_MMAP_GPU_ID(args->gpu_id);

	return 0;

err_free:
	amdgpu_amdkfd_gpuvm_free_memory_of_gpu(dev->adev, (struct kgd_mem *)mem,
					       pdd->drm_priv, NULL);
err_unlock:
	amdgpu_read_unlock(dev->ddev);
err_pdd:
err_large_bar:
err_read_lock:
	mutex_unlock(&p->mutex);
	return err;
}

static int kfd_ioctl_free_memory_of_gpu(struct file *filep,
					struct kfd_process *p, void *data)
{
	struct kfd_ioctl_free_memory_of_gpu_args *args = data;
	struct kfd_process_device *pdd;
	struct kfd_bo *buf_obj;
	int ret;
	uint64_t size = 0;

	mutex_lock(&p->mutex);
	/*
	 * Safeguard to prevent user space from freeing signal BO.
	 * It will be freed at process termination.
	 */
	if (p->signal_handle && (p->signal_handle == args->handle)) {
		pr_err("Free signal BO is not allowed\n");
		ret = -EPERM;
		goto err_unlock;
	}

	pdd = kfd_process_device_data_by_id(p, GET_GPU_ID(args->handle));
	if (!pdd) {
		pr_err("Process device data doesn't exist\n");
		ret = -EINVAL;
		goto err_pdd;
	}

	ret = amdgpu_read_lock(pdd->dev->ddev, true);
	if (ret)
		goto err_read_lock;

	buf_obj = kfd_process_device_find_bo(pdd,
					GET_IDR_HANDLE(args->handle));
	if (!buf_obj) {
		ret = -EINVAL;
		goto err_bo;
	}

	ret = amdgpu_amdkfd_gpuvm_free_memory_of_gpu(pdd->dev->adev,
						buf_obj->mem, pdd->drm_priv, &size);

	/* If freeing the buffer failed, leave the handle in place for
	 * clean-up during process tear-down.
	 */
	if (!ret)
		kfd_process_device_remove_obj_handle(
			pdd, GET_IDR_HANDLE(args->handle));

	WRITE_ONCE(pdd->vram_usage, pdd->vram_usage - size);

err_bo:
	amdgpu_read_unlock(pdd->dev->ddev);
err_unlock:
err_pdd:
err_read_lock:
	mutex_unlock(&p->mutex);
	return ret;
}

static bool kfd_flush_tlb_after_unmap(struct kfd_dev *dev)
{
	return KFD_GC_VERSION(dev) == IP_VERSION(9, 4, 2) ||
		(KFD_GC_VERSION(dev) == IP_VERSION(9, 4, 1) &&
		dev->adev->sdma.instance[0].fw_version >= 18);
}

static int kfd_ioctl_map_memory_to_gpu(struct file *filep,
					struct kfd_process *p, void *data)
{
	struct kfd_ioctl_map_memory_to_gpu_args *args = data;
	struct kfd_process_device *pdd, *peer_pdd;
	void *mem;
	struct kfd_dev *dev;
	long err = 0;
	int i;
	uint32_t *devices_arr = NULL;
	bool table_freed = false;

	trace_kfd_map_memory_to_gpu_start(p);
	if (!args->n_devices) {
		pr_debug("Device IDs array empty\n");
		return -EINVAL;
	}
	if (args->n_success > args->n_devices) {
		pr_debug("n_success exceeds n_devices\n");
		return -EINVAL;
	}

	devices_arr = kmalloc_array(args->n_devices, sizeof(*devices_arr),
				    GFP_KERNEL);
	if (!devices_arr)
		return -ENOMEM;

	err = copy_from_user(devices_arr,
			     (void __user *)args->device_ids_array_ptr,
			     args->n_devices * sizeof(*devices_arr));
	if (err != 0) {
		err = -EFAULT;
		goto copy_from_user_failed;
	}

	mutex_lock(&p->mutex);
	pdd = kfd_process_device_data_by_id(p, GET_GPU_ID(args->handle));
	if (!pdd) {
		err = -EINVAL;
		goto get_process_device_data_failed;
	}
	dev = pdd->dev;

	pdd = kfd_bind_process_to_device(dev, p);
	if (IS_ERR(pdd)) {
		err = PTR_ERR(pdd);
		goto bind_process_to_device_failed;
	}

	mem = kfd_process_device_translate_handle(pdd,
						GET_IDR_HANDLE(args->handle));
	if (!mem) {
		err = -ENOMEM;
		goto get_mem_obj_from_handle_failed;
	}

	for (i = args->n_success; i < args->n_devices; i++) {
		peer_pdd = kfd_process_device_data_by_id(p, devices_arr[i]);
		if (!peer_pdd) {
			pr_debug("Getting device by id failed for 0x%x\n",
				 devices_arr[i]);
			err = -EINVAL;
			goto get_mem_obj_from_handle_failed;
		}

		peer_pdd = kfd_bind_process_to_device(peer_pdd->dev, p);
		if (IS_ERR(peer_pdd)) {
			err = PTR_ERR(peer_pdd);
			goto get_mem_obj_from_handle_failed;
		}

		err = amdgpu_read_lock(peer_pdd->dev->ddev, true);
		if (err)
			goto map_memory_to_gpu_failed;

		err = amdgpu_amdkfd_gpuvm_map_memory_to_gpu(
			peer_pdd->dev->adev, (struct kgd_mem *)mem,
			peer_pdd->drm_priv, &table_freed);
		if (err) {
			pr_err("Failed to map to gpu %d/%d\n",
			       i, args->n_devices);
			amdgpu_read_unlock(peer_pdd->dev->ddev);
			goto map_memory_to_gpu_failed;
		}

		amdgpu_read_unlock(peer_pdd->dev->ddev);
		args->n_success = i+1;
	}

	mutex_unlock(&p->mutex);

	err = amdgpu_amdkfd_gpuvm_sync_memory(dev->adev, (struct kgd_mem *) mem, true);
	if (err) {
		pr_debug("Sync memory failed, wait interrupted by user signal\n");
		goto sync_memory_failed;
	}

	/* Flush TLBs after waiting for the page table updates to complete */
	if (table_freed || !kfd_flush_tlb_after_unmap(dev)) {
		for (i = 0; i < args->n_devices; i++) {
			peer_pdd = kfd_process_device_data_by_id(p, devices_arr[i]);
			if (WARN_ON_ONCE(!peer_pdd))
				continue;
			if (!amdgpu_read_lock(peer_pdd->dev->ddev, true)) {
				kfd_flush_tlb(peer_pdd, TLB_FLUSH_LEGACY);
				amdgpu_read_unlock(peer_pdd->dev->ddev);
			}
		}
	}
	kfree(devices_arr);

	trace_kfd_map_memory_to_gpu_end(p,
			args->n_devices * sizeof(*devices_arr), "Success");
	return err;

get_process_device_data_failed:
bind_process_to_device_failed:
get_mem_obj_from_handle_failed:
map_memory_to_gpu_failed:
	mutex_unlock(&p->mutex);
copy_from_user_failed:
sync_memory_failed:
	kfree(devices_arr);
	trace_kfd_map_memory_to_gpu_end(p,
		args->n_devices * sizeof(*devices_arr), "Failed");

	return err;
}

static int kfd_ioctl_unmap_memory_from_gpu(struct file *filep,
					struct kfd_process *p, void *data)
{
	struct kfd_ioctl_unmap_memory_from_gpu_args *args = data;
	struct kfd_process_device *pdd, *peer_pdd;
	void *mem;
	long err = 0;
	uint32_t *devices_arr = NULL, i;

	if (!args->n_devices) {
		pr_debug("Device IDs array empty\n");
		return -EINVAL;
	}
	if (args->n_success > args->n_devices) {
		pr_debug("n_success exceeds n_devices\n");
		return -EINVAL;
	}

	devices_arr = kmalloc_array(args->n_devices, sizeof(*devices_arr),
				    GFP_KERNEL);
	if (!devices_arr)
		return -ENOMEM;

	err = copy_from_user(devices_arr,
			     (void __user *)args->device_ids_array_ptr,
			     args->n_devices * sizeof(*devices_arr));
	if (err != 0) {
		err = -EFAULT;
		goto copy_from_user_failed;
	}

	mutex_lock(&p->mutex);
	pdd = kfd_process_device_data_by_id(p, GET_GPU_ID(args->handle));
	if (!pdd) {
		err = -EINVAL;
		goto bind_process_to_device_failed;
	}

	mem = kfd_process_device_translate_handle(pdd,
						GET_IDR_HANDLE(args->handle));
	if (!mem) {
		err = -ENOMEM;
		goto get_mem_obj_from_handle_failed;
	}

	for (i = args->n_success; i < args->n_devices; i++) {
		peer_pdd = kfd_process_device_data_by_id(p, devices_arr[i]);
		if (!peer_pdd) {
			err = -EINVAL;
			goto get_mem_obj_from_handle_failed;
		}

		err = amdgpu_read_lock(peer_pdd->dev->ddev, true);
		if (err)
			goto unmap_memory_from_gpu_failed;

		err = amdgpu_amdkfd_gpuvm_unmap_memory_from_gpu(
			peer_pdd->dev->adev, (struct kgd_mem *)mem, peer_pdd->drm_priv);
		if (err) {
			pr_err("Failed to unmap from gpu %d/%d\n",
			       i, args->n_devices);
			amdgpu_read_unlock(peer_pdd->dev->ddev);
			goto unmap_memory_from_gpu_failed;
		}
		amdgpu_read_unlock(peer_pdd->dev->ddev);
		args->n_success = i+1;
	}
	mutex_unlock(&p->mutex);

	if (kfd_flush_tlb_after_unmap(pdd->dev)) {
		err = amdgpu_amdkfd_gpuvm_sync_memory(pdd->dev->adev,
				(struct kgd_mem *) mem, true);
		if (err) {
			pr_debug("Sync memory failed, wait interrupted by user signal\n");
			goto sync_memory_failed;
		}

		/* Flush TLBs after waiting for the page table updates to complete */
		for (i = 0; i < args->n_devices; i++) {
			peer_pdd = kfd_process_device_data_by_id(p, devices_arr[i]);
			if (WARN_ON_ONCE(!peer_pdd))
				continue;
			if (!amdgpu_read_lock(peer_pdd->dev->ddev, true)) {
				kfd_flush_tlb(peer_pdd, TLB_FLUSH_HEAVYWEIGHT);
				amdgpu_read_unlock(peer_pdd->dev->ddev);
			}
		}
	}
	kfree(devices_arr);

	return 0;

bind_process_to_device_failed:
get_mem_obj_from_handle_failed:
unmap_memory_from_gpu_failed:
	mutex_unlock(&p->mutex);
copy_from_user_failed:
sync_memory_failed:
	kfree(devices_arr);
	return err;
}

static int kfd_ioctl_alloc_queue_gws(struct file *filep,
		struct kfd_process *p, void *data)
{
	int retval;
	struct kfd_ioctl_alloc_queue_gws_args *args = data;
	struct queue *q;
	struct kfd_dev *dev;

	mutex_lock(&p->mutex);
	q = pqm_get_user_queue(&p->pqm, args->queue_id);

	if (q) {
		dev = q->device;
	} else {
		retval = -EINVAL;
		goto out_unlock;
	}

	if (!dev->gws) {
		retval = -ENODEV;
		goto out_unlock;
	}

	if (dev->dqm->sched_policy == KFD_SCHED_POLICY_NO_HWS) {
		retval = -ENODEV;
		goto out_unlock;
	}

	if (dev->gws_debug_workaround && p->debug_trap_enabled) {
		retval = -EBUSY;
		goto out_unlock;
	}

	retval = amdgpu_read_lock(dev->ddev, true);
	if (retval)
		goto out_unlock;

	retval = pqm_set_gws(&p->pqm, args->queue_id, args->num_gws ? dev->gws : NULL);

	amdgpu_read_unlock(dev->ddev);
	mutex_unlock(&p->mutex);

	args->first_gws = 0;
	return retval;

out_unlock:
	mutex_unlock(&p->mutex);
	return retval;
}

static int kfd_ioctl_get_dmabuf_info(struct file *filep,
		struct kfd_process *p, void *data)
{
	struct kfd_ioctl_get_dmabuf_info_args *args = data;
	struct kfd_dev *dev = NULL;
	struct amdgpu_device *dmabuf_adev;
	void *metadata_buffer = NULL;
	uint32_t flags;
	unsigned int i;
	int r;

	/* Find a KFD GPU device that supports the get_dmabuf_info query */
	for (i = 0; kfd_topology_enum_kfd_devices(i, &dev) == 0; i++)
		if (dev)
			break;
	if (!dev)
		return -EINVAL;

	if (args->metadata_ptr) {
		metadata_buffer = kzalloc(args->metadata_size, GFP_KERNEL);
		if (!metadata_buffer)
			return -ENOMEM;
	}

	/* Get dmabuf info from KGD */
	r = amdgpu_amdkfd_get_dmabuf_info(dev->adev, args->dmabuf_fd,
					  &dmabuf_adev, &args->size,
					  metadata_buffer, args->metadata_size,
					  &args->metadata_size, &flags);
	if (r)
		goto exit;

	/* Reverse-lookup gpu_id from kgd pointer */
	dev = kfd_device_by_adev(dmabuf_adev);
	if (!dev) {
		r = -EINVAL;
		goto exit;
	}
	args->gpu_id = dev->id;
	args->flags = flags;

	/* Copy metadata buffer to user mode */
	if (metadata_buffer) {
		r = copy_to_user((void __user *)args->metadata_ptr,
				 metadata_buffer, args->metadata_size);
		if (r != 0)
			r = -EFAULT;
	}

exit:
	kfree(metadata_buffer);

	return r;
}

static int kfd_ioctl_import_dmabuf(struct file *filep,
				   struct kfd_process *p, void *data)
{
	struct kfd_ioctl_import_dmabuf_args *args = data;
	struct kfd_process_device *pdd;
	int r;

	mutex_lock(&p->mutex);
	pdd = kfd_process_device_data_by_id(p, args->gpu_id);
	mutex_unlock(&p->mutex);
	if (!pdd)
		return -EINVAL;

	r = kfd_ipc_import_dmabuf(pdd->dev, p, args->gpu_id, args->dmabuf_fd,
				  args->va_addr, &args->handle, NULL);
	if (r)
		pr_err("Failed to import dmabuf\n");

	return r;
}

static int kfd_ioctl_ipc_export_handle(struct file *filep,
				       struct kfd_process *p,
				       void *data)
{
	struct kfd_ioctl_ipc_export_handle_args *args = data;
	struct kfd_process_device *pdd;
	int r;

	mutex_lock(&p->mutex);
	pdd = kfd_process_device_data_by_id(p, args->gpu_id);
	mutex_unlock(&p->mutex);
	if (!pdd)
		return -EINVAL;

	r = kfd_ipc_export_as_handle(pdd->dev, p, args->handle, args->share_handle,
				     args->flags);
	if (r)
		pr_err("Failed to export IPC handle\n");

	return r;
}

static int kfd_ioctl_ipc_import_handle(struct file *filep,
				       struct kfd_process *p,
				       void *data)
{
	struct kfd_ioctl_ipc_import_handle_args *args = data;
	struct kfd_process_device *pdd;
	int r;

	mutex_lock(&p->mutex);
	pdd = kfd_process_device_data_by_id(p, args->gpu_id);
	mutex_unlock(&p->mutex);
	if (!pdd)
		return -EINVAL;

	r = kfd_ipc_import_handle(pdd->dev, p, args->gpu_id, args->share_handle,
				  args->va_addr, &args->handle,
				  &args->mmap_offset, &args->flags);
	if (r)
		pr_err("Failed to import IPC handle\n");

	return r;
}

#ifndef PTRACE_MODE_ATTACH_REALCREDS
#define PTRACE_MODE_ATTACH_REALCREDS  PTRACE_MODE_ATTACH
#endif

/* Maximum number of entries for process pages array which lives on stack */
#define MAX_PP_STACK_COUNT 16
/* Maximum number of pages kmalloc'd to hold struct page's during copy */
#define MAX_KMALLOC_PAGES (PAGE_SIZE * 2)
#define MAX_PP_KMALLOC_COUNT (MAX_KMALLOC_PAGES/sizeof(struct page *))

static void kfd_put_sg_table(struct sg_table *sg)
{
	unsigned int i;
	struct scatterlist *s;

	for_each_sg(sg->sgl, s, sg->nents, i)
		put_page(sg_page(s));
}


/* Create a sg table for the given userptr BO by pinning its system pages
 * @bo: userptr BO
 * @offset: Offset into BO
 * @mm/@task: mm_struct & task_struct of the process that holds the BO
 * @size: in/out: desired size / actual size which could be smaller
 * @sg_size: out: Size of sg table. This is ALIGN_UP(@size)
 * @ret_sg: out sg table
 */
static int kfd_create_sg_table_from_userptr_bo(struct kfd_bo *bo,
					       int64_t offset, int cma_write,
					       struct mm_struct *mm,
					       struct task_struct *task,
					       uint64_t *size,
					       uint64_t *sg_size,
					       struct sg_table **ret_sg)
{
	int ret, locked = 1;
	struct sg_table *sg = NULL;
	unsigned int i, offset_in_page, flags = 0;
	unsigned long nents, n;
	unsigned long pa = (bo->cpuva + offset) & PAGE_MASK;
	unsigned int cur_page = 0;
	struct scatterlist *s;
	uint64_t sz = *size;
	struct page **process_pages;

	*sg_size = 0;
	sg = kmalloc(sizeof(*sg), GFP_KERNEL);
	if (!sg)
		return -ENOMEM;

	offset_in_page = offset & (PAGE_SIZE - 1);
	nents = (sz + offset_in_page + PAGE_SIZE - 1) / PAGE_SIZE;

	ret = sg_alloc_table(sg, nents, GFP_KERNEL);
	if (unlikely(ret)) {
		ret = -ENOMEM;
		goto sg_alloc_fail;
	}
	process_pages = kmalloc_array(nents, sizeof(struct pages *),
				      GFP_KERNEL);
	if (!process_pages) {
		ret = -ENOMEM;
		goto page_alloc_fail;
	}

	if (cma_write)
		flags = FOLL_WRITE;
	locked = 1;
	mmap_read_lock(mm);
	n = kcl_get_user_pages_remote(task, mm, pa, nents, flags, process_pages,
				  NULL, &locked);
	if (locked)
		mmap_read_unlock(mm);
	if (n <= 0) {
		pr_err("CMA: Invalid virtual address 0x%lx\n", pa);
		ret = -EFAULT;
		goto get_user_fail;
	}
	if (n != nents) {
		/* Pages pinned < requested. Set the size accordingly */
		*size = (n * PAGE_SIZE) - offset_in_page;
		pr_debug("Requested %lx but pinned %lx\n", nents, n);
	}

	sz = 0;
	for_each_sg(sg->sgl, s, n, i) {
		sg_set_page(s, process_pages[cur_page], PAGE_SIZE,
			    offset_in_page);
		sg_dma_address(s) = page_to_phys(process_pages[cur_page]);
		offset_in_page = 0;
		cur_page++;
		sz += PAGE_SIZE;
	}
	*ret_sg = sg;
	*sg_size = sz;

	kfree(process_pages);
	return 0;

get_user_fail:
	kfree(process_pages);
page_alloc_fail:
	sg_free_table(sg);
sg_alloc_fail:
	kfree(sg);
	return ret;
}

static void kfd_free_cma_bos(struct cma_iter *ci)
{
	struct cma_system_bo *cma_bo, *tmp;

	list_for_each_entry_safe(cma_bo, tmp, &ci->cma_list, list) {
		struct kfd_dev *dev = cma_bo->dev;
		struct kfd_process_device *pdd;

		/* sg table is deleted by free_memory_of_gpu */
		if (cma_bo->sg)
			kfd_put_sg_table(cma_bo->sg);
		pdd = kfd_get_process_device_data(dev, ci->p);
		amdgpu_amdkfd_gpuvm_free_memory_of_gpu(dev->adev, cma_bo->mem, pdd->drm_priv, NULL);
		list_del(&cma_bo->list);
		kfree(cma_bo);
	}
}

/* 1 second timeout */
#define CMA_WAIT_TIMEOUT msecs_to_jiffies(1000)

static int kfd_cma_fence_wait(struct dma_fence *f)
{
	int ret;

	ret = dma_fence_wait_timeout(f, false, CMA_WAIT_TIMEOUT);
	if (likely(ret > 0))
		return 0;
	if (!ret)
		ret = -ETIME;
	return ret;
}

/* Put previous (old) fence @pf but it waits for @pf to signal if the context
 * of the current fence @cf is different.
 */
static int kfd_fence_put_wait_if_diff_context(struct dma_fence *cf,
					      struct dma_fence *pf)
{
	int ret = 0;

	if (pf && cf && cf->context != pf->context)
		ret = kfd_cma_fence_wait(pf);
	dma_fence_put(pf);
	return ret;
}

#define MAX_SYSTEM_BO_SIZE (512*PAGE_SIZE)

/* Create an equivalent system BO for the given @bo. If @bo is a userptr then
 * create a new system BO by pinning underlying system pages of the given
 * userptr BO. If @bo is in Local Memory then create an empty system BO and
 * then copy @bo into this new BO.
 * @bo: Userptr BO or Local Memory BO
 * @offset: Offset into bo
 * @size: in/out: The size of the new BO could be less than requested if all
 *        the pages couldn't be pinned or size > MAX_SYSTEM_BO_SIZE. This would
 *        be reflected in @size
 * @mm/@task: mm/task to which @bo belongs to
 * @cma_bo: out: new system BO
 */
static int kfd_create_cma_system_bo(struct kfd_dev *kdev, struct kfd_bo *bo,
				    uint64_t *size, uint64_t offset,
				    int cma_write, struct kfd_process *p,
				    struct mm_struct *mm,
				    struct task_struct *task,
				    struct cma_system_bo **cma_bo)
{
	int ret;
	struct kfd_process_device *pdd = NULL;
	struct cma_system_bo *cbo;
	uint64_t bo_size = 0;
	struct dma_fence *f;

	uint32_t flags = KFD_IOC_ALLOC_MEM_FLAGS_GTT | KFD_IOC_ALLOC_MEM_FLAGS_WRITABLE |
			 KFD_IOC_ALLOC_MEM_FLAGS_NO_SUBSTITUTE;

	*cma_bo = NULL;
	cbo = kzalloc(sizeof(**cma_bo), GFP_KERNEL);
	if (!cbo)
		return -ENOMEM;

	INIT_LIST_HEAD(&cbo->list);
	if (bo->mem_type == KFD_IOC_ALLOC_MEM_FLAGS_VRAM)
		bo_size = min_t(uint64_t, *size, MAX_SYSTEM_BO_SIZE);
	else if (bo->cpuva) {
		ret = kfd_create_sg_table_from_userptr_bo(bo, offset,
							  cma_write, mm, task,
							  size, &bo_size,
							  &cbo->sg);
		if (ret) {
			pr_err("CMA: BO create with sg failed %d\n", ret);
			goto sg_fail;
		}
	} else {
		WARN_ON(1);
		ret = -EINVAL;
		goto sg_fail;
	}
	mutex_lock(&p->mutex);
	pdd = kfd_get_process_device_data(kdev, p);
	if (!pdd) {
		mutex_unlock(&p->mutex);
		pr_err("Process device data doesn't exist\n");
		ret = -EINVAL;
		goto pdd_fail;
	}

	ret = amdgpu_amdkfd_gpuvm_alloc_memory_of_gpu(kdev->adev, 0ULL, bo_size,
						      pdd->drm_priv, cbo->sg,
						      &cbo->mem, NULL, flags,
						      false);
	mutex_unlock(&p->mutex);
	if (ret) {
		pr_err("Failed to create shadow system BO %d\n", ret);
		goto pdd_fail;
	}

	if (bo->mem_type == KFD_IOC_ALLOC_MEM_FLAGS_VRAM) {
		ret = amdgpu_amdkfd_copy_mem_to_mem(kdev->adev, bo->mem,
						    offset, cbo->mem, 0,
						    bo_size, &f, size);
		if (ret) {
			pr_err("CMA: Intermediate copy failed %d\n", ret);
			goto copy_fail;
		}

		/* Wait for the copy to finish as subsequent copy will be done
		 * by different device
		 */
		ret = kfd_cma_fence_wait(f);
		dma_fence_put(f);
		if (ret) {
			pr_err("CMA: Intermediate copy timed out %d\n", ret);
			goto copy_fail;
		}
	}

	cbo->dev = kdev;
	*cma_bo = cbo;

	return ret;

copy_fail:
	amdgpu_amdkfd_gpuvm_free_memory_of_gpu(kdev->adev, bo->mem, pdd->drm_priv, NULL);
pdd_fail:
	if (cbo->sg) {
		kfd_put_sg_table(cbo->sg);
		sg_free_table(cbo->sg);
		kfree(cbo->sg);
	}
sg_fail:
	kfree(cbo);
	return ret;
}

/* Update cma_iter.cur_bo with KFD BO that is assocaited with
 * cma_iter.array.va_addr
 */
static int kfd_cma_iter_update_bo(struct cma_iter *ci)
{
	struct kfd_memory_range *arr = ci->array;
	uint64_t va_end = arr->va_addr + arr->size - 1;

	mutex_lock(&ci->p->mutex);
	ci->cur_bo = kfd_process_find_bo_from_interval(ci->p, arr->va_addr,
						       va_end);
	mutex_unlock(&ci->p->mutex);

	if (!ci->cur_bo || va_end > ci->cur_bo->it.last) {
		pr_err("CMA failed. Range out of bounds\n");
		return -EFAULT;
	}
	return 0;
}

/* Advance iter by @size bytes. */
static int kfd_cma_iter_advance(struct cma_iter *ci, unsigned long size)
{
	int ret = 0;

	ci->offset += size;
	if (WARN_ON(size > ci->total || ci->offset > ci->array->size))
		return -EFAULT;
	ci->total -= size;
	/* If current range is copied, move to next range if available. */
	if (ci->offset == ci->array->size) {

		/* End of all ranges */
		if (!(--ci->nr_segs))
			return 0;

		ci->array++;
		ci->offset = 0;
		ret = kfd_cma_iter_update_bo(ci);
		if (ret)
			return ret;
	}
	ci->bo_offset = (ci->array->va_addr + ci->offset) -
			   ci->cur_bo->it.start;
	return ret;
}

static int kfd_cma_iter_init(struct kfd_memory_range *arr, unsigned long segs,
			     struct kfd_process *p, struct mm_struct *mm,
			     struct task_struct *task, struct cma_iter *ci)
{
	int ret;
	int nr;

	if (!arr || !segs)
		return -EINVAL;

	memset(ci, 0, sizeof(*ci));
	INIT_LIST_HEAD(&ci->cma_list);
	ci->array = arr;
	ci->nr_segs = segs;
	ci->p = p;
	ci->offset = 0;
	ci->mm = mm;
	ci->task = task;
	for (nr = 0; nr < segs; nr++)
		ci->total += arr[nr].size;

	/* Valid but size is 0. So copied will also be 0 */
	if (!ci->total)
		return 0;

	ret = kfd_cma_iter_update_bo(ci);
	if (!ret)
		ci->bo_offset = arr->va_addr - ci->cur_bo->it.start;
	return ret;
}

static bool kfd_cma_iter_end(struct cma_iter *ci)
{
	if (!(ci->nr_segs) || !(ci->total))
		return true;
	return false;
}

/* Copies @size bytes from si->cur_bo to di->cur_bo BO. The function assumes
 * both source and dest. BOs are userptr BOs. Both BOs can either belong to
 * current process or one of the BOs can belong to a differnt
 * process. @Returns 0 on success, -ve on failure
 *
 * @si: Source iter
 * @di: Dest. iter
 * @cma_write: Indicates if it is write to remote or read from remote
 * @size: amount of bytes to be copied
 * @copied: Return number of bytes actually copied.
 */
static int kfd_copy_userptr_bos(struct cma_iter *si, struct cma_iter *di,
				bool cma_write, uint64_t size,
				uint64_t *copied)
{
	int i, ret = 0, locked;
	unsigned int nents, nl;
	unsigned int offset_in_page;
	struct page *pp_stack[MAX_PP_STACK_COUNT];
	struct page **process_pages = pp_stack;
	unsigned long rva, lva = 0, flags = 0;
	uint64_t copy_size, to_copy = size;
	struct cma_iter *li, *ri;

	if (cma_write) {
		ri = di;
		li = si;
		flags |= FOLL_WRITE;
	} else {
		li = di;
		ri = si;
	}
	/* rva: remote virtual address. Page aligned to start page.
	 * rva + offset_in_page: Points to remote start address
	 * lva: local virtual address. Points to the start address.
	 * nents: computes number of remote pages to request
	 */
	offset_in_page = ri->bo_offset & (PAGE_SIZE - 1);
	rva = (ri->cur_bo->cpuva + ri->bo_offset) & PAGE_MASK;
	lva = li->cur_bo->cpuva + li->bo_offset;

	nents = (size + offset_in_page + PAGE_SIZE - 1) / PAGE_SIZE;

	copy_size = min_t(uint64_t, size, PAGE_SIZE - offset_in_page);
	*copied = 0;

	if (nents > MAX_PP_STACK_COUNT) {
		/* For reliability kmalloc only 2 pages worth */
		process_pages = kmalloc(min_t(size_t, MAX_KMALLOC_PAGES,
					      sizeof(struct pages *)*nents),
					GFP_KERNEL);

		if (!process_pages)
			return -ENOMEM;
	}

	while (nents && to_copy) {
		nl = min_t(unsigned int, MAX_PP_KMALLOC_COUNT, nents);
		locked = 1;
		mmap_read_lock(ri->mm);
		nl = kcl_get_user_pages_remote(ri->task, ri->mm, rva, nl,
					   flags, process_pages, NULL,
					   &locked);
		if (locked)
			mmap_read_unlock(ri->mm);
		if (nl <= 0) {
			pr_err("CMA: Invalid virtual address 0x%lx\n", rva);
			ret = -EFAULT;
			break;
		}

		for (i = 0; i < nl; i++) {
			unsigned int n;
			void *kaddr = kmap(process_pages[i]);

			if (cma_write) {
				n = copy_from_user(kaddr+offset_in_page,
						   (void *)lva, copy_size);
				set_page_dirty(process_pages[i]);
			} else {
				n = copy_to_user((void *)lva,
						 kaddr+offset_in_page,
						 copy_size);
			}
			kunmap(kaddr);
			if (n) {
				ret = -EFAULT;
				break;
			}
			to_copy -= copy_size;
			if (!to_copy)
				break;
			lva += copy_size;
			rva += (copy_size + offset_in_page);
			WARN_ONCE(rva & (PAGE_SIZE - 1),
				  "CMA: Error in remote VA computation");
			offset_in_page = 0;
			copy_size = min_t(uint64_t, to_copy, PAGE_SIZE);
		}

		for (i = 0; i < nl; i++)
			put_page(process_pages[i]);

		if (ret)
			break;
		nents -= nl;
	}

	if (process_pages != pp_stack)
		kfree(process_pages);

	*copied = (size - to_copy);
	return ret;

}

static int kfd_create_kgd_mem(struct kfd_dev *kdev, uint64_t size,
			      struct kfd_process *p, struct kgd_mem **mem)
{
	int ret;
	struct kfd_process_device *pdd = NULL;
	uint32_t flags = KFD_IOC_ALLOC_MEM_FLAGS_GTT | KFD_IOC_ALLOC_MEM_FLAGS_WRITABLE |
			 KFD_IOC_ALLOC_MEM_FLAGS_NO_SUBSTITUTE;

	if (!mem || !size || !p || !kdev)
		return -EINVAL;

	*mem = NULL;

	mutex_lock(&p->mutex);
	pdd = kfd_get_process_device_data(kdev, p);
	if (!pdd) {
		mutex_unlock(&p->mutex);
		pr_err("Process device data doesn't exist\n");
		return -EINVAL;
	}

	ret = amdgpu_amdkfd_gpuvm_alloc_memory_of_gpu(kdev->adev, 0ULL, size,
						      pdd->drm_priv, NULL,
						      mem, NULL, flags, false);

	mutex_unlock(&p->mutex);
	if (ret) {
		pr_err("Failed to create shadow system BO %d\n", ret);
		return -EINVAL;
	}

	return 0;
}

static int kfd_destroy_kgd_mem(struct kgd_mem *mem)
{
	struct amdgpu_device *adev;
	struct task_struct *task;
	struct kfd_process *p;
	struct kfd_process_device *pdd;
	uint32_t gpu_id, gpu_idx;
	int r;

	if (!mem)
		return -EINVAL;

	adev = amdgpu_ttm_adev(mem->bo->tbo.bdev);
	task = get_pid_task(mem->process_info->pid, PIDTYPE_PID);
	p = kfd_get_process(task);
	r = kfd_process_gpuid_from_adev(p, adev, &gpu_id, &gpu_idx);
	if (r < 0) {
		pr_warn("no gpu id found, mem maybe leaking\n");
		return -EINVAL;
	}
	pdd = kfd_process_device_from_gpuidx(p, gpu_idx);

	/* param adev is not used*/
	return amdgpu_amdkfd_gpuvm_free_memory_of_gpu(pdd->dev->adev, mem, pdd->drm_priv, NULL);
}

/* Copies @size bytes from si->cur_bo to di->cur_bo starting at their
 * respective offset.
 * @si: Source iter
 * @di: Dest. iter
 * @cma_write: Indicates if it is write to remote or read from remote
 * @size: amount of bytes to be copied
 * @f: Return the last fence if any
 * @copied: Return number of bytes actually copied.
 */
static int kfd_copy_bos(struct cma_iter *si, struct cma_iter *di,
			int cma_write, uint64_t size,
			struct dma_fence **f, uint64_t *copied,
			struct kgd_mem **tmp_mem)
{
	int err = 0;
	struct kfd_bo *dst_bo = di->cur_bo, *src_bo = si->cur_bo;
	uint64_t src_offset = si->bo_offset, dst_offset = di->bo_offset;
	struct kgd_mem *src_mem = src_bo->mem, *dst_mem = dst_bo->mem;
	struct kfd_dev *dev = dst_bo->dev;
	int d2d = 0;

	*copied = 0;
	if (f)
		*f = NULL;
	if (src_bo->cpuva && dst_bo->cpuva)
		return kfd_copy_userptr_bos(si, di, cma_write, size, copied);

	/* If either source or dest. is userptr, create a shadow system BO
	 * by using the underlying userptr BO pages. Then use this shadow
	 * BO for copy. src_offset & dst_offset are adjusted because the new BO
	 * is only created for the window (offset, size) requested.
	 * The shadow BO is created on the other device. This means if the
	 * other BO is a device memory, the copy will be using that device.
	 * The BOs are stored in cma_list for deferred cleanup. This minimizes
	 * fence waiting just to the last fence.
	 */
	if (src_bo->cpuva) {
		dev = dst_bo->dev;
		err = kfd_create_cma_system_bo(dev, src_bo, &size,
					       si->bo_offset, cma_write,
					       si->p, si->mm, si->task,
					       &si->cma_bo);
		src_mem = si->cma_bo->mem;
		src_offset = si->bo_offset & (PAGE_SIZE - 1);
		list_add_tail(&si->cma_bo->list, &si->cma_list);
	} else if (dst_bo->cpuva) {
		dev = src_bo->dev;
		err = kfd_create_cma_system_bo(dev, dst_bo, &size,
					       di->bo_offset, cma_write,
					       di->p, di->mm, di->task,
					       &di->cma_bo);
		dst_mem = di->cma_bo->mem;
		dst_offset = di->bo_offset & (PAGE_SIZE - 1);
		list_add_tail(&di->cma_bo->list, &di->cma_list);
	} else if (src_bo->dev->adev!= dst_bo->dev->adev) {
		/* This indicates that atleast on of the BO is in local mem.
		 * If both are in local mem of different devices then create an
		 * intermediate System BO and do a double copy
		 * [VRAM]--gpu1-->[System BO]--gpu2-->[VRAM].
		 * If only one BO is in VRAM then use that GPU to do the copy
		 */
		if (src_bo->mem_type == KFD_IOC_ALLOC_MEM_FLAGS_VRAM &&
		    dst_bo->mem_type == KFD_IOC_ALLOC_MEM_FLAGS_VRAM) {
			dev = dst_bo->dev;
			size = min_t(uint64_t, size, MAX_SYSTEM_BO_SIZE);
			d2d = 1;

			if (*tmp_mem == NULL) {
				if (kfd_create_kgd_mem(src_bo->dev,
							MAX_SYSTEM_BO_SIZE,
							si->p,
							tmp_mem))
					return -EINVAL;
			}

			if (amdgpu_amdkfd_copy_mem_to_mem(src_bo->dev->adev,
						src_bo->mem, si->bo_offset,
						*tmp_mem, 0,
						size, f, &size))
				/* tmp_mem will be freed in caller.*/
				return -EINVAL;

			kfd_cma_fence_wait(*f);
			dma_fence_put(*f);

			src_mem = *tmp_mem;
			src_offset = 0;
		} else if (src_bo->mem_type == KFD_IOC_ALLOC_MEM_FLAGS_VRAM)
			dev = src_bo->dev;
		/* else already set to dst_bo->dev */
	}

	if (err) {
		pr_err("Failed to create system BO %d", err);
		return -EINVAL;
	}

	err = amdgpu_amdkfd_copy_mem_to_mem(dev->adev, src_mem, src_offset,
					    dst_mem, dst_offset, size, f,
					    copied);
	/* The tmp_bo allocates additional memory. So it is better to wait and
	 * delete. Also since multiple GPUs are involved the copies are
	 * currently not pipelined.
	 */
	if (*tmp_mem && d2d) {
		if (!err) {
			kfd_cma_fence_wait(*f);
			dma_fence_put(*f);
			*f = NULL;
		}
	}
	return err;
}

/* Copy single range from source iterator @si to destination iterator @di.
 * @si will move to next range and @di will move by bytes copied.
 * @return : 0 for success or -ve for failure
 * @f: The last fence if any
 * @copied: out: number of bytes copied
 */
static int kfd_copy_single_range(struct cma_iter *si, struct cma_iter *di,
				 bool cma_write, struct dma_fence **f,
				 uint64_t *copied, struct kgd_mem **tmp_mem)
{
	int err = 0;
	uint64_t copy_size, n;
	uint64_t size = si->array->size;
	struct kfd_bo *src_bo = si->cur_bo;
	struct dma_fence *lfence = NULL;

	if (!src_bo || !di || !copied)
		return -EINVAL;
	*copied = 0;
	if (f)
		*f = NULL;

	while (size && !kfd_cma_iter_end(di)) {
		struct dma_fence *fence = NULL;

		copy_size = min(size, (di->array->size - di->offset));

		err = kfd_copy_bos(si, di, cma_write, copy_size,
				&fence, &n, tmp_mem);
		if (err) {
			pr_err("CMA %d failed\n", err);
			break;
		}

		if (fence) {
			err = kfd_fence_put_wait_if_diff_context(fence,
								 lfence);
			lfence = fence;
			if (err)
				break;
		}

		size -= n;
		*copied += n;
		err = kfd_cma_iter_advance(si, n);
		if (err)
			break;
		err = kfd_cma_iter_advance(di, n);
		if (err)
			break;
	}

	if (f)
		*f = dma_fence_get(lfence);
	dma_fence_put(lfence);

	return err;
}

static int kfd_ioctl_cross_memory_copy(struct file *filep,
				       struct kfd_process *local_p, void *data)
{
	struct kfd_ioctl_cross_memory_copy_args *args = data;
	struct kfd_memory_range *src_array, *dst_array;
	struct kfd_process *remote_p;
	struct task_struct *remote_task;
	struct mm_struct *remote_mm;
	struct pid *remote_pid;
	struct dma_fence *lfence = NULL;
	uint64_t copied = 0, total_copied = 0;
	struct cma_iter di, si;
	const char *cma_op;
	int err = 0;
	struct kgd_mem *tmp_mem = NULL;

	/* Check parameters */
	if (args->src_mem_range_array == 0 || args->dst_mem_range_array == 0 ||
		args->src_mem_array_size == 0 || args->dst_mem_array_size == 0)
		return -EINVAL;
	args->bytes_copied = 0;

	/* Allocate space for source and destination arrays */
	src_array = kmalloc_array((args->src_mem_array_size +
				  args->dst_mem_array_size),
				  sizeof(struct kfd_memory_range),
				  GFP_KERNEL);
	if (!src_array)
		return -ENOMEM;
	dst_array = &src_array[args->src_mem_array_size];

	if (copy_from_user(src_array, (void __user *)args->src_mem_range_array,
			   args->src_mem_array_size *
			   sizeof(struct kfd_memory_range))) {
		err = -EFAULT;
		goto copy_from_user_fail;
	}
	if (copy_from_user(dst_array, (void __user *)args->dst_mem_range_array,
			   args->dst_mem_array_size *
			   sizeof(struct kfd_memory_range))) {
		err = -EFAULT;
		goto copy_from_user_fail;
	}

	/* Get remote process */
	remote_pid = find_get_pid(args->pid);
	if (!remote_pid) {
		pr_err("Cross mem copy failed. Invalid PID %d\n", args->pid);
		err = -ESRCH;
		goto copy_from_user_fail;
	}

	remote_task = get_pid_task(remote_pid, PIDTYPE_PID);
	if (!remote_pid) {
		pr_err("Cross mem copy failed. Invalid PID or task died %d\n",
			args->pid);
		err = -ESRCH;
		goto get_pid_task_fail;
	}

	/* Check access permission */
	remote_mm = mm_access(remote_task, PTRACE_MODE_ATTACH_REALCREDS);
	if (!remote_mm || IS_ERR(remote_mm)) {
		err = IS_ERR(remote_mm) ? PTR_ERR(remote_mm) : -ESRCH;
		if (err == -EACCES) {
			pr_err("Cross mem copy failed. Permission error\n");
			err = -EPERM;
		} else
			pr_err("Cross mem copy failed. Invalid task %d\n",
			       err);
		goto mm_access_fail;
	}

	remote_p = kfd_get_process(remote_task);
	if (IS_ERR(remote_p)) {
		pr_err("Cross mem copy failed. Invalid kfd process %d\n",
		       args->pid);
		err = -EINVAL;
		goto kfd_process_fail;
	}
	/* Initialise cma_iter si & @di with source & destination range. */
	if (KFD_IS_CROSS_MEMORY_WRITE(args->flags)) {
		cma_op = "WRITE";
		pr_debug("CMA WRITE: local -> remote\n");
		err = kfd_cma_iter_init(dst_array, args->dst_mem_array_size,
					remote_p, remote_mm, remote_task, &di);
		if (err)
			goto kfd_process_fail;
		err = kfd_cma_iter_init(src_array, args->src_mem_array_size,
					local_p, current->mm, current, &si);
		if (err)
			goto kfd_process_fail;
	} else {
		cma_op = "READ";
		pr_debug("CMA READ: remote -> local\n");

		err = kfd_cma_iter_init(dst_array, args->dst_mem_array_size,
					local_p, current->mm, current, &di);
		if (err)
			goto kfd_process_fail;
		err = kfd_cma_iter_init(src_array, args->src_mem_array_size,
					remote_p, remote_mm, remote_task, &si);
		if (err)
			goto kfd_process_fail;
	}

	/* Copy one si range at a time into di. After each call to
	 * kfd_copy_single_range() si will move to next range. di will be
	 * incremented by bytes copied
	 */
	while (!kfd_cma_iter_end(&si) && !kfd_cma_iter_end(&di)) {
		struct dma_fence *fence = NULL;

		err = kfd_copy_single_range(&si, &di,
					KFD_IS_CROSS_MEMORY_WRITE(args->flags),
					&fence, &copied, &tmp_mem);
		total_copied += copied;

		if (err)
			break;

		/* Release old fence if a later fence is created. If no
		 * new fence is created, then keep the preivous fence
		 */
		if (fence) {
			err = kfd_fence_put_wait_if_diff_context(fence,
								 lfence);
			lfence = fence;
			if (err)
				break;
		}
	}

	/* Wait for the last fence irrespective of error condition */
	if (lfence) {
		err = kfd_cma_fence_wait(lfence);
		dma_fence_put(lfence);
		if (err)
			pr_err("CMA %s failed. BO timed out\n", cma_op);
	}

	if (tmp_mem)
		kfd_destroy_kgd_mem(tmp_mem);

	kfd_free_cma_bos(&si);
	kfd_free_cma_bos(&di);

kfd_process_fail:
	mmput(remote_mm);
mm_access_fail:
	put_task_struct(remote_task);
get_pid_task_fail:
	put_pid(remote_pid);
copy_from_user_fail:
	kfree(src_array);

	/* An error could happen after partial copy. In that case this will
	 * reflect partial amount of bytes copied
	 */
	args->bytes_copied = total_copied;
	return err;
}

static int kfd_ioctl_dbg_set_debug_trap(struct file *filep,
				struct kfd_process *p, void *data)
{
	struct kfd_ioctl_dbg_trap_args *args = data;
	struct task_struct *thread = NULL;
	int r = 0, i;
	struct kfd_process *target = NULL;
	struct pid *pid = NULL;
	uint32_t *user_array = NULL;
	uint32_t debug_trap_action;
	uint64_t exception_mask;
	uint32_t data1;
	uint32_t data2;
	uint32_t data3;
	bool check_devices;
	bool need_user_array;
	uint32_t size_to_copy_to_user_array = 0;
	bool is_attach;
	bool need_proc_create = false;

	debug_trap_action = args->op;
	data1 = args->data1;
	data2 = args->data2;
	data3 = args->data3;
	exception_mask = args->exception_mask;

	if (sched_policy == KFD_SCHED_POLICY_NO_HWS) {
		pr_err("Unsupported sched_policy: %i", sched_policy);
		r = -EINVAL;
		goto out;
	}

	is_attach = debug_trap_action == KFD_IOC_DBG_TRAP_ENABLE && data1 == 1;

	check_devices =
		debug_trap_action != KFD_IOC_DBG_TRAP_NODE_SUSPEND &&
		debug_trap_action != KFD_IOC_DBG_TRAP_NODE_RESUME &&
		debug_trap_action != KFD_IOC_DBG_TRAP_GET_QUEUE_SNAPSHOT &&
		debug_trap_action != KFD_IOC_DBG_TRAP_DEVICE_SNAPSHOT &&
		debug_trap_action != KFD_IOC_DBG_TRAP_GET_VERSION;

	need_user_array =
		debug_trap_action == KFD_IOC_DBG_TRAP_NODE_SUSPEND ||
		debug_trap_action == KFD_IOC_DBG_TRAP_NODE_RESUME;

	pid = find_get_pid(args->pid);
	if (!pid) {
		pr_debug("Cannot find pid info for %i\n",
				args->pid);
		r =  -ESRCH;
		goto out;
	}

	thread = get_pid_task(pid, PIDTYPE_PID);

	rcu_read_lock();
	need_proc_create = is_attach && thread && thread != current &&
					ptrace_parent(thread) == current;
	rcu_read_unlock();

	target = need_proc_create ?
		kfd_create_process(thread) : kfd_lookup_process_by_pid(pid);
	if (!target) {
		pr_debug("Cannot find process info for %i\n",
				args->pid);
		r = -ESRCH;
		goto out;
	}

	if (target != p) {
		bool is_debugger_attached = false;

		rcu_read_lock();
		if (ptrace_parent(target->lead_thread) == current)
			is_debugger_attached = true;
		rcu_read_unlock();

		if (!is_debugger_attached) {
			pr_err("Cannot debug process\n");
			r = -ESRCH;
			goto out;
		}
	}

	mutex_lock(&target->mutex);

	if (need_user_array) {
		/* data 1 has the number of IDs */
		size_t user_array_size = sizeof(uint32_t) * data1;

		user_array = kzalloc(user_array_size, GFP_KERNEL);
		if (!user_array) {
			r = -ENOMEM;
			goto unlock_out;
		}
		/* We need to copy the queue IDs from userspace */
		if (copy_from_user(user_array,
					(uint32_t *) args->ptr,
					user_array_size)) {
			r = -EFAULT;
			goto unlock_out;
		}
	}

	if (!is_attach &&
			debug_trap_action != KFD_IOC_DBG_TRAP_GET_VERSION &&
			debug_trap_action != KFD_IOC_DBG_TRAP_RUNTIME_ENABLE &&
			!target->debug_trap_enabled) {
		pr_err("Debugging is not enabled for this process\n");
		r = -EINVAL;
		goto unlock_out;
	}

	if (check_devices) {
		for (i = 0; i < target->n_pdds; i++) {
			struct kfd_process_device *pdd = target->pdds[i];

			if (!KFD_IS_SOC15(pdd->dev)) {
				r = -ENODEV;
				goto unlock_out;
			}

			if (pdd->dev->gws_debug_workaround &&
							pdd->qpd.num_gws) {
				r = -EBUSY;
				goto unlock_out;
			}
		}
	}

	switch (debug_trap_action) {
	case KFD_IOC_DBG_TRAP_ENABLE:
		switch (data1) {
		case 0:
			r = kfd_dbg_trap_disable(target);
			break;
		case 1:
			if (target != p)
				target->debugger_process = p;
			r = kfd_dbg_trap_enable(target, args->data2,
						(void __user *) args->ptr,
						&args->data3);
			if (!r)
				target->exception_enable_mask = exception_mask;
			break;
		default:
			pr_err("Invalid trap enable option: %i\n",
					data1);
			r = -EINVAL;
		}
		break;

	case KFD_IOC_DBG_TRAP_SET_WAVE_LAUNCH_OVERRIDE:
		r = kfd_dbg_trap_set_wave_launch_override(
				target,
				data1,
				data2,
				data3,
				&args->data2,
				&args->data3);
		break;

	case KFD_IOC_DBG_TRAP_SET_WAVE_LAUNCH_MODE:
		r = kfd_dbg_trap_set_wave_launch_mode(target,
						data1);
		break;

	case KFD_IOC_DBG_TRAP_NODE_SUSPEND:
		r = suspend_queues(target,
				data1, /* Number of queues */
				data2, /* Grace Period */
				exception_mask, /* Clear mask */
				user_array); /* array of queue ids */

		size_to_copy_to_user_array = data1;

		break;

	case KFD_IOC_DBG_TRAP_NODE_RESUME:
		r = resume_queues(target,
				false,
				data1, /* Number of queues */
				user_array); /* array of queue ids */

		size_to_copy_to_user_array = data1;

		break;
	case KFD_IOC_DBG_TRAP_QUERY_DEBUG_EVENT:
		r = kfd_dbg_ev_query_debug_event(target,
				/* data1 = 0 (process) or gpu_id or queue_id */
				&args->data1,
				&args->data2,
				exception_mask, /* Clear mask */
				&args->exception_mask /* Status mask */);
		break;
	case KFD_IOC_DBG_TRAP_GET_QUEUE_SNAPSHOT:
		r = pqm_get_queue_snapshot(&target->pqm,
					   exception_mask, /* Clear mask  */
					   (void __user *)args->ptr,
					   args->data1);

		args->data1 = r < 0 ? 0 : r;
		if (r > 0)
			r = 0;
		break;
	case KFD_IOC_DBG_TRAP_GET_VERSION:
		args->data1 = KFD_IOCTL_DBG_MAJOR_VERSION;
		args->data2 = KFD_IOCTL_DBG_MINOR_VERSION;
		break;
	case KFD_IOC_DBG_TRAP_CLEAR_ADDRESS_WATCH:
		r = kfd_dbg_trap_clear_address_watch(target, data1);
		break;
	case KFD_IOC_DBG_TRAP_SET_ADDRESS_WATCH:
		r = kfd_dbg_trap_set_address_watch(target,
				args->ptr, /* watch address */
				data3,     /* watch address mask */
				&data1,    /* watch id */
				data2);    /* watch mode */
		if (r)
			goto unlock_out;

		/* Save the watch point ID for the caller */
		args->data1 = data1;
		break;
	case KFD_IOC_DBG_TRAP_SET_PRECISE_MEM_OPS:
		r = kfd_dbg_trap_set_precise_mem_ops(target, data1);
		break;
	case KFD_IOC_DBG_TRAP_QUERY_EXCEPTION_INFO:
		r = kfd_dbg_trap_query_exception_info(target,
				data1,
				data2,
				data3 == 1,
				(void __user *) args->ptr, /* info */
				&args->data4);      /* info size */
		break;
	case KFD_IOC_DBG_TRAP_DEVICE_SNAPSHOT:
		r = kfd_dbg_trap_device_snapshot(target,
				exception_mask,
				(void __user *) args->ptr,
				&args->data1);
		break;
	case KFD_IOC_DBG_TRAP_RUNTIME_ENABLE:
		if (data1)
			r = kfd_dbg_runtime_enable(target,
				args->ptr,
				data2);
		else
			r = kfd_dbg_runtime_disable(target);
		break;
	case KFD_IOC_DBG_TRAP_SEND_RUNTIME_EVENT:
		r = kfd_dbg_send_exception_to_runtime(target,
						data1,
						data2,
						exception_mask);
		break;
	case KFD_IOC_DBG_TRAP_SET_EXCEPTIONS_ENABLED:
		kfd_dbg_set_enabled_debug_exception_mask(target, exception_mask);
		break;
	default:
		pr_err("Invalid option: %i\n", debug_trap_action);
		r = -EINVAL;
	}

unlock_out:
	mutex_unlock(&target->mutex);

out:
	if (user_array && size_to_copy_to_user_array) {
		/* Copy the user array to userspace even on error. */
		if (copy_to_user((void __user *)args->ptr, user_array,
				sizeof(uint32_t) * size_to_copy_to_user_array))
			pr_err("copy_to_user failed\n");
	}

	if (thread)
		put_task_struct(thread);
	if (pid)
		put_pid(pid);

	if (target)
		kfd_unref_process(target);
	kfree(user_array);
	return r;
}

/* Handle requests for watching SMI events */
static int kfd_ioctl_smi_events(struct file *filep,
				struct kfd_process *p, void *data)
{
	struct kfd_ioctl_smi_events_args *args = data;
	struct kfd_process_device *pdd;

	mutex_lock(&p->mutex);

	pdd = kfd_process_device_data_by_id(p, args->gpuid);
	mutex_unlock(&p->mutex);
	if (!pdd)
		return -EINVAL;

	return kfd_smi_event_open(pdd->dev, &args->anon_fd);
}

static int kfd_ioctl_set_xnack_mode(struct file *filep,
				    struct kfd_process *p, void *data)
{
	struct kfd_ioctl_set_xnack_mode_args *args = data;
	int r = 0;

	mutex_lock(&p->mutex);
	if (args->xnack_enabled >= 0) {
		if (!list_empty(&p->pqm.queues)) {
			pr_debug("Process has user queues running\n");
			mutex_unlock(&p->mutex);
			return -EBUSY;
		}
		if (args->xnack_enabled && !kfd_process_xnack_mode(p, true))
			r = -EPERM;
		else
			p->xnack_enabled = args->xnack_enabled;
	} else {
		args->xnack_enabled = p->xnack_enabled;
	}
	mutex_unlock(&p->mutex);

	return r;
}

#if IS_ENABLED(CONFIG_HSA_AMD_SVM)
static int kfd_ioctl_svm(struct file *filep, struct kfd_process *p, void *data)
{
	struct kfd_ioctl_svm_args *args = data;
	int r = 0;

	pr_debug("start 0x%llx size 0x%llx op 0x%x nattr 0x%x\n",
		 args->start_addr, args->size, args->op, args->nattr);

	if ((args->start_addr & ~PAGE_MASK) || (args->size & ~PAGE_MASK))
		return -EINVAL;
	if (!args->start_addr || !args->size)
		return -EINVAL;

	r = svm_ioctl(p, args->op, args->start_addr, args->size, args->nattr,
		      args->attrs);

	return r;
}
#else
static int kfd_ioctl_svm(struct file *filep, struct kfd_process *p, void *data)
{
	return -EPERM;
}
#endif

static int kfd_ioctl_rlc_spm(struct file *filep,
				   struct kfd_process *p, void *data)
{
	return kfd_rlc_spm(p, data);
}

static int criu_checkpoint_process(struct kfd_process *p,
			     uint8_t __user *user_priv_data,
			     uint64_t *priv_offset)
{
	struct kfd_criu_process_priv_data process_priv;
	int ret;

	memset(&process_priv, 0, sizeof(process_priv));

	process_priv.version = KFD_CRIU_PRIV_VERSION;
	/* For CR, we don't consider negative xnack mode which is used for
	 * querying without changing it, here 0 simply means disabled and 1
	 * means enabled so retry for finding a valid PTE.
	 */
	process_priv.xnack_mode = p->xnack_enabled ? 1 : 0;

	ret = copy_to_user(user_priv_data + *priv_offset,
				&process_priv, sizeof(process_priv));

	if (ret) {
		pr_err("Failed to copy process information to user\n");
		ret = -EFAULT;
	}

	*priv_offset += sizeof(process_priv);
	return ret;
}

static int criu_checkpoint_devices(struct kfd_process *p,
			     uint32_t num_devices,
			     uint8_t __user *user_addr,
			     uint8_t __user *user_priv_data,
			     uint64_t *priv_offset)
{
	struct kfd_criu_device_priv_data *device_priv = NULL;
	struct kfd_criu_device_bucket *device_buckets = NULL;
	int ret = 0, i;

	device_buckets = kvzalloc(num_devices * sizeof(*device_buckets), GFP_KERNEL);
	if (!device_buckets) {
		ret = -ENOMEM;
		goto exit;
	}

	device_priv = kvzalloc(num_devices * sizeof(*device_priv), GFP_KERNEL);
	if (!device_priv) {
		ret = -ENOMEM;
		goto exit;
	}

	for (i = 0; i < num_devices; i++) {
		struct kfd_process_device *pdd = p->pdds[i];

		device_buckets[i].user_gpu_id = pdd->user_gpu_id;
		device_buckets[i].actual_gpu_id = pdd->dev->id;

		/*
		 * priv_data does not contain useful information for now and is reserved for
		 * future use, so we do not set its contents.
		 */
	}

	ret = copy_to_user(user_addr, device_buckets, num_devices * sizeof(*device_buckets));
	if (ret) {
		pr_err("Failed to copy device information to user\n");
		ret = -EFAULT;
		goto exit;
	}

	ret = copy_to_user(user_priv_data + *priv_offset,
			   device_priv,
			   num_devices * sizeof(*device_priv));
	if (ret) {
		pr_err("Failed to copy device information to user\n");
		ret = -EFAULT;
	}
	*priv_offset += num_devices * sizeof(*device_priv);

exit:
	kvfree(device_buckets);
	kvfree(device_priv);
	return ret;
}

static uint32_t get_process_num_bos(struct kfd_process *p)
{
	uint32_t num_of_bos = 0;
	int i;

	/* Run over all PDDs of the process */
	for (i = 0; i < p->n_pdds; i++) {
		struct kfd_process_device *pdd = p->pdds[i];
		struct kfd_bo *buf_obj;
		int id;

		idr_for_each_entry(&pdd->alloc_idr, buf_obj, id) {
			struct kgd_mem *kgd_mem = (struct kgd_mem *)buf_obj->mem;

			if ((uint64_t)kgd_mem->va > pdd->gpuvm_base)
				num_of_bos++;
		}
	}
	return num_of_bos;
}

static int criu_get_prime_handle(struct drm_gem_object *gobj, int flags,
				      u32 *shared_fd)
{
	struct dma_buf *dmabuf;
	int ret;

#ifdef HAVE_DRM_DRV_GEM_PRIME_EXPORT_PI
	dmabuf = amdgpu_gem_prime_export(gobj, flags);
#else
	dmabuf = amdgpu_gem_prime_export(gobj->dev, gobj, flags);
#endif
	if (IS_ERR(dmabuf)) {
		ret = PTR_ERR(dmabuf);
		pr_err("dmabuf export failed for the BO\n");
		return ret;
	}

	ret = dma_buf_fd(dmabuf, flags);
	if (ret < 0) {
		pr_err("dmabuf create fd failed, ret:%d\n", ret);
		goto out_free_dmabuf;
	}

	*shared_fd = ret;
	return 0;

out_free_dmabuf:
	dma_buf_put(dmabuf);
	return ret;
}

static int criu_checkpoint_bos(struct kfd_process *p,
			       uint32_t num_bos,
			       uint8_t __user *user_bos,
			       uint8_t __user *user_priv_data,
			       uint64_t *priv_offset)
{
	struct kfd_criu_bo_bucket *bo_buckets;
	struct kfd_criu_bo_priv_data *bo_privs;
	int ret = 0, pdd_index, bo_index = 0, id;
	struct kfd_bo *buf_obj;

	bo_buckets = kvzalloc(num_bos * sizeof(*bo_buckets), GFP_KERNEL);
	if (!bo_buckets)
		return -ENOMEM;

	bo_privs = kvzalloc(num_bos * sizeof(*bo_privs), GFP_KERNEL);
	if (!bo_privs) {
		ret = -ENOMEM;
		goto exit;
	}

	for (pdd_index = 0; pdd_index < p->n_pdds; pdd_index++) {
		struct kfd_process_device *pdd = p->pdds[pdd_index];
		struct amdgpu_bo *dumper_bo;
		struct kgd_mem *kgd_mem;

		idr_for_each_entry(&pdd->alloc_idr, buf_obj, id) {
			struct kfd_criu_bo_bucket *bo_bucket;
			struct kfd_criu_bo_priv_data *bo_priv;
			int i, dev_idx = 0;

			if (!buf_obj) {
				ret = -ENOMEM;
				goto exit;
			}

			kgd_mem = (struct kgd_mem *)buf_obj->mem;
			dumper_bo = kgd_mem->bo;

			if ((uint64_t)kgd_mem->va <= pdd->gpuvm_base)
				continue;

			bo_bucket = &bo_buckets[bo_index];
			bo_priv = &bo_privs[bo_index];

			bo_bucket->gpu_id = pdd->user_gpu_id;
			bo_bucket->addr = (uint64_t)kgd_mem->va;
			bo_bucket->size = amdgpu_bo_size(dumper_bo);
			bo_bucket->alloc_flags = (uint32_t)kgd_mem->alloc_flags;
			bo_priv->idr_handle = id;

			if (bo_bucket->alloc_flags & KFD_IOC_ALLOC_MEM_FLAGS_USERPTR) {
				ret = amdgpu_ttm_tt_get_userptr(&dumper_bo->tbo,
								&bo_priv->user_addr);
				if (ret) {
					pr_err("Failed to obtain user address for user-pointer bo\n");
					goto exit;
				}
			}
			if (bo_bucket->alloc_flags
			    & (KFD_IOC_ALLOC_MEM_FLAGS_VRAM | KFD_IOC_ALLOC_MEM_FLAGS_GTT)) {
				ret = criu_get_prime_handle(&dumper_bo->tbo.base,
						bo_bucket->alloc_flags &
						KFD_IOC_ALLOC_MEM_FLAGS_WRITABLE ? DRM_RDWR : 0,
						&bo_bucket->dmabuf_fd);
				if (ret)
					goto exit;
			} else {
				bo_bucket->dmabuf_fd = KFD_INVALID_FD;
			}
			if (bo_bucket->alloc_flags & KFD_IOC_ALLOC_MEM_FLAGS_DOORBELL)
				bo_bucket->offset = KFD_MMAP_TYPE_DOORBELL |
					KFD_MMAP_GPU_ID(pdd->dev->id);
			else if (bo_bucket->alloc_flags &
				KFD_IOC_ALLOC_MEM_FLAGS_MMIO_REMAP)
				bo_bucket->offset = KFD_MMAP_TYPE_MMIO |
					KFD_MMAP_GPU_ID(pdd->dev->id);
			else
				bo_bucket->offset = amdgpu_bo_mmap_offset(dumper_bo);

			for (i = 0; i < p->n_pdds; i++) {
				if (amdgpu_amdkfd_bo_mapped_to_dev(p->pdds[i]->dev->adev, kgd_mem))
					bo_priv->mapped_gpuids[dev_idx++] = p->pdds[i]->user_gpu_id;
			}

			pr_debug("bo_size = 0x%llx, bo_addr = 0x%llx bo_offset = 0x%llx\n"
					"gpu_id = 0x%x alloc_flags = 0x%x idr_handle = 0x%x",
					bo_bucket->size,
					bo_bucket->addr,
					bo_bucket->offset,
					bo_bucket->gpu_id,
					bo_bucket->alloc_flags,
					bo_priv->idr_handle);
			bo_index++;
		}
	}

	ret = copy_to_user(user_bos, bo_buckets, num_bos * sizeof(*bo_buckets));
	if (ret) {
		pr_err("Failed to copy BO information to user\n");
		ret = -EFAULT;
		goto exit;
	}

	ret = copy_to_user(user_priv_data + *priv_offset, bo_privs, num_bos * sizeof(*bo_privs));
	if (ret) {
		pr_err("Failed to copy BO priv information to user\n");
		ret = -EFAULT;
		goto exit;
	}

	*priv_offset += num_bos * sizeof(*bo_privs);

exit:
	while (ret && bo_index--) {
		if (bo_buckets[bo_index].alloc_flags
		    & (KFD_IOC_ALLOC_MEM_FLAGS_VRAM | KFD_IOC_ALLOC_MEM_FLAGS_GTT))
			close_fd(bo_buckets[bo_index].dmabuf_fd);
	}

	kvfree(bo_buckets);
	kvfree(bo_privs);
	return ret;
}

static int criu_get_process_object_info(struct kfd_process *p,
					uint32_t *num_devices,
					uint32_t *num_bos,
					uint32_t *num_objects,
					uint64_t *objs_priv_size)
{
	uint64_t queues_priv_data_size, svm_priv_data_size, priv_size;
	uint32_t num_queues, num_events, num_svm_ranges;
	int ret;

	*num_devices = p->n_pdds;
	*num_bos = get_process_num_bos(p);

	ret = kfd_process_get_queue_info(p, &num_queues, &queues_priv_data_size);
	if (ret)
		return ret;

	num_events = kfd_get_num_events(p);

	ret = svm_range_get_info(p, &num_svm_ranges, &svm_priv_data_size);
	if (ret)
		return ret;

	*num_objects = num_queues + num_events + num_svm_ranges;

	if (objs_priv_size) {
		priv_size = sizeof(struct kfd_criu_process_priv_data);
		priv_size += *num_devices * sizeof(struct kfd_criu_device_priv_data);
		priv_size += *num_bos * sizeof(struct kfd_criu_bo_priv_data);
		priv_size += queues_priv_data_size;
		priv_size += num_events * sizeof(struct kfd_criu_event_priv_data);
		priv_size += svm_priv_data_size;
		*objs_priv_size = priv_size;
	}
	return 0;
}

static int criu_checkpoint(struct file *filep,
			   struct kfd_process *p,
			   struct kfd_ioctl_criu_args *args)
{
	int ret;
	uint32_t num_devices, num_bos, num_objects;
	uint64_t priv_size, priv_offset = 0;

	if (!args->devices || !args->bos || !args->priv_data)
		return -EINVAL;

	mutex_lock(&p->mutex);

	if (!p->n_pdds) {
		pr_err("No pdd for given process\n");
		ret = -ENODEV;
		goto exit_unlock;
	}

	/* Confirm all process queues are evicted */
	if (!p->queues_paused) {
		pr_err("Cannot dump process when queues are not in evicted state\n");
		/* CRIU plugin did not call op PROCESS_INFO before checkpointing */
		ret = -EINVAL;
		goto exit_unlock;
	}

	ret = criu_get_process_object_info(p, &num_devices, &num_bos, &num_objects, &priv_size);
	if (ret)
		goto exit_unlock;

	if (num_devices != args->num_devices ||
	    num_bos != args->num_bos ||
	    num_objects != args->num_objects ||
	    priv_size != args->priv_data_size) {

		ret = -EINVAL;
		goto exit_unlock;
	}

	/* each function will store private data inside priv_data and adjust priv_offset */
	ret = criu_checkpoint_process(p, (uint8_t __user *)args->priv_data, &priv_offset);
	if (ret)
		goto exit_unlock;

	ret = criu_checkpoint_devices(p, num_devices, (uint8_t __user *)args->devices,
				(uint8_t __user *)args->priv_data, &priv_offset);
	if (ret)
		goto exit_unlock;

	ret = criu_checkpoint_bos(p, num_bos, (uint8_t __user *)args->bos,
			    (uint8_t __user *)args->priv_data, &priv_offset);
	if (ret)
		goto exit_unlock;

	if (num_objects) {
		ret = kfd_criu_checkpoint_queues(p, (uint8_t __user *)args->priv_data,
						 &priv_offset);
		if (ret)
			goto close_bo_fds;

		ret = kfd_criu_checkpoint_events(p, (uint8_t __user *)args->priv_data,
						 &priv_offset);
		if (ret)
			goto close_bo_fds;

		ret = kfd_criu_checkpoint_svm(p, (uint8_t __user *)args->priv_data, &priv_offset);
		if (ret)
			goto close_bo_fds;
	}

close_bo_fds:
	if (ret) {
		/* If IOCTL returns err, user assumes all FDs opened in criu_dump_bos are closed */
		uint32_t i;
		struct kfd_criu_bo_bucket *bo_buckets = (struct kfd_criu_bo_bucket *) args->bos;

		for (i = 0; i < num_bos; i++) {
			if (bo_buckets[i].alloc_flags & KFD_IOC_ALLOC_MEM_FLAGS_VRAM)
				close_fd(bo_buckets[i].dmabuf_fd);
		}
	}

exit_unlock:
	mutex_unlock(&p->mutex);
	if (ret)
		pr_err("Failed to dump CRIU ret:%d\n", ret);
	else
		pr_debug("CRIU dump ret:%d\n", ret);

	return ret;
}

static int criu_restore_process(struct kfd_process *p,
				struct kfd_ioctl_criu_args *args,
				uint64_t *priv_offset,
				uint64_t max_priv_data_size)
{
	int ret = 0;
	struct kfd_criu_process_priv_data process_priv;

	if (*priv_offset + sizeof(process_priv) > max_priv_data_size)
		return -EINVAL;

	ret = copy_from_user(&process_priv,
				(void __user *)(args->priv_data + *priv_offset),
				sizeof(process_priv));
	if (ret) {
		pr_err("Failed to copy process private information from user\n");
		ret = -EFAULT;
		goto exit;
	}
	*priv_offset += sizeof(process_priv);

	if (process_priv.version != KFD_CRIU_PRIV_VERSION) {
		pr_err("Invalid CRIU API version (checkpointed:%d current:%d)\n",
			process_priv.version, KFD_CRIU_PRIV_VERSION);
		return -EINVAL;
	}

	pr_debug("Setting XNACK mode\n");
	if (process_priv.xnack_mode && !kfd_process_xnack_mode(p, true)) {
		pr_err("xnack mode cannot be set\n");
		ret = -EPERM;
		goto exit;
	} else {
		pr_debug("set xnack mode: %d\n", process_priv.xnack_mode);
		p->xnack_enabled = process_priv.xnack_mode;
	}

exit:
	return ret;
}

static int criu_restore_devices(struct kfd_process *p,
				struct kfd_ioctl_criu_args *args,
				uint64_t *priv_offset,
				uint64_t max_priv_data_size)
{
	struct kfd_criu_device_bucket *device_buckets;
	struct kfd_criu_device_priv_data *device_privs;
	int ret = 0;
	uint32_t i;

	if (args->num_devices != p->n_pdds)
		return -EINVAL;

	if (*priv_offset + (args->num_devices * sizeof(*device_privs)) > max_priv_data_size)
		return -EINVAL;

	device_buckets = kmalloc_array(args->num_devices, sizeof(*device_buckets), GFP_KERNEL);
	if (!device_buckets)
		return -ENOMEM;

	ret = copy_from_user(device_buckets, (void __user *)args->devices,
				args->num_devices * sizeof(*device_buckets));
	if (ret) {
		pr_err("Failed to copy devices buckets from user\n");
		ret = -EFAULT;
		goto exit;
	}

	for (i = 0; i < args->num_devices; i++) {
		struct kfd_dev *dev;
		struct kfd_process_device *pdd;
		struct file *drm_file;

		/* device private data is not currently used */

		if (!device_buckets[i].user_gpu_id) {
			pr_err("Invalid user gpu_id\n");
			ret = -EINVAL;
			goto exit;
		}

		dev = kfd_device_by_id(device_buckets[i].actual_gpu_id);
		if (!dev) {
			pr_err("Failed to find device with gpu_id = %x\n",
				device_buckets[i].actual_gpu_id);
			ret = -EINVAL;
			goto exit;
		}

		pdd = kfd_get_process_device_data(dev, p);
		if (!pdd) {
			pr_err("Failed to get pdd for gpu_id = %x\n",
					device_buckets[i].actual_gpu_id);
			ret = -EINVAL;
			goto exit;
		}
		pdd->user_gpu_id = device_buckets[i].user_gpu_id;

		drm_file = fget(device_buckets[i].drm_fd);
		if (!drm_file) {
			pr_err("Invalid render node file descriptor sent from plugin (%d)\n",
				device_buckets[i].drm_fd);
			ret = -EINVAL;
			goto exit;
		}

		if (pdd->drm_file) {
			ret = -EINVAL;
			goto exit;
		}

		/* create the vm using render nodes for kfd pdd */
		if (kfd_process_device_init_vm(pdd, drm_file)) {
			pr_err("could not init vm for given pdd\n");
			/* On success, the PDD keeps the drm_file reference */
			fput(drm_file);
			ret = -EINVAL;
			goto exit;
		}
		/*
		 * pdd now already has the vm bound to render node so below api won't create a new
		 * exclusive kfd mapping but use existing one with renderDXXX but is still needed
		 * for iommu v2 binding  and runtime pm.
		 */
		pdd = kfd_bind_process_to_device(dev, p);
		if (IS_ERR(pdd)) {
			ret = PTR_ERR(pdd);
			goto exit;
		}
	}

	/*
	 * We are not copying device private data from user as we are not using the data for now,
	 * but we still adjust for its private data.
	 */
	*priv_offset += args->num_devices * sizeof(*device_privs);

exit:
	kfree(device_buckets);
	return ret;
}

static int criu_restore_memory_of_gpu(struct kfd_process_device *pdd,
				      struct kfd_criu_bo_bucket *bo_bucket,
				      struct kfd_criu_bo_priv_data *bo_priv,
				      struct kgd_mem **kgd_mem)
{
	int idr_handle;
	uint64_t cpuva = 0;
	int ret;
	const bool criu_resume = true;
	unsigned int mem_type = 0;
	uint64_t offset;

	if (bo_bucket->alloc_flags & KFD_IOC_ALLOC_MEM_FLAGS_DOORBELL) {
		if (bo_bucket->size != kfd_doorbell_process_slice(pdd->dev))
			return -EINVAL;

		offset = kfd_get_process_doorbells(pdd);
	} else if (bo_bucket->alloc_flags & KFD_IOC_ALLOC_MEM_FLAGS_MMIO_REMAP) {
		/* MMIO BOs need remapped bus address */
		if (bo_bucket->size != PAGE_SIZE) {
			pr_err("Invalid page size\n");
			return -EINVAL;
		}
		offset = pdd->dev->adev->rmmio_remap.bus_addr;
		if (!offset) {
			pr_err("amdgpu_amdkfd_get_mmio_remap_phys_addr failed\n");
			return -ENOMEM;
		}
	} else if (bo_bucket->alloc_flags & KFD_IOC_ALLOC_MEM_FLAGS_USERPTR) {
		cpuva = bo_priv->user_addr;
		offset = bo_priv->user_addr;
	}

	/* Create the BO */
	ret = amdgpu_amdkfd_gpuvm_alloc_memory_of_gpu(pdd->dev->adev, bo_bucket->addr,
						      bo_bucket->size, pdd->drm_priv, NULL, kgd_mem,
						      &offset, bo_bucket->alloc_flags, criu_resume);
	if (ret) {
		pr_err("Could not create the BO\n");
		return ret;
	}

	pr_debug("New BO created: size:0x%llx addr:0x%llx offset:0x%llx\n",
		 bo_bucket->size, bo_bucket->addr, offset);

	/* Restore previous IDR handle */
	mem_type = bo_bucket->alloc_flags & (KFD_IOC_ALLOC_MEM_FLAGS_VRAM |
					     KFD_IOC_ALLOC_MEM_FLAGS_GTT |
					     KFD_IOC_ALLOC_MEM_FLAGS_USERPTR |
					     KFD_IOC_ALLOC_MEM_FLAGS_DOORBELL |
					     KFD_IOC_ALLOC_MEM_FLAGS_MMIO_REMAP);

	idr_handle = kfd_process_device_create_obj_handle(pdd, *kgd_mem, bo_bucket->addr,
							  bo_bucket->size, cpuva, mem_type,
							  bo_priv->idr_handle);
	if (idr_handle < 0) {
		pr_err("Could not allocate idr\n");

		amdgpu_amdkfd_gpuvm_free_memory_of_gpu(pdd->dev->adev, *kgd_mem, pdd->drm_priv,
						       NULL);
		return -ENOMEM;
	}

	if (bo_bucket->alloc_flags & KFD_IOC_ALLOC_MEM_FLAGS_DOORBELL)
		bo_bucket->restored_offset = KFD_MMAP_TYPE_DOORBELL | KFD_MMAP_GPU_ID(pdd->dev->id);
	if (bo_bucket->alloc_flags & KFD_IOC_ALLOC_MEM_FLAGS_MMIO_REMAP) {
		bo_bucket->restored_offset = KFD_MMAP_TYPE_MMIO | KFD_MMAP_GPU_ID(pdd->dev->id);
	} else if (bo_bucket->alloc_flags & KFD_IOC_ALLOC_MEM_FLAGS_GTT) {
		bo_bucket->restored_offset = offset;
	} else if (bo_bucket->alloc_flags & KFD_IOC_ALLOC_MEM_FLAGS_VRAM) {
		bo_bucket->restored_offset = offset;
		/* Update the VRAM usage count */
		WRITE_ONCE(pdd->vram_usage, pdd->vram_usage + bo_bucket->size);
	}

	return 0;
}

static int criu_restore_bo(struct kfd_process *p,
			   struct kfd_criu_bo_bucket *bo_bucket,
			   struct kfd_criu_bo_priv_data *bo_priv)
{
	struct kfd_process_device *pdd;
	struct kgd_mem *kgd_mem;
	int ret;
	int j;

	pr_debug("Restoring BO size:0x%llx addr:0x%llx gpu_id:0x%x flags:0x%x idr_handle:0x%x\n",
		 bo_bucket->size, bo_bucket->addr, bo_bucket->gpu_id, bo_bucket->alloc_flags,
		 bo_priv->idr_handle);

	pdd = kfd_process_device_data_by_id(p, bo_bucket->gpu_id);
	if (!pdd) {
		pr_err("Failed to get pdd\n");
		return -ENODEV;
	}

	ret = criu_restore_memory_of_gpu(pdd, bo_bucket, bo_priv, &kgd_mem);
	if (ret)
		return ret;

	/* now map these BOs to GPU/s */
	for (j = 0; j < p->n_pdds; j++) {
		struct kfd_dev *peer;
		struct kfd_process_device *peer_pdd;

		if (!bo_priv->mapped_gpuids[j])
			break;

		peer_pdd = kfd_process_device_data_by_id(p, bo_priv->mapped_gpuids[j]);
		if (IS_ERR(peer_pdd))
			return -EINVAL;

		peer = peer_pdd->dev;

		peer_pdd = kfd_bind_process_to_device(peer, p);
		if (IS_ERR(peer_pdd))
			return PTR_ERR(peer_pdd);

		ret = amdgpu_amdkfd_gpuvm_map_memory_to_gpu(peer->adev, kgd_mem, peer_pdd->drm_priv,
							    NULL);
		if (ret) {
			pr_err("Failed to map to gpu %d/%d\n", j, p->n_pdds);
			return ret;
		}
	}

	pr_debug("map memory was successful for the BO\n");
	/* create the dmabuf object and export the bo */
	if (bo_bucket->alloc_flags
	    & (KFD_IOC_ALLOC_MEM_FLAGS_VRAM | KFD_IOC_ALLOC_MEM_FLAGS_GTT)) {
		ret = criu_get_prime_handle(&kgd_mem->bo->tbo.base, DRM_RDWR,
					    &bo_bucket->dmabuf_fd);
		if (ret)
			return ret;
	} else {
		bo_bucket->dmabuf_fd = KFD_INVALID_FD;
	}

	return 0;
}

static int criu_restore_bos(struct kfd_process *p,
			    struct kfd_ioctl_criu_args *args,
			    uint64_t *priv_offset,
			    uint64_t max_priv_data_size)
{
	struct kfd_criu_bo_bucket *bo_buckets = NULL;
	struct kfd_criu_bo_priv_data *bo_privs = NULL;
	int ret = 0;
	uint32_t i;

	if (*priv_offset + (args->num_bos * sizeof(*bo_privs)) > max_priv_data_size)
		return -EINVAL;

	/* Prevent MMU notifications until stage-4 IOCTL (CRIU_RESUME) is received */
	amdgpu_amdkfd_block_mmu_notifications(p->kgd_process_info);

	bo_buckets = kvmalloc_array(args->num_bos, sizeof(*bo_buckets), GFP_KERNEL);
	if (!bo_buckets)
		return -ENOMEM;

	ret = copy_from_user(bo_buckets, (void __user *)args->bos,
			     args->num_bos * sizeof(*bo_buckets));
	if (ret) {
		pr_err("Failed to copy BOs information from user\n");
		ret = -EFAULT;
		goto exit;
	}

	bo_privs = kvmalloc_array(args->num_bos, sizeof(*bo_privs), GFP_KERNEL);
	if (!bo_privs) {
		ret = -ENOMEM;
		goto exit;
	}

	ret = copy_from_user(bo_privs, (void __user *)args->priv_data + *priv_offset,
			     args->num_bos * sizeof(*bo_privs));
	if (ret) {
		pr_err("Failed to copy BOs information from user\n");
		ret = -EFAULT;
		goto exit;
	}
	*priv_offset += args->num_bos * sizeof(*bo_privs);

	/* Create and map new BOs */
	for (i = 0; i < args->num_bos; i++) {
		ret = criu_restore_bo(p, &bo_buckets[i], &bo_privs[i]);
		if (ret) {
			pr_debug("Failed to restore BO[%d] ret%d\n", i, ret);
			goto exit;
		}
	} /* done */

	/* Copy only the buckets back so user can read bo_buckets[N].restored_offset */
	ret = copy_to_user((void __user *)args->bos,
				bo_buckets,
				(args->num_bos * sizeof(*bo_buckets)));
	if (ret)
		ret = -EFAULT;

exit:
	while (ret && i--) {
		if (bo_buckets[i].alloc_flags
		    & (KFD_IOC_ALLOC_MEM_FLAGS_VRAM | KFD_IOC_ALLOC_MEM_FLAGS_GTT))
			close_fd(bo_buckets[i].dmabuf_fd);
	}
	kvfree(bo_buckets);
	kvfree(bo_privs);
	return ret;
}

static int criu_restore_objects(struct file *filep,
				struct kfd_process *p,
				struct kfd_ioctl_criu_args *args,
				uint64_t *priv_offset,
				uint64_t max_priv_data_size)
{
	int ret = 0;
	uint32_t i;

	BUILD_BUG_ON(offsetof(struct kfd_criu_queue_priv_data, object_type));
	BUILD_BUG_ON(offsetof(struct kfd_criu_event_priv_data, object_type));
	BUILD_BUG_ON(offsetof(struct kfd_criu_svm_range_priv_data, object_type));

	for (i = 0; i < args->num_objects; i++) {
		uint32_t object_type;

		if (*priv_offset + sizeof(object_type) > max_priv_data_size) {
			pr_err("Invalid private data size\n");
			return -EINVAL;
		}

		ret = get_user(object_type, (uint32_t __user *)(args->priv_data + *priv_offset));
		if (ret) {
			pr_err("Failed to copy private information from user\n");
			goto exit;
		}

		switch (object_type) {
		case KFD_CRIU_OBJECT_TYPE_QUEUE:
			ret = kfd_criu_restore_queue(p, (uint8_t __user *)args->priv_data,
						     priv_offset, max_priv_data_size);
			if (ret)
				goto exit;
			break;
		case KFD_CRIU_OBJECT_TYPE_EVENT:
			ret = kfd_criu_restore_event(filep, p, (uint8_t __user *)args->priv_data,
						     priv_offset, max_priv_data_size);
			if (ret)
				goto exit;
			break;
		case KFD_CRIU_OBJECT_TYPE_SVM_RANGE:
			ret = kfd_criu_restore_svm(p, (uint8_t __user *)args->priv_data,
						     priv_offset, max_priv_data_size);
			if (ret)
				goto exit;
			break;
		default:
			pr_err("Invalid object type:%u at index:%d\n", object_type, i);
			ret = -EINVAL;
			goto exit;
		}
	}
exit:
	return ret;
}

static int criu_restore(struct file *filep,
			struct kfd_process *p,
			struct kfd_ioctl_criu_args *args)
{
	uint64_t priv_offset = 0;
	int ret = 0;

	pr_debug("CRIU restore (num_devices:%u num_bos:%u num_objects:%u priv_data_size:%llu)\n",
		 args->num_devices, args->num_bos, args->num_objects, args->priv_data_size);

	if (!args->bos || !args->devices || !args->priv_data || !args->priv_data_size ||
	    !args->num_devices || !args->num_bos)
		return -EINVAL;

	mutex_lock(&p->mutex);

	/*
	 * Set the process to evicted state to avoid running any new queues before all the memory
	 * mappings are ready.
	 */
	ret = kfd_process_evict_queues(p, false);
	if (ret)
		goto exit_unlock;

	/* Each function will adjust priv_offset based on how many bytes they consumed */
	ret = criu_restore_process(p, args, &priv_offset, args->priv_data_size);
	if (ret)
		goto exit_unlock;

	ret = criu_restore_devices(p, args, &priv_offset, args->priv_data_size);
	if (ret)
		goto exit_unlock;

	ret = criu_restore_bos(p, args, &priv_offset, args->priv_data_size);
	if (ret)
		goto exit_unlock;

	ret = criu_restore_objects(filep, p, args, &priv_offset, args->priv_data_size);
	if (ret)
		goto exit_unlock;

	if (priv_offset != args->priv_data_size) {
		pr_err("Invalid private data size\n");
		ret = -EINVAL;
	}

exit_unlock:
	mutex_unlock(&p->mutex);
	if (ret)
		pr_err("Failed to restore CRIU ret:%d\n", ret);
	else
		pr_debug("CRIU restore successful\n");

	return ret;
}

static int criu_unpause(struct file *filep,
			struct kfd_process *p,
			struct kfd_ioctl_criu_args *args)
{
	int ret;

	mutex_lock(&p->mutex);

	if (!p->queues_paused) {
		mutex_unlock(&p->mutex);
		return -EINVAL;
	}

	ret = kfd_process_restore_queues(p);
	if (ret)
		pr_err("Failed to unpause queues ret:%d\n", ret);
	else
		p->queues_paused = false;

	mutex_unlock(&p->mutex);

	return ret;
}

static int criu_resume(struct file *filep,
			struct kfd_process *p,
			struct kfd_ioctl_criu_args *args)
{
	struct kfd_process *target = NULL;
	struct pid *pid = NULL;
	int ret = 0;

	pr_debug("Inside %s, target pid for criu restore: %d\n", __func__,
		 args->pid);

	pid = find_get_pid(args->pid);
	if (!pid) {
		pr_err("Cannot find pid info for %i\n", args->pid);
		return -ESRCH;
	}

	pr_debug("calling kfd_lookup_process_by_pid\n");
	target = kfd_lookup_process_by_pid(pid);

	put_pid(pid);

	if (!target) {
		pr_debug("Cannot find process info for %i\n", args->pid);
		return -ESRCH;
	}

	mutex_lock(&target->mutex);
	ret = kfd_criu_resume_svm(target);
	if (ret) {
		pr_err("kfd_criu_resume_svm failed for %i\n", args->pid);
		goto exit;
	}

	ret =  amdgpu_amdkfd_criu_resume(target->kgd_process_info);
	if (ret)
		pr_err("amdgpu_amdkfd_criu_resume failed for %i\n", args->pid);

exit:
	mutex_unlock(&target->mutex);

	kfd_unref_process(target);
	return ret;
}

static int criu_process_info(struct file *filep,
				struct kfd_process *p,
				struct kfd_ioctl_criu_args *args)
{
	int ret = 0;

	mutex_lock(&p->mutex);

	if (!p->n_pdds) {
		pr_err("No pdd for given process\n");
		ret = -ENODEV;
		goto err_unlock;
	}

	ret = kfd_process_evict_queues(p, false);
	if (ret)
		goto err_unlock;

	p->queues_paused = true;

	args->pid = task_pid_nr_ns(p->lead_thread,
					task_active_pid_ns(p->lead_thread));

	ret = criu_get_process_object_info(p, &args->num_devices, &args->num_bos,
					   &args->num_objects, &args->priv_data_size);
	if (ret)
		goto err_unlock;

	dev_dbg(kfd_device, "Num of devices:%u bos:%u objects:%u priv_data_size:%lld\n",
				args->num_devices, args->num_bos, args->num_objects,
				args->priv_data_size);

err_unlock:
	if (ret) {
		kfd_process_restore_queues(p);
		p->queues_paused = false;
	}
	mutex_unlock(&p->mutex);
	return ret;
}

static int kfd_ioctl_criu(struct file *filep, struct kfd_process *p, void *data)
{
	struct kfd_ioctl_criu_args *args = data;
	int ret;

	dev_dbg(kfd_device, "CRIU operation: %d\n", args->op);
	switch (args->op) {
	case KFD_CRIU_OP_PROCESS_INFO:
		ret = criu_process_info(filep, p, args);
		break;
	case KFD_CRIU_OP_CHECKPOINT:
		ret = criu_checkpoint(filep, p, args);
		break;
	case KFD_CRIU_OP_UNPAUSE:
		ret = criu_unpause(filep, p, args);
		break;
	case KFD_CRIU_OP_RESTORE:
		ret = criu_restore(filep, p, args);
		break;
	case KFD_CRIU_OP_RESUME:
		ret = criu_resume(filep, p, args);
		break;
	default:
		dev_dbg(kfd_device, "Unsupported CRIU operation:%d\n", args->op);
		ret = -EINVAL;
		break;
	}

	if (ret)
		dev_dbg(kfd_device, "CRIU operation:%d err:%d\n", args->op, ret);

	return ret;
}

#define AMDKFD_IOCTL_DEF(ioctl, _func, _flags) \
	[_IOC_NR(ioctl)] = {.cmd = ioctl, .func = _func, .flags = _flags, \
			    .cmd_drv = 0, .name = #ioctl}

/** Ioctl table */
static const struct amdkfd_ioctl_desc amdkfd_ioctls[] = {
	AMDKFD_IOCTL_DEF(AMDKFD_IOC_GET_VERSION,
			kfd_ioctl_get_version, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_CREATE_QUEUE,
			kfd_ioctl_create_queue, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_DESTROY_QUEUE,
			kfd_ioctl_destroy_queue, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_SET_MEMORY_POLICY,
			kfd_ioctl_set_memory_policy, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_GET_CLOCK_COUNTERS,
			kfd_ioctl_get_clock_counters, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_GET_PROCESS_APERTURES,
			kfd_ioctl_get_process_apertures, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_UPDATE_QUEUE,
			kfd_ioctl_update_queue, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_CREATE_EVENT,
			kfd_ioctl_create_event, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_DESTROY_EVENT,
			kfd_ioctl_destroy_event, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_SET_EVENT,
			kfd_ioctl_set_event, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_RESET_EVENT,
			kfd_ioctl_reset_event, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_WAIT_EVENTS,
			kfd_ioctl_wait_events, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_DBG_REGISTER_DEPRECATED,
			kfd_ioctl_dbg_register, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_DBG_UNREGISTER_DEPRECATED,
			kfd_ioctl_dbg_unregister, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_DBG_ADDRESS_WATCH_DEPRECATED,
			kfd_ioctl_dbg_address_watch, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_DBG_WAVE_CONTROL_DEPRECATED,
			kfd_ioctl_dbg_wave_control, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_SET_SCRATCH_BACKING_VA,
			kfd_ioctl_set_scratch_backing_va, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_GET_TILE_CONFIG,
			kfd_ioctl_get_tile_config, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_SET_TRAP_HANDLER,
			kfd_ioctl_set_trap_handler, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_GET_PROCESS_APERTURES_NEW,
			kfd_ioctl_get_process_apertures_new, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_ACQUIRE_VM,
			kfd_ioctl_acquire_vm, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_ALLOC_MEMORY_OF_GPU,
			kfd_ioctl_alloc_memory_of_gpu, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_FREE_MEMORY_OF_GPU,
			kfd_ioctl_free_memory_of_gpu, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_MAP_MEMORY_TO_GPU,
			kfd_ioctl_map_memory_to_gpu, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_UNMAP_MEMORY_FROM_GPU,
			kfd_ioctl_unmap_memory_from_gpu, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_SET_CU_MASK,
			kfd_ioctl_set_cu_mask, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_GET_QUEUE_WAVE_STATE,
			kfd_ioctl_get_queue_wave_state, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_GET_DMABUF_INFO,
				kfd_ioctl_get_dmabuf_info, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_IMPORT_DMABUF,
				kfd_ioctl_import_dmabuf, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_ALLOC_QUEUE_GWS,
			kfd_ioctl_alloc_queue_gws, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_SMI_EVENTS,
			kfd_ioctl_smi_events, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_SVM, kfd_ioctl_svm, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_SET_XNACK_MODE,
			kfd_ioctl_set_xnack_mode, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_CRIU_OP,
			kfd_ioctl_criu, KFD_IOC_FLAG_CHECKPOINT_RESTORE),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_IPC_IMPORT_HANDLE,
				kfd_ioctl_ipc_import_handle, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_IPC_EXPORT_HANDLE,
				kfd_ioctl_ipc_export_handle, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_CROSS_MEMORY_COPY,
				kfd_ioctl_cross_memory_copy, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_DBG_TRAP,
			kfd_ioctl_dbg_set_debug_trap, 0),

	AMDKFD_IOCTL_DEF(AMDKFD_IOC_RLC_SPM,
			kfd_ioctl_rlc_spm, 0),

};

static long kfd_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	struct kfd_process *process;
	amdkfd_ioctl_t *func;
	const struct amdkfd_ioctl_desc *ioctl = NULL;
	unsigned int nr = _IOC_NR(cmd);
	char stack_kdata[128];
	char *kdata = NULL;
	unsigned int usize, asize;
	int retcode = -EINVAL;
	bool ptrace_attached = false;

	if (((nr >= AMDKFD_COMMAND_START) && (nr < AMDKFD_COMMAND_END)) ||
	    ((nr >= AMDKFD_COMMAND_START_2) && (nr < AMDKFD_COMMAND_END_2))) {
		u32 amdkfd_size;

		ioctl = &amdkfd_ioctls[nr];

		amdkfd_size = _IOC_SIZE(ioctl->cmd);
		usize = asize = _IOC_SIZE(cmd);
		if (amdkfd_size > asize)
			asize = amdkfd_size;

		cmd = ioctl->cmd;
	} else
		goto err_i1;

	dev_dbg(kfd_device, "ioctl cmd 0x%x (#0x%x), arg 0x%lx\n", cmd, nr, arg);

	/* Get the process struct from the filep. Only the process
	 * that opened /dev/kfd can use the file descriptor. Child
	 * processes need to create their own KFD device context.
	 */
	process = filep->private_data;

	rcu_read_lock();
	if ((ioctl->flags & KFD_IOC_FLAG_CHECKPOINT_RESTORE) &&
	    ptrace_parent(process->lead_thread) == current)
		ptrace_attached = true;
	rcu_read_unlock();

	if (process->lead_thread != current->group_leader
	    && !ptrace_attached) {
		dev_dbg(kfd_device, "Using KFD FD in wrong process\n");
		retcode = -EBADF;
		goto err_i1;
	}

	/* Do not trust userspace, use our own definition */
	func = ioctl->func;

	if (unlikely(!func)) {
		dev_dbg(kfd_device, "no function\n");
		retcode = -EINVAL;
		goto err_i1;
	}

	/*
	 * Versions of docker shipped in Ubuntu 18.xx and 20.xx do not support
	 * CAP_CHECKPOINT_RESTORE, so we also allow access if CAP_SYS_ADMIN as CAP_SYS_ADMIN is a
	 * more priviledged access.
	 */
	if (unlikely(ioctl->flags & KFD_IOC_FLAG_CHECKPOINT_RESTORE)) {
		if (!capable(CAP_CHECKPOINT_RESTORE) &&
						!capable(CAP_SYS_ADMIN)) {
			retcode = -EACCES;
			goto err_i1;
		}
	}

	if (cmd & (IOC_IN | IOC_OUT)) {
		if (asize <= sizeof(stack_kdata)) {
			kdata = stack_kdata;
		} else {
			kdata = kmalloc(asize, GFP_KERNEL);
			if (!kdata) {
				retcode = -ENOMEM;
				goto err_i1;
			}
		}
		if (asize > usize)
			memset(kdata + usize, 0, asize - usize);
	}

	if (cmd & IOC_IN) {
		if (copy_from_user(kdata, (void __user *)arg, usize) != 0) {
			retcode = -EFAULT;
			goto err_i1;
		}
	} else if (cmd & IOC_OUT) {
		memset(kdata, 0, usize);
	}

	retcode = func(filep, process, kdata);

	if (cmd & IOC_OUT)
		if (copy_to_user((void __user *)arg, kdata, usize) != 0)
			retcode = -EFAULT;

err_i1:
	if (!ioctl)
		dev_dbg(kfd_device, "invalid ioctl: pid=%d, cmd=0x%02x, nr=0x%02x\n",
			  task_pid_nr(current), cmd, nr);

	if (kdata != stack_kdata)
		kfree(kdata);

	if (retcode)
		dev_dbg(kfd_device, "ioctl cmd (#0x%x), arg 0x%lx, ret = %d\n",
				nr, arg, retcode);

	return retcode;
}

static int kfd_mmio_mmap(struct kfd_dev *dev, struct kfd_process *process,
		      struct vm_area_struct *vma)
{
	phys_addr_t address;
	int ret;

	if (vma->vm_end - vma->vm_start != PAGE_SIZE)
		return -EINVAL;

	address = dev->adev->rmmio_remap.bus_addr;

	vma->vm_flags |= VM_IO | VM_DONTCOPY | VM_DONTEXPAND | VM_NORESERVE |
				VM_DONTDUMP | VM_PFNMAP;

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

	pr_debug("pasid 0x%x mapping mmio page\n"
		 "     target user address == 0x%08llX\n"
		 "     physical address    == 0x%08llX\n"
		 "     vm_flags            == 0x%04lX\n"
		 "     size                == 0x%04lX\n",
		 process->pasid, (unsigned long long) vma->vm_start,
		 address, vma->vm_flags, PAGE_SIZE);

	ret = io_remap_pfn_range(vma,
				vma->vm_start,
				address >> PAGE_SHIFT,
				PAGE_SIZE,
				vma->vm_page_prot);
	return ret;
}


static int kfd_mmap(struct file *filp, struct vm_area_struct *vma)
{
	struct kfd_process *process;
	struct kfd_dev *dev = NULL;
	unsigned long mmap_offset;
	unsigned int gpu_id;

	process = kfd_get_process(current);
	if (IS_ERR(process))
		return PTR_ERR(process);

	mmap_offset = vma->vm_pgoff << PAGE_SHIFT;
	gpu_id = KFD_MMAP_GET_GPU_ID(mmap_offset);
	if (gpu_id)
		dev = kfd_device_by_id(gpu_id);

	switch (mmap_offset & KFD_MMAP_TYPE_MASK) {
	case KFD_MMAP_TYPE_DOORBELL:
		if (!dev)
			return -ENODEV;
		return kfd_doorbell_mmap(dev, process, vma);

	case KFD_MMAP_TYPE_EVENTS:
		return kfd_event_mmap(process, vma);

	case KFD_MMAP_TYPE_RESERVED_MEM:
		if (!dev)
			return -ENODEV;
		return kfd_reserved_mem_mmap(dev, process, vma);
	case KFD_MMAP_TYPE_MMIO:
		if (!dev)
			return -ENODEV;
		return kfd_mmio_mmap(dev, process, vma);
	}

	return -EFAULT;
}
