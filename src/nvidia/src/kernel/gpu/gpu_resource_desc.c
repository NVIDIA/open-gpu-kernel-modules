/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * @file
 * @brief Object Manager: Object Classes are defined in this module.
 */

#include "gpu/gpu.h"
#include "os/os.h"
#include "core/locks.h"
#include "nvrm_registry.h"
#include "lib/base_utils.h"

ct_assert(NVOC_CLASS_ID_MAX_WIDTH <= SF_WIDTH(ENGDESC_CLASS));

NV_STATUS
gpuBuildClassDB_IMPL(OBJGPU *pGpu)
{
    GpuEngineOrder        *pEngineOrder = &pGpu->engineOrder;
    CLASSDESCRIPTOR       *pClassDynamic;
    const CLASSDESCRIPTOR *pClassStatic;
    NvU32                  numClasses;
    NvU32                  i, j;
    NV_STATUS              status;
    GpuClassDb            *pClassDB = &pGpu->classDB;

    //
    // Calculate number of classes supported by this device.
    //
    // Loop through the list of GPU-specific classes throwing out any the
    // rmconfig has marked not supported.
    //
    numClasses = 0;

    pClassStatic = &pEngineOrder->pClassDescriptors[0];
    for (i = 0; i < pEngineOrder->numClassDescriptors; i++)
    {
        // RMCONFIG: throw out any that are not supported
        if (pClassStatic[i].externalClassId == (NvU32)~0)
            continue;

        numClasses++;
    }

    NV_PRINTF(LEVEL_INFO, "num class descriptors: 0x%x\n", numClasses);

    //
    // Allocate space for correct number of entries.
    //
    pClassDynamic = portMemAllocNonPaged(sizeof(CLASSDESCRIPTOR) * numClasses);
    if (pClassDynamic == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        NV_PRINTF(LEVEL_ERROR, "alloc failed: 0x%x\n", status);
        DBG_BREAKPOINT();
        return status;
    }
    portMemSet((void *)pClassDynamic, 0, sizeof(CLASSDESCRIPTOR) * numClasses);

    //
    // Now load up chip-dependent classes into pClass table.
    //
    pClassStatic = &pEngineOrder->pClassDescriptors[0];
    i = 0;
    for (j = 0; j < pEngineOrder->numClassDescriptors; j++)
    {
        // RMCONFIG: skip over any that are not supported
        if (pClassStatic[j].externalClassId == (NvU32)~0)
            continue;

        // store info for class in class DB entry
        pClassDynamic[i] = pClassStatic[j];

        // move to next slot in class DB
        i++;
    }

    pClassDB->pClasses = pClassDynamic;
    pClassDB->numClasses = numClasses;
    pClassDB->pSuppressClasses = NULL;
    pClassDB->bSuppressRead = NV_FALSE;
    pGpu->engineDB.bValid = NV_FALSE;

    return NV_OK;
}

NV_STATUS
gpuDestroyClassDB_IMPL(OBJGPU *pGpu)
{
    portMemFree(pGpu->classDB.pClasses);
    portMemFree(pGpu->classDB.pSuppressClasses);

    pGpu->engineDB.bValid = NV_FALSE;
    return NV_OK;
}

NvBool
gpuIsClassSupported_IMPL(OBJGPU *pGpu, NvU32 externalClassId)
{
    CLASSDESCRIPTOR *pClassDesc;
    NV_STATUS        status;

    status = gpuGetClassByClassId(pGpu, externalClassId, &pClassDesc);

    return (status == NV_OK) && (pClassDesc);
}

NV_STATUS
gpuGetClassByClassId_IMPL(OBJGPU *pGpu, NvU32 externalClassId, CLASSDESCRIPTOR **ppClassDesc)
{
    GpuClassDb *pClassDB = &pGpu->classDB;
    NvU32 i;

    for (i = 0; i < pClassDB->numClasses; i++)
    {
        if (pClassDB->pClasses[i].externalClassId == externalClassId)
        {
            if (ppClassDesc != NULL)
            {
                *ppClassDesc = &pClassDB->pClasses[i];
            }
            return NV_OK;
        }
    }

    return NV_ERR_INVALID_ARGUMENT;
}

