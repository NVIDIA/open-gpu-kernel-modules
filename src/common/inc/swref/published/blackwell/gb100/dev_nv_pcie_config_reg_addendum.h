/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __dev_nv_pcie_config_reg_addendum_h__
#define __dev_nv_pcie_config_reg_addendum_h__

/*!--------------------- Config Register Defines---------------------*/
// Partition IDs
enum CONFIG_REGISTERS_PARTITION_ID {
    NV_PCIE_PARTITION_ID_TYPE0_HEADER = 0x0,
    NV_PCIE_PARTITION_ID_CFG_SPACE,
    NV_PCIE_PARTITION_ID_EXT_CFG_SPACE,
    NV_PCIE_PARTITION_ID_INVALID
};

// Base address of each partition
#define NV_PCIE_PARTITION_ID_TYPE0_HEADER_BASE_ADDR                             0x0
#define NV_PCIE_PARTITION_ID_CFG_SPACE_BASE_ADDR                                0x34
#define NV_PCIE_PARTITION_ID_EXT_CFG_SPACE_BASE_ADDR                            0x100

// Masks for partition 1 - PCI Config Space
#define NV_PCIE_PARTITION_ID_CFG_SPACE_CAP_ID_MASK                              0xFF
#define NV_PCIE_PARTITION_ID_CFG_SPACE_NEXT_PTR_MASK                            0xFF00
#define NV_PCIE_PARTITION_ID_CFG_SPACE_MSGBOX_ID_MASK                           0x1000000

// Masks for partition 2 - Extended Config Space
#define NV_PCIE_PARTITION_ID_EXT_CFG_SPACE_CAP_ID_MASK                          0xFFFF
#define NV_PCIE_PARTITION_ID_EXT_CFG_SPACE_NEXT_PTR_MASK                        0xFFF00000
#define NV_PCIE_PARTITION_ID_EXT_CFG_SPACE_VENDOR_ID_MASK                       0xFFFF
#define NV_PCIE_PARTITION_ID_EXT_CFG_SPACE_DVSEC_LEN_MASK                       0xFFF00000

// Capability IDs for ALL group of registers defined in PCIe Spec
#define NV_PCIE_REG_CAP_ID_CFG_PCIE_CAP                                         0x10
#define NV_PCIE_REG_CAP_ID_CFG_MSIX_CAP                                         0x11
#define NV_PCIE_REG_CAP_ID_CFG_POWER_MGMT_CAP                                   0x1
#define NV_PCIE_REG_CAP_ID_CFG_VENDOR_SPECIFIC_CAP                              0x9

#define NV_PCIE_REG_CAP_ID_EXT_CFG_AER_CAP                                      0x1
#define NV_PCIE_REG_CAP_ID_EXT_CFG_DSN_CAP                                      0x3
#define NV_PCIE_REG_CAP_ID_EXT_CFG_POWER_BUDGETING_CAP                          0x4
#define NV_PCIE_REG_CAP_ID_EXT_CFG_ARI_CAP                                      0xe
#define NV_PCIE_REG_CAP_ID_EXT_CFG_ATS_CAP                                      0xf
#define NV_PCIE_REG_CAP_ID_EXT_CFG_SR_IOV_CAP                                   0x10
#define NV_PCIE_REG_CAP_ID_EXT_CFG_PF_RESIZABLE_BAR_CAP                         0x15
#define NV_PCIE_REG_CAP_ID_EXT_CFG_LTR_CAP                                      0x18
#define NV_PCIE_REG_CAP_ID_EXT_CFG_SECONDARY_PCIE_CAP                           0x19
#define NV_PCIE_REG_CAP_ID_EXT_CFG_PASID_CAP                                    0x1b
#define NV_PCIE_REG_CAP_ID_EXT_CFG_L1_PM_SUBSTATE_CAP                           0x1e
#define NV_PCIE_REG_CAP_ID_EXT_CFG_DVSEC_CAP                                    0x23
#define NV_PCIE_REG_CAP_ID_EXT_CFG_VF_RESIZABLE_BAR_CAP                         0x24
#define NV_PCIE_REG_CAP_ID_EXT_CFG_DATA_LINK_FEATURE_CAP                        0x25
#define NV_PCIE_REG_CAP_ID_EXT_CFG_X16P0_GTS_CAP                                0x26
#define NV_PCIE_REG_CAP_ID_EXT_CFG_LANE_MARGINING_AT_RECEIVER_CAP               0x27
#define NV_PCIE_REG_CAP_ID_EXT_CFG_32P0_GTS_CAP                                 0x2a
#define NV_PCIE_REG_CAP_ID_EXT_CFG_ALTERNATE_PROTOCOL_CAP                       0x2b
#define NV_PCIE_REG_CAP_ID_EXT_CFG_DOE_CAP                                      0x2e
#define NV_PCIE_REG_CAP_ID_EXT_CFG_DEVICE3_CAP                                  0x2f
#define NV_PCIE_REG_CAP_ID_EXT_CFG_IDE_CAP                                      0x30
#define NV_PCIE_REG_CAP_ID_EXT_CFG_64P0_GTS_CAP                                 0x31
#define NV_PCIE_REG_CAP_ID_EXT_CFG_FLIT_LOGGING_CAP                             0x32

