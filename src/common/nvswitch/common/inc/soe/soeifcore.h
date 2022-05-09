/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _SOECORE_H_
#define _SOECORE_H_

/*!
 * @file   soeifutil.h
 * @brief  SOE CORE Command Queue
 *
 *         The CORE unit ID will be used for sending and recieving
 *         Command Messages between driver and CORE unit of SOE
 */

/*!
 * Commands offered by the SOE utility Interface.
 */
enum
{
    /*!
     * Read the BIOS Size
     */
    RM_SOE_CORE_CMD_READ_BIOS_SIZE,

    /*!
     * Read the BIOS
     */
    RM_SOE_CORE_CMD_READ_BIOS,

    /*!
     * Run DMA self-test
     */
    RM_SOE_CORE_CMD_DMA_SELFTEST,
};

// Timeout for SOE reset callback function
#define SOE_UNLOAD_CALLBACK_TIMEOUT_US 10000 // 10ms

#define SOE_DMA_TEST_BUF_SIZE       512

#define SOE_DMA_TEST_INIT_PATTERN   0xab
#define SOE_DMA_TEST_XFER_PATTERN   0xcd

#define RM_SOE_DMA_READ_TEST_SUBCMD    0x00
#define RM_SOE_DMA_WRITE_TEST_SUBCMD   0x01
/*!
 * CORE queue command payload
 */
typedef struct
{
    NvU8 cmdType;
    RM_FLCN_U64 dmaHandle;
    NvU32 offset;
    NvU32 sizeInBytes;
} RM_SOE_CORE_CMD_BIOS;

typedef struct
{
    NvU8        cmdType;
    NvU8        subCmdType;
    RM_FLCN_U64 dmaHandle;
    NvU8        dataPattern;
    NvU16       xferSize;
} RM_SOE_CORE_CMD_DMA_TEST;

typedef union
{
    NvU8 cmdType;
    RM_SOE_CORE_CMD_BIOS bios;
    RM_SOE_CORE_CMD_DMA_TEST dma_test;
} RM_SOE_CORE_CMD;
#endif  // _SOECORE_H_
