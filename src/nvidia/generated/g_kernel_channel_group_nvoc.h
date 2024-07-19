
#ifndef _G_KERNEL_CHANNEL_GROUP_NVOC_H_
#define _G_KERNEL_CHANNEL_GROUP_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_kernel_channel_group_nvoc.h"

#ifndef KERNEL_CHANNEL_GROUP_H
#define KERNEL_CHANNEL_GROUP_H 1

#include "kernel/mem_mgr/vaspace.h"

#include "ctrl/ctrl0080/ctrl0080gr.h" // NV03_DEVICE

#include "libraries/containers/btree.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "nvoc/prelude.h"
#include "resserv/resserv.h"
#include "gpu/gpu_resource.h"

#include "kernel/gpu/fifo/kernel_channel.h"

#include "kernel/gpu/fifo/kernel_ctxshare.h"

// Forward declaration

struct KernelChannelGroupApi;

#ifndef __NVOC_CLASS_KernelChannelGroupApi_TYPEDEF__
#define __NVOC_CLASS_KernelChannelGroupApi_TYPEDEF__
typedef struct KernelChannelGroupApi KernelChannelGroupApi;
#endif /* __NVOC_CLASS_KernelChannelGroupApi_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelChannelGroupApi
#define __nvoc_class_id_KernelChannelGroupApi 0x2b5b80
#endif /* __nvoc_class_id_KernelChannelGroupApi */



/*!
 * Stores the list of all client kctxshareApi objects
 * that reference the same shared kctxshare object
 */
MAKE_LIST(KernelChannelGroupApiList, KernelChannelGroupApi *);

typedef enum
{
    CHANNELGROUP_STATE_ENABLE,
    CHANNELGROUP_STATE_SCHED,
    CHANNELGROUP_STATE_COUNT // Should be last
} CHANNELGROUP_STATE;

//
// Describes the engine context memory for a channel
// (Stored in KernelChannelGroup because it's shared by all channels in the group)
//
typedef struct ENGINE_CTX_DESCRIPTOR
{
    MEMORY_DESCRIPTOR *pMemDesc; // Context memory
    VA_LIST vaList;              // Map to track the gpu va mapping to the context buffer
    NvU32 engDesc;               // Which engine type
} ENGINE_CTX_DESCRIPTOR;


//
// HW method buffer used by supporting engines to save/restore
// faulting methods after corresponding fault is handled.
//
typedef struct _HW_ENG_FAULT_METHOD_BUFFER
{
    NvU64              bar2Addr;
    MEMORY_DESCRIPTOR *pMemDesc;
} HW_ENG_FAULT_METHOD_BUFFER;

//
// dword array size used to track the valid subcontext mask.
// We use 1 bit per subcontext; so need 2 dwords to store the valid bitmask.
//
#define SUBCTX_MASK_ARRAY_SIZE 2

/**
 * This class represents data that is shared when a TSG is duped.
 *
 * Instances of this class are ref-counted and will be kept alive until
 * all TSG copies have been freed.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_CHANNEL_GROUP_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct KernelChannelGroup {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct RsShared __nvoc_base_RsShared;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct RsShared *__nvoc_pbase_RsShared;    // shr super
    struct KernelChannelGroup *__nvoc_pbase_KernelChannelGroup;    // kchangrp

    // Data members
    NODE node;
    NvU32 grpID;
    NvU32 runlistId;
    NvU32 chanCount;
    RM_ENGINE_TYPE engineType;
    struct OBJVASPACE *pVAS;
    NvU32 gfid;
    OBJEHEAP *pSubctxIdHeap;
    CHANNEL_LIST *pChanList;
    NvU64 timesliceUs;
    ENGINE_CTX_DESCRIPTOR **ppEngCtxDesc;
    NvBool bAllocatedByRm;
    NvBool bLegacyMode;
    HW_ENG_FAULT_METHOD_BUFFER *pMthdBuffers;
    NvU32 (*ppSubctxMask)[2];
    NvU32 (*ppZombieSubctxMask)[2];
    NvU32 *pStateMask;
    NvU32 *pInterleaveLevel;
    NvBool bRunlistAssigned;
    struct CTX_BUF_POOL_INFO *pCtxBufPool;
    struct CTX_BUF_POOL_INFO *pChannelBufPool;
    struct MapNode mapNode;
    KernelChannelGroupApiList apiObjList;
    NvBool bIsCallingContextVgpuPlugin;
};

#ifndef __NVOC_CLASS_KernelChannelGroup_TYPEDEF__
#define __NVOC_CLASS_KernelChannelGroup_TYPEDEF__
typedef struct KernelChannelGroup KernelChannelGroup;
#endif /* __NVOC_CLASS_KernelChannelGroup_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelChannelGroup
#define __nvoc_class_id_KernelChannelGroup 0xec6de1
#endif /* __nvoc_class_id_KernelChannelGroup */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelChannelGroup;

