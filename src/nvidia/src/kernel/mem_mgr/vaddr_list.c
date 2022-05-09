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

 /**
 * @brief Init map tracker object
 *
 * @param[in] pVaList   tracker object pointer
 *
 *  @return NV_TRUE if tracker object is initialized successfully, NV_FALSE if not
 */
NV_STATUS vaListInit(VA_LIST *pVaList)
{
    VADDR_LIST_INFO *pVaListInfo = NULL;
    VA_INFO         *pVaInfo     = NULL;

    NV_ASSERT_OR_RETURN(pVaList, NV_ERR_INVALID_ARGUMENT);

    mapInit(pVaList,  portMemAllocatorGetGlobalNonPaged());

    pVaListInfo = (VADDR_LIST_INFO *)portMemAllocNonPaged(sizeof(VADDR_LIST_INFO));
    if (pVaListInfo == NULL)
    {
        mapDestroy(pVaList);
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }
    portMemSet(pVaListInfo, 0, sizeof(VADDR_LIST_INFO));
    pVaListInfo->bRelease = NV_TRUE;

    pVaInfo = mapInsertNew(pVaList, 0);
    if (pVaInfo == NULL)
    {
        mapDestroy(pVaList);
        portMemFree(pVaListInfo);
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    pVaInfo->pVaListInfo = pVaListInfo;

    return NV_OK;
}

/**
 * @brief Destroy map tracker object
 *
 * @param[in] pVaList   tracker object pointer
 *
 *  @return NV_TRUE if tracker object is initialized successfully, NV_FALSE if not
 */
void vaListDestroy(VA_LIST *pVaList)
{
    VA_INFO *pVaInfo = NULL;

    NV_ASSERT_OR_RETURN_VOID(pVaList);

    pVaInfo = mapFind(pVaList, 0);
    if (pVaInfo)
    {
        NV_ASSERT(pVaInfo->vAddr  == 0);
        NV_ASSERT(pVaInfo->refCnt == 0);
        NV_ASSERT(pVaInfo->pVaListInfo);

        portMemFree(pVaInfo->pVaListInfo);
        pVaInfo->pVaListInfo = NULL;

        mapRemove(pVaList, pVaInfo);
    }
    else
    {
        DBG_BREAKPOINT();
    }

    if (mapCount(pVaList) != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "non-zero mapCount(pVaList): 0x%x\n",
                  mapCount(pVaList));

        DBG_BREAKPOINT();
    }

    mapDestroy(pVaList);
}

NV_STATUS vaListSetManaged(VA_LIST *pVaList, NvBool bManaged)
{
    VA_INFO *pVaInfo = NULL;

    NV_ASSERT_OR_RETURN(pVaList, NV_ERR_INVALID_ARGUMENT);

    // We can change the prop only when no active maps
    pVaInfo = mapFind(pVaList, 0);
    NV_ASSERT_OR_RETURN(pVaInfo, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN((mapCount(pVaList) == 1) ||
        (pVaInfo->pVaListInfo->bRelease == !!bManaged),
        NV_ERR_INVALID_STATE);

    pVaInfo->pVaListInfo->bRelease = bManaged;

    return NV_OK;
}

NvBool vaListGetManaged(VA_LIST *pVaList)
{
    VA_INFO *pVaInfo = NULL;

    NV_ASSERT_OR_RETURN(pVaList, NV_FALSE);

    pVaInfo = mapFind(pVaList, 0);
    NV_ASSERT_OR_RETURN(pVaInfo, NV_FALSE);
    return pVaInfo->pVaListInfo->bRelease;
}

NV_STATUS vaListAddVa(VA_LIST *pVaList, OBJVASPACE *pVAS, NvU64 vAddr)
{
    NV_STATUS status  = NV_OK;
    VA_INFO  *pVaInfo = NULL;

    NV_ASSERT_OR_RETURN(pVaList, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pVAS, NV_ERR_INVALID_ARGUMENT);

    pVaInfo = mapFind(pVaList, (NvU64)NV_PTR_TO_NvP64(pVAS));
    if (pVaInfo)
    {
        NV_ASSERT_OR_RETURN(pVaInfo->refCnt, NV_ERR_INVALID_STATE);

        if (pVaInfo->vAddr == vAddr)
        {
            pVaInfo->refCnt++;
        }
        else
        {
            DBG_BREAKPOINT();
            status = NV_ERR_INVALID_STATE;
        }
    }
    else
    {
        pVaInfo = mapInsertNew(pVaList,  (NvU64)NV_PTR_TO_NvP64(pVAS));
        if (pVaInfo)
        {
            pVaInfo->vAddr  = vAddr;
            pVaInfo->refCnt = 1;
        }
        else
        {
            DBG_BREAKPOINT();
            status = NV_ERR_INSUFFICIENT_RESOURCES;
        }
    }

    return status;
}

NV_STATUS vaListRemoveVa(VA_LIST *pVaList, OBJVASPACE *pVAS)
{
    NV_STATUS status  = NV_OK;
    VA_INFO  *pVaInfo = NULL;

    NV_ASSERT_OR_RETURN(pVaList, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pVAS, NV_ERR_INVALID_ARGUMENT);

    pVaInfo = mapFind(pVaList, (NvU64)NV_PTR_TO_NvP64(pVAS));
    if (pVaInfo)
    {
        NV_ASSERT_OR_RETURN(pVaInfo->refCnt, NV_ERR_INVALID_STATE);

        pVaInfo->refCnt--;

        if (pVaInfo->refCnt == 0)
        {
            mapRemove(pVaList, pVaInfo);
        }
    }
    else
    {
        status = NV_ERR_OBJECT_NOT_FOUND;
    }

    return status;
}

NV_STATUS vaListFindVa(VA_LIST *pVaList, OBJVASPACE *pVAS, NvU64 *pVaddr)
{
    NV_STATUS status  = NV_OK;
    VA_INFO  *pVaInfo = NULL;

    NV_ASSERT_OR_RETURN(pVaList, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pVAS, NV_ERR_INVALID_ARGUMENT);

    pVaInfo = mapFind(pVaList, (NvU64)NV_PTR_TO_NvP64(pVAS));
    if (pVaInfo)
    {
        NV_ASSERT_OR_RETURN(pVaInfo->refCnt, NV_ERR_INVALID_STATE);
        *pVaddr = pVaInfo->vAddr;
    }
    else
    {
        status = NV_ERR_OBJECT_NOT_FOUND;
    }

    return status;
}

NV_STATUS vaListGetRefCount(VA_LIST *pVaList, OBJVASPACE *pVAS, NvU64 *refCount)
{
    NV_STATUS status = NV_OK;
    VA_INFO  *pVaInfo;

    NV_ASSERT_OR_RETURN(pVaList != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pVAS != NULL, NV_ERR_INVALID_ARGUMENT);

    pVaInfo = mapFind(pVaList, (NvU64)NV_PTR_TO_NvP64(pVAS));
    if (pVaInfo != NULL)
    {
        NV_ASSERT_OR_RETURN(pVaInfo->refCnt, NV_ERR_INVALID_STATE);
        *refCount = pVaInfo->refCnt;
    }
    else
    {
        status = NV_ERR_OBJECT_NOT_FOUND;
    }

    return status;
}
