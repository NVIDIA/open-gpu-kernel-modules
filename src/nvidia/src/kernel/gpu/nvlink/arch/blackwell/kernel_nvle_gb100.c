/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvctassert.h"
#include "kernel/gpu/gpu.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/intr/intr.h"
#include "kernel/gpu_mgr/gpu_mgr.h"
#include "kernel/gpu/gpu_fabric_probe.h"
#include "nvlink_inband_msg.h"
#include "nvrm_registry.h"

/*
 * Gets the GPUs Alid, Clid, and DCF
 */
NV_STATUS
knvlinkEncryptionGetGpuIdentifiers_GB100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    if (pKernelNvlink == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    // Switch system
    if (GPU_IS_NVSWITCH_DETECTED(pGpu))
    {
        NV2080_CTRL_NVLINK_GET_NVLE_LIDS_PARAMS lidParams;
        NvU32 probeClid;
        GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel = pGpu->pGpuFabricProbeInfoKernel;

        if (pGpuFabricProbeInfoKernel == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "%s: pGpuFabricProbeInfoKernel is NULL\n", __FUNCTION__);
            return NV_ERR_INVALID_ARGUMENT;
        }

        if (!gpuFabricProbeIsReceived(pGpuFabricProbeInfoKernel))
        {
            NV_PRINTF(LEVEL_ERROR, "%s: GPU%u Fabric probe has not been received\n", __FUNCTION__, gpuGetInstance(pGpu));
            return NV_ERR_NVLINK_FABRIC_FAILURE;
        }
        
        // Get CLID from probe response
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            gpuFabricProbeGetRemapTableIndex(pGpuFabricProbeInfoKernel, &probeClid));

        // Get CLID from remap table entry
        portMemSet(&lidParams, 0, sizeof(lidParams));
        lidParams.probeClid = probeClid;
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                                knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                NV2080_CTRL_NVLINK_GET_NVLE_LIDS,
                                (void *)&lidParams, sizeof(lidParams)));
        pKernelNvlink->alid = lidParams.alid;

        // Validate and store clid
        if (probeClid != lidParams.clid)
        {
            NV_PRINTF(LEVEL_ERROR, "Remap clid %d doesn't match probe response clid %d\n", lidParams.clid, probeClid);
            return NV_ERR_INVALID_DATA;
        }
        pKernelNvlink->clid = probeClid;
    }
    else // Direct connect system
    {
        pKernelNvlink->alid = (NvU16)pGpu->gpuId;
        pKernelNvlink->clid = pGpu->gpuId;
    }
    return NV_OK;
}

NV_STATUS
knvlinkEncryptionUpdateTopology_GB100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32   remoteGpuAlid,
    NvU32   remoteGpuClid
)
{
    NV2080_CTRL_NVLINK_UPDATE_NVLE_TOPOLOGY_PARAMS params;
    NvU32 status;
    params.localGpuAlid = pKernelNvlink->alid;
    params.localGpuClid = pKernelNvlink->clid;
    params.remoteGpuAlid = remoteGpuAlid;
    params.remoteGpuClid = remoteGpuClid;
    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                NV2080_CTRL_NVLINK_UPDATE_NVLE_TOPOLOGY,
                                (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to execute GSP-RM GPC to update Nvlink topology in GSP\n");
    }

    return status;
}