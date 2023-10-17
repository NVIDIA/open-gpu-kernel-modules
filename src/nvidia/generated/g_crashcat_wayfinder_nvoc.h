#ifndef _G_CRASHCAT_WAYFINDER_NVOC_H_
#define _G_CRASHCAT_WAYFINDER_NVOC_H_
#include "nvoc/runtime.h"

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

#include "g_crashcat_wayfinder_nvoc.h"

#ifndef CRASHCAT_WAYFINDER_H
#define CRASHCAT_WAYFINDER_H

#include "nvoc/object.h"
#include "nv-crashcat.h"

struct CrashCatQueue;

#ifndef __NVOC_CLASS_CrashCatQueue_TYPEDEF__
#define __NVOC_CLASS_CrashCatQueue_TYPEDEF__
typedef struct CrashCatQueue CrashCatQueue;
#endif /* __NVOC_CLASS_CrashCatQueue_TYPEDEF__ */

#ifndef __nvoc_class_id_CrashCatQueue
#define __nvoc_class_id_CrashCatQueue 0xbaa900
#endif /* __nvoc_class_id_CrashCatQueue */


struct CrashCatReport;

#ifndef __NVOC_CLASS_CrashCatReport_TYPEDEF__
#define __NVOC_CLASS_CrashCatReport_TYPEDEF__
typedef struct CrashCatReport CrashCatReport;
#endif /* __NVOC_CLASS_CrashCatReport_TYPEDEF__ */

#ifndef __nvoc_class_id_CrashCatReport
#define __nvoc_class_id_CrashCatReport 0xde4777
#endif /* __nvoc_class_id_CrashCatReport */



struct CrashCatWayfinderHal {
    unsigned short __nvoc_HalVarIdx;
};
typedef struct CrashCatWayfinderHal CrashCatWayfinderHal;
void __nvoc_init_halspec_CrashCatWayfinderHal(CrashCatWayfinderHal*, NV_CRASHCAT_WAYFINDER_VERSION);

#ifdef NVOC_CRASHCAT_WAYFINDER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct __nvoc_inner_struc_CrashCatWayfinder_1__ {
    NvCrashCatWayfinderL0_V1 wfl0;
    NvCrashCatWayfinderL1_V1 wfl1;
};


struct CrashCatWayfinder {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Object __nvoc_base_Object;
    struct Object *__nvoc_pbase_Object;
    struct CrashCatWayfinder *__nvoc_pbase_CrashCatWayfinder;
    struct CrashCatWayfinderHal wayfinderHal;
    struct CrashCatQueue *PRIVATE_FIELD(pQueue);
    struct __nvoc_inner_struc_CrashCatWayfinder_1__ PRIVATE_FIELD(v1);
};

#ifndef __NVOC_CLASS_CrashCatWayfinder_TYPEDEF__
#define __NVOC_CLASS_CrashCatWayfinder_TYPEDEF__
typedef struct CrashCatWayfinder CrashCatWayfinder;
#endif /* __NVOC_CLASS_CrashCatWayfinder_TYPEDEF__ */

#ifndef __nvoc_class_id_CrashCatWayfinder
#define __nvoc_class_id_CrashCatWayfinder 0x085e32
#endif /* __nvoc_class_id_CrashCatWayfinder */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatWayfinder;

#define __staticCast_CrashCatWayfinder(pThis) \
    ((pThis)->__nvoc_pbase_CrashCatWayfinder)

#ifdef __nvoc_crashcat_wayfinder_h_disabled
#define __dynamicCast_CrashCatWayfinder(pThis) ((CrashCatWayfinder*)NULL)
#else //__nvoc_crashcat_wayfinder_h_disabled
#define __dynamicCast_CrashCatWayfinder(pThis) \
    ((CrashCatWayfinder*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(CrashCatWayfinder)))
#endif //__nvoc_crashcat_wayfinder_h_disabled


NV_STATUS __nvoc_objCreateDynamic_CrashCatWayfinder(CrashCatWayfinder**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_CrashCatWayfinder(CrashCatWayfinder**, Dynamic*, NvU32,
        NV_CRASHCAT_WAYFINDER_VERSION CrashCatWayfinderHal_version, NvU32 arg_wfl0);
#define __objCreate_CrashCatWayfinder(ppNewObj, pParent, createFlags, CrashCatWayfinderHal_version, arg_wfl0) \
    __nvoc_objCreate_CrashCatWayfinder((ppNewObj), staticCast((pParent), Dynamic), (createFlags), CrashCatWayfinderHal_version, arg_wfl0)

struct CrashCatQueue *crashcatWayfinderGetReportQueue_V1(struct CrashCatWayfinder *arg0);


#ifdef __nvoc_crashcat_wayfinder_h_disabled
static inline struct CrashCatQueue *crashcatWayfinderGetReportQueue(struct CrashCatWayfinder *arg0) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatWayfinder was disabled!");
    return NULL;
}
#else //__nvoc_crashcat_wayfinder_h_disabled
#define crashcatWayfinderGetReportQueue(arg0) crashcatWayfinderGetReportQueue_V1(arg0)
#endif //__nvoc_crashcat_wayfinder_h_disabled

#define crashcatWayfinderGetReportQueue_HAL(arg0) crashcatWayfinderGetReportQueue(arg0)

void crashcatWayfinderSetWFL0_V1(struct CrashCatWayfinder *arg0, NvU32 wfl0);


#ifdef __nvoc_crashcat_wayfinder_h_disabled
static inline void crashcatWayfinderSetWFL0(struct CrashCatWayfinder *arg0, NvU32 wfl0) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatWayfinder was disabled!");
}
#else //__nvoc_crashcat_wayfinder_h_disabled
#define crashcatWayfinderSetWFL0(arg0, wfl0) crashcatWayfinderSetWFL0_V1(arg0, wfl0)
#endif //__nvoc_crashcat_wayfinder_h_disabled

#define crashcatWayfinderSetWFL0_HAL(arg0, wfl0) crashcatWayfinderSetWFL0(arg0, wfl0)

NV_STATUS crashcatWayfinderConstruct_IMPL(struct CrashCatWayfinder *arg_, NvU32 arg_wfl0);

#define __nvoc_crashcatWayfinderConstruct(arg_, arg_wfl0) crashcatWayfinderConstruct_IMPL(arg_, arg_wfl0)
void crashcatWayfinderDestruct_IMPL(struct CrashCatWayfinder *arg0);

#define __nvoc_crashcatWayfinderDestruct(arg0) crashcatWayfinderDestruct_IMPL(arg0)
#undef PRIVATE_FIELD

#ifndef NVOC_CRASHCAT_WAYFINDER_H_PRIVATE_ACCESS_ALLOWED
#ifndef __nvoc_crashcat_wayfinder_h_disabled
#undef crashcatWayfinderSetWFL0
void NVOC_PRIVATE_FUNCTION(crashcatWayfinderSetWFL0)(struct CrashCatWayfinder *arg0, NvU32 wfl0);
#endif //__nvoc_crashcat_wayfinder_h_disabled

#endif // NVOC_CRASHCAT_WAYFINDER_H_PRIVATE_ACCESS_ALLOWED


#endif // CRASHCAT_VECTOR_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_CRASHCAT_WAYFINDER_NVOC_H_
