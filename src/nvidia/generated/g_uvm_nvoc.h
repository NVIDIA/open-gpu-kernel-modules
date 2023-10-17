#ifndef _G_UVM_NVOC_H_
#define _G_UVM_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

typedef struct OBJUVM *POBJUVM;

#ifdef NVOC_UVM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct OBJUVM {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct IntrService __nvoc_base_IntrService;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct IntrService *__nvoc_pbase_IntrService;
    struct OBJUVM *__nvoc_pbase_OBJUVM;
    void (*__uvmStateDestroy__)(OBJGPU *, struct OBJUVM *);
    NV_STATUS (*__uvmStateInitUnlocked__)(OBJGPU *, struct OBJUVM *);
    void (*__uvmRegisterIntrService__)(OBJGPU *, struct OBJUVM *, IntrServiceRecord *);
    NvU32 (*__uvmServiceInterrupt__)(OBJGPU *, struct OBJUVM *, IntrServiceServiceInterruptArguments *);
    NV_STATUS (*__uvmStateLoad__)(POBJGPU, struct OBJUVM *, NvU32);
    NV_STATUS (*__uvmStateUnload__)(POBJGPU, struct OBJUVM *, NvU32);
    NV_STATUS (*__uvmServiceNotificationInterrupt__)(OBJGPU *, struct OBJUVM *, IntrServiceServiceNotificationInterruptArguments *);
    NV_STATUS (*__uvmStateInitLocked__)(POBJGPU, struct OBJUVM *);
    NV_STATUS (*__uvmStatePreLoad__)(POBJGPU, struct OBJUVM *, NvU32);
    NV_STATUS (*__uvmStatePostUnload__)(POBJGPU, struct OBJUVM *, NvU32);
    NV_STATUS (*__uvmStatePreUnload__)(POBJGPU, struct OBJUVM *, NvU32);
    void (*__uvmInitMissing__)(POBJGPU, struct OBJUVM *);
    NV_STATUS (*__uvmStatePreInitLocked__)(POBJGPU, struct OBJUVM *);
    NV_STATUS (*__uvmStatePreInitUnlocked__)(POBJGPU, struct OBJUVM *);
    NvBool (*__uvmClearInterrupt__)(OBJGPU *, struct OBJUVM *, IntrServiceClearInterruptArguments *);
    NV_STATUS (*__uvmStatePostLoad__)(POBJGPU, struct OBJUVM *, NvU32);
    NV_STATUS (*__uvmConstructEngine__)(POBJGPU, struct OBJUVM *, ENGDESCRIPTOR);
    NvBool (*__uvmIsPresent__)(POBJGPU, struct OBJUVM *);
    ACCESS_CNTR_BUFFER *pAccessCounterBuffers;
    NvU32 accessCounterBufferCount;
    NvHandle hClient;
    NvHandle hSubdevice;
    RM_API *pRmApi;
};

#ifndef __NVOC_CLASS_OBJUVM_TYPEDEF__
#define __NVOC_CLASS_OBJUVM_TYPEDEF__
typedef struct OBJUVM OBJUVM;
#endif /* __NVOC_CLASS_OBJUVM_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJUVM
#define __nvoc_class_id_OBJUVM 0xf9a17d
#endif /* __nvoc_class_id_OBJUVM */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJUVM;

#define __staticCast_OBJUVM(pThis) \
    ((pThis)->__nvoc_pbase_OBJUVM)

#ifdef __nvoc_uvm_h_disabled
#define __dynamicCast_OBJUVM(pThis) ((OBJUVM*)NULL)
#else //__nvoc_uvm_h_disabled
#define __dynamicCast_OBJUVM(pThis) \
    ((OBJUVM*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJUVM)))
#endif //__nvoc_uvm_h_disabled

