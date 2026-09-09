/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 *
 * @file    kern_fsp_gb100.c
 * @brief   Provides the implementation for BLACKWELL chip specific FSP HAL
 *          interfaces.
 */
#include "kernel/gpu/gpu.h"
#include "gpu/fsp/kern_fsp.h"
#include "gpu/fsp/kern_fsp_retval.h"
#include "events/gpu/fsp/fsp_events.h"
#include "nvoc/event_bus.h"
#include "nvport/time.h"
#include "gpu/gsp/kernel_gsp.h"
#include "fsp/fsp_caps_query_rpc.h"
#include "fsp/fsp_clock_boost_rpc.h"
#include "mctp_format.h"
#include "nvdm_format.h"

#include "published/blackwell/gb100/dev_therm.h"
#include "published/blackwell/gb100/dev_therm_addendum.h"
#include "published/blackwell/gb100/dev_fsp_pri.h"
#include "published/blackwell/gb100/dev_fsp_addendum.h"
#include "published/blackwell/gb100/dev_gsp.h"
#include "published/blackwell/gb100/dev_oob_pri.h"
#include "published/blackwell/gb100/dev_bus_zb.h"
#include "published/blackwell/gb100/dev_bus_zb_addendum.h"
#include "published/blackwell/gb100/dev_top_zb.h"
#include "published/blackwell/gb100/hwproject.h"

#include "cper/gpu_cper.h"
#include "os/os.h"
#include "nvrm_registry.h"
#include "nverror.h"

#define NV_DEFINED_DOWNLOAD_LOG_COMMAND 0x06

#define KFSP_GB100_GPU_INIT_ERROR_SUBTYPE_FSP_BOOT_TIMEOUT 1
#define KFSP_GB100_GPU_INIT_ERROR_SUBTYPE_FSP_FUSE_ERROR   2

#define KFSP_GB100_GPU_INIT_ERROR_FMT \
    "Error status 0x%x while polling for FSP boot complete, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x"

#define KFSP_GB100_GPU_INIT_FUSE_ERROR_FMT \
    "FSP fuse error check has failed. Status = 0x%x."

#define CMS2_LOG_START   0x50U
#define CMS2_LOG_END     0x7FU
#define CMS2_LOG_DWORDS  (CMS2_LOG_END - CMS2_LOG_START + 1)
#define CMS2_LOG_BYTES   (CMS2_LOG_DWORDS * sizeof(NvU32))
#define DMEM_LOG_MAX_BYTES (4096)
#define DMEM_RESPONSE_TIMEOUT_US  (5000)

#pragma pack(push)
#pragma pack(1)

typedef struct
{
    NvU8  messageType    : 7;
    NvU8  ic             : 1;
    NvU32 iana;
    NvU8  instanceId     : 5;
    NvU8  rsvd           : 1;
    NvU8  d              : 1;
    NvU8  rq             : 1;
    NvU8  vendorMessageType;
    NvU8  commandCode;
    NvU8  messageVersion;
} MctpVdmIanaRequest;

typedef struct
{
    MctpVdmIanaRequest request;
    NvU8  completionCode;
} MctpVdmIanaResponse;

typedef struct
{
    NvU32 mctpHeader;
    MctpVdmIanaRequest request;
    NvU8 sessionId;
} MctpDownloadLogV1;

typedef struct
{
    NvU8 sessionId;
    NvU8 length;
    NvU8 data[52];
} MctpVdmIanaDownloadLogResponseV1;

#pragma pack(pop)

static void _kfspPrintCms2Log_GB100(OBJGPU *pGpu, KernelFsp *pKernelFsp, NvU8 *cms2Log);
static void _kfspPrintDmemLog_GB100(OBJGPU *pGpu, KernelFsp *pKernelFsp, NvU8 *pDmemLog, NvU32 logSize);
static NvBool _kfspWaitBootCond_GB100(OBJGPU *pGpu, void *pArg);
static void _kfspGatherCms2Log_GB100(OBJGPU *pGpu, NvU32  *cms2Log);
static NV_STATUS _kfspGatherDmemLog_GB100(OBJGPU *pGpu, KernelFsp *pKernelFsp, NvU8 *pDmemLog, const NvU32 maxLogSize, NvU32 *pLogSize);

