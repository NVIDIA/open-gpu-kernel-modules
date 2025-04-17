/*
 * SPDX-FileCopyrightText: Copyright (c) 2000-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************* PCIE Chipset Routines *********************************\
*                                                                           *
*   One time initialization code to update the PCI Express chipset and      *
*   our own PCI Express related values (includes workarounds and registry   *
*   overrides).                                                             *
*                                                                           *
****************************************************************************/

#include "nvrm_registry.h"
#include "core/core.h"
#include "os/os.h"
#include "platform/platform.h"
#include "platform/chipset/chipset.h"
#include "platform/chipset/chipset_info.h"
#include "nvpcie.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/gpu.h"
#include "gpu/timer/objtmr.h"
#include "gpu/bif/kernel_bif.h"
#include "gpu/gsp/gsp_static_config.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "gpu/mem_mgr/virt_mem_allocator_common.h"
#include "ctrl/ctrl2080/ctrl2080bus.h" // NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_*
#include "core/thread_state.h"
#include "Nvcm.h"
#include "nvdevid.h"
#include "nvmisc.h"

#include "published/maxwell/gm107/dev_nv_xve.h" // NV_XVE_VCCAP_CTRL0*
#include "published/pcie_switch/pcie_switch_ref.h"

//
// static functions
//

static NV_STATUS objClInitPcieChipset(OBJGPU *, OBJCL *);
static void      objClBuildPcieAtomicsAllowList(OBJGPU *, OBJCL *);
static NvBool    objClInitGpuPortData(OBJGPU *, OBJCL *pCl);
static NV_STATUS objClSetPortCapsOffsets(OBJCL *, PORTDATA *);
static NV_STATUS objClSetPortPcieEnhancedCapsOffsets(OBJCL *, PORTDATA *);
static void *    objClFindRootPort(OBJGPU *, OBJCL *, NvU32, NvU8, NvU8 *, NvU8 *, NvU8 *, NvU16 *, NvU16 *);
static NvBool    objClBR03Exists(OBJGPU *, OBJCL *);
static NvBool    objClBR04Exists(OBJGPU *, OBJCL *);
static void *    objClPcieMapEnhCfgSpace(OBJCL *, NvU32, NvU8, NvU8, NvU8);
static void      objClPcieUnmapEnhCfgSpace(OBJCL *, void *);
static void       _objClPostSetupFuncRegkeyOverrides(OBJGPU *, OBJCL *);
static void      objClGpuMapRootPort(OBJGPU *, OBJCL *);
static void      objClGpuUnmapRootPort(OBJGPU *);
static void      objClGpuMapEnhCfgSpace(OBJGPU *, OBJCL *);
static void      objClGpuUnmapEnhCfgSpace(OBJGPU *);
static NV_STATUS objClGpuIs3DController(OBJGPU *);
static void      objClLoadPcieVirtualP2PApproval(OBJGPU *);
static void      _objClAdjustTcVcMap(OBJGPU *, OBJCL *, PORTDATA *);
static void      _objClGetDownstreamAtomicsEnabledMask(void  *, NvU32, NvU32 *);
static void      _objClGetUpstreamAtomicRoutingCap(void  *, NvU32, NvBool *);
static void      _objClGetDownstreamAtomicRoutingCap(void  *, NvU32, NvBool *);

extern void _Set_ASPM_L0S_L1(OBJCL *, NvBool, NvBool);

static NV_STATUS addHwbcToList(OBJGPU *, OBJHWBC *);

#define INTEL_HASWELL_POWER_CONTROL_UNIT_DEVICE_ID 0x2FC0
#define HASWELL_CPU_CAPID4_OFFSET 0x94
#define INTEL_C0_OR_C1_CPUID 0x306f2

#define NV_ACPI_TABLE_SIGNATURE_GFCM NvU32_BUILD('G','F','C','M')
#define NV_ACPI_TABLE_SIGNATURE_TDSR NvU32_BUILD('T','D','S','R')
#define NV_ACPI_TABLE_SIGNATURE_TDSX NvU32_BUILD('T','D','S','X')

NV_STATUS
clInitDeviceInfo_IMPL(OBJCL *pCl, OBJGPU *pGpu)
{
    NvU32 gpuId;

    // Find our NV device on the PCI bus and save it's pci bus/device address.
    gpuId = clInitMappingPciBusDevice(pGpu, pCl);
    if (gpuId == NV0000_CTRL_GPU_INVALID_ID)
        return NV_ERR_INVALID_DEVICE;

    // Now find our ports
    if (!objClInitGpuPortData(pGpu, pCl))
        return NV_ERR_OPERATING_SYSTEM;

    return NV_OK;
}

static void
_objClPostSetupFuncRegkeyOverrides(OBJGPU *pGpu, OBJCL *pCl)
{
    NvU32 data;

    if (osReadRegistryDword(pGpu, NV_REG_STR_CL_ASLM_CFG, &data) == NV_OK)
    {
        switch(DRF_VAL(_REG_STR, _CL_ASLM_CFG, _NV_LINK_UPGRADE, data))
        {
            case NV_REG_STR_CL_ASLM_CFG_NV_LINK_UPGRADE_YES:
                pCl->setProperty(pCl, PDB_PROP_CL_ASLM_SUPPORTS_NV_LINK_UPGRADE, NV_TRUE);
                break;
            case NV_REG_STR_CL_ASLM_CFG_NV_LINK_UPGRADE_NO:
                pCl->setProperty(pCl, PDB_PROP_CL_ASLM_SUPPORTS_NV_LINK_UPGRADE, NV_FALSE);
                break;
            default:
                break;
        }

        switch(DRF_VAL(_REG_STR, _CL_ASLM_CFG, _HOT_RESET, data))
        {
            case NV_REG_STR_CL_ASLM_CFG_HOT_RESET_YES:
                pCl->setProperty(pCl, PDB_PROP_CL_ASLM_SUPPORTS_HOT_RESET, NV_TRUE);
                break;
            case NV_REG_STR_CL_ASLM_CFG_HOT_RESET_NO:
                pCl->setProperty(pCl, PDB_PROP_CL_ASLM_SUPPORTS_HOT_RESET, NV_FALSE);
                break;
            default:
                break;
        }

        switch(DRF_VAL(_REG_STR, _CL_ASLM_CFG, _FAST_UPGRADE, data))
        {
            case NV_REG_STR_CL_ASLM_CFG_FAST_UPGRADE_YES:
                pCl->setProperty(pCl, PDB_PROP_CL_ASLM_SUPPORTS_FAST_LINK_UPGRADE, NV_TRUE);
                break;
            case NV_REG_STR_CL_ASLM_CFG_FAST_UPGRADE_NO:
                pCl->setProperty(pCl, PDB_PROP_CL_ASLM_SUPPORTS_FAST_LINK_UPGRADE, NV_FALSE);
                break;
            default:
                break;
        }

        switch(DRF_VAL(_REG_STR, _CL_ASLM_CFG, _GEN2_LINK_UPGRADE, data))
        {
            case NV_REG_STR_CL_ASLM_CFG_GEN2_LINK_UPGRADE_YES:
                pCl->setProperty(pCl, PDB_PROP_CL_ASLM_SUPPORTS_GEN2_LINK_UPGRADE, NV_TRUE);
                break;
            case NV_REG_STR_CL_ASLM_CFG_GEN2_LINK_UPGRADE_NO:
                pCl->setProperty(pCl, PDB_PROP_CL_ASLM_SUPPORTS_GEN2_LINK_UPGRADE, NV_FALSE);
                break;
            default:
                break;
        }
    }
}

NV_STATUS
addHwbcToList (OBJGPU *pGpu, OBJHWBC *pHWBC)
{
    HWBC_LIST *pHWBCList;
    HWBC_LIST *pGpuHWBCList;

    pHWBCList = portMemAllocNonPaged(sizeof(HWBC_LIST));
    if (NULL == pHWBCList)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }
    portMemSet(pHWBCList, 0, sizeof(HWBC_LIST));
    pHWBCList->pHWBC = pHWBC;
    if (NULL == pGpu->pHWBCList)
    {
        pGpu->pHWBCList = pHWBCList;
    }
    else
    {
        pGpuHWBCList = pGpu->pHWBCList;
        while (pGpuHWBCList->pNext)
        {
            pGpuHWBCList = pGpuHWBCList->pNext;
        }
        pGpuHWBCList->pNext = pHWBCList;
    }
    return NV_OK;
}

/*! @brief Build PCIe atomics allow list for x86 CPUs
 *         using CPU model and family.
 *         
 * Building allow list using only CPU model and family helps with
 * passthrough virtualization where the host and passthrough VM
 * has the same CPU model and family unlike the chipset.
 * For non-x86 CPUs, allow list is built only during chipset discovery.
 *
 * @param[in]   pGpu              GPU object pointer
 * @param[in]   pCl               Core logic object pointer
 * @return None
 */
static
void
objClBuildPcieAtomicsAllowList(OBJGPU *pGpu, OBJCL *pCl)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();

    // For non-x86 CPUs, allow list is built during chipset discovery.

    // Intel IceLake
    if ((pSys->cpuInfo.family == 0x6) &&
        (pSys->cpuInfo.model == 0x6a) &&
        (pSys->cpuInfo.stepping == 0x6))
    {
        pCl->setProperty(pCl, PDB_PROP_CL_BUG_3562968_WAR_ALLOW_PCIE_ATOMICS, NV_TRUE);
    }
    // Intel SapphireRapids
    else if ((pSys->cpuInfo.family == 0x6) &&
             (pSys->cpuInfo.model == 0x8f))
    {
        pCl->setProperty(pCl, PDB_PROP_CL_BUG_3562968_WAR_ALLOW_PCIE_ATOMICS, NV_TRUE);
    }
    // AMD Milan
    else if (pSys->cpuInfo.family == 0x19 &&
             pSys->cpuInfo.model == 0x1 &&
             pSys->cpuInfo.stepping == 0x1)
    {
        pCl->setProperty(pCl, PDB_PROP_CL_BUG_3562968_WAR_ALLOW_PCIE_ATOMICS, NV_TRUE);
    }
    // AMD Genoa
    else if (pSys->cpuInfo.family == 0x19 &&
             pSys->cpuInfo.model == 0x11)
    {
        pCl->setProperty(pCl, PDB_PROP_CL_BUG_3562968_WAR_ALLOW_PCIE_ATOMICS, NV_TRUE);
    }
    return;
}

//
// Determine which chipset we're using (from available options)
// and initialize chipset-specific functions
//
NV_STATUS
objClInitPcieChipset(OBJGPU *pGpu, OBJCL *pCl)
{
    OBJSYS    *pSys       = SYS_GET_INSTANCE();
    OBJOS     *pOS        = SYS_GET_OS(pSys);
    OBJPFM    *pPfm       = SYS_GET_PFM(pSys);
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
    NvU32      i;
    NvU32      domain;
    NvU16      chipsetInfoIndex;
    NvU32      devCap2;
    NvU32      devCtrl2;
    NvBool     rootPortLtrSupported;
    NvBool     tempLtrSupported;
    NvBool     needsNosnoopWAR = NV_FALSE;
    NV_STATUS  status;

    if (pGpu != NULL)
    {
        domain = gpuGetDomain(pGpu);

        if (clStoreBusTopologyCache(pCl, domain, PCI_MAX_BUSES) != NV_OK)
        {
            return NV_ERR_GENERIC;
        }
    }
    else
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (!pCl->ChipsetInitialized &&
        !pPfm->getProperty(pPfm, PDB_PROP_PFM_NO_HOSTBRIDGE_DETECT))
    {
        needsNosnoopWAR = clRootportNeedsNosnoopWAR_HAL(pGpu, pCl);

        if (needsNosnoopWAR)
            pCl->setProperty(pCl, PDB_PROP_CL_ROOTPORT_NEEDS_NOSNOOP_WAR, NV_TRUE);

        if (hypervisorIsVgxHyper())
        {
            pCl->setProperty(pCl, PDB_PROP_CL_NOSNOOP_NOT_CAPABLE, NV_TRUE);
        }
        // Find the first host bridge
        if (clFindFHBAndGetChipsetInfoIndex(pCl, &chipsetInfoIndex) == NV_OK)
        {
            pCl->Chipset = chipsetInfo[chipsetInfoIndex].chipset;
            // If the chipset info is not found, chipsetInfo[chipsetInfoIndex].setupFunc = NULL
            if ((chipsetInfo[chipsetInfoIndex].setupFunc != NULL) &&
                (chipsetInfo[chipsetInfoIndex].setupFunc(pCl) != NV_OK))
            {
                NV_PRINTF(LEVEL_ERROR, "*** Chipset Setup Function Error!\n");
            }
        }
        else
        {
            NV_PRINTF(LEVEL_WARNING,
                      "*** Chipset has no definition! (vendor ID 0x%04x, device ID 0x%04x)\n",
                      pCl->FHBBusInfo.vendorID, pCl->FHBBusInfo.deviceID);
        }
        if (pOS->getProperty(pOS, PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS))
        {
            pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_SKIP_MCFG_READ, NV_TRUE);
            pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE, NV_TRUE);
        }

        if (NVCPU_IS_FAMILY_ARM)
        {
            for (i=0; armChipsetAllowListInfo[i].vendorID; i++)
            {
                if (pCl->FHBBusInfo.vendorID == armChipsetAllowListInfo[i].vendorID &&
                    pCl->FHBBusInfo.deviceID == armChipsetAllowListInfo[i].deviceID)
                {
                    break;
                }
            }

            if (armChipsetAllowListInfo[i].vendorID == 0)
            {
                portDbgPrintf(
                    "NVRM: Chipset not recognized (vendor ID 0x%04x, device ID 0x%04x)\n",
                    pCl->FHBBusInfo.vendorID, pCl->FHBBusInfo.deviceID);

                // Allow the driver to run on AARCH64 even when the chipset is not matched,
                // but we need a disclaimer message printed...
                portDbgPrintf(
                    "The NVIDIA GPU driver for AArch64 has not been qualified on this platform\n"
                    "and therefore it is not recommended or intended for use in any production\n"
                    "environment.\n");
            }
        }

#if !defined(NVCPU_PPC64LE)
        //
        // If PCIe config space base addresses are not found through chipset specific
        // setup functions, read pcie config space addresses from the MCFG table and
        // update pCl->pPcieConfigSpaceBase linked-list.
        // Skip reading MCFG table for old SLI chipsets.
        //

        if (!(pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE)       &&
             (pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_SKIP_MCFG_READ))) &&
             !(IS_SIM_MODS(GPU_GET_OS(pGpu))))
        {
            if (clStorePcieConfigSpaceBaseFromMcfg(pCl) == NV_OK)
            {
                pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE, NV_TRUE);
            }
        }
#endif // !defined(NVCPU_PPC64LE)

        _objClPostSetupFuncRegkeyOverrides(pGpu, pCl);
        pCl->ChipsetInitialized = NV_TRUE;

        //
        // Verify PCI Express Enhanced configuration space of First Host Bridge
        // through vendor ID and device ID
        //

        if (pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE))
        {
            if (clPcieReadDword(pCl,
                                pCl->FHBAddr.domain,
                                pCl->FHBAddr.bus,
                                pCl->FHBAddr.device,
                                pCl->FHBAddr.func,
                                0)
               != (NvU32)(pCl->FHBBusInfo.vendorID | pCl->FHBBusInfo.deviceID << 16))
            {
                NV_PRINTF(LEVEL_ERROR,
                          "*** PCI-E config space not consistent with PCI config space, FHB vendor ID and device ID not equal!\n");
                NV_PRINTF(LEVEL_ERROR,
                          "*** Setting PCI-E config space inaccessible!\n");
                pCl->setProperty(pCl,PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE, NV_FALSE);
                clFreePcieConfigSpaceBase(pCl);
            }
        }
    }
    else if (pPfm->getProperty(pPfm, PDB_PROP_PFM_NO_HOSTBRIDGE_DETECT))
    {
        NV_PRINTF(LEVEL_INFO,
                  "Skipping PCI Express host bridge initialization\n");
    }

    // handles for ports
    if (!objClInitGpuPortData(pGpu, pCl))
    {
        NV_PRINTF(LEVEL_ERROR, "*** Unable to get PCI port handles\n");
        return NV_ERR_OPERATING_SYSTEM;
    }

    if (!pOS->getProperty(pOS, PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS))
    {
        objClGpuMapEnhCfgSpace(pGpu, pCl);
        objClGpuMapRootPort(pGpu, pCl);
    }

    domain = pGpu->gpuClData.upstreamPort.addr.domain;

    OBJHWBC *pHWBC = pCl->pHWBC;
    NvU8 childBus = pGpu->gpuClData.upstreamPort.addr.bus;

    while (pHWBC != NULL)
    {
        if ((pHWBC->domain == domain) && (pHWBC->minBus <= childBus) && (pHWBC->maxBus >= childBus))
        {
            if (HWBC_PLX_PEX8747 == pHWBC->bcRes)
            {
                plxPex8747GetFirmwareInfo(pCl, pGpu, pHWBC);

                //
                // Check to avoid unsetting the flag when GPU has more than
                // 1 PLX in its upstream. This happens when motherboard has
                // a plx bridge.
                //
                if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_PLX_PRESENT))
                {
                    // Set property only if the PLX has our firmware flashed.
                    pGpu->setProperty(pGpu, PDB_PROP_GPU_IS_PLX_PRESENT,
                                                        pHWBC->hasPlxFirmwareInfo);
                }
            }
            NV_ASSERT_OK_OR_RETURN(addHwbcToList(pGpu, pHWBC));
            pHWBC = pHWBC->pFirstChild;
        }
        else
            pHWBC = pHWBC->pSibling;
    }

    for (i=0; rootPortInfo[i].vendorID; i++)
        if (pGpu->gpuClData.rootPort.VendorID == rootPortInfo[i].vendorID &&
            pGpu->gpuClData.rootPort.DeviceID == rootPortInfo[i].deviceID)
            break;

    if (rootPortInfo[i].setupFunc)
    {
        if (NV_OK != rootPortInfo[i].setupFunc(pGpu, pCl)) {
            NV_PRINTF(LEVEL_ERROR, "*** Root Port Setup Function Error\n");
        }
    }

    for (i = 0; upstreamPortInfo[i].vendorID; i++)
    {
        if (pGpu->gpuClData.upstreamPort.VendorID == upstreamPortInfo[i].vendorID &&
            pGpu->gpuClData.upstreamPort.DeviceID == upstreamPortInfo[i].deviceID)
        {
            break;
        }
    }

    if (upstreamPortInfo[i].setupFunc)
    {
        if (NV_OK != upstreamPortInfo[i].setupFunc(pGpu, pCl))
        {
            NV_PRINTF(LEVEL_ERROR, "Upstream port Setup Function Error\n");
        }
    }

    // Verify PCI Express Enhanced configuration space through vendor ID and device ID
    if (pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE) &&
        pGpu->gpuClData.rootPort.addr.valid)
    {
        if (clPcieReadDword(pCl,
               pGpu->gpuClData.rootPort.addr.domain,
               pGpu->gpuClData.rootPort.addr.bus,
               pGpu->gpuClData.rootPort.addr.device,
               pGpu->gpuClData.rootPort.addr.func,
               0)
           != (NvU32)(pGpu->gpuClData.rootPort.VendorID | pGpu->gpuClData.rootPort.DeviceID << 16))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "*** PCI-E config space not consistent with PCI config space, root port vendor ID and device ID not equal!\n");
            pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE, NV_FALSE);
            clFreePcieConfigSpaceBase(pCl);
        }
    }

    rootPortLtrSupported = NV_FALSE;
    pCl->setProperty(pCl, PDB_PROP_CL_UPSTREAM_LTR_SUPPORTED, NV_FALSE);

    if (pGpu->gpuClData.rootPort.addr.valid)
    {
        if ((NV_OK == clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.rootPort,
                                                CL_PCIE_DEV_CAP_2, &devCap2)) &&
            (NV_OK == clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.rootPort,
                                                CL_PCIE_DEV_CTRL_2, &devCtrl2)))
        {
            if (CL_IS_LTR_PORT_SUPPORTED(devCap2) && CL_IS_LTR_PORT_ENABLED(devCtrl2))
            {
                rootPortLtrSupported = NV_TRUE;
            }
        }
    }

    //
    // No need to check upstream components' LTR support if root port
    // does not support LTR
    //
    if (rootPortLtrSupported)
    {
        status = clCheckUpstreamLtrSupport(pGpu, pCl, &tempLtrSupported);
        if (status == NV_OK && tempLtrSupported == NV_TRUE)
        {
            pCl->setProperty(pCl, PDB_PROP_CL_UPSTREAM_LTR_SUPPORTED, NV_TRUE);
        }
        else
        {
            NV_PRINTF(LEVEL_INFO, "LTR capability not supported.\n");
        }
    }

    // Cache L1SS enablement info from chipset side
    kbifCacheChipsetL1SubstatesEnable(pGpu, pKernelBif);

    return NV_OK;
}

/*! @brief Check LTR capability throughout the hierarchy of
 *         switches in between root port and endpoint.
 *
 * @param[i]   pGpu              GPU object pointer
 * @param[i]   pCl               Core logic object pointer
 * @param[out] pTempLtrSupported True if LTR is supported
 */
NV_STATUS
clCheckUpstreamLtrSupport_IMPL
(
    OBJGPU  *pGpu,
    OBJCL   *pCl,
    NvBool  *pTempLtrSupported
)
{
    NvU32      portCaps   = 0;
    NvU32      domain     = gpuGetDomain(pGpu);
    NvU8       bus        = gpuGetBus(pGpu);
    NvU32      PCIECapPtr;
    void      *pHandleUp;
    NvU8       busUp, devUp, funcUp;
    NvU16      vendorIDUp, deviceIDUp;
    NvU32      devCap2;
    NvU32      devCtrl2;
    NV_STATUS  status  = NV_OK;
    *pTempLtrSupported = NV_FALSE;

    if (!pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE))
    {
        {
            NV_PRINTF(LEVEL_ERROR, "PCIE config space is inaccessible!\n");
            status = NV_ERR_NOT_SUPPORTED;
            goto clCheckUpstreamLtrSupport_exit;
        }
    }

    do
    {
        // find upstream port
        pHandleUp = clFindP2PBrdg(pCl, domain, bus,
                                  &busUp, &devUp, &funcUp,
                                  &vendorIDUp, &deviceIDUp);

        // make sure handle was found
        if (!pHandleUp)
        {
            status = NV_ERR_NOT_SUPPORTED;
            goto clCheckUpstreamLtrSupport_exit;
        }

        status = clSetPortPcieCapOffset(pCl, pHandleUp, &PCIECapPtr);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_INFO, "Capability pointer not found.\n");
            status = NV_ERR_NOT_SUPPORTED;
            goto clCheckUpstreamLtrSupport_exit;
        }

        devCap2  = osPciReadDword(pHandleUp,
                                  CL_PCIE_DEV_CAP_2  - CL_PCIE_BEGIN + PCIECapPtr);
        devCtrl2 = osPciReadDword(pHandleUp,
                                  CL_PCIE_DEV_CTRL_2 - CL_PCIE_BEGIN + PCIECapPtr);

        if ((!CL_IS_LTR_PORT_SUPPORTED(devCap2)) ||
            (!CL_IS_LTR_PORT_ENABLED(devCtrl2)))
        {
            //
            // Even if a single switch in the hierarchy doesn't support
            // LTR, it has to be disabled. No need to check further.
            //
            *pTempLtrSupported = NV_FALSE;
            status = NV_ERR_NOT_SUPPORTED;
            goto clCheckUpstreamLtrSupport_exit;
        }
        portCaps = osPciReadDword(pHandleUp,
                                  CL_PCIE_CAP - CL_PCIE_BEGIN + PCIECapPtr);

        bus = busUp;
    } while (!CL_IS_ROOT_PORT(portCaps));
    *pTempLtrSupported = NV_TRUE;
    status = NV_OK;

clCheckUpstreamLtrSupport_exit:
    return status;
}

/*! @brief Check PCIe Atomics capability throughout the hierarchy of
 *         switches in between root port and endpoint.
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pCl         Core logic object pointer
 * @param[out] pAtomicMask Mask of supported atomic size, including one or more of:
 *                         OS_PCIE_CAP_MASK_REQ_ATOMICS_32
 *                         OS_PCIE_CAP_MASK_REQ_ATOMICS_64
 *                         OS_PCIE_CAP_MASK_REQ_ATOMICS_128
 *
 * @return NV_OK if PCIe Atomics is supported throughout the hierarchy, else
 *         NV_ERR_NOT_SUPPORTED
 */
