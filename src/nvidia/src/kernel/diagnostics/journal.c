/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/***************************** HW State Routines ***************************\
*                                                                           *
*         RM robust error journal (formerly RCDB)                           *
*                                                                           *
\***************************************************************************/

#include "gpu_mgr/gpu_mgr.h"
#include "nvrm_registry.h"
#include "nvBldVer.h"
#include "nvVer.h"
#include "os/os.h"
#include "core/system.h"
#include "gpu/gpu.h"
#include "diagnostics/journal.h"
#include "platform/chipset/chipset.h"
#include "diagnostics/nv_debug_dump.h"
#include "diagnostics/tracer.h"
#include "core/locks.h"
#include "rmapi/rs_utils.h"
#include "vgpu/rpc.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "nvdevid.h"
#include "nvop.h"
#include "jt.h"



#include "ctrl/ctrl0000/ctrl0000nvd.h"

#include "nvlimits.h"
#include "Nvcm.h"

#include "lib/protobuf/prb_util.h"
#include "g_all_dcl_pb.h"
#include "g_nvdebug_pb.h"
#include "nv_ref.h"

#define NOCAT_UNKNOWN_STR                       "*** unknown ***"
#define NOCAT_NA_STR                            "N/A"
#define NOCAT_LEGACY_STR                        "LEGACY"
#define NOCAT_FULLCHIP_TDR_STR                  "FULL CHIP RESET"
#define NOCAT_BUS_RESET_TDR_STR                 "BUS RESET"
#define NOCAT_GC6_RESET_TDR_STR                 "GC6 RESET"
#define NOCAT_NORMAL_TDR_STR                    "NORMAL TDR"
#define NOCAT_UCODE_RESET_TDR_STR               "UCODE RESET"
#define NOCAT_SURPRISE_REMOVAL_TDR_STR          "SURPRISE REMOVAL"
#define NOCAT_DEFAULT_TAG_VALUE_STR             "prod"
#define NOCAT_DEFAULT_TDR_REASON_SRC_STR        "KMD"
#define NOCAT_FBSIZETESTED                      0x10

// Reducing size to 4K for reducing non-paged allocations on win8
#define NVDUMP_DEBUGGER_BUFFER_SIZE (4 * 1024)

#define JOURNAL_BUFFER_SIZE_DEFAULT    (4 * 1024)

#define JOURNAL_ASSERT_RECORD_QUALIFYING_STACK_ENTRIES    5

static void nvdDebuggerControlFunc(void);

#if (defined(_WIN32) || defined(_WIN64) || defined(NV_UNIX)) && !defined(NV_MODS)
#if !defined(DEBUG) && !defined(QA_BUILD)
static NvBool rcdProbeGpuPresent(OBJGPU *pGpu, NvU64 ip);
static NvBool rcdProbeAllGpusPresent(NvU64 ip);
static volatile NvS32 probeGpuRecursion = 0;
#endif
#endif
static NvU32 _rcdbGetOcaRecordSize(Journal *pRcDB, RMCD_RECORD_TYPE type);
static volatile NvS32 concurrentRingBufferAccess = 0;
static volatile NvS32 assertListRecursion = 0;
static void rcdbFindRingBufferForType(Journal *pRcDB, RMCD_RECORD_TYPE recType, RING_BUFFER_LOG **ppRingBuffer);
static NV_STATUS _rcdbGetNocatJournalRecord(OBJRCDB* pRcdb,
    NvU32 id, NvBool bExactMatch,
    RmRCCommonJournal_RECORD** ppReturnedCommon, RM_NOCAT_JOURNAL_ENTRY** ppReturnedJournal);
static NV_STATUS _rcdbReleaseNocatJournalRecord(RM_NOCAT_JOURNAL_ENTRY* pReturnedJournal);
static NV_STATUS _rcdbNocatReportAssert(OBJGPU* pGpu, RmRCCommonAssert_RECORD* pAssert);

// Global flag to make sure we never re-enter the nvLog code.
#if defined(DEBUG) || defined(ASSERT_BUILD) || defined(QA_BUILD) || ((defined(_WIN32) || defined(_WIN64) || defined(NV_UNIX)) && !defined(NV_MODS))
static volatile NvS32 nvLogRecursion = 0;
#endif

// NvDump interface config - communicates with external kernel debuggers
NVDUMP_EXPORT volatile NV_DECLARE_ALIGNED(NVDUMP_CONFIG, 8) nvDumpConfig =
{
    NVDUMP_CONFIG_SIGNATURE, // sigHead
    NvP64_NULL, // debuggerControlFuncAddr
    { NvP64_NULL, NVDUMP_DEBUGGER_BUFFER_SIZE, 0 }, // buffer
    0, // gpuSelect
    NVDUMP_COMPONENT_SYS_ALL, // component
    NVDUMP_STATUS_IDLE, // dumpStatus
    NV_OK, // rmStatus

    NVDUMP_CONFIG_SIGNATURE // sigTail
};

void
rcdbDestruct_IMPL(Journal *pRcDB)
{
    EVENT_JOURNAL *pJournal = &pRcDB->Journal;

    // Deallocate NvDebug debugger dump buffer.
    if (nvDumpConfig.buffer.address != NvP64_NULL)
    {
        portMemFree(NvP64_VALUE(nvDumpConfig.buffer.address));
        nvDumpConfig.buffer.address = NvP64_NULL;
    }

    // Delete Journal and Btree
    if (pJournal->pBuffer != NULL)
    {
        portMemFree(pJournal->pBuffer);
        portMemFree(pJournal->AssertList.ppList);

        // clear journal of anything
        portMemSet(pJournal, 0, sizeof(EVENT_JOURNAL));
    }

    rcdbClearErrorHistory(pRcDB);

    rcdbDestroyRingBufferCollection(pRcDB);

    portMemFree(pRcDB->previousDriverVersion);
    pRcDB->previousDriverVersion = NULL;

    portMemFree(pRcDB->previousDriverBranch);
    pRcDB->previousDriverBranch = NULL;
}

static void
_initJournal(EVENT_JOURNAL *pJournal, NvU32 size)
{
    // verify we are not abandoning any memory allocations.
    NV_ASSERT(NULL == pJournal->pBuffer);
    NV_ASSERT(NULL == (NvU8*) pJournal->AssertList.ppList);

    // init the Journal to an empty buffer.
    pJournal->pBuffer = NULL;
    pJournal->BufferSize = 0;
    pJournal->pFree = pJournal->pBuffer;
    pJournal->BufferRemaining = pJournal->BufferSize;
    pJournal->pCurrCollection = NULL;
    pJournal->RecordCount = 0;

    // init the assert list to an empty buffer.
    pJournal->AssertList.ppList = NULL;
    pJournal->AssertList.Size = 0;
    pJournal->AssertList.Count = 0;
    pJournal->AssertList.QualifyingStackSize = JOURNAL_ASSERT_RECORD_QUALIFYING_STACK_ENTRIES;

    // allocate and initialize journal buffer memory
    pJournal->pBuffer = portMemAllocNonPaged(size);
    if (pJournal->pBuffer != NULL )
    {
        pJournal->BufferSize = size;
        pJournal->pFree = pJournal->pBuffer;
        pJournal->BufferRemaining = pJournal->BufferSize;

        // if the journal is large enough to hold at least one assert record,
        // init the assert list as well.
        if (sizeof(RmRCCommonAssert_RECORD) <= pJournal->BufferSize)
        {
            pJournal->AssertList.Size = pJournal->BufferSize / sizeof(RmRCCommonAssert_RECORD);
            pJournal->AssertList.ppList = portMemAllocNonPaged(pJournal->AssertList.Size * sizeof(pJournal->AssertList.ppList[0]));
            if (pJournal->AssertList.ppList == NULL )
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Failure to allocate RC assert tracking buffer \n");
                pJournal->AssertList.Size = 0;
            }
        }
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "Failure to allocate RC journal buffer \n");
    }
}

NV_STATUS
rcdbConstruct_IMPL(Journal *pRcDB)
{
    EVENT_JOURNAL *pJournal = &pRcDB->Journal;
    RING_BUFFER_LOG_COLLECTION *pRingBufferColl = &pRcDB->RingBufferColl;
    NvU32 i;
    void *pBuffer;

    // Time parameters
    NvU32 sec, usec;
    NvU64 timeStamp;
    NvU64 systemTime;
    NvU64 timeStampFreq;

    _initJournal(pJournal, JOURNAL_BUFFER_SIZE_DEFAULT);

    portMemSet(pRingBufferColl, 0x00, sizeof(pRcDB->RingBufferColl));

    pRcDB->BugcheckCount = 0;

    // Allocate NvDebug debugger dump buffer.
    pBuffer = portMemAllocNonPaged(nvDumpConfig.buffer.size);
    if (pBuffer != NULL)
    {
        nvDumpConfig.buffer.address = NV_SIGN_EXT_PTR_TO_NvP64(pBuffer);
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR,
                  "failed to allocate NVD debugger dump buffer\n");
    }

    // Initialize NvDebug debugger function address.
    nvDumpConfig.debuggerControlFuncAddr = NV_SIGN_EXT_PTR_TO_NvP64(nvdDebuggerControlFunc);

    //
    // Create RC Diagnostic report Wrap Buffer
    //
    if (NULL == rcdbCreateRingBuffer(pRcDB, RmRcDiagReport, MAX_RCDB_RCDIAG_WRAP_BUFF))
    {
        NV_PRINTF(LEVEL_ERROR, "failed to allocate RC Diagnostic Ring Buffer\n");
    }
    // init the RC error report data
    pRcDB->RcErrRptNextIdx = 0;
    pRcDB->RcErrRptRecordsDropped = NV_FALSE;

    // Initialize RC Error Counters.
    for ( i = 0  ;  i < MAX_RC_ERROR_COUNTER  ;  i++)
    {
        pRcDB->rcErrorCounterArray[i].rcErrorType  = RC_ERROR_COUNTER_TYPE_INVALID;
        pRcDB->rcErrorCounterArray[i].rcErrorCount = 0;
        pRcDB->rcErrorCounterArray[i].rcLastCHID   = INVALID_CHID;
        pRcDB->rcErrorCounterArray[i].rcLastTime   = 0;
    }
     pRcDB->rcErrorCounterArray[RC_ERROR_COUNTER_OTHER_INDEX].rcErrorType  = RC_ERROR_COUNTER_OTHER_TYPE;

     // clear the Nocat Queue descriptors & counters
     portMemSet(&pRcDB->nocatJournalDescriptor, 0x00, sizeof(pRcDB->nocatJournalDescriptor));
     portMemSet(pRcDB->nocatJournalDescriptor.lastRecordId, 0xff, sizeof(pRcDB->nocatJournalDescriptor.lastRecordId));
     pRcDB->nocatJournalDescriptor.nocatLastRecordType = NV2080_NOCAT_JOURNAL_REC_TYPE_UNKNOWN;
     pRcDB->nocatJournalDescriptor.cacheFreshnessPeriodticks = NOCAT_CACHE_FRESHNESS_PERIOD_MS;
     pRcDB->nocatJournalDescriptor.cacheFreshnessPeriodticks *= osGetTimestampFreq();
     pRcDB->nocatJournalDescriptor.cacheFreshnessPeriodticks /= 1000ULL;

     //
     // Create NOCAT report Wrap Buffer
     //
     if (NULL == rcdbCreateRingBuffer(pRcDB, RmNocatReport, MAX_RCDB_NOCAT_WRAP_BUFF))
     {
         NV_PRINTF(LEVEL_ERROR, "failed to allocate NOCAT Ring Buffer\n");
     }

     // Save params for timestamp conversion
     timeStampFreq = osGetTimestampFreq();
     timeStamp = osGetTimestamp();
     osGetCurrentTime(&sec, &usec);
     systemTime = ((NvU64)sec * 1000000) + (NvU64)usec;

     pRcDB->systemTimeReference = systemTime - ((timeStamp * 1000000) / timeStampFreq);
     pRcDB->timeStampFreq = timeStampFreq;

     return NV_OK;
}

//
// Retrieve the previous driver version from volatile registry entires
// and then save the current driver version for next time.
//
NV_STATUS rcdbSavePreviousDriverVersion_IMPL
(
    OBJGPU  *pGpu,
    Journal *pRcDB
)
{
    NV_STATUS nvStatus = NV_OK;

    NvU32     regEntrySize = 0;
    NvU32     changeListNum = NV_LAST_OFFICIAL_CHANGELIST_NUM;

    // Only run this code only once each time the driver is loaded.
    if (pRcDB->bPrevDriverCodeExecuted)
        return NV_OK;

    pRcDB->bPrevDriverCodeExecuted = NV_TRUE;

    //
    // Get the previous driver version information
    // from volatile registry settings.
    //
    nvStatus = osReadRegistryVolatileSize(pGpu,
        NV_REG_STR_RM_RC_PREV_DRIVER_VERSION, &regEntrySize);

    // Early exit if this platform does not support volatile registry.
    if (nvStatus == NV_ERR_NOT_SUPPORTED)
        return NV_OK;

    if ((NV_OK == nvStatus) && (0 != regEntrySize))
    {
        //
        // Previous driver version is there, so assume all previous driver
        // information is there as well.
        //
        pRcDB->previousDriverVersion = portMemAllocNonPaged(regEntrySize + 1);
        if (pRcDB->previousDriverVersion == NULL)
        {
            nvStatus = NV_ERR_NO_MEMORY;
            DBG_BREAKPOINT();
            goto rcdbSavePreviousDriverVersion_writeRegistry;
        }

        nvStatus = osReadRegistryVolatile(pGpu,
                                     NV_REG_STR_RM_RC_PREV_DRIVER_VERSION,
                                     (NvU8 *)pRcDB->previousDriverVersion,
                                     regEntrySize);
        if (nvStatus != NV_OK)
        {
            DBG_BREAKPOINT();
            goto rcdbSavePreviousDriverVersion_writeRegistry;
        }
        pRcDB->previousDriverVersion[regEntrySize] = 0;

        nvStatus = osReadRegistryVolatileSize(pGpu,
            NV_REG_STR_RM_RC_PREV_DRIVER_BRANCH, &regEntrySize);
        if ((nvStatus != NV_OK) || (0 == regEntrySize))
        {
            DBG_BREAKPOINT();
            goto rcdbSavePreviousDriverVersion_writeRegistry;
        }

        pRcDB->previousDriverBranch = portMemAllocNonPaged(regEntrySize + 1);
        if (pRcDB->previousDriverBranch == NULL)
        {
            nvStatus = NV_ERR_NO_MEMORY;
            DBG_BREAKPOINT();
            goto rcdbSavePreviousDriverVersion_writeRegistry;
        }

        nvStatus = osReadRegistryVolatile(pGpu,
                                         NV_REG_STR_RM_RC_PREV_DRIVER_BRANCH,
                                         (NvU8 *)pRcDB->previousDriverBranch,
                                         regEntrySize);
        if (nvStatus != NV_OK)
        {
            DBG_BREAKPOINT();
            goto rcdbSavePreviousDriverVersion_writeRegistry;
        }
        pRcDB->previousDriverBranch[regEntrySize] = 0;

        nvStatus = osReadRegistryVolatile(pGpu,
                                     NV_REG_STR_RM_RC_PREV_DRIVER_CHANGELIST,
                                     (NvU8 *)&pRcDB->prevDriverChangelist,
                                     sizeof(pRcDB->prevDriverChangelist));
        if (nvStatus != NV_OK)
        {
            DBG_BREAKPOINT();
            goto rcdbSavePreviousDriverVersion_writeRegistry;
        }

        nvStatus = osReadRegistryVolatile(pGpu,
                                     NV_REG_STR_RM_RC_PREV_DRIVER_LOAD_COUNT,
                                     (NvU8 *)&pRcDB->driverLoadCount,
                                     sizeof(pRcDB->driverLoadCount));
        if (nvStatus != NV_OK)
        {
            DBG_BREAKPOINT();
            goto rcdbSavePreviousDriverVersion_writeRegistry;
        }
    }

    // Always write out the driver info, even if there was an error reading it.
rcdbSavePreviousDriverVersion_writeRegistry:
    pRcDB->driverLoadCount++;

    osWriteRegistryVolatile(pGpu,
                            NV_REG_STR_RM_RC_PREV_DRIVER_VERSION,
                            (NvU8 *)NV_VERSION_STRING,
                            sizeof(NV_VERSION_STRING));

    osWriteRegistryVolatile(pGpu,
                            NV_REG_STR_RM_RC_PREV_DRIVER_BRANCH,
                            (NvU8 *)NV_BUILD_BRANCH_VERSION,
                            sizeof(NV_BUILD_BRANCH_VERSION));

    osWriteRegistryVolatile(pGpu,
                            NV_REG_STR_RM_RC_PREV_DRIVER_CHANGELIST,
                            (NvU8 *)&changeListNum,
                            sizeof(changeListNum));

    osWriteRegistryVolatile(pGpu,
                            NV_REG_STR_RM_RC_PREV_DRIVER_LOAD_COUNT,
                            (NvU8 *)&pRcDB->driverLoadCount,
                            sizeof(pRcDB->driverLoadCount));

    return nvStatus;
}

NV_STATUS rcdbAddAssertJournalRecWithLine(void *pVoidGpu, NvU32 lineNum, void** ppRec, NvU8 jGroup, NvU8 type, NvU16 size, NvU32 level, NvU64 key)
{
    OBJSYS                     *pSys;
    Journal                    *pRcDB;
    OBJGPU                     *pPossibleNULLGpu;
    JOURNAL_ASSERT_LIST        *pAssertList;
    RmRCCommonAssert_RECORD     newAssertRec;
    RmRCCommonAssert_RECORD    *pAssertRec;
    NV_STATUS                   rmStatus = NV_ERR_GENERIC;
    NvU32                       i;

    //
    // Note: we allow NULL pGpu here, as many clients (such as KMD)
    // do not have access to pGpu.  And much of the RM does not provide this either.
    //
    pPossibleNULLGpu = reinterpretCast(pVoidGpu, OBJGPU *);

    pSys = SYS_GET_INSTANCE();
    if (!pSys)
    {
        return NV_ERR_INVALID_STATE;
    }

    pRcDB = SYS_GET_RCDB(pSys);
    if (!pRcDB)
    {
        return NV_ERR_INVALID_STATE;
    }

    pAssertList = &pRcDB->Journal.AssertList;

    *ppRec = NULL;

    RMTRACE_PROBE4_PRIMTYPE(rcjournal, assertlog, NvU32, (pPossibleNULLGpu ? pPossibleNULLGpu->gpuId : 0), NvU8, type, NvU32, level, NvU64, key);

    // create a local instance of the Assert record.
    portMemSet(&newAssertRec, 0x00, sizeof(newAssertRec));
    rcdbSetCommonJournalRecord(pPossibleNULLGpu, &newAssertRec.common);
    newAssertRec.count = 1;
    newAssertRec.breakpointAddrHint = key;
    newAssertRec.lineNum = lineNum;

    if (pRcDB->getProperty(pRcDB, PDB_PROP_RCDB_COMPRESS))
    {
        // search for a pre-existing assert record with the same stack
        for (i = 0; i < pAssertList->Count; ++i)
        {
            pAssertRec = pAssertList->ppList[i];
            if ((newAssertRec.breakpointAddrHint == pAssertRec->breakpointAddrHint) &&
                (0 == portMemCmp(newAssertRec.callStack, pAssertRec->callStack,
                    sizeof(newAssertRec.callStack[0]) * pAssertList->QualifyingStackSize)))
            {
                pAssertRec->count++;
                pAssertRec->lastTimeStamp = newAssertRec.common.timeStamp;

                rmStatus = NV_OK;
                break;
            }
        }
    }

    if (rmStatus != NV_OK)
    {
        // Discard to avoid reentry from messing up record array.
        if (portAtomicIncrementS32(&assertListRecursion) == 1)
        {
            rmStatus = rcdbAllocNextJournalRec(pRcDB, (NVCD_RECORD **)&pAssertRec, jGroup, type, size);
            if (NV_OK == rmStatus)
            {
                // the Header is filled in when the record is allocated, so update the local instance header.
                newAssertRec.common.Header = pAssertRec->common.Header;
                *pAssertRec = newAssertRec;
                if (pAssertList->Count < pAssertList->Size)
                {
                    pAssertList->ppList[pAssertList->Count] = pAssertRec;
                    ++(pAssertList->Count);
                }
                else
                {
                    // based on the way the assert list size is calculated this should never happen....
                    NV_PRINTF(LEVEL_ERROR,
                              "failed to insert tracking for assert record\n");
                }
            }
        }
        portAtomicDecrementS32(&assertListRecursion);
    }

    if (rmStatus == NV_OK)
    {
        RMTRACE_RMJOURNAL(_ASSERTLOG, (pPossibleNULLGpu ? pPossibleNULLGpu->gpuId : RMTRACE_UNKNOWN_GPUID),
                                      type,
                                      jGroup,
                                      key,
                                      pAssertRec->count,
                                      pAssertRec->common.timeStamp,
                                      pAssertRec->lastTimeStamp);
        *ppRec = pAssertRec;

        _rcdbNocatReportAssert(pPossibleNULLGpu, pAssertRec);
    }
    else
    {
        _rcdbNocatReportAssert(pPossibleNULLGpu, &newAssertRec);
    }

    return rmStatus;
}

