#ifndef _G_SPDM_NVOC_H_
#define _G_SPDM_NVOC_H_
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

#include "g_spdm_nvoc.h"

#ifndef NV_SPDM_H
#define NV_SPDM_H
// SPDM_H may conflict with libspdm headers.

#include "gpu/gpu.h"
#include "nvport/nvport.h"

/****************************************************************************\
 *                                                                           *
 *      SPDM module header.                                                  *
 *                                                                           *
 ****************************************************************************/

/* ------------------------ Macros ----------------------------------------- */
#define HAL_MAPPING_ONLY_IF_SPDM_SUPPORTED_IMPL \
    {                                           \
        default: _IMPL;                         \
    }
#define HAL_MAPPING_ONLY_IF_SPDM_SUPPORTED_HAL  \
    {                                           \
        GH100   : _GH100;                       \
        default : return NV_ERR_NOT_SUPPORTED;  \
    }
#define HAL_MAPPING_ONLY_IF_SPDM_SUPPORTED_VOID \
    {                                           \
        default: _IMPL;                         \
    }

#ifdef NVOC_SPDM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct Spdm {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Object __nvoc_base_Object;
    struct Object *__nvoc_pbase_Object;
    struct Spdm *__nvoc_pbase_Spdm;
    NV_STATUS (*__spdmGetCertChains__)(struct OBJGPU *, struct Spdm *, void *, NvU32 *, void *, NvU32 *);
    NV_STATUS (*__spdmGetAttestationReport__)(struct OBJGPU *, struct Spdm *, NvU8 *, void *, NvU32 *, NvBool *, void *, NvU32 *);
    NV_STATUS (*__spdmDeviceInit__)(struct OBJGPU *, struct Spdm *);
    NV_STATUS (*__spdmDeviceDeinit__)(struct OBJGPU *, struct Spdm *, NvBool);
    NV_STATUS (*__spdmDeviceSecuredSessionSupported__)(struct OBJGPU *, struct Spdm *);
    NV_STATUS (*__spdmCheckConnection__)(struct OBJGPU *, struct Spdm *);
    NV_STATUS (*__spdmMessageProcess__)(struct OBJGPU *, struct Spdm *, NvU8 *, NvU32, NvU8 *, NvU32 *);
    NV_STATUS (*__spdmGetCertificates__)(struct OBJGPU *, struct Spdm *);
    void *pLibspdmContext;
    NvU32 libspdmContextSize;
    void *pLibspdmScratch;
    NvU32 libspdmScratchSize;
    NvU32 sessionId;
    NvU32 guestId;
    NvU32 endpointId;
    NvU8 *pAttestationCertChain;
    size_t attestationCertChainSize;
    void *pDeviceIOContext;
    void *pLastExchange;
    size_t lastExchangeSize;
    NvBool bExportSecretCleared;
};

#ifndef __NVOC_CLASS_Spdm_TYPEDEF__
#define __NVOC_CLASS_Spdm_TYPEDEF__
typedef struct Spdm Spdm;
#endif /* __NVOC_CLASS_Spdm_TYPEDEF__ */

#ifndef __nvoc_class_id_Spdm
#define __nvoc_class_id_Spdm 0x261ee8
#endif /* __nvoc_class_id_Spdm */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Spdm;

#define __staticCast_Spdm(pThis) \
    ((pThis)->__nvoc_pbase_Spdm)

#ifdef __nvoc_spdm_h_disabled
#define __dynamicCast_Spdm(pThis) ((Spdm*)NULL)
#else //__nvoc_spdm_h_disabled
#define __dynamicCast_Spdm(pThis) \
    ((Spdm*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Spdm)))
#endif //__nvoc_spdm_h_disabled


