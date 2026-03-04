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

#include "nvlink_common.h"
#include "common_nvswitch.h"
#include "error_nvswitch.h"
#include "haldef_nvswitch.h"

#include "inforom/inforom_nvswitch.h"

#include "soe/soeififr.h"
#include "rmsoecmdif.h"
#include "flcn/flcn_nvswitch.h"
#include "rmflcncmdif_nvswitch.h"

// Interface functions
static NvlStatus _nvswitch_inforom_unpack_object(const char *, NvU8 *, NvU32 *);
static NvlStatus _nvswitch_inforom_pack_object(const char *, NvU32 *, NvU8 *);
static NvlStatus _nvswitch_inforom_read_file(nvswitch_device *device,
                                            const char objectName[INFOROM_FS_FILE_NAME_SIZE],
                                            NvU32 packedObjectSize, NvU8 *pPackedObject);
static NvlStatus _nvswitch_inforom_write_file(nvswitch_device *device,
                                            const char objectName[INFOROM_FS_FILE_NAME_SIZE],
                                            NvU32 packedObjectSize,
                                            NvU8 *pPackedObject);

static NvlStatus
_nvswitch_inforom_calc_packed_object_size
(
    const char *objectFormat,
    NvU16      *pPackedObjectSize
)
{
    NvU16 count;
    char type;
    NvU16 packedObjectSize = 0;

    while ((type = *objectFormat++) != '\0')
    {
        count = 0;
        while ((type >= '0') && (type <= '9'))
        {
            count *= 10;
            count += (type - '0');
            type = *objectFormat++;
        }
        count = (count > 0) ? count : 1;

        switch (type)
        {
            case INFOROM_FMT_S08:
                packedObjectSize += count;
                break;
            case INFOROM_FMT_U04:
                if (count % 2)
                    return -NVL_ERR_INVALID_STATE;
                packedObjectSize += (count / 2);
                break;
            case INFOROM_FMT_U08:
                packedObjectSize += count;
                break;
            case INFOROM_FMT_U16:
                packedObjectSize += (count * 2);
                break;
            case INFOROM_FMT_U24:
                packedObjectSize += (count * 3);
                break;
            case INFOROM_FMT_U32:
                packedObjectSize += (count * 4);
                break;
            case INFOROM_FMT_U64:
                packedObjectSize += (count * 8);
                break;
            case INFOROM_FMT_BINARY:
                packedObjectSize += count;
                break;
            default:
                return -NVL_BAD_ARGS;
        }
    }

    *pPackedObjectSize = packedObjectSize;

    return NVL_SUCCESS;
}

static NV_INLINE void
_nvswitch_inforom_unpack_uint_field
(
    NvU8  **ppPackedObject,
    NvU32 **ppObject,
    NvU8    width
)
{
    NvU8 i;
    NvU64 field = 0;

    if (width > 8)
    {
        return;
    }

    for (i = 0; i < width; i++, (*ppPackedObject)++)
    {
        field |= (((NvU64)**ppPackedObject) << (8 * i));
    }

    if (width <= 4)
    {
        **ppObject = (NvU32)field;
        (*ppObject)++;
    }
    else
    {
        **(NvU64 **)ppObject = field;
        *ppObject += 2;
    }
}

static NvlStatus
_nvswitch_inforom_unpack_object
(
    const char *objectFormat,
    NvU8       *pPackedObject,
    NvU32      *pObject
)
{
    NvU16 count;
    char type;
    NvU64 field;

    while ((type = *objectFormat++) != '\0')
    {
        count = 0;
        while ((type >= '0') && (type <= '9'))
        {
            count *= 10;
            count += (type - '0');
            type = *objectFormat++;
        }
        count = (count > 0) ? count : 1;

        for (; count > 0; count--)
        {
            switch (type)
            {
                case INFOROM_FMT_S08:
                    field = *pPackedObject++;
                    field |= ((field & 0x80) ? ~0xff : 0);
                    *pObject++ = (NvU32)field;
                    break;
                case INFOROM_FMT_U04:
                    // Extract two nibbles per byte, and adjust count accordingly
                    if (count % 2)
                        return -NVL_ERR_INVALID_STATE;
                    field = *pPackedObject++;
                    *pObject++ = (NvU32)(field & 0x0f);
                    *pObject++ = (NvU32)((field & 0xf0) >> 4);
                    count--;
                    break;
                case INFOROM_FMT_U08:
                    _nvswitch_inforom_unpack_uint_field(&pPackedObject, &pObject, 1);
                    break;
                case INFOROM_FMT_U16:
                    _nvswitch_inforom_unpack_uint_field(&pPackedObject, &pObject, 2);
                    break;
                case INFOROM_FMT_U24:
                    _nvswitch_inforom_unpack_uint_field(&pPackedObject, &pObject, 3);
                    break;
                case INFOROM_FMT_U32:
                    _nvswitch_inforom_unpack_uint_field(&pPackedObject, &pObject, 4);
                    break;
                case INFOROM_FMT_U64:
                    _nvswitch_inforom_unpack_uint_field(&pPackedObject, &pObject, 8);
                    break;
                case INFOROM_FMT_BINARY:
                    nvswitch_os_memcpy(pObject, pPackedObject, count);
                    pObject += NV_CEIL(count, 4);
                    pPackedObject += count;
                    // Adjust count to exit the loop.
                    count = 1;
                    break;
                default:
                    return -NVL_BAD_ARGS;
            }
        }
    }

    return NVL_SUCCESS;
}

