/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


/* ------------------------- System Includes -------------------------------- */
#include "gpu/gpu.h"
#include "gpu/bif/kernel_bif.h"
#include "platform/chipset/chipset.h"

#include "published/maxwell/gm107/dev_nv_xve.h"

// Defines for C73 chipset registers
#ifndef NV_XVR_VEND_XP1
#define NV_XVR_VEND_XP1                                  0x00000F04 /* RW-4R */

#define NV_XVR_VEND_XP1_IGNORE_L0S                            23:23 /* RWIVF */
#define NV_XVR_VEND_XP1_IGNORE_L0S_INIT                  0x00000000 /* RWI-V */
#define NV_XVR_VEND_XP1_IGNORE_L0S__PROD                 0x00000000 /* RW--V */
#define NV_XVR_VEND_XP1_IGNORE_L0S_EN                    0x00000001 /* RW--V */
#endif


/* ------------------------ Public Functions -------------------------------- */

/*!
 * @brief Get PCIe config test registers
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  BIF object pointer
 */
void
kbifGetPcieConfigAccessTestRegisters_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32     *pciStart,
    NvU32     *pcieStart
)
{
   *pciStart  = NV_XVE_ID;
   *pcieStart = NV_XVE_VCCAP_HDR;
}

/*!
 * @brief Verify PCIe config test registers
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  BIF object pointer
 *
 * @return  NV_OK
 */
NV_STATUS
kbifVerifyPcieConfigAccessTestRegisters_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32      nvXveId,
    NvU32      nvXveVccapHdr
)
{
    NvU32 data;

    GPU_BUS_CFG_RD32(pGpu, NV_XVE_ID, &data);

    if (FLD_TEST_DRF(_XVE, _ID, _VENDOR, _NVIDIA, data))
    {
        if (data != nvXveId)
            return NV_ERR_NOT_SUPPORTED;

        GPU_BUS_CFG_RD32(pGpu, NV_XVE_VCCAP_HDR, &data);

        if (FLD_TEST_DRF(_XVE, _VCCAP_HDR, _ID, _VC, data) &&
            FLD_TEST_DRF(_XVE, _VCCAP_HDR, _VER, _1, data))
        {
            if (data != nvXveVccapHdr)
                return NV_ERR_NOT_SUPPORTED;
            return NV_OK;
        }
    }
    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief Re-arm MSI
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 */
void
kbifRearmMSI_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NV_STATUS status = gpuSanityCheckRegisterAccess(pGpu, 0, NULL);

    if (status != NV_OK)
    {
        return;
    }

    // The 32 byte value doesn't matter, HW only looks at the offset.
    osGpuWriteReg032(pGpu, DEVICE_BASE(NV_PCFG) + NV_XVE_CYA_2, 0);
}

/*!
 * @brief Check if MSI is enabled in HW
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  BIF object pointer
 *
 * @return  True if MSI enabled else False
 */
NvBool
kbifIsMSIEnabledInHW_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 data32;
    if (NV_OK != GPU_BUS_CFG_RD32(pGpu, NV_XVE_MSI_CTRL, &data32))
    {
        NV_PRINTF(LEVEL_ERROR, "unable to read NV_XVE_MSI_CTRL\n");
    }

    return FLD_TEST_DRF(_XVE, _MSI_CTRL, _MSI, _ENABLE, data32);
}

/*!
 * @brief Check if access to PCI config space is enabled
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 *
 * @return  True if access to PCI config space is enabled
 */
