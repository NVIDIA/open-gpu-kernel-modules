/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
*       Chipset and Root Port information                                   *
*                                                                           *
\***************************************************************************/

#include "platform/chipset/chipset_info.h"
#include "platform/chipset/chipset.h"
#include "core/system.h"
#include "os/os.h"
#include "nvpcie.h"
#include "nvdevid.h"

#include "nvcst.h"

//
// X48 & X38 share the same chipset ID, differentiate between two, we need to look at the
// capability Identifier register bit 89
//
#define PCIE_CHIPSET_CAPABILITY_ID_OFFSET_X48   0xE0     // PCIE chipset capability ID offset
#define PCIE_CHIPSET_DETECT_OFFSET              0x8      // PCIE chipset identifier
#define PCIE_CHIPSET_DETECT_BIT                 0x19     // Chipset detect bit

// Used to check if chipset is X38 or X48
#define IS_CHIPSET_X38(n)  (!((n) & NVBIT(PCIE_CHIPSET_DETECT_BIT)))

static NV_STATUS Intel_Core_Nehalem_Processor_setupFunc(OBJCL *pCl);
static NV_STATUS Intel_Huron_River_setupFunc(OBJCL *pCl);
void _Set_ASPM_L0S_L1(OBJCL *pCl, NvBool bDisableL0S, NvBool bDisableL1);

RPINFO rootPortInfo[] =
{
    {PCI_VENDOR_ID_BROADCOM, 0x0140, RP_BROADCOM_HT2100, Broadcom_HT2100_setupFunc},
    {PCI_VENDOR_ID_BROADCOM, 0x0142, RP_BROADCOM_HT2100, Broadcom_HT2100_setupFunc},
    {PCI_VENDOR_ID_BROADCOM, 0x0144, RP_BROADCOM_HT2100, Broadcom_HT2100_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_2581_ROOT_PORT, RP_INTEL_2581, Intel_RP25XX_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_2585_ROOT_PORT, RP_INTEL_2585, Intel_RP25XX_setupFunc},
    {PCI_VENDOR_ID_INTEL, 0x2589                        , RP_INTEL_2589, Intel_RP25XX_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_2591_ROOT_PORT, RP_INTEL_2591, Intel_RP25XX_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_3597_ROOT_PORT, RP_INTEL_3597, NULL},
    {PCI_VENDOR_ID_INTEL, 0x2775,                         RP_INTEL_2775, Intel_RP25XX_setupFunc},
    {PCI_VENDOR_ID_INTEL, 0x2771,                         RP_INTEL_2771, Intel_RP25XX_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_8110_ROOT_PORT, RP_INTEL_8110, Intel_RP81XX_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_8112_ROOT_PORT, RP_INTEL_8112, Intel_RP81XX_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_8180_ROOT_PORT, RP_INTEL_8180, Intel_RP81XX_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_8181_ROOT_PORT, RP_INTEL_8181, Intel_RP81XX_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_8184_ROOT_PORT, RP_INTEL_8184, Intel_RP81XX_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_8185_ROOT_PORT, RP_INTEL_8185, Intel_RP81XX_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_3C02_ROOT_PORT, RP_INTEL_3C02, Intel_RP3C0X_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_3C03_ROOT_PORT, RP_INTEL_3C03, Intel_RP3C0X_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_3C04_ROOT_PORT, RP_INTEL_3C04, Intel_RP3C0X_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_3C05_ROOT_PORT, RP_INTEL_3C05, Intel_RP3C0X_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_3C06_ROOT_PORT, RP_INTEL_3C06, Intel_RP3C0X_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_3C07_ROOT_PORT, RP_INTEL_3C07, Intel_RP3C0X_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_3C08_ROOT_PORT, RP_INTEL_3C08, Intel_RP3C0X_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_3C09_ROOT_PORT, RP_INTEL_3C09, Intel_RP3C0X_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_3C0A_ROOT_PORT, RP_INTEL_3C0A, Intel_RP3C0X_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_3C0B_ROOT_PORT, RP_INTEL_3C0B, Intel_RP3C0X_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_2F04_ROOT_PORT, RP_INTEL_2F04, Intel_RP2F0X_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_2F08_ROOT_PORT, RP_INTEL_2F08, Intel_RP2F0X_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_0C01_ROOT_PORT, RP_INTEL_0C01, Intel_RP0C0X_setupFunc},

// last element must have zero vendor id and device id
    {0,                   0,                              RP_UNKNOWN,    NULL}
};

BRINFO upstreamPortInfo[] =
{
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_1901_ROOT_PORT,  Intel_Skylake_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_9D18_PCH_BRIDGE, Intel_Skylake_U_Pch_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_A117_PCH_BRIDGE, Intel_Skylake_H_Pch_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_A118_PCH_BRIDGE, Intel_Skylake_H_Pch_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_9C98_PCH_BRIDGE, Intel_Broadwell_setupFunc},
    {PCI_VENDOR_ID_INTEL, DEVICE_ID_INTEL_9D10_PCH_BRIDGE, Intel_Kabylake_Y_setupFunc},
    {PCI_VENDOR_ID_AMD,   DEVICE_ID_AMD_1483_ROOT_PORT,    AMD_RP1483_setupFunc},
    {PCI_VENDOR_ID_AMD,   DEVICE_ID_AMD_1480_ROOT_PORT,    AMD_RP1480_setupFunc},
    {PCI_VENDOR_ID_AMD,   DEVICE_ID_AMD_1630_ROOT_PORT,    AMD_RP1630_setupFunc},

    // last element must have zero vendor id and device id
    {0, 0, NULL}
};

