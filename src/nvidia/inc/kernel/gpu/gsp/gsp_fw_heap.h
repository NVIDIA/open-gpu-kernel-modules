/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef GSP_FW_HEAP_H
#define GSP_FW_HEAP_H

// Static defines for the GSP FW WPR Heap
#define GSP_FW_HEAP_PARAM_OS_SIZE_LIBOS2                   (0 << 20)   // No FB heap usage
#define GSP_FW_HEAP_PARAM_OS_SIZE_LIBOS3                  (20 << 20)

//
// Calibrated by observing RM init heap usage - the amount of GSP-RM heap memory
// used during GSP-RM boot and Kernel RM initialization, up to and including the
// first client subdevice allocation.
//
#define GSP_FW_HEAP_PARAM_BASE_RM_SIZE_TU10X               (8 << 20)   // Turing thru Ada
#define GSP_FW_HEAP_PARAM_BASE_RM_SIZE_GH100              (14 << 20)   // Hopper+

//
// Calibrated by observing RM init heap usage on GPUs with various FB sizes.
// This seems to fit the data fairly well, but is likely inaccurate (differences
// in heap usage are more likely correlate with GPU architecture than FB size).
// TODO: this requires more detailed profiling and tuning.
//
#define GSP_FW_HEAP_PARAM_SIZE_PER_GB_FB                  (96 << 10)   // All architectures

//
// This number is calibrated by profiling the WPR heap usage of a single
// client channel allocation. In practice, channel allocations on newer
// architectures are around 44KB-46KB (depending on internal memory
// tracking overhead configured in GSP-RM).
//
// Note: Turing supports 4096 channels vs 2048 on Ampere+, but the heap is
// sized to support only 2048 channels - otherwise the Turing heap balloons
// to 176MB+ on already small-FB parts.
//
// Note: The 2048 channel limit is per-runlist. GSP-RM currently uses only
// a single ChID space across all engines, because per-runlist channel RAM is
// disabled by default in most configurations. If/once per-runlist ChID spaces
// are used on GSP-RM, this will likely no longer be sufficient/relevant as
// the memory that could be needed for client channel allocations will far
// exceed the amount we can carve out for GSP-RM's heap (max ChID will no longer
// be the limiter).
//
#define GSP_FW_HEAP_PARAM_CLIENT_ALLOC_SIZE      ((48 << 10) * 2048)   // Support 2048 channels

#define GSP_FW_HEAP_SIZE_VGPU_DEFAULT                    (549 << 20)   // Default for all VGPU configs

// Min/max bounds for heap size override by regkey
#define GSP_FW_HEAP_SIZE_OVERRIDE_LIBOS2_MIN_MB                (64u)
#define GSP_FW_HEAP_SIZE_OVERRIDE_LIBOS2_MAX_MB               (256u)

#define GSP_FW_HEAP_SIZE_OVERRIDE_LIBOS3_BAREMETAL_MIN_MB      (84u)
#define GSP_FW_HEAP_SIZE_OVERRIDE_LIBOS3_BAREMETAL_MAX_MB     (276u)

#define GSP_FW_HEAP_SIZE_OVERRIDE_LIBOS3_VGPU_MIN_MB          (549u)
#define GSP_FW_HEAP_SIZE_OVERRIDE_LIBOS3_VGPU_MAX_MB         (1024u)

#endif // GSP_FW_HEAP_H
