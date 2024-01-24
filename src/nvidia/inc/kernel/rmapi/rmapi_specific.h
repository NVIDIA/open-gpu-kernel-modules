/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _RMAPI_SPECIFIC_H
#define _RMAPI_SPECIFIC_H

#include "nvstatus.h"
#include "nvtypes.h"

#include "resserv/rs_resource.h"

/**
 * Make custom modifications to class-specific allocation params as needed.
 *
 * @param[inout] ppResDesc      Resource descriptor used for the allocation, may change if
 *                              the class's ID changes as a result of this function.
 * @param[in]    pRmAllocParams Allocation params
 *
 * @return NV_ERR_INVALID_CLASS if an invalid class transition happens, NV_OK otherwise.
 */
NV_STATUS rmapiFixupAllocParams(RS_RESOURCE_DESC **ppResDesc, RS_RES_ALLOC_PARAMS_INTERNAL *pRmAllocParams);

/**
 * Returns NV_TRUE if the control call specific by cmd can be invoked at an increased
 * IRQL level, NV_FALSE otherwise.
 *
 * @param[in] cmd Control call ID
 *
 * @return NV_TRUE if control can be RAISED_IRQL, NV_FALSE otherwise.
 */
NvBool rmapiRmControlCanBeRaisedIrql(NvU32 cmd);

/**
 * Returns NV_TRUE if the control call specific by cmd can bypass acquiring locks,
 * NV_FALSE otherwise.
 *
 * @param[in] cmd Control call ID
 *
 * @return NV_TRUE if control can be BYPASS_LOCK, NV_FALSE otherwise.
 */
NvBool rmapiRmControlCanBeBypassLock(NvU32 cmd);

#endif // _RMAPI_SPECIFIC_H
