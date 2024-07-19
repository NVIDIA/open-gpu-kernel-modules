/*******************************************************************************
    Copyright (c) 2013-2024 NVidia Corporation

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be
        included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*******************************************************************************/

//
//     uvm_types.h
//
//     This file contains basic datatypes that UVM requires.
//

#ifndef _UVM_TYPES_H_
#define _UVM_TYPES_H_

#include "nvlimits.h"
#include "nvtypes.h"
#include "nvstatus.h"
#include "nvCpuUuid.h"


/*******************************************************************************
    UVM stream types
*******************************************************************************/

typedef enum
{
    UvmStreamTypeRegular = 0,
    UvmStreamTypeAll = 1,
    UvmStreamTypeNone = 2
} UvmStreamType;

#define UVM_STREAM_INVALID  ((UvmStream)0ULL)
#define UVM_STREAM_ALL      ((UvmStream)2ULL)
#define UVM_STREAM_NONE     ((UvmStream)3ULL)

typedef unsigned long long UvmStream;

// The maximum number of sub-processors per parent GPU.
#define UVM_PARENT_ID_MAX_SUB_PROCESSORS 8

// The maximum number of GPUs changed when multiple MIG instances per
// uvm_parent_gpu_t were added. The old version is kept as a convenience
// for code that needs to maintain forward compatibility.
#define UVM_MAX_GPUS_V1       NV_MAX_DEVICES
#define UVM_MAX_PROCESSORS_V1 (UVM_MAX_GPUS_V1 + 1)
#define UVM_MAX_GPUS          (NV_MAX_DEVICES * UVM_PARENT_ID_MAX_SUB_PROCESSORS)
#define UVM_MAX_PROCESSORS    (UVM_MAX_GPUS + 1)

#define UVM_PROCESSOR_MASK_SIZE ((UVM_MAX_PROCESSORS + (sizeof(NvU64) * 8) - 1) / (sizeof(NvU64) * 8))

#define UVM_INIT_FLAGS_DISABLE_HMM                       ((NvU64)0x1)
#define UVM_INIT_FLAGS_MULTI_PROCESS_SHARING_MODE        ((NvU64)0x2)
#define UVM_INIT_FLAGS_MASK                              ((NvU64)0x3)

#define UVM_RANGE_GROUP_ID_NONE        ((NvU64)0)

//------------------------------------------------------------------------------
// UVM GPU mapping types
//
// These types indicate the kinds of accesses allowed from a given GPU at the
// specified virtual address range. There are 3 basic kinds of accesses: read,
// write and atomics. Each type indicates what kinds of accesses are allowed.
// Accesses of any disallowed kind are fatal. The "Default" type specifies that
// the UVM driver should decide on the types of accesses allowed.
//------------------------------------------------------------------------------
typedef enum
{
    UvmGpuMappingTypeDefault = 0,
    UvmGpuMappingTypeReadWriteAtomic = 1,
    UvmGpuMappingTypeReadWrite = 2,
    UvmGpuMappingTypeReadOnly = 3,
    UvmGpuMappingTypeCount = 4
} UvmGpuMappingType;

//------------------------------------------------------------------------------
// UVM GPU caching types
//
// These types indicate the cacheability of the specified virtual address range
// from a given GPU. The "Default" type specifies that the UVM driver should
// set caching on or off as required to follow the UVM coherence model. The
// "ForceUncached" and "ForceCached" types will always turn caching off or on
// respectively. These two types override the cacheability specified by the UVM
// coherence model.
//------------------------------------------------------------------------------
typedef enum
{
    UvmGpuCachingTypeDefault = 0,
    UvmGpuCachingTypeForceUncached = 1,
    UvmGpuCachingTypeForceCached = 2,
    UvmGpuCachingTypeCount = 3
} UvmGpuCachingType;

//------------------------------------------------------------------------------
// UVM GPU format types
//
// These types indicate the memory format of the specified virtual address
// range for a given GPU. The "Default" type specifies that the UVM driver will
// detect the format based on the allocation and is mutually inclusive with
// UvmGpuFormatElementBitsDefault.
//------------------------------------------------------------------------------
typedef enum {
   UvmGpuFormatTypeDefault = 0,
   UvmGpuFormatTypeBlockLinear = 1,
   UvmGpuFormatTypeCount = 2
} UvmGpuFormatType;

//------------------------------------------------------------------------------
// UVM GPU Element bits types
//
// These types indicate the element size of the specified virtual address range
// for a given GPU. The "Default" type specifies that the UVM driver will
// detect the element size based on the allocation and is mutually inclusive
// with UvmGpuFormatTypeDefault. The element size is specified in bits:
// UvmGpuFormatElementBits8 uses the 8-bits format.
//------------------------------------------------------------------------------
typedef enum {
   UvmGpuFormatElementBitsDefault = 0,
   UvmGpuFormatElementBits8 = 1,
   UvmGpuFormatElementBits16 = 2,
   // Cuda does not support 24-bit width
   UvmGpuFormatElementBits32 = 4,
   UvmGpuFormatElementBits64 = 5,
   UvmGpuFormatElementBits128 = 6,
   UvmGpuFormatElementBitsCount = 7
} UvmGpuFormatElementBits;

//------------------------------------------------------------------------------
// UVM GPU Compression types
//
// These types indicate the compression type of the specified virtual address
// range for a given GPU. The "Default" type specifies that the UVM driver will
// detect the compression attributes based on the allocation. Any type other
// than the default will override the compression behavior of the physical
// allocation. UvmGpuCompressionTypeEnabledNoPlc will disable PLC but enables
// generic compression. UvmGpuCompressionTypeEnabledNoPlc type is only supported
// on Turing plus GPUs. Since UvmGpuCompressionTypeEnabledNoPlc type enables
// generic compression, it can only be used when the compression attribute of
// the underlying physical allocation is enabled.
//------------------------------------------------------------------------------
typedef enum {
    UvmGpuCompressionTypeDefault = 0,
    UvmGpuCompressionTypeEnabledNoPlc = 1,
    UvmGpuCompressionTypeCount = 2
} UvmGpuCompressionType;

typedef struct
{
    // UUID of the physical GPU if the GPU is not SMC capable or SMC enabled,
    // or the GPU instance UUID of the partition.
    NvProcessorUuid gpuUuid;
    NvU32           gpuMappingType;     // UvmGpuMappingType
    NvU32           gpuCachingType;     // UvmGpuCachingType
    NvU32           gpuFormatType;      // UvmGpuFormatType
    NvU32           gpuElementBits;     // UvmGpuFormatElementBits
    NvU32           gpuCompressionType; // UvmGpuCompressionType
} UvmGpuMappingAttributes;

// forward declaration of OS-dependent structure
struct UvmGlobalState_tag;

// Platform specific parameters for UvmRegisterGpu*
typedef union
{
    struct {
        // File descriptor for RM's control file
        int ctrlFd;
        // RM client handle
        NvHandle hClient;
        // RM SMC partition reference
        NvHandle hSmcPartRef;
    } rm_linux;
} UvmGpuPlatformParams;

// Platform specific parameters for UvmRegisterGpuVaSpace
typedef union
{
    struct {
        // File descriptor for RM's control file
        int ctrlFd;
        // RM client handle
        NvHandle hClient;
        // RM GPU VA space handle
        NvHandle hVaSpace;
    } rm_linux;
    struct {
        // RM client handle
        NvHandle hClient;
        // RM GPU VA space handle
        NvHandle hVaSpace;
    } rm_windows;
} UvmGpuVaSpacePlatformParams;

