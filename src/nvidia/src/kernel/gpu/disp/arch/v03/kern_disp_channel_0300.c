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

#include "gpu/disp/kern_disp.h"
#include "disp/v03_00/dev_disp.h"

static CHNSTATEMAP dispHwChnStateToSwStateMapCurs_v03_00 [] =
{
    {NV_PDISP_FE_CHNSTATUS_CURS_STATE_DEALLOC,          dispChnState_Dealloc},
    {NV_PDISP_FE_CHNSTATUS_CURS_STATE_IDLE,             dispChnState_Idle},
    {NV_PDISP_FE_CHNSTATUS_CURS_STATE_BUSY,             dispChnState_Busy},
};

static CHNSTATEMAP dispHwChnStateToSwStateMapWinim_v03_00 [] =
{
    {NV_PDISP_FE_CHNSTATUS_WINIM_STATE_DEALLOC,         dispChnState_Dealloc},
    {NV_PDISP_FE_CHNSTATUS_WINIM_STATE_UNCONNECTED,     dispChnState_Unconnected},
    {NV_PDISP_FE_CHNSTATUS_WINIM_STATE_INIT1,           dispChnState_Initialize1},
    {NV_PDISP_FE_CHNSTATUS_WINIM_STATE_INIT2,           dispChnState_Initialize2},
    {NV_PDISP_FE_CHNSTATUS_WINIM_STATE_IDLE,            dispChnState_Idle},
    {NV_PDISP_FE_CHNSTATUS_WINIM_STATE_BUSY,            dispChnState_Busy},
    {NV_PDISP_FE_CHNSTATUS_WINIM_STATE_SHUTDOWN1,       dispChnState_Shutdown1},
    {NV_PDISP_FE_CHNSTATUS_WINIM_STATE_SHUTDOWN2,       dispChnState_Shutdown2},
};

static CHNSTATEMAP dispHwChnStateToSwStateMapCore_v03_00 [] =
{
    {NV_PDISP_FE_CHNSTATUS_CORE_STATE_DEALLOC,          dispChnState_Dealloc},
    {NV_PDISP_FE_CHNSTATUS_CORE_STATE_DEALLOC_LIMBO,    dispChnState_DeallocLimbo},
    {NV_PDISP_FE_CHNSTATUS_CORE_STATE_VBIOS_INIT1,      dispChnState_Vbiosinit1},
    {NV_PDISP_FE_CHNSTATUS_CORE_STATE_VBIOS_INIT2,      dispChnState_Vbiosinit2},
    {NV_PDISP_FE_CHNSTATUS_CORE_STATE_VBIOS_OPERATION,  dispChnState_Vbiosoper},
    {NV_PDISP_FE_CHNSTATUS_CORE_STATE_EFI_INIT1,        dispChnState_Fcodeinit1},
    {NV_PDISP_FE_CHNSTATUS_CORE_STATE_EFI_INIT2,        dispChnState_Fcodeinit2},
    {NV_PDISP_FE_CHNSTATUS_CORE_STATE_EFI_OPERATION,    dispChnState_Fcode},
    {NV_PDISP_FE_CHNSTATUS_CORE_STATE_UNCONNECTED,      dispChnState_Unconnected},
    {NV_PDISP_FE_CHNSTATUS_CORE_STATE_INIT1,            dispChnState_Initialize1},
    {NV_PDISP_FE_CHNSTATUS_CORE_STATE_INIT2,            dispChnState_Initialize2},
    {NV_PDISP_FE_CHNSTATUS_CORE_STATE_IDLE,             dispChnState_Idle},
    {NV_PDISP_FE_CHNSTATUS_CORE_STATE_BUSY,             dispChnState_Busy},
    {NV_PDISP_FE_CHNSTATUS_CORE_STATE_SHUTDOWN1,        dispChnState_Shutdown1},
    {NV_PDISP_FE_CHNSTATUS_CORE_STATE_SHUTDOWN2,        dispChnState_Shutdown2},
};

