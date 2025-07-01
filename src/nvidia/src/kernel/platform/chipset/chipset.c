/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


/***************************** HW State Routines ***************************\
*         Core Logic Object Function Definitions.                           *
\***************************************************************************/

#include "core/system.h"
#include "platform/chipset/chipset.h"
#include "platform/platform.h"
#include "platform/chipset/chipset_info.h"
#include "os/os.h"
#include "nvrm_registry.h"
#include "nvpcie.h"
#include "nv_ref.h"
#include "kernel/gpu/bif/kernel_bif.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/gsp/gsp_static_config.h"
#include "ctrl/ctrl2080/ctrl2080bus.h"

// local static funcs
static void clDestructHWBC(OBJHWBC *pHWBC);

NV_STATUS
clConstruct_IMPL(OBJCL *pCl)
{
    // Used to track when the link has gone into Recovery, which can cause CEs.
    pCl->EnteredRecoverySinceErrorsLastChecked = NV_FALSE;

    pCl->pPcieConfigSpaceBase = NULL;

    //
    // We set this property by default.
    // Chipset setup function can override this.
    // Right now only Tegra chipsets overide this setting.
    //
    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IO_COHERENT, NV_TRUE);

    return NV_OK;
}


void
clInitPropertiesFromRegistry_IMPL(OBJGPU *pGpu, OBJCL *pCl)
{
    NvU32  data32;

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_DISABLE_BR03_FLOW_CONTROL, &data32) == NV_OK
            && data32)
    {
        pCl->setProperty(pCl, PDB_PROP_CL_DISABLE_BR03_FLOW_CONTROL, NV_TRUE);
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_FORCE_ENABLE_GEN2, &data32) == NV_OK)
    {
        if (NV_REG_STR_RM_FORCE_ENABLE_GEN2_YES == data32)
        {
            pCl->setProperty(pCl, PDB_PROP_CL_PCIE_FORCE_GEN2_ENABLE, NV_TRUE);
        }
    }

    if ((osReadRegistryDword(pGpu, NV_REG_STR_RM_FORCE_DISABLE_IOMAP_WC, &data32) == NV_OK)
            && (data32 == NV_REG_STR_RM_FORCE_DISABLE_IOMAP_WC_YES))
    {
        pCl->setProperty(pCl, PDB_PROP_CL_DISABLE_IOMAP_WC, NV_TRUE);
    }

    osQADbgRegistryInit();
}

static void
clDestructHWBC(OBJHWBC *pHWBC)
{
    if (pHWBC->pSibling)
    {
        clDestructHWBC(pHWBC->pSibling);
        pHWBC->pSibling = NULL;
    }
    if (pHWBC->pFirstChild)
    {
        clDestructHWBC(pHWBC->pFirstChild);
        pHWBC->pFirstChild = NULL;
    }

    portMemFree(pHWBC);
}

void
clDestruct_IMPL(OBJCL *pCl)
{
    if (pCl->pHWBC)
    {
        clDestructHWBC(pCl->pHWBC);
        pCl->pHWBC = NULL;
    }

    clFreeBusTopologyCache(pCl);

    clFreePcieConfigSpaceBase(pCl);
}

