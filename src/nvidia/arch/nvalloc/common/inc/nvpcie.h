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

#define PCI_IS_VENDORID_VALID(id)   (((id) != 0x0000) && ((id) != 0xFFFF))

#define PCI_CLASS_BRIDGE_DEV    0x06
#define PCI_SUBCLASS_BR_HOST    0x00
#define PCI_MULTIFUNCTION       0x80

// From PCI Local Bus Specification, Revision 3.0
// and  PCI Express Base Specification 6.0
// numbers in comments to right of values indicate
// the referenced section in the PCIE spec


#define CAP_ID_MASK                                 0xFF

#define CAP_ID_NULL                                 0x00        // 7.9.28.1
#define CAP_ID_PMI                                  0x01        // 7.5.2.1
#define CAP_ID_AGP                                  0x02
#define CAP_ID_VPD                                  0x03        // 7.9.18.1
#define CAP_ID_SLOT_ID                              0x04
#define CAP_ID_MSI                                  0x05        // 7.7.1.1
#define CAP_ID_HOT_SWAP                             0x06
#define CAP_ID_PCI_X                                0x07
#define CAP_ID_HYPER_TRANSPORT                      0x08
#define CAP_ID_VENDOR_SPECIFIC                      0x09
#define CAP_ID_DEBUG_PORT                           0x0A
#define CAP_ID_CRC                                  0x0B
#define CAP_ID_HOT_PLUG                             0x0C
#define CAP_ID_SUBSYSTEM_ID                         0x0D        // 7.9.23.1
#define CAP_ID_AGP8X                                0x0E
#define CAP_ID_SECURE                               0x0F
#define CAP_ID_PCI_EXPRESS                          0x10        // 7.5.3.1
#define CAP_ID_MSI_X                                0x11        // 7.7.2.1
#define CAP_ID_ENHANCED_ALLOCATION                  0x14        // 7.8.5.1
#define CAP_ID_FPB                                  0x15        // 7.8.11.1
#define CAP_ID_AF                                   0x13        // 7.9.21.1

//
// sizes for static PCI capabilities structure
//
#define CAP_NULL_SIZE                               0x04        // 7.9.28
#define CAP_PMI_SIZE                                0x08        // 7.5.2
#define CAP_VPD_SIZE                                0x08        // 7.9.18
#define CAP_PCI_X_SIZE                              0x3C        // 7.5.3
#define CAP_PCI_EXPRESS_SIZE                        0x3C        // 7.5.3
#define CAP_FPB_SIZE                                0x08        // 7.8.11.1
#define CAP_AF_SIZE                                 0x08        // 7.9.21
#define CAP_SUBSYSTEM_ID_SIZE                       0x08        // 7.9.23

// MSI capability size related fields
#define PCI_MSI_CONTROL                             0x02        // 7.7.1.2
#define PCI_MSI_CONTROL_64BIT_CAPABLE               7:7
#define PCI_MSI_CONTROL_64BIT_CAPABLE_FALSE         0
#define PCI_MSI_CONTROL_64BIT_CAPABLE_TRUE          1
#define PCI_MSI_CONTROL_PVM_CAPABLE                 8:8
#define PCI_MSI_CONTROL_PVM_CAPABLE_FALSE           0
#define PCI_MSI_CONTROL_PVM_CAPABLE_TRUE            1
#define PCI_MSI_BASE_SIZE                           0x0C        // 7.7.1
#define PCI_MSI_64BIT_ADDR_CAPABLE_ADJ_SIZE         0x04        // 7.7.1
#define PCI_MSI_PVM_CAPABLE_ADJ_SIZE                0x08        // 7.7.1

// MSI-X capability size related fields
#define PCI_MSI_X_BASE_SIZE                         0x0C        // 7.7.2
#define PCI_MSI_X_CONTROL                           0x02        // 7.7.2.2
#define PCI_MSI_X_CONTROL_TABLE_SIZE                10:0
#define PCI_MSI_X_TABLE_OFFSET_BIR                  0x04        // 7.7.2.3
#define PCI_MSI_X_TABLE_OFFSET                      31:3
#define PCI_MSI_X_PBR_OFFSET_BIR                    0x08        // 7.7.2.4
#define PCI_MSI_X_PBR_OFFSET                        31:3
#define PCI_MSI_X_TABLE_ENTRY_SIZE                  0x10        // 7.7.2
#define PCI_MSI_X_PBR_ENTRY_SIZE                    0x10        // 7.7.2

