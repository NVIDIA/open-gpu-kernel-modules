/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/fsp/kern_fsp.h"
#include "platform/chipset/chipset.h"
#include "ctrl/ctrl2080/ctrl2080bus.h"

#include "published/hopper/gh100/dev_fb.h"
#include "published/hopper/gh100/dev_xtl_ep_pri.h"
#include "published/hopper/gh100/dev_xtl_ep_pcfg_gpu.h"
#include "published/hopper/gh100/hwproject.h"
#include "published/hopper/gh100/dev_pmc.h"
#include "published/hopper/gh100/dev_vm.h"

#include "os/os.h"

/*!
 * @brief Check if MSI is enabled in HW
 *
 * @param[in] pGpu        GPU object pointer
 * @param[in] pKernelBif  Kernel BIF object pointer
 *
 * @return  True if MSI enabled else False
 */
NvBool
kbifIsMSIEnabledInHW_GH100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 data32;

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_MSI_64_HEADER,
                             &data32) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "unable to read NV_EP_PCFG_GPU_MSI_64_HEADER\n");
    }

    return FLD_TEST_DRF_NUM(_EP_PCFG_GPU, _MSI_64_HEADER,
                            _MSI_ENABLE, 0x1, data32);
}

/*!
 * @brief Check if MSIX is enabled in HW
 *
 * @param[in] pGpu        GPU object pointer
 * @param[in] pKernelBif  Kernel BIF object pointer
 *
 * @return  True if MSIX enabled else False
 */
NvBool
kbifIsMSIXEnabledInHW_GH100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 data32;

    if (IS_VIRTUAL(pGpu))
    {
        // SR-IOV guests only support MSI-X
        return IS_VIRTUAL_WITH_SRIOV(pGpu);
    }
    else
    {
        if (GPU_BUS_CFG_CYCLE_RD32(pGpu,
                NV_EP_PCFG_GPU_MSIX_CAP_HEADER, &data32) != NV_OK)
        {
            NV_ASSERT_FAILED("Unable to read NV_EP_PCFG_GPU_MSIX_CAP_HEADER\n");
            return NV_FALSE;
        }
        return FLD_TEST_DRF(_EP_PCFG_GPU, _MSIX_CAP_HEADER, _ENABLE,
                            _ENABLED, data32);
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
kbifIsPciIoAccessEnabled_GH100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32   data = 0;

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu,
                             NV_EP_PCFG_GPU_CTRL_CMD_AND_STATUS, &data) == NV_OK)
    {
        if (FLD_TEST_DRF(_EP_PCFG_GPU, _CTRL_CMD_AND_STATUS, _CMD_IO_SPACE, _ENABLE, data))
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
kbifIs3dController_GH100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32   data = 0;

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu,
                             NV_EP_PCFG_GPU_REVISION_ID_AND_CLASSCODE, &data) == NV_OK)
    {
        if (FLD_TEST_DRF(_EP_PCFG_GPU, _REVISION_ID_AND_CLASSCODE, _BASE_CLASSCODE, _3D, data))
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
kbifEnableExtendedTagSupport_GH100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32   reg;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJCL  *pCl  = SYS_GET_CL(pSys);

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_DEVICE_CAPABILITIES,
                             &reg) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unable to read NV_EP_PCFG_GPU_DEVICE_CAPABILITIES\n");
        goto _kbifEnableExtendedTagSupport_GH100_exit;
    }

    reg = GPU_DRF_VAL(_EP_PCFG_GPU, _DEVICE_CAPABILITIES,
                      _EXTENDED_TAG_FIELD_SUPPORTED, reg);
    if ((reg != 0) &&
        !pCl->getProperty(pCl, PDB_PROP_CL_EXTENDED_TAG_FIELD_NOT_CAPABLE))
    {
        if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS,
                                 &reg) != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Unable to read NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS\n");
            goto _kbifEnableExtendedTagSupport_GH100_exit;
        }
        reg = FLD_SET_DRF(_EP_PCFG_GPU, _DEVICE_CONTROL_STATUS,
                          _EXTENDED_TAG_FIELD_ENABLE, _INIT, reg);

        if (GPU_BUS_CFG_CYCLE_WR32(pGpu, NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS,
                                 reg) != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Unable to write NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS\n");
            goto _kbifEnableExtendedTagSupport_GH100_exit;
        }
    }