// Structure to capture the details of each regId in LUT
typedef struct
{
    NvU32 hwDefCapGrpFirstAddr;
    NvU32 hwDefCapGrpLastAddr;
    NvU32 capId;
} PCIE_HW_DEF_REG_INFO;

typedef struct
{
    PCIE_HW_DEF_REG_INFO hwDefRegInfo;
    NvU32 msgboxId;
} PCIE_CFG_REG_INFO;

typedef struct
{
    PCIE_HW_DEF_REG_INFO hwDefRegInfo;
    NvU32 vendorId;
    NvU32 dvsecLen;
} PCIE_EXT_CFG_REG_INFO;

// Invalid IDs
#define NV_PCIE_REG_MSGBOX_ID_INVALID                                           0x0
#define NV_PCIE_REG_VEN_ID_INVALID                                              0x0
#define NV_PCIE_REG_DVSEC_LENGTH_INVALID                                        0x0

static const PCIE_CFG_REG_INFO pcieCfgRegInfo[] = {
    {{NV_PF0_PCI_EXPRESS_CAPABILITIES          , NV_PF0_LINK_CONTROL_AND_STATUS_2          , NV_PCIE_REG_CAP_ID_CFG_PCIE_CAP            }, NV_PCIE_REG_MSGBOX_ID_INVALID},
    {{NV_PF0_MSIX_CAPABILITY_HEADR_AND_CONTROL , NV_PF0_PBA_OFFSET_PBA_BIR                 , NV_PCIE_REG_CAP_ID_CFG_MSIX_CAP            }, NV_PCIE_REG_MSGBOX_ID_INVALID},
    {{NV_PF0_POWER_MANAGEMENT_CAPABILITIES     , NV_PF0_POWER_MANAGEMENT_CONTROL_STATUS    , NV_PCIE_REG_CAP_ID_CFG_POWER_MGMT_CAP      }, NV_PCIE_REG_MSGBOX_ID_INVALID},
    {{NV_PF0_VENDOR_SPECIFIC_HEADER_CAP        , NV_PF0_VENDOR_SPECIFIC_CAP_MSG_BOX_MUTEX  , NV_PCIE_REG_CAP_ID_CFG_VENDOR_SPECIFIC_CAP }, NV_PF0_VENDOR_SPECIFIC_HEADER_CAP_MSG_BOX_ENABLED_DEFAULT},
    {{NV_PF0_VENDOR_SPECIFIC2_HEADER_CAP       , NV_PF0_VENDOR_SPECIFIC2_HEADER_CAP_DWORD5 , NV_PCIE_REG_CAP_ID_CFG_VENDOR_SPECIFIC_CAP }, NV_PF0_VENDOR_SPECIFIC2_HEADER_CAP_MSG_BOX_ENABLED_DEFAULT},
};

