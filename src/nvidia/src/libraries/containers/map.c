/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "containers/map.h"

CONT_VTABLE_DEFN(MapBase, mapIterRange_IMPL, NULL);

static void _mapRotateLeft(MapNode **pPRoot, MapNode *x);
static void _mapRotateRight(MapNode **pPRoot, MapNode *x);
static void _mapInsertFixup(MapNode **pRoot, MapNode *x);
static void _mapDeleteFixup(MapNode **pRoot, MapNode *parentOfX, MapNode *x);

/**
 * @brief Replace the old node with the new one.
 * @details Does nothing if old node is NULL. Does not
 *          update oldnode links
 */
static void _mapPutNodeInPosition(MapBase *pMap, MapNode *pTargetPosition,
                                  MapNode *pNewNode);

/**
 * @brief Take on target node's children connections.
 * @details Does nothing is any of the input is NULL.
 *          Does not update oldnode links
 */
static void _mapAdoptChildrenNodes(MapNode *pTargetNode, MapNode *pNewNode);

/**
 * @brief Basic insertion procedure
 * @details Shared by three versions of map insertion functions
 */
static NvBool _mapInsertBase(MapBase *pMap, NvU64 key, void *pValue);

void mapInit_IMPL
(
    NonIntrusiveMap     *pMap,
    PORT_MEM_ALLOCATOR  *pAllocator,
    NvU32               valueSize
)
{
    NV_ASSERT_OR_RETURN_VOID(NULL != pMap);
    NV_ASSERT_OR_RETURN_VOID(NULL != pAllocator);
    portMemSet(&(pMap->base), 0, sizeof(pMap->base));
    CONT_VTABLE_INIT(MapBase, &pMap->base);
    pMap->pAllocator = pAllocator;
    pMap->valueSize = valueSize;
    pMap->base.nodeOffset = (NvS32)(0 - sizeof(MapNode));
}

void mapInitIntrusive_IMPL
(
    IntrusiveMap    *pMap,
    NvS32           nodeOffset
)
{
    NV_ASSERT_OR_RETURN_VOID(NULL != pMap);
    portMemSet(&(pMap->base), 0, sizeof(pMap->base));
    CONT_VTABLE_INIT(MapBase, &pMap->base);
    pMap->base.nodeOffset = nodeOffset;
}

static void _mapDestroy(MapBase *pMap, PORT_MEM_ALLOCATOR *pAllocator)
{
    MapNode *pNode;

    NV_ASSERT_OR_RETURN_VOID(NULL != pMap);

    pNode = pMap->pRoot;
    while (NULL != pNode)
    {
        while (NULL != pNode->pLeft)
            pNode = pNode->pLeft;

        while (NULL != pNode->pRight)
            pNode = pNode->pRight;

        if ((NULL == pNode->pLeft) && (NULL == pNode->pRight))
        {
            MapNode *pTemp = pNode->pParent;

            // update parent node
            if (NULL != pTemp)
            {
                if (pTemp->pLeft == pNode)
                    pTemp->pLeft = NULL;
                else
                    pTemp->pRight = NULL;
            }

            // free the node
            pNode->pParent = NULL;
            NV_CHECKED_ONLY(pNode->pMap = NULL);
            if (NULL != pAllocator)
            {
                PORT_FREE(pAllocator, pNode);
            }

            pNode = pTemp;
        }
    }

    pMap->pRoot = NULL;
    pMap->count = 0;
    NV_CHECKED_ONLY(pMap->versionNumber++);
}

void mapDestroy_IMPL
(
    NonIntrusiveMap *pMap
)
{
    _mapDestroy(&pMap->base, pMap->pAllocator);
}

void mapDestroyIntrusive_IMPL
(
    MapBase *pMap
)
{
    _mapDestroy(pMap, NULL);
}

NvU32 mapCount_IMPL
(
    MapBase *pMap
)
{
    NV_ASSERT_OR_RETURN(pMap, 0);
    return pMap->count;
}