NV_STATUS rcdbAddAssertJournalRec(void *pVoidGpu, void** ppRec, NvU8 jGroup, NvU8 type, NvU16 size, NvU32 level, NvU64 key)
{
    return rcdbAddAssertJournalRecWithLine(pVoidGpu, NV_RM_ASSERT_UNKNOWN_LINE_NUM, ppRec, jGroup, type, size, level, key);
}
// Populate stateMask with flags that represent the power state and other useful things.
static NvU64 _getCommonJournalStateMask(OBJGPU *pGpu)
{
    NvU64 stateMask = REF_NUM(NV_RM_JOURNAL_STATE_MASK_GC6_STATE,
        pGpu->gc6State.currentState);

    if (!gpuIsGpuFullPower(pGpu))
        stateMask |= NV_RM_JOURNAL_STATE_MASK_IS_NOT_FULL_POWER;

    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_CONNECTED))
        stateMask |= NV_RM_JOURNAL_STATE_MASK_IS_NOT_CONNECTED;

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_STANDBY))
        stateMask |= NV_RM_JOURNAL_STATE_MASK_IS_IN_STANDBY;

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_HIBERNATE))
        stateMask |= NV_RM_JOURNAL_STATE_MASK_IS_IN_HIBERNATE;

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_PM_CODEPATH))
        stateMask |= NV_RM_JOURNAL_STATE_MASK_IS_IN_PM_CODEPATH;

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_GC6_RESET))
        stateMask |= NV_RM_JOURNAL_STATE_MASK_IS_IN_GC6_RESET;

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_FULLCHIP_RESET))
        stateMask |= NV_RM_JOURNAL_STATE_MASK_IS_IN_FULLCHIP_RESET;

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_SECONDARY_BUS_RESET))
        stateMask |= NV_RM_JOURNAL_STATE_MASK_IS_IN_SEC_BUS_RESET;

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_TIMEOUT_RECOVERY))
        stateMask |= NV_RM_JOURNAL_STATE_MASK_IS_IN_TIMEOUT_RECOVERY;

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_LOST))
        stateMask |= NV_RM_JOURNAL_STATE_MASK_IS_LOST;

    return stateMask;
}

// Fill in the common portion of the journal structure.
void
rcdbSetCommonJournalRecord
(
    OBJGPU *pGpu,
    RmRCCommonJournal_RECORD *pRec
)
{
    OS_THREAD_HANDLE threadId;

    pRec->timeStamp = osGetTimestamp();
    pRec->GPUTag    = 0;
    pRec->CPUTag    = 0;
    pRec->stateMask = 0;

    if (pGpu)
    {
        pRec->GPUTag    = pGpu->gpuId;
        pRec->stateMask = _getCommonJournalStateMask(pGpu);
    }

    if (NV_OK == osGetCurrentThread(&threadId))
    {
        pRec->CPUTag = (NvU64)threadId;
    }
}

NV_STATUS
rcdbAddBugCheckRec_IMPL
(
    OBJGPU  *pGpu,
    Journal *pRcDB,
    NvU32    bugCheckCode
)
{
    RmJournalBugcheck_RECORD *pRec;
    NV_STATUS                 rmStatus;

    rmStatus = rcdbAllocNextJournalRec(pRcDB,
                                       (NVCD_RECORD **)&pRec,
                                       RmGroup,
                                       RmJournalBugCheck,
                                       sizeof(*pRec));
    if (NV_OK == rmStatus)
    {
        rcdbSetCommonJournalRecord(pGpu, &pRec->common);
        pRec->bugCheckCode = bugCheckCode;
    }

     pRcDB->BugcheckCount++;

    return rmStatus;
}

NV_STATUS
rcdbAddPowerStateRec_IMPL
(
    OBJGPU  *pGpu,
    Journal *pRcDB,
    NvU32    powerEvent,
    NvU32    state,
    NvU32    fastBootPowerState
)
{
    RmPowerState_RECORD       newRmDiagWrapBuffRec;

    // Create Records, then write it.
    newRmDiagWrapBuffRec.powerState = state;
    newRmDiagWrapBuffRec.powerEvent = powerEvent;
    newRmDiagWrapBuffRec.fastBootPowerState = fastBootPowerState;
    rcdbAddRecToRingBuffer(pGpu, pRcDB, RmPowerState,
                              sizeof(RmPowerState_RECORD), (NvU8 *)&newRmDiagWrapBuffRec);
    return NV_OK;
}

NV_STATUS
rcdbGetRcDiagRecBoundaries_IMPL
(
    Journal  *pRcDB,
    NvU16    *pStart,
    NvU16    *pEnd,
    NvU32     owner,
    NvU32     processId
)
{
    NV_STATUS                   status = NV_ERR_MISSING_TABLE_ENTRY;
    RmRCCommonJournal_RECORD   *pCommon;
    RmRcDiag_RECORD            *pRecord = NULL;
    RING_BUFFER_LOG            *pRingBuffer = NULL;
    NvU32                       i;
    NvU16                       logicalStartIdx;
    NvU16                       start = 0;
    NvU16                       end = 0;
    NvBool                      foundStart = NV_FALSE;
    NvBool                      foundEnd = NV_FALSE;

    // scan the buffer to find all the qualified records & return the
    // first & last indicies of the qualified records found.

    // Get the Diag Report Ring buffer.
    rcdbFindRingBufferForType(pRcDB, RmRcDiagReport, &pRingBuffer);

    // attempt to claim ownership
    if (portAtomicIncrementS32(&concurrentRingBufferAccess) == 1)
    {
        // get the logical start of the buffer.
        logicalStartIdx = pRingBuffer->headIndex;

        // run thru all the entries in the buffer, start to end, until we find the start & end of the range we are looking for.
        for (i = 0; i < pRingBuffer->numEntries; ++i)
        {
            // get a pointer to the record from the buffer.
            pCommon = (RmRCCommonJournal_RECORD *)(((NvU8 *)pRingBuffer->pBuffer) + (rcdbGetOcaRecordSizeWithHeader(pRcDB, RmRcDiagReport) * ((logicalStartIdx + i) % pRingBuffer->maxEntries)));
            pRecord = (RmRcDiag_RECORD*) &(pCommon[1]);

            // check to see if the record qualifies
            if (((RCDB_RCDIAG_DEFAULT_OWNER != owner) && (pRecord->owner != owner) && (NV0000_CTRL_CMD_NVD_RCERR_RPT_ANY_OWNER_ID != owner))
                || ((NV0000_CTRL_CMD_NVD_RCERR_RPT_ANY_PROCESS_ID != processId) && (pRecord->processId != processId)))
            {
                continue;
            }
            switch (foundStart)
            {
            case NV_FALSE:
                // check if this is a start record.
                // we want the first record to be a start record to insure that all the reports that are in the range are complete
                // (I.E. we didn't wrap over the first record of a report)
                if (0 != (pRecord->flags & NV0000_CTRL_CMD_NVD_RCERR_RPT_FLAGS_POS_FIRST))
                {
                    // yes save the idx as the first Idx, & note that we found the start of the range.
                    start = pRecord->idx;
                    foundStart = NV_TRUE;
                }
                // fall thru to check if the start of the report is also the end of the report.

            case NV_TRUE:
                // check if this is an end record.
                // we want the last record in the range to be an end record to insure that all the reports that are in the range are complete
                // (Note -- in the case of end records, this should only be an issue if we are interrupting the collection of a report)
                if (0 != (pRecord->flags & NV0000_CTRL_CMD_NVD_RCERR_RPT_FLAGS_POS_LAST))
                {
                    // save the idx as the last idx & continue scanning until we have checked all the records.
                    // the last idx saved will be the last idx.
                    end = pRecord->idx;
                    foundEnd = foundStart;
                }
                break;
            }
        }
        // checking end is sufficient, because end can't be set w/o start being set first.
        if (foundEnd)
        {
            // we found a complete range, mark us as succeeding.
            status = NV_OK;

            // pass up the results.
            if (NULL != pEnd)
            {
                *pEnd = end;
            }
            if (NULL != pStart)
            {
                *pStart = start;
            }
        }
    }
    else
    {
        // the buffer is currently busy.
        status = NV_ERR_BUSY_RETRY;
    }
    portAtomicDecrementS32(&concurrentRingBufferAccess);
    return status;
}

RmRCCommonJournal_RECORD *
rcdbAddRcDiagRec_IMPL
(
    OBJGPU  *pGpu,
    Journal *pRcDB,
    RmRcDiag_RECORD       *pRmDiagWrapBuffRec
)
{
    RmRCCommonJournal_RECORD *pCommon;
    NvU32   usec;

    // Create Records, then write it.
    pRmDiagWrapBuffRec->idx = (pRcDB->RcErrRptNextIdx)++;
    if (MAX_RCDB_RCDIAG_ENTRIES < pRmDiagWrapBuffRec->count)
    {
        NV_ASSERT_FAILED("Diag report to large for buffer");
        pRmDiagWrapBuffRec->data[MAX_RCDB_RCDIAG_ENTRIES - 1].offset = 0;
        pRmDiagWrapBuffRec->data[MAX_RCDB_RCDIAG_ENTRIES - 1].tag = NV0000_CTRL_CMD_NVD_RCERR_RPT_REG_OVERFLOWED;
        pRmDiagWrapBuffRec->data[MAX_RCDB_RCDIAG_ENTRIES - 1].value = pRmDiagWrapBuffRec->count - MAX_RCDB_RCDIAG_ENTRIES + 1;
        pRmDiagWrapBuffRec->count = MAX_RCDB_RCDIAG_ENTRIES;
    }
    osGetCurrentTime(&(pRmDiagWrapBuffRec->timeStamp), &usec);

    pCommon = rcdbAddRecToRingBuffer(pGpu, pRcDB, RmRcDiagReport,
                                     sizeof(RmRcDiag_RECORD), (NvU8 *)pRmDiagWrapBuffRec);

    pRcDB->RcErrRptRecordsDropped |= pRcDB->RcErrRptNextIdx >= MAX_RCDB_RCDIAG_WRAP_BUFF;
    return pCommon;
}

RmRCCommonJournal_RECORD *
rcdbAddRcDiagRecFromGsp_IMPL
(
    OBJGPU  *pGpu,
    Journal *pRcDB,
    RmRCCommonJournal_RECORD   *pCommonGsp,
    RmRcDiag_RECORD            *pRmDiagGsp
)
{
    RmRCCommonJournal_RECORD   *pCommonCpu;

    pCommonCpu = rcdbAddRcDiagRec(pGpu, pRcDB, pRmDiagGsp);
    if (pCommonCpu)
    {
        NV_ASSERT(pCommonCpu->GPUTag == pCommonGsp->GPUTag);
        pCommonCpu->stateMask |= pCommonGsp->stateMask;
    }

    return pCommonCpu;
}

NV_STATUS
_rcdbInternalGetRcDiagRec
(
    Journal                    *pRcDB,
    NvU16                       reqIdx,
    RmRCCommonJournal_RECORD  **ppRmDiagWrapBuffRec,
    NvU32                       owner,
    NvU32                       processId
)
{
    RmRCCommonJournal_RECORD   *pCommon;
    RmRcDiag_RECORD*            pRecord = NULL;
    NV_STATUS                   status = NV_ERR_INVALID_INDEX;
    RING_BUFFER_LOG            *pRingBuffer = NULL;

    NvU32                       i;

    // assume we will fail.
    *ppRmDiagWrapBuffRec = NULL;

    // Find the ring buffer for the diag reports
    rcdbFindRingBufferForType(pRcDB, RmRcDiagReport, &pRingBuffer);

    // is the requested record in the buffer?
    if ((NvU16)(pRcDB->RcErrRptNextIdx - reqIdx) <= pRingBuffer->numEntries)
    {
        // calculate the location of the record.
        // find the record just past the last record in the buffer. to use as the initial offset.
        i = pRingBuffer->headIndex + pRingBuffer->numEntries;

        // subtract off the diff between the next idx to be used & the requested idx.
        i -= pRcDB->RcErrRptNextIdx - reqIdx;

        // wrap the offset to the size of the buffer.
        i %= pRingBuffer->maxEntries;

        // get a pointer to the record from the buffer.
        pCommon = (RmRCCommonJournal_RECORD *)(((NvU8 *)pRingBuffer->pBuffer) + (rcdbGetOcaRecordSizeWithHeader(pRcDB, RmRcDiagReport) * i));
        pRecord = (RmRcDiag_RECORD*) &(pCommon[1]);

        // verify we have the record that was requested.
        NV_ASSERT_OR_RETURN(pRecord->idx == reqIdx, NV_ERR_INVALID_INDEX);

        // we found the requested Index,
        // check to see if the record qualifies
        if (((RCDB_RCDIAG_DEFAULT_OWNER == owner) || (pRecord->owner == owner) || (NV0000_CTRL_CMD_NVD_RCERR_RPT_ANY_OWNER_ID == owner))
            && ((NV0000_CTRL_CMD_NVD_RCERR_RPT_ANY_PROCESS_ID == processId) || (pRecord->processId == processId)))
        {
            // combination of ANY_OWNER_ID && ANY_PROCESS_ID is not valid
            if (NV0000_CTRL_CMD_NVD_RCERR_RPT_ANY_OWNER_ID == owner && NV0000_CTRL_CMD_NVD_RCERR_RPT_ANY_PROCESS_ID == processId)
            {
                status = NV_ERR_INSUFFICIENT_PERMISSIONS;
                goto exit;
            }
            // we found a record that fully qualifies
            *ppRmDiagWrapBuffRec = pCommon;
            status = NV_OK;
        }
        else
        {
            // we found the record, but it does not pass the filter.
            status = NV_ERR_INSUFFICIENT_PERMISSIONS;
        }
    }
exit:
    return status;
}

NV_STATUS
rcdbGetRcDiagRec_IMPL
(
    Journal                    *pRcDB,
    NvU16                       reqIdx,
    RmRCCommonJournal_RECORD  **ppRmDiagWrapBuffRec,
    NvU32                       owner,
    NvU32                       processId
)
{
    NV_STATUS                   status;

    if (ppRmDiagWrapBuffRec == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    *ppRmDiagWrapBuffRec = NULL;

    if (portAtomicIncrementS32(&concurrentRingBufferAccess) == 1)
    {
        status = _rcdbInternalGetRcDiagRec(pRcDB, reqIdx, ppRmDiagWrapBuffRec, owner, processId);
    }
    else
    {
        status = NV_ERR_BUSY_RETRY;
    }
    portAtomicDecrementS32(&concurrentRingBufferAccess);
    return status;
}

//
//  The function to set context data for all the RmRcDiag_RECORDs in a specified range
//
NV_STATUS
rcdbUpdateRcDiagRecContext_IMPL
(
    Journal                    *pRcDB,
    NvU16                       rangeStartIdx,
    NvU16                       rangeEndIdx,
    NvU32                       processId,
    NvU32                       owner
)
{
    RmRCCommonJournal_RECORD   *pCommon = NULL;
    RmRcDiag_RECORD*            pRecord = NULL;
    NV_STATUS                   status = NV_OK;
    NV_STATUS                   recStatus = NV_ERR_OUT_OF_RANGE;

    NvU16                       i;

    // go from the start index thru the end index.
    // note we use != because the indicies will wrap.
    for (i = rangeStartIdx; i != (NvU16)(rangeEndIdx + 1U); i++)
    {
        recStatus = rcdbGetRcDiagRec(pRcDB, i, &pCommon, RCDB_RCDIAG_DEFAULT_OWNER, NV0000_CTRL_CMD_NVD_RCERR_RPT_ANY_PROCESS_ID);
        if (NV_OK != recStatus)
        {
            // something went wrong,
            // record the status & skip this record.
            status = recStatus;
            continue;
        }
        // get the pointer to the diag record.
        pRecord = (RmRcDiag_RECORD*) &(pCommon[1]);

        pRecord->owner = owner;
        pRecord->processId = processId;
    }
    return status;
}

//
// size must include NVCD_RECORD size too
//
NV_STATUS rcdbAllocNextJournalRec_IMPL(Journal *pRcDB, NVCD_RECORD** ppRec, NvU8 jGroup, NvU8 type, NvU16 size)
{
    EVENT_JOURNAL *pJournal = &pRcDB->Journal;

    if ( ppRec == NULL )
        return NV_ERR_GENERIC;

    if ( pJournal->pBuffer == NULL || pJournal->BufferSize == 0 )
        return NV_ERR_GENERIC;

    if ( size == 0 || pJournal->BufferRemaining < size )
    {
        return NV_ERR_GENERIC;
    }

    *ppRec = (NVCD_RECORD*)(pJournal->pFree);

    (*ppRec)->cRecordGroup = jGroup;
    (*ppRec)->cRecordType = type;
    (*ppRec)->wRecordSize = size;

    if ( pJournal->pCurrCollection )
    {
        pJournal->pCurrCollection->NumRecords++;
        pJournal->pCurrCollection->Header.wRecordSize += size;
    }
    else
    {
        // standalone record (not part of collection) - increase total count
        pJournal->RecordCount++;
    }

    pJournal->pFree += size;
    pJournal->BufferRemaining -= size;

    return NV_OK;
}

NV_STATUS rcdbClearErrorHistory_IMPL(Journal *pRcDB)
{
    SYS_ERROR_INFO         *pSysErrorInfo = &pRcDB->ErrorInfo;
    RMFIFOERRORELEMENT_V3* pFifoErrorInfo;
    RMFIFOERRORELEMENT_V3* pFreeErrorInfo;

    // Wait until any errors currently being reported are complete
    while (!portAtomicCompareAndSwapU32(&pSysErrorInfo->InUse, 1, 0))
    {
        // We're not going to sleep, but safe to sleep also means safe to spin..
        NV_ASSERT_OR_RETURN(portSyncExSafeToSleep(), NV_ERR_INVALID_STATE);
        portUtilSpin();
    }

    pFifoErrorInfo = (RMFIFOERRORELEMENT_V3*) pSysErrorInfo->pErrorList;
    while (NULL != pFifoErrorInfo)
    {
        pFreeErrorInfo = pFifoErrorInfo;
        pFifoErrorInfo = pFifoErrorInfo->ErrorHeader.pNextError;
        rcdbDeleteErrorElement(pRcDB, pFreeErrorInfo);
    }

    pSysErrorInfo->ErrorCount = 0x0;
    pSysErrorInfo->LogCount = 0x0;
    pSysErrorInfo->pErrorList = NULL;

    portAtomicSetU32(&pSysErrorInfo->InUse, 0);
    return NV_OK;
}


NV_STATUS rcdbDeleteErrorElement_IMPL(Journal *pRcDB, void *pDelete)
{
    RMFIFOERRORELEMENT_V3* pFifoDelete = (RMFIFOERRORELEMENT_V3*)pDelete;
    RMCD_ERROR_BLOCK*              pErrorBlock;
    RMCD_ERROR_BLOCK*              pOldErrorBlock;

    // Free Additional Error Block
    for (pErrorBlock = pFifoDelete->ErrorHeader.pErrorBlock; pErrorBlock != NULL;)
    {
        pOldErrorBlock = pErrorBlock;
        pErrorBlock = pErrorBlock->pNext;
        portMemFree(pOldErrorBlock->pBlock);
        portMemFree(pOldErrorBlock);
    }

    // Free Error Collector
    portMemFree(pFifoDelete);

    return NV_OK;
}

// Frees up the all the ring buffers
void rcdbDestroyRingBufferCollection_IMPL(Journal *pRcDB)
{
    RING_BUFFER_LOG_COLLECTION *pRingBufferColl = &pRcDB->RingBufferColl;
    NvU32 i;
    RING_BUFFER_LOG* pCurrentBuffer = pRingBufferColl->pFirstEntry;

    for (i = 0; i < pRingBufferColl->NumRingBuffers; i++)
    {
        RING_BUFFER_LOG* pTempCurrentBuffer = pCurrentBuffer;

        NV_ASSERT(pCurrentBuffer != NULL);
        NV_ASSERT(pCurrentBuffer->pBuffer != NULL);

        portMemFree(pCurrentBuffer->pBuffer);

        pCurrentBuffer = pCurrentBuffer->pNextRingBuffer;

        // Free the current ring buffer entry.
        portMemFree(pTempCurrentBuffer);
    }

    // pCurrentBuffer should be NULL if our accounting of NumEntries is correct
    NV_ASSERT(pCurrentBuffer == NULL);

    portMemSet(pRingBufferColl, 0x00, sizeof(*pRingBufferColl));
}


static NvU32 _rcdbInsertJournalRecordToList (RmRCCommonJournal_RECORD *pList, RmRCCommonJournal_RECORD *pRecord);
static void _rcdbDumpCommonJournalRecord(PRB_ENCODER *pPrbEnc,const PRB_FIELD_DESC *pFieldDesc,PRmRCCommonJournal_RECORD pRec);

/*!
 * @brief Initialize the GPU accessible flag
 *
 * @param[in] pGPU
 * @param[in] pRcDB
 *
 * @return NV_OK
 */
NV_STATUS
rcdbDumpInitGpuAccessibleFlag_IMPL
(
    OBJGPU  *pGpu,
    Journal *pRcDB
)
{
    pRcDB->nvDumpState.bGpuAccessible =
        pRcDB->nvDumpState.bRMLock                                    &&
        !pGpu->bIsSOC                                                 &&
        !IS_VIRTUAL(pGpu)                                             &&
        gpuIsGpuFullPower(pGpu)                                       &&
        !pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_FULLCHIP_RESET)      &&
        !pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_SECONDARY_BUS_RESET) &&
        !pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_GC6_RESET)           &&
        !pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_PM_CODEPATH)         &&
        !pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_LOST);

    // The GPU should be there... but make sure.
    if (pRcDB->nvDumpState.bGpuAccessible)
    {
        if (GPU_REG_RD32(pGpu, NV_PMC_BOOT_0) != pGpu->chipId0)
        {
            pRcDB->nvDumpState.bGpuAccessible = NV_FALSE;
        }
    }

    return NV_OK;
}

