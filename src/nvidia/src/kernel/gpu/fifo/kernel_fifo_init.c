/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/fifo/kernel_fifo.h"
#include "kernel/gpu/fifo/kernel_channel_group.h"
#include "kernel/gpu/fifo/kernel_sched_mgr.h"
#include "kernel/gpu/rc/kernel_rc.h"

#include "vgpu/vgpu_events.h"

#include "nvRmReg.h"

#include "class/cl2080.h"

static void _kfifoPreConstructRegistryOverrides(OBJGPU *pGpu, KernelFifo *pKernelFifo);

NV_STATUS
kfifoConstructEngine_IMPL
(
    OBJGPU        *pGpu,
    KernelFifo    *pKernelFifo,
    ENGDESCRIPTOR  engDesc
)
{
    NvU32 i;

    _kfifoPreConstructRegistryOverrides(pGpu, pKernelFifo);

    portMemSet((void *)&pKernelFifo->engineInfo, 0, sizeof(ENGINE_INFO));

    portMemSet((void *)&pKernelFifo->userdInfo, 0, sizeof(PREALLOCATED_USERD_INFO));

    for (i = 0; i < RM_ENGINE_TYPE_LAST; i++)
    {
        pKernelFifo->pRunlistBufPool[i] = NULL;
    }

    pKernelFifo->pDummyPageMemDesc = NULL;
    pKernelFifo->pppRunlistBufMemDesc = NULL;

    NV_ASSERT_OK_OR_RETURN(kfifoConstructHal_HAL(pGpu, pKernelFifo));

    listInit(&pKernelFifo->postSchedulingEnableHandlerList,
             portMemAllocatorGetGlobalNonPaged());
    listInit(&pKernelFifo->preSchedulingDisableHandlerList,
             portMemAllocatorGetGlobalNonPaged());

    return NV_OK;
}

static void
_kfifoPreConstructRegistryOverrides
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    NvU32 data32;

    pKernelFifo->bNumChannelsOverride = 0;
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_NUM_FIFOS,
                            &pKernelFifo->numChannelsOverride) == NV_OK)
    {
        pKernelFifo->bNumChannelsOverride = 1;
    }

    pKernelFifo->bPerRunlistChramOverride = NV_FALSE;

    if ((osReadRegistryDword(pGpu, NV_REG_STR_RM_SUPPORT_USERD_MAP_DMA,
                            &data32) == NV_OK) && data32)
    {
        NV_PRINTF(LEVEL_ERROR, "Enabling MapMemoryDma of USERD\n");
        pKernelFifo->bUserdMapDmaSupported = NV_TRUE;
    }

    return;
}

void
kfifoDestruct_IMPL
(
    KernelFifo *pKernelFifo
)
{
    ENGINE_INFO *pEngineInfo = &pKernelFifo->engineInfo;

    // Free all outstanding callback entries
    listDestroy(&pKernelFifo->postSchedulingEnableHandlerList);
    listDestroy(&pKernelFifo->preSchedulingDisableHandlerList);

    objDelete(pKernelFifo->pKernelSchedMgr);
    pKernelFifo->pKernelSchedMgr = NULL;

    portMemFree(pEngineInfo->engineInfoList);
    pEngineInfo->engineInfoList = NULL;

    return;
}

NV_STATUS
kfifoStateInitLocked_IMPL
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    KernelRc *pKernelRc = GPU_GET_KERNEL_RC(pGpu);

    // Check if per runlist channel ram should be enabled
    if (kfifoIsPerRunlistChramSupportedInHw(pKernelFifo) &&
        !pKernelFifo->bPerRunlistChramOverride)
    {
        //
        // On production platforms. SRIOV gets enabled
        // only on host RM for SR-IOV capable SKUs (See gpuInitRegistryOverrides).
        // On MODS, the tests use the regkey to turn on SR-IOV
        //
        {
            if (gpuIsSriovEnabled(pGpu))
            {
                NV_PRINTF(LEVEL_INFO, "Enabling per runlist channel RAM on host RM\n");
                pKernelFifo->bUsePerRunlistChram = NV_TRUE;
            }
        }
    }

    NV_ASSERT_OK_OR_RETURN(kfifoConstructUsermodeMemdescs_HAL(pGpu, pKernelFifo));

    NV_ASSERT_OK_OR_RETURN(kfifoChidMgrConstruct(pGpu, pKernelFifo));

    if (pKernelRc != NULL)
    {
        krcInitRegistryOverridesDelayed(pGpu, pKernelRc);
    }

    // Get maximum number of secure channels when APM/HCC is enabled
    NV_ASSERT_OK_OR_RETURN(kfifoGetMaxSecureChannels(pGpu, pKernelFifo));

    return NV_OK;
}

void
kfifoStateDestroy_IMPL
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    KernelChannel *pKernelChannel;
    CHANNEL_ITERATOR chanIt;

    // On LDDM, we don't free these during freechannel because it's possible
    // we wouldn't be able to reallocate them (we want to keep them preallocated
    // from boot time). But we need to free before shutdown, so do that here.
    kfifoGetChannelIterator(pGpu, pKernelFifo, &chanIt);
    while ((kfifoGetNextKernelChannel(pGpu, pKernelFifo, &chanIt, &pKernelChannel) == NV_OK))
    {
        RM_ENGINE_TYPE rmEngineType;

        rmEngineType = kchannelGetEngineType(pKernelChannel);

        if (RM_ENGINE_TYPE_IS_GR(rmEngineType))
        {
            MEMORY_DESCRIPTOR *grCtxBufferMemDesc = NULL;
            NvU32 grIdx = RM_ENGINE_TYPE_GR_IDX(rmEngineType);

            NV_ASSERT_OK(
                kchangrpGetEngineContextMemDesc(pGpu,
                                                pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup,
                                                &grCtxBufferMemDesc));

            if (grCtxBufferMemDesc != NULL)
            {
                memdescFree(grCtxBufferMemDesc);
                memdescDestroy(grCtxBufferMemDesc);

                // Now clear context buffer pointer.
                NV_ASSERT_OK(kchannelUnmapEngineCtxBuf(pGpu, pKernelChannel, ENG_GR(grIdx)));

                NV_ASSERT_OK(
                    kchannelSetEngineContextMemDesc(pGpu,
                                                    pKernelChannel,
                                                    ENG_GR(grIdx),
                                                    NULL));
            }
        }
    }

    // Notify the handlers that the channel will soon be disabled
    NV_ASSERT_OK(kfifoTriggerPreSchedulingDisableCallback(pGpu, pKernelFifo));

    //
    // Free up allocated memory.
    //
    kfifoChidMgrDestruct(pKernelFifo);

    // Destroy regardless of NULL, if pointers are null, is just a NOP
    memdescDestroy(pKernelFifo->pRegVF);
    memdescDestroy(pKernelFifo->pBar1VF);
    memdescDestroy(pKernelFifo->pBar1PrivVF);

    return;
}
