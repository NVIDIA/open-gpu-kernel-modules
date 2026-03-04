/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @file   vaddr_list.c
 * @brief  Virtual memory map tracking utility routines.
 */

#include "mem_mgr/vaddr_list.h"

/*!
 * @brief Permanently converts a VA_LIST_SIMPLE into a VA_LIST_DICT
 */
NV_STATUS
vaListInitMap(VA_LIST *pVaList)
{
    NV_STATUS status;
    VADDR_LIST_INFO *pVaListInfo;
    VA_INFO *pVaInfo;
    struct VA_LIST_INLINE vaCache[2];

    NV_ASSERT_OR_RETURN(pVaList != NULL, NV_ERR_INVALID_ARGUMENT);

    // Save the data in the simple list
    pVaListInfo = portMemAllocNonPaged(sizeof(*pVaListInfo));
    if (pVaListInfo == NULL)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }
    *pVaListInfo = pVaList->impl.simple.common;
    vaCache[0] = pVaList->impl.simple.entries[0];
    vaCache[1] = pVaList->impl.simple.entries[1];

    // Initialize the new map
    portMemSet(pVaList, 0, sizeof(*pVaList));
    mapInit(&pVaList->impl.map, portMemAllocatorGetGlobalNonPaged());

    // Insert the common info into the map
    pVaInfo = mapInsertNew(&pVaList->impl.map, 0);
    if (pVaInfo == NULL)
    {
        mapDestroy(&pVaList->impl.map);
        portMemFree(pVaListInfo);
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    pVaInfo->pVaListInfo = pVaListInfo;
    pVaList->type = VA_LIST_DICT;

    // Safe to use vaListDestroy from this point

    // Add the cached va into the map
    status = vaListAddVa(pVaList, vaCache[0].pVas, vaCache[0].vAddr);
    if (status != NV_OK)
    {
        vaListDestroy(pVaList);
        return status;
    }

    // Add the cached va into the map
    status = vaListAddVa(pVaList, vaCache[1].pVas, vaCache[1].vAddr);
    if (status != NV_OK)
    {
        vaListDestroy(pVaList);
        return status;
    }

    // Update the refcount from the cache
    pVaInfo = mapFind(&pVaList->impl.map, (NvU64)NV_PTR_TO_NvP64(vaCache[0].pVas));
    if (pVaInfo == NULL)
    {
        vaListDestroy(pVaList);
        return NV_ERR_INVALID_STATE;
    }
    pVaInfo->refCnt = vaCache[0].refCnt;

    // Update the refcount from the cache
    pVaInfo = mapFind(&pVaList->impl.map, (NvU64)NV_PTR_TO_NvP64(vaCache[1].pVas));
    if (pVaInfo == NULL)
    {
        vaListDestroy(pVaList);
        return NV_ERR_INVALID_STATE;
    }
    pVaInfo->refCnt = vaCache[1].refCnt;

    return NV_OK;
}

/*!
 * @brief Return number of unique addresses in this map
 */
NvU32
vaListMapCount(VA_LIST *pVaList)
{
    NvU32 count;

    NV_ASSERT_OR_RETURN(pVaList != NULL, 0);

    count = 0;
    switch (pVaList->type)
    {
        case VA_LIST_SIMPLE:
        {
            if (pVaList->impl.simple.entries[0].pVas != NULL)
                count++;

            if (pVaList->impl.simple.entries[1].pVas != NULL)
                count++;

            break;
        }
        case VA_LIST_DICT:
        {
            count += mapCount(&pVaList->impl.map);
            // Account for the extra node
            if (count > 0)
                count--;

            break;
        }
    }

    return count;
}

/*!
 * @brief Init map tracker object
 *
 * @param[in] pVaList   tracker object pointer
 */
NV_STATUS
vaListInit(VA_LIST *pVaList)
{
    NV_ASSERT_OR_RETURN(pVaList != NULL, NV_ERR_INVALID_ARGUMENT);

    portMemSet(pVaList, 0, sizeof(*pVaList));
    pVaList->type = VA_LIST_SIMPLE;
    pVaList->impl.simple.common.bRelease = NV_TRUE;
    return NV_OK;
}

