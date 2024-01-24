/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _RMSLIGPUMGR_H_
#define _RMSLIGPUMGR_H_

NvBool      gpumgrAreGpusInitialized(NvU32 gpuMask);
void        gpumgrDetectSliLinkFromGpus(NvBool, NvU32, NvU32, NvU32 *, NvU32 *, NvBool *, NvU32 *, NvU32 *);
NvBool      gpumgrDetectHighSpeedVideoBridges(NvU32 linkOutputMask);
NvBool      gpumgrUpdateSliLinkRouting(OBJGPU *pGpuParent, OBJGPU *pGpuChild,
                                       NvU32 bridgeType,
                                       NvU32 parentPinsetIndex, NvU32 childPinsetIndex,
                                       NvBool bForceUpdate);
NV_STATUS   gpumgrPinsetToPinsetTableIndex(NvU32 pinset, NvU32 *pPinsetIndex);
NV_STATUS   gpumgrGetSliLinks(NV0000_CTRL_GPU_GET_VIDEO_LINKS_PARAMS *);

#define drPinSet_None   0x0
#define drPinSet_A      0x1
#define drPinSet_B      0x2
#define DR_PINSET_COUNT 2

//// Broadcast device defines
//
// These next defines are for the OBJGPUMGR.gpuSliLinkRoute array.
#define NV_SLILINK_ROUTE_INPUT                1:0
#define NV_SLILINK_ROUTE_INPUT_UNTESTED      0x00
#define NV_SLILINK_ROUTE_INPUT_IDENTITY      0x01
#define NV_SLILINK_ROUTE_INPUT_FAILED        0x02
#define NV_SLILINK_ROUTE_INPUT_PASSED        0x03
#define NV_SLILINK_ROUTE_OUTPUT               3:2
#define NV_SLILINK_ROUTE_OUTPUT_UNTESTED     0x00
#define NV_SLILINK_ROUTE_OUTPUT_IDENTITY     0x01
#define NV_SLILINK_ROUTE_OUTPUT_FAILED       0x02
#define NV_SLILINK_ROUTE_OUTPUT_PASSED       0x03
#define NV_SLILINK_ROUTE_INPUT_CHILD_PORT     7:4
#define NV_SLILINK_ROUTE_NVLINK               8:8
#define NV_SLILINK_ROUTE_NVLINK_TRUE         0x01
#define NV_SLILINK_ROUTE_NVLINK_FALSE        0x02

#endif //_RMSLIGPUMGR_H_
