#ifndef _G_KERNEL_FALCON_NVOC_H_
#define _G_KERNEL_FALCON_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * Provides definitions for all KernelFalcon data structures and
 * interfaces.
 */

#include "g_kernel_falcon_nvoc.h"

#ifndef KERNEL_FALCON_H
#define KERNEL_FALCON_H

#include "core/core.h"
#include "gpu/falcon/falcon_common.h"
#include "gpu/intr/intr_service.h"

struct KernelChannel;

#ifndef __NVOC_CLASS_KernelChannel_TYPEDEF__
#define __NVOC_CLASS_KernelChannel_TYPEDEF__
typedef struct KernelChannel KernelChannel;
#endif /* __NVOC_CLASS_KernelChannel_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelChannel
#define __nvoc_class_id_KernelChannel 0x5d8d70
#endif /* __nvoc_class_id_KernelChannel */



typedef struct KernelFalconEngineConfig {
    NvU32 registerBase;        // i.e. NV_P{GSP,SEC,NVDEC}
    NvU32 riscvRegisterBase;   // i.e. NV_FALCON2_{GSP,SEC,NVDEC}_BASE
    NvU32 fbifBase;            // i.e. NV_P{GSP,SEC,NVDEC}_FBIF_BASE
    NvBool bBootFromHs;        // whether engine has Boot-from-HS (true for HS-capable engines GA10X+)
    NvU32 pmcEnableMask;       // engine's enable bitmask in PMC (or 0 if engine reset is not in PMC)
    NvU32 bIsPmcDeviceEngine;  // whether engine's enable bit is in NV_PMC_DEVICE_ENABLE (vs. NV_PMC_ENABLE)
    ENGDESCRIPTOR physEngDesc; // The engine descriptor for the falcon (e.g. ENG_SEC2)
    NvU32 ctxAttr;             // Memory attributes used for context buffers
    NvU32 ctxBufferSize;       // Context buffer size in bytes
    NvU32 addrSpaceList;       // index into ADDRLIST array in mem_desc.h
} KernelFalconEngineConfig;

/*!
 * Base class for booting Falcon cores (including RISC-V)
 */
#ifdef NVOC_KERNEL_FALCON_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelFalcon {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct KernelFalcon *__nvoc_pbase_KernelFalcon;
    NvBool (*__kflcnIsRiscvActive__)(struct OBJGPU *, struct KernelFalcon *);
    void (*__kflcnRiscvProgramBcr__)(struct OBJGPU *, struct KernelFalcon *, NvBool);
    void (*__kflcnSwitchToFalcon__)(struct OBJGPU *, struct KernelFalcon *);
    NV_STATUS (*__kflcnResetHw__)(struct OBJGPU *, struct KernelFalcon *);
    NV_STATUS (*__kflcnPreResetWait__)(struct OBJGPU *, struct KernelFalcon *);
    NV_STATUS (*__kflcnWaitForResetToFinish__)(struct OBJGPU *, struct KernelFalcon *);
    NvBool (*__kflcnIsEngineInReset__)(struct OBJGPU *, struct KernelFalcon *);
    NvU32 (*__kflcnReadIntrStatus__)(struct OBJGPU *, struct KernelFalcon *);
    void (*__kflcnIntrRetrigger__)(struct OBJGPU *, struct KernelFalcon *);
    NvU32 (*__kflcnMaskImemAddr__)(struct OBJGPU *, struct KernelFalcon *, NvU32);
    NvU32 (*__kflcnMaskDmemAddr__)(struct OBJGPU *, struct KernelFalcon *, NvU32);
    NvU32 registerBase;
    NvU32 riscvRegisterBase;
    NvU32 fbifBase;
    NvBool bBootFromHs;
    NvU32 pmcEnableMask;
    NvU32 bIsPmcDeviceEngine;
    ENGDESCRIPTOR physEngDesc;
    NvU32 ctxAttr;
    NvU32 ctxBufferSize;
    NvU32 addrSpaceList;
};

