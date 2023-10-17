#ifndef _G_DCE_CLIENT_NVOC_H_
#define _G_DCE_CLIENT_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_dce_client_nvoc.h"

#ifndef _DCE_CLIENT_H_
#define _DCE_CLIENT_H_

/*!
 * @file   dce_client.h
 * @brief  Provides definitions for all DceClient data structures and interfaces.
 */

#include "gpu/eng_state.h"
#include "core/core.h"
#include "objrpc.h"
#include "os/dce_rm_client_ipc.h"

/*!
 * Temporary alias of DceClient to OBJDCECLIENTRM
 */
#define DceClient OBJDCECLIENTRM

/*!
 * Defines the structure used to contain all generic information related to
 * the DceClient.
 */
#ifdef NVOC_DCE_CLIENT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct OBJDCECLIENTRM {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct OBJDCECLIENTRM *__nvoc_pbase_OBJDCECLIENTRM;
    NV_STATUS (*__dceclientConstructEngine__)(struct OBJGPU *, struct OBJDCECLIENTRM *, ENGDESCRIPTOR);
    void (*__dceclientStateDestroy__)(struct OBJGPU *, struct OBJDCECLIENTRM *);
    NV_STATUS (*__dceclientStateLoad__)(POBJGPU, struct OBJDCECLIENTRM *, NvU32);
    NV_STATUS (*__dceclientStateUnload__)(POBJGPU, struct OBJDCECLIENTRM *, NvU32);
    NV_STATUS (*__dceclientStateInitLocked__)(POBJGPU, struct OBJDCECLIENTRM *);
    NV_STATUS (*__dceclientStatePreLoad__)(POBJGPU, struct OBJDCECLIENTRM *, NvU32);
    NV_STATUS (*__dceclientStatePostUnload__)(POBJGPU, struct OBJDCECLIENTRM *, NvU32);
    NV_STATUS (*__dceclientStatePreUnload__)(POBJGPU, struct OBJDCECLIENTRM *, NvU32);
    NV_STATUS (*__dceclientStateInitUnlocked__)(POBJGPU, struct OBJDCECLIENTRM *);
    void (*__dceclientInitMissing__)(POBJGPU, struct OBJDCECLIENTRM *);
    NV_STATUS (*__dceclientStatePreInitLocked__)(POBJGPU, struct OBJDCECLIENTRM *);
    NV_STATUS (*__dceclientStatePreInitUnlocked__)(POBJGPU, struct OBJDCECLIENTRM *);
    NV_STATUS (*__dceclientStatePostLoad__)(POBJGPU, struct OBJDCECLIENTRM *, NvU32);
    NvBool (*__dceclientIsPresent__)(POBJGPU, struct OBJDCECLIENTRM *);
    struct OBJRPC *pRpc;
    NvU32 clientId[2];
};

#ifndef __NVOC_CLASS_OBJDCECLIENTRM_TYPEDEF__
#define __NVOC_CLASS_OBJDCECLIENTRM_TYPEDEF__
typedef struct OBJDCECLIENTRM OBJDCECLIENTRM;
#endif /* __NVOC_CLASS_OBJDCECLIENTRM_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJDCECLIENTRM
#define __nvoc_class_id_OBJDCECLIENTRM 0x61649c
#endif /* __nvoc_class_id_OBJDCECLIENTRM */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJDCECLIENTRM;

#define __staticCast_OBJDCECLIENTRM(pThis) \
    ((pThis)->__nvoc_pbase_OBJDCECLIENTRM)

#ifdef __nvoc_dce_client_h_disabled
#define __dynamicCast_OBJDCECLIENTRM(pThis) ((OBJDCECLIENTRM*)NULL)
#else //__nvoc_dce_client_h_disabled
#define __dynamicCast_OBJDCECLIENTRM(pThis) \
    ((OBJDCECLIENTRM*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJDCECLIENTRM)))
