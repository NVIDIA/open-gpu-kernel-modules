/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#include "diagnostics/gpu_acct.h"
#include "gpu/timer/objtmr.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "containers/map.h"
#include "containers/list.h"
#include "ctrl/ctrl0000/ctrl0000gpuacct.h"
#include "ctrl/ctrl0000/ctrl0000gpu.h" // NV0000_CTRL_GPU_MAX_ATTACHED_GPUS
#include "virtualization/hypervisor/hypervisor.h"
#include "rmapi/rmapi.h"
#include "rmapi/rmapi_utils.h"

static NV_STATUS gpuacctInitState(GpuAccounting *);
static NvU64 gpuacctGetCurrTime(void);
static NV_STATUS gpuacctAddProcEntry(GPU_ACCT_PROC_DATA_STORE *, GPUACCT_PROC_ENTRY *, NvBool);
static NV_STATUS gpuacctRemoveProcEntry(GPU_ACCT_PROC_DATA_STORE *, GPUACCT_PROC_ENTRY *);
static NV_STATUS gpuacctLookupProcEntry(GPU_ACCT_PROC_DATA_STORE *, NvU32, GPUACCT_PROC_ENTRY **);
static NV_STATUS gpuacctAllocProcEntry(GPU_ACCT_PROC_DATA_STORE *, NvU32, NvU32, GPUACCT_PROC_ENTRY **);
static NV_STATUS gpuacctFreeProcEntry(GPU_ACCT_PROC_DATA_STORE *, GPUACCT_PROC_ENTRY *);
static NV_STATUS gpuacctCleanupDataStore(GPU_ACCT_PROC_DATA_STORE *);
static NV_STATUS gpuacctDestroyDataStore(GPU_ACCT_PROC_DATA_STORE *);
static NV_STATUS gpuacctInitDataStore(GPU_ACCT_PROC_DATA_STORE *);
static NV_STATUS gpuacctStartTimerCallbacks(OBJGPU *, GPUACCT_GPU_INSTANCE_INFO *);
static void gpuacctStopTimerCallbacks(OBJGPU *, GPUACCT_GPU_INSTANCE_INFO *);
static NV_STATUS gpuacctSampleGpuUtil(OBJGPU *, OBJTMR *, TMR_EVENT *);

/*!
 * Constrcutor for gpu accounting class.
 *
 * @param[in,out]  pGpuAcct  GPUACCT object pointer
 *
 * @return  NV_OK  If successfully constructed.
 * @return  Other
 *     Bubbles up errors from:
 *         * gpuacctInitState
 */
NV_STATUS
gpuacctConstruct_IMPL
(
    GpuAccounting *pGpuAcct
)
{
    NV_STATUS status = NV_OK;

    status = gpuacctInitState(pGpuAcct);

    return status;
}

/*!
 * Initializes the data store.
 *
 * @param[in]  pDS       Pointer to data store.
 *
 * @return  NV_OK
 */
static NV_STATUS
gpuacctInitDataStore
(
    GPU_ACCT_PROC_DATA_STORE *pDS
)
{
    NV_STATUS status = NV_OK;

    mapInitIntrusive(&pDS->procMap);
    listInitIntrusive(&pDS->procList);

    return status;
}

/*!
 * Initializes GPUACCT state.
 *
 * @param[in]  pGpuAcct  GPUACCT object pointer
 *
 * @return  NV_OK  Upon successful initialization of GPUACCT.
 * @return  Other
 *     Bubbles up errors from:
 *         * gpuacctInitDataStore
 */
static NV_STATUS gpuacctInitState
(
    GpuAccounting *pGpuAcct
)
{
    NV_STATUS status = NV_OK;
    NvU32 i, j;

    for (i = 0; i < NV0000_CTRL_GPU_MAX_ATTACHED_GPUS; i++)
    {
        pGpuAcct->gpuInstanceInfo[i].pTmrEvent      = NULL;
        pGpuAcct->gpuInstanceInfo[i].pSamplesParams = NULL;

        status = gpuacctInitDataStore(&pGpuAcct->gpuInstanceInfo[i].deadProcAcctInfo);
        if (status != NV_OK)
        {
            goto gpuacctInitState_cleanup;
        }

        status = gpuacctInitDataStore(&pGpuAcct->gpuInstanceInfo[i].liveProcAcctInfo);
        if (status != NV_OK)
        {
            goto gpuacctInitState_cleanup;
        }
    }

    return status;

gpuacctInitState_cleanup:

    for (j = 0; j <= i; j++)
    {
        gpuacctDestroyDataStore(&pGpuAcct->gpuInstanceInfo[j].deadProcAcctInfo);
        gpuacctDestroyDataStore(&pGpuAcct->gpuInstanceInfo[j].liveProcAcctInfo);
    }

    return status;
}

/*!
 * Cleans up data store.
 *
 * @param[in]  pDS       Pointer to data store.
 *
 * @return  NV_OK.
 * @return  NV_ERR_INVALID_ARGUMENT.
 */
static NV_STATUS
gpuacctCleanupDataStore
(
    GPU_ACCT_PROC_DATA_STORE *pDS
)
{
    NV_ASSERT_OR_RETURN(pDS != NULL, NV_ERR_INVALID_ARGUMENT);

    for (GPU_ACCT_PROC_LISTIter iter = listIterAll(&pDS->procList);
         listIterNext(&iter);
         iter = listIterAll(&pDS->procList))
    {
        GPUACCT_PROC_ENTRY *pEntry = iter.pValue;
        if (pEntry)
        {
            gpuacctFreeProcEntry(pDS, pEntry);
        }
    }

    return NV_OK;
}

/*!
 * Destroys data store.
 *
 * @param[in]  pDS       Pointer to data store which needs to be destroyed.
 *
 * @return  NV_OK.
 */
static NV_STATUS
gpuacctDestroyDataStore
(
    GPU_ACCT_PROC_DATA_STORE *pDS
)
{
    NV_STATUS status = gpuacctCleanupDataStore(pDS);

    NV_ASSERT_OR_RETURN(status == NV_OK, status);

    mapDestroy(&pDS->procMap);
    listDestroy(&pDS->procList);

    return NV_OK;
}

/*!
 * Destructor
 *
 * @param[in]  pGpuAcct  GPUACCT object pointer
 */
void gpuacctDestruct_IMPL
(
    GpuAccounting *pGpuAcct
)
{
    NvU32 i;

    for (i = 0; i < NV0000_CTRL_GPU_MAX_ATTACHED_GPUS; i++)
    {
        gpuacctDestroyDataStore(&pGpuAcct->gpuInstanceInfo[i].deadProcAcctInfo);
        gpuacctDestroyDataStore(&pGpuAcct->gpuInstanceInfo[i].liveProcAcctInfo);
    }
}

/*!
 * Allocates an entry for a process and push it in the data store.
 *
 * @param[in]  pDS       Pointer to data store where process entry is to be added.
 * @param[in]  pid       PID of the process.
 * @param[in]  procType  Type of the process.
 * @param[out] ppEntry   Pointer to process entry.
 *
 * @return  NV_OK
 * @return  Other
 *     Bubbles up errors from:
 *         * portMemAllocNonPaged
 *         * gpuacctAddProcEntry
 */
static NV_STATUS
gpuacctAllocProcEntry
(
    GPU_ACCT_PROC_DATA_STORE *pDS,
    NvU32 pid,
    NvU32 procType,
    GPUACCT_PROC_ENTRY **ppEntry
)
{
    GPUACCT_PROC_ENTRY *pEntry;
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(ppEntry != NULL, NV_ERR_INVALID_ARGUMENT);
    *ppEntry = NULL;

    pEntry = portMemAllocNonPaged(sizeof(GPUACCT_PROC_ENTRY));
    if (pEntry == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }
    portMemSet(pEntry, 0, sizeof(GPUACCT_PROC_ENTRY));

    pEntry->procId = pid;
    pEntry->procType = procType;

    status = gpuacctAddProcEntry(pDS, pEntry, NV_TRUE);
    if (status != NV_OK)
    {
        goto out;
    }

    *ppEntry = pEntry;

out:
    if (status != NV_OK)
    {
        portMemSet(pEntry, 0, sizeof(GPUACCT_PROC_ENTRY));
        portMemFree(pEntry);
    }
    return status;
}

