/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl0000/ctrl0000syncgpuboost.finn
//

#include "ctrl/ctrl0000/ctrl0000base.h"

#include "ctrl/ctrlxxxx.h"
#include "nvtypes.h"
#include "nvlimits.h"

/* --------------------------- Macros ----------------------------------------*/
// There are at least 2 GPUs in a sync group. Hence max is half of max devices.
#define NV0000_SYNC_GPU_BOOST_MAX_GROUPS       (0x10) /* finn: Evaluated from "((NV_MAX_DEVICES) >> 1)" */
#define NV0000_SYNC_GPU_BOOST_INVALID_GROUP_ID 0xFFFFFFFF

/*-------------------------Command Prototypes---------------------------------*/

/*!
 * Query whether SYNC GPU BOOST MANAGER is enabled or disabled.
 */
#define NV0000_CTRL_CMD_SYNC_GPU_BOOST_INFO    (0xa01) /* finn: Evaluated from "(FINN_NV01_ROOT_SYNC_GPU_BOOST_INTERFACE_ID << 8) | NV0000_SYNC_GPU_BOOST_INFO_PARAMS_MESSAGE_ID" */

#define NV0000_SYNC_GPU_BOOST_INFO_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV0000_SYNC_GPU_BOOST_INFO_PARAMS {
    // [out] Specifies if Sync Gpu Boost Manager is enabled or not.
    NvBool bEnabled;
} NV0000_SYNC_GPU_BOOST_INFO_PARAMS;

/*!
 *  Creates a Synchronized GPU-Boost Group (SGBG)
 */
#define NV0000_CTRL_CMD_SYNC_GPU_BOOST_GROUP_CREATE (0xa02) /* finn: Evaluated from "(FINN_NV01_ROOT_SYNC_GPU_BOOST_INTERFACE_ID << 8) | NV0000_SYNC_GPU_BOOST_GROUP_CREATE_PARAMS_MESSAGE_ID" */

/*! 
 * Describes a Synchronized GPU-Boost Group configuration
 */
typedef struct NV0000_SYNC_GPU_BOOST_GROUP_CONFIG {
    // [in] Number of elements in @ref gpuIds
    NvU32  gpuCount;

    // [in] IDs of GPUs to be put in the Sync Boost Group
    NvU32  gpuIds[NV_MAX_DEVICES];

    // [out] Unique ID of the SGBG, if created
    NvU32  boostGroupId;

    // [in] If this group represents  bridgeless SLI
    NvBool bBridgeless;
} NV0000_SYNC_GPU_BOOST_GROUP_CONFIG;

#define NV0000_SYNC_GPU_BOOST_GROUP_CREATE_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV0000_SYNC_GPU_BOOST_GROUP_CREATE_PARAMS {
    NV0000_SYNC_GPU_BOOST_GROUP_CONFIG boostConfig;
} NV0000_SYNC_GPU_BOOST_GROUP_CREATE_PARAMS;

/*!
 *  Destroys a previously created Synchronized GPU-Boost Group(SGBG)
 */
#define NV0000_CTRL_CMD_SYNC_GPU_BOOST_GROUP_DESTROY (0xa03) /* finn: Evaluated from "(FINN_NV01_ROOT_SYNC_GPU_BOOST_INTERFACE_ID << 8) | NV0000_SYNC_GPU_BOOST_GROUP_DESTROY_PARAMS_MESSAGE_ID" */

#define NV0000_SYNC_GPU_BOOST_GROUP_DESTROY_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV0000_SYNC_GPU_BOOST_GROUP_DESTROY_PARAMS {
    // [[in] Unique ID of the SGBG to be destroyed
    NvU32 boostGroupId;
} NV0000_SYNC_GPU_BOOST_GROUP_DESTROY_PARAMS;

/*!
 * Get configuration information for all Synchronized Boost Groups in the system.
 */
#define NV0000_CTRL_CMD_SYNC_GPU_BOOST_GROUP_INFO (0xa04) /* finn: Evaluated from "(FINN_NV01_ROOT_SYNC_GPU_BOOST_INTERFACE_ID << 8) | NV0000_SYNC_GPU_BOOST_GROUP_INFO_PARAMS_MESSAGE_ID" */

#define NV0000_SYNC_GPU_BOOST_GROUP_INFO_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV0000_SYNC_GPU_BOOST_GROUP_INFO_PARAMS {
    // [out] Number of groups retrieved. @ref NV0000_SYNC_GPU_BOOST_GROUP_INFO_PARAMS::boostGroups
    NvU32                              groupCount;

    // [out] @ref NV0000_SYNC_GPU_BOOST_GROUP_CONFIG
    NV0000_SYNC_GPU_BOOST_GROUP_CONFIG pBoostGroups[NV0000_SYNC_GPU_BOOST_MAX_GROUPS];
} NV0000_SYNC_GPU_BOOST_GROUP_INFO_PARAMS;

/* _ctrl0000syncgpuboost_h_ */
