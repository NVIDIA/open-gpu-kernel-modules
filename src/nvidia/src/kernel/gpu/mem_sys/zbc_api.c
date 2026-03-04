/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/mem_sys/zbc_api.h"
#include "vgpu/rpc.h"

NV_STATUS
zbcapiCtrlCmdGetZbcClearTableSize_VF
(
    ZbcApi *pZbcApi,
    NV9096_CTRL_GET_ZBC_CLEAR_TABLE_SIZE_PARAMS *pGetZBCClearTableSizeParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pZbcApi);
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);
    NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_INVALID_STATE);

    if (pGetZBCClearTableSizeParams->tableType >= NV9096_CTRL_ZBC_CLEAR_TABLE_TYPE_COUNT)
        return NV_ERR_INVALID_PARAMETER;

    pGetZBCClearTableSizeParams->indexStart = pVSI->zbcTableSizes[pGetZBCClearTableSizeParams->tableType].indexStart;
    pGetZBCClearTableSizeParams->indexEnd   = pVSI->zbcTableSizes[pGetZBCClearTableSizeParams->tableType].indexEnd;
    return NV_OK;
}
