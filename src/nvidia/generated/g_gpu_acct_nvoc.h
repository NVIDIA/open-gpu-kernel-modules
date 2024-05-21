
#ifndef _G_GPU_ACCT_NVOC_H_
#define _G_GPU_ACCT_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2013-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_gpu_acct_nvoc.h"

#ifndef _GPU_ACCT_H_
#define _GPU_ACCT_H_

#include "core/system.h"
#include "containers/map.h"
#include "containers/list.h"
#include "virtualization/common_vgpu_mgr.h"
#include "ctrl/ctrl0000/ctrl0000gpuacct.h"
#include "ctrl/ctrl0000/ctrl0000gpu.h" // NV0000_CTRL_GPU_MAX_ATTACHED_GPUS
#include "ctrl/ctrl2080/ctrl2080perf.h" // NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2_PARAMS


typedef struct TMR_EVENT TMR_EVENT;

// Sum of NV_MAX_LIVE_ACCT_PROCESS and NV_MAX_DEAD_ACCT_PROCESS is defined as
// NV0000_GPUACCT_PID_MAX_COUNT, which is equal to the max number of processes
// that can be returned by NV0000_CTRL_CMD_GPUACCT_GET_ACCOUNTING_PIDS RM
// control call.
// A VM/host can have a max of 4k compute/graphics processes, so the
// NV0000_GPUACCT_PID_MAX_COUNT limit for max no processes returned by
// NV0000_CTRL_CMD_GPUACCT_GET_ACCOUNTING_PIDS RM control is sufficient.
#define NV_MAX_LIVE_ACCT_PROCESS     3872
#define NV_MAX_DEAD_ACCT_PROCESS     128

#define NV_GPUACCT_PROC_TYPE_CPU     0
#define NV_GPUACCT_PROC_TYPE_GPU     1

#define NV_INVALID_VM_INDEX          0xFFFFFFFF
#define NV_INVALID_VM_PID            0xFFFFFFFF

#define IS_VALID_SUBPID(x) (((x) > 0) && ((x) != 0xFFFFFFFF))

typedef struct
{
    NvU32  procId;      // Pid of the process.
    NvU32  procType;    // Type of the process.
    NvU32  gpuUtil;     // Process's average GR engine utilization.
    NvU64  sumUtil;     // Running sum of process's GR engine utilization.
    NvU32  fbUtil;      // Process's average FB bandwidth utilization.
    NvU64  sumFbUtil;   // Running sum of process's FB bandwidth utilization.
    NvU32  startSampleCount;   // At process start, this variable is set to
                               // the current total sample count of GPU.
    NvU32  totalSampleCount;   // At process stop, this variable is set to
                               // difference of
                               // (Current total sample count - Start sample count).
    NvU64  maxFbUsage;  // Process's high watermark FB allocated (in bytes).
    NvU64  startTime;   // Time when accounting is started for the process.
    NvU64  endTime;     // Time when accounting is stopped for the process.
    NvU32  refCount;    // Count to keep track of accounting requests.

    // Following members are only used on Grid host.
    NvU32  isGuestProcess; // Set if the entry corresponds to a guest VM process.

    MapNode  mapNode;   // Node in the Map.
    ListNode listNode;  // Node in the list.
} GPUACCT_PROC_ENTRY;

MAKE_INTRUSIVE_MAP (GPU_ACCT_PROC_MAP,  GPUACCT_PROC_ENTRY, mapNode);
MAKE_INTRUSIVE_LIST(GPU_ACCT_PROC_LIST, GPUACCT_PROC_ENTRY, listNode);

typedef struct
{
    GPU_ACCT_PROC_MAP  procMap;
    GPU_ACCT_PROC_LIST procList;
} GPU_ACCT_PROC_DATA_STORE;

typedef struct
{
    NvU32             vmPId;              // Plugin/VM process id.
    NvU32             isAccountingEnabled;// Whether accounting is enabled on
                                          // this VM.
    GPU_ACCT_PROC_DATA_STORE  liveVMProcAcctInfo; // Pointer to list of live processes
                                                  // running on this VM.
    GPU_ACCT_PROC_DATA_STORE  deadVMProcAcctInfo; // Pointer to list of dead processes
                                                  // running on this VM.
} GPUACCT_VM_INSTANCE_INFO;

typedef struct
{
    TMR_EVENT        *pTmrEvent;                // Pointer to the timer event created to schedule main callback
    NvU64             lastUpdateTimestamp;      // Time stamp of last PMU sample set.
    NvU32             totalSampleCount;         // Total samples of GPU of since accounting started for this GPU.

    // Pre-allocated buffer for making ctrl calls in callbacks
    NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2_PARAMS *pSamplesParams;

    GPU_ACCT_PROC_DATA_STORE  liveProcAcctInfo; // Pointer to list of live processes
                                                // running on this GPU.
    GPU_ACCT_PROC_DATA_STORE  deadProcAcctInfo; // Pointer to list of dead processes
                                                // running on this GPU.
    GPUACCT_VM_INSTANCE_INFO  vmInstanceInfo[MAX_VGPU_DEVICES_PER_PGPU];
} GPUACCT_GPU_INSTANCE_INFO;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GPU_ACCT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct GpuAccounting {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct GpuAccounting *__nvoc_pbase_GpuAccounting;    // gpuacct

    // Data members
    GPUACCT_GPU_INSTANCE_INFO gpuInstanceInfo[32];
};