static CHNSTATEMAP dispHwChnStateToSwStateMapWin_v03_00 [] =
{
    {NV_PDISP_FE_CHNSTATUS_WIN_STATE_DEALLOC,           dispChnState_Dealloc},
    {NV_PDISP_FE_CHNSTATUS_WIN_STATE_UNCONNECTED,       dispChnState_Unconnected},
    {NV_PDISP_FE_CHNSTATUS_WIN_STATE_INIT1,             dispChnState_Initialize1},
    {NV_PDISP_FE_CHNSTATUS_WIN_STATE_INIT2,             dispChnState_Initialize2},
    {NV_PDISP_FE_CHNSTATUS_WIN_STATE_IDLE,              dispChnState_Idle},
    {NV_PDISP_FE_CHNSTATUS_WIN_STATE_BUSY,              dispChnState_Busy},
    {NV_PDISP_FE_CHNSTATUS_WIN_STATE_SHUTDOWN1,         dispChnState_Shutdown1},
    {NV_PDISP_FE_CHNSTATUS_WIN_STATE_SHUTDOWN2,         dispChnState_Shutdown2},
};

/*!
 * @brief Check if a channel has been allocated in HW.
 *
 * @param[in] pGpu              OBJGPU pointer
 * @param[in] pDisp             OBJDISP pointer
 * @param[in] channelClass      Class of the channel
 * @param[in] channelInstance   Channel instance #
 *
 * @return NV_TRUE if it is allocated, NV_FALSE otherwise.
 */
NvBool
kdispIsChannelAllocatedHw_v03_00
(
    POBJGPU      pGpu,
    KernelDisplay *pKernelDisplay,
    DISPCHNCLASS channelClass,
    NvU32        channelInstance
)
{
    NvBool allocated = NV_FALSE;
    NvU32 channelCtl;
    NvU32 chnCtlOffset = 0;
    NV_STATUS status; 

    status = kdispGetChnCtlRegs_HAL(pKernelDisplay, channelClass, channelInstance, &chnCtlOffset);
    NV_ASSERT(status == NV_OK);

    switch (channelClass)
    {
        case dispChnClass_Curs:
            channelCtl = GPU_REG_RD32(pGpu, chnCtlOffset);
            allocated = (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_CURS, _ALLOCATION, _ALLOCATE, channelCtl)) ? NV_TRUE : NV_FALSE;
            break;

        case dispChnClass_Winim:
            channelCtl = GPU_REG_RD32(pGpu, chnCtlOffset);
            allocated = (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_WINIM, _ALLOCATION, _ALLOCATE, channelCtl)) ? NV_TRUE : NV_FALSE;
            break;

        case dispChnClass_Core:
            channelCtl = GPU_REG_RD32(pGpu, chnCtlOffset);
            allocated = (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_CORE, _ALLOCATION, _ALLOCATE, channelCtl)) ? NV_TRUE : NV_FALSE;
            break;

        case dispChnClass_Win:
            channelCtl = GPU_REG_RD32(pGpu, chnCtlOffset);
            allocated = (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_WIN, _ALLOCATION, _ALLOCATE, channelCtl)) ? NV_TRUE : NV_FALSE;
            break;

        default:
            // We shouldn't reach here even if incorrect args are supplied
            DBG_BREAKPOINT();
            break;
    }

    return allocated;
}

/*!
 * @brief Apply the specified accelerate bits to accelerate channel transition.
 *
 * @param[in] pGpu                  OBJGPU pointer
 * @param[in] pDisp                 OBJDISP pointer
 * @param[in] channelClass          Class of the channel
 * @param[in] channelInstance       Channel instance #
 * @param[in] *pCurrChannelCtl      Current CHNCTL reg
 * @param[in] acceleratorsToApply   Accelerators to apply
 * @param[in] keepCurrAccelerators  Indicate to keep the current accelerators or not
 */
