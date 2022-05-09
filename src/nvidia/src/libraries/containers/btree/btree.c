/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


/***************************** Balanced Tree *******************************\
*                                                                           *
*   A generic library to red black tree -- every operation is O(log(n))     *
*   check http://en.wikipedia.org/wiki/Red-black_tree or similar www pages  *
*                                                                           *
\***************************************************************************/

#include "utils/nvprintf.h"
#include "utils/nvassert.h"
#include "nvport/nvport.h"
#include "containers/btree.h"

//
// Debugging support.
//
#if PORT_IS_CHECKED_BUILD

//
// Dump current tree to debug port.
//
static NV_STATUS
_btreeDumpBranch
(
    NODE *pNode,
    NvU32 level
)
{
    NvU32 i;
    if (pNode)
    {
        _btreeDumpBranch(pNode->left, level+1);

        NV_PRINTF(LEVEL_INFO, "NVRM_BTREE: ");
        for (i=0; i<level; i++)
        {
            NV_PRINTF(LEVEL_INFO, ".");
        }
        NV_PRINTF(LEVEL_INFO, "Node         = 0x%p\n", pNode);
        NV_PRINTF(LEVEL_INFO, "keyStart     = 0x%llx\n", pNode->keyStart);
        NV_PRINTF(LEVEL_INFO, "keyEnd       = 0x%llx\n", pNode->keyEnd);
        NV_PRINTF(LEVEL_INFO, "isRed        = 0x%d\n", pNode->isRed ? 1 : 0);
        NV_PRINTF(LEVEL_INFO, "parent       = 0x%p\n", pNode->parent);
        NV_PRINTF(LEVEL_INFO, "left         = 0x%p\n", pNode->left);
        NV_PRINTF(LEVEL_INFO, "right        = 0x%p\n", pNode->right);

        _btreeDumpBranch(pNode->right, level+1);
    }
    return (NV_OK);
}

static NV_STATUS
_btreeDumpTree
(
    NODE *pRoot
)
{
    NV_PRINTF(LEVEL_INFO, "NVRM_BTREE: ======================== Tree Dump ==========================\n\r");
    if (pRoot == NULL)
    {
        NV_PRINTF(LEVEL_INFO, "NVRM_BTREE: NULL\n\r");
    }
    else
    {
        _btreeDumpBranch(pRoot, 0);
    }
    NV_PRINTF(LEVEL_INFO, "NVRM_BTREE: =============================================================\n\r");
    return (NV_OK);
}

//
// Validate node.
//
#define VALIDATE_NODE(pn)                           \
{                                                   \
    NV_ASSERT(_btreeNodeValidate(pn) == NV_OK);     \
}

#define VALIDATE_TREE(pt)                           \
{                                                   \
    NV_ASSERT(_btreeTreeValidate(pt) == NV_OK);     \
}

