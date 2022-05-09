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

#ifndef _NV_UNIX_NVOS_PARAMS_WRAPPERS_H_
#define _NV_UNIX_NVOS_PARAMS_WRAPPERS_H_

#include <nvos.h>

/*
 * This is a wrapper for NVOS02_PARAMETERS with file descriptor
 */

typedef struct
{
    NVOS02_PARAMETERS params;
    int fd;
} nv_ioctl_nvos02_parameters_with_fd;

/*
 * This is a wrapper for NVOS33_PARAMETERS with file descriptor
 */
typedef struct
{
    NVOS33_PARAMETERS params;
    int fd;
} nv_ioctl_nvos33_parameters_with_fd;

#endif // _NV_UNIX_NVOS_PARAMS_WRAPPERS_H_