/*!
 * @brief Performs a dump of the specified system component into the given buffer.
 *
 * @param[in] pSys The system object
 * @param[in] component NVDUMP_IS_SYS_COMPONENT(component) must be true.
 * @param[in, out] pBuffer Buffer to populate with dump results
 * @param[in] policy Policy for buffer allocation: use this one, allocate one or count
 * @param[in, out] pBufferCallback Callback function for use with fixed-sized buffer encoding.
 *                                 If this is NULL then pBuffer->size is assumed to be large
 *                                 enough for the whole dump. Otherwise pBufferCallback is called
 *                                 when the buffer is full or when a message ends, allowing the
 *                                 the callback to construct the whole buffer piece by piece.
 *
 * @return NV_OK on success and specific error status on failure
 */
NV_STATUS
rcdbDumpComponent_IMPL
(
    OBJRCDB *pRcDB,
    NvU32 component,
    NVDUMP_BUFFER *pBuffer,
    NVDUMP_BUFFER_POLICY policy,
    PrbBufferCallback *pBufferCallback
)
{
    NVD_STATE *pNvDumpState = &pRcDB->nvDumpState;
    void *pBuff;
    PRB_ENCODER encoder;
    NV_STATUS status = NV_OK;
    NvU8 startingDepth;

    // Validate arguments.
    NV_ASSERT_OR_RETURN(pBuffer != NULL, NV_ERR_INVALID_ARGUMENT);

    // Make sure we were not reentered.
    if (pNvDumpState->bDumpInProcess)
        return NV_ERR_STATE_IN_USE;

    // Initialize dump state.
    pNvDumpState->bDumpInProcess    = NV_TRUE;
    pNvDumpState->bugCheckCode      = 0;
    pNvDumpState->internalCode      = NVD_ERROR_CODE(NVD_EXTERNALLY_GENERATED, 0);
    pNvDumpState->bRMLock           = rmapiLockIsOwner();
    pNvDumpState->bGpuAccessible    = NV_FALSE;
    pNvDumpState->initialbufferSize = pBuffer->size;
    pNvDumpState->nvDumpType        = NVD_DUMP_TYPE_API;

    // Clear dump buffer.
    pBuffer->curNumBytes = 0;

    // Start encoding protobuf dump message.
    switch (policy)
    {
        case NVDUMP_BUFFER_PROVIDED:
            prbEncStart(&encoder, NVDEBUG_NVDUMP, NvP64_VALUE(pBuffer->address),
                        pBuffer->size, pBufferCallback);
            break;
        case NVDUMP_BUFFER_ALLOCATE:
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                prbEncStartAlloc(&encoder, NVDEBUG_NVDUMP,
                                 pBuffer->size, pBufferCallback));
            break;
        case NVDUMP_BUFFER_COUNT:
            prbEncStartCount(&encoder, NVDEBUG_NVDUMP, NVDUMP_MAX_DUMP_SIZE);
            break;
        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    startingDepth = prbEncNestingLevel(&encoder);

    switch (component)
    {
        case NVDUMP_COMPONENT_SYS_RCDB:
        {
            NV_CHECK_OK(status, LEVEL_ERROR,
                rcdbDumpSystemFunc(pRcDB, &encoder, pNvDumpState));
            break;
        }
        case NVDUMP_COMPONENT_SYS_SYSINFO:
        {
            NV_CHECK_OK(status, LEVEL_ERROR,
                rcdbDumpSystemInfo(pRcDB, &encoder, pNvDumpState));
            break;
        }
        case NVDUMP_COMPONENT_SYS_ALL:
        {
            NV_CHECK_OK(status, LEVEL_ERROR,
                rcdbDumpSystemInfo(pRcDB, &encoder, pNvDumpState));
            NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_ERROR,
                rcdbDumpSystemFunc(pRcDB, &encoder, pNvDumpState));
            break;
        }
        default:
        {
            NV_PRINTF(LEVEL_ERROR,
                      "called with invalid component %u selected.\n",
                      component);
            status = NV_ERR_INVALID_ARGUMENT;
            break;
        }
    }

    NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_ERROR,
        prbEncUnwindNesting(&encoder, startingDepth));

    {
        NvU32   gpu;
        OBJGPU *pGpu;

        for (gpu = 0; gpu < NV_MAX_DEVICES; gpu++)
        {
            pGpu = gpumgrGetGpu(gpu);

            if ((pGpu != NULL) && IS_GSP_CLIENT(pGpu))
            {
                NV_RM_RPC_DUMP_PROTOBUF_COMPONENT(pGpu, status, &encoder,
                    pNvDumpState, component);

                NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_ERROR,
                    prbEncUnwindNesting(&encoder, startingDepth));
            }
        }
    }

    // Finish encoding protobuf dump message.
    pBuffer->curNumBytes = prbEncFinish(&encoder, &pBuff);
    pBuffer->address = NV_SIGN_EXT_PTR_TO_NvP64(pBuff);
    pNvDumpState->bDumpInProcess = NV_FALSE;

    return status;
}

static NV_STATUS
_rcdbGetTimeInfo
(
    PRB_ENCODER          *pPrbEnc,
    NVD_STATE            *pNvDumpState,
    const PRB_FIELD_DESC *pFieldDesc
)
{
    NvU64 timeSinceBoot;
    NvU32 sec;
    NvU32 usec;
    NV_STATUS nvStatus = NV_OK;
    NvU8 startingDepth = prbEncNestingLevel(pPrbEnc);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        prbEncNestedStart(pPrbEnc, pFieldDesc));

    prbEncAddUInt64(pPrbEnc,
                    NVDEBUG_SYSTEMINFO_TIMEINFO_TIMESTAMP_FREQ,
                    osGetTimestampFreq());

    // Add Timestamp
    prbEncAddUInt64(pPrbEnc,
                    NVDEBUG_SYSTEMINFO_TIMEINFO_TIMESTAMP_DUMP,
                    osGetTimestamp());
    osGetCurrentTime(&sec, &usec);
    prbEncAddUInt64(pPrbEnc,
                    NVDEBUG_SYSTEMINFO_TIMEINFO_SYSTEM_TIME_DUMP,
                    (NvU64)sec * 1000000 + usec);

    // Add time since boot in seconds.
    osGetCurrentTick(&timeSinceBoot);
    prbEncAddUInt32(pPrbEnc,
                    NVDEBUG_SYSTEMINFO_TIMEINFO_TIME_SINCE_BOOT_SEC,
                    (NvU32)(timeSinceBoot / 1000000000ULL));

    // Unwind the protobuf to the correct depth.
    NV_CHECK_OK(nvStatus, LEVEL_ERROR,
        prbEncUnwindNesting(pPrbEnc, startingDepth));

    return nvStatus;
}

static NV_STATUS
_rcdbGetResourceServerData
(
    PRB_ENCODER          *pPrbEnc,
    NVD_STATE            *pNvDumpState,
    const PRB_FIELD_DESC *pFieldDesc
)
{
    NV_STATUS nvStatus = NV_OK;
    NvU8 startingDepth = prbEncNestingLevel(pPrbEnc);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        prbEncNestedStart(pPrbEnc, pFieldDesc));

    prbEncAddUInt32(pPrbEnc,
                    NVDEBUG_SYSTEMINFO_RESOURCESERVER_NUM_CLIENTS,
                    serverGetClientCount(&g_resServ));
    prbEncAddUInt64(pPrbEnc,
                    NVDEBUG_SYSTEMINFO_RESOURCESERVER_NUM_RESOURCES,
                    serverGetResourceCount(&g_resServ));

    for (RmClient **ppClient = serverutilGetFirstClientUnderLock();
         ppClient;
         ppClient = serverutilGetNextClientUnderLock(ppClient))
    {
        if (*ppClient == NULL)
            continue;

        RmClient *pRmClient = *ppClient;
        RsClient *pRsClient = staticCast(pRmClient, RsClient);

        NV_CHECK_OK_OR_GOTO(nvStatus, LEVEL_ERROR,
            prbEncNestedStart(pPrbEnc, NVDEBUG_SYSTEMINFO_RESOURCESERVER_CLIENT_INFO),
            out);

        prbEncAddUInt32(pPrbEnc,
            NVDEBUG_SYSTEMINFO_RESOURCESERVER_CLIENTINFO_CLIENT_HANDLE,
            pRsClient->hClient);
        prbEncAddUInt32(pPrbEnc,
            NVDEBUG_SYSTEMINFO_RESOURCESERVER_CLIENTINFO_PROCESS_ID,
            pRmClient->ProcID);
        prbEncAddString(pPrbEnc,
            NVDEBUG_SYSTEMINFO_RESOURCESERVER_CLIENTINFO_PROCESS_NAME,
            pRmClient->name);
        prbEncAddUInt32(pPrbEnc,
            NVDEBUG_SYSTEMINFO_RESOURCESERVER_CLIENTINFO_FLAGS,
            pRmClient->Flags);
        prbEncAddUInt32(pPrbEnc,
            NVDEBUG_SYSTEMINFO_RESOURCESERVER_CLIENTINFO_PRIV_LEVEL,
            (NvU32)pRmClient->cachedPrivilege);

        RS_ITERATOR it = clientRefIter(pRsClient, 0, 0, RS_ITERATE_DESCENDANTS, NV_TRUE);
        while (clientRefIterNext(it.pClient, &it))
        {
            if (it.pResourceRef == NULL || it.pResourceRef->pResource == NULL)
                continue;

            RsResource *pRes = it.pResourceRef->pResource;
            RmResource *pRmRes = dynamicCast(pRes, RmResource);

            NV_CHECK_OK_OR_GOTO(nvStatus, LEVEL_ERROR,
                prbEncNestedStart(pPrbEnc, NVDEBUG_SYSTEMINFO_RESOURCESERVER_CLIENTINFO_ALLOCATIONS),
                out);

            prbEncAddUInt32(pPrbEnc,
                NVDEBUG_SYSTEMINFO_RESOURCESERVER_CLIENTINFO_CLIENTALLOCATION_OBJECT_HANDLE,
                it.pResourceRef->hResource);
            prbEncAddUInt32(pPrbEnc,
                NVDEBUG_SYSTEMINFO_RESOURCESERVER_CLIENTINFO_CLIENTALLOCATION_OBJECT_CLASS_ID,
                it.pResourceRef->externalClassId);
            prbEncAddUInt32(pPrbEnc,
                NVDEBUG_SYSTEMINFO_RESOURCESERVER_CLIENTINFO_CLIENTALLOCATION_PARENT_HANDLE,
                ((it.pResourceRef->pParentRef != NULL) ? it.pResourceRef->pParentRef->hResource : 0));
            prbEncAddUInt32(pPrbEnc,
                NVDEBUG_SYSTEMINFO_RESOURCESERVER_CLIENTINFO_CLIENTALLOCATION_GPU_INSTANCE,
                ((pRmRes != NULL) ? pRmRes->rpcGpuInstance : 0xFFFFFFFF));

            prbEncNestedEnd(pPrbEnc);
        }
        prbEncNestedEnd(pPrbEnc);
    }

    prbEncNestedEnd(pPrbEnc);

out:
    // Unwind the protobuf to the correct depth.
    NV_CHECK_OK(nvStatus, LEVEL_ERROR,
        prbEncUnwindNesting(pPrbEnc, startingDepth));

    return nvStatus;
}

static const char * GPU_NA_UUID = "N/A";

NV_STATUS
rcdbDumpSystemInfo_IMPL
(
    OBJRCDB *pRcDB,
    PRB_ENCODER *pPrbEnc,
    NVD_STATE   *pNvDumpState
)
{
    OBJGPU     *pGpu;
    NvU8       *pGidString;
    NvU32       gpu;
    NvU32       numGpus;
    NvU32       gidStrlen;
    NvU32       sizeStr;
    NV_STATUS   nvStatus = NV_OK;
    NvBool      bRelease;
    NvU8        startingDepth = prbEncNestingLevel(pPrbEnc);

    OBJSYS     *pSys = SYS_GET_INSTANCE();
    OBJCL      *pCl = SYS_GET_CL(pSys);
    OBJGPU     *pParent;
    NvU32       gpuIndex;
    NvU32       gpuMask;
    NvBool      bGpuDone[NV_MAX_DEVICES];

    // All of this stuff should run OK even without the RM lock.
    // No need to check pRcDB->nvDumpState.bNoRMLock;

    switch (DRF_VAL(_NVD, _ERROR_CODE, _MAJOR, pNvDumpState->internalCode))
    {
    case NVD_GPU_GENERATED:
    case NVD_SKIP_ZERO:
        // don't report on these internal codes.
        return NV_OK;
        break;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        prbEncNestedStart(pPrbEnc, NVDEBUG_NVDUMP_SYSTEM_INFO));

    NV_CHECK_OK_OR_GOTO(nvStatus, LEVEL_ERROR,
        _rcdbGetTimeInfo(pPrbEnc, pNvDumpState, NVDEBUG_SYSTEMINFO_TIME_INFO),
        External_Cleanup);

    NV_CHECK_OK_OR_GOTO(nvStatus, LEVEL_ERROR,
        _rcdbGetResourceServerData(pPrbEnc, pNvDumpState, NVDEBUG_SYSTEMINFO_RESSERV_INFO),
        External_Cleanup);

    prbEncAddUInt32(pPrbEnc,
                    NVDEBUG_SYSTEMINFO_BUGCHECK_COUNT,
                    pRcDB->BugcheckCount);

    // Add NorthBridge Info
    NV_CHECK_OK_OR_GOTO(nvStatus, LEVEL_ERROR,
        prbEncNestedStart(pPrbEnc, NVDEBUG_SYSTEMINFO_NORTHBRIDGE_INFO),
        External_Cleanup);

    prbEncAddUInt32(pPrbEnc,
        NVDEBUG_SYSTEMINFO_NORTHBRIDGEINFO_ID,
        pCl->FHBBusInfo.vendorID |
        (pCl->FHBBusInfo.deviceID << 16));

    prbEncAddUInt32(pPrbEnc,
        NVDEBUG_SYSTEMINFO_NORTHBRIDGEINFO_SSID,
        pCl->FHBBusInfo.subvendorID |
        (pCl->FHBBusInfo.subdeviceID << 16));

    NV_CHECK_OK_OR_GOTO(nvStatus, LEVEL_ERROR, // NVDEBUG_SYSTEMINFO_NORTHBRIDGE_INFO
        prbEncNestedEnd(pPrbEnc),
        External_Cleanup);

    //CPU Info
    NV_CHECK_OK_OR_GOTO(nvStatus, LEVEL_ERROR,
        prbEncNestedStart(pPrbEnc, NVDEBUG_SYSTEMINFO_CPU_INFO),
        External_Cleanup);

    prbEncAddUInt32(pPrbEnc,
        NVDEBUG_SYSTEMINFO_CPUINFO_CPU_TYPE,
        pSys->cpuInfo.type);

    prbEncAddUInt32(pPrbEnc,
        NVDEBUG_SYSTEMINFO_CPUINFO_CPU_CAPS,
        pSys->cpuInfo.caps);

    prbEncAddUInt32(pPrbEnc,
        NVDEBUG_SYSTEMINFO_CPUINFO_NUM_CPU_CORES,
        pSys->cpuInfo.numPhysicalCpus);

    prbEncAddUInt32(pPrbEnc,
        NVDEBUG_SYSTEMINFO_CPUINFO_NUM_LOGICAL_CPUS,
        pSys->cpuInfo.numLogicalCpus);

    NV_CHECK_OK_OR_GOTO(nvStatus, LEVEL_ERROR, // NVDEBUG_SYSTEMINFO_CPU_INFO
        prbEncNestedEnd(pPrbEnc),
        External_Cleanup);

    //GPU Info
    NV_CHECK_OK_OR_GOTO(nvStatus, LEVEL_ERROR,
        prbEncNestedStart(pPrbEnc, NVDEBUG_SYSTEMINFO_GPU_INFO),
        External_Cleanup);

    // Count the number of GPUs and List the gpuIds
    numGpus = 0;
    for (gpu = 0; gpu < NV_MAX_DEVICES; gpu++)
    {
        const NvU32 gidFlags =
            DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _FORMAT, _BINARY) |
            DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _TYPE, _SHA1);

        pGpu = gpumgrGetGpu(gpu);

        if (pGpu)
        {
            numGpus++;

            prbEncAddUInt32(pPrbEnc,
                NVDEBUG_SYSTEMINFO_GPUINFO_GPU_ID,
                pGpu->gpuId);

            nvStatus = gpuGetGidInfo(pGpu, &pGidString,
                &gidStrlen, gidFlags);
            if (NV_OK == nvStatus)
            {
                prbEncAddBytes(pPrbEnc,
                    NVDEBUG_SYSTEMINFO_GPUINFO_GPU_UUID,
                    pGidString, gidStrlen);
                portMemFree(pGidString);
            }
            else if (pGpu->gpuUuid.isInitialized)
            {
                prbEncAddBytes(pPrbEnc,
                    NVDEBUG_SYSTEMINFO_GPUINFO_GPU_UUID,
                    pGpu->gpuUuid.uuid, sizeof(pGpu->gpuUuid.uuid));
            }
            else
            {
                prbEncAddString(pPrbEnc,
                    NVDEBUG_SYSTEMINFO_GPUINFO_GPU_UUID,
                    GPU_NA_UUID);
            }

            prbEncAddUInt32(pPrbEnc,
                NVDEBUG_SYSTEMINFO_GPUINFO_DEVICE_ID,
                pGpu->idInfo.PCIDeviceID);

            prbEncAddUInt32(pPrbEnc,
                NVDEBUG_SYSTEMINFO_GPUINFO_PMCBOOT0,
                pGpu->chipId0);

            prbEncAddUInt32(pPrbEnc,
                NVDEBUG_SYSTEMINFO_GPUINFO_SUBDEV_ID,
                pGpu->idInfo.PCISubDeviceID);
        }
    }

    prbEncAddUInt32(pPrbEnc,
        NVDEBUG_SYSTEMINFO_GPUINFO_NUM_GPUS,
        numGpus);

    NV_CHECK_OK_OR_GOTO(nvStatus, LEVEL_ERROR, // NVDEBUG_SYSTEMINFO_GPU_INFO
        prbEncNestedEnd(pPrbEnc),
        External_Cleanup);

    //OS Info
    NV_CHECK_OK_OR_GOTO(nvStatus, LEVEL_ERROR,
        prbEncNestedStart(pPrbEnc, NVDEBUG_SYSTEMINFO_OS_INFO),
        External_Cleanup);

    nvStatus = osGetVersionDump(pPrbEnc);
    if (nvStatus != NV_OK)
        goto External_Cleanup;

    NV_CHECK_OK_OR_GOTO(nvStatus, LEVEL_ERROR, // NVDEBUG_SYSTEMINFO_OS_INFO
        prbEncNestedEnd(pPrbEnc),
        External_Cleanup);

    // Driver Info
    NV_CHECK_OK_OR_GOTO(nvStatus, LEVEL_ERROR,
        prbEncNestedStart(pPrbEnc, NVDEBUG_SYSTEMINFO_DRIVER_INFO),
        External_Cleanup);

    sizeStr = (sizeof("RELEASE") < sizeof(NV_DISPLAY_DRIVER_TITLE) ?
        sizeof("RELEASE") :
        sizeof(NV_DISPLAY_DRIVER_TITLE));

    if (portMemCmp(NV_DISPLAY_DRIVER_TITLE, "RELEASE", sizeStr) == 0)
        bRelease = NV_TRUE;
    else
        bRelease = NV_FALSE;

    prbEncAddBool(pPrbEnc,
        NVDEBUG_SYSTEMINFO_DRIVERINFO_IS_RELEASE,
        bRelease);

    prbEncAddString(pPrbEnc,
        NVDEBUG_SYSTEMINFO_DRIVERINFO_VERSION,
        NV_VERSION_STRING);

    prbEncAddString(pPrbEnc,
        NVDEBUG_SYSTEMINFO_DRIVERINFO_BRANCH,
        NV_BUILD_BRANCH_VERSION);

    prbEncAddUInt32(pPrbEnc,
        NVDEBUG_SYSTEMINFO_DRIVERINFO_CHANGELIST,
        NV_LAST_OFFICIAL_CHANGELIST_NUM);

    // Only write previous driver version if loaded more than once.
    if (pRcDB->driverLoadCount > 1)
    {
        if (pRcDB->previousDriverVersion != NULL)
        {
            prbEncAddString(pPrbEnc,
                NVDEBUG_SYSTEMINFO_DRIVERINFO_PREVIOUS_VERSION,
                pRcDB->previousDriverVersion);
        }

        if (pRcDB->previousDriverBranch != NULL)
        {
            prbEncAddString(pPrbEnc,
                NVDEBUG_SYSTEMINFO_DRIVERINFO_PREVIOUS_BRANCH,
                pRcDB->previousDriverBranch);
        }

        prbEncAddUInt32(pPrbEnc,
            NVDEBUG_SYSTEMINFO_DRIVERINFO_PREVIOUS_CHANGELIST,
            pRcDB->prevDriverChangelist);
    }

    prbEncAddUInt32(pPrbEnc,
        NVDEBUG_SYSTEMINFO_DRIVERINFO_LOAD_COUNT,
        pRcDB->driverLoadCount);

    NV_CHECK_OK_OR_GOTO(nvStatus, LEVEL_ERROR, // NVDEBUG_SYSTEMINFO_DRIVER_INFO
        prbEncNestedEnd(pPrbEnc),
        External_Cleanup);

    // Dump an table of
    // Master GPU -- gpuId
    // List all gpus involved by gpuIds
    portMemSet(bGpuDone, NV_FALSE, sizeof(bGpuDone));
    for (gpu = 0; gpu < NV_MAX_DEVICES; gpu++)
    {
        pGpu = gpumgrGetGpu(gpu);

        if ((pGpu) && (bGpuDone[gpu] == NV_FALSE))
        {
            pParent = gpumgrGetParentGPU(pGpu);

            NV_CHECK_OK_OR_GOTO(nvStatus, LEVEL_ERROR,
                prbEncNestedStart(pPrbEnc, NVDEBUG_SYSTEMINFO_GPU_CONFIG),
                External_Cleanup);

            prbEncAddUInt32(pPrbEnc, NVDEBUG_SYSTEMINFO_CONFIG_MASTER_ID, pParent->gpuId);
            gpuMask = gpumgrGetGpuMask(pGpu);
            gpuIndex = 0;
            pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex);
            while (pGpu)
            {
                prbEncAddUInt32(pPrbEnc, NVDEBUG_SYSTEMINFO_CONFIG_GPU_ID, pGpu->gpuId);

                // gpuIndex is either the next or the MAX
                bGpuDone[gpuIndex - 1] = NV_TRUE;
                pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex);
            }

            NV_CHECK_OK_OR_GOTO(nvStatus, LEVEL_ERROR, // NVDEBUG_SYSTEMINFO_GPU_CONFIG
                prbEncNestedEnd(pPrbEnc),
                External_Cleanup);
        }
    }

    // Error state
    NV_CHECK_OK_OR_GOTO(nvStatus, LEVEL_ERROR,
        prbEncNestedStart(pPrbEnc, NVDEBUG_SYSTEMINFO_ERROR_STATE),
        External_Cleanup);

    prbEncAddUInt32(pPrbEnc,
        NVDEBUG_SYSTEMINFO_ERRORSTATE_BUGCHECK_CODE,
        pNvDumpState->bugCheckCode);

    prbEncAddBool(pPrbEnc,
        NVDEBUG_SYSTEMINFO_ERRORSTATE_GOT_RM_LOCK,
        pNvDumpState->bRMLock);

    prbEncAddUInt32(pPrbEnc,
        NVDEBUG_SYSTEMINFO_ERRORSTATE_DUMP_BUFFER_SIZE,
        pNvDumpState->initialbufferSize);

    //
    // prbEncNestedEnd for NVDEBUG_SYSTEMINFO_ERROR_STATE and
    // NVDEBUG_NVDUMP_SYSTEM_INFO are handled by prbEncUnwindNesting.
    //

