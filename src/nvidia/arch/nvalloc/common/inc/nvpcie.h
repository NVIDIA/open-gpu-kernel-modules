/*
 * SPDX-FileCopyrightText: Copyright (c) 2000-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef NVPCIE_H
#define NVPCIE_H

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
*       Private PCI Express related defines and structures.                 *
*                                                                           *
\***************************************************************************/

#define PCI_VENDOR_ID           0x00
#ifndef PCI_DEVICE_ID
#define PCI_DEVICE_ID           0x02
#endif
#define PCI_BASE_ADDRESS_1      0x14   /* Aperture Base */
#define PCI_BASE_ADDRESS_2      0x18   /* Aperture Base */
#define PCI_CAPABILITY_LIST     0x34
#define PCI_DEVICE_SPECIFIC     0x40

#define NV_PCI_ID                   0x0
#define NV_PCI_ID_VENDOR            15:0
#define NV_PCI_ID_VENDOR_NVIDIA     0x10DE
#define NV_PCI_ID_DEVICE            31:16

#define PCI_MAX_SLOTS           255
#define PCI_MAX_LANE_WIDTH      32

#define PCI_MAX_FUNCTION        8
#define PCI_INVALID_VENDORID    0xFFFF
#define PCI_INVALID_DEVICEID    0xFFFF
#define PCI_INVALID_SUBVENDORID 0xFFFF
#define PCI_INVALID_SUBDEVICEID 0xFFFF

#define PCI_CLASS_BRIDGE_DEV    0x06
#define PCI_SUBCLASS_BR_HOST    0x00
#define PCI_MULTIFUNCTION       0x80

// From PCI Local Bus Specification, Revision 3.0

#define CAP_ID_MASK             0xFF

#define CAP_ID_PMI              0x01
#define CAP_ID_AGP              0x02
#define CAP_ID_VPD              0x03
#define CAP_ID_SLOT_ID          0x04
#define CAP_ID_MSI              0x05
#define CAP_ID_HOT_SWAP         0x06
#define CAP_ID_PCI_X            0x07
#define CAP_ID_HYPER_TRANSPORT  0x08
#define CAP_ID_VENDOR_SPECIFIC  0x09
#define CAP_ID_DEBUG_PORT       0x0A
#define CAP_ID_CRC              0x0B
#define CAP_ID_HOT_PLUG         0x0C
#define CAP_ID_SUBSYSTEM_ID     0x0D
#define CAP_ID_AGP8X            0x0E
#define CAP_ID_SECURE           0x0F
#define CAP_ID_PCI_EXPRESS      0x10
#define CAP_ID_MSI_X            0x11

//
// Extended config space size is 4096 bytes.
//
#define PCI_EXTENDED_CONFIG_SPACE_LENGTH 4096

//
// From PCI Local Bus Specification, Revision 3.0
//     HEADER TYPE0 Definitions - Byte offsets
//
#define PCI_HEADER_TYPE0_VENDOR_ID       0x00
#define PCI_HEADER_TYPE0_DEVICE_ID       0x02
#define PCI_HEADER_TYPE0_COMMAND         0x04
#define PCI_HEADER_TYPE0_STATUS          0x06
#define PCI_HEADER_TYPE0_REVISION_ID     0x08
#define PCI_HEADER_TYPE0_PROGIF          0x09
#define PCI_HEADER_TYPE0_SUBCLASS        0x0A
#define PCI_HEADER_TYPE0_BASECLASS       0x0B
#define PCI_HEADER_TYPE0_CACHE_LINE_SIZE 0x0C
#define PCI_HEADER_TYPE0_LATENCY_TIMER   0x0D
#define PCI_HEADER_TYPE0_HEADER_TYPE     0x0E
#define PCI_HEADER_TYPE0_BIST            0x0F
#define PCI_HEADER_TYPE0_BAR0            0x10
#define PCI_HEADER_TYPE0_BAR1            0x14
#define PCI_HEADER_TYPE0_BAR2            0x18
#define PCI_HEADER_TYPE0_BAR3            0x1C
#define PCI_HEADER_TYPE0_BAR4            0x20
#define PCI_HEADER_TYPE0_BAR5            0x24
#define PCI_HEADER_TYPE0_CBCIS_PTR       0x28
#define PCI_HEADER_TYPE0_SUBSYS_VEN_ID   0x2C
#define PCI_HEADER_TYPE0_SUBSYS_ID       0x2E
#define PCI_HEADER_TYPE0_ROMBAR          0x30
#define PCI_HEADER_TYPE0_CAP_PTR         0x34
#define PCI_HEADER_TYPE0_INT_LINE        0x3C
#define PCI_HEADER_TYPE0_INT_PIN         0x3D
#define PCI_HEADER_TYPE0_MIN_GNT         0x3E
#define PCI_HEADER_TYPE0_MAX_LAT         0x3F

