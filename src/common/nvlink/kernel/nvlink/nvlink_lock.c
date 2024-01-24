/*
 * SPDX-FileCopyrightText: Copyright (c) 2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvlink.h"
#include "nvtypes.h"
#include "nvlink_export.h"
#include "nvlink_os.h"
#include "nvlink_ctx.h"
#include "nvlink_helper.h"
#include "nvlink_lock.h"

//
// Only enabling top level locking for linux as required by Bug 4108674.
// Per link locking is still disabled at all times. It will be enabled 
// after other locking related clean up is done.
// 

static void   _sort_links(nvlink_link **, NvU32, NvBool (*)(void *, void *));
static NvBool _compare(void *, void *);

#if defined(NV_LINUX)
#undef TOP_LEVEL_LOCKING_DISABLED 
#   define TOP_LEVEL_LOCKING_DISABLED 0
#endif  /* defined(NV_LINUX) */
/*
 * Allocate top level lock. Return NVL_SUCCESS if 
 * the lock was allocated else return NVL_ERR_GENERIC.
 */
NvlStatus
nvlink_lib_top_lock_alloc(void)
{
    if (TOP_LEVEL_LOCKING_DISABLED)
    {
        return NVL_SUCCESS;
    }

    void *top_lock = NULL;

    // Check if top level lock is already allocated
    if (nvlinkLibCtx.topLevelLock != NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Top-level lock already allocated\n",
            __FUNCTION__));

        return NVL_ERR_GENERIC; 
    }

    top_lock = nvlink_allocLock();
    if (NULL == top_lock)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to allocate top-level lock\n",
            __FUNCTION__));

        return NVL_ERR_GENERIC; 
    }

    nvlinkLibCtx.topLevelLock = top_lock; 

    // Top-level lock allocated

    return NVL_SUCCESS;
}

/*
 * Free top level lock. Return NVL_SUCCESS if 
 * the lock was freed else return NVL_ERR_GENERIC.
 */
NvlStatus
nvlink_lib_top_lock_free(void)
{
    if (TOP_LEVEL_LOCKING_DISABLED)
    {
        return NVL_SUCCESS;
    }

    // Check if already freed 
    if (NULL == nvlinkLibCtx.topLevelLock)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Top-level lock not allocated/already freed\n",
            __FUNCTION__));

        return NVL_ERR_GENERIC; 
    }

    nvlink_freeLock(nvlinkLibCtx.topLevelLock);
    nvlinkLibCtx.topLevelLock = NULL;

    // Top-level lock freed 
 
    return NVL_SUCCESS;
}

/*
 * Allocate per-link lock. Return NVL_SUCCESS if 
 * the lock was allocated else return NVL_ERR_GENERIC.
 */
NvlStatus
nvlink_lib_link_lock_alloc
(
    nvlink_link *link
)
{
    if (PER_LINK_LOCKING_DISABLED)
    {
        return NVL_SUCCESS;
    }

    void *link_lock = NULL;

    // Check if already allocated
    if (link->linkLock != NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Link lock already allocated on this link\n",
            __FUNCTION__));

        return NVL_ERR_GENERIC; 
    }

    link_lock = nvlink_allocLock();
    if (NULL == link_lock)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to allocate link lock\n",
            __FUNCTION__));

        return NVL_ERR_GENERIC; 
    }

    // Link lock allocated
    link->linkLock = link_lock;

    return NVL_SUCCESS;
}

/*
 * Free per-link lock. Return NVL_SUCCESS if 
 * the lock was freed else return NVL_ERR_GENERIC.
 */
NvlStatus
nvlink_lib_link_lock_free
(
    nvlink_link *link
)
{
    if (PER_LINK_LOCKING_DISABLED)
    {
        return NVL_SUCCESS;
    }

    // Check if already freed 
    if (NULL == link->linkLock)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Link lock not allocated/already freed\n",
            __FUNCTION__));

        return NVL_ERR_GENERIC; 
    }

    nvlink_freeLock(link->linkLock);
    link->linkLock = NULL;

    // Link lock freed

    return NVL_SUCCESS;
}

/*
 * Acquire top level lock. Return NVL_SUCCESS if 
 * the lock was acquired else return NVL_ERR_STATE_IN_USE.
 */
NvlStatus
nvlink_lib_top_lock_acquire(void)
{
    if (TOP_LEVEL_LOCKING_DISABLED)
    {
        return NVL_SUCCESS;
    }

    // Check if top-level lock is allocated before attempting to acquire
    if (NULL == nvlinkLibCtx.topLevelLock)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Top-level lock is not allocated\n",
            __FUNCTION__));

        return NVL_ERR_GENERIC; 
    }

    //
    // ToDo: Check if the lock was acquired succesfully
    // Currently the nvlink_acquireLock function doesn't report failures
    //
    nvlink_acquireLock(nvlinkLibCtx.topLevelLock);

    return NVL_SUCCESS;
}

/*
 * Release top level lock. Return NVL_SUCCESS if 
 * the lock was released else return NVL_ERR_GENERIC.
 */
NvlStatus
nvlink_lib_top_lock_release(void)
{
    if (TOP_LEVEL_LOCKING_DISABLED)
    {
        return NVL_SUCCESS;
    }

    // Check if top-level lock is allocated before attempting to release
    if (NULL == nvlinkLibCtx.topLevelLock)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Top-level lock is not allocated\n",
            __FUNCTION__));

        return NVL_ERR_GENERIC; 
    }

    //
    // ToDo: Check if the lock was released succesfully
    // Currently the nvlink_releaseLock function doesn't report failures
    //
    nvlink_releaseLock(nvlinkLibCtx.topLevelLock);

    return NVL_SUCCESS;
}