// Enhanced Allocation Capability size related fields
#define PCI_ENHANCED_ALLOCATION_FIRST_DW            0x00        // 7.8.5.1
#define PCI_ENHANCED_ALLOCATION_FIRST_DW_NUM_ENTRIES    21:16
#define PCI_ENHANCED_ALLOCATION_TYPE_0_BASE_SIZE    0x04        // 7.8.5.1
#define PCI_ENHANCED_ALLOCATION_TYPE_1_BASE_SIZE    0x08        // 7.8.5.2
#define PCI_ENHANCED_ALLOCATION_ENTRY_HEADER        0x00        // 7.8.5.3
#define PCI_ENHANCED_ALLOCATION_ENTRY_HEADER_ENTRY_SIZE 2:0

// PCI Vendor Specific Capability size related fields
#define PCI_VENDOR_SPECIFIC_CAP_HEADER              0x00        // 7.9.4
#define PCI_VENDOR_SPECIFIC_CAP_HEADER_LENGTH       23:16

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
#define PCI_HEADER_TYPE0_HEADER_TYPE_0      0
#define PCI_HEADER_TYPE0_HEADER_TYPE_1      1
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
#define PCIE_CAPABILITY_BASE                0x100   // 1st PCIE capability.

// PCI Express Capability ID in the enhanced configuration space
#define PCIE_CAP_ID_NULL                                  0x00        // 7.9.28.1
#define PCIE_CAP_ID_ERROR                                 0x01        // 7.8.4.1
#define PCIE_CAP_ID_VC                                    0x02        // 7.9.1.1
#define PCIE_CAP_ID_SERIAL                                0x03        // 7.9.3.1
#define PCIE_CAP_ID_POWER                                 0x04        // 7.8.1.1
#define PCIE_CAP_ID_ROOT_COMPLEX                          0x05        // 7.9.8.1
#define PCIE_CAP_ID_ROOT_COMPLEX_INTERNAL_LINK_CTRL       0x06        // 7.9.9.1
#define PCIE_CAP_ID_ROOT_COMPLEX_EVENT_COLLECTOR_ENDPOINT 0x07        // 7.9.10.1
#define PCIE_CAP_ID_PCIE_CAP_ID_MFVC                      0x08        // 7.9.2.1
#define PCIE_CAP_ID_RCRB                                  0x0A        // 7.9.7.1
#define PCIE_CAP_ID_ACS                                   0x0D        // 7.7.11.1
#define PCIE_CAP_ID_ARI                                   0x0E        // 7.8.8.1
#define PCIE_CAP_ID_MULTICAST                             0x12        // 7.9.11.1
#define PCIE_CAP_ID_RESIZABLE_BAR                         0x15        // 7.8.6.1
#define PCIE_CAP_ID_DYNAMIC_POWER_ALLOCATION              0x16        // 7.9.12.1
#define PCIE_CAP_ID_TPH                                   0x17        // 7.9.13.1
#define PCIE_CAP_ID_LATENCY_TOLERANCE                     0x18        // 7.8.2.1
#define PCIE_CAP_ID_SECONDARY_PCIE_CAPABILITY             0x19        // 7.7.3.1
#define PCIE_CAP_ID_PASID                                 0x1B        // 7.8.9.1
#define PCIE_CAP_ID_DPC                                   0x1D        // 7.9.14.1
#define PCIE_CAP_ID_L1_PM_SUBSTATES                       0x1E        // 7.8.3.1
#define PCIE_CAP_ID_PTM                                   0x1F        // 7.9.15.1
#define PCIE_CAP_ID_FRS_QUEUING                           0x21        // 7.8.10.1
#define PCIE_CAP_ID_READINESS_TIME_REPORTING              0x22        // 7.9.16.1
#define PCIE_CAP_ID_VENDOR_SPECIFIC                       0x23        // 7.9.6.1
#define PCIE_CAP_ID_VF_RESIZABLE_BAR                      0x24        // 7.8.7.1
#define PCIE_CAP_ID_DATA_LINK                             0x25        // 7.7.4.1
#define PCIE_CAP_ID_PHYSLAYER_16_GT                       0x26        // 7.7.5.1
#define PCIE_CAP_ID_LANE_MARGINING_AT_RECEVER             0x27        // 7.7.10.1
#define PCIE_CAP_ID_HIERARCHY_ID                          0x28        // 7.9.17.1
#define PCIE_CAP_ID_NPEM                                  0x29        // 7.9.19.1
#define PCIE_CAP_ID_PHYSLAYER_32_GT                       0x2A        // 7.7.6.1
#define PCIE_CAP_ID_ALTERNATE_PROTOCOL                    0x2B        // 7.9.20.1
#define PCIE_CAP_ID_SFI                                   0x2C        // 7.9.22.1
#define PCIE_CAP_ID_SHADOW_FUNCTIONS                      0x2D        // 7.9.25.1
#define PCIE_CAP_ID_DATA_OBJECT_EXCHANGE                  0x2E        // 7.9.24.1
#define PCIE_CAP_ID_DEVICE_3                              0x2F        // 7.7.9.1
#define PCIE_CAP_ID_IDE                                   0x30        // 7.9.26.1
#define PCIE_CAP_ID_PHYSLAYER_64_GT                       0x31        // 7.7.7.1
#define PCIE_CAP_ID_FLT_LOGGING                           0x32        // 7.7.8.1
#define PCIE_CAP_ID_FLIT_PERF_MEASURMENT                  0x33        // 7.8.12.1
#define PCIE_CAP_ID_FLIT_ERROR_INJECTION                  0x34        // 7.8.13.1

