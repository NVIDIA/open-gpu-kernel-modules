/*
 * SPDX-FileCopyrightText: Copyright (c) 2001-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrl5070/ctrl5070or.finn
//

#include "ctrl5070common.h"
#include "ctrl/ctrl5070/ctrl5070base.h"

/*
 * NV5070_CTRL_CMD_SET_DAC_PWR
 *
 * This command sets the DAC power control register. orNumber, normalPower,
 * and safePower will always have to be specified. However, HSync, VSync,
 * and data for normal and/or safe mode can be empty, leaving the current
 * values intact.
 *
 *      orNumber
 *          The dac for which the settings need to be programmed.
 *
 *      normalHSync
 *          The normal operating state for the H sync signal.
 *
 *      normalVSync
 *          The normal operating state for the V sync signal.
 *
 *      normalData
 *          The normal video data input pin of the d/a converter.
 *
 *      normalPower
 *          The normal state of the dac macro power.
 *
 *      safeHSync
 *          The safe operating state for the H sync signal.
 *
 *      safeVSync
 *          The safe operating state for the V sync signal.
 *
 *      safeData
 *          The safe video data input pin of the d/a converter.
 *
 *      safePower
 *          The safe state of the dac macro power.
 *
 *      flags
 *          The following flags have been defined:
 *              (1) SPECIFIED_NORMAL: Indicates whether HSync, VSync, data,
 *                  for normal state have been specified in the parameters.
 *              (2) SPECIFIED_SAFE: Indicates whether HSync, VSync, data,
 *                  for safe state have been specified in the parameters.
 *              (3) SPECIFIED_FORCE_SWITCH: Indicates whether to force the
 *                  change immediately instead of waiting for VSync
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_GENERIC
 *      NV_ERR_TIMEOUT
 */
#define NV5070_CTRL_CMD_SET_DAC_PWR                            (0x50700404) /* finn: Evaluated from "(FINN_NV50_DISPLAY_OR_INTERFACE_ID << 8) | NV5070_CTRL_CMD_SET_DAC_PWR_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_CMD_SET_DAC_PWR_NORMAL_HSYNC                            1:0
#define NV5070_CTRL_CMD_SET_DAC_PWR_NORMAL_HSYNC_ENABLE        (0x00000000)
#define NV5070_CTRL_CMD_SET_DAC_PWR_NORMAL_HSYNC_LO            (0x00000001)
#define NV5070_CTRL_CMD_SET_DAC_PWR_NORMAL_HSYNC_HI            (0x00000002)

#define NV5070_CTRL_CMD_SET_DAC_PWR_NORMAL_VSYNC                            1:0
#define NV5070_CTRL_CMD_SET_DAC_PWR_NORMAL_VSYNC_ENABLE        (0x00000000)
#define NV5070_CTRL_CMD_SET_DAC_PWR_NORMAL_VSYNC_LO            (0x00000001)
#define NV5070_CTRL_CMD_SET_DAC_PWR_NORMAL_VSYNC_HI            (0x00000002)

#define NV5070_CTRL_CMD_SET_DAC_PWR_NORMAL_DATA                             1:0
#define NV5070_CTRL_CMD_SET_DAC_PWR_NORMAL_DATA_ENABLE         (0x00000000)
#define NV5070_CTRL_CMD_SET_DAC_PWR_NORMAL_DATA_LO             (0x00000001)
#define NV5070_CTRL_CMD_SET_DAC_PWR_NORMAL_DATA_HI             (0x00000002)

#define NV5070_CTRL_CMD_SET_DAC_PWR_NORMAL_PWR                              0:0
#define NV5070_CTRL_CMD_SET_DAC_PWR_NORMAL_PWR_OFF             (0x00000000)
#define NV5070_CTRL_CMD_SET_DAC_PWR_NORMAL_PWR_ON              (0x00000001)

#define NV5070_CTRL_CMD_SET_DAC_PWR_SAFE_HSYNC                              1:0
#define NV5070_CTRL_CMD_SET_DAC_PWR_SAFE_HSYNC_ENABLE          (0x00000000)
#define NV5070_CTRL_CMD_SET_DAC_PWR_SAFE_HSYNC_LO              (0x00000001)
#define NV5070_CTRL_CMD_SET_DAC_PWR_SAFE_HSYNC_HI              (0x00000002)