#define PDB_PROP_UVM_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_UVM_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_OBJUVM(OBJUVM**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJUVM(OBJUVM**, Dynamic*, NvU32);
#define __objCreate_OBJUVM(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJUVM((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define uvmStateDestroy(pGpu, pUvm) uvmStateDestroy_DISPATCH(pGpu, pUvm)
#define uvmStateInitUnlocked(pGpu, pUvm) uvmStateInitUnlocked_DISPATCH(pGpu, pUvm)
#define uvmRegisterIntrService(arg0, pUvm, arg1) uvmRegisterIntrService_DISPATCH(arg0, pUvm, arg1)
#define uvmServiceInterrupt(arg0, pUvm, arg1) uvmServiceInterrupt_DISPATCH(arg0, pUvm, arg1)
#define uvmStateLoad(pGpu, pEngstate, arg0) uvmStateLoad_DISPATCH(pGpu, pEngstate, arg0)
#define uvmStateUnload(pGpu, pEngstate, arg0) uvmStateUnload_DISPATCH(pGpu, pEngstate, arg0)
#define uvmServiceNotificationInterrupt(pGpu, pIntrService, pParams) uvmServiceNotificationInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define uvmStateInitLocked(pGpu, pEngstate) uvmStateInitLocked_DISPATCH(pGpu, pEngstate)
#define uvmStatePreLoad(pGpu, pEngstate, arg0) uvmStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define uvmStatePostUnload(pGpu, pEngstate, arg0) uvmStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define uvmStatePreUnload(pGpu, pEngstate, arg0) uvmStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define uvmInitMissing(pGpu, pEngstate) uvmInitMissing_DISPATCH(pGpu, pEngstate)
#define uvmStatePreInitLocked(pGpu, pEngstate) uvmStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define uvmStatePreInitUnlocked(pGpu, pEngstate) uvmStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define uvmClearInterrupt(pGpu, pIntrService, pParams) uvmClearInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define uvmStatePostLoad(pGpu, pEngstate, arg0) uvmStatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define uvmConstructEngine(pGpu, pEngstate, arg0) uvmConstructEngine_DISPATCH(pGpu, pEngstate, arg0)
#define uvmIsPresent(pGpu, pEngstate) uvmIsPresent_DISPATCH(pGpu, pEngstate)
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

NV_STATUS uvmInitAccessCntrBuffer_GV100(OBJGPU *pGpu, struct OBJUVM *pUvm, struct AccessCounterBuffer *pAccessCounterBuffer);


#ifdef __nvoc_uvm_h_disabled
static inline NV_STATUS uvmInitAccessCntrBuffer(OBJGPU *pGpu, struct OBJUVM *pUvm, struct AccessCounterBuffer *pAccessCounterBuffer) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_uvm_h_disabled
#define uvmInitAccessCntrBuffer(pGpu, pUvm, pAccessCounterBuffer) uvmInitAccessCntrBuffer_GV100(pGpu, pUvm, pAccessCounterBuffer)
#endif //__nvoc_uvm_h_disabled

#define uvmInitAccessCntrBuffer_HAL(pGpu, pUvm, pAccessCounterBuffer) uvmInitAccessCntrBuffer(pGpu, pUvm, pAccessCounterBuffer)

NV_STATUS uvmDestroyAccessCntrBuffer_GV100(OBJGPU *pGpu, struct OBJUVM *pUvm, struct AccessCounterBuffer *pAccessCounterBuffer);


#ifdef __nvoc_uvm_h_disabled
static inline NV_STATUS uvmDestroyAccessCntrBuffer(OBJGPU *pGpu, struct OBJUVM *pUvm, struct AccessCounterBuffer *pAccessCounterBuffer) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_uvm_h_disabled
#define uvmDestroyAccessCntrBuffer(pGpu, pUvm, pAccessCounterBuffer) uvmDestroyAccessCntrBuffer_GV100(pGpu, pUvm, pAccessCounterBuffer)
#endif //__nvoc_uvm_h_disabled

#define uvmDestroyAccessCntrBuffer_HAL(pGpu, pUvm, pAccessCounterBuffer) uvmDestroyAccessCntrBuffer(pGpu, pUvm, pAccessCounterBuffer)

static inline NV_STATUS uvmAccessCntrBufferUnregister_ac1694(OBJGPU *arg0, struct OBJUVM *arg1, NvU32 accessCounterIndex) {
    return NV_OK;
}


#ifdef __nvoc_uvm_h_disabled
static inline NV_STATUS uvmAccessCntrBufferUnregister(OBJGPU *arg0, struct OBJUVM *arg1, NvU32 accessCounterIndex) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_uvm_h_disabled
#define uvmAccessCntrBufferUnregister(arg0, arg1, accessCounterIndex) uvmAccessCntrBufferUnregister_ac1694(arg0, arg1, accessCounterIndex)
#endif //__nvoc_uvm_h_disabled

