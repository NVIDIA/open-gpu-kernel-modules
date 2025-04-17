/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */



#include "nvidia-push-init.h"
#include "nvidia-push-utils.h"
#include "nvidia-push-priv.h"
#include "nvidia-push-priv-imports.h"

#include "nvos.h"

#include "nv_assert.h"

#include "alloc/alloc_channel.h"
#include "class/cl0002.h" // NV01_CONTEXT_DMA
#include "class/cl003e.h" // NV01_MEMORY_SYSTEM
#include "class/cl0040.h" // NV01_MEMORY_LOCAL_USER

#include "class/cla16f.h" // KEPLER_CHANNEL_GPFIFO_B
#include "class/cla26f.h" // KEPLER_CHANNEL_GPFIFO_C
#include "class/clb06f.h" // MAXWELL_CHANNEL_GPFIFO_A
#include "class/clc06f.h" // PASCAL_CHANNEL_GPFIFO_A
#include "class/clc36f.h" // VOLTA_CHANNEL_GPFIFO_A
#include "class/clc46f.h" // TURING_CHANNEL_GPFIFO_A
#include "class/cl50a0.h" // NV50_MEMORY_VIRTUAL
#include "class/clc56f.h" // AMPERE_CHANNEL_GPFIFO_A
#include "class/clc86f.h" // HOPPER_CHANNEL_GPFIFO_A
#include "class/clc96f.h" // BLACKWELL_CHANNEL_GPFIFO_A
#include "class/clca6f.h" // BLACKWELL_CHANNEL_GPFIFO_B
#include "class/clc361.h" // VOLTA_USERMODE_A
#include "class/clc661.h" // HOPPER_USERMODE_A

#include "ctrl/ctrl0080/ctrl0080fifo.h" // NV0080_CTRL_CMD_FIFO_GET_CAPS_V2
#include "ctrl/ctrl2080/ctrl2080bus.h" // NV2080_CTRL_CMD_BUS_GET_INFO
#include "ctrl/ctrla06f/ctrla06fgpfifo.h" // KEPLER_CHANNEL_GPFIFO_A
#include "ctrl/ctrlc36f.h" // VOLTA_CHANNEL_GPFIFO_A

static NvU32 GetHandle(
    const NvU32 *pHandlePool,
    NvU8 handlePoolSize,
    NvU64 *pUsedHandleBitmask)
{
    NvU8 i;
    const NvU64 usedHandleBitmask = *pUsedHandleBitmask;

    /*
     * We assume there are less than 64 handles in the pool. If the
     * pool is larger than that, we'll need a fancier bitmask.
     */
    nvAssert(handlePoolSize < (sizeof(NvU64) * 8));

    for (i = 0; i < handlePoolSize; i++) {
        if ((usedHandleBitmask & NVBIT64(i)) == 0) {
            *pUsedHandleBitmask |= NVBIT64(i);
            return pHandlePool[i];
        }
    }

    nvAssert(!"Exhausted handlePool!");

    return 0;
}

static NvU32 GetChannelHandle(
    const NvPushAllocChannelParams *pParams,
    NvU64 *pUsedHandleBitmask)
{
    return GetHandle(pParams->handlePool,
                     ARRAY_LEN(pParams->handlePool),
                     pUsedHandleBitmask);
}

static NvU32 GetDeviceHandle(
    const NvPushAllocDeviceParams *pParams,
    NvU64 *pUsedHandleBitmask)
{
    return GetHandle(pParams->handlePool,
                     ARRAY_LEN(pParams->handlePool),
                     pUsedHandleBitmask);
}

static void FreeSemaSurface(NvPushChannelPtr p)
{
    NvPushDevicePtr pDevice = p->pDevice;
    NvU32 *handle = p->progressSemaphore.handle;
    void **ptr = p->progressSemaphore.ptr;
    NvU32 status;
    int sd;

    if (p->progressSemaphore.gpuVA) {
        for (sd = pDevice->numSubDevices - 1; sd >= 0; sd--) {
            const int deviceIndex = __nvPushGetDeviceIndex(pDevice, sd);
            status = nvPushImportRmApiUnmapMemoryDma(
                         pDevice,
                         pDevice->subDevice[sd].handle,
                         pDevice->subDevice[deviceIndex].gpuVASpaceCtxDma,
                         handle[sd],
                         0,
                         p->progressSemaphore.gpuVA);
            if (status != NVOS_STATUS_SUCCESS) {
                nvAssert(!"Failed to unmap progressSemaphore");
            }
        }
        p->progressSemaphore.gpuVA = 0;
    }

    for (sd = pDevice->numSubDevices - 1; sd >= 0; sd--) {
        const int deviceIndex = __nvPushGetDeviceIndex(pDevice, sd);
        if (!handle[sd]) {
            continue;
        }
        status = nvPushImportRmApiFree(
                     pDevice,
                     pDevice->subDevice[deviceIndex].deviceHandle,
                     handle[sd]);
        if (status != NVOS_STATUS_SUCCESS) {
            nvAssert(!"Failed to free progressSemaphore");
        }
        handle[sd] = 0;

        /* Freeing this memory automatically unmaps it. */
        ptr[sd] = NULL;
    }
}

static NvBool AllocSemaSurface(
    NvPushChannelPtr p,
    const NvPushAllocChannelParams *pParams,
    NvBool coherent,
    NvU64 *pUsedHandleBitmask)
{
    NvPushDevicePtr pDevice = p->pDevice;
    NvU32 *handle = p->progressSemaphore.handle;
    void **ptr = p->progressSemaphore.ptr;
    NvU32 status;
    const NvU64 size = 4096;
    unsigned int sd;

    /* 1. Allocate sysmem surface(s) to back the semaphore, get CPU mapping */
    for (sd = 0; sd < pDevice->numSubDevices; sd++) {
        const int deviceIndex = __nvPushGetDeviceIndex(pDevice, sd);
        NvU64 limit = size - 1;
        const NvU32 flags = DRF_DEF(OS02, _FLAGS, _PHYSICALITY, _NONCONTIGUOUS) |
                      (coherent ? DRF_DEF(OS02, _FLAGS, _COHERENCY, _CACHED) :
                                  DRF_DEF(OS02, _FLAGS, _COHERENCY, _UNCACHED));

        handle[sd] = GetChannelHandle(pParams, pUsedHandleBitmask);

        status = nvPushImportRmApiAllocMemory64(pDevice,
                                                pDevice->subDevice[deviceIndex].deviceHandle,
                                                handle[sd],
                                                NV01_MEMORY_SYSTEM,
                                                flags,
                                                &ptr[sd],
                                                &limit);

        if (status != NVOS_STATUS_SUCCESS) {
            handle[sd] = 0;
            nvAssert(!"Failed to allocate FIFO semaphore surface");
            goto fail;
        }
    }

    /* 2. Map the surface(s) into the GPU(s) */
    for (sd = 0; sd < pDevice->numSubDevices; sd++) {
        NvU32 flags = DRF_DEF(OS46, _FLAGS, _ACCESS, _READ_WRITE) |
                      DRF_DEF(OS46, _FLAGS, _PAGE_SIZE, _4KB) |
                      (coherent ? DRF_DEF(OS46, _FLAGS, _CACHE_SNOOP, _ENABLE) :
                                  DRF_DEF(OS46, _FLAGS, _CACHE_SNOOP, _DISABLE));
        const int deviceIndex = __nvPushGetDeviceIndex(pDevice, sd);

        /*
         * Note that this mapping is somewhat special because we use a
         * different surface for each subdevice, but want to map at the same
         * virtual address on all subdevices.
         */
        if (sd == 0) {
            /*
             * Create a new virtual mapping.
             *
             * The MapMemoryDma call will assign to
             * 'p->progressSemaphore.gpuVA'.
             *
             * In !clientSli, this creates a broadcast mapping that we override
             * with the _DMA_UNICAST_REUSE_ALLOC flag below.
             * In clientSli, each mapping is already unicast.
             *
             * In both cases, the DMA_OFFSET_FIXED flag ensures the VA matches
             * between all subdevices.
             */
            p->progressSemaphore.gpuVA = 0;
            flags = FLD_SET_DRF(OS46, _FLAGS, _DMA_OFFSET_FIXED, _FALSE, flags);
        } else {
            /*
             * The MapMemoryDma call will read from
             * 'p->progressSemaphore.gpuVA'.
             */
            nvAssert(p->progressSemaphore.gpuVA != 0);
            if (!pDevice->clientSli) {
                flags = FLD_SET_DRF(OS46, _FLAGS, _DMA_UNICAST_REUSE_ALLOC, _TRUE, flags);
            }
            flags = FLD_SET_DRF(OS46, _FLAGS, _DMA_OFFSET_FIXED, _TRUE, flags);
        }

        status = nvPushImportRmApiMapMemoryDma(pDevice,
                                               pDevice->subDevice[sd].handle,
                                               pDevice->subDevice[deviceIndex].gpuVASpaceCtxDma,
                                               handle[sd],
                                               0,
                                               size,
                                               flags,
                                               &p->progressSemaphore.gpuVA);
        if (status != NVOS_STATUS_SUCCESS) {
            nvAssert(!"Failed to map FIFO semaphore surface");
            goto fail;
        }
    }

    return TRUE;
fail:
    FreeSemaSurface(p);
    return FALSE;
}

