/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      class/cla0bd.finn
//

#define NVFBC_SW_SESSION (0xa0bdU) /* finn: Evaluated from "NVA0BD_ALLOC_PARAMETERS_MESSAGE_ID" */

/*
 * NVA0BD_ALLOC_PARAMETERS
 *
 *   displayOrdinal
 *     This parameter specifies the display identifier.
 *   sessionType
 *     This parameter NVFBC session type. Possible values are specified 
 *     by NVA0BD_NVFBC_SESSION_TYPE_* macros.
 *   sessionFlags
 *     This parameter returns various flags values of the NVFBC session.
 *     Valid flag values include:
 *       NVA0BD_NVFBC_SESSION_FLAG_DIFFMAP_ENABLED
 *         When true this flag indicates there are user has enabled 
 *         diff map feature for the session.
 *       NVA0BD_NVFBC_SESSION_FLAG_CLASSIFICATIONMAP_ENABLED
 *         When true this flag indicates there are user has enabled 
 *         diff map feature for the session.
 *   hMaxResolution
 *     This parameter returns the max horizontal resolution supported by
 *     the NvFBC session.
 *   vMaxResolution
 *     This parameter returns the max vertical resolution supported by
 *     the NvFBC session.
 */

#define NVA0BD_ALLOC_PARAMETERS_MESSAGE_ID (0xa0bdU)

typedef struct NVA0BD_ALLOC_PARAMETERS {
    NvU32 displayOrdinal;
    NvU32 sessionType;
    NvU32 sessionFlags;
    NvU32 hMaxResolution;
    NvU32 vMaxResolution;
} NVA0BD_ALLOC_PARAMETERS;

#define NVA0BD_NVFBC_SESSION_TYPE_UNKNOWN                         0x000000
#define NVA0BD_NVFBC_SESSION_TYPE_TOSYS                           0x000001
#define NVA0BD_NVFBC_SESSION_TYPE_CUDA                            0x000002
#define NVA0BD_NVFBC_SESSION_TYPE_VID                             0x000003
#define NVA0BD_NVFBC_SESSION_TYPE_HWENC                           0x000004

#define NVA0BD_NVFBC_SESSION_FLAG_DIFFMAP_ENABLED                 0:0
#define NVA0BD_NVFBC_SESSION_FLAG_DIFFMAP_ENABLED_FALSE           (0x00000000)
#define NVA0BD_NVFBC_SESSION_FLAG_DIFFMAP_ENABLED_TRUE            (0x00000001)
#define NVA0BD_NVFBC_SESSION_FLAG_CLASSIFICATIONMAP_ENABLED       1:1
#define NVA0BD_NVFBC_SESSION_FLAG_CLASSIFICATIONMAP_ENABLED_FALSE (0x00000000)
#define NVA0BD_NVFBC_SESSION_FLAG_CLASSIFICATIONMAP_ENABLED_TRUE  (0x00000001)

