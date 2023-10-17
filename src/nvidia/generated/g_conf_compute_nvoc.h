#ifndef _G_CONF_COMPUTE_NVOC_H_
#define _G_CONF_COMPUTE_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_conf_compute_nvoc.h"

#ifndef CONF_COMPUTE_H
#define CONF_COMPUTE_H

#include "gpu/gpu.h"
#include "gpu/eng_state.h"
#include "gpu/spdm/spdm.h"
#include "ctrl/ctrl2080/ctrl2080spdm.h"
#include "ctrl/ctrl2080/ctrl2080internal.h"
#include "cc_drv.h"
#include "conf_compute/cc_keystore.h"
#include "kernel/gpu/fifo/kernel_channel.h"
#include "kernel/gpu/intr/engine_idx.h"
#include "kernel/gpu/conf_compute/ccsl_context.h"

/****************************************************************************\
 *                                                                           *
 *      Confidential Compute module header.                                  *
 *                                                                           *
 ****************************************************************************/

#ifdef NVOC_CONF_COMPUTE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct ConfidentialCompute {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct ConfidentialCompute *__nvoc_pbase_ConfidentialCompute;
    NV_STATUS (*__confComputeConstructEngine__)(struct OBJGPU *, struct ConfidentialCompute *, ENGDESCRIPTOR);
    NV_STATUS (*__confComputeStatePreInitLocked__)(struct OBJGPU *, struct ConfidentialCompute *);
    NV_STATUS (*__confComputeStateInitLocked__)(struct OBJGPU *, struct ConfidentialCompute *);
    NV_STATUS (*__confComputeStatePostLoad__)(struct OBJGPU *, struct ConfidentialCompute *, NvU32);
    NV_STATUS (*__confComputeStatePreUnload__)(struct OBJGPU *, struct ConfidentialCompute *, NvU32);
    void (*__confComputeStateDestroy__)(struct OBJGPU *, struct ConfidentialCompute *);
    NV_STATUS (*__confComputeKeyStoreRetrieveViaChannel__)(struct ConfidentialCompute *, struct KernelChannel *, ROTATE_IV_TYPE, NvBool, CC_KMB *);
    NV_STATUS (*__confComputeKeyStoreRetrieveViaKeyId__)(struct ConfidentialCompute *, NvU32, ROTATE_IV_TYPE, NvBool, CC_KMB *);
    NV_STATUS (*__confComputeDeriveSecrets__)(struct ConfidentialCompute *, NvU32);
    NvBool (*__confComputeIsSpdmEnabled__)(struct OBJGPU *, struct ConfidentialCompute *);
    NvBool (*__confComputeIsDebugModeEnabled__)(struct OBJGPU *, struct ConfidentialCompute *);
    NvBool (*__confComputeIsGpuCcCapable__)(struct OBJGPU *, struct ConfidentialCompute *);
    void (*__confComputeKeyStoreDepositIvMask__)(struct ConfidentialCompute *, NvU32, void *);
    NV_STATUS (*__confComputeKeyStoreInit__)(struct ConfidentialCompute *);
    void (*__confComputeKeyStoreDeinit__)(struct ConfidentialCompute *);
    void *(*__confComputeKeyStoreGetExportMasterKey__)(struct ConfidentialCompute *);
    NV_STATUS (*__confComputeKeyStoreDeriveKey__)(struct ConfidentialCompute *, NvU32);
    void (*__confComputeKeyStoreClearExportMasterKey__)(struct ConfidentialCompute *);
    NV_STATUS (*__confComputeKeyStoreUpdateKey__)(struct ConfidentialCompute *, NvU32);
    NV_STATUS (*__confComputeStateLoad__)(POBJGPU, struct ConfidentialCompute *, NvU32);
    NV_STATUS (*__confComputeStateUnload__)(POBJGPU, struct ConfidentialCompute *, NvU32);
    NV_STATUS (*__confComputeStatePreLoad__)(POBJGPU, struct ConfidentialCompute *, NvU32);
    NV_STATUS (*__confComputeStatePostUnload__)(POBJGPU, struct ConfidentialCompute *, NvU32);
    NV_STATUS (*__confComputeStateInitUnlocked__)(POBJGPU, struct ConfidentialCompute *);
    void (*__confComputeInitMissing__)(POBJGPU, struct ConfidentialCompute *);
    NV_STATUS (*__confComputeStatePreInitUnlocked__)(POBJGPU, struct ConfidentialCompute *);
    NvBool (*__confComputeIsPresent__)(POBJGPU, struct ConfidentialCompute *);
    NvBool PDB_PROP_CONFCOMPUTE_ENABLED;
    NvBool PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED;
    NvBool PDB_PROP_CONFCOMPUTE_APM_FEATURE_ENABLED;
    NvBool PDB_PROP_CONFCOMPUTE_DEVTOOLS_MODE_ENABLED;
    NvBool PDB_PROP_CONFCOMPUTE_ENABLE_EARLY_INIT;
    NvBool PDB_PROP_CONFCOMPUTE_GPUS_READY_CHECK_ENABLED;
    NvBool PDB_PROP_CONFCOMPUTE_SPDM_ENABLED;
    NvBool PDB_PROP_CONFCOMPUTE_ENCRYPT_READY;
    NvBool PDB_PROP_CONFCOMPUTE_ENCRYPT_ENABLED;
    NvU32 gspProxyRegkeys;
    struct Spdm *pSpdm;
    NV2080_CTRL_INTERNAL_CONF_COMPUTE_GET_STATIC_INFO_PARAMS ccStaticInfo;
    struct ccslContext_t *pRpcCcslCtx;
    struct ccslContext_t *pDmaCcslCtx;
    struct ccslContext_t *pNonReplayableFaultCcslCtx;
    struct ccslContext_t *pReplayableFaultCcslCtx;
    PORT_SPINLOCK *pAesEngineLock;
    NvU8 PRIVATE_FIELD(m_exportMasterKey)[32];
    void *PRIVATE_FIELD(m_keySlot);
};