NV_STATUS
kfspWaitForSecureBoot_GB100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    NV_STATUS status  = NV_OK;
    RMTIMEOUT timeout;
    NvU32 timeoutUs;

    //
    // Polling for FSP boot complete
    // FBFalcon training during devinit alone takes 2 seconds, up to 3 on HBM3,
    // but the default threadstate timeout on windows is 1800 ms. Increase to 4 seconds
    // for this wait to match MODS GetGFWBootTimeoutMs.
    // For flags, we must not use the GPU TMR since it is inaccessible.
    //
    timeoutUs = NV_MAX(gpuScaleTimeout(pGpu, 4000000), pGpu->timeoutData.defaultus);
    gpuSetTimeout(pGpu, timeoutUs, &timeout, GPU_TIMEOUT_FLAGS_OSTIMER);

    NvU64 timeoutNs = (NvU64)timeoutUs * 1000ULL;
    NvU64 waitStartNs = portTimeGetUptimeNanosecondsHighPrecision();
    status = gpuTimeoutCondWait(pGpu, _kfspWaitBootCond_GB100, NULL, &timeout);

    if (status != NV_OK)
    {
        NvU64 waitEndNs = portTimeGetUptimeNanosecondsHighPrecision();
        NvU32 fspBootComplete = GPU_REG_RD32(pGpu, NV_THERM_I2CS_SCRATCH_FSP_BOOT_COMPLETE);
        NvU32 s0 = GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(0));
        NvU32 s1 = GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(1));
        NvU32 s2 = GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(2));
        NvU32 s3 = GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(3));
        NV_ASSERT_OK(gpuMarkDeviceForReset(pGpu));
        eventEmit(FspBootTimeout, pKernelFsp, timeoutNs, waitEndNs - waitStartNs,
                  status, fspBootComplete, s0, s1, s2, s3);

        kfspDumpDebugState_HAL(pGpu, pKernelFsp);
    }

    if (GPU_FLD_TEST_DRF_DEF(pGpu, _PFSP, _FUSE_ERROR_CHECK, _STATUS, _SUCCESS))
    {
        NV_PRINTF(LEVEL_INFO, "FSP fuse error check has passed. Status = 0x%08x.\n",
                    GPU_REG_RD32(pGpu, NV_PFSP_FUSE_ERROR_CHECK));
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR,
                  "****************************************** FSP Fuse Check Failure ************************************************\n");
        {
            NvU32 fuseStatus = GPU_REG_RD32(pGpu, NV_PFSP_FUSE_ERROR_CHECK);

            eventEmit(FspFuseError, pKernelFsp, fuseStatus);
        }
        NV_PRINTF(LEVEL_ERROR,
                    "** FSP fuse error check has failed. Status = 0x%x.                                                               **\n",
                    GPU_REG_RD32(pGpu, NV_PFSP_FUSE_ERROR_CHECK));
        NV_PRINTF(LEVEL_ERROR,
                  "******************************************************************************************************************\n");
        if (pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_FSP_FUSE_ERROR_CHECK_ENABLED))
        {
            status = NV_ERR_SECURE_BOOT_FAILED;
        }
    }

    return status;
}

void
kfspCheckForClockBoostCapability_GB100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    NV_STATUS status = NV_OK;
    FSP_CAPS_QUERY_RPC_PAYLOAD_PARAMS inputPayload = { 0 };
    inputPayload.subMessageId = NVDM_TYPE_CLOCK_BOOST;
    pKernelFsp->bClockBoostSupported = NV_FALSE;

    status = kfspSendAndReadMessage(pGpu, pKernelFsp, (NvU8*) &inputPayload, sizeof(inputPayload),
                                    NVDM_TYPE_CAPS_QUERY, NULL, 0);

    if (status == NV_OK)
    {
        NV_PRINTF(LEVEL_NOTICE, "FSP has clock boost capability\n");
        pKernelFsp->bClockBoostSupported = NV_TRUE;
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "FSP doesn't have clock boost capability\n");
    }
}