// Platform specific parameters for UvmRegisterChannel and UvmUnregisterChannel
typedef union
{
    struct {
        // File descriptor for RM's control file
        int ctrlFd;
        // RM client handle
        NvHandle hClient;
        // RM channel handle
        NvHandle hChannel;
    } rm_linux;
} UvmChannelPlatformParams;

// Platform specific parameters for UvmMapExternalAllocation
typedef union
{
    struct {
        // File descriptor for RM's control file
        int ctrlFd;
        // RM client handle
        NvHandle hClient;
        // RM allocation handle
        NvHandle hMemory;
    } rm_linux;
} UvmAllocationPlatformParams;

//------------------------------------------------------------------------------
//    Tools API types
//------------------------------------------------------------------------------

#define UVM_DEBUG_V1    0x00000001

typedef NvUPtr UvmDebugSession;

//------------------------------------------------------------------------------
// Counter scope: It can be one of the following:
// - Single GPU for a process (UvmCounterScopeProcessSingleGpu)
// - Aggregate of all GPUs for a process (UvmCounterScopeProcessAllGpu)
// - Single GPU system-wide (UvmCounterScopeGlobalSingleGpu)
// (UvmCounterScopeGlobalSingleGpu is not supported for CUDA 6.0)
//
// Note: The user must not assume that the counter values are equal to zero
// at the time of enabling counters.
// Difference between end state counter value and start state counter value
// should be used to find out the correct value over a given period of time.
//------------------------------------------------------------------------------
typedef enum
{
    UvmCounterScopeProcessSingleGpu = 0,
    UvmCounterScopeProcessAllGpu = 1,
    UvmCounterScopeGlobalSingleGpu = 2,
    UvmCounterScopeSize
} UvmCounterScope;

//------------------------------------------------------------------------------
// Following numbers assigned to the counter name are used to index their value
// in the counter array.
//------------------------------------------------------------------------------
typedef enum
{
    UvmCounterNameBytesXferHtD = 0,         // host to device
    UvmCounterNameBytesXferDtH = 1,         // device to host
    UvmCounterNameCpuPageFaultCount = 2,
#ifdef __windows__
    UvmCounterNameWddmBytesXferBtH = 3,     // backing store to host
    UvmCounterNameWddmBytesXferHtB = 4,     // host to backing store
    //
    // eviction (device to backing store)
    //
    UvmCounterNameWddmBytesXferDtB = 5,
    //
    // restoration (backing store to device)
    //
    UvmCounterNameWddmBytesXferBtD = 6,
#endif
    //
    // bytes prefetched host to device.
    // These bytes are also counted in
    // UvmCounterNameBytesXferHtD
    //
    UvmCounterNamePrefetchBytesXferHtD = 7,
    //
    // bytes prefetched device to host.
    // These bytes are also counted in
    // UvmCounterNameBytesXferDtH
    //
    UvmCounterNamePrefetchBytesXferDtH = 8,
    //
    // number of faults reported on the GPU
    //
    UvmCounterNameGpuPageFaultCount = 9,
    UVM_TOTAL_COUNTERS
} UvmCounterName;

#define UVM_COUNTER_NAME_FLAG_BYTES_XFER_HTD 0x1
#define UVM_COUNTER_NAME_FLAG_BYTES_XFER_DTH 0x2
#define UVM_COUNTER_NAME_FLAG_CPU_PAGE_FAULT_COUNT 0x4
#define UVM_COUNTER_NAME_FLAG_WDDM_BYTES_XFER_BTH 0x8
#define UVM_COUNTER_NAME_FLAG_WDDM_BYTES_XFER_HTB 0x10
#define UVM_COUNTER_NAME_FLAG_BYTES_XFER_DTB 0x20
#define UVM_COUNTER_NAME_FLAG_BYTES_XFER_BTD 0x40
#define UVM_COUNTER_NAME_FLAG_PREFETCH_BYTES_XFER_HTD 0x80
#define UVM_COUNTER_NAME_FLAG_PREFETCH_BYTES_XFER_DTH 0x100
#define UVM_COUNTER_NAME_FLAG_GPU_PAGE_FAULT_COUNT 0x200

//------------------------------------------------------------------------------
// UVM counter config structure
//
// - scope: Please see the UvmCounterScope  enum (above), for details.
// - name: Name of the counter. Please check UvmCounterName for list.
// - gpuid: Identifies the GPU for which the counter will be enabled/disabled
//          This parameter is ignored in AllGpu scopes.
// - state: A value of 0 will disable the counter, a value of 1 will enable
//           the counter.
//------------------------------------------------------------------------------
typedef struct
{
    NvU32           scope; //UVM_DEBUG_V1 (UvmCounterScope)
    NvU32           name;  //UVM_DEBUG_V1 (UvmCounterName)
    NvProcessorUuid gpuid; //UVM_DEBUG_V1
    NvU32           state; //UVM_DEBUG_V1
} UvmCounterConfig;

#define UVM_COUNTER_CONFIG_STATE_DISABLE_REQUESTED  0
#define UVM_COUNTER_CONFIG_STATE_ENABLE_REQUESTED   1

typedef enum
{
    UvmEventMemoryAccessTypeInvalid        = 0,
    UvmEventMemoryAccessTypeRead           = 1,
    UvmEventMemoryAccessTypeWrite          = 2,
    UvmEventMemoryAccessTypeAtomic         = 3,
    UvmEventMemoryAccessTypePrefetch       = 4,
    // ---- Add new values above this line
    UvmEventNumMemoryAccessTypes
} UvmEventMemoryAccessType;

typedef enum
{
    UvmEventTypeInvalid                    = 0,

    UvmEventTypeMemoryViolation            = 1,
    UvmEventTypeCpuFault                   = UvmEventTypeMemoryViolation,
    UvmEventTypeMigration                  = 2,
    UvmEventTypeGpuFault                   = 3,
    UvmEventTypeGpuFaultReplay             = 4,
    UvmEventTypeFaultBufferOverflow        = 5,
    UvmEventTypeFatalFault                 = 6,
    UvmEventTypeReadDuplicate              = 7,
    UvmEventTypeReadDuplicateInvalidate    = 8,
    UvmEventTypePageSizeChange             = 9,
    UvmEventTypeThrashingDetected          = 10,
    UvmEventTypeThrottlingStart            = 11,
    UvmEventTypeThrottlingEnd              = 12,
    UvmEventTypeMapRemote                  = 13,
    UvmEventTypeEviction                   = 14,

    // ---- Add new values above this line
    UvmEventNumTypes,

    // ---- Private event types for uvm tests
    UvmEventTestTypesFirst                 = 62,

    UvmEventTypeTestHmmSplitInvalidate     = UvmEventTestTypesFirst,
    UvmEventTypeTestAccessCounter          = UvmEventTestTypesFirst + 1,

    UvmEventTestTypesLast                  = UvmEventTypeTestAccessCounter,

    UvmEventNumTypesAll
} UvmEventType;