#define uvmAccessCntrBufferUnregister_HAL(arg0, arg1, accessCounterIndex) uvmAccessCntrBufferUnregister(arg0, arg1, accessCounterIndex)

static inline NV_STATUS uvmAccessCntrBufferRegister_ac1694(OBJGPU *arg0, struct OBJUVM *arg1, NvU32 accessCounterIndex, NvU32 arg2, RmPhysAddr *arg3) {
    return NV_OK;
}


#ifdef __nvoc_uvm_h_disabled
static inline NV_STATUS uvmAccessCntrBufferRegister(OBJGPU *arg0, struct OBJUVM *arg1, NvU32 accessCounterIndex, NvU32 arg2, RmPhysAddr *arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_uvm_h_disabled
#define uvmAccessCntrBufferRegister(arg0, arg1, accessCounterIndex, arg2, arg3) uvmAccessCntrBufferRegister_ac1694(arg0, arg1, accessCounterIndex, arg2, arg3)
#endif //__nvoc_uvm_h_disabled

#define uvmAccessCntrBufferRegister_HAL(arg0, arg1, accessCounterIndex, arg2, arg3) uvmAccessCntrBufferRegister(arg0, arg1, accessCounterIndex, arg2, arg3)

NV_STATUS uvmUnloadAccessCntrBuffer_GV100(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex);


#ifdef __nvoc_uvm_h_disabled
static inline NV_STATUS uvmUnloadAccessCntrBuffer(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_uvm_h_disabled
#define uvmUnloadAccessCntrBuffer(pGpu, pUvm, accessCounterIndex) uvmUnloadAccessCntrBuffer_GV100(pGpu, pUvm, accessCounterIndex)
#endif //__nvoc_uvm_h_disabled

#define uvmUnloadAccessCntrBuffer_HAL(pGpu, pUvm, accessCounterIndex) uvmUnloadAccessCntrBuffer(pGpu, pUvm, accessCounterIndex)

NV_STATUS uvmSetupAccessCntrBuffer_GV100(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex);


#ifdef __nvoc_uvm_h_disabled
static inline NV_STATUS uvmSetupAccessCntrBuffer(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_uvm_h_disabled
#define uvmSetupAccessCntrBuffer(pGpu, pUvm, accessCounterIndex) uvmSetupAccessCntrBuffer_GV100(pGpu, pUvm, accessCounterIndex)
#endif //__nvoc_uvm_h_disabled

#define uvmSetupAccessCntrBuffer_HAL(pGpu, pUvm, accessCounterIndex) uvmSetupAccessCntrBuffer(pGpu, pUvm, accessCounterIndex)

NV_STATUS uvmReadAccessCntrBufferPutPtr_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 *arg0);


#ifdef __nvoc_uvm_h_disabled
static inline NV_STATUS uvmReadAccessCntrBufferPutPtr(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_uvm_h_disabled
#define uvmReadAccessCntrBufferPutPtr(pGpu, pUvm, accessCounterIndex, arg0) uvmReadAccessCntrBufferPutPtr_TU102(pGpu, pUvm, accessCounterIndex, arg0)
#endif //__nvoc_uvm_h_disabled

#define uvmReadAccessCntrBufferPutPtr_HAL(pGpu, pUvm, accessCounterIndex, arg0) uvmReadAccessCntrBufferPutPtr(pGpu, pUvm, accessCounterIndex, arg0)

NV_STATUS uvmReadAccessCntrBufferGetPtr_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 *arg0);


#ifdef __nvoc_uvm_h_disabled
static inline NV_STATUS uvmReadAccessCntrBufferGetPtr(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_uvm_h_disabled
#define uvmReadAccessCntrBufferGetPtr(pGpu, pUvm, accessCounterIndex, arg0) uvmReadAccessCntrBufferGetPtr_TU102(pGpu, pUvm, accessCounterIndex, arg0)
#endif //__nvoc_uvm_h_disabled

#define uvmReadAccessCntrBufferGetPtr_HAL(pGpu, pUvm, accessCounterIndex, arg0) uvmReadAccessCntrBufferGetPtr(pGpu, pUvm, accessCounterIndex, arg0)

NV_STATUS uvmReadAccessCntrBufferFullPtr_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvBool *arg0);


#ifdef __nvoc_uvm_h_disabled
static inline NV_STATUS uvmReadAccessCntrBufferFullPtr(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvBool *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_uvm_h_disabled
#define uvmReadAccessCntrBufferFullPtr(pGpu, pUvm, accessCounterIndex, arg0) uvmReadAccessCntrBufferFullPtr_TU102(pGpu, pUvm, accessCounterIndex, arg0)
#endif //__nvoc_uvm_h_disabled

#define uvmReadAccessCntrBufferFullPtr_HAL(pGpu, pUvm, accessCounterIndex, arg0) uvmReadAccessCntrBufferFullPtr(pGpu, pUvm, accessCounterIndex, arg0)

NV_STATUS uvmResetAccessCntrBuffer_GV100(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 arg0);


#ifdef __nvoc_uvm_h_disabled
static inline NV_STATUS uvmResetAccessCntrBuffer(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_uvm_h_disabled
#define uvmResetAccessCntrBuffer(pGpu, pUvm, accessCounterIndex, arg0) uvmResetAccessCntrBuffer_GV100(pGpu, pUvm, accessCounterIndex, arg0)
#endif //__nvoc_uvm_h_disabled

#define uvmResetAccessCntrBuffer_HAL(pGpu, pUvm, accessCounterIndex, arg0) uvmResetAccessCntrBuffer(pGpu, pUvm, accessCounterIndex, arg0)

NV_STATUS uvmAccessCntrSetGranularity_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, ACCESS_CNTR_TYPE arg0, NvU32 arg1);


#ifdef __nvoc_uvm_h_disabled
static inline NV_STATUS uvmAccessCntrSetGranularity(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, ACCESS_CNTR_TYPE arg0, NvU32 arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_uvm_h_disabled
#define uvmAccessCntrSetGranularity(pGpu, pUvm, accessCounterIndex, arg0, arg1) uvmAccessCntrSetGranularity_TU102(pGpu, pUvm, accessCounterIndex, arg0, arg1)
#endif //__nvoc_uvm_h_disabled

#define uvmAccessCntrSetGranularity_HAL(pGpu, pUvm, accessCounterIndex, arg0, arg1) uvmAccessCntrSetGranularity(pGpu, pUvm, accessCounterIndex, arg0, arg1)

NV_STATUS uvmAccessCntrSetThreshold_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 arg0);


#ifdef __nvoc_uvm_h_disabled
static inline NV_STATUS uvmAccessCntrSetThreshold(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_uvm_h_disabled
#define uvmAccessCntrSetThreshold(pGpu, pUvm, accessCounterIndex, arg0) uvmAccessCntrSetThreshold_TU102(pGpu, pUvm, accessCounterIndex, arg0)
#endif //__nvoc_uvm_h_disabled

#define uvmAccessCntrSetThreshold_HAL(pGpu, pUvm, accessCounterIndex, arg0) uvmAccessCntrSetThreshold(pGpu, pUvm, accessCounterIndex, arg0)

NV_STATUS uvmAccessCntrSetCounterLimit_GV100(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 arg0, NvU32 arg1);


#ifdef __nvoc_uvm_h_disabled
static inline NV_STATUS uvmAccessCntrSetCounterLimit(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 arg0, NvU32 arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_uvm_h_disabled
#define uvmAccessCntrSetCounterLimit(pGpu, pUvm, accessCounterIndex, arg0, arg1) uvmAccessCntrSetCounterLimit_GV100(pGpu, pUvm, accessCounterIndex, arg0, arg1)
#endif //__nvoc_uvm_h_disabled

#define uvmAccessCntrSetCounterLimit_HAL(pGpu, pUvm, accessCounterIndex, arg0, arg1) uvmAccessCntrSetCounterLimit(pGpu, pUvm, accessCounterIndex, arg0, arg1)

NV_STATUS uvmWriteAccessCntrBufferGetPtr_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 arg0);


#ifdef __nvoc_uvm_h_disabled
static inline NV_STATUS uvmWriteAccessCntrBufferGetPtr(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_uvm_h_disabled
#define uvmWriteAccessCntrBufferGetPtr(pGpu, pUvm, accessCounterIndex, arg0) uvmWriteAccessCntrBufferGetPtr_TU102(pGpu, pUvm, accessCounterIndex, arg0)
#endif //__nvoc_uvm_h_disabled

#define uvmWriteAccessCntrBufferGetPtr_HAL(pGpu, pUvm, accessCounterIndex, arg0) uvmWriteAccessCntrBufferGetPtr(pGpu, pUvm, accessCounterIndex, arg0)

NV_STATUS uvmEnableAccessCntr_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvBool arg0);


#ifdef __nvoc_uvm_h_disabled
static inline NV_STATUS uvmEnableAccessCntr(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvBool arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_uvm_h_disabled
#define uvmEnableAccessCntr(pGpu, pUvm, accessCounterIndex, arg0) uvmEnableAccessCntr_TU102(pGpu, pUvm, accessCounterIndex, arg0)
#endif //__nvoc_uvm_h_disabled

#define uvmEnableAccessCntr_HAL(pGpu, pUvm, accessCounterIndex, arg0) uvmEnableAccessCntr(pGpu, pUvm, accessCounterIndex, arg0)

NV_STATUS uvmDisableAccessCntr_GV100(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvBool arg0);


#ifdef __nvoc_uvm_h_disabled
static inline NV_STATUS uvmDisableAccessCntr(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvBool arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_uvm_h_disabled
#define uvmDisableAccessCntr(pGpu, pUvm, accessCounterIndex, arg0) uvmDisableAccessCntr_GV100(pGpu, pUvm, accessCounterIndex, arg0)
#endif //__nvoc_uvm_h_disabled

#define uvmDisableAccessCntr_HAL(pGpu, pUvm, accessCounterIndex, arg0) uvmDisableAccessCntr(pGpu, pUvm, accessCounterIndex, arg0)

NV_STATUS uvmEnableAccessCntrIntr_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 arg0);


#ifdef __nvoc_uvm_h_disabled
static inline NV_STATUS uvmEnableAccessCntrIntr(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_uvm_h_disabled
#define uvmEnableAccessCntrIntr(pGpu, pUvm, arg0) uvmEnableAccessCntrIntr_TU102(pGpu, pUvm, arg0)
#endif //__nvoc_uvm_h_disabled

#define uvmEnableAccessCntrIntr_HAL(pGpu, pUvm, arg0) uvmEnableAccessCntrIntr(pGpu, pUvm, arg0)

NV_STATUS uvmDisableAccessCntrIntr_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm);


#ifdef __nvoc_uvm_h_disabled
static inline NV_STATUS uvmDisableAccessCntrIntr(OBJGPU *pGpu, struct OBJUVM *pUvm) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_uvm_h_disabled
#define uvmDisableAccessCntrIntr(pGpu, pUvm) uvmDisableAccessCntrIntr_TU102(pGpu, pUvm)
#endif //__nvoc_uvm_h_disabled

#define uvmDisableAccessCntrIntr_HAL(pGpu, pUvm) uvmDisableAccessCntrIntr(pGpu, pUvm)

NV_STATUS uvmGetAccessCntrRegisterMappings_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvP64 *arg0, NvP64 *arg1, NvP64 *arg2, NvP64 *arg3, NvP64 *arg4, NvP64 *arg5, NvU32 *arg6);


#ifdef __nvoc_uvm_h_disabled
static inline NV_STATUS uvmGetAccessCntrRegisterMappings(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvP64 *arg0, NvP64 *arg1, NvP64 *arg2, NvP64 *arg3, NvP64 *arg4, NvP64 *arg5, NvU32 *arg6) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_uvm_h_disabled
#define uvmGetAccessCntrRegisterMappings(pGpu, pUvm, accessCounterIndex, arg0, arg1, arg2, arg3, arg4, arg5, arg6) uvmGetAccessCntrRegisterMappings_TU102(pGpu, pUvm, accessCounterIndex, arg0, arg1, arg2, arg3, arg4, arg5, arg6)
#endif //__nvoc_uvm_h_disabled

#define uvmGetAccessCntrRegisterMappings_HAL(pGpu, pUvm, accessCounterIndex, arg0, arg1, arg2, arg3, arg4, arg5, arg6) uvmGetAccessCntrRegisterMappings(pGpu, pUvm, accessCounterIndex, arg0, arg1, arg2, arg3, arg4, arg5, arg6)

NV_STATUS uvmAccessCntrService_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm);


#ifdef __nvoc_uvm_h_disabled
static inline NV_STATUS uvmAccessCntrService(OBJGPU *pGpu, struct OBJUVM *pUvm) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_uvm_h_disabled
#define uvmAccessCntrService(pGpu, pUvm) uvmAccessCntrService_TU102(pGpu, pUvm)
#endif //__nvoc_uvm_h_disabled

#define uvmAccessCntrService_HAL(pGpu, pUvm) uvmAccessCntrService(pGpu, pUvm)

NvU32 uvmGetAccessCounterBufferSize_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex);


#ifdef __nvoc_uvm_h_disabled
static inline NvU32 uvmGetAccessCounterBufferSize(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return 0;
}
#else //__nvoc_uvm_h_disabled
#define uvmGetAccessCounterBufferSize(pGpu, pUvm, accessCounterIndex) uvmGetAccessCounterBufferSize_TU102(pGpu, pUvm, accessCounterIndex)
#endif //__nvoc_uvm_h_disabled

#define uvmGetAccessCounterBufferSize_HAL(pGpu, pUvm, accessCounterIndex) uvmGetAccessCounterBufferSize(pGpu, pUvm, accessCounterIndex)

void uvmProgramWriteAccessCntrBufferAddress_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU64 addr);


#ifdef __nvoc_uvm_h_disabled
static inline void uvmProgramWriteAccessCntrBufferAddress(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvU64 addr) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
}
#else //__nvoc_uvm_h_disabled
#define uvmProgramWriteAccessCntrBufferAddress(pGpu, pUvm, accessCounterIndex, addr) uvmProgramWriteAccessCntrBufferAddress_TU102(pGpu, pUvm, accessCounterIndex, addr)
#endif //__nvoc_uvm_h_disabled

