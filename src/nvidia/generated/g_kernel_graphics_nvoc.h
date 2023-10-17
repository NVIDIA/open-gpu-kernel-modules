#ifndef _G_KERNEL_GRAPHICS_NVOC_H_
#define _G_KERNEL_GRAPHICS_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_kernel_graphics_nvoc.h"

#ifndef _KERNEL_GRAPHICS_H_
#define _KERNEL_GRAPHICS_H_

#include "core/core.h"
#include "gpu/eng_state.h"
#include "gpu/gpu.h"
#include "kernel/gpu/gr/kernel_graphics_object.h"
#include "kernel/gpu/gr/kernel_graphics_context.h"
#include "kernel/mem_mgr/ctx_buf_pool.h"
#include "kernel/gpu/gr/fecs_event_list.h"
#include "eventbufferproducer.h"
#include "kernel/gpu/intr/intr_service.h"

#include "ctrl/ctrl2080/ctrl2080internal.h"


#define GR_VERTEX_CACHE_SIZE 16

struct KGRAPHICS_STATIC_INFO;
typedef struct KGRAPHICS_STATIC_INFO KGRAPHICS_STATIC_INFO;
typedef struct KGRAPHICS_FECS_TRACE_INFO KGRAPHICS_FECS_TRACE_INFO;
typedef struct KGRAPHICS_GLOBAL_CTX_BUFFERS_INFO KGRAPHICS_GLOBAL_CTX_BUFFERS_INFO;

/*!
 * Static info retrieved from Physical RM detailing the configuration of the
 * visible portions of this graphics engine. This data is mostly used to service
 * control calls which export data from RM.
 */
struct KGRAPHICS_STATIC_INFO
{
    //
    // @ref NV0080_CTRL_CMD_GR_GET_INFO
    // @ref NV0080_CTRL_CMD_GR_GET_INFO_V2
    // @ref NV2080_CTRL_CMD_GR_GET_INFO
    // @ref NV2080_CTRL_CMD_GR_GET_INFO_V2
    //
    NV2080_CTRL_INTERNAL_STATIC_GR_INFO *pGrInfo;

    //
    // @ref NV0080_CTRL_CMD_GR_GET_CAPS
    // @ref NV0080_CTRL_CMD_GR_GET_CAPS_V2
    // @ref NV2080_CTRL_CMD_GR_GET_CAPS
    // @ref NV2080_CTRL_CMD_GR_GET_CAPS_V2
    //
    NV2080_CTRL_INTERNAL_STATIC_GR_CAPS grCaps;

    //
    // @ref NV2080_CTRL_CMD_GR_GET_GLOBAL_SM_ORDER
    // @ref NV2080_CTRL_CMD_GR_GET_SM_TO_GPC_TPC_MAPPINGS
    //
    NV2080_CTRL_INTERNAL_STATIC_GR_GLOBAL_SM_ORDER globalSmOrder;

    //
    // @ref NV2080_CTRL_CMD_GR_GET_GPC_MASK
    // @ref NV2080_CTRL_CMD_GR_GET_TPC_MASK
    // @ref NV2080_CTRL_CMD_GR_GET_PHYS_GPC_MASK
    //
    NV2080_CTRL_INTERNAL_STATIC_GR_FLOORSWEEPING_MASKS floorsweepingMasks;

    // @ref NV2080_CTRL_CMD_GR_GET_PPC_MASK
    NV2080_CTRL_INTERNAL_STATIC_GR_PPC_MASKS *pPpcMasks;

    // @ref NV2080_CTRL_CMD_GR_GET_ZCULL_INFO
    NV2080_CTRL_INTERNAL_STATIC_GR_ZCULL_INFO *pZcullInfo;

    // @ref NV2080_CTRL_CMD_GR_GET_ROP_INFO
    NV2080_CTRL_INTERNAL_STATIC_GR_ROP_INFO *pRopInfo;

    //
    // @ref NV2080_CTRL_CMD_GR_GET_ENGINE_CONTEXT_PROPERTIES
    // @ref NV2080_CTRL_CMD_GR_GET_ATTRIBUTE_BUFFER_SIZE
    //
    NV2080_CTRL_INTERNAL_STATIC_GR_CONTEXT_BUFFERS_INFO *pContextBuffersInfo;

    // @ref NV2080_CTRL_CMD_GR_GET_SM_ISSUE_RATE_MODIFIER
    NV2080_CTRL_INTERNAL_STATIC_GR_SM_ISSUE_RATE_MODIFIER *pSmIssueRateModifier;

    //
    // @ref NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_FECS_RECORD_SIZE
    // @ref NV2080_CTRL_CMD_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE
    //
    NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE fecsRecordSize;

    //
    // @ref NV2080_CTRL_CMD_INTERNAL_STATIC_KGR_GET_FECS_TRACE_DEFINES
    // @ref NV2080_CTRL_CMD_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES
    //
    NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES *pFecsTraceDefines;

    // @ref bPerSubcontextContextHeaderSupported
    NV2080_CTRL_INTERNAL_STATIC_GR_PDB_PROPERTIES pdbTable;
};

struct KGRAPHICS_GLOBAL_CTX_BUFFERS_INFO
{
    NvU32 globalCtxBufferSize;

    GR_GLOBALCTX_BUFFERS *pGlobalCtxBuffers;
    NvBool                bSizeAligned[GR_GLOBALCTX_BUFFER_COUNT];
    GR_BUFFER_ATTR        globalCtxAttr[GR_GLOBALCTX_BUFFER_COUNT];
    GR_BUFFER_ATTR        localCtxAttr[GR_GLOBALCTX_BUFFER_COUNT];
    GR_BUFFER_ATTR        vfGlobalCtxAttr[GR_GLOBALCTX_BUFFER_COUNT];
};

