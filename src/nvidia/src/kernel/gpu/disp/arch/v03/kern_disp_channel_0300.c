/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/gpu.h"
#include "gpu/disp/kern_disp.h"
#include "gpu/disp/disp_channel.h"
#include "gpu/disp/disp_objs.h"
#include "os/os.h"

#include "disp/v03_00/dev_disp.h"

#include "class/clc371.h"
#include "class/clc373.h"

static NV_STATUS
_kdispGetChnStatusRegs
(
    KernelDisplay       *pKernelDisplay,
    DISPCHNCLASS        channelClass,
    NvU32               channelInstance,
    NvU32               *pChnStatus
)
{
    NV_STATUS status = NV_OK;

    switch (channelClass)
    {
        case dispChnClass_Curs:
            if (channelInstance < NV_PDISP_FE_CHNSTATUS_CURS__SIZE_1)
            {
                *pChnStatus = NV_PDISP_FE_CHNSTATUS_CURS(channelInstance);
            }
            break;

        case dispChnClass_Winim:
            if (channelInstance < NV_PDISP_FE_CHNSTATUS_WINIM__SIZE_1)
            {
                *pChnStatus = NV_PDISP_FE_CHNSTATUS_WINIM(channelInstance);
            }
            break;

        case dispChnClass_Core:
            *pChnStatus = NV_PDISP_FE_CHNSTATUS_CORE;
            break;

        case dispChnClass_Win:
            if (channelInstance < NV_PDISP_FE_CHNSTATUS_WIN__SIZE_1)
            {
                *pChnStatus = NV_PDISP_FE_CHNSTATUS_WIN(channelInstance);
            }
            break;

        default:
            status = NV_ERR_INVALID_ARGUMENT;
            break;
    }

    return status;
}

static NV_STATUS _kdispGetChnCtlRegs
(
    KernelDisplay *pKernelDisplay,
    NvU32 channelClass,
    NvU32 channelInstance,
    NvU32 *pChnCtl
)
{
    NV_STATUS status = NV_OK;

    switch (channelClass)
    {
        case dispChnClass_Curs:
            if (channelInstance < NV_PDISP_FE_CHNCTL_CURS__SIZE_1)
            {
                *pChnCtl = NV_PDISP_FE_CHNCTL_CURS(channelInstance);
            }
            break;

        case dispChnClass_Winim:
            if (channelInstance < NV_PDISP_FE_PBBASE_WINIM__SIZE_1)
            {
                *pChnCtl = NV_PDISP_FE_CHNCTL_WINIM(channelInstance);
            }
            break;

        case dispChnClass_Core:
            *pChnCtl = NV_PDISP_FE_CHNCTL_CORE;
            break;

        case dispChnClass_Win:
            if (channelInstance < NV_PDISP_FE_PBBASE_WIN__SIZE_1)
            {
                *pChnCtl = NV_PDISP_FE_CHNCTL_WIN(channelInstance);
            }
            break;

        default:
            status = NV_ERR_INVALID_ARGUMENT;
            break;
    }

    return status;
}

NvBool kdispIsChannelAllocatedHw_v03_00
(
    OBJGPU *pGpu,
    KernelDisplay *pKernelDisplay,
    NvU32   channelClass,
    NvU32   channelInstance
)
{
    NV_STATUS status;
    NvU32 offset = 0;
    NvU32 channelCtl;
    NvBool allocated = NV_FALSE;

    status = _kdispGetChnCtlRegs(pKernelDisplay, channelClass, channelInstance, &offset);    
    NV_ASSERT(status == NV_OK);
    channelCtl = GPU_REG_RD32(pGpu, offset);

    switch (channelClass)
    {
        case dispChnClass_Curs:
            allocated = (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_CURS, _ALLOCATION, _ALLOCATE, channelCtl)) ? NV_TRUE : NV_FALSE;
            break;

        case dispChnClass_Winim:
            allocated = (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_WINIM, _ALLOCATION, _ALLOCATE, channelCtl)) ? NV_TRUE : NV_FALSE;
            break;

        case dispChnClass_Core:
            allocated = (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_CORE, _ALLOCATION, _ALLOCATE, channelCtl)) ? NV_TRUE : NV_FALSE;
            break;

        case dispChnClass_Win:
            allocated = (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_WIN, _ALLOCATION, _ALLOCATE, channelCtl)) ? NV_TRUE : NV_FALSE;
            break;

        default:
            DBG_BREAKPOINT();
            break;            
    }

    return allocated;
}