#ifndef __NVOC_CLASS_ConfidentialCompute_TYPEDEF__
#define __NVOC_CLASS_ConfidentialCompute_TYPEDEF__
typedef struct ConfidentialCompute ConfidentialCompute;
#endif /* __NVOC_CLASS_ConfidentialCompute_TYPEDEF__ */

#ifndef __nvoc_class_id_ConfidentialCompute
#define __nvoc_class_id_ConfidentialCompute 0x9798cc
#endif /* __nvoc_class_id_ConfidentialCompute */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ConfidentialCompute;

#define __staticCast_ConfidentialCompute(pThis) \
    ((pThis)->__nvoc_pbase_ConfidentialCompute)

#ifdef __nvoc_conf_compute_h_disabled
#define __dynamicCast_ConfidentialCompute(pThis) ((ConfidentialCompute*)NULL)
#else //__nvoc_conf_compute_h_disabled
#define __dynamicCast_ConfidentialCompute(pThis) \
    ((ConfidentialCompute*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(ConfidentialCompute)))
#endif //__nvoc_conf_compute_h_disabled

#define PDB_PROP_CONFCOMPUTE_ENCRYPT_ENABLED_BASE_CAST
#define PDB_PROP_CONFCOMPUTE_ENCRYPT_ENABLED_BASE_NAME PDB_PROP_CONFCOMPUTE_ENCRYPT_ENABLED
#define PDB_PROP_CONFCOMPUTE_APM_FEATURE_ENABLED_BASE_CAST
#define PDB_PROP_CONFCOMPUTE_APM_FEATURE_ENABLED_BASE_NAME PDB_PROP_CONFCOMPUTE_APM_FEATURE_ENABLED
#define PDB_PROP_CONFCOMPUTE_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_CONFCOMPUTE_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING
#define PDB_PROP_CONFCOMPUTE_ENABLE_EARLY_INIT_BASE_CAST
#define PDB_PROP_CONFCOMPUTE_ENABLE_EARLY_INIT_BASE_NAME PDB_PROP_CONFCOMPUTE_ENABLE_EARLY_INIT
#define PDB_PROP_CONFCOMPUTE_ENCRYPT_READY_BASE_CAST
#define PDB_PROP_CONFCOMPUTE_ENCRYPT_READY_BASE_NAME PDB_PROP_CONFCOMPUTE_ENCRYPT_READY
#define PDB_PROP_CONFCOMPUTE_GPUS_READY_CHECK_ENABLED_BASE_CAST
#define PDB_PROP_CONFCOMPUTE_GPUS_READY_CHECK_ENABLED_BASE_NAME PDB_PROP_CONFCOMPUTE_GPUS_READY_CHECK_ENABLED
#define PDB_PROP_CONFCOMPUTE_ENABLED_BASE_CAST
#define PDB_PROP_CONFCOMPUTE_ENABLED_BASE_NAME PDB_PROP_CONFCOMPUTE_ENABLED
#define PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED_BASE_CAST
#define PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED_BASE_NAME PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED
#define PDB_PROP_CONFCOMPUTE_DEVTOOLS_MODE_ENABLED_BASE_CAST
#define PDB_PROP_CONFCOMPUTE_DEVTOOLS_MODE_ENABLED_BASE_NAME PDB_PROP_CONFCOMPUTE_DEVTOOLS_MODE_ENABLED
#define PDB_PROP_CONFCOMPUTE_SPDM_ENABLED_BASE_CAST
#define PDB_PROP_CONFCOMPUTE_SPDM_ENABLED_BASE_NAME PDB_PROP_CONFCOMPUTE_SPDM_ENABLED

