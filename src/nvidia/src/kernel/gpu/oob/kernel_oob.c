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
#include "os/os.h"
#include "nvrm_registry.h"
#include "gpu/gpu.h"
#include "gpu/gpu_access.h"
#include "kernel/gpu/oob/kernel_oob.h"
#include "crashcat/crashcat_report.h"

static void
_koobReadRegkeyOverrides(OBJGPU *pGpu, KernelOob *pKernelOob)
{
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_KERNEL_OOB_LOGGING, &pKernelOob->registryOverride) != NV_OK)
    {
        pKernelOob->registryOverride = NV_REG_STR_RM_KERNEL_OOB_LOGGING_DEFAULT;
    }
}

NV_STATUS
koobConstructEngine_IMPL
(
    OBJGPU *pGpu,
    KernelOob *pKernelOob,
    ENGDESCRIPTOR engDesc
)
{
    _koobReadRegkeyOverrides(pGpu, pKernelOob);

    pKernelOob->drvEventSeqNumber = 0;
    pKernelOob->memSubsysErrorMask = 0;
    pKernelOob->poisonErrorStatus = 0;
    portMemSet(pKernelOob->mnocBufferPool.mnocEventSendParams, 0,
               sizeof(pKernelOob->mnocBufferPool.mnocEventSendParams));
    portMemSet(pKernelOob->nsmSendBuffer, 0, sizeof(pKernelOob->nsmSendBuffer));

    pKernelOob->bReportCached = NV_FALSE;
    portMemSet(&pKernelOob->reportCache, 0, sizeof(pKernelOob->reportCache));
    pKernelOob->reportCacheTimestamp = 0;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, koobInitAperture_HAL(pGpu, pKernelOob));

    pKernelOob->bSupported = koobCheckSupported_HAL(pGpu, pKernelOob);

    return NV_OK;
}

void
koobDriverStartupNotify_IMPL
(
    OBJGPU *pGpu,
    KernelOob *pKernelOob
)
{
    koobSendEvent_HAL(pGpu, pKernelOob, KOOB_EVENT_DRIVER_STARTUP, NULL);
}

void
koobDriverShutdownNotify_IMPL
(
    OBJGPU *pGpu,
    KernelOob *pKernelOob
)
{
    koobSendEvent_HAL(pGpu, pKernelOob, KOOB_EVENT_DRIVER_SHUTDOWN, NULL);
}

void
koobDestruct_IMPL
(
    KernelOob *pKernelOob
)
{
    return;
}

NvBool
koobIsSupported_IMPL
(
    OBJGPU *pGpu,
    KernelOob *pKernelOob
)
{
    return pKernelOob->bSupported;
}

/*!
 * @brief Get unix epoch (since 01/01/1970) time in ns
 *
 * Detect if the OS timestamp is not in ns and convert it to ns.
 *
 * @returns timestamp in ns
 */
#define KOOB_NS_IN_S     1000000000ULL   // nanoseconds in a second
NvU64
koobGetTimestampEpochNs_IMPL(OBJGPU *pGpu, KernelOob *pKernelOob)
{
    NvU64   timestamp = osGetTimestamp();
    NvU64   freq = osGetTimestampFreq();
    NvU32   factor;

    if (freq == KOOB_NS_IN_S)
    {
        return timestamp;
    }
    else if (freq < KOOB_NS_IN_S)
    {
        factor = (NvU32)(KOOB_NS_IN_S / freq);
        timestamp *= factor;
    }
    else
    {
        factor = (NvU32)(freq / KOOB_NS_IN_S);
        timestamp /= factor;
    }

    return timestamp;
}
#undef KOOB_NS_IN_S

/*!
 * HW-agnostic layer for sending Driver Event Message (Xid & Debug Report) to OOBHUB.
 * HW-independent wrapper around the HAL methods.
 *
 * @param[in]   pGpu            GPU object pointer
 * @param[in]   pKernelOob      KernelOob object pointer
 * @param[in]   xid             XID number
 * @param[in]   msgLen          XID message size
 * @param[in]   pMsgString      XID message string
 *
 * @return  void
 */
void
koobLogMessageCommon_IMPL
(
    OBJGPU *pGpu,
    KernelOob *pKernelOob,
    NvU32 xid,
    NvU32 msgLen,
    NvU8 *pMsgString
)
{
    KoobEventPayload payload = {0};

    //
    // Skip the event logging in case we might be in bad state already.
    //
    if (API_GPU_IN_RESET_SANITY_CHECK(pGpu) ||
        pGpu->getProperty(pGpu, PDB_PROP_GPU_PREPARING_FULLCHIP_RESET) ||
        pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_LOST) ||
        (xid == ROBUST_CHANNEL_GPU_HAS_FALLEN_OFF_THE_BUS))
    {
        return;
    }

    payload.driverVerboseMessagePayload.xid = xid;
    payload.driverVerboseMessagePayload.msgLen = msgLen;
    payload.driverVerboseMessagePayload.pMsgString = pMsgString;

    koobSendEvent_HAL(pGpu, pKernelOob, KOOB_EVENT_DRIVER_VERBOSE_MESSAGE, &payload);

    return;
}

/*!
 * HW-agnostic layer for sending CPER Event Message to OOBHUB.
 * HW-independent wrapper around the HAL methods.
 *
 * @param[in]   pGpu            GPU object pointer
 * @param[in]   pKernelOob      KernelOob object pointer
 * @param[in]   pCperBytes      CPER blob
 * @param[in]   recordSize      CPER blob size
 *
 * @return  void
 */
void
koobCperSendCommon_IMPL
(
    OBJGPU *pGpu,
    KernelOob *pKernelOob,
    NvU8 *pCperBytes,
    NvU32 recordSize
)
{
    KoobEventPayload payload = {0};
    //
    // Skip the event logging in case we might be in bad state already.
    //
    if (API_GPU_IN_RESET_SANITY_CHECK(pGpu) ||
        pGpu->getProperty(pGpu, PDB_PROP_GPU_PREPARING_FULLCHIP_RESET) ||
        pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_LOST))
    {
        return;
    }

    payload.cperPayload.recordSize = recordSize;
    payload.cperPayload.cperBytes = pCperBytes;

    koobSendEvent_HAL(pGpu, pKernelOob, KOOB_EVENT_CPER, &payload);

    return;
}

/*!
 * Cache the crashcat report for OOB logging
 *
 * @param[in]   pGpu            GPU object pointer
 * @param[in]   pKernelOob      KernelOob object pointer
 * @param[in]   pReport         CrashCat report pointer
 *
 * @return  void
 */
void
koobCacheCrashcatReport_IMPL
(
    OBJGPU *pGpu,
    KernelOob *pKernelOob,
    CrashCatReport *pReport
)
{
    pKernelOob->bReportCached = NV_TRUE;
    pKernelOob->reportCacheTimestamp = koobGetTimestampEpochNs(pGpu, pKernelOob);
    crashcatReportExcerpt_HAL(pReport, &pKernelOob->reportCache);
}