#define uvmProgramWriteAccessCntrBufferAddress_HAL(pGpu, pUvm, accessCounterIndex, addr) uvmProgramWriteAccessCntrBufferAddress(pGpu, pUvm, accessCounterIndex, addr)

void uvmProgramAccessCntrBufferEnabled_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvBool bEn);


#ifdef __nvoc_uvm_h_disabled
static inline void uvmProgramAccessCntrBufferEnabled(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex, NvBool bEn) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
}
#else //__nvoc_uvm_h_disabled
#define uvmProgramAccessCntrBufferEnabled(pGpu, pUvm, accessCounterIndex, bEn) uvmProgramAccessCntrBufferEnabled_TU102(pGpu, pUvm, accessCounterIndex, bEn)
#endif //__nvoc_uvm_h_disabled

#define uvmProgramAccessCntrBufferEnabled_HAL(pGpu, pUvm, accessCounterIndex, bEn) uvmProgramAccessCntrBufferEnabled(pGpu, pUvm, accessCounterIndex, bEn)

NvBool uvmIsAccessCntrBufferEnabled_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex);


#ifdef __nvoc_uvm_h_disabled
static inline NvBool uvmIsAccessCntrBufferEnabled(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return NV_FALSE;
}
#else //__nvoc_uvm_h_disabled
#define uvmIsAccessCntrBufferEnabled(pGpu, pUvm, accessCounterIndex) uvmIsAccessCntrBufferEnabled_TU102(pGpu, pUvm, accessCounterIndex)
#endif //__nvoc_uvm_h_disabled

