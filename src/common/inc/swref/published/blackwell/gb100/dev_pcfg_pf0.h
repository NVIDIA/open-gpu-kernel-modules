/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gb100_dev_pcfg_pf0_h__
#define __gb100_dev_pcfg_pf0_h__

#define NV_PF0_CORRECTABLE_ERROR_STATUS_REPLAY_TIMER_TIMEOUT_STATUS                                  12:12               /* RWCVF */
#define NV_PF0_STATUS_COMMAND_IO_SPACE_ENABLE                                                        0:0                 /* RWIVF */
#define NV_PF0_SUBSYSTEM_ID_AND_VENDOR_ID                                                            0x0000002c          /* R--4R */
#define NV_PF0_LINK_CONTROL_AND_STATUS                                                               0x00000050          /* RW-4R */
#define NV_PF0_REVISION_ID_AND_CLASS_CODE_BASE_CLASS_CODE_3D                                         0                   /*       */
#define NV_PF0_UNCORRECTABLE_ERROR_STATUS_UNSUPPORTED_REQUEST_ERROR_STATUS                           20:20               /* RWCVF */
#define NV_PF0_BASE_ADDRESS_REGISTERS_5                                                              0x00000024          /* RW-4R */
#define NV_PF0_VF_BAR_0                                                                              0x00000324          /* RW-4R */
#define NV_PF0_LINK_CAPABILITIES                                                                     0x0000004c          /* R--4R */
#define NV_PF0_BASE_ADDRESS_REGISTERS_0                                                              0x00000010          /* RW-4R */
#define NV_PF0_DEVICE_CONTROL_AND_STATUS                                                             0x00000048          /* RW-4R */
#define NV_PF0_BASE_ADDRESS_REGISTERS_0_ADDR_TYPE                                                    2:1                 /* R-IVF */
#define NV_PF0_MSIX_CAPABILITY_HEADR_AND_CONTROL                                                    0x0000007c          /* RW-4R */
#define NV_PF0_DEVICE_CONTROL_2_ATOMICOP_REQUESTER_ENABLE                                            6:6                 /* RWIVF */
#define NV_PF0_DEVICE_CONTROL_AND_STATUS_CORRECTABLE_ERROR_DETECTED                                  16:16               /* RWIVF */
#define NV_PF0_DEVICE_CONTROL_AND_STATUS_UNSUPPORTED_REQUEST_DETECTED                                19:19               /* RWIVF */
#define NV_PF0_UNCORRECTABLE_ERROR_STATUS_POISONED_TLP_RECEIVED                                      12:12               /* RWCVF */
#define NV_PF0_DESIGNATED_VENDOR_SPECIFIC_0_HEADER_2_AND_GENERAL_MNOC_INTERFACE_AVAILABLE            19:19               /* R-IVF */
#define NV_PF0_CORRECTABLE_ERROR_STATUS_BAD_DLLP_STATUS                                              7:7                 /* RWCVF */
#define NV_PF0_DEVICE_VENDOR_ID                                                                      0x00000000          /* R--4R */
#define NV_PF0_DEVICE_CAPABILITIES                                                                   0x00000044          /* R--4R */
#define NV_PF0_REVISION_ID_AND_CLASS_CODE                                                            0x00000008          /* R--4R */
#define NV_PF0_UNCORRECTABLE_ERROR_STATUS_UNEXPECTED_COMPLETION_STATUS                               16:16               /* RWCVF */
#define NV_PF0_DESIGNATED_VENDOR_SPECIFIC_0_HEADER_2_AND_GENERAL                                     0x00000af8          /* R--4R */
#define NV_PF0_DEVICE_CONTROL_AND_STATUS_NON_FATAL_ERROR_DETECTED                                    17:17               /* RWIVF */
#define NV_PF0_DEVICE_CAPABILITIES_EXTENDED_TAG_FIELD_SUPPORTED                                      5:5                 /* R-IVF */
#define NV_PF0_L1_PM_SUBSTATES_CONTROL_1                                                             0x00000348          /* RW-4R */
#define NV_PF0_STATUS_COMMAND_BUS_MASTER_ENABLE_ENABLE                                               1                   /*       */
#define NV_PF0_DESIGNATED_VENDOR_SPECIFIC_0_HEADER_2_AND_GENERAL_MNOC_INTERFACE_AVAILABLE_DEFAULT    0x00000001          /* R-I-V */
#define NV_PF0_INITIAL_AND_TOTAL_VFS                                                                 0x0000030c          /* R--4R */
#define NV_PF0_VF_STRIDE_AND_OFFSET                                                                  0x00000314          /* R--4R */
#define NV_PF0_STATUS_COMMAND_BUS_MASTER_ENABLE_DISABLE                                              0                   /*       */
#define NV_PF0_DEVICE_CONTROL_AND_STATUS_ENABLE_NO_SNOOP                                             11:11               /* RWIVF */
#define NV_PF0_UNCORRECTABLE_ERROR_STATUS_COMPLETION_TIMEOUT_STATUS                                  14:14               /* RWCVF */
#define NV_PF0_REVISION_ID_AND_CLASS_CODE_BASE_CLASS_CODE                                            31:24               /* R-IVF */
#define NV_PF0_PF_RESIZABLE_BAR_CONTROL_BAR_SIZE                                                     13:8                /* RWIVF */
#define NV_PF0_DEVICE_CONTROL_AND_STATUS_FATAL_ERROR_DETECTED                                        18:18               /* RWIVF */
#define NV_PF0_CORRECTABLE_ERROR_STATUS_BAD_TLP_STATUS                                               6:6                 /* RWCVF */
#define NV_PF0_STATUS_COMMAND_BUS_MASTER_ENABLE                                                      2:2                 /* RWIVF */
#define NV_PF0_DEVICE_CONTROL_AND_STATUS_BRIDGE_CONFIGURATION_RETRY_ENABLE_INITIATE_FUNCTION_LEVEL_RESET 15:15               /* RWIVF */
#define NV_PF0_UNCORRECTABLE_ERROR_STATUS_MALFORMED_TLP_STATUS                                       18:18               /* RWCVF */
#define NV_PF0_DEVICE_CAPABILITIES_FUNCTION_LEVEL_RESET_CAPABILITY_DEFAULT                           0x00000001          /* R-I-V */
#define NV_PF0_CORRECTABLE_ERROR_STATUS_REPLAY_NUM_ROLLOVER_STATUS                                   8:8                 /* RWCVF */
#define NV_PF0_MSIX_CAPABILITY_HEADR_AND_CONTROL_MSIX_ENABLE                                        31:31               /* RWIVF */
#define NV_PF0_DEVICE_CONTROL_AND_STATUS_ENABLE_RELAXED_ORDERING_DEFAULT                             0x00000001          /* RWI-V */
#define NV_PF0_DEVICE_CONTROL_AND_STATUS_EXTENDED_TAG_FIELD_ENABLE_DEFAULT                           0x00000001          /* RWI-V */
#define NV_PF0_PF_RESIZABLE_BAR_CONTROL                                                              0x000001a0          /* RW-4R */
#define NV_PF0_INITIAL_AND_TOTAL_VFS_TOTAL_VFS                                                       31:16               /* R-EVF */
#define NV_PF0_REVISION_ID_AND_CLASS_CODE_PROGRAMMING_INTERFACE                                      15:8                /* R-IVF */
#define NV_PF0_DEVICE_VENDOR_ID_VENDOR_ID_DEFAULT                                                    0x000010de          /* R-E-V */
#define NV_PF0_DEVICE_CONTROL_AND_STATUS_EXTENDED_TAG_FIELD_ENABLE                                   8:8                 /* RWIVF */
#define NV_PF0_DEVICE_CONTROL_2                                                                      0x00000068          /* RW-4R */
#define NV_PF0_DEVICE_CAPABILITIES_FUNCTION_LEVEL_RESET_CAPABILITY                                   28:28               /* R-IVF */
#define NV_PF0_CORRECTABLE_ERROR_STATUS_RECEIVER_ERROR_STATUS                                        0:0                 /* RWCVF */
#define NV_PF0_REVISION_ID_AND_CLASS_CODE_SUB_CLASS_CODE                                             23:16               /* R-IVF */
#define NV_PF0_DEVICE_VENDOR_ID_VENDOR_ID                                                            15:0                /* R-EVF */
#define NV_PF0_BASE_ADDRESS_REGISTERS_0_ADDR_TYPE_64BIT                                              2                   /*       */
#define NV_PF0_UNCORRECTABLE_ERROR_STATUS                                                            0x0000014c          /* RW-4R */
#define NV_PF0_UNCORRECTABLE_ERROR_STATUS_DATA_LINK_PROTOCOL_ERROR_STATUS                            4:4                 /* RWCVF */
#define NV_PF0_STATUS_COMMAND                                                                        0x00000004          /* RW-4R */
#define NV_PF0_CORRECTABLE_ERROR_STATUS_ADVISORY_NON_FATAL_ERROR_STATUS                              13:13               /* RWCVF */
#define NV_PF0_STATUS_COMMAND_IO_SPACE_ENABLE_ENABLE                                                 1                   /*       */
#define NV_PF0_DEVICE_CONTROL_AND_STATUS_ENABLE_RELAXED_ORDERING                                     4:4                 /* RWIVF */
#define NV_PF0_VF_STRIDE_AND_OFFSET_FIRST_VF_OFFSET                                                  15:0                /* R-IVF */
#define NV_PF0_CORRECTABLE_ERROR_STATUS                                                              0x00000158          /* RW-4R */
#define NV_PF0_DESIGNATED_VENDOR_SPECIFIC_0_HEADER_2_AND_GENERAL_RECOVERY_INDICATION                 17:17               /* R-IVF */
#define NV_PF0_DESIGNATED_VENDOR_SPECIFIC_0_HEADER_1_NV_DVSEC0_VENDOR_ID                             15:0                /* R-IVF */
#define NV_PF0_DESIGNATED_VENDOR_SPECIFIC_0_HEADER_2_AND_GENERAL_RECOVERY_INDICATION_DEFAULT         0x00000000          /* R-I-V */
#define NV_PF0_DESIGNATED_VENDOR_SPECIFIC_0_HEADER_1_NV_DVSEC0_VENDOR_ID_DEFAULT                     0x000010de          /* R-I-V */
#define NV_PF0_DESIGNATED_VENDOR_SPECIFIC_0_HEADER_1                                                 0x00000af4          /* R--4R */
#define NV_PF0_DEVICE_CAPABILITIES_2                                                                 0x00000064          /* R--4R */
#define NV_PF0_DEVICE_CAPABILITIES_2_COMPLETION_TIMEOUT_RANGES_SUPPORTED                             3:0                 /* R-EVF */
#define NV_PF0_DEVICE_CAPABILITIES_2_COMPLETION_TIMEOUT_RANGES_SUPPORTED_DEFAULT                     0x00000003          /* R-E-V */
#define NV_PF0_DEVICE_CAPABILITIES_2_COMPLETION_TIMEOUT_DISABLE_SUPPORTED                            4:4                 /* R-IVF */
#define NV_PF0_DEVICE_CAPABILITIES_2_COMPLETION_TIMEOUT_DISABLE_SUPPORTED_DEFAULT                    0x00000001          /* R-I-V */
#define NV_PF0_DEVICE_CAPABILITIES_2_ARI_FORWARDING_SUPPORTED                                        5:5                 /* R-IVF */
#define NV_PF0_DEVICE_CAPABILITIES_2_ARI_FORWARDING_SUPPORTED_DEFAULT                                0x00000000          /* R-I-V */
#define NV_PF0_DEVICE_CAPABILITIES_2_ATOMICOP_ROUTING_SUPPORTED                                      6:6                 /* R-CVF */
#define NV_PF0_DEVICE_CAPABILITIES_2_ATOMICOP_ROUTING_SUPPORTED_DEFAULT                              0x00000000          /* R-C-V */
#define NV_PF0_DEVICE_CAPABILITIES_2_32_BIT_ATOMICOP_COMPLETER_SUPPORTED                             7:7                 /* R-IVF */
#define NV_PF0_DEVICE_CAPABILITIES_2_32_BIT_ATOMICOP_COMPLETER_SUPPORTED_DEFAULT                     0x00000001          /* R-I-V */
#define NV_PF0_DEVICE_CAPABILITIES_2_64_BIT_ATOMICOP_COMPLETER_SUPPORTED                             8:8                 /* R-IVF */
#define NV_PF0_DEVICE_CAPABILITIES_2_64_BIT_ATOMICOP_COMPLETER_SUPPORTED_DEFAULT                     0x00000001          /* R-I-V */
#define NV_PF0_DEVICE_CAPABILITIES_2_128_BIT_CAS_COMPLETER_SUPPORTED                                 9:9                 /* R-IVF */
#define NV_PF0_DEVICE_CAPABILITIES_2_128_BIT_CAS_COMPLETER_SUPPORTED_DEFAULT                         0x00000000          /* R-I-V */
#define NV_PF0_DEVICE_CAPABILITIES_2_NO_RO_ENABLED_PR_PR_PASSING                                     10:10               /* R-EVF */
#define NV_PF0_DEVICE_CAPABILITIES_2_NO_RO_ENABLED_PR_PR_PASSING_DEFAULT                             0x00000000          /* R-E-V */
#define NV_PF0_DEVICE_CAPABILITIES_2_LTR_MECHANISM_SUPPORTED                                         11:11               /* R-IVF */
#define NV_PF0_DEVICE_CAPABILITIES_2_LTR_MECHANISM_SUPPORTED_DEFAULT                                 0x00000001          /* R-I-V */
#define NV_PF0_DEVICE_CAPABILITIES_2_TPH_COMPLETER_SUPPORTED                                         13:12               /* R-IVF */
#define NV_PF0_DEVICE_CAPABILITIES_2_TPH_COMPLETER_SUPPORTED_DEFAULT                                 0x00000000          /* R-I-V */
#define NV_PF0_DEVICE_CAPABILITIES_2_10_BIT_TAG_COMPLETER_SUPPORTED                                  16:16               /* R-EVF */
#define NV_PF0_DEVICE_CAPABILITIES_2_10_BIT_TAG_COMPLETER_SUPPORTED_DEFAULT                          0x00000001          /* R-E-V */
#define NV_PF0_DEVICE_CAPABILITIES_2_10_BIT_TAG_REQUESTER_SUPPORTED                                  17:17               /* R-EVF */
#define NV_PF0_DEVICE_CAPABILITIES_2_10_BIT_TAG_REQUESTER_SUPPORTED_DEFAULT                          0x00000001          /* R-E-V */
#define NV_PF0_DEVICE_CAPABILITIES_2_OBFF_SUPPORTED                                                  19:18               /* R-EVF */
#define NV_PF0_DEVICE_CAPABILITIES_2_OBFF_SUPPORTED_DEFAULT                                          0x00000001          /* R-E-V */
#define NV_PF0_DEVICE_CAPABILITIES_2_EXTENDED_FMT_FIELD_SUPPORTED                                    20:20               /* R-IVF */
#define NV_PF0_DEVICE_CAPABILITIES_2_EXTENDED_FMT_FIELD_SUPPORTED_DEFAULT                            0x00000001          /* R-I-V */
#define NV_PF0_DEVICE_CAPABILITIES_2_END_END_TLP_PREFIX_SUPPORTED                                    21:21               /* R-EVF */
#define NV_PF0_DEVICE_CAPABILITIES_2_END_END_TLP_PREFIX_SUPPORTED_DEFAULT                            0x00000000          /* R-E-V */
#define NV_PF0_DEVICE_CAPABILITIES_2_MAX_END_END_TLP_PREFIXES                                        23:22               /* R-CVF */
#define NV_PF0_DEVICE_CAPABILITIES_2_MAX_END_END_TLP_PREFIXES_DEFAULT                                0x00000001          /* R-C-V */
#define NV_PF0_DEVICE_CAPABILITIES_2_EMERGENCY_POWER_REDUCTION_SUPPORTED                             25:24               /* R-EVF */
#define NV_PF0_DEVICE_CAPABILITIES_2_EMERGENCY_POWER_REDUCTION_SUPPORTED_DEFAULT                     0x00000002          /* R-E-V */
#define NV_PF0_DEVICE_CAPABILITIES_2_EMERGENCY_POWER_REDUCTION_INITIALIZATION_REQUIRED               26:26               /* R-EVF */
#define NV_PF0_DEVICE_CAPABILITIES_2_EMERGENCY_POWER_REDUCTION_INITIALIZATION_REQUIRED_DEFAULT       0x00000000          /* R-E-V */
#define NV_PF0_DEVICE_CAPABILITIES_2_DMWR_COMPLETER_SUPPORTED                                        28:28               /* R-EVF */
#define NV_PF0_DEVICE_CAPABILITIES_2_DMWR_COMPLETER_SUPPORTED_DEFAULT                                0x00000000          /* R-E-V */
#define NV_PF0_DEVICE_CAPABILITIES_2_DMWR_LENGTHS_SUPPORTED                                          30:29               /* R-EVF */
#define NV_PF0_DEVICE_CAPABILITIES_2_DMWR_LENGTHS_SUPPORTED_DEFAULT                                  0x00000000          /* R-E-V */
#define NV_PF0_DEVICE_CAPABILITIES_2_FRS_SUPPORTED                                                   31:31               /* R-EVF */
#define NV_PF0_DEVICE_CAPABILITIES_2_FRS_SUPPORTED_DEFAULT                                           0x00000001          /* R-E-V */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1                                                           0x00000b04          /* R--4R */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_FUSE_POD                                                  0:0                 /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_FUSE_SCPM                                                 1:1                 /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_IFF_SEQUENCE_TOO_BIG                                      2:2                 /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_PRE_IFF_CRC_CHECK_FAILED                                  3:3                 /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_POST_IFF_CRC_CHECK_FAILED                                 4:4                 /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_IFF_ECC_UNCORRECTABLE_ERROR                               5:5                 /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_IFF_CMD_FORMAT_ERROR                                      6:6                 /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_IFF_PRI_ERROR                                             7:7                 /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_C2C_MISC_LINK_ERROR                                       8:8                 /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_C2C_HBI_LINK_ERROR                                        9:9                 /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_FSP_SCPM                                                  10:10               /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_FSP_DCLS                                                  11:11               /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_FSP_EMP                                                   12:12               /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_FSP_UNCORRECTABLE_ERRORS                                  13:13               /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_FSP_WDT                                                   14:14               /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_SEC2_SCPM                                                 15:15               /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_SEC2_DCLS                                                 16:16               /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_SEC2_WDT                                                  17:17               /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_GSP_DCLS                                                  18:18               /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_GSP_WDT                                                   19:19               /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_PMU_DCLS                                                  20:20               /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_PMU_WDT                                                   21:21               /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_FUSE_POD_2ND                                              22:22               /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_FUSE_SCPM_2ND                                             23:23               /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_IFF_SEQUENCE_TOO_BIG_2ND                                  24:24               /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_PRE_IFF_CRC_CHECK_FAILED_2ND                              25:25               /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_POST_IFF_CRC_CHECK_FAILED_2ND                             26:26               /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_IFF_ECC_UNCORRECTABLE_ERROR_2ND                           27:27               /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_IFF_CMD_FORMAT_ERROR_2ND                                  28:28               /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_IFF_PRI_ERROR_2ND                                         29:29               /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_DEVICE_LOCKDOWN                                           30:30               /* R-IVF */
#define NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1_FUNCTION_LOCKDOWN                                         31:31               /* R-IVF */

#endif // __gb100_dev_pcfg_pf0_h__