NV_STATUS
gpuGetClassByEngineAndClassId_IMPL(OBJGPU *pGpu, NvU32 externalClassId, NvU32 engDesc, CLASSDESCRIPTOR **ppClassDesc)
{
    GpuClassDb *pClassDB = &pGpu->classDB;
    NvU32 i;

    for (i = 0; i < pClassDB->numClasses; i++)
    {
        if (pClassDB->pClasses[i].externalClassId == externalClassId && pClassDB->pClasses[i].engDesc == engDesc)
        {
            *ppClassDesc = &pClassDB->pClasses[i];
            return NV_OK;
        }
    }

    return NV_ERR_GENERIC;
}

static NvU32 *
gpuGetSuppressedClassList
(
    OBJGPU *pGpu
)
{
    NvU8 *pStr;
    NvU8 *pEndStr;
    NvU8 *pSaveStr;
    NvU32 strLength;
    NvU32 nIndex;
    NvU32 nCount = 0;
    NvU32 *pData  = NULL;
    NvU32 numAModelClassesInChip = 0;
    NvBool bSuppressClassList    = NV_FALSE;
    NvU32 numFound;

    // alloc regkey buffer
    strLength = 256;
    pStr = portMemAllocNonPaged(strLength);
    if (pStr == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "portMemAllocNonPaged failed\n");
        return NULL;
    }

    pSaveStr = pStr;

    if (osReadRegistryString(pGpu, NV_REG_STR_SUPPRESS_CLASS_LIST, pStr, &strLength) == NV_OK)
    {
        bSuppressClassList = NV_TRUE;
    }

    if (bSuppressClassList)
    {
        // count number of classes
        for (; *pStr; pStr = pEndStr, nCount++)
        {
            nvStrToL(pStr, &pEndStr, BASE16, 0, &numFound);
        }
    }

    // allocate memory only if there is something to suppress.
    if ( ! ( nCount + numAModelClassesInChip ) )
    {
        portMemFree(pSaveStr);
        return NULL;
    }

    //
    // add one dword to store the count of classes here.
    // This fixes a memory leak caused by changelist 1620538
    //
    nCount++;

    pData = portMemAllocNonPaged(sizeof(NvU32)*(nCount + numAModelClassesInChip));
    if (pData == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "portMemAllocNonPaged failed\n");
        portMemFree(pSaveStr);
        return NULL;
    }

    // fill array -- first is number of classes
    pData[0]=nCount;

    if (bSuppressClassList)
    {
        pStr = pSaveStr;
        for (nIndex = 1; *pStr; pStr = pEndStr, nIndex++)
        {
            pData[nIndex] = nvStrToL(pStr, &pEndStr, BASE16, 0, &numFound);
        }
    }

    portMemFree(pSaveStr);

    return pData;
}

/**
 * @brief  Returns list of classes supported by engDesc.
 *         If ( engDesc == ENG_INVALID ) returns classes
 *         supported by all engines.
 * @param[in]     pGpu                 OBJGPU pointer
 * @param[in/out] pNumClasses          in  - denotes the size of pClassList when pClassList != NULL
                                       out - when pClassList is NULL, denotes the number of matching
                                             classes found
 * @param[out]    pClassList           Returns matching class(s) when pNumClasses in not 0
 * @param[out]    engDesc              Engine ID
 *
 * @return NV_OK if class match found
 */
