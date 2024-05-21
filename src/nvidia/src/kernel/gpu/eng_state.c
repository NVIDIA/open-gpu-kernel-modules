/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/gpu.h"
#include "gpu/eng_state.h"
#include "core/hal.h"
#include "core/locks.h"

#include "gpu/bus/kern_bus.h"


NV_STATUS
engstateConstructBase_IMPL
(
    OBJENGSTATE        *pEngstate,
    OBJGPU             *pGpu,
    ENGDESCRIPTOR       engDesc
)
{
    pEngstate->pGpu         = pGpu;
    pEngstate->engDesc      = engDesc;
    pEngstate->currentState = ENGSTATE_STATE_UNDEFINED;

    if (pEngstate->getProperty(pEngstate, PDB_PROP_ENGSTATE_IS_MISSING))
        return NV_ERR_NOT_SUPPORTED;

#if NV_PRINTF_STRINGS_ALLOWED
    nvDbgSnprintf(pEngstate->name, sizeof(pEngstate->name), "%s:%d",
        objGetClassName(pEngstate), ENGDESC_FIELD(pEngstate->engDesc, _INST));
#endif

    return NV_OK;
}

void
engstateLogStateTransitionPre_IMPL
(
    OBJENGSTATE   *pEngstate,
    ENGSTATE_STATE targetState,
    ENGSTATE_TRANSITION_DATA *pData
)
{
    ENGSTATE_STATS *stats = &pEngstate->stats[targetState];
    NV_ASSERT_OR_RETURN_VOID(targetState < ENGSTATE_STATE_COUNT);

    // First call, init
    portMemSet(stats, 0, sizeof(ENGSTATE_STATS));
    portMemSet(pData, 0, sizeof(ENGSTATE_TRANSITION_DATA));
    osGetPerformanceCounter(&pData->transitionStartTimeNs);

#if PORT_IS_FUNC_SUPPORTED(portMemExTrackingGetActiveStats)
    {
        PORT_MEM_TRACK_ALLOCATOR_STATS memstats = {0};
        portMemExTrackingGetActiveStats(NULL, &memstats);

        pData->memoryAllocCount = (NvS64) memstats.numAllocations;
        pData->memoryAllocSize  = (NvS64) memstats.usefulSize;
    }
#endif
}

void
engstateLogStateTransitionPost_IMPL
(
    OBJENGSTATE   *pEngstate,
    ENGSTATE_STATE targetState,
    ENGSTATE_TRANSITION_DATA *pData
)
{
    ENGSTATE_STATS *stats = &pEngstate->stats[targetState];
    NvU64 endTimeNs;

    NV_ASSERT_OR_RETURN_VOID(targetState < ENGSTATE_STATE_COUNT);

    osGetPerformanceCounter(&endTimeNs);
    stats->transitionTimeUs = (endTimeNs - pData->transitionStartTimeNs) / 1000;

#if NV_PRINTF_STRINGS_ALLOWED
    const char *stateStrings[ENGSTATE_STATE_COUNT] =
    {
        "Undefined",
        "Construct",
        "Pre-Init",
        "Init",
        "Pre-Load",
        "Load",
        "Post-Load",
        "Pre-Unload",
        "Unload",
        "Post-Unload",
        "Destroy"
    };
    ct_assert(ENGSTATE_STATE_COUNT == 11);

    NV_PRINTF(LEVEL_INFO,
        "Engine %s state change: %s -> %s, took %uus\n",
        engstateGetName(pEngstate),
        stateStrings[pEngstate->currentState], stateStrings[targetState],
        stats->transitionTimeUs);
#else
    NV_PRINTF(LEVEL_INFO,
        "Engine 0x%06x:%d state change: %d -> %d, took %uus\n",
        objGetClassId(pEngstate), ENGDESC_FIELD(pEngstate->engDesc, _INST),
        pEngstate->currentState, targetState,
        stats->transitionTimeUs);
#endif

#if PORT_IS_FUNC_SUPPORTED(portMemExTrackingGetActiveStats)
    {
        PORT_MEM_TRACK_ALLOCATOR_STATS memstats = {0};
        portMemExTrackingGetActiveStats(NULL, &memstats);

        stats->memoryAllocCount = (NvS32)((NvS64)memstats.numAllocations - pData->memoryAllocCount);
        stats->memoryAllocSize  = (NvS32)((NvS64)memstats.usefulSize     - pData->memoryAllocSize);

        NV_PRINTF(LEVEL_INFO, "    Memory usage change: %d allocations, %d bytes\n",
            stats->memoryAllocCount, stats->memoryAllocSize);
    }
#endif

    pEngstate->currentState = targetState;
}

const char *
engstateGetName_IMPL
(
    OBJENGSTATE *pEngstate
)
{
#if NV_PRINTF_STRINGS_ALLOWED
    return pEngstate->name;
#else
    return "";
#endif
}

/*!
 * @brief generic constructor
 */
NV_STATUS
engstateConstructEngine_IMPL
(
    OBJGPU       *pGpu,
    OBJENGSTATE  *pEngstate,
    ENGDESCRIPTOR engDesc
)
{
    return NV_OK;
}

/*!
 * @brief destructor
 */
void
engstateDestruct_IMPL
(
    OBJENGSTATE *pEngstate
)
{
}

/*!
 * @brief init missing engine
 */
void
engstateInitMissing_IMPL
(
    OBJGPU      *pGpu,
    OBJENGSTATE *pEngstate
)
{
    return;
}