/*
 * The size of the "progress tracker" portion of the pushbuffer.
 *
 * We use one set of progress tracker methods for every two GPFIFO entries (one
 * GPFIFO entry is for the main pushbuffer, the other is for the progress
 * tracker methods).
 */
static inline NvU32 ProgressTrackerBufferSize(NvPushChannelPtr buffer)
{
    return __nvPushProgressTrackerEntrySize(buffer->pDevice) *
        (buffer->numGpFifoEntries / 2);
}

/*
 * The size of the pushbuffer allocation, including all segments and GPFIFO
 * entries.
 */
static inline NvU32 CalculateGPBufferSize(NvPushChannelPtr buffer)
{
    return __nvPushProgressTrackerOffset(buffer) +
        ProgressTrackerBufferSize(buffer);
}

/*!
 * Set up an NvPushChannelSegmentRec's initial state based on the provided data
 *
 * \param segment   Pointer to segment structure to initialize
 * \param ptr       CPU mapping to the base of the segment.
 * \param gpuOffset GPU mapping of the base of the segment.
 * \param size      Size of the segment, in bytes.
 */
static void InitDmaSegment(NvPushChannelSegmentPtr segment,
                           void *ptr,
                           NvU64 gpuOffset,
                           NvU32 size)
{
    segment->base            = (NvPushChannelUnion *)ptr;
    segment->buffer          = (NvPushChannelUnion *)ptr;
    segment->sizeInBytes     = size;
    segment->freeDwords      = size >> 2;
    segment->gpuMapOffset    = gpuOffset;
    segment->putOffset       = 0;
}

/*!
 * Set up the work submit token.  RM will write this into the "error context
 * DMA" at the offset we request.
 */
static NvBool RequestChidToken(NvPushChannelPtr p)
{
    NvPushDevicePtr pDevice = p->pDevice;
    int deviceIndex;

    for (deviceIndex = 0;
         deviceIndex < __nvPushGetNumDevices(pDevice);
         deviceIndex++) {

        NVC36F_CTRL_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX_PARAMS notifParams = { 0 };
        NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS tokenParams = { 0 };
        NvU32 status;

        notifParams.index = NV_CHANNELGPFIFO_NOTIFICATION_TYPE__SIZE_1 + deviceIndex;

        status = nvPushImportRmApiControl(pDevice,
                         p->channelHandle[deviceIndex],
                         NVC36F_CTRL_CMD_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX,
                         &notifParams,
                         sizeof(notifParams));
        if (status != NVOS_STATUS_SUCCESS) {
            return FALSE;
        }

        /*
         * Request the channel's "work submit token".  This isn't actually used for
         * anything but RM needs it to be called after the channel has been allocated,
         * for reasons.
         */
        status = nvPushImportRmApiControl(pDevice,
                         p->channelHandle[deviceIndex],
                         NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN,
                         &tokenParams,
                         sizeof(tokenParams));
        if (status != NVOS_STATUS_SUCCESS) {
            return FALSE;
        }
    }
    return TRUE;
}

static NvBool BindAndScheduleChannel(NvPushDevicePtr pDevice,
                                     NvU32 channelHandle,
                                     NvU32 engineType)
{
    NVA06F_CTRL_BIND_PARAMS bindParams = { 0 };
    NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS scheduleParams = { 0 };
    NvBool ret;

    bindParams.engineType = engineType;
    ret = nvPushImportRmApiControl(pDevice,
                                   channelHandle,
                                   NVA06F_CTRL_CMD_BIND,
                                   &bindParams,
                                   sizeof(bindParams));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvPushImportLogError(pDevice, "Failed to bind the channel");
        return FALSE;
    }

    scheduleParams.bEnable = NV_TRUE;
    ret = nvPushImportRmApiControl(pDevice,
                                   channelHandle,
                                   NVA06F_CTRL_CMD_GPFIFO_SCHEDULE,
                                   &scheduleParams,
                                   sizeof(scheduleParams));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvPushImportLogError(pDevice,
                             "Failed to schedule the channel");
        return FALSE;
    }

    return TRUE;
}

