
#ifndef _G_CRASHCAT_QUEUE_NVOC_H_
#define _G_CRASHCAT_QUEUE_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_crashcat_queue_nvoc.h"

#ifndef CRASHCAT_QUEUE_H
#define CRASHCAT_QUEUE_H

#include "nvoc/object.h"
#include "nv-crashcat.h"
#include "crashcat/crashcat_wayfinder.h" // for CrashCatWayfinderHal spec


struct CrashCatEngine;

#ifndef __NVOC_CLASS_CrashCatEngine_TYPEDEF__
#define __NVOC_CLASS_CrashCatEngine_TYPEDEF__
typedef struct CrashCatEngine CrashCatEngine;
#endif /* __NVOC_CLASS_CrashCatEngine_TYPEDEF__ */

#ifndef __nvoc_class_id_CrashCatEngine
#define __nvoc_class_id_CrashCatEngine 0x654166
#endif /* __nvoc_class_id_CrashCatEngine */



struct CrashCatReport;

#ifndef __NVOC_CLASS_CrashCatReport_TYPEDEF__
#define __NVOC_CLASS_CrashCatReport_TYPEDEF__
typedef struct CrashCatReport CrashCatReport;
#endif /* __NVOC_CLASS_CrashCatReport_TYPEDEF__ */

#ifndef __nvoc_class_id_CrashCatReport
#define __nvoc_class_id_CrashCatReport 0xde4777
#endif /* __nvoc_class_id_CrashCatReport */



typedef struct
{
    NV_CRASHCAT_MEM_APERTURE aperture;
    NvU32 size;
    NvU64 offset;

    NvU32 putRegOffset;
    NvU32 getRegOffset;
} CrashCatQueueConfig;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_CRASHCAT_QUEUE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct CrashCatQueue {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct CrashCatQueue *__nvoc_pbase_CrashCatQueue;    // crashcatQueue

    // Data members
    CrashCatQueueConfig PRIVATE_FIELD(config);
    struct CrashCatEngine *PRIVATE_FIELD(pEngine);
    void *PRIVATE_FIELD(pMapping);
};

#ifndef __NVOC_CLASS_CrashCatQueue_TYPEDEF__
#define __NVOC_CLASS_CrashCatQueue_TYPEDEF__
typedef struct CrashCatQueue CrashCatQueue;
#endif /* __NVOC_CLASS_CrashCatQueue_TYPEDEF__ */

#ifndef __nvoc_class_id_CrashCatQueue
#define __nvoc_class_id_CrashCatQueue 0xbaa900
#endif /* __nvoc_class_id_CrashCatQueue */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatQueue;

#define __staticCast_CrashCatQueue(pThis) \
    ((pThis)->__nvoc_pbase_CrashCatQueue)

#ifdef __nvoc_crashcat_queue_h_disabled
#define __dynamicCast_CrashCatQueue(pThis) ((CrashCatQueue*)NULL)
#else //__nvoc_crashcat_queue_h_disabled
#define __dynamicCast_CrashCatQueue(pThis) \
    ((CrashCatQueue*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(CrashCatQueue)))
#endif //__nvoc_crashcat_queue_h_disabled

NV_STATUS __nvoc_objCreateDynamic_CrashCatQueue(CrashCatQueue**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_CrashCatQueue(CrashCatQueue**, Dynamic*, NvU32, CrashCatQueueConfig * arg_pQueueConfig);
#define __objCreate_CrashCatQueue(ppNewObj, pParent, createFlags, arg_pQueueConfig) \
    __nvoc_objCreate_CrashCatQueue((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pQueueConfig)


// Wrapper macros

// Dispatch functions
struct CrashCatReport *crashcatQueueConsumeNextReport_V1(struct CrashCatQueue *arg1);


#ifdef __nvoc_crashcat_queue_h_disabled
static inline struct CrashCatReport *crashcatQueueConsumeNextReport(struct CrashCatQueue *arg1) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatQueue was disabled!");
    return NULL;
}
#else //__nvoc_crashcat_queue_h_disabled
#define crashcatQueueConsumeNextReport(arg1) crashcatQueueConsumeNextReport_V1(arg1)
#endif //__nvoc_crashcat_queue_h_disabled

#define crashcatQueueConsumeNextReport_HAL(arg1) crashcatQueueConsumeNextReport(arg1)

NV_STATUS crashcatQueueConstruct_IMPL(struct CrashCatQueue *arg_, CrashCatQueueConfig *arg_pQueueConfig);

#define __nvoc_crashcatQueueConstruct(arg_, arg_pQueueConfig) crashcatQueueConstruct_IMPL(arg_, arg_pQueueConfig)
void crashcatQueueDestruct_IMPL(struct CrashCatQueue *arg1);

#define __nvoc_crashcatQueueDestruct(arg1) crashcatQueueDestruct_IMPL(arg1)
#undef PRIVATE_FIELD


#endif // CRASHCAT_QUEUE_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_CRASHCAT_QUEUE_NVOC_H_
