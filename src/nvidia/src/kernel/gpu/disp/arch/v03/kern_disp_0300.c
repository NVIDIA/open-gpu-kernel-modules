/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