NV_STATUS __nvoc_objCreateDynamic_ConfidentialCompute(ConfidentialCompute**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_ConfidentialCompute(ConfidentialCompute**, Dynamic*, NvU32);
#define __objCreate_ConfidentialCompute(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_ConfidentialCompute((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define confComputeConstructEngine(pGpu, pConfCompute, engDesc) confComputeConstructEngine_DISPATCH(pGpu, pConfCompute, engDesc)
#define confComputeStatePreInitLocked(pGpu, pConfCompute) confComputeStatePreInitLocked_DISPATCH(pGpu, pConfCompute)
#define confComputeStatePreInitLocked_HAL(pGpu, pConfCompute) confComputeStatePreInitLocked_DISPATCH(pGpu, pConfCompute)
#define confComputeStateInitLocked(pGpu, pConfCompute) confComputeStateInitLocked_DISPATCH(pGpu, pConfCompute)
#define confComputeStatePostLoad(pGpu, pConfCompute, flags) confComputeStatePostLoad_DISPATCH(pGpu, pConfCompute, flags)
#define confComputeStatePostLoad_HAL(pGpu, pConfCompute, flags) confComputeStatePostLoad_DISPATCH(pGpu, pConfCompute, flags)
#define confComputeStatePreUnload(pGpu, pConfCompute, flags) confComputeStatePreUnload_DISPATCH(pGpu, pConfCompute, flags)
#define confComputeStatePreUnload_HAL(pGpu, pConfCompute, flags) confComputeStatePreUnload_DISPATCH(pGpu, pConfCompute, flags)
#define confComputeStateDestroy(pGpu, pConfCompute) confComputeStateDestroy_DISPATCH(pGpu, pConfCompute)
#define confComputeStateDestroy_HAL(pGpu, pConfCompute) confComputeStateDestroy_DISPATCH(pGpu, pConfCompute)
#define confComputeKeyStoreRetrieveViaChannel(pConfCompute, pKernelChannel, rotateOperation, includeSecrets, keyMaterialBundle) confComputeKeyStoreRetrieveViaChannel_DISPATCH(pConfCompute, pKernelChannel, rotateOperation, includeSecrets, keyMaterialBundle)
#define confComputeKeyStoreRetrieveViaChannel_HAL(pConfCompute, pKernelChannel, rotateOperation, includeSecrets, keyMaterialBundle) confComputeKeyStoreRetrieveViaChannel_DISPATCH(pConfCompute, pKernelChannel, rotateOperation, includeSecrets, keyMaterialBundle)
#define confComputeKeyStoreRetrieveViaKeyId(pConfCompute, globalKeyId, rotateOperation, includeSecrets, keyMaterialBundle) confComputeKeyStoreRetrieveViaKeyId_DISPATCH(pConfCompute, globalKeyId, rotateOperation, includeSecrets, keyMaterialBundle)
#define confComputeKeyStoreRetrieveViaKeyId_HAL(pConfCompute, globalKeyId, rotateOperation, includeSecrets, keyMaterialBundle) confComputeKeyStoreRetrieveViaKeyId_DISPATCH(pConfCompute, globalKeyId, rotateOperation, includeSecrets, keyMaterialBundle)
#define confComputeDeriveSecrets(pConfCompute, engine) confComputeDeriveSecrets_DISPATCH(pConfCompute, engine)
#define confComputeDeriveSecrets_HAL(pConfCompute, engine) confComputeDeriveSecrets_DISPATCH(pConfCompute, engine)
#define confComputeIsSpdmEnabled(pGpu, pConfCompute) confComputeIsSpdmEnabled_DISPATCH(pGpu, pConfCompute)
#define confComputeIsSpdmEnabled_HAL(pGpu, pConfCompute) confComputeIsSpdmEnabled_DISPATCH(pGpu, pConfCompute)
#define confComputeIsDebugModeEnabled(pGpu, pConfCompute) confComputeIsDebugModeEnabled_DISPATCH(pGpu, pConfCompute)
#define confComputeIsDebugModeEnabled_HAL(pGpu, pConfCompute) confComputeIsDebugModeEnabled_DISPATCH(pGpu, pConfCompute)
#define confComputeIsGpuCcCapable(pGpu, pConfCompute) confComputeIsGpuCcCapable_DISPATCH(pGpu, pConfCompute)
#define confComputeIsGpuCcCapable_HAL(pGpu, pConfCompute) confComputeIsGpuCcCapable_DISPATCH(pGpu, pConfCompute)
#define confComputeKeyStoreDepositIvMask(pConfCompute, globalKeyId, ivMask) confComputeKeyStoreDepositIvMask_DISPATCH(pConfCompute, globalKeyId, ivMask)
#define confComputeKeyStoreDepositIvMask_HAL(pConfCompute, globalKeyId, ivMask) confComputeKeyStoreDepositIvMask_DISPATCH(pConfCompute, globalKeyId, ivMask)
#define confComputeKeyStoreInit(pConfCompute) confComputeKeyStoreInit_DISPATCH(pConfCompute)
#define confComputeKeyStoreInit_HAL(pConfCompute) confComputeKeyStoreInit_DISPATCH(pConfCompute)
#define confComputeKeyStoreDeinit(pConfCompute) confComputeKeyStoreDeinit_DISPATCH(pConfCompute)
#define confComputeKeyStoreDeinit_HAL(pConfCompute) confComputeKeyStoreDeinit_DISPATCH(pConfCompute)
#define confComputeKeyStoreGetExportMasterKey(pConfCompute) confComputeKeyStoreGetExportMasterKey_DISPATCH(pConfCompute)
#define confComputeKeyStoreGetExportMasterKey_HAL(pConfCompute) confComputeKeyStoreGetExportMasterKey_DISPATCH(pConfCompute)
#define confComputeKeyStoreDeriveKey(pConfCompute, globalKeyId) confComputeKeyStoreDeriveKey_DISPATCH(pConfCompute, globalKeyId)
#define confComputeKeyStoreDeriveKey_HAL(pConfCompute, globalKeyId) confComputeKeyStoreDeriveKey_DISPATCH(pConfCompute, globalKeyId)
#define confComputeKeyStoreClearExportMasterKey(pConfCompute) confComputeKeyStoreClearExportMasterKey_DISPATCH(pConfCompute)
#define confComputeKeyStoreClearExportMasterKey_HAL(pConfCompute) confComputeKeyStoreClearExportMasterKey_DISPATCH(pConfCompute)
#define confComputeKeyStoreUpdateKey(pConfCompute, globalKeyId) confComputeKeyStoreUpdateKey_DISPATCH(pConfCompute, globalKeyId)
#define confComputeKeyStoreUpdateKey_HAL(pConfCompute, globalKeyId) confComputeKeyStoreUpdateKey_DISPATCH(pConfCompute, globalKeyId)
#define confComputeStateLoad(pGpu, pEngstate, arg0) confComputeStateLoad_DISPATCH(pGpu, pEngstate, arg0)
#define confComputeStateUnload(pGpu, pEngstate, arg0) confComputeStateUnload_DISPATCH(pGpu, pEngstate, arg0)
#define confComputeStatePreLoad(pGpu, pEngstate, arg0) confComputeStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define confComputeStatePostUnload(pGpu, pEngstate, arg0) confComputeStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define confComputeStateInitUnlocked(pGpu, pEngstate) confComputeStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define confComputeInitMissing(pGpu, pEngstate) confComputeInitMissing_DISPATCH(pGpu, pEngstate)
#define confComputeStatePreInitUnlocked(pGpu, pEngstate) confComputeStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define confComputeIsPresent(pGpu, pEngstate) confComputeIsPresent_DISPATCH(pGpu, pEngstate)
static inline NV_STATUS confComputeEarlyInit_56cd7a(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    return NV_OK;
}


#ifdef __nvoc_conf_compute_h_disabled
static inline NV_STATUS confComputeEarlyInit(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    NV_ASSERT_FAILED_PRECOMP("ConfidentialCompute was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_conf_compute_h_disabled
#define confComputeEarlyInit(pGpu, pConfCompute) confComputeEarlyInit_56cd7a(pGpu, pConfCompute)
#endif //__nvoc_conf_compute_h_disabled

#define confComputeEarlyInit_HAL(pGpu, pConfCompute) confComputeEarlyInit(pGpu, pConfCompute)

NV_STATUS confComputeStartEncryption_KERNEL(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute);


#ifdef __nvoc_conf_compute_h_disabled
static inline NV_STATUS confComputeStartEncryption(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    NV_ASSERT_FAILED_PRECOMP("ConfidentialCompute was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_conf_compute_h_disabled
#define confComputeStartEncryption(pGpu, pConfCompute) confComputeStartEncryption_KERNEL(pGpu, pConfCompute)
#endif //__nvoc_conf_compute_h_disabled

#define confComputeStartEncryption_HAL(pGpu, pConfCompute) confComputeStartEncryption(pGpu, pConfCompute)

NV_STATUS confComputeStopEncryption_KERNEL(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute);


#ifdef __nvoc_conf_compute_h_disabled
static inline NV_STATUS confComputeStopEncryption(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    NV_ASSERT_FAILED_PRECOMP("ConfidentialCompute was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_conf_compute_h_disabled
#define confComputeStopEncryption(pGpu, pConfCompute) confComputeStopEncryption_KERNEL(pGpu, pConfCompute)
#endif //__nvoc_conf_compute_h_disabled

#define confComputeStopEncryption_HAL(pGpu, pConfCompute) confComputeStopEncryption(pGpu, pConfCompute)

NV_STATUS confComputeEstablishSpdmSessionAndKeys_KERNEL(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute);


#ifdef __nvoc_conf_compute_h_disabled
static inline NV_STATUS confComputeEstablishSpdmSessionAndKeys(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    NV_ASSERT_FAILED_PRECOMP("ConfidentialCompute was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_conf_compute_h_disabled
#define confComputeEstablishSpdmSessionAndKeys(pGpu, pConfCompute) confComputeEstablishSpdmSessionAndKeys_KERNEL(pGpu, pConfCompute)
#endif //__nvoc_conf_compute_h_disabled

#define confComputeEstablishSpdmSessionAndKeys_HAL(pGpu, pConfCompute) confComputeEstablishSpdmSessionAndKeys(pGpu, pConfCompute)

NV_STATUS confComputeConstructEngine_IMPL(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, ENGDESCRIPTOR engDesc);

static inline NV_STATUS confComputeConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, ENGDESCRIPTOR engDesc) {
    return pConfCompute->__confComputeConstructEngine__(pGpu, pConfCompute, engDesc);
}

NV_STATUS confComputeStatePreInitLocked_IMPL(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute);

static inline NV_STATUS confComputeStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    return pConfCompute->__confComputeStatePreInitLocked__(pGpu, pConfCompute);
}

NV_STATUS confComputeStateInitLocked_IMPL(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute);

static inline NV_STATUS confComputeStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    return pConfCompute->__confComputeStateInitLocked__(pGpu, pConfCompute);
}

NV_STATUS confComputeStatePostLoad_IMPL(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvU32 flags);

static inline NV_STATUS confComputeStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvU32 flags) {
    return pConfCompute->__confComputeStatePostLoad__(pGpu, pConfCompute, flags);
}

static inline NV_STATUS confComputeStatePreUnload_56cd7a(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvU32 flags) {
    return NV_OK;
}

static inline NV_STATUS confComputeStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvU32 flags) {
    return pConfCompute->__confComputeStatePreUnload__(pGpu, pConfCompute, flags);
}

void confComputeStateDestroy_IMPL(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute);

static inline void confComputeStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    pConfCompute->__confComputeStateDestroy__(pGpu, pConfCompute);
}

NV_STATUS confComputeKeyStoreRetrieveViaChannel_GH100(struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation, NvBool includeSecrets, CC_KMB *keyMaterialBundle);

static inline NV_STATUS confComputeKeyStoreRetrieveViaChannel_46f6a7(struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation, NvBool includeSecrets, CC_KMB *keyMaterialBundle) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS confComputeKeyStoreRetrieveViaChannel_DISPATCH(struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation, NvBool includeSecrets, CC_KMB *keyMaterialBundle) {
    return pConfCompute->__confComputeKeyStoreRetrieveViaChannel__(pConfCompute, pKernelChannel, rotateOperation, includeSecrets, keyMaterialBundle);
}

NV_STATUS confComputeKeyStoreRetrieveViaKeyId_GH100(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId, ROTATE_IV_TYPE rotateOperation, NvBool includeSecrets, CC_KMB *keyMaterialBundle);

static inline NV_STATUS confComputeKeyStoreRetrieveViaKeyId_46f6a7(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId, ROTATE_IV_TYPE rotateOperation, NvBool includeSecrets, CC_KMB *keyMaterialBundle) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS confComputeKeyStoreRetrieveViaKeyId_DISPATCH(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId, ROTATE_IV_TYPE rotateOperation, NvBool includeSecrets, CC_KMB *keyMaterialBundle) {
    return pConfCompute->__confComputeKeyStoreRetrieveViaKeyId__(pConfCompute, globalKeyId, rotateOperation, includeSecrets, keyMaterialBundle);
}

NV_STATUS confComputeDeriveSecrets_GH100(struct ConfidentialCompute *pConfCompute, NvU32 engine);

static inline NV_STATUS confComputeDeriveSecrets_46f6a7(struct ConfidentialCompute *pConfCompute, NvU32 engine) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS confComputeDeriveSecrets_DISPATCH(struct ConfidentialCompute *pConfCompute, NvU32 engine) {
    return pConfCompute->__confComputeDeriveSecrets__(pConfCompute, engine);
}

static inline NvBool confComputeIsSpdmEnabled_cbe027(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    return ((NvBool)(0 == 0));
}

static inline NvBool confComputeIsSpdmEnabled_491d52(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    return ((NvBool)(0 != 0));
}

static inline NvBool confComputeIsSpdmEnabled_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    return pConfCompute->__confComputeIsSpdmEnabled__(pGpu, pConfCompute);
}

NvBool confComputeIsDebugModeEnabled_GH100(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute);

static inline NvBool confComputeIsDebugModeEnabled_491d52(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    return ((NvBool)(0 != 0));
}

static inline NvBool confComputeIsDebugModeEnabled_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    return pConfCompute->__confComputeIsDebugModeEnabled__(pGpu, pConfCompute);
}

NvBool confComputeIsGpuCcCapable_GH100(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute);

static inline NvBool confComputeIsGpuCcCapable_491d52(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    return ((NvBool)(0 != 0));
}

static inline NvBool confComputeIsGpuCcCapable_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    return pConfCompute->__confComputeIsGpuCcCapable__(pGpu, pConfCompute);
}

