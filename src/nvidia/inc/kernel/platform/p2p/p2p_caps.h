/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef P2P_CAPS_H
#define P2P_CAPS_H

typedef enum
{
    P2P_CONNECTIVITY_UNKNOWN = 0,
    P2P_CONNECTIVITY_PCIE_PROPRIETARY,
    P2P_CONNECTIVITY_PCIE_BAR1,
    P2P_CONNECTIVITY_NVLINK,
    P2P_CONNECTIVITY_NVLINK_INDIRECT,
    P2P_CONNECTIVITY_C2C,
} P2P_CONNECTIVITY;

NV_STATUS p2pGetCaps       (NvU32 gpuMask, NvBool *pP2PWriteCapable, NvBool *pP2PReadCapable,
                            P2P_CONNECTIVITY *pConnectivity);
NV_STATUS p2pGetCapsStatus (NvU32 gpuMask, NvU8 *pP2PWriteCapStatus, NvU8 *pP2PReadCapStatus,
                            P2P_CONNECTIVITY *pConnectivity
                            );

#endif // P2P_CAPS_H
