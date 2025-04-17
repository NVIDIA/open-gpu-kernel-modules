
#ifndef _G_UVM_NVOC_H_
#define _G_UVM_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2012-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_uvm_nvoc.h"

#ifndef UVM_H
#define UVM_H

/*!
 * @file
 * @brief  Provides definitions for all OBJUVM data structures and interfaces.
 */

#include "core/core.h"
#include "rmapi/control.h"
#include "rmapi/rmapi_utils.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/gpu.h"
#include "nvoc/utility.h"
#include "kernel/gpu/intr/intr_service.h"

#include "gpu/eng_state.h"

typedef enum
{
    MIMC,
    MOMC
} ACCESS_CNTR_TYPE;

/*!
 * Defines the structure used to contain all generic information related to
 * the OBJUVM.
 *  Contains the Unified Virtual Memory (UVM) feature related data.
 */


struct AccessCounterBuffer;

#ifndef __NVOC_CLASS_AccessCounterBuffer_TYPEDEF__
#define __NVOC_CLASS_AccessCounterBuffer_TYPEDEF__
typedef struct AccessCounterBuffer AccessCounterBuffer;
#endif /* __NVOC_CLASS_AccessCounterBuffer_TYPEDEF__ */

#ifndef __nvoc_class_id_AccessCounterBuffer
#define __nvoc_class_id_AccessCounterBuffer 0x1f0074
#endif /* __nvoc_class_id_AccessCounterBuffer */



/*
 * This structure is used to store all the necessary information concerning the access counter buffer.
 * It is contained within the UVM object.
*/
typedef struct
{
    // kernel fields
    struct AccessCounterBuffer *pAccessCounterBuffer;       // AccessCounterBuffer object

    // physical fields
    NvU64               bar2UvmAccessCntrBufferAddr; //This is the bar2 VA that is used by the gpu in
                                                     // order to access the buffer
    MEMORY_DESCRIPTOR   *pUvmAccessCntrMemDesc;      // Memory descriptor of the reconstructed access counter buffer
} ACCESS_CNTR_BUFFER;

typedef enum
{
    intr_notify,
    intr_error,
    intr_all
} ACCESS_CNTR_INTR_TYPE;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_UVM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__OBJUVM;
struct NVOC_METADATA__OBJENGSTATE;
struct NVOC_METADATA__IntrService;
struct NVOC_VTABLE__OBJUVM;


struct OBJUVM {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__OBJUVM *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct IntrService __nvoc_base_IntrService;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct IntrService *__nvoc_pbase_IntrService;    // intrserv super
    struct OBJUVM *__nvoc_pbase_OBJUVM;    // uvm

    // Vtable with 31 per-object function pointers
    NV_STATUS (*__uvmInitAccessCntrBuffer__)(OBJGPU *, struct OBJUVM * /*this*/, struct AccessCounterBuffer *);  // halified (2 hals) body
    NV_STATUS (*__uvmDestroyAccessCntrBuffer__)(OBJGPU *, struct OBJUVM * /*this*/, struct AccessCounterBuffer *);  // halified (2 hals) body
    NV_STATUS (*__uvmAccessCntrBufferUnregister__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32);  // halified (2 hals)
    NV_STATUS (*__uvmAccessCntrBufferRegister__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32, NvU32, RmPhysAddr *);  // halified (2 hals)
    NV_STATUS (*__uvmUnloadAccessCntrBuffer__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32);  // halified (2 hals) body
    NV_STATUS (*__uvmSetupAccessCntrBuffer__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32);  // halified (2 hals) body
    NV_STATUS (*__uvmReadAccessCntrBufferPutPtr__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__uvmReadAccessCntrBufferGetPtr__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__uvmReadAccessCntrBufferFullPtr__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32, NvBool *);  // halified (2 hals) body
    NV_STATUS (*__uvmAccessCntrSetGranularity__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32, ACCESS_CNTR_TYPE, NvU32);  // halified (2 hals) body
    NV_STATUS (*__uvmAccessCntrSetThreshold__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32, NvU32);  // halified (2 hals) body
    NV_STATUS (*__uvmAccessCntrSetCounterLimit__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32, NvU32, NvU32);  // halified (2 hals) body
    NV_STATUS (*__uvmWriteAccessCntrBufferGetPtr__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32, NvU32);  // halified (2 hals) body
    NV_STATUS (*__uvmEnableAccessCntr__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32, NvBool);  // halified (2 hals) body
    NV_STATUS (*__uvmDisableAccessCntr__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32, NvBool);  // halified (2 hals) body
    NV_STATUS (*__uvmEnableAccessCntrIntr__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32);  // halified (2 hals) body
    NV_STATUS (*__uvmDisableAccessCntrIntr__)(OBJGPU *, struct OBJUVM * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__uvmGetAccessCntrRegisterMappings__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32, NvP64 *, NvP64 *, NvP64 *, NvP64 *, NvP64 *, NvP64 *, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__uvmAccessCntrService__)(OBJGPU *, struct OBJUVM * /*this*/);  // halified (2 hals) body
    NvU32 (*__uvmGetAccessCounterBufferSize__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32);  // halified (2 hals) body
    void (*__uvmProgramWriteAccessCntrBufferAddress__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32, NvU64);  // halified (2 hals) body
    void (*__uvmProgramAccessCntrBufferEnabled__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32, NvBool);  // halified (2 hals) body
    NvBool (*__uvmIsAccessCntrBufferEnabled__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32);  // halified (2 hals) body
    NvBool (*__uvmIsAccessCntrBufferPushed__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32);  // halified (2 hals) body
    NvU32 (*__uvmGetRegOffsetAccessCntrBufferPut__)(struct OBJUVM * /*this*/, NvU32);  // halified (3 hals) body
    NvU32 (*__uvmGetRegOffsetAccessCntrBufferGet__)(struct OBJUVM * /*this*/, NvU32);  // halified (3 hals) body
    NvU32 (*__uvmGetRegOffsetAccessCntrBufferHi__)(struct OBJUVM * /*this*/, NvU32);  // halified (3 hals) body
    NvU32 (*__uvmGetRegOffsetAccessCntrBufferLo__)(struct OBJUVM * /*this*/, NvU32);  // halified (3 hals) body
    NvU32 (*__uvmGetRegOffsetAccessCntrBufferConfig__)(struct OBJUVM * /*this*/, NvU32);  // halified (3 hals) body
    NvU32 (*__uvmGetRegOffsetAccessCntrBufferInfo__)(struct OBJUVM * /*this*/, NvU32);  // halified (3 hals) body
    NvU32 (*__uvmGetRegOffsetAccessCntrBufferSize__)(struct OBJUVM * /*this*/, NvU32);  // halified (3 hals) body

