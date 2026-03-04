/*
 * SPDX-FileCopyrightText: Copyright (c) 2011-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl2080/ctrl2080power.finn
//

#include "ctrl/ctrl2080/ctrl2080base.h"



/*!
 * @brief GC6 flavor ids
 */
typedef enum NV2080_CTRL_GC6_FLAVOR_ID {
    NV2080_CTRL_GC6_FLAVOR_ID_MSHYBRID = 0,
    NV2080_CTRL_GC6_FLAVOR_ID_OPTIMUS = 1,


    NV2080_CTRL_GC6_FLAVOR_ID_MAX = 4,
} NV2080_CTRL_GC6_FLAVOR_ID;

/*
 * NV2080_CTRL_CMD_GC6_ENTRY
 *
 * This command executes the steps of GC6 entry sequence
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED (non-fatal)
 *   NV_ERR_INVALID_STATE (non-fatal)
 *   NV_ERR_INVALID_ARGUMENT (non-fatal)
 *   NV_ERR_NOT_READY (non-fatal)
 *   NV_ERR_TIMEOUT
 *   NV_ERR_GENERIC
 */
#define NV2080_CTRL_CMD_GC6_ENTRY (0x2080270d) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_POWER_INTERFACE_ID << 8) | NV2080_CTRL_GC6_ENTRY_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GC6_ENTRY_PARAMS_MESSAGE_ID (0xDU)

typedef struct NV2080_CTRL_GC6_ENTRY_PARAMS {
    NV2080_CTRL_GC6_FLAVOR_ID flavorId;
    NvU32                     stepMask;
    struct {


        NvBool bIsRTD3Transition;
        NvBool bIsRTD3CoreRailPowerCut;


        NvBool bSkipPstateSanity;
    } params;
} NV2080_CTRL_GC6_ENTRY_PARAMS;

/*
 * NV2080_CTRL_CMD_GC6_EXIT
 *
 * This command executes the steps of GC6 exit sequence
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_GENERIC
 */
#define NV2080_CTRL_CMD_GC6_EXIT (0x2080270e) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_POWER_INTERFACE_ID << 8) | NV2080_CTRL_GC6_EXIT_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_GC6_EXIT_PARAMS_MESSAGE_ID (0xEU)

typedef struct NV2080_CTRL_GC6_EXIT_PARAMS {
    NV2080_CTRL_GC6_FLAVOR_ID flavorId;
    struct {
        NvBool bIsGpuSelfWake;
        NvBool bIsRTD3Transition;


        NvBool bIsRTD3HotTransition;   //output
    } params;
} NV2080_CTRL_GC6_EXIT_PARAMS;

/*!
 * @brief GC6 step ids
 */
typedef enum NV2080_CTRL_GC6_STEP_ID {
    NV2080_CTRL_GC6_STEP_ID_SR_ENTRY = 0,
    NV2080_CTRL_GC6_STEP_ID_GPU_OFF = 1,
    NV2080_CTRL_GC6_STEP_ID_MAX = 2,
} NV2080_CTRL_GC6_STEP_ID;

typedef struct NV2080_CTRL_GC6_FLAVOR_INFO {
    NV2080_CTRL_GC6_FLAVOR_ID flavorId;
    NvU32                     stepMask;
} NV2080_CTRL_GC6_FLAVOR_INFO;



/* _ctrl2080power_h_ */