/*!
 * @brief Remove all mappings from the list
 */
void
vaListClear(VA_LIST *pVaList)
{
    NvBool bManaged = vaListGetManaged(pVaList);

    NV_ASSERT_OR_RETURN_VOID(pVaList != NULL);

    switch (pVaList->type)
    {
        case VA_LIST_SIMPLE:
        {
            break;
        }
        case VA_LIST_DICT:
        {
            VA_INFO *pVaInfo = mapFind(&pVaList->impl.map, 0);

            // Delete the dummy state node
            if (pVaInfo != NULL)
                portMemFree(pVaInfo->pVaListInfo);

            // Remove all of the map nodes and delete the map
            mapDestroy(&pVaList->impl.map);

            break;
        }
    }

    portMemSet(pVaList, 0, sizeof(*pVaList));
    pVaList->type = VA_LIST_SIMPLE;
    pVaList->impl.simple.common.bRelease = bManaged;
}

/*!
 * @brief Destroy map tracker object
 */
void
vaListDestroy(VA_LIST *pVaList)
{
    NV_ASSERT_OR_RETURN_VOID(pVaList != NULL);

    //
    // Skip DBG_BREAKPOINT when we are in Physical RM.
    // DBG_BREAKPOINT is the result of the lack of eviction of the context buffers from client RM.
    //
    if (vaListMapCount(pVaList) != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "non-zero mapCount(pVaList): 0x%x\n",
                  vaListMapCount(pVaList));

        DBG_BREAKPOINT();
    }

    switch (pVaList->type)
    {
        case VA_LIST_SIMPLE:
        {
            portMemSet(pVaList, 0, sizeof(*pVaList));

            break;
        }
        case VA_LIST_DICT:
        {
            VA_INFO *pVaInfo = mapFind(&pVaList->impl.map, 0);
            if (pVaInfo != NULL)
            {
                NV_ASSERT(pVaInfo->vAddr  == 0);
                NV_ASSERT(pVaInfo->refCnt == 0);
                NV_ASSERT(pVaInfo->pVaListInfo);

                portMemFree(pVaInfo->pVaListInfo);
                pVaInfo->pVaListInfo = NULL;

                mapRemove(&pVaList->impl.map, pVaInfo);
            }
            else
            {
                DBG_BREAKPOINT();
            }

            mapDestroy(&pVaList->impl.map);

            break;
        }
    }
}

/*!
 * @brief Indicate that this list tracks addresses mapped by RM
 *        The mode may only be changed when the list is empty.
 *
 * @param pVaList
 * @param bManaged   NV_FALSE to indicate addresses not mapped by RM
 *
 * @return NV_ERR_INVALID_STATE if list is not empty and mode is changing
 */
NV_STATUS
vaListSetManaged(VA_LIST *pVaList, NvBool bManaged)
{
    NV_ASSERT_OR_RETURN(pVaList != NULL, NV_ERR_INVALID_ARGUMENT);

    // We can change the prop only when no active maps
    NV_ASSERT_OR_RETURN((vaListMapCount(pVaList) == 0) ||
                        (vaListGetManaged(pVaList) == bManaged),
                        NV_ERR_INVALID_STATE);

    switch (pVaList->type)
    {
        case VA_LIST_SIMPLE:
        {
            pVaList->impl.simple.common.bRelease = bManaged;

            break;
        }
        case VA_LIST_DICT:
        {
            VA_INFO *pVaInfo = mapFind(&pVaList->impl.map, 0);

            NV_ASSERT_OR_RETURN(pVaInfo != NULL, NV_ERR_INVALID_STATE);

            pVaInfo->pVaListInfo->bRelease = bManaged;

            break;
        }
    }

    return NV_OK;
}

/*!
 * @brief returns NV_TRUE if the addresses tracked by this list are mapped by RM
 */
