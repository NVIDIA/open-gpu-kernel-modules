/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
    // Physical memory aperture through which bootArgsPa is accessed
    GSP_DMA_TARGET target;
    // Physical address that will be stuffed in NV_PGSP_FALCON_MAILBOX(0|1)
    NvU64          bootArgsOffset;
} GSP_RM_PARAMS;

/*!
 * @brief GSP-CC Microcode Parameters for Boot Partitions
 */
typedef struct GSP_FMC_BOOT_PARAMS
{
    GSP_FMC_INIT_PARAMS         initParams;
    GSP_ACR_BOOT_GSP_RM_PARAMS  bootGspRmParams;
    GSP_RM_PARAMS               gspRmParams;
} GSP_FMC_BOOT_PARAMS;

#endif // GSPIFPUB_H
