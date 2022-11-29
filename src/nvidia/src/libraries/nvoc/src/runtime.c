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

#include "nvport/nvport.h"

#include "nvtypes.h"

#include "nvoc/rtti.h"
#include "nvoc/runtime.h"

#include "nvoc/object.h"

#  include "utils/nvassert.h"


static NV_FORCEINLINE Dynamic *__nvoc_fullyDerive_IMPL(Dynamic *pDynamic)
{
    return (Dynamic*)((NvU8*)pDynamic - pDynamic->__nvoc_rtti->offset);
}

Dynamic *fullyDeriveWrapper(Dynamic *pDynamic)
{
    return __nvoc_fullyDerive_IMPL(pDynamic);
}

const struct NVOC_RTTI_PROVIDER __nvoc_rtti_provider = { 0 };

NVOC_CLASS_ID __nvoc_objGetClassId(Dynamic *pObj)
{
    Dynamic *pDerivedObj = __nvoc_fullyDerive(pObj);
    return pDerivedObj->__nvoc_rtti->pClassDef->classInfo.classId;
}

const NVOC_CLASS_INFO *__nvoc_objGetClassInfo(Dynamic *pObj)
{
    Dynamic *pDerivedObj = __nvoc_fullyDerive(pObj);
    return &pDerivedObj->__nvoc_rtti->pClassDef->classInfo;
}

Dynamic *objFindAncestor_IMPL(Dynamic *pDynamic, NVOC_CLASS_ID classId)
{
    Object *pObj = dynamicCast(pDynamic, Object);
    NV_ASSERT(pObj != NULL);

    while ((pObj = pObj->pParent) != NULL)
    {
        if (objDynamicCastById(pObj, classId) != NULL) return __nvoc_fullyDerive(pObj);
    }

    NV_ASSERT(0);
    return NULL;
}

void objAddChild_IMPL(Object *pObj, Object *pChild)
{
    NV_ASSERT(pChild->pParent == NULL);
    pChild->pParent = pObj;
    pChild->childTree.pSibling = pObj->childTree.pChild;
    pObj->childTree.pChild = pChild;
}

void objRemoveChild_IMPL(Object *pObj, Object *pChild)
{
    Object **ppChild;

    NV_ASSERT(pObj == pChild->pParent);
    pChild->pParent = NULL;
    ppChild = &pObj->childTree.pChild;
    while (*ppChild != NULL)
    {
        if (*ppChild == pChild)
        {
            *ppChild = pChild->childTree.pSibling;
            return;
        }

        ppChild = &(*ppChild)->childTree.pSibling;
    }
}

Object *objGetChild_IMPL(Object *pObj)
{
    NV_ASSERT(pObj != NULL);
    return pObj->childTree.pChild;
}

Object *objGetSibling_IMPL(Object *pObj)
{
    NV_ASSERT(pObj != NULL);
    return pObj->childTree.pSibling;
}

Object *objGetDirectParent_IMPL(Object *pObj)
{
    NV_ASSERT(pObj != NULL);
    return pObj->pParent;
}

//! Internal backing method for objDelete.
void __nvoc_objDelete(Dynamic *pDynamic)
{
    Dynamic *pDerivedObj;
    Object *pObj, *pChild;

    if (pDynamic == NULL)
    {
        return;
    }

    pDynamic->__nvoc_rtti->dtor(pDynamic);

    pObj = dynamicCast(pDynamic, Object);
    if (pObj->pParent != NULL)
    {
        objRemoveChild(pObj->pParent, pObj);
    }

    if ((pChild = objGetChild(pObj)) != NULL)
    {
#if NV_PRINTF_STRINGS_ALLOWED
        portDbgPrintf("NVOC: %s: Child class %s not freed from parent class %s.",
                            __FUNCTION__,
                            objGetClassInfo(pChild)->name,
                            objGetClassInfo(pObj)->name);
#endif
        PORT_BREAKPOINT_CHECKED();
    }

    pDerivedObj = __nvoc_fullyDerive(pDynamic);
    portMemFree(pDerivedObj);
}

//! Internal method to fill out an object's RTTI pointers from a class definition.
void __nvoc_initRtti(Dynamic *pNewObject, const struct NVOC_CLASS_DEF *pClassDef)
{
    NvU32 relativeIdx;
    for (relativeIdx = 0; relativeIdx < pClassDef->pCastInfo->numRelatives; relativeIdx++)
    {
        const struct NVOC_RTTI *pRelative = pClassDef->pCastInfo->relatives[relativeIdx];
        const struct NVOC_RTTI **ppRelativeRtti = &((Dynamic*)((NvU8*)pNewObject + pRelative->offset))->__nvoc_rtti;
        *ppRelativeRtti = pRelative;
    }
}