NvBool
kbifIsPciIoAccessEnabled_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32   data = 0;

    if (NV_OK == GPU_BUS_CFG_RD32(pGpu, NV_XVE_DEV_CTRL, &data))
    {
        if (FLD_TEST_DRF(_XVE, _DEV_CTRL, _CMD_IO_SPACE, _ENABLED, data))
        {
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

/*!
 * @brief Check if device is a 3D controller
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 *
 * @return  True if device is a 3D controller
 */
NvBool
kbifIs3dController_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32   data = 0;

    if (NV_OK == GPU_BUS_CFG_RD32(pGpu, NV_XVE_REV_ID, &data))
    {
        if (FLD_TEST_DRF(_XVE, _REV_ID, _CLASS_CODE, _3D, data))
        {
            return NV_TRUE;
        }
    }

    return NV_FALSE;
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
kbifEnableNoSnoop_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvBool     bEnable
)
{
    NvU8  fieldVal;
    NvU32 regVal;

    regVal = GPU_REG_RD32(pGpu, DEVICE_BASE(NV_PCFG) + NV_XVE_DEVICE_CONTROL_STATUS);

    fieldVal = bEnable ? 1 : 0;
    regVal   = FLD_SET_DRF_NUM(_XVE, _DEVICE_CONTROL_STATUS,
                               _ENABLE_NO_SNOOP, fieldVal, regVal);

    GPU_REG_WR32(pGpu, DEVICE_BASE(NV_PCFG) + NV_XVE_DEVICE_CONTROL_STATUS, regVal);

    return NV_OK;
}

/*!
 * @brief Enables Relaxed Ordering PCI-E Capability in the PCI Config Space
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 */
void
kbifPcieConfigEnableRelaxedOrdering_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 xveDevCtrlStatus;

    if(NV_ERR_GENERIC  == GPU_BUS_CFG_RD32(pGpu, NV_XVE_DEVICE_CONTROL_STATUS, &xveDevCtrlStatus))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unable to read NV_XVE_DEVICE_CONTROL_STATUS!\n");
        DBG_BREAKPOINT();
    }
    else
    {
        GPU_BUS_CFG_FLD_WR_DRF_DEF(pGpu, xveDevCtrlStatus, _XVE, _DEVICE_CONTROL_STATUS,
                                   _ENABLE_RELAXED_ORDERING, _INIT);
    }
}

/*!
 * @brief Disables Relaxed Ordering PCI-E Capability in the PCI Config Space
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 */
void
kbifPcieConfigDisableRelaxedOrdering_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 xveDevCtrlStatus;

    if(NV_ERR_GENERIC  == GPU_BUS_CFG_RD32(pGpu, NV_XVE_DEVICE_CONTROL_STATUS, &xveDevCtrlStatus))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unable to read NV_XVE_DEVICE_CONTROL_STATUS!\n");
        DBG_BREAKPOINT();
    }
    else
    {
        xveDevCtrlStatus = FLD_SET_DRF_NUM(_XVE, _DEVICE_CONTROL_STATUS,
                                           _ENABLE_RELAXED_ORDERING, 0, xveDevCtrlStatus);
        GPU_BUS_CFG_WR32(pGpu, NV_XVE_DEVICE_CONTROL_STATUS, xveDevCtrlStatus);
    }
}

/*!
 * @brief Get XVE status bits
 *
 * @param[in]   pGpu        GPU object pointer
 * @param[in]   pKernelBif  BIF object pointer
 * @param[out]  pBits       PCIe error status values
 * @param[out]  pStatus     Full XVE status
 *
 * @return  NV_OK
 */