// Opaque forward declarations
typedef struct KGRAPHICS_PRIVATE_DATA KGRAPHICS_PRIVATE_DATA;
typedef struct KGRAPHICS_FECS_TRACE_INFO KGRAPHICS_FECS_TRACE_INFO;

/*!
 * KernelGraphics is a logical abstraction of the GPU Graphics Engine. The
 * Public API of the Graphics Engine is exposed through this object, and any
 * interfaces which do not manage the underlying Graphics hardware can be
 * managed by this object.
 */
#ifdef NVOC_KERNEL_GRAPHICS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelGraphics {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct IntrService __nvoc_base_IntrService;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct IntrService *__nvoc_pbase_IntrService;
    struct KernelGraphics *__nvoc_pbase_KernelGraphics;
    NV_STATUS (*__kgraphicsConstructEngine__)(OBJGPU *, struct KernelGraphics *, ENGDESCRIPTOR);
    NV_STATUS (*__kgraphicsStateInitLocked__)(OBJGPU *, struct KernelGraphics *);
    NV_STATUS (*__kgraphicsStateLoad__)(OBJGPU *, struct KernelGraphics *, NvU32);
    NV_STATUS (*__kgraphicsStatePreUnload__)(OBJGPU *, struct KernelGraphics *, NvU32);
    NV_STATUS (*__kgraphicsStateUnload__)(OBJGPU *, struct KernelGraphics *, NvU32);
    void (*__kgraphicsStateDestroy__)(OBJGPU *, struct KernelGraphics *);
    NvBool (*__kgraphicsIsPresent__)(OBJGPU *, struct KernelGraphics *);
    NV_STATUS (*__kgraphicsStatePostLoad__)(OBJGPU *, struct KernelGraphics *, NvU32);
    void (*__kgraphicsRegisterIntrService__)(OBJGPU *, struct KernelGraphics *, IntrServiceRecord *);
    NV_STATUS (*__kgraphicsServiceNotificationInterrupt__)(OBJGPU *, struct KernelGraphics *, IntrServiceServiceNotificationInterruptArguments *);
    NvBool (*__kgraphicsClearInterrupt__)(OBJGPU *, struct KernelGraphics *, IntrServiceClearInterruptArguments *);
    NvU32 (*__kgraphicsServiceInterrupt__)(OBJGPU *, struct KernelGraphics *, IntrServiceServiceInterruptArguments *);
    NV_STATUS (*__kgraphicsStatePreLoad__)(POBJGPU, struct KernelGraphics *, NvU32);
    NV_STATUS (*__kgraphicsStatePostUnload__)(POBJGPU, struct KernelGraphics *, NvU32);
    NV_STATUS (*__kgraphicsStateInitUnlocked__)(POBJGPU, struct KernelGraphics *);
    void (*__kgraphicsInitMissing__)(POBJGPU, struct KernelGraphics *);
    NV_STATUS (*__kgraphicsStatePreInitLocked__)(POBJGPU, struct KernelGraphics *);
    NV_STATUS (*__kgraphicsStatePreInitUnlocked__)(POBJGPU, struct KernelGraphics *);
    NvBool PRIVATE_FIELD(bCtxswLoggingSupported);
    NvBool PRIVATE_FIELD(bIntrDrivenCtxswLoggingEnabled);
    NvBool PRIVATE_FIELD(bBottomHalfCtxswLoggingEnabled);
    NvBool PRIVATE_FIELD(bDeferContextInit);
    NvBool PRIVATE_FIELD(bPerSubcontextContextHeaderSupported);
    NvBool PRIVATE_FIELD(bSetContextBuffersGPUPrivileged);
    NvBool PRIVATE_FIELD(bUcodeSupportsPrivAccessMap);
    NvBool PRIVATE_FIELD(bRtvCbSupported);
    NvBool PRIVATE_FIELD(bFecsRecordUcodeSeqnoSupported);
    NvU32 PRIVATE_FIELD(instance);
    KGRAPHICS_PRIVATE_DATA *PRIVATE_FIELD(pPrivate);
    NvBool PRIVATE_FIELD(bCollectingDeferredStaticData);
    NvBool PRIVATE_FIELD(bBug4162646War);
    KGRAPHICS_FECS_TRACE_INFO *PRIVATE_FIELD(pFecsTraceInfo);
    struct KGRAPHICS_GLOBAL_CTX_BUFFERS_INFO PRIVATE_FIELD(globalCtxBuffersInfo);
    struct CTX_BUF_POOL_INFO *PRIVATE_FIELD(pCtxBufPool);
    CTX_BUF_INFO PRIVATE_FIELD(maxCtxBufSize)[9];
    GR_BUFFER_ATTR PRIVATE_FIELD(ctxAttr)[9];
};