NV_STATUS
clGetAtomicTypesSupported_IMPL
(
    NvU32  domain,
    NvU8   bus,
    OBJCL *pCl,
    NvU32 *pAtomicMask
)
{
    NvU32      portCaps    = 0;
    NvBool     bRoutingCap = NV_TRUE;
    NV_STATUS  status      = NV_OK;
    NvU32      PCIECapPtr;
    void      *pHandleUp;
    NvU8       busUp, devUp, funcUp;
    NvU16      vendorIDUp, deviceIDUp;

    do
    {
        // find virtual P2P bridge
        pHandleUp = clFindP2PBrdg(pCl, domain, bus,
                                  &busUp, &devUp, &funcUp,
                                  &vendorIDUp, &deviceIDUp);

        // make sure handle was found
        if (!pHandleUp)
        {
            status = NV_ERR_NOT_SUPPORTED;
            goto clGetAtomicTypesSupported_exit;
        }

        status = clSetPortPcieCapOffset(pCl, pHandleUp, &PCIECapPtr);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_INFO, "Capability pointer not found.\n");
            status = NV_ERR_NOT_SUPPORTED;
            goto clGetAtomicTypesSupported_exit;
        }

        // Read PCIe Capability
        portCaps = osPciReadDword(pHandleUp, CL_PCIE_CAP - CL_PCIE_BEGIN + PCIECapPtr);

        if (CL_IS_ROOT_PORT(portCaps))
        {
            _objClGetDownstreamAtomicsEnabledMask(pHandleUp, PCIECapPtr, pAtomicMask);
        }
        else if (CL_IS_UPSTREAM_PORT(portCaps))
        {
            _objClGetUpstreamAtomicRoutingCap(pHandleUp, PCIECapPtr, &bRoutingCap);
            if (!bRoutingCap)
            {
                status = NV_ERR_NOT_SUPPORTED;
                goto clGetAtomicTypesSupported_exit;
            }
        }
        else if (CL_IS_DOWNSTREAM_PORT(portCaps))
        {
            _objClGetDownstreamAtomicRoutingCap(pHandleUp, PCIECapPtr, &bRoutingCap);
            if (!bRoutingCap)
            {
                status = NV_ERR_NOT_SUPPORTED;
                goto clGetAtomicTypesSupported_exit;
            }
        }
        else
        {
            // Invalid port
            status = NV_ERR_NOT_SUPPORTED;
            goto clGetAtomicTypesSupported_exit;
        }

        bus = busUp;
    } while (!CL_IS_ROOT_PORT(portCaps));

clGetAtomicTypesSupported_exit:
    return status;
}

/*!
 * @brief Get the supported Atomics mask bit configuration for root port
 *
 * @param[in]  pHandle     Handle for the P2P bridge
 * @param[in]  PCIECapPtr  PCIe Capability pointer
 * @param[out] pAtomicMask Mask of supported atomic size
 */
static void
_objClGetDownstreamAtomicsEnabledMask
(
    void  *pHandle,
    NvU32  PCIECapPtr,
    NvU32 *pAtomicMask
)
{
    NvU32 devCap2;

    devCap2  = osPciReadDword(pHandle,
                              CL_PCIE_DEV_CAP_2  - CL_PCIE_BEGIN + PCIECapPtr);
    if (CL_IS_32BIT_ATOMICS_SUPPORTED(devCap2))
    {
        *pAtomicMask |= CL_ATOMIC_32BIT;
    }
    if (CL_IS_64BIT_ATOMICS_SUPPORTED(devCap2))
    {
        *pAtomicMask |= CL_ATOMIC_64BIT;
    }
    if (CL_IS_128BIT_ATOMICS_SUPPORTED(devCap2))
    {
        *pAtomicMask |= CL_ATOMIC_128BIT;
    }
}

/*!
 * @brief Check if upstream port is capable of atomic routing
 *        and whether egress is blocked
 *
 * @param[in]  pHandle      Handle for the P2P bridge
 * @param[in]  PCIECapPtr   PCIe Capability pointer
 * @param[out] pbRoutingCap Atomic routing capable
 */
static void
_objClGetUpstreamAtomicRoutingCap
(
    void   *pHandle,
    NvU32   PCIECapPtr,
    NvBool *pbRoutingCap
)
{
    NvU32 devCap2;
    NvU32 devCtrl2;

    devCap2  = osPciReadDword(pHandle,
                              CL_PCIE_DEV_CAP_2  - CL_PCIE_BEGIN + PCIECapPtr);
    devCtrl2 = osPciReadDword(pHandle,
                              CL_PCIE_DEV_CTRL_2 - CL_PCIE_BEGIN + PCIECapPtr);

    if ((!CL_IS_ATOMICS_SUPPORTED(devCap2)) ||
        (CL_IS_ATOMICS_EGRESS_BLOCKED(devCtrl2)))
    {
        *pbRoutingCap = NV_FALSE;
    }
}

/*!
 * @brief Check if downstream port is capable of atomic routing
 *
 * @param[in]  pHandle      Handle for the P2P bridge
 * @param[in]  PCIECapPtr   PCIe Capability pointer
 * @param[out] pbRoutingCap Atomic routing capable
 */
static void
_objClGetDownstreamAtomicRoutingCap
(
    void   *pHandle,
    NvU32   PCIECapPtr,
    NvBool *pbRoutingCap
)
{
    NvU32 devCap2;

    devCap2  = osPciReadDword(pHandle,
                              CL_PCIE_DEV_CAP_2  - CL_PCIE_BEGIN + PCIECapPtr);

    if (!CL_IS_ATOMICS_SUPPORTED(devCap2))
    {
        *pbRoutingCap = NV_FALSE;
    }
}

static void
_objClAdjustTcVcMap(OBJGPU *pGpu, OBJCL *pCl, PORTDATA *pPort)
{
    NvU32 epVcCtrl0, epTcVcMap, upTcVcMap, upVcCtrl0, subsetTcVcMap;

    // read port TC/VC map
    if (NV_OK != clPcieReadPortConfigReg(pGpu, pCl, pPort,
                                            CL_VC_RESOURCE_CTRL_0, &upVcCtrl0))
    {
        NV_PRINTF(LEVEL_INFO,
                  "NVPCIE: Can not read VC resource control 0 on port %04x:%02x:%02x.%1x (bug 1048498).\n",
                  pPort->addr.domain, pPort->addr.bus, pPort->addr.device,
                  pPort->addr.func);
    }
    else
    {
        upTcVcMap = DRF_VAL(_XVE, _VCCAP_CTRL0, _MAP, upVcCtrl0);

        // read NVIDIA TC/VC map
        if (NV_OK != GPU_BUS_CFG_RD32(pGpu, NV_XVE_VCCAP_CTRL0, &epVcCtrl0))
        {
             NV_PRINTF(LEVEL_ERROR, "Cannot read NV_XVE_VCCAP_CTRL0\n");
             return;
        }
        epTcVcMap = DRF_VAL(_XVE, _VCCAP_CTRL0, _MAP, epVcCtrl0);

        subsetTcVcMap = epTcVcMap & upTcVcMap;
        if (epTcVcMap != subsetTcVcMap)
        {
            NV_PRINTF(LEVEL_INFO,
                      "NVPCIE: TC/VC map is inconsistent (Port %04x:%02x:%02x.%1x 0x%02x, GPU 0x%02x)!\n",
                      pPort->addr.domain, pPort->addr.bus, pPort->addr.device,
                      pPort->addr.func, (upTcVcMap << 1) | 1,
                      (epTcVcMap << 1) | 1);

            NV_PRINTF(LEVEL_INFO,
                      "NVPCIE: Fixing TC/VC map to common subset 0x%02x.\n",
                      (subsetTcVcMap << 1) | 1);

            epVcCtrl0 = FLD_SET_DRF_NUM(_XVE, _VCCAP_CTRL0, _MAP, subsetTcVcMap,
                                            epVcCtrl0);
            GPU_BUS_CFG_WR32(pGpu, NV_XVE_VCCAP_CTRL0, epVcCtrl0);
        }
    }
}


#define MAX_MULTI_GPU_BOARD_IDS   4
typedef struct
{
    NvU16 gpuDevIds[MAX_MULTI_GPU_BOARD_IDS];
    NvU16 gpuSubVenIds[MAX_MULTI_GPU_BOARD_IDS];
    NvU16 gpuSubDevIds[MAX_MULTI_GPU_BOARD_IDS];
} NV_MULTI_GPU_BOARD_CONFIGS;

static const NV_MULTI_GPU_BOARD_CONFIGS multiGpuBoards[] =
{
// gpuDevIds,                       gpuSubVenIds,                   gpuSubDevIds

// A16 GPUs are 4xGPU boards with no nvlink
{{NV_PCI_DEVID_DEVICE_PG171_SKU200_PG179_SKU220, 0},
                                    {NV_PCI_SUBID_VENDOR_NVIDIA,
                                     0},                            {NV_PCI_SUBID_DEVICE_PG171_SKU200,
                                                                     0}},
};

/*!
 * Searches through multiGpuBoards[] for the specified DEVID and possibly the
 * SSVID and SSDID to determine if the GPU is in a multi GPU board.
 *
 * @param[in]  gpuDevId    DEVID of the GPU
 * @param[in]  gpuSubVenId The subdevice VENID of the GPU
 * @param[in]  gpuSubDevId The sbudevice DEVID of the GPU
 *
 * @return NV_TRUE if the GPU is in a multigpu board, NV_FALSE otherwise
 */
static NvBool
gpuDevIdIsMultiGpuBoard
(
    NvU16   gpuDevId,
    NvU16   gpuSubVenId,
    NvU16   gpuSubDevId
)
{
    NvU32 i, j;
    NvBool bFound = NV_FALSE, bInvalidSubIds = NV_FALSE;

    for (i = 0; i < NV_ARRAY_ELEMENTS(multiGpuBoards);
         i++)
    {
        bInvalidSubIds = NV_FALSE;

        for (j = 0; j < MAX_MULTI_GPU_BOARD_IDS; j++)
        {
            //
            // As soon as we see a NULL subvenid or subdevid, we stop checking them
            // for this entry
            //
            if ((multiGpuBoards[i].gpuSubVenIds[j] == 0) ||
                (multiGpuBoards[i].gpuSubDevIds[j] == 0))
            {
                bInvalidSubIds = NV_TRUE;
            }

            // Arrays are NULL terminated
            if (multiGpuBoards[i].gpuDevIds[j] == 0)
            {
                break;
            }
            // If the devid matches we might have a match
            else if (multiGpuBoards[i].gpuDevIds[j] == gpuDevId)
            {
                // Do we need to also compare the subdevice ids?
                if ((bInvalidSubIds) ||
                    ((multiGpuBoards[i].gpuSubVenIds[j] == gpuSubVenId) &&
                     (multiGpuBoards[i].gpuSubDevIds[j] == gpuSubDevId)))
                {
                    bFound = NV_TRUE;
                    break;
                }
            }
        }

        if (bFound)
        {
            break;
        }
    }

    return bFound;
}


//
// Determine if any updates are needed to the PCI Express
//
void
clUpdatePcieConfig_IMPL(OBJGPU *pGpu, OBJCL *pCl)
{
    OBJSYS    *pSys       = SYS_GET_INSTANCE();
    OBJPFM    *pPfm       = SYS_GET_PFM(pSys);
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
    NvBool     bIsMultiGpu;
    NvU32      busIntfType = kbifGetBusIntfType_HAL(pKernelBif);

    // verify we're an PCI Express graphics card
    if (busIntfType != NV2080_CTRL_BUS_INFO_TYPE_PCI_EXPRESS &&
        busIntfType != NV2080_CTRL_BUS_INFO_TYPE_FPCI)
    {
        return;
    }

    // Find our NV device on the PCI bus and save it's pci bus/device address.
    (void)clInitMappingPciBusDevice(pGpu, pCl);
    NV_PRINTF(LEVEL_INFO,
              "GPU Domain %X Bus %X Device %X Func %X\n",
              gpuGetDomain(pGpu), gpuGetBus(pGpu), gpuGetDevice(pGpu), 0);

    // Set 3d controller property
    if (objClGpuIs3DController(pGpu) != NV_OK)
    {
        return;
    }

    // Load PCI Express virtual P2P approval config
    objClLoadPcieVirtualP2PApproval(pGpu);

    //
    // Disable NOSNOOP bit for Passthrough.
    //
    if (IS_PASSTHRU(pGpu))
    {
        pCl->setProperty(pCl, PDB_PROP_CL_NOSNOOP_NOT_CAPABLE, NV_TRUE);
    }

    objClBuildPcieAtomicsAllowList(pGpu, pCl);

    objClInitPcieChipset(pGpu, pCl);

    //
    // Now that chipset capabilities have been initialized, configure the
    // device's config space device control status bits to match. This should
    // happen before any DMA.
    //
    kbifInitPcieDeviceControlStatus(pGpu, pKernelBif);

    //
    // Probe root port PCIe atomic capabilities.
    // kbifProbePcieReqAtomicCaps_HAL should be called from here instead of
    // kbif construct because of the dependency on the chipset
    // discovery to build the allow list for enabling PCIe atomics feature.
    //
    kbifProbePcieReqAtomicCaps_HAL(pGpu, pKernelBif);
    //
    // Read device atomic completer capabilities early so they can be
    // passed to GSP.
    //
    kbifProbePcieCplAtomicCaps_HAL(pGpu, pKernelBif);

    //
    // Passthrough configurations do not typically present the upstream
    // bridge required for detecting multi-GPU boards. So for hypervisors
    // with passthrough pretend to separate GPUs.
    //
    if (!pPfm->getProperty(pPfm, PDB_PROP_PFM_NO_HOSTBRIDGE_DETECT))
    {

        bIsMultiGpu = gpuIsMultiGpuBoard(pGpu);

        // Update the board ID only if the Gpu is a multiGpu board
        if (bIsMultiGpu)
        {
            gpumgrUpdateBoardId(pGpu);

            // All multi-GPU boards are Gemini boards.
            pGpu->setProperty(pGpu, PDB_PROP_GPU_IS_GEMINI, NV_TRUE);
        }
    }

    // Initialize ASPM L1 mask state for Upstream port
    if (clIsL1SupportedForUpstreamPort(pGpu, pCl))
    {
        pCl->setProperty(pCl, PDB_PROP_CL_ASPM_L1_UPSTREAM_PORT_SUPPORTED, NV_TRUE);
    }
    else
    {
        pCl->setProperty(pCl, PDB_PROP_CL_ASPM_L1_UPSTREAM_PORT_SUPPORTED, NV_FALSE);
    }

    NV_PRINTF(LEVEL_INFO,
              "Chipset %X Domain %X Bus %X Device %X Func %X PCIE PTR %X\n",
              pCl->Chipset,
              pGpu->gpuClData.upstreamPort.addr.domain,
              pGpu->gpuClData.upstreamPort.addr.bus,
              pGpu->gpuClData.upstreamPort.addr.device,
              pGpu->gpuClData.upstreamPort.addr.func,
              pGpu->gpuClData.upstreamPort.PCIECapPtr);
    NV_PRINTF(LEVEL_INFO,
              "Chipset %X Root Port Domain %X Bus %X Device %X Func %X PCIE PTR %X\n",
              pCl->Chipset, pGpu->gpuClData.rootPort.addr.domain,
              pGpu->gpuClData.rootPort.addr.bus,
              pGpu->gpuClData.rootPort.addr.device,
              pGpu->gpuClData.rootPort.addr.func,
              pGpu->gpuClData.rootPort.PCIECapPtr);
    NV_PRINTF(LEVEL_INFO,
              "Chipset %X Board Upstream Port Domain %X Bus %X Device %X Func %X PCIE PTR %X\n",
              pCl->Chipset, pGpu->gpuClData.boardUpstreamPort.addr.domain,
              pGpu->gpuClData.boardUpstreamPort.addr.bus,
              pGpu->gpuClData.boardUpstreamPort.addr.device,
              pGpu->gpuClData.boardUpstreamPort.addr.func,
              pGpu->gpuClData.boardUpstreamPort.PCIECapPtr);
    NV_PRINTF(LEVEL_INFO,
              "Chipset %X Board Downstream Port Domain %X Bus %X Device %X Func %X PCIE PTR %X\n",
              pCl->Chipset, pGpu->gpuClData.boardDownstreamPort.addr.domain,
              pGpu->gpuClData.boardDownstreamPort.addr.bus,
              pGpu->gpuClData.boardDownstreamPort.addr.device,
              pGpu->gpuClData.boardDownstreamPort.addr.func,
              pGpu->gpuClData.boardDownstreamPort.PCIECapPtr);

    NV_PRINTF(LEVEL_INFO,
              "FHB Domain %X Bus %X Device %X Func %X VendorID %X DeviceID %X\n",
              pCl->FHBAddr.domain, pCl->FHBAddr.bus, pCl->FHBAddr.device,
              pCl->FHBAddr.func, pCl->FHBBusInfo.vendorID,
              pCl->FHBBusInfo.deviceID);

    //
    // Match the GPU TC/VC map to that of the chipset
    // Some SBIOS revisions do not program the TC/VC map correctly. RM will
    // always ensure that the GPU TC/VC map is a subset of the RP TC/VC map.
    // The GPU's map can be more restrictive than the RP, but it can *never*
    // be larger.
    //
    // Virtual channel capability is no longer supported for GH100
    // Bug 200570282, comment#41
    //
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_VC_CAPABILITY_SUPPORTED))
    {
        _objClAdjustTcVcMap(pGpu, pCl, &pGpu->gpuClData.rootPort);

        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_BEHIND_BRIDGE))
        {
            //
            // Match the GPU TC/VC map to that of the parent bridge.
            // Normally, the SBIOS programs up these mappings during POST.
            // However, this does not happen when coming out of Zero Power.
            //
            _objClAdjustTcVcMap(pGpu, pCl, &pGpu->gpuClData.upstreamPort);
        }
    }
}

//
// Here's our attempt to configure PCI Express on our own (for NT4?).
//
// First, we'll attempt to recognize the chipset to determine if we know how to
// program it.
//
NV_STATUS clInitPcie_IMPL
(
    OBJGPU *pGpu,
    OBJCL  *pCl
)
{
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
    NvU32   busIntfType;

    if (pKernelBif == NULL)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    busIntfType = kbifGetBusIntfType_HAL(GPU_GET_KERNEL_BIF(pGpu));

    // verify we're an PCI Express graphics card
    if (busIntfType != NV2080_CTRL_BUS_INFO_TYPE_PCI_EXPRESS &&
        busIntfType != NV2080_CTRL_BUS_INFO_TYPE_FPCI)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    /* enable chipset-specific overrides */
    clUpdatePcieConfig(pGpu, pCl);

    return NV_OK;
}

NV_STATUS
clTeardownPcie_IMPL
(
    OBJGPU       *pGpu,
    OBJCL        *pCl
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJOS *pOS = SYS_GET_OS(pSys);
    if (!pOS->getProperty(pOS, PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS))
    {
        objClGpuUnmapRootPort(pGpu);
        objClGpuUnmapEnhCfgSpace(pGpu);
    }
    return NV_OK;
}