/*
 * Sort the array of links in order of (DBDF, link#) - 
 * lowest to highest and acquire link locks.
 * Return NVL_SUCCESS if all the link locks were acquired.
 * Else if any link lock failed to be acquired, release 
 * all acquired link locks and return NVL_ERR_STATE_IN_USE.
 */
NvlStatus
nvlink_lib_link_locks_acquire
(
    nvlink_link **links,
    int           numLinks
)
{
    if (PER_LINK_LOCKING_DISABLED)
    {
        return NVL_SUCCESS;
    }

    int i;
    nvlink_link *link_prev  = NULL;

    // Check if array of links is already empty before attempting to release. 
    if ((NULL == links) || (numLinks == 0))
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Could not release the link locks. Link array is empty !\n",
            __FUNCTION__));

        return NVL_ERR_GENERIC; 
    }

    // Sort the link array in increasing order of (DBDF, link#)
    _sort_links(links, numLinks, _compare);

    for (i = 0; i < numLinks; i++)
    {
        //
        // Don't acquire locks on loop back links twice since the current link is
        // the same as the previous one
        //
        if (links[i] != link_prev)
        {
            nvlink_acquireLock(links[i]->linkLock);

            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                "%s: Acquire link lock for dom:%d bus:%d dev:%d fun:%d link:%d\n",
                __FUNCTION__,

            links[i]->dev->pciInfo.domain, links[i]->dev->pciInfo.bus,
            links[i]->dev->pciInfo.device, links[i]->dev->pciInfo.function,
            links[i]->linkNumber));
        }

        link_prev = links[i];
    }

    //
    // ToDo: Check if the lock was acquired succesfully
    // Currently the nvlink_acquireLock function doesn't report failures
    //
    return NVL_SUCCESS;
}

/*
 * Loop over all the links and call nvlink_releaseLock(links[i]->linkLock).
 * Return NVL_SUCCESS if all the link locks were released.
 * Else if any link lock failed to be released return NVL_ERR_GENERIC.
 */
NvlStatus
nvlink_lib_link_locks_release
(
    nvlink_link **links,
    int           numLinks
)
{
    if (PER_LINK_LOCKING_DISABLED)
    {
        return NVL_SUCCESS;
    }

    int i;
    nvlink_link *link_prev  = NULL;

    // Check if array of links is already empty before attempting to release. 
    if ((NULL == links) || (numLinks == 0))
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Could not release the link locks. Link array is empty !\n",
            __FUNCTION__));

        return NVL_ERR_GENERIC; 
    }

    // Sort the link array in increasing order of (DBDF, link#)
    _sort_links(links, numLinks, _compare);

    for (i = 0; i < numLinks; i++)
    {
        //
        // Don't release locks on loop back links twice since the current link is
        // the same as the previous one
        //
        if (links[i] != link_prev)
        {
            nvlink_releaseLock(links[i]->linkLock);

            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                "%s: Release link lock for dom:%d bus:%d dev:%d fun:%d link:%d\n",
                __FUNCTION__,

            links[i]->dev->pciInfo.domain, links[i]->dev->pciInfo.bus,
            links[i]->dev->pciInfo.device, links[i]->dev->pciInfo.function,
            links[i]->linkNumber));
        }

        link_prev = links[i];
    }

    //
    // ToDo: Check if the lock was released succesfully
    // Currently the nvlink_releaseLock function doesn't report failures
    //
    return NVL_SUCCESS;
}

/*
 * Sorts the links in the increasing order of DBDF, link#
 */
static void
_sort_links
(
    nvlink_link **links,
    NvU32         numLinks,
    NvBool      (*compare)(void *, void *)
)
{
    nvlink_link *temp = NULL;
    NvU32        i, j;

    for (i = 0; i < numLinks; i++)
    {
        for (j = i + 1; j < numLinks; j++)
        {
            if (_compare(links[j], links[i]))
            {
                temp     = links[i];
                links[i] = links[j];
                links[j] = temp;
            }
        }
    }
}

/*
 * Compare function for _nvlink_sort - compares DBDF, link#
 */
static NvBool
_compare
(
    void *link1,
    void *link2
)
{
      nvlink_link *l1 = (nvlink_link *) link1;
      nvlink_link *l2 = (nvlink_link *) link2;
  
      // Compare link domains
      if (l1->dev->pciInfo.domain < l2->dev->pciInfo.domain)
      {
          return NV_TRUE;
      }
  
      if (l1->dev->pciInfo.domain > l2->dev->pciInfo.domain)
      {
          return NV_FALSE;
      }
  
      // Domain is same for devices of links. Compare bus next
  
      // Compare link buses 
      if (l1->dev->pciInfo.bus < l2->dev->pciInfo.bus)
      {
          return NV_TRUE;
      }
  
      if (l1->dev->pciInfo.bus > l2->dev->pciInfo.bus)
      {
          return NV_FALSE;
      }
  
      // Bus is same for devices of links. Compare device next
  
      // Compare link devices
      if (l1->dev->pciInfo.device < l2->dev->pciInfo.device)
      {
          return NV_TRUE;
      }
  
      if (l1->dev->pciInfo.device > l2->dev->pciInfo.device)
      {
          return NV_FALSE;
      }
  
      // Device is same for devices of links. Compare function next
  
      // Compare link functions
      if (l1->dev->pciInfo.function < l2->dev->pciInfo.function)
      {
          return NV_TRUE;
      }
  
      if (l1->dev->pciInfo.function > l2->dev->pciInfo.function)
      {
          return NV_FALSE;
      }
  
      // DBDF is same for both the links. Check the link#
  
      // Compare link numbers
      if (l1->linkNumber < l2->linkNumber)
      {
          return NV_TRUE;
      }
      else
      {
          return NV_FALSE;
      }
}
