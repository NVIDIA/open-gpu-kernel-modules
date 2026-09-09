/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "nvstatus.h"
#include "nvtypes.h"
#include "nvmisc.h"
#include "os/os.h"
#include "nvrm_registry.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/gpu.h"
#include "gpu/gpu_access.h"
#include "gpu/oob/nsmapi.h"
#include "gpu/oob/drv_event.h"
#include "gpu/oob/kernel_oob.h"
#include "gpu/gsp/kernel_gsp.h"
#include "gpu/falcon/kernel_falcon.h"
#include "swref/published/blackwell/gb100/dev_oob_pri.h"

//
// In my observation, transactions generally complete within a few
// hundred usec, so bumping this by an order of magnitude should
// provide a reasonably safe margin while avoiding holding the GPU lock
// for many seconds that the default timeout would imply.
//
#define OOBHUB_SEND_TIMEOUT_US  500000

static NvU32 _koobMnocMboxSend_HELPER(OBJGPU *pGpu, KernelOob *pKernelOob, NvU8 *pSendBuffer, NvU32 msgSize);
static void _koobMnocMboxSend_WORKITEM(NvU32 gpuInstance, void *pArgs);
static KoobMnocMboxSendParams *_koobAllocSendBuf(KernelOob *pKernelOob, NvBool bToWorkItem);
static void _koobFreeSendBuf(KernelOob *pKernelOob, NvBool bToWorkItem, KoobMnocMboxSendParams *pBuf);

static NV_STATUS
_mnocBufferPoolConstruct
(
    KoobMnocBufferPool  *pPool
)
{
    pPool->pBufSpinLock = portSyncSpinlockCreate(portMemAllocatorGetGlobalNonPaged());
    if (pPool->pBufSpinLock == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Alloc spinlock failed\n");
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }
    pPool->availableSendBufSlots = NV_BITMASK64(KOOB_MNOC_NUM_BUFS);

    return NV_OK;
}

static void
_mnocBufferPoolDestruct
(
    KoobMnocBufferPool  *pPool
)
{
    portSyncSpinlockDestroy(pPool->pBufSpinLock);
    pPool->pBufSpinLock = NULL;
}

NV_STATUS
koobStateLoad_GB100
(
    OBJGPU    *pGpu,
    KernelOob *pKernelOob,
    NvU32      flags
)
{
    NV_ASSERT_OK_OR_RETURN(_mnocBufferPoolConstruct(&pKernelOob->mnocBufferPool));
    koobDriverStartupNotify(pGpu, pKernelOob);
    return NV_OK;
}

NV_STATUS
koobStateUnload_GB100
(
    OBJGPU    *pGpu,
    KernelOob *pKernelOob,
    NvU32      flags
)
{
    koobDriverShutdownNotify(pGpu, pKernelOob);
    _mnocBufferPoolDestruct(&pKernelOob->mnocBufferPool);
    return NV_OK;
}

NV_STATUS
koobInitAperture_GB100
(
    OBJGPU *pGpu,
    KernelOob *pKernelOob
)
{
    return ioaprtInit(&pKernelOob->mboxAperture, pGpu->pIOApertures[DEVICE_INDEX_GPU],
                      NV_POOBHUB_PEREGRINE_MNOC_RX_FIFO_DATA(0), 0x200);
}

/*!
 * Compute whether sending messages OOB is supported by config and FW
 *
 * @param[in]  pGpu            GPU object pointer
 * @param[in]  pKernelOob      KernelOob object pointer
 *
 * @return  NV_TRUE  if supported by config and FW
 *          NV_FALSE otherwise
 */
