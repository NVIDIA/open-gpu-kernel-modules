/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "published/blackwell/gb100/dev_pcfg_pf0.h"
#include "published/blackwell/gb100/dev_vm.h"
#include "gpu/bif/kernel_bif.h"
#include "platform/chipset/chipset.h"
#include "ctrl/ctrl2080/ctrl2080bus.h"
#include "os/os.h"

/*!
 * @brief Check if MSIX is enabled in HW
 *
 * @param[in] pGpu        GPU object pointer
 * @param[in] pKernelBif  Kernel BIF object pointer
 *
 * @return  True if MSIX enabled else False
 */
NvBool
kbifIsMSIXEnabledInHW_GB100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 regVal;

    // TODO : Return NV_TRUE for GSP-RM temporarily
    if (RMCFG_FEATURE_PLATFORM_GSP)
    {
        return NV_TRUE;
    }

    if (IS_VIRTUAL(pGpu))
    {
        // SR-IOV guests only support MSI-X
        return IS_VIRTUAL_WITH_SRIOV(pGpu);
    }
    else
    {
        if (GPU_BUS_CFG_CYCLE_RD32(pGpu,NV_PF0_MSIX_CAPABILITY_HEADR_AND_CONTROL,
                                   &regVal) != NV_OK)
        {
            NV_ASSERT_FAILED("Unable to read NV_PF0_MSIX_CAPABILITY_HEADR_AND_CONTROL\n");
            return NV_FALSE;
        }
        return FLD_TEST_DRF_NUM(_PF0, _MSIX_CAPABILITY,
                                _HEADR_AND_CONTROL_MSIX_ENABLE, 0x1, regVal);
    }
}

/*!
 * @brief Check if access to PCI config space is enabled or not
 *
 * @param[in] pGpu        GPU object pointer
 * @param[in] pKernelBif  Kernel BIF object pointer
 *
 * @return NV_TRUE Pci IO access is enabled
 */
NvBool
kbifIsPciIoAccessEnabled_GB100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32   regVal ;

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_STATUS_COMMAND, &regVal) == NV_OK)
    {
        if (FLD_TEST_DRF(_PF0, _STATUS, _COMMAND_IO_SPACE_ENABLE, _ENABLE,
                         regVal))
        {
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

/*!
 * @brief Check if device is a 3D controller
 *
 * @param[in] pGpu        GPU object pointer
 * @param[in] pKernelBif  Kernel BIF object pointer
 *
 * @return NV_TRUE If device is a 3D controller
 */
NvBool
kbifIs3dController_GB100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32   regVal;

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_REVISION_ID_AND_CLASS_CODE,
                               &regVal) == NV_OK)
    {
        if (FLD_TEST_DRF(_PF0, _REVISION_ID_AND_CLASS_CODE, _BASE_CLASS_CODE, _3D,
                         regVal))
        {
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

/*!
 * @brief Enables extended tag support for GPU.
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 *
 */
void
kbifEnableExtendedTagSupport_GB100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32   regVal;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJCL  *pCl  = SYS_GET_CL(pSys);

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_DEVICE_CAPABILITIES,
                               &regVal) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_PF0_DEVICE_CAPABILITIES\n");
        goto _kbifEnableExtendedTagSupport_GB100_exit;
    }

    regVal = GPU_DRF_VAL(_PF0, _DEVICE_CAPABILITIES,
                         _EXTENDED_TAG_FIELD_SUPPORTED, regVal);
    if ((regVal != 0) &&
        !pCl->getProperty(pCl, PDB_PROP_CL_EXTENDED_TAG_FIELD_NOT_CAPABLE))
    {
        if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_DEVICE_CONTROL_AND_STATUS,
                                   &regVal) != NV_OK)
        {
            NV_PRINTF(LEVEL_INFO,
                      "Unable to read NV_PF0_DEVICE_CONTROL_AND_STATUS\n");
            goto _kbifEnableExtendedTagSupport_GB100_exit;
        }
        regVal = FLD_SET_DRF(_PF0, _DEVICE_CONTROL_AND_STATUS,
                             _EXTENDED_TAG_FIELD_ENABLE, _DEFAULT, regVal);

        if (GPU_BUS_CFG_CYCLE_WR32(pGpu, NV_PF0_DEVICE_CONTROL_AND_STATUS,
                                   regVal) != NV_OK)
        {
            NV_PRINTF(LEVEL_INFO,
                      "Unable to write NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS\n");
            goto _kbifEnableExtendedTagSupport_GB100_exit;
        }
    }

_kbifEnableExtendedTagSupport_GB100_exit:
    return;
}

