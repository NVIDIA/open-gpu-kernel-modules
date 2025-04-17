/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrl2080/ctrl2080nvd.finn
//

#include "ctrl/ctrl2080/ctrl2080base.h"

#include "ctrl/ctrlxxxx.h"
/*
 * NV2080_CTRL_CMD_NVD_GET_DUMP_SIZE
 *
 * This command gets the expected dump size of a particular GPU dump component.
 * Note that events that occur between this command and a later
 * NV2080_CTRL_CMD_NVD_GET_DUMP command could alter the size of
 * the buffer required.
 *
 *   component
 *     One of NVDUMP_COMPONENT < 0x400 defined in nvdump.h to estimate
 *     the size of.
 *   size
 *     This parameter returns the expected size.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT if component is invalid.
 *
 */

#define NV2080_CTRL_CMD_NVD_GET_DUMP_SIZE (0x20802401) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVD_INTERFACE_ID << 8) | NV2080_CTRL_NVD_GET_DUMP_SIZE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVD_GET_DUMP_SIZE_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV2080_CTRL_NVD_GET_DUMP_SIZE_PARAMS {
    NvU32 component;
    NvU32 size;
} NV2080_CTRL_NVD_GET_DUMP_SIZE_PARAMS;

/*
 * NV2080_CTRL_CMD_NVD_GET_DUMP
 *
 * This command gets a dump of a particular GPU dump component. If triggers
 * is non-zero, the command waits for the trigger to occur
 * before it returns.
 *
 *   pBuffer
 *     This parameter points to the buffer for the data.
 *  component
 *     One of NVDUMP_COMPONENT < 0x400 defined in nvdump.h to select
 *     for dumping.
 *  size
 *     On entry, this parameter specifies the maximum length for
 *     the returned data. On exit, it specifies the number of bytes
 *     returned.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NVOS_ERROR_INVALID_ARGUMENT if component is invalid.
 *   NVOS_ERROR_INVALID_ADDRESS if pBuffer is invalid
 *   NVOS_ERROR_INVALID_???? if the buffer was too small
 *
 *
 */
#define NV2080_CTRL_CMD_NVD_GET_DUMP (0x20802402) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVD_INTERFACE_ID << 8) | NV2080_CTRL_NVD_GET_DUMP_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NVD_GET_DUMP_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV2080_CTRL_NVD_GET_DUMP_PARAMS {
    NV_DECLARE_ALIGNED(NvP64 pBuffer, 8);
    NvU32 component;
    NvU32 size;
} NV2080_CTRL_NVD_GET_DUMP_PARAMS;

/*
 * NV2080_CTRL_CMD_NVD_GET_NOCAT_JOURNAL
 *
 * This command returns the contents of the Journal used by  NOCAT, and
 * optionally clears the data
 *
 *   clear:
 *     [IN] indicates if should the data be cleared after reporting
 *
 *   JournalRecords :
 *     [OUT] an array of Journal records reported.
 *
 *   outstandingAssertCount:
 *     [OUT] number of asserts that remain to be reported on.
 *
 *   reportedAssertCount:
 *     [OUT] the number of asserts contained in the report
 *
 *   asserts:
 *     [OUT] an array of up to NV2080_NOCAT_JOURNAL_MAX_ASSERT_RECORDS assert reports
 */


#define NV2080_CTRL_CMD_NVD_GET_NOCAT_JOURNAL    (0x20802409) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVD_INTERFACE_ID << 8) | NV2080_CTRL_NVD_GET_NOCAT_JOURNAL_PARAMS_MESSAGE_ID" */

#define NV2080_NOCAT_JOURNAL_MAX_DIAG_BUFFER     1024
#define NV2080_NOCAT_JOURNAL_MAX_STR_LEN         65
#define NV2080_NOCAT_JOURNAL_MAX_JOURNAL_RECORDS 10
#define NV2080_NOCAT_JOURNAL_MAX_ASSERT_RECORDS  32

// structure to hold clock details.
typedef struct NV2080_NOCAT_JOURNAL_OVERCLOCK_DETAILS {
    NvS32 userMinOffset;
    NvS32 userMaxOffset;
    NvU32 factoryMinOffset;
    NvU32 factoryMaxOffset;
    NvU32 lastActiveClock;
    NvU32 lastActiveVolt;
    NvU32 lastActivePoint;
    NvU32 kappa;
} NV2080_NOCAT_JOURNAL_OVERCLOCK_DETAILS;


