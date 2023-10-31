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

/*!
 * @file addrtree.c
 */

#include "gpu/mem_mgr/phys_mem_allocator/addrtree.h"
#include "gpu/mem_mgr/phys_mem_allocator/phys_mem_allocator_util.h"
#include "utils/nvprintf.h"
#include "utils/nvassert.h"
#include "nvport/nvport.h"
#include "nvmisc.h"

// Returns if (x, x+y-1) contains (u, u+v-1) in one dimention
#define RANGE_CONTAINS(x, y, u, v) ((x <= u) && ((x + y - 1) >= (u + v - 1)))

static PMA_PAGESTATUS pmaAddrtreeReadLevel(void *pMap, NvU32 levelNum, NvU64 frameNum, NvBool bReadAttrib);
static void _addrtreeUpdateAncestors(PMA_ADDRTREE *pTree, ADDRTREE_NODE *pNode, PMA_PAGESTATUS newStateMask);
static void _addrtreeConvertLevelFrameToNodeIndex(PMA_ADDRTREE *pTree, NvU32 levelNum, NvU64 frameNum,
                                                  ADDRTREE_NODE **ppNode, NvU32 *pIndex);
PMA_PAGESTATUS _pmaAddrtreeReadLevelAndSkipUnavailable(void *pMap, NvU32 levelNum, NvU64 frameNum,
                                                     PMA_PAGESTATUS searchState, NvBool bAllowFree,
                                                     NvU64 *pNumFramesToSkip, NvBool bReverse);

static NvU64 alignUpToMod(NvU64 frame, NvU64 alignment, NvU64 mod)
{
    if ((frame & (alignment - 1)) <= mod)
        return NV_ALIGN_DOWN(frame, alignment) + mod;
    else
        return NV_ALIGN_UP(frame, alignment) + mod;
}

static NvU32
addrtreeGetTreeLevel(NvU64 pageSize)
{
    NvU32 level = 0;
    switch (pageSize)
    {
        case _PMA_64KB:  level = 5; break;
        // 128KB is supported in wrapper routines
        case _PMA_2MB:   level = 4; break;
        case _PMA_512MB: level = 2; break;
        default: break;
    }

    NV_ASSERT(level != 0);
    return level;
}

static void
pmaAddrtreePrintLevel(ADDRTREE_LEVEL *pLevel)
{
    NvU32 i;
    NvU32 mapIndex = MAP_IDX_ALLOC_PIN;
    ADDRTREE_NODE *pNode = NULL;

    for (i = 0; i < pLevel->nodeCount; i++)
    {
        pNode = &pLevel->pNodeList[i];
        NV_PRINTF(LEVEL_INFO, "S[%d]=0x%llx A[%d]=0x%llx\n", i,
                  pNode->seeChild[mapIndex], i, pNode->state[mapIndex]);

        // In case compiler complains when the above print is compiled out
        (void)pNode;
        (void)mapIndex;
    }
}

void pmaAddrtreePrintTree(void *pMap, const char* str)
{
    NvU32 i;
    PMA_ADDRTREE *pTree = (PMA_ADDRTREE *)pMap;
    NV_PRINTF(LEVEL_INFO, "%s ==== \n", str);

    for (i = 0; i < pTree->levelCount - 1; i++)
    {
        NV_PRINTF(LEVEL_INFO, "Level [%d]\n", i);
        pmaAddrtreePrintLevel(&pTree->levels[i]);
    }
    NV_PRINTF(LEVEL_INFO, "END printing Tree ==== \n");
}

static NvU64
_makeMaskUpToIndex(NvU32 index)
{
    NV_ASSERT(index <= 64);
    if (index == 64)
    {
        return NV_U64_MAX;
    }
    else
    {
        return (1ULL << index) - 1;
    }
}

void *
pmaAddrtreeInit
(
    NvU64      numFrames,
    NvU64      addrBase,
    PMA_STATS *pPmaStats,
    NvBool     bProtected
)
{
    //
    // Hardcoding this for now to get things started
    // TODO: use more flexible configs
    //
    NvU32 levelSizes[] = {_TREE_2TB, _TREE_32GB, _TREE_512MB, _TREE_128MB, _TREE_2MB, _TREE_64KB};
    NvU32 levelCount = sizeof(levelSizes) / sizeof(levelSizes[0]);
    //NV_PRINTF(LEVEL_INFO, "numFrames: 0x%llx, addrBase 0x%llx \n", numFrames, addrBase);

    NvU64 totalNodeCount, num2mbPages;
    NvU32 i;
    PMA_ADDRTREE *newTree;
    NvU64 numFramesToAllocate;

    // PMA already ensures this
    NV_ASSERT(NV_IS_ALIGNED(addrBase, PMA_GRANULARITY));

    newTree = (PMA_ADDRTREE*)portMemAllocNonPaged((NvLength)sizeof(struct pma_addrtree));
    if (newTree == NULL)
    {
        return NULL;
    }
    portMemSet(newTree, 0, (NvLength)sizeof(*newTree));
    newTree->levels = NULL;
    newTree->root = NULL;

    // Allocate the levels
    newTree->levels = (ADDRTREE_LEVEL *)portMemAllocNonPaged((NvLength)(levelCount * sizeof(ADDRTREE_LEVEL)));
    if (newTree->levels == NULL)
    {
        goto error;
    }
    portMemSet(newTree->levels, 0, (NvLength)(levelCount * sizeof(ADDRTREE_LEVEL)));

    newTree->levelCount = levelCount;
    newTree->totalFrames = numFrames;
    num2mbPages = numFrames / (_PMA_2MB >> PMA_PAGE_SHIFT);

    pPmaStats->numFreeFrames += newTree->totalFrames;
    pPmaStats->num2mbPages += num2mbPages;
    pPmaStats->numFree2mbPages += num2mbPages;

    if (bProtected)
    {
        pPmaStats->numFreeFramesProtected += newTree->totalFrames;
        pPmaStats->num2mbPagesProtected += num2mbPages;
        pPmaStats->numFree2mbPagesProtected += num2mbPages;
    }

    newTree->bProtected = bProtected;
    newTree->pPmaStats = pPmaStats;

    //
    // Now pad the beginning of addrtree, and round down to the largest
    // single page allocation supported so that its level will be aligned.
    // The biggest single page is 512 MB currently.
    //
    newTree->numPaddingFrames = (addrBase - NV_ALIGN_DOWN64(addrBase, _PMA_512MB)) >> PMA_PAGE_SHIFT;
    numFramesToAllocate = newTree->totalFrames + newTree->numPaddingFrames;

    // Allocate each level and calculate number of nodes needed
    totalNodeCount = 0;
    for (i = 0; i < levelCount; i++)
    {
        ADDRTREE_LEVEL *cur  = &newTree->levels[i];
        cur->nodeCount = numFramesToAllocate >> (levelSizes[i] - PMA_PAGE_SHIFT);
        cur->pageSizeShift = levelSizes[i];

        // To be a tree, there needs to be one root node
        if (i == 0 && cur->nodeCount != 0)
        {
            NV_PRINTF(LEVEL_ERROR, "Total memory is > 2TB. PMA Address Tree cannot account for this much.\n");
            goto error;
        }

        // The 64KB level is a placeholder level which doesn't need to be allocated
        if (i == levelCount - 1)
        {
            break;
        }

        // Always allocate at least 1 node for each level and account for non-full nodes
        if ((cur->nodeCount == 0) || (numFramesToAllocate % (1ULL << (levelSizes[i] - PMA_PAGE_SHIFT)) != 0))
            cur->nodeCount++;

        totalNodeCount += cur->nodeCount;

        //NV_PRINTF(LEVEL_INFO, "Level %d: nodeCount: %d, totalNodeCount: %d\n", i, cur->nodeCount, totalNodeCount);
    }

    // Allocate all the nodes needed in a linear array
    newTree->root = (ADDRTREE_NODE *)portMemAllocNonPaged((NvLength)(totalNodeCount * sizeof(ADDRTREE_NODE)));
    if (newTree->root == NULL)
    {
        goto error;
    }
    portMemSet(newTree->root, 0, (NvLength)(totalNodeCount * sizeof(ADDRTREE_NODE)));
    newTree->root->parent = NULL;

    NvU32 curIdx = 0;

    // Skip the last level because we don't really need to allocate 64K level
    for (i = 0; i < levelCount - 1; i++)
    {
        NvU64 nextLevelStart = curIdx + newTree->levels[i].nodeCount;
        //
        // The maxChildren a node can have is a factor of
        // the difference in page sizes between levels
        //
        NvU32 maxChildren = 1U<<(levelSizes[i] - levelSizes[i + 1]);
        newTree->levels[i].maxFramesPerNode = maxChildren;

        //NV_PRINTF(LEVEL_INFO, "Level %d: maxChildren per node: %d\n", i, maxChildren);

        NvU32 j;
        for (j = 0; j < newTree->levels[i].nodeCount; j++)
        {
            ADDRTREE_NODE *curNode = &newTree->root[curIdx];

            // Register first node in the level structure
            if (j == 0)
            {
                newTree->levels[i].pNodeList = curNode;
            }

            // Populate curNode
            curNode->level = i;
            curNode->frame = maxChildren * j;

            // All nodes before this node must have maxChildren
            NvU64 childrenNodeIdx = nextLevelStart + j * maxChildren;
            curNode->children = &newTree->root[childrenNodeIdx];

            // The last node may not have maxChildren, calculate how many it does have
            // OK to just cast because we know numChildren must be at most 64
            NvU32 lastNodeNumValidChildren = (NvU32)(newTree->levels[i+1].nodeCount - (j * maxChildren));
            NvU32 numValidChildren = 0;

            //
            // If this is not the last node in a level,
            // then it must have maxChildren.
            // Otherwise calculate how many it does have.
            //
            if (j != newTree->levels[i].nodeCount - 1)
            {
                curNode->numChildren = maxChildren;
                numValidChildren = maxChildren;
            }
            else
            {
                curNode->numChildren = maxChildren;
                numValidChildren = lastNodeNumValidChildren;

                //
                // Mark invalid children as allocated so that reads of partial nodes
                // do not return that they are available
                //
                NvU64 invalidChildrenMask = _makeMaskUpToIndex(maxChildren) & ~_makeMaskUpToIndex(lastNodeNumValidChildren);
                curNode->state[MAP_IDX_ALLOC_PIN] = invalidChildrenMask;
                _addrtreeUpdateAncestors(newTree, curNode, STATE_PIN);
            }

            // Populate curNode->children[*]->parent, except for the last level
            if (i != levelCount - 2)
            {
                NvU32 k;

                for (k = 0; k < numValidChildren; k++)
                {
                    ADDRTREE_NODE *curChild = &curNode->children[k];
                    curChild->parent = curNode;
                }
            }

            curIdx++;
        }
    }

    return (void *)newTree;

error:
    pmaAddrtreeDestroy(newTree);
    return NULL;
}