External_Cleanup:
    // Unwind the protobuf to the correct depth.
    NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(nvStatus, LEVEL_ERROR,
        prbEncUnwindNesting(pPrbEnc, startingDepth));

    return nvStatus;
}

//
// Routine to dump RcDB Debug Info
//
NV_STATUS
rcdbDumpSystemFunc_IMPL
(
    OBJRCDB *pRcDB,
    PRB_ENCODER *pPrbEnc,
    NVD_STATE *pNvDumpState
)
{
    OBJGPU  *pGpu = gpumgrGetSomeGpu();

    switch (DRF_VAL(_NVD, _ERROR_CODE, _MAJOR, pNvDumpState->internalCode))
    {
    case NVD_GPU_GENERATED:
    case NVD_SKIP_ZERO:
        // don't report on these internal codes.
        return NV_OK;
        break;
    }

    rcdbDumpJournal(pRcDB, pGpu, pPrbEnc, pNvDumpState, NVDEBUG_NVDUMP_DCL_MSG);
    if (pGpu != NULL)
    {
        rcdbDumpErrorCounters(pRcDB, pGpu, pPrbEnc);
    }
    else
    {
        NV_PRINTF(LEVEL_WARNING,
                  "no GPU - won't dump ring buffers or journal\n");
    }

    return NV_OK;
}

static NvU32
_rcdbInsertErrorHistoryToList(RmRCCommonJournal_RECORD   *pList, NVD_STATE *pNvDumpState)
{
    OBJSYS                *pSys          = SYS_GET_INSTANCE();
    Journal               *pRcDB         = SYS_GET_RCDB(pSys);
    SYS_ERROR_INFO        *pSysErrorInfo = &pRcDB->ErrorInfo;
    RMPRBERRORELEMENT_V2*  pPrbErrorElement;
    RMCD_ERROR_BLOCK*      pErrorBlock;
    NV_STATUS              status = NV_OK;

    //
    // If we are called from the OCA dump, make sure we have the rm lock.
    // TO DO:  Try to dump as much as possible without the lock.
    //
    if (!pNvDumpState->bRMLock)
        return NV_OK;

    // Get Past Exceptions
    pPrbErrorElement = (RMPRBERRORELEMENT_V2*)pSysErrorInfo->pErrorList;
    while (NULL != pPrbErrorElement)
    {
        pErrorBlock = pPrbErrorElement->ErrorHeader.pErrorBlock;
        switch (pPrbErrorElement->RmPrbErrorData.common.Header.cRecordType)
        {
            case RmPrbErrorInfo_V2:
                _rcdbInsertJournalRecordToList (pList, &(pPrbErrorElement->RmPrbErrorData.common));
                break;

            case RmPrbFullDump_V2:
                //
                // Full crash dumps are a single NvDebug.NvDump message, and
                // should be contained in a single block.
                //
                if (pErrorBlock != NULL)
                {
                    if (pErrorBlock->pNext != NULL)
                    {
                        NV_PRINTF(LEVEL_WARNING,
                                  "only one error block expected!\n");
                    }
                    _rcdbInsertJournalRecordToList (pList, &(pPrbErrorElement->RmPrbErrorData.common));
                }
                break;
            default:
                // Can only handle protobuf formatted messages
                NV_PRINTF(LEVEL_ERROR, "unknown error element type: %d\n",
                          pPrbErrorElement->RmPrbErrorData.common.Header.cRecordType);
                break;
        }
        pPrbErrorElement = (RMPRBERRORELEMENT_V2*)pPrbErrorElement->ErrorHeader.pNextError;
    }
    return status;
}

static void
_rcdbDumpCommonJournalRecord
(
    PRB_ENCODER               *pPrbEnc,
    const PRB_FIELD_DESC      *pFieldDesc,
    RmRCCommonJournal_RECORD  *pRec
)
{
    NV_STATUS nvStatus = NV_OK;

    NV_CHECK_OK(nvStatus, LEVEL_ERROR,
        prbEncNestedStart(pPrbEnc, pFieldDesc));

    if (nvStatus == NV_OK)
    {
        if (pRec->timeStamp != 0)
            prbEncAddUInt64(pPrbEnc, JOURNAL_COMMON_TIME_STAMP, pRec->timeStamp);
        if (pRec->GPUTag != 0)
            prbEncAddUInt32(pPrbEnc, JOURNAL_COMMON_GPU_TAG,    pRec->GPUTag);
        if (pRec->CPUTag != 0)
            prbEncAddUInt64(pPrbEnc, JOURNAL_COMMON_CPU_TAG,    pRec->CPUTag);
        if (pRec->stateMask != 0)
            prbEncAddUInt64(pPrbEnc, JOURNAL_COMMON_STATE_MASK, pRec->stateMask);
        NV_CHECK_OK(nvStatus, LEVEL_ERROR, prbEncNestedEnd(pPrbEnc));
    }
}

static void
rcdbDumpCommonAssertRecord
(
    PRB_ENCODER              *pPrbEnc,
    NVD_STATE                *pNvDumpState,
    RmRCCommonAssert_RECORD  *pRec,
    NvU32                     type
)
{
    NvU32 i;

    prbEncAddUInt32(pPrbEnc, JOURNAL_ASSERT_TYPE,                 type);

    if (pRec->lastTimeStamp != 0)
        prbEncAddUInt64(pPrbEnc, JOURNAL_ASSERT_LAST_TIME_STAMP,  pRec->lastTimeStamp);

    prbEncAddUInt64(pPrbEnc, JOURNAL_ASSERT_BREAKPOINT_ADDR_HINT, pRec->breakpointAddrHint);

    // if there is a line number, add it to the message.
    if (pRec->lineNum != NV_RM_ASSERT_UNKNOWN_LINE_NUM)
        prbEncAddUInt32(pPrbEnc, JOURNAL_ASSERT_SOURCE_LINE, pRec->lineNum);

    if (pRec->count != 1)
        prbEncAddUInt32(pPrbEnc, JOURNAL_ASSERT_COUNT,            pRec->count);

    for (i = 0; i < NV_ARRAY_ELEMENTS(pRec->callStack); i++)
    {
        if (pRec->callStack[i] == 0)
            break;

        prbEncAddUInt64(pPrbEnc, JOURNAL_ASSERT_CALL_STACK, pRec->callStack[i]);
    }
}

static NV_STATUS
_rcdbDumpDclMsgRecord(
    PRB_ENCODER *pPrbEnc,
    NVD_STATE *pNvDumpState,
    const PRB_FIELD_DESC *pFieldDesc,
    RmRCCommonJournal_RECORD *pDclRecord
    )
{
    NV_STATUS nvStatus = NV_OK;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        prbEncNestedStart(pPrbEnc, pFieldDesc));

    _rcdbDumpCommonJournalRecord(pPrbEnc, DCL_DCLMSG_COMMON, pDclRecord);

    switch (pDclRecord->Header.cRecordType)
    {
        case RmRC2SwDbgBreakpoint_V3:
        case RmRC2SwRmAssert_V3:
        {
            RmRC2SwRmAssert3_RECORD* pRecord = (RmRC2SwRmAssert3_RECORD*)pDclRecord;

            NV_CHECK_OK(nvStatus, LEVEL_ERROR,
                prbEncNestedStart(pPrbEnc, DCL_DCLMSG_JOURNAL_ASSERT));
            if (nvStatus == NV_OK)
            {
                rcdbDumpCommonAssertRecord(pPrbEnc, pNvDumpState,
                    &pRecord->commonAssert, pDclRecord->Header.cRecordType);

                prbEncAddUInt32(pPrbEnc, JOURNAL_ASSERT_LEVEL, pRecord->level);
                NV_CHECK_OK(nvStatus, LEVEL_ERROR, prbEncNestedEnd(pPrbEnc));
            }
            break;
        }
        case RmRC2GpuTimeout_V3:
        {
            RmRC2GpuTimeout3_RECORD* pRecord = (RmRC2GpuTimeout3_RECORD*)pDclRecord;

            NV_CHECK_OK(nvStatus, LEVEL_ERROR,
                prbEncNestedStart(pPrbEnc, DCL_DCLMSG_JOURNAL_ASSERT));
            if (nvStatus == NV_OK)
            {
                rcdbDumpCommonAssertRecord(pPrbEnc, pNvDumpState, pRecord, pDclRecord->Header.cRecordType);
                NV_CHECK_OK(nvStatus, LEVEL_ERROR, prbEncNestedEnd(pPrbEnc));
            }
            break;
        }
        case RmBadRead_V2:
        {
            RmRC2BadRead2_RECORD* pRecord = (RmRC2BadRead2_RECORD*)pDclRecord;

            NV_CHECK_OK(nvStatus, LEVEL_ERROR,
                prbEncNestedStart(pPrbEnc, DCL_DCLMSG_JOURNAL_BADREAD));
            if (nvStatus == NV_OK)
            {
                prbEncAddUInt32(pPrbEnc, JOURNAL_BADREAD_MEMORY_SPACE, pRecord->MemorySpace);
                prbEncAddUInt32(pPrbEnc, JOURNAL_BADREAD_OFFSET, pRecord->Offset);
                prbEncAddUInt32(pPrbEnc, JOURNAL_BADREAD_MASK, pRecord->Mask);
                prbEncAddUInt32(pPrbEnc, JOURNAL_BADREAD_VALUE, pRecord->Value);
                prbEncAddUInt32(pPrbEnc, JOURNAL_BADREAD_REASON, pRecord->Reason);
                NV_CHECK_OK(nvStatus, LEVEL_ERROR, prbEncNestedEnd(pPrbEnc));
            }
            break;
        }
        case RmDclMsg:
        {
            RM_DATA_COLLECTION_RECORD *pRecord = (RM_DATA_COLLECTION_RECORD*) pDclRecord;
            // Add the bytes after RM_DATA_COLLECTION_RECORD
            prbEncAddBytes(pPrbEnc, pRecord->fieldDesc, (void *) (pRecord + 1),
                pRecord->common.Header.wRecordSize - sizeof(*pRecord));
            break;
        }
        case RmJournalEngDump:
        {
            RM_DATA_COLLECTION_RECORD *pRecord = (RM_DATA_COLLECTION_RECORD*) pDclRecord;
            // Add the bytes after RM_DATA_COLLECTION_RECORD
            prbEncCatMsg(pPrbEnc, (void *)(pRecord + 1),
                    pRecord->common.Header.wRecordSize - sizeof(*pRecord));
            break;
        }
        case RmJournalBugCheck:
        {
            RmJournalBugcheck_RECORD* pRecord = (RmJournalBugcheck_RECORD*)pDclRecord;
            NV_CHECK_OK(nvStatus, LEVEL_ERROR,
                prbEncNestedStart(pPrbEnc, DCL_DCLMSG_JOURNAL_BUGCHECK));
            if (nvStatus == NV_OK)
            {
                prbEncAddUInt32(pPrbEnc, JOURNAL_BUGCHECK_CODE, pRecord->bugCheckCode);
                NV_CHECK_OK(nvStatus, LEVEL_ERROR, prbEncNestedEnd(pPrbEnc));
            }
            break;
        }
        case RmPrbErrorInfo_V2:
        case RmPrbFullDump_V2:
        {
            RMPRBERRORELEMENT_V2*   pRecord = (RMPRBERRORELEMENT_V2*)((NvU8 *)pDclRecord
                                                - NV_OFFSETOF(RMPRBERRORELEMENT_V2, RmPrbErrorData));
            RMCD_ERROR_BLOCK*       pErrorBlock;

            for (pErrorBlock = pRecord->ErrorHeader.pErrorBlock;
                (pErrorBlock != NULL); pErrorBlock = pErrorBlock->pNext)
            {
                    prbEncCatMsg(pPrbEnc, (void *)pErrorBlock->pBlock,
                                    pErrorBlock->blockSize);
            }
            break;
        }
        case RmNocatReport:
        {
            // currently not added to the OCA dump
            break;
        }

        default:
            // These are the only ones we know about
            NV_PRINTF(LEVEL_ERROR,
                        "unknown Dcl Record entry type: %d\n",
                        pDclRecord->Header.cRecordType);
            break;
    }

    NV_CHECK_OK(nvStatus, LEVEL_ERROR, prbEncNestedEnd(pPrbEnc));
    return 0;
}

static NvU32
_rcdbInsertJournalRecordToList (RmRCCommonJournal_RECORD *pList, RmRCCommonJournal_RECORD *pRecord)
{
    RmRCCommonJournal_RECORD *pCurrentRecord = pList;
    RmRCCommonJournal_RECORD *pNextRecord;

    if ((NULL != pList) && (NULL != pRecord))
    {
        for (pNextRecord = (RmRCCommonJournal_RECORD *)pList->pNext; pNextRecord != pList; pNextRecord = (RmRCCommonJournal_RECORD *)pNextRecord->pNext)
        {
            if (pRecord->timeStamp  < pNextRecord->timeStamp)
            {
                break;
            }
            pCurrentRecord = pNextRecord;
        }
        pRecord->pNext = pCurrentRecord->pNext;
        pCurrentRecord->pNext = (NvU8 *)pRecord;
    }
    return 0;
}

// Todo: format the records into a protobuf DCL record at the source
static NvU32
rcdbInsertRingBufferToList(
    Journal                    *pRcDB,
    RmRCCommonJournal_RECORD   *pList,
    RING_BUFFER_LOG            *pRingBuffer
)
{
    RmRCCommonJournal_RECORD *pCommon;
    NvU32 recordSize;
    NvU32 i;

    recordSize = rcdbGetOcaRecordSizeWithHeader(pRcDB, pRingBuffer->entryType);

    //
    // Order does not matter here because the record will be inserted into the
    // list based on the time of the record, not its postion in the buffer.
    //
    for (i = 0; i < pRingBuffer->numEntries; i++)
    {
        pCommon = (RmRCCommonJournal_RECORD *)(((NvU8 *)pRingBuffer->pBuffer) + (recordSize * i));

        _rcdbInsertJournalRecordToList (pList, pCommon);
    }

    return 0; // return value should be discarded
}

static NvU32
rcdbInsertRingBufferCollectionToList(
    Journal                    *pRcDB,
    RmRCCommonJournal_RECORD   *pList)
{
    RING_BUFFER_LOG_COLLECTION *pRingBufferColl = &pRcDB->RingBufferColl;
    RING_BUFFER_LOG *pCurrentBuffer;
    NvU32 i;


    pCurrentBuffer = pRingBufferColl->pFirstEntry;
    for (i = 0; i < pRingBufferColl->NumRingBuffers; i++)
    {
        NvU32 recSize = pCurrentBuffer->bufferSize;

        NV_ASSERT(pCurrentBuffer->maxEntries *
                  rcdbGetOcaRecordSizeWithHeader(pRcDB, pCurrentBuffer->entryType) ==
                  pCurrentBuffer->bufferSize);

        if (recSize > 0)
        {
            rcdbInsertRingBufferToList (pRcDB, pList, pCurrentBuffer);
        }
        pCurrentBuffer = pCurrentBuffer->pNextRingBuffer;
    }

    // Assert that we traversed through the entire list.
    NV_ASSERT(pCurrentBuffer == NULL);

    // return value should be ignored
    return 0;
}

NvU32
rcdbDumpJournal_IMPL
(
    OBJRCDB *pRcDB,
    OBJGPU *pGpu,
    PRB_ENCODER *pPrbEnc,
    NVD_STATE *pNvDumpState,
    const PRB_FIELD_DESC *pFieldDesc
)
{
    OS_DRIVER_BLOCK DriverBlock;
    EVENT_JOURNAL *pJournal = &pRcDB->Journal;
    NvU8 *pJournalBuff      = pJournal->pBuffer;
    RmRCCommonJournal_RECORD *pRecord;
    NvU32 recSize;
    NV_STATUS nvStatus = NV_OK;
    RmRCCommonJournal_RECORD List;

    // It is OK to dump the journal entries without the RM lock.
    // No need to check pRcDB->nvDumpState.bNoRMLock;

    recSize = pJournal->BufferSize - pJournal->BufferRemaining;

    if (NULL != pGpu)
    {
        //
        // Add RVA Header, even when there are no journal records.
        // This header is required to resolve code addresses using the PDB file.
        // We can log code addresses outside of the journal entries.
        //
        NV_CHECK_OK(nvStatus, LEVEL_ERROR, prbEncNestedStart(pPrbEnc, pFieldDesc));
        if (nvStatus == NV_OK)
        {
            NV_CHECK_OK(nvStatus, LEVEL_ERROR,
                prbEncNestedStart(pPrbEnc, DCL_DCLMSG_JOURNAL_RVAHEADER));
            if (nvStatus == NV_OK)
            {
                portMemSet(&DriverBlock, 0x00, sizeof(DriverBlock));
                osGetDriverBlock(pGpu->pOsGpuInfo, &DriverBlock);
                prbEncAddUInt64(pPrbEnc, JOURNAL_RVAHEADER_DRIVER_START, (NvU64)DriverBlock.driverStart);
                prbEncAddUInt32(pPrbEnc, JOURNAL_RVAHEADER_OFFSET, DriverBlock.offset);
                prbEncAddUInt32(pPrbEnc, JOURNAL_RVAHEADER_POINTER_SIZE, sizeof(pJournal));
                prbEncAddUInt64(pPrbEnc, JOURNAL_RVAHEADER_UNIQUE_ID_HIGH, *((NvU64*) DriverBlock.unique_id));
                prbEncAddUInt64(pPrbEnc, JOURNAL_RVAHEADER_UNIQUE_ID_LOW, *((NvU64*) (DriverBlock.unique_id + 8)));
                prbEncAddUInt32(pPrbEnc, JOURNAL_RVAHEADER_AGE, DriverBlock.age);
                NV_CHECK_OK(nvStatus, LEVEL_ERROR, prbEncNestedEnd(pPrbEnc));
            }
            NV_CHECK_OK(nvStatus, LEVEL_ERROR, prbEncNestedEnd(pPrbEnc));
        }
    }

    // init the list to an empty state
    portMemSet(&List, 0x00, sizeof(List));
    List.pNext = (NvU8 *)&List;

    //
    // Don't dump the ring buffers if something is adding to them.
    // If we can dump the ring buffers, hold the lock for them until the
    // dump is complete to insure that a record is not changed mid-dump.
    //
    if (portAtomicIncrementS32(&concurrentRingBufferAccess) != 1)
    {
        //
        // If IRQL is low, spin until it gets available
        //
        if (!osIsRaisedIRQL() && (NULL != pGpu))
        {
            RMTIMEOUT         timeout;
            NV_STATUS         status = NV_OK;
            gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);
            do {
                portAtomicDecrementS32(&concurrentRingBufferAccess);

                if (NV_ERR_TIMEOUT == status)
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "timed out waiting for Rm journal ring buffer to be available\n");
                    DBG_BREAKPOINT();
                    return 0;
                }
                status = gpuCheckTimeout(pGpu, &timeout);
                osSpinLoop();
            } while (portAtomicIncrementS32(&concurrentRingBufferAccess) != 1);
        }
        else
        {
            NV_ASSERT_FAILED("Ring Buffer unavailable for dump at high irql.");
        }
    }

    rcdbInsertRingBufferCollectionToList (pRcDB, &List);

    _rcdbInsertErrorHistoryToList(&List, pNvDumpState);

    // Skip if size is smaller than a header
    while (recSize > sizeof(RmRCCommonJournal_RECORD))
    {
        pRecord = (RmRCCommonJournal_RECORD *)pJournalBuff;

        if (pRecord->Header.cRecordGroup != RmGroup)
        {
            // We only log RM related data
            NV_ASSERT(pRecord->Header.cRecordGroup == RmGroup);
            break;
        }

        // Just a safety net...
        if (pRecord->Header.wRecordSize > recSize)
        {
            break;
        }
        _rcdbInsertJournalRecordToList (&List, pRecord);

        recSize -= pRecord->Header.wRecordSize;
        pJournalBuff += pRecord->Header.wRecordSize;
    }


    // dump out the records that have been added to the list.
    for (pRecord = (RmRCCommonJournal_RECORD *)List.pNext; pRecord != &List; pRecord = (RmRCCommonJournal_RECORD *)pRecord->pNext)
    {
        _rcdbDumpDclMsgRecord(pPrbEnc, pNvDumpState, pFieldDesc, pRecord);
    }
    portAtomicDecrementS32(&concurrentRingBufferAccess);

    // return value should be ignored
    return 0;
}