// structure to hold clock configuration & state.
typedef struct NV2080_NOCAT_JOURNAL_OVERCLOCK_CFG {
    NvU32                                  pstateVer;
    NV2080_NOCAT_JOURNAL_OVERCLOCK_DETAILS gpcOverclock;
    NV2080_NOCAT_JOURNAL_OVERCLOCK_DETAILS mclkOverclock;
    NvBool                                 bUserOverclocked;
    NvBool                                 bFactoryOverclocked;
} NV2080_NOCAT_JOURNAL_OVERCLOCK_CFG;

// structure to hold the GPU context at the time of the report.
typedef struct NV2080_NOCAT_JOURNAL_GPU_STATE {
    NvBool                             bValid;
    NvU32                              strap;
    NvU16                              deviceId;
    NvU16                              vendorId;
    NvU16                              subsystemVendor;
    NvU16                              subsystemId;
    NvU16                              revision;
    NvU16                              type;
    NvU32                              vbiosVersion;
    NvBool                             bOptimus;
    NvBool                             bMsHybrid;
    NvBool                             bFullPower;
    NvU32                              vbiosOemVersion;
    NvU16                              memoryType;
    NvU8                               tag[NV2080_NOCAT_JOURNAL_MAX_STR_LEN];
    NvU8                               vbiosProject[NV2080_NOCAT_JOURNAL_MAX_STR_LEN];
    NvBool                             bInFullchipReset;
    NvBool                             bInSecBusReset;
    NvBool                             bInGc6Reset;
    NV2080_NOCAT_JOURNAL_OVERCLOCK_CFG overclockCfg;
} NV2080_NOCAT_JOURNAL_GPU_STATE;

#define NV2080_NOCAT_JOURNAL_REC_TYPE_UNKNOWN  0
#define NV2080_NOCAT_JOURNAL_REC_TYPE_BUGCHECK 1
#define NV2080_NOCAT_JOURNAL_REC_TYPE_ENGINE   2
#define NV2080_NOCAT_JOURNAL_REC_TYPE_TDR      3
#define NV2080_NOCAT_JOURNAL_REC_TYPE_RC       4
#define NV2080_NOCAT_JOURNAL_REC_TYPE_ASSERT   5
#define NV2080_NOCAT_JOURNAL_REC_TYPE_ANY      6

// this should be relative to the highest type value
#define NV2080_NOCAT_JOURNAL_REC_TYPE_COUNT    (0x7) /* finn: Evaluated from "NV2080_NOCAT_JOURNAL_REC_TYPE_ANY + 1" */
typedef struct NV2080_NOCAT_JOURNAL_ENTRY {
    NvU8  recType;
    NvU32 bugcheck;
    NvU32 tdrBucketId;
    NvU8  source[NV2080_NOCAT_JOURNAL_MAX_STR_LEN];
    NvU32 subsystem;
    NV_DECLARE_ALIGNED(NvU64 errorCode, 8);
    NvU32 diagBufferLen;
    NvU8  diagBuffer[NV2080_NOCAT_JOURNAL_MAX_DIAG_BUFFER];
    NvU8  faultingEngine[NV2080_NOCAT_JOURNAL_MAX_STR_LEN];
    NvU32 mmuFaultType;
    NvU32 mmuErrorSrc;
    NvU8  tdrReason[NV2080_NOCAT_JOURNAL_MAX_STR_LEN];
} NV2080_NOCAT_JOURNAL_ENTRY;

typedef struct NV2080_NOCAT_JOURNAL_RECORD {
    NvU32                          GPUTag;
    NV_DECLARE_ALIGNED(NvU64 loadAddress, 8);
    NV_DECLARE_ALIGNED(NvU64 timeStamp, 8);
    NV_DECLARE_ALIGNED(NvU64 stateMask, 8);
    NV2080_NOCAT_JOURNAL_GPU_STATE nocatGpuState;
    NV_DECLARE_ALIGNED(NV2080_NOCAT_JOURNAL_ENTRY nocatJournalEntry, 8);
} NV2080_NOCAT_JOURNAL_RECORD;

// NOCAT activity counter indexes
// collection activity
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_COLLECT_REQ_IDX          0
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_GRANDFATHERED_RECORD_IDX 1
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_ALLOCATED_IDX            2
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_COLLECTED_IDX            3
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_NOTIFICATIONS_IDX        4
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_NOTIFICATION_FAIL_IDX    5
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_ALLOC_FAILED_IDX         6
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_COLLECT_FAILED_IDX       7
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_COLLECT_LOCKED_OUT_IDX   8
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_CTRL_INSERT_RECORDS_IDX  9
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_RPC_INSERT_RECORDS_IDX   10

// Journal Lock activity
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_JOURNAL_LOCKED_IDX       11
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_JOURNAL_LOCK_UPDATED_IDX 12
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_JOURNAL_UNLOCKED_IDX     13

