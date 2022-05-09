/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @file
 * @brief   Implementation for info block utility
 */

#include "os/os.h"
#include "core/info_block.h"

//
// getInfoPtr
//
// Return pointer to HAL implementation-specific private data info block.
//
void *
getInfoPtr(PENG_INFO_LINK_NODE head, NvU32 dataId)
{
    PENG_INFO_LINK_NODE curNode = head;

    while (curNode && (curNode->dataId != dataId))
        curNode = curNode->next;

    if (curNode == NULL)
        return NULL;

    return curNode->infoBlock;
}

//
// testInfoPtr
//
// Returns info weather HAL implementation-specific private data info block is allocated.
//
NvBool
testInfoPtr(PENG_INFO_LINK_NODE head, NvU32 dataId)
{
    PENG_INFO_LINK_NODE curNode = head;

    while (curNode && (curNode->dataId != dataId))
        curNode = curNode->next;

    if (curNode == NULL)
        return NV_FALSE;

    return NV_TRUE;
}

//
// createLinkNode
//
// Allocate and initialize new info block.
//
static PENG_INFO_LINK_NODE
createLinkNode(NvU32 dataId, NvU32 size)
{
    PENG_INFO_LINK_NODE newNode;
    NV_STATUS rmStatus;

    newNode = portMemAllocNonPaged(sizeof(ENG_INFO_LINK_NODE));
    if (newNode == NULL)
    {
        rmStatus = NV_ERR_NO_MEMORY;
        NV_ASSERT(rmStatus == NV_OK);
        return NULL;
    }

    portMemSet(newNode, 0, sizeof(ENG_INFO_LINK_NODE));

    newNode->infoBlock = portMemAllocNonPaged(size);
    if (newNode->infoBlock == NULL)
    {
        rmStatus = NV_ERR_NO_MEMORY;
        portMemFree(newNode);
        NV_ASSERT(rmStatus == NV_OK);
        return NULL;
    }

    portMemSet(newNode->infoBlock, 0, size);

    newNode->dataId = dataId;

    return newNode;
}

//
// addInfoPtr
//
// Create new HAL privata data block and add it to specified list.
//
void *
addInfoPtr(PENG_INFO_LINK_NODE *head, NvU32 dataId, NvU32 size)
{
    PENG_INFO_LINK_NODE curNode = *head;
    PENG_INFO_LINK_NODE newNode = createLinkNode(dataId, size);

    if (newNode == NULL)
        return NULL;

    while (curNode && curNode->next)
        curNode = curNode->next;

    if (!curNode)
       *head = newNode;
    else
        curNode->next = newNode;

    return newNode->infoBlock;
}

//
// deleteInfoPtr
//
// Destroy HAL privata data block and remove it from specified list.
//
void
deleteInfoPtr(PENG_INFO_LINK_NODE *head, NvU32 dataId)
{
    PENG_INFO_LINK_NODE curNode = *head;

    if (!curNode)
        return ;

    // check list head
    if (curNode->dataId == dataId)
    {
        *head = curNode->next;
        NV_ASSERT(curNode->infoBlock);
        portMemFree(curNode->infoBlock);
        portMemFree(curNode);
        return ;
    }

    // search for it
    while (curNode->next && (curNode->next->dataId != dataId))
        curNode = curNode->next;

    if (curNode->next)
    {
        PENG_INFO_LINK_NODE delNode;

        delNode = curNode->next;
        curNode->next = curNode->next->next;
        NV_ASSERT(delNode->infoBlock);
        portMemFree(delNode->infoBlock);
        portMemFree(delNode);
    }

    return ;
}
