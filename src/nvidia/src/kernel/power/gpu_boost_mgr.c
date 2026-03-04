/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief Implementation of the Sync Gpu Boost Manager methods.
 */

/*------------------------------Includes--------------------------------------*/
#include "power/gpu_boost_mgr.h"
#include "os/os.h"
#include "core/locks.h"
#include "gpu/gpu_access.h"
#include "gpu/gpu.h"
#include "gpu/power/syncgpuboost.h"
#include "nvlimits.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/gsp/gsp_static_config.h"
#include "vgpu/vgpu_events.h"
#include "gpu/perf/kern_perf_gpuboostsync.h"
#include "nvdevid.h"
#include "platform/sli/sli.h"

/*-----------------------Static Private Method Prototypes---------------------*/
static NV_STATUS _gpuboostmgrApplyPolicyFilters(NV0000_SYNC_GPU_BOOST_GROUP_CONFIG *);

/*----------------------------Object Methods----------------------------------*/

/*!
 * @brief OBJGPUBOOSTMGR object method to create a Sync Gpu Boost Group (SGBG)
 *
 * This method will create a new SGBG if none of the  GPUs specified in the input
 * params is already a part of another SGBG.
 *
 * @param [in]  pBoostConfig                @ref NV0000_SYNC_GPU_BOOST_GROUP_CONFIG
 * @param [out] pBoostConfig::boostGroupId  If SGBG creation succeeds, this field
 *                                          has the assigned group ID. This ID is
 *                                          unique across RM.
 * @returns     NV_OK                             Success
 * @returns     NV_ERR_INSUFFICIENT_RESOURCES     No new groups possible.
 * @returns     NV_ERR_INVALID_STATE              Internal state messed up.
 * and a few more returned from second level functions.
 */
NV_STATUS
gpuboostmgrCreateGroup_IMPL
(
    OBJGPUBOOSTMGR                      *pBoostMgr,
    NV0000_SYNC_GPU_BOOST_GROUP_CONFIG  *pBoostConfig
)
{
    NV_STATUS  status      = NV_OK;
    NODE      *pGpuIdNode  = NULL;
    NvBool     bCleanup    = NV_FALSE;
    NvU32      i;

    // See if we can accomodate one more SGBG
    NV_ASSERT_OR_RETURN(pBoostMgr->groupCount < NV0000_SYNC_GPU_BOOST_MAX_GROUPS,
                          NV_ERR_INSUFFICIENT_RESOURCES);

    // Check if requested config is valid.
    status = gpuboostmgrCheckConfig(pBoostMgr, pBoostConfig);
    if (NV_OK != status)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Invalid Boost Config. Failing Boost Group creation.\n");
        NV_ASSERT_OR_GOTO(NV_OK == status, gpuboostmgrCreateGroup_exit);
    }

    //
    // Allocate a @ref SYNC_GPU_BOOST_GROUP for the requested group at the first
    // available index.
    //
    // Note: A linear search is sufficient for practical purposes currently.
    //
    for (i = 0; i < NV0000_SYNC_GPU_BOOST_MAX_GROUPS; i++)
    {
        // Found an unused index.
        if (0 == pBoostMgr->pBoostGroups[i].gpuCount)
        {
            NvU32 j;

            // Setup the internal state for the new group
            for(j = 0; j < pBoostConfig->gpuCount; j++)
            {
                // Allocate GPU ID node.
                pGpuIdNode = portMemAllocNonPaged(sizeof(*pGpuIdNode));
                if (NULL == pGpuIdNode)
                {
                    bCleanup = NV_TRUE;
                    NV_ASSERT_OR_GOTO((pGpuIdNode != NULL), gpuboostmgrCreateGroup_exit);
                }

                // Add each unique GPU ID in the GPU ID tree
                portMemSet(pGpuIdNode, 0, sizeof(*pGpuIdNode));
                pGpuIdNode->keyStart = pBoostConfig->gpuIds[j];
                pGpuIdNode->keyEnd   = pBoostConfig->gpuIds[j];
                status               = btreeInsert(pGpuIdNode, &pBoostMgr->pGpuIdTree);
                if (NV_OK != status)
                {
                    bCleanup = NV_TRUE;
                    NV_ASSERT_OR_GOTO(NV_OK == status, gpuboostmgrCreateGroup_exit);
                }

                pBoostMgr->pBoostGroups[i].gpuIds[j] = pBoostConfig->gpuIds[j];
            }

            pBoostMgr->groupCount++;
            pBoostMgr->pBoostGroups[i].gpuCount    = pBoostConfig->gpuCount;
            pBoostMgr->pBoostGroups[i].bBridgeless = pBoostConfig->bBridgeless;

            // Set the out param
            pBoostConfig->boostGroupId = i;

            // We are done.
            break;
        }
    }

    // Since we've come so far, there can't be 0 unused entries
    if (i == NV0000_SYNC_GPU_BOOST_MAX_GROUPS)
    {
        NV_PRINTF(LEVEL_ERROR, "Inconsistency in pBoostGroups state.\n");
        status = NV_ERR_INVALID_STATE;
        NV_ASSERT_OR_GOTO(0, gpuboostmgrCreateGroup_exit);
    }