void
kdispApplyAccelerators_v03_00
(
    OBJGPU      *pGpu,
    KernelDisplay *pKernelDisplay,
    DISPCHNCLASS channelClass,
    NvU32        channelInstance,
    NvU32       *pCurrChannelCtl,
    NvU32        acceleratorsToApply,
    NvBool       keepCurrAccelerators
)
{
    NvU32 channelCtl = 0;
    NvU32 chnCtlOffset = 0;
    NV_STATUS status;

    if (pCurrChannelCtl != NULL)
        channelCtl = *pCurrChannelCtl;

    // Get the address for the chnCtl register we are interested in.
    status = kdispGetChnCtlRegs_HAL(pKernelDisplay, channelClass, channelInstance, &chnCtlOffset);
    NV_ASSERT(status == NV_OK);

    switch (channelClass)
    {
        case dispChnClass_Curs:
            if (pCurrChannelCtl == NULL)
                channelCtl = GPU_REG_RD32(pGpu, chnCtlOffset);

            if ((acceleratorsToApply == DISP_ACCL_NONE) || (keepCurrAccelerators == NV_FALSE))
            {
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CURS, _LOCK_PIO_FIFO,       _DISABLE,           channelCtl);
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CURS, _IGNORE_INTERLOCK,    _DISABLE,           channelCtl);
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CURS, _TRASH_MODE,          _DISABLE,           channelCtl);
            }

            if (acceleratorsToApply & DISP_ACCL_LOCK_PIO_FIFO)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CURS, _LOCK_PIO_FIFO,       _ENABLE,            channelCtl);

            if (acceleratorsToApply & DISP_ACCL_IGNORE_INTERLOCK)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CURS, _IGNORE_INTERLOCK,    _ENABLE,            channelCtl);

            if (acceleratorsToApply & DISP_ACCL_TRASH_ONLY)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CURS, _TRASH_MODE,          _TRASH_ONLY,        channelCtl);

            // If you specify both trash and trash and abort, RM will do trash and abort.
            if (acceleratorsToApply & DISP_ACCL_TRASH_AND_ABORT)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CURS, _TRASH_MODE,          _TRASH_AND_ABORT,   channelCtl);

            GPU_REG_WR32(pGpu, chnCtlOffset, channelCtl);
            break;

        case dispChnClass_Winim:
            if (pCurrChannelCtl == NULL)
                channelCtl = GPU_REG_RD32(pGpu, chnCtlOffset);

            if ((acceleratorsToApply == DISP_ACCL_NONE) || (keepCurrAccelerators == NV_FALSE))
            {
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WINIM, _PUTPTR_WRITE,       _ENABLE,            channelCtl);
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WINIM, _IGNORE_INTERLOCK,   _DISABLE,           channelCtl);
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WINIM, _TRASH_MODE,         _DISABLE,           channelCtl);
            }

            if (acceleratorsToApply & DISP_ACCL_DISABLE_PUTPTR_WRITE)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WINIM, _PUTPTR_WRITE,       _DISABLE,           channelCtl);

            if (acceleratorsToApply & DISP_ACCL_IGNORE_INTERLOCK)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WINIM, _IGNORE_INTERLOCK,   _ENABLE,            channelCtl);

            if (acceleratorsToApply & DISP_ACCL_TRASH_ONLY)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WINIM, _TRASH_MODE,         _TRASH_ONLY,        channelCtl);

            if (acceleratorsToApply & DISP_ACCL_TRASH_AND_ABORT)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WINIM, _TRASH_MODE,         _TRASH_AND_ABORT,   channelCtl);

            GPU_REG_WR32(pGpu, chnCtlOffset, channelCtl);
            break;

        case dispChnClass_Core:
            if (pCurrChannelCtl == NULL)
                channelCtl = GPU_REG_RD32(pGpu, chnCtlOffset);

            if ((acceleratorsToApply == DISP_ACCL_NONE) || (keepCurrAccelerators == NV_FALSE))
            {
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CORE, _PUTPTR_WRITE,        _ENABLE,            channelCtl);
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CORE, _SKIP_NOTIF,          _DISABLE,           channelCtl);
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CORE, _IGNORE_INTERLOCK,    _DISABLE,           channelCtl);
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CORE, _INTR_DURING_SHTDWN,  _DISABLE,           channelCtl);
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CORE, _TRASH_MODE,          _DISABLE,           channelCtl);
            }

            if (acceleratorsToApply & DISP_ACCL_DISABLE_PUTPTR_WRITE)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CORE, _PUTPTR_WRITE,        _DISABLE,           channelCtl);

            if (acceleratorsToApply & DISP_ACCL_SKIP_NOTIF)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CORE, _SKIP_NOTIF,          _ENABLE,            channelCtl);

            if (acceleratorsToApply & DISP_ACCL_IGNORE_INTERLOCK)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CORE, _IGNORE_INTERLOCK,    _ENABLE,            channelCtl);

            if (acceleratorsToApply & DISP_ACCL_DISABLE_INTR_DURING_SHTDWN)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CORE, _INTR_DURING_SHTDWN,  _DISABLE,           channelCtl);

            if (acceleratorsToApply & DISP_ACCL_TRASH_ONLY)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CORE, _TRASH_MODE,          _TRASH_ONLY,        channelCtl);

            if (acceleratorsToApply & DISP_ACCL_TRASH_AND_ABORT)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_CORE, _TRASH_MODE,          _TRASH_AND_ABORT,   channelCtl);

            GPU_REG_WR32(pGpu, chnCtlOffset, channelCtl);
            break;

        case dispChnClass_Win:
            if (pCurrChannelCtl == NULL)
                channelCtl = GPU_REG_RD32(pGpu, chnCtlOffset);

            if ((acceleratorsToApply == DISP_ACCL_NONE) || (keepCurrAccelerators == NV_FALSE))
            {
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _PUTPTR_WRITE,     _ENABLE,            channelCtl);
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _SKIP_SYNCPOINT,   _DISABLE,           channelCtl);
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _IGNORE_TIMESTAMP, _DISABLE,           channelCtl);
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _IGNORE_PI,        _DISABLE,           channelCtl);
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _SKIP_NOTIF,       _DISABLE,           channelCtl);
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _SKIP_SEMA,        _DISABLE,           channelCtl);
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _IGNORE_INTERLOCK, _DISABLE,           channelCtl);
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _TRASH_MODE,       _DISABLE,           channelCtl);
            }

            if (acceleratorsToApply & DISP_ACCL_DISABLE_PUTPTR_WRITE)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _PUTPTR_WRITE,     _DISABLE,           channelCtl);

            if (acceleratorsToApply & DISP_ACCL_SKIP_SYNCPOINT)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _SKIP_SYNCPOINT,   _DISABLE,           channelCtl);

            if (acceleratorsToApply & DISP_ACCL_IGNORE_TIMESTAMP)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _IGNORE_TIMESTAMP, _DISABLE,           channelCtl);

            if (acceleratorsToApply & DISP_ACCL_IGNORE_PI)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _IGNORE_PI,        _ENABLE,            channelCtl);

            if (acceleratorsToApply & DISP_ACCL_SKIP_NOTIF)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _SKIP_NOTIF,       _ENABLE,            channelCtl);

            if (acceleratorsToApply & DISP_ACCL_SKIP_SEMA)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _SKIP_SEMA,        _ENABLE,            channelCtl);

            if (acceleratorsToApply & DISP_ACCL_IGNORE_INTERLOCK)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _IGNORE_INTERLOCK, _ENABLE,            channelCtl);

            if (acceleratorsToApply & DISP_ACCL_TRASH_ONLY)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _TRASH_MODE,       _TRASH_ONLY,        channelCtl);

            if (acceleratorsToApply & DISP_ACCL_TRASH_AND_ABORT)
                channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _TRASH_MODE,       _TRASH_AND_ABORT,   channelCtl);

            GPU_REG_WR32(pGpu, chnCtlOffset, channelCtl);

            // WAR for method fetch doesn't drop pending methods when TRASH occurs. see HW bug 3385499 for details
            if ((acceleratorsToApply & DISP_ACCL_TRASH_AND_ABORT) ||
                (acceleratorsToApply & DISP_ACCL_TRASH_ONLY))
            {
                kdispApplyWarForBug3385499_HAL(pGpu, pKernelDisplay, channelClass, channelInstance, chnCtlOffset);
            }
            break;

        default:
            // We shouldn't reach here even if incorrect args are supplied
            DBG_BREAKPOINT();
            break;
    }

    // Copy out if required.
    if (pCurrChannelCtl != NULL)
        *pCurrChannelCtl = channelCtl;
}

