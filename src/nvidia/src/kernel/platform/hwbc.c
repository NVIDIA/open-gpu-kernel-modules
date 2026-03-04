/*
 * SPDX-FileCopyrightText: Copyright (c) 2000-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************* Hardware Broadcast Routines ***************************\
*                                                                           *
*   Initialization code for broadcast devices                               *
*                                                                           *
****************************************************************************/

#include "platform/chipset/chipset.h"
#include "nvpcie.h"
#include "os/os.h"
#include "gpu/gpu.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/mem_mgr/virt_mem_allocator_common.h"

#include "ctrl/ctrl2080/ctrl2080bus.h"

#include "published/br03/dev_br03_xvd.h"
#include "published/br03/dev_br03_xvu.h"
#include "published/br04/br04_ref.h"

#include "platform/hwbc.h"

//
// These BR04 registers/bits/values are not properly defined in the headers
//

#define NV_BR04_XVU_ROM_REVISION_ID_UPPER_16BITS_P390               0x0390
#define NV_BR04_XVU_ROM_REVISION_ID_UPPER_16BITS_P535               0x0535
#define NV_BR04_XVU_ROM_REVISION_ID_UPPER_16BITS_P710               0x0710
#define NV_BR04_XVU_ROM_REVISION_ID_UPPER_16BITS_P711               0x0711
#define NV_BR04_XVU_ROM_REVISION_ID_UPPER_16BITS_P712               0x0712
#define NV_BR04_XVU_ROM_REVISION_ID_UPPER_16BITS_P713               0x0713
#define NV_BR04_XVU_ROM_REVISION_ID_UPPER_16BITS_P737               0x0737
#define NV_BR04_XVU_ROM_REVISION_ID_UPPER_16BITS_P790               0x0790
#define NV_BR04_XVU_ROM_REVISION_ID_UPPER_16BITS_P797               0x0797
#define NV_BR04_XVU_ROM_REVISION_ID_UPPER_16BITS_P838               0x0838
#define NV_BR04_XVU_ROM_REVISION_ID_UPPER_16BITS_P881               0x0881
#define NV_BR04_XVU_ROM_REVISION_ID_UPPER_16BITS_P883               0x0883
#define NV_BR04_XVU_ROM_REVISION_ID_UPPER_16BITS_P884               0x0884
#define NV_BR04_XVU_ROM_REVISION_ID_UPPER_16BITS_P885               0x0885
#define NV_BR04_XVU_ROM_REVISION_ID_UPPER_16BITS_P894               0x0894
#define NV_BR04_XVU_ROM_REVISION_ID_UPPER_16BITS_P984               0x0984
#define NV_BR04_XVU_ROM_REVISION_ID_UPPER_16BITS                    31:16

#ifndef NV_BR04_XVD_G2_PRIV_XP_LCTRL_2_REV2P0_COMPLIANCE_DIS_ONE
#define NV_BR04_XVD_G2_PRIV_XP_LCTRL_2_REV2P0_COMPLIANCE_DIS_ONE    1
#endif

#ifndef NV_XPU_PEX_PLL_CTL2
#define NV_XPU_PEX_PLL_CTL2                                         0x00000E2C
#define NV_XPU_PEX_PLL_CTL2_PLL_CP_CNTL                             22:20
#define NV_XPU_PEX_PLL_CTL2_PLL_CP_CNTL_22P5UA                      0x00000004
#define NV_XPU_PEX_PLL_CTL2_PLL_CP_CNTL_30UA                        0x00000007
#endif

#ifndef NV_XPD_PEX_PLL_CTL2
#define NV_XPD_PEX_PLL_CTL2                                         0x00000F18
#define NV_XPD_PEX_PLL_CTL2_PLL_CP_CNTL                             22:20
#define NV_XPD_PEX_PLL_CTL2_PLL_CP_CNTL_22P5UA                      0x00000004
#endif

#ifndef NV_BR04_XVU_CTRIM_DP_1
#define NV_BR04_XVU_CTRIM_DP_1                                      0x00000D04
#define NV_BR04_XVU_CTRIM_DP_2                                      0x00000D08
#define NV_BR04_XVU_CTRIM_DP_3                                      0x00000D0C
#define NV_BR04_XVU_CTRIM_DP_4                                      0x00000D10
#define NV_BR04_XVU_CTRIM_DP_5                                      0x00000D14
#define NV_BR04_XVU_CTRIM_DP_6                                      0x00000D18
#endif

#ifndef NV_XPU_PEX_PAD_CTL_3
#define NV_XPU_PEX_PAD_CTL_3                                        0x00000E0C
#define NV_XPU_PEX_PAD_CTL_3_TX_PEAK_R2_1C                          23:20
#define NV_XPU_PEX_PAD_CTL_3_TX_PEAK_R2_1C_22DB                     0x00000007
#define NV_XPU_PEX_PAD_CTL_3_TX_PEAK_R2_1C_36DB                     0x0000000A
#define NV_XPU_PEX_PAD_CTL_3_TX_PEAK_R2_1C_6DB                      0x0000000F
#endif

#ifndef NV_BR04_XVU_CYA_BIT0_RSVD_28_DP0_DE_EMP_NEG_3P5_DB
#define NV_BR04_XVU_CYA_BIT0_RSVD_28_DP0_DE_EMP_NEG_3P5_DB          0x00000001
#define NV_BR04_XVU_CYA_BIT0_RSVD_29_DP1_DE_EMP_NEG_3P5_DB          0x00000001
#define NV_BR04_XVU_CYA_BIT0_RSVD_30_DP2_DE_EMP_NEG_3P5_DB          0x00000001
#define NV_BR04_XVU_CYA_BIT0_RSVD_31_DP3_DE_EMP_NEG_3P5_DB          0x00000001
#endif

#ifndef NV_BR04_XVU_CYA_NIBBLE0_RSVD_0_UP0_ASPM_DISABLE
#define NV_BR04_XVU_CYA_NIBBLE0_RSVD_0_UP0_ASPM_DISABLE             0x00000008
#define NV_BR04_XVU_CYA_NIBBLE0_RSVD_4_DPX_ASPM_DISABLE             0x00000008
#endif

#ifndef NV_BR04_XVD_G2_PRIV_XP_LCTRL_2_ADVERTISED_RATE_CHANGE_ONE
#define NV_BR04_XVD_G2_PRIV_XP_LCTRL_2_ADVERTISED_RATE_CHANGE_ONE   1
#endif

#ifndef NV_PES_XVU_ROM_ACCESS
#define NV_PES_XVU_ROM_ACCESS(i)                            (0x00001000+(i)*4)
#endif

#define NV_BR04_FIRMWARE_SIGNATURE                                  0x42523034
#define NV_PLX_FIRMWARE_SIGNATURE                                   0x0000005a

//
// PLX PEX8747 definitions
// PLX PEX 8747 data book has info about the header definitions.
//

#define PCI_VENDOR_ID_PLX                                   0x10B5
#define PLX_DEVICE_ID_PEX8747                               0x8747

#define NV_PLX_PEX8747_TRANSACTION_STATUS_BIT                   0x00000004

#define NV_PLX_PEX8747_ROM_REVISION_ADDR                        0x00000008
#define NV_PLX_PEX8747_ROM_REVISION_ADDR_ID                     7:0

#define NV_PLX_EEPROM_CONTROL_AND_STATUS_REG                    0x00000260
#define NV_PLX_EEPROM_CONTROL_AND_STATUS_REG_CTRL               15:0
#define NV_PLX_EEPROM_CONTROL_AND_STATUS_REG_CTRL_ADDR          12:0
#define NV_PLX_EEPROM_CONTROL_AND_STATUS_REG_CTRL_CMD           15:13
#define NV_PLX_EEPROM_CONTROL_AND_STATUS_REG_CTRL_CMD_READ      0x00000003
#define NV_PLX_EEPROM_CONTROL_AND_STATUS_REG_STATUS             23:16
#define NV_PLX_EEPROM_CONTROL_AND_STATUS_REG_ADDR_WIDTH         23:22
#define NV_PLX_EEPROM_CONTROL_AND_STATUS_REG_ADDR_WIDTH_2BYTE   0x00000002

#define NV_PLX_EEPROM_STATUS_REG_ADDR                           0x00000262
#define NV_PLX_EEPROM_STATUS_REG_ADDR_TRANSACTION               3:3
#define NV_PLX_EEPROM_STATUS_REG_ADDR_TRANSACTION_COMPLETE      0x00000000
#define NV_PLX_EEPROM_STATUS_REG_ADDR_TRANSACTION_NOT_COMPLETE  0x00000001

#define NV_PLX_EEPROM_BUFFER_ADDR                               0x00000264

#define NV_PLX_EEPROM_DATA_ADDR_ZERO                            0x00000000
#define NV_PLX_EEPROM_DATA_ADDR_ZERO_SIGNATURE                  7:0
#define NV_PLX_EEPROM_DATA_ADDR_ZERO_CONFIGBYTE_COUNT           31:16

#define HWBC_UPSTREAM_BUS_SPEED_GEN1PCIE                   1
#define HWBC_UPSTREAM_BUS_SPEED_GEN2PCIE                   2
#define HWBC_UPSTREAM_BUS_SPEED_GEN3PCIE                   3

static NV_STATUS Plx_Pex8747_setupFunc(OBJHWBC *pPlx, OBJCL *pCl);
static NV_STATUS Plx_Pex8747_ChangeUpstreamBusSpeed(OBJHWBC *pPlx, OBJCL *pCl, NvU32 cmd);
static RmPhysAddr Plx_Pex8747_GetBar0(OBJCL *pCl, OBJHWBC *pPlx);

//
// static functions
//

static OBJHWBC  *objClFindUpperHWBC(OBJCL *, NBADDR, OBJHWBC *, RmPhysAddr);
static NV_STATUS objClSetupBR03(OBJHWBC *, OBJCL *);
static NvU32     objClGetBr03Bar0(OBJCL *, OBJHWBC *);
static NV_STATUS objClFreeBr03Bar0(OBJCL *, OBJHWBC *);

static NV_STATUS objClResumeBridgeHWBC(OBJCL *, OBJHWBC *);

static NV_STATUS Nvidia_BR04_ShiftAliasingRegisters(RmPhysAddr);
static RmPhysAddr Nvidia_BR04_GetBar0(OBJCL *, OBJHWBC *, NvS32);
static NV_STATUS Nvidia_BR04_setupFunc(OBJHWBC *, OBJCL *);

//
// This function examines a PLX firmware ROM to retrieve firmware details.
//
void
plxPex8747GetFirmwareInfo
(
 OBJCL *pCl,
 OBJGPU *pGpu,
 OBJHWBC *pHWBC
)
{
    return;
} // end of Plx_Pex8747_GetFirmwareInfo()