static NvBool AllocChannelObject(
    NvPushChannelPtr buffer,
    const NvPushAllocChannelParams *pParams,
    NvU64 *pUsedHandleBitmask,
    NvU64 gpuAddress)
{
    NvPushDevicePtr pDevice = buffer->pDevice;
    NV_CHANNEL_ALLOC_PARAMS params = { 0 };
    unsigned int sd;
    NvU32 userdMapHandle[NV_MAX_SUBDEVICES];
    NvU32 ret;
    const NvU64 gpFifoOffset = gpuAddress + __nvPushGpFifoOffset(buffer);
    int deviceIndex;

    for (deviceIndex = 0;
         deviceIndex < __nvPushGetNumDevices(pDevice);
         deviceIndex++) {
        buffer->channelHandle[deviceIndex] = GetChannelHandle(pParams, pUsedHandleBitmask);
        nvAssert(buffer->notifiers.errorCtxDma != 0);

        /* Open the DMA channel by allocating the CHANNEL_GPFIFO object */
        params.hObjectError  = buffer->notifiers.errorCtxDma;
        if (pDevice->subDevice[deviceIndex].gpuVASpaceObject != 0) {
            params.hVASpace = pDevice->subDevice[deviceIndex].gpuVASpaceObject;
        } else {
            params.hObjectBuffer = pDevice->subDevice[deviceIndex].gpuVASpaceCtxDma;
        }
        // Offset is relative to the ctx dma
        params.gpFifoOffset  = gpFifoOffset;

        if (pDevice->hal.caps.allocateDoubleSizeGpFifo) {
            // On Tegra, we have to allocate twice the GPFIFO size. This is because
            // the kernel will add its own entries (max 2) for the kickoff for the
            // pre-sync and post-sync fences. This means the max kickoff size is not
            // actually buffer->numGpFifoEntries - 1, it's
            // most likely buffer->numGpFifoEntries - 3.
            //
            // TODO: Tell the users the actual max kickoff size to avoid this
            // WAR. NvRmTegraChannelGetMaxKickoffGpfifoCount() retrieves this piece
            // of info on Tegra. Bug 2404063.
            params.gpFifoEntries = buffer->numGpFifoEntries * 2;
        } else {
            params.gpFifoEntries = buffer->numGpFifoEntries;
        }

        params.flags         = 0;
        if (pParams->secureChannel) {
            params.flags |= DRF_DEF(OS04, _FLAGS, _CC_SECURE, _TRUE);
        }
        if (pParams->difrPrefetch) {
            params.flags |= DRF_DEF(OS04,
                                    _FLAGS,
                                    _SET_EVICT_LAST_CE_PREFETCH_CHANNEL,
                                    _TRUE);
        }

        if (pDevice->hal.caps.clientAllocatesUserD) {
            if (pDevice->clientSli) {
                params.hUserdMemory[0] = buffer->userD[deviceIndex].hMemory;
                params.userdOffset[0]  = 0;
            } else {
                for (sd = 0; sd < pDevice->numSubDevices; sd++) {
                    params.hUserdMemory[sd] = buffer->userD[0].hMemory;
                    params.userdOffset[sd]  = 0;
                }
            }
            userdMapHandle[deviceIndex] = buffer->userD[deviceIndex].hMemory;
        } else {
            userdMapHandle[deviceIndex] = buffer->channelHandle[deviceIndex];
        }
        params.engineType = pParams->engineType;
        if (pDevice->clientSli) {
            params.subDeviceId = (1 << deviceIndex);
        }

        if ((ret = nvPushImportRmApiAlloc(pDevice,
                                          pDevice->subDevice[deviceIndex].deviceHandle,
                                          buffer->channelHandle[deviceIndex],
                                          pDevice->gpfifoClass,
                                          &params)) != NVOS_STATUS_SUCCESS)
        {
            nvPushImportLogError(pDevice,
                                 "Push buffer object allocation failed: 0x%x (%s)",
                                 ret, nvstatusToString(ret));
            buffer->channelHandle[deviceIndex] = 0;
            return FALSE;
        }

        if (!BindAndScheduleChannel(pDevice,
                                    buffer->channelHandle[deviceIndex],
                                    pParams->engineType)) {
            return FALSE;
        }
    }

    for (sd = 0; sd < pDevice->numSubDevices; sd++) {
        void *pUserD;

        deviceIndex = __nvPushGetDeviceIndex(pDevice, sd);

        // Map the DMA controls for each subdevice.
        ret = nvPushImportRmApiMapMemory(pDevice,
                                         pDevice->subDevice[sd].handle,
                                         userdMapHandle[deviceIndex],
                                         0,
                                         pDevice->userDSize,
                                         &pUserD,
                                         0);
        if (ret != NVOS_STATUS_SUCCESS) {
            nvPushImportLogError(pDevice,
                                 "Push buffer mapping failed: 0x%x (%s)",
                                 ret, nvstatusToString(ret));
            return FALSE;
        }

        buffer->control[sd] = pUserD;
    }

    return TRUE;
}

/*
 * It might be nice to suballocate these rather
 * than create a separate RM allocation for each channel.
 */
static NvBool nvDmaAllocUserD(
    NvPushChannelPtr p,
    const NvPushAllocChannelParams *pParams,
    NvU64 *pUsedHandleBitmask)
{
    NvPushDevicePtr pDevice = p->pDevice;
    int deviceIndex;
    NvBool bHasFB = pDevice->hasFb;

    if (!pDevice->hal.caps.clientAllocatesUserD) {
        return TRUE;
    }

    for (deviceIndex = 0;
         deviceIndex < __nvPushGetNumDevices(pDevice);
         deviceIndex++) {
        NV_MEMORY_ALLOCATION_PARAMS memAllocParams = { 0 };
        NvU32 ret;

        /* For GPUs which do not have framebuffer memory, use allocation from
         * system memory instead.
         */
        const NvU32 hClass = bHasFB ? NV01_MEMORY_LOCAL_USER : NV01_MEMORY_SYSTEM;
        const NvU32 attr =
            bHasFB ?
                    DRF_DEF(OS32, _ATTR, _LOCATION, _VIDMEM) |
                    DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _4KB) |
                    DRF_DEF(OS32, _ATTR, _COHERENCY, _UNCACHED)
                :
                    DRF_DEF(OS32, _ATTR, _LOCATION, _PCI) |
                    DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _4KB) |
                    DRF_DEF(OS32, _ATTR, _COHERENCY, _UNCACHED);
        const NvU32 flags =
            bHasFB ?
                    NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE |
                    NVOS32_ALLOC_FLAGS_PERSISTENT_VIDMEM
                :
                    NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE;

        NvU32 hMemory = GetChannelHandle(pParams, pUsedHandleBitmask);

        memAllocParams.owner = pDevice->clientHandle;
        memAllocParams.type = NVOS32_TYPE_DMA;
        memAllocParams.size = pDevice->userDSize;
        memAllocParams.attr = attr;
        memAllocParams.flags = flags;
        memAllocParams.alignment = pDevice->userDSize;

        ret = nvPushImportRmApiAlloc(pDevice,
                                     pDevice->subDevice[deviceIndex].deviceHandle,
                                     hMemory,
                                     hClass,
                                     &memAllocParams);
        if (ret != NV_OK) {
            return FALSE;
        }

        p->userD[deviceIndex].hMemory = hMemory;
    }

    return TRUE;
}

static NvBool IsClassSupported(
    const NvPushDeviceRec *pDevice,
    NvU32 classNumber)
{
    unsigned int j;
    for (j = 0; j < pDevice->numClasses; j++) {
        if (classNumber == pDevice->supportedClasses[j]) {
            return TRUE;
        }
    }
    return FALSE;
}

int nvPushGetSupportedClassIndex(
    NvPushDevicePtr pDevice,
    const void *pClassTable,
    size_t classTableStride,
    size_t classTableLength)
{
    unsigned int i;

    for (i = 0; i < classTableLength; i++) {

        const NvU8 *bytes = (const NvU8 *)pClassTable;
        const size_t byteOffset = i * classTableStride;
        const NvPushSupportedClass *pClass =
            (const NvPushSupportedClass *) (bytes + byteOffset);

        if (nvPushIsAModel(pDevice)) {
            if (pDevice->amodelConfig == pClass->amodelConfig) {
                return i;
            }
            continue;
        }

        if (IsClassSupported(pDevice, pClass->classNumber)) {
            return i;
        }
    }
    return -1;
}