NvU64 mapKey_IMPL
(
    MapBase     *pMap,
    void        *pValue
)
{
    MapNode *pNode = mapValueToNode(pMap, pValue);
    NV_ASSERT_OR_RETURN(NULL != pNode, 0);
    NV_ASSERT_CHECKED(pNode->pMap == pMap);
    return pNode->key;
}

void *mapInsertNew_IMPL
(
    NonIntrusiveMap *pMap,
    NvU64            key
)
{
    void *pNode = NULL;
    void *pValue;

    NV_ASSERT_OR_RETURN(NULL != pMap, NULL);

    pNode = PORT_ALLOC(pMap->pAllocator, sizeof(MapNode) + pMap->valueSize);
    NV_ASSERT_OR_RETURN(NULL != pNode, NULL);

    portMemSet(pNode, 0, sizeof(MapNode) + pMap->valueSize);
    pValue = mapNodeToValue(&pMap->base, pNode);

    // check key duplication
    if (!_mapInsertBase(&(pMap->base), key, pValue))
    {
        PORT_FREE(pMap->pAllocator, pNode);
        return NULL;
    }

    return pValue;
}

void *mapInsertValue_IMPL
(
    NonIntrusiveMap *pMap,
    NvU64            key,
    const void      *pValue
)
{
    void *pCurrent;

    NV_ASSERT_OR_RETURN(NULL != pValue, NULL);

    pCurrent = mapInsertNew_IMPL(pMap, key);
    if (NULL == pCurrent)
        return NULL;

    return portMemCopy(pCurrent, pMap->valueSize, pValue, pMap->valueSize);
}

NvBool mapInsertExisting_IMPL
(
    IntrusiveMap    *pMap,
    NvU64           key,
    void            *pValue
)
{
    NV_ASSERT_OR_RETURN(NULL != pMap, NV_FALSE);
    NV_ASSERT_OR_RETURN(NULL != pValue, NV_FALSE);
    return _mapInsertBase(&(pMap->base), key, pValue);
}

void mapRemove_IMPL
(
    NonIntrusiveMap     *pMap,
    void                *pValue
)
{
    if (pValue == NULL)
        return;
    mapRemoveIntrusive_IMPL(&(pMap->base), pValue);
    PORT_FREE(pMap->pAllocator, mapValueToNode(&pMap->base, pValue));
}

void mapRemoveIntrusive_IMPL
(
    MapBase *pMap,
    void    *pValue
)
{
    MapNode *x; // child node of y, might be NULL
    MapNode *y; // successor for z
    MapNode *z; // node to remove
    MapNode *parentOfX;
    NvU32 yWasBlack;

    // do nothing is pValue is NULL
    if (pValue == NULL)
        return;

    // 1. find y, the successor for z
    z = mapValueToNode(pMap, pValue);
    NV_ASSERT_OR_RETURN_VOID(NULL != z);
    NV_ASSERT_CHECKED(z->pMap == pMap);

    if (z->pLeft == NULL || z->pRight == NULL)
    {
        // z has at least one empty successor, y = z
        y = z;
    }

    else
    {
        // y is z's least greater node
        y = z->pRight;

        while (y->pLeft != NULL)
            y = y->pLeft;
    }

    // 2. find x, y's children
    if (y->pLeft != NULL)
        x = y->pLeft;
    else
        x = y->pRight;

    // 3. put x into y's position
    _mapPutNodeInPosition(pMap, y, x);
    // 4. put y into z's position if not the same
    parentOfX = y->pParent;
    yWasBlack = !y->bIsRed;

    if (y != z)
    {
        _mapPutNodeInPosition(pMap, z, y);
        _mapAdoptChildrenNodes(z, y);
        y->bIsRed = z->bIsRed;

        if (parentOfX == z)
            parentOfX = y;
    }

    // 5. fixup, to rebalance the tree
    if (yWasBlack)
        _mapDeleteFixup(&(pMap->pRoot), parentOfX, x);

    // 6. update the count
    NV_CHECKED_ONLY(pMap->versionNumber++);
    NV_CHECKED_ONLY(z->pMap = NULL);
    pMap->count--;
    return;
}