gpuboostmgrCreateGroup_exit:
    if (bCleanup)
    {
        NvU32 k;

        // Clean up stray state in case of failure
        for(k = 0; k < pBoostConfig->gpuCount; k++)
        {
            btreeSearch(pBoostConfig->gpuIds[k],
                        &pGpuIdNode,
                        pBoostMgr->pGpuIdTree);
            if (NULL != pGpuIdNode)
            {
                btreeUnlink(pGpuIdNode, &pBoostMgr->pGpuIdTree);
                portMemFree(pGpuIdNode);
            }
        }

        // Destroy @ref SYNC_GPU_BOOST_GROUP object
        portMemSet(&(pBoostMgr->pBoostGroups[i]), 0, sizeof(SYNC_GPU_BOOST_GROUP));
    }

    return status;
}

/*!
 * @brief OBJGPUBOOSTMGR object method to destroy a Sync Gpu Boost Group (SGBG)
 *
 * This method will destroy an existing SGBG
 *
 * @param [in]  boostGroupId       Unique ID of the SGBG to be destroyed

 * @returns     NV_OK                     Success
 * @returns     NV_ERR_ILLEGAL_ACTION     No SGBGs to destroy in the first place
 *                                        Or the requested SGBG is already destroyed.
 *  and a few more returned from second level functions.
 */
NV_STATUS
gpuboostmgrDestroyGroup_IMPL
(
    OBJGPUBOOSTMGR  *pBoostMgr,
    NvU32            boostGroupId
)
{
    NV_STATUS  status = NV_OK;
    NODE      *pGpuIdNode = NULL;
    NvU32      i;

    // Can't try to destroy a non-existing group
    NV_ASSERT_OR_RETURN(pBoostMgr->groupCount > 0, NV_ERR_ILLEGAL_ACTION);
    NV_ASSERT_OR_RETURN(NV0000_SYNC_GPU_BOOST_MAX_GROUPS > boostGroupId, NV_ERR_OUT_OF_RANGE);
    NV_ASSERT_OR_RETURN(0 != pBoostMgr->pBoostGroups[boostGroupId].gpuCount, NV_ERR_ILLEGAL_ACTION);

    // Remove each GPU ID from the ID tree before destorying the group.
    for(i = 0; i < pBoostMgr->pBoostGroups[boostGroupId].gpuCount; i++)
    {
        status = btreeSearch(pBoostMgr->pBoostGroups[boostGroupId].gpuIds[i],
                             &pGpuIdNode,
                             pBoostMgr->pGpuIdTree);
        NV_ASSERT_OR_RETURN(((NV_OK == status) && (NULL != pGpuIdNode)), status);
        btreeUnlink(pGpuIdNode, &pBoostMgr->pGpuIdTree);
        portMemFree(pGpuIdNode);
    }

    // Destroy @ref SYNC_GPU_BOOST_GROUP object
    portMemSet(&(pBoostMgr->pBoostGroups[boostGroupId]), 0, sizeof(SYNC_GPU_BOOST_GROUP));

    // Decrement groupCount
    pBoostMgr->groupCount--;

    return NV_OK;
}

