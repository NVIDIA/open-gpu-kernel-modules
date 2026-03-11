
#ifndef _G_CONF_COMPUTE_NVOC_H_
#define _G_CONF_COMPUTE_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2021-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_conf_compute_nvoc.h"

#ifndef CONF_COMPUTE_H
#define CONF_COMPUTE_H

#include "gpu/gpu.h"
#include "gpu/eng_state.h"
#include "gpu/spdm/spdm.h"
#include "ctrl/ctrlc56f.h"
#include "cc_drv.h"
#include "conf_compute/cc_keystore.h"
#include "conf_compute/cc_keyrotation.h"
#include "kernel/gpu/fifo/kernel_channel.h"
#include "kernel/gpu/fifo/kernel_fifo.h"
#include "kernel/gpu/intr/engine_idx.h"
#include "kernel/gpu/conf_compute/ccsl_context.h"
#include "kernel/gpu/timer/objtmr.h"

#define CONCAT64(hi, low) ((((NvU64)(hi) << 32)) | ((NvU64)(low)))

/****************************************************************************\
 *                                                                           *
 *      Confidential Compute module header.                                  *
 *                                                                           *
 ****************************************************************************/

//
// Per-key info regarding encryption ops
typedef struct
{
    NvU64 totalBytesEncrypted;
    NvU64 totalEncryptOps;
} KEY_ROTATION_STATS_INFO;

// Info needed by workitem to perform key rotation
typedef struct
{
    NvU32 h2dKey;
    NvU32 d2hKey;
    KEY_ROTATION_STATUS status;
} KEY_ROTATION_WORKITEM_INFO;

// Info needed by timer to setup timeouts for key rotation
typedef struct
{
    TMR_EVENT *pTimer;
    NvU64      timeLeftNs; // time left before hitting timeout
} KEY_ROTATION_TIMEOUT_INFO;

typedef enum
{
    CHANNEL_IV_OPERATION_NONE = 0,               // Don't include IV and don't rotate
    CHANNEL_IV_OPERATION_INCLUDE_ONLY = 1,       // Include IV but don't rotate
    CHANNEL_IV_OPERATION_ROTATE_ONLY = 2,        // Don't include IV but rotate
    CHANNEL_IV_OPERATION_INCLUDE_AND_ROTATE = 3, // Include IV and rotate
    CHANNEL_IV_OPERATION_INVALID
} CHANNEL_IV_OPERATION;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_CONF_COMPUTE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__ConfidentialCompute;
struct NVOC_METADATA__OBJENGSTATE;
struct NVOC_VTABLE__ConfidentialCompute;


struct ConfidentialCompute {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__ConfidentialCompute *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct ConfidentialCompute *__nvoc_pbase_ConfidentialCompute;    // confCompute

    // Vtable with 40 per-object function pointers
    void (*__confComputeDestruct__)(struct ConfidentialCompute * /*this*/);  // halified (2 hals) override (engstate) base (engstate) body
    NV_STATUS (*__confComputeStatePostLoad__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/, NvU32);  // virtual halified (2 hals) override (engstate) base (engstate) body
    NV_STATUS (*__confComputeStatePreUnload__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/, NvU32);  // virtual halified (2 hals) override (engstate) base (engstate) body
    void (*__confComputeSetErrorState__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__confComputeKeyStoreDeriveViaChannel__)(struct ConfidentialCompute * /*this*/, struct KernelChannel *, ROTATE_IV_TYPE);  // halified (3 hals) body
    NV_STATUS (*__confComputeKeyStoreRetrieveViaChannel__)(struct ConfidentialCompute * /*this*/, struct KernelChannel *, ROTATE_IV_TYPE, CHANNEL_IV_OPERATION, CC_KMB *);  // halified (3 hals) body
    NV_STATUS (*__confComputeKeyStoreRetrieveViaKeyId__)(struct ConfidentialCompute * /*this*/, NvU32, ROTATE_IV_TYPE, NvBool, CC_KMB *);  // halified (2 hals) body
    NV_STATUS (*__confComputeDeriveSecretsForCEKeySpace__)(struct ConfidentialCompute * /*this*/, RM_ENGINE_TYPE, NvU32);  // halified (2 hals) body
    NV_STATUS (*__confComputeDeriveInitialKeySeed__)(struct ConfidentialCompute * /*this*/);  // halified (3 hals) body
    NV_STATUS (*__confComputeGetAndUpdateCurrentKeySeed__)(struct ConfidentialCompute * /*this*/, NvU8 *);  // halified (2 hals) body
    NV_STATUS (*__confComputeDeriveSecrets__)(struct ConfidentialCompute * /*this*/, NvU32);  // halified (3 hals) body
    NV_STATUS (*__confComputeRotationNotifHandler__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/, RM_API *, NvHandle, struct KernelChannel *, NvBool *, NvBool);  // halified (2 hals)
    NV_STATUS (*__confComputeUpdateSecrets__)(struct ConfidentialCompute * /*this*/, NvU32);  // halified (2 hals) body
    NV_STATUS (*__confComputeUpdatePerChannelSecrets__)(struct ConfidentialCompute * /*this*/, struct KernelChannel *);  // halified (2 hals) body
    NvBool (*__confComputeIsSpdmEnabled__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/);  // halified (2 hals) body
    RM_ENGINE_TYPE (*__confComputeGetEngineIdFromKeySpace__)(struct ConfidentialCompute * /*this*/, NvU32);  // halified (3 hals) body
    NV_STATUS (*__confComputeGetKeySpaceFromKChannel__)(struct ConfidentialCompute * /*this*/, struct KernelChannel *, NvU16 *);  // halified (3 hals) body
    NV_STATUS (*__confComputeGetLceKeyIdFromKChannel__)(struct ConfidentialCompute * /*this*/, struct KernelChannel *, ROTATE_IV_TYPE, NvU16 *);  // halified (3 hals) body
    NvU32 (*__confComputeGetMaxCeKeySpaceIdx__)(struct ConfidentialCompute * /*this*/);  // halified (3 hals) body
    NvBool (*__confComputeGlobalKeyIsKernelPriv__)(struct ConfidentialCompute * /*this*/, NvU32);  // halified (2 hals) body
    NvBool (*__confComputeGlobalKeyIsUvmKey__)(struct ConfidentialCompute * /*this*/, NvU32);  // halified (2 hals) body
    NV_STATUS (*__confComputeGetKeyPairByChannel__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/, struct KernelChannel *, NvU32 *, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__confComputeTriggerKeyRotation__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/);  // halified (3 hals) body
    void (*__confComputeGetKeyPairForKeySpace__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/, NvU32, NvBool, NvU32 *, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__confComputeEnableKeyRotationCallback__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/, NvBool);  // halified (3 hals) body
    NV_STATUS (*__confComputeEnableKeyRotationSupport__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/);  // halified (4 hals) body
    NV_STATUS (*__confComputeEnableInternalKeyRotationSupport__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/);  // halified (3 hals) body
    NvBool (*__confComputeIsDebugModeEnabled__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/);  // halified (2 hals) body
    NvBool (*__confComputeIsGpuCcCapable__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/);  // halified (3 hals) body
    NV_STATUS (*__confComputeTestPlatformSupport__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__confComputeDeriveSessionKeys__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/);  // halified (2 hals) body
    void (*__confComputeKeyStoreDepositIvMask__)(struct ConfidentialCompute * /*this*/, NvU32, void *);  // halified (2 hals) body
    NV_STATUS (*__confComputeKeyStoreUpdateKey__)(struct ConfidentialCompute * /*this*/, NvU32);  // halified (2 hals) body
    NvBool (*__confComputeKeyStoreIsValidGlobalKeyId__)(struct ConfidentialCompute * /*this*/, NvU32);  // halified (2 hals) body
    NV_STATUS (*__confComputeKeyStoreInit__)(struct ConfidentialCompute * /*this*/);  // halified (2 hals) body
    void (*__confComputeKeyStoreDeinit__)(struct ConfidentialCompute * /*this*/);  // halified (2 hals) body
    void * (*__confComputeKeyStoreGetExportMasterKey__)(struct ConfidentialCompute * /*this*/);  // halified (2 hals) body
    void * (*__confComputeGetCurrentKeySeed__)(struct ConfidentialCompute * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__confComputeKeyStoreDeriveKey__)(struct ConfidentialCompute * /*this*/, NvU32);  // halified (2 hals) body
    void (*__confComputeKeyStoreClearExportMasterKey__)(struct ConfidentialCompute * /*this*/);  // halified (2 hals) body

    // 13 PDB properties
//  NvBool PDB_PROP_CONFCOMPUTE_IS_MISSING inherited from OBJENGSTATE
    NvBool PDB_PROP_CONFCOMPUTE_ENABLED;
    NvBool PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED;
    NvBool PDB_PROP_CONFCOMPUTE_APM_FEATURE_ENABLED;
    NvBool PDB_PROP_CONFCOMPUTE_DEVTOOLS_MODE_ENABLED;
    NvBool PDB_PROP_CONFCOMPUTE_ENABLE_EARLY_INIT;
    NvBool PDB_PROP_CONFCOMPUTE_GPUS_READY_CHECK_ENABLED;
    NvBool PDB_PROP_CONFCOMPUTE_MULTI_GPU_PROTECTED_PCIE_MODE_ENABLED;
    NvBool PDB_PROP_CONFCOMPUTE_MULTI_GPU_NVLE_MODE_ENABLED;
    NvBool PDB_PROP_CONFCOMPUTE_KEY_ROTATION_SUPPORTED;
    NvBool PDB_PROP_CONFCOMPUTE_KEY_ROTATION_ENABLED;
    NvBool PDB_PROP_CONFCOMPUTE_INTERNAL_KEY_ROTATION_ENABLED;
    NvBool PDB_PROP_CONFCOMPUTE_WAR_5107790_SYSMEM_FLUSH_ADDR;

