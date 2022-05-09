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

#ifndef __NVKMS_PREALLOC_TYPES_H__
#define __NVKMS_PREALLOC_TYPES_H__

#include "nvtypes.h"

enum NVPreallocType {
    PREALLOC_TYPE_IMP_PARAMS,
    PREALLOC_TYPE_SHUT_DOWN_HEADS_SET_MODE,
    PREALLOC_TYPE_RESTORE_CONSOLE_SET_MODE,
    PREALLOC_TYPE_MODE_SET_WORK_AREA,
    PREALLOC_TYPE_FLIP_WORK_AREA,
    PREALLOC_TYPE_PROPOSED_MODESET_HW_STATE,
    PREALLOC_TYPE_VALIDATE_PROPOSED_MODESET_HW_STATE,
    PREALLOC_TYPE_VALIDATE_MODE_HW_MODE_TIMINGS,
    PREALLOC_TYPE_MAX
};

struct NVDevPreallocRec {
    void *ptr[PREALLOC_TYPE_MAX];
    NvU8 used[(PREALLOC_TYPE_MAX + 7) / 8];
};

#endif /* __NVKMS_PREALLOC_TYPES_H__ */