NvBool
vaListGetManaged(VA_LIST *pVaList)
{
    NvBool bRelease = NV_TRUE;
    NV_ASSERT_OR_RETURN(pVaList != NULL, NV_FALSE);

    switch (pVaList->type)
    {
        case VA_LIST_SIMPLE:
        {
            bRelease = pVaList->impl.simple.common.bRelease;

            break;
        }
        case VA_LIST_DICT:
        {
            VA_INFO *pVaInfo = mapFind(&pVaList->impl.map, 0);

            NV_ASSERT_OR_RETURN(pVaInfo != NULL, NV_FALSE);

            bRelease = pVaInfo->pVaListInfo->bRelease;

            break;
        }
    }

    return bRelease;
}

/*!
 * @brief Add a new VAS / vAddr pair to the list
 *        If the pair already exists, increase its refcount
 *
 * @param pVaList
 * @param pVas         OBJVASPACE this vaddr belongs to
 * @param vAddr        Virtual address being tracked
 */
NV_STATUS
vaListAddVa(VA_LIST *pVaList, OBJVASPACE *pVAS, NvU64 vAddr)
{
    NV_STATUS status = NV_OK;
    NV_ASSERT_OR_RETURN(pVaList != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pVAS != NULL, NV_ERR_INVALID_ARGUMENT);

    switch (pVaList->type)
    {
        case VA_LIST_SIMPLE:
        {
            if (pVaList->impl.simple.entries[0].pVas == pVAS)
            {
                NV_ASSERT_OR_RETURN(pVaList->impl.simple.entries[0].refCnt > 0, NV_ERR_INVALID_STATE);
                NV_ASSERT_OR_RETURN(pVaList->impl.simple.entries[0].refCnt < NV_U64_MAX, NV_ERR_INVALID_STATE);
                NV_ASSERT_OR_RETURN(pVaList->impl.simple.entries[0].vAddr == vAddr, NV_ERR_INVALID_STATE);
                pVaList->impl.simple.entries[0].refCnt++;

                break;
            }
            else if (pVaList->impl.simple.entries[1].pVas == pVAS)
            {
                NV_ASSERT_OR_RETURN(pVaList->impl.simple.entries[1].refCnt > 0, NV_ERR_INVALID_STATE);
                NV_ASSERT_OR_RETURN(pVaList->impl.simple.entries[1].refCnt < NV_U64_MAX, NV_ERR_INVALID_STATE);
                NV_ASSERT_OR_RETURN(pVaList->impl.simple.entries[1].vAddr == vAddr, NV_ERR_INVALID_STATE);
                pVaList->impl.simple.entries[1].refCnt++;

                break;
            }

            if (vaListMapCount(pVaList) == 2)
            {
                // Convert the SIMPLE list into a MAP
                status = vaListInitMap(pVaList);
                if (status != NV_OK)
                    return status;

                return vaListAddVa(pVaList, pVAS, vAddr);
            }

            if (pVaList->impl.simple.entries[0].pVas == NULL)
            {
                pVaList->impl.simple.entries[0].pVas = pVAS;
                pVaList->impl.simple.entries[0].vAddr = vAddr;
                pVaList->impl.simple.entries[0].refCnt = 1;
            }
            else if (pVaList->impl.simple.entries[1].pVas == NULL)
            {
                pVaList->impl.simple.entries[1].pVas = pVAS;
                pVaList->impl.simple.entries[1].vAddr = vAddr;
                pVaList->impl.simple.entries[1].refCnt = 1;
            }
            else
            {
                // Should never be reached. Should've converted to map
                NV_ASSERT_FAILED("Simple va list full");
                return NV_ERR_INVALID_STATE;
            }

            break;
        }
        case VA_LIST_DICT:
        {
            VA_INFO *pVaInfo = mapFind(&pVaList->impl.map, (NvU64)NV_PTR_TO_NvP64(pVAS));

            if (pVaInfo != NULL)
            {
                NV_ASSERT_OR_RETURN(pVaInfo->refCnt > 0, NV_ERR_INVALID_STATE);
                NV_ASSERT_OR_RETURN(pVaInfo->refCnt < NV_U64_MAX, NV_ERR_INVALID_STATE);
                NV_ASSERT_OR_RETURN(pVaInfo->vAddr == vAddr, NV_ERR_INVALID_STATE);
                pVaInfo->refCnt++;
            }
            else
            {
                pVaInfo = mapInsertNew(&pVaList->impl.map, (NvU64)NV_PTR_TO_NvP64(pVAS));
                NV_ASSERT_OR_RETURN(pVaInfo != NULL, NV_ERR_INSUFFICIENT_RESOURCES);
                pVaInfo->vAddr  = vAddr;
                pVaInfo->refCnt = 1;
            }

            break;
        }
    }

    return status;
}