//
// Find our NV device on the PCI bus and save it's pci bus/device address.
//
NvU32
clInitMappingPciBusDevice_IMPL
(
    OBJGPU       *pGpu,
    OBJCL        *pCl
)
{
    void *handle;
    NvU32 domain;
    NvU16 bus;
    NvU8 device;
    NvU16 vendorID, deviceID;
    NvBool bFoundDevice = NV_FALSE;

    if (IsT194(pGpu) || IsT234(pGpu))
        return NV0000_CTRL_GPU_INVALID_ID;

    // do we already know our domain/bus/device?
    if (!gpuIsDBDFValid(pGpu))
    {
        // we're checking all the device/funcs for the first 10 buses!
        // Note that we give up the enumeration once we find our first
        // or in the Multichip case our second device.
        for (domain = 0; (domain < PCI_MAX_DOMAINS) && !bFoundDevice; domain++)
        {
            for (bus = 0; (bus < PCI_MAX_BUSES) && !bFoundDevice; bus++)
            {
                for (device = 0; device < PCI_MAX_DEVICES; device++)
                {
                    // read at domain, bus, device (we're always function 0)
                    handle = osPciInitHandle(domain, (NvU8) bus, device, 0, &vendorID, &deviceID);
                    if (!handle)
                        continue;

                    if (vendorID != NV_CONFIG_PCI_NV_0_VENDOR_ID_NVIDIA)
                        continue;

                    // make sure we're a VGA device class
                    if ((osPciReadByte(handle, 0xB)) != PCI_CLASS_DISPLAY_DEV)
                        continue;

                    // if the BAR0 matches our PhysAddr, it's the correct device
                    if (((osPciReadDword(handle, PCI_BASE_ADDRESS_0) & PCI_BASE_ADDRESS_0_VALID_MASK)) !=
                        pGpu->busInfo.gpuPhysAddr)
                        continue;

                    // save our domain/bus/device/function
                    pGpu->busInfo.nvDomainBusDeviceFunc = gpuEncodeDomainBusDevice(domain, (NvU8)bus, device);
                    pGpu->busInfo.bNvDomainBusDeviceFuncValid = NV_TRUE;

                    bFoundDevice = NV_TRUE;

                    break;
                }
            }
        }
    }

    domain = gpuGetDomain(pGpu);
    bus = gpuGetBus(pGpu);
    device = gpuGetDevice(pGpu);

    if (!gpuIsDBDFValid(pGpu))
    {
        {
            NV_PRINTF(LEVEL_ERROR,
                    "NVRM initMappingPciBusDevice: can't find a device!\n");
            return NV0000_CTRL_GPU_INVALID_ID;    // couldn't find it
        }
    }

    return gpuGenerate32BitId(domain, (NvU8)bus, device);
}

//
// Walk the PCIE Capabilities and if the subsystem ID is found then return
// the subvendorID and subdeviceID
//
static void getSubsystemFromPCIECapabilities
(
    NvU32 domain,
    NvU8 bus,
    NvU8 device,
    NvU8 func,
    NvU16 *subvendorID,
    NvU16 *subdeviceID
)
{
    void *handle;
    NvU32 PCIECapPtr;
    NvU32 PCIECap;
    NvU32 PCIECapNext;

    handle = osPciInitHandle(domain, bus, device, func, NULL, NULL);

    // We start from Cap. List and search for Subsystem ID Capability
    PCIECapNext = osPciReadByte(handle, PCI_CAPABILITY_LIST);
    if (PCIECapNext)
    {
        do
        {
            PCIECapPtr = PCIECapNext;
            PCIECap = osPciReadDword(handle, CL_PCIE_CAP - CL_PCIE_BEGIN + PCIECapPtr);
            PCIECapNext = (NvU8)((PCIECap >> 8) & 0xFF);
        } while ((PCIECapNext != 0) &&
                 ((PCIECap & CAP_ID_MASK) != CAP_ID_SUBSYSTEM_ID));

        if ((PCIECap & CAP_ID_MASK) == CAP_ID_SUBSYSTEM_ID)
        {
            if (subvendorID)
            {
                *subvendorID = osPciReadWord(handle, CL_PCIE_CAP - CL_PCIE_BEGIN + PCIECapPtr + 4);
            }
            if (subdeviceID)
            {
                *subdeviceID = osPciReadWord(handle, CL_PCIE_CAP - CL_PCIE_BEGIN + PCIECapPtr + 6);
            }
        }
    }
}

//
// PCI Express Support
// Find first host bridge's domain, bus, device, function, if not already found
//