void confComputeKeyStoreDepositIvMask_GH100(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId, void *ivMask);

static inline void confComputeKeyStoreDepositIvMask_b3696a(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId, void *ivMask) {
    return;
}

static inline void confComputeKeyStoreDepositIvMask_DISPATCH(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId, void *ivMask) {
    pConfCompute->__confComputeKeyStoreDepositIvMask__(pConfCompute, globalKeyId, ivMask);
}

NV_STATUS confComputeKeyStoreInit_GH100(struct ConfidentialCompute *pConfCompute);

static inline NV_STATUS confComputeKeyStoreInit_46f6a7(struct ConfidentialCompute *pConfCompute) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS confComputeKeyStoreInit_DISPATCH(struct ConfidentialCompute *pConfCompute) {
    return pConfCompute->__confComputeKeyStoreInit__(pConfCompute);
}

void confComputeKeyStoreDeinit_GH100(struct ConfidentialCompute *pConfCompute);

static inline void confComputeKeyStoreDeinit_b3696a(struct ConfidentialCompute *pConfCompute) {
    return;
}

static inline void confComputeKeyStoreDeinit_DISPATCH(struct ConfidentialCompute *pConfCompute) {
    pConfCompute->__confComputeKeyStoreDeinit__(pConfCompute);
}

