#ifndef _G_KERNEL_CHANNEL_NVOC_H_
#define _G_KERNEL_CHANNEL_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_kernel_channel_nvoc.h"

#ifndef KERNEL_CHANNEL_H
#define KERNEL_CHANNEL_H

#include "core/core.h"
#include "os/os.h"
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
#include "ctrl/ctrla16f.h"
#include "ctrl/ctrla26f.h"
#include "ctrl/ctrlb06f.h"
#include "ctrl/ctrlc06f.h"
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
    NvU32 eventActionDisable;   // event disable action cmd value
    NvU32 eventActionSingle;    // event single-shot enable action cmd value
    NvU32 eventActionRepeat;    // event repeat enable action cmd value
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

/*!
 * Class for the kernel side of a Channel object.
 */
#ifdef NVOC_KERNEL_CHANNEL_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelChannel {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Notifier __nvoc_base_Notifier;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct INotifier *__nvoc_pbase_INotifier;
    struct Notifier *__nvoc_pbase_Notifier;
    struct KernelChannel *__nvoc_pbase_KernelChannel;
    NV_STATUS (*__kchannelMap__)(struct KernelChannel *, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NV_STATUS (*__kchannelUnmap__)(struct KernelChannel *, CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__kchannelGetMapAddrSpace__)(struct KernelChannel *, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NV_STATUS (*__kchannelGetMemInterMapParams__)(struct KernelChannel *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__kchannelCheckMemInterUnmap__)(struct KernelChannel *, NvBool);
    NV_STATUS (*__kchannelCreateUserMemDesc__)(struct OBJGPU *, struct KernelChannel *);
    NvBool (*__kchannelIsUserdAddrSizeValid__)(struct KernelChannel *, NvU32, NvU32);
    NV_STATUS (*__kchannelCtrlCmdResetIsolatedChannel__)(struct KernelChannel *, NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdGetClassEngineid__)(struct KernelChannel *, NV906F_CTRL_GET_CLASS_ENGINEID_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdResetChannel__)(struct KernelChannel *, NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdGetDeferRCState__)(struct KernelChannel *, NV906F_CTRL_CMD_GET_DEFER_RC_STATE_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdGetMmuFaultInfo__)(struct KernelChannel *, NV906F_CTRL_GET_MMU_FAULT_INFO_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdEventSetNotification__)(struct KernelChannel *, NV906F_CTRL_EVENT_SET_NOTIFICATION_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdGetClassEngineidA06F__)(struct KernelChannel *, NVA06F_CTRL_GET_CLASS_ENGINEID_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdResetChannelA06F__)(struct KernelChannel *, NVA06F_CTRL_CMD_RESET_CHANNEL_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdGpFifoSchedule__)(struct KernelChannel *, NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdBind__)(struct KernelChannel *, NVA06F_CTRL_BIND_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdGetMmuFaultInfoA06F__)(struct KernelChannel *, NVA06F_CTRL_GET_MMU_FAULT_INFO_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdSetErrorNotifier__)(struct KernelChannel *, NVA06F_CTRL_SET_ERROR_NOTIFIER_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdSetInterleaveLevel__)(struct KernelChannel *, NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdRestartRunlist__)(struct KernelChannel *, NVA06F_CTRL_RESTART_RUNLIST_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdGetClassEngineidA16F__)(struct KernelChannel *, NVA16F_CTRL_GET_CLASS_ENGINEID_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdResetChannelA16F__)(struct KernelChannel *, NVA16F_CTRL_CMD_RESET_CHANNEL_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdGpFifoScheduleA16F__)(struct KernelChannel *, NVA16F_CTRL_GPFIFO_SCHEDULE_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdGetClassEngineidA26F__)(struct KernelChannel *, NVA26F_CTRL_GET_CLASS_ENGINEID_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdResetChannelA26F__)(struct KernelChannel *, NVA26F_CTRL_CMD_RESET_CHANNEL_PARAMS *);
    NV_STATUS (*__kchannelFCtrlCmdGpFifoScheduleA26F__)(struct KernelChannel *, NVA26F_CTRL_GPFIFO_SCHEDULE_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdGetClassEngineidB06F__)(struct KernelChannel *, NVB06F_CTRL_GET_CLASS_ENGINEID_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdResetChannelB06F__)(struct KernelChannel *, NVB06F_CTRL_CMD_RESET_CHANNEL_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdGpFifoScheduleB06F__)(struct KernelChannel *, NVB06F_CTRL_GPFIFO_SCHEDULE_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdBindB06F__)(struct KernelChannel *, NVB06F_CTRL_BIND_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdGetEngineCtxSize__)(struct KernelChannel *, NVB06F_CTRL_GET_ENGINE_CTX_SIZE_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdGetEngineCtxData__)(struct KernelChannel *, NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdMigrateEngineCtxData__)(struct KernelChannel *, NVB06F_CTRL_MIGRATE_ENGINE_CTX_DATA_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdGetEngineCtxState__)(struct KernelChannel *, NVB06F_CTRL_GET_ENGINE_CTX_STATE_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdGetChannelHwState__)(struct KernelChannel *, NVB06F_CTRL_GET_CHANNEL_HW_STATE_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdSetChannelHwState__)(struct KernelChannel *, NVB06F_CTRL_SET_CHANNEL_HW_STATE_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdSaveEngineCtxData__)(struct KernelChannel *, NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdRestoreEngineCtxData__)(struct KernelChannel *, NVB06F_CTRL_RESTORE_ENGINE_CTX_DATA_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdGetClassEngineidC06F__)(struct KernelChannel *, NVC06F_CTRL_GET_CLASS_ENGINEID_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdResetChannelC06F__)(struct KernelChannel *, NVC06F_CTRL_CMD_RESET_CHANNEL_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdGpFifoScheduleC06F__)(struct KernelChannel *, NVC06F_CTRL_GPFIFO_SCHEDULE_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdBindC06F__)(struct KernelChannel *, NVC06F_CTRL_BIND_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdGetClassEngineidC36F__)(struct KernelChannel *, NVC36F_CTRL_GET_CLASS_ENGINEID_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdResetChannelC36F__)(struct KernelChannel *, NVC36F_CTRL_CMD_RESET_CHANNEL_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdGpFifoScheduleC36F__)(struct KernelChannel *, NVC36F_CTRL_GPFIFO_SCHEDULE_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdBindC36F__)(struct KernelChannel *, NVC36F_CTRL_BIND_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdGpfifoGetWorkSubmitToken__)(struct KernelChannel *, NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdGpfifoUpdateFaultMethodBuffer__)(struct KernelChannel *, NVC36F_CTRL_GPFIFO_UPDATE_FAULT_METHOD_BUFFER_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdGpfifoSetWorkSubmitTokenNotifIndex__)(struct KernelChannel *, NVC36F_CTRL_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdStopChannel__)(struct KernelChannel *, NVA06F_CTRL_STOP_CHANNEL_PARAMS *);
    NV_STATUS (*__kchannelCtrlCmdGetKmb__)(struct KernelChannel *, NVC56F_CTRL_CMD_GET_KMB_PARAMS *);
    NV_STATUS (*__kchannelCtrlRotateSecureChannelIv__)(struct KernelChannel *, NVC56F_CTRL_ROTATE_SECURE_CHANNEL_IV_PARAMS *);
    NV_STATUS (*__kchannelCtrlGetTpcPartitionMode__)(struct KernelChannel *, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *);
    NV_STATUS (*__kchannelCtrlSetTpcPartitionMode__)(struct KernelChannel *, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *);
    NV_STATUS (*__kchannelCtrlGetMMUDebugMode__)(struct KernelChannel *, NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS *);
    NV_STATUS (*__kchannelCtrlProgramVidmemPromote__)(struct KernelChannel *, NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *);
    NvBool (*__kchannelShareCallback__)(struct KernelChannel *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__kchannelGetOrAllocNotifShare__)(struct KernelChannel *, NvHandle, NvHandle, struct NotifShare **);
    NV_STATUS (*__kchannelMapTo__)(struct KernelChannel *, RS_RES_MAP_TO_PARAMS *);
    void (*__kchannelSetNotificationShare__)(struct KernelChannel *, struct NotifShare *);
    NvU32 (*__kchannelGetRefCount__)(struct KernelChannel *);
    void (*__kchannelAddAdditionalDependants__)(struct RsClient *, struct KernelChannel *, RsResourceRef *);
    NV_STATUS (*__kchannelControl_Prologue__)(struct KernelChannel *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kchannelGetRegBaseOffsetAndSize__)(struct KernelChannel *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__kchannelInternalControlForward__)(struct KernelChannel *, NvU32, void *, NvU32);
    NV_STATUS (*__kchannelUnmapFrom__)(struct KernelChannel *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__kchannelControl_Epilogue__)(struct KernelChannel *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kchannelControlLookup__)(struct KernelChannel *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__kchannelGetInternalObjectHandle__)(struct KernelChannel *);
    NV_STATUS (*__kchannelControl__)(struct KernelChannel *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kchannelGetMemoryMappingDescriptor__)(struct KernelChannel *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__kchannelControlFilter__)(struct KernelChannel *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kchannelUnregisterEvent__)(struct KernelChannel *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__kchannelControlSerialization_Prologue__)(struct KernelChannel *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__kchannelCanCopy__)(struct KernelChannel *);
    void (*__kchannelPreDestruct__)(struct KernelChannel *);
    NV_STATUS (*__kchannelIsDuplicate__)(struct KernelChannel *, NvHandle, NvBool *);
    void (*__kchannelControlSerialization_Epilogue__)(struct KernelChannel *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__kchannelGetNotificationListPtr__)(struct KernelChannel *);
    struct NotifShare *(*__kchannelGetNotificationShare__)(struct KernelChannel *);
    NvBool (*__kchannelAccessCallback__)(struct KernelChannel *, struct RsClient *, void *, RsAccessRight);
    NvU16 nextObjectClassID;
    struct KernelChannel *pNextBindKernelChannel;
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
    NvBool bIsContextBound;
    FIFO_INSTANCE_BLOCK *pFifoHalData[8];
    MEMORY_DESCRIPTOR *pInstSubDeviceMemDesc[8];
    MEMORY_DESCRIPTOR *pUserdSubDeviceMemDesc[8];
    NvBool bClientAllocatedUserD;
    NvU32 swState[8];
    NvU32 ProcessID;
    NvU32 SubProcessID;
    NvU32 bcStateCurrent;
    NvU32 notifyIndex[2];
    NvU32 *pNotifyActions;
    NvU64 userdLength;
    NvBool bSkipCtxBufferAlloc;
    NvU32 subctxId;
    NvU32 cid;
    struct MIG_INSTANCE_REF partitionRef;
    NvU32 runqueue;
    RM_ENGINE_TYPE engineType;
    CC_KMB clientKmb;
    NvBool bCCSecureChannel;
};

#ifndef __NVOC_CLASS_KernelChannel_TYPEDEF__
#define __NVOC_CLASS_KernelChannel_TYPEDEF__
typedef struct KernelChannel KernelChannel;
#endif /* __NVOC_CLASS_KernelChannel_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelChannel
#define __nvoc_class_id_KernelChannel 0x5d8d70
#endif /* __nvoc_class_id_KernelChannel */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelChannel;

#define __staticCast_KernelChannel(pThis) \
    ((pThis)->__nvoc_pbase_KernelChannel)

#ifdef __nvoc_kernel_channel_h_disabled
#define __dynamicCast_KernelChannel(pThis) ((KernelChannel*)NULL)
#else //__nvoc_kernel_channel_h_disabled
#define __dynamicCast_KernelChannel(pThis) \
    ((KernelChannel*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelChannel)))