    // Data members
    ACCESS_CNTR_BUFFER *pAccessCounterBuffers;
    NvU32 accessCounterBufferCount;
    NvHandle hClient;
    NvHandle hSubdevice;
    RM_API *pRmApi;
};


// Vtable with 18 per-class function pointers
struct NVOC_VTABLE__OBJUVM {
    void (*__uvmStateDestroy__)(OBJGPU *, struct OBJUVM * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__uvmStateInitUnlocked__)(OBJGPU *, struct OBJUVM * /*this*/);  // virtual override (engstate) base (engstate)
    void (*__uvmRegisterIntrService__)(OBJGPU *, struct OBJUVM * /*this*/, IntrServiceRecord *);  // virtual override (intrserv) base (intrserv)
    NvU32 (*__uvmServiceInterrupt__)(OBJGPU *, struct OBJUVM * /*this*/, IntrServiceServiceInterruptArguments *);  // virtual override (intrserv) base (intrserv)
    NV_STATUS (*__uvmConstructEngine__)(OBJGPU *, struct OBJUVM * /*this*/, ENGDESCRIPTOR);  // virtual inherited (engstate) base (engstate)
    void (*__uvmInitMissing__)(OBJGPU *, struct OBJUVM * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__uvmStatePreInitLocked__)(OBJGPU *, struct OBJUVM * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__uvmStatePreInitUnlocked__)(OBJGPU *, struct OBJUVM * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__uvmStateInitLocked__)(OBJGPU *, struct OBJUVM * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__uvmStatePreLoad__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__uvmStateLoad__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__uvmStatePostLoad__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__uvmStatePreUnload__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__uvmStateUnload__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__uvmStatePostUnload__)(OBJGPU *, struct OBJUVM * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NvBool (*__uvmIsPresent__)(OBJGPU *, struct OBJUVM * /*this*/);  // virtual inherited (engstate) base (engstate)
    NvBool (*__uvmClearInterrupt__)(OBJGPU *, struct OBJUVM * /*this*/, IntrServiceClearInterruptArguments *);  // virtual inherited (intrserv) base (intrserv)
    NV_STATUS (*__uvmServiceNotificationInterrupt__)(OBJGPU *, struct OBJUVM * /*this*/, IntrServiceServiceNotificationInterruptArguments *);  // virtual inherited (intrserv) base (intrserv)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__OBJUVM {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
    const struct NVOC_METADATA__IntrService metadata__IntrService;
    const struct NVOC_VTABLE__OBJUVM vtable;
};

#ifndef __NVOC_CLASS_OBJUVM_TYPEDEF__
#define __NVOC_CLASS_OBJUVM_TYPEDEF__
typedef struct OBJUVM OBJUVM;
#endif /* __NVOC_CLASS_OBJUVM_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJUVM
#define __nvoc_class_id_OBJUVM 0xf9a17d
#endif /* __nvoc_class_id_OBJUVM */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJUVM;

#define __staticCast_OBJUVM(pThis) \
    ((pThis)->__nvoc_pbase_OBJUVM)

#ifdef __nvoc_uvm_h_disabled
#define __dynamicCast_OBJUVM(pThis) ((OBJUVM*) NULL)
#else //__nvoc_uvm_h_disabled
#define __dynamicCast_OBJUVM(pThis) \
    ((OBJUVM*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJUVM)))
#endif //__nvoc_uvm_h_disabled