/*!
 * @brief Returns information about each Sync Gpu Boost Group defined in the system
 */
NV_STATUS
gpuboostmgrQueryGroups_IMPL
(
    OBJGPUBOOSTMGR                          *pBoostMgr,
    NV0000_SYNC_GPU_BOOST_GROUP_INFO_PARAMS *pParams
)
{
    NvU32     i;
    NvU32     j;

    j = 0;
    for (i = 0; i < NV0000_SYNC_GPU_BOOST_MAX_GROUPS; i++)
    {
        if (NV_OK == gpuboostmgrValidateGroupId(pBoostMgr, i))
        {
            pParams->pBoostGroups[j].gpuCount     = pBoostMgr->pBoostGroups[i].gpuCount;
            pParams->pBoostGroups[j].boostGroupId = i;
            portMemCopy(pParams->pBoostGroups[j].gpuIds,
                        sizeof(pParams->pBoostGroups[j].gpuIds),
                        pBoostMgr->pBoostGroups[i].gpuIds,
                        sizeof(pParams->pBoostGroups[j].gpuIds));
            j++;
        }
    }

    pParams->groupCount = j;

    return NV_OK;
}

/*!
 * @brief OBJGPUBOOSTMGR object method to validate a config for a Sync Gpu Boost Group (SGBG)
 *        @ref NV0000_SYNC_GPU_BOOST_GROUP_CONFIG
 *
 * The following checks needs to be met for a config to be converted to an SGBG
 *   1. There is room for a new SGBG to be tracked in RM.
 *   2. Valid GPU IDs are specified
 *   3. None of the GPUs specified is already a part of an existing SGBG
 *
 * @param [in]  boostConfig                @ref NV0000_SYNC_GPU_BOOST_GROUP_CONFIG
 *
 * @returns     NV_OK                             Success
 * @returns     other values                      Config specified cannot be accepted.
 */
