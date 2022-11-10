/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _SMBPBI_NVSWITCH_H_
#define _SMBPBI_NVSWITCH_H_

#include "soe/soeifsmbpbi.h"
#include "smbpbi_shared_nvswitch.h"
#include "oob/smbpbi_priv.h"

typedef struct
{
    NvBool isValid;
    NvU64  attemptedTrainingMask0;
    NvU64  trainingErrorMask0;
} NVSWITCH_LINK_TRAINING_ERROR_INFO;

typedef struct
{
    NvBool isValid;
    NvU64  mask0;
} NVSWITCH_LINK_RUNTIME_ERROR_INFO;

struct smbpbi
{
    SOE_SMBPBI_SHARED_SURFACE       *sharedSurface;
    NvU64                           dmaHandle;
    NvU32                           logMessageNesting;
};

NvlStatus nvswitch_smbpbi_init(nvswitch_device *);
NvlStatus nvswitch_smbpbi_post_init(nvswitch_device *);
NvlStatus nvswitch_smbpbi_set_link_error_info(nvswitch_device *,
                                              NVSWITCH_LINK_TRAINING_ERROR_INFO *pLinkTrainingErrorInfo,
                                              NVSWITCH_LINK_RUNTIME_ERROR_INFO  *pLinkRuntimeError);
void nvswitch_smbpbi_unload(nvswitch_device *);
void nvswitch_smbpbi_destroy(nvswitch_device *);
NvlStatus nvswitch_smbpbi_refresh_ecc_counts(nvswitch_device *);

#endif //_SMBPBI_NVSWITCH_H_
