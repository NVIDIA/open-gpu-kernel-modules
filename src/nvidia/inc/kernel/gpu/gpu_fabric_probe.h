/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef GPU_FABRIC_PROBE_H
#define GPU_FABRIC_PROBE_H


#include "nvlink_inband_msg.h"

#define GPU_FABRIC_PROBE_SEC_TO_NS 1000000000ULL

#define GPU_FABRIC_PROBE_DEFAULT_DELAY 5 // 5 seconds

#define GPU_FABRIC_PROBE_DEFAULT_PROBE_SLOWDOWN_THRESHOLD 10

typedef struct GPU_FABRIC_PROBE_INFO_KERNEL GPU_FABRIC_PROBE_INFO_KERNEL;

NV_STATUS gpuFabricProbeStart(OBJGPU *pGpu,
                              GPU_FABRIC_PROBE_INFO_KERNEL **ppGpuFabricProbeInfoKernel);
void gpuFabricProbeStop(GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel);

void gpuFabricProbeSuspend(GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel);
NV_STATUS gpuFabricProbeResume(GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel);

NV_STATUS gpuFabricProbeGetGpuFabricHandle(GPU_FABRIC_PROBE_INFO_KERNEL *pInfo, NvU64 *pHandle);
NV_STATUS gpuFabricProbeGetGfId(GPU_FABRIC_PROBE_INFO_KERNEL *pInfo, NvU32 *pGfId);
NV_STATUS gpuFabricProbeGetfmCaps(GPU_FABRIC_PROBE_INFO_KERNEL *pInfo, NvU64 *pFmCaps);
NV_STATUS gpuFabricProbeGetClusterUuid(GPU_FABRIC_PROBE_INFO_KERNEL *pInfo, NvUuid *pClusterUuid);
NV_STATUS gpuFabricProbeGetFabricPartitionId(GPU_FABRIC_PROBE_INFO_KERNEL *pInfo, NvU16 *pFabricPartitionId);
NV_STATUS gpuFabricProbeGetGpaAddress(GPU_FABRIC_PROBE_INFO_KERNEL *pInfo, NvU64 *pGpaAddress);
NV_STATUS gpuFabricProbeGetGpaAddressRange(GPU_FABRIC_PROBE_INFO_KERNEL *pInfo, NvU64 *pGpaAddressRange);
NV_STATUS gpuFabricProbeGetFlaAddress(GPU_FABRIC_PROBE_INFO_KERNEL *pInfo, NvU64 *pFlaAddress);
NV_STATUS gpuFabricProbeGetFlaAddressRange(GPU_FABRIC_PROBE_INFO_KERNEL *pInfo, NvU64 *pFlaAddressRange);
NV_STATUS gpuFabricProbeGetNumProbeReqs(GPU_FABRIC_PROBE_INFO_KERNEL *pInfo, NvU64 *numProbes);

NvBool gpuFabricProbeIsReceived(GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel);
NvBool gpuFabricProbeIsSuccess(GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel);
NV_STATUS gpuFabricProbeGetFmStatus(GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel);
NvBool gpuFabricProbeIsSupported(OBJGPU *pGpu);
NV_STATUS gpuFabricProbeSetBwMode(NvU8 mode);
NV_STATUS gpuFabricProbeGetlinkMaskToBeReduced(GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
                                               NvU32 *linkMaskToBeReduced);
#endif // GPU_FABRIC_PROBE_H