void
kdispSetChannelTrashAndAbortAccel_v03_00
(
    OBJGPU *pGpu,
    KernelDisplay *pKernelDisplay,
    NvU32  internalChnClass,
    NvU32  channelInstance,
    NvBool bSet
)
{
    NV_STATUS status;
    NvU32 chnCtlOffset = 0;
    NvU32 channelCtl = 0;

    status = _kdispGetChnCtlRegs(pKernelDisplay, internalChnClass, channelInstance, &chnCtlOffset);
    NV_ASSERT_OR_RETURN_VOID(status == NV_OK);

    channelCtl = GPU_REG_RD32(pGpu, chnCtlOffset);

    switch (internalChnClass)
    {
        case dispChnClass_Curs:
            channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CURS, _LOCK_PIO_FIFO,       _DISABLE,           channelCtl);
            channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CURS, _IGNORE_INTERLOCK,    _DISABLE,           channelCtl);
            channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CURS, _TRASH_MODE,          _DISABLE,           channelCtl);

            if (bSet)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CURS, _TRASH_MODE,      _TRASH_AND_ABORT,   channelCtl);

            GPU_REG_WR32(pGpu, chnCtlOffset, channelCtl);
            break;

        case dispChnClass_Winim:
            channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WINIM, _PUTPTR_WRITE,       _ENABLE,            channelCtl);
            channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WINIM, _IGNORE_INTERLOCK,   _DISABLE,           channelCtl);
            channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WINIM, _TRASH_MODE,         _DISABLE,           channelCtl);

            if (bSet)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WINIM, _TRASH_MODE,     _TRASH_AND_ABORT,   channelCtl);

            GPU_REG_WR32(pGpu, chnCtlOffset, channelCtl);
            break;

        case dispChnClass_Core:
            channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CORE, _PUTPTR_WRITE,        _ENABLE,            channelCtl);
            channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CORE, _SKIP_NOTIF,          _DISABLE,           channelCtl);
            channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CORE, _IGNORE_INTERLOCK,    _DISABLE,           channelCtl);
            channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CORE, _INTR_DURING_SHTDWN,  _DISABLE,           channelCtl);
            channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CORE, _TRASH_MODE,          _DISABLE,           channelCtl);

            if (bSet)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CORE, _TRASH_MODE,      _TRASH_AND_ABORT,   channelCtl);

            GPU_REG_WR32(pGpu, chnCtlOffset, channelCtl);
            break;

        case dispChnClass_Win:
            channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _PUTPTR_WRITE,     _ENABLE,            channelCtl);
            channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _SKIP_SYNCPOINT,   _DISABLE,           channelCtl);
            channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _IGNORE_TIMESTAMP, _DISABLE,           channelCtl);
            channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _IGNORE_PI,        _DISABLE,           channelCtl);
            channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _SKIP_NOTIF,       _DISABLE,           channelCtl);
            channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _SKIP_SEMA,        _DISABLE,           channelCtl);
            channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _IGNORE_INTERLOCK, _DISABLE,           channelCtl);
            channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _TRASH_MODE,       _DISABLE,           channelCtl);

            if (bSet)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _TRASH_MODE,   _TRASH_AND_ABORT,   channelCtl);

            GPU_REG_WR32(pGpu, chnCtlOffset, channelCtl);

            // Add a pushbuffer disconnect/re-connect sequence for Trash mode
            if (bSet)
                kdispApplyChannelConnectDisconnect_HAL(pGpu, pKernelDisplay, channelInstance, internalChnClass, chnCtlOffset);
            break;

        default:
            // We shouldn't reach here even if incorrect args are supplied
            DBG_BREAKPOINT();
            break;
    }
}

