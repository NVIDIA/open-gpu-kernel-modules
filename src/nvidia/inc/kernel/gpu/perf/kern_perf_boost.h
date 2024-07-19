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

#ifndef KERNEL_PERF_BOOST_H
#define KERNEL_PERF_BOOST_H

/* ------------------------ Includes --------------------------------------- */
#include "gpu/gpu_resource.h"
#include "gpu/timer/objtmr.h"

/* ------------------------ Macros ----------------------------------------- */
/*!
 * Maximum number of boost instances
 */
#define KERNEL_PERF_BOOST_HINT_COUNT  512

/*!
 * Index in the boost array reserved for all CUDA clients
 */
#define KERNEL_PERF_BOOST_HINT_CUDA_CLIENT_INDEX  (0x00000000)

/* ------------------------ Datatypes -------------------------------------- */
/*!
 * Structure that represents one request for the Boost Hint from a specific
 * client.
 */
typedef struct
{
    /*!
     * Specifices the handle of the client who requested this boost
     */
    NvHandle    hClient;

    /*!
     * Specifies the duration of the boost in seconds. This has to be less
     * than NV2080_CTRL_KERNEL_PERF_BOOST_DURATION_MAX.
     */
    NvU32       duration;

    /*!
     * Specifies the actual command. _CLEAR is to clear existing boost.
     * _BOOST_1LEVEL is to boost P-State one level higher. _BOOST_TO_MAX is to boost
     * to the highest P-State.
     *
     *  Note: _BOOST_1LEVEL is deprecated.
     */
    NvU32       flags;

    /*!
     * Specifies number of clients who requested
     * CUDA boost. This is used only in case of the CUDA clients.
     */
    NvU32       refCount;

    /*!
     * NV_TRUE if the boost duration is infinite, NV_FALSE otherwise.
     */
    NvBool      bIsInfinite;
} KERNEL_PERF_BOOST_HINT;

/*!
 * Structure that represents array of Boosts
 */
typedef struct
{
    /*!
     * One entry of this array corresponds one boost request
     */
    KERNEL_PERF_BOOST_HINT  boostHints[KERNEL_PERF_BOOST_HINT_COUNT];
} KERNEL_PERF_BOOST_HINTS;

/* -------------------- Function Prototypes -------------------------------- */
#endif // KERNEL_PERF_BOOST_H
