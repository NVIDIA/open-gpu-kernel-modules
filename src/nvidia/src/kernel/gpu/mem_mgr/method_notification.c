/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/********************************* DMA Manager *****************************\
*                                                                           *
*   Method notifications are handled in this module.  DMA report and OS     *
*   action are dealt with on a per-object basis.                            *
*                                                                           *
****************************************************************************/

#include "core/core.h"
#include "gpu/gpu.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "gpu/mem_mgr/virt_mem_allocator_common.h"
#include "gpu/mem_mgr/context_dma.h"
#include "os/os.h"
#include "gpu/timer/objtmr.h"
#include "gpu/device/device.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "platform/sli/sli.h"

//---------------------------------------------------------------------------
//
//  Notification completion.
//
//---------------------------------------------------------------------------

void notifyMethodComplete
(
    OBJGPU   *pGpu,
    ChannelDescendant *pObject,
    NvU32     Offset,
    NvV32     Data,
    NvU32     info32,
    NvU16     info16,
    NV_STATUS CompletionStatus
)
{
    if (pObject->bNotifyTrigger)
    {
        pObject->bNotifyTrigger = NV_FALSE;

        //
        // Do any OS specified action related to this notification.
        //
        if (pObject->notifyAction)
        {
            PEVENTNOTIFICATION pEventNotifications = inotifyGetNotificationList(staticCast(pObject, INotifier));
            notifyEvents(pGpu, pEventNotifications, 0, Offset, Data, CompletionStatus, pObject->notifyAction);
        }
    }
}

static NV_STATUS notifyWriteNotifier
(
    OBJGPU    *pGpu,
    ContextDma *NotifyXlate,
    NvV32      Info32,
    NvV16      Info16,
    NV_STATUS  CompletionStatus,
    NvU64      Offset,
    NvBool     TimeSupplied,
    NvU64      Time
)
{
    NV_STATUS status;
    NOTIFICATION *pNotifyBuffer;

    //
    // Fill in the notification structure.
    //
    status = ctxdmaGetKernelVA( NotifyXlate, Offset, sizeof(*pNotifyBuffer),
        (void **)&(pNotifyBuffer),
        gpumgrGetSubDeviceInstanceFromGpu(gpumgrGetParentGPU(pGpu)));

    if (status != NV_OK)
    {
        return status;
    }

    notifyFillNOTIFICATION(pGpu, pNotifyBuffer, Info32, Info16,
                           CompletionStatus, TimeSupplied, Time);
    return status;
}

void
notifyFillNOTIFICATION
(
    OBJGPU       *pGpu,
    NOTIFICATION *pNotifyBuffer,
    NvV32         Info32,
    NvV16         Info16,
    NV_STATUS     CompletionStatus,
    NvBool        TimeSupplied,
    NvU64         Time
)
{
    INFO16_STATUS infoStatus;
    NvU32         TimeHi, TimeLo;

    if (!TimeSupplied)
    {
        OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
        tmrGetCurrentTime(pTmr, &Time);
    }

    TimeLo = NvU64_LO32(Time);
    TimeHi = NvU64_HI32(Time);

    //
    // Since notifiers are not read by the GPU, and only the CPU, these
    // writes to not need to be flushed. A subsequent CPU read to this data
    // will be serialized with these writes
    //
    MEM_WR32(&pNotifyBuffer->OtherInfo32, Info32);
    MEM_WR32(&pNotifyBuffer->TimeHi, TimeHi);
    MEM_WR32(&pNotifyBuffer->TimeLo, TimeLo);

    //
    // Combine into 32b write to avoid issues in environments that don't
    // support 16b writes.  For example, when routing all memory requests
    // through IFB we are limited to 32b read/writes only.
    //
    infoStatus.Info16Status_16.Status = (NvV16) CompletionStatus;
    infoStatus.Info16Status_16.OtherInfo16 = Info16;
    MEM_WR32(&pNotifyBuffer->Info16Status.Info16Status_32,
             infoStatus.Info16Status_32);
}