/*!
 * @brief Remove mapping associated with VAS from tracking
 *
 * @param pVaList
 * @param pVAS      OBJVASPACE to remove
 *
 * @return NV_ERR_OBJECT_NOT_FOUND if VAS not in list
 */
NV_STATUS
vaListRemoveVa(VA_LIST *pVaList, OBJVASPACE *pVAS)
{
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pVaList != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pVAS != NULL, NV_ERR_INVALID_ARGUMENT);

    switch (pVaList->type)
    {
        case VA_LIST_SIMPLE:
        {
            if (pVaList->impl.simple.entries[0].pVas == pVAS)
            {
                NV_ASSERT_OR_RETURN(pVaList->impl.simple.entries[0].refCnt > 0, NV_ERR_INVALID_STATE);

                pVaList->impl.simple.entries[0].refCnt--;
                if (pVaList->impl.simple.entries[0].refCnt == 0)
                {
                    pVaList->impl.simple.entries[0].pVas = NULL;
                    pVaList->impl.simple.entries[0].vAddr = 0;
                }
            }
            else if (pVaList->impl.simple.entries[1].pVas == pVAS)
            {
                NV_ASSERT_OR_RETURN(pVaList->impl.simple.entries[1].refCnt > 0, NV_ERR_INVALID_STATE);

                pVaList->impl.simple.entries[1].refCnt--;
                if (pVaList->impl.simple.entries[1].refCnt == 0)
                {
                    pVaList->impl.simple.entries[1].pVas = NULL;
                    pVaList->impl.simple.entries[1].vAddr = 0;
                }
            }
            else
            {
                status = NV_ERR_OBJECT_NOT_FOUND;
            }

            break;
        }
        case VA_LIST_DICT:
        {
            VA_INFO *pVaInfo = mapFind(&pVaList->impl.map, (NvU64)NV_PTR_TO_NvP64(pVAS));
            if (pVaInfo != NULL)
            {
                NV_ASSERT_OR_RETURN(pVaInfo->refCnt > 0, NV_ERR_INVALID_STATE);

                pVaInfo->refCnt--;

                if (pVaInfo->refCnt == 0)
                {
                    mapRemove(&pVaList->impl.map, pVaInfo);
                }
            }
            else
            {
                status = NV_ERR_OBJECT_NOT_FOUND;
            }

            break;
        }
    }

    return status;
}

/*!
 * @brief Find virtual address associated with given OBJVASPACE
 *
 * @param pVaList
 * @param pVAS      OBJVASPACE to remove
 * @param pVaddr    address associated with VAS
 *
 * @return NV_ERR_OBJECT_NOT_FOUND if VAS not in list
 */
NV_STATUS
vaListFindVa(VA_LIST *pVaList, OBJVASPACE *pVAS, NvU64 *pVaddr)
{
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pVaList != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pVAS != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pVaddr != NULL, NV_ERR_INVALID_ARGUMENT);

    switch (pVaList->type)
    {
        case VA_LIST_SIMPLE:
        {
            if (pVaList->impl.simple.entries[0].pVas == pVAS)
            {
                NV_ASSERT_OR_RETURN(pVaList->impl.simple.entries[0].refCnt > 0, NV_ERR_INVALID_STATE);
                *pVaddr = pVaList->impl.simple.entries[0].vAddr;
            }
            else if (pVaList->impl.simple.entries[1].pVas == pVAS)
            {
                NV_ASSERT_OR_RETURN(pVaList->impl.simple.entries[1].refCnt > 0, NV_ERR_INVALID_STATE);
                *pVaddr = pVaList->impl.simple.entries[1].vAddr;
            }
            else
            {
                status = NV_ERR_OBJECT_NOT_FOUND;
            }

            break;
        }
        case VA_LIST_DICT:
        {
            VA_INFO *pVaInfo = mapFind(&pVaList->impl.map, (NvU64)NV_PTR_TO_NvP64(pVAS));
            if (pVaInfo != NULL)
            {
                NV_ASSERT_OR_RETURN(pVaInfo->refCnt > 0, NV_ERR_INVALID_STATE);
                *pVaddr = pVaInfo->vAddr;
            }
            else
            {
                status = NV_ERR_OBJECT_NOT_FOUND;
            }

            break;
        }
    }

    return status;
}