//
// Find the broadcast resource in the direct upper hierarchy of the port
// Return NULL if none is found
// Do not perform per-gpu memory tracking as pCl remains
// during the SLI transitions.
//
static OBJHWBC *
objClFindUpperHWBC
(
    OBJCL *pCl,
    NBADDR port,
    OBJHWBC *child,
    RmPhysAddr currentGpuPhysAddr
)
{
    NvU16 vendorID, deviceID;
    void *handle;
    OBJHWBC *pHWBC = NULL;

    if (!port.valid)
    {
        goto objClFindUpperHWBC_exit;
    }

    handle   = osPciInitHandle(port.domain, port.bus, port.device, port.func, &vendorID, &deviceID);
    if (!handle)
    {
        goto objClFindUpperHWBC_exit;
    }

    if ((vendorID == PCI_VENDOR_ID_PLX) && (deviceID == PLX_DEVICE_ID_PEX8747))
    {
        pHWBC = portMemAllocNonPaged(sizeof(OBJHWBC));
        if (pHWBC == NULL)
        {
            NV_ASSERT_OR_GOTO((pHWBC != NULL), objClFindUpperHWBC_exit);
        }
        portMemSet((void *)pHWBC, 0, sizeof(OBJHWBC));

        pHWBC->bcRes = HWBC_PLX_PEX8747;

        // Find upstream port
        pHWBC->ctrlDev.domain = port.domain;
        pHWBC->ctrlDev.handle = clFindP2PBrdg(pCl, port.domain, port.bus,
                                              &pHWBC->ctrlDev.bus,
                                              &pHWBC->ctrlDev.device,
                                              &pHWBC->ctrlDev.func,
                                              &vendorID, &deviceID);
        // check everything
        NV_ASSERT(pHWBC->ctrlDev.handle);
        NV_ASSERT(vendorID == PCI_VENDOR_ID_PLX &&
                  deviceID == PLX_DEVICE_ID_PEX8747);
        NV_ASSERT(osPciReadWord(pHWBC->ctrlDev.handle,
                                PCI_COMMON_CLASS_SUBCLASS) ==
                  PCI_COMMON_CLASS_SUBBASECLASS_P2P);
        NV_ASSERT(osPciReadByte(pHWBC->ctrlDev.handle,
                  PCI_TYPE_1_SECONDARY_BUS_NUMBER) == port.bus);

        pHWBC->ctrlDev.valid = 1;

        // set upstream port info
        pHWBC->domain = port.domain;
        pHWBC->minBus = port.bus;
        pHWBC->maxBus = osPciReadByte(pHWBC->ctrlDev.handle,
                                      PCI_TYPE_1_SUBORDINATE_BUS_NUMBER);

        pHWBC->pFirstChild = child;
        pHWBC->hasPlxFirmwareInfo = NV_FALSE;

        pHWBC->hwbcId = gpuGenerate32BitId(pHWBC->ctrlDev.domain, pHWBC->ctrlDev.bus, pHWBC->ctrlDev.device | pHWBC->ctrlDev.func);

        goto objClFindUpperHWBC_exit;
    }

    if (vendorID != PCI_VENDOR_ID_NVIDIA)
    {
        goto objClFindUpperHWBC_exit;
    }

    if (deviceID == NV_BR03_XVU_DEV_ID_DEVICE_ID_BR03) // BR03
    {
        pHWBC = portMemAllocNonPaged(sizeof(OBJHWBC));
        if (pHWBC == NULL)
        {
            NV_ASSERT_OR_GOTO((pHWBC != NULL), objClFindUpperHWBC_exit);
        }
        portMemSet((void *)pHWBC, 0, sizeof(OBJHWBC));

        pHWBC->bcRes = HWBC_NVIDIA_BR03;

        pHWBC->ctrlDev.domain = port.domain;
        pHWBC->ctrlDev.handle = clFindP2PBrdg(pCl, port.domain, port.bus,
                                              &pHWBC->ctrlDev.bus,
                                              &pHWBC->ctrlDev.device,
                                              &pHWBC->ctrlDev.func,
                                              &vendorID, &deviceID);
        // check everything
        NV_ASSERT(pHWBC->ctrlDev.handle);
        NV_ASSERT(vendorID == PCI_VENDOR_ID_NVIDIA && deviceID == NV_BR03_XVU_DEV_ID_DEVICE_ID_BR03);
        NV_ASSERT(osPciReadWord(pHWBC->ctrlDev.handle, PCI_COMMON_CLASS_SUBCLASS) == PCI_COMMON_CLASS_SUBBASECLASS_P2P);
        NV_ASSERT(osPciReadByte(pHWBC->ctrlDev.handle, PCI_TYPE_1_SECONDARY_BUS_NUMBER) == port.bus);
        pHWBC->ctrlDev.valid = 1;

        // set upstream port info
        pHWBC->domain = port.domain;
        pHWBC->minBus = port.bus;
        pHWBC->maxBus = osPciReadByte(pHWBC->ctrlDev.handle, PCI_TYPE_1_SUBORDINATE_BUS_NUMBER);

        pHWBC->pFirstChild = child;
        pHWBC->gpuPhysAddr = currentGpuPhysAddr;

        pHWBC->hwbcId = gpuGenerate32BitId(pHWBC->ctrlDev.domain, pHWBC->ctrlDev.bus, pHWBC->ctrlDev.device | pHWBC->ctrlDev.func);

        if (!pCl->getProperty(pCl, PDB_PROP_CL_DISABLE_BR03_FLOW_CONTROL))
            objClSetupBR03(pHWBC, pCl);

        goto objClFindUpperHWBC_exit;
    }

    if (IS_DEVID_BR04(deviceID))
    {
        pHWBC = portMemAllocNonPaged(sizeof(OBJHWBC));
        if (pHWBC == NULL)
        {
            NV_ASSERT_OR_GOTO((pHWBC != NULL), objClFindUpperHWBC_exit);
        }
        portMemSet((void *)pHWBC, 0, sizeof(OBJHWBC));

        pHWBC->bcRes = HWBC_NVIDIA_BR04;

        // Find upstream port
        pHWBC->ctrlDev.domain = port.domain;
        pHWBC->ctrlDev.handle = clFindP2PBrdg(pCl, port.domain, port.bus,
                                              &pHWBC->ctrlDev.bus,
                                              &pHWBC->ctrlDev.device,
                                              &pHWBC->ctrlDev.func,
                                              &vendorID, &deviceID);
        // check everything
        NV_ASSERT(pHWBC->ctrlDev.handle);
        NV_ASSERT(vendorID == PCI_VENDOR_ID_NVIDIA &&
                  deviceID >= NV_BR04_XVU_DEV_ID_DEVICE_ID_BR04_0 &&
                  deviceID <= NV_BR04_XVU_DEV_ID_DEVICE_ID_BR04_15);
        NV_ASSERT(osPciReadWord(pHWBC->ctrlDev.handle,
                                PCI_COMMON_CLASS_SUBCLASS) ==
                  PCI_COMMON_CLASS_SUBBASECLASS_P2P);
        NV_ASSERT(osPciReadByte(pHWBC->ctrlDev.handle,
                  PCI_TYPE_1_SECONDARY_BUS_NUMBER) == port.bus);

        pHWBC->ctrlDev.valid = 1;

        // set upstream port info
        pHWBC->domain = port.domain;
        pHWBC->minBus = port.bus;
        pHWBC->maxBus = osPciReadByte(pHWBC->ctrlDev.handle,
                                      PCI_TYPE_1_SUBORDINATE_BUS_NUMBER);

        pHWBC->pFirstChild = child;
        pHWBC->hasPlxFirmwareInfo = NV_FALSE;

        pHWBC->hwbcId = gpuGenerate32BitId(pHWBC->ctrlDev.domain, pHWBC->ctrlDev.bus, pHWBC->ctrlDev.device | pHWBC->ctrlDev.func);

        pCl->br04HwbcCount++;

        goto objClFindUpperHWBC_exit;
    }

    handle = osPciInitHandle(port.domain, port.bus, 0, 0, &vendorID, &deviceID);

    if (!handle || vendorID != PCI_VENDOR_ID_NVIDIA)
    {
        NV_ASSERT(pHWBC == NULL);
        goto objClFindUpperHWBC_exit;
    }

    NV_ASSERT(pHWBC == NULL);

objClFindUpperHWBC_exit:
    return pHWBC;
}

//
// Find all Broadcast resources upstream of the GPU.
// This could be BR03, BR04, PLX, or chipset support.
//
NvBool
objClSetPcieHWBC
(
    OBJGPU *pGpu,
    OBJCL *pCl
)
{
    NBADDR port;
    NvU16 vendorID, deviceID;
    OBJHWBC **root = NULL, *father = NULL, *now = NULL, *next = NULL;


    // check if the upstream port is valid
    if (!pGpu || !pGpu->gpuClData.upstreamPort.addr.valid)
    {
        return NV_FALSE;
    }

    // for now all BC HW are from NVIDIA or PLX
    if ((pGpu->gpuClData.upstreamPort.VendorID != PCI_VENDOR_ID_NVIDIA) &&
        (pGpu->gpuClData.upstreamPort.VendorID != PCI_VENDOR_ID_PLX))
    {
        return NV_FALSE;
    }

    if (pGpu->gpuClData.upstreamPort.DeviceID ==
        NV_BR03_XVU_DEV_ID_DEVICE_ID_BR03)
    {
        pGpu->setProperty(pGpu, PDB_PROP_GPU_BEHIND_BR03, NV_TRUE);
    }
    else if (IS_DEVID_BR04(pGpu->gpuClData.upstreamPort.DeviceID))
    {
        pGpu->setProperty(pGpu, PDB_PROP_GPU_BEHIND_BR04, NV_TRUE);
    }

    port = pGpu->gpuClData.upstreamPort.addr;

    // now try to search from up to down
    root = &pCl->pHWBC;
    while (*root)
    {
        if (((*root)->domain == port.domain) && ((*root)->minBus <= port.bus) && ((*root)->maxBus >= port.bus)) // bus in range
        {
            father = *root;
            if (father->bcRes == HWBC_NVIDIA_BR03)
            {
                pGpu->setProperty(pGpu, PDB_PROP_GPU_BEHIND_BR03, NV_TRUE);
            }
            else if (father->bcRes == HWBC_NVIDIA_BR04)
            {
                pGpu->setProperty(pGpu, PDB_PROP_GPU_BEHIND_BR04, NV_TRUE);
            }
            root = &(*root)->pFirstChild;
        }
        else
        {
            root = &(*root)->pSibling;
        }
    }


    // search from down to up
    do
    {
        if (father && (father->domain == port.domain) && (father->minBus == port.bus))
        {
            if (now)
            {
                now->pParent = father;
            }
            break; // got it in the list
        }

        next = objClFindUpperHWBC(pCl, port, now,
                                  pGpu->busInfo.gpuPhysAddr);

        if (!next)
            break; // nothing else

        if (now)
        {
            now->pParent = next;
        }
        now = next;

        if (now->bcRes == HWBC_NVIDIA_BR03)
        {
            pGpu->setProperty(pGpu, PDB_PROP_GPU_BEHIND_BR03, NV_TRUE);
        }
        else if (now->bcRes == HWBC_NVIDIA_BR04)
        {
            pGpu->setProperty(pGpu, PDB_PROP_GPU_BEHIND_BR04, NV_TRUE);
        }

        port.domain = now->ctrlDev.domain;
        port.handle = clFindP2PBrdg(pCl, now->ctrlDev.domain, now->ctrlDev.bus,
                                    &port.bus,
                                    &port.device,
                                    &port.func,
                                    &vendorID,
                                    &deviceID);

    } while (port.handle && (vendorID == PCI_VENDOR_ID_NVIDIA || vendorID == PCI_VENDOR_ID_PLX));

    *root = now;

    // search from top to bottom again and call setup on the bridges
    root = &pCl->pHWBC;
    port = pGpu->gpuClData.upstreamPort.addr;

    while (*root)
    {
        if (((*root)->domain == port.domain) && ((*root)->minBus <= port.bus) && ((*root)->maxBus >= port.bus)) // bus in range
        {
            if ((*root)->bcRes == HWBC_NVIDIA_BR04)
            {
                Nvidia_BR04_setupFunc(*root, pCl);
            }
            else if ((*root)->bcRes == HWBC_PLX_PEX8747)
            {
                Plx_Pex8747_setupFunc(*root, pCl);
            }
            root = &(*root)->pFirstChild;
        }
        else
            root = &(*root)->pSibling;
    }

    return NV_TRUE;
}