#endif //__nvoc_dce_client_h_disabled

#define PDB_PROP_DCECLIENT_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_DCECLIENT_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_OBJDCECLIENTRM(OBJDCECLIENTRM**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJDCECLIENTRM(OBJDCECLIENTRM**, Dynamic*, NvU32);
#define __objCreate_OBJDCECLIENTRM(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJDCECLIENTRM((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define dceclientConstructEngine(arg0, arg1, arg2) dceclientConstructEngine_DISPATCH(arg0, arg1, arg2)
#define dceclientStateDestroy(arg0, arg1) dceclientStateDestroy_DISPATCH(arg0, arg1)
#define dceclientStateLoad(pGpu, pEngstate, arg0) dceclientStateLoad_DISPATCH(pGpu, pEngstate, arg0)
#define dceclientStateUnload(pGpu, pEngstate, arg0) dceclientStateUnload_DISPATCH(pGpu, pEngstate, arg0)
#define dceclientStateInitLocked(pGpu, pEngstate) dceclientStateInitLocked_DISPATCH(pGpu, pEngstate)
#define dceclientStatePreLoad(pGpu, pEngstate, arg0) dceclientStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define dceclientStatePostUnload(pGpu, pEngstate, arg0) dceclientStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define dceclientStatePreUnload(pGpu, pEngstate, arg0) dceclientStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define dceclientStateInitUnlocked(pGpu, pEngstate) dceclientStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define dceclientInitMissing(pGpu, pEngstate) dceclientInitMissing_DISPATCH(pGpu, pEngstate)
#define dceclientStatePreInitLocked(pGpu, pEngstate) dceclientStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define dceclientStatePreInitUnlocked(pGpu, pEngstate) dceclientStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define dceclientStatePostLoad(pGpu, pEngstate, arg0) dceclientStatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define dceclientIsPresent(pGpu, pEngstate) dceclientIsPresent_DISPATCH(pGpu, pEngstate)
NV_STATUS dceclientConstructEngine_IMPL(struct OBJGPU *arg0, struct OBJDCECLIENTRM *arg1, ENGDESCRIPTOR arg2);

static inline NV_STATUS dceclientConstructEngine_DISPATCH(struct OBJGPU *arg0, struct OBJDCECLIENTRM *arg1, ENGDESCRIPTOR arg2) {
    return arg1->__dceclientConstructEngine__(arg0, arg1, arg2);
}

void dceclientStateDestroy_IMPL(struct OBJGPU *arg0, struct OBJDCECLIENTRM *arg1);

static inline void dceclientStateDestroy_DISPATCH(struct OBJGPU *arg0, struct OBJDCECLIENTRM *arg1) {
    arg1->__dceclientStateDestroy__(arg0, arg1);
}