static const PCIE_EXT_CFG_REG_INFO pcieExtCfgRegInfo[] = {
    {{NV_PF0_DSN_CAP_HEADER                                            , NV_PF0_DSN_SERIAL_NUMBER_HIGH                            , NV_PCIE_REG_CAP_ID_EXT_CFG_DSN_CAP                        }, NV_PCIE_REG_VEN_ID_INVALID                                               , NV_PCIE_REG_DVSEC_LENGTH_INVALID                                     },
    {{NV_PF0_ADVANCED_ERROR_REPORTING_EXTENDED_CAPABILITY_HEADER       , NV_PF0_TLP_PREFIX_LOG_REGISTER_3                         , NV_PCIE_REG_CAP_ID_EXT_CFG_AER_CAP                        }, NV_PCIE_REG_VEN_ID_INVALID                                               , NV_PCIE_REG_DVSEC_LENGTH_INVALID                                     },
    {{NV_PF0_POWER_BUDGETING_EXTENDED_CAPABILITY_HEADER                , NV_PF0_POWER_BUDGETING_CAPABILITY                        , NV_PCIE_REG_CAP_ID_EXT_CFG_POWER_BUDGETING_CAP            }, NV_PCIE_REG_VEN_ID_INVALID                                               , NV_PCIE_REG_DVSEC_LENGTH_INVALID                                     },
    {{NV_PF0_ARI_EXTENDED_CAPABILITY_HEADER                            , NV_PF0_ARI_CAPABILITY_AND_CONTROL                        , NV_PCIE_REG_CAP_ID_EXT_CFG_ARI_CAP                        }, NV_PCIE_REG_VEN_ID_INVALID                                               , NV_PCIE_REG_DVSEC_LENGTH_INVALID                                     },
    {{NV_PF0_ATS_EXTENDED_CAP_HEADER                                   , NV_PF0_ATS_CAP_AND_CONTROL                               , NV_PCIE_REG_CAP_ID_EXT_CFG_ATS_CAP                        }, NV_PCIE_REG_VEN_ID_INVALID                                               , NV_PCIE_REG_DVSEC_LENGTH_INVALID                                     },
    {{NV_PF0_SR_IOV_EXTENDED_CAPABILITY_HEADER                         , NV_PF0_VF_MIGRATION_STATE_ARRAY_OFFSET                   , NV_PCIE_REG_CAP_ID_EXT_CFG_SR_IOV_CAP                     }, NV_PCIE_REG_VEN_ID_INVALID                                               , NV_PCIE_REG_DVSEC_LENGTH_INVALID                                     },
    {{NV_PF0_PF_RESIZABLE_BAR_EXTENDED_CAPABILITY_HEADER               , NV_PF0_PF_RESIZABLE_BAR_CONTROL                          , NV_PCIE_REG_CAP_ID_EXT_CFG_PF_RESIZABLE_BAR_CAP           }, NV_PCIE_REG_VEN_ID_INVALID                                               , NV_PCIE_REG_DVSEC_LENGTH_INVALID                                     },
    {{NV_PF0_LTR_EXTENDED_CAPABILITY_HEADER                            , NV_PF0_MAX_SNOOP_AND_NO_SNOOP_LATENCY                    , NV_PCIE_REG_CAP_ID_EXT_CFG_LTR_CAP                        }, NV_PCIE_REG_VEN_ID_INVALID                                               , NV_PCIE_REG_DVSEC_LENGTH_INVALID                                     },
    {{NV_PF0_SECONDARY_PCI_EXPRESS_EXTENDED_CAPABILITY_HEADER          , NV_PF0_LANE_EQUALIZATION_CONTROL_0_1(7)                  , NV_PCIE_REG_CAP_ID_EXT_CFG_SECONDARY_PCIE_CAP             }, NV_PCIE_REG_VEN_ID_INVALID                                               , NV_PCIE_REG_DVSEC_LENGTH_INVALID                                     },
    {{NV_PF0_PASID_CAP_HEADER                                          , NV_PF0_PASID_CAP_AND_CONTROL                             , NV_PCIE_REG_CAP_ID_EXT_CFG_PASID_CAP                      }, NV_PCIE_REG_VEN_ID_INVALID                                               , NV_PCIE_REG_DVSEC_LENGTH_INVALID                                     },
    {{NV_PF0_L1_PM_SUBSTATE_EXTENDED_CAPABILITY_HEADER                 , NV_PF0_L1_PM_SUBSTATES_STATUS                            , NV_PCIE_REG_CAP_ID_EXT_CFG_L1_PM_SUBSTATE_CAP             }, NV_PCIE_REG_VEN_ID_INVALID                                               , NV_PCIE_REG_DVSEC_LENGTH_INVALID                                     },
    {{NV_PF0_VF_RESIZABLE_BAR_EXTENDED_CAPABILITY_HEADER               , NV_PF0_VF_RESIZABLE_BAR_CONTROL                          , NV_PCIE_REG_CAP_ID_EXT_CFG_VF_RESIZABLE_BAR_CAP           }, NV_PCIE_REG_VEN_ID_INVALID                                               , NV_PCIE_REG_DVSEC_LENGTH_INVALID                                     },
    {{NV_PF0_DATA_LINK_FEATURE_EXTENDED_CAPABILITY_HEADER              , NV_PF0_DATA_LINK_FEATURE_STATUS_REGISTER                 , NV_PCIE_REG_CAP_ID_EXT_CFG_DATA_LINK_FEATURE_CAP          }, NV_PCIE_REG_VEN_ID_INVALID                                               , NV_PCIE_REG_DVSEC_LENGTH_INVALID                                     },
    {{NV_PF0_X16P0_GTS_CAPABILITY_HEADER                               , NV_PF0_X16P0_GTS_LANE_EQUALIZATION_CONTROL_0_3(3)        , NV_PCIE_REG_CAP_ID_EXT_CFG_X16P0_GTS_CAP                  }, NV_PCIE_REG_VEN_ID_INVALID                                               , NV_PCIE_REG_DVSEC_LENGTH_INVALID                                     },
    {{NV_PF0_LANE_MARGINING_AT_THE_RECEIVER_EXTENDED_CAPABILITY_HEADER , NV_PF0_LANE_MARGINING_CONTROL_STATUS_0(15)               , NV_PCIE_REG_CAP_ID_EXT_CFG_LANE_MARGINING_AT_RECEIVER_CAP }, NV_PCIE_REG_VEN_ID_INVALID                                               , NV_PCIE_REG_DVSEC_LENGTH_INVALID                                     },
    {{NV_PF0_PL_32P0_GTS_CAPABILITY_HEADER                             , NV_PF0_PL_32P0_GTS_LANE_EQUALIZATION_CONTROL_0_3(3)      , NV_PCIE_REG_CAP_ID_EXT_CFG_32P0_GTS_CAP                   }, NV_PCIE_REG_VEN_ID_INVALID                                               , NV_PCIE_REG_DVSEC_LENGTH_INVALID                                     },
    {{NV_PF0_ALTERNATE_PROTOCOL_CAP_HEADER                             , NV_PF0_ALTERNATE_PROTOCOL_SELECTIVE_ENABLE_MASK          , NV_PCIE_REG_CAP_ID_EXT_CFG_ALTERNATE_PROTOCOL_CAP         }, NV_PCIE_REG_VEN_ID_INVALID                                               , NV_PCIE_REG_DVSEC_LENGTH_INVALID                                     },
    {{NV_PF0_DOE_CAP_HEADER                                            , NV_PF0_DOE_READ_DATA_MAILBOX                             , NV_PCIE_REG_CAP_ID_EXT_CFG_DOE_CAP                        }, NV_PCIE_REG_VEN_ID_INVALID                                               , NV_PCIE_REG_DVSEC_LENGTH_INVALID                                     },
    {{NV_PF0_DEVICE3_EXTENDED_CAP_HEADER                               , NV_PF0_DEVICE3_STATUS_3                                  , NV_PCIE_REG_CAP_ID_EXT_CFG_DEVICE3_CAP                    }, NV_PCIE_REG_VEN_ID_INVALID                                               , NV_PCIE_REG_DVSEC_LENGTH_INVALID                                     },
    {{NV_PF0_IDE_CAP_HEADER                                            , NV_PF0_IDE_ADDRESS_ASSOCIATION_3_6(15)                   , NV_PCIE_REG_CAP_ID_EXT_CFG_IDE_CAP                        }, NV_PCIE_REG_VEN_ID_INVALID                                               , NV_PCIE_REG_DVSEC_LENGTH_INVALID                                     },
    {{NV_PF0_PHYSICAL_LAYER_64P0_GTS_EXTENDED_CAPABILITY_HEADER        , NV_PF0_64P0_GTS_LANE_EQUALIZATION_CONTROL_0_3(3)         , NV_PCIE_REG_CAP_ID_EXT_CFG_64P0_GTS_CAP                   }, NV_PCIE_REG_VEN_ID_INVALID                                               , NV_PCIE_REG_DVSEC_LENGTH_INVALID                                     },
    {{NV_PF0_FLIT_LOGGING_EXTENDED_CAPABILITY_HEADER                   , NV_PF0_FBER_MEASUREMENT_STATUS_10                        , NV_PCIE_REG_CAP_ID_EXT_CFG_FLIT_LOGGING_CAP               }, NV_PCIE_REG_VEN_ID_INVALID                                               , NV_PCIE_REG_DVSEC_LENGTH_INVALID                                     },
    {{NV_PF0_DVSEC0_CAP_HEADER                                         , NV_PF0_NV_CXL_DVSEC0_CXL_LOCK_CAP_3                      , NV_PCIE_REG_CAP_ID_EXT_CFG_DVSEC_CAP                      }, NV_PF0_DVSEC0_CXL_HEADER_1_DVSEC_VENDOR_ID_DEFAULT                       , NV_PF0_DVSEC0_CXL_HEADER_1_DVSEC_LENGTH_DEFAULT                      },
    {{NV_PF0_DVSEC8_CAP_HEADER                                         , NV_PF0_DVSEC8_REGISTER_BLOCK_3_REGISTER_OFFSET_HIGH      , NV_PCIE_REG_CAP_ID_EXT_CFG_DVSEC_CAP                      }, NV_PF0_DVSEC8_HEADER_1_DVSEC_VENDOR_ID_DEFAULT                           , NV_PF0_DVSEC8_HEADER_1_DVSEC_LENGTH_DEFAULT                          },
    {{NV_PF0_DVSEC0_CAPABILITY_HEADER                                  , NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1                       , NV_PCIE_REG_CAP_ID_EXT_CFG_DVSEC_CAP                      }, NV_PF0_DESIGNATED_VENDOR_SPECIFIC_0_HEADER_1_NV_DVSEC0_VENDOR_ID_DEFAULT , NV_PF0_DESIGNATED_VENDOR_SPECIFIC_0_HEADER_1_NV_DVSEC0_LENGTH_DEFAULT},
    {{NV_PF0_DVSEC1_CAPABILITY_HEADER                                  , NV_PF0_DESIGNATED_VENDOR_SPECIFIC_1_HEADER_2             , NV_PCIE_REG_CAP_ID_EXT_CFG_DVSEC_CAP                      }, NV_PF0_DESIGNATED_VENDOR_SPECIFIC_1_HEADER_1_NV_DVSEC1_VENDOR_ID_DEFAULT , NV_PF0_DESIGNATED_VENDOR_SPECIFIC_1_HEADER_1_NV_DVSEC1_LENGTH_DEFAULT},
    {{NV_PF0_DVSEC2_CAP_HEADER                                         , NV_PF0_NON_CXL_FUNCTION_REGISTER_8                       , NV_PCIE_REG_CAP_ID_EXT_CFG_DVSEC_CAP                      }, NV_PF0_DVSEC2_HEADER_1_DVSEC_VENDOR_ID_DEFAULT                           , NV_PF0_DVSEC2_HEADER_1_DVSEC_LENGTH_DEFAULT                          },
    {{NV_PF0_DVSEC7_CAP_HEADER                                         , NV_PF0_DVSEC7_FLEX_BUS_PORT_STATUS_2                     , NV_PCIE_REG_CAP_ID_EXT_CFG_DVSEC_CAP                      }, NV_PF0_DVSEC7_HEADER_1_DVSEC_VENDOR_ID_DEFAULT                           , NV_PF0_DVSEC7_HEADER_1_DVSEC_LENGTH_DEFAULT                          },
    {{NV_PF0_DVSEC2_CAPABILITY_HEADER                                  , NV_PF0_DESIGNATED_VENDOR_SPECIFIC_2_HEADER_2_AND_GENERAL , NV_PCIE_REG_CAP_ID_EXT_CFG_DVSEC_CAP                      }, NV_PF0_DESIGNATED_VENDOR_SPECIFIC_2_HEADER_1_NV_DVSEC2_VENDOR_ID_DEFAULT , NV_PF0_DESIGNATED_VENDOR_SPECIFIC_2_HEADER_1_NV_DVSEC2_LENGTH_DEFAULT},
};

/*--------------------------------------------------- Partition IDs Range --------------------------------------------------------------*/
#define NV_PCIE_PARTITION_ID_TYPE0_HEADER_START                                 0x0
#define NV_PCIE_PARTITION_ID_TYPE0_HEADER_END                                   0x3c

#define NV_PCIE_PARTITION_ID_CFG_SPACE_START                                    0x40
#define NV_PCIE_PARTITION_ID_CFG_SPACE_END                                      0xFC

#define NV_PCIE_PARTITION_ID_EXT_CFG_SPACE_START                                0x100
// #define NV_PCIE_PARTITION_ID_EXT_CFG_SPACE_END                                  NV_PF0_DESIGNATED_VENDOR_SPECIFIC_2_HEADER_2_AND_GENERAL
/*--------------------------------------------------- Partition IDs Range --------------------------------------------------------------*/

/*!--------------------- End of Config Register Defines---------------------*/

#endif // __dev_nv_pcie_config_reg_addendum_h__