//
// We can access BR03 registers through 3 different ways
// 1. BR03 BAR0
// 2. PCI Express Enhanced Configuration Space
// 3. Setup BR03 Register Aliasing then through GPU register space
//
// This function unifies all the above approaches
// and returns a 32bit physical address for the BR03 registers
//
// objClFreeBr03Bar0 must be called after we finish setting the registers
//
static NvU32
objClGetBr03Bar0
(
    OBJCL *pCl,
    OBJHWBC *pBR03
)
{
    NvU8 Rev;
    NvU32 bar0 = 0;

    if (!pBR03 ||
         pBR03->bcRes != HWBC_NVIDIA_BR03 ||
         !pBR03->ctrlDev.valid)
        return 0;

    Rev = osPciReadByte(pBR03->ctrlDev.handle, NV_BR03_XVU_REV_CC);

    if (Rev != 0xA1) // Rev A1 BAR0 is broken
        bar0 = (osPciReadDword(pBR03->ctrlDev.handle, PCI_BASE_ADDRESS_0) & PCI_BASE_ADDRESS_0_VALID_MASK) & ~RM_PAGE_MASK;

    //
    // Warning: Future OS may forbid us to map the PCIE enhanced configuration space directly
    //
    if (!bar0 &&
         pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE) &&
         pCl->pPcieConfigSpaceBase)
    {
        RmPhysAddr pcieConfigSpaceBase;

        pcieConfigSpaceBase = clFindPcieConfigSpaceBase(pCl,
                                                        pBR03->ctrlDev.domain,
                                                        pBR03->ctrlDev.bus);
        if ((pcieConfigSpaceBase) &&
           ((pcieConfigSpaceBase & ~((RmPhysAddr)(0xFFFFFFFF))) == 0 ))
        {
            //
            // if pcieConfigSpaceBase is limited to under 4GB then the cast is fine.
            //
            bar0 = (NvU32)pcieConfigSpaceBase | pBR03->ctrlDev.bus << PCIE_BUS_SHIFT;
        }
    }

    if (!bar0) // we can not setup any registers
        return bar0;

     //
     // Implement Register Aliasing
     // gpuPhysAddr has to be set to a 32bits address
     //
    if (pBR03->gpuPhysAddr && !(pBR03->gpuPhysAddr>>32))
    {
        volatile NvU32 *pData;
        NvU32 addr = (NvU32)pBR03->gpuPhysAddr + BR03_GPU_REGISTER_ALIAS_OFFSET;

        pData = osMapKernelSpace(bar0, RM_PAGE_SIZE, NV_MEMORY_UNCACHED,
                                 NV_PROTECT_READ_WRITE);

        if (!pData)
            return 0;

        // must not be enabled already
        if (REF_VAL(NV_BR03_XVU_MCC_REG_ALIAS_ACCESS, pData[NV_BR03_XVU_MCC_REG_ALIAS / sizeof(*pData)]) != 0)

        {
            osUnmapKernelSpace((void*)pData, RM_PAGE_SIZE);
            return HWBC_ERROR_BR03_INVALID_BAR0;
        }

        // enable the register aliasing and setup the address
        pData[NV_BR03_XVU_MCC_REG_ALIAS / sizeof(*pData)] =
            (DRF_SHIFTMASK(NV_BR03_XVU_MCC_REG_ALIAS_BASE_ADDRESS) & addr) |
            REF_NUM(NV_BR03_XVU_MCC_REG_ALIAS_ACCESS, NV_BR03_XVU_MCC_REG_ALIAS_ACCESS_ENABLED);

        osUnmapKernelSpace((void*) pData, RM_PAGE_SIZE);

        return addr;
    }

    return bar0;
}

static NV_STATUS
objClFreeBr03Bar0
(
    OBJCL *pCl,
    OBJHWBC *pBR03
)
{
    NvU8 Rev;
    NvU32 bar0 = 0;
    volatile NvU32 *pData;

    if (!pBR03 ||
         pBR03->bcRes != HWBC_NVIDIA_BR03 ||
         !pBR03->ctrlDev.valid)
        return NV_ERR_GENERIC;

    Rev = osPciReadByte(pBR03->ctrlDev.handle, NV_BR03_XVU_REV_CC);

    if (Rev != 0xA1) // Rev A1 BAR0 is broken
        bar0 = (osPciReadDword(pBR03->ctrlDev.handle, PCI_BASE_ADDRESS_0) & PCI_BASE_ADDRESS_0_VALID_MASK) & ~RM_PAGE_MASK;

    //
    // Warning: Future OS may forbid us to map the PCIE enhanced configuration space directly
    //
    if (!bar0 &&
         pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE) &&
         pCl->pPcieConfigSpaceBase)
    {
        RmPhysAddr pcieConfigSpaceBase;

        pcieConfigSpaceBase = clFindPcieConfigSpaceBase(pCl,
                                                        pBR03->ctrlDev.domain,
                                                        pBR03->ctrlDev.bus);
        if ((pcieConfigSpaceBase) &&
           ((pcieConfigSpaceBase & ~((RmPhysAddr)(0xFFFFFFFF))) == 0 ))
        {
            // If pcieConfigSpaceBase is limited under 4GB then the cast is fine.
            bar0 = (NvU32)pcieConfigSpaceBase | pBR03->ctrlDev.bus << PCIE_BUS_SHIFT;
        }
    }

    if (!bar0) // we can not setup any registers
        return NV_ERR_GENERIC;

    // check if register alias has been enabled
    pData = osMapKernelSpace(bar0, RM_PAGE_SIZE, NV_MEMORY_UNCACHED,
                             NV_PROTECT_READ_WRITE);

    if (!pData)
        return NV_ERR_GENERIC;

    // if enabled then disable it
    if (REF_VAL(NV_BR03_XVU_MCC_REG_ALIAS_ACCESS, pData[NV_BR03_XVU_MCC_REG_ALIAS / sizeof(*pData)]))
    {
        pData[NV_BR03_XVU_MCC_REG_ALIAS / sizeof(*pData)] =
            REF_NUM(NV_BR03_XVU_MCC_REG_ALIAS_ACCESS, NV_BR03_XVU_MCC_REG_ALIAS_ACCESS_DISABLED);
    }

    osUnmapKernelSpace((void*) pData, RM_PAGE_SIZE);

    return NV_OK;
}

//
// Setup the registers for BR03
//
static NV_STATUS
objClSetupBR03
(
    OBJHWBC *pBR03,
    OBJCL *pCl
)
{
    void *handle;
    NvU16 vendorID, deviceID;
    volatile NvU32 *pData;
    NvU32 bar0 = 0, addr, dev, dport, sport, type;
    NvU32 bufferSize[4][3]; // credit limit
    NvU8 needRes[4] = {1, 0, 0, 0}, total = 0, totalDevices = 3;
    NvU32 laneWidth[2] = {0, 0};

    // check if it is BR03
    if (!pBR03 || pBR03->bcRes != HWBC_NVIDIA_BR03)
        return NV_ERR_GENERIC;

    if (!pBR03->ctrlDev.handle)
        return NV_ERR_GENERIC;

    bar0 = objClGetBr03Bar0(pCl, pBR03);

    if (!bar0)
    {
        // In this case, use specific error code
        if (bar0 == HWBC_ERROR_BR03_INVALID_BAR0)
            return NV_ERR_INVALID_DEVICE;
        else
            return NV_ERR_GENERIC;
    }


    pData = osMapKernelSpace(bar0, (3 * RM_PAGE_SIZE), NV_MEMORY_UNCACHED,
                             NV_PROTECT_READ_WRITE);

    if (!pData)
        return NV_ERR_GENERIC;

    if (pData[NV_BR03_XVU_INT_FLOW_CTL / sizeof(*pData)])
    {
        NV_PRINTF(LEVEL_WARNING,
                  "*** BR03 registers has already been programmed!\n");
        osUnmapKernelSpace((void*) pData, 3 * RM_PAGE_SIZE);
        objClFreeBr03Bar0(pCl, pBR03);
        return NV_OK;
    }

    // check which devices are connected
    for (dev = 0; dev < 2; dev++)
    {
        NvU8 bus;
        handle = osPciInitHandle(pBR03->domain, pBR03->minBus, (NvU8)dev, 0, &vendorID, &deviceID);
        if (!handle) continue;

        // read laneWidth
        laneWidth[dev] = CL_MAX_LINK_WIDTH(osPciReadDword(handle, NV_BR03_XVD_LINK_CTRLSTAT) >> 16);

        bus = (NvU8)osPciReadByte(handle, PCI_TYPE_1_SECONDARY_BUS_NUMBER);
        vendorID = PCI_INVALID_VENDORID;
        handle = osPciInitHandle(pBR03->domain, bus, 0, 0, &vendorID, &deviceID);
        // we only allocated resource for NVIDIA device
        if (handle && vendorID == PCI_VENDOR_ID_NVIDIA)
        {
            needRes[dev+1] = 1;
            total++;
        } else
        if (!handle || !PCI_IS_VENDORID_VALID(vendorID))
            totalDevices--;
    }

    NV_ASSERT(total);

    // work around for INT_FLOW_CTL state updating bug
    if (totalDevices == 2) // one device is not connected
    {
        for (addr = NV_BR03_XVU_INT_FLOW_CTL_DP0_TO_UP0_CPL / sizeof(*pData);
                addr <= NV_BR03_XVU_INT_FLOW_CTL_UP0_TO_MH0_PW / sizeof(*pData);
                addr ++)
        {
            if (pData[addr]) // if any one of them get updated?
            {
                NV_PRINTF(LEVEL_WARNING,
                          "*** BR03 registers has already been programmed (one device workaround)!\n");
                osUnmapKernelSpace((void*) pData, 3 * RM_PAGE_SIZE);
                objClFreeBr03Bar0(pCl, pBR03);
                return NV_OK;
            }
        }
    }

    // Try to read all the credit limit first
    addr = NV_BR03_XVU_UP0_INT_BUFSIZE_CPL / sizeof(*pData);
    for (sport=0; sport<4; sport++) // for up0, dp0, dp1, mh0 separately
    {
        for (type=0; type<3; type++) // for cpl, np, pw separately
        {
            bufferSize[sport][type] = pData[addr];
            addr ++;
        }
        addr ++;
    }

    NV_PRINTF(LEVEL_INFO, "*** Setup BR03 registers!\n");

    // Now set it
    for (sport=0; sport<4; sport++) // source up0, dp0, dp1 or mh0
    {
        NvU8 alloc = 0;
        if (!needRes[sport])
            continue;

        // register address
        addr = ((sport + 3) % 4 * 0x30 + NV_BR03_XVU_INT_FLOW_CTL_DP0_TO_UP0_CPL) / sizeof(*pData);

        for (dport=0; dport<4; dport++) // dest up0, dp0, dp1 or mh0
        {
            if (sport == dport)
                continue;
            if (needRes[dport])
            {
                for (type=0; type<3; type++) // type cpl, np, pw
                {
                    NvS16 theader, tdata, header, data;
                    theader = (NvS16)REF_VAL(NV_BR03_XVU_UP0_INT_BUFSIZE_CPL_H, bufferSize[sport][type]) - totalDevices;
                    tdata = (NvS16)REF_VAL(NV_BR03_XVU_UP0_INT_BUFSIZE_CPL_D, bufferSize[sport][type]) - totalDevices * 8;

                    if (type == 1) // np data is always 0
                    {
                        theader += 1; // MH0 doesn't use any NPH credits
                        tdata = 0;
                    }

                    NV_ASSERT(theader >= 0 && tdata >= 0);

                    header = theader / total + (theader % total > alloc);
                    data = tdata / total + (tdata % total > alloc);

                    pData[addr + type] = REF_NUM(NV_BR03_XVU_UP0_INT_BUFSIZE_CPL_H, header) |
                        REF_NUM(NV_BR03_XVU_UP0_INT_BUFSIZE_CPL_D, data);
                }
                alloc++;
            }
            addr += 4;
        }
    }

    // now update the change
    // BR03_REG32(pData, INT_FLOW_CTL) = 1;
    pData[NV_BR03_XVU_INT_FLOW_CTL / sizeof(*pData)] = 1;

    // ITX Allocation
    // BR03_REG32(pData, ITX_ALLOCATION) =
    pData[NV_BR03_XVU_ITX_ALLOCATION / sizeof(*pData)] =
        REF_NUM(NV_BR03_XVU_ITX_ALLOCATION_UP0, 6) |
        REF_NUM(NV_BR03_XVU_ITX_ALLOCATION_DP0, 4) |
        REF_NUM(NV_BR03_XVU_ITX_ALLOCATION_DP1, 4) |
        REF_NUM(NV_BR03_XVU_ITX_ALLOCATION_MH0, 2);

    // OPPORTUNISTIC_ACK and OPPORTUNISTIC_UPDATE_FC for UP0, DP0 and DP1
    pData[NV_BR03_XVU_XP_0 / sizeof(*pData)] |=
        REF_NUM(NV_BR03_XVU_XP_0_OPPORTUNISTIC_ACK, 1) |
        REF_NUM(NV_BR03_XVU_XP_0_OPPORTUNISTIC_UPDATE_FC, 1);
    pData[(NV_BR03_XVD_XP_0 + RM_PAGE_SIZE) / sizeof(*pData)] |=
        REF_NUM(NV_BR03_XVD_XP_0_OPPORTUNISTIC_ACK, 1) |
        REF_NUM(NV_BR03_XVD_XP_0_OPPORTUNISTIC_UPDATE_FC, 1);
    pData[(NV_BR03_XVD_XP_0 + 2 * RM_PAGE_SIZE) / sizeof(*pData)] |=
        REF_NUM(NV_BR03_XVD_XP_0_OPPORTUNISTIC_ACK, 1) |
        REF_NUM(NV_BR03_XVD_XP_0_OPPORTUNISTIC_UPDATE_FC, 1);

    // both 8 lanes, mini-dagwood
    // program UPDATE_FC_THRESHOLD to optimize flow control settings
    if (laneWidth[0] == 8 && laneWidth[1] == 8)
    {
        pData[(NV_BR03_XVD_XP_0 + RM_PAGE_SIZE) / sizeof(*pData)] =
            FLD_SET_DRF_NUM(_BR03_XVD, _XP_0, _UPDATE_FC_THRESHOLD, 0x19, pData[(NV_BR03_XVD_XP_0 + RM_PAGE_SIZE) / sizeof(*pData)]);
        pData[(NV_BR03_XVD_XP_0 + 2 * RM_PAGE_SIZE) / sizeof(*pData)] =
            FLD_SET_DRF_NUM(_BR03_XVD, _XP_0, _UPDATE_FC_THRESHOLD, 0x18, pData[(NV_BR03_XVD_XP_0 + 2 * RM_PAGE_SIZE) / sizeof(*pData)]);
    }

    osUnmapKernelSpace((void*) pData, 3 * RM_PAGE_SIZE);
    objClFreeBr03Bar0(pCl, pBR03);

    return NV_OK;
}



