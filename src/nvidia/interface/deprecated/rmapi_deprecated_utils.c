/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "deprecated/rmapi_deprecated.h"

#include "class/cl0080.h" // NV01_DEVICE_0
#include "class/cl2080.h" // NV20_SUBDEVICE_0
#include "ctrl/ctrl0000/ctrl0000client.h" // NV0000_CTRL_CMD_CLIENT_GET_CHILD_HANDLE
#include "ctrl/ctrl0080/ctrl0080gpu.h" // NV0080_CTRL_CMD_GPU_FIND_SUBDEVICE_HANDLE
#include "nvos.h"

NV_STATUS
RmDeprecatedGetHandleParent
(
    DEPRECATED_CONTEXT *pContext,
    NvHandle            hClient,
    NvHandle            hObject,
    NvHandle           *phParent
)
{
    NV0000_CTRL_CLIENT_GET_HANDLE_INFO_PARAMS parentParams = {0};
    NV_STATUS                                 status;

    parentParams.hObject = hObject;
    parentParams.index = NV0000_CTRL_CMD_CLIENT_GET_HANDLE_INFO_INDEX_PARENT;

    status = pContext->RmControl(pContext, hClient, hClient, NV0000_CTRL_CMD_CLIENT_GET_HANDLE_INFO,
                                 &parentParams, sizeof(parentParams));

    *phParent = parentParams.data.hResult;

    return status;
}

NV_STATUS
RmDeprecatedGetClassID
(
    DEPRECATED_CONTEXT *pContext,
    NvHandle            hClient,
    NvHandle            hObject,
    NvU32              *pClassId
)
{
    NV0000_CTRL_CLIENT_GET_HANDLE_INFO_PARAMS classIdParams = {0};
    NV_STATUS                                 status;

    classIdParams.hObject = hObject;
    classIdParams.index = NV0000_CTRL_CMD_CLIENT_GET_HANDLE_INFO_INDEX_CLASSID;

    status = pContext->RmControl(pContext, hClient, hClient,
                                 NV0000_CTRL_CMD_CLIENT_GET_HANDLE_INFO,
                                 &classIdParams,
                                 sizeof(classIdParams));

    *pClassId = NvU64_LO32(classIdParams.data.iResult);

    return status;
}

NV_STATUS
RmDeprecatedFindOrCreateSubDeviceHandle
(
    DEPRECATED_CONTEXT *pContext,
    NvHandle            hClient,
    NvHandle            hDeviceOrSubDevice,
    NvHandle           *pHSubDevice,
    NvBool             *pBMustFree
)
{
    NV0080_CTRL_GPU_FIND_SUBDEVICE_HANDLE_PARAM findParams = {0};
    NV_STATUS                                   status;
    NvU32                                       classId;

    //
    // Step 1.) check if we already have a subdevice
    //
    status = RmDeprecatedGetClassID(pContext, hClient, hDeviceOrSubDevice, &classId);

    if (status != NV_OK)
        return status;

    if (classId == NV20_SUBDEVICE_0)
    {
        *pBMustFree = NV_FALSE;
        *pHSubDevice = hDeviceOrSubDevice;
        return NV_OK;
    }
    else if (classId != NV01_DEVICE_0)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // Step 2.) check if there is a subdevice allocated under this device
    //
    findParams.subDeviceInst = 0;

    status = pContext->RmControl(pContext, hClient, hDeviceOrSubDevice,
                                 NV0080_CTRL_CMD_GPU_FIND_SUBDEVICE_HANDLE,
                                 &findParams,
                                 sizeof(findParams));

    if (status == NV_OK && findParams.hSubDevice)
    {
        *pBMustFree = NV_FALSE;
        *pHSubDevice = findParams.hSubDevice;
        return status;
    }

    //
    // Step 3.) if there is no device, we temporarily allocate a subdevice.
    // Subdevice must be freed before we exit out to allow the client to reserve
    // it if it chooses to do so later on.
    //
    *pBMustFree = NV_TRUE;

    *pHSubDevice = 0;

    status = pContext->RmAlloc(pContext, hClient, hDeviceOrSubDevice, pHSubDevice, NV20_SUBDEVICE_0, 0);

    return status;
}