/*!
 * @brief Dump debug registers for FSP
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] pKernelFsp KernelFsp pointer
 *
 * @return NV_OK, or error if failed
 */
void
kfspDumpDebugState_GB100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    const NvU32  logBufferSize = DMEM_LOG_MAX_BYTES;
    NvU32        i;
    NvU32        logSize = 0;
    NvU8        *pLogBuffer;

    const NvU32 fspUcodeVersion = GPU_REG_RD_DRF(pGpu, _GFW, _FSP_UCODE_VERSION, _FULL);
    //
    // Older microcodes did not have the version populated in scratch.
    // They will report a version of 0.
    //
    if (fspUcodeVersion > 0)
    {
        NV_PRINTF(LEVEL_ERROR, "FSP microcode v%u.%u\n",
                  DRF_VAL(_GFW, _FSP_UCODE_VERSION, _MAJOR, fspUcodeVersion),
                  DRF_VAL(_GFW, _FSP_UCODE_VERSION, _MINOR, fspUcodeVersion));
    }

    NV_PRINTF(LEVEL_ERROR, "GPU %04x:%02x:%02x\n",
              gpuGetDomain(pGpu), gpuGetBus(pGpu), gpuGetDevice(pGpu));

    NV_PRINTF(LEVEL_ERROR, "NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(0) = 0x%x\n",
              GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(0)));
    NV_PRINTF(LEVEL_ERROR, "NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(1) = 0x%x\n",
              GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(1)));
    NV_PRINTF(LEVEL_ERROR, "NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(2) = 0x%x\n",
              GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(2)));
    NV_PRINTF(LEVEL_ERROR, "NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(3) = 0x%x\n",
              GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(3)));

    NV_PRINTF(LEVEL_ERROR, "NV_PGSP_FALCON_MAILBOX0 = 0x%x\n",
              GPU_REG_RD32(pGpu, NV_PGSP_FALCON_MAILBOX0));
    NV_PRINTF(LEVEL_ERROR, "NV_PGSP_FALCON_MAILBOX1 = 0x%x\n",
              GPU_REG_RD32(pGpu, NV_PGSP_FALCON_MAILBOX1));
    NV_PRINTF(LEVEL_ERROR, "NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR = 0x%x\n",
              GPU_REG_RD32(pGpu, NV_PBUS0_PRI_BASE + NV_PBUS_ZB_SW_SCRATCH_GSP_FMC_ERROR));
    for(i = 0; i < NV_PGSP_MAILBOX__SIZE_1; i++)
    {
        NV_PRINTF(LEVEL_ERROR, "NV_PGSP_MAILBOX(%d) = 0x%x\n",
                  i, GPU_REG_RD32(pGpu, NV_PGSP_MAILBOX(i)));
    }
    pLogBuffer = portMemAllocNonPaged(logBufferSize);

    if (pLogBuffer == NULL)
        return;

    _kfspGatherCms2Log_GB100(pGpu, (NvU32*) pLogBuffer);
    _kfspPrintCms2Log_GB100(pGpu, pKernelFsp, pLogBuffer);

    if (_kfspGatherDmemLog_GB100(pGpu, pKernelFsp, pLogBuffer, logBufferSize, &logSize) == NV_OK)
    {
        _kfspPrintDmemLog_GB100(pGpu, pKernelFsp, pLogBuffer, logSize);
    }

    portMemFree(pLogBuffer);
}

NV_STATUS
kfspSendClockBoostRpc_GB100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU8      cmd
)
{
    FSP_CLOCK_BOOST_RPC_PAYLOAD_PARAMS inputPayload = { 0 };
    inputPayload.subMessageId = cmd;

    return kfspSendAndReadMessage(pGpu, pKernelFsp, (NvU8*) &inputPayload, sizeof(inputPayload),
                                  NVDM_TYPE_CLOCK_BOOST, NULL, 0);
}

static void
_kfspPrintCms2Log_GB100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU8      *cms2Log
)
{
    NV_PRINTF(LEVEL_ERROR, "CMS2 Log:\n");
    nvDbgDumpBufferBytes(cms2Log, CMS2_LOG_BYTES);
}

