/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/mem_mgr/mem_mgr.h"
#include "mem_mgr/ctx_buf_pool.h"

#include "published/volta/gv100/dev_mmu.h"

#include "class/clc361.h"

NV_STATUS
memmgrScrubMapDoorbellRegion_GV100
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    OBJCHANNEL    *pChannel
)
{
    NV_STATUS  status = NV_OK;
    RM_API    *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    // GSPTODO: disable doorbell region scrubbing for now
    if (RMCFG_FEATURE_PLATFORM_GSP)
        return NV_OK;

    // allocate object for the class VOLTA_USER_MODE_A
    status = pRmApi->AllocWithHandle(pRmApi,
                                     pChannel->hClient,
                                     pChannel->subdeviceId,
                                     pChannel->doorbellRegionHandle,
                                     VOLTA_USERMODE_A,
                                     NULL,
                                     0);
    if (status != NV_OK)
        goto exit;

    // Map the doorbell region onto CPU to submit work in the future
    status = pRmApi->MapToCpu(pRmApi,
                              pChannel->hClient,
                              pChannel->deviceId,
                              pChannel->doorbellRegionHandle,
                              0,
                              NVC361_NV_USERMODE__SIZE,
                              (void**)(&pChannel->pDoorbellRegion),
                              DRF_DEF(OS33, _FLAGS, _ACCESS, _WRITE_ONLY));
    if (status != NV_OK)
    {
        pRmApi->Free(pRmApi, pChannel->hClient, pChannel->doorbellRegionHandle);
        goto exit;
    }

    pChannel->pDoorbellRegisterOffset = (NvU32*)(pChannel->pDoorbellRegion +
                                          NVC361_NOTIFY_CHANNEL_PENDING);

    // setting the use of doorbell register for this channel
    pChannel->bUseDoorbellRegister = NV_TRUE;
exit:
    return status;


}

/*!
 *  Returns the max context size
 *
 *  @returns NvU64
 */
NvU64
memmgrGetMaxContextSize_GV100
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    extern NvU64 memmgrGetMaxContextSize_GP100(OBJGPU *pGpu, MemoryManager *pMemoryManager);

    NvU64 size = memmgrGetMaxContextSize_GP100(pGpu, pMemoryManager);

    // In Volta, the GR context buffer size increased by about 847 KB (doubled from Pascal)
    if (RMCFG_FEATURE_PLATFORM_WINDOWS)
    {
        //
        // We are increasing the reserved mem size by 10 MB.
        // This is to account for GR context buffer allocs by KMD's private channels.
        // KMD allocates 10 GR channels (not in virtual context mode).
        // This is causing an additional 8470 KB allocations from RM reserved heap.
        // See bug 1882679 for more details.
        //
        size += (10 * 1024 *1024);
    }
    else if (RMCFG_FEATURE_PLATFORM_MODS)
    {
        // Double the context size
        size *= 2;
    }
    else
    {
        if (!ctxBufPoolIsSupported(pGpu))
        {
            //
            // Increase the context size by 120 MB.
            // This is needed to run the same number glxgears instances as in GP102.
            // See bug 1885000 comment 7 and bug 1885000 comment 36
            //
            size += (120 * 1024 *1024);
        }
    }

    return size;
}

/**
 *  This will return NV_TRUE if surface is BL. otherwise it returns NV_FALSE.
 */
NvBool
memmgrIsSurfaceBlockLinear_GV100
(
    MemoryManager     *pMemoryManager,
    Memory            *pMemory,
    NvU32              kind,
    NvU32              dmaFlags
)
{
    if (FLD_TEST_DRF(OS03, _FLAGS, _PTE_KIND, _BL, dmaFlags))
    {
        return NV_TRUE;
    }
    else if (FLD_TEST_DRF(OS03, _FLAGS, _PTE_KIND, _PITCH, dmaFlags))
    {
        return NV_FALSE;
    }

    return (kind != NV_MMU_PTE_KIND_PITCH) ? NV_TRUE: NV_FALSE;
}
