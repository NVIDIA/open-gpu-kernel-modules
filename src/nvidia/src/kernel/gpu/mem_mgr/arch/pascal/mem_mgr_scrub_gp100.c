/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/core.h"
#include "gpu/gpu.h"
#include "os/os.h"
#include "gpu/mem_mgr/mem_scrub.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/heap.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/fifo/kernel_fifo.h"

static NV_STATUS _memmgrPostSchedulingEnableHandler_GP100(OBJGPU *, void *);
static NV_STATUS _memmgrPreSchedulingDisableHandler_GP100(OBJGPU *, void *);

/*!
 * Scrub initialization routine
 *
 * @returns NV_OK on success, NV_ERR_GENERIC on failure
 */
NV_STATUS
memmgrScrubInit_GP100
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    if (memmgrIsScrubOnFreeEnabled(pMemoryManager))
    {
        NV_ASSERT_OK_OR_RETURN(kfifoAddSchedulingHandler(pGpu,
                    GPU_GET_KERNEL_FIFO(pGpu),
                    _memmgrPostSchedulingEnableHandler_GP100, NULL,
                    _memmgrPreSchedulingDisableHandler_GP100, NULL));
    }

    return NV_OK;
}

/*!
 * Performs initialization that is dependant on work done in gpuStateLoad() such
 * as channel initialization.
 *
 * @param[in] pGpu        OBJGPU pointer
 * @param[in] pUnusedData Callback data which is unused
 *
 * @returns  NV_OK on success, NV_ERR_GENERIC on failure
 */
static NV_STATUS
_memmgrPostSchedulingEnableHandler_GP100
(
    OBJGPU *pGpu,
    void   *pUnusedData
)
{
    Heap            *pHeap     = GPU_GET_HEAP(pGpu);
    NvBool           bIsMIGEnabled = IS_MIG_ENABLED(pGpu);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NvBool bIsVgpuLegacyPolicy = (pKernelMIGManager != NULL) && kmigmgrUseLegacyVgpuPolicy(pGpu, pKernelMIGManager);
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    //
    // Disabling scrub on free for SLI, waiting on the bug:1915380
    // Bug: 2997744, skipping the top level scrubber since partitions are not created.
    //
    if (!IsSLIEnabled(pGpu) &&
         memmgrIsScrubOnFreeEnabled(pMemoryManager) &&
         memmgrIsPmaInitialized(pMemoryManager) &&
         !(bIsMIGEnabled && IS_VIRTUAL(pGpu) && !bIsVgpuLegacyPolicy))
    {
        NV_ASSERT_OK_OR_RETURN(scrubberConstruct(pGpu, pHeap));
    }

    return NV_OK;
}

NV_STATUS
memmgrScrubHandlePostSchedulingEnable_GP100
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    return _memmgrPostSchedulingEnableHandler_GP100(pGpu, NULL);
}

/*!
 * Performs cleanup on resources that need to be freed before StateUnload routes
 * are called
 *
 * @param[in] pGpu        OBJGPU pointer
 * @param[in] pUnusedData Unused callback data
 *
 * @returns NV_OK
 */
static NV_STATUS
_memmgrPreSchedulingDisableHandler_GP100
(
    OBJGPU *pGpu,
    void   *pUnusedData
)
{
    Heap             *pHeap               = GPU_GET_HEAP(pGpu);
    OBJMEMSCRUB      *pMemscrub           = NULL;
    NvBool            bIsMIGEnabled       = IS_MIG_ENABLED(pGpu);
    MemoryManager    *pMemoryManager      = GPU_GET_MEMORY_MANAGER(pGpu);
    NvBool            bIsMIGInUse         = IS_MIG_IN_USE(pGpu);
    KernelMIGManager *pKernelMIGManager   = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NvBool            bIsVgpuLegacyPolicy = (pKernelMIGManager != NULL) && kmigmgrUseLegacyVgpuPolicy(pGpu, pKernelMIGManager);

    if (!pHeap)
        return NV_ERR_GENERIC;

    //
    // Top level scrubber was allocated with MIG disabled, it must be destroyed
    // with MIG disabled as well
    //
    if (bIsMIGInUse && !(IS_VIRTUAL(pGpu) && bIsVgpuLegacyPolicy))
        return NV_WARN_MORE_PROCESSING_REQUIRED;

    pMemscrub = pHeap->pmaObject.pScrubObj;

    // Bug: 2997744, skipping the top level scrubber since GPU instances are not created.
    if (!IsSLIEnabled(pGpu) &&
         memmgrIsScrubOnFreeEnabled(pMemoryManager) &&
         memmgrIsPmaInitialized(pMemoryManager) &&
         !(bIsMIGEnabled && IS_VIRTUAL(pGpu) && !bIsVgpuLegacyPolicy))
    {
        scrubberDestruct(pGpu, pHeap, pMemscrub);
        pHeap->pmaObject.pScrubObj = NULL;
    }

    return NV_OK;
}

NV_STATUS
memmgrScrubHandlePreSchedulingDisable_GP100
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    return _memmgrPreSchedulingDisableHandler_GP100(pGpu, NULL);
}

/*!
 * FB scrub cleanup routine. Deallocates any dynamic memory used.
 *
 * @returns NV_OK on success, NV_ERR_GENERIC on failure
 */
void
memmgrScrubDestroy_GP100
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    if (memmgrIsScrubOnFreeEnabled(pMemoryManager))
    {
        kfifoRemoveSchedulingHandler(pGpu, GPU_GET_KERNEL_FIFO(pGpu),
            _memmgrPostSchedulingEnableHandler_GP100, NULL,
            _memmgrPreSchedulingDisableHandler_GP100, NULL);
    }
}
