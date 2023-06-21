/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "bios_nvswitch.h"
#include "error_nvswitch.h"
#include "rmsoecmdif.h"
#include "nvswitch/lr10/dev_ext_devices.h"

#include "flcn/flcn_nvswitch.h"

#include "rmflcncmdif_nvswitch.h"
#include "haldef_nvswitch.h"

static NvlStatus
_nvswitch_core_bios_read
(
    nvswitch_device *device,
    NvU8 readType,
    NvU32 reqSize,
    NvU8 *pData
)
{
#define MAX_READ_SIZE 0x2000
    RM_FLCN_CMD_SOE     cmd;
    NVSWITCH_TIMEOUT    timeout;
    NvU32               cmdSeqDesc = 0;
    NV_STATUS           status;
    FLCN               *pFlcn = NULL;
    RM_SOE_CORE_CMD_BIOS *pParams = &cmd.cmd.core.bios;
    NvU64 dmaHandle = 0;
    NvU8 *pReadBuffer = NULL;
    NvU32 offset = 0;
    NvU32 bufferSize = (reqSize < SOE_DMA_MAX_SIZE) ? SOE_DMA_MAX_SIZE : MAX_READ_SIZE;

    // Create DMA mapping for SOE CORE transactions
    status = nvswitch_os_alloc_contig_memory(device->os_handle,
                 (void**)&pReadBuffer, bufferSize, (device->dma_addr_width == 32));
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Failed to allocate contig memory\n");
        return status;
    }

    status = nvswitch_os_map_dma_region(device->os_handle,
                                        pReadBuffer,
                                        &dmaHandle,
                                        bufferSize,
                                        NVSWITCH_DMA_DIR_TO_SYSMEM);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Failed to map dma region to sysmem\n");
        nvswitch_os_free_contig_memory(device->os_handle, pReadBuffer, bufferSize);
        return status;
    }

    pFlcn = device->pSoe->pFlcn;

    while (offset < reqSize)
    {
        nvswitch_os_memset(pReadBuffer, 0, bufferSize);
        nvswitch_os_memset(&cmd, 0, sizeof(cmd));

        cmd.hdr.unitId = RM_SOE_UNIT_CORE;
        cmd.hdr.size   = RM_SOE_CMD_SIZE(CORE, BIOS);
        cmd.cmd.core.bios.cmdType = readType;
        RM_FLCN_U64_PACK(&pParams->dmaHandle, &dmaHandle);
        pParams->offset = offset;
        pParams->sizeInBytes = NV_MIN((reqSize - offset), MAX_READ_SIZE);
        cmdSeqDesc = 0;

        status = nvswitch_os_sync_dma_region_for_device(device->os_handle, dmaHandle,
                        bufferSize, NVSWITCH_DMA_DIR_TO_SYSMEM);
        if (status != NV_OK)
        {
            nvswitch_os_unmap_dma_region(device->os_handle, pReadBuffer, dmaHandle,
                    bufferSize, NVSWITCH_DMA_DIR_TO_SYSMEM);
            nvswitch_os_free_contig_memory(device->os_handle, pReadBuffer, bufferSize);
            NVSWITCH_PRINT(device, ERROR, "Failed to yield to DMA controller\n");
            return status;
        }

        nvswitch_timeout_create(NVSWITCH_INTERVAL_1SEC_IN_NS * 4, &timeout);

        status = flcnQueueCmdPostBlocking(device, pFlcn,
                                    (PRM_FLCN_CMD)&cmd,
                                    NULL,   // pMsg             - not used for now
                                    NULL,   // pPayload         - not used for now
                                    SOE_RM_CMDQ_LOG_ID,
                                    &cmdSeqDesc,
                                    &timeout);
        if (status != NV_OK)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: CORE read failed. rc:%d\n",
                            __FUNCTION__, status);
            break;
        }

        status = nvswitch_os_sync_dma_region_for_cpu(device->os_handle, dmaHandle,
                            bufferSize, NVSWITCH_DMA_DIR_TO_SYSMEM);
        if (status != NV_OK)
        {
            NVSWITCH_PRINT(device, ERROR, "DMA controller failed to yield back\n");
            break;
        }

        if (readType == RM_SOE_CORE_CMD_READ_BIOS)
        {
            nvswitch_os_memcpy(((NvU8*)&pData[offset]), pReadBuffer, pParams->sizeInBytes);
        }
        else if (readType == RM_SOE_CORE_CMD_READ_BIOS_SIZE)
        {
            nvswitch_os_memcpy(((NvU8*)pData), pReadBuffer, reqSize);
            break;
        }

        offset += pParams->sizeInBytes;
    }

    nvswitch_os_unmap_dma_region(device->os_handle, pReadBuffer, dmaHandle,
        bufferSize, NVSWITCH_DMA_DIR_TO_SYSMEM);

    nvswitch_os_free_contig_memory(device->os_handle, pReadBuffer, bufferSize);
    return status;
}

NvlStatus
nvswitch_bios_read
(
    nvswitch_device *device,
    NvU32 size,
    void *pData
)
{
    return _nvswitch_core_bios_read(device, RM_SOE_CORE_CMD_READ_BIOS, size, (NvU8*)pData);
}

NvlStatus
nvswitch_bios_read_size
(
    nvswitch_device *device,
    NvU32 *pSize
)
{
    if (pSize == NULL)
    {
        return -NVL_BAD_ARGS;
    }

    return _nvswitch_core_bios_read(device, RM_SOE_CORE_CMD_READ_BIOS_SIZE, sizeof(NvU32), (NvU8*)pSize);
}

/*!
 * @brief Retrieves BIOS Image via SOE's CORE task
 * This function needs SOE to be initialized for the Util task to respond.
 * Upon success the BIOS Image will be place in device.biosImage
 * @param[in/out] device - pointer to the nvswitch device.
 */
NvlStatus
nvswitch_bios_get_image
(
    nvswitch_device *device
)
{
    NvU8 *pBiosRawBuffer = NULL;
    NvlStatus status = NVL_SUCCESS;
    NvU32 biosSize = 0;

    if (device->biosImage.pImage != NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
                    "NVRM: %s: bios already available, skip reading"
                    "\n", __FUNCTION__);

        return NVL_SUCCESS;
    }

    if (!device->pSoe)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: SOE not initialized yet. \n",
                __FUNCTION__);
        return NVL_SUCCESS;
    }

    status = device->hal.nvswitch_get_bios_size(device, &biosSize);
    if (status != NVL_SUCCESS || biosSize == 0)
    {
        NVSWITCH_PRINT(device, ERROR,
                    "NVRM: %s: bios read size failed"
                    "\n", __FUNCTION__);
        return status;
    }

    NVSWITCH_PRINT(device, SETUP,
                    "NVRM: %s: BIOS Size = 0x%x"
                    "\n", __FUNCTION__, biosSize);

    pBiosRawBuffer = (NvU8*) nvswitch_os_malloc(biosSize);
    if (pBiosRawBuffer == NULL)
    {
            NVSWITCH_PRINT(device, ERROR,
                    "%s : failed memory allocation"
                    "\n", __FUNCTION__);

        return -NVL_NO_MEM;
    }

    nvswitch_os_memset(pBiosRawBuffer, 0, biosSize);

    status = nvswitch_bios_read(device, biosSize, pBiosRawBuffer);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, " Failed to retrieve BIOS image, Code 0x%x\n", status);
        nvswitch_os_free(pBiosRawBuffer);
        return status;
    }

    device->biosImage.pImage = pBiosRawBuffer;
    device->biosImage.size   = biosSize;

    return NVL_SUCCESS;
}
