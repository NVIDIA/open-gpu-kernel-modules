/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES
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

#ifndef DEV_BR04_XVU_H
#define DEV_BR04_XVU_H

#define NV_BR04_XVU                                              0x00000DFF:0x00000000 /* RW--D */
#define NV_BR04_XVU_DEV_ID                                                  0x00000000 /* R--4R */
#define NV_BR04_XVU_DEV_ID_VENDOR_ID                                              15:0 /* C--VF */
#define NV_BR04_XVU_DEV_ID_VENDOR_ID_NVIDIA                                 0x000010DE /* C---V */
#define NV_BR04_XVU_DEV_ID_DEVICE_ID                                             31:16 /* R-IVF */
#define NV_BR04_XVU_DEV_ID_DEVICE_ID_BR04_0                                 0x000005B0 /* R---V */
#define NV_BR04_XVU_DEV_ID_DEVICE_ID_BR04_15                                0x000005BF /* R---V */
#define NV_BR04_XVU_DEV_ID_DEVICE_ID_DEFAULT                                0x000005BF /* R-I-V */
#define NV_BR04_XVU_BUS                                                     0x00000018 /* RW-4R */
#define NV_BR04_XVU_BUS_PRI_NUMBER                                                 7:0 /* RWIUF */
#define NV_BR04_XVU_BUS_PRI_NUMBER_INIT                                     0x00000000 /* RWI-V */
#define NV_BR04_XVU_BUS_SEC_NUMBER                                                15:8 /* RWIUF */
#define NV_BR04_XVU_BUS_SEC_NUMBER_INIT                                     0x00000000 /* RWI-V */
#define NV_BR04_XVU_BUS_SUB_NUMBER                                               23:16 /* RWIUF */
#define NV_BR04_XVU_BUS_SUB_NUMBER_INIT                                     0x00000000 /* RWI-V */
#define NV_BR04_XVU_REV_CC                                                  0x00000008 /* R--4R */
#define NV_BR04_XVU_REV_CC_MINOR_REVISION_ID                                       3:0 /* R--VF */
#define NV_BR04_XVU_REV_CC_MAJOR_REVISION_ID                                       7:4 /* R--VF */
#define NV_BR04_XVU_INT_FLOW_CTL                                            0x00000360 /* RW-4R */
#define NV_BR04_XVU_INT_FLOW_CTL_UP0_TOO_CPL(i)                    (0x000004F0+(i)*16) /* RW-4A */
#define NV_BR04_XVU_INT_FLOW_CTL_DP0_TOO_CPL(i)                    (0x00000370+(i)*16) /* RW-4A */
#define NV_BR04_XVU_INT_FLOW_CTL_DP0_TOO_CPL__SIZE_1                                 6 /*       */
#define NV_BR04_XVU_INT_FLOW_CTL_DP0_TOO_CPL_H                                     6:0 /* RWIUF */
#define NV_BR04_XVU_INT_FLOW_CTL_DP0_TOO_CPL_H_INIT                         0x00000000 /* RWI-V */
#define NV_BR04_XVU_INT_FLOW_CTL_DP0_TOO_CPL_D                                    15:7 /* RWIUF */
#define NV_BR04_XVU_INT_FLOW_CTL_DP0_TOO_CPL_D_INIT                         0x00000000 /* RWI-V */
#define NV_BR04_XVU_INT_FLOW_CTL_UP0_TOO_NP(i)                     (0x000004F4+(i)*16) /* RW-4A */
#define NV_BR04_XVU_INT_FLOW_CTL_UP0_TOO_PW(i)                     (0x000004F8+(i)*16) /* RW-4A */
#define NV_BR04_XVU_HGPU_CTRL                                               0x00000980 /* RW-4R */
#define NV_BR04_XVU_HGPU_CTRL_EN                                                   0:0 /* RWIVF */
#define NV_BR04_XVU_HGPU_CTRL_EN_DISABLED                                   0x00000000 /* RWI-V */
#define NV_BR04_XVU_HGPU_CTRL_EN_ENABLED                                    0x00000001 /* RW--V */
#define NV_BR04_XVU_HGPU_PEER_FB_LOWER_BASE                                 0x00000990 /* RW-4R */
#define NV_BR04_XVU_HGPU_PEER_FB_UPPER_BASE                                 0x00000994 /* RW-4R */
#define NV_BR04_XVU_ITX_ALLOCATION                                          0x000005B0 /* RW-4R */
#define NV_BR04_XVU_ITX_ALLOCATION_DP0                                             3:0 /* RWIUF */
#define NV_BR04_XVU_ITX_ALLOCATION_DP0_INIT                                 0x00000001 /* RWI-V */
#define NV_BR04_XVU_ITX_ALLOCATION_DP1                                             7:4 /* RWIUF */
#define NV_BR04_XVU_ITX_ALLOCATION_DP1_INIT                                 0x00000001 /* RWI-V */
#define NV_BR04_XVU_ITX_ALLOCATION_DP2                                            11:8 /* RWIUF */
#define NV_BR04_XVU_ITX_ALLOCATION_DP2_INIT                                 0x00000001 /* RWI-V */
#define NV_BR04_XVU_ITX_ALLOCATION_DP3                                           15:12 /* RWIUF */
#define NV_BR04_XVU_ITX_ALLOCATION_DP3_INIT                                 0x00000001 /* RWI-V */
#define NV_BR04_XVU_ITX_ALLOCATION_UP0                                           19:16 /* RWIUF */
#define NV_BR04_XVU_ITX_ALLOCATION_UP0_INIT                                 0x00000001 /* RWI-V */
#define NV_BR04_XVU_ITX_ALLOCATION_MH0                                           23:20 /* RWIUF */
#define NV_BR04_XVU_ITX_ALLOCATION_MH0_INIT                                 0x00000001 /* RWI-V */
#define NV_BR04_XVU_MCC_REG_ALIAS                                           0x00000600 /* RW-4R */
#define NV_BR04_XVU_MCC_REG_ALIAS_DONOR_BUS                                        7:0 /* RWIUF */
#define NV_BR04_XVU_MCC_REG_ALIAS_DONOR_BUS_INIT                            0x00000000 /* R-I-V */
#define NV_BR04_XVU_MCC_REG_ALIAS_ACCESS                                           8:8 /* RWIVF */
#define NV_BR04_XVU_MCC_REG_ALIAS_ACCESS_DISABLED                           0x00000000 /* RW--V */
#define NV_BR04_XVU_MCC_REG_ALIAS_ACCESS_ENABLED                            0x00000001 /* RWI-V */
#define NV_BR04_XVU_MCC_REG_ALIAS_ADDR_SELECT                                      9:9 /* RWIVF */
#define NV_BR04_XVU_MCC_REG_ALIAS_ADDR_SELECT_AUTO                          0x00000000 /* RWI-V */
#define NV_BR04_XVU_MCC_REG_ALIAS_ADDR_SELECT_MANUAL                        0x00000001 /* RW--V */
#define NV_BR04_XVU_MCC_REG_OFFSET                                          0x00000604 /* RW-4R */
#define NV_BR04_XVU_BAR_0                                                   0x00000010 /* RW-4R */
#define NV_BR04_XVU_DEV_CTRLSTAT                                            0x00000068 /* RW-4R */
#define NV_BR04_XVU_DEV_CTRLSTAT_CORR_ERR_RPT_EN                                   0:0 /* RWIVF */
#define NV_BR04_XVU_DEV_CTRLSTAT_CORR_ERR_RPT_EN_INIT                       0x00000000 /* RWI-V */
#define NV_BR04_XVU_LINK_CTRLSTAT                                           0x00000070 /* RW-4R */
#define NV_BR04_XVU_LINK_CTRLSTAT_ASPM_CTRL                                        1:0 /* RWIVF */
#define NV_BR04_XVU_LINK_CTRLSTAT_ASPM_CTRL_DISABLED                        0x00000000 /* RWI-V */
#define NV_BR04_XVU_LINK_CTRLSTAT_ASPM_CTRL_L0S                             0x00000001 /* RW--V */
#define NV_BR04_XVU_LINK_CTRLSTAT_ASPM_CTRL_L1                              0x00000002 /* RW--V */
#define NV_BR04_XVU_LINK_CTRLSTAT_ASPM_CTRL_L0S_L1                          0x00000003 /* RW--V */
#define NV_BR04_XVU_LINK_CTRLSTAT_LINK_SPEED                                     19:16 /* R--VF */
#define NV_BR04_XVU_LINK_CTRLSTAT_LINK_SPEED_2P5G                           0x00000001 /* R---V */
#define NV_BR04_XVU_LINK_CTRLSTAT_LINK_SPEED_5P0G                           0x00000002 /* R---V */
#define NV_BR04_XVU_LINK_CTRLSTAT2                                          0x00000090 /* RW-4R */
#define NV_BR04_XVU_LINK_CTRLSTAT2_TARGET_LINK_SPEED                               3:0 /* RWIVF */
#define NV_BR04_XVU_LINK_CTRLSTAT2_TARGET_LINK_SPEED_2P5G                   0x00000001 /* RW--V */
#define NV_BR04_XVU_LINK_CTRLSTAT2_TARGET_LINK_SPEED_5P0G                   0x00000002 /* RWI-V */
#define NV_BR04_XVU_G2_PRIV_XP_0                                            0x00000C00 /* RW-4R */
#define NV_BR04_XVU_G2_PRIV_XP_0_REPLAY_TIMER_LIMIT                              28:19 /* RWIVF */
#define NV_BR04_XVU_G2_PRIV_XP_0_REPLAY_TIMER_LIMIT_INIT                    0x00000000 /* RWI-V */
#define NV_BR04_XVU_G2_PRIV_XP_0_OPPORTUNISTIC_ACK                               29:29 /* RWIVF */
#define NV_BR04_XVU_G2_PRIV_XP_0_OPPORTUNISTIC_ACK_INIT                     0x00000000 /* RWI-V */
#define NV_BR04_XVU_G2_PRIV_XP_0_OPPORTUNISTIC_UPDATE_FC                         30:30 /* RWIVF */
#define NV_BR04_XVU_G2_PRIV_XP_0_OPPORTUNISTIC_UPDATE_FC_INIT               0x00000000 /* RWI-V */
#define NV_BR04_XVU_G2_PRIV_XP_LCTRL_2                                      0x00000C44 /* RW-4R */
#define NV_BR04_XVU_G2_PRIV_XP_LCTRL_2_SPEED_CHANGE                                0:0 /* CWIVF */
#define NV_BR04_XVU_G2_PRIV_XP_LCTRL_2_SPEED_CHANGE_ZERO                    0x00000000 /* CWI-V */
#define NV_BR04_XVU_G2_PRIV_XP_LCTRL_2_CYA_DEEMPHASIS_OVERRIDE                     2:2 /* RWIVF */
#define NV_BR04_XVU_G2_PRIV_XP_LCTRL_2_CYA_DEEMPHASIS_OVERRIDE_DISABLED     0x00000000 /* RWI-V */
#define NV_BR04_XVU_G2_PRIV_XP_LCTRL_2_CYA_DEEMPHASIS_OVERRIDE_ENABLED      0x00000001 /* RW--V */
#define NV_BR04_XVU_G2_PRIV_XP_LCTRL_2_TARGET_LINK_SPEED                           7:4 /* RWIVF */
#define NV_BR04_XVU_G2_PRIV_XP_LCTRL_2_TARGET_LINK_SPEED_2P5                0x00000001 /* RW--V */
#define NV_BR04_XVU_G2_PRIV_XP_LCTRL_2_TARGET_LINK_SPEED_5P0                0x00000002 /* RWI-V */
#define NV_BR04_XVU_G2_PRIV_XP_LCTRL_2_DATA_RATE_SUPPORTED                        11:8 /* RWIVF */
#define NV_BR04_XVU_G2_PRIV_XP_LCTRL_2_DATA_RATE_SUPPORTED_2P5              0x00000001 /* RW--V */
#define NV_BR04_XVU_G2_PRIV_XP_LCTRL_2_DATA_RATE_SUPPORTED_5P0_2P5          0x00000002 /* RWI-V */
#define NV_BR04_XVU_G2_PRIV_XP_LCTRL_2_DATA_RATE_SUPPORTED_REMOTE                15:12 /* R-IVF */
#define NV_BR04_XVU_G2_PRIV_XP_LCTRL_2_DATA_RATE_SUPPORTED_REMOTE_2P5       0x00000001 /* R-I-V */
#define NV_BR04_XVU_G2_PRIV_XP_LCTRL_2_DATA_RATE_SUPPORTED_REMOTE_5P0_2P5   0x00000002 /* R---V */
#define NV_BR04_XVU_BOOT_1                                                  0x00000204 /* R--4R */
#define NV_BR04_XVU_BOOT_1_LINK_SPEED                                              1:1 /* RWIVF */
#define NV_BR04_XVU_BOOT_1_LINK_SPEED_2500                                  0x00000000 /* RWI-V */
#define NV_BR04_XVU_BOOT_1_LINK_SPEED_5000                                  0x00000001 /* RW--V */
#define NV_BR04_XVU_CYA_BIT0                                                0x00000AB0 /* RW-4R */
#define NV_BR04_XVU_CYA_BIT0_RSVD_28                                             28:28 /* RWIVF */
#define NV_BR04_XVU_CYA_BIT0_RSVD_28_INIT                                   0x00000000 /* RWI-V */
#define NV_BR04_XVU_CYA_BIT0_RSVD_29                                             29:29 /* RWIVF */
#define NV_BR04_XVU_CYA_BIT0_RSVD_29_INIT                                   0x00000000 /* RWI-V */
#define NV_BR04_XVU_CYA_BIT0_RSVD_30                                             30:30 /* RWIVF */
#define NV_BR04_XVU_CYA_BIT0_RSVD_30_INIT                                   0x00000000 /* RWI-V */
#define NV_BR04_XVU_CYA_BIT0_RSVD_31                                             31:31 /* RWIVF */
#define NV_BR04_XVU_CYA_BIT0_RSVD_31_INIT                                   0x00000000 /* RWI-V */
#define NV_BR04_XVU_CYA_NIBBLE0                                             0x00000AB4 /* RW-4R */
#define NV_BR04_XVU_CYA_NIBBLE0_RSVD_0                                             3:0 /* RWIVF */
#define NV_BR04_XVU_CYA_NIBBLE0_RSVD_0_INIT                                 0x00000000 /* RWI-V */
#define NV_BR04_XVU_CYA_NIBBLE0_RSVD_4                                           19:16 /* RWIVF */
#define NV_BR04_XVU_CYA_NIBBLE0_RSVD_4_INIT                                 0x00000000 /* RWI-V */
#define NV_BR04_XVU_ROM_REVISION                                            0x00000B08 /* RW-4R */

#endif // DEV_BR04_XVU_H