void
notifyFillNvNotification
(
    OBJGPU         *pGpu,
    NvNotification *pNotification,
    NvV32           Info32,
    NvV16           Info16,
    NV_STATUS       CompletionStatus,
    NvBool          TimeSupplied,
    NvU64           Time
)
{
    NvU32 TimeHi, TimeLo;

    if (!TimeSupplied)
    {
        OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
        tmrGetCurrentTime(pTmr, &Time);
    }

    TimeLo = NvU64_LO32(Time);
    TimeHi = NvU64_HI32(Time);

    //
    // Since notifiers are not read by the GPU, and only the CPU, these
    // writes do not need to be flushed. A subsequent CPU read to this data
    // will be serialized with these writes
    //
    MEM_WR16(&pNotification->info16, Info16);
    MEM_WR32(&pNotification->info32, Info32);
    MEM_WR32(&pNotification->timeStamp.nanoseconds[0], TimeHi);
    MEM_WR32(&pNotification->timeStamp.nanoseconds[1], TimeLo);
    MEM_WR16(&pNotification->status, CompletionStatus);
}

NV_STATUS notifyFillNotifier
(
    OBJGPU    *pGpu,
    ContextDma *NotifyXlate,
    NvV32      Info32,
    NvV16      Info16,
    NV_STATUS  CompletionStatus
)
{
    return notifyWriteNotifier(pGpu, NotifyXlate, Info32,
                               Info16, CompletionStatus,
                               0, NV_FALSE, 0);
}

NV_STATUS notifyFillNotifierOffsetTimestamp
(
    OBJGPU    *pGpu,
    ContextDma *NotifyXlate,
    NvV32      Info32,
    NvV16      Info16,
    NV_STATUS  CompletionStatus,
    NvU64      Offset,
    NvU64      Time
)
{
    return notifyWriteNotifier(pGpu, NotifyXlate, Info32,
                               Info16, CompletionStatus,
                               Offset,
                               NV_TRUE, Time);
}

NV_STATUS notifyFillNotifierOffset
(
    OBJGPU    *pGpu,
    ContextDma *NotifyXlate,
    NvV32      Info32,
    NvV16      Info16,
    NV_STATUS  CompletionStatus,
    NvU64      Offset
)
{
    return notifyWriteNotifier(pGpu, NotifyXlate, Info32,
                               Info16, CompletionStatus,
                               Offset,
                               NV_FALSE, 0);
}

NV_STATUS notifyFillNotifierArrayTimestamp
(
    OBJGPU    *pGpu,
    ContextDma *NotifyXlate,
    NvV32      Info32,
    NvV16      Info16,
    NV_STATUS  CompletionStatus,
    NvU32      Index,
    NvU64      Time
)
{
    return notifyWriteNotifier(pGpu, NotifyXlate, Info32,
                               Info16, CompletionStatus,
                               Index * sizeof(NOTIFICATION),
                               NV_TRUE, Time);
}

NV_STATUS notifyFillNotifierArray
(
    OBJGPU    *pGpu,
    ContextDma *NotifyXlate,
    NvV32      Info32,
    NvV16      Info16,
    NV_STATUS  CompletionStatus,
    NvU32      Index
)
{
    return notifyWriteNotifier(pGpu, NotifyXlate, Info32,
                               Info16, CompletionStatus,
                               Index * sizeof(NOTIFICATION),
                               NV_FALSE, 0);
}

/*
 * @brief fills notifier at GPU VA base + index with given info,
 * time and completion status
 *
 * Looks up dma memory mapping with given GPU VA and performs writes.
 * Notifier write is skipped when CPU kernel mapping is missing.
 *
 * @param[in] pGpu              OBJGPU pointer
 * @param[in] pDevice           Device pointer
 * @param[in] hMemoryCtx        Handle of a memory object to which NotifyGPUVABase belongs
 * @param[in] NotifyGPUVABase   64b GPU VA base address of semaphore
 * @param[in] Info32            32b info part
 * @param[in] Info16            16b info part
 * @param[in] CompletionStatus  NV_STATUS value to write to notifier status
 * @param[in] Index             index of notifier in notifier array
 * @param[in] Time              64b time stamp
 *
 * @return NV_ERR_INVALID_ADDRESS on wrong GPU VA address or out of bound index,
 *         NV_OK on success
 *
 */