// Property macros
#define PDB_PROP_UVM_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_UVM_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_OBJUVM(OBJUVM**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJUVM(OBJUVM**, Dynamic*, NvU32);
#define __objCreate_OBJUVM(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJUVM((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define uvmStateDestroy_FNPTR(pUvm) pUvm->__nvoc_metadata_ptr->vtable.__uvmStateDestroy__
#define uvmStateDestroy(pGpu, pUvm) uvmStateDestroy_DISPATCH(pGpu, pUvm)
#define uvmStateInitUnlocked_FNPTR(pUvm) pUvm->__nvoc_metadata_ptr->vtable.__uvmStateInitUnlocked__
#define uvmStateInitUnlocked(pGpu, pUvm) uvmStateInitUnlocked_DISPATCH(pGpu, pUvm)
#define uvmInitAccessCntrBuffer_FNPTR(pUvm) pUvm->__uvmInitAccessCntrBuffer__
#define uvmInitAccessCntrBuffer(pGpu, pUvm, pAccessCounterBuffer) uvmInitAccessCntrBuffer_DISPATCH(pGpu, pUvm, pAccessCounterBuffer)
#define uvmInitAccessCntrBuffer_HAL(pGpu, pUvm, pAccessCounterBuffer) uvmInitAccessCntrBuffer_DISPATCH(pGpu, pUvm, pAccessCounterBuffer)
#define uvmDestroyAccessCntrBuffer_FNPTR(pUvm) pUvm->__uvmDestroyAccessCntrBuffer__
#define uvmDestroyAccessCntrBuffer(pGpu, pUvm, pAccessCounterBuffer) uvmDestroyAccessCntrBuffer_DISPATCH(pGpu, pUvm, pAccessCounterBuffer)
#define uvmDestroyAccessCntrBuffer_HAL(pGpu, pUvm, pAccessCounterBuffer) uvmDestroyAccessCntrBuffer_DISPATCH(pGpu, pUvm, pAccessCounterBuffer)
#define uvmAccessCntrBufferUnregister_FNPTR(arg_this) arg_this->__uvmAccessCntrBufferUnregister__
#define uvmAccessCntrBufferUnregister(arg1, arg_this, accessCounterIndex) uvmAccessCntrBufferUnregister_DISPATCH(arg1, arg_this, accessCounterIndex)
#define uvmAccessCntrBufferUnregister_HAL(arg1, arg_this, accessCounterIndex) uvmAccessCntrBufferUnregister_DISPATCH(arg1, arg_this, accessCounterIndex)
#define uvmAccessCntrBufferRegister_FNPTR(arg_this) arg_this->__uvmAccessCntrBufferRegister__
#define uvmAccessCntrBufferRegister(arg1, arg_this, accessCounterIndex, arg4, arg5) uvmAccessCntrBufferRegister_DISPATCH(arg1, arg_this, accessCounterIndex, arg4, arg5)
#define uvmAccessCntrBufferRegister_HAL(arg1, arg_this, accessCounterIndex, arg4, arg5) uvmAccessCntrBufferRegister_DISPATCH(arg1, arg_this, accessCounterIndex, arg4, arg5)
#define uvmUnloadAccessCntrBuffer_FNPTR(pUvm) pUvm->__uvmUnloadAccessCntrBuffer__
#define uvmUnloadAccessCntrBuffer(pGpu, pUvm, accessCounterIndex) uvmUnloadAccessCntrBuffer_DISPATCH(pGpu, pUvm, accessCounterIndex)
#define uvmUnloadAccessCntrBuffer_HAL(pGpu, pUvm, accessCounterIndex) uvmUnloadAccessCntrBuffer_DISPATCH(pGpu, pUvm, accessCounterIndex)
#define uvmSetupAccessCntrBuffer_FNPTR(pUvm) pUvm->__uvmSetupAccessCntrBuffer__
#define uvmSetupAccessCntrBuffer(pGpu, pUvm, accessCounterIndex) uvmSetupAccessCntrBuffer_DISPATCH(pGpu, pUvm, accessCounterIndex)
#define uvmSetupAccessCntrBuffer_HAL(pGpu, pUvm, accessCounterIndex) uvmSetupAccessCntrBuffer_DISPATCH(pGpu, pUvm, accessCounterIndex)
#define uvmReadAccessCntrBufferPutPtr_FNPTR(pUvm) pUvm->__uvmReadAccessCntrBufferPutPtr__
#define uvmReadAccessCntrBufferPutPtr(pGpu, pUvm, accessCounterIndex, arg4) uvmReadAccessCntrBufferPutPtr_DISPATCH(pGpu, pUvm, accessCounterIndex, arg4)
#define uvmReadAccessCntrBufferPutPtr_HAL(pGpu, pUvm, accessCounterIndex, arg4) uvmReadAccessCntrBufferPutPtr_DISPATCH(pGpu, pUvm, accessCounterIndex, arg4)
#define uvmReadAccessCntrBufferGetPtr_FNPTR(pUvm) pUvm->__uvmReadAccessCntrBufferGetPtr__
#define uvmReadAccessCntrBufferGetPtr(pGpu, pUvm, accessCounterIndex, arg4) uvmReadAccessCntrBufferGetPtr_DISPATCH(pGpu, pUvm, accessCounterIndex, arg4)
#define uvmReadAccessCntrBufferGetPtr_HAL(pGpu, pUvm, accessCounterIndex, arg4) uvmReadAccessCntrBufferGetPtr_DISPATCH(pGpu, pUvm, accessCounterIndex, arg4)
#define uvmReadAccessCntrBufferFullPtr_FNPTR(pUvm) pUvm->__uvmReadAccessCntrBufferFullPtr__
#define uvmReadAccessCntrBufferFullPtr(pGpu, pUvm, accessCounterIndex, arg4) uvmReadAccessCntrBufferFullPtr_DISPATCH(pGpu, pUvm, accessCounterIndex, arg4)
#define uvmReadAccessCntrBufferFullPtr_HAL(pGpu, pUvm, accessCounterIndex, arg4) uvmReadAccessCntrBufferFullPtr_DISPATCH(pGpu, pUvm, accessCounterIndex, arg4)
#define uvmAccessCntrSetGranularity_FNPTR(pUvm) pUvm->__uvmAccessCntrSetGranularity__
#define uvmAccessCntrSetGranularity(pGpu, pUvm, accessCounterIndex, arg4, arg5) uvmAccessCntrSetGranularity_DISPATCH(pGpu, pUvm, accessCounterIndex, arg4, arg5)
#define uvmAccessCntrSetGranularity_HAL(pGpu, pUvm, accessCounterIndex, arg4, arg5) uvmAccessCntrSetGranularity_DISPATCH(pGpu, pUvm, accessCounterIndex, arg4, arg5)
#define uvmAccessCntrSetThreshold_FNPTR(pUvm) pUvm->__uvmAccessCntrSetThreshold__
#define uvmAccessCntrSetThreshold(pGpu, pUvm, accessCounterIndex, arg4) uvmAccessCntrSetThreshold_DISPATCH(pGpu, pUvm, accessCounterIndex, arg4)
#define uvmAccessCntrSetThreshold_HAL(pGpu, pUvm, accessCounterIndex, arg4) uvmAccessCntrSetThreshold_DISPATCH(pGpu, pUvm, accessCounterIndex, arg4)
#define uvmAccessCntrSetCounterLimit_FNPTR(pUvm) pUvm->__uvmAccessCntrSetCounterLimit__
#define uvmAccessCntrSetCounterLimit(pGpu, pUvm, accessCounterIndex, arg4, arg5) uvmAccessCntrSetCounterLimit_DISPATCH(pGpu, pUvm, accessCounterIndex, arg4, arg5)
#define uvmAccessCntrSetCounterLimit_HAL(pGpu, pUvm, accessCounterIndex, arg4, arg5) uvmAccessCntrSetCounterLimit_DISPATCH(pGpu, pUvm, accessCounterIndex, arg4, arg5)
#define uvmWriteAccessCntrBufferGetPtr_FNPTR(pUvm) pUvm->__uvmWriteAccessCntrBufferGetPtr__
#define uvmWriteAccessCntrBufferGetPtr(pGpu, pUvm, accessCounterIndex, arg4) uvmWriteAccessCntrBufferGetPtr_DISPATCH(pGpu, pUvm, accessCounterIndex, arg4)
#define uvmWriteAccessCntrBufferGetPtr_HAL(pGpu, pUvm, accessCounterIndex, arg4) uvmWriteAccessCntrBufferGetPtr_DISPATCH(pGpu, pUvm, accessCounterIndex, arg4)
#define uvmEnableAccessCntr_FNPTR(pUvm) pUvm->__uvmEnableAccessCntr__
#define uvmEnableAccessCntr(pGpu, pUvm, accessCounterIndex, arg4) uvmEnableAccessCntr_DISPATCH(pGpu, pUvm, accessCounterIndex, arg4)
#define uvmEnableAccessCntr_HAL(pGpu, pUvm, accessCounterIndex, arg4) uvmEnableAccessCntr_DISPATCH(pGpu, pUvm, accessCounterIndex, arg4)
#define uvmDisableAccessCntr_FNPTR(pUvm) pUvm->__uvmDisableAccessCntr__
#define uvmDisableAccessCntr(pGpu, pUvm, accessCounterIndex, arg4) uvmDisableAccessCntr_DISPATCH(pGpu, pUvm, accessCounterIndex, arg4)
#define uvmDisableAccessCntr_HAL(pGpu, pUvm, accessCounterIndex, arg4) uvmDisableAccessCntr_DISPATCH(pGpu, pUvm, accessCounterIndex, arg4)
#define uvmEnableAccessCntrIntr_FNPTR(pUvm) pUvm->__uvmEnableAccessCntrIntr__
#define uvmEnableAccessCntrIntr(pGpu, pUvm, arg3) uvmEnableAccessCntrIntr_DISPATCH(pGpu, pUvm, arg3)
#define uvmEnableAccessCntrIntr_HAL(pGpu, pUvm, arg3) uvmEnableAccessCntrIntr_DISPATCH(pGpu, pUvm, arg3)
#define uvmDisableAccessCntrIntr_FNPTR(pUvm) pUvm->__uvmDisableAccessCntrIntr__
#define uvmDisableAccessCntrIntr(pGpu, pUvm) uvmDisableAccessCntrIntr_DISPATCH(pGpu, pUvm)
#define uvmDisableAccessCntrIntr_HAL(pGpu, pUvm) uvmDisableAccessCntrIntr_DISPATCH(pGpu, pUvm)
#define uvmGetAccessCntrRegisterMappings_FNPTR(pUvm) pUvm->__uvmGetAccessCntrRegisterMappings__
#define uvmGetAccessCntrRegisterMappings(pGpu, pUvm, accessCounterIndex, arg4, arg5, arg6, arg7, arg8, arg9, arg10) uvmGetAccessCntrRegisterMappings_DISPATCH(pGpu, pUvm, accessCounterIndex, arg4, arg5, arg6, arg7, arg8, arg9, arg10)
#define uvmGetAccessCntrRegisterMappings_HAL(pGpu, pUvm, accessCounterIndex, arg4, arg5, arg6, arg7, arg8, arg9, arg10) uvmGetAccessCntrRegisterMappings_DISPATCH(pGpu, pUvm, accessCounterIndex, arg4, arg5, arg6, arg7, arg8, arg9, arg10)
#define uvmRegisterIntrService_FNPTR(pUvm) pUvm->__nvoc_metadata_ptr->vtable.__uvmRegisterIntrService__
#define uvmRegisterIntrService(arg1, pUvm, arg3) uvmRegisterIntrService_DISPATCH(arg1, pUvm, arg3)
#define uvmServiceInterrupt_FNPTR(pUvm) pUvm->__nvoc_metadata_ptr->vtable.__uvmServiceInterrupt__
#define uvmServiceInterrupt(arg1, pUvm, arg3) uvmServiceInterrupt_DISPATCH(arg1, pUvm, arg3)
#define uvmAccessCntrService_FNPTR(pUvm) pUvm->__uvmAccessCntrService__
#define uvmAccessCntrService(pGpu, pUvm) uvmAccessCntrService_DISPATCH(pGpu, pUvm)
#define uvmAccessCntrService_HAL(pGpu, pUvm) uvmAccessCntrService_DISPATCH(pGpu, pUvm)
#define uvmGetAccessCounterBufferSize_FNPTR(pUvm) pUvm->__uvmGetAccessCounterBufferSize__
#define uvmGetAccessCounterBufferSize(pGpu, pUvm, accessCounterIndex) uvmGetAccessCounterBufferSize_DISPATCH(pGpu, pUvm, accessCounterIndex)
#define uvmGetAccessCounterBufferSize_HAL(pGpu, pUvm, accessCounterIndex) uvmGetAccessCounterBufferSize_DISPATCH(pGpu, pUvm, accessCounterIndex)
#define uvmProgramWriteAccessCntrBufferAddress_FNPTR(pUvm) pUvm->__uvmProgramWriteAccessCntrBufferAddress__
#define uvmProgramWriteAccessCntrBufferAddress(pGpu, pUvm, accessCounterIndex, addr) uvmProgramWriteAccessCntrBufferAddress_DISPATCH(pGpu, pUvm, accessCounterIndex, addr)
#define uvmProgramWriteAccessCntrBufferAddress_HAL(pGpu, pUvm, accessCounterIndex, addr) uvmProgramWriteAccessCntrBufferAddress_DISPATCH(pGpu, pUvm, accessCounterIndex, addr)
#define uvmProgramAccessCntrBufferEnabled_FNPTR(pUvm) pUvm->__uvmProgramAccessCntrBufferEnabled__
#define uvmProgramAccessCntrBufferEnabled(pGpu, pUvm, accessCounterIndex, bEn) uvmProgramAccessCntrBufferEnabled_DISPATCH(pGpu, pUvm, accessCounterIndex, bEn)
#define uvmProgramAccessCntrBufferEnabled_HAL(pGpu, pUvm, accessCounterIndex, bEn) uvmProgramAccessCntrBufferEnabled_DISPATCH(pGpu, pUvm, accessCounterIndex, bEn)
#define uvmIsAccessCntrBufferEnabled_FNPTR(pUvm) pUvm->__uvmIsAccessCntrBufferEnabled__
#define uvmIsAccessCntrBufferEnabled(pGpu, pUvm, accessCounterIndex) uvmIsAccessCntrBufferEnabled_DISPATCH(pGpu, pUvm, accessCounterIndex)
#define uvmIsAccessCntrBufferEnabled_HAL(pGpu, pUvm, accessCounterIndex) uvmIsAccessCntrBufferEnabled_DISPATCH(pGpu, pUvm, accessCounterIndex)
#define uvmIsAccessCntrBufferPushed_FNPTR(pUvm) pUvm->__uvmIsAccessCntrBufferPushed__
#define uvmIsAccessCntrBufferPushed(pGpu, pUvm, accessCounterIndex) uvmIsAccessCntrBufferPushed_DISPATCH(pGpu, pUvm, accessCounterIndex)
#define uvmIsAccessCntrBufferPushed_HAL(pGpu, pUvm, accessCounterIndex) uvmIsAccessCntrBufferPushed_DISPATCH(pGpu, pUvm, accessCounterIndex)
#define uvmGetRegOffsetAccessCntrBufferPut_FNPTR(pUvm) pUvm->__uvmGetRegOffsetAccessCntrBufferPut__
#define uvmGetRegOffsetAccessCntrBufferPut(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferPut_DISPATCH(pUvm, accessCounterIndex)
#define uvmGetRegOffsetAccessCntrBufferPut_HAL(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferPut_DISPATCH(pUvm, accessCounterIndex)
#define uvmGetRegOffsetAccessCntrBufferGet_FNPTR(pUvm) pUvm->__uvmGetRegOffsetAccessCntrBufferGet__
#define uvmGetRegOffsetAccessCntrBufferGet(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferGet_DISPATCH(pUvm, accessCounterIndex)
#define uvmGetRegOffsetAccessCntrBufferGet_HAL(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferGet_DISPATCH(pUvm, accessCounterIndex)
#define uvmGetRegOffsetAccessCntrBufferHi_FNPTR(pUvm) pUvm->__uvmGetRegOffsetAccessCntrBufferHi__
#define uvmGetRegOffsetAccessCntrBufferHi(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferHi_DISPATCH(pUvm, accessCounterIndex)
#define uvmGetRegOffsetAccessCntrBufferHi_HAL(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferHi_DISPATCH(pUvm, accessCounterIndex)
#define uvmGetRegOffsetAccessCntrBufferLo_FNPTR(pUvm) pUvm->__uvmGetRegOffsetAccessCntrBufferLo__
#define uvmGetRegOffsetAccessCntrBufferLo(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferLo_DISPATCH(pUvm, accessCounterIndex)
#define uvmGetRegOffsetAccessCntrBufferLo_HAL(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferLo_DISPATCH(pUvm, accessCounterIndex)
#define uvmGetRegOffsetAccessCntrBufferConfig_FNPTR(pUvm) pUvm->__uvmGetRegOffsetAccessCntrBufferConfig__
#define uvmGetRegOffsetAccessCntrBufferConfig(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferConfig_DISPATCH(pUvm, accessCounterIndex)
#define uvmGetRegOffsetAccessCntrBufferConfig_HAL(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferConfig_DISPATCH(pUvm, accessCounterIndex)
#define uvmGetRegOffsetAccessCntrBufferInfo_FNPTR(pUvm) pUvm->__uvmGetRegOffsetAccessCntrBufferInfo__
#define uvmGetRegOffsetAccessCntrBufferInfo(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferInfo_DISPATCH(pUvm, accessCounterIndex)
#define uvmGetRegOffsetAccessCntrBufferInfo_HAL(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferInfo_DISPATCH(pUvm, accessCounterIndex)
#define uvmGetRegOffsetAccessCntrBufferSize_FNPTR(pUvm) pUvm->__uvmGetRegOffsetAccessCntrBufferSize__
#define uvmGetRegOffsetAccessCntrBufferSize(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferSize_DISPATCH(pUvm, accessCounterIndex)
#define uvmGetRegOffsetAccessCntrBufferSize_HAL(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferSize_DISPATCH(pUvm, accessCounterIndex)
#define uvmConstructEngine_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateConstructEngine__
#define uvmConstructEngine(pGpu, pEngstate, arg3) uvmConstructEngine_DISPATCH(pGpu, pEngstate, arg3)
#define uvmInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define uvmInitMissing(pGpu, pEngstate) uvmInitMissing_DISPATCH(pGpu, pEngstate)
#define uvmStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__
#define uvmStatePreInitLocked(pGpu, pEngstate) uvmStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define uvmStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define uvmStatePreInitUnlocked(pGpu, pEngstate) uvmStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define uvmStateInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitLocked__
#define uvmStateInitLocked(pGpu, pEngstate) uvmStateInitLocked_DISPATCH(pGpu, pEngstate)
#define uvmStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__
#define uvmStatePreLoad(pGpu, pEngstate, arg3) uvmStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define uvmStateLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateLoad__
#define uvmStateLoad(pGpu, pEngstate, arg3) uvmStateLoad_DISPATCH(pGpu, pEngstate, arg3)
#define uvmStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostLoad__
#define uvmStatePostLoad(pGpu, pEngstate, arg3) uvmStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define uvmStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__
#define uvmStatePreUnload(pGpu, pEngstate, arg3) uvmStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define uvmStateUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateUnload__
#define uvmStateUnload(pGpu, pEngstate, arg3) uvmStateUnload_DISPATCH(pGpu, pEngstate, arg3)
#define uvmStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__
#define uvmStatePostUnload(pGpu, pEngstate, arg3) uvmStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define uvmIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__
#define uvmIsPresent(pGpu, pEngstate) uvmIsPresent_DISPATCH(pGpu, pEngstate)
#define uvmClearInterrupt_FNPTR(pIntrService) pIntrService->__nvoc_base_IntrService.__nvoc_metadata_ptr->vtable.__intrservClearInterrupt__
#define uvmClearInterrupt(pGpu, pIntrService, pParams) uvmClearInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define uvmServiceNotificationInterrupt_FNPTR(pIntrService) pIntrService->__nvoc_base_IntrService.__nvoc_metadata_ptr->vtable.__intrservServiceNotificationInterrupt__
#define uvmServiceNotificationInterrupt(pGpu, pIntrService, pParams) uvmServiceNotificationInterrupt_DISPATCH(pGpu, pIntrService, pParams)