static void
_kfspPrintDmemLog_GB100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU8      *pDmemLog,
    NvU32      logSize
)
{
    NV_PRINTF(LEVEL_ERROR, "DMEM Log:\n");
    nvDbgDumpBufferBytes(pDmemLog, logSize);
}

static NvBool
_kfspWaitBootCond_GB100
(
    OBJGPU *pGpu,
    void   *pArg
)
{
    //
    // In GB100, Bootfsm triggers FSP execution out of chip reset.
    // FSP writes 0xFF value in NV_THERM_I2CS_SCRATCH register after completion of boot
    //
    return GPU_FLD_TEST_DRF_DEF(pGpu, _THERM_I2CS_SCRATCH, _FSP_BOOT_COMPLETE, _STATUS, _SUCCESS);
}

static void
_kfspGatherCms2Log_GB100
(
    OBJGPU *pGpu,
    NvU32  *cms2Log
)
{
    NvU32 index;

    for (index = 0; index < CMS2_LOG_DWORDS; index++)
    {
        GPU_REG_WR32(pGpu, NV_POOBHUB_RCV_INDIRECT_CMS2_MEM_RD_ADDR, CMS2_LOG_START + index);
        cms2Log[index] = GPU_REG_RD32(pGpu, NV_POOBHUB_RCV_INDIRECT_CMS2_MEM_RD_DATA);
    }
}

static NV_STATUS
_kfspGatherDmemLog_GB100
(
    OBJGPU      *pGpu,
    KernelFsp   *pKernelFsp,
    NvU8        *pDmemLog,
    const NvU32  maxLogSize,
    NvU32       *pLogSize
)
{
    MctpDownloadLogV1 logRequest = {0};
    NvU32 packetNumber = 0;
    NvU32 responseSize = 0;
    NvU32 packetSize = 0;
    NvU8  recvBuffer[68];
    // Response starts after the NVDM and MCTP VDM IANA headers
    MctpVdmIanaDownloadLogResponseV1 *pResponse = (MctpVdmIanaDownloadLogResponseV1*)(recvBuffer + sizeof(NvU32) + sizeof(MctpVdmIanaResponse));
    NvU32 minPacketSize = sizeof(NvU32) + sizeof(MctpVdmIanaResponse) +
                          sizeof(MctpVdmIanaDownloadLogResponseV1) - sizeof(pResponse->data);

    logRequest.mctpHeader = REF_NUM(MCTP_HEADER_SOM,  1) |
                            REF_NUM(MCTP_HEADER_EOM,  1) |
                            REF_NUM(MCTP_HEADER_SEID, 0) |
                            REF_NUM(MCTP_HEADER_SEQ,  0) |
                            REF_NUM(MCTP_HEADER_TAG,  1);

    logRequest.request.messageType       = MCTP_MSG_HEADER_TYPE_VENDOR_IANA;
    logRequest.request.ic                = 0;
    logRequest.request.iana              = MCTP_MSG_HEADER_VENDOR_ID_IANA_NV;
    logRequest.request.instanceId        = 0;
    logRequest.request.d                 = 0;
    logRequest.request.rq                = 1;
    logRequest.request.vendorMessageType = 1;
    logRequest.request.commandCode       = NV_DEFINED_DOWNLOAD_LOG_COMMAND;
    logRequest.request.messageVersion    = 1;
    // 0xFF requests a new session
    logRequest.sessionId                 = 0xFF;

    //
    // The download log command works by returning up to 52 bytes at a time
    // while FSP keeps tracks of the current location in the log based on
    // the session ID.
    //
    while(responseSize + sizeof(pResponse->data) < maxLogSize)
    {
        NV_ASSERT_OK_OR_RETURN(kfspSendPacket_HAL(pGpu, pKernelFsp, (NvU8*)&logRequest, sizeof(logRequest)));
        //
        // Older FSP versions do not support DMEM logging and will ignore
        // the request causing a timeout here.
        //
        if (kfspPollForResponse(pGpu, pKernelFsp, DMEM_RESPONSE_TIMEOUT_US) != NV_OK)
            return NV_ERR_NOT_SUPPORTED;

        NV_ASSERT_OK_OR_RETURN(kfspReadPacket_HAL(pGpu, pKernelFsp, recvBuffer, sizeof(recvBuffer), &packetSize));

        NV_ASSERT_OR_RETURN(packetSize >= minPacketSize, NV_ERR_INVALID_STATE);
        NV_ASSERT_OR_RETURN(pResponse->length <= sizeof(pResponse->data), NV_ERR_INVALID_STATE);

        // A response size of 0 indicates the end of the logs
        if (pResponse->length == 0)
            break;

        if (packetNumber == 0)
        {
            // The first response holds the sessionId to be used for future requests
            logRequest.sessionId = pResponse->sessionId;
        }

        portMemCopy(pDmemLog + responseSize, pResponse->length, pResponse->data, pResponse->length);
        responseSize += pResponse->length;

        packetNumber++;
    }

    *pLogSize = responseSize;

    return NV_OK;
}

