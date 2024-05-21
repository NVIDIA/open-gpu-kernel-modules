/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "kernel/virtualization/hypervisor/hypervisor.h"
#include "nvrm_registry.h"
#include "vgpu/sdk-structures.h"

KernelVideoEngine *
kvidengFromEngDesc
(
    OBJGPU *pGpu,
    NvU32 engDesc
)
{
    NvU32 i;
    for (i = 0; i < pGpu->numKernelVideoEngines; i++)
    {
        if (engDesc == pGpu->kernelVideoEngines[i]->physEngDesc)
            return pGpu->kernelVideoEngines[i];
    }

    return NULL;
}

NvBool
kvidengIsVideoTraceLogSupported_IMPL
(
    OBJGPU *pGpu
)
{
    NvBool bSupported = !hypervisorIsVgxHyper() &&
                        !gpuIsSriovEnabled(pGpu) &&
                        !RMCFG_FEATURE_MODS_FEATURES &&
                        !IS_SIMULATION(pGpu);

    bSupported &= !gpuIsCCFeatureEnabled(pGpu);

    if (IS_VIRTUAL(pGpu))
    {
        // ensure profiling capability is enabled
        // only full SRIOV platforms is supported
        VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);
        bSupported &= (pVSI != NULL) &&
                      pVSI->vgpuStaticProperties.bProfilingTracingEnabled &&
                      IS_VIRTUAL_WITH_FULL_SRIOV(pGpu);
    }

    if (pGpu->kernelVideoEngines[0] != NULL)
    {
        NvU32 data = pGpu->kernelVideoEngines[0]->videoTraceInfo.eventTraceRegkeyData;
        bSupported &= (data != NV_REG_STR_RM_VIDEO_EVENT_TRACE_DISABLED);
    }

    NV_PRINTF(LEVEL_INFO, "video engine event tracing is %s.\n", bSupported ? "supported" : "unsupported");

    return bSupported;
}

NV_STATUS kvidengConstruct_IMPL
(
    KernelVideoEngine *pKernelVideoEngine,
    OBJGPU *pGpu,
    ENGDESCRIPTOR physEngDesc
)
{
    pKernelVideoEngine->physEngDesc = physEngDesc;
    pKernelVideoEngine->bVideoTraceEnabled = NV_FALSE;
    return NV_OK;
}

