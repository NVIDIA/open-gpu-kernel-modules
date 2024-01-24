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

#ifndef _OS_CAPABILITY_H_
#define _OS_CAPABILITY_H_

// OS specific RM capabilities structure
typedef struct OS_RM_CAPS OS_RM_CAPS;

// RM capabilities
#define NV_RM_CAP_SYS_BASE                      0x0
#define NV_RM_CAP_SYS_PROFILER_CONTEXT          (NV_RM_CAP_SYS_BASE + 0)
#define NV_RM_CAP_SYS_PROFILER_DEVICE           (NV_RM_CAP_SYS_BASE + 1)
#define NV_RM_CAP_SYS_SMC_CONFIG                (NV_RM_CAP_SYS_BASE + 2)
#define NV_RM_CAP_SYS_SMC_MONITOR               (NV_RM_CAP_SYS_BASE + 3)

#define NV_RM_CAP_SYS_FABRIC_IMEX_MGMT          (NV_RM_CAP_SYS_BASE + 4)

#define NV_RM_CAP_SMC_PARTITION_BASE            0x100
#define NV_RM_CAP_SMC_PARTITION_ACCESS          (NV_RM_CAP_SMC_PARTITION_BASE + 0)

#define NV_RM_CAP_EXT_BASE                      0x200
#define NV_RM_CAP_EXT_FABRIC_MGMT               (NV_RM_CAP_EXT_BASE + 0)

#define NV_RM_CAP_SMC_EXEC_PARTITION_BASE       0x300
#define NV_RM_CAP_SMC_EXEC_PARTITION_ACCESS     (NV_RM_CAP_SMC_EXEC_PARTITION_BASE + 0)

#endif
