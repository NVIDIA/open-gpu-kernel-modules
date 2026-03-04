/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __gm107_dev_nv_xve_h__
#define __gm107_dev_nv_xve_h__
#define NV_PCFG                             0x00088FFF:0x00088000 /* RW--D */
#define NV_XVE_ID                            0x00000000 /* R--4R */
#define NV_XVE_ID_VENDOR                           15:0 /* C--VF */
#define NV_XVE_ID_VENDOR_NVIDIA              0x000010DE /* C---V */
#define NV_XVE_DEV_CTRL                                        0x00000004 /* RW-4R */
#define NV_XVE_DEV_CTRL_CMD_IO_SPACE                                   0:0 /* RWIVF */
#define NV_XVE_DEV_CTRL_CMD_IO_SPACE_DISABLED                   0x00000000 /* RWI-V */
#define NV_XVE_DEV_CTRL_CMD_IO_SPACE_ENABLED                    0x00000001 /* RW--V */
#define NV_XVE_DEV_CTRL_CMD_MEMORY_SPACE                               1:1 /* RWIVF */
#define NV_XVE_DEV_CTRL_CMD_MEMORY_SPACE_DISABLED               0x00000000 /* RWI-V */
#define NV_XVE_DEV_CTRL_CMD_MEMORY_SPACE_ENABLED                0x00000001 /* RW--V */
#define NV_XVE_DEV_CTRL_CMD_BUS_MASTER                                 2:2 /* RWIVF */
#define NV_XVE_DEV_CTRL_CMD_BUS_MASTER_DISABLED                 0x00000000 /* RWI-V */
#define NV_XVE_DEV_CTRL_CMD_BUS_MASTER_ENABLED                  0x00000001 /* RW--V */
#define NV_XVE_REV_ID                                 0x00000008 /* R--4R */
#define NV_XVE_REV_ID_CLASS_CODE                            31:8 /* R-CVF */
#define NV_XVE_REV_ID_CLASS_CODE_3D                   0x00030200 /* ----V */
#define NV_XVE_BAR0                                 0x00000010 /* RW-4R */
#define NV_XVE_BAR0_BASE_ADDRESS                         31:24 /* RWIVF */
#define NV_XVE_BAR0_BASE_ADDRESS_INIT               0x00000000 /* RWI-V */
#define NV_XVE_BAR1_LO                                 0x00000014 /* RW-4R */
#define NV_XVE_BAR1_HI                                 0x00000018 /* RW-4R */
#define NV_XVE_BAR2_LO                                 0x0000001C /* RW-4R */
#define NV_XVE_BAR2_HI                                 0x00000020 /* RW-4R */
#define NV_XVE_BAR3                                 0x00000024 /* RW-4R */
#define NV_XVE_SUBSYSTEM                      0x0000002C /* R--4R */
#define NV_XVE_MSI_CTRL                                    0x00000068 /* RW-4R */
#define NV_XVE_MSI_CTRL_MSI                                     16:16 /* RWIVF */
#define NV_XVE_MSI_CTRL_MSI_DISABLE                        0x00000000 /* RWI-V */
#define NV_XVE_MSI_CTRL_MSI_ENABLE                         0x00000001 /* RW--V */
#define NV_XVE_DEVICE_CONTROL_STATUS                                        0x00000080 /* RWI4R */
#define NV_XVE_DEVICE_CONTROL_STATUS_CORR_ERROR_REPORTING_ENABLE                   0:0 /* RWIVF */
#define NV_XVE_DEVICE_CONTROL_STATUS_CORR_ERROR_REPORTING_ENABLE_INIT       0x00000000 /* RWI-V */
#define NV_XVE_DEVICE_CONTROL_STATUS_NON_FATAL_ERROR_REPORTING_ENABLE              1:1 /* RWIVF */
#define NV_XVE_DEVICE_CONTROL_STATUS_NON_FATAL_ERROR_REPORTING_ENABLE_INIT  0x00000000 /* RWI-V */
#define NV_XVE_DEVICE_CONTROL_STATUS_FATAL_ERROR_REPORTING_ENABLE                  2:2 /* RWIVF */
#define NV_XVE_DEVICE_CONTROL_STATUS_FATAL_ERROR_REPORTING_ENABLE_INIT      0x00000000 /* RWI-V */
#define NV_XVE_DEVICE_CONTROL_STATUS_UNSUPP_REQ_REPORTING_ENABLE                   3:3 /* RWIVF */
#define NV_XVE_DEVICE_CONTROL_STATUS_UNSUPP_REQ_REPORTING_ENABLE_INIT       0x00000000 /* RWI-V */
#define NV_XVE_DEVICE_CONTROL_STATUS_ENABLE_RELAXED_ORDERING                       4:4 /* RWIVF */
#define NV_XVE_DEVICE_CONTROL_STATUS_ENABLE_RELAXED_ORDERING_INIT           0x00000001 /* RWI-V */
#define NV_XVE_DEVICE_CONTROL_STATUS_MAX_PAYLOAD_SIZE                              7:5 /* RWIVF */
#define NV_XVE_DEVICE_CONTROL_STATUS_MAX_PAYLOAD_SIZE_INIT                  0x00000000 /* RWI-V */
#define NV_XVE_DEVICE_CONTROL_STATUS_MAX_PAYLOAD_SIZE_128B                  0x00000000 /* R---V */
#define NV_XVE_DEVICE_CONTROL_STATUS_MAX_PAYLOAD_SIZE_256B                  0x00000001 /* R---V */
#define NV_XVE_DEVICE_CONTROL_STATUS_MAX_PAYLOAD_SIZE_512B                  0x00000002 /* R---V */
#define NV_XVE_DEVICE_CONTROL_STATUS_MAX_PAYLOAD_SIZE_1024B                 0x00000003 /* R---V */
#define NV_XVE_DEVICE_CONTROL_STATUS_MAX_PAYLOAD_SIZE_2048B                 0x00000004 /* R---V */
#define NV_XVE_DEVICE_CONTROL_STATUS_MAX_PAYLOAD_SIZE_4096B                 0x00000005 /* R---V */
#define NV_XVE_DEVICE_CONTROL_STATUS_EXTENDED_TAG_FIELD_ENABLE                     8:8 /* RWIVF */
#define NV_XVE_DEVICE_CONTROL_STATUS_EXTENDED_TAG_FIELD_ENABLE_INIT         0x00000001 /* RWI-V */
#define NV_XVE_DEVICE_CONTROL_STATUS_EXTENDED_TAG_FIELD_ENABLE_ENABLED      0x00000001 /* RW--V */
#define NV_XVE_DEVICE_CONTROL_STATUS_PHANTOM_FUNCTIONS_ENABLE                      9:9 /* R-IVF */
#define NV_XVE_DEVICE_CONTROL_STATUS_PHANTOM_FUNCTIONS_ENABLE_INIT          0x00000000 /* R-I-V */
#define NV_XVE_DEVICE_CONTROL_STATUS_AUXILLARY_POWER_PM_ENABLE                   10:10 /* R-IVF */
#define NV_XVE_DEVICE_CONTROL_STATUS_AUXILLARY_POWER_PM_ENABLE_INIT         0x00000000 /* R-I-V */
#define NV_XVE_DEVICE_CONTROL_STATUS_ENABLE_NO_SNOOP                             11:11 /* RWIVF */
#define NV_XVE_DEVICE_CONTROL_STATUS_ENABLE_NO_SNOOP_INIT                   0x00000001 /* RWI-V */
#define NV_XVE_DEVICE_CONTROL_STATUS_MAX_READ_REQUEST_SIZE                       14:12 /* RWIVF */
#define NV_XVE_DEVICE_CONTROL_STATUS_MAX_READ_REQUEST_SIZE_INIT             0x00000002 /* RWI-V */
#define NV_XVE_DEVICE_CONTROL_STATUS_RSVD                                        15:15 /* C--VF */
#define NV_XVE_DEVICE_CONTROL_STATUS_RSVD_INIT                              0x00000000 /* C---V */
#define NV_XVE_DEVICE_CONTROL_STATUS_CORR_ERROR_DETECTED                         16:16 /* RWIVF */
#define NV_XVE_DEVICE_CONTROL_STATUS_CORR_ERROR_DETECTED_INIT               0x00000000 /* R-I-V */
#define NV_XVE_DEVICE_CONTROL_STATUS_CORR_ERROR_DETECTED_CLEAR              0x00000001 /* -W--C */
#define NV_XVE_DEVICE_CONTROL_STATUS_NON_FATAL_ERROR_DETECTED                    17:17 /* RWIVF */
#define NV_XVE_DEVICE_CONTROL_STATUS_NON_FATAL_ERROR_DETECTED_INIT          0x00000000 /* R-I-V */
#define NV_XVE_DEVICE_CONTROL_STATUS_NON_FATAL_ERROR_DETECTED_CLEAR         0x00000001 /* -W--C */
#define NV_XVE_DEVICE_CONTROL_STATUS_FATAL_ERROR_DETECTED                        18:18 /* RWIVF */
#define NV_XVE_DEVICE_CONTROL_STATUS_FATAL_ERROR_DETECTED_INIT              0x00000000 /* R-I-V */
#define NV_XVE_DEVICE_CONTROL_STATUS_FATAL_ERROR_DETECTED_CLEAR             0x00000001 /* -W--C */
#define NV_XVE_DEVICE_CONTROL_STATUS_UNSUPP_REQUEST_DETECTED                     19:19 /* RWIVF */
#define NV_XVE_DEVICE_CONTROL_STATUS_UNSUPP_REQUEST_DETECTED_INIT           0x00000000 /* R-I-V */
#define NV_XVE_DEVICE_CONTROL_STATUS_UNSUPP_REQUEST_DETECTED_CLEAR          0x00000001 /* -W--C */
#define NV_XVE_DEVICE_CONTROL_STATUS_AUX_POWER_DETECTED                          20:20 /* R-IVF */
#define NV_XVE_DEVICE_CONTROL_STATUS_AUX_POWER_DETECTED_INIT                0x00000000 /* R-I-V */
#define NV_XVE_DEVICE_CONTROL_STATUS_TRANSACTIONS_PENDING                        21:21 /* R-IVF */
#define NV_XVE_DEVICE_CONTROL_STATUS_TRANSACTIONS_PENDING_INIT              0x00000000 /* R-I-V */
#define NV_XVE_LINK_CAPABILITIES                                             0x00000084 /* R--4R */
#define NV_XVE_LINK_CONTROL_STATUS                                                              0x00000088 /* RWI4R */
#define NV_XVE_VCCAP_HDR                                 0x00000100 /* R--4R */
#define NV_XVE_VCCAP_HDR_ID                                    15:0 /* C--VF */
#define NV_XVE_VCCAP_HDR_ID_VC                           0x00000002 /* C---V */
#define NV_XVE_VCCAP_HDR_VER                                  19:16 /* C--VF */
#define NV_XVE_VCCAP_HDR_VER_1                           0x00000001 /* C---V */
#define NV_XVE_VCCAP_CTRL0                               0x00000114 /* RW-4R */
#define NV_XVE_VCCAP_CTRL0_MAP                                  7:1 /* RWIVF */
#define NV_XVE_AER_UNCORR_ERR                                    0x00000424 /* RWC4R */
#define NV_XVE_AER_UNCORR_ERR_DLINK_PROTO_ERR                           4:4 /* RWCVF */
#define NV_XVE_AER_UNCORR_ERR_DLINK_PROTO_ERR_ACTIVE             0x00000001 /* R---V */
#define NV_XVE_AER_UNCORR_ERR_DLINK_PROTO_ERR_NOT_ACTIVE         0x00000000 /* R-C-V */
#define NV_XVE_AER_UNCORR_ERR_DLINK_PROTO_ERR_CLEAR              0x00000001 /* -W--C */
#define NV_XVE_AER_UNCORR_ERR_POISONED_TLP                            12:12 /* RWCVF */
#define NV_XVE_AER_UNCORR_ERR_POISONED_TLP_ACTIVE                0x00000001 /* R---V */
#define NV_XVE_AER_UNCORR_ERR_POISONED_TLP_NOT_ACTIVE            0x00000000 /* R-C-V */
#define NV_XVE_AER_UNCORR_ERR_POISONED_TLP_CLEAR                 0x00000001 /* -W--C */
#define NV_XVE_AER_UNCORR_ERR_CPL_TIMEOUT                             14:14 /* RWCVF */
#define NV_XVE_AER_UNCORR_ERR_CPL_TIMEOUT_ACTIVE                 0x00000001 /* R---V */
#define NV_XVE_AER_UNCORR_ERR_CPL_TIMEOUT_NOT_ACTIVE             0x00000000 /* R-C-V */
#define NV_XVE_AER_UNCORR_ERR_CPL_TIMEOUT_CLEAR                  0x00000001 /* -W--C */
#define NV_XVE_AER_UNCORR_ERR_UNEXP_CPL                               16:16 /* RWCVF */
#define NV_XVE_AER_UNCORR_ERR_UNEXP_CPL_ACTIVE                   0x00000001 /* R---V */
#define NV_XVE_AER_UNCORR_ERR_UNEXP_CPL_NOT_ACTIVE               0x00000000 /* R-C-V */
#define NV_XVE_AER_UNCORR_ERR_UNEXP_CPL_CLEAR                    0x00000001 /* -W--C */
#define NV_XVE_AER_UNCORR_ERR_MALFORMED_TLP                           18:18 /* RWCVF */
#define NV_XVE_AER_UNCORR_ERR_MALFORMED_TLP_ACTIVE               0x00000001 /* R---V */
#define NV_XVE_AER_UNCORR_ERR_MALFORMED_TLP_NOT_ACTIVE           0x00000000 /* R-C-V */
#define NV_XVE_AER_UNCORR_ERR_MALFORMED_TLP_CLEAR                0x00000001 /* -W--C */
#define NV_XVE_AER_UNCORR_ERR_UNSUPPORTED_REQ                         20:20 /* RWCVF */
#define NV_XVE_AER_UNCORR_ERR_UNSUPPORTED_REQ_ACTIVE             0x00000001 /* R---V */
#define NV_XVE_AER_UNCORR_ERR_UNSUPPORTED_REQ_NOT_ACTIVE         0x00000000 /* R-C-V */
#define NV_XVE_AER_UNCORR_ERR_UNSUPPORTED_REQ_CLEAR              0x00000001 /* -W--C */
#define NV_XVE_AER_CORR_ERR                                      0x00000430 /* RW-4R */
#define NV_XVE_AER_CORR_ERR_RCV_ERR                                     0:0 /* RWCVF */
#define NV_XVE_AER_CORR_ERR_RCV_ERR_NOT_ACTIVE                   0x00000000 /* R-C-V */
#define NV_XVE_AER_CORR_ERR_RCV_ERR_ACTIVE                       0x00000001 /* R---V */
#define NV_XVE_AER_CORR_ERR_RCV_ERR_CLEAR                        0x00000001 /* -W--C */
#define NV_XVE_AER_CORR_ERR_BAD_TLP                                     6:6 /* RWCVF */
#define NV_XVE_AER_CORR_ERR_BAD_TLP_NOT_ACTIVE                   0x00000000 /* R-C-V */
#define NV_XVE_AER_CORR_ERR_BAD_TLP_ACTIVE                       0x00000001 /* R---V */
#define NV_XVE_AER_CORR_ERR_BAD_TLP_CLEAR                        0x00000001 /* -W--C */
#define NV_XVE_AER_CORR_ERR_BAD_DLLP                                    7:7 /* RWCVF */
#define NV_XVE_AER_CORR_ERR_BAD_DLLP_NOT_ACTIVE                  0x00000000 /* R-C-V */
#define NV_XVE_AER_CORR_ERR_BAD_DLLP_ACTIVE                      0x00000001 /* R---V */
#define NV_XVE_AER_CORR_ERR_BAD_DLLP_CLEAR                       0x00000001 /* -W--C */
#define NV_XVE_AER_CORR_ERR_RPLY_ROLLOVER                               8:8 /* RWCVF */
#define NV_XVE_AER_CORR_ERR_RPLY_ROLLOVER_NOT_ACTIVE             0x00000000 /* R-C-V */
#define NV_XVE_AER_CORR_ERR_RPLY_ROLLOVER_ACTIVE                 0x00000001 /* R---V */
#define NV_XVE_AER_CORR_ERR_RPLY_ROLLOVER_CLEAR                  0x00000001 /* -W--C */
#define NV_XVE_AER_CORR_ERR_RPLY_TIMEOUT                              12:12 /* RWCVF */
#define NV_XVE_AER_CORR_ERR_RPLY_TIMEOUT_NOT_ACTIVE              0x00000000 /* R-C-V */
#define NV_XVE_AER_CORR_ERR_RPLY_TIMEOUT_ACTIVE                  0x00000001 /* R---V */
#define NV_XVE_AER_CORR_ERR_RPLY_TIMEOUT_CLEAR                   0x00000001 /* -W--C */
#define NV_XVE_AER_CORR_ERR_ADVISORY_NONFATAL                         13:13 /* RWCVF */
#define NV_XVE_AER_CORR_ERR_ADVISORY_NONFATAL_NOT_ACTIVE         0x00000000 /* R-C-V */
#define NV_XVE_AER_CORR_ERR_ADVISORY_NONFATAL_ACTIVE             0x00000001 /* R---V */
#define NV_XVE_AER_CORR_ERR_ADVISORY_NONFATAL_CLEAR              0x00000001 /* -W--C */
#define NV_XVE_CYA_2                                             0x00000704 /* RW-4R */
#define NV_XVE_DEVICE_CONTROL_STATUS_2                           0x000000A0 /* RWI4R */
#define NV_XVE_L1_PM_SUBSTATES_CTRL1                             0x00000260 /* RW-4R */
#define NV_XVE_SW_RESET                                          0x00000718 /* RW-4R */
#define NV_XVE_SW_RESET_RESET                                           0:0 /* RWCVF */
#endif // __gm107_dev_nv_xve_h__