//
// Validate a nodes branch and count values.
//
static NV_STATUS
_btreeNodeValidate
(
    NODE *pNode
)
{
    NV_STATUS status;
    
    status = NV_OK;
    if (pNode == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "NVRM_BTREE: ERROR validating NULL NODE.\n\r");
        NV_ASSERT_FAILED("DBG_BREAKPOINT");
        return (NV_ERR_INVALID_PARAMETER);
    }
    if (pNode->left)
    {
        if (pNode->left->keyEnd >= pNode->keyStart)
        {
            NV_PRINTF(LEVEL_ERROR, "NVRM_BTREE: ERROR inconsistent left branch, keyStart = 0x%llx\n", pNode->keyStart);
            NV_PRINTF(LEVEL_ERROR, "                                       Left keyEnd = 0x%llx\n", pNode->left->keyEnd);
            NV_ASSERT_FAILED("DBG_BREAKPOINT");
            status = NV_ERR_INVALID_PARAMETER;
        }
        if (pNode->left->parent != pNode)
        {
            NV_PRINTF(LEVEL_ERROR, "NVRM_BTREE: ERROR inconsistent left branch, Node    = 0x%p\n", pNode);
            NV_PRINTF(LEVEL_ERROR, "                                       left->parent = 0x%p\n", pNode->left);
            NV_ASSERT_FAILED("DBG_BREAKPOINT");
            status = NV_ERR_INVALID_PARAMETER;
        }
    }
    if (pNode->right)
    {
        if (pNode->right->keyStart <= pNode->keyEnd)
        {
            NV_PRINTF(LEVEL_ERROR, "NVRM_BTREE: ERROR inconsistent right branch, keyEnd = 0x%llx\n", pNode->keyEnd);
            NV_PRINTF(LEVEL_ERROR, "                                       Right keyStart = 0x%llx\n", pNode->right->keyStart);
            NV_ASSERT_FAILED("DBG_BREAKPOINT");
            status = NV_ERR_INVALID_PARAMETER;
        }
        if (pNode->right->parent != pNode)
        {
            NV_PRINTF(LEVEL_ERROR, "NVRM_BTREE: ERROR inconsistent right branch, Node    = 0x%p\n", pNode);
            NV_PRINTF(LEVEL_ERROR, "                                       right->parent = 0x%p\n", pNode->right);
            NV_ASSERT_FAILED("DBG_BREAKPOINT");
            status = NV_ERR_INVALID_PARAMETER;
        }
    }

    // red black tree property: Every red node that is not a leaf has only black children. 
    if (pNode->isRed)
    {
        if (pNode->left && pNode->left->isRed)
        {
            NV_ASSERT_FAILED("DBG_BREAKPOINT");
            status = NV_ERR_INVALID_PARAMETER;
        }
        if (pNode->right && pNode->right->isRed)
        {
            NV_ASSERT_FAILED("DBG_BREAKPOINT");
            status = NV_ERR_INVALID_PARAMETER;
        }
    }

    return (status);
}


static NV_STATUS
_btreeBranchValidate
(
    NODE *pNode
)
{
    NV_STATUS status;   
    status = NV_OK;
    if (pNode)
    {
        if (pNode->left)
        {
            status |= _btreeBranchValidate(pNode->left);
        }
        status |= _btreeNodeValidate(pNode);
        if (pNode->right)
        {
            status |= _btreeBranchValidate(pNode->right);
        }
    }
    return (status);
}

static NV_STATUS
_btreeTreeValidate
(
    NODE *pRoot
)
{
    NV_STATUS status;
    
    status = NV_OK;
    if (pRoot)
    {
        NV_ASSERT(!pRoot->isRed);
        status = _btreeNodeValidate(pRoot);
        if (pRoot->left)
        {
            status |= _btreeBranchValidate(pRoot->left);
        }
        if (pRoot->right)
        {
            status |= _btreeBranchValidate(pRoot->right);
        }
    }
    if (status)
    {
        _btreeDumpTree(pRoot);
    }
    return (status);
}
   
#else
//
// Validate nothing.
//
#define VALIDATE_NODE(pn)
#define VALIDATE_TREE(pt)
#endif // PORT_IS_CHECKED_BUILD

// rbt helper function
static void _rotateLeft(NODE **pRoot, NODE *x)
{
    // rotate node x to left
    NODE *y = x->right;

    NV_ASSERT (x);
    NV_ASSERT (y);

    // establish x->right link
    x->right = y->left;
    if (y->left)
    {
        y->left->parent = x;
    }

    // establish y->parent link
    y->parent = x->parent;
    if (x->parent)
    {
        if (x == x->parent->left)
        {
            x->parent->left = y;
        }
        else
        {
            x->parent->right = y;
        }
    }
    else
    {
        *pRoot = y;
    }

    // link x and y
    y->left = x;
    x->parent = y;
    VALIDATE_NODE(x);
}