NV_STATUS
gpuboostmgrCheckConfig_IMPL
(
    OBJGPUBOOSTMGR                     *pBoostMgr,
    NV0000_SYNC_GPU_BOOST_GROUP_CONFIG *pBoostConfig
)
{
    NV_STATUS  status = NV_OK;
    OBJGPU    *pGpu   = NULL;
    NODE      *pNode  = NULL;
    NvU32      i;

    NV_ASSERT_OR_RETURN(NULL != pBoostConfig, NV_ERR_INVALID_ARGUMENT);

    if (0 == pBoostConfig->gpuCount ||
        NV_MAX_DEVICES < pBoostConfig->gpuCount)
    {
        status = NV_ERR_OUT_OF_RANGE;
        NV_PRINTF(LEVEL_ERROR, "Invalid Gpu Count 0x%x\n", pBoostConfig->gpuCount);
        DBG_BREAKPOINT();
        goto gpuboostmgrCheckConfig_exit;
    }

    // Policy filters will specify if we can support a given config.
    status = _gpuboostmgrApplyPolicyFilters(pBoostConfig);
    NV_ASSERT_OR_GOTO(NV_OK == status, gpuboostmgrCheckConfig_exit);

    for (i = 0; i < pBoostConfig->gpuCount; i++)
    {
        // Check for invalid GPU ID
        if (NV0000_CTRL_GPU_INVALID_ID == pBoostConfig->gpuIds[i])
        {
            status = NV_ERR_INVALID_ARGUMENT;
            NV_PRINTF(LEVEL_ERROR,
                      "Invalid GPU ID 0x%x at index 0x%x\n",
                      pBoostConfig->gpuIds[i], i);
            DBG_BREAKPOINT();
            goto gpuboostmgrCheckConfig_exit;
        }

        //
        // Check for OBJGPU being available.
        //
        // We do not need to take a GPU lock here, as currently, the API lock
        // guarantees that OBJ* will not be destroyed while the API
        // lock is being held. If at all the check needs to read/write OBJGPU state,
        // we will need the GPU lock.
        //
        // We expect that going ahead, client locks will provide similar guarantees
        // on the GPU state.
        //
        pGpu = gpumgrGetGpuFromId(pBoostConfig->gpuIds[i]);
        if (NULL == pGpu)
        {
            status = NV_ERR_INVALID_ARGUMENT;
            NV_PRINTF(LEVEL_ERROR,
                      "OBJGPU not constructed yet for ID 0x%x at index 0x%x\n",
                      pBoostConfig->gpuIds[i], i);
            DBG_BREAKPOINT();
            goto gpuboostmgrCheckConfig_exit;
        }

        // A GPU cannot be in more than one Boost Group
        if (NV_OK == btreeSearch(pBoostConfig->gpuIds[i], &pNode, pBoostMgr->pGpuIdTree))
        {
            status = NV_ERR_INVALID_ARGUMENT;
            NV_PRINTF(LEVEL_ERROR,
                      "GPU with ID 0x%x already in use in another group\n",
                      pBoostConfig->gpuIds[i]);
            DBG_BREAKPOINT();
            goto gpuboostmgrCheckConfig_exit;
        }
    }

gpuboostmgrCheckConfig_exit:
    return status;
}

/*!
 * @brief Constructor
 */
NV_STATUS
gpuboostmgrConstruct_IMPL(OBJGPUBOOSTMGR *pBoostMgr)
{
    return NV_OK;
}

/*!
 * @brief Destructor
 */
void
gpuboostmgrDestruct_IMPL(OBJGPUBOOSTMGR *pBoostMgr)
{
    btreeDestroyNodes(pBoostMgr->pGpuIdTree);
    portMemSet(pBoostMgr->pBoostGroups, 0, NV0000_SYNC_GPU_BOOST_MAX_GROUPS * sizeof(SYNC_GPU_BOOST_GROUP));
    pBoostMgr->groupCount = 0;
}


/*!
 * @brief Checks if the boost group ID belongs to a valid Sync Gpu Boost Group
 *
 * @param [in] boostGroupId    ID to be checked
 *
 * @returns    NV_OK                 group ID belongs to a valid Group.
 * @returns    NV_ERR_INVALID_INDEX  group ID is not used by  any SGBG
 */
NV_STATUS
gpuboostmgrValidateGroupId_IMPL
(
    OBJGPUBOOSTMGR *pBoostMgr,
    NvU32           boostGroupId
)
{
    if (NV0000_SYNC_GPU_BOOST_MAX_GROUPS <= boostGroupId)
        return NV_ERR_INVALID_INDEX;

    // If group count is 0, the group index points to an invalid/empty boost group
    if (0 == pBoostMgr->pBoostGroups[boostGroupId].gpuCount)
        return NV_ERR_INVALID_INDEX;

    return NV_OK;
}

/*!
 * @brief       Increments the ref count for a Sync Gpu Boost Group
 *
 * @param [in]  boostGroupId   ID of the SGBG whose ref count needs incrementing.
 *
 * @returns     NV_OK          Ref count incremented successfully
 *
 */