/*!
 * @brief Wrapper around StatePreInitUnlocked and StatePreInitLocked
 */
NV_STATUS
engstateStatePreInit_IMPL(OBJGPU *pGpu, OBJENGSTATE *pEngstate)
{
    LOCK_ASSERT_AND_RETURN(rmGpuLockIsOwner());

    /* Check if we overrode the unlocked variant */
    if ((engstateStatePreInitUnlocked_FNPTR(pEngstate)      !=
         engstateStatePreInitUnlocked_IMPL))
    {
        NV_STATUS status, lockStatus;

        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);

        status = engstateStatePreInitUnlocked(pGpu, pEngstate);

        lockStatus = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE,
                                       RM_LOCK_MODULES_INIT);

        if (status == NV_OK)
            status = lockStatus;
        if (status != NV_OK)
            return status;
    }

    return engstateStatePreInitLocked(pGpu, pEngstate);
}

/*!
 * @brief state pre-init locked
 */
NV_STATUS
engstateStatePreInitLocked_IMPL(OBJGPU *pGpu, OBJENGSTATE *pEngstate)
{
    return NV_OK;
}

/*!
 * @brief state pre-init unlocked
 */
NV_STATUS
engstateStatePreInitUnlocked_IMPL(OBJGPU *pGpu, OBJENGSTATE *pEngstate)
{
    return NV_OK;
}

/*!
 * @brief Wrapper around StateInitUnlocked and StateInitLocked
 */
NV_STATUS
engstateStateInit_IMPL(OBJGPU *pGpu, OBJENGSTATE *pEngstate)
{
    LOCK_ASSERT_AND_RETURN(rmGpuLockIsOwner());

    /* Check if we overrode the unlocked variant */
    if (engstateStateInitUnlocked_FNPTR(pEngstate) != engstateStateInitUnlocked_IMPL)
    {
        NV_STATUS status, lockStatus;

        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);

        status = engstateStateInitUnlocked(pGpu, pEngstate);
        lockStatus = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE,
                                       RM_LOCK_MODULES_INIT);

        if (status == NV_OK)
            status = lockStatus;
        if (status != NV_OK)
            return status;
    }

    return engstateStateInitLocked(pGpu, pEngstate);
}

/*!
 * @brief state init locked
 */
NV_STATUS
engstateStateInitLocked_IMPL(OBJGPU *pGpu, OBJENGSTATE *pEngstate)
{
    return NV_OK;
}

/*!
 * @brief state init unlocked
 */
NV_STATUS
engstateStateInitUnlocked_IMPL(OBJGPU *pGpu, OBJENGSTATE *pEngstate)
{
    return NV_OK;
}

/*!
 * @brief state pre-load
 */
NV_STATUS
engstateStatePreLoad_IMPL
(
    OBJGPU  *pGpu,
    OBJENGSTATE *pEngstate,
    NvU32 flags
)
{
    return NV_OK;
}

/*!
 * @brief state load
 */
NV_STATUS
engstateStateLoad_IMPL
(
    OBJGPU  *pGpu,
    OBJENGSTATE *pEngstate,
    NvU32 flags
)
{
    return NV_OK;
}

/*!
 * @brief state post-load
 */
NV_STATUS
engstateStatePostLoad_IMPL
(
    OBJGPU  *pGpu,
    OBJENGSTATE *pEngstate,
    NvU32 flags
)
{
    return NV_OK;
}

/*!
 * @brief state unload
 */
NV_STATUS
engstateStateUnload_IMPL
(
    OBJGPU  *pGpu,
    OBJENGSTATE *pEngstate,
    NvU32 flags
)
{
    return NV_OK;
}

/*!
 * @brief state pre-unload
 */
NV_STATUS
engstateStatePreUnload_IMPL
(
    OBJGPU  *pGpu,
    OBJENGSTATE *pEngstate,
    NvU32 flags
)
{
    return NV_OK;
}

/*!
 * @brief state post-unload
 */
NV_STATUS
engstateStatePostUnload_IMPL
(
    OBJGPU  *pGpu,
    OBJENGSTATE *pEngstate,
    NvU32 flags
)
{
    return NV_OK;
}

/*!
 * @brief state destroy
 */
void
engstateStateDestroy_IMPL
(
    OBJGPU *pGpu,
    OBJENGSTATE *pEngstate
)
{
}

/*!
 * @brief returns the ENGDESCRIPTOR associated with this ENGSTATE
 *
 * @param[in]   pEngstate
 */
ENGDESCRIPTOR
engstateGetDescriptor_IMPL
(
    OBJENGSTATE *pEngstate
)
{
    return pEngstate->engDesc;
}

/*!
 * @brief checks for presence of the hardware associated with this ENGSTATE
 *
 * @param[in]   pGpu
 * @param[in]   pEngstate
 */
NvBool
engstateIsPresent_IMPL
(
    OBJGPU *pGpu,
    OBJENGSTATE *pEngstate
)
{
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);

    NV_ASSERT(pEngstate != NULL);
    return kbusCheckEngine_HAL(pGpu, pKernelBus, pEngstate->engDesc);
}


/*!
 * @brief returns the FIFO associated with this ENGSTATE
 *
 * @param[in]   pEngstate
 */
OBJFIFO *
engstateGetFifo_IMPL
(
    OBJENGSTATE *pEngstate
)
{
    OBJGPU         *pGpu = ENG_GET_GPU(pEngstate);

    return GPU_GET_FIFO(pGpu);
}

