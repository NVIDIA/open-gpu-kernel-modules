/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#ifndef _NV_CHARDEV_NUMBERS_H_
#define _NV_CHARDEV_NUMBERS_H_

// NVIDIA's reserved major character device number (Linux).
#define NV_MAJOR_DEVICE_NUMBER  195

// Minor numbers 0 to 247 reserved for regular devices
#define NV_MINOR_DEVICE_NUMBER_REGULAR_MAX         247

// Minor numbers 248 to 253 currently unused

// Minor number 254 reserved for the modeset device (provided by NVKMS)
#define NV_MINOR_DEVICE_NUMBER_MODESET_DEVICE      254

// Minor number 255 reserved for the control device
#define NV_MINOR_DEVICE_NUMBER_CONTROL_DEVICE      255

#endif  // _NV_CHARDEV_NUMBERS_H_