//
// From PCI Express Base Specification Revision 2.0
// HEADER TYPE1 Definitions
#define PCI_HEADER_TYPE1_BRIDGE_CONTROL         0x3E
#define PCI_HEADER_TYPE1_BRIDGE_CONTROL_VGA_EN  0x08

#define PCIE_LINK_CAPABILITIES_2                                                 0x000000A4 /* R--4R */
#define PCIE_LINK_CAPABILITIES_2_RSVD                                                   0:0 /* C--VF */
#define PCIE_LINK_CAPABILITIES_2_RSVD_INIT                                       0x00000000 /* C---V */
#define PCIE_LINK_CAPABILITIES_2_SUPPORTED_LINK_SPEED                                   7:1 /* R-EVF */
#define PCIE_LINK_CAPABILITIES_2_SUPPORTED_LINK_SPEED_GEN1_GEN2_GEN3_GEN4_GEN5   0x0000001F /* R---V */
#define PCIE_LINK_CAPABILITIES_2_SUPPORTED_LINK_SPEED_HIDDEN                     0x00000000 /* R---V */
#define PCIE_LINK_CAPABILITIES_2_SUPPORTED_LINK_SPEED_GEN1                       0x00000001 /* R---V */
#define PCIE_LINK_CAPABILITIES_2_SUPPORTED_LINK_SPEED_GEN1_GEN2                  0x00000003 /* R---V */
#define PCIE_LINK_CAPABILITIES_2_SUPPORTED_LINK_SPEED_GEN1_GEN2_GEN3             0x00000007 /* R-E-V */
#define PCIE_LINK_CAPABILITIES_2_SUPPORTED_LINK_SPEED_GEN1_GEN2_GEN3_GEN4        0x0000000F /* R---V */
#define PCIE_LINK_CAPABILITIES_2_CROSS_LINK_SUPPORT                                     8:8 /* C--VF */
#define PCIE_LINK_CAPABILITIES_2_CROSS_LINK_SUPPORT_DISABLED                     0x00000000 /* C---V */
#define PCIE_LINK_CAPABILITIES_2_RET_PRESENCE_DET_SUPP                                23:23 /* R-EVF */
#define PCIE_LINK_CAPABILITIES_2_RET_PRESENCE_DET_SUPP_UNSET                     0x00000000 /* R-E-V */
#define PCIE_LINK_CAPABILITIES_2_2RET_PRESENCE_DET_SUPP                               24:24 /* R-EVF */
#define PCIE_LINK_CAPABILITIES_2_2RET_PRESENCE_DET_SUPP_UNSET                    0x00000000 /* R-E-V */
#define PCIE_LINK_CAPABILITIES_2_RSVD1                                                31:25 /* C--VF */
#define PCIE_LINK_CAPABILITIES_2_RSVD1_INIT                                      0x00000000 /* C---V */

//
// PCI Express Virtual Peer-to-Peer Approval Definition
//
// These offsets are unused in hardware on existing chips and are reserved on
// future chips. Software has defined a virtual PCI capability that may be
// emulated by hypervisors at these offsets, and this capability is not tied
// to any specific hardware.
//
//
#define NV_PCI_VIRTUAL_P2P_APPROVAL_CAP_0                       0x000000C8
#define NV_PCI_VIRTUAL_P2P_APPROVAL_CAP_0_ID                           7:0
#define NV_PCI_VIRTUAL_P2P_APPROVAL_CAP_0_NEXT                        15:8
#define NV_PCI_VIRTUAL_P2P_APPROVAL_CAP_0_LENGTH                     23:16
#define NV_PCI_VIRTUAL_P2P_APPROVAL_CAP_0_SIG_LO                     31:24
#define NV_PCI_VIRTUAL_P2P_APPROVAL_CAP_1                       0x000000CC
#define NV_PCI_VIRTUAL_P2P_APPROVAL_CAP_1_SIG_HI                      15:0
#define NV_PCI_VIRTUAL_P2P_APPROVAL_CAP_1_VERSION                    18:16
#define NV_PCI_VIRTUAL_P2P_APPROVAL_CAP_1_PEER_CLIQUE_ID             22:19
#define NV_PCI_VIRTUAL_P2P_APPROVAL_CAP_1_RSVD                       31:23

