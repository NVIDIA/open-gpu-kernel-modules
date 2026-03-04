/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _INFOROM_NVSWITCH_H_
#define _INFOROM_NVSWITCH_H_

#include "inforom/ifrstruct.h"
#include "inforom/omsdef.h"
#include "nv_list.h"
#include "smbpbi_shared_nvswitch.h"

#define INFOROM_MAX_PACKED_SIZE (32*1024)

#define INFOROM_FS_FILE_SIZE(pPackedFile) \
    (((pPackedFile)[INFOROM_OBJECT_HEADER_V1_00_SIZE_OFFSET]) | \
     ((pPackedFile)[INFOROM_OBJECT_HEADER_V1_00_SIZE_OFFSET + 1] << 8))
#define INFOROM_FS_FILE_NAMES_MATCH(fileName1, fileName2) \
    ((((NvU8)((fileName1)[0])) == ((NvU8)((fileName2)[0]))) && \
     (((NvU8)((fileName1)[1])) == ((NvU8)((fileName2)[1]))) && \
     (((NvU8)((fileName1)[2])) == ((NvU8)((fileName2)[2]))))

#define INFOROM_FS_COPY_FILE_NAME(destName, srcName) \
{                                                    \
    (destName)[0] = (srcName)[0];                    \
    (destName)[1] = (srcName)[1];                    \
    (destName)[2] = (srcName)[2];                    \
}

#define m_inforom_nvl_get_new_errors_per_minute(value, pSum)    \
    do                                                          \
    {                                                           \
        *pSum = (*pSum - (*pSum / 60)) + value;                 \
    } while (NV_FALSE)                                          \

//
// OS type defines.
//
#define INFOROM_BBX_OBJ_V1_00_SYSTEM_OS_TYPE_OTHER                           0x0
#define INFOROM_BBX_OBJ_V1_00_SYSTEM_OS_TYPE_WIN9X                           0x1
#define INFOROM_BBX_OBJ_V1_00_SYSTEM_OS_TYPE_WIN2K                           0x2
#define INFOROM_BBX_OBJ_V1_00_SYSTEM_OS_TYPE_WIN                             0x4
#define INFOROM_BBX_OBJ_V1_00_SYSTEM_OS_TYPE_UNIX                            0x5

#define INFOROM_BBX_OBJ_V1_00_SYSTEM_OS_MAJOR                                7:0
#define INFOROM_BBX_OBJ_V1_00_SYSTEM_OS_MINOR                               15:8
#define INFOROM_BBX_OBJ_V1_00_SYSTEM_OS_BUILD                              31:16


struct INFOROM_OBJECT_CACHE_ENTRY
{
    INFOROM_OBJECT_HEADER_V1_00         header;
    struct INFOROM_OBJECT_CACHE_ENTRY  *pNext;
};

struct inforom
{
    // InfoROM Objects
    // RO objects - statically allocated as the InfoROM should always contain
    // these objects.
    struct
    {
        NvBool                      bValid;
        union {
            NvU8 v1[INFOROM_OBD_OBJECT_V1_XX_PACKED_SIZE];
            NvU8 v2[INFOROM_OBD_OBJECT_V2_XX_PACKED_SIZE];
        } packedObject;
        union {
            INFOROM_OBJECT_HEADER_V1_00 header;
            INFOROM_OBD_OBJECT_V1_XX    v1;
            INFOROM_OBD_OBJECT_V2_XX    v2;
        } object;
    } OBD;

    struct
    {
        NvBool                      bValid;
        NvU8                        packedObject[INFOROM_OEM_OBJECT_V1_00_PACKED_SIZE];
        INFOROM_OEM_OBJECT_V1_00    object;
    } OEM;

    struct
    {
        NvBool                      bValid;
        NvU8                        packedObject[INFOROM_IMG_OBJECT_V1_00_PACKED_SIZE];
        INFOROM_IMG_OBJECT_V1_00    object;
    } IMG;

    INFOROM_NVLINK_STATE               *pNvlinkState;
    INFOROM_ECC_STATE                  *pEccState;
    INFOROM_OMS_STATE                  *pOmsState;

    //
    // descriptor cache for all the inforom objects. This is to handle inforom objects in a generic way.
    //
    struct INFOROM_OBJECT_CACHE_ENTRY   *pObjectCache;
};

// Generic InfoROM APIs
NvlStatus nvswitch_initialize_inforom(nvswitch_device *device);
NvlStatus nvswitch_inforom_read_object(nvswitch_device* device,
                const char objectName[3], const char *pObjectFormat,
                NvU8 *pPackedObject, void *pObject);
NvlStatus nvswitch_inforom_write_object(nvswitch_device* device,
                const char objectName[3], const char *pObjectFormat,
                void *pObject, NvU8 *pOldPackedObject);