#define __staticCast_KernelChannelGroup(pThis) \
    ((pThis)->__nvoc_pbase_KernelChannelGroup)

#ifdef __nvoc_kernel_channel_group_h_disabled
#define __dynamicCast_KernelChannelGroup(pThis) ((KernelChannelGroup*)NULL)
#else //__nvoc_kernel_channel_group_h_disabled
#define __dynamicCast_KernelChannelGroup(pThis) \
    ((KernelChannelGroup*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelChannelGroup)))
#endif //__nvoc_kernel_channel_group_h_disabled

NV_STATUS __nvoc_objCreateDynamic_KernelChannelGroup(KernelChannelGroup**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelChannelGroup(KernelChannelGroup**, Dynamic*, NvU32);
#define __objCreate_KernelChannelGroup(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelChannelGroup((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
static inline NV_STATUS kchangrpSetInterleaveLevelSched_56cd7a(struct OBJGPU *pGpu, struct KernelChannelGroup *pKernelChannelGroup, NvU32 value) {
    return NV_OK;
}

NV_STATUS kchangrpSetInterleaveLevelSched_GM107(struct OBJGPU *pGpu, struct KernelChannelGroup *pKernelChannelGroup, NvU32 value);


#ifdef __nvoc_kernel_channel_group_h_disabled
static inline NV_STATUS kchangrpSetInterleaveLevelSched(struct OBJGPU *pGpu, struct KernelChannelGroup *pKernelChannelGroup, NvU32 value) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroup was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_group_h_disabled
#define kchangrpSetInterleaveLevelSched(pGpu, pKernelChannelGroup, value) kchangrpSetInterleaveLevelSched_56cd7a(pGpu, pKernelChannelGroup, value)
#endif //__nvoc_kernel_channel_group_h_disabled

#define kchangrpSetInterleaveLevelSched_HAL(pGpu, pKernelChannelGroup, value) kchangrpSetInterleaveLevelSched(pGpu, pKernelChannelGroup, value)

static inline void kchangrpUpdateSubcontextMask_b3696a(struct OBJGPU *pGpu, struct KernelChannelGroup *arg2, NvU32 arg3, NvBool arg4) {
    return;
}

void kchangrpUpdateSubcontextMask_GV100(struct OBJGPU *pGpu, struct KernelChannelGroup *arg2, NvU32 arg3, NvBool arg4);


#ifdef __nvoc_kernel_channel_group_h_disabled
static inline void kchangrpUpdateSubcontextMask(struct OBJGPU *pGpu, struct KernelChannelGroup *arg2, NvU32 arg3, NvBool arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroup was disabled!");
}
#else //__nvoc_kernel_channel_group_h_disabled
#define kchangrpUpdateSubcontextMask(pGpu, arg2, arg3, arg4) kchangrpUpdateSubcontextMask_b3696a(pGpu, arg2, arg3, arg4)
#endif //__nvoc_kernel_channel_group_h_disabled

#define kchangrpUpdateSubcontextMask_HAL(pGpu, arg2, arg3, arg4) kchangrpUpdateSubcontextMask(pGpu, arg2, arg3, arg4)

static inline void kchangrpSetSubcontextZombieState_b3696a(struct OBJGPU *pGpu, struct KernelChannelGroup *arg2, NvU32 arg3, NvBool arg4) {
    return;
}


#ifdef __nvoc_kernel_channel_group_h_disabled
static inline void kchangrpSetSubcontextZombieState(struct OBJGPU *pGpu, struct KernelChannelGroup *arg2, NvU32 arg3, NvBool arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroup was disabled!");
}
#else //__nvoc_kernel_channel_group_h_disabled
#define kchangrpSetSubcontextZombieState(pGpu, arg2, arg3, arg4) kchangrpSetSubcontextZombieState_b3696a(pGpu, arg2, arg3, arg4)
#endif //__nvoc_kernel_channel_group_h_disabled

#define kchangrpSetSubcontextZombieState_HAL(pGpu, arg2, arg3, arg4) kchangrpSetSubcontextZombieState(pGpu, arg2, arg3, arg4)

static inline NvBool kchangrpGetSubcontextZombieState_ceaee8(struct OBJGPU *pGpu, struct KernelChannelGroup *arg2, NvU32 arg3) {
    NV_ASSERT_PRECOMP(0);
    return ((NvBool)(0 != 0));
}


#ifdef __nvoc_kernel_channel_group_h_disabled
static inline NvBool kchangrpGetSubcontextZombieState(struct OBJGPU *pGpu, struct KernelChannelGroup *arg2, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroup was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_channel_group_h_disabled
#define kchangrpGetSubcontextZombieState(pGpu, arg2, arg3) kchangrpGetSubcontextZombieState_ceaee8(pGpu, arg2, arg3)
#endif //__nvoc_kernel_channel_group_h_disabled

#define kchangrpGetSubcontextZombieState_HAL(pGpu, arg2, arg3) kchangrpGetSubcontextZombieState(pGpu, arg2, arg3)

NV_STATUS kchangrpAllocFaultMethodBuffers_GV100(struct OBJGPU *pGpu, struct KernelChannelGroup *pKernelChannelGroup);


#ifdef __nvoc_kernel_channel_group_h_disabled
static inline NV_STATUS kchangrpAllocFaultMethodBuffers(struct OBJGPU *pGpu, struct KernelChannelGroup *pKernelChannelGroup) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroup was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_group_h_disabled
#define kchangrpAllocFaultMethodBuffers(pGpu, pKernelChannelGroup) kchangrpAllocFaultMethodBuffers_GV100(pGpu, pKernelChannelGroup)
#endif //__nvoc_kernel_channel_group_h_disabled

#define kchangrpAllocFaultMethodBuffers_HAL(pGpu, pKernelChannelGroup) kchangrpAllocFaultMethodBuffers(pGpu, pKernelChannelGroup)

NV_STATUS kchangrpFreeFaultMethodBuffers_GV100(struct OBJGPU *pGpu, struct KernelChannelGroup *pKernelChannelGroup);


#ifdef __nvoc_kernel_channel_group_h_disabled
static inline NV_STATUS kchangrpFreeFaultMethodBuffers(struct OBJGPU *pGpu, struct KernelChannelGroup *pKernelChannelGroup) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroup was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_group_h_disabled
#define kchangrpFreeFaultMethodBuffers(pGpu, pKernelChannelGroup) kchangrpFreeFaultMethodBuffers_GV100(pGpu, pKernelChannelGroup)
#endif //__nvoc_kernel_channel_group_h_disabled

#define kchangrpFreeFaultMethodBuffers_HAL(pGpu, pKernelChannelGroup) kchangrpFreeFaultMethodBuffers(pGpu, pKernelChannelGroup)

NV_STATUS kchangrpMapFaultMethodBuffers_GV100(struct OBJGPU *pGpu, struct KernelChannelGroup *pKernelChannelGroup, NvU32 runqueue);


#ifdef __nvoc_kernel_channel_group_h_disabled
static inline NV_STATUS kchangrpMapFaultMethodBuffers(struct OBJGPU *pGpu, struct KernelChannelGroup *pKernelChannelGroup, NvU32 runqueue) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroup was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_group_h_disabled
#define kchangrpMapFaultMethodBuffers(pGpu, pKernelChannelGroup, runqueue) kchangrpMapFaultMethodBuffers_GV100(pGpu, pKernelChannelGroup, runqueue)
#endif //__nvoc_kernel_channel_group_h_disabled

#define kchangrpMapFaultMethodBuffers_HAL(pGpu, pKernelChannelGroup, runqueue) kchangrpMapFaultMethodBuffers(pGpu, pKernelChannelGroup, runqueue)

NV_STATUS kchangrpUnmapFaultMethodBuffers_GV100(struct OBJGPU *pGpu, struct KernelChannelGroup *pKernelChannelGroup, NvU32 runqueue);


#ifdef __nvoc_kernel_channel_group_h_disabled
static inline NV_STATUS kchangrpUnmapFaultMethodBuffers(struct OBJGPU *pGpu, struct KernelChannelGroup *pKernelChannelGroup, NvU32 runqueue) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroup was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_group_h_disabled
#define kchangrpUnmapFaultMethodBuffers(pGpu, pKernelChannelGroup, runqueue) kchangrpUnmapFaultMethodBuffers_GV100(pGpu, pKernelChannelGroup, runqueue)
#endif //__nvoc_kernel_channel_group_h_disabled

#define kchangrpUnmapFaultMethodBuffers_HAL(pGpu, pKernelChannelGroup, runqueue) kchangrpUnmapFaultMethodBuffers(pGpu, pKernelChannelGroup, runqueue)

static inline NV_STATUS kchangrpSetRealtime_56cd7a(struct OBJGPU *pGpu, struct KernelChannelGroup *pKernelChannelGroup, NvBool bRealtime) {
    return NV_OK;
}


#ifdef __nvoc_kernel_channel_group_h_disabled
static inline NV_STATUS kchangrpSetRealtime(struct OBJGPU *pGpu, struct KernelChannelGroup *pKernelChannelGroup, NvBool bRealtime) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroup was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_group_h_disabled
#define kchangrpSetRealtime(pGpu, pKernelChannelGroup, bRealtime) kchangrpSetRealtime_56cd7a(pGpu, pKernelChannelGroup, bRealtime)
#endif //__nvoc_kernel_channel_group_h_disabled

#define kchangrpSetRealtime_HAL(pGpu, pKernelChannelGroup, bRealtime) kchangrpSetRealtime(pGpu, pKernelChannelGroup, bRealtime)

NV_STATUS kchangrpConstruct_IMPL(struct KernelChannelGroup *arg_pKernelChannelGroup);

#define __nvoc_kchangrpConstruct(arg_pKernelChannelGroup) kchangrpConstruct_IMPL(arg_pKernelChannelGroup)
void kchangrpDestruct_IMPL(struct KernelChannelGroup *pKernelChannelGroup);

#define __nvoc_kchangrpDestruct(pKernelChannelGroup) kchangrpDestruct_IMPL(pKernelChannelGroup)
void kchangrpSetState_IMPL(struct KernelChannelGroup *pKernelChannelGroup, NvU32 subdevice, CHANNELGROUP_STATE state);

#ifdef __nvoc_kernel_channel_group_h_disabled
static inline void kchangrpSetState(struct KernelChannelGroup *pKernelChannelGroup, NvU32 subdevice, CHANNELGROUP_STATE state) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroup was disabled!");
}
#else //__nvoc_kernel_channel_group_h_disabled
#define kchangrpSetState(pKernelChannelGroup, subdevice, state) kchangrpSetState_IMPL(pKernelChannelGroup, subdevice, state)
#endif //__nvoc_kernel_channel_group_h_disabled

void kchangrpClearState_IMPL(struct KernelChannelGroup *pKernelChannelGroup, NvU32 subdevice, CHANNELGROUP_STATE state);

#ifdef __nvoc_kernel_channel_group_h_disabled
static inline void kchangrpClearState(struct KernelChannelGroup *pKernelChannelGroup, NvU32 subdevice, CHANNELGROUP_STATE state) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroup was disabled!");
}
#else //__nvoc_kernel_channel_group_h_disabled
#define kchangrpClearState(pKernelChannelGroup, subdevice, state) kchangrpClearState_IMPL(pKernelChannelGroup, subdevice, state)
#endif //__nvoc_kernel_channel_group_h_disabled