// static sized structure sizes
#define PCIE_CAP_HEADER_SIZE                                0x04        // 7.6.3
#define PCIE_CAP_NULL_SIZE                                  0x04        // 7.9.28
#define PCIE_CAP_ERROR_SIZE                                 0x48        // 7.8.4
#define PCIE_CAP_POWER_SIZE                                 0x10        // 7.8.1
#define PCIE_CAP_ROOT_COMPLEX_INTERNAL_LINK_CTRL_SIZE       0x0C        // 7.9.9
#define PCIE_CAP_ROOT_COMPLEX_EVENT_COLLECTOR_ENDPOINT_SIZE 0x0C        // 7.9.10
#define PCIE_CAP_SECONDARY_PCIE_SIZE                        0x4C        // 7.7.3.1
#define PCIE_CAP_DATA_LINK_SIZE                             0x0C        // 7.7.4
#define PCIE_CAP_PHYSLAYER_16_GT_SIZE                       0x40        // 7.7.5
#define PCIE_CAP_PHYSLAYER_32_GT_SIZE                       0x40        // 7.7.6
#define PCIE_CAP_PHYSLAYER_64_GT_SIZE                       0x20        // 7.7.7
#define PCIE_CAP_FLT_LOGGING_SIZE                           0x3C        // 7.7.8
#define PCIE_CAP_DEVICE_3_SIZE                              0x10        // 7.7.9
#define PCIE_CAP_LANE_MARGINING_AT_RECEVER_SIZE             0x88        // 7.7.10
#define PCIE_CAP_ACS_SIZE                                   0x10        // 7.7.11
#define PCIE_CAP_LATENCY_TOLERANCE_SIZE                     0x08        // 7.8.2
#define PCIE_CAP_L1_PM_SUBSTATE_SIZE                        0x14        // 7.8.3
#define PCIE_CAP_RESIZABLE_BAR_SIZE                         0x34        // 7.8.6
#define PCIE_CAP_VF_RESIZABLE_BAR_SIZE                      0x34        // 7.8.7
#define PCIE_CAP_ARI_SIZE                                   0x08        // 7.8.8
#define PCIE_CAP_PASID_SIZE                                 0x08        // 7.8.9
#define PCIE_CAP_FRS_QUEUING_SIZE                           0x10        // 7.8.10
#define PCIE_CAP_FPB_SIZE                                   0x24        // 7.8.11
#define PCIE_CAP_FLIT_PERF_MEASURMENT_SIZE                  0x24        // 7.8.12
#define PCIE_CAP_FLIT_ERROR_INJECTION_SIZE                  0x24        // 7.8.13
#define PCIE_CAP_DEV_SERIAL_SIZE                            0x0C        // 7.9.3
#define PCIE_CAP_RCRB_SIZE                                  0x14        // 7.9.7
#define PCIE_CAP_MULTICAST_SIZE                             0x30        // 7.9.11
#define PCIE_CAP_DYNAMIC_POWER_ALLOCATION_SIZE              0x30        // 7.9.12
#define PCIE_CAP_DPC_SIZE                                   0x5C        // 7.9.14
#define PCIE_CAP_PTM_SIZE                                   0x0C        // 7.9.15
#define PCIE_CAP_READINESS_TIME_REPORTING_SIZE              0x0C        // 7.9.16
#define PCIE_CAP_HIERARCHY_ID_SIZE                          0x0C        // 7.9.17
#define PCIE_CAP_NPEM_SIZE                                  0x10        // 7.9.19
#define PCIE_CAP_ALTERNATE_PROTOCOL_SIZE                    0x14        // 7.9.20
#define PCIE_CAP_SFI_SIZE                                   0x14        // 7.9.22
#define PCIE_CAP_DATA_OBJECT_EXCHANGE_SIZE                  0x18        // 7.9.24
#define PCIE_CAP_SHADOW_FUNCTIONS_SIZE                      0x1C        // 7.9.25
#define PCIE_CAP_IDE_SIZE                                   0x34        // 7.9.26