static NvBool GetChannelClassAndUserDSize(
    NvPushDevicePtr pDevice,
    const NvPushAllocDeviceParams *pParams)
{
    const struct {
        NvPushSupportedClass base;
        size_t gpFifoSize;
    } gpFifoDmaClasses[] = {
    {
        { BLACKWELL_CHANNEL_GPFIFO_B,
          NV_AMODEL_GB20X },
        sizeof(BlackwellBControlGPFifo)
    },
    {
        { BLACKWELL_CHANNEL_GPFIFO_A,
          NV_AMODEL_BLACKWELL },
        sizeof(BlackwellAControlGPFifo)
    },
    {
        { HOPPER_CHANNEL_GPFIFO_A,
          NV_AMODEL_HOPPER },
        sizeof(HopperAControlGPFifo)
    },
    {
        { AMPERE_CHANNEL_GPFIFO_A,
          NV_AMODEL_ADA },
        sizeof(AmpereAControlGPFifo)
    },
    {
        { AMPERE_CHANNEL_GPFIFO_A,
          NV_AMODEL_AMPERE },
        sizeof(AmpereAControlGPFifo)
    },
    {
        { TURING_CHANNEL_GPFIFO_A,
          NV_AMODEL_TURING },
        sizeof(TuringAControlGPFifo)
    },
    {
        { VOLTA_CHANNEL_GPFIFO_A,
          NV_AMODEL_VOLTA },
        sizeof(VoltaAControlGPFifo)
    },
    {
        { PASCAL_CHANNEL_GPFIFO_A,
          NV_AMODEL_PASCAL },
        sizeof(PascalAControlGPFifo)
    },
    {
        { MAXWELL_CHANNEL_GPFIFO_A,
          NV_AMODEL_MAXWELL },
        sizeof(MaxwellAControlGPFifo)
    },
    {
        { KEPLER_CHANNEL_GPFIFO_C,
          NV_AMODEL_KEPLER_SM35 },
        sizeof(KeplerCControlGPFifo)
    },
    {
        { KEPLER_CHANNEL_GPFIFO_B,
          NV_AMODEL_KEPLER },
        sizeof(KeplerBControlGPFifo)
    },

    };

    int i;

    i = nvPushGetSupportedClassIndex(pDevice, gpFifoDmaClasses,
                                     sizeof(gpFifoDmaClasses[0]),
                                     ARRAY_LEN(gpFifoDmaClasses));
    if (i == -1) {
        return FALSE;
    }

    pDevice->gpfifoClass = gpFifoDmaClasses[i].base.classNumber;
    pDevice->userDSize = gpFifoDmaClasses[i].gpFifoSize;
    return TRUE;
}

/*
 * Query GPU<->CPU coherency.  In particular, *pCoherent is set to TRUE when
 * the GPU is capable of accessing CPU-cached system memory coherently with
 * respect to CPU accesses.
 *
 * For surfaces with CPU read/write or CPU read-mostly such as notifiers:
 * If *pCoherent is TRUE:
 * - create CPU mappings with COHERENCY_WRITE_BACK
 * - create GPU mappings with CACHE_SNOOP_ENABLE
 * If *pCoherent is FALSE:
 * - create CPU mappings with COHERENCY_UNCACHED
 * - create GPU mappings with CACHE_SNOOP_DISABLE
 *
 * (CPU write-mostly surfaces such as the pushbuffer always use WRITE_COMBINED
 * memory.)
 *
 * Note we only query on the first subdevice and assume the other subdevices
 * are the same.
 */
static NvBool GetCoherenceFlags(
    NvPushChannelPtr pChannel,
    NvBool *pCoherent)
{
    NvPushDevicePtr pDevice = pChannel->pDevice;
    NV2080_CTRL_BUS_GET_INFO_PARAMS busInfo = { 0 };
    struct {
        NV2080_CTRL_BUS_INFO coherentFlags;
    } busInfoList;

    NvU32 ret;

    NVMISC_MEMSET(&busInfoList, 0, sizeof(busInfoList));
    busInfoList.coherentFlags.index =
        NV2080_CTRL_BUS_INFO_INDEX_COHERENT_DMA_FLAGS;

    busInfo.busInfoListSize = sizeof(busInfoList) /
                              sizeof(NV2080_CTRL_BUS_INFO);
    busInfo.busInfoList = NV_PTR_TO_NvP64(&busInfoList);

    ret = nvPushImportRmApiControl(pDevice,
                                   pDevice->subDevice[0].handle,
                                   NV2080_CTRL_CMD_BUS_GET_INFO,
                                   &busInfo, sizeof(busInfo));

    if (ret != NVOS_STATUS_SUCCESS) {
        return FALSE;
    }

    *pCoherent =
        FLD_TEST_DRF(2080_CTRL_BUS_INFO, _COHERENT_DMA_FLAGS, _GPUGART, _TRUE,
                     busInfoList.coherentFlags.data);
    return TRUE;
}