void pmaAddrtreeDestroy(void *pMap)
{
    NvU64 num2mbPages;
    PMA_ADDRTREE *pTree = (PMA_ADDRTREE *)pMap;
    NV_ASSERT_OR_RETURN_VOID(pTree != NULL);

    num2mbPages = pTree->totalFrames / (_PMA_2MB >> PMA_PAGE_SHIFT);
    pTree->pPmaStats->numFreeFrames -= pTree->totalFrames;
    pTree->pPmaStats->numFree2mbPages -= num2mbPages;

    if (pTree->bProtected)
    {
        pTree->pPmaStats->numFreeFramesProtected -= pTree->totalFrames;
        pTree->pPmaStats->numFree2mbPagesProtected -= num2mbPages;
    }

    portMemFree(pTree->root);
    portMemFree(pTree->levels);

    portMemFree(pTree);
}

static void
_addrtreeConvertFrame(PMA_ADDRTREE *pTree, NvU32 sourceLevel, NvU64 sourceFrame, NvU32 targetLevel, NvU64 *pTargetFrame)
{
    // Converting up the tree, frame number goes down
    if (sourceLevel > targetLevel)
    {
        *pTargetFrame = sourceFrame >> (pTree->levels[targetLevel+1].pageSizeShift - pTree->levels[sourceLevel+1].pageSizeShift);
    }
    else
    {
        *pTargetFrame = sourceFrame << (pTree->levels[sourceLevel+1].pageSizeShift - pTree->levels[targetLevel+1].pageSizeShift);
    }
}

//
// Given a node and index into its children array, whether this node is
// on the target level, the current accumulated status, and the current
// valid status mask, read the status of this node index and return
// - The remaining mask of children that will still need to be
//   read to determine the state at a lower level of the tree in
//   pStatusMask
// - The accumulated status combined with this node index's status
//
// To read the effective value of a node, callers must start with their
// desired status mask, call getNodeIndexStatus, and use the returned
// status mask to know what states still need to be read at the
// next level down in the tree.
//
// If statusMask is 0 upon return, reading children will give no more information
// That is, shouldCheckChildren == (*pStatusMask != 0)
//
// If the caller wants to keep reading status downwards in the tree, it must
// call getNodeIndexStatus with bIsTargetLevel = NV_FALSE. Here is pseudocode
// for reading a node:
//   *pCumulativeStatus = 0;
//   *pStatusMask = MAP_MASK;
//   while (!targetLevel) {
//       getNodeIndexStatus(node, index, NV_FALSE, pStatusMask, pCumulativeStatus);
//       // increment while condition, typically the following:
//       if (!shouldCheckChildren(stateMask))
//       {
//           // do logic
//           goto exit;
//       }
//    }
//
//    // At target level
//    getNodeIndexStatus(node, index, NV_TRUE, pStatusMask, pCumulativeStatus);
//
static void
getNodeIndexStatus
(
    ADDRTREE_NODE *node,
    NvU32 index,
    NvBool bIsTargetLevel,
    PMA_PAGESTATUS *pStatusMask,
    PMA_PAGESTATUS *pCumulativeStatus
)
{
    PMA_PAGESTATUS curStatusMask = *pStatusMask;
    PMA_PAGESTATUS nextStatusMask = 0;
    NvU64 state = 0;
    NvU32 i;

    NV_ASSERT(index < node->numChildren);

    for (i = 0; i < PMA_BITS_PER_PAGE; i++)
    {
        if (curStatusMask & (NVBIT(i)))
        {
            if (node->seeChild[i] & NVBIT64(index))
            {
                nextStatusMask |= NVBIT64(i);
            }

            // Assert that either state or seeChild is set, not both
            NV_ASSERT(((node->seeChild[i] & node->state[i]) & (NVBIT64(index))) == 0);

            //
            // But state does need to be accurate and returned as PMA_PAGESTATUS
            // seeChild must be checked to get an accurate state
            //
            if ((node->state[i] | node->seeChild[i]) & (NVBIT64(index)))
            {
                state |= NVBIT64(i);
            }
        }
    }

    //NV_PRINTF(LEVEL_ERROR, "curStatusMask=0x%llx, nextStatusMask=0x%llx, index=0x%x, state=0x%llx\n",
    //          (NvU64)curStatusMask, nextStatusMask, index, state);

    *pStatusMask = nextStatusMask;

    if (bIsTargetLevel)
    {
        *pCumulativeStatus |= state;
    }
    else
    {
        *pCumulativeStatus |= state & ~nextStatusMask;
    }
}

static NvBool
shouldCheckChildren(PMA_PAGESTATUS statusMask)
{
    return statusMask != 0;
}

// This function returns if the current node holds valid information.
// If not, returns the effective state of this node
static NvBool
_addrtreeNodeValid
(
    PMA_ADDRTREE   *pTree,
    ADDRTREE_NODE  *node,
    PMA_PAGESTATUS *pFoundState
)
{
    ADDRTREE_NODE *n = pTree->root;
    NvU64 newFrame = 0;
    NvU32 newIndex = 0;
    *pFoundState = STATE_FREE;
    PMA_PAGESTATUS stateMask = MAP_MASK; // check all states TODO

    //NV_PRINTF(LEVEL_INFO, "Source level=%d frame=0x%llx.\n",
    //          node->level, node->frame);

    while(n->level != node->level)
    {
        _addrtreeConvertFrame(pTree, node->level, node->frame, n->level, &newFrame);
        newIndex = (NvU32)(newFrame - n->frame);

        getNodeIndexStatus(n, newIndex, NV_FALSE, &stateMask, pFoundState);

        if (!shouldCheckChildren(stateMask))
        {
            return NV_FALSE;
        }

        // Go to the right children to continue walking down
        n = &n->children[newIndex];
    }

    return NV_TRUE;

}

static NvU64
_addrtreeComputeMask(ADDRTREE_NODE *node, NvU64 frameStart, NvU64 numFrames)
{
    NV_ASSERT(node->numChildren <= 64);

    NvU64 mask = _makeMaskUpToIndex(node->numChildren);

    // If node is contained within the range, return a full mask
    if (RANGE_CONTAINS(frameStart, numFrames, node->frame, node->numChildren))
    {
        //NV_PRINTF(LEVEL_ERRORS, "frameStart=0x%llx, numFrames=0x%llx, node: start=0x%llx, num=0x%x, mask=0x%llx\n",
        //          frameStart, numFrames, node->frame, node->numChildren, mask);
        return mask;
    }
    else
    {
        // If the node doesn't cover the start, unset the beginning bits
        if (frameStart > node->frame)
        {
            mask &= (~_makeMaskUpToIndex((NvU32)(frameStart - node->frame)));
        }

        // If the node doesn't cover the end, unset the last bits
        if (frameStart + numFrames < node->frame + node->numChildren)
        {
            mask &= (_makeMaskUpToIndex((NvU32)(frameStart + numFrames - node->frame)));
        }
    }
    return mask;
}