//
// Return the Bus, Device, Func numbers of the root port for the given GPU.
//
static NvBool
objClInitGpuPortData
(
    OBJGPU *pGpu,
    OBJCL  *pCl
)
{
    void *handle;
    NvU16 vendorID;
    NvU16 deviceID;
    NvU8  bus;
    NvU8  device;
    NvU8  func;
    NvU8  gpuBus;
    NvU32 domain = 0;
    NvU32 linkCap;

    // return it, if we've got it already
    if (pGpu->gpuClData.upstreamPort.addr.valid)
        return NV_TRUE;

    NV_ASSERT(gpuIsDBDFValid(pGpu));

    domain = gpuGetDomain(pGpu);
    gpuBus = gpuGetBus(pGpu);

    handle = clFindP2PBrdg(pCl, domain, gpuBus, &bus, &device, &func,
                           &vendorID, &deviceID);

    if (!handle)
    {
        OBJSYS *pSys = SYS_GET_INSTANCE();
        OBJHYPERVISOR *pHypervisor = SYS_GET_HYPERVISOR(pSys);

        // This is bad, we didn't find the upstream port device (assume
        // bus0/device0/func0)
        pGpu->gpuClData.upstreamPort.addr.domain = 0x0;
        pGpu->gpuClData.upstreamPort.addr.bus    = 0x0;
        pGpu->gpuClData.upstreamPort.addr.device = 0x0;
        pGpu->gpuClData.upstreamPort.addr.func   = 0x0;
        pGpu->gpuClData.upstreamPort.addr.valid  = 0x1;
        pGpu->gpuClData.upstreamPort.addr.handle = NULL;
        pGpu->gpuClData.upstreamPort.DeviceID    = PCI_INVALID_DEVICEID;
        pGpu->gpuClData.upstreamPort.VendorID    = PCI_INVALID_VENDORID;

        //
        // Hypervisors typically hide the PCI topology from the guest, so we
        // don't expect to always be able to find an upstream port.
        //
        // For MODS debug breakpoints are always fatal and MODS is sometimes run
        // on systems where the up stream port cannot be determined
        if ((kbifGetBusIntfType_HAL(GPU_GET_KERNEL_BIF(pGpu)) !=
             NV2080_CTRL_BUS_INFO_TYPE_FPCI) &&
            (!pHypervisor || !pHypervisor->bDetected) &&
            !RMCFG_FEATURE_MODS_FEATURES)
        {
            DBG_BREAKPOINT();
            return NV_FALSE;
        }
    }
    else
    {
        pGpu->gpuClData.upstreamPort.addr.domain = domain;
        pGpu->gpuClData.upstreamPort.addr.bus    = bus;
        pGpu->gpuClData.upstreamPort.addr.device = device;
        pGpu->gpuClData.upstreamPort.addr.func   = func;
        pGpu->gpuClData.upstreamPort.addr.valid  = 0x1;
        pGpu->gpuClData.upstreamPort.addr.handle = handle;
        pGpu->gpuClData.upstreamPort.DeviceID    = deviceID;
        pGpu->gpuClData.upstreamPort.VendorID    = vendorID;

        objClSetPortCapsOffsets(pCl, &pGpu->gpuClData.upstreamPort);
    }

    // Root port
    handle = objClFindRootPort(pGpu, pCl, domain, gpuBus, &bus, &device, &func, &vendorID, &deviceID);
    if (handle)
    {
        pGpu->gpuClData.rootPort.addr.domain = domain;
        pGpu->gpuClData.rootPort.addr.bus    = bus;
        pGpu->gpuClData.rootPort.addr.device = device;
        pGpu->gpuClData.rootPort.addr.func   = func;
        pGpu->gpuClData.rootPort.addr.valid  = 0x1;
        pGpu->gpuClData.rootPort.addr.handle = handle;
        pGpu->gpuClData.rootPort.DeviceID    = deviceID;
        pGpu->gpuClData.rootPort.VendorID    = vendorID;

        objClSetPortCapsOffsets(pCl, &pGpu->gpuClData.rootPort);
    }
    else
    {
        // This is a valid topology if a PCIe-to-PCI bridge chip is being used.
        pGpu->gpuClData.rootPort.DeviceID  = PCI_INVALID_DEVICEID;
        pGpu->gpuClData.rootPort.VendorID  = PCI_INVALID_VENDORID;
    }

    // Assuming that both BR03 and BR04 can exist on the same system
    pGpu->setProperty(pGpu, PDB_PROP_GPU_IS_BR03_PRESENT, objClBR03Exists(pGpu, pCl));
    pGpu->setProperty(pGpu, PDB_PROP_GPU_IS_BR04_PRESENT, objClBR04Exists(pGpu, pCl));

    // board downstream port
    domain = gpuGetDomain(pGpu);
    handle = clFindBrdgUpstreamPort(pGpu, pCl, NV_FALSE, &bus, &device, &func, &vendorID, &deviceID, NULL);
    if (handle)
    {
        pGpu->gpuClData.boardDownstreamPort.addr.domain = domain;
        pGpu->gpuClData.boardDownstreamPort.addr.bus    = bus;
        pGpu->gpuClData.boardDownstreamPort.addr.device = device;
        pGpu->gpuClData.boardDownstreamPort.addr.func   = func;
        pGpu->gpuClData.boardDownstreamPort.addr.valid  = 0x1;
        pGpu->gpuClData.boardDownstreamPort.addr.handle = handle;
        pGpu->gpuClData.boardDownstreamPort.DeviceID    = deviceID;
        pGpu->gpuClData.boardDownstreamPort.VendorID    = vendorID;

        objClSetPortCapsOffsets(pCl, &pGpu->gpuClData.boardDownstreamPort);
    }
    else if ( pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_BR03_PRESENT) ||
              pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_BR04_PRESENT) ) // Error
    {
        pGpu->gpuClData.boardDownstreamPort.DeviceID  = PCI_INVALID_DEVICEID;
        pGpu->gpuClData.boardDownstreamPort.VendorID  = PCI_INVALID_VENDORID;
        DBG_BREAKPOINT();
        return NV_FALSE;
    }
    // else: boardDownstreamPort data is not initialized, as it would
    // represent the GPU.  We do not access the PCIE config space of the GPU.

    // board upstream port
    domain = gpuGetDomain(pGpu);
    handle = clFindBrdgUpstreamPort(pGpu, pCl, NV_TRUE, &bus, &device, &func, &vendorID, &deviceID, NULL);
    if (handle)
    {
        pGpu->gpuClData.boardUpstreamPort.addr.domain = domain;
        pGpu->gpuClData.boardUpstreamPort.addr.bus    = bus;
        pGpu->gpuClData.boardUpstreamPort.addr.device = device;
        pGpu->gpuClData.boardUpstreamPort.addr.func   = func;
        pGpu->gpuClData.boardUpstreamPort.addr.valid  = 0x1;
        pGpu->gpuClData.boardUpstreamPort.addr.handle = handle;
        pGpu->gpuClData.boardUpstreamPort.DeviceID    = deviceID;
        pGpu->gpuClData.boardUpstreamPort.VendorID    = vendorID;

        objClSetPortCapsOffsets(pCl, &pGpu->gpuClData.boardUpstreamPort);
    }
    else if ( (!pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_BR03_PRESENT)) &&
              (!pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_BR04_PRESENT)) )
    {
        // No BR03, same as upstream port
        pGpu->gpuClData.boardUpstreamPort.addr.domain = pGpu->gpuClData.upstreamPort.addr.domain;
        pGpu->gpuClData.boardUpstreamPort.addr.bus    = pGpu->gpuClData.upstreamPort.addr.bus;
        pGpu->gpuClData.boardUpstreamPort.addr.device = pGpu->gpuClData.upstreamPort.addr.device;
        pGpu->gpuClData.boardUpstreamPort.addr.func   = pGpu->gpuClData.upstreamPort.addr.func;
        pGpu->gpuClData.boardUpstreamPort.addr.valid  = pGpu->gpuClData.upstreamPort.addr.valid;
        pGpu->gpuClData.boardUpstreamPort.addr.handle = pGpu->gpuClData.upstreamPort.addr.handle;
        pGpu->gpuClData.boardUpstreamPort.DeviceID    = pGpu->gpuClData.upstreamPort.DeviceID;
        pGpu->gpuClData.boardUpstreamPort.VendorID    = pGpu->gpuClData.upstreamPort.VendorID;

        pGpu->gpuClData.boardUpstreamPort.PCIECapPtr       = pGpu->gpuClData.upstreamPort.PCIECapPtr;
        pGpu->gpuClData.boardUpstreamPort.PCIEErrorCapPtr  = pGpu->gpuClData.upstreamPort.PCIEErrorCapPtr;
        pGpu->gpuClData.boardUpstreamPort.PCIEVCCapPtr     = pGpu->gpuClData.upstreamPort.PCIEVCCapPtr;
        pGpu->gpuClData.boardUpstreamPort.PCIEL1SsCapPtr   = pGpu->gpuClData.upstreamPort.PCIEL1SsCapPtr;
    }
    else //Error
    {
        DBG_BREAKPOINT();
        return NV_FALSE;
    }

    objClSetPcieHWBC(pGpu, pCl);

    // set property to indicate whether the GPU is behind any bridge
    if (pGpu->gpuClData.rootPort.addr.valid && pGpu->gpuClData.upstreamPort.addr.valid)
    {
        if ((pGpu->gpuClData.rootPort.addr.domain != pGpu->gpuClData.upstreamPort.addr.domain) ||
            (pGpu->gpuClData.rootPort.addr.bus    != pGpu->gpuClData.upstreamPort.addr.bus) ||
            (pGpu->gpuClData.rootPort.addr.device != pGpu->gpuClData.upstreamPort.addr.device) ||
            (pGpu->gpuClData.rootPort.addr.func   != pGpu->gpuClData.upstreamPort.addr.func))
        {
            pGpu->setProperty(pGpu, PDB_PROP_GPU_BEHIND_BRIDGE, NV_TRUE);
        }
    }
    //
    // If GPU is behind bridge then to enable\disable ASPM L0S\L1 should
    // be decided based upon the link capability of the immediate bridge
    // with which GPU is connected (Bug 540109)
    //
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_BEHIND_BRIDGE))
    {
        if (pGpu->gpuClData.upstreamPort.addr.valid)
        {
            if (clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.upstreamPort,
                                           CL_PCIE_LINK_CAP, &linkCap) == NV_OK)
            {
                if (!CL_IS_L0_SUPPORTED(linkCap))
                {
                    pGpu->setProperty(pGpu,
                                      PDB_PROP_GPU_UPSTREAM_PORT_L0S_UNSUPPORTED,
                                      NV_TRUE);
                }
                if (!CL_IS_L1_SUPPORTED(linkCap))
                {
                    pGpu->setProperty(pGpu,
                                      PDB_PROP_GPU_UPSTREAM_PORT_L1_UNSUPPORTED,
                                      NV_TRUE);
                }
            }
            else
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Error reading pcie link control status of upstream port\n");
                // disable L0\L1 support by default
                pGpu->setProperty(pGpu, PDB_PROP_GPU_UPSTREAM_PORT_L0S_UNSUPPORTED, NV_TRUE);
                pGpu->setProperty(pGpu, PDB_PROP_GPU_UPSTREAM_PORT_L1_UNSUPPORTED, NV_TRUE);
            }
        }
    }

    //
    // We must check for specific ports on which we should always support ASPM L1.
    // This allows us to include ASPM L1 support on specific ports, such as PEX
    // slots coming off the CPU rather than the chipset.  POR for which to support
    // is generally determined by HW.  SW then adds it here.
    //

    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_UPSTREAM_PORT_L1_UNSUPPORTED))
    {
        NvU16 vendorID;
        NvU16 deviceID;

        if (pGpu->gpuClData.upstreamPort.addr.valid)
        {
            vendorID = pGpu->gpuClData.upstreamPort.VendorID;
            deviceID = pGpu->gpuClData.upstreamPort.DeviceID;
        }
        else if (pGpu->gpuClData.rootPort.addr.valid)
        {
            vendorID = pGpu->gpuClData.rootPort.VendorID;
            deviceID = pGpu->gpuClData.rootPort.DeviceID;
        }
        else
        {
            vendorID = PCI_INVALID_VENDORID;
            deviceID = PCI_INVALID_DEVICEID;
        }

        //
        // Any root ports not in our list remains off the supported list.  That
        // means ASPM L1 support will be dictated by the chipset POR as a whole,
        // and not on a per-root-port basis.
        //

        switch(vendorID)
        {
            case PCI_VENDOR_ID_INTEL:
                switch(deviceID)
                {
                    case INTEL_LYNNFIELD_ROOTPORT_CPU1:
                        // Bug 706926: Intel H57 Chipset (based on LPC D31 F0 PCI DevID 3B08h )
                        // need to remove from ASPM POR
                        if (pCl->getProperty(pCl, PDB_PROP_CL_INTEL_CPU_ROOTPORT1_NEEDS_H57_WAR))
                            break;

                    case INTEL_LYNNFIELD_ROOTPORT_CPU2:
                        pGpu->setProperty(pGpu, PDB_PROP_GPU_UPSTREAM_PORT_L1_POR_SUPPORTED, NV_TRUE);
                        break;

                    default:
                        // This device id is not in our per-root-port POR.
                        break;
                }
                break;

            default:
                // This vendor has no devices in our per-root-port POR.
                break;
        }
    }

    return NV_TRUE;
}

//
// Find out the P2P Bridge(port) which bridges to the secondary bus
//
void *
clFindP2PBrdg_IMPL
(
    OBJCL *pCl,
    NvU32 domain,
    NvU8  secBus,
    NvU8 *pbus,
    NvU8 *pdevice,
    NvU8 *pfunc,
    NvU16 *vendorID,
    NvU16 *deviceID
)
{
    PBUSTOPOLOGYINFO pBusTopologyInfo;
    OBJGPU *pGpu;
    NvU32 gpuAttachCnt, gpuAttachMask, gpuInstance;
    NvU16 secBus16 = secBus;

    gpumgrGetGpuAttachInfo(&gpuAttachCnt, &gpuAttachMask);
    gpuInstance = 0;
    pGpu = gpumgrGetNextGpu(gpuAttachMask, &gpuInstance);

    if (pGpu && !IS_SIMULATION(pGpu))
    {
        secBus16 = PCI_MAX_BUSES;
        // And we will store the PCI-E topology only once
    }
    else
    {
        // If pGPU is NULL we do not know if we are in simulation
        // I verified this does not happen. pGPU is always set.
        secBus16 = secBus;
        if (gpuAttachCnt > 1)
        {
            // Free the topology in this case only when the GPU count is greater than 1
            // Of we have only one GPU then we do not need to rescan the bus.
            clFreeBusTopologyCache(pCl);
        }
    }

    // If the bus topology is not cached, do it here
    if (clStoreBusTopologyCache(pCl, domain, secBus16) != NV_OK)
    {
        return NULL;
    }

    pBusTopologyInfo = pCl->pBusTopologyInfo;

    while (pBusTopologyInfo)
    {
        if ((pBusTopologyInfo->secBus == secBus) &&
            (pBusTopologyInfo->domain == domain) &&
            (pBusTopologyInfo->bus <= secBus - 1) &&
            (pBusTopologyInfo->bVgaAdapter == NV_FALSE))
        {
            *pbus       = pBusTopologyInfo->bus;
            *pdevice    = pBusTopologyInfo->device;
            *pfunc      = pBusTopologyInfo->func;
            *vendorID   = pBusTopologyInfo->busInfo.vendorID;
            *deviceID   = pBusTopologyInfo->busInfo.deviceID;
            return pBusTopologyInfo->handle;
        }
        pBusTopologyInfo = pBusTopologyInfo->next;
    }

    return NULL;
}

static void
objClGpuMapRootPort
(
    OBJGPU *pGpu,
    OBJCL *pCl
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJOS  *pOS = SYS_GET_OS(pSys);
    NBADDR *pRoot = &pGpu->gpuClData.rootPort.addr;
    void   *vAddr;
    RmPhysAddr pcieConfigSpaceBase;

    NV_ASSERT_OR_RETURN_VOID(!pOS->getProperty(pOS, PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS));

    if (!pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE)
        || !pRoot->valid
        || (pGpu->gpuClData.rootPort.vAddr != 0))
    {
        return;
    }

    pcieConfigSpaceBase = clFindPcieConfigSpaceBase(pCl, pRoot->domain, pRoot->bus);
    if (pcieConfigSpaceBase == 0)
    {
        return;
    }

    vAddr = osMapKernelSpace(pcieConfigSpaceBase |
                             pRoot->bus << PCIE_BUS_SHIFT |
                             pRoot->device << PCIE_DEVICE_SHIFT |
                             pRoot->func << PCIE_FUNC_SHIFT,
                             RM_PAGE_SIZE,
                             NV_MEMORY_UNCACHED,
                             NV_PROTECT_READ_WRITE);
    if (vAddr == 0)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "NVPCIE: unable to map root port PCIE config space.\n");
        return;
    }

    pGpu->gpuClData.rootPort.vAddr = vAddr;
}

static void
objClGpuUnmapRootPort
(
    OBJGPU *pGpu
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJOS  *pOS = SYS_GET_OS(pSys);

    NV_ASSERT_OR_RETURN_VOID(!pOS->getProperty(pOS, PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS));

    if (pGpu->gpuClData.rootPort.vAddr != 0)
    {
        osUnmapKernelSpace(pGpu->gpuClData.rootPort.vAddr, RM_PAGE_SIZE);
        pGpu->gpuClData.rootPort.vAddr = 0;
    }
}

static void
objClGpuMapEnhCfgSpace
(
    OBJGPU *pGpu,
    OBJCL *pCl
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJOS *pOS = SYS_GET_OS(pSys);
    NvU8 bus;
    NvU8 device;
    NvU32 domain;
    RmPhysAddr pcieConfigSpaceBase;

    NV_ASSERT_OR_RETURN_VOID(!pOS->getProperty(pOS, PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS));

    if (!pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE) ||
        (pGpu->gpuCfgAddr != NULL) || !gpuIsDBDFValid(pGpu))
    {
        return;
    }

    domain = gpuGetDomain(pGpu);
    bus = gpuGetBus(pGpu);
    device = gpuGetDevice(pGpu);

    pcieConfigSpaceBase = clFindPcieConfigSpaceBase(pCl, domain, bus);
    if (pcieConfigSpaceBase == 0)
    {
        return;
    }

    pGpu->gpuCfgAddr = osMapKernelSpace(pcieConfigSpaceBase |
                                        (bus << PCIE_BUS_SHIFT) |
                                        (device << PCIE_DEVICE_SHIFT),
                                        RM_PAGE_SIZE,
                                        NV_MEMORY_UNCACHED,
                                        NV_PROTECT_READ_WRITE);

    if (pGpu->gpuCfgAddr == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "unable to map GPU's PCI-E configuration space.\n");
        return;
    }
}

static void
objClGpuUnmapEnhCfgSpace
(
    OBJGPU *pGpu
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJOS *pOS = SYS_GET_OS(pSys);

    NV_ASSERT_OR_RETURN_VOID(!pOS->getProperty(pOS, PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS));

    if (pGpu->gpuCfgAddr != NULL)
    {
        osUnmapKernelSpace(pGpu->gpuCfgAddr, RM_PAGE_SIZE);
        pGpu->gpuCfgAddr = NULL;
    }
}

static NV_STATUS
objClSetPortCapsOffsets
(
    OBJCL *pCl,
    PORTDATA     *pPort
)
{
    NV_CHECK_OK_OR_RETURN(LEVEL_INFO, 
                          clSetPortPcieCapOffset(pCl, pPort->addr.handle,
                          &pPort->PCIECapPtr));
    NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
                          objClSetPortPcieEnhancedCapsOffsets(pCl, pPort));

    return NV_OK;
}

//
// Look for the PCI Express capability offset
// Copied from nvagp.c and modified
//
NV_STATUS
clSetPortPcieCapOffset_IMPL
(
    OBJCL *pCl,
    void *handle,
    NvU32 *cap_offset
)
{
    NvU8   cap_next;
    NvU32  pcie_caps;

    if (handle == 0)
        return NV_ERR_GENERIC;    // handle hasn't been initialized

    if (((osPciReadDword(handle, 0x4)) & 0x00100000) == 0)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "NVPCIE: Upstream port doesn't support PCI Express Capability structure. This is a violation of PCIE spec\n");
        NV_ASSERT(0);
        return NV_ERR_GENERIC;    // chipset doesn't support capability ptrs
    }

    // find the PCI offset for the PCI Express Cap ID

    cap_next = osPciReadByte(handle, PCI_CAPABILITY_LIST);
    while (cap_next)
    {
        pcie_caps = osPciReadDword(handle, cap_next);
        if ((pcie_caps & CAP_ID_MASK) == CAP_ID_PCI_EXPRESS)
            break;      // found the PCI Express Cap ID
        cap_next = (NvU8)((pcie_caps >> 8) & 0xFF);
    }

    if (cap_next == 0)
        return NV_ERR_GENERIC;    // didn't find the PCI Express capid

    *cap_offset = cap_next;
    return NV_OK;
}

//
// Look for the PCI Express Enhanced capability list
//
static NV_STATUS
objClSetPortPcieEnhancedCapsOffsets
(
    OBJCL *pCl,
    PORTDATA     *pPort
)
{
    NvU32  cap_next, cap_type, value;

    // make sure we can access the enhanced configuration space
    if (!pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE) ||
        (!pPort->addr.valid))
    {
        return NV_ERR_INVALID_STATE;
    }

    // Enhanced CFG space starts at 0x100
    cap_next = 0x100;
    while (cap_next)
    {
        value = clPcieReadDword(pCl,
                                pPort->addr.domain,
                                pPort->addr.bus,
                                pPort->addr.device,
                                pPort->addr.func,
                                cap_next);
        cap_type = REF_VAL(PCIE_CAP_HEADER_ID, value);
        switch (cap_type)
        {
            case PCIE_CAP_ID_ERROR:
                pPort->PCIEErrorCapPtr = cap_next;
                break;
            case PCIE_CAP_ID_VC:
                pPort->PCIEVCCapPtr = cap_next;
                break;
            case PCIE_CAP_ID_L1_PM_SUBSTATES:
                pPort->PCIEL1SsCapPtr = cap_next;
                break;
            case PCIE_CAP_ID_ACS:
                pPort->PCIEAcsCapPtr = cap_next;
                break;
        }
        cap_next = REF_VAL(PCIE_CAP_HEADER_NEXT, value);
    }

    return NV_OK;
}

NV_STATUS
clPcieReadPortConfigReg_IMPL
(
    OBJGPU   *pGpu,
    OBJCL    *pCl,
    PORTDATA *pPort,
    NvU32     offset,
    NvU32    *value
)
{
    if ((kbifGetBusIntfType_HAL(GPU_GET_KERNEL_BIF(pGpu)) !=
         NV2080_CTRL_BUS_INFO_TYPE_PCI_EXPRESS) ||
        !pPort->addr.valid)
    {
        return NV_ERR_GENERIC;
    }

    if ((offset >= CL_PCIE_BEGIN) && (offset <= CL_PCIE_END))
    {
        if (pPort->PCIECapPtr)
            *value = osPciReadDword(pPort->addr.handle,
                                    offset - CL_PCIE_BEGIN + pPort->PCIECapPtr);
        else
            return NV_ERR_GENERIC;
    }
    else if ((offset >= CL_AER_BEGIN) && (offset <= CL_AER_END))
    {
        if (pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE) && pPort->PCIEErrorCapPtr)
            *value = clPcieReadDword(pCl,
                                     pPort->addr.domain,
                                     pPort->addr.bus,
                                     pPort->addr.device,
                                     pPort->addr.func,
                                     offset - CL_AER_BEGIN + pPort->PCIEErrorCapPtr);
        else
            return NV_ERR_GENERIC;
    }
    else if ((offset >= CL_VC_BEGIN) && (offset <= CL_VC_END))
    {
        if (pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE) && pPort->PCIEVCCapPtr)
            *value = clPcieReadDword(pCl,
                                     pPort->addr.domain,
                                     pPort->addr.bus,
                                     pPort->addr.device,
                                     pPort->addr.func,
                                     offset - CL_VC_BEGIN + pPort->PCIEVCCapPtr);
        else
            return NV_ERR_GENERIC;
    }
    else if ((offset >= CL_L1_SS_BEGIN) && (offset <= CL_L1_SS_END))
    {
        if (pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE) && pPort->PCIEL1SsCapPtr)
            *value = clPcieReadDword(pCl,
                                     pPort->addr.domain,
                                     pPort->addr.bus,
                                     pPort->addr.device,
                                     pPort->addr.func,
                                     offset - CL_L1_SS_BEGIN + pPort->PCIEL1SsCapPtr);
        else
            return NV_ERR_GENERIC;
    }
    else if ((offset >= CL_ACS_BEGIN) && (offset <= CL_ACS_END))
    {
        if (pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE) && pPort->PCIEAcsCapPtr)
        {
            // ACS config space registers are a mix of 16 and 32 bit registers.
            switch (offset)
            {
                case CL_ACS_BEGIN:
                case CL_ACS_EGRESS_CTL_V:
                {
                    *value = clPcieReadDword(pCl,
                                             pPort->addr.domain,
                                             pPort->addr.bus,
                                             pPort->addr.device,
                                             pPort->addr.func,
                                             offset - CL_ACS_BEGIN + pPort->PCIEAcsCapPtr);
                    break;
                }
                case CL_ACS_CAP:
                case CL_ACS_CTRL:
                {
                    *value = (NvU32) clPcieReadWord(pCl,
                                                    pPort->addr.domain,
                                                    pPort->addr.bus,
                                                    pPort->addr.device,
                                                    pPort->addr.func,
                                                    offset - CL_ACS_BEGIN + pPort->PCIEAcsCapPtr);
                }
            }
        }
        else
            return NV_ERR_GENERIC;
    }
    else if (offset + sizeof(NvU32) <= 0x40)
    {
        *value = osPciReadDword(pPort->addr.handle, offset);
    }
    else
        return NV_ERR_GENERIC; // read from extended PCI Express configuration space, currently not supported yet

    return NV_OK;
}

static NvBool
objClBR03Exists
(
    OBJGPU *pGpu,
    OBJCL *pCl
)
{
    void *handle;
    NvU8 dummy8, gpuBus;
    NvU32 gpuDomain;
    NvU16 vendor, device;

    if ((kbifGetBusIntfType_HAL(GPU_GET_KERNEL_BIF(pGpu)) !=
         NV2080_CTRL_BUS_INFO_TYPE_PCI_EXPRESS) ||
        !pGpu->gpuClData.upstreamPort.addr.valid)
    {
        return NV_FALSE;
    }

    gpuDomain = gpuGetDomain(pGpu);
    gpuBus = gpuGetBus(pGpu);

    // Upstream port
    handle = clFindP2PBrdg(pCl, gpuDomain, gpuBus, &dummy8, &dummy8, &dummy8, &vendor, &device);

    // make sure handle was found
    if (!handle)
        return NV_FALSE;

    // make sure that this is the BR03 upstream port
    if ( (vendor != PCI_VENDOR_ID_NVIDIA) ||
         (device != NV_BR03_XVU_DEV_ID_DEVICE_ID_BR03) )
    {
        return NV_FALSE;
    }

    return NV_TRUE;
}

static NvBool
objClBR04Exists
(
    OBJGPU *pGpu,
    OBJCL *pCl
)
{
    void *handle;
    NvU8 dummy8, gpuBus;
    NvU32 gpuDomain;
    NvU16 vendor, device;

    if ((kbifGetBusIntfType_HAL(GPU_GET_KERNEL_BIF(pGpu)) !=
         NV2080_CTRL_BUS_INFO_TYPE_PCI_EXPRESS) ||
        !pGpu->gpuClData.upstreamPort.addr.valid)
    {
        return NV_FALSE;
    }

    gpuDomain = gpuGetDomain(pGpu);
    gpuBus = gpuGetBus(pGpu);

    // Upstream port
    handle = clFindP2PBrdg(pCl, gpuDomain, gpuBus, &dummy8, &dummy8, &dummy8, &vendor, &device);

    // make sure handle was found
    if (!handle)
        return NV_FALSE;

    // make sure that this is the BR04 upstream port
    if ( (vendor != PCI_VENDOR_ID_NVIDIA) ||
         (!IS_DEVID_BR04(device)) )
    {
        return NV_FALSE;
    }

    return NV_TRUE;
}

void *
clFindBrdgUpstreamPort_IMPL
(
    OBJGPU *pGpu,
    OBJCL *pCl,
    NvBool portUpstreamOfBrdg,
    NvU8 *pbus,
    NvU8 *pdev,
    NvU8 *pfunc,
    NvU16 *pvendorID,
    NvU16 *pdeviceID,
    NvU8 *pbusBrdg
)
{
    void *handleBrdg, *handleUpstream;
    NvU8 bus = 0, dev = 0, func = 0, gpuBus;
    NvU32 domain = 0;
    NvU16 vendor = 0, device = 0;

    if ((kbifGetBusIntfType_HAL(GPU_GET_KERNEL_BIF(pGpu)) !=
         NV2080_CTRL_BUS_INFO_TYPE_PCI_EXPRESS) ||
        !pGpu->gpuClData.upstreamPort.addr.valid)
    {
        return NULL;
    }

    domain = gpuGetDomain(pGpu);
    gpuBus = gpuGetBus(pGpu);

    // find upstream port of Brdg
    handleBrdg = clFindP2PBrdg(pCl, domain, gpuBus,
                               &bus, &dev, &func, &vendor, &device);

    // if caller asks for it, save off bus # of this Brdg
    if (pbusBrdg)
    {
        *pbusBrdg = bus;
    }

    // make sure that this is the Brdg upstream port
    if (!handleBrdg || !IS_SUPPORTED_PCIE_SWITCH(vendor, device))
    {
        return NULL;
    }

    if (portUpstreamOfBrdg)
    {
        // find port upstream of Brdg
        handleUpstream = clFindP2PBrdg(pCl, domain, bus,
                                       &bus, &dev, &func, &vendor, &device);

        // make sure handle was found
        if (!handleUpstream)
            return NULL;
    }
    else
    {
        handleUpstream = handleBrdg;
    }

    *pbus = bus;
    *pdev = dev;
    *pfunc = func;
    *pvendorID = vendor;
    *pdeviceID = device;

    return handleUpstream;
}

static void *
objClFindRootPort
(
    OBJGPU *pGpu,
    OBJCL *pCl,
    NvU32 domain,
    NvU8 bus,
    NvU8 *pbusRp,
    NvU8 *pdevRp,
    NvU8 *pfuncRp,
    NvU16 *pvendorIDRp,
    NvU16 *pdeviceIDRp
)
{
    NvU32 PCIECapPtr;
    NvU32 portCaps = 0;
    void *handleUp;
    NvU8 busUp, devUp, funcUp;
    NvU16 vendorIDUp, deviceIDUp;
    NV_STATUS status;

    do
    {
        // find upstream port
        handleUp = clFindP2PBrdg(pCl, domain, bus,
                                 &busUp, &devUp, &funcUp,
                                 &vendorIDUp, &deviceIDUp);

        // make sure handle was found
        if (!handleUp)
            return NULL;

        status = clSetPortPcieCapOffset(pCl, handleUp, &PCIECapPtr);
        if (status != NV_OK)
        {
            // If capability pointer is not found ignore and move to next upstream port.
            bus = busUp;
            continue;
        }

        portCaps = osPciReadDword(handleUp,
                                  CL_PCIE_CAP - CL_PCIE_BEGIN + PCIECapPtr);

        bus = busUp;
    } while (!CL_IS_ROOT_PORT(portCaps));

    *pbusRp = busUp;
    *pdevRp = devUp;
    *pfuncRp = funcUp;
    *pvendorIDRp = vendorIDUp;
    *pdeviceIDRp = deviceIDUp;

    return handleUp;
}