static NV_INLINE void
_nvswitch_inforom_pack_uint_field
(
    NvU8  **ppPackedObject,
    NvU32 **ppObject,
    NvU8    width
)
{
    NvU8 i;
    NvU64 field = (width <= 4) ? **ppObject : **((NvU64 **)ppObject);

    if (width > 8)
    {
        return;
    }

    for (i = 0; i < width; i++, (*ppPackedObject)++)
    {
        **ppPackedObject = (NvU8)((field >> (8 * i)) & 0xff);
    }

    if (width <= 4)
    {
        (*ppObject)++;
    }
    else
    {
        *ppObject += 2;
    }
}

static NvlStatus
_nvswitch_inforom_pack_object
(
    const char *objectFormat,
    NvU32      *pObject,
    NvU8       *pPackedObject
)
{
    NvU16 count;
    char type;
    NvU64 field;

    while ((type = *objectFormat++) != '\0')
    {
        count = 0;
        while ((type >= '0') && (type <= '9'))
        {
            count *= 10;
            count += (type - '0');
            type = *objectFormat++;
        }
        count = (count > 0) ? count : 1;

        for (; count > 0; count--)
        {
            switch (type)
            {
                case INFOROM_FMT_S08:
                    field = *pObject++;
                    *pPackedObject++ = (NvS8)field;
                    break;
                case INFOROM_FMT_U04:
                    // Encode two nibbles per byte, and adjust count accordingly
                    if (count % 2)
                        return -NVL_ERR_INVALID_STATE;
                    field = (*pObject++) & 0xf;
                    field |= (((*pObject++) & 0xf) << 4);
                    *pPackedObject++ = (NvU8)field;
                    count--;
                    break;
                case INFOROM_FMT_U08:
                    _nvswitch_inforom_pack_uint_field(&pPackedObject, &pObject, 1);
                    break;
                case INFOROM_FMT_U16:
                    _nvswitch_inforom_pack_uint_field(&pPackedObject, &pObject, 2);
                    break;
                case INFOROM_FMT_U24:
                    _nvswitch_inforom_pack_uint_field(&pPackedObject, &pObject, 3);
                    break;
                case INFOROM_FMT_U32:
                    _nvswitch_inforom_pack_uint_field(&pPackedObject, &pObject, 4);
                    break;
                case INFOROM_FMT_U64:
                    _nvswitch_inforom_pack_uint_field(&pPackedObject, &pObject, 8);
                    break;
                case INFOROM_FMT_BINARY:
                    nvswitch_os_memcpy(pPackedObject, pObject, count);
                    pObject += NV_CEIL(count, 4);
                    pPackedObject += count;
                    // Adjust count to exit the loop.
                    count = 1;
                    break;
                default:
                    return -NVL_BAD_ARGS;
            }
        }
    }

    return NVL_SUCCESS;
}

/*!
 * Interface to copy string of inforom object.
 * inforom_U008 is NvU32, and we use 0xff bits to store the character.
 * Therefore we need a special copy API.
 *
 * @param[in]       pSrc          Source pointer
 * @param[out]      pDst          Destination pointer
 * @param[in]       length        Length of the string
 */
void
nvswitch_inforom_string_copy
(
    inforom_U008   *pSrc,
    NvU8           *pDst,
    NvU32           length
)
{
    NvU32 i;

    for (i = 0; i < length; ++i)
    {
        pDst[i] = (NvU8)(pSrc[i] & 0xff);
    }
}