//
// For the given state, for the mask of a node's children,
// the function returns
//
// NV_TRUE  if the mask is available
// NV_FALSE if the mask is not available
//          and the last child index which does not satisfy the given state
//
static NvBool
_addrtreeNodeMaskAvailable(
    ADDRTREE_NODE  *node,
    NvU64           mask,
    PMA_PAGESTATUS  state,
    NvBool          bReverse,
    NvU64          *pDiff
)
{
    NvU64 allocated = 0;
    NvU32 i;

    for (i = 0; i < PMA_BITS_PER_PAGE; i++)
    {
        //
        // allocated tracks which pages are not available.
        // We are looking for pages in free or state status.
        // Pages in state status are available, so exclude them.
        //
        if ((NVBIT(i) & state) == 0)
        {
            // Note that once we see a seeChild being set, we would
            // count that as allocated because some children are allocated
            allocated |= node->seeChild[i];
            allocated |= node->state[i];
        }
    }

    allocated &= mask;

    if (!bReverse)
    {
        //
        // Skip past all unavailable and return last child index
        // which does not satisfy the given state
        // This will underflow for allocated == 0, but in that case,
        // *pDiff should not be read anyway
        //
        *pDiff = 64 - portUtilCountLeadingZeros64(allocated) - 1;
    }
    else
    {
        // Return the first child index that doesn't satisfy given state
        *pDiff = portUtilCountTrailingZeros64(allocated);
    }

    return (allocated == 0);
}

// This function returns the node on a specific level that contains the frame
// The node may or may not contain valid information. That is handled later.
static NvU64
_addrtreeGetNodeIdx(PMA_ADDRTREE *pTree, NvU32 level, NvU64 frame)
{
    ADDRTREE_LEVEL *treeLevel = &pTree->levels[level];

    // Current level's frame number should be the same as next level's node number
    NV_ASSERT(frame < pTree->levels[level+1].nodeCount);

    return (frame / (treeLevel->maxFramesPerNode));
}

//
// Optimization: enable scanning functions to skip over fully allocated
// parent nodes with _pmaAddrtreeReadLevelAndSkipUnavailable
//
// For a given level and startFrame, return the number of frames to skip on the
// given level based on the parent's allocation state.
// Add this number to skip to the startFrame to get the next node which is not
// fully allocated.
//
PMA_PAGESTATUS
_pmaAddrtreeReadLevelAndSkipUnavailable
(
    void          *pMap,
    NvU32          levelNum,
    NvU64          frameNum,
    PMA_PAGESTATUS searchState,
    NvBool         bAllowFree,
    NvU64         *pNumFramesToSkip,
    NvBool         bReverse
)
{
    NvU32 index;
    ADDRTREE_NODE *pNode;
    PMA_ADDRTREE *pTree = (PMA_ADDRTREE *)pMap;
    ADDRTREE_NODE *n = pTree->root;
    NvU64 newFrame = 0;
    NvU32 newIndex;
    PMA_PAGESTATUS status = STATE_FREE;
    PMA_PAGESTATUS stateMask = MAP_MASK;

    _addrtreeConvertLevelFrameToNodeIndex(pTree, levelNum, frameNum, &pNode, &index);

    while (n->level != pNode->level)
    {
        _addrtreeConvertFrame(pTree, pNode->level, pNode->frame, n->level, &newFrame);
        newIndex = (NvU32)(newFrame - n->frame);

        getNodeIndexStatus(n, newIndex, NV_FALSE, &stateMask, &status);

        if (!shouldCheckChildren(stateMask))
        {
            //
            // There's no need to scan further down.
            // Return the number of frames to skip over on the target level
            // in order to skip over this n->level ancestor.
            //

            NvBool bWrongState = (status != searchState) && !(bAllowFree && (status == STATE_FREE));

            if (bWrongState)
            {
                //
                // This node is fully allocated.
                // Return the number of frames of the target level we should skip.
                //
                NvU64 targetFrameStartForThisNode;
                NvU64 numTargetFramesPerAncestor;

                _addrtreeConvertFrame(pTree, n->level, newFrame, levelNum, &targetFrameStartForThisNode);
                _addrtreeConvertFrame(pTree, n->level, 1ULL, levelNum, &numTargetFramesPerAncestor);

                if (!bReverse)
                    *pNumFramesToSkip = numTargetFramesPerAncestor - (frameNum - targetFrameStartForThisNode);
                else
                    *pNumFramesToSkip = frameNum - targetFrameStartForThisNode + 1;
            } else {
                //
                // This node is in a state we're searching for.
                // The caller will store off one frame of the target level
                //
                *pNumFramesToSkip = 1;
            }

            goto exit;
        }

        // Go to the right children to continue walking down
        n = &n->children[newIndex];
    }

    *pNumFramesToSkip = 1;

    // Extract the final value from this pNode
    getNodeIndexStatus(pNode, index, NV_TRUE, &stateMask, &status);

exit:
    NV_ASSERT(*pNumFramesToSkip > 0);
    return status;
}

//
// For the given state, between frameStart and (frameStart + numFrames - 1)
// for a given level, this function returns
//
// NV_TRUE  if the range is available
// NV_FALSE if the range is not available
//          and the frame number of the last frame which does not
//          satisfy the given state in the variable pDiff
//
// XXX: Caution! This will not properly pick up nodes only in state `state`
// If it were used for discontig scanning, it would be wrong!
//
static NvBool
_pmaAddrtreeAvailable
(
    PMA_ADDRTREE  *pTree,
    NvU32          level,
    PMA_PAGESTATUS state,
    NvU64          frameStart,
    NvU64          numFrames,
    NvU64          *pDiff
)
{
    NvU64 i, startIdx, endIdx, childrenMask;
    PMA_PAGESTATUS foundState;
    NvU64 nodeIndexDiff;
    ADDRTREE_NODE *node;

    NV_ASSERT(level != 0); // TODO handle the root node case
    //NV_PRINTF(LEVEL_INFO, "level=%d, frameStart=0x%llx, numFrames=%llx\n",
    //          level, frameStart, numFrames);

    startIdx = _addrtreeGetNodeIdx(pTree, level, frameStart);
    endIdx   = _addrtreeGetNodeIdx(pTree, level, (frameStart + numFrames - 1));

    //NV_PRINTF(LEVEL_INFO, "startIdx = %llx, endIdx = 0x%llx\n", startIdx, endIdx);

    // Begin checking from the end so that we can skip the most frames in the overall search
    for (i = endIdx; i >= startIdx; i--)
    {
        // Protect against underflow
        if (i == (NvU64)-1) break;

        //NV_PRINTF(LEVEL_INFO, "IN LOOP: i=0x%llx, startIdx=%llx\n", i, startIdx);
        node = &(pTree->levels[level].pNodeList[i]);
        if (_addrtreeNodeValid(pTree, node, &foundState))
        {
            childrenMask = _addrtreeComputeMask(node, frameStart, numFrames);

            if (!_addrtreeNodeMaskAvailable(node, childrenMask, state, NV_FALSE, &nodeIndexDiff))
            {
                *pDiff = node->frame + nodeIndexDiff;
                return NV_FALSE;
            }
        }
        else
        {
            //NV_PRINTF(LEVEL_INFO, "IN LOOP: Node is INVALID. allocated?-%d\n",
            //          (NvU32)state);
            if ((foundState != STATE_FREE) && (foundState != state))
            {
                // This node is completely allocated.
                // Return the frame number of the last frame in this node
                *pDiff = node->frame + node->numChildren - 1;
                return NV_FALSE;
            }
            else
            {
                //
                // This node is completely free or in a state we're looking for,
                // continue checking
                //
                continue;
            }
        }
    }

    return NV_TRUE;
}

//
// For the given state, between frameStart and (frameStart + numFrames - 1)
// for a given level, this function returns
//
// NV_TRUE  if the range is available
// NV_FALSE if the range is not available
//          and the frame number of the first frame which does not
//          satisfy the given state in the variable pDiff
//
// XXX: Caution! This will not properly pick up nodes only in state `state`
// If it were used for discontig scanning, it would be wrong!
//
static NvBool
_pmaAddrtreeAvailableReverse
(
    PMA_ADDRTREE  *pTree,
    NvU32          level,
    PMA_PAGESTATUS state,
    NvU64          frameStart,
    NvU64          numFrames,
    NvU64          *pDiff
)
{
    NvU64 i, startIdx, endIdx, childrenMask;
    PMA_PAGESTATUS foundState;
    NvU64 nodeIndexDiff;
    ADDRTREE_NODE *node;

    NV_ASSERT(level != 0); // TODO handle the root node case

    startIdx = _addrtreeGetNodeIdx(pTree, level, frameStart);
    endIdx   = _addrtreeGetNodeIdx(pTree, level, (frameStart + numFrames - 1));

    // For reverse alloc, begin checking from the start so that we can skip the most frames in the overall search
    for (i = startIdx; i <= endIdx; i++)
    {
        node = &(pTree->levels[level].pNodeList[i]);
        if (_addrtreeNodeValid(pTree, node, &foundState))
        {
            childrenMask = _addrtreeComputeMask(node, frameStart, numFrames);

            if (!_addrtreeNodeMaskAvailable(node, childrenMask, state, NV_TRUE, &nodeIndexDiff))
            {
                *pDiff = node->frame + nodeIndexDiff;
                return NV_FALSE;
            }
        }
        else
        {
            if ((foundState != STATE_FREE) && (foundState != state))
            {
                // This node is completely allocated.
                // Return the frame number of the first frame in this node
                *pDiff = node->frame;
                return NV_FALSE;
            }
            else
            {
                //
                // This node is completely free or in a state we're looking for,
                // continue checking
                //
                continue;
            }
        }
    }

    return NV_TRUE;
}