NV_STATUS kvidengInitLogging_KERNEL
(
    OBJGPU *pGpu,
    KernelVideoEngine *pKernelVideoEngine
)
{
    NV_STATUS status;
    NvU32 eventBufferSize;
    VIDEO_TRACE_RING_BUFFER *pTraceBuf;
    NvU64 seed;
    NvBool bIsFbBroken = NV_FALSE;
    NV_ADDRESS_SPACE addressSpace = ADDR_FBMEM;
    NvBool bAlwaysLogging;
    NvU32 data;

    NV_CHECK_OR_RETURN(LEVEL_INFO, kvidengIsVideoTraceLogSupported(pGpu), NV_OK);

    data = pKernelVideoEngine->videoTraceInfo.eventTraceRegkeyData;

    bAlwaysLogging = DRF_VAL(_REG_STR, _RM_VIDEO_EVENT_TRACE, _ALWAYS_LOG, (data)) ==
                    NV_REG_STR_RM_VIDEO_EVENT_TRACE_ALWAYS_LOG_ENABLED;

    eventBufferSize = DRF_VAL(_REG_STR, _RM_VIDEO_EVENT_TRACE, _EVENT_BUFFER_SIZE_IN_4k, (data)) * 0x1000;

    bIsFbBroken = pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB) ||
                  pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM);

    if (bIsFbBroken)
        addressSpace = ADDR_SYSMEM;

    // Allocate the staging buffer
    NV_ASSERT_OK_OR_GOTO(
        status,
        memdescCreate(&pKernelVideoEngine->videoTraceInfo.pTraceBufferEngineMemDesc,
                      pGpu,
                      eventBufferSize,
                      0,
                      NV_TRUE,
                      addressSpace,
                      NV_MEMORY_UNCACHED,
                      MEMDESC_FLAGS_NONE),
        exit);

    NV_ASSERT_OK_OR_GOTO(
        status,
        memdescAlloc(pKernelVideoEngine->videoTraceInfo.pTraceBufferEngineMemDesc),
        exit);

    pTraceBuf = (VIDEO_TRACE_RING_BUFFER *)kbusMapRmAperture_HAL(pGpu,
                                                        pKernelVideoEngine->videoTraceInfo.pTraceBufferEngineMemDesc);

    NV_ASSERT_OR_ELSE(pTraceBuf != NULL,
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto exit;);

    // clear trace buffer
    portMemSet(pTraceBuf, 0, eventBufferSize);
    pTraceBuf->bufferSize = eventBufferSize - sizeof(VIDEO_TRACE_RING_BUFFER);
    pTraceBuf->readPtr = 0;
    pTraceBuf->writePtr = 0;
    pTraceBuf->flags = bAlwaysLogging ? VIDEO_TRACE_FLAG__LOGGING_ENABLED : 0;

    pKernelVideoEngine->videoTraceInfo.pTraceBufferEngine = pTraceBuf;
    pKernelVideoEngine->videoTraceInfo.bAlwaysLogging = bAlwaysLogging;

    // Allocate allocate scratch pad for variable data
    pKernelVideoEngine->videoTraceInfo.pTraceBufferVariableData =
        portMemAllocNonPaged(RM_VIDEO_TRACE_MAX_VARIABLE_DATA_SIZE);

    if (pKernelVideoEngine->videoTraceInfo.pTraceBufferVariableData == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto exit;
    }
    portMemSet(pKernelVideoEngine->videoTraceInfo.pTraceBufferVariableData,
               0x00,
               RM_VIDEO_TRACE_MAX_VARIABLE_DATA_SIZE);

    /*!
     * Random number generator used for generate noisy timestamp
     */
    osGetCurrentTick(&seed);
    pKernelVideoEngine->videoTraceInfo.pVideoLogPrng = portCryptoPseudoRandomGeneratorCreate(seed);

    pKernelVideoEngine->bVideoTraceEnabled = NV_TRUE;

exit:
    if (status != NV_OK)
    {
        kvidengFreeLogging(pGpu, pKernelVideoEngine);
    }

    return status;
}

void kvidengFreeLogging_KERNEL
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

    memdescFree(pKernelVideoEngine->videoTraceInfo.pTraceBufferEngineMemDesc);
    memdescDestroy(pKernelVideoEngine->videoTraceInfo.pTraceBufferEngineMemDesc);
    pKernelVideoEngine->videoTraceInfo.pTraceBufferEngineMemDesc = NULL;

    portMemFree(pKernelVideoEngine->videoTraceInfo.pTraceBufferVariableData);
    pKernelVideoEngine->videoTraceInfo.pTraceBufferVariableData = NULL;

    /*!
     * Free random number generator used for generate noisy timestamp
     */
    portCryptoPseudoRandomGeneratorDestroy(pKernelVideoEngine->videoTraceInfo.pVideoLogPrng);
    pKernelVideoEngine->videoTraceInfo.pVideoLogPrng = NULL;

    pKernelVideoEngine->bVideoTraceEnabled = NV_FALSE;
}

/*!
 * This helper function is responsible for freeing the space from ringbuffer by advancing the read pointer.
 *
 * @param[in]  pGpu
 * @param[in]  oldReadPtr    read pointer of the starting point for free
 * @param[in]  size          size to be freed in NvU32
 * @param[in]  pTraceBuffer  pointer to ringbuffer to get data from.
 *
 * @return NV_STATUS to indicate if free is successful.
 */