/*!
 * @brief Return the CHNSTATUS register for the channel
 *
 * @param[in]  pGpu             OBJGPU pointer
 * @param[in]  channelClass     Class of the channel
 * @param[in]  channelInstance  Channel instance #
 * @param[out] *pChnStatus      CHNSTATUS register offset for the channel
  *
 * @return NV_OK on success.
 */
NV_STATUS
kdispGetChnStatusRegs_v03_00
(
    KernelDisplay       *pKernelDisplay,
    DISPCHNCLASS        channelClass,
    NvU32               channelInstance,
    NvU32               *pChnStatus
)
{
    NV_STATUS status = NV_ERR_INVALID_ARGUMENT;

    if (pChnStatus == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    switch (channelClass)
    {
        case dispChnClass_Curs:
            if (channelInstance < NV_PDISP_FE_CHNSTATUS_CURS__SIZE_1)
            {
                *pChnStatus = NV_PDISP_FE_CHNSTATUS_CURS(channelInstance);
                status = NV_OK;
            }
            break;

        case dispChnClass_Winim:
            if (channelInstance < NV_PDISP_FE_CHNSTATUS_WINIM__SIZE_1)
            {
                *pChnStatus = NV_PDISP_FE_CHNSTATUS_WINIM(channelInstance);
                status = NV_OK;
            }
            break;

        case dispChnClass_Core:
            *pChnStatus = NV_PDISP_FE_CHNSTATUS_CORE;
            status = NV_OK;
            break;

        case dispChnClass_Win:
            if (channelInstance < NV_PDISP_FE_CHNSTATUS_WIN__SIZE_1)
            {
                *pChnStatus = NV_PDISP_FE_CHNSTATUS_WIN(channelInstance);
                status = NV_OK;
            }
            break;

        default:
            status = NV_ERR_INVALID_ARGUMENT;
            break;
    }

    return status;
}

/*!
 * @brief Get the ChnCtl reg.
 *
 * @param[in]  pKernelDisplay   KernelDisplay pointer
 * @param[in]  channelClass     Class of the channel
 * @param[in]  channelInstance  Channel instance #
 * @param[out] *pChnCtl         CHNCTL register offset for the channel
 *
 * @return NV_OK on success.
 */
NV_STATUS
kdispGetChnCtlRegs_v03_00
(
    KernelDisplay       *pKernelDisplay,
    DISPCHNCLASS        channelClass,
    NvU32               channelInstance,
    NvU32               *pChnCtl
)
{
    NV_STATUS status = NV_ERR_INVALID_ARGUMENT;

    if (pChnCtl == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    switch (channelClass)
    {
        case dispChnClass_Curs:
            if (channelInstance < NV_PDISP_FE_CHNCTL_CURS__SIZE_1)
            {
                *pChnCtl = NV_PDISP_FE_CHNCTL_CURS(channelInstance);
                status = NV_OK;
            }
            break;

        case dispChnClass_Winim:
            if (channelInstance < NV_PDISP_FE_PBBASE_WINIM__SIZE_1)
            {
                *pChnCtl = NV_PDISP_FE_CHNCTL_WINIM(channelInstance);
                status = NV_OK;
            }
            break;

        case dispChnClass_Core:
            *pChnCtl = NV_PDISP_FE_CHNCTL_CORE;
            status = NV_OK;
            break;

        case dispChnClass_Win:
            if (channelInstance < NV_PDISP_FE_PBBASE_WIN__SIZE_1)
            {
                *pChnCtl = NV_PDISP_FE_CHNCTL_WIN(channelInstance);
                status = NV_OK;
            }
            break;

        default:
            status = NV_ERR_INVALID_ARGUMENT;
            break;
    }

    return status;
}

/*!
 * @brief WAR for HW bug 3385499. Only used in dispApplyAccelerators_v03_00
 *
 * @param[in] pGpu                  OBJGPU pointer
 * @param[in] pKernelDisplay        KernelDisplay pointer
 * @param[in] channelClass          Class of the channel
 * @param[in] channelInstance       Channel instance #
 * @param[in] chnCtlOffset          Address for the chnCtl register
 */
void
kdispApplyWarForBug3385499_v03_00
(
    OBJGPU              *pGpu,
    KernelDisplay       *pKernelDisplay,
    DISPCHNCLASS        channelClass,
    NvU32               channelInstance,
    NvU32               chnCtlOffset
)
{
    NvU32 chnStatus = 0;
    NvU32 chnStatusOffset = 0;
    NvU32 channelCtl = GPU_REG_RD32(pGpu, chnCtlOffset);
    NvU32 status = NV_OK;

    status = kdispGetChnStatusRegs_HAL(pKernelDisplay, channelClass, channelInstance, &chnStatusOffset);
    NV_ASSERT(status == NV_OK);

    // 10 micro seconds time out
    RMTIMEOUT timeout;
    gpuSetTimeout(pGpu, 10, &timeout, 0);

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
    }
    while (status == NV_OK);

    // disconnect the channel
    channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _CONNECTION, _DISCONNECT, channelCtl);
    GPU_REG_WR32(pGpu, chnCtlOffset, channelCtl);

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
    }
    while (status == NV_OK);

    // re-connect the channel
    channelCtl = FLD_SET_DRF(_PDISP, _FE_CHNCTL_WIN, _CONNECTION, _CONNECT, channelCtl);
    GPU_REG_WR32(pGpu, chnCtlOffset, channelCtl);

    return;
}