/*!
 * Frees a process entry from the data store.
 *
 * @param[in]  pDS       Pointer to data store where process entry is stored.
 * @param[in]  pEntry    Pointer to process entry.
 *
 * @return  NV_OK
 * @return  NV_ERR_INVALID_ARGUMENT
 */
static NV_STATUS
gpuacctFreeProcEntry
(
    GPU_ACCT_PROC_DATA_STORE *pDS,
    GPUACCT_PROC_ENTRY *pEntry
)
{
    NV_STATUS status = gpuacctRemoveProcEntry(pDS, pEntry);

    NV_ASSERT_OR_RETURN(status == NV_OK, status);

    portMemSet(pEntry, 0, sizeof(GPUACCT_PROC_ENTRY));
    portMemFree(pEntry);

    return NV_OK;
}

/*!
 * Looks up an entry for a process in the data store.
 *
 * @param[in]  pDS       Pointer to data store where process entry will be looked.
 * @param[in]  pid       PID of the process.
 * @param[out] ppEntry   Pointer to process entry.
 *
 * @return  NV_OK
 * @return  NV_ERR_INVALID_ARGUMENT
 */
static NV_STATUS
gpuacctLookupProcEntry
(
    GPU_ACCT_PROC_DATA_STORE *pDS,
    NvU32 pid,
    GPUACCT_PROC_ENTRY **ppEntry
)
{
    NV_ASSERT_OR_RETURN(ppEntry != NULL, NV_ERR_INVALID_ARGUMENT);
    *ppEntry = NULL;

    *ppEntry = (GPUACCT_PROC_ENTRY *)mapFind(&pDS->procMap, pid);

    return NV_OK;
}

/*!
 * Removes a process entry from the data store.
 *
 * @param[in]  pDS       Pointer to data store where process entry is stored.
 * @param[in]  pEntry    Pointer to process entry.
 *
 * @return  NV_OK
 * @return  NV_ERR_INVALID_ARGUMENT
 */
static NV_STATUS
gpuacctRemoveProcEntry
(
    GPU_ACCT_PROC_DATA_STORE *pDS,
    GPUACCT_PROC_ENTRY *pEntry
)
{
    NV_ASSERT_OR_RETURN(pDS != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pEntry != NULL, NV_ERR_INVALID_ARGUMENT);

    mapRemove(&pDS->procMap, pEntry);
    listRemove(&pDS->procList, pEntry);

    return NV_OK;
}

/*!
 * Adds a process entry in the data store.
 *
 * @note If data store is full, the entry at the head will be removed and freed.
 *
 * @param[in]  pDS       Pointer to data store where process entry will be added.
 * @param[in]  pEntry    Pointer to process entry.
 *
 * @return  NV_OK
 */
static NV_STATUS
gpuacctAddProcEntry
(
    GPU_ACCT_PROC_DATA_STORE *pDS,
    GPUACCT_PROC_ENTRY *pEntry,
    NvBool isLiveProcEntry
)
{
    NvU32 maxProcLimit;
    GPUACCT_PROC_ENTRY *pOldEntry;

    NV_ASSERT_OR_RETURN(pDS != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pEntry != NULL, NV_ERR_INVALID_ARGUMENT);

    maxProcLimit = isLiveProcEntry ? NV_MAX_LIVE_ACCT_PROCESS : NV_MAX_DEAD_ACCT_PROCESS;

    if (listCount(&pDS->procList) >= maxProcLimit)
    {
        pOldEntry = NULL;
        pOldEntry = listHead(&pDS->procList);
        if (pOldEntry)
        {
            gpuacctFreeProcEntry(pDS, pOldEntry);
        }
    }

    if (mapInsertExisting(&pDS->procMap, pEntry->procId, pEntry) == NV_TRUE)
    {
        listAppendExisting(&pDS->procList, pEntry);
    }
    else
    {
        return NV_ERR_INSERT_DUPLICATE_NAME;
    }
    return NV_OK;
}

/*!
 * Finds the process entry for input PMU sample's pid/subpid.
 *
 * @param[in]  pGpuInstanceInfo   Pointer to GPU node.
 * @param[in]  pid                Input pid.
 * @param[in]  subPid             Input sub pid.
 * @param[out] ppEntry            Double pointer to return proc entry structure.
 *
 */
static NV_STATUS
gpuacctFindProcEntryFromPidSubpid
(
    GPUACCT_GPU_INSTANCE_INFO *pGpuInstanceInfo,
    NvU32 pid,
    NvU32 subPid,
    GPUACCT_PROC_ENTRY **ppEntry)
{
    NV_STATUS status;
    NvU32 pidToSearch;
    GPU_ACCT_PROC_DATA_STORE *pDS;

    if (pGpuInstanceInfo == NULL || ppEntry == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    *ppEntry    = NULL; // Set the output pointer to NULL.
    pDS         = NULL; // Initialize data store pointer to NULL.
    pidToSearch = 0;
    status      = NV_OK;

    if (subPid != 0)
    {
        NvU32 vmIndex;

        // It's a process running on VM, find data store for the VM.
        for (vmIndex = 0; vmIndex < MAX_VGPU_DEVICES_PER_PGPU; ++vmIndex)
        {
            if (pGpuInstanceInfo->vmInstanceInfo[vmIndex].vmPId == pid)
            {
                break;
            }
        }
        if (vmIndex == MAX_VGPU_DEVICES_PER_PGPU)
        {
            // Didn't find vm proc id on this GPU, return error.
            return NV_ERR_INVALID_STATE;
        }
        pDS = &pGpuInstanceInfo->vmInstanceInfo[vmIndex].liveVMProcAcctInfo;
        pidToSearch = subPid;
    }
    else
    {
        // It's a process running on GPU, return data store for GPU.
        pDS = &pGpuInstanceInfo->liveProcAcctInfo;
        pidToSearch = pid;
    }

    if (pDS != NULL)
    {
        status = gpuacctLookupProcEntry(pDS, pidToSearch, ppEntry);
    }

    return status;
}

/*!
 * Samples the gpu utilization and updates running average for all
 * processes running in one GPU instance.
 *
 * @note This is a self scheduling callback that's executed every 1 second.
 *
 * @param[in]  pGpu      GPU object pointer.
 * @param[in]  pTmr      Timer object pointer.
 * @param[in]  pContext  Pointer to process entry.
 *
 * @return  NV_OK
 * @return  NV_ERR_INVALID_STATE
 * @return  Other
 *     Bubbles up errors from:
 *         * tmrScheduleCallbackRelSec
 */
static NV_STATUS
gpuacctSampleGpuUtil
(
    OBJGPU *pGpu,
    OBJTMR *pTmr,
    TMR_EVENT *pTmrEvent
)
{
    GPUACCT_GPU_INSTANCE_INFO *pGpuInstanceInfo = pTmrEvent->pUserData;
    NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2_PARAMS *pParams;
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(!IS_GSP_CLIENT(pGpu), NV_ERR_INVALID_STATE);

    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    if (pGpuInstanceInfo == NULL || pTmr == NULL || pGpuInstanceInfo->pSamplesParams == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "NULL objects found\n");
        return NV_ERR_INVALID_STATE;
    }

    pParams = pGpuInstanceInfo->pSamplesParams;
    portMemSet(pParams, 0, sizeof(*pParams));
    pParams->type = NV2080_CTRL_GPUMON_SAMPLE_TYPE_PERFMON_UTIL;
    pParams->bufSize = sizeof(pParams->samples);
    pParams->tracker = 0;

    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2,
                             pParams,
                             sizeof(NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2_PARAMS));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "GET_GPUMON_PERFMON_UTIL_SAMPLES failed with status : %d\n",
                  status);
        goto gpuacctSampleGpuUtil_out;
    }

    gpuacctProcessGpuUtil(pGpuInstanceInfo, &pParams->samples[0]);