NV_STATUS
clFindFHBAndGetChipsetInfoIndex_IMPL
(
    OBJCL *pCl,
    NvU16 *pChipsetInfoIndex
)
{
    void     *handle;
    NvU32     domain;
    NvU8      revisionID = 0;
    NvU32     i;
    NvBool    matchFound = NV_FALSE;
    NvU16     bus;
    NvU8      device, func;
    NvU16     vendorID, deviceID, subvendorID, subdeviceID;
    PBUSTOPOLOGYINFO pBusTopologyInfo;
    //
    // PC motherboards have a host bridge to connect PCIE root complex to rest of the system.
    // However, Tegra devices only have a PCI-to-PCI bridge.
    // So allow Tegra chipset initialization, even if a host bridge is not found (SUBBASECLASS_P2P).
    // See bug 1547160 comment#17 for more details (dGPU connected with Tegra device through PCIe).
    // There is no host bridge or PCI-to-PCI bridge present in between
    // Tegra iGPUs and PCIe-RC. Tegra iGPUs are directly connected to endpoint.
    // PCI config for Sub Base Class code will always be programmed as
    // PCI_COMMON_CLASS_SUBBASECLASS_3DCTRL for Tegra iGPUs.
    //
    NvU16    pciSubBaseClass[] = {PCI_COMMON_CLASS_SUBBASECLASS_HOST,
        PCI_COMMON_CLASS_SUBBASECLASS_P2P, PCI_COMMON_CLASS_SUBBASECLASS_3DCTRL};

    // return it, if we've got it already
    if (pCl->chipsetIDBusAddr.valid)
        return NV_OK;

    // Initialize to 0
    pCl->chipsetIDBusAddr.domain = 0x0;
    pCl->chipsetIDBusAddr.bus    = 0x0;
    pCl->chipsetIDBusAddr.device = 0x0;
    pCl->chipsetIDBusAddr.func   = 0x0;
    pCl->chipsetIDBusAddr.valid  = 0x0;
    pCl->chipsetIDBusAddr.handle = NULL;
    pCl->chipsetIDInfo.deviceID    = PCI_INVALID_DEVICEID;
    pCl->chipsetIDInfo.vendorID    = PCI_INVALID_VENDORID;
    pCl->chipsetIDInfo.subdeviceID = PCI_INVALID_SUBDEVICEID;
    pCl->chipsetIDInfo.subvendorID = PCI_INVALID_SUBVENDORID;

    for (i = 0; chipsetInfo[i].vendorID; i++)
    {
        pBusTopologyInfo = pCl->pBusTopologyInfo;
        while (pBusTopologyInfo)
        {
            if ((pBusTopologyInfo->busInfo.vendorID == chipsetInfo[i].vendorID) &&
                (pBusTopologyInfo->busInfo.deviceID == chipsetInfo[i].deviceID))
            {
                matchFound = NV_TRUE;
                break;
            }
            pBusTopologyInfo = pBusTopologyInfo->next;
        }

        if (matchFound)
        {
            if  (pChipsetInfoIndex != NULL)
            {
                *pChipsetInfoIndex = (NvU16) i;
            }

            //
            // IBM Wildhorse system has NV chipset attached to secondary K8 at bus 0x80
            // (bug 227308).
            // Do not change the algorithm for older chipsets where the devcie at bus%0x40 ==0, 0, 0 is
            // considered as a host bridge.
            //
            if (((pBusTopologyInfo->pciSubBaseClass & 0xFF) == PCI_SUBCLASS_BR_HOST) ||
                (!(pBusTopologyInfo->bus % 0x40) && !pBusTopologyInfo->device && !pBusTopologyInfo->func))
            {
                pCl->FHBAddr.domain = pBusTopologyInfo->domain;
                pCl->FHBAddr.bus    = pBusTopologyInfo->bus;
                pCl->FHBAddr.device = pBusTopologyInfo->device;
                pCl->FHBAddr.func   = pBusTopologyInfo->func;
                pCl->FHBAddr.valid  = 0x1;
                pCl->FHBAddr.handle = pBusTopologyInfo->handle;

                // Store a copy of deviceID,  vendorID, subdeviceID and subvendorID;
                pCl->FHBBusInfo.deviceID    = pBusTopologyInfo->busInfo.deviceID;
                pCl->FHBBusInfo.vendorID    = pBusTopologyInfo->busInfo.vendorID;
                pCl->FHBBusInfo.subdeviceID = pBusTopologyInfo->busInfo.subdeviceID;
                pCl->FHBBusInfo.subvendorID = pBusTopologyInfo->busInfo.subvendorID;
                pCl->FHBBusInfo.revisionID  = pBusTopologyInfo->busInfo.revisionID;

                pCl->chipsetIDBusAddr.domain = pBusTopologyInfo->domain;
                pCl->chipsetIDBusAddr.bus    = pBusTopologyInfo->bus;
                pCl->chipsetIDBusAddr.device = pBusTopologyInfo->device;
                pCl->chipsetIDBusAddr.func   = pBusTopologyInfo->func;
                pCl->chipsetIDBusAddr.valid  = 0x1;
                pCl->chipsetIDBusAddr.handle = pBusTopologyInfo->handle;

                // Store a copy of deviceID,  vendorID, subdeviceID and subvendorID;
                pCl->chipsetIDInfo.deviceID    = pBusTopologyInfo->busInfo.deviceID;
                pCl->chipsetIDInfo.vendorID    = pBusTopologyInfo->busInfo.vendorID;
                pCl->chipsetIDInfo.subdeviceID = pBusTopologyInfo->busInfo.subdeviceID;
                pCl->chipsetIDInfo.subvendorID = pBusTopologyInfo->busInfo.subvendorID;
                return NV_OK;
            }
            else
            {
                pCl->chipsetIDBusAddr.domain = pBusTopologyInfo->domain;
                pCl->chipsetIDBusAddr.bus    = pBusTopologyInfo->bus;
                pCl->chipsetIDBusAddr.device = pBusTopologyInfo->device;
                pCl->chipsetIDBusAddr.func   = pBusTopologyInfo->func;
                pCl->chipsetIDBusAddr.valid  = 0x1;
                pCl->chipsetIDBusAddr.handle = pBusTopologyInfo->handle;

                // Store a copy of deviceID,  vendorID, subdeviceID and subvendorID;
                pCl->chipsetIDInfo.deviceID    = pBusTopologyInfo->busInfo.deviceID;
                pCl->chipsetIDInfo.vendorID    = pBusTopologyInfo->busInfo.vendorID;
                pCl->chipsetIDInfo.subdeviceID = pBusTopologyInfo->busInfo.subdeviceID;
                pCl->chipsetIDInfo.subvendorID = pBusTopologyInfo->busInfo.subvendorID;

                if (pCl->chipsetIDInfo.subvendorID == 0)
                {
                    getSubsystemFromPCIECapabilities(pCl->chipsetIDBusAddr.domain,
                                                     pCl->chipsetIDBusAddr.bus,
                                                     pCl->chipsetIDBusAddr.device,
                                                     pCl->chipsetIDBusAddr.func,
                                                     &pCl->chipsetIDInfo.subvendorID,
                                                     &pCl->chipsetIDInfo.subdeviceID);
                }
                break;
            }
        }
    }

    if ((!matchFound) && (pChipsetInfoIndex != NULL))
    {
        // This should be the entry with NULL information
        NV_ASSERT(chipsetInfo[i].vendorID == 0);
        *pChipsetInfoIndex = (NvU16) i;
    }

    //
    // We are here because VendorId and deviceId in chipsetInfo table does not
    // match with Host Bridge ID. In that case we need to find FHB either in
    // cached bus topology or need to loop through PCI bus to find the FHB.
    //
    for (i = 0; i < NV_ARRAY_ELEMENTS(pciSubBaseClass); i++)
    {
        pBusTopologyInfo = pCl->pBusTopologyInfo;
        while (pBusTopologyInfo)
        {
            if (pBusTopologyInfo->pciSubBaseClass == pciSubBaseClass[i])
            {
                pCl->FHBAddr.domain = pBusTopologyInfo->domain;
                pCl->FHBAddr.bus    = pBusTopologyInfo->bus;
                pCl->FHBAddr.device = pBusTopologyInfo->device;
                pCl->FHBAddr.func   = pBusTopologyInfo->func;
                pCl->FHBAddr.valid  = 0x1;
                pCl->FHBAddr.handle = pBusTopologyInfo->handle;

                // Store a copy of deviceID,  vendorID, subdeviceID and subvendorID;
                pCl->FHBBusInfo.deviceID    = pBusTopologyInfo->busInfo.deviceID;
                pCl->FHBBusInfo.vendorID    = pBusTopologyInfo->busInfo.vendorID;
                pCl->FHBBusInfo.subdeviceID = pBusTopologyInfo->busInfo.subdeviceID;
                pCl->FHBBusInfo.subvendorID = pBusTopologyInfo->busInfo.subvendorID;
                pCl->FHBBusInfo.revisionID  = pBusTopologyInfo->busInfo.revisionID;

                if (!matchFound)
                {
                    pCl->chipsetIDBusAddr.domain = pBusTopologyInfo->domain;
                    pCl->chipsetIDBusAddr.bus    = pBusTopologyInfo->bus;
                    pCl->chipsetIDBusAddr.device = pBusTopologyInfo->device;
                    pCl->chipsetIDBusAddr.func   = pBusTopologyInfo->func;
                    pCl->chipsetIDBusAddr.valid  = 0x1;
                    pCl->chipsetIDBusAddr.handle = pBusTopologyInfo->handle;

                    // Store a copy of deviceID,  vendorID, subdeviceID and subvendorID;
                    pCl->chipsetIDInfo.deviceID    = pBusTopologyInfo->busInfo.deviceID;
                    pCl->chipsetIDInfo.vendorID    = pBusTopologyInfo->busInfo.vendorID;
                    pCl->chipsetIDInfo.subdeviceID = pBusTopologyInfo->busInfo.subdeviceID;
                    pCl->chipsetIDInfo.subvendorID = pBusTopologyInfo->busInfo.subvendorID;

                    if (pCl->chipsetIDInfo.subvendorID == 0)
                    {
                        getSubsystemFromPCIECapabilities(pCl->chipsetIDBusAddr.domain,
                                                         pCl->chipsetIDBusAddr.bus,
                                                         pCl->chipsetIDBusAddr.device,
                                                         pCl->chipsetIDBusAddr.func,
                                                         &pCl->chipsetIDInfo.subvendorID,
                                                         &pCl->chipsetIDInfo.subdeviceID);
                    }
                }

                NV_PRINTF(LEVEL_INFO,
                          "DeviceId[%x] VendorID[%x] BDF[%x:%x:%x] SubClassId[%x] device found.\n",
                          pBusTopologyInfo->busInfo.deviceID,
                          pBusTopologyInfo->busInfo.vendorID,
                          pBusTopologyInfo->bus,
                          pBusTopologyInfo->device,
                          pBusTopologyInfo->func,
                          pciSubBaseClass[i]);

                return NV_OK;
            }
            pBusTopologyInfo = pBusTopologyInfo->next;
        }
    }

    NV_PRINTF(LEVEL_ERROR,
              "NVRM : This is Bad. FHB/P2P/3DCTRL not found in cached bus topology!!!\n");

    // HB/P2P/3DCTRL is not present in cached bus topology.
    NV_ASSERT(0);

    //
    // Don't bother scanning all domains, which takes approximately forever.
    // If we can't find it in domain 0, we're probably not going to anyway
    // (and something is already wrong).
    //
    domain = 0;

    for (bus = 0; bus < PCI_MAX_BUSES; bus++)
    {
        for (device = 0; device < PCI_MAX_DEVICES; device++)
        {
            for (func = 0; func < PCI_MAX_FUNCTION; func++)
            {
                // read at domain, bus, device, func
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
                    break;           // skip to the next device

                if ((osPciReadByte(handle, PCI_HEADER_TYPE0_BASECLASS)) != PCI_CLASS_BRIDGE_DEV)
                    break;           // not a bridge device

                if ((osPciReadByte(handle, PCI_HEADER_TYPE0_SUBCLASS))  != PCI_SUBCLASS_BR_HOST)
                    break;           // not a host bridge

                subdeviceID = osPciReadWord(handle, PCI_HEADER_TYPE0_SUBSYS_ID);
                subvendorID = osPciReadWord(handle, PCI_HEADER_TYPE0_SUBSYS_VEN_ID);
                revisionID  = osPciReadByte(handle, PCI_HEADER_TYPE0_REVISION_ID);

                // Found it
                pCl->FHBAddr.domain = domain;
                pCl->FHBAddr.bus    = (NvU8)bus;
                pCl->FHBAddr.device = device;
                pCl->FHBAddr.func   = func;
                pCl->FHBAddr.valid  = 0x1;
                pCl->FHBAddr.handle = handle;

                // Store a copy of deviceID,  vendorID, subdeviceID and subvendorID;
                pCl->FHBBusInfo.deviceID    = deviceID;
                pCl->FHBBusInfo.vendorID    = vendorID;
                pCl->FHBBusInfo.subdeviceID = subdeviceID;
                pCl->FHBBusInfo.subvendorID = subvendorID;
                pCl->FHBBusInfo.revisionID  = revisionID;

                if (!matchFound)
                {
                    pCl->chipsetIDBusAddr.domain = domain;
                    pCl->chipsetIDBusAddr.bus    = (NvU8)bus;
                    pCl->chipsetIDBusAddr.device = device;
                    pCl->chipsetIDBusAddr.func   = func;
                    pCl->chipsetIDBusAddr.valid  = 0x1;
                    pCl->chipsetIDBusAddr.handle = handle;

                    // Store a copy of deviceID,  vendorID, subdeviceID and subvendorID;
                    pCl->chipsetIDInfo.deviceID    = deviceID;
                    pCl->chipsetIDInfo.vendorID    = vendorID;
                    pCl->chipsetIDInfo.subdeviceID = subdeviceID;
                    pCl->chipsetIDInfo.subvendorID = subvendorID;
                }
                return NV_OK;
            }
        }
    }

    // This is bad, we didn't find the First Host Bridge device (assume domain0/bus0/device0/func0)
    pCl->FHBAddr.domain = 0x0;
    pCl->FHBAddr.bus    = 0x0;
    pCl->FHBAddr.device = 0x0;
    pCl->FHBAddr.func   = 0x0;
    pCl->FHBAddr.valid  = 0x1;
    pCl->FHBAddr.handle = NULL;
    pCl->FHBBusInfo.deviceID    = PCI_INVALID_DEVICEID;
    pCl->FHBBusInfo.vendorID    = PCI_INVALID_VENDORID;
    pCl->FHBBusInfo.subdeviceID = PCI_INVALID_SUBDEVICEID;
    pCl->FHBBusInfo.subvendorID = PCI_INVALID_SUBVENDORID;

    DBG_BREAKPOINT();

    NV_ASSERT(0); //We can't find a host bridge, bad!


    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief Check if ASPM L1 is supported from upstream component
 *
 * @param[in] pGpu GPU object pointer
 * @param[in] pCl  CL  object pointer
 *
 * @return NV_TRUE if L1 is supported
 */
NvBool
clIsL1SupportedForUpstreamPort_IMPL
(
    OBJGPU *pGpu,
    OBJCL  *pCl
)
{
    NvU32     linkCap;
    NvBool    bSupported = NV_FALSE;
    NV_STATUS status     = NV_OK;

    if (pGpu->gpuClData.upstreamPort.addr.valid)
    {
        status = clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.upstreamPort, CL_PCIE_LINK_CAP, &linkCap);
        if (status == NV_OK)
        {
            if ((CL_IS_L1_SUPPORTED(linkCap)))
            {
                bSupported = NV_TRUE;
            }
        }
    }
    else if (pGpu->gpuClData.rootPort.addr.valid)
    {
        status = clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.rootPort, CL_PCIE_LINK_CAP, &linkCap);
        if (status == NV_OK)
        {
            if ((CL_IS_L1_SUPPORTED(linkCap)))
            {
                bSupported = NV_TRUE;
            }
        }
    }

    return bSupported;
}