static NvBool TryAllocAndMapPushbuffer(
    NvPushChannelPtr pChannel,
    const NvU32 allocFlags,
    const NvU32 mapFlags,
    const NvU32 limit,
    void **pCpuAddress,
    NvU64 *pGpuAddress)
{
    NvU32 ret;
    NvU64 localLimit;
    NvU64 size = limit + 1;
    void  *cpuAddress = NULL;
    NvU64 gpuAddress = 0;
    NvPushDevicePtr pDevice = pChannel->pDevice;
    int deviceIndex;
    NvBool vaAlloc[NV_MAX_SUBDEVICES] = { 0 };
    NvBool vaMap[NV_MAX_SUBDEVICES] = { 0 };
    NvBool surfaceAlloc = FALSE;

    for (deviceIndex = 0;
         deviceIndex < __nvPushGetNumDevices(pDevice);
         deviceIndex++) {
        NV_MEMORY_ALLOCATION_PARAMS vaParams = { 0 };

        vaParams.owner = 0x70757368;
        vaParams.type = NVOS32_TYPE_DMA;
        vaParams.flags =
            NVOS32_ALLOC_FLAGS_MEMORY_HANDLE_PROVIDED |
            NVOS32_ALLOC_FLAGS_VIRTUAL;
        vaParams.size = size;
        vaParams.hVASpace = pDevice->subDevice[deviceIndex].gpuVASpaceObject;

        if (deviceIndex == 0) {
            /* For the first device, RM assigns a virtual address. */
            if (pChannel->pDevice->hal.caps.extendedBase) {
                /*
                 * Force the virtual mapping to be naturally aligned.
                 * This ensures that the allocation cannot cross a 40-bit
                 * boundary, so we can initialize the higher bits of the VA
                 * with the PB_EXTENDED_BASE_OPERAND GPFIFO command once at
                 * init time and not worry about it being able to change
                 * between any two GPFIFO entries.
                 */
                vaParams.flags |= NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE;
                vaParams.alignment = size;
                ROUNDUP_POW2_U64(vaParams.alignment);
            }
        } else {
            /* For subsequent devices, use the same virtual address. */
            vaParams.flags |= NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE;
            nvAssert(gpuAddress != 0);
            vaParams.offset = gpuAddress;
        }

        ret = nvPushImportRmApiAlloc(
                  pDevice,
                  pDevice->subDevice[deviceIndex].deviceHandle,
                  pChannel->pushbufferVAHandle[deviceIndex],
                  NV50_MEMORY_VIRTUAL,
                  &vaParams);

        if (ret != NVOS_STATUS_SUCCESS) {
            goto fail;
        }
        vaAlloc[deviceIndex] = TRUE;

        if (deviceIndex == 0) {
            gpuAddress = vaParams.offset;
            nvAssert(vaParams.size >= size);
            /* The VA allocation may have been bloated to a larger size, to
             * align with the page size.  Adjust to ensure that we allocate a
             * surface of at least that size, or else attempts to map it will
             * fail. */
            size = vaParams.size;
        } else {
            nvAssert(gpuAddress == vaParams.offset);
            nvAssert(vaParams.size == size);
        }
    }

    /* Allocate a single surface in system memory for the pushbuffer. */
    localLimit = size - 1;
    ret = nvPushImportRmApiAllocMemory64(
              pDevice,
              pDevice->subDevice[0].deviceHandle,
              pChannel->pushbufferHandle,
              NV01_MEMORY_SYSTEM,
              allocFlags,
              &cpuAddress,
              &localLimit);

    if (ret != NVOS_STATUS_SUCCESS) {
        goto fail;
    }
    nvAssert(localLimit + 1 >= size);
    surfaceAlloc = TRUE;

    for (deviceIndex = 0;
         deviceIndex < __nvPushGetNumDevices(pDevice);
         deviceIndex++) {
        NvU64 mapOffset = 0;

        ret = nvPushImportRmApiMapMemoryDma(
                  pDevice,
                  pDevice->subDevice[deviceIndex].deviceHandle,
                  pChannel->pushbufferVAHandle[deviceIndex],
                  pChannel->pushbufferHandle,
                  0,
                  size,
                  mapFlags,
                  &mapOffset);

        if (ret != NVOS_STATUS_SUCCESS) {
            goto fail;
        }
        vaMap[deviceIndex] = TRUE;
        /* mapMemoryDma takes in a relative offset but assigns an absolute VA */
        nvAssert(mapOffset == gpuAddress);
    }

    /* success */
    *pCpuAddress = cpuAddress;
    *pGpuAddress = gpuAddress;
    return TRUE;

fail:
    for (deviceIndex = __nvPushGetNumDevices(pDevice) - 1;
         deviceIndex >= 0;
         deviceIndex--) {
        if (vaMap[deviceIndex]) {
            ret = nvPushImportRmApiUnmapMemoryDma(pDevice,
                                pDevice->subDevice[deviceIndex].deviceHandle,
                                pChannel->pushbufferVAHandle[deviceIndex],
                                pChannel->pushbufferHandle,
                                0,
                                gpuAddress);
            nvAssert(ret == NVOS_STATUS_SUCCESS);
            vaMap[deviceIndex] = FALSE;
        }
        if (vaAlloc[deviceIndex]) {
            ret = nvPushImportRmApiFree(pDevice,
                                pDevice->subDevice[deviceIndex].deviceHandle,
                                pChannel->pushbufferVAHandle[deviceIndex]);
            nvAssert(ret == NVOS_STATUS_SUCCESS);
            vaAlloc[deviceIndex] = FALSE;
        }
    };

    if (surfaceAlloc) {
        ret = nvPushImportRmApiFree(pDevice,
                                    pDevice->subDevice[0].deviceHandle,
                                    pChannel->pushbufferHandle);
        nvAssert(ret == NVOS_STATUS_SUCCESS);
    }

    return FALSE;
}

static NvBool AllocPushbuffer(
    NvPushChannelPtr pChannel,
    const NvPushAllocChannelParams *pParams,
    NvU64 *pUsedHandleBitmask,
    void **pCpuAddress,
    NvU64 *pGpuAddress)
{
    const NvU32 size = CalculateGPBufferSize(pChannel);
    NvU32 limit = size - 1;
    int deviceIndex;

    pChannel->pushbufferHandle = GetChannelHandle(pParams, pUsedHandleBitmask);
    for (deviceIndex = 0;
         deviceIndex < __nvPushGetNumDevices(pChannel->pDevice);
         deviceIndex++) {
        pChannel->pushbufferVAHandle[deviceIndex] =
            GetChannelHandle(pParams, pUsedHandleBitmask);
    }

    if (TryAllocAndMapPushbuffer(
            pChannel,
            DRF_DEF(OS02, _FLAGS, _PHYSICALITY, _NONCONTIGUOUS) |
                DRF_DEF(OS02, _FLAGS, _COHERENCY, _WRITE_COMBINE),
            DRF_DEF(OS46, _FLAGS, _CACHE_SNOOP, _DISABLE),
            limit,
            pCpuAddress,
            pGpuAddress)) {
        return TRUE;
    }

    pChannel->pushbufferHandle = 0;
    NVMISC_MEMSET(pChannel->pushbufferVAHandle, 0, sizeof(pChannel->pushbufferVAHandle));
    return FALSE;
}

/*!
 * Free resources allocated in AllocUserMode().
 */
static void FreeUserMode(
    NvPushDevicePtr pDevice)
{
    NvU32 sd;

    for (sd = 0; sd < pDevice->numSubDevices; sd++) {

        if (pDevice->subDevice[sd].pUserMode != NULL) {
            nvPushImportRmApiUnmapMemory(
                  pDevice,
                  pDevice->subDevice[sd].handle,
                  pDevice->subDevice[sd].hUserMode,
                  pDevice->subDevice[sd].pUserMode,
                  0 /* flags */);
            pDevice->subDevice[sd].pUserMode = NULL;
        }

        if (pDevice->subDevice[sd].hUserMode != 0) {
            nvPushImportRmApiFree(
                  pDevice,
                  pDevice->subDevice[sd].handle,
                  pDevice->subDevice[sd].hUserMode);
            pDevice->subDevice[sd].hUserMode = 0;
        }
    }
}

/*!
 * Allocate and map the "usermode" object on each subdevice, supported on GV100
 * and up.  This mapping exposes registers considered safe for userspace to
 * access directly.  Most importantly, it contains the "doorbell" register
 * which we use to notify HOST that we've updated GP_PUT so that it will fetch
 * work for the channel.
 */
static NvBool AllocUserMode(
    NvPushDevicePtr pDevice,
    const NvPushAllocDeviceParams *pParams,
    NvU64 *pUsedHandleBitmask)
{
    unsigned int sd;

    static const NvPushSupportedClass userModeClasses[] = {
        { HOPPER_USERMODE_A,
          NV_AMODEL_HOPPER },
        { VOLTA_USERMODE_A,
          NV_AMODEL_VOLTA },
    };
    int i;

    if (!pDevice->hal.caps.clientAllocatesUserD) {
        return TRUE;
    }

    i = nvPushGetSupportedClassIndex(pDevice, userModeClasses,
                                     sizeof(userModeClasses[0]),
                                     ARRAY_LEN(userModeClasses));
    if (i == -1) {
        return FALSE;
    }

    for (sd = 0; sd < pDevice->numSubDevices; sd++) {
        NvU32 ret;
        void *allocParams = NULL;

        NV_HOPPER_USERMODE_A_PARAMS hopperParams = { 0 };
        if (userModeClasses[i].classNumber != VOLTA_USERMODE_A) {
            allocParams = &hopperParams;
            // The BAR1 mapping is used for (faster and more efficient) writes
            // to perform work submission, but can't be used for reads.
            // If we ever want to read from the USERMODE region (e.g., to read
            // PTIMER) then we need a second mapping.
            hopperParams.bBar1Mapping = NV_TRUE;
        }

        pDevice->subDevice[sd].hUserMode =
            GetDeviceHandle(pParams, pUsedHandleBitmask);

        ret = nvPushImportRmApiAlloc(
                  pDevice,
                  pDevice->subDevice[sd].handle,
                  pDevice->subDevice[sd].hUserMode,
                  userModeClasses[i].classNumber,
                  allocParams);

        if (ret != NVOS_STATUS_SUCCESS) {
            pDevice->subDevice[sd].hUserMode = 0;
            goto fail;
        }

        ret = nvPushImportRmApiMapMemory(
                  pDevice,
                  pDevice->subDevice[sd].handle,
                  pDevice->subDevice[sd].hUserMode,
                  0, /* offset */
                  NVC361_NV_USERMODE__SIZE,
                  &pDevice->subDevice[sd].pUserMode,
                  0 /* flags */);

        if (ret != NVOS_STATUS_SUCCESS) {
            goto fail;
        }
    }

    return TRUE;

fail:
    FreeUserMode(pDevice);
    return FALSE;
}

