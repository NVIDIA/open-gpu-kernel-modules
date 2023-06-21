/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef GPU_CHILD_CLASS_DEFS_H
#define GPU_CHILD_CLASS_DEFS_H

/*!
 * @file
 * @details Provides the class definitions for every GPU child class without the
 *          need to include every individual header.
 */

#include "core/prelude.h"

#define GPU_CHILD_CLASS_DEFS_GPU_CHILD(className, accessorName, numInstances, bConstructEarly, gpuField) \
    extern const struct NVOC_CLASS_DEF NV_CONCATENATE(__nvoc_class_def_, className);

#define GPU_CHILD \
    GPU_CHILD_CLASS_DEFS_GPU_CHILD
#include "gpu/gpu_child_list.h"

// Sub-classes of GPU children
// Pmu sub-classes
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Pmu10;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Pmu20;

#endif // GPU_CHILD_CLASS_DEFS_H