NvBool
koobCheckSupported_GB100
(
    OBJGPU *pGpu,
    KernelOob *pKernelOob
)
{
    NvU32 privLevelMask = GPU_REG_RD32(pGpu, NV_POOBHUB_PEREGRINE_MNOC_RECEIVEMBOX_PRIV_LEVEL_MASK(KOOB_MNOC_MBOX_PORT));
    NvU32 sourceEnMask = DRF_VAL(_POOBHUB_PEREGRINE, _MNOC_RECEIVEMBOX_PRIV_LEVEL_MASK, _SOURCE_ENABLE, privLevelMask);

    if (pKernelOob->registryOverride == NV_REG_STR_RM_KERNEL_OOB_LOGGING_DISABLE)
    {
        NV_PRINTF(LEVEL_NOTICE, "Kernel OOB logging is disabled by registry override.\n");
        return NV_FALSE;
    }

    // Not supported in CC mode for first release.
    if (gpuIsCCFeatureEnabled(pGpu))
    {
        NV_PRINTF(LEVEL_NOTICE, "Kernel OOB logging is disabled because it is incompatible with CC.\n");
        return NV_FALSE;
    }

    // OOBHUB MNOC PLM shows registers are accessible by Kernel-RM. 0x1 is PCIe source ID.
#define KOOB_PLM_CHECK(f, c, v)    FLD_TEST_DRF(_POOBHUB_PEREGRINE, _MNOC_RECEIVEMBOX_PRIV_LEVEL_MASK, f, c, v)
    if (!KOOB_PLM_CHECK(_READ_PROTECTION, _ALL_LEVELS_ENABLED, privLevelMask) ||
        !KOOB_PLM_CHECK(_WRITE_PROTECTION, _ALL_LEVELS_ENABLED, privLevelMask) ||
        !KOOB_PLM_CHECK(_READ_VIOLATION, _REPORT_ERROR, privLevelMask) ||
        !KOOB_PLM_CHECK(_WRITE_VIOLATION, _REPORT_ERROR, privLevelMask) ||
        !KOOB_PLM_CHECK(_SOURCE_READ_CONTROL, _BLOCKED, privLevelMask) ||
        !KOOB_PLM_CHECK(_SOURCE_WRITE_CONTROL, _BLOCKED, privLevelMask) ||
        !(sourceEnMask == 0x1))
    {
        NV_PRINTF(LEVEL_NOTICE, "Kernel OOB logging is disabled because OOBHUB FW does not support feature.\n");
        return NV_FALSE;
    }
#undef KOOB_PLM_CHECK

    return NV_TRUE;
}

/*!
 * Encode the Driver Event Payload for reporting XID and debug reports to the BMC
 *
 * @param[in]  pKernelOob             KernelOob object pointer
 * @param[in]  pSendBuffer            Destination buffer
 * @param[in]  nsmEventId             NSM Event ID
 * @param[in]  nsmEventPayloadSize    Size of the NSM Event Payload
 * @param[out] pNsmEventMsgSize       Size of entire message (packet header + payloads)
 *
 */
NV_STATUS
koobNsmEncodeEvent_GB100
(
    OBJGPU *pGpu,
    KernelOob *pKernelOob,
    NvU8 *pSendBuffer,
    NvU8 nsmEventId,
    NvU32 nsmEventPayloadSize,
    NvU32 *pNsmEventMsgSize
)
{
    struct nsm_encdec_context  ctx;
    struct nsm_event_info_v2   info;

    ctx.base = pSendBuffer;
    ctx.total_size = KOOB_MNOC_MBOX_MAX_PKT_SIZE;

    // Version 2 supports 4 B encoding of data_size_bytes
    info.version = 2;
    info.event_id = nsmEventId;
    info.event_class = NSM_API_EVENT_CLASS_GENERAL;
    info.event_state = 0;
    info.data_size_bytes = nsmEventPayloadSize;
    info.data = pKernelOob->nsmSendBuffer;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        nsm_api_encode_event_v2(&ctx, &info, 0xFF, 0));

    *pNsmEventMsgSize = info.data_size_bytes + sizeof(struct nsm_event_v2);

    return NV_OK;
}

static NvU32
_koobMnocMboxSend_HELPER
(
    OBJGPU *pGpu,
    KernelOob *pKernelOob,
    NvU8 *pSendBuffer,
    NvU32 msgSize
)
{
    RMTIMEOUT timeout;

    gpuSetTimeout(pGpu, gpuScaleTimeout(pGpu, OOBHUB_SEND_TIMEOUT_US),
                  &timeout, GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        gpuMnocMboxSend_HAL(pGpu, &pKernelOob->mboxAperture, KOOB_MNOC_MBOX_PORT,
                            &timeout, pSendBuffer, msgSize));

    return NV_OK;
}

static void
_koobMnocMboxSend_WORKITEM
(
    NvU32   gpuInstance,
    void    *pArgs
)
{
    OBJGPU  *pGpu = gpumgrGetGpu(gpuInstance);
    KernelOob *pKernelOob = GPU_GET_KERNEL_OOB(pGpu);
    KoobMnocMboxSendParams *pParams = pArgs;

    if (FULL_GPU_SANITY_CHECK(pGpu) && pGpu->getProperty(pGpu, PDB_PROP_GPU_STATE_INITIALIZED))
    {
        (void)_koobMnocMboxSend_HELPER(pGpu, pKernelOob,
                                       pParams->mnocPacketSendBuffer, pParams->msgSize);
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "GPU isn't full power! gpuInstance = 0x%x.\n", gpuInstance);
    }

    _koobFreeSendBuf(pKernelOob, NV_TRUE, pParams);
}

