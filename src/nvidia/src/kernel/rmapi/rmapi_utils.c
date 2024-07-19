/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "nvoc/rtti.h"

#include "gpu/gpu.h"
#include "gpu_mgr/gpu_mgr.h"

#include "class/cl0080.h"
#include "class/cl2080.h"

#include "ctrl/ctrl0080.h"
#include "ctrl/ctrl2080.h"
#include "ctrl/ctrl402c.h"
#include "ctrl/ctrl90cc.h"
#include "ctrl/ctrl90e7/ctrl90e7bbx.h"

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
                                &hClient,
                                sizeof(hClient)),
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
                                &nv0080AllocParams,
                                sizeof(nv0080AllocParams)),
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
                                &nv2080AllocParams,
                                sizeof(nv2080AllocParams)),
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

NV_STATUS
rmapiutilGetControlInfo
(
    NvU32 cmd,
    NvU32 *pFlags,
    NvU32 *pAccessRight,
    NvU32 *pParamsSize
)
{
    RS_RESOURCE_DESC *pResourceDesc = RsResInfoByExternalClassId(DRF_VAL(XXXX, _CTRL_CMD, _CLASS, cmd));

    if (pResourceDesc != NULL)
    {
        struct NVOC_CLASS_DEF *pClassDef = (void*)pResourceDesc->pClassInfo;
        if (pClassDef != NULL)
        {
            const struct NVOC_EXPORTED_METHOD_DEF *pMethodDef =
                nvocGetExportedMethodDefFromMethodInfo_IMPL(pClassDef->pExportInfo, cmd);

            if (pMethodDef != NULL)
            {
                if (pFlags != NULL)
                    *pFlags = pMethodDef->flags;

                if (pAccessRight != NULL)
                    *pAccessRight = pMethodDef->accessRight;

                if (pParamsSize != NULL)
                    *pParamsSize = pMethodDef->paramSize;

                return NV_OK;
            }
        }
    }

    return NV_ERR_OBJECT_NOT_FOUND;
}

NvBool rmapiutilSkipErrorMessageForUnsupportedVgpuGuestControl(NvU32 cmd)
{
    switch (cmd)
    {
        case NV2080_CTRL_CMD_GPU_GET_OEM_BOARD_INFO:
        case NV2080_CTRL_CMD_GPU_GET_INFOROM_IMAGE_VERSION:
        case NV2080_CTRL_CMD_GPU_GET_INFOROM_OBJECT_VERSION:
        case NV2080_CTRL_CMD_GPU_GET_RESET_STATUS:
        case NV2080_CTRL_CMD_GPU_GET_DRAIN_AND_RESET_STATUS:
        case NV2080_CTRL_CMD_BUS_GET_PEX_COUNTERS:
        case NV2080_CTRL_CMD_FB_GET_OFFLINED_PAGES:
        case NV2080_CTRL_CMD_FB_GET_REMAPPED_ROWS:
        case NV2080_CTRL_CMD_ECC_GET_VOLATILE_COUNTS:
        case NV2080_CTRL_CMD_GPU_QUERY_INFOROM_ECC_SUPPORT:
        case NV0080_CTRL_CMD_FB_GET_COMPBIT_STORE_INFO:
        case NV2080_CTRL_CMD_BUS_CLEAR_PEX_COUNTERS:
        case NV2080_CTRL_CMD_HSHUB_GET_AVAILABLE_MASK:
        case NV2080_CTRL_CMD_I2C_WRITE_REG:
        case NV2080_CTRL_CMD_I2C_READ_BUFFER:
        case NV2080_CTRL_CMD_I2C_READ_REG:
        case NV2080_CTRL_CMD_FB_SETUP_VPR_REGION:
        case NV2080_CTRL_CMD_I2C_ACCESS:
        case NV90CC_CTRL_CMD_POWER_REQUEST_FEATURES:
        case NV90CC_CTRL_CMD_POWER_RELEASE_FEATURES:
        case NV2080_CTRL_CMD_FLCN_USTREAMER_QUEUE_INFO:
        case NV2080_CTRL_CMD_INTERNAL_NVLINK_ENABLE_COMPUTE_PEER_ADDR:
        case NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_CTRL:
        case NV2080_CTRL_CMD_BIOS_GET_NBSI_V2:
        case NV2080_CTRL_CMD_BIOS_GET_UEFI_SUPPORT:
        case NV2080_CTRL_CMD_BUS_GET_PCIE_LTR_LATENCY:
        case NV2080_CTRL_CMD_BUS_SET_PCIE_LTR_LATENCY:
        case NV2080_CTRL_CMD_BUS_SET_PCIE_SPEED:
        case NV2080_CTRL_CMD_FB_GET_CALIBRATION_LOCK_FAILED:
        case NV2080_CTRL_CMD_GPU_GET_ILLUM:
        case NV2080_CTRL_CMD_GPU_GET_VPR_CAPS:
        case NV2080_CTRL_CMD_GPU_QUERY_FUNCTION_STATUS:
        case NV2080_CTRL_CMD_GPU_SET_ILLUM:
        case NV2080_CTRL_CMD_LPWR_DIFR_CTRL:
        case NV2080_CTRL_CMD_PMGR_GET_MODULE_INFO:
        case NV402C_CTRL_CMD_I2C_GET_PORT_SPEED:
        case NV90E7_CTRL_CMD_BBX_GET_LAST_FLUSH_TIME:
            return NV_TRUE;

        default:
            return NV_FALSE;
    }
}

