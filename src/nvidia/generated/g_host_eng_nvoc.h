
#ifndef _G_HOST_ENG_NVOC_H_
#define _G_HOST_ENG_NVOC_H_
#include "nvoc/runtime.h"

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 0

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_host_eng_nvoc.h"

#ifndef HOST_ENG_H
#define HOST_ENG_H

/*!
 * @file   host_eng.h
 * @brief  Provides definitions for all OBJHOSTENG data structures and interfaces.
 */

#include "core/core.h"
#include "gpu/gpu_timeout.h"

#include "kernel/gpu/fifo/kernel_channel.h"

typedef struct OBJHOSTENG *POBJHOSTENG;

/*!
 * Interface class for all Hosteng modules.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_HOST_ENG_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct OBJHOSTENG {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Ancestor object pointers for `staticCast` feature
    struct OBJHOSTENG *__nvoc_pbase_OBJHOSTENG;    // hosteng

    // Vtable with 1 per-object function pointer
    NV_STATUS (*__hostengHaltAndReset__)(struct OBJGPU *, struct OBJHOSTENG * /*this*/, RMTIMEOUT *);  // virtual
};

#ifndef __NVOC_CLASS_OBJHOSTENG_TYPEDEF__
#define __NVOC_CLASS_OBJHOSTENG_TYPEDEF__
typedef struct OBJHOSTENG OBJHOSTENG;
#endif /* __NVOC_CLASS_OBJHOSTENG_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJHOSTENG
#define __nvoc_class_id_OBJHOSTENG 0xb356e7
#endif /* __nvoc_class_id_OBJHOSTENG */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJHOSTENG;

#define __staticCast_OBJHOSTENG(pThis) \
    ((pThis)->__nvoc_pbase_OBJHOSTENG)

#ifdef __nvoc_host_eng_h_disabled
#define __dynamicCast_OBJHOSTENG(pThis) ((OBJHOSTENG*)NULL)
#else //__nvoc_host_eng_h_disabled
#define __dynamicCast_OBJHOSTENG(pThis) \
    ((OBJHOSTENG*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJHOSTENG)))
#endif //__nvoc_host_eng_h_disabled

NV_STATUS __nvoc_objCreateDynamic_OBJHOSTENG(OBJHOSTENG**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJHOSTENG(OBJHOSTENG**, Dynamic*, NvU32);
#define __objCreate_OBJHOSTENG(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJHOSTENG((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define hostengHaltAndReset_FNPTR(pHosteng) pHosteng->__hostengHaltAndReset__
#define hostengHaltAndReset(pGpu, pHosteng, pRmTimeout) hostengHaltAndReset_DISPATCH(pGpu, pHosteng, pRmTimeout)

// Dispatch functions
static inline NV_STATUS hostengHaltAndReset_DISPATCH(struct OBJGPU *pGpu, struct OBJHOSTENG *pHosteng, RMTIMEOUT *pRmTimeout) {
    return pHosteng->__hostengHaltAndReset__(pGpu, pHosteng, pRmTimeout);
}

NV_STATUS hostengHaltAndReset_IMPL(struct OBJGPU *pGpu, struct OBJHOSTENG *pHosteng, RMTIMEOUT *pRmTimeout);

#undef PRIVATE_FIELD


#endif // HOST_ENG_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_HOST_ENG_NVOC_H_
