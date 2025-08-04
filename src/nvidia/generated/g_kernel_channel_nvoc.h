
#ifndef _G_KERNEL_CHANNEL_NVOC_H_
#define _G_KERNEL_CHANNEL_NVOC_H_

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_kernel_channel_nvoc.h"

#ifndef KERNEL_CHANNEL_H
#define KERNEL_CHANNEL_H

#include "core/core.h"
#include "resserv/resserv.h"
#include "nvoc/prelude.h"
#include "gpu/gpu_resource.h"
#include "kernel/gpu/gpu_engine_type.h"
#include "kernel/gpu/fifo/kernel_ctxshare.h"
#include "kernel/gpu/fifo/kernel_fifo.h"
#include "kernel/gpu/gr/kernel_graphics_context.h"
#include "kernel/gpu/intr/intr_service.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"

#include "ctrl/ctrl0090.h"
#include "ctrl/ctrl208f/ctrl208ffifo.h"
#include "ctrl/ctrl506f.h"
#include "ctrl/ctrl906f.h"
#include "ctrl/ctrla06f.h"
#include "ctrl/ctrlb06f.h"
#include "ctrl/ctrlc36f.h"
#include "ctrl/ctrlc56f.h"

#include "cc_drv.h"


struct OBJGPU;

#ifndef __NVOC_CLASS_OBJGPU_TYPEDEF__
#define __NVOC_CLASS_OBJGPU_TYPEDEF__
typedef struct OBJGPU OBJGPU;
#endif /* __NVOC_CLASS_OBJGPU_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPU
#define __nvoc_class_id_OBJGPU 0x7ef3cb
#endif /* __nvoc_class_id_OBJGPU */



struct UserInfo;

#ifndef __NVOC_CLASS_UserInfo_TYPEDEF__
#define __NVOC_CLASS_UserInfo_TYPEDEF__
typedef struct UserInfo UserInfo;
#endif /* __NVOC_CLASS_UserInfo_TYPEDEF__ */

#ifndef __nvoc_class_id_UserInfo
#define __nvoc_class_id_UserInfo 0x21d236
#endif /* __nvoc_class_id_UserInfo */


/*!
 * @brief Type of hErrorContext or hEccErrorContext
 *
 * This is RPCed to GSP in #NV_CHANNEL_ALLOC_PARAMS.internalFlags
 * along with the actual memdesc in
 * #NV_CHANNEL_ALLOC_PARAMS.errorNotifierMem and
 * #NV_CHANNEL_ALLOC_PARAMS.eccErrorNotifierMem.
 */
typedef enum {
    /*!
     * Initial state as passed in NV_CHANNEL_ALLOC_PARAMS by
     * kernel CPU-RM clients.
     */
    ERROR_NOTIFIER_TYPE_UNKNOWN = 0,
    /*! @brief Error notifier is explicitly not set.
     *
     * The corresponding hErrorContext or hEccErrorContext must be
     * NV01_NULL_OBJECT.
     */
    ERROR_NOTIFIER_TYPE_NONE,
    /*! @brief Error notifier is a ContextDma */
    ERROR_NOTIFIER_TYPE_CTXDMA,
    /*! @brief Error notifier is a NvNotification array in sysmem/vidmem */
    ERROR_NOTIFIER_TYPE_MEMORY
} ErrorNotifierType;

//
// Iterates over the ChannelDescendants on a channel
// Uses an RS_ORDERED_ITERATOR and filters it by EngineID / ClassID
//
typedef struct {
    RS_ORDERED_ITERATOR rsIter;
    RM_ENGINE_TYPE engineID;
    NvU32 classID;
} KernelChannelChildIterator;

typedef enum
{
    CHANNEL_CLASS_TYPE_DMA,
    CHANNEL_CLASS_TYPE_GPFIFO,
} CHANNEL_CLASS_TYPE;

//
// Channel class info structure.
//
// Filled in by CliGetChannelClassInfo() routine.
//
typedef struct
{
    NvU32 notifiersMaxCount;    // max# of notifiers for class
    NvU32 rcNotifierIndex;      // RC notifier index differs depending on the channel class
    CHANNEL_CLASS_TYPE classType;
} CLI_CHANNEL_CLASS_INFO;

void CliGetChannelClassInfo(NvU32, CLI_CHANNEL_CLASS_INFO*);

/*!
 * This structure represents an iterator for all objects
 * with given class number or engine tag on a channel or TSG.
 * It is created by function @ref kchannelGetChildIterOverGroup.
 */
typedef struct
{
    NvU32 engDesc;
    NvU32 classNum;

    //
    // During iteration, a copy of the current channel/TSG as well as the
    // next object node to start iterating from is tracked.
    //
    CHANNEL_NODE channelNode;
    KernelChannelChildIterator kchannelIter;
} KernelChannelChildIterOverGroup;

typedef struct _def_instance_block
{
    MEMORY_DESCRIPTOR  *pInstanceBlockDesc;
    MEMORY_DESCRIPTOR  *pRamfcDesc;
    /*!
     * Used only for Suspend Resume RM internal channel.
     * Will be moved to the Host context RL infolist.
     */
    MEMORY_DESCRIPTOR  *pRLMemDesc;
} FIFO_INSTANCE_BLOCK;

/* Bitfields in NV_CHANNEL_ALLOC_PARAMS.internalFlags */
#define NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_PRIVILEGE                       1:0
#define NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_PRIVILEGE_USER                  0x0
#define NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_PRIVILEGE_ADMIN                 0x1
#define NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_PRIVILEGE_KERNEL                0x2
#define NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_ERROR_NOTIFIER_TYPE             3:2
#define NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_ERROR_NOTIFIER_TYPE_UNKNOWN     ERROR_NOTIFIER_TYPE_UNKNOWN
#define NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_ERROR_NOTIFIER_TYPE_NONE        ERROR_NOTIFIER_TYPE_NONE
#define NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_ERROR_NOTIFIER_TYPE_CTXDMA      ERROR_NOTIFIER_TYPE_CTXDMA
#define NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_ERROR_NOTIFIER_TYPE_MEMORY      ERROR_NOTIFIER_TYPE_MEMORY
#define NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_ECC_ERROR_NOTIFIER_TYPE         5:4
#define NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_ECC_ERROR_NOTIFIER_TYPE_UNKNOWN ERROR_NOTIFIER_TYPE_UNKNOWN
#define NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_ECC_ERROR_NOTIFIER_TYPE_NONE    ERROR_NOTIFIER_TYPE_NONE
#define NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_ECC_ERROR_NOTIFIER_TYPE_CTXDMA  ERROR_NOTIFIER_TYPE_CTXDMA
#define NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_ECC_ERROR_NOTIFIER_TYPE_MEMORY  ERROR_NOTIFIER_TYPE_MEMORY

// Channel is created by GSP RM
#define NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_GSP_OWNED                       6:6
#define NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_GSP_OWNED_NO                    0x0
#define NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_GSP_OWNED_YES                   0x1

// Channel is created by UVM
#define NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_UVM_OWNED                       7:7
#define NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_UVM_OWNED_NO                    0x0
#define NV_KERNELCHANNEL_ALLOC_INTERNALFLAGS_UVM_OWNED_YES                   0x1

// printf format specifier for value returned by kchannelGetDebugTag
#define FMT_CHANNEL_DEBUG_TAG "channel 0x%08x"

/*!
 * Class for the kernel side of a Channel object.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_CHANNEL_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelChannel;
struct NVOC_METADATA__GpuResource;
struct NVOC_METADATA__Notifier;
struct NVOC_VTABLE__KernelChannel;


struct KernelChannel {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelChannel *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResource __nvoc_base_GpuResource;
    struct Notifier __nvoc_base_Notifier;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super
    struct INotifier *__nvoc_pbase_INotifier;    // inotify super^2
    struct Notifier *__nvoc_pbase_Notifier;    // notify super
    struct KernelChannel *__nvoc_pbase_KernelChannel;    // kchannel

    // Vtable with 19 per-object function pointers
    NV_STATUS (*__kchannelAllocMem__)(struct OBJGPU *, struct KernelChannel * /*this*/, NvU32, NvU32);  // halified (2 hals) body
    void (*__kchannelDestroyMem__)(struct OBJGPU *, struct KernelChannel * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kchannelAllocHwID__)(struct OBJGPU *, struct KernelChannel * /*this*/, NvHandle, NvU32, NvU32, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kchannelFreeHwID__)(struct OBJGPU *, struct KernelChannel * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kchannelGetUserdInfo__)(struct OBJGPU *, struct KernelChannel * /*this*/, NvU64 *, NvU64 *, NvU64 *);  // halified (2 hals)
    NV_STATUS (*__kchannelGetUserdBar1MapOffset__)(struct OBJGPU *, struct KernelChannel * /*this*/, NvU64 *, NvU32 *);  // halified (2 hals)
    NV_STATUS (*__kchannelCreateUserdMemDescBc__)(struct OBJGPU *, struct KernelChannel * /*this*/, NvHandle, NvHandle *, NvU64 *);  // halified (2 hals) body
    NV_STATUS (*__kchannelCreateUserdMemDesc__)(struct OBJGPU *, struct KernelChannel * /*this*/, NvHandle, NvHandle, NvU64, NvU64 *, NvU32 *);  // halified (2 hals)
    void (*__kchannelDestroyUserdMemDesc__)(struct OBJGPU *, struct KernelChannel * /*this*/);  // halified (2 hals)
    NV_STATUS (*__kchannelCreateUserMemDesc__)(struct OBJGPU *, struct KernelChannel * /*this*/);  // halified (3 hals)
    NvBool (*__kchannelIsUserdAddrSizeValid__)(struct KernelChannel * /*this*/, NvU32, NvU32);  // halified (5 hals) body
    NV_STATUS (*__kchannelGetEngine__)(struct OBJGPU *, struct KernelChannel * /*this*/, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__kchannelCtrlCmdGetKmb__)(struct KernelChannel * /*this*/, NVC56F_CTRL_CMD_GET_KMB_PARAMS *);  // halified (2 hals) exported (id=0xc56f010b) body
    NV_STATUS (*__kchannelCtrlRotateSecureChannelIv__)(struct KernelChannel * /*this*/, NVC56F_CTRL_ROTATE_SECURE_CHANNEL_IV_PARAMS *);  // halified (2 hals) exported (id=0xc56f010c) body
    NV_STATUS (*__kchannelSetEncryptionStatsBuffer__)(struct OBJGPU *, struct KernelChannel * /*this*/, MEMORY_DESCRIPTOR *, NvBool);  // halified (2 hals) body
    NV_STATUS (*__kchannelGetClassEngineID__)(struct OBJGPU *, struct KernelChannel * /*this*/, NvHandle, NvU32 *, NvU32 *, RM_ENGINE_TYPE *);  // halified (2 hals) body
    NV_STATUS (*__kchannelEnableVirtualContext__)(struct KernelChannel * /*this*/);  // halified (2 hals)
    NV_STATUS (*__kchannelDeriveAndRetrieveKmb__)(struct OBJGPU *, struct KernelChannel * /*this*/, CC_KMB *);  // halified (2 hals) body
    NV_STATUS (*__kchannelSetKeyRotationNotifier__)(struct OBJGPU *, struct KernelChannel * /*this*/, NvBool);  // halified (2 hals) body

    // Data members
    NvU16 nextObjectClassID;
    struct KernelChannel *pNextBindKernelChannel;
    FIFO_MMU_EXCEPTION_DATA *pMmuExceptionData;
    NvHandle hErrorContext;
    MEMORY_DESCRIPTOR *pErrContextMemDesc;
    ErrorNotifierType errorContextType;
    NvU64 errorContextOffset;
    NvHandle hEccErrorContext;
    MEMORY_DESCRIPTOR *pEccErrContextMemDesc;
    ErrorNotifierType eccErrorContextType;
    NvU64 eccErrorContextOffset;
    struct UserInfo *pUserInfo;
    NvHandle hVASpace;
    struct OBJVASPACE *pVAS;
    NvHandle hKernelGraphicsContext;
    NvU8 privilegeLevel;
    NvU32 runlistId;
    NvU32 ChID;
    struct KernelChannelGroupApi *pKernelChannelGroupApi;
    struct KernelCtxShareApi *pKernelCtxShareApi;
    NvU32 refCount;
    NvBool bGspOwned;
    NvBool bUvmOwned;
    NvBool bIsContextBound;
    FIFO_INSTANCE_BLOCK *pFifoHalData[8];
    MEMORY_DESCRIPTOR *pInstSubDeviceMemDesc[8];
    MEMORY_DESCRIPTOR *pUserdSubDeviceMemDesc[8];
    NvBool bClientAllocatedUserD;
    NvU32 swState[8];
    NvBool bIsRcPending[8];
    NvU32 ProcessID;
    NvU32 SubProcessID;
    NvU32 bcStateCurrent;
    NvU32 notifyIndex[3];
    NvU64 userdLength;
    NvBool bSkipCtxBufferAlloc;
    NvU32 subctxId;
    NvU32 vaSpaceId;
    NvU32 cid;
    struct MIG_INSTANCE_REF partitionRef;
    NvU32 runqueue;
    RM_ENGINE_TYPE engineType;
    NvU32 goldenCtxUpdateFlags;
    CC_KMB clientKmb;
    NvHandle hEncryptStatsBuf;
    MEMORY_DESCRIPTOR *pEncStatsBufMemDesc;
    CC_CRYPTOBUNDLE_STATS *pEncStatsBuf;
    MEMORY_DESCRIPTOR *pKeyRotationNotifierMemDesc;
    NvNotification *pKeyRotationNotifier;
    NvBool bCCSecureChannel;
    NvBool bUseScrubKey;
};


