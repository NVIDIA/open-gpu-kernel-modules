/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "nvstatus.h"
#include "os/os.h"
#include "nv.h"
#include "nv-hypervisor.h"

HYPERVISOR_TYPE NV_API_CALL nv_get_hypervisor_type(void)
{
    return OS_HYPERVISOR_UNKNOWN;
}

NV_STATUS NV_API_CALL nv_vgpu_get_type_ids(
    nvidia_stack_t *sp,
    nv_state_t *pNv,
    NvU32 *numVgpuTypes,
    NvU32 **vgpuTypeIds,
    NvBool isVirtfn
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS NV_API_CALL nv_vgpu_process_vf_info(
    nvidia_stack_t *sp,
    nv_state_t *pNv,
    NvU8 cmd,
    NvU32 domain,
    NvU8 bus,
    NvU8 slot,
    NvU8 function,
    NvBool isMdevAttached,
    void *vf_pci_info
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS NV_API_CALL nv_vgpu_get_type_info(
    nvidia_stack_t *sp,
    nv_state_t *pNv,
    NvU32 vgpuTypeId,
    char *buffer,
    int type_info,
    NvU8 devfn
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS NV_API_CALL nv_vgpu_create_request(
    nvidia_stack_t *sp,
    nv_state_t *pNv,
    const NvU8 *pMdevUuid,
    NvU32 vgpuTypeId,
    NvU16 *vgpuId,
    NvU32 gpuPciBdf
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS NV_API_CALL nv_vgpu_update_request(
    nvidia_stack_t *sp ,
    const NvU8 *pMdevUuid,
    VGPU_DEVICE_STATE deviceState,
    NvU64 *offsets,
    NvU64 *sizes,
    const char *configParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS NV_API_CALL nv_vgpu_get_sparse_mmap(
    nvidia_stack_t *sp ,
    nv_state_t *pNv,
    const NvU8 *pMdevUuid,
    NvU64 **offsets,
    NvU64 **sizes,
    NvU32 *numAreas
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS NV_API_CALL nv_gpu_bind_event(
    nvidia_stack_t *sp
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS NV_API_CALL nv_vgpu_start(
    nvidia_stack_t *sp,
    const NvU8 *pMdevUuid,
    void *waitQueue,
    NvS32 *returnStatus,
    NvU8 *vmName,
    NvU32 qemuPid
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS NV_API_CALL nv_vgpu_delete(
    nvidia_stack_t *sp,
    const NvU8 *pMdevUuid,
    NvU16 vgpuId
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS NV_API_CALL nv_vgpu_get_bar_info(
    nvidia_stack_t *sp,
    nv_state_t *pNv,
    const NvU8 *pMdevUuid,
    NvU64 *size,
    NvU32 regionIndex,
    void *pVgpuVfioRef
)
{
    return NV_ERR_NOT_SUPPORTED;
}

void initVGXSpecificRegistry(OBJGPU *pGpu)
{}