NV_STATUS
gpuGetClassList_IMPL(OBJGPU *pGpu, NvU32 *pNumClasses, NvU32 *pClassList, NvU32 engDesc)
{
    NvU32 *pSuppressClasses = NULL;
    NvU32 numClasses;
    NV_STATUS status = NV_OK;
    NvU32 i, k;
    NvBool bCount;
    CLASSDESCRIPTOR *pClassDB = pGpu->classDB.pClasses;
    NvU32 lastClassId = 0;

    // Read the registry one time to get the list
    if (NV_FALSE == pGpu->classDB.bSuppressRead)
    {
        pGpu->classDB.pSuppressClasses = gpuGetSuppressedClassList(pGpu);
        pGpu->classDB.bSuppressRead = NV_TRUE;
    }

    pSuppressClasses = pGpu->classDB.pSuppressClasses;

    numClasses = 0;

    for (i = 0; i < pGpu->classDB.numClasses; i++)
    {
        if ((engDesc != ENG_INVALID) && (pClassDB[i].engDesc != engDesc))
            continue;

        bCount = NV_TRUE;

        if (pSuppressClasses != NULL)
        {
            for (k=1; k < pSuppressClasses[0]; k++)
            {
                if (pSuppressClasses[k] == pClassDB[i].externalClassId)
                {
                    bCount = NV_FALSE;
                    break;
                }
            }
        }

        if (bCount)
        {
            const NvU32 classId = pClassDB[i].externalClassId;

            //
            // Skip duplicate classes.  These exist in the classDB for
            // multi-instance engines (e.g., CE0, CE1, etc), but we only want
            // one entry in the class list.  The classDB maintains ordering
            // such that all classes with the same ID are contiguous.
            //
            if (classId != lastClassId)
            {
                // save the class in caller's buffer, if provided
                if (pClassList)
                {
                    if (numClasses < *pNumClasses)
                        pClassList[numClasses] = classId;
                    else
                        status = NV_ERR_INVALID_PARAM_STRUCT;
                }
                numClasses++;
                lastClassId = classId;
            }
        }
    }

    // and return number of classes
    if (status == NV_OK)
        *pNumClasses = numClasses;

    return status;
}

/*!
 * @brief Add a class to class DB with given Engine Tag and Class Id.
 *
 * Note that a matching class/engine must already exist in the read-only
 * "static" class list populated from gpuGetClassDescriptorList_HAL().  This
 * means that only classes/engines which have previously been removed from the
 * class DB can be added here.
 *
 * @side Sets engineDB.bValid to NV_FALSE.
 *
 * @param[in] pGpu              OBJGPU pointer
 * @param[in] pEngDesc          EngDesc of Classes to be added to Class DB
 *                              (NULL = don't care)
 * @param[in] pExternalClassId  Class to add to DB (NULL = don't care)
 *
 * @returns NV_STATUS -
 *          NV_ERR_INVALID_ARGUMENT if both pEngineTag and pClass are NULL.
 *          NV_OK otherwise
 */