/*!
 * @brief Read the channel state.
 *
 * @param[in]  pGpu             OBJGPU pointer
 * @param[in]  pKernelDisplay   KernelDisplay pointer
 * @param[in]  channelClass     Class of the channel
 * @param[in]  channelInstance  Channel instance #
 * @param[out] pChannelState    Channel state
 *
 * @return NV_OK on success.
 */
NV_STATUS
kdispReadChannelState_v03_00
(
    OBJGPU         *pGpu,
    KernelDisplay  *pKernelDisplay,
    DISPCHNCLASS    channelClass,
    NvU32           channelInstance,
    DISPCHNSTATE   *pChannelState
)
{
    NV_STATUS       status = NV_OK;
    NvU32           hwChannelState;
    CHNSTATEMAP    *chnStateMap;
    NvU32           numChnStates = 0, i;
    DISPCHNSTATE    swChannelState = 0;
    NvU32           chnStatusOffset = 0;
    NvU32           chnStatusRegRead;

    if (pChannelState == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = kdispGetChnStatusRegs_HAL(pKernelDisplay, channelClass, channelInstance, &chnStatusOffset);
    if (status != NV_OK)
        goto kdispReadChannelState_v03_00Done;

    // Read the desired CHNSTATUS reg.
    chnStatusRegRead = GPU_REG_RD32(pGpu, chnStatusOffset);

    switch (channelClass)
    {
        case dispChnClass_Curs:
            hwChannelState = DRF_VAL(_PDISP, _FE_CHNSTATUS_CURS, _STATE, chnStatusRegRead);
            chnStateMap    = dispHwChnStateToSwStateMapCurs_v03_00;
            numChnStates   = NV_ARRAY_ELEMENTS(dispHwChnStateToSwStateMapCurs_v03_00);
            break;

        case dispChnClass_Winim:
            hwChannelState = DRF_VAL(_PDISP, _FE_CHNSTATUS_WINIM, _STATE, chnStatusRegRead);
            chnStateMap    = dispHwChnStateToSwStateMapWinim_v03_00;
            numChnStates   = NV_ARRAY_ELEMENTS(dispHwChnStateToSwStateMapWinim_v03_00);
            break;

        case dispChnClass_Core:
            hwChannelState = DRF_VAL(_PDISP, _FE_CHNSTATUS_CORE, _STATE, chnStatusRegRead);
            chnStateMap    = dispHwChnStateToSwStateMapCore_v03_00;
            numChnStates   = NV_ARRAY_ELEMENTS(dispHwChnStateToSwStateMapCore_v03_00);
            break;

        case dispChnClass_Win:
            hwChannelState = DRF_VAL(_PDISP, _FE_CHNSTATUS_WIN, _STATE, chnStatusRegRead);
            chnStateMap    = dispHwChnStateToSwStateMapWin_v03_00;
            numChnStates   = NV_ARRAY_ELEMENTS(dispHwChnStateToSwStateMapWin_v03_00);
            break;

        default:
            status = NV_ERR_INVALID_CLASS;
            // We shouldn't reach here even if incorrect args are supplied
            DBG_BREAKPOINT();
            goto kdispReadChannelState_v03_00Done;
    }

    for (i = 0; i < numChnStates; ++i)
    {
        if (chnStateMap[i].hwChannelState == hwChannelState)
        {
            swChannelState = chnStateMap[i].dispChnState;
            break;
        }
    }

    if (i == numChnStates)
    {
        // We failed to find a valid match
        status = NV_ERR_INVALID_STATE;
    }

    if (status == NV_OK)
    {
        *pChannelState = swChannelState;
    }

kdispReadChannelState_v03_00Done:
    return status;
}

/*!
 * @brief Determines which accelerator bits are currently set and returns
 *        them in pAccelerators.
 *
 * @param[in]  pGpu             OBJGPU pointer
 * @param[in]  pDisp            OBJDISP pointer
 * @param[in]  channelClass     Class of the channel
 * @param[in]  channelInstance  Channel instance #
 * @param[in]  *pCurrChannelCtl Current CHNCTL reg
 * @param[out] *pAccelerators   Accelerators mask
 *
 * @return NV_OK on success.
 */
NV_STATUS
kdispGetAccelerators_v03_00
(
    OBJGPU      *pGpu,
    KernelDisplay *pKernelDisplay,
    DISPCHNCLASS channelClass,
    NvU32        channelInstance,
    NvU32       *pCurrChannelCtl,
    NvU32       *pAccelerators
)
{
    NvU32 channelCtl = 0;
    NvU32 accelerators = DISP_ACCL_NONE;
    NvU32 chnCtlOffset = 0;
    NV_STATUS status;

    // The very purpose of calling this function is to set up pAccelerators
    if (pAccelerators == NULL)
        return NV_ERR_GENERIC;

    if (pCurrChannelCtl != NULL)
        channelCtl = *pCurrChannelCtl;

    // Get the address for the chnCtl register we are interested in.
    status = kdispGetChnCtlRegs_HAL(pKernelDisplay, channelClass, channelInstance, &chnCtlOffset);
    NV_ASSERT(status == NV_OK);

    switch (channelClass)
    {
        case dispChnClass_Curs:
            if (pCurrChannelCtl == NULL)
                channelCtl = GPU_REG_RD32(pGpu, chnCtlOffset);

            if (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_CURS, _LOCK_PIO_FIFO,       _ENABLE,            channelCtl))
                accelerators |= DISP_ACCL_LOCK_PIO_FIFO;

            if (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_CURS, _IGNORE_INTERLOCK,    _ENABLE,            channelCtl))
                accelerators |= DISP_ACCL_IGNORE_INTERLOCK;

            if (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_CURS, _TRASH_MODE,          _TRASH_ONLY,        channelCtl))
                accelerators |= DISP_ACCL_TRASH_ONLY;
            else if (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_CURS, _TRASH_MODE,     _TRASH_AND_ABORT,   channelCtl))
                accelerators |= DISP_ACCL_TRASH_AND_ABORT;

            break;

        case dispChnClass_Winim:
            if (pCurrChannelCtl == NULL)
                channelCtl = GPU_REG_RD32(pGpu, chnCtlOffset);

            if (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_WINIM, _PUTPTR_WRITE,       _DISABLE,           channelCtl))
                accelerators |= DISP_ACCL_DISABLE_PUTPTR_WRITE;

            if (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_WINIM, _IGNORE_INTERLOCK,   _ENABLE,            channelCtl))
                accelerators |= DISP_ACCL_IGNORE_INTERLOCK;

            if (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_WINIM, _TRASH_MODE,         _TRASH_ONLY,        channelCtl))
                accelerators |= DISP_ACCL_TRASH_ONLY;
            else if (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_WINIM, _TRASH_MODE,    _TRASH_AND_ABORT,   channelCtl))
                accelerators |= DISP_ACCL_TRASH_AND_ABORT;

            break;

        case dispChnClass_Core:
            if (pCurrChannelCtl == NULL)
                channelCtl = GPU_REG_RD32(pGpu, chnCtlOffset);

            if (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_CORE, _PUTPTR_WRITE,        _DISABLE,           channelCtl))
                accelerators |= DISP_ACCL_DISABLE_PUTPTR_WRITE;

            if (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_CORE, _SKIP_NOTIF,          _ENABLE,            channelCtl))
                accelerators |= DISP_ACCL_SKIP_NOTIF;

            if (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_CORE, _IGNORE_INTERLOCK,    _ENABLE,            channelCtl))
                accelerators |= DISP_ACCL_IGNORE_INTERLOCK;

            if (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_CORE, _INTR_DURING_SHTDWN,  _DISABLE,           channelCtl))
                accelerators |= DISP_ACCL_DISABLE_INTR_DURING_SHTDWN;

            if (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_CORE, _TRASH_MODE,          _TRASH_ONLY,        channelCtl))
                accelerators |= DISP_ACCL_TRASH_ONLY;
            else if (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_CORE, _TRASH_MODE,     _TRASH_AND_ABORT,   channelCtl))
                accelerators |= DISP_ACCL_TRASH_AND_ABORT;

            break;

        case dispChnClass_Win:
            if (pCurrChannelCtl == NULL)
                channelCtl = GPU_REG_RD32(pGpu, chnCtlOffset);

            if (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_WIN, _PUTPTR_WRITE,         _DISABLE,           channelCtl))
                accelerators |= DISP_ACCL_DISABLE_PUTPTR_WRITE;

            if (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_WIN, _SKIP_SYNCPOINT,       _ENABLE,           channelCtl))
                accelerators |= DISP_ACCL_SKIP_SYNCPOINT;

            if (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_WIN, _IGNORE_TIMESTAMP,     _ENABLE,           channelCtl))
                accelerators |= DISP_ACCL_IGNORE_TIMESTAMP;

            if (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_WIN, _IGNORE_PI,            _ENABLE,            channelCtl))
                accelerators |= DISP_ACCL_IGNORE_PI;

            if (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_WIN, _SKIP_NOTIF,           _ENABLE,            channelCtl))
                accelerators |= DISP_ACCL_SKIP_NOTIF;

            if (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_WIN, _SKIP_SEMA,            _ENABLE,            channelCtl))
                accelerators |= DISP_ACCL_SKIP_SEMA;

            if (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_WIN, _IGNORE_INTERLOCK,     _ENABLE,            channelCtl))
                accelerators |= DISP_ACCL_IGNORE_INTERLOCK;

            if (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_WIN, _TRASH_MODE,           _TRASH_ONLY,        channelCtl))
                accelerators |= DISP_ACCL_TRASH_ONLY;
            else if (FLD_TEST_DRF(_PDISP, _FE_CHNCTL_WIN, _TRASH_MODE,      _TRASH_AND_ABORT,   channelCtl))
                accelerators |= DISP_ACCL_TRASH_AND_ABORT;

            break;

        default:
            // We shouldn't reach here even if incorrect args are supplied
            DBG_BREAKPOINT();
            break;
    }

    // Copy out if required.
    if (pCurrChannelCtl != NULL)
        *pCurrChannelCtl = channelCtl;

    *pAccelerators = accelerators;

    return NV_OK;
}

