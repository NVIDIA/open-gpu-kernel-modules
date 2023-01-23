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

#ifndef _NVBITMASK_H_
#define _NVBITMASK_H_

#include "kernel/gpu/gpu_engine_type.h"

//
// Engine Type capability mask bit-array helper MACROS to support on growing number of engine types
// The caps is defined as
//      NvU32 caps[NVGPU_ENGINE_CAPS_MASK_ARRAY_MAX]
//
#define NVGPU_ENGINE_CAPS_MASK_BITS                32
#define NVGPU_ENGINE_CAPS_MASK_ARRAY_MAX           ((RM_ENGINE_TYPE_LAST-1)/NVGPU_ENGINE_CAPS_MASK_BITS + 1)
#define NVGPU_GET_ENGINE_CAPS_MASK(caps, id)       (caps[(id)/NVGPU_ENGINE_CAPS_MASK_BITS] & NVBIT((id) % NVGPU_ENGINE_CAPS_MASK_BITS))
#define NVGPU_SET_ENGINE_CAPS_MASK(caps, id)       (caps[(id)/NVGPU_ENGINE_CAPS_MASK_BITS] |= NVBIT((id) % NVGPU_ENGINE_CAPS_MASK_BITS))

#endif //_NVBITMASK_H_