static NV_STATUS
Intel_25XX_setupFunc
(
    OBJCL *pCl
)
{
    RmPhysAddr baseAddress;

    if (!pCl->FHBAddr.valid)
        return NV_ERR_GENERIC;
    baseAddress = (RmPhysAddr)(osPciReadDword(pCl->FHBAddr.handle,
                                              INTEL_25XX_CONFIG_SPACE_BASE));

    // PCI-E enhanced config space is 256M aligned
    baseAddress &= ( ~ 0x0fffffff);

    if (baseAddress)
    {
        if (clInsertPcieConfigSpaceBase(pCl, baseAddress, 0, 0, (NvU8)(PCI_MAX_BUSES - 1)) == NV_OK)
            pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE, NV_TRUE);
    }

    pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_SKIP_MCFG_READ, NV_TRUE);

    return NV_OK;
}

// Montevina
static NV_STATUS
Intel_2A40_setupFunc
(
    OBJCL *pCl
)
{
    RmPhysAddr baseAddress;

    if (!pCl->FHBAddr.valid)
        return NV_ERR_GENERIC;
    baseAddress = (RmPhysAddr)(osPciReadDword(pCl->FHBAddr.handle,
                                              INTEL_2A40_CONFIG_SPACE_BASE));

    //
    // PCI-E v1.1 enhanced config space is aligned between 1M and 256M,
    // depending on # of buses (see PCIE v1.1, section 7.2.2).
    //
    baseAddress &= ( ~ 0x000fffff);

    if (baseAddress)
    {
        if (clInsertPcieConfigSpaceBase(pCl, baseAddress, 0, 0, (NvU8)(PCI_MAX_BUSES - 1)) == NV_OK)
        {
            pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE, NV_TRUE);
            //
            // Get the specific Montevina revision to see if its safe to enable
            // ALSM.  Note that the initial Montevina boards do not support ASLM.
            //
            if (pCl->FHBBusInfo.revisionID >= INTEL_2A40_ASLM_CAPABLE_REVID)
            {
                // Supports ASLM
                pCl->setProperty(pCl, PDB_PROP_CL_ASLM_SUPPORTS_NV_LINK_UPGRADE, NV_TRUE);
            }
        }
    }

    pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_SKIP_MCFG_READ, NV_TRUE);

    return NV_OK;
}

// Calpella - Arrandale
static NV_STATUS
Intel_0040_setupFunc
(
    OBJCL *pCl
)
{
    pCl->setProperty(pCl, PDB_PROP_CL_ASLM_SUPPORTS_GEN2_LINK_UPGRADE, NV_TRUE);

    return Intel_Core_Nehalem_Processor_setupFunc(pCl);
}

// Eaglelake
static NV_STATUS
Intel_2E00_setupFunc
(
    OBJCL *pCl
)
{
    RmPhysAddr baseAddress;

    if (!pCl->FHBAddr.valid)
    {
        return NV_ERR_GENERIC;
    }

    baseAddress = (RmPhysAddr)(osPciReadDword(pCl->FHBAddr.handle,
                                              INTEL_2E00_CONFIG_SPACE_BASE));

    //
    // PCI-E v1.1 enhanced config space is aligned between 1M and 256M,
    // depending on # of buses (see PCIE v1.1, section 7.2.2)
    //
    baseAddress &= ( ~ 0x000fffff);

    if (baseAddress)
    {
        if (clInsertPcieConfigSpaceBase(pCl, baseAddress, 0, 0, (NvU8)(PCI_MAX_BUSES - 1)) == NV_OK)
            pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE, NV_TRUE);
    }

    // Set ASPM L0S\L1 properties
    _Set_ASPM_L0S_L1(pCl, NV_TRUE, NV_FALSE);

    pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_SKIP_MCFG_READ, NV_TRUE);

    return NV_OK;
}


// Q35/BearlakeB/IntelQ33
static NV_STATUS
Intel_29X0_setupFunc
(
    OBJCL *pCl
)
{
    NV_STATUS  rmStatus;

    rmStatus = Intel_29XX_setupFunc(pCl);

    // Set ASPM L0S\L1 properties
    _Set_ASPM_L0S_L1(pCl, NV_TRUE, NV_TRUE);

    return rmStatus;
}

static NV_STATUS
Intel_29E0_setupFunc
(
    OBJCL *pCl
)
{
    NV_STATUS  rmStatus;
    NvU32      capId;

    rmStatus = Intel_29XX_setupFunc(pCl);
    //
    // Both X48 & X38 chipset share the same device ID and bit 89
    // of pci capability register is used to differentiate between two
    // (Bug 549707)
    //
    capId = clPcieReadDword(
             pCl,
             pCl->FHBAddr.domain,
             pCl->FHBAddr.bus,
             pCl->FHBAddr.device,
             pCl->FHBAddr.func,
             (PCIE_CHIPSET_CAPABILITY_ID_OFFSET_X48 + PCIE_CHIPSET_DETECT_OFFSET));

    if (IS_CHIPSET_X38(capId))
    {
        // Not capable of Gen1/Gen2 switch
        pCl->setProperty(pCl,
            PDB_PROP_CL_PCIE_GEN1_GEN2_SWITCH_CHIPSET_DISABLED_GEFORCE,
            NV_TRUE);
    }

    // Set ASPM L0S\L1 properties
    _Set_ASPM_L0S_L1(pCl, NV_TRUE, NV_FALSE);

    pCl->setProperty(pCl, PDB_PROP_CL_PCIE_GEN2_AT_LESS_THAN_X16_DISABLED, NV_TRUE);

    return rmStatus;
}

static NV_STATUS
Intel_29XX_setupFunc
(
    OBJCL *pCl
)
{
    RmPhysAddr baseAddress;

    if (!pCl->FHBAddr.valid)
        return NV_ERR_GENERIC;

    baseAddress = (RmPhysAddr)(osPciReadDword(pCl->FHBAddr.handle,
                                              INTEL_29XX_CONFIG_SPACE_BASE));

    //
    // PCI-E v1.1 enhanced config space is aligned between 1M and 256M,
    // depending on # of buses (see PCIE v1.1, section 7.2.2).
    //
    baseAddress &= ( ~ 0x000fffff);

    if (baseAddress)
    {
        if (clInsertPcieConfigSpaceBase(pCl, baseAddress, 0, 0, (NvU8)(PCI_MAX_BUSES - 1)) == NV_OK)
            pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE, NV_TRUE);
    }

    pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_SKIP_MCFG_READ, NV_TRUE);

    return NV_OK;
}