#ifndef __NVOC_CLASS_KernelFalcon_TYPEDEF__
#define __NVOC_CLASS_KernelFalcon_TYPEDEF__
typedef struct KernelFalcon KernelFalcon;
#endif /* __NVOC_CLASS_KernelFalcon_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelFalcon
#define __nvoc_class_id_KernelFalcon 0xb6b1af
#endif /* __nvoc_class_id_KernelFalcon */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFalcon;

#define __staticCast_KernelFalcon(pThis) \
    ((pThis)->__nvoc_pbase_KernelFalcon)

#ifdef __nvoc_kernel_falcon_h_disabled
#define __dynamicCast_KernelFalcon(pThis) ((KernelFalcon*)NULL)
#else //__nvoc_kernel_falcon_h_disabled
#define __dynamicCast_KernelFalcon(pThis) \
    ((KernelFalcon*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelFalcon)))
#endif //__nvoc_kernel_falcon_h_disabled


NV_STATUS __nvoc_objCreateDynamic_KernelFalcon(KernelFalcon**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelFalcon(KernelFalcon**, Dynamic*, NvU32);
#define __objCreate_KernelFalcon(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelFalcon((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define kflcnIsRiscvActive(pGpu, pKernelFlcn) kflcnIsRiscvActive_DISPATCH(pGpu, pKernelFlcn)
#define kflcnIsRiscvActive_HAL(pGpu, pKernelFlcn) kflcnIsRiscvActive_DISPATCH(pGpu, pKernelFlcn)
#define kflcnRiscvProgramBcr(pGpu, pKernelFlcn, bBRFetch) kflcnRiscvProgramBcr_DISPATCH(pGpu, pKernelFlcn, bBRFetch)
#define kflcnRiscvProgramBcr_HAL(pGpu, pKernelFlcn, bBRFetch) kflcnRiscvProgramBcr_DISPATCH(pGpu, pKernelFlcn, bBRFetch)
#define kflcnSwitchToFalcon(pGpu, pKernelFlcn) kflcnSwitchToFalcon_DISPATCH(pGpu, pKernelFlcn)
#define kflcnSwitchToFalcon_HAL(pGpu, pKernelFlcn) kflcnSwitchToFalcon_DISPATCH(pGpu, pKernelFlcn)
#define kflcnResetHw(pGpu, pKernelFlcn) kflcnResetHw_DISPATCH(pGpu, pKernelFlcn)
#define kflcnPreResetWait(pGpu, pKernelFlcn) kflcnPreResetWait_DISPATCH(pGpu, pKernelFlcn)
#define kflcnPreResetWait_HAL(pGpu, pKernelFlcn) kflcnPreResetWait_DISPATCH(pGpu, pKernelFlcn)
#define kflcnWaitForResetToFinish(pGpu, pKernelFlcn) kflcnWaitForResetToFinish_DISPATCH(pGpu, pKernelFlcn)
#define kflcnWaitForResetToFinish_HAL(pGpu, pKernelFlcn) kflcnWaitForResetToFinish_DISPATCH(pGpu, pKernelFlcn)
#define kflcnIsEngineInReset(pGpu, pKernelFlcn) kflcnIsEngineInReset_DISPATCH(pGpu, pKernelFlcn)
#define kflcnReadIntrStatus(pGpu, pKerneFlcn) kflcnReadIntrStatus_DISPATCH(pGpu, pKerneFlcn)
#define kflcnReadIntrStatus_HAL(pGpu, pKerneFlcn) kflcnReadIntrStatus_DISPATCH(pGpu, pKerneFlcn)
#define kflcnIntrRetrigger(pGpu, pKernelFlcn) kflcnIntrRetrigger_DISPATCH(pGpu, pKernelFlcn)
#define kflcnIntrRetrigger_HAL(pGpu, pKernelFlcn) kflcnIntrRetrigger_DISPATCH(pGpu, pKernelFlcn)
#define kflcnMaskImemAddr(pGpu, pKernelFlcn, addr) kflcnMaskImemAddr_DISPATCH(pGpu, pKernelFlcn, addr)
#define kflcnMaskImemAddr_HAL(pGpu, pKernelFlcn, addr) kflcnMaskImemAddr_DISPATCH(pGpu, pKernelFlcn, addr)
#define kflcnMaskDmemAddr(pGpu, pKernelFlcn, addr) kflcnMaskDmemAddr_DISPATCH(pGpu, pKernelFlcn, addr)
#define kflcnMaskDmemAddr_HAL(pGpu, pKernelFlcn, addr) kflcnMaskDmemAddr_DISPATCH(pGpu, pKernelFlcn, addr)
NvU32 kflcnRegRead_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 offset);