//------------------------------------------------------------------------------
// Bit flags used to enable/ disable events:
//------------------------------------------------------------------------------
#define UVM_EVENT_ENABLE_MEMORY_VIOLATION             ((NvU64)1 << UvmEventTypeMemoryViolation)
#define UVM_EVENT_ENABLE_CPU_FAULT                    ((NvU64)1 << UvmEventTypeCpuFault)
#define UVM_EVENT_ENABLE_MIGRATION                    ((NvU64)1 << UvmEventTypeMigration)
#define UVM_EVENT_ENABLE_GPU_FAULT                    ((NvU64)1 << UvmEventTypeGpuFault)
#define UVM_EVENT_ENABLE_GPU_FAULT_REPLAY             ((NvU64)1 << UvmEventTypeGpuFaultReplay)
#define UVM_EVENT_ENABLE_FAULT_BUFFER_OVERFLOW        ((NvU64)1 << UvmEventTypeFaultBufferOverflow)
#define UVM_EVENT_ENABLE_FATAL_FAULT                  ((NvU64)1 << UvmEventTypeFatalFault)
#define UVM_EVENT_ENABLE_READ_DUPLICATE               ((NvU64)1 << UvmEventTypeReadDuplicate)
#define UVM_EVENT_ENABLE_READ_DUPLICATE_INVALIDATE    ((NvU64)1 << UvmEventTypeReadDuplicateInvalidate)
#define UVM_EVENT_ENABLE_PAGE_SIZE_CHANGE             ((NvU64)1 << UvmEventTypePageSizeChange)
#define UVM_EVENT_ENABLE_THRASHING_DETECTED           ((NvU64)1 << UvmEventTypeThrashingDetected)
#define UVM_EVENT_ENABLE_THROTTLING_START             ((NvU64)1 << UvmEventTypeThrottlingStart)
#define UVM_EVENT_ENABLE_THROTTLING_END               ((NvU64)1 << UvmEventTypeThrottlingEnd)
#define UVM_EVENT_ENABLE_MAP_REMOTE                   ((NvU64)1 << UvmEventTypeMapRemote)
#define UVM_EVENT_ENABLE_EVICTION                     ((NvU64)1 << UvmEventTypeEviction)
#define UVM_EVENT_ENABLE_TEST_ACCESS_COUNTER          ((NvU64)1 << UvmEventTypeTestAccessCounter)
#define UVM_EVENT_ENABLE_TEST_HMM_SPLIT_INVALIDATE    ((NvU64)1 << UvmEventTypeTestHmmSplitInvalidate)

//------------------------------------------------------------------------------
// Information associated with a memory violation event
//------------------------------------------------------------------------------
typedef struct
{
    //
    // eventType has to be 1st argument of this structure. Setting eventType to
    // UvmEventTypeMemoryViolation helps to identify event data in a queue.
    //
    NvU8 eventType;
    NvU8 accessType;          // read/write violation (UvmEventMemoryAccessType)
    //
    // This structure is shared between UVM kernel and tools.
    // Manually padding the structure so that compiler options like pragma pack
    // or malign-double will have no effect on the field offsets.
    //
    NvU16 padding16Bits;
    NvU32 padding32Bits;
    NvU64 address;            // faulting address
    NvU64 timeStamp;          // cpu time when the fault occurred
    NvU32 pid;                // process id causing the fault
    NvU32 threadId;           // thread id causing the fault
    NvU64 pc;                 // address of the instruction causing the fault
} UvmEventCpuFaultInfo;

typedef enum
{
    UvmEventMigrationDirectionInvalid = 0,
    UvmEventMigrationDirectionCpuToGpu = 1,
    UvmEventMigrationDirectionGpuToCpu = 2,
    // ---- Add new values above this line
    UvmEventNumMigrationDirections
} UvmEventMigrationDirection;

//------------------------------------------------------------------------------
// Information associated with a migration event
//------------------------------------------------------------------------------
typedef struct
{
    //
    // eventType has to be the 1st argument of this structure.
    // Setting eventType = UvmEventTypeMigration helps to identify event data in
    // a queue.
    //
    NvU8 eventType;
    // direction of migration (UvmEventMigrationDirection )
    // this field is deprecated, in favor of (src|dst)Index
    NvU8 direction;
    //
    // Indices are used for the source and destination of migration instead of
    // using gpu uuid/cpu id. This reduces the size of each event. gpuIndex to
    // gpuUuid relation can be obtained from UvmEventGetGpuUuidTable.
    // Currently we do not distinguish between CPUs so they all use index 0xFF.
    //
    NvU8 srcIndex;                 // source CPU/GPU index
    NvU8 dstIndex;                 // destination CPU/GPU index
    //
    // This structure is shared between UVM kernel and tools.
    // Manually padding the structure so that compiler options like pragma pack
    // or malign-double will have no effect on the field offsets
    //
    NvU32 padding32Bits;
    NvU64 address;                 // base virtual addr used for migration
    NvU64 migratedBytes;           // number of bytes migrated
    NvU64 beginTimeStamp;          // cpu time stamp when the migration was
                                   // queued on the gpu
    NvU64 endTimeStamp;            // cpu time stamp when the migration
                                   // finalization was communicated to the cpu
    NvU64 streamId;                // stream causing the migration
} UvmEventMigrationInfo_Lite;

typedef enum
{
    // These fault types are handled and may be "fixed" by the UVM driver
    UvmFaultTypeInvalid                                                        = 0,
    UvmFaultTypeInvalidPde                                                     = 1,
    UvmFaultTypeInvalidPte                                                     = 2,
    UvmFaultTypeWrite                                                          = 3,
    UvmFaultTypeAtomic                                                         = 4,
    // The next fault types are fatal and cannot be serviced by the UVM driver
    UvmFaultTypeFatal                                                          = 5,
    UvmFaultTypeInvalidPdeSize                                                 = UvmFaultTypeFatal,
    UvmFaultTypeLimitViolation                                                 = 6,
    UvmFaultTypeUnboundInstBlock                                               = 7,
    UvmFaultTypePrivViolation                                                  = 8,
    UvmFaultTypePitchMaskViolation                                             = 9,
    UvmFaultTypeWorkCreation                                                   = 10,
    UvmFaultTypeUnsupportedAperture                                            = 11,
    UvmFaultTypeCompressionFailure                                             = 12,
    UvmFaultTypeUnsupportedKind                                                = 13,
    UvmFaultTypeRegionViolation                                                = 14,
    UvmFaultTypePoison                                                         = 15,
    UvmFaultTypeCcViolation                                                    = 16,
    // ---- Add new values above this line
    UvmEventNumFaultTypes
} UvmEventFaultType;

typedef enum
{
    UvmEventFatalReasonInvalid            = 0,
    UvmEventFatalReasonInvalidAddress     = 1,
    UvmEventFatalReasonInvalidPermissions = 2,
    UvmEventFatalReasonInvalidFaultType   = 3,
    UvmEventFatalReasonOutOfMemory        = 4,
    UvmEventFatalReasonInternalError      = 5,

    // This value is reported when a fault is triggered in an invalid context
    // Example: CPU fault on a managed allocation while a kernel is running on a
    // pre-Pascal GPU
    UvmEventFatalReasonInvalidOperation   = 6,
    // ---- Add new values above this line
    UvmEventNumFatalReasons
} UvmEventFatalReason;