//
// clCountBR
//
// Returns the count of cascaded BR04s right under this GPU.
//
void
clCountBR_IMPL
(
    OBJGPU *pGpu,
    OBJCL *pCl,
    NvU8 *pBR04Count
)
{
    void *handleUp;
    NvU16 vendorIDUp, deviceIDUp;
    NvU8 bus = 0xff, busUp, deviceUp, funcUp;
    NvU32 domain;
    NvU8 downstreamPortBus;
    NvU8 BR04Count = 0;

    domain = gpuGetDomain(pGpu);
    handleUp = clFindBrdgUpstreamPort(pGpu, pCl, NV_TRUE,
                                      &busUp, &deviceUp, &funcUp,
                                      &vendorIDUp, &deviceIDUp,
                                      &downstreamPortBus);

    while (handleUp)
    {
        if ((vendorIDUp == PCI_VENDOR_ID_NVIDIA) && IS_DEVID_BR04(deviceIDUp))
        {
            BR04Count++;
        }
        else
        {
            break;
        }

        bus = busUp;

        // continue up
        handleUp = clFindP2PBrdg(pCl, domain, bus, &busUp, &deviceUp,
                                 &funcUp, &vendorIDUp, &deviceIDUp);
        if (IS_SUPPORTED_PCIE_SWITCH(vendorIDUp, deviceIDUp))
        {
            // port up of bridge
            handleUp = clFindP2PBrdg(pCl, domain, busUp, &busUp, &deviceUp,
                                     &funcUp, &vendorIDUp, &deviceIDUp);
        }
    }

    *pBR04Count = BR04Count;
    return ;
}

//
// clSearchBR04() returns the bus, revision of the BR04s in the system,
// and their count.
// It ignores the BR04s located in Gemini boards.
// It includes BR04s located on the motherboard, or on a riser board.
//
void
clSearchBR04_IMPL
(
    OBJCL *pCl,
    NvU8  *pBR04BusArray,
    NvU8  *pBR04RevArray,
    NvU8  *pBR04Count
)
{
    PBUSTOPOLOGYINFO pBusTopologyInfo = pCl->pBusTopologyInfo;
    PBUSTOPOLOGYINFO pBusTopologyInfoBR04DS, pBusTopologyInfoBR04GPU;
    NvU16 vendorID, deviceID;
    void *handleBR04;
    NvU32 regValue = 0;
    NvU8 BR04DSPorts = 0;
    NvU8 BR04Count = 0;

    while (pBusTopologyInfo)
    {
        if (pBusTopologyInfo->bVgaAdapter)
        {
            // This is not a P2P bridge
            pBusTopologyInfo = pBusTopologyInfo->next;
            continue;
        }

        if (!IS_DEVID_BR04(pBusTopologyInfo->busInfo.deviceID))
        {
            // This is not a BR04
            pBusTopologyInfo = pBusTopologyInfo->next;
            continue;
        }

        //
        // Look at the devices connected to this BR04.
        // If it is a Gemini GPU, then skip.
        // BR04 has one upstream port and 2 to 4 downstream ports.
        // We look at the downstream ports of BR04.
        // We explicitely look for at least 2 BR04 downstream ports.
        //
        pBusTopologyInfoBR04DS = pCl->pBusTopologyInfo;
        BR04DSPorts = 0;
        while (pBusTopologyInfoBR04DS)
        {
            if ((pBusTopologyInfoBR04DS->bus == pBusTopologyInfo->secBus) &&
                IS_DEVID_BR04(pBusTopologyInfoBR04DS->busInfo.deviceID))
            {
                //
                // We have one potential downstream port
                // Look to if a Gemini GPU is connected to this BR04
                //
                pBusTopologyInfoBR04GPU = pCl->pBusTopologyInfo;
                while (pBusTopologyInfoBR04GPU)
                {
                    if (pBusTopologyInfoBR04GPU->bus == pBusTopologyInfoBR04DS->secBus)
                    {
                        break;
                    }
                    pBusTopologyInfoBR04GPU = pBusTopologyInfoBR04GPU->next;
                }
                if (pBusTopologyInfoBR04GPU)
                {
                    // There is something
                    if ((pBusTopologyInfoBR04GPU->busInfo.vendorID == PCI_VENDOR_ID_NVIDIA) &&
                        (gpuDevIdIsMultiGpuBoard(pBusTopologyInfoBR04GPU->busInfo.deviceID, 0, 0)))
                    {
                        // This is a BR04 in a Gemini, skip this DS port
                        pBusTopologyInfoBR04DS = pBusTopologyInfoBR04DS->next;
                        continue;
                    }
                }
                BR04DSPorts++;
            }
            pBusTopologyInfoBR04DS = pBusTopologyInfoBR04DS->next;
        }

        if (BR04DSPorts > 1)
        {
            //
            // We have at least 2 downstream ports. This is a BR04.
            // This is a not a downstream port of a BR04 connected to an upstream port of another BR04.
            // Get the revision.
            //
            handleBR04 = osPciInitHandle(pBusTopologyInfo->domain,
                                         pBusTopologyInfo->bus,
                                         pBusTopologyInfo->device,
                                        pBusTopologyInfo->func, &vendorID, &deviceID);
            regValue = osPciReadDword(handleBR04, NV_BR04_XVU_REV_CC);
            if (DRF_VAL(_BR04_XVU, _REV_CC, _MAJOR_REVISION_ID, regValue) == 0xA)
            {
                pBR04RevArray[BR04Count] = (NvU8) DRF_VAL(_BR04_XVU, _REV_CC, _MINOR_REVISION_ID, regValue);
            }
            else
            {
                pBR04RevArray[BR04Count] = 0xFF;
            }
            pBR04BusArray[BR04Count] = pBusTopologyInfo->bus;
            BR04Count++;
        }
        pBusTopologyInfo = pBusTopologyInfo->next;
    }

    *pBR04Count = BR04Count;
}

//
// clFindCommonBR
//
// Returns the bus number of a common bridge behind the 2 GPUs.
// The returned values are 0xFF when no bridge is found.
// This function finds the most upper bridge(s) if bScanAll is set to NV_TRUE.
// This function finds the first recognized bridge (BR04) under the GPUs if bScanAll is set to NV_FALSE.
//
void
clFindCommonBR_IMPL
(
    OBJGPU *pGpu1,
    OBJGPU *pGpu2,
    OBJCL  *pCl,
    NvU8   *pBR04Bus,
    NvBool  bScanAll
)
{
    void *handleUp1, *handleUp2;
    NvU16 vendorIDUp1, deviceIDUp1, vendorIDUp2, deviceIDUp2;
    NvU8 bus1 = 0xff, busUp1, deviceUp1, funcUp1, bus2 = 0xff, busUp2, deviceUp2, funcUp2;
    NvU32 domain1, domain2;
    NvU8 downstreamPortBus1, downstreamPortBus2;
    NvU8 BR04Bus = 0xFF;

    NV_ASSERT(pGpu1 != pGpu2);

    domain1 = gpuGetDomain(pGpu1);
    domain2 = gpuGetDomain(pGpu2);

    if (domain1 != domain2)
    {
        //
        //1. If two GPUs are from different PCI domains, then there can not be a common BR04 bridge
        //   that connects to both GPUs. Because a new domain will start form a host bridge.
        //2. Returning early when two GPUs are from different PCI domains save significant GPU initialization
        //   time when we have more that 6 GPUs in the system connected to different domains. This function
        //   is called multiple times while searching for 2-way 3-way and 4-way sli combination. (Bug 770154)
        //

        *pBR04Bus = BR04Bus;
        return;
    }

    handleUp1 = clFindBrdgUpstreamPort(pGpu1, pCl, NV_TRUE,
                                       &busUp1, &deviceUp1, &funcUp1,
                                       &vendorIDUp1, &deviceIDUp1,
                                       &downstreamPortBus1);

    // Traverse the PCI-E tree under GPU1
    while (handleUp1)
    {
        if (IS_SUPPORTED_PCIE_SWITCH(vendorIDUp1, deviceIDUp1))
        {
            handleUp2 = clFindBrdgUpstreamPort(pGpu2, pCl, NV_TRUE,
                                               &busUp2, &deviceUp2, &funcUp2,
                                               &vendorIDUp2, &deviceIDUp2,
                                               &downstreamPortBus2);

            // Traverse the PCI-E tree under GPU2
            while (handleUp2)
            {
                // Same bus
                if (busUp2 == busUp1)
                {
                    if ((vendorIDUp2 == PCI_VENDOR_ID_NVIDIA) &&
                        IS_DEVID_BR04(deviceIDUp1) && IS_DEVID_BR04(deviceIDUp2))
                    {
                        BR04Bus = busUp2;
                        break;
                    }
                }

                bus2 = busUp2;

                // continue up
                handleUp2 = clFindP2PBrdg(pCl, domain2, bus2, &busUp2, &deviceUp2,
                                          &funcUp2, &vendorIDUp2, &deviceIDUp2);
            }
        }

        //
        // If we requested to not scan all the devices up to the root port,
        // and we found one, stop right here.
        //
        if (!bScanAll && (BR04Bus != 0xFF))
        {
            break;
        }

        bus1 = busUp1;

        // continue up
        handleUp1 = clFindP2PBrdg(pCl, domain1, bus1, &busUp1, &deviceUp1,
                                  &funcUp1, &vendorIDUp1, &deviceIDUp1);
    }

    *pBR04Bus = BR04Bus;
    return ;
}

//
// clFindCommonDownstreamBR_IMPL
// This function finds the common bridge immediately downstream the GPUs.
// clFindCommonBR can return multiple bridges if bScanAll is NV_TRUE.
//
void
clFindCommonDownstreamBR_IMPL
(
    OBJGPU *pGpu1,
    OBJGPU *pGpu2,
    OBJCL  *pCl,
    NvU8   *pPciSwitchBus
)
{
    void *handleUp1, *handleUp2;
    NvU16 vendorIDUp1, deviceIDUp1, vendorIDUp2, deviceIDUp2;
    NvU8 bus1 = 0xff, busUp1, deviceUp1, funcUp1;
    NvU8 bus2 = 0xff, busUp2, deviceUp2, funcUp2;
    NvU32 domain1, domain2;
    NvU8 downstreamPortBus1, downstreamPortBus2;
    NvU8 pciSwitchBus = 0xFF;

    NV_ASSERT(pGpu1 != pGpu2);

    domain1 = gpuGetDomain(pGpu1);
    domain2 = gpuGetDomain(pGpu2);

    if (domain1 != domain2)
    {
        //
        //1. If two GPUs are from different PCI domains, then there can not be a common BR03/BR04 bridge
        //   that connects to both GPUs. Because a new domain will start form a host bridge.
        //2. Returning early when two GPUs are from different PCI domains save significant GPU initialization
        //   time when we have more that 6 GPUs in the system connected to different domains. This function
        //   is called multiple times while searching for 2-way 3-way and 4-way sli combination. (Bug 770154)
        //

        *pPciSwitchBus = pciSwitchBus;
        return;
    }

    handleUp1 = clFindBrdgUpstreamPort(pGpu1, pCl, NV_TRUE,
                                       &busUp1, &deviceUp1, &funcUp1,
                                       &vendorIDUp1, &deviceIDUp1,
                                       &downstreamPortBus1);

    // Traverse the PCI-E tree under GPU1
    while (handleUp1)
    {
        if (IS_SUPPORTED_PCIE_SWITCH(vendorIDUp1, deviceIDUp1))
        {
            handleUp2 = clFindBrdgUpstreamPort(pGpu2, pCl, NV_TRUE,
                                               &busUp2, &deviceUp2, &funcUp2,
                                               &vendorIDUp2, &deviceIDUp2,
                                               &downstreamPortBus2);

            // Traverse the PCI-E tree under GPU2
            while (handleUp2)
            {
                // Same bus
                if (busUp2 == busUp1)
                {
                    if (IS_SUPPORTED_PCIE_SWITCH(vendorIDUp2, deviceIDUp1) &&
                        IS_SUPPORTED_PCIE_SWITCH(vendorIDUp2, deviceIDUp2))
                    {
                        pciSwitchBus = busUp2;
                        break;
                    }
                }

                bus2 = busUp2;

                // continue up
                handleUp2 = clFindP2PBrdg(pCl, domain2, bus2, &busUp2, &deviceUp2,
                                          &funcUp2, &vendorIDUp2, &deviceIDUp2);
            }
        }

        // If we found a supported switch, stop right here.
        if (pciSwitchBus != 0xFF)
        {
            break;
        }

        bus1 = busUp1;

        // continue up
        handleUp1 = clFindP2PBrdg(pCl, domain1, bus1, &busUp1, &deviceUp1,
                                  &funcUp1, &vendorIDUp1, &deviceIDUp1);
    }

    *pPciSwitchBus  = pciSwitchBus;
    return;
}

// clFindBR
//
// Returns the bus number of the most upper bridge(s) under the GPUs,
// a boolean indicating that a non BR04 A03 bridge has been found between the GPU
// and the host bridge - other than dagwood.
// The returned values are 0xFF when no bridge is found.
// The *pBRNot3rdParty argument is currently solely used and has been tested with X58.
//
void
clFindBR_IMPL
(
    OBJGPU *pGpu,
    OBJCL  *pCl,
    NvU8   *pBR03Bus,
    NvU8   *pBR04Bus,
    NvBool *pBRNot3rdParty,
    NvBool *pNoUnsupportedBRFound,
    NvBool *pNoOnboardBR04
)
{
    void *handleUp, *br04handle = NULL;
    NvU16 vendorIDUp, deviceIDUp;
    NvU8 bus = 0xff, busUp, deviceUp, funcUp;
    NvU32 domain;
    NvU8 downstreamPortBus1;
    NvU32 regValue = 0;
    NvU32 gpuBrNot3rdPartyCount = 0, gpuBrCount = 0;
    NvBool bGpuIsMultiGpuBoard = NV_FALSE;
    NvU32  gpuBR04Count          = 0;
    NvU8 BR03Bus = 0xFF;
    NvU8 BR04Bus = 0xFF;
    NvU8 PLXBus  = 0xFF;
    NvBool brNot3rdParty = NV_FALSE;
    NvBool bNoOnboardBR04        = NV_TRUE;
    NvBool bNoUnsupportedBRFound = NV_TRUE;

    NvU8 BR04Rev = 0x0;

    domain = gpuGetDomain(pGpu);
    handleUp = clFindBrdgUpstreamPort(pGpu, pCl, NV_TRUE,
                                      &busUp, &deviceUp, &funcUp,
                                      &vendorIDUp, &deviceIDUp,
                                      &downstreamPortBus1);

    bGpuIsMultiGpuBoard = gpuIsMultiGpuBoard(pGpu);

    // Traverse the pci tree
    while (handleUp)
    {
        gpuBrCount++;
        br04handle = NULL;
        BR04Rev = 0x0;
        if (IS_DEVID_BR04(deviceIDUp))
        {
            BR04Bus = busUp;
            br04handle = handleUp;
            regValue = osPciReadDword(br04handle, NV_BR04_XVU_REV_CC);
            if (DRF_VAL(_BR04_XVU, _REV_CC, _MAJOR_REVISION_ID, regValue) == 0xA)
            {
                BR04Rev = (NvU8) DRF_VAL(_BR04_XVU, _REV_CC, _MINOR_REVISION_ID, regValue);
            }
            gpuBR04Count++;
        }

        if (deviceIDUp == NV_BR03_XVU_DEV_ID_DEVICE_ID_BR03)
        {
            BR03Bus = busUp;
        }

        //
        // Check if upstream device ID is matching with any device ID
        // on Patsburg PCIE DID range.
        //
        if (((deviceIDUp >= PATSBURG_PCIE_DEVICE_MIN_DEVID) &&
             (deviceIDUp <= PATSBURG_PCIE_DEVICE_MAX_DEVID)) ||
             (deviceIDUp == PATSBURG_PCIE_DEVICE_DEVID))
        {
            bNoUnsupportedBRFound = NV_FALSE;
        }

        if (IS_DEVID_SUPPORTED_PLX(deviceIDUp))
        {
            PLXBus = busUp;
        }

        // Do not count the BR04A03, PLX, and the bridges behind the multi-GPU boards.
        if ((BR04Rev != 0x3) && (PLXBus == 0xFF) && ((gpuBrCount != 1) || (bGpuIsMultiGpuBoard == NV_FALSE)))
        {
            gpuBrNot3rdPartyCount++;
        }

        bus = busUp;

        // continue up
        handleUp = clFindP2PBrdg(pCl, domain, bus, &busUp, &deviceUp,
                                 &funcUp, &vendorIDUp, &deviceIDUp);
    }

    if (bGpuIsMultiGpuBoard && gpuBR04Count)
    {
        // Ignore one BR04 in case of Gemini
        gpuBR04Count--;
    }
    if (gpuBR04Count)
    {
        bNoOnboardBR04 = NV_FALSE;
    }

    // One bridge is just behind the root port. Ignore it.
    brNot3rdParty = (gpuBrNot3rdPartyCount > 1);

    *pBR03Bus = BR03Bus;
    *pBR04Bus = BR04Bus;
    *pBRNot3rdParty = brNot3rdParty;
    *pNoOnboardBR04 = bNoOnboardBR04 ;
    *pNoUnsupportedBRFound = bNoUnsupportedBRFound;

    return ;
}

//
// Free the cached bus topology
// Do not perform per-gpu memory tracking as pCl remains
// during the SLI transitions.
//
void
clFreeBusTopologyCache_IMPL(OBJCL *pCl)
{
    PBUSTOPOLOGYINFO pBusTopologyInfo = pCl->pBusTopologyInfo, pBusTopologyInfoNext;

    while (pBusTopologyInfo)
    {
        pBusTopologyInfoNext = pBusTopologyInfo->next;
        portMemFree(pBusTopologyInfo);
        pBusTopologyInfo = pBusTopologyInfoNext;
    }
    pCl->pBusTopologyInfo = NULL;
}

//
// Cache the bus topology
// Do not perform per-gpu memory tracking as pCl remains
// during the SLI transitions.
// Perform Bus topology caching for new domain.
//
NV_STATUS
clStoreBusTopologyCache_IMPL
(
    OBJCL *pCl,
    NvU32  domain,
    NvU16  secBus
)
{
    void *handle;
    NvU16 vendorID, deviceID;
    NvS16 bus = 0;
    NvS8  device = 0, func = 0;
    NvU16 pciSubBaseClass;
    PBUSTOPOLOGYINFO pBusTopologyInfo = NULL, pBusTopologyInfoLast = NULL;

    if (pCl->pBusTopologyInfo)
    {
        pBusTopologyInfo = pCl->pBusTopologyInfo;
        while (pBusTopologyInfo)
        {
            if (pBusTopologyInfo->domain == domain)
            {
                // Already cached
                return NV_OK;
            }

            // Keep track of the current node  This will capture the last node on exit.
            pBusTopologyInfoLast = pBusTopologyInfo;
            pBusTopologyInfo = pBusTopologyInfo->next;
        }
    }

    // We did not find our domain, so enumerate devices again and update cache.
    for (bus = 0; bus < secBus; bus++)
    {
        for (device = 0; device < PCI_MAX_DEVICES; device++)
        {
            for (func = 0; func < PCI_MAX_FUNCTION; func++)
            {
                // read at bus, device, func
                handle = osPciInitHandle(domain, (NvU8)bus, device, func, &vendorID, &deviceID);
                if (!handle)
                {
                    if (func == 0)
                    {
                        // If a read to function zero of a specified bus/device master aborts,
                        // then it is assumed that no such device exists on the bus since
                        // devices are required to implement function number zero.
                        // In this case reads to the remaining functions are not necessary.
                        break;
                    }
                    else
                    {
                        continue;
                    }
                }

                if (!PCI_IS_VENDORID_VALID(vendorID))
                {
                    break;           // skip to the next device
                }

                pciSubBaseClass = osPciReadWord(handle, PCI_COMMON_CLASS_SUBCLASS);
                pBusTopologyInfo = portMemAllocNonPaged(sizeof(BUSTOPOLOGYINFO));
                if (pBusTopologyInfo == NULL)
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "Buffer Allocation for clStoreBusTopologyCache FAILED\n");
                    clFreeBusTopologyCache(pCl);

                    return NV_ERR_INSUFFICIENT_RESOURCES;
                }

                portMemSet(pBusTopologyInfo, 0, sizeof(BUSTOPOLOGYINFO));

                //
                // Append the new node to the end of the cache linked list.
                // NOTE: pBusTopologyInfoLast holds either the last node in the
                // cache or is NULL, in which case pCl's cache list does not exist.
                //
                if (!pBusTopologyInfoLast)
                {
                    pCl->pBusTopologyInfo = pBusTopologyInfo;
                }
                else
                {
                    pBusTopologyInfoLast->next = pBusTopologyInfo;
                }
                pBusTopologyInfo->next = NULL;
                pBusTopologyInfoLast = pBusTopologyInfo;

                pBusTopologyInfo->handle              = handle;
                pBusTopologyInfo->domain              = domain;
                pBusTopologyInfo->bus                 = (NvU8)bus;
                pBusTopologyInfo->device              = device;
                pBusTopologyInfo->func                = func;
                pBusTopologyInfo->pciSubBaseClass     = pciSubBaseClass;
                pBusTopologyInfo->busInfo.vendorID    = vendorID;
                pBusTopologyInfo->busInfo.deviceID    = deviceID;
                pBusTopologyInfo->busInfo.subvendorID = osPciReadWord(handle, PCI_COMMON_SUBSYSTEM_VENDOR_ID);
                pBusTopologyInfo->busInfo.subdeviceID = osPciReadWord(handle, PCI_COMMON_SUBSYSTEM_ID);
                pBusTopologyInfo->busInfo.revisionID  = osPciReadByte(handle, PCI_HEADER_TYPE0_REVISION_ID);

                if ((pciSubBaseClass == PCI_COMMON_CLASS_SUBBASECLASS_P2P) ||
                    (pciSubBaseClass == PCI_COMMON_CLASS_SUBBASECLASS_HOST) ||
                    (pciSubBaseClass == PCI_COMMON_CLASS_SUBBASECLASS_3DCTRL))
                {
                    pBusTopologyInfo->secBus = (NvU8)osPciReadByte(handle, PCI_TYPE_1_SECONDARY_BUS_NUMBER);
                    pBusTopologyInfo->bVgaAdapter = NV_FALSE;
                }
                else
                {
                    pBusTopologyInfo->bVgaAdapter = NV_TRUE;
                }

                if (func == 0 && ((osPciReadByte(handle, 0xE)) & PCI_MULTIFUNCTION) == 0)
                {
                    break;        // no need to cycle through functions
                }
            }
        }
    }
    //
    // Adding thread reset timeout here to fix Cisco bug 1277168.
    // Enumerating pcie bus topology in cisco host c240 takes too long
    // leading to RM timeout.
    //
    threadStateResetTimeout(NULL);

    return NV_OK;
}


NV_STATUS
clPcieWriteRootPortConfigReg_IMPL
(
    OBJGPU *pGpu,
    OBJCL  *pCl,
    NvU32   offset,
    NvU32   value
)
{
    if ((kbifGetBusIntfType_HAL(GPU_GET_KERNEL_BIF(pGpu)) !=
         NV2080_CTRL_BUS_INFO_TYPE_PCI_EXPRESS) ||
        !pGpu->gpuClData.rootPort.addr.valid)
    {
        return NV_ERR_GENERIC;
    }
    if ((offset >= CL_PCIE_BEGIN) && (offset + sizeof(NvU32) <= CL_PCIE_END))
    {
        if (pGpu->gpuClData.rootPort.PCIECapPtr)
            osPciWriteDword(pGpu->gpuClData.rootPort.addr.handle,
                            offset - CL_PCIE_BEGIN + pGpu->gpuClData.rootPort.PCIECapPtr, value);
        else
            return NV_ERR_GENERIC;
    }
    else if ((offset >= CL_AER_BEGIN) && (offset + sizeof(NvU32) <= CL_AER_END))
    {
        if (pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE) && pGpu->gpuClData.rootPort.PCIEErrorCapPtr)
            clPcieWriteDword(pCl,
                             pGpu->gpuClData.rootPort.addr.domain,
                             pGpu->gpuClData.rootPort.addr.bus,
                             pGpu->gpuClData.rootPort.addr.device,
                             pGpu->gpuClData.rootPort.addr.func,
                             offset - CL_AER_BEGIN + pGpu->gpuClData.rootPort.PCIEErrorCapPtr,
                             value);
        else
            return NV_ERR_GENERIC;
    }
    else if ((offset >= CL_VC_BEGIN) && (offset + sizeof(NvU32) <= CL_VC_END))
    {
        if (pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE) && pGpu->gpuClData.rootPort.PCIEVCCapPtr)
            clPcieWriteDword(pCl,
                             pGpu->gpuClData.rootPort.addr.domain,
                             pGpu->gpuClData.rootPort.addr.bus,
                             pGpu->gpuClData.rootPort.addr.device,
                             pGpu->gpuClData.rootPort.addr.func,
                             offset - CL_VC_BEGIN + pGpu->gpuClData.rootPort.PCIEVCCapPtr,
                             value);
        else
            return NV_ERR_GENERIC;
    }
    else if ((offset >= CL_L1_SS_BEGIN) && (offset + sizeof(NvU32) <= CL_L1_SS_END))
    {
        if (pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE) && pGpu->gpuClData.rootPort.PCIEL1SsCapPtr)
            clPcieWriteDword(pCl,
                             pGpu->gpuClData.rootPort.addr.domain,
                             pGpu->gpuClData.rootPort.addr.bus,
                             pGpu->gpuClData.rootPort.addr.device,
                             pGpu->gpuClData.rootPort.addr.func,
                             offset - CL_L1_SS_BEGIN + pGpu->gpuClData.rootPort.PCIEL1SsCapPtr,
                             value);
        else
            return NV_ERR_GENERIC;
    }
    else
        return NV_ERR_GENERIC; // invalid property

    return NV_OK;
}