NV_STATUS
clResumeBridge_IMPL
(
    OBJCL *pCl
)
{
    NV_STATUS status = NV_OK;
    NvBool    bFirstGpuResuming = NV_TRUE;
    OBJGPU   *pGpu = NULL;
    NvU32     gpuCount, gpuMask, gpuIndex, gpuResumingCount;

    if (!pCl->pHWBC)
    {
        return NV_OK;
    }
    //
    // We set the bridge when the 1st GPU is resuming
    //
    gpuIndex = gpuResumingCount = 0;
    gpumgrGetGpuAttachInfo(&gpuCount, &gpuMask);
    while (((pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex)) != NULL) &&
           (bFirstGpuResuming == NV_TRUE))
    {
        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_PM_RESUME_CODEPATH))
        {
            if (++gpuResumingCount > 1)
            {
                bFirstGpuResuming = NV_FALSE;
            }
        }
        else if (gpuIsGpuFullPower(pGpu))
        {
            bFirstGpuResuming = NV_FALSE;
        }
    }

    if (bFirstGpuResuming == NV_TRUE)
    {
        status = objClResumeBridgeHWBC(pCl, pCl->pHWBC);
    }

    return status;
}

static NV_STATUS
objClResumeBridgeHWBC
(
    OBJCL *pCl,
    OBJHWBC *pHWBC
)
{
    NV_STATUS status = NV_OK;

    if (pHWBC->bcRes == HWBC_PLX_PEX8747)
    {
        status = Plx_Pex8747_setupFunc(pHWBC, pCl);
    }

    if (status != NV_OK)
    {
        return status;
    }

    if (pHWBC->pSibling)
    {
        status = objClResumeBridgeHWBC(pCl, pHWBC->pSibling);
        if (status != NV_OK)
        {
            return status;
        }
    }

    if (pHWBC->pFirstChild)
    {
        status = objClResumeBridgeHWBC(pCl, pHWBC->pFirstChild);
    }

    return status;
}


//
// Determine whether a set of GPUs are connected under a conjoint BR04 heirarchy
//
// If flat is NV_TRUE test whether the GPUs are under the same BR04.
//
// If devId is not 0 test for a specific device ID.
//
NvBool
clFindBR04_IMPL
(
    OBJGPU **pGpus,
    NvU32    numGpus,
    NvBool   flat,
    NvU32    devId,
    OBJCL   *pCl
)
{
    NvU32 i;

    // Sanity check inputs
    if (0 == numGpus)
        return NV_FALSE;

    for (i = 0; i < numGpus; i++)
    {
        if (NULL == pGpus[i])
            return NV_FALSE;
    }

    //
    // Make sure all GPUs' upstream ports are BR04.  Check also if root port is
    // the same as first GPU's.  If all root ports are the same we should have
    // a conjunct BR04 heirarchy.
    //
    // If we're testing for a "flat" heirarchy, where all GPUs are attached
    // directly to the same BR04, check board downstream port.
    //
    for (i = 0; i < numGpus; i++)
    {
        // Testing if upstream port PCI addresses are valid won't work, because
        // those ports may be hidden and not have PCI addresses.  They should
        // still have VendorID/DevID set.

        if (PCI_VENDOR_ID_NVIDIA != pGpus[i]->gpuClData.upstreamPort.VendorID ||
            NV_BR04_XVU_DEV_ID_DEVICE_ID_BR04_0 >
                                    pGpus[i]->gpuClData.upstreamPort.DeviceID ||
            NV_BR04_XVU_DEV_ID_DEVICE_ID_BR04_15 <
                                    pGpus[i]->gpuClData.upstreamPort.DeviceID ||
            (devId && devId != pGpus[i]->gpuClData.upstreamPort.DeviceID)
           )
        {
            return NV_FALSE;
        }

        if (flat)
        {
            if (!pGpus[i]->gpuClData.boardDownstreamPort.addr.valid)
                return NV_FALSE;

            if (pGpus[i]->gpuClData.boardDownstreamPort.addr.bus !=
                    pGpus[0]->gpuClData.boardDownstreamPort.addr.bus)
                return NV_FALSE;
        }
        else
        {
            if (!pGpus[i]->gpuClData.rootPort.addr.valid)
                return NV_FALSE;
        }
    }

    return NV_TRUE;
}

//
// Nvidia_BR04_ShiftAliasingRegisters : Remaps the aliasing registers for
// the targetted BR04 to the first available empty slot
//
static NV_STATUS
Nvidia_BR04_ShiftAliasingRegisters
(
    RmPhysAddr addr
)
{
    OBJGPU *pGpu;
    NvU32 gpuMask, gpuInstance;
    NvU32 data;
    volatile NvU32 *pData = NULL;
    NvU32 minBus, maxBus, i;
    NvBool Shifted = NV_FALSE;

    pData = osMapKernelSpace(addr,
                             NV_BR04_XVU_CONFIG_SIZE,
                             NV_MEMORY_UNCACHED,
                             NV_PROTECT_READ_WRITE);
    if (pData == NULL)
    {
        return NV_ERR_GENERIC;
    }

    gpumgrGetGpuAttachInfo(NULL, &gpuMask);
    gpuInstance = 0;

    minBus = DRF_VAL(_BR04_XVU, _BUS, _SEC_NUMBER,
                     pData[NV_BR04_XVU_BUS / sizeof(*pData)]);
    maxBus = DRF_VAL(_BR04_XVU, _BUS, _SUB_NUMBER,
                     pData[NV_BR04_XVU_BUS / sizeof(*pData)]);


    while (!Shifted && ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance))))
    {
        // set up the aliasing in a downstream GPU

        if (gpuGetBus(pGpu) >= minBus &&
            gpuGetBus(pGpu) <= maxBus)
        {
            // Check in each of the two slots in the BR04 window.
            for (i = 0; i < 2; i++)
            {
                data = GPU_REG_RD32(pGpu, NV_BR04(i) + NV_BR04_XVU_DEV_ID);
                if (!IS_DEVID_BR04(DRF_VAL(_BR04_XVU, _DEV_ID, _DEVICE_ID,
                                           data)))
                {
                    // set up the aliasing in the first empty slot
                    pData[NV_BR04_XVU_MCC_REG_OFFSET / sizeof(*pData)] =
                        NV_BR04(i);
#                   define NV_BR04_XVU_MCC_REG_ALIAS_DONOR_BUS_CUSTOM 0x80
                    pData[NV_BR04_XVU_MCC_REG_ALIAS / sizeof(*pData)] =
                        REF_NUM(NV_BR04_XVU_MCC_REG_ALIAS_ACCESS,
                                NV_BR04_XVU_MCC_REG_ALIAS_ACCESS_ENABLED) |
                        REF_NUM(NV_BR04_XVU_MCC_REG_ALIAS_ADDR_SELECT,
                                NV_BR04_XVU_MCC_REG_ALIAS_ADDR_SELECT_MANUAL) |
                        REF_NUM(NV_BR04_XVU_MCC_REG_ALIAS_DONOR_BUS,
                                NV_BR04_XVU_MCC_REG_ALIAS_DONOR_BUS_CUSTOM) |
                        ((NvU32)pGpu->busInfo.gpuPhysAddr + NV_BR04(i));

                    // Per bug 461247, we need to delay at least 100us for the change to propagate.
                    osDelay(1);

                    Shifted = NV_TRUE;
                    break;
                }
            }
        }
    }

    // If we couldn't find an empty slot, we must mask another slot.
    gpuInstance = 0;
    while (!Shifted && ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance))))
    {
        // set up the aliasing in a downstream GPU
        if (gpuGetBus(pGpu) >= minBus &&
            gpuGetBus(pGpu) <= maxBus)
        {
            // Check in each of the two slots in the BR04 window.
            for (i = 0; i < 2; i++)
            {
                NvU32 SecBus, SubBus;

                SecBus = DRF_VAL(_BR04_XVU, _BUS, _SEC_NUMBER,
                  GPU_REG_RD32(pGpu, NV_BR04(i) + NV_BR04_XVU_BUS));
                SubBus = DRF_VAL(_BR04_XVU, _BUS, _SUB_NUMBER,
                  GPU_REG_RD32(pGpu, NV_BR04(i) + NV_BR04_XVU_BUS));


                if (SecBus != minBus || SubBus != maxBus)
                {
                    // set up the aliasing in the first different slot
                    pData[NV_BR04_XVU_MCC_REG_OFFSET / sizeof(*pData)] =
                        NV_BR04(i);
                    pData[NV_BR04_XVU_MCC_REG_ALIAS / sizeof(*pData)] =
                        REF_NUM(NV_BR04_XVU_MCC_REG_ALIAS_ACCESS,
                                NV_BR04_XVU_MCC_REG_ALIAS_ACCESS_ENABLED) |
                        REF_NUM(NV_BR04_XVU_MCC_REG_ALIAS_ADDR_SELECT,
                                NV_BR04_XVU_MCC_REG_ALIAS_ADDR_SELECT_MANUAL) |
                        REF_NUM(NV_BR04_XVU_MCC_REG_ALIAS_DONOR_BUS,
                                NV_BR04_XVU_MCC_REG_ALIAS_DONOR_BUS_CUSTOM) |
                        ((NvU32)pGpu->busInfo.gpuPhysAddr + NV_BR04(i));

                    // Per bug 461247, we need to delay at least 100us for the change to propagate.
                    osDelay(1);

                    Shifted = NV_TRUE;
                    break;
                }
            }
        }
    }


    osUnmapKernelSpace((void*)pData, NV_BR04_XVU_CONFIG_SIZE);

    return NV_OK;
}

