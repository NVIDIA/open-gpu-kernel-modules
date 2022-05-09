/*
 * Copyright (c) 2017, NVIDIA CORPORATION. All rights reserved.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef __NVIDIA_DRM_CONFTEST_H__
#define __NVIDIA_DRM_CONFTEST_H__

#include "conftest.h"

/*
 * NOTE: This file is expected to get included at the top before including any
 * of linux/drm headers.
 *
 * The goal is to redefine refcount_dec_and_test and refcount_inc before
 * including drm header files, so that the drm macro/inline calls to
 * refcount_dec_and_test* and refcount_inc get redirected to
 * alternate implementation in this file.
 */

#if NV_IS_EXPORT_SYMBOL_GPL_refcount_inc

#include <linux/refcount.h>

#define refcount_inc(__ptr)         \
    do {                            \
        atomic_inc(&(__ptr)->refs); \
    } while(0)

#endif

#if NV_IS_EXPORT_SYMBOL_GPL_refcount_dec_and_test

#include <linux/refcount.h>

#define refcount_dec_and_test(__ptr) atomic_dec_and_test(&(__ptr)->refs)

#endif

#if defined(NV_DRM_DRIVER_HAS_GEM_PRIME_RES_OBJ) || \
    defined(NV_DRM_GEM_OBJECT_HAS_RESV)
#define NV_DRM_FENCE_AVAILABLE
#else
#undef NV_DRM_FENCE_AVAILABLE
#endif

#endif /* defined(__NVIDIA_DRM_CONFTEST_H__) */