#define NV_PCI_VIRTUAL_P2P_APPROVAL_SIGNATURE                   0x00503250

// Chipset-specific definitions.
// Intel SantaRosa definitions
#define INTEL_2A00_CONFIG_SPACE_BASE        0x60

// Intel Montevina definitions
#define INTEL_2A40_CONFIG_SPACE_BASE        0x60
#define INTEL_2A40_ASLM_CAPABLE_REVID       0x05

// Intel EagleLake definitions
#define INTEL_2E00_CONFIG_SPACE_BASE        0x60

// Intel Bearlake definitions
#define INTEL_29XX_CONFIG_SPACE_BASE        0x60

// Intel BroadWater definitions
#define INTEL_29A0_CONFIG_SPACE_BASE        0x60

// Intel Grantsdale definitions
#define INTEL_25XX_CONFIG_SPACE_BASE        0x48

// Intel Tumwater definitions
#define INTEL_359E_CONFIG_SPACE_BASE        0xCC

// Intel Greencreek definitions
#define INTEL_25E0_CONFIG_SPACE_BASE_ADDRESS    0xE0000000

// Intel Stoakley definitions
#define INTEL_4000_CONFIG_SPACE_BASE_ADDRESS    0xE0000000

// Intel SkullTrail definitions
#define INTEL_4003_CONFIG_SPACE_BASE_ADDRESS_F  0xF0000000
#define INTEL_4003_CONFIG_SPACE_BASE_ADDRESS_E  0xE0000000
#define INTEL_4003_CONFIG_SPACE_BASE_ADDRESS    INTEL_4003_CONFIG_SPACE_BASE_ADDRESS_F
#define INTEL_4003_CONFIG_SPACE_BASE            0x64

// SiS 656
#define SIS_656_CONFIG_SPACE_BASE           0xE0
#define SIS_656_CONFIG_SPACE_BASE_ADDRESS   3:0     // mapped to 31:28

// PCI/PCIE definitions
#define PCI_MAX_CAPS                        20      // max caps to parse
#define PCI_MAX_DEVICES                     32      // max devices on bus
#define PCI_MAX_FUNCTIONS                   8       // max functions for a device
#define PCI_CAP_HEADER_ID                   7:0     // PCI cap header id
#define PCI_CAP_HEADER_NEXT                 15:8    // PCI cap header next
#define PCI_COMMON_CLASS_SUBCLASS           0x0a    // PCI class/subclass (word)
#define PCI_COMMON_CLASS_SUBBASECLASS_HOST  0x0600  // Host bridge (connect PCI to CPU) [00] + Bridge Device [06]
#define PCI_COMMON_CLASS_SUBBASECLASS_P2P   0x0604  // PCI-to-PCI bridge (connects PCI buses) [04] + Bridge Device [06]
#define PCI_COMMON_CLASS_SUBBASECLASS_VGA   0x0300
#define PCI_COMMON_CLASS_SUBBASECLASS_3DCTRL 0x0302
#define PCI_COMMON_CAP_PTR                  0x34    // PCI common cap ptr (byte)
#define PCI_TYPE_1_SECONDARY_BUS_NUMBER     0x19    // PCI type 1 sec bus (byte)
#define PCI_TYPE_1_SUBORDINATE_BUS_NUMBER   0x1a    // PCI type 1 sub bus (byte)
#define PCIE_CAP_HEADER_ID                  15:0    // PCIE cap header id
#define PCIE_CAP_HEADER_ID_INVALID          0xffff
#define PCIE_CAP_HEADER_NEXT                31:20   // PCIE cap header next
#define PCIE_BUS_SHIFT                      20      // PCIE cfg space bus shift
#define PCIE_DEVICE_SHIFT                   15      // PCIE cfg space dev shift
#define PCIE_FUNC_SHIFT                     12      // PCIE cfg space func shift
#define PCIE_CAP_VERSION                    19:16   // PCIE cap version
#define PCIE_CAP_VERSION_2P0                2       // PCIE 2.0 version
#define PCIE_LINK_CNTRL_STATUS_2_OFFSET     0x30    // PCIE Link Control/Status 2 offset
#define PCIE_LINK_STATUS_2                  31:16   // PCIE Link Status 2 Register
#define PCIE_LINK_STATUS_2_DE_EMPHASIS      0:0     // PCIE De-Emphasis Level
#define PCI_COMMON_SUBSYSTEM_VENDOR_ID      0x2c    // PCI subsystem Vendor Id
#define PCI_COMMON_SUBSYSTEM_ID             0x2e    // PCI subsystem Id


