/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _RMAPI_DEPRECATED_H_
#define _RMAPI_DEPRECATED_H_

#include "nvtypes.h"
#include "nvstatus.h"
#include "nvmisc.h"
#include "nvos.h"
#include "nvsecurityinfo.h"
//
// This file provides implementations for deprecated RM API by building on the
// modern APIs. The implementations support running in either
// user-mode or kernel-mode context and should have no dependencies on RM
// internals.
//

/*!
 * GSS legacy command masks
 */
#define RM_GSS_LEGACY_MASK                   0x00008000
#define RM_GSS_LEGACY_MASK_NON_PRIVILEGED    0x00008000
#define RM_GSS_LEGACY_MASK_PRIVILEGED        0x0000C000


typedef enum
{
    RMAPI_DEPRECATED_COPYIN,
    RMAPI_DEPRECATED_COPYOUT,
    RMAPI_DEPRECATED_COPYRELEASE,
} RMAPI_DEPRECATED_COPY_OP;

typedef enum
{
    RMAPI_DEPRECATED_BUFFER_EMPLACE,     // Use buffer passed into CopyUser
    RMAPI_DEPRECATED_BUFFER_ALLOCATE     // Allocate a new buffer in CopyUser
} RMAPI_DEPRECATED_BUFFER_POLICY;

/**
 * Fields are populated by the deprecated RM API caller. RmAlloc, RmControl, and
 * RmFree should be routed to RM. pExtendedContext can hold any domain specific
 * state needed by the RmAlloc/etc implementations. AllocMem/FreeMem are routed
 * to OS layers for allocation/free-up of system memory.
 */
typedef struct _DEPRECATED_CONTEXT
{
    NV_STATUS (*RmAlloc)(struct _DEPRECATED_CONTEXT *pContext, NvHandle hClient, NvHandle hParent,
                         NvHandle *phObject, NvU32 hClass, void *pAllocParams);

    NV_STATUS (*RmControl)(struct _DEPRECATED_CONTEXT *pContext, NvHandle hClient, NvHandle hObject,
                           NvU32 cmd, void *pParams, NvU32 paramsSize);

    NV_STATUS (*RmFree)(struct _DEPRECATED_CONTEXT *pContext, NvHandle hClient, NvHandle hObject);

    NV_STATUS (*RmMapMemory)(struct _DEPRECATED_CONTEXT *pContext, NvHandle hClient, NvHandle hDevice,
                             NvHandle hMemory, NvU64 offset, NvU64 length, NvP64 *ppCpuVirtAddr, NvU32 flags);

    // Copies data in/out of user-mode address space.
    NV_STATUS (*CopyUser)(struct _DEPRECATED_CONTEXT *pContext, RMAPI_DEPRECATED_COPY_OP op,
                          RMAPI_DEPRECATED_BUFFER_POLICY bufPolicy, NvP64 dataPtr,
                          NvU32 dataSize, void **ppKernelPtr);
    void * (*AllocMem)(NvU32 length);
    void (*FreeMem)(void *pAddress);
    void *pExtendedContext;
} DEPRECATED_CONTEXT;

/**
 * List of deprecated APIs supported by this library
 */
void RmDeprecatedAllocObject(DEPRECATED_CONTEXT *pContext, NVOS05_PARAMETERS *pArgs);
void RmDeprecatedAddVblankCallback(DEPRECATED_CONTEXT *pContext, NVOS61_PARAMETERS *pArgs);
void RmDeprecatedAllocContextDma(DEPRECATED_CONTEXT *pContext, NVOS39_PARAMETERS *pArgs);
void RmDeprecatedBindContextDma(DEPRECATED_CONTEXT *pContext, NVOS49_PARAMETERS *pArgs);
void RmDeprecatedI2CAccess(DEPRECATED_CONTEXT *pContext, NVOS_I2C_ACCESS_PARAMS *pArgs);
void RmDeprecatedIdleChannels(DEPRECATED_CONTEXT *pContext, NVOS30_PARAMETERS *pArgs);
void RmDeprecatedVidHeapControl(DEPRECATED_CONTEXT *pContext, NVOS32_PARAMETERS *pArgs);
void RmDeprecatedAllocMemory(DEPRECATED_CONTEXT *pContext, NVOS02_PARAMETERS *pArgs);


/**
 * List of utility functions (used within shims)
 */
typedef NV_STATUS (*RmDeprecatedControlHandler)(API_SECURITY_INFO*,DEPRECATED_CONTEXT*,NVOS54_PARAMETERS*);
RmDeprecatedControlHandler RmDeprecatedGetControlHandler(NVOS54_PARAMETERS *pArgs);

NV_STATUS RmDeprecatedGetHandleParent(DEPRECATED_CONTEXT *pContext, NvHandle hClient,
                                      NvHandle hObject, NvHandle *phParent);
NV_STATUS RmDeprecatedGetClassID(DEPRECATED_CONTEXT *pContext, NvHandle hClient,
                                 NvHandle hObject, NvU32 *pClassId);
NV_STATUS RmDeprecatedFindOrCreateSubDeviceHandle(DEPRECATED_CONTEXT *pContext, NvHandle hClient,
                                                  NvHandle hDeviceOrSubDevice, NvHandle *pHSubDevice,
                                                  NvBool *pBMustFree);
NV_STATUS RmDeprecatedConvertOs32ToOs02Flags(NvU32 attr, NvU32 attr2, NvU32 os32Flags, NvU32 *pOs02Flags);
NV_STATUS RmDeprecatedConvertOs02ToOs32Flags(NvU32 os02Flags, NvU32 *pAttr, NvU32 *pAttr2, NvU32 *pOs32Flags);

NV_STATUS RmDeprecatedGetOrAllocObject(DEPRECATED_CONTEXT *pContext, NvHandle hClient, NvU32 classId, NvHandle *pHObject);

NV_STATUS RmCopyUserForDeprecatedApi(RMAPI_DEPRECATED_COPY_OP op,RMAPI_DEPRECATED_BUFFER_POLICY bufPolicy,
                                     NvP64 dataPtr, NvU32 dataSize, void **ppKernelPtr, NvBool bUserModeArgs);
#endif
