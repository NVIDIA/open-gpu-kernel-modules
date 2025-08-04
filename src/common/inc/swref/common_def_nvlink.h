/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef COMMON_DEF_NVLINK_H
#define COMMON_DEF_NVLINK_H

//
// Arch CONNECTION defines, replaces forceconfig. See Bugs 1665737,
// 1665734 and 1734252.
// This per link connection state is passed up from chiplib
// and can be controlled on the command line.
// The max number of connections is speced in __SIZE_1.
//
#define NV_NVLINK_ARCH_CONNECTION                         31:0
#define NV_NVLINK_ARCH_CONNECTION__SIZE_1                   32
#define NV_NVLINK_ARCH_CONNECTION_DISABLED          0x00000000
#define NV_NVLINK_ARCH_CONNECTION_PEER_MASK                7:0
#define NV_NVLINK_ARCH_CONNECTION_ENABLED                  8:8
#define NV_NVLINK_ARCH_CONNECTION_PHYSICAL_LINK          21:16
#define NV_NVLINK_ARCH_CONNECTION_RESERVED               29:20
#define NV_NVLINK_ARCH_CONNECTION_PEERS_COMPUTE_ONLY     30:30
#define NV_NVLINK_ARCH_CONNECTION_CPU                    31:31

#endif // COMMON_DEF_NVLINK_H
