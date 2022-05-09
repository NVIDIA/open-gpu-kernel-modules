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

#ifndef DEV_BR04_XVD_H
#define DEV_BR04_XVD_H

#define NV_BR04_XVD_LINK_CTRLSTAT                                           0x00000070 /* RW-4R */
#define NV_BR04_XVD_LINK_CTRLSTAT_ASPM_CTRL                                        1:0 /* RWIVF */
#define NV_BR04_XVD_LINK_CTRLSTAT_ASPM_CTRL_DISABLED                        0x00000000 /* RWI-V */
#define NV_BR04_XVD_LINK_CTRLSTAT_ASPM_CTRL_L0S                             0x00000001 /* RW--V */
#define NV_BR04_XVD_LINK_CTRLSTAT_ASPM_CTRL_L1                              0x00000002 /* RW--V */
#define NV_BR04_XVD_LINK_CTRLSTAT_ASPM_CTRL_L0S_L1                          0x00000003 /* RW--V */
#define NV_BR04_XVD_LINK_CTRLSTAT2                                          0x00000090 /* RW-4R */
#define NV_BR04_XVD_LINK_CTRLSTAT2_TARGET_LINK_SPEED                               3:0 /* RWIVF */
#define NV_BR04_XVD_LINK_CTRLSTAT2_TARGET_LINK_SPEED_2P5G                   0x00000001 /* RW--V */
#define NV_BR04_XVD_LINK_CTRLSTAT2_TARGET_LINK_SPEED_5P0G                   0x00000002 /* RWI-V */
#define NV_BR04_XVD_G2_PRIV_XP_LCTRL_2                                      0x0000046C /* RW-4R */
#define NV_BR04_XVD_G2_PRIV_XP_LCTRL_2_ADVERTISED_RATE_CHANGE                      1:1 /* CWIVF */
#define NV_BR04_XVD_G2_PRIV_XP_LCTRL_2_ADVERTISED_RATE_CHANGE_ZERO          0x00000000 /* CWI-V */
#define NV_BR04_XVD_BUS                                                     0x00000018 /* RW-4R */
#define NV_BR04_XVD_BUS_SEC_NUMBER                                                15:8 /* RWIUF */
#define NV_BR04_XVD_BUS_SEC_NUMBER_INIT                                     0x00000000 /* RWI-V */
#define NV_BR04_XVD_BUS_SUB_NUMBER                                               23:16 /* RWIUF */
#define NV_BR04_XVD_BUS_SUB_NUMBER_INIT                                     0x00000000 /* RWI-V */
#define NV_BR04_XVD_G2_PRIV_XP_CONFIG                                       0x00000494 /* RW-4R */
#define NV_BR04_XVD_G2_PRIV_XP_CONFIG_GEN2_REPLAY_TIMER_LIMIT                     11:2 /* RWIVF */
#define NV_BR04_XVD_G2_PRIV_XP_CONFIG_GEN2_REPLAY_TIMER_LIMIT_INIT          0x00000000 /* RWI-V */
#define NV_BR04_XVD_LINK_CTRLSTAT_DLL_LINK_SM                                    29:29 /* R--VF */
#define NV_BR04_XVD_LINK_CTRLSTAT_DLL_LINK_SM_NOT_ACTIVE                    0x00000000 /* R---V */
#define NV_BR04_XVD_LINK_CTRLSTAT_DLL_LINK_SM_ACTIVE                        0x00000001 /* R---V */
#define NV_BR04_XVD_G2_VEND_XP                                              0x00000400 /* RW-4R */
#define NV_BR04_XVD_G2_VEND_XP_OPPORTUNISTIC_ACK                                 28:28 /* RWIVF */
#define NV_BR04_XVD_G2_VEND_XP_OPPORTUNISTIC_ACK_INIT                       0x00000000 /* RWI-V */
#define NV_BR04_XVD_G2_VEND_XP_OPPORTUNISTIC_UPDATEFC                            29:29 /* RWIVF */
#define NV_BR04_XVD_G2_VEND_XP_OPPORTUNISTIC_UPDATEFC_INIT                  0x00000000 /* RWI-V */
#define NV_BR04_XVD_G2_VEND_XP1                                             0x00000404 /* RW-4R */
#define NV_BR04_XVD_G2_VEND_XP1_REPLAY_TIMER_LIMIT                                 9:0 /* RWIVF */
#define NV_BR04_XVD_G2_VEND_XP1_REPLAY_TIMER_LIMIT_INIT                     0x00000000 /* RWI-V */

#endif // DEV_BR04_XVD_H
