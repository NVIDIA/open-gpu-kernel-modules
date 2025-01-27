/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrlc370/ctrlc370or.finn
//



#include "ctrl/ctrlc370/ctrlc370base.h"
/* C370 is partially derived from 5070 */
#include "ctrl/ctrl5070/ctrl5070or.h"


//
// NVC370_CTRL_CMD_SET_SOR_FLUSH_MODE
//
// This command is used enable/disable flush mode on all the heads attached to the SOR

//  [in] subDeviceInstance
//     This parameter specifies the subdevice instance within the
//     NV04_DISPLAY_COMMON parent device to which the operation should be
//     directed. This parameter must specify a value between zero and the
//     total number of subdevices within the parent device. This parameter
//     should be set to zero for default behavior.
//   [in] sorNumber
//     All heads connected to this SOR will be programmed
//   [in] headIndex
//     Phase 1 flush exit programs SF_DP_LINKCTL for each head separately
//   [in] attachFailedHeadMask
//     Head mask for the heads where add stream failed
//   [in] phase
//     Flush has 2 Phases (Broad overview):
//         - For Entry
//           - Phase 1: Set core to debug, disable AUDIO and HDCP and engage flush
//           - Phase 2: Disable SOR and SF
//         - For Exit
//           - Phase 1: Program SF for each stream
//           - Phase 2: Exit flush, enable AUDIO and HDCP and disengage flush
//     This design allows interleaving add/delete MST information in-between them
//   [in] bEnable
//     Whether to enable/disable flush
// Possible return values:
//  NV_OK
//  NV_ERR_NOT_SUPPORTED
//

#define NVC370_CTRL_CMD_SET_SOR_FLUSH_MODE (0xc3700401U) /* finn: Evaluated from "(FINN_NVC370_DISPLAY_OR_INTERFACE_ID << 8) | NVC370_CTRL_SET_SOR_FLUSH_MODE_PARAMS_MESSAGE_ID" */

#define NVC370_CTRL_SET_SOR_FLUSH_MODE_PARAMS_MESSAGE_ID (0x1U)

typedef struct NVC370_CTRL_SET_SOR_FLUSH_MODE_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  sorNumber;
    NvU32  headIndex;
    NvU32  attachFailedHeadMask;
    NvU8   phase;
    NvBool bEnable;
} NVC370_CTRL_SET_SOR_FLUSH_MODE_PARAMS;


/* _ctrlc370or_h_ */

#define NVC370_CTRL_SET_SOR_FLUSH_RUN                1:0
#define NVC370_CTRL_SET_SOR_FLUSH_RUN_PHASE_INVALID  (0x00000000U)
#define NVC370_CTRL_SET_SOR_FLUSH_RUN_PHASE1         (0x00000001U)
#define NVC370_CTRL_SET_SOR_FLUSH_RUN_PHASE2         (0x00000002U)