/*!
 * @brief Enable/disable no snoop for GPU
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 * @param[in]  bEnable     True if No snoop needs to be enabled
 *
 * @return NV_OK If no snoop modified as requested
 */
NV_STATUS
kbifEnableNoSnoop_GB100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvBool     bEnable
)
{
    NvU8  fieldVal;
    NvU32 regVal;
    NvU32 status = NV_OK;

    status = GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_DEVICE_CONTROL_AND_STATUS,
                                    &regVal);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Failed to read NV_PF0_DEVICE_CONTROL_AND_STATUS.\n");
        goto _kbifEnableNoSnoop_GB100_exit;
    }

    fieldVal = bEnable ? 1 : 0;
    regVal   = FLD_SET_DRF_NUM(_PF0, _DEVICE_CONTROL_AND_STATUS,
                               _ENABLE_NO_SNOOP, fieldVal, regVal);
    status   = GPU_BUS_CFG_CYCLE_WR32(pGpu, NV_PF0_DEVICE_CONTROL_AND_STATUS,
                                      regVal);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Failed to write NV_PF0_DEVICE_CONTROL_AND_STATUS.\n");
        goto _kbifEnableNoSnoop_GB100_exit;
    }

_kbifEnableNoSnoop_GB100_exit:
    return status;
}

/*!
 * @brief Enables Relaxed Ordering PCI-E Capability in the PCI Config Space
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 */
void
kbifPcieConfigEnableRelaxedOrdering_GB100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 xveDevCtrlStatus;

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_DEVICE_CONTROL_AND_STATUS,
                               &xveDevCtrlStatus) == NV_ERR_GENERIC)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Unable to read NV_PF0_DEVICE_CONTROL_AND_STATUS!\n");
    }
    else
    {
        GPU_BUS_CFG_CYCLE_FLD_WR_DRF_DEF(pGpu, xveDevCtrlStatus,
                                         _PF0, _DEVICE_CONTROL_AND_STATUS,
                                         _ENABLE_RELAXED_ORDERING, _DEFAULT);
    }
}

/*!
 * @brief Disables Relaxed Ordering PCI-E Capability in the PCI Config Space
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 */
void
kbifPcieConfigDisableRelaxedOrdering_GB100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 xtlDevCtrlStatus;

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_DEVICE_CONTROL_AND_STATUS,
                               &xtlDevCtrlStatus) == NV_ERR_GENERIC)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Unable to read NV_PF0_DEVICE_CONTROL_AND_STATUS!\n");
    }
    else
    {
        xtlDevCtrlStatus = FLD_SET_DRF_NUM(_PF0, _DEVICE_CONTROL_AND_STATUS,
                                           _ENABLE_RELAXED_ORDERING, 0, xtlDevCtrlStatus);
        GPU_BUS_CFG_CYCLE_WR32(pGpu, NV_PF0_DEVICE_CONTROL_AND_STATUS,
                               xtlDevCtrlStatus);
    }
}

/*!
 * @brief  Get the status of XTL
 *         (Function name ideally should be bifGetXtlStatusBits)
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  BIF object pointer
 * @param[out] pBits       PCIE error status values
 * @param[out] pStatus     Full XTL status
 *
 * @return NV_OK
 */
NV_STATUS
kbifGetXveStatusBits_GB100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32     *pBits,
    NvU32     *pStatus
)
{
    // control/status reg 0x68
    NvU32 xtlDevCtrlStatus;

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_DEVICE_CONTROL_AND_STATUS,
                               &xtlDevCtrlStatus) != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Unable to read NV_PF0_DEVICE_CONTROL_AND_STATUS!\n");
    }
    if (pBits == NULL)
        return NV_ERR_GENERIC;

    *pBits = 0;

    // The register read above returns garbage on fmodel, so just return.
    if (IS_FMODEL(pGpu))
    {
        if (pStatus)
        {
            *pStatus = 0;
        }
        return NV_OK;
    }

    // Optionally return full status
    if (pStatus)
    {
        *pStatus = xtlDevCtrlStatus;
    }

    if (xtlDevCtrlStatus & DRF_NUM(_PF0, _DEVICE_CONTROL_AND_STATUS,
                                   _CORRECTABLE_ERROR_DETECTED, 1))
    {
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_CORR_ERROR;
    }
    if (xtlDevCtrlStatus & DRF_NUM(_PF0, _DEVICE_CONTROL_AND_STATUS,
                                   _NON_FATAL_ERROR_DETECTED, 1))
    {
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_NON_FATAL_ERROR;
    }
    if (xtlDevCtrlStatus & DRF_NUM(_PF0, _DEVICE_CONTROL_AND_STATUS,
                                   _FATAL_ERROR_DETECTED, 1))
    {
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_FATAL_ERROR;
    }
    if (xtlDevCtrlStatus & DRF_NUM(_PF0, _DEVICE_CONTROL_AND_STATUS,
                                   _UNSUPPORTED_REQUEST_DETECTED, 1))
    {
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_UNSUPP_REQUEST;
    }

    if (pKernelBif->EnteredRecoverySinceErrorsLastChecked)
    {
        pKernelBif->EnteredRecoverySinceErrorsLastChecked = NV_FALSE;
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_ENTERED_RECOVERY;
    }

    return NV_OK;
}