#define uvmIsAccessCntrBufferEnabled_HAL(pGpu, pUvm, accessCounterIndex) uvmIsAccessCntrBufferEnabled(pGpu, pUvm, accessCounterIndex)

NvBool uvmIsAccessCntrBufferPushed_TU102(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex);


#ifdef __nvoc_uvm_h_disabled
static inline NvBool uvmIsAccessCntrBufferPushed(OBJGPU *pGpu, struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return NV_FALSE;
}
#else //__nvoc_uvm_h_disabled
#define uvmIsAccessCntrBufferPushed(pGpu, pUvm, accessCounterIndex) uvmIsAccessCntrBufferPushed_TU102(pGpu, pUvm, accessCounterIndex)
#endif //__nvoc_uvm_h_disabled

#define uvmIsAccessCntrBufferPushed_HAL(pGpu, pUvm, accessCounterIndex) uvmIsAccessCntrBufferPushed(pGpu, pUvm, accessCounterIndex)

NvU32 uvmGetRegOffsetAccessCntrBufferPut_TU102(struct OBJUVM *pUvm, NvU32 accessCounterIndex);


#ifdef __nvoc_uvm_h_disabled
static inline NvU32 uvmGetRegOffsetAccessCntrBufferPut(struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return 0;
}
#else //__nvoc_uvm_h_disabled
#define uvmGetRegOffsetAccessCntrBufferPut(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferPut_TU102(pUvm, accessCounterIndex)
#endif //__nvoc_uvm_h_disabled