void nvswitch_destroy_inforom(nvswitch_device *device);
NvlStatus nvswitch_inforom_add_object(struct inforom *pInforom,
                                    INFOROM_OBJECT_HEADER_V1_00 *pHeader);
NvlStatus nvswitch_inforom_get_object_version_info(nvswitch_device *device,
                const char objectName[3], NvU8 *pVersion, NvU8 *pSubVersion);
void *nvswitch_add_halinfo_node(NVListPtr head, int type, int size);
void *nvswitch_get_halinfo_node(NVListPtr head, int type);
void nvswitch_inforom_post_init(nvswitch_device *device);
NvlStatus nvswitch_initialize_inforom_objects(nvswitch_device *device);
void nvswitch_destroy_inforom_objects(nvswitch_device *device);
NvlStatus nvswitch_inforom_load_object(nvswitch_device* device,
                struct inforom *pInforom, const char objectName[3],
                const char *pObjectFormat, NvU8 *pPackedObject, void *pObject);
void nvswitch_inforom_read_static_data(nvswitch_device *device,
                struct inforom  *pInforom, RM_SOE_SMBPBI_INFOROM_DATA *pData);
void nvswitch_inforom_string_copy(inforom_U008 *pSrc, NvU8 *pDst, NvU32 size);

// InfoROM RO APIs
NvlStatus nvswitch_inforom_read_only_objects_load(nvswitch_device *device);

// InfoROM NVL APIs
NvlStatus nvswitch_inforom_nvlink_load(nvswitch_device *device);
void nvswitch_inforom_nvlink_unload(nvswitch_device *device);
NvlStatus nvswitch_inforom_nvlink_flush(nvswitch_device *device);
NvlStatus nvswitch_inforom_nvlink_get_minion_data(nvswitch_device *device,
                                            NvU8 linkId, NvU32 *seedData);
NvlStatus nvswitch_inforom_nvlink_set_minion_data(nvswitch_device *device,
                                NvU8 linkId, NvU32 *seedData, NvU32 size);
NvlStatus nvswitch_inforom_nvlink_log_error_event(nvswitch_device *device, void *error_event);
NvlStatus nvswitch_inforom_nvlink_get_max_correctable_error_rate(nvswitch_device *device,
                NVSWITCH_GET_NVLINK_MAX_CORRECTABLE_ERROR_RATES_PARAMS *params);
NvlStatus nvswitch_inforom_nvlink_get_errors(nvswitch_device *device,
                                NVSWITCH_GET_NVLINK_ERROR_COUNTS_PARAMS *params);
NvlStatus nvswitch_inforom_nvlink_setL1Threshold(nvswitch_device *device, NvU32 word1, NvU32 word2);
NvlStatus nvswitch_inforom_nvlink_getL1Threshold(nvswitch_device *device, NvU32 *word1, NvU32 *word2);

// InfoROM ECC APIs
NvlStatus nvswitch_inforom_ecc_load(nvswitch_device *device);
void nvswitch_inforom_ecc_unload(nvswitch_device *device);
NvlStatus nvswitch_inforom_ecc_flush(nvswitch_device *device);
NvlStatus nvswitch_inforom_ecc_log_err_event(nvswitch_device *device,
                                INFOROM_NVS_ECC_ERROR_EVENT *err_event);
NvlStatus nvswitch_inforom_ecc_get_errors(nvswitch_device *device,
                                NVSWITCH_GET_ECC_ERROR_COUNTS_PARAMS *params);

// InfoROM OMS APIs
NvlStatus nvswitch_inforom_oms_load(nvswitch_device *device);
void nvswitch_inforom_oms_unload(nvswitch_device *device);
NvlStatus nvswitch_inforom_oms_set_device_disable(nvswitch_device *device,
                                        NvBool bDisable);
NvlStatus nvswitch_inforom_oms_get_device_disable(nvswitch_device *device,
                                        NvBool *pBDisabled);

// InfoROM BBX APIs
NvlStatus nvswitch_inforom_bbx_load(nvswitch_device *device);
void nvswitch_inforom_bbx_unload(nvswitch_device * device);
NvlStatus nvswitch_inforom_bbx_add_sxid(nvswitch_device *device,
                                    NvU32 exceptionType, NvU32 data0,
                                    NvU32 data1, NvU32 data2);
NvlStatus nvswitch_inforom_bbx_get_sxid(nvswitch_device *device,
                            NVSWITCH_GET_SXIDS_PARAMS *params);
NvlStatus nvswitch_inforom_bbx_get_data(nvswitch_device *device, NvU8 dataType, void *params);

// InfoROM DEM APIs
NvlStatus nvswitch_inforom_dem_load(nvswitch_device *device);
void nvswitch_inforom_dem_unload(nvswitch_device * device);
#endif // _INFOROM_NVSWITCH_H_
