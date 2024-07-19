
#ifndef _G_HAL_MGR_NVOC_H_
#define _G_HAL_MGR_NVOC_H_
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

#pragma once
#include "g_hal_mgr_nvoc.h"

#ifndef _HAL_MGR_H_
#define _HAL_MGR_H_

#include "core/core.h"
#include "core/hal.h"

#define HALMGR_GET_HAL(p, halid)     halmgrGetHal((p), halid)


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_HAL_MGR_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct OBJHALMGR {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct OBJHALMGR *__nvoc_pbase_OBJHALMGR;    // halmgr

    // Data members
    struct OBJHAL *pHalList[71];
};

#ifndef __NVOC_CLASS_OBJHALMGR_TYPEDEF__
#define __NVOC_CLASS_OBJHALMGR_TYPEDEF__
typedef struct OBJHALMGR OBJHALMGR;
#endif /* __NVOC_CLASS_OBJHALMGR_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJHALMGR
#define __nvoc_class_id_OBJHALMGR 0xbf26de
#endif /* __nvoc_class_id_OBJHALMGR */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJHALMGR;

#define __staticCast_OBJHALMGR(pThis) \
    ((pThis)->__nvoc_pbase_OBJHALMGR)

#ifdef __nvoc_hal_mgr_h_disabled
#define __dynamicCast_OBJHALMGR(pThis) ((OBJHALMGR*)NULL)
#else //__nvoc_hal_mgr_h_disabled
#define __dynamicCast_OBJHALMGR(pThis) \
    ((OBJHALMGR*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJHALMGR)))
#endif //__nvoc_hal_mgr_h_disabled

NV_STATUS __nvoc_objCreateDynamic_OBJHALMGR(OBJHALMGR**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJHALMGR(OBJHALMGR**, Dynamic*, NvU32);
#define __objCreate_OBJHALMGR(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJHALMGR((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
NV_STATUS halmgrConstruct_IMPL(struct OBJHALMGR *arg_);

#define __nvoc_halmgrConstruct(arg_) halmgrConstruct_IMPL(arg_)
void halmgrDestruct_IMPL(struct OBJHALMGR *arg1);

#define __nvoc_halmgrDestruct(arg1) halmgrDestruct_IMPL(arg1)
NV_STATUS halmgrCreateHal_IMPL(struct OBJHALMGR *arg1, NvU32 arg2);

#ifdef __nvoc_hal_mgr_h_disabled
static inline NV_STATUS halmgrCreateHal(struct OBJHALMGR *arg1, NvU32 arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJHALMGR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_hal_mgr_h_disabled
#define halmgrCreateHal(arg1, arg2) halmgrCreateHal_IMPL(arg1, arg2)
#endif //__nvoc_hal_mgr_h_disabled

NV_STATUS halmgrGetHalForGpu_IMPL(struct OBJHALMGR *arg1, NvU32 arg2, NvU32 arg3, NvU32 *arg4);

#ifdef __nvoc_hal_mgr_h_disabled
static inline NV_STATUS halmgrGetHalForGpu(struct OBJHALMGR *arg1, NvU32 arg2, NvU32 arg3, NvU32 *arg4) {
    NV_ASSERT_FAILED_PRECOMP("OBJHALMGR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_hal_mgr_h_disabled
#define halmgrGetHalForGpu(arg1, arg2, arg3, arg4) halmgrGetHalForGpu_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_hal_mgr_h_disabled

struct OBJHAL *halmgrGetHal_IMPL(struct OBJHALMGR *arg1, NvU32 arg2);

#ifdef __nvoc_hal_mgr_h_disabled
static inline struct OBJHAL *halmgrGetHal(struct OBJHALMGR *arg1, NvU32 arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJHALMGR was disabled!");
    return NULL;
}
#else //__nvoc_hal_mgr_h_disabled
#define halmgrGetHal(arg1, arg2) halmgrGetHal_IMPL(arg1, arg2)
#endif //__nvoc_hal_mgr_h_disabled

#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_HAL_MGR_NVOC_H_