NV_STATUS
clPcieReadAerCapability_IMPL
(
    OBJGPU            *pGpu,
    OBJCL             *pCl,
    PcieAerCapability *pAER
)
{
    NV_STATUS status = NV_ERR_GENERIC;

    if ( pAER )
    {
        portMemSet(pAER, 0, sizeof(*pAER));
        status = clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.rootPort, CL_AER_CAP, &pAER->PexEnhCapHeader);
        if ( status != NV_OK ) // if cap id read fails, then there's no AER
            return status;

        // sucess, read the rest of the struct
        clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.rootPort, CL_AER_UNCORRECTABLE_STATUS,  &pAER->UncorrErrStatusReg);
        clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.rootPort, CL_AER_UNCORRECTABLE_MASK,    &pAER->UncorrErrMaskReg);
        clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.rootPort, CL_AER_UNCORRECTABLE_SEVERITY,&pAER->UncorrErrSeverityReg);
        clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.rootPort, CL_AER_CORRECTABLE_STATUS,    &pAER->CorrErrStatusReg);
        clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.rootPort, CL_AER_CORRECTABLE_MASK,      &pAER->CorrErrMaskReg);
        clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.rootPort, CL_AER_ADVANCED_CAP_CONTROL,  &pAER->AEcapCrtlReg);
        clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.rootPort, CL_AER_HEADER_LOG+ 0,         (NvU32*)&pAER->HeaderLogReg.Header[0]);
        clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.rootPort, CL_AER_HEADER_LOG+ 4,         (NvU32*)&pAER->HeaderLogReg.Header[1]);
        clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.rootPort, CL_AER_HEADER_LOG+ 8,         (NvU32*)&pAER->HeaderLogReg.Header[2]);
        clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.rootPort, CL_AER_HEADER_LOG+12,         (NvU32*)&pAER->HeaderLogReg.Header[3]);
        clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.rootPort, CL_AER_ROOT_ERROR_COMMAND,    &pAER->RootErrCmd);
        clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.rootPort, CL_AER_ROOT_ERROR_STATUS,     &pAER->RooErrStatus);
        clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.rootPort, CL_AER_ERROR_SOURCE,          &pAER->ErrSrcReg);

        status = NV_OK;
    }

    return status;
}

NV_STATUS
clPcieReadL1SsCapability_IMPL
(
    OBJGPU                  *pGpu,
    OBJCL                   *pCl,
    PPexL1SubstateCapability pL1Ss
)
{
    NV_STATUS status = NV_ERR_GENERIC;

    if (pL1Ss)
    {
        portMemSet(pL1Ss, 0, sizeof(*pL1Ss));
        status = clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.rootPort, CL_L1_SS_CAP_HDR, &pL1Ss->PexEnhCapHeader);
        if ( status != NV_OK ) // if cap id read fails, then there's no L1 PM Substates Capability
            return status;

        // sucess, read the rest of the struct
        clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.rootPort, CL_L1_SS_CAP_REG, &pL1Ss->Capabilities);
        clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.rootPort, CL_L1_SS_CTRL1_REG, &pL1Ss->Control1Reg);
        clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.rootPort, CL_L1_SS_CTRL2_REG, &pL1Ss->Control2Reg);
        status = NV_OK;
    }

    return status;
}

NV_STATUS
clPcieReadDevCtrlStatus_IMPL
(
    OBJGPU *pGpu,
    OBJCL  *pCl,
    NvU32*  pDevCtrlStatusFlags,
    NvU32*  pDevCtrlStatus
)
{
    NV_STATUS status = NV_ERR_GENERIC;
    NvU32 clDevCtrlStatus = 0;

    if ( pDevCtrlStatusFlags )
    {
        status = clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.rootPort,
                                            CL_PCIE_DEV_CTRL_STATUS, &clDevCtrlStatus);

        if ( status == NV_OK )
        {
            if ( pDevCtrlStatus )
                *pDevCtrlStatus = clDevCtrlStatus; // optionally return full status

            *pDevCtrlStatusFlags = 0;

            if ( pCl->EnteredRecoverySinceErrorsLastChecked )
            {
                pCl->EnteredRecoverySinceErrorsLastChecked = NV_FALSE;
                *pDevCtrlStatusFlags |= NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_ENTERED_RECOVERY;
            }

            if ( clDevCtrlStatus & CL_PCIE_DEVICE_CONTROL_STATUS_CORR_ERROR_DETECTED )
                *pDevCtrlStatusFlags |= NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_CORR_ERROR;
            if ( clDevCtrlStatus & CL_PCIE_DEVICE_CONTROL_STATUS_NON_FATAL_ERROR_DETECTED )
                *pDevCtrlStatusFlags |= NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_NON_FATAL_ERROR;
            if ( clDevCtrlStatus & CL_PCIE_DEVICE_CONTROL_STATUS_FATAL_ERROR_DETECTED )
                *pDevCtrlStatusFlags |= NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_FATAL_ERROR;
            if ( clDevCtrlStatus & CL_PCIE_DEVICE_CONTROL_STATUS_UNSUPP_REQUEST_DETECTED )
                *pDevCtrlStatusFlags |= NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_UNSUPP_REQUEST;
        }
    }

    return status;
}

NV_STATUS
clPcieClearDevCtrlStatus_IMPL
(
    OBJGPU *pGpu,
    OBJCL  *pCl,
    NvU32*  pDevCtrlStatus
)
{
    NV_STATUS status = NV_ERR_GENERIC;
    NvU32 clDevCtrlStatus = 0;

    if ( pDevCtrlStatus )
    {
        clDevCtrlStatus = *pDevCtrlStatus;
        if ( clDevCtrlStatus == 0 )
        {
            return NV_OK;
        }
    }
    else
    {
        status = clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.rootPort,
                                            CL_PCIE_DEV_CTRL_STATUS, &clDevCtrlStatus);
        if ( status != NV_OK )
            return status;
    }

    status = clPcieWriteRootPortConfigReg(pGpu, pCl, CL_PCIE_DEV_CTRL_STATUS, clDevCtrlStatus);

    return status;
}

NvBool
clUpstreamVgaDecodeEnabled_IMPL
(
    OBJGPU *pGpu,
    OBJCL *pCl
)
{
    NvU32 domain;
    NvU8 bus;
    NvU32 PCIECapPtr;
    NvU32 portCaps = 0;
    void *handleUp;
    NvU8 busUp, devUp, funcUp;
    NvU16 vendorIDUp, deviceIDUp;
    NvU16 bridgeCtl;

    domain = gpuGetDomain(pGpu);
    bus = gpuGetBus(pGpu);
    do
    {
        // find upstream port
        handleUp = clFindP2PBrdg(pCl, domain, bus,
                                 &busUp, &devUp, &funcUp,
                                 &vendorIDUp, &deviceIDUp);

        // make sure handle was found
        if (!handleUp)
        {
            return NV_FALSE;
        }

        bus = busUp;

        if (NV_OK != clSetPortPcieCapOffset(pCl, handleUp, &PCIECapPtr))
        {
            //
            // If capability pointer is not found, ignore and move to next
            // upstream port.
            //
            continue;
        }

        portCaps = osPciReadDword(handleUp,
                                  CL_PCIE_CAP - CL_PCIE_BEGIN + PCIECapPtr);

        //
        // If the VGA Enable bit isn't set on any bridge between the device and
        // the root port, bail early.
        //
        bridgeCtl = clPcieReadWord(pCl, domain, busUp, devUp, funcUp,
                                      PCI_HEADER_TYPE1_BRIDGE_CONTROL);
        if ((bridgeCtl & PCI_HEADER_TYPE1_BRIDGE_CONTROL_VGA_EN) == 0)
        {
            return NV_FALSE;
        }
    } while (!CL_IS_ROOT_PORT(portCaps));

    //
    // The VGA Enable bit must have been set in every bridge between the device
    // and the root port.
    //
    return NV_TRUE;
}

static void *
objClPcieMapEnhCfgSpace
(
    OBJCL *pCl,
    NvU32 domain,
    NvU8 bus,
    NvU8 device,
    NvU8 func
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJOS  *pOS     = SYS_GET_OS(pSys);
    OBJGPU *pGpu    = NULL;
    NvU32 gpuAttachCnt, gpuAttachMask, gpuInstance;
    RmPhysAddr pcieConfigSpaceBase;

    NV_ASSERT_OR_RETURN(!pOS->getProperty(pOS, PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS), NULL);

    gpumgrGetGpuAttachInfo(&gpuAttachCnt, &gpuAttachMask);

    gpuInstance = 0;
    while ((pGpu = gpumgrGetNextGpu(gpuAttachMask, &gpuInstance)) != NULL)
    {
        if (pGpu->gpuClData.rootPort.vAddr != 0
            && domain == pGpu->gpuClData.rootPort.addr.domain
            && bus == pGpu->gpuClData.rootPort.addr.bus
            && device == pGpu->gpuClData.rootPort.addr.device
            && func == pGpu->gpuClData.rootPort.addr.func)
        {
            return pGpu->gpuClData.rootPort.vAddr;
        }
        else if ((pGpu->gpuCfgAddr != NULL) &&
                 (gpuIsDBDFValid(pGpu)) &&
                 (domain == gpuGetDomain(pGpu)) &&
                 (bus == gpuGetBus(pGpu)) &&
                 (device == gpuGetDevice(pGpu)) &&
                 (func == 0))
        {
            return pGpu->gpuCfgAddr;
        }
    }

    pcieConfigSpaceBase = clFindPcieConfigSpaceBase(pCl, domain, bus);
    if (pcieConfigSpaceBase == 0)
    {
        return NULL;
    }

    return osMapKernelSpace(pcieConfigSpaceBase |
                            bus << PCIE_BUS_SHIFT |
                            device << PCIE_DEVICE_SHIFT |
                            func << PCIE_FUNC_SHIFT,
                            RM_PAGE_SIZE,
                            NV_MEMORY_UNCACHED,
                            NV_PROTECT_READ_WRITE);
}

static void
objClPcieUnmapEnhCfgSpace
(
    OBJCL *pCl,
    void *addr
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJOS  *pOS = SYS_GET_OS(pSys);
    OBJGPU *pGpu = NULL;
    NvU32 gpuAttachCnt, gpuAttachMask, gpuInstance;

    NV_ASSERT_OR_RETURN_VOID(!pOS->getProperty(pOS, PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS));

    gpumgrGetGpuAttachInfo(&gpuAttachCnt, &gpuAttachMask);

    gpuInstance = 0;
    while ((pGpu = gpumgrGetNextGpu(gpuAttachMask, &gpuInstance)) != NULL)
    {
        if ((pGpu->gpuClData.rootPort.vAddr == addr) ||
            (pGpu->gpuCfgAddr == addr))
        {
            return;
        }
    }

    osUnmapKernelSpace(addr, RM_PAGE_SIZE);
}

NvU16
clPcieReadWord_IMPL
(
    OBJCL *pCl,
    NvU32  domain,
    NvU8   bus,
    NvU8   device,
    NvU8   func,
    NvU32  offset
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJOS *pOS   = SYS_GET_OS(pSys);
    NvU16 *pData = NULL, value = 0;
    NV_ASSERT(offset + sizeof(value) <= 0x1000); // Enhanced Configuration Space is 4K
    NV_ASSERT(device < PCI_MAX_DEVICES); // Maximum device number is 32
    NV_ASSERT(func < PCI_MAX_FUNCTIONS); // Maximum function number is 8

    if (!pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE) ||
        (offset + sizeof(value) > 0x1000))
    {
        NV_ASSERT_FAILED("clPcieReadWord() failed!");
        return 0;
    }

    // Check if there is an OS specific implementation
    void *handle = osPciInitHandle(domain, bus, device, func, NULL, NULL);
    if ((handle != NULL) && osTestPcieExtendedConfigAccess(handle, offset))
    {
        return osPciReadWord(handle, offset);
    }

    NV_ASSERT_OR_RETURN(!pOS->getProperty(pOS, PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS), 0);

    pData = objClPcieMapEnhCfgSpace(pCl, domain, bus, device, func);
    if (!pData)
        return 0;
    value = pData[offset/sizeof(value)];
    objClPcieUnmapEnhCfgSpace(pCl, pData);

    return value;
}

NvU32
clPcieReadDword_IMPL
(
    OBJCL *pCl,
    NvU32  domain,
    NvU8   bus,
    NvU8   device,
    NvU8   func,
    NvU32  offset
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJOS *pOS   = SYS_GET_OS(pSys);
    NvU32 *pData = NULL, value = 0;
    NV_ASSERT(offset + sizeof(value) <= 0x1000); // Enhanced Configuration Space is 4K
    NV_ASSERT(device < PCI_MAX_DEVICES); // Maximum device number is 32
    NV_ASSERT(func < PCI_MAX_FUNCTIONS); // Maximum function number is 8

    if (!pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE) ||
        (offset + sizeof(value) > 0x1000))
    {
        NV_ASSERT_FAILED("clPcieReadDword() failed!");
        return 0;
    }

    // Check if there is an OS specific implementation
    void *handle = osPciInitHandle(domain, bus, device, func, NULL, NULL);
    if ((handle != NULL) && osTestPcieExtendedConfigAccess(handle, offset))
    {
        return osPciReadDword(handle, offset);
    }

    NV_ASSERT_OR_RETURN(!pOS->getProperty(pOS, PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS), 0);

    pData = objClPcieMapEnhCfgSpace(pCl, domain, bus, device, func);
    if (!pData)
        return 0;
    value = MEM_RD32(pData + offset/sizeof(value));
    objClPcieUnmapEnhCfgSpace(pCl, pData);

    return value;
}

void
clPcieWriteWord_IMPL
(
    OBJCL *pCl,
    NvU32  domain,
    NvU8   bus,
    NvU8   device,
    NvU8   func,
    NvU32  offset,
    NvU16  value
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJOS *pOS   = SYS_GET_OS(pSys);
    NvU16 *pData = NULL;
    NV_ASSERT(offset + sizeof(value) <= 0x1000); // Enhanced Configuration Space is 4K
    NV_ASSERT(device < PCI_MAX_DEVICES); // Maximum device number is 32
    NV_ASSERT(func < PCI_MAX_FUNCTIONS); // Maximum function number is 8

    if (!pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE) ||
        (offset + sizeof(value) > 0x1000))
    {
        NV_ASSERT_FAILED("clPcieWriteWord() failed!");
        return;
    }

    // Check if there is an OS specific implementation
    void *handle = osPciInitHandle(domain, bus, device, func, NULL, NULL);
    if ((handle != NULL) && osTestPcieExtendedConfigAccess(handle, offset))
    {
        osPciWriteWord(handle, offset, value);
        return;
    }

    NV_ASSERT_OR_RETURN_VOID(!pOS->getProperty(pOS, PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS));

    pData = objClPcieMapEnhCfgSpace(pCl, domain, bus, device, func);
    if (!pData)
        return;
    pData[offset/sizeof(value)] = value;
    objClPcieUnmapEnhCfgSpace(pCl, pData);
}

void
clPcieWriteDword_IMPL
(
    OBJCL *pCl,
    NvU32  domain,
    NvU8   bus,
    NvU8   device,
    NvU8   func,
    NvU32  offset,
    NvU32  value
)
{

    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJOS *pOS   = SYS_GET_OS(pSys);
    NvU32 *pData = NULL;
    NV_ASSERT(offset + sizeof(value) <= 0x1000); // Enhanced Configuration Space is 4K
    NV_ASSERT(device < PCI_MAX_DEVICES); // Maximum device number is 32
    NV_ASSERT(func < PCI_MAX_FUNCTIONS); // Maximum function number is 8

    if (!pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE) ||
        (offset + sizeof(value) > 0x1000))
    {
        NV_ASSERT_FAILED("clPcieWriteDword() failed!");
        return;
    }

    // Check if there is an OS specific implementation
    void *handle = osPciInitHandle(domain, bus, device, func, NULL, NULL);
    if ((handle != NULL) && osTestPcieExtendedConfigAccess(handle, offset))
    {
        osPciWriteDword(handle, offset, value);
        return;
    }

    NV_ASSERT_OR_RETURN_VOID(!pOS->getProperty(pOS, PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS));

    pData = objClPcieMapEnhCfgSpace(pCl, domain, bus, device, func);
    if (!pData)
        return;
    pData[offset/sizeof(value)] = value;
    objClPcieUnmapEnhCfgSpace(pCl, pData);
}

// root port table and specific overrides

NV_STATUS Broadcom_HT2100_setupFunc(OBJGPU *pGpu, OBJCL *pCl)
{
    pCl->setProperty(pCl, PDB_PROP_CL_EXTENDED_TAG_FIELD_NOT_CAPABLE, NV_TRUE);
    return NV_OK;
}

NV_STATUS Intel_RP25XX_setupFunc(OBJGPU *pGpu, OBJCL *pCl)
{
    // This root port has an AER cap, but it is not advertised.
    // We need to enable it for some bug workarounds.
    if (pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE))
    {
        pGpu->gpuClData.rootPort.PCIEErrorCapPtr = 0x1c0;
        if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_BEHIND_BRIDGE))
        {
            // Intel chipset is real root port and upstream of the GPU.
            pGpu->gpuClData.upstreamPort.PCIEErrorCapPtr = 0x1c0;
        }
    }
    return NV_OK;
}

NV_STATUS Intel_RP81XX_setupFunc(OBJGPU *pGpu, OBJCL *pCl)
{
    pCl->setProperty(pCl, PDB_PROP_CL_NOSNOOP_NOT_CAPABLE, NV_TRUE);
    pCl->setProperty(pCl, PDB_PROP_CL_RELAXED_ORDERING_NOT_CAPABLE, NV_TRUE);
    return NV_OK;
}

NV_STATUS Intel_RP3C0X_setupFunc(OBJGPU *pGpu, OBJCL *pCl)
{
    pCl->setProperty(pCl, PDB_PROP_CL_ROOTPORT_NEEDS_NOSNOOP_WAR, NV_TRUE);

    return NV_OK;
}

NV_STATUS Intel_RP0C0X_setupFunc(OBJGPU *pGpu, OBJCL *pCl)
{
    pCl->setProperty(pCl, PDB_PROP_CL_ROOTPORT_NEEDS_NOSNOOP_WAR, NV_TRUE);

    return NV_OK;
}

NV_STATUS Intel_RP2F0X_setupFunc(OBJGPU *pGpu, OBJCL *pCl)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJHYPERVISOR *pHypervisor = SYS_GET_HYPERVISOR(pSys);
    NvU32 domain;
    // Socket 0 default PCIE location is bus = 0x7f, device = 0x1e, func = 0x3
    NvU16 bus = 0x7f;
    NvU8 device = 0x1e;
    NvU8 func = 3;
    void *handle;
    NvU16 vendorID, deviceID;
    NvU32 val;
    NvU32 eax, ebx, ecx, edx;
    NvBool bC0orC1CPUID = NV_FALSE;

    // Determine if CPU is C0/C1 Stepping by CPUID
    if (osNv_cpuid(1, 0, &eax, &ebx, &ecx, &edx))
    {
        // CPUID is returned to eax
        bC0orC1CPUID = (eax == INTEL_C0_OR_C1_CPUID);
    }

    pCl->setProperty(pCl, PDB_PROP_CL_ROOTPORT_NEEDS_NOSNOOP_WAR, NV_TRUE);

    if (pGpu == NULL)
    {
        return NV_ERR_GENERIC;
    }
    // For GPU passthrough case, bail out and remain on Gen3.
    if (pHypervisor && pHypervisor->bIsHVMGuest)
    {
        return NV_OK;
    }
    // The CAPID logic only works for detecting C0 vs C1 CPU
    // So we need to rule out other CPUs first and apply this logic only for C0/C1 CPU (Bug 1694363)
    if (bC0orC1CPUID)
    {
        domain = gpuGetDomain(pGpu);
        handle = osPciInitHandle(domain, (NvU8)bus, device, func,
                                        &vendorID, &deviceID);
        if (handle && (vendorID == 0x8086) &&
            ((deviceID & 0xfff0) == INTEL_HASWELL_POWER_CONTROL_UNIT_DEVICE_ID) &&
            pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE))
        {
            val = clPcieReadDword(pCl, 0, (NvU8)bus,
                                 device, func, HASWELL_CPU_CAPID4_OFFSET);
            // Bit 29 of CAPID4 has 1 for C1-CPU and 0 for C0-CPU
            if ((val & (1 << 29)) == 0)
            {
                pSys->setProperty(pSys, PDB_PROP_SYS_HASWELL_CPU_C0_STEPPING, NV_TRUE);
            }
        }
        else
        {
            PBUSTOPOLOGYINFO pBusTopologyInfo = pCl->pBusTopologyInfo;
            while (pBusTopologyInfo)
            {
                if ((pBusTopologyInfo->busInfo.vendorID == 0x8086) &&
                    ((pBusTopologyInfo->busInfo.deviceID & 0xFFF0) == INTEL_HASWELL_POWER_CONTROL_UNIT_DEVICE_ID) &&
                    pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE))
                {
                    val = clPcieReadDword(pCl, 0,
                                          pBusTopologyInfo->bus,
                                          pBusTopologyInfo->device,
                                          pBusTopologyInfo->func,
                                          HASWELL_CPU_CAPID4_OFFSET);
                    // Bit 29 of CAPID4 has 1 for C1-CPU and 0 for C0-CPU
                    if ((val & (1 << 29)) == 0)
                    {
                        pSys->setProperty(pSys, PDB_PROP_SYS_HASWELL_CPU_C0_STEPPING, NV_TRUE);
                    }
                    break;
                }
                pBusTopologyInfo = pBusTopologyInfo->next;
            }
        }
    }

    return NV_OK;
}

// Intel Broadwell Setup Function
NV_STATUS Intel_Broadwell_setupFunc(OBJGPU *pGpu, OBJCL *pCl)
{
    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IN_ASPM_POR_LIST, NV_TRUE);

    switch (pCl->FHBBusInfo.deviceID)
    {
        case DEVICE_ID_INTEL_BROADWELL_U_HOST_BRIDGE:
        {
                // Set ASPM L0S\L1 properties
                _Set_ASPM_L0S_L1(pCl, NV_TRUE, NV_TRUE);

                // Enable L0s and L1 on mobile only
                pCl->setProperty(pCl, PDB_PROP_CL_ASPM_L0S_CHIPSET_ENABLED_MOBILE_ONLY, NV_TRUE);
                pCl->setProperty(pCl, PDB_PROP_CL_ASPM_L1_CHIPSET_ENABLED_MOBILE_ONLY, NV_TRUE);
                break;
        }

        case DEVICE_ID_INTEL_BROADWELL_H_HOST_BRIDGE:
        {
            // Set ASPM L0S\L1 properties
            _Set_ASPM_L0S_L1(pCl, NV_FALSE, NV_FALSE);
            break;
        }
        default:
            break;
    }

    return NV_OK;
}

