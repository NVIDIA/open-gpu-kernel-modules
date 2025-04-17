/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef GSPIFPUB_H
#define GSPIFPUB_H

#include <nvtypes.h>

/*!
 * @file    gspifpub.h
 * @brief   GSP Command/Message Interfaces - Published
 */

/*!
 * Target physical memory apertures supported for DMA
 */
typedef enum {
    GSP_DMA_TARGET_LOCAL_FB,
    GSP_DMA_TARGET_COHERENT_SYSTEM,
    GSP_DMA_TARGET_NONCOHERENT_SYSTEM,
    GSP_DMA_TARGET_COUNT
} GSP_DMA_TARGET;

/*!
 * @brief GSP-CC Microcode Initialization Parameters
 */
typedef struct GSP_FMC_INIT_PARAMS
{
    // CC initialization "registry keys"
    NvU32 regkeys;
} GSP_FMC_INIT_PARAMS;

/*!
 * @brief GSP-ACR BOOT_GSP_RM Command Parameters
 *
 * The wprCarveout fields have no effect in environments where the WPR can be allocated
 * implicitly by ACR.
 */
typedef struct GSP_ACR_BOOT_GSP_RM_PARAMS
{
    // Physical memory aperture through which gspRmDescPa is accessed
    GSP_DMA_TARGET target;
    // Size in bytes of the GSP-RM descriptor structure
    NvU32          gspRmDescSize;
    // Physical offset in the target aperture of the GSP-RM descriptor structure
    NvU64          gspRmDescOffset;
    // Physical offset in FB to set the start of the WPR containing GSP-RM
    NvU64          wprCarveoutOffset;
    // Size in bytes of the WPR containing GSP-RM
    NvU32          wprCarveoutSize;
    // Whether to boot GSP-RM or GSP-Proxy through ACR
    NvBool         bIsGspRmBoot;
} GSP_ACR_BOOT_GSP_RM_PARAMS;

/*!
 * @brief GSP-RM Parameters
 */
typedef struct GSP_RM_PARAMS
{
    // Physical memory aperture through which bootArgsOffset is accessed
    GSP_DMA_TARGET target;
    // Physical offset in the memory aperture that will be passed to GSP-RM
    NvU64          bootArgsOffset;
} GSP_RM_PARAMS;

/*!
 * @brief GSP-SPDM Parameters
 *
 * Parameters required to set up a communication mechanism between Kernel-RM
 * and SPDM partition inside GSP.
 */
typedef struct GSP_SPDM_PARAMS
{
    // Physical Memory Aperture through which all addresses are accessed
    GSP_DMA_TARGET target;

    // Physical offset in the memory aperture where SPDM payload is stored
    NvU64 payloadBufferOffset;

    // Size of the above payload buffer
    NvU32 payloadBufferSize;
} GSP_SPDM_PARAMS;

typedef struct GSP_RM_MEM_PARAMS
{
    NvU32 flushSysmemAddrValLo;
    NvU32 flushSysmemAddrValHi;
} GSP_RM_MEM_PARAMS;

/*!
 * @brief GSP-CC Microcode Parameters for Boot Partitions
 */
typedef struct GSP_FMC_BOOT_PARAMS
{
    GSP_FMC_INIT_PARAMS         initParams;
    GSP_ACR_BOOT_GSP_RM_PARAMS  bootGspRmParams;
    GSP_RM_PARAMS               gspRmParams;
    GSP_SPDM_PARAMS             gspSpdmParams;
    GSP_RM_MEM_PARAMS           gspRmMemParams;
} GSP_FMC_BOOT_PARAMS;

/*!
 * @brief Definitions for messages to GSP which will convey NVLE programming keys
 */
#define RM_GSP_NVLE_CMD_ID_UPDATE_SESSION_KEYS  (0x80)
#define RM_GSP_NVLE_SESSION_KEY_ENTRY_SET_COUNT (1)
#define RM_GSP_NVLE_AES_256_GCM_KEY_SIZE_BYTES  (32)
#define RM_GSP_NVLE_AES_256_GCM_KEY_SIZE_DWORDS (RM_GSP_NVLE_AES_256_GCM_KEY_SIZE_BYTES / sizeof(NvU32))
#define RM_GSP_NVLE_CMD_TAG_SIZE_BYTES          (16)
#define RM_GSP_NVLE_CMD_TAG_SIZE_DWORDS         (RM_GSP_NVLE_CMD_TAG_SIZE_BYTES / sizeof(NvU32))

typedef struct
{
    NvU32  remoteScfDcfGpuId;
    NvU32  key[RM_GSP_NVLE_AES_256_GCM_KEY_SIZE_DWORDS];
    NvBool bValid;
} RM_GSP_NVLE_SESSION_KEY_ENTRY;

typedef struct
{
    NvU32                         cmdId;
    NvBool                        bForKeyRotation;
    RM_GSP_NVLE_SESSION_KEY_ENTRY wrappedKeyEntries[RM_GSP_NVLE_SESSION_KEY_ENTRY_SET_COUNT];
    NvU32                         keyEntriesTag[RM_GSP_NVLE_CMD_TAG_SIZE_DWORDS];
} RM_GSP_NVLE_UPDATE_SESSION_KEYS;

#endif // GSPIFPUB_H