/*!
 * @brief Check if L0s mask is enabled for upstream component
 *
 * @param[in] pGpu GPU object pointer
 * @param[in] pCl  CL  object pointer
 *
 * @return NV_TRUE if mask is enabled (implies L0s is disabled)
 */
NvBool
clIsL0sMaskEnabledForUpstreamPort_IMPL
(
    OBJGPU *pGpu,
    OBJCL  *pCl
)
{
    NvU32  linkCtrl;
    NvBool bEnable = NV_FALSE;

    if (!pGpu->gpuClData.upstreamPort.addr.valid)
    {
        if (!pGpu->gpuClData.rootPort.addr.valid)
        {
            bEnable = NV_TRUE;
        }
        else
        {
            if (clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.rootPort,
                CL_PCIE_LINK_CTRL_STATUS, &linkCtrl) != NV_OK)
            {
                bEnable = NV_TRUE;
            }
            else
            {
                if (!(linkCtrl & CL_PCIE_LINK_CTRL_STATUS_ASPM_L0S_BIT))
                {
                    bEnable = NV_TRUE;
                }
            }
        }
    }
    else
    {
        if (clPcieReadPortConfigReg(pGpu, pCl, &pGpu->gpuClData.upstreamPort,
            CL_PCIE_LINK_CTRL_STATUS, &linkCtrl) != NV_OK)
        {
            bEnable = NV_TRUE;
        }
        else
        {
            if (!(linkCtrl & CL_PCIE_LINK_CTRL_STATUS_ASPM_L0S_BIT))
            {
                bEnable = NV_TRUE;
            }
        }
    }

    return bEnable;
}