gpuacctSampleGpuUtil_out:

    // Reschedule
    status = tmrEventScheduleRelSec(pTmr, pGpuInstanceInfo->pTmrEvent, 1);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Error sheduling callback for util 0x%x\n",
                  status);
        return status;
    }

    return status;
}

/*!
 * Processes gpu utilization samples, updating running average for all
 * processes running in one GPU instance.
 *
 * @param[in]  pGpuInstanceInfo   GPUACCT gpu instance info.
 * @param[in]  pUtilSampleBuffer  perfmon samples collected for the GPU
 *
 */
void
gpuacctProcessGpuUtil
(
    GPUACCT_GPU_INSTANCE_INFO *pGpuInstanceInfo,
    NV2080_CTRL_PERF_GPUMON_PERFMON_UTIL_SAMPLE *pUtilSampleBuffer
)
{
    GPUACCT_PROC_ENTRY *pEntry;
    NV_STATUS status = NV_OK;
    NvU64 maxTimeStamp = 0;
    NvU32 index;

    for (index = 0; index < NV2080_CTRL_PERF_GPUMON_SAMPLE_COUNT_PERFMON_UTIL; ++index)
    {
        if (pUtilSampleBuffer[index].base.timeStamp <= pGpuInstanceInfo->lastUpdateTimestamp)
        {
            // Skip this input sample entry if the sample has already been processed
            // in previous gpuacctSampleGpuUtil() call.
            continue;
        }

        // Find max timestamp in the input pUtilSampleBuffer pmon sample set.
        maxTimeStamp = NV_MAX(maxTimeStamp, pUtilSampleBuffer[index].base.timeStamp);

        pGpuInstanceInfo->totalSampleCount++;

        pEntry = NULL;

        // If the PMU sample entry's pid or subpid is invalid, then we won't find the
        // pid-subpid entry in data store, so skip processing this PMU gr sample.
        // When run on GSP, only process VF samples.
        if (pUtilSampleBuffer[index].gr.procId       != NV2080_GPUMON_PID_INVALID &&
            pUtilSampleBuffer[index].gr.subProcessID != NV2080_GPUMON_PID_INVALID
        )
        {
            // Find data store in which we should look up the PMU gr sample's pid/subpid.
            status = gpuacctFindProcEntryFromPidSubpid(pGpuInstanceInfo,
                                                       pUtilSampleBuffer[index].gr.procId,
                                                       pUtilSampleBuffer[index].gr.subProcessID,
                                                       &pEntry);
            if (status == NV_OK && pEntry != NULL)
            {
                pEntry->sumUtil += pUtilSampleBuffer[index].gr.util;

                NV_PRINTF(LEVEL_INFO, "pid=%d subPid=%d util=%4d sumUtil=%lld sampleCount=%u (total=%u)\n",
                          pUtilSampleBuffer[index].gr.procId,
                          pUtilSampleBuffer[index].gr.subProcessID,
                          pUtilSampleBuffer[index].gr.util,
                          pEntry->sumUtil,
                          pGpuInstanceInfo->totalSampleCount - pEntry->startSampleCount,
                          pGpuInstanceInfo->totalSampleCount);
            }
        }

        // If the PMU sample entry's pid or subpid is invalid, then we won't find the
        // pid-subpid entry in data store, so skip processing this PMU fb sample.
        // When run on GSP, only process VF samples.
        if (pUtilSampleBuffer[index].fb.procId       != NV2080_GPUMON_PID_INVALID &&
            pUtilSampleBuffer[index].fb.subProcessID != NV2080_GPUMON_PID_INVALID
        )
        {
            // If GR sample and FB sample are of same pid-subpid, no need to find the proc entry again.
            if (pUtilSampleBuffer[index].gr.procId       != pUtilSampleBuffer[index].fb.procId ||
                pUtilSampleBuffer[index].gr.subProcessID != pUtilSampleBuffer[index].fb.subProcessID)
            {
                // Find data store in which we should look up the PMU fb sample's pid/subpid.
                pEntry = NULL;
                status = gpuacctFindProcEntryFromPidSubpid(pGpuInstanceInfo,
                                                           pUtilSampleBuffer[index].fb.procId,
                                                           pUtilSampleBuffer[index].fb.subProcessID,
                                                           &pEntry);
            }
            if (status == NV_OK && pEntry != NULL)
            {
                pEntry->sumFbUtil += pUtilSampleBuffer[index].fb.util;

            }
        }
    }

    // Update max time stamp found in current input sample.
    pGpuInstanceInfo->lastUpdateTimestamp = maxTimeStamp;
}

/*!
 * Starts gpu accounting for the process.
 *
 * @note This functions schedules a 1 second self scheduling timer callback
 * that computes running average of gpu utilization until stopped.
 *
 * @param[in]  pGpuAcct    GPUACCT object pointer
 * @param[in]  gpuInstance gpu instance.
 * @param[in]  pid         PID of the process.
 * @param[in]  subPid      Pid of the process running on guest VM.
 *
 * @return  NV_OK
 * @return  NV_ERR_INVALID_STATE
 * @return  NV_ERR_STATE_IN_USE
 * @return  Other
 *     Bubbles up errors from:
 *         * gpuacctLookupProcEntry
 *         * gpuacctAllocProcEntry
 */
NV_STATUS
gpuacctStartGpuAccounting_IMPL
(
    GpuAccounting *pGpuAcct,
    NvU32 gpuInstance,
    NvU32 pid,
    NvU32 subPid
)
{
    OBJGPU *pGpu;
    NvU32 vmIndex;
    NvU32 searchPid;
    NV_STATUS status = NV_OK;
    GPUACCT_PROC_ENTRY *pEntry = NULL;
    GPU_ACCT_PROC_DATA_STORE *pDS = NULL;
    NvBool bVgpuOnGspEnabled;

    pGpu = gpumgrGetGpu(gpuInstance);
    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_STATE);

    GPUACCT_GPU_INSTANCE_INFO *gpuInstanceInfo = &pGpuAcct->gpuInstanceInfo[gpuInstance];

    vmIndex = NV_INVALID_VM_INDEX;

    bVgpuOnGspEnabled = IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && RMCFG_FEATURE_PLATFORM_GSP;
    if ((hypervisorIsVgxHyper() || bVgpuOnGspEnabled) && IS_VALID_SUBPID(subPid))
    {
        NvU32 i;

        for (i = 0; i < MAX_VGPU_DEVICES_PER_PGPU; i++)
        {
            if (pid == gpuInstanceInfo->vmInstanceInfo[i].vmPId)
            {
                // Check if accounting mode is enabled for this VM.
                // If not, just return.
                if (gpuInstanceInfo->vmInstanceInfo[i].isAccountingEnabled ==
                        NV0000_CTRL_GPU_ACCOUNTING_STATE_DISABLED)
                {
                    return status;
                }

                // Accounting enabled, save the vmIndex and break out of loop.
                vmIndex = i;
                break;
            }
        }
    }

    pDS = (vmIndex == NV_INVALID_VM_INDEX)  ?
          &gpuInstanceInfo->liveProcAcctInfo :
          &gpuInstanceInfo->vmInstanceInfo[vmIndex].liveVMProcAcctInfo;

    NV_ASSERT_OR_RETURN(pDS != NULL, NV_ERR_INVALID_STATE);

    searchPid = (vmIndex == NV_INVALID_VM_INDEX) ? pid : subPid;

    status = gpuacctLookupProcEntry(pDS, searchPid, &pEntry);
    // If pid entry already exists, increment refcount and return.
    if (pEntry != NULL)
    {
        goto out;
    }

    // Create entry for the incoming pid.
    status = gpuacctAllocProcEntry(pDS, searchPid,
                                   NV_GPUACCT_PROC_TYPE_CPU, &pEntry);
    NV_ASSERT_OR_RETURN(status == NV_OK, status);
    NV_ASSERT_OR_RETURN(pEntry != NULL, NV_ERR_NO_MEMORY);

    pEntry->isGuestProcess = (vmIndex == NV_INVALID_VM_INDEX) ? NV_FALSE : NV_TRUE;

    pEntry->startTime = gpuacctGetCurrTime();

    pEntry->startSampleCount = gpuInstanceInfo->totalSampleCount;

    NV_PRINTF(LEVEL_INFO, "pid=%d startSampleCount=%u\n",
              searchPid, pEntry->startSampleCount);

