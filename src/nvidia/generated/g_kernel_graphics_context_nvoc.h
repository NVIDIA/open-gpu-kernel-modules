#ifndef _G_KERNEL_GRAPHICS_CONTEXT_NVOC_H_
#define _G_KERNEL_GRAPHICS_CONTEXT_NVOC_H_
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
#ifdef NVOC_KERNEL_GRAPHICS_CONTEXT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelGraphicsContext {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct KernelGraphicsContext *__nvoc_pbase_KernelGraphicsContext;
    NvBool (*__kgrctxCanCopy__)(struct KernelGraphicsContext *);
    NvHandle (*__kgrctxGetInternalObjectHandle__)(struct KernelGraphicsContext *);
    NV_STATUS (*__kgrctxCtrlGetTpcPartitionMode__)(struct KernelGraphicsContext *, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *);
    NV_STATUS (*__kgrctxCtrlSetTpcPartitionMode__)(struct KernelGraphicsContext *, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *);
    NV_STATUS (*__kgrctxCtrlGetMMUDebugMode__)(struct KernelGraphicsContext *, NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS *);
    NV_STATUS (*__kgrctxCtrlProgramVidmemPromote__)(struct KernelGraphicsContext *, NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *);
    NvBool (*__kgrctxShareCallback__)(struct KernelGraphicsContext *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__kgrctxCheckMemInterUnmap__)(struct KernelGraphicsContext *, NvBool);
    NV_STATUS (*__kgrctxMapTo__)(struct KernelGraphicsContext *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__kgrctxGetMapAddrSpace__)(struct KernelGraphicsContext *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvU32 (*__kgrctxGetRefCount__)(struct KernelGraphicsContext *);
    void (*__kgrctxAddAdditionalDependants__)(struct RsClient *, struct KernelGraphicsContext *, RsResourceRef *);
    NV_STATUS (*__kgrctxControl_Prologue__)(struct KernelGraphicsContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kgrctxGetRegBaseOffsetAndSize__)(struct KernelGraphicsContext *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__kgrctxInternalControlForward__)(struct KernelGraphicsContext *, NvU32, void *, NvU32);
    NV_STATUS (*__kgrctxUnmapFrom__)(struct KernelGraphicsContext *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__kgrctxControl_Epilogue__)(struct KernelGraphicsContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kgrctxControlLookup__)(struct KernelGraphicsContext *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__kgrctxControl__)(struct KernelGraphicsContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kgrctxUnmap__)(struct KernelGraphicsContext *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__kgrctxGetMemInterMapParams__)(struct KernelGraphicsContext *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__kgrctxGetMemoryMappingDescriptor__)(struct KernelGraphicsContext *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__kgrctxControlFilter__)(struct KernelGraphicsContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kgrctxControlSerialization_Prologue__)(struct KernelGraphicsContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__kgrctxPreDestruct__)(struct KernelGraphicsContext *);
    NV_STATUS (*__kgrctxIsDuplicate__)(struct KernelGraphicsContext *, NvHandle, NvBool *);
    void (*__kgrctxControlSerialization_Epilogue__)(struct KernelGraphicsContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kgrctxMap__)(struct KernelGraphicsContext *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__kgrctxAccessCallback__)(struct KernelGraphicsContext *, struct RsClient *, void *, RsAccessRight);
    struct KernelGraphicsContextShared *PRIVATE_FIELD(pShared);
};

#ifndef __NVOC_CLASS_KernelGraphicsContext_TYPEDEF__
#define __NVOC_CLASS_KernelGraphicsContext_TYPEDEF__
typedef struct KernelGraphicsContext KernelGraphicsContext;
#endif /* __NVOC_CLASS_KernelGraphicsContext_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGraphicsContext
#define __nvoc_class_id_KernelGraphicsContext 0x7ead09
#endif /* __nvoc_class_id_KernelGraphicsContext */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGraphicsContext;

#define __staticCast_KernelGraphicsContext(pThis) \
    ((pThis)->__nvoc_pbase_KernelGraphicsContext)

#ifdef __nvoc_kernel_graphics_context_h_disabled
#define __dynamicCast_KernelGraphicsContext(pThis) ((KernelGraphicsContext*)NULL)
#else //__nvoc_kernel_graphics_context_h_disabled
#define __dynamicCast_KernelGraphicsContext(pThis) \
    ((KernelGraphicsContext*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelGraphicsContext)))
#endif //__nvoc_kernel_graphics_context_h_disabled


