
#ifndef _G_KERNEL_GRAPHICS_CONTEXT_NVOC_H_
#define _G_KERNEL_GRAPHICS_CONTEXT_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_kernel_graphics_context_nvoc.h"

#ifndef _KERNEL_GRAPHICS_CONTEXT_H_
#define _KERNEL_GRAPHICS_CONTEXT_H_

#include "gpu/gpu_resource.h"
#include "resserv/rs_server.h"
#include "resserv/rs_resource.h"
#include "ctrl/ctrl83de.h"
#include "ctrl/ctrl0090.h"
#include "mmu/gmmu_fmt.h"
#include "gpu/gpu_halspec.h"
#include "mem_mgr/vaddr_list.h"
#include "kernel/gpu/gr/kernel_graphics_context_buffers.h"


struct KernelChannel;

#ifndef __NVOC_CLASS_KernelChannel_TYPEDEF__
#define __NVOC_CLASS_KernelChannel_TYPEDEF__
typedef struct KernelChannel KernelChannel;
#endif /* __NVOC_CLASS_KernelChannel_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelChannel
#define __nvoc_class_id_KernelChannel 0x5d8d70
#endif /* __nvoc_class_id_KernelChannel */



struct KernelChannelGroupApi;

#ifndef __NVOC_CLASS_KernelChannelGroupApi_TYPEDEF__
#define __NVOC_CLASS_KernelChannelGroupApi_TYPEDEF__
typedef struct KernelChannelGroupApi KernelChannelGroupApi;
#endif /* __NVOC_CLASS_KernelChannelGroupApi_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelChannelGroupApi
#define __nvoc_class_id_KernelChannelGroupApi 0x2b5b80
#endif /* __nvoc_class_id_KernelChannelGroupApi */



struct KernelSMDebuggerSession;

#ifndef __NVOC_CLASS_KernelSMDebuggerSession_TYPEDEF__
#define __NVOC_CLASS_KernelSMDebuggerSession_TYPEDEF__
typedef struct KernelSMDebuggerSession KernelSMDebuggerSession;
#endif /* __NVOC_CLASS_KernelSMDebuggerSession_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelSMDebuggerSession
#define __nvoc_class_id_KernelSMDebuggerSession 0x4adc81
#endif /* __nvoc_class_id_KernelSMDebuggerSession */


MAKE_LIST(KernelSMDebuggerSessionList, KernelSMDebuggerSession *);

typedef struct
{
    const NV_ADDRESS_SPACE *pAllocList;
    NvU32 cpuAttr;
} GR_BUFFER_ATTR;

typedef struct NV_CONTEXT_BUFFER
{
    MEMORY_DESCRIPTOR *pMemDesc;
    VA_LIST            vAddrList;
} NV_CONTEXT_BUFFER;

typedef enum
{
    GR_OBJECT_TYPE_3D,
    GR_OBJECT_TYPE_2D,
    GR_OBJECT_TYPE_MEM,
    GR_OBJECT_TYPE_COMPUTE,
    GR_OBJECT_TYPE_INVALID,
} GR_OBJECT_TYPE;

/* Structure used when allocating/deallocating GFxP buffers for vGPU  */
typedef struct VGPU_GFXP_BUFFERS
{
    NvBool      bIsBufferAllocated;
    NvU32       refCountChannel; /* count of channels using these buffers */
    NvHandle    hMemory[NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND_BUFFERS_CONTEXT_POOL];
    NvHandle    hDma[NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND_BUFFERS_CONTEXT_POOL];
    NvU64       dmaOffset[NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND_BUFFERS_CONTEXT_POOL];
} VGPU_GFXP_BUFFERS;

typedef struct
{
    MEMORY_DESCRIPTOR *memDesc[GR_GLOBALCTX_BUFFER_COUNT];
    NvBool             bAllocated;

    // Tracks whether Physical has initialized the memory descriptor for the promoted Kernel buffer
    NvBool             bInitialized[GR_GLOBALCTX_BUFFER_COUNT];
} GR_GLOBALCTX_BUFFERS;

struct KernelGraphicsContextUnicast;
typedef struct KernelGraphicsContextUnicast KernelGraphicsContextUnicast;


struct KernelGraphics;

#ifndef __NVOC_CLASS_KernelGraphics_TYPEDEF__
#define __NVOC_CLASS_KernelGraphics_TYPEDEF__
typedef struct KernelGraphics KernelGraphics;
#endif /* __NVOC_CLASS_KernelGraphics_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGraphics
#define __nvoc_class_id_KernelGraphics 0xea3fa9
#endif /* __nvoc_class_id_KernelGraphics */



struct KernelGraphicsContextShared;

#ifndef __NVOC_CLASS_KernelGraphicsContextShared_TYPEDEF__
#define __NVOC_CLASS_KernelGraphicsContextShared_TYPEDEF__
typedef struct KernelGraphicsContextShared KernelGraphicsContextShared;
#endif /* __NVOC_CLASS_KernelGraphicsContextShared_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGraphicsContextShared
#define __nvoc_class_id_KernelGraphicsContextShared 0xe7abeb
#endif /* __nvoc_class_id_KernelGraphicsContextShared */



struct KernelGraphicsObject;

#ifndef __NVOC_CLASS_KernelGraphicsObject_TYPEDEF__
#define __NVOC_CLASS_KernelGraphicsObject_TYPEDEF__
typedef struct KernelGraphicsObject KernelGraphicsObject;
#endif /* __NVOC_CLASS_KernelGraphicsObject_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGraphicsObject
#define __nvoc_class_id_KernelGraphicsObject 0x097648
#endif /* __nvoc_class_id_KernelGraphicsObject */



/*!
 * KernelGraphicsContext is a logical abstraction of GrContext object (Per Channel or
 * per ChannelGroup) in Kernel side. The Public API of the GrContext is exposed through
 * this object, and any interfaces which do not manage the underlying HW can be managed
 * by this object.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_GRAPHICS_CONTEXT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelGraphicsContext;
struct NVOC_METADATA__GpuResource;
struct NVOC_VTABLE__KernelGraphicsContext;


struct KernelGraphicsContext {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelGraphicsContext *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResource __nvoc_base_GpuResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super
    struct KernelGraphicsContext *__nvoc_pbase_KernelGraphicsContext;    // kgrctx

    // Vtable with 7 per-object function pointers
    NvBool (*__kgrctxShouldPreAllocPmBuffer__)(struct OBJGPU *, struct KernelGraphicsContext * /*this*/, struct KernelChannel *);  // halified (2 hals) body
    GR_GLOBALCTX_BUFFER (*__kgrctxGetRegisterAccessMapId__)(struct OBJGPU *, struct KernelGraphicsContext * /*this*/, struct KernelChannel *);  // halified (2 hals)
    NV_STATUS (*__kgrctxCtrlGetTpcPartitionMode__)(struct KernelGraphicsContext * /*this*/, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *);  // exported (id=0x900103)
    NV_STATUS (*__kgrctxCtrlSetTpcPartitionMode__)(struct KernelGraphicsContext * /*this*/, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *);  // exported (id=0x900101)
    NV_STATUS (*__kgrctxCtrlGetMMUDebugMode__)(struct KernelGraphicsContext * /*this*/, NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS *);  // exported (id=0x900105)
    NV_STATUS (*__kgrctxCtrlProgramVidmemPromote__)(struct KernelGraphicsContext * /*this*/, NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *);  // exported (id=0x900107)
    NV_STATUS (*__kgrctxCtrlSetLgSectorPromotion__)(struct KernelGraphicsContext * /*this*/, NV0090_CTRL_SET_LG_SECTOR_PROMOTION_PARAMS *);  // exported (id=0x90010b)

    // Data members
    struct KernelGraphicsContextShared *PRIVATE_FIELD(pShared);
    VGPU_GFXP_BUFFERS *PRIVATE_FIELD(pVgpuGfxpBuffers);
};