// Dispatch functions
static inline void uvmStateDestroy_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm) {
    pUvm->__nvoc_metadata_ptr->vtable.__uvmStateDestroy__(pGpu, pUvm);
}

static inline NV_STATUS uvmStateInitUnlocked_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm) {
    return pUvm->__nvoc_metadata_ptr->vtable.__uvmStateInitUnlocked__(pGpu, pUvm);
}

static inline NV_STATUS uvmInitAccessCntrBuffer_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm, struct AccessCounterBuffer *pAccessCounterBuffer) {
    return pUvm->__uvmInitAccessCntrBuffer__(pGpu, pUvm, pAccessCounterBuffer);
}

static inline NV_STATUS uvmDestroyAccessCntrBuffer_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm, struct AccessCounterBuffer *pAccessCounterBuffer) {
    return pUvm->__uvmDestroyAccessCntrBuffer__(pGpu, pUvm, pAccessCounterBuffer);
}

static inline NV_STATUS uvmAccessCntrBufferUnregister_DISPATCH(OBJGPU *arg1, struct OBJUVM *arg_this, NvU32 accessCounterIndex) {
    return arg_this->__uvmAccessCntrBufferUnregister__(arg1, arg_this, accessCounterIndex);
}

static inline NV_STATUS uvmAccessCntrBufferRegister_DISPATCH(OBJGPU *arg1, struct OBJUVM *arg_this, NvU32 accessCounterIndex, NvU32 arg4, RmPhysAddr *arg5) {
    return arg_this->__uvmAccessCntrBufferRegister__(arg1, arg_this, accessCounterIndex, arg4, arg5);
}