// Virtual Channel Capability size related fields
#define PCIE_VC_REGISTER_1                                  0x04        // 7.9.1.2
#define PCIE_VC_REGISTER_1_EXTENDED_VC_COUNT                2:0
#define PCIE_VIRTUAL_CHANNELS_BASE_SIZE                     0x18        // 7.9.1
#define PCIE_VIRTUAL_CHANNELS_EXTENDED_VC_ENTRY_SIZE        0x10        // 7.9.1

// Multi Function Virtual Channel Capability size related fields
#define PCIE_MFVC_REGISTER_1                                0x04        // 7.9.2.2
#define PCIE_MFVC_REGISTER_1_EXTENDED_VC_COUNT              2:0
#define PCIE_PCIE_CAP_ID_MFVC_BASE_SIZE                     0x18        // 7.9.2
#define PCIE_PCIE_CAP_ID_MFVC_EXTENDED_VC_ENTRY_SIZE        0x10        // 7.9.2

// Vendor Specific Capability size related fields
#define PCIE_VENDOR_SPECIFIC_HEADER_1                       0x04        // 7.9.6.2
#define PCIE_VENDOR_SPECIFIC_HEADER_1_LENGTH                31:20

// Root Complex Capability size related fields
#define PCIE_ROOT_COMPLEX_SELF_DESC_REGISTER                0x04        // 7.9.8.2
#define PCIE_ROOT_COMPLEX_SELF_DESC_REGISTER_NUM_LINK_ENTRIES   15:8
#define PCIE_ROOT_COMPLEX_BASE_SIZE                         0x0C        // 7.9.8
#define PCIE_ROOT_COMPLEX_LINK_ENTRY_SIZE                   0x10        // 7.9.8.3

// TPH capability size related fields
#define PCIE_TPH_REQUESTOR_REGISTER                         0x04        // 7.9.13.2
#define PCIE_TPH_REQUESTOR_REGISTER_ST_TABLE_SIZE           26:16
#define PCIE_TPH_BASE_SIZE                                  0x0C        // 7.9.13
#define PCIE_TPH_ST_ENTRY_SIZE                              0x02        // 7.9.13.4

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

// Diagnostic collection actions.
#define RM_PCIE_ACTION_NOP                          0
#define RM_PCIE_ACTION_COLLECT_CONFIG_SPACE         1
#define RM_PCIE_ACTION_COLLECT_PCI_CAP_STRUCT       2
#define RM_PCIE_ACTION_COLLECT_PCIE_CAP_STRUCT      3
#define RM_PCIE_ACTION_COLLECT_ALL_PCI_CAPS         4
#define RM_PCIE_ACTION_COLLECT_ALL_PCIE_CAPS        5
#define RM_PCIE_ACTION_REPORT_PCI_CAPS_COUNT        6
#define RM_PCIE_ACTION_REPORT_PCIE_CAPS_COUNT       7
#define RM_PCIE_ACTION_EOS                          0xff


// Diagnostic collection device Type ids
#define RM_PCIE_DEVICE_TYPE_NONE                    0xff
#define RM_PCIE_DEVICE_TYPE_GPU                     0
#define RM_PCIE_DEVICE_TYPE_UPSTREAM_BRIDGE         1
#define RM_PCIE_DEVICE_COUNT                        2

// Diagnostic collection capability Type ids
#define RM_PCIE_DC_CAP_TYPE_NONE                    0xff
#define RM_PCIE_DC_CAP_TYPE_PCI                     0
#define RM_PCIE_DC_CAP_TYPE_PCIE                    1
#define RM_PCIE_DC_CAP_TYPE_COUNT                   2

typedef struct _def_bif_dc_diagnostic_collection_command
{
    NvU8              action;
    NvU8              deviceType;
    NvU16             locator;
    NvU16             length;
} CL_PCIE_DC_DIAGNOSTIC_COLLECTION_ENTRY;

typedef struct _def_cl_pcie_dc_capability_map_entry
{
    NvU16   id;
    NvU16   blkOffset;
} CL_PCIE_DC_CAPABILITY_MAP_ENTRY;
typedef struct
{
    NvU16  count;
    CL_PCIE_DC_CAPABILITY_MAP_ENTRY entries[PCI_MAX_CAPS];
} CL_PCIE_DC_CAPABILITY_MAP;

typedef struct OBJCL OBJCL;
typedef struct OBJGPU OBJGPU;
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
NvBool gpuIsMultiGpuBoard(OBJGPU *);

#endif // NVPCIE_H