static NvBool
_pmaAddrtreeContigSearchLoop
(
    PMA_ADDRTREE  *pTree,
    NvU32          level,
    PMA_PAGESTATUS state,
    NvU64          addrBase,
    NvU64          localStart,
    NvU64          localEnd,
    NvU64          numFrames,
    NvU64          frameAlignment,
    NvU64         *freeList
)
{
    NvBool found = NV_FALSE;
    NvU64 freeStart;
    NvU64 diff;
    PMA_PAGESTATUS startStatus, endStatus;

    if ((state != STATE_FREE) && (state != STATE_UNPIN))
    {
        NV_PRINTF(LEVEL_INFO, "Scanning for state %d is not supported\n", state);
        return found;
    }

    freeStart = localStart;
    while (!found)
    {
        NvU64 endFrame = freeStart + numFrames - 1;
        NvU64 framesToSkip = 0;

        if (endFrame > localEnd)
        {
            // freeStart + numFrames too close to local search end.  Re-starting search
            break;
        }

        //
        // Read endStatus first so we don't have to waste time traversing the
        // tree again to read startStatus if endStatus is not even usable
        //
        endStatus = _pmaAddrtreeReadLevelAndSkipUnavailable(pTree, level, endFrame, state, NV_TRUE, &framesToSkip, NV_FALSE);

        if (framesToSkip > 1) {
            freeStart = NV_ALIGN_UP(endFrame + framesToSkip, frameAlignment);
            NV_ASSERT(freeStart != 0);
            continue;
        }

        startStatus = _pmaAddrtreeReadLevelAndSkipUnavailable(pTree, level, freeStart, state, NV_TRUE, &framesToSkip, NV_FALSE);

        if (framesToSkip > 1) {
            freeStart += NV_ALIGN_UP(framesToSkip, frameAlignment);
            NV_ASSERT(freeStart != 0);
            continue;
        }

        if ((endStatus == STATE_FREE) || (endStatus == state))
        {
            if ((startStatus == STATE_FREE) || (startStatus == state))
            {
                if (_pmaAddrtreeAvailable(pTree, level, state, freeStart, numFrames, &diff))
                {
                    found = NV_TRUE;
                    // Substract off padding when returning
                    *freeList = addrBase + ((freeStart << pTree->levels[level+1].pageSizeShift) -
                                (pTree->numPaddingFrames << PMA_PAGE_SHIFT));
                    //NV_PRINTF(LEVEL_INFO, "found! 0x%llx\n", freeStart);
                }
                else
                {
                    //NV_PRINTF(LEVEL_INFO, "Frame number of allocated frame = 0x%llx\n",
                    //          diff);
                    //
                    // Find the next aligned free frame and set it as the start
                    // frame for next iteration's scan.
                    //
                    freeStart = NV_ALIGN_UP(diff + 1, frameAlignment);
                    NV_ASSERT(freeStart != 0);
                }
            }
            else
            {
                // Start point isn't free, so bump to check the next aligned frame
                freeStart += frameAlignment;
            }
        }
        else
        {
            //
            // End point isn't usable, so jump to after the end to check again
            // However, align the new start point properly before next iteration.
            //
            freeStart += NV_ALIGN_UP(numFrames, frameAlignment);
        }
    }

    return found;
}

static NvBool
_pmaAddrtreeContigSearchLoopReverse
(
    PMA_ADDRTREE  *pTree,
    NvU32          level,
    PMA_PAGESTATUS state,
    NvU64          addrBase,
    NvU64          localStart,
    NvU64          localEnd,
    NvU64          numFrames,
    NvU64          frameAlignment,
    NvU64         *freeList
)
{
    NvBool found = NV_FALSE;
    NvU64 freeStart;
    NvU64 diff;
    PMA_PAGESTATUS startStatus, endStatus;

    if ((state != STATE_FREE) && (state != STATE_UNPIN))
    {
        NV_PRINTF(LEVEL_INFO, "Scanning for state %d is not supported\n", state);
        return found;
    }

    freeStart = localEnd + 1 - numFrames;  // First frame from end able to accommodate num_frames allocation.
    freeStart = NV_ALIGN_DOWN(freeStart, frameAlignment);
    while (!found)
    {
        NvU64 framesToSkip = 0;

        if (freeStart < localStart || (NvS64)freeStart < 0)  // Account for underflow
        {
            // freeStart too close to local search start. Re-starting search
            break;
        }

        //
        // For reverse allocation, read startStatus first so we don't have to waste time
        // traversing the tree again to read endStatus if startStatus is not even usable
        //
        startStatus = _pmaAddrtreeReadLevelAndSkipUnavailable(pTree, level, freeStart, state, NV_TRUE, &framesToSkip, NV_TRUE);
        if (framesToSkip > 1) {
            NvU64 newEndFrame = freeStart - framesToSkip;
            freeStart = newEndFrame + 1 - numFrames;
            freeStart = NV_ALIGN_DOWN(freeStart, frameAlignment);
            continue;
        }

        endStatus = _pmaAddrtreeReadLevelAndSkipUnavailable(pTree, level, freeStart + numFrames - 1, state, NV_TRUE, &framesToSkip, NV_TRUE);
        if (framesToSkip > 1) {
            freeStart -= NV_ALIGN_UP(framesToSkip, frameAlignment);
            continue;
        }

        if ((startStatus == STATE_FREE) || (startStatus == state))
        {
            if ((endStatus == STATE_FREE) || (endStatus == state))
            {
                if (_pmaAddrtreeAvailableReverse(pTree, level, state, freeStart, numFrames, &diff))
                {
                    found = NV_TRUE;
                    // Subtract off padding when returning
                    *freeList = addrBase + ((freeStart << pTree->levels[level+1].pageSizeShift) -
                                (pTree->numPaddingFrames << PMA_PAGE_SHIFT));
                    //NV_PRINTF(LEVEL_INFO, "found! 0x%llx\n", freeStart);
                }
                else
                {
                    //NV_PRINTF(LEVEL_INFO, "Frame number of allocated frame = 0x%llx\n",
                    //          diff);
                    //
                    // Find the next aligned free frame and set it as the end
                    // frame for next iteration's scan.
                    //
                    freeStart = NV_ALIGN_DOWN(diff - numFrames, frameAlignment);
                }
            }
            else
            {
                // Start point isn't free, so bump to check the next aligned frame
                freeStart -= frameAlignment;
            }
        }
        else
        {
            //
            // End point isn't usable, so jump to after the end to check again
            // However, align the new start point properly before next iteration.
            //
            freeStart -= NV_ALIGN_UP(numFrames, frameAlignment);
        }
    }

    return found;
}

static NV_STATUS
_pmaAddrtreeScanContiguous
(
    void *pMap,
    NvU64 addrBase,
    NvU64 rangeStart,
    NvU64 rangeEnd,
    NvU64 numPages,
    NvU64 *freeList,
    NvU64 pageSize,
    NvU64 alignment,
    NvU64 *numPagesAlloc,
    NvBool bSkipEvict,
    NvBool bReverseAlloc
)
{
    NvU64 localStart, localEnd, frameAlignment;
    NvBool found;
    NvU32 level;
    PMA_ADDRTREE *pTree = (PMA_ADDRTREE *)pMap;

    // This requirement is ensured in PMA
    NV_ASSERT(alignment >= pageSize && portUtilIsPowerOfTwo(alignment));

    *numPagesAlloc = 0;

    // Only focus on the level above the pageSize level. Children are ignored.
    level = addrtreeGetTreeLevel(pageSize);
    if (level == 0)
    {
        NV_PRINTF(LEVEL_ERROR, "address tree cannot handle page size 0x%llx\n",
                               pageSize);
        return NV_ERR_INVALID_ARGUMENT;
    }

    frameAlignment = alignment / pageSize;

    // Handle restricted allocations
    if (rangeStart != 0 || rangeEnd != 0)
    {
        rangeStart += (pTree->numPaddingFrames << PMA_PAGE_SHIFT);
        rangeEnd += (pTree->numPaddingFrames << PMA_PAGE_SHIFT);
        localStart = NV_ALIGN_UP64(rangeStart, alignment) >> pTree->levels[level].pageSizeShift;
        localEnd   = NV_MIN(rangeEnd >> pTree->levels[level].pageSizeShift, pTree->levels[level].nodeCount - 1);
    }
    else
    {
        localStart = NV_ALIGN_UP64(pTree->numPaddingFrames << PMA_PAGE_SHIFT, alignment) >> pTree->levels[level].pageSizeShift;
        localEnd   = pTree->levels[level].nodeCount - 1;
    }

    //NV_PRINTF(LEVEL_INFO, "Scanning level %d with addrBase 0x%llx in frame range 0x%llx..0x%llx, "
    //                      "pages to allocate 0x%llx (pageSize=0x%x, alignment=0x%x)\n",
    //                      (level - 1), addrBase, localStart, localEnd, numPages, pageSize, alignment);
    if (!bReverseAlloc)
    {
        found = _pmaAddrtreeContigSearchLoop(pTree, level - 1, STATE_FREE, addrBase, localStart, localEnd,
                                             numPages, frameAlignment, freeList);
    }
    else
    {
        found = _pmaAddrtreeContigSearchLoopReverse(pTree, level - 1, STATE_FREE, addrBase, localStart, localEnd,
                                                    numPages, frameAlignment, freeList);
    }

    if (found)
    {
        *numPagesAlloc = numPages;
        return NV_OK;
    }

    if (bSkipEvict)
    {
        return NV_ERR_NO_MEMORY;
    }

    // Loop back to the beginning and continue searching for evictable pages
    if (!bReverseAlloc)
    {
        found = _pmaAddrtreeContigSearchLoop(pTree, level - 1, STATE_UNPIN, addrBase, localStart, localEnd,
                                             numPages, frameAlignment, freeList);
    }
    else
    {
        found = _pmaAddrtreeContigSearchLoopReverse(pTree, level - 1, STATE_UNPIN, addrBase, localStart, localEnd,
                                                    numPages, frameAlignment, freeList);
    }
    if (found)
        return NV_ERR_IN_USE;
    else
        return NV_ERR_NO_MEMORY;
}