struct KernelGraphics_PRIVATE {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct IntrService __nvoc_base_IntrService;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct IntrService *__nvoc_pbase_IntrService;
    struct KernelGraphics *__nvoc_pbase_KernelGraphics;
    NV_STATUS (*__kgraphicsConstructEngine__)(OBJGPU *, struct KernelGraphics *, ENGDESCRIPTOR);
    NV_STATUS (*__kgraphicsStateInitLocked__)(OBJGPU *, struct KernelGraphics *);
    NV_STATUS (*__kgraphicsStateLoad__)(OBJGPU *, struct KernelGraphics *, NvU32);
    NV_STATUS (*__kgraphicsStatePreUnload__)(OBJGPU *, struct KernelGraphics *, NvU32);
    NV_STATUS (*__kgraphicsStateUnload__)(OBJGPU *, struct KernelGraphics *, NvU32);
    void (*__kgraphicsStateDestroy__)(OBJGPU *, struct KernelGraphics *);
    NvBool (*__kgraphicsIsPresent__)(OBJGPU *, struct KernelGraphics *);
    NV_STATUS (*__kgraphicsStatePostLoad__)(OBJGPU *, struct KernelGraphics *, NvU32);
    void (*__kgraphicsRegisterIntrService__)(OBJGPU *, struct KernelGraphics *, IntrServiceRecord *);
    NV_STATUS (*__kgraphicsServiceNotificationInterrupt__)(OBJGPU *, struct KernelGraphics *, IntrServiceServiceNotificationInterruptArguments *);
    NvBool (*__kgraphicsClearInterrupt__)(OBJGPU *, struct KernelGraphics *, IntrServiceClearInterruptArguments *);
    NvU32 (*__kgraphicsServiceInterrupt__)(OBJGPU *, struct KernelGraphics *, IntrServiceServiceInterruptArguments *);
    NV_STATUS (*__kgraphicsStatePreLoad__)(POBJGPU, struct KernelGraphics *, NvU32);
    NV_STATUS (*__kgraphicsStatePostUnload__)(POBJGPU, struct KernelGraphics *, NvU32);
    NV_STATUS (*__kgraphicsStateInitUnlocked__)(POBJGPU, struct KernelGraphics *);
    void (*__kgraphicsInitMissing__)(POBJGPU, struct KernelGraphics *);
    NV_STATUS (*__kgraphicsStatePreInitLocked__)(POBJGPU, struct KernelGraphics *);
    NV_STATUS (*__kgraphicsStatePreInitUnlocked__)(POBJGPU, struct KernelGraphics *);
    NvBool bCtxswLoggingSupported;
    NvBool bIntrDrivenCtxswLoggingEnabled;
    NvBool bBottomHalfCtxswLoggingEnabled;
    NvBool bDeferContextInit;
    NvBool bPerSubcontextContextHeaderSupported;
    NvBool bSetContextBuffersGPUPrivileged;
    NvBool bUcodeSupportsPrivAccessMap;
    NvBool bRtvCbSupported;
    NvBool bFecsRecordUcodeSeqnoSupported;
    NvU32 instance;
    KGRAPHICS_PRIVATE_DATA *pPrivate;
    NvBool bCollectingDeferredStaticData;
    NvBool bBug4162646War;
    KGRAPHICS_FECS_TRACE_INFO *pFecsTraceInfo;
    struct KGRAPHICS_GLOBAL_CTX_BUFFERS_INFO globalCtxBuffersInfo;
    struct CTX_BUF_POOL_INFO *pCtxBufPool;
    CTX_BUF_INFO maxCtxBufSize[9];
    GR_BUFFER_ATTR ctxAttr[9];
};

#ifndef __NVOC_CLASS_KernelGraphics_TYPEDEF__
#define __NVOC_CLASS_KernelGraphics_TYPEDEF__
typedef struct KernelGraphics KernelGraphics;
#endif /* __NVOC_CLASS_KernelGraphics_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGraphics
#define __nvoc_class_id_KernelGraphics 0xea3fa9
#endif /* __nvoc_class_id_KernelGraphics */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGraphics;

#define __staticCast_KernelGraphics(pThis) \
    ((pThis)->__nvoc_pbase_KernelGraphics)

#ifdef __nvoc_kernel_graphics_h_disabled
#define __dynamicCast_KernelGraphics(pThis) ((KernelGraphics*)NULL)
#else //__nvoc_kernel_graphics_h_disabled
#define __dynamicCast_KernelGraphics(pThis) \
    ((KernelGraphics*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelGraphics)))
#endif //__nvoc_kernel_graphics_h_disabled

#define PDB_PROP_KGRAPHICS_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KGRAPHICS_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelGraphics(KernelGraphics**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelGraphics(KernelGraphics**, Dynamic*, NvU32);
#define __objCreate_KernelGraphics(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelGraphics((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define kgraphicsConstructEngine(arg0, arg1, arg2) kgraphicsConstructEngine_DISPATCH(arg0, arg1, arg2)
#define kgraphicsStateInitLocked(arg0, arg1) kgraphicsStateInitLocked_DISPATCH(arg0, arg1)
#define kgraphicsStateLoad(arg0, arg1, flags) kgraphicsStateLoad_DISPATCH(arg0, arg1, flags)
#define kgraphicsStatePreUnload(pGpu, arg0, flags) kgraphicsStatePreUnload_DISPATCH(pGpu, arg0, flags)
#define kgraphicsStateUnload(arg0, arg1, flags) kgraphicsStateUnload_DISPATCH(arg0, arg1, flags)
#define kgraphicsStateDestroy(arg0, arg1) kgraphicsStateDestroy_DISPATCH(arg0, arg1)
#define kgraphicsIsPresent(arg0, arg1) kgraphicsIsPresent_DISPATCH(arg0, arg1)
#define kgraphicsStatePostLoad(arg0, arg1, flags) kgraphicsStatePostLoad_DISPATCH(arg0, arg1, flags)
#define kgraphicsRegisterIntrService(arg0, arg1, arg2) kgraphicsRegisterIntrService_DISPATCH(arg0, arg1, arg2)
#define kgraphicsServiceNotificationInterrupt(arg0, arg1, arg2) kgraphicsServiceNotificationInterrupt_DISPATCH(arg0, arg1, arg2)
#define kgraphicsClearInterrupt(arg0, arg1, arg2) kgraphicsClearInterrupt_DISPATCH(arg0, arg1, arg2)
#define kgraphicsClearInterrupt_HAL(arg0, arg1, arg2) kgraphicsClearInterrupt_DISPATCH(arg0, arg1, arg2)
#define kgraphicsServiceInterrupt(arg0, arg1, arg2) kgraphicsServiceInterrupt_DISPATCH(arg0, arg1, arg2)
#define kgraphicsServiceInterrupt_HAL(arg0, arg1, arg2) kgraphicsServiceInterrupt_DISPATCH(arg0, arg1, arg2)
#define kgraphicsStatePreLoad(pGpu, pEngstate, arg0) kgraphicsStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kgraphicsStatePostUnload(pGpu, pEngstate, arg0) kgraphicsStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kgraphicsStateInitUnlocked(pGpu, pEngstate) kgraphicsStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kgraphicsInitMissing(pGpu, pEngstate) kgraphicsInitMissing_DISPATCH(pGpu, pEngstate)
#define kgraphicsStatePreInitLocked(pGpu, pEngstate) kgraphicsStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kgraphicsStatePreInitUnlocked(pGpu, pEngstate) kgraphicsStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
static inline NvBool kgraphicsShouldForceMainCtxContiguity_cbe027(OBJGPU *arg0, struct KernelGraphics *arg1) {
    return ((NvBool)(0 == 0));
}