static void CheckCaps(NvPushDevicePtr pDevice)
{
    int deviceIndex;

    pDevice->hostLBoverflowBug1667921 = FALSE;

    for (deviceIndex = 0;
         deviceIndex < __nvPushGetNumDevices(pDevice);
         deviceIndex++) {
        NV0080_CTRL_FIFO_GET_CAPS_V2_PARAMS fifoCapsParams = { 0 };
        NvU32 ret;

        ret = nvPushImportRmApiControl(pDevice,
                                   pDevice->subDevice[deviceIndex].deviceHandle,
                                   NV0080_CTRL_CMD_FIFO_GET_CAPS_V2,
                                   &fifoCapsParams,
                                   sizeof(fifoCapsParams));
        if (ret != NVOS_STATUS_SUCCESS) {
            nvAssert(!"Failed to determine chip fifo capabilities");
            return;
        }

        pDevice->hostLBoverflowBug1667921 |=
            !!NV0080_CTRL_FIFO_GET_CAP(fifoCapsParams.capsTbl,
              NV0080_CTRL_FIFO_CAPS_HAS_HOST_LB_OVERFLOW_BUG_1667921);
    }
}


static void FreeNotifiers(
    NvPushChannelPtr pChannel)
{
    NvPushDevicePtr pDevice = pChannel->pDevice;

    if (pChannel->notifiers.errorCtxDma != 0) {
        nvPushImportRmApiFree(pDevice,
                              pDevice->clientHandle,
                              pChannel->notifiers.errorCtxDma);
        pChannel->notifiers.errorCtxDma = 0;

    }

    if (pChannel->notifiers.gpuAddress != 0) {
        int deviceIndex;
        for (deviceIndex = __nvPushGetNumDevices(pDevice) - 1;
             deviceIndex >= 0;
             deviceIndex--) {
            nvPushImportRmApiUnmapMemoryDma(pDevice,
                                            pDevice->subDevice[deviceIndex].deviceHandle,
                                            pDevice->subDevice[deviceIndex].gpuVASpaceCtxDma,
                                            pChannel->notifiers.memoryHandle,
                                            0,
                                            pChannel->notifiers.gpuAddress);
        }
        pChannel->notifiers.gpuAddress = 0;
    }

    if (pChannel->notifiers.memoryHandle != 0) {
        nvPushImportRmApiFree(pDevice,
                              pDevice->subDevice[0].deviceHandle,
                              pChannel->notifiers.memoryHandle);
        pChannel->notifiers.memoryHandle = 0;
    }
}

/*
 * Allocate enough notifier memory to store:
 * - numNotifiers host driver requested NvNotifications, per subDevice
 * - NV_PUSH_NUM_INTERNAL_NOTIFIERS NvNotifications, per channel
 */
static NvBool AllocNotifiers(
    NvPushChannelPtr pChannel,
    const NvPushAllocChannelParams *pParams,
    NvBool coherent,
    NvU64 *pUsedHandleBitmask)
{
    NvPushDevicePtr pDevice = pChannel->pDevice;
    const NvU32 size =
        (((pParams->numNotifiers * pDevice->numSubDevices) +
          NV_PUSH_NUM_INTERNAL_NOTIFIERS) *
         sizeof(NvNotification));
    NV_CONTEXT_DMA_ALLOCATION_PARAMS ctxdmaParams = { 0 };

    NvU64 limit = size - 1;
    int deviceIndex;
    NvU32 ret;
    NvU32 allocFlags, gpuMapFlags;

    /*
     * The host-driver specified number of notifiers must not collide
     * with the reserved bit we use to indicate internal notifiers.
     */
    if (pParams->numNotifiers & NV_PUSH_NOTIFIER_INTERNAL_BIT) {
        return FALSE;
    }

    pChannel->notifiers.num = pParams->numNotifiers;
    pChannel->notifiers.memoryHandle =
        GetChannelHandle(pParams, pUsedHandleBitmask);

    allocFlags = DRF_DEF(OS02,_FLAGS,_PHYSICALITY,_NONCONTIGUOUS),
    gpuMapFlags = 0;
    if (coherent) {
        allocFlags = FLD_SET_DRF(OS02, _FLAGS, _COHERENCY, _WRITE_BACK, allocFlags);
        gpuMapFlags = FLD_SET_DRF(OS46, _FLAGS, _CACHE_SNOOP, _ENABLE, gpuMapFlags);
    } else {
        allocFlags = FLD_SET_DRF(OS02, _FLAGS, _COHERENCY, _UNCACHED, allocFlags);
        gpuMapFlags = FLD_SET_DRF(OS46, _FLAGS, _CACHE_SNOOP, _DISABLE, gpuMapFlags);
    }

    ret = nvPushImportRmApiAllocMemory64(
              pDevice,
              pDevice->subDevice[0].deviceHandle,
              pChannel->notifiers.memoryHandle,
              NV01_MEMORY_SYSTEM,
              allocFlags,
              (void **)&pChannel->notifiers.cpuAddress,
              &limit);

    if (ret != NVOS_STATUS_SUCCESS) {
        pChannel->notifiers.memoryHandle = 0;
        goto fail;
    }

    /* Map the memory into the GPU's VA space. */

    for (deviceIndex = 0;
         deviceIndex < __nvPushGetNumDevices(pDevice);
         deviceIndex++) {
        NvU32 mapFlags = gpuMapFlags;
        NvU64 gpuAddress;
        if (deviceIndex == 0) {
            /* For the first device, RM assigns a virtual address. */
            gpuAddress = 0;
        } else {
            /* For subsequent devices, use the same virtual address. */
            mapFlags = FLD_SET_DRF(OS46, _FLAGS, _DMA_OFFSET_FIXED, _TRUE,
                                   mapFlags);
            gpuAddress = pChannel->notifiers.gpuAddress;
            nvAssert(gpuAddress != 0);
        }
        ret = nvPushImportRmApiMapMemoryDma(
                  pDevice,
                  pDevice->subDevice[deviceIndex].deviceHandle,
                  pDevice->subDevice[deviceIndex].gpuVASpaceCtxDma,
                  pChannel->notifiers.memoryHandle,
                  0, /* offset */
                  size,
                  mapFlags,
                  &gpuAddress);

        if (ret != NVOS_STATUS_SUCCESS) {
            goto fail;
        }

        if (deviceIndex == 0) {
            pChannel->notifiers.gpuAddress = gpuAddress;
        } else {
            nvAssert(pChannel->notifiers.gpuAddress == gpuAddress);
        }
    }

    /* Create the internal notifier ctxDma. */

    pChannel->notifiers.errorCtxDma =
        GetChannelHandle(pParams, pUsedHandleBitmask);

    ctxdmaParams.hMemory = pChannel->notifiers.memoryHandle;
    ctxdmaParams.flags = DRF_DEF(OS03, _FLAGS, _MAPPING, _KERNEL) |
                         DRF_DEF(OS03, _FLAGS, _HASH_TABLE, _DISABLE);
    /* the internal notifiers are at the start of the memory */
    ctxdmaParams.offset = 0;
    ctxdmaParams.limit = (NV_PUSH_NUM_INTERNAL_NOTIFIERS *
                          sizeof(NvNotification)) - 1;

    ret = nvPushImportRmApiAlloc(pDevice,
                                 pDevice->subDevice[0].deviceHandle,
                                 pChannel->notifiers.errorCtxDma,
                                 NV01_CONTEXT_DMA,
                                 &ctxdmaParams);

    if (ret != NVOS_STATUS_SUCCESS) {
        pChannel->notifiers.errorCtxDma = 0;
        goto fail;
    }

    /*
     * Initialize the error notifier; note that there is only one
     * error notifier shared by all subdevices, so we specify master as the
     * subDeviceMask.
     */
    nvPushInitWaitForNotifier(pChannel,
                              NV_PUSH_ERROR_NOTIFIER_INDEX,
                              NV_PUSH_SUBDEVICE_MASK_PRIMARY);

    return TRUE;

fail:
    FreeNotifiers(pChannel);
    return FALSE;
}