/*!
 * Send MNOC MBOX message to OOBHUB
 *
 * @param[in]  pKernelOob      KernelOob object pointer
 * @param[in]  pParams         Send buffer descriptor
 *
 */
NV_STATUS
koobMnocMboxSend_GB100
(
    OBJGPU *pGpu,
    KernelOob *pKernelOob,
    NvBool bToWorkItem,
    KoobMnocMboxSendParams *pParams
)
{
    NV_STATUS   status;

    if (bToWorkItem)
    {
        status = osQueueWorkItem(pGpu,
                                 _koobMnocMboxSend_WORKITEM,
                                 pParams,
                                 (OsQueueWorkItemFlags){
                                   .bLockSema = NV_TRUE,
                                   .apiLock = WORKITEM_FLAGS_API_LOCK_READ_WRITE,
                                   .bLockGpus = NV_TRUE,
                                   .bFullGpuSanity = NV_FALSE,
                                   .bDontFreeParams = NV_TRUE});
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,"Queuing workitem for sending message to OOBHUB failed with status :0x%x\n",
                      status);
        }
    }
    else
    {
        status = _koobMnocMboxSend_HELPER(pGpu, pKernelOob, pParams->mnocPacketSendBuffer, pParams->msgSize);
    }

    return status;
}

/*!
 * Fill the Driver Startup Event Message payload
 *
 * @param[in]       pGpu            GPU object pointer
 * @param[in]       pKernelOob      KernelOob object pointer
 * @param[in]       payloadBuffer   destination buffer
 * @param[in,out]   pNsmPayloadSize payload size
 *
 * @return  void
 */
static void
_koobCreateStartupPayload
(
    OBJGPU *pGpu,
    KernelOob *pKernelOob,
    NvU8 *payloadBuffer,
    NvU32 *pNsmPayloadSize
)
{
    StartupEventPayload *rec = (StartupEventPayload *)payloadBuffer;

    rec->queueDepth = 0x01; // for compatibility with PMU
    rec->basicCapabilities = DRF_DEF(_STARTUP_EVENT, _CAPS, _REQ_SUPPORTED,         _NO)  |
                             DRF_DEF(_STARTUP_EVENT, _CAPS, _ASYNC_EVENT_SUPPORTED, _YES);
    rec->reserved = 0;

    *pNsmPayloadSize = sizeof(StartupEventPayload);
}

/*!
 * Fill the Driver Message Event payload
 *
 * @param[in]       pGpu            GPU object pointer
 * @param[in]       pKernelOob      KernelOob object pointer
 * @param[in]       payloadBuffer   destination buffer
 * @param[in]       pPayload        payload descriptor
 * @param[in,out]   pNsmPayloadSize payload size
 *
 * @return  void
 */