struct KernelGraphicsContext_PRIVATE {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelGraphicsContext *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResource __nvoc_base_GpuResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super
    struct KernelGraphicsContext *__nvoc_pbase_KernelGraphicsContext;    // kgrctx

    // Vtable with 7 per-object function pointers
    NvBool (*__kgrctxShouldPreAllocPmBuffer__)(struct OBJGPU *, struct KernelGraphicsContext * /*this*/, struct KernelChannel *);  // halified (2 hals) body
    GR_GLOBALCTX_BUFFER (*__kgrctxGetRegisterAccessMapId__)(struct OBJGPU *, struct KernelGraphicsContext * /*this*/, struct KernelChannel *);  // halified (2 hals)
    NV_STATUS (*__kgrctxCtrlGetTpcPartitionMode__)(struct KernelGraphicsContext * /*this*/, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *);  // exported (id=0x900103)
    NV_STATUS (*__kgrctxCtrlSetTpcPartitionMode__)(struct KernelGraphicsContext * /*this*/, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *);  // exported (id=0x900101)
    NV_STATUS (*__kgrctxCtrlGetMMUDebugMode__)(struct KernelGraphicsContext * /*this*/, NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS *);  // exported (id=0x900105)
    NV_STATUS (*__kgrctxCtrlProgramVidmemPromote__)(struct KernelGraphicsContext * /*this*/, NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *);  // exported (id=0x900107)
    NV_STATUS (*__kgrctxCtrlSetLgSectorPromotion__)(struct KernelGraphicsContext * /*this*/, NV0090_CTRL_SET_LG_SECTOR_PROMOTION_PARAMS *);  // exported (id=0x90010b)

    // Data members
    struct KernelGraphicsContextShared *pShared;
    VGPU_GFXP_BUFFERS *pVgpuGfxpBuffers;
};