// Vtable with 30 per-class function pointers
struct NVOC_VTABLE__KernelChannel {
    NV_STATUS (*__kchannelMap__)(struct KernelChannel * /*this*/, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual override (res) base (gpures)
    NV_STATUS (*__kchannelUnmap__)(struct KernelChannel * /*this*/, CALL_CONTEXT *, RsCpuMapping *);  // virtual override (res) base (gpures)
    NV_STATUS (*__kchannelGetMapAddrSpace__)(struct KernelChannel * /*this*/, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual override (gpures) base (gpures)
    NV_STATUS (*__kchannelGetMemInterMapParams__)(struct KernelChannel * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual override (rmres) base (gpures)
    NV_STATUS (*__kchannelCheckMemInterUnmap__)(struct KernelChannel * /*this*/, NvBool);  // virtual override (rmres) base (gpures)
    NV_STATUS (*__kchannelControl__)(struct KernelChannel * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__kchannelShareCallback__)(struct KernelChannel * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kchannelGetRegBaseOffsetAndSize__)(struct KernelChannel * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kchannelInternalControlForward__)(struct KernelChannel * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__kchannelGetInternalObjectHandle__)(struct KernelChannel * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__kchannelAccessCallback__)(struct KernelChannel * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kchannelGetMemoryMappingDescriptor__)(struct KernelChannel * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kchannelControlSerialization_Prologue__)(struct KernelChannel * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__kchannelControlSerialization_Epilogue__)(struct KernelChannel * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kchannelControl_Prologue__)(struct KernelChannel * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__kchannelControl_Epilogue__)(struct KernelChannel * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__kchannelCanCopy__)(struct KernelChannel * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__kchannelIsDuplicate__)(struct KernelChannel * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__kchannelPreDestruct__)(struct KernelChannel * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__kchannelControlFilter__)(struct KernelChannel * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__kchannelIsPartialUnmapSupported__)(struct KernelChannel * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__kchannelMapTo__)(struct KernelChannel * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__kchannelUnmapFrom__)(struct KernelChannel * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__kchannelGetRefCount__)(struct KernelChannel * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__kchannelAddAdditionalDependants__)(struct RsClient *, struct KernelChannel * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
    PEVENTNOTIFICATION * (*__kchannelGetNotificationListPtr__)(struct KernelChannel * /*this*/);  // virtual inherited (notify) base (notify)
    struct NotifShare * (*__kchannelGetNotificationShare__)(struct KernelChannel * /*this*/);  // virtual inherited (notify) base (notify)
    void (*__kchannelSetNotificationShare__)(struct KernelChannel * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__kchannelUnregisterEvent__)(struct KernelChannel * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__kchannelGetOrAllocNotifShare__)(struct KernelChannel * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (notify)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelChannel {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_METADATA__Notifier metadata__Notifier;
    const struct NVOC_VTABLE__KernelChannel vtable;
};

#ifndef __NVOC_CLASS_KernelChannel_TYPEDEF__
#define __NVOC_CLASS_KernelChannel_TYPEDEF__
typedef struct KernelChannel KernelChannel;
#endif /* __NVOC_CLASS_KernelChannel_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelChannel
#define __nvoc_class_id_KernelChannel 0x5d8d70
#endif /* __nvoc_class_id_KernelChannel */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelChannel;

#define __staticCast_KernelChannel(pThis) \
    ((pThis)->__nvoc_pbase_KernelChannel)

#ifdef __nvoc_kernel_channel_h_disabled
#define __dynamicCast_KernelChannel(pThis) ((KernelChannel*) NULL)
#else //__nvoc_kernel_channel_h_disabled
#define __dynamicCast_KernelChannel(pThis) \
    ((KernelChannel*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelChannel)))
#endif //__nvoc_kernel_channel_h_disabled

NV_STATUS __nvoc_objCreateDynamic_KernelChannel(KernelChannel**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelChannel(KernelChannel**, Dynamic*, NvU32, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_KernelChannel(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_KernelChannel((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros for implementation functions
NV_STATUS kchannelConstruct_IMPL(struct KernelChannel *arg_pKernelChannel, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __nvoc_kchannelConstruct(arg_pKernelChannel, arg_pCallContext, arg_pParams) kchannelConstruct_IMPL(arg_pKernelChannel, arg_pCallContext, arg_pParams)

void kchannelDestruct_IMPL(struct KernelChannel *pResource);
#define __nvoc_kchannelDestruct(pResource) kchannelDestruct_IMPL(pResource)

NV_STATUS kchannelRegisterChild_IMPL(struct KernelChannel *pKernelChannel, ChannelDescendant *pObject);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelRegisterChild(struct KernelChannel *pKernelChannel, ChannelDescendant *pObject) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelRegisterChild(pKernelChannel, pObject) kchannelRegisterChild_IMPL(pKernelChannel, pObject)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelDeregisterChild_IMPL(struct KernelChannel *pKernelChannel, ChannelDescendant *pObject);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelDeregisterChild(struct KernelChannel *pKernelChannel, ChannelDescendant *pObject) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelDeregisterChild(pKernelChannel, pObject) kchannelDeregisterChild_IMPL(pKernelChannel, pObject)
#endif // __nvoc_kernel_channel_h_disabled

#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelNotifyRc(struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelNotifyRc(pKernelChannel) kchannelNotifyRc_IMPL(pKernelChannel)
#endif // __nvoc_kernel_channel_h_disabled

void kchannelNotifyEvent_IMPL(struct KernelChannel *pKernelChannel, NvU32 notifyIndex, NvU32 info32, NvU16 info16, void *pNotifyParams, NvU32 notifyParamsSize);
#ifdef __nvoc_kernel_channel_h_disabled
static inline void kchannelNotifyEvent(struct KernelChannel *pKernelChannel, NvU32 notifyIndex, NvU32 info32, NvU16 info16, void *pNotifyParams, NvU32 notifyParamsSize) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelNotifyEvent(pKernelChannel, notifyIndex, info32, info16, pNotifyParams, notifyParamsSize) kchannelNotifyEvent_IMPL(pKernelChannel, notifyIndex, info32, info16, pNotifyParams, notifyParamsSize)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelUpdateNotifierMem_IMPL(struct KernelChannel *pKernelChannel, NvU32 notifyIndex, NvU32 info32, NvU16 info16, NvU32 notifierStatus);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelUpdateNotifierMem(struct KernelChannel *pKernelChannel, NvU32 notifyIndex, NvU32 info32, NvU16 info16, NvU32 notifierStatus) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelUpdateNotifierMem(pKernelChannel, notifyIndex, info32, info16, notifierStatus) kchannelUpdateNotifierMem_IMPL(pKernelChannel, notifyIndex, info32, info16, notifierStatus)
#endif // __nvoc_kernel_channel_h_disabled

#ifdef __nvoc_kernel_channel_h_disabled
static inline NvBool kchannelIsSchedulable(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_FALSE;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelIsSchedulable(pGpu, pKernelChannel) kchannelIsSchedulable_IMPL(pGpu, pKernelChannel)
#endif // __nvoc_kernel_channel_h_disabled

#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelGetChannelPhysicalState(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NV208F_CTRL_FIFO_GET_CHANNEL_STATE_PARAMS *pChannelStateParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelGetChannelPhysicalState(pGpu, pKernelChannel, pChannelStateParams) kchannelGetChannelPhysicalState_KERNEL(pGpu, pKernelChannel, pChannelStateParams)
#endif // __nvoc_kernel_channel_h_disabled

#ifdef __nvoc_kernel_channel_h_disabled
static inline NvU32 kchannelEmbedRunlistID(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return 0;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelEmbedRunlistID(pGpu, pKernelChannel) kchannelEmbedRunlistID_13cd8d(pGpu, pKernelChannel)
#endif // __nvoc_kernel_channel_h_disabled

#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelFwdToInternalCtrl(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 internalCmd, RmCtrlParams *pRmCtrlParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelFwdToInternalCtrl(pGpu, pKernelChannel, internalCmd, pRmCtrlParams) kchannelFwdToInternalCtrl_56cd7a(pGpu, pKernelChannel, internalCmd, pRmCtrlParams)
#endif // __nvoc_kernel_channel_h_disabled

#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelAllocChannel(struct KernelChannel *pKernelChannel, NV_CHANNEL_ALLOC_PARAMS *pChannelGpfifoParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelAllocChannel(pKernelChannel, pChannelGpfifoParams) kchannelAllocChannel_56cd7a(pKernelChannel, pChannelGpfifoParams)
#endif // __nvoc_kernel_channel_h_disabled

#ifdef __nvoc_kernel_channel_h_disabled
static inline NvBool kchannelIsValid(struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_FALSE;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelIsValid(pKernelChannel) kchannelIsValid_88bc07(pKernelChannel)
#endif // __nvoc_kernel_channel_h_disabled

NvBool kchannelCheckIsUserMode_IMPL(struct KernelChannel *pKernelChannel);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NvBool kchannelCheckIsUserMode(struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_FALSE;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCheckIsUserMode(pKernelChannel) kchannelCheckIsUserMode_IMPL(pKernelChannel)
#endif // __nvoc_kernel_channel_h_disabled

NvBool kchannelCheckIsKernel_IMPL(struct KernelChannel *pKernelChannel);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NvBool kchannelCheckIsKernel(struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_FALSE;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCheckIsKernel(pKernelChannel) kchannelCheckIsKernel_IMPL(pKernelChannel)
#endif // __nvoc_kernel_channel_h_disabled

NvBool kchannelCheckIsAdmin_IMPL(struct KernelChannel *pKernelChannel);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NvBool kchannelCheckIsAdmin(struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_FALSE;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCheckIsAdmin(pKernelChannel) kchannelCheckIsAdmin_IMPL(pKernelChannel)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelBindToRunlist_IMPL(struct KernelChannel *pKernelChannel, RM_ENGINE_TYPE localRmEngineType, ENGDESCRIPTOR engineDesc);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelBindToRunlist(struct KernelChannel *pKernelChannel, RM_ENGINE_TYPE localRmEngineType, ENGDESCRIPTOR engineDesc) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelBindToRunlist(pKernelChannel, localRmEngineType, engineDesc) kchannelBindToRunlist_IMPL(pKernelChannel, localRmEngineType, engineDesc)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelSetEngineContextMemDesc_IMPL(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 engine, MEMORY_DESCRIPTOR *pMemDesc);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelSetEngineContextMemDesc(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 engine, MEMORY_DESCRIPTOR *pMemDesc) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelSetEngineContextMemDesc(pGpu, pKernelChannel, engine, pMemDesc) kchannelSetEngineContextMemDesc_IMPL(pGpu, pKernelChannel, engine, pMemDesc)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelMapEngineCtxBuf_IMPL(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 engine);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelMapEngineCtxBuf(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 engine) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelMapEngineCtxBuf(pGpu, pKernelChannel, engine) kchannelMapEngineCtxBuf_IMPL(pGpu, pKernelChannel, engine)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelUnmapEngineCtxBuf_IMPL(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 engine);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelUnmapEngineCtxBuf(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 engine) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelUnmapEngineCtxBuf(pGpu, pKernelChannel, engine) kchannelUnmapEngineCtxBuf_IMPL(pGpu, pKernelChannel, engine)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCheckBcStateCurrent_IMPL(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCheckBcStateCurrent(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCheckBcStateCurrent(pGpu, pKernelChannel) kchannelCheckBcStateCurrent_IMPL(pGpu, pKernelChannel)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelUpdateWorkSubmitTokenNotifIndex_IMPL(struct OBJGPU *pGpu, struct KernelChannel *arg_this, NvU32 index);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelUpdateWorkSubmitTokenNotifIndex(struct OBJGPU *pGpu, struct KernelChannel *arg_this, NvU32 index) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelUpdateWorkSubmitTokenNotifIndex(pGpu, arg_this, index) kchannelUpdateWorkSubmitTokenNotifIndex_IMPL(pGpu, arg_this, index)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelNotifyWorkSubmitToken_IMPL(struct OBJGPU *pGpu, struct KernelChannel *arg_this, NvU32 token);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelNotifyWorkSubmitToken(struct OBJGPU *pGpu, struct KernelChannel *arg_this, NvU32 token) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelNotifyWorkSubmitToken(pGpu, arg_this, token) kchannelNotifyWorkSubmitToken_IMPL(pGpu, arg_this, token)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelMapUserD_IMPL(struct OBJGPU *pGpu, struct KernelChannel *arg_this, RS_PRIV_LEVEL arg3, NvU64 arg4, NvU32 arg5, NvP64 *arg6, NvP64 *arg7);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelMapUserD(struct OBJGPU *pGpu, struct KernelChannel *arg_this, RS_PRIV_LEVEL arg3, NvU64 arg4, NvU32 arg5, NvP64 *arg6, NvP64 *arg7) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelMapUserD(pGpu, arg_this, arg3, arg4, arg5, arg6, arg7) kchannelMapUserD_IMPL(pGpu, arg_this, arg3, arg4, arg5, arg6, arg7)
#endif // __nvoc_kernel_channel_h_disabled

void kchannelUnmapUserD_IMPL(struct OBJGPU *pGpu, struct KernelChannel *arg_this, RS_PRIV_LEVEL arg3, NvP64 *arg4, NvP64 *arg5);
#ifdef __nvoc_kernel_channel_h_disabled
static inline void kchannelUnmapUserD(struct OBJGPU *pGpu, struct KernelChannel *arg_this, RS_PRIV_LEVEL arg3, NvP64 *arg4, NvP64 *arg5) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelUnmapUserD(pGpu, arg_this, arg3, arg4, arg5) kchannelUnmapUserD_IMPL(pGpu, arg_this, arg3, arg4, arg5)
#endif // __nvoc_kernel_channel_h_disabled

void kchannelFillMmuExceptionInfo_IMPL(struct KernelChannel *pKernelChannel, FIFO_MMU_EXCEPTION_DATA *arg2);
#ifdef __nvoc_kernel_channel_h_disabled
static inline void kchannelFillMmuExceptionInfo(struct KernelChannel *pKernelChannel, FIFO_MMU_EXCEPTION_DATA *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelFillMmuExceptionInfo(pKernelChannel, arg2) kchannelFillMmuExceptionInfo_IMPL(pKernelChannel, arg2)
#endif // __nvoc_kernel_channel_h_disabled

void kchannelFreeMmuExceptionInfo_IMPL(struct KernelChannel *pKernelChannel);
#ifdef __nvoc_kernel_channel_h_disabled
static inline void kchannelFreeMmuExceptionInfo(struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelFreeMmuExceptionInfo(pKernelChannel) kchannelFreeMmuExceptionInfo_IMPL(pKernelChannel)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelGetFromDualHandle_IMPL(struct RsClient *arg1, NvHandle arg2, struct KernelChannel **arg3);
#define kchannelGetFromDualHandle(arg1, arg2, arg3) kchannelGetFromDualHandle_IMPL(arg1, arg2, arg3)

NV_STATUS kchannelGetFromDualHandleRestricted_IMPL(struct RsClient *arg1, NvHandle arg2, struct KernelChannel **arg3);
#define kchannelGetFromDualHandleRestricted(arg1, arg2, arg3) kchannelGetFromDualHandleRestricted_IMPL(arg1, arg2, arg3)

NvU32 kchannelGetGfid_IMPL(struct KernelChannel *pKernelChannel);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NvU32 kchannelGetGfid(struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return 0;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelGetGfid(pKernelChannel) kchannelGetGfid_IMPL(pKernelChannel)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCtrlCmdResetIsolatedChannel_IMPL(struct KernelChannel *pKernelChannel, NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS *pResetParams);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCtrlCmdResetIsolatedChannel(struct KernelChannel *pKernelChannel, NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS *pResetParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCtrlCmdResetIsolatedChannel(pKernelChannel, pResetParams) kchannelCtrlCmdResetIsolatedChannel_IMPL(pKernelChannel, pResetParams)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCtrlCmdInternalResetIsolatedChannel_IMPL(struct KernelChannel *pKernelChannel, NV506F_CTRL_CMD_INTERNAL_RESET_ISOLATED_CHANNEL_PARAMS *pResetParams);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCtrlCmdInternalResetIsolatedChannel(struct KernelChannel *pKernelChannel, NV506F_CTRL_CMD_INTERNAL_RESET_ISOLATED_CHANNEL_PARAMS *pResetParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCtrlCmdInternalResetIsolatedChannel(pKernelChannel, pResetParams) kchannelCtrlCmdInternalResetIsolatedChannel_IMPL(pKernelChannel, pResetParams)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCtrlCmdGetClassEngineid_IMPL(struct KernelChannel *pKernelChannel, NV906F_CTRL_GET_CLASS_ENGINEID_PARAMS *pParams);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCtrlCmdGetClassEngineid(struct KernelChannel *pKernelChannel, NV906F_CTRL_GET_CLASS_ENGINEID_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCtrlCmdGetClassEngineid(pKernelChannel, pParams) kchannelCtrlCmdGetClassEngineid_IMPL(pKernelChannel, pParams)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCtrlCmdResetChannel_IMPL(struct KernelChannel *pKernelChannel, NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS *pResetChannelParams);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCtrlCmdResetChannel(struct KernelChannel *pKernelChannel, NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS *pResetChannelParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCtrlCmdResetChannel(pKernelChannel, pResetChannelParams) kchannelCtrlCmdResetChannel_IMPL(pKernelChannel, pResetChannelParams)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCtrlCmdGetDeferRCState_IMPL(struct KernelChannel *pKernelChannel, NV906F_CTRL_CMD_GET_DEFER_RC_STATE_PARAMS *pStateParams);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCtrlCmdGetDeferRCState(struct KernelChannel *pKernelChannel, NV906F_CTRL_CMD_GET_DEFER_RC_STATE_PARAMS *pStateParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCtrlCmdGetDeferRCState(pKernelChannel, pStateParams) kchannelCtrlCmdGetDeferRCState_IMPL(pKernelChannel, pStateParams)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCtrlCmdGetMmuFaultInfo_IMPL(struct KernelChannel *pKernelChannel, NV906F_CTRL_GET_MMU_FAULT_INFO_PARAMS *pFaultInfoParams);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCtrlCmdGetMmuFaultInfo(struct KernelChannel *pKernelChannel, NV906F_CTRL_GET_MMU_FAULT_INFO_PARAMS *pFaultInfoParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCtrlCmdGetMmuFaultInfo(pKernelChannel, pFaultInfoParams) kchannelCtrlCmdGetMmuFaultInfo_IMPL(pKernelChannel, pFaultInfoParams)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCtrlCmdGpFifoSchedule_IMPL(struct KernelChannel *pKernelChannel, NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCtrlCmdGpFifoSchedule(struct KernelChannel *pKernelChannel, NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCtrlCmdGpFifoSchedule(pKernelChannel, pSchedParams) kchannelCtrlCmdGpFifoSchedule_IMPL(pKernelChannel, pSchedParams)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCtrlCmdBind_IMPL(struct KernelChannel *pKernelChannel, NVA06F_CTRL_BIND_PARAMS *pParams);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCtrlCmdBind(struct KernelChannel *pKernelChannel, NVA06F_CTRL_BIND_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCtrlCmdBind(pKernelChannel, pParams) kchannelCtrlCmdBind_IMPL(pKernelChannel, pParams)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCtrlCmdSetErrorNotifier_IMPL(struct KernelChannel *pKernelChannel, NVA06F_CTRL_SET_ERROR_NOTIFIER_PARAMS *pSetErrorNotifierParams);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCtrlCmdSetErrorNotifier(struct KernelChannel *pKernelChannel, NVA06F_CTRL_SET_ERROR_NOTIFIER_PARAMS *pSetErrorNotifierParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCtrlCmdSetErrorNotifier(pKernelChannel, pSetErrorNotifierParams) kchannelCtrlCmdSetErrorNotifier_IMPL(pKernelChannel, pSetErrorNotifierParams)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCtrlCmdSetInterleaveLevel_IMPL(struct KernelChannel *pKernelChannel, NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS *pParams);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCtrlCmdSetInterleaveLevel(struct KernelChannel *pKernelChannel, NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCtrlCmdSetInterleaveLevel(pKernelChannel, pParams) kchannelCtrlCmdSetInterleaveLevel_IMPL(pKernelChannel, pParams)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCtrlCmdGetContextId_IMPL(struct KernelChannel *pKernelChannel, NVA06F_CTRL_GET_CONTEXT_ID_PARAMS *pParams);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCtrlCmdGetContextId(struct KernelChannel *pKernelChannel, NVA06F_CTRL_GET_CONTEXT_ID_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCtrlCmdGetContextId(pKernelChannel, pParams) kchannelCtrlCmdGetContextId_IMPL(pKernelChannel, pParams)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCtrlCmdRestartRunlist_IMPL(struct KernelChannel *pKernelChannel, NVA06F_CTRL_RESTART_RUNLIST_PARAMS *pParams);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCtrlCmdRestartRunlist(struct KernelChannel *pKernelChannel, NVA06F_CTRL_RESTART_RUNLIST_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCtrlCmdRestartRunlist(pKernelChannel, pParams) kchannelCtrlCmdRestartRunlist_IMPL(pKernelChannel, pParams)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCtrlCmdGetEngineCtxSize_IMPL(struct KernelChannel *pKernelChannel, NVB06F_CTRL_GET_ENGINE_CTX_SIZE_PARAMS *pCtxSizeParams);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCtrlCmdGetEngineCtxSize(struct KernelChannel *pKernelChannel, NVB06F_CTRL_GET_ENGINE_CTX_SIZE_PARAMS *pCtxSizeParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCtrlCmdGetEngineCtxSize(pKernelChannel, pCtxSizeParams) kchannelCtrlCmdGetEngineCtxSize_IMPL(pKernelChannel, pCtxSizeParams)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCtrlCmdGetEngineCtxData_IMPL(struct KernelChannel *pKernelChannel, NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *pCtxBuffParams);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCtrlCmdGetEngineCtxData(struct KernelChannel *pKernelChannel, NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *pCtxBuffParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCtrlCmdGetEngineCtxData(pKernelChannel, pCtxBuffParams) kchannelCtrlCmdGetEngineCtxData_IMPL(pKernelChannel, pCtxBuffParams)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCtrlCmdMigrateEngineCtxData_IMPL(struct KernelChannel *pKernelChannel, NVB06F_CTRL_MIGRATE_ENGINE_CTX_DATA_PARAMS *pCtxBuffParams);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCtrlCmdMigrateEngineCtxData(struct KernelChannel *pKernelChannel, NVB06F_CTRL_MIGRATE_ENGINE_CTX_DATA_PARAMS *pCtxBuffParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCtrlCmdMigrateEngineCtxData(pKernelChannel, pCtxBuffParams) kchannelCtrlCmdMigrateEngineCtxData_IMPL(pKernelChannel, pCtxBuffParams)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCtrlCmdGetEngineCtxState_IMPL(struct KernelChannel *pKernelChannel, NVB06F_CTRL_GET_ENGINE_CTX_STATE_PARAMS *pCtxStateParams);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCtrlCmdGetEngineCtxState(struct KernelChannel *pKernelChannel, NVB06F_CTRL_GET_ENGINE_CTX_STATE_PARAMS *pCtxStateParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCtrlCmdGetEngineCtxState(pKernelChannel, pCtxStateParams) kchannelCtrlCmdGetEngineCtxState_IMPL(pKernelChannel, pCtxStateParams)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCtrlCmdGetChannelHwState_IMPL(struct KernelChannel *pKernelChannel, NVB06F_CTRL_GET_CHANNEL_HW_STATE_PARAMS *pParams);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCtrlCmdGetChannelHwState(struct KernelChannel *pKernelChannel, NVB06F_CTRL_GET_CHANNEL_HW_STATE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCtrlCmdGetChannelHwState(pKernelChannel, pParams) kchannelCtrlCmdGetChannelHwState_IMPL(pKernelChannel, pParams)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCtrlCmdSetChannelHwState_IMPL(struct KernelChannel *pKernelChannel, NVB06F_CTRL_SET_CHANNEL_HW_STATE_PARAMS *pParams);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCtrlCmdSetChannelHwState(struct KernelChannel *pKernelChannel, NVB06F_CTRL_SET_CHANNEL_HW_STATE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCtrlCmdSetChannelHwState(pKernelChannel, pParams) kchannelCtrlCmdSetChannelHwState_IMPL(pKernelChannel, pParams)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCtrlCmdSaveEngineCtxData_IMPL(struct KernelChannel *pKernelChannel, NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS *pCtxBuffParams);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCtrlCmdSaveEngineCtxData(struct KernelChannel *pKernelChannel, NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS *pCtxBuffParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCtrlCmdSaveEngineCtxData(pKernelChannel, pCtxBuffParams) kchannelCtrlCmdSaveEngineCtxData_IMPL(pKernelChannel, pCtxBuffParams)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCtrlCmdRestoreEngineCtxData_IMPL(struct KernelChannel *pKernelChannel, NVB06F_CTRL_RESTORE_ENGINE_CTX_DATA_PARAMS *pCtxBuffParams);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCtrlCmdRestoreEngineCtxData(struct KernelChannel *pKernelChannel, NVB06F_CTRL_RESTORE_ENGINE_CTX_DATA_PARAMS *pCtxBuffParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCtrlCmdRestoreEngineCtxData(pKernelChannel, pCtxBuffParams) kchannelCtrlCmdRestoreEngineCtxData_IMPL(pKernelChannel, pCtxBuffParams)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCtrlCmdGpfifoGetWorkSubmitToken_IMPL(struct KernelChannel *pKernelChannel, NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS *pTokenParams);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCtrlCmdGpfifoGetWorkSubmitToken(struct KernelChannel *pKernelChannel, NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS *pTokenParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCtrlCmdGpfifoGetWorkSubmitToken(pKernelChannel, pTokenParams) kchannelCtrlCmdGpfifoGetWorkSubmitToken_IMPL(pKernelChannel, pTokenParams)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCtrlCmdInternalGpFifoGetWorkSubmitToken_IMPL(struct KernelChannel *pKernelChannel, NVC36F_CTRL_INTERNAL_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS *pTokenParams);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCtrlCmdInternalGpFifoGetWorkSubmitToken(struct KernelChannel *pKernelChannel, NVC36F_CTRL_INTERNAL_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS *pTokenParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCtrlCmdInternalGpFifoGetWorkSubmitToken(pKernelChannel, pTokenParams) kchannelCtrlCmdInternalGpFifoGetWorkSubmitToken_IMPL(pKernelChannel, pTokenParams)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCtrlCmdGpfifoUpdateFaultMethodBuffer_IMPL(struct KernelChannel *pKernelChannel, NVC36F_CTRL_GPFIFO_UPDATE_FAULT_METHOD_BUFFER_PARAMS *pFaultMthdBufferParams);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCtrlCmdGpfifoUpdateFaultMethodBuffer(struct KernelChannel *pKernelChannel, NVC36F_CTRL_GPFIFO_UPDATE_FAULT_METHOD_BUFFER_PARAMS *pFaultMthdBufferParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCtrlCmdGpfifoUpdateFaultMethodBuffer(pKernelChannel, pFaultMthdBufferParams) kchannelCtrlCmdGpfifoUpdateFaultMethodBuffer_IMPL(pKernelChannel, pFaultMthdBufferParams)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCtrlCmdGpfifoSetWorkSubmitTokenNotifIndex_IMPL(struct KernelChannel *pKernelChannel, NVC36F_CTRL_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX_PARAMS *pParams);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCtrlCmdGpfifoSetWorkSubmitTokenNotifIndex(struct KernelChannel *pKernelChannel, NVC36F_CTRL_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCtrlCmdGpfifoSetWorkSubmitTokenNotifIndex(pKernelChannel, pParams) kchannelCtrlCmdGpfifoSetWorkSubmitTokenNotifIndex_IMPL(pKernelChannel, pParams)
#endif // __nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCtrlCmdStopChannel_IMPL(struct KernelChannel *pKernelChannel, NVA06F_CTRL_STOP_CHANNEL_PARAMS *pStopChannelParams);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCtrlCmdStopChannel(struct KernelChannel *pKernelChannel, NVA06F_CTRL_STOP_CHANNEL_PARAMS *pStopChannelParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelCtrlCmdStopChannel(pKernelChannel, pStopChannelParams) kchannelCtrlCmdStopChannel_IMPL(pKernelChannel, pStopChannelParams)
#endif // __nvoc_kernel_channel_h_disabled

NvU32 kchannelGetGoldenCtxUpdateFlags_IMPL(struct KernelChannel *pKernelChannel);
#ifdef __nvoc_kernel_channel_h_disabled
static inline NvU32 kchannelGetGoldenCtxUpdateFlags(struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return 0;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelGetGoldenCtxUpdateFlags(pKernelChannel) kchannelGetGoldenCtxUpdateFlags_IMPL(pKernelChannel)
#endif // __nvoc_kernel_channel_h_disabled

#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelRotateSecureChannelIv(struct KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation, NvU32 *encryptIv, NvU32 *decryptIv) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_h_disabled
#define kchannelRotateSecureChannelIv(pKernelChannel, rotateOperation, encryptIv, decryptIv) kchannelRotateSecureChannelIv_46f6a7(pKernelChannel, rotateOperation, encryptIv, decryptIv)
#endif // __nvoc_kernel_channel_h_disabled


// Wrapper macros for halified functions
#define kchannelMap_FNPTR(pKernelChannel) pKernelChannel->__nvoc_metadata_ptr->vtable.__kchannelMap__
#define kchannelMap(pKernelChannel, pCallContext, pParams, pCpuMapping) kchannelMap_DISPATCH(pKernelChannel, pCallContext, pParams, pCpuMapping)
#define kchannelUnmap_FNPTR(pKernelChannel) pKernelChannel->__nvoc_metadata_ptr->vtable.__kchannelUnmap__
#define kchannelUnmap(pKernelChannel, pCallContext, pCpuMapping) kchannelUnmap_DISPATCH(pKernelChannel, pCallContext, pCpuMapping)
#define kchannelGetMapAddrSpace_FNPTR(pKernelChannel) pKernelChannel->__nvoc_metadata_ptr->vtable.__kchannelGetMapAddrSpace__
#define kchannelGetMapAddrSpace(pKernelChannel, pCallContext, mapFlags, pAddrSpace) kchannelGetMapAddrSpace_DISPATCH(pKernelChannel, pCallContext, mapFlags, pAddrSpace)
#define kchannelGetMemInterMapParams_FNPTR(pKernelChannel) pKernelChannel->__nvoc_metadata_ptr->vtable.__kchannelGetMemInterMapParams__
#define kchannelGetMemInterMapParams(pKernelChannel, pParams) kchannelGetMemInterMapParams_DISPATCH(pKernelChannel, pParams)
#define kchannelCheckMemInterUnmap_FNPTR(pKernelChannel) pKernelChannel->__nvoc_metadata_ptr->vtable.__kchannelCheckMemInterUnmap__
#define kchannelCheckMemInterUnmap(pKernelChannel, bSubdeviceHandleProvided) kchannelCheckMemInterUnmap_DISPATCH(pKernelChannel, bSubdeviceHandleProvided)
#define kchannelNotifyRc_HAL(pKernelChannel) kchannelNotifyRc(pKernelChannel)
#define kchannelIsSchedulable_HAL(pGpu, pKernelChannel) kchannelIsSchedulable(pGpu, pKernelChannel)
#define kchannelAllocMem_FNPTR(pKernelChannel) pKernelChannel->__kchannelAllocMem__
#define kchannelAllocMem(pGpu, pKernelChannel, Flags, verifFlags) kchannelAllocMem_DISPATCH(pGpu, pKernelChannel, Flags, verifFlags)
#define kchannelAllocMem_HAL(pGpu, pKernelChannel, Flags, verifFlags) kchannelAllocMem_DISPATCH(pGpu, pKernelChannel, Flags, verifFlags)
#define kchannelDestroyMem_FNPTR(pKernelChannel) pKernelChannel->__kchannelDestroyMem__
#define kchannelDestroyMem(pGpu, pKernelChannel) kchannelDestroyMem_DISPATCH(pGpu, pKernelChannel)
#define kchannelDestroyMem_HAL(pGpu, pKernelChannel) kchannelDestroyMem_DISPATCH(pGpu, pKernelChannel)
#define kchannelGetChannelPhysicalState_HAL(pGpu, pKernelChannel, pChannelStateParams) kchannelGetChannelPhysicalState(pGpu, pKernelChannel, pChannelStateParams)
#define kchannelEmbedRunlistID_HAL(pGpu, pKernelChannel) kchannelEmbedRunlistID(pGpu, pKernelChannel)
#define kchannelAllocHwID_FNPTR(pKernelChannel) pKernelChannel->__kchannelAllocHwID__
#define kchannelAllocHwID(pGpu, pKernelChannel, hClient, Flags, verifFlags2, ChID) kchannelAllocHwID_DISPATCH(pGpu, pKernelChannel, hClient, Flags, verifFlags2, ChID)
#define kchannelAllocHwID_HAL(pGpu, pKernelChannel, hClient, Flags, verifFlags2, ChID) kchannelAllocHwID_DISPATCH(pGpu, pKernelChannel, hClient, Flags, verifFlags2, ChID)
#define kchannelFreeHwID_FNPTR(pKernelChannel) pKernelChannel->__kchannelFreeHwID__
#define kchannelFreeHwID(pGpu, pKernelChannel) kchannelFreeHwID_DISPATCH(pGpu, pKernelChannel)
#define kchannelFreeHwID_HAL(pGpu, pKernelChannel) kchannelFreeHwID_DISPATCH(pGpu, pKernelChannel)
#define kchannelGetUserdInfo_FNPTR(arg_this) arg_this->__kchannelGetUserdInfo__
#define kchannelGetUserdInfo(pGpu, arg_this, userBase, offset, length) kchannelGetUserdInfo_DISPATCH(pGpu, arg_this, userBase, offset, length)
#define kchannelGetUserdInfo_HAL(pGpu, arg_this, userBase, offset, length) kchannelGetUserdInfo_DISPATCH(pGpu, arg_this, userBase, offset, length)
#define kchannelGetUserdBar1MapOffset_FNPTR(arg_this) arg_this->__kchannelGetUserdBar1MapOffset__
#define kchannelGetUserdBar1MapOffset(pGpu, arg_this, bar1Offset, bar1MapSize) kchannelGetUserdBar1MapOffset_DISPATCH(pGpu, arg_this, bar1Offset, bar1MapSize)
#define kchannelGetUserdBar1MapOffset_HAL(pGpu, arg_this, bar1Offset, bar1MapSize) kchannelGetUserdBar1MapOffset_DISPATCH(pGpu, arg_this, bar1Offset, bar1MapSize)
#define kchannelCreateUserdMemDescBc_FNPTR(pKernelChannel) pKernelChannel->__kchannelCreateUserdMemDescBc__
#define kchannelCreateUserdMemDescBc(pGpu, pKernelChannel, arg3, arg4, arg5) kchannelCreateUserdMemDescBc_DISPATCH(pGpu, pKernelChannel, arg3, arg4, arg5)
#define kchannelCreateUserdMemDescBc_HAL(pGpu, pKernelChannel, arg3, arg4, arg5) kchannelCreateUserdMemDescBc_DISPATCH(pGpu, pKernelChannel, arg3, arg4, arg5)
#define kchannelCreateUserdMemDesc_FNPTR(arg_this) arg_this->__kchannelCreateUserdMemDesc__
#define kchannelCreateUserdMemDesc(pGpu, arg_this, arg3, arg4, arg5, arg6, arg7) kchannelCreateUserdMemDesc_DISPATCH(pGpu, arg_this, arg3, arg4, arg5, arg6, arg7)
#define kchannelCreateUserdMemDesc_HAL(pGpu, arg_this, arg3, arg4, arg5, arg6, arg7) kchannelCreateUserdMemDesc_DISPATCH(pGpu, arg_this, arg3, arg4, arg5, arg6, arg7)
#define kchannelDestroyUserdMemDesc_FNPTR(arg_this) arg_this->__kchannelDestroyUserdMemDesc__
#define kchannelDestroyUserdMemDesc(pGpu, arg_this) kchannelDestroyUserdMemDesc_DISPATCH(pGpu, arg_this)
#define kchannelDestroyUserdMemDesc_HAL(pGpu, arg_this) kchannelDestroyUserdMemDesc_DISPATCH(pGpu, arg_this)
#define kchannelCreateUserMemDesc_FNPTR(arg_this) arg_this->__kchannelCreateUserMemDesc__
#define kchannelCreateUserMemDesc(pGpu, arg_this) kchannelCreateUserMemDesc_DISPATCH(pGpu, arg_this)
#define kchannelCreateUserMemDesc_HAL(pGpu, arg_this) kchannelCreateUserMemDesc_DISPATCH(pGpu, arg_this)
#define kchannelIsUserdAddrSizeValid_FNPTR(pKernelChannel) pKernelChannel->__kchannelIsUserdAddrSizeValid__
#define kchannelIsUserdAddrSizeValid(pKernelChannel, userdAddrLo, userdAddrHi) kchannelIsUserdAddrSizeValid_DISPATCH(pKernelChannel, userdAddrLo, userdAddrHi)
#define kchannelIsUserdAddrSizeValid_HAL(pKernelChannel, userdAddrLo, userdAddrHi) kchannelIsUserdAddrSizeValid_DISPATCH(pKernelChannel, userdAddrLo, userdAddrHi)
#define kchannelGetEngine_FNPTR(pKernelChannel) pKernelChannel->__kchannelGetEngine__
#define kchannelGetEngine(pGpu, pKernelChannel, engDesc) kchannelGetEngine_DISPATCH(pGpu, pKernelChannel, engDesc)
#define kchannelGetEngine_HAL(pGpu, pKernelChannel, engDesc) kchannelGetEngine_DISPATCH(pGpu, pKernelChannel, engDesc)
#define kchannelFwdToInternalCtrl_HAL(pGpu, pKernelChannel, internalCmd, pRmCtrlParams) kchannelFwdToInternalCtrl(pGpu, pKernelChannel, internalCmd, pRmCtrlParams)
#define kchannelAllocChannel_HAL(pKernelChannel, pChannelGpfifoParams) kchannelAllocChannel(pKernelChannel, pChannelGpfifoParams)
#define kchannelIsValid_HAL(pKernelChannel) kchannelIsValid(pKernelChannel)
#define kchannelCtrlCmdGetKmb_FNPTR(pKernelChannel) pKernelChannel->__kchannelCtrlCmdGetKmb__
#define kchannelCtrlCmdGetKmb(pKernelChannel, pGetKmbParams) kchannelCtrlCmdGetKmb_DISPATCH(pKernelChannel, pGetKmbParams)
#define kchannelCtrlCmdGetKmb_HAL(pKernelChannel, pGetKmbParams) kchannelCtrlCmdGetKmb_DISPATCH(pKernelChannel, pGetKmbParams)
#define kchannelCtrlRotateSecureChannelIv_FNPTR(pKernelChannel) pKernelChannel->__kchannelCtrlRotateSecureChannelIv__
#define kchannelCtrlRotateSecureChannelIv(pKernelChannel, pRotateIvParams) kchannelCtrlRotateSecureChannelIv_DISPATCH(pKernelChannel, pRotateIvParams)
#define kchannelCtrlRotateSecureChannelIv_HAL(pKernelChannel, pRotateIvParams) kchannelCtrlRotateSecureChannelIv_DISPATCH(pKernelChannel, pRotateIvParams)
#define kchannelSetEncryptionStatsBuffer_FNPTR(pKernelChannel) pKernelChannel->__kchannelSetEncryptionStatsBuffer__
#define kchannelSetEncryptionStatsBuffer(pGpu, pKernelChannel, pMemDesc, bSet) kchannelSetEncryptionStatsBuffer_DISPATCH(pGpu, pKernelChannel, pMemDesc, bSet)
#define kchannelSetEncryptionStatsBuffer_HAL(pGpu, pKernelChannel, pMemDesc, bSet) kchannelSetEncryptionStatsBuffer_DISPATCH(pGpu, pKernelChannel, pMemDesc, bSet)
#define kchannelGetClassEngineID_FNPTR(pKernelChannel) pKernelChannel->__kchannelGetClassEngineID__
#define kchannelGetClassEngineID(pGpu, pKernelChannel, handle, classEngineID, classID, rmEngineID) kchannelGetClassEngineID_DISPATCH(pGpu, pKernelChannel, handle, classEngineID, classID, rmEngineID)
#define kchannelGetClassEngineID_HAL(pGpu, pKernelChannel, handle, classEngineID, classID, rmEngineID) kchannelGetClassEngineID_DISPATCH(pGpu, pKernelChannel, handle, classEngineID, classID, rmEngineID)
#define kchannelEnableVirtualContext_FNPTR(arg_this) arg_this->__kchannelEnableVirtualContext__
#define kchannelEnableVirtualContext(arg_this) kchannelEnableVirtualContext_DISPATCH(arg_this)
#define kchannelEnableVirtualContext_HAL(arg_this) kchannelEnableVirtualContext_DISPATCH(arg_this)
#define kchannelRotateSecureChannelIv_HAL(pKernelChannel, rotateOperation, encryptIv, decryptIv) kchannelRotateSecureChannelIv(pKernelChannel, rotateOperation, encryptIv, decryptIv)
#define kchannelDeriveAndRetrieveKmb_FNPTR(pKernelChannel) pKernelChannel->__kchannelDeriveAndRetrieveKmb__
#define kchannelDeriveAndRetrieveKmb(pGpu, pKernelChannel, keyMaterialBundle) kchannelDeriveAndRetrieveKmb_DISPATCH(pGpu, pKernelChannel, keyMaterialBundle)
#define kchannelDeriveAndRetrieveKmb_HAL(pGpu, pKernelChannel, keyMaterialBundle) kchannelDeriveAndRetrieveKmb_DISPATCH(pGpu, pKernelChannel, keyMaterialBundle)
#define kchannelSetKeyRotationNotifier_FNPTR(pKernelChannel) pKernelChannel->__kchannelSetKeyRotationNotifier__
#define kchannelSetKeyRotationNotifier(pGpu, pKernelChannel, bSet) kchannelSetKeyRotationNotifier_DISPATCH(pGpu, pKernelChannel, bSet)
#define kchannelSetKeyRotationNotifier_HAL(pGpu, pKernelChannel, bSet) kchannelSetKeyRotationNotifier_DISPATCH(pGpu, pKernelChannel, bSet)
#define kchannelControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresControl__
#define kchannelControl(pGpuResource, pCallContext, pParams) kchannelControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define kchannelShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define kchannelShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) kchannelShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define kchannelGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define kchannelGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) kchannelGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define kchannelInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define kchannelInternalControlForward(pGpuResource, command, pParams, size) kchannelInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define kchannelGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define kchannelGetInternalObjectHandle(pGpuResource) kchannelGetInternalObjectHandle_DISPATCH(pGpuResource)
#define kchannelAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define kchannelAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) kchannelAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define kchannelGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define kchannelGetMemoryMappingDescriptor(pRmResource, ppMemDesc) kchannelGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define kchannelControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define kchannelControlSerialization_Prologue(pResource, pCallContext, pParams) kchannelControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kchannelControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define kchannelControlSerialization_Epilogue(pResource, pCallContext, pParams) kchannelControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kchannelControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define kchannelControl_Prologue(pResource, pCallContext, pParams) kchannelControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kchannelControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define kchannelControl_Epilogue(pResource, pCallContext, pParams) kchannelControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kchannelCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define kchannelCanCopy(pResource) kchannelCanCopy_DISPATCH(pResource)
#define kchannelIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define kchannelIsDuplicate(pResource, hMemory, pDuplicate) kchannelIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define kchannelPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define kchannelPreDestruct(pResource) kchannelPreDestruct_DISPATCH(pResource)
#define kchannelControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define kchannelControlFilter(pResource, pCallContext, pParams) kchannelControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define kchannelIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define kchannelIsPartialUnmapSupported(pResource) kchannelIsPartialUnmapSupported_DISPATCH(pResource)
#define kchannelMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define kchannelMapTo(pResource, pParams) kchannelMapTo_DISPATCH(pResource, pParams)
#define kchannelUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define kchannelUnmapFrom(pResource, pParams) kchannelUnmapFrom_DISPATCH(pResource, pParams)
#define kchannelGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define kchannelGetRefCount(pResource) kchannelGetRefCount_DISPATCH(pResource)
#define kchannelAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define kchannelAddAdditionalDependants(pClient, pResource, pReference) kchannelAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define kchannelGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyGetNotificationListPtr__
#define kchannelGetNotificationListPtr(pNotifier) kchannelGetNotificationListPtr_DISPATCH(pNotifier)
#define kchannelGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyGetNotificationShare__
#define kchannelGetNotificationShare(pNotifier) kchannelGetNotificationShare_DISPATCH(pNotifier)
#define kchannelSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifySetNotificationShare__
#define kchannelSetNotificationShare(pNotifier, pNotifShare) kchannelSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define kchannelUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyUnregisterEvent__
#define kchannelUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) kchannelUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define kchannelGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyGetOrAllocNotifShare__
#define kchannelGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) kchannelGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS kchannelMap_DISPATCH(struct KernelChannel *pKernelChannel, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pKernelChannel->__nvoc_metadata_ptr->vtable.__kchannelMap__(pKernelChannel, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS kchannelUnmap_DISPATCH(struct KernelChannel *pKernelChannel, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pKernelChannel->__nvoc_metadata_ptr->vtable.__kchannelUnmap__(pKernelChannel, pCallContext, pCpuMapping);
}

static inline NV_STATUS kchannelGetMapAddrSpace_DISPATCH(struct KernelChannel *pKernelChannel, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pKernelChannel->__nvoc_metadata_ptr->vtable.__kchannelGetMapAddrSpace__(pKernelChannel, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS kchannelGetMemInterMapParams_DISPATCH(struct KernelChannel *pKernelChannel, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pKernelChannel->__nvoc_metadata_ptr->vtable.__kchannelGetMemInterMapParams__(pKernelChannel, pParams);
}

static inline NV_STATUS kchannelCheckMemInterUnmap_DISPATCH(struct KernelChannel *pKernelChannel, NvBool bSubdeviceHandleProvided) {
    return pKernelChannel->__nvoc_metadata_ptr->vtable.__kchannelCheckMemInterUnmap__(pKernelChannel, bSubdeviceHandleProvided);
}

static inline NV_STATUS kchannelAllocMem_DISPATCH(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 Flags, NvU32 verifFlags) {
    return pKernelChannel->__kchannelAllocMem__(pGpu, pKernelChannel, Flags, verifFlags);
}

static inline void kchannelDestroyMem_DISPATCH(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel) {
    pKernelChannel->__kchannelDestroyMem__(pGpu, pKernelChannel);
}

static inline NV_STATUS kchannelAllocHwID_DISPATCH(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvHandle hClient, NvU32 Flags, NvU32 verifFlags2, NvU32 ChID) {
    return pKernelChannel->__kchannelAllocHwID__(pGpu, pKernelChannel, hClient, Flags, verifFlags2, ChID);
}

static inline NV_STATUS kchannelFreeHwID_DISPATCH(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel) {
    return pKernelChannel->__kchannelFreeHwID__(pGpu, pKernelChannel);
}

static inline NV_STATUS kchannelGetUserdInfo_DISPATCH(struct OBJGPU *pGpu, struct KernelChannel *arg_this, NvU64 *userBase, NvU64 *offset, NvU64 *length) {
    return arg_this->__kchannelGetUserdInfo__(pGpu, arg_this, userBase, offset, length);
}

static inline NV_STATUS kchannelGetUserdBar1MapOffset_DISPATCH(struct OBJGPU *pGpu, struct KernelChannel *arg_this, NvU64 *bar1Offset, NvU32 *bar1MapSize) {
    return arg_this->__kchannelGetUserdBar1MapOffset__(pGpu, arg_this, bar1Offset, bar1MapSize);
}

static inline NV_STATUS kchannelCreateUserdMemDescBc_DISPATCH(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvHandle arg3, NvHandle *arg4, NvU64 *arg5) {
    return pKernelChannel->__kchannelCreateUserdMemDescBc__(pGpu, pKernelChannel, arg3, arg4, arg5);
}

static inline NV_STATUS kchannelCreateUserdMemDesc_DISPATCH(struct OBJGPU *pGpu, struct KernelChannel *arg_this, NvHandle arg3, NvHandle arg4, NvU64 arg5, NvU64 *arg6, NvU32 *arg7) {
    return arg_this->__kchannelCreateUserdMemDesc__(pGpu, arg_this, arg3, arg4, arg5, arg6, arg7);
}

static inline void kchannelDestroyUserdMemDesc_DISPATCH(struct OBJGPU *pGpu, struct KernelChannel *arg_this) {
    arg_this->__kchannelDestroyUserdMemDesc__(pGpu, arg_this);
}

static inline NV_STATUS kchannelCreateUserMemDesc_DISPATCH(struct OBJGPU *pGpu, struct KernelChannel *arg_this) {
    return arg_this->__kchannelCreateUserMemDesc__(pGpu, arg_this);
}

static inline NvBool kchannelIsUserdAddrSizeValid_DISPATCH(struct KernelChannel *pKernelChannel, NvU32 userdAddrLo, NvU32 userdAddrHi) {
    return pKernelChannel->__kchannelIsUserdAddrSizeValid__(pKernelChannel, userdAddrLo, userdAddrHi);
}

static inline NV_STATUS kchannelGetEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 *engDesc) {
    return pKernelChannel->__kchannelGetEngine__(pGpu, pKernelChannel, engDesc);
}

static inline NV_STATUS kchannelCtrlCmdGetKmb_DISPATCH(struct KernelChannel *pKernelChannel, NVC56F_CTRL_CMD_GET_KMB_PARAMS *pGetKmbParams) {
    return pKernelChannel->__kchannelCtrlCmdGetKmb__(pKernelChannel, pGetKmbParams);
}

static inline NV_STATUS kchannelCtrlRotateSecureChannelIv_DISPATCH(struct KernelChannel *pKernelChannel, NVC56F_CTRL_ROTATE_SECURE_CHANNEL_IV_PARAMS *pRotateIvParams) {
    return pKernelChannel->__kchannelCtrlRotateSecureChannelIv__(pKernelChannel, pRotateIvParams);
}

static inline NV_STATUS kchannelSetEncryptionStatsBuffer_DISPATCH(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, MEMORY_DESCRIPTOR *pMemDesc, NvBool bSet) {
    return pKernelChannel->__kchannelSetEncryptionStatsBuffer__(pGpu, pKernelChannel, pMemDesc, bSet);
}

static inline NV_STATUS kchannelGetClassEngineID_DISPATCH(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvHandle handle, NvU32 *classEngineID, NvU32 *classID, RM_ENGINE_TYPE *rmEngineID) {
    return pKernelChannel->__kchannelGetClassEngineID__(pGpu, pKernelChannel, handle, classEngineID, classID, rmEngineID);
}

static inline NV_STATUS kchannelEnableVirtualContext_DISPATCH(struct KernelChannel *arg_this) {
    return arg_this->__kchannelEnableVirtualContext__(arg_this);
}

static inline NV_STATUS kchannelDeriveAndRetrieveKmb_DISPATCH(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, CC_KMB *keyMaterialBundle) {
    return pKernelChannel->__kchannelDeriveAndRetrieveKmb__(pGpu, pKernelChannel, keyMaterialBundle);
}

static inline NV_STATUS kchannelSetKeyRotationNotifier_DISPATCH(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvBool bSet) {
    return pKernelChannel->__kchannelSetKeyRotationNotifier__(pGpu, pKernelChannel, bSet);
}

static inline NV_STATUS kchannelControl_DISPATCH(struct KernelChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kchannelControl__(pGpuResource, pCallContext, pParams);
}

static inline NvBool kchannelShareCallback_DISPATCH(struct KernelChannel *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kchannelShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS kchannelGetRegBaseOffsetAndSize_DISPATCH(struct KernelChannel *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kchannelGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS kchannelInternalControlForward_DISPATCH(struct KernelChannel *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kchannelInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle kchannelGetInternalObjectHandle_DISPATCH(struct KernelChannel *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kchannelGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool kchannelAccessCallback_DISPATCH(struct KernelChannel *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__kchannelAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS kchannelGetMemoryMappingDescriptor_DISPATCH(struct KernelChannel *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__kchannelGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS kchannelControlSerialization_Prologue_DISPATCH(struct KernelChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kchannelControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void kchannelControlSerialization_Epilogue_DISPATCH(struct KernelChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__kchannelControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kchannelControl_Prologue_DISPATCH(struct KernelChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kchannelControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void kchannelControl_Epilogue_DISPATCH(struct KernelChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__kchannelControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool kchannelCanCopy_DISPATCH(struct KernelChannel *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__kchannelCanCopy__(pResource);
}

static inline NV_STATUS kchannelIsDuplicate_DISPATCH(struct KernelChannel *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__kchannelIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void kchannelPreDestruct_DISPATCH(struct KernelChannel *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__kchannelPreDestruct__(pResource);
}

static inline NV_STATUS kchannelControlFilter_DISPATCH(struct KernelChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kchannelControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool kchannelIsPartialUnmapSupported_DISPATCH(struct KernelChannel *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__kchannelIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS kchannelMapTo_DISPATCH(struct KernelChannel *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kchannelMapTo__(pResource, pParams);
}

static inline NV_STATUS kchannelUnmapFrom_DISPATCH(struct KernelChannel *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kchannelUnmapFrom__(pResource, pParams);
}

static inline NvU32 kchannelGetRefCount_DISPATCH(struct KernelChannel *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__kchannelGetRefCount__(pResource);
}

static inline void kchannelAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct KernelChannel *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__kchannelAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * kchannelGetNotificationListPtr_DISPATCH(struct KernelChannel *pNotifier) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__kchannelGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * kchannelGetNotificationShare_DISPATCH(struct KernelChannel *pNotifier) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__kchannelGetNotificationShare__(pNotifier);
}

static inline void kchannelSetNotificationShare_DISPATCH(struct KernelChannel *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvoc_metadata_ptr->vtable.__kchannelSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS kchannelUnregisterEvent_DISPATCH(struct KernelChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__kchannelUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS kchannelGetOrAllocNotifShare_DISPATCH(struct KernelChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__kchannelGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS kchannelNotifyRc_IMPL(struct KernelChannel *pKernelChannel);


NvBool kchannelIsSchedulable_IMPL(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel);


NV_STATUS kchannelGetChannelPhysicalState_KERNEL(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NV208F_CTRL_FIFO_GET_CHANNEL_STATE_PARAMS *pChannelStateParams);


static inline NvU32 kchannelEmbedRunlistID_13cd8d(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel) {
    NV_ASSERT_PRECOMP(0);
    return 0;
}


static inline NV_STATUS kchannelFwdToInternalCtrl_56cd7a(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 internalCmd, RmCtrlParams *pRmCtrlParams) {
    return NV_OK;
}


static inline NV_STATUS kchannelAllocChannel_56cd7a(struct KernelChannel *pKernelChannel, NV_CHANNEL_ALLOC_PARAMS *pChannelGpfifoParams) {
    return NV_OK;
}


static inline NvBool kchannelIsValid_88bc07(struct KernelChannel *pKernelChannel) {
    return NV_TRUE;
}


static inline NV_STATUS kchannelRotateSecureChannelIv_46f6a7(struct KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation, NvU32 *encryptIv, NvU32 *decryptIv) {
    return NV_ERR_NOT_SUPPORTED;
}


NV_STATUS kchannelMap_IMPL(struct KernelChannel *pKernelChannel, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);

NV_STATUS kchannelUnmap_IMPL(struct KernelChannel *pKernelChannel, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);

NV_STATUS kchannelGetMapAddrSpace_IMPL(struct KernelChannel *pKernelChannel, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);

NV_STATUS kchannelGetMemInterMapParams_IMPL(struct KernelChannel *pKernelChannel, RMRES_MEM_INTER_MAP_PARAMS *pParams);

NV_STATUS kchannelCheckMemInterUnmap_IMPL(struct KernelChannel *pKernelChannel, NvBool bSubdeviceHandleProvided);

NV_STATUS kchannelAllocMem_GM107(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 Flags, NvU32 verifFlags);

static inline NV_STATUS kchannelAllocMem_56cd7a(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 Flags, NvU32 verifFlags) {
    return NV_OK;
}

void kchannelDestroyMem_GM107(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel);

static inline void kchannelDestroyMem_b3696a(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel) {
    return;
}

NV_STATUS kchannelAllocHwID_GM107(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvHandle hClient, NvU32 Flags, NvU32 verifFlags2, NvU32 ChID);

static inline NV_STATUS kchannelAllocHwID_46f6a7(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvHandle hClient, NvU32 Flags, NvU32 verifFlags2, NvU32 ChID) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kchannelFreeHwID_GM107(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel);

static inline NV_STATUS kchannelFreeHwID_56cd7a(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel) {
    return NV_OK;
}

NV_STATUS kchannelGetUserdInfo_GM107(struct OBJGPU *pGpu, struct KernelChannel *arg2, NvU64 *userBase, NvU64 *offset, NvU64 *length);

static inline NV_STATUS kchannelGetUserdInfo_92bfc3(struct OBJGPU *pGpu, struct KernelChannel *arg2, NvU64 *userBase, NvU64 *offset, NvU64 *length) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kchannelGetUserdBar1MapOffset_GM107(struct OBJGPU *pGpu, struct KernelChannel *arg2, NvU64 *bar1Offset, NvU32 *bar1MapSize);

static inline NV_STATUS kchannelGetUserdBar1MapOffset_5baef9(struct OBJGPU *pGpu, struct KernelChannel *arg2, NvU64 *bar1Offset, NvU32 *bar1MapSize) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS kchannelCreateUserdMemDescBc_GV100(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvHandle arg3, NvHandle *arg4, NvU64 *arg5);

static inline NV_STATUS kchannelCreateUserdMemDescBc_56cd7a(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvHandle arg3, NvHandle *arg4, NvU64 *arg5) {
    return NV_OK;
}

NV_STATUS kchannelCreateUserdMemDesc_GV100(struct OBJGPU *pGpu, struct KernelChannel *arg2, NvHandle arg3, NvHandle arg4, NvU64 arg5, NvU64 *arg6, NvU32 *arg7);

static inline NV_STATUS kchannelCreateUserdMemDesc_56cd7a(struct OBJGPU *pGpu, struct KernelChannel *arg2, NvHandle arg3, NvHandle arg4, NvU64 arg5, NvU64 *arg6, NvU32 *arg7) {
    return NV_OK;
}

void kchannelDestroyUserdMemDesc_GV100(struct OBJGPU *pGpu, struct KernelChannel *arg2);

static inline void kchannelDestroyUserdMemDesc_b3696a(struct OBJGPU *pGpu, struct KernelChannel *arg2) {
    return;
}

NV_STATUS kchannelCreateUserMemDesc_GM107(struct OBJGPU *pGpu, struct KernelChannel *arg2);

NV_STATUS kchannelCreateUserMemDesc_GA10B(struct OBJGPU *pGpu, struct KernelChannel *arg2);

static inline NV_STATUS kchannelCreateUserMemDesc_56cd7a(struct OBJGPU *pGpu, struct KernelChannel *arg2) {
    return NV_OK;
}

NvBool kchannelIsUserdAddrSizeValid_GV100(struct KernelChannel *pKernelChannel, NvU32 userdAddrLo, NvU32 userdAddrHi);

NvBool kchannelIsUserdAddrSizeValid_GA100(struct KernelChannel *pKernelChannel, NvU32 userdAddrLo, NvU32 userdAddrHi);

NvBool kchannelIsUserdAddrSizeValid_GH100(struct KernelChannel *pKernelChannel, NvU32 userdAddrLo, NvU32 userdAddrHi);

NvBool kchannelIsUserdAddrSizeValid_GB10B(struct KernelChannel *pKernelChannel, NvU32 userdAddrLo, NvU32 userdAddrHi);

static inline NvBool kchannelIsUserdAddrSizeValid_88bc07(struct KernelChannel *pKernelChannel, NvU32 userdAddrLo, NvU32 userdAddrHi) {
    return NV_TRUE;
}

NV_STATUS kchannelGetEngine_GM107(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 *engDesc);

static inline NV_STATUS kchannelGetEngine_56cd7a(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 *engDesc) {
    return NV_OK;
}

NV_STATUS kchannelCtrlCmdResetIsolatedChannel_IMPL(struct KernelChannel *pKernelChannel, NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS *pResetParams);

NV_STATUS kchannelCtrlCmdInternalResetIsolatedChannel_IMPL(struct KernelChannel *pKernelChannel, NV506F_CTRL_CMD_INTERNAL_RESET_ISOLATED_CHANNEL_PARAMS *pResetParams);

NV_STATUS kchannelCtrlCmdGetClassEngineid_IMPL(struct KernelChannel *pKernelChannel, NV906F_CTRL_GET_CLASS_ENGINEID_PARAMS *pParams);

NV_STATUS kchannelCtrlCmdResetChannel_IMPL(struct KernelChannel *pKernelChannel, NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS *pResetChannelParams);

NV_STATUS kchannelCtrlCmdGetDeferRCState_IMPL(struct KernelChannel *pKernelChannel, NV906F_CTRL_CMD_GET_DEFER_RC_STATE_PARAMS *pStateParams);

NV_STATUS kchannelCtrlCmdGetMmuFaultInfo_IMPL(struct KernelChannel *pKernelChannel, NV906F_CTRL_GET_MMU_FAULT_INFO_PARAMS *pFaultInfoParams);

NV_STATUS kchannelCtrlCmdGpFifoSchedule_IMPL(struct KernelChannel *pKernelChannel, NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams);

NV_STATUS kchannelCtrlCmdBind_IMPL(struct KernelChannel *pKernelChannel, NVA06F_CTRL_BIND_PARAMS *pParams);

NV_STATUS kchannelCtrlCmdSetErrorNotifier_IMPL(struct KernelChannel *pKernelChannel, NVA06F_CTRL_SET_ERROR_NOTIFIER_PARAMS *pSetErrorNotifierParams);

NV_STATUS kchannelCtrlCmdSetInterleaveLevel_IMPL(struct KernelChannel *pKernelChannel, NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS *pParams);

NV_STATUS kchannelCtrlCmdGetContextId_IMPL(struct KernelChannel *pKernelChannel, NVA06F_CTRL_GET_CONTEXT_ID_PARAMS *pParams);

NV_STATUS kchannelCtrlCmdRestartRunlist_IMPL(struct KernelChannel *pKernelChannel, NVA06F_CTRL_RESTART_RUNLIST_PARAMS *pParams);

NV_STATUS kchannelCtrlCmdGetEngineCtxSize_IMPL(struct KernelChannel *pKernelChannel, NVB06F_CTRL_GET_ENGINE_CTX_SIZE_PARAMS *pCtxSizeParams);

NV_STATUS kchannelCtrlCmdGetEngineCtxData_IMPL(struct KernelChannel *pKernelChannel, NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *pCtxBuffParams);

NV_STATUS kchannelCtrlCmdMigrateEngineCtxData_IMPL(struct KernelChannel *pKernelChannel, NVB06F_CTRL_MIGRATE_ENGINE_CTX_DATA_PARAMS *pCtxBuffParams);

NV_STATUS kchannelCtrlCmdGetEngineCtxState_IMPL(struct KernelChannel *pKernelChannel, NVB06F_CTRL_GET_ENGINE_CTX_STATE_PARAMS *pCtxStateParams);

NV_STATUS kchannelCtrlCmdGetChannelHwState_IMPL(struct KernelChannel *pKernelChannel, NVB06F_CTRL_GET_CHANNEL_HW_STATE_PARAMS *pParams);

NV_STATUS kchannelCtrlCmdSetChannelHwState_IMPL(struct KernelChannel *pKernelChannel, NVB06F_CTRL_SET_CHANNEL_HW_STATE_PARAMS *pParams);

NV_STATUS kchannelCtrlCmdSaveEngineCtxData_IMPL(struct KernelChannel *pKernelChannel, NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS *pCtxBuffParams);

NV_STATUS kchannelCtrlCmdRestoreEngineCtxData_IMPL(struct KernelChannel *pKernelChannel, NVB06F_CTRL_RESTORE_ENGINE_CTX_DATA_PARAMS *pCtxBuffParams);

NV_STATUS kchannelCtrlCmdGpfifoGetWorkSubmitToken_IMPL(struct KernelChannel *pKernelChannel, NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS *pTokenParams);

NV_STATUS kchannelCtrlCmdInternalGpFifoGetWorkSubmitToken_IMPL(struct KernelChannel *pKernelChannel, NVC36F_CTRL_INTERNAL_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS *pTokenParams);

NV_STATUS kchannelCtrlCmdGpfifoUpdateFaultMethodBuffer_IMPL(struct KernelChannel *pKernelChannel, NVC36F_CTRL_GPFIFO_UPDATE_FAULT_METHOD_BUFFER_PARAMS *pFaultMthdBufferParams);

NV_STATUS kchannelCtrlCmdGpfifoSetWorkSubmitTokenNotifIndex_IMPL(struct KernelChannel *pKernelChannel, NVC36F_CTRL_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX_PARAMS *pParams);

NV_STATUS kchannelCtrlCmdStopChannel_IMPL(struct KernelChannel *pKernelChannel, NVA06F_CTRL_STOP_CHANNEL_PARAMS *pStopChannelParams);

static inline NV_STATUS kchannelCtrlCmdGetKmb_46f6a7(struct KernelChannel *pKernelChannel, NVC56F_CTRL_CMD_GET_KMB_PARAMS *pGetKmbParams) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kchannelCtrlCmdGetKmb_KERNEL(struct KernelChannel *pKernelChannel, NVC56F_CTRL_CMD_GET_KMB_PARAMS *pGetKmbParams);

static inline NV_STATUS kchannelCtrlRotateSecureChannelIv_46f6a7(struct KernelChannel *pKernelChannel, NVC56F_CTRL_ROTATE_SECURE_CHANNEL_IV_PARAMS *pRotateIvParams) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kchannelCtrlRotateSecureChannelIv_KERNEL(struct KernelChannel *pKernelChannel, NVC56F_CTRL_ROTATE_SECURE_CHANNEL_IV_PARAMS *pRotateIvParams);

NV_STATUS kchannelSetEncryptionStatsBuffer_KERNEL(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, MEMORY_DESCRIPTOR *pMemDesc, NvBool bSet);

static inline NV_STATUS kchannelSetEncryptionStatsBuffer_56cd7a(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, MEMORY_DESCRIPTOR *pMemDesc, NvBool bSet) {
    return NV_OK;
}

static inline NV_STATUS kchannelCtrlGetTpcPartitionMode_a094e1(struct KernelChannel *pKernelChannel, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams) {
    return kgrctxCtrlHandle(resservGetTlsCallContext(), pKernelChannel->hKernelGraphicsContext);
}

static inline NV_STATUS kchannelCtrlSetTpcPartitionMode_a094e1(struct KernelChannel *pKernelChannel, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams) {
    return kgrctxCtrlHandle(resservGetTlsCallContext(), pKernelChannel->hKernelGraphicsContext);
}

static inline NV_STATUS kchannelCtrlGetMMUDebugMode_a094e1(struct KernelChannel *pKernelChannel, NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS *pParams) {
    return kgrctxCtrlHandle(resservGetTlsCallContext(), pKernelChannel->hKernelGraphicsContext);
}

static inline NV_STATUS kchannelCtrlProgramVidmemPromote_a094e1(struct KernelChannel *pKernelChannel, NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *pParams) {
    return kgrctxCtrlHandle(resservGetTlsCallContext(), pKernelChannel->hKernelGraphicsContext);
}

static inline NV_STATUS kchannelCtrlSetLgSectorPromotion_a094e1(struct KernelChannel *pKernelChannel, NV0090_CTRL_SET_LG_SECTOR_PROMOTION_PARAMS *pParams) {
    return kgrctxCtrlHandle(resservGetTlsCallContext(), pKernelChannel->hKernelGraphicsContext);
}

NV_STATUS kchannelGetClassEngineID_GM107(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvHandle handle, NvU32 *classEngineID, NvU32 *classID, RM_ENGINE_TYPE *rmEngineID);

static inline NV_STATUS kchannelGetClassEngineID_46f6a7(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvHandle handle, NvU32 *classEngineID, NvU32 *classID, RM_ENGINE_TYPE *rmEngineID) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kchannelEnableVirtualContext_GM107(struct KernelChannel *arg1);

static inline NV_STATUS kchannelEnableVirtualContext_46f6a7(struct KernelChannel *arg1) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kchannelDeriveAndRetrieveKmb_56cd7a(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, CC_KMB *keyMaterialBundle) {
    return NV_OK;
}

NV_STATUS kchannelDeriveAndRetrieveKmb_KERNEL(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, CC_KMB *keyMaterialBundle);

NV_STATUS kchannelSetKeyRotationNotifier_KERNEL(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvBool bSet);

static inline NV_STATUS kchannelSetKeyRotationNotifier_56cd7a(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvBool bSet) {
    return NV_OK;
}

static inline NvU32 kchannelGetDebugTag(const struct KernelChannel *pKernelChannel) {
    if (pKernelChannel == ((void *)0))
        return 4294967295U;
    return (pKernelChannel->runlistId << 24) | pKernelChannel->ChID;
}

static inline NvBool kchannelIsCtxBufferAllocSkipped(struct KernelChannel *pKernelChannel) {
    return pKernelChannel->bSkipCtxBufferAlloc;
}

static inline NvU32 kchannelGetSubctxId(struct KernelChannel *pKernelChannel) {
    return pKernelChannel->subctxId;
}

static inline NvU32 kchannelGetVaSpaceId(struct KernelChannel *pKernelChannel) {
    return pKernelChannel->vaSpaceId;
}

static inline NvU32 kchannelGetCid(struct KernelChannel *pKernelChannel) {
    return pKernelChannel->cid;
}

static inline struct MIG_INSTANCE_REF *kchannelGetMIGReference(struct KernelChannel *pKernelChannel) {
    return &pKernelChannel->partitionRef;
}

static inline NvU32 kchannelGetRunqueue(struct KernelChannel *pKernelChannel) {
    return pKernelChannel->runqueue;
}

static inline NvU32 kchannelGetRunlistId(struct KernelChannel *pKernelChannel) {
    return pKernelChannel->runlistId;
}

static inline void kchannelSetRunlistId(struct KernelChannel *pKernelChannel, NvU32 runlistId) {
    pKernelChannel->runlistId = runlistId;
}

static inline RM_ENGINE_TYPE kchannelGetEngineType(struct KernelChannel *pKernelChannel) {
    return pKernelChannel->engineType;
}

#undef PRIVATE_FIELD

#ifndef NVOC_KERNEL_CHANNEL_H_PRIVATE_ACCESS_ALLOWED
#undef kchannelDeriveAndRetrieveKmb
NV_STATUS NVOC_PRIVATE_FUNCTION(kchannelDeriveAndRetrieveKmb)(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, CC_KMB *keyMaterialBundle);

#undef kchannelDeriveAndRetrieveKmb_HAL
NV_STATUS NVOC_PRIVATE_FUNCTION(kchannelDeriveAndRetrieveKmb_HAL)(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, CC_KMB *keyMaterialBundle);

#undef kchannelSetKeyRotationNotifier
NV_STATUS NVOC_PRIVATE_FUNCTION(kchannelSetKeyRotationNotifier)(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvBool bSet);

#undef kchannelSetKeyRotationNotifier_HAL
NV_STATUS NVOC_PRIVATE_FUNCTION(kchannelSetKeyRotationNotifier_HAL)(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvBool bSet);

#ifndef __nvoc_kernel_channel_h_disabled
#undef kchannelRotateSecureChannelIv
NV_STATUS NVOC_PRIVATE_FUNCTION(kchannelRotateSecureChannelIv)(struct KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation, NvU32 *encryptIv, NvU32 *decryptIv);
#endif //__nvoc_kernel_channel_h_disabled

#endif // NVOC_KERNEL_CHANNEL_H_PRIVATE_ACCESS_ALLOWED


RS_ORDERED_ITERATOR kchannelGetIter(
    struct RsClient *pClient,
    RsResourceRef *pScopeRef);

NV_STATUS kchannelGetNextKernelChannel(
    struct OBJGPU *pGpu,
    CHANNEL_ITERATOR *pIt,
    struct KernelChannel **ppKernelChannel);

NV_STATUS CliGetKernelChannelWithDevice(struct RsClient       *pClient,
                                        NvHandle        hParent,
                                        NvHandle        hKernelChannel,
                                        struct KernelChannel **ppKernelChannel);

NV_STATUS CliGetKernelChannel(struct RsClient       *pClient,
                              NvHandle        hKernelChannel,
                              struct KernelChannel **ppKernelChannel);

/*!
 * @brief Helper to get type and memdesc of a channel notifier (memory/ctxdma)
 */
NV_STATUS kchannelGetNotifierInfo(struct OBJGPU *pGpu,
                                  Device *pDevice,
                                  NvHandle hErrorContext,
                                  MEMORY_DESCRIPTOR **ppMemDesc,
                                  ErrorNotifierType *pNotifierType,
                                  NvU64 *pOffset);

// Utils to iterate over ChannelDescendants on one Channels
void kchannelGetChildIterator(struct KernelChannel *pKernelChannel,
                              NvU32 classID,
                              RM_ENGINE_TYPE engineID,
                              KernelChannelChildIterator *pIter);
ChannelDescendant *kchannelGetNextChild(KernelChannelChildIterator *pIter);
// Simpler function to call if you just need one result
ChannelDescendant *kchannelGetOneChild(struct KernelChannel *pKernelChannel,
                                       NvU32 classID,
                                       NvU32 engineID);

// Utils to iterate over ChannelDescendants on all Channels in the same ChannelGroup
void kchannelGetChildIterOverGroup(struct KernelChannel *pKernelChannel,
                                   NvU32 classNum,
                                   NvU32 engDesc,
                                   KernelChannelChildIterOverGroup *pIt);
ChannelDescendant *kchannelGetNextChildOverGroup(KernelChannelChildIterOverGroup *pIt);

NV_STATUS kchannelFindChildByHandle(struct KernelChannel *pKernelChannel, NvHandle hResource, ChannelDescendant **ppObject);

// Bitmap for KernelChannel->swState
#define KERNEL_CHANNEL_SW_STATE_CPU_MAP                    NVBIT(0) //UserD is mapped
#define KERNEL_CHANNEL_SW_STATE_RUNLIST_SET                NVBIT(1) // RunlistId is set
#define KERNEL_CHANNEL_SW_STATE_DISABLED_FOR_KEY_ROTATION  NVBIT(2) // disabled for key rotation
#define KERNEL_CHANNEL_SW_STATE_ENABLE_AFTER_KEY_ROTATION  NVBIT(3) // RM should enable after key rotation

NvBool kchannelIsCpuMapped(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel);
void kchannelSetCpuMapped(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvBool bCpuMapped);
NvBool kchannelIsRunlistSet(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel);
void kchannelSetRunlistSet(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvBool bRunlistSet);
NvBool kchannelIsDisabledForKeyRotation(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel);
void kchannelDisableForKeyRotation(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvBool bDisable);
NvBool kchannelIsEnableAfterKeyRotation(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel);
void kchannelEnableAfterKeyRotation(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvBool bEnable);

#endif // KERNEL_CHANNEL_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_CHANNEL_NVOC_H_