#ifdef __nvoc_kernel_falcon_h_disabled
static inline NvU32 kflcnRegRead(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 offset) {
    NV_ASSERT_FAILED_PRECOMP("KernelFalcon was disabled!");
    return 0;
}
#else //__nvoc_kernel_falcon_h_disabled
#define kflcnRegRead(pGpu, pKernelFlcn, offset) kflcnRegRead_TU102(pGpu, pKernelFlcn, offset)
#endif //__nvoc_kernel_falcon_h_disabled

#define kflcnRegRead_HAL(pGpu, pKernelFlcn, offset) kflcnRegRead(pGpu, pKernelFlcn, offset)

void kflcnRegWrite_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 offset, NvU32 data);


#ifdef __nvoc_kernel_falcon_h_disabled
static inline void kflcnRegWrite(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 offset, NvU32 data) {
    NV_ASSERT_FAILED_PRECOMP("KernelFalcon was disabled!");
}
#else //__nvoc_kernel_falcon_h_disabled
#define kflcnRegWrite(pGpu, pKernelFlcn, offset, data) kflcnRegWrite_TU102(pGpu, pKernelFlcn, offset, data)
#endif //__nvoc_kernel_falcon_h_disabled

#define kflcnRegWrite_HAL(pGpu, pKernelFlcn, offset, data) kflcnRegWrite(pGpu, pKernelFlcn, offset, data)

NvU32 kflcnRiscvRegRead_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 offset);


#ifdef __nvoc_kernel_falcon_h_disabled
static inline NvU32 kflcnRiscvRegRead(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 offset) {
    NV_ASSERT_FAILED_PRECOMP("KernelFalcon was disabled!");
    return 0;
}
#else //__nvoc_kernel_falcon_h_disabled
#define kflcnRiscvRegRead(pGpu, pKernelFlcn, offset) kflcnRiscvRegRead_TU102(pGpu, pKernelFlcn, offset)
#endif //__nvoc_kernel_falcon_h_disabled

#define kflcnRiscvRegRead_HAL(pGpu, pKernelFlcn, offset) kflcnRiscvRegRead(pGpu, pKernelFlcn, offset)

void kflcnRiscvRegWrite_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 offset, NvU32 data);


#ifdef __nvoc_kernel_falcon_h_disabled
static inline void kflcnRiscvRegWrite(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 offset, NvU32 data) {
    NV_ASSERT_FAILED_PRECOMP("KernelFalcon was disabled!");
}
#else //__nvoc_kernel_falcon_h_disabled
#define kflcnRiscvRegWrite(pGpu, pKernelFlcn, offset, data) kflcnRiscvRegWrite_TU102(pGpu, pKernelFlcn, offset, data)
#endif //__nvoc_kernel_falcon_h_disabled

#define kflcnRiscvRegWrite_HAL(pGpu, pKernelFlcn, offset, data) kflcnRiscvRegWrite(pGpu, pKernelFlcn, offset, data)

NvBool kflcnIsRiscvCpuEnabled_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);


#ifdef __nvoc_kernel_falcon_h_disabled
static inline NvBool kflcnIsRiscvCpuEnabled(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    NV_ASSERT_FAILED_PRECOMP("KernelFalcon was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_falcon_h_disabled
#define kflcnIsRiscvCpuEnabled(pGpu, pKernelFlcn) kflcnIsRiscvCpuEnabled_TU102(pGpu, pKernelFlcn)
#endif //__nvoc_kernel_falcon_h_disabled

#define kflcnIsRiscvCpuEnabled_HAL(pGpu, pKernelFlcn) kflcnIsRiscvCpuEnabled(pGpu, pKernelFlcn)

void kflcnReset_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);


#ifdef __nvoc_kernel_falcon_h_disabled
static inline void kflcnReset(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    NV_ASSERT_FAILED_PRECOMP("KernelFalcon was disabled!");
}
#else //__nvoc_kernel_falcon_h_disabled
#define kflcnReset(pGpu, pKernelFlcn) kflcnReset_TU102(pGpu, pKernelFlcn)
#endif //__nvoc_kernel_falcon_h_disabled

#define kflcnReset_HAL(pGpu, pKernelFlcn) kflcnReset(pGpu, pKernelFlcn)

void kflcnSecureReset_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);