void mapRemoveByKey_IMPL
(
    NonIntrusiveMap    *pMap,
    NvU64               key
)
{
    mapRemove_IMPL(pMap, mapFind_IMPL(&(pMap->base), key));
}

void mapRemoveByKeyIntrusive_IMPL
(
    MapBase        *pMap,
    NvU64           key
)
{
    mapRemoveIntrusive_IMPL(pMap, mapFind_IMPL(pMap, key));
}

void *mapFind_IMPL
(
    MapBase *pMap,
    NvU64 key
)
{
    MapNode *pCurrent;
    NV_ASSERT_OR_RETURN(NULL != pMap, NULL);
    pCurrent = pMap->pRoot;

    while (pCurrent != NULL)
    {
        if (key < pCurrent->key)
            pCurrent = pCurrent->pLeft;
        else if (key > pCurrent->key)
            pCurrent = pCurrent->pRight;
        else
            return mapNodeToValue(pMap, pCurrent);
    }

    return NULL;
}

void *mapFindGEQ_IMPL
(
    MapBase *pMap,
    NvU64   keyMin
)
{
    MapNode *pCurrent;
    MapNode *pResult;
    NV_ASSERT_OR_RETURN(NULL != pMap, NULL);
    pCurrent = pMap->pRoot;
    pResult = NULL;

    while (pCurrent != NULL)
    {
        if (pCurrent->key > keyMin)
        {
            pResult = pCurrent;
            pCurrent = pCurrent->pLeft;
        }

        else if (pCurrent->key == keyMin)
            return mapNodeToValue(pMap, pCurrent);
        else
            pCurrent = pCurrent->pRight;
    }

    if (pResult == NULL)
        return NULL;

    return mapNodeToValue(pMap, pResult);
}

void *mapFindLEQ_IMPL
(
    MapBase *pMap,
    NvU64 keyMax
)
{
    MapNode *pCurrent;
    MapNode *pResult;
    NV_ASSERT_OR_RETURN(NULL != pMap, NULL);
    pCurrent = pMap->pRoot;
    pResult = NULL;

    while (pCurrent != NULL)
    {
        if (pCurrent->key > keyMax)
            pCurrent = pCurrent->pLeft;
        else if (pCurrent->key == keyMax)
            return  mapNodeToValue(pMap, pCurrent);
        else
        {
            pResult = pCurrent;
            pCurrent = pCurrent->pRight;
        }
    }

    if (pResult == NULL)
        return NULL;

    return mapNodeToValue(pMap, pResult);
}

void *mapNext_IMPL
(
    MapBase     *pMap,
    void        *pValue
)
{
    MapNode *pCurrent;
    MapNode *pNode = mapValueToNode(pMap, pValue);

    NV_ASSERT_OR_RETURN(NULL != pNode, NULL);
    NV_ASSERT_CHECKED(pNode->pMap == pMap);

    if (NULL != (pCurrent = pNode->pRight))
    {
        while (pCurrent->pLeft != NULL)
            pCurrent = pCurrent->pLeft;

        return mapNodeToValue(pMap, pCurrent);
    }

    else
    {
        pCurrent = pNode->pParent;

        while (pCurrent != NULL && pNode == pCurrent->pRight)
        {
            if (pCurrent == pMap->pRoot)
                return NULL;

            pNode = pCurrent;
            pCurrent = pCurrent->pParent;
        }

        if (pCurrent == NULL)
            return NULL;

        return mapNodeToValue(pMap, pCurrent);
    }
}

void *mapPrev_IMPL
(
    MapBase *pMap,
    void    *pValue
)
{
    MapNode *pCurrent;
    MapNode *pNode = mapValueToNode(pMap, pValue);

    NV_ASSERT_OR_RETURN(NULL != pNode, NULL);
    NV_ASSERT_CHECKED(pNode->pMap == pMap);

    if (NULL != (pCurrent = pNode->pLeft))
    {
        while (pCurrent->pRight != NULL)
            pCurrent = pCurrent->pRight;

        return mapNodeToValue(pMap, pCurrent);
    }

    else
    {
        pCurrent = pNode->pParent;

        while (pCurrent != NULL && pNode == pCurrent->pLeft)
        {
            if (pCurrent == pMap->pRoot)
            {
                return NULL;
            }

            pNode = pCurrent;
            pCurrent = pCurrent->pParent;
        }

        if (pCurrent == NULL)
            return NULL;

        return mapNodeToValue(pMap, pCurrent);
    }
}

