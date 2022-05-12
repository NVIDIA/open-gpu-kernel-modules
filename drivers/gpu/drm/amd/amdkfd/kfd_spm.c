/*
 * Copyright 2020 Advanced Micro Devices, Inc.
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
#include "kfd_priv.h"
#include "amdgpu_amdkfd.h"
#include "amdgpu_irq.h"
#include "ivsrcid/gfx/irqsrcs_gfx_9_0.h"
#include "ivsrcid/ivsrcid_vislands30.h"
#include <linux/mmu_context.h> // for use_mm()
#include <linux/wait.h>

struct user_buf {
	uint64_t __user *user_addr;
	u32 ubufsize;
};

struct kfd_spm_cntr {
	struct user_buf ubuf;
	struct mutex spm_worker_mutex;
	u64    gpu_addr;
	u32    ring_size;
	u32    ring_mask;
	u32    ring_rptr;
	u32    size_copied;
	u32    has_data_loss;
	u32    *cpu_addr;
	void   *spm_obj;
	wait_queue_head_t spm_buf_wq;
	bool   has_user_buf;
	bool   is_user_buf_filled;
	bool   is_spm_started;
};

static int kfd_spm_data_copy(struct kfd_process_device *pdd, u32 size_to_copy)
{
	struct kfd_spm_cntr *spm = pdd->spm_cntr;
	uint64_t __user *user_address;
	uint64_t *ring_buf;
	u32 user_buf_space_left;
	int ret = 0;

	if (spm->ubuf.user_addr == NULL)
		return -EFAULT;

	user_address = (uint64_t *)((uint64_t)spm->ubuf.user_addr + spm->size_copied);
	ring_buf =  (uint64_t *)((uint64_t)spm->cpu_addr + spm->ring_rptr);

	if (user_address == NULL)
		return -EFAULT;

	user_buf_space_left = spm->ubuf.ubufsize - spm->size_copied;

	if (size_to_copy < user_buf_space_left) {
		ret = copy_to_user(user_address, ring_buf, size_to_copy);
		if (ret) {
			spm->has_data_loss = true;
			return -EFAULT;
		}
		spm->size_copied += size_to_copy;
		spm->ring_rptr += size_to_copy;
	} else {
		ret = copy_to_user(user_address, ring_buf, user_buf_space_left);
		if (ret) {
			spm->has_data_loss = true;
			return -EFAULT;
		}

		spm->size_copied = spm->ubuf.ubufsize;
		spm->ring_rptr += user_buf_space_left;
		WRITE_ONCE(spm->is_user_buf_filled, true);
		wake_up(&pdd->spm_cntr->spm_buf_wq);
	}

	return ret;
}

static int kfd_spm_read_ring_buffer(struct kfd_process_device *pdd)
{
	struct kfd_spm_cntr *spm = pdd->spm_cntr;
	u32 size_to_copy;
	int ret = 0;
	u32 ring_wptr;

	ring_wptr = READ_ONCE(spm->cpu_addr[0]) & spm->ring_mask;

	/* keep SPM ring buffer running */
	if (!spm->has_user_buf || spm->is_user_buf_filled) {
		spm->ring_rptr = ring_wptr;
		spm->has_data_loss = true;
		/* set flag due to there is no flag setup
		 * when read ring buffer timeout.
		 */
		if (!spm->is_user_buf_filled)
			spm->is_user_buf_filled = true;
		goto exit;
	}

	if (spm->ring_rptr == ring_wptr)
		goto exit;

	if ((spm->ring_rptr >= 0) &&  (spm->ring_rptr  < 0x20)) {
		/*
		 * First 8DW, only use for WritePtr, it is not Counter data
		 */
		spm->ring_rptr = 0x20;
	}

	if (ring_wptr > spm->ring_rptr) {
		size_to_copy = ring_wptr - spm->ring_rptr;
		ret = kfd_spm_data_copy(pdd, size_to_copy);
	} else {
		size_to_copy = spm->ring_size - spm->ring_rptr;
		ret = kfd_spm_data_copy(pdd, size_to_copy);

		/* correct counter start point */
		if (spm->ring_size == spm->ring_rptr) {
			if (ring_wptr == 0) {
				/* reset rptr to start point of ring buffer */
				spm->ring_rptr = ring_wptr;
				goto exit;
			}
			spm->ring_rptr = 0x20;
			size_to_copy = ring_wptr - spm->ring_rptr;
			if (!ret)
				ret = kfd_spm_data_copy(pdd, size_to_copy);
		}
	}