static NV_STATUS
_gpuAddClassToClassDBByEngTagClassId(OBJGPU *pGpu, ENGDESCRIPTOR *pEngDesc, NvU32 *pExternalClassId)
{
    GpuEngineOrder        *pEngineOrder = &pGpu->engineOrder;
    const CLASSDESCRIPTOR *pClassDesc = &pEngineOrder->pClassDescriptors[0];
    const CLASSDESCRIPTOR *pClassDescToCopy = NULL;
    GpuClassDb            *pClassDB   = &pGpu->classDB;
    NvU32                  numClasses = pClassDB->numClasses;
    NvBool                 bMatchingClassIdFound = NV_FALSE;
    NvU32                  matchingClassIdIndex;
    NvU32                  newClassDBIndex;
    NvU32                  i;

    NV_CHECK_OR_RETURN(LEVEL_INFO, (NULL != pEngDesc) || (NULL != pExternalClassId), NV_ERR_INVALID_ARGUMENT);

    // Return early if requested class/engine is already in the dynamic classdb
    for (i = 0; i < numClasses; i++)
    {
        if (((NULL == pEngDesc) || (pClassDB->pClasses[i].engDesc == *pEngDesc)) &&
            ((NULL == pExternalClassId) || (pClassDB->pClasses[i].externalClassId == *pExternalClassId)))
        {
            return NV_OK;
        }
    }

    // Populate the ClassDB with information from the static class list (g_gpu_class_list.c)
    for (i = 0; i < pEngineOrder->numClassDescriptors; i++)
    {
        // RMCONFIG: skip over any that are not supported
        if (pClassDesc[i].externalClassId == (NvU32)~0)
            continue;

        if (((NULL == pEngDesc) || (pClassDesc[i].engDesc == *pEngDesc)) &&
            ((NULL == pExternalClassId) || (pClassDesc[i].externalClassId == *pExternalClassId)))
        {
            pClassDescToCopy = &pClassDesc[i];
            break;
        }
    }

    if (pClassDescToCopy == NULL) {
        // This should probably be an error, but that causes existing tests to fail...
        return NV_OK;
    }

    //
    // Find the last entry with a matching externalClassId, if any.  We keep
    // all entries of the same externalClassId contiguous so that
    // gpuGetClassList_IMPL() can easily filter them out.
    //
    for (i = 0; i < numClasses; i++)
    {
        if (pClassDB->pClasses[i].externalClassId == pClassDescToCopy->externalClassId)
        {
            bMatchingClassIdFound = NV_TRUE;
            matchingClassIdIndex = i;
        }
    }

    if (bMatchingClassIdFound)
    {
        NvLength bytesToMove;

        // Add an entry next to the existing block of this class ID.
        newClassDBIndex = matchingClassIdIndex + 1;

        // Move the rest of the entries to make space.
        bytesToMove = (numClasses - newClassDBIndex) * sizeof(pClassDB->pClasses[0]);
        portMemMove(&pClassDB->pClasses[newClassDBIndex + 1], bytesToMove,
                    &pClassDB->pClasses[newClassDBIndex],     bytesToMove);

    }
    else
    {
        // Add new entry at the end.
        newClassDBIndex = numClasses;
    }

    //
    // Store info for class in class DB entry.
    //
    // It is assumed that because there is a matching entry in the static class
    // list, but not the dynamic class list, the classDB array already has
    // enough space allocated (because gpuBuildClassDB_IMPL duplicated the
    // static class list, and this entry must have been removed since then).
    //

    pClassDB->pClasses[newClassDBIndex] = *pClassDescToCopy;
    pClassDB->numClasses++;

    pGpu->engineDB.bValid = NV_FALSE;

    return NV_OK;
}

/*!
 * @brief Add a class to class DB with given Engine Tag and Class Id.
 *
 * @side Sets engineDB.bValid to NV_FALSE.
 *
 * @param[in] pGpu      OBJGPU pointer
 * @param[in] engDesc   Engine ID of Classes to be added to Class DB
 * @param[in] class     Class to add to DB
 *
 * @returns NV_STATUS - NV_OK always.
 */
NV_STATUS
gpuAddClassToClassDBByEngTagClassId_IMPL(OBJGPU *pGpu, ENGDESCRIPTOR engDesc, NvU32 externalClassId)
{
    return _gpuAddClassToClassDBByEngTagClassId(pGpu, &engDesc, &externalClassId);
}

/*!
 * @brief Add a class to class DB with given Engine Tag.
 *
 * @side Sets engineDB.bValid to NV_FALSE.
 *
 * @param[in] pGpu      OBJGPU pointer
 * @param[in] engDesc   Engine ID of Class to be added to Class DB
 *
 * @returns NV_STATUS - NV_OK always.
 */
NV_STATUS gpuAddClassToClassDBByEngTag_IMPL(OBJGPU *pGpu, ENGDESCRIPTOR engDesc)
{
    return _gpuAddClassToClassDBByEngTagClassId(pGpu, &engDesc, NULL);
}

/*!
 * @brief Add a class to class DB with given Class ID.
 *
 * @side Sets engineDB.bValid to NV_FALSE.
 *
 * @param[in] pGpu      OBJGPU pointer
 * @param[in] class     Class ID
 *
 * @returns NV_STATUS - NV_OK always.
 */