// @todo: do we need to change the definition of pFirst and pLast?
// currently they are mapNodes
MapIterBase mapIterRange_IMPL
(
    MapBase *pMap,
    void    *pFirst,
    void    *pLast
)
{
    MapIterBase it;
    MapNode *pFirstNode;
    MapNode *pLastNode;
    NV_ASSERT(pMap);

    portMemSet(&it, 0, sizeof(it));
    it.pMap = pMap;

    if (pMap->count == 0)
    {
        NV_CHECKED_ONLY(it.versionNumber = pMap->versionNumber);
        return it;
    }

    NV_ASSERT(pFirst);
    NV_ASSERT(pLast);
    NV_ASSERT_CHECKED((mapValueToNode(pMap, pFirst))->pMap == pMap);
    NV_ASSERT_CHECKED((mapValueToNode(pMap, pLast))->pMap == pMap);
    NV_ASSERT(mapKey_IMPL(pMap, pLast) >= mapKey_IMPL(pMap, pFirst));
    pFirstNode = mapValueToNode(pMap, pFirst);
    pLastNode = mapValueToNode(pMap, pLast);
    it.pNode = pFirstNode;
    it.pLast = pLastNode;
    NV_CHECKED_ONLY(it.versionNumber = pMap->versionNumber);
    return it;
}

// @todo: not sure about ppvalue, change it from void * to void **
NvBool mapIterNext_IMPL(MapIterBase *pIt)
{
    NV_ASSERT_OR_RETURN(pIt, NV_FALSE);

#if PORT_IS_CHECKED_BUILD
    if (pIt->bValid && !CONT_ITER_IS_VALID(pIt->pMap, pIt))
    {
        NV_ASSERT(CONT_ITER_IS_VALID(pIt->pMap, pIt));
        PORT_DUMP_STACK();
        pIt->bValid = NV_FALSE;
    }
#endif

    if (!pIt->pNode)
        return NV_FALSE;

    pIt->pValue = mapNodeToValue(pIt->pMap, pIt->pNode);

    if (pIt->pNode == pIt->pLast)
        pIt->pNode = NULL;
    else
        pIt->pNode = mapValueToNode(pIt->pMap,
                                    mapNext_IMPL(pIt->pMap, pIt->pValue));

    return NV_TRUE;
}

static void _mapRotateLeft
(
    MapNode **pPRoot,
    MapNode *x
)
{
    // rotate node x to left
    MapNode *y = x->pRight;
    // establish x->pRight link
    x->pRight = y->pLeft;

    if (y->pLeft)
        y->pLeft->pParent = x;

    // establish y->pParent link
    y->pParent = x->pParent;

    if (x->pParent)
    {
        if (x == x->pParent->pLeft)
            x->pParent->pLeft = y;
        else
            x->pParent->pRight = y;
    }

    else
        (*pPRoot) = y;

    // link x and y
    y->pLeft = x;
    x->pParent = y;
}

static void _mapRotateRight
(
    MapNode **pPRoot,
    MapNode *x
)
{
    // rotate node x to right
    MapNode *y = x->pLeft;
    // establish x->pLeft link
    x->pLeft = y->pRight;

    if (y->pRight)
        y->pRight->pParent = x;

    // establish y->pParent link
    y->pParent = x->pParent;

    if (x->pParent)
    {
        if (x == x->pParent->pRight)
            x->pParent->pRight = y;
        else
            x->pParent->pLeft = y;
    }

    else
        (*pPRoot) = y;

    // link x and y
    y->pRight = x;
    x->pParent = y;
}