NV_STATUS
kbifGetXveStatusBits_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32     *pBits,
    NvU32     *pStatus
)
{
    // control/status reg
    NvU32 xveDevCtrlStatus;

    if (NV_OK  != GPU_BUS_CFG_RD32(pGpu, NV_XVE_DEVICE_CONTROL_STATUS, &xveDevCtrlStatus))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unable to read NV_XVE_DEVICE_CONTROL_STATUS!\n");
    }
    if ( pBits == NULL )
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

    if (pStatus)
        *pStatus = xveDevCtrlStatus;

    if (xveDevCtrlStatus & DRF_NUM(_XVE, _DEVICE_CONTROL_STATUS, _CORR_ERROR_DETECTED, 1))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_CORR_ERROR;
    if (xveDevCtrlStatus & DRF_NUM(_XVE, _DEVICE_CONTROL_STATUS, _NON_FATAL_ERROR_DETECTED, 1))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_NON_FATAL_ERROR;
    if (xveDevCtrlStatus & DRF_NUM(_XVE, _DEVICE_CONTROL_STATUS, _FATAL_ERROR_DETECTED, 1))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_FATAL_ERROR;
    if (xveDevCtrlStatus & DRF_NUM(_XVE, _DEVICE_CONTROL_STATUS, _UNSUPP_REQUEST_DETECTED, 1))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_UNSUPP_REQUEST;

    if (pKernelBif->EnteredRecoverySinceErrorsLastChecked)
    {
        pKernelBif->EnteredRecoverySinceErrorsLastChecked = NV_FALSE;
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_ENTERED_RECOVERY;
    }

    return NV_OK;
}

/*!
 * @brief Clear the XVE status bits
 *
 * @param[in]   pGpu        GPU object pointer
 * @param[in]   pKernelBif  BIF object pointer
 * @param[out]  pStatus     Full XVE status
 *
 * @return  NV_OK
 */
NV_STATUS
kbifClearXveStatus_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32     *pStatus
)
{
    NvU32 xveDevCtrlStatus;

    if (pStatus)
    {
        xveDevCtrlStatus = *pStatus;
        if (xveDevCtrlStatus == 0)
        {
            return NV_OK;
        }
    }
    else
    {
        if (NV_OK  != GPU_BUS_CFG_RD32(pGpu, NV_XVE_DEVICE_CONTROL_STATUS, &xveDevCtrlStatus))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Unable to read NV_XVE_DEVICE_CONTROL_STATUS!\n");
        }
    }

    GPU_BUS_CFG_WR32(pGpu, NV_XVE_DEVICE_CONTROL_STATUS, xveDevCtrlStatus);

    return NV_OK;
}

/*!
 * @brief Get XVE AER bits
 *
 * @param[in]   pGpu        GPU object pointer
 * @param[in]   pKernelBif  BIF object pointer
 * @param[out]  pBits       PCIe AER error status values
 *
 * @return  NV_OK
 */