static NvU32 GetExtendedBase(NvU64 offset)
{
    return NvU64_HI32(offset) >> 8;
}

static void InitGpFifoExtendedBase(
    NvPushChannelPtr pChannel)
{
    const NvU64 pbBase = pChannel->main.gpuMapOffset;
    const NvU32 extendedBase = GetExtendedBase(pbBase);
    NvU32 *gpPointer = &(pChannel->gpfifo[pChannel->gpPutOffset*2]);
    NvU32 i;

    if (!pChannel->pDevice->hal.caps.extendedBase) {
        nvAssert(extendedBase == 0);
        return;
    }

    /*
     * Because of the natural VA alignment specified when allocating the
     * pushbuffer, all parts of the pushbuffer surface should be in the same
     * 40-bit region.
     */
    nvAssert(GetExtendedBase(pChannel->main.gpuMapOffset) ==
             GetExtendedBase(pChannel->progressTracker.gpuMapOffset));
    nvAssert(GetExtendedBase(pChannel->main.gpuMapOffset +
                             pChannel->main.sizeInBytes - 1) ==
             GetExtendedBase(pChannel->main.gpuMapOffset));
    nvAssert(GetExtendedBase(pChannel->progressTracker.gpuMapOffset +
                             pChannel->progressTracker.sizeInBytes - 1) ==
             GetExtendedBase(pChannel->progressTracker.gpuMapOffset));

    /* Set the "extended base" for all subsequent methods */
    gpPointer[0] = DRF_NUM(C86F, _GP_ENTRY0, _PB_EXTENDED_BASE_OPERAND, extendedBase);
    gpPointer[1] = DRF_DEF(C86F, _GP_ENTRY1, _OPCODE, _SET_PB_SEGMENT_EXTENDED_BASE);
    gpPointer += 2;

    /* Pad out with NOP GPFIFO methods so everything remains aligned. */
    for (i = 1; i < NV_PUSH_NUM_GPFIFO_ENTRIES_PER_KICKOFF; i++) {
        gpPointer[0] = 0;
        gpPointer[1] = DRF_DEF(C86F, _GP_ENTRY1, _OPCODE, _NOP);
        gpPointer += 2;
    }

    pChannel->gpPutOffset += NV_PUSH_NUM_GPFIFO_ENTRIES_PER_KICKOFF;

}

NvBool nvPushAllocChannel(
    const NvPushAllocChannelParams *pParams,
    NvPushChannelPtr buffer)
{
    NvPushDevicePtr pDevice;
    void  *cpuAddress = NULL;
    NvU64  gpuAddress = 0;
    NvU64  usedHandleBitmask = 0;
    NvBool coherent = FALSE;

    NVMISC_MEMSET(buffer, 0, sizeof(*buffer));

    pDevice = pParams->pDevice;

    buffer->pDevice = pDevice;
    buffer->logNvDiss = pParams->logNvDiss;
    buffer->noTimeout = pParams->noTimeout;
    buffer->ignoreChannelErrors = pParams->ignoreChannelErrors;

    buffer->currentSubDevMask = NV_PUSH_SUBDEVICE_MASK_ALL;

    /*
     * Assign main.sizeInBytes early, because the rest of
     * initialization relies on knowing the main pushbuffer size.
     * Note this must fit in NV_PUSH_PROGRESS_TRACKER_SEMAPHORE_GET,
     * which stores dwords.
     */
    nvAssert((DRF_MASK(NV_PUSH_PROGRESS_TRACKER_SEMAPHORE_GET) * 4) >
             pParams->pushBufferSizeInBytes);
    buffer->main.sizeInBytes = pParams->pushBufferSizeInBytes;

    /*
     * Compute numGpFifoEntries.  There are several constraints:
     *
     * - We make numGpFifoEntries 1/64th the size of the main
     *   pushbuffer.  The maximum pushbuffer size is 1048572, and we
     *   consume 2 gpFifo entries per kickoff.  This works out to be
     *   128 bytes of pushbuffer (32 dwords) per kickoff, before we
     *   are gpFifo-limited.
     *
     * - Per dev_pbdma.ref, "The number of GP entries in the circular
     *   buffer is always a power of 2."  So, round up to the next
     *   power of two.
     *
     * - Because we consume 2 gpFifo entries per kickoff
     *   (NV_PUSH_NUM_GPFIFO_ENTRIES_PER_KICKOFF), we also align to a
     *   multiple of 2.  This should be guaranteed by the power of 2
     *   check.
     *
     * - numGpFifoEntries must fit in
     *   NV_PUSH_PROGRESS_TRACKER_SEMAPHORE_GP_GET so that the
     *   progress tracker semaphore releases can report the consumed
     *   gpFifo entry.  The distribution of bits in
     *   NV_PUSH_PROGRESS_TRACKER_SEMAPHORE should ensure this is
     *   satisfied.
     */

    buffer->numGpFifoEntries = pParams->pushBufferSizeInBytes / 64;

    buffer->numGpFifoEntries = nvNextPow2_U32(buffer->numGpFifoEntries);

    nvAssert((buffer->numGpFifoEntries %
              NV_PUSH_NUM_GPFIFO_ENTRIES_PER_KICKOFF) == 0);

    nvAssert((DRF_MASK(NV_PUSH_PROGRESS_TRACKER_SEMAPHORE_GP_GET) * 2) >
             buffer->numGpFifoEntries);

    if (!GetCoherenceFlags(buffer, &coherent)) {
        goto failed;
    }

    if (!AllocNotifiers(buffer, pParams, coherent, &usedHandleBitmask)) {
        nvPushImportLogError(pDevice,
                             "Failed to allocate notification memory.");
        goto failed;
    }

    /* Only allocate memory for one pushbuffer.  All subdevices will share */
    if (!AllocPushbuffer(buffer,
                         pParams,
                         &usedHandleBitmask,
                         &cpuAddress,
                         &gpuAddress)) {
        nvPushImportLogError(pDevice,
                             "Push buffer DMA allocation failed");
        goto failed;
    }

    /* First the "main" pushbuffer */
    InitDmaSegment(&buffer->main,
                   cpuAddress,
                   gpuAddress,
                   pParams->pushBufferSizeInBytes);
    /* Next the GPFIFO */
    buffer->gpfifo =
        (NvU32 *)((char *)cpuAddress +  __nvPushGpFifoOffset(buffer));
    buffer->gpPutOffset = 0;
    /* Next the "progressTracker" */
    InitDmaSegment(&buffer->progressTracker,
                   (char *)cpuAddress + __nvPushProgressTrackerOffset(buffer),
                   gpuAddress + __nvPushProgressTrackerOffset(buffer),
                   ProgressTrackerBufferSize(buffer));

    if (!nvDmaAllocUserD(buffer, pParams, &usedHandleBitmask)) {
        goto failed;
    }

    if (!AllocChannelObject(buffer, pParams,
                            &usedHandleBitmask, gpuAddress)) {
        goto failed;
    }

    if (pDevice->hal.caps.clientAllocatesUserD &&
        !RequestChidToken(buffer)) {
        goto failed;
    }

    if (!AllocSemaSurface(buffer, pParams, coherent, &usedHandleBitmask)) {
        goto failed;
    }

#if defined(DEBUG)
    if (buffer->logNvDiss) {
        nvPushImportLogNvDiss(buffer, "nvdiss:  encoding 2\n");
    }
#endif /* DEBUG */

    InitGpFifoExtendedBase(buffer);

    if (!__nvPushTestPushBuffer(buffer)) {
        goto failed;
    }

    buffer->initialized = TRUE;

    return TRUE;

failed:
    nvPushFreeChannel(buffer);
    return FALSE;
}