// Intel Skylake Setup Function
NV_STATUS Intel_Skylake_setupFunc(OBJGPU *pGpu, OBJCL *pCl)
{
    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IN_ASPM_POR_LIST, NV_TRUE);

    pCl->setProperty(pCl, PDB_PROP_CL_ROOTPORT_NEEDS_NOSNOOP_WAR, NV_TRUE);

    switch (pCl->FHBBusInfo.deviceID)
    {
        case DEVICE_ID_INTEL_SKYLAKE_U_HOST_BRIDGE:
        {
            // Set ASPM L0S\L1 properties
            _Set_ASPM_L0S_L1(pCl, NV_TRUE, NV_TRUE);

            // Enable L1 on mobile only
            pCl->setProperty(pCl, PDB_PROP_CL_ASPM_L1_CHIPSET_ENABLED_MOBILE_ONLY, NV_TRUE);
            break;
        }

        case DEVICE_ID_INTEL_SKYLAKE_S_HOST_BRIDGE:
        case DEVICE_ID_INTEL_SKYLAKE_H_HOST_BRIDGE:
        {
            // Set ASPM L0S\L1 properties
            _Set_ASPM_L0S_L1(pCl, NV_TRUE, NV_FALSE);

            break;
        }

        case DEVICE_ID_INTEL_KABYLAKE_H_HOST_BRIDGE:
        {
            // Set ASPM L0S\L1 properties
            _Set_ASPM_L0S_L1(pCl, NV_TRUE, NV_TRUE);

            // Enable L1 on mobile only
            pCl->setProperty(pCl, PDB_PROP_CL_ASPM_L1_CHIPSET_ENABLED_MOBILE_ONLY, NV_TRUE);

            break;
        }

        default:
            break;
    }

    return NV_OK;
}

// Intel PCH (0x9D18) Setup Function
NV_STATUS Intel_Skylake_U_Pch_setupFunc(OBJGPU *pGpu, OBJCL *pCl)
{
    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IN_ASPM_POR_LIST, NV_TRUE);

    // Set ASPM L0S\L1 properties
    _Set_ASPM_L0S_L1(pCl, NV_TRUE, NV_TRUE);

    pGpu->setProperty(pGpu, PDB_PROP_GPU_UPSTREAM_PORT_L1_POR_SUPPORTED, NV_TRUE);

    // Enable L1 on mobile only
    pGpu->setProperty(pGpu, PDB_PROP_GPU_UPSTREAM_PORT_L1_POR_MOBILE_ONLY, NV_TRUE);

    pCl->setProperty(pCl, PDB_PROP_CL_ROOTPORT_NEEDS_NOSNOOP_WAR, NV_TRUE);

    return NV_OK;
}

// Intel PCH (0xA117, 0xA118) Setup Function
NV_STATUS Intel_Skylake_H_Pch_setupFunc(OBJGPU *pGpu, OBJCL *pCl)
{
    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IN_ASPM_POR_LIST, NV_TRUE);

    // Set ASPM L0S\L1 properties
    _Set_ASPM_L0S_L1(pCl, NV_TRUE, NV_FALSE);

    pGpu->setProperty(pGpu, PDB_PROP_GPU_UPSTREAM_PORT_L1_POR_SUPPORTED, NV_TRUE);

    pCl->setProperty(pCl, PDB_PROP_CL_ROOTPORT_NEEDS_NOSNOOP_WAR, NV_TRUE);

    return NV_OK;
}

// Intel Kabylake Setup Function
NV_STATUS Intel_Kabylake_Y_setupFunc(OBJGPU *pGpu, OBJCL *pCl)
{
    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IN_ASPM_POR_LIST, NV_TRUE);

    // Set ASPM L0S\L1 properties
    _Set_ASPM_L0S_L1(pCl, NV_TRUE, NV_TRUE);

    // Enable L1 on mobile only
    pCl->setProperty(pCl, PDB_PROP_CL_ASPM_L1_CHIPSET_ENABLED_MOBILE_ONLY, NV_TRUE);

    pCl->setProperty(pCl, PDB_PROP_CL_ROOTPORT_NEEDS_NOSNOOP_WAR, NV_TRUE);

    return NV_OK;
}

// Setup function to disable L0s for AMD root port 1483
NV_STATUS AMD_RP1483_setupFunc(OBJGPU *pGpu, OBJCL *pCl)
{
    //
    // Set ASPM L0S\L1 properties
    // Bug id: 200533783
    //
    _Set_ASPM_L0S_L1(pCl, NV_TRUE, NV_FALSE);

    return NV_OK;
}

//
// Setup function to disable L0s for AMD root port 1480
// AMD Castle Peak
//
NV_STATUS AMD_RP1480_setupFunc(OBJGPU *pGpu, OBJCL *pCl)
{
    //
    // Set ASPM L0S\L1 properties
    // Bug id: 200533783
    //
    _Set_ASPM_L0S_L1(pCl, NV_TRUE, NV_FALSE);

    return NV_OK;
}

//
// Setup function to disable L0s for AMD root port 1630
// AMD Renoir-H
//
NV_STATUS AMD_RP1630_setupFunc(OBJGPU *pGpu, OBJCL *pCl)
{
    //
    // Set ASPM L0S\L1 properties
    // Bug id: 200533783
    //
    _Set_ASPM_L0S_L1(pCl, NV_TRUE, NV_FALSE);

    return NV_OK;
}


static NV_STATUS
objClGpuIs3DController(OBJGPU *pGpu)
{
    NvU8 bus, dev;
    NvU32 domain;
    NvU16 vendorID, deviceID;
    void *handle;

    domain = gpuGetDomain(pGpu);
    bus    = gpuGetBus(pGpu);
    dev    = gpuGetDevice(pGpu);

    // read at bus, device (we're always function 0)
    handle = osPciInitHandle(domain, bus, dev, 0, &vendorID, &deviceID);
    if (!handle)
    {
        NV_PRINTF(LEVEL_ERROR, "GPU Config Space not accessible \n");
        return NV_ERR_GENERIC;
    }

    if (osPciReadWord(handle, PCI_COMMON_CLASS_SUBCLASS) == PCI_COMMON_CLASS_SUBBASECLASS_3DCTRL)
    {
        pGpu->setProperty(pGpu, PDB_PROP_GPU_3D_CONTROLLER, NV_TRUE);
    }

    return NV_OK;
}

NV_STATUS
clPcieGetMaxCapableLinkWidth_IMPL
(
    OBJCL  *pCl,
    OBJGPU *pGpu,
    NvU32  *result
)
{
    NvU32 linkCap;

    //
    // Taking care only mobile systems about system max capable link width issue
    // of bug 427155.
    //
    if ((kbifGetBusIntfType_HAL(GPU_GET_KERNEL_BIF(pGpu)) ==
         NV2080_CTRL_BUS_INFO_TYPE_PCI_EXPRESS) &&
        pGpu->gpuClData.rootPort.addr.valid)
    {
        // We are already disabling ASLM for BR03, so leave it alone.
        if (!pCl->pHWBC || pCl->pHWBC->bcRes != HWBC_NVIDIA_BR03)
        {
            if (clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.rootPort, CL_PCIE_LINK_CAP, &linkCap) == NV_OK)
            {
                *result = CL_MAX_LINK_WIDTH(linkCap);
                return NV_OK;
            }
        }
    }

    *result = 0;
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
clPcieIsRelaxedOrderingSafe_IMPL
(
    OBJCL   *pCl,
    OBJGPU  *pGpu,
    NvBool  *result
)
{
    //
    // TODO. This is left for a follow-on change. The first change allows the
    // force-enable option, and establishes all the plumbing for topology
    // detection, but it's considered always safe to enable relaxed ordering,
    // until this logic is developed.
    //
    *result = NV_TRUE;
    return NV_OK;
}

/*!
 * Pulls the devid info out of the pGpu to pass to gpuDevIdIsMultiGpuBoard().
 *
 * @param[in]  pGpu       OBJGPU pointer
 *
 * @return NV_TRUE if the GPU is in a multigpu board, NV_FALSE otherwise
 */
NvBool
gpuIsMultiGpuBoard
(
    OBJGPU *pGpu
)
{

    return pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_PLX_PRESENT) ||
        gpuDevIdIsMultiGpuBoard(
            (NvU16) DRF_VAL(_PCI, _DEVID, _DEVICE, pGpu->idInfo.PCIDeviceID),
            (NvU16) DRF_VAL(_PCI, _SUBID, _VENDOR, pGpu->idInfo.PCISubDeviceID),
            (NvU16) DRF_VAL(_PCI, _SUBID, _DEVICE, pGpu->idInfo.PCISubDeviceID));
}

/*
 * @brief Scans for the RSDP structure and returns the address of
 * RSDT or XSDT ACPI table.
 *
 * We will use these tables to find the addresses of other tables.
 * https://www.acpi.info/DOWNLOADS/ACPIspec40.pdf sections 5.2.5.1 and 5.2.5.3.
 *
 * @param[in]  pOS          OBJOS pointer
 * @param[in]  start        RmPhysAddr starting point from where to look for the RDSP table
 * @param[in]  len          NvU64 limit until we look for the RDSP table
 * @param[out] pRsdtAddr    NvU32* physical address of the RSDT table
 * @param[out] pXsdtAddr    NvU64* physical address of the XSDT table
 *
 * @returns NV_TRUE the RDST or XDST table has been found, NV_FALSE otherwise.
 */
static NvBool scanForRsdtXsdtTables(OBJOS *pOS,
                                    RmPhysAddr start,
                                    NvU64 len,
                                    NvU32 *pRsdtAddr,
                                    NvU64 *pXsdtAddr)
{
    NvU8 *pData = NULL;
    NvU64 i, c;
    NvU8 checksum = 0;
    NvU8 rsdpRev  = 0;
    NvU32 mode = NV_MEMORY_UNCACHED;

    if (pOS->getProperty(pOS, PDB_PROP_OS_CACHED_MEMORY_MAPPINGS_FOR_ACPI_TABLE))
        mode = NV_MEMORY_CACHED;

    pData = osMapKernelSpace(start, len, mode, NV_PROTECT_READ_WRITE);
    if (NULL == pData)
    {
        return NV_FALSE;
    }

    //
    // Look for RSDP signature 'RSD PTR ' which is
    // always located at 16 byte boundary.
    //
    for (i = 0; i < len; i += 0x10)
    {
        if ((MEM_RD32(pData + i) == NvU32_BUILD(' ', 'D', 'S', 'R')) &&
            (MEM_RD32(pData + i + 4) == NvU32_BUILD(' ', 'R', 'T', 'P')))
        {
            for (c = 0 ; c < 20 && c < len; c++)
            {
                  checksum += MEM_RD08(pData + i + c);
            }

            if (checksum == 0)
            {
                //
                // Get RSDP structure revision. RSDP revision must be
                // either 0 (ACPI 1.0) or 2 (ACPI 2.0 or later).
                //
                rsdpRev = MEM_RD08(pData + i + 15);

                //
                // As per ACPI specification, we must use XSDT if
                // the revision is greater than 1 otherwise use RSDT.
                //
                if (rsdpRev > 1)
                {
                    *pXsdtAddr = MEM_RD32(pData + i + 24) |  ((NvU64)MEM_RD32(pData + i + 28) << 32);
                }
                else
                {
                    *pRsdtAddr = MEM_RD32(pData + i + 16);
                }
                break;
            }
        }
    }
    osUnmapKernelSpace((void*)pData, len);


    return ((*pRsdtAddr != 0) || (*pXsdtAddr != 0));
}

/*
 * @brief Returns the addresses of the tables RSDT and XSDT
 *
 * @param[out] pRsdtAddr    NvU32* physical address of the RSDT table
 * @param[out] pXsdtAddr    NvU64* physical address of the XSDT table
 *
 * @returns NV_OK if RDST or XDST table was found, NV_ERR_* otherwise.
 */
static NV_STATUS
GetRsdtXsdtTablesAddr
(
    OBJCL *pCl,
    NvU32 *pRsdtAddr,
    NvU64 *pXsdtAddr
)
{
    OBJSYS         *pSys      = SYS_GET_INSTANCE();
    OBJOS          *pOS       = SYS_GET_OS(pSys);
    volatile NvU16 *edbaSeg   = NULL;
    NvU32           startAddr = 0;
    NvU32           size      = 0;
    NvU32           mode      = NV_MEMORY_UNCACHED;
    NV_STATUS       status    = NV_ERR_GENERIC;


    if ((pRsdtAddr == NULL) || (pXsdtAddr == NULL))
    {
        return NV_ERR_INVALID_STATE;
    }

    //
    // It doesn't make sense to search for the ACPI tables in the BIOS area
    // on non-X86 CPUs, so just skip that here.
    //
    if (!NVCPU_IS_FAMILY_X86)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    //
    // Sections from https://www.acpi.info/DOWNLOADS/ACPIspec40.pdf referenced below.
    // 1. Section 5.2.5.1 and 5.2.5.2 - get the Root System Description Pointer (RSDP)
    // Search physical memory ranges on 16-byte boundaries for a valid
    // Root System Description Pointer structure signature and checksum
    // match as follows:
    // * The first 1 KB of the Extended BIOS Data Area (EBDA).
    // For EISA or MCA systems, the EBDA can be found in the two-byte location
    // 40:0Eh on the BIOS data area.
    // * The BIOS read-only memory space between 0E0000h and 0FFFFFh.
    // * In Unified Extensible Firmware Interface (UEFI) enabled systems, a pointer
    // to the RSDP structure exists within the EFI System Table (section 5.2.5.2).
    //
    // 2. Section 5.2.5.3
    // The RSDP structure contains the RSDT address at offset 16 and the XSDT address at offset 24
    //
#   define ACPI_EBDA_SEG_ADDR 0x40e
#   define ACPI_EBDA_LEN 0x400
#   define BIOS_RO_MEMORY_BASE 0xE0000
#   define BIOS_RO_MEMORY_SIZE 0x20000
#   define ACPI_RSDP_STRUCT_LEN 0x24

    if (pOS->getProperty(pOS, PDB_PROP_OS_CACHED_MEMORY_MAPPINGS_FOR_ACPI_TABLE))
    {
        mode = NV_MEMORY_CACHED;
    }

    // First try and get RSDP from OS EFI tables.
    if (RMCFG_FEATURE_PLATFORM_UNIX && (pSys->getProperty(pSys, PDB_PROP_SYS_IS_UEFI)))
    {
        status = osGetAcpiRsdpFromUefi(&startAddr);
        if (status != NV_OK)
        {
            goto GetRsdtXsdtTablesAddr_exit;
        }

        size = ACPI_RSDP_STRUCT_LEN;
        if (scanForRsdtXsdtTables(pOS , startAddr, size, pRsdtAddr, pXsdtAddr) == NV_TRUE)
        {
            status = NV_OK;
            goto GetRsdtXsdtTablesAddr_exit;
        }
    }

    //
    // Now try legacy methods to find RSDP. Search for RSDP
    // in first 1 KB of the Extended BIOS Data Area (EBDA).
    //
    edbaSeg = osMapKernelSpace(ACPI_EBDA_SEG_ADDR, ACPI_EBDA_LEN,
                               mode, NV_PROTECT_READ_WRITE);
    if (NULL != edbaSeg)
    {
        startAddr = MEM_RD16(edbaSeg) << 4;
        size      = ACPI_EBDA_LEN;
        osUnmapKernelSpace((void*)edbaSeg, ACPI_EBDA_LEN);
        if (scanForRsdtXsdtTables(pOS , startAddr, size, pRsdtAddr, pXsdtAddr) == NV_TRUE)
        {
            status = NV_OK;
            goto GetRsdtXsdtTablesAddr_exit;
        }
    }

    // Finally, search for RSDP in BIOS read-only memory space.
    startAddr = BIOS_RO_MEMORY_BASE;
    size      = BIOS_RO_MEMORY_SIZE;
    if (scanForRsdtXsdtTables(pOS, startAddr, size, pRsdtAddr, pXsdtAddr) == NV_TRUE)
    {
        status = NV_OK;
    }

GetRsdtXsdtTablesAddr_exit:
    return status;
}

/*
 * @brief Gets a copy of the MCFG table in buffer
 *
 * @param[in]  POS            OBJOS  pointer
 * @param[out] ppMcfgTable    void** pointer to buffer for MCFG table
 * @param[out] pTableSize     NvU32* pointer to MCFG table size
 *
 * @returns NV_TRUE the RDST or XDST table has been found, NV_FALSE otherwise.
 */
static NvBool
GetMcfgTableFromOS
(
    OBJCL   *pCl,
    OBJOS   *pOS,
    void   **ppMcfgTable,
    NvU32   *pTableSize
)
{
    NvU32 retSize       = 0;

    NV_ASSERT_OR_RETURN (!pOS->getProperty(pOS, PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS), NV_FALSE);

    if (osInitGetAcpiTable() == NV_OK)
    {
        //
        // The first call to osGetAcpiTable is to get the ACPI table size.
        // A non-zero retSize means the function has returned a valid
        // size for the table specified.
        //
        if ((osGetAcpiTable(NV_ACPI_TABLE_SIGNATURE_GFCM, NULL,
                            *pTableSize, &retSize) != NV_OK) && retSize)
        {
            *pTableSize = retSize;

            // Allocate memory as per the size of table
            *ppMcfgTable = portMemAllocNonPaged(*pTableSize);
            if (*ppMcfgTable == NULL)
            {
                return NV_FALSE;
            }

            // Second call to actually get the table
            if (osGetAcpiTable(NV_ACPI_TABLE_SIGNATURE_GFCM, ppMcfgTable,
                               *pTableSize, &retSize) == NV_OK)
            {
                pOS->setProperty(pOS, PDB_PROP_OS_GET_ACPI_TABLE_FROM_UEFI, NV_TRUE);
            }
            else
            {
                portMemFree(*ppMcfgTable);
                *pTableSize = 0;
            }
        }

        return NV_TRUE;
    }

    return (((*pTableSize != 0) && (*ppMcfgTable != NULL)) ? NV_TRUE : NV_FALSE);
}

/*
 * @brief Scans the RSDT and/or XSDT tables to get the address of the table matching the
 *        signature.
 *
 * https://www.acpi.info/DOWNLOADS/ACPIspec40.pdf sections 5.2.7 and 5.2.8
 * The description header addresses start at offset 36.
 *
 * @param[in]  POS            OBJOS pointer
 * @param[in]  rsdtAddr       NvU64 physical address of the RSDT table
 * @param[in]  xsdtAddr       NvU64 physical address of the XSDT table
 * @param[in]  tableSignature NvU32 the table signature
 *
 * @returns the address of the DSDT table, or 0 if an error occurred.
 */
static NvU64
ScanForTable
(
    OBJCL *pCl,
    OBJOS *pOS,
    NvU64  rsdtAddr,
    NvU64  xsdtAddr,
    NvU32  tableSignature
)
{
    NvU8 *pData = NULL, *pHeader = NULL;
    NvU64 i, c, step;
    NvU32 len = 0;
    NvU8 checksum = 0;
    NvU64 tableAddr = 0;
    NvU64 sdtAddr;
    NvU32 signature;
    NvU32 current_sig;
    NvBool bTableFound = NV_FALSE;
    NvU32 mode = NV_MEMORY_UNCACHED;

    if ((rsdtAddr == 0) && (xsdtAddr == 0))
    {
        return 0;
    }

    if (rsdtAddr)
    {
        sdtAddr = rsdtAddr;
        signature = NV_ACPI_TABLE_SIGNATURE_TDSR;
        step = 4;
    }
    else
    {
        sdtAddr = xsdtAddr;
        signature = NV_ACPI_TABLE_SIGNATURE_TDSX;
        step = 8;
    }

    // First get the length of RSDT/XSDT table
    if (pOS->getProperty(pOS, PDB_PROP_OS_CACHED_MEMORY_MAPPINGS_FOR_ACPI_TABLE))
    {
        mode = NV_MEMORY_CACHED;
    }

    //
    // The length of any system description table(SDT), in bytes, including the header,
    // is always at byte offset 4 from the table starting offset. The length of table
    // is a 4 byte field within SDT header.
    //
    pData = osMapKernelSpace(sdtAddr, 8, mode, NV_PROTECT_READ_WRITE);

    if (NULL == pData)
    {
        goto scanForTable_exit;
    }

    current_sig = MEM_RD32((NvU32 *)(pData));

    if (current_sig != signature)
    {
        goto scanForTable_exit;
    }

    len = MEM_RD32((NvU32 *)(pData + 4));

    osUnmapKernelSpace((void*)pData, 8);

    // Now map the whole of RSDT/XSDT table
    pData = osMapKernelSpace(sdtAddr, len, mode, NV_PROTECT_READ_WRITE);
    if (NULL == pData)
    {
        goto scanForTable_exit;
    }

    for (c = 0 ; c < len; c++)
    {
        checksum += MEM_RD08(&(pData[c]));
    }

    if (checksum != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "Checksum mismatch\n");
        goto scanForTable_exit;
    }

    // Each System Descriptor Table begins with a "SDT Header" of length 36 bytes.
    i = 36;

    //
    // Now get the table specified by tableSignature by parsing RSDT/XSDT table
    // (specified by table signature). Each entry in RSDT/XSDT refers to other
    // ACPI Description Tables.
    //
    while (i < len)
    {
        if (rsdtAddr)
        {
            tableAddr = MEM_RD32((NvU32 *)(pData + i));
        }
        else
        {
            portMemCopy((NvU8 *)&tableAddr, sizeof(NvU64), (NvU8 *)(pData + i), sizeof(NvU64));
        }
        pHeader = osMapKernelSpace(tableAddr, 8, mode, NV_PROTECT_READ_WRITE);
        if (pHeader == NULL)
        {
            i += step;
            continue;
        }

        if (MEM_RD32((NvU32 *)(pHeader)) == tableSignature)
        {
            bTableFound = NV_TRUE;
            break;
        }

        osUnmapKernelSpace((void*)pHeader, 8);
        pHeader = NULL;
        i += step;
    }

scanForTable_exit:
    if (pData)
    {
        osUnmapKernelSpace((void*)pData, len);
    }

    if (pHeader)
    {
        osUnmapKernelSpace((void*)pHeader, 8);
    }

    if (bTableFound == NV_TRUE)
    {
        return tableAddr;
    }

    // Table not found
    return 0;
}

typedef struct
{
    NvU64 BaseAddress;
    NvU32 PCISegmentGroupNumber:16;
    NvU32 StartBusNumber:8;
    NvU32 EndBusNumber:8;
    NvU32 Reserved;
} MCFG_ADDRESS_ALLOCATION_STRUCTURE;

/*
 * @brief Store PCI-E config space base addresses for all domain numbers
 *
 * @param[in]  pOS            OBJOS pointer
 * @param[in]  pCl            OBJCL pointer
 * @param[in]  pMcfgTable     Pointer to buffer for MCFG table
 * @param[in]  len            Length of MCFG table
 *
 */
static NV_STATUS storePcieGetConfigSpaceBaseFromMcfgTable(OBJOS *pOS, OBJCL *pCl, NvU8 *pMcfgTable, NvU32 len)
{
    MCFG_ADDRESS_ALLOCATION_STRUCTURE *pMcfgAddressAllocationStructure;
    MCFG_ADDRESS_ALLOCATION_STRUCTURE mcfgAddressAllocationStructure;
    NvU8 EndBusNumber;
    NvU32 i;
    NV_STATUS status = NV_OK;

    //
    // Get the Exteneded PCI config space address by parsing
    // MCFG table through all config space base address
    // structures of length 44 bytes each.
    //
    i = 44;

    while (i < len)
    {
        if ((len - i) < sizeof(MCFG_ADDRESS_ALLOCATION_STRUCTURE))
        {
            // These are trailing bytes, we can exit the loop now.
            break;
        }
        portMemCopy((NvU8*)&mcfgAddressAllocationStructure, sizeof(MCFG_ADDRESS_ALLOCATION_STRUCTURE), (NvU8*)(pMcfgTable + i), sizeof(MCFG_ADDRESS_ALLOCATION_STRUCTURE));
        pMcfgAddressAllocationStructure = &mcfgAddressAllocationStructure;

        // Fix up Bad System Bioses -- See Bug 715753
        EndBusNumber = (NvU8)pMcfgAddressAllocationStructure->EndBusNumber;

        if (!((NvU8)pMcfgAddressAllocationStructure->StartBusNumber < EndBusNumber))
        {
            EndBusNumber = (NvU8)(PCI_MAX_BUSES - 1);
        }

        if (pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE))
        {
            // node for domain 0 is added in pCl->pPcieConfigSpaceBase by chipsetInfo[i].setupFunc(pCl)
            PPCIECONFIGSPACEBASE pPcieConfigSpaceBase = pCl->pPcieConfigSpaceBase;

            while (pPcieConfigSpaceBase)
            {
                if (pPcieConfigSpaceBase->domain == (NvU32)pMcfgAddressAllocationStructure->PCISegmentGroupNumber)
                {
                    //
                    // node for this domain already exist in pCl->pPcieConfigSpaceBase linked-list,
                    // if start and end bus number doesn't match, update the existing node
                    //
                    if ((pPcieConfigSpaceBase->startBusNumber != (NvU8)pMcfgAddressAllocationStructure->StartBusNumber) ||
                        (pPcieConfigSpaceBase->endBusNumber != (NvU8)EndBusNumber))
                    {
                        pPcieConfigSpaceBase->startBusNumber = (NvU8)pMcfgAddressAllocationStructure->StartBusNumber;
                        pPcieConfigSpaceBase->endBusNumber = (NvU8)EndBusNumber;
                        pPcieConfigSpaceBase->baseAddress = pMcfgAddressAllocationStructure->BaseAddress;
                    }
                    break;
                }
                pPcieConfigSpaceBase = pPcieConfigSpaceBase->next;
            }

            if (!pPcieConfigSpaceBase)
            {
                // There are multiple domains, node for this domain doesn't exist, insert node for this domain in the linked-list.
                status = clInsertPcieConfigSpaceBase(pCl,
                                                     pMcfgAddressAllocationStructure->BaseAddress,
                                                     (NvU32)pMcfgAddressAllocationStructure->PCISegmentGroupNumber,
                                                     (NvU8)pMcfgAddressAllocationStructure->StartBusNumber,
                                                     (NvU8)EndBusNumber);
            }
        }
        else
        {
            status = clInsertPcieConfigSpaceBase(pCl,
                                                 pMcfgAddressAllocationStructure->BaseAddress,
                                                 (NvU32)pMcfgAddressAllocationStructure->PCISegmentGroupNumber,
                                                 (NvU8)pMcfgAddressAllocationStructure->StartBusNumber,
                                                 (NvU8)EndBusNumber);
        }

        if (status != NV_OK)
        {
            break;
        }
        i += sizeof(MCFG_ADDRESS_ALLOCATION_STRUCTURE);
    }

    return status;
}