NV_STATUS
gpuboostmgrIncrementRefCount_IMPL
(
    OBJGPUBOOSTMGR *pBoostMgr,
    NvU32           boostGroupId
)
{
    NV_STATUS status = NV_OK;

    status = gpuboostmgrValidateGroupId(pBoostMgr, boostGroupId);
    if (NV_OK != status)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid group ID 0x%x\n", boostGroupId);
        NV_ASSERT_OR_RETURN(0, status);
    }

    if (NV_U32_MAX == pBoostMgr->pBoostGroups[boostGroupId].refCount)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Max limit reached for ref count on group 0x%x\n",
                  boostGroupId);
        NV_ASSERT_OR_RETURN(0, NV_ERR_INSUFFICIENT_RESOURCES);
    }

    //
    // Increment the ref count on the SGBG in @ref OJGPUBOOSTMGR
    // Trigger state change for the SGB Algorithm if this is the first client
    // referencing the group
    //
    pBoostMgr->pBoostGroups[boostGroupId].refCount++;
    if (1 == pBoostMgr->pBoostGroups[boostGroupId].refCount)
    {
        status = kperfGpuBoostSyncStateUpdate(pBoostMgr, boostGroupId, NV_TRUE);
        if (NV_OK != status)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Could not activate Sync GPU Boost on group 0x%x. Status: 0x%08x\n",
                      boostGroupId, status);
            pBoostMgr->pBoostGroups[boostGroupId].refCount--;
            NV_ASSERT(0);
        }
    }

    return status;
}

/*!
 * @brief       Decrements the ref count for a Sync Gpu Boost
 * Group
 *
 * @param [in]  boostGroupId   ID of the SGBG whose ref count needs decrementing.
 *
 * @returns     NV_OK          Ref count decremented successfully
 *
 */
NV_STATUS
gpuboostmgrDecrementRefCount_IMPL
(
    OBJGPUBOOSTMGR *pBoostMgr,
    NvU32           boostGroupId
)
{
    NV_STATUS status = NV_OK;

    status = gpuboostmgrValidateGroupId(pBoostMgr, boostGroupId);
    if (NV_OK != status)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid group ID 0x%x\n", boostGroupId);
        NV_ASSERT_OR_RETURN(0, status);
    }

    if (0 == pBoostMgr->pBoostGroups[boostGroupId].refCount)
    {
        NV_PRINTF(LEVEL_ERROR, "Ref count on group 0x%x is already 0\n",
                  boostGroupId);
        NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_REQUEST);
    }

    //
    // Decrement the ref count on the SGBG in @ref OJGPUBOOSTMGR
    // Trigger state change for the SGB Algorithm if this was the last client
    // referencing the group
    //
    pBoostMgr->pBoostGroups[boostGroupId].refCount--;
    if (0 == pBoostMgr->pBoostGroups[boostGroupId].refCount)
    {
        status = kperfGpuBoostSyncStateUpdate(pBoostMgr, boostGroupId, NV_FALSE);
        if (NV_OK != status)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Could not deactivate Sync GPU Boost on group 0x%x. Status: 0x%08x\n",
                      boostGroupId, status);
            NV_ASSERT(0);
        }
    }

    return status;
}

/*!
 * This iterator function return the GPU at given index from the given SGBG
 *
 * @params [in] pBoostGrp   Pointer to the SGBG to iterate over
 * @params [in] grpId       ID of the Boost Group over which to iterate.
 *                          ID corresponds to index in @ref OBJGPUBOOSTMGR::pBoostGroups
 * @params [in] pIndex      0 based index into the pBoostGrp, at which the
 *                          iteration commences.
 *
 * @returns  OBJGPU* if a GPU object is found.
 *           NULL otherwise
 *
 * Note: Always pass in an initial index of 0, when beginning the iteration. This
 *       method lends itself to a while() construct  like so -
 *       while (NULL != (pGpu = ItrMethod(pBoostGrp, &index)){ //foo };
 */