NV_STATUS notifyFillNotifierGPUVATimestamp
(
    OBJGPU    *pGpu,
    Device    *pDevice,
    NvHandle   hMemoryCtx,
    NvU64      NotifyGPUVABase,
    NvV32      Info32,
    NvV16      Info16,
    NV_STATUS  CompletionStatus,
    NvU32      Index,
    NvU64      Time
)
{
    NvU64                 notifyGPUVA;
    NvBool                bFound;
    CLI_DMA_MAPPING_INFO *pDmaMappingInfo;
    NvU64                 offset;
    NvU32                 subdeviceInstance;
    NOTIFICATION         *pNotifier;

    notifyGPUVA = NotifyGPUVABase + (Index * sizeof(NOTIFICATION));

    // Memory context is required for mapping lookup
    bFound = CliGetDmaMappingInfo(RES_GET_CLIENT(pDevice),
                                  RES_GET_HANDLE(pDevice),
                                  hMemoryCtx,
                                  notifyGPUVA,
                                  gpumgrGetDeviceGpuMask(pGpu->deviceInstance),
                                  &pDmaMappingInfo);
    if (!bFound)
    {
        NV_PRINTF(LEVEL_ERROR, "Can't find mapping; notifier not written\n");
        return NV_ERR_INVALID_ADDRESS;
    }

    offset = notifyGPUVA - pDmaMappingInfo->DmaOffset;
    if ((offset + sizeof(NOTIFICATION)) > pDmaMappingInfo->pMemDesc->Size)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "offset+size doesn't fit into mapping; notifier not written\n");
        return NV_ERR_INVALID_ADDRESS;
    }

    //
    // Set idx to default position in the dma mapped address array
    //
    subdeviceInstance = gpumgrGetSubDeviceInstanceFromGpu(gpumgrGetParentGPU(pGpu));

    SLI_LOOP_START(SLI_LOOP_FLAGS_NONE)

    if (IsSLIEnabled(pGpu) &&
        (memdescGetAddressSpace(pDmaMappingInfo->pMemDesc) == ADDR_FBMEM))
    {
        //
        // If SLI and it is vidmem, replace idx with appropriate SLI index
        // otherwise, this just stays the default value.
        //
        subdeviceInstance = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
    }

    if (!pDmaMappingInfo->KernelVAddr[subdeviceInstance])
    {
        NV_PRINTF(LEVEL_ERROR, "KernelVAddr==NULL; notifier not written\n");
    }
    else
    {
        pNotifier = (PNOTIFICATION)((NvU8*)pDmaMappingInfo->KernelVAddr[subdeviceInstance] + offset);

        notifyFillNOTIFICATION(pGpu, pNotifier, Info32, Info16,
                               CompletionStatus, NV_TRUE, Time);
    }

    SLI_LOOP_END

    return NV_OK;
}

/*
 * @brief fills notifier at GPU VA base + index with current time, given info,
 * and completion status
 *
 * Use this function to fill notifier through BAR1 when you have GPU VA.
 *
 * Wrapper for notifyFillNotifierGPUVATimestamp.
 * Gets current time and routes data to notifyFillNotifierGPUVATimestamp
 *
 * @param[in] pGpu              OBJGPU pointer
 * @param[in] pDevice           Device pointer
 * @param[in] hMemoryCtx        Handle of a memory object to which NotifyGPUVABase belongs
 * @param[in] NotifyGPUVABase   64b GPU VA base address of semaphore
 * @param[in] Info32            32b info part
 * @param[in] Info16            16b info part
 * @param[in] CompletionStatus  NV_STATUS value to write to notifier status
 * @param[in] Index             index of notifier in notifier array
 * @param[in] Time              64b time stamp
 *
 * @return status of notifyFillNotifierGPUVATimestamp
 */
NV_STATUS notifyFillNotifierGPUVA
(
    OBJGPU    *pGpu,
    Device    *pDevice,
    NvHandle   hMemoryCtx,
    NvU64      NotifyGPUVABase,
    NvV32      Info32,
    NvV16      Info16,
    NV_STATUS  CompletionStatus,
    NvU32      Index
)
{
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    NvU64   Time;

    tmrGetCurrentTime(pTmr, &Time);

    return notifyFillNotifierGPUVATimestamp(pGpu,
                                            pDevice,
                                            hMemoryCtx,
                                            NotifyGPUVABase,
                                            Info32,
                                            Info16,
                                            CompletionStatus,
                                            Index,
                                            Time);
}

/*
 * @brief fills notifiers by given memory info and index with given time, info,
 * and completion status
 *
 * Use this function to fill notifier through BAR2 when you have memory info.
 *

 * @param[in] pGpu              OBJGPU pointer
 * @param[in] hClient           NvU32 client handle
 * @param[in] NotifyGPUVABase   64b GPU VA base address of semaphore
 * @param[in] Info32            32b info part
 * @param[in] Info16            16b info part
 * @param[in] CompletionStatus  NV_STATUS value to write to notifier status
 * @param[in] Index             index of notifier in notifier array
 *
 * @return NV_ERR_GENERIC if RM aperture mapping failed.
 */
