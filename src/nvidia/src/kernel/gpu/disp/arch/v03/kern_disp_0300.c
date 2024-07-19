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

/******************************************************************************
*
*       Kernel Display Module
*       This file contains functions managing display on CPU RM
*
******************************************************************************/

#define RM_STRICT_CONFIG_EMIT_DISP_ENGINE_DEFINITIONS     0

#include "gpu/gpu.h"
#include "gpu/disp/kern_disp.h"
#include "gpu/disp/head/kernel_head.h"
#include "gpu/external_device/gsync.h"

#include "disp/v03_00/dev_disp.h"

#include "class/clc371.h"
#include "class/clc373.h"

NV_STATUS
kdispGetChannelNum_v03_00
(
    KernelDisplay *pKernelDisplay,
    DISPCHNCLASS channelClass,
    NvU32 channelInstance,
    NvU32 *pChannelNum
)
{
    NV_STATUS status = NV_ERR_INVALID_ARGUMENT;

    if (pChannelNum == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    const KernelDisplayStaticInfo *pStaticInfo = pKernelDisplay->pStaticInfo;
    NV_ASSERT_OR_RETURN(pStaticInfo != NULL, NV_ERR_INVALID_STATE);

    switch (channelClass)
    {
        case dispChnClass_Curs:
            if ((channelInstance < NV_PDISP_CHN_NUM_CURS__SIZE_1) &&
                (channelInstance < NV_PDISP_FE_HW_SYS_CAP_HEAD_EXISTS__SIZE_1))
            {
                if (FLD_IDX_TEST_DRF(_PDISP, _FE_HW_SYS_CAP, _HEAD_EXISTS, channelInstance, _YES, pStaticInfo->feHwSysCap))
                {
                    *pChannelNum = NV_PDISP_CHN_NUM_CURS(channelInstance);
                    status = NV_OK;
                }
                else
                {
                    status = NV_ERR_NOT_SUPPORTED;
                }
            }
            break;

        case dispChnClass_Winim:
            if (channelInstance < NV_PDISP_CHN_NUM_WINIM__SIZE_1)
            {
                if (pStaticInfo->windowPresentMask & NVBIT32(channelInstance))
                {
                    *pChannelNum = NV_PDISP_CHN_NUM_WINIM(channelInstance);
                    status = NV_OK;
                }
                else
                {
                    status = NV_ERR_NOT_SUPPORTED;
                }
            }
            break;

        case dispChnClass_Core:
            *pChannelNum = NV_PDISP_CHN_NUM_CORE;
            status = NV_OK;
            break;

        case dispChnClass_Win:
            if (channelInstance < NV_PDISP_CHN_NUM_WIN__SIZE_1)
            {
                if (pStaticInfo->windowPresentMask & NVBIT32(channelInstance))
                {
                    *pChannelNum = NV_PDISP_CHN_NUM_WIN(channelInstance);
                    status = NV_OK;
                }
                else
                {
                    status = NV_ERR_NOT_SUPPORTED;
                }
            }
            break;

        case dispChnClass_Any:
            // Assert incase of physical RM, Any channel is kernel only channel.
            NV_ASSERT_OR_RETURN(RMCFG_FEATURE_KERNEL_RM, NV_ERR_INVALID_CHANNEL);
            *pChannelNum = NV_PDISP_CHN_NUM_ANY;
            status = NV_OK;
            break;

        default:
            NV_PRINTF(LEVEL_ERROR, "Unknown channel class %x\n", channelClass);
            status = NV_ERR_INVALID_CHANNEL;
            DBG_BREAKPOINT();
            break;
    }

    return status;
}

/*!
 * @brief Get the register base address for display capabilities registers
 *
 * @param      pGpu
 * @param      pKernelDisplay
 * @param[out] pOffset NvU32 pointer to return base offset
 * @param[out] pSize   NvU32 pointer to return size
 */
void
kdispGetDisplayCapsBaseAndSize_v03_00
(
    OBJGPU        *pGpu,
    KernelDisplay *pKernelDisplay,
    NvU32         *pOffset,
    NvU32         *pSize
)
{
    if (pOffset)
    {
        // Tegra offsets needs to be subtracted with -0x610000.
        *pOffset = DRF_BASE(NV_PDISP_FE_SW) +
                   kdispGetBaseOffset_HAL(pGpu, pKernelDisplay);
    }

    if (pSize)
    {
        *pSize = sizeof(NvC373DispCapabilities_Map);
    }
}

/*!
 * @brief Get the register base address for SF user space.
 *
 * @param      pGpu
 * @param      pKernelDisplay
 * @param[out] pOffset NvU32 pointer to return base offset
 * @param[out] pSize   NvU32 pointer to return size
 */
void
kdispGetDisplaySfUserBaseAndSize_v03_00
(
    OBJGPU        *pGpu,
    KernelDisplay *pKernelDisplay,
    NvU32         *pOffset,
    NvU32         *pSize
)
{
    if (pOffset)
    {
        // Tegra offsets needs to be subtracted with -0x610000.
        *pOffset = DRF_BASE(NV_PDISP_SF_USER_0) +
                   kdispGetBaseOffset_HAL(pGpu, pKernelDisplay);
    }

    if (pSize)
    {
        *pSize = sizeof(NvC371DispSfUserMap);
    }
}

/*!
 * @brief Get the register base address and size of channel user area
 *
 * @param      pGpu
 * @param      pKernelDisplay
 * @param[in]  channelClass     Class of the channel
 * @param[in]  channelInstance  Channel instance #
 * @param[out] pOffset          User space bease address
 * @param[out] pSize            User space length (optional)
 *
 * @return NV_STATUS
 */
NV_STATUS
kdispGetDisplayChannelUserBaseAndSize_v03_00
(
    OBJGPU        *pGpu,
    KernelDisplay *pKernelDisplay,
    DISPCHNCLASS   channelClass,
    NvU32          channelInstance,
    NvU32         *pOffset,
    NvU32         *pSize
)
{
    NvU32 dispChannelNum;
    NV_STATUS status;

    if (pOffset == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    status = kdispGetChannelNum_HAL(pKernelDisplay, channelClass, channelInstance, &dispChannelNum);
    if (status != NV_OK)
        return status;

    NV_ASSERT(dispChannelNum < NV_UDISP_FE_CHN_ASSY_BASEADR__SIZE_1);

    *pOffset = NV_UDISP_FE_CHN_ASSY_BASEADR(dispChannelNum);

    //
    // The user are size for Core Channel is 64KB (32K for Armed and 32k for Assembly),
    // and all other channels are 4KB (2K for Armed and 2k for Assembly).
    //
    if (pSize != NULL)
    {
        switch (channelClass)
        {
            case dispChnClass_Curs:
                *pSize = NV_UDISP_FE_CHN_ASSY_BASEADR_CURS(dispChannelNum + 1) - NV_UDISP_FE_CHN_ASSY_BASEADR_CURS(dispChannelNum);
                break;

            case dispChnClass_Winim:
                *pSize = NV_UDISP_FE_CHN_ASSY_BASEADR_WINIM(dispChannelNum + 1) - NV_UDISP_FE_CHN_ASSY_BASEADR_WINIM(dispChannelNum);
                break;

            case dispChnClass_Core:
                *pSize = (NV_UDISP_FE_CHN_ARMED_BASEADR_CORE - NV_UDISP_FE_CHN_ASSY_BASEADR_CORE) * 2;
                break;

            case dispChnClass_Win:
                *pSize = NV_UDISP_FE_CHN_ASSY_BASEADR_WIN(dispChannelNum + 1) - NV_UDISP_FE_CHN_ASSY_BASEADR_WIN(dispChannelNum);
                break;

            default:
                break;
        }
    }

    return NV_OK;
}

/*!
 * @brief Validate selected sw class.
 *
 * @param[in]  pGpu         GPU object pointer
 * @param[in]  pKernelDisplay  KernelDisplay object pointer
 * @param[in]  swClass      Selected class name
 */
NV_STATUS
kdispSelectClass_v03_00_KERNEL
(
    OBJGPU *pGpu,
    KernelDisplay *pKernelDisplay,
    NvU32 swClass
)
{
    if (!gpuIsClassSupported(pGpu, swClass))
    {
        NV_PRINTF(LEVEL_ERROR, "class %x not supported\n", swClass);
        return NV_ERR_INVALID_CLASS;
    }

    return NV_OK;
}

/*!
 * @brief Read line count and frame count from RG_DPCA.
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelDisplay KernelDisplay pointer
 * @param[in]  head           head index
 * @param[out] pLineCount     line count
 * @param[out] pFrameCount    frame count
 *
 * @return NV_STATUS
 */
NV_STATUS
kdispReadRgLineCountAndFrameCount_v03_00_KERNEL
(
    OBJGPU        *pGpu,
    KernelDisplay *pKernelDisplay,
    NvU32          head,
    NvU32         *pLineCount,
    NvU32         *pFrameCount
)
{
    NvU32 data32;

    if (head >= kdispGetNumHeads(pKernelDisplay))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    data32 = GPU_REG_RD32(pGpu,NV_PDISP_RG_DPCA(head));

    *pLineCount = DRF_VAL(_PDISP, _RG_DPCA, _LINE_CNT, data32);
    *pFrameCount = DRF_VAL(_PDISP, _RG_DPCA, _FRM_CNT, data32);

    return NV_OK;
}

/*!
 * @brief - restore original  LSR_MIN_TIME
 *
 * @param[in]  pGpu            GPU  object pointer
 * @param[in]  pKernelDisplay  KernelDisplay pointer
 * @param[in]  head            head number
 * @param[out] oriLsrMinTime   original LSR_MIN_TIME value
 */
void
kdispRestoreOriginalLsrMinTime_v03_00
(
    OBJGPU  *pGpu,
    KernelDisplay *pKernelDisplay,
    NvU32    head,
    NvU32    origLsrMinTime
)
{
    NvU32 feFliplock;

    feFliplock = GPU_REG_RD32(pGpu, NV_PDISP_FE_FLIPLOCK);

    feFliplock = FLD_SET_DRF_NUM(_PDISP, _FE_FLIPLOCK, _LSR_MIN_TIME,
            origLsrMinTime, feFliplock);

    GPU_REG_WR32(pGpu, NV_PDISP_FE_FLIPLOCK, feFliplock);
}

/*!
 * @brief - Set LSR_MIN_TIME for swap barrier
 *
 * @param[in]  pGpu            GPU  object pointer
 * @param[in]  pKernelDisplay  KernelDisplay pointer
 * @param[in]  head            head number
 * @param[out] pOriLsrMinTime  pointer to original LSR_MIN_TIME value
 * @param[in]  pNewLsrMinTime  new LSR_MIN_TIME value to be set. If this is 0
 *                             use default LSR_MIN_TIME value of respective gpu.
 */
void
kdispSetSwapBarrierLsrMinTime_v03_00
(
    OBJGPU  *pGpu,
    KernelDisplay *pKernelDisplay,
    NvU32    head,
    NvU32   *pOrigLsrMinTime,
    NvU32    newLsrMinTime
)
{
    NvU32    dsiFliplock;

    dsiFliplock = GPU_REG_RD32(pGpu, NV_PDISP_FE_FLIPLOCK);
    *pOrigLsrMinTime = DRF_VAL(_PDISP, _FE_FLIPLOCK, _LSR_MIN_TIME, dsiFliplock);

    if (newLsrMinTime == 0)
    {
        dsiFliplock = FLD_SET_DRF_NUM(_PDISP, _FE_FLIPLOCK, _LSR_MIN_TIME,
                      FLIPLOCK_LSR_MIN_TIME_FOR_SAWP_BARRIER_V02, dsiFliplock);
    }
    else
    {
        dsiFliplock = FLD_SET_DRF_NUM(_PDISP, _FE_FLIPLOCK, _LSR_MIN_TIME,
                                      newLsrMinTime, dsiFliplock);
    }

    GPU_REG_WR32(pGpu, NV_PDISP_FE_FLIPLOCK, dsiFliplock);
}

/*!
 * @brief Get the LOADV counter
 *
 * @param[in]  pGpu                    OBJGPU pointer
 * @param[in]  pKernelHead             KernelHead object pointer
 *
 * @return the current LOADV counter
 */
NvU32
kheadGetLoadVCounter_v03_00
(
    OBJGPU                 *pGpu,
    KernelHead             *pKernelHead
)
{
    return GPU_REG_RD32(pGpu, NV_PDISP_POSTCOMP_HEAD_LOADV_COUNTER(pKernelHead->PublicId));
}

NvU32
kdispGetPBTargetAperture_v03_00
(
    OBJGPU        *pGpu,
    KernelDisplay *pKernelDisplay,
    NvU32         memAddrSpace,
    NvU32         cacheSnoop
)
{
    NvU32 pbTargetAperture = PHYS_NVM;

    if ((memAddrSpace == ADDR_SYSMEM) && (cacheSnoop != 0U))
    {
        pbTargetAperture = PHYS_PCI_COHERENT;
    }
    else if (memAddrSpace == ADDR_SYSMEM)
    {
        pbTargetAperture = PHYS_PCI;
    }
    else
    {
        pbTargetAperture = PHYS_NVM;
    }

    return pbTargetAperture;
}

NvU32
kheadReadPendingRgLineIntr_v03_00
(
    OBJGPU *pGpu,
    KernelHead *pKernelHead,
    THREAD_STATE_NODE   *pThreadState
)
{
    NvU32 intr;
    NvU32 headIntrMask = 0;

    intr = GPU_REG_RD32_EX(pGpu, NV_PDISP_FE_RM_INTR_DISPATCH, pThreadState);

    if (!FLD_IDX_TEST_DRF(_PDISP, _FE_RM_INTR_DISPATCH, _HEAD_TIMING, pKernelHead->PublicId, _PENDING, intr))
    {
        return headIntrMask;
    }

    intr = GPU_REG_RD32_EX(pGpu, NV_PDISP_FE_RM_INTR_STAT_HEAD_TIMING(pKernelHead->PublicId), pThreadState);

    if (FLD_TEST_DRF(_PDISP, _FE_EVT_STAT_HEAD_TIMING, _RG_LINE_A, _PENDING, intr))
    {
        headIntrMask |= headIntr_RgLineA;
    }

    if (FLD_TEST_DRF(_PDISP, _FE_EVT_STAT_HEAD_TIMING, _RG_LINE_B, _PENDING, intr))
    {
        headIntrMask |= headIntr_RgLineB;
    }

    return headIntrMask;
}

void
kheadResetRgLineIntrMask_v03_00
(
    OBJGPU *pGpu,
    KernelHead *pKernelHead,
    NvU32 headIntrMask,
    THREAD_STATE_NODE   *pThreadState
)
{
    NvU32 writeIntr = 0;

    if (headIntrMask & headIntr_RgLineA)
    {
        writeIntr |= DRF_DEF(_PDISP, _FE_EVT_STAT_HEAD_TIMING, _RG_LINE_A, _RESET);
    }

    if (headIntrMask & headIntr_RgLineB)
    {
        writeIntr |= DRF_DEF(_PDISP, _FE_EVT_STAT_HEAD_TIMING, _RG_LINE_B, _RESET);
    }

    GPU_REG_WR32_EX(pGpu, NV_PDISP_FE_EVT_STAT_HEAD_TIMING(pKernelHead->PublicId), writeIntr, pThreadState);
}

NvBool
kheadReadPendingVblank_v03_00
(
    OBJGPU *pGpu,
    KernelHead *pKernelHead,
    NvU32 *pCachedIntr,
    THREAD_STATE_NODE *pThreadState
)
{
    NvU32  intr = pCachedIntr ? *pCachedIntr : GPU_REG_RD32_EX(pGpu, NV_PDISP_FE_RM_INTR_DISPATCH, pThreadState);

    if (!FLD_IDX_TEST_DRF(_PDISP, _FE_RM_INTR_DISPATCH, _HEAD_TIMING, pKernelHead->PublicId, _PENDING, intr))
    {
        return NV_FALSE;
    }

    intr = GPU_REG_RD32_EX(pGpu, NV_PDISP_FE_RM_INTR_STAT_HEAD_TIMING(pKernelHead->PublicId), pThreadState);

    if (FLD_TEST_DRF(_PDISP, _FE_EVT_STAT_HEAD_TIMING, _LAST_DATA, _PENDING, intr))
    {
        return NV_TRUE;
    }

    return NV_FALSE;
}

void kheadResetPendingVblank_KERNEL(OBJGPU *pGpu, KernelHead *pKernelHead, THREAD_STATE_NODE *pThreadState)
{
    NvU32 writeIntr = GPU_REG_RD32(pGpu, NV_PDISP_FE_EVT_STAT_HEAD_TIMING(pKernelHead->PublicId));

    writeIntr = DRF_DEF(_PDISP, _FE_EVT_STAT_HEAD_TIMING, _LAST_DATA, _RESET);

    GPU_REG_WR32(pGpu, NV_PDISP_FE_EVT_STAT_HEAD_TIMING(pKernelHead->PublicId), writeIntr);
}
