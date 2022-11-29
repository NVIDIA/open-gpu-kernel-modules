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
// Source file: ctrl/ctrl5070/ctrl5070or.finn
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
 * NV5070_CTRL_CMD_GET_SOR_PWM
 *
 * This command returns SOR's current PWM settings.
 *
 *      orNumber
 *          The OR number for which the seq ctrls are to be modified.
 *
 *      targetFreq
 *          The target PWM freq. This is the PWM frequency we planned on
 *          programming.
 *
 *      actualFreq
 *          Actual PWM freq programmed into PWM.
 *
 *      div
 *          The divider being used currently for generating PWM clk.
 *          A valued of 0 means that PWM is disabled.
 *
 *      resolution
 *          The resolution of steps currently programmed or the max number of
 *          clocks per cycle. The possible values for NV50 are 128, 256, 512
 *          and 1024. This field is irrelevant when div is 0.
 *
 *      dutyCycle
 *          Duty cycle in range 0-1024
 *
 *      sourcePCLK (OUT)
 *          The PWM source clock selector. This field is non-zero if the PCLK
 *          is selected as the PWM source clock. Otherwise, the PWM source
 *          clock is XTAL.
 *
 *      head (IN)
 *          The head for which the pixel clock is sourced from.
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_GENERIC
 */
#define NV5070_CTRL_CMD_GET_SOR_PWM                                (0x50700420) /* finn: Evaluated from "(FINN_NV50_DISPLAY_OR_INTERFACE_ID << 8) | NV5070_CTRL_CMD_GET_SOR_PWM_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_CMD_GET_SOR_PWM_PARAMS_MESSAGE_ID (0x20U)

typedef struct NV5070_CTRL_CMD_GET_SOR_PWM_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       orNumber;
    NvU32                       targetFreq;
    NvU32                       actualFreq;
    NvU32                       div;
    NvU32                       resolution;
    NvU32                       dutyCycle;
    NvU32                       sourcePCLK;
    NvU32                       head;
} NV5070_CTRL_CMD_GET_SOR_PWM_PARAMS;


/*
 * NV5070_CTRL_CMD_SET_SOR_PWM
 *
 * This command returns SOR's current PWM settings.
 *
 *      orNumber
 *          The OR number for which the seq ctrls are to be modified.
 *
 *      targetFreq
 *          The target PWM freq to be programmed.
 *
 *      actualFreq
 *          Actual PWM freq programmed into PWM after all the specified
 *          settings have been applied.
 *
 *      div
 *          The divider to use for generating PWM clk.
 *          Set this to 0 to disable PWM. Note that only one of div
 *          or targetFreq can be specified at a time since specifying one
 *          automatically determines the value of the other. Selection is
 *          done via USE_SPECIFIED_DIV flag.
 *
 *      resolution
 *          The resolution or the max number of clocks per cycle desired.
 *          Note that if it's not possible to program the given resolution
 *          and frequency (or div) combination, RM would not attempt to
 *          smartly lower the resolution. The call would return failure.
 *          The possible values for NV50 are 128, 256, 512 and 1024. This
 *          field is irrelevant when div is 0.
 *
 *      dutyCycle
 *          Duty cycle in range 0-1024
 *
 *      flags
 *          The following flags have been defined:
 *              (1) USE_SPECIFIED_DIV: Indicates whether RM should use
 *                  specified div or targetFreq when determining the divider
 *                  for xtal clock.
 *              (2) PROG_DUTY_CYCLE: Indicates whether or not the caller
 *                  desires to program duty cycle. Normally whenever pwm freq
 *                  and range need to be programmed, it's expected that duty
 *                  cycle would be reprogrammed as well but this is not
 *                  enforced.
 *              (3) PROG_FREQ_AND_RANGE: Indicates whether or not the caller
 *                  desires to program a new PWM setting (div and resolution).
 *              (4) SOURCE_CLOCK: Indicates whether the PCLK or XTAL is used
 *                  as the PWM clock source. GT21x and better.
 *
 *      head (IN)
 *          The head for which the pixel clock is sourced from.
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_GENERIC
 */