NV_STATUS RmDeprecatedGetOrAllocObject
(
    DEPRECATED_CONTEXT *pContext,
    NvHandle hClient,
    NvU32 classId,
    NvHandle *pHObject
)
{
    NV_STATUS status;

    NV0000_CTRL_CMD_CLIENT_GET_CHILD_HANDLE_PARAMS params = {0};
    params.hParent = *pHObject;
    params.classId = classId;
    status = pContext->RmControl(pContext, hClient, hClient,
                                 NV0000_CTRL_CMD_CLIENT_GET_CHILD_HANDLE,
                                 &params, sizeof(params));
    // Object already exists, just return it
    if (status == NV_OK && params.hObject != 0)
    {
        *pHObject = params.hObject;
    }
    else
    {
        // Object does not exist yet, allocate.
        void *pAllocParams = (void*)0; // TODO: Fill for classes that need them
        status = pContext->RmAlloc(pContext, hClient, *pHObject,
                                   pHObject, classId, pAllocParams);
    }
    return status;
}

NV_STATUS
RmDeprecatedConvertOs32ToOs02Flags
(
    NvU32 attr,
    NvU32 attr2,
    NvU32 os32Flags,
    NvU32 *pOs02Flags
)
{
    NvU32 os02Flags = 0;
    NV_STATUS rmStatus = NV_OK;

    switch (DRF_VAL(OS32, _ATTR, _PHYSICALITY, attr))
    {
        case NVOS32_ATTR_PHYSICALITY_DEFAULT: // NVOS02 defaults to contiguous.
        case NVOS32_ATTR_PHYSICALITY_CONTIGUOUS:
        {
            os02Flags = FLD_SET_DRF(OS02, _FLAGS, _PHYSICALITY, _CONTIGUOUS, os02Flags);
            break;
        }
        case NVOS32_ATTR_PHYSICALITY_NONCONTIGUOUS:
        {
            os02Flags = FLD_SET_DRF(OS02, _FLAGS, _PHYSICALITY, _NONCONTIGUOUS, os02Flags);
            break;
        }
        default:
        {
            rmStatus = NV_ERR_INVALID_FLAGS;
            break;
        }
    }

    switch (DRF_VAL(OS32, _ATTR, _LOCATION, attr))
    {
        case NVOS32_ATTR_LOCATION_PCI:
        case NVOS32_ATTR_LOCATION_ANY: // NVOS02 defaults to PCI
        {
            os02Flags = FLD_SET_DRF(OS02, _FLAGS, _LOCATION, _PCI, os02Flags);
            break;
        }
        case NVOS32_ATTR_LOCATION_AGP:
        {
            os02Flags = FLD_SET_DRF(OS02, _FLAGS, _LOCATION, _AGP, os02Flags);
            break;
        }
        case NVOS32_ATTR_LOCATION_VIDMEM:
        {
            os02Flags = FLD_SET_DRF(OS02, _FLAGS, _LOCATION, _VIDMEM, os02Flags);
            break;
        }
        default:
        {
            rmStatus = NV_ERR_INVALID_FLAGS;
            break;
        }
    }

    switch (DRF_VAL(OS32, _ATTR, _COHERENCY, attr))
    {
        case NVOS32_ATTR_COHERENCY_UNCACHED:
        {
            os02Flags = FLD_SET_DRF(OS02, _FLAGS, _COHERENCY, _UNCACHED, os02Flags);
            break;
        }
        case NVOS32_ATTR_COHERENCY_CACHED:
        {
            os02Flags = FLD_SET_DRF(OS02, _FLAGS, _COHERENCY, _CACHED, os02Flags);
            break;
        }
        case NVOS32_ATTR_COHERENCY_WRITE_COMBINE:
        {
            os02Flags = FLD_SET_DRF(OS02, _FLAGS, _COHERENCY, _WRITE_COMBINE, os02Flags);
            break;
        }
        case NVOS32_ATTR_COHERENCY_WRITE_THROUGH:
        {
            os02Flags = FLD_SET_DRF(OS02, _FLAGS, _COHERENCY, _WRITE_THROUGH, os02Flags);
            break;
        }
        case NVOS32_ATTR_COHERENCY_WRITE_PROTECT:
        {
            os02Flags = FLD_SET_DRF(OS02, _FLAGS, _COHERENCY, _WRITE_PROTECT, os02Flags);
            break;
        }
        case NVOS32_ATTR_COHERENCY_WRITE_BACK:
        {
            os02Flags = FLD_SET_DRF(OS02, _FLAGS, _COHERENCY, _WRITE_BACK, os02Flags);
            break;
        }
        default:
        {
            rmStatus = NV_ERR_INVALID_FLAGS;
            break;
        }
    }

    switch (DRF_VAL(OS32, _ATTR2, _GPU_CACHEABLE, attr2))
    {
        case NVOS32_ATTR2_GPU_CACHEABLE_YES:
        {
            os02Flags = FLD_SET_DRF(OS02, _FLAGS, _GPU_CACHEABLE, _YES, os02Flags);
            break;
        }
        case NVOS32_ATTR2_GPU_CACHEABLE_DEFAULT: // NVOS02 defaults to non-cacheable
        case NVOS32_ATTR2_GPU_CACHEABLE_NO:
        {
            os02Flags = FLD_SET_DRF(OS02, _FLAGS, _GPU_CACHEABLE, _NO, os02Flags);
            break;
        }
        default:
        {
            rmStatus = NV_ERR_INVALID_FLAGS;
            break;
        }
    }

    // VidHeapControl never creates a mapping
    os02Flags = FLD_SET_DRF(OS02, _FLAGS, _MAPPING, _NO_MAP, os02Flags);
    if (os32Flags & NVOS32_ALLOC_FLAGS_KERNEL_MAPPING_MAP)
        os02Flags = FLD_SET_DRF(OS02, _FLAGS, _KERNEL_MAPPING, _MAP, os02Flags);
    else
        os02Flags = FLD_SET_DRF(OS02, _FLAGS, _KERNEL_MAPPING, _NO_MAP, os02Flags);

    if (FLD_TEST_DRF(OS32, _ATTR2, _PROTECTION_USER, _READ_ONLY, attr2))
        os02Flags = FLD_SET_DRF(OS02, _FLAGS, _ALLOC_USER_READ_ONLY, _YES, os02Flags);

    if (FLD_TEST_DRF(OS32, _ATTR2, _PROTECTION_DEVICE, _READ_ONLY, attr2))
        os02Flags = FLD_SET_DRF(OS02, _FLAGS, _ALLOC_DEVICE_READ_ONLY, _YES, os02Flags);

    if (FLD_TEST_DRF(OS32, _ATTR2, _NISO_DISPLAY, _YES, attr2))
        os02Flags = FLD_SET_DRF(OS02, _FLAGS, _ALLOC_NISO_DISPLAY, _YES, os02Flags);
    else
        os02Flags = FLD_SET_DRF(OS02, _FLAGS, _ALLOC_NISO_DISPLAY, _NO, os02Flags);

    if (rmStatus == NV_OK)
    {
        *pOs02Flags = os02Flags;
    }

    return rmStatus;
}