_kbifEnableExtendedTagSupport_GH100_exit:
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
kbifEnableNoSnoop_GH100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvBool     bEnable
)
{
    NvU8  fieldVal;
    NvU32 regVal;
    NvU32 status = NV_OK;

    status = GPU_BUS_CFG_CYCLE_RD32(pGpu,
                NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS, &regVal);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
            "Failed to read NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS.\n");
        goto _kbifEnableNoSnoop_GH100_exit;
    }

    fieldVal = bEnable ? 1 : 0;
    regVal   = FLD_SET_DRF_NUM(_EP_PCFG_GPU, _DEVICE_CONTROL_STATUS,
                               _ENABLE_NO_SNOOP, fieldVal, regVal);
    status   = GPU_BUS_CFG_CYCLE_WR32(pGpu,
                                    NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS, regVal);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
            "Failed to write NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS.\n");
        goto _kbifEnableNoSnoop_GH100_exit;
    }

_kbifEnableNoSnoop_GH100_exit:
    return status;
}

/*!
 * @brief Enables Relaxed Ordering PCI-E Capability in the PCI Config Space
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 */
void
kbifPcieConfigEnableRelaxedOrdering_GH100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 xveDevCtrlStatus;

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu,
                             NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS,
                             &xveDevCtrlStatus) == NV_ERR_GENERIC)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unable to read NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS!\n");
    }
    else
    {
        GPU_BUS_CFG_CYCLE_FLD_WR_DRF_DEF(pGpu, xveDevCtrlStatus, _EP_PCFG_GPU,
                                   _DEVICE_CONTROL_STATUS, _ENABLE_RELAXED_ORDERING, _INIT);
    }
}