#endif //__nvoc_kernel_channel_h_disabled


NV_STATUS __nvoc_objCreateDynamic_KernelChannel(KernelChannel**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelChannel(KernelChannel**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_KernelChannel(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_KernelChannel((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define kchannelMap(pKernelChannel, pCallContext, pParams, pCpuMapping) kchannelMap_DISPATCH(pKernelChannel, pCallContext, pParams, pCpuMapping)
#define kchannelUnmap(pKernelChannel, pCallContext, pCpuMapping) kchannelUnmap_DISPATCH(pKernelChannel, pCallContext, pCpuMapping)
#define kchannelGetMapAddrSpace(pKernelChannel, pCallContext, mapFlags, pAddrSpace) kchannelGetMapAddrSpace_DISPATCH(pKernelChannel, pCallContext, mapFlags, pAddrSpace)
#define kchannelGetMemInterMapParams(pKernelChannel, pParams) kchannelGetMemInterMapParams_DISPATCH(pKernelChannel, pParams)
#define kchannelCheckMemInterUnmap(pKernelChannel, bSubdeviceHandleProvided) kchannelCheckMemInterUnmap_DISPATCH(pKernelChannel, bSubdeviceHandleProvided)
#define kchannelCreateUserMemDesc(pGpu, arg0) kchannelCreateUserMemDesc_DISPATCH(pGpu, arg0)
#define kchannelCreateUserMemDesc_HAL(pGpu, arg0) kchannelCreateUserMemDesc_DISPATCH(pGpu, arg0)
#define kchannelIsUserdAddrSizeValid(pKernelChannel, userdAddrLo, userdAddrHi) kchannelIsUserdAddrSizeValid_DISPATCH(pKernelChannel, userdAddrLo, userdAddrHi)
#define kchannelIsUserdAddrSizeValid_HAL(pKernelChannel, userdAddrLo, userdAddrHi) kchannelIsUserdAddrSizeValid_DISPATCH(pKernelChannel, userdAddrLo, userdAddrHi)
#define kchannelCtrlCmdResetIsolatedChannel(pKernelChannel, pResetParams) kchannelCtrlCmdResetIsolatedChannel_DISPATCH(pKernelChannel, pResetParams)
#define kchannelCtrlCmdGetClassEngineid(pKernelChannel, pParams) kchannelCtrlCmdGetClassEngineid_DISPATCH(pKernelChannel, pParams)
#define kchannelCtrlCmdResetChannel(pKernelChannel, pResetChannelParams) kchannelCtrlCmdResetChannel_DISPATCH(pKernelChannel, pResetChannelParams)
#define kchannelCtrlCmdGetDeferRCState(pKernelChannel, pStateParams) kchannelCtrlCmdGetDeferRCState_DISPATCH(pKernelChannel, pStateParams)
#define kchannelCtrlCmdGetMmuFaultInfo(pKernelChannel, pFaultInfoParams) kchannelCtrlCmdGetMmuFaultInfo_DISPATCH(pKernelChannel, pFaultInfoParams)
#define kchannelCtrlCmdEventSetNotification(pKernelChannel, pSetEventParams) kchannelCtrlCmdEventSetNotification_DISPATCH(pKernelChannel, pSetEventParams)
#define kchannelCtrlCmdGetClassEngineidA06F(pKernelChannel, pParams) kchannelCtrlCmdGetClassEngineidA06F_DISPATCH(pKernelChannel, pParams)
#define kchannelCtrlCmdResetChannelA06F(pKernelChannel, pResetChannelParams) kchannelCtrlCmdResetChannelA06F_DISPATCH(pKernelChannel, pResetChannelParams)
#define kchannelCtrlCmdGpFifoSchedule(pKernelChannel, pSchedParams) kchannelCtrlCmdGpFifoSchedule_DISPATCH(pKernelChannel, pSchedParams)
#define kchannelCtrlCmdBind(pKernelChannel, pParams) kchannelCtrlCmdBind_DISPATCH(pKernelChannel, pParams)
#define kchannelCtrlCmdGetMmuFaultInfoA06F(pKernelChannel, pFaultInfoParams) kchannelCtrlCmdGetMmuFaultInfoA06F_DISPATCH(pKernelChannel, pFaultInfoParams)
#define kchannelCtrlCmdSetErrorNotifier(pKernelChannel, pSetErrorNotifierParams) kchannelCtrlCmdSetErrorNotifier_DISPATCH(pKernelChannel, pSetErrorNotifierParams)
#define kchannelCtrlCmdSetInterleaveLevel(pKernelChannel, pParams) kchannelCtrlCmdSetInterleaveLevel_DISPATCH(pKernelChannel, pParams)
#define kchannelCtrlCmdRestartRunlist(pKernelChannel, pParams) kchannelCtrlCmdRestartRunlist_DISPATCH(pKernelChannel, pParams)
#define kchannelCtrlCmdGetClassEngineidA16F(pKernelChannel, pParams) kchannelCtrlCmdGetClassEngineidA16F_DISPATCH(pKernelChannel, pParams)
#define kchannelCtrlCmdResetChannelA16F(pKernelChannel, pResetChannelParams) kchannelCtrlCmdResetChannelA16F_DISPATCH(pKernelChannel, pResetChannelParams)
#define kchannelCtrlCmdGpFifoScheduleA16F(pKernelChannel, pSchedParams) kchannelCtrlCmdGpFifoScheduleA16F_DISPATCH(pKernelChannel, pSchedParams)
#define kchannelCtrlCmdGetClassEngineidA26F(pKernelChannel, pParams) kchannelCtrlCmdGetClassEngineidA26F_DISPATCH(pKernelChannel, pParams)
#define kchannelCtrlCmdResetChannelA26F(pKernelChannel, pResetChannelParams) kchannelCtrlCmdResetChannelA26F_DISPATCH(pKernelChannel, pResetChannelParams)
#define kchannelFCtrlCmdGpFifoScheduleA26F(pKernelChannel, pSchedParams) kchannelFCtrlCmdGpFifoScheduleA26F_DISPATCH(pKernelChannel, pSchedParams)
#define kchannelCtrlCmdGetClassEngineidB06F(pKernelChannel, pParams) kchannelCtrlCmdGetClassEngineidB06F_DISPATCH(pKernelChannel, pParams)
#define kchannelCtrlCmdResetChannelB06F(pKernelChannel, pResetChannelParams) kchannelCtrlCmdResetChannelB06F_DISPATCH(pKernelChannel, pResetChannelParams)
#define kchannelCtrlCmdGpFifoScheduleB06F(pKernelChannel, pSchedParams) kchannelCtrlCmdGpFifoScheduleB06F_DISPATCH(pKernelChannel, pSchedParams)
#define kchannelCtrlCmdBindB06F(pKernelChannel, pParams) kchannelCtrlCmdBindB06F_DISPATCH(pKernelChannel, pParams)
#define kchannelCtrlCmdGetEngineCtxSize(pKernelChannel, pCtxSizeParams) kchannelCtrlCmdGetEngineCtxSize_DISPATCH(pKernelChannel, pCtxSizeParams)
#define kchannelCtrlCmdGetEngineCtxData(pKernelChannel, pCtxBuffParams) kchannelCtrlCmdGetEngineCtxData_DISPATCH(pKernelChannel, pCtxBuffParams)
#define kchannelCtrlCmdMigrateEngineCtxData(pKernelChannel, pCtxBuffParams) kchannelCtrlCmdMigrateEngineCtxData_DISPATCH(pKernelChannel, pCtxBuffParams)
#define kchannelCtrlCmdGetEngineCtxState(pKernelChannel, pCtxStateParams) kchannelCtrlCmdGetEngineCtxState_DISPATCH(pKernelChannel, pCtxStateParams)
#define kchannelCtrlCmdGetChannelHwState(pKernelChannel, pParams) kchannelCtrlCmdGetChannelHwState_DISPATCH(pKernelChannel, pParams)
#define kchannelCtrlCmdSetChannelHwState(pKernelChannel, pParams) kchannelCtrlCmdSetChannelHwState_DISPATCH(pKernelChannel, pParams)
#define kchannelCtrlCmdSaveEngineCtxData(pKernelChannel, pCtxBuffParams) kchannelCtrlCmdSaveEngineCtxData_DISPATCH(pKernelChannel, pCtxBuffParams)
#define kchannelCtrlCmdRestoreEngineCtxData(pKernelChannel, pCtxBuffParams) kchannelCtrlCmdRestoreEngineCtxData_DISPATCH(pKernelChannel, pCtxBuffParams)
#define kchannelCtrlCmdGetClassEngineidC06F(pKernelChannel, pParams) kchannelCtrlCmdGetClassEngineidC06F_DISPATCH(pKernelChannel, pParams)
#define kchannelCtrlCmdResetChannelC06F(pKernelChannel, pResetChannelParams) kchannelCtrlCmdResetChannelC06F_DISPATCH(pKernelChannel, pResetChannelParams)
#define kchannelCtrlCmdGpFifoScheduleC06F(pKernelChannel, pSchedParams) kchannelCtrlCmdGpFifoScheduleC06F_DISPATCH(pKernelChannel, pSchedParams)
#define kchannelCtrlCmdBindC06F(pKernelChannel, pParams) kchannelCtrlCmdBindC06F_DISPATCH(pKernelChannel, pParams)
#define kchannelCtrlCmdGetClassEngineidC36F(pKernelChannel, pParams) kchannelCtrlCmdGetClassEngineidC36F_DISPATCH(pKernelChannel, pParams)
#define kchannelCtrlCmdResetChannelC36F(pKernelChannel, pResetChannelParams) kchannelCtrlCmdResetChannelC36F_DISPATCH(pKernelChannel, pResetChannelParams)
#define kchannelCtrlCmdGpFifoScheduleC36F(pKernelChannel, pSchedParams) kchannelCtrlCmdGpFifoScheduleC36F_DISPATCH(pKernelChannel, pSchedParams)
#define kchannelCtrlCmdBindC36F(pKernelChannel, pParams) kchannelCtrlCmdBindC36F_DISPATCH(pKernelChannel, pParams)
#define kchannelCtrlCmdGpfifoGetWorkSubmitToken(pKernelChannel, pTokenParams) kchannelCtrlCmdGpfifoGetWorkSubmitToken_DISPATCH(pKernelChannel, pTokenParams)
#define kchannelCtrlCmdGpfifoUpdateFaultMethodBuffer(pKernelChannel, pFaultMthdBufferParams) kchannelCtrlCmdGpfifoUpdateFaultMethodBuffer_DISPATCH(pKernelChannel, pFaultMthdBufferParams)
#define kchannelCtrlCmdGpfifoSetWorkSubmitTokenNotifIndex(pKernelChannel, pParams) kchannelCtrlCmdGpfifoSetWorkSubmitTokenNotifIndex_DISPATCH(pKernelChannel, pParams)
#define kchannelCtrlCmdStopChannel(pKernelChannel, pStopChannelParams) kchannelCtrlCmdStopChannel_DISPATCH(pKernelChannel, pStopChannelParams)
#define kchannelCtrlCmdGetKmb(pKernelChannel, pGetKmbParams) kchannelCtrlCmdGetKmb_DISPATCH(pKernelChannel, pGetKmbParams)
#define kchannelCtrlCmdGetKmb_HAL(pKernelChannel, pGetKmbParams) kchannelCtrlCmdGetKmb_DISPATCH(pKernelChannel, pGetKmbParams)
#define kchannelCtrlRotateSecureChannelIv(pKernelChannel, pRotateIvParams) kchannelCtrlRotateSecureChannelIv_DISPATCH(pKernelChannel, pRotateIvParams)
#define kchannelCtrlRotateSecureChannelIv_HAL(pKernelChannel, pRotateIvParams) kchannelCtrlRotateSecureChannelIv_DISPATCH(pKernelChannel, pRotateIvParams)
#define kchannelCtrlGetTpcPartitionMode(pKernelChannel, pParams) kchannelCtrlGetTpcPartitionMode_DISPATCH(pKernelChannel, pParams)
#define kchannelCtrlSetTpcPartitionMode(pKernelChannel, pParams) kchannelCtrlSetTpcPartitionMode_DISPATCH(pKernelChannel, pParams)
#define kchannelCtrlGetMMUDebugMode(pKernelChannel, pParams) kchannelCtrlGetMMUDebugMode_DISPATCH(pKernelChannel, pParams)
#define kchannelCtrlProgramVidmemPromote(pKernelChannel, pParams) kchannelCtrlProgramVidmemPromote_DISPATCH(pKernelChannel, pParams)
#define kchannelShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) kchannelShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define kchannelGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) kchannelGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
#define kchannelMapTo(pResource, pParams) kchannelMapTo_DISPATCH(pResource, pParams)
#define kchannelSetNotificationShare(pNotifier, pNotifShare) kchannelSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define kchannelGetRefCount(pResource) kchannelGetRefCount_DISPATCH(pResource)
#define kchannelAddAdditionalDependants(pClient, pResource, pReference) kchannelAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define kchannelControl_Prologue(pResource, pCallContext, pParams) kchannelControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kchannelGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) kchannelGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define kchannelInternalControlForward(pGpuResource, command, pParams, size) kchannelInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define kchannelUnmapFrom(pResource, pParams) kchannelUnmapFrom_DISPATCH(pResource, pParams)
#define kchannelControl_Epilogue(pResource, pCallContext, pParams) kchannelControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kchannelControlLookup(pResource, pParams, ppEntry) kchannelControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define kchannelGetInternalObjectHandle(pGpuResource) kchannelGetInternalObjectHandle_DISPATCH(pGpuResource)
#define kchannelControl(pGpuResource, pCallContext, pParams) kchannelControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define kchannelGetMemoryMappingDescriptor(pRmResource, ppMemDesc) kchannelGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define kchannelControlFilter(pResource, pCallContext, pParams) kchannelControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define kchannelUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) kchannelUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define kchannelControlSerialization_Prologue(pResource, pCallContext, pParams) kchannelControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kchannelCanCopy(pResource) kchannelCanCopy_DISPATCH(pResource)
#define kchannelPreDestruct(pResource) kchannelPreDestruct_DISPATCH(pResource)
#define kchannelIsDuplicate(pResource, hMemory, pDuplicate) kchannelIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define kchannelControlSerialization_Epilogue(pResource, pCallContext, pParams) kchannelControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kchannelGetNotificationListPtr(pNotifier) kchannelGetNotificationListPtr_DISPATCH(pNotifier)
#define kchannelGetNotificationShare(pNotifier) kchannelGetNotificationShare_DISPATCH(pNotifier)
#define kchannelAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) kchannelAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS kchannelNotifyRc_IMPL(struct KernelChannel *pKernelChannel);


#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelNotifyRc(struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelNotifyRc(pKernelChannel) kchannelNotifyRc_IMPL(pKernelChannel)
#endif //__nvoc_kernel_channel_h_disabled

#define kchannelNotifyRc_HAL(pKernelChannel) kchannelNotifyRc(pKernelChannel)

NvBool kchannelIsSchedulable_IMPL(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel);


#ifdef __nvoc_kernel_channel_h_disabled
static inline NvBool kchannelIsSchedulable(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelIsSchedulable(pGpu, pKernelChannel) kchannelIsSchedulable_IMPL(pGpu, pKernelChannel)
#endif //__nvoc_kernel_channel_h_disabled

#define kchannelIsSchedulable_HAL(pGpu, pKernelChannel) kchannelIsSchedulable(pGpu, pKernelChannel)

NV_STATUS kchannelAllocMem_GM107(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 Flags, NvU32 verifFlags);


#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelAllocMem(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 Flags, NvU32 verifFlags) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelAllocMem(pGpu, pKernelChannel, Flags, verifFlags) kchannelAllocMem_GM107(pGpu, pKernelChannel, Flags, verifFlags)
#endif //__nvoc_kernel_channel_h_disabled

#define kchannelAllocMem_HAL(pGpu, pKernelChannel, Flags, verifFlags) kchannelAllocMem(pGpu, pKernelChannel, Flags, verifFlags)

void kchannelDestroyMem_GM107(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel);


#ifdef __nvoc_kernel_channel_h_disabled
static inline void kchannelDestroyMem(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelDestroyMem(pGpu, pKernelChannel) kchannelDestroyMem_GM107(pGpu, pKernelChannel)
#endif //__nvoc_kernel_channel_h_disabled

#define kchannelDestroyMem_HAL(pGpu, pKernelChannel) kchannelDestroyMem(pGpu, pKernelChannel)

NV_STATUS kchannelGetChannelPhysicalState_KERNEL(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NV208F_CTRL_FIFO_GET_CHANNEL_STATE_PARAMS *pChannelStateParams);


#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelGetChannelPhysicalState(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NV208F_CTRL_FIFO_GET_CHANNEL_STATE_PARAMS *pChannelStateParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelGetChannelPhysicalState(pGpu, pKernelChannel, pChannelStateParams) kchannelGetChannelPhysicalState_KERNEL(pGpu, pKernelChannel, pChannelStateParams)
#endif //__nvoc_kernel_channel_h_disabled

#define kchannelGetChannelPhysicalState_HAL(pGpu, pKernelChannel, pChannelStateParams) kchannelGetChannelPhysicalState(pGpu, pKernelChannel, pChannelStateParams)

static inline NvU32 kchannelEmbedRunlistIDForSMC_13cd8d(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel) {
    NV_ASSERT_PRECOMP(0);
    return 0;
}


#ifdef __nvoc_kernel_channel_h_disabled
static inline NvU32 kchannelEmbedRunlistIDForSMC(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return 0;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelEmbedRunlistIDForSMC(pGpu, pKernelChannel) kchannelEmbedRunlistIDForSMC_13cd8d(pGpu, pKernelChannel)
#endif //__nvoc_kernel_channel_h_disabled

#define kchannelEmbedRunlistIDForSMC_HAL(pGpu, pKernelChannel) kchannelEmbedRunlistIDForSMC(pGpu, pKernelChannel)

NV_STATUS kchannelAllocHwID_GM107(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvHandle hClient, NvU32 Flags, NvU32 verifFlags2, NvU32 ChID);


#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelAllocHwID(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvHandle hClient, NvU32 Flags, NvU32 verifFlags2, NvU32 ChID) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelAllocHwID(pGpu, pKernelChannel, hClient, Flags, verifFlags2, ChID) kchannelAllocHwID_GM107(pGpu, pKernelChannel, hClient, Flags, verifFlags2, ChID)
#endif //__nvoc_kernel_channel_h_disabled

#define kchannelAllocHwID_HAL(pGpu, pKernelChannel, hClient, Flags, verifFlags2, ChID) kchannelAllocHwID(pGpu, pKernelChannel, hClient, Flags, verifFlags2, ChID)

NV_STATUS kchannelFreeHwID_GM107(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel);


#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelFreeHwID(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelFreeHwID(pGpu, pKernelChannel) kchannelFreeHwID_GM107(pGpu, pKernelChannel)
#endif //__nvoc_kernel_channel_h_disabled

#define kchannelFreeHwID_HAL(pGpu, pKernelChannel) kchannelFreeHwID(pGpu, pKernelChannel)

NV_STATUS kchannelGetUserdInfo_GM107(struct OBJGPU *pGpu, struct KernelChannel *arg0, NvU64 *userBase, NvU64 *offset, NvU64 *length);


#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelGetUserdInfo(struct OBJGPU *pGpu, struct KernelChannel *arg0, NvU64 *userBase, NvU64 *offset, NvU64 *length) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelGetUserdInfo(pGpu, arg0, userBase, offset, length) kchannelGetUserdInfo_GM107(pGpu, arg0, userBase, offset, length)
#endif //__nvoc_kernel_channel_h_disabled

#define kchannelGetUserdInfo_HAL(pGpu, arg0, userBase, offset, length) kchannelGetUserdInfo(pGpu, arg0, userBase, offset, length)

NV_STATUS kchannelGetUserdBar1MapOffset_GM107(struct OBJGPU *pGpu, struct KernelChannel *arg0, NvU64 *bar1Offset, NvU32 *bar1MapSize);


#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelGetUserdBar1MapOffset(struct OBJGPU *pGpu, struct KernelChannel *arg0, NvU64 *bar1Offset, NvU32 *bar1MapSize) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelGetUserdBar1MapOffset(pGpu, arg0, bar1Offset, bar1MapSize) kchannelGetUserdBar1MapOffset_GM107(pGpu, arg0, bar1Offset, bar1MapSize)
#endif //__nvoc_kernel_channel_h_disabled

#define kchannelGetUserdBar1MapOffset_HAL(pGpu, arg0, bar1Offset, bar1MapSize) kchannelGetUserdBar1MapOffset(pGpu, arg0, bar1Offset, bar1MapSize)

NV_STATUS kchannelCreateUserdMemDescBc_GV100(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvHandle arg0, NvHandle *arg1, NvU64 *arg2);


#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCreateUserdMemDescBc(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvHandle arg0, NvHandle *arg1, NvU64 *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelCreateUserdMemDescBc(pGpu, pKernelChannel, arg0, arg1, arg2) kchannelCreateUserdMemDescBc_GV100(pGpu, pKernelChannel, arg0, arg1, arg2)
#endif //__nvoc_kernel_channel_h_disabled

#define kchannelCreateUserdMemDescBc_HAL(pGpu, pKernelChannel, arg0, arg1, arg2) kchannelCreateUserdMemDescBc(pGpu, pKernelChannel, arg0, arg1, arg2)

NV_STATUS kchannelCreateUserdMemDesc_GV100(struct OBJGPU *pGpu, struct KernelChannel *arg0, NvHandle arg1, NvHandle arg2, NvU64 arg3, NvU64 *arg4, NvU32 *arg5);


#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCreateUserdMemDesc(struct OBJGPU *pGpu, struct KernelChannel *arg0, NvHandle arg1, NvHandle arg2, NvU64 arg3, NvU64 *arg4, NvU32 *arg5) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelCreateUserdMemDesc(pGpu, arg0, arg1, arg2, arg3, arg4, arg5) kchannelCreateUserdMemDesc_GV100(pGpu, arg0, arg1, arg2, arg3, arg4, arg5)
#endif //__nvoc_kernel_channel_h_disabled

#define kchannelCreateUserdMemDesc_HAL(pGpu, arg0, arg1, arg2, arg3, arg4, arg5) kchannelCreateUserdMemDesc(pGpu, arg0, arg1, arg2, arg3, arg4, arg5)

void kchannelDestroyUserdMemDesc_GV100(struct OBJGPU *pGpu, struct KernelChannel *arg0);


#ifdef __nvoc_kernel_channel_h_disabled
static inline void kchannelDestroyUserdMemDesc(struct OBJGPU *pGpu, struct KernelChannel *arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelDestroyUserdMemDesc(pGpu, arg0) kchannelDestroyUserdMemDesc_GV100(pGpu, arg0)
#endif //__nvoc_kernel_channel_h_disabled

#define kchannelDestroyUserdMemDesc_HAL(pGpu, arg0) kchannelDestroyUserdMemDesc(pGpu, arg0)

NV_STATUS kchannelGetEngine_GM107(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 *engDesc);


#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelGetEngine(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 *engDesc) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelGetEngine(pGpu, pKernelChannel, engDesc) kchannelGetEngine_GM107(pGpu, pKernelChannel, engDesc)
#endif //__nvoc_kernel_channel_h_disabled

#define kchannelGetEngine_HAL(pGpu, pKernelChannel, engDesc) kchannelGetEngine(pGpu, pKernelChannel, engDesc)

static inline NV_STATUS kchannelFwdToInternalCtrl_56cd7a(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 internalCmd, RmCtrlParams *pRmCtrlParams) {
    return NV_OK;
}


#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelFwdToInternalCtrl(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 internalCmd, RmCtrlParams *pRmCtrlParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelFwdToInternalCtrl(pGpu, pKernelChannel, internalCmd, pRmCtrlParams) kchannelFwdToInternalCtrl_56cd7a(pGpu, pKernelChannel, internalCmd, pRmCtrlParams)
#endif //__nvoc_kernel_channel_h_disabled

#define kchannelFwdToInternalCtrl_HAL(pGpu, pKernelChannel, internalCmd, pRmCtrlParams) kchannelFwdToInternalCtrl(pGpu, pKernelChannel, internalCmd, pRmCtrlParams)

static inline NV_STATUS kchannelAllocChannel_56cd7a(struct KernelChannel *pKernelChannel, NV_CHANNEL_ALLOC_PARAMS *pChannelGpfifoParams) {
    return NV_OK;
}


#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelAllocChannel(struct KernelChannel *pKernelChannel, NV_CHANNEL_ALLOC_PARAMS *pChannelGpfifoParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelAllocChannel(pKernelChannel, pChannelGpfifoParams) kchannelAllocChannel_56cd7a(pKernelChannel, pChannelGpfifoParams)
#endif //__nvoc_kernel_channel_h_disabled

#define kchannelAllocChannel_HAL(pKernelChannel, pChannelGpfifoParams) kchannelAllocChannel(pKernelChannel, pChannelGpfifoParams)

static inline NvBool kchannelIsValid_cbe027(struct KernelChannel *pKernelChannel) {
    return ((NvBool)(0 == 0));
}


#ifdef __nvoc_kernel_channel_h_disabled
static inline NvBool kchannelIsValid(struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelIsValid(pKernelChannel) kchannelIsValid_cbe027(pKernelChannel)
#endif //__nvoc_kernel_channel_h_disabled

#define kchannelIsValid_HAL(pKernelChannel) kchannelIsValid(pKernelChannel)

NV_STATUS kchannelGetClassEngineID_GM107(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvHandle handle, NvU32 *classEngineID, NvU32 *classID, RM_ENGINE_TYPE *rmEngineID);


#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelGetClassEngineID(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvHandle handle, NvU32 *classEngineID, NvU32 *classID, RM_ENGINE_TYPE *rmEngineID) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelGetClassEngineID(pGpu, pKernelChannel, handle, classEngineID, classID, rmEngineID) kchannelGetClassEngineID_GM107(pGpu, pKernelChannel, handle, classEngineID, classID, rmEngineID)
#endif //__nvoc_kernel_channel_h_disabled

#define kchannelGetClassEngineID_HAL(pGpu, pKernelChannel, handle, classEngineID, classID, rmEngineID) kchannelGetClassEngineID(pGpu, pKernelChannel, handle, classEngineID, classID, rmEngineID)

NV_STATUS kchannelEnableVirtualContext_GM107(struct KernelChannel *arg0);


#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelEnableVirtualContext(struct KernelChannel *arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelEnableVirtualContext(arg0) kchannelEnableVirtualContext_GM107(arg0)
#endif //__nvoc_kernel_channel_h_disabled

#define kchannelEnableVirtualContext_HAL(arg0) kchannelEnableVirtualContext(arg0)

static inline NV_STATUS kchannelRotateSecureChannelIv_46f6a7(struct KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation, NvU32 *encryptIv, NvU32 *decryptIv) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelRotateSecureChannelIv(struct KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation, NvU32 *encryptIv, NvU32 *decryptIv) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelRotateSecureChannelIv(pKernelChannel, rotateOperation, encryptIv, decryptIv) kchannelRotateSecureChannelIv_46f6a7(pKernelChannel, rotateOperation, encryptIv, decryptIv)
#endif //__nvoc_kernel_channel_h_disabled

#define kchannelRotateSecureChannelIv_HAL(pKernelChannel, rotateOperation, encryptIv, decryptIv) kchannelRotateSecureChannelIv(pKernelChannel, rotateOperation, encryptIv, decryptIv)

NV_STATUS kchannelRetrieveKmb_KERNEL(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation, NvBool includeSecrets, CC_KMB *keyMaterialBundle);


#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelRetrieveKmb(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation, NvBool includeSecrets, CC_KMB *keyMaterialBundle) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelRetrieveKmb(pGpu, pKernelChannel, rotateOperation, includeSecrets, keyMaterialBundle) kchannelRetrieveKmb_KERNEL(pGpu, pKernelChannel, rotateOperation, includeSecrets, keyMaterialBundle)
#endif //__nvoc_kernel_channel_h_disabled

#define kchannelRetrieveKmb_HAL(pGpu, pKernelChannel, rotateOperation, includeSecrets, keyMaterialBundle) kchannelRetrieveKmb(pGpu, pKernelChannel, rotateOperation, includeSecrets, keyMaterialBundle)

NV_STATUS kchannelMap_IMPL(struct KernelChannel *pKernelChannel, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);

static inline NV_STATUS kchannelMap_DISPATCH(struct KernelChannel *pKernelChannel, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pKernelChannel->__kchannelMap__(pKernelChannel, pCallContext, pParams, pCpuMapping);
}

NV_STATUS kchannelUnmap_IMPL(struct KernelChannel *pKernelChannel, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);

static inline NV_STATUS kchannelUnmap_DISPATCH(struct KernelChannel *pKernelChannel, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pKernelChannel->__kchannelUnmap__(pKernelChannel, pCallContext, pCpuMapping);
}

NV_STATUS kchannelGetMapAddrSpace_IMPL(struct KernelChannel *pKernelChannel, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);

static inline NV_STATUS kchannelGetMapAddrSpace_DISPATCH(struct KernelChannel *pKernelChannel, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pKernelChannel->__kchannelGetMapAddrSpace__(pKernelChannel, pCallContext, mapFlags, pAddrSpace);
}

NV_STATUS kchannelGetMemInterMapParams_IMPL(struct KernelChannel *pKernelChannel, RMRES_MEM_INTER_MAP_PARAMS *pParams);

static inline NV_STATUS kchannelGetMemInterMapParams_DISPATCH(struct KernelChannel *pKernelChannel, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pKernelChannel->__kchannelGetMemInterMapParams__(pKernelChannel, pParams);
}

NV_STATUS kchannelCheckMemInterUnmap_IMPL(struct KernelChannel *pKernelChannel, NvBool bSubdeviceHandleProvided);

static inline NV_STATUS kchannelCheckMemInterUnmap_DISPATCH(struct KernelChannel *pKernelChannel, NvBool bSubdeviceHandleProvided) {
    return pKernelChannel->__kchannelCheckMemInterUnmap__(pKernelChannel, bSubdeviceHandleProvided);
}

NV_STATUS kchannelCreateUserMemDesc_GM107(struct OBJGPU *pGpu, struct KernelChannel *arg0);

NV_STATUS kchannelCreateUserMemDesc_GA10B(struct OBJGPU *pGpu, struct KernelChannel *arg0);

static inline NV_STATUS kchannelCreateUserMemDesc_DISPATCH(struct OBJGPU *pGpu, struct KernelChannel *arg0) {
    return arg0->__kchannelCreateUserMemDesc__(pGpu, arg0);
}

NvBool kchannelIsUserdAddrSizeValid_GV100(struct KernelChannel *pKernelChannel, NvU32 userdAddrLo, NvU32 userdAddrHi);

NvBool kchannelIsUserdAddrSizeValid_GA100(struct KernelChannel *pKernelChannel, NvU32 userdAddrLo, NvU32 userdAddrHi);

NvBool kchannelIsUserdAddrSizeValid_GH100(struct KernelChannel *pKernelChannel, NvU32 userdAddrLo, NvU32 userdAddrHi);

static inline NvBool kchannelIsUserdAddrSizeValid_DISPATCH(struct KernelChannel *pKernelChannel, NvU32 userdAddrLo, NvU32 userdAddrHi) {
    return pKernelChannel->__kchannelIsUserdAddrSizeValid__(pKernelChannel, userdAddrLo, userdAddrHi);
}

NV_STATUS kchannelCtrlCmdResetIsolatedChannel_IMPL(struct KernelChannel *pKernelChannel, NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS *pResetParams);

static inline NV_STATUS kchannelCtrlCmdResetIsolatedChannel_DISPATCH(struct KernelChannel *pKernelChannel, NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS *pResetParams) {
    return pKernelChannel->__kchannelCtrlCmdResetIsolatedChannel__(pKernelChannel, pResetParams);
}

NV_STATUS kchannelCtrlCmdGetClassEngineid_IMPL(struct KernelChannel *pKernelChannel, NV906F_CTRL_GET_CLASS_ENGINEID_PARAMS *pParams);

static inline NV_STATUS kchannelCtrlCmdGetClassEngineid_DISPATCH(struct KernelChannel *pKernelChannel, NV906F_CTRL_GET_CLASS_ENGINEID_PARAMS *pParams) {
    return pKernelChannel->__kchannelCtrlCmdGetClassEngineid__(pKernelChannel, pParams);
}

NV_STATUS kchannelCtrlCmdResetChannel_IMPL(struct KernelChannel *pKernelChannel, NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS *pResetChannelParams);

static inline NV_STATUS kchannelCtrlCmdResetChannel_DISPATCH(struct KernelChannel *pKernelChannel, NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS *pResetChannelParams) {
    return pKernelChannel->__kchannelCtrlCmdResetChannel__(pKernelChannel, pResetChannelParams);
}

NV_STATUS kchannelCtrlCmdGetDeferRCState_IMPL(struct KernelChannel *pKernelChannel, NV906F_CTRL_CMD_GET_DEFER_RC_STATE_PARAMS *pStateParams);

static inline NV_STATUS kchannelCtrlCmdGetDeferRCState_DISPATCH(struct KernelChannel *pKernelChannel, NV906F_CTRL_CMD_GET_DEFER_RC_STATE_PARAMS *pStateParams) {
    return pKernelChannel->__kchannelCtrlCmdGetDeferRCState__(pKernelChannel, pStateParams);
}

NV_STATUS kchannelCtrlCmdGetMmuFaultInfo_IMPL(struct KernelChannel *pKernelChannel, NV906F_CTRL_GET_MMU_FAULT_INFO_PARAMS *pFaultInfoParams);

static inline NV_STATUS kchannelCtrlCmdGetMmuFaultInfo_DISPATCH(struct KernelChannel *pKernelChannel, NV906F_CTRL_GET_MMU_FAULT_INFO_PARAMS *pFaultInfoParams) {
    return pKernelChannel->__kchannelCtrlCmdGetMmuFaultInfo__(pKernelChannel, pFaultInfoParams);
}

NV_STATUS kchannelCtrlCmdEventSetNotification_IMPL(struct KernelChannel *pKernelChannel, NV906F_CTRL_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams);

static inline NV_STATUS kchannelCtrlCmdEventSetNotification_DISPATCH(struct KernelChannel *pKernelChannel, NV906F_CTRL_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams) {
    return pKernelChannel->__kchannelCtrlCmdEventSetNotification__(pKernelChannel, pSetEventParams);
}

static inline NV_STATUS kchannelCtrlCmdGetClassEngineidA06F_6a9a13(struct KernelChannel *pKernelChannel, NVA06F_CTRL_GET_CLASS_ENGINEID_PARAMS *pParams) {
    return kchannelCtrlCmdGetClassEngineid(pKernelChannel, pParams);
}

static inline NV_STATUS kchannelCtrlCmdGetClassEngineidA06F_DISPATCH(struct KernelChannel *pKernelChannel, NVA06F_CTRL_GET_CLASS_ENGINEID_PARAMS *pParams) {
    return pKernelChannel->__kchannelCtrlCmdGetClassEngineidA06F__(pKernelChannel, pParams);
}

static inline NV_STATUS kchannelCtrlCmdResetChannelA06F_ef73a1(struct KernelChannel *pKernelChannel, NVA06F_CTRL_CMD_RESET_CHANNEL_PARAMS *pResetChannelParams) {
    return kchannelCtrlCmdResetChannel(pKernelChannel, pResetChannelParams);
}

static inline NV_STATUS kchannelCtrlCmdResetChannelA06F_DISPATCH(struct KernelChannel *pKernelChannel, NVA06F_CTRL_CMD_RESET_CHANNEL_PARAMS *pResetChannelParams) {
    return pKernelChannel->__kchannelCtrlCmdResetChannelA06F__(pKernelChannel, pResetChannelParams);
}

NV_STATUS kchannelCtrlCmdGpFifoSchedule_IMPL(struct KernelChannel *pKernelChannel, NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams);

static inline NV_STATUS kchannelCtrlCmdGpFifoSchedule_DISPATCH(struct KernelChannel *pKernelChannel, NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams) {
    return pKernelChannel->__kchannelCtrlCmdGpFifoSchedule__(pKernelChannel, pSchedParams);
}

NV_STATUS kchannelCtrlCmdBind_IMPL(struct KernelChannel *pKernelChannel, NVA06F_CTRL_BIND_PARAMS *pParams);

static inline NV_STATUS kchannelCtrlCmdBind_DISPATCH(struct KernelChannel *pKernelChannel, NVA06F_CTRL_BIND_PARAMS *pParams) {
    return pKernelChannel->__kchannelCtrlCmdBind__(pKernelChannel, pParams);
}

static inline NV_STATUS kchannelCtrlCmdGetMmuFaultInfoA06F_a7f9ac(struct KernelChannel *pKernelChannel, NVA06F_CTRL_GET_MMU_FAULT_INFO_PARAMS *pFaultInfoParams) {
    return kchannelCtrlCmdGetMmuFaultInfo(pKernelChannel, pFaultInfoParams);
}

static inline NV_STATUS kchannelCtrlCmdGetMmuFaultInfoA06F_DISPATCH(struct KernelChannel *pKernelChannel, NVA06F_CTRL_GET_MMU_FAULT_INFO_PARAMS *pFaultInfoParams) {
    return pKernelChannel->__kchannelCtrlCmdGetMmuFaultInfoA06F__(pKernelChannel, pFaultInfoParams);
}

NV_STATUS kchannelCtrlCmdSetErrorNotifier_IMPL(struct KernelChannel *pKernelChannel, NVA06F_CTRL_SET_ERROR_NOTIFIER_PARAMS *pSetErrorNotifierParams);

static inline NV_STATUS kchannelCtrlCmdSetErrorNotifier_DISPATCH(struct KernelChannel *pKernelChannel, NVA06F_CTRL_SET_ERROR_NOTIFIER_PARAMS *pSetErrorNotifierParams) {
    return pKernelChannel->__kchannelCtrlCmdSetErrorNotifier__(pKernelChannel, pSetErrorNotifierParams);
}

NV_STATUS kchannelCtrlCmdSetInterleaveLevel_IMPL(struct KernelChannel *pKernelChannel, NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS *pParams);

static inline NV_STATUS kchannelCtrlCmdSetInterleaveLevel_DISPATCH(struct KernelChannel *pKernelChannel, NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS *pParams) {
    return pKernelChannel->__kchannelCtrlCmdSetInterleaveLevel__(pKernelChannel, pParams);
}

NV_STATUS kchannelCtrlCmdRestartRunlist_IMPL(struct KernelChannel *pKernelChannel, NVA06F_CTRL_RESTART_RUNLIST_PARAMS *pParams);

static inline NV_STATUS kchannelCtrlCmdRestartRunlist_DISPATCH(struct KernelChannel *pKernelChannel, NVA06F_CTRL_RESTART_RUNLIST_PARAMS *pParams) {
    return pKernelChannel->__kchannelCtrlCmdRestartRunlist__(pKernelChannel, pParams);
}

static inline NV_STATUS kchannelCtrlCmdGetClassEngineidA16F_6a9a13(struct KernelChannel *pKernelChannel, NVA16F_CTRL_GET_CLASS_ENGINEID_PARAMS *pParams) {
    return kchannelCtrlCmdGetClassEngineid(pKernelChannel, pParams);
}

static inline NV_STATUS kchannelCtrlCmdGetClassEngineidA16F_DISPATCH(struct KernelChannel *pKernelChannel, NVA16F_CTRL_GET_CLASS_ENGINEID_PARAMS *pParams) {
    return pKernelChannel->__kchannelCtrlCmdGetClassEngineidA16F__(pKernelChannel, pParams);
}

static inline NV_STATUS kchannelCtrlCmdResetChannelA16F_ef73a1(struct KernelChannel *pKernelChannel, NVA16F_CTRL_CMD_RESET_CHANNEL_PARAMS *pResetChannelParams) {
    return kchannelCtrlCmdResetChannel(pKernelChannel, pResetChannelParams);
}

static inline NV_STATUS kchannelCtrlCmdResetChannelA16F_DISPATCH(struct KernelChannel *pKernelChannel, NVA16F_CTRL_CMD_RESET_CHANNEL_PARAMS *pResetChannelParams) {
    return pKernelChannel->__kchannelCtrlCmdResetChannelA16F__(pKernelChannel, pResetChannelParams);
}

static inline NV_STATUS kchannelCtrlCmdGpFifoScheduleA16F_6546a6(struct KernelChannel *pKernelChannel, NVA16F_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams) {
    return kchannelCtrlCmdGpFifoSchedule(pKernelChannel, pSchedParams);
}

static inline NV_STATUS kchannelCtrlCmdGpFifoScheduleA16F_DISPATCH(struct KernelChannel *pKernelChannel, NVA16F_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams) {
    return pKernelChannel->__kchannelCtrlCmdGpFifoScheduleA16F__(pKernelChannel, pSchedParams);
}

static inline NV_STATUS kchannelCtrlCmdGetClassEngineidA26F_6a9a13(struct KernelChannel *pKernelChannel, NVA26F_CTRL_GET_CLASS_ENGINEID_PARAMS *pParams) {
    return kchannelCtrlCmdGetClassEngineid(pKernelChannel, pParams);
}

static inline NV_STATUS kchannelCtrlCmdGetClassEngineidA26F_DISPATCH(struct KernelChannel *pKernelChannel, NVA26F_CTRL_GET_CLASS_ENGINEID_PARAMS *pParams) {
    return pKernelChannel->__kchannelCtrlCmdGetClassEngineidA26F__(pKernelChannel, pParams);
}

static inline NV_STATUS kchannelCtrlCmdResetChannelA26F_ef73a1(struct KernelChannel *pKernelChannel, NVA26F_CTRL_CMD_RESET_CHANNEL_PARAMS *pResetChannelParams) {
    return kchannelCtrlCmdResetChannel(pKernelChannel, pResetChannelParams);
}

static inline NV_STATUS kchannelCtrlCmdResetChannelA26F_DISPATCH(struct KernelChannel *pKernelChannel, NVA26F_CTRL_CMD_RESET_CHANNEL_PARAMS *pResetChannelParams) {
    return pKernelChannel->__kchannelCtrlCmdResetChannelA26F__(pKernelChannel, pResetChannelParams);
}

static inline NV_STATUS kchannelFCtrlCmdGpFifoScheduleA26F_6546a6(struct KernelChannel *pKernelChannel, NVA26F_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams) {
    return kchannelCtrlCmdGpFifoSchedule(pKernelChannel, pSchedParams);
}

static inline NV_STATUS kchannelFCtrlCmdGpFifoScheduleA26F_DISPATCH(struct KernelChannel *pKernelChannel, NVA26F_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams) {
    return pKernelChannel->__kchannelFCtrlCmdGpFifoScheduleA26F__(pKernelChannel, pSchedParams);
}

static inline NV_STATUS kchannelCtrlCmdGetClassEngineidB06F_6a9a13(struct KernelChannel *pKernelChannel, NVB06F_CTRL_GET_CLASS_ENGINEID_PARAMS *pParams) {
    return kchannelCtrlCmdGetClassEngineid(pKernelChannel, pParams);
}

static inline NV_STATUS kchannelCtrlCmdGetClassEngineidB06F_DISPATCH(struct KernelChannel *pKernelChannel, NVB06F_CTRL_GET_CLASS_ENGINEID_PARAMS *pParams) {
    return pKernelChannel->__kchannelCtrlCmdGetClassEngineidB06F__(pKernelChannel, pParams);
}

static inline NV_STATUS kchannelCtrlCmdResetChannelB06F_ef73a1(struct KernelChannel *pKernelChannel, NVB06F_CTRL_CMD_RESET_CHANNEL_PARAMS *pResetChannelParams) {
    return kchannelCtrlCmdResetChannel(pKernelChannel, pResetChannelParams);
}

static inline NV_STATUS kchannelCtrlCmdResetChannelB06F_DISPATCH(struct KernelChannel *pKernelChannel, NVB06F_CTRL_CMD_RESET_CHANNEL_PARAMS *pResetChannelParams) {
    return pKernelChannel->__kchannelCtrlCmdResetChannelB06F__(pKernelChannel, pResetChannelParams);
}

static inline NV_STATUS kchannelCtrlCmdGpFifoScheduleB06F_6546a6(struct KernelChannel *pKernelChannel, NVB06F_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams) {
    return kchannelCtrlCmdGpFifoSchedule(pKernelChannel, pSchedParams);
}

static inline NV_STATUS kchannelCtrlCmdGpFifoScheduleB06F_DISPATCH(struct KernelChannel *pKernelChannel, NVB06F_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams) {
    return pKernelChannel->__kchannelCtrlCmdGpFifoScheduleB06F__(pKernelChannel, pSchedParams);
}

static inline NV_STATUS kchannelCtrlCmdBindB06F_2c1c21(struct KernelChannel *pKernelChannel, NVB06F_CTRL_BIND_PARAMS *pParams) {
    return kchannelCtrlCmdBind(pKernelChannel, pParams);
}

static inline NV_STATUS kchannelCtrlCmdBindB06F_DISPATCH(struct KernelChannel *pKernelChannel, NVB06F_CTRL_BIND_PARAMS *pParams) {
    return pKernelChannel->__kchannelCtrlCmdBindB06F__(pKernelChannel, pParams);
}

NV_STATUS kchannelCtrlCmdGetEngineCtxSize_IMPL(struct KernelChannel *pKernelChannel, NVB06F_CTRL_GET_ENGINE_CTX_SIZE_PARAMS *pCtxSizeParams);

static inline NV_STATUS kchannelCtrlCmdGetEngineCtxSize_DISPATCH(struct KernelChannel *pKernelChannel, NVB06F_CTRL_GET_ENGINE_CTX_SIZE_PARAMS *pCtxSizeParams) {
    return pKernelChannel->__kchannelCtrlCmdGetEngineCtxSize__(pKernelChannel, pCtxSizeParams);
}

NV_STATUS kchannelCtrlCmdGetEngineCtxData_IMPL(struct KernelChannel *pKernelChannel, NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *pCtxBuffParams);

static inline NV_STATUS kchannelCtrlCmdGetEngineCtxData_DISPATCH(struct KernelChannel *pKernelChannel, NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS *pCtxBuffParams) {
    return pKernelChannel->__kchannelCtrlCmdGetEngineCtxData__(pKernelChannel, pCtxBuffParams);
}

NV_STATUS kchannelCtrlCmdMigrateEngineCtxData_IMPL(struct KernelChannel *pKernelChannel, NVB06F_CTRL_MIGRATE_ENGINE_CTX_DATA_PARAMS *pCtxBuffParams);

static inline NV_STATUS kchannelCtrlCmdMigrateEngineCtxData_DISPATCH(struct KernelChannel *pKernelChannel, NVB06F_CTRL_MIGRATE_ENGINE_CTX_DATA_PARAMS *pCtxBuffParams) {
    return pKernelChannel->__kchannelCtrlCmdMigrateEngineCtxData__(pKernelChannel, pCtxBuffParams);
}

NV_STATUS kchannelCtrlCmdGetEngineCtxState_IMPL(struct KernelChannel *pKernelChannel, NVB06F_CTRL_GET_ENGINE_CTX_STATE_PARAMS *pCtxStateParams);

static inline NV_STATUS kchannelCtrlCmdGetEngineCtxState_DISPATCH(struct KernelChannel *pKernelChannel, NVB06F_CTRL_GET_ENGINE_CTX_STATE_PARAMS *pCtxStateParams) {
    return pKernelChannel->__kchannelCtrlCmdGetEngineCtxState__(pKernelChannel, pCtxStateParams);
}

NV_STATUS kchannelCtrlCmdGetChannelHwState_IMPL(struct KernelChannel *pKernelChannel, NVB06F_CTRL_GET_CHANNEL_HW_STATE_PARAMS *pParams);

static inline NV_STATUS kchannelCtrlCmdGetChannelHwState_DISPATCH(struct KernelChannel *pKernelChannel, NVB06F_CTRL_GET_CHANNEL_HW_STATE_PARAMS *pParams) {
    return pKernelChannel->__kchannelCtrlCmdGetChannelHwState__(pKernelChannel, pParams);
}

NV_STATUS kchannelCtrlCmdSetChannelHwState_IMPL(struct KernelChannel *pKernelChannel, NVB06F_CTRL_SET_CHANNEL_HW_STATE_PARAMS *pParams);

static inline NV_STATUS kchannelCtrlCmdSetChannelHwState_DISPATCH(struct KernelChannel *pKernelChannel, NVB06F_CTRL_SET_CHANNEL_HW_STATE_PARAMS *pParams) {
    return pKernelChannel->__kchannelCtrlCmdSetChannelHwState__(pKernelChannel, pParams);
}

NV_STATUS kchannelCtrlCmdSaveEngineCtxData_IMPL(struct KernelChannel *pKernelChannel, NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS *pCtxBuffParams);

static inline NV_STATUS kchannelCtrlCmdSaveEngineCtxData_DISPATCH(struct KernelChannel *pKernelChannel, NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS *pCtxBuffParams) {
    return pKernelChannel->__kchannelCtrlCmdSaveEngineCtxData__(pKernelChannel, pCtxBuffParams);
}

NV_STATUS kchannelCtrlCmdRestoreEngineCtxData_IMPL(struct KernelChannel *pKernelChannel, NVB06F_CTRL_RESTORE_ENGINE_CTX_DATA_PARAMS *pCtxBuffParams);

static inline NV_STATUS kchannelCtrlCmdRestoreEngineCtxData_DISPATCH(struct KernelChannel *pKernelChannel, NVB06F_CTRL_RESTORE_ENGINE_CTX_DATA_PARAMS *pCtxBuffParams) {
    return pKernelChannel->__kchannelCtrlCmdRestoreEngineCtxData__(pKernelChannel, pCtxBuffParams);
}

static inline NV_STATUS kchannelCtrlCmdGetClassEngineidC06F_6a9a13(struct KernelChannel *pKernelChannel, NVC06F_CTRL_GET_CLASS_ENGINEID_PARAMS *pParams) {
    return kchannelCtrlCmdGetClassEngineid(pKernelChannel, pParams);
}

static inline NV_STATUS kchannelCtrlCmdGetClassEngineidC06F_DISPATCH(struct KernelChannel *pKernelChannel, NVC06F_CTRL_GET_CLASS_ENGINEID_PARAMS *pParams) {
    return pKernelChannel->__kchannelCtrlCmdGetClassEngineidC06F__(pKernelChannel, pParams);
}

static inline NV_STATUS kchannelCtrlCmdResetChannelC06F_ef73a1(struct KernelChannel *pKernelChannel, NVC06F_CTRL_CMD_RESET_CHANNEL_PARAMS *pResetChannelParams) {
    return kchannelCtrlCmdResetChannel(pKernelChannel, pResetChannelParams);
}

static inline NV_STATUS kchannelCtrlCmdResetChannelC06F_DISPATCH(struct KernelChannel *pKernelChannel, NVC06F_CTRL_CMD_RESET_CHANNEL_PARAMS *pResetChannelParams) {
    return pKernelChannel->__kchannelCtrlCmdResetChannelC06F__(pKernelChannel, pResetChannelParams);
}

static inline NV_STATUS kchannelCtrlCmdGpFifoScheduleC06F_6546a6(struct KernelChannel *pKernelChannel, NVC06F_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams) {
    return kchannelCtrlCmdGpFifoSchedule(pKernelChannel, pSchedParams);
}

static inline NV_STATUS kchannelCtrlCmdGpFifoScheduleC06F_DISPATCH(struct KernelChannel *pKernelChannel, NVC06F_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams) {
    return pKernelChannel->__kchannelCtrlCmdGpFifoScheduleC06F__(pKernelChannel, pSchedParams);
}

static inline NV_STATUS kchannelCtrlCmdBindC06F_2c1c21(struct KernelChannel *pKernelChannel, NVC06F_CTRL_BIND_PARAMS *pParams) {
    return kchannelCtrlCmdBind(pKernelChannel, pParams);
}

static inline NV_STATUS kchannelCtrlCmdBindC06F_DISPATCH(struct KernelChannel *pKernelChannel, NVC06F_CTRL_BIND_PARAMS *pParams) {
    return pKernelChannel->__kchannelCtrlCmdBindC06F__(pKernelChannel, pParams);
}

static inline NV_STATUS kchannelCtrlCmdGetClassEngineidC36F_6a9a13(struct KernelChannel *pKernelChannel, NVC36F_CTRL_GET_CLASS_ENGINEID_PARAMS *pParams) {
    return kchannelCtrlCmdGetClassEngineid(pKernelChannel, pParams);
}

static inline NV_STATUS kchannelCtrlCmdGetClassEngineidC36F_DISPATCH(struct KernelChannel *pKernelChannel, NVC36F_CTRL_GET_CLASS_ENGINEID_PARAMS *pParams) {
    return pKernelChannel->__kchannelCtrlCmdGetClassEngineidC36F__(pKernelChannel, pParams);
}

static inline NV_STATUS kchannelCtrlCmdResetChannelC36F_ef73a1(struct KernelChannel *pKernelChannel, NVC36F_CTRL_CMD_RESET_CHANNEL_PARAMS *pResetChannelParams) {
    return kchannelCtrlCmdResetChannel(pKernelChannel, pResetChannelParams);
}

static inline NV_STATUS kchannelCtrlCmdResetChannelC36F_DISPATCH(struct KernelChannel *pKernelChannel, NVC36F_CTRL_CMD_RESET_CHANNEL_PARAMS *pResetChannelParams) {
    return pKernelChannel->__kchannelCtrlCmdResetChannelC36F__(pKernelChannel, pResetChannelParams);
}

static inline NV_STATUS kchannelCtrlCmdGpFifoScheduleC36F_6546a6(struct KernelChannel *pKernelChannel, NVC36F_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams) {
    return kchannelCtrlCmdGpFifoSchedule(pKernelChannel, pSchedParams);
}

static inline NV_STATUS kchannelCtrlCmdGpFifoScheduleC36F_DISPATCH(struct KernelChannel *pKernelChannel, NVC36F_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams) {
    return pKernelChannel->__kchannelCtrlCmdGpFifoScheduleC36F__(pKernelChannel, pSchedParams);
}

static inline NV_STATUS kchannelCtrlCmdBindC36F_2c1c21(struct KernelChannel *pKernelChannel, NVC36F_CTRL_BIND_PARAMS *pParams) {
    return kchannelCtrlCmdBind(pKernelChannel, pParams);
}

static inline NV_STATUS kchannelCtrlCmdBindC36F_DISPATCH(struct KernelChannel *pKernelChannel, NVC36F_CTRL_BIND_PARAMS *pParams) {
    return pKernelChannel->__kchannelCtrlCmdBindC36F__(pKernelChannel, pParams);
}

NV_STATUS kchannelCtrlCmdGpfifoGetWorkSubmitToken_IMPL(struct KernelChannel *pKernelChannel, NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS *pTokenParams);

static inline NV_STATUS kchannelCtrlCmdGpfifoGetWorkSubmitToken_DISPATCH(struct KernelChannel *pKernelChannel, NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS *pTokenParams) {
    return pKernelChannel->__kchannelCtrlCmdGpfifoGetWorkSubmitToken__(pKernelChannel, pTokenParams);
}

NV_STATUS kchannelCtrlCmdGpfifoUpdateFaultMethodBuffer_IMPL(struct KernelChannel *pKernelChannel, NVC36F_CTRL_GPFIFO_UPDATE_FAULT_METHOD_BUFFER_PARAMS *pFaultMthdBufferParams);

static inline NV_STATUS kchannelCtrlCmdGpfifoUpdateFaultMethodBuffer_DISPATCH(struct KernelChannel *pKernelChannel, NVC36F_CTRL_GPFIFO_UPDATE_FAULT_METHOD_BUFFER_PARAMS *pFaultMthdBufferParams) {
    return pKernelChannel->__kchannelCtrlCmdGpfifoUpdateFaultMethodBuffer__(pKernelChannel, pFaultMthdBufferParams);
}

NV_STATUS kchannelCtrlCmdGpfifoSetWorkSubmitTokenNotifIndex_IMPL(struct KernelChannel *pKernelChannel, NVC36F_CTRL_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX_PARAMS *pParams);

static inline NV_STATUS kchannelCtrlCmdGpfifoSetWorkSubmitTokenNotifIndex_DISPATCH(struct KernelChannel *pKernelChannel, NVC36F_CTRL_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX_PARAMS *pParams) {
    return pKernelChannel->__kchannelCtrlCmdGpfifoSetWorkSubmitTokenNotifIndex__(pKernelChannel, pParams);
}

NV_STATUS kchannelCtrlCmdStopChannel_IMPL(struct KernelChannel *pKernelChannel, NVA06F_CTRL_STOP_CHANNEL_PARAMS *pStopChannelParams);

static inline NV_STATUS kchannelCtrlCmdStopChannel_DISPATCH(struct KernelChannel *pKernelChannel, NVA06F_CTRL_STOP_CHANNEL_PARAMS *pStopChannelParams) {
    return pKernelChannel->__kchannelCtrlCmdStopChannel__(pKernelChannel, pStopChannelParams);
}

NV_STATUS kchannelCtrlCmdGetKmb_KERNEL(struct KernelChannel *pKernelChannel, NVC56F_CTRL_CMD_GET_KMB_PARAMS *pGetKmbParams);

static inline NV_STATUS kchannelCtrlCmdGetKmb_DISPATCH(struct KernelChannel *pKernelChannel, NVC56F_CTRL_CMD_GET_KMB_PARAMS *pGetKmbParams) {
    return pKernelChannel->__kchannelCtrlCmdGetKmb__(pKernelChannel, pGetKmbParams);
}

NV_STATUS kchannelCtrlRotateSecureChannelIv_KERNEL(struct KernelChannel *pKernelChannel, NVC56F_CTRL_ROTATE_SECURE_CHANNEL_IV_PARAMS *pRotateIvParams);

static inline NV_STATUS kchannelCtrlRotateSecureChannelIv_DISPATCH(struct KernelChannel *pKernelChannel, NVC56F_CTRL_ROTATE_SECURE_CHANNEL_IV_PARAMS *pRotateIvParams) {
    return pKernelChannel->__kchannelCtrlRotateSecureChannelIv__(pKernelChannel, pRotateIvParams);
}

static inline NV_STATUS kchannelCtrlGetTpcPartitionMode_a094e1(struct KernelChannel *pKernelChannel, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams) {
    return kgrctxCtrlHandle(resservGetTlsCallContext(), pKernelChannel->hKernelGraphicsContext);
}

static inline NV_STATUS kchannelCtrlGetTpcPartitionMode_DISPATCH(struct KernelChannel *pKernelChannel, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams) {
    return pKernelChannel->__kchannelCtrlGetTpcPartitionMode__(pKernelChannel, pParams);
}

static inline NV_STATUS kchannelCtrlSetTpcPartitionMode_a094e1(struct KernelChannel *pKernelChannel, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams) {
    return kgrctxCtrlHandle(resservGetTlsCallContext(), pKernelChannel->hKernelGraphicsContext);
}

static inline NV_STATUS kchannelCtrlSetTpcPartitionMode_DISPATCH(struct KernelChannel *pKernelChannel, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams) {
    return pKernelChannel->__kchannelCtrlSetTpcPartitionMode__(pKernelChannel, pParams);
}

static inline NV_STATUS kchannelCtrlGetMMUDebugMode_a094e1(struct KernelChannel *pKernelChannel, NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS *pParams) {
    return kgrctxCtrlHandle(resservGetTlsCallContext(), pKernelChannel->hKernelGraphicsContext);
}

static inline NV_STATUS kchannelCtrlGetMMUDebugMode_DISPATCH(struct KernelChannel *pKernelChannel, NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS *pParams) {
    return pKernelChannel->__kchannelCtrlGetMMUDebugMode__(pKernelChannel, pParams);
}

static inline NV_STATUS kchannelCtrlProgramVidmemPromote_a094e1(struct KernelChannel *pKernelChannel, NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *pParams) {
    return kgrctxCtrlHandle(resservGetTlsCallContext(), pKernelChannel->hKernelGraphicsContext);
}

static inline NV_STATUS kchannelCtrlProgramVidmemPromote_DISPATCH(struct KernelChannel *pKernelChannel, NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *pParams) {
    return pKernelChannel->__kchannelCtrlProgramVidmemPromote__(pKernelChannel, pParams);
}

static inline NvBool kchannelShareCallback_DISPATCH(struct KernelChannel *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__kchannelShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS kchannelGetOrAllocNotifShare_DISPATCH(struct KernelChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__kchannelGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

static inline NV_STATUS kchannelMapTo_DISPATCH(struct KernelChannel *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__kchannelMapTo__(pResource, pParams);
}

static inline void kchannelSetNotificationShare_DISPATCH(struct KernelChannel *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__kchannelSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 kchannelGetRefCount_DISPATCH(struct KernelChannel *pResource) {
    return pResource->__kchannelGetRefCount__(pResource);
}

static inline void kchannelAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct KernelChannel *pResource, RsResourceRef *pReference) {
    pResource->__kchannelAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS kchannelControl_Prologue_DISPATCH(struct KernelChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kchannelControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kchannelGetRegBaseOffsetAndSize_DISPATCH(struct KernelChannel *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__kchannelGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS kchannelInternalControlForward_DISPATCH(struct KernelChannel *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__kchannelInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS kchannelUnmapFrom_DISPATCH(struct KernelChannel *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__kchannelUnmapFrom__(pResource, pParams);
}

static inline void kchannelControl_Epilogue_DISPATCH(struct KernelChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__kchannelControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kchannelControlLookup_DISPATCH(struct KernelChannel *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__kchannelControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle kchannelGetInternalObjectHandle_DISPATCH(struct KernelChannel *pGpuResource) {
    return pGpuResource->__kchannelGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS kchannelControl_DISPATCH(struct KernelChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__kchannelControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS kchannelGetMemoryMappingDescriptor_DISPATCH(struct KernelChannel *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__kchannelGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS kchannelControlFilter_DISPATCH(struct KernelChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kchannelControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kchannelUnregisterEvent_DISPATCH(struct KernelChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__kchannelUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS kchannelControlSerialization_Prologue_DISPATCH(struct KernelChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kchannelControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool kchannelCanCopy_DISPATCH(struct KernelChannel *pResource) {
    return pResource->__kchannelCanCopy__(pResource);
}

static inline void kchannelPreDestruct_DISPATCH(struct KernelChannel *pResource) {
    pResource->__kchannelPreDestruct__(pResource);
}

static inline NV_STATUS kchannelIsDuplicate_DISPATCH(struct KernelChannel *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__kchannelIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void kchannelControlSerialization_Epilogue_DISPATCH(struct KernelChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__kchannelControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline PEVENTNOTIFICATION *kchannelGetNotificationListPtr_DISPATCH(struct KernelChannel *pNotifier) {
    return pNotifier->__kchannelGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *kchannelGetNotificationShare_DISPATCH(struct KernelChannel *pNotifier) {
    return pNotifier->__kchannelGetNotificationShare__(pNotifier);
}

static inline NvBool kchannelAccessCallback_DISPATCH(struct KernelChannel *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__kchannelAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvU32 kchannelGetDebugTag(const struct KernelChannel *pKernelChannel) {
    if (pKernelChannel == ((void *)0))
        return 4294967295U;
    return pKernelChannel->ChID;
}

static inline NvBool kchannelIsCtxBufferAllocSkipped(struct KernelChannel *pKernelChannel) {
    return pKernelChannel->bSkipCtxBufferAlloc;
}

static inline NvU32 kchannelGetSubctxId(struct KernelChannel *pKernelChannel) {
    return pKernelChannel->subctxId;
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
#else //__nvoc_kernel_channel_h_disabled
#define kchannelRegisterChild(pKernelChannel, pObject) kchannelRegisterChild_IMPL(pKernelChannel, pObject)
#endif //__nvoc_kernel_channel_h_disabled

NV_STATUS kchannelDeregisterChild_IMPL(struct KernelChannel *pKernelChannel, ChannelDescendant *pObject);

#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelDeregisterChild(struct KernelChannel *pKernelChannel, ChannelDescendant *pObject) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelDeregisterChild(pKernelChannel, pObject) kchannelDeregisterChild_IMPL(pKernelChannel, pObject)
#endif //__nvoc_kernel_channel_h_disabled

void kchannelNotifyGeneric_IMPL(struct KernelChannel *pKernelChannel, NvU32 notifyIndex, void *pNotifyParams, NvU32 notifyParamsSize);

#ifdef __nvoc_kernel_channel_h_disabled
static inline void kchannelNotifyGeneric(struct KernelChannel *pKernelChannel, NvU32 notifyIndex, void *pNotifyParams, NvU32 notifyParamsSize) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelNotifyGeneric(pKernelChannel, notifyIndex, pNotifyParams, notifyParamsSize) kchannelNotifyGeneric_IMPL(pKernelChannel, notifyIndex, pNotifyParams, notifyParamsSize)
#endif //__nvoc_kernel_channel_h_disabled

NvBool kchannelCheckIsUserMode_IMPL(struct KernelChannel *pKernelChannel);

#ifdef __nvoc_kernel_channel_h_disabled
static inline NvBool kchannelCheckIsUserMode(struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelCheckIsUserMode(pKernelChannel) kchannelCheckIsUserMode_IMPL(pKernelChannel)
#endif //__nvoc_kernel_channel_h_disabled

NvBool kchannelCheckIsKernel_IMPL(struct KernelChannel *pKernelChannel);

#ifdef __nvoc_kernel_channel_h_disabled
static inline NvBool kchannelCheckIsKernel(struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelCheckIsKernel(pKernelChannel) kchannelCheckIsKernel_IMPL(pKernelChannel)
#endif //__nvoc_kernel_channel_h_disabled

NvBool kchannelCheckIsAdmin_IMPL(struct KernelChannel *pKernelChannel);

#ifdef __nvoc_kernel_channel_h_disabled
static inline NvBool kchannelCheckIsAdmin(struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelCheckIsAdmin(pKernelChannel) kchannelCheckIsAdmin_IMPL(pKernelChannel)
#endif //__nvoc_kernel_channel_h_disabled

NV_STATUS kchannelBindToRunlist_IMPL(struct KernelChannel *pKernelChannel, RM_ENGINE_TYPE localRmEngineType, ENGDESCRIPTOR engineDesc);

#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelBindToRunlist(struct KernelChannel *pKernelChannel, RM_ENGINE_TYPE localRmEngineType, ENGDESCRIPTOR engineDesc) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelBindToRunlist(pKernelChannel, localRmEngineType, engineDesc) kchannelBindToRunlist_IMPL(pKernelChannel, localRmEngineType, engineDesc)
#endif //__nvoc_kernel_channel_h_disabled

NV_STATUS kchannelSetEngineContextMemDesc_IMPL(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 engine, MEMORY_DESCRIPTOR *pMemDesc);

#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelSetEngineContextMemDesc(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 engine, MEMORY_DESCRIPTOR *pMemDesc) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelSetEngineContextMemDesc(pGpu, pKernelChannel, engine, pMemDesc) kchannelSetEngineContextMemDesc_IMPL(pGpu, pKernelChannel, engine, pMemDesc)
#endif //__nvoc_kernel_channel_h_disabled

NV_STATUS kchannelMapEngineCtxBuf_IMPL(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 engine);

#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelMapEngineCtxBuf(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 engine) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelMapEngineCtxBuf(pGpu, pKernelChannel, engine) kchannelMapEngineCtxBuf_IMPL(pGpu, pKernelChannel, engine)
#endif //__nvoc_kernel_channel_h_disabled

NV_STATUS kchannelUnmapEngineCtxBuf_IMPL(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 engine);

#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelUnmapEngineCtxBuf(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvU32 engine) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelUnmapEngineCtxBuf(pGpu, pKernelChannel, engine) kchannelUnmapEngineCtxBuf_IMPL(pGpu, pKernelChannel, engine)
#endif //__nvoc_kernel_channel_h_disabled

NV_STATUS kchannelCheckBcStateCurrent_IMPL(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel);

#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelCheckBcStateCurrent(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelCheckBcStateCurrent(pGpu, pKernelChannel) kchannelCheckBcStateCurrent_IMPL(pGpu, pKernelChannel)
#endif //__nvoc_kernel_channel_h_disabled

NV_STATUS kchannelUpdateWorkSubmitTokenNotifIndex_IMPL(struct OBJGPU *pGpu, struct KernelChannel *arg0, NvU32 index);

#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelUpdateWorkSubmitTokenNotifIndex(struct OBJGPU *pGpu, struct KernelChannel *arg0, NvU32 index) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelUpdateWorkSubmitTokenNotifIndex(pGpu, arg0, index) kchannelUpdateWorkSubmitTokenNotifIndex_IMPL(pGpu, arg0, index)
#endif //__nvoc_kernel_channel_h_disabled

NV_STATUS kchannelNotifyWorkSubmitToken_IMPL(struct OBJGPU *pGpu, struct KernelChannel *arg0, NvU32 token);

#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelNotifyWorkSubmitToken(struct OBJGPU *pGpu, struct KernelChannel *arg0, NvU32 token) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelNotifyWorkSubmitToken(pGpu, arg0, token) kchannelNotifyWorkSubmitToken_IMPL(pGpu, arg0, token)
#endif //__nvoc_kernel_channel_h_disabled

NV_STATUS kchannelMapUserD_IMPL(struct OBJGPU *pGpu, struct KernelChannel *arg0, RS_PRIV_LEVEL arg1, NvU64 arg2, NvU32 arg3, NvP64 *arg4, NvP64 *arg5);

#ifdef __nvoc_kernel_channel_h_disabled
static inline NV_STATUS kchannelMapUserD(struct OBJGPU *pGpu, struct KernelChannel *arg0, RS_PRIV_LEVEL arg1, NvU64 arg2, NvU32 arg3, NvP64 *arg4, NvP64 *arg5) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelMapUserD(pGpu, arg0, arg1, arg2, arg3, arg4, arg5) kchannelMapUserD_IMPL(pGpu, arg0, arg1, arg2, arg3, arg4, arg5)
#endif //__nvoc_kernel_channel_h_disabled

void kchannelUnmapUserD_IMPL(struct OBJGPU *pGpu, struct KernelChannel *arg0, RS_PRIV_LEVEL arg1, NvP64 *arg2, NvP64 *arg3);

#ifdef __nvoc_kernel_channel_h_disabled
static inline void kchannelUnmapUserD(struct OBJGPU *pGpu, struct KernelChannel *arg0, RS_PRIV_LEVEL arg1, NvP64 *arg2, NvP64 *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelUnmapUserD(pGpu, arg0, arg1, arg2, arg3) kchannelUnmapUserD_IMPL(pGpu, arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_channel_h_disabled

NV_STATUS kchannelGetFromDualHandle_IMPL(struct RsClient *arg0, NvHandle arg1, struct KernelChannel **arg2);

#define kchannelGetFromDualHandle(arg0, arg1, arg2) kchannelGetFromDualHandle_IMPL(arg0, arg1, arg2)
NV_STATUS kchannelGetFromDualHandleRestricted_IMPL(struct RsClient *arg0, NvHandle arg1, struct KernelChannel **arg2);

#define kchannelGetFromDualHandleRestricted(arg0, arg1, arg2) kchannelGetFromDualHandleRestricted_IMPL(arg0, arg1, arg2)
NvU32 kchannelGetGfid_IMPL(struct KernelChannel *pKernelChannel);

#ifdef __nvoc_kernel_channel_h_disabled
static inline NvU32 kchannelGetGfid(struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannel was disabled!");
    return 0;
}
#else //__nvoc_kernel_channel_h_disabled
#define kchannelGetGfid(pKernelChannel) kchannelGetGfid_IMPL(pKernelChannel)
#endif //__nvoc_kernel_channel_h_disabled

#undef PRIVATE_FIELD

#ifndef NVOC_KERNEL_CHANNEL_H_PRIVATE_ACCESS_ALLOWED
#ifndef __nvoc_kernel_channel_h_disabled
#undef kchannelRotateSecureChannelIv
NV_STATUS NVOC_PRIVATE_FUNCTION(kchannelRotateSecureChannelIv)(struct KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation, NvU32 *encryptIv, NvU32 *decryptIv);
#endif //__nvoc_kernel_channel_h_disabled

#ifndef __nvoc_kernel_channel_h_disabled
#undef kchannelRetrieveKmb
NV_STATUS NVOC_PRIVATE_FUNCTION(kchannelRetrieveKmb)(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation, NvBool includeSecrets, CC_KMB *keyMaterialBundle);
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
#define KERNEL_CHANNEL_SW_STATE_CPU_MAP        NVBIT(0) //UserD is mapped
#define KERNEL_CHANNEL_SW_STATE_RUNLIST_SET    NVBIT(1) // RunlistId is set

NvBool kchannelIsCpuMapped(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel);
void kchannelSetCpuMapped(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvBool bCpuMapped);
NvBool kchannelIsRunlistSet(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel);
void kchannelSetRunlistSet(struct OBJGPU *pGpu, struct KernelChannel *pKernelChannel, NvBool bRunlistSet);

#endif // KERNEL_CHANNEL_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_CHANNEL_NVOC_H_