POBJGPU
gpuboostmgrGpuItr_IMPL
(
    OBJGPUBOOSTMGR       *pBoostMgr,
    NvU32                 grpId,
    NvU32                *pIndex
)
{
    NV_ASSERT_OR_RETURN(NULL != pIndex, NULL);

    if (0 == pBoostMgr->pBoostGroups[grpId].gpuCount)
    {
        NV_PRINTF(LEVEL_ERROR, "Gpu Count is 0 for group ID: 0x%x\n", grpId);
        return NULL;
    }
    if (*pIndex == pBoostMgr->pBoostGroups[grpId].gpuCount)
    {
        return NULL;
    }

    return gpumgrGetGpuFromId(pBoostMgr->pBoostGroups[grpId].gpuIds[(*pIndex)++]);
}

/*!
 * @brief Retrieves the ID of the SGBG to which a GPU belongs
 *
 * @param [in]  pBoostMgr
 * @param [in]  pGpu
 * @param [out] pBoostGrpId   ID of the @ref SYNC_GPU_BOOST_GROUP to which the
 *                             given GPU belongs.
 *                             NV0000_SYNC_GPU_BOOST_INVALID_GROUP_ID value if
 *                             group is not found
 *
 * @return NV_OK                   if SGBG is found
 * @return NV_ERR_OBJECT_NOT_FOUND if SGBG is not found
 * @return  NV_ERR_INVALID_ARGUMENT Null or incorrect arguments passed in.
 */
NV_STATUS
gpuboostmgrGetBoostGrpIdFromGpu_IMPL
(
    OBJGPUBOOSTMGR *pBoostMgr,
    OBJGPU         *pGpu,
    NvU32          *pBoostGrpId
)
{
    NvU32   i;
    NvU32   index = 0;
    OBJGPU *pGpuTemp = NULL;

    *pBoostGrpId = NV0000_SYNC_GPU_BOOST_INVALID_GROUP_ID;

    NV_ASSERT_OR_RETURN(NULL != pGpu, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NULL != pBoostGrpId, NV_ERR_INVALID_ARGUMENT);

    for (i = 0; i < pBoostMgr->groupCount; i++)
    {
        while (NULL != (pGpuTemp = gpuboostmgrGpuItr(pBoostMgr, i, &index)))
        {
            if (pGpuTemp->gpuId == pGpu->gpuId)
            {
                *pBoostGrpId = i;
                return NV_OK;
            }
        }
    }

    NV_PRINTF(LEVEL_INFO,
              "No Boost Group found for the gpu with ID: 0x%08x\n",
              pGpu->gpuId);

    return NV_ERR_OBJECT_NOT_FOUND;
}

/*!
 * @return NV_TRUE  If the Sync GPU Boost Group at the given ID has the algorithm
 *                  active.
 *         NV_FALSE If the group ID is invalid or the algorithm is inactive.
 */
NvBool
gpuboostmgrIsBoostGrpActive_IMPL
(
    OBJGPUBOOSTMGR  *pBoostMgr,
    NvU32            grpId
)
{
    if (grpId == NV0000_SYNC_GPU_BOOST_INVALID_GROUP_ID)
    {
        return NV_FALSE;
    }

    return ((0 != pBoostMgr->pBoostGroups[grpId].gpuCount) &&
            (0 != pBoostMgr->pBoostGroups[grpId].refCount));
}

/*------------------------------Static Private Methods------------------------*/

/*!
 * Applies filter policies which determine whether or not to allow the given GPUs
 * to be in the same SGBG.
 */