#ifdef __nvoc_kernel_falcon_h_disabled
static inline void kflcnSecureReset(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    NV_ASSERT_FAILED_PRECOMP("KernelFalcon was disabled!");
}
#else //__nvoc_kernel_falcon_h_disabled
#define kflcnSecureReset(pGpu, pKernelFlcn) kflcnSecureReset_TU102(pGpu, pKernelFlcn)
#endif //__nvoc_kernel_falcon_h_disabled

#define kflcnSecureReset_HAL(pGpu, pKernelFlcn) kflcnSecureReset(pGpu, pKernelFlcn)

void kflcnEnable_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvBool bEnable);


#ifdef __nvoc_kernel_falcon_h_disabled
static inline void kflcnEnable(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvBool bEnable) {
    NV_ASSERT_FAILED_PRECOMP("KernelFalcon was disabled!");
}
#else //__nvoc_kernel_falcon_h_disabled
#define kflcnEnable(pGpu, pKernelFlcn, bEnable) kflcnEnable_TU102(pGpu, pKernelFlcn, bEnable)
#endif //__nvoc_kernel_falcon_h_disabled

#define kflcnEnable_HAL(pGpu, pKernelFlcn, bEnable) kflcnEnable(pGpu, pKernelFlcn, bEnable)

void kflcnStartCpu_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);


#ifdef __nvoc_kernel_falcon_h_disabled
static inline void kflcnStartCpu(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    NV_ASSERT_FAILED_PRECOMP("KernelFalcon was disabled!");
}
#else //__nvoc_kernel_falcon_h_disabled
#define kflcnStartCpu(pGpu, pKernelFlcn) kflcnStartCpu_TU102(pGpu, pKernelFlcn)
#endif //__nvoc_kernel_falcon_h_disabled

#define kflcnStartCpu_HAL(pGpu, pKernelFlcn) kflcnStartCpu(pGpu, pKernelFlcn)

void kflcnDisableCtxReq_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);


#ifdef __nvoc_kernel_falcon_h_disabled
static inline void kflcnDisableCtxReq(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    NV_ASSERT_FAILED_PRECOMP("KernelFalcon was disabled!");
}
#else //__nvoc_kernel_falcon_h_disabled
#define kflcnDisableCtxReq(pGpu, pKernelFlcn) kflcnDisableCtxReq_TU102(pGpu, pKernelFlcn)
#endif //__nvoc_kernel_falcon_h_disabled

#define kflcnDisableCtxReq_HAL(pGpu, pKernelFlcn) kflcnDisableCtxReq(pGpu, pKernelFlcn)

NV_STATUS kflcnWaitForHalt_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 timeoutUs, NvU32 flags);


#ifdef __nvoc_kernel_falcon_h_disabled
static inline NV_STATUS kflcnWaitForHalt(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 timeoutUs, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("KernelFalcon was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_falcon_h_disabled
#define kflcnWaitForHalt(pGpu, pKernelFlcn, timeoutUs, flags) kflcnWaitForHalt_TU102(pGpu, pKernelFlcn, timeoutUs, flags)
#endif //__nvoc_kernel_falcon_h_disabled

#define kflcnWaitForHalt_HAL(pGpu, pKernelFlcn, timeoutUs, flags) kflcnWaitForHalt(pGpu, pKernelFlcn, timeoutUs, flags)

NvBool kflcnIsRiscvActive_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

NvBool kflcnIsRiscvActive_GA10X(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

static inline NvBool kflcnIsRiscvActive_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    return pKernelFlcn->__kflcnIsRiscvActive__(pGpu, pKernelFlcn);
}

void kflcnRiscvProgramBcr_GA102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvBool bBRFetch);

static inline void kflcnRiscvProgramBcr_f2d351(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvBool bBRFetch) {
    NV_ASSERT_PRECOMP(0);
}

static inline void kflcnRiscvProgramBcr_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvBool bBRFetch) {
    pKernelFlcn->__kflcnRiscvProgramBcr__(pGpu, pKernelFlcn, bBRFetch);
}