// Vtable with 25 per-class function pointers
struct NVOC_VTABLE__KernelGraphicsContext {
    NvBool (*__kgrctxCanCopy__)(struct KernelGraphicsContext * /*this*/);  // inline virtual override (res) base (gpures) body
    NvHandle (*__kgrctxGetInternalObjectHandle__)(struct KernelGraphicsContext * /*this*/);  // virtual override (gpures) base (gpures)
    NV_STATUS (*__kgrctxControl__)(struct KernelGraphicsContext * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kgrctxMap__)(struct KernelGraphicsContext * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kgrctxUnmap__)(struct KernelGraphicsContext * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__kgrctxShareCallback__)(struct KernelGraphicsContext * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kgrctxGetRegBaseOffsetAndSize__)(struct KernelGraphicsContext * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kgrctxGetMapAddrSpace__)(struct KernelGraphicsContext * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kgrctxInternalControlForward__)(struct KernelGraphicsContext * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvBool (*__kgrctxAccessCallback__)(struct KernelGraphicsContext * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kgrctxGetMemInterMapParams__)(struct KernelGraphicsContext * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kgrctxCheckMemInterUnmap__)(struct KernelGraphicsContext * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kgrctxGetMemoryMappingDescriptor__)(struct KernelGraphicsContext * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kgrctxControlSerialization_Prologue__)(struct KernelGraphicsContext * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__kgrctxControlSerialization_Epilogue__)(struct KernelGraphicsContext * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kgrctxControl_Prologue__)(struct KernelGraphicsContext * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__kgrctxControl_Epilogue__)(struct KernelGraphicsContext * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kgrctxIsDuplicate__)(struct KernelGraphicsContext * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__kgrctxPreDestruct__)(struct KernelGraphicsContext * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__kgrctxControlFilter__)(struct KernelGraphicsContext * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__kgrctxIsPartialUnmapSupported__)(struct KernelGraphicsContext * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__kgrctxMapTo__)(struct KernelGraphicsContext * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__kgrctxUnmapFrom__)(struct KernelGraphicsContext * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__kgrctxGetRefCount__)(struct KernelGraphicsContext * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__kgrctxAddAdditionalDependants__)(struct RsClient *, struct KernelGraphicsContext * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelGraphicsContext {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_VTABLE__KernelGraphicsContext vtable;
};

#ifndef __NVOC_CLASS_KernelGraphicsContext_TYPEDEF__
#define __NVOC_CLASS_KernelGraphicsContext_TYPEDEF__
typedef struct KernelGraphicsContext KernelGraphicsContext;
#endif /* __NVOC_CLASS_KernelGraphicsContext_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGraphicsContext
#define __nvoc_class_id_KernelGraphicsContext 0x7ead09
#endif /* __nvoc_class_id_KernelGraphicsContext */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGraphicsContext;

#define __staticCast_KernelGraphicsContext(pThis) \
    ((pThis)->__nvoc_pbase_KernelGraphicsContext)

#ifdef __nvoc_kernel_graphics_context_h_disabled
#define __dynamicCast_KernelGraphicsContext(pThis) ((KernelGraphicsContext*) NULL)
#else //__nvoc_kernel_graphics_context_h_disabled
#define __dynamicCast_KernelGraphicsContext(pThis) \
    ((KernelGraphicsContext*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelGraphicsContext)))
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS __nvoc_objCreateDynamic_KernelGraphicsContext(KernelGraphicsContext**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelGraphicsContext(KernelGraphicsContext**, Dynamic*, NvU32, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_KernelGraphicsContext(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_KernelGraphicsContext((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define kgrctxCanCopy_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__kgrctxCanCopy__
#define kgrctxCanCopy(arg_this) kgrctxCanCopy_DISPATCH(arg_this)
#define kgrctxGetInternalObjectHandle_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__kgrctxGetInternalObjectHandle__
#define kgrctxGetInternalObjectHandle(arg_this) kgrctxGetInternalObjectHandle_DISPATCH(arg_this)
#define kgrctxShouldPreAllocPmBuffer_FNPTR(pKernelGraphicsContext) pKernelGraphicsContext->__kgrctxShouldPreAllocPmBuffer__
#define kgrctxShouldPreAllocPmBuffer(pGpu, pKernelGraphicsContext, pKernelChannel) kgrctxShouldPreAllocPmBuffer_DISPATCH(pGpu, pKernelGraphicsContext, pKernelChannel)
#define kgrctxShouldPreAllocPmBuffer_HAL(pGpu, pKernelGraphicsContext, pKernelChannel) kgrctxShouldPreAllocPmBuffer_DISPATCH(pGpu, pKernelGraphicsContext, pKernelChannel)
#define kgrctxGetRegisterAccessMapId_FNPTR(arg_this) arg_this->__kgrctxGetRegisterAccessMapId__
#define kgrctxGetRegisterAccessMapId(arg1, arg_this, arg3) kgrctxGetRegisterAccessMapId_DISPATCH(arg1, arg_this, arg3)
#define kgrctxGetRegisterAccessMapId_HAL(arg1, arg_this, arg3) kgrctxGetRegisterAccessMapId_DISPATCH(arg1, arg_this, arg3)
#define kgrctxCtrlGetTpcPartitionMode_FNPTR(pKernelGraphicsContext) pKernelGraphicsContext->__kgrctxCtrlGetTpcPartitionMode__
#define kgrctxCtrlGetTpcPartitionMode(pKernelGraphicsContext, pParams) kgrctxCtrlGetTpcPartitionMode_DISPATCH(pKernelGraphicsContext, pParams)
#define kgrctxCtrlSetTpcPartitionMode_FNPTR(pKernelGraphicsContext) pKernelGraphicsContext->__kgrctxCtrlSetTpcPartitionMode__
#define kgrctxCtrlSetTpcPartitionMode(pKernelGraphicsContext, pParams) kgrctxCtrlSetTpcPartitionMode_DISPATCH(pKernelGraphicsContext, pParams)
#define kgrctxCtrlGetMMUDebugMode_FNPTR(pKernelGraphicsContext) pKernelGraphicsContext->__kgrctxCtrlGetMMUDebugMode__
#define kgrctxCtrlGetMMUDebugMode(pKernelGraphicsContext, pParams) kgrctxCtrlGetMMUDebugMode_DISPATCH(pKernelGraphicsContext, pParams)
#define kgrctxCtrlProgramVidmemPromote_FNPTR(pKernelGraphicsContext) pKernelGraphicsContext->__kgrctxCtrlProgramVidmemPromote__
#define kgrctxCtrlProgramVidmemPromote(pKernelGraphicsContext, pParams) kgrctxCtrlProgramVidmemPromote_DISPATCH(pKernelGraphicsContext, pParams)
#define kgrctxCtrlSetLgSectorPromotion_FNPTR(pKernelGraphicsContext) pKernelGraphicsContext->__kgrctxCtrlSetLgSectorPromotion__
#define kgrctxCtrlSetLgSectorPromotion(pKernelGraphicsContext, pParams) kgrctxCtrlSetLgSectorPromotion_DISPATCH(pKernelGraphicsContext, pParams)
#define kgrctxControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresControl__
#define kgrctxControl(pGpuResource, pCallContext, pParams) kgrctxControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define kgrctxMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresMap__
#define kgrctxMap(pGpuResource, pCallContext, pParams, pCpuMapping) kgrctxMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define kgrctxUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define kgrctxUnmap(pGpuResource, pCallContext, pCpuMapping) kgrctxUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define kgrctxShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define kgrctxShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) kgrctxShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define kgrctxGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define kgrctxGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) kgrctxGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define kgrctxGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define kgrctxGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) kgrctxGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define kgrctxInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define kgrctxInternalControlForward(pGpuResource, command, pParams, size) kgrctxInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define kgrctxAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define kgrctxAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) kgrctxAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define kgrctxGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define kgrctxGetMemInterMapParams(pRmResource, pParams) kgrctxGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define kgrctxCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define kgrctxCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) kgrctxCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define kgrctxGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define kgrctxGetMemoryMappingDescriptor(pRmResource, ppMemDesc) kgrctxGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define kgrctxControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define kgrctxControlSerialization_Prologue(pResource, pCallContext, pParams) kgrctxControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kgrctxControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define kgrctxControlSerialization_Epilogue(pResource, pCallContext, pParams) kgrctxControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kgrctxControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define kgrctxControl_Prologue(pResource, pCallContext, pParams) kgrctxControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kgrctxControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define kgrctxControl_Epilogue(pResource, pCallContext, pParams) kgrctxControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kgrctxIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define kgrctxIsDuplicate(pResource, hMemory, pDuplicate) kgrctxIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define kgrctxPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define kgrctxPreDestruct(pResource) kgrctxPreDestruct_DISPATCH(pResource)
#define kgrctxControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define kgrctxControlFilter(pResource, pCallContext, pParams) kgrctxControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define kgrctxIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define kgrctxIsPartialUnmapSupported(pResource) kgrctxIsPartialUnmapSupported_DISPATCH(pResource)
#define kgrctxMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define kgrctxMapTo(pResource, pParams) kgrctxMapTo_DISPATCH(pResource, pParams)
#define kgrctxUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define kgrctxUnmapFrom(pResource, pParams) kgrctxUnmapFrom_DISPATCH(pResource, pParams)
#define kgrctxGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define kgrctxGetRefCount(pResource) kgrctxGetRefCount_DISPATCH(pResource)
#define kgrctxAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define kgrctxAddAdditionalDependants(pClient, pResource, pReference) kgrctxAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool kgrctxCanCopy_DISPATCH(struct KernelGraphicsContext *arg_this) {
    return arg_this->__nvoc_metadata_ptr->vtable.__kgrctxCanCopy__(arg_this);
}

static inline NvHandle kgrctxGetInternalObjectHandle_DISPATCH(struct KernelGraphicsContext *arg_this) {
    return arg_this->__nvoc_metadata_ptr->vtable.__kgrctxGetInternalObjectHandle__(arg_this);
}

static inline NvBool kgrctxShouldPreAllocPmBuffer_DISPATCH(struct OBJGPU *pGpu, struct KernelGraphicsContext *pKernelGraphicsContext, struct KernelChannel *pKernelChannel) {
    return pKernelGraphicsContext->__kgrctxShouldPreAllocPmBuffer__(pGpu, pKernelGraphicsContext, pKernelChannel);
}

static inline GR_GLOBALCTX_BUFFER kgrctxGetRegisterAccessMapId_DISPATCH(struct OBJGPU *arg1, struct KernelGraphicsContext *arg_this, struct KernelChannel *arg3) {
    return arg_this->__kgrctxGetRegisterAccessMapId__(arg1, arg_this, arg3);
}

static inline NV_STATUS kgrctxCtrlGetTpcPartitionMode_DISPATCH(struct KernelGraphicsContext *pKernelGraphicsContext, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams) {
    return pKernelGraphicsContext->__kgrctxCtrlGetTpcPartitionMode__(pKernelGraphicsContext, pParams);
}

static inline NV_STATUS kgrctxCtrlSetTpcPartitionMode_DISPATCH(struct KernelGraphicsContext *pKernelGraphicsContext, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams) {
    return pKernelGraphicsContext->__kgrctxCtrlSetTpcPartitionMode__(pKernelGraphicsContext, pParams);
}

static inline NV_STATUS kgrctxCtrlGetMMUDebugMode_DISPATCH(struct KernelGraphicsContext *pKernelGraphicsContext, NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS *pParams) {
    return pKernelGraphicsContext->__kgrctxCtrlGetMMUDebugMode__(pKernelGraphicsContext, pParams);
}

static inline NV_STATUS kgrctxCtrlProgramVidmemPromote_DISPATCH(struct KernelGraphicsContext *pKernelGraphicsContext, NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *pParams) {
    return pKernelGraphicsContext->__kgrctxCtrlProgramVidmemPromote__(pKernelGraphicsContext, pParams);
}

static inline NV_STATUS kgrctxCtrlSetLgSectorPromotion_DISPATCH(struct KernelGraphicsContext *pKernelGraphicsContext, NV0090_CTRL_SET_LG_SECTOR_PROMOTION_PARAMS *pParams) {
    return pKernelGraphicsContext->__kgrctxCtrlSetLgSectorPromotion__(pKernelGraphicsContext, pParams);
}

static inline NV_STATUS kgrctxControl_DISPATCH(struct KernelGraphicsContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kgrctxControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS kgrctxMap_DISPATCH(struct KernelGraphicsContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kgrctxMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS kgrctxUnmap_DISPATCH(struct KernelGraphicsContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kgrctxUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool kgrctxShareCallback_DISPATCH(struct KernelGraphicsContext *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kgrctxShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS kgrctxGetRegBaseOffsetAndSize_DISPATCH(struct KernelGraphicsContext *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kgrctxGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS kgrctxGetMapAddrSpace_DISPATCH(struct KernelGraphicsContext *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kgrctxGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS kgrctxInternalControlForward_DISPATCH(struct KernelGraphicsContext *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kgrctxInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvBool kgrctxAccessCallback_DISPATCH(struct KernelGraphicsContext *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__kgrctxAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS kgrctxGetMemInterMapParams_DISPATCH(struct KernelGraphicsContext *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__kgrctxGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS kgrctxCheckMemInterUnmap_DISPATCH(struct KernelGraphicsContext *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__kgrctxCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS kgrctxGetMemoryMappingDescriptor_DISPATCH(struct KernelGraphicsContext *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__kgrctxGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS kgrctxControlSerialization_Prologue_DISPATCH(struct KernelGraphicsContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kgrctxControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void kgrctxControlSerialization_Epilogue_DISPATCH(struct KernelGraphicsContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__kgrctxControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kgrctxControl_Prologue_DISPATCH(struct KernelGraphicsContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kgrctxControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void kgrctxControl_Epilogue_DISPATCH(struct KernelGraphicsContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__kgrctxControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kgrctxIsDuplicate_DISPATCH(struct KernelGraphicsContext *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__kgrctxIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void kgrctxPreDestruct_DISPATCH(struct KernelGraphicsContext *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__kgrctxPreDestruct__(pResource);
}

static inline NV_STATUS kgrctxControlFilter_DISPATCH(struct KernelGraphicsContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kgrctxControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool kgrctxIsPartialUnmapSupported_DISPATCH(struct KernelGraphicsContext *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__kgrctxIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS kgrctxMapTo_DISPATCH(struct KernelGraphicsContext *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kgrctxMapTo__(pResource, pParams);
}

static inline NV_STATUS kgrctxUnmapFrom_DISPATCH(struct KernelGraphicsContext *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kgrctxUnmapFrom__(pResource, pParams);
}

static inline NvU32 kgrctxGetRefCount_DISPATCH(struct KernelGraphicsContext *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__kgrctxGetRefCount__(pResource);
}

static inline void kgrctxAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct KernelGraphicsContext *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__kgrctxAddAdditionalDependants__(pClient, pResource, pReference);
}

NvBool kgrctxShouldManageCtxBuffers_KERNEL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, NvU32 gfid);


#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NvBool kgrctxShouldManageCtxBuffers(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxShouldManageCtxBuffers(arg1, arg2, gfid) kgrctxShouldManageCtxBuffers_KERNEL(arg1, arg2, gfid)
#endif //__nvoc_kernel_graphics_context_h_disabled

#define kgrctxShouldManageCtxBuffers_HAL(arg1, arg2, gfid) kgrctxShouldManageCtxBuffers(arg1, arg2, gfid)

NvBool kgrctxShouldCleanup_KERNEL(struct OBJGPU *pGpu, struct KernelGraphicsContext *pKernelGraphicsContext);


#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NvBool kgrctxShouldCleanup(struct OBJGPU *pGpu, struct KernelGraphicsContext *pKernelGraphicsContext) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxShouldCleanup(pGpu, pKernelGraphicsContext) kgrctxShouldCleanup_KERNEL(pGpu, pKernelGraphicsContext)
#endif //__nvoc_kernel_graphics_context_h_disabled

#define kgrctxShouldCleanup_HAL(pGpu, pKernelGraphicsContext) kgrctxShouldCleanup(pGpu, pKernelGraphicsContext)

void kgrctxUnmapBuffers_KERNEL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, KernelGraphicsContextUnicast *arg3, struct KernelChannel *arg4);


#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxUnmapBuffers(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, KernelGraphicsContextUnicast *arg3, struct KernelChannel *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxUnmapBuffers(arg1, arg2, arg3, arg4) kgrctxUnmapBuffers_KERNEL(arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_graphics_context_h_disabled

#define kgrctxUnmapBuffers_HAL(arg1, arg2, arg3, arg4) kgrctxUnmapBuffers(arg1, arg2, arg3, arg4)

NV_STATUS kgrctxUnmapCtxBuffers_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphicsObject *arg3, struct KernelGraphics *arg4, NvBool bDestructor);


#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxUnmapCtxBuffers(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphicsObject *arg3, struct KernelGraphics *arg4, NvBool bDestructor) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxUnmapCtxBuffers(arg1, arg2, arg3, arg4, bDestructor) kgrctxUnmapCtxBuffers_IMPL(arg1, arg2, arg3, arg4, bDestructor)
#endif //__nvoc_kernel_graphics_context_h_disabled

#define kgrctxUnmapCtxBuffers_HAL(arg1, arg2, arg3, arg4, bDestructor) kgrctxUnmapCtxBuffers(arg1, arg2, arg3, arg4, bDestructor)

void kgrctxIncObjectCount_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, NvU32 classNum);


#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxIncObjectCount(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, NvU32 classNum) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxIncObjectCount(arg1, arg2, classNum) kgrctxIncObjectCount_IMPL(arg1, arg2, classNum)
#endif //__nvoc_kernel_graphics_context_h_disabled

#define kgrctxIncObjectCount_HAL(arg1, arg2, classNum) kgrctxIncObjectCount(arg1, arg2, classNum)

void kgrctxDecObjectCount_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, NvU32 classNum);


#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxDecObjectCount(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, NvU32 classNum) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxDecObjectCount(arg1, arg2, classNum) kgrctxDecObjectCount_IMPL(arg1, arg2, classNum)
#endif //__nvoc_kernel_graphics_context_h_disabled

#define kgrctxDecObjectCount_HAL(arg1, arg2, classNum) kgrctxDecObjectCount(arg1, arg2, classNum)

static inline NvBool kgrctxCanCopy_e661f0(struct KernelGraphicsContext *arg1) {
    return NV_TRUE;
}

NvHandle kgrctxGetInternalObjectHandle_IMPL(struct KernelGraphicsContext *arg1);

NvBool kgrctxShouldPreAllocPmBuffer_VF(struct OBJGPU *pGpu, struct KernelGraphicsContext *pKernelGraphicsContext, struct KernelChannel *pKernelChannel);

NvBool kgrctxShouldPreAllocPmBuffer_PF(struct OBJGPU *pGpu, struct KernelGraphicsContext *pKernelGraphicsContext, struct KernelChannel *pKernelChannel);

static inline GR_GLOBALCTX_BUFFER kgrctxGetRegisterAccessMapId_aa21e9(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelChannel *arg3) {
    return GR_GLOBALCTX_BUFFER_PRIV_ACCESS_MAP;
}

GR_GLOBALCTX_BUFFER kgrctxGetRegisterAccessMapId_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelChannel *arg3);

NV_STATUS kgrctxCtrlGetTpcPartitionMode_IMPL(struct KernelGraphicsContext *pKernelGraphicsContext, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams);

NV_STATUS kgrctxCtrlSetTpcPartitionMode_IMPL(struct KernelGraphicsContext *pKernelGraphicsContext, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams);

NV_STATUS kgrctxCtrlGetMMUDebugMode_IMPL(struct KernelGraphicsContext *pKernelGraphicsContext, NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS *pParams);

NV_STATUS kgrctxCtrlProgramVidmemPromote_IMPL(struct KernelGraphicsContext *pKernelGraphicsContext, NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *pParams);

NV_STATUS kgrctxCtrlSetLgSectorPromotion_IMPL(struct KernelGraphicsContext *pKernelGraphicsContext, NV0090_CTRL_SET_LG_SECTOR_PROMOTION_PARAMS *pParams);

static inline struct KernelGraphicsContextShared *kgrctxGetShared(struct OBJGPU *pGpu, struct KernelGraphicsContext *pKernelGraphicsContext) {
    struct KernelGraphicsContext_PRIVATE *pKernelGraphicsContext_PRIVATE = (struct KernelGraphicsContext_PRIVATE *)pKernelGraphicsContext;
    return pKernelGraphicsContext_PRIVATE->pShared;
}

static inline VGPU_GFXP_BUFFERS *kgrctxGetVgpuGfxpBuffers(struct OBJGPU *pGpu, struct KernelGraphicsContext *pKernelGraphicsContext) {
    struct KernelGraphicsContext_PRIVATE *pKernelGraphicsContext_PRIVATE = (struct KernelGraphicsContext_PRIVATE *)pKernelGraphicsContext;
    return pKernelGraphicsContext_PRIVATE->pVgpuGfxpBuffers;
}

static inline void kgrctxSetVgpuGfxpBuffers(struct KernelGraphicsContext *pKernelGraphicsContext, VGPU_GFXP_BUFFERS *pBuffers) {
    struct KernelGraphicsContext_PRIVATE *pKernelGraphicsContext_PRIVATE = (struct KernelGraphicsContext_PRIVATE *)pKernelGraphicsContext;
    pKernelGraphicsContext_PRIVATE->pVgpuGfxpBuffers = pBuffers;
}

NV_STATUS kgrctxFromKernelChannel_IMPL(struct KernelChannel *arg1, struct KernelGraphicsContext **arg2);

#define kgrctxFromKernelChannel(arg1, arg2) kgrctxFromKernelChannel_IMPL(arg1, arg2)
NV_STATUS kgrctxFromKernelChannelGroupApi_IMPL(struct KernelChannelGroupApi *arg1, struct KernelGraphicsContext **arg2);

#define kgrctxFromKernelChannelGroupApi(arg1, arg2) kgrctxFromKernelChannelGroupApi_IMPL(arg1, arg2)
NV_STATUS kgrctxGetGlobalContextBufferExternalId_IMPL(GR_GLOBALCTX_BUFFER arg1, NvU32 *pExternalId);

#define kgrctxGetGlobalContextBufferExternalId(arg1, pExternalId) kgrctxGetGlobalContextBufferExternalId_IMPL(arg1, pExternalId)
NV_STATUS kgrctxGetGlobalContextBufferInternalId_IMPL(NvU32 externalId, GR_GLOBALCTX_BUFFER *arg2);

#define kgrctxGetGlobalContextBufferInternalId(externalId, arg2) kgrctxGetGlobalContextBufferInternalId_IMPL(externalId, arg2)
NV_STATUS kgrctxCtxBufferToFifoEngineId_IMPL(GR_CTX_BUFFER arg1, NvU32 *pFifoEngineId);

#define kgrctxCtxBufferToFifoEngineId(arg1, pFifoEngineId) kgrctxCtxBufferToFifoEngineId_IMPL(arg1, pFifoEngineId)
NV_STATUS kgrctxGlobalCtxBufferToFifoEngineId_IMPL(GR_GLOBALCTX_BUFFER arg1, NvU32 *pFifoEngineId);

#define kgrctxGlobalCtxBufferToFifoEngineId(arg1, pFifoEngineId) kgrctxGlobalCtxBufferToFifoEngineId_IMPL(arg1, pFifoEngineId)
NV_STATUS kgrctxGetGidInfoInPlace_IMPL(struct OBJGPU *pGpu, NvU8 *pUuidBuffer, NvU32 uuidBufferSize, NvU32 flags);

#define kgrctxGetGidInfoInPlace(pGpu, pUuidBuffer, uuidBufferSize, flags) kgrctxGetGidInfoInPlace_IMPL(pGpu, pUuidBuffer, uuidBufferSize, flags)
GMMU_APERTURE kgrctxGetExternalAllocAperture_IMPL(NvU32 addressSpace);

#define kgrctxGetExternalAllocAperture(addressSpace) kgrctxGetExternalAllocAperture_IMPL(addressSpace)
NV_STATUS kgrctxFillCtxBufferInfo_IMPL(struct MEMORY_DESCRIPTOR *arg1, NvU32 externalId, NvBool bBufferGlobal, NV2080_CTRL_GR_CTX_BUFFER_INFO *arg4);

#define kgrctxFillCtxBufferInfo(arg1, externalId, bBufferGlobal, arg4) kgrctxFillCtxBufferInfo_IMPL(arg1, externalId, bBufferGlobal, arg4)
NV_STATUS kgrctxConstruct_IMPL(struct KernelGraphicsContext *arg_pKernelGraphicsContext, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_kgrctxConstruct(arg_pKernelGraphicsContext, arg_pCallContext, arg_pParams) kgrctxConstruct_IMPL(arg_pKernelGraphicsContext, arg_pCallContext, arg_pParams)
NV_STATUS kgrctxCopyConstruct_IMPL(struct KernelGraphicsContext *arg1, struct CALL_CONTEXT *arg2, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg3);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxCopyConstruct(struct KernelGraphicsContext *arg1, struct CALL_CONTEXT *arg2, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxCopyConstruct(arg1, arg2, arg3) kgrctxCopyConstruct_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxDestruct_IMPL(struct KernelGraphicsContext *arg1);

#define __nvoc_kgrctxDestruct(arg1) kgrctxDestruct_IMPL(arg1)
NV_STATUS kgrctxGetUnicast_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, KernelGraphicsContextUnicast **arg3);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxGetUnicast(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, KernelGraphicsContextUnicast **arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxGetUnicast(arg1, arg2, arg3) kgrctxGetUnicast_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxLookupMmuFaultInfo_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, NV83DE_CTRL_DEBUG_READ_MMU_FAULT_INFO_PARAMS *arg3);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxLookupMmuFaultInfo(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, NV83DE_CTRL_DEBUG_READ_MMU_FAULT_INFO_PARAMS *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxLookupMmuFaultInfo(arg1, arg2, arg3) kgrctxLookupMmuFaultInfo_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxLookupMmuFault_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, NV83DE_MMU_FAULT_INFO *arg3);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxLookupMmuFault(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, NV83DE_MMU_FAULT_INFO *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxLookupMmuFault(arg1, arg2, arg3) kgrctxLookupMmuFault_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxClearMmuFault_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxClearMmuFault(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxClearMmuFault(arg1, arg2) kgrctxClearMmuFault_IMPL(arg1, arg2)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxRecordMmuFault_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, NvU32 mmuFaultInfo, NvU64 mmuFaultAddress, NvU32 mmuFaultType, NvU32 mmuFaultAccessType);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxRecordMmuFault(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, NvU32 mmuFaultInfo, NvU64 mmuFaultAddress, NvU32 mmuFaultType, NvU32 mmuFaultAccessType) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxRecordMmuFault(arg1, arg2, mmuFaultInfo, mmuFaultAddress, mmuFaultType, mmuFaultAccessType) kgrctxRecordMmuFault_IMPL(arg1, arg2, mmuFaultInfo, mmuFaultAddress, mmuFaultType, mmuFaultAccessType)
#endif //__nvoc_kernel_graphics_context_h_disabled

KernelSMDebuggerSessionListIter kgrctxGetDebuggerSessionIter_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline KernelSMDebuggerSessionListIter kgrctxGetDebuggerSessionIter(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    KernelSMDebuggerSessionListIter ret;
    portMemSet(&ret, 0, sizeof(KernelSMDebuggerSessionListIter));
    return ret;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxGetDebuggerSessionIter(arg1, arg2) kgrctxGetDebuggerSessionIter_IMPL(arg1, arg2)
#endif //__nvoc_kernel_graphics_context_h_disabled

NvBool kgrctxRegisterKernelSMDebuggerSession_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelSMDebuggerSession *arg3);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NvBool kgrctxRegisterKernelSMDebuggerSession(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelSMDebuggerSession *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxRegisterKernelSMDebuggerSession(arg1, arg2, arg3) kgrctxRegisterKernelSMDebuggerSession_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxDeregisterKernelSMDebuggerSession_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelSMDebuggerSession *arg3);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxDeregisterKernelSMDebuggerSession(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelSMDebuggerSession *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxDeregisterKernelSMDebuggerSession(arg1, arg2, arg3) kgrctxDeregisterKernelSMDebuggerSession_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_context_h_disabled

NvBool kgrctxIsValid_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelChannel *arg3);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NvBool kgrctxIsValid(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelChannel *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxIsValid(arg1, arg2, arg3) kgrctxIsValid_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_context_h_disabled

NvBool kgrctxIsMainContextAllocated_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NvBool kgrctxIsMainContextAllocated(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxIsMainContextAllocated(arg1, arg2) kgrctxIsMainContextAllocated_IMPL(arg1, arg2)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxGetMainContextBuffer_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct MEMORY_DESCRIPTOR **arg3);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxGetMainContextBuffer(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct MEMORY_DESCRIPTOR **arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxGetMainContextBuffer(arg1, arg2, arg3) kgrctxGetMainContextBuffer_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxGetBufferCount_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, NvU32 *pBufferCount);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxGetBufferCount(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, NvU32 *pBufferCount) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxGetBufferCount(arg1, arg2, arg3, pBufferCount) kgrctxGetBufferCount_IMPL(arg1, arg2, arg3, pBufferCount)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxGetCtxBuffers_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, NvU32 gfid, NvU32 bufferCount, struct MEMORY_DESCRIPTOR **arg6, NvU32 *pCtxBufferType, NvU32 *pBufferCountOut, NvU32 *pFirstGlobalBuffer);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxGetCtxBuffers(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, NvU32 gfid, NvU32 bufferCount, struct MEMORY_DESCRIPTOR **arg6, NvU32 *pCtxBufferType, NvU32 *pBufferCountOut, NvU32 *pFirstGlobalBuffer) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxGetCtxBuffers(arg1, arg2, arg3, gfid, bufferCount, arg6, pCtxBufferType, pBufferCountOut, pFirstGlobalBuffer) kgrctxGetCtxBuffers_IMPL(arg1, arg2, arg3, gfid, bufferCount, arg6, pCtxBufferType, pBufferCountOut, pFirstGlobalBuffer)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxGetCtxBufferInfo_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, NvU32 gfid, NvU32 bufferMaxCount, NvU32 *pBufferCount, NV2080_CTRL_GR_CTX_BUFFER_INFO *arg7);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxGetCtxBufferInfo(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, NvU32 gfid, NvU32 bufferMaxCount, NvU32 *pBufferCount, NV2080_CTRL_GR_CTX_BUFFER_INFO *arg7) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxGetCtxBufferInfo(arg1, arg2, arg3, gfid, bufferMaxCount, pBufferCount, arg7) kgrctxGetCtxBufferInfo_IMPL(arg1, arg2, arg3, gfid, bufferMaxCount, pBufferCount, arg7)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxGetCtxBufferPtes_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, NvU32 gfid, NvU32 bufferType, NvU32 firstPage, NvU64 *pPhysAddrs, NvU32 addrsSize, NvU32 *pNumPages, NvBool *pbNoMorePages);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxGetCtxBufferPtes(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, NvU32 gfid, NvU32 bufferType, NvU32 firstPage, NvU64 *pPhysAddrs, NvU32 addrsSize, NvU32 *pNumPages, NvBool *pbNoMorePages) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxGetCtxBufferPtes(arg1, arg2, arg3, gfid, bufferType, firstPage, pPhysAddrs, addrsSize, pNumPages, pbNoMorePages) kgrctxGetCtxBufferPtes_IMPL(arg1, arg2, arg3, gfid, bufferType, firstPage, pPhysAddrs, addrsSize, pNumPages, pbNoMorePages)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxAllocMainCtxBuffer_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct KernelChannel *arg4);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxAllocMainCtxBuffer(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct KernelChannel *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxAllocMainCtxBuffer(arg1, arg2, arg3, arg4) kgrctxAllocMainCtxBuffer_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxAllocPatchBuffer_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct KernelChannel *arg4);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxAllocPatchBuffer(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct KernelChannel *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxAllocPatchBuffer(arg1, arg2, arg3, arg4) kgrctxAllocPatchBuffer_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxAllocPmBuffer_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct KernelChannel *arg4);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxAllocPmBuffer(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct KernelChannel *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxAllocPmBuffer(arg1, arg2, arg3, arg4) kgrctxAllocPmBuffer_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxAllocCtxBuffers_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct KernelGraphicsObject *arg4);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxAllocCtxBuffers(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct KernelGraphicsObject *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxAllocCtxBuffers(arg1, arg2, arg3, arg4) kgrctxAllocCtxBuffers_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxMapGlobalCtxBuffer_IMPL(struct OBJGPU *pGpu, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, NvU32 gfid, struct OBJVASPACE *arg5, GR_GLOBALCTX_BUFFER arg6, NvBool bIsReadOnly);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxMapGlobalCtxBuffer(struct OBJGPU *pGpu, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, NvU32 gfid, struct OBJVASPACE *arg5, GR_GLOBALCTX_BUFFER arg6, NvBool bIsReadOnly) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxMapGlobalCtxBuffer(pGpu, arg2, arg3, gfid, arg5, arg6, bIsReadOnly) kgrctxMapGlobalCtxBuffer_IMPL(pGpu, arg2, arg3, gfid, arg5, arg6, bIsReadOnly)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxMapGlobalCtxBuffers_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, NvU32 gfid, struct KernelChannel *arg5);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxMapGlobalCtxBuffers(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, NvU32 gfid, struct KernelChannel *arg5) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxMapGlobalCtxBuffers(arg1, arg2, arg3, gfid, arg5) kgrctxMapGlobalCtxBuffers_IMPL(arg1, arg2, arg3, gfid, arg5)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxMapCtxBuffers_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct KernelGraphicsObject *arg4);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxMapCtxBuffers(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct KernelGraphicsObject *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxMapCtxBuffers(arg1, arg2, arg3, arg4) kgrctxMapCtxBuffers_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxPrepareInitializeCtxBuffer_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct KernelChannel *arg4, NvU32 externalId, NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ENTRY *arg6, NvBool *pbAddEntry);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxPrepareInitializeCtxBuffer(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct KernelChannel *arg4, NvU32 externalId, NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ENTRY *arg6, NvBool *pbAddEntry) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxPrepareInitializeCtxBuffer(arg1, arg2, arg3, arg4, externalId, arg6, pbAddEntry) kgrctxPrepareInitializeCtxBuffer_IMPL(arg1, arg2, arg3, arg4, externalId, arg6, pbAddEntry)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxPreparePromoteCtxBuffer_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelChannel *arg3, NvU32 externalId, NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ENTRY *arg5, NvBool *pbAddEntry);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxPreparePromoteCtxBuffer(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelChannel *arg3, NvU32 externalId, NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ENTRY *arg5, NvBool *pbAddEntry) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxPreparePromoteCtxBuffer(arg1, arg2, arg3, externalId, arg5, pbAddEntry) kgrctxPreparePromoteCtxBuffer_IMPL(arg1, arg2, arg3, externalId, arg5, pbAddEntry)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxMarkCtxBufferInitialized_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct KernelChannel *arg4, NvU32 externalId);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxMarkCtxBufferInitialized(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct KernelChannel *arg4, NvU32 externalId) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxMarkCtxBufferInitialized(arg1, arg2, arg3, arg4, externalId) kgrctxMarkCtxBufferInitialized_IMPL(arg1, arg2, arg3, arg4, externalId)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxSetupDeferredPmBuffer_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct KernelChannel *arg4);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxSetupDeferredPmBuffer(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct KernelChannel *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxSetupDeferredPmBuffer(arg1, arg2, arg3, arg4) kgrctxSetupDeferredPmBuffer_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxUnmapGlobalCtxBuffers_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct OBJVASPACE *arg4, NvU32 gfid);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxUnmapGlobalCtxBuffers(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct OBJVASPACE *arg4, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxUnmapGlobalCtxBuffers(arg1, arg2, arg3, arg4, gfid) kgrctxUnmapGlobalCtxBuffers_IMPL(arg1, arg2, arg3, arg4, gfid)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxUnmapGlobalCtxBuffer_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct OBJVASPACE *arg4, GR_GLOBALCTX_BUFFER arg5);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxUnmapGlobalCtxBuffer(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct OBJVASPACE *arg4, GR_GLOBALCTX_BUFFER arg5) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxUnmapGlobalCtxBuffer(arg1, arg2, arg3, arg4, arg5) kgrctxUnmapGlobalCtxBuffer_IMPL(arg1, arg2, arg3, arg4, arg5)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxUnmapMainCtxBuffer_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct KernelChannel *arg4);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxUnmapMainCtxBuffer(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct KernelChannel *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxUnmapMainCtxBuffer(arg1, arg2, arg3, arg4) kgrctxUnmapMainCtxBuffer_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxUnmapCtxPmBuffer_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct OBJVASPACE *arg4);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxUnmapCtxPmBuffer(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct OBJVASPACE *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxUnmapCtxPmBuffer(arg1, arg2, arg3, arg4) kgrctxUnmapCtxPmBuffer_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxUnmapCtxZcullBuffer_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct OBJVASPACE *arg4);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxUnmapCtxZcullBuffer(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct OBJVASPACE *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxUnmapCtxZcullBuffer(arg1, arg2, arg3, arg4) kgrctxUnmapCtxZcullBuffer_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxUnmapCtxPreemptionBuffers_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct OBJVASPACE *arg4);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxUnmapCtxPreemptionBuffers(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct OBJVASPACE *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxUnmapCtxPreemptionBuffers(arg1, arg2, arg3, arg4) kgrctxUnmapCtxPreemptionBuffers_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxUnmapAssociatedCtxBuffers_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct KernelChannel *arg4);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxUnmapAssociatedCtxBuffers(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, struct KernelChannel *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxUnmapAssociatedCtxBuffers(arg1, arg2, arg3, arg4) kgrctxUnmapAssociatedCtxBuffers_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxFreeMainCtxBuffer_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxFreeMainCtxBuffer(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxFreeMainCtxBuffer(arg1, arg2) kgrctxFreeMainCtxBuffer_IMPL(arg1, arg2)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxFreeZcullBuffer_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxFreeZcullBuffer(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxFreeZcullBuffer(arg1, arg2) kgrctxFreeZcullBuffer_IMPL(arg1, arg2)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxFreeCtxPreemptionBuffers_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxFreeCtxPreemptionBuffers(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxFreeCtxPreemptionBuffers(arg1, arg2) kgrctxFreeCtxPreemptionBuffers_IMPL(arg1, arg2)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxFreePatchBuffer_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxFreePatchBuffer(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxFreePatchBuffer(arg1, arg2) kgrctxFreePatchBuffer_IMPL(arg1, arg2)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxFreePmBuffer_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxFreePmBuffer(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxFreePmBuffer(arg1, arg2) kgrctxFreePmBuffer_IMPL(arg1, arg2)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxFreeLocalGlobalCtxBuffers_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxFreeLocalGlobalCtxBuffers(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxFreeLocalGlobalCtxBuffers(arg1, arg2) kgrctxFreeLocalGlobalCtxBuffers_IMPL(arg1, arg2)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxFreeAssociatedCtxBuffers_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxFreeAssociatedCtxBuffers(struct OBJGPU *arg1, struct KernelGraphicsContext *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxFreeAssociatedCtxBuffers(arg1, arg2) kgrctxFreeAssociatedCtxBuffers_IMPL(arg1, arg2)
#endif //__nvoc_kernel_graphics_context_h_disabled

#undef PRIVATE_FIELD


/**
 * This refcounted class encapsulates the context data that is shared when a
 * context is duped.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_GRAPHICS_CONTEXT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelGraphicsContextShared;
struct NVOC_METADATA__RsShared;

struct KernelGraphicsContextUnicast {
    NvU32 channelObjects;
    NvU32 objectCounts[4];
    NV_CONTEXT_BUFFER ctxPatchBuffer;
    NV_CONTEXT_BUFFER zcullCtxswBuffer;
    NV_CONTEXT_BUFFER pmCtxswBuffer;
    struct MEMORY_DESCRIPTOR *pMainCtxBuffer;
    GR_GLOBALCTX_BUFFERS localCtxBuffer;
    VA_LIST globalCtxBufferVaList[10];
    NvBool bKGrMainCtxBufferInitialized;
    NvBool bKGrPatchCtxBufferInitialized;
    NvBool bKGrPmCtxBufferInitialized;
    NV_CONTEXT_BUFFER preemptCtxswBuffer;
    NV_CONTEXT_BUFFER spillCtxswBuffer;
    NV_CONTEXT_BUFFER betaCBCtxswBuffer;
    NV_CONTEXT_BUFFER pagepoolCtxswBuffer;
    NV_CONTEXT_BUFFER rtvCbCtxswBuffer;
    NV_CONTEXT_BUFFER setupCtxswBuffer;
    NvBool bVprChannel;
    NvBool bSupportsPerSubctxHeader;
    struct MMU_FAULT_INFO {
        NV83DE_MMU_FAULT_INFO mmuFaultInfo;
        NV83DE_CTRL_DEBUG_READ_MMU_FAULT_INFO_ENTRY mmuFaultInfoList[4];
        NvU32 head;
        NvU32 tail;
    } mmuFault;
};



struct KernelGraphicsContextShared {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelGraphicsContextShared *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RsShared __nvoc_base_RsShared;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct RsShared *__nvoc_pbase_RsShared;    // shr super
    struct KernelGraphicsContextShared *__nvoc_pbase_KernelGraphicsContextShared;    // shrkgrctx

    // Data members
    struct KernelGraphicsContextUnicast PRIVATE_FIELD(kernelGraphicsContextUnicast);
    KernelSMDebuggerSessionList PRIVATE_FIELD(activeDebuggers);
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelGraphicsContextShared {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RsShared metadata__RsShared;
};

#ifndef __NVOC_CLASS_KernelGraphicsContextShared_TYPEDEF__
#define __NVOC_CLASS_KernelGraphicsContextShared_TYPEDEF__
typedef struct KernelGraphicsContextShared KernelGraphicsContextShared;
#endif /* __NVOC_CLASS_KernelGraphicsContextShared_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGraphicsContextShared
#define __nvoc_class_id_KernelGraphicsContextShared 0xe7abeb
#endif /* __nvoc_class_id_KernelGraphicsContextShared */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGraphicsContextShared;

#define __staticCast_KernelGraphicsContextShared(pThis) \
    ((pThis)->__nvoc_pbase_KernelGraphicsContextShared)

#ifdef __nvoc_kernel_graphics_context_h_disabled
#define __dynamicCast_KernelGraphicsContextShared(pThis) ((KernelGraphicsContextShared*) NULL)
#else //__nvoc_kernel_graphics_context_h_disabled
#define __dynamicCast_KernelGraphicsContextShared(pThis) \
    ((KernelGraphicsContextShared*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelGraphicsContextShared)))
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS __nvoc_objCreateDynamic_KernelGraphicsContextShared(KernelGraphicsContextShared**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelGraphicsContextShared(KernelGraphicsContextShared**, Dynamic*, NvU32);
#define __objCreate_KernelGraphicsContextShared(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelGraphicsContextShared((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
NV_STATUS shrkgrctxConstruct_IMPL(struct KernelGraphicsContextShared *arg_);

#define __nvoc_shrkgrctxConstruct(arg_) shrkgrctxConstruct_IMPL(arg_)
NV_STATUS shrkgrctxInit_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContextShared *arg2, struct KernelGraphicsContext *arg3);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS shrkgrctxInit(struct OBJGPU *arg1, struct KernelGraphicsContextShared *arg2, struct KernelGraphicsContext *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContextShared was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define shrkgrctxInit(arg1, arg2, arg3) shrkgrctxInit_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS shrkgrctxConstructUnicast_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContextShared *arg2, struct KernelGraphicsContext *arg3, struct KernelGraphics *arg4, KernelGraphicsContextUnicast *arg5);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS shrkgrctxConstructUnicast(struct OBJGPU *arg1, struct KernelGraphicsContextShared *arg2, struct KernelGraphicsContext *arg3, struct KernelGraphics *arg4, KernelGraphicsContextUnicast *arg5) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContextShared was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define shrkgrctxConstructUnicast(arg1, arg2, arg3, arg4, arg5) shrkgrctxConstructUnicast_IMPL(arg1, arg2, arg3, arg4, arg5)
#endif //__nvoc_kernel_graphics_context_h_disabled

void shrkgrctxDestruct_IMPL(struct KernelGraphicsContextShared *arg1);

#define __nvoc_shrkgrctxDestruct(arg1) shrkgrctxDestruct_IMPL(arg1)
void shrkgrctxTeardown_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContextShared *arg2, struct KernelGraphicsContext *arg3);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void shrkgrctxTeardown(struct OBJGPU *arg1, struct KernelGraphicsContextShared *arg2, struct KernelGraphicsContext *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContextShared was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define shrkgrctxTeardown(arg1, arg2, arg3) shrkgrctxTeardown_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_context_h_disabled

void shrkgrctxDestructUnicast_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContextShared *arg2, struct KernelGraphicsContext *arg3, KernelGraphicsContextUnicast *arg4);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void shrkgrctxDestructUnicast(struct OBJGPU *arg1, struct KernelGraphicsContextShared *arg2, struct KernelGraphicsContext *arg3, KernelGraphicsContextUnicast *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContextShared was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define shrkgrctxDestructUnicast(arg1, arg2, arg3, arg4) shrkgrctxDestructUnicast_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_graphics_context_h_disabled

void shrkgrctxDetach_IMPL(struct OBJGPU *arg1, struct KernelGraphicsContextShared *arg2, struct KernelGraphicsContext *arg3, struct KernelChannel *arg4);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void shrkgrctxDetach(struct OBJGPU *arg1, struct KernelGraphicsContextShared *arg2, struct KernelGraphicsContext *arg3, struct KernelChannel *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContextShared was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define shrkgrctxDetach(arg1, arg2, arg3, arg4) shrkgrctxDetach_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_graphics_context_h_disabled

#undef PRIVATE_FIELD


/*! Handle NV0090 ctrl call forwarding */
NV_STATUS kgrctxCtrlHandle
(
    CALL_CONTEXT *,
    NvHandle hKernelGraphicsContext
);

#endif // _KERNEL_GRAPHICS_CONTEXT_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_GRAPHICS_CONTEXT_NVOC_H_