/*
 * @brief Locate and store the PCI-E config space base address for all domain
 * numbers from the MCFG table.
 *
 * @param[in]  pCl            OBJCL pointer
 *
 * @returns NV_OK if successfull, NV_ERR_* otherwise.
 */
NV_STATUS
clStorePcieConfigSpaceBaseFromMcfg_IMPL(OBJCL *pCl)
{
    OBJSYS    *pSys       = SYS_GET_INSTANCE();
    OBJOS     *pOS        = SYS_GET_OS(pSys);
    NvU64      rsdtAddr   = 0;
    NvU64      xsdtAddr   = 0;
    NvU64      mcfgAddr   = 0;
    NvU8      *pData      = NULL;
    NvU32      len        = 0;
    NvU32      mode       = NV_MEMORY_UNCACHED;
    NvU8       checksum   = 0;
    NvU64      c          = 0;
    NV_STATUS  status     = NV_ERR_GENERIC;

    NV_ASSERT_OR_RETURN (!pOS->getProperty(pOS, PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS), NV_ERR_INVALID_STATE);

    if (!pCl->FHBAddr.valid)
    {
        return NV_ERR_INVALID_DATA;
    }

    if (GetMcfgTableFromOS(pCl, pOS, (void **)&pData, &len) == NV_FALSE)
    {
        //
        // If OS api doesn't provide MCFG table then MCFG table address
        // can be found by parsing RSDT/XSDT tables.
        //
        status = GetRsdtXsdtTablesAddr(pCl, (NvU32*)&rsdtAddr, &xsdtAddr);
        if (status != NV_OK)
        {
            goto clStorePcieConfigSpaceBaseFromMcfg_exit;
        }

        mcfgAddr = ScanForTable(pCl, pOS, rsdtAddr, xsdtAddr, NV_ACPI_TABLE_SIGNATURE_GFCM);
        if (mcfgAddr == 0)
        {
            status = NV_ERR_INSUFFICIENT_RESOURCES;
            goto clStorePcieConfigSpaceBaseFromMcfg_exit;
        }

        if (pOS->getProperty(pOS, PDB_PROP_OS_CACHED_MEMORY_MAPPINGS_FOR_ACPI_TABLE))
        {
            mode = NV_MEMORY_CACHED;
        }

        // Get MCFG Table Length
        pData = osMapKernelSpace(mcfgAddr, 8, mode, NV_PROTECT_READ_WRITE);
        if (NULL == pData)
        {
            status = NV_ERR_OPERATING_SYSTEM;
            goto clStorePcieConfigSpaceBaseFromMcfg_exit;
        }

        if (MEM_RD32((NvU32 *)(pData)) != NV_ACPI_TABLE_SIGNATURE_GFCM)
        {
            status = NV_ERR_INVALID_DATA;
            goto clStorePcieConfigSpaceBaseFromMcfg_exit;
        }

        len = MEM_RD32((NvU32 *)(pData + 4));
        osUnmapKernelSpace((void*)pData, 8);

        // Now Map whole table
        pData = osMapKernelSpace(mcfgAddr, len, mode, NV_PROTECT_READ_WRITE);
        if (NULL == pData)
        {
            status = NV_ERR_OPERATING_SYSTEM;
            goto clStorePcieConfigSpaceBaseFromMcfg_exit;
        }

        // Validate checksum
        for (c = 0 ; c < len; c++)
        {
            checksum += MEM_RD08(&(pData[c]));
        }

        if (checksum != 0)
        {
            status = NV_ERR_INVALID_DATA;
            goto clStorePcieConfigSpaceBaseFromMcfg_exit;
        }

    }

    status = storePcieGetConfigSpaceBaseFromMcfgTable(pOS, pCl, pData, len);

clStorePcieConfigSpaceBaseFromMcfg_exit:
    if (pData)
    {
        if (pOS->getProperty(pOS, PDB_PROP_OS_GET_ACPI_TABLE_FROM_UEFI))
        {
            portMemFree(pData);
        }
        else
        {
            osUnmapKernelSpace((void*)pData, len);
        }
    }

    return status;
}

NV_STATUS
clInsertPcieConfigSpaceBase_IMPL
(
    OBJCL     *pCl,
    RmPhysAddr baseAddress,
    NvU32      domain,
    NvU8       startBusNumber,
    NvU8       endBusNumber
)
{
    PPCIECONFIGSPACEBASE pPcieConfigSpaceBase;

    pPcieConfigSpaceBase = portMemAllocNonPaged(sizeof(PCIECONFIGSPACEBASE));
    if (pPcieConfigSpaceBase == NULL)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    pPcieConfigSpaceBase->baseAddress = baseAddress;
    pPcieConfigSpaceBase->domain = domain;
    pPcieConfigSpaceBase->startBusNumber = startBusNumber;
    pPcieConfigSpaceBase->endBusNumber = endBusNumber;
    pPcieConfigSpaceBase->next = pCl->pPcieConfigSpaceBase;
    pCl->pPcieConfigSpaceBase = pPcieConfigSpaceBase;

    NV_PRINTF(LEVEL_INFO,
              "PCIe Config BaseAddress 0x%llx Domain %x startBusNumber %x endBusNumber %x\n",
              pPcieConfigSpaceBase->baseAddress, pPcieConfigSpaceBase->domain,
              pPcieConfigSpaceBase->startBusNumber,
              pPcieConfigSpaceBase->endBusNumber);

    return NV_OK;
}


RmPhysAddr
clFindPcieConfigSpaceBase_IMPL
(
    OBJCL *pCl,
    NvU32  domain,
    NvU8   bus
)
{
    PPCIECONFIGSPACEBASE pPcieConfigSpaceBase = pCl->pPcieConfigSpaceBase;

    while (pPcieConfigSpaceBase)
    {
        if ((pPcieConfigSpaceBase->domain == domain) &&
            (pPcieConfigSpaceBase->startBusNumber <= bus) &&
            (pPcieConfigSpaceBase->endBusNumber >= bus))
        {
            return pPcieConfigSpaceBase->baseAddress;
        }
        pPcieConfigSpaceBase = pPcieConfigSpaceBase->next;
    }
    return 0;
}

void
clFreePcieConfigSpaceBase_IMPL(OBJCL *pCl)
{
    PPCIECONFIGSPACEBASE pPcieConfigSpaceBase = pCl->pPcieConfigSpaceBase;
    PPCIECONFIGSPACEBASE pPcieConfigSpaceBaseNext;

    while (pPcieConfigSpaceBase)
    {
        pPcieConfigSpaceBaseNext = pPcieConfigSpaceBase->next;
        portMemFree(pPcieConfigSpaceBase);
        pPcieConfigSpaceBase = pPcieConfigSpaceBaseNext;
    }
    pCl->pPcieConfigSpaceBase = NULL;
}

//
// Locate and parse the PCI Express Virtual P2P Approval capability from the
// given GPUs PCI configuration space, if it exists. The capability's presence
// across multiple GPUs will define which GPUs are capable of P2P with each
// other.
//
static void
objClLoadPcieVirtualP2PApproval(OBJGPU *pGpu)
{
    void *handle;
    NvU32 data32;
    NvU8  version;
    NvU8  cap;
    NvU8  bus = gpuGetBus(pGpu);
    NvU8  device = gpuGetDevice(pGpu);
    NvU32 domain = gpuGetDomain(pGpu);
    NvU32 offset = NV_PCI_VIRTUAL_P2P_APPROVAL_CAP_0;
    NvU32 sig    = 0;

    if (!IS_PASSTHRU(pGpu))
    {
        NV_PRINTF(LEVEL_INFO,
                  "Skipping non-pass-through GPU%u\n", gpuGetInstance(pGpu));
        return;
    }

    handle = osPciInitHandle(domain, bus, device, 0, NULL, NULL);

    //
    // Check for the Virtual P2P Approval Capability in PCI config space.
    // We've specifically reserved offset 0xC8 for it, but hypervisors
    // aren't strictly required to put it there. They are, however, required
    // to link it in the capability list.
    //
    cap = osPciReadByte(handle, PCI_CAPABILITY_LIST);
    while ((cap != 0) && (sig != NV_PCI_VIRTUAL_P2P_APPROVAL_SIGNATURE))
    {
        offset = cap;
        data32 = osPciReadDword(handle, offset);
        cap = (NvU8)((data32 >> 8) & 0xFF);

        if ((data32 & CAP_ID_MASK) != CAP_ID_VENDOR_SPECIFIC)
            continue;

        sig = DRF_VAL(_PCI, _VIRTUAL_P2P_APPROVAL_CAP_0, _SIG_LO, data32);
        data32 = osPciReadDword(handle, offset + 4);
        sig |= (DRF_VAL(_PCI, _VIRTUAL_P2P_APPROVAL_CAP_1, _SIG_HI, data32) << 8);
    }

    if (sig != NV_PCI_VIRTUAL_P2P_APPROVAL_SIGNATURE)
    {
        NV_PRINTF(LEVEL_INFO,
                  "No virtual P2P approval capability found in GPU%u's capability list\n",
                  gpuGetInstance(pGpu));
        return;
    }

    // data32 now contains the second dword of the capability structure.
    version = (NvU8)DRF_VAL(_PCI, _VIRTUAL_P2P_APPROVAL_CAP_1, _VERSION,
                            data32);
    if (version != 0)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Unable to handle virtual P2P approval capability version %u on GPU%u\n",
                  version, gpuGetInstance(pGpu));
        return;
    }

    pGpu->pciePeerClique.id = (NvU8)DRF_VAL(_PCI, _VIRTUAL_P2P_APPROVAL_CAP_1,
                                            _PEER_CLIQUE_ID, data32);
    pGpu->pciePeerClique.bValid = NV_TRUE;

    NV_PRINTF(LEVEL_INFO,
              "Hypervisor has assigned GPU%u to peer clique %u\n",
              gpuGetInstance(pGpu), pGpu->pciePeerClique.id);
}

/*!
 * @brief : Enable L0s and L1 support for GPU's upstream port
 * Refer Section 7.8.7. Link Control Register of PCIE Spec 3.
 * Note: This function is used for force enabling ASPM and shouldn't be used for normal driver operations
 */
NV_STATUS
clControlL0sL1LinkControlUpstreamPort_IMPL
(
    OBJGPU *pGpu,
    OBJCL  *pCl,
    NvBool  bEnable
)
{
    void  *handle              = pGpu->gpuClData.upstreamPort.addr.handle;
    NvU32 PCIECapPtr           = pGpu->gpuClData.upstreamPort.PCIECapPtr;
    NvU32 linkControlRegOffset = PCIECapPtr + 0x10;
    NvU16 regVal;

    regVal = osPciReadWord(handle, linkControlRegOffset);
    if (regVal == 0xFFFF)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Link Control register read failed for upstream port\n");
        return NV_ERR_GENERIC;
    }

    if(bEnable)
    {
        // 1:0 - 11b L0s and L1 Entry Enabled
        regVal |= 0x3;
    }
    else
    {
        // 0:0 - 00b L0s and L1 Entry Disabled
        regVal &= 0x0;
    }
    osPciWriteDword(handle, linkControlRegOffset, regVal);
    return NV_OK;
}

/*!
 * @brief: Enable L0s and L1 support from chipset
 * Note: This function is used for force enabling ASPM and shouldn't be used
 * for normal driver operations
 *
 * @param[in] pGpu      GPU object pointer
 * @param[in] pBif      BIF object pointer
 * @param[in] aspmState L0s/L1 state (enable/disable)
 *
 * @return NV_OK if ASPM state updated, else return error
 */
NV_STATUS
clChipsetAspmPublicControl_IMPL
(
    OBJGPU *pGpu,
    OBJCL  *pCl,
    NvU32   aspmState
)
{
    void  *pHandle             = pGpu->gpuClData.upstreamPort.addr.handle;
    NvU32 PCIECapPtr           = pGpu->gpuClData.upstreamPort.PCIECapPtr;
    NvU32 linkControlRegOffset = PCIECapPtr + 0x10;
    NvU32 regVal;

    // sanity check
    if (aspmState > CL_PCIE_LINK_CTRL_STATUS_ASPM_MASK)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid ASPM state passed.\n");
        return NV_ERR_INVALID_DATA;
    }

    regVal = osPciReadDword(pHandle, linkControlRegOffset);
    if (regVal == 0xFFFF)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Link Control register read failed for upstream port\n");
        return NV_ERR_GENERIC;
    }

    // Mask all bits except ASPM control bits and update only ASPM bits (1:0)
    regVal = (~CL_PCIE_LINK_CTRL_STATUS_ASPM_MASK) & regVal;
    regVal |= aspmState;

    osPciWriteDword(pHandle, linkControlRegOffset, regVal);

    return NV_OK;
}

/*!
 * @brief: Returns the gen speed of the root node
 */
NV_STATUS
clPcieGetRootGenSpeed_IMPL
(
    OBJGPU  *pGpu,
    OBJCL   *pCl,
    NvU8    *genSpeed
)
{
    void  *handle;
    NvU32 PCIECapPtr;
    NvU32 linkCaps;

    *genSpeed = 0;

    handle = pGpu->gpuClData.rootPort.addr.handle;
    if (handle == NULL)
    {
        return NV_ERR_GENERIC;
    }

    PCIECapPtr = pGpu->gpuClData.rootPort.PCIECapPtr;

    linkCaps = osPciReadDword(handle, CL_PCIE_LINK_CAP - CL_PCIE_BEGIN + PCIECapPtr);

    // Read field 3:0 to get max link speed
    *genSpeed = (NvU8) (linkCaps & 0xF);

    return NV_OK;
}

/*!
 * @brief: Returns the support for CPM of the root node
 */
NvU32
clGetChipsetL1ClockPMSupport_IMPL
(
    OBJGPU  *pGpu,
    OBJCL   *pCl
)
{
    void  *handle;
    NvU32  PCIECapPtr;
    NvU32  linkCaps;
    NvU32  clockPmSupport;

    handle = pGpu->gpuClData.rootPort.addr.handle;
    if (handle == NULL)
    {
        return 0;
    }

    PCIECapPtr = pGpu->gpuClData.rootPort.PCIECapPtr;

    linkCaps = osPciReadDword(handle, CL_PCIE_LINK_CAP - CL_PCIE_BEGIN + PCIECapPtr);

    // Read field 18:18 to get clock PM support
    clockPmSupport = (linkCaps & CL_PCIE_LINK_CAP_CLOCK_PM_BIT);

    return clockPmSupport;
}

/*!
 * @brief: Returns the value of link_capabilities_2 of the downstream port
 *
 * @param[i]   pGpu       GPU object pointer
 * @param[out] pLinkCaps2 link_capabilities_2 register value
 *
 * @return     NV_OK
 */
NV_STATUS
clPcieGetDownstreamPortLinkCap2_IMPL
(
    OBJGPU  *pGpu,
    OBJCL   *pCl,
    NvU32   *pLinkCaps2
)
{
    void  *pHandle;
    NvU32 PCIECapPtr;

    //
    // If there is a switch this is equal to boardDownstreamPort
    // If there is no switch this is equal to rootPort
    //
    pHandle = pGpu->gpuClData.upstreamPort.addr.handle;
    if (pHandle == NULL)
    {
        return NV_ERR_GENERIC;
    }

    PCIECapPtr = pGpu->gpuClData.upstreamPort.PCIECapPtr;

    *pLinkCaps2 = osPciReadDword(pHandle, CL_PCIE_LINK_CAP_2 - CL_PCIE_BEGIN + PCIECapPtr);

    return NV_OK;
}

NvBool clRootportNeedsNosnoopWAR_FWCLIENT(OBJGPU *pGpu, OBJCL *pCl)
{
    const GspStaticConfigInfo *pSCI = GPU_GET_GSP_STATIC_INFO(pGpu);

    NV_ASSERT_OR_RETURN(pSCI != NULL, NV_FALSE);

    return pSCI->bClRootportNeedsNosnoopWAR;
}

/*!
 * @brief   determine the size of the specified PCI capability on the specified device.
 *
 * @param[in]   deviceHandle    handle to the device whose capability we are getting the size for
 * @param[in]   capId           the ID of the capacity we want to get the size for
 * @param[in]   capOffset       the offset in the PCIE configuration space where the capacity is located
 *
 * @return  NvU16 size of the requested capacity.
 *
 */
static NvU16 _clPcieGetPcieCapSize(void *deviceHandle, NvU16 capType, NvU16 capId, NvU16 capOffset)
{
    NvU16 capSize = 0;
    NvU32 tempDword;
    NvU16 count;
    NvU16 idx;

    if (deviceHandle == NULL)
    {
        return 0;
    }
    switch (capType)
    {
    case RM_PCIE_DC_CAP_TYPE_PCI:
        switch (capId)
        {
        case CAP_ID_NULL:
            capSize = CAP_NULL_SIZE;
            break;

        case CAP_ID_PMI:
            capSize = CAP_PMI_SIZE;
            break;

        case CAP_ID_VPD:
            capSize = CAP_VPD_SIZE;
            break;

        case CAP_ID_MSI:
            capSize = PCI_MSI_BASE_SIZE;
            tempDword = osPciReadWord(deviceHandle, capOffset + PCI_MSI_CONTROL);

            if (FLD_TEST_REF(PCI_MSI_CONTROL_64BIT_CAPABLE, _TRUE, tempDword))
            {
                capSize += PCI_MSI_64BIT_ADDR_CAPABLE_ADJ_SIZE;
            }
            if (FLD_TEST_REF(PCI_MSI_CONTROL_PVM_CAPABLE, _TRUE, tempDword))
            {
                capSize += PCI_MSI_PVM_CAPABLE_ADJ_SIZE;
            }
            break;

        case CAP_ID_PCI_EXPRESS:
            capSize = CAP_PCI_EXPRESS_SIZE;
            break;

        case CAP_ID_MSI_X:
            capSize = PCI_MSI_X_BASE_SIZE;
            // todo: add table collection
            break;

        case CAP_ID_ENHANCED_ALLOCATION:
            tempDword = osPciReadByte(deviceHandle, PCI_HEADER_TYPE0_HEADER_TYPE);
            switch (tempDword)
            {
            case PCI_HEADER_TYPE0_HEADER_TYPE_0:
                capSize = PCI_ENHANCED_ALLOCATION_TYPE_0_BASE_SIZE;
                break;

            case PCI_HEADER_TYPE0_HEADER_TYPE_1:
                capSize = PCI_ENHANCED_ALLOCATION_TYPE_0_BASE_SIZE;
                break;

            default:
                break;
            }
            // get the entry count.
            if (capSize != 0)
            {
                tempDword = osPciReadDword(deviceHandle, capOffset + PCI_ENHANCED_ALLOCATION_FIRST_DW);
                count = REF_VAL(PCI_ENHANCED_ALLOCATION_FIRST_DW_NUM_ENTRIES, tempDword);
                for (idx = 0; idx < count; ++idx)
                {
                    tempDword = osPciReadDword(deviceHandle, capOffset + capSize + PCI_ENHANCED_ALLOCATION_ENTRY_HEADER);
                    tempDword = REF_VAL(PCI_ENHANCED_ALLOCATION_ENTRY_HEADER_ENTRY_SIZE, tempDword);
                    capSize += (NvU16)((tempDword + 1) * sizeof(NvU32));
                }
            }
            break;

        case CAP_ID_FPB:
            capSize = CAP_FPB_SIZE;
            break;

        case CAP_ID_AF:
            capSize = CAP_AF_SIZE;
            break;

        case CAP_ID_SUBSYSTEM_ID:
            capSize = CAP_SUBSYSTEM_ID_SIZE;
            break;

        case CAP_ID_AGP:
        case CAP_ID_SLOT_ID:
        case CAP_ID_HOT_SWAP:
        case CAP_ID_PCI_X:
        case CAP_ID_HYPER_TRANSPORT:
        case CAP_ID_DEBUG_PORT:
        case CAP_ID_CRC:
        case CAP_ID_HOT_PLUG:
        case CAP_ID_AGP8X:
        case CAP_ID_SECURE:
            // don't know the sizes of these capabilities right now so just grab 32 bytes;
            capSize = 32;
            break;

        default:
            // this is an unrecognized capability.
            // Assume it is a Vendor Specific capability
            tempDword = osPciReadDword(deviceHandle, capOffset + PCI_VENDOR_SPECIFIC_CAP_HEADER);
            capSize = REF_VAL(PCI_VENDOR_SPECIFIC_CAP_HEADER_LENGTH, tempDword);
            break;
        }
        break;

    case RM_PCIE_DC_CAP_TYPE_PCIE:
        switch (capId)
        {
        case PCIE_CAP_ID_SECONDARY_PCIE_CAPABILITY:
            capSize = PCIE_CAP_SECONDARY_PCIE_SIZE;
            break;

        case PCIE_CAP_ID_DATA_LINK:
            capSize = PCIE_CAP_DATA_LINK_SIZE;
            break;

        case PCIE_CAP_ID_PHYSLAYER_16_GT:
            capSize = PCIE_CAP_PHYSLAYER_16_GT_SIZE;
            break;

        case PCIE_CAP_ID_PHYSLAYER_32_GT:
            capSize = PCIE_CAP_PHYSLAYER_32_GT_SIZE;
            break;

        case PCIE_CAP_ID_PHYSLAYER_64_GT:
            capSize = PCIE_CAP_PHYSLAYER_64_GT_SIZE;
            break;

        case PCIE_CAP_ID_FLT_LOGGING:
            capSize = PCIE_CAP_FLT_LOGGING_SIZE;
            break;

        case PCIE_CAP_ID_DEVICE_3:
            capSize = PCIE_CAP_DEVICE_3_SIZE;
            break;

        case PCIE_CAP_ID_LANE_MARGINING_AT_RECEVER:
            capSize = PCIE_CAP_LANE_MARGINING_AT_RECEVER_SIZE;
            break;

        case PCIE_CAP_ID_ACS:
            capSize = PCIE_CAP_ACS_SIZE;
            break;

        case PCIE_CAP_ID_POWER:
            capSize = PCIE_CAP_POWER_SIZE;
            break;

        case PCIE_CAP_ID_LATENCY_TOLERANCE:
            capSize = PCIE_CAP_LATENCY_TOLERANCE_SIZE;
            break;

        case PCIE_CAP_ID_L1_PM_SUBSTATES:
            capSize = PCIE_CAP_L1_PM_SUBSTATE_SIZE;
            break;

        case PCIE_CAP_ID_ERROR:
            capSize = PCIE_CAP_ERROR_SIZE;
            break;

        case PCIE_CAP_ID_RESIZABLE_BAR:
            capSize = PCIE_CAP_RESIZABLE_BAR_SIZE;
            break;

        case PCIE_CAP_ID_VF_RESIZABLE_BAR:
            capSize = PCIE_CAP_VF_RESIZABLE_BAR_SIZE;
            break;

        case PCIE_CAP_ID_ARI:
            capSize = PCIE_CAP_ARI_SIZE;
            break;

        case PCIE_CAP_ID_PASID:
            capSize = PCIE_CAP_PASID_SIZE;
            break;

        case PCIE_CAP_ID_FRS_QUEUING:
            capSize = PCIE_CAP_FRS_QUEUING_SIZE;
            break;

        case PCIE_CAP_ID_FLIT_PERF_MEASURMENT:
            capSize = PCIE_CAP_FLIT_PERF_MEASURMENT_SIZE;
            break;

        case PCIE_CAP_ID_FLIT_ERROR_INJECTION:
            capSize = PCIE_CAP_FLIT_ERROR_INJECTION_SIZE;
            break;

        case PCIE_CAP_ID_VC:
            capSize = PCIE_VIRTUAL_CHANNELS_BASE_SIZE;
            tempDword = osPciReadDword(deviceHandle, capOffset + PCIE_VC_REGISTER_1);
            count = REF_VAL(PCIE_VC_REGISTER_1_EXTENDED_VC_COUNT, tempDword);
            capSize += count * PCIE_VIRTUAL_CHANNELS_EXTENDED_VC_ENTRY_SIZE;
            // to do: add arbitration tables.
            break;

        case PCIE_CAP_ID_PCIE_CAP_ID_MFVC:
            capSize = PCIE_PCIE_CAP_ID_MFVC_BASE_SIZE;
            tempDword = osPciReadDword(deviceHandle, capOffset + PCIE_MFVC_REGISTER_1);
            count = REF_VAL(PCIE_MFVC_REGISTER_1_EXTENDED_VC_COUNT, tempDword);
            capSize += count * PCIE_PCIE_CAP_ID_MFVC_EXTENDED_VC_ENTRY_SIZE;
            // to do: add arbitration tables.
            break;

        case PCIE_CAP_ID_SERIAL:
            capSize = PCIE_CAP_DEV_SERIAL_SIZE;
            break;

        case PCIE_CAP_ID_VENDOR_SPECIFIC:
            tempDword = osPciReadDword(deviceHandle, capOffset + PCIE_VENDOR_SPECIFIC_HEADER_1);
            capSize = REF_VAL(PCIE_VENDOR_SPECIFIC_HEADER_1_LENGTH, tempDword);
            break;

        case PCIE_CAP_ID_RCRB:
            capSize = PCIE_CAP_RCRB_SIZE;
            break;

        case PCIE_CAP_ID_ROOT_COMPLEX:
            capSize = PCIE_ROOT_COMPLEX_BASE_SIZE;
            tempDword = osPciReadDword(deviceHandle, capOffset + PCIE_ROOT_COMPLEX_SELF_DESC_REGISTER);
            count = REF_VAL(PCIE_ROOT_COMPLEX_SELF_DESC_REGISTER_NUM_LINK_ENTRIES, tempDword);
            capSize += count * PCIE_ROOT_COMPLEX_LINK_ENTRY_SIZE;
            break;

        case PCIE_CAP_ID_ROOT_COMPLEX_INTERNAL_LINK_CTRL:
            capSize = PCIE_CAP_ROOT_COMPLEX_INTERNAL_LINK_CTRL_SIZE;
            break;

        case PCIE_CAP_ID_ROOT_COMPLEX_EVENT_COLLECTOR_ENDPOINT:
            capSize = PCIE_CAP_ROOT_COMPLEX_EVENT_COLLECTOR_ENDPOINT_SIZE;
            break;

        case PCIE_CAP_ID_MULTICAST:
            capSize = PCIE_CAP_MULTICAST_SIZE;
            break;

        case PCIE_CAP_ID_DYNAMIC_POWER_ALLOCATION:
            capSize = PCIE_CAP_DYNAMIC_POWER_ALLOCATION_SIZE;
            break;

        case PCIE_CAP_ID_TPH:
            capSize = PCIE_TPH_BASE_SIZE;
            tempDword = osPciReadDword(deviceHandle, capOffset + PCIE_TPH_REQUESTOR_REGISTER);
            count = REF_VAL(PCIE_TPH_REQUESTOR_REGISTER_ST_TABLE_SIZE, tempDword);
            capSize += count * PCIE_TPH_ST_ENTRY_SIZE;
            break;

        case PCIE_CAP_ID_DPC:
            capSize = PCIE_CAP_DPC_SIZE;
            break;

        case PCIE_CAP_ID_PTM:
            capSize = PCIE_CAP_PTM_SIZE;
            break;

        case PCIE_CAP_ID_READINESS_TIME_REPORTING:
            capSize = PCIE_CAP_READINESS_TIME_REPORTING_SIZE;
            break;

        case PCIE_CAP_ID_HIERARCHY_ID:
            capSize = PCIE_CAP_HIERARCHY_ID_SIZE;
            break;

        case PCIE_CAP_ID_NPEM:
            capSize = PCIE_CAP_NPEM_SIZE;
            break;

        case PCIE_CAP_ID_ALTERNATE_PROTOCOL:
            capSize = PCIE_CAP_ALTERNATE_PROTOCOL_SIZE;
            break;

        case PCIE_CAP_ID_SFI:
            capSize = PCIE_CAP_SFI_SIZE;
            break;

        case PCIE_CAP_ID_DATA_OBJECT_EXCHANGE:
            capSize = PCIE_CAP_DATA_OBJECT_EXCHANGE_SIZE;
            break;

        case PCIE_CAP_ID_SHADOW_FUNCTIONS:
            capSize = PCIE_CAP_SHADOW_FUNCTIONS_SIZE;
            break;

        case PCIE_CAP_ID_IDE:
            capSize = PCIE_CAP_IDE_SIZE;
            break;

        case PCIE_CAP_ID_NULL:
            capSize = PCIE_CAP_NULL_SIZE;
            break;

        default:
            // this is an unrecognized capability.
            // so just grab the header so we can try to figure out what it is
            // (unlike PCI Capabilities PCIE has a specific ID for vendor specific capabilities,
            // so any unrecognized capability is something we do not support)
            capSize = PCIE_CAP_HEADER_SIZE;
            break;
        }
        break;
    }
    return capSize;
}

