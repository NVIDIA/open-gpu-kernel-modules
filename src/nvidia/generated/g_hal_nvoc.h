
#ifndef _G_HAL_NVOC_H_
#define _G_HAL_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_hal_nvoc.h"

#ifndef _OBJHAL_H_
#define _OBJHAL_H_

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
* Module: hal.h                                                             *
*       Defines and structures used for the HAL Object.                     *
*                                                                           *
\***************************************************************************/

#include "core/core.h"

//
// HAL Info Block Id:
//
//  31                                             7                0
//  .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  | 24 bits                                     | 8 bits          |
//  .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   Info ID #                                     Impl
//
// Impl: The hal implementation
// Info ID number: unique id for a particular info type
//
#define MKHALINFOID(impl,infoId)   (((infoId & 0xffffff) << 8) | (impl & 0xff))

typedef struct MODULEDESCRIPTOR MODULEDESCRIPTOR, *PMODULEDESCRIPTOR;

struct MODULEDESCRIPTOR {

    // (rmconfig) per-obj function ptr to init hal interfaces
    const HAL_IFACE_SETUP *pHalSetIfaces;
};

typedef struct OBJHAL *POBJHAL;

#ifndef __NVOC_CLASS_OBJHAL_TYPEDEF__
#define __NVOC_CLASS_OBJHAL_TYPEDEF__
typedef struct OBJHAL OBJHAL;
#endif /* __NVOC_CLASS_OBJHAL_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJHAL
#define __nvoc_class_id_OBJHAL 0xe803b6
#endif /* __nvoc_class_id_OBJHAL */



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_HAL_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct OBJHAL {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct OBJHAL *__nvoc_pbase_OBJHAL;    // objhal

    // Data members
    struct MODULEDESCRIPTOR moduleDescriptor;
};

#ifndef __NVOC_CLASS_OBJHAL_TYPEDEF__
#define __NVOC_CLASS_OBJHAL_TYPEDEF__
typedef struct OBJHAL OBJHAL;
#endif /* __NVOC_CLASS_OBJHAL_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJHAL
#define __nvoc_class_id_OBJHAL 0xe803b6
#endif /* __nvoc_class_id_OBJHAL */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJHAL;

#define __staticCast_OBJHAL(pThis) \
    ((pThis)->__nvoc_pbase_OBJHAL)

#ifdef __nvoc_hal_h_disabled
#define __dynamicCast_OBJHAL(pThis) ((OBJHAL*)NULL)
#else //__nvoc_hal_h_disabled
#define __dynamicCast_OBJHAL(pThis) \
    ((OBJHAL*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJHAL)))
#endif //__nvoc_hal_h_disabled

NV_STATUS __nvoc_objCreateDynamic_OBJHAL(OBJHAL**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJHAL(OBJHAL**, Dynamic*, NvU32);
#define __objCreate_OBJHAL(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJHAL((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
PMODULEDESCRIPTOR objhalGetModuleDescriptor_IMPL(struct OBJHAL *pHal);

#ifdef __nvoc_hal_h_disabled
static inline PMODULEDESCRIPTOR objhalGetModuleDescriptor(struct OBJHAL *pHal) {
    NV_ASSERT_FAILED_PRECOMP("OBJHAL was disabled!");
    return NULL;
}
#else //__nvoc_hal_h_disabled
#define objhalGetModuleDescriptor(pHal) objhalGetModuleDescriptor_IMPL(pHal)
#endif //__nvoc_hal_h_disabled

#undef PRIVATE_FIELD


#endif // _OBJHAL_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_HAL_NVOC_H_