#ifndef __NVOC_CLASS_GpuAccounting_TYPEDEF__
#define __NVOC_CLASS_GpuAccounting_TYPEDEF__
typedef struct GpuAccounting GpuAccounting;
#endif /* __NVOC_CLASS_GpuAccounting_TYPEDEF__ */

#ifndef __nvoc_class_id_GpuAccounting
#define __nvoc_class_id_GpuAccounting 0x0f1350
#endif /* __nvoc_class_id_GpuAccounting */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuAccounting;

#define __staticCast_GpuAccounting(pThis) \
    ((pThis)->__nvoc_pbase_GpuAccounting)

#ifdef __nvoc_gpu_acct_h_disabled
#define __dynamicCast_GpuAccounting(pThis) ((GpuAccounting*)NULL)
#else //__nvoc_gpu_acct_h_disabled
#define __dynamicCast_GpuAccounting(pThis) \
    ((GpuAccounting*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GpuAccounting)))
#endif //__nvoc_gpu_acct_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GpuAccounting(GpuAccounting**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GpuAccounting(GpuAccounting**, Dynamic*, NvU32);
#define __objCreate_GpuAccounting(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_GpuAccounting((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
NV_STATUS gpuacctConstruct_IMPL(struct GpuAccounting *arg_);

#define __nvoc_gpuacctConstruct(arg_) gpuacctConstruct_IMPL(arg_)
void gpuacctDestruct_IMPL(struct GpuAccounting *arg1);

#define __nvoc_gpuacctDestruct(arg1) gpuacctDestruct_IMPL(arg1)
NV_STATUS gpuacctGetAccountingMode_IMPL(struct GpuAccounting *arg1, NvU32 arg2, NV0000_CTRL_GPUACCT_GET_ACCOUNTING_STATE_PARAMS *arg3);

