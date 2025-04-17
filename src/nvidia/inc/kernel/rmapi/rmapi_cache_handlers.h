/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _RMAPI_CACHE_HANDLERS_H_
#define _RMAPI_CACHE_HANDLERS_H_

#include "nvtypes.h"
#include "nvstatus.h"
#include "nvlimits.h"
#include "ctrl/ctrl0073.h"
#include "rmconfig.h"
#include "core/prelude.h"

typedef NV_STATUS (*RmapiCacheGetByInputHandler)(void *cachedEntry, void* pParams, NvBool bSet);

//
// Rm Cmd Cache Handler structure definitions and function declarations.
//

typedef struct DispSystemGetSupportedCacheEntry
{
    NvBool valid;
    NvU32 displayMask;
    NvU32 displayMaskDDC;
} DispSystemGetSupportedCacheEntry;

NV_STATUS _dispSystemGetSupportedCacheHandler(void *cachedEntry, void* pParams, NvBool bSet);

typedef struct DispSystemGetInternalDisplaysCacheEntry
{
    NvBool valid;
    NvU32 internalDisplaysMask;
    NvU32 availableInternalDisplaysMask;
} DispSystemGetInternalDisplaysCacheEntry;

NV_STATUS _dispSystemGetInternalDisplaysCacheHandler(void *cachedEntry, void* pParams, NvBool bSet);

typedef struct DispSpecificGetTypeCacheTable
{
    struct
    {
        NvBool valid;
        NvU32 displayType;
    } cachedEntries[NV_MAX_DEVICES];
} DispSpecificGetTypeCacheTable;

NV_STATUS _dispSpecificGetTypeCacheHandler(void *cachedEntry, void *pProvidedParams, NvBool bSet);

#endif
