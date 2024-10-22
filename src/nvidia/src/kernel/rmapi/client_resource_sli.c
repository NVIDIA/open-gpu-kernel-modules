/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "core/locks.h"
#include "rmapi/client_resource.h"

#include "platform/sli/sli.h"
#include "gpu_mgr/gpu_mgr.h"
#include "resserv/rs_client.h"
#include "resserv/rs_server.h"
#include "resserv/rs_access_map.h"

#include "ctrl/ctrl0000/ctrl0000gpu.h"

//
// cliresCtrlCmdGpuGetVideoLinks
//
// Lock Requirements:
//      Assert that API lock held on entry
//      No GPUs lock
//
NV_STATUS
cliresCtrlCmdGpuGetVideoLinks_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_GPU_GET_VIDEO_LINKS_PARAMS *pParams
)
{
    NV_ASSERT_OR_RETURN(rmapiLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    return gpumgrGetSliLinks(pParams);
}