static NV_STATUS
_gpuboostmgrApplyPolicyFilters(NV0000_SYNC_GPU_BOOST_GROUP_CONFIG *pBoostConfig)
{
    NV_STATUS  status           = NV_ERR_NOT_COMPATIBLE;
    OBJGPU    *pGpu             = NULL;
    OBJGPU    *pGpuItr          = NULL;
    OBJGPUGRP *pGpuGrp          = NULL;
    NvBool     bIsSli           = NV_TRUE;
    NvBool     bIsUnlinkedSli   = NV_TRUE;
    NvBool     bMatchingDevId   = NV_TRUE;
    NvU32      i;

    NV_ASSERT_OR_RETURN(NULL != pBoostConfig, NV_ERR_INVALID_ARGUMENT);

    pGpu = gpumgrGetGpuFromId(pBoostConfig->gpuIds[0]);
    NV_ASSERT_OR_RETURN(NULL != pGpu, NV_ERR_OBJECT_NOT_FOUND);
    pGpuGrp = gpumgrGetGpuGrpFromGpu(pGpu);

    //
    // Group Filter Policy:
    // If GPUs are in same SLI device - Allow
    // else if GPUs have the same (devid, subsystem id, subvendor id, board proj id, board sku id) - Allow
    // else - Disallow
    //

    //
    // Check if all GPUs are in same SLI Device.
    // We compare @ref OBJGPUGRP for each GPU as opposed to PBJGPU:deviceInstance
    // because in the future we may move away from the SLI Device Model.
    //
    for (i = 1; i < pBoostConfig->gpuCount; i++)
    {
        pGpuItr = gpumgrGetGpuFromId(pBoostConfig->gpuIds[i]);
        NV_ASSERT_OR_RETURN(NULL != pGpuItr, NV_ERR_OBJECT_NOT_FOUND);

        if (pGpuGrp == gpumgrGetGpuGrpFromGpu(pGpuItr))
        {
            continue;
        }
        else
        {
            bIsSli = NV_FALSE;
            break;
        }
    }
    if (bIsSli)
    {
        status = NV_OK;
        goto _gpuboostmgrApplyPolicyFilters_exit;
    }

    //
    // Check if Unlinked SLI is enabled for all GPUs within the group
    //
    for (i = 0; i < pBoostConfig->gpuCount; i++)
    {
        pGpuItr = gpumgrGetGpuFromId(pBoostConfig->gpuIds[i]);
        NV_ASSERT_OR_RETURN(NULL != pGpuItr, NV_ERR_OBJECT_NOT_FOUND);

        if (IsUnlinkedSLIEnabled(pGpuItr))
        {
            continue;
        }
        else
        {
            bIsUnlinkedSli = NV_FALSE;
            break;
        }
    }
    if (bIsUnlinkedSli)
    {
        status = NV_OK;
        goto _gpuboostmgrApplyPolicyFilters_exit;
    }

    //
    // Check if all the GPUs have same dev id. This needs to be ensured for the Sync
    // Boost algorithm to provide the expected benefits, unless otherwise specified.
    //
    NvU32 pciDevId = 0;
    NvU64 boardProjNum = 0;
    NvU64 boardSkuNum  = 0;
    NvU16 subVendor    = 0;
    NvU16 subDevice    = 0;
    GspStaticConfigInfo *pGSCI = NULL;

    pciDevId = DRF_VAL(_PCI, _DEVID, _DEVICE, pGpu->idInfo.PCIDeviceID);

    // Cache all necessary values for one of the GPUs
    {
        if ( IS_GSP_CLIENT(pGpu) )
        {
            pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);
        }

        if ( !IS_GSP_CLIENT(pGpu)  || !pGSCI->bVbiosValid )
        {
            status = NV_ERR_NOT_SUPPORTED;
            NV_ASSERT_OR_GOTO(NV_FALSE, _gpuboostmgrApplyPolicyFilters_exit);
        }

        portMemCopy(&boardSkuNum, sizeof(boardSkuNum), pGSCI->SKUInfo.projectSKU, sizeof(pGSCI->SKUInfo.projectSKU));
        portMemCopy(&boardProjNum, sizeof(boardProjNum), pGSCI->SKUInfo.project, sizeof(pGSCI->SKUInfo.project));
        subVendor = pGSCI->vbiosSubVendor;
        subDevice = pGSCI->vbiosSubDevice;
    }

    // Compare each GPU's values with the cached values.
    for (i = 1; i < pBoostConfig->gpuCount; i++)
    {
        NvU64 boardProjNumItr = 0;
        NvU64 boardSkuNumItr  = 0;
        NvU16 subVendorItr    = 0;
        NvU16 subDeviceItr    = 0;

        pGpuItr = gpumgrGetGpuFromId(pBoostConfig->gpuIds[i]);
        NV_ASSERT_OR_RETURN(NULL != pGpuItr, NV_ERR_OBJECT_NOT_FOUND);

        // Extract values for the GPU in the iteration.
        {
            if ( IS_GSP_CLIENT(pGpuItr) )
            {
                pGSCI = GPU_GET_GSP_STATIC_INFO(pGpuItr);
            }

            if ( !IS_GSP_CLIENT(pGpuItr)  || !pGSCI->bVbiosValid )
            {
                status = NV_ERR_OBJECT_NOT_FOUND;
                bMatchingDevId = NV_FALSE;
                NV_ASSERT_OR_GOTO(NV_FALSE, _gpuboostmgrApplyPolicyFilters_exit);
            }
            portMemCopy(&boardSkuNumItr, sizeof(boardSkuNumItr), pGSCI->SKUInfo.projectSKU, sizeof(pGSCI->SKUInfo.projectSKU));
            portMemCopy(&boardProjNumItr, sizeof(boardProjNumItr), pGSCI->SKUInfo.project, sizeof(pGSCI->SKUInfo.project));
            subVendorItr = pGSCI->vbiosSubVendor;
            subDeviceItr = pGSCI->vbiosSubDevice;
        }

        // Go to the next GPU if all values match
        if ((pciDevId     == DRF_VAL(_PCI, _DEVID, _DEVICE, pGpuItr->idInfo.PCIDeviceID)) &&
            (boardSkuNum  == boardSkuNumItr)  &&
            (boardProjNum == boardProjNumItr) &&
            (subVendor    == subVendorItr)    &&
            (subDevice    == subDeviceItr))
        {
            continue;
        }
        else
        {
            // At least one mismatch.
            bMatchingDevId = NV_FALSE;
            break;
        }
    }
    if (bMatchingDevId)
    {
        status = NV_OK;
        goto _gpuboostmgrApplyPolicyFilters_exit;
    }

