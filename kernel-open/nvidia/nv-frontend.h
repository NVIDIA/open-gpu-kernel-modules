/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2013 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NV_FRONTEND_H_
#define _NV_FRONTEND_H_

#include "nvtypes.h"
#include "nv-linux.h"
#include "nv-register-module.h"

#define NV_MAX_MODULE_INSTANCES                 8

#define NV_FRONTEND_MINOR_NUMBER(x)             minor((x)->i_rdev)

#define NV_FRONTEND_CONTROL_DEVICE_MINOR_MAX    255
#define NV_FRONTEND_CONTROL_DEVICE_MINOR_MIN    (NV_FRONTEND_CONTROL_DEVICE_MINOR_MAX - \
                                                 NV_MAX_MODULE_INSTANCES)

#define NV_FRONTEND_IS_CONTROL_DEVICE(x)        ((x <= NV_FRONTEND_CONTROL_DEVICE_MINOR_MAX) && \
                                                 (x > NV_FRONTEND_CONTROL_DEVICE_MINOR_MIN))

int nvidia_frontend_add_device(nvidia_module_t *, nv_linux_state_t *);
int nvidia_frontend_remove_device(nvidia_module_t *, nv_linux_state_t *);

extern nvidia_module_t *nv_minor_num_table[];

#endif