#ifdef __nvoc_gpu_acct_h_disabled
static inline NV_STATUS gpuacctGetAccountingMode(struct GpuAccounting *arg1, NvU32 arg2, NV0000_CTRL_GPUACCT_GET_ACCOUNTING_STATE_PARAMS *arg3) {
    NV_ASSERT_FAILED_PRECOMP("GpuAccounting was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_acct_h_disabled
#define gpuacctGetAccountingMode(arg1, arg2, arg3) gpuacctGetAccountingMode_IMPL(arg1, arg2, arg3)
#endif //__nvoc_gpu_acct_h_disabled

NV_STATUS gpuacctEnableAccounting_IMPL(struct GpuAccounting *arg1, NvU32 arg2, NV0000_CTRL_GPUACCT_SET_ACCOUNTING_STATE_PARAMS *arg3);

#ifdef __nvoc_gpu_acct_h_disabled
static inline NV_STATUS gpuacctEnableAccounting(struct GpuAccounting *arg1, NvU32 arg2, NV0000_CTRL_GPUACCT_SET_ACCOUNTING_STATE_PARAMS *arg3) {
    NV_ASSERT_FAILED_PRECOMP("GpuAccounting was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_acct_h_disabled
#define gpuacctEnableAccounting(arg1, arg2, arg3) gpuacctEnableAccounting_IMPL(arg1, arg2, arg3)
#endif //__nvoc_gpu_acct_h_disabled

NV_STATUS gpuacctDisableAccounting_IMPL(struct GpuAccounting *arg1, NvU32 arg2, NV0000_CTRL_GPUACCT_SET_ACCOUNTING_STATE_PARAMS *arg3);

#ifdef __nvoc_gpu_acct_h_disabled
static inline NV_STATUS gpuacctDisableAccounting(struct GpuAccounting *arg1, NvU32 arg2, NV0000_CTRL_GPUACCT_SET_ACCOUNTING_STATE_PARAMS *arg3) {
    NV_ASSERT_FAILED_PRECOMP("GpuAccounting was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_acct_h_disabled
#define gpuacctDisableAccounting(arg1, arg2, arg3) gpuacctDisableAccounting_IMPL(arg1, arg2, arg3)
#endif //__nvoc_gpu_acct_h_disabled

NV_STATUS gpuacctClearAccountingData_IMPL(struct GpuAccounting *arg1, NvU32 arg2, NV0000_CTRL_GPUACCT_CLEAR_ACCOUNTING_DATA_PARAMS *arg3);

#ifdef __nvoc_gpu_acct_h_disabled
static inline NV_STATUS gpuacctClearAccountingData(struct GpuAccounting *arg1, NvU32 arg2, NV0000_CTRL_GPUACCT_CLEAR_ACCOUNTING_DATA_PARAMS *arg3) {
    NV_ASSERT_FAILED_PRECOMP("GpuAccounting was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_acct_h_disabled
#define gpuacctClearAccountingData(arg1, arg2, arg3) gpuacctClearAccountingData_IMPL(arg1, arg2, arg3)
#endif //__nvoc_gpu_acct_h_disabled

NV_STATUS gpuacctStartGpuAccounting_IMPL(struct GpuAccounting *arg1, NvU32 arg2, NvU32 arg3, NvU32 arg4);

#ifdef __nvoc_gpu_acct_h_disabled
static inline NV_STATUS gpuacctStartGpuAccounting(struct GpuAccounting *arg1, NvU32 arg2, NvU32 arg3, NvU32 arg4) {
    NV_ASSERT_FAILED_PRECOMP("GpuAccounting was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_acct_h_disabled
#define gpuacctStartGpuAccounting(arg1, arg2, arg3, arg4) gpuacctStartGpuAccounting_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_gpu_acct_h_disabled

NV_STATUS gpuacctStopGpuAccounting_IMPL(struct GpuAccounting *arg1, NvU32 arg2, NvU32 arg3, NvU32 arg4);

#ifdef __nvoc_gpu_acct_h_disabled
static inline NV_STATUS gpuacctStopGpuAccounting(struct GpuAccounting *arg1, NvU32 arg2, NvU32 arg3, NvU32 arg4) {
    NV_ASSERT_FAILED_PRECOMP("GpuAccounting was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_acct_h_disabled
#define gpuacctStopGpuAccounting(arg1, arg2, arg3, arg4) gpuacctStopGpuAccounting_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_gpu_acct_h_disabled

NV_STATUS gpuacctUpdateProcPeakFbUsage_IMPL(struct GpuAccounting *arg1, NvU32 arg2, NvU32 arg3, NvU32 arg4, NvU64 arg5);

#ifdef __nvoc_gpu_acct_h_disabled
static inline NV_STATUS gpuacctUpdateProcPeakFbUsage(struct GpuAccounting *arg1, NvU32 arg2, NvU32 arg3, NvU32 arg4, NvU64 arg5) {
    NV_ASSERT_FAILED_PRECOMP("GpuAccounting was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_acct_h_disabled
#define gpuacctUpdateProcPeakFbUsage(arg1, arg2, arg3, arg4, arg5) gpuacctUpdateProcPeakFbUsage_IMPL(arg1, arg2, arg3, arg4, arg5)
#endif //__nvoc_gpu_acct_h_disabled

NV_STATUS gpuacctGetProcAcctInfo_IMPL(struct GpuAccounting *arg1, NV0000_CTRL_GPUACCT_GET_PROC_ACCOUNTING_INFO_PARAMS *arg2);

#ifdef __nvoc_gpu_acct_h_disabled
static inline NV_STATUS gpuacctGetProcAcctInfo(struct GpuAccounting *arg1, NV0000_CTRL_GPUACCT_GET_PROC_ACCOUNTING_INFO_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("GpuAccounting was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_acct_h_disabled
#define gpuacctGetProcAcctInfo(arg1, arg2) gpuacctGetProcAcctInfo_IMPL(arg1, arg2)
#endif //__nvoc_gpu_acct_h_disabled

NV_STATUS gpuacctGetAcctPids_IMPL(struct GpuAccounting *arg1, NV0000_CTRL_GPUACCT_GET_ACCOUNTING_PIDS_PARAMS *arg2);

#ifdef __nvoc_gpu_acct_h_disabled
static inline NV_STATUS gpuacctGetAcctPids(struct GpuAccounting *arg1, NV0000_CTRL_GPUACCT_GET_ACCOUNTING_PIDS_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("GpuAccounting was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_acct_h_disabled
#define gpuacctGetAcctPids(arg1, arg2) gpuacctGetAcctPids_IMPL(arg1, arg2)
#endif //__nvoc_gpu_acct_h_disabled

NV_STATUS gpuacctSetProcType_IMPL(struct GpuAccounting *arg1, NvU32 arg2, NvU32 arg3, NvU32 arg4, NvU32 arg5);

#ifdef __nvoc_gpu_acct_h_disabled
static inline NV_STATUS gpuacctSetProcType(struct GpuAccounting *arg1, NvU32 arg2, NvU32 arg3, NvU32 arg4, NvU32 arg5) {
    NV_ASSERT_FAILED_PRECOMP("GpuAccounting was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_acct_h_disabled
#define gpuacctSetProcType(arg1, arg2, arg3, arg4, arg5) gpuacctSetProcType_IMPL(arg1, arg2, arg3, arg4, arg5)
#endif //__nvoc_gpu_acct_h_disabled

#undef PRIVATE_FIELD


void vmAcctInitState(struct OBJGPU *pGpu, NvU32 vmPid);
void vmAcctDestructState(NvU32 vmPid, struct OBJGPU *pGpu);
void gpuacctProcessGpuUtil(GPUACCT_GPU_INSTANCE_INFO *, NV2080_CTRL_PERF_GPUMON_PERFMON_UTIL_SAMPLE *);

#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_GPU_ACCT_NVOC_H_