/*!
 * Read and unpack an object from the InfoROM filesystem.
 *
 * @param[in]  device           switch device pointer
 * @param[in]  pInforom         INFOROM object pointer
 * @param[in]  objectName       Name of the object to read from the InfoROM
 * @param[in]  pObjectFormat    Ascii-string describing the layout of the
 *                              object to read. Used to calculate the packed
 *                              object size and to unpack the data.
 * @param[out] pPackedObject    Written with the packed object read from the
 *                              InfoROM. It is assumed that this is large
 *                              enough to hold the packed data size computed
 *                              from the pObjectFormat string. This argument
 *                              cannot be NULL.
 * @param[out] pObject          Written with the unpacked object read from the
 *                              InfoROM. It is assumed that this is large
 *                              enough to hold the unpacked data size computed
 *                              from the pObjectFormat string. This argument
 *                              may be NULL.
 *
 * @return NVL_SUCCESS
 *      Object successfully read, and unpacked if necessary
 * @return -NVL_BAD_ARGS
 *      If one of the required pointer arguments is NULL
 * @return -NVL_ERR_NOT_SUPPORTED
 *      The InfoROM filesystem image is not supported
 * @return Other error
 *      If packed size determination fails, object unpacking fails, or there
 *      is a filesystem adapter failure in reading any packed data, it may
 *      result in other error values.
 */

NvlStatus
nvswitch_inforom_read_object
(
    nvswitch_device *device,
    const char   objectName[3],
    const char  *pObjectFormat,
    NvU8        *pPackedObject,
    void        *pObject
)
{
    struct inforom      *pInforom = device->pInforom;
    NvlStatus           status;
    NvU16               packedSize;
    NvU16               fileSize;

    if (pInforom == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    status = _nvswitch_inforom_calc_packed_object_size(pObjectFormat, &packedSize);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    status = _nvswitch_inforom_read_file(device, objectName, packedSize, pPackedObject);

    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "InfoROM FS read for %c%c%c failed! rc:%d\n",
                        objectName[0], objectName[1], objectName[2], status);
        return status;
    }

    //
    // Verify a couple things about the object data:
    //  1. The size in the header matches the calculated packed size.
    //  2. The type is as it was expected
    //
    fileSize = INFOROM_FS_FILE_SIZE(pPackedObject);
    if (packedSize != fileSize)
    {
        NVSWITCH_PRINT(device, ERROR,
                    "NVRM: %s: object %c%c%c was found, but discarded due to "
                    "a size mismatch! (Expected = 0x%X bytes, Actual = 0x%X "
                    "bytes)\n", __FUNCTION__,
                    objectName[0], objectName[1], objectName[2],
                    packedSize, fileSize);
        return -NVL_ERR_INVALID_STATE;
    }

    if (!INFOROM_FS_FILE_NAMES_MATCH(pPackedObject, objectName))
    {
        NVSWITCH_PRINT(device, ERROR,
                    "NVRM: %s: object %c%c%c was found, but discarded due to "
                    "a type mismatch in the header!\n", __FUNCTION__,
                    objectName[0], objectName[1], objectName[2]);
        return -NVL_ERR_INVALID_STATE;
    }

    if (pObject != NULL)
    {
        status = _nvswitch_inforom_unpack_object(pObjectFormat, pPackedObject, pObject);
        if (status != NVL_SUCCESS)
        {
            return status;
        }
    }

    return status;
}