typedef enum
{
    UvmEventMigrationCauseInvalid        = 0,

    // The migration was initiated by the user via UvmMigrate/UvmMigrateAsync
    UvmEventMigrationCauseUser           = 1,

    // The UVM runtime initiated the migration to ensure that processors can
    // access data coherently
    UvmEventMigrationCauseCoherence      = 2,

    // Speculative migration of pages that are likely to be accessed in the
    // near future. Initiated by the UVM driver performance heuristics.
    UvmEventMigrationCausePrefetch       = 3,

    // Migration performed to evict memory from the GPU.
    UvmEventMigrationCauseEviction       = 4,

    // Migration of pages that are being accessed remotely by the GPU and
    // detected via access counter notifications.
    UvmEventMigrationCauseAccessCounters = 5,

    // ---- Add new values above this line
    UvmEventNumMigrationCauses
} UvmEventMigrationCause;

//------------------------------------------------------------------------------
// Information associated with a migration event UVM onwards
//------------------------------------------------------------------------------
typedef struct
{
    //
    // eventType has to be the 1st argument of this structure. Setting eventType
    // to UvmEventTypeMigration helps to identify event data in a queue.
    //
    NvU8 eventType;
    //
    // Cause that triggered the migration
    //
    NvU8 migrationCause;
    //
    // Indices are used for the source and destination of migration instead of
    // using gpu uuid/cpu id. This reduces the size of each event. The index to
    // gpuUuid relation can be obtained from UvmToolsGetProcessorUuidTable.
    // Currently we do not distinguish between CPUs so they all use index 0.
    //
    NvU8 srcIndex;                 // source CPU/GPU index
    NvU8 dstIndex;                 // destination CPU/GPU index
    //
    // This structure is shared between UVM kernel and tools.
    // Manually padding the structure so that compiler options like pragma pack
    // or malign-double will have no effect on the field offsets
    //
    NvU32 padding32Bits;
    NvU64 address;                 // base virtual addr used for migration
    NvU64 migratedBytes;           // number of bytes migrated
    NvU64 beginTimeStamp;          // cpu time stamp when the memory transfer
                                   // was queued on the gpu
    NvU64 endTimeStamp;            // cpu time stamp when the memory transfer
                                   // finalization was communicated to the cpu
                                   // For asynchronous operations this field
                                   // will be zero
    NvU64 rangeGroupId;            // range group tied with this migration
    NvU64 beginTimeStampGpu;       // time stamp when the migration started
                                   // on the gpu
    NvU64 endTimeStampGpu;         // time stamp when the migration finished
                                   // on the gpu
} UvmEventMigrationInfo_V1;

typedef struct
{
    //
    // eventType has to be the 1st argument of this structure. Setting eventType
    // to UvmEventTypeMigration helps to identify event data in a queue.
    //
    NvU8 eventType;
    //
    // Cause that triggered the migration
    //
    NvU8 migrationCause;
    //
    // This structure is shared between UVM kernel and tools.
    // Manually padding the structure so that compiler options like pragma pack
    // or malign-double will have no effect on the field offsets
    //
    NvU16 padding16Bits;
    //
    // Indices are used for the source and destination of migration instead of
    // using gpu uuid/cpu id. This reduces the size of each event. The index to
    // gpuUuid relation can be obtained from UvmToolsGetProcessorUuidTable.
    // Currently we do not distinguish between CPUs so they all use index 0.
    //
    NvU16 srcIndex;                // source CPU/GPU index
    NvU16 dstIndex;                // destination CPU/GPU index
    NvS32 srcNid;                  // source CPU NUMA node ID
    NvS32 dstNid;                  // destination CPU NUMA node ID
    NvU64 address;                 // base virtual addr used for migration
    NvU64 migratedBytes;           // number of bytes migrated
    NvU64 beginTimeStamp;          // cpu time stamp when the memory transfer
                                   // was queued on the gpu
    NvU64 endTimeStamp;            // cpu time stamp when the memory transfer
                                   // finalization was communicated to the cpu
                                   // For asynchronous operations this field
                                   // will be zero
    NvU64 rangeGroupId;            // range group tied with this migration
    NvU64 beginTimeStampGpu;       // time stamp when the migration started
                                   // on the gpu
    NvU64 endTimeStampGpu;         // time stamp when the migration finished
                                   // on the gpu
} UvmEventMigrationInfo_V2;

typedef enum
{
    UvmEventFaultClientTypeInvalid            = 0,
    UvmEventFaultClientTypeGpc                = 1,
    UvmEventFaultClientTypeHub                = 2,

    // ---- Add new values above this line
    UvmEventNumFaultClientTypes
} UvmEventFaultClientType;

//------------------------------------------------------------------------------
// This info is provided per gpu fault
// This event can be treated as a start event for gpu fault handling
//------------------------------------------------------------------------------
typedef struct
{
    //
    // eventType has to be the 1st argument of this structure.
    // Setting eventType = UvmEventTypeGpuFault helps to identify event data in
    // a queue.
    //
    NvU8 eventType;
    NvU8 faultType;       // type of gpu fault, refer UvmEventFaultType
    NvU8 accessType;      // memory access type, refer UvmEventMemoryAccessType
    NvU8 gpuIndex;        // GPU that experienced the fault
    union
    {
        NvU16 gpcId;      // If this is a replayable fault, this field contains
                          // the physical GPC index where the fault was
                          // triggered

        NvU16 channelId;  // If this is a non-replayable fault, this field
                          // contains the id of the channel that launched the
                          // operation that caused the fault.
                          //
                          // TODO: Bug 3283289: this field is ambiguous for
                          // Ampere+ GPUs, but it is never consumed by clients.
    };
    NvU16 clientId;       // Id of the MMU client that triggered the fault. This
                          // is the value provided by HW and is architecture-
                          // specific. There are separate client ids for
                          // different client types (See dev_fault.h).
    NvU64 address;        // virtual address at which gpu faulted
    NvU64 timeStamp;      // time stamp when the cpu started processing the
                          // fault
    NvU64 timeStampGpu;   // gpu time stamp when the fault entry was written
                          // in the fault buffer
    NvU32 batchId;        // Per-GPU unique id to identify the faults serviced
                          // in batch before:
                          // - Issuing a replay for replayable faults
                          // - Re-scheduling the channel for non-replayable
                          //   faults.
    NvU8 clientType;      // Volta+ GPUs can fault on clients other than GR.
                          // UvmEventFaultClientTypeGpc indicates replayable
                          // fault, while UvmEventFaultClientTypeHub indicates
                          // non-replayable fault.

    //
    // This structure is shared between UVM kernel and tools.
    // Manually padding the structure so that compiler options like pragma pack
    // or malign-double will have no effect on the field offsets
    //
    NvU8 padding8Bits;
    NvU16 padding16Bits;
} UvmEventGpuFaultInfo_V1;