static NV_STATUS
Intel_25E0_setupFunc
(
    OBJCL *pCl
)
{
    RmPhysAddr baseAddress;

    if (!pCl->FHBAddr.valid)
        return NV_ERR_GENERIC;
    baseAddress = (RmPhysAddr)INTEL_25E0_CONFIG_SPACE_BASE_ADDRESS;

    if (clInsertPcieConfigSpaceBase(pCl, baseAddress, 0, 0, (NvU8)(PCI_MAX_BUSES - 1)) == NV_OK)
        pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE, NV_TRUE);

    pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_SKIP_MCFG_READ, NV_TRUE);

    return NV_OK;
}

static NV_STATUS
Intel_27XX_setupFunc
(
    OBJCL *pCl
)
{
    RmPhysAddr baseAddress;

    if (!pCl->FHBAddr.valid)
        return NV_ERR_GENERIC;
    baseAddress = (RmPhysAddr)(osPciReadDword(pCl->FHBAddr.handle,
                                              INTEL_25XX_CONFIG_SPACE_BASE));

    // PCI-E enhanced config space is 256M aligned
    baseAddress &= ( ~ 0x0fffffff);

    if (baseAddress)
    {
        if (clInsertPcieConfigSpaceBase(pCl, baseAddress, 0, 0, (NvU8)(PCI_MAX_BUSES - 1)) == NV_OK)
            pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE, NV_TRUE);
    }

    pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_SKIP_MCFG_READ, NV_TRUE);

    return NV_OK;
}

static NV_STATUS
Intel_359E_setupFunc
(
    OBJCL *pCl
)
{
    RmPhysAddr baseAddress;

    if (!pCl->FHBAddr.valid)
        return NV_ERR_GENERIC;
    baseAddress = (RmPhysAddr)(osPciReadDword(pCl->FHBAddr.handle,
                                              INTEL_359E_CONFIG_SPACE_BASE));

    // PCI-E enhanced config space is 256M aligned
    baseAddress &= ( ~ 0x0fffffff);

    if (baseAddress)
    {
        if (clInsertPcieConfigSpaceBase(pCl, baseAddress, 0, 0, (NvU8)(PCI_MAX_BUSES - 1)) == NV_OK)
            pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE, NV_TRUE);
    }

    pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_SKIP_MCFG_READ, NV_TRUE);

    return NV_OK;
}

static NV_STATUS
Intel_4000_setupFunc
(
    OBJCL *pCl
)
{
    RmPhysAddr baseAddress;

    if (!pCl->FHBAddr.valid)
        return NV_ERR_GENERIC;

    baseAddress = (RmPhysAddr)INTEL_4000_CONFIG_SPACE_BASE_ADDRESS;

    if (clInsertPcieConfigSpaceBase(pCl, baseAddress, 0, 0, (NvU8)(PCI_MAX_BUSES - 1)) == NV_OK)
        pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE, NV_TRUE);

    pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_SKIP_MCFG_READ, NV_TRUE);

    return NV_OK;
}

static NV_STATUS
Intel_4003_setupFunc
(
    OBJCL *pCl
)
{
    void *pHandle;
    NvU32 hecbase;
    RmPhysAddr baseAddress;

    pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE, NV_FALSE);

    // The Intel "SkullTrail" (aka 5400) motherboard chipset does not
    // have a fixed PCIe enhanced config space base address. We can find
    // it by reading bus 0, device 16, function 0, register 0x64 (HECBASE),
    // per Intel 5400 Chipset Memory Controller Hub Datasheet.

    pHandle = osPciInitHandle(0, 0, 16, 0, NULL, NULL);
    if (pHandle == NULL)
        return NV_ERR_GENERIC;

    // Note: This read is on device 16, not on FHB (first host bridge).

    hecbase = osPciReadDword(pHandle, INTEL_4003_CONFIG_SPACE_BASE);
    if (hecbase != 0xFFFFFFFF)
    {
        // The part we are interested in are the 12 bits [23:12],
        // which make up bits [39:28] of the base address. So we
        // isolate the 12 bits we need and shift into place. The
        // high byte of the 40-bit address is shifted away.

        baseAddress = (RmPhysAddr) (hecbase & 0x00FFF000) << 16;

        if (clInsertPcieConfigSpaceBase(pCl, baseAddress, 0, 0, (NvU8)(PCI_MAX_BUSES - 1)) == NV_OK)
            pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE, NV_TRUE);
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Can't read HECBASE register on Intel SkullTrail!\n");
    }

    if (clPcieReadDword(pCl, pCl->FHBAddr.domain, pCl->FHBAddr.bus, pCl->FHBAddr.device, pCl->FHBAddr.func, 0)
            != (NvU32)(pCl->FHBBusInfo.vendorID | pCl->FHBBusInfo.deviceID << 16))
    {
        pCl->pPcieConfigSpaceBase->baseAddress = INTEL_4003_CONFIG_SPACE_BASE_ADDRESS_E;
        if (clPcieReadDword(pCl, pCl->FHBAddr.domain, pCl->FHBAddr.bus, pCl->FHBAddr.device, pCl->FHBAddr.func, 0)
                != (NvU32)(pCl->FHBBusInfo.vendorID | pCl->FHBBusInfo.deviceID << 16))
        {
            return NV_ERR_GENERIC;
        }
    }

    pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_SKIP_MCFG_READ, NV_TRUE);

    return NV_OK;
}