static NvlStatus
_nvswitch_inforom_read_file
(
    nvswitch_device *device,
    const char objectName[INFOROM_FS_FILE_NAME_SIZE],
    NvU32 packedObjectSize,
    NvU8 *pPackedObject
)
{
    NvlStatus status = NVL_SUCCESS;
    void *pDmaBuf;
    NvU64 dmaHandle;
    NvU32 fsRet;
    FLCN *pFlcn = device->pSoe->pFlcn;
    RM_FLCN_CMD_SOE soeCmd;
    RM_SOE_IFR_CMD *pIfrCmd = &soeCmd.cmd.ifr;
    RM_SOE_IFR_CMD_PARAMS *pParams = &pIfrCmd->params;
    NvU32 cmdSeqDesc;
    NVSWITCH_TIMEOUT timeout;
    // The first 4 bytes are reserved for status/debug data from SOE
    NvU32 transferSize = packedObjectSize + sizeof(NvU32);

    status = nvswitch_os_alloc_contig_memory(device->os_handle, &pDmaBuf, transferSize,
                                            (device->dma_addr_width == 32));
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to allocate contig memory\n", __FUNCTION__);
        return status;
    }

    status = nvswitch_os_map_dma_region(device->os_handle, pDmaBuf, &dmaHandle,
                                        transferSize, NVSWITCH_DMA_DIR_TO_SYSMEM);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to map DMA region\n", __FUNCTION__);
        goto ifr_dma_free_and_exit;
    }

    nvswitch_os_memset(&soeCmd, 0, sizeof(soeCmd));
    soeCmd.hdr.unitId = RM_SOE_UNIT_IFR;
    soeCmd.hdr.size = sizeof(soeCmd);
    pIfrCmd->cmdType = RM_SOE_IFR_READ;

    RM_FLCN_U64_PACK(&pParams->dmaHandle, &dmaHandle);
    nvswitch_os_memcpy(pParams->fileName, objectName, INFOROM_FS_FILE_NAME_SIZE);
    pParams->offset = 0;
    pParams->sizeInBytes = packedObjectSize;

    //SOE will copy entire file into SYSMEM
    nvswitch_os_memset(pDmaBuf, 0, transferSize);

    cmdSeqDesc = 0;
    nvswitch_timeout_create(NVSWITCH_INTERVAL_4SEC_IN_NS, &timeout);
    status = flcnQueueCmdPostBlocking(device, pFlcn, (PRM_FLCN_CMD)&soeCmd, NULL, NULL,
                                          SOE_RM_CMDQ_LOG_ID, &cmdSeqDesc, &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: DMA transfer failed\n", __FUNCTION__);
        goto ifr_dma_unmap_and_exit;
    }

    status = nvswitch_os_sync_dma_region_for_cpu(device->os_handle, dmaHandle,
                                                        transferSize,
                                                        NVSWITCH_DMA_DIR_TO_SYSMEM);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to sync DMA region\n", __FUNCTION__);
        goto ifr_dma_unmap_and_exit;
    }

    nvswitch_os_memcpy(pPackedObject, (NvU8 *)pDmaBuf + sizeof(NvU32), packedObjectSize);

    fsRet = *(NvU32*)pDmaBuf;
    if (fsRet != NV_OK)
    {
        status = -NVL_IO_ERROR;
        NVSWITCH_PRINT(device, ERROR, "%s: FS error %x. Filename: %3s\n", __FUNCTION__, fsRet,
                        pParams->fileName);
    }

ifr_dma_unmap_and_exit:
    nvswitch_os_unmap_dma_region(device->os_handle, pDmaBuf, dmaHandle,
                                        transferSize, NVSWITCH_DMA_DIR_FROM_SYSMEM);
ifr_dma_free_and_exit:
    nvswitch_os_free_contig_memory(device->os_handle, pDmaBuf, transferSize);

    return status;
}

static NvlStatus
_nvswitch_inforom_write_file
(
    nvswitch_device *device,
    const char objectName[INFOROM_FS_FILE_NAME_SIZE],
    NvU32 packedObjectSize,
    NvU8 *pPackedObject
)
{
    NvlStatus status = NVL_SUCCESS;
    void *pDmaBuf;
    NvU64 dmaHandle;
    NvU32 fsRet;
    FLCN *pFlcn = device->pSoe->pFlcn;
    RM_FLCN_CMD_SOE soeCmd;
    RM_SOE_IFR_CMD *pIfrCmd = &soeCmd.cmd.ifr;
    RM_SOE_IFR_CMD_PARAMS *pParams = &pIfrCmd->params;
    NvU32 cmdSeqDesc;
    NVSWITCH_TIMEOUT timeout;
    // The first 4 bytes are reserved for status/debug data from SOE
    NvU32 transferSize = packedObjectSize + sizeof(NvU32);

    status = nvswitch_os_alloc_contig_memory(device->os_handle, &pDmaBuf, transferSize,
                                            (device->dma_addr_width == 32));
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to allocate contig memory\n", __FUNCTION__);
        return status;
    }

    status = nvswitch_os_map_dma_region(device->os_handle, pDmaBuf, &dmaHandle,
                                        transferSize, NVSWITCH_DMA_DIR_BIDIRECTIONAL);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to map DMA region\n", __FUNCTION__);
        goto ifr_dma_free_and_exit;
    }

    nvswitch_os_memset(&soeCmd, 0, sizeof(soeCmd));
    soeCmd.hdr.unitId = RM_SOE_UNIT_IFR;
    soeCmd.hdr.size = sizeof(soeCmd);
    pIfrCmd->cmdType = RM_SOE_IFR_WRITE;

    RM_FLCN_U64_PACK(&pParams->dmaHandle, &dmaHandle);
    nvswitch_os_memcpy(pParams->fileName, objectName, INFOROM_FS_FILE_NAME_SIZE);
    pParams->offset = 0;
    pParams->sizeInBytes = packedObjectSize;

    //SOE will copy entire file from SYSMEM
    nvswitch_os_memset(pDmaBuf, 0, transferSize);
    nvswitch_os_memcpy((NvU8 *)pDmaBuf + sizeof(NvU32), pPackedObject, packedObjectSize);

    status = nvswitch_os_sync_dma_region_for_device(device->os_handle, dmaHandle,
                                                        transferSize,
                                                        NVSWITCH_DMA_DIR_BIDIRECTIONAL);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to sync DMA region\n", __FUNCTION__);
        goto ifr_dma_unmap_and_exit;
    }

    cmdSeqDesc = 0;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_4SEC_IN_NS, &timeout);
    status = flcnQueueCmdPostBlocking(device, pFlcn, (PRM_FLCN_CMD)&soeCmd, NULL, NULL,
                                          SOE_RM_CMDQ_LOG_ID, &cmdSeqDesc, &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: DMA transfer failed\n", __FUNCTION__);
        goto ifr_dma_unmap_and_exit;
    }

    status = nvswitch_os_sync_dma_region_for_cpu(device->os_handle, dmaHandle,
                                                        transferSize,
                                                        NVSWITCH_DMA_DIR_BIDIRECTIONAL);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to sync DMA region\n", __FUNCTION__);
        goto ifr_dma_unmap_and_exit;
    }

    fsRet = *(NvU32*)pDmaBuf;
    if (fsRet != NV_OK)
    {
        status = -NVL_IO_ERROR;
        NVSWITCH_PRINT(device, ERROR, "%s: FS returned %x. Filename: %3s\n", __FUNCTION__, fsRet,
                        pParams->fileName);
    }