/*!
 * @brief Clear XVE status
 *        (Function name ideally should be bifClearXtlStatus)
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  BIF object pointer
 * @param[out] pStatus     Full XTL status
 *
 * @return NV_OK
 */
NV_STATUS
kbifClearXveStatus_GB100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32     *pStatus
)
{
    NvU32 xtlDevCtrlStatus;

    if (pStatus)
    {
        xtlDevCtrlStatus = *pStatus;
        if (xtlDevCtrlStatus == 0)
        {
            return NV_OK;
        }
    }
    else
    {
        if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_DEVICE_CONTROL_AND_STATUS,
                                   &xtlDevCtrlStatus) != NV_OK)
        {
            NV_PRINTF(LEVEL_INFO,
                      "Unable to read NV_PF0_DEVICE_CONTROL_AND_STATUS!\n");
        }
    }

    GPU_BUS_CFG_CYCLE_WR32(pGpu, NV_PF0_DEVICE_CONTROL_AND_STATUS,
                           xtlDevCtrlStatus);

    return NV_OK;
}

/*!
 * @brief Get XTL AER bits
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  BIF object pointer
 * @param[out] pBits       XTL AER bits value
 *
 * @return NV_OK
 */
NV_STATUS
kbifGetXveAerBits_GB100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32     *pBits
)
{
    NvU32 xtlAerUncorr;
    NvU32 xtlAerCorr;

    if (pBits == NULL)
        return NV_ERR_GENERIC;

    // The register read below returns garbage on fmodel, so just return.
    if (IS_FMODEL(pGpu))
    {
        return NV_OK;
    }

    *pBits = 0;

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_UNCORRECTABLE_ERROR_STATUS,
                               &xtlAerUncorr) != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Unable to read NV_PF0_UNCORRECTABLE_ERROR_STATUS\n");
        return NV_ERR_GENERIC;
    }
    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_CORRECTABLE_ERROR_STATUS,
                               &xtlAerCorr) != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Unable to read NV_PF0_CORRECTABLE_ERROR_STATUS\n");
        return NV_ERR_GENERIC;
    }

    if (FLD_TEST_DRF_NUM(_PF0, _UNCORRECTABLE_ERROR_STATUS,
                         _DATA_LINK_PROTOCOL_ERROR_STATUS, 0x1, xtlAerUncorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_DLINK_PROTO_ERR;
    if (FLD_TEST_DRF_NUM(_PF0, _UNCORRECTABLE_ERROR_STATUS,
                         _POISONED_TLP_RECEIVED, 0x1, xtlAerUncorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_POISONED_TLP;
    if (FLD_TEST_DRF_NUM(_PF0, _UNCORRECTABLE_ERROR_STATUS,
                         _COMPLETION_TIMEOUT_STATUS, 0x1, xtlAerUncorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_CPL_TIMEOUT;
    if (FLD_TEST_DRF_NUM(_PF0, _UNCORRECTABLE_ERROR_STATUS,
                         _UNEXPECTED_COMPLETION_STATUS, 0x1, xtlAerUncorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_UNEXP_CPL;
    if (FLD_TEST_DRF_NUM(_PF0, _UNCORRECTABLE_ERROR_STATUS,
                         _MALFORMED_TLP_STATUS, 0x1, xtlAerUncorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_MALFORMED_TLP;
    if (FLD_TEST_DRF_NUM(_PF0, _UNCORRECTABLE_ERROR_STATUS,
                         _UNSUPPORTED_REQUEST_ERROR_STATUS, 0x1, xtlAerUncorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_UNSUPPORTED_REQ;

    // FERMI-TODO

    if (FLD_TEST_DRF_NUM(_PF0, _CORRECTABLE_ERROR_STATUS,
                         _RECEIVER_ERROR_STATUS, 0x1, xtlAerCorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RCV_ERR;
    if (FLD_TEST_DRF_NUM(_PF0, _CORRECTABLE_ERROR_STATUS,
                         _BAD_TLP_STATUS, 0x1, xtlAerCorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_BAD_TLP;
    if (FLD_TEST_DRF_NUM(_PF0, _CORRECTABLE_ERROR_STATUS,
                         _BAD_DLLP_STATUS, 0x1, xtlAerCorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_BAD_DLLP;
    if (FLD_TEST_DRF_NUM(_PF0, _CORRECTABLE_ERROR_STATUS,
                         _REPLAY_NUM_ROLLOVER_STATUS, 0x1, xtlAerCorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RPLY_ROLLOVER;
    if (FLD_TEST_DRF_NUM(_PF0, _CORRECTABLE_ERROR_STATUS,
                         _REPLAY_TIMER_TIMEOUT_STATUS, 0x1, xtlAerCorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RPLY_TIMEOUT;
    if (FLD_TEST_DRF_NUM(_PF0, _CORRECTABLE_ERROR_STATUS,
                         _ADVISORY_NON_FATAL_ERROR_STATUS, 0x1, xtlAerCorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_ADVISORY_NONFATAL;

    return NV_OK;
}

/*!
 * @brief Clear XTL AER bits
 *
 * @param[in] pGpu        GPU object pointer
 * @param[in] pKernelBif  BIF object pointer
 * @param[in] bits        XTL AER bits to be cleared
 *
 * @return NV_OK
 */
NV_STATUS
kbifClearXveAer_GB100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32      bits
)
{
    NvU32 xtlAerUncorr = 0;
    NvU32 xtlAerCorr   = 0;

    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_DLINK_PROTO_ERR)
        xtlAerUncorr = FLD_SET_DRF_NUM(_PF0, _UNCORRECTABLE_ERROR_STATUS,
                                       _DATA_LINK_PROTOCOL_ERROR_STATUS, 0x1,
                                       xtlAerUncorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_POISONED_TLP)
        xtlAerUncorr = FLD_SET_DRF_NUM(_PF0, _UNCORRECTABLE_ERROR_STATUS,
                                       _POISONED_TLP_RECEIVED, 0x1,
                                       xtlAerUncorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_CPL_TIMEOUT)
        xtlAerUncorr = FLD_SET_DRF_NUM(_PF0, _UNCORRECTABLE_ERROR_STATUS,
                                       _COMPLETION_TIMEOUT_STATUS, 0x1,
                                       xtlAerUncorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_UNEXP_CPL)
        xtlAerUncorr = FLD_SET_DRF_NUM(_PF0, _UNCORRECTABLE_ERROR_STATUS,
                                       _UNEXPECTED_COMPLETION_STATUS, 0x1,
                                       xtlAerUncorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_MALFORMED_TLP)
        xtlAerUncorr = FLD_SET_DRF_NUM(_PF0, _UNCORRECTABLE_ERROR_STATUS,
                                       _MALFORMED_TLP_STATUS, 0x1,
                                       xtlAerUncorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_UNSUPPORTED_REQ)
        xtlAerUncorr = FLD_SET_DRF_NUM(_PF0, _UNCORRECTABLE_ERROR_STATUS,
                                       _UNSUPPORTED_REQUEST_ERROR_STATUS, 0x1,
                                       xtlAerUncorr);

    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RCV_ERR)
        xtlAerCorr = FLD_SET_DRF_NUM(_PF0, _CORRECTABLE_ERROR_STATUS,
                                     _RECEIVER_ERROR_STATUS, 0x1,
                                     xtlAerCorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_BAD_TLP)
        xtlAerCorr = FLD_SET_DRF_NUM(_PF0, _CORRECTABLE_ERROR_STATUS,
                                     _BAD_TLP_STATUS, 0x1,
                                     xtlAerCorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_BAD_DLLP)
        xtlAerCorr = FLD_SET_DRF_NUM(_PF0, _CORRECTABLE_ERROR_STATUS,
                                     _BAD_DLLP_STATUS, 0x1,
                                     xtlAerCorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RPLY_ROLLOVER)
        xtlAerCorr = FLD_SET_DRF_NUM(_PF0, _CORRECTABLE_ERROR_STATUS,
                                     _REPLAY_NUM_ROLLOVER_STATUS, 0x1,
                                     xtlAerCorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RPLY_TIMEOUT)
        xtlAerCorr = FLD_SET_DRF_NUM(_PF0, _CORRECTABLE_ERROR_STATUS,
                                     _REPLAY_TIMER_TIMEOUT_STATUS, 0x1,
                                     xtlAerCorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_ADVISORY_NONFATAL)
        xtlAerCorr = FLD_SET_DRF_NUM(_PF0, _CORRECTABLE_ERROR_STATUS,
                                     _ADVISORY_NON_FATAL_ERROR_STATUS, 0x1,
                                     xtlAerCorr);

    if (xtlAerUncorr != 0)
    {
        GPU_BUS_CFG_CYCLE_WR32(pGpu, NV_PF0_UNCORRECTABLE_ERROR_STATUS,
                               xtlAerUncorr);
    }
    if (xtlAerCorr != 0)
    {
        GPU_BUS_CFG_CYCLE_WR32(pGpu, NV_PF0_CORRECTABLE_ERROR_STATUS,
                               xtlAerCorr);
    }

    return NV_OK;
}

/*!
 * @brief Get bus options: link specific capabilities or
 * pcie device specific parameters or pcie link specific parameters
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  BIF object pointer
 * @param[in]  options     XTL bus options
 * @param[out] pAddrReg    Address of the register for the given bus option
 *
 * @return  NV_OK on success
 *          NV_ERR_GENERIC
 */
NV_STATUS
kbifGetBusOptionsAddr_GB100
(
    OBJGPU     *pGpu,
    KernelBif  *pKernelBif,
    BUS_OPTIONS options,
    NvU32      *pAddrReg
)
{
    NV_STATUS  status = NV_OK;

    switch (options)
    {
        case BUS_OPTIONS_DEV_CONTROL_STATUS:
            *pAddrReg = NV_PF0_DEVICE_CONTROL_AND_STATUS;
            break;
        case BUS_OPTIONS_DEV_CONTROL_STATUS_2:
            *pAddrReg = NV_PF0_DEVICE_CONTROL_2;
            break;
        case BUS_OPTIONS_LINK_CONTROL_STATUS:
            *pAddrReg = NV_PF0_LINK_CONTROL_AND_STATUS;
            break;
        case BUS_OPTIONS_LINK_CAPABILITIES:
            *pAddrReg = NV_PF0_LINK_CAPABILITIES;
            break;
        case BUS_OPTIONS_L1_PM_SUBSTATES_CTRL_1:
            *pAddrReg = NV_PF0_L1_PM_SUBSTATES_CONTROL_1;
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "Invalid register type passed 0x%x\n",
                      options);
            status = NV_ERR_GENERIC;
            break;
    }
    return status;
}

/*!
 * @brief     Enable PCIe atomics if PCIe hierarchy supports it
 *
 * @param[in] pGpu       GPU object pointer
 * @param[in] pKernelBif Kernel BIF object pointer
 */
void
kbifEnablePcieAtomics_GB100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 regVal;

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_DEVICE_CONTROL_2, &regVal) != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "Read of NV_PF0_DEVICE_CONTROL_2 failed.\n");
        return;
    }

    regVal = FLD_SET_DRF_NUM(_PF0, _DEVICE_CONTROL_2, _ATOMICOP_REQUESTER_ENABLE, 0x1, regVal);

    if (GPU_BUS_CFG_CYCLE_WR32(pGpu, NV_PF0_DEVICE_CONTROL_2, regVal) != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "Write of NV_PF0_DEVICE_CONTROL_2 failed.\n");
        return;
    }

    NV_PRINTF(LEVEL_INFO, "PCIe Requester atomics enabled.\n");
}

/*!
 * @brief Check and cache Function level reset support
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 *
 */
void
kbifCacheFlrSupport_GB100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 regVal = 0;

    // Read config register
    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_DEVICE_CAPABILITIES,
                               &regVal) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_PF0_DEVICE_CAPABILITIES\n");
        return;
    }

    // Check if FLR is supported
    if (FLD_TEST_DRF(_PF0, _DEVICE_CAPABILITIES, _FUNCTION_LEVEL_RESET_CAPABILITY,
                     _DEFAULT, regVal))
    {
        pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_FLR_SUPPORTED, NV_TRUE);
    }
}

/*!
 * @brief Check and cache 64b BAR0 support
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 *
 */
void
kbifCache64bBar0Support_GB100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 regVal = 0;

    // Read config register
    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_BASE_ADDRESS_REGISTERS_0,
                               &regVal) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_PF0_BASE_ADDRESS_REGISTERS_0\n");
        return;
    }

    // Check if 64b BAR0 is supported
    if (FLD_TEST_DRF(_PF0_BASE_ADDRESS, _REGISTERS_0, _ADDR_TYPE,
                     _64BIT, regVal))
    {
        pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_64BIT_BAR0_SUPPORTED, NV_TRUE);
    }
}

/*!
 * @brief Check and cache MNOC interface support
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 *
 */
void
kbifCacheMnocSupport_GB100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 regVal = 0;

    pKernelBif->bMnocAvailable = NV_FALSE;

    if (IS_PASSTHRU(pGpu))
    {
        //
        // Long story short, when in recovery mode, RM will never come into picture
        // Conversely, if RM is up, that means we have the valid boot ROM image
        // and thus MNOC capability will always be available
        //
        pKernelBif->bMnocAvailable = NV_TRUE;
        return;
    }

    // Read vendorID from DVSEC
    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_DESIGNATED_VENDOR_SPECIFIC_0_HEADER_1,
                               &regVal) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_PF0_DESIGNATED_VENDOR_SPECIFIC_0_HEADER_1\n");
        return;
    }

    if (!FLD_TEST_DRF(_PF0_DESIGNATED, _VENDOR_SPECIFIC_0_HEADER_1,
                     _NV_DVSEC0_VENDOR_ID, _DEFAULT, regVal))
    {
        // Return early if this is not GPU (for eg., it could be CXL)
        return;
    }

    // Read config register for MNOC support
    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_DESIGNATED_VENDOR_SPECIFIC_0_HEADER_2_AND_GENERAL,
                               &regVal) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_PF0_DESIGNATED_VENDOR_SPECIFIC_0_HEADER_2_AND_GENERAL\n");
        return;
    }

    // Return early if we are not in normal mode
    if (!(FLD_TEST_DRF(_PF0_DESIGNATED, _VENDOR_SPECIFIC_0_HEADER_2_AND_GENERAL,
                       _RECOVERY_INDICATION, _DEFAULT, regVal)))
    {
        return;
    }

    // Check if MNOC is supported
    if (FLD_TEST_DRF(_PF0_DESIGNATED, _VENDOR_SPECIFIC_0_HEADER_2_AND_GENERAL,
                     _MNOC_INTERFACE_AVAILABLE, _DEFAULT, regVal))
    {
        pKernelBif->bMnocAvailable = NV_TRUE;
    }
}

/*!
 * @brief: Get BAR information from PCIe config space
 *
 * @param[in]  pGpu               OBJGPU pointer
 * @param[in]  barRegCSBase       The base register 0 address
 * @param[in]  barIndex           The BAR index to check
 * @param[out] pBarBaseAddress    The start address of the specified BAR
 * @param[out] pIs64BitBar        To indicate if the BAR is using 64bit address
 *
 * @returns NV_STATUS
 */
static NV_STATUS
_kbifGetBarInfo_GB100
(
    OBJGPU *pGpu,
    NvU32   barRegCSBase,
    NvU32   barIndex,
    NvU64  *pBarBaseAddress,
    NvBool *pIs64BitBar
)
{
    NV_STATUS status         = NV_OK;
    NvBool    barIs64Bit     = NV_FALSE;
    NvU32     barAddrLow     = 0;
    NvU32     barAddrHigh    = 0;
    NvU32     barRegCSLimit  = barRegCSBase + NV_PF0_BASE_ADDRESS_REGISTERS_5 - NV_PF0_BASE_ADDRESS_REGISTERS_0;
    NvU32     barRegCSOffset = barRegCSBase;
    NvU64     barBaseAddr    = 0;
    NvU32     i              = 0;

    for (i = 0; i <= barIndex; i++)
    {
        if ((status = GPU_BUS_CFG_CYCLE_RD32(pGpu, barRegCSOffset, &barAddrLow)) != NV_OK)
        {
            return status;
        }

        //
        // The SPACE_TYPE, ADDRESS_TYPE, PREFETCHABLE and BASE_ADDRESS fields
        // have the same definition as for Base Address Register 0
        //
        barIs64Bit = FLD_TEST_DRF(_PF0_BASE_ADDRESS, _REGISTERS_0, _ADDR_TYPE, _64BIT, barAddrLow);

        if (i != barIndex)
        {
            barRegCSOffset += (barIs64Bit ? 8 : 4);

            if (barRegCSOffset >= barRegCSLimit)
            {
                return NV_ERR_INVALID_INDEX;
            }
        }
    }

    if (pBarBaseAddress != NULL)
    {
        // Get the BAR address
        barBaseAddr = barAddrLow & 0xFFFFFFF0;
        if (barIs64Bit)
        {
            // Read and save the bar high address
            status = GPU_BUS_CFG_CYCLE_RD32(pGpu, barRegCSOffset + 4, &barAddrHigh);
            NV_ASSERT_OR_RETURN((status == NV_OK), status);

            barBaseAddr |= (NvU64)barAddrHigh << 32;
        }

        *pBarBaseAddress = barBaseAddr;
    }

    if (pIs64BitBar != NULL)
    {
        *pIs64BitBar = barIs64Bit;
    }

    return NV_OK;
}

/*! @brief Fetch VF details such as no. of VFs, First VF offset etc
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
*/
void
kbifCacheVFInfo_GB100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NV_STATUS status     = NV_OK;
    NvU32     regVal     = 0;
    NvU64     barAddr    = 0;
    NvBool    barIs64Bit = NV_FALSE;

    // Get total VF count
    status = GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_INITIAL_AND_TOTAL_VFS, &regVal);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_PF0_INITIAL_AND_TOTAL_VFS\n");
        return;
    }
    pGpu->sriovState.totalVFs = GPU_DRF_VAL(_PF0, _INITIAL_AND_TOTAL_VFS,
                                            _TOTAL_VFS, regVal);

    // Get first VF offset
    status = GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_VF_STRIDE_AND_OFFSET, &regVal);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_PF0_VF_STRIDE_AND_OFFSET\n");
        return;
    }
    pGpu->sriovState.firstVFOffset = GPU_DRF_VAL(_PF0, _VF_STRIDE_AND_OFFSET,
                                                 _FIRST_VF_OFFSET, regVal);

    // Get VF BAR0 info
    status = _kbifGetBarInfo_GB100(pGpu, NV_PF0_VF_BAR_0, 0, &barAddr, &barIs64Bit);
    NV_ASSERT(status == NV_OK);

    pGpu->sriovState.firstVFBarAddress[0] = barAddr;
    pGpu->sriovState.b64bitVFBar0         = barIs64Bit;

    // Get VF BAR1 info
    status = _kbifGetBarInfo_GB100(pGpu, NV_PF0_VF_BAR_0, 1, &barAddr, &barIs64Bit);
    NV_ASSERT(status == NV_OK);

    pGpu->sriovState.firstVFBarAddress[1] = barAddr;
    pGpu->sriovState.b64bitVFBar1         = barIs64Bit;

    // Get VF BAR2 info
    status = _kbifGetBarInfo_GB100(pGpu, NV_PF0_VF_BAR_0, 2, &barAddr, &barIs64Bit);
    NV_ASSERT(status == NV_OK);

    pGpu->sriovState.firstVFBarAddress[2] = barAddr;
    pGpu->sriovState.b64bitVFBar2         = barIs64Bit;
}