// rbt helper function
static void _rotateRight(NODE **pRoot, NODE *x)
{
    // rotate node x to right
    NODE *y = x->left;

    NV_ASSERT (x);
    NV_ASSERT (y);

    // establish x->left link
    x->left = y->right;
    if (y->right)
    {
        y->right->parent = x;
    }

    // establish y->parent link
    y->parent = x->parent;
    if (x->parent)
    {
        if (x == x->parent->right)
        {
            x->parent->right = y;
        }
        else
        {
            x->parent->left = y;
        }
    }
    else
    {
        *pRoot = y;
    }

    // link x and y
    y->right = x;
    x->parent = y;
    VALIDATE_NODE(x);
}

// rbt helper function:
//  - maintain red-black tree balance after inserting node x
static void _insertFixup(NODE **pRoot, NODE *x)
{
    // check red-black properties
    while((x!=*pRoot) && x->parent->isRed)
    {
        // we have a violation
        if (x->parent == x->parent->parent->left)
        {
            NODE *y = x->parent->parent->right;
            if (y && y->isRed)
            {
                // uncle is RED
                x->parent->isRed = NV_FALSE;
                y->isRed = NV_FALSE;
                x->parent->parent->isRed = NV_TRUE;
                x = x->parent->parent;
            }
            else
            {
                // uncle is BLACK
                if (x == x->parent->right)
                {
                    // make x a left child
                    x = x->parent;
                    _rotateLeft(pRoot, x);
                }

                // recolor and rotate
                x->parent->isRed = NV_FALSE;
                x->parent->parent->isRed = NV_TRUE;
                _rotateRight(pRoot, x->parent->parent);
            }
        }
        else
        {
            // mirror image of above code
            NODE *y = x->parent->parent->left;
            if (y && y->isRed)
            {
                // uncle is RED
                x->parent->isRed = NV_FALSE;
                y->isRed = NV_FALSE;
                x->parent->parent->isRed = NV_TRUE;
                x = x->parent->parent;
            }
            else
            {
                // uncle is BLACK
                if (x == x->parent->left)
                {
                    x = x->parent;
                    _rotateRight(pRoot, x);
                }
                x->parent->isRed = NV_FALSE;
                x->parent->parent->isRed = NV_TRUE;
                _rotateLeft(pRoot, x->parent->parent);
            }
        }
    }
    (*pRoot)->isRed = NV_FALSE;
}

// insert a new node (no duplicates allowed)
NV_STATUS
btreeInsert
(
    PNODE  newNode,
    PNODE *pRoot
)
{
    NODE *current;
    NODE *parent;

    if (newNode == NULL || pRoot == NULL)
    {
        return NV_ERR_INVALID_POINTER;
    }

    // find future parent
    current = *pRoot;
    parent = NULL;

    if (newNode->keyEnd < newNode->keyStart)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    while (current)
    {
        parent = current;
        if (newNode->keyEnd < current->keyStart)
        {
            current = current->left;
        }
        else if (newNode->keyStart > current->keyEnd)
        {
            current = current->right;
        }
        else
        {
            return NV_ERR_INSERT_DUPLICATE_NAME;
        }
    }

    // the caller allocated the node already, just fix the links
    newNode->parent = parent;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->isRed = NV_TRUE;

    // insert node in tree
    if(parent)
    {
        if (newNode->keyEnd < parent->keyStart)
        {
            parent->left = newNode;
        }
        else
        {
            parent->right = newNode;
        }
    }
    else
    {
        *pRoot = newNode;
    }

    _insertFixup(pRoot, newNode);
    VALIDATE_NODE(newNode);

    return NV_OK;
}