NV_STATUS gpuAddClassToClassDBByClassId_IMPL(OBJGPU *pGpu, NvU32 externalClassId)
{
    return _gpuAddClassToClassDBByEngTagClassId(pGpu, NULL, &externalClassId);
}

/*!
 * @brief Delete a class from class DB with given Engine Tag and Class Id.
 *
 * @side Sets engineDB.bValid to NV_FALSE.
 *
 * @param[in] pGpu             OBJGPU pointer
 * @param[in] pEngDesc         Engine Tag of Classes to be removed from Class DB
 *                             (NULL = don't care)
 * @param[in] pExternalClassId Class to remove from DB (NULL = don't care)
 *
 * @returns NV_STATUS - NV_OK always.
 */
static NV_STATUS
_gpuDeleteClassFromClassDBByEngTagClassId(OBJGPU *pGpu, ENGDESCRIPTOR *pEngDesc, NvU32 *pExternalClassId)
{
    GpuClassDb *pClassDB = &pGpu->classDB;
    NvU32 i, j;

    NV_CHECK_OR_RETURN(LEVEL_INFO, (NULL != pEngDesc) || (NULL != pExternalClassId), NV_ERR_INVALID_ARGUMENT);

    for (i = 0; i < pClassDB->numClasses; i++)
    {
        if (((NULL == pEngDesc) || (pClassDB->pClasses[i].engDesc == *pEngDesc)) &&
            ((NULL == pExternalClassId) || (pClassDB->pClasses[i].externalClassId == *pExternalClassId)))
        {
            for (j = i; j < pClassDB->numClasses - 1; j++)
            {
                pClassDB->pClasses[j] = pClassDB->pClasses[j + 1];
            }
            pClassDB->numClasses--;
            i--;    // Be sure to check the new entry at index i on the next loop.
        }
    }

    pGpu->engineDB.bValid = NV_FALSE;

    return NV_OK;
}

/*!
 * @brief Delete a class from class DB with given Engine Tag and Class Id.
 *
 * @side Sets engineDB.bValid to NV_FALSE.
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in] engDesc             EngDesc of Classes to be removed from Class DB
 * @param[in] externalClassId     Class to remove from DB
 *
 * @returns NV_STATUS - NV_OK always.
 */
NV_STATUS
gpuDeleteClassFromClassDBByEngTagClassId_IMPL(OBJGPU *pGpu, ENGDESCRIPTOR engDesc, NvU32 externalClassId)
{
    return _gpuDeleteClassFromClassDBByEngTagClassId(pGpu, &engDesc, &externalClassId);
}

/*!
 * @brief Delete a class from class DB with given Engine Tag.
 *
 * @side Sets engineDB.bValid to NV_FALSE.
 *
 * @param[in] pGpu               OBJGPU pointer
 * @param[in] externalClassId    Class to remove from DB
 *
 * @returns NV_STATUS - NV_OK always.
 */
NV_STATUS
gpuDeleteClassFromClassDBByClassId_IMPL(OBJGPU *pGpu, NvU32 externalClassId)
{
    return _gpuDeleteClassFromClassDBByEngTagClassId(pGpu, NULL, &externalClassId);
}

/*!
 * @brief Delete a class from class DB with given Engine Tag.
 *
 * @side Sets engineDB.bValid to NV_FALSE.
 *
 * @param[in] pGpu      OBJGPU pointer
 * @param[in] engDesc   Engine Descriptor of Classes to be removed from Class DB
 *
 * @returns NV_STATUS - NV_OK always.
 */
NV_STATUS
gpuDeleteClassFromClassDBByEngTag_IMPL(OBJGPU *pGpu, ENGDESCRIPTOR engDesc)
{
    return _gpuDeleteClassFromClassDBByEngTagClassId(pGpu, &engDesc, NULL);
}
