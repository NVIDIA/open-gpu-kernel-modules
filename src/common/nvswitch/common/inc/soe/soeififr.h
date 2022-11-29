/*
 * SPDX-FileCopyrightText: Copyright (c) 2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _SOEIFIFR_H_
#define _SOEIFIFR_H_

#include "flcnifcmn.h"

#define INFOROM_FS_FILE_NAME_SIZE       3

enum
{
    RM_SOE_IFR_READ,
    RM_SOE_IFR_WRITE,
    RM_SOE_IFR_BBX_INITIALIZE,
    RM_SOE_IFR_BBX_SHUTDOWN,
    RM_SOE_IFR_BBX_SXID_ADD,
    RM_SOE_IFR_BBX_SXID_GET,
};

typedef struct
{
    NvU8        cmdType;
    RM_FLCN_U64 dmaHandle;
    NvU32       offset;
    NvU32       sizeInBytes;
    char        fileName[INFOROM_FS_FILE_NAME_SIZE];
} RM_SOE_IFR_CMD_PARAMS;

typedef struct
{
    NvU8        cmdType;
    RM_FLCN_U64 time;
    NvU8        osType;
    NvU32       osVersion;
} RM_SOE_IFR_CMD_BBX_INIT_PARAMS;

typedef struct
{
    NvU8        cmdType;
} RM_SOE_IFR_CMD_BBX_SHUTDOWN_PARAMS;

typedef struct
{
    NvU8        cmdType;
    NvU32       exceptionType;
    NvU32       data[3];
} RM_SOE_IFR_CMD_BBX_SXID_ADD_PARAMS;

typedef struct
{
    NvU8 cmdType;
    RM_FLCN_U64 dmaHandle;
} RM_SOE_IFR_CMD_BBX_SXID_GET_PARAMS;

typedef union
{
	NvU8	cmdType;
	RM_SOE_IFR_CMD_PARAMS params;
    RM_SOE_IFR_CMD_BBX_INIT_PARAMS bbxInit;
    RM_SOE_IFR_CMD_BBX_SXID_ADD_PARAMS bbxSxidAdd;
    RM_SOE_IFR_CMD_BBX_SXID_GET_PARAMS bbxSxidGet;
} RM_SOE_IFR_CMD;

#endif // _SOEIFIFR_H_