//! Internal backing method for objCreateDynamic.
NV_STATUS __nvoc_objCreateDynamic(
        Dynamic               **ppNewObject,
        Dynamic                *pParent,
        const NVOC_CLASS_INFO  *pClassInfo,
        NvU32                   createFlags,
        ...)
{
    NV_STATUS status;
    va_list   args;

    const struct NVOC_CLASS_DEF *pClassDef =
        (const struct NVOC_CLASS_DEF*)pClassInfo;

    if (pClassDef == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    else if (pClassDef->objCreatefn == NULL)
    {
        return NV_ERR_INVALID_CLASS;
    }

    va_start(args, createFlags);
    status = pClassDef->objCreatefn(ppNewObject, pParent, createFlags, args);
    va_end(args);

    return status;
}

Dynamic *objDynamicCastById_IMPL(Dynamic *pFromObj, NVOC_CLASS_ID classId)
{
    NvU32 i, numBases;
    Dynamic *pDerivedObj;

    const struct NVOC_RTTI *const   *bases;
    const struct NVOC_RTTI          *pFromRtti;
    const struct NVOC_RTTI          *pDerivedRtti;

    if (pFromObj == NULL)
    {
        return NULL;
    }

    pFromRtti = pFromObj->__nvoc_rtti;

    // fastpath, we're dynamic casting to what we already have
    if (classId == pFromRtti->pClassDef->classInfo.classId)
    {
        return pFromObj;
    }

    pDerivedObj = __nvoc_fullyDerive(pFromObj);
    pDerivedRtti = pDerivedObj->__nvoc_rtti;

    // fastpath, we're dynamic casting to the fully derived class
    if (classId == pDerivedRtti->pClassDef->classInfo.classId)
    {
        return pDerivedObj;
    }

    // slowpath, search all the possibilities for a match
    numBases = pDerivedRtti->pClassDef->pCastInfo->numRelatives;
    bases = pDerivedRtti->pClassDef->pCastInfo->relatives;

    for (i = 0; i < numBases; i++)
    {
        if (classId == bases[i]->pClassDef->classInfo.classId)
        {
            return (Dynamic*)((NvU8*)pDerivedObj + bases[i]->offset);
        }
    }

    return NULL;
}

//! Internal backing method for dynamicCast.
Dynamic *__nvoc_dynamicCast(Dynamic *pFromObj, const NVOC_CLASS_INFO *pClassInfo)
{
    return objDynamicCastById(pFromObj, pClassInfo->classId);
}

/*!
 * @brief Internal dummy destructor for non-fully-derived pointers.
 *
 * Resolves pDynamic to its most derived pointer and then calls the real
 * destructor on the fully-derived object.
 */
void __nvoc_destructFromBase(Dynamic *pDynamic)
{
    Dynamic *pDerivedObj = __nvoc_fullyDerive(pDynamic);
    pDerivedObj->__nvoc_rtti->dtor(pDerivedObj);
}

const struct NVOC_EXPORTED_METHOD_DEF* nvocGetExportedMethodDefFromMethodInfo_IMPL(const struct NVOC_EXPORT_INFO *pExportInfo, NvU32 methodId)
{
    NvU32 exportLength;
    const struct NVOC_EXPORTED_METHOD_DEF *exportArray;

    if (pExportInfo == NULL)
        return NULL;

    exportLength = pExportInfo->numEntries;
    exportArray =  pExportInfo->pExportEntries;

    if (exportArray != NULL && exportLength > 0)
    {
        // The export array is sorted by methodId, so we can binary search it
        NvU32 low = 0;
        NvU32 high = exportLength;
        while (1)
        {
            NvU32 mid  = (low + high) / 2;

            if (exportArray[mid].methodId == methodId)
                return &exportArray[mid];

            if (high == mid || low == mid)
                break;

            if (exportArray[mid].methodId > methodId)
                high = mid;
            else
                low = mid;
        }
    }

    return NULL;
}

const struct NVOC_EXPORTED_METHOD_DEF *objGetExportedMethodDef_IMPL(Dynamic *pObj, NvU32 methodId)
{
    const struct NVOC_CASTINFO *const pCastInfo = pObj->__nvoc_rtti->pClassDef->pCastInfo;
    const NvU32 numRelatives = pCastInfo->numRelatives;
    const struct NVOC_RTTI *const *relatives = pCastInfo->relatives;
    NvU32 i;

    for (i = 0; i < numRelatives; i++)
    {
        const void *pDef = nvocGetExportedMethodDefFromMethodInfo_IMPL(relatives[i]->pClassDef->pExportInfo, methodId);
        if (pDef != NULL)
            return pDef;
    }

    return NULL;
}