//
// the PCI extended config space BAR is a property of the CPU, not the
// actual chipset because Intel integrated that part of the
// Northbridge in the CPU.
// This function currently applies to processors:
// - i7: INTEL_QUICKPATH_SYSTEM_ADDRESS_DECODER_I7,
// - i5 Auburndale: INTEL_QUICKPATH_SYSTEM_ADDRESS_DECODER_I5_A,
// - i5 Lynnfield :INTEL_QUICKPATH_SYSTEM_ADDRESS_DECODER_I5_L.
//

static NV_STATUS
Intel_Core_Nehalem_Processor_setupFunc
(
    OBJCL *pCl
)
{
    NvS32 bus;
    RmPhysAddr baseAddress;

    pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE, NV_FALSE);

    //
    // Intel data sheet 320835 describes how to find the PCI extended
    // config space BAR in a Core i7 CPU.
    //
    // Current link for i7:
    // https://download.intel.com/design/processor/datashts/320835.pdf
    // Link for i5 not found. It has been reverse engineered.

    // We need to find the pci functions, "Intel QuickPath
    // Architecture System Address Decoder" device 0, function 1 on
    // the CPU's PCI bus, Device ID =
    // INTEL_QUICKPATH_SYSTEM_ADDRESS_DECODER* [section
    // 2.3]. The bus number is the highest bus number in the PCI
    // fabric.

    for (bus = 0xff; bus >= 0; --bus)
    {
        NvU16 vendorId, deviceId;
        void *pHandle = osPciInitHandle(0, bus, 0, 1, &vendorId, &deviceId);
        if (pHandle
            && (vendorId == PCI_VENDOR_ID_INTEL)
            && ((deviceId == INTEL_QUICKPATH_SYSTEM_ADDRESS_DECODER_I7) ||
                (deviceId == INTEL_QUICKPATH_SYSTEM_ADDRESS_DECODER_I5_A) ||
                (deviceId == INTEL_QUICKPATH_SYSTEM_ADDRESS_DECODER_I5_L) ||
                (deviceId == INTEL_QUICKPATH_SYSTEM_ADDRESS_DECODER_I5_6)))
        {
            // [section 2.6.5] describes how to decode the config
            // space bar
            const NvU32 sad_pciexbar_low  = osPciReadDword(pHandle, 0x50);
            const NvU64 sad_pciexbar_high = osPciReadDword(pHandle, 0x54);

            const NvU64 sad_pciexbar = (sad_pciexbar_high << 32) | sad_pciexbar_low;
            const NvU64 address      = sad_pciexbar & 0xfffff00000ULL;
            const NvU32 size         = (sad_pciexbar_low >> 1) & 0x7;
            const NvU32 enabled      = (sad_pciexbar_low & 1);

            // if it's disabled, then skip.
            if (enabled == 0)
                continue;

            // if it's not a size we know, then skip. 0 = 256MB, 6 = 64MB, 7 =
            // 128MB BAR size
            if ((size != 0) && (size != 7) && (size != 6))
                continue;

            baseAddress = (RmPhysAddr)address;

            if (clInsertPcieConfigSpaceBase(pCl, baseAddress, 0, 0, (NvU8)(PCI_MAX_BUSES - 1)) == NV_OK)
                pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE, NV_TRUE);
            break;
        }
    }

    return NV_OK;
}

static NV_STATUS
Intel_3400_setupFunc
(
    OBJCL *pCl
)
{
    // Set ASPM L0S\L1 properties
    _Set_ASPM_L0S_L1(pCl, NV_TRUE, NV_FALSE);

    return Intel_Core_Nehalem_Processor_setupFunc(pCl);
}

//
// 3B42 is the device Id of the IBEX south bridge. It is not the device id of the host bridge.
// On such core i5 platforms the host bridge is in the CPU, so we cannot rely on it
// to detect the P55 platform.
//
static NV_STATUS
Intel_3B42_setupFunc
(
    OBJCL *pCl
)
{
    void *pHandle;
    NvU16 deviceId;
    NvU16 cpuDeviceId;
    NvU16 cpuVId;

    // We need to find out if it is Intel H57 Chipset (based on LPC D31 F0
    // PCI DevID 3B08h )and remove it from ASPM POR. Refer Bug 706926 for more details
    pHandle = osPciInitHandle(0, 0, 31 , 0, NULL, &deviceId);

    if ((pHandle != NULL) && (deviceId == 0x3B08))
    {
       // Set PDB_PROP_CL_INTEL_CPU_ROOTPORT1_NEEDS_H57_WAR to disable L1 in CPU Root Port for H57.
       pCl->setProperty(pCl, PDB_PROP_CL_INTEL_CPU_ROOTPORT1_NEEDS_H57_WAR, NV_TRUE);
    }

     //
    // Bug 782125 : [PEX ASPM] Enable GPU L0s and L1 on H55 LPC (@ D31 F0) (deviceId == 0x3B06) and
    // H57 (deviceId == 0x3B08) with Clarkdale CPUs dev 0 func 0 (deviceId == 0x0042) for All
    // Fermi and Later GPUs
    //
    if ((pHandle != NULL) && ((deviceId == 0x3B06) || (deviceId == 0x3B08)))
    {
        // We need to find out if CPU is Clarkdale by reading 'Register 0 of Dev 0 Func 0
        // VID: 0x8086 and DEVID: 0x0040
        pHandle = osPciInitHandle(0, 0, 0 , 0, &cpuVId, &cpuDeviceId);

        if ((pHandle != NULL) && (cpuVId == PCI_VENDOR_ID_INTEL) && (cpuDeviceId == 0x0040))
        {
            // Enable L1P and L0
            _Set_ASPM_L0S_L1(pCl, NV_FALSE, NV_FALSE);
        }
    }

    return Intel_Core_Nehalem_Processor_setupFunc(pCl);
}

