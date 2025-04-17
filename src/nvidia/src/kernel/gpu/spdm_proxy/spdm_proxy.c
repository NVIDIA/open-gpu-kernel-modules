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

/*!
 * Provides the implementation for SPDM module that interacts between RM and SPDM.
 */
/* ------------------------------ Includes ---------------------------------- */
#include "gpu/spdm/spdm.h"
#include "gpu/conf_compute/conf_compute.h"
#include "core/locks.h"
#include "gpu/gpu.h"
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
 * @brief spdmProxyCtrlSpdmPartition
 *        The physical layer function to handle cotrol request from kernel SPDM.
 *
 * @param[in]      pGpu                     : OBJGPU Pointer
 * @param[in]      pSpdmPartitionParams     : SPDM RPC structure pointer
 *
 * @return  return NV_OK if no error detected.
 */
NV_STATUS spdmProxyCtrlSpdmPartition_IMPL
(
    OBJGPU                                     *pGpu,
    NV2080_CTRL_INTERNAL_SPDM_PARTITION_PARAMS *pSpdmPartitionParams
)
{

    return NV_ERR_NOT_SUPPORTED;

}