/*!
 * @brief   create a map of the locations of the capabilities of the specified type of the specified type.
 *
 * @param[in]   deviceHandle    handle to the device whose capability we are getting the size for
 * @param[in]   type            the type of capabilities we are mapping (PCI vs PCIE)
 * @param[out]  capMap          a map of the capabilities found in the configuration space
 *
 * @return  NvU16 size of the requested capacity.
 */
static NvU16 _clPciePopulateCapMap(void * pDeviceHandle, NvU16 type, CL_PCIE_DC_CAPABILITY_MAP * pCapMap)
{
    NvU32 tempDword;
    NvU16 blkOffset = 0;

    if (pCapMap == NULL)
    {
        return 0;
    }
    // clear the capability map.
    portMemSet(pCapMap, 0, sizeof(*pCapMap));

    // if there is not a valid device handle, we are done.
    if (pDeviceHandle == NULL)
    {
        return 0;
    }

    // get the offset to the first block depending on type
    switch (type)
    {
    case RM_PCIE_DC_CAP_TYPE_PCI:
        blkOffset = osPciReadByte(pDeviceHandle, PCI_HEADER_TYPE0_CAP_PTR);
        break;
    case RM_PCIE_DC_CAP_TYPE_PCIE:
        blkOffset = PCIE_CAPABILITY_BASE;
        break;
    default:
        return 0;
        break;
    }
    // run thru the capabilities until we run out of capabilities,
    //   or run out of space
    while (blkOffset != 0)
    {
        // save the offset of the capability
        pCapMap->entries[pCapMap->count].blkOffset = blkOffset;

        // get the capability id & location of next capability
        switch (type)
        {
        case RM_PCIE_DC_CAP_TYPE_PCI:
            // read the capability header
            tempDword = osPciReadWord(pDeviceHandle, blkOffset);

            // extract the capability id
            pCapMap->entries[pCapMap->count].id = REF_VAL(PCI_CAP_HEADER_ID, tempDword);

            // extract the offset for the next capability
            blkOffset = REF_VAL(PCI_CAP_HEADER_NEXT, tempDword);
            break;

        case RM_PCIE_DC_CAP_TYPE_PCIE:
            // read the capability header
            tempDword = osPciReadDword(pDeviceHandle, blkOffset);

            // extract the capability id
            pCapMap->entries[pCapMap->count].id = REF_VAL(PCIE_CAP_HEADER_ID, tempDword);

            // extract the offset for the next capability
            blkOffset = REF_VAL(PCIE_CAP_HEADER_NEXT, tempDword);
            break;
        }
        // move on to the next entry.
        pCapMap->count++;

        if (pCapMap->count >= NV_ARRAY_ELEMENTS(pCapMap->entries))
        {
            // we've run out of space
            break;
        }
    }
    return pCapMap->count;
}

/*!
 * @brief   copy a block of data from the PCI config space to a buffer.
 *
 * @param[out]  pBuffer         a pointer to the buffer that the collected data will be copied to
 * @param[out]  bufferSz        the size of the buffer the data will be stored in
 * @param[in]   deviceHandle    handle to the device whose data is being collected
 * @param[in]   base            the offset of the data to be collected in config space
 * @param[out]  blockSz         size of the data to be collected.
 *
 * @return  NvU16 size of the collected.
 */
static NvU16 _clPcieCopyConfigSpaceDiagData(NvU8* pBuffer, NvU32 bufferSz, void *pDeviceHandle, NvU32 base, NvU32 blockSz)
{
    NvU32   offset = base;
    NvU16   dataSz = 0;

    if ((pBuffer == NULL) || (bufferSz == 0) || (pDeviceHandle == NULL) || blockSz > (bufferSz))
    {
        return 0;
    }

    // switch on the boundary we are at so we can do whatever we need to reach a dword boundary.
    switch (offset & 0x03)
    {
    case 0:             // we are at a dword boundary. move on to the DWORD copies
        break;

    case 1:             // we are on a byte boundary, read a byte to get us to a word boundary
    case 3:
        *pBuffer = osPciReadByte(pDeviceHandle, offset);
        pBuffer += 1;
        offset += 1;
        dataSz += 1;

        // did we reach a dword boundary?
        if ((offset & 0x03) == 0)
        {
            break;
        }
        // fall thru to read next word & bring us to a DWORD boundary;

    case 2:             // we are on a word boundary,
        // if we need at least a word of data,
        // read another word to get us to a dword boundary
        // if we need less than a word, we will pick it up below.
        if ((blockSz - dataSz) > 1)
        {
            *((NvU16*)pBuffer) = osPciReadWord(pDeviceHandle, offset);
            pBuffer += 2;
            offset += 2;
            dataSz += 2;
        }
        break;
    }

    // do all the full dword reads
    for (; (blockSz - dataSz) >= sizeof(NvU32); dataSz += sizeof(NvU32))
    {
        // read a dword of data
        *((NvU32*)pBuffer) = osPciReadDword(pDeviceHandle, offset);

        // update the references to the next data to be read/written.
        pBuffer += sizeof(NvU32);
        offset += sizeof(NvU32);
    }

    // we are at the nearest dword boundary to the end of the block.
    // read any remaining data.
    switch (blockSz - dataSz)
    {
    default:    // something is wrong, we shouldn't have more than 3 bytes to get.
        break;

    case 0:     // we have everything we want.
        break;

    case 2:     // we have at least a word left, read the word.
    case 3:
        *((NvU16*)pBuffer) = osPciReadWord(pDeviceHandle, offset);
        pBuffer += 2;
        offset += 2;
        dataSz += 2;
        // did we get everything?
        if ((blockSz - dataSz) == 0)
        {
            break;
        }
        // fall thru to grab the last byte.

    case 1:     // we have 1 byte left, read it as a byte.
        *pBuffer = osPciReadByte(pDeviceHandle, offset);
        dataSz += 1;
        break;
    }
    return dataSz;
}

/*!
 * @brief   Save the header & data for a diagnostic block
 *
 * @param[in]   pDeviceHandle   handle of device we are collecting data from
 *                              if NULL data will not be collected from config space
 * @param[in]   pScriptEntry    pointer to collection script entry we are collecting data for
 *                              may be NULL in which case only the data is transferred
 * @param[in]   blkOffset       offset in PCIE config space we are collecting
 * @param[in]   blkSize         size of the block in config space we are collecting
 * @param[out]  pBuffer         pointer to diagnostic output buffer
 * @param[in]   bufferSize      size of diagnostic output buffer
 *
 * @return  NvU16 size of diagnostic data collected.
 */
NvU16 _clPcieSavePcieDiagnosticBlock(void *pDeviceHandle, CL_PCIE_DC_DIAGNOSTIC_COLLECTION_ENTRY *pScriptEntry, NvU16 blkOffset, NvU16 blkSize, void * pBuffer, NvU32 size)
{
    CL_PCIE_DC_DIAGNOSTIC_COLLECTION_ENTRY *pBlkHeader;
    NvU16 collectedDataSize = 0;

    // do some parameter checks
    if ((pBuffer == NULL)
        || ((blkOffset + sizeof(*pBlkHeader) + blkSize) > PCI_EXTENDED_CONFIG_SPACE_LENGTH)
        || ((sizeof(*pBlkHeader) + blkSize) > size))
    {
        return 0;
    }
    if (pScriptEntry != NULL)
    {
        // copy the block header & update the size
        pBlkHeader = (CL_PCIE_DC_DIAGNOSTIC_COLLECTION_ENTRY*)pBuffer;
        *pBlkHeader = *pScriptEntry;
        collectedDataSize += sizeof(*pBlkHeader);
    }
    if((pDeviceHandle != NULL)
        && (0 < blkSize)
        && (pBuffer != NULL))
    {
        // get the data block
        collectedDataSize += _clPcieCopyConfigSpaceDiagData(&(((NvU8*)pBuffer)[collectedDataSize]), size - collectedDataSize,
            pDeviceHandle,
            blkOffset, blkSize);
    }
    return collectedDataSize;
}

/*!
 * @brief   Retrieve diagnostic information to be used to identify the cause of errors based on a provided script
 *
 * @param[in]   pGpu        GPU object pointer
 * @param[in]   pScript     pointer to collection script
 * @param[in]   count       number of entries in the collection script
 * @param[out]  pBuffer     pointer to diagnostic output buffer
 * @param[in]   bufferSize  size of diagnostic output buffer
 *
 * @return  NvU16 size of diagnostic data collected.
 */
NvU16 _clPcieGetDiagnosticData(OBJGPU *pGpu, CL_PCIE_DC_DIAGNOSTIC_COLLECTION_ENTRY *pScript, NvU16 count, NvU8 * pBuffer, NvU32 size)
{
    static volatile NvS32   capMapWriteLock = 0;
    static volatile NvBool  capMapInitialized = NV_FALSE;
    static CL_PCIE_DC_CAPABILITY_MAP
                            capMap[RM_PCIE_DEVICE_COUNT][RM_PCIE_DC_CAP_TYPE_COUNT];
    NBADDR                  *pUpstreamPort = NULL;
    NvU32                   domain = 0;
    NvU8                    bus = 0;
    NvU8                    device = 0;
    NvU16                   vendorId, deviceId;
    void                    *pPCIeHandles[RM_PCIE_DEVICE_COUNT];
    NvU16                   collectedDataSize = 0;
    NvU16                   idx;
    NvU16                   idx2;
    CL_PCIE_DC_CAPABILITY_MAP
                            *pActiveMap = NULL;
    CL_PCIE_DC_DIAGNOSTIC_COLLECTION_ENTRY
                            blkHeader;
    NvU16                   capType;
    NvBool                  bCollectAll;

    if ((pGpu == NULL) || (pBuffer == NULL) || (size == 0) || IS_RTLSIM(pGpu))
    {
        return 0;
    }
    portMemSet(pPCIeHandles, 0, sizeof(pPCIeHandles));

    // get PCIE Handles.
    pUpstreamPort = &pGpu->gpuClData.upstreamPort.addr;
    pPCIeHandles[RM_PCIE_DEVICE_TYPE_UPSTREAM_BRIDGE] = osPciInitHandle(pUpstreamPort->domain,
        pUpstreamPort->bus,
        pUpstreamPort->device,
        0,
        &vendorId,
        &deviceId);
    if (osGpuReadReg032(pGpu, PCI_HEADER_TYPE0_VENDOR_ID) == pGpu->chipId0)
    {
        domain = gpuGetDomain(pGpu);
        bus = gpuGetBus(pGpu);
        device = gpuGetDevice(pGpu);
        pPCIeHandles[RM_PCIE_DEVICE_TYPE_GPU] = osPciInitHandle(domain, bus, device, 0, &vendorId, &deviceId);
    }

    // run thru the collection script entries.
    for (idx = 0; idx < count; idx++)
    {
        blkHeader = pScript[idx];
        // verify there is space, & we can access the device.
        if ((size - collectedDataSize) < (sizeof(blkHeader) + blkHeader.length))
        {
            // if this block doesn't fit, skip it but continue because another block might.
            continue;
        }
        if ((blkHeader.deviceType == RM_PCIE_DEVICE_TYPE_NONE) || (blkHeader.action == RM_PCIE_ACTION_EOS))
        {
            NV_ASSERT(blkHeader.action == RM_PCIE_ACTION_EOS);
            break;
        }
        if (pPCIeHandles[blkHeader.deviceType] == NULL)
        {
            continue;
        }
        switch (blkHeader.action)
        {
        case RM_PCIE_ACTION_NOP:
            break;

        case RM_PCIE_ACTION_COLLECT_CONFIG_SPACE:
            collectedDataSize += _clPcieSavePcieDiagnosticBlock(pPCIeHandles[blkHeader.deviceType],
                &blkHeader,
                blkHeader.locator, blkHeader.length,
                &pBuffer[collectedDataSize], size - collectedDataSize);
            break;

        case RM_PCIE_ACTION_COLLECT_PCI_CAP_STRUCT:
        case RM_PCIE_ACTION_COLLECT_PCIE_CAP_STRUCT:
        case RM_PCIE_ACTION_COLLECT_ALL_PCI_CAPS:
        case RM_PCIE_ACTION_COLLECT_ALL_PCIE_CAPS:

            if (portAtomicIncrementS32(&capMapWriteLock) == 1)
            {
                if (!capMapInitialized)
                {
                    // map out all the capabilities
                    for (idx = 0; idx < NV_ARRAY_ELEMENTS(capMap); idx++)
                    {
                        for (idx2 = 0; idx2 < NV_ARRAY_ELEMENTS(capMap[0]); idx2++)
                        {
                            _clPciePopulateCapMap(pPCIeHandles[idx], idx2, &capMap[idx][idx2]);
                        }
                    }
                    capMapInitialized = NV_TRUE;
                }
            }
            portAtomicDecrementS32(&capMapWriteLock);

            if (!capMapInitialized)
            {
                break;
            }
            // figure out which map to use.
            switch (blkHeader.action)
            {
            case RM_PCIE_ACTION_COLLECT_PCI_CAP_STRUCT:
            case RM_PCIE_ACTION_COLLECT_ALL_PCI_CAPS:
                capType = RM_PCIE_DC_CAP_TYPE_PCI;
                break;

            case RM_PCIE_ACTION_COLLECT_PCIE_CAP_STRUCT:
            case RM_PCIE_ACTION_COLLECT_ALL_PCIE_CAPS:
                capType = RM_PCIE_DC_CAP_TYPE_PCIE;
                break;
            default:
                // will never happen, but having this default handling
                // deals with Linux compiler capType may not be initialized before use warning.
                capType = RM_PCIE_DC_CAP_TYPE_NONE;
                break;
            }

            // even if I don't think it can happen, need to check for it.
            if (capType == RM_PCIE_DC_CAP_TYPE_NONE)
            {
                break;
            }
            pActiveMap = &capMap[blkHeader.deviceType][capType];

            // collect the data using the map determined above
            switch (blkHeader.action)
            {
            default:    // default needed to satisfy Linux compiler.
            case RM_PCIE_ACTION_COLLECT_PCI_CAP_STRUCT:
            case RM_PCIE_ACTION_COLLECT_PCIE_CAP_STRUCT:
                bCollectAll = NV_FALSE;
                break;

            case RM_PCIE_ACTION_COLLECT_ALL_PCI_CAPS:
            case RM_PCIE_ACTION_COLLECT_ALL_PCIE_CAPS:
                bCollectAll = NV_TRUE;
                break;
            }
            // find the requested capability within the specified type
            for (idx2 = 0; idx2 < pActiveMap->count; idx2++)
            {
                if (bCollectAll || (pActiveMap->entries[idx2].id == blkHeader.locator))
                {
                    blkHeader.locator = pActiveMap->entries[idx2].id;

                    // determine the size of the capability
                    blkHeader.length =
                        _clPcieGetPcieCapSize(pPCIeHandles[blkHeader.deviceType],
                            capType, blkHeader.locator, pActiveMap->entries[idx2].blkOffset);
                    collectedDataSize += _clPcieSavePcieDiagnosticBlock(pPCIeHandles[blkHeader.deviceType],
                        &blkHeader,
                        pActiveMap->entries[idx2].blkOffset, blkHeader.length,
                        &pBuffer[collectedDataSize], size - collectedDataSize);
                }
            }
            break;

        case RM_PCIE_ACTION_REPORT_PCI_CAPS_COUNT:
            blkHeader.length = capMap[blkHeader.deviceType][RM_PCIE_DC_CAP_TYPE_PCI].count;
            collectedDataSize += _clPcieSavePcieDiagnosticBlock(NULL,
                &blkHeader,
                0, 0,
                &pBuffer[collectedDataSize], size - collectedDataSize);
            break;

        case RM_PCIE_ACTION_REPORT_PCIE_CAPS_COUNT:
            blkHeader.length = capMap[blkHeader.deviceType][RM_PCIE_DC_CAP_TYPE_PCIE].count;
            collectedDataSize += _clPcieSavePcieDiagnosticBlock(NULL,
                &blkHeader,
                0, 0,
                &pBuffer[collectedDataSize], size - collectedDataSize);
            break;

        default:
            break;
        }
    }
    return collectedDataSize;
}

/*!
 * @brief   Retrieve diagnostic information to be used to identify the cause of GPU Lost
 *
 * @param[in]   pGpu        GPU object pointer
 * @param[in]   pCl         CL object pointer
 * @param[out]  pBuffer     pointer to diagnostic output buffer
 * @param[in]   bufferSize  size of diagnostic output buffer
 *
 * @return  NvU32 size of diagnostic data collected.
 */
NvU16 clPcieGetGpuLostDiagnosticData_IMPL(OBJGPU *pGpu, OBJCL *pCl, NvU8 * pBuffer, NvU32 size)
{
    static CL_PCIE_DC_DIAGNOSTIC_COLLECTION_ENTRY gpuLostCollectionScript[] =
    {
        { RM_PCIE_ACTION_COLLECT_CONFIG_SPACE,     RM_PCIE_DEVICE_TYPE_UPSTREAM_BRIDGE, 0x000,                        0x40 },
        { RM_PCIE_ACTION_COLLECT_CONFIG_SPACE,     RM_PCIE_DEVICE_TYPE_GPU,             0x000,                        0x40 },
        { RM_PCIE_ACTION_COLLECT_ALL_PCI_CAPS,     RM_PCIE_DEVICE_TYPE_GPU,             0,                            0    },
        { RM_PCIE_ACTION_COLLECT_ALL_PCIE_CAPS,    RM_PCIE_DEVICE_TYPE_GPU,             0,                            0    },
        { RM_PCIE_ACTION_COLLECT_ALL_PCI_CAPS,     RM_PCIE_DEVICE_TYPE_UPSTREAM_BRIDGE, 0,                            0    },
        { RM_PCIE_ACTION_COLLECT_ALL_PCIE_CAPS,    RM_PCIE_DEVICE_TYPE_UPSTREAM_BRIDGE, 0,                            0    },
        { RM_PCIE_ACTION_EOS,                      RM_PCIE_DEVICE_TYPE_NONE,            0,                            0    }
    };

    return _clPcieGetDiagnosticData(pGpu,
        gpuLostCollectionScript, NV_ARRAY_ELEMENTS(gpuLostCollectionScript),
        pBuffer, size);
}

/*!
 * @brief Parse config space for ACS redirect configuration.
 *
 * @param[in]  pGpu                GPU object pointer
 * @param[in]  pCl                 CL object pointer
 * @param[in]  domain              DBDF domain
 * @param[in]  bus                 DBDF bus
 * @param[in]  device              DBDF device
 * @param[in]  func                DBDF function
 * @param[out] pAcsRoutingConfig  ACS routing ctrl value filtered by capability field.
 *
 * @returns NV_OK on success, NV_ERR_INVALID_STATE in case config space is inaccessible, NV_ERR_GENERIC if ACS is unsupported.
 */
NV_STATUS
clGetPortAcsRedirectConfig_IMPL
(
    OBJGPU *pGpu,
    OBJCL  *pCl,
    NvU32   domain,
    NvU8    bus,
    NvU8    device,
    NvU8    func,
    NvU32  *pAcsRoutingConfig
)
{
    PORTDATA portData = {0};
    NvU32 acsCtrl;
    NvU32 acsCap;

    *pAcsRoutingConfig = 0;

    // Initialize portData struct for the PCI node.
    portData.addr.domain = domain;
    portData.addr.bus    = bus;
    portData.addr.device = device;
    portData.addr.func   = func;
    portData.addr.valid  = 0x1;
    portData.addr.handle = osPciInitHandle(domain, bus, device, func, 0, 0);

    // This could be faster if RM implemented caching of extended config space.
    NV_ASSERT_OK_OR_RETURN(objClSetPortPcieEnhancedCapsOffsets(pCl, &portData));

    // If ACS is not implemented by the bridge or RC, RM will return here. This is not an error.
    if (clPcieReadPortConfigReg(pGpu, pCl, &portData, CL_ACS_CAP, &acsCap) != NV_OK)
    {
        return NV_OK;
    }
    if (clPcieReadPortConfigReg(pGpu, pCl, &portData, CL_ACS_CTRL, &acsCtrl) != NV_OK)
    {
        return NV_OK;
    }

    // Ctrl bits set without a corresponding capability bit are filtered.
    *pAcsRoutingConfig = acsCtrl & acsCap;

    return NV_OK;
}