void kflcnSwitchToFalcon_GA10X(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

static inline void kflcnSwitchToFalcon_b3696a(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    return;
}

static inline void kflcnSwitchToFalcon_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    pKernelFlcn->__kflcnSwitchToFalcon__(pGpu, pKernelFlcn);
}

static inline NV_STATUS kflcnResetHw_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    return pKernelFlcn->__kflcnResetHw__(pGpu, pKernelFlcn);
}

NV_STATUS kflcnPreResetWait_GA10X(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

static inline NV_STATUS kflcnPreResetWait_56cd7a(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    return NV_OK;
}

static inline NV_STATUS kflcnPreResetWait_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    return pKernelFlcn->__kflcnPreResetWait__(pGpu, pKernelFlcn);
}

NV_STATUS kflcnWaitForResetToFinish_GA102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

NV_STATUS kflcnWaitForResetToFinish_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

static inline NV_STATUS kflcnWaitForResetToFinish_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    return pKernelFlcn->__kflcnWaitForResetToFinish__(pGpu, pKernelFlcn);
}

static inline NvBool kflcnIsEngineInReset_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    return pKernelFlcn->__kflcnIsEngineInReset__(pGpu, pKernelFlcn);
}

NvU32 kflcnReadIntrStatus_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKerneFlcn);

NvU32 kflcnReadIntrStatus_GA102(struct OBJGPU *pGpu, struct KernelFalcon *pKerneFlcn);

static inline NvU32 kflcnReadIntrStatus_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKerneFlcn) {
    return pKerneFlcn->__kflcnReadIntrStatus__(pGpu, pKerneFlcn);
}

void kflcnIntrRetrigger_GA100(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

static inline void kflcnIntrRetrigger_b3696a(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    return;
}

static inline void kflcnIntrRetrigger_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    pKernelFlcn->__kflcnIntrRetrigger__(pGpu, pKernelFlcn);
}

NvU32 kflcnMaskImemAddr_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 addr);

NvU32 kflcnMaskImemAddr_GA100(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 addr);

static inline NvU32 kflcnMaskImemAddr_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 addr) {
    return pKernelFlcn->__kflcnMaskImemAddr__(pGpu, pKernelFlcn, addr);
}

NvU32 kflcnMaskDmemAddr_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 addr);

NvU32 kflcnMaskDmemAddr_GA100(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 addr);

static inline NvU32 kflcnMaskDmemAddr_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 addr) {
    return pKernelFlcn->__kflcnMaskDmemAddr__(pGpu, pKernelFlcn, addr);
}

void kflcnConfigureEngine_IMPL(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFalcon, KernelFalconEngineConfig *pFalconConfig);

#ifdef __nvoc_kernel_falcon_h_disabled
static inline void kflcnConfigureEngine(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFalcon, KernelFalconEngineConfig *pFalconConfig) {
    NV_ASSERT_FAILED_PRECOMP("KernelFalcon was disabled!");
}
#else //__nvoc_kernel_falcon_h_disabled
#define kflcnConfigureEngine(pGpu, pKernelFalcon, pFalconConfig) kflcnConfigureEngine_IMPL(pGpu, pKernelFalcon, pFalconConfig)
#endif //__nvoc_kernel_falcon_h_disabled

NV_STATUS kflcnAllocContext_IMPL(struct OBJGPU *arg0, struct KernelFalcon *arg1, struct KernelChannel *arg2, NvU32 arg3);

