
#ifndef _G_KERNEL_GRAPHICS_NVOC_H_
#define _G_KERNEL_GRAPHICS_NVOC_H_

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
typedef struct KGRAPHICS_BUG4208224_CONTEXT_INFO KGRAPHICS_BUG4208224_CONTEXT_INFO;

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

    // @ref NV2080_CTRL_CMD_GR_GET_SM_ISSUE_RATE_MODIFIER_V2
    NV2080_CTRL_INTERNAL_STATIC_GR_SM_ISSUE_RATE_MODIFIER_V2 *pSmIssueRateModifierV2;

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

#define KGRAPHICS_SCRUBBER_HANDLE_VAS        0xdada0042       
#define KGRAPHICS_SCRUBBER_HANDLE_PBVIRT     (KGRAPHICS_SCRUBBER_HANDLE_VAS + 1)      
#define KGRAPHICS_SCRUBBER_HANDLE_PBPHYS     (KGRAPHICS_SCRUBBER_HANDLE_VAS + 2)      
#define KGRAPHICS_SCRUBBER_HANDLE_CHANNEL    (KGRAPHICS_SCRUBBER_HANDLE_VAS + 3)      
#define KGRAPHICS_SCRUBBER_HANDLE_3DOBJ      (KGRAPHICS_SCRUBBER_HANDLE_VAS + 4)      
#define KGRAPHICS_SCRUBBER_HANDLE_USERD      (KGRAPHICS_SCRUBBER_HANDLE_VAS + 5)      

        
struct KGRAPHICS_BUG4208224_CONTEXT_INFO
{
    /* Dynamically allocated client handles */
    NvHandle hClient;
    NvHandle hDeviceId;
    NvHandle hSubdeviceId;

    // Have resources been setup
    NvBool bConstructed;
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

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_GRAPHICS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelGraphics;
struct NVOC_METADATA__OBJENGSTATE;
struct NVOC_METADATA__IntrService;
struct NVOC_VTABLE__KernelGraphics;


struct KernelGraphics {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelGraphics *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct IntrService __nvoc_base_IntrService;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct IntrService *__nvoc_pbase_IntrService;    // intrserv super
    struct KernelGraphics *__nvoc_pbase_KernelGraphics;    // kgraphics

    // Vtable with 10 per-object function pointers
    NV_STATUS (*__kgraphicsCreateBug4208224Channel__)(OBJGPU *, struct KernelGraphics * /*this*/);  // halified (2 hals)
    NV_STATUS (*__kgraphicsInitializeBug4208224WAR__)(OBJGPU *, struct KernelGraphics * /*this*/);  // halified (3 hals)
    NvBool (*__kgraphicsIsBug4208224WARNeeded__)(OBJGPU *, struct KernelGraphics * /*this*/);  // halified (3 hals)
    NV_STATUS (*__kgraphicsLoadStaticInfo__)(OBJGPU *, struct KernelGraphics * /*this*/, NvU32);  // halified (2 hals)
    NvBool (*__kgraphicsIsUnrestrictedAccessMapSupported__)(OBJGPU *, struct KernelGraphics * /*this*/);  // halified (2 hals)
    NvU32 (*__kgraphicsGetFecsTraceRdOffset__)(OBJGPU *, struct KernelGraphics * /*this*/);  // halified (2 hals)
    void (*__kgraphicsSetFecsTraceRdOffset__)(OBJGPU *, struct KernelGraphics * /*this*/, NvU32);  // halified (3 hals)
    void (*__kgraphicsSetFecsTraceWrOffset__)(OBJGPU *, struct KernelGraphics * /*this*/, NvU32);  // halified (3 hals)
    void (*__kgraphicsSetFecsTraceHwEnable__)(OBJGPU *, struct KernelGraphics * /*this*/, NvBool);  // halified (3 hals)
    NvBool (*__kgraphicsIsCtxswLoggingEnabled__)(OBJGPU *, struct KernelGraphics * /*this*/);  // halified (2 hals) body

    // Data members
    NvBool PRIVATE_FIELD(bCtxswLoggingSupported);
    NvBool PRIVATE_FIELD(bCtxswLoggingEnabled);
    NvBool PRIVATE_FIELD(bIntrDrivenCtxswLoggingEnabled);
    NvBool PRIVATE_FIELD(bBottomHalfCtxswLoggingEnabled);
    NvBool PRIVATE_FIELD(bOverrideContextBuffersToGpuCached);
    NvBool PRIVATE_FIELD(bPeFiroBufferEnabled);
    NvBool PRIVATE_FIELD(bDeferContextInit);
    NvBool PRIVATE_FIELD(bPerSubcontextContextHeaderSupported);
    NvBool PRIVATE_FIELD(bSetContextBuffersGPUPrivileged);
    NvBool PRIVATE_FIELD(bUcodeSupportsPrivAccessMap);
    NvBool PRIVATE_FIELD(bRtvCbSupported);
    NvBool PRIVATE_FIELD(bFecsRecordUcodeSeqnoSupported);
    NvBool PRIVATE_FIELD(bBug4208224WAREnabled);
    NvU32 PRIVATE_FIELD(instance);
    KGRAPHICS_PRIVATE_DATA *PRIVATE_FIELD(pPrivate);
    NvBool PRIVATE_FIELD(bCollectingDeferredStaticData);
    KGRAPHICS_FECS_TRACE_INFO *PRIVATE_FIELD(pFecsTraceInfo);
    struct KGRAPHICS_GLOBAL_CTX_BUFFERS_INFO PRIVATE_FIELD(globalCtxBuffersInfo);
    struct CTX_BUF_POOL_INFO *PRIVATE_FIELD(pCtxBufPool);
    CTX_BUF_INFO PRIVATE_FIELD(maxCtxBufSize)[10];
    GR_BUFFER_ATTR PRIVATE_FIELD(ctxAttr)[10];
    struct KGRAPHICS_BUG4208224_CONTEXT_INFO PRIVATE_FIELD(bug4208224Info);
};


struct KernelGraphics_PRIVATE {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelGraphics *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct IntrService __nvoc_base_IntrService;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct IntrService *__nvoc_pbase_IntrService;    // intrserv super
    struct KernelGraphics *__nvoc_pbase_KernelGraphics;    // kgraphics