ifr_dma_unmap_and_exit:
    nvswitch_os_unmap_dma_region(device->os_handle, pDmaBuf, dmaHandle,
                                        packedObjectSize, NVSWITCH_DMA_DIR_FROM_SYSMEM);
ifr_dma_free_and_exit:
    nvswitch_os_free_contig_memory(device->os_handle, pDmaBuf, transferSize);

    return status;
}

/*!
 * Pack and write an object to the InfoROM filesystem.
 *
 * @param[in]     device            switch device pointer
 * @param[in]     pInforom          INFOROM object pointer
 * @param[in]     objectName        Name of the object to write to the InfoROM
 * @param[in]     pObjectFormat     Ascii-string describing the layout of the
 *                                  object to write. Used to calculate the
 *                                  packed object size and to pack the data.
 * @param[in]     pObject           Contains the unpacked object to write to
 *                                  the InfoROM. It is assumed that this is
 *                                  large enough to hold the unpacked data
 *                                  size computed from the pObjectFormat
 *                                  string. This argument may not be NULL.
 * @param[in|out] pOldPackedObject  As input, contains the old packed data of
 *                                  the object, to be used to determine if any
 *                                  parts of the write can be avoided. This
 *                                  argument may be NULL.
 *
 * @return NVL_SUCCESS
 *      If the object data is successfully written
 * @return -NVL_BAD_ARGS
 *      If any of the required pointers are NULL
 * @return -NVL_ERR_NOT_SUPPORTED
 *      If the InfoROM filesystem image is not supported
 * @return Other error
 *      If dynamic memory allocation fails, packed size determination fails,
 *      object packing fails, or if there is a filesystem adapter failure in
 *      writing the packed data, it may result in other error values.
 */

NvlStatus
nvswitch_inforom_write_object
(
    nvswitch_device *device,
    const char   objectName[3],
    const char  *pObjectFormat,
    void        *pObject,
    NvU8        *pOldPackedObject
)
{
    struct inforom      *pInforom = device->pInforom;
    NvlStatus status;
    NvU8 *pPackedObject;
    NvU16 packedObjectSize;

    if (pInforom == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    status = _nvswitch_inforom_calc_packed_object_size(pObjectFormat,
                &packedObjectSize);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    if (packedObjectSize > INFOROM_MAX_PACKED_SIZE)
    {
        NVSWITCH_ASSERT(packedObjectSize > INFOROM_MAX_PACKED_SIZE);
        return -NVL_ERR_INVALID_STATE;
    }

    // Allocate a buffer to pack the object into
    pPackedObject = nvswitch_os_malloc(packedObjectSize);
    if (!pPackedObject)
    {
        return -NVL_NO_MEM;
    }

    status = _nvswitch_inforom_pack_object(pObjectFormat, pObject, pPackedObject);
    if (status != NVL_SUCCESS)
    {
        goto done;
    }

    status = _nvswitch_inforom_write_file(device, objectName, packedObjectSize, pPackedObject);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "InfoROM FS write for %c%c%c failed! rc:%d\n",
                        objectName[0], objectName[1], objectName[2], status);
        goto done;
    }

