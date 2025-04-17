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

#pragma once
#include "g_lock_test_nvoc.h"

#ifndef LOCK_TEST_H
#define LOCK_TEST_H

#include "gpu/gpu_resource.h"
#include "nvoc/prelude.h"
#include "nvstatus.h"
#include "resserv/resserv.h"

NVOC_PREFIX(lockTestRelaxedDupObj) class LockTestRelaxedDupObject : GpuResource
{
public:
    NV_STATUS lockTestRelaxedDupObjConstruct(LockTestRelaxedDupObject *pResource,
                                             CALL_CONTEXT *pCallContext,
                                             RS_RES_ALLOC_PARAMS_INTERNAL *pParams) :
                                             GpuResource(pCallContext, pParams);

    void lockTestRelaxedDupObjDestruct(LockTestRelaxedDupObject *pResource);
    virtual NvBool lockTestRelaxedDupObjCanCopy(LockTestRelaxedDupObject *pResource) {return NV_TRUE;};

};

#endif // LOCK_TEST_H