NvU32
rcdbDumpErrorCounters_IMPL(Journal *pRcDB, OBJGPU *pGpu, PRB_ENCODER *pPrbEnc)
{
    NvU32                   i;
    NvU32                   rcErrTyp = RC_ERROR_COUNTER_TYPE_INVALID;
    NV_STATUS               nvStatus = NV_OK;
    NvU8                    startingDepth = prbEncNestingLevel(pPrbEnc);

    // Opens NVDEBUG_NVDUMP_DCL_MSG
    NV_CHECK_OK_OR_GOTO(nvStatus, LEVEL_ERROR,
        prbEncNestedStart(pPrbEnc, NVDEBUG_NVDUMP_DCL_MSG),
        cleanupAndExit);

    for (i = 0; i <= RC_ERROR_COUNTER_OTHER_INDEX; i++)
    {
        // For Counters
        rcErrTyp = pRcDB->rcErrorCounterArray[i].rcErrorType;
        if (rcErrTyp != RC_ERROR_COUNTER_TYPE_INVALID)
        {
            NV_CHECK_OK_OR_GOTO(nvStatus, LEVEL_ERROR,
                prbEncNestedStart(pPrbEnc, DCL_DCLMSG_RCCOUNTER),
                cleanupAndExit);

            // Write Power Event
            prbEncAddUInt32(pPrbEnc, RC_RCCOUNTER_RCERRORTYPE, rcErrTyp);

            // Write Power State
            prbEncAddUInt32(pPrbEnc, RC_RCCOUNTER_COUNT, pRcDB->rcErrorCounterArray[i].rcErrorCount);

            // Dump the channel ID and the last time when this error occurred on this channel ID
            prbEncAddUInt32(pPrbEnc, RC_RCCOUNTER_RCLASTCHID, pRcDB->rcErrorCounterArray[i].rcLastCHID);
            prbEncAddUInt64(pPrbEnc, RC_RCCOUNTER_RCLASTTIME, pRcDB->rcErrorCounterArray[i].rcLastTime);

            NV_CHECK_OK_OR_GOTO(nvStatus, LEVEL_ERROR,
                prbEncNestedEnd(pPrbEnc),
                cleanupAndExit);
        }
    } // For Counters

    // Close NVDEBUG_NVDUMP_DCL_MSG handled by prbEncUnwindNesting.

cleanupAndExit:
    // Unwind the protobuff to inital depth
    NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(nvStatus, LEVEL_ERROR,
        prbEncUnwindNesting(pPrbEnc, startingDepth));

    return 0;
}

static void
_rcdbAddRmGpuDumpCallback
(
    void *pData
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    NV_STATUS status;

    NvU32 gpuInstance = *((NvU32 *)pData);
    status = osAcquireRmSema(pSys->pSema);
    if (status == NV_OK)
    {
        // LOCK: acquire API lock
        status = rmapiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_DIAG);
        if (status == NV_OK)
        {
            // LOCK: acquire GPUs lock
            status = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE,
                                       RM_LOCK_MODULES_DIAG);
            if (status == NV_OK)
            {
                Journal *pRcDB = SYS_GET_RCDB(pSys);
                OBJGPU  *pGpu = gpumgrGetGpu(gpuInstance);

                //
                // Mark the Journal object as in the deferred dump path so we won't
                // re-attempt again.
                //
                pRcDB->setProperty(pRcDB, PDB_PROP_RCDB_IN_DEFERRED_DUMP_CODEPATH, NV_TRUE);

                status = rcdbAddRmGpuDump(pGpu);
                NV_ASSERT(status == NV_OK);

                pRcDB->setProperty(pRcDB, PDB_PROP_RCDB_IN_DEFERRED_DUMP_CODEPATH, NV_FALSE);

                // UNLOCK: release GPUs lock
                rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
            }
            else
            {
                NV_PRINTF(LEVEL_ERROR, "failed to acquire the GPU locks!\n");
            }
            // UNLOCK: release API lock
            rmapiLockRelease();
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "failed to acquire the API lock!\n");
        }
        osReleaseRmSema(pSys->pSema, NULL);
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "failed to acquire the OS semaphore!\n");
    }
}

static NV_STATUS
nvdDebuggerBufferCallback(void *pEncoder, NvBool bBufferFull)
{
    if (bBufferFull)
    {
        nvDumpConfig.dumpStatus = NVDUMP_STATUS_DUMP_BUFFER_FULL;
    }
    else
    {
        nvDumpConfig.dumpStatus = NVDUMP_STATUS_DUMP_END_OF_MSG;
    }

    return NV_OK;
}

/*!
 * @brief NvDebug kernel debugger dump control
 *
 * Allows external kernel debuggers to control the RM's dump interface
 * without assuming anything about the current system state.
 *
 * WARNING! This function should never be called directly!
 *
 * If correctly setup, a kernel debugger will place a processor
 * hardware watchpoint on the nvDumpConfig.handshake variable.
 * Each time this is written to, the debugger will break and get a chance
 * to examine the rest of the nvDumpConfig state.
 *
 * @return This function should never return! External debugger should abort it!
 */
static void
nvdDebuggerControlFunc(void)
{
    OBJSYS        *pSys = SYS_GET_INSTANCE();
    Journal       *pRcDB = SYS_GET_RCDB(pSys);
    OBJGPU        *pGpu = NULL;
    NvDebugDump   *pNvd = NULL;
    NVDUMP_BUFFER *pBuffer = (NVDUMP_BUFFER *)&nvDumpConfig.buffer; // discard volatile

    // Process actions while debugger provides work to do.
    while (nvDumpConfig.dumpStatus != NVDUMP_STATUS_IDLE)
    {
        nvDumpConfig.rmStatus = NV_OK;

        NV_PRINTF(LEVEL_INFO,
                  "Dump triggered: gpuSelect=%u, component=%u,  dumpStatus=%u\n",
                  nvDumpConfig.gpuSelect, nvDumpConfig.component,
                  nvDumpConfig.dumpStatus);

        if (NVDUMP_IS_GPU_COMPONENT(nvDumpConfig.component))
        {
            pGpu = gpumgrGetGpu(nvDumpConfig.gpuSelect);
            pNvd = GPU_GET_NVD(pGpu);

            switch (nvDumpConfig.dumpStatus)
            {
                case NVDUMP_STATUS_COUNT_REQUESTED:
                    nvDumpConfig.rmStatus = nvdDumpComponent(
                        pGpu, pNvd, nvDumpConfig.component, pBuffer,
                        NVDUMP_BUFFER_COUNT, NULL);
                    nvDumpConfig.dumpStatus = NVDUMP_STATUS_COUNT_COMPLETE;
                    break;
                case NVDUMP_STATUS_DUMP_REQUESTED:
                    nvDumpConfig.rmStatus = nvdDumpComponent(
                        pGpu, pNvd, nvDumpConfig.component, pBuffer,
                        NVDUMP_BUFFER_PROVIDED, &nvdDebuggerBufferCallback);
                    nvDumpConfig.dumpStatus = NVDUMP_STATUS_DUMP_COMPLETE;
                    break;
                default:
                    NV_PRINTF(LEVEL_ERROR, "Invalid dumpStatus %u\n",
                              nvDumpConfig.dumpStatus);
                    nvDumpConfig.rmStatus = NV_ERR_INVALID_STATE;
                    nvDumpConfig.dumpStatus = NVDUMP_STATUS_ERROR;
                    break;
            }
        }
        else if (NVDUMP_IS_SYS_COMPONENT(nvDumpConfig.component))
        {
            switch (nvDumpConfig.dumpStatus)
            {
                case NVDUMP_STATUS_COUNT_REQUESTED:
                    nvDumpConfig.rmStatus = rcdbDumpComponent(pRcDB,
                        nvDumpConfig.component, pBuffer,
                        NVDUMP_BUFFER_COUNT, NULL);
                    nvDumpConfig.dumpStatus = NVDUMP_STATUS_COUNT_COMPLETE;
                    break;
                case NVDUMP_STATUS_DUMP_REQUESTED:
                    nvDumpConfig.rmStatus = rcdbDumpComponent(pRcDB,
                        nvDumpConfig.component, pBuffer,
                        NVDUMP_BUFFER_PROVIDED, &nvdDebuggerBufferCallback);
                    nvDumpConfig.dumpStatus = NVDUMP_STATUS_DUMP_COMPLETE;
                    break;
                default:
                    NV_PRINTF(LEVEL_ERROR, "Invalid dumpStatus %u\n",
                              nvDumpConfig.dumpStatus);
                    nvDumpConfig.rmStatus = NV_ERR_INVALID_STATE;
                    nvDumpConfig.dumpStatus = NVDUMP_STATUS_ERROR;

                    break;
            }
        }
        else
        {
             NV_PRINTF(LEVEL_ERROR, "Invalid component %u\n",
                       nvDumpConfig.component);
             nvDumpConfig.rmStatus = NV_ERR_INVALID_PARAM_STRUCT;
             nvDumpConfig.dumpStatus = NVDUMP_STATUS_ERROR;
        }
    }

    // Ensure we really don't exit this function without debugger.
    while (1)
    {
        NV_PRINTF(LEVEL_ERROR, "Should never reach this point!\n");
        DBG_BREAKPOINT();
    }
}

/*!
 * @brief   Release Build NV_ASSERT function
 *
 * @details Called by NV_ASSERT when the assertion fails.
 *          By putting this logic in its own function, we save on binary size.
 */
#if (defined(_WIN32) || defined(_WIN64) || defined(NV_UNIX) || RMCFG_FEATURE_PLATFORM_GSP) && !defined(NV_MODS)
static void _rcdbRmAssert(NvU32 level, NvU32 lineNum, NvU64 ip)
{
    RmRC2SwRmAssert3_RECORD* pRec = NULL;
    if (rcdbAddAssertJournalRecWithLine(NULL, lineNum, (void **)&pRec, RmGroup,
        RmRC2SwRmAssert_V3, sizeof(RmRC2SwRmAssert3_RECORD),
        level, ip) == NV_OK)
    {
        pRec->level = level;
    }

#if !defined(DEBUG) && !defined(QA_BUILD)
    {
        OBJSYS *pSys = SYS_GET_INSTANCE();

        // Add assert to NvLog.  But skip when nvLog asserts to avoid stack overflow.
        if (portAtomicIncrementS32(&nvLogRecursion) == 1)
        {
            // check for GPU lost.
            rcdProbeAllGpusPresent(ip);
        }
        portAtomicDecrementS32(&nvLogRecursion);

        if ((pSys != NULL) && ((NV_DEBUG_BREAK_ATTRIBUTES_ASSERT) &
            DRF_VAL(_DEBUG, _BREAK, _ATTRIBUTES, pSys->debugFlags)))
        {
            REL_DBG_BREAKPOINT_MSG("NVRM-RC: Nvidia Release NV_ASSERT Break\n");
        }
    }

    // If enabled bugcheck on assert
    osDbgBugCheckOnAssert();

#endif
}

//
// Some param-less wrappers for rcdbXxxEx() functions.
// If the params are not needed, calling these functions saves on binary size
//
void rcdbRmAssert(NvU32 LineNum, NvU64 ip) {  _rcdbRmAssert(0, LineNum, ip); }
void rcdbRmAssertStatus(NvU32 status, NvU32 LineNum, NvU64 ip) { _rcdbRmAssert(status, LineNum, ip); }

#endif // (defined(_WIN32) || defined(_WIN64) || defined(NV_UNIX) || RMCFG_FEATURE_PLATFORM_GSP) && !defined(NV_MODS)

#if (defined(_WIN32) || defined(_WIN64) || defined(NV_UNIX)) && !defined(NV_MODS)

/*!
 * @brief   Release Build DBGBREAKPOINT() function
 *
 * @details Called by DBGBREAKPOINT when the assertion fails.
 *          By putting this logic in its own function, we save on binary size.
 */
static void _rcdbDbgBreakEx(void *pGpu, NvU32 lineNum, NvU32 level, NvU64 ip)
{
    RmRC2SwRmAssert3_RECORD* pRec = NULL;
    if (rcdbAddAssertJournalRecWithLine(pGpu, lineNum, (void**)&pRec, RmGroup,
         RmRC2SwDbgBreakpoint_V3, sizeof(RmRC2SwRmAssert3_RECORD), level, ip) == NV_OK)
    {
        pRec->level = level;
    }

#if !defined(DEBUG) && !defined(QA_BUILD)
    {
        OBJSYS *pSys = SYS_GET_INSTANCE();

        // Add assert to NvLog.  But skip when nvLog asserts to avoid stack overflow.
        if (portAtomicIncrementS32(&nvLogRecursion) == 1)
        {
            NV_PRINTF(LEVEL_NOTICE, "Breakpoint at 0x%llx.\n", ip);
        }
        portAtomicDecrementS32(&nvLogRecursion);

        if ((pSys != NULL) && ((NV_DEBUG_BREAK_ATTRIBUTES_DBG_BREAK) &
            DRF_VAL(_DEBUG, _BREAK, _ATTRIBUTES, pSys->debugFlags)))
        {
            REL_DBG_BREAKPOINT_MSG("NVRM-RC: Nvidia Release Debug Break\n");
        }
    }
#endif

    // If enabled bugcheck on assert
    osDbgBugCheckOnAssert();
}

void rcdbDbgBreak(NvU64 ip)                             { _rcdbDbgBreakEx(NULL, NV_RM_ASSERT_UNKNOWN_LINE_NUM, 0,      ip); }
void rcdbDbgBreakGpu(void *pGpu, NvU64 ip)              { _rcdbDbgBreakEx(pGpu, NV_RM_ASSERT_UNKNOWN_LINE_NUM, 0,      ip); }
void rcdbDbgBreakStatus(NvU32 status, NvU64 ip)         { _rcdbDbgBreakEx(NULL, NV_RM_ASSERT_UNKNOWN_LINE_NUM, status, ip); }
void rcdbDbgBreakEx(void *pGpu, NvU32 status, NvU64 ip) { _rcdbDbgBreakEx(pGpu, NV_RM_ASSERT_UNKNOWN_LINE_NUM, status, ip); }

#endif

NV_STATUS
rcdbAddRmEngDump
(
    OBJGPU  *pGpu,
    NvU32 component
)
{
    OBJSYS          *pSys     = SYS_GET_INSTANCE();
    Journal         *pRcDB    = SYS_GET_RCDB(pSys);
    NvDebugDump     *pNvd     = GPU_GET_NVD(pGpu);
    NVDUMP_BUFFER    nvDumpBuffer = {0};
    RM_DATA_COLLECTION_RECORD *pRec;
    NV_STATUS        rmStatus;
    NvU16            totalSize;

    nvDumpBuffer.size = NVDUMP_MAX_DUMP_SIZE;

    rmStatus = nvdDumpComponent(pGpu, pNvd, component, &nvDumpBuffer,
               NVDUMP_BUFFER_ALLOCATE, NULL);
    if (rmStatus != NV_OK)
    {
        goto rcdbAddRmEngDump_error_handle;
    }

    totalSize = (NvU16)(nvDumpBuffer.curNumBytes + sizeof(*pRec));
    //align to 8 bytes to keep the readability of RM journal
    totalSize = (totalSize + 0x7) & ~0x7;
    // check for overflow
    if (((NvU32)totalSize) < nvDumpBuffer.curNumBytes + sizeof(*pRec))
    {
        goto rcdbAddRmEngDump_error_handle;
    }

    rmStatus = rcdbAllocNextJournalRec(pRcDB, (NVCD_RECORD **)&pRec, RmGroup,
                                       RmJournalEngDump, totalSize);
    if (rmStatus != NV_OK)
    {
        goto rcdbAddRmEngDump_error_handle;
    }
    rcdbSetCommonJournalRecord(pGpu, &pRec->common);

    // copy the dump buffer right after the RM_DATA_COLLECTION_RECORD struct
    portMemCopy((void *)(pRec + 1), nvDumpBuffer.curNumBytes, NvP64_VALUE(nvDumpBuffer.address), nvDumpBuffer.curNumBytes);

    pRec->fieldDesc = NVDEBUG_NVDUMP_GPU_INFO;

rcdbAddRmEngDump_error_handle:
    if (nvDumpBuffer.address != NvP64_NULL)
    {
        portMemFree(NvP64_VALUE(nvDumpBuffer.address));
    }

    return rmStatus;
}


// Finds the ring buffer for a corresponding type. Returns error if not allocated.
static void
rcdbFindRingBufferForType
(
    Journal *pRcDB,
    RMCD_RECORD_TYPE recType,
    RING_BUFFER_LOG **ppRingBuffer
)
{
    NvU32 i;
    RING_BUFFER_LOG *pCurrentRingBuffer = NULL;
    RING_BUFFER_LOG_COLLECTION *pRingBufferColl = &pRcDB->RingBufferColl;

    NV_ASSERT(ppRingBuffer != NULL);
    *ppRingBuffer = NULL;

    //
    // Loop through our ring buffer collection, and find the
    // ring buffer corresponding to our type.
    //
    pCurrentRingBuffer = pRingBufferColl->pFirstEntry;
    for (i = 0; i < pRingBufferColl->NumRingBuffers; i++)
    {
        NV_ASSERT(pCurrentRingBuffer != NULL);
        if (pCurrentRingBuffer->entryType == recType)
        {
            *ppRingBuffer = pCurrentRingBuffer;
            return;
        }
        pCurrentRingBuffer = pCurrentRingBuffer->pNextRingBuffer;
    }

    NV_PRINTF(LEVEL_INFO, "Ring Buffer not found for type %d\n", recType);
    return;
}

//
// Creates a ring buffer capable of holding "maxEntries" number of entries, and
// adds it to the ring buffer collection.
// Returns a pointer to the created ring buffer so that individual modules can
// examine the data on-demand easily.
//
//PRINT_BUFFER_LOG
NvU8 *
rcdbCreateRingBuffer_IMPL
(
    Journal *pRcDB,
    RMCD_RECORD_TYPE type,
    NvU32   maxEntries
)
{
    NV_STATUS status;
    RING_BUFFER_LOG_COLLECTION *pRingBufferColl = &pRcDB->RingBufferColl;
    RING_BUFFER_LOG *pRingBuffer;
    NvU8*           pBuffer = NULL;
    NvU32 bufferSize, entrySize;

    rcdbFindRingBufferForType(pRcDB, type, &pRingBuffer);

    entrySize = rcdbGetOcaRecordSizeWithHeader(pRcDB, type);
    if (entrySize == 0)
    {
        NV_ASSERT(entrySize != 0);
        return NULL;
    }

    // We need to store maxEntries number of entries. Check for overflow too
    if (portSafeMulU32(maxEntries, entrySize, &bufferSize) == NV_FALSE)
    {
        return NULL;
    }

    if (pRingBuffer != NULL)
    {
        NvU32 totalSize;

        if (portSafeAddU32(bufferSize, pRingBuffer->bufferSize, &totalSize) == NV_FALSE)
        {
            return NULL;
        }

        bufferSize = totalSize;
        pRingBuffer->refCount++;

        //
        // XXX The collect-all design of the ring buffers allows for
        // interleaved entries for different GPUs. This makes it
        // hard to dynamically shrink any given ring buffer as GPUs are
        // torn down, and requires that an upper bound be placed on
        // the buffer's size.
        //
        // The upper bound, as chosen, is somewhat arbitrary, but at
        // the time of this writing, consistent with the use of
        // this interface (i.e. the number of entries for each type is
        // the same for each GPU).
        //
        if (bufferSize > pRingBuffer->maxBufferSize)
             return NULL;
    }
    else
    {
        pRingBuffer = portMemAllocNonPaged(sizeof(RING_BUFFER_LOG));
        if (pRingBuffer == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            NV_ASSERT(status == NV_OK);
            return NULL;
        }

        portMemSet(pRingBuffer, 0x00, sizeof(*pRingBuffer));
        pRingBuffer->refCount = 1;
    }

    pBuffer = portMemAllocNonPaged(bufferSize);
    if (pBuffer == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        NV_ASSERT(status == NV_OK);
        pRingBuffer->refCount--;
        if (pRingBuffer->pBuffer == NULL)
        {
            portMemFree(pRingBuffer);
        }
        return NULL;
    }

    // Now, initialize the entries the RING_BUFFER structure.
    pRingBuffer->maxEntries += maxEntries;

    // Add the ring buffer to the beginning of the ring buffer collection.
    if (pRingBuffer->pBuffer == NULL)
    {
        if (portSafeMulU32(bufferSize, NV_MAX_DEVICES, &pRingBuffer->maxBufferSize) == NV_FALSE)
        {
            pRingBuffer->refCount--;
            if (pRingBuffer->pBuffer == NULL)
            {
                portMemFree(pRingBuffer);
            }

            portMemFree(pBuffer);
            return NULL;
        }

        pRingBuffer->maxBufferSize = (bufferSize * NV_MAX_DEVICES);
        pRingBuffer->entryType = type;
        pRingBuffer->pNextRingBuffer = pRingBufferColl->pFirstEntry;
        pRingBufferColl->pFirstEntry = pRingBuffer;
        pRingBufferColl->NumRingBuffers++;
    }
    else
    {
        NvU32 copySize;

        if (portSafeSubU32(bufferSize, pRingBuffer->bufferSize, &copySize) == NV_FALSE)
        {
            pRingBuffer->refCount--;
            if (pRingBuffer->pBuffer == NULL)
            {
                portMemFree(pRingBuffer);
            }

            portMemFree(pBuffer);
            return NULL;
        }

        portMemCopy(pBuffer, copySize, pRingBuffer->pBuffer, copySize);
        portMemFree(pRingBuffer->pBuffer);
    }

    pRingBuffer->bufferSize = bufferSize;
    pRingBuffer->pBuffer = pBuffer;
    return (NvU8 *)pRingBuffer;
}