NV_STATUS
kbifGetXveAerBits_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32     *pBits
)
{
    NvU32 xveAerUncorr;
    NvU32 xveAerCorr;

    if (pBits == NULL)
    {
        return NV_ERR_GENERIC;
    }

    *pBits = 0;

    if (NV_OK != GPU_BUS_CFG_RD32(pGpu, NV_XVE_AER_UNCORR_ERR, &xveAerUncorr))
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_XVE_AER_UNCORR_ERR\n");
        return NV_ERR_GENERIC;
    }
    if (NV_OK != GPU_BUS_CFG_RD32(pGpu, NV_XVE_AER_CORR_ERR, &xveAerCorr))
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_XVE_AER_CORR_ERR\n");
        return NV_ERR_GENERIC;
    }

    // The register read above returns garbage on fmodel, so just return.
    if (IS_FMODEL(pGpu))
    {
        return NV_OK;
    }

    if (FLD_TEST_DRF(_XVE, _AER_UNCORR_ERR, _DLINK_PROTO_ERR, _ACTIVE, xveAerUncorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_DLINK_PROTO_ERR;
    if (FLD_TEST_DRF(_XVE, _AER_UNCORR_ERR, _POISONED_TLP, _ACTIVE, xveAerUncorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_POISONED_TLP;
    if (FLD_TEST_DRF(_XVE, _AER_UNCORR_ERR, _CPL_TIMEOUT, _ACTIVE, xveAerUncorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_CPL_TIMEOUT;
    if (FLD_TEST_DRF(_XVE, _AER_UNCORR_ERR, _UNEXP_CPL, _ACTIVE, xveAerUncorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_UNEXP_CPL;
    if (FLD_TEST_DRF(_XVE, _AER_UNCORR_ERR, _MALFORMED_TLP, _ACTIVE, xveAerUncorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_MALFORMED_TLP;
    if (FLD_TEST_DRF(_XVE, _AER_UNCORR_ERR, _UNSUPPORTED_REQ, _ACTIVE, xveAerUncorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_UNSUPPORTED_REQ;

    if (FLD_TEST_DRF(_XVE, _AER_CORR_ERR, _RCV_ERR, _ACTIVE, xveAerCorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RCV_ERR;
    if (FLD_TEST_DRF(_XVE, _AER_CORR_ERR, _BAD_TLP, _ACTIVE, xveAerCorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_BAD_TLP;
    if (FLD_TEST_DRF(_XVE, _AER_CORR_ERR, _BAD_DLLP , _ACTIVE, xveAerCorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_BAD_DLLP;
    if (FLD_TEST_DRF(_XVE, _AER_CORR_ERR, _RPLY_ROLLOVER, _ACTIVE, xveAerCorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RPLY_ROLLOVER;
    if (FLD_TEST_DRF(_XVE, _AER_CORR_ERR, _RPLY_TIMEOUT, _ACTIVE, xveAerCorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RPLY_TIMEOUT;
    if (FLD_TEST_DRF(_XVE, _AER_CORR_ERR, _ADVISORY_NONFATAL, _ACTIVE, xveAerCorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_ADVISORY_NONFATAL;

    return NV_OK;
}

/*!
 * @brief Clear the XVE AER bits
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  BIF object pointer
 * @param[in]  bits        PCIe AER error status values
 *
 * @return  NV_OK
 */
NV_STATUS
kbifClearXveAer_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32      bits
)
{
    NvU32 xveAerUncorr = 0;
    NvU32 xveAerCorr   = 0;

    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_DLINK_PROTO_ERR)
        xveAerUncorr = FLD_SET_DRF(_XVE, _AER_UNCORR_ERR, _DLINK_PROTO_ERR, _CLEAR, xveAerUncorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_POISONED_TLP)
        xveAerUncorr = FLD_SET_DRF(_XVE, _AER_UNCORR_ERR, _POISONED_TLP, _CLEAR, xveAerUncorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_CPL_TIMEOUT)
        xveAerUncorr = FLD_SET_DRF(_XVE, _AER_UNCORR_ERR, _CPL_TIMEOUT, _CLEAR, xveAerUncorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_UNEXP_CPL)
        xveAerUncorr = FLD_SET_DRF(_XVE, _AER_UNCORR_ERR, _UNEXP_CPL, _CLEAR, xveAerUncorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_MALFORMED_TLP)
        xveAerUncorr = FLD_SET_DRF(_XVE, _AER_UNCORR_ERR, _MALFORMED_TLP, _CLEAR, xveAerUncorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_UNSUPPORTED_REQ)
        xveAerUncorr = FLD_SET_DRF(_XVE, _AER_UNCORR_ERR, _UNSUPPORTED_REQ, _CLEAR, xveAerUncorr);

    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RCV_ERR)
        xveAerCorr = FLD_SET_DRF(_XVE, _AER_CORR_ERR, _RCV_ERR, _CLEAR, xveAerCorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_BAD_TLP)
        xveAerCorr = FLD_SET_DRF(_XVE, _AER_CORR_ERR, _BAD_TLP, _CLEAR, xveAerCorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_BAD_DLLP)
        xveAerCorr = FLD_SET_DRF(_XVE, _AER_CORR_ERR, _BAD_DLLP, _CLEAR, xveAerCorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RPLY_ROLLOVER)
        xveAerCorr = FLD_SET_DRF(_XVE, _AER_CORR_ERR, _RPLY_ROLLOVER, _CLEAR, xveAerCorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RPLY_TIMEOUT)
        xveAerCorr = FLD_SET_DRF(_XVE, _AER_CORR_ERR, _RPLY_TIMEOUT, _CLEAR, xveAerCorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_ADVISORY_NONFATAL)
        xveAerCorr = FLD_SET_DRF(_XVE, _AER_CORR_ERR, _ADVISORY_NONFATAL, _CLEAR, xveAerCorr);

    if (xveAerUncorr != 0)
    {
        GPU_BUS_CFG_WR32(pGpu, NV_XVE_AER_UNCORR_ERR, xveAerUncorr);
    }
    if (xveAerCorr != 0)
    {
        GPU_BUS_CFG_WR32(pGpu, NV_XVE_AER_CORR_ERR, xveAerCorr);
    }

    return NV_OK;
}

/*!
 * @brief Returns the BAR0 offset and size of the PCI config space mirror
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 * @param[out]  pBase      BAR0 offset of the PCI config space mirror
 * @param[out]  pSize      Size in bytes of the PCI config space mirror
 *
 * @returns NV_OK
 */
NV_STATUS
kbifGetPciConfigSpacePriMirror_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32     *pBase,
    NvU32     *pSize
)
{
    *pBase = DEVICE_BASE(NV_PCFG);
    *pSize = DEVICE_EXTENT(NV_PCFG) - DEVICE_BASE(NV_PCFG) + 1;
    return NV_OK;
}

/*!
 * @brief C73 chipset WAR
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 */
void
kbifExecC73War_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    OBJSYS  *pSys = SYS_GET_INSTANCE();
    OBJOS   *pOS  = SYS_GET_OS(pSys);
    OBJCL   *pCl  = SYS_GET_CL(pSys);
    NvU32    val;

    if (CS_NVIDIA_C73 == pCl->Chipset)
    {
        //
        // Turn off L0s on the chipset which are required by the suspend/resume
        // cycles in Vista. See bug 400044 for more details.
        //

        // vAddr is a mapped cpu virtual addr into the root ports config space.
        if (!pOS->getProperty(pOS, PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS) &&
            (pGpu->gpuClData.rootPort.vAddr != 0))
        {
            val = MEM_RD32((NvU8*)pGpu->gpuClData.rootPort.vAddr+NV_XVR_VEND_XP1);
            val = FLD_SET_DRF(_XVR, _VEND_XP1, _IGNORE_L0S, _EN, val);
            MEM_WR32((NvU8*)pGpu->gpuClData.rootPort.vAddr+NV_XVR_VEND_XP1, val);
        }
        else if (pOS->getProperty(pOS, PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS) &&
                 pGpu->gpuClData.rootPort.addr.valid)
        {
            val = osPciReadDword(pGpu->gpuClData.rootPort.addr.handle, NV_XVR_VEND_XP1);
            val = FLD_SET_DRF(_XVR, _VEND_XP1, _IGNORE_L0S, _EN, val);
            osPciWriteDword(pGpu->gpuClData.rootPort.addr.handle, NV_XVR_VEND_XP1, val);
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Cannot turn off L0s on C73 chipset, suspend/resume may fail (Bug 400044).\n");
            DBG_BREAKPOINT();
        }
    }
}

NV_STATUS
kbifGetBusOptionsAddr_GM107
(
    OBJGPU     *pGpu,
    KernelBif  *pKernelBif,
    BUS_OPTIONS options,
    NvU32      *addrReg
)
{
    NV_STATUS status = NV_OK;

    switch (options)
    {
        case BUS_OPTIONS_DEV_CONTROL_STATUS:
            *addrReg = NV_XVE_DEVICE_CONTROL_STATUS;
            break;
        case BUS_OPTIONS_LINK_CONTROL_STATUS:
            *addrReg = NV_XVE_LINK_CONTROL_STATUS;
            break;
        case BUS_OPTIONS_LINK_CAPABILITIES:
            *addrReg = NV_XVE_LINK_CAPABILITIES;
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "Invalid register type passed 0x%x\n",
                      options);
            status = NV_ERR_GENERIC;
            break;
    }
    return status;
}