void
kfspFrtsSysmemLocationClear_GB100
(
    OBJGPU *pGpu,
    KernelFsp *pKernelFsp
)
{
    const DEVICE_INFO_ENTRY *pEntry;
    NV_STATUS status = gpuGetOneDeviceEntry(pGpu,
                                            NV_PTOP_ZB_DEVICE_INFO_DEV_TYPE_ENUM_PBUS,
                                            DEVICE_INFO_DIELET_INSTANCE_ANY,
                                            0,
                                            DEVICE_INFO_DIE_LOCAL_INSTANCE_ID_ANY,
                                            &pEntry);

    NV_ASSERT_OR_RETURN_VOID(status == NV_OK);

    GPU_REG_WR32(
        pGpu,
        pEntry->devicePriBase + NV_PBUS_ZB_SW_FRTS_INSECURE_CONFIG,
        REF_DEF(NV_PBUS_ZB_SW_FRTS_INSECURE_CONFIG_SIZE_4K, _INVALID));
    GPU_REG_WR32(
        pGpu, pEntry->devicePriBase + NV_PBUS_ZB_SW_FRTS_INSECURE_ADDR_HI32, 0U);
    GPU_REG_WR32(
        pGpu, pEntry->devicePriBase + NV_PBUS_ZB_SW_FRTS_INSECURE_ADDR_LO32, 0U);
}

NV_STATUS
kfspFrtsSysmemLocationProgram_GB100
(
    OBJGPU *pGpu,
    KernelFsp *pKernelFsp
)
{
    NV_ASSERT_OR_RETURN(pKernelFsp->pSysmemFrtsMemdesc != NULL, NV_ERR_INVALID_STATE);
    RmPhysAddr frtsSysmemAddr = memdescGetPhysAddr(pKernelFsp->pSysmemFrtsMemdesc, AT_GPU, 0U);

    GPU_REG_WR32(
        pGpu, NV_PBUS0_PRI_BASE + NV_PBUS_ZB_SW_FRTS_INSECURE_ADDR_LO32, NvU64_LO32(frtsSysmemAddr));
    GPU_REG_WR32(
        pGpu, NV_PBUS0_PRI_BASE + NV_PBUS_ZB_SW_FRTS_INSECURE_ADDR_HI32, NvU64_HI32(frtsSysmemAddr));
    GPU_REG_WR32(
        pGpu,
        NV_PBUS0_PRI_BASE + NV_PBUS_ZB_SW_FRTS_INSECURE_CONFIG,
        FLD_SET_DRF(
            _PBUS_ZB, _SW_FRTS_INSECURE_CONFIG, _MEDIA_TYPE, _SYSMEM,
        REF_NUM(
            NV_PBUS_ZB_SW_FRTS_INSECURE_CONFIG_SIZE_4K,
            (memdescGetSize(pKernelFsp->pSysmemFrtsMemdesc) >>
                NV_PBUS_ZB_SW_FRTS_INSECURE_CONFIG_SIZE_4K_SHIFT))));
    return NV_OK;
}