void
rcdbDestroyRingBuffer_IMPL
(
    Journal *pRcDB,
    RMCD_RECORD_TYPE type
)
{
    RING_BUFFER_LOG_COLLECTION *pRingBufferColl = &pRcDB->RingBufferColl;
    RING_BUFFER_LOG *pRingBuffer, *pCurrentRingBuffer;
    NvU32 i;

    rcdbFindRingBufferForType(pRcDB, type, &pRingBuffer);
    if (pRingBuffer == NULL)
        return;

    if (--pRingBuffer->refCount > 0)
        return;

    pCurrentRingBuffer = pRingBufferColl->pFirstEntry;
    if (pCurrentRingBuffer == pRingBuffer)
    {
        pRingBufferColl->pFirstEntry = pCurrentRingBuffer->pNextRingBuffer;
    }
    else
    {
        for (i = 0; i < pRingBufferColl->NumRingBuffers; i++)
        {
            if (pCurrentRingBuffer->pNextRingBuffer == pRingBuffer)
            {
                pCurrentRingBuffer->pNextRingBuffer =
                    pRingBuffer->pNextRingBuffer;
                break;
            }
            pCurrentRingBuffer = pCurrentRingBuffer->pNextRingBuffer;
        }
    }

    portMemFree(pRingBuffer->pBuffer);
    portMemFree(pRingBuffer);

    pRingBufferColl->NumRingBuffers--;
}

/*
** _rcdbAllocRecFromRingBuffer allocates a buffer entry from the
**  specified ring buffer.
**
**  parameters:
**      pGpu            a pointer to the GPU object associated with the entry.
**      pRcdb           a pointer toe the Journal that contains the ring buffers
**      type            the record type to locate a buffer for.
**      recordSize      the size of the expected record
**
**  notes:
**      it is assumed the caller has successfully acquired the concurrentRingBufferAccess lock.
**      failure to do so can result in concurrency issues.
*/
RmRCCommonJournal_RECORD *
_rcdbAllocRecFromRingBuffer
(
    OBJGPU             *pGpu,
    Journal            *pRcDB,
    RMCD_RECORD_TYPE    type
)
{
    RING_BUFFER_LOG    *pRingBuffer = NULL;
    NvU32               newItemIndex;
    RmRCCommonJournal_RECORD
                       *pCommon = NULL;

    // Find the ring buffer for this entry in the collection.
    rcdbFindRingBufferForType(pRcDB, type, &pRingBuffer);

    if (pRingBuffer == NULL)
    {
        NV_ASSERT(0);
        //
        // There is no ring buffer allocated for this type.
        // Nothing we can do about it.
        //
        return NULL;
    }

    newItemIndex = (pRingBuffer->numEntries + pRingBuffer->headIndex) % pRingBuffer->maxEntries;

    // prepend the rmJournalCommon record to record.
    pCommon = (RmRCCommonJournal_RECORD*)(pRingBuffer->pBuffer + (rcdbGetOcaRecordSizeWithHeader(pRcDB, type) * newItemIndex));
    pCommon->Header.cRecordGroup = RmGroup;
    pCommon->Header.cRecordType = type;
    pCommon->Header.wRecordSize = (NvU16)rcdbGetOcaRecordSizeWithHeader(pRcDB, type);
    rcdbSetCommonJournalRecord(pGpu, pCommon);

    // Increment the number of entries or advance the head index.
    if (pRingBuffer->numEntries < pRingBuffer->maxEntries)
    {
        ++pRingBuffer->numEntries;
    }
    else
    {
        ++(pRingBuffer->headIndex);
        if (pRingBuffer->headIndex >= pRingBuffer->maxEntries)
        {
            pRingBuffer->headIndex = 0;
        }
    }
    return pCommon;
}

/*
** rcdbAddRecToRingBuffer_IMPL allocates a buffer entry from the
**  specified ring buffer & copies the supplied data buffer into it.
**
**  parameters:
**      pGpu            a pointer to the GPU object associated with the entry.
**      pRcdb           a pointer toe the Journal that contains the ring buffers
**      type            the record type to locate a buffer for.
**      recordSize      the size of the expected record
**      pRecord         a pointer to the data that will populate the new ring buffer entry.
**
**  notes:
*/
RmRCCommonJournal_RECORD *
rcdbAddRecToRingBuffer_IMPL
(
    OBJGPU             *pGpu,
    Journal            *pRcDB,
    RMCD_RECORD_TYPE    type,
    NvU32               recordSize,
    NvU8               *pRecord
)
{
    RmRCCommonJournal_RECORD *pCommon = NULL;

    NV_ASSERT(recordSize == _rcdbGetOcaRecordSize(pRcDB, type));

    if (portAtomicIncrementS32(&concurrentRingBufferAccess) == 1)
    {
        pCommon = _rcdbAllocRecFromRingBuffer(pGpu, pRcDB, type);
        if (pCommon != NULL)
        {
            // copy the record to follow the common header.
            portMemCopy(&(pCommon[1]), recordSize, pRecord, recordSize);
        }
    }
    portAtomicDecrementS32(&concurrentRingBufferAccess);

    return pCommon;
}

static NvU32 _rcdbGetOcaRecordSize(Journal *pRcDB, RMCD_RECORD_TYPE type)
{
    switch(type)
    {
        case RmRcDiagReport:
            return sizeof(RmRcDiag_RECORD);
            break;
        case RmNocatReport:
            return sizeof(RM_NOCAT_JOURNAL_ENTRY);
            break;
        default:
            return 0;
    }
}

NvU32 rcdbGetOcaRecordSizeWithHeader_IMPL(Journal *pRcDB, RMCD_RECORD_TYPE type)
{
    NvU32 recSz;

    recSz = _rcdbGetOcaRecordSize(pRcDB, type);
    if (0 < recSz)
    {
        recSz += sizeof(RmRCCommonJournal_RECORD);
    }

    //
    // On architecture like RISC-V, loads/stores need to be aligned to the
    // request size (1, 2, 4, 8-byte). Here, OCA record and header are stored
    // in a ring buffer, hence total recSz needs to be 8-byte aligned for both
    // producer (GSP RM) and consumer (CPU RM) of this data.
    //
    return NV_ALIGN_UP(recSz, 8);
}

NV_STATUS
rcdbAddRmGpuDump
(
    OBJGPU *pGpu
)
{
    NV_STATUS           status = NV_OK;
    OBJSYS             *pSys               = SYS_GET_INSTANCE();
    Journal            *pRcDB              = SYS_GET_RCDB(pSys);
    NvDebugDump        *pNvd               = GPU_GET_NVD(pGpu);
    NVD_STATE          *pNvDumpState       = &pRcDB->nvDumpState;
    SYS_ERROR_INFO     *pSysErrorInfo      = &pRcDB->ErrorInfo;
    RMPRBERRORELEMENT_V2 *pPrbErrorInfo    = NULL;
    RMPRBERRORELEMENT_V2 *pErrorList       = NULL;
    RMCD_ERROR_BLOCK   *pNewErrorBlock     = NULL;
    RMERRORHEADER      *pErrorHeader       = NULL;
    PRB_ENCODER         prbEnc;
    NvU32               bufferUsed;
    NvU8               *pBuf               = NULL;

    //
    // The deferred dump codepath will block out other dumps until the DPC can
    // be executed. If this is the deferred callback attempting to do the dump,
    // carry on.
    //
    if (pNvDumpState->bDumpInProcess &&
        !pRcDB->getProperty(pRcDB, PDB_PROP_RCDB_IN_DEFERRED_DUMP_CODEPATH))
    {
        return NV_ERR_STATE_IN_USE;
    }

    prbEnc.depth = 0;
    pNvDumpState->bDumpInProcess    = NV_TRUE;
    pNvDumpState->nvDumpType        = NVD_DUMP_TYPE_OCA;
    pNvDumpState->bRMLock           = rmapiLockIsOwner();

    rcdbDumpInitGpuAccessibleFlag(pGpu, pRcDB);

    //
    // General process:
    //  1. Start the protobuf encoder in ALLOCATE mode, and dump the data
    //  2. Allocate an error element to stick in the Journal list
    //  3. Add the protobuf dump to the error element
    //  4. Put the error element at the end of the error list on OBJRCDB
    //
    status = prbEncStartAlloc(&prbEnc, NVDEBUG_NVDUMP, NVDUMP_MAX_DUMP_SIZE,
                              NULL);
    if (status != NV_OK)
    {
        //
        // If we couldn't allocate the memory, it may be because we're at a
        // raised IRQL. It's not a great idea to be gathering a bunch of state
        // from the interrupt context anyway, so queue a work item to come back
        // later and try again.
        //
        NvU32 *pGpuInstance = NULL;

        //
        // If that's what we've already done and we're still failing, bail out
        // to avoid an infinite fail/queue-work-item loop.
        //
        if (pRcDB->getProperty(pRcDB, PDB_PROP_RCDB_IN_DEFERRED_DUMP_CODEPATH))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "deferred GPU dump encoder init failed (status = 0x%x)\n",
                      status);
            goto done;
        }

        NV_PRINTF(LEVEL_INFO, "deferring GPU dump for normal context\n");

        //
        // This will be freed by the OS work item layer. We pass the GPU
        // instance as the data separately because if the GPU has fallen off
        // the bus, the OS layer may refuse to execute work items attached to
        // it. Instead, use the system work item interface and handle the GPU
        // ourselves.
        //
        pGpuInstance = portMemAllocNonPaged(sizeof(NvU32));
        if (pGpuInstance == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto done;
        }

        *pGpuInstance = gpuGetInstance(pGpu);
        status = osQueueSystemWorkItem(_rcdbAddRmGpuDumpCallback,
                                       pGpuInstance);
        if (status != NV_OK)
        {
            portMemFree(pGpuInstance);
            goto done;
        }

        //
        // Since we've queued the work item, leave the dump state marked as in
        // use to prevent other interrupts and codepaths from attempting to
        // initiate the dump and/or queue a new work item.
        //
        return NV_WARN_MORE_PROCESSING_REQUIRED;
    }

    status = nvdDumpAllEngines(pGpu, pNvd, &prbEnc, pNvDumpState);
    if (status != NV_OK)
    {
        //
        // If the dump failed somewhere, unwind the encoder and then drop
        // through to finish it out so we can get the pointer to the
        // allocated buffer to free.
        //
        while (prbEnc.depth > 1)
        {
            prbEncNestedEnd(&prbEnc);
        }
    }

    bufferUsed = prbEncFinish(&prbEnc, (void **)&pBuf);

    if (status != NV_OK)
    {
        goto done;
    }

    // Allocate and initialize the error element
    pPrbErrorInfo = portMemAllocNonPaged(sizeof(RMPRBERRORELEMENT_V2));
    if (pPrbErrorInfo == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    portMemSet(pPrbErrorInfo, 0, sizeof(RMPRBERRORELEMENT_V2));
    pPrbErrorInfo->RmPrbErrorData.common.Header.cRecordGroup = RmGroup;
    pPrbErrorInfo->RmPrbErrorData.common.Header.cRecordType  = RmPrbFullDump_V2;
    pPrbErrorInfo->RmPrbErrorData.common.Header.wRecordSize  = sizeof(RMPRBERRORELEMENT_V2);
    rcdbSetCommonJournalRecord(pGpu, &(pPrbErrorInfo->RmPrbErrorData.common));
    pErrorHeader = &pPrbErrorInfo->ErrorHeader;
    pErrorHeader->pErrorBlock = NULL;

    //
    // Allocate and initialize the error "block" associated with this protobuf
    // dump
    //
    pNewErrorBlock = portMemAllocNonPaged(sizeof(RMCD_ERROR_BLOCK));
    if (pNewErrorBlock == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    portMemSet(pNewErrorBlock, 0, sizeof(RMCD_ERROR_BLOCK));
    pNewErrorBlock->pBlock = pBuf;
    pNewErrorBlock->blockSize = bufferUsed;
    pNewErrorBlock->pNext = NULL;
    pErrorHeader->pErrorBlock = pNewErrorBlock;

    // Add the error element to the Journal list
    if (pSysErrorInfo->pErrorList != NULL)
    {
        pErrorList = (RMPRBERRORELEMENT_V2*)pSysErrorInfo->pErrorList;
        while (pErrorList->ErrorHeader.pNextError != NULL)
        {
            pErrorList = (RMPRBERRORELEMENT_V2*)pErrorList->ErrorHeader.pNextError;
        }

        pErrorList->ErrorHeader.pNextError = (RMFIFOERRORELEMENT_V3*)pPrbErrorInfo;
    }
    else
    {
        pSysErrorInfo->pErrorList = pPrbErrorInfo;
    }

    pSysErrorInfo->ErrorCount++;

done:
    if (status != NV_OK)
    {
        if (pBuf != NULL)
        {
            portMemFree(pPrbErrorInfo);
            portMemFree(pBuf);
        }
    }

    pNvDumpState->bDumpInProcess = NV_FALSE;
    return status;
}

#if (defined(_WIN32) || defined(_WIN64) || defined(NV_UNIX)) && !defined(NV_MODS)
#if !defined(DEBUG) && !defined(QA_BUILD)
/*
 */
NvBool
rcdProbeGpuPresent(
    OBJGPU  *pGpu,
    NvU64    ip
)
{
    NvU32       testValue;
    NvBool      bFoundLostGpu = NV_FALSE;

    // protect against recursion when probing the GPU.
    if (portAtomicIncrementS32(&probeGpuRecursion) == 1)
    {
        if (NULL != pGpu)
        {
            // is the GPU we are checking allready reported lost?
            if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_PM_CODEPATH) &&
                !pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_LOST))
            {
                testValue = GPU_CHECK_REG_RD32(pGpu, NV_PMC_BOOT_0, (~(pGpu->chipId0)));
                if (testValue == GPU_REG_VALUE_INVALID)
                {
                    // there shouldn't be a need to make a journal entry,
                    // as that should have been done by GPU_CHECK_REG_RD32

                    // Add GPU lost detection to to NvLog.
                    // But skip when nvLog asserts to avoid stack overflow.
#if defined(DEBUG) || defined(QA_BUILD) || ((defined(_WIN32) || defined(_WIN64) || defined(NV_UNIX)) && !defined(NV_MODS))
                    if (portAtomicIncrementS32(&nvLogRecursion) == 1)
#endif
                    {
                        NV_PRINTF(LEVEL_ERROR,
                                  "found GPU %d (0x%p) inaccessible After assert\n",
                                  pGpu->gpuInstance, pGpu);
                    }
#if defined(DEBUG) || defined(QA_BUILD) || ((defined(_WIN32) || defined(_WIN64) || defined(NV_UNIX)) && !defined(NV_MODS))
                    portAtomicDecrementS32(&nvLogRecursion);
#endif
                    bFoundLostGpu = NV_TRUE;
                }
            }
        }
    }
    portAtomicDecrementS32(&probeGpuRecursion);
    return bFoundLostGpu;
}

NvBool
rcdProbeAllGpusPresent(
    NvU64   ip
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    NvBool  bFoundLostGpu = NV_FALSE;
    OBJGPU *pGpu;
    NvU32   gpuMask;
    NvU32   gpuIndex = 0;

    if (pSys->getProperty(pSys, PDB_PROP_SYS_DESTRUCTING))
    {
        return NV_FALSE;
    }

    gpumgrGetGpuAttachInfo(NULL, &gpuMask);
    pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex);
    while (pGpu)
    {
        bFoundLostGpu = bFoundLostGpu  || rcdProbeGpuPresent(pGpu, ip);
        pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex);
    }
    return bFoundLostGpu;
}
#endif // !defined(DEBUG) && !defined(QA_BUILD)
#endif // (defined(_WIN32) || defined(_WIN64) || defined(NV_UNIX)) && !defined(NV_MODS)

void
rcdbAddCrashedFalcon
(
    Falcon *pFlcn
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    Journal *pRcDB = SYS_GET_RCDB(pSys);

    pRcDB->pCrashedFlcn = pFlcn;
}


/*
** _rcdbNocatCollectContext records the context of the GPU at the time the error is reported.
**
**  parameters:
**      pGpu        pointer to GPU to be reported on.
**      pContext    pointer to context structure to be filled in.
**
**   returns:
**      NV_ERR_INVALID_ARGUMENT -- pContext is NULL
*/
NV_STATUS
_rcdbNocatCollectContext(OBJGPU *pGpu, Journal* pRcdb, NV2080_NOCAT_JOURNAL_GPU_STATE* pContext)
{
    NV2080_NOCAT_JOURNAL_GPU_STATE* pContextCache = NULL;
    const char *pTag;

    if (pRcdb == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // determine which tag to use.
    if (pRcdb->nocatJournalDescriptor.tag[0] != '\0')
    {
        pTag = (char *)pRcdb->nocatJournalDescriptor.tag;
    }
    else
    {
        pTag = NOCAT_DEFAULT_TAG_VALUE_STR;
    }
    if (pGpu == NULL)
    {
        // w/o a GPU the only thing we can do is set the tag.
        if (pContext != NULL)
        {
            portMemSet(pContext, 0, sizeof(*pContext));

                portStringCopy((char *)pContext->tag,
                    NV2080_NOCAT_JOURNAL_MAX_STR_LEN,
                    pTag,
                    portStringLength(pTag) + 1);
        }
        return NV_OK;
    }
#if NOCAT_COLLECT_PERF
    pGpuCache = &(pGpu->nocatGpuCache);
#endif
    pContextCache = &(pRcdb->nocatJournalDescriptor.nocatGpuState);

    // insert tag if we have one.
    portStringCopy((char *)pContextCache->tag,
        NV2080_NOCAT_JOURNAL_MAX_STR_LEN,
        pTag,
        portStringLength(pTag) + 1);

    if (!pContextCache->bValid)
    {
        pContextCache->deviceId = (NvU16)(DRF_VAL(_PCI, _DEVID, _DEVICE, pGpu->idInfo.PCIDeviceID));
        pContextCache->vendorId = (NvU16)(DRF_VAL(_PCI, _SUBID, _VENDOR, pGpu->idInfo.PCIDeviceID));
        pContextCache->subsystemVendor = (NvU16)(DRF_VAL(_PCI, _SUBID, _VENDOR, pGpu->idInfo.PCISubDeviceID));
        pContextCache->subsystemId = (NvU16)(DRF_VAL(_PCI, _SUBID, _DEVICE, pGpu->idInfo.PCISubDeviceID));
        pContextCache->revision = pGpu->idInfo.PCIRevisionID;
        pContextCache->type = pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_MOBILE);
        pContextCache->bMsHybrid = FLD_TEST_DRF(_JT_FUNC, _CAPS, _MSHYB_ENABLED, _TRUE,
            pGpu->acpiMethodData.jtMethodData.jtCaps);

        portStringCopy((char *)pContextCache->vbiosProject, NV2080_NOCAT_JOURNAL_MAX_STR_LEN,
            NOCAT_UNKNOWN_STR, portStringLength(NOCAT_UNKNOWN_STR) + 1);

        if (!osIsRaisedIRQL())
        {
            NV_STATUS status = pGpu->acpiMethodData.capsMethodData.status;
            if (status == NV_OK)
            {
                pContextCache->bOptimus =
                    FLD_TEST_DRF(OP_FUNC, _OPTIMUSCAPS, _OPTIMUS_CAPABILITIES,
                        _DYNAMIC_POWER_CONTROL, pGpu->acpiMethodData.capsMethodData.optimusCaps);
            }

            pContextCache->bValid = NV_TRUE;
        }
    }
    if (pContext != NULL)
    {
        portMemSet(pContext, 0, sizeof(*pContext));

        *pContext = *pContextCache;

        pContext->bFullPower = gpuIsGpuFullPower(pGpu);
        pContext->bInGc6Reset = pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_GC6_RESET);
        pContext->bInFullchipReset = pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_FULLCHIP_RESET);
        pContext->bInSecBusReset = pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_SECONDARY_BUS_RESET);
    }
    return NV_OK;
}