/*!
 * Free resources allocated by AllocChannel().
 */
void nvPushFreeChannel(NvPushChannelPtr buffer)
{
    NvPushDevicePtr pDevice = buffer->pDevice;
    unsigned int sd;
    int deviceIndex;

    if (pDevice == NULL) {
        goto done;
    }

    /* Unmap pushbuffer DMA controls */
    for (sd = 0; sd < pDevice->numSubDevices; sd++) {
        NvU32 userdMapHandle;

        deviceIndex = __nvPushGetDeviceIndex(pDevice, sd);
        if (pDevice->hal.caps.clientAllocatesUserD) {
            userdMapHandle = buffer->userD[deviceIndex].hMemory;
        } else {
            userdMapHandle = buffer->channelHandle[deviceIndex];
        }

        if (buffer->control[sd]) {
            nvPushImportRmApiUnmapMemory(pDevice,
                                         pDevice->subDevice[sd].handle,
                                         userdMapHandle,
                                         buffer->control[sd],
                                         0);
            buffer->control[sd] = NULL;
        }
    }

    for (deviceIndex = __nvPushGetNumDevices(pDevice) - 1;
         deviceIndex >= 0;
         deviceIndex--) {
        if (buffer->channelHandle[deviceIndex] != 0) {
            nvPushImportRmApiFree(pDevice,
                                  pDevice->subDevice[deviceIndex].deviceHandle,
                                  buffer->channelHandle[deviceIndex]);
            buffer->channelHandle[deviceIndex] = 0;
        }

        if (buffer->userD[deviceIndex].hMemory != 0) {
            nvPushImportRmApiFree(pDevice,
                                  pDevice->subDevice[deviceIndex].deviceHandle,
                                  buffer->userD[deviceIndex].hMemory);
            buffer->userD[deviceIndex].hMemory = 0;
        }

        if (buffer->pushbufferVAHandle[deviceIndex] != 0) {
            nvPushImportRmApiFree(pDevice,
                                  pDevice->subDevice[deviceIndex].deviceHandle,
                                  buffer->pushbufferVAHandle[deviceIndex]);
            buffer->pushbufferVAHandle[deviceIndex] = 0;
        }
    }

    if (buffer->pushbufferHandle != 0) {
        nvPushImportRmApiFree(pDevice,
                              pDevice->subDevice[0].deviceHandle,
                              buffer->pushbufferHandle);
        buffer->pushbufferHandle = 0;
    }

    FreeNotifiers(buffer);

    FreeSemaSurface(buffer);

done:
    NVMISC_MEMSET(buffer, 0, sizeof(*buffer));
}

NvBool nvPushAllocDevice(
    const NvPushAllocDeviceParams *pParams,
    NvPushDevicePtr pDevice)
{
    unsigned int sd;
    NvU64 usedHandleBitmask = 0;

    NVMISC_MEMSET(pDevice, 0, sizeof(*pDevice));

    pDevice->hostDevice       = pParams->hostDevice;
    pDevice->pImports         = pParams->pImports;
    pDevice->numSubDevices    = pParams->numSubDevices;
    pDevice->clientSli        = pParams->clientSli;
    pDevice->clientHandle     = pParams->clientHandle;

    pDevice->numClasses       = pParams->numClasses;
    pDevice->supportedClasses = pParams->supportedClasses;

    pDevice->hasFb            = IsClassSupported(pDevice, NV01_MEMORY_LOCAL_USER);

    pDevice->confidentialComputeMode  = pParams->confidentialComputeMode;

    for (sd = 0; sd < pParams->numSubDevices; sd++) {
        pDevice->subDevice[sd].handle = pParams->subDevice[sd].handle;
        pDevice->subDevice[sd].deviceHandle = pParams->subDevice[sd].deviceHandle;
        pDevice->subDevice[sd].gpuVASpaceObject = pParams->subDevice[sd].gpuVASpaceObject;
        pDevice->subDevice[sd].gpuVASpaceCtxDma = pParams->subDevice[sd].gpuVASpace;
    }

    if (pParams->amodel.config != NV_AMODEL_NONE) {
        nvAssert(!"Ignoring AModel configuration on non-XAMODEL build");
    }
    pDevice->amodelConfig = pParams->amodel.config;

    CheckCaps(pDevice);

    if (!GetChannelClassAndUserDSize(pDevice, pParams)) {
        nvPushImportLogError(pDevice,
                             "No supported command buffer format found");
        goto fail;
    }

    if (!__nvPushGetHal(pParams, pDevice->gpfifoClass, &pDevice->hal)) {
        nvPushImportLogError(pDevice, "No push buffer implementation found.");
        goto fail;
    }

    if (!AllocUserMode(pDevice, pParams, &usedHandleBitmask)) {
        nvPushImportLogError(pDevice,
                             "Unable to allocate push buffer controls.");
        goto fail;
    }


    return TRUE;

fail:
    nvPushFreeDevice(pDevice);

    return FALSE;
}

void nvPushFreeDevice(
    NvPushDevicePtr pDevice)
{
    FreeUserMode(pDevice);

    NVMISC_MEMSET(pDevice, 0, sizeof(*pDevice));
}