typedef struct
{
    //
    // eventType has to be the 1st argument of this structure.
    // Setting eventType = UvmEventTypeGpuFault helps to identify event data in
    // a queue.
    //
    NvU8 eventType;
    NvU8 faultType;       // type of gpu fault, refer UvmEventFaultType
    NvU16 gpuIndex;       // GPU that experienced the fault
    union
    {
        NvU16 gpcId;      // If this is a replayable fault, this field contains
                          // the physical GPC index where the fault was
                          // triggered

        NvU16 channelId;  // If this is a non-replayable fault, this field
                          // contains the id of the channel that launched the
                          // operation that caused the fault.
                          //
                          // TODO: Bug 3283289: this field is ambiguous for
                          // Ampere+ GPUs, but it is never consumed by clients.
    };
    NvU16 clientId;       // Id of the MMU client that triggered the fault. This
                          // is the value provided by HW and is architecture-
                          // specific. There are separate client ids for
                          // different client types (See dev_fault.h).
    NvU64 address;        // virtual address at which gpu faulted
    NvU64 timeStamp;      // time stamp when the cpu started processing the
                          // fault
    NvU64 timeStampGpu;   // gpu time stamp when the fault entry was written
                          // in the fault buffer
    NvU32 batchId;        // Per-GPU unique id to identify the faults serviced
                          // in batch before:
                          // - Issuing a replay for replayable faults
                          // - Re-scheduling the channel for non-replayable
                          //   faults.
    NvU8 clientType;      // Volta+ GPUs can fault on clients other than GR.
                          // UvmEventFaultClientTypeGpc indicates replayable
                          // fault, while UvmEventFaultClientTypeHub indicates
                          // non-replayable fault.
    NvU8 accessType;      // memory access type, refer UvmEventMemoryAccessType
    //
    // This structure is shared between UVM kernel and tools.
    // Manually padding the structure so that compiler options like pragma pack
    // or malign-double will have no effect on the field offsets
    //
    NvU16 padding16bits;
} UvmEventGpuFaultInfo_V2;

//------------------------------------------------------------------------------
// This info is provided when a gpu fault is replayed (for replayable faults)
// or when the channel that launched the operation that triggered the fault is
// rescheduled for execution (for non-replayable faults).
//
// This event can be treated as an end event for gpu fault handling.
// Any other events eg migration events caused as a side-effect of the gpu fault
// would lie between the start and end event.
//------------------------------------------------------------------------------
typedef struct
{
    //
    // eventType has to be the 1st argument of this structure.
    // Setting eventType = UvmEventTypeGpuFaultReplay helps to identify event
    // data in a queue.
    //
    NvU8 eventType;
    NvU8 gpuIndex;          // GPU that experienced the fault
    NvU8 clientType;        // See clientType in UvmEventGpuFaultInfo
    //
    // This structure is shared between UVM kernel and tools.
    // Manually padding the structure so that compiler options like pragma pack
    // or malign-double will have no effect on the field offsets
    //
    NvU8 padding8bits;
    NvU32 batchId;          // Per-GPU unique id to identify the faults that
                            // have been serviced in batch
    NvU64 timeStamp;        // cpu time when the replay of the faulting memory
                            // accesses is queued on the gpu
    NvU64 timeStampGpu;     // gpu time stamp when the replay operation finished
                            // executing on the gpu
} UvmEventGpuFaultReplayInfo_V1;

typedef struct
{
    //
    // eventType has to be the 1st argument of this structure.
    // Setting eventType = UvmEventTypeGpuFaultReplay helps to identify event
    // data in a queue.
    //
    NvU8 eventType;
    NvU8 clientType;        // See clientType in UvmEventGpuFaultInfo
    NvU16 gpuIndex;         // GPU that experienced the fault
    NvU32 batchId;          // Per-GPU unique id to identify the faults that
                            // have been serviced in batch
    NvU64 timeStamp;        // cpu time when the replay of the faulting memory
                            // accesses is queued on the gpu
    NvU64 timeStampGpu;     // gpu time stamp when the replay operation finished
                            // executing on the gpu
} UvmEventGpuFaultReplayInfo_V2;

//------------------------------------------------------------------------------
// This info is provided per fatal fault
//------------------------------------------------------------------------------
typedef struct
{
    //
    // eventType has to be the 1st argument of this structure.
    // Setting eventType = UvmEventTypeFatalFault helps to identify event data
    // in a queue.
    //
    NvU8 eventType;
    NvU8 faultType;       // type of gpu fault, refer UvmEventFaultType. Only
                          // valid if processorIndex is a GPU
    NvU8 accessType;      // memory access type, refer UvmEventMemoryAccessType
    NvU8 processorIndex;  // processor that experienced the fault
    NvU8 reason;          // reason why the fault is fatal, refer
                          // UvmEventFatalReason
    NvU8 padding8bits;
    NvU16 padding16bits;
    NvU64 address;        // virtual address at which the processor faulted
    NvU64 timeStamp;      // CPU time when the fault is detected to be fatal
} UvmEventFatalFaultInfo_V1;

typedef struct
{
    //
    // eventType has to be the 1st argument of this structure.
    // Setting eventType = UvmEventTypeFatalFault helps to identify event data
    // in a queue.
    //
    NvU8 eventType;
    NvU8 faultType;       // type of gpu fault, refer UvmEventFaultType. Only
                          // valid if processorIndex is a GPU
    NvU8 accessType;      // memory access type, refer UvmEventMemoryAccessType
    NvU8 reason;          // reason why the fault is fatal, refer
                          // UvmEventFatalReason
    NvU16 processorIndex; // processor that experienced the fault
    NvU16 padding16bits;
    NvU64 address;        // virtual address at which the processor faulted
    NvU64 timeStamp;      // CPU time when the fault is detected to be fatal
} UvmEventFatalFaultInfo_V2;

typedef struct
{
    //
    // eventType has to be the 1st argument of this structure.
    // Setting eventType = UvmEventTypeReadDuplicate helps to identify event
    // data in a queue.
    //
    NvU8 eventType;
    //
    // This structure is shared between UVM kernel and tools.
    // Manually padding the structure so that compiler options like pragma pack
    // or malign-double will have no effect on the field offsets
    //
    NvU8  padding8bits;
    NvU16 padding16bits;
    NvU32 padding32bits;
    NvU64 processors;       // mask that specifies in which processors this
                            // memory region is read-duplicated
    NvU64 address;          // virtual address of the memory region that is
                            // read-duplicated
    NvU64 size;             // size in bytes of the memory region that is
                            // read-duplicated
    NvU64 timeStamp;        // cpu time stamp when the memory region becomes
                            // read-duplicate. Since many processors can
                            // participate in read-duplicate this is time stamp
                            // when all the operations have been pushed to all
                            // the processors.
} UvmEventReadDuplicateInfo_V1;

typedef struct
{
    //
    // eventType has to be the 1st argument of this structure.
    // Setting eventType = UvmEventTypeReadDuplicate helps to identify event
    // data in a queue.
    //
    NvU8 eventType;
    //
    // This structure is shared between UVM kernel and tools.
    // Manually padding the structure so that compiler options like pragma pack
    // or malign-double will have no effect on the field offsets
    //
    NvU8  padding8bits;
    NvU16 padding16bits;
    NvU32 padding32bits;
    NvU64 address;          // virtual address of the memory region that is
                            // read-duplicated
    NvU64 size;             // size in bytes of the memory region that is
                            // read-duplicated
    NvU64 timeStamp;        // cpu time stamp when the memory region becomes
                            // read-duplicate. Since many processors can
                            // participate in read-duplicate this is time stamp
                            // when all the operations have been pushed to all
                            // the processors.
    NvU64 processors[UVM_PROCESSOR_MASK_SIZE];
                            // mask that specifies in which processors this
                            // memory region is read-duplicated. This is last
                            // so UVM_PROCESSOR_MASK_SIZE can grow.
} UvmEventReadDuplicateInfo_V2;

