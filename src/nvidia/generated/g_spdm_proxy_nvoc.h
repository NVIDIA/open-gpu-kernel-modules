
#ifndef _G_SPDM_PROXY_NVOC_H_
#define _G_SPDM_PROXY_NVOC_H_

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#pragma once
#include "g_spdm_proxy_nvoc.h"

#ifndef SPDM_PROXY_H
#define SPDM_PROXY_H

#include "gpu/gpu.h"
#include "gpu/eng_state.h"
#include "ctrl/ctrl2080/ctrl2080spdm.h"
#include "ctrl/ctrl2080/ctrl2080internal.h"

/****************************************************************************\
 *                                                                           *
 *      SPDM Proxy module header.                                            *
 *                                                                           *
 ****************************************************************************/


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_SPDM_PROXY_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI
struct NVOC_METADATA__SpdmProxy;


struct SpdmProxy {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__SpdmProxy *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Ancestor object pointers for `staticCast` feature
    struct SpdmProxy *__nvoc_pbase_SpdmProxy;    // spdmProxy
};


// Metadata with per-class RTTI
struct NVOC_METADATA__SpdmProxy {
    const struct NVOC_RTTI rtti;
};

#ifndef __NVOC_CLASS_SpdmProxy_TYPEDEF__
#define __NVOC_CLASS_SpdmProxy_TYPEDEF__
typedef struct SpdmProxy SpdmProxy;
#endif /* __NVOC_CLASS_SpdmProxy_TYPEDEF__ */

#ifndef __nvoc_class_id_SpdmProxy
#define __nvoc_class_id_SpdmProxy 0x5b722d
#endif /* __nvoc_class_id_SpdmProxy */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_SpdmProxy;

#define __staticCast_SpdmProxy(pThis) \
    ((pThis)->__nvoc_pbase_SpdmProxy)

#ifdef __nvoc_spdm_proxy_h_disabled
#define __dynamicCast_SpdmProxy(pThis) ((SpdmProxy*) NULL)
#else //__nvoc_spdm_proxy_h_disabled
#define __dynamicCast_SpdmProxy(pThis) \
    ((SpdmProxy*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(SpdmProxy)))
#endif //__nvoc_spdm_proxy_h_disabled

NV_STATUS __nvoc_objCreateDynamic_SpdmProxy(SpdmProxy**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_SpdmProxy(SpdmProxy**, Dynamic*, NvU32);
#define __objCreate_SpdmProxy(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_SpdmProxy((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
NV_STATUS spdmProxyCtrlSpdmPartition_IMPL(struct OBJGPU *pGpu, NV2080_CTRL_INTERNAL_SPDM_PARTITION_PARAMS *pSpdmPartitionParams);


#define spdmProxyCtrlSpdmPartition(pGpu, pSpdmPartitionParams) spdmProxyCtrlSpdmPartition_IMPL(pGpu, pSpdmPartitionParams)
#define spdmProxyCtrlSpdmPartition_HAL(pGpu, pSpdmPartitionParams) spdmProxyCtrlSpdmPartition(pGpu, pSpdmPartitionParams)

#undef PRIVATE_FIELD


#endif // SPDM_PROXY_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_SPDM_PROXY_NVOC_H_
