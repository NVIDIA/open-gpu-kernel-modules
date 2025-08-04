/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/fsp/kern_fsp.h"
#include "gpu/fsp/kern_fsp_retval.h"
#include "gpu/gsp/kernel_gsp.h"
#include "fsp/fsp_caps_query_rpc.h"
#include "fsp/fsp_clock_boost_rpc.h"
#include "fsp/fsp_nvdm_format.h"

#include "published/blackwell/gb100/dev_therm.h"
#include "published/blackwell/gb100/dev_therm_addendum.h"
#include "published/blackwell/gb100/dev_fsp_pri.h"
#include "published/blackwell/gb100/dev_fsp_addendum.h"
#include "published/blackwell/gb100/dev_gsp.h"
#include "published/blackwell/gb100/dev_oob_pri.h"

#include "os/os.h"
#include "nvRmReg.h"
#include "nverror.h"

#define CMS2_LOG_START   0x50U
#define CMS2_LOG_END     0x7FU
#define CMS2_LOG_DWORDS  (CMS2_LOG_END - CMS2_LOG_START + 1)
#define CMS2_LOG_BYTES   (CMS2_LOG_DWORDS * sizeof(NvU32))

static void _kfspPrintCms2Log_GB100(OBJGPU *pGpu, KernelFsp *pKernelFsp, NvU8 *cms2Log);
static NvBool _kfspWaitBootCond_GB100(OBJGPU *pGpu, void *pArg);
static void _kfspGatherCms2Log_GB100(OBJGPU *pGpu, NvU32  *cms2Log);

NV_STATUS
kfspWaitForSecureBoot_GB100
(
    OBJGPU    *pGpu,
    KernelFsp *pKernelFsp
)
{
    NV_STATUS status  = NV_OK;
    RMTIMEOUT timeout;

    //
    // Polling for FSP boot complete
    // FBFalcon training during devinit alone takes 2 seconds, up to 3 on HBM3,
    // but the default threadstate timeout on windows is 1800 ms. Increase to 4 seconds
    // for this wait to match MODS GetGFWBootTimeoutMs.
    // For flags, we must not use the GPU TMR since it is inaccessible.
    //
    gpuSetTimeout(pGpu, NV_MAX(gpuScaleTimeout(pGpu, 4000000), pGpu->timeoutData.defaultus),
                  &timeout, GPU_TIMEOUT_FLAGS_OSTIMER);

    status = gpuTimeoutCondWait(pGpu, _kfspWaitBootCond_GB100, NULL, &timeout);

    if (status != NV_OK)
    {
        NV_ASSERT_OK(gpuMarkDeviceForReset(pGpu));
        NV_ERROR_LOG((void*) pGpu, GPU_INIT_ERROR, "Error status 0x%x while polling for FSP boot complete, "
                     "0x%x, 0x%x, 0x%x, 0x%x, 0x%x",
                     status,
                     GPU_REG_RD32(pGpu, NV_THERM_I2CS_SCRATCH_FSP_BOOT_COMPLETE),
                     GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(0)),
                     GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(1)),
                     GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(2)),
                     GPU_REG_RD32(pGpu, NV_PFSP_FALCON_COMMON_SCRATCH_GROUP_2(3)));

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
        NV_ERROR_LOG((void*) pGpu, GPU_INIT_ERROR, "FSP fuse error check has failed. Status = 0x%x.",
                      GPU_REG_RD32(pGpu, NV_PFSP_FUSE_ERROR_CHECK));
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
    NvU32 i;
    NvU32 cms2Log[CMS2_LOG_DWORDS];
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
    for(i = 0; i < NV_PGSP_MAILBOX__SIZE_1; i++)
    {
        NV_PRINTF(LEVEL_ERROR, "NV_PGSP_MAILBOX(%d) = 0x%x\n",
                  i, GPU_REG_RD32(pGpu, NV_PGSP_MAILBOX(i)));
    }

    _kfspGatherCms2Log_GB100(pGpu, cms2Log);
    _kfspPrintCms2Log_GB100(pGpu, pKernelFsp, (NvU8*) cms2Log);
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