out:
    if (subPid != NV_INVALID_VM_PID)
    {
        pEntry->refCount++;
    }
    return status;
}

/*!
 * Stops gpu accounting for the process.
 *
 * @note Stops the timer for computing gpu utilization and moves the process
 * entry from list of live processes to dead processes.
 *
 * @param[in]  pGpuAcct    GPUACCT object pointer
 * @param[in]  gpuInstance gpu instance.
 * @param[in]  pid         PID of the process.
 * @param[in]  subPid      Pid of the process running on guest VM.
 *
 * @return  NV_OK
 * @return  NV_ERR_INVALID_STATE
 * @return  NV_ERR_STATE_IN_USE
 * @return  Other
 *     Bubbles up errors from:
 *         * gpuacctLookupProcEntry
 *         * gpuacctRemoveProcEntry
 *         * gpuacctAddProcEntry
 */
NV_STATUS
gpuacctStopGpuAccounting_IMPL
(
    GpuAccounting *pGpuAcct,
    NvU32 gpuInstance,
    NvU32 pid,
    NvU32 subPid
)
{
    OBJGPU *pGpu;
    GPUACCT_GPU_INSTANCE_INFO *pGpuInstanceInfo;
    GPU_ACCT_PROC_DATA_STORE *pLiveDS;
    GPU_ACCT_PROC_DATA_STORE *pDeadDS;
    GPUACCT_PROC_ENTRY *pEntry;
    GPUACCT_PROC_ENTRY *pOldEntry;
    NV_STATUS status;
    NvU32 searchPid;
    NvU32 vmIndex;
    NvBool bVgpuOnGspEnabled;

    pGpu = gpumgrGetGpu(gpuInstance);
    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_STATE);

    pGpuInstanceInfo = &pGpuAcct->gpuInstanceInfo[gpuInstance];

    vmIndex = NV_INVALID_VM_INDEX;

    bVgpuOnGspEnabled = IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && RMCFG_FEATURE_PLATFORM_GSP;

    // Find vmIndex, if subPid is passed.
    if ((hypervisorIsVgxHyper() || bVgpuOnGspEnabled) && IS_VALID_SUBPID(subPid))
    {
        NvBool bVMFound = NV_FALSE;
        NvU32 i;

        for (i = 0; i < MAX_VGPU_DEVICES_PER_PGPU; i++)
        {
            if (pid == pGpuInstanceInfo->vmInstanceInfo[i].vmPId)
            {
                bVMFound = NV_TRUE;
                vmIndex = i;
                break;
            }
        }
        if (bVMFound == NV_FALSE)
        {
            return NV_ERR_INVALID_STATE;
        }
    }

    if (vmIndex == NV_INVALID_VM_INDEX)
    {
        // Delete the pid from live process list only if subpid is zero.
        if (subPid != 0)
        {
            return NV_OK;
        }
        pLiveDS = &pGpuInstanceInfo->liveProcAcctInfo;
        pDeadDS = &pGpuInstanceInfo->deadProcAcctInfo;

        searchPid = pid;
    }
    else
    {
        pLiveDS = &pGpuInstanceInfo->vmInstanceInfo[vmIndex].liveVMProcAcctInfo;
        pDeadDS = &pGpuInstanceInfo->vmInstanceInfo[vmIndex].deadVMProcAcctInfo;

        searchPid = subPid;
    }

    status = gpuacctLookupProcEntry(pLiveDS, searchPid, &pEntry);
    if (status != NV_OK)
    {
        return status;
    }

    if (pEntry == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    // return and continue accounting unless this is the last request.
    if (--pEntry->refCount != 0)
    {
        return NV_OK;
    }

    pEntry->endTime = gpuacctGetCurrTime();
    pEntry->totalSampleCount = pGpuInstanceInfo->totalSampleCount - pEntry->startSampleCount;

    NV_PRINTF(LEVEL_INFO, "pid=%d\n", searchPid);

    // Move process entry to dead process list for process that run on gpu.
    if (pEntry->procType == NV_GPUACCT_PROC_TYPE_GPU)
    {
        // Only keep the latest entry for the pid.
        status = gpuacctLookupProcEntry(pDeadDS, searchPid, &pOldEntry);
        if (status != NV_OK)
        {
            return status;
        }

        if (pOldEntry != NULL)
        {
            status = gpuacctFreeProcEntry(pDeadDS, pOldEntry);
            if (status != NV_OK)
            {
                return status;
            }
        }

        // Move the entry to dead procs data store
        status = gpuacctRemoveProcEntry(pLiveDS, pEntry);
        if (status != NV_OK)
        {
            return status;
        }

        status = gpuacctAddProcEntry(pDeadDS, pEntry, NV_FALSE);
        if (status != NV_OK)
        {
            portMemSet(pEntry, 0, sizeof(GPUACCT_PROC_ENTRY));
            portMemFree(pEntry);
            return status;
        }
    }
    else
    {
        status = gpuacctFreeProcEntry(pLiveDS, pEntry);
        if (status != NV_OK)
        {
            return status;
        }
    }

    return status;
}

/*!
 * Updates peak (high-water mark) FB utilization of the process.
 *
 * @param[in]  pGpuAcct    GPUACCT object pointer
 * @param[in]  gpuInstance gpu instance.
 * @param[in]  pid         PID of the process.
 * @param[in]  subPid      PID of the process running on guest VM.
 * @param[in]  fbUsage     Current FB usage of the process.
 *
 * @return  NV_OK
 * @return  NV_ERR_INVALID_STATE
 * @return  Other
 *     Bubbles up errors from:
 *         * gpuacctLookupProcEntry
 */
