/*
 * SPDX-FileCopyrightText: Copyright (c) 2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef _NVIDIA_HEADSURFACE_CONSTANTS_H_
#define _NVIDIA_HEADSURFACE_CONSTANTS_H_

/* Possible values for NvHsFragmentUniforms::resamplingMethod */
#define NVIDIA_HEADSURFACE_RESAMPLING_METHOD_BICUBIC_TRIANGULAR            1
#define NVIDIA_HEADSURFACE_RESAMPLING_METHOD_BICUBIC_BELL_SHAPED           2
#define NVIDIA_HEADSURFACE_RESAMPLING_METHOD_BICUBIC_BSPLINE               3
#define NVIDIA_HEADSURFACE_RESAMPLING_METHOD_BICUBIC_ADAPTIVE_TRIANGULAR   4
#define NVIDIA_HEADSURFACE_RESAMPLING_METHOD_BICUBIC_ADAPTIVE_BELL_SHAPED  5
#define NVIDIA_HEADSURFACE_RESAMPLING_METHOD_BICUBIC_ADAPTIVE_BSPLINE      6

/* Uniform sampler binding indices */
#define NVIDIA_HEADSURFACE_UNIFORM_SAMPLER_BINDING_PRIMARY_TEX             0
#define NVIDIA_HEADSURFACE_UNIFORM_SAMPLER_BINDING_CURSOR_TEX              1
#define NVIDIA_HEADSURFACE_UNIFORM_SAMPLER_BINDING_BLEND_TEX               2
#define NVIDIA_HEADSURFACE_UNIFORM_SAMPLER_BINDING_OFFSET_TEX              3
#define NVIDIA_HEADSURFACE_UNIFORM_SAMPLER_BINDING_OVERLAY_TEX             4
#define NVIDIA_HEADSURFACE_UNIFORM_SAMPLER_BINDING_LUT_TEX                 5
#define NVIDIA_HEADSURFACE_UNIFORM_SAMPLER_BINDING_NUM                     6

#endif /* _NVIDIA_HEADSURFACE_CONSTANTS_H_ */