typedef struct
{
    //
    // eventType has to be the 1st argument of this structure.
    // Setting eventType = UvmEventTypeReadDuplicateInvalidate helps to
    // identify event data in a queue.
    //
    NvU8 eventType;
    NvU8 residentIndex;     // index of the cpu/gpu that now contains the only
                            // valid copy of the memory region
    //
    // This structure is shared between UVM kernel and tools.
    // Manually padding the structure so that compiler options like pragma pack
    // or malign-double will have no effect on the field offsets
    //
    NvU16 padding16bits;
    NvU32 padding32bits;
    NvU64 address;          // virtual address of the memory region that is
                            // read-duplicated
    NvU64 size;             // size of the memory region that is
                            // read-duplicated
    NvU64 timeStamp;        // cpu time stamp when the memory region is no
                            // longer read-duplicate. Since many processors can
                            // participate in read-duplicate this is time stamp
                            // when all the operations have been pushed to all
                            // the processors.
} UvmEventReadDuplicateInvalidateInfo_V1;

typedef struct
{
    //
    // eventType has to be the 1st argument of this structure.
    // Setting eventType = UvmEventTypeReadDuplicateInvalidate helps to
    // identify event data in a queue.
    //
    NvU8 eventType;
    NvU8 padding8bits;
    NvU16 residentIndex;
    //
    // This structure is shared between UVM kernel and tools.
    // Manually padding the structure so that compiler options like pragma pack
    // or malign-double will have no effect on the field offsets
    //
    NvU32 padding32bits;
    NvU64 address;          // virtual address of the memory region that is
                            // read-duplicated
    NvU64 size;             // size of the memory region that is
                            // read-duplicated
    NvU64 timeStamp;        // cpu time stamp when the memory region is no
                            // longer read-duplicate. Since many processors can
                            // participate in read-duplicate this is time stamp
                            // when all the operations have been pushed to all
                            // the processors.
} UvmEventReadDuplicateInvalidateInfo_V2;

typedef struct
{
    //
    // eventType has to be the 1st argument of this structure.
    // Setting eventType = UvmEventTypePageSizeChange helps to identify event
    // data in a queue.
    //
    NvU8 eventType;
    //
    // This structure is shared between UVM kernel and tools.
    // Manually padding the structure so that compiler options like pragma pack
    // or malign-double will have no effect on the field offsets
    //
    NvU8 processorIndex;    // cpu/gpu processor index for which the page size
                            // changed
    NvU16 padding16bits;
    NvU32 size;             // new page size
    NvU64 address;          // virtual address of the page whose size has
                            // changed
    NvU64 timeStamp;        // cpu time stamp when the new page size is
                            // queued on the gpu
} UvmEventPageSizeChangeInfo_V1;

typedef struct
{
    //
    // eventType has to be the 1st argument of this structure.
    // Setting eventType = UvmEventTypePageSizeChange helps to identify event
    // data in a queue.
    //
    NvU8 eventType;
    //
    // This structure is shared between UVM kernel and tools.
    // Manually padding the structure so that compiler options like pragma pack
    // or malign-double will have no effect on the field offsets
    //
    NvU8 padding8bits;
    NvU16 processorIndex;   // cpu/gpu processor index for which the page size
                            // changed
    NvU32 size;             // new page size
    NvU64 address;          // virtual address of the page whose size has
                            // changed
    NvU64 timeStamp;        // cpu time stamp when the new page size is
                            // queued on the gpu
} UvmEventPageSizeChangeInfo_V2;

typedef struct
{
    //
    // eventType has to be the 1st argument of this structure.
    // Setting eventType = UvmEventTypeThrashingDetected helps to identify event
    // data in a queue.
    //
    NvU8 eventType;
    //
    // This structure is shared between UVM kernel and tools.
    // Manually padding the structure so that compiler options like pragma pack
    // or malign-double will have no effect on the field offsets
    //
    NvU8 padding8bits;
    NvU16 padding16bits;
    NvU32 padding32bits;
    NvU64 processors;       // mask that specifies which processors are
                            // fighting for this memory region
    NvU64 address;          // virtual address of the memory region that is
                            // thrashing
    NvU64 size;             // size of the memory region that is thrashing
    NvU64 timeStamp;        // cpu time stamp when thrashing is detected
} UvmEventThrashingDetectedInfo_V1;

typedef struct
{
    //
    // eventType has to be the 1st argument of this structure.
    // Setting eventType = UvmEventTypeThrashingDetected helps to identify event
    // data in a queue.
    //
    NvU8 eventType;
    //
    // This structure is shared between UVM kernel and tools.
    // Manually padding the structure so that compiler options like pragma pack
    // or malign-double will have no effect on the field offsets
    //
    NvU8 padding8bits;
    NvU16 padding16bits;
    NvU32 padding32bits;
    NvU64 address;          // virtual address of the memory region that is
                            // thrashing
    NvU64 size;             // size of the memory region that is thrashing
    NvU64 timeStamp;        // cpu time stamp when thrashing is detected
    NvU64 processors[UVM_PROCESSOR_MASK_SIZE];
                            // mask that specifies which processors are
                            // fighting for this memory region. This is last
                            // so UVM_PROCESSOR_MASK_SIZE can grow.
} UvmEventThrashingDetectedInfo_V2;

typedef struct
{
    //
    // eventType has to be the 1st argument of this structure.
    // Setting eventType = UvmEventTypeThrottlingStart helps to identify event
    // data in a queue.
    //
    NvU8 eventType;
    NvU8 processorIndex;    // index of the cpu/gpu that was throttled
    //
    // This structure is shared between UVM kernel and tools.
    // Manually padding the structure so that compiler options like pragma pack
    // or malign-double will have no effect on the field offsets
    //
    NvU16 padding16bits;
    NvU32 padding32bits;
    NvU64 address;          // address of the page whose servicing is being
                            // throttled
    NvU64 timeStamp;        // cpu start time stamp for the throttling operation
} UvmEventThrottlingStartInfo_V1;

typedef struct
{
    //
    // eventType has to be the 1st argument of this structure.
    // Setting eventType = UvmEventTypeThrottlingStart helps to identify event
    // data in a queue.
    //
    NvU8 eventType;
    //
    // This structure is shared between UVM kernel and tools.
    // Manually padding the structure so that compiler options like pragma pack
    // or malign-double will have no effect on the field offsets
    //
    NvU8  padding8bits;
    NvU16 processorIndex;   // index of the cpu/gpu that was throttled
    NvU32 padding32bits;
    NvU64 address;          // address of the page whose servicing is being
                            // throttled
    NvU64 timeStamp;        // cpu start time stamp for the throttling operation
} UvmEventThrottlingStartInfo_V2;

typedef struct
{
    //
    // eventType has to be the 1st argument of this structure.
    // Setting eventType = UvmEventTypeThrottlingEnd helps to identify event
    // data in a queue.
    //
    NvU8 eventType;
    NvU8 processorIndex;    // index of the cpu/gpu that was throttled
    //
    // This structure is shared between UVM kernel and tools.
    // Manually padding the structure so that compiler options like pragma pack
    // or malign-double will have no effect on the field offsets
    //
    NvU16 padding16bits;
    NvU32 padding32bits;
    NvU64 address;          // address of the page whose servicing is being
                            // throttled
    NvU64 timeStamp;        // cpu end time stamp for the throttling operation
} UvmEventThrottlingEndInfo_V1;