static inline NV_STATUS uvmUnloadAccessCntrBuffer_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    return pUvm->__uvmUnloadAccessCntrBuffer__(pGpu, pUvm, accessCounterIndex);
}

static inline NV_STATUS uvmSetupAccessCntrBuffer_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    return pUvm->__uvmSetupAccessCntrBuffer__(pGpu, pUvm, accessCounterIndex);
}

static inline NV_STATUS uvmReadAccessCntrBufferPutPtr_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 *arg4) {
    return pUvm->__uvmReadAccessCntrBufferPutPtr__(pGpu, pUvm, accessCounterIndex, arg4);
}

static inline NV_STATUS uvmReadAccessCntrBufferGetPtr_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 *arg4) {
    return pUvm->__uvmReadAccessCntrBufferGetPtr__(pGpu, pUvm, accessCounterIndex, arg4);
}

static inline NV_STATUS uvmReadAccessCntrBufferFullPtr_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvBool *arg4) {
    return pUvm->__uvmReadAccessCntrBufferFullPtr__(pGpu, pUvm, accessCounterIndex, arg4);
}

static inline NV_STATUS uvmAccessCntrSetGranularity_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, ACCESS_CNTR_TYPE arg4, NvU32 arg5) {
    return pUvm->__uvmAccessCntrSetGranularity__(pGpu, pUvm, accessCounterIndex, arg4, arg5);
}