NV_STATUS
clInit_IMPL(
    OBJGPU *pGpu,
    OBJCL  *pCl
)
{
    //
    // Common code for all buses
    //
    (void)clInitMappingPciBusDevice(pGpu, pCl);

    if (kbifGetBusIntfType_HAL(GPU_GET_KERNEL_BIF(pGpu)) ==
        NV2080_CTRL_BUS_INFO_TYPE_PCI_EXPRESS)
    {
        return clInitPcie(pGpu, pCl);
    }

    return NV_OK; // no Init is needed for PCI
}

void
clUpdateConfig_IMPL
(
    OBJGPU *pGpu,
    OBJCL  *pCl
)
{
    // Common code for all buses
    clInitMappingPciBusDevice(pGpu, pCl);

    if (kbifGetBusIntfType_HAL(GPU_GET_KERNEL_BIF(pGpu)) ==
        NV2080_CTRL_BUS_INFO_TYPE_PCI_EXPRESS)
    {
        clUpdatePcieConfig(pGpu, pCl);
        return;
    }

    return; // no Update is needed for PCI
}

NV_STATUS
clTeardown_IMPL(
    OBJGPU *pGpu,
    OBJCL  *pCl
)
{
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);

    if (pKernelBif == NULL)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    clFreeBusTopologyCache(pCl);

    switch (kbifGetBusIntfType_HAL(pKernelBif))
    {
        case NV2080_CTRL_BUS_INFO_TYPE_PCI_EXPRESS:
            return clTeardownPcie(pGpu, pCl);

        case NV2080_CTRL_BUS_INFO_TYPE_PCI:
        case NV2080_CTRL_BUS_INFO_TYPE_FPCI:
            return NV_OK;

        default:
            return NV_ERR_GENERIC;
    }
}