done:
    nvswitch_os_free(pPackedObject);
    return status;
}

/*!
 * @brief Looks for an object of type pType in the object cache.
 */
static NvlStatus
_nvswitch_inforom_get_cached_object
(
    struct inforom               *pInforom,
    const char                   *pType,
    INFOROM_OBJECT_HEADER_V1_00 **ppHeader
)
{
    struct INFOROM_OBJECT_CACHE_ENTRY *pCacheEntry = pInforom->pObjectCache;

    while (pCacheEntry != NULL)
    {
        if (INFOROM_FS_FILE_NAMES_MATCH(pType, pCacheEntry->header.type))
        {
            *ppHeader = &pCacheEntry->header;
            return NVL_SUCCESS;
        }

        pCacheEntry = pCacheEntry->pNext;
    }

    return -NVL_NOT_FOUND;
}

/*!
 * @brief Adds an object's unpacked header and packed data to the object cache.
 *
 * @param[in]  pInforom         INFOROM object pointer
 * @param[in]  pHeader          A pointer to the object's unpacked header
 *
 * @return NVL_SUCCESS
 *      If the object information is successfully added to the object cache
 * @return Other error
 *      If dynamic memory allocation of the cache entry fails
 */
NvlStatus nvswitch_inforom_add_object
(
    struct inforom              *pInforom,
    INFOROM_OBJECT_HEADER_V1_00 *pHeader
)
{
    struct INFOROM_OBJECT_CACHE_ENTRY *pCacheEntry = NULL;

    if (!pInforom || !pHeader)
    {
        return -NVL_ERR_INVALID_STATE;
    }

    // Allocate a new cache entry
    pCacheEntry = nvswitch_os_malloc(sizeof(struct INFOROM_OBJECT_CACHE_ENTRY));
    if (!pCacheEntry)
    {
        return -NVL_NO_MEM;
    }

    nvswitch_os_memset(pCacheEntry, 0,
                sizeof(struct INFOROM_OBJECT_CACHE_ENTRY));

    nvswitch_os_memcpy(&pCacheEntry->header, pHeader,
                sizeof(INFOROM_OBJECT_HEADER_V1_00));

    pCacheEntry->pNext     = pInforom->pObjectCache;
    pInforom->pObjectCache = pCacheEntry;

    return NVL_SUCCESS;
}

/*!
 * Get the version/subversion of an object from the Inforom.
 *
 * @param[in]  device       switch device pointer
 * @param[in]  pInforom     INFOROM object pointer
 * @param[in]  objectName   The name of the object to get the version info of
 * @param[out] pVersion     The version of the named object
 * @param[out] pSubVersion  The subversion of the named object
 *
 * @return NVL_SUCCESS
 *      Version information successfully read from the inforom.
 *
 * @return -NVL_ERR_NOT_SUPPORTED
 *      The InfoROM filesystem could not be used.
 *
 * @return Other error
 *      From @inforomReadObject if the object was not cached and could not be
 *      read from the filesystem.
 */
