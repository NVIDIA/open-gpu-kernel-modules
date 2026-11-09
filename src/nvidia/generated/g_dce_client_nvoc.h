
#ifndef _G_DCE_CLIENT_NVOC_H_
#define _G_DCE_CLIENT_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2020-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_dce_client_nvoc.h"

#ifndef _DCE_CLIENT_H_
#define _DCE_CLIENT_H_

/*!
 * @file   dce_client.h
 * @brief  Provides definitions for all DceClient data structures and interfaces.
 */

#include "gpu/eng_state.h"
#include "core/core.h"
#include "gpu/rpc/objrpc.h"
#include "os/dce_rm_client_ipc.h"

/*!
 * Temporary alias of DceClient to OBJDCECLIENTRM
 */
#define DceClient OBJDCECLIENTRM

/*!
 * Defines the structure used to contain all generic information related to
 * the DceClient.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_DCE_CLIENT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__OBJDCECLIENTRM;
struct NVOC_METADATA__OBJENGSTATE;


struct OBJDCECLIENTRM {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__OBJDCECLIENTRM *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct OBJDCECLIENTRM *__nvoc_pbase_OBJDCECLIENTRM;    // dceclient

    // 1 PDB property
//  NvBool PDB_PROP_DCECLIENT_IS_MISSING inherited from OBJENGSTATE

    // Data members
    struct OBJRPC *pRpc;
    NvU32 clientId[2];
    NvU32 hInternalClient;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__OBJDCECLIENTRM {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
};

#ifndef __nvoc_class_id_OBJDCECLIENTRM
#define __nvoc_class_id_OBJDCECLIENTRM 0x61649cu
typedef struct OBJDCECLIENTRM OBJDCECLIENTRM;
#endif /* __nvoc_class_id_OBJDCECLIENTRM */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJDCECLIENTRM;

#define __staticCast_OBJDCECLIENTRM(pThis) \
    ((pThis)->__nvoc_pbase_OBJDCECLIENTRM)

#ifdef __nvoc_dce_client_h_disabled
#define __dynamicCast_OBJDCECLIENTRM(pThis) ((OBJDCECLIENTRM*) NULL)
#else //__nvoc_dce_client_h_disabled
#define __dynamicCast_OBJDCECLIENTRM(pThis) \
    ((OBJDCECLIENTRM*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJDCECLIENTRM)))
#endif //__nvoc_dce_client_h_disabled

// Property macros
#define PDB_PROP_DCECLIENT_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_DCECLIENT_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING


