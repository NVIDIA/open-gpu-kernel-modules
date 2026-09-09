/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
/*
 * This header file defines the types NvHdcpTopology for custom hdcp property
 */

#ifndef __NV_HDCP_TOPOLOGY_H__
#define __NV_HDCP_TOPOLOGY_H__

#include "nvtypes.h"

/*
 * HDCP Topology Data Structure
 *
 * This struct reflects the entire hdcp topology and various hdcp 
 * devices connected downstream to Tegra
 *
 */

#define HDCP_TOPOLOGY_MAX_LINK_COUNT (2)
#define HDCP_TOPOLOGY_MAX_DEV_COUNT  (255)
#define HDCP_TOPOLOGY_KSV_SIZE       (5)

struct NvHdcpTopology {
    NvBool isHdcpCapable;
    NvBool isHdcpAuthOn;
    NvBool isHdcpRp;
    NvBool isHdcp2X;
    NvBool maxCascadeExceeded;
    NvBool maxDeviceExceeded;
    NvBool isHdcp1DevDownstream;
    NvBool isHdcp2LegacyDevDownstream;
    NvU8   cascadeDepth;
    NvU8   linkCount;
    NvU8   bksv[HDCP_TOPOLOGY_MAX_LINK_COUNT * HDCP_TOPOLOGY_KSV_SIZE];
    NvU8   numOfBksv;
    NvU8   bksvList[HDCP_TOPOLOGY_MAX_DEV_COUNT * HDCP_TOPOLOGY_KSV_SIZE];
};

#endif /* __NV_HDCP_TOPOLOGY_H__ */