/*!
 * @brief 
 *
 * @param[in]  
 * @param[in]  
 * @param[in]  
 * @param[out] 
 *
 * @return NV_OK on success.
 */
NV_STATUS
kdispReadDebugStatus_v03_00
(
    OBJGPU         *pGpu,
    KernelDisplay  *pKernelDisplay,
    DISPCHNCLASS    channelClass,
    NvU32           channelInstance,
    NvBool         *pIsDebugEnabled
)
{
    NV_STATUS       status = NV_OK;
    NvU32           debugCtlRegVal;
    NvU32           dispChannelNum;

    NV_ASSERT_OR_RETURN(((channelClass == dispChnClass_Win) ||
                         (channelClass == dispChnClass_Core)),
                        NV_ERR_INVALID_ARGUMENT);

    NV_ASSERT_OR_RETURN((pIsDebugEnabled != NULL), NV_ERR_INVALID_ARGUMENT);

    status = kdispGetChannelNum_HAL(pKernelDisplay, channelClass, channelInstance, &dispChannelNum);
    if (status != NV_OK)
        goto done;

    *pIsDebugEnabled = NV_FALSE;

    // Read the desired debug chnCtl reg value.
    debugCtlRegVal  = GPU_REG_RD32(pGpu, NV_PDISP_FE_DEBUG_CTL(dispChannelNum)) & 0x1;

    if (FLD_TEST_DRF(_PDISP, _FE_DEBUG_CTL, _MODE, _ENABLE, debugCtlRegVal))
    {
        *pIsDebugEnabled = NV_TRUE;
    }
    else
    {
        *pIsDebugEnabled = NV_FALSE;
    }

 done:
    return status;
}