static inline NV_STATUS uvmAccessCntrSetThreshold_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 arg4) {
    return pUvm->__uvmAccessCntrSetThreshold__(pGpu, pUvm, accessCounterIndex, arg4);
}

static inline NV_STATUS uvmAccessCntrSetCounterLimit_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 arg4, NvU32 arg5) {
    return pUvm->__uvmAccessCntrSetCounterLimit__(pGpu, pUvm, accessCounterIndex, arg4, arg5);
}

static inline NV_STATUS uvmWriteAccessCntrBufferGetPtr_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 arg4) {
    return pUvm->__uvmWriteAccessCntrBufferGetPtr__(pGpu, pUvm, accessCounterIndex, arg4);
}

static inline NV_STATUS uvmEnableAccessCntr_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvBool arg4) {
    return pUvm->__uvmEnableAccessCntr__(pGpu, pUvm, accessCounterIndex, arg4);
}

static inline NV_STATUS uvmDisableAccessCntr_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvBool arg4) {
    return pUvm->__uvmDisableAccessCntr__(pGpu, pUvm, accessCounterIndex, arg4);
}

static inline NV_STATUS uvmEnableAccessCntrIntr_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 arg3) {
    return pUvm->__uvmEnableAccessCntrIntr__(pGpu, pUvm, arg3);
}

static inline NV_STATUS uvmDisableAccessCntrIntr_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm) {
    return pUvm->__uvmDisableAccessCntrIntr__(pGpu, pUvm);
}

static inline NV_STATUS uvmGetAccessCntrRegisterMappings_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvP64 *arg4, NvP64 *arg5, NvP64 *arg6, NvP64 *arg7, NvP64 *arg8, NvP64 *arg9, NvU32 *arg10) {
    return pUvm->__uvmGetAccessCntrRegisterMappings__(pGpu, pUvm, accessCounterIndex, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
}

static inline void uvmRegisterIntrService_DISPATCH(OBJGPU *arg1, struct OBJUVM *pUvm, IntrServiceRecord arg3[179]) {
    pUvm->__nvoc_metadata_ptr->vtable.__uvmRegisterIntrService__(arg1, pUvm, arg3);
}

static inline NvU32 uvmServiceInterrupt_DISPATCH(OBJGPU *arg1, struct OBJUVM *pUvm, IntrServiceServiceInterruptArguments *arg3) {
    return pUvm->__nvoc_metadata_ptr->vtable.__uvmServiceInterrupt__(arg1, pUvm, arg3);
}

static inline NV_STATUS uvmAccessCntrService_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm) {
    return pUvm->__uvmAccessCntrService__(pGpu, pUvm);
}

static inline NvU32 uvmGetAccessCounterBufferSize_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    return pUvm->__uvmGetAccessCounterBufferSize__(pGpu, pUvm, accessCounterIndex);
}

static inline void uvmProgramWriteAccessCntrBufferAddress_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU64 addr) {
    pUvm->__uvmProgramWriteAccessCntrBufferAddress__(pGpu, pUvm, accessCounterIndex, addr);
}

static inline void uvmProgramAccessCntrBufferEnabled_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvBool bEn) {
    pUvm->__uvmProgramAccessCntrBufferEnabled__(pGpu, pUvm, accessCounterIndex, bEn);
}

static inline NvBool uvmIsAccessCntrBufferEnabled_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    return pUvm->__uvmIsAccessCntrBufferEnabled__(pGpu, pUvm, accessCounterIndex);
}

static inline NvBool uvmIsAccessCntrBufferPushed_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    return pUvm->__uvmIsAccessCntrBufferPushed__(pGpu, pUvm, accessCounterIndex);
}

static inline NvU32 uvmGetRegOffsetAccessCntrBufferPut_DISPATCH(struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    return pUvm->__uvmGetRegOffsetAccessCntrBufferPut__(pUvm, accessCounterIndex);
}

static inline NvU32 uvmGetRegOffsetAccessCntrBufferGet_DISPATCH(struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    return pUvm->__uvmGetRegOffsetAccessCntrBufferGet__(pUvm, accessCounterIndex);
}

static inline NvU32 uvmGetRegOffsetAccessCntrBufferHi_DISPATCH(struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    return pUvm->__uvmGetRegOffsetAccessCntrBufferHi__(pUvm, accessCounterIndex);
}

static inline NvU32 uvmGetRegOffsetAccessCntrBufferLo_DISPATCH(struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    return pUvm->__uvmGetRegOffsetAccessCntrBufferLo__(pUvm, accessCounterIndex);
}

static inline NvU32 uvmGetRegOffsetAccessCntrBufferConfig_DISPATCH(struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    return pUvm->__uvmGetRegOffsetAccessCntrBufferConfig__(pUvm, accessCounterIndex);
}

static inline NvU32 uvmGetRegOffsetAccessCntrBufferInfo_DISPATCH(struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    return pUvm->__uvmGetRegOffsetAccessCntrBufferInfo__(pUvm, accessCounterIndex);
}

static inline NvU32 uvmGetRegOffsetAccessCntrBufferSize_DISPATCH(struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    return pUvm->__uvmGetRegOffsetAccessCntrBufferSize__(pUvm, accessCounterIndex);
}

static inline NV_STATUS uvmConstructEngine_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pEngstate, ENGDESCRIPTOR arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__uvmConstructEngine__(pGpu, pEngstate, arg3);
}

static inline void uvmInitMissing_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__uvmInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS uvmStatePreInitLocked_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__uvmStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS uvmStatePreInitUnlocked_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__uvmStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS uvmStateInitLocked_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__uvmStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS uvmStatePreLoad_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__uvmStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS uvmStateLoad_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__uvmStateLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS uvmStatePostLoad_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__uvmStatePostLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS uvmStatePreUnload_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__uvmStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS uvmStateUnload_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__uvmStateUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS uvmStatePostUnload_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__uvmStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline NvBool uvmIsPresent_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__uvmIsPresent__(pGpu, pEngstate);
}

