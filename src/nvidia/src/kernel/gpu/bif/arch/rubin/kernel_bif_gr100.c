/*
 * SPDX-FileCopyrightText: Copyright (c) 2025-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/gpu.h"

#include "nverror.h"
#include "gpu/bif/kernel_bif.h"
#include "published/rubin/gr100/dev_pcfg_pf0.h"
#include "published/rubin/gr100/dev_nv_pcie_reset_addendum.h"

/*!
 * @brief Do CXL reset for Fn0 of GPU
 *
 * @param[in]  pGpu  GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 *
 * @return  NV_OK if successful
 */
NV_STATUS kbifDoCxlReset_GR100
(
    OBJGPU      *pGpu,
    KernelBif   *pKernelBif
)
{
    NV_STATUS status = NV_OK;

    pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_FULLCHIP_RESET, NV_TRUE);
    status = osDoCxlReset(pGpu);
    return status;
}

/*!
 * @brief Helper function to encode reset state as a mask
 *
 * @param[in] bFlrPend        FLR pending bit
 * @param[in] bConvPend       Conventional reset pending bit
 * @param[in] bFlrAlive       FLR Alive bit
 * @param[in] bConvAlive      Conventional reset alive bit
 * @param[in] bIllegalUphyAck Illegal Uphy Ack detect
 * 
 * @return    resetFailMask   Mask of reset status bits
 */
static inline
NvU32 _kbifEncodeResetFailStatus_GR100
(
    NvBool bFlrPend,
    NvBool bConvPend,
    NvBool bFlrAlive,
    NvBool bConvAlive,
    NvBool bIllegalUphyAck
)
{
    NvU32 resetFailMask = 0;

    if (bIllegalUphyAck)
    {
        resetFailMask = FLD_SET_DRF_NUM(_PCIE, _RESET_INFO, _ILLEGAL_UPHY_ACK, 0x1, resetFailMask);
    }

    if (bConvAlive)
    {
        resetFailMask = FLD_SET_DRF_NUM(_PCIE, _RESET_INFO, _CONV_RESET_ALIVE, 0x1, resetFailMask);
    }

    if (bFlrAlive)
    {
        resetFailMask = FLD_SET_DRF_NUM(_PCIE, _RESET_INFO, _FLR_ALIVE, 0x1, resetFailMask);
    }

    if (bConvPend)
    {
        resetFailMask = FLD_SET_DRF_NUM(_PCIE, _RESET_INFO, _CONV_RESET_PENDING, 0x1, resetFailMask);
    }

    if (bFlrPend)
    {
        resetFailMask = FLD_SET_DRF_NUM(_PCIE, _RESET_INFO, _FLR_PENDING, 0x1, resetFailMask);
    }

    return resetFailMask;
}

/*!
 * @brief Classify reset status, recommend the action, and emit XiD
 *
 * @param[in]  pGpu          GPU object pointer
 * @param[in]  pKernelBif    Kernel BIF object pointer
 * @param[in]  resetFailMask Encoded mask for reset fail status
 */