// rbt helper function
//  - maintain red-black tree balance after deleting node x
//  - this is a bit ugly because we use NULL as a sentinel
static void _deleteFixup(NODE **pRoot, NODE *parentOfX, NODE *x)
{
    while ((x != *pRoot) && (!x || !x->isRed))
    {
        NV_ASSERT (!(x == NULL && parentOfX == NULL));
        // NULL nodes are sentinel nodes.  If we delete a sentinel node (x==NULL) it
        // must have a parent node (or be the root). Hence, parentOfX == NULL with 
        // x==NULL is never possible (tree invariant)

        if ((parentOfX != NULL) && (x == parentOfX->left))
        {
            NODE *w = parentOfX->right;
            if (w && w->isRed)
            {
                w->isRed = NV_FALSE;
                parentOfX->isRed = NV_TRUE;
                _rotateLeft(pRoot, parentOfX);
                w = parentOfX->right;
            }
            if (!w || (((!w->left || !w->left->isRed) && (!w->right || !w->right->isRed))))
            {
                if (w)
                {
                    w->isRed = NV_TRUE;
                }
                x = parentOfX;
            }
            else
            {
                if (!w->right || !w->right->isRed)
                {
                    w->left->isRed = NV_FALSE;
                    w->isRed = NV_TRUE;
                    _rotateRight(pRoot, w);
                    w = parentOfX->right;
                }
                w->isRed = parentOfX->isRed;
                parentOfX->isRed = NV_FALSE;
                w->right->isRed = NV_FALSE;
                _rotateLeft(pRoot, parentOfX);
                x = *pRoot;
            }
        }
        else if (parentOfX != NULL)
        {
            NODE *w = parentOfX->left;
            if (w && w->isRed)
            {
                w->isRed = NV_FALSE;
                parentOfX->isRed = NV_TRUE;
                _rotateRight(pRoot, parentOfX);
                w = parentOfX->left;
            }
            if (!w || ((!w->right || !w->right->isRed) && (!w->left || !w->left->isRed)))
            {
                if (w)
                {
                    w->isRed = NV_TRUE;
                }
                x = parentOfX;
            }
            else
            {
                if (!w->left || !w->left->isRed)
                {
                    w->right->isRed = NV_FALSE;
                    w->isRed = NV_TRUE;
                    _rotateLeft(pRoot, w);
                    w = parentOfX->left;
                }
                w->isRed = parentOfX->isRed;
                parentOfX->isRed = NV_FALSE;
                w->left->isRed = NV_FALSE;
                _rotateRight(pRoot, parentOfX);
                x = *pRoot;
            }
        }
        else if (x == NULL)
        {
            // This should never happen.
            break;
        }
        parentOfX = x->parent;
    }
    if (x)
    {
        x->isRed = NV_FALSE;
    }
}

//
// Unlink node from tree
//
NV_STATUS
btreeUnlink
(
    PNODE pNode,
    PNODE *pRoot
)
{
    NODE *x;
    NODE *y;
    NODE *z;
    NODE *parentOfX;
    NvU32 yWasBlack;

    NV_ASSERT_CHECKED(btreeSearch(pNode->keyStart, &z, *pRoot) == NV_OK);
    NV_ASSERT_CHECKED(z == pNode);

    if (pNode == NULL || pRoot == NULL)
    {
        return NV_ERR_INVALID_POINTER;
    }

    z = pNode;

    // unlink
    if (!z->left || !z->right)
    {
        // y has a SENTINEL node as a child
        y = z;
    }
    else
    {
        // find tree successor
        y = z->right;
        while (y->left)
        {
            y = y->left;
        }
    }

    // x is y's only child
    if (y->left)
    {
        x = y->left;
    }
    else
    {
        x = y->right;
    }

    // remove y from the parent chain
    parentOfX = y->parent;
    if (x)
    {
        x->parent = parentOfX;
    }
    if (y->parent)
    {
        if (y == y->parent->left)
        {
            y->parent->left = x;
        }
        else
        {
            y->parent->right = x;
        }
    }
    else
    {
        *pRoot = x;
    }

    yWasBlack = !y->isRed;
    if (y != z)
    {
        // we need to replace z with y so the memory for z can be freed
        y->parent = z->parent;
        if (z->parent)
        {
            if (z == z->parent->left)
            {
                z->parent->left = y;
            }
            else
            {
                z->parent->right = y;
            }
        }
        else
        {
            *pRoot = y;
        }

        y->isRed = z->isRed;

        y->left = z->left;
        if (z->left)
        {
            z->left->parent = y;
        }
        y->right = z->right;
        if (z->right)
        {
            z->right->parent = y;
        }

        if (parentOfX == z)
        {
            parentOfX = y;
        }
    }

    if (yWasBlack)
    {
        _deleteFixup(pRoot, parentOfX, x);
        if (parentOfX)
        {
            VALIDATE_NODE(parentOfX);
        }
    }

    return NV_OK;
}

