/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * Provides the implementation for SPDM module that interacts between RM and SPDM.
 */
/* ------------------------------ Includes ---------------------------------- */
#include "gpu/spdm/spdm.h"
#include "gpu/conf_compute/conf_compute.h"
#include "core/locks.h"
#include "ctrl/ctrl2080/ctrl2080spdm.h"
#include "rmapi/client_resource.h"
#include "gpu/conf_compute/ccsl.h"
#include "gpu/conf_compute/conf_compute.h"
#include "spdm/rmspdmvendordef.h"
#include "gpu/timer/objtmr.h"
/* ------------------------- Macros and Defines ----------------------------- */

/* ------------------------- Static Functions ------------------------------ */

/* ------------------------- Public Functions ------------------------------ */

/*!
 * @brief spdmCtrlSpdmPartition
 *        Common function used to call gspCommandPostBlocking based on the platform on which it runs (i.e. vGPU, GSP-RM, Monolithic).
 *
 * @param[in]     pGpu                     : OBJGPU Pointer
 * @param[in]     pSpdmPartitionParams     : SPDM RPC structure pointer
 */
NV_STATUS spdmCtrlSpdmPartition
(
    OBJGPU                                     *pGpu,
    NV2080_CTRL_INTERNAL_SPDM_PARTITION_PARAMS *pSpdmPartitionParams
)
{
    NV_STATUS status = NV_OK;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    if (IS_VIRTUAL(pGpu))
    {
        // The control call currently doesn't support the vGPU environment, therefore return NV_ERR_NOT_SUPPORTED.
        return NV_ERR_NOT_SUPPORTED;
    }
    else if (IS_GSP_CLIENT(pGpu))
    {
        RM_API *pRmApi   = GPU_GET_PHYSICAL_RMAPI(pGpu);

        // Calls the subdeviceCtrlCmdSpdmPartition_IMPL control call in Physical RM mode.
        status = pRmApi->Control(pRmApi,
                                 pGpu->hInternalClient,
                                 pGpu->hInternalSubdevice,
                                 NV2080_CTRL_INTERNAL_SPDM_PARTITION,
                                 pSpdmPartitionParams,
                                 sizeof(NV2080_CTRL_INTERNAL_SPDM_PARTITION_PARAMS));
    }
    else
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "spdmCtrlSpdmPartition failed with error 0x%0x\n", status);
    }

    return status;
}