    // Vtable with 10 per-object function pointers
    NV_STATUS (*__kgraphicsCreateBug4208224Channel__)(OBJGPU *, struct KernelGraphics * /*this*/);  // halified (2 hals)
    NV_STATUS (*__kgraphicsInitializeBug4208224WAR__)(OBJGPU *, struct KernelGraphics * /*this*/);  // halified (3 hals)
    NvBool (*__kgraphicsIsBug4208224WARNeeded__)(OBJGPU *, struct KernelGraphics * /*this*/);  // halified (3 hals)
    NV_STATUS (*__kgraphicsLoadStaticInfo__)(OBJGPU *, struct KernelGraphics * /*this*/, NvU32);  // halified (2 hals)
    NvBool (*__kgraphicsIsUnrestrictedAccessMapSupported__)(OBJGPU *, struct KernelGraphics * /*this*/);  // halified (2 hals)
    NvU32 (*__kgraphicsGetFecsTraceRdOffset__)(OBJGPU *, struct KernelGraphics * /*this*/);  // halified (2 hals)
    void (*__kgraphicsSetFecsTraceRdOffset__)(OBJGPU *, struct KernelGraphics * /*this*/, NvU32);  // halified (3 hals)
    void (*__kgraphicsSetFecsTraceWrOffset__)(OBJGPU *, struct KernelGraphics * /*this*/, NvU32);  // halified (3 hals)
    void (*__kgraphicsSetFecsTraceHwEnable__)(OBJGPU *, struct KernelGraphics * /*this*/, NvBool);  // halified (3 hals)
    NvBool (*__kgraphicsIsCtxswLoggingEnabled__)(OBJGPU *, struct KernelGraphics * /*this*/);  // halified (2 hals) body

