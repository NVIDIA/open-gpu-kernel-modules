/*
 * SPDX-FileCopyrightText: Copyright (c) 2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#ifndef _NVLINK_LOCK_H_
#define _NVLINK_LOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvlink_common.h"

/*
 * Allocate top level lock. Return NVL_SUCCESS if 
 * the lock was allocated else return NVL_ERR_GENERIC.
 */
NvlStatus nvlink_lib_top_lock_alloc(void);

/*
 * Free top level lock. Return NVL_SUCCESS if 
 * the lock was freed else return NVL_ERR_GENERIC.
 */
NvlStatus nvlink_lib_top_lock_free(void);

/*
 * Allocate per-link lock. Return NVL_SUCCESS if 
 * the lock was allocated else return NVL_ERR_GENERIC.
 */
NvlStatus nvlink_lib_link_lock_alloc(nvlink_link *link);

/*
 * Free per-link lock. Return NVL_SUCCESS if 
 * the lock was freed else return NVL_ERR_GENERIC.
 */
NvlStatus nvlink_lib_link_lock_free(nvlink_link *link);

/*
 * Acquire top level lock. Return NVL_SUCCESS if 
 * the lock was acquired else return NVL_ERR_STATE_IN_USE.
 */
NvlStatus nvlink_lib_top_lock_acquire(void);

/*
 * Release top level lock. Return NVL_SUCCESS if 
 * the lock was released else return NVL_ERR_GENERIC.
 */
NvlStatus nvlink_lib_top_lock_release(void);

/*
 * Sort the array of links in order of (DBDF, link#) - 
 * lowest to highest and acquire link locks.
 * Return NVL_SUCCESS if all the link locks were acquired.
 * Else if any link lock failed to be acquired, release 
 * all acquired link locks and return NVL_ERR_STATE_IN_USE.
 */
NvlStatus nvlink_lib_link_locks_acquire(nvlink_link **links, int numLinks);

/*
 * Loop over all the links and call nvlink_releaseLock(links[i]->linkLock).
 * Return NVL_SUCCESS if all the link locks were released.
 * Else if any link lock failed to be released return NVL_ERR_GENERIC.
 */
NvlStatus nvlink_lib_link_locks_release(nvlink_link **links, int numLinks);

#ifdef __cplusplus
}
#endif

#endif // _NVLINK_LOCK_H_