NV_STATUS
gpuacctUpdateProcPeakFbUsage_IMPL
(
    GpuAccounting *pGpuAcct,
    NvU32 gpuInstance,
    NvU32 pid,
    NvU32 subPid,
    NvU64 fbUsage
)
{
    GPUACCT_PROC_ENTRY *pEntry;
    GPU_ACCT_PROC_DATA_STORE *pDS = NULL;
    NV_STATUS status;

    // Find live process data store for the VM if subpid was passed.
    if (hypervisorIsVgxHyper() && IS_VALID_SUBPID(subPid))
    {
        NvU32 vmIndex;

        for (vmIndex = 0; vmIndex < MAX_VGPU_DEVICES_PER_PGPU; vmIndex++)
        {
            if (pGpuAcct->gpuInstanceInfo[gpuInstance].vmInstanceInfo[vmIndex].vmPId == pid)
            {
                pDS = &pGpuAcct->gpuInstanceInfo[gpuInstance].vmInstanceInfo[vmIndex].liveVMProcAcctInfo;
                break;
            }
        }
    }
    else
    {
        pDS = &pGpuAcct->gpuInstanceInfo[gpuInstance].liveProcAcctInfo;
    }

    NV_ASSERT_OR_RETURN(pDS != NULL, NV_ERR_INVALID_STATE);

    if (hypervisorIsVgxHyper() && IS_VALID_SUBPID(subPid))
    {
        status = gpuacctLookupProcEntry(pDS, subPid, &pEntry);
    }
    else
    {
        status = gpuacctLookupProcEntry(pDS, pid, &pEntry);
    }

    if (status != NV_OK)
    {
        return status;
    }

    if (pEntry == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    pEntry->procType = NV_GPUACCT_PROC_TYPE_GPU;

    if (fbUsage > pEntry->maxFbUsage)
    {
        pEntry->maxFbUsage = fbUsage;
    }

    return status;
}

/*!
 * Set process type.
 *
 * @param[in]  pGpuAcct    GPUACCT object pointer
 * @param[in]  gpuInstance gpu instance.
 * @param[in]  pid         PID of the process.
 * @param[in]  subPid      PID of the process running on guest VM.
 * @param[in]  procType    Type of the process.
 *
 * @return  NV_OK
 * @return  NV_ERR_INVALID_STATE
 * @return  Other
 *     Bubbles up errors from:
 *         * gpuacctLookupProcEntry
 */
NV_STATUS
gpuacctSetProcType_IMPL
(
    GpuAccounting *pGpuAcct,
    NvU32 gpuInstance,
    NvU32 pid,
    NvU32 subPid,
    NvU32 procType
)
{
    OBJGPU *pGpu;
    GPUACCT_PROC_ENTRY *pEntry;
    GPU_ACCT_PROC_DATA_STORE *pDS = NULL;
    NV_STATUS status;
    NvBool bVgpuOnGspEnabled;

    pGpu = gpumgrGetGpu(gpuInstance);
    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_STATE);

    bVgpuOnGspEnabled = IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu);
    // If on VGX host and the call is for a guest process.
    if ((hypervisorIsVgxHyper() || bVgpuOnGspEnabled) && IS_VALID_SUBPID(subPid))
    {
        NvU32 vmIndex;

        for (vmIndex = 0; vmIndex < MAX_VGPU_DEVICES_PER_PGPU; vmIndex++)
        {
            if (pGpuAcct->gpuInstanceInfo[gpuInstance].vmInstanceInfo[vmIndex].vmPId == pid)
            {
                pDS = &pGpuAcct->gpuInstanceInfo[gpuInstance].vmInstanceInfo[vmIndex].liveVMProcAcctInfo;
                break;
            }
        }
    }
    else
    {
        pDS = &pGpuAcct->gpuInstanceInfo[gpuInstance].liveProcAcctInfo;
    }

    NV_ASSERT_OR_RETURN(pDS != NULL, NV_ERR_INVALID_STATE);

    if ((hypervisorIsVgxHyper() || bVgpuOnGspEnabled) && IS_VALID_SUBPID(subPid))
    {
        status = gpuacctLookupProcEntry(pDS, subPid, &pEntry);
    }
    else
    {
        status = gpuacctLookupProcEntry(pDS, pid, &pEntry);
    }

    if (status != NV_OK)
    {
        return status;
    }

    if (pEntry == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    pEntry->procType = procType;

    return status;
}

/*!
 * Gets GPU accounting info for the process.
 *
 * @param[in]     pGpuAcct    GPUACCT object pointer
 * @param[in,out] pParams     NV0000_CTRL_GPUACCT_GET_PROC_ACCOUNTING_INFO_PARAMS pointer.
 *
 * @return  NV_OK
 * @return  NV_ERR_INVALID_ARGUMENT
 * @return  NV_ERR_INVALID_STATE
 * @return  NV_ERR_OBJECT_NOT_FOUND
 * @return  Other
 *     Bubbles up errors from:
 *         * gpuacctLookupProcEntry
 */
NV_STATUS
gpuacctGetProcAcctInfo_IMPL
(
    GpuAccounting *pGpuAcct,
    NV0000_CTRL_GPUACCT_GET_PROC_ACCOUNTING_INFO_PARAMS *pParams
)
{
    GPUACCT_PROC_ENTRY *pEntry;
    GPU_ACCT_PROC_DATA_STORE *pDS = NULL;
    NV_STATUS status;
    OBJGPU *pGpu;
    NvU32 vmIndex;
    GPUACCT_GPU_INSTANCE_INFO *pGpuInstanceInfo;
    NvU32 sampleCount;
    NvBool isLiveProcess;
    NvBool bVgpuOnGspEnabled;

    NV_ASSERT_OR_RETURN(pParams != NULL, NV_ERR_INVALID_ARGUMENT);

    pGpu = gpumgrGetGpuFromId(pParams->gpuId);
    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_ARGUMENT);

    pGpuInstanceInfo = &pGpuAcct->gpuInstanceInfo[pGpu->gpuInstance];
    vmIndex = NV_INVALID_VM_INDEX;

    bVgpuOnGspEnabled = IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && RMCFG_FEATURE_PLATFORM_GSP;

    // if subPid passed in, find the VM index.
    if ((hypervisorIsVgxHyper() || bVgpuOnGspEnabled) && IS_VALID_SUBPID(pParams->subPid))
    {
        NvBool bVMFound = NV_FALSE;
        NvU32 i;

        if (pParams->pid != NV_INVALID_VM_PID)
        {
            for (i = 0; i < MAX_VGPU_DEVICES_PER_PGPU; i++)
            {
                if (pGpuInstanceInfo->vmInstanceInfo[i].vmPId == pParams->pid)
                {
                    vmIndex = i;
                    bVMFound = NV_TRUE;
                    break;
                }
            }
            // If call is for VM process and VM not found, cannot continue, return.
            if (bVMFound == NV_FALSE)
            {
                return NV_ERR_INVALID_ARGUMENT;
            }
        }
    }

    isLiveProcess = NV_FALSE;

    // Try finding process entry in dead process list.
    if (vmIndex == NV_INVALID_VM_INDEX)
    {
        pDS = &pGpuInstanceInfo->deadProcAcctInfo;
    }
    else
    {
        pDS = &pGpuInstanceInfo->vmInstanceInfo[vmIndex].deadVMProcAcctInfo;
    }
    NV_ASSERT_OR_RETURN(pDS != NULL, NV_ERR_INVALID_STATE);

    if (vmIndex == NV_INVALID_VM_INDEX)
    {
        status = gpuacctLookupProcEntry(pDS, pParams->pid, &pEntry);
    }
    else
    {
        status = gpuacctLookupProcEntry(pDS, pParams->subPid, &pEntry);
    }

    if (status != NV_OK)
    {
        return status;
    }

    // If process entry not found in dead process list, try finding in live
    // process list.
    if (pEntry == NULL)
    {
        isLiveProcess = NV_TRUE;

        if (vmIndex == NV_INVALID_VM_INDEX)
        {
            pDS = &pGpuInstanceInfo->liveProcAcctInfo;
        }
        else
        {
            pDS = &pGpuInstanceInfo->vmInstanceInfo[vmIndex].liveVMProcAcctInfo;
        }
        NV_ASSERT_OR_RETURN(pDS != NULL, NV_ERR_INVALID_STATE);

        if (vmIndex == NV_INVALID_VM_INDEX)
        {
            status = gpuacctLookupProcEntry(pDS, pParams->pid, &pEntry);
        }
        else
        {
            status = gpuacctLookupProcEntry(pDS, pParams->subPid, &pEntry);
        }
        if (status != NV_OK)
        {
            return status;
        }

        if ((pEntry == NULL) || (pEntry->procType != NV_GPUACCT_PROC_TYPE_GPU))
        {
            return NV_ERR_OBJECT_NOT_FOUND;
        }
    }

    pParams->maxFbUsage = pEntry->maxFbUsage;
    pParams->gpuUtil    = pEntry->gpuUtil;
    pParams->fbUtil     = pEntry->fbUtil;
    pParams->startTime  = pEntry->startTime;
    pParams->endTime    = pEntry->endTime;

    sampleCount = isLiveProcess == NV_TRUE ?
                  pGpuInstanceInfo->totalSampleCount - pEntry->startSampleCount:
                  pEntry->totalSampleCount;
    if (sampleCount)
    {
        pParams->gpuUtil = (NvU32)(pEntry->sumUtil / sampleCount);
        pParams->gpuUtil /= 100;

        pParams->fbUtil = (NvU32)(pEntry->sumFbUtil / sampleCount);
        pParams->fbUtil /= 100;
    }

    return NV_OK;
}

/*!
 * Gets all the pids for which accounting data is available.
 *
 * @param[in]     pGpuAcct    GPUACCT object pointer
 * @param[in,out] pParams     NV0000_CTRL_GPUACCT_GET_ACCOUNTING_PIDS_PARAMS pointer.
 *
 * @return  NV_OK
 * @return  NV_ERR_INVALID_ARGUMENT
 * @return  NV_ERR_INVALID_STATE
 */
