/*
 * SPDX-FileCopyrightText: Copyright (c) 2024-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NVOC_KERNEL_NVLINK_H_PRIVATE_ACCESS_ALLOWED

#include "core/core.h"

#include "nvctassert.h"
#include "kernel/gpu/gpu.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/intr/intr.h"
#include "kernel/gpu_mgr/gpu_mgr.h"
#include "kernel/gpu/gpu_fabric_probe.h"
#include "nvlink_inband_msg.h"
#include "nvrm_registry.h"

ct_assert(NV2080_CTRL_NVLINK_MAX_REMAP_TABLE_ENTRIES == NVLINK_NVLE_MAX_REMAP_TABLE_ENTRIES);

/*!
 * Gets the alid of the GPU, and gets the alids and updates the clids in the remap table
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 * @param[in] bGpa           Whether to select FLA or SPA/GPA remap table
 * @param[in] bGet           Whether to get or set the LID information
 */
NV_STATUS
knvlinkEncryptionGetUpdateGpuIdentifiers_GB100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvBool        bGpa,
    NvBool        bGet
)
{
    if (pKernelNvlink == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pKernelNvlink->bGotNvleIdentifiers)
    {
        return NV_OK;
    }

    // Switch system
    if (GPU_IS_NVSWITCH_DETECTED(pGpu))
    {
        OBJSYS    *pSys    = SYS_GET_INSTANCE();
        OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);

        NV2080_CTRL_NVLINK_GET_UPDATE_NVLE_LIDS_PARAMS lidParams;
        GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel = pGpu->pGpuFabricProbeInfoKernel;

        if (pGpuFabricProbeInfoKernel == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "%s: pGpuFabricProbeInfoKernel is NULL\n", __FUNCTION__);
            return NV_ERR_INVALID_ARGUMENT;
        }

        if (!gpuFabricProbeIsReceived(pGpuFabricProbeInfoKernel))
        {
            NV_PRINTF(LEVEL_ERROR, "%s: Fabric probe has not been received\n", __FUNCTION__);
            return NV_ERR_NVLINK_FABRIC_FAILURE;
        }

        portMemSet(&lidParams, 0, sizeof(lidParams));
        lidParams.bGpa = (bGpa ? NV_TRUE : NV_FALSE);
        lidParams.bGet = (bGet ? NV_TRUE : NV_FALSE);

        if (bGet)
        {
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                                  knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                                      NV2080_CTRL_NVLINK_GET_UPDATE_NVLE_LIDS,
                                                      (void *)&lidParams, sizeof(lidParams)));

            // Cache the ALID for the GPU
            pKernelNvlink->alid = lidParams.alid;

            //
            // Determine the CLIDs for all the ALIDs returned from the remap table. Check if a mapping already
            // exists for this ALID, otherwise assign a new CLID
            //
            NvU32 remapTableIdx;
            for (remapTableIdx = 0; remapTableIdx < NV2080_CTRL_NVLINK_MAX_REMAP_TABLE_ENTRIES; remapTableIdx++)
            {
                // Check if the ALID is a valid one
                if (lidParams.alidList[remapTableIdx].bValid == NV_FALSE)
                {
                    continue;
                }
  
                // Check if the ALID is present in the ALID-CLID map
                NvU32 clid;
                if (!gpuMgrIsNvleAlidPresent(pGpuMgr, lidParams.alidList[remapTableIdx].alid, &clid))
                {
                    NV_CHECK_OR_RETURN(LEVEL_ERROR,
                        (gpuMgrCacheNvleAlid(pGpuMgr, lidParams.alidList[remapTableIdx].alid, &clid) == NV_TRUE),
                        NV_ERR_INVALID_STATE);
                }

                if (lidParams.alidList[remapTableIdx].alid == pKernelNvlink->alid)
                {
                    // Cache the CLID for the GPU
                    pKernelNvlink->clid = clid;
                }
            }
        }
        else
        {
            NvU32 clid;

            // Construct the lidList with CLIDs for updating the LID fields. Initialize all entries to invalid
            for (clid = 0; clid < NV2080_CTRL_NVLINK_MAX_REMAP_TABLE_ENTRIES; clid++)
            {
                lidParams.alidList[clid].bValid = NV_FALSE;
            }

            for (clid = 0; clid < pGpuMgr->alidClidMap.alidCount; clid++)
            {
                lidParams.alidList[clid].bValid = NV_TRUE;
                lidParams.alidList[clid].alid   = pGpuMgr->alidClidMap.alid[clid];
            }

            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                                  knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                                      NV2080_CTRL_NVLINK_GET_UPDATE_NVLE_LIDS,
                                                      (void *)&lidParams, sizeof(lidParams)));
        }
    }
    else // Direct connect system
    {
        pKernelNvlink->alid = (NvU16)pGpu->gpuId;
        pKernelNvlink->clid = pGpu->gpuId;
    }

    return NV_OK;
}


/*!
 * Update the NVLE topology information in the key manager
 *
 * @param[in] pGpu0           Local OBJGPU pointer
 * @param[in] pKernelNvlink0  Local KernelNvlink pointer
 * @param[in] pGpu1           Remote OBJGPU pointer
 * @param[in] pKernelNvlink1  Remote KernelNvlink pointer
 */
NV_STATUS
knvlinkEncryptionUpdateTopology_GB100
(
    OBJGPU       *pGpu0,
    KernelNvlink *pKernelNvlink0,
    OBJGPU       *pGpu1,
    KernelNvlink *pKernelNvlink1
)
{
    NV2080_CTRL_NVLINK_UPDATE_NVLE_TOPOLOGY_PARAMS params;
    NvU32 remapTableIdx;
    NvU32 status;

    portMemSet(&params, 0, sizeof(params));
    params.localGpuAlid  = knvlinkGetALID(pGpu0, pKernelNvlink0);
    params.localGpuClid  = knvlinkGetCLID(pGpu0, pKernelNvlink0);
    params.remoteGpuAlid = knvlinkGetALID(pGpu1, pKernelNvlink1);;
    params.remoteGpuClid = knvlinkGetCLID(pGpu1, pKernelNvlink1);

    status = knvlinkExecGspRmRpc(pGpu0, pKernelNvlink0,
                                NV2080_CTRL_NVLINK_UPDATE_NVLE_TOPOLOGY,
                                (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to execute GSP-RM GPC to update Nvlink topology in GSP\n");
        return status;
    }

    // Cache the FLA/GPA remap table addr for all the GPU CLIDs
    for (remapTableIdx = 0; remapTableIdx < NV2080_CTRL_NVLINK_MAX_REMAP_TABLE_ENTRIES; remapTableIdx++)
    {
        pKernelNvlink0->flaRemapTabAddr[remapTableIdx] = params.flaRemapTabAddr[remapTableIdx];
        pKernelNvlink0->gpaRemapTabAddr[remapTableIdx] = params.gpaRemapTabAddr[remapTableIdx];
    }

    return NV_OK;
}
