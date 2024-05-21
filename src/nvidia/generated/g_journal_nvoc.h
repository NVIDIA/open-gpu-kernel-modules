
#ifndef _G_JOURNAL_NVOC_H_
#define _G_JOURNAL_NVOC_H_
#include "nvoc/runtime.h"

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 0

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#pragma once
#include "g_journal_nvoc.h"

#ifndef _JOURNAL_H_
#define _JOURNAL_H_

//
// Journal object defines and Structures
//

#include "kernel/core/core.h"

#include "kernel/core/system.h"
#include "kernel/diagnostics/journal_structs.h"
#include "kernel/diagnostics/nv_debug_dump.h"
#include "kernel/diagnostics/traceable.h"

#include "ctrl/ctrl2080/ctrl2080nvd.h"

#include "lib/protobuf/prb.h"
#include "nvdump.h"

// Local definition to set the default MMU Fault Type.
#define NV_PFAULT_FAULT_TYPE_UNKNOWN                0x000000FF;
// Local definition to set the default MMU Error Source/Client ID.
#define NV_PFIFO_INTR_MMU_FAULT_INFO_CLIENT_UNKNOWN 0x000000FF;

#define NOCAT_COLLECT_PERF 0
#define NOCAT_PROBE_FB_MEMORY 0



typedef struct _def_assert_list
{
    NvU32                   QualifyingStackSize;    // The number of stack entries to check in order
                                                    // to consider a record unique
    NvU32                   Size;                   // size of list of pointers to records
    NvU32                   Count;                  // number of records in the record list
    PRmRCCommonAssert_RECORD *ppList;               // list of pointers to records.
} JOURNAL_ASSERT_LIST;

typedef struct _def_event_journal
{
    NvU8*                   pBuffer;                // pointer to buffer
    NvU8*                   pFree;                  // pointer to first free byte in the buffer
    NVCDRecordCollection*   pCurrCollection;        // current collection being created
    NvU32                   BufferSize;             // size of buffer in bytes
    NvU32                   BufferRemaining;        // remaining bytes in buffer;
    NvU32                   RecordCount;            // total # of records currently in the buffer
    JOURNAL_ASSERT_LIST     AssertList;             // list of asserts within the journal.
} EVENT_JOURNAL;

typedef struct _def_sys_error_info
{
    volatile NvU32  InUse;                            // Atomically set when a thread is accessing the structure
    NvU32           LogCount;                         // Count of Logged Event Messages
    NvU32           ErrorCount;                       // Count of Logged Event Messages
    NvU32           TotalErrorCount;                  // Total Number of Errors Encountered
    void *          pErrorList;                       // Error List
    void *          pNextError;                       // Used to walk error list
} SYS_ERROR_INFO;

// Max errors that should be logged into a SYS_ERROR_INFO.
#define MAX_ERROR_LOG_COUNT (0x10)

// Max error that should be added to SYS_ERROR_INFO.pErrorList
#define MAX_ERROR_LIST_COUNT (0x10)

typedef struct RING_BUFFER_LOG RING_BUFFER_LOG, *PRING_BUFFER_LOG;

// A node in the linked list of ring buffers
struct RING_BUFFER_LOG
{
    RMCD_RECORD_TYPE entryType;  // Type of the entries stored in this ring buffer.
    NvU32            maxEntries; // Capacity of the ring buffer
    NvU32            maxBufferSize;
    NvU32            headIndex;  // Index of the first item in the ring buffer.
    NvU32            refCount;   // Now many GPUs are using this Ring Buffer
    //
    // NOTE: If you want to look at the most recent item added to the ring buffer,
    // you need to look at the entry at (headIndex+numEntries-1)%maxEntries
    //
    NvU32            numEntries;
    NvU32            bufferSize;
    NvU8             *pBuffer;   // Buffer holding the entries
    RING_BUFFER_LOG  *pNextRingBuffer;

};

//
// A list of circular buffers. Each circular buffer is capable of
// storing the last n events of a particular type.
//
typedef struct
{
    RING_BUFFER_LOG* pFirstEntry;  // Pointer to the first circular buffer in the list
    NvU32        NumRingBuffers;

} RING_BUFFER_LOG_COLLECTION;

typedef struct
{
    RmRCCommonJournal_RECORD common;
    const PRB_FIELD_DESC *fieldDesc;
} RM_DATA_COLLECTION_RECORD;