NV_STATUS
gpuacctGetAcctPids_IMPL
(
    GpuAccounting *pGpuAcct,
    NV0000_CTRL_GPUACCT_GET_ACCOUNTING_PIDS_PARAMS *pParams
)
{
    GPUACCT_PROC_ENTRY *pEntry;
    GPU_ACCT_PROC_LIST *pList;
    OBJGPU *pGpu;
    NvU32 count;
    NvU32 vmPid;
    NvU32 vmIndex;
    GPUACCT_GPU_INSTANCE_INFO *pGpuInstanceInfo;
    NvBool bVgpuOnGspEnabled;

    ct_assert((NV_MAX_LIVE_ACCT_PROCESS + NV_MAX_DEAD_ACCT_PROCESS) <= NV0000_GPUACCT_PID_MAX_COUNT);

    if (pParams == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    pGpu = gpumgrGetGpuFromId(pParams->gpuId);
    if (pGpu == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    pGpuInstanceInfo = &pGpuAcct->gpuInstanceInfo[pGpu->gpuInstance];
    count = 0;
    vmIndex = NV_INVALID_VM_INDEX;

    bVgpuOnGspEnabled = IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && RMCFG_FEATURE_PLATFORM_GSP;

    // Set vmPid if we are on VGX host and pAcctPidsParams->pid is non zero.
    // pAcctPidsParams->pid will be set when the RM control call is coming
    // from VGPU plugin, otherwise will be 0.
    vmPid = ((hypervisorIsVgxHyper() || bVgpuOnGspEnabled) && (pParams->pid != 0)) ?
            pParams->pid :
            NV_INVALID_VM_PID;

    // Find vmIndex if vmPid is provided.
    if (vmPid != NV_INVALID_VM_PID)
    {
        NvU32 i;
        NvBool bVMFound = NV_FALSE;

        for (i = 0; i < MAX_VGPU_DEVICES_PER_PGPU; i++)
        {
            if (pGpuInstanceInfo->vmInstanceInfo[i].vmPId == vmPid)
            {
                vmIndex = i;
                bVMFound = NV_TRUE;
                break;
            }
        }
        if (bVMFound == NV_FALSE)
        {
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    if (vmIndex == NV_INVALID_VM_INDEX)
    {
        // Skip dead info if the requester is not from root namespace.
        if (osIsInitNs() != NV_TRUE)
        {
            goto addLiveProc;
        }

        pList = &pGpuInstanceInfo->deadProcAcctInfo.procList;
    }
    else
    {
        pList = &pGpuInstanceInfo->vmInstanceInfo[vmIndex].deadVMProcAcctInfo.procList;
    }
    NV_ASSERT_OR_RETURN(pList != NULL, NV_ERR_INVALID_STATE);

    GPU_ACCT_PROC_LISTIter iter = listIterAll(pList);
    while (listIterNext(&iter))
    {
        pEntry = iter.pValue;
        if (pEntry)
        {
            pParams->pidTbl[count++] = pEntry->procId;
        }
    }

addLiveProc:
    if (vmIndex == NV_INVALID_VM_INDEX)
    {
        pList = &pGpuInstanceInfo->liveProcAcctInfo.procList;
    }
    else
    {
        pList = &pGpuInstanceInfo->vmInstanceInfo[vmIndex].liveVMProcAcctInfo.procList;
    }
    NV_ASSERT_OR_RETURN(pList != NULL, NV_ERR_INVALID_STATE);

    iter = listIterAll(pList);
    while (listIterNext(&iter))
    {
        pEntry = iter.pValue;
        if (pEntry && pEntry->procType == NV_GPUACCT_PROC_TYPE_GPU)
        {
            pParams->pidTbl[count++] = pEntry->procId;
        }
    }

    pParams->pidCount = count;

    return NV_OK;
}

/*!
 * Gets accounting mode.
 *
 * @param[in]      pGpuAcct             GPUACCT object pointer
 * @param[in]      gpuInstance          gpu instance.
 * @param[in,out]  pGetAcctModeParams   NV0000_CTRL_GPUACCT_GET_ACCOUNTING_STATE_PARAMS pointer.
 *
 * @return  NV_OK
 * @return  NV_ERR_INVALID_ARGUMENT
 */
NV_STATUS
gpuacctGetAccountingMode_IMPL
(
    GpuAccounting *pGpuAcct,
    NvU32 gpuInstance,
    NV0000_CTRL_GPUACCT_GET_ACCOUNTING_STATE_PARAMS *pGetAcctModeParams
)
{
    OBJGPU *pGpu;
    NvU32 vmPid = NV_INVALID_VM_PID;
    NvBool bVgpuOnGspEnabled;

    NV_ASSERT_OR_RETURN(pGetAcctModeParams != NULL, NV_ERR_INVALID_ARGUMENT);

    pGpu = gpumgrGetGpu(gpuInstance);
    if (pGpu == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    bVgpuOnGspEnabled = IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && RMCFG_FEATURE_PLATFORM_GSP;

    // Set vmPid if we are on VGX host and pParams->pid is non zero.
    // pParams->pid will be set when the RM control call is coming
    // from VGPU plugin, otherwise will be 0.
    vmPid = ((hypervisorIsVgxHyper() || bVgpuOnGspEnabled) && (pGetAcctModeParams->pid != 0)) ?
            pGetAcctModeParams->pid :
            NV_INVALID_VM_PID;

    if (vmPid == NV_INVALID_VM_PID)
    {
        pGetAcctModeParams->state = pGpu->getProperty(pGpu, PDB_PROP_GPU_ACCOUNTING_ON) ?
                                    NV0000_CTRL_GPU_ACCOUNTING_STATE_ENABLED :
                                    NV0000_CTRL_GPU_ACCOUNTING_STATE_DISABLED;
    }
    else
    {
        NvU32 vmIndex;
        NvBool bVMFound;

        GPUACCT_GPU_INSTANCE_INFO *gpuInstanceInfo = &pGpuAcct->gpuInstanceInfo[pGpu->gpuInstance];
        bVMFound = NV_FALSE;
        for (vmIndex = 0; vmIndex < MAX_VGPU_DEVICES_PER_PGPU; vmIndex++)
        {
            if (gpuInstanceInfo->vmInstanceInfo[vmIndex].vmPId == vmPid)
            {
                bVMFound = NV_TRUE;
                pGetAcctModeParams->state = gpuInstanceInfo->vmInstanceInfo[vmIndex].isAccountingEnabled;
                break;
            }
        }
        if (bVMFound == NV_FALSE)
        {
            return NV_ERR_OBJECT_NOT_FOUND;
        }
    }

    return NV_OK;
}

/*!
 * Starts timer callbacks.
 *
 * @param[in]      pGpu
 * @param[in]      pGpuInstanceInfo     GPUACCT gpu instance info.
 *
 * @return  NV_OK
 * @return  NV_ERR_INVALID_STATE
 * @return  NV_ERR_NO_MEMORY
 */
static NV_STATUS
gpuacctStartTimerCallbacks
(
    OBJGPU *pGpu,
    GPUACCT_GPU_INSTANCE_INFO *pGpuInstanceInfo
)
{
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    NV_STATUS status = NV_OK;

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ACCOUNTING_ON))
        return NV_OK;

    if (pTmr == NULL && !IS_GSP_CLIENT(pGpu))
    {
        NV_PRINTF(LEVEL_ERROR, "NULL pTmr object found\n");
        return NV_ERR_INVALID_STATE;
    }

    pGpuInstanceInfo->pSamplesParams =
        portMemAllocNonPaged(sizeof(*pGpuInstanceInfo->pSamplesParams));
    if (pGpuInstanceInfo->pSamplesParams == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to allocate memory for sample params\n");
        goto done;
    }

    if (IS_GSP_CLIENT(pGpu))
        goto done;

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        tmrEventCreate(pTmr, &pGpuInstanceInfo->pTmrEvent, gpuacctSampleGpuUtil,
                       pGpuInstanceInfo, TMR_FLAGS_NONE),
        done);

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        tmrEventScheduleRelSec(pTmr, pGpuInstanceInfo->pTmrEvent, 1),
        done);

  done:
    if (status == NV_OK)
        pGpu->setProperty(pGpu, PDB_PROP_GPU_ACCOUNTING_ON, NV_TRUE);
    else
        gpuacctStopTimerCallbacks(pGpu, pGpuInstanceInfo);

    return status;
}

/*!
 * Stops timer callbacks.
 *
 * @param[in]      pGpu
 * @param[in]      pGpuInstanceInfo     GPUACCT gpu instance info.
 */
static void
gpuacctStopTimerCallbacks
(
    OBJGPU *pGpu,
    GPUACCT_GPU_INSTANCE_INFO *pGpuInstanceInfo
)
{
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);

    if (IS_GSP_CLIENT(pGpu))
        goto done;

    if (pTmr != NULL && pGpuInstanceInfo->pTmrEvent != NULL)
    {
        tmrEventDestroy(pTmr, pGpuInstanceInfo->pTmrEvent);
        pGpuInstanceInfo->pTmrEvent = NULL;
    }

done:
    portMemFree(pGpuInstanceInfo->pSamplesParams);
    pGpuInstanceInfo->pSamplesParams = NULL;
    pGpu->setProperty(pGpu, PDB_PROP_GPU_ACCOUNTING_ON, NV_FALSE);
}