/*!
 * @brief Clears Bus Master Enable bit in command register, disabling
 *  Function 0 - from issuing any new requests to sysmem.
 *
 * @param[in] pGpu        GPU object pointer
 * @param[in] pKernelBif  KernelBif object pointer
 *
 * @return NV_OK
 */
NV_STATUS
kbifStopSysMemRequests_GB100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvBool     bStop
)
{
    NvU32 regVal;

    NV_ASSERT_OK_OR_RETURN(GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_STATUS_COMMAND,
                                                  &regVal));
    if (bStop)
    {
        regVal = FLD_SET_DRF(_PF0, _STATUS, _COMMAND_BUS_MASTER_ENABLE, _DISABLE,
                             regVal);
    }
    else
    {
        regVal = FLD_SET_DRF(_PF0, _STATUS, _COMMAND_BUS_MASTER_ENABLE, _ENABLE,
                             regVal);
    }

    NV_ASSERT_OK_OR_RETURN(GPU_BUS_CFG_CYCLE_WR32(pGpu, NV_PF0_STATUS_COMMAND,
                                                  regVal));

    return NV_OK;
}

/*!
 * @brief Trigger FLR.
 *
 * @param[in] pGpu        GPU object pointer
 * @param[in] pKernelBif  KernelBif object pointer
 *
 * @return  NV_OK if successful.
 */
