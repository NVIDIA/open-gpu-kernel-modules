/*
 * SPDX-FileCopyrightText: Copyright (c)2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
*       VblankCallback Module
*       This file contains functions managing the vblank callback.
*
******************************************************************************/

#include "gpu/disp/vblank_callback/vblank_callback.h"
#include "os/os.h"
#include "gpu/disp/kern_disp.h"
#include "gpu/disp/head/kernel_head.h"

static NV_STATUS
_vblankCallback
(
    OBJGPU     *pGpu,
    void       *pObject,
    NvU32       Parm1,
    NvU32       Parm2,
    NV_STATUS   rmStatus
)
{
    VblankCallback *pVblankCallback = (VblankCallback *)pObject;
    if (pVblankCallback->CallBack.bIsVblankNotifyEnable)
    {
        pVblankCallback->pProc(pVblankCallback->pParm1, pVblankCallback->pParm2);
    }
    return NV_OK;
}

NV_STATUS
vblcbConstruct_IMPL
(
    VblankCallback               *pVblankCallback,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    OBJSYS          *pSys           = SYS_GET_INSTANCE();
    OBJOS           *pOS            = SYS_GET_OS(pSys);
    OBJGPU          *pGpu           = GPU_RES_GET_GPU(pVblankCallback);
    KernelDisplay   *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    KernelHead      *pKernelHead    = NULL;
    NV_STATUS        status         = NV_OK;
    NV_VBLANK_CALLBACK_ALLOCATION_PARAMETERS *pAllocParams = pParams->pAllocParams;

    if (pCallContext->secInfo.privLevel < RS_PRIV_LEVEL_KERNEL)
    {
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    if (pKernelDisplay == NULL)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    pKernelHead = KDISP_GET_HEAD(pKernelDisplay,  pAllocParams->LogicalHead);

    if (pKernelHead == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pVblankCallback->LogicalHead = pAllocParams->LogicalHead;
    pVblankCallback->pProc = pAllocParams->pProc;
    pVblankCallback->pParm1 = pAllocParams->pParm1;
    pVblankCallback->pParm2 = pAllocParams->pParm2;
    pVblankCallback->CallBack.Flags = VBLANK_CALLBACK_FLAG_LOW_LATENCY | VBLANK_CALLBACK_FLAG_PERSISTENT | VBLANK_CALLBACK_FLAG_SPECIFIED_VBLANK_NEXT;
    pVblankCallback->CallBack.Proc = _vblankCallback;
    pVblankCallback->CallBack.pObject = pVblankCallback;
    pVblankCallback->CallBack.bObjectIsChannelDescendant = NV_FALSE;
    pVblankCallback->CallBack.Param1 = 0;
    pVblankCallback->CallBack.Param2 = 0;
    pVblankCallback->CallBack.VBlankCount = 0;
    pVblankCallback->CallBack.VBlankOffset = 0;
    pVblankCallback->CallBack.TimeStamp = 0;
    pVblankCallback->CallBack.MC_CallbackFlag = 0;
    pVblankCallback->CallBack.Status = NV_OK;
    pVblankCallback->CallBack.bIsVblankNotifyEnable = NV_TRUE;
    pVblankCallback->CallBack.Next = NULL;

    kheadAddVblankCallback(pGpu, pKernelHead, &pVblankCallback->CallBack);
    status = pOS->osSetupVBlank(pGpu, pAllocParams->pProc, pAllocParams->pParm1, pAllocParams->pParm2, pAllocParams->LogicalHead, &pVblankCallback->CallBack);

    if (status != NV_OK)
    {
        kheadDeleteVblankCallback(pGpu, pKernelHead, &pVblankCallback->CallBack);
    }

    return status;
}

void
vblcbDestruct_IMPL
(
    VblankCallback *pVblankCallback
)
{
    OBJSYS  *pSys  = SYS_GET_INSTANCE();
    OBJOS   *pOS   = SYS_GET_OS(pSys);
    OBJGPU  *pGpu  = GPU_RES_GET_GPU(pVblankCallback);
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    KernelHead    *pKernelHead    = KDISP_GET_HEAD(pKernelDisplay, pVblankCallback->LogicalHead);

    pOS->osSetupVBlank(pGpu, NULL, NULL, NULL, pVblankCallback->LogicalHead, NULL);
    kheadDeleteVblankCallback(pGpu, pKernelHead, &pVblankCallback->CallBack);
}

NV_STATUS
vblcbCtrlSetVBlankNotification_IMPL
(
    VblankCallback *pVblankCallback,
    NV9010_CTRL_CMD_SET_VBLANK_NOTIFICATION_PARAMS *pParams
)
{
    OBJGPU        *pGpu           = GPU_RES_GET_GPU(pVblankCallback);
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    KernelHead    *pKernelHead    = KDISP_GET_HEAD(pKernelDisplay, pVblankCallback->LogicalHead);
    NV_STATUS      status         = NV_ERR_INVALID_ARGUMENT;
    if (pVblankCallback->CallBack.Proc != NULL)
    {
        if (pParams->bSetVBlankNotifyEnable)
        {
            pVblankCallback->CallBack.bIsVblankNotifyEnable = NV_TRUE;
            kheadAddVblankCallback(pGpu, pKernelHead, &pVblankCallback->CallBack);
        }
        else
        {
            pVblankCallback->CallBack.bIsVblankNotifyEnable = NV_FALSE;
            kheadDeleteVblankCallback(pGpu, pKernelHead, &pVblankCallback->CallBack);
        }
        status = NV_OK;
    }
    return status;
}