static inline NvBool uvmClearInterrupt_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return pIntrService->__nvoc_metadata_ptr->vtable.__uvmClearInterrupt__(pGpu, pIntrService, pParams);
}

static inline NV_STATUS uvmServiceNotificationInterrupt_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return pIntrService->__nvoc_metadata_ptr->vtable.__uvmServiceNotificationInterrupt__(pGpu, pIntrService, pParams);
}

NV_STATUS uvmInitializeAccessCntrBuffer_IMPL(OBJGPU *pGpu, struct OBJUVM *pUvm, struct AccessCounterBuffer *pAccessCounterBuffer);


#ifdef __nvoc_uvm_h_disabled
static inline NV_STATUS uvmInitializeAccessCntrBuffer(OBJGPU *pGpu, struct OBJUVM *pUvm, struct AccessCounterBuffer *pAccessCounterBuffer) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_uvm_h_disabled
#define uvmInitializeAccessCntrBuffer(pGpu, pUvm, pAccessCounterBuffer) uvmInitializeAccessCntrBuffer_IMPL(pGpu, pUvm, pAccessCounterBuffer)
#endif //__nvoc_uvm_h_disabled

#define uvmInitializeAccessCntrBuffer_HAL(pGpu, pUvm, pAccessCounterBuffer) uvmInitializeAccessCntrBuffer(pGpu, pUvm, pAccessCounterBuffer)

NV_STATUS uvmTerminateAccessCntrBuffer_IMPL(OBJGPU *pGpu, struct OBJUVM *pUvm, struct AccessCounterBuffer *pAccessCounterBuffer);


#ifdef __nvoc_uvm_h_disabled
static inline NV_STATUS uvmTerminateAccessCntrBuffer(OBJGPU *pGpu, struct OBJUVM *pUvm, struct AccessCounterBuffer *pAccessCounterBuffer) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_uvm_h_disabled
#define uvmTerminateAccessCntrBuffer(pGpu, pUvm, pAccessCounterBuffer) uvmTerminateAccessCntrBuffer_IMPL(pGpu, pUvm, pAccessCounterBuffer)
#endif //__nvoc_uvm_h_disabled

#define uvmTerminateAccessCntrBuffer_HAL(pGpu, pUvm, pAccessCounterBuffer) uvmTerminateAccessCntrBuffer(pGpu, pUvm, pAccessCounterBuffer)

static inline NV_STATUS uvmResetAccessCntrBuffer_92bfc3(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 arg4) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS uvmResetAccessCntrBuffer_GV100(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 arg4);

NV_STATUS uvmResetAccessCntrBuffer_GB100(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 arg4);

static inline NV_STATUS uvmResetAccessCntrBuffer_395e98(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 arg4) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_uvm_h_disabled
static inline NV_STATUS uvmResetAccessCntrBuffer(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 arg4) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_uvm_h_disabled
#define uvmResetAccessCntrBuffer(pGpu, pUvm, accessCounterIndex, arg4) uvmResetAccessCntrBuffer_92bfc3(pGpu, pUvm, accessCounterIndex, arg4)
#endif //__nvoc_uvm_h_disabled

#define uvmResetAccessCntrBuffer_HAL(pGpu, pUvm, accessCounterIndex, arg4) uvmResetAccessCntrBuffer(pGpu, pUvm, accessCounterIndex, arg4)

void uvmStateDestroy_IMPL(OBJGPU *pGpu, struct OBJUVM *pUvm);

NV_STATUS uvmStateInitUnlocked_IMPL(OBJGPU *pGpu, struct OBJUVM *pUvm);

NV_STATUS uvmInitAccessCntrBuffer_GV100(OBJGPU *pGpu, struct OBJUVM *pUvm, struct AccessCounterBuffer *pAccessCounterBuffer);

static inline NV_STATUS uvmInitAccessCntrBuffer_ac1694(OBJGPU *pGpu, struct OBJUVM *pUvm, struct AccessCounterBuffer *pAccessCounterBuffer) {
    return NV_OK;
}

NV_STATUS uvmDestroyAccessCntrBuffer_GV100(OBJGPU *pGpu, struct OBJUVM *pUvm, struct AccessCounterBuffer *pAccessCounterBuffer);

static inline NV_STATUS uvmDestroyAccessCntrBuffer_ac1694(OBJGPU *pGpu, struct OBJUVM *pUvm, struct AccessCounterBuffer *pAccessCounterBuffer) {
    return NV_OK;
}

NV_STATUS uvmAccessCntrBufferUnregister_IMPL(OBJGPU *arg1, struct OBJUVM *arg2, NvU32 accessCounterIndex);

static inline NV_STATUS uvmAccessCntrBufferUnregister_ac1694(OBJGPU *arg1, struct OBJUVM *arg2, NvU32 accessCounterIndex) {
    return NV_OK;
}

NV_STATUS uvmAccessCntrBufferRegister_IMPL(OBJGPU *arg1, struct OBJUVM *arg2, NvU32 accessCounterIndex, NvU32 arg4, RmPhysAddr *arg5);

static inline NV_STATUS uvmAccessCntrBufferRegister_ac1694(OBJGPU *arg1, struct OBJUVM *arg2, NvU32 accessCounterIndex, NvU32 arg4, RmPhysAddr *arg5) {
    return NV_OK;
}

NV_STATUS uvmUnloadAccessCntrBuffer_GV100(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex);

static inline NV_STATUS uvmUnloadAccessCntrBuffer_ac1694(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    return NV_OK;
}

NV_STATUS uvmSetupAccessCntrBuffer_GV100(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex);

static inline NV_STATUS uvmSetupAccessCntrBuffer_ac1694(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    return NV_OK;
}

NV_STATUS uvmReadAccessCntrBufferPutPtr_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 *arg4);

static inline NV_STATUS uvmReadAccessCntrBufferPutPtr_395e98(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 *arg4) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS uvmReadAccessCntrBufferGetPtr_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 *arg4);

static inline NV_STATUS uvmReadAccessCntrBufferGetPtr_395e98(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 *arg4) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS uvmReadAccessCntrBufferFullPtr_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvBool *arg4);

static inline NV_STATUS uvmReadAccessCntrBufferFullPtr_395e98(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvBool *arg4) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS uvmAccessCntrSetGranularity_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, ACCESS_CNTR_TYPE arg4, NvU32 arg5);