/*
**  _rcdbSetTdrReason translates the reason code to a string & puts that string
**  in the provided buffer.
**
**  parameters:
**      tdrReason       the reason code for the TDR
**      pTdrReasonStr   pointer to the place to copy the reason string to
**      maxLen          the size of the buffer pointed to in pTdrReasonStr.
**
*/
void _rcdbSetTdrReason
(
    Journal            *pRcdb,
    NvU32               tdrReason,
    char               *pTdrReasonStr,
    NvU32               maxLen
)
{
    const char *pTmpStr;

    // validate inputs.
    if (pRcdb == NULL)
    {
        return;
    }

    // is there a string buffer & is it large enough to hold more than a NULL string
    if ((pTdrReasonStr == NULL) || (maxLen < 2))
    {
        pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_BAD_PARAM_IDX]++;
        return;
    }
    switch (tdrReason)
    {
    case NV2080_CTRL_NOCAT_TDR_TYPE_NONE:
        pTmpStr = NOCAT_NA_STR;
        break;
    case NV2080_CTRL_NOCAT_TDR_TYPE_LEGACY:
        pTmpStr = NOCAT_LEGACY_STR;
        break;
    case NV2080_CTRL_NOCAT_TDR_TYPE_FULLCHIP:
        pTmpStr = NOCAT_FULLCHIP_TDR_STR;
        break;
    case NV2080_CTRL_NOCAT_TDR_TYPE_BUSRESET:
        pTmpStr = NOCAT_BUS_RESET_TDR_STR;
        break;
    case NV2080_CTRL_NOCAT_TDR_TYPE_GC6_RESET:
        pTmpStr = NOCAT_GC6_RESET_TDR_STR;
        break;
    case NV2080_CTRL_NOCAT_TDR_TYPE_SURPRISE_REMOVAL:
        pTmpStr = NOCAT_SURPRISE_REMOVAL_TDR_STR;
        break;
    case NV2080_CTRL_NOCAT_TDR_TYPE_UCODE_RESET:
        pTmpStr = NOCAT_UCODE_RESET_TDR_STR;
        break;
    default:
        pTmpStr = NOCAT_UNKNOWN_STR;
        break;
    }
    portStringCopy(pTdrReasonStr, maxLen,
        pTmpStr, portStringLength(pTmpStr) + 1);
}

/*
** _rcdbAllocNocatJournalRecord allocates a buffer entry from the Journal ring buffer
**  for the specified type
**
**  parameters:
**      pGpu            a pointer to the GPU object associated with the entry.
**      pRcdb           a pointer toe the Journal that contains the ring buffers
**      type            the record type to locate a buffer for.
**
**  returns a pointer to a record in the ring buffer, or NULL if a record could not be allocated.
**
**  notes:
**      it is assumed the caller has successfully acquired the concurrentRingBufferAccess lock.
**      the lock should be held until access the buffer is completed.
**      failure to do so can result in concurrency issues.
**
**      if successful, the buffer that is returned is cleared & an id assigned.
*/
RM_NOCAT_JOURNAL_ENTRY* _rcdbAllocNocatJournalRecord
(
    OBJGPU     *pGpu,
    OBJRCDB    *pRcdb,
    RmRCCommonJournal_RECORD   **ppCommon
)
{
    nocatQueueDescriptor   *pDesc = NULL;
    RmRCCommonJournal_RECORD* pCommon;
    RM_NOCAT_JOURNAL_ENTRY * pNocatEntry = NULL;

    // make sure someone has the lock.
    if (concurrentRingBufferAccess == 0)
    {
        return NULL;
    }

    pDesc = &pRcdb->nocatJournalDescriptor;

    // Get the next record from the appropriate nocat ring buffer.
    pCommon = _rcdbAllocRecFromRingBuffer(
        pGpu,
        pRcdb,
        RmNocatReport);

    if (pCommon != NULL)
    {
        // advance the pointer past the common header.
        pNocatEntry = (RM_NOCAT_JOURNAL_ENTRY*)(((NvU8*)pCommon) + sizeof(RmRCCommonJournal_RECORD));

        // clear the record & assign an id.
        portMemSet(pNocatEntry, 0, sizeof(*pNocatEntry));
        pNocatEntry->id = pDesc->nextRecordId++;
        pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_ALLOCATED_IDX]++;
        portAtomicIncrementS32(&pNocatEntry->inUse);
    }
    else
    {
        pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_ALLOC_FAILED_IDX]++;
    }
    if (ppCommon != NULL)
    {
        *ppCommon = pCommon;
    }
    return pNocatEntry;
}

/*
** _rcdbGetNocatJournalRecord returns a pointer to the requested record,
**      or optionally the oldest record if the requested one is not available.
**
**  parameters:
**      pRcdb           a pointer toe the Journal that contains the ring buffers
**      id              id of the record we are looking for
**      bExactMatch     indicates if we want an exact match, or the closest record.
**      ppCommon        a pointer to a pointer that will hold the pointer to
**                      the common part of the record.
**                      this can be NULL
**      ppReturnedNocatEntry
**                      a pointer to a pointer that will hold the pointer to
**                      the nocat part of the record
**                      this can be NULL
**
**  notes:
**      it is assumed the caller has successfully acquired the concurrentRingBufferAccess lock.
**      the lock should be held until access the buffer is completed.
**      failure to do so can result in concurrency issues.
*/
NV_STATUS
_rcdbGetNocatJournalRecord
(
    OBJRCDB            *pRcdb,
    NvU32               reqId,
    NvBool              bExactMatch,
    RmRCCommonJournal_RECORD
                      **ppReturnedCommon,
    RM_NOCAT_JOURNAL_ENTRY
                      **ppReturnedNocatEntry
)
{
    nocatQueueDescriptor     *pDesc;
    RmRCCommonJournal_RECORD *pCommon = NULL;
    RM_NOCAT_JOURNAL_ENTRY   *pNocatEntry = NULL;
    RING_BUFFER_LOG          *pRingBuffer = NULL;
    NvS32                     offset;
    NvS32                     idx;

    // make sure someone has the lock.
    if (concurrentRingBufferAccess == 0)
    {
        return NV_ERR_BUSY_RETRY;
    }

    // is there anything to do
    if ((ppReturnedCommon == NULL) && (ppReturnedNocatEntry == NULL))
    {
        return NV_OK;
    }

    // validate inputs.
    if (pRcdb == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    pDesc = &pRcdb->nocatJournalDescriptor;

    // assume we will fail
    if (ppReturnedCommon != NULL)
    {
        *ppReturnedCommon = NULL;
    }
    if (ppReturnedNocatEntry != NULL)
    {
        *ppReturnedNocatEntry = NULL;
    }

    // if there is nothing in the buffer,
    // we can't return a record.
    if ((pDesc->nextRecordId - pDesc->nextReportedId) == 0)
    {
        pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_NO_RECORDS_IDX]++;
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    // Find the ring buffer for the diag reports
    rcdbFindRingBufferForType(pRcdb, RmNocatReport, &pRingBuffer);
    if (pRingBuffer == NULL)
    {
        pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_BAD_BUFFER_IDX]++;
        return NV_ERR_OBJECT_NOT_FOUND;
    }
    // determine how far back from the head our record should be.
    offset = pDesc->nextRecordId - reqId;

    // start of from the next record we will replace.
    // this will be the oldest buffer in the record,
    // or the next empty record, either way, we will wrap to the right one
    idx = pRingBuffer->headIndex;

    // is the requested record in the buffer?
    if ((0 <= offset) && ((NvU16)offset <= pRingBuffer->numEntries))
    {
        // back out the offset from the newest/empty record.
        idx += pRingBuffer->numEntries - offset;
        pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_MATCH_FOUND_IDX]++;
    }
    else if (bExactMatch)
    {
        // the record is not in the buffer, & we weren't asked for the closest match.
        pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_NO_MATCH_IDX]++;
        return NV_ERR_OBJECT_NOT_FOUND;
    }
    else
    {
        pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_CLOSEST_FOUND_IDX]++;
    }
    // wrap the idx to the current size of the buffer.
    idx %= pRingBuffer->numEntries;

    // get a pointer to the common record & the record from the buffer.
    pCommon = (RmRCCommonJournal_RECORD*)(((NvU8*)pRingBuffer->pBuffer) + (rcdbGetOcaRecordSizeWithHeader(pRcdb, RmNocatReport) * idx));

    // get a pointer to the data that follows the common header, that is the record data.
    pNocatEntry = (RM_NOCAT_JOURNAL_ENTRY*)(((NvU8*)pCommon) + sizeof(RmRCCommonJournal_RECORD));
    portAtomicIncrementS32(&pNocatEntry->inUse);

    // pass the record along
    if (ppReturnedCommon != NULL)
    {
        *ppReturnedCommon = pCommon;
    }
    if (ppReturnedNocatEntry != NULL)
    {
        *ppReturnedNocatEntry = pNocatEntry;
    }
    return NV_OK;
}
/*
** _rcdbGetNocatJournalRecord returns a pointer to the requested record,
**      or optionally the oldest record if the requested one is not available.
**
**  parameters:
**      pRcdb           a pointer toe the Journal that contains the ring buffers
**      id              id of the record we are looking for
**      bExactMatch     indicates if we want an exact match, or the closest record.
**      ppCommon        a pointer to a pointer that will hold the pointer to
**                      the common part of the record.
**                      this can be NULL
**      ppReturnedNocatEntry
**                      a pointer to a pointer that will hold the pointer to
**                      the nocat part of the record
**                      this can be NULL
**
**  notes:
**      it is assumed the caller has successfully acquired the concurrentRingBufferAccess lock.
**      the lock should be held until access the buffer is completed.
**      failure to do so can result in concurrency issues.
*/
NV_STATUS
_rcdbReleaseNocatJournalRecord
(
    RM_NOCAT_JOURNAL_ENTRY  *pNocatEntry
)
{
    if (pNocatEntry == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    if (portAtomicDecrementS32(&pNocatEntry->inUse) != 0)
    {
        return NV_ERR_BUSY_RETRY;
    }
    return NV_OK;
}

/*
** _rcdbGetNewestNocatJournalRecordForType returns a pointer to the newest record for the
**  specified type if there is one.
**
**  parameters:
**      pRcdb           a pointer toe the Journal that contains the ring buffers
**      type            type of record we want.
**      ppCommon        a pointer to a pointer that will hold the pointer to
**                      the common part of the record.
**                      this can be NULL
**      ppCommon        a pointer to a pointer that will hold the pointer to
**                      the nocat part of the record
**                      this can be NULL
**
**  notes:
**      it is assumed the caller has successfully acquired the concurrentRingBufferAccess lock.
**      the lock should be held until access the buffer is completed.
**      failure to do so can result in concurrency issues.
*/
NV_STATUS
_rcdbGetNewestNocatJournalRecordForType
(
    OBJRCDB            *pRcdb,
    NvU32               type,
    RmRCCommonJournal_RECORD
                      **ppReturnedCommon,
    RM_NOCAT_JOURNAL_ENTRY
                      **ppReturnedNocatEntry
)
{
    if (type >= NV2080_NOCAT_JOURNAL_REC_TYPE_COUNT)
    {
        // we failed
        if (ppReturnedCommon != NULL)
        {
            *ppReturnedCommon = NULL;
        }
        if (ppReturnedNocatEntry != NULL)
        {
            *ppReturnedNocatEntry = NULL;
        }
        return NV_ERR_OBJECT_NOT_FOUND;
    }
    return _rcdbGetNocatJournalRecord(pRcdb, pRcdb->nocatJournalDescriptor.lastRecordId[type], NV_TRUE,
        ppReturnedCommon, ppReturnedNocatEntry);
}

/*
** rcdbReportNextNocatJournalEntry fills in the provided Nocat Journal record with the next record
** to be reported, then updates the last reported id.
**
**  parameters:
**      pReturnedNocatEntry a pointer to the buffer where the journal record will be transferred to
**
**  returns:
**      NV_OK -- the record was successfully updated with the next record to report.
**      NV_ERR_INVALID_ARGUMENT -- the provided pointer is NULL
**      NV_ERR_OBJECT_NOT_FOUND -- we could not locate a record to report.
**
**  notes:
**      we are transferring the record to the target location here instead of passing a pointer
**      to insure the data is transferred while we hold the concurrentRingBufferAccess lock.
**      failure to do so can result in concurrency issues.
**
**      priority is determined by the record journal queue values. the lower value has
**      higher priority.
**
**      now that we have moved from a single entry, to a queue, we need to
**      consume the entry once we report it
**
*/
NV_STATUS
rcdbReportNextNocatJournalEntry
(
    NV2080_NOCAT_JOURNAL_RECORD
                       *pReturnedNocatEntry
)
{
    OBJSYS                   *pSys = SYS_GET_INSTANCE();
    Journal                  *pRcdb = SYS_GET_RCDB(pSys);
    NV_STATUS                 status = NV_ERR_OBJECT_NOT_FOUND;
    nocatQueueDescriptor     *pDesc;
    RmRCCommonJournal_RECORD *pCommon = NULL;
    RM_NOCAT_JOURNAL_ENTRY   *pNocatEntry = NULL;

    // validate inputs.
    if (pRcdb == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_REQUESTED_IDX]++;

    if (pReturnedNocatEntry == NULL)
    {
        pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_BAD_PARAM_IDX]++;
        return NV_ERR_INVALID_ARGUMENT;
    }
    portMemSet(pReturnedNocatEntry, 0, sizeof(*pReturnedNocatEntry));

    if (portAtomicIncrementS32(&concurrentRingBufferAccess) == 1)
    {
        pDesc = &pRcdb->nocatJournalDescriptor;
        _rcdbGetNocatJournalRecord(pRcdb,
            pDesc->nextReportedId, NV_FALSE,
            &pCommon, &pNocatEntry);
        if ((pCommon != NULL) && (pNocatEntry != NULL))
        {
            // we have a record, push it into the return buffer
            pReturnedNocatEntry->GPUTag = pCommon->GPUTag;

            // copy over the data into the supplied buffer.
            pReturnedNocatEntry->loadAddress = pDesc->loadAddress;
            pReturnedNocatEntry->timeStamp = pCommon->timeStamp;
            pReturnedNocatEntry->stateMask = pCommon->stateMask;
            pReturnedNocatEntry->nocatGpuState = pNocatEntry->nocatGpuState;
            pReturnedNocatEntry->nocatJournalEntry = pNocatEntry->nocatJournalEntry;

            // check if we lost any records.
            pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_DROPPED_IDX] +=
                pNocatEntry->id - pDesc->nextReportedId;

            // update the NocatJournalNextReportedId
            pDesc->nextReportedId = pNocatEntry->id + 1;
            pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_REPORTED_IDX]++;

            _rcdbReleaseNocatJournalRecord(pNocatEntry);
            status = NV_OK;

        }
    }
    else
    {
        pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_BUSY_IDX]++;
        status = NV_ERR_BUSY_RETRY;
    }
    portAtomicDecrementS32(&concurrentRingBufferAccess);
    if ((pRcdb->nocatJournalDescriptor.lockTimestamp != 0) && (rcdbGetNocatOutstandingCount(pRcdb) == 0))
    {
        pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_JOURNAL_UNLOCKED_IDX]++;
        pRcdb->nocatJournalDescriptor.lockTimestamp = 0;
    }
    return status;
}

/*
** rcdbGetNocatOutstandingCount returns the number of NOCAT events that have
** been recorded since the last reported record.
**
**  parameters:
**      pRcdb -- a pointer to the Journal object.
**
**  returns:
**      number of NOCAT events that have been recorded since the last reported record.
**      or NV_U32_MAX if a NULL journal object pointer is provided.
**
**  notes:
**      the returned count includes records that have been dropped due to wrapping.
**
*/
NvU32
rcdbGetNocatOutstandingCount(Journal *pRcdb)
{
    NvU32 count = NV_U32_MAX;
    if (pRcdb != NULL)
    {
        if (portAtomicIncrementS32(&concurrentRingBufferAccess) == 1)
        {
            count = pRcdb->nocatJournalDescriptor.nextRecordId
                - pRcdb->nocatJournalDescriptor.nextReportedId;
        }
        portAtomicDecrementS32(&concurrentRingBufferAccess);
    }
    return count;
}

/*
** _rcdbSendNocatJournalNotification sends an ETW Notification that a NOCAT Journal record has been posted.
**
**  parameters:
**      pGpu -- a pointer to the GPU object associated with teh new entry
**              (may be NULL)
**      pRcdb -- a pointer to the Journal object NOCAT is using.
**      posted -- the number of records posted since the last record that was retrieved.
**      pCommon -- a pointer to the common record header associated with the record.
**      type -- the record type
**
**  returns:
**      NV_OK -- the call to post the record was made.
**          note that the call to post the record does not return a status,
**          so we do not know if the call was successful.
**      NV_ERR_INVALID_ARGUMENT -- one of the required pointers is NULL
**
*/
NV_STATUS
_rcdbSendNocatJournalNotification
(
    OBJGPU *pGpu,
    Journal *pRcdb,
    NvU32    posted,
    NvU64    timeStamp,
    NvU32 type
)
{
    if (pRcdb == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    if (pRcdb->nvDumpState.bDumpInProcess)
    {
        // we can't reliably do an ETW when we are in the process of collecting data for a dump.
        // (it can throw an exception)
        // so bail w/o sending the notification.
        pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_NOTIFICATION_FAIL_IDX]++;
        return NV_OK;
    }

    RMTRACE_NOCAT(_REPORT_PENDING, (pGpu ? pGpu->gpuId : RMTRACE_UNKNOWN_GPUID),
        RmNocatReport,
        posted,
        type,
        rcdbGetNocatOutstandingCount(pRcdb),
        timeStamp);

    // count the number of notifications.
    pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_NOTIFICATIONS_IDX]++;
    return NV_OK;
}

/*
** rcdbInitNocatGpuCache_IMPL initializes a per GPU cache held in the GPU object to be used by NOCAT
**
**  parameters:
**      pGpu -- a pointer to the GPU Object the containing the cache
**
**  notes:
**      this function:
**      * caches the driver load address
**      * allocates a small block of memory in the frame buffer for testing
**      * initializes the GPU context cache
**
*/
void rcdbInitNocatGpuCache_IMPL(OBJGPU *pGpu)
{
    OS_DRIVER_BLOCK         driverBlock;
    OBJSYS                 *pSys = SYS_GET_INSTANCE();
    Journal                *pRcdb = SYS_GET_RCDB(pSys);
#if NOCAT_PROBE_FB_MEMORY
    NvU8                   *pCpuPtr;
    NV_STATUS              status;
#endif

    if (pGpu == NULL)
    {
        return;
    }
    portMemSet(&driverBlock, 0x00, sizeof(driverBlock));
    if (osGetDriverBlock(pGpu->pOsGpuInfo, &driverBlock) == NV_OK)
    {
        pRcdb->nocatJournalDescriptor.loadAddress = (NvU64)driverBlock.driverStart;
    }

#if NOCAT_PROBE_FB_MEMORY
    // Allocate some memory for virtual BAR2 testing
    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM) && !IsAMODEL(pGpu))
    {
        memdescCreateExisting(&pGpu->nocatGpuCache.fbTestMemDesc,
            pGpu, NOCAT_FBSIZETESTED, ADDR_FBMEM, NV_MEMORY_UNCACHED, MEMDESC_FLAGS_NONE);
        memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_102, 
                        (&pGpu->nocatGpuCache.fbTestMemDesc));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Could not allocate vidmem for NOCAT bar2 testing\n");
            return;
        }
        pCpuPtr = kbusMapRmAperture_HAL(pGpu, &pGpu->nocatGpuCache.fbTestMemDesc);
        if (pCpuPtr == NULL)
        {
            memdescFree(&pGpu->nocatGpuCache.fbTestMemDesc);
            memdescDestroy(&pGpu->nocatGpuCache.fbTestMemDesc);
            pGpu->nocatGpuCache.pCpuPtr = NULL;
            return;
        }
        pGpu->nocatGpuCache.pCpuPtr = pCpuPtr;
    }
#endif
    // initialize the context cache
    if (portAtomicIncrementS32(&concurrentRingBufferAccess) == 1)
    {
        _rcdbNocatCollectContext(pGpu, pRcdb, NULL);
    }
    portAtomicDecrementS32(&concurrentRingBufferAccess);

    return;
}

/*
** rcdbCleanupNocatGpuCache_IMPL returns per GPU resources used by NOCAT.
**
**  parameters:
**      pGpu -- a pointer to the GPU Object the containing the cache
**
**  notes:
**      This will free up the FB test window if allocated, and clear out the cache
**
*/
void rcdbCleanupNocatGpuCache_IMPL(OBJGPU *pGpu)
{
#if NOCAT_PROBE_FB_MEMORY
    if (pGpu == NULL)
    {
        return;
    }
    if (pGpu->nocatGpuCache.pCpuPtr != NULL)
    {
        kbusUnmapRmApertureWithFlags_HAL(pGpu, &pGpu->nocatGpuCache.fbTestMemDesc,
            &pGpu->nocatGpuCache.pCpuPtr, TRANSFER_FLAGS_NONE);
        memdescFree(&pGpu->nocatGpuCache.fbTestMemDesc);
        memdescDestroy(&pGpu->nocatGpuCache.fbTestMemDesc);
    }
    portMemSet(&pGpu->nocatGpuCache, 0, sizeof(pGpu->nocatGpuCache));
#endif

    return;
}