//
// Nvidia_BR04_GetBar0 : Returns physical address where BR04 config space can
// be accessed.  Returns NULL on error.
//
// We can access BR04 registers in the following ways, from most- to
// least-preferred:
// 1. Through the region in GPU register space reserved for bridge register
// access
// 2. BR04 BAR0
// 3. PCIE enhanced config space
//
// There is no need for a "free" function as with BR03 because the register
// aliasing is done differently; the BR04 claims the space in the GPU hierarchy
// by default at startup.
//
// BR04 registers are accessed one PCI device at a time.  portNum should be < 0
// for the upstream port, or 0 through 3 for each of the downstream ports,
// respectively.
//
static RmPhysAddr
Nvidia_BR04_GetBar0
(
    OBJCL *pCl,
    OBJHWBC *pBR04,
    NvS32 portNum
)
{
    RmPhysAddr bar0 = 0;
    volatile NvU32* pData = NULL;
    NvU32 i;
    OBJGPU *pGpu;
    NvU32 gpuMask, gpuInstance;
    NvU32 data;
    NV_STATUS rmStatus;

    // Sanity checks
    if (!pBR04 ||
        pBR04->bcRes != HWBC_NVIDIA_BR04 ||
        !pBR04->ctrlDev.valid ||
        portNum > 3)
    {
        return 0;
    }

    //
    // Access through window in GPU BAR0 space
    //
    if (pBR04->gpuPhysAddr)
    {
        // Try the two slots in the GPU's window
        for (i = 0; i < 2; i++)
        {
            bar0 = pBR04->gpuPhysAddr + NV_BR04(i);

            pData = osMapKernelSpace(bar0,
                                     NV_BR04_XVU_CONFIG_SIZE,
                                     NV_MEMORY_UNCACHED,
                                     NV_PROTECT_READ_WRITE);
            if (!pData)
            {
                return 0;
            }

            if ( (NvU8)REF_VAL(NV_BR04_XVU_BUS_PRI_NUMBER,
                               pData[NV_BR04_XVU_BUS / sizeof(*pData)]) ==
                 pBR04->ctrlDev.bus )
            {
                osUnmapKernelSpace((void*)pData, NV_BR04_XVU_CONFIG_SIZE);

                // Downstream ports are within the window at an offset
                if (portNum >= 0)
                {
                    bar0 += NV_BR04_XVD_OFFSET(portNum);
                }
                return bar0;
            }

            osUnmapKernelSpace((void*)pData, NV_BR04_XVU_CONFIG_SIZE);
        }

        // Neither slot is correct.
        bar0 = 0;
    }

    gpumgrGetGpuAttachInfo(NULL, &gpuMask);
    gpuInstance = 0;

    // first pass, see if this BR04 is already visible
    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)))
    {
        //
        // Make sure we only try to look for slots in GPUs
        // downstream of ourselves. Checking this saves us a
        // few unnecessary BR04 register writes.
        //
        if (!(gpuGetBus(pGpu) >= pBR04->minBus &&
             gpuGetBus(pGpu) <= pBR04->maxBus))
        {
            continue;
        }

        // Check in each of the two slots in the BR04 window.
        for (i = 0; i < 2; i++)
        {
            data = GPU_REG_RD32(pGpu, NV_BR04(i) + NV_BR04_XVU_DEV_ID);
            if (IS_DEVID_BR04(DRF_VAL(_BR04_XVU, _DEV_ID, _DEVICE_ID, data)))
            {
                data = GPU_REG_RD32(pGpu, NV_BR04(i) + NV_BR04_XVU_BUS);
                if (DRF_VAL(_BR04_XVU, _BUS, _PRI_NUMBER, data) ==
                    pBR04->ctrlDev.bus)
                {
                    // Set BR04 address selection to manual to keep it from
                    // hopping to another GPU's BAR0 window.
                    data = GPU_REG_RD32(pGpu, NV_BR04(i) + NV_BR04_XVU_MCC_REG_ALIAS);
                    data = FLD_SET_DRF(_BR04, _XVU_MCC_REG_ALIAS, _ADDR_SELECT, _MANUAL, data);
                    data = FLD_SET_DRF(_BR04, _XVU_MCC_REG_ALIAS, _DONOR_BUS, _CUSTOM, data);
                    GPU_REG_WR32(pGpu, NV_BR04(i) + NV_BR04_XVU_MCC_REG_ALIAS, data);

                    pBR04->gpuPhysAddr = pGpu->busInfo.gpuPhysAddr;
                    return (pBR04->gpuPhysAddr + NV_BR04(i));
                }
            }
        }
    }

    // second pass, start rearranging aliasing registers
    gpuInstance = 0;
    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)))
    {
        //
        // Make sure we only try to look for slots in GPUs
        // downstream of ourselves. Checking this saves us a
        // few unnecessary BR04 register writes.
        //
        if (!(gpuGetBus(pGpu) >= pBR04->minBus &&
             gpuGetBus(pGpu) <= pBR04->maxBus))
        {
            continue;
        }

        // Check in each of the two slots in the BR04 window.
        for (i = 0; i < 2; i++)
        {
            data = GPU_REG_RD32(pGpu, NV_BR04(i) + NV_BR04_XVU_DEV_ID);
            if (IS_DEVID_BR04(DRF_VAL(_BR04_XVU, _DEV_ID, _DEVICE_ID, data)))
            {
                data = GPU_REG_RD32(pGpu, NV_BR04(i) + NV_BR04_XVU_BUS);
                if (DRF_VAL(_BR04_XVU, _BUS, _PRI_NUMBER, data) ==
                    pBR04->ctrlDev.bus)
                {
                    // Set BR04 address selection to manual to keep it from
                    // hopping to another GPU's BAR0 window.
                    data = GPU_REG_RD32(pGpu, NV_BR04(i) + NV_BR04_XVU_MCC_REG_ALIAS);
                    data = FLD_SET_DRF(_BR04, _XVU_MCC_REG_ALIAS, _ADDR_SELECT, _MANUAL, data);
                    data = FLD_SET_DRF(_BR04, _XVU_MCC_REG_ALIAS, _DONOR_BUS, _CUSTOM, data);
                    GPU_REG_WR32(pGpu, NV_BR04(i) + NV_BR04_XVU_MCC_REG_ALIAS, data);

                    pBR04->gpuPhysAddr = pGpu->busInfo.gpuPhysAddr;
                    return (pBR04->gpuPhysAddr + NV_BR04(i));
                }
                else
                {
                    rmStatus = Nvidia_BR04_ShiftAliasingRegisters(
                        pGpu->busInfo.gpuPhysAddr + NV_BR04(i));
                    // XXX handle Nvidia_BR04_ShiftAliasingRegisters() failures
                    NV_ASSERT(rmStatus == NV_OK);
                    i--; // look again
                }
            }
        }
    }

    //
    // Access through BR04 BAR0 space
    //
    // Usually downstream ports are located one bus number higher than upstream
    // port, with device number equal to downstream port number.
    //
    bar0 = osPciReadDword(pBR04->ctrlDev.handle, NV_BR04_XVU_BAR_0) &
           ~RM_PAGE_MASK;

    if (bar0)
    {
        if (portNum >= 0)
        {
            bar0 += NV_BR04_XVD_OFFSET(portNum);
        }

        // BR04 A01 WAR: find this BR04 a home
        rmStatus = Nvidia_BR04_ShiftAliasingRegisters(bar0);
        NV_ASSERT(rmStatus == NV_OK);

        return bar0;
    }

    // Else access through PCIE space
    if (pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE) &&
        pCl->pPcieConfigSpaceBase)
    {
        RmPhysAddr pcieConfigSpaceBase;

        pcieConfigSpaceBase = clFindPcieConfigSpaceBase(pCl,
                                                        pBR04->ctrlDev.domain,
                                                        pBR04->ctrlDev.bus);
        if ((pcieConfigSpaceBase) &&
           ((pcieConfigSpaceBase & ~((RmPhysAddr)(0xFFFFFFFF))) == 0 ))
        {
            if (portNum >= 0)
            {
                bar0 = pcieConfigSpaceBase |
                       ((pBR04->ctrlDev.bus + 1) << PCIE_BUS_SHIFT) |
                       (portNum << PCIE_DEVICE_SHIFT);
            }
            else
            {
                bar0 = pcieConfigSpaceBase | pBR04->ctrlDev.bus << PCIE_BUS_SHIFT;
            }
        }

        if (bar0 != 0)
        {
            // BR04 A01 WAR: find this BR04 a home
            rmStatus = Nvidia_BR04_ShiftAliasingRegisters(bar0);
            // XXX handle Nvidia_BR04_ShiftAliasingRegisters() failures
            NV_ASSERT(rmStatus == NV_OK);
        }
    }

    return bar0;
}



