/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//******************************************************************************
//
//   Module: rpcgm204.c
//
//   Description:
//       This module implements RPC fuctions for maxwell family.
//
//******************************************************************************

#include "vgpu/rpc.h"
#include "vgpu/dev_vgpu.h"
#include "gpu/bif/kernel_bif.h"
#include "g_rpc_private.h"

NV_STATUS rpcGetEngineUtilizationWrapper_GM204(OBJGPU *pGpu,
                                               OBJRPC *pRpc,
                                               NvHandle hClient,
                                               NvHandle hObject,
                                               NvU32 cmd,
                                               void *pParamStructPtr,
                                               NvU32 paramSize)

{

    NV_STATUS status;

    status = rpcGetEngineUtilization_HAL(pGpu, pRpc, hClient, hObject, cmd,
                                         pParamStructPtr, paramSize);

    return status;
}