// this is the structure to hold a NOCAT report.
typedef struct
{
    NvU32                           id;                 // record id
    volatile NvS32                  inUse;              // indicates the record is actively being used.
    NV2080_NOCAT_JOURNAL_GPU_STATE  nocatGpuState;      // contains the state of the
                                                        // associated GPU if there is one,
    NV2080_NOCAT_JOURNAL_ENTRY      nocatJournalEntry;  // the NOCAT report data -- IDs, diag data etc.
} RM_NOCAT_JOURNAL_ENTRY;

typedef struct
{
    NvU64       timestamp           NV_ALIGN_BYTES(8);
    NvU8        recType;
    NvU32       bugcheck;
    const char *pSource;
    NvU32       subsystem;
    NvU64       errorCode           NV_ALIGN_BYTES(8);
    NvU32       diagBufferLen;
    const NvU8 *pDiagBuffer;
    const char *pFaultingEngine;
    NvU32       tdrReason;
} NOCAT_JOURNAL_PARAMS;

#define ASSERT_CALL_STACK_SIZE 10
#define NOCAT_CACHE_FRESHNESS_PERIOD_MS 10ULL
typedef struct
{
    NvU32       callStack[ASSERT_CALL_STACK_SIZE];          // Call stack when the assert occurred.
    NvU32       count;
} RM_NOCAT_ASSERT_DIAG_BUFFER;

typedef struct _nocatQueueDescriptor
{
    NvU64   loadAddress;
    NvU32   nextRecordId;
    NvU32   nextReportedId;
    NvU32   nocatLastRecordType;
    NvU64   lockTimestamp;
    NvU32   lastRecordId[NV2080_NOCAT_JOURNAL_REC_TYPE_COUNT];
    RM_NOCAT_ASSERT_DIAG_BUFFER   lastAssertData;
    NvU8    tag[NV2080_NOCAT_JOURNAL_MAX_STR_LEN];
    NvU64   cacheFreshnessPeriodticks;
    NV2080_NOCAT_JOURNAL_GPU_STATE  nocatGpuState;      // cache contains the state of the
                                                        // associated GPU if there is one.

    NvU32   nocatEventCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_COUNTER_COUNT];
} nocatQueueDescriptor;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_JOURNAL_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct OBJRCDB {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct Object __nvoc_base_Object;
    struct OBJTRACEABLE __nvoc_base_OBJTRACEABLE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct OBJTRACEABLE *__nvoc_pbase_OBJTRACEABLE;    // traceable super
    struct OBJRCDB *__nvoc_pbase_OBJRCDB;    // rcdb

    // 2 PDB properties
    NvBool PDB_PROP_RCDB_COMPRESS;
    NvBool PDB_PROP_RCDB_IN_DEFERRED_DUMP_CODEPATH;

    // Data members
    SYS_ERROR_INFO ErrorInfo;
    EVENT_JOURNAL Journal;
    NvU32 BugcheckCount;
    RING_BUFFER_LOG_COLLECTION RingBufferColl;
    NVD_STATE nvDumpState;
    rcErrorCounterEntry rcErrorCounterArray[10];
    NvBool bPrevDriverCodeExecuted;
    char *previousDriverVersion;
    char *previousDriverBranch;
    NvU32 prevDriverChangelist;
    NvU32 driverLoadCount;
    NvU16 RcErrRptNextIdx;
    NvBool RcErrRptRecordsDropped;
    struct Falcon *pCrashedFlcn;
    nocatQueueDescriptor nocatJournalDescriptor;
    NvU64 systemTimeReference;
    NvU64 timeStampFreq;
};

#ifndef __NVOC_CLASS_OBJRCDB_TYPEDEF__
#define __NVOC_CLASS_OBJRCDB_TYPEDEF__
typedef struct OBJRCDB OBJRCDB;
#endif /* __NVOC_CLASS_OBJRCDB_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJRCDB
#define __nvoc_class_id_OBJRCDB 0x15dec8
#endif /* __nvoc_class_id_OBJRCDB */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJRCDB;

#define __staticCast_OBJRCDB(pThis) \
    ((pThis)->__nvoc_pbase_OBJRCDB)

#ifdef __nvoc_journal_h_disabled
#define __dynamicCast_OBJRCDB(pThis) ((OBJRCDB*)NULL)
#else //__nvoc_journal_h_disabled
#define __dynamicCast_OBJRCDB(pThis) \
    ((OBJRCDB*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJRCDB)))