NV_STATUS
kvidengRingbufferMakeSpace
(
    OBJGPU                  *pGpu,
    NvU32                    oldReadPtr,
    NvU32                    size,
    VIDEO_TRACE_RING_BUFFER *pTraceBuffer
)
{
    NV_STATUS status = NV_OK;
    NvU32 hasSize = 0;
    NvU32 oldWritePtr;
    NvU64 adjustedReadPtr;

    NV_ASSERT_OR_RETURN(pTraceBuffer != NULL, NV_ERR_INVALID_ARGUMENT);

    // Read in writePtr first so that we don't need to worry about sync between driver and uCode.
    oldWritePtr = pTraceBuffer->writePtr;
    adjustedReadPtr = (NvU64)oldReadPtr;

    if (oldWritePtr < oldReadPtr)
    {
        // Cross over 32bit boundary
        hasSize = (0xFFFFFFFF - oldReadPtr) + oldWritePtr + 1;
    }
    else
    {
        hasSize = oldWritePtr - oldReadPtr;
    }
    if (hasSize > pTraceBuffer->bufferSize)
    {
        hasSize = pTraceBuffer->bufferSize;
    }

    // Make sure we are not free pass over the write pointer.
    if (size > hasSize)
        size = hasSize;

    // Get newly adjusted readPtr in 64bits
    adjustedReadPtr += size;

    if (oldReadPtr != pTraceBuffer->readPtr)
    {
        NvU64 newReadPtr = 0;
        if (oldReadPtr > pTraceBuffer->readPtr)
        {
            // 32bit turn over
            newReadPtr = (NvU64)(pTraceBuffer->readPtr) + 0x100000000ULL;
        }
        // Only adjust the read pointer if newly freed space by other readers is not enough.
        if (adjustedReadPtr > newReadPtr)
        {
            pTraceBuffer->readPtr = oldReadPtr + size;
        }
    }
    else
    {
        pTraceBuffer->readPtr += size;
    }

    return status;
}

/*!
 * This function is responsible for reading data from ringbuffer
 *
 * @param[in]  pGpu
 * @param[in]  pKernelVideoEngine
 * @param[in]  pDataOut           output data pointer
 * @param[in]  sizeOut            output size in NvU32
 * @param[in]  pTraceBuffer       pointer to ringbuffer to get data from.
 *
 * @return size of data read successfully.
 */
NvU32
kvidengRingbufferGet_IMPL
(
    OBJGPU                  *pGpu,
    KernelVideoEngine       *pKernelVideoEngine,
    NvU8                    *pDataOut,
    NvU32                    sizeOut,
    VIDEO_TRACE_RING_BUFFER *pTraceBuffer
)
{
    NV_ASSERT_OR_RETURN(pDataOut != NULL, 0);
    NV_ASSERT_OR_RETURN(pTraceBuffer != NULL, 0);

    // Read in writePtr first so that we don't need to worry about sync between driver and uCode.
    NvU32 oldWritePtr = pTraceBuffer->writePtr;
    NvU32 oldReadPtr = pTraceBuffer->readPtr;
    NvU32 usedReadPtr = pTraceBuffer->readPtr;

    NvU32 writeOffset = 0;
    NvU32 readOffset = 0;
    NvU32 size2Top = 0;

    NvU32 hasSize = 0;

    if (oldWritePtr < oldReadPtr)
    {
        // Cross over 32bit boundary
        hasSize = (0xFFFFFFFF - oldReadPtr) + oldWritePtr + 1;
    }
    else
    {
        hasSize = oldWritePtr - oldReadPtr;
    }

    if (hasSize >= pTraceBuffer->bufferSize)
    {
        // The reader is too far behind, the data is over-written and invalid. Adjust read pointer used.
        hasSize = pTraceBuffer->bufferSize;
        if (oldWritePtr >= pTraceBuffer->bufferSize)
        {
            usedReadPtr = oldWritePtr - pTraceBuffer->bufferSize;
        }
        else
        {
            usedReadPtr = (0xFFFFFFFF - (pTraceBuffer->bufferSize - oldWritePtr)) + 1;
        }
    }

    if ((sizeOut > hasSize) || (oldWritePtr == usedReadPtr))
    {
        // Not enough data.
        return 0;
    }

    writeOffset = oldWritePtr % pTraceBuffer->bufferSize;
    readOffset = usedReadPtr % pTraceBuffer->bufferSize;
    size2Top = pTraceBuffer->bufferSize - readOffset;
    if ((writeOffset > readOffset) || ((writeOffset <= readOffset) && (sizeOut <= size2Top)))
    {
        portMemCopy(pDataOut, sizeOut, &(pTraceBuffer->pData[readOffset]), sizeOut);

        // Update read pointer, however we need to make sure read pointer is not updated by "free" call
        // so that the data got was validated. If the read pointer is changed by free call,
        // we can not guarantee the data read is valid. Therefore, 0 will be returned
        // to indicate the data read is not valid.
        if (pTraceBuffer->readPtr == oldReadPtr)
        {
            pTraceBuffer->readPtr = usedReadPtr + sizeOut;
        }
        else
        {
            // Output data could be corrupted. Invalidate the output by return 0.
            sizeOut = 0;
        }
    }
    else if ((writeOffset <= readOffset) && (sizeOut > size2Top))
    {
        // Has data accross top of the buffer, do 2 chunk read.
        kvidengRingbufferGet(pGpu, pKernelVideoEngine, pDataOut, size2Top, pTraceBuffer);
        kvidengRingbufferGet(pGpu, pKernelVideoEngine, &(pDataOut[size2Top]), sizeOut - size2Top, pTraceBuffer);
    }

    return sizeOut;
}