NV_STATUS __nvoc_objCreateDynamic_KernelGraphicsContext(KernelGraphicsContext**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelGraphicsContext(KernelGraphicsContext**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_KernelGraphicsContext(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_KernelGraphicsContext((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define kgrctxCanCopy(arg0) kgrctxCanCopy_DISPATCH(arg0)
#define kgrctxGetInternalObjectHandle(arg0) kgrctxGetInternalObjectHandle_DISPATCH(arg0)
#define kgrctxCtrlGetTpcPartitionMode(pKernelGraphicsContext, pParams) kgrctxCtrlGetTpcPartitionMode_DISPATCH(pKernelGraphicsContext, pParams)
#define kgrctxCtrlSetTpcPartitionMode(pKernelGraphicsContext, pParams) kgrctxCtrlSetTpcPartitionMode_DISPATCH(pKernelGraphicsContext, pParams)
#define kgrctxCtrlGetMMUDebugMode(pKernelGraphicsContext, pParams) kgrctxCtrlGetMMUDebugMode_DISPATCH(pKernelGraphicsContext, pParams)
#define kgrctxCtrlProgramVidmemPromote(pKernelGraphicsContext, pParams) kgrctxCtrlProgramVidmemPromote_DISPATCH(pKernelGraphicsContext, pParams)
#define kgrctxShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) kgrctxShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define kgrctxCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) kgrctxCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define kgrctxMapTo(pResource, pParams) kgrctxMapTo_DISPATCH(pResource, pParams)
#define kgrctxGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) kgrctxGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define kgrctxGetRefCount(pResource) kgrctxGetRefCount_DISPATCH(pResource)
#define kgrctxAddAdditionalDependants(pClient, pResource, pReference) kgrctxAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define kgrctxControl_Prologue(pResource, pCallContext, pParams) kgrctxControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kgrctxGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) kgrctxGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define kgrctxInternalControlForward(pGpuResource, command, pParams, size) kgrctxInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define kgrctxUnmapFrom(pResource, pParams) kgrctxUnmapFrom_DISPATCH(pResource, pParams)
#define kgrctxControl_Epilogue(pResource, pCallContext, pParams) kgrctxControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kgrctxControlLookup(pResource, pParams, ppEntry) kgrctxControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define kgrctxControl(pGpuResource, pCallContext, pParams) kgrctxControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define kgrctxUnmap(pGpuResource, pCallContext, pCpuMapping) kgrctxUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define kgrctxGetMemInterMapParams(pRmResource, pParams) kgrctxGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define kgrctxGetMemoryMappingDescriptor(pRmResource, ppMemDesc) kgrctxGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define kgrctxControlFilter(pResource, pCallContext, pParams) kgrctxControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define kgrctxControlSerialization_Prologue(pResource, pCallContext, pParams) kgrctxControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kgrctxPreDestruct(pResource) kgrctxPreDestruct_DISPATCH(pResource)
#define kgrctxIsDuplicate(pResource, hMemory, pDuplicate) kgrctxIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define kgrctxControlSerialization_Epilogue(pResource, pCallContext, pParams) kgrctxControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kgrctxMap(pGpuResource, pCallContext, pParams, pCpuMapping) kgrctxMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define kgrctxAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) kgrctxAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NvBool kgrctxShouldManageCtxBuffers_KERNEL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, NvU32 gfid);


#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NvBool kgrctxShouldManageCtxBuffers(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxShouldManageCtxBuffers(arg0, arg1, gfid) kgrctxShouldManageCtxBuffers_KERNEL(arg0, arg1, gfid)
#endif //__nvoc_kernel_graphics_context_h_disabled

#define kgrctxShouldManageCtxBuffers_HAL(arg0, arg1, gfid) kgrctxShouldManageCtxBuffers(arg0, arg1, gfid)

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

NvBool kgrctxShouldPreAllocPmBuffer_PF(struct OBJGPU *pGpu, struct KernelGraphicsContext *pKernelGraphicsContext, struct KernelChannel *pKernelChannel);


#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NvBool kgrctxShouldPreAllocPmBuffer(struct OBJGPU *pGpu, struct KernelGraphicsContext *pKernelGraphicsContext, struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxShouldPreAllocPmBuffer(pGpu, pKernelGraphicsContext, pKernelChannel) kgrctxShouldPreAllocPmBuffer_PF(pGpu, pKernelGraphicsContext, pKernelChannel)
#endif //__nvoc_kernel_graphics_context_h_disabled

#define kgrctxShouldPreAllocPmBuffer_HAL(pGpu, pKernelGraphicsContext, pKernelChannel) kgrctxShouldPreAllocPmBuffer(pGpu, pKernelGraphicsContext, pKernelChannel)

void kgrctxUnmapBuffers_KERNEL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, KernelGraphicsContextUnicast *arg2, struct KernelChannel *arg3);


#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxUnmapBuffers(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, KernelGraphicsContextUnicast *arg2, struct KernelChannel *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxUnmapBuffers(arg0, arg1, arg2, arg3) kgrctxUnmapBuffers_KERNEL(arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_context_h_disabled

#define kgrctxUnmapBuffers_HAL(arg0, arg1, arg2, arg3) kgrctxUnmapBuffers(arg0, arg1, arg2, arg3)

NV_STATUS kgrctxUnmapCtxBuffers_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphicsObject *arg2, struct KernelGraphics *arg3, NvBool bDestructor);


#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxUnmapCtxBuffers(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphicsObject *arg2, struct KernelGraphics *arg3, NvBool bDestructor) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxUnmapCtxBuffers(arg0, arg1, arg2, arg3, bDestructor) kgrctxUnmapCtxBuffers_IMPL(arg0, arg1, arg2, arg3, bDestructor)
#endif //__nvoc_kernel_graphics_context_h_disabled

#define kgrctxUnmapCtxBuffers_HAL(arg0, arg1, arg2, arg3, bDestructor) kgrctxUnmapCtxBuffers(arg0, arg1, arg2, arg3, bDestructor)

void kgrctxIncObjectCount_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, NvU32 classNum);


#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxIncObjectCount(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, NvU32 classNum) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxIncObjectCount(arg0, arg1, classNum) kgrctxIncObjectCount_IMPL(arg0, arg1, classNum)
#endif //__nvoc_kernel_graphics_context_h_disabled

#define kgrctxIncObjectCount_HAL(arg0, arg1, classNum) kgrctxIncObjectCount(arg0, arg1, classNum)

void kgrctxDecObjectCount_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, NvU32 classNum);


#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxDecObjectCount(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, NvU32 classNum) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxDecObjectCount(arg0, arg1, classNum) kgrctxDecObjectCount_IMPL(arg0, arg1, classNum)
#endif //__nvoc_kernel_graphics_context_h_disabled

#define kgrctxDecObjectCount_HAL(arg0, arg1, classNum) kgrctxDecObjectCount(arg0, arg1, classNum)

GR_GLOBALCTX_BUFFER kgrctxGetRegisterAccessMapId_PF(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelChannel *arg2);


#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline GR_GLOBALCTX_BUFFER kgrctxGetRegisterAccessMapId(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelChannel *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    GR_GLOBALCTX_BUFFER ret;
    portMemSet(&ret, 0, sizeof(GR_GLOBALCTX_BUFFER));
    return ret;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxGetRegisterAccessMapId(arg0, arg1, arg2) kgrctxGetRegisterAccessMapId_PF(arg0, arg1, arg2)
#endif //__nvoc_kernel_graphics_context_h_disabled

#define kgrctxGetRegisterAccessMapId_HAL(arg0, arg1, arg2) kgrctxGetRegisterAccessMapId(arg0, arg1, arg2)

static inline NvBool kgrctxCanCopy_0c883b(struct KernelGraphicsContext *arg0) {
    return ((NvBool)(0 == 0));
}

static inline NvBool kgrctxCanCopy_DISPATCH(struct KernelGraphicsContext *arg0) {
    return arg0->__kgrctxCanCopy__(arg0);
}

NvHandle kgrctxGetInternalObjectHandle_IMPL(struct KernelGraphicsContext *arg0);

static inline NvHandle kgrctxGetInternalObjectHandle_DISPATCH(struct KernelGraphicsContext *arg0) {
    return arg0->__kgrctxGetInternalObjectHandle__(arg0);
}

NV_STATUS kgrctxCtrlGetTpcPartitionMode_IMPL(struct KernelGraphicsContext *pKernelGraphicsContext, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams);

static inline NV_STATUS kgrctxCtrlGetTpcPartitionMode_DISPATCH(struct KernelGraphicsContext *pKernelGraphicsContext, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams) {
    return pKernelGraphicsContext->__kgrctxCtrlGetTpcPartitionMode__(pKernelGraphicsContext, pParams);
}

NV_STATUS kgrctxCtrlSetTpcPartitionMode_IMPL(struct KernelGraphicsContext *pKernelGraphicsContext, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams);

static inline NV_STATUS kgrctxCtrlSetTpcPartitionMode_DISPATCH(struct KernelGraphicsContext *pKernelGraphicsContext, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams) {
    return pKernelGraphicsContext->__kgrctxCtrlSetTpcPartitionMode__(pKernelGraphicsContext, pParams);
}

NV_STATUS kgrctxCtrlGetMMUDebugMode_IMPL(struct KernelGraphicsContext *pKernelGraphicsContext, NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS *pParams);

static inline NV_STATUS kgrctxCtrlGetMMUDebugMode_DISPATCH(struct KernelGraphicsContext *pKernelGraphicsContext, NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS *pParams) {
    return pKernelGraphicsContext->__kgrctxCtrlGetMMUDebugMode__(pKernelGraphicsContext, pParams);
}

NV_STATUS kgrctxCtrlProgramVidmemPromote_IMPL(struct KernelGraphicsContext *pKernelGraphicsContext, NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *pParams);

static inline NV_STATUS kgrctxCtrlProgramVidmemPromote_DISPATCH(struct KernelGraphicsContext *pKernelGraphicsContext, NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *pParams) {
    return pKernelGraphicsContext->__kgrctxCtrlProgramVidmemPromote__(pKernelGraphicsContext, pParams);
}

static inline NvBool kgrctxShareCallback_DISPATCH(struct KernelGraphicsContext *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__kgrctxShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS kgrctxCheckMemInterUnmap_DISPATCH(struct KernelGraphicsContext *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__kgrctxCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS kgrctxMapTo_DISPATCH(struct KernelGraphicsContext *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__kgrctxMapTo__(pResource, pParams);
}

static inline NV_STATUS kgrctxGetMapAddrSpace_DISPATCH(struct KernelGraphicsContext *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__kgrctxGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NvU32 kgrctxGetRefCount_DISPATCH(struct KernelGraphicsContext *pResource) {
    return pResource->__kgrctxGetRefCount__(pResource);
}

static inline void kgrctxAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct KernelGraphicsContext *pResource, RsResourceRef *pReference) {
    pResource->__kgrctxAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS kgrctxControl_Prologue_DISPATCH(struct KernelGraphicsContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kgrctxControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kgrctxGetRegBaseOffsetAndSize_DISPATCH(struct KernelGraphicsContext *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__kgrctxGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS kgrctxInternalControlForward_DISPATCH(struct KernelGraphicsContext *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__kgrctxInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS kgrctxUnmapFrom_DISPATCH(struct KernelGraphicsContext *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__kgrctxUnmapFrom__(pResource, pParams);
}

static inline void kgrctxControl_Epilogue_DISPATCH(struct KernelGraphicsContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__kgrctxControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kgrctxControlLookup_DISPATCH(struct KernelGraphicsContext *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__kgrctxControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS kgrctxControl_DISPATCH(struct KernelGraphicsContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__kgrctxControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS kgrctxUnmap_DISPATCH(struct KernelGraphicsContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__kgrctxUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS kgrctxGetMemInterMapParams_DISPATCH(struct KernelGraphicsContext *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__kgrctxGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS kgrctxGetMemoryMappingDescriptor_DISPATCH(struct KernelGraphicsContext *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__kgrctxGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS kgrctxControlFilter_DISPATCH(struct KernelGraphicsContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kgrctxControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kgrctxControlSerialization_Prologue_DISPATCH(struct KernelGraphicsContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kgrctxControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void kgrctxPreDestruct_DISPATCH(struct KernelGraphicsContext *pResource) {
    pResource->__kgrctxPreDestruct__(pResource);
}

static inline NV_STATUS kgrctxIsDuplicate_DISPATCH(struct KernelGraphicsContext *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__kgrctxIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void kgrctxControlSerialization_Epilogue_DISPATCH(struct KernelGraphicsContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__kgrctxControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kgrctxMap_DISPATCH(struct KernelGraphicsContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__kgrctxMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool kgrctxAccessCallback_DISPATCH(struct KernelGraphicsContext *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__kgrctxAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS kgrctxFromKernelChannel_IMPL(struct KernelChannel *arg0, struct KernelGraphicsContext **arg1);

#define kgrctxFromKernelChannel(arg0, arg1) kgrctxFromKernelChannel_IMPL(arg0, arg1)
NV_STATUS kgrctxFromKernelChannelGroupApi_IMPL(struct KernelChannelGroupApi *arg0, struct KernelGraphicsContext **arg1);

#define kgrctxFromKernelChannelGroupApi(arg0, arg1) kgrctxFromKernelChannelGroupApi_IMPL(arg0, arg1)
NV_STATUS kgrctxGetGlobalContextBufferExternalId_IMPL(GR_GLOBALCTX_BUFFER arg0, NvU32 *pExternalId);

#define kgrctxGetGlobalContextBufferExternalId(arg0, pExternalId) kgrctxGetGlobalContextBufferExternalId_IMPL(arg0, pExternalId)
NV_STATUS kgrctxGetGlobalContextBufferInternalId_IMPL(NvU32 externalId, GR_GLOBALCTX_BUFFER *arg0);

#define kgrctxGetGlobalContextBufferInternalId(externalId, arg0) kgrctxGetGlobalContextBufferInternalId_IMPL(externalId, arg0)
NV_STATUS kgrctxCtxBufferToFifoEngineId_IMPL(GR_CTX_BUFFER arg0, NvU32 *pFifoEngineId);

#define kgrctxCtxBufferToFifoEngineId(arg0, pFifoEngineId) kgrctxCtxBufferToFifoEngineId_IMPL(arg0, pFifoEngineId)
NV_STATUS kgrctxGlobalCtxBufferToFifoEngineId_IMPL(GR_GLOBALCTX_BUFFER arg0, NvU32 *pFifoEngineId);

#define kgrctxGlobalCtxBufferToFifoEngineId(arg0, pFifoEngineId) kgrctxGlobalCtxBufferToFifoEngineId_IMPL(arg0, pFifoEngineId)
NV_STATUS kgrctxGetGidInfoInPlace_IMPL(struct OBJGPU *pGpu, NvU8 *pUuidBuffer, NvU32 uuidBufferSize, NvU32 flags);

#define kgrctxGetGidInfoInPlace(pGpu, pUuidBuffer, uuidBufferSize, flags) kgrctxGetGidInfoInPlace_IMPL(pGpu, pUuidBuffer, uuidBufferSize, flags)
GMMU_APERTURE kgrctxGetExternalAllocAperture_IMPL(NvU32 addressSpace);

#define kgrctxGetExternalAllocAperture(addressSpace) kgrctxGetExternalAllocAperture_IMPL(addressSpace)
NV_STATUS kgrctxFillCtxBufferInfo_IMPL(struct MEMORY_DESCRIPTOR *arg0, NvU32 externalId, NvBool bBufferGlobal, NV2080_CTRL_GR_CTX_BUFFER_INFO *arg1);

#define kgrctxFillCtxBufferInfo(arg0, externalId, bBufferGlobal, arg1) kgrctxFillCtxBufferInfo_IMPL(arg0, externalId, bBufferGlobal, arg1)
NV_STATUS kgrctxConstruct_IMPL(struct KernelGraphicsContext *arg_pKernelGraphicsContext, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_kgrctxConstruct(arg_pKernelGraphicsContext, arg_pCallContext, arg_pParams) kgrctxConstruct_IMPL(arg_pKernelGraphicsContext, arg_pCallContext, arg_pParams)
NV_STATUS kgrctxCopyConstruct_IMPL(struct KernelGraphicsContext *arg0, struct CALL_CONTEXT *arg1, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg2);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxCopyConstruct(struct KernelGraphicsContext *arg0, struct CALL_CONTEXT *arg1, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxCopyConstruct(arg0, arg1, arg2) kgrctxCopyConstruct_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxDestruct_IMPL(struct KernelGraphicsContext *arg0);

#define __nvoc_kgrctxDestruct(arg0) kgrctxDestruct_IMPL(arg0)
NV_STATUS kgrctxGetUnicast_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, KernelGraphicsContextUnicast **arg2);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxGetUnicast(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, KernelGraphicsContextUnicast **arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxGetUnicast(arg0, arg1, arg2) kgrctxGetUnicast_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxLookupMmuFaultInfo_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, NV83DE_CTRL_DEBUG_READ_MMU_FAULT_INFO_PARAMS *arg2);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxLookupMmuFaultInfo(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, NV83DE_CTRL_DEBUG_READ_MMU_FAULT_INFO_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxLookupMmuFaultInfo(arg0, arg1, arg2) kgrctxLookupMmuFaultInfo_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxLookupMmuFault_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, NV83DE_MMU_FAULT_INFO *arg2);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxLookupMmuFault(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, NV83DE_MMU_FAULT_INFO *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxLookupMmuFault(arg0, arg1, arg2) kgrctxLookupMmuFault_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxClearMmuFault_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxClearMmuFault(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxClearMmuFault(arg0, arg1) kgrctxClearMmuFault_IMPL(arg0, arg1)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxRecordMmuFault_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, NvU32 mmuFaultInfo, NvU64 mmuFaultAddress, NvU32 mmuFaultType, NvU32 mmuFaultAccessType);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxRecordMmuFault(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, NvU32 mmuFaultInfo, NvU64 mmuFaultAddress, NvU32 mmuFaultType, NvU32 mmuFaultAccessType) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxRecordMmuFault(arg0, arg1, mmuFaultInfo, mmuFaultAddress, mmuFaultType, mmuFaultAccessType) kgrctxRecordMmuFault_IMPL(arg0, arg1, mmuFaultInfo, mmuFaultAddress, mmuFaultType, mmuFaultAccessType)
#endif //__nvoc_kernel_graphics_context_h_disabled

KernelSMDebuggerSessionListIter kgrctxGetDebuggerSessionIter_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline KernelSMDebuggerSessionListIter kgrctxGetDebuggerSessionIter(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    KernelSMDebuggerSessionListIter ret;
    portMemSet(&ret, 0, sizeof(KernelSMDebuggerSessionListIter));
    return ret;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxGetDebuggerSessionIter(arg0, arg1) kgrctxGetDebuggerSessionIter_IMPL(arg0, arg1)
#endif //__nvoc_kernel_graphics_context_h_disabled

NvBool kgrctxRegisterKernelSMDebuggerSession_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelSMDebuggerSession *arg2);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NvBool kgrctxRegisterKernelSMDebuggerSession(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelSMDebuggerSession *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxRegisterKernelSMDebuggerSession(arg0, arg1, arg2) kgrctxRegisterKernelSMDebuggerSession_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxDeregisterKernelSMDebuggerSession_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelSMDebuggerSession *arg2);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxDeregisterKernelSMDebuggerSession(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelSMDebuggerSession *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxDeregisterKernelSMDebuggerSession(arg0, arg1, arg2) kgrctxDeregisterKernelSMDebuggerSession_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_graphics_context_h_disabled

NvBool kgrctxIsValid_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelChannel *arg2);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NvBool kgrctxIsValid(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelChannel *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxIsValid(arg0, arg1, arg2) kgrctxIsValid_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_graphics_context_h_disabled

NvBool kgrctxIsMainContextAllocated_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NvBool kgrctxIsMainContextAllocated(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxIsMainContextAllocated(arg0, arg1) kgrctxIsMainContextAllocated_IMPL(arg0, arg1)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxGetMainContextBuffer_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct MEMORY_DESCRIPTOR **arg2);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxGetMainContextBuffer(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct MEMORY_DESCRIPTOR **arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxGetMainContextBuffer(arg0, arg1, arg2) kgrctxGetMainContextBuffer_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxGetBufferCount_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, NvU32 *pBufferCount);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxGetBufferCount(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, NvU32 *pBufferCount) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxGetBufferCount(arg0, arg1, arg2, pBufferCount) kgrctxGetBufferCount_IMPL(arg0, arg1, arg2, pBufferCount)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxGetCtxBuffers_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, NvU32 gfid, NvU32 bufferCount, struct MEMORY_DESCRIPTOR **arg3, NvU32 *pCtxBufferType, NvU32 *pBufferCountOut, NvU32 *pFirstGlobalBuffer);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxGetCtxBuffers(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, NvU32 gfid, NvU32 bufferCount, struct MEMORY_DESCRIPTOR **arg3, NvU32 *pCtxBufferType, NvU32 *pBufferCountOut, NvU32 *pFirstGlobalBuffer) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxGetCtxBuffers(arg0, arg1, arg2, gfid, bufferCount, arg3, pCtxBufferType, pBufferCountOut, pFirstGlobalBuffer) kgrctxGetCtxBuffers_IMPL(arg0, arg1, arg2, gfid, bufferCount, arg3, pCtxBufferType, pBufferCountOut, pFirstGlobalBuffer)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxGetCtxBufferInfo_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, NvU32 gfid, NvU32 bufferMaxCount, NvU32 *pBufferCount, NV2080_CTRL_GR_CTX_BUFFER_INFO *arg3);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxGetCtxBufferInfo(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, NvU32 gfid, NvU32 bufferMaxCount, NvU32 *pBufferCount, NV2080_CTRL_GR_CTX_BUFFER_INFO *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxGetCtxBufferInfo(arg0, arg1, arg2, gfid, bufferMaxCount, pBufferCount, arg3) kgrctxGetCtxBufferInfo_IMPL(arg0, arg1, arg2, gfid, bufferMaxCount, pBufferCount, arg3)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxGetCtxBufferPtes_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, NvU32 gfid, NvU32 bufferType, NvU32 firstPage, NvU64 *pPhysAddrs, NvU32 addrsSize, NvU32 *pNumPages, NvBool *pbNoMorePages);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxGetCtxBufferPtes(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, NvU32 gfid, NvU32 bufferType, NvU32 firstPage, NvU64 *pPhysAddrs, NvU32 addrsSize, NvU32 *pNumPages, NvBool *pbNoMorePages) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxGetCtxBufferPtes(arg0, arg1, arg2, gfid, bufferType, firstPage, pPhysAddrs, addrsSize, pNumPages, pbNoMorePages) kgrctxGetCtxBufferPtes_IMPL(arg0, arg1, arg2, gfid, bufferType, firstPage, pPhysAddrs, addrsSize, pNumPages, pbNoMorePages)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxAllocMainCtxBuffer_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct KernelChannel *arg3);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxAllocMainCtxBuffer(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct KernelChannel *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxAllocMainCtxBuffer(arg0, arg1, arg2, arg3) kgrctxAllocMainCtxBuffer_IMPL(arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxAllocPatchBuffer_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct KernelChannel *arg3);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxAllocPatchBuffer(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct KernelChannel *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxAllocPatchBuffer(arg0, arg1, arg2, arg3) kgrctxAllocPatchBuffer_IMPL(arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxAllocPmBuffer_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct KernelChannel *arg3);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxAllocPmBuffer(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct KernelChannel *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxAllocPmBuffer(arg0, arg1, arg2, arg3) kgrctxAllocPmBuffer_IMPL(arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxAllocCtxBuffers_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct KernelGraphicsObject *arg3);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxAllocCtxBuffers(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct KernelGraphicsObject *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxAllocCtxBuffers(arg0, arg1, arg2, arg3) kgrctxAllocCtxBuffers_IMPL(arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxMapGlobalCtxBuffer_IMPL(struct OBJGPU *pGpu, struct KernelGraphicsContext *arg0, struct KernelGraphics *arg1, NvU32 gfid, struct OBJVASPACE *arg2, GR_GLOBALCTX_BUFFER arg3, NvBool bIsReadOnly);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxMapGlobalCtxBuffer(struct OBJGPU *pGpu, struct KernelGraphicsContext *arg0, struct KernelGraphics *arg1, NvU32 gfid, struct OBJVASPACE *arg2, GR_GLOBALCTX_BUFFER arg3, NvBool bIsReadOnly) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxMapGlobalCtxBuffer(pGpu, arg0, arg1, gfid, arg2, arg3, bIsReadOnly) kgrctxMapGlobalCtxBuffer_IMPL(pGpu, arg0, arg1, gfid, arg2, arg3, bIsReadOnly)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxMapGlobalCtxBuffers_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, NvU32 gfid, struct KernelChannel *arg3);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxMapGlobalCtxBuffers(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, NvU32 gfid, struct KernelChannel *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxMapGlobalCtxBuffers(arg0, arg1, arg2, gfid, arg3) kgrctxMapGlobalCtxBuffers_IMPL(arg0, arg1, arg2, gfid, arg3)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxMapCtxBuffers_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct KernelGraphicsObject *arg3);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxMapCtxBuffers(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct KernelGraphicsObject *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxMapCtxBuffers(arg0, arg1, arg2, arg3) kgrctxMapCtxBuffers_IMPL(arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxPrepareInitializeCtxBuffer_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct KernelChannel *arg3, NvU32 externalId, NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ENTRY *arg4, NvBool *pbAddEntry);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxPrepareInitializeCtxBuffer(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct KernelChannel *arg3, NvU32 externalId, NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ENTRY *arg4, NvBool *pbAddEntry) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxPrepareInitializeCtxBuffer(arg0, arg1, arg2, arg3, externalId, arg4, pbAddEntry) kgrctxPrepareInitializeCtxBuffer_IMPL(arg0, arg1, arg2, arg3, externalId, arg4, pbAddEntry)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxPreparePromoteCtxBuffer_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelChannel *arg2, NvU32 externalId, NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ENTRY *arg3, NvBool *pbAddEntry);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxPreparePromoteCtxBuffer(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelChannel *arg2, NvU32 externalId, NV2080_CTRL_GPU_PROMOTE_CTX_BUFFER_ENTRY *arg3, NvBool *pbAddEntry) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxPreparePromoteCtxBuffer(arg0, arg1, arg2, externalId, arg3, pbAddEntry) kgrctxPreparePromoteCtxBuffer_IMPL(arg0, arg1, arg2, externalId, arg3, pbAddEntry)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxMarkCtxBufferInitialized_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct KernelChannel *arg3, NvU32 externalId);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxMarkCtxBufferInitialized(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct KernelChannel *arg3, NvU32 externalId) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxMarkCtxBufferInitialized(arg0, arg1, arg2, arg3, externalId) kgrctxMarkCtxBufferInitialized_IMPL(arg0, arg1, arg2, arg3, externalId)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS kgrctxSetupDeferredPmBuffer_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct KernelChannel *arg3);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS kgrctxSetupDeferredPmBuffer(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct KernelChannel *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxSetupDeferredPmBuffer(arg0, arg1, arg2, arg3) kgrctxSetupDeferredPmBuffer_IMPL(arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxUnmapGlobalCtxBuffers_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct OBJVASPACE *arg3, NvU32 gfid);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxUnmapGlobalCtxBuffers(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct OBJVASPACE *arg3, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxUnmapGlobalCtxBuffers(arg0, arg1, arg2, arg3, gfid) kgrctxUnmapGlobalCtxBuffers_IMPL(arg0, arg1, arg2, arg3, gfid)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxUnmapGlobalCtxBuffer_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct OBJVASPACE *arg3, GR_GLOBALCTX_BUFFER arg4);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxUnmapGlobalCtxBuffer(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct OBJVASPACE *arg3, GR_GLOBALCTX_BUFFER arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxUnmapGlobalCtxBuffer(arg0, arg1, arg2, arg3, arg4) kgrctxUnmapGlobalCtxBuffer_IMPL(arg0, arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxUnmapMainCtxBuffer_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct KernelChannel *arg3);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxUnmapMainCtxBuffer(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct KernelChannel *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxUnmapMainCtxBuffer(arg0, arg1, arg2, arg3) kgrctxUnmapMainCtxBuffer_IMPL(arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxUnmapCtxPmBuffer_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct OBJVASPACE *arg3);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxUnmapCtxPmBuffer(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct OBJVASPACE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxUnmapCtxPmBuffer(arg0, arg1, arg2, arg3) kgrctxUnmapCtxPmBuffer_IMPL(arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxUnmapCtxZcullBuffer_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct OBJVASPACE *arg3);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxUnmapCtxZcullBuffer(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct OBJVASPACE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxUnmapCtxZcullBuffer(arg0, arg1, arg2, arg3) kgrctxUnmapCtxZcullBuffer_IMPL(arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxUnmapCtxPreemptionBuffers_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct OBJVASPACE *arg3);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxUnmapCtxPreemptionBuffers(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct OBJVASPACE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxUnmapCtxPreemptionBuffers(arg0, arg1, arg2, arg3) kgrctxUnmapCtxPreemptionBuffers_IMPL(arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxUnmapAssociatedCtxBuffers_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct KernelChannel *arg3);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxUnmapAssociatedCtxBuffers(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1, struct KernelGraphics *arg2, struct KernelChannel *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxUnmapAssociatedCtxBuffers(arg0, arg1, arg2, arg3) kgrctxUnmapAssociatedCtxBuffers_IMPL(arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxFreeMainCtxBuffer_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxFreeMainCtxBuffer(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxFreeMainCtxBuffer(arg0, arg1) kgrctxFreeMainCtxBuffer_IMPL(arg0, arg1)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxFreeZcullBuffer_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxFreeZcullBuffer(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxFreeZcullBuffer(arg0, arg1) kgrctxFreeZcullBuffer_IMPL(arg0, arg1)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxFreeCtxPreemptionBuffers_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxFreeCtxPreemptionBuffers(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxFreeCtxPreemptionBuffers(arg0, arg1) kgrctxFreeCtxPreemptionBuffers_IMPL(arg0, arg1)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxFreePatchBuffer_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxFreePatchBuffer(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxFreePatchBuffer(arg0, arg1) kgrctxFreePatchBuffer_IMPL(arg0, arg1)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxFreePmBuffer_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxFreePmBuffer(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxFreePmBuffer(arg0, arg1) kgrctxFreePmBuffer_IMPL(arg0, arg1)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxFreeLocalGlobalCtxBuffers_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxFreeLocalGlobalCtxBuffers(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxFreeLocalGlobalCtxBuffers(arg0, arg1) kgrctxFreeLocalGlobalCtxBuffers_IMPL(arg0, arg1)
#endif //__nvoc_kernel_graphics_context_h_disabled

void kgrctxFreeAssociatedCtxBuffers_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void kgrctxFreeAssociatedCtxBuffers(struct OBJGPU *arg0, struct KernelGraphicsContext *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContext was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define kgrctxFreeAssociatedCtxBuffers(arg0, arg1) kgrctxFreeAssociatedCtxBuffers_IMPL(arg0, arg1)
#endif //__nvoc_kernel_graphics_context_h_disabled

#undef PRIVATE_FIELD


/**
 * This refcounted class encapsulates the context data that is shared when a
 * context is duped.
 */
#ifdef NVOC_KERNEL_GRAPHICS_CONTEXT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
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
    const struct NVOC_RTTI *__nvoc_rtti;
    struct RsShared __nvoc_base_RsShared;
    struct Object *__nvoc_pbase_Object;
    struct RsShared *__nvoc_pbase_RsShared;
    struct KernelGraphicsContextShared *__nvoc_pbase_KernelGraphicsContextShared;
    struct KernelGraphicsContextUnicast PRIVATE_FIELD(kernelGraphicsContextUnicast);
    KernelSMDebuggerSessionList PRIVATE_FIELD(activeDebuggers);
};

#ifndef __NVOC_CLASS_KernelGraphicsContextShared_TYPEDEF__
#define __NVOC_CLASS_KernelGraphicsContextShared_TYPEDEF__
typedef struct KernelGraphicsContextShared KernelGraphicsContextShared;
#endif /* __NVOC_CLASS_KernelGraphicsContextShared_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGraphicsContextShared
#define __nvoc_class_id_KernelGraphicsContextShared 0xe7abeb
#endif /* __nvoc_class_id_KernelGraphicsContextShared */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGraphicsContextShared;

#define __staticCast_KernelGraphicsContextShared(pThis) \
    ((pThis)->__nvoc_pbase_KernelGraphicsContextShared)

#ifdef __nvoc_kernel_graphics_context_h_disabled
#define __dynamicCast_KernelGraphicsContextShared(pThis) ((KernelGraphicsContextShared*)NULL)
#else //__nvoc_kernel_graphics_context_h_disabled
#define __dynamicCast_KernelGraphicsContextShared(pThis) \
    ((KernelGraphicsContextShared*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelGraphicsContextShared)))
#endif //__nvoc_kernel_graphics_context_h_disabled


NV_STATUS __nvoc_objCreateDynamic_KernelGraphicsContextShared(KernelGraphicsContextShared**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelGraphicsContextShared(KernelGraphicsContextShared**, Dynamic*, NvU32);
#define __objCreate_KernelGraphicsContextShared(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelGraphicsContextShared((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

NV_STATUS shrkgrctxConstruct_IMPL(struct KernelGraphicsContextShared *arg_);

#define __nvoc_shrkgrctxConstruct(arg_) shrkgrctxConstruct_IMPL(arg_)
NV_STATUS shrkgrctxInit_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContextShared *arg1, struct KernelGraphicsContext *arg2);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS shrkgrctxInit(struct OBJGPU *arg0, struct KernelGraphicsContextShared *arg1, struct KernelGraphicsContext *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContextShared was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define shrkgrctxInit(arg0, arg1, arg2) shrkgrctxInit_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_graphics_context_h_disabled

NV_STATUS shrkgrctxConstructUnicast_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContextShared *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, KernelGraphicsContextUnicast *arg4);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline NV_STATUS shrkgrctxConstructUnicast(struct OBJGPU *arg0, struct KernelGraphicsContextShared *arg1, struct KernelGraphicsContext *arg2, struct KernelGraphics *arg3, KernelGraphicsContextUnicast *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContextShared was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define shrkgrctxConstructUnicast(arg0, arg1, arg2, arg3, arg4) shrkgrctxConstructUnicast_IMPL(arg0, arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_graphics_context_h_disabled

void shrkgrctxDestruct_IMPL(struct KernelGraphicsContextShared *arg0);

#define __nvoc_shrkgrctxDestruct(arg0) shrkgrctxDestruct_IMPL(arg0)
void shrkgrctxTeardown_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContextShared *arg1, struct KernelGraphicsContext *arg2);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void shrkgrctxTeardown(struct OBJGPU *arg0, struct KernelGraphicsContextShared *arg1, struct KernelGraphicsContext *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContextShared was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define shrkgrctxTeardown(arg0, arg1, arg2) shrkgrctxTeardown_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_graphics_context_h_disabled

void shrkgrctxDestructUnicast_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContextShared *arg1, struct KernelGraphicsContext *arg2, KernelGraphicsContextUnicast *arg3);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void shrkgrctxDestructUnicast(struct OBJGPU *arg0, struct KernelGraphicsContextShared *arg1, struct KernelGraphicsContext *arg2, KernelGraphicsContextUnicast *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContextShared was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define shrkgrctxDestructUnicast(arg0, arg1, arg2, arg3) shrkgrctxDestructUnicast_IMPL(arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_context_h_disabled

void shrkgrctxDetach_IMPL(struct OBJGPU *arg0, struct KernelGraphicsContextShared *arg1, struct KernelGraphicsContext *arg2, struct KernelChannel *arg3);

#ifdef __nvoc_kernel_graphics_context_h_disabled
static inline void shrkgrctxDetach(struct OBJGPU *arg0, struct KernelGraphicsContextShared *arg1, struct KernelGraphicsContext *arg2, struct KernelChannel *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsContextShared was disabled!");
}
#else //__nvoc_kernel_graphics_context_h_disabled
#define shrkgrctxDetach(arg0, arg1, arg2, arg3) shrkgrctxDetach_IMPL(arg0, arg1, arg2, arg3)
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