//
// Search for node in tree.
//
NV_STATUS
btreeSearch
(
    NvU64  keyOffset,
    PNODE *pNode,
    PNODE  root
)
{
    // uninitialized ?
    NODE *current = root;
    while(current)
    {
        VALIDATE_NODE(current);
        if (keyOffset < current->keyStart)
        {
            current = current->left;
        }
        else if (keyOffset > current->keyEnd)
        {
            current = current->right;
        }
        else
        {
            *pNode = current;
            return NV_OK;
        }
    }
    *pNode = NULL;
    return NV_ERR_OBJECT_NOT_FOUND;
}

//
// Enumerate tree (starting at the node with specified value)
//
NV_STATUS
btreeEnumStart
(
    NvU64  keyOffset,
    PNODE *pNode,
    PNODE  root
)
{
    *pNode = NULL;

    // initialized ?
    if (root)
    {
        NODE *current = root;
        VALIDATE_TREE(root);
        while(current)
        {
            if (keyOffset < current->keyStart)
            {
                *pNode = current;
                current = current->left;
            }
            else if (keyOffset > current->keyEnd)
            {
                current = current->right;
            }
            else
            {
                *pNode = current;
                break;

            }
        }
        if (*pNode)
        {
            VALIDATE_NODE(*pNode);
        }
        return NV_OK;
    }
    return NV_OK;
}

NV_STATUS
btreeEnumNext
(
    PNODE *pNode,
    PNODE  root
)
{   
    // no nodes ?
    NODE *current = NULL;
    VALIDATE_NODE(*pNode);
    VALIDATE_NODE(root);
    if (root && *pNode)
    {
        // if we don't have a right subtree return the parent
        current = *pNode;

        // pick the leftmost node of the right subtree ?
        if (current->right)
        {
            current = current->right;
            for(;current->left;)
            {
                current = current->left;
            }
        }
        else
        {
            // go up until we find the right inorder node
            for(current = current->parent; current; current = current->parent)
            {
                if (current->keyStart > (*pNode)->keyEnd)
                {
                    break;
                }
            }
        }
    }
    *pNode = current;
    if (*pNode)
    {
        VALIDATE_NODE(*pNode);
    }
    return NV_OK;
}



//
// Frees all the "Data" fields stored in Nodes.
// If each Node is embedded in the structure pointed by its "Data" field, then
// this function destroys the whole btree
//
NV_STATUS
btreeDestroyData
(
    PNODE   pNode
)
{
    if (pNode == NULL)
        return NV_OK;

    btreeDestroyData(pNode->left);
    btreeDestroyData(pNode->right);
    portMemFree (pNode->Data);

    return NV_OK;
}



//
// Frees all the nodes and data stored in them.
// Don't use if the nodes were allocated within other structs 
// (e.g. if the Node is embedded within the struct pointed by its "Data" field)
//
NV_STATUS
btreeDestroyNodes
(
    PNODE   pNode
)
{
    if (pNode == NULL)
        return NV_OK;

    btreeDestroyNodes(pNode->left);
    btreeDestroyNodes(pNode->right);
    portMemFree (pNode);

    return NV_OK;
}