static void _mapInsertFixup
(
    MapNode **pPRoot,
    MapNode *x
)
{
    // check red-black properties
    while ((x != *pPRoot) && x->pParent->bIsRed)
    {
        // we have a violation
        if (x->pParent == x->pParent->pParent->pLeft)
        {
            MapNode *y = x->pParent->pParent->pRight;

            if (y && y->bIsRed)
            {
                // uncle is RED
                x->pParent->bIsRed = NV_FALSE;
                y->bIsRed = NV_FALSE;
                x->pParent->pParent->bIsRed = NV_TRUE;
                x = x->pParent->pParent;
            }

            else
            {
                // uncle is BLACK
                if (x == x->pParent->pRight)
                {
                    // make x a left child
                    x = x->pParent;
                    _mapRotateLeft(pPRoot, x);
                }

                // recolor and rotate
                x->pParent->bIsRed = NV_FALSE;
                x->pParent->pParent->bIsRed = NV_TRUE;
                _mapRotateRight(pPRoot, x->pParent->pParent);
            }
        }

        else
        {
            // mirror image of above code
            MapNode *y = x->pParent->pParent->pLeft;

            if (y && y->bIsRed)
            {
                // uncle is RED
                x->pParent->bIsRed = NV_FALSE;
                y->bIsRed = NV_FALSE;
                x->pParent->pParent->bIsRed = NV_TRUE;
                x = x->pParent->pParent;
            }

            else
            {
                // uncle is BLACK
                if (x == x->pParent->pLeft)
                {
                    x = x->pParent;
                    _mapRotateRight(pPRoot, x);
                }

                x->pParent->bIsRed = NV_FALSE;
                x->pParent->pParent->bIsRed = NV_TRUE;
                _mapRotateLeft(pPRoot, x->pParent->pParent);
            }
        }
    }

    (*pPRoot)->bIsRed = NV_FALSE;
}

static void _mapDeleteFixup
(
    MapNode **pPRoot,
    MapNode *parentOfX,
    MapNode *x
)
{
    while ((x != *pPRoot) && (!x || !x->bIsRed))
    {
        //NV_ASSERT (!(x == NULL && parentOfX == NULL));
        // NULL nodes are sentinel nodes.  If we delete a sentinel node (x==NULL) it
        // must have a parent node (or be the root). Hence, parentOfX == NULL with
        // x==NULL is never possible (tree invariant)
        if ((parentOfX != NULL) && (x == parentOfX->pLeft))
        {
            MapNode *w = parentOfX->pRight;

            if (w && w->bIsRed)
            {
                w->bIsRed = NV_FALSE;
                parentOfX->bIsRed = NV_TRUE;
                _mapRotateLeft(pPRoot, parentOfX);
                w = parentOfX->pRight;
            }

            if (!w || (((!w->pLeft || !w->pLeft->bIsRed)
                        && (!w->pRight || !w->pRight->bIsRed))))
            {
                if (w)
                    w->bIsRed = NV_TRUE;

                x = parentOfX;
            }

            else
            {
                if (!w->pRight || !w->pRight->bIsRed)
                {
                    w->pLeft->bIsRed = NV_FALSE;
                    w->bIsRed = NV_TRUE;
                    _mapRotateRight(pPRoot, w);
                    w = parentOfX->pRight;
                }

                w->bIsRed = parentOfX->bIsRed;
                parentOfX->bIsRed = NV_FALSE;
                w->pRight->bIsRed = NV_FALSE;
                _mapRotateLeft(pPRoot, parentOfX);
                x = *pPRoot;
            }
        }

        else if (parentOfX != NULL)
        {
            MapNode *w = parentOfX->pLeft;

            if (w && w->bIsRed)
            {
                w->bIsRed = NV_FALSE;
                parentOfX->bIsRed = NV_TRUE;
                _mapRotateRight(pPRoot, parentOfX);
                w = parentOfX->pLeft;
            }

            if (!w || ((!w->pRight || !w->pRight->bIsRed) &&
                       (!w->pLeft || !w->pLeft->bIsRed)))
            {
                if (w)
                    w->bIsRed = NV_TRUE;

                x = parentOfX;
            }

            else
            {
                if (!w->pLeft || !w->pLeft->bIsRed)
                {
                    w->pRight->bIsRed = NV_FALSE;
                    w->bIsRed = NV_TRUE;
                    _mapRotateLeft(pPRoot, w);
                    w = parentOfX->pLeft;
                }

                w->bIsRed = parentOfX->bIsRed;
                parentOfX->bIsRed = NV_FALSE;
                w->pLeft->bIsRed = NV_FALSE;
                _mapRotateRight(pPRoot, parentOfX);
                x = *pPRoot;
            }
        }

        else if (x == NULL)
        {
            // This should never happen.
            break;
        }

        parentOfX = x->pParent;
    }

    if (x)
        x->bIsRed = NV_FALSE;
}