/*
** rcdbNocatInsertNocatError records a reported NOCAT error
**
**  parameters:
**      pGpu        Pointer to GPU associated with the error
**                  may be NULL if there is no GPU associated with the error
**                  if NULL the primary GPU is used
**      pNewEntry   A pointer to a structure that contains all the available data for the report
*/
NvU32
rcdbNocatInsertNocatError(
    OBJGPU *pGpu,
    NOCAT_JOURNAL_PARAMS *pNewEntry
)
{
    OBJSYS                     *pSys = SYS_GET_INSTANCE();
    Journal                    *pRcdb = SYS_GET_RCDB(pSys);
#if(NOCAT_PROBE_FB_MEMORY)
    NvBool                      bCheckFBState = NV_FALSE;
#endif
    RmRCCommonJournal_RECORD   *pCommon = NULL;
    RM_NOCAT_JOURNAL_ENTRY     *pNocatEntry = NULL;
    NvU32                       id = INVALID_RCDB_NOCAT_ID;
    const char                 *pSource = NULL;
    NvU32                       diagBufferLen = 0;
    const char                 *pFaultingEngine = NULL;
    NvBool                      postRecord;
    // validate inputs.
    if (pRcdb == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_COLLECT_REQ_IDX]++;
    if (pNewEntry == NULL)
    {
        pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_BAD_PARAM_IDX]++;
        return 0;
    }
    // assign a timestamp if none was provided
    if (pNewEntry->timestamp == 0)
    {
        pNewEntry->timestamp = osGetTimestamp();
    }

    // initially set postRecord based on the current state of the lock;
    postRecord = pRcdb->nocatJournalDescriptor.lockTimestamp == 0;

    // perform any record type specific setup
    switch (pNewEntry->recType)
    {
    case NV2080_NOCAT_JOURNAL_REC_TYPE_BUGCHECK:
#if(NOCAT_PROBE_FB_MEMORY)
        bCheckFBState = NV_TRUE;
#endif
        // fall thru

    case NV2080_NOCAT_JOURNAL_REC_TYPE_TDR:
        // lock the journal so we don't wrap over the record we are inserting.
        if (pRcdb->nocatJournalDescriptor.lockTimestamp == 0)
        {
            pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_JOURNAL_LOCKED_IDX]++;
        }
        else
        {
            pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_JOURNAL_LOCK_UPDATED_IDX]++;
        }

        pRcdb->nocatJournalDescriptor.lockTimestamp = pNewEntry->timestamp;
        postRecord = NV_TRUE;
        break;

    case NV2080_NOCAT_JOURNAL_REC_TYPE_RC:
#if(NOCAT_PROBE_FB_MEMORY)
        bCheckFBState = NV_TRUE;
#endif
        // set the source
        pSource = "RC Error";
        break;

    case NV2080_NOCAT_JOURNAL_REC_TYPE_ASSERT:
        // set the source
        pSource = "ASSERT";
        break;

    case NV2080_NOCAT_JOURNAL_REC_TYPE_ENGINE:
        break;

    case NV2080_NOCAT_JOURNAL_REC_TYPE_UNKNOWN:
    default:
        return 0;
        break;
    }
    // check if we should post the record when locked.
    if (!postRecord)
    {
        if ((NvS64)(pNewEntry->timestamp - pRcdb->nocatJournalDescriptor.lockTimestamp) < 0)
        {
            // the record predates the lock, so it's Grandfathered in.
            postRecord = NV_TRUE;
            pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_GRANDFATHERED_RECORD_IDX]++;
        }
        else
        {
            // we are dropping the record, count that.
            pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_COLLECT_LOCKED_OUT_IDX]++;
        }
    }
    if (postRecord)
    {
        // is the buffer available?
        if (portAtomicIncrementS32(&concurrentRingBufferAccess) == 1)
        {
            // start recording this new record by allocating a record from the buffer.
            pNocatEntry = _rcdbAllocNocatJournalRecord(pGpu, pRcdb, &pCommon);
            if (pNocatEntry != NULL)
            {
                pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_COLLECTED_IDX]++;

                // update the time stamp to the one supplied.
                pCommon->timeStamp = pNewEntry->timestamp;

                // save the record Id for the type.
                pRcdb->nocatJournalDescriptor.lastRecordId[pNewEntry->recType] =
                    pRcdb->nocatJournalDescriptor.lastRecordId[NV2080_NOCAT_JOURNAL_REC_TYPE_ANY] =
                    pRcdb->nocatJournalDescriptor.nextRecordId - 1;

                // set the type.
                pNocatEntry->nocatJournalEntry.recType = pNewEntry->recType;

                // set bugcheck
                pNocatEntry->nocatJournalEntry.bugcheck = pNewEntry->bugcheck;

                // get context
                _rcdbNocatCollectContext(pGpu, pRcdb, &(pNocatEntry->nocatGpuState));

#if(NOCAT_PROBE_FB_MEMORY)
                if ((bCheckFBState)
                    && (pGpu != NULL)
                    && (pGpu->nocatGpuCache.pCpuPtr != NULL)
                    // If using Coherent CPU mapping instead of BAR2 do not call VerifyBar2
                    && !pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING))
                {
                    switch (kbusVerifyBar2_HAL(pGpu, GPU_GET_KERNEL_BUS(pGpu),
                        &pGpu->nocatGpuCache.fbTestMemDesc, pGpu->nocatGpuCache.pCpuPtr, 0, NOCAT_FBSIZETESTED))
                    {
                    case NV_OK:                     // everything passed
                        break;

                    case NV_ERR_MEMORY_ERROR:       // BAR 0 failed & BAR 2 was not checked, or BAR 2 failed
                        // for now we don't know which BAR failed, so mark both.
                        // but only one BAR failed.
                        // (if BAR 0 Failed, BAR 2 was not checked)
                        pCommon->stateMask |=
                            NV_RM_JOURNAL_STATE_MASK_VIDMEM_FAILED_BAR0
                            | NV_RM_JOURNAL_STATE_MASK_VIDMEM_FAILED_BAR2;
                        break;

                    default:                        // some other processing error cause us to not test the BAR
                        break;
                    }
                }
#endif
                // is there a valid string for source?
                // (non NULL ptr & more than just a termination)
                if ((pNewEntry->pSource != NULL) && (pNewEntry->pSource[0] != '\0'))
                {
                    // yes, use that.
                    pSource = pNewEntry->pSource;
                }
                // the caller did not supply a source,
                // did we set a default source based on record type?
                else if (pSource == NULL)
                {
                    // no, supply the unknown string for source.
                    pSource = NOCAT_UNKNOWN_STR;
                }
                portStringCopy((char*)pNocatEntry->nocatJournalEntry.source,
                    NV2080_NOCAT_JOURNAL_MAX_STR_LEN,
                    pSource,
                    portStringLength(pSource) + 1);

                pNocatEntry->nocatJournalEntry.subsystem = pNewEntry->subsystem;
                pNocatEntry->nocatJournalEntry.errorCode = pNewEntry->errorCode;

                if ((pNewEntry->pDiagBuffer != NULL) && (pNewEntry->diagBufferLen != 0))
                {
                    // checking length here as we don't want portMemCopy to assert
                    if (pNewEntry->diagBufferLen < NV_ARRAY_ELEMENTS(pNocatEntry->nocatJournalEntry.diagBuffer))
                    {
                        diagBufferLen = pNewEntry->diagBufferLen;
                    }
                    else
                    {
                        // make best effort
                        diagBufferLen = NV_ARRAY_ELEMENTS(pNocatEntry->nocatJournalEntry.diagBuffer);
                    }
                    portMemCopy(pNocatEntry->nocatJournalEntry.diagBuffer,
                        sizeof(pNocatEntry->nocatJournalEntry.diagBuffer),
                        pNewEntry->pDiagBuffer, diagBufferLen);
                }
                pNocatEntry->nocatJournalEntry.diagBufferLen = diagBufferLen;

                pFaultingEngine = pNewEntry->pFaultingEngine != NULL ?
                    pNewEntry->pFaultingEngine : NOCAT_UNKNOWN_STR;

                portStringCopy((char*)pNocatEntry->nocatJournalEntry.faultingEngine,
                    NV2080_NOCAT_JOURNAL_MAX_STR_LEN,
                    pFaultingEngine, portStringLength(pFaultingEngine) + 1);

                _rcdbSetTdrReason(pRcdb, pNewEntry->tdrReason,
                    (char*)pNocatEntry->nocatJournalEntry.tdrReason,
                    sizeof(pNocatEntry->nocatJournalEntry.tdrReason));

                _rcdbReleaseNocatJournalRecord(pNocatEntry);
            }
            else
            {
                // record was not allocated, bail.
                postRecord = NV_FALSE;
                pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_COLLECT_FAILED_IDX]++;
            }
        }
        else
        {
            // we are busy, so we can't insert the record, count the record as dropped & count the busy.
            postRecord = NV_FALSE;
            pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_BUSY_IDX]++;
        }
        portAtomicDecrementS32(&concurrentRingBufferAccess);
    }

    // no matter what happened, trigger the event to indicate a record was processed.
    _rcdbSendNocatJournalNotification(pGpu, pRcdb, postRecord, pNewEntry->timestamp, pNewEntry->recType);

    return id;
}
/*
** rcdbNocatInsertBugcheck is the interface to record a bugcheck NOCAT report
**
**  parameters:
**      deviceInstance  The instance of the GPU associated with the bugcheck.
**      bugcheck        The bugcheck number
*/
NvU32
rcdbNocatInsertBugcheck
(
    NvU32               deviceInstance,
    NvU32               bugCheckCode)
{
    NOCAT_JOURNAL_PARAMS newEntry;

    portMemSet(&newEntry, 0, sizeof(newEntry));
    newEntry.recType = NV2080_NOCAT_JOURNAL_REC_TYPE_BUGCHECK;
    newEntry.bugcheck = bugCheckCode;
    newEntry.pSource = "OS";
    newEntry.errorCode = bugCheckCode;
    return rcdbNocatInsertNocatError(gpumgrGetGpu(deviceInstance), &newEntry);
}

/*
** rcdbNocatInitEngineErrorEvent initializes a parameter structure for an engine error event
**
**  parameters:
**      pNewEntry       Pointer to event parameter structure to be initialized
*/
NV_STATUS
rcdbNocatInitEngineErrorEvent
(
    NOCAT_JOURNAL_PARAMS *pNewEntry
)
{
    if (pNewEntry == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    portMemSet(pNewEntry, 0, sizeof(*pNewEntry));
    pNewEntry->recType = NV2080_NOCAT_JOURNAL_REC_TYPE_ENGINE;
    return NV_OK;
}

/*
** rcdbNocatInsertEngineError records a reported NOCAT error from an engine,
**
**  parameters:
**      pGpu            Pointer to GPU associated with the error
**                      may be NULL if there is no GPU associated with the error
**                      if NULL the primary GPU is used
**      pSource         A string indicating the reporting source of the error.
**                      if NULL, a default values will be used
**      subsystem       The optional subsystem ID used by the source to identify the error
**      errorCode       The error code
**      pDiagBuffer     A pointer to the diagnostic buffer associated with the error
**                      may be NULL
**      diagBufferLen   The size of the diagnostic buffer
**                      if the size exceeds the supported diagBuffer size, the buffer contents will be truncated to fit.
*/
NvU32
rcdbNocatInsertEngineError(
    OBJGPU             *pGpu,
    const char         *pSource,
    NvU32               subsystem,
    NvU64               errorCode,
    NvU8               *pDiagBuffer,
    NvU32               diagBufferLen
)
{
    NOCAT_JOURNAL_PARAMS newEntry;

    rcdbNocatInitEngineErrorEvent(&newEntry);
    newEntry.pSource = pSource;
    newEntry.subsystem = subsystem;
    newEntry.errorCode = errorCode;
    newEntry.pDiagBuffer = pDiagBuffer;
    newEntry.diagBufferLen = diagBufferLen;
    return rcdbNocatInsertNocatError(pGpu, &newEntry);
}

/*
** rcdbNocatInsertTDRError records an TDR error,
**
**  parameters:
**      pGpu            Pointer to GPU associated with the error
**                      may be NULL if there is no GPU associated with the error
**                      if NULL the primary GPU is used
**      pSource         A string indicating the reporting source of the error.
**                      if NULL, a default values will be used
**      subsystem       The optional subsystem ID used by the source to identify the error
**      errorCode       The error code
**      TDRBucket       The TDR bucket
**      pDiagBuffer     A pointer to the diagnostic buffer associated with the error
**                      may be NULL
**      diagBufferLen   The size of the diagnostic buffer
**                      if the size exceeds the supported diagBuffer size,
**                      the buffer contents will be truncated to fit.
**      tdrReason       A reason code for the TDR
**      pFaultingApp    A pointer to the faulting app name if known
*/
NvU32
rcdbNocatInsertTDRError
(
    OBJGPU             *pGpu,
    const char         *pSource,
    NvU32               subsystem,
    NvU64               errorCode,
    NvU32               TdrBucket,
    NvU8               *pDiagBuffer,
    NvU32               diagBufferLen,
    NvU32               tdrReason,
    const char         *pFaultingEngine
)
{
    NOCAT_JOURNAL_PARAMS newEntry;

    portMemSet(&newEntry, 0, sizeof(newEntry));
    newEntry.recType = NV2080_NOCAT_JOURNAL_REC_TYPE_TDR;
    newEntry.pSource = pSource;
    newEntry.subsystem = subsystem;
    newEntry.errorCode = errorCode;
    newEntry.pDiagBuffer = pDiagBuffer;
    newEntry.diagBufferLen = diagBufferLen;
    newEntry.pFaultingEngine = pFaultingEngine;
    return rcdbNocatInsertNocatError(pGpu, &newEntry);
}
NV_STATUS
rcdbNocatInitRCErrorEvent
(
    NOCAT_JOURNAL_PARAMS *pNewEntry
)
{
    if (pNewEntry == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    portMemSet(pNewEntry, 0, sizeof(*pNewEntry));
    pNewEntry->recType = NV2080_NOCAT_JOURNAL_REC_TYPE_RC;
    pNewEntry->pSource = "RC ERROR";
    return NV_OK;
}

/*
** _rcdbNocatReportAssert adds an assert record.
**
**  parameters:
**      pGpu        Pointer to GPU associated with the error
**                  may be NULL
**      pAssertRec  A pointer to the assert to report
*/
NV_STATUS
_rcdbNocatReportAssert
(
    OBJGPU                  *pGpu,
    RmRCCommonAssert_RECORD *pAssertRec
)
{
    OBJSYS                 *pSys = SYS_GET_INSTANCE();
    Journal                *pRcdb = SYS_GET_RCDB(pSys);
    NOCAT_JOURNAL_PARAMS    newEntry;
    RM_NOCAT_ASSERT_DIAG_BUFFER    diagBuffer;
    RM_NOCAT_ASSERT_DIAG_BUFFER   *pDiagData;
    NvU32                   idx;
    RM_NOCAT_JOURNAL_ENTRY *pNocatEntry = NULL;
    NvU32                   gpuCnt= 0;
    OBJGPU                  *pTmpGpu = gpumgrGetGpu(0);
    NvBool                  recordPosted = NV_FALSE;


    // validate inputs.
    if ((pRcdb == NULL) || (pAssertRec == NULL))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    if (pGpu == NULL)
    {
        // we don't have a GPU, if there is only 1 GPU,
        // we can safely use it for logging this assert
        gpumgrGetGpuAttachInfo(&gpuCnt, NULL);
        if (gpuCnt == 1)
        {
            pGpu = pTmpGpu;
        }
    }

    // start off assuming we will be recording a report
    portMemSet(&newEntry, 0, sizeof(newEntry));
    newEntry.timestamp = pAssertRec->common.timeStamp;
    newEntry.recType = NV2080_NOCAT_JOURNAL_REC_TYPE_ASSERT;
    newEntry.pSource = "ASSERT";

    // save the assert point as the error code.
    newEntry.errorCode =
        (NvU32)((pAssertRec->breakpointAddrHint - pRcdb->nocatJournalDescriptor.loadAddress)
            & 0xffffffff);

    // put the line number in the upper 32 bits.
    newEntry.errorCode |= ((NvU64)pAssertRec->lineNum) << 32;

    for (idx = 0; idx < NV_ARRAY_ELEMENTS(pAssertRec->callStack); idx++)
    {
        diagBuffer.callStack[idx] =
            (NvU32)((pAssertRec->callStack[idx] - pRcdb->nocatJournalDescriptor.loadAddress)
                & 0xffffffff);
    }
    // initialize count
    diagBuffer.count = 1;

    // setup the pointer to our diag buffer & its length
    newEntry.pDiagBuffer = (NvU8 *)&diagBuffer;

    newEntry.diagBufferLen = sizeof(diagBuffer);

    // is the last thing we logged an assert, & is this the same assert?
    if ((pRcdb->nocatJournalDescriptor.lastRecordId[NV2080_NOCAT_JOURNAL_REC_TYPE_ASSERT]
        == pRcdb->nocatJournalDescriptor.lastRecordId[NV2080_NOCAT_JOURNAL_REC_TYPE_ANY])
        && (0 == portMemCmp(&pRcdb->nocatJournalDescriptor.lastAssertData,
            diagBuffer.callStack,                                       // same stack
            sizeof(diagBuffer.callStack))))
    {
        // it is the same as the last assert we logged. so don't log it again.
        // but see if we can increment the counter in an unreported assert.
        // check if the last record is also an assert
        if (portAtomicIncrementS32(&concurrentRingBufferAccess) == 1)
        {
            // get the last record from the buffer
            _rcdbGetNewestNocatJournalRecordForType(pRcdb,
                NV2080_NOCAT_JOURNAL_REC_TYPE_ANY,
                NULL, &pNocatEntry);
            if (pNocatEntry != NULL)
            {
                // is it an assert?
                if (pNocatEntry->nocatJournalEntry.recType == (NV2080_NOCAT_JOURNAL_REC_TYPE_ASSERT))
                {
                    // increment the count
                    pDiagData = (RM_NOCAT_ASSERT_DIAG_BUFFER*)&pNocatEntry->nocatJournalEntry.diagBuffer;
                    pDiagData->count++;
                    recordPosted = NV_TRUE;
                }
                _rcdbReleaseNocatJournalRecord(pNocatEntry);
            }
        }
        portAtomicDecrementS32(&concurrentRingBufferAccess);
    }
    if (!recordPosted)
    {
        // we are logging this assert, save off the stack so we can use it to
        // compare against future asserts.
        portMemCopy(&pRcdb->nocatJournalDescriptor.lastAssertData,
            sizeof(pRcdb->nocatJournalDescriptor.lastAssertData),
            &diagBuffer, sizeof(diagBuffer));
        rcdbNocatInsertNocatError(pGpu, &newEntry);
    }

    return NV_OK;
}

/*
** rcdbNocatInsertRMCDErrorEvent creates an event from an RMCD error block
**
**  parameters:
**  pGpu        pointer to GPU object associated with the error
**  recType     the type of event to create
**  pSource     a pointer to the source string
**  subsystem   the subsystem associated with the event.
**  errorCode   error code for the event
**  pFault      pointer to a faulting component identifier associated with the error
*/
NvU32 rcdbNocatInsertRMCDErrorEvent(OBJGPU *pGpu, NvU32 recType,
    const char *pSource, NvU32 subsystem, NvU64 errorCode, const char *pFault,
    RMCD_ERROR_BLOCK *pRcdError)
{
    NOCAT_JOURNAL_PARAMS    newEntry;

    portMemSet(&newEntry, 0, sizeof(newEntry));
    newEntry.recType = recType;
    newEntry.pSource = pSource;
    newEntry.subsystem = subsystem;
    newEntry.errorCode = errorCode;
    newEntry.pFaultingEngine = pFault;
    if (pRcdError != NULL)
    {
        newEntry.pDiagBuffer = (NvU8 * )pRcdError->pBlock;
        newEntry.diagBufferLen = pRcdError->blockSize;
    }
    return rcdbNocatInsertNocatError(pGpu, &newEntry);
}

/*
** rcdbSetNocatTdrReason sets the TDR reason code in the most recent TDR record if there is one,
**  otherwise, it creates one with the reason code.
**
**  parameters:
**      pReasonData     the data supplied with including the reason code.
**                      if a TDR record exists, the reason will be added to the existing record,
**                      otherwise the rest of the data will be used to create a new TDR event.
*/
NV_STATUS rcdbSetNocatTdrReason
(
    NV2080CtrlNocatJournalDataTdrReason *pReasonData
)
{
    OBJSYS             *pSys = SYS_GET_INSTANCE();
    Journal            *pRcdb = SYS_GET_RCDB(pSys);
    RM_NOCAT_JOURNAL_ENTRY* pNocatEntry = NULL;

    // validate inputs.
    if ((pRcdb == NULL) || (pReasonData == NULL))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_UPDATE_REQ_IDX]++;

    if (portAtomicIncrementS32(&concurrentRingBufferAccess) == 1)
    {
        // see if there is a TDR record.
        _rcdbGetNewestNocatJournalRecordForType(pRcdb,
            NV2080_NOCAT_JOURNAL_REC_TYPE_TDR,
            NULL, &pNocatEntry);
        if (pNocatEntry != NULL)
        {
            // there is, set the reason.
            _rcdbSetTdrReason(pRcdb, pReasonData->reasonCode,
                (char *)pNocatEntry->nocatJournalEntry.tdrReason,
                sizeof(pNocatEntry->nocatJournalEntry.tdrReason));
            pRcdb->nocatJournalDescriptor.nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_UPDATED_IDX]++;
            _rcdbReleaseNocatJournalRecord(pNocatEntry);
        }
    }
    portAtomicDecrementS32(&concurrentRingBufferAccess);

    // if we did not get a TDR record, create one.
    // we need to do it after the ring buffers are released.
    if (pNocatEntry == NULL)
    {
        NOCAT_JOURNAL_PARAMS newEntry;

        portMemSet(&newEntry, 0, sizeof(newEntry));
        newEntry.recType = NV2080_NOCAT_JOURNAL_REC_TYPE_TDR;
        newEntry.pSource = (char *)pReasonData->source;
        newEntry.subsystem = pReasonData->subsystem;
        newEntry.errorCode = pReasonData->errorCode;
        newEntry.tdrReason = pReasonData->reasonCode;
        return rcdbNocatInsertNocatError(NULL, &newEntry);
    }
    return NV_OK;
}