#ifdef __nvoc_kernel_graphics_h_disabled
static inline NvBool kgraphicsShouldForceMainCtxContiguity(OBJGPU *arg0, struct KernelGraphics *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsShouldForceMainCtxContiguity(arg0, arg1) kgraphicsShouldForceMainCtxContiguity_cbe027(arg0, arg1)
#endif //__nvoc_kernel_graphics_h_disabled

#define kgraphicsShouldForceMainCtxContiguity_HAL(arg0, arg1) kgraphicsShouldForceMainCtxContiguity(arg0, arg1)

NV_STATUS kgraphicsAllocKgraphicsBuffers_KERNEL(OBJGPU *arg0, struct KernelGraphics *arg1, struct KernelGraphicsContext *arg2, struct KernelChannel *arg3);


#ifdef __nvoc_kernel_graphics_h_disabled
static inline NV_STATUS kgraphicsAllocKgraphicsBuffers(OBJGPU *arg0, struct KernelGraphics *arg1, struct KernelGraphicsContext *arg2, struct KernelChannel *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsAllocKgraphicsBuffers(arg0, arg1, arg2, arg3) kgraphicsAllocKgraphicsBuffers_KERNEL(arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_h_disabled

#define kgraphicsAllocKgraphicsBuffers_HAL(arg0, arg1, arg2, arg3) kgraphicsAllocKgraphicsBuffers(arg0, arg1, arg2, arg3)

NV_STATUS kgraphicsAllocGrGlobalCtxBuffers_TU102(OBJGPU *arg0, struct KernelGraphics *arg1, NvU32 gfid, struct KernelGraphicsContext *arg2);


#ifdef __nvoc_kernel_graphics_h_disabled
static inline NV_STATUS kgraphicsAllocGrGlobalCtxBuffers(OBJGPU *arg0, struct KernelGraphics *arg1, NvU32 gfid, struct KernelGraphicsContext *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsAllocGrGlobalCtxBuffers(arg0, arg1, gfid, arg2) kgraphicsAllocGrGlobalCtxBuffers_TU102(arg0, arg1, gfid, arg2)
#endif //__nvoc_kernel_graphics_h_disabled

#define kgraphicsAllocGrGlobalCtxBuffers_HAL(arg0, arg1, gfid, arg2) kgraphicsAllocGrGlobalCtxBuffers(arg0, arg1, gfid, arg2)

NV_STATUS kgraphicsAllocGlobalCtxBuffers_GP100(OBJGPU *arg0, struct KernelGraphics *arg1, NvU32 gfid);


#ifdef __nvoc_kernel_graphics_h_disabled
static inline NV_STATUS kgraphicsAllocGlobalCtxBuffers(OBJGPU *arg0, struct KernelGraphics *arg1, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsAllocGlobalCtxBuffers(arg0, arg1, gfid) kgraphicsAllocGlobalCtxBuffers_GP100(arg0, arg1, gfid)
#endif //__nvoc_kernel_graphics_h_disabled

#define kgraphicsAllocGlobalCtxBuffers_HAL(arg0, arg1, gfid) kgraphicsAllocGlobalCtxBuffers(arg0, arg1, gfid)

NV_STATUS kgraphicsLoadStaticInfo_KERNEL(OBJGPU *arg0, struct KernelGraphics *arg1, NvU32 swizzId);


#ifdef __nvoc_kernel_graphics_h_disabled
static inline NV_STATUS kgraphicsLoadStaticInfo(OBJGPU *arg0, struct KernelGraphics *arg1, NvU32 swizzId) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsLoadStaticInfo(arg0, arg1, swizzId) kgraphicsLoadStaticInfo_KERNEL(arg0, arg1, swizzId)
#endif //__nvoc_kernel_graphics_h_disabled

#define kgraphicsLoadStaticInfo_HAL(arg0, arg1, swizzId) kgraphicsLoadStaticInfo(arg0, arg1, swizzId)

static inline void kgraphicsNonstallIntrCheckAndClear_b3696a(OBJGPU *arg0, struct KernelGraphics *arg1, struct THREAD_STATE_NODE *arg2) {
    return;
}


#ifdef __nvoc_kernel_graphics_h_disabled
static inline void kgraphicsNonstallIntrCheckAndClear(OBJGPU *arg0, struct KernelGraphics *arg1, struct THREAD_STATE_NODE *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsNonstallIntrCheckAndClear(arg0, arg1, arg2) kgraphicsNonstallIntrCheckAndClear_b3696a(arg0, arg1, arg2)
#endif //__nvoc_kernel_graphics_h_disabled

#define kgraphicsNonstallIntrCheckAndClear_HAL(arg0, arg1, arg2) kgraphicsNonstallIntrCheckAndClear(arg0, arg1, arg2)

void kgraphicsInitFecsRegistryOverrides_GP100(OBJGPU *arg0, struct KernelGraphics *arg1);


#ifdef __nvoc_kernel_graphics_h_disabled
static inline void kgraphicsInitFecsRegistryOverrides(OBJGPU *arg0, struct KernelGraphics *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsInitFecsRegistryOverrides(arg0, arg1) kgraphicsInitFecsRegistryOverrides_GP100(arg0, arg1)
#endif //__nvoc_kernel_graphics_h_disabled

#define kgraphicsInitFecsRegistryOverrides_HAL(arg0, arg1) kgraphicsInitFecsRegistryOverrides(arg0, arg1)

NvBool kgraphicsIsUnrestrictedAccessMapSupported_PF(OBJGPU *arg0, struct KernelGraphics *arg1);


#ifdef __nvoc_kernel_graphics_h_disabled
static inline NvBool kgraphicsIsUnrestrictedAccessMapSupported(OBJGPU *arg0, struct KernelGraphics *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsIsUnrestrictedAccessMapSupported(arg0, arg1) kgraphicsIsUnrestrictedAccessMapSupported_PF(arg0, arg1)
#endif //__nvoc_kernel_graphics_h_disabled

#define kgraphicsIsUnrestrictedAccessMapSupported_HAL(arg0, arg1) kgraphicsIsUnrestrictedAccessMapSupported(arg0, arg1)

NV_STATUS kgraphicsConstructEngine_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1, ENGDESCRIPTOR arg2);

static inline NV_STATUS kgraphicsConstructEngine_DISPATCH(OBJGPU *arg0, struct KernelGraphics *arg1, ENGDESCRIPTOR arg2) {
    return arg1->__kgraphicsConstructEngine__(arg0, arg1, arg2);
}

NV_STATUS kgraphicsStateInitLocked_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1);

static inline NV_STATUS kgraphicsStateInitLocked_DISPATCH(OBJGPU *arg0, struct KernelGraphics *arg1) {
    return arg1->__kgraphicsStateInitLocked__(arg0, arg1);
}

NV_STATUS kgraphicsStateLoad_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1, NvU32 flags);

static inline NV_STATUS kgraphicsStateLoad_DISPATCH(OBJGPU *arg0, struct KernelGraphics *arg1, NvU32 flags) {
    return arg1->__kgraphicsStateLoad__(arg0, arg1, flags);
}

NV_STATUS kgraphicsStatePreUnload_IMPL(OBJGPU *pGpu, struct KernelGraphics *arg0, NvU32 flags);

static inline NV_STATUS kgraphicsStatePreUnload_DISPATCH(OBJGPU *pGpu, struct KernelGraphics *arg0, NvU32 flags) {
    return arg0->__kgraphicsStatePreUnload__(pGpu, arg0, flags);
}

NV_STATUS kgraphicsStateUnload_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1, NvU32 flags);

static inline NV_STATUS kgraphicsStateUnload_DISPATCH(OBJGPU *arg0, struct KernelGraphics *arg1, NvU32 flags) {
    return arg1->__kgraphicsStateUnload__(arg0, arg1, flags);
}

void kgraphicsStateDestroy_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1);

static inline void kgraphicsStateDestroy_DISPATCH(OBJGPU *arg0, struct KernelGraphics *arg1) {
    arg1->__kgraphicsStateDestroy__(arg0, arg1);
}

NvBool kgraphicsIsPresent_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1);

static inline NvBool kgraphicsIsPresent_DISPATCH(OBJGPU *arg0, struct KernelGraphics *arg1) {
    return arg1->__kgraphicsIsPresent__(arg0, arg1);
}

NV_STATUS kgraphicsStatePostLoad_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1, NvU32 flags);

static inline NV_STATUS kgraphicsStatePostLoad_DISPATCH(OBJGPU *arg0, struct KernelGraphics *arg1, NvU32 flags) {
    return arg1->__kgraphicsStatePostLoad__(arg0, arg1, flags);
}

void kgraphicsRegisterIntrService_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1, IntrServiceRecord arg2[168]);

