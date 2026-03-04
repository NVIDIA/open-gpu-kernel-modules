/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef SBIOS_TABLE_VERSION_H
#define SBIOS_TABLE_VERSION_H

#define CONTROLLER_SBIOS_TABLE_VERSION_10                   (0x10)
#define CONTROLLER_SBIOS_TABLE_VERSION_20                   (0x20)
#define CONTROLLER_SBIOS_TABLE_VERSION_21                   (0x21)
#define CONTROLLER_SBIOS_TABLE_VERSION_22                   (0x22)
#define CONTROLLER_SBIOS_TABLE_VERSION_23                   (0x23)
#define CONTROLLER_SBIOS_TABLE_VERSION_24                   (0x24)
#define CONTROLLER_SBIOS_TABLE_VERSION_25                   (0x25)
#define CONTROLLER_SBIOS_TABLE_MAX_ENTRIES                  (8)

// NOTE: When adding a new version, make sure to update MAX_VERSION accordingly.
#define CONTROLLER_SBIOS_TABLE_MAX_VERSION                  (0x25)

/*!
 * Layout of Controller 2x data used for static config
 */
#define NVPCF_CONTROLLER_STATIC_TABLE_VERSION_20                          (0x20)
#define NVPCF_CONTROLLER_STATIC_TABLE_VERSION_21                          (0x21)
#define NVPCF_CONTROLLER_STATIC_TABLE_VERSION_22                          (0x22)
#define NVPCF_CONTROLLER_STATIC_TABLE_VERSION_23                          (0x23)
#define NVPCF_CONTROLLER_STATIC_TABLE_VERSION_24                          (0x24)
#define NVPCF_CONTROLLER_STATIC_TABLE_VERSION_25                          (0x25)
#define NVPCF_CONTROLLER_STATIC_TABLE_MAX_ENTRIES                         (8)

// NOTE: When adding a new version, make sure to update MAX_VERSION accordingly.
#define NVPCF_CONTROLLER_STATIC_TABLE_MAX_VERSION                         (0x25)

#endif // SBIOS_TABLE_VERSION_H