    // Data members
    NvU32 gspProxyRegkeys;
    NV2080_CTRL_INTERNAL_CONF_COMPUTE_GET_STATIC_INFO_PARAMS ccStaticInfo;
    struct ccslContext_t *pRpcCcslCtx;
    struct ccslContext_t *pDmaCcslCtx;
    struct ccslContext_t *pNonReplayableFaultCcslCtx;
    struct ccslContext_t *pReplayableFaultCcslCtx;
    struct ccslContext_t *pGspSec2RpcCcslCtx;
    struct ccslContext_t *pNvleP2pWrappingCcslCtx;
    NvBool bAcceptClientRequest;
    struct TMR_EVENT *pGspHeartbeatTimer;
    NvU32 heartbeatPeriodSec;
    NvU32 keyRotationEnableMask;
    NvU64 keyRotationInternalThreshold;
    NvU64 attackerAdvantage;
    KEY_ROTATION_STATS_INFO aggregateStats[114];
    NvU8 PRIVATE_FIELD(m_exportMasterKey)[32];
    void *PRIVATE_FIELD(m_keySlot);
    NvU8 PRIVATE_FIELD(channelKeySeed)[32];
    KEY_ROTATION_STATUS PRIVATE_FIELD(keyRotationState)[114];
    KEY_ROTATION_STATS_INFO PRIVATE_FIELD(freedChannelAggregateStats)[114];
    KEY_ROTATION_TIMEOUT_INFO PRIVATE_FIELD(keyRotationTimeoutInfo)[114];
    NvU32 PRIVATE_FIELD(keyRotationCount)[114];
    NvU32 PRIVATE_FIELD(keyRotationTimeout);
    NvU64 PRIVATE_FIELD(keyRotationThresholdDelta);
    NvU64 PRIVATE_FIELD(keyRotationUpperThreshold);
    NvU64 PRIVATE_FIELD(keyRotationLowerThreshold);
};