    // Data members
    NvBool bCtxswLoggingSupported;
    NvBool bCtxswLoggingEnabled;
    NvBool bIntrDrivenCtxswLoggingEnabled;
    NvBool bBottomHalfCtxswLoggingEnabled;
    NvBool bOverrideContextBuffersToGpuCached;
    NvBool bPeFiroBufferEnabled;
    NvBool bDeferContextInit;
    NvBool bPerSubcontextContextHeaderSupported;
    NvBool bSetContextBuffersGPUPrivileged;
    NvBool bUcodeSupportsPrivAccessMap;
    NvBool bRtvCbSupported;
    NvBool bFecsRecordUcodeSeqnoSupported;
    NvBool bBug4208224WAREnabled;
    NvU32 instance;
    KGRAPHICS_PRIVATE_DATA *pPrivate;
    NvBool bCollectingDeferredStaticData;
    KGRAPHICS_FECS_TRACE_INFO *pFecsTraceInfo;
    struct KGRAPHICS_GLOBAL_CTX_BUFFERS_INFO globalCtxBuffersInfo;
    struct CTX_BUF_POOL_INFO *pCtxBufPool;
    CTX_BUF_INFO maxCtxBufSize[10];
    GR_BUFFER_ATTR ctxAttr[10];
    struct KGRAPHICS_BUG4208224_CONTEXT_INFO bug4208224Info;
};


// Vtable with 18 per-class function pointers
struct NVOC_VTABLE__KernelGraphics {
    NV_STATUS (*__kgraphicsConstructEngine__)(OBJGPU *, struct KernelGraphics * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kgraphicsStateInitLocked__)(OBJGPU *, struct KernelGraphics * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kgraphicsStateLoad__)(OBJGPU *, struct KernelGraphics * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kgraphicsStatePreUnload__)(OBJGPU *, struct KernelGraphics * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kgraphicsStateUnload__)(OBJGPU *, struct KernelGraphics * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    void (*__kgraphicsStateDestroy__)(OBJGPU *, struct KernelGraphics * /*this*/);  // virtual override (engstate) base (engstate)
    NvBool (*__kgraphicsIsPresent__)(OBJGPU *, struct KernelGraphics * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kgraphicsStatePostLoad__)(OBJGPU *, struct KernelGraphics * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    void (*__kgraphicsRegisterIntrService__)(OBJGPU *, struct KernelGraphics * /*this*/, IntrServiceRecord *);  // virtual override (intrserv) base (intrserv)
    NV_STATUS (*__kgraphicsServiceNotificationInterrupt__)(OBJGPU *, struct KernelGraphics * /*this*/, IntrServiceServiceNotificationInterruptArguments *);  // virtual override (intrserv) base (intrserv)
    NvBool (*__kgraphicsClearInterrupt__)(OBJGPU *, struct KernelGraphics * /*this*/, IntrServiceClearInterruptArguments *);  // virtual halified (singleton optimized) override (intrserv) base (intrserv)
    NvU32 (*__kgraphicsServiceInterrupt__)(OBJGPU *, struct KernelGraphics * /*this*/, IntrServiceServiceInterruptArguments *);  // virtual halified (singleton optimized) override (intrserv) base (intrserv)
    void (*__kgraphicsInitMissing__)(struct OBJGPU *, struct KernelGraphics * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgraphicsStatePreInitLocked__)(struct OBJGPU *, struct KernelGraphics * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgraphicsStatePreInitUnlocked__)(struct OBJGPU *, struct KernelGraphics * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgraphicsStateInitUnlocked__)(struct OBJGPU *, struct KernelGraphics * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgraphicsStatePreLoad__)(struct OBJGPU *, struct KernelGraphics * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgraphicsStatePostUnload__)(struct OBJGPU *, struct KernelGraphics * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelGraphics {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
    const struct NVOC_METADATA__IntrService metadata__IntrService;
    const struct NVOC_VTABLE__KernelGraphics vtable;
};

#ifndef __NVOC_CLASS_KernelGraphics_TYPEDEF__
#define __NVOC_CLASS_KernelGraphics_TYPEDEF__
typedef struct KernelGraphics KernelGraphics;
#endif /* __NVOC_CLASS_KernelGraphics_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGraphics
#define __nvoc_class_id_KernelGraphics 0xea3fa9
#endif /* __nvoc_class_id_KernelGraphics */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGraphics;

#define __staticCast_KernelGraphics(pThis) \
    ((pThis)->__nvoc_pbase_KernelGraphics)

#ifdef __nvoc_kernel_graphics_h_disabled
#define __dynamicCast_KernelGraphics(pThis) ((KernelGraphics*) NULL)
#else //__nvoc_kernel_graphics_h_disabled
#define __dynamicCast_KernelGraphics(pThis) \
    ((KernelGraphics*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelGraphics)))
#endif //__nvoc_kernel_graphics_h_disabled

// Property macros
#define PDB_PROP_KGRAPHICS_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KGRAPHICS_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelGraphics(KernelGraphics**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelGraphics(KernelGraphics**, Dynamic*, NvU32);
#define __objCreate_KernelGraphics(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelGraphics((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define kgraphicsConstructEngine_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__kgraphicsConstructEngine__
#define kgraphicsConstructEngine(arg1, arg_this, arg3) kgraphicsConstructEngine_DISPATCH(arg1, arg_this, arg3)
#define kgraphicsStateInitLocked_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__kgraphicsStateInitLocked__
#define kgraphicsStateInitLocked(arg1, arg_this) kgraphicsStateInitLocked_DISPATCH(arg1, arg_this)
#define kgraphicsStateLoad_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__kgraphicsStateLoad__
#define kgraphicsStateLoad(arg1, arg_this, flags) kgraphicsStateLoad_DISPATCH(arg1, arg_this, flags)
#define kgraphicsStatePreUnload_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__kgraphicsStatePreUnload__
#define kgraphicsStatePreUnload(pGpu, arg_this, flags) kgraphicsStatePreUnload_DISPATCH(pGpu, arg_this, flags)
#define kgraphicsStateUnload_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__kgraphicsStateUnload__
#define kgraphicsStateUnload(arg1, arg_this, flags) kgraphicsStateUnload_DISPATCH(arg1, arg_this, flags)
#define kgraphicsStateDestroy_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__kgraphicsStateDestroy__
#define kgraphicsStateDestroy(arg1, arg_this) kgraphicsStateDestroy_DISPATCH(arg1, arg_this)
#define kgraphicsIsPresent_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__kgraphicsIsPresent__
#define kgraphicsIsPresent(arg1, arg_this) kgraphicsIsPresent_DISPATCH(arg1, arg_this)
#define kgraphicsStatePostLoad_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__kgraphicsStatePostLoad__
#define kgraphicsStatePostLoad(arg1, arg_this, flags) kgraphicsStatePostLoad_DISPATCH(arg1, arg_this, flags)
#define kgraphicsRegisterIntrService_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__kgraphicsRegisterIntrService__
#define kgraphicsRegisterIntrService(arg1, arg_this, arg3) kgraphicsRegisterIntrService_DISPATCH(arg1, arg_this, arg3)
#define kgraphicsServiceNotificationInterrupt_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__kgraphicsServiceNotificationInterrupt__
#define kgraphicsServiceNotificationInterrupt(arg1, arg_this, arg3) kgraphicsServiceNotificationInterrupt_DISPATCH(arg1, arg_this, arg3)
#define kgraphicsCreateBug4208224Channel_FNPTR(arg_this) arg_this->__kgraphicsCreateBug4208224Channel__
#define kgraphicsCreateBug4208224Channel(arg1, arg_this) kgraphicsCreateBug4208224Channel_DISPATCH(arg1, arg_this)
#define kgraphicsCreateBug4208224Channel_HAL(arg1, arg_this) kgraphicsCreateBug4208224Channel_DISPATCH(arg1, arg_this)
#define kgraphicsInitializeBug4208224WAR_FNPTR(arg_this) arg_this->__kgraphicsInitializeBug4208224WAR__
#define kgraphicsInitializeBug4208224WAR(arg1, arg_this) kgraphicsInitializeBug4208224WAR_DISPATCH(arg1, arg_this)
#define kgraphicsInitializeBug4208224WAR_HAL(arg1, arg_this) kgraphicsInitializeBug4208224WAR_DISPATCH(arg1, arg_this)
#define kgraphicsIsBug4208224WARNeeded_FNPTR(arg_this) arg_this->__kgraphicsIsBug4208224WARNeeded__
#define kgraphicsIsBug4208224WARNeeded(arg1, arg_this) kgraphicsIsBug4208224WARNeeded_DISPATCH(arg1, arg_this)
#define kgraphicsIsBug4208224WARNeeded_HAL(arg1, arg_this) kgraphicsIsBug4208224WARNeeded_DISPATCH(arg1, arg_this)
#define kgraphicsLoadStaticInfo_FNPTR(arg_this) arg_this->__kgraphicsLoadStaticInfo__
#define kgraphicsLoadStaticInfo(arg1, arg_this, swizzId) kgraphicsLoadStaticInfo_DISPATCH(arg1, arg_this, swizzId)
#define kgraphicsLoadStaticInfo_HAL(arg1, arg_this, swizzId) kgraphicsLoadStaticInfo_DISPATCH(arg1, arg_this, swizzId)
#define kgraphicsClearInterrupt_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__kgraphicsClearInterrupt__
#define kgraphicsClearInterrupt(arg1, arg_this, arg3) kgraphicsClearInterrupt_DISPATCH(arg1, arg_this, arg3)
#define kgraphicsClearInterrupt_HAL(arg1, arg_this, arg3) kgraphicsClearInterrupt_DISPATCH(arg1, arg_this, arg3)
#define kgraphicsServiceInterrupt_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__kgraphicsServiceInterrupt__
#define kgraphicsServiceInterrupt(arg1, arg_this, arg3) kgraphicsServiceInterrupt_DISPATCH(arg1, arg_this, arg3)
#define kgraphicsServiceInterrupt_HAL(arg1, arg_this, arg3) kgraphicsServiceInterrupt_DISPATCH(arg1, arg_this, arg3)
#define kgraphicsIsUnrestrictedAccessMapSupported_FNPTR(arg_this) arg_this->__kgraphicsIsUnrestrictedAccessMapSupported__
#define kgraphicsIsUnrestrictedAccessMapSupported(arg1, arg_this) kgraphicsIsUnrestrictedAccessMapSupported_DISPATCH(arg1, arg_this)
#define kgraphicsIsUnrestrictedAccessMapSupported_HAL(arg1, arg_this) kgraphicsIsUnrestrictedAccessMapSupported_DISPATCH(arg1, arg_this)
#define kgraphicsGetFecsTraceRdOffset_FNPTR(arg_this) arg_this->__kgraphicsGetFecsTraceRdOffset__
#define kgraphicsGetFecsTraceRdOffset(arg1, arg_this) kgraphicsGetFecsTraceRdOffset_DISPATCH(arg1, arg_this)
#define kgraphicsGetFecsTraceRdOffset_HAL(arg1, arg_this) kgraphicsGetFecsTraceRdOffset_DISPATCH(arg1, arg_this)
#define kgraphicsSetFecsTraceRdOffset_FNPTR(arg_this) arg_this->__kgraphicsSetFecsTraceRdOffset__
#define kgraphicsSetFecsTraceRdOffset(arg1, arg_this, rdOffset) kgraphicsSetFecsTraceRdOffset_DISPATCH(arg1, arg_this, rdOffset)
#define kgraphicsSetFecsTraceRdOffset_HAL(arg1, arg_this, rdOffset) kgraphicsSetFecsTraceRdOffset_DISPATCH(arg1, arg_this, rdOffset)
#define kgraphicsSetFecsTraceWrOffset_FNPTR(arg_this) arg_this->__kgraphicsSetFecsTraceWrOffset__
#define kgraphicsSetFecsTraceWrOffset(arg1, arg_this, wrOffset) kgraphicsSetFecsTraceWrOffset_DISPATCH(arg1, arg_this, wrOffset)
#define kgraphicsSetFecsTraceWrOffset_HAL(arg1, arg_this, wrOffset) kgraphicsSetFecsTraceWrOffset_DISPATCH(arg1, arg_this, wrOffset)
#define kgraphicsSetFecsTraceHwEnable_FNPTR(arg_this) arg_this->__kgraphicsSetFecsTraceHwEnable__
#define kgraphicsSetFecsTraceHwEnable(arg1, arg_this, bEnable) kgraphicsSetFecsTraceHwEnable_DISPATCH(arg1, arg_this, bEnable)
#define kgraphicsSetFecsTraceHwEnable_HAL(arg1, arg_this, bEnable) kgraphicsSetFecsTraceHwEnable_DISPATCH(arg1, arg_this, bEnable)
#define kgraphicsIsCtxswLoggingEnabled_FNPTR(pKernelGraphics) pKernelGraphics->__kgraphicsIsCtxswLoggingEnabled__
#define kgraphicsIsCtxswLoggingEnabled(pGpu, pKernelGraphics) kgraphicsIsCtxswLoggingEnabled_DISPATCH(pGpu, pKernelGraphics)
#define kgraphicsIsCtxswLoggingEnabled_HAL(pGpu, pKernelGraphics) kgraphicsIsCtxswLoggingEnabled_DISPATCH(pGpu, pKernelGraphics)
#define kgraphicsInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define kgraphicsInitMissing(pGpu, pEngstate) kgraphicsInitMissing_DISPATCH(pGpu, pEngstate)
#define kgraphicsStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__
#define kgraphicsStatePreInitLocked(pGpu, pEngstate) kgraphicsStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kgraphicsStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define kgraphicsStatePreInitUnlocked(pGpu, pEngstate) kgraphicsStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kgraphicsStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__
#define kgraphicsStateInitUnlocked(pGpu, pEngstate) kgraphicsStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kgraphicsStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__
#define kgraphicsStatePreLoad(pGpu, pEngstate, arg3) kgraphicsStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kgraphicsStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__
#define kgraphicsStatePostUnload(pGpu, pEngstate, arg3) kgraphicsStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)

// Dispatch functions
static inline NV_STATUS kgraphicsConstructEngine_DISPATCH(OBJGPU *arg1, struct KernelGraphics *arg_this, ENGDESCRIPTOR arg3) {
    return arg_this->__nvoc_metadata_ptr->vtable.__kgraphicsConstructEngine__(arg1, arg_this, arg3);
}

static inline NV_STATUS kgraphicsStateInitLocked_DISPATCH(OBJGPU *arg1, struct KernelGraphics *arg_this) {
    return arg_this->__nvoc_metadata_ptr->vtable.__kgraphicsStateInitLocked__(arg1, arg_this);
}

static inline NV_STATUS kgraphicsStateLoad_DISPATCH(OBJGPU *arg1, struct KernelGraphics *arg_this, NvU32 flags) {
    return arg_this->__nvoc_metadata_ptr->vtable.__kgraphicsStateLoad__(arg1, arg_this, flags);
}

static inline NV_STATUS kgraphicsStatePreUnload_DISPATCH(OBJGPU *pGpu, struct KernelGraphics *arg_this, NvU32 flags) {
    return arg_this->__nvoc_metadata_ptr->vtable.__kgraphicsStatePreUnload__(pGpu, arg_this, flags);
}

static inline NV_STATUS kgraphicsStateUnload_DISPATCH(OBJGPU *arg1, struct KernelGraphics *arg_this, NvU32 flags) {
    return arg_this->__nvoc_metadata_ptr->vtable.__kgraphicsStateUnload__(arg1, arg_this, flags);
}

static inline void kgraphicsStateDestroy_DISPATCH(OBJGPU *arg1, struct KernelGraphics *arg_this) {
    arg_this->__nvoc_metadata_ptr->vtable.__kgraphicsStateDestroy__(arg1, arg_this);
}

static inline NvBool kgraphicsIsPresent_DISPATCH(OBJGPU *arg1, struct KernelGraphics *arg_this) {
    return arg_this->__nvoc_metadata_ptr->vtable.__kgraphicsIsPresent__(arg1, arg_this);
}

static inline NV_STATUS kgraphicsStatePostLoad_DISPATCH(OBJGPU *arg1, struct KernelGraphics *arg_this, NvU32 flags) {
    return arg_this->__nvoc_metadata_ptr->vtable.__kgraphicsStatePostLoad__(arg1, arg_this, flags);
}

static inline void kgraphicsRegisterIntrService_DISPATCH(OBJGPU *arg1, struct KernelGraphics *arg_this, IntrServiceRecord arg3[179]) {
    arg_this->__nvoc_metadata_ptr->vtable.__kgraphicsRegisterIntrService__(arg1, arg_this, arg3);
}

static inline NV_STATUS kgraphicsServiceNotificationInterrupt_DISPATCH(OBJGPU *arg1, struct KernelGraphics *arg_this, IntrServiceServiceNotificationInterruptArguments *arg3) {
    return arg_this->__nvoc_metadata_ptr->vtable.__kgraphicsServiceNotificationInterrupt__(arg1, arg_this, arg3);
}

static inline NV_STATUS kgraphicsCreateBug4208224Channel_DISPATCH(OBJGPU *arg1, struct KernelGraphics *arg_this) {
    return arg_this->__kgraphicsCreateBug4208224Channel__(arg1, arg_this);
}

static inline NV_STATUS kgraphicsInitializeBug4208224WAR_DISPATCH(OBJGPU *arg1, struct KernelGraphics *arg_this) {
    return arg_this->__kgraphicsInitializeBug4208224WAR__(arg1, arg_this);
}

static inline NvBool kgraphicsIsBug4208224WARNeeded_DISPATCH(OBJGPU *arg1, struct KernelGraphics *arg_this) {
    return arg_this->__kgraphicsIsBug4208224WARNeeded__(arg1, arg_this);
}

static inline NV_STATUS kgraphicsLoadStaticInfo_DISPATCH(OBJGPU *arg1, struct KernelGraphics *arg_this, NvU32 swizzId) {
    return arg_this->__kgraphicsLoadStaticInfo__(arg1, arg_this, swizzId);
}

static inline NvBool kgraphicsClearInterrupt_DISPATCH(OBJGPU *arg1, struct KernelGraphics *arg_this, IntrServiceClearInterruptArguments *arg3) {
    return arg_this->__nvoc_metadata_ptr->vtable.__kgraphicsClearInterrupt__(arg1, arg_this, arg3);
}

static inline NvU32 kgraphicsServiceInterrupt_DISPATCH(OBJGPU *arg1, struct KernelGraphics *arg_this, IntrServiceServiceInterruptArguments *arg3) {
    return arg_this->__nvoc_metadata_ptr->vtable.__kgraphicsServiceInterrupt__(arg1, arg_this, arg3);
}

static inline NvBool kgraphicsIsUnrestrictedAccessMapSupported_DISPATCH(OBJGPU *arg1, struct KernelGraphics *arg_this) {
    return arg_this->__kgraphicsIsUnrestrictedAccessMapSupported__(arg1, arg_this);
}

static inline NvU32 kgraphicsGetFecsTraceRdOffset_DISPATCH(OBJGPU *arg1, struct KernelGraphics *arg_this) {
    return arg_this->__kgraphicsGetFecsTraceRdOffset__(arg1, arg_this);
}

static inline void kgraphicsSetFecsTraceRdOffset_DISPATCH(OBJGPU *arg1, struct KernelGraphics *arg_this, NvU32 rdOffset) {
    arg_this->__kgraphicsSetFecsTraceRdOffset__(arg1, arg_this, rdOffset);
}

static inline void kgraphicsSetFecsTraceWrOffset_DISPATCH(OBJGPU *arg1, struct KernelGraphics *arg_this, NvU32 wrOffset) {
    arg_this->__kgraphicsSetFecsTraceWrOffset__(arg1, arg_this, wrOffset);
}

static inline void kgraphicsSetFecsTraceHwEnable_DISPATCH(OBJGPU *arg1, struct KernelGraphics *arg_this, NvBool bEnable) {
    arg_this->__kgraphicsSetFecsTraceHwEnable__(arg1, arg_this, bEnable);
}

static inline NvBool kgraphicsIsCtxswLoggingEnabled_DISPATCH(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics) {
    return pKernelGraphics->__kgraphicsIsCtxswLoggingEnabled__(pGpu, pKernelGraphics);
}

static inline void kgraphicsInitMissing_DISPATCH(struct OBJGPU *pGpu, struct KernelGraphics *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__kgraphicsInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kgraphicsStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelGraphics *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgraphicsStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kgraphicsStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelGraphics *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgraphicsStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kgraphicsStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelGraphics *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgraphicsStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kgraphicsStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelGraphics *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgraphicsStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kgraphicsStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelGraphics *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgraphicsStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline NvBool kgraphicsShouldForceMainCtxContiguity_88bc07(OBJGPU *arg1, struct KernelGraphics *arg2) {
    return NV_TRUE;
}


#ifdef __nvoc_kernel_graphics_h_disabled
static inline NvBool kgraphicsShouldForceMainCtxContiguity(OBJGPU *arg1, struct KernelGraphics *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsShouldForceMainCtxContiguity(arg1, arg2) kgraphicsShouldForceMainCtxContiguity_88bc07(arg1, arg2)
#endif //__nvoc_kernel_graphics_h_disabled

#define kgraphicsShouldForceMainCtxContiguity_HAL(arg1, arg2) kgraphicsShouldForceMainCtxContiguity(arg1, arg2)

NV_STATUS kgraphicsAllocKgraphicsBuffers_KERNEL(OBJGPU *arg1, struct KernelGraphics *arg2, struct KernelGraphicsContext *arg3, struct KernelChannel *arg4);


#ifdef __nvoc_kernel_graphics_h_disabled
static inline NV_STATUS kgraphicsAllocKgraphicsBuffers(OBJGPU *arg1, struct KernelGraphics *arg2, struct KernelGraphicsContext *arg3, struct KernelChannel *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsAllocKgraphicsBuffers(arg1, arg2, arg3, arg4) kgraphicsAllocKgraphicsBuffers_KERNEL(arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_graphics_h_disabled

#define kgraphicsAllocKgraphicsBuffers_HAL(arg1, arg2, arg3, arg4) kgraphicsAllocKgraphicsBuffers(arg1, arg2, arg3, arg4)

NV_STATUS kgraphicsAllocGrGlobalCtxBuffers_TU102(OBJGPU *arg1, struct KernelGraphics *arg2, NvU32 gfid, struct KernelGraphicsContext *arg4);


#ifdef __nvoc_kernel_graphics_h_disabled
static inline NV_STATUS kgraphicsAllocGrGlobalCtxBuffers(OBJGPU *arg1, struct KernelGraphics *arg2, NvU32 gfid, struct KernelGraphicsContext *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsAllocGrGlobalCtxBuffers(arg1, arg2, gfid, arg4) kgraphicsAllocGrGlobalCtxBuffers_TU102(arg1, arg2, gfid, arg4)
#endif //__nvoc_kernel_graphics_h_disabled

#define kgraphicsAllocGrGlobalCtxBuffers_HAL(arg1, arg2, gfid, arg4) kgraphicsAllocGrGlobalCtxBuffers(arg1, arg2, gfid, arg4)

NV_STATUS kgraphicsAllocGlobalCtxBuffers_GP100(OBJGPU *arg1, struct KernelGraphics *arg2, NvU32 gfid);


#ifdef __nvoc_kernel_graphics_h_disabled
static inline NV_STATUS kgraphicsAllocGlobalCtxBuffers(OBJGPU *arg1, struct KernelGraphics *arg2, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsAllocGlobalCtxBuffers(arg1, arg2, gfid) kgraphicsAllocGlobalCtxBuffers_GP100(arg1, arg2, gfid)
#endif //__nvoc_kernel_graphics_h_disabled

#define kgraphicsAllocGlobalCtxBuffers_HAL(arg1, arg2, gfid) kgraphicsAllocGlobalCtxBuffers(arg1, arg2, gfid)

static inline void kgraphicsNonstallIntrCheckAndClear_b3696a(OBJGPU *arg1, struct KernelGraphics *arg2, struct THREAD_STATE_NODE *arg3) {
    return;
}


#ifdef __nvoc_kernel_graphics_h_disabled
static inline void kgraphicsNonstallIntrCheckAndClear(OBJGPU *arg1, struct KernelGraphics *arg2, struct THREAD_STATE_NODE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsNonstallIntrCheckAndClear(arg1, arg2, arg3) kgraphicsNonstallIntrCheckAndClear_b3696a(arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_h_disabled

#define kgraphicsNonstallIntrCheckAndClear_HAL(arg1, arg2, arg3) kgraphicsNonstallIntrCheckAndClear(arg1, arg2, arg3)

void kgraphicsInitFecsRegistryOverrides_GP100(OBJGPU *arg1, struct KernelGraphics *arg2);


#ifdef __nvoc_kernel_graphics_h_disabled
static inline void kgraphicsInitFecsRegistryOverrides(OBJGPU *arg1, struct KernelGraphics *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsInitFecsRegistryOverrides(arg1, arg2) kgraphicsInitFecsRegistryOverrides_GP100(arg1, arg2)
#endif //__nvoc_kernel_graphics_h_disabled

#define kgraphicsInitFecsRegistryOverrides_HAL(arg1, arg2) kgraphicsInitFecsRegistryOverrides(arg1, arg2)

static inline void kgraphicsSetCtxswLoggingEnabled_fdfbe2(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics, NvBool bEnabled) {
    struct KernelGraphics_PRIVATE *pKernelGraphics_PRIVATE = (struct KernelGraphics_PRIVATE *)pKernelGraphics;
    pKernelGraphics_PRIVATE->bCtxswLoggingEnabled = bEnabled;
}


#ifdef __nvoc_kernel_graphics_h_disabled
static inline void kgraphicsSetCtxswLoggingEnabled(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics, NvBool bEnabled) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsSetCtxswLoggingEnabled(pGpu, pKernelGraphics, bEnabled) kgraphicsSetCtxswLoggingEnabled_fdfbe2(pGpu, pKernelGraphics, bEnabled)
#endif //__nvoc_kernel_graphics_h_disabled

#define kgraphicsSetCtxswLoggingEnabled_HAL(pGpu, pKernelGraphics, bEnabled) kgraphicsSetCtxswLoggingEnabled(pGpu, pKernelGraphics, bEnabled)

NV_STATUS kgraphicsConstructEngine_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2, ENGDESCRIPTOR arg3);

NV_STATUS kgraphicsStateInitLocked_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2);

NV_STATUS kgraphicsStateLoad_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2, NvU32 flags);

NV_STATUS kgraphicsStatePreUnload_IMPL(OBJGPU *pGpu, struct KernelGraphics *arg2, NvU32 flags);

NV_STATUS kgraphicsStateUnload_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2, NvU32 flags);

void kgraphicsStateDestroy_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2);

NvBool kgraphicsIsPresent_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2);

NV_STATUS kgraphicsStatePostLoad_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2, NvU32 flags);

void kgraphicsRegisterIntrService_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2, IntrServiceRecord arg3[179]);

NV_STATUS kgraphicsServiceNotificationInterrupt_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2, IntrServiceServiceNotificationInterruptArguments *arg3);

NV_STATUS kgraphicsCreateBug4208224Channel_TU102(OBJGPU *arg1, struct KernelGraphics *arg2);

static inline NV_STATUS kgraphicsCreateBug4208224Channel_56cd7a(OBJGPU *arg1, struct KernelGraphics *arg2) {
    return NV_OK;
}

static inline NV_STATUS kgraphicsInitializeBug4208224WAR_56cd7a(OBJGPU *arg1, struct KernelGraphics *arg2) {
    return NV_OK;
}

NV_STATUS kgraphicsInitializeBug4208224WAR_TU102(OBJGPU *arg1, struct KernelGraphics *arg2);

static inline NvBool kgraphicsIsBug4208224WARNeeded_3dd2c9(OBJGPU *arg1, struct KernelGraphics *arg2) {
    return NV_FALSE;
}

NvBool kgraphicsIsBug4208224WARNeeded_TU102(OBJGPU *arg1, struct KernelGraphics *arg2);

NV_STATUS kgraphicsLoadStaticInfo_VF(OBJGPU *arg1, struct KernelGraphics *arg2, NvU32 swizzId);

NV_STATUS kgraphicsLoadStaticInfo_KERNEL(OBJGPU *arg1, struct KernelGraphics *arg2, NvU32 swizzId);

NvBool kgraphicsClearInterrupt_GP100(OBJGPU *arg1, struct KernelGraphics *arg2, IntrServiceClearInterruptArguments *arg3);

NvU32 kgraphicsServiceInterrupt_GP100(OBJGPU *arg1, struct KernelGraphics *arg2, IntrServiceServiceInterruptArguments *arg3);

static inline NvBool kgraphicsIsUnrestrictedAccessMapSupported_3dd2c9(OBJGPU *arg1, struct KernelGraphics *arg2) {
    return NV_FALSE;
}

NvBool kgraphicsIsUnrestrictedAccessMapSupported_PF(OBJGPU *arg1, struct KernelGraphics *arg2);

static inline NvU32 kgraphicsGetFecsTraceRdOffset_474d46(OBJGPU *arg1, struct KernelGraphics *arg2) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

NvU32 kgraphicsGetFecsTraceRdOffset_GA100(OBJGPU *arg1, struct KernelGraphics *arg2);

void kgraphicsSetFecsTraceRdOffset_GP100(OBJGPU *arg1, struct KernelGraphics *arg2, NvU32 rdOffset);

void kgraphicsSetFecsTraceRdOffset_GA100(OBJGPU *arg1, struct KernelGraphics *arg2, NvU32 rdOffset);

void kgraphicsSetFecsTraceRdOffset_GB100(OBJGPU *arg1, struct KernelGraphics *arg2, NvU32 rdOffset);

void kgraphicsSetFecsTraceWrOffset_GP100(OBJGPU *arg1, struct KernelGraphics *arg2, NvU32 wrOffset);

void kgraphicsSetFecsTraceWrOffset_GA100(OBJGPU *arg1, struct KernelGraphics *arg2, NvU32 wrOffset);

void kgraphicsSetFecsTraceWrOffset_GB100(OBJGPU *arg1, struct KernelGraphics *arg2, NvU32 wrOffset);

void kgraphicsSetFecsTraceHwEnable_GP100(OBJGPU *arg1, struct KernelGraphics *arg2, NvBool bEnable);

void kgraphicsSetFecsTraceHwEnable_GA100(OBJGPU *arg1, struct KernelGraphics *arg2, NvBool bEnable);

void kgraphicsSetFecsTraceHwEnable_GB100(OBJGPU *arg1, struct KernelGraphics *arg2, NvBool bEnable);

NvBool kgraphicsIsCtxswLoggingEnabled_FWCLIENT(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics);

static inline NvBool kgraphicsIsCtxswLoggingEnabled_920139(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics) {
    struct KernelGraphics_PRIVATE *pKernelGraphics_PRIVATE = (struct KernelGraphics_PRIVATE *)pKernelGraphics;
    return pKernelGraphics_PRIVATE->bCtxswLoggingEnabled;
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

static inline NvBool kgraphicsIsOverrideContextBuffersToGpuCached(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics) {
    struct KernelGraphics_PRIVATE *pKernelGraphics_PRIVATE = (struct KernelGraphics_PRIVATE *)pKernelGraphics;
    return pKernelGraphics_PRIVATE->bOverrideContextBuffersToGpuCached;
}

static inline void kgraphicsSetOverrideContextBuffersToGpuCached(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics, NvBool bProp) {
    struct KernelGraphics_PRIVATE *pKernelGraphics_PRIVATE = (struct KernelGraphics_PRIVATE *)pKernelGraphics;
    pKernelGraphics_PRIVATE->bOverrideContextBuffersToGpuCached = bProp;
}

static inline NvBool kgraphicsGetPeFiroBufferEnabled(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics) {
    struct KernelGraphics_PRIVATE *pKernelGraphics_PRIVATE = (struct KernelGraphics_PRIVATE *)pKernelGraphics;
    return pKernelGraphics_PRIVATE->bPeFiroBufferEnabled;
}

static inline void kgraphicsSetPeFiroBufferEnabled(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics, NvBool bProp) {
    struct KernelGraphics_PRIVATE *pKernelGraphics_PRIVATE = (struct KernelGraphics_PRIVATE *)pKernelGraphics;
    pKernelGraphics_PRIVATE->bPeFiroBufferEnabled = bProp;
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

static inline NvBool kgraphicsGetBug4208224WAREnabled(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics) {
    struct KernelGraphics_PRIVATE *pKernelGraphics_PRIVATE = (struct KernelGraphics_PRIVATE *)pKernelGraphics;
    return pKernelGraphics_PRIVATE->bBug4208224WAREnabled;
}

static inline void kgraphicsSetBug4208224WAREnabled(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics, NvBool bProp) {
    struct KernelGraphics_PRIVATE *pKernelGraphics_PRIVATE = (struct KernelGraphics_PRIVATE *)pKernelGraphics;
    pKernelGraphics_PRIVATE->bBug4208224WAREnabled = bProp;
}

void kgraphicsDestruct_IMPL(struct KernelGraphics *arg1);

#define __nvoc_kgraphicsDestruct(arg1) kgraphicsDestruct_IMPL(arg1)
void kgraphicsInvalidateStaticInfo_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline void kgraphicsInvalidateStaticInfo(OBJGPU *arg1, struct KernelGraphics *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsInvalidateStaticInfo(arg1, arg2) kgraphicsInvalidateStaticInfo_IMPL(arg1, arg2)
#endif //__nvoc_kernel_graphics_h_disabled

const CTX_BUF_INFO *kgraphicsGetCtxBufferInfo_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2, GR_CTX_BUFFER arg3);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline const CTX_BUF_INFO *kgraphicsGetCtxBufferInfo(OBJGPU *arg1, struct KernelGraphics *arg2, GR_CTX_BUFFER arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NULL;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsGetCtxBufferInfo(arg1, arg2, arg3) kgraphicsGetCtxBufferInfo_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_h_disabled

void kgraphicsSetCtxBufferInfo_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2, GR_CTX_BUFFER arg3, NvU64 size, NvU64 align, RM_ATTR_PAGE_SIZE attr, NvBool bContiguous);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline void kgraphicsSetCtxBufferInfo(OBJGPU *arg1, struct KernelGraphics *arg2, GR_CTX_BUFFER arg3, NvU64 size, NvU64 align, RM_ATTR_PAGE_SIZE attr, NvBool bContiguous) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsSetCtxBufferInfo(arg1, arg2, arg3, size, align, attr, bContiguous) kgraphicsSetCtxBufferInfo_IMPL(arg1, arg2, arg3, size, align, attr, bContiguous)
#endif //__nvoc_kernel_graphics_h_disabled

void kgraphicsClearCtxBufferInfo_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline void kgraphicsClearCtxBufferInfo(OBJGPU *arg1, struct KernelGraphics *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsClearCtxBufferInfo(arg1, arg2) kgraphicsClearCtxBufferInfo_IMPL(arg1, arg2)
#endif //__nvoc_kernel_graphics_h_disabled

NV_STATUS kgraphicsInitCtxBufPool_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2, struct Heap *arg3);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline NV_STATUS kgraphicsInitCtxBufPool(OBJGPU *arg1, struct KernelGraphics *arg2, struct Heap *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsInitCtxBufPool(arg1, arg2, arg3) kgraphicsInitCtxBufPool_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_h_disabled

struct CTX_BUF_POOL_INFO *kgraphicsGetCtxBufPool_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline struct CTX_BUF_POOL_INFO *kgraphicsGetCtxBufPool(OBJGPU *arg1, struct KernelGraphics *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NULL;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsGetCtxBufPool(arg1, arg2) kgraphicsGetCtxBufPool_IMPL(arg1, arg2)
#endif //__nvoc_kernel_graphics_h_disabled

void kgraphicsDestroyCtxBufPool_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline void kgraphicsDestroyCtxBufPool(OBJGPU *arg1, struct KernelGraphics *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsDestroyCtxBufPool(arg1, arg2) kgraphicsDestroyCtxBufPool_IMPL(arg1, arg2)
#endif //__nvoc_kernel_graphics_h_disabled

GR_GLOBALCTX_BUFFERS *kgraphicsGetGlobalCtxBuffers_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2, NvU32 gfid);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline GR_GLOBALCTX_BUFFERS *kgraphicsGetGlobalCtxBuffers(OBJGPU *arg1, struct KernelGraphics *arg2, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NULL;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsGetGlobalCtxBuffers(arg1, arg2, gfid) kgraphicsGetGlobalCtxBuffers_IMPL(arg1, arg2, gfid)
#endif //__nvoc_kernel_graphics_h_disabled

NvBool kgraphicsIsGlobalCtxBufferSizeAligned_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2, GR_GLOBALCTX_BUFFER arg3);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline NvBool kgraphicsIsGlobalCtxBufferSizeAligned(OBJGPU *arg1, struct KernelGraphics *arg2, GR_GLOBALCTX_BUFFER arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsIsGlobalCtxBufferSizeAligned(arg1, arg2, arg3) kgraphicsIsGlobalCtxBufferSizeAligned_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_h_disabled

const GR_BUFFER_ATTR *kgraphicsGetGlobalPrivAccessMapAttr_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline const GR_BUFFER_ATTR *kgraphicsGetGlobalPrivAccessMapAttr(OBJGPU *arg1, struct KernelGraphics *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NULL;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsGetGlobalPrivAccessMapAttr(arg1, arg2) kgraphicsGetGlobalPrivAccessMapAttr_IMPL(arg1, arg2)
#endif //__nvoc_kernel_graphics_h_disabled

NV_STATUS kgraphicsMapCtxBuffer_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2, MEMORY_DESCRIPTOR *arg3, struct OBJVASPACE *arg4, VA_LIST *arg5, NvBool bAlignSize, NvBool bIsReadOnly);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline NV_STATUS kgraphicsMapCtxBuffer(OBJGPU *arg1, struct KernelGraphics *arg2, MEMORY_DESCRIPTOR *arg3, struct OBJVASPACE *arg4, VA_LIST *arg5, NvBool bAlignSize, NvBool bIsReadOnly) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsMapCtxBuffer(arg1, arg2, arg3, arg4, arg5, bAlignSize, bIsReadOnly) kgraphicsMapCtxBuffer_IMPL(arg1, arg2, arg3, arg4, arg5, bAlignSize, bIsReadOnly)
#endif //__nvoc_kernel_graphics_h_disabled

void kgraphicsUnmapCtxBuffer_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2, struct OBJVASPACE *arg3, VA_LIST *arg4);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline void kgraphicsUnmapCtxBuffer(OBJGPU *arg1, struct KernelGraphics *arg2, struct OBJVASPACE *arg3, VA_LIST *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsUnmapCtxBuffer(arg1, arg2, arg3, arg4) kgraphicsUnmapCtxBuffer_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_graphics_h_disabled

void kgraphicsFreeGlobalCtxBuffers_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2, NvU32 gfid);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline void kgraphicsFreeGlobalCtxBuffers(OBJGPU *arg1, struct KernelGraphics *arg2, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsFreeGlobalCtxBuffers(arg1, arg2, gfid) kgraphicsFreeGlobalCtxBuffers_IMPL(arg1, arg2, gfid)
#endif //__nvoc_kernel_graphics_h_disabled

NV_STATUS kgraphicsGetMainCtxBufferSize_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2, NvBool bIncludeSubctxHdrs, NvU32 *pSize);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline NV_STATUS kgraphicsGetMainCtxBufferSize(OBJGPU *arg1, struct KernelGraphics *arg2, NvBool bIncludeSubctxHdrs, NvU32 *pSize) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsGetMainCtxBufferSize(arg1, arg2, bIncludeSubctxHdrs, pSize) kgraphicsGetMainCtxBufferSize_IMPL(arg1, arg2, bIncludeSubctxHdrs, pSize)
#endif //__nvoc_kernel_graphics_h_disabled

NV_STATUS kgraphicsGetClassByType_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2, NvU32 objectType, NvU32 *pClass);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline NV_STATUS kgraphicsGetClassByType(OBJGPU *arg1, struct KernelGraphics *arg2, NvU32 objectType, NvU32 *pClass) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsGetClassByType(arg1, arg2, objectType, pClass) kgraphicsGetClassByType_IMPL(arg1, arg2, objectType, pClass)
#endif //__nvoc_kernel_graphics_h_disabled

const GR_BUFFER_ATTR *kgraphicsGetContextBufferAttr_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2, GR_CTX_BUFFER arg3);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline const GR_BUFFER_ATTR *kgraphicsGetContextBufferAttr(OBJGPU *arg1, struct KernelGraphics *arg2, GR_CTX_BUFFER arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NULL;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsGetContextBufferAttr(arg1, arg2, arg3) kgraphicsGetContextBufferAttr_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_h_disabled

NV_STATUS kgraphicsCreateGoldenImageChannel_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline NV_STATUS kgraphicsCreateGoldenImageChannel(OBJGPU *arg1, struct KernelGraphics *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsCreateGoldenImageChannel(arg1, arg2) kgraphicsCreateGoldenImageChannel_IMPL(arg1, arg2)
#endif //__nvoc_kernel_graphics_h_disabled

NvBool kgraphicsIsGFXSupported_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline NvBool kgraphicsIsGFXSupported(OBJGPU *arg1, struct KernelGraphics *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsIsGFXSupported(arg1, arg2) kgraphicsIsGFXSupported_IMPL(arg1, arg2)
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

NV_STATUS kgraphicsInitializeDeferredStaticData_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2, NvHandle hClient, NvHandle hSubdevice);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline NV_STATUS kgraphicsInitializeDeferredStaticData(OBJGPU *arg1, struct KernelGraphics *arg2, NvHandle hClient, NvHandle hSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsInitializeDeferredStaticData(arg1, arg2, hClient, hSubdevice) kgraphicsInitializeDeferredStaticData_IMPL(arg1, arg2, hClient, hSubdevice)
#endif //__nvoc_kernel_graphics_h_disabled

const struct KGRAPHICS_STATIC_INFO *kgraphicsGetStaticInfo_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline const struct KGRAPHICS_STATIC_INFO *kgraphicsGetStaticInfo(OBJGPU *arg1, struct KernelGraphics *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NULL;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsGetStaticInfo(arg1, arg2) kgraphicsGetStaticInfo_IMPL(arg1, arg2)
#endif //__nvoc_kernel_graphics_h_disabled

NV_STATUS kgraphicsGetCaps_IMPL(OBJGPU *arg1, struct KernelGraphics *arg2, NvU8 *pGrCaps);

#ifdef __nvoc_kernel_graphics_h_disabled
static inline NV_STATUS kgraphicsGetCaps(OBJGPU *arg1, struct KernelGraphics *arg2, NvU8 *pGrCaps) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphics was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_h_disabled
#define kgraphicsGetCaps(arg1, arg2, pGrCaps) kgraphicsGetCaps_IMPL(arg1, arg2, pGrCaps)
#endif //__nvoc_kernel_graphics_h_disabled

#undef PRIVATE_FIELD


#endif // _KERNEL_GRAPHICS_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_GRAPHICS_NVOC_H_