exit:
	amdgpu_amdkfd_rlc_spm_set_rdptr(pdd->dev->adev, spm->ring_rptr);
	return ret;
}

static void kfd_spm_work(struct work_struct *work)
{
	struct kfd_process_device *pdd = container_of(work, struct kfd_process_device, spm_work);
	struct mm_struct *mm = NULL; // referenced

	mm = get_task_mm(pdd->process->lead_thread);
	if (mm) {
		kthread_use_mm(mm);
		{ /* attach mm */
			mutex_lock(&pdd->spm_cntr->spm_worker_mutex);
			kfd_spm_read_ring_buffer(pdd);
			mutex_unlock(&pdd->spm_cntr->spm_worker_mutex);
		} /* detach mm */
		kthread_unuse_mm(mm);
		/* release the mm structure */
		mmput(mm);
	}
}

void kfd_spm_init_process_device(struct kfd_process_device *pdd)
{
	mutex_init(&pdd->spm_mutex);
	pdd->spm_cntr = NULL;
}

static int kfd_acquire_spm(struct kfd_process_device *pdd, struct amdgpu_device *adev)
{
	int ret = 0;

	mutex_lock(&pdd->spm_mutex);

	if (pdd->spm_cntr) {
		mutex_unlock(&pdd->spm_mutex);
		return -EINVAL;
	}

	pdd->spm_cntr = kzalloc(sizeof(struct kfd_spm_cntr), GFP_KERNEL);
	if (!pdd->spm_cntr) {
		mutex_unlock(&pdd->spm_mutex);
		return -ENOMEM;
	}
	mutex_unlock(&pdd->spm_mutex);

	/* git spm ring buffer 4M */
	pdd->spm_cntr->ring_size = order_base_2(4 * 1024 * 1024/4);
	pdd->spm_cntr->ring_size = (1 << pdd->spm_cntr->ring_size) * 4 - 0xff;
	pdd->spm_cntr->ring_mask = pdd->spm_cntr->ring_size - 1;
	pdd->spm_cntr->has_user_buf = false;

	ret = amdgpu_amdkfd_alloc_gtt_mem(adev,
			pdd->spm_cntr->ring_size, &pdd->spm_cntr->spm_obj,
			&pdd->spm_cntr->gpu_addr, (void *)&pdd->spm_cntr->cpu_addr,
			false, false);

	if (ret)
		goto alloc_gtt_mem_failure;

	ret =  amdgpu_amdkfd_rlc_spm_acquire(adev, pdd->drm_priv,
			pdd->spm_cntr->gpu_addr, pdd->spm_cntr->ring_size);

	/*
	 * By definition, the last 8 DWs of the buffer are not part of the rings
	 *  and are instead part of the Meta data area.
	 */
	pdd->spm_cntr->ring_size -= 0x20;

	if (ret)
		goto acquire_spm_failure;

	mutex_init(&pdd->spm_cntr->spm_worker_mutex);

	init_waitqueue_head(&pdd->spm_cntr->spm_buf_wq);
	INIT_WORK(&pdd->spm_work, kfd_spm_work);

	spin_lock_init(&pdd->spm_irq_lock);

	goto out;

acquire_spm_failure:
	amdgpu_amdkfd_free_gtt_mem(adev, pdd->spm_cntr->spm_obj);

alloc_gtt_mem_failure:
	mutex_lock(&pdd->spm_mutex);
	kfree(pdd->spm_cntr);
	pdd->spm_cntr = NULL;
	mutex_unlock(&pdd->spm_mutex);

out:
	return ret;
}