_gpuboostmgrApplyPolicyFilters_exit:
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "GPUs not compatible to be put in the same group\n");
        DBG_BREAKPOINT();
    }

    return status;
}

NV_STATUS
syncgpuboostConstruct_IMPL
(
    SyncGpuBoost    *pSyncGpuBoost,
    CALL_CONTEXT    *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    OBJSYS                  *pSys       = SYS_GET_INSTANCE();
    OBJGPUBOOSTMGR          *pBoostMgr  = SYS_GET_GPUBOOSTMGR(pSys);
    NV_STATUS                rmStatus   = NV_OK;
    NV0060_ALLOC_PARAMETERS *p0060Params = pParams->pAllocParams;

    NV_ASSERT_OR_RETURN(NULL != p0060Params, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OK_OR_RETURN(gpuboostmgrIncrementRefCount(pBoostMgr, p0060Params->gpuBoostGroupId));

    pSyncGpuBoost->gpuBoostGroupId = p0060Params->gpuBoostGroupId;

    return rmStatus;
}

void
syncgpuboostDestruct_IMPL
(
    SyncGpuBoost *pSyncGpuBoost
)
{
    RS_RES_FREE_PARAMS_INTERNAL *pParams = NULL;
    OBJSYS          *pSys       = SYS_GET_INSTANCE();
    OBJGPUBOOSTMGR  *pBoostMgr  = SYS_GET_GPUBOOSTMGR(pSys);
    NV_STATUS        rmStatus   = NV_OK;

    resGetFreeParams(staticCast(pSyncGpuBoost, RsResource), NULL, &pParams);

    // Can't do much if ref count decrement failed. Assert and continue deletion.
    rmStatus = gpuboostmgrDecrementRefCount(pBoostMgr, pSyncGpuBoost->gpuBoostGroupId);
    NV_ASSERT(NV_OK == rmStatus);

    pParams->status = rmStatus;

    return;
}
