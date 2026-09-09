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

/*!
 * Gets the alid of the GPU, and gets the alids and updates the clids in the remap table
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 * @param[in] bGet           Whether to get or set the LID information
 */
NV_STATUS
knvlinkEncryptionGetUpdateGpuIdentifiers_GB100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvBool        bGet
)
{
    NV_ASSERT_OR_RETURN((pKernelNvlink != NULL),  NV_ERR_INVALID_ARGUMENT);

    // Switch system
    if (GPU_IS_NVSWITCH_DETECTED(pGpu))
    {
        OBJSYS    *pSys    = SYS_GET_INSTANCE();
        OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
        NvU32      clid;

        NV2080_CTRL_NVLINK_GET_UPDATE_NVLE_LIDS_V2_PARAMS lidParams;
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
        lidParams.bGet = (bGet ? NV_TRUE : NV_FALSE);

        if (bGet)
        {
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                                  knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                                      NV2080_CTRL_NVLINK_GET_UPDATE_NVLE_LIDS_V2,
                                                      (void *)&lidParams, sizeof(lidParams)));

            // Cache the ALID, CLID for the GPU and the ALID-CLID mappings into GPUMGR
            pKernelNvlink->alid         = lidParams.alid;
            pKernelNvlink->clid         = lidParams.clid;
            pKernelNvlink->bClidUpdated = lidParams.bClidUpdated;

            if (!pKernelNvlink->bClidUpdated)
            {
                //
                // Multi-node setup:
                // We should never reach here on multi-node setups where NVLE LIDs get and update are first
                // triggered by direct NV2080_CTRL_NVLINK_GET_UPDATE_NVLE_LIDS calls. Subsequently, if this
                // function gets called, CLIDs would have been updated and bClidUpdated should be true.
                //
                // Single-node setup:
                // On single node setups, however, GPUMGR acts as a temporary storage for the ALID-CLID map
                // while the map is being constructed and it finally gets copied over to each GPU's Kernel
                // Nvlink object after the CLIDs are updated.
                //
                if (!gpuMgrIsNvleAlidPresent(pGpuMgr, pKernelNvlink->alid, &clid))
                {
                    NV_CHECK_OR_RETURN(LEVEL_ERROR,
                        (gpuMgrCacheNvleAlidClid(pGpuMgr, pKernelNvlink->alid, pKernelNvlink->clid) == NV_TRUE),
                        NV_ERR_INVALID_STATE);
                }
            }
        }
        else
        {
            //
            // Construct the lidList with CLIDs for updating the LID fields. Initialize all entries to invalid.
            // Note: This path is taken only on single node setups. On multi-node setups, the ALID-CLID map is
            // passed in through direct calls to NV2080_CTRL_NVLINK_GET_UPDATE_NVLE_LIDS from client.
            //
            lidParams.alidClidTable.numEntries = NV2080_CTRL_NVLINK_MAX_ALID_CLID_TABLE_ENTRIES;
            portMemCopy(lidParams.alidClidTable.alidClidMap,
                        NV2080_CTRL_NVLINK_MAX_ALID_CLID_TABLE_ENTRIES * sizeof(ALID_CLID_MAP),
                        pGpuMgr->alidClidTable.alidClidMap,
                        NV2080_CTRL_NVLINK_MAX_ALID_CLID_TABLE_ENTRIES * sizeof(ALID_CLID_MAP));

            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                                  knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                                      NV2080_CTRL_NVLINK_GET_UPDATE_NVLE_LIDS_V2,
                                                      (void *)&lidParams, sizeof(lidParams)));

            // Cache the ALID, CLID for the GPU and the ALID-CLID mappings into KernelNvlink
            pKernelNvlink->alid         = lidParams.alid;
            pKernelNvlink->clid         = lidParams.clid;
            pKernelNvlink->bClidUpdated = lidParams.bClidUpdated;
        }

        if (pKernelNvlink->bClidUpdated)
        {
            // CLIDs are updated for this GPU's remap table. Copy the ALID-CLID map to KernelNvlink
            pKernelNvlink->alidClidTable.numEntries = lidParams.alidClidTable.numEntries;
            portMemCopy(pKernelNvlink->alidClidTable.alidClidMap,
                        NV2080_CTRL_NVLINK_MAX_ALID_CLID_TABLE_ENTRIES * sizeof(ALID_CLID_MAP),
                        lidParams.alidClidTable.alidClidMap,
                        NV2080_CTRL_NVLINK_MAX_ALID_CLID_TABLE_ENTRIES * sizeof(ALID_CLID_MAP));
        }
    }
    else // Direct connect system
    {
        pKernelNvlink->alid = (NvU16)pGpu->gpuId;
        pKernelNvlink->clid = pGpu->gpuId;
    }

    return NV_OK;
}