static int kfd_release_spm(struct kfd_process_device *pdd, struct amdgpu_device *adev)
{
	unsigned long flags;

	mutex_lock(&pdd->spm_mutex);
	if (!pdd->spm_cntr) {
		mutex_unlock(&pdd->spm_mutex);
		return -EINVAL;
	}

	spin_lock_irqsave(&pdd->spm_irq_lock, flags);
	pdd->spm_cntr->is_spm_started = false;
	spin_unlock_irqrestore(&pdd->spm_irq_lock, flags);

	flush_work(&pdd->spm_work);
	wake_up_all(&pdd->spm_cntr->spm_buf_wq);

	amdgpu_amdkfd_rlc_spm_release(adev, pdd->drm_priv);
	amdgpu_amdkfd_free_gtt_mem(adev, pdd->spm_cntr->spm_obj);

	spin_lock_irqsave(&pdd->spm_irq_lock, flags);
	kfree(pdd->spm_cntr);
	pdd->spm_cntr = NULL;
	spin_unlock_irqrestore(&pdd->spm_irq_lock, flags);

	mutex_unlock(&pdd->spm_mutex);
	return 0;
}

static void spm_copy_data_to_usr(struct kfd_ioctl_spm_args *user_spm_data,
			struct kfd_process_device *pdd)
{
	mutex_lock(&pdd->spm_cntr->spm_worker_mutex);
	user_spm_data->bytes_copied = pdd->spm_cntr->size_copied;
	user_spm_data->has_data_loss = pdd->spm_cntr->has_data_loss;
	pdd->spm_cntr->has_user_buf = false;
	mutex_unlock(&pdd->spm_cntr->spm_worker_mutex);
}

static void spm_set_dest_info(struct kfd_process_device *pdd,
			struct kfd_ioctl_spm_args *user_spm_data)
{
	mutex_lock(&pdd->spm_cntr->spm_worker_mutex);
	pdd->spm_cntr->ubuf.user_addr = (uint64_t *)user_spm_data->dest_buf;
	pdd->spm_cntr->ubuf.ubufsize = user_spm_data->buf_size;
	pdd->spm_cntr->has_data_loss = false;
	pdd->spm_cntr->size_copied = 0;
	pdd->spm_cntr->is_user_buf_filled = false;
	pdd->spm_cntr->has_user_buf = true;
	mutex_unlock(&pdd->spm_cntr->spm_worker_mutex);
}

static int spm_wait_for_fill_awake(struct kfd_spm_cntr *spm,
			struct kfd_ioctl_spm_args *user_spm_data)
{
	int ret = 0;

	long timeout = msecs_to_jiffies(user_spm_data->timeout);
	long start_jiffies = jiffies;

	ret = wait_event_interruptible_timeout(spm->spm_buf_wq,
				 (READ_ONCE(spm->is_user_buf_filled) == true),
				 timeout);

	switch (ret) {
	case -ERESTARTSYS:
		/* Subtract elapsed time from timeout so we wait that much
		 * less when the call gets restarted.
		 */
		timeout -= (jiffies - start_jiffies);
		if (timeout <= 0) {
			ret = -ETIME;
			timeout = 0;
			pr_debug("[%s] interrupted by signal\n", __func__);
		}
		break;

	case 0:
	default:
		timeout = ret;
		ret = 0;
		break;
	}
	user_spm_data->timeout = jiffies_to_msecs(timeout);

	return ret;
}