// Intel Huron River Chipset Common Function
static NV_STATUS
Intel_Huron_River_setupFunc
(
    OBJCL *pCl
)
{
    // Enable Gen2 ASLM
    pCl->setProperty(pCl, PDB_PROP_CL_ASLM_SUPPORTS_GEN2_LINK_UPGRADE, NV_TRUE);

    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IN_ASPM_POR_LIST, NV_TRUE);

    // Set ASPM L0S\L1 properties
    _Set_ASPM_L0S_L1(pCl, NV_TRUE, NV_TRUE);

    // Enable L0s and L1 on mobile only
    pCl->setProperty(pCl, PDB_PROP_CL_ASPM_L0S_CHIPSET_ENABLED_MOBILE_ONLY, NV_TRUE);
    pCl->setProperty(pCl, PDB_PROP_CL_ASPM_L1_CHIPSET_ENABLED_MOBILE_ONLY, NV_TRUE);

    return NV_OK;
}

// Intel Huron River Chipset HM67/QM67 Function - supports SLI
static NV_STATUS
Intel_1C4B_setupFunc
(
    OBJCL *pCl
)
{
    return Intel_Huron_River_setupFunc(pCl);
}

// Intel Huron River Chipset HM65 Function - does not support SLI
static NV_STATUS
Intel_1C49_setupFunc
(
    OBJCL *pCl
)
{
    return Intel_Huron_River_setupFunc(pCl);
}

// Intel P67 Chipset Setup Function
static NV_STATUS
Intel_1C10_setupFunc
(
    OBJCL *pCl
)
{
    return NV_OK;
}

// Intel P67 Chipset Cougar-PointSetup Function - supports SLI
static NV_STATUS
Intel_1C46_setupFunc
(
    OBJCL *pCl
)
{
    return NV_OK;
}

// Intel X79 Patsburg Chipset - supports SLI
static NV_STATUS
Intel_1D40_setupFunc
(
    OBJCL *pCl
)
{
    pCl->setProperty(pCl, PDB_PROP_CL_ON_PCIE_GEN3_PATSBURG, NV_TRUE);

    if (DEVICE_ID_INTEL_0E00_HOST_BRIDGE == pCl->FHBBusInfo.deviceID)
    {
        pCl->setProperty(pCl, PDB_PROP_CL_ALLOW_PCIE_GEN3_ON_PATSBURG_WITH_IVBE_CPU, NV_TRUE);
    }

    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IN_ASPM_POR_LIST, NV_TRUE);

    // Set ASPM L0S\L1 properties
    _Set_ASPM_L0S_L1(pCl, NV_TRUE, NV_TRUE);

    // Enable L0s on mobile parts only
    pCl->setProperty(pCl, PDB_PROP_CL_ASPM_L0S_CHIPSET_ENABLED_MOBILE_ONLY, NV_TRUE);

    return NV_OK;
}

// Intel X99 platform
static NV_STATUS
Intel_8D47_setupFunc
(
    OBJCL *pCl
)
{
    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IN_ASPM_POR_LIST, NV_TRUE);

    // Set ASPM L0S\L1 properties
    _Set_ASPM_L0S_L1(pCl, NV_TRUE, NV_TRUE);

    // Enable L0s on mobile parts only
    pCl->setProperty(pCl, PDB_PROP_CL_ASPM_L0S_CHIPSET_ENABLED_MOBILE_ONLY, NV_TRUE);

    // Disable MSCG on X99 chipset
    pCl->setProperty(pCl, PDB_PROP_CL_BUG_1681803_WAR_DISABLE_MSCG, NV_TRUE);

    return NV_OK;
}

// Intel C612 platform (X99 based)
static NV_STATUS
Intel_8D44_setupFunc
(
    OBJCL *pCl
)
{
    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IN_ASPM_POR_LIST, NV_TRUE);

    // Set ASPM L0S\L1 properties
    _Set_ASPM_L0S_L1(pCl, NV_TRUE, NV_TRUE);

    // Enable L0s on mobile parts only
    pCl->setProperty(pCl, PDB_PROP_CL_ASPM_L0S_CHIPSET_ENABLED_MOBILE_ONLY, NV_TRUE);

    return NV_OK;
}

// Intel Z75 Ivy Bridge CPU - supports SLI
static NV_STATUS
Intel_1E10_setupFunc
(
    OBJCL *pCl
)
{
    // Set ASPM L0S\L1 properties
    _Set_ASPM_L0S_L1(pCl, NV_FALSE, NV_FALSE);

    // Set PDB to disable Gen3 on GIGABYTE Sniper 3 motherboard. Bug 1340801.
    if (pCl->chipsetIDInfo.subvendorID == NV_PCI_SUBID_VENDOR_GIGABYTE)
    {
        switch(pCl->chipsetIDInfo.subdeviceID)
        {
            case GIGABYTE_SNIPER_3_SSDEVID_1:
            case GIGABYTE_SNIPER_3_SSDEVID_2:
                pCl->setProperty(pCl, PDB_PROP_CL_BUG_1340801_DISABLE_GEN3_ON_GIGABYTE_SNIPER_3, NV_TRUE);
                break;
            default:
                break;
        }
    }

    return NV_OK;
}

// Intel SharkBay (Haswell) - Lynx Point platform
static NV_STATUS
Intel_8C4B_setupFunc
(
    OBJCL *pCl
)
{

    // Set ASPM L0S\L1 properties
    _Set_ASPM_L0S_L1(pCl, NV_TRUE, NV_FALSE);

    // Enable L0s on mobile only
    pCl->setProperty(pCl, PDB_PROP_CL_ASPM_L0S_CHIPSET_ENABLED_MOBILE_ONLY, NV_TRUE);

    return NV_OK;
}

// Intel Z97 platform
static NV_STATUS
Intel_8CC4_setupFunc
(
    OBJCL *pCl
)
{
    return NV_OK;
}

// Intel Z170 platform
static NV_STATUS
Intel_A145_setupFunc
(
    OBJCL *pCl
)
{
    return NV_OK;
}

