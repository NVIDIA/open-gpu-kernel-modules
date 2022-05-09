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
#include "error_nvswitch.h"

#include "inforom/inforom_nvswitch.h"

NvlStatus
nvswitch_inforom_oms_get_device_disable
(
    nvswitch_device *device,
    NvBool *pBDisabled
)
{
    struct inforom *pInforom = device->pInforom;
    INFOROM_OMS_STATE *pOmsState;

    if (pInforom == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    pOmsState = pInforom->pOmsState;
    if (pOmsState == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    *pBDisabled = device->hal.nvswitch_oms_get_device_disable(pOmsState);

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_inforom_oms_set_device_disable
(
    nvswitch_device *device,
    NvBool bForceDeviceDisable
)
{
    struct inforom *pInforom = device->pInforom;
    INFOROM_OMS_STATE *pOmsState;

    if (pInforom == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    pOmsState = pInforom->pOmsState;
    if (pOmsState == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    device->hal.nvswitch_oms_set_device_disable(pOmsState, bForceDeviceDisable);

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_inforom_oms_load
(
    nvswitch_device *device
)
{
    NvlStatus status;
    NvU8 version = 0;
    NvU8 subversion = 0;
    INFOROM_OMS_STATE *pOmsState = NULL;
    struct inforom *pInforom = device->pInforom;

    if (pInforom == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    status = nvswitch_inforom_get_object_version_info(device, "OMS", &version,
                                                    &subversion);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, INFO, "no OMS object found, rc:%d\n", status);
        return NVL_SUCCESS;
    }

    if (!INFOROM_OBJECT_SUBVERSION_SUPPORTS_NVSWITCH(subversion))
    {
        NVSWITCH_PRINT(device, WARN, "OMS v%u.%u not supported\n",
                    version, subversion);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    NVSWITCH_PRINT(device, INFO, "OMS v%u.%u found\n", version, subversion);

    pOmsState = nvswitch_os_malloc(sizeof(INFOROM_OMS_STATE));
    if (pOmsState == NULL)
    {
        return -NVL_NO_MEM;
    }
    nvswitch_os_memset(pOmsState, 0, sizeof(INFOROM_OMS_STATE));

    switch (version)
    {
        case 1:
            pOmsState->pFmt = INFOROM_OMS_OBJECT_V1S_FMT;
            pOmsState->pPackedObject = nvswitch_os_malloc(INFOROM_OMS_OBJECT_V1_PACKED_SIZE);
            if (pOmsState->pPackedObject == NULL)
            {
                status = -NVL_NO_MEM;
                goto nvswitch_inforom_oms_version_fail;
            }

            pOmsState->pOms = nvswitch_os_malloc(sizeof(INFOROM_OMS_OBJECT));
            if (pOmsState->pOms == NULL)
            {
                status = -NVL_NO_MEM;
                nvswitch_os_free(pOmsState->pPackedObject);
                goto nvswitch_inforom_oms_version_fail;
            }

            break;

        default:
            NVSWITCH_PRINT(device, WARN, "OMS v%u.%u not supported\n",
                        version, subversion);
            goto nvswitch_inforom_oms_version_fail;
            break;
    }

    status = nvswitch_inforom_load_object(device, pInforom, "OMS",
                                        pOmsState->pFmt,
                                        pOmsState->pPackedObject,
                                        pOmsState->pOms);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Failed to load OMS object, rc: %d\n",
                        status);
        goto nvswitch_inforom_oms_load_fail;
    }

    pInforom->pOmsState = pOmsState;

    device->hal.nvswitch_initialize_oms_state(device, pOmsState);

    return NVL_SUCCESS;

nvswitch_inforom_oms_load_fail:
    nvswitch_os_free(pOmsState->pOms);
    nvswitch_os_free(pOmsState->pPackedObject);
nvswitch_inforom_oms_version_fail:
    nvswitch_os_free(pOmsState);

    return status;
}

void
nvswitch_inforom_oms_unload
(
    nvswitch_device *device
)
{
    struct inforom *pInforom = device->pInforom;
    INFOROM_OMS_STATE *pOmsState;
    NvlStatus status;

    if (pInforom == NULL)
    {
        return;
    }

    pOmsState = pInforom->pOmsState;
    if (pOmsState == NULL)
    {
        return;
    }

    (void)device->hal.nvswitch_read_oob_blacklist_state(device);
    status = device->hal.nvswitch_oms_inforom_flush(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
                    "Flushing OMS failed during unload, rc:%d\n", status);
    }

    nvswitch_os_free(pOmsState->pPackedObject);
    nvswitch_os_free(pOmsState->pOms);
    nvswitch_os_free(pOmsState);
    pInforom->pOmsState = NULL;
}
