/*
 * SPDX-FileCopyrightText: Copyright (c) 2001-2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _cl0080_h_
#define _cl0080_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvlimits.h"
#include "nvtypes.h"

#define  NV01_DEVICE_0                                             (0x00000080)
/* NvNotification[] fields and values */
#define NV080_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
/* pio method data structure */
typedef volatile struct _cl0080_tag0 {
 NvV32 Reserved00[0x7c0];
} Nv080Typedef, Nv01Device0;
#define  NV080_TYPEDEF                                             Nv01Device0

/* NvAlloc parameteters */
#define NV0080_MAX_DEVICES                                         NV_MAX_DEVICES
/**
 * @brief Alloc param 
 *
 * @param vaMode mode for virtual address space allocation
 *  Three modes:
 *  NV_DEVICE_ALLOCATION_VAMODE_OPTIONAL_MULTIPLE_VASPACES
 *  NV_DEVICE_ALLOCATION_VAMODE_SINGLE_VASPACE
 *  NV_DEVICE_ALLOCATION_VAMODE_MULTIPLE_VASPACES
 *  Detailed description of these modes is in nvos.h
 **/ 
typedef struct {
    NvU32       deviceId;
    NvHandle    hClientShare;
    NvHandle    hTargetClient;
    NvHandle    hTargetDevice;
    NvV32       flags;
    NvU64       vaSpaceSize NV_ALIGN_BYTES(8);
    NvU64       vaStartInternal NV_ALIGN_BYTES(8);
    NvU64       vaLimitInternal NV_ALIGN_BYTES(8);
    NvV32       vaMode;
} NV0080_ALLOC_PARAMETERS;

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _cl0080_h_ */
