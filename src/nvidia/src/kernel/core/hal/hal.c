/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/system.h"
#include "core/hal_mgr.h"
#include "core/hal.h"

#include "vgpu/rpc.h"

#include "g_hal_private.h"

PMODULEDESCRIPTOR
objhalGetModuleDescriptor_IMPL(OBJHAL *thisHal)
{
    return &thisHal->moduleDescriptor;
}

//
// registerHalModule() is referred by functions in generated file g_hal_private.h
// So, placed it here instead of gt_hal_register.h to avoid duplications of this
// function as g_hal_private.h is included by several files
//
NV_STATUS
registerHalModule(NvU32 halImpl, const HAL_IFACE_SETUP *pHalSetIfaces)
{
    OBJSYS           *pSys = SYS_GET_INSTANCE();
    OBJHALMGR        *pHalMgr = SYS_GET_HALMGR(pSys);
    OBJHAL           *pHal;
    PMODULEDESCRIPTOR pMod;
    NV_STATUS         rmStatus;

    // create a HAL object
    rmStatus = halmgrCreateHal(pHalMgr, halImpl);
    if (rmStatus != NV_OK)
    {
        return rmStatus;
    }

    // retrieve the HAL object
    pHal = HALMGR_GET_HAL(pHalMgr, halImpl);
    NV_ASSERT(pHal);

    // init the iface descriptor lists
    pMod = objhalGetModuleDescriptor(pHal);

    // point to rmconfig structure that can init our engines' interfaces
    pMod->pHalSetIfaces = pHalSetIfaces;

    return NV_OK;
}