#define NV5070_CTRL_CMD_SET_DAC_PWR_SAFE_VSYNC                              1:0
#define NV5070_CTRL_CMD_SET_DAC_PWR_SAFE_VSYNC_ENABLE          (0x00000000)
#define NV5070_CTRL_CMD_SET_DAC_PWR_SAFE_VSYNC_LO              (0x00000001)
#define NV5070_CTRL_CMD_SET_DAC_PWR_SAFE_VSYNC_HI              (0x00000002)

#define NV5070_CTRL_CMD_SET_DAC_PWR_SAFE_DATA                               1:0
#define NV5070_CTRL_CMD_SET_DAC_PWR_SAFE_DATA_ENABLE           (0x00000000)
#define NV5070_CTRL_CMD_SET_DAC_PWR_SAFE_DATA_LO               (0x00000001)
#define NV5070_CTRL_CMD_SET_DAC_PWR_SAFE_DATA_HI               (0x00000002)

#define NV5070_CTRL_CMD_SET_DAC_PWR_SAFE_PWR                                0:0
#define NV5070_CTRL_CMD_SET_DAC_PWR_SAFE_PWR_OFF               (0x00000000)
#define NV5070_CTRL_CMD_SET_DAC_PWR_SAFE_PWR_ON                (0x00000001)

#define NV5070_CTRL_CMD_SET_DAC_PWR_FLAGS_SPECIFIED_NORMAL                  0:0
#define NV5070_CTRL_CMD_SET_DAC_PWR_FLAGS_SPECIFIED_NORMAL_NO  (0x00000000)
#define NV5070_CTRL_CMD_SET_DAC_PWR_FLAGS_SPECIFIED_NORMAL_YES (0x00000001)
#define NV5070_CTRL_CMD_SET_DAC_PWR_FLAGS_SPECIFIED_SAFE                    1:1
#define NV5070_CTRL_CMD_SET_DAC_PWR_FLAGS_SPECIFIED_SAFE_NO    (0x00000000)
#define NV5070_CTRL_CMD_SET_DAC_PWR_FLAGS_SPECIFIED_SAFE_YES   (0x00000001)
#define NV5070_CTRL_CMD_SET_DAC_PWR_FLAGS_FORCE_SWITCH                      2:2
#define NV5070_CTRL_CMD_SET_DAC_PWR_FLAGS_FORCE_SWITCH_NO      (0x00000000)
#define NV5070_CTRL_CMD_SET_DAC_PWR_FLAGS_FORCE_SWITCH_YES     (0x00000001)
#define NV5070_CTRL_CMD_SET_DAC_PWR_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV5070_CTRL_CMD_SET_DAC_PWR_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       orNumber;

    NvU32                       normalHSync;
    NvU32                       normalVSync;
    NvU32                       normalData;
    NvU32                       normalPower;
    NvU32                       safeHSync;
    NvU32                       safeVSync;
    NvU32                       safeData;
    NvU32                       safePower;
    NvU32                       flags;
} NV5070_CTRL_CMD_SET_DAC_PWR_PARAMS;