static void _mapPutNodeInPosition
(
    MapBase *pMap,
    MapNode *pTargetPosition,
    MapNode *pNewNode
)
{
    // error check - can be removed
    if (pTargetPosition == NULL)
        return;

    // 1. change connection from new node side
    if (pNewNode != NULL)
        pNewNode->pParent = pTargetPosition->pParent;

    // 2. connection from parent side
    if (pTargetPosition->pParent != NULL)
    {
        if (pTargetPosition == pTargetPosition->pParent->pLeft)
            pTargetPosition->pParent->pLeft = pNewNode;
        else
            pTargetPosition->pParent->pRight = pNewNode;
    }

    else
        pMap->pRoot = pNewNode;
}

static void _mapAdoptChildrenNodes
(
    MapNode *pTargetNode,
    MapNode *pNewNode
)
{
    // error check - can be removed
    if (pTargetNode == NULL || pNewNode == NULL)
        return;

    // take on connections
    pNewNode->pLeft = pTargetNode->pLeft;

    if (pTargetNode->pLeft != NULL)
        pTargetNode->pLeft->pParent = pNewNode;

    pNewNode->pRight = pTargetNode->pRight;

    if (pTargetNode->pRight != NULL)
        pTargetNode->pRight->pParent = pNewNode;
}

static NvBool _mapInsertBase
(
    MapBase *pMap,
    NvU64    key,
    void    *pValue
)
{
    MapNode *pCurrent;
    MapNode *pParent;
    MapNode *pNode;
    pNode = mapValueToNode(pMap, pValue);
    // 1. locate parent leaf node for the new node
    pCurrent = pMap->pRoot;
    pParent = NULL;

    while (pCurrent != NULL)
    {
        pParent = pCurrent;

        if (key < pCurrent->key)
            pCurrent =  pCurrent->pLeft;
        else if (key > pCurrent->key)
            pCurrent = pCurrent->pRight;
        else
        {
            // duplication detected
            return NV_FALSE;
        }
    }

    // 2. set up the new node structure
    NV_CHECKED_ONLY(pNode->pMap = pMap);
    pNode->key      = key;
    pNode->pParent  = pParent;
    pNode->pLeft    = NULL;
    pNode->pRight   = NULL;
    pNode->bIsRed   = NV_TRUE;

    // 3. insert node in tree
    if (pParent != NULL)
    {
        if (pNode->key < pParent->key)
            pParent->pLeft = pNode;
        else
            pParent->pRight = pNode;
    }

    else
        pMap->pRoot = pNode;

    // 4. balance the tree
    _mapInsertFixup(&(pMap->pRoot), pNode);
    NV_CHECKED_ONLY(pMap->versionNumber++);
    pMap->count++;
    return NV_TRUE;
}

NvBool mapIsValid_IMPL(void *pMap)
{
#if NV_TYPEOF_SUPPORTED
    return NV_TRUE;
#else
    if (CONT_VTABLE_VALID((MapBase*)pMap))
        return NV_TRUE;

    NV_ASSERT_FAILED("vtable not valid!");
    CONT_VTABLE_INIT(MapBase, (MapBase*)pMap);
    return NV_FALSE;
#endif
}