static inline NV_STATUS uvmAccessCntrSetGranularity_395e98(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, ACCESS_CNTR_TYPE arg4, NvU32 arg5) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS uvmAccessCntrSetThreshold_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 arg4);

static inline NV_STATUS uvmAccessCntrSetThreshold_395e98(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 arg4) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS uvmAccessCntrSetCounterLimit_GV100(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 arg4, NvU32 arg5);

static inline NV_STATUS uvmAccessCntrSetCounterLimit_395e98(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 arg4, NvU32 arg5) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS uvmWriteAccessCntrBufferGetPtr_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 arg4);

static inline NV_STATUS uvmWriteAccessCntrBufferGetPtr_395e98(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 arg4) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS uvmEnableAccessCntr_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvBool arg4);

static inline NV_STATUS uvmEnableAccessCntr_395e98(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvBool arg4) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS uvmDisableAccessCntr_GV100(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvBool arg4);

static inline NV_STATUS uvmDisableAccessCntr_395e98(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvBool arg4) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS uvmEnableAccessCntrIntr_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 arg3);

static inline NV_STATUS uvmEnableAccessCntrIntr_395e98(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 arg3) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS uvmDisableAccessCntrIntr_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm);

static inline NV_STATUS uvmDisableAccessCntrIntr_395e98(OBJGPU *pGpu, struct OBJUVM *pUvm) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS uvmGetAccessCntrRegisterMappings_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvP64 *arg4, NvP64 *arg5, NvP64 *arg6, NvP64 *arg7, NvP64 *arg8, NvP64 *arg9, NvU32 *arg10);

static inline NV_STATUS uvmGetAccessCntrRegisterMappings_395e98(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvP64 *arg4, NvP64 *arg5, NvP64 *arg6, NvP64 *arg7, NvP64 *arg8, NvP64 *arg9, NvU32 *arg10) {
    return NV_ERR_NOT_SUPPORTED;
}

void uvmRegisterIntrService_IMPL(OBJGPU *arg1, struct OBJUVM *pUvm, IntrServiceRecord arg3[179]);

NvU32 uvmServiceInterrupt_IMPL(OBJGPU *arg1, struct OBJUVM *pUvm, IntrServiceServiceInterruptArguments *arg3);

NV_STATUS uvmAccessCntrService_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm);

static inline NV_STATUS uvmAccessCntrService_395e98(OBJGPU *pGpu, struct OBJUVM *pUvm) {
    return NV_ERR_NOT_SUPPORTED;
}

NvU32 uvmGetAccessCounterBufferSize_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex);

static inline NvU32 uvmGetAccessCounterBufferSize_b2b553(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    return 0;
}

void uvmProgramWriteAccessCntrBufferAddress_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU64 addr);

static inline void uvmProgramWriteAccessCntrBufferAddress_f2d351(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU64 addr) {
    NV_ASSERT_PRECOMP(0);
}

void uvmProgramAccessCntrBufferEnabled_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvBool bEn);

static inline void uvmProgramAccessCntrBufferEnabled_f2d351(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvBool bEn) {
    NV_ASSERT_PRECOMP(0);
}

NvBool uvmIsAccessCntrBufferEnabled_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex);

static inline NvBool uvmIsAccessCntrBufferEnabled_72a2e1(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    NV_ASSERT_PRECOMP(0);
    return NV_FALSE;
}

NvBool uvmIsAccessCntrBufferPushed_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex);

static inline NvBool uvmIsAccessCntrBufferPushed_72a2e1(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    NV_ASSERT_PRECOMP(0);
    return NV_FALSE;
}

NvU32 uvmGetRegOffsetAccessCntrBufferPut_TU102(struct OBJUVM *pUvm, NvU32 accessCounterIndex);

NvU32 uvmGetRegOffsetAccessCntrBufferPut_GB100(struct OBJUVM *pUvm, NvU32 accessCounterIndex);

static inline NvU32 uvmGetRegOffsetAccessCntrBufferPut_b2b553(struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    return 0;
}

NvU32 uvmGetRegOffsetAccessCntrBufferGet_TU102(struct OBJUVM *pUvm, NvU32 accessCounterIndex);

NvU32 uvmGetRegOffsetAccessCntrBufferGet_GB100(struct OBJUVM *pUvm, NvU32 accessCounterIndex);

static inline NvU32 uvmGetRegOffsetAccessCntrBufferGet_b2b553(struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    return 0;
}

NvU32 uvmGetRegOffsetAccessCntrBufferHi_TU102(struct OBJUVM *pUvm, NvU32 accessCounterIndex);

NvU32 uvmGetRegOffsetAccessCntrBufferHi_GB100(struct OBJUVM *pUvm, NvU32 accessCounterIndex);

static inline NvU32 uvmGetRegOffsetAccessCntrBufferHi_b2b553(struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    return 0;
}

NvU32 uvmGetRegOffsetAccessCntrBufferLo_TU102(struct OBJUVM *pUvm, NvU32 accessCounterIndex);

NvU32 uvmGetRegOffsetAccessCntrBufferLo_GB100(struct OBJUVM *pUvm, NvU32 accessCounterIndex);

static inline NvU32 uvmGetRegOffsetAccessCntrBufferLo_b2b553(struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    return 0;
}

NvU32 uvmGetRegOffsetAccessCntrBufferConfig_TU102(struct OBJUVM *pUvm, NvU32 accessCounterIndex);

NvU32 uvmGetRegOffsetAccessCntrBufferConfig_GB100(struct OBJUVM *pUvm, NvU32 accessCounterIndex);

static inline NvU32 uvmGetRegOffsetAccessCntrBufferConfig_b2b553(struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    return 0;
}

NvU32 uvmGetRegOffsetAccessCntrBufferInfo_TU102(struct OBJUVM *pUvm, NvU32 accessCounterIndex);

NvU32 uvmGetRegOffsetAccessCntrBufferInfo_GB100(struct OBJUVM *pUvm, NvU32 accessCounterIndex);

static inline NvU32 uvmGetRegOffsetAccessCntrBufferInfo_b2b553(struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    return 0;
}

NvU32 uvmGetRegOffsetAccessCntrBufferSize_TU102(struct OBJUVM *pUvm, NvU32 accessCounterIndex);

NvU32 uvmGetRegOffsetAccessCntrBufferSize_GB100(struct OBJUVM *pUvm, NvU32 accessCounterIndex);

static inline NvU32 uvmGetRegOffsetAccessCntrBufferSize_b2b553(struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    return 0;
}

#undef PRIVATE_FIELD


#endif // UVM_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_UVM_NVOC_H_