static void
_koobCreateDriverMessagePayload
(
    OBJGPU *pGpu,
    KernelOob *pKernelOob,
    NvU8 *payloadBuffer,
    KoobDriverVerboseMessagePayload *pPayload,
    NvU32 *pNsmPayloadSize
)
{
    KernelGsp        *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
    KernelFalcon     *pKernelFlcn = staticCast(pKernelGsp, KernelFalcon);
    drvEventRecord    rec = {0};
    RmDebugHeader     rmDebugHeader = {0};
    CoreDumpRegs      coreDumpRegs = {0};
    NvU32             textSize = NV_MIN(pPayload->msgLen, NV_DRV_EVENT_TEXT_STRING_MAX);
    NvU64             tstamp = koobGetTimestampEpochNs(pGpu, pKernelOob);

    ct_assert((sizeof(rmDebugHeader) + sizeof(CoreDumpRegs) + sizeof(CrashCatExcerpt)) <= NV_DRV_EVENT_DATA_DUMP_MAX);
    ct_assert(sizeof(pKernelOob->nsmSendBuffer[0]) == sizeof(NvU8));

    if (textSize < pPayload->msgLen)
    {
        rec.flags = FLD_SET_DRF(_DRV_EVENT, _FLAGS, _TRUNCATED, _YES, rec.flags);
    }

    // Stage data for Driver Event Record
    rec.dbgDumpOffset = (NvU8)(sizeof(rec) + textSize);
    rec.reason = pPayload->xid;
    rec.seqNumber = ++pKernelOob->drvEventSeqNumber;
    rec.timestampLo = tstamp & ~(NvU32)0;
    rec.timestampHi = (tstamp >> 32) & ~(NvU32)0;

    // Stage data for RM Debug Header
    rmDebugHeader.payloadVersion = KOOB_DEBUG_PAYLOAD_VERSION;
    rmDebugHeader.xid = pPayload->xid;
    rmDebugHeader.debugDumpTimestamp = tstamp;
    rmDebugHeader.crashcatReportTimestamp = pKernelOob->bReportCached ? pKernelOob->reportCacheTimestamp : 0;
    rmDebugHeader.memSubsysErrorMask = (pPayload->xid == MEMORY_SUBSYSTEM_ERROR) ? pKernelOob->memSubsysErrorMask : 0;
    rmDebugHeader.poisonErrorStatus =
                (pPayload->xid == UNRECOVERABLE_ECC_ERROR_ESCAPE)? pKernelOob->poisonErrorStatus : 0;
    rmDebugHeader.bGspRmHeartbeatTimedOut = kgspHeartbeatIsGspRmHeartbeatTimedOut(pGpu, pKernelGsp);
    rmDebugHeader.bLibosHeartbeatTimedOut = kgspHeartbeatIsLibosHeartbeatTimedOut(pGpu, pKernelGsp);

    // Stage data for ICD dump
    kflcnDumpCoreRegs_HAL(pGpu, pKernelFlcn, &coreDumpRegs);
    kflcnDumpPeripheralRegs_HAL(pGpu, pKernelFlcn, &coreDumpRegs);
    kflcnDumpTracepc(pGpu, pKernelFlcn, &coreDumpRegs);

    // CrashCat Report should already by staged when it arrives, if it arrived in time.

#define KOOB_ADD_TO_PAYLOAD(src, srcSize, offset)                                               \
    {                                                                                           \
        portMemCopy(&payloadBuffer[offset], *pNsmPayloadSize - (offset), (src), (srcSize));     \
        offset += srcSize;                                                                      \
    }

    {
        NvU32 offset = 0;

        // Add Driver Event Record to payload
        KOOB_ADD_TO_PAYLOAD(&rec, sizeof(rec), offset);

        // Add XID message string to payload
        KOOB_ADD_TO_PAYLOAD(pPayload->pMsgString, textSize, offset);

        // Add RM Debug Header to payload
        KOOB_ADD_TO_PAYLOAD(&rmDebugHeader, sizeof(rmDebugHeader), offset);

        // Add ICD Dump to payload
        KOOB_ADD_TO_PAYLOAD(&coreDumpRegs, sizeof(CoreDumpRegs), offset);

        // Add Crashcat Report to payload
        if (pKernelOob->bReportCached)
        {
            CrashCatExcerpt *pExcerpt = &pKernelOob->reportCache;

            // Fixed-size data
            KOOB_ADD_TO_PAYLOAD(&pExcerpt->report, sizeof(pExcerpt->report), offset);
            KOOB_ADD_TO_PAYLOAD(&pExcerpt->riscv64CsrState, sizeof(pExcerpt->riscv64CsrState), offset);
            KOOB_ADD_TO_PAYLOAD(&pExcerpt->riscv64GprState, sizeof(pExcerpt->riscv64GprState), offset);

            // Truncated variable-sized arrays up to CRASHCAT_EXCERPT_MAX_SIZE length
            KOOB_ADD_TO_PAYLOAD(&pExcerpt->riscv64StackTrace_V1, pExcerpt->riscv64StackTraceSize, offset);
            KOOB_ADD_TO_PAYLOAD(&pExcerpt->riscv64PcTrace_V1, pExcerpt->riscv64PcTraceSize, offset);
            KOOB_ADD_TO_PAYLOAD(&pExcerpt->io32State_V1, pExcerpt->io32StateSize, offset);
        }

        *pNsmPayloadSize = offset;
    }
#undef KOOB_ADD_TO_PAYLOAD
}

