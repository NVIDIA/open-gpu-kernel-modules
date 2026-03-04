/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef __NV_MIG_TYPES_H__
#define __NV_MIG_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

typedef NvU32 MIGDeviceId;

#define NO_MIG_DEVICE 0L

/* Convert a MIGDeviceId into a 0-based per-GPU subdevice index. */
#define MIG_DEVICE_ID_SUBDEV_MASK  0xf0000000
#define MIG_DEVICE_ID_SUBDEV_SHIFT 28

#define MIG_DEVICE_ID_TO_SUBDEV(migDeviceId) (((migDeviceId) & MIG_DEVICE_ID_SUBDEV_MASK) >> MIG_DEVICE_ID_SUBDEV_SHIFT)

#ifdef __cplusplus
}
#endif

#endif /* __NV_MIG_TYPES_H__ */
