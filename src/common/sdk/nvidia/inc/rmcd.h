/*
 * SPDX-FileCopyrightText: Copyright (c) 2002-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef RMCD_H
#define RMCD_H

//******************************************************************************
//
// Module Name: RMCD.H
//
// This file contains structures and constants that define the resource manager
// specific data for the crash dump file. The record definitions defined here
// are always stored after the crash dump file header. Each record defined here
// is preceded by the NVCD_RECORD structure.
//
//******************************************************************************

#include "nvgputypes.h"
#include "nvcd.h"

#define NV_RMCD_VERSION  (20)

// Define RC Reset Callback function type
#define RM_RC_CALLBACK_HANDLE_UPDATE 1
#if RM_RC_CALLBACK_HANDLE_UPDATE
typedef NvU32 RC_RESET_CALLBACK(NvHandle hClient, NvHandle hDevice, NvHandle hFifo, NvHandle hChannel,
                                void *pContext, NvBool clearRc);
#else
typedef NvU32 RC_RESET_CALLBACK(NvHandle hClient, NvHandle hDevice, NvHandle hChannel, void *pContext, NvBool clearRc);
#endif

typedef struct _rc_error_context {
    struct OBJGPU   *pGpu;          // GPU device
    NvU32           ChId;           // Channel ID of channel in RC recovery
    NvU32           secChId;        // Channel ID of secondary channel
    NvHandle        sechClient;     // Client handle of secondary channel
    NvU32           exceptType;     // ROBUST_CHANNEL_* error identifier
    NvU32           EngineId;       // Engine ID of faulting engine
    NvU32           subdeviceInstance;       // This parameter returns the unicast subdevice instance number
                                             // associated with the specified GPU.  This value can be used to
                                             // instantiate a unicast reference to the GPU using the NV20_SUBDEVICE
                                             // classes.

    // FIFO_MMU_EXCEPTION_DATA
    NvU32           addrLo;         // MMU fault address (Lo)
    NvU32           addrHi;         // MMU fault address (Hi)
    NvU32           faultType;      // MMU fault code
    const char      *faultStr;      // MMU fault string

} RC_ERROR_CONTEXT, *PRC_ERROR_CONTEXT;

#define MAX_FBBAS               0x2
#define FBBA_MUX_SEL_MAX        0xF
#define FBBA_RXB_STATUS_SEL_MAX 0x10
#define RDSTAT_MAX              0x2

// Define the resource manager group record types
typedef enum
{
    RmCurrentStateInfo_V2   = 4,    // Revised Version 2 RM current state info. record
    RmMissedNotifierInfo    = 9,    // RM Driver Indicates Missed Notifier
    RmGlobalInfo_V3         = 10,   // Revised Version 3 of Global Info

    RmProtoBuf              = 131,    // ProtoBuf
    RmProtoBuf_V2           = 132,    // ProtoBuf + NvDump
    RmDclMsg                = 133,    // One of the optional DlcMsg fields, encoded
    RmJournalEngDump        = 134,
    DP_ASSERT_HIT           = 135,
    DP_LOG_CALL             = 136,
    RmPrbFullDump           = 137,    // Full NvDump protobuf message

    // Includes RmRCCommonJournal_RECORD structure.
    RmJournalBugCheck       = 138,    // Bugcheck record
    RmRC2SwRmAssert_V3      = 139,    // Retail logged RM_ASSERT info
    RmRC2GpuTimeout_V3      = 140,    // Retail logged RM_TIMEOUT events
    RmRC2SwDbgBreakpoint_V3 = 141,    // Uses same format as RmRC2SwRmAssert_V3
    RmBadRead_V2            = 142,    // Record with Bad Read Information
    RmSurpriseRemoval_V2    = 143,    // Surprise Removal
    RmPowerState            = 144,    // PowerState
    RmPrbErrorInfo_V2       = 145,    // Protobuf error record
    RmPrbFullDump_V2        = 146,    // Full NvDump protobuf message
    RmRcDiagReport          = 147,    // Rc Diagnostic report message
    RmNocatReport           = 149,    // Nocat reports.
    RmDispState             = 150,    // Display error record

} RMCD_RECORD_TYPE;

typedef struct {
    NVCD_RECORD Header;                 // Global information record header
    NvU32       dwSize;                 // Total Protobuf Message Size
}  RmProtoBuf_RECORD;
typedef RmProtoBuf_RECORD *PRmProtoBuf_RECORD;

//
// Please include RmRCCommonJournal_RECORD structure at the top of all new and
// revised journal record structures.
//
#define NV_RM_JOURNAL_STATE_MASK_GC6_STATE                                3:0

#define NV_RM_JOURNAL_STATE_MASK_IS_NOT_FULL_POWER                 0x00000010
#define NV_RM_JOURNAL_STATE_MASK_IS_NOT_CONNECTED                  0x00000020
#define NV_RM_JOURNAL_STATE_MASK_IS_IN_STANDBY                     0x00000040
#define NV_RM_JOURNAL_STATE_MASK_IS_IN_HIBERNATE                   0x00000080
#define NV_RM_JOURNAL_STATE_MASK_IS_IN_PM_CODEPATH                 0x00000100
#define NV_RM_JOURNAL_STATE_MASK_IS_IN_GC6_RESET                   0x00000200
#define NV_RM_JOURNAL_STATE_MASK_IS_IN_FULLCHIP_RESET              0x00000400
#define NV_RM_JOURNAL_STATE_MASK_IS_IN_SEC_BUS_RESET               0x00000800
#define NV_RM_JOURNAL_STATE_MASK_IS_IN_TIMEOUT_RECOVERY            0x00001000
#define NV_RM_JOURNAL_STATE_MASK_IS_LOST                           0x00002000
#define NV_RM_JOURNAL_STATE_MASK_VIDMEM_FAILED_BAR0                0x00004000
#define NV_RM_JOURNAL_STATE_MASK_VIDMEM_FAILED_BAR2                0x00008000
#define NV_RM_JOURNAL_STATE_MASK_VIDMEM_FAILED_FBHUB               0x00010000
#define NV_RM_JOURNAL_STATE_MASK_GSP_RM_ENABLED                    0x00020000


typedef struct _RmRCCommonJournal_RECORD
{
    NVCD_RECORD Header;
    NvU32       GPUTag;
    NvU64       CPUTag;
    NvU64       timeStamp    NV_ALIGN_BYTES(8);
    NvU64       stateMask;
    NvU8       *pNext;              // temporary  link used for sorting.
                                    // note using type NvU8 because Linux
                                    // generated code does not properly
                                    // resove the refeerence to the struct type.
                                    // & balks about void * type.
} RmRCCommonJournal_RECORD;
typedef RmRCCommonJournal_RECORD *PRmRCCommonJournal_RECORD;

// RM_ASSERT + DBG_BREAKPOINT info
typedef struct
{
    RmRCCommonJournal_RECORD common;
    NvU64       lastTimeStamp      NV_ALIGN_BYTES(8);
    NvU64       breakpointAddrHint NV_ALIGN_BYTES(8);   // address that can identify bp module
    NvU64       callStack[10]      NV_ALIGN_BYTES(8);   // Call stack when the assert occurred.
    NvU32       count;
    NvU32       lineNum;
    NvU32       lastReportedCount;                  // last count reported to NOCAT
    NvU64       nextReportedTimeStamp;              // time stamp to use as start time for the
                                                    // next NOCAT report.
} RmRCCommonAssert_RECORD;
typedef RmRCCommonAssert_RECORD *PRmRCCommonAssert_RECORD;
#define NV_RM_ASSERT_UNKNOWN_LINE_NUM     0

typedef RmRCCommonAssert_RECORD RmRC2GpuTimeout3_RECORD;
typedef RmRC2GpuTimeout3_RECORD *PRmRC2GpuTimeout3_RECORD;

// How Serious is this RM_ASSERT/DBG_BREAKPOINT
// (1) Info -- This is unexpected but we should continue to run
// (2) Error --
// (3) Fatal -- This is hopeless -- FBI Timeout, Bus Error Etc
#define NV_RM_ASSERT_TYPE                3:0
#define NV_RM_ASSERT_TYPE_INFO           0x00000001
#define NV_RM_ASSERT_TYPE_ERROR          0x00000002
#define NV_RM_ASSERT_TYPE_FATAL          0x00000003

// HW Unit which is having the issue
#define NV_RM_ASSERT_HW_UNIT             15:8
#define NV_RM_ASSERT_HW_UNIT_NULL        (0x00)
#define NV_RM_ASSERT_HW_UNIT_GRAPHICS    (0x01)
#define NV_RM_ASSERT_HW_UNIT_COPY0       (0x02)
#define NV_RM_ASSERT_HW_UNIT_COPY1       (0x03)
#define NV_RM_ASSERT_HW_UNIT_VP          (0x04)
#define NV_RM_ASSERT_HW_UNIT_ME          (0x05)
#define NV_RM_ASSERT_HW_UNIT_PPP         (0x06)
#define NV_RM_ASSERT_HW_UNIT_BSP         (0x07)
#define NV_RM_ASSERT_HW_UNIT_NVDEC0      NV_RM_ASSERT_HW_UNIT_BSP
#define NV_RM_ASSERT_HW_UNIT_MPEG        (0x08)
#define NV_RM_ASSERT_HW_UNIT_SW          (0x09)
#define NV_RM_ASSERT_HW_UNIT_CIPHER      (0x0a)
#define NV_RM_ASSERT_HW_UNIT_VIC         (0x0b)
#define NV_RM_ASSERT_HW_UNIT_MSENC       (0x0c)
#define NV_RM_ASSERT_HW_UNIT_NVENC0      NV_RM_ASSERT_HW_UNIT_MSENC
#define NV_RM_ASSERT_HW_UNIT_NVENC1      (0x0d)
#define NV_RM_ASSERT_HW_UNIT_HOST        (0x0e)
#define NV_RM_ASSERT_HW_UNIT_ROM         (0x0f)
#define NV_RM_ASSERT_HW_UNIT_INSTMEM     (0x10)
#define NV_RM_ASSERT_HW_UNIT_DISP        (0x11)
#define NV_RM_ASSERT_HW_UNIT_NVENC2      (0x12)
#define NV_RM_ASSERT_HW_UNIT_NVDEC1      (0x13)
#define NV_RM_ASSERT_HW_UNIT_NVDEC2      (0x14)
#define NV_RM_ASSERT_HW_UNIT_NVDEC3      (0x15)
#define NV_RM_ASSERT_HW_UNIT_NVDEC4      (0x16)
#define NV_RM_ASSERT_HW_UNIT_NVDEC5      (0x17)
#define NV_RM_ASSERT_HW_UNIT_NVDEC6      (0x18)
#define NV_RM_ASSERT_HW_UNIT_NVDEC7      (0x19)
#define NV_RM_ASSERT_HW_UNIT_ALLENGINES  (0xff)
// SW Module which generated the error
#define NV_RM_ASSERT_SW_MODULE           15:8

// This is a specific error number which we wish to follow regardless of builds
// We want to use this for backend processing
// This is also a compromise.  Ideally, each event would have a unique id but
// instead of doing this we use EIP which is unique per load.  If we subtracted off
// the Module Load Address then it would be unique per build,  Using EIP allows us
// to use the debugger to lookup the source code that corresponds to the event.
#define NV_RM_ASSERT_LEVEL_TAG          30:16
// Host Errors
#define NV_RM_ASSERT_LEVEL_TAG_BAR1_PAGE_FAULT      (0x0001)
#define NV_RM_ASSERT_LEVEL_TAG_IFB_PAGE_FAULT       (0x0002)
#define NV_RM_ASSERT_LEVEL_TAG_PIO_ERROR            (0x0003)
#define NV_RM_ASSERT_LEVEL_TAG_CHSW_SAVE_INVALID    (0x0004)
#define NV_RM_ASSERT_LEVEL_TAG_CHSW_ERROR           (0x0005)
#define NV_RM_ASSERT_LEVEL_TAG_FBIRD_TIMEOUT        (0x0006)
#define NV_RM_ASSERT_LEVEL_TAG_CPUQ_FBIBUSY_TIMEOUT (0x0007)
#define NV_RM_ASSERT_LEVEL_TAG_CHSW_FSM_TIMEOUT     (0x0008)
#define NV_RM_ASSERT_LEVEL_TAG_FB_FLUSH_TIMEOUT     (0x0009)
#define NV_RM_ASSERT_LEVEL_TAG_P2PSTATE_TIMEOUT     (0x000a)
#define NV_RM_ASSERT_LEVEL_TAG_VBIOS_CHECKSUM       (0x000b)
#define NV_RM_ASSERT_LEVEL_TAG_DISP_SYNC            (0x000c)

// What is the generating Source -- GPU or SW
#define NV_RM_ASSERT_LEVEL_SOURCE        31:31
#define NV_RM_ASSERT_LEVEL_SOURCE_SW     0x00000000
#define NV_RM_ASSERT_LEVEL_SOURCE_HW     0x00000001


// RM_ASSERT + DBG_BREAKPOINT info
typedef struct
{
    RmRCCommonAssert_RECORD commonAssert;
    NvU32       level;
} RmRC2SwRmAssert3_RECORD;
typedef RmRC2SwRmAssert3_RECORD *PRmRC2SwRmAssert3_RECORD;

// RM JOURNAL BUG CHECK
typedef struct
{
    RmRCCommonJournal_RECORD common;
    NvU32                    bugCheckCode;
} RmJournalBugcheck_RECORD, *PRmJournalBugcheck_RECORD;

typedef struct
{
    NVCD_RECORD Header;
    NvS32       GpuTemp;
    NvS32       LocTemp;
} RmRC2TempReading_RECORD;
typedef RmRC2TempReading_RECORD *PRmRC2TempReading_RECORD;

typedef enum {
    MEMORY_BAR0 = 1,
    MEMORY_FB,
    MEMORY_INSTANCE,
    MEMORY_PCI,
} RMCD_BAD_READ_SPACE;

typedef struct
{
    RmRCCommonJournal_RECORD common;
    NvU32       MemorySpace;            // Which Memory Space
    NvU32       Offset;                 // Offset in Memory Space
    NvU32       Mask;                   // Mask used to detect bad read
    NvU32       Value;                  // Value Return
    NvU32       Reason;                 // Potential Reason why this might have happened
} RmRC2BadRead2_RECORD;

typedef RmRC2BadRead2_RECORD *PRmRC2BadRead2_RECORD;

typedef struct
{
    RmRCCommonJournal_RECORD common;
    NvU32       hostEngTag;
    NvU32       exceptType;
    NvU32       exceptLevel;
} RmRCRecovery_RECORD;

typedef RmRCRecovery_RECORD *PRmRCRecovery_RECORD;

typedef struct
{
    RmRCCommonJournal_RECORD common;
} RmPrbInfo_RECORD_V2, *PRmPrbInfo_RECORD_V2;

// Counters per type of RC Error.  WIll only keep first n (10) types.
#define MAX_RC_ERROR_COUNTER            10
#define RC_ERROR_COUNTER_OTHER_INDEX    MAX_RC_ERROR_COUNTER-1
#define RC_ERROR_COUNTER_TYPE_INVALID   ((NvU32)(~0))
#define RC_ERROR_COUNTER_OTHER_TYPE     1024

typedef struct
{
    NvU32        rcFaultType;
    NvU32        rcErrorSrc;
    NvU64        mmuFaultVA;
    NvU32        mmuAccessType;
} rcExtraMMUInfo;

typedef struct
{
    NvU32        rcGRErrType;
    NvU32        rcGRExcptnSubTyp;
} rcExtraGRInfo;

typedef struct
{
    NvU32        rcPBDMAID;
    NvU64        rcPBDMAErrFlag;
} rcExtraPBDMAInfo;

/*
* Union for storing extra data values for each kind of RC Error
*
*        MMU
*       =====
*      rcFaultType
*      rcErrorSrc
*
*        GR
*      =======
*      rcGRErrType
*      rcGRExcptnSubTyp
*
*        PBDMA
*      =========
*      rcPBDMAID
*      rcPBDMAErrFlag
*/