/*!
 * Enables accounting for the gpu.
 *
 * @param[in]      pGpuAcct             GPUACCT object pointer
 * @param[in]      gpuInstance          gpu instance.
 * @param[in,out]  pSetAcctModeParams   NV0000_CTRL_GPUACCT_SET_ACCOUNTING_STATE_PARAMS pointer.
 *
 * @return  NV_OK
 * @return  NV_ERR_INVALID_ARGUMENT
 */
NV_STATUS
gpuacctEnableAccounting_IMPL
(
    GpuAccounting *pGpuAcct,
    NvU32 gpuInstance,
    NV0000_CTRL_GPUACCT_SET_ACCOUNTING_STATE_PARAMS *pSetAcctModeParams
)
{
    OBJGPU *pGpu;
    GPUACCT_GPU_INSTANCE_INFO *pGpuInstanceInfo;
    NV_STATUS status = NV_OK;
    NvU32 vmPid = NV_INVALID_VM_PID;
    NvBool bVgpuOnGspEnabled;

    NV_ASSERT_OR_RETURN(pSetAcctModeParams != NULL, NV_ERR_INVALID_ARGUMENT);

    pGpu = gpumgrGetGpu(gpuInstance);
    if (pGpu == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    //
    // Accounting mode is not supported if SMC is enabled.
    // TODO:CUDANVML-43 tracks a top-level task to make accounting mode
    // MIG aware.
    //
    if (IS_MIG_ENABLED(pGpu))
        return NV_ERR_NOT_SUPPORTED;

    bVgpuOnGspEnabled = IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && RMCFG_FEATURE_PLATFORM_GSP;

    // Set vmPid if we are on VGX host and pParams->pid is non zero.
    // pParams->pid will be set when the RM control call is coming
    // from VGPU plugin, otherwise will be 0.
    vmPid = ((hypervisorIsVgxHyper() || bVgpuOnGspEnabled) && (pSetAcctModeParams->pid != 0)) ?
                   pSetAcctModeParams->pid :
                   NV_INVALID_VM_PID;

    pGpuInstanceInfo = &pGpuAcct->gpuInstanceInfo[pGpu->gpuInstance];

    if (vmPid == NV_INVALID_VM_PID)
    {
        status = gpuacctStartTimerCallbacks(pGpu, pGpuInstanceInfo);
        if (status != NV_OK)
            return status;
    }
    else
    {
        NvU32 vmIndex;
        NvBool bVMFound = NV_FALSE;
        for(vmIndex = 0; vmIndex < MAX_VGPU_DEVICES_PER_PGPU; vmIndex++)
        {
            if (pGpuInstanceInfo->vmInstanceInfo[vmIndex].vmPId == vmPid)
            {
                bVMFound = NV_TRUE;
                pGpuInstanceInfo->vmInstanceInfo[vmIndex].isAccountingEnabled = NV0000_CTRL_GPU_ACCOUNTING_STATE_ENABLED;
                break;
            }
        }
        if (bVMFound == NV_FALSE)
        {
            return NV_ERR_OBJECT_NOT_FOUND;
        }
    }

    return status;
}

/*!
 * Disables accounting for the gpu and cleans up accounting data.
 *
 * @param[in]      pGpuAcct             GPUACCT object pointer
 * @param[in]      gpuInstance          gpu instance.
 * @param[in,out]  pSetAcctModeParams   NV0000_CTRL_GPUACCT_SET_ACCOUNTING_STATE_PARAMS pointer.
 *
 * @return  NV_OK
 * @return  NV_ERR_INVALID_ARGUMENT
 * @return  NV_ERR_INVALID_STATE
 */
NV_STATUS
gpuacctDisableAccounting_IMPL
(
    GpuAccounting *pGpuAcct,
    NvU32 gpuInstance,
    NV0000_CTRL_GPUACCT_SET_ACCOUNTING_STATE_PARAMS *pSetAcctModeParams
)
{
    GPU_ACCT_PROC_DATA_STORE *pDS = NULL;
    OBJGPU *pGpu;
    NvU32 vmPid = NV_INVALID_VM_PID;
    GPUACCT_GPU_INSTANCE_INFO *pGpuInstanceInfo;
    NvBool bVgpuOnGspEnabled;

    NV_ASSERT_OR_RETURN(pSetAcctModeParams != NULL, NV_ERR_INVALID_ARGUMENT);

    pGpu = gpumgrGetGpu(gpuInstance);
    if (pGpu == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    bVgpuOnGspEnabled = IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && RMCFG_FEATURE_PLATFORM_GSP;

    // Set vmPid if we are on VGX host and pParams->pid is non zero.
    // pParams->pid will be set when the RM control call is coming
    // from VGPU plugin, otherwise will be 0.
    vmPid = ((hypervisorIsVgxHyper() || bVgpuOnGspEnabled) && (pSetAcctModeParams->pid != 0)) ?
             pSetAcctModeParams->pid :
             NV_INVALID_VM_PID;

    // On VGX host, accounting mode will be enabled by default, so that we can
    // start gathering accounting data regardless of when guest comes and goes.
    // Don't allow user to disable accounting mode on VGX host.
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ACCOUNTING_ON) &&
        (hypervisorIsVgxHyper() || bVgpuOnGspEnabled) && (vmPid == NV_INVALID_VM_PID))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    pGpuInstanceInfo = &pGpuAcct->gpuInstanceInfo[gpuInstance];

    if (vmPid == NV_INVALID_VM_PID)
    {
        gpuacctStopTimerCallbacks(pGpu, pGpuInstanceInfo);

        pDS = &pGpuInstanceInfo->liveProcAcctInfo;
    }
    else
    {
        NvU32 i;

        for (i = 0; i < MAX_VGPU_DEVICES_PER_PGPU; i++)
        {
            if (pGpuInstanceInfo->vmInstanceInfo[i].vmPId == vmPid)
            {
                pGpuInstanceInfo->vmInstanceInfo[i].isAccountingEnabled = NV0000_CTRL_GPU_ACCOUNTING_STATE_DISABLED;

                pDS = &pGpuInstanceInfo->vmInstanceInfo[i].liveVMProcAcctInfo;
                break;
            }
        }
    }
    NV_ASSERT_OR_RETURN(pDS != NULL, NV_ERR_INVALID_STATE);

    // remove (stale) entries from live process data store.
    gpuacctCleanupDataStore(pDS);

    return NV_OK;
}

