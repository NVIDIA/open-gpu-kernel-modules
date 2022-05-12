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

#include "amdgpu_object.h"
#include "amdgpu_amdkfd.h"
#include <uapi/linux/kfd_ioctl.h>
#include "amdgpu_ids.h"

void amdgpu_amdkfd_rlc_spm_cntl(struct amdgpu_device *adev, bool cntl)
{
	struct amdgpu_ring *kiq_ring = &adev->gfx.kiq.ring;

	spin_lock(&adev->gfx.kiq.ring_lock);
	amdgpu_ring_alloc(kiq_ring, adev->gfx.spmfuncs->set_spm_config_size);
	if (cntl)
		adev->gfx.spmfuncs->start(adev);
	else
		adev->gfx.spmfuncs->stop(adev);
	amdgpu_ring_commit(kiq_ring);
	spin_unlock(&adev->gfx.kiq.ring_lock);
}

void amdgpu_amdkfd_rlc_spm_set_rdptr(struct amdgpu_device *adev, u32 rptr)
{
	struct amdgpu_ring *kiq_ring = &adev->gfx.kiq.ring;

	spin_lock(&adev->gfx.kiq.ring_lock);
	amdgpu_ring_alloc(kiq_ring, adev->gfx.spmfuncs->set_spm_config_size);
	adev->gfx.spmfuncs->set_rdptr(adev, rptr);
	amdgpu_ring_commit(kiq_ring);
	spin_unlock(&adev->gfx.kiq.ring_lock);
}

int amdgpu_amdkfd_rlc_spm_acquire(struct amdgpu_device *adev, struct amdgpu_vm *vm, u64 gpu_addr, u32 size)
{
	struct amdgpu_ring *kiq_ring = &adev->gfx.kiq.ring;
	int r;

	if (!adev->gfx.rlc.funcs->update_spm_vmid)
		return -EINVAL;

	r = amdgpu_vmid_alloc_reserved(adev, vm, AMDGPU_GFXHUB_0);
	if (r)
		return r;

	/* init spm vmid with 0x0 */
	adev->gfx.rlc.funcs->update_spm_vmid(adev, 0);

	/* set spm ring registers */
	spin_lock(&adev->gfx.kiq.ring_lock);
	amdgpu_ring_alloc(kiq_ring, adev->gfx.spmfuncs->set_spm_config_size);
	adev->gfx.spmfuncs->set_spm_perfmon_ring_buf(adev, gpu_addr, size);
	amdgpu_ring_commit(kiq_ring);
	spin_unlock(&adev->gfx.kiq.ring_lock);
	return r;
}

void amdgpu_amdkfd_rlc_spm_release(struct amdgpu_device *adev, struct amdgpu_vm *vm)
{
	struct amdgpu_ring *kiq_ring = &adev->gfx.kiq.ring;

	/* stop spm stream and interrupt */
	spin_lock(&adev->gfx.kiq.ring_lock);
	amdgpu_ring_alloc(kiq_ring, adev->gfx.spmfuncs->set_spm_config_size);
	adev->gfx.spmfuncs->stop(adev);
	amdgpu_ring_commit(kiq_ring);
	spin_unlock(&adev->gfx.kiq.ring_lock);

	amdgpu_vmid_free_reserved(adev, vm, AMDGPU_GFXHUB_0);

	/* revert spm vmid with 0xf */
	if (adev->gfx.rlc.funcs->update_spm_vmid)
		adev->gfx.rlc.funcs->update_spm_vmid(adev, 0xf);
}

void amdgpu_amdkfd_rlc_spm_interrupt(struct amdgpu_device *adev)
{
	if (adev->kfd.dev)
		kgd2kfd_spm_interrupt(adev->kfd.dev);
}

