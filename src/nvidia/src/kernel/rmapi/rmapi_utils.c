/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "rmapi/rmapi_utils.h"
#include "rmapi/rs_utils.h"
#include "resource_desc.h"

#include "gpu/gpu.h"
#include "gpu_mgr/gpu_mgr.h"

#include "class/cl0080.h"
#include "class/cl2080.h"

NV_STATUS
rmapiutilAllocClientAndDeviceHandles
(
    RM_API   *pRmApi,
    OBJGPU   *pGpu,
    NvHandle *phClient,
    NvHandle *phDevice,
    NvHandle *phSubDevice
)
{
    NV_STATUS rmStatus;
    NV0080_ALLOC_PARAMETERS nv0080AllocParams;
    NV2080_ALLOC_PARAMETERS nv2080AllocParams;
    NvHandle hClient = NV01_NULL_OBJECT;
    NvHandle hDevice = NV01_NULL_OBJECT;
    NvHandle hSubDevice = NV01_NULL_OBJECT;

    NV_ASSERT_OR_RETURN(phClient != NULL, NV_ERR_INVALID_ARGUMENT);

    // Allocate a client
    NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
        pRmApi->AllocWithHandle(pRmApi,
                                NV01_NULL_OBJECT,
                                NV01_NULL_OBJECT,
                                NV01_NULL_OBJECT,
                                NV01_ROOT,
                                &hClient),
        cleanup);

    // Allocate a device
    NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
        serverutilGenResourceHandle(hClient, &hDevice),
        cleanup);

    portMemSet(&nv0080AllocParams, 0, sizeof(nv0080AllocParams));
    nv0080AllocParams.deviceId = gpuGetDeviceInstance(pGpu);
    nv0080AllocParams.hClientShare = hClient;

    NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
        pRmApi->AllocWithHandle(pRmApi,
                                hClient,
                                hClient,
                                hDevice,
                                NV01_DEVICE_0,
                                &nv0080AllocParams),
        cleanup);

    // Allocate a subDevice
    NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
        serverutilGenResourceHandle(hClient, &hSubDevice),
        cleanup);

    portMemSet(&nv2080AllocParams, 0, sizeof(nv2080AllocParams));
    nv2080AllocParams.subDeviceId = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

    NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
        pRmApi->AllocWithHandle(pRmApi,
                                hClient,
                                hDevice,
                                hSubDevice,
                                NV20_SUBDEVICE_0,
                                &nv2080AllocParams),
        cleanup);

    *phClient = hClient;
    if (phDevice != NULL)
        *phDevice = hDevice;
    if (phSubDevice != NULL)
        *phSubDevice = hSubDevice;

    return rmStatus;

cleanup:
    rmapiutilFreeClientAndDeviceHandles(pRmApi, &hClient, &hDevice, &hSubDevice);
    return rmStatus;
}

void
rmapiutilFreeClientAndDeviceHandles
(
    RM_API   *pRmApi,
    NvHandle *phClient,
    NvHandle *phDevice,
    NvHandle *phSubDevice
)
{
    NV_ASSERT_OR_RETURN_VOID(phClient != NULL);
    NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR, *phClient != NV01_NULL_OBJECT);

    if (phSubDevice != NULL && *phSubDevice != NV01_NULL_OBJECT)
    {
        pRmApi->Free(pRmApi, *phClient, *phSubDevice);
        *phSubDevice = NV01_NULL_OBJECT;
    }

    if (phDevice != NULL && *phDevice != NV01_NULL_OBJECT)
    {
        pRmApi->Free(pRmApi, *phClient, *phDevice);
        *phDevice = NV01_NULL_OBJECT;
    }

    pRmApi->Free(pRmApi, *phClient, *phClient);
    *phClient = NV01_NULL_OBJECT;
}

NvBool
rmapiutilIsExternalClassIdInternalOnly
(
    NvU32 externalClassId
)
{
    RS_RESOURCE_DESC *pResDesc = RsResInfoByExternalClassId(externalClassId);
    NV_ASSERT_OR_RETURN(pResDesc != NULL, NV_FALSE);
    return (pResDesc->flags & RS_FLAGS_INTERNAL_ONLY) != 0x0;
}