static void
_kbifReportResetStatus_GR100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32      resetFailMask
)
{
    NvBool                        bEmitXiD = NV_TRUE;
    NvU32                         resetStatus;
    KBIF_RESET_RECOMMENDED_ACTION resetRecAction;

    switch (resetFailMask)
    {
        case NV_PCIE_RESET_STATUS_PASS:
            {
                resetStatus    = NV_PCIE_RESET_STATUS_PASS;
                resetRecAction = NV_PCIE_RESET_ACTION_NONE;
                bEmitXiD       = NV_FALSE;
                break;
            }

        case NV_PCIE_RESET_STATUS_ILLEGAL_ACK:
            {
                resetStatus    = NV_PCIE_RESET_STATUS_ILLEGAL_ACK;
                resetRecAction = NV_PCIE_RESET_ACTION_SBR;
                break;
            }

        case NV_PCIE_RESET_STATUS_FLR_ASSERTION_TIMEOUT:
            {
                resetStatus    = NV_PCIE_RESET_STATUS_FLR_ASSERTION_TIMEOUT;
                resetRecAction = NV_PCIE_RESET_ACTION_SBR;
                break;
            }

        case NV_PCIE_RESET_STATUS_FLR_DEASSERTION_TIMEOUT:
            {
                resetStatus    = NV_PCIE_RESET_STATUS_FLR_DEASSERTION_TIMEOUT;
                resetRecAction = NV_PCIE_RESET_ACTION_SBR;
                break;
            }

        case NV_PCIE_RESET_STATUS_CONV_RESET_ASSERTION_TIMEOUT:
            {
                resetStatus    = NV_PCIE_RESET_STATUS_CONV_RESET_ASSERTION_TIMEOUT;
                resetRecAction = NV_PCIE_RESET_ACTION_SBR;
                break;
            }

        case NV_PCIE_RESET_STATUS_CONV_RESET_DEASSERTION_TIMEOUT:
            {
                resetStatus    = NV_PCIE_RESET_STATUS_CONV_RESET_DEASSERTION_TIMEOUT;
                resetRecAction = NV_PCIE_RESET_ACTION_SBR;
                break;
            }

        default:
            {
                resetStatus    = NV_PCIE_RESET_STATUS_UNCLASSIFIED;
                resetRecAction = NV_PCIE_RESET_ACTION_NONE;
            }
    }

    if (bEmitXiD)
    {
        switch (resetRecAction)
        {
            case NV_PCIE_RESET_ACTION_SBR:
                {
                    nvErrorLog_va((void *)pGpu, PCIE_RESET_FAILURE,
                        "PCIE_RESET_FAILURE: Classification: %d, Recommendation: Perform SBR\n", resetStatus);

                    gpuMarkDeviceForReset(pGpu);
                    break;
                }

            case NV_PCIE_RESET_ACTION_NONE:
            default:
                {
                    // We don't know how this could even occur, so print what's known
                    nvErrorLog_va((void *)pGpu, PCIE_RESET_FAILURE,
                        "Possible PCIE_RESET_FAILURE: Classification: %d, Recommendation: Unknown\n", resetStatus);
                }
        }
    }
}

/*!
 * @brief 1. Read reset state from DVSEC registers
 *        2. Classify the error status
 *        3. Recommend Action
 *        4. Emit XiD
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 * 
 * @return     NV_OK                 Able to check reset status and raise XiD
 *             NV_ERR_NOT_SUPPORTED  Config space register read failed
 */
NV_STATUS
kbifCheckResetStatus_GR100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32  regVal;
    NvU32  resetFailMask;
    NvBool bFlrResetPending;
    NvBool bFlrResetAlive;
    NvBool bConvResetPending;
    NvBool bConvResetAlive;
    NvBool bIllegalUphyAck;

    // 1. Read reset state from DVSEC registers
    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_DESIGNATED_VENDOR_SPECIFIC_0_HEADER_2_AND_GENERAL, &regVal) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_PF0_DESIGNATED_VENDOR_SPECIFIC_0_HEADER_2_AND_GENERAL\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    bFlrResetPending  = DRF_VAL(_PF0, _DESIGNATED_VENDOR_SPECIFIC_0_HEADER_2_AND_GENERAL,
                                _FLR_PENDING, regVal);

    bFlrResetAlive    = DRF_VAL(_PF0, _DESIGNATED_VENDOR_SPECIFIC_0_HEADER_2_AND_GENERAL,
                                _FLR_ALIVE, regVal);

    bConvResetPending = DRF_VAL(_PF0, _DESIGNATED_VENDOR_SPECIFIC_0_HEADER_2_AND_GENERAL,
                                _CONV_RESET_PENDING, regVal);

    bConvResetAlive   = DRF_VAL(_PF0, _DESIGNATED_VENDOR_SPECIFIC_0_HEADER_2_AND_GENERAL,
                                _CONV_RESET_ALIVE, regVal);

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_DVSEC0_GPU_BOOT_STATUS, &regVal) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_PF0_DVSEC0_GPU_BOOT_STATUS\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    bIllegalUphyAck = DRF_VAL(_PF0, _DVSEC0_GPU_BOOT_STATUS, _ILLEGAL_UPHY_ACK, regVal);

    // Create encoded mask for reset fail state 
    resetFailMask = _kbifEncodeResetFailStatus_GR100(bFlrResetPending, bConvResetPending,
                              bFlrResetAlive, bConvResetAlive, bIllegalUphyAck);

    //
    // 2. Classify the error status
    // 3. Recommend Action
    // 4. Emit XiD
    //
    _kbifReportResetStatus_GR100(pGpu, pKernelBif, resetFailMask);

    return NV_OK;
}
