/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef NVGSP_PROXY_REG_H
#define NVGSP_PROXY_REG_H

#define NVGSP_PROXY_REG_CONFIDENTIAL_COMPUTE                        0:0
#define NVGSP_PROXY_REG_CONFIDENTIAL_COMPUTE_DISABLE                0x00000000
#define NVGSP_PROXY_REG_CONFIDENTIAL_COMPUTE_ENABLE                 0x00000001

#define NVGSP_PROXY_REG_CONF_COMPUTE_EARLY_INIT                     1:1
#define NVGSP_PROXY_REG_CONF_COMPUTE_EARLY_INIT_DISABLE             0x00000000
#define NVGSP_PROXY_REG_CONF_COMPUTE_EARLY_INIT_ENABLE              0x00000001

#define NVGSP_PROXY_REG_CONF_COMPUTE_DEV_MODE                       2:2
#define NVGSP_PROXY_REG_CONF_COMPUTE_DEV_MODE_DISABLE               0x00000000
#define NVGSP_PROXY_REG_CONF_COMPUTE_DEV_MODE_ENABLE                0x00000001

#define NVGSP_PROXY_REG_CONF_COMPUTE_MULTI_GPU_MODE                 5:3
#define NVGSP_PROXY_REG_CONF_COMPUTE_MULTI_GPU_MODE_NONE            0x00000000
#define NVGSP_PROXY_REG_CONF_COMPUTE_MULTI_GPU_MODE_PROTECTED_PCIE  0x00000001
#define NVGSP_PROXY_REG_CONF_COMPUTE_MULTI_GPU_MODE_NVLE            0x00000002

#define NVGSP_PROXY_REG_NVLINK_ENCRYPTION                           6:6
#define NVGSP_PROXY_REG_NVLINK_ENCRYPTION_DISABLE                   0x00000000
#define NVGSP_PROXY_REG_NVLINK_ENCRYPTION_ENABLE                    0x00000001

#endif // NVGSP_PROXY_REG_H
