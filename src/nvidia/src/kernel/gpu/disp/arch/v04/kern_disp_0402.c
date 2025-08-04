/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************************************************************
*
*       Kernel Display Module
*       This file contains functions managing display on CPU RM
*
******************************************************************************/

#define RM_STRICT_CONFIG_EMIT_DISP_ENGINE_DEFINITIONS     0

#include "os/os.h"
#include "gpu/gpu.h"
#include "gpu/disp/kern_disp.h"
#include "disp/v04_02/dev_disp.h"

/*!
 * @brief Return base offset for NV_PDISP that needs to be adjusted
 *        for register accesses.
 */
NvS32 kdispGetBaseOffset_v04_02
(
    OBJGPU  *pGpu,
    KernelDisplay *pKernelDisplay
)
{
    return (0x0 - DRF_BASE(NV_PDISP));
}

/*!
 * @brief Tracks display bandwidth requests and forwards highest request to ICC
 *
 * @param[in] pGpu                          OBJGPU pointer
 * @param[in] pKernelDisplay                KernelDisplay pointer
 * @param[in] iccBwClient                   Identifies requester
 *                                            (DISPLAY_ICC_BW_CLIENT_xxx value)
 * @param[in] minRequiredIsoBandwidthKBPS   ISO BW requested (KB/sec)
 * @param[in] minRequiredFloorBandwidthKBPS dramclk freq * pipe width (KB/sec)
 *
 * @returns NV_OK if successful,
 *          NV_ERR_INSUFFICIENT_RESOURCES if one of the bandwidth values is too
 *            high, and bandwidth cannot be allocated,
 *          NV_ERR_INVALID_PARAMETER if iccBwClient is not a valid value,
 *          NV_ERR_NOT_SUPPORTED if the functionality is not available, or
 *          NV_ERR_GENERIC if some other kind of error occurred.
 */
NV_STATUS
kdispArbAndAllocDisplayBandwidth_v04_02
(
    OBJGPU                     *pGpu,
    KernelDisplay              *pKernelDisplay,
    enum DISPLAY_ICC_BW_CLIENT  iccBwClient,
    NvU32                       minRequiredIsoBandwidthKBPS,
    NvU32                       minRequiredFloorBandwidthKBPS
)
{
typedef struct
{
    NvU32   minRequiredIsoBandwidthKBPS;
    NvU32   minRequiredFloorBandwidthKBPS;
} ICC_BW_VALUES;

    static ICC_BW_VALUES clientBwValues[NUM_DISPLAY_ICC_BW_CLIENTS] = {0};
    static ICC_BW_VALUES oldArbBwValues = {0};
    ICC_BW_VALUES newArbBwValues;
    NV_STATUS   status = NV_OK;

    NV_PRINTF(LEVEL_INFO, "%s requests ISO BW = %u KBPS, floor BW = %u KBPS\n",
              (iccBwClient == DISPLAY_ICC_BW_CLIENT_RM) ? "RM" :
              (iccBwClient == DISPLAY_ICC_BW_CLIENT_EXT) ? "Ext client" :
              "Unknown client",
              minRequiredIsoBandwidthKBPS,
              minRequiredFloorBandwidthKBPS);
    if (iccBwClient >= NUM_DISPLAY_ICC_BW_CLIENTS)
    {
        NV_PRINTF(LEVEL_ERROR, "Bad iccBwClient value (%u)\n", iccBwClient);
        NV_ASSERT(NV_FALSE);
        return NV_ERR_INVALID_PARAMETER;
    }
    if (iccBwClient == DISPLAY_ICC_BW_CLIENT_RM)
    {
        //
        // DD should have allocated the required ISO BW prior to the modeset.
        // It is not safe for RM to do the allocation because the allocation
        // may fail, but the modeset has already started and cannot be aborted.
        // (The only reason RM needs to put its ISO BW request in at all is to
        // make sure the required BW is maintained until all of the RM work at
        // the end of the modeset is done.)
        //
        NV_ASSERT(minRequiredIsoBandwidthKBPS <=
                  clientBwValues[DISPLAY_ICC_BW_CLIENT_EXT].minRequiredIsoBandwidthKBPS);
    }
    clientBwValues[iccBwClient].minRequiredIsoBandwidthKBPS = minRequiredIsoBandwidthKBPS;
    clientBwValues[iccBwClient].minRequiredFloorBandwidthKBPS = minRequiredFloorBandwidthKBPS;
    //
    // Make sure there are only two BW clients; otherwise, we would need a loop
    // to process the array elements.
    //
    ct_assert(NUM_DISPLAY_ICC_BW_CLIENTS <= 2);
    newArbBwValues.minRequiredIsoBandwidthKBPS =
        NV_MAX(clientBwValues[DISPLAY_ICC_BW_CLIENT_RM].minRequiredIsoBandwidthKBPS,
               clientBwValues[DISPLAY_ICC_BW_CLIENT_EXT].minRequiredIsoBandwidthKBPS);
    newArbBwValues.minRequiredFloorBandwidthKBPS =
        NV_MAX(clientBwValues[DISPLAY_ICC_BW_CLIENT_RM].minRequiredFloorBandwidthKBPS,
               clientBwValues[DISPLAY_ICC_BW_CLIENT_EXT].minRequiredFloorBandwidthKBPS);
    if ((oldArbBwValues.minRequiredIsoBandwidthKBPS !=
         newArbBwValues.minRequiredIsoBandwidthKBPS) ||
        (oldArbBwValues.minRequiredFloorBandwidthKBPS !=
         newArbBwValues.minRequiredFloorBandwidthKBPS))
    {
        NV_PRINTF(LEVEL_INFO, "Sending request to icc_set_bw: ISO BW = %u KBPS, floor BW = %u KBPS\n",
                  newArbBwValues.minRequiredIsoBandwidthKBPS,
                  newArbBwValues.minRequiredFloorBandwidthKBPS);
        status =
            osTegraAllocateDisplayBandwidth(pGpu->pOsGpuInfo,
                                            newArbBwValues.minRequiredIsoBandwidthKBPS,
                                            newArbBwValues.minRequiredFloorBandwidthKBPS);
        NV_PRINTF(LEVEL_INFO, "Allocation request returns:  %s (0x%08X)\n",
                  nvstatusToString(status), status);
        if (status == NV_OK)
        {
            oldArbBwValues = newArbBwValues;
        }
    }
    return status;
}