NV_STATUS
kbifTriggerFlr_GB100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32     regVal = 0;
    NV_STATUS status = NV_OK;

    status = GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_DEVICE_CONTROL_AND_STATUS,
                                    &regVal);
    if (status != NV_OK)
    {
        NV_ASSERT_FAILED("Config space read of device control failed\n");
        return status;
    }

    regVal = FLD_SET_DRF_NUM(_PF0, _DEVICE_CONTROL_AND_STATUS ,
                             _BRIDGE_CONFIGURATION_RETRY_ENABLE_INITIATE_FUNCTION_LEVEL_RESET,
                             0x1, regVal);

    status = GPU_BUS_CFG_CYCLE_WR32(pGpu, NV_PF0_DEVICE_CONTROL_AND_STATUS,
                                    regVal);
    if (status != NV_OK)
    {
        NV_ASSERT_FAILED("FLR trigger failed\n");
        return status;
    }

    return status;
}

/*!
 * @brief Wait for to get config access.
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  KernelBif object pointer
 * @param[in]  pTimeout    Value in microseconds to dev init complete
 *
 * @return  NV_OK if successful.
 */
NV_STATUS
kbifConfigAccessWait_GB100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    RMTIMEOUT *pTimeout
)
{
    NvU32     regVal = 0;
    NV_STATUS status = NV_OK;

    while (NV_TRUE)
    {
        //
        // This read only register should be accessible over config cycle
        // once devinit is complete without RM having to restore config space.
        // This register is not really reset on FLR.
        //
        if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_DEVICE_VENDOR_ID, &regVal) == NV_OK)
        {
            if (FLD_TEST_DRF(_PF0, _DEVICE_VENDOR_ID, _VENDOR_ID, _DEFAULT, regVal))
            {
                break;
            }
        }
        status = gpuCheckTimeout(pGpu, pTimeout);
        if (status == NV_ERR_TIMEOUT)
        {
            NV_ASSERT_FAILED("Timed out waiting for devinit to complete\n");
            return status;
        }

        //
        // Put ourself into wait state for 1ms. This function runs in the context
        // of DxgkDdiResetFromTimeout which is at PASSIVE_LEVEL(lower priority)
        // which means it can be in wait state for longer delays of the order of
        // milliseconds
        //
        osDelay(1);
    }

    return status;
}