/*!
 * @brief Disables Relaxed Ordering PCI-E Capability in the PCI Config Space
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 */
void
kbifPcieConfigDisableRelaxedOrdering_GH100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 xtlDevCtrlStatus;

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS,
                             &xtlDevCtrlStatus) == NV_ERR_GENERIC)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unable to read NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS!\n");
    }
    else
    {
        xtlDevCtrlStatus = FLD_SET_DRF_NUM(_EP_PCFG_GPU, _DEVICE_CONTROL_STATUS,
                                           _ENABLE_RELAXED_ORDERING, 0, xtlDevCtrlStatus);
        GPU_BUS_CFG_CYCLE_WR32(pGpu, NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS,
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
kbifGetXveStatusBits_GH100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32     *pBits,
    NvU32     *pStatus
)
{
    // control/status reg 0x68
    NvU32 xtlDevCtrlStatus;

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu,
                             NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS,
                             &xtlDevCtrlStatus) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unable to read NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS!\n");
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

    if (xtlDevCtrlStatus & DRF_NUM(_EP_PCFG_GPU, _DEVICE_CONTROL_STATUS,
                                   _CORR_ERROR_DETECTED, 1))
    {
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_CORR_ERROR;
    }
    if (xtlDevCtrlStatus & DRF_NUM(_EP_PCFG_GPU, _DEVICE_CONTROL_STATUS,
                                   _NON_FATAL_ERROR_DETECTED, 1))
    {
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_NON_FATAL_ERROR;
    }
    if (xtlDevCtrlStatus & DRF_NUM(_EP_PCFG_GPU, _DEVICE_CONTROL_STATUS,
                                   _FATAL_ERROR_DETECTED, 1))
    {
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_FATAL_ERROR;
    }
    if (xtlDevCtrlStatus & DRF_NUM(_EP_PCFG_GPU, _DEVICE_CONTROL_STATUS,
                                   _UNSUPP_REQUEST_DETECTED, 1))
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
kbifClearXveStatus_GH100
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
        if (GPU_BUS_CFG_CYCLE_RD32(pGpu,
                NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS, &xtlDevCtrlStatus) != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Unable to read NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS!\n");
        }
    }

    GPU_BUS_CFG_CYCLE_WR32(pGpu, NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS, xtlDevCtrlStatus);

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
kbifGetXveAerBits_GH100
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

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_UNCORRECTABLE_ERROR_STATUS,
                             &xtlAerUncorr) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_EP_PCFG_GPU_UNCORRECTABLE_ERROR_STATUS\n");
        return NV_ERR_GENERIC;
    }
    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_CORRECTABLE_ERROR_STATUS,
                             &xtlAerCorr) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_EP_PCFG_GPU_CORRECTABLE_ERROR_STATUS\n");
        return NV_ERR_GENERIC;
    }

    if (FLD_TEST_DRF_NUM(_EP_PCFG_GPU, _UNCORRECTABLE_ERROR_STATUS, _DL_PROTOCOL_ERROR, 0x1, xtlAerUncorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_DLINK_PROTO_ERR;
    if (FLD_TEST_DRF_NUM(_EP_PCFG_GPU, _UNCORRECTABLE_ERROR_STATUS, _POISONED_TLP_RCVD, 0x1, xtlAerUncorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_POISONED_TLP;
    if (FLD_TEST_DRF_NUM(_EP_PCFG_GPU, _UNCORRECTABLE_ERROR_STATUS, _COMPLETION_TIMEOUT, 0x1, xtlAerUncorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_CPL_TIMEOUT;
    if (FLD_TEST_DRF_NUM(_EP_PCFG_GPU, _UNCORRECTABLE_ERROR_STATUS, _UNEXPECTED_COMPLETION, 0x1, xtlAerUncorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_UNEXP_CPL;
    if (FLD_TEST_DRF_NUM(_EP_PCFG_GPU, _UNCORRECTABLE_ERROR_STATUS, _MALFORMED_TLP, 0x1, xtlAerUncorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_MALFORMED_TLP;
    if (FLD_TEST_DRF_NUM(_EP_PCFG_GPU, _UNCORRECTABLE_ERROR_STATUS, _UNSUPPORTED_REQUEST_ERROR, 0x1, xtlAerUncorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_UNSUPPORTED_REQ;

    // FERMI-TODO

    if (FLD_TEST_DRF_NUM(_EP_PCFG_GPU, _CORRECTABLE_ERROR_STATUS, _RECEIVER_ERROR, 0x1, xtlAerCorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RCV_ERR;
    if (FLD_TEST_DRF_NUM(_EP_PCFG_GPU, _CORRECTABLE_ERROR_STATUS, _BAD_TLP, 0x1, xtlAerCorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_BAD_TLP;
    if (FLD_TEST_DRF_NUM(_EP_PCFG_GPU, _CORRECTABLE_ERROR_STATUS, _BAD_DLLP , 0x1, xtlAerCorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_BAD_DLLP;
    if (FLD_TEST_DRF_NUM(_EP_PCFG_GPU, _CORRECTABLE_ERROR_STATUS, _REPLAY_NUM_ROLLOVER, 0x1, xtlAerCorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RPLY_ROLLOVER;
    if (FLD_TEST_DRF_NUM(_EP_PCFG_GPU, _CORRECTABLE_ERROR_STATUS, _REPLAY_TIMER_TIMEOUT, 0x1, xtlAerCorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RPLY_TIMEOUT;
    if (FLD_TEST_DRF_NUM(_EP_PCFG_GPU, _CORRECTABLE_ERROR_STATUS, _ADVISORY_NON_FATAL_ERROR, 0x1, xtlAerCorr))
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
kbifClearXveAer_GH100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32      bits
)
{
    NvU32 xtlAerUncorr = 0;
    NvU32 xtlAerCorr   = 0;

    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_DLINK_PROTO_ERR)
        xtlAerUncorr = FLD_SET_DRF_NUM(_EP_PCFG_GPU, _UNCORRECTABLE_ERROR_STATUS, _DL_PROTOCOL_ERROR, 0x1,
                                       xtlAerUncorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_POISONED_TLP)
        xtlAerUncorr = FLD_SET_DRF_NUM(_EP_PCFG_GPU, _UNCORRECTABLE_ERROR_STATUS, _POISONED_TLP_RCVD, 0x1,
                                       xtlAerUncorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_CPL_TIMEOUT)
        xtlAerUncorr = FLD_SET_DRF_NUM(_EP_PCFG_GPU, _UNCORRECTABLE_ERROR_STATUS, _COMPLETION_TIMEOUT, 0x1,
                                       xtlAerUncorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_UNEXP_CPL)
        xtlAerUncorr = FLD_SET_DRF_NUM(_EP_PCFG_GPU, _UNCORRECTABLE_ERROR_STATUS, _UNEXPECTED_COMPLETION, 0x1,
                                       xtlAerUncorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_MALFORMED_TLP)
        xtlAerUncorr = FLD_SET_DRF_NUM(_EP_PCFG_GPU, _UNCORRECTABLE_ERROR_STATUS, _MALFORMED_TLP, 0x1,
                                       xtlAerUncorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_UNSUPPORTED_REQ)
        xtlAerUncorr = FLD_SET_DRF_NUM(_EP_PCFG_GPU, _UNCORRECTABLE_ERROR_STATUS, _UNSUPPORTED_REQUEST_ERROR, 0x1,
                                       xtlAerUncorr);

    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RCV_ERR)
        xtlAerCorr = FLD_SET_DRF_NUM(_EP_PCFG_GPU, _CORRECTABLE_ERROR_STATUS, _RECEIVER_ERROR, 0x1,
                                     xtlAerCorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_BAD_TLP)
        xtlAerCorr = FLD_SET_DRF_NUM(_EP_PCFG_GPU, _CORRECTABLE_ERROR_STATUS, _BAD_TLP, 0x1,
                                     xtlAerCorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_BAD_DLLP)
        xtlAerCorr = FLD_SET_DRF_NUM(_EP_PCFG_GPU, _CORRECTABLE_ERROR_STATUS, _BAD_DLLP, 0x1,
                                     xtlAerCorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RPLY_ROLLOVER)
        xtlAerCorr = FLD_SET_DRF_NUM(_EP_PCFG_GPU, _CORRECTABLE_ERROR_STATUS, _REPLAY_NUM_ROLLOVER, 0x1,
                                     xtlAerCorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RPLY_TIMEOUT)
        xtlAerCorr = FLD_SET_DRF_NUM(_EP_PCFG_GPU, _CORRECTABLE_ERROR_STATUS, _REPLAY_TIMER_TIMEOUT, 0x1,
                                     xtlAerCorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_ADVISORY_NONFATAL)
        xtlAerCorr = FLD_SET_DRF_NUM(_EP_PCFG_GPU, _CORRECTABLE_ERROR_STATUS, _ADVISORY_NON_FATAL_ERROR, 0x1,
                                     xtlAerCorr);

    if (xtlAerUncorr != 0)
    {
        GPU_BUS_CFG_CYCLE_WR32(pGpu, NV_EP_PCFG_GPU_UNCORRECTABLE_ERROR_STATUS, xtlAerUncorr);
    }
    if (xtlAerCorr != 0)
    {
        GPU_BUS_CFG_CYCLE_WR32(pGpu, NV_EP_PCFG_GPU_CORRECTABLE_ERROR_STATUS, xtlAerCorr);
    }

    return NV_OK;
}

/*!
 * @brief Return the BAR0 offset and size of the PCI config space mirror.
 *
 * @param[in]  pGpu          GPU object pointer
 * @param[in]  pKernelBif    Kernel BIF object pointer
 * @param[out] pBase         BAR0 offset of the PCI config space mirror
 * @param[out] pSize         Size in bytes of the PCI config space mirror
 *
 * @returns NV_OK
 */
NV_STATUS
kbifGetPciConfigSpacePriMirror_GH100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32     *pBase,
    NvU32     *pSize
)
{
    *pBase = DEVICE_BASE(NV_EP_PCFGM);
    *pSize = DEVICE_EXTENT(NV_EP_PCFGM) - DEVICE_BASE(NV_EP_PCFGM) + 1;
    return NV_OK;
}

/*!
 * Kernel-RM only function to enable PCIe requester atomics by
 * using OS HAL interface and to cache the completer side capabilities returned
 * by the OS HAL interface. RM regkeys "RMSysmemSelectAtomicsConfig" and
 * "RMSysmemOverridePcieReqAtomicOps" can be used to force override the
 * settings.
 *
 *
 * @param[in]       pGpu        OBJGPU pointer
 * @param[in/out]   pKernelBif  Kernel BIF object pointer, PCIe requester atomics details
 *                              are cached in here.
 * @returns None
 */
void
kbifProbePcieReqAtomicCaps_GH100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32   osAtomicsMask    = 0;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJCL  *pCl  = SYS_GET_CL(pSys);

    if (!pCl->getProperty(pCl, PDB_PROP_CL_BUG_3562968_WAR_ALLOW_PCIE_ATOMICS))
    {
        NV_PRINTF(LEVEL_INFO, "PCIe atomics not supported in this platform!\n");
        return;
    }

    if (osConfigurePcieReqAtomics(pGpu->pOsGpuInfo, &osAtomicsMask) != NV_OK ||
        osAtomicsMask == 0)
    {
        NV_PRINTF(LEVEL_INFO, "PCIe requester atomics not enabled since "
                  "completer is not capable!\n");
        return;
    }

    pKernelBif->osPcieAtomicsOpMask = osAtomicsMask;

    // Program PCIe atomics register settings
    kbifEnablePcieAtomics_HAL(pGpu, pKernelBif);

    return;
}

/*!
 * @brief Enable PCIe atomics if PCIe hierarchy supports it
 *
 * @param[in] pGpu       GPU object pointer
 * @param[in] pKernelBif Kernel BIF object pointer
 *
 */
void
kbifEnablePcieAtomics_GH100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 regVal;

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS_2, &regVal) != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "Read of NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS_2 failed.\n");
        return;
    }

    regVal = FLD_SET_DRF_NUM(_EP_PCFG_GPU, _DEVICE_CONTROL_STATUS_2, _ATOMIC_OP_REQUESTER_ENABLE, 0x1, regVal);

    if (GPU_BUS_CFG_CYCLE_WR32(pGpu, NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS_2, regVal) != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "Write of NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS_2 failed.\n");
        return;
    }

    NV_PRINTF(LEVEL_INFO, "PCIe Requester atomics enabled.\n");
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
kbifGetBusOptionsAddr_GH100
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
            *pAddrReg = NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS;
            break;
        case BUS_OPTIONS_LINK_CONTROL_STATUS:
            *pAddrReg = NV_EP_PCFG_GPU_LINK_CONTROL_STATUS;
            break;
        case BUS_OPTIONS_LINK_CAPABILITIES:
            *pAddrReg = NV_EP_PCFG_GPU_LINK_CAPABILITIES;
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
 * @brief Returns size of MSIX vector control table
 *
 * @param  pGpu  OBJGPU pointer
 * @param  pKernelBif  Kernel BIF object pointer
 */
NvU32
kbifGetMSIXTableVectorControlSize_GH100
(
    OBJGPU *pGpu,
    KernelBif *pKernelBif
)
{
    return NV_VIRTUAL_FUNCTION_PRIV_MSIX_TABLE_VECTOR_CONTROL__SIZE_1;
}


/*!
 * @brief Check and cache Function level reset support
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 *
 */
void
kbifCacheFlrSupport_GH100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 regVal = 0;

    // Read config register
    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_DEVICE_CAPABILITIES,
                               &regVal) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_EP_PCFG_GPU_DEVICE_CAPABILITIES\n");
        return;
    }

    // Check if FLR is supported
    if (FLD_TEST_DRF(_EP_PCFG_GPU, _DEVICE_CAPABILITIES, _FUNCTION_LEVEL_RESET_CAPABILITY,
                     _SUPPORTED, regVal))
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
kbifCache64bBar0Support_GH100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 regVal = 0;

    // Read config register
    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_BARREG0,
                               &regVal) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_EP_PCFG_GPU_BARREG0\n");
        return;
    }

    // Check if 64b BAR0 is supported
    if (FLD_TEST_DRF(_EP_PCFG_GPU, _BARREG0, _REG_ADDR_TYPE,
                     _64BIT, regVal))
    {
        pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_64BIT_BAR0_SUPPORTED, NV_TRUE);
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
_kbifGetBarInfo_GH100
(
    OBJGPU    *pGpu,
    NvU32      barRegCSBase,
    NvU32      barIndex,
    NvU64     *pBarBaseAddress,
    NvBool    *pIs64BitBar
)
{
    NV_STATUS status         = NV_OK;
    NvBool    barIs64Bit     = NV_FALSE;
    NvU32     barAddrLow     = 0;
    NvU32     barAddrHigh    = 0;
    NvU32     barRegCSLimit  = barRegCSBase + NV_EP_PCFG_GPU_BARREG5 - NV_EP_PCFG_GPU_BARREG0;
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
        barIs64Bit = FLD_TEST_DRF(_EP_PCFG_GPU, _BARREG0, _REG_ADDR_TYPE, _64BIT, barAddrLow);

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
kbifCacheVFInfo_GH100
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
    status = GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_SRIOV_INIT_TOT_VF, &regVal);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_EP_PCFG_GPU_SRIOV_INIT_TOT_VF\n");
        return;
    }
    pGpu->sriovState.totalVFs = GPU_DRF_VAL(_EP_PCFG_GPU, _SRIOV_INIT_TOT_VF,
                                            _TOTAL_VFS, regVal);

    // Get first VF offset
    status = GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_SRIOV_FIRST_VF_STRIDE, &regVal);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_EP_PCFG_GPU_SRIOV_FIRST_VF_STRIDE\n");
        return;
    }
    pGpu->sriovState.firstVFOffset = GPU_DRF_VAL(_EP_PCFG_GPU, _SRIOV_FIRST_VF_STRIDE,
                                                 _FIRST_VF_OFFSET, regVal);

    // Get VF BAR0 info
    status = _kbifGetBarInfo_GH100(pGpu, NV_EP_PCFG_GPU_VF_BAR0, 0, &barAddr, &barIs64Bit);
    NV_ASSERT(status == NV_OK);

    pGpu->sriovState.firstVFBarAddress[0] = barAddr;
    pGpu->sriovState.b64bitVFBar0         = barIs64Bit;

    // Get VF BAR1 info
    status = _kbifGetBarInfo_GH100(pGpu, NV_EP_PCFG_GPU_VF_BAR0, 1, &barAddr, &barIs64Bit);
    NV_ASSERT(status == NV_OK);

    pGpu->sriovState.firstVFBarAddress[1] = barAddr;
    pGpu->sriovState.b64bitVFBar1         = barIs64Bit;

    // Get VF BAR2 info
    status = _kbifGetBarInfo_GH100(pGpu, NV_EP_PCFG_GPU_VF_BAR0, 2, &barAddr, &barIs64Bit);
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
kbifStopSysMemRequests_GH100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvBool     bStop
)
{
    NvU32 regVal;

    NV_ASSERT_OK_OR_RETURN(GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_CTRL_CMD_AND_STATUS,
                                                  &regVal));

    if (bStop)
    {
        regVal = FLD_SET_DRF(_EP_PCFG_GPU, _CTRL_CMD_AND_STATUS, _CMD_BUS_MASTER, _DISABLE, regVal);
    }
    else
    {
        regVal = FLD_SET_DRF(_EP_PCFG_GPU, _CTRL_CMD_AND_STATUS, _CMD_BUS_MASTER, _ENABLE, regVal);
    }

    NV_ASSERT_OK_OR_RETURN(GPU_BUS_CFG_CYCLE_WR32(pGpu, NV_EP_PCFG_GPU_CTRL_CMD_AND_STATUS,
                                                  regVal));

    return NV_OK;
}

/*!
 * @brief Waits for function issued transaction completions (sysmem to GPU) to arrive
 *
 * @param[in] pGpu        GPU object pointer
 * @param[in] pKernelBif  KernelBif object pointer
 *
 * @return NV_OK
 */
NV_STATUS
kbifWaitForTransactionsComplete_GH100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    RMTIMEOUT timeout;
    NvU32     regVal;

    NV_ASSERT_OK_OR_RETURN(GPU_BUS_CFG_CYCLE_RD32(
        pGpu, NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS, &regVal));

    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);

    // Wait for number of pending transactions to go to 0
    while (DRF_VAL(_EP_PCFG_GPU, _DEVICE_CONTROL_STATUS, _TRANSACTIONS_PENDING, regVal) != 0)
    {
        NV_ASSERT_OK_OR_RETURN(gpuCheckTimeout(pGpu, &timeout));
        NV_ASSERT_OK_OR_RETURN(GPU_BUS_CFG_CYCLE_RD32(
            pGpu, NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS, &regVal));
    }

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
kbifTriggerFlr_GH100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32     regVal = 0;
    NV_STATUS status = NV_OK;

    status = GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS,
                                    &regVal);

    if (status != NV_OK)
    {
        NV_ASSERT_FAILED("Config space read of device control failed\n");
        return status;
    }

    regVal = FLD_SET_DRF_NUM(_EP_PCFG_GPU, _DEVICE_CONTROL_STATUS ,
                             _INITIATE_FN_LVL_RST, 0x1, regVal);

    status = GPU_BUS_CFG_CYCLE_WR32(pGpu, NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS,
                                    regVal);

    if (status != NV_OK)
    {
        NV_ASSERT_FAILED("FLR trigger failed\n");
        return status;
    }

    return status;
}

/*!
 * @brief Try restoring BAR registers and command register using config cycles
 *
 * @param[in] pGpu       GPU object pointer
 * @param[in] pKernelBif KernelBif object pointer
 *
 * @return    NV_OK on success
 *            NV_ERR_INVALID_READ if the register read returns unexpected value
 *            NV_ERR_OBJECT_NOT_FOUND if the object is not found
 */
NV_STATUS
kbifRestoreBarsAndCommand_GH100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32  *pBarRegOffsets = pKernelBif->barRegOffsets;
    NvU32  barOffsetEntry;

    // Restore all BAR registers
    for (barOffsetEntry = 0; barOffsetEntry < KBIF_NUM_BAR_OFFSET_ENTRIES; barOffsetEntry++)
    {
        if (pBarRegOffsets[barOffsetEntry] != KBIF_INVALID_BAR_REG_OFFSET)
        {
            GPU_BUS_CFG_CYCLE_WR32(pGpu, pBarRegOffsets[barOffsetEntry],
                                   pKernelBif->cacheData.gpuBootConfigSpace[pBarRegOffsets[barOffsetEntry]/sizeof(NvU32)]);
        }
    }

    // Restore Device Control register
    GPU_BUS_CFG_CYCLE_WR32(pGpu, NV_EP_PCFG_GPU_CTRL_CMD_AND_STATUS,
                           pKernelBif->cacheData.gpuBootConfigSpace[NV_EP_PCFG_GPU_CTRL_CMD_AND_STATUS/sizeof(NvU32)]);

    if (GPU_REG_RD32(pGpu, NV_PMC_BOOT_0) != pGpu->chipId0)
    {
        return NV_ERR_INVALID_READ;
    }

    return NV_OK;
}

/*!
 * @brief HAL specific BIF software state initialization
 *
 * @param[in] pGpu       GPU object pointer
 * @param[in] pKernelBif KernelBif object pointer
 *
 * @return    NV_OK on success
 */
NV_STATUS
kbifInit_GH100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    // Cache the offsets of BAR registers into an array for subsequent use
    kbifStoreBarRegOffsets_HAL(pGpu, pKernelBif, NV_EP_PCFG_GPU_VF_BAR0);

    return NV_OK;
}