#ifdef __nvoc_kernel_falcon_h_disabled
static inline NV_STATUS kflcnAllocContext(struct OBJGPU *arg0, struct KernelFalcon *arg1, struct KernelChannel *arg2, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelFalcon was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_falcon_h_disabled
#define kflcnAllocContext(arg0, arg1, arg2, arg3) kflcnAllocContext_IMPL(arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_falcon_h_disabled

NV_STATUS kflcnFreeContext_IMPL(struct OBJGPU *arg0, struct KernelFalcon *arg1, struct KernelChannel *arg2, NvU32 arg3);

#ifdef __nvoc_kernel_falcon_h_disabled
static inline NV_STATUS kflcnFreeContext(struct OBJGPU *arg0, struct KernelFalcon *arg1, struct KernelChannel *arg2, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelFalcon was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_falcon_h_disabled
#define kflcnFreeContext(arg0, arg1, arg2, arg3) kflcnFreeContext_IMPL(arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_falcon_h_disabled

struct KernelFalcon *kflcnGetKernelFalconForEngine_IMPL(struct OBJGPU *pGpu, ENGDESCRIPTOR physEngDesc);

#define kflcnGetKernelFalconForEngine(pGpu, physEngDesc) kflcnGetKernelFalconForEngine_IMPL(pGpu, physEngDesc)
#undef PRIVATE_FIELD


// Basic implementation of KernelFalcon that can be instantiated.
#ifdef NVOC_KERNEL_FALCON_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct GenericKernelFalcon {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct KernelFalcon __nvoc_base_KernelFalcon;
    struct IntrService __nvoc_base_IntrService;
    struct Object __nvoc_base_Object;
    struct KernelFalcon *__nvoc_pbase_KernelFalcon;
    struct IntrService *__nvoc_pbase_IntrService;
    struct Object *__nvoc_pbase_Object;
    struct GenericKernelFalcon *__nvoc_pbase_GenericKernelFalcon;
    NV_STATUS (*__gkflcnResetHw__)(struct OBJGPU *, struct GenericKernelFalcon *);
    NvBool (*__gkflcnIsEngineInReset__)(struct OBJGPU *, struct GenericKernelFalcon *);
    void (*__gkflcnRegisterIntrService__)(struct OBJGPU *, struct GenericKernelFalcon *, IntrServiceRecord *);
    NV_STATUS (*__gkflcnServiceNotificationInterrupt__)(struct OBJGPU *, struct GenericKernelFalcon *, IntrServiceServiceNotificationInterruptArguments *);
    NvBool (*__gkflcnClearInterrupt__)(struct OBJGPU *, struct GenericKernelFalcon *, IntrServiceClearInterruptArguments *);
    NvU32 (*__gkflcnServiceInterrupt__)(struct OBJGPU *, struct GenericKernelFalcon *, IntrServiceServiceInterruptArguments *);
};

#ifndef __NVOC_CLASS_GenericKernelFalcon_TYPEDEF__
#define __NVOC_CLASS_GenericKernelFalcon_TYPEDEF__
typedef struct GenericKernelFalcon GenericKernelFalcon;
#endif /* __NVOC_CLASS_GenericKernelFalcon_TYPEDEF__ */

#ifndef __nvoc_class_id_GenericKernelFalcon
#define __nvoc_class_id_GenericKernelFalcon 0xabcf08
#endif /* __nvoc_class_id_GenericKernelFalcon */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GenericKernelFalcon;

#define __staticCast_GenericKernelFalcon(pThis) \
    ((pThis)->__nvoc_pbase_GenericKernelFalcon)

#ifdef __nvoc_kernel_falcon_h_disabled
#define __dynamicCast_GenericKernelFalcon(pThis) ((GenericKernelFalcon*)NULL)
#else //__nvoc_kernel_falcon_h_disabled
#define __dynamicCast_GenericKernelFalcon(pThis) \
    ((GenericKernelFalcon*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GenericKernelFalcon)))
#endif //__nvoc_kernel_falcon_h_disabled