/*!
 * @brief Returns size of MSIX vector control table
 *
 * @param[in] pGpu        GPU object pointer
 * @param[in] pKernelBif  KernelBif object pointer
 */
NvU32
kbifGetMSIXTableVectorControlSize_GB100
(
    OBJGPU *pGpu,
    KernelBif *pKernelBif
)
{
    return NV_VIRTUAL_FUNCTION_PRIV_MSIX_TABLE_VECTOR_CONTROL__SIZE_1;
}

/*!
 * @brief Caches the value of the config space devcap2 register.
 *        The value is forwarded to GSP.
 *
 * @param[in] pGpu        GPU object pointer
 * @param[in] pKernelBif  KernelBif object pointer
 */
void kbifProbePcieCplAtomicCaps_GB100(OBJGPU *pGpu, KernelBif *pKernelBif)
{
    kbifReadPcieCplCapsFromConfigSpace_HAL(pGpu, pKernelBif, &pKernelBif->pcieAtomicsCplDeviceCapMask);
}

/*
 * @brief Reads the device caps register and returns an NvU32 mask describing
 *        the types of completer atomics that the device exposes.
 *        The mask is decoded using BIF_PCIE_CPL_ATOMICS_* defines.
 *
 * @param[in]  pGpu        OBJGPU object pointer
 * @param[in]  pKernelBif  KernelBif object pointer
 * @param[out] pBifAtomicsmask NvU32* Parameter containing the mask describing
 *             support for completing atomic operations.
 *
 * @return void
 */