/*!
 * @brief Get refcount of OBJVASPACE in list
 *
 * @param pVaList
 * @param pVAS      OBJVASPACE to check
 * @param pRefCount refcount associated with VAS
 *
 * @return NV_ERR_OBJECT_NOT_FOUND if VAS not in list
 */
NV_STATUS
vaListGetRefCount(VA_LIST *pVaList, OBJVASPACE *pVAS, NvU64 *pRefCount)
{
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pVaList != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pVAS != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pRefCount != NULL, NV_ERR_INVALID_ARGUMENT);

    switch (pVaList->type)
    {
        case VA_LIST_SIMPLE:
        {
            if (pVaList->impl.simple.entries[0].pVas == pVAS)
            {
                NV_ASSERT_OR_RETURN(pVaList->impl.simple.entries[0].refCnt > 0, NV_ERR_INVALID_STATE);
                *pRefCount = pVaList->impl.simple.entries[0].refCnt;
            }
            else if (pVaList->impl.simple.entries[1].pVas == pVAS)
            {
                NV_ASSERT_OR_RETURN(pVaList->impl.simple.entries[1].refCnt > 0, NV_ERR_INVALID_STATE);
                *pRefCount = pVaList->impl.simple.entries[1].refCnt;
            }
            else
            {
                status = NV_ERR_OBJECT_NOT_FOUND;
            }

            break;
        }
        case VA_LIST_DICT:
        {
            VA_INFO *pVaInfo = mapFind(&pVaList->impl.map, (NvU64)NV_PTR_TO_NvP64(pVAS));
            if (pVaInfo != NULL)
            {
                NV_ASSERT_OR_RETURN(pVaInfo->refCnt > 0, NV_ERR_INVALID_STATE);
                *pRefCount = pVaInfo->refCnt;
            }
            else
            {
                status = NV_ERR_OBJECT_NOT_FOUND;
            }

            break;
        }
    }

    return status;
}

/*!
 * @brief Set refcount of OBJVASPACE in list
 *
 * @param pVaList
 * @param pVAS      OBJVASPACE to check
 * @param refCount  refcount associated with VAS
 *
 * @return NV_ERR_OBJECT_NOT_FOUND if VAS not in list
 */
NV_STATUS
vaListSetRefCount(VA_LIST *pVaList, OBJVASPACE *pVAS, NvU64 refCount)
{
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pVaList != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pVAS != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(refCount != 0, NV_ERR_INVALID_ARGUMENT);

    switch (pVaList->type)
    {
        case VA_LIST_SIMPLE:
        {
            if (pVaList->impl.simple.entries[0].pVas == pVAS)
            {
                pVaList->impl.simple.entries[0].refCnt = refCount;
            }
            else if (pVaList->impl.simple.entries[1].pVas == pVAS)
            {
                pVaList->impl.simple.entries[1].refCnt = refCount;
            }
            else
            {
                status = NV_ERR_OBJECT_NOT_FOUND;
            }

            break;
        }
        case VA_LIST_DICT:
        {
            VA_INFO *pVaInfo = mapFind(&pVaList->impl.map, (NvU64)NV_PTR_TO_NvP64(pVAS));
            if (pVaInfo != NULL)
            {
                pVaInfo->refCnt = refCount;
            }
            else
            {
                status = NV_ERR_OBJECT_NOT_FOUND;
            }

            break;
        }
    }

    return status;
}