NV_STATUS __nvoc_objCreateDynamic_GenericKernelFalcon(GenericKernelFalcon**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GenericKernelFalcon(GenericKernelFalcon**, Dynamic*, NvU32, struct OBJGPU * arg_pGpu, KernelFalconEngineConfig * arg_pFalconConfig);
#define __objCreate_GenericKernelFalcon(ppNewObj, pParent, createFlags, arg_pGpu, arg_pFalconConfig) \
    __nvoc_objCreate_GenericKernelFalcon((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pGpu, arg_pFalconConfig)

#define gkflcnResetHw(pGpu, pGenKernFlcn) gkflcnResetHw_DISPATCH(pGpu, pGenKernFlcn)
#define gkflcnIsEngineInReset(pGpu, pGenKernFlcn) gkflcnIsEngineInReset_DISPATCH(pGpu, pGenKernFlcn)
#define gkflcnRegisterIntrService(arg0, arg1, arg2) gkflcnRegisterIntrService_DISPATCH(arg0, arg1, arg2)
#define gkflcnServiceNotificationInterrupt(arg0, arg1, arg2) gkflcnServiceNotificationInterrupt_DISPATCH(arg0, arg1, arg2)
#define gkflcnClearInterrupt(pGpu, pIntrService, pParams) gkflcnClearInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define gkflcnServiceInterrupt(pGpu, pIntrService, pParams) gkflcnServiceInterrupt_DISPATCH(pGpu, pIntrService, pParams)
NV_STATUS gkflcnResetHw_IMPL(struct OBJGPU *pGpu, struct GenericKernelFalcon *pGenKernFlcn);

static inline NV_STATUS gkflcnResetHw_DISPATCH(struct OBJGPU *pGpu, struct GenericKernelFalcon *pGenKernFlcn) {
    return pGenKernFlcn->__gkflcnResetHw__(pGpu, pGenKernFlcn);
}

NvBool gkflcnIsEngineInReset_IMPL(struct OBJGPU *pGpu, struct GenericKernelFalcon *pGenKernFlcn);

static inline NvBool gkflcnIsEngineInReset_DISPATCH(struct OBJGPU *pGpu, struct GenericKernelFalcon *pGenKernFlcn) {
    return pGenKernFlcn->__gkflcnIsEngineInReset__(pGpu, pGenKernFlcn);
}

void gkflcnRegisterIntrService_IMPL(struct OBJGPU *arg0, struct GenericKernelFalcon *arg1, IntrServiceRecord arg2[163]);

static inline void gkflcnRegisterIntrService_DISPATCH(struct OBJGPU *arg0, struct GenericKernelFalcon *arg1, IntrServiceRecord arg2[163]) {
    arg1->__gkflcnRegisterIntrService__(arg0, arg1, arg2);
}

NV_STATUS gkflcnServiceNotificationInterrupt_IMPL(struct OBJGPU *arg0, struct GenericKernelFalcon *arg1, IntrServiceServiceNotificationInterruptArguments *arg2);

static inline NV_STATUS gkflcnServiceNotificationInterrupt_DISPATCH(struct OBJGPU *arg0, struct GenericKernelFalcon *arg1, IntrServiceServiceNotificationInterruptArguments *arg2) {
    return arg1->__gkflcnServiceNotificationInterrupt__(arg0, arg1, arg2);
}

static inline NvBool gkflcnClearInterrupt_DISPATCH(struct OBJGPU *pGpu, struct GenericKernelFalcon *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return pIntrService->__gkflcnClearInterrupt__(pGpu, pIntrService, pParams);
}

static inline NvU32 gkflcnServiceInterrupt_DISPATCH(struct OBJGPU *pGpu, struct GenericKernelFalcon *pIntrService, IntrServiceServiceInterruptArguments *pParams) {
    return pIntrService->__gkflcnServiceInterrupt__(pGpu, pIntrService, pParams);
}

NV_STATUS gkflcnConstruct_IMPL(struct GenericKernelFalcon *arg_pGenKernFlcn, struct OBJGPU *arg_pGpu, KernelFalconEngineConfig *arg_pFalconConfig);

#define __nvoc_gkflcnConstruct(arg_pGenKernFlcn, arg_pGpu, arg_pFalconConfig) gkflcnConstruct_IMPL(arg_pGenKernFlcn, arg_pGpu, arg_pFalconConfig)
#undef PRIVATE_FIELD


#endif // KERNEL_FALCON_H

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_KERNEL_FALCON_NVOC_H_
