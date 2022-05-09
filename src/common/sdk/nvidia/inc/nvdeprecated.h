/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2014 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef NV_DEPRECATED_H
#define NV_DEPRECATED_H

/*!
 * @file nvdeprecated.h
 *
 * @brief Deprecation in the NVIDIA SDK
 *
 * Why deprecate:
 *     Code is deprecated when you want to remove a feature entirely, but cannot
 *     do so immediately, nor in a single step, due to a requirement to remain
 *     backward compatible (keep older clients working).
 *
 * Backwards compatibility:
 *     Deprecated symbols and features may be supported for an unknown amount of
 *     time. "Deprecated" means that we want that time interval to be small, but
 *     that may not be under our control.
 *
 *     This file provides the following ways to support deprecated features:
 *
 *     1) Defining NV_STRICT_SDK before including a SDK headers. This will
 *        remove *all* deprecated APIs from the NV SDK, for example:
 *
 *        #define NV_STRICT_SDK
 *        #include "sdk/foo.h"
 *
 *     2) Defining the per-feature compatibility setting before including the
 *        SDK, for example:
 *
 *        #define NV_DEPRECATED_NVOS_STATUS   0   // enable compatibility mode
 *        #include "sdk/foo.h"
 *
 * How to deprecate a feature in the SDK:
 *
 *    1) Define the deprecated feature in this file. Often, you'll want to
 *       start with SDK compatibility enabled by default, for example:
 *
 *       #ifndef NV_DEPRECATED_FEATURE_NAME
 *       #define NV_DEPRECATED_FEATURE_NAME 0
 *       #endif
 *
 *    2) Wrap SDK definitions with compatibility #ifdefs:
 *
 *       #if NV_DEPRECATED_COMPAT(FEATURE_NAME)
 *           ...legacy definitions...
 *       #endif
 *
 *    3) In the API implementation, consider stubbing or wrapping the new API.
 *
 *    4) Update older clients: file bugs to track this effort. Bug numbers
 *       should be placed in comments near the deprecated features that RM is
 *       supporting. That way, people reading the code can easily find the
 *       bug(s) that show the status of completely removing the deprecated
 *       feature.
 *
 *    5) Once all the client (calling) code has been upgraded, change the
 *       macro to "compatibility off". This is a little more cautious and
 *       conservative than jumping directly to step (6), because it allows you
 *       to recover from a test failure (remember, there are extended, offline
 *       tests that are not, unfortunately, run in DVS, nor per-CL checkin)
 *       with a tiny change in code.
 *
 *    6) Once the code base has migrated, remove all definitions from the SDK.
 */

/*
 *  \defgroup Deprecated SDK Features
 *
 *            0 = Compatibility on by default (i.e.: defines present in SDK)
 *            1 = Compatibility off by default (i.e.: defines NOT in SDK)
 *
 *  @{
 */

/*!
 * RM Config Get/Set API is deprecated and RmControl should be used instead.
 * Bugs: XXXXXX, XXXXXX, etc
 */
#ifndef NV_DEPRECATED_RM_CONFIG_GET_SET
#define NV_DEPRECATED_RM_CONFIG_GET_SET     0
#endif

#ifndef NV_DEPRECATED_NVOS_STATUS
/* NVOS_STATUS codes is deprecated. NV_STATUS to be used instead */
#define NV_DEPRECATED_NVOS_STATUS           0
#endif

#ifndef NV_DEPRECATED_RM_STATUS
/* RM_STATUS codes is deprecated. NV_STATUS to be used instead */
#define NV_DEPRECATED_RM_STATUS           0
#endif

#ifndef NV_DEPRECATED_UNSAFE_HANDLES
/* Using NvU32 for handles is deprecated. NvHandle to be used instead */
#define NV_DEPRECATED_UNSAFE_HANDLES        0
#endif

/**@}*/

/*!
 *  Utility Macros
 */

#ifdef NV_STRICT_SDK
// In strict mode, all obsolete features are unavailable in the SDK.
#define NV_DEPRECATED_COMPAT(feature)       0
#else
#define NV_DEPRECATED_COMPAT(feature)       (!NV_DEPRECATED_##feature)
#endif

#endif
