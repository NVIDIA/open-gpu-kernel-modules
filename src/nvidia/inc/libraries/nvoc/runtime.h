/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NVOC_RUNTIME_H_
#define _NVOC_RUNTIME_H_

#include "nvport/nvport.h"
#include "nvtypes.h"
#include "nvstatus.h"
#include "nvmisc.h"

#include "nvoc/object.h"

#ifdef __cplusplus
extern "C" {
#endif

NVOC_CLASS_ID __nvoc_objGetClassId(Dynamic *pObj);
const NVOC_CLASS_INFO *__nvoc_objGetClassInfo(Dynamic *pObj);

void __nvoc_objDelete(Dynamic *pDynamic);

NV_STATUS __nvoc_handleObjCreateMemAlloc(NvU32 createFlags, NvU32 allocSize, void **ppLocalPtr, void **ppThis);

NV_STATUS __nvoc_objCreateDynamic(
        Dynamic               **pNewObject,
        Dynamic                *pParent,
        const NVOC_CLASS_INFO  *pClassInfo,
        NvU32                   createFlags,
        ...);

Dynamic *__nvoc_dynamicCast(Dynamic *pFromObj, const NVOC_CLASS_INFO *pClassInfo);
Dynamic *__nvoc_dynamicCastById(Dynamic *pFromObj, NVOC_CLASS_ID classId);

void __nvoc_destructFromBase(Dynamic *pDynamic);

Dynamic *fullyDeriveWrapper(Dynamic *pDynamic);

extern const NVOC_RTTI_PROVIDER __nvoc_rtti_provider;

#define objFindAncestor(pObj, classId)          objFindAncestor_IMPL(staticCast((pObj), Dynamic), classId)
#define objDynamicCastById(pObj, classId)       objDynamicCastById_IMPL(staticCast((pObj), Dynamic), classId)
#define objFindAncestorOfType(TYPE, pObj)       dynamicCast(objFindAncestor((pObj), classId(TYPE)), TYPE)
#define __nvoc_fullyDerive(pObj)                __nvoc_fullyDerive_IMPL(staticCast((pObj), Dynamic))
#define objFullyDerive(pObj)                    fullyDeriveWrapper(staticCast((pObj), Dynamic))
#define objGetExportedMethodDef(pObj, methodId) objGetExportedMethodDef_IMPL(pObj, methodId)

//! Contains data needed to call the exported method from kernel
struct NVOC_EXPORTED_METHOD_DEF
{
    void (*pFunc) (void);                         // Pointer to the method itself
    NvU32 flags;                                  // Export flags used for permission, method attribute verification (eg. NO_LOCK, PRIVILEGED...)
    NvU32 accessRight;                            // Access rights required for this method
    NvU32 methodId;                               // Id of the method in the class. Used for method identification.
    NvU32 paramSize;                              // Size of the parameter structure that the method takes as the argument (0 if it takes no arguments)
    const NVOC_CLASS_INFO* pClassInfo;            // Class info for the parent class of the method

#if NV_PRINTF_STRINGS_ALLOWED
    const char  *func;                            // Debug info
#endif
};

struct NVOC_EXPORT_INFO {
    NvU32 numEntries;                             // Number of entries
    const struct NVOC_EXPORTED_METHOD_DEF *pExportEntries; //An array of exported methods
};

/*!
 * @brief Finds the closest ancestor of this object with the given class ID.
 *
 * This is a linear-time operation.
 */
Dynamic *objFindAncestor_IMPL(Dynamic *pDynamic, NVOC_CLASS_ID classId);

/*!
 * @brief Finds the exported method with the given method ID.
 *
 * If the method isn't found in the derived class, we search the ancestors.
 * Returns NULL if the search is unsuccessful.
 * This is a linear-time operation.
 */
const struct NVOC_EXPORTED_METHOD_DEF* objGetExportedMethodDef_IMPL(Dynamic* pObj, NvU32 methodId);
const struct NVOC_EXPORTED_METHOD_DEF* nvocGetExportedMethodDefFromMethodInfo_IMPL(const struct NVOC_EXPORT_INFO *pExportInfo, NvU32 methodId);

/*!
 * @brief Dynamic cast by class id
 */
Dynamic *objDynamicCastById_IMPL(Dynamic *pFromObj, NVOC_CLASS_ID classId);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