// lookup activity
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_NO_RECORDS_IDX           14
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_BAD_BUFFER_IDX           15
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_MATCH_FOUND_IDX          16
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_NO_MATCH_IDX             17
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_CLOSEST_FOUND_IDX        18

// reporting activity
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_REQUESTED_IDX            19
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_REPORTED_IDX             20
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_DROPPED_IDX              21

// update activity
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_UPDATE_REQ_IDX           22
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_UPDATED_IDX              23
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_UPDATE_FAILED_IDX        24

// general errors
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_BUSY_IDX                 25
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_BAD_PARAM_IDX            26
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_BAD_TYPE_IDX             27

// reserved entries for temporary use.
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_RES4_IDX                 28
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_RES3_IDX                 29
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_RES2_IDX                 30
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_RES1_IDX                 31

// this should be relative to the highest counter index
#define NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_COUNTER_COUNT            (0x20) /* finn: Evaluated from "NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_RES1_IDX + 1" */

#define NV2080_CTRL_NOCAT_GET_COUNTERS_ONLY         0:0
#define NV2080_CTRL_NOCAT_GET_COUNTERS_ONLY_YES                       1
#define NV2080_CTRL_NOCAT_GET_COUNTERS_ONLY_NO                        0

#define NV2080_CTRL_NOCAT_GET_RESET_COUNTERS        1:1
#define NV2080_CTRL_NOCAT_GET_RESET_COUNTERS_YES                      1
#define NV2080_CTRL_NOCAT_GET_RESET_COUNTERS_NO                       0


#define NV2080_CTRL_NVD_GET_NOCAT_JOURNAL_PARAMS_MESSAGE_ID (0x9U)

typedef struct NV2080_CTRL_NVD_GET_NOCAT_JOURNAL_PARAMS {
    NvU32 flags;
    NvU32 nocatRecordCount;
    NvU32 nocatOutstandingRecordCount;
    NV_DECLARE_ALIGNED(NV2080_NOCAT_JOURNAL_RECORD journalRecords[NV2080_NOCAT_JOURNAL_MAX_JOURNAL_RECORDS], 8);
    NvU32 activityCounters[NV2080_NOCAT_JOURNAL_REPORT_ACTIVITY_COUNTER_COUNT];
    NvU8  reserved[NV2080_NOCAT_JOURNAL_MAX_STR_LEN];
} NV2080_CTRL_NVD_GET_NOCAT_JOURNAL_PARAMS;

 /*
 * NV2080_CTRL_CMD_NVD_SET_NOCAT_JOURNAL_DATA
 *
 * This command reports the TDR data collected by KMD to be added to the
 * nocat record
 *
 *   dataType
 *     [IN] specifies the type of data provided.
 *  targetRecordType
 *     [IN] specifies record type the data is intended for.
 *  nocatJournalData
 *     [IN] specifies the data to be added.
 */

#define NV2080_CTRL_CMD_NVD_SET_NOCAT_JOURNAL_DATA     (0x2080240b) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVD_INTERFACE_ID << 8) | NV2080_CTRL_NVD_SET_NOCAT_JOURNAL_DATA_PARAMS_MESSAGE_ID" */

// data types & structures
#define NV2080_CTRL_NOCAT_JOURNAL_DATA_TYPE_EMPTY      0
#define NV2080_CTRL_NOCAT_JOURNAL_DATA_TYPE_TDR_REASON 1
#define NV2080_CTRL_NOCAT_JOURNAL_DATA_TYPE_SET_TAG    2
#define NV2080_CTRL_NOCAT_JOURNAL_DATA_TYPE_RCLOG      3

#define NV2080_CTRL_NOCAT_TDR_TYPE_NONE                0
#define NV2080_CTRL_NOCAT_TDR_TYPE_LEGACY              1
#define NV2080_CTRL_NOCAT_TDR_TYPE_FULLCHIP            2
#define NV2080_CTRL_NOCAT_TDR_TYPE_BUSRESET            3
#define NV2080_CTRL_NOCAT_TDR_TYPE_GC6_RESET           4
#define NV2080_CTRL_NOCAT_TDR_TYPE_SURPRISE_REMOVAL    5
#define NV2080_CTRL_NOCAT_TDR_TYPE_UCODE_RESET         6
#define NV2080_CTRL_NOCAT_TDR_TYPE_GPU_RC_RESET        7
#define NV2080_CTRL_NOCAT_TDR_TYPE_TEST                8