//
// Set up registers for BR04
//
static NV_STATUS
Nvidia_BR04_setupFunc
(
    OBJHWBC *pBR04,
    OBJCL *pCl
)
{
    volatile NvU32 *pData = NULL;    // Register access for upstream port
    volatile NvU32 *pDpData[4] = { NULL, NULL, NULL, NULL};  // Register access for each downstream port
    RmPhysAddr bar0;
    BR04_PORT tport, sport;     // Terminating port, source port
    NvU32 type;                  // Transaction type (cpl, non-posted, posted)
    NvU32 addr;                  // Index (# of dwords) into BAR0
    NvS32 i;
    NV_STATUS status = NV_OK;
    NvU32 numActiveDPs = 0;
    NvU8 CreditSet = 0;
    NvU32 regValue;
    NvU32 regValue2;
    NvBool enableCorrErrors = NV_FALSE;
    void *pHandle;
    NvU16 vendorID, deviceID;

    // Empirically determined values for port credit allocation
    // See HW bug 325819.
    static NvU16 PortCreditsHeader[2][3][NUM_BR04_PORTS][NUM_BR04_PORTS] =
    {
        { // 1-DP or 2-DP configuration
            { // type = CPL
                { 0,  0,  3,  0, 27,  0}, // tport = DP0
                { 0,  0,  0,  0,  0,  0}, // tport = DP1
                { 3,  0,  0,  0, 27,  0}, // tport = DP2
                { 0,  0,  0,  0,  0,  0}, // tport = DP3
                {31,  0, 30,  0,  0,  0}, // tport = UP0
                { 0,  0,  0,  0,  0,  0}  // tport = MH0
            },
            { // type = NP
                { 0,  0, 15,  0, 15,  0}, // tport = DP0
                { 0,  0,  0,  0,  0,  0}, // tport = DP1
                {15,  0,  0,  0, 15,  0}, // tport = DP2
                { 0,  0,  0,  0,  0,  0}, // tport = DP3
                {31,  0, 31,  0,  0,  0}, // tport = UP0
                { 0,  0,  0,  0,  0,  0}  // tport = MH0
            },
            { // type = PW
                { 0,  0, 14,  0, 15,  0}, // tport = DP0
                { 0,  0,  0,  0,  0,  0}, // tport = DP1
                {14,  0,  0,  0, 15,  0}, // tport = DP2
                { 0,  0,  0,  0,  0,  0}, // tport = DP3
                {31,  0, 30,  0,  0,  0}, // tport = UP0
                { 0,  0,  0,  0,  0,  0}  // tport = MH0
            }
        },
        { // 3-DP or 4-DP configuration
            { // type = CPL
                { 0,  3,  3,  3, 19,  0}, // tport = DP0
                { 3,  0,  3,  3, 19,  0}, // tport = DP1
                { 3,  3,  0,  3, 19,  0}, // tport = DP2
                { 3,  3,  3,  0, 19,  0}, // tport = DP3
                {15, 15, 15, 14,  0,  0}, // tport = UP0
                { 0,  0,  0,  0,  0,  0}  // tport = MH0
            },
            { // type = NP
                { 0,  7,  7,  7,  7,  0}, // tport = DP0
                { 7,  0,  7,  7,  7,  0}, // tport = DP1
                { 7,  7,  0,  7,  7,  0}, // tport = DP2
                { 7,  7,  7,  0,  7,  0}, // tport = DP3
                {15, 15, 15, 15,  0,  0}, // tport = UP0
                { 0,  0,  0,  0,  0,  0}  // tport = MH0
            },
            { // type = PW
                { 0,  7,  7,  6,  7,  0}, // tport = DP0
                { 7,  0,  7,  6,  7,  0}, // tport = DP1
                { 7,  6,  0,  7,  7,  0}, // tport = DP2
                { 7,  7,  6,  0,  7,  0}, // tport = DP3
                {15, 15, 15, 14,  0,  0}, // tport = UP0
                { 0,  0,  0,  0,  0,  0}  // tport = MH0
            }
        }
    };

    static NvU16 PortCreditsData[2][3][NUM_BR04_PORTS][NUM_BR04_PORTS] =
    {
        { // 1-DP or 2-DP configuration
            { // type = CPL
                {  0,   0,  24,   0, 216,   0}, // tport = DP0
                {  0,   0,   0,   0,   0,   0}, // tport = DP1
                { 24,   0,   0,   0, 216,   0}, // tport = DP2
                {  0,   0,   0,   0,   0,   0}, // tport = DP3
                {120,   0, 112,   0,   0,   0}, // tport = UP0
                {  0,   0,   0,   0,   0,   0}  // tport = MH0
            },
            { // type = NP
                {  0,   0,   0,   0,   0,   0}, // tport = DP0
                {  0,   0,   0,   0,   0,   0}, // tport = DP1
                {  0,   0,   0,   0,   0,   0}, // tport = DP2
                {  0,   0,   0,   0,   0,   0}, // tport = DP3
                {  0,   0,   0,   0,   0,   0}, // tport = UP0
                {  0,   0,   0,   0,   0,   0}  // tport = MH0
            },
            { // type = PW
                {  0,   0, 112,   0, 120,   0}, // tport = DP0
                {  0,   0,   0,   0,   0,   0}, // tport = DP1
                {112,   0,   0,   0, 120,   0}, // tport = DP2
                {  0,   0,   0,   0,   0,   0}, // tport = DP3
                {120,   0, 112,   0,   0,   0}, // tport = UP0
                {  0,   0,   0,   0,   0,   0}  // tport = MH0
            }
        },
        { // 3-DP or 4-DP configuration
            { // type = CPL
                {  0,  24,  24,  24, 152,   0}, // tport = DP0
                { 24,   0,  24,  24, 152,   0}, // tport = DP1
                { 24,  24,   0,  24, 152,   0}, // tport = DP2
                { 24,  24,  24,   0, 152,   0}, // tport = DP3
                { 56,  56,  56,  48,   0,   0}, // tport = UP0
                {  0,   0,   0,   0,   0,   0}  // tport = MH0
            },
            { // type = NP
                {  0,   0,   0,   0,   0,   0}, // tport = DP0
                {  0,   0,   0,   0,   0,   0}, // tport = DP1
                {  0,   0,   0,   0,   0,   0}, // tport = DP2
                {  0,   0,   0,   0,   0,   0}, // tport = DP3
                {  0,   0,   0,   0,   0,   0}, // tport = UP0
                {  0,   0,   0,   0,   0,   0}  // tport = MH0
            },
            { // type = PW
                {  0,  56,  56,  48,  56,   0}, // tport = DP0
                { 56,   0,  56,  48,  56,   0}, // tport = DP1
                { 56,  48,   0,  56,  56,   0}, // tport = DP2
                { 56,  56,  48,   0,  56,   0}, // tport = DP3
                { 56,  56,  56,  48,   0,   0}, // tport = UP0
                {  0,   0,   0,   0,   0,   0}  // tport = MH0
            }
        }
    };

    // Check if HWBC resource is BR04
    if (pBR04 == NULL || pBR04->bcRes != HWBC_NVIDIA_BR04)
    {
        return NV_ERR_GENERIC;
    }

    if (pBR04->ctrlDev.handle == NULL)
    {
        return NV_ERR_GENERIC;
    }

    // Set up config access
    if (0 == (bar0 = Nvidia_BR04_GetBar0(pCl, pBR04, -1)))
    {
        return NV_ERR_GENERIC;
    }

    pData = osMapKernelSpace(bar0,
                             NV_BR04_XVU_CONFIG_SIZE,
                             NV_MEMORY_UNCACHED,
                             NV_PROTECT_READ_WRITE);
    if (pData == NULL)
    {
        return NV_ERR_GENERIC;
    }

    for (i = 0; i < 4; i++)
    {
        pDpData[i] = NULL;
        if (0 == (bar0 = Nvidia_BR04_GetBar0(pCl, pBR04, i)) ||
            NULL == (pDpData[i] = osMapKernelSpace(bar0,
                                                   NV_BR04_XVD_CONFIG_SIZE,
                                                   NV_MEMORY_UNCACHED,
                                                   NV_PROTECT_READ_WRITE)))
        {
            // If we've failed here we have to roll back any successful
            // mappings, then return error.
            for (i--; i >= 0; i--)
            {
                if (pDpData[i] != NULL)
                {
                    osUnmapKernelSpace((void*)(pDpData[i]),
                                       NV_BR04_XVD_CONFIG_SIZE);
                }
            }
            osUnmapKernelSpace((void*)pData, NV_BR04_XVU_CONFIG_SIZE);

            return NV_ERR_GENERIC;
        }
    }

    //
    // Set clock trims for BR04 A01.  See bug 344709 and bug 333577 for more details.
    //

    regValue = pData[NV_BR04_XVU_REV_CC / sizeof(*pData)];
    if (DRF_VAL(_BR04_XVU, _REV_CC, _MAJOR_REVISION_ID, regValue) == 0xa)
    {
        switch(DRF_VAL(_BR04_XVU, _REV_CC, _MINOR_REVISION_ID, regValue))
        {
        // A01
        case 1:
            NV_PRINTF(LEVEL_INFO, "*** Set clock trims for BR04 A01.\n");
            pData[NV_BR04_XVU_CTRIM_DP_1 / sizeof(*pData)] = 0x10D4A0E8;
            pData[NV_BR04_XVU_CTRIM_DP_2 / sizeof(*pData)] = 0x12639CC8;
            pData[NV_BR04_XVU_CTRIM_DP_3 / sizeof(*pData)] = 0x107418E7;
            pData[NV_BR04_XVU_CTRIM_DP_4 / sizeof(*pData)] = 0x10C424C5;
            pData[NV_BR04_XVU_CTRIM_DP_5 / sizeof(*pData)] = 0x10953128;
            pData[NV_BR04_XVU_CTRIM_DP_6 / sizeof(*pData)] = 0x00000129;
            break;

        // A03
        case 3:
            //
            // Turn off ASPM on BR04 Upstream Port for BR04 A03.
            //

            regValue = pData[NV_BR04_XVU_CYA_NIBBLE0 / sizeof(*pData)];
            regValue2 = FLD_SET_DRF(_BR04, _XVU_CYA_NIBBLE0, _RSVD_0, _UP0_ASPM_DISABLE, regValue);
            if (regValue2 != regValue)
            {
                pData[NV_BR04_XVU_CYA_NIBBLE0 / sizeof(*pData)] = regValue2;
            }
            break;

        default:
            break;
        }
    }

    //
    // Allow Gen2 on upstream and downstream ports, if supported.
    //

    NV_PRINTF(LEVEL_INFO, "*** Enabling BR04 Gen2 features.\n");

    // Enable Gen2 support globally on BR04 links.
    regValue = pData[NV_BR04_XVU_BOOT_1 / sizeof(*pData)];
    regValue2 = FLD_SET_DRF(_BR04, _XVU_BOOT_1, _LINK_SPEED, _5000, regValue);
    if (regValue2 != regValue)
    {
        pData[NV_BR04_XVU_BOOT_1 / sizeof(*pData)] = regValue2;
    }

    // Set supported data rates to include Gen2 on downstream links.
    for (i = 0; i < 4; ++i)
    {
        regValue = pDpData[i][NV_BR04_XVD_LINK_CTRLSTAT2 / sizeof(*pDpData[i])];
        regValue2 = FLD_SET_DRF(_BR04, _XVD_LINK_CTRLSTAT2, _TARGET_LINK_SPEED, _5P0G, regValue);
        if (regValue2 != regValue)
        {
            pDpData[i][NV_BR04_XVD_LINK_CTRLSTAT2 / sizeof(*pDpData[i])] = regValue2;

            regValue = pDpData[i][NV_BR04_XVD_G2_PRIV_XP_LCTRL_2 / sizeof(*pDpData[i])];
            regValue = FLD_SET_DRF(_BR04, _XVD_G2_PRIV_XP_LCTRL_2, _ADVERTISED_RATE_CHANGE, _ONE, regValue);
            pDpData[i][NV_BR04_XVD_G2_PRIV_XP_LCTRL_2 / sizeof(*pDpData[i])] = regValue;
        }
    }


    // Have at least a 1us delay after an advertised rate change to avoid a chipset replay on the next transaction.
    // Bug 778455.
    osDelay(1);

    regValue = pData[NV_BR04_XVU_ROM_REVISION / sizeof(*pData)];
    if (REF_VAL(NV_BR04_XVU_ROM_REVISION_ID_UPPER_16BITS, regValue) ==
      NV_BR04_XVU_ROM_REVISION_ID_UPPER_16BITS_P737 ||
      REF_VAL(NV_BR04_XVU_ROM_REVISION_ID_UPPER_16BITS, regValue) ==
      NV_BR04_XVU_ROM_REVISION_ID_UPPER_16BITS_P535)
    {
        //
        // Set deemphasis to -2.2Db on BR04 Upstream Port on P737 and P535
        //
        regValue = pData[NV_XPU_PEX_PAD_CTL_3 / sizeof(*pData)];
        regValue2 = regValue;
        regValue2 = FLD_SET_DRF(_XPU, _PEX_PAD_CTL_3, _TX_PEAK_R2_1C, _22DB, regValue2);
        if (regValue2 != regValue)
        {
            pData[NV_XPU_PEX_PAD_CTL_3 / sizeof(*pData)] = regValue2;
        }
    }
    else
    {
        //
        // Set deemphasis to -3.5Db on all BR04 Upstream Ports
        //
        regValue = pData[NV_XPU_PEX_PAD_CTL_3 / sizeof(*pData)];
        regValue2 = regValue;
        regValue2 = FLD_SET_DRF(_XPU, _PEX_PAD_CTL_3, _TX_PEAK_R2_1C, _36DB, regValue2);
        if (regValue2 != regValue)
        {
            pData[NV_XPU_PEX_PAD_CTL_3 / sizeof(*pData)] = regValue2;
        }
    }

    //
    // Set NV_PES_XVU_CYA_BIT0 [31:28] for the DPs where an NV device is
    // connected.  This ensures these ports are at -3.5dB and the rest remain
    // untouched.
    //

    regValue2 = pData[NV_BR04_XVU_CYA_BIT0 / sizeof(*pData)];
    for (i = 0; i < 4; ++i)
    {
        regValue = pDpData[i][NV_BR04_XVD_BUS / sizeof(*pDpData[i])];
        if (regValue == 0xFFFFFFFF)
            continue;

        pHandle = osPciInitHandle(pBR04->domain, (NvU8) REF_VAL(NV_BR04_XVD_BUS_SEC_NUMBER, regValue), 0, 0, &vendorID, &deviceID);
        if (pHandle != NULL && vendorID == PCI_VENDOR_ID_NVIDIA)
        {
            switch(i)
            {
                case 0:
                    regValue2 = FLD_SET_DRF(_BR04, _XVU_CYA_BIT0, _RSVD_28, _DP0_DE_EMP_NEG_3P5_DB,
                                            regValue2);
                    break;
                case 1:
                    regValue2 = FLD_SET_DRF(_BR04, _XVU_CYA_BIT0, _RSVD_29, _DP1_DE_EMP_NEG_3P5_DB,
                                            regValue2);
                    break;
                case 2:
                    regValue2 = FLD_SET_DRF(_BR04, _XVU_CYA_BIT0, _RSVD_30, _DP2_DE_EMP_NEG_3P5_DB,
                                            regValue2);
                    break;
                case 3:
                    regValue2 = FLD_SET_DRF(_BR04, _XVU_CYA_BIT0, _RSVD_31, _DP3_DE_EMP_NEG_3P5_DB,
                                            regValue2);
                    break;
            }
        }
    }
    pData[NV_BR04_XVU_CYA_BIT0 / sizeof(*pData)] = regValue2;


    //
    // Set NV_BR04_XVU_G2_PRIV_XP_LCTRL_2_CYA_DEEMPHASIS_OVERRIDE_ENABLED
    //
    regValue = pData[NV_BR04_XVU_G2_PRIV_XP_LCTRL_2 / sizeof(*pData)];
    regValue2 = regValue;
    regValue2 = FLD_SET_DRF(_BR04, _XVU_G2_PRIV_XP_LCTRL_2, _CYA_DEEMPHASIS_OVERRIDE, _ENABLED,
                            regValue2);
    if (regValue2 != regValue)
    {
        pData[NV_BR04_XVU_G2_PRIV_XP_LCTRL_2 / sizeof(*pData)] = regValue2;
    }

    //
    // Train upstream port to Gen2, if supported.
    //

    NV_PRINTF(LEVEL_INFO, "*** Setup BR04 upstream link speed.\n");

    // First check to make sure the other end of the link supports Gen2.  If not, skip all this.
    regValue = pData[NV_BR04_XVU_G2_PRIV_XP_LCTRL_2 / sizeof(*pData)];
    if (REF_VAL(NV_BR04_XVU_G2_PRIV_XP_LCTRL_2_DATA_RATE_SUPPORTED_REMOTE, regValue) == NV_BR04_XVU_G2_PRIV_XP_LCTRL_2_DATA_RATE_SUPPORTED_REMOTE_5P0_2P5)
    {
        // Get the current pcie speed info
        regValue = pData[NV_BR04_XVU_LINK_CTRLSTAT / sizeof(*pData)];
        regValue = REF_VAL(NV_BR04_XVU_LINK_CTRLSTAT_LINK_SPEED, regValue);

        // if already at target speed then quit
        if (regValue != NV_BR04_XVU_LINK_CTRLSTAT_LINK_SPEED_5P0G)
        {
            //
            // Set the UP to Gen2 and allow all downstream GPUs to negotiate their own
            // upstream Gen2 speed.
            //

            regValue = pData[NV_BR04_XVU_LINK_CTRLSTAT2 / sizeof(*pData)];
            regValue = FLD_SET_DRF(_BR04, _XVU_LINK_CTRLSTAT2, _TARGET_LINK_SPEED, _5P0G, regValue);
            pData[NV_BR04_XVU_LINK_CTRLSTAT2 / sizeof(*pData)] = regValue;

            // These values needed for bug 361633.
#           ifndef NV_BR04_XVU_DEV_CTRLSTAT_CORR_ERR_RPT_EN_ENABLED
#           define NV_BR04_XVU_DEV_CTRLSTAT_CORR_ERR_RPT_EN_DISABLED 0
#           define NV_BR04_XVU_DEV_CTRLSTAT_CORR_ERR_RPT_EN_ENABLED 1
#           endif

            // Disable CEs during training per bug 361633
            regValue = pData[NV_BR04_XVU_DEV_CTRLSTAT / sizeof(*pData)];
            if (REF_VAL(NV_BR04_XVU_DEV_CTRLSTAT_CORR_ERR_RPT_EN, regValue)
              != NV_BR04_XVU_DEV_CTRLSTAT_CORR_ERR_RPT_EN_DISABLED)
            {
                regValue = FLD_SET_DRF(_BR04, _XVU_DEV_CTRLSTAT, _CORR_ERR_RPT_EN, _DISABLED, regValue);
                pData[NV_BR04_XVU_DEV_CTRLSTAT / sizeof(*pData)] = regValue;
                enableCorrErrors = NV_TRUE;
            }

            //
            // Trigger speed change on UP
            //

            // This #define should be removed when this is available in the hardware ref headers.
            // See NVBug 332731 for more details.
#           ifndef NV_BR04_XVU_G2_PRIV_XP_LCTRL_2_SPEED_CHANGE_ONE
#           define NV_BR04_XVU_G2_PRIV_XP_LCTRL_2_SPEED_CHANGE_ONE                    0x00000001
#           endif

            regValue = pData[NV_BR04_XVU_G2_PRIV_XP_LCTRL_2 / sizeof(*pData)];
            regValue = FLD_SET_DRF(_BR04, _XVU_G2_PRIV_XP_LCTRL_2, _DATA_RATE_SUPPORTED, _5P0_2P5, regValue);
            regValue = FLD_SET_DRF(_BR04, _XVU_G2_PRIV_XP_LCTRL_2, _TARGET_LINK_SPEED, _5P0, regValue);
            pData[NV_BR04_XVU_G2_PRIV_XP_LCTRL_2 / sizeof(*pData)] = regValue;

            regValue = FLD_SET_DRF(_BR04, _XVU_G2_PRIV_XP_LCTRL_2, _SPEED_CHANGE, _ONE, regValue);
            pData[NV_BR04_XVU_G2_PRIV_XP_LCTRL_2 / sizeof(*pData)] = regValue;

            // Per bug 340793, we need to delay at least 5us for the training to complete.
            osDelay(1);

            // Get the vendor ID and device ID
            regValue = pData[NV_BR04_XVU_DEV_ID / sizeof(*pData)];
            if (REF_VAL(NV_BR04_XVU_DEV_ID_VENDOR_ID, regValue) != NV_BR04_XVU_DEV_ID_VENDOR_ID_NVIDIA ||
              (REF_VAL(NV_BR04_XVU_DEV_ID_DEVICE_ID, regValue) | 0xF) != NV_BR04_XVU_DEV_ID_DEVICE_ID_DEFAULT)
            {
                // Ouch.  The BR04 appears to have fallen off the bus.
                NV_PRINTF(LEVEL_ERROR,
                          "*** BR04 has fallen off the bus after we tried to train it to Gen2!\n");
                DBG_BREAKPOINT();

                status = NV_ERR_GENERIC;
                goto Nvidia_BR04_setupFunc_exit;
            }

            // Get the current pcie speed info
            regValue = pData[NV_BR04_XVU_LINK_CTRLSTAT / sizeof(*pData)];
            regValue = REF_VAL(NV_BR04_XVU_LINK_CTRLSTAT_LINK_SPEED, regValue);

            // if already at target speed then quit
            if (regValue == NV_BR04_XVU_LINK_CTRLSTAT_LINK_SPEED_5P0G)
            {
                NV_PRINTF(LEVEL_INFO, "Verified we are at Gen2 speed.\n");
            }
            else
            {
                NV_PRINTF(LEVEL_ERROR, "Failed to train to Gen2 speed.\n");
            }

            if (enableCorrErrors == NV_TRUE)
            {
                regValue = pData[NV_BR04_XVU_DEV_CTRLSTAT / sizeof(*pData)];
                regValue = FLD_SET_DRF(_BR04, _XVU_DEV_CTRLSTAT, _CORR_ERR_RPT_EN, _ENABLED, regValue);
                pData[NV_BR04_XVU_DEV_CTRLSTAT / sizeof(*pData)] = regValue;
            }
        }
        else
        {
            NV_PRINTF(LEVEL_INFO,
                      "Already in Gen2 speed.  No need to transition.\n");
        }
    }
    else
    {
        NV_PRINTF(LEVEL_INFO,
                  "*** Gen2 not supported by other side of link.\n");
    }


    //
    // Turn off L0s and L1 on all BR04 XVU (Upstream Port)
    //

    regValue = pData[NV_BR04_XVU_LINK_CTRLSTAT / sizeof(*pData)];
    regValue2 = FLD_SET_DRF(_BR04, _XVU_LINK_CTRLSTAT, _ASPM_CTRL, _DISABLED, regValue);
    if (regValue2 != regValue)
    {
        pData[NV_BR04_XVU_LINK_CTRLSTAT / sizeof(*pData)] = regValue2;
    }

    //
    // Set charge pump to optimal value on all BR04 XVU (Upstream Port)
    //

    regValue = pData[NV_XPU_PEX_PLL_CTL2 / sizeof(*pData)];
    regValue2 = FLD_SET_DRF(_XPU, _PEX_PLL_CTL2, _PLL_CP_CNTL, _30UA, regValue);
    if (regValue2 != regValue)
    {
        pData[NV_XPU_PEX_PLL_CTL2 / sizeof(*pData)] = regValue2;
    }

    //
    // Set charge pump to optimal value on all BR04 XVDs (Downstream Ports)
    //

    regValue = pData[NV_XPD_PEX_PLL_CTL2 / sizeof(*pData)];
    regValue2 = FLD_SET_DRF(_XPD, _PEX_PLL_CTL2, _PLL_CP_CNTL, _22P5UA, regValue);
    if (regValue2 != regValue)
    {
        pData[NV_XPD_PEX_PLL_CTL2 / sizeof(*pData)] = regValue2;
    }

    // For the upstream port: Set NV_BR04_XVU_G2_PRIV_XP_0_REPLAY_TIMER_LIMIT
    // (Bits 28:19) of register NV_BR04_XVU_G2_PRIV_XP_0 (0xC00) to the value
    // 0x3E9 when the drivers are loading due to the OS booting up or resuming
    // from sleep/hibernation. (WAR for bugs 319189 and 526518)

#   define NV_BR04_XVU_G2_PRIV_XP_0_REPLAY_TIMER_LIMIT_BUG_319189_WAR 0x3E9

    regValue = pData[NV_BR04_XVU_G2_PRIV_XP_0 / sizeof(*pData)];
    if (REF_VAL(NV_BR04_XVU_G2_PRIV_XP_0_REPLAY_TIMER_LIMIT, regValue)
      != NV_BR04_XVU_G2_PRIV_XP_0_REPLAY_TIMER_LIMIT_BUG_319189_WAR)
    {
        regValue = FLD_SET_DRF(_BR04, _XVU_G2_PRIV_XP_0, _REPLAY_TIMER_LIMIT, _BUG_319189_WAR, regValue);
        pData[NV_BR04_XVU_G2_PRIV_XP_0 / sizeof(*pData)] = regValue;

        // For each downstream port: Set NV_BR04_XVD_G2_VEND_XP1_REPLAY_TIMER_LIMIT
        // (Bits 9:0) of register NV_BR04_XVD_G2_VEND_XP1 (0x404) and
        // NV_BR04_XVD_G2_PRIV_XP_CONFIG_GEN2_REPLAY_TIMER_LIMIT (Bits 11:2) of
        // register NV_BR04_XVD_G2_PRIV_XP_CONFIG (0x494) to 0x3E9. (WAR for bugs 319189 and 526518)

#       define NV_BR04_XVD_G2_VEND_XP1_REPLAY_TIMER_LIMIT_BUG_319189_WAR 0x3E9
#       define NV_BR04_XVD_G2_PRIV_XP_CONFIG_GEN2_REPLAY_TIMER_LIMIT_BUG_319189_WAR 0x3E9

        for (i = 0; i < 4; ++i)
        {
            regValue = pDpData[i][NV_BR04_XVD_G2_VEND_XP1 / sizeof(*pDpData[i])];
            regValue = FLD_SET_DRF(_BR04, _XVD_G2_VEND_XP1, _REPLAY_TIMER_LIMIT, _BUG_319189_WAR, regValue);
            pDpData[i][NV_BR04_XVD_G2_VEND_XP1 / sizeof(*pDpData[i])] = regValue;

            regValue = pDpData[i][NV_BR04_XVD_G2_PRIV_XP_CONFIG / sizeof(*pDpData[i])];
            regValue = FLD_SET_DRF(_BR04, _XVD_G2_PRIV_XP_CONFIG_GEN2, _REPLAY_TIMER_LIMIT, _BUG_319189_WAR, regValue);
            pDpData[i][NV_BR04_XVD_G2_PRIV_XP_CONFIG / sizeof(*pDpData[i])] = regValue;
        }
    }

    //
    // Enable ASPM on BR04 A03 Downstream Ports.
    //

    regValue = pData[NV_BR04_XVU_REV_CC / sizeof(*pData)];
    if (DRF_VAL(_BR04_XVU, _REV_CC, _MAJOR_REVISION_ID, regValue) == 0xa &&
        DRF_VAL(_BR04_XVU, _REV_CC, _MINOR_REVISION_ID, regValue) == 0x3)
    {
        for (i = 0; i < 4; ++i)
        {
            regValue = pDpData[i][NV_BR04_XVD_LINK_CTRLSTAT / sizeof(*pDpData[i])];
            regValue2 = FLD_SET_DRF(_BR04, _XVD_LINK_CTRLSTAT, _ASPM_CTRL, _L0S_L1, regValue);
            if (regValue2 != regValue)
            {
                pDpData[i][NV_BR04_XVD_LINK_CTRLSTAT / sizeof(*pDpData[i])] = regValue2;
            }
        }
    }

    // Check if registers already programmed
    if (pData[NV_BR04_XVU_INT_FLOW_CTL / sizeof(*pData)])
    {
        if (!(pData[NV_BR04_XVU_INT_FLOW_CTL / sizeof(*pData)] & 0xff))
        {
            //
            // WAR for bug 779279.  Credits lost on downstream ports during SBIOS POST on some platforms.
            //

            for (i = 0; i < 4; ++i)
            {
                pData[NV_BR04_XVU_INT_FLOW_CTL_UP0_TOO_CPL(i) / sizeof(*pData)] = 0;
                pData[NV_BR04_XVU_INT_FLOW_CTL_UP0_TOO_NP(i) / sizeof(*pData)] = 0;
                pData[NV_BR04_XVU_INT_FLOW_CTL_UP0_TOO_PW(i) / sizeof(*pData)] = 0;
            }

            pData[NV_BR04_XVU_INT_FLOW_CTL / sizeof(*pData)] = 1;

            if (!(pData[NV_BR04_XVU_INT_FLOW_CTL / sizeof(*pData)] & 0xff))
            {
                // The WAR did not work!
                NV_PRINTF(LEVEL_ERROR,
                          "*** BR04 WAR for bug 779279 is not working!\n");
                DBG_BREAKPOINT();
            }
        }
        else
        {
            NV_PRINTF(LEVEL_WARNING,
                      "*** BR04 registers have already been programmed.\n");
        }
        status = NV_OK;
        goto Nvidia_BR04_setupFunc_exit;
    }

    // Check if hidden GPU mode is enabled; if so, set phantom range for P2P
    // transfers
    if (REF_VAL(NV_BR04_XVU_HGPU_CTRL_EN,
                pData[NV_BR04_XVU_HGPU_CTRL / sizeof(*pData)]) ==
        NV_BR04_XVU_HGPU_CTRL_EN_ENABLED)
    {
        pData[NV_BR04_XVU_HGPU_PEER_FB_UPPER_BASE / sizeof(*pData)] =
            NvU64_HI32(HGPU_P2P_PHANTOM_BASE);
        pData[NV_BR04_XVU_HGPU_PEER_FB_LOWER_BASE / sizeof(*pData)] =
            NvU64_LO32(HGPU_P2P_PHANTOM_BASE & 0xffffffff);
    }

    NV_PRINTF(LEVEL_INFO, "*** Setup BR04 registers.\n");

    // determine how many active DPs we have.  See HW bug 325819 for
    // original algorithm description and bug 346133 for new logic.
    for (i = 0; i < 4; i++)
    {
        if (REF_VAL(NV_BR04_XVD_LINK_CTRLSTAT_DLL_LINK_SM,
          pDpData[i][NV_BR04_XVD_LINK_CTRLSTAT / sizeof(*pDpData[i])]) ==
          NV_BR04_XVD_LINK_CTRLSTAT_DLL_LINK_SM_ACTIVE)
        {
            // we detected a presence in the force
            ++numActiveDPs;
        }
    }

    // assert that we have a legal 1-, 2-, 3-, or 4-DP configuration
    // and choose the right array entry
    if (numActiveDPs >= 3)
        CreditSet = 1;
    else
        CreditSet = 0;

    addr = NV_BR04_XVU_INT_FLOW_CTL_DP0_TOO_CPL(0) / sizeof(*pData);
    for (tport = 0; tport < NUM_BR04_PORTS; tport ++)
    {
        for (sport = 0; sport < NUM_BR04_PORTS; sport ++)
        {
            for (type = 0; type < 3; type++)
            {
                pData[addr] =
                    REF_NUM(NV_BR04_XVU_INT_FLOW_CTL_DP0_TOO_CPL_D,
                            PortCreditsData[CreditSet][type][tport][sport]) |
                    REF_NUM(NV_BR04_XVU_INT_FLOW_CTL_DP0_TOO_CPL_H,
                            PortCreditsHeader[CreditSet][type][tport][sport]);
                addr++;
            }
            addr++;
        }
    }



    // Tell BR04 to use values just written
    pData[NV_BR04_XVU_INT_FLOW_CTL / sizeof(*pData)] = 1;

    // ITX Allocation (values based on BR03 values)
    pData[NV_BR04_XVU_ITX_ALLOCATION / sizeof(*pData)] =
        REF_NUM(NV_BR04_XVU_ITX_ALLOCATION_UP0, 6) |
        REF_NUM(NV_BR04_XVU_ITX_ALLOCATION_DP0, 6) |
        REF_NUM(NV_BR04_XVU_ITX_ALLOCATION_DP1, 6) |
        REF_NUM(NV_BR04_XVU_ITX_ALLOCATION_DP2, 6) |
        REF_NUM(NV_BR04_XVU_ITX_ALLOCATION_DP3, 6) |
        REF_NUM(NV_BR04_XVU_ITX_ALLOCATION_MH0, 6);

    // OPPORTUNISTIC_ACK and OPPORTUNISTIC_UPDATE_FC for UP0, DP*
    pData[NV_BR04_XVU_G2_PRIV_XP_0 / sizeof(*pData)] |=
        REF_NUM(NV_BR04_XVU_G2_PRIV_XP_0_OPPORTUNISTIC_ACK, 1) |
        REF_NUM(NV_BR04_XVU_G2_PRIV_XP_0_OPPORTUNISTIC_UPDATE_FC, 1);
    for (i = 0; i < 4; i++)
    {
        pDpData[i][NV_BR04_XVD_G2_VEND_XP / sizeof(*pData)] |=
            REF_NUM(NV_BR04_XVD_G2_VEND_XP_OPPORTUNISTIC_ACK, 1) |
            REF_NUM(NV_BR04_XVD_G2_VEND_XP_OPPORTUNISTIC_UPDATEFC, 1);
    }

    status = NV_OK;

Nvidia_BR04_setupFunc_exit:
    osUnmapKernelSpace((void*) pData, NV_BR04_XVU_CONFIG_SIZE);
    for (i = 0; i < 4; i++)
    {
        osUnmapKernelSpace((void*)(pDpData[i]), NV_BR04_XVD_CONFIG_SIZE);
    }

    return status;
}