/*!
 * Fill the CPER Event Message payload
 *
 * @param[in]       pGpu            GPU object pointer
 * @param[in]       pKernelOob      KernelOob object pointer
 * @param[in]       payloadBuffer   destination buffer
 * @param[in]       pPayload        payload descriptor
 * @param[in,out]   pNsmPayloadSize payload size
 *
 * @return  void
 */
static void
_koobCreateCperPayload
(
    OBJGPU *pGpu,
    KernelOob *pKernelOob,
    NvU8 *payloadBuffer,
    KoobCperPayload *pPayload,
    NvU32 *pNsmPayloadSize
)
{
    CperEventPayload *rec = (CperEventPayload *)payloadBuffer;

    rec->reserved = 0;
    portMemCopy(&rec->data, *pNsmPayloadSize, pPayload->cperBytes, pPayload->recordSize);
    *pNsmPayloadSize = sizeof(CperEventPayload) + pPayload->recordSize;
}

static KoobMnocMboxSendParams *
_koobAllocSendBuf
(
    KernelOob *pKernelOob,
    NvBool bToWorkItem
)
{
    KoobMnocMboxSendParams  *pBuf = NULL;

    if (bToWorkItem)
    {
        NvU64   bufIdx;

        if (pKernelOob->mnocBufferPool.pBufSpinLock == NULL)
        {
            // The driver must be unloading.
            NV_PRINTF(LEVEL_ERROR, "The spinlock has disappeared\n");
            return NULL;
        }

        portSyncSpinlockAcquire(pKernelOob->mnocBufferPool.pBufSpinLock);

        bufIdx = pKernelOob->mnocBufferPool.availableSendBufSlots;
        if (bufIdx != 0)
        {
            LOWESTBITIDX_64(bufIdx);
            pKernelOob->mnocBufferPool.availableSendBufSlots &= ~BIT64(bufIdx);
            pBuf = &pKernelOob->mnocBufferPool.mnocEventSendParams[bufIdx];
        }

        portSyncSpinlockRelease(pKernelOob->mnocBufferPool.pBufSpinLock);

        return pBuf;
    }
    else
    {
        return portMemAllocNonPaged(sizeof(KoobMnocMboxSendParams));
    }
}

static void
_koobFreeSendBuf
(
    KernelOob *pKernelOob,
    NvBool bToWorkItem,
    KoobMnocMboxSendParams *pBuf
)
{
    if (bToWorkItem)
    {
        NvU32 offset;
        NvU8 bufIdx;
        NvU32 rem;

        NV_ASSERT_OR_RETURN_VOID(pBuf >= pKernelOob->mnocBufferPool.mnocEventSendParams);
        offset = (NvU8 *)pBuf - (NvU8 *)pKernelOob->mnocBufferPool.mnocEventSendParams;
        rem = offset % sizeof(KoobMnocMboxSendParams);  // need to use an intermediate var, as
                                                        // NV_ASSERT() can't handle this argument
        NV_ASSERT_OR_RETURN_VOID(rem == 0);
        bufIdx = pBuf - pKernelOob->mnocBufferPool.mnocEventSendParams;
        NV_ASSERT_OR_RETURN_VOID(bufIdx < KOOB_MNOC_NUM_BUFS);

        if (pKernelOob->mnocBufferPool.pBufSpinLock == NULL)
        {
            // The driver must be unloading.
            NV_PRINTF(LEVEL_ERROR, "The spinlock has disappeared\n");
            return;
        }

        portSyncSpinlockAcquire(pKernelOob->mnocBufferPool.pBufSpinLock);

        pKernelOob->mnocBufferPool.availableSendBufSlots |= BIT64(bufIdx);

        portSyncSpinlockRelease(pKernelOob->mnocBufferPool.pBufSpinLock);
    }
    else
    {
        portMemFree(pBuf);
    }
}

/*!
 * Send an Event Message to OOBHUB
 *
 * @param[in]   pGpu            GPU object pointer
 * @param[in]   pKernelOob      KernelOob object pointer
 * @param[in]   eventId         event ID
 * @param[in]   pPayload        payload descriptor
 *
 * @return  void
 */