static int kfd_set_dest_buffer(struct kfd_process_device *pdd, struct amdgpu_device *adev, void *data)
{
	struct kfd_ioctl_spm_args *user_spm_data;
	struct kfd_spm_cntr *spm;
	unsigned long flags;
	int ret = 0;

	user_spm_data = (struct kfd_ioctl_spm_args *) data;

	mutex_lock(&pdd->spm_mutex);
	spm = pdd->spm_cntr;

	if (spm == NULL) {
		mutex_unlock(&pdd->spm_mutex);
		return -EINVAL;
	}

	if (user_spm_data->timeout && spm->has_user_buf &&
	    !READ_ONCE(spm->is_user_buf_filled)) {
		ret = spm_wait_for_fill_awake(spm, user_spm_data);
		if (ret == -ETIME) {
			/* Copy (partial) data to user buffer after a timeout */
			schedule_work(&pdd->spm_work);
			flush_work(&pdd->spm_work);
			/* This is not an error */
			ret = 0;
		} else if (ret) {
			/* handle other errors normally, including -ERESTARTSYS */
			mutex_unlock(&pdd->spm_mutex);
			return ret;
		}
	} else if (!user_spm_data->timeout && spm->has_user_buf) {
		/* Copy (partial) data to user buffer */
		schedule_work(&pdd->spm_work);
		flush_work(&pdd->spm_work);
	}

	if (spm->has_user_buf) {
		/* get info about filled space in previous output buffer */
		spm_copy_data_to_usr(user_spm_data, pdd);
	}

	if (user_spm_data->dest_buf) {
		/* setup new dest buf, start streaming if necessary */
		spm_set_dest_info(pdd, user_spm_data);

		/* Start SPM  */
		if (spm->is_spm_started == false) {
			amdgpu_amdkfd_rlc_spm_cntl(adev, 1);
			spin_lock_irqsave(&pdd->spm_irq_lock, flags);
			spm->is_spm_started = true;
			spin_unlock_irqrestore(&pdd->spm_irq_lock, flags);
		} else {
			/* If SPM was already started, there may already
			 * be data in the ring-buffer that needs to be read.
			 */
			schedule_work(&pdd->spm_work);
		}
	} else {
		amdgpu_amdkfd_rlc_spm_cntl(adev, 0);
		spin_lock_irqsave(&pdd->spm_irq_lock, flags);
		spm->is_spm_started = false;
		spin_unlock_irqrestore(&pdd->spm_irq_lock, flags);
	}

	mutex_unlock(&pdd->spm_mutex);

	return ret;
}

int kfd_rlc_spm(struct kfd_process *p,  void *data)
{
	struct kfd_ioctl_spm_args *args = data;
	struct kfd_dev *dev;
	struct kfd_process_device *pdd;

	dev = kfd_device_by_id(args->gpu_id);
	if (!dev) {
		pr_debug("Could not find gpu id 0x%x\n", args->gpu_id);
		return -EINVAL;
	}

	pdd = kfd_get_process_device_data(dev, p);
	if (!pdd)
		return -EINVAL;

	switch (args->op) {
	case KFD_IOCTL_SPM_OP_ACQUIRE:
		dev->spm_pasid = p->pasid;
		return  kfd_acquire_spm(pdd, dev->adev);

	case KFD_IOCTL_SPM_OP_RELEASE:
		return  kfd_release_spm(pdd, dev->adev);

	case KFD_IOCTL_SPM_OP_SET_DEST_BUF:
		return  kfd_set_dest_buffer(pdd, dev->adev, data);

	default:
		return -EINVAL;
	}

	return -EINVAL;
}

void kgd2kfd_spm_interrupt(struct kfd_dev *dev)
{
	struct kfd_process_device *pdd;
	uint16_t pasid = dev->spm_pasid;

	struct kfd_process *p = kfd_lookup_process_by_pasid(pasid);
	unsigned long flags;

	if (!p) {
		pr_debug("kfd_spm_interrupt p = %p\n", p);
		return; /* Presumably process exited. */
	}

	pdd = kfd_get_process_device_data(dev, p);
	if (!pdd)
		return;

	spin_lock_irqsave(&pdd->spm_irq_lock, flags);

	if (pdd->spm_cntr && pdd->spm_cntr->is_spm_started)
		schedule_work(&pdd->spm_work);
	spin_unlock_irqrestore(&pdd->spm_irq_lock, flags);

	kfd_unref_process(p);
}