//
// Plx_Pex8747_GetBar0 : Returns physical address of chip's BAR0 address.
// Returns NULL on error.
//
static RmPhysAddr
Plx_Pex8747_GetBar0
(
    OBJCL *pCl,
    OBJHWBC *pPlx
)
{
    RmPhysAddr bar0 = 0;

    // Sanity checks
    if ((!pPlx) || (pPlx->bcRes != HWBC_PLX_PEX8747) || (!pPlx->ctrlDev.valid))
    {
        NV_PRINTF(LEVEL_ERROR, "Not a PLX PEX8747!\n");
        return 0;
    }

    //
    // Usually downstream ports are located one bus number higher than upstream
    // port, with device number equal to downstream port number.
    //
    bar0 = osPciReadDword(pPlx->ctrlDev.handle, 0x10);

    // This is just for sanity's sake.  If there is no bar0 it should read back as 0 anyhow.
    if (bar0 == 0xffffffff)
    {
        bar0 = 0;
    }

    if (!bar0)
    {
        NV_PRINTF(LEVEL_ERROR, "Device has no BAR0!\n");
    }

    return bar0;
}

static NV_STATUS
Plx_Pex8747_ChangeUpstreamBusSpeed
(
    OBJHWBC *pPlx,
    OBJCL *pCl,
    NvU32 cmd
)
{
    volatile NvU32 *pData = NULL;    // Register access for upstream port
    RmPhysAddr bar0;
    NvU32 regValue;

    // Sanity checks
    if ((!pPlx) || (pPlx->bcRes != HWBC_PLX_PEX8747) || (!pPlx->ctrlDev.valid))
    {
        NV_PRINTF(LEVEL_ERROR, "Not a PLX PEX8747!\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    bar0 = Plx_Pex8747_GetBar0(pCl, pPlx);
    if (0 == bar0)
    {
        return NV_ERR_GENERIC;
    }

    pData = osMapKernelSpace(bar0, RM_PAGE_SIZE, NV_MEMORY_UNCACHED, NV_PROTECT_READ_WRITE);
    if (!pData)
    {
        return NV_ERR_GENERIC;
    }

    //
    // Get the current PCIe link speed
    //

    regValue = pData[0x78 / sizeof(*pData)];
    regValue = (regValue >> 16) & 0xF;

    //
    // If already at target speed then return success
    //

    if (regValue == cmd)
    {
        osUnmapKernelSpace((void*) pData, RM_PAGE_SIZE);
        NV_PRINTF(LEVEL_INFO,
                  "Already at Gen%u speed.  No need to transition.\n",
                  cmd);
        return NV_OK;
    }

    //
    // Set the target link speed.
    //

    regValue = pData[0x98 / sizeof(*pData)];
    regValue = (regValue & 0xFFFFFFF0) | cmd;
    pData[0x98 / sizeof(*pData)] = regValue;

    //
    // Enable Retrain Link bit on Upstream port
    //

    regValue = pData[0xF70 / sizeof(*pData)];
    regValue = regValue | 0x20000000;
    pData[0xF70 / sizeof(*pData)] = regValue;

    //
    // Trigger speed change by writing the Retrain Link bit.
    //

    regValue = pData[0x78 / sizeof(*pData)];
    regValue = regValue | 0x20;
    pData[0x78 / sizeof(*pData)] = regValue;

    //
    // Wait long enough to ensure the speed change has at least begun.
    //

    osDelay(1);

    //
    // Disable Retrain Link bit on Upstream port
    //

    regValue = pData[0xF70 / sizeof(*pData)];
    regValue = regValue & 0xDFFFFFFF;
    pData[0xF70 / sizeof(*pData)] = regValue;

    //
    // Get the current PCIe link speed
    //

    regValue = pData[0x78 / sizeof(*pData)];
    regValue = (regValue >> 16) & 0xF;
    if (regValue != cmd)
    {
        osUnmapKernelSpace((void*) pData, RM_PAGE_SIZE);
        NV_PRINTF(LEVEL_ERROR, "Failed to train to Gen%u speed.\n", cmd);
        return NV_ERR_GENERIC;
    }

    osUnmapKernelSpace((void*) pData, RM_PAGE_SIZE);
    return NV_OK;
}

//
// Set up registers for PLX PEX8747
//
static NV_STATUS
Plx_Pex8747_setupFunc
(
    OBJHWBC *pPlx,
    OBJCL *pCl
)
{
    volatile NvU32 *pData = NULL;    // Register access for upstream port
    RmPhysAddr bar0;
    NvU32 regValue;

    // Sanity checks
    if ((!pPlx) || (pPlx->bcRes != HWBC_PLX_PEX8747) || (!pPlx->ctrlDev.valid))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    bar0 = Plx_Pex8747_GetBar0(pCl, pPlx);
    if (0 == bar0)
    {
        return NV_ERR_GENERIC;
    }

    pData = osMapKernelSpace(bar0, 0x12000, NV_MEMORY_UNCACHED, NV_PROTECT_READ_WRITE);
    if (!pData)
    {
        return NV_ERR_GENERIC;
    }

    //
    // Get the current PCIe link speed
    //

    regValue = pData[0x78 / sizeof(*pData)];
    regValue = (regValue >> 16) & 0xF;
    if (regValue == 1)
    {
        //
        // Mask electrical idle on all lanes
        //

        regValue = pData[0x204 / sizeof(*pData)];
        regValue = regValue | 0xFFFF;
        pData[0x204 / sizeof(*pData)] = regValue;

        //
        // Set inferred mode
        //

        regValue = pData[0x220 / sizeof(*pData)];
        regValue = regValue | 0x10000;
        pData[0x220 / sizeof(*pData)] = regValue;

        //
        // Switch to Gen3
        //

        Plx_Pex8747_ChangeUpstreamBusSpeed(pPlx, pCl, HWBC_UPSTREAM_BUS_SPEED_GEN3PCIE);
    }

    //
    // Enable L1 in Link Control register of Port 8, if it is supported.
    //
    regValue = pData[0x8074 / sizeof(*pData)];
    if (CL_IS_L1_SUPPORTED(regValue))
    {
        regValue = pData[0x8078 / sizeof(*pData)];
        regValue = regValue | 0x2;
        pData[0x8078 / sizeof(*pData)] = regValue;
    }

    //
    // Enable L1 in Link Control register of Port 9, if it is supported.
    //
    regValue = pData[0x9074 / sizeof(*pData)];
    if (CL_IS_L1_SUPPORTED(regValue))
    {
        regValue = pData[0x9078 / sizeof(*pData)];
        regValue = regValue | 0x2;
        pData[0x9078 / sizeof(*pData)] = regValue;
    }

    //
    // Enable L1 in Link Control register of Port 16, if it is supported.
    //
    regValue = pData[0x10074 / sizeof(*pData)];
    if (CL_IS_L1_SUPPORTED(regValue))
    {
        regValue = pData[0x10078 / sizeof(*pData)];
        regValue = regValue | 0x2;
        pData[0x10078 / sizeof(*pData)] = regValue;
    }

    //
    // Enable L1 in Link Control register of Port 17, if it is supported.
    //
    regValue = pData[0x11074 / sizeof(*pData)];
    if (CL_IS_L1_SUPPORTED(regValue))
    {
        regValue = pData[0x11078 / sizeof(*pData)];
        regValue = regValue | 0x2;
        pData[0x11078 / sizeof(*pData)] = regValue;
    }

    osUnmapKernelSpace((void*) pData, 0x12000);
    return NV_OK;
}

