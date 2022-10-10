/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/mem_mgr/mem_mgr.h"
#include "kernel/gpu/fifo/usermode_api.h"
#include "kernel/gpu/fifo/kernel_fifo.h"
#include "kernel/gpu/bus/kern_bus.h"

#include "class/clc661.h" // HOPPER_USERMODE_A
#include "class/cl003e.h" // NV01_MEMORY_SYSTEM
#include "class/cl003f.h" // NV01_MEMORY_LOCAL_PRIVILEGED

NV_STATUS
usrmodeConstructHal_GH100
(
    UserModeApi                  *pUserModeApi,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    Memory *pMemory                             = staticCast(pUserModeApi, Memory);
    NvU32  hClass                               = pCallContext->pResourceRef->externalClassId;
    NvBool bBar1Mapping                         = NV_FALSE;
    NvBool bPriv                                = NV_FALSE;
    NV_HOPPER_USERMODE_A_PARAMS *pAllocParams   = (NV_HOPPER_USERMODE_A_PARAMS*)pParams->pAllocParams;
    NV_STATUS status                            = NV_OK;
    MEMORY_DESCRIPTOR *pMemDesc                 = NULL;
    OBJGPU *pGpu                                = pMemory->pGpu;
    KernelFifo *pKernelFifo                     = GPU_GET_KERNEL_FIFO(pGpu);
    KernelBus *pKernelBus                       = GPU_GET_KERNEL_BUS(pGpu);
    MemoryManager *pMemoryManager               = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU64 offset                                = 0;
    NvU32 size                                  = 0;
    NvU32 attr                                  = 0;
    NvU32 attr2                                 = 0;

    attr = FLD_SET_DRF(OS32, _ATTR,  _PHYSICALITY, _CONTIGUOUS, attr);
    attr = FLD_SET_DRF(OS32, _ATTR,  _COHERENCY, _CACHED, attr);

    attr2 = FLD_SET_DRF(OS32, _ATTR2, _GPU_CACHEABLE, _NO, attr2 );

    if (hClass >= HOPPER_USERMODE_A && pAllocParams != NULL)
    {
        bBar1Mapping = pAllocParams->bBar1Mapping;
        bPriv = pAllocParams->bPriv;
    }

    // BAR1/2 is not used on coherent platforms like SHH, so use BAR0 path instead
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING))
    {
        bBar1Mapping = NV_FALSE;
    }

    // 
    // If we are in inst_in_sys mode, BAR1 page tables are not used so we cannot 
    // support internal MMIO for work submission. 
    // It is safe to return BAR0 mapping here because USERD cannot be in vidmem
    // anyway so UMDs can use this mapping transparently. 
    // TODO: we should rename bBar1Mapping to accommodate cases where RM 
    // transparently gives out BAR0 mappings instead.
    //
    if (bBar1Mapping && pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM))
    {
        NV_ASSERT_OR_RETURN(kbusIsBar1PhysicalModeEnabled(pKernelBus),
                            NV_ERR_INVALID_STATE);
        bBar1Mapping = NV_FALSE;
    }

    if (bPriv && pCallContext->secInfo.privLevel < RS_PRIV_LEVEL_KERNEL)
    {
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    NV_ASSERT_OK_OR_RETURN(kfifoGetUsermodeMapInfo_HAL(pGpu, pKernelFifo, &offset, &size));

    if (bBar1Mapping)
    {
        offset = kfifoGetMmioUsermodeOffset_HAL(pGpu, pKernelFifo, bPriv);
        size = kfifoGetMmioUsermodeSize_HAL(pGpu, pKernelFifo, bPriv);
    }

    status = memCreateMemDesc(pGpu, &pMemDesc, bBar1Mapping ? ADDR_SYSMEM : ADDR_REGMEM,
                              offset, size, attr, attr2);

    if (bBar1Mapping)
    {
        memdescSetPteKind(pMemDesc, memmgrGetMessageKind_HAL(pGpu, pMemoryManager));
        memdescSetFlag(pMemDesc, MEMDESC_FLAGS_MAP_SYSCOH_OVER_BAR1, NV_TRUE);
    }
    else
    {
        memdescSetFlag(pMemDesc, MEMDESC_FLAGS_SKIP_REGMEM_PRIV_CHECK, NV_TRUE);
    }

    status = memConstructCommon(pMemory, bBar1Mapping? NV01_MEMORY_SYSTEM : NV01_MEMORY_LOCAL_PRIVILEGED,
                    0, pMemDesc, 0, NULL, 0, 0, 0, 0, NVOS32_MEM_TAG_NONE, NULL);

    return status;
}