// PCI Express Capability ID in the enhanced configuration space
#define PCIE_CAP_ID_ERROR                   0x1     // PCIE Advanced Error Reporting
#define PCIE_CAP_ID_VC                      0x2     // PCIE Virtual Channel (VC)
#define PCIE_CAP_ID_SERIAL                  0x3     // PCIE Device Serial Number
#define PCIE_CAP_ID_POWER                   0x4     // PCIE Power Budgeting
#define PCIE_CAP_ID_L1_PM_SUBSTATES         0x1E    // PCIE L1 PM Substates

// Intel CPU family.
#define INTEL_CPU_FAMILY_06                 0x06
#define INTEL_CPU_FAMILY_15                 0x0f
#define INTEL_CPU_FAMILY_16                 0x10
#define INTEL_CPU_FAMILY_21                 0x15

// Intel CPU Model. Calculated as Model += (extModel << 4).
#define INTEL_CPU_MODEL_2A                  0x2a
#define INTEL_CPU_MODEL_2D                  0x2d
#define INTEL_CPU_MODEL_3A                  0x3a
#define INTEL_CPU_MODEL_3F                  0x3f
// Symbolic defines for each possible virtual channel
enum
{
    RM_PCIE_VIRTUAL_CHANNEL_0   = 0,
    RM_PCIE_VIRTUAL_CHANNEL_1,
    RM_PCIE_VIRTUAL_CHANNEL_2,
    RM_PCIE_VIRTUAL_CHANNEL_3,
    RM_PCIE_VIRTUAL_CHANNEL_4,
    RM_PCIE_VIRTUAL_CHANNEL_5,
    RM_PCIE_VIRTUAL_CHANNEL_6,
    RM_PCIE_VIRTUAL_CHANNEL_7,
    RM_PCIE_VIRTUAL_CHANNEL_INVALID
};

struct OBJCL;
// root port setup functions
NV_STATUS Broadcom_HT2100_setupFunc(OBJGPU *, OBJCL*);

NV_STATUS Intel_RP25XX_setupFunc(OBJGPU *, OBJCL*);
NV_STATUS Intel_RP81XX_setupFunc(OBJGPU *, OBJCL*);
NV_STATUS Intel_RP3C0X_setupFunc(OBJGPU *, OBJCL*);
NV_STATUS Intel_RP2F0X_setupFunc(OBJGPU *, OBJCL*);
NV_STATUS Intel_RP0C0X_setupFunc(OBJGPU *, OBJCL*);
NV_STATUS Intel_Broadwell_setupFunc(OBJGPU *, OBJCL*);
NV_STATUS Intel_Skylake_setupFunc(OBJGPU *, OBJCL*);
NV_STATUS Intel_Skylake_U_Pch_setupFunc(OBJGPU *, OBJCL*);
NV_STATUS Intel_Skylake_H_Pch_setupFunc(OBJGPU *, OBJCL*);
NV_STATUS Intel_Kabylake_Y_setupFunc(OBJGPU *, OBJCL*);

NV_STATUS Nvidia_RPC19_setupFunc(OBJGPU *, OBJCL*);
NV_STATUS Nvidia_RPC51_setupFunc(OBJGPU *, OBJCL*);
NV_STATUS Nvidia_RPC55_setupFunc(OBJGPU *, OBJCL*);

NV_STATUS AMD_RP1480_setupFunc(OBJGPU *, OBJCL*);
NV_STATUS AMD_RP1630_setupFunc(OBJGPU *, OBJCL*);
NV_STATUS AMD_RP1483_setupFunc(OBJGPU *, OBJCL*);

// Determines if the GPU is in a multi-GPU board based on devid checks
NvBool gpuIsMultiGpuBoard(OBJGPU *, NvBool *, NvBool *);

#endif // NVPCIE_H