NV_STATUS __nvoc_objCreateDynamic_OBJDCECLIENTRM(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJDCECLIENTRM(OBJDCECLIENTRM**, Dynamic*, NvU32);
#define __objCreate_OBJDCECLIENTRM(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags) \
    __nvoc_objCreate_OBJDCECLIENTRM((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags))


// Wrapper macros for implementation functions
void dceclientDestruct_IMPL(struct OBJDCECLIENTRM *arg_this);
#define __nvoc_dceclientDestruct(arg_this) dceclientDestruct_IMPL(arg_this)

NV_STATUS dceclientInitRpcInfra_IMPL(struct OBJGPU *arg1, struct OBJDCECLIENTRM *arg_this);
#ifdef __nvoc_dce_client_h_disabled
static inline NV_STATUS dceclientInitRpcInfra(struct OBJGPU *arg1, struct OBJDCECLIENTRM *arg_this) {
    NV_ASSERT_FAILED_PRECOMP("OBJDCECLIENTRM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_dce_client_h_disabled
#define dceclientInitRpcInfra(arg1, arg_this) dceclientInitRpcInfra_IMPL(arg1, arg_this)
#endif // __nvoc_dce_client_h_disabled

void dceclientDeinitRpcInfra_IMPL(struct OBJDCECLIENTRM *arg_this);
#ifdef __nvoc_dce_client_h_disabled
static inline void dceclientDeinitRpcInfra(struct OBJDCECLIENTRM *arg_this) {
    NV_ASSERT_FAILED_PRECOMP("OBJDCECLIENTRM was disabled!");
}
#else // __nvoc_dce_client_h_disabled
#define dceclientDeinitRpcInfra(arg_this) dceclientDeinitRpcInfra_IMPL(arg_this)
#endif // __nvoc_dce_client_h_disabled

NV_STATUS dceclientSendRmInitRpc_IMPL(struct OBJGPU *arg1, struct OBJDCECLIENTRM *arg_this, NvBool arg3, NvBool arg4);
#ifdef __nvoc_dce_client_h_disabled
static inline NV_STATUS dceclientSendRmInitRpc(struct OBJGPU *arg1, struct OBJDCECLIENTRM *arg_this, NvBool arg3, NvBool arg4) {
    NV_ASSERT_FAILED_PRECOMP("OBJDCECLIENTRM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_dce_client_h_disabled
#define dceclientSendRmInitRpc(arg1, arg_this, arg3, arg4) dceclientSendRmInitRpc_IMPL(arg1, arg_this, arg3, arg4)
#endif // __nvoc_dce_client_h_disabled

NV_STATUS dceclientDceRmInit_IMPL(struct OBJGPU *arg1, struct OBJDCECLIENTRM *arg_this, NvBool arg3);
#ifdef __nvoc_dce_client_h_disabled
static inline NV_STATUS dceclientDceRmInit(struct OBJGPU *arg1, struct OBJDCECLIENTRM *arg_this, NvBool arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJDCECLIENTRM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_dce_client_h_disabled
#define dceclientDceRmInit(arg1, arg_this, arg3) dceclientDceRmInit_IMPL(arg1, arg_this, arg3)
#endif // __nvoc_dce_client_h_disabled

NV_STATUS dceclientSendRpc_IMPL(struct OBJDCECLIENTRM *arg_this, void *arg2, NvU32 arg3);
#ifdef __nvoc_dce_client_h_disabled
static inline NV_STATUS dceclientSendRpc(struct OBJDCECLIENTRM *arg_this, void *arg2, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJDCECLIENTRM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_dce_client_h_disabled
#define dceclientSendRpc(arg_this, arg2, arg3) dceclientSendRpc_IMPL(arg_this, arg2, arg3)
#endif // __nvoc_dce_client_h_disabled


// Wrapper macros for halified functions
#define dceclientConstructEngine_FNPTR(arg_this) arg_this->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateConstructEngine__
#define dceclientConstructEngine(arg1, arg_this, arg3) dceclientConstructEngine_DISPATCH(arg1, arg_this, arg3)
#define dceclientStateDestroy_FNPTR(arg_this) arg_this->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateDestroy__
#define dceclientStateDestroy(arg1, arg_this) dceclientStateDestroy_DISPATCH(arg1, arg_this)
#define dceclientStateLoad_FNPTR(arg_this) arg_this->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateLoad__
#define dceclientStateLoad(arg1, arg_this, arg3) dceclientStateLoad_DISPATCH(arg1, arg_this, arg3)
#define dceclientStateUnload_FNPTR(arg_this) arg_this->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateUnload__
#define dceclientStateUnload(arg1, arg_this, arg3) dceclientStateUnload_DISPATCH(arg1, arg_this, arg3)
#define dceclientConstructApertures_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateConstructApertures__
#define dceclientConstructApertures(pGpu, pEngstate) dceclientConstructApertures_DISPATCH(pGpu, pEngstate)
#define dceclientDestructApertures_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateDestructApertures__
#define dceclientDestructApertures(pGpu, pEngstate) dceclientDestructApertures_DISPATCH(pGpu, pEngstate)
#define dceclientInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define dceclientInitMissing(pGpu, pEngstate) dceclientInitMissing_DISPATCH(pGpu, pEngstate)
#define dceclientStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__
#define dceclientStatePreInitLocked(pGpu, pEngstate) dceclientStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define dceclientStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define dceclientStatePreInitUnlocked(pGpu, pEngstate) dceclientStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define dceclientStateInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitLocked__
#define dceclientStateInitLocked(pGpu, pEngstate) dceclientStateInitLocked_DISPATCH(pGpu, pEngstate)
#define dceclientStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__
#define dceclientStateInitUnlocked(pGpu, pEngstate) dceclientStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define dceclientStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__
#define dceclientStatePreLoad(pGpu, pEngstate, arg3) dceclientStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define dceclientStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostLoad__
#define dceclientStatePostLoad(pGpu, pEngstate, arg3) dceclientStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define dceclientStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__
#define dceclientStatePreUnload(pGpu, pEngstate, arg3) dceclientStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define dceclientStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__
#define dceclientStatePostUnload(pGpu, pEngstate, arg3) dceclientStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define dceclientIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__
#define dceclientIsPresent(pGpu, pEngstate) dceclientIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS dceclientConstructEngine_DISPATCH(struct OBJGPU *arg1, struct OBJDCECLIENTRM *arg_this, ENGDESCRIPTOR arg3) {
    return arg_this->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateConstructEngine__(arg1, staticCast(arg_this, OBJENGSTATE), arg3);
}

static inline void dceclientStateDestroy_DISPATCH(struct OBJGPU *arg1, struct OBJDCECLIENTRM *arg_this) {
    arg_this->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateDestroy__(arg1, staticCast(arg_this, OBJENGSTATE));
}

static inline NV_STATUS dceclientStateLoad_DISPATCH(struct OBJGPU *arg1, struct OBJDCECLIENTRM *arg_this, NvU32 arg3) {
    return arg_this->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateLoad__(arg1, staticCast(arg_this, OBJENGSTATE), arg3);
}

static inline NV_STATUS dceclientStateUnload_DISPATCH(struct OBJGPU *arg1, struct OBJDCECLIENTRM *arg_this, NvU32 arg3) {
    return arg_this->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateUnload__(arg1, staticCast(arg_this, OBJENGSTATE), arg3);
}

static inline NV_STATUS dceclientConstructApertures_DISPATCH(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateConstructApertures__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline void dceclientDestructApertures_DISPATCH(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate) {
    pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateDestructApertures__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline void dceclientInitMissing_DISPATCH(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate) {
    pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NV_STATUS dceclientStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NV_STATUS dceclientStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NV_STATUS dceclientStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitLocked__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NV_STATUS dceclientStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NV_STATUS dceclientStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NV_STATUS dceclientStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostLoad__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NV_STATUS dceclientStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NV_STATUS dceclientStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NvBool dceclientIsPresent_DISPATCH(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

// Virtual method declarations and/or inline definitions
NV_STATUS dceclientConstructEngine_IMPL(struct OBJGPU *arg1, struct OBJDCECLIENTRM *arg_this, ENGDESCRIPTOR arg3);

void dceclientStateDestroy_IMPL(struct OBJGPU *arg1, struct OBJDCECLIENTRM *arg_this);

NV_STATUS dceclientStateLoad_IMPL(struct OBJGPU *arg1, struct OBJDCECLIENTRM *arg_this, NvU32 arg3);

NV_STATUS dceclientStateUnload_IMPL(struct OBJGPU *arg1, struct OBJDCECLIENTRM *arg_this, NvU32 arg3);

// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


NV_STATUS rpcRmApiControl_dce(RM_API *pRmApi,
                            NvHandle hClient, NvHandle hObject,
                            NvU32 cmd, void *pParamStructPtr,
                            NvU32 paramsSize);
NV_STATUS rpcRmApiAlloc_dce(RM_API *pRmApi,
                            NvHandle hClient, NvHandle hParent,
                            NvHandle hObject, NvU32 hClass,
                            void *pAllocParams, NvU32 allocParamsSize);
NV_STATUS rpcRmApiDupObject_dce(RM_API *pRmApi, NvHandle hClient,
                                NvHandle hParent, NvHandle *phObject, NvHandle hClientSrc,
                                NvHandle hObjectSrc, NvU32 flags);
NV_STATUS rpcRmApiFree_dce(RM_API *pRmApi, NvHandle hClient, NvHandle hObject);
NV_STATUS rpcDceRmInit_dce(RM_API *pRmApi, NvBool bInit, NvBool bFromResume);
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_DCE_CLIENT_NVOC_H_