#define uvmGetRegOffsetAccessCntrBufferPut_HAL(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferPut(pUvm, accessCounterIndex)

NvU32 uvmGetRegOffsetAccessCntrBufferGet_TU102(struct OBJUVM *pUvm, NvU32 accessCounterIndex);


#ifdef __nvoc_uvm_h_disabled
static inline NvU32 uvmGetRegOffsetAccessCntrBufferGet(struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return 0;
}
#else //__nvoc_uvm_h_disabled
#define uvmGetRegOffsetAccessCntrBufferGet(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferGet_TU102(pUvm, accessCounterIndex)
#endif //__nvoc_uvm_h_disabled

#define uvmGetRegOffsetAccessCntrBufferGet_HAL(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferGet(pUvm, accessCounterIndex)

NvU32 uvmGetRegOffsetAccessCntrBufferHi_TU102(struct OBJUVM *pUvm, NvU32 accessCounterIndex);


#ifdef __nvoc_uvm_h_disabled
static inline NvU32 uvmGetRegOffsetAccessCntrBufferHi(struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return 0;
}
#else //__nvoc_uvm_h_disabled
#define uvmGetRegOffsetAccessCntrBufferHi(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferHi_TU102(pUvm, accessCounterIndex)
#endif //__nvoc_uvm_h_disabled