NV_STATUS notifyFillNotifierMemoryTimestamp
(
    OBJGPU       *pGpu,
    Memory       *pMemory,
    NvV32         Info32,
    NvV16         Info16,
    NV_STATUS     CompletionStatus,
    NvU32         Index,
    NvU64         Time
)
{
    NvNotification * pDebugNotifier = NULL;
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    TRANSFER_SURFACE surf = {0};

    //
    // Check if there's already a CPU mapping we can use. If not, attempt to
    // map the notifier, which may fail if we're in a context where we can't
    // create mappings.
    //
    pDebugNotifier = (NvNotification *)((NvUPtr)pMemory->KernelVAddr);
    if (pDebugNotifier == NULL)
    {
        surf.pMemDesc = pMemory->pMemDesc;
        surf.offset = Index * sizeof(NvNotification);

        pDebugNotifier =
            (NvNotification *) memmgrMemBeginTransfer(pMemoryManager, &surf,
                                                      sizeof(NvNotification),
                                                      TRANSFER_FLAGS_SHADOW_ALLOC);
        NV_ASSERT_OR_RETURN(pDebugNotifier != NULL, NV_ERR_INVALID_STATE);
    }
    else
    {
        //
        // If a CPU pointer has been passed by caller ensure that the notifier
        // is in sysmem or in case it in vidmem, BAR access to the same is not
        // blocked (for HCC)
        //
        NV_ASSERT_OR_RETURN(
            memdescGetAddressSpace(pMemory->pMemDesc) == ADDR_SYSMEM ||
            !kbusIsBarAccessBlocked(pKernelBus), NV_ERR_INVALID_ARGUMENT);
        pDebugNotifier = &pDebugNotifier[Index];
    }

    notifyFillNvNotification(pGpu, pDebugNotifier, Info32, Info16,
                             CompletionStatus, NV_TRUE, Time);

    if (pMemory->KernelVAddr == NvP64_NULL)
    {
        memmgrMemEndTransfer(pMemoryManager, &surf, sizeof(NvNotification), 0);
    }

    return NV_OK;
}

/*
 * @brief fills notifiers by given memory info and index with current time,
 * info and completion status.
 *
 * Use this function to fill notifier through BAR2 when you have memory info.
 *
 * Current time wrapper around notifyFillNotifierMemoryTimestamp.
 *
 * @param[in] pGpu              OBJGPU pointer
 * @param[in] hClient           NvU32 client handle
 * @param[in] NotifyGPUVABase   64b GPU VA base address of semaphore
 * @param[in] Info32            32b info part
 * @param[in] Info16            16b info part
 * @param[in] CompletionStatus  NV_STATUS value to write to notifier status
 * @param[in] Index             index of notifier in notifier array
 *
 * @return status of notifyFillNotifierMemoryTimestamp
 */
NV_STATUS notifyFillNotifierMemory
(
    OBJGPU    *pGpu,
    Memory    *pMemory,
    NvV32      Info32,
    NvV16      Info16,
    NV_STATUS  CompletionStatus,
    NvU32      Index
)
{
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    NvU64   Time;

    tmrGetCurrentTime(pTmr, &Time);

    return notifyFillNotifierMemoryTimestamp(pGpu,
                                             pMemory,
                                             Info32,
                                             Info16,
                                             CompletionStatus,
                                             Index,
                                             Time);

}

/*
 * @brief fill semaphore structure at GPU VA base given time and release value
 *
 * Looks up dma memory mapping with given GPU VA and performs writes.
 * Semaphore write is skipped when CPU kernel mapping is missing.
 *
 * @param[in] pGpu                  OBJGPU pointer
 * @param[in] pDevice               Device pointer
 * @param[in] SemaphoreGPUVABase    64b GPU VA base address of semaphore
 * @param[in] ReleaseValue          NvU32 value to write to semaphore upon release
 * @param[in] Index                 index of semaphore in semaphore array
 * @param[in] Time                  64b time stamp
 *
 * @return NV_ERR_INVALID_ADDRESS on wrong GPU VA address or out of bound index,
 *         NV_OK on success
 *
 */