NV_STATUS __nvoc_objCreateDynamic_Spdm(Spdm**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Spdm(Spdm**, Dynamic*, NvU32);
#define __objCreate_Spdm(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_Spdm((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define spdmGetCertChains(pGpu, pSpdm, pKeyExCertChain, pKeyExCertChainSize, pAttestationCertChain, pAttestationCertChainSize) spdmGetCertChains_DISPATCH(pGpu, pSpdm, pKeyExCertChain, pKeyExCertChainSize, pAttestationCertChain, pAttestationCertChainSize)
#define spdmGetCertChains_HAL(pGpu, pSpdm, pKeyExCertChain, pKeyExCertChainSize, pAttestationCertChain, pAttestationCertChainSize) spdmGetCertChains_DISPATCH(pGpu, pSpdm, pKeyExCertChain, pKeyExCertChainSize, pAttestationCertChain, pAttestationCertChainSize)
#define spdmGetAttestationReport(pGpu, pSpdm, pNonce, pAttestationReport, pAttestationReportSize, pbIsCecAttestationReportPresent, pCecAttestationReport, pCecAttestationReportSize) spdmGetAttestationReport_DISPATCH(pGpu, pSpdm, pNonce, pAttestationReport, pAttestationReportSize, pbIsCecAttestationReportPresent, pCecAttestationReport, pCecAttestationReportSize)
#define spdmGetAttestationReport_HAL(pGpu, pSpdm, pNonce, pAttestationReport, pAttestationReportSize, pbIsCecAttestationReportPresent, pCecAttestationReport, pCecAttestationReportSize) spdmGetAttestationReport_DISPATCH(pGpu, pSpdm, pNonce, pAttestationReport, pAttestationReportSize, pbIsCecAttestationReportPresent, pCecAttestationReport, pCecAttestationReportSize)
#define spdmDeviceInit(pGpu, pSpdm) spdmDeviceInit_DISPATCH(pGpu, pSpdm)
#define spdmDeviceInit_HAL(pGpu, pSpdm) spdmDeviceInit_DISPATCH(pGpu, pSpdm)
#define spdmDeviceDeinit(pGpu, pSpdm, bForceClear) spdmDeviceDeinit_DISPATCH(pGpu, pSpdm, bForceClear)
#define spdmDeviceDeinit_HAL(pGpu, pSpdm, bForceClear) spdmDeviceDeinit_DISPATCH(pGpu, pSpdm, bForceClear)
#define spdmDeviceSecuredSessionSupported(pGpu, pSpdm) spdmDeviceSecuredSessionSupported_DISPATCH(pGpu, pSpdm)
#define spdmDeviceSecuredSessionSupported_HAL(pGpu, pSpdm) spdmDeviceSecuredSessionSupported_DISPATCH(pGpu, pSpdm)
#define spdmCheckConnection(pGpu, pSpdm) spdmCheckConnection_DISPATCH(pGpu, pSpdm)
#define spdmCheckConnection_HAL(pGpu, pSpdm) spdmCheckConnection_DISPATCH(pGpu, pSpdm)
#define spdmMessageProcess(pGpu, pSpdm, pRequest, requestSize, pResponse, pResponseSize) spdmMessageProcess_DISPATCH(pGpu, pSpdm, pRequest, requestSize, pResponse, pResponseSize)
#define spdmMessageProcess_HAL(pGpu, pSpdm, pRequest, requestSize, pResponse, pResponseSize) spdmMessageProcess_DISPATCH(pGpu, pSpdm, pRequest, requestSize, pResponse, pResponseSize)
#define spdmGetCertificates(pGpu, pSpdm) spdmGetCertificates_DISPATCH(pGpu, pSpdm)
#define spdmGetCertificates_HAL(pGpu, pSpdm) spdmGetCertificates_DISPATCH(pGpu, pSpdm)
NV_STATUS spdmConstruct_IMPL(struct Spdm *arg_pSpdm);


#define __nvoc_spdmConstruct(arg_pSpdm) spdmConstruct_IMPL(arg_pSpdm)
void spdmDestruct_IMPL(struct Spdm *pSpdm);


#define __nvoc_spdmDestruct(pSpdm) spdmDestruct_IMPL(pSpdm)
NV_STATUS spdmContextInit_IMPL(struct OBJGPU *pGpu, struct Spdm *pSpdm);


#ifdef __nvoc_spdm_h_disabled
static inline NV_STATUS spdmContextInit(struct OBJGPU *pGpu, struct Spdm *pSpdm) {
    NV_ASSERT_FAILED_PRECOMP("Spdm was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_spdm_h_disabled
#define spdmContextInit(pGpu, pSpdm) spdmContextInit_IMPL(pGpu, pSpdm)
#endif //__nvoc_spdm_h_disabled

#define spdmContextInit_HAL(pGpu, pSpdm) spdmContextInit(pGpu, pSpdm)

NV_STATUS spdmContextDeinit_IMPL(struct OBJGPU *pGpu, struct Spdm *pSpdm, NvBool bForceClear);


#ifdef __nvoc_spdm_h_disabled
static inline NV_STATUS spdmContextDeinit(struct OBJGPU *pGpu, struct Spdm *pSpdm, NvBool bForceClear) {
    NV_ASSERT_FAILED_PRECOMP("Spdm was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_spdm_h_disabled
#define spdmContextDeinit(pGpu, pSpdm, bForceClear) spdmContextDeinit_IMPL(pGpu, pSpdm, bForceClear)
#endif //__nvoc_spdm_h_disabled

#define spdmContextDeinit_HAL(pGpu, pSpdm, bForceClear) spdmContextDeinit(pGpu, pSpdm, bForceClear)

NV_STATUS spdmStart_IMPL(struct OBJGPU *pGpu, struct Spdm *pSpdm);


#ifdef __nvoc_spdm_h_disabled
static inline NV_STATUS spdmStart(struct OBJGPU *pGpu, struct Spdm *pSpdm) {
    NV_ASSERT_FAILED_PRECOMP("Spdm was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_spdm_h_disabled
#define spdmStart(pGpu, pSpdm) spdmStart_IMPL(pGpu, pSpdm)
#endif //__nvoc_spdm_h_disabled

#define spdmStart_HAL(pGpu, pSpdm) spdmStart(pGpu, pSpdm)

NV_STATUS spdmRetrieveExportSecret_IMPL(struct OBJGPU *pGpu, struct Spdm *pSpdm, NvU32 keySize, NvU8 *pKeyOut);


#ifdef __nvoc_spdm_h_disabled
static inline NV_STATUS spdmRetrieveExportSecret(struct OBJGPU *pGpu, struct Spdm *pSpdm, NvU32 keySize, NvU8 *pKeyOut) {
    NV_ASSERT_FAILED_PRECOMP("Spdm was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_spdm_h_disabled
#define spdmRetrieveExportSecret(pGpu, pSpdm, keySize, pKeyOut) spdmRetrieveExportSecret_IMPL(pGpu, pSpdm, keySize, pKeyOut)
#endif //__nvoc_spdm_h_disabled

#define spdmRetrieveExportSecret_HAL(pGpu, pSpdm, keySize, pKeyOut) spdmRetrieveExportSecret(pGpu, pSpdm, keySize, pKeyOut)

NV_STATUS spdmGetCertChains_GH100(struct OBJGPU *pGpu, struct Spdm *pSpdm, void *pKeyExCertChain, NvU32 *pKeyExCertChainSize, void *pAttestationCertChain, NvU32 *pAttestationCertChainSize);

static inline NV_STATUS spdmGetCertChains_46f6a7(struct OBJGPU *pGpu, struct Spdm *pSpdm, void *pKeyExCertChain, NvU32 *pKeyExCertChainSize, void *pAttestationCertChain, NvU32 *pAttestationCertChainSize) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS spdmGetCertChains_DISPATCH(struct OBJGPU *pGpu, struct Spdm *pSpdm, void *pKeyExCertChain, NvU32 *pKeyExCertChainSize, void *pAttestationCertChain, NvU32 *pAttestationCertChainSize) {
    return pSpdm->__spdmGetCertChains__(pGpu, pSpdm, pKeyExCertChain, pKeyExCertChainSize, pAttestationCertChain, pAttestationCertChainSize);
}

NV_STATUS spdmGetAttestationReport_GH100(struct OBJGPU *pGpu, struct Spdm *pSpdm, NvU8 *pNonce, void *pAttestationReport, NvU32 *pAttestationReportSize, NvBool *pbIsCecAttestationReportPresent, void *pCecAttestationReport, NvU32 *pCecAttestationReportSize);

static inline NV_STATUS spdmGetAttestationReport_46f6a7(struct OBJGPU *pGpu, struct Spdm *pSpdm, NvU8 *pNonce, void *pAttestationReport, NvU32 *pAttestationReportSize, NvBool *pbIsCecAttestationReportPresent, void *pCecAttestationReport, NvU32 *pCecAttestationReportSize) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS spdmGetAttestationReport_DISPATCH(struct OBJGPU *pGpu, struct Spdm *pSpdm, NvU8 *pNonce, void *pAttestationReport, NvU32 *pAttestationReportSize, NvBool *pbIsCecAttestationReportPresent, void *pCecAttestationReport, NvU32 *pCecAttestationReportSize) {
    return pSpdm->__spdmGetAttestationReport__(pGpu, pSpdm, pNonce, pAttestationReport, pAttestationReportSize, pbIsCecAttestationReportPresent, pCecAttestationReport, pCecAttestationReportSize);
}

NV_STATUS spdmDeviceInit_GH100(struct OBJGPU *pGpu, struct Spdm *pSpdm);

static inline NV_STATUS spdmDeviceInit_46f6a7(struct OBJGPU *pGpu, struct Spdm *pSpdm) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS spdmDeviceInit_DISPATCH(struct OBJGPU *pGpu, struct Spdm *pSpdm) {
    return pSpdm->__spdmDeviceInit__(pGpu, pSpdm);
}

NV_STATUS spdmDeviceDeinit_GH100(struct OBJGPU *pGpu, struct Spdm *pSpdm, NvBool bForceClear);

static inline NV_STATUS spdmDeviceDeinit_46f6a7(struct OBJGPU *pGpu, struct Spdm *pSpdm, NvBool bForceClear) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS spdmDeviceDeinit_DISPATCH(struct OBJGPU *pGpu, struct Spdm *pSpdm, NvBool bForceClear) {
    return pSpdm->__spdmDeviceDeinit__(pGpu, pSpdm, bForceClear);
}

NV_STATUS spdmDeviceSecuredSessionSupported_GH100(struct OBJGPU *pGpu, struct Spdm *pSpdm);

static inline NV_STATUS spdmDeviceSecuredSessionSupported_46f6a7(struct OBJGPU *pGpu, struct Spdm *pSpdm) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS spdmDeviceSecuredSessionSupported_DISPATCH(struct OBJGPU *pGpu, struct Spdm *pSpdm) {
    return pSpdm->__spdmDeviceSecuredSessionSupported__(pGpu, pSpdm);
}

NV_STATUS spdmCheckConnection_GH100(struct OBJGPU *pGpu, struct Spdm *pSpdm);

static inline NV_STATUS spdmCheckConnection_46f6a7(struct OBJGPU *pGpu, struct Spdm *pSpdm) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS spdmCheckConnection_DISPATCH(struct OBJGPU *pGpu, struct Spdm *pSpdm) {
    return pSpdm->__spdmCheckConnection__(pGpu, pSpdm);
}

NV_STATUS spdmMessageProcess_GH100(struct OBJGPU *pGpu, struct Spdm *pSpdm, NvU8 *pRequest, NvU32 requestSize, NvU8 *pResponse, NvU32 *pResponseSize);

static inline NV_STATUS spdmMessageProcess_46f6a7(struct OBJGPU *pGpu, struct Spdm *pSpdm, NvU8 *pRequest, NvU32 requestSize, NvU8 *pResponse, NvU32 *pResponseSize) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS spdmMessageProcess_DISPATCH(struct OBJGPU *pGpu, struct Spdm *pSpdm, NvU8 *pRequest, NvU32 requestSize, NvU8 *pResponse, NvU32 *pResponseSize) {
    return pSpdm->__spdmMessageProcess__(pGpu, pSpdm, pRequest, requestSize, pResponse, pResponseSize);
}

NV_STATUS spdmGetCertificates_GH100(struct OBJGPU *pGpu, struct Spdm *pSpdm);

static inline NV_STATUS spdmGetCertificates_46f6a7(struct OBJGPU *pGpu, struct Spdm *pSpdm) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS spdmGetCertificates_DISPATCH(struct OBJGPU *pGpu, struct Spdm *pSpdm) {
    return pSpdm->__spdmGetCertificates__(pGpu, pSpdm);
}

#undef PRIVATE_FIELD


#endif // NV_SPDM_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_SPDM_NVOC_H_