//
// This function wraps the real _pmaAddrtreeScanContiguous
// to allow addrtree to scan for 128KB page size
//
NV_STATUS
pmaAddrtreeScanContiguous
(
    void *pMap,
    NvU64 addrBase,
    NvU64 rangeStart,
    NvU64 rangeEnd,
    NvU64 numPages,
    NvU64 *freeList,
    NvU64 pageSize,
    NvU64 alignment,
    NvU64 *numPagesAlloc,
    NvBool bSkipEvict,
    NvBool bReverseAlloc
)
{
    if (NV_UNLIKELY(pageSize == _PMA_128KB)) {
        //
        // Call the contig function with twice as many 64KB frames,
        // and cut in half the number of allocated frames.
        // Contig allocations are all or nothing so the number of
        // allocated frames cannot be odd.
        //
        NV_STATUS status;

        status = _pmaAddrtreeScanContiguous(
            pMap,
            addrBase,
            rangeStart,
            rangeEnd,
            numPages * 2,
            freeList,
            _PMA_64KB,
            alignment,
            numPagesAlloc,
            bSkipEvict,
            bReverseAlloc);

        *numPagesAlloc /= 2;

        return status;
    }
    else
    {
        return _pmaAddrtreeScanContiguous(
            pMap,
            addrBase,
            rangeStart,
            rangeEnd,
            numPages,
            freeList,
            pageSize,
            alignment,
            numPagesAlloc,
            bSkipEvict,
            bReverseAlloc);
    }
}

static NvU64
_pmaAddrtreeDiscontigSearchLoop
(
    PMA_ADDRTREE  *pTree,
    NvU32          level,
    PMA_PAGESTATUS state,
    NvU64          addrBase,
    NvU64          localStart,
    NvU64          localEnd,
    NvU64          numFrames,
    NvU64          frameAlignment,
    NvU64         *freeList,
    NvBool         bReverseAlloc
)
{
    NvU64 found = 0;
    NvU64 freeStart;
    PMA_PAGESTATUS startStatus;

    if ((state != STATE_FREE) && (state != STATE_UNPIN))
    {
        NV_PRINTF(LEVEL_INFO, "Scanning for state %d is not supported\n", state);
        return found;
    }

    freeStart = !bReverseAlloc ? localStart : localEnd;

    //
    // We only need one frame at a time on this level,
    // so we can skip much of the frame logic
    //
    while (found != numFrames)
    {
        NvU64 framesToSkip = 0;

        if (freeStart > localEnd || freeStart < localStart || (NvS64)freeStart < 0) break;

        //
        // For discontig, we MUST only pick up the exact state.
        // Otherwise we give away pages for eviction that we already stored off to be allocated.
        //
        startStatus = _pmaAddrtreeReadLevelAndSkipUnavailable(pTree, level, freeStart, state, NV_FALSE, &framesToSkip, bReverseAlloc);

        if (startStatus == state)
        {
            // Substract off padding when returning
            freeList[found++] = addrBase + ((freeStart << pTree->levels[level+1].pageSizeShift) -
                                (pTree->numPaddingFrames << PMA_PAGE_SHIFT));
        }
        freeStart = !bReverseAlloc ? (freeStart + framesToSkip) : (freeStart - framesToSkip);
    }

    return found;
}