NvBool kchangrpIsStateSet_IMPL(struct KernelChannelGroup *pKernelChannelGroup, NvU32 subdevice, CHANNELGROUP_STATE state);

#ifdef __nvoc_kernel_channel_group_h_disabled
static inline NvBool kchangrpIsStateSet(struct KernelChannelGroup *pKernelChannelGroup, NvU32 subdevice, CHANNELGROUP_STATE state) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroup was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_channel_group_h_disabled
#define kchangrpIsStateSet(pKernelChannelGroup, subdevice, state) kchangrpIsStateSet_IMPL(pKernelChannelGroup, subdevice, state)
#endif //__nvoc_kernel_channel_group_h_disabled

NV_STATUS kchangrpAddChannel_IMPL(struct OBJGPU *pGpu, struct KernelChannelGroup *pKernelChannelGroup, struct KernelChannel *pKernelChannel);

#ifdef __nvoc_kernel_channel_group_h_disabled
static inline NV_STATUS kchangrpAddChannel(struct OBJGPU *pGpu, struct KernelChannelGroup *pKernelChannelGroup, struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroup was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_group_h_disabled
#define kchangrpAddChannel(pGpu, pKernelChannelGroup, pKernelChannel) kchangrpAddChannel_IMPL(pGpu, pKernelChannelGroup, pKernelChannel)
#endif //__nvoc_kernel_channel_group_h_disabled

NV_STATUS kchangrpRemoveChannel_IMPL(struct OBJGPU *pGpu, struct KernelChannelGroup *pKernelChannelGroup, struct KernelChannel *pKernelChannel);

#ifdef __nvoc_kernel_channel_group_h_disabled
static inline NV_STATUS kchangrpRemoveChannel(struct OBJGPU *pGpu, struct KernelChannelGroup *pKernelChannelGroup, struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroup was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_group_h_disabled
#define kchangrpRemoveChannel(pGpu, pKernelChannelGroup, pKernelChannel) kchangrpRemoveChannel_IMPL(pGpu, pKernelChannelGroup, pKernelChannel)
#endif //__nvoc_kernel_channel_group_h_disabled

NV_STATUS kchangrpInit_IMPL(struct OBJGPU *pGpu, struct KernelChannelGroup *pKernelChannelGroup, struct OBJVASPACE *pVAS, NvU32 gfid);

#ifdef __nvoc_kernel_channel_group_h_disabled
static inline NV_STATUS kchangrpInit(struct OBJGPU *pGpu, struct KernelChannelGroup *pKernelChannelGroup, struct OBJVASPACE *pVAS, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroup was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_group_h_disabled
#define kchangrpInit(pGpu, pKernelChannelGroup, pVAS, gfid) kchangrpInit_IMPL(pGpu, pKernelChannelGroup, pVAS, gfid)
#endif //__nvoc_kernel_channel_group_h_disabled

NV_STATUS kchangrpDestroy_IMPL(struct OBJGPU *pGpu, struct KernelChannelGroup *pKernelChannelGroup);

#ifdef __nvoc_kernel_channel_group_h_disabled
static inline NV_STATUS kchangrpDestroy(struct OBJGPU *pGpu, struct KernelChannelGroup *pKernelChannelGroup) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroup was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_group_h_disabled
#define kchangrpDestroy(pGpu, pKernelChannelGroup) kchangrpDestroy_IMPL(pGpu, pKernelChannelGroup)
#endif //__nvoc_kernel_channel_group_h_disabled

NV_STATUS kchangrpAllocEngineContextDescriptor_IMPL(struct OBJGPU *pGpu, struct KernelChannelGroup *pKernelChannelGroup);

#ifdef __nvoc_kernel_channel_group_h_disabled
static inline NV_STATUS kchangrpAllocEngineContextDescriptor(struct OBJGPU *pGpu, struct KernelChannelGroup *pKernelChannelGroup) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroup was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_group_h_disabled
#define kchangrpAllocEngineContextDescriptor(pGpu, pKernelChannelGroup) kchangrpAllocEngineContextDescriptor_IMPL(pGpu, pKernelChannelGroup)
#endif //__nvoc_kernel_channel_group_h_disabled

NV_STATUS kchangrpGetEngineContextMemDesc_IMPL(struct OBJGPU *pGpu, struct KernelChannelGroup *arg2, MEMORY_DESCRIPTOR **arg3);

#ifdef __nvoc_kernel_channel_group_h_disabled
static inline NV_STATUS kchangrpGetEngineContextMemDesc(struct OBJGPU *pGpu, struct KernelChannelGroup *arg2, MEMORY_DESCRIPTOR **arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroup was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_group_h_disabled
#define kchangrpGetEngineContextMemDesc(pGpu, arg2, arg3) kchangrpGetEngineContextMemDesc_IMPL(pGpu, arg2, arg3)
#endif //__nvoc_kernel_channel_group_h_disabled

NV_STATUS kchangrpSetInterleaveLevel_IMPL(struct OBJGPU *pGpu, struct KernelChannelGroup *pKernelChannelGroup, NvU32 value);

#ifdef __nvoc_kernel_channel_group_h_disabled
static inline NV_STATUS kchangrpSetInterleaveLevel(struct OBJGPU *pGpu, struct KernelChannelGroup *pKernelChannelGroup, NvU32 value) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroup was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_group_h_disabled
#define kchangrpSetInterleaveLevel(pGpu, pKernelChannelGroup, value) kchangrpSetInterleaveLevel_IMPL(pGpu, pKernelChannelGroup, value)
#endif //__nvoc_kernel_channel_group_h_disabled

#undef PRIVATE_FIELD


MAKE_INTRUSIVE_MAP(KernelChannelGroupMap, KernelChannelGroup, mapNode);

#endif // KERNEL_CHANNEL_GROUP_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_CHANNEL_GROUP_NVOC_H_