// Intel Z270 platform
static NV_STATUS
Intel_A2C5_setupFunc
(
    OBJCL *pCl
)
{
    return NV_OK;
}

// Intel C62x/C422 platform
static NV_STATUS
Intel_A242_setupFunc
(
    OBJCL *pCl
)
{
    return NV_OK;
}

// IntelX299 platform
static NV_STATUS
Intel_A2D2_setupFunc
(
    OBJCL *pCl
)
{
    return NV_OK;
}

// Intel Z370 platform
static NV_STATUS
Intel_A2C9_setupFunc
(
    OBJCL *pCl
)
{
    return NV_OK;
}

// Intel CannonLake platform
static NV_STATUS
Intel_A301_setupFunc
(
    OBJCL *pCl
)
{
    return NV_OK;
}

// Intel Comet Lake platform
static NV_STATUS
Intel_0685_setupFunc
(
    OBJCL *pCl
)
{
    return NV_OK;
}

// Intel Z590 platform (Rocket Lake)
static NV_STATUS
Intel_4381_setupFunc
(
    OBJCL *pCl
)
{
    pCl->setProperty(pCl, PDB_PROP_CL_HAS_RESIZABLE_BAR_ISSUE, NV_TRUE);

    //
    // Apply the WAR to restrict the max target gen speed capable to previous gen
    // on ASUS Z590 (Intel RKL-S) platform only
    // Bug 3751839
    //
    if (pCl->chipsetIDInfo.subvendorID == PCI_VENDOR_ID_ASUS)
    {
        pCl->setProperty(pCl, PDB_PROP_CL_BUG_3751839_GEN_SPEED_WAR, NV_TRUE);
    }

    return NV_OK;
}

// Intel Z690 platform (Alder Lake)
static NV_STATUS
Intel_7A82_setupFunc
(
    OBJCL *pCl
)
{
    pCl->setProperty(pCl, PDB_PROP_CL_HAS_RESIZABLE_BAR_ISSUE, NV_TRUE);

    return NV_OK;
}

// Intel Z790 platform (Raptor Lake)
static NV_STATUS
Intel_7A04_setupFunc
(
    OBJCL *pCl
)
{
    pCl->setProperty(pCl, PDB_PROP_CL_HAS_RESIZABLE_BAR_ISSUE, NV_TRUE);

    return NV_OK;
}

static NV_STATUS
Nvidia_T210_setupFunc
(
    OBJCL *pCl
)
{
    if (!pCl->FHBAddr.valid)
        return NV_ERR_GENERIC;

    if (clInsertPcieConfigSpaceBase(pCl, 0, 0, 0, (NvU8)(PCI_MAX_BUSES - 1)) == NV_OK)
            pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE, NV_TRUE);

    //
    // setting this prevents trying to access the PCI MCFG table to get config information;
    // this is part of the ACPI spec, which doesn't apply to Tegra
    //
    pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_SKIP_MCFG_READ, NV_TRUE);

    // Enable Gen2 ASLM
    pCl->setProperty(pCl, PDB_PROP_CL_ASLM_SUPPORTS_GEN2_LINK_UPGRADE, NV_TRUE);

    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IO_COHERENT, NV_FALSE);

    _Set_ASPM_L0S_L1(pCl, NV_FALSE, NV_FALSE);
    return NV_OK;
}

static NV_STATUS
Nvidia_T194_setupFunc
(
    OBJCL *pCl
)
{
    NV_STATUS status;

    status = Nvidia_T210_setupFunc(pCl);
    if (status != NV_OK)
        return status;

    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IO_COHERENT, NV_TRUE);

    return NV_OK;
}

static NV_STATUS
Nvidia_TH500_setupFunc
(
    OBJCL *pCl
)
{
    if (!pCl->FHBAddr.valid)
        return NV_ERR_GENERIC;

    if (clInsertPcieConfigSpaceBase(pCl, 0, 0, 0, (NvU8)(PCI_MAX_BUSES - 1)) == NV_OK)
        pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE, NV_TRUE);

    // Enable Gen2 ASLM
    pCl->setProperty(pCl, PDB_PROP_CL_ASLM_SUPPORTS_GEN2_LINK_UPGRADE, NV_TRUE);

    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IO_COHERENT, NV_TRUE);

    pCl->setProperty(pCl, PDB_PROP_CL_BUG_3562968_WAR_ALLOW_PCIE_ATOMICS, NV_TRUE);

    _Set_ASPM_L0S_L1(pCl, NV_FALSE, NV_FALSE);

    return NV_OK;
}

static NV_STATUS
SiS_656_setupFunc
(
    OBJCL *pCl
)
{
    NvU32 PcieConfigBaseReg;
    RmPhysAddr baseAddress;

    if (!pCl->FHBAddr.valid)
        return NV_ERR_GENERIC;

    PcieConfigBaseReg = osPciReadDword(pCl->FHBAddr.handle,
            SIS_656_CONFIG_SPACE_BASE);

    baseAddress = (RmPhysAddr)(REF_VAL(SIS_656_CONFIG_SPACE_BASE_ADDRESS, PcieConfigBaseReg ));

    if (baseAddress)
    {
        baseAddress <<= 28;
        if (clInsertPcieConfigSpaceBase(pCl, baseAddress, 0, 0, (NvU8)(PCI_MAX_BUSES - 1)) == NV_OK)
            pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE, NV_TRUE);
    }

    pCl->setProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_SKIP_MCFG_READ, NV_TRUE);

    return NV_OK;
}

static NV_STATUS
ATI_RS400_setupFunc
(
    OBJCL *pCl
)
{
    NvU32 nbcfg;

    if (!pCl->FHBAddr.valid)
        return NV_ERR_GENERIC;

    // Distinguish chipset revisions. A21 and earlier have PCI-E issues
    // that require special treatment.
    nbcfg = osPciReadDword(pCl->FHBAddr.handle, 0x9c);
    if (nbcfg & 1)
        pCl->Chipset = CS_ATI_RS400;
    else
        pCl->Chipset = CS_ATI_RS400_A21;

    return NV_OK;
}