NvBool
kdispIsChannelIdle_v03_00
(
    OBJGPU *pGpu,
    KernelDisplay *pKernelDisplay,
    NvU32  channelClass,
    NvU32  channelInstance
)
{
    NV_STATUS       status = NV_OK;
    NvU32           hwChannelState;
    NvU32           chnStatusOffset = 0;
    NvU32           chnStatusRegRead;
    NvU32           channelState = 0;
    NvU32           timeoutValus = 10;
    RMTIMEOUT       timeout;


    status = _kdispGetChnStatusRegs(pKernelDisplay, channelClass, channelInstance, &chnStatusOffset);
    NV_ASSERT_OR_RETURN(status == NV_OK, NV_FALSE);

    gpuSetTimeout(pGpu, timeoutValus, &timeout, 0);

    do
    {
        chnStatusRegRead = GPU_REG_RD32(pGpu, chnStatusOffset);

        switch (channelClass)
        {
            case dispChnClass_Curs:
                hwChannelState = DRF_VAL(_PDISP, _FE_CHNSTATUS_CURS, _STATE, chnStatusRegRead);
                channelState = NVBIT(NV_PDISP_FE_CHNSTATUS_CURS_STATE_IDLE);
                break;

            case dispChnClass_Winim:
                hwChannelState = DRF_VAL(_PDISP, _FE_CHNSTATUS_WINIM, _STATE, chnStatusRegRead);
                channelState = NVBIT(NV_PDISP_FE_CHNSTATUS_WINIM_STATE_IDLE);
                break;

            case dispChnClass_Core:
                hwChannelState = DRF_VAL(_PDISP, _FE_CHNSTATUS_CORE, _STATE, chnStatusRegRead);
                channelState = NVBIT(NV_PDISP_FE_CHNSTATUS_CORE_STATE_IDLE);
                break;

            case dispChnClass_Win:
                hwChannelState = DRF_VAL(_PDISP, _FE_CHNSTATUS_WIN, _STATE, chnStatusRegRead);
                channelState = NVBIT(NV_PDISP_FE_CHNSTATUS_WIN_STATE_IDLE);
                break;

            default:
                status = NV_ERR_INVALID_CLASS;
                return NV_FALSE;
        }

        if (gpuCheckTimeout(pGpu, &timeout) == NV_ERR_TIMEOUT)
        {
            NV_PRINTF(LEVEL_ERROR, "timeout! current Channel state = 0x%x\n", hwChannelState);
            return NV_FALSE;
        }

        // loosen this loop
        osSpinLoop();    
    } while (!(NVBIT(hwChannelState) & channelState));

    return NV_TRUE;
}

void
kdispApplyChannelConnectDisconnect_v03_00
(
    OBJGPU      *pGpu,
    KernelDisplay *pKernelDisplay,
    NvU32        channelInstance,
    NvU32        channelClass,
    NvU32        chnCtlOffset
)
{
    NvU32 chnStatus = 0;
    NvU32 channelCtl = GPU_REG_RD32(pGpu, chnCtlOffset);
    NvU32 status = NV_OK;
    NvU32 chnStatusOffset = 0;
    NvU32 timeoutValus = 10;
    RMTIMEOUT timeout;

    status = _kdispGetChnStatusRegs(pKernelDisplay, channelClass, channelInstance, &chnStatusOffset);
    NV_ASSERT(status == NV_OK);

    gpuSetTimeout(pGpu, timeoutValus, &timeout, 0);
    // wait for METHOD_EXEC to go idle
    do
    {
        chnStatus = GPU_REG_RD32(pGpu, chnStatusOffset);

        if (FLD_TEST_DRF(_PDISP, _FE_CHNSTATUS_WIN, _STATUS_METHOD_EXEC, _IDLE, chnStatus))
        {
            break;
        }

        if (gpuCheckTimeout(pGpu, &timeout) == NV_ERR_TIMEOUT)
        {
            NV_PRINTF(LEVEL_ERROR,
                "timeout waiting for METHOD_EXEC to IDLE\n");
                status = NV_ERR_TIMEOUT;
                break;
        }
        osSpinLoop();    
    }
    while (status == NV_OK);

    // disconnect the channel
    channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _CONNECTION, _DISCONNECT, channelCtl);
    GPU_REG_WR32(pGpu, chnCtlOffset, channelCtl);

    gpuSetTimeout(pGpu, timeoutValus, &timeout, 0);
    // wait for channel to get disconnected
    do
    {
        chnStatus = GPU_REG_RD32(pGpu, chnStatusOffset);

        if (FLD_TEST_DRF(_PDISP, _FE_CHNSTATUS_WIN, _STATE, _UNCONNECTED, chnStatus))
        {
            break;
        }

        if (gpuCheckTimeout(pGpu, &timeout) == NV_ERR_TIMEOUT)
        {
            NV_PRINTF(LEVEL_ERROR,
                "timeout waiting for channel state to UNCONNECTED\n");
            status = NV_ERR_TIMEOUT;
            break;
        }
        osSpinLoop();    
    }
    while (status == NV_OK);

    // re-connect the channel
    channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _CONNECTION, _CONNECT, channelCtl);
    GPU_REG_WR32(pGpu, chnCtlOffset, channelCtl);
}