static NV_STATUS
_pmaAddrtreeScanDiscontiguous
(
    void *pMap,
    NvU64 addrBase,
    NvU64 rangeStart,
    NvU64 rangeEnd,
    NvU64 numPages,
    NvU64 *freeList,
    NvU64 pageSize,
    NvU64 alignment,
    NvU64 *numPagesAlloc,
    NvBool bSkipEvict,
    NvBool bReverseAlloc
)
{
    NvU64 localStart, localEnd;
    NvU64 foundFree;
    NvU64 foundEvictable;
    NvU32 level;
    PMA_ADDRTREE *pTree = (PMA_ADDRTREE *)pMap;

    // This requirement is ensured in PMA
    NV_ASSERT(alignment == pageSize);

    // Only focus on the level above the pageSize level. Children are ignored.
    level = addrtreeGetTreeLevel(pageSize);
    if (level == 0)
    {
        NV_PRINTF(LEVEL_ERROR, "address tree cannot handle page size 0x%llx\n",
                               pageSize);
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Handle restricted allocations
    if (rangeStart != 0 || rangeEnd != 0)
    {
        // Embedded % requires special handling.
        NV_ASSERT_OR_ELSE_STR(rangeStart % pageSize == 0,
            "rangeStart %% pageSize == 0", /*do nothing*/);
        NV_ASSERT_OR_ELSE_STR((rangeEnd + 1) % pageSize == 0,
            "(rangeEnd + 1) %% pageSize == 0", /*do nothing*/);

        rangeStart += (pTree->numPaddingFrames << PMA_PAGE_SHIFT);
        rangeEnd += (pTree->numPaddingFrames << PMA_PAGE_SHIFT);
        localStart = NV_ALIGN_UP(rangeStart, alignment) >> pTree->levels[level].pageSizeShift;
        localEnd   = NV_MIN(rangeEnd >> pTree->levels[level].pageSizeShift, pTree->levels[level].nodeCount - 1);
    }
    else
    {
        localStart = NV_ALIGN_UP64(pTree->numPaddingFrames << PMA_PAGE_SHIFT, alignment) >> pTree->levels[level].pageSizeShift;
        localEnd   = pTree->levels[level].nodeCount - 1;
    }

    //NV_PRINTF(LEVEL_INFO, "Scanning level %d with addrBase 0x%llx in frame range 0x%llx..0x%llx, "
    //                      "pages to allocate 0x%llx (pageSize=0x%x, alignment=0x%x)\n",
    //                      (level - 1), addrBase, localStart, localEnd, numPages, pageSize, alignment);

    foundFree = _pmaAddrtreeDiscontigSearchLoop(pTree, level - 1, STATE_FREE, addrBase, localStart, localEnd,
                                                numPages, alignment, freeList, bReverseAlloc);


    // numPagesAlloc must be set for partial allocations
    *numPagesAlloc = foundFree;

    if (foundFree == numPages)
    {
        return NV_OK;
    }
    else if (bSkipEvict)
    {
        return NV_ERR_NO_MEMORY;
    }

    //
    // Loop back to the beginning and continue searching for evictable pages
    // This next search picks up only evictable pages and not free pages
    //
    foundEvictable = _pmaAddrtreeDiscontigSearchLoop(pTree, level - 1, STATE_UNPIN, addrBase, localStart, localEnd,
                                                     (numPages - foundFree), alignment, (freeList + foundFree), bReverseAlloc);

    if ((foundFree + foundEvictable) == numPages)
        return NV_ERR_IN_USE;
    else
        return NV_ERR_NO_MEMORY;
}

//
// This function wraps the real _pmaAddrtreeScanDiscontiguous
// to allow addrtree to scan for 128KB page size
//
NV_STATUS
pmaAddrtreeScanDiscontiguous
(
    void *pMap,
    NvU64 addrBase,
    NvU64 rangeStart,
    NvU64 rangeEnd,
    NvU64 numPages,
    NvU64 *freeList,
    NvU64 pageSize,
    NvU64 alignment,
    NvU64 *numPagesAlloc,
    NvBool bSkipEvict,
    NvBool bReverseAlloc
)
{
    if (NV_UNLIKELY(pageSize == _PMA_128KB)) {
        NV_STATUS status = NV_OK;
        NvU64 i;
        NvU64 localNumPagesAlloc;
        *numPagesAlloc = 0;

        if (rangeEnd == 0) {
            NV_ASSERT(rangeStart == 0);
            rangeEnd = ((PMA_ADDRTREE *)pMap)->totalFrames << PMA_PAGE_SHIFT;
        }

        for (i = 0; i < numPages; i++) {
            //
            // Only call the contig function because we need the two frames to be
            // contiguous.
            // Alignment only needs to be aligned to 64KB and power of 2,
            // so it is ok to pass through even if it is 128KB
            //
            status = _pmaAddrtreeScanContiguous(
                pMap,
                addrBase,
                rangeStart,
                rangeEnd,
                2,
                freeList + i,
                _PMA_64KB,
                alignment,
                &localNumPagesAlloc,
                bSkipEvict,
                bReverseAlloc);

            // Give back the first of every two 64KB frames
            *numPagesAlloc += localNumPagesAlloc / 2;

            if (status != NV_OK)
            {
                return status;
            }

            if (!bReverseAlloc)
                rangeStart = freeList[i] + _PMA_128KB;
            else
            {
                rangeEnd = freeList[i] - 1;
                if (rangeEnd < rangeStart || (NvS64)rangeEnd < 0)
                {
                    // Extended the current implementation to reverse alloc here but
                    // isn't this logic incorrect for tryEvict case? As we are closing
                    // off the region for tryEvict case after an allocation is made.
                    // Also we don't check evictable for remaining pages after the
                    // first NV_ERR_IN_USE is returned.
                    if (i < numPages - 1) return NV_ERR_NO_MEMORY;
                }
            }
        }

        return status;
    }
    else
    {
        return _pmaAddrtreeScanDiscontiguous(
            pMap,
            addrBase,
            rangeStart,
            rangeEnd,
            numPages,
            freeList,
            pageSize,
            alignment,
            numPagesAlloc,
            bSkipEvict,
            bReverseAlloc);
    }
}

// Either set or clear a specified bit in old and return the result
static NvU64
replaceBit(NvU64 old, NvU32 bit, NvBool bIsSet)
{
    NV_ASSERT(bit < 64);
    NvU64 mask = NVBIT64(bit);
    return bIsSet ? (old | mask) : (old & ~mask);
}

static void
_addrtreeUpdateAncestors
(
    PMA_ADDRTREE   *pTree,
    ADDRTREE_NODE  *pNode,
    PMA_PAGESTATUS  newStateMask
)
{
    ADDRTREE_NODE *pParent;
    NvU32 i;
    NvU64 newFrame;
    NvU32 newIndex;
    PMA_PAGESTATUS stateMask;
    NvU64 fillPattern;

    pParent = pNode->parent;

    while (pParent != NULL)
    {
        //NV_PRINTF(LEVEL_INFO, "Current level %d Maybe updating parent at level %d newStateMask=%x\n",
        //          pNode->level, pNode->level - 1, newStateMask);

        if (newStateMask == 0)
            break;

        fillPattern = _makeMaskUpToIndex(pNode->numChildren);
        _addrtreeConvertFrame(pTree, pNode->level, pNode->frame, pParent->level, &newFrame);
        newIndex = (NvU32)(newFrame - pParent->frame);

        for (i = 0; i < PMA_BITS_PER_PAGE; i++)
        {
            stateMask = NVBIT(i);

            if (stateMask & newStateMask)
            {
                NvBool bSeeChild;
                NvBool bSetState;
                NvU64 newSeeChild;
                NvU64 newState;

                // Calculate what this node should look like to the parent
                bSetState = NV_FALSE;
                bSeeChild = NV_TRUE;
                if (pNode->seeChild[i] == 0)
                {
                    if (pNode->state[i] == fillPattern)
                    {
                        bSetState = NV_TRUE;
                        bSeeChild = NV_FALSE;
                    }
                    else if (pNode->state[i] == 0)
                    {
                        bSeeChild = NV_FALSE;
                    }
                }

                newSeeChild = replaceBit(pParent->seeChild[i], newIndex, bSeeChild);
                newState  = replaceBit(pParent->state[i], newIndex, bSetState);

                //NV_PRINTF(LEVEL_INFO, "bSetState %d bSeeChild %d newSeeChild=0x%llx newState=0x%llx \n",
                //    bSetState, bSeeChild, newSeeChild, newState);

                //
                // If the parent won't change from this update, do not do any further
                // checking on this state: remove it from the mask
                //
                if (pParent->seeChild[i] == newSeeChild && pParent->state[i] == newState)
                {
                    newStateMask &= ~stateMask;
                    continue;
                }

                //NV_PRINTF(LEVEL_INFO, "update parent\n");

                pParent->seeChild[i] = newSeeChild;
                pParent->state[i] = newState;

            }
        }

        pNode = pParent;
        pParent = pNode->parent;
    }

}

//
// This function traverses the tree and changes the state of the frame at `index`
// in `node` to the desired states and change all its ancestors to set seeChild.
// Note that this function also makes `node` a valid node unconditionally.
//
static void
_addrtreeSetState
(
    PMA_ADDRTREE   *pTree,
    ADDRTREE_NODE  *pNode,
    NvU32           index,
    PMA_PAGESTATUS  newState,
    PMA_PAGESTATUS  newStateMask
)
{
    ADDRTREE_NODE *n = pTree->root;
    ADDRTREE_NODE *pChildNode;
    NvU32 i;
    NvU64 newFrame = 0;
    NvU32 newIndex;
    NvU64 stateMask;
    NvU64 childMask = 0;

    //NV_PRINTF(LEVEL_INFO, "Source level=%d frame=0x%llx.\n",
    //          pNode->level, pNode->frame);
    NV_ASSERT(index < pNode->numChildren);

    // Walk down from root and update all its ancestors
    while(n->level != pNode->level)
    {
        _addrtreeConvertFrame(pTree, pNode->level, pNode->frame, n->level, &newFrame);
        newIndex = (NvU32)(newFrame - n->frame);

        //NV_PRINTF(LEVEL_INFO, "Going to traverse level=%d newFrame=0x%x newIndex=0x%x \n",
        //          n->level, newFrame, newIndex);

        childMask = NVBIT64(newIndex);
        pChildNode = &n->children[newIndex];

        for (i = 0; i < PMA_BITS_PER_PAGE; i++)
        {
            stateMask = NVBIT64(i);

            if (stateMask & newStateMask)
            {
                //
                // If entire node has some state, update children to contain the same state
                // Only update child state if this node's seeChild is 0
                //
                if ((childMask & n->seeChild[i]) == 0)
                {
                    if ((n->state[i] & childMask) != 0)
                    {
                        //
                        // Note that we may overwrite the pinned state of the edge cases here,
                        // but they will still be in the correct state since they cannot be the
                        // ones being freed.
                        //
                        pChildNode->state[i] = _makeMaskUpToIndex(pChildNode->numChildren);
                    }
                    else
                    {
                        //
                        // There is no risk of overwriting the ALLOC_PIN frames set to
                        // protect the end-of-region edge case. If seeChild == 0, these nodes
                        // and all ancestors must be in the set state, not the clear state
                        // because addrtree will not give out frames that are out of bounds
                        //
                        pChildNode->state[i] = 0;
                    }

                    // Set the child's seeChild to 0 so we force update it next iteration
                    pChildNode->seeChild[i] = 0;
                }
                n->seeChild[i] |= childMask;
            }
        }
        // Go to the right children to continue walking down
        n = pChildNode;
    }


    //NV_PRINTF(LEVEL_INFO, "Setting pNode level=%d frame=0x%llx index=0x%x to state=0x%llx mask=0x%llx\n",
    //          pNode->level, pNode->frame, index, newState, newStateMask);

    //
    // Important loop to actually set the state bits now.
    // Do not refactor this unless you know what you are doing!
    // Update the node, then go update the ancestors.
    //
    for (i = 0; i < PMA_BITS_PER_PAGE; i++)
    {
        stateMask = NVBIT64(i);

        if (stateMask & newStateMask)
        {
            // Set the current node index as valid since we are changing its states
            pNode->seeChild[i] &= ~NVBIT64(index);

            if (stateMask & newStateMask & newState)
            {
                pNode->state[i] |= NVBIT64(index);
            }
            else
            {
                pNode->state[i] &= ~NVBIT64(index);
            }
        }
    }

    _addrtreeUpdateAncestors(pTree, pNode, newStateMask);
}

static PMA_PAGESTATUS
_addrtreeGetState
(
    PMA_ADDRTREE   *pTree,
    ADDRTREE_NODE  *node,
    NvU32           index,
    NvBool          bReadAttrib
)
{
    ADDRTREE_NODE *n = pTree->root;

    NvU64 newFrame = 0;
    NvU32 newIndex;
    PMA_PAGESTATUS status = STATE_FREE;
    PMA_PAGESTATUS stateMask = bReadAttrib ? MAP_MASK : STATE_MASK;

    while(n->level != node->level)
    {
        _addrtreeConvertFrame(pTree, node->level, node->frame, n->level, &newFrame);
        newIndex = (NvU32)(newFrame - n->frame);

        //NV_PRINTF(LEVEL_INFO, "n->level=0x%x, node->level=0x%x\n", n->level, node->level);

        getNodeIndexStatus(n, newIndex, NV_FALSE, &stateMask, &status);

        if (!shouldCheckChildren(stateMask))
        {
            return status;
        }

        // Go to the right children to continue walking down
        n = &n->children[newIndex];
    }

    // Extract the final value from this node
    getNodeIndexStatus(node, index, NV_TRUE, &stateMask, &status);

    return status;
}

// TODO: this is pretty similar to _addrtreeConvertFrame: maybe combine

static void
_addrtreeConvertLevelFrameToNodeIndex
(
    PMA_ADDRTREE   *pTree,
    NvU32           levelNum,
    NvU64           frameNum,
    ADDRTREE_NODE **ppNode,
    NvU32          *pIndex
)
{
    NvU32 nodeIdx;
    ADDRTREE_LEVEL *pLevel;
    NvU32 framesPerNode;

    pLevel = &pTree->levels[levelNum];
    framesPerNode = pLevel->maxFramesPerNode;

    nodeIdx = (NvU32)(frameNum / framesPerNode);
    *pIndex = (NvU32)(frameNum % framesPerNode);
    *ppNode = &pLevel->pNodeList[nodeIdx];

    NV_ASSERT(*pIndex < (*ppNode)->numChildren);
}

//frameNum is a levelNum frame
static PMA_PAGESTATUS
pmaAddrtreeReadLevel
(
    void  *pMap,
    NvU32  levelNum,
    NvU64  frameNum,
    NvBool bReadAttrib
)
{
    NvU32 index;
    ADDRTREE_NODE *pNode;
    PMA_PAGESTATUS state;
    PMA_ADDRTREE *pTree = (PMA_ADDRTREE *)pMap;

    _addrtreeConvertLevelFrameToNodeIndex(pTree, levelNum,
                                  frameNum, &pNode, &index);

    state = _addrtreeGetState(pTree, pNode, index, bReadAttrib);
    return state;
}

//
// This function returns if the node pointed to by pNode index
// has any seeChild bits set for those seeChild bits that are valid for this node.
// It also returns the value of the pNode index in pState
// This is very similar to _addrtreeNodeValid, but requires some tweaked logic
// This is necessary for a very specific check in _pmaAddrtreeChangePageStateAttribEx
// This node must have at least one more level beneath it!
//
static NvBool
_addrtreeNodeIndexHasSeeChildSet
(
    PMA_ADDRTREE   *pTree,
    ADDRTREE_NODE  *pNode,
    NvU32           index,
    PMA_PAGESTATUS *pState
)
{
    ADDRTREE_NODE *n = pTree->root;
    NvU64 newFrame = 0;
    NvU32 newIndex = 0;
    *pState = STATE_FREE;

    // TODO: try this for only STATE_MASK, because stats will only
    // get corrupted if the STATE_MASK values differ.
    PMA_PAGESTATUS stateMask = MAP_MASK; // check all states TODO

    while(n->level != pNode->level)
    {
        _addrtreeConvertFrame(pTree, pNode->level, pNode->frame, n->level, &newFrame);
        newIndex = (NvU32)(newFrame - n->frame);

        getNodeIndexStatus(n, newIndex, NV_FALSE, &stateMask, pState);

        if (!shouldCheckChildren(stateMask))
        {
            // State is fully realized in an ancestor above the parent
            return NV_FALSE;
        }

        // Go to the right children to continue walking down
        n = &n->children[newIndex];
    }

    // Extract the value from this pNode
    getNodeIndexStatus(pNode, index, NV_FALSE, &stateMask, pState);

    // Now check if the final child has any seeChild set
    if (shouldCheckChildren(stateMask))
    {
        // Target child is different from parent
        return NV_TRUE;
    }
    else
    {
        // State is fully realized in parent
        return NV_FALSE;
    }
}

// frameNumStart is the 64k frameNum to start with
static void
__pmaAddrtreeChangePageStateAttribEx
(
    void           *pMap,
    NvU64           frameNumStart,
    NvU64           pageSize,
    PMA_PAGESTATUS  newState,
    PMA_PAGESTATUS  newStateMask
)
{
    NvU32 index;
    ADDRTREE_NODE *pNode;
    NvU32 targetLevelNum = addrtreeGetTreeLevel(pageSize) - 1;
    NvU64 targetFrameNum;
    NvU32 levelNum = addrtreeGetTreeLevel(_PMA_64KB) - 1;
    NvU32 levelNum2mb = addrtreeGetTreeLevel(_PMA_2MB) - 1;
    PMA_PAGESTATUS oldState, updatedState, oldState2mb, updatedState2mb;
    PMA_ADDRTREE *pTree = (PMA_ADDRTREE *)pMap;
    PMA_PAGESTATUS targetFoundState;
    ADDRTREE_NODE *pTargetNode;
    NvU32 targetIndex;

    frameNumStart += pTree->numPaddingFrames;
    newStateMask &= MAP_MASK;

    _addrtreeConvertFrame(pTree, levelNum, frameNumStart, targetLevelNum, &targetFrameNum);

    _addrtreeConvertLevelFrameToNodeIndex(pTree, targetLevelNum, targetFrameNum, &pTargetNode, &targetIndex);

    //
    // In address tree, if a node is partially allocated, any node above that node is considered fully
    // allocated because you cannot allocate a node-aligned block at the higher level.
    // Because of address tree structure, we don't get an accurate count of the number of frames
    // that were allocated beforehand if we're overwriting some state.
    // e.g. if a node is half allocated and then we allocate the rest of it at a higher level,
    // then there's no way to know how many frames were allocated before without going downwards and reading them.
    // Until something better is implemented, implement a heuristic wherein we only do the optimized case
    // when the current node has no seeChild bits are set for thoese seeChild bits that are valid for this node.
    // Also, we won't get any optimization out of doing this for 64KB, so skip it and avoid wasting time
    // reading state.
    // Since stats only care about free/unpin/pin states,.we could perhaps relax this restriction
    //

    if ((pageSize >= _PMA_2MB) &&
        !_addrtreeNodeIndexHasSeeChildSet(pTree, pTargetNode, targetIndex, &targetFoundState))
    {
        // Do optimized case

        // Figure out how many 64KB frames and how many 2MB frames we're going to touch
        NvU64 numFramesTouched    = pageSize >> PMA_PAGE_SHIFT;
        NvU64 num2mbFramesTouched = pageSize >> _TREE_2MB;

        updatedState = (targetFoundState & ~newStateMask) | (newState & newStateMask);

        _addrtreeSetState(pTree, pTargetNode, targetIndex, newState, newStateMask);

        // In this case, the states at the 2MB and 64KB levels are the same because we're changing at
        // least a 2MB node that had no valid children
        pmaStatsUpdateState(&pTree->pPmaStats->numFreeFrames,
                            numFramesTouched, targetFoundState, updatedState);
        pmaStatsUpdateState(&pTree->pPmaStats->numFree2mbPages,
                            num2mbFramesTouched, targetFoundState, updatedState);

        if (pTree->bProtected)
        {
            pmaStatsUpdateState(&pTree->pPmaStats->numFreeFramesProtected,
                                numFramesTouched, targetFoundState, updatedState);
            pmaStatsUpdateState(&pTree->pPmaStats->numFree2mbPagesProtected,
                                num2mbFramesTouched, targetFoundState, updatedState);
        }
    }
    else
    {
        // Do unoptimized case
        NvU32 framesPerPage = (NvU32)(pageSize >> PMA_PAGE_SHIFT);
        NvU32 j;

        for (j = 0; j < framesPerPage; j++)
        {
            NvU64 frameNum2mb;
            NvU64 frameNum = frameNumStart + j;

            _addrtreeConvertFrame(pTree, levelNum, frameNum, levelNum2mb, &frameNum2mb);
            oldState2mb = pmaAddrtreeReadLevel(pTree, levelNum2mb, frameNum2mb, NV_TRUE);

            _addrtreeConvertLevelFrameToNodeIndex(pTree, levelNum, frameNum, &pNode, &index);

            // The read is done only to update the stats tracking
            oldState = pmaAddrtreeReadLevel(pTree, levelNum, frameNum, NV_TRUE);
             _addrtreeSetState(pTree, pNode, index, newState, newStateMask);

            // Calculate what the new state will be
            updatedState = (oldState & ~newStateMask) | (newState & newStateMask);

            pmaStatsUpdateState(&pTree->pPmaStats->numFreeFrames, 1, oldState, updatedState);

            if (pTree->bProtected)
            {
                pmaStatsUpdateState(&pTree->pPmaStats->numFreeFramesProtected,
                                    1, oldState, updatedState);
            }

            updatedState2mb = pmaAddrtreeReadLevel(pTree, levelNum2mb, frameNum2mb, NV_TRUE);

            if (updatedState2mb != oldState2mb)
            {
                pmaStatsUpdateState(&pTree->pPmaStats->numFree2mbPages, 1,
                                    oldState2mb, updatedState2mb);

                if (pTree->bProtected)
                {
                    pmaStatsUpdateState(&pTree->pPmaStats->numFree2mbPagesProtected, 1,
                                        oldState2mb, updatedState2mb);
                }
            }
        }

    }
}

//
// This function wraps the real __pmaAddrtreeChangePageStateAttribEx
// to allow addrtree to set 128KB page size
//
void
pmaAddrtreeChangePageStateAttribEx
(
    void           *pMap,
    NvU64           frameNumStart,
    NvU64           pageSize,
    PMA_PAGESTATUS  newState,
    PMA_PAGESTATUS  newStateMask
)
{
    if (NV_UNLIKELY(pageSize == _PMA_128KB)) {
        NvU64 i;
        for (i = 0; i < 2; i++) {
            __pmaAddrtreeChangePageStateAttribEx(
                pMap,
                frameNumStart + i,
                _PMA_64KB,
                newState,
                newStateMask);
        }
    }
    else
    {
        __pmaAddrtreeChangePageStateAttribEx(
            pMap,
            frameNumStart,
            pageSize,
            newState,
            newStateMask);
    }
}

//
// These accessor functions can be made more efficient. TODO improve this.
// We have page size information in the alloc path, but in general, we don't use
// _pmaAddrtreeChangePageStateAttribEx to its fullest extent for all other cases where
// we change the state of the tree.
// If we had the page size information, we won't need to walk the tree for every frame.
// For now, pmaAddrtreeChangeState and pmaAddrtreeRead only operate on 64K level frameNums
//
void
pmaAddrtreeChangeStateAttribEx
(
    void           *pMap,
    NvU64           frameNum,
    PMA_PAGESTATUS  newState,
    PMA_PAGESTATUS  newStateMask
)
{
    pmaAddrtreeChangePageStateAttribEx(pMap, frameNum, _PMA_64KB, newState, newStateMask);
}

void
pmaAddrtreeChangeBlockStateAttrib
(
    void *pMap,
    NvU64 frame,
    NvU64 len,
    PMA_PAGESTATUS newState,
    PMA_PAGESTATUS writeMask
)
{
    while (len != 0)
    {
        len--;
        pmaAddrtreeChangeStateAttribEx(pMap, frame + len, newState, writeMask);
    }
}

PMA_PAGESTATUS pmaAddrtreeRead
(
    void  *pMap,
    NvU64  frameNum,
    NvBool bReadAttrib
)
{
    NvU32 index;
    ADDRTREE_NODE *pNode;
    NvU32 levelNum = addrtreeGetTreeLevel(_PMA_64KB) - 1;
    PMA_PAGESTATUS state;
    PMA_ADDRTREE *pTree = (PMA_ADDRTREE *)pMap;
    frameNum += pTree->numPaddingFrames;

    _addrtreeConvertLevelFrameToNodeIndex(pTree, levelNum,
                                  frameNum, &pNode, &index);

    state = _addrtreeGetState(pTree, pNode, index, bReadAttrib);
    return state;
}


void pmaAddrtreeGetSize
(
    void  *pMap,
    NvU64 *pBytesTotal
)
{
    PMA_ADDRTREE *pTree = (PMA_ADDRTREE *)pMap;
    *pBytesTotal = (pTree->totalFrames << PMA_PAGE_SHIFT);
}

//
// The algorithm here is very simplistic. But maybe that's OK because this call
// is not used a whole lot. We can optimize it but might not worth the effort.
//
void pmaAddrtreeGetLargestFree
(
    void  *pMap,
    NvU64 *pLargestFree
)
{
    NvU64 i, length = 0, largestLength = 0;
    PMA_ADDRTREE *pTree = (PMA_ADDRTREE *)pMap;

    for (i = 0; i < pTree->totalFrames; i++)
    {
        if (pmaAddrtreeRead(pTree, i, NV_FALSE) != STATE_FREE)
        {
            largestLength = NV_MAX(length, largestLength);
            length = 0;
        }
        else
        {
            length++;
        }
    }

    largestLength = NV_MAX(length, largestLength);
    *pLargestFree = (largestLength << PMA_PAGE_SHIFT);
}

//
// Check whether the specified frame range is available completely for eviction
//
// Returns:
//  -  -1 if the whole range is evictable
//
//  -   Index of the last unevictable frame number
//
// For now, just do a dumb scan frame by frame
static NvS64
_pmaAddrtreeScanNumaUnevictable
(
    PMA_ADDRTREE *pTree,
    NvU64         frameBegin,
    NvU64         frameEnd
)
{
    NvU64 frame;
    PMA_PAGESTATUS frameStatus;

    for(frame = frameEnd; frame >= frameBegin; frame--)
    {
        frameStatus = pmaAddrtreeRead((void*)pTree, frame, NV_TRUE);
        if (frameStatus != STATE_UNPIN)
        {
            return frame;
        }
    }
    return -1;
}

//
// Determine a contiguous evictable range of size actualSize
//
// Returns:
//  -  NV_ERR_NO_MEMORY if eviction is not possible for this size
//
//  -  NV_OK if there is a valid contiguous evictable range
//     starting and ending at address stored at evictStart and evictEnd
//
//

NV_STATUS pmaAddrtreeScanContiguousNumaEviction
(
    void    *pMap,
    NvU64    addrBase,
    NvLength actualSize,
    NvU64    pageSize,
    NvU64   *evictStart,
    NvU64   *evictEnd
)
{
    NV_STATUS status = NV_ERR_NO_MEMORY;
    PMA_ADDRTREE *pTree = (PMA_ADDRTREE *)pMap;

    NvU64 alignedAddrBase;
    NvU64 frameNum;
    NvU64 endFrame, frameStart;
    NvU64 alignment = pageSize;
    NvU64 frameAlignment, frameAlignmentPadding;
    NvU64 numFrames = actualSize >> PMA_PAGE_SHIFT;
    NvU64 framesToSkip;
    NvU32 level = addrtreeGetTreeLevel(_PMA_64KB) - 1;
    PMA_PAGESTATUS startStatus, endStatus;

    endFrame = pTree->totalFrames - 1;

    if (pTree->totalFrames < numFrames)
        return status;

    // Copied from _pmaAddrtreeContigSearchLoop, pmaRegmapScanContiguous
    // We need to do this one the 64K frame level because addrtree will currently
    // qualify a 2MB node that is half unpin and half free as entirely
    // unpin, which doesn't work in NUMA mode because PMA cannot ask
    // UVM to evict free pages in NUMA mode

    frameAlignment = alignment >> PMA_PAGE_SHIFT;
    alignedAddrBase = NV_ALIGN_UP(addrBase, alignment);
    // May need to modify to work with internal address tree padding
    frameAlignmentPadding = (alignedAddrBase - addrBase) >> PMA_PAGE_SHIFT;
    frameStart = alignUpToMod(0, frameAlignment, frameAlignmentPadding);

    for (frameNum = frameStart; frameNum <= endFrame; )
    {
        NvS64 firstUnevictableFrame;
        NvU64 endFrame = frameNum + numFrames - 1;

        //
        // Read endStatus first so we don't have to waste time traversing the
        // tree again to read startStatus if endStatus is not even usable
        //
        endStatus = _pmaAddrtreeReadLevelAndSkipUnavailable(pTree, level, endFrame, STATE_UNPIN, NV_FALSE, &framesToSkip, NV_FALSE);

        if (framesToSkip > 1) {
            frameNum = NV_ALIGN_UP(endFrame + framesToSkip, frameAlignment);
            NV_ASSERT(frameNum != 0);
            continue;
        }

        startStatus = _pmaAddrtreeReadLevelAndSkipUnavailable(pTree, level, frameNum, STATE_UNPIN, NV_FALSE, &framesToSkip, NV_FALSE);

        if (framesToSkip > 1) {
            frameNum += NV_ALIGN_UP(framesToSkip, frameAlignment);
            NV_ASSERT(frameNum != 0);
            continue;
        }

        // Check against the whole state since we've already ready that in addrtree
        if (endStatus != STATE_UNPIN)
        {
            // end is not available jump from start to after numFrames
            frameNum += numFrames;
            frameNum = alignUpToMod(frameNum, frameAlignment, frameAlignmentPadding);
            continue;
        }

        if (startStatus != STATE_UNPIN)
        {
            // startFrame is unavailable, jump to next aligned frame
            frameNum += frameAlignment;
            continue;
        }

        // First occurrence of 0 in STATE_UNPIN  from frameNum to frameNum + numFrames - 1
        firstUnevictableFrame = _pmaAddrtreeScanNumaUnevictable(pMap, frameNum, frameNum + numFrames - 1);

        if (firstUnevictableFrame == -1)
        {
            NV_PRINTF(LEVEL_INFO, " %s evictable frame = %lld evictstart = %llx evictEnd = %llx\n",
                                    __FUNCTION__, frameNum, addrBase + (frameNum << PMA_PAGE_SHIFT),
                                   (addrBase + (frameNum << PMA_PAGE_SHIFT) + actualSize - 1));

            // Subtract off padding when returning
            *evictStart =   addrBase   + (frameNum << PMA_PAGE_SHIFT) - (pTree->numPaddingFrames << PMA_PAGE_SHIFT);
            *evictEnd   =  *evictStart + actualSize - 1;
            status = NV_OK;
            break;
        }
        else
        {
            // get the next aligned frame after the unevictable frame.
            frameNum = alignUpToMod(firstUnevictableFrame + 1, frameAlignment, frameAlignmentPadding);
        }
    }

    return status;

}

NvU64 pmaAddrtreeGetEvictingFrames(void *pMap)
{
    return ((PMA_ADDRTREE *)pMap)->frameEvictionsInProcess;
}

void pmaAddrtreeSetEvictingFrames(void *pMap, NvU64 frameEvictionsInProcess)
{
    ((PMA_ADDRTREE *)pMap)->frameEvictionsInProcess = frameEvictionsInProcess;
}