NV_STATUS semaphoreFillGPUVATimestamp
(
    OBJGPU    *pGpu,
    Device    *pDevice,
    NvHandle   hMemCtx,
    NvU64      SemaphoreGPUVABase,
    NvV32      ReleaseValue,
    NvU32      Index,
    NvBool     bBroadcast,
    NvU64      Time
)
{
    NvU64                 semaphoreGPUVA;
    NvU64                 semaphoreGPUVAOffset;
    CLI_DMA_MAPPING_INFO *pDmaMappingInfo;
    NvU64                 offset;
    NvU32                 timeHi, timeLo;
    NvU32                 subdeviceInstance;
    NvGpuSemaphore       *pSemaphore;
    NvBool                bBcState = gpumgrGetBcEnabledStatus(pGpu);
    NvBool                bFound;

    if (!portSafeMulU64((NvU64) Index,
                        (NvU64) sizeof(NvGpuSemaphore),
                        &semaphoreGPUVAOffset) ||
        !portSafeAddU64(SemaphoreGPUVABase,
                        semaphoreGPUVAOffset,
                        &semaphoreGPUVA))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    bFound = CliGetDmaMappingInfo(RES_GET_CLIENT(pDevice),
                                  RES_GET_HANDLE(pDevice),
                                  hMemCtx,
                                  semaphoreGPUVA,
                                  gpumgrGetDeviceGpuMask(pGpu->deviceInstance),
                                  &pDmaMappingInfo);
    if (!bFound)
    {
        NV_PRINTF(LEVEL_ERROR, "Can't find mapping; semaphore not released\n");
        return NV_ERR_INVALID_ADDRESS;
    }

    offset = semaphoreGPUVA - pDmaMappingInfo->DmaOffset;
    if ((offset + sizeof(NvGpuSemaphore)) > pDmaMappingInfo->pMemDesc->Size)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "offset+size doesn't fit into mapping; semaphore not released\n");
        return NV_ERR_INVALID_ADDRESS;
    }

    timeLo = NvU64_LO32(Time);
    timeHi = NvU64_HI32(Time);

    //
    // Set idx to default position in the dma mapped address array
    //
    subdeviceInstance = gpumgrGetSubDeviceInstanceFromGpu(gpumgrGetParentGPU(pGpu));

    osFlushCpuWriteCombineBuffer();

    gpumgrSetBcEnabledStatus(pGpu, bBroadcast);
    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)

    if (IsSLIEnabled(pGpu) &&
        (memdescGetAddressSpace(pDmaMappingInfo->pMemDesc) == ADDR_FBMEM))
    {
        //
        // If SLI and it is vidmem, replace idx with appropriate SLI index
        // otherwise, this just stays the default value.
        //
        subdeviceInstance = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
    }

    if (!pDmaMappingInfo->KernelVAddr[subdeviceInstance])
    {
        NV_PRINTF(LEVEL_ERROR, "KernelVAddr==NULL; semaphore not released\n");
    }
    else
    {
        pSemaphore = (NvGpuSemaphore*)((NvU8*)pDmaMappingInfo->KernelVAddr[subdeviceInstance] + offset);

        MEM_WR32(&(pSemaphore->timeStamp.nanoseconds[0]), timeLo);
        MEM_WR32(&(pSemaphore->timeStamp.nanoseconds[1]), timeHi);
        MEM_WR32(&(pSemaphore->data[0]), ReleaseValue);
    }

    SLI_LOOP_END

    gpumgrSetBcEnabledStatus(pGpu, bBcState);
    osFlushCpuWriteCombineBuffer();

    return NV_OK;
}

/*
 * @brief fill semaphore at GPU VA with given release value and current time stamp
 *
 * Use this function to fill Semaphore through BAR1 when you have GPU VA.
 *
 * Wrapper for semaphore handling. Gets current time and routes data to
 * semaphoreFillGPUVATimestamp.
 *
 * @param[in] pGpu                  OBJGPU pointer
 * @param[in] pDevice               Device pointer
 * @param[in] SemaphoreGPUVABase    64b GPU VA base address of semaphore
 * @param[in] ReleaseValue          NvU32 value to write to semaphore upon release
 * @param[in] Index                 index of semaphore in semaphore array
 *
 * @return status of semaphoreFillGPUVATimestamp
 */
NV_STATUS semaphoreFillGPUVA
(
    OBJGPU    *pGpu,
    Device    *pDevice,
    NvHandle   hMemCtx,
    NvU64      SemaphoreGPUVABase,
    NvV32      ReleaseValue,
    NvU32      Index,
    NvBool     bBroadcast
)
{
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    NvU64   Time;

    tmrGetCurrentTime(pTmr, &Time);

    return semaphoreFillGPUVATimestamp(pGpu,
                                       pDevice,
                                       hMemCtx,
                                       SemaphoreGPUVABase,
                                       ReleaseValue,
                                       Index,
                                       bBroadcast,
                                       Time);
}