void
koobSendEvent_GB100
(
    OBJGPU *pGpu,
    KernelOob *pKernelOob,
    KOOB_EVENT_TYPE eventId,
    KoobEventPayload *pPayload
)
{
    NV_STATUS               status;
    const NvU32             NSM_PAYLOAD_SIZE_MAX = KOOB_MNOC_MBOX_MAX_PKT_SIZE - sizeof(struct nsm_event_v2);
    NvU8                    nsmEventId;
    NvU32                   nsmPayloadSize = NSM_PAYLOAD_SIZE_MAX;
    NvU32                   msgSize = 0;
    NvBool                  bToWorkItem = osIsRaisedIRQL();
    KoobMnocMboxSendParams  *pMnocMboxSendParams;

    if (!koobIsSupported(pGpu, pKernelOob))
    {
        NV_PRINTF(LEVEL_INFO, "Skipping Kernel OOB message because it is not supported in this configuration.\n");
        return;
    }

    pMnocMboxSendParams = _koobAllocSendBuf(pKernelOob, bToWorkItem);
    if (pMnocMboxSendParams == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "No buffer space for kernel OOB message.\n");
        return;
    }

    // Clear packet staging buffers
    portMemSet(pMnocMboxSendParams, 0, sizeof(KoobMnocMboxSendParams));
    portMemSet(pKernelOob->nsmSendBuffer, 0, sizeof(pKernelOob->nsmSendBuffer));

    switch (eventId)
    {
        case KOOB_EVENT_DRIVER_STARTUP:
        {
            _koobCreateStartupPayload(pGpu, pKernelOob, pKernelOob->nsmSendBuffer, &nsmPayloadSize);
            nsmEventId = NSM_TYPE_INTERNAL_EVENT_STARTUP_V2;
            break;
        }
        case KOOB_EVENT_DRIVER_SHUTDOWN:
        {
            nsmPayloadSize = 0;
            nsmEventId = NSM_TYPE_INTERNAL_EVENT_SHUTDOWN_V2;
            break;
        }
        case KOOB_EVENT_DRIVER_VERBOSE_MESSAGE:
        {
            ct_assert((sizeof(drvEventRecord) + NV_DRV_EVENT_TEXT_STRING_MAX + NV_DRV_EVENT_DATA_DUMP_MAX) <=
                      sizeof(pKernelOob->nsmSendBuffer));

            _koobCreateDriverMessagePayload(pGpu, pKernelOob, pKernelOob->nsmSendBuffer,
                                            &pPayload->driverVerboseMessagePayload, &nsmPayloadSize);
            nsmEventId = NSM_TYPE_INTERNAL_DRIVER_VERBOSE_MESSAGE;

            break;
        }
        case KOOB_EVENT_CPER:
        {
            const NvU32 PAYLOAD_SIZE_MAX = NSM_PAYLOAD_SIZE_MAX - sizeof(CperEventPayload);

            if (PAYLOAD_SIZE_MAX < pPayload->cperPayload.recordSize)
            {
                NV_PRINTF(LEVEL_ERROR, "Could not send NSM message: CPER record too long (0x%08x)\n",
                          pPayload->cperPayload.recordSize);
                status = NV_ERR_GENERIC;
                goto koobSendEvent_GB100_exit;
            }

            _koobCreateCperPayload(pGpu, pKernelOob, pKernelOob->nsmSendBuffer, &pPayload->cperPayload,
                                            &nsmPayloadSize);
            nsmEventId = NSM_TYPE_INTERNAL_CPER;

            break;
        }
        default:
        {
            NV_PRINTF(LEVEL_INFO, "Sending event to OOBHUB failed. Illegal event ID: 0x%02x\n", eventId);
            status = NV_ERR_GENERIC;
            goto koobSendEvent_GB100_exit;
        }
    }

    // Encode the payload in NSM Message
    status = koobNsmEncodeEvent_HAL(pGpu, pKernelOob, pMnocMboxSendParams->mnocPacketSendBuffer,
                                    nsmEventId,
                                    nsmPayloadSize,
                                    &msgSize);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Could not encode NSM event : 0x%08x\n", status);
        goto koobSendEvent_GB100_exit;
    }

    // Send NSM encoded packet to OOBHUB
    pMnocMboxSendParams->msgSize = msgSize;
    status = koobMnocMboxSend_HAL(pGpu, pKernelOob, bToWorkItem, pMnocMboxSendParams);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Could not send NSM message : 0x%08x\n", status);
    }

koobSendEvent_GB100_exit:
    // The buffer must be freed
    // - if we are sending in-line (no work item),
    // - if scheduling of a work item failed.
    // If scheduling succeeded, the work item will free it upon completing the transmission.
    if (!(bToWorkItem && (status == NV_OK)))
    {
        _koobFreeSendBuf(pKernelOob, bToWorkItem, pMnocMboxSendParams);
    }

    return;
}