#define uvmGetRegOffsetAccessCntrBufferHi_HAL(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferHi(pUvm, accessCounterIndex)

NvU32 uvmGetRegOffsetAccessCntrBufferLo_TU102(struct OBJUVM *pUvm, NvU32 accessCounterIndex);


#ifdef __nvoc_uvm_h_disabled
static inline NvU32 uvmGetRegOffsetAccessCntrBufferLo(struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return 0;
}
#else //__nvoc_uvm_h_disabled
#define uvmGetRegOffsetAccessCntrBufferLo(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferLo_TU102(pUvm, accessCounterIndex)
#endif //__nvoc_uvm_h_disabled

#define uvmGetRegOffsetAccessCntrBufferLo_HAL(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferLo(pUvm, accessCounterIndex)

NvU32 uvmGetRegOffsetAccessCntrBufferConfig_TU102(struct OBJUVM *pUvm, NvU32 accessCounterIndex);


#ifdef __nvoc_uvm_h_disabled
static inline NvU32 uvmGetRegOffsetAccessCntrBufferConfig(struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return 0;
}
#else //__nvoc_uvm_h_disabled
#define uvmGetRegOffsetAccessCntrBufferConfig(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferConfig_TU102(pUvm, accessCounterIndex)
#endif //__nvoc_uvm_h_disabled

#define uvmGetRegOffsetAccessCntrBufferConfig_HAL(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferConfig(pUvm, accessCounterIndex)

NvU32 uvmGetRegOffsetAccessCntrBufferInfo_TU102(struct OBJUVM *pUvm, NvU32 accessCounterIndex);


#ifdef __nvoc_uvm_h_disabled
static inline NvU32 uvmGetRegOffsetAccessCntrBufferInfo(struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return 0;
}
#else //__nvoc_uvm_h_disabled
#define uvmGetRegOffsetAccessCntrBufferInfo(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferInfo_TU102(pUvm, accessCounterIndex)
#endif //__nvoc_uvm_h_disabled

#define uvmGetRegOffsetAccessCntrBufferInfo_HAL(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferInfo(pUvm, accessCounterIndex)

NvU32 uvmGetRegOffsetAccessCntrBufferSize_TU102(struct OBJUVM *pUvm, NvU32 accessCounterIndex);


#ifdef __nvoc_uvm_h_disabled
static inline NvU32 uvmGetRegOffsetAccessCntrBufferSize(struct OBJUVM *pUvm, NvU32 accessCounterIndex) {
    NV_ASSERT_FAILED_PRECOMP("OBJUVM was disabled!");
    return 0;
}
#else //__nvoc_uvm_h_disabled
#define uvmGetRegOffsetAccessCntrBufferSize(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferSize_TU102(pUvm, accessCounterIndex)
#endif //__nvoc_uvm_h_disabled

