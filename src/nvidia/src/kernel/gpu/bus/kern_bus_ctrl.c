/*
 * SPDX-FileCopyrightText: Copyright (c) 2002-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "nv_ref.h"
#include "platform/chipset/chipset.h"
#include "os/os.h"
#include "core/system.h"
#include "core/locks.h"
#include "gpu/bif/kernel_bif.h"
#include "gpu/mmu/kern_gmmu.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "vgpu/rpc.h"
#include "gpu/device/device.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/subdevice/subdevice_diag.h"
#include "platform/sli/sli.h"

#include "ctrl/ctrl0080/ctrl0080host.h"
#include "ctrl/ctrl2080/ctrl2080bus.h"
#include "ctrl/ctrl208f/ctrl208fbus.h"

static NvU32
kbusControlGetCaps
(
    OBJGPU *pGpu
)
{
    NvU32 caps = 0;

    // if the Chip is integrated.
    if ( IsTEGRA(pGpu) || pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_SOC_SDM ))
    {
        caps |= NV2080_CTRL_BUS_INFO_CAPS_CHIP_INTEGRATED;
    }

    return caps;
}

static NV_STATUS
_kbusGetHostCaps(OBJGPU *pGpu, NvU8 *pHostCaps)
{
    NV_STATUS  status           = NV_OK;
    NvBool     bCapsInitialized = NV_FALSE;

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    {
        KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
        if (pKernelBus == NULL)
        {
            status = NV_ERR_INVALID_POINTER;
            SLI_LOOP_BREAK;
        }

        kbusGetDeviceCaps(pGpu, pKernelBus, pHostCaps, bCapsInitialized);
        bCapsInitialized = NV_TRUE;
    }
    SLI_LOOP_END

    return status;
}

static NV_STATUS
_getAspmL1FlagsSendRpc
(
    OBJGPU *pGpu,
    NvBool *bCyaMaskL1,
    NvBool *bEnableAspmDtL1
)
{
    RM_API    *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS  rmStatus;
    NV2080_CTRL_INTERNAL_BIF_GET_ASPM_L1_FLAGS_PARAMS *pBifAspmL1Flags;

    // Allocate memory for the command parameter
    pBifAspmL1Flags = portMemAllocNonPaged(sizeof(*pBifAspmL1Flags));
    if (pBifAspmL1Flags == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Could not allocate pBifAspmL1Flags.");
        rmStatus = NV_ERR_NO_MEMORY;
        goto _getAspmL1FlagsSendRpc_exit;
    }
    portMemSet(pBifAspmL1Flags, 0, sizeof(*pBifAspmL1Flags));

    // Send RPC to GSP to get physical BIF PDBs
    rmStatus = pRmApi->Control(pRmApi,
                                pGpu->hInternalClient,
                                pGpu->hInternalSubdevice,
                                NV2080_CTRL_CMD_INTERNAL_BIF_GET_ASPM_L1_FLAGS,
                                pBifAspmL1Flags, sizeof(*pBifAspmL1Flags));

    if (NV_OK != rmStatus)
    {
        NV_PRINTF(LEVEL_ERROR,
                    "Error 0x%x receiving bus ASPM disable flags from GSP.\n", rmStatus);
        goto _getAspmL1FlagsSendRpc_exit;
    }

    *bCyaMaskL1      = pBifAspmL1Flags->bCyaMaskL1;
    *bEnableAspmDtL1 = pBifAspmL1Flags->bEnableAspmDtL1;

_getAspmL1FlagsSendRpc_exit:
    portMemFree(pBifAspmL1Flags);
    return rmStatus;
}

//
// HOST RM Device Controls
//
NV_STATUS
deviceCtrlCmdHostGetCaps_IMPL
(
    Device *pDevice,
    NV0080_CTRL_HOST_GET_CAPS_PARAMS *pHostCapsParams
)
{
    OBJGPU     *pGpu        = GPU_RES_GET_GPU(pDevice);

    // sanity check array size
    if (pHostCapsParams->capsTblSize != NV0080_CTRL_HOST_CAPS_TBL_SIZE)
    {
        NV_PRINTF(LEVEL_ERROR, "size mismatch: client 0x%x rm 0x%x\n",
                  pHostCapsParams->capsTblSize,
                  NV0080_CTRL_HOST_CAPS_TBL_SIZE);
        return NV_ERR_INVALID_ARGUMENT;
    }

    return _kbusGetHostCaps(pGpu, NvP64_VALUE(pHostCapsParams->capsTbl));
}

//
// HOST RM Device Controls
//
NV_STATUS
deviceCtrlCmdHostGetCapsV2_IMPL
(
    Device *pDevice,
    NV0080_CTRL_HOST_GET_CAPS_V2_PARAMS *pHostCapsParamsV2
)
{
    OBJGPU     *pGpu        = GPU_RES_GET_GPU(pDevice);
    NvU8       *pHostCaps   = pHostCapsParamsV2->capsTbl;
    NV_STATUS   rmStatus    = NV_OK;

    rmStatus = _kbusGetHostCaps(pGpu, pHostCaps);

    return rmStatus;
}

//
// BUS RM SubDevice Controls
//
NV_STATUS
subdeviceCtrlCmdBusGetPciInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_BUS_GET_PCI_INFO_PARAMS *pPciInfoParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);

    if (pKernelBif == NULL || !kbifIsPciBusFamily(pKernelBif))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    pPciInfoParams->pciDeviceId = pGpu->idInfo.PCIDeviceID;
    pPciInfoParams->pciSubSystemId = pGpu->idInfo.PCISubDeviceID;
    pPciInfoParams->pciRevisionId = pGpu->idInfo.PCIRevisionID;

    //
    // Return device ID field.  We no longer support probing past the BR02 bridge.
    //
    pPciInfoParams->pciExtDeviceId = REF_VAL(NV_CONFIG_PCI_NV_0_DEVICE_ID, pPciInfoParams->pciDeviceId);

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdBusGetAspmDisableFlags_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_BUS_GET_ASPM_DISABLE_FLAGS_PARAMS *pParams
)
{
    OBJGPU    *pGpu   = GPU_RES_GET_GPU(pSubdevice);
    OBJSYS    *pSys   = SYS_GET_INSTANCE();
    OBJCL     *pCl    = SYS_GET_CL(pSys);
    NV_STATUS  rmStatus;
    NvBool     bCyaMaskL1, bEnableAspmDtL1;

    // Send RPC to GSP to obtain BIF PDB values.
    rmStatus = _getAspmL1FlagsSendRpc(pGpu, &bCyaMaskL1, &bEnableAspmDtL1);
    if (NV_OK != rmStatus)
    {
        return rmStatus;
    }

    pParams->aspmDisableFlags[NV2080_CTRL_ASPM_DISABLE_FLAGS_L1_MASK_REGKEY_OVERRIDE]                = bCyaMaskL1;
    // This flag correnpond to an deprecated PDB_PROP_OS_RM_MAKES_POLICY_DECISIONS property which is always returing TURE on non-MACOSX.
    pParams->aspmDisableFlags[NV2080_CTRL_ASPM_DISABLE_FLAGS_OS_RM_MAKES_POLICY_DECISIONS]           = NV_TRUE;
    pParams->aspmDisableFlags[NV2080_CTRL_ASPM_DISABLE_FLAGS_GPU_BEHIND_BRIDGE]                      = pGpu->getProperty(pGpu, PDB_PROP_GPU_BEHIND_BRIDGE);
    pParams->aspmDisableFlags[NV2080_CTRL_ASPM_DISABLE_FLAGS_GPU_UPSTREAM_PORT_L1_UNSUPPORTED]       = pGpu->getProperty(pGpu, PDB_PROP_GPU_UPSTREAM_PORT_L1_UNSUPPORTED);
    pParams->aspmDisableFlags[NV2080_CTRL_ASPM_DISABLE_FLAGS_GPU_UPSTREAM_PORT_L1_POR_SUPPORTED]     = pGpu->getProperty(pGpu, PDB_PROP_GPU_UPSTREAM_PORT_L1_POR_SUPPORTED);
    pParams->aspmDisableFlags[NV2080_CTRL_ASPM_DISABLE_FLAGS_GPU_UPSTREAM_PORT_L1_POR_MOBILE_ONLY]   = pGpu->getProperty(pGpu, PDB_PROP_GPU_UPSTREAM_PORT_L1_POR_MOBILE_ONLY);
    pParams->aspmDisableFlags[NV2080_CTRL_ASPM_DISABLE_FLAGS_CL_ASPM_L1_CHIPSET_DISABLED]            = pCl->getProperty(pCl, PDB_PROP_CL_ASPM_L1_CHIPSET_DISABLED);
    pParams->aspmDisableFlags[NV2080_CTRL_ASPM_DISABLE_FLAGS_CL_ASPM_L1_CHIPSET_ENABLED_MOBILE_ONLY] = pCl->getProperty(pCl, PDB_PROP_CL_ASPM_L1_CHIPSET_ENABLED_MOBILE_ONLY);
    pParams->aspmDisableFlags[NV2080_CTRL_ASPM_DISABLE_FLAGS_BIF_ENABLE_ASPM_DT_L1]                  = bEnableAspmDtL1;

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdBusGetNvlinkPeerIdMask_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_BUS_GET_NVLINK_PEER_ID_MASK_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && RMCFG_FEATURE_PLATFORM_GSP)
    {
        return NV_ERR_NOT_SUPPORTED;
    }
    else
    {
        KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
        NvU32 gfid;

        NV_ASSERT_OR_RETURN(rmapiLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

        // This control call should always run in context of a VF.
        NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));
        if (IS_GFID_PF(gfid))
        {
            return NV_ERR_NOT_SUPPORTED;
        }

        portMemCopy((void *)pParams->nvlinkPeerIdMask,
                    sizeof(pParams->nvlinkPeerIdMask),
                    (void *)pKernelBus->p2p.busNvlinkPeerNumberMask,
                    sizeof(pParams->nvlinkPeerIdMask));

        return NV_OK;
    }
}

static NV_STATUS
getBusInfos(OBJGPU *pGpu, NV2080_CTRL_BUS_INFO *pBusInfos, NvU32 busInfoListSize)
{
    KernelBif        *pKernelBif  = GPU_GET_KERNEL_BIF(pGpu);
    VirtMemAllocator *pDma        = GPU_GET_DMA(pGpu);
    KernelGmmu       *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);

    NV_STATUS status = NV_OK;
    NvU32 i = 0;

    for (i = 0; i < busInfoListSize; i++)
    {
        NvBool bSendRpc = NV_FALSE;

        switch (pBusInfos[i].index)
        {
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GEN_INFO:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GEN2_INFO:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_LINK_WIDTH_SWITCH_ERROR_COUNT:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_LINK_SPEED_SWITCH_ERROR_COUNT:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_CYA_ASPM:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_LINECODE_ERRORS_CLEAR:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_CRC_ERRORS_CLEAR:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_NAKS_RECEIVED_CLEAR:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_FAILED_L0S_EXITS_CLEAR:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_CORRECTABLE_ERRORS_CLEAR:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_NONFATAL_ERRORS_CLEAR:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_FATAL_ERRORS_CLEAR:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_UNSUPPORTED_REQUESTS_CLEAR:
                bSendRpc = IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu);
                break;
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_ASLM_STATUS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_LINECODE_ERRORS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_CRC_ERRORS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_NAKS_RECEIVED:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_FAILED_L0S_EXITS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_CORRECTABLE_ERRORS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_NONFATAL_ERRORS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_FATAL_ERRORS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_UNSUPPORTED_REQUESTS:
                bSendRpc = IS_GSP_CLIENT(pGpu);
                break;
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_AER:
            case NV2080_CTRL_BUS_INFO_INDEX_MSI_INFO:
                bSendRpc = IS_VIRTUAL(pGpu);
                break;
        }

        if (bSendRpc)
        {
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kbusSendBusInfo(pGpu, GPU_GET_KERNEL_BUS(pGpu), &pBusInfos[i]));
            continue;
        }

        switch (pBusInfos[i].index)
        {
            case NV2080_CTRL_BUS_INFO_INDEX_TYPE:
            {
                pBusInfos[i].data = kbifGetBusIntfType_HAL(pKernelBif);
                break;
            }
            case NV2080_CTRL_BUS_INFO_INDEX_INTLINE:
            {
                pBusInfos[i].data = pGpu->busInfo.IntLine;
                break;
            }
            case NV2080_CTRL_BUS_INFO_INDEX_CAPS:
            {
                pBusInfos[i].data = kbusControlGetCaps(pGpu);
                break;
            }
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_CAPS:
            {
                if (kbifIsPciBusFamily(pKernelBif) && IS_VIRTUAL(pGpu))
                {
                    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);
                    pBusInfos[i].data = pVSI->pcieGpuLinkCaps;
                    break;
                }
            }
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_ROOT_LINK_CAPS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_UPSTREAM_LINK_CAPS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_UPSTREAM_GEN_INFO:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_DOWNSTREAM_LINK_CAPS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_CTRL_STATUS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_ROOT_LINK_CTRL_STATUS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_UPSTREAM_LINK_CTRL_STATUS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_DOWNSTREAM_LINK_CTRL_STATUS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_ERRORS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_ROOT_LINK_ERRORS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GEN_INFO:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GEN2_INFO:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_AER:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_BOARD_LINK_CAPS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_BOARD_GEN_INFO:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_BOARD_LINK_CTRL_STATUS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_ASLM_STATUS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_LINK_WIDTH_SWITCH_ERROR_COUNT:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_LINK_SPEED_SWITCH_ERROR_COUNT:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_CYA_ASPM:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_LINECODE_ERRORS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_CRC_ERRORS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_NAKS_RECEIVED:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_FAILED_L0S_EXITS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_LINECODE_ERRORS_CLEAR:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_CRC_ERRORS_CLEAR:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_NAKS_RECEIVED_CLEAR:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_FAILED_L0S_EXITS_CLEAR:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_CORRECTABLE_ERRORS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_NONFATAL_ERRORS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_FATAL_ERRORS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_UNSUPPORTED_REQUESTS:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_CORRECTABLE_ERRORS_CLEAR:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_NONFATAL_ERRORS_CLEAR:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_FATAL_ERRORS_CLEAR:
            case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_UNSUPPORTED_REQUESTS_CLEAR:
            case NV2080_CTRL_BUS_INFO_INDEX_MSI_INFO:
            {
                if (kbifIsPciBusFamily(pKernelBif))
                {
                    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kbifControlGetPCIEInfo(pGpu, pKernelBif, &pBusInfos[i]));
                }
                else
                {
                    status = NV_ERR_NOT_SUPPORTED;
                }
                break;
            }
            case NV2080_CTRL_BUS_INFO_INDEX_COHERENT_DMA_FLAGS:
            {
                pBusInfos[i].data = 0;

                if (pGpu->getProperty(pGpu, PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY))
                {
                    //
                    // On SOC Display all of the (system) memory that nvdisplay HW needs
                    // to read from/write to be allocated as non-coherent. it doesn't matter
                    // whether this system memory is accessed over the NISO inteface
                    // (e.g., for pushbuffers, semaphores, notifiers, etc) or
                    // over the ISO interface (for window/cursor surface pixel data,
                    // LUT entries, etc). on Orin, there's an ISO2AXI SHIM that ISOHUB will
                    // go through to interface with the memory subsystem on Orin,
                    // and a similar NISO2AXI SHIM will also exist for FE as well.
                    // a similar NISO2AXI SHIM will also exist for FE as well.
                    //
                    break;
                }

                if ((pKernelBif != NULL) &&
                    FLD_TEST_REF(BIF_DMA_CAPS_SNOOP, _CTXDMA, kbifGetDmaCaps(pGpu, pKernelBif)))
                {
                    pBusInfos[i].data |=
                        DRF_DEF(2080,
                                _CTRL_BUS_INFO_COHERENT_DMA_FLAGS,
                                _CTXDMA,
                                _TRUE);
                }

                if (pDma->gpuGartCaps & DMA_GPU_GART_CAPS_SNOOP)
                {
                    pBusInfos[i].data |=
                        DRF_DEF(2080,
                                _CTRL_BUS_INFO_COHERENT_DMA_FLAGS,
                                _GPUGART,
                                _TRUE);
                }
                break;
            }
            case NV2080_CTRL_BUS_INFO_INDEX_NONCOHERENT_DMA_FLAGS:
            {
                pBusInfos[i].data = 0;

                if (pGpu->getProperty(pGpu, PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY))
                {
                    //
                    // On SOC Display all of the (system) memory that nvdisplay HW needs
                    // to read from/write to be allocated as non-coherent. it doesn't matter
                    // whether this system memory is accessed over the NISO inteface
                    // (e.g., for pushbuffers, semaphores, notifiers, etc) or
                    // over the ISO interface (for window/cursor surface pixel data,
                    // LUT entries, etc). on Orin, there's an ISO2AXI SHIM that ISOHUB will
                    // go through to interface with the memory subsystem on Orin,
                    // and a similar NISO2AXI SHIM will also exist for FE as well.
                    //
                    pBusInfos[i].data |=
                        DRF_DEF(2080,
                                _CTRL_BUS_INFO_NONCOHERENT_DMA_FLAGS,
                                _CTXDMA,
                                _TRUE);
                    break;
                }

                if ((pKernelBif != NULL) &&
                    FLD_TEST_REF(BIF_DMA_CAPS_NOSNOOP, _CTXDMA, kbifGetDmaCaps(pGpu, pKernelBif)))
                {
                    pBusInfos[i].data |=
                        DRF_DEF(2080,
                                _CTRL_BUS_INFO_NONCOHERENT_DMA_FLAGS,
                                _CTXDMA,
                                _TRUE);
                }

                if (pDma->gpuGartCaps & DMA_GPU_GART_CAPS_NOSNOOP)
                {
                    pBusInfos[i].data |=
                        DRF_DEF(2080,
                                _CTRL_BUS_INFO_NONCOHERENT_DMA_FLAGS,
                                _GPUGART,
                                _TRUE);
                }

                break;
            }
            case NV2080_CTRL_BUS_INFO_INDEX_GPU_GART_SIZE:
            {
                pBusInfos[i].data = (NvU32)(kgmmuGetMaxVASize(pKernelGmmu) >> 20);
                break;
            }
            case NV2080_CTRL_BUS_INFO_INDEX_GPU_GART_SIZE_HI:
            {
                pBusInfos[i].data = (NvU32)((kgmmuGetMaxVASize(pKernelGmmu) >> 20) >> 32);
                break;
            }
            case NV2080_CTRL_BUS_INFO_INDEX_GPU_GART_FLAGS:
            {
                pBusInfos[i].data =
                    DRF_DEF(2080,
                            _CTRL_BUS_INFO_GPU_GART_FLAGS,
                            _UNIFIED,
                            _TRUE);
                break;
            }
            case NV2080_CTRL_BUS_INFO_INDEX_BUS_NUMBER:
            {
                if (kbifIsPciBusFamily(pKernelBif))
                {
                    pBusInfos[i].data = gpuGetBus(pGpu);
                }
                else
                {
                    status = NV_ERR_NOT_SUPPORTED;
                }
                break;
            }
            case NV2080_CTRL_BUS_INFO_INDEX_DEVICE_NUMBER:
            {
                if (kbifIsPciBusFamily(pKernelBif))
                {
                    pBusInfos[i].data = gpuGetDevice(pGpu);
                }
                else
                {
                    status = NV_ERR_NOT_SUPPORTED;
                }
                break;
            }
            case NV2080_CTRL_BUS_INFO_INDEX_INTERFACE_TYPE:
            case NV2080_CTRL_BUS_INFO_INDEX_GPU_INTERFACE_TYPE:
            {
                //
                // We no longer support AGP/PCIe bridges so Bus/GPU interface
                // types are the same
                //
                pBusInfos[i].data = kbifGetBusIntfType_HAL(pKernelBif);
                break;
            }
            case NV2080_CTRL_BUS_INFO_INDEX_DOMAIN_NUMBER:
            {
                pBusInfos[i].data = gpuGetDomain(pGpu);
                break;
            }
            case NV2080_CTRL_BUS_INFO_INDEX_SYSMEM_CONNECTION_TYPE:
            {
                if (pGpu->getProperty(pGpu, PDB_PROP_GPU_NVLINK_SYSMEM))
                {
                    pBusInfos[i].data = NV2080_CTRL_BUS_INFO_INDEX_SYSMEM_CONNECTION_TYPE_NVLINK;
                }
                else if (pGpu->getProperty(pGpu, PDB_PROP_GPU_C2C_SYSMEM))
                {
                    pBusInfos[i].data = NV2080_CTRL_BUS_INFO_INDEX_SYSMEM_CONNECTION_TYPE_C2C;
                }
                else
                {
                    pBusInfos[i].data = NV2080_CTRL_BUS_INFO_INDEX_SYSMEM_CONNECTION_TYPE_PCIE;
                }
                break;
            }
            default:
            {
                pBusInfos[i].data = 0;
                status = NV_ERR_INVALID_ARGUMENT;
                break;
            }
        }

        if (status != NV_OK)
        {
            break;
        }
    }

    return status;
}

NV_STATUS
subdeviceCtrlCmdBusGetInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_BUS_GET_INFO_PARAMS *pBusInfoParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);

    if ((pBusInfoParams->busInfoListSize == 0) ||
        (NvP64_VALUE(pBusInfoParams->busInfoList) == NULL))
    {
        return NV_OK;
    }

    return getBusInfos(pGpu, NvP64_VALUE(pBusInfoParams->busInfoList), pBusInfoParams->busInfoListSize);
}

NV_STATUS
subdeviceCtrlCmdBusGetInfoV2_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_BUS_GET_INFO_V2_PARAMS *pBusInfoParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);

    if ((pBusInfoParams->busInfoListSize > NV2080_CTRL_BUS_INFO_MAX_LIST_SIZE) ||
        (pBusInfoParams->busInfoListSize == 0))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    return getBusInfos(pGpu, (NV2080_CTRL_BUS_INFO*)pBusInfoParams->busInfoList, pBusInfoParams->busInfoListSize);
}

NV_STATUS
subdeviceCtrlCmdBusGetPciBarInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_BUS_GET_PCI_BAR_INFO_PARAMS *pBarInfoParams
)
{
    OBJGPU    *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    NvU32 i;

    if (!kbifIsPciBusFamily(GPU_GET_KERNEL_BIF(pGpu)))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    // store the number of valid bars
    pBarInfoParams->pciBarCount = pKernelBus->totalPciBars;

    NV_ASSERT(pBarInfoParams->pciBarCount <= NV2080_CTRL_BUS_MAX_PCI_BARS);

    // store info for each of the valid bars
    for (i = 0; i < pBarInfoParams->pciBarCount; i++)
    {
        pBarInfoParams->pciBarInfo[i].flags = 0;
        pBarInfoParams->pciBarInfo[i].barSize = (NvU32)(pKernelBus->pciBarSizes[i] >> 20);
        pBarInfoParams->pciBarInfo[i].barSizeBytes = pKernelBus->pciBarSizes[i];
        pBarInfoParams->pciBarInfo[i].barOffset = pKernelBus->pciBars[i];
    }

    // clear remaining entries to zero
    for (; i < NV2080_CTRL_BUS_MAX_PCI_BARS; i++)
    {
        pBarInfoParams->pciBarInfo[i].flags = 0;
        pBarInfoParams->pciBarInfo[i].barSize = 0;
        pBarInfoParams->pciBarInfo[i].barSizeBytes = 0;
        pBarInfoParams->pciBarInfo[i].barOffset = 0;
    }

    return NV_OK;
}

NV_STATUS
diagapiCtrlCmdBusIsBar1Virtual_IMPL
(
    DiagApi *pDiagApi,
    NV208F_CTRL_BUS_IS_BAR1_VIRTUAL_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDiagApi);
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);

    pParams->bIsVirtual = !kbusIsBar1PhysicalModeEnabled(pKernelBus);
    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdBusSysmemAccess_IMPL
(
    Subdevice* pSubdevice,
    NV2080_CTRL_BUS_SYSMEM_ACCESS_PARAMS* pParams
)
{
    OBJGPU      *pGpu       = GPU_RES_GET_GPU(pSubdevice);
    KernelBif   *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);

    return kbifDisableSysmemAccess_HAL(pGpu, pKernelBif, pParams->bDisable);
}

NV_STATUS
subdeviceCtrlCmdBusGetPcieSupportedGpuAtomics_VF
(
    Subdevice *pSubdevice,
    NV2080_CTRL_CMD_BUS_GET_PCIE_SUPPORTED_GPU_ATOMICS_PARAMS *pParams
)
{
    // Atomics not supported in VF. See bug 3497203.
    for (NvU32 i = 0; i < NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_OP_TYPE_COUNT; i++)
    {
        pParams->atomicOp[i].bSupported = NV_FALSE;
        pParams->atomicOp[i].attributes = 0x0;
    }

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdBusGetC2CInfo_VF
(
    Subdevice *pSubdevice,
    NV2080_CTRL_CMD_BUS_GET_C2C_INFO_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

    NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_INVALID_STATE);

    pParams->bIsLinkUp = pVSI->c2cInfo.bIsLinkUp;
    pParams->nrLinks = pVSI->c2cInfo.nrLinks;
    pParams->linkMask = pVSI->c2cInfo.linkMask;
    pParams->perLinkBwMBps = pVSI->c2cInfo.perLinkBwMBps;
    pParams->remoteType = pVSI->c2cInfo.remoteType;

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdBusGetC2CLpwrStats_VF
(
    Subdevice                                      *pSubdevice,
    NV2080_CTRL_CMD_BUS_GET_C2C_LPWR_STATS_PARAMS  *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
subdeviceCtrlCmdBusSetC2CLpwrStateVote_VF
(
    Subdevice                                           *pSubdevice,
    NV2080_CTRL_CMD_BUS_SET_C2C_LPWR_STATE_VOTE_PARAMS  *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