void
kbifReadPcieCplCapsFromConfigSpace_GB100
(
    OBJGPU *pGpu,
    KernelBif *pKernelBif,
    NvU32  *pBifAtomicsmask
)
{
    NvU32 deviceCaps2;

    NV_ASSERT_OR_RETURN_VOID(pBifAtomicsmask != NULL);

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_DEVICE_CAPABILITIES_2, &deviceCaps2) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_PF0_DEVICE_CAPABILITIES_2\n");
        return;
    }

    *pBifAtomicsmask = 0;

    if (DRF_VAL(_PF0, _DEVICE_CAPABILITIES_2, _32_BIT_ATOMICOP_COMPLETER_SUPPORTED, deviceCaps2) == NV_TRUE)
    {
        *pBifAtomicsmask |= BIF_PCIE_CPL_ATOMICS_FETCHADD_32;
        *pBifAtomicsmask |= BIF_PCIE_CPL_ATOMICS_SWAP_32;
        *pBifAtomicsmask |= BIF_PCIE_CPL_ATOMICS_CAS_32;
    }
    if (DRF_VAL(_PF0, _DEVICE_CAPABILITIES_2, _64_BIT_ATOMICOP_COMPLETER_SUPPORTED, deviceCaps2) == NV_TRUE)
    {
        *pBifAtomicsmask |= BIF_PCIE_CPL_ATOMICS_FETCHADD_64;
        *pBifAtomicsmask |= BIF_PCIE_CPL_ATOMICS_SWAP_64;
        *pBifAtomicsmask |= BIF_PCIE_CPL_ATOMICS_CAS_64;
    }
    if (DRF_VAL(_PF0, _DEVICE_CAPABILITIES_2, _128_BIT_CAS_COMPLETER_SUPPORTED, deviceCaps2) == NV_TRUE)
    {
        *pBifAtomicsmask |= BIF_PCIE_CPL_ATOMICS_CAS_128;
    }
}