#define uvmGetRegOffsetAccessCntrBufferSize_HAL(pUvm, accessCounterIndex) uvmGetRegOffsetAccessCntrBufferSize(pUvm, accessCounterIndex)

void uvmStateDestroy_IMPL(OBJGPU *pGpu, struct OBJUVM *pUvm);

static inline void uvmStateDestroy_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm) {
    pUvm->__uvmStateDestroy__(pGpu, pUvm);
}

NV_STATUS uvmStateInitUnlocked_IMPL(OBJGPU *pGpu, struct OBJUVM *pUvm);

static inline NV_STATUS uvmStateInitUnlocked_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pUvm) {
    return pUvm->__uvmStateInitUnlocked__(pGpu, pUvm);
}

void uvmRegisterIntrService_IMPL(OBJGPU *arg0, struct OBJUVM *pUvm, IntrServiceRecord arg1[168]);

static inline void uvmRegisterIntrService_DISPATCH(OBJGPU *arg0, struct OBJUVM *pUvm, IntrServiceRecord arg1[168]) {
    pUvm->__uvmRegisterIntrService__(arg0, pUvm, arg1);
}

NvU32 uvmServiceInterrupt_IMPL(OBJGPU *arg0, struct OBJUVM *pUvm, IntrServiceServiceInterruptArguments *arg1);

static inline NvU32 uvmServiceInterrupt_DISPATCH(OBJGPU *arg0, struct OBJUVM *pUvm, IntrServiceServiceInterruptArguments *arg1) {
    return pUvm->__uvmServiceInterrupt__(arg0, pUvm, arg1);
}

static inline NV_STATUS uvmStateLoad_DISPATCH(POBJGPU pGpu, struct OBJUVM *pEngstate, NvU32 arg0) {
    return pEngstate->__uvmStateLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS uvmStateUnload_DISPATCH(POBJGPU pGpu, struct OBJUVM *pEngstate, NvU32 arg0) {
    return pEngstate->__uvmStateUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS uvmServiceNotificationInterrupt_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return pIntrService->__uvmServiceNotificationInterrupt__(pGpu, pIntrService, pParams);
}

static inline NV_STATUS uvmStateInitLocked_DISPATCH(POBJGPU pGpu, struct OBJUVM *pEngstate) {
    return pEngstate->__uvmStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS uvmStatePreLoad_DISPATCH(POBJGPU pGpu, struct OBJUVM *pEngstate, NvU32 arg0) {
    return pEngstate->__uvmStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS uvmStatePostUnload_DISPATCH(POBJGPU pGpu, struct OBJUVM *pEngstate, NvU32 arg0) {
    return pEngstate->__uvmStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS uvmStatePreUnload_DISPATCH(POBJGPU pGpu, struct OBJUVM *pEngstate, NvU32 arg0) {
    return pEngstate->__uvmStatePreUnload__(pGpu, pEngstate, arg0);
}

static inline void uvmInitMissing_DISPATCH(POBJGPU pGpu, struct OBJUVM *pEngstate) {
    pEngstate->__uvmInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS uvmStatePreInitLocked_DISPATCH(POBJGPU pGpu, struct OBJUVM *pEngstate) {
    return pEngstate->__uvmStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS uvmStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct OBJUVM *pEngstate) {
    return pEngstate->__uvmStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NvBool uvmClearInterrupt_DISPATCH(OBJGPU *pGpu, struct OBJUVM *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return pIntrService->__uvmClearInterrupt__(pGpu, pIntrService, pParams);
}

static inline NV_STATUS uvmStatePostLoad_DISPATCH(POBJGPU pGpu, struct OBJUVM *pEngstate, NvU32 arg0) {
    return pEngstate->__uvmStatePostLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS uvmConstructEngine_DISPATCH(POBJGPU pGpu, struct OBJUVM *pEngstate, ENGDESCRIPTOR arg0) {
    return pEngstate->__uvmConstructEngine__(pGpu, pEngstate, arg0);
}

static inline NvBool uvmIsPresent_DISPATCH(POBJGPU pGpu, struct OBJUVM *pEngstate) {
    return pEngstate->__uvmIsPresent__(pGpu, pEngstate);
}

#undef PRIVATE_FIELD


#endif // UVM_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_UVM_NVOC_H_