#endif //__nvoc_journal_h_disabled

// Property macros
#define PDB_PROP_RCDB_IN_DEFERRED_DUMP_CODEPATH_BASE_CAST
#define PDB_PROP_RCDB_IN_DEFERRED_DUMP_CODEPATH_BASE_NAME PDB_PROP_RCDB_IN_DEFERRED_DUMP_CODEPATH
#define PDB_PROP_RCDB_COMPRESS_BASE_CAST
#define PDB_PROP_RCDB_COMPRESS_BASE_NAME PDB_PROP_RCDB_COMPRESS

NV_STATUS __nvoc_objCreateDynamic_OBJRCDB(OBJRCDB**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJRCDB(OBJRCDB**, Dynamic*, NvU32);
#define __objCreate_OBJRCDB(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJRCDB((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
NV_STATUS rcdbConstruct_IMPL(struct OBJRCDB *arg_pRcdb);

#define __nvoc_rcdbConstruct(arg_pRcdb) rcdbConstruct_IMPL(arg_pRcdb)
void rcdbDestruct_IMPL(struct OBJRCDB *pRcdb);

#define __nvoc_rcdbDestruct(pRcdb) rcdbDestruct_IMPL(pRcdb)
NV_STATUS rcdbSavePreviousDriverVersion_IMPL(struct OBJGPU *pGpu, struct OBJRCDB *pRcdb);

#ifdef __nvoc_journal_h_disabled
static inline NV_STATUS rcdbSavePreviousDriverVersion(struct OBJGPU *pGpu, struct OBJRCDB *pRcdb) {
    NV_ASSERT_FAILED_PRECOMP("OBJRCDB was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_journal_h_disabled
#define rcdbSavePreviousDriverVersion(pGpu, pRcdb) rcdbSavePreviousDriverVersion_IMPL(pGpu, pRcdb)
#endif //__nvoc_journal_h_disabled

NV_STATUS rcdbClearErrorHistory_IMPL(struct OBJRCDB *pRcdb);

#ifdef __nvoc_journal_h_disabled
static inline NV_STATUS rcdbClearErrorHistory(struct OBJRCDB *pRcdb) {
    NV_ASSERT_FAILED_PRECOMP("OBJRCDB was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_journal_h_disabled
#define rcdbClearErrorHistory(pRcdb) rcdbClearErrorHistory_IMPL(pRcdb)
#endif //__nvoc_journal_h_disabled

NV_STATUS rcdbDeleteErrorElement_IMPL(struct OBJRCDB *pRcdb, void *arg2);

#ifdef __nvoc_journal_h_disabled
static inline NV_STATUS rcdbDeleteErrorElement(struct OBJRCDB *pRcdb, void *arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJRCDB was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_journal_h_disabled
#define rcdbDeleteErrorElement(pRcdb, arg2) rcdbDeleteErrorElement_IMPL(pRcdb, arg2)
#endif //__nvoc_journal_h_disabled

void rcdbDestroyRingBufferCollection_IMPL(struct OBJRCDB *pRcdb);

#ifdef __nvoc_journal_h_disabled
static inline void rcdbDestroyRingBufferCollection(struct OBJRCDB *pRcdb) {
    NV_ASSERT_FAILED_PRECOMP("OBJRCDB was disabled!");
}
#else //__nvoc_journal_h_disabled
#define rcdbDestroyRingBufferCollection(pRcdb) rcdbDestroyRingBufferCollection_IMPL(pRcdb)
#endif //__nvoc_journal_h_disabled

NV_STATUS rcdbAllocNextJournalRec_IMPL(struct OBJRCDB *pRcdb, NVCD_RECORD **arg2, NvU8 arg3, NvU8 arg4, NvU16 arg5);

#ifdef __nvoc_journal_h_disabled
static inline NV_STATUS rcdbAllocNextJournalRec(struct OBJRCDB *pRcdb, NVCD_RECORD **arg2, NvU8 arg3, NvU8 arg4, NvU16 arg5) {
    NV_ASSERT_FAILED_PRECOMP("OBJRCDB was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_journal_h_disabled
#define rcdbAllocNextJournalRec(pRcdb, arg2, arg3, arg4, arg5) rcdbAllocNextJournalRec_IMPL(pRcdb, arg2, arg3, arg4, arg5)
#endif //__nvoc_journal_h_disabled

NV_STATUS rcdbAddBugCheckRec_IMPL(struct OBJGPU *pGpu, struct OBJRCDB *pRcdb, NvU32 bugCheckCode);

#ifdef __nvoc_journal_h_disabled
static inline NV_STATUS rcdbAddBugCheckRec(struct OBJGPU *pGpu, struct OBJRCDB *pRcdb, NvU32 bugCheckCode) {
    NV_ASSERT_FAILED_PRECOMP("OBJRCDB was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_journal_h_disabled
#define rcdbAddBugCheckRec(pGpu, pRcdb, bugCheckCode) rcdbAddBugCheckRec_IMPL(pGpu, pRcdb, bugCheckCode)
#endif //__nvoc_journal_h_disabled

NV_STATUS rcdbAddPowerStateRec_IMPL(struct OBJGPU *pGpu, struct OBJRCDB *pRcdb, NvU32 powerEvent, NvU32 state, NvU32 fastBootPowerState);

#ifdef __nvoc_journal_h_disabled
static inline NV_STATUS rcdbAddPowerStateRec(struct OBJGPU *pGpu, struct OBJRCDB *pRcdb, NvU32 powerEvent, NvU32 state, NvU32 fastBootPowerState) {
    NV_ASSERT_FAILED_PRECOMP("OBJRCDB was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_journal_h_disabled
#define rcdbAddPowerStateRec(pGpu, pRcdb, powerEvent, state, fastBootPowerState) rcdbAddPowerStateRec_IMPL(pGpu, pRcdb, powerEvent, state, fastBootPowerState)
#endif //__nvoc_journal_h_disabled

NV_STATUS rcdbDumpInitGpuAccessibleFlag_IMPL(struct OBJGPU *pGpu, struct OBJRCDB *pRcdb);

#ifdef __nvoc_journal_h_disabled
static inline NV_STATUS rcdbDumpInitGpuAccessibleFlag(struct OBJGPU *pGpu, struct OBJRCDB *pRcdb) {
    NV_ASSERT_FAILED_PRECOMP("OBJRCDB was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_journal_h_disabled
#define rcdbDumpInitGpuAccessibleFlag(pGpu, pRcdb) rcdbDumpInitGpuAccessibleFlag_IMPL(pGpu, pRcdb)
#endif //__nvoc_journal_h_disabled

NvU8 *rcdbCreateRingBuffer_IMPL(struct OBJRCDB *pRcdb, RMCD_RECORD_TYPE type, NvU32 maxEntries);

#ifdef __nvoc_journal_h_disabled
static inline NvU8 *rcdbCreateRingBuffer(struct OBJRCDB *pRcdb, RMCD_RECORD_TYPE type, NvU32 maxEntries) {
    NV_ASSERT_FAILED_PRECOMP("OBJRCDB was disabled!");
    return NULL;
}
#else //__nvoc_journal_h_disabled
#define rcdbCreateRingBuffer(pRcdb, type, maxEntries) rcdbCreateRingBuffer_IMPL(pRcdb, type, maxEntries)
#endif //__nvoc_journal_h_disabled

void rcdbDestroyRingBuffer_IMPL(struct OBJRCDB *pRcdb, RMCD_RECORD_TYPE type);

#ifdef __nvoc_journal_h_disabled
static inline void rcdbDestroyRingBuffer(struct OBJRCDB *pRcdb, RMCD_RECORD_TYPE type) {
    NV_ASSERT_FAILED_PRECOMP("OBJRCDB was disabled!");
}
#else //__nvoc_journal_h_disabled
#define rcdbDestroyRingBuffer(pRcdb, type) rcdbDestroyRingBuffer_IMPL(pRcdb, type)
#endif //__nvoc_journal_h_disabled

RmRCCommonJournal_RECORD *rcdbAddRecToRingBuffer_IMPL(struct OBJGPU *pGpu, struct OBJRCDB *pRcdb, RMCD_RECORD_TYPE type, NvU32 recordSize, NvU8 *pRecord);

#ifdef __nvoc_journal_h_disabled
static inline RmRCCommonJournal_RECORD *rcdbAddRecToRingBuffer(struct OBJGPU *pGpu, struct OBJRCDB *pRcdb, RMCD_RECORD_TYPE type, NvU32 recordSize, NvU8 *pRecord) {
    NV_ASSERT_FAILED_PRECOMP("OBJRCDB was disabled!");
    return NULL;
}
#else //__nvoc_journal_h_disabled
#define rcdbAddRecToRingBuffer(pGpu, pRcdb, type, recordSize, pRecord) rcdbAddRecToRingBuffer_IMPL(pGpu, pRcdb, type, recordSize, pRecord)
#endif //__nvoc_journal_h_disabled

NvU32 rcdbGetOcaRecordSizeWithHeader_IMPL(struct OBJRCDB *pRcdb, RMCD_RECORD_TYPE type);

#ifdef __nvoc_journal_h_disabled
static inline NvU32 rcdbGetOcaRecordSizeWithHeader(struct OBJRCDB *pRcdb, RMCD_RECORD_TYPE type) {
    NV_ASSERT_FAILED_PRECOMP("OBJRCDB was disabled!");
    return 0;
}
#else //__nvoc_journal_h_disabled
#define rcdbGetOcaRecordSizeWithHeader(pRcdb, type) rcdbGetOcaRecordSizeWithHeader_IMPL(pRcdb, type)
#endif //__nvoc_journal_h_disabled

NvU32 rcdbDumpJournal_IMPL(struct OBJRCDB *pRcdb, struct OBJGPU *pGpu, PRB_ENCODER *pPrbEnc, NVD_STATE *pNvDumpState, const PRB_FIELD_DESC *pFieldDesc);

#ifdef __nvoc_journal_h_disabled
static inline NvU32 rcdbDumpJournal(struct OBJRCDB *pRcdb, struct OBJGPU *pGpu, PRB_ENCODER *pPrbEnc, NVD_STATE *pNvDumpState, const PRB_FIELD_DESC *pFieldDesc) {
    NV_ASSERT_FAILED_PRECOMP("OBJRCDB was disabled!");
    return 0;
}
#else //__nvoc_journal_h_disabled
#define rcdbDumpJournal(pRcdb, pGpu, pPrbEnc, pNvDumpState, pFieldDesc) rcdbDumpJournal_IMPL(pRcdb, pGpu, pPrbEnc, pNvDumpState, pFieldDesc)
#endif //__nvoc_journal_h_disabled

NV_STATUS rcdbDumpComponent_IMPL(struct OBJRCDB *pRcdb, NvU32 component, NVDUMP_BUFFER *pBuffer, NVDUMP_BUFFER_POLICY policy, PrbBufferCallback *pBufferCallback);

#ifdef __nvoc_journal_h_disabled
static inline NV_STATUS rcdbDumpComponent(struct OBJRCDB *pRcdb, NvU32 component, NVDUMP_BUFFER *pBuffer, NVDUMP_BUFFER_POLICY policy, PrbBufferCallback *pBufferCallback) {
    NV_ASSERT_FAILED_PRECOMP("OBJRCDB was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_journal_h_disabled
#define rcdbDumpComponent(pRcdb, component, pBuffer, policy, pBufferCallback) rcdbDumpComponent_IMPL(pRcdb, component, pBuffer, policy, pBufferCallback)
#endif //__nvoc_journal_h_disabled

NV_STATUS rcdbDumpSystemInfo_IMPL(struct OBJRCDB *pRcdb, PRB_ENCODER *pPrbEnc, NVD_STATE *pNvDumpState);

#ifdef __nvoc_journal_h_disabled
static inline NV_STATUS rcdbDumpSystemInfo(struct OBJRCDB *pRcdb, PRB_ENCODER *pPrbEnc, NVD_STATE *pNvDumpState) {
    NV_ASSERT_FAILED_PRECOMP("OBJRCDB was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_journal_h_disabled
#define rcdbDumpSystemInfo(pRcdb, pPrbEnc, pNvDumpState) rcdbDumpSystemInfo_IMPL(pRcdb, pPrbEnc, pNvDumpState)
#endif //__nvoc_journal_h_disabled

NV_STATUS rcdbDumpSystemFunc_IMPL(struct OBJRCDB *pRcdb, PRB_ENCODER *pPrbEnc, NVD_STATE *pNvDumpState);

#ifdef __nvoc_journal_h_disabled
static inline NV_STATUS rcdbDumpSystemFunc(struct OBJRCDB *pRcdb, PRB_ENCODER *pPrbEnc, NVD_STATE *pNvDumpState) {
    NV_ASSERT_FAILED_PRECOMP("OBJRCDB was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_journal_h_disabled
#define rcdbDumpSystemFunc(pRcdb, pPrbEnc, pNvDumpState) rcdbDumpSystemFunc_IMPL(pRcdb, pPrbEnc, pNvDumpState)
#endif //__nvoc_journal_h_disabled

NvU32 rcdbDumpErrorCounters_IMPL(struct OBJRCDB *pRcDB, struct OBJGPU *pGpu, PRB_ENCODER *pPrbEnc);

#ifdef __nvoc_journal_h_disabled
static inline NvU32 rcdbDumpErrorCounters(struct OBJRCDB *pRcDB, struct OBJGPU *pGpu, PRB_ENCODER *pPrbEnc) {
    NV_ASSERT_FAILED_PRECOMP("OBJRCDB was disabled!");
    return 0;
}
#else //__nvoc_journal_h_disabled
#define rcdbDumpErrorCounters(pRcDB, pGpu, pPrbEnc) rcdbDumpErrorCounters_IMPL(pRcDB, pGpu, pPrbEnc)
#endif //__nvoc_journal_h_disabled

NV_STATUS rcdbGetRcDiagRecBoundaries_IMPL(struct OBJRCDB *pRcdb, NvU16 *arg2, NvU16 *arg3, NvU32 arg4, NvU32 arg5);

#ifdef __nvoc_journal_h_disabled
static inline NV_STATUS rcdbGetRcDiagRecBoundaries(struct OBJRCDB *pRcdb, NvU16 *arg2, NvU16 *arg3, NvU32 arg4, NvU32 arg5) {
    NV_ASSERT_FAILED_PRECOMP("OBJRCDB was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_journal_h_disabled
#define rcdbGetRcDiagRecBoundaries(pRcdb, arg2, arg3, arg4, arg5) rcdbGetRcDiagRecBoundaries_IMPL(pRcdb, arg2, arg3, arg4, arg5)
#endif //__nvoc_journal_h_disabled

RmRCCommonJournal_RECORD *rcdbAddRcDiagRec_IMPL(struct OBJGPU *pGpu, struct OBJRCDB *pRcdb, RmRcDiag_RECORD *arg3);

#ifdef __nvoc_journal_h_disabled
static inline RmRCCommonJournal_RECORD *rcdbAddRcDiagRec(struct OBJGPU *pGpu, struct OBJRCDB *pRcdb, RmRcDiag_RECORD *arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJRCDB was disabled!");
    return NULL;
}
#else //__nvoc_journal_h_disabled
#define rcdbAddRcDiagRec(pGpu, pRcdb, arg3) rcdbAddRcDiagRec_IMPL(pGpu, pRcdb, arg3)
#endif //__nvoc_journal_h_disabled

RmRCCommonJournal_RECORD *rcdbAddRcDiagRecFromGsp_IMPL(struct OBJGPU *pGpu, struct OBJRCDB *pRcdb, RmRCCommonJournal_RECORD *arg3, RmRcDiag_RECORD *arg4);

#ifdef __nvoc_journal_h_disabled
static inline RmRCCommonJournal_RECORD *rcdbAddRcDiagRecFromGsp(struct OBJGPU *pGpu, struct OBJRCDB *pRcdb, RmRCCommonJournal_RECORD *arg3, RmRcDiag_RECORD *arg4) {
    NV_ASSERT_FAILED_PRECOMP("OBJRCDB was disabled!");
    return NULL;
}
#else //__nvoc_journal_h_disabled
#define rcdbAddRcDiagRecFromGsp(pGpu, pRcdb, arg3, arg4) rcdbAddRcDiagRecFromGsp_IMPL(pGpu, pRcdb, arg3, arg4)
#endif //__nvoc_journal_h_disabled

NV_STATUS rcdbGetRcDiagRec_IMPL(struct OBJRCDB *pRcdb, NvU16 arg2, RmRCCommonJournal_RECORD **arg3, NvU32 arg4, NvU32 arg5);

#ifdef __nvoc_journal_h_disabled
static inline NV_STATUS rcdbGetRcDiagRec(struct OBJRCDB *pRcdb, NvU16 arg2, RmRCCommonJournal_RECORD **arg3, NvU32 arg4, NvU32 arg5) {
    NV_ASSERT_FAILED_PRECOMP("OBJRCDB was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_journal_h_disabled
#define rcdbGetRcDiagRec(pRcdb, arg2, arg3, arg4, arg5) rcdbGetRcDiagRec_IMPL(pRcdb, arg2, arg3, arg4, arg5)
#endif //__nvoc_journal_h_disabled

NV_STATUS rcdbUpdateRcDiagRecContext_IMPL(struct OBJRCDB *pRcdb, NvU16 arg2, NvU16 arg3, NvU32 arg4, NvU32 arg5);

#ifdef __nvoc_journal_h_disabled
static inline NV_STATUS rcdbUpdateRcDiagRecContext(struct OBJRCDB *pRcdb, NvU16 arg2, NvU16 arg3, NvU32 arg4, NvU32 arg5) {
    NV_ASSERT_FAILED_PRECOMP("OBJRCDB was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_journal_h_disabled
#define rcdbUpdateRcDiagRecContext(pRcdb, arg2, arg3, arg4, arg5) rcdbUpdateRcDiagRecContext_IMPL(pRcdb, arg2, arg3, arg4, arg5)
#endif //__nvoc_journal_h_disabled

void rcdbInitNocatGpuCache_IMPL(struct OBJGPU *pGpu);

#define rcdbInitNocatGpuCache(pGpu) rcdbInitNocatGpuCache_IMPL(pGpu)
void rcdbCleanupNocatGpuCache_IMPL(struct OBJGPU *pGpu);

#define rcdbCleanupNocatGpuCache(pGpu) rcdbCleanupNocatGpuCache_IMPL(pGpu)
#undef PRIVATE_FIELD



NV_STATUS rcdbAddRmDclMsg(void* msg, NvU16 size, const PRB_FIELD_DESC *fieldDesc);
NV_STATUS rcdbAddRmEngDump(struct OBJGPU *pGpu, NvU32 component);
NV_STATUS rcdbAddRmGpuDump(struct OBJGPU *pGpu);
void rcdbSetCommonJournalRecord(struct OBJGPU *pGpu, PRmRCCommonJournal_RECORD pRec);
void rcdbAddCrashedFalcon(struct Falcon *pFlcn);

NV_STATUS rcdbAddAssertJournalRec(void* pGpu, void** ppRec, NvU8 jGroup,
    NvU8 type, NvU16 size, NvU32 level, NvU64 key);

NV_STATUS rcdbAddAssertJournalRecWithLine(void *pVoidGpu, NvU32 lineNum,
    void** ppRec, NvU8 jGroup, NvU8 type, NvU16 size, NvU32 level, NvU64 key);

/*! insert a record into the NOCAT Journal */
NvU32 rcdbNocatInsertNocatError(struct OBJGPU *pGpu,
    NOCAT_JOURNAL_PARAMS *nocatJournalEntry);

NvU32 rcdbNocatInsertBugcheck(NvU32 deviceInstance, NvU32 bugCheckCode);

NV_STATUS rcdbNocatInitEngineErrorEvent(NOCAT_JOURNAL_PARAMS *nocatJournalEntry);
NvU32 rcdbNocatInsertEngineError(struct OBJGPU *pGpu,
    const char *pSource, NvU32 subsystem, NvU64 errorCode,
    NvU8 *pDiagBuffer, NvU32 diagBufferLen);

NvU32 rcdbNocatInsertRMCDErrorEvent(struct OBJGPU *pGpu, NvU32 recType,
    const char *pSource, NvU32 subsystem, NvU64 errorCode, const char *pFault,
    RMCD_ERROR_BLOCK *pRcdError);

NV_STATUS rcdbNocatInitTDRErrorEvent(NOCAT_JOURNAL_PARAMS *nocatJournalEntry);
NvU32 rcdbNocatInsertTDRError(struct OBJGPU *pGpu,
    const char *pSource, NvU32 subsystem, NvU64 errorCode,
    NvU32 tdrBucket,
    NvU8 *pDiagBuffer, NvU32 diagBufferLen,
    NvU32 tdrReason, const char *pFaultingApp);

NV_STATUS rcdbReportNextNocatJournalEntry(NV2080_NOCAT_JOURNAL_RECORD* pReport);

NV_STATUS rcdbSetNocatTdrReason(NV2080CtrlNocatJournalDataTdrReason *pReasonData);

NvU32 rcdbGetNocatOutstandingCount(Journal *pRcdb);

NV_STATUS rcdbNocatJournalReportTestFill(void);

#endif // _JOURNAL_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_JOURNAL_NVOC_H_
