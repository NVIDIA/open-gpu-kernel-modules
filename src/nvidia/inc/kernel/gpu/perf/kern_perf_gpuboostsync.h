/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef KERNEL_PERF_GPU_BOOST_SYNC_H
#define KERNEL_PERF_GPU_BOOST_SYNC_H

/* ------------------------ Includes --------------------------------------- */
#include "power/gpu_boost_mgr.h"
#include "ctrl/ctrl2080/ctrl2080internal.h"

/* ------------------------ Macros ----------------------------------------- */
/* ------------------------ Datatypes -------------------------------------- */
/*
 * GPU Boost synchronization info
 */
struct KERNEL_PERF_GPU_BOOST_SYNC
{
    /*!
     * Cached GPU Boost synchronization limits.
     */
    NvU32   limits[NV2080_CTRL_INTERNAL_PERF_SYNC_GPU_BOOST_LIMITS_NUM];

    /*!
     * Hysteresis value for GPU Boost synchronization hysteresis algorithm.
     */
    NvU64   hysteresisus;

    /*!
     * Hysteresis algorithm for SLI GPU Boost synchronization enabled
     */
    NvBool  bHystersisEnable;

    /*!
     * SLI GPU Boost feature is enabled.
     */
    NvBool  bSliGpuBoostSyncEnable;

    /*!
     * Bridgeless information, supports only MIO bridge
     */
    NvBool  bBridgeless;
};

/*!
 * SLI GPU Boost synchronization sub-structure.
 */
typedef struct
{
    /*!
     *  Timestamp of previous GPU Boost synchronization loop.
     */
    NvU64   prevChangeTsns;

    /*!
     * Limits of previous GPU Boost synchronization loop.
     */
    NvU32   prevLimits[NV2080_CTRL_INTERNAL_PERF_SYNC_GPU_BOOST_LIMITS_NUM];
} SLI_GPU_BOOST_SYNC;

/* ------------------------ Function Prototypes ---------------------------- */
NV_STATUS kperfGpuBoostSyncStateUpdate(OBJGPUBOOSTMGR *pBoostMgr, NvU32 boostGroupId, NvBool bActivate);
NV_STATUS kPerfGpuBoostSyncBridgelessUpdateInfo(OBJGPU *pGpu, NvBool bBridgeless);

#endif // KERNEL_PERF_GPU_BOOST_SYNC_H