typedef union
{
    rcExtraMMUInfo      rcMMUData;
    rcExtraGRInfo       rcGRData;
    rcExtraPBDMAInfo    rcPBDMAData;
} rcExtraInfo;

typedef struct
{
    NvU32        rcErrorType;
    NvU32        rcErrorCount;
    NvU32        rcLastCHID;
    NvU64        rcLastTime;
    rcExtraInfo  rcExtrInfo;
} rcErrorCounterEntry;

typedef struct
{
    NvU32                    powerEvent;   // The name of the point we are tracing
                                           // Such as NVRM_OCA_PWR_EVENT_SET_POWER_ENTRY
    NvU32                    powerState;
    NvU32                    fastBootPowerState;    // Tracking fast boot shutdown/hibernate states
} RmPowerState_RECORD, *PRmPowerState_RECORD;

#define MAX_RCDB_RCDIAG_ENTRIES     200
typedef struct
{
    NvU32   offset;
    NvU32   tag;
    NvU32   value;
    NvU32   attribute;
} RmRcDiagRecordEntry;

#define RCDB_RCDIAG_DEFAULT_OWNER   0
typedef struct
{
    NvU16                   idx;
    NvU32                   timeStamp;      // time in seconds since 1/1/1970
    NvU16                   type;
    NvU32                   flags;
    NvU16                   count;
    NvU32                   owner;          // a handle indicating the ownership
    NvU32                   processId;      // process Id of the originating process.
    RmRcDiagRecordEntry     data[MAX_RCDB_RCDIAG_ENTRIES];
} RmRcDiag_RECORD, *PRmRcDiag_RECORD;

#define MAX_RCDB_RCDIAG_WRAP_BUFF                 32
#define INVALID_RCDB_RCDIAG_INDEX                 0xffffffff

#define MAX_RCDB_NOCAT_WRAP_BUFF                  100
#define INVALID_RCDB_NOCAT_ID                     0xffffffff

#endif // RMCD_H