/*
 * NV5070_CTRL_CMD_GET_SOR_OP_MODE
 *
 * This command returns current settings for the specified SOR.
 *
 *      orNumber
 *          The OR number for which the operating mode needs to be read.
 *
 *      category
 *          Whether LVDS or CSTM setting are desired.
 *
 *      puTxda
 *          Status of data pins of link A
 *
 *      puTxdb
 *          Status of data pins of link B
 *
 *      puTxca
 *          Status of link A clock
 *
 *      puTxcb
 *          Status of link B clock
 *
 *      upper
 *          Whether LVDS bank A is the upper, odd, or first pixel.
 *
 *      mode
 *          Current protocol.
 *
 *      linkActA
 *          Status of link B clock
 *
 *      linkActB
 *          Status of link B clock
 *
 *      lvdsEn
 *          Output driver configuration.
 *
 *      lvdsDual
 *          Whether LVDS dual-link mode is turned on or not.
 *
 *      dupSync
 *          Whether DE, HSYNC, and VSYNC are used for encoding instead of
 *          RES, CNTLE, and CNTLF.
 *
 *      newMode
 *          Whether new or old mode is being used.
 *
 *      balanced
 *          Whether balanced encoding is enabled.
 *
 *      plldiv
 *          Feedback divider for the hi-speed pll
 *
 *      rotClk
 *          Skew of TXC clock.
 *
 *      rotDat
 *          How much are the 8 bits of each color channel rotated by
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_GENERIC
 */
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE                   (0x50700422) /* finn: Evaluated from "(FINN_NV50_DISPLAY_OR_INTERFACE_ID << 8) | NV5070_CTRL_CMD_GET_SOR_OP_MODE_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_CATEGORY                            0:0
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_CATEGORY_LVDS     0x00000000
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_CATEGORY_CUSTOM   0x00000001

#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXDA_0                           0:0
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXDA_0_DISABLE 0x00000000
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXDA_0_ENABLE  0x00000001
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXDA_1                           1:1
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXDA_1_DISABLE 0x00000000
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXDA_1_ENABLE  0x00000001
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXDA_2                           2:2
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXDA_2_DISABLE 0x00000000
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXDA_2_ENABLE  0x00000001
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXDA_3                           3:3
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXDA_3_DISABLE 0x00000000
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXDA_3_ENABLE  0x00000001

#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXDB_0                           0:0
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXDB_0_DISABLE 0x00000000
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXDB_0_ENABLE  0x00000001
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXDB_1                           1:1
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXDB_1_DISABLE 0x00000000
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXDB_1_ENABLE  0x00000001
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXDB_2                           2:2
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXDB_2_DISABLE 0x00000000
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXDB_2_ENABLE  0x00000001
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXDB_3                           3:3
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXDB_3_DISABLE 0x00000000
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXDB_3_ENABLE  0x00000001

#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXCA                             0:0
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXCA_DISABLE   0x00000000
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXCA_ENABLE    0x00000001

#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXCB                             0:0
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXCB_DISABLE   0x00000000
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PU_TXCB_ENABLE    0x00000001

#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_UPPER                               0:0
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_UPPER_UPPER_RESET 0x00000001

#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_MODE                                0:0
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_MODE_LVDS         0x00000000
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_MODE_TMDS         0x00000001

#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_LINKACTA                            0:0
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_LINKACTA_DISABLE  0x00000000
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_LINKACTA_ENABLE   0x00000001

#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_LINKACTB                            0:0
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_LINKACTB_DISABLE  0x00000000
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_LINKACTB_ENABLE   0x00000001

#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_LVDS_EN                             0:0
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_LVDS_EN_DISABLE   0x00000000
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_LVDS_EN_ENABLE    0x00000001

#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_LVDS_DUAL                           0:0
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_LVDS_DUAL_DISABLE 0x00000000
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_LVDS_DUAL_ENABLE  0x00000001

#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_DUP_SYNC                            0:0
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_DUP_SYNC_DISABLE  0x00000000
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_DUP_SYNC_ENABLE   0x00000001

#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_NEW_MODE                            0:0
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_NEW_MODE_DISABLE  0x00000000
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_NEW_MODE_ENABLE   0x00000001

#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_BALANCED                            0:0
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_BALANCED_DISABLE  0x00000000
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_BALANCED_ENABLE   0x00000001

#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PLLDIV                              0:0
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PLLDIV_BY_7       0x00000000
#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PLLDIV_BY_10      0x00000001

#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_ROTCLK                              3:0

#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_ROTDAT                              2:0

#define NV5070_CTRL_CMD_GET_SOR_OP_MODE_PARAMS_MESSAGE_ID (0x22U)

typedef struct NV5070_CTRL_CMD_GET_SOR_OP_MODE_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       orNumber;

    NvU32                       category;
    NvU32                       puTxda;
    NvU32                       puTxdb;
    NvU32                       puTxca;
    NvU32                       puTxcb;
    NvU32                       upper;
    NvU32                       mode;
    NvU32                       linkActA;
    NvU32                       linkActB;
    NvU32                       lvdsEn;
    NvU32                       lvdsDual;
    NvU32                       dupSync;
    NvU32                       newMode;
    NvU32                       balanced;
    NvU32                       plldiv;
    NvU32                       rotClk;
    NvU32                       rotDat;
} NV5070_CTRL_CMD_GET_SOR_OP_MODE_PARAMS;


/*
 * NV5070_CTRL_CMD_SET_SOR_OP_MODE
 *
 * This command applies the specified settings to the specified SOR.
 *
 *      orNumber
 *          The OR number for which the operating mode needs to be read.
 *          Note that if DCB doesn't report LVDS for the specified orNumber,
 *          the call will return failure.
 *
 *      category
 *          Whether LVDS or CSTM settings are specified.
 *
 *      puTxda
 *          Used to enable or disable the data pins of link A.
 *
 *      puTxdb
 *          Used to enable or disable the data pins of link B.
 *
 *      puTxca
 *          Used to enable or disable link A clock.
 *
 *      puTxcb
 *          Used to enable or disable link B clock.
 *
 *      upper
 *          Whether LVDS bank A should be the upper, odd, or first pixel.
 *
 *      mode
 *          What protocol (LVDS/TMDS to use).
 *
 *      linkActA
 *          Used to enable or disable the digital logic of link A.
 *
 *      linkActB
 *          Used to enable or disable the digital logic of link B.
 *
 *      lvdsEn
 *          Output driver configuration.
 *
 *      lvdsDual
 *          Whether to turn on LVDS dual-link mode.
 *
 *      dupSync
 *          Whether to use DE, HSYNC, and VSYNC for encoding instead of
 *          RES, CNTLE, and CNTLF.
 *
 *      newMode
 *          Whether to use new or old mode.
 *
 *      balanced
 *          Whether or not to use balanced encoding.
 *
 *      plldiv
 *          Feedback divider to use for the hi-speed pll.
 *
 *      rotClk
 *          How much to skew TXC clock.
 *
 *      rotDat
 *          How much to rotate the 8 bits of each color channel by.
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_GENERIC
 */
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE                   (0x50700423) /* finn: Evaluated from "(FINN_NV50_DISPLAY_OR_INTERFACE_ID << 8) | NV5070_CTRL_CMD_SET_SOR_OP_MODE_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_CATEGORY                            0:0
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_CATEGORY_LVDS     0x00000000
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_CATEGORY_CUSTOM   0x00000001

#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXDA_0                           0:0
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXDA_0_DISABLE 0x00000000
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXDA_0_ENABLE  0x00000001
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXDA_1                           1:1
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXDA_1_DISABLE 0x00000000
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXDA_1_ENABLE  0x00000001
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXDA_2                           2:2
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXDA_2_DISABLE 0x00000000
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXDA_2_ENABLE  0x00000001
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXDA_3                           3:3
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXDA_3_DISABLE 0x00000000
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXDA_3_ENABLE  0x00000001

#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXDB_0                           0:0
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXDB_0_DISABLE 0x00000000
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXDB_0_ENABLE  0x00000001
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXDB_1                           1:1
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXDB_1_DISABLE 0x00000000
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXDB_1_ENABLE  0x00000001
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXDB_2                           2:2
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXDB_2_DISABLE 0x00000000
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXDB_2_ENABLE  0x00000001
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXDB_3                           3:3
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXDB_3_DISABLE 0x00000000
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXDB_3_ENABLE  0x00000001

#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXCA                             0:0
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXCA_DISABLE   0x00000000
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXCA_ENABLE    0x00000001

#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXCB                             0:0
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXCB_DISABLE   0x00000000
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PU_TXCB_ENABLE    0x00000001

#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_UPPER                               0:0
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_UPPER_UPPER_RESET 0x00000001

#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_MODE                                0:0
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_MODE_LVDS         0x00000000
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_MODE_TMDS         0x00000001

#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_LINKACTA                            0:0
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_LINKACTA_DISABLE  0x00000000
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_LINKACTA_ENABLE   0x00000001

#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_LINKACTB                            0:0
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_LINKACTB_DISABLE  0x00000000
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_LINKACTB_ENABLE   0x00000001

#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_LVDS_EN                             0:0
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_LVDS_EN_DISABLE   0x00000000
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_LVDS_EN_ENABLE    0x00000001

#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_LVDS_DUAL                           0:0
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_LVDS_DUAL_DISABLE 0x00000000
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_LVDS_DUAL_ENABLE  0x00000001

#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_DUP_SYNC                            0:0
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_DUP_SYNC_DISABLE  0x00000000
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_DUP_SYNC_ENABLE   0x00000001

#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_NEW_MODE                            0:0
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_NEW_MODE_DISABLE  0x00000000
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_NEW_MODE_ENABLE   0x00000001

#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_BALANCED                            0:0
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_BALANCED_DISABLE  0x00000000
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_BALANCED_ENABLE   0x00000001

#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PLLDIV                              0:0
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PLLDIV_BY_7       0x00000000
#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PLLDIV_BY_10      0x00000001

#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_ROTCLK                              3:0

#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_ROTDAT                              2:0

#define NV5070_CTRL_CMD_SET_SOR_OP_MODE_PARAMS_MESSAGE_ID (0x23U)

typedef struct NV5070_CTRL_CMD_SET_SOR_OP_MODE_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       orNumber;

    NvU32                       category;
    NvU32                       puTxda;
    NvU32                       puTxdb;
    NvU32                       puTxca;
    NvU32                       puTxcb;
    NvU32                       upper;
    NvU32                       mode;
    NvU32                       linkActA;
    NvU32                       linkActB;
    NvU32                       lvdsEn;
    NvU32                       lvdsDual;
    NvU32                       dupSync;
    NvU32                       newMode;
    NvU32                       balanced;
    NvU32                       plldiv;
    NvU32                       rotClk;
    NvU32                       rotDat;
} NV5070_CTRL_CMD_SET_SOR_OP_MODE_PARAMS;

/*
 * NV5070_CTRL_CMD_SET_SOR_FLUSH_MODE
 *
 * Set the given SOR number into flush mode in preparation for DP link training.
 *
 *   orNumber [in]
 *     The SOR number to set into flush mode.
 *
 *   bEnable [in]
 *     Whether to enable or disable flush mode on this SOR.
 * 
 *   bImmediate [in]
 *     If set to true, will enable flush in immediate mode.
 *     If not, will enable flush in loadv mode.
 *     NOTE: We do not support exit flush in LoadV mode.
 * 
 *   headMask [in]
 *     Optional.  If set brings only the heads in the head mask out of flush
 *     OR will stay in flush mode until last head is out of flush mode.
 *     Caller can use _HEAD__ALL to specify all the heads are to be brought out.
 *     NOTE: headMask would be considered only while exiting from flush mode.
 * 
 *   bForceRgDiv [in]
 *      If set forces RgDiv. Should be used only for HW/SW testing
 * 
 *   bUseBFM [in]
 *      If Set then it mean we are using BFM else executing on non-BFM paltforms.
 * 
 *   bFireAndForget [in]
 *       Fire the flush mode & perform post-processing without waiting for it
 *       to be done. This is required for special cases like GC5 where we have
 *       ELV blocked, RG stall & we trigger flush for one shot mode & then do
 *       a modeset by disabling it without actually waiting for it to get
 *       disabled. We will not get any vblank interrupt in this case as we have
 *       stalled RG.
 * 
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV5070_CTRL_CMD_SET_SOR_FLUSH_MODE (0x50700457) /* finn: Evaluated from "(FINN_NV50_DISPLAY_OR_INTERFACE_ID << 8) | NV5070_CTRL_SET_SOR_FLUSH_MODE_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_SET_SOR_FLUSH_MODE_PARAMS_MESSAGE_ID (0x57U)

typedef struct NV5070_CTRL_SET_SOR_FLUSH_MODE_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       sorNumber;
    NvBool                      bEnable;
    NvBool                      bImmediate;
    NvU32                       headMask;
    NvBool                      bForceRgDiv;
    NvBool                      bUseBFM;
    NvBool                      bFireAndForget;
} NV5070_CTRL_SET_SOR_FLUSH_MODE_PARAMS;

#define NV5070_CTRL_SET_SOR_FLUSH_MODE_PARAMS_HEADMASK_HEAD(i)          (i):(i)
#define NV5070_CTRL_SET_SOR_FLUSH_MODE_PARAMS_HEADMASK_HEAD__SIZE_1 NV5070_CTRL_CMD_MAX_HEADS
#define NV5070_CTRL_SET_SOR_FLUSH_MODE_PARAMS_HEADMASK_HEAD_ALL     0xFFFFFFFF



/* _ctrl5070or_h_ */