NV_STATUS
subdeviceCtrlCmdBusGetBFD_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_BUS_GET_BFD_PARAMSARR *pBusGetBFDParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJCL  *pCl = SYS_GET_CL(pSys);
    BUSTOPOLOGYINFO *pBusTopologyInfo = pCl->pBusTopologyInfo;
    NvU32 i = 0;

    while(pBusTopologyInfo && i < 32)
    {
        pBusGetBFDParams->params[i].valid    = NV_TRUE;
        pBusGetBFDParams->params[i].deviceID = pBusTopologyInfo->busInfo.deviceID;
        pBusGetBFDParams->params[i].vendorID = pBusTopologyInfo->busInfo.vendorID;
        pBusGetBFDParams->params[i].domain   = pBusTopologyInfo->domain;
        pBusGetBFDParams->params[i].bus      = (NvU16)pBusTopologyInfo->bus;
        pBusGetBFDParams->params[i].device   = (NvU16)pBusTopologyInfo->device;
        pBusGetBFDParams->params[i].function = (NvU8)pBusTopologyInfo->func;
        i++;
        pBusTopologyInfo = pBusTopologyInfo->next;
    }
    if(i < 32)
    {
        pBusGetBFDParams->params[i].valid = NV_FALSE;
    }

    pBusTopologyInfo = pCl->pBusTopologyInfo;
    return NV_OK;
}