#define NV5070_CTRL_CMD_SET_SOR_PWM                               (0x50700421) /* finn: Evaluated from "(FINN_NV50_DISPLAY_OR_INTERFACE_ID << 8) | NV5070_CTRL_CMD_SET_SOR_PWM_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_CMD_SET_SOR_PWM_FLAGS_USE_SPECIFIED_DIV                 0:0
#define NV5070_CTRL_CMD_SET_SOR_PWM_FLAGS_USE_SPECIFIED_DIV_NO    (0x00000000)
#define NV5070_CTRL_CMD_SET_SOR_PWM_FLAGS_USE_SPECIFIED_DIV_YES   (0x00000001)
#define NV5070_CTRL_CMD_SET_SOR_PWM_FLAGS_PROG_DUTY_CYCLE                   1:1
#define NV5070_CTRL_CMD_SET_SOR_PWM_FLAGS_PROG_DUTY_CYCLE_NO      (0x00000000)
#define NV5070_CTRL_CMD_SET_SOR_PWM_FLAGS_PROG_DUTY_CYCLE_YES     (0x00000001)
#define NV5070_CTRL_CMD_SET_SOR_PWM_FLAGS_PROG_FREQ_AND_RANGE               2:2
#define NV5070_CTRL_CMD_SET_SOR_PWM_FLAGS_PROG_FREQ_AND_RANGE_NO  (0x00000000)
#define NV5070_CTRL_CMD_SET_SOR_PWM_FLAGS_PROG_FREQ_AND_RANGE_YES (0x00000001)
#define NV5070_CTRL_CMD_SET_SOR_PWM_FLAGS_SOURCE_CLOCK                      3:3
#define NV5070_CTRL_CMD_SET_SOR_PWM_FLAGS_SOURCE_CLOCK_XTAL       (0x00000000)
#define NV5070_CTRL_CMD_SET_SOR_PWM_FLAGS_SOURCE_CLOCK_PCLK       (0x00000001)

#define NV5070_CTRL_CMD_SET_SOR_PWM_PARAMS_MESSAGE_ID (0x21U)