void *confComputeKeyStoreGetExportMasterKey_GH100(struct ConfidentialCompute *pConfCompute);

static inline void *confComputeKeyStoreGetExportMasterKey_fa6e19(struct ConfidentialCompute *pConfCompute) {
    return ((void *)0);
}

static inline void *confComputeKeyStoreGetExportMasterKey_DISPATCH(struct ConfidentialCompute *pConfCompute) {
    return pConfCompute->__confComputeKeyStoreGetExportMasterKey__(pConfCompute);
}

NV_STATUS confComputeKeyStoreDeriveKey_GH100(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId);

static inline NV_STATUS confComputeKeyStoreDeriveKey_46f6a7(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS confComputeKeyStoreDeriveKey_DISPATCH(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId) {
    return pConfCompute->__confComputeKeyStoreDeriveKey__(pConfCompute, globalKeyId);
}

void confComputeKeyStoreClearExportMasterKey_GH100(struct ConfidentialCompute *pConfCompute);

static inline void confComputeKeyStoreClearExportMasterKey_b3696a(struct ConfidentialCompute *pConfCompute) {
    return;
}

static inline void confComputeKeyStoreClearExportMasterKey_DISPATCH(struct ConfidentialCompute *pConfCompute) {
    pConfCompute->__confComputeKeyStoreClearExportMasterKey__(pConfCompute);
}

NV_STATUS confComputeKeyStoreUpdateKey_GH100(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId);

static inline NV_STATUS confComputeKeyStoreUpdateKey_46f6a7(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS confComputeKeyStoreUpdateKey_DISPATCH(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId) {
    return pConfCompute->__confComputeKeyStoreUpdateKey__(pConfCompute, globalKeyId);
}

static inline NV_STATUS confComputeStateLoad_DISPATCH(POBJGPU pGpu, struct ConfidentialCompute *pEngstate, NvU32 arg0) {
    return pEngstate->__confComputeStateLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS confComputeStateUnload_DISPATCH(POBJGPU pGpu, struct ConfidentialCompute *pEngstate, NvU32 arg0) {
    return pEngstate->__confComputeStateUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS confComputeStatePreLoad_DISPATCH(POBJGPU pGpu, struct ConfidentialCompute *pEngstate, NvU32 arg0) {
    return pEngstate->__confComputeStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS confComputeStatePostUnload_DISPATCH(POBJGPU pGpu, struct ConfidentialCompute *pEngstate, NvU32 arg0) {
    return pEngstate->__confComputeStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS confComputeStateInitUnlocked_DISPATCH(POBJGPU pGpu, struct ConfidentialCompute *pEngstate) {
    return pEngstate->__confComputeStateInitUnlocked__(pGpu, pEngstate);
}

static inline void confComputeInitMissing_DISPATCH(POBJGPU pGpu, struct ConfidentialCompute *pEngstate) {
    pEngstate->__confComputeInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS confComputeStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct ConfidentialCompute *pEngstate) {
    return pEngstate->__confComputeStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NvBool confComputeIsPresent_DISPATCH(POBJGPU pGpu, struct ConfidentialCompute *pEngstate) {
    return pEngstate->__confComputeIsPresent__(pGpu, pEngstate);
}

NvBool confComputeAcceptClientRequest_IMPL(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute);

#ifdef __nvoc_conf_compute_h_disabled
static inline NvBool confComputeAcceptClientRequest(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    NV_ASSERT_FAILED_PRECOMP("ConfidentialCompute was disabled!");
    return NV_FALSE;
}
#else //__nvoc_conf_compute_h_disabled
#define confComputeAcceptClientRequest(pGpu, pConfCompute) confComputeAcceptClientRequest_IMPL(pGpu, pConfCompute)
#endif //__nvoc_conf_compute_h_disabled

#undef PRIVATE_FIELD

#ifndef NVOC_CONF_COMPUTE_H_PRIVATE_ACCESS_ALLOWED
#undef confComputeKeyStoreInit
NV_STATUS NVOC_PRIVATE_FUNCTION(confComputeKeyStoreInit)(struct ConfidentialCompute *pConfCompute);

#undef confComputeKeyStoreInit_HAL
NV_STATUS NVOC_PRIVATE_FUNCTION(confComputeKeyStoreInit_HAL)(struct ConfidentialCompute *pConfCompute);

#undef confComputeKeyStoreDeinit
void NVOC_PRIVATE_FUNCTION(confComputeKeyStoreDeinit)(struct ConfidentialCompute *pConfCompute);

#undef confComputeKeyStoreDeinit_HAL
void NVOC_PRIVATE_FUNCTION(confComputeKeyStoreDeinit_HAL)(struct ConfidentialCompute *pConfCompute);

#undef confComputeKeyStoreGetExportMasterKey
void *NVOC_PRIVATE_FUNCTION(confComputeKeyStoreGetExportMasterKey)(struct ConfidentialCompute *pConfCompute);

#undef confComputeKeyStoreGetExportMasterKey_HAL
void *NVOC_PRIVATE_FUNCTION(confComputeKeyStoreGetExportMasterKey_HAL)(struct ConfidentialCompute *pConfCompute);

#undef confComputeKeyStoreDeriveKey
NV_STATUS NVOC_PRIVATE_FUNCTION(confComputeKeyStoreDeriveKey)(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId);

#undef confComputeKeyStoreDeriveKey_HAL
NV_STATUS NVOC_PRIVATE_FUNCTION(confComputeKeyStoreDeriveKey_HAL)(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId);

#undef confComputeKeyStoreClearExportMasterKey
void NVOC_PRIVATE_FUNCTION(confComputeKeyStoreClearExportMasterKey)(struct ConfidentialCompute *pConfCompute);

#undef confComputeKeyStoreClearExportMasterKey_HAL
void NVOC_PRIVATE_FUNCTION(confComputeKeyStoreClearExportMasterKey_HAL)(struct ConfidentialCompute *pConfCompute);

#undef confComputeKeyStoreUpdateKey
NV_STATUS NVOC_PRIVATE_FUNCTION(confComputeKeyStoreUpdateKey)(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId);

#undef confComputeKeyStoreUpdateKey_HAL
NV_STATUS NVOC_PRIVATE_FUNCTION(confComputeKeyStoreUpdateKey_HAL)(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId);

#endif // NVOC_CONF_COMPUTE_H_PRIVATE_ACCESS_ALLOWED


// Refactor spdmCtrlSpdmPartition into SPDM module.

/*!
 * @brief spdmCtrlSpdmPartition
 *        Common function used to call gspCommandPostBlocking based on the platform on which it runs (i.e. vGPU, GSP-RM, Monolithic).
 *
 * @param[in]     pGpu                     : OBJGPU Pointer
 * @param[in]     pSpdmPartitionParams     : SPDM RPC structure pointer
 */
NV_STATUS spdmCtrlSpdmPartition(struct OBJGPU *pGpu, NV2080_CTRL_INTERNAL_SPDM_PARTITION_PARAMS *pSpdmPartitionParams);

// spdmCtrlWriteSharedMemory is a common function called from both Physical-RM and Kernel-RM.
// However, SPDM module is disabled on Phyiscal-RM and hence declared in conf_compute.h.

#endif // CONF_COMPUTE_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_CONF_COMPUTE_NVOC_H_