static NV_STATUS
ATI_RS480_setupFunc
(
    OBJCL *pCl
)
{
    NvU32 rev;

    if (!pCl->FHBAddr.valid)
        return NV_ERR_GENERIC;

    // Distinguish chipset revisions. A21 and earlier have PCI-E issues
    // that require special treatment.
    rev = osPciReadDword(pCl->FHBAddr.handle, 0x08);
    if (rev & 0xff)
        pCl->Chipset = CS_ATI_RS480;
    else
        pCl->Chipset = CS_ATI_RS480_A21;

    return NV_OK;
}

//
// AMD RS 780 and GX790
//
NV_STATUS
AMD_RS780_setupFunc(OBJCL *pCl)
{
    if (!pCl->FHBAddr.valid)
        return NV_ERR_GENERIC;

    _Set_ASPM_L0S_L1(pCl, NV_FALSE, NV_TRUE);
    return NV_OK;
}

//
// AMD FX 790
//
NV_STATUS
AMD_FX790_setupFunc(OBJCL *pCl)
{
    if (!pCl->FHBAddr.valid)
        return NV_ERR_GENERIC;

    _Set_ASPM_L0S_L1(pCl, NV_TRUE, NV_FALSE);
    return NV_OK;
}

NV_STATUS ATI_RD870_setupFunc(OBJCL *pCl)
{
    return NV_OK;
}

NV_STATUS ATI_RD890_setupFunc(OBJCL *pCl)
{
    pCl->setProperty(pCl, PDB_PROP_CL_RELAXED_ORDERING_NOT_CAPABLE, NV_TRUE);
    return NV_OK;
}

NV_STATUS ATI_RX780_setupFunc(OBJCL *pCl)
{
    pCl->setProperty(pCl, PDB_PROP_CL_RELAXED_ORDERING_NOT_CAPABLE, NV_TRUE);
    return NV_OK;
}

// AMD FX890 and AMD GX890 Chipset Setup Function
static NV_STATUS
AMD_FX890_setupFunc
(
    OBJCL *pCl
)
{
   // Set ASPM L0S\L1 properties
    _Set_ASPM_L0S_L1(pCl, NV_FALSE, NV_FALSE);

   return NV_OK;
}

// AMD FX990 and AMD X990 Chipset Setup Function
static NV_STATUS
ATI_FX990_setupFunc
(
    OBJCL *pCl
)
{
    pCl->setProperty(pCl, PDB_PROP_CL_BUG_999673_P2P_ARBITRARY_SPLIT_WAR, NV_TRUE);

   // Set ASPM L0S\L1 properties
    _Set_ASPM_L0S_L1(pCl, NV_FALSE, NV_FALSE);

    return NV_OK;
}


// AMD X370 Chipset Setup Function
static NV_STATUS
AMD_X370_setupFunc
(
    OBJCL *pCl
)
{

    // Set ASPM L0S\L1 properties
    _Set_ASPM_L0S_L1(pCl, NV_FALSE, NV_FALSE);

    return NV_OK;
}

// VIA VX900 Chipset Setup Function
static NV_STATUS
VIA_VX900_setupFunc
(
    OBJCL *pCl
)
{
    // This chipset is not capable of Gen1/Gen2 switch.
    pCl->setProperty(pCl, PDB_PROP_CL_PCIE_GEN1_GEN2_SWITCH_CHIPSET_DISABLED, NV_TRUE);

   return NV_OK;
}

// AppliedMicro XGene Storm Setup Function
static NV_STATUS
APM_Storm_setupFunc
(
    OBJCL *pCl
)
{
    // This chipset has trouble with multiple traffic classes
    pCl->setProperty(pCl, PDB_PROP_CL_PCIE_NON_COHERENT_USE_TC0_ONLY, NV_TRUE);

    return NV_OK;
}

// Generic ARMV8 setup function
static NV_STATUS
ARMV8_generic_setupFunc
(
    OBJCL *pCl
)
{
    return NV_OK;
}

// Marvell ThunderX2 Setup Function
static NV_STATUS
Marvell_ThunderX2_setupFunc
(
    OBJCL *pCl
)
{
    // TODO Need to check if any more PDB properties should be set
    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IO_COHERENT, NV_TRUE);
    return NV_OK;
}

// QEMU Setup Function
static NV_STATUS
QEMU_setupFunc
(
    OBJCL *pCl
)
{
    //
    // TODO Need to check if any more PDB properties should be set and
    // use ACPI tables to determine whether system is I/O coherent,
    // instead of hard coding.
    //
    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IO_COHERENT, NV_TRUE);
    return NV_OK;
}

// Ampere eMag Setup Function
static NV_STATUS
Ampere_eMag_setupFunc
(
    OBJCL *pCl
)
{
    // TODO Need to check if any more PDB properties should be set
    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IO_COHERENT, NV_TRUE);
    return NV_OK;
}

// Huawei Kunpeng Setup Function
static NV_STATUS
Huawei_Kunpeng920_setupFunc
(
    OBJCL *pCl
)
{
    //
    // TODO Need to check if any more PDB properties should be set and
    // use ACPI tables to determine whether system is I/O coherent,
    // instead of hard coding.
    //
    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IO_COHERENT, NV_TRUE);
    return NV_OK;
}

// Mellanox BlueField Setup Function
static NV_STATUS
Mellanox_BlueField_setupFunc
(
    OBJCL *pCl
)
{
    // TODO Need to check if any more PDB properties should be set
    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IO_COHERENT, NV_TRUE);
    return NV_OK;
}

// Mellanox BlueField3 Setup Function
static NV_STATUS
Mellanox_BlueField3_setupFunc
(
    OBJCL *pCl
)
{
    // Bug 4151565: BlueField 3 does not support WC mapping 
    pCl->setProperty(pCl, PDB_PROP_CL_DISABLE_IOMAP_WC, NV_TRUE);
    return NV_OK;
}


