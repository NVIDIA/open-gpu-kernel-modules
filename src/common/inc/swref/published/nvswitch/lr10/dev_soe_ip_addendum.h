/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the Software),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#ifndef __lr10_dev_soe_ip_addendum_h__
#define __lr10_dev_soe_ip_addendum_h__

#define NV_SOE_MUTEX_DEFINES                              \
    NV_MUTEX_ID_SOE_EMEM_ACCESS,                          \

#define NV_SOE_EMEM_ACCESS_PORT_NVSWITCH                           (0)
#define NV_SOE_EMEM_ACCESS_PORT_NVWATCH                            (1)
#define UNUSED_EMEM_ACCESS_PORT_2                                  (2)
#define UNUSED_EMEM_ACCESS_PORT_3                                  (3)

#define NUM_SAW_ENGINE                                              1
#define NUM_NVLINK_ENGINE                                           9

#define NUM_TLC_ENGINE                                              36
#define NUM_NVLIPT_LNK_ENGINE                                       36

#define NV_PRIV_OFFSET_NVLSAW                                        0x00028000

#define NV_GFW_SOE_EXIT_AND_HALT                                     (NV_PRIV_OFFSET_NVLSAW + NV_NVLSAW_SW_SCRATCH_15)
#define NV_GFW_SOE_EXIT_AND_HALT_REQUESTED                           0:0
#define NV_GFW_SOE_EXIT_AND_HALT_REQUESTED_YES                       0x1
#define NV_GFW_SOE_EXIT_AND_HALT_REQUESTED_NO                        0x0
#define NV_GFW_SOE_EXIT_AND_HALT_TIMEOUT                             150000  //150ms

#define NV_GFW_SOE_BOOT                                              (NV_PRIV_OFFSET_NVLSAW + NV_NVLSAW_SW_SCRATCH_3)
#define NV_GFW_SOE_BOOT_PROGRESS                                     7:0
#define NV_GFW_SOE_BOOT_PROGRESS_NOT_STARTED                         0x00000000
#define NV_GFW_SOE_BOOT_PROGRESS_STARTED                             0x00000001
#define NV_GFW_SOE_BOOT_PROGRESS_SECURE_DATA_VERIFY_DONE             0x00000002
#define NV_GFW_SOE_BOOT_PROGRESS_SYSTEM_VALIDITY_DONE                0x00000003
#define NV_GFW_SOE_BOOT_PROGRESS_PRELTSSM_OVERRIDES_DONE             0x00000004
#define NV_GFW_SOE_BOOT_PROGRESS_REPORT_INFOROM_CARVEOUT_DONE        0x00000005
#define NV_GFW_SOE_BOOT_PROGRESS_REPORT_ROMDIR_DONE                  0x00000006
#define NV_GFW_SOE_BOOT_PROGRESS_REPORT_ERASE_LEDGER_DONE            0x00000007
#define NV_GFW_SOE_BOOT_PROGRESS_FW_SECURITY_INIT_DONE               0x00000008
#define NV_GFW_SOE_BOOT_PROGRESS_POSTLTSSM_OVERRIDES_DONE            0x00000009
#define NV_GFW_SOE_BOOT_PROGRESS_IMAGE_VERIFY_DONE                   0x0000000A
#define NV_GFW_SOE_BOOT_PROGRESS_COMPLETED                           0x000000FF

#define NV_GFW_SOE_BOOT_VALIDATION_STATUS                            10:8
#define NV_GFW_SOE_BOOT_VALIDATION_STATUS_UNSUPPORTED                0x00000000
#define NV_GFW_SOE_BOOT_VALIDATION_STATUS_IN_PROGRESS                0x00000001
#define NV_GFW_SOE_BOOT_VALIDATION_STATUS_PASS_NO_TRUST              0x00000002
#define NV_GFW_SOE_BOOT_VALIDATION_STATUS_PASS_TRUSTED               0x00000003
#define NV_GFW_SOE_BOOT_VALIDATION_STATUS_FAIL                       0x00000004
#define NV_GFW_SOE_BOOT_VALIDATION_STATUS_PASS_UNTRUSTED             0x00000005
#define NV_GFW_SOE_BOOT_VALIDATION_STATUS_WARN_NO_TRUST              0x00000006
#define NV_GFW_SOE_BOOT_VALIDATION_STATUS_WARN_TRUSTED               0x00000007

#define NV_GFW_SOE_PROGRESS_CODE                                     (NV_PRIV_OFFSET_NVLSAW + NV_NVLSAW_SW_SCRATCH_13)
#define NV_GFW_SOE_PROGRESS_CODE_VALUE                               3:0
#define NV_GFW_SOE_PROGRESS_CODE_VALUE_NOT_STARTED                   0x00000000
#define NV_GFW_SOE_PROGRESS_CODE_VALUE_STARTED                       0x00000001
#define NV_GFW_SOE_PROGRESS_CODE_VALUE_EXIT                          0x00000002
#define NV_GFW_SOE_PROGRESS_CODE_VALUE_EXIT_SECUREMODE               0x00000003
#define NV_GFW_SOE_PROGRESS_CODE_VALUE_ABORTED                       0x00000004
#define NV_GFW_SOE_PROGRESS_CODE_VALUE_COMPLETED                     0x00000005

#define NV_SOE_RESET_SEQUENCE                                        (NV_PRIV_OFFSET_NVLSAW + NV_NVLSAW_SW_SCRATCH_15)
#define NV_SOE_RESET_SEQUENCE_REQUESTED                              0:0
#define NV_SOE_RESET_SEQUENCE_REQUESTED_YES                          0x00000001
#define NV_SOE_RESET_SEQUENCE_REQUESTED_NO                           0x00000000

#define NUM_NPG_ENGINE                                               9
#define NUM_NPG_BCAST_ENGINE                                         1
#define NUM_NPORT_ENGINE                                             36
#define NUM_NPORT_MULTICAST_BCAST_ENGINE                             1
#define NUM_NXBAR_ENGINE                                             4
#define NUM_NXBAR_BCAST_ENGINE                                       1
#define NUM_TILE_ENGINE                                              16
#define NUM_TILE_MULTICAST_BCAST_ENGINE                              1
#define NUM_NVLW_ENGINE                                              9
#define NUM_BUS_ENGINE                                               1

#define NUM_GIN_ENGINE                                               0
#define NUM_SYS_PRI_HUB                                              0
#define NUM_PRI_MASTER_RS                                            0
#define NUM_MINION_ENGINE                                            NUM_NVLW_ENGINE


#endif // __lr10_dev_soe_ip_addendum_h__