NV_STATUS
RmDeprecatedConvertOs02ToOs32Flags
(
    NvU32 os02Flags,
    NvU32 *pAttr,
    NvU32 *pAttr2,
    NvU32 *pOs32Flags
)
{
    NvU32 os32Flags = 0;
    NvU32 attr = 0, attr2 = 0;
    NV_STATUS rmStatus = NV_OK;

    attr = FLD_SET_DRF(OS32, _ATTR, _PAGE_SIZE, _DEFAULT, attr);

    if (FLD_TEST_DRF(OS02, _FLAGS, _PHYSICALITY, _CONTIGUOUS, os02Flags))
        attr = FLD_SET_DRF(OS32, _ATTR, _PHYSICALITY, _CONTIGUOUS, attr);
    else
        attr = FLD_SET_DRF(OS32, _ATTR, _PHYSICALITY, _NONCONTIGUOUS, attr);

    switch (DRF_VAL(OS02, _FLAGS, _LOCATION, os02Flags))
    {
        case NVOS02_FLAGS_LOCATION_PCI:
        {
            attr = FLD_SET_DRF(OS32, _ATTR, _LOCATION, _PCI, attr);
            break;
        }
        case NVOS02_FLAGS_LOCATION_VIDMEM:
        {
            attr = FLD_SET_DRF(OS32, _ATTR, _LOCATION, _VIDMEM, attr);
            break;
        }
        default:
        {
            rmStatus = NV_ERR_INVALID_FLAGS;
            break;
        }
    }

    switch (DRF_VAL(OS02, _FLAGS, _COHERENCY, os02Flags))
    {
        case NVOS02_FLAGS_COHERENCY_UNCACHED:
        {
            attr = FLD_SET_DRF(OS32, _ATTR, _COHERENCY, _UNCACHED, attr);
            break;
        }
        case NVOS02_FLAGS_COHERENCY_CACHED:
        {
            attr = FLD_SET_DRF(OS32, _ATTR, _COHERENCY, _CACHED, attr);
            break;
        }
        case NVOS02_FLAGS_COHERENCY_WRITE_COMBINE:
        {
            attr = FLD_SET_DRF(OS32, _ATTR, _COHERENCY, _WRITE_COMBINE, attr);
            break;
        }
        case NVOS02_FLAGS_COHERENCY_WRITE_THROUGH:
        {
            attr = FLD_SET_DRF(OS32, _ATTR, _COHERENCY, _WRITE_THROUGH, attr);
            break;
        }
        case NVOS02_FLAGS_COHERENCY_WRITE_PROTECT:
        {
            attr = FLD_SET_DRF(OS32, _ATTR, _COHERENCY, _WRITE_PROTECT, attr);
            break;
        }
        case NVOS02_FLAGS_COHERENCY_WRITE_BACK:
        {
            attr = FLD_SET_DRF(OS32, _ATTR, _COHERENCY, _WRITE_BACK, attr);
            break;
        }
        default:
        {
            rmStatus = NV_ERR_INVALID_FLAGS;
            break;
        }
    }

    if (FLD_TEST_DRF(OS02, _FLAGS, _GPU_CACHEABLE, _YES, os02Flags))
        attr2 |= DRF_DEF(OS32, _ATTR2, _GPU_CACHEABLE, _YES);
    else
        attr2 |= DRF_DEF(OS32, _ATTR2, _GPU_CACHEABLE, _NO);

    if (FLD_TEST_DRF(OS02, _FLAGS, _KERNEL_MAPPING, _MAP, os02Flags))
        os32Flags |= NVOS32_ALLOC_FLAGS_KERNEL_MAPPING_MAP;

    if (FLD_TEST_DRF(OS02, _FLAGS, _ALLOC_NISO_DISPLAY, _YES, os02Flags))
        attr2 |= DRF_DEF(OS32, _ATTR2, _NISO_DISPLAY, _YES);
    else
        attr2 |= DRF_DEF(OS32, _ATTR2, _NISO_DISPLAY, _NO);

    if (FLD_TEST_DRF(OS02, _FLAGS, _ALLOC_USER_READ_ONLY, _YES, os02Flags))
        attr2 |= DRF_DEF(OS32, _ATTR2, _PROTECTION_USER, _READ_ONLY);

    if (FLD_TEST_DRF(OS02, _FLAGS, _ALLOC_DEVICE_READ_ONLY, _YES, os02Flags))
        attr2 |= DRF_DEF(OS32, _ATTR2, _PROTECTION_DEVICE, _READ_ONLY);

    if (rmStatus == NV_OK)
    {
        *pOs32Flags = os32Flags;
        *pAttr = attr;
        *pAttr2 = attr2;
    }

    return rmStatus;
}