// Vtable with 12 per-class function pointers
struct NVOC_VTABLE__ConfidentialCompute {
    NV_STATUS (*__confComputeConstructEngine__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__confComputeStatePreInitLocked__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/);  // virtual halified (singleton optimized) override (engstate) base (engstate) body
    NV_STATUS (*__confComputeStateInitLocked__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/);  // virtual override (engstate) base (engstate)
    void (*__confComputeInitMissing__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__confComputeStatePreInitUnlocked__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__confComputeStateInitUnlocked__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__confComputeStatePreLoad__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__confComputeStateLoad__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__confComputeStateUnload__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__confComputeStatePostUnload__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    void (*__confComputeStateDestroy__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/);  // virtual inherited (engstate) base (engstate)
    NvBool (*__confComputeIsPresent__)(struct OBJGPU *, struct ConfidentialCompute * /*this*/);  // virtual inherited (engstate) base (engstate)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__ConfidentialCompute {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
    const struct NVOC_VTABLE__ConfidentialCompute vtable;
};

#ifndef __nvoc_class_id_ConfidentialCompute
#define __nvoc_class_id_ConfidentialCompute 0x9798ccu
typedef struct ConfidentialCompute ConfidentialCompute;
#endif /* __nvoc_class_id_ConfidentialCompute */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_ConfidentialCompute;

#define __staticCast_ConfidentialCompute(pThis) \
    ((pThis)->__nvoc_pbase_ConfidentialCompute)

#ifdef __nvoc_conf_compute_h_disabled
#define __dynamicCast_ConfidentialCompute(pThis) ((ConfidentialCompute*) NULL)
#else //__nvoc_conf_compute_h_disabled
#define __dynamicCast_ConfidentialCompute(pThis) \
    ((ConfidentialCompute*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(ConfidentialCompute)))
#endif //__nvoc_conf_compute_h_disabled

// Property macros
#define PDB_PROP_CONFCOMPUTE_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_CONFCOMPUTE_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING
#define PDB_PROP_CONFCOMPUTE_ENABLED_BASE_CAST
#define PDB_PROP_CONFCOMPUTE_ENABLED_BASE_NAME PDB_PROP_CONFCOMPUTE_ENABLED
#define PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED_BASE_CAST
#define PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED_BASE_NAME PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED
#define PDB_PROP_CONFCOMPUTE_APM_FEATURE_ENABLED_BASE_CAST
#define PDB_PROP_CONFCOMPUTE_APM_FEATURE_ENABLED_BASE_NAME PDB_PROP_CONFCOMPUTE_APM_FEATURE_ENABLED
#define PDB_PROP_CONFCOMPUTE_DEVTOOLS_MODE_ENABLED_BASE_CAST
#define PDB_PROP_CONFCOMPUTE_DEVTOOLS_MODE_ENABLED_BASE_NAME PDB_PROP_CONFCOMPUTE_DEVTOOLS_MODE_ENABLED
#define PDB_PROP_CONFCOMPUTE_ENABLE_EARLY_INIT_BASE_CAST
#define PDB_PROP_CONFCOMPUTE_ENABLE_EARLY_INIT_BASE_NAME PDB_PROP_CONFCOMPUTE_ENABLE_EARLY_INIT
#define PDB_PROP_CONFCOMPUTE_GPUS_READY_CHECK_ENABLED_BASE_CAST
#define PDB_PROP_CONFCOMPUTE_GPUS_READY_CHECK_ENABLED_BASE_NAME PDB_PROP_CONFCOMPUTE_GPUS_READY_CHECK_ENABLED
#define PDB_PROP_CONFCOMPUTE_MULTI_GPU_PROTECTED_PCIE_MODE_ENABLED_BASE_CAST
#define PDB_PROP_CONFCOMPUTE_MULTI_GPU_PROTECTED_PCIE_MODE_ENABLED_BASE_NAME PDB_PROP_CONFCOMPUTE_MULTI_GPU_PROTECTED_PCIE_MODE_ENABLED
#define PDB_PROP_CONFCOMPUTE_MULTI_GPU_NVLE_MODE_ENABLED_BASE_CAST
#define PDB_PROP_CONFCOMPUTE_MULTI_GPU_NVLE_MODE_ENABLED_BASE_NAME PDB_PROP_CONFCOMPUTE_MULTI_GPU_NVLE_MODE_ENABLED
#define PDB_PROP_CONFCOMPUTE_KEY_ROTATION_SUPPORTED_BASE_CAST
#define PDB_PROP_CONFCOMPUTE_KEY_ROTATION_SUPPORTED_BASE_NAME PDB_PROP_CONFCOMPUTE_KEY_ROTATION_SUPPORTED
#define PDB_PROP_CONFCOMPUTE_KEY_ROTATION_ENABLED_BASE_CAST
#define PDB_PROP_CONFCOMPUTE_KEY_ROTATION_ENABLED_BASE_NAME PDB_PROP_CONFCOMPUTE_KEY_ROTATION_ENABLED
#define PDB_PROP_CONFCOMPUTE_INTERNAL_KEY_ROTATION_ENABLED_BASE_CAST
#define PDB_PROP_CONFCOMPUTE_INTERNAL_KEY_ROTATION_ENABLED_BASE_NAME PDB_PROP_CONFCOMPUTE_INTERNAL_KEY_ROTATION_ENABLED
#define PDB_PROP_CONFCOMPUTE_WAR_5107790_SYSMEM_FLUSH_ADDR_BASE_CAST
#define PDB_PROP_CONFCOMPUTE_WAR_5107790_SYSMEM_FLUSH_ADDR_BASE_NAME PDB_PROP_CONFCOMPUTE_WAR_5107790_SYSMEM_FLUSH_ADDR


NV_STATUS __nvoc_objCreateDynamic_ConfidentialCompute(ConfidentialCompute**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_ConfidentialCompute(ConfidentialCompute**, Dynamic*, NvU32);
#define __objCreate_ConfidentialCompute(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags) \
    __nvoc_objCreate_ConfidentialCompute((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags))


// Wrapper macros for implementation functions
NvBool confComputeAcceptClientRequest_IMPL(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute);
#ifdef __nvoc_conf_compute_h_disabled
static inline NvBool confComputeAcceptClientRequest(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    NV_ASSERT_FAILED_PRECOMP("ConfidentialCompute was disabled!");
    return NV_FALSE;
}
#else // __nvoc_conf_compute_h_disabled
#define confComputeAcceptClientRequest(pGpu, pConfCompute) confComputeAcceptClientRequest_IMPL(pGpu, pConfCompute)
#endif // __nvoc_conf_compute_h_disabled

NV_STATUS confComputeInitChannelIterForKey_IMPL(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvU32 globalKey, CHANNEL_ITERATOR *pIter);
#ifdef __nvoc_conf_compute_h_disabled
static inline NV_STATUS confComputeInitChannelIterForKey(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvU32 globalKey, CHANNEL_ITERATOR *pIter) {
    NV_ASSERT_FAILED_PRECOMP("ConfidentialCompute was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_conf_compute_h_disabled
#define confComputeInitChannelIterForKey(pGpu, pConfCompute, globalKey, pIter) confComputeInitChannelIterForKey_IMPL(pGpu, pConfCompute, globalKey, pIter)
#endif // __nvoc_conf_compute_h_disabled

NV_STATUS confComputeGetNextChannelForKey_IMPL(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, CHANNEL_ITERATOR *pIt, NvU32 globalKey, struct KernelChannel **ppKernelChannel);
#ifdef __nvoc_conf_compute_h_disabled
static inline NV_STATUS confComputeGetNextChannelForKey(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, CHANNEL_ITERATOR *pIt, NvU32 globalKey, struct KernelChannel **ppKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("ConfidentialCompute was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_conf_compute_h_disabled
#define confComputeGetNextChannelForKey(pGpu, pConfCompute, pIt, globalKey, ppKernelChannel) confComputeGetNextChannelForKey_IMPL(pGpu, pConfCompute, pIt, globalKey, ppKernelChannel)
#endif // __nvoc_conf_compute_h_disabled

NV_STATUS confComputeGetKeySlotFromGlobalKeyId_IMPL(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId, NvU32 *pSlot);
#ifdef __nvoc_conf_compute_h_disabled
static inline NV_STATUS confComputeGetKeySlotFromGlobalKeyId(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId, NvU32 *pSlot) {
    NV_ASSERT_FAILED_PRECOMP("ConfidentialCompute was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_conf_compute_h_disabled
#define confComputeGetKeySlotFromGlobalKeyId(pConfCompute, globalKeyId, pSlot) confComputeGetKeySlotFromGlobalKeyId_IMPL(pConfCompute, globalKeyId, pSlot)
#endif // __nvoc_conf_compute_h_disabled

NV_STATUS confComputeCheckAndPerformKeyRotation_IMPL(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvU32 h2dKey, NvU32 d2hKey);
#ifdef __nvoc_conf_compute_h_disabled
static inline NV_STATUS confComputeCheckAndPerformKeyRotation(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvU32 h2dKey, NvU32 d2hKey) {
    NV_ASSERT_FAILED_PRECOMP("ConfidentialCompute was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_conf_compute_h_disabled
#define confComputeCheckAndPerformKeyRotation(pGpu, pConfCompute, h2dKey, d2hKey) confComputeCheckAndPerformKeyRotation_IMPL(pGpu, pConfCompute, h2dKey, d2hKey)
#endif // __nvoc_conf_compute_h_disabled

NV_STATUS confComputePerformKeyRotation_IMPL(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvU32 h2dKey, NvU32 d2hKey, NvBool bWorkItem);
#ifdef __nvoc_conf_compute_h_disabled
static inline NV_STATUS confComputePerformKeyRotation(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvU32 h2dKey, NvU32 d2hKey, NvBool bWorkItem) {
    NV_ASSERT_FAILED_PRECOMP("ConfidentialCompute was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_conf_compute_h_disabled
#define confComputePerformKeyRotation(pGpu, pConfCompute, h2dKey, d2hKey, bWorkItem) confComputePerformKeyRotation_IMPL(pGpu, pConfCompute, h2dKey, d2hKey, bWorkItem)
#endif // __nvoc_conf_compute_h_disabled

NV_STATUS confComputeForceKeyRotation_IMPL(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvU32 h2dKey, NvU32 d2hKey);
#ifdef __nvoc_conf_compute_h_disabled
static inline NV_STATUS confComputeForceKeyRotation(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvU32 h2dKey, NvU32 d2hKey) {
    NV_ASSERT_FAILED_PRECOMP("ConfidentialCompute was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_conf_compute_h_disabled
#define confComputeForceKeyRotation(pGpu, pConfCompute, h2dKey, d2hKey) confComputeForceKeyRotation_IMPL(pGpu, pConfCompute, h2dKey, d2hKey)
#endif // __nvoc_conf_compute_h_disabled

NV_STATUS confComputeSetKeyRotationStatus_IMPL(struct ConfidentialCompute *pConfCompute, NvU32 globalKey, KEY_ROTATION_STATUS status);
#ifdef __nvoc_conf_compute_h_disabled
static inline NV_STATUS confComputeSetKeyRotationStatus(struct ConfidentialCompute *pConfCompute, NvU32 globalKey, KEY_ROTATION_STATUS status) {
    NV_ASSERT_FAILED_PRECOMP("ConfidentialCompute was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_conf_compute_h_disabled
#define confComputeSetKeyRotationStatus(pConfCompute, globalKey, status) confComputeSetKeyRotationStatus_IMPL(pConfCompute, globalKey, status)
#endif // __nvoc_conf_compute_h_disabled

NV_STATUS confComputeGetKeyRotationStatus_IMPL(struct ConfidentialCompute *pConfCompute, NvU32 globalKey, KEY_ROTATION_STATUS *pStatus);
#ifdef __nvoc_conf_compute_h_disabled
static inline NV_STATUS confComputeGetKeyRotationStatus(struct ConfidentialCompute *pConfCompute, NvU32 globalKey, KEY_ROTATION_STATUS *pStatus) {
    NV_ASSERT_FAILED_PRECOMP("ConfidentialCompute was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_conf_compute_h_disabled
#define confComputeGetKeyRotationStatus(pConfCompute, globalKey, pStatus) confComputeGetKeyRotationStatus_IMPL(pConfCompute, globalKey, pStatus)
#endif // __nvoc_conf_compute_h_disabled

void confComputeGetKeyPairByKey_IMPL(struct ConfidentialCompute *pConfCompute, NvU32 globalKey, NvU32 *pH2DKey, NvU32 *pD2HKey);
#ifdef __nvoc_conf_compute_h_disabled
static inline void confComputeGetKeyPairByKey(struct ConfidentialCompute *pConfCompute, NvU32 globalKey, NvU32 *pH2DKey, NvU32 *pD2HKey) {
    NV_ASSERT_FAILED_PRECOMP("ConfidentialCompute was disabled!");
}
#else // __nvoc_conf_compute_h_disabled
#define confComputeGetKeyPairByKey(pConfCompute, globalKey, pH2DKey, pD2HKey) confComputeGetKeyPairByKey_IMPL(pConfCompute, globalKey, pH2DKey, pD2HKey)
#endif // __nvoc_conf_compute_h_disabled

NV_STATUS confComputeUpdateFreedChannelStats_IMPL(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel);
#ifdef __nvoc_conf_compute_h_disabled
static inline NV_STATUS confComputeUpdateFreedChannelStats(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("ConfidentialCompute was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_conf_compute_h_disabled
#define confComputeUpdateFreedChannelStats(pGpu, pConfCompute, pKernelChannel) confComputeUpdateFreedChannelStats_IMPL(pGpu, pConfCompute, pKernelChannel)
#endif // __nvoc_conf_compute_h_disabled

NV_STATUS confComputeStartKeyRotationTimer_IMPL(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvU32 h2dKey);
#ifdef __nvoc_conf_compute_h_disabled
static inline NV_STATUS confComputeStartKeyRotationTimer(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvU32 h2dKey) {
    NV_ASSERT_FAILED_PRECOMP("ConfidentialCompute was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_conf_compute_h_disabled
#define confComputeStartKeyRotationTimer(pGpu, pConfCompute, h2dKey) confComputeStartKeyRotationTimer_IMPL(pGpu, pConfCompute, h2dKey)
#endif // __nvoc_conf_compute_h_disabled

NV_STATUS confComputeStopKeyRotationTimer_IMPL(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvU32 h2dKey);
#ifdef __nvoc_conf_compute_h_disabled
static inline NV_STATUS confComputeStopKeyRotationTimer(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvU32 h2dKey) {
    NV_ASSERT_FAILED_PRECOMP("ConfidentialCompute was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_conf_compute_h_disabled
#define confComputeStopKeyRotationTimer(pGpu, pConfCompute, h2dKey) confComputeStopKeyRotationTimer_IMPL(pGpu, pConfCompute, h2dKey)
#endif // __nvoc_conf_compute_h_disabled

NvBool confComputeIsUvmKeyRotationPending_IMPL(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute);
#ifdef __nvoc_conf_compute_h_disabled
static inline NvBool confComputeIsUvmKeyRotationPending(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    NV_ASSERT_FAILED_PRECOMP("ConfidentialCompute was disabled!");
    return NV_FALSE;
}
#else // __nvoc_conf_compute_h_disabled
#define confComputeIsUvmKeyRotationPending(pGpu, pConfCompute) confComputeIsUvmKeyRotationPending_IMPL(pGpu, pConfCompute)
#endif // __nvoc_conf_compute_h_disabled

NV_STATUS confComputeSetKeyRotationThreshold_IMPL(struct ConfidentialCompute *pConfCompute, NvU64 attackerAdvantage);
#ifdef __nvoc_conf_compute_h_disabled
static inline NV_STATUS confComputeSetKeyRotationThreshold(struct ConfidentialCompute *pConfCompute, NvU64 attackerAdvantage) {
    NV_ASSERT_FAILED_PRECOMP("ConfidentialCompute was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_conf_compute_h_disabled
#define confComputeSetKeyRotationThreshold(pConfCompute, attackerAdvantage) confComputeSetKeyRotationThreshold_IMPL(pConfCompute, attackerAdvantage)
#endif // __nvoc_conf_compute_h_disabled

void confComputeGetKeyRotationThreshold_IMPL(struct ConfidentialCompute *pConfCompute, NvU64 *pLowerThreshold, NvU64 *pUpperThreshold);
#ifdef __nvoc_conf_compute_h_disabled
static inline void confComputeGetKeyRotationThreshold(struct ConfidentialCompute *pConfCompute, NvU64 *pLowerThreshold, NvU64 *pUpperThreshold) {
    NV_ASSERT_FAILED_PRECOMP("ConfidentialCompute was disabled!");
}
#else // __nvoc_conf_compute_h_disabled
#define confComputeGetKeyRotationThreshold(pConfCompute, pLowerThreshold, pUpperThreshold) confComputeGetKeyRotationThreshold_IMPL(pConfCompute, pLowerThreshold, pUpperThreshold)
#endif // __nvoc_conf_compute_h_disabled

NvBool confComputeIsUpperThresholdCrossed_IMPL(struct ConfidentialCompute *pConfCompute, const KEY_ROTATION_STATS_INFO *pStatsInfo);
#ifdef __nvoc_conf_compute_h_disabled
static inline NvBool confComputeIsUpperThresholdCrossed(struct ConfidentialCompute *pConfCompute, const KEY_ROTATION_STATS_INFO *pStatsInfo) {
    NV_ASSERT_FAILED_PRECOMP("ConfidentialCompute was disabled!");
    return NV_FALSE;
}
#else // __nvoc_conf_compute_h_disabled
#define confComputeIsUpperThresholdCrossed(pConfCompute, pStatsInfo) confComputeIsUpperThresholdCrossed_IMPL(pConfCompute, pStatsInfo)
#endif // __nvoc_conf_compute_h_disabled

NvBool confComputeIsLowerThresholdCrossed_IMPL(struct ConfidentialCompute *pConfCompute, const KEY_ROTATION_STATS_INFO *pStatsInfo);
#ifdef __nvoc_conf_compute_h_disabled
static inline NvBool confComputeIsLowerThresholdCrossed(struct ConfidentialCompute *pConfCompute, const KEY_ROTATION_STATS_INFO *pStatsInfo) {
    NV_ASSERT_FAILED_PRECOMP("ConfidentialCompute was disabled!");
    return NV_FALSE;
}
#else // __nvoc_conf_compute_h_disabled
#define confComputeIsLowerThresholdCrossed(pConfCompute, pStatsInfo) confComputeIsLowerThresholdCrossed_IMPL(pConfCompute, pStatsInfo)
#endif // __nvoc_conf_compute_h_disabled

NvBool confComputeIsGivenThresholdCrossed_IMPL(const CC_CRYPTOBUNDLE_STATS *pStatsInfo, NvU64 threshold, NvBool bEncrypt);
#define confComputeIsGivenThresholdCrossed(pStatsInfo, threshold, bEncrypt) confComputeIsGivenThresholdCrossed_IMPL(pStatsInfo, threshold, bEncrypt)


// Wrapper macros for halified functions
#define confComputeConstructEngine_FNPTR(pConfCompute) pConfCompute->__nvoc_metadata_ptr->vtable.__confComputeConstructEngine__
#define confComputeConstructEngine(pGpu, pConfCompute, engDesc) confComputeConstructEngine_DISPATCH(pGpu, pConfCompute, engDesc)
#define confComputeDestruct_FNPTR(pConfCompute) pConfCompute->__confComputeDestruct__
#define __nvoc_confComputeDestruct(pConfCompute) confComputeDestruct_DISPATCH(pConfCompute)
#define confComputeStatePreInitLocked_FNPTR(pConfCompute) pConfCompute->__nvoc_metadata_ptr->vtable.__confComputeStatePreInitLocked__
#define confComputeStatePreInitLocked(pGpu, pConfCompute) confComputeStatePreInitLocked_DISPATCH(pGpu, pConfCompute)
#define confComputeStatePreInitLocked_HAL(pGpu, pConfCompute) confComputeStatePreInitLocked_DISPATCH(pGpu, pConfCompute)
#define confComputeStateInitLocked_FNPTR(pConfCompute) pConfCompute->__nvoc_metadata_ptr->vtable.__confComputeStateInitLocked__
#define confComputeStateInitLocked(pGpu, pConfCompute) confComputeStateInitLocked_DISPATCH(pGpu, pConfCompute)
#define confComputeStatePostLoad_FNPTR(pConfCompute) pConfCompute->__confComputeStatePostLoad__
#define confComputeStatePostLoad(pGpu, pConfCompute, flags) confComputeStatePostLoad_DISPATCH(pGpu, pConfCompute, flags)
#define confComputeStatePostLoad_HAL(pGpu, pConfCompute, flags) confComputeStatePostLoad_DISPATCH(pGpu, pConfCompute, flags)
#define confComputeStatePreUnload_FNPTR(pConfCompute) pConfCompute->__confComputeStatePreUnload__
#define confComputeStatePreUnload(pGpu, pConfCompute, flags) confComputeStatePreUnload_DISPATCH(pGpu, pConfCompute, flags)
#define confComputeStatePreUnload_HAL(pGpu, pConfCompute, flags) confComputeStatePreUnload_DISPATCH(pGpu, pConfCompute, flags)
#define confComputeSetErrorState_FNPTR(pConfCompute) pConfCompute->__confComputeSetErrorState__
#define confComputeSetErrorState(pGpu, pConfCompute) confComputeSetErrorState_DISPATCH(pGpu, pConfCompute)
#define confComputeSetErrorState_HAL(pGpu, pConfCompute) confComputeSetErrorState_DISPATCH(pGpu, pConfCompute)
#define confComputeKeyStoreDeriveViaChannel_FNPTR(pConfCompute) pConfCompute->__confComputeKeyStoreDeriveViaChannel__
#define confComputeKeyStoreDeriveViaChannel(pConfCompute, pKernelChannel, rotateOperation) confComputeKeyStoreDeriveViaChannel_DISPATCH(pConfCompute, pKernelChannel, rotateOperation)
#define confComputeKeyStoreDeriveViaChannel_HAL(pConfCompute, pKernelChannel, rotateOperation) confComputeKeyStoreDeriveViaChannel_DISPATCH(pConfCompute, pKernelChannel, rotateOperation)
#define confComputeKeyStoreRetrieveViaChannel_FNPTR(pConfCompute) pConfCompute->__confComputeKeyStoreRetrieveViaChannel__
#define confComputeKeyStoreRetrieveViaChannel(pConfCompute, pKernelChannel, rotateOperation, ivOperation, keyMaterialBundle) confComputeKeyStoreRetrieveViaChannel_DISPATCH(pConfCompute, pKernelChannel, rotateOperation, ivOperation, keyMaterialBundle)
#define confComputeKeyStoreRetrieveViaChannel_HAL(pConfCompute, pKernelChannel, rotateOperation, ivOperation, keyMaterialBundle) confComputeKeyStoreRetrieveViaChannel_DISPATCH(pConfCompute, pKernelChannel, rotateOperation, ivOperation, keyMaterialBundle)
#define confComputeKeyStoreRetrieveViaKeyId_FNPTR(pConfCompute) pConfCompute->__confComputeKeyStoreRetrieveViaKeyId__
#define confComputeKeyStoreRetrieveViaKeyId(pConfCompute, globalKeyId, rotateOperation, bIncludeIvOrNonce, keyMaterialBundle) confComputeKeyStoreRetrieveViaKeyId_DISPATCH(pConfCompute, globalKeyId, rotateOperation, bIncludeIvOrNonce, keyMaterialBundle)
#define confComputeKeyStoreRetrieveViaKeyId_HAL(pConfCompute, globalKeyId, rotateOperation, bIncludeIvOrNonce, keyMaterialBundle) confComputeKeyStoreRetrieveViaKeyId_DISPATCH(pConfCompute, globalKeyId, rotateOperation, bIncludeIvOrNonce, keyMaterialBundle)
#define confComputeDeriveSecretsForCEKeySpace_FNPTR(pConfCompute) pConfCompute->__confComputeDeriveSecretsForCEKeySpace__
#define confComputeDeriveSecretsForCEKeySpace(pConfCompute, ceRmEngineTypeIdx, ccKeyspaceIndex) confComputeDeriveSecretsForCEKeySpace_DISPATCH(pConfCompute, ceRmEngineTypeIdx, ccKeyspaceIndex)
#define confComputeDeriveSecretsForCEKeySpace_HAL(pConfCompute, ceRmEngineTypeIdx, ccKeyspaceIndex) confComputeDeriveSecretsForCEKeySpace_DISPATCH(pConfCompute, ceRmEngineTypeIdx, ccKeyspaceIndex)
#define confComputeDeriveInitialKeySeed_FNPTR(pConfCompute) pConfCompute->__confComputeDeriveInitialKeySeed__
#define confComputeDeriveInitialKeySeed(pConfCompute) confComputeDeriveInitialKeySeed_DISPATCH(pConfCompute)
#define confComputeDeriveInitialKeySeed_HAL(pConfCompute) confComputeDeriveInitialKeySeed_DISPATCH(pConfCompute)
#define confComputeGetAndUpdateCurrentKeySeed_FNPTR(pConfCompute) pConfCompute->__confComputeGetAndUpdateCurrentKeySeed__
#define confComputeGetAndUpdateCurrentKeySeed(pConfCompute, pKey) confComputeGetAndUpdateCurrentKeySeed_DISPATCH(pConfCompute, pKey)
#define confComputeGetAndUpdateCurrentKeySeed_HAL(pConfCompute, pKey) confComputeGetAndUpdateCurrentKeySeed_DISPATCH(pConfCompute, pKey)
#define confComputeDeriveSecrets_FNPTR(pConfCompute) pConfCompute->__confComputeDeriveSecrets__
#define confComputeDeriveSecrets(pConfCompute, engine) confComputeDeriveSecrets_DISPATCH(pConfCompute, engine)
#define confComputeDeriveSecrets_HAL(pConfCompute, engine) confComputeDeriveSecrets_DISPATCH(pConfCompute, engine)
#define confComputeRotationNotifHandler_FNPTR(pConfCompute) pConfCompute->__confComputeRotationNotifHandler__
#define confComputeRotationNotifHandler(pGpu, pConfCompute, pRmApi, hClient, pKernelChannel, bCheckKeyRotation, bSet) confComputeRotationNotifHandler_DISPATCH(pGpu, pConfCompute, pRmApi, hClient, pKernelChannel, bCheckKeyRotation, bSet)
#define confComputeRotationNotifHandler_HAL(pGpu, pConfCompute, pRmApi, hClient, pKernelChannel, bCheckKeyRotation, bSet) confComputeRotationNotifHandler_DISPATCH(pGpu, pConfCompute, pRmApi, hClient, pKernelChannel, bCheckKeyRotation, bSet)
#define confComputeUpdateSecrets_FNPTR(pConfCompute) pConfCompute->__confComputeUpdateSecrets__
#define confComputeUpdateSecrets(pConfCompute, globalKeyId) confComputeUpdateSecrets_DISPATCH(pConfCompute, globalKeyId)
#define confComputeUpdateSecrets_HAL(pConfCompute, globalKeyId) confComputeUpdateSecrets_DISPATCH(pConfCompute, globalKeyId)
#define confComputeUpdatePerChannelSecrets_FNPTR(pConfCompute) pConfCompute->__confComputeUpdatePerChannelSecrets__
#define confComputeUpdatePerChannelSecrets(pConfCompute, pKernelChannel) confComputeUpdatePerChannelSecrets_DISPATCH(pConfCompute, pKernelChannel)
#define confComputeUpdatePerChannelSecrets_HAL(pConfCompute, pKernelChannel) confComputeUpdatePerChannelSecrets_DISPATCH(pConfCompute, pKernelChannel)
#define confComputeIsSpdmEnabled_FNPTR(pConfCompute) pConfCompute->__confComputeIsSpdmEnabled__
#define confComputeIsSpdmEnabled(pGpu, pConfCompute) confComputeIsSpdmEnabled_DISPATCH(pGpu, pConfCompute)
#define confComputeIsSpdmEnabled_HAL(pGpu, pConfCompute) confComputeIsSpdmEnabled_DISPATCH(pGpu, pConfCompute)
#define confComputeGetEngineIdFromKeySpace_FNPTR(pConfCompute) pConfCompute->__confComputeGetEngineIdFromKeySpace__
#define confComputeGetEngineIdFromKeySpace(pConfCompute, keySpace) confComputeGetEngineIdFromKeySpace_DISPATCH(pConfCompute, keySpace)
#define confComputeGetEngineIdFromKeySpace_HAL(pConfCompute, keySpace) confComputeGetEngineIdFromKeySpace_DISPATCH(pConfCompute, keySpace)
#define confComputeGetKeySpaceFromKChannel_FNPTR(pConfCompute) pConfCompute->__confComputeGetKeySpaceFromKChannel__
#define confComputeGetKeySpaceFromKChannel(pConfCompute, pKernelChannel, keyspace) confComputeGetKeySpaceFromKChannel_DISPATCH(pConfCompute, pKernelChannel, keyspace)
#define confComputeGetKeySpaceFromKChannel_HAL(pConfCompute, pKernelChannel, keyspace) confComputeGetKeySpaceFromKChannel_DISPATCH(pConfCompute, pKernelChannel, keyspace)
#define confComputeGetLceKeyIdFromKChannel_FNPTR(pConfCompute) pConfCompute->__confComputeGetLceKeyIdFromKChannel__
#define confComputeGetLceKeyIdFromKChannel(pConfCompute, pKernelChannel, rotateOperation, pKeyId) confComputeGetLceKeyIdFromKChannel_DISPATCH(pConfCompute, pKernelChannel, rotateOperation, pKeyId)
#define confComputeGetLceKeyIdFromKChannel_HAL(pConfCompute, pKernelChannel, rotateOperation, pKeyId) confComputeGetLceKeyIdFromKChannel_DISPATCH(pConfCompute, pKernelChannel, rotateOperation, pKeyId)
#define confComputeGetMaxCeKeySpaceIdx_FNPTR(pConfCompute) pConfCompute->__confComputeGetMaxCeKeySpaceIdx__
#define confComputeGetMaxCeKeySpaceIdx(pConfCompute) confComputeGetMaxCeKeySpaceIdx_DISPATCH(pConfCompute)
#define confComputeGetMaxCeKeySpaceIdx_HAL(pConfCompute) confComputeGetMaxCeKeySpaceIdx_DISPATCH(pConfCompute)
#define confComputeGlobalKeyIsKernelPriv_FNPTR(pConfCompute) pConfCompute->__confComputeGlobalKeyIsKernelPriv__
#define confComputeGlobalKeyIsKernelPriv(pConfCompute, keyId) confComputeGlobalKeyIsKernelPriv_DISPATCH(pConfCompute, keyId)
#define confComputeGlobalKeyIsKernelPriv_HAL(pConfCompute, keyId) confComputeGlobalKeyIsKernelPriv_DISPATCH(pConfCompute, keyId)
#define confComputeGlobalKeyIsUvmKey_FNPTR(pConfCompute) pConfCompute->__confComputeGlobalKeyIsUvmKey__
#define confComputeGlobalKeyIsUvmKey(pConfCompute, keyId) confComputeGlobalKeyIsUvmKey_DISPATCH(pConfCompute, keyId)
#define confComputeGlobalKeyIsUvmKey_HAL(pConfCompute, keyId) confComputeGlobalKeyIsUvmKey_DISPATCH(pConfCompute, keyId)
#define confComputeGetKeyPairByChannel_FNPTR(pConfCompute) pConfCompute->__confComputeGetKeyPairByChannel__
#define confComputeGetKeyPairByChannel(pGpu, pConfCompute, pKernelChannel, pH2DKey, pD2HKey) confComputeGetKeyPairByChannel_DISPATCH(pGpu, pConfCompute, pKernelChannel, pH2DKey, pD2HKey)
#define confComputeGetKeyPairByChannel_HAL(pGpu, pConfCompute, pKernelChannel, pH2DKey, pD2HKey) confComputeGetKeyPairByChannel_DISPATCH(pGpu, pConfCompute, pKernelChannel, pH2DKey, pD2HKey)
#define confComputeTriggerKeyRotation_FNPTR(pConfCompute) pConfCompute->__confComputeTriggerKeyRotation__
#define confComputeTriggerKeyRotation(pGpu, pConfCompute) confComputeTriggerKeyRotation_DISPATCH(pGpu, pConfCompute)
#define confComputeTriggerKeyRotation_HAL(pGpu, pConfCompute) confComputeTriggerKeyRotation_DISPATCH(pGpu, pConfCompute)
#define confComputeGetKeyPairForKeySpace_FNPTR(pConfCompute) pConfCompute->__confComputeGetKeyPairForKeySpace__
#define confComputeGetKeyPairForKeySpace(pGpu, pConfCompute, arg3, arg4, arg5, arg6) confComputeGetKeyPairForKeySpace_DISPATCH(pGpu, pConfCompute, arg3, arg4, arg5, arg6)
#define confComputeGetKeyPairForKeySpace_HAL(pGpu, pConfCompute, arg3, arg4, arg5, arg6) confComputeGetKeyPairForKeySpace_DISPATCH(pGpu, pConfCompute, arg3, arg4, arg5, arg6)
#define confComputeEnableKeyRotationCallback_FNPTR(pConfCompute) pConfCompute->__confComputeEnableKeyRotationCallback__
#define confComputeEnableKeyRotationCallback(pGpu, pConfCompute, bEnable) confComputeEnableKeyRotationCallback_DISPATCH(pGpu, pConfCompute, bEnable)
#define confComputeEnableKeyRotationCallback_HAL(pGpu, pConfCompute, bEnable) confComputeEnableKeyRotationCallback_DISPATCH(pGpu, pConfCompute, bEnable)
#define confComputeEnableKeyRotationSupport_FNPTR(pConfCompute) pConfCompute->__confComputeEnableKeyRotationSupport__
#define confComputeEnableKeyRotationSupport(pGpu, pConfCompute) confComputeEnableKeyRotationSupport_DISPATCH(pGpu, pConfCompute)
#define confComputeEnableKeyRotationSupport_HAL(pGpu, pConfCompute) confComputeEnableKeyRotationSupport_DISPATCH(pGpu, pConfCompute)
#define confComputeEnableInternalKeyRotationSupport_FNPTR(pConfCompute) pConfCompute->__confComputeEnableInternalKeyRotationSupport__
#define confComputeEnableInternalKeyRotationSupport(pGpu, pConfCompute) confComputeEnableInternalKeyRotationSupport_DISPATCH(pGpu, pConfCompute)
#define confComputeEnableInternalKeyRotationSupport_HAL(pGpu, pConfCompute) confComputeEnableInternalKeyRotationSupport_DISPATCH(pGpu, pConfCompute)
#define confComputeIsDebugModeEnabled_FNPTR(pConfCompute) pConfCompute->__confComputeIsDebugModeEnabled__
#define confComputeIsDebugModeEnabled(pGpu, pConfCompute) confComputeIsDebugModeEnabled_DISPATCH(pGpu, pConfCompute)
#define confComputeIsDebugModeEnabled_HAL(pGpu, pConfCompute) confComputeIsDebugModeEnabled_DISPATCH(pGpu, pConfCompute)
#define confComputeIsGpuCcCapable_FNPTR(pConfCompute) pConfCompute->__confComputeIsGpuCcCapable__
#define confComputeIsGpuCcCapable(pGpu, pConfCompute) confComputeIsGpuCcCapable_DISPATCH(pGpu, pConfCompute)
#define confComputeIsGpuCcCapable_HAL(pGpu, pConfCompute) confComputeIsGpuCcCapable_DISPATCH(pGpu, pConfCompute)
#define confComputeTestPlatformSupport_FNPTR(pConfCompute) pConfCompute->__confComputeTestPlatformSupport__
#define confComputeTestPlatformSupport(pGpu, pConfCompute) confComputeTestPlatformSupport_DISPATCH(pGpu, pConfCompute)
#define confComputeTestPlatformSupport_HAL(pGpu, pConfCompute) confComputeTestPlatformSupport_DISPATCH(pGpu, pConfCompute)
#define confComputeDeriveSessionKeys_FNPTR(pConfCompute) pConfCompute->__confComputeDeriveSessionKeys__
#define confComputeDeriveSessionKeys(pGpu, pConfCompute) confComputeDeriveSessionKeys_DISPATCH(pGpu, pConfCompute)
#define confComputeDeriveSessionKeys_HAL(pGpu, pConfCompute) confComputeDeriveSessionKeys_DISPATCH(pGpu, pConfCompute)
#define confComputeKeyStoreDepositIvMask_FNPTR(pConfCompute) pConfCompute->__confComputeKeyStoreDepositIvMask__
#define confComputeKeyStoreDepositIvMask(pConfCompute, globalKeyId, ivMask) confComputeKeyStoreDepositIvMask_DISPATCH(pConfCompute, globalKeyId, ivMask)
#define confComputeKeyStoreDepositIvMask_HAL(pConfCompute, globalKeyId, ivMask) confComputeKeyStoreDepositIvMask_DISPATCH(pConfCompute, globalKeyId, ivMask)
#define confComputeKeyStoreUpdateKey_FNPTR(pConfCompute) pConfCompute->__confComputeKeyStoreUpdateKey__
#define confComputeKeyStoreUpdateKey(pConfCompute, globalKeyId) confComputeKeyStoreUpdateKey_DISPATCH(pConfCompute, globalKeyId)
#define confComputeKeyStoreUpdateKey_HAL(pConfCompute, globalKeyId) confComputeKeyStoreUpdateKey_DISPATCH(pConfCompute, globalKeyId)
#define confComputeKeyStoreIsValidGlobalKeyId_FNPTR(pConfCompute) pConfCompute->__confComputeKeyStoreIsValidGlobalKeyId__
#define confComputeKeyStoreIsValidGlobalKeyId(pConfCompute, globalKeyId) confComputeKeyStoreIsValidGlobalKeyId_DISPATCH(pConfCompute, globalKeyId)
#define confComputeKeyStoreIsValidGlobalKeyId_HAL(pConfCompute, globalKeyId) confComputeKeyStoreIsValidGlobalKeyId_DISPATCH(pConfCompute, globalKeyId)
#define confComputeKeyStoreInit_FNPTR(pConfCompute) pConfCompute->__confComputeKeyStoreInit__
#define confComputeKeyStoreInit(pConfCompute) confComputeKeyStoreInit_DISPATCH(pConfCompute)
#define confComputeKeyStoreInit_HAL(pConfCompute) confComputeKeyStoreInit_DISPATCH(pConfCompute)
#define confComputeKeyStoreDeinit_FNPTR(pConfCompute) pConfCompute->__confComputeKeyStoreDeinit__
#define confComputeKeyStoreDeinit(pConfCompute) confComputeKeyStoreDeinit_DISPATCH(pConfCompute)
#define confComputeKeyStoreDeinit_HAL(pConfCompute) confComputeKeyStoreDeinit_DISPATCH(pConfCompute)
#define confComputeKeyStoreGetExportMasterKey_FNPTR(pConfCompute) pConfCompute->__confComputeKeyStoreGetExportMasterKey__
#define confComputeKeyStoreGetExportMasterKey(pConfCompute) confComputeKeyStoreGetExportMasterKey_DISPATCH(pConfCompute)
#define confComputeKeyStoreGetExportMasterKey_HAL(pConfCompute) confComputeKeyStoreGetExportMasterKey_DISPATCH(pConfCompute)
#define confComputeGetCurrentKeySeed_FNPTR(pConfCompute) pConfCompute->__confComputeGetCurrentKeySeed__
#define confComputeGetCurrentKeySeed(pConfCompute) confComputeGetCurrentKeySeed_DISPATCH(pConfCompute)
#define confComputeGetCurrentKeySeed_HAL(pConfCompute) confComputeGetCurrentKeySeed_DISPATCH(pConfCompute)
#define confComputeKeyStoreDeriveKey_FNPTR(pConfCompute) pConfCompute->__confComputeKeyStoreDeriveKey__
#define confComputeKeyStoreDeriveKey(pConfCompute, globalKeyId) confComputeKeyStoreDeriveKey_DISPATCH(pConfCompute, globalKeyId)
#define confComputeKeyStoreDeriveKey_HAL(pConfCompute, globalKeyId) confComputeKeyStoreDeriveKey_DISPATCH(pConfCompute, globalKeyId)
#define confComputeKeyStoreClearExportMasterKey_FNPTR(pConfCompute) pConfCompute->__confComputeKeyStoreClearExportMasterKey__
#define confComputeKeyStoreClearExportMasterKey(pConfCompute) confComputeKeyStoreClearExportMasterKey_DISPATCH(pConfCompute)
#define confComputeKeyStoreClearExportMasterKey_HAL(pConfCompute) confComputeKeyStoreClearExportMasterKey_DISPATCH(pConfCompute)
#define confComputeInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define confComputeInitMissing(pGpu, pEngstate) confComputeInitMissing_DISPATCH(pGpu, pEngstate)
#define confComputeStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define confComputeStatePreInitUnlocked(pGpu, pEngstate) confComputeStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define confComputeStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__
#define confComputeStateInitUnlocked(pGpu, pEngstate) confComputeStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define confComputeStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__
#define confComputeStatePreLoad(pGpu, pEngstate, arg3) confComputeStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define confComputeStateLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateLoad__
#define confComputeStateLoad(pGpu, pEngstate, arg3) confComputeStateLoad_DISPATCH(pGpu, pEngstate, arg3)
#define confComputeStateUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateUnload__
#define confComputeStateUnload(pGpu, pEngstate, arg3) confComputeStateUnload_DISPATCH(pGpu, pEngstate, arg3)
#define confComputeStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__
#define confComputeStatePostUnload(pGpu, pEngstate, arg3) confComputeStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define confComputeStateDestroy_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateDestroy__
#define confComputeStateDestroy(pGpu, pEngstate) confComputeStateDestroy_DISPATCH(pGpu, pEngstate)
#define confComputeIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__
#define confComputeIsPresent(pGpu, pEngstate) confComputeIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS confComputeConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, ENGDESCRIPTOR engDesc) {
    return pConfCompute->__nvoc_metadata_ptr->vtable.__confComputeConstructEngine__(pGpu, pConfCompute, engDesc);
}

static inline void confComputeDestruct_DISPATCH(struct ConfidentialCompute *pConfCompute) {
    pConfCompute->__confComputeDestruct__(pConfCompute);
}

static inline NV_STATUS confComputeStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    return pConfCompute->__nvoc_metadata_ptr->vtable.__confComputeStatePreInitLocked__(pGpu, pConfCompute);
}

static inline NV_STATUS confComputeStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    return pConfCompute->__nvoc_metadata_ptr->vtable.__confComputeStateInitLocked__(pGpu, pConfCompute);
}

static inline NV_STATUS confComputeStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvU32 flags) {
    return pConfCompute->__confComputeStatePostLoad__(pGpu, pConfCompute, flags);
}

static inline NV_STATUS confComputeStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvU32 flags) {
    return pConfCompute->__confComputeStatePreUnload__(pGpu, pConfCompute, flags);
}

static inline void confComputeSetErrorState_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    pConfCompute->__confComputeSetErrorState__(pGpu, pConfCompute);
}

static inline NV_STATUS confComputeKeyStoreDeriveViaChannel_DISPATCH(struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation) {
    return pConfCompute->__confComputeKeyStoreDeriveViaChannel__(pConfCompute, pKernelChannel, rotateOperation);
}

static inline NV_STATUS confComputeKeyStoreRetrieveViaChannel_DISPATCH(struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation, CHANNEL_IV_OPERATION ivOperation, CC_KMB *keyMaterialBundle) {
    return pConfCompute->__confComputeKeyStoreRetrieveViaChannel__(pConfCompute, pKernelChannel, rotateOperation, ivOperation, keyMaterialBundle);
}

static inline NV_STATUS confComputeKeyStoreRetrieveViaKeyId_DISPATCH(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId, ROTATE_IV_TYPE rotateOperation, NvBool bIncludeIvOrNonce, CC_KMB *keyMaterialBundle) {
    return pConfCompute->__confComputeKeyStoreRetrieveViaKeyId__(pConfCompute, globalKeyId, rotateOperation, bIncludeIvOrNonce, keyMaterialBundle);
}

static inline NV_STATUS confComputeDeriveSecretsForCEKeySpace_DISPATCH(struct ConfidentialCompute *pConfCompute, RM_ENGINE_TYPE ceRmEngineTypeIdx, NvU32 ccKeyspaceIndex) {
    return pConfCompute->__confComputeDeriveSecretsForCEKeySpace__(pConfCompute, ceRmEngineTypeIdx, ccKeyspaceIndex);
}

static inline NV_STATUS confComputeDeriveInitialKeySeed_DISPATCH(struct ConfidentialCompute *pConfCompute) {
    return pConfCompute->__confComputeDeriveInitialKeySeed__(pConfCompute);
}

static inline NV_STATUS confComputeGetAndUpdateCurrentKeySeed_DISPATCH(struct ConfidentialCompute *pConfCompute, NvU8 pKey[]) {
    return pConfCompute->__confComputeGetAndUpdateCurrentKeySeed__(pConfCompute, pKey);
}

static inline NV_STATUS confComputeDeriveSecrets_DISPATCH(struct ConfidentialCompute *pConfCompute, NvU32 engine) {
    return pConfCompute->__confComputeDeriveSecrets__(pConfCompute, engine);
}

static inline NV_STATUS confComputeRotationNotifHandler_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, RM_API *pRmApi, NvHandle hClient, struct KernelChannel *pKernelChannel, NvBool *bCheckKeyRotation, NvBool bSet) {
    return pConfCompute->__confComputeRotationNotifHandler__(pGpu, pConfCompute, pRmApi, hClient, pKernelChannel, bCheckKeyRotation, bSet);
}

static inline NV_STATUS confComputeUpdateSecrets_DISPATCH(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId) {
    return pConfCompute->__confComputeUpdateSecrets__(pConfCompute, globalKeyId);
}

static inline NV_STATUS confComputeUpdatePerChannelSecrets_DISPATCH(struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel) {
    return pConfCompute->__confComputeUpdatePerChannelSecrets__(pConfCompute, pKernelChannel);
}

static inline NvBool confComputeIsSpdmEnabled_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    return pConfCompute->__confComputeIsSpdmEnabled__(pGpu, pConfCompute);
}

static inline RM_ENGINE_TYPE confComputeGetEngineIdFromKeySpace_DISPATCH(struct ConfidentialCompute *pConfCompute, NvU32 keySpace) {
    return pConfCompute->__confComputeGetEngineIdFromKeySpace__(pConfCompute, keySpace);
}

static inline NV_STATUS confComputeGetKeySpaceFromKChannel_DISPATCH(struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel, NvU16 *keyspace) {
    return pConfCompute->__confComputeGetKeySpaceFromKChannel__(pConfCompute, pKernelChannel, keyspace);
}

static inline NV_STATUS confComputeGetLceKeyIdFromKChannel_DISPATCH(struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation, NvU16 *pKeyId) {
    return pConfCompute->__confComputeGetLceKeyIdFromKChannel__(pConfCompute, pKernelChannel, rotateOperation, pKeyId);
}

static inline NvU32 confComputeGetMaxCeKeySpaceIdx_DISPATCH(struct ConfidentialCompute *pConfCompute) {
    return pConfCompute->__confComputeGetMaxCeKeySpaceIdx__(pConfCompute);
}

static inline NvBool confComputeGlobalKeyIsKernelPriv_DISPATCH(struct ConfidentialCompute *pConfCompute, NvU32 keyId) {
    return pConfCompute->__confComputeGlobalKeyIsKernelPriv__(pConfCompute, keyId);
}

static inline NvBool confComputeGlobalKeyIsUvmKey_DISPATCH(struct ConfidentialCompute *pConfCompute, NvU32 keyId) {
    return pConfCompute->__confComputeGlobalKeyIsUvmKey__(pConfCompute, keyId);
}

static inline NV_STATUS confComputeGetKeyPairByChannel_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel, NvU32 *pH2DKey, NvU32 *pD2HKey) {
    return pConfCompute->__confComputeGetKeyPairByChannel__(pGpu, pConfCompute, pKernelChannel, pH2DKey, pD2HKey);
}

static inline NV_STATUS confComputeTriggerKeyRotation_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    return pConfCompute->__confComputeTriggerKeyRotation__(pGpu, pConfCompute);
}

static inline void confComputeGetKeyPairForKeySpace_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvU32 arg3, NvBool arg4, NvU32 *arg5, NvU32 *arg6) {
    pConfCompute->__confComputeGetKeyPairForKeySpace__(pGpu, pConfCompute, arg3, arg4, arg5, arg6);
}

static inline NV_STATUS confComputeEnableKeyRotationCallback_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvBool bEnable) {
    return pConfCompute->__confComputeEnableKeyRotationCallback__(pGpu, pConfCompute, bEnable);
}

static inline NV_STATUS confComputeEnableKeyRotationSupport_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    return pConfCompute->__confComputeEnableKeyRotationSupport__(pGpu, pConfCompute);
}

static inline NV_STATUS confComputeEnableInternalKeyRotationSupport_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    return pConfCompute->__confComputeEnableInternalKeyRotationSupport__(pGpu, pConfCompute);
}

static inline NvBool confComputeIsDebugModeEnabled_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    return pConfCompute->__confComputeIsDebugModeEnabled__(pGpu, pConfCompute);
}

static inline NvBool confComputeIsGpuCcCapable_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    return pConfCompute->__confComputeIsGpuCcCapable__(pGpu, pConfCompute);
}

static inline NV_STATUS confComputeTestPlatformSupport_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    return pConfCompute->__confComputeTestPlatformSupport__(pGpu, pConfCompute);
}

static inline NV_STATUS confComputeDeriveSessionKeys_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute) {
    return pConfCompute->__confComputeDeriveSessionKeys__(pGpu, pConfCompute);
}

static inline void confComputeKeyStoreDepositIvMask_DISPATCH(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId, void *ivMask) {
    pConfCompute->__confComputeKeyStoreDepositIvMask__(pConfCompute, globalKeyId, ivMask);
}

static inline NV_STATUS confComputeKeyStoreUpdateKey_DISPATCH(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId) {
    return pConfCompute->__confComputeKeyStoreUpdateKey__(pConfCompute, globalKeyId);
}

static inline NvBool confComputeKeyStoreIsValidGlobalKeyId_DISPATCH(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId) {
    return pConfCompute->__confComputeKeyStoreIsValidGlobalKeyId__(pConfCompute, globalKeyId);
}

static inline NV_STATUS confComputeKeyStoreInit_DISPATCH(struct ConfidentialCompute *pConfCompute) {
    return pConfCompute->__confComputeKeyStoreInit__(pConfCompute);
}

static inline void confComputeKeyStoreDeinit_DISPATCH(struct ConfidentialCompute *pConfCompute) {
    pConfCompute->__confComputeKeyStoreDeinit__(pConfCompute);
}

static inline void * confComputeKeyStoreGetExportMasterKey_DISPATCH(struct ConfidentialCompute *pConfCompute) {
    return pConfCompute->__confComputeKeyStoreGetExportMasterKey__(pConfCompute);
}

static inline void * confComputeGetCurrentKeySeed_DISPATCH(struct ConfidentialCompute *pConfCompute) {
    return pConfCompute->__confComputeGetCurrentKeySeed__(pConfCompute);
}

static inline NV_STATUS confComputeKeyStoreDeriveKey_DISPATCH(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId) {
    return pConfCompute->__confComputeKeyStoreDeriveKey__(pConfCompute, globalKeyId);
}

static inline void confComputeKeyStoreClearExportMasterKey_DISPATCH(struct ConfidentialCompute *pConfCompute) {
    pConfCompute->__confComputeKeyStoreClearExportMasterKey__(pConfCompute);
}

static inline void confComputeInitMissing_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__confComputeInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS confComputeStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__confComputeStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS confComputeStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__confComputeStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS confComputeStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__confComputeStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS confComputeStateLoad_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__confComputeStateLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS confComputeStateUnload_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__confComputeStateUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS confComputeStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__confComputeStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline void confComputeStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__confComputeStateDestroy__(pGpu, pEngstate);
}

static inline NvBool confComputeIsPresent_DISPATCH(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__confComputeIsPresent__(pGpu, pEngstate);
}

// Virtual method declarations and/or inline definitions
NV_STATUS confComputeConstructEngine_IMPL(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, ENGDESCRIPTOR engDesc);

NV_STATUS confComputeStateInitLocked_IMPL(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute);

// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
void confComputeDestruct_KERNEL(struct ConfidentialCompute *pConfCompute);

NV_STATUS confComputeStatePostLoad_IMPL(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvU32 flags);

NV_STATUS confComputeStatePreUnload_KERNEL(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvU32 flags);

void confComputeSetErrorState_KERNEL(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute);

NV_STATUS confComputeKeyStoreDeriveViaChannel_GB100(struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation);

NV_STATUS confComputeKeyStoreRetrieveViaChannel_GH100(struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation, CHANNEL_IV_OPERATION ivOperation, CC_KMB *keyMaterialBundle);

NV_STATUS confComputeKeyStoreRetrieveViaChannel_GB100(struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation, CHANNEL_IV_OPERATION ivOperation, CC_KMB *keyMaterialBundle);

NV_STATUS confComputeKeyStoreRetrieveViaKeyId_GH100(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId, ROTATE_IV_TYPE rotateOperation, NvBool bIncludeIvOrNonce, CC_KMB *keyMaterialBundle);

NV_STATUS confComputeDeriveSecretsForCEKeySpace_GH100(struct ConfidentialCompute *pConfCompute, RM_ENGINE_TYPE ceRmEngineTypeIdx, NvU32 ccKeyspaceIndex);

NV_STATUS confComputeDeriveInitialKeySeed_GB100(struct ConfidentialCompute *pConfCompute);

NV_STATUS confComputeGetAndUpdateCurrentKeySeed_GB100(struct ConfidentialCompute *pConfCompute, NvU8 pKey[]);

NV_STATUS confComputeDeriveSecrets_GH100(struct ConfidentialCompute *pConfCompute, NvU32 engine);

NV_STATUS confComputeDeriveSecrets_GB100(struct ConfidentialCompute *pConfCompute, NvU32 engine);

NV_STATUS confComputeRotationNotifHandler_GB100(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, RM_API *pRmApi, NvHandle hClient, struct KernelChannel *pKernelChannel, NvBool *bCheckKeyRotation, NvBool bSet);

NV_STATUS confComputeRotationNotifHandler_GH100(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, RM_API *pRmApi, NvHandle hClient, struct KernelChannel *pKernelChannel, NvBool *bCheckKeyRotation, NvBool bSet);

NV_STATUS confComputeUpdateSecrets_GH100(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId);

NV_STATUS confComputeUpdatePerChannelSecrets_GB100(struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel);

RM_ENGINE_TYPE confComputeGetEngineIdFromKeySpace_GH100(struct ConfidentialCompute *pConfCompute, NvU32 keySpace);

RM_ENGINE_TYPE confComputeGetEngineIdFromKeySpace_GB100(struct ConfidentialCompute *pConfCompute, NvU32 keySpace);

NV_STATUS confComputeGetKeySpaceFromKChannel_GH100(struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel, NvU16 *keyspace);

NV_STATUS confComputeGetKeySpaceFromKChannel_GB100(struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel, NvU16 *keyspace);

NV_STATUS confComputeGetLceKeyIdFromKChannel_GH100(struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation, NvU16 *pKeyId);

NV_STATUS confComputeGetLceKeyIdFromKChannel_GB100(struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation, NvU16 *pKeyId);

NvBool confComputeGlobalKeyIsKernelPriv_GH100(struct ConfidentialCompute *pConfCompute, NvU32 keyId);

NvBool confComputeGlobalKeyIsUvmKey_GH100(struct ConfidentialCompute *pConfCompute, NvU32 keyId);

NV_STATUS confComputeGetKeyPairByChannel_GH100(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel, NvU32 *pH2DKey, NvU32 *pD2HKey);

NV_STATUS confComputeTriggerKeyRotation_GH100(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute);

void confComputeGetKeyPairForKeySpace_GH100(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvU32 arg3, NvBool arg4, NvU32 *arg5, NvU32 *arg6);

NV_STATUS confComputeEnableKeyRotationCallback_GH100(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvBool bEnable);

NV_STATUS confComputeEnableKeyRotationSupport_GH100(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute);

NV_STATUS confComputeEnableKeyRotationSupport_GB100(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute);

NV_STATUS confComputeEnableInternalKeyRotationSupport_GH100(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute);

NvBool confComputeIsDebugModeEnabled_GH100(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute);

NvBool confComputeIsGpuCcCapable_GH100(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute);

NvBool confComputeIsGpuCcCapable_GB100(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute);

NV_STATUS confComputeTestPlatformSupport_GH100(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute);

NV_STATUS confComputeDeriveSessionKeys_KERNEL(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute);

void confComputeKeyStoreDepositIvMask_GH100(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId, void *ivMask);

NV_STATUS confComputeKeyStoreUpdateKey_GH100(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId);

NvBool confComputeKeyStoreIsValidGlobalKeyId_GH100(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId);

NV_STATUS confComputeKeyStoreInit_GH100(struct ConfidentialCompute *pConfCompute);

void confComputeKeyStoreDeinit_GH100(struct ConfidentialCompute *pConfCompute);

void * confComputeKeyStoreGetExportMasterKey_GH100(struct ConfidentialCompute *pConfCompute);

void * confComputeGetCurrentKeySeed_GB100(struct ConfidentialCompute *pConfCompute);

NV_STATUS confComputeKeyStoreDeriveKey_GH100(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId);

void confComputeKeyStoreClearExportMasterKey_GH100(struct ConfidentialCompute *pConfCompute);

// Inline HAL method definitions
static inline void confComputeDestruct_d44104(struct ConfidentialCompute *pConfCompute){
    return;
}

static inline NV_STATUS confComputeStatePreInitLocked_ac1694(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute){
    return NV_OK;
}

static inline NV_STATUS confComputeStatePostLoad_ac1694(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvU32 flags){
    return NV_OK;
}

static inline NV_STATUS confComputeStatePreUnload_ac1694(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvU32 flags){
    return NV_OK;
}

static inline void confComputeSetErrorState_d44104(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute){
    return;
}

static inline NV_STATUS confComputeKeyStoreDeriveViaChannel_ac1694(struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation){
    return NV_OK;
}

static inline NV_STATUS confComputeKeyStoreDeriveViaChannel_395e98(struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation){
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS confComputeKeyStoreRetrieveViaChannel_395e98(struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation, CHANNEL_IV_OPERATION ivOperation, CC_KMB *keyMaterialBundle){
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS confComputeKeyStoreRetrieveViaKeyId_395e98(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId, ROTATE_IV_TYPE rotateOperation, NvBool bIncludeIvOrNonce, CC_KMB *keyMaterialBundle){
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS confComputeDeriveSecretsForCEKeySpace_395e98(struct ConfidentialCompute *pConfCompute, RM_ENGINE_TYPE ceRmEngineTypeIdx, NvU32 ccKeyspaceIndex){
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS confComputeDeriveInitialKeySeed_ac1694(struct ConfidentialCompute *pConfCompute){
    return NV_OK;
}

static inline NV_STATUS confComputeDeriveInitialKeySeed_395e98(struct ConfidentialCompute *pConfCompute){
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS confComputeGetAndUpdateCurrentKeySeed_395e98(struct ConfidentialCompute *pConfCompute, NvU8 pKey[]){
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS confComputeDeriveSecrets_395e98(struct ConfidentialCompute *pConfCompute, NvU32 engine){
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS confComputeUpdateSecrets_395e98(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId){
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS confComputeUpdatePerChannelSecrets_395e98(struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel){
    return NV_ERR_NOT_SUPPORTED;
}

static inline NvBool confComputeIsSpdmEnabled_e661f0(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute){
    return NV_TRUE;
}

static inline NvBool confComputeIsSpdmEnabled_d69453(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute){
    return NV_FALSE;
}

static inline RM_ENGINE_TYPE confComputeGetEngineIdFromKeySpace_4f6cff(struct ConfidentialCompute *pConfCompute, NvU32 keySpace){
    return RM_ENGINE_TYPE_NULL;
}

static inline NV_STATUS confComputeGetKeySpaceFromKChannel_395e98(struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel, NvU16 *keyspace){
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS confComputeGetLceKeyIdFromKChannel_395e98(struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel, ROTATE_IV_TYPE rotateOperation, NvU16 *pKeyId){
    return NV_ERR_NOT_SUPPORTED;
}

static inline NvU32 confComputeGetMaxCeKeySpaceIdx_a3eb5b(struct ConfidentialCompute *pConfCompute){
    return CC_KEYSPACE_LCE7;
}

static inline NvU32 confComputeGetMaxCeKeySpaceIdx_ca9aa5(struct ConfidentialCompute *pConfCompute){
    return CC_KEYSPACE_LCE17;
}

static inline NvU32 confComputeGetMaxCeKeySpaceIdx_b2b553(struct ConfidentialCompute *pConfCompute){
    return 0;
}

static inline NvBool confComputeGlobalKeyIsKernelPriv_d69453(struct ConfidentialCompute *pConfCompute, NvU32 keyId){
    return NV_FALSE;
}

static inline NvBool confComputeGlobalKeyIsUvmKey_d69453(struct ConfidentialCompute *pConfCompute, NvU32 keyId){
    return NV_FALSE;
}

static inline NV_STATUS confComputeGetKeyPairByChannel_395e98(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, struct KernelChannel *pKernelChannel, NvU32 *pH2DKey, NvU32 *pD2HKey){
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS confComputeTriggerKeyRotation_395e98(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute){
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS confComputeTriggerKeyRotation_ac1694(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute){
    return NV_OK;
}

static inline void confComputeGetKeyPairForKeySpace_d44104(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvU32 arg3, NvBool arg4, NvU32 *arg5, NvU32 *arg6){
    return;
}

static inline NV_STATUS confComputeEnableKeyRotationCallback_ac1694(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute, NvBool bEnable){
    return NV_OK;
}

static inline NV_STATUS confComputeEnableKeyRotationSupport_ac1694(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute){
    return NV_OK;
}

static inline NV_STATUS confComputeEnableInternalKeyRotationSupport_ac1694(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute){
    return NV_OK;
}

static inline NvBool confComputeIsDebugModeEnabled_e661f0(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute){
    return NV_TRUE;
}

static inline NvBool confComputeIsGpuCcCapable_d69453(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute){
    return NV_FALSE;
}

static inline NV_STATUS confComputeTestPlatformSupport_ac1694(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute){
    return NV_OK;
}

static inline NV_STATUS confComputeDeriveSessionKeys_395e98(struct OBJGPU *pGpu, struct ConfidentialCompute *pConfCompute){
    return NV_ERR_NOT_SUPPORTED;
}

static inline void confComputeKeyStoreDepositIvMask_d44104(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId, void *ivMask){
    return;
}

static inline NV_STATUS confComputeKeyStoreUpdateKey_395e98(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId){
    return NV_ERR_NOT_SUPPORTED;
}

static inline NvBool confComputeKeyStoreIsValidGlobalKeyId_d69453(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId){
    return NV_FALSE;
}

static inline NV_STATUS confComputeKeyStoreInit_395e98(struct ConfidentialCompute *pConfCompute){
    return NV_ERR_NOT_SUPPORTED;
}

static inline void confComputeKeyStoreDeinit_d44104(struct ConfidentialCompute *pConfCompute){
    return;
}

static inline void * confComputeKeyStoreGetExportMasterKey_9e2234(struct ConfidentialCompute *pConfCompute){
    return ((void *)0);
}

static inline void * confComputeGetCurrentKeySeed_9e2234(struct ConfidentialCompute *pConfCompute){
    return ((void *)0);
}

static inline NV_STATUS confComputeKeyStoreDeriveKey_395e98(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId){
    return NV_ERR_NOT_SUPPORTED;
}

static inline void confComputeKeyStoreClearExportMasterKey_d44104(struct ConfidentialCompute *pConfCompute){
    return;
}

// Static dispatch method declarations
// Static inline method definitions
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

#undef confComputeGetCurrentKeySeed
void *NVOC_PRIVATE_FUNCTION(confComputeGetCurrentKeySeed)(struct ConfidentialCompute *pConfCompute);

#undef confComputeGetCurrentKeySeed_HAL
void *NVOC_PRIVATE_FUNCTION(confComputeGetCurrentKeySeed_HAL)(struct ConfidentialCompute *pConfCompute);

#undef confComputeKeyStoreDeriveKey
NV_STATUS NVOC_PRIVATE_FUNCTION(confComputeKeyStoreDeriveKey)(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId);

#undef confComputeKeyStoreDeriveKey_HAL
NV_STATUS NVOC_PRIVATE_FUNCTION(confComputeKeyStoreDeriveKey_HAL)(struct ConfidentialCompute *pConfCompute, NvU32 globalKeyId);

#undef confComputeKeyStoreClearExportMasterKey
void NVOC_PRIVATE_FUNCTION(confComputeKeyStoreClearExportMasterKey)(struct ConfidentialCompute *pConfCompute);

#undef confComputeKeyStoreClearExportMasterKey_HAL
void NVOC_PRIVATE_FUNCTION(confComputeKeyStoreClearExportMasterKey_HAL)(struct ConfidentialCompute *pConfCompute);

#endif // NVOC_CONF_COMPUTE_H_PRIVATE_ACCESS_ALLOWED


/*!
 * @brief 1Hz callback function to perform key rotation
 *
 * @param[in]     pGpu                     : OBJGPU Pointer
 * @param[in]     data                     : void Pointer
 */
void confComputeKeyRotationCallback(struct OBJGPU *pGpu, void *data);
#endif // CONF_COMPUTE_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_CONF_COMPUTE_NVOC_H_
