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

/*!
 *
 * @file    kern_fsp_gr100.c
 * @brief   Provides the implementation for RUBIN chip specific FSP HAL
 *          interfaces.
 */
#include "kernel/gpu/gpu.h"
#include "gpu/gpu_access.h"
#include "gpu/fsp/kern_fsp.h"

#include "published/rubin/gr100/dev_fsp_pri.h"
#include "published/rubin/gr100/dev_mnoc_pri_zb.h"

// Shared boot-completion and debug register definitions.
#include "published/rubin/gr100/dev_therm.h"
#include "published/rubin/gr100/dev_therm_addendum.h"

#include "cper/gpu_cper.h"
#include "events/gpu/fsp/fsp_events.h"
#include "nvoc/event_bus.h"
#include "nvport/time.h"
#include "nverror.h"

// Keep NV_ERROR_LOG and CPER legacy-Xid text identical.
#define KFSP_GR100_GPU_INIT_ERROR_FMT \
    "Error status 0x%x while polling for FSP boot complete, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x"

#define KERNEL_FSP_MBOX_PORT                (2)
#define KERNEL_FSP_MBOX_MAX_PACKET_BYTES    (1024)

static NvBool
_kfspWaitBootCond_GR100
(
    OBJGPU *pGpu,
    void   *pArg
)
{
    //
    // In GR100, Bootfsm triggers FSP execution out of chip reset.
    // FSP writes 0xFF value in NV_THERM_I2CS_SCRATCH register after completion of boot
    //
    return GPU_FLD_TEST_DRF_DEF(pGpu, _THERM_I2CS_SCRATCH, _FSP_BOOT_COMPLETE, _STATUS, _SUCCESS);
}

NV_STATUS
kfspWaitForSecureBoot_GR100
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
    // FBFalcon training during devinit can take up to 30 seconds on Rubin
    // compute GPUs.
    // For flags, we must not use the GPU TMR since it is inaccessible.
    //
    timeoutUs = NV_MAX(gpuScaleTimeout(pGpu, 30000000), pGpu->timeoutData.defaultus);

    gpuSetTimeout(pGpu, timeoutUs, &timeout,
                  GPU_TIMEOUT_FLAGS_OSTIMER | GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE);

    NvU64 timeoutNs = (NvU64)timeoutUs * 1000ULL;
    NvU64 waitStartNs = portTimeGetUptimeNanosecondsHighPrecision();
    status = gpuTimeoutCondWait(pGpu, _kfspWaitBootCond_GR100, NULL, &timeout);

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

    return status;
}

NV_STATUS
kfspConstructHal_GR100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    return ioaprtInit(&pKernelFsp->mboxAperture, pGpu->pIOApertures[DEVICE_INDEX_GPU],
                           NV_PFSP_MNOC_RX_FIFO_DATA(0), 0x200);
}

NV_STATUS
kfspSendPacket_GR100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU8      *pPacket,
    NvU32      packetSize
)
{
    RMTIMEOUT timeout;

    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);

    return gpuMnocMboxSend_HAL(pGpu, &pKernelFsp->mboxAperture, KERNEL_FSP_MBOX_PORT,
                               &timeout, pPacket, packetSize);
}

NV_STATUS
kfspReadPacket_GR100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp,
    NvU8      *pPacket,
    NvU32      maxPacketSize,
    NvU32     *pBytesRead
)
{
    NvU32 recvSize = maxPacketSize;

    NV_ASSERT_OR_RETURN(pBytesRead != NULL, NV_ERR_INVALID_POINTER);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          gpuMnocMboxRecv_HAL(pGpu, &pKernelFsp->mboxAperture,
                                              KERNEL_FSP_MBOX_PORT,
                                              pPacket, &recvSize));
    *pBytesRead = recvSize;

    return NV_OK;
}

NvBool
kfspCanSendPacket_GR100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    // SendPacket is a blocking call
    return NV_TRUE;
}

NvBool
kfspIsResponseAvailable_GR100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    return gpuMnocMboxIsMsgAvailable_HAL(pGpu, &pKernelFsp->mboxAperture, KERNEL_FSP_MBOX_PORT);
}

NvU32
kfspGetMaxSendMessageSize_GR100
(
    OBJGPU *pGpu,
    KernelFsp *pKernelFsp
)
{
    //
    // FSP does not support receiving multi-packet messages over MNOC
    // RM sends faster than FSP can process since there is no per packet acknowledgement
    // 
    return  kfspGetMaxSendPacketSize_HAL(pGpu, pKernelFsp) - FSP_MESSAGE_HEADER_BYTES;
}

NvU32
kfspGetMaxSendPacketSize_GR100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    return NV_MIN(gpuMnocMboxMaxMessageSize_HAL(pGpu), KERNEL_FSP_MBOX_MAX_PACKET_BYTES);
}

NvU32
kfspGetMaxRecvPacketSize_GR100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    return gpuMnocMboxMaxMessageSize_HAL(pGpu);
}
  