/*!
 * This helper function is looking for starting point of an event record.
 *
 * @param[in]  pGpu
 * @param[in]  pKernelVideoEngine
 * @param[in]  magic_hi           magic Hi identifying types of records
 * @param[in]  magic_lo           magic Lo identifying types of records
 * @param[in]  pTraceBuffer       pointer to ringbuffer to get data from.
 *
 * @return NV_STATUS to indicate if free is successful.
 */
static NV_STATUS
_eventbufferGotoNextRecord
(
    OBJGPU                  *pGpu,
    KernelVideoEngine       *pKernelVideoEngine,
    NvU32                    magic_hi,
    NvU32                    magic_lo,
    VIDEO_TRACE_RING_BUFFER *pTraceBuffer
)
{
    NV_ASSERT_OR_RETURN(pTraceBuffer != NULL, NV_ERR_INVALID_ARGUMENT);

    NvU32 i = 0;
    NvU32 oldReadPtr = pTraceBuffer->readPtr;
    NvU32 offset = oldReadPtr;
    NvU32 hasSize = kvidengRingbufferGetDataSize(pGpu, pTraceBuffer);

    if (hasSize < 8)
    {
        // Not enough for record magic, empty the buffer!
        offset += hasSize;
    }
    else
    {
        union {
            struct {
                NvU32 lo;
                NvU32 hi;
            };
            NvU64 val64bits;
        } magic;

        magic.hi =
            (pTraceBuffer->pData[(offset + 7) % pTraceBuffer->bufferSize] << 24) +
            (pTraceBuffer->pData[(offset + 6) % pTraceBuffer->bufferSize] << 16) +
            (pTraceBuffer->pData[(offset + 5) % pTraceBuffer->bufferSize] << 8) +
            (pTraceBuffer->pData[(offset + 4) % pTraceBuffer->bufferSize]);
        magic.lo =
            (pTraceBuffer->pData[(offset + 3) % pTraceBuffer->bufferSize] << 24) +
            (pTraceBuffer->pData[(offset + 2) % pTraceBuffer->bufferSize] << 16) +
            (pTraceBuffer->pData[(offset + 1) % pTraceBuffer->bufferSize] << 8) +
            (pTraceBuffer->pData[offset % pTraceBuffer->bufferSize]);

        for (i = 0; i < hasSize - 8; i++)
        {
            if ((magic.lo == magic_lo) && (magic.hi == magic_hi))
            {
                break;
            }

            magic.val64bits = (magic.val64bits >> 8) | (((NvU64)(pTraceBuffer->pData[(offset + i + 8) % pTraceBuffer->bufferSize])) << 56);
        }

        offset += i;
        if (i == (hasSize - 8))
        {
            // Did not find magic. Empty the buffer
            offset += 8;
        }
    }

    if (offset != oldReadPtr)
    {
        NvU32 skipSize = 0;
        if (offset < pTraceBuffer->readPtr)
        {
            // 32bit turn over
            skipSize = offset + (0xFFFFFFFFUL - oldReadPtr) + 1UL;
        }
        else
        {
            skipSize = offset - oldReadPtr;
        }
        kvidengRingbufferMakeSpace(pGpu, oldReadPtr, skipSize, pTraceBuffer);
    }

    return NV_OK;
}

