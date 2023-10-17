/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#include "kernel/gpu/video/kernel_video_engine.h"
#include "kernel/gpu/mem_mgr/mem_desc.h"
#include "kernel/gpu/bus/kern_bus.h"
#include "kernel/os/os.h"
#include "nvrm_registry.h"

NV_STATUS kvidengConstruct_IMPL
(
    KernelVideoEngine *pKernelVideoEngine,
    OBJGPU *pGpu,
    ENGDESCRIPTOR physEngDesc
)
{
    pKernelVideoEngine->physEngDesc = physEngDesc;
    return NV_OK;
}

NV_STATUS kvidengInitLogging_IMPL
(
    OBJGPU *pGpu,
    KernelVideoEngine *pKernelVideoEngine
)
{
    NV_STATUS status;
    NvU32 data = NV_REG_STR_RM_VIDEO_EVENT_TRACE_DISABLED;
    NvBool alwaysLogging;

    if (!gpuIsVideoTraceLogSupported(pGpu))
        return NV_OK;

    NV_ASSERT_OR_RETURN(pKernelVideoEngine != NULL, NV_ERR_INVALID_STATE);

    status = osReadRegistryDword(pGpu, NV_REG_STR_RM_VIDEO_EVENT_TRACE, &data);
    if (status != NV_OK)
    {
        // When GLOBAL_FEATURE_GR2069_VIDEO_EVENT is enabled and the registry is not set,
        // default eventbuffer size to 32K and staging buffer to 4K.
        data = DRF_NUM(_REG_STR, _RM_VIDEO_EVENT_TRACE, _STAGING_BUFFER_SIZE_IN_4k, 1) |
               DRF_NUM(_REG_STR, _RM_VIDEO_EVENT_TRACE, _EVENT_BUFFER_SIZE_IN_4k, 0x8);
    }

    alwaysLogging = DRF_VAL(_REG_STR, _RM_VIDEO_EVENT_TRACE, _ALWAYS_LOG, (data)) ==
                    NV_REG_STR_RM_VIDEO_EVENT_TRACE_ALWAYS_LOG_ENABLED;

    if (data != NV_REG_STR_RM_VIDEO_EVENT_TRACE_DISABLED)
    {
        NvU32 eventBufferSize;
        VIDEO_TRACE_RING_BUFFER *pTraceBuf;
        NvU64 seed;
        NvBool bIsFbBroken = NV_FALSE;
        NV_ADDRESS_SPACE videoBufferAddressSpace = ADDR_FBMEM;

        eventBufferSize = DRF_VAL(_REG_STR, _RM_VIDEO_EVENT_TRACE, _EVENT_BUFFER_SIZE_IN_4k, (data)) * 0x1000;

        bIsFbBroken = pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB) ||
                      pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM);

        if (bIsFbBroken)
            videoBufferAddressSpace = ADDR_SYSMEM;

        // Allocate the staging buffer
        NV_ASSERT_OK_OR_GOTO(
            status,
            memdescCreate(&pKernelVideoEngine->videoTraceInfo.pTraceBufferEngineMemDesc,
                          pGpu,
                          eventBufferSize,
                          0,
                          NV_TRUE,
                          videoBufferAddressSpace,
                          NV_MEMORY_UNCACHED,
                          MEMDESC_FLAGS_NONE),
            exit);

        NV_ASSERT_OK_OR_GOTO(
            status,
            memdescAlloc(pKernelVideoEngine->videoTraceInfo.pTraceBufferEngineMemDesc),
            exit);

        pTraceBuf = (VIDEO_TRACE_RING_BUFFER *)kbusMapRmAperture_HAL(pGpu, pKernelVideoEngine->videoTraceInfo.pTraceBufferEngineMemDesc);

        NV_ASSERT_OR_ELSE(pTraceBuf != NULL,
            status = NV_ERR_INSUFFICIENT_RESOURCES;
            goto exit;);

        // clear trace buffer
        portMemSet(pTraceBuf, 0, eventBufferSize);

        pTraceBuf->bufferSize = eventBufferSize - sizeof(VIDEO_TRACE_RING_BUFFER);
        pTraceBuf->readPtr = 0;
        pTraceBuf->writePtr = 0;
        pTraceBuf->flags = alwaysLogging ? VIDEO_TRACE_FLAG__LOGGING_ENABLED : 0;

        pKernelVideoEngine->videoTraceInfo.pTraceBufferEngine = pTraceBuf;

        // Allocate allocate scratch pad for variable data
        pKernelVideoEngine->videoTraceInfo.pTraceBufferVariableData = portMemAllocNonPaged(RM_VIDEO_TRACE_MAX_VARIABLE_DATA_SIZE);

        if (pKernelVideoEngine->videoTraceInfo.pTraceBufferVariableData == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto exit;
        }
        portMemSet(pKernelVideoEngine->videoTraceInfo.pTraceBufferVariableData, 0x00, RM_VIDEO_TRACE_MAX_VARIABLE_DATA_SIZE);

        /*!
         * Random number generator used for generate noisy timestamp
         */
        osGetCurrentTick(&seed);
        pKernelVideoEngine->videoTraceInfo.pVideoLogPrng = portCryptoPseudoRandomGeneratorCreate(seed);
    }

exit:
    if (status != NV_OK)
    {
        kvidengFreeLogging(pGpu, pKernelVideoEngine);

        if (status == NV_WARN_NOTHING_TO_DO)
            status = NV_OK;
    }
    else
    {
        pKernelVideoEngine->bVideoTraceEnabled = NV_TRUE;
    }

    return status;
}

void kvidengFreeLogging_IMPL
(
    OBJGPU *pGpu,
    KernelVideoEngine *pKernelVideoEngine
)
{
    if (pKernelVideoEngine->videoTraceInfo.pTraceBufferEngine != NULL)
    {
        kbusUnmapRmAperture_HAL(pGpu, pKernelVideoEngine->videoTraceInfo.pTraceBufferEngineMemDesc,
                                &(pKernelVideoEngine->videoTraceInfo.pTraceBufferEngine),
                                NV_TRUE);
        pKernelVideoEngine->videoTraceInfo.pTraceBufferEngine = NULL;
    }

    if (pKernelVideoEngine->videoTraceInfo.pTraceBufferEngineMemDesc != NULL)
    {
        memdescFree(pKernelVideoEngine->videoTraceInfo.pTraceBufferEngineMemDesc);
        memdescDestroy(pKernelVideoEngine->videoTraceInfo.pTraceBufferEngineMemDesc);
        pKernelVideoEngine->videoTraceInfo.pTraceBufferEngineMemDesc = NULL;
    }

    portMemFree(pKernelVideoEngine->videoTraceInfo.pTraceBufferVariableData);
    pKernelVideoEngine->videoTraceInfo.pTraceBufferVariableData = NULL;

    /*!
     * Free random number generator used for generate noisy timestamp
     */
    portCryptoPseudoRandomGeneratorDestroy(pKernelVideoEngine->videoTraceInfo.pVideoLogPrng);
    pKernelVideoEngine->videoTraceInfo.pVideoLogPrng = NULL;

    pKernelVideoEngine->bVideoTraceEnabled = NV_FALSE;
}
