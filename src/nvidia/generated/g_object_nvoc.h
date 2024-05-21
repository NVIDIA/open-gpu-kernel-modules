
#ifndef _G_OBJECT_NVOC_H_
#define _G_OBJECT_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2015-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*
 * This file is part of the NVOC runtime.
 */

#pragma once
#include "g_object_nvoc.h"

#ifndef _NVOC_OBJECT_H_
#define _NVOC_OBJECT_H_

#include "nvtypes.h"
#include "nvstatus.h"

#include "nvoc/prelude.h"


struct Object;

#ifndef __NVOC_CLASS_Object_TYPEDEF__
#define __NVOC_CLASS_Object_TYPEDEF__
typedef struct Object Object;
#endif /* __NVOC_CLASS_Object_TYPEDEF__ */

#ifndef __nvoc_class_id_Object
#define __nvoc_class_id_Object 0x497031
#endif /* __nvoc_class_id_Object */


struct NVOC_CLASS_INFO;

/*!
 * Tracks the head of an object's child list, and the next object in its
 * parent's child list.
 */
struct NVOC_CHILD_TREE
{
    struct Object *pChild;
    struct Object *pSibling;
};

//! The base class of all instantiable NVOC objects.

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_OBJECT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct Object {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj

    // Data members
    struct Object *pParent;
    struct NVOC_CHILD_TREE childTree;
    NvU32 ipVersion;
    NvU32 createFlags;
};

#ifndef __NVOC_CLASS_Object_TYPEDEF__
#define __NVOC_CLASS_Object_TYPEDEF__
typedef struct Object Object;
#endif /* __NVOC_CLASS_Object_TYPEDEF__ */

#ifndef __nvoc_class_id_Object
#define __nvoc_class_id_Object 0x497031
#endif /* __nvoc_class_id_Object */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

#define __staticCast_Object(pThis) \
    ((pThis)->__nvoc_pbase_Object)

#ifdef __nvoc_object_h_disabled
#define __dynamicCast_Object(pThis) ((Object*)NULL)
#else //__nvoc_object_h_disabled
#define __dynamicCast_Object(pThis) \
    ((Object*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Object)))
#endif //__nvoc_object_h_disabled

NV_STATUS __nvoc_objCreateDynamic_Object(Object**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Object(Object**, Dynamic*, NvU32);
#define __objCreate_Object(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_Object((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
void objAddChild_IMPL(struct Object *pObj, struct Object *pChild);

#ifdef __nvoc_object_h_disabled
static inline void objAddChild(struct Object *pObj, struct Object *pChild) {
    NV_ASSERT_FAILED_PRECOMP("Object was disabled!");
}
#else //__nvoc_object_h_disabled
#define objAddChild(pObj, pChild) objAddChild_IMPL(pObj, pChild)
#endif //__nvoc_object_h_disabled

void objRemoveChild_IMPL(struct Object *pObj, struct Object *pChild);

#ifdef __nvoc_object_h_disabled
static inline void objRemoveChild(struct Object *pObj, struct Object *pChild) {
    NV_ASSERT_FAILED_PRECOMP("Object was disabled!");
}
#else //__nvoc_object_h_disabled
#define objRemoveChild(pObj, pChild) objRemoveChild_IMPL(pObj, pChild)
#endif //__nvoc_object_h_disabled

struct Object *objGetChild_IMPL(struct Object *pObj);

#ifdef __nvoc_object_h_disabled
static inline struct Object *objGetChild(struct Object *pObj) {
    NV_ASSERT_FAILED_PRECOMP("Object was disabled!");
    return NULL;
}
#else //__nvoc_object_h_disabled
#define objGetChild(pObj) objGetChild_IMPL(pObj)
#endif //__nvoc_object_h_disabled

struct Object *objGetSibling_IMPL(struct Object *pObj);

#ifdef __nvoc_object_h_disabled
static inline struct Object *objGetSibling(struct Object *pObj) {
    NV_ASSERT_FAILED_PRECOMP("Object was disabled!");
    return NULL;
}
#else //__nvoc_object_h_disabled
#define objGetSibling(pObj) objGetSibling_IMPL(pObj)
#endif //__nvoc_object_h_disabled

struct Object *objGetDirectParent_IMPL(struct Object *pObj);

#ifdef __nvoc_object_h_disabled
static inline struct Object *objGetDirectParent(struct Object *pObj) {
    NV_ASSERT_FAILED_PRECOMP("Object was disabled!");
    return NULL;
}
#else //__nvoc_object_h_disabled
#define objGetDirectParent(pObj) objGetDirectParent_IMPL(pObj)
#endif //__nvoc_object_h_disabled

#undef PRIVATE_FIELD


//
// IP versioning definitions are temporary until NVOC halspec support is
// finished.
//
// IP_VERSION format as defined by the hardware engines.
// A _MAJOR value of 0 means the object has no version number.
//

#define NV_ODB_IP_VER_DEV                                       7:0 /* R-IVF */
#define NV_ODB_IP_VER_ECO                                      15:8 /* R-IVF */
#define NV_ODB_IP_VER_MINOR                                   23:16 /* R-IVF */
#define NV_ODB_IP_VER_MAJOR                                   31:24 /* R-IVF */

#define IPVersion(pObj)                            staticCast((pObj), Object)->ipVersion
//  v0 .. v1  inclusive
#define IsIPVersionInRange(pObj, v0, v1)           ((IPVersion(pObj) >= (v0)) && (IPVersion(pObj) <= (v1)))

#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_OBJECT_NVOC_H_
