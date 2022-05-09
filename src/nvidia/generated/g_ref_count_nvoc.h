#ifndef _G_REF_COUNT_NVOC_H_
#define _G_REF_COUNT_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_ref_count_nvoc.h"

#ifndef REF_COUNT_H
#define REF_COUNT_H

/****************** Resource Manager Defines and Structures *****************\
*                                                                            *
*       Defines and structures used for the Reference-Counting Object.       *
*                                                                            *
\****************************************************************************/

#include "containers/map.h"
#include "nvoc/object.h"

#define NV_REQUESTER_INIT               NV_U64_MIN
#define NV_REQUESTER_RM                 NV_U64_MAX
#define NV_REQUESTER_CLIENT_OBJECT(c,o) (((NvU64)(c) << 32) | o)

typedef enum
{
    REFCNT_STATE_DEFAULT  = 0,
    REFCNT_STATE_ENABLED,
    REFCNT_STATE_DISABLED,
    REFCNT_STATE_ERROR,
} REFCNT_STATE;

typedef struct
{
    NvU32 numReferences;
} REFCNT_REQUESTER_ENTRY, *PREFCNT_REQUESTER_ENTRY;

MAKE_MAP(REFCNT_REQUESTER_ENTRY_MAP, REFCNT_REQUESTER_ENTRY);

typedef struct OBJREFCNT *POBJREFCNT;

#ifndef __NVOC_CLASS_OBJREFCNT_TYPEDEF__
#define __NVOC_CLASS_OBJREFCNT_TYPEDEF__
typedef struct OBJREFCNT OBJREFCNT;
#endif /* __NVOC_CLASS_OBJREFCNT_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJREFCNT
#define __nvoc_class_id_OBJREFCNT 0xf89281
#endif /* __nvoc_class_id_OBJREFCNT */



//
// XXX-IOM:
//  These callback types are good candidates to be replaced with IOM
//  functionality, where small derived classes can be created on a 'callback'
//  base interface, should that become more practical (currently, adding any
//  kind of class still requires a non-trivial amount of boilerplate to wire
//  up).
//
typedef NV_STATUS RefcntStateChangeCallback(POBJREFCNT, Dynamic *,
                                            REFCNT_STATE, REFCNT_STATE);

typedef void RefcntResetCallback(POBJREFCNT, Dynamic *, NvU64);

#ifdef NVOC_REF_COUNT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct OBJREFCNT {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Object __nvoc_base_Object;
    struct Object *__nvoc_pbase_Object;
    struct OBJREFCNT *__nvoc_pbase_OBJREFCNT;
    NvBool PDB_PROP_REFCNT_ALLOW_RECURSIVE_REQUESTS;
    Dynamic *pParent;
    NvU32 tag;
    REFCNT_REQUESTER_ENTRY_MAP requesterTree;
    REFCNT_STATE state;
    NvU32 count;
    RefcntStateChangeCallback *refcntStateChangeCallback;
    RefcntResetCallback *refcntResetCallback;
};

#ifndef __NVOC_CLASS_OBJREFCNT_TYPEDEF__
#define __NVOC_CLASS_OBJREFCNT_TYPEDEF__
typedef struct OBJREFCNT OBJREFCNT;
#endif /* __NVOC_CLASS_OBJREFCNT_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJREFCNT
#define __nvoc_class_id_OBJREFCNT 0xf89281
#endif /* __nvoc_class_id_OBJREFCNT */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJREFCNT;

#define __staticCast_OBJREFCNT(pThis) \
    ((pThis)->__nvoc_pbase_OBJREFCNT)

#ifdef __nvoc_ref_count_h_disabled
#define __dynamicCast_OBJREFCNT(pThis) ((OBJREFCNT*)NULL)
#else //__nvoc_ref_count_h_disabled
#define __dynamicCast_OBJREFCNT(pThis) \
    ((OBJREFCNT*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJREFCNT)))
#endif //__nvoc_ref_count_h_disabled

#define PDB_PROP_REFCNT_ALLOW_RECURSIVE_REQUESTS_BASE_CAST
#define PDB_PROP_REFCNT_ALLOW_RECURSIVE_REQUESTS_BASE_NAME PDB_PROP_REFCNT_ALLOW_RECURSIVE_REQUESTS

NV_STATUS __nvoc_objCreateDynamic_OBJREFCNT(OBJREFCNT**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJREFCNT(OBJREFCNT**, Dynamic*, NvU32, Dynamic * arg_pParent, NvU32 arg_tag, RefcntStateChangeCallback * arg_pStateChangeCallback, RefcntResetCallback * arg_pResetCallback);
#define __objCreate_OBJREFCNT(ppNewObj, pParent, createFlags, arg_pParent, arg_tag, arg_pStateChangeCallback, arg_pResetCallback) \
    __nvoc_objCreate_OBJREFCNT((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pParent, arg_tag, arg_pStateChangeCallback, arg_pResetCallback)

NV_STATUS refcntConstruct_IMPL(POBJREFCNT arg_pRefcnt, Dynamic *arg_pParent, NvU32 arg_tag, RefcntStateChangeCallback *arg_pStateChangeCallback, RefcntResetCallback *arg_pResetCallback);
#define __nvoc_refcntConstruct(arg_pRefcnt, arg_pParent, arg_tag, arg_pStateChangeCallback, arg_pResetCallback) refcntConstruct_IMPL(arg_pRefcnt, arg_pParent, arg_tag, arg_pStateChangeCallback, arg_pResetCallback)
void refcntDestruct_IMPL(POBJREFCNT pRefcnt);
#define __nvoc_refcntDestruct(pRefcnt) refcntDestruct_IMPL(pRefcnt)
NV_STATUS refcntRequestReference_IMPL(POBJREFCNT pRefcnt, NvU64 arg0, NvU32 arg1, NvBool arg2);
#ifdef __nvoc_ref_count_h_disabled
static inline NV_STATUS refcntRequestReference(POBJREFCNT pRefcnt, NvU64 arg0, NvU32 arg1, NvBool arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJREFCNT was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_ref_count_h_disabled
#define refcntRequestReference(pRefcnt, arg0, arg1, arg2) refcntRequestReference_IMPL(pRefcnt, arg0, arg1, arg2)
#endif //__nvoc_ref_count_h_disabled

NV_STATUS refcntReleaseReferences_IMPL(POBJREFCNT pRefcnt, NvU64 arg0, NvBool arg1);
#ifdef __nvoc_ref_count_h_disabled
static inline NV_STATUS refcntReleaseReferences(POBJREFCNT pRefcnt, NvU64 arg0, NvBool arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJREFCNT was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_ref_count_h_disabled
#define refcntReleaseReferences(pRefcnt, arg0, arg1) refcntReleaseReferences_IMPL(pRefcnt, arg0, arg1)
#endif //__nvoc_ref_count_h_disabled

NV_STATUS refcntReset_IMPL(POBJREFCNT pRefcnt, NvBool arg0);
#ifdef __nvoc_ref_count_h_disabled
static inline NV_STATUS refcntReset(POBJREFCNT pRefcnt, NvBool arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJREFCNT was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_ref_count_h_disabled
#define refcntReset(pRefcnt, arg0) refcntReset_IMPL(pRefcnt, arg0)
#endif //__nvoc_ref_count_h_disabled

#undef PRIVATE_FIELD


#endif // REF_COUNT_H

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_REF_COUNT_NVOC_H_