static inline void kgraphicsRegisterIntrService_DISPATCH(OBJGPU *arg0, struct KernelGraphics *arg1, IntrServiceRecord arg2[168]) {
    arg1->__kgraphicsRegisterIntrService__(arg0, arg1, arg2);
}

NV_STATUS kgraphicsServiceNotificationInterrupt_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1, IntrServiceServiceNotificationInterruptArguments *arg2);

static inline NV_STATUS kgraphicsServiceNotificationInterrupt_DISPATCH(OBJGPU *arg0, struct KernelGraphics *arg1, IntrServiceServiceNotificationInterruptArguments *arg2) {
    return arg1->__kgraphicsServiceNotificationInterrupt__(arg0, arg1, arg2);
}

NvBool kgraphicsClearInterrupt_GP100(OBJGPU *arg0, struct KernelGraphics *arg1, IntrServiceClearInterruptArguments *arg2);

static inline NvBool kgraphicsClearInterrupt_DISPATCH(OBJGPU *arg0, struct KernelGraphics *arg1, IntrServiceClearInterruptArguments *arg2) {
    return arg1->__kgraphicsClearInterrupt__(arg0, arg1, arg2);
}

NvU32 kgraphicsServiceInterrupt_GP100(OBJGPU *arg0, struct KernelGraphics *arg1, IntrServiceServiceInterruptArguments *arg2);

static inline NvU32 kgraphicsServiceInterrupt_DISPATCH(OBJGPU *arg0, struct KernelGraphics *arg1, IntrServiceServiceInterruptArguments *arg2) {
    return arg1->__kgraphicsServiceInterrupt__(arg0, arg1, arg2);
}