/*!
 * Clears accounting data for the gpu.
 *
 * @param[in]      pGpuAcct               GPUACCT object pointer
 * @param[in]      gpuInstance            gpu instance.
 * @param[in,out]  pClearAcctDataParams   NV0000_CTRL_GPUACCT_CLEAR_ACCOUNTING_DATA_PARAMS pointer.
 *
 * @return  NV_OK
 * @return  NV_ERR_INVALID_ARGUMENT
 * @return  NV_ERR_INVALID_STATE
 */
NV_STATUS
gpuacctClearAccountingData_IMPL
(
    GpuAccounting *pGpuAcct,
    NvU32 gpuInstance,
    NV0000_CTRL_GPUACCT_CLEAR_ACCOUNTING_DATA_PARAMS *pClearAcctDataParams
)
{
    OBJGPU *pGpu;
    NvU32 vmPid = NV_INVALID_VM_PID;
    GPUACCT_GPU_INSTANCE_INFO *gpuInstanceInfo = &pGpuAcct->gpuInstanceInfo[gpuInstance];
    NvBool bVgpuOnGspEnabled;

    pGpu = gpumgrGetGpu(gpuInstance);
    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pClearAcctDataParams != NULL, NV_ERR_INVALID_ARGUMENT);

    bVgpuOnGspEnabled = IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && RMCFG_FEATURE_PLATFORM_GSP;

    // Set vmPid if we are on VGX host and pParams->pid is non zero.
    // pParams->pid will be set when the RM control call is coming
    // from VGPU plugin, otherwise will be 0.
    vmPid = ((hypervisorIsVgxHyper() || bVgpuOnGspEnabled) && (pClearAcctDataParams->pid != 0)) ?
            pClearAcctDataParams->pid :
            NV_INVALID_VM_PID;

    if (vmPid == NV_INVALID_VM_PID)
    {
        gpuacctCleanupDataStore(&gpuInstanceInfo->deadProcAcctInfo);
    }
    else
    {
        NvU32 vmIndex;

        for (vmIndex = 0; vmIndex < MAX_VGPU_DEVICES_PER_PGPU; vmIndex++)
        {
            if (gpuInstanceInfo->vmInstanceInfo[vmIndex].vmPId == vmPid)
            {
                gpuacctCleanupDataStore(&gpuInstanceInfo->vmInstanceInfo[vmIndex].deadVMProcAcctInfo);
                break;
            }
        }
    }

    return NV_OK;
}

/*!
 * Returns the time in micro seconds.
 *
 */
static NvU64 gpuacctGetCurrTime
(
     void
)
{
    NvU64 currTime = 0x00000000ffffffff;
    NvU32 currTimeHi, currTimeLo;
    osGetCurrentTime(&currTimeHi, &currTimeLo);

    currTime = currTime & currTimeHi;
    currTime = (currTime * 1000000) + currTimeLo;

    return currTime;
}

/*!
 * Destroys accounting data for the VM.
 *
 * @param[in]  vmInstanceInfo   VM instance.
  */
static void
_vmAcctDestroyDataStore(GPUACCT_VM_INSTANCE_INFO *pVMInstanceInfo)
{
    if (pVMInstanceInfo == NULL)
    {
        return;
    }

    gpuacctDestroyDataStore(&pVMInstanceInfo->liveVMProcAcctInfo);
    gpuacctDestroyDataStore(&pVMInstanceInfo->deadVMProcAcctInfo);
}

/*!
 * Initilizes data store for a VM
 *
 * @param[in]  vmInstanceInfo   VM instance.
 *
 * @return  NV_OK
 * @return  NV_ERR_INVALID_ARGUMENT
 * @return  NV_ERR_INVALID_STATE
 */
static NV_STATUS
vmAcctInitDataStore(GPUACCT_VM_INSTANCE_INFO *vmInstanceInfo)
{
    NV_STATUS status;

    if (vmInstanceInfo == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = gpuacctInitDataStore(&vmInstanceInfo->deadVMProcAcctInfo);
    if (status != NV_OK)
    {
        gpuacctDestroyDataStore(&vmInstanceInfo->deadVMProcAcctInfo);
        return status;
    }

    status = gpuacctInitDataStore(&vmInstanceInfo->liveVMProcAcctInfo);
    if (status != NV_OK)
    {
        gpuacctDestroyDataStore(&vmInstanceInfo->deadVMProcAcctInfo);
        gpuacctDestroyDataStore(&vmInstanceInfo->liveVMProcAcctInfo);
    }

    return status;
}

/*!
 * Top level function to initiate VM data store creation.
 *
 * @param[in]  pSys             Pointer of Sys.
 * @param[in]  pGpu             Pointer to physical gpu object.
 * @param[in]  vmPid            VM/Plugin process id.
*/
void
vmAcctInitState(OBJGPU *pGpu, NvU32 vmPid)
{
    NvU32 vmIndex;
    NvU32 targetVMIndex;
    NvBool vmInstanceFound;

    OBJSYS *pSys = SYS_GET_INSTANCE();
    GpuAccounting *pGpuAcct = SYS_GET_GPUACCT(pSys);

    if (pGpu && pGpuAcct)
    {
        GPUACCT_GPU_INSTANCE_INFO *gpuInstanceInfo = &pGpuAcct->gpuInstanceInfo[pGpu->gpuInstance];
        targetVMIndex = 0;
        vmInstanceFound = NV_FALSE;

        // Check if we have data store for this VM pid.
        // If found, delete those data store.
        for (vmIndex = 0; vmIndex < MAX_VGPU_DEVICES_PER_PGPU; vmIndex++)
        {
            GPUACCT_VM_INSTANCE_INFO *vmInstanceInfo = &gpuInstanceInfo->vmInstanceInfo[vmIndex];

            if (vmPid == vmInstanceInfo->vmPId)
            {
                vmInstanceFound = NV_TRUE;
                targetVMIndex   = vmIndex;

                _vmAcctDestroyDataStore(vmInstanceInfo);

                vmInstanceInfo->isAccountingEnabled = NV0000_CTRL_GPU_ACCOUNTING_STATE_DISABLED;

                break;
            }
            if ((vmInstanceFound == NV_FALSE) && (vmInstanceInfo->vmPId == 0))
            {
                vmInstanceFound = NV_TRUE;
                targetVMIndex   = vmIndex;
            }
        }

        if (vmInstanceFound == NV_TRUE)
        {
            GPUACCT_VM_INSTANCE_INFO *vmInstanceInfo = &gpuInstanceInfo->vmInstanceInfo[targetVMIndex];

            if (vmAcctInitDataStore(vmInstanceInfo) == NV_OK)
            {
                vmInstanceInfo->vmPId = vmPid;
                vmInstanceInfo->isAccountingEnabled = NV0000_CTRL_GPU_ACCOUNTING_STATE_DISABLED;
            }
            else
            {
                NV_PRINTF(LEVEL_INFO,
                          "Failed to create process accounting data store for VM Pid : %d\n",
                          vmInstanceInfo->vmPId);
            }
        }
    }
}

/*!
 * Top level function to destroy VM data store.
 *
 * @param[in]  pSys           Pointer of Sys.
 * @param[in]  vmPid          VM/Plugin process id.
 * @param[in]  gpuPciId       gpuid of the physical gpu associated with the VM.
   */
void
vmAcctDestructState(NvU32 vmPid, OBJGPU *pGpu)
{
    GpuAccounting *pGpuAcct;
    NvU32 vmIndex;

    OBJSYS *pSys = SYS_GET_INSTANCE();
    pGpuAcct = SYS_GET_GPUACCT(pSys);

    if (pGpu && pGpuAcct)
    {
        for (vmIndex = 0; vmIndex < MAX_VGPU_DEVICES_PER_PGPU; vmIndex++)
        {
            GPUACCT_VM_INSTANCE_INFO *vmInstanceInfo = &pGpuAcct->gpuInstanceInfo[pGpu->gpuInstance].vmInstanceInfo[vmIndex];
            if (vmPid == vmInstanceInfo->vmPId)
            {
                _vmAcctDestroyDataStore(vmInstanceInfo);
                vmInstanceInfo->vmPId = 0;

                break;
            }
        }
    }
}