/*!
 * This function is to get one record from a trace buffer.
 *
 * @param[in]  pGpu
 * @param[in]  pKernelVideoEngine
 * @param[in]  pTraceBuffer       pointer to source ringbuffer to get data from.
 * @param[in]  pRecord            pointer to a trace record to copy data to.
 * @param[in]  magic_hi           magic Hi identifying types of records
 * @param[in]  magic_lo           magic Lo identifying types of records
 *
 * @return size of data copied successfully.
 */
NvU32
kvidengEventbufferGetRecord_IMPL
(
    OBJGPU                     *pGpu,
    KernelVideoEngine          *pKernelVideoEngine,
    VIDEO_TRACE_RING_BUFFER    *pTraceBuffer,
    VIDEO_ENGINE_EVENT__RECORD *pRecord,
    NvU32                       magic_hi,
    NvU32                       magic_lo
)
{
    if (!pKernelVideoEngine->bVideoTraceEnabled)
    {
        return 0;
    }

    NV_ASSERT_OR_RETURN(pTraceBuffer != NULL, 0);
    NV_ASSERT_OR_RETURN(pRecord != NULL, 0);
    NV_ASSERT_OR_RETURN(pKernelVideoEngine->videoTraceInfo.pTraceBufferVariableData != NULL, 0);

    NvU32 size = 0;

    _eventbufferGotoNextRecord(pGpu, pKernelVideoEngine, magic_hi, magic_lo, pTraceBuffer);

    size = kvidengRingbufferGet(pGpu, pKernelVideoEngine, (NvU8*)pRecord, sizeof(VIDEO_ENGINE_EVENT__RECORD), pTraceBuffer);

    if (size != sizeof(VIDEO_ENGINE_EVENT__RECORD))
    {
        return 0;
    }

    if (pRecord->event_id == VIDEO_ENGINE_EVENT_ID__LOG_DATA)
    {
        NvU32 dataSize = 0;

        if (pRecord->event_log_data.size > RM_VIDEO_TRACE_MAX_VARIABLE_DATA_SIZE)
        {
            // Corrupted size.
            return 0;
        }

        dataSize = kvidengRingbufferGet(pGpu, pKernelVideoEngine, pKernelVideoEngine->videoTraceInfo.pTraceBufferVariableData, pRecord->event_log_data.size, pTraceBuffer);
        if (dataSize != pRecord->event_log_data.size)
        {
            // Corrupted data
            return 0;
        }
        size += dataSize;
    }

    // Got a record and return total size of the data;
    return size;
}

/*!
 * This function gets the data size of a ringbuffer for external caller to peek data size of a ringbugger
 *
 * @param[in]  pGpu
 * @param[in]  pKernelVideoEngine
 * @param[in]  pTraceBuffer        pointer to a ringbuffer
 *
 * @return size of data in the ringbuffer
 */
NvU32
kvidengRingbufferGetDataSize_IMPL
(
    OBJGPU                  *pGpu,
    VIDEO_TRACE_RING_BUFFER *pTraceBuffer
)
{
    NV_ASSERT_OR_RETURN(pTraceBuffer != NULL, 0);

    // Read in read/write Ptrs first so that we don't need to worry about sync between driver and uCode.
    NvU32 oldReadPtr = pTraceBuffer->readPtr;
    NvU32 oldWritePtr = pTraceBuffer->writePtr;

    NvU32 hasSize = 0;

    if (oldWritePtr < oldReadPtr)
    {
        // Cross over 32bit boundary
        hasSize = (0xFFFFFFFF - oldReadPtr) + oldWritePtr + 1;
    }
    else
    {
        hasSize = oldWritePtr - oldReadPtr;
    }

    if (hasSize > pTraceBuffer->bufferSize)
    {
        hasSize = pTraceBuffer->bufferSize;
    }

    return hasSize;
}