typedef struct
{
    //
    // eventType has to be the 1st argument of this structure.
    // Setting eventType = UvmEventTypeThrottlingEnd helps to identify event
    // data in a queue.
    //
    NvU8 eventType;
    //
    // This structure is shared between UVM kernel and tools.
    // Manually padding the structure so that compiler options like pragma pack
    // or malign-double will have no effect on the field offsets
    //
    NvU8  padding8bits;
    NvU16 processorIndex;   // index of the cpu/gpu that was throttled
    NvU32 padding32bits;
    NvU64 address;          // address of the page whose servicing is being
                            // throttled
    NvU64 timeStamp;        // cpu end time stamp for the throttling operation
} UvmEventThrottlingEndInfo_V2;

typedef enum
{
    UvmEventMapRemoteCauseInvalid     = 0,

    // The remote mapping is created to ensure coherence on systems with no
    // GPU fault support (UVM-Lite)
    UvmEventMapRemoteCauseCoherence   = 1,

    // The thrashing mitigation policy pinned a memory region on a specific
    // processor memory. This cause is used for the remote mappings created
    // on the rest of processors to the pinned location.
    UvmEventMapRemoteCauseThrashing   = 2,

    // The remote mapping was created to enforce the PreferredLocation or
    // AccessedBy hints provided by the user.
    UvmEventMapRemoteCausePolicy      = 3,

    // There is no available memory on the system so a remote mapping was
    // created to the current location.
    UvmEventMapRemoteCauseOutOfMemory = 4,

    // On GPUs with access counters, memory evicted to sysmem is always mapped
    // from the GPU. The UVM driver will invalidate the mapping if the region
    // is heavily accessed by the GPU later on.
    UvmEventMapRemoteCauseEviction    = 5,
} UvmEventMapRemoteCause;

typedef struct
{
    //
    // eventType has to be the 1st argument of this structure.
    // Setting eventType = UvmEventTypeMapRemote helps to identify event data
    // in a queue.
    //
    NvU8 eventType;
    NvU8 srcIndex;          // index of the cpu/gpu being remapped
    NvU8 dstIndex;          // index of the cpu/gpu memory that contains the
                            // memory region data
    NvU8 mapRemoteCause;    // field to type UvmEventMapRemoteCause that tells
                            // the cause for the page to be mapped remotely
    //
    // This structure is shared between UVM kernel and tools.
    // Manually padding the structure so that compiler options like pragma pack
    // or malign-double will have no effect on the field offsets
    //
    NvU32 padding32bits;
    NvU64 address;          // virtual address of the memory region that is
                            // thrashing
    NvU64 size;             // size of the memory region that is thrashing
    NvU64 timeStamp;        // cpu time stamp when all the required operations
                            // have been pushed to the processor
    NvU64 timeStampGpu;     // time stamp when the new mapping is effective in
                            // the processor specified by srcIndex. If srcIndex
                            // is a cpu, this field will be zero.
} UvmEventMapRemoteInfo_V1;

typedef struct
{
    //
    // eventType has to be the 1st argument of this structure.
    // Setting eventType = UvmEventTypeMapRemote helps to identify event data
    // in a queue.
    //
    NvU8 eventType;
    NvU8 mapRemoteCause;    // field to type UvmEventMapRemoteCause that tells
                            // the cause for the page to be mapped remotely
    //
    // This structure is shared between UVM kernel and tools.
    // Manually padding the structure so that compiler options like pragma pack
    // or malign-double will have no effect on the field offsets
    //
    NvU16 padding16bits;
    NvU16 srcIndex;         // index of the cpu/gpu being remapped
    NvU16 dstIndex;         // index of the cpu/gpu memory that contains the
                            // memory region data
    NvU64 address;          // virtual address of the memory region that is
                            // thrashing
    NvU64 size;             // size of the memory region that is thrashing
    NvU64 timeStamp;        // cpu time stamp when all the required operations
                            // have been pushed to the processor
    NvU64 timeStampGpu;     // time stamp when the new mapping is effective in
                            // the processor specified by srcIndex. If srcIndex
                            // is a cpu, this field will be zero.
} UvmEventMapRemoteInfo_V2;

typedef struct
{
    //
    // eventType has to be the 1st argument of this structure.
    // Setting eventType = UvmEventTypeEviction helps to identify event data
    // in a queue.
    //
    NvU8 eventType;
    NvU8 srcIndex;          // index of the cpu/gpu from which data is being
                            // evicted
    NvU8 dstIndex;          // index of the cpu/gpu memory to which data is
                            // going to be stored
    //
    // This structure is shared between UVM kernel and tools.
    // Manually padding the structure so that compiler options like pragma pack
    // or malign-double will have no effect on the field offsets
    //
    NvU8  padding8bits;
    NvU32 padding32bits;
    NvU64 addressOut;       // virtual address of the memory region that is
                            // being evicted
    NvU64 addressIn;        // virtual address that caused the eviction
    NvU64 size;             // size of the memory region that being evicted
    NvU64 timeStamp;        // cpu time stamp when eviction starts on the cpu
} UvmEventEvictionInfo_V1;

typedef struct
{
    //
    // eventType has to be the 1st argument of this structure.
    // Setting eventType = UvmEventTypeEviction helps to identify event data
    // in a queue.
    //
    NvU8 eventType;
    //
    // This structure is shared between UVM kernel and tools.
    // Manually padding the structure so that compiler options like pragma pack
    // or malign-double will have no effect on the field offsets
    //
    NvU8  padding8bits;
    NvU16 padding16bits;
    NvU16 srcIndex;         // index of the cpu/gpu from which data is being
                            // evicted
    NvU16 dstIndex;         // index of the cpu/gpu memory to which data is
                            // going to be stored
    NvU64 addressOut;       // virtual address of the memory region that is
                            // being evicted
    NvU64 addressIn;        // virtual address that caused the eviction
    NvU64 size;             // size of the memory region that being evicted
    NvU64 timeStamp;        // cpu time stamp when eviction starts on the cpu
} UvmEventEvictionInfo_V2;

// TODO: Bug 1870362: [uvm] Provide virtual address and processor index in
// AccessCounter events
//
// Currently we are just passing raw information from the notification buffer
// entries, which includes physical address + aperture. Instead, translate the
// information to something more useful such as virtual address and then index
// of the processor where the accessed data is resident. Most of the
// implementation is required to service access counter notifications
// themselves.
typedef enum
{
    UvmEventAperturePeer0   = 1,
    UvmEventAperturePeer1   = 2,
    UvmEventAperturePeer2   = 3,
    UvmEventAperturePeer3   = 4,
    UvmEventAperturePeer4   = 5,
    UvmEventAperturePeer5   = 6,
    UvmEventAperturePeer6   = 7,
    UvmEventAperturePeer7   = 8,
    UvmEventAperturePeerMax = UvmEventAperturePeer7,
    UvmEventApertureSys     = 9,
    UvmEventApertureVid     = 10,
} UvmEventApertureType;