// Amazon Gravitron2 Setup Function
static NV_STATUS
Amazon_Gravitron2_setupFunc
(
    OBJCL *pCl
)
{
    // TODO Need to check if any more PDB properties should be set
    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IO_COHERENT, NV_TRUE);
    return NV_OK;
}

// Fujitsu A64FX Setup Function
static NV_STATUS
Fujitsu_A64FX_setupFunc
(
    OBJCL *pCl
)
{
    // TODO Need to check if any more PDB properties should be set
    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IO_COHERENT, NV_TRUE);
    return NV_OK;
}

// Ampere Quicksilver Setup Function
static NV_STATUS
Ampere_Altra_setupFunc
(
    OBJCL *pCl
)
{
    // TODO Need to check if any more PDB properties should be set
    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IO_COHERENT, NV_TRUE);

    // WAR bug 2915474: Ampere Altra rev0 does not correctly handle UC/WC iomaps.
    if (pCl->pBusTopologyInfo->busInfo.revisionID == 0x0)
    {
        pCl->setProperty(pCl, PDB_PROP_CL_DISABLE_IOMAP_WC, NV_TRUE);
    }

    return NV_OK;
}

static NV_STATUS
Arm_NeoverseN1_setupFunc
(
    OBJCL *pCl
)
{
    // TODO Need to check if any more PDB properties should be set
    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IO_COHERENT, NV_TRUE);
    return NV_OK;
}

static NV_STATUS
Riscv_generic_setupFunc
(
    OBJCL *pCl
)
{
    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IO_COHERENT, NV_TRUE);
    return NV_OK;
}

static NV_STATUS
PLDA_XpressRichAXI_setupFunc
(
    OBJCL *pCl
)
{
#if NVCPU_IS_RISCV64
    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IO_COHERENT, NV_TRUE);
#endif
    return NV_OK;
}

// Ampere AmpereOne Setup Function
static NV_STATUS
Ampere_AmpereOne_setupFunc
(
    OBJCL *pCl
)
{
    // TODO Need to check if any more PDB properties should be set
    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IO_COHERENT, NV_TRUE);
    return NV_OK;
}

void
csGetInfoStrings
(
    OBJCL *pCl,
    NvU8 *pChipsetNameStr,
    NvU8 *pVendorNameStr,
    NvU8 *pSliBondNameStr,
    NvU8 *pSubSysVendorNameStr,
    NvU32 nameStrLen
)
{
    NvU32 i;
    const char* pszUnknown = "Unknown";
    NvU32 szUnknownLen = portStringLength(pszUnknown) + 1;

    if (!pCl->chipsetIDBusAddr.valid)
    {
        portStringCopy((char *) pChipsetNameStr, szUnknownLen,
                       pszUnknown, szUnknownLen);
        portStringCopy((char *) pVendorNameStr, szUnknownLen,
                       pszUnknown, szUnknownLen);
        portStringCopy((char *) pSliBondNameStr, szUnknownLen,
                       pszUnknown, szUnknownLen);
        portStringCopy((char *) pSubSysVendorNameStr, szUnknownLen,
                       pszUnknown, szUnknownLen);
        return ;
    }

    for (i = 0; chipsetInfo[i].chipset; i++)
    {
        if ((pCl->chipsetIDInfo.vendorID == chipsetInfo[i].vendorID) &&
             (pCl->chipsetIDInfo.deviceID == chipsetInfo[i].deviceID))
        {
            portStringCopy((char*)pChipsetNameStr,
                            nameStrLen,
                            chipsetInfo[i].name,
                            nameStrLen);
            break;
        }
    }
    if (!chipsetInfo[i].chipset)
    {
        portStringCopy((char *) pChipsetNameStr, szUnknownLen,
                       pszUnknown, szUnknownLen);
    }

    for (i = 0; vendorName[i].vendorID; i++)
    {
        if (pCl->chipsetIDInfo.vendorID == vendorName[i].vendorID)
        {
            portStringCopy((char*)pVendorNameStr,
                            nameStrLen,
                            vendorName[i].name,
                            nameStrLen);
            break;
        }
    }
    if (!vendorName[i].vendorID)
    {
        portStringCopy((char *) pVendorNameStr, szUnknownLen,
                       pszUnknown, szUnknownLen);
    }

    for (i = 0; vendorName[i].vendorID; i++)
    {
        if (pCl->chipsetIDInfo.subvendorID == vendorName[i].vendorID)
        {
            portStringCopy((char*)pSubSysVendorNameStr,
                            nameStrLen,
                            vendorName[i].name,
                            nameStrLen);
            break;
        }
    }
    if (!vendorName[i].vendorID)
    {
        portStringCopy((char *)pSubSysVendorNameStr, szUnknownLen,
                       pszUnknown, szUnknownLen);
    }

    {
        portStringCopy((char *)pSliBondNameStr, szUnknownLen,
                       pszUnknown, szUnknownLen);
    }
}

//
// This function sets the pdb properties to disable ASPM L0S\L1
//
void
_Set_ASPM_L0S_L1
(
    OBJCL         *pCl,
    NvBool         bDisableL0S,
    NvBool         bDisableL1
)
{
    //
    // this chipset is part of exception list to enable/disable L0S/L1
    // (refer bug 529308)
    //
    pCl->setProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IN_ASPM_POR_LIST, NV_TRUE);

    if (bDisableL0S)
    {
        // Not capable of ASPM-L0s
        pCl->setProperty(pCl, PDB_PROP_CL_ASPM_L0S_CHIPSET_DISABLED, NV_TRUE);
    }
    if (bDisableL1)
    {
        // Not capable of ASPM L1
        pCl->setProperty(pCl, PDB_PROP_CL_ASPM_L1_CHIPSET_DISABLED, NV_TRUE);
    }
}