static inline NV_STATUS dceclientStateLoad_DISPATCH(POBJGPU pGpu, struct OBJDCECLIENTRM *pEngstate, NvU32 arg0) {
    return pEngstate->__dceclientStateLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS dceclientStateUnload_DISPATCH(POBJGPU pGpu, struct OBJDCECLIENTRM *pEngstate, NvU32 arg0) {
    return pEngstate->__dceclientStateUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS dceclientStateInitLocked_DISPATCH(POBJGPU pGpu, struct OBJDCECLIENTRM *pEngstate) {
    return pEngstate->__dceclientStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS dceclientStatePreLoad_DISPATCH(POBJGPU pGpu, struct OBJDCECLIENTRM *pEngstate, NvU32 arg0) {
    return pEngstate->__dceclientStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS dceclientStatePostUnload_DISPATCH(POBJGPU pGpu, struct OBJDCECLIENTRM *pEngstate, NvU32 arg0) {
    return pEngstate->__dceclientStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS dceclientStatePreUnload_DISPATCH(POBJGPU pGpu, struct OBJDCECLIENTRM *pEngstate, NvU32 arg0) {
    return pEngstate->__dceclientStatePreUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS dceclientStateInitUnlocked_DISPATCH(POBJGPU pGpu, struct OBJDCECLIENTRM *pEngstate) {
    return pEngstate->__dceclientStateInitUnlocked__(pGpu, pEngstate);
}

static inline void dceclientInitMissing_DISPATCH(POBJGPU pGpu, struct OBJDCECLIENTRM *pEngstate) {
    pEngstate->__dceclientInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS dceclientStatePreInitLocked_DISPATCH(POBJGPU pGpu, struct OBJDCECLIENTRM *pEngstate) {
    return pEngstate->__dceclientStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS dceclientStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct OBJDCECLIENTRM *pEngstate) {
    return pEngstate->__dceclientStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS dceclientStatePostLoad_DISPATCH(POBJGPU pGpu, struct OBJDCECLIENTRM *pEngstate, NvU32 arg0) {
    return pEngstate->__dceclientStatePostLoad__(pGpu, pEngstate, arg0);
}

static inline NvBool dceclientIsPresent_DISPATCH(POBJGPU pGpu, struct OBJDCECLIENTRM *pEngstate) {
    return pEngstate->__dceclientIsPresent__(pGpu, pEngstate);
}

void dceclientDestruct_IMPL(struct OBJDCECLIENTRM *arg0);

#define __nvoc_dceclientDestruct(arg0) dceclientDestruct_IMPL(arg0)
NV_STATUS dceclientInitRpcInfra_IMPL(struct OBJGPU *arg0, struct OBJDCECLIENTRM *arg1);

#ifdef __nvoc_dce_client_h_disabled
static inline NV_STATUS dceclientInitRpcInfra(struct OBJGPU *arg0, struct OBJDCECLIENTRM *arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJDCECLIENTRM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_dce_client_h_disabled
#define dceclientInitRpcInfra(arg0, arg1) dceclientInitRpcInfra_IMPL(arg0, arg1)
#endif //__nvoc_dce_client_h_disabled

void dceclientDeinitRpcInfra_IMPL(struct OBJDCECLIENTRM *arg0);

#ifdef __nvoc_dce_client_h_disabled
static inline void dceclientDeinitRpcInfra(struct OBJDCECLIENTRM *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJDCECLIENTRM was disabled!");
}
#else //__nvoc_dce_client_h_disabled
#define dceclientDeinitRpcInfra(arg0) dceclientDeinitRpcInfra_IMPL(arg0)
#endif //__nvoc_dce_client_h_disabled

NV_STATUS dceclientDceRmInit_IMPL(struct OBJGPU *arg0, struct OBJDCECLIENTRM *arg1, NvBool arg2);

#ifdef __nvoc_dce_client_h_disabled
static inline NV_STATUS dceclientDceRmInit(struct OBJGPU *arg0, struct OBJDCECLIENTRM *arg1, NvBool arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJDCECLIENTRM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_dce_client_h_disabled
#define dceclientDceRmInit(arg0, arg1, arg2) dceclientDceRmInit_IMPL(arg0, arg1, arg2)
#endif //__nvoc_dce_client_h_disabled

NV_STATUS dceclientSendRpc_IMPL(struct OBJDCECLIENTRM *arg0, void *arg1, NvU32 arg2);

#ifdef __nvoc_dce_client_h_disabled
static inline NV_STATUS dceclientSendRpc(struct OBJDCECLIENTRM *arg0, void *arg1, NvU32 arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJDCECLIENTRM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_dce_client_h_disabled
#define dceclientSendRpc(arg0, arg1, arg2) dceclientSendRpc_IMPL(arg0, arg1, arg2)
#endif //__nvoc_dce_client_h_disabled

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
NV_STATUS rpcDceRmInit_dce(RM_API *pRmApi, NvBool bInit);
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_DCE_CLIENT_NVOC_H_
