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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef __NV_AMODEL_ENUM_H__
#define __NV_AMODEL_ENUM_H__



typedef enum _NVAModelConfig {
    NV_AMODEL_NONE = 0,
    NV_AMODEL_KEPLER,
    NV_AMODEL_KEPLER_SM35,
    NV_AMODEL_MAXWELL,
    NV_AMODEL_PASCAL,
    NV_AMODEL_VOLTA,
    NV_AMODEL_TURING,
    NV_AMODEL_AMPERE,
    NV_AMODEL_ADA,
    NV_AMODEL_HOPPER,
    NV_AMODEL_BLACKWELL,
} NVAModelConfig;

#endif /* __NV_AMODEL_ENUM_H__ */