static inline NV_STATUS kgraphicsStatePreLoad_DISPATCH(POBJGPU pGpu, struct KernelGraphics *pEngstate, NvU32 arg0) {
    return pEngstate->__kgraphicsStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kgraphicsStatePostUnload_DISPATCH(POBJGPU pGpu, struct KernelGraphics *pEngstate, NvU32 arg0) {
    return pEngstate->__kgraphicsStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kgraphicsStateInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelGraphics *pEngstate) {
    return pEngstate->__kgraphicsStateInitUnlocked__(pGpu, pEngstate);
}

static inline void kgraphicsInitMissing_DISPATCH(POBJGPU pGpu, struct KernelGraphics *pEngstate) {
    pEngstate->__kgraphicsInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kgraphicsStatePreInitLocked_DISPATCH(POBJGPU pGpu, struct KernelGraphics *pEngstate) {
    return pEngstate->__kgraphicsStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kgraphicsStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelGraphics *pEngstate) {
    return pEngstate->__kgraphicsStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline KGRAPHICS_FECS_TRACE_INFO *kgraphicsGetFecsTraceInfo(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics) {
    struct KernelGraphics_PRIVATE *pKernelGraphics_PRIVATE = (struct KernelGraphics_PRIVATE *)pKernelGraphics;
    return pKernelGraphics_PRIVATE->pFecsTraceInfo;
}

static inline NvU32 kgraphicsGetInstance(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics) {
    struct KernelGraphics_PRIVATE *pKernelGraphics_PRIVATE = (struct KernelGraphics_PRIVATE *)pKernelGraphics;
    return pKernelGraphics_PRIVATE->instance;
}

static inline NvBool kgraphicsIsCtxswLoggingSupported(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics) {
    struct KernelGraphics_PRIVATE *pKernelGraphics_PRIVATE = (struct KernelGraphics_PRIVATE *)pKernelGraphics;
    return pKernelGraphics_PRIVATE->bCtxswLoggingSupported;
}

static inline void kgraphicsSetCtxswLoggingSupported(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics, NvBool bProp) {
    struct KernelGraphics_PRIVATE *pKernelGraphics_PRIVATE = (struct KernelGraphics_PRIVATE *)pKernelGraphics;
    pKernelGraphics_PRIVATE->bCtxswLoggingSupported = bProp;
}

static inline NvBool kgraphicsIsIntrDrivenCtxswLoggingEnabled(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics) {
    struct KernelGraphics_PRIVATE *pKernelGraphics_PRIVATE = (struct KernelGraphics_PRIVATE *)pKernelGraphics;
    return pKernelGraphics_PRIVATE->bIntrDrivenCtxswLoggingEnabled;
}

static inline void kgraphicsSetIntrDrivenCtxswLoggingEnabled(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics, NvBool bProp) {
    struct KernelGraphics_PRIVATE *pKernelGraphics_PRIVATE = (struct KernelGraphics_PRIVATE *)pKernelGraphics;
    pKernelGraphics_PRIVATE->bIntrDrivenCtxswLoggingEnabled = bProp;
}

static inline NvBool kgraphicsIsBottomHalfCtxswLoggingEnabled(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics) {
    struct KernelGraphics_PRIVATE *pKernelGraphics_PRIVATE = (struct KernelGraphics_PRIVATE *)pKernelGraphics;
    return pKernelGraphics_PRIVATE->bBottomHalfCtxswLoggingEnabled;
}

static inline void kgraphicsSetBottomHalfCtxswLoggingEnabled(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics, NvBool bProp) {
    struct KernelGraphics_PRIVATE *pKernelGraphics_PRIVATE = (struct KernelGraphics_PRIVATE *)pKernelGraphics;
    pKernelGraphics_PRIVATE->bBottomHalfCtxswLoggingEnabled = bProp;
}

static inline NvBool kgraphicsShouldDeferContextInit(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics) {
    struct KernelGraphics_PRIVATE *pKernelGraphics_PRIVATE = (struct KernelGraphics_PRIVATE *)pKernelGraphics;
    return pKernelGraphics_PRIVATE->bDeferContextInit;
}

static inline NvBool kgraphicsIsPerSubcontextContextHeaderSupported(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics) {
    struct KernelGraphics_PRIVATE *pKernelGraphics_PRIVATE = (struct KernelGraphics_PRIVATE *)pKernelGraphics;
    return pKernelGraphics_PRIVATE->bPerSubcontextContextHeaderSupported;
}

static inline void kgraphicsSetPerSubcontextContextHeaderSupported(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics, NvBool bProp) {
    struct KernelGraphics_PRIVATE *pKernelGraphics_PRIVATE = (struct KernelGraphics_PRIVATE *)pKernelGraphics;
    pKernelGraphics_PRIVATE->bPerSubcontextContextHeaderSupported = bProp;
}

static inline NvBool kgraphicsShouldSetContextBuffersGPUPrivileged(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics) {
    struct KernelGraphics_PRIVATE *pKernelGraphics_PRIVATE = (struct KernelGraphics_PRIVATE *)pKernelGraphics;
    return pKernelGraphics_PRIVATE->bSetContextBuffersGPUPrivileged;
}

static inline NvBool kgraphicsDoesUcodeSupportPrivAccessMap(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics) {
    struct KernelGraphics_PRIVATE *pKernelGraphics_PRIVATE = (struct KernelGraphics_PRIVATE *)pKernelGraphics;
    return pKernelGraphics_PRIVATE->bUcodeSupportsPrivAccessMap;
}

static inline NvBool kgraphicsIsRtvCbSupported(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics) {
    struct KernelGraphics_PRIVATE *pKernelGraphics_PRIVATE = (struct KernelGraphics_PRIVATE *)pKernelGraphics;
    return pKernelGraphics_PRIVATE->bRtvCbSupported;
}

static inline NvBool kgraphicsIsFecsRecordUcodeSeqnoSupported(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics) {
    struct KernelGraphics_PRIVATE *pKernelGraphics_PRIVATE = (struct KernelGraphics_PRIVATE *)pKernelGraphics;
    return pKernelGraphics_PRIVATE->bFecsRecordUcodeSeqnoSupported;
}

void kgraphicsDestruct_IMPL(struct KernelGraphics *arg0);

#define __nvoc_kgraphicsDestruct(arg0) kgraphicsDestruct_IMPL(arg0)
void kgraphicsInvalidateStaticInfo_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline void kgraphicsInvalidateStaticInfo(OBJGPU *arg0, struct KernelGraphics *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsInvalidateStaticInfo(arg0, arg1) kgraphicsInvalidateStaticInfo_IMPL(arg0, arg1)
#endif //__nvoc_kernel_graphics_h_disabled

const CTX_BUF_INFO *kgraphicsGetCtxBufferInfo_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1, GR_CTX_BUFFER arg2);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline const CTX_BUF_INFO *kgraphicsGetCtxBufferInfo(OBJGPU *arg0, struct KernelGraphics *arg1, GR_CTX_BUFFER arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NULL;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsGetCtxBufferInfo(arg0, arg1, arg2) kgraphicsGetCtxBufferInfo_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_graphics_h_disabled

void kgraphicsSetCtxBufferInfo_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1, GR_CTX_BUFFER arg2, NvU64 size, NvU64 align, RM_ATTR_PAGE_SIZE attr, NvBool bContiguous);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline void kgraphicsSetCtxBufferInfo(OBJGPU *arg0, struct KernelGraphics *arg1, GR_CTX_BUFFER arg2, NvU64 size, NvU64 align, RM_ATTR_PAGE_SIZE attr, NvBool bContiguous) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsSetCtxBufferInfo(arg0, arg1, arg2, size, align, attr, bContiguous) kgraphicsSetCtxBufferInfo_IMPL(arg0, arg1, arg2, size, align, attr, bContiguous)
#endif //__nvoc_kernel_graphics_h_disabled

void kgraphicsClearCtxBufferInfo_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline void kgraphicsClearCtxBufferInfo(OBJGPU *arg0, struct KernelGraphics *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsClearCtxBufferInfo(arg0, arg1) kgraphicsClearCtxBufferInfo_IMPL(arg0, arg1)
#endif //__nvoc_kernel_graphics_h_disabled

NV_STATUS kgraphicsInitCtxBufPool_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1, struct Heap *arg2);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline NV_STATUS kgraphicsInitCtxBufPool(OBJGPU *arg0, struct KernelGraphics *arg1, struct Heap *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsInitCtxBufPool(arg0, arg1, arg2) kgraphicsInitCtxBufPool_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_graphics_h_disabled

struct CTX_BUF_POOL_INFO *kgraphicsGetCtxBufPool_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline struct CTX_BUF_POOL_INFO *kgraphicsGetCtxBufPool(OBJGPU *arg0, struct KernelGraphics *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NULL;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsGetCtxBufPool(arg0, arg1) kgraphicsGetCtxBufPool_IMPL(arg0, arg1)
#endif //__nvoc_kernel_graphics_h_disabled

void kgraphicsDestroyCtxBufPool_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline void kgraphicsDestroyCtxBufPool(OBJGPU *arg0, struct KernelGraphics *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsDestroyCtxBufPool(arg0, arg1) kgraphicsDestroyCtxBufPool_IMPL(arg0, arg1)
#endif //__nvoc_kernel_graphics_h_disabled

GR_GLOBALCTX_BUFFERS *kgraphicsGetGlobalCtxBuffers_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1, NvU32 gfid);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline GR_GLOBALCTX_BUFFERS *kgraphicsGetGlobalCtxBuffers(OBJGPU *arg0, struct KernelGraphics *arg1, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NULL;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsGetGlobalCtxBuffers(arg0, arg1, gfid) kgraphicsGetGlobalCtxBuffers_IMPL(arg0, arg1, gfid)
#endif //__nvoc_kernel_graphics_h_disabled

NvBool kgraphicsIsGlobalCtxBufferSizeAligned_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1, GR_GLOBALCTX_BUFFER arg2);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline NvBool kgraphicsIsGlobalCtxBufferSizeAligned(OBJGPU *arg0, struct KernelGraphics *arg1, GR_GLOBALCTX_BUFFER arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsIsGlobalCtxBufferSizeAligned(arg0, arg1, arg2) kgraphicsIsGlobalCtxBufferSizeAligned_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_graphics_h_disabled

const GR_BUFFER_ATTR *kgraphicsGetGlobalPrivAccessMapAttr_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline const GR_BUFFER_ATTR *kgraphicsGetGlobalPrivAccessMapAttr(OBJGPU *arg0, struct KernelGraphics *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NULL;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsGetGlobalPrivAccessMapAttr(arg0, arg1) kgraphicsGetGlobalPrivAccessMapAttr_IMPL(arg0, arg1)
#endif //__nvoc_kernel_graphics_h_disabled

NV_STATUS kgraphicsMapCtxBuffer_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1, MEMORY_DESCRIPTOR *arg2, struct OBJVASPACE *arg3, VA_LIST *arg4, NvBool bAlignSize, NvBool bIsReadOnly);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline NV_STATUS kgraphicsMapCtxBuffer(OBJGPU *arg0, struct KernelGraphics *arg1, MEMORY_DESCRIPTOR *arg2, struct OBJVASPACE *arg3, VA_LIST *arg4, NvBool bAlignSize, NvBool bIsReadOnly) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsMapCtxBuffer(arg0, arg1, arg2, arg3, arg4, bAlignSize, bIsReadOnly) kgraphicsMapCtxBuffer_IMPL(arg0, arg1, arg2, arg3, arg4, bAlignSize, bIsReadOnly)
#endif //__nvoc_kernel_graphics_h_disabled

void kgraphicsUnmapCtxBuffer_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1, struct OBJVASPACE *arg2, VA_LIST *arg3);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline void kgraphicsUnmapCtxBuffer(OBJGPU *arg0, struct KernelGraphics *arg1, struct OBJVASPACE *arg2, VA_LIST *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsUnmapCtxBuffer(arg0, arg1, arg2, arg3) kgraphicsUnmapCtxBuffer_IMPL(arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_h_disabled

void kgraphicsFreeGlobalCtxBuffers_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1, NvU32 gfid);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline void kgraphicsFreeGlobalCtxBuffers(OBJGPU *arg0, struct KernelGraphics *arg1, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsFreeGlobalCtxBuffers(arg0, arg1, gfid) kgraphicsFreeGlobalCtxBuffers_IMPL(arg0, arg1, gfid)
#endif //__nvoc_kernel_graphics_h_disabled

NV_STATUS kgraphicsGetMainCtxBufferSize_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1, NvBool bIncludeSubctxHdrs, NvU32 *pSize);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline NV_STATUS kgraphicsGetMainCtxBufferSize(OBJGPU *arg0, struct KernelGraphics *arg1, NvBool bIncludeSubctxHdrs, NvU32 *pSize) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsGetMainCtxBufferSize(arg0, arg1, bIncludeSubctxHdrs, pSize) kgraphicsGetMainCtxBufferSize_IMPL(arg0, arg1, bIncludeSubctxHdrs, pSize)
#endif //__nvoc_kernel_graphics_h_disabled

NV_STATUS kgraphicsGetClassByType_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1, NvU32 objectType, NvU32 *pClass);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline NV_STATUS kgraphicsGetClassByType(OBJGPU *arg0, struct KernelGraphics *arg1, NvU32 objectType, NvU32 *pClass) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsGetClassByType(arg0, arg1, objectType, pClass) kgraphicsGetClassByType_IMPL(arg0, arg1, objectType, pClass)
#endif //__nvoc_kernel_graphics_h_disabled

const GR_BUFFER_ATTR *kgraphicsGetContextBufferAttr_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1, GR_CTX_BUFFER arg2);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline const GR_BUFFER_ATTR *kgraphicsGetContextBufferAttr(OBJGPU *arg0, struct KernelGraphics *arg1, GR_CTX_BUFFER arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NULL;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsGetContextBufferAttr(arg0, arg1, arg2) kgraphicsGetContextBufferAttr_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_graphics_h_disabled

NV_STATUS kgraphicsCreateGoldenImageChannel_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline NV_STATUS kgraphicsCreateGoldenImageChannel(OBJGPU *arg0, struct KernelGraphics *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsCreateGoldenImageChannel(arg0, arg1) kgraphicsCreateGoldenImageChannel_IMPL(arg0, arg1)
#endif //__nvoc_kernel_graphics_h_disabled

NvBool kgraphicsIsGFXSupported_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline NvBool kgraphicsIsGFXSupported(OBJGPU *arg0, struct KernelGraphics *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsIsGFXSupported(arg0, arg1) kgraphicsIsGFXSupported_IMPL(arg0, arg1)
#endif //__nvoc_kernel_graphics_h_disabled

NV_STATUS kgraphicsDiscoverMaxLocalCtxBufferSize_IMPL(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline NV_STATUS kgraphicsDiscoverMaxLocalCtxBufferSize(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsDiscoverMaxLocalCtxBufferSize(pGpu, pKernelGraphics) kgraphicsDiscoverMaxLocalCtxBufferSize_IMPL(pGpu, pKernelGraphics)
#endif //__nvoc_kernel_graphics_h_disabled

NV_STATUS kgraphicsInitializeDeferredStaticData_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1, NvHandle hClient, NvHandle hSubdevice);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline NV_STATUS kgraphicsInitializeDeferredStaticData(OBJGPU *arg0, struct KernelGraphics *arg1, NvHandle hClient, NvHandle hSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsInitializeDeferredStaticData(arg0, arg1, hClient, hSubdevice) kgraphicsInitializeDeferredStaticData_IMPL(arg0, arg1, hClient, hSubdevice)
#endif //__nvoc_kernel_graphics_h_disabled

const struct KGRAPHICS_STATIC_INFO *kgraphicsGetStaticInfo_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline const struct KGRAPHICS_STATIC_INFO *kgraphicsGetStaticInfo(OBJGPU *arg0, struct KernelGraphics *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NULL;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsGetStaticInfo(arg0, arg1) kgraphicsGetStaticInfo_IMPL(arg0, arg1)
#endif //__nvoc_kernel_graphics_h_disabled

NV_STATUS kgraphicsGetCaps_IMPL(OBJGPU *arg0, struct KernelGraphics *arg1, NvU8 *pGrCaps);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline NV_STATUS kgraphicsGetCaps(OBJGPU *arg0, struct KernelGraphics *arg1, NvU8 *pGrCaps) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsGetCaps(arg0, arg1, pGrCaps) kgraphicsGetCaps_IMPL(arg0, arg1, pGrCaps)
#endif //__nvoc_kernel_graphics_h_disabled

#undef PRIVATE_FIELD


#endif // _KERNEL_GRAPHICS_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_GRAPHICS_NVOC_H_