NvlStatus
nvswitch_inforom_get_object_version_info
(
    nvswitch_device *device,
    const char  objectName[3],
    NvU8       *pVersion,
    NvU8       *pSubVersion
)
{
    NvlStatus status = NVL_SUCCESS;
    struct inforom      *pInforom = device->pInforom;
    NvU8 packedHeader[INFOROM_OBJECT_HEADER_V1_00_PACKED_SIZE];
    INFOROM_OBJECT_HEADER_V1_00 *pHeader = NULL;
    INFOROM_OBJECT_HEADER_V1_00 header;
    NvU8 *pFile;
    NvU16 fileSize;

    if (pInforom == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    pFile = NULL;

    // First, check the cache for the object in question
    status = _nvswitch_inforom_get_cached_object(pInforom, objectName, &pHeader);
    if (status != NVL_SUCCESS)
    {
        //
        // The object wasn't cached, so we need to read it from
        // the filesystem. Since just the header is read, no checksum
        // verification is performed.
        //
        status = _nvswitch_inforom_read_file(device, objectName,
                    INFOROM_OBJECT_HEADER_V1_00_PACKED_SIZE, packedHeader);
        if (status != NVL_SUCCESS)
        {
            goto done;
        }

        // Unpack the header
        status = _nvswitch_inforom_unpack_object(INFOROM_OBJECT_HEADER_V1_00_FMT,
                    packedHeader, (NvU32 *)&header);
        if (status != NVL_SUCCESS)
        {
            goto done;
        }

        pHeader  = &header;

        //
        // Verify that the file is not corrupt, by attempting to read in the
        // entire file. We only need to do this for objects that weren't
        // cached, as we assume that cached objects were validated when they
        // were added to the cache.
        //
        fileSize = (NvU16)pHeader->size;
        if ((fileSize == 0) || (fileSize > INFOROM_MAX_PACKED_SIZE))
        {
            status = -NVL_ERR_INVALID_STATE;
            goto done;
        }

        pFile = nvswitch_os_malloc(fileSize);
        if (!pFile)
        {
            status = -NVL_NO_MEM;
            goto done;
        }

        status = _nvswitch_inforom_read_file(device, objectName, fileSize, pFile);
done:
        if (pFile != NULL)
        {
            nvswitch_os_free(pFile);
        }
    }

    if (status == NVL_SUCCESS && pHeader != NULL)
    {
        *pVersion    = (NvU8)(pHeader->version & 0xFF);
        *pSubVersion = (NvU8)(pHeader->subversion & 0xFF);
    }

    return status;
}

/*!
 *  Fill in the static identification data structure for the use by the SOE
 *  to be passed on to a BMC over the I2CS interface.
 *  For LR10 only so no HAL is needed.
 *
 * @param[in]      device       switch device pointer
 * @param[in]      pInforom     INFOROM object pointer
 * @param[in, out] pData        Target data structure pointer (the structure
 *                              must be zero initialized by the caller)
 */
void
nvswitch_inforom_read_static_data
(
    nvswitch_device            *device,
    struct inforom             *pInforom,
    RM_SOE_SMBPBI_INFOROM_DATA *pData
)
{
#define _INFOROM_TO_SOE_STRING_COPY(obj, irName, soeName)                                   \
{                                                                                           \
    NvU32   _i;                                                                             \
    ct_assert(NV_ARRAY_ELEMENTS(pInforom->obj.object.v1.irName) <=                             \
              NV_ARRAY_ELEMENTS(pData->obj.soeName));                                       \
    for (_i = 0; _i < NV_ARRAY_ELEMENTS(pInforom->obj.object.v1.irName); ++_i)                 \
    {                                                                                       \
        pData->obj.soeName[_i] = (NvU8)(pInforom->obj.object.v1.irName[_i] & 0xff);            \
    }                                                                                       \
    if (NV_ARRAY_ELEMENTS(pInforom->obj.object.v1.irName) <                                    \
        NV_ARRAY_ELEMENTS(pData->obj.soeName))                                              \
    {                                                                                       \
        do                                                                                  \
        {                                                                                   \
            pData->obj.soeName[_i++] = 0;                                                   \
        }                                                                                   \
        while (_i < NV_ARRAY_ELEMENTS(pData->obj.soeName));                                 \
    }                                                                                       \
}

    if (pInforom->OBD.bValid)
    {
        /* This should be called for LR10 (i.e., version 1.xx) only */
        if ((pInforom->OBD.object.header.version & 0xFF) == 1)
        {
            pData->OBD.bValid = NV_TRUE;
            pData->OBD.buildDate = (NvU32)pInforom->OBD.object.v1.buildDate;
            nvswitch_inforom_string_copy(pInforom->OBD.object.v1.marketingName,
                                          pData->OBD.marketingName,
                                          NV_ARRAY_ELEMENTS(pData->OBD.marketingName));

            nvswitch_inforom_string_copy(pInforom->OBD.object.v1.serialNumber,
                                          pData->OBD.serialNum,
                                          NV_ARRAY_ELEMENTS(pData->OBD.serialNum));

            //
            // boardPartNum requires special handling, as its size exceeds that
            // of its InfoROM representation
            //
            _INFOROM_TO_SOE_STRING_COPY(OBD, productPartNumber, boardPartNum);
        }
    }

    if (pInforom->OEM.bValid)
    {
        pData->OEM.bValid = NV_TRUE;
        nvswitch_inforom_string_copy(pInforom->OEM.object.oemInfo,
                                      pData->OEM.oemInfo,
                                      NV_ARRAY_ELEMENTS(pData->OEM.oemInfo));
    }

    if (pInforom->IMG.bValid)
    {
        pData->IMG.bValid = NV_TRUE;
        nvswitch_inforom_string_copy(pInforom->IMG.object.version,
                                      pData->IMG.inforomVer,
                                      NV_ARRAY_ELEMENTS(pData->IMG.inforomVer));
    }

#undef _INFOROM_TO_SOE_STRING_COPY
}

/*!
 *
 * Wrapper to read an inforom object into system and cache the header
 *
 */
NvlStatus
nvswitch_inforom_load_object
(
    nvswitch_device *device,
    struct inforom  *pInforom,
    const char   objectName[3],
    const char  *pObjectFormat,
    NvU8        *pPackedObject,
    void        *pObject
)
{
    NvlStatus status;

    status = nvswitch_inforom_read_object(device, objectName, pObjectFormat,
                                        pPackedObject, pObject);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Failed to read %c%c%c object, rc:%d\n",
                    objectName[0], objectName[1], objectName[2], status);
        return status;
    }

    status = nvswitch_inforom_add_object(pInforom,
                        (INFOROM_OBJECT_HEADER_V1_00 *)pObject);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
                    "Failed to cache %c%c%c object header, rc:%d\n",
                    objectName[0], objectName[1], objectName[2], status);
        return status;
    }

    return status;
}

