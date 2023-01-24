/*
 * SPDX-FileCopyrightText: Copyright (c) 2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "common_nvswitch.h"
#include "inforom/inforom_nvswitch.h"

NvlStatus
nvswitch_inforom_read_only_objects_load
(
    nvswitch_device *device
)
{
    NvlStatus status;
    struct inforom *pInforom = device->pInforom;

    if (pInforom == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    status = device->hal.nvswitch_inforom_load_obd(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Failed to load OBD object, rc:%d\n",
                    status);
    }
    else
    {
        pInforom->OBD.bValid = NV_TRUE;
    }

    status = nvswitch_inforom_load_object(device, pInforom, "OEM",
                                        INFOROM_OEM_OBJECT_V1_00_FMT,
                                        pInforom->OEM.packedObject,
                                        &pInforom->OEM.object);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Failed to load OEM object, rc:%d\n",
                    status);
    }
    else
    {
        pInforom->OEM.bValid = NV_TRUE;
    }

    status = nvswitch_inforom_load_object(device, pInforom, "IMG",
                                        INFOROM_IMG_OBJECT_V1_00_FMT,
                                        pInforom->IMG.packedObject,
                                        &pInforom->IMG.object);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Failed to load IMG object, rc:%d\n",
                    status);
    }
    else
    {
        pInforom->IMG.bValid = NV_TRUE;
    }

    return NVL_SUCCESS;
}
