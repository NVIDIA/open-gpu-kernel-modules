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
nvswitch_inforom_ecc_load
(
    nvswitch_device *device
)
{
    NvlStatus status;
    NvU8 version = 0;
    NvU8 subversion = 0;
    INFOROM_ECC_STATE *pEccState = NULL;
    struct inforom *pInforom = device->pInforom;

    if (pInforom == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    status = nvswitch_inforom_get_object_version_info(device, "ECC", &version,
                                                    &subversion);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, WARN, "no ECC object found, rc:%d\n", status);
        return NVL_SUCCESS;
    }

    if (!INFOROM_OBJECT_SUBVERSION_SUPPORTS_NVSWITCH(subversion))
    {
        NVSWITCH_PRINT(device, WARN, "ECC v%u.%u not supported\n",
                    version, subversion);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    NVSWITCH_PRINT(device, INFO, "ECC v%u.%u found\n", version, subversion);

    pEccState = nvswitch_os_malloc(sizeof(INFOROM_ECC_STATE));
    if (pEccState == NULL)
    {
        return -NVL_NO_MEM;
    }
    nvswitch_os_memset(pEccState, 0, sizeof(INFOROM_ECC_STATE));

    switch (version)
    {
        case 6:
            pEccState->pFmt = INFOROM_ECC_OBJECT_V6_S0_FMT;
            pEccState->pPackedObject = nvswitch_os_malloc(INFOROM_ECC_OBJECT_V6_S0_PACKED_SIZE);
            if (pEccState->pPackedObject == NULL)
            {
                status = -NVL_NO_MEM;
                goto nvswitch_inforom_ecc_version_fail;
            }

            pEccState->pEcc = nvswitch_os_malloc(sizeof(INFOROM_ECC_OBJECT));
            if (pEccState->pEcc == NULL)
            {
                status = -NVL_NO_MEM;
                nvswitch_os_free(pEccState->pPackedObject);
                goto nvswitch_inforom_ecc_version_fail;
            }

            break;

        default:
            NVSWITCH_PRINT(device, WARN, "ECC v%u.%u not supported\n",
                        version, subversion);
            goto nvswitch_inforom_ecc_version_fail;
            break;
    }

    status = nvswitch_inforom_read_object(device, "ECC", pEccState->pFmt,
                                        pEccState->pPackedObject,
                                        pEccState->pEcc);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Failed to read ECC object, rc:%d\n", status);
        goto nvswitch_inforom_read_fail;
    }

    status = nvswitch_inforom_add_object(pInforom, &pEccState->pEcc->header);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Failed to cache ECC object header, rc:%d\n",
                    status);
        goto nvswitch_inforom_read_fail;
    }

    pInforom->pEccState = pEccState;

    // Update shared surface counts, non-fatal if we encounter a failure
    status = nvswitch_smbpbi_refresh_ecc_counts(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, WARN, "Failed to update ECC counts on SMBPBI "
                       "shared surface rc:%d\n", status);
    }

    return NVL_SUCCESS;

nvswitch_inforom_read_fail:
    nvswitch_os_free(pEccState->pPackedObject);
    nvswitch_os_free(pEccState->pEcc);
nvswitch_inforom_ecc_version_fail:
    nvswitch_os_free(pEccState);

    return status;
}

void
nvswitch_inforom_ecc_unload
(
    nvswitch_device *device
)
{
    INFOROM_ECC_STATE *pEccState;
    struct inforom *pInforom = device->pInforom;

    if (pInforom == NULL)
    {
        return;
    }

    pEccState = pInforom->pEccState;
    if (pEccState == NULL)
    {
        return;
    }

    //
    // Flush the data to InfoROM before unloading the object
    // Currently the driver doesn't support deferred processing and so the
    // error logging path in the interrupt handler cannot defer the flush.
    // This is WAR until the driver adds support for deferred processing
    //
    nvswitch_inforom_ecc_flush(device);

    nvswitch_os_free(pEccState->pPackedObject);
    nvswitch_os_free(pEccState->pEcc);
    nvswitch_os_free(pEccState);
    pInforom->pEccState = NULL;
}

NvlStatus
nvswitch_inforom_ecc_flush
(
    struct nvswitch_device *device
)
{
    NvlStatus status = NVL_SUCCESS;
    struct inforom *pInforom = device->pInforom;
    INFOROM_ECC_STATE *pEccState;

    if (pInforom == NULL || pInforom->pEccState == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    pEccState = pInforom->pEccState;

    if (pEccState->bDirty)
    {
        status = nvswitch_inforom_write_object(device, "ECC",
                                        pEccState->pFmt, pEccState->pEcc,
                                        pEccState->pPackedObject);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "Failed to flush ECC object to InfoROM, rc: %d\n", status);
        }
        else
        {
            pEccState->bDirty = NV_FALSE;
        }
    }

    return status;
}

NvlStatus
nvswitch_inforom_ecc_log_err_event
(
    struct nvswitch_device *device,
    INFOROM_NVS_ECC_ERROR_EVENT  *err_event
)
{
    NvlStatus status;
    INFOROM_ECC_STATE *pEccState;
    NvU64 time_ns;
    struct inforom *pInforom = device->pInforom;

    if (pInforom == NULL || pInforom->pEccState == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (err_event == NULL)
    {
        return -NVL_BAD_ARGS;
    }

    pEccState = pInforom->pEccState;

    time_ns = nvswitch_os_get_platform_time();
    err_event->timestamp = (NvU32)(time_ns / NVSWITCH_INTERVAL_1SEC_IN_NS);

    // Scrub the incoming address field if it is invalid
    if (!(err_event->bAddressValid))
    {
        err_event->address = 0;
    }

    // Invoke the chip dependent inforom logging routine
    status = device->hal.nvswitch_inforom_ecc_log_error_event(device, pEccState->pEcc,
                                                            err_event);
    if (status == NVL_SUCCESS)
    {
        //
        // If the error was logged successfully, mark the object as dirty to be
        // written on the subsequent flush.
        //
        pEccState->bDirty = NV_TRUE;
    }

    return status;
}

NvlStatus
nvswitch_inforom_ecc_get_errors
(
    nvswitch_device *device,
    NVSWITCH_GET_ECC_ERROR_COUNTS_PARAMS *params
)
{
    struct inforom *pInforom = device->pInforom;

    if (pInforom == NULL || pInforom->pEccState == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    return device->hal.nvswitch_inforom_ecc_get_errors(device, params);
}