typedef struct
{
    //
    // eventType has to be the 1st argument of this structure.
    // Setting eventType = UvmEventTypeAccessCounter helps to identify event
    // data in a queue.
    //
    NvU8 eventType;
    NvU8 srcIndex;          // index of the gpu that received the access counter
                            // notification
    //
    // This structure is shared between UVM kernel and tools.
    // Manually padding the structure so that compiler options like pragma pack
    // or malign-double will have no effect on the field offsets
    //
    // See uvm_access_counter_buffer_entry_t for details
    NvU8 aperture;
    NvU8 instancePtrAperture;

    NvU8 isVirtual;
    NvU8 isFromCpu;

    NvU8 veId;

    // The physical access counter notification was triggered on a managed
    // memory region. This is not set for virtual access counter notifications.
    NvU8 physOnManaged;

    NvU32 value;
    NvU32 subGranularity;
    NvU32 tag;
    NvU32 bank;
    NvU64 address;
    NvU64 instancePtr;
} UvmEventTestAccessCounterInfo_V1;

typedef struct
{
    //
    // eventType has to be the 1st argument of this structure.
    // Setting eventType = UvmEventTypeAccessCounter helps to identify event
    // data in a queue.
    //
    NvU8 eventType;
    // See uvm_access_counter_buffer_entry_t for details
    NvU8 aperture;
    NvU8 instancePtrAperture;
    NvU8 isVirtual;
    NvU8 isFromCpu;
    NvU8 veId;

    // The physical access counter notification was triggered on a managed
    // memory region. This is not set for virtual access counter notifications.
    NvU8 physOnManaged;

    //
    // This structure is shared between UVM kernel and tools.
    // Manually padding the structure so that compiler options like pragma pack
    // or malign-double will have no effect on the field offsets
    //
    NvU8  padding8bits;
    NvU16 srcIndex;         // index of the gpu that received the access counter
                            // notification
    NvU16 padding16bits;
    NvU32 value;
    NvU32 subGranularity;
    NvU32 tag;
    NvU32 bank;
    NvU32 padding32bits;
    NvU64 address;
    NvU64 instancePtr;
} UvmEventTestAccessCounterInfo_V2;

typedef struct
{
    NvU8 eventType;
} UvmEventTestSplitInvalidateInfo;

//------------------------------------------------------------------------------
// Entry added in the event queue buffer when an enabled event occurs. For
// compatibility with all tools ensure that this structure is 64 bit aligned.
//------------------------------------------------------------------------------
typedef struct
{
    union
    {
        union
        {
            NvU8 eventType;
            UvmEventMigrationInfo_Lite migration_Lite;

            UvmEventCpuFaultInfo cpuFault;
            UvmEventMigrationInfo_V1 migration;
            UvmEventGpuFaultInfo_V1 gpuFault;
            UvmEventGpuFaultReplayInfo_V1 gpuFaultReplay;
            UvmEventFatalFaultInfo_V1 fatalFault;
            UvmEventReadDuplicateInfo_V1 readDuplicate;
            UvmEventReadDuplicateInvalidateInfo_V1 readDuplicateInvalidate;
            UvmEventPageSizeChangeInfo_V1 pageSizeChange;
            UvmEventThrashingDetectedInfo_V1 thrashing;
            UvmEventThrottlingStartInfo_V1 throttlingStart;
            UvmEventThrottlingEndInfo_V1 throttlingEnd;
            UvmEventMapRemoteInfo_V1 mapRemote;
            UvmEventEvictionInfo_V1 eviction;
        } eventData;

        union
        {
            NvU8 eventType;

            UvmEventTestAccessCounterInfo_V1 accessCounter;
            UvmEventTestSplitInvalidateInfo splitInvalidate;
        } testEventData;
    };
} UvmEventEntry_V1;

typedef struct
{
    union
    {
        union
        {
            NvU8 eventType;
            UvmEventMigrationInfo_Lite migration_Lite;

            UvmEventCpuFaultInfo cpuFault;
            UvmEventMigrationInfo_V2 migration;
            UvmEventGpuFaultInfo_V2 gpuFault;
            UvmEventGpuFaultReplayInfo_V2 gpuFaultReplay;
            UvmEventFatalFaultInfo_V2 fatalFault;
            UvmEventReadDuplicateInfo_V2 readDuplicate;
            UvmEventReadDuplicateInvalidateInfo_V2 readDuplicateInvalidate;
            UvmEventPageSizeChangeInfo_V2 pageSizeChange;
            UvmEventThrashingDetectedInfo_V2 thrashing;
            UvmEventThrottlingStartInfo_V2 throttlingStart;
            UvmEventThrottlingEndInfo_V2 throttlingEnd;
            UvmEventMapRemoteInfo_V2 mapRemote;
            UvmEventEvictionInfo_V2 eviction;
        } eventData;

        union
        {
            NvU8 eventType;

            UvmEventTestAccessCounterInfo_V2 accessCounter;
            UvmEventTestSplitInvalidateInfo splitInvalidate;
        } testEventData;
    };
} UvmEventEntry_V2;

//------------------------------------------------------------------------------
// Type of time stamp used in the event entry:
//
// On windows we support QPC type which uses RDTSC if possible else fallbacks to
// HPET.
//
// On Linux ClockGetTime provides similar functionality.
// In UvmEventTimeStampTypeAuto the system decides which time stamp best suites
// current environment.
//------------------------------------------------------------------------------
typedef enum
{
    UvmEventTimeStampTypeInvalid = 0,
    UvmEventTimeStampTypeWin32QPC = 1,
    UvmEventTimeStampTypePosixClockGetTime = 2,
    UvmEventTimeStampTypeAuto = 3,
    // ---- Add new values above this line
    UvmEventNumTimeStampTypes
} UvmEventTimeStampType;

//------------------------------------------------------------------------------
// An opaque queue handle is returned to the user when a queue is created.
//------------------------------------------------------------------------------
typedef NvUPtr UvmEventQueueHandle;

//------------------------------------------------------------------------------
// Setting default page size to 4k,
// this can be updated to 64k in case of power PC
//------------------------------------------------------------------------------
#define UVM_DEBUG_ACCESS_PAGE_SIZE      (1 << 12) // 4k page

typedef enum
{
    UvmDebugAccessTypeRead = 0,
    UvmDebugAccessTypeWrite = 1,
} UvmDebugAccessType;

typedef enum {
    UvmToolsEventQueueVersion_V1 = 1,
    UvmToolsEventQueueVersion_V2 = 2,
} UvmToolsEventQueueVersion;

typedef struct UvmEventControlData_tag {
    // entries between get_ahead and get_behind are currently being read
    volatile NvU32 get_ahead;
    volatile NvU32 get_behind;

    // entries between put_ahead and put_behind are currently being written
    volatile NvU32 put_ahead;
    volatile NvU32 put_behind;

    // counter of dropped events
    NvU64 dropped[UvmEventNumTypesAll];
} UvmToolsEventControlData;

// TODO: Bug 4465348: remove this after replacing old references.
typedef UvmToolsEventControlData UvmToolsEventControlData_V1;

//------------------------------------------------------------------------------
// UVM Tools forward types (handles) definitions
//------------------------------------------------------------------------------
struct UvmToolsSession_tag;
struct UvmToolsEventQueue_tag;
struct UvmToolsCounters_tag;

typedef struct UvmToolsSession_tag UvmToolsSession;
typedef struct UvmToolsEventQueue_tag UvmToolsEventQueue;
typedef struct UvmToolsCounters_tag UvmToolsCounters;

typedef UvmToolsSession *UvmToolsSessionHandle;
typedef UvmToolsEventQueue *UvmToolsEventQueueHandle;
typedef UvmToolsCounters *UvmToolsCountersHandle;

#endif // _UVM_TYPES_H_