void clSyncWithGsp_IMPL(OBJCL *pCl, GspSystemInfo *pGSI)
{
    NvU32 idx = 0;
#define CL_SYNC_PDB(prop)                                                       \
    do {                                                                        \
        pGSI->clPdbProperties |= pCl->getProperty(pCl, prop) ? NVBIT64(idx) : 0;\
        idx++;                                                                  \
    } while (0)

    CL_SYNC_PDB(PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE);
    CL_SYNC_PDB(PDB_PROP_CL_FORCE_SNOOP_READS_AND_WRITES_WAR_BUG_410390);
    CL_SYNC_PDB(PDB_PROP_CL_DISABLE_BR03_FLOW_CONTROL);
    CL_SYNC_PDB(PDB_PROP_CL_ASLM_SUPPORTS_NV_LINK_UPGRADE);
    CL_SYNC_PDB(PDB_PROP_CL_ASLM_SUPPORTS_FAST_LINK_UPGRADE);
    CL_SYNC_PDB(PDB_PROP_CL_ASLM_SUPPORTS_HOT_RESET);
    CL_SYNC_PDB(PDB_PROP_CL_ASLM_SUPPORTS_GEN2_LINK_UPGRADE);
    CL_SYNC_PDB(PDB_PROP_CL_IS_CHIPSET_IN_ASPM_POR_LIST);
    CL_SYNC_PDB(PDB_PROP_CL_ASPM_L0S_CHIPSET_DISABLED);
    CL_SYNC_PDB(PDB_PROP_CL_ASPM_L1_CHIPSET_DISABLED);
    CL_SYNC_PDB(PDB_PROP_CL_ASPM_L0S_CHIPSET_ENABLED_MOBILE_ONLY);
    CL_SYNC_PDB(PDB_PROP_CL_ASPM_L1_CHIPSET_ENABLED_MOBILE_ONLY);
    CL_SYNC_PDB(PDB_PROP_CL_ASPM_L1_UPSTREAM_PORT_SUPPORTED);
    CL_SYNC_PDB(PDB_PROP_CL_PCIE_GEN1_GEN2_SWITCH_CHIPSET_DISABLED);
    CL_SYNC_PDB(PDB_PROP_CL_PCIE_GEN1_GEN2_SWITCH_CHIPSET_DISABLED_GEFORCE);
    CL_SYNC_PDB(PDB_PROP_CL_EXTENDED_TAG_FIELD_NOT_CAPABLE);
    CL_SYNC_PDB(PDB_PROP_CL_NOSNOOP_NOT_CAPABLE);
    CL_SYNC_PDB(PDB_PROP_CL_RELAXED_ORDERING_NOT_CAPABLE);
    CL_SYNC_PDB(PDB_PROP_CL_PCIE_FORCE_GEN2_ENABLE);
    CL_SYNC_PDB(PDB_PROP_CL_PCIE_GEN2_AT_LESS_THAN_X16_DISABLED);
    CL_SYNC_PDB(PDB_PROP_CL_ROOTPORT_NEEDS_NOSNOOP_WAR);
    CL_SYNC_PDB(PDB_PROP_CL_INTEL_CPU_ROOTPORT1_NEEDS_H57_WAR);
    CL_SYNC_PDB(PDB_PROP_CL_PCIE_CONFIG_SKIP_MCFG_READ);
    CL_SYNC_PDB(PDB_PROP_CL_ON_PCIE_GEN3_PATSBURG);
    CL_SYNC_PDB(PDB_PROP_CL_ALLOW_PCIE_GEN3_ON_PATSBURG_WITH_IVBE_CPU);
    CL_SYNC_PDB(PDB_PROP_CL_BUG_999673_P2P_ARBITRARY_SPLIT_WAR);
    CL_SYNC_PDB(PDB_PROP_CL_UPSTREAM_LTR_SUPPORTED);
    CL_SYNC_PDB(PDB_PROP_CL_BUG_1340801_DISABLE_GEN3_ON_GIGABYTE_SNIPER_3);
    CL_SYNC_PDB(PDB_PROP_CL_BUG_1681803_WAR_DISABLE_MSCG);
    CL_SYNC_PDB(PDB_PROP_CL_PCIE_NON_COHERENT_USE_TC0_ONLY);
    CL_SYNC_PDB(PDB_PROP_CL_UNSUPPORTED_CHIPSET);
    CL_SYNC_PDB(PDB_PROP_CL_IS_CHIPSET_IO_COHERENT);
    CL_SYNC_PDB(PDB_PROP_CL_DISABLE_IOMAP_WC);
    CL_SYNC_PDB(PDB_PROP_CL_HAS_RESIZABLE_BAR_ISSUE);
    CL_SYNC_PDB(PDB_PROP_CL_BUG_3751839_GEN_SPEED_WAR);
    CL_SYNC_PDB(PDB_PROP_CL_BUG_3562968_WAR_ALLOW_PCIE_ATOMICS);
    CL_SYNC_PDB(PDB_PROP_CL_WAR_AMD_5107271);

#undef CL_SYNC_PDB

    NV_ASSERT(idx < (sizeof(pGSI->clPdbProperties) * 8));

    pGSI->Chipset = pCl->Chipset;
    pGSI->FHBBusInfo = pCl->FHBBusInfo;
    pGSI->chipsetIDInfo = pCl->chipsetIDInfo;

}