typedef struct NV2080CtrlNocatJournalDataTdrReason {
    NvU32 flags;
    NvU8  source[NV2080_NOCAT_JOURNAL_MAX_STR_LEN];
    NvU32 subsystem;
    NV_DECLARE_ALIGNED(NvU64 errorCode, 8);
    NvU32 reasonCode;
} NV2080CtrlNocatJournalDataTdrReason;

#define NV2080_CTRL_NOCAT_TAG_CLEAR                 0:0
#define NV2080_CTRL_NOCAT_TAG_CLEAR_YES 1
#define NV2080_CTRL_NOCAT_TAG_CLEAR_NO  0
typedef struct NV2080CtrlNocatJournalSetTag {
    NvU32 flags;
    NvU8  tag[NV2080_NOCAT_JOURNAL_MAX_STR_LEN];
} NV2080CtrlNocatJournalSetTag;

typedef struct NV2080CtrlNocatJournalRclog {
    NvU32 flags;
    NvU32 rclogSize;        // rclog size
    NvU32 rmGpuId;        // RMGpuId associated with the adapter
    NvU32 APIType;        // API Type (dx9, dx1x, ogl, etc.)
    NvU32 contextType;        // Context type (OGL, DX, etc.)
    NvU32 exceptType;        // ROBUST_CHANNEL_* error identifier
    NvU8  processImageName[NV2080_NOCAT_JOURNAL_MAX_STR_LEN];    // process image name (without path)
} NV2080CtrlNocatJournalRclog;

#define NV2080_CTRL_NVD_SET_NOCAT_JOURNAL_DATA_PARAMS_MESSAGE_ID (0xBU)

typedef struct NV2080_CTRL_NVD_SET_NOCAT_JOURNAL_DATA_PARAMS {
    NvU32 dataType;
    NvU32 targetRecordType;
    union {
        NV_DECLARE_ALIGNED(NV2080CtrlNocatJournalDataTdrReason tdrReason, 8);
        NV2080CtrlNocatJournalSetTag tagData;
        NV2080CtrlNocatJournalRclog  rclog;
    } nocatJournalData;
} NV2080_CTRL_NVD_SET_NOCAT_JOURNAL_DATA_PARAMS;
 /*
 * NV2080_CTRL_CMD_NVD_INSERT_NOCAT_JOURNAL_RECORD
 *
 * This command Inserts a NOCAT Journal record from an outside component. 
 *
 *  nocatJournalData
 *     [IN] specifies the data to be added.
 */

#define NV2080_CTRL_CMD_NVD_INSERT_NOCAT_JOURNAL_RECORD (0x2080240c) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_NVD_INTERFACE_ID << 8) | NV2080_CTRL_CMD_NVD_INSERT_NOCAT_JOURNAL_RECORD_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_NOCAT_INSERT_ALLOW_NULL_STR         0:0
#define NV2080_CTRL_NOCAT_INSERT_ALLOW_NULL_STR_YES     1
#define NV2080_CTRL_NOCAT_INSERT_ALLOW_NULL_STR_NO      0
#define NV2080_CTRL_NOCAT_INSERT_ALLOW_0_LEN_BUFFER     1:1
#define NV2080_CTRL_NOCAT_INSERT_ALLOW_0_LEN_BUFFER_YES 1
#define NV2080_CTRL_NOCAT_INSERT_ALLOW_0_LEN_BUFFER_NO  0

typedef struct NV2080CtrlNocatJournalInsertRecord {
    NvU32 flags;
    NV_DECLARE_ALIGNED(NvU64 timestamp, 8);
    NvU8  recType;
    NvU32 bugcheck;
    char  source[NV2080_NOCAT_JOURNAL_MAX_STR_LEN];
    NvU32 subsystem;
    NV_DECLARE_ALIGNED(NvU64 errorCode, 8);
    char  faultingEngine[NV2080_NOCAT_JOURNAL_MAX_STR_LEN];
    NvU32 tdrReason;
    NvU32 diagBufferLen;
    NvU8  diagBuffer[NV2080_NOCAT_JOURNAL_MAX_DIAG_BUFFER];
} NV2080CtrlNocatJournalInsertRecord;
#define NV2080_CTRL_CMD_NVD_INSERT_NOCAT_JOURNAL_RECORD_PARAMS_MESSAGE_ID (0xCU)

typedef struct NV2080_CTRL_CMD_NVD_INSERT_NOCAT_JOURNAL_RECORD_PARAMS {
    NV_DECLARE_ALIGNED(NV2080CtrlNocatJournalInsertRecord nocatJournalRecord, 8);
} NV2080_CTRL_CMD_NVD_INSERT_NOCAT_JOURNAL_RECORD_PARAMS;
/* _ctr2080nvd_h_ */
