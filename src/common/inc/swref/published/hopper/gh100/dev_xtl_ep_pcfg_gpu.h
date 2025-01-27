/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2023 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gh100_dev_xtl_ep_pcfg_gpu_h__
#define __gh100_dev_xtl_ep_pcfg_gpu_h__
#define NV_EP_PCFG_GPU_ID                                                                                      0x00000000           /* R--4R */
#define NV_EP_PCFG_GPU_ID_VENDOR                                                                               15:0                 /* R-EVF */
#define NV_EP_PCFG_GPU_ID_VENDOR_NVIDIA                                                                        0x000010DE           /* R-E-V */
#define NV_EP_PCFG_GPU_CTRL_CMD_AND_STATUS                                                                     0x00000004           /* RW-4R */
#define NV_EP_PCFG_GPU_CTRL_CMD_AND_STATUS_CMD_IO_SPACE                                                        0:0                  /* RWIVF */
#define NV_EP_PCFG_GPU_CTRL_CMD_AND_STATUS_CMD_IO_SPACE_ENABLE                                                 0x00000001           /* RW--V */
#define NV_EP_PCFG_GPU_CTRL_CMD_AND_STATUS_CMD_MEM_SPACE                                                       1:1                  /* RWIVF */
#define NV_EP_PCFG_GPU_CTRL_CMD_AND_STATUS_CMD_MEM_SPACE_DEFAULT                                               0x00000000           /* RWI-V */
#define NV_EP_PCFG_GPU_CTRL_CMD_AND_STATUS_CMD_MEM_SPACE_ENABLE                                                0x00000001           /* RW--V */
#define NV_EP_PCFG_GPU_CTRL_CMD_AND_STATUS_CMD_BUS_MASTER                                                      2:2                  /* RWIVF */
#define NV_EP_PCFG_GPU_CTRL_CMD_AND_STATUS_CMD_BUS_MASTER_DISABLE                                              0x00000000           /* RWI-V */
#define NV_EP_PCFG_GPU_CTRL_CMD_AND_STATUS_CMD_BUS_MASTER_ENABLE                                               0x00000001           /* RW--V */
#define NV_EP_PCFG_GPU_REVISION_ID_AND_CLASSCODE                                                               0x00000008           /* R--4R */
#define NV_EP_PCFG_GPU_REVISION_ID_AND_CLASSCODE_PGM_INTERFACE                                                 15:8                 /* R-IVF */
#define NV_EP_PCFG_GPU_REVISION_ID_AND_CLASSCODE_SUB_CLASSCODE                                                 23:16                /* R-IVF */
#define NV_EP_PCFG_GPU_REVISION_ID_AND_CLASSCODE_BASE_CLASSCODE                                                31:24                /* R-IVF */
#define NV_EP_PCFG_GPU_REVISION_ID_AND_CLASSCODE_BASE_CLASSCODE_3D                                             0x00000003           /* R-I-V */
#define NV_EP_PCFG_GPU_BARREG0                                                                                 0x00000010           /* RW-4R */
#define NV_EP_PCFG_GPU_BARREG0_REG_ADDR_TYPE                                                                   2:1                  /* R-IVF */
#define NV_EP_PCFG_GPU_BARREG0_REG_ADDR_TYPE_32BIT                                                             0x00000000           /* R-I-V */
#define NV_EP_PCFG_GPU_BARREG0_REG_ADDR_TYPE_64BIT                                                             0x00000002           /* R---V */
#define NV_EP_PCFG_GPU_BARREG0_REG_BASE_ADDRESS                                                                31:18                /* RWIVF */
#define NV_EP_PCFG_GPU_BARREG0_REG_BASE_ADDRESS_INIT                                                           0x00000000           /* RWI-V */
#define NV_EP_PCFG_GPU_BARREG5                                                                                 0x00000024           /* RW-4R */
#define NV_EP_PCFG_GPU_SUBSYSTEM_ID                                                                            0x0000002C           /* R--4R */
#define NV_EP_PCFG_GPU_MSI_64_HEADER                                                                           0x00000048           /* RW-4R */
#define NV_EP_PCFG_GPU_MSI_64_HEADER_MSI_ENABLE                                                                16:16                /* RWIVF */
#define NV_EP_PCFG_GPU_DEVICE_CAPABILITIES                                                                     0x00000064           /* R--4R */
#define NV_EP_PCFG_GPU_DEVICE_CAPABILITIES_EXTENDED_TAG_FIELD_SUPPORTED                                        5:5                  /* R-IVF */
#define NV_EP_PCFG_GPU_DEVICE_CAPABILITIES_FUNCTION_LEVEL_RESET_CAPABILITY                                     28:28                /* R-IVF */
#define NV_EP_PCFG_GPU_DEVICE_CAPABILITIES_FUNCTION_LEVEL_RESET_CAPABILITY_NOT_SUPPORTED                       0x00000000           /* R-I-V */
#define NV_EP_PCFG_GPU_DEVICE_CAPABILITIES_FUNCTION_LEVEL_RESET_CAPABILITY_SUPPORTED                           0x00000001           /* R---V */
#define NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS                                                                   0x00000068           /* RW-4R */
#define NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS_ENABLE_RELAXED_ORDERING                                           4:4                  /* RWIVF */
#define NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS_ENABLE_RELAXED_ORDERING_INIT                                      0x00000001           /* RWI-V */
#define NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS_EXTENDED_TAG_FIELD_ENABLE                                         8:8                  /* RWIVF */
#define NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS_EXTENDED_TAG_FIELD_ENABLE_INIT                                    0x00000001           /* RWI-V */
#define NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS_ENABLE_NO_SNOOP                                                   11:11                /* RWIVF */
#define NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS_INITIATE_FN_LVL_RST                                               15:15                /* RWIVF */
#define NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS_CORR_ERROR_DETECTED                                               16:16                /* RWIVF */
#define NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS_NON_FATAL_ERROR_DETECTED                                          17:17                /* RWIVF */
#define NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS_FATAL_ERROR_DETECTED                                              18:18                /* RWIVF */
#define NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS_UNSUPP_REQUEST_DETECTED                                           19:19                /* RWIVF */
#define NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS_TRANSACTIONS_PENDING                                              21:21                /* R-IVF */
#define NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS_2                                                                 0x00000088           /* RW-4R */
#define NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS_2_ATOMIC_OP_REQUESTER_ENABLE                                      6:6                  /* RWIVF */
#define NV_EP_PCFG_GPU_DEVICE_CONTROL_STATUS_2_ATOMIC_OP_REQUESTER_ENABLE_DEFAULT                              0x00000000           /* RWI-V */
#define NV_EP_PCFG_GPU_LINK_CAPABILITIES                                                                       0x0000006C           /* R--4R */
#define NV_EP_PCFG_GPU_LINK_CONTROL_STATUS                                                                     0x00000070           /* RW-4R */
#define NV_EP_PCFG_GPU_LINK_CONTROL_STATUS_CURRENT_LINK_SPEED                                                  19:16                /* R-EVF */
#define NV_EP_PCFG_GPU_MSIX_CAP_HEADER                                                                         0x000000B0           /* RW-4R */
#define NV_EP_PCFG_GPU_MSIX_CAP_HEADER_ENABLE                                                                  31:31                /* RWIVF */
#define NV_EP_PCFG_GPU_MSIX_CAP_HEADER_ENABLE_ENABLED                                                          0x00000001           /* RW--V */
#define NV_EP_PCFG_GPU_PF_RESIZE_BAR_CTRL                                                                      0x0000013C           /* RW-4R */
#define NV_EP_PCFG_GPU_PF_RESIZE_BAR_CTRL_BAR_SIZE                                                             13:8                 /* RWIVF */
#define NV_EP_PCFG_GPU_PF_RESIZE_BAR_CTRL_BAR_SIZE_MIN                                                         0x00000006           /* RW--V */
#define NV_EP_PCFG_GPU_UNCORRECTABLE_ERROR_STATUS                                                              0x000001BC           /* RW-4R */
#define NV_EP_PCFG_GPU_UNCORRECTABLE_ERROR_STATUS_DL_PROTOCOL_ERROR                                            4:4                  /* RWCVF */
#define NV_EP_PCFG_GPU_UNCORRECTABLE_ERROR_STATUS_POISONED_TLP_RCVD                                            12:12                /* RWCVF */
#define NV_EP_PCFG_GPU_UNCORRECTABLE_ERROR_STATUS_COMPLETION_TIMEOUT                                           14:14                /* RWCVF */
#define NV_EP_PCFG_GPU_UNCORRECTABLE_ERROR_STATUS_UNEXPECTED_COMPLETION                                        16:16                /* RWCVF */
#define NV_EP_PCFG_GPU_UNCORRECTABLE_ERROR_STATUS_MALFORMED_TLP                                                18:18                /* RWCVF */
#define NV_EP_PCFG_GPU_UNCORRECTABLE_ERROR_STATUS_UNSUPPORTED_REQUEST_ERROR                                    20:20                /* RWCVF */
#define NV_EP_PCFG_GPU_CORRECTABLE_ERROR_STATUS                                                                0x000001C8           /* RW-4R */
#define NV_EP_PCFG_GPU_CORRECTABLE_ERROR_STATUS_RECEIVER_ERROR                                                 0:0                  /* RWCVF */
#define NV_EP_PCFG_GPU_CORRECTABLE_ERROR_STATUS_BAD_TLP                                                        6:6                  /* RWCVF */
#define NV_EP_PCFG_GPU_CORRECTABLE_ERROR_STATUS_BAD_DLLP                                                       7:7                  /* RWCVF */
#define NV_EP_PCFG_GPU_CORRECTABLE_ERROR_STATUS_REPLAY_NUM_ROLLOVER                                            8:8                  /* RWCVF */
#define NV_EP_PCFG_GPU_CORRECTABLE_ERROR_STATUS_REPLAY_TIMER_TIMEOUT                                           12:12                /* RWCVF */
#define NV_EP_PCFG_GPU_CORRECTABLE_ERROR_STATUS_ADVISORY_NON_FATAL_ERROR                                       13:13                /* RWCVF */
#define NV_EP_PCFG_GPU_SRIOV_INIT_TOT_VF                                                                       0x0000025C           /* R--4R */
#define NV_EP_PCFG_GPU_SRIOV_INIT_TOT_VF_TOTAL_VFS                                                             31:16                /* R-EVF */
#define NV_EP_PCFG_GPU_SRIOV_FIRST_VF_STRIDE                                                                   0x00000264           /* R--4R */
#define NV_EP_PCFG_GPU_SRIOV_FIRST_VF_STRIDE_FIRST_VF_OFFSET                                                   15:0                 /* R-IVF */
#define NV_EP_PCFG_GPU_VF_BAR0                                                                                 0x00000274           /* RW-4R */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC                                                                          0x000002B4           /* R--4R */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_FAULT_FUSE_POD                                                           0:0                  /* R-CVF */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_FAULT_FUSE_SCPM                                                          1:1                  /* R-CVF */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_FAULT_FSP_SCPM                                                           2:2                  /* R-CVF */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_FAULT_SEC2_SCPM                                                          3:3                  /* R-CVF */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_FAULT_FSP_DCLS                                                           4:4                  /* R-CVF */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_FAULT_SEC2_DCLS                                                          5:5                  /* R-CVF */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_FAULT_GSP_DCLS                                                           6:6                  /* R-CVF */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_FAULT_PMU_DCLS                                                           7:7                  /* R-CVF */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_FAULT_SEQ_TOO_BIG                                                        8:8                  /* R-CVF */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_FAULT_PRE_IFF_CRC                                                        9:9                  /* R-CVF */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_FAULT_POST_IFF_CRC                                                       10:10                /* R-CVF */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_FAULT_ECC                                                                11:11                /* R-CVF */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_FAULT_CMD                                                                12:12                /* R-CVF */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_FAULT_PRI                                                                13:13                /* R-CVF */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_FAULT_WDG                                                                14:14                /* R-CVF */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_FAULT_BOOTFSM                                                            15:15                /* R-CVF */
#define NV_EP_PCFG_GPU_VSEC_DEBUG_SEC_IFF_POS                                                                  22:16                /* R-CVF */
#define NV_EP_PCFG_GPU_L1_PM_SS_CONTROL_1_REGISTER                                                             0x00000298           /* RW-4R */
#define NV_EP_PCFG_GPU_PF_RESIZE_BAR_CAP                                                                       0x00000138           /* R--4R */

#endif // __gh100_dev_xtl_ep_pcfg_gpu_h__