typedef struct NV5070_CTRL_CMD_SET_SOR_PWM_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       orNumber;
    NvU32                       targetFreq;
    NvU32                       actualFreq;
    NvU32                       div;                  // equivalent of NV_PDISP_SOR_PWM_DIV_DIVIDE
    NvU32                       resolution;           // equivalent of NV_PDISP_SOR_PWM_DIV_RANGE
    NvU32                       dutyCycle;
    NvU32                       flags;
    NvU32                       head;
} NV5070_CTRL_CMD_SET_SOR_PWM_PARAMS;


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
 * NV5070_CTRL_CMD_GET_PIOR_OP_MODE
 *
 * This command returns current settings for the specified PIOR.
 *
 *      orNumber
 *          The OR number for which the operating mode needs to be programmed.
 *
 *      category
 *          Whether ext TMDS, TV, DRO or DRI settings are desired.
 *          EXT TV is not supported at the moment.
 *          EXT DisplayPort is specified through EXT 10BPC 444.
 *
 *      clkPolarity
 *          Whether or not output clock is inverted relative to generated clock.
 *
 *      clkMode
 *          Whether data being transmitted is SDR or DDR.
 *
 *      clkPhs
 *          Position of the edge on which data is launched.
 *
 *      unusedPins
 *          Status of unused pins of this PIOR.
 *
 *      polarity
 *          Whether or not sync and DE pin polarities are inverted.
 *
 *      dataMuxing
 *          How are the bits are multiplexed together.
 *
 *      clkDelay
 *          Extra delay for the clock.
 *
 *      dataDelay
 *          Extra delay for the data.
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_GENERIC
 */
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE                        (0x50700430) /* finn: Evaluated from "(FINN_NV50_DISPLAY_OR_INTERFACE_ID << 8) | NV5070_CTRL_CMD_GET_PIOR_OP_MODE_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_CATEGORY                           2:0
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_CATEGORY_EXT_TMDS      0x00000000
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_CATEGORY_EXT_TV        0x00000001
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_CATEGORY_DRO           0x00000003
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_CATEGORY_DRI           0x00000004
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_CATEGORY_EXT_10BPC_444 0x00000005

#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_CLK_POLARITY                       0:0
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_CLK_POLARITY_NORMAL    0x00000000
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_CLK_POLARITY_INV       0x00000001

#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_CLK_MODE                           0:0
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_CLK_MODE_SDR           0x00000000
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_CLK_MODE_DDR           0x00000001

#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_CLK_PHS                            1:0
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_CLK_PHS_0              0x00000000
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_CLK_PHS_1              0x00000001
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_CLK_PHS_2              0x00000002
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_CLK_PHS_3              0x00000003

#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_UNUSED_PINS                        0:0
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_UNUSED_PINS_LO         0x00000000
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_UNUSED_PINS_TS         0x00000001

#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_POLARITY_H                         0:0
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_POLARITY_H_NORMAL      0x00000000
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_POLARITY_H_INV         0x00000001
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_POLARITY_V                         1:1
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_POLARITY_V_NORMAL      0x00000000
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_POLARITY_V_INV         0x00000001
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_POLARITY_DE                        2:2
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_POLARITY_DE_NORMAL     0x00000000
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_POLARITY_DE_INV        0x00000001

#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_DATA_MUXING                        3:0
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_DATA_MUXING_RGB_0      0x00000000
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_DATA_MUXING_RGB_1      0x00000001
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_DATA_MUXING_DIST_RNDR  0x00000003
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_DATA_MUXING_YUV_0      0x00000004
#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_DATA_MUXING_UYVY       0x00000005

#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_CLK_DLY                            2:0

#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_DATA_DLY                           2:0

#define NV5070_CTRL_CMD_GET_PIOR_OP_MODE_PARAMS_MESSAGE_ID (0x30U)

typedef struct NV5070_CTRL_CMD_GET_PIOR_OP_MODE_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       orNumber;

    NvU32                       category;
    NvU32                       clkPolarity;
    NvU32                       clkMode;
    NvU32                       clkPhs;
    NvU32                       unusedPins;
    NvU32                       polarity;
    NvU32                       dataMuxing;
    NvU32                       clkDelay;
    NvU32                       dataDelay;
} NV5070_CTRL_CMD_GET_PIOR_OP_MODE_PARAMS;


/*
 * NV5070_CTRL_CMD_SET_PIOR_OP_MODE
 *
 * This command applies the specified settings to the specified PIOR.
 *
 *      orNumber
 *          The OR number for which the operating mode needs to be programmed.
 *
 *      category
 *          Whether ext TMDS, TV, DRO or DRI settings are to be programmed.
 *          EXT TV is not supported at the moment.
 *          EXT DisplayPort is specified through EXT 10BPC 444.
 *
 *      clkPolarity
 *          Whether or not to invert output clock relative to generated clock.
 *
 *      clkMode
 *          Whether data being transmitted should be SDR or DDR.
 *
 *      clkPhs
 *          Position of the edge on which data should be launched.
 *
 *      unusedPins
 *          What to do with unused pins of this PIOR.
 *
 *      polarity
 *          Whether or not to invert sync and DE pin polarities.
 *
 *      dataMuxing
 *          How to multiplex the bits together.
 *
 *      clkDelay
 *          Extra delay for the clock.
 *
 *      dataDelay
 *          Extra delay for the data.
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_GENERIC
 */
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE                           (0x50700431) /* finn: Evaluated from "(FINN_NV50_DISPLAY_OR_INTERFACE_ID << 8) | NV5070_CTRL_CMD_SET_PIOR_OP_MODE_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_CATEGORY                           2:0
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_CATEGORY_EXT_TMDS         0x00000000
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_CATEGORY_EXT_TV           0x00000001
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_CATEGORY_DRO              0x00000003
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_CATEGORY_DRI              0x00000004
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_CATEGORY_EXT_10BPC_444    0x00000005

#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_CLK_POLARITY                       0:0
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_CLK_POLARITY_NORMAL       0x00000000
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_CLK_POLARITY_INV          0x00000001

#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_CLK_MODE                           0:0
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_CLK_MODE_SDR              0x00000000
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_CLK_MODE_DDR              0x00000001

#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_CLK_PHS                            1:0
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_CLK_PHS_0                 0x00000000
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_CLK_PHS_1                 0x00000001
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_CLK_PHS_2                 0x00000002
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_CLK_PHS_3                 0x00000003

#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_UNUSED_PINS                        0:0
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_UNUSED_PINS_LO            0x00000000
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_UNUSED_PINS_TS            0x00000001

#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_POLARITY_H                         0:0
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_POLARITY_H_NORMAL         0x00000000
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_POLARITY_H_INV            0x00000001
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_POLARITY_V                         1:1
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_POLARITY_V_NORMAL         0x00000000
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_POLARITY_V_INV            0x00000001
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_POLARITY_DE                        2:2
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_POLARITY_DE_NORMAL        0x00000000
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_POLARITY_DE_INV           0x00000001

#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_DATA_MUXING                        3:0
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_DATA_MUXING_RGB_0         0x00000000
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_DATA_MUXING_RGB_1         0x00000001
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_DATA_MUXING_DIST_RNDR     0x00000003
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_DATA_MUXING_YUV_0         0x00000004
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_DATA_MUXING_UYVY          0x00000005

#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_CLK_DLY                            2:0

#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_DATA_DLY                           2:0

#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_DRO_MASTER                         1:0

#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_DRO_DRIVE_PIN_SET                  2:0
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_DRO_DRIVE_PIN_SET_NEITHER 0
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_DRO_DRIVE_PIN_SET_A       1
#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_DRO_DRIVE_PIN_SET_B       2


#define NV5070_CTRL_CMD_SET_PIOR_OP_MODE_PARAMS_MESSAGE_ID (0x31U)

typedef struct NV5070_CTRL_CMD_SET_PIOR_OP_MODE_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       orNumber;

    NvU32                       category;
    NvU32                       clkPolarity;
    NvU32                       clkMode;
    NvU32                       clkPhs;
    NvU32                       unusedPins;
    NvU32                       polarity;
    NvU32                       dataMuxing;
    NvU32                       clkDelay;
    NvU32                       dataDelay;
    NvU32                       dro_master;
    NvU32                       dro_drive_pin_set;
} NV5070_CTRL_CMD_SET_PIOR_OP_MODE_PARAMS;

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