/*!
 * @brief Inforom State Initialization
 *
 * Initializes the filesystem layer of the InfoROM so that InfoROM objects can
 * be read. Also load certain InfoROM objects that are neded as early as possible
 * in the initialization path (See bug 992278).
 *
 * @param[in]     device    switch device pointer
 * @param[in/out] pInforom  INFOROM object pointer.
 *
 * @return NVL_SUCCESS
 *      If the filesystem layer is initialized successfully
 * @return -NVL_ERR_NOT_SUPPORTED
 *      If an adapter could not be set up for the InfoROM image device.
 * @return Other error
 *      From attempting to determine the image device location of the InfoROM
 *      or constructing a filesystem adapter for the image.
 */
NvlStatus
nvswitch_initialize_inforom
(
    nvswitch_device *device
)
{
    struct inforom      *pInforom;

    pInforom = nvswitch_os_malloc(sizeof(struct inforom));
    if (!pInforom)
    {
        return -NVL_NO_MEM;
    }
    nvswitch_os_memset(pInforom, 0, sizeof(struct inforom));

    device->pInforom = pInforom;

    return NVL_SUCCESS;
}

/*!
 * @brief Tears down the state of the InfoROM.
 *
 * This includes tearing down the HAL, the FSAL, and freeing any objects left
 * in the object cache.
 *
 * @param[in]  device       switch device pointer
 * @param[in]  pInforom     INFOROM object pointer
 */
void
nvswitch_destroy_inforom
(
    nvswitch_device *device
)
{
    struct inforom                    *pInforom = device->pInforom;
    struct INFOROM_OBJECT_CACHE_ENTRY *pCacheEntry;
    struct INFOROM_OBJECT_CACHE_ENTRY *pTmpCacheEntry;

    if (pInforom)
    {
        pCacheEntry = pInforom->pObjectCache;
        while (pCacheEntry != NULL)
        {
            pTmpCacheEntry = pCacheEntry;
            pCacheEntry = pCacheEntry->pNext;
            nvswitch_os_free(pTmpCacheEntry);
        }

        nvswitch_os_free(pInforom);
        device->pInforom = NULL;
    }
}

void
nvswitch_inforom_post_init
(
    nvswitch_device *device
)
{
    return;
}

NvlStatus
nvswitch_initialize_inforom_objects
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

    // RO objects
    status = nvswitch_inforom_read_only_objects_load(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, INFO, "Failed to load RO objects, rc:%d\n",
                    status);
    }

    // NVL object
    status = nvswitch_inforom_nvlink_load(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, INFO, "Failed to load NVL object, rc:%d\n",
                    status);
    }

    status = nvswitch_inforom_ecc_load(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, INFO, "Failed to load ECC object, rc:%d\n",
                    status);
    }

    status = nvswitch_inforom_oms_load(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, INFO, "Failed to load OMS object, rc:%d\n",
                    status);
    }

    status = nvswitch_inforom_bbx_load(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, INFO, "Failed to load BBX object, rc: %d\n",
                    status);
    }

    status = device->hal.nvswitch_smbpbi_dem_load(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, INFO, "Failed to load DEM object, rc: %d\n",
                    status);
    }

    return NVL_SUCCESS;
}

void
nvswitch_destroy_inforom_objects
(
    nvswitch_device *device
)
{
    struct inforom *pInforom = device->pInforom;

    if (pInforom == NULL)
    {
        return;
    }

    // BBX object
    nvswitch_inforom_bbx_unload(device);

    // ECC object
    nvswitch_inforom_ecc_unload(device);

    // NVL object
    nvswitch_inforom_nvlink_unload(device);

    // OMS object
    nvswitch_inforom_oms_unload(device);
}
