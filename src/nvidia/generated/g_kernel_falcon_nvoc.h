
#ifndef _G_KERNEL_FALCON_NVOC_H_
#define _G_KERNEL_FALCON_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once
#include "g_kernel_falcon_nvoc.h"

#ifndef KERNEL_FALCON_H
#define KERNEL_FALCON_H

#include "core/core.h"
#include "gpu/falcon/falcon_common.h"
#include "gpu/falcon/kernel_crashcat_engine.h"
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

    KernelCrashCatEngineConfig crashcatEngConfig;
} KernelFalconEngineConfig;

/*!
 * Base class for booting Falcon cores (including RISC-V)
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_FALCON_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelFalcon;
struct NVOC_METADATA__KernelCrashCatEngine;
struct NVOC_VTABLE__KernelFalcon;


struct KernelFalcon {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelFalcon *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct KernelCrashCatEngine __nvoc_base_KernelCrashCatEngine;

    // Ancestor object pointers for `staticCast` feature
    struct CrashCatEngine *__nvoc_pbase_CrashCatEngine;    // crashcatEngine super^2
    struct KernelCrashCatEngine *__nvoc_pbase_KernelCrashCatEngine;    // kcrashcatEngine super
    struct KernelFalcon *__nvoc_pbase_KernelFalcon;    // kflcn

    // Vtable with 23 per-object function pointers
    NvU32 (*__kflcnRegRead__)(struct OBJGPU *, struct KernelFalcon * /*this*/, NvU32);  // virtual halified (2 hals) override (kcrashcatEngine) base (kcrashcatEngine) body
    void (*__kflcnRegWrite__)(struct OBJGPU *, struct KernelFalcon * /*this*/, NvU32, NvU32);  // virtual halified (2 hals) override (kcrashcatEngine) base (kcrashcatEngine) body
    NvU32 (*__kflcnRiscvRegRead__)(struct OBJGPU *, struct KernelFalcon * /*this*/, NvU32);  // halified (2 hals) body
    void (*__kflcnRiscvRegWrite__)(struct OBJGPU *, struct KernelFalcon * /*this*/, NvU32, NvU32);  // halified (2 hals) body
    NvBool (*__kflcnIsRiscvCpuEnabled__)(struct OBJGPU *, struct KernelFalcon * /*this*/);  // halified (2 hals) body
    NvBool (*__kflcnIsRiscvActive__)(struct OBJGPU *, struct KernelFalcon * /*this*/);  // halified (3 hals) body
    NvBool (*__kflcnIsRiscvSelected__)(struct OBJGPU *, struct KernelFalcon * /*this*/);  // halified (4 hals) body
    void (*__kflcnRiscvProgramBcr__)(struct OBJGPU *, struct KernelFalcon * /*this*/, NvBool);  // halified (3 hals) body
    void (*__kflcnSwitchToFalcon__)(struct OBJGPU *, struct KernelFalcon * /*this*/);  // halified (4 hals) body
    NV_STATUS (*__kflcnReset__)(struct OBJGPU *, struct KernelFalcon * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kflcnResetIntoRiscv__)(struct OBJGPU *, struct KernelFalcon * /*this*/);  // halified (3 hals) body
    void (*__kflcnStartCpu__)(struct OBJGPU *, struct KernelFalcon * /*this*/);  // halified (2 hals) body
    void (*__kflcnDisableCtxReq__)(struct OBJGPU *, struct KernelFalcon * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kflcnPreResetWait__)(struct OBJGPU *, struct KernelFalcon * /*this*/);  // halified (3 hals) body
    NV_STATUS (*__kflcnWaitForResetToFinish__)(struct OBJGPU *, struct KernelFalcon * /*this*/);  // halified (3 hals) body
    NV_STATUS (*__kflcnWaitForHalt__)(struct OBJGPU *, struct KernelFalcon * /*this*/, NvU32, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kflcnWaitForHaltRiscv__)(struct OBJGPU *, struct KernelFalcon * /*this*/, NvU32, NvU32);  // halified (3 hals) body
    NvU32 (*__kflcnReadIntrStatus__)(struct OBJGPU *, struct KernelFalcon * /*this*/);  // halified (2 hals) body
    NvU32 (*__kflcnRiscvReadIntrStatus__)(struct OBJGPU *, struct KernelFalcon * /*this*/);  // halified (3 hals) body
    void (*__kflcnIntrRetrigger__)(struct OBJGPU *, struct KernelFalcon * /*this*/);  // halified (3 hals) body
    NvU32 (*__kflcnMaskImemAddr__)(struct OBJGPU *, struct KernelFalcon * /*this*/, NvU32);  // halified (3 hals) body
    NvU32 (*__kflcnMaskDmemAddr__)(struct OBJGPU *, struct KernelFalcon * /*this*/, NvU32);  // virtual halified (3 hals) override (kcrashcatEngine) base (kcrashcatEngine) body
    NvU32 (*__kflcnGetEccInterruptMask__)(struct OBJGPU *, struct KernelFalcon * /*this*/);  // halified (2 hals) body

    // Data members
    NvTristate riscvMode;
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


// Vtable with 13 per-class function pointers
struct NVOC_VTABLE__KernelFalcon {
    NV_STATUS (*__kflcnResetHw__)(struct OBJGPU *, struct KernelFalcon * /*this*/);  // pure virtual
    NvBool (*__kflcnConfigured__)(struct KernelFalcon * /*this*/);  // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
    void (*__kflcnUnload__)(struct KernelFalcon * /*this*/);  // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
    void (*__kflcnVprintf__)(struct KernelFalcon * /*this*/, NvBool, const char *, va_list);  // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
    NvU32 (*__kflcnPriRead__)(struct KernelFalcon * /*this*/, NvU32);  // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
    void (*__kflcnPriWrite__)(struct KernelFalcon * /*this*/, NvU32, NvU32);  // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
    void * (*__kflcnMapBufferDescriptor__)(struct KernelFalcon * /*this*/, CrashCatBufferDescriptor *);  // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
    void (*__kflcnUnmapBufferDescriptor__)(struct KernelFalcon * /*this*/, CrashCatBufferDescriptor *);  // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
    void (*__kflcnSyncBufferDescriptor__)(struct KernelFalcon * /*this*/, CrashCatBufferDescriptor *, NvU32, NvU32);  // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
    void (*__kflcnReadDmem__)(struct KernelFalcon * /*this*/, NvU32, NvU32, void *);  // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kcrashcatEngine)
    void (*__kflcnReadEmem__)(struct KernelFalcon * /*this*/, NvU64, NvU64, void *);  // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kcrashcatEngine)
    const NvU32 * (*__kflcnGetScratchOffsets__)(struct KernelFalcon * /*this*/, NV_CRASHCAT_SCRATCH_GROUP_ID);  // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kcrashcatEngine)
    NvU32 (*__kflcnGetWFL0Offset__)(struct KernelFalcon * /*this*/);  // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kcrashcatEngine)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelFalcon {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__KernelCrashCatEngine metadata__KernelCrashCatEngine;
    const struct NVOC_VTABLE__KernelFalcon vtable;
};

#ifndef __NVOC_CLASS_KernelFalcon_TYPEDEF__
#define __NVOC_CLASS_KernelFalcon_TYPEDEF__
typedef struct KernelFalcon KernelFalcon;
#endif /* __NVOC_CLASS_KernelFalcon_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelFalcon
#define __nvoc_class_id_KernelFalcon 0xb6b1af
#endif /* __nvoc_class_id_KernelFalcon */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFalcon;

#define __staticCast_KernelFalcon(pThis) \
    ((pThis)->__nvoc_pbase_KernelFalcon)

#ifdef __nvoc_kernel_falcon_h_disabled
#define __dynamicCast_KernelFalcon(pThis) ((KernelFalcon*) NULL)
#else //__nvoc_kernel_falcon_h_disabled
#define __dynamicCast_KernelFalcon(pThis) \
    ((KernelFalcon*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelFalcon)))
#endif //__nvoc_kernel_falcon_h_disabled

NV_STATUS __nvoc_objCreateDynamic_KernelFalcon(KernelFalcon**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelFalcon(KernelFalcon**, Dynamic*, NvU32);
#define __objCreate_KernelFalcon(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelFalcon((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define kflcnRegRead_FNPTR(pKernelFlcn) pKernelFlcn->__kflcnRegRead__
#define kflcnRegRead(pGpu, pKernelFlcn, offset) kflcnRegRead_DISPATCH(pGpu, pKernelFlcn, offset)
#define kflcnRegRead_HAL(pGpu, pKernelFlcn, offset) kflcnRegRead_DISPATCH(pGpu, pKernelFlcn, offset)
#define kflcnRegWrite_FNPTR(pKernelFlcn) pKernelFlcn->__kflcnRegWrite__
#define kflcnRegWrite(pGpu, pKernelFlcn, offset, data) kflcnRegWrite_DISPATCH(pGpu, pKernelFlcn, offset, data)
#define kflcnRegWrite_HAL(pGpu, pKernelFlcn, offset, data) kflcnRegWrite_DISPATCH(pGpu, pKernelFlcn, offset, data)
#define kflcnRiscvRegRead_FNPTR(pKernelFlcn) pKernelFlcn->__kflcnRiscvRegRead__
#define kflcnRiscvRegRead(pGpu, pKernelFlcn, offset) kflcnRiscvRegRead_DISPATCH(pGpu, pKernelFlcn, offset)
#define kflcnRiscvRegRead_HAL(pGpu, pKernelFlcn, offset) kflcnRiscvRegRead_DISPATCH(pGpu, pKernelFlcn, offset)
#define kflcnRiscvRegWrite_FNPTR(pKernelFlcn) pKernelFlcn->__kflcnRiscvRegWrite__
#define kflcnRiscvRegWrite(pGpu, pKernelFlcn, offset, data) kflcnRiscvRegWrite_DISPATCH(pGpu, pKernelFlcn, offset, data)
#define kflcnRiscvRegWrite_HAL(pGpu, pKernelFlcn, offset, data) kflcnRiscvRegWrite_DISPATCH(pGpu, pKernelFlcn, offset, data)
#define kflcnIsRiscvCpuEnabled_FNPTR(pKernelFlcn) pKernelFlcn->__kflcnIsRiscvCpuEnabled__
#define kflcnIsRiscvCpuEnabled(pGpu, pKernelFlcn) kflcnIsRiscvCpuEnabled_DISPATCH(pGpu, pKernelFlcn)
#define kflcnIsRiscvCpuEnabled_HAL(pGpu, pKernelFlcn) kflcnIsRiscvCpuEnabled_DISPATCH(pGpu, pKernelFlcn)
#define kflcnIsRiscvActive_FNPTR(pKernelFlcn) pKernelFlcn->__kflcnIsRiscvActive__
#define kflcnIsRiscvActive(pGpu, pKernelFlcn) kflcnIsRiscvActive_DISPATCH(pGpu, pKernelFlcn)
#define kflcnIsRiscvActive_HAL(pGpu, pKernelFlcn) kflcnIsRiscvActive_DISPATCH(pGpu, pKernelFlcn)
#define kflcnIsRiscvSelected_FNPTR(pKernelFlcn) pKernelFlcn->__kflcnIsRiscvSelected__
#define kflcnIsRiscvSelected(pGpu, pKernelFlcn) kflcnIsRiscvSelected_DISPATCH(pGpu, pKernelFlcn)
#define kflcnIsRiscvSelected_HAL(pGpu, pKernelFlcn) kflcnIsRiscvSelected_DISPATCH(pGpu, pKernelFlcn)
#define kflcnRiscvProgramBcr_FNPTR(pKernelFlcn) pKernelFlcn->__kflcnRiscvProgramBcr__
#define kflcnRiscvProgramBcr(pGpu, pKernelFlcn, bBRFetch) kflcnRiscvProgramBcr_DISPATCH(pGpu, pKernelFlcn, bBRFetch)
#define kflcnRiscvProgramBcr_HAL(pGpu, pKernelFlcn, bBRFetch) kflcnRiscvProgramBcr_DISPATCH(pGpu, pKernelFlcn, bBRFetch)
#define kflcnSwitchToFalcon_FNPTR(pKernelFlcn) pKernelFlcn->__kflcnSwitchToFalcon__
#define kflcnSwitchToFalcon(pGpu, pKernelFlcn) kflcnSwitchToFalcon_DISPATCH(pGpu, pKernelFlcn)
#define kflcnSwitchToFalcon_HAL(pGpu, pKernelFlcn) kflcnSwitchToFalcon_DISPATCH(pGpu, pKernelFlcn)
#define kflcnResetHw_FNPTR(pKernelFlcn) pKernelFlcn->__nvoc_metadata_ptr->vtable.__kflcnResetHw__
#define kflcnResetHw(pGpu, pKernelFlcn) kflcnResetHw_DISPATCH(pGpu, pKernelFlcn)
#define kflcnReset_FNPTR(pKernelFlcn) pKernelFlcn->__kflcnReset__
#define kflcnReset(pGpu, pKernelFlcn) kflcnReset_DISPATCH(pGpu, pKernelFlcn)
#define kflcnReset_HAL(pGpu, pKernelFlcn) kflcnReset_DISPATCH(pGpu, pKernelFlcn)
#define kflcnResetIntoRiscv_FNPTR(pKernelFlcn) pKernelFlcn->__kflcnResetIntoRiscv__
#define kflcnResetIntoRiscv(pGpu, pKernelFlcn) kflcnResetIntoRiscv_DISPATCH(pGpu, pKernelFlcn)
#define kflcnResetIntoRiscv_HAL(pGpu, pKernelFlcn) kflcnResetIntoRiscv_DISPATCH(pGpu, pKernelFlcn)
#define kflcnStartCpu_FNPTR(pKernelFlcn) pKernelFlcn->__kflcnStartCpu__
#define kflcnStartCpu(pGpu, pKernelFlcn) kflcnStartCpu_DISPATCH(pGpu, pKernelFlcn)
#define kflcnStartCpu_HAL(pGpu, pKernelFlcn) kflcnStartCpu_DISPATCH(pGpu, pKernelFlcn)
#define kflcnDisableCtxReq_FNPTR(pKernelFlcn) pKernelFlcn->__kflcnDisableCtxReq__
#define kflcnDisableCtxReq(pGpu, pKernelFlcn) kflcnDisableCtxReq_DISPATCH(pGpu, pKernelFlcn)
#define kflcnDisableCtxReq_HAL(pGpu, pKernelFlcn) kflcnDisableCtxReq_DISPATCH(pGpu, pKernelFlcn)
#define kflcnPreResetWait_FNPTR(pKernelFlcn) pKernelFlcn->__kflcnPreResetWait__
#define kflcnPreResetWait(pGpu, pKernelFlcn) kflcnPreResetWait_DISPATCH(pGpu, pKernelFlcn)
#define kflcnPreResetWait_HAL(pGpu, pKernelFlcn) kflcnPreResetWait_DISPATCH(pGpu, pKernelFlcn)
#define kflcnWaitForResetToFinish_FNPTR(pKernelFlcn) pKernelFlcn->__kflcnWaitForResetToFinish__
#define kflcnWaitForResetToFinish(pGpu, pKernelFlcn) kflcnWaitForResetToFinish_DISPATCH(pGpu, pKernelFlcn)
#define kflcnWaitForResetToFinish_HAL(pGpu, pKernelFlcn) kflcnWaitForResetToFinish_DISPATCH(pGpu, pKernelFlcn)
#define kflcnWaitForHalt_FNPTR(pKernelFlcn) pKernelFlcn->__kflcnWaitForHalt__
#define kflcnWaitForHalt(pGpu, pKernelFlcn, timeoutUs, flags) kflcnWaitForHalt_DISPATCH(pGpu, pKernelFlcn, timeoutUs, flags)
#define kflcnWaitForHalt_HAL(pGpu, pKernelFlcn, timeoutUs, flags) kflcnWaitForHalt_DISPATCH(pGpu, pKernelFlcn, timeoutUs, flags)
#define kflcnWaitForHaltRiscv_FNPTR(pKernelFlcn) pKernelFlcn->__kflcnWaitForHaltRiscv__
#define kflcnWaitForHaltRiscv(pGpu, pKernelFlcn, timeoutUs, flags) kflcnWaitForHaltRiscv_DISPATCH(pGpu, pKernelFlcn, timeoutUs, flags)
#define kflcnWaitForHaltRiscv_HAL(pGpu, pKernelFlcn, timeoutUs, flags) kflcnWaitForHaltRiscv_DISPATCH(pGpu, pKernelFlcn, timeoutUs, flags)
#define kflcnReadIntrStatus_FNPTR(pKerneFlcn) pKerneFlcn->__kflcnReadIntrStatus__
#define kflcnReadIntrStatus(pGpu, pKerneFlcn) kflcnReadIntrStatus_DISPATCH(pGpu, pKerneFlcn)
#define kflcnReadIntrStatus_HAL(pGpu, pKerneFlcn) kflcnReadIntrStatus_DISPATCH(pGpu, pKerneFlcn)
#define kflcnRiscvReadIntrStatus_FNPTR(pKernelFlcn) pKernelFlcn->__kflcnRiscvReadIntrStatus__
#define kflcnRiscvReadIntrStatus(pGpu, pKernelFlcn) kflcnRiscvReadIntrStatus_DISPATCH(pGpu, pKernelFlcn)
#define kflcnRiscvReadIntrStatus_HAL(pGpu, pKernelFlcn) kflcnRiscvReadIntrStatus_DISPATCH(pGpu, pKernelFlcn)
#define kflcnIntrRetrigger_FNPTR(pKernelFlcn) pKernelFlcn->__kflcnIntrRetrigger__
#define kflcnIntrRetrigger(pGpu, pKernelFlcn) kflcnIntrRetrigger_DISPATCH(pGpu, pKernelFlcn)
#define kflcnIntrRetrigger_HAL(pGpu, pKernelFlcn) kflcnIntrRetrigger_DISPATCH(pGpu, pKernelFlcn)
#define kflcnMaskImemAddr_FNPTR(pKernelFlcn) pKernelFlcn->__kflcnMaskImemAddr__
#define kflcnMaskImemAddr(pGpu, pKernelFlcn, addr) kflcnMaskImemAddr_DISPATCH(pGpu, pKernelFlcn, addr)
#define kflcnMaskImemAddr_HAL(pGpu, pKernelFlcn, addr) kflcnMaskImemAddr_DISPATCH(pGpu, pKernelFlcn, addr)
#define kflcnMaskDmemAddr_FNPTR(pKernelFlcn) pKernelFlcn->__kflcnMaskDmemAddr__
#define kflcnMaskDmemAddr(pGpu, pKernelFlcn, addr) kflcnMaskDmemAddr_DISPATCH(pGpu, pKernelFlcn, addr)
#define kflcnMaskDmemAddr_HAL(pGpu, pKernelFlcn, addr) kflcnMaskDmemAddr_DISPATCH(pGpu, pKernelFlcn, addr)
#define kflcnGetEccInterruptMask_FNPTR(pKernelFlcn) pKernelFlcn->__kflcnGetEccInterruptMask__
#define kflcnGetEccInterruptMask(pGpu, pKernelFlcn) kflcnGetEccInterruptMask_DISPATCH(pGpu, pKernelFlcn)
#define kflcnGetEccInterruptMask_HAL(pGpu, pKernelFlcn) kflcnGetEccInterruptMask_DISPATCH(pGpu, pKernelFlcn)
#define kflcnConfigured_FNPTR(arg_this) arg_this->__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr->vtable.__kcrashcatEngineConfigured__
#define kflcnConfigured(arg_this) kflcnConfigured_DISPATCH(arg_this)
#define kflcnUnload_FNPTR(arg_this) arg_this->__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr->vtable.__kcrashcatEngineUnload__
#define kflcnUnload(arg_this) kflcnUnload_DISPATCH(arg_this)
#define kflcnVprintf_FNPTR(arg_this) arg_this->__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr->vtable.__kcrashcatEngineVprintf__
#define kflcnVprintf(arg_this, bReportStart, fmt, args) kflcnVprintf_DISPATCH(arg_this, bReportStart, fmt, args)
#define kflcnPriRead_FNPTR(arg_this) arg_this->__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr->vtable.__kcrashcatEnginePriRead__
#define kflcnPriRead(arg_this, offset) kflcnPriRead_DISPATCH(arg_this, offset)
#define kflcnPriWrite_FNPTR(arg_this) arg_this->__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr->vtable.__kcrashcatEnginePriWrite__
#define kflcnPriWrite(arg_this, offset, data) kflcnPriWrite_DISPATCH(arg_this, offset, data)
#define kflcnMapBufferDescriptor_FNPTR(arg_this) arg_this->__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr->vtable.__kcrashcatEngineMapBufferDescriptor__
#define kflcnMapBufferDescriptor(arg_this, pBufDesc) kflcnMapBufferDescriptor_DISPATCH(arg_this, pBufDesc)
#define kflcnUnmapBufferDescriptor_FNPTR(arg_this) arg_this->__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr->vtable.__kcrashcatEngineUnmapBufferDescriptor__
#define kflcnUnmapBufferDescriptor(arg_this, pBufDesc) kflcnUnmapBufferDescriptor_DISPATCH(arg_this, pBufDesc)
#define kflcnSyncBufferDescriptor_FNPTR(arg_this) arg_this->__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr->vtable.__kcrashcatEngineSyncBufferDescriptor__
#define kflcnSyncBufferDescriptor(arg_this, pBufDesc, offset, size) kflcnSyncBufferDescriptor_DISPATCH(arg_this, pBufDesc, offset, size)
#define kflcnReadDmem_FNPTR(arg_this) arg_this->__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr->vtable.__kcrashcatEngineReadDmem__
#define kflcnReadDmem(arg_this, offset, size, pBuf) kflcnReadDmem_DISPATCH(arg_this, offset, size, pBuf)
#define kflcnReadDmem_HAL(arg_this, offset, size, pBuf) kflcnReadDmem_DISPATCH(arg_this, offset, size, pBuf)
#define kflcnReadEmem_FNPTR(arg_this) arg_this->__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr->vtable.__kcrashcatEngineReadEmem__
#define kflcnReadEmem(arg_this, offset, size, pBuf) kflcnReadEmem_DISPATCH(arg_this, offset, size, pBuf)
#define kflcnReadEmem_HAL(arg_this, offset, size, pBuf) kflcnReadEmem_DISPATCH(arg_this, offset, size, pBuf)
#define kflcnGetScratchOffsets_FNPTR(arg_this) arg_this->__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr->vtable.__kcrashcatEngineGetScratchOffsets__
#define kflcnGetScratchOffsets(arg_this, scratchGroupId) kflcnGetScratchOffsets_DISPATCH(arg_this, scratchGroupId)
#define kflcnGetScratchOffsets_HAL(arg_this, scratchGroupId) kflcnGetScratchOffsets_DISPATCH(arg_this, scratchGroupId)
#define kflcnGetWFL0Offset_FNPTR(arg_this) arg_this->__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr->vtable.__kcrashcatEngineGetWFL0Offset__
#define kflcnGetWFL0Offset(arg_this) kflcnGetWFL0Offset_DISPATCH(arg_this)
#define kflcnGetWFL0Offset_HAL(arg_this) kflcnGetWFL0Offset_DISPATCH(arg_this)

// Dispatch functions
static inline NvU32 kflcnRegRead_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 offset) {
    return pKernelFlcn->__kflcnRegRead__(pGpu, pKernelFlcn, offset);
}

static inline void kflcnRegWrite_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 offset, NvU32 data) {
    pKernelFlcn->__kflcnRegWrite__(pGpu, pKernelFlcn, offset, data);
}

static inline NvU32 kflcnRiscvRegRead_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 offset) {
    return pKernelFlcn->__kflcnRiscvRegRead__(pGpu, pKernelFlcn, offset);
}

static inline void kflcnRiscvRegWrite_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 offset, NvU32 data) {
    pKernelFlcn->__kflcnRiscvRegWrite__(pGpu, pKernelFlcn, offset, data);
}

static inline NvBool kflcnIsRiscvCpuEnabled_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    return pKernelFlcn->__kflcnIsRiscvCpuEnabled__(pGpu, pKernelFlcn);
}

static inline NvBool kflcnIsRiscvActive_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    return pKernelFlcn->__kflcnIsRiscvActive__(pGpu, pKernelFlcn);
}

static inline NvBool kflcnIsRiscvSelected_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    return pKernelFlcn->__kflcnIsRiscvSelected__(pGpu, pKernelFlcn);
}

static inline void kflcnRiscvProgramBcr_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvBool bBRFetch) {
    pKernelFlcn->__kflcnRiscvProgramBcr__(pGpu, pKernelFlcn, bBRFetch);
}

static inline void kflcnSwitchToFalcon_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    pKernelFlcn->__kflcnSwitchToFalcon__(pGpu, pKernelFlcn);
}

static inline NV_STATUS kflcnResetHw_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    return pKernelFlcn->__nvoc_metadata_ptr->vtable.__kflcnResetHw__(pGpu, pKernelFlcn);
}

static inline NV_STATUS kflcnReset_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    return pKernelFlcn->__kflcnReset__(pGpu, pKernelFlcn);
}

static inline NV_STATUS kflcnResetIntoRiscv_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    return pKernelFlcn->__kflcnResetIntoRiscv__(pGpu, pKernelFlcn);
}

static inline void kflcnStartCpu_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    pKernelFlcn->__kflcnStartCpu__(pGpu, pKernelFlcn);
}

static inline void kflcnDisableCtxReq_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    pKernelFlcn->__kflcnDisableCtxReq__(pGpu, pKernelFlcn);
}

static inline NV_STATUS kflcnPreResetWait_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    return pKernelFlcn->__kflcnPreResetWait__(pGpu, pKernelFlcn);
}

static inline NV_STATUS kflcnWaitForResetToFinish_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    return pKernelFlcn->__kflcnWaitForResetToFinish__(pGpu, pKernelFlcn);
}

static inline NV_STATUS kflcnWaitForHalt_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 timeoutUs, NvU32 flags) {
    return pKernelFlcn->__kflcnWaitForHalt__(pGpu, pKernelFlcn, timeoutUs, flags);
}

static inline NV_STATUS kflcnWaitForHaltRiscv_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 timeoutUs, NvU32 flags) {
    return pKernelFlcn->__kflcnWaitForHaltRiscv__(pGpu, pKernelFlcn, timeoutUs, flags);
}

static inline NvU32 kflcnReadIntrStatus_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKerneFlcn) {
    return pKerneFlcn->__kflcnReadIntrStatus__(pGpu, pKerneFlcn);
}

static inline NvU32 kflcnRiscvReadIntrStatus_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    return pKernelFlcn->__kflcnRiscvReadIntrStatus__(pGpu, pKernelFlcn);
}

static inline void kflcnIntrRetrigger_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    pKernelFlcn->__kflcnIntrRetrigger__(pGpu, pKernelFlcn);
}

static inline NvU32 kflcnMaskImemAddr_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 addr) {
    return pKernelFlcn->__kflcnMaskImemAddr__(pGpu, pKernelFlcn, addr);
}

static inline NvU32 kflcnMaskDmemAddr_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 addr) {
    return pKernelFlcn->__kflcnMaskDmemAddr__(pGpu, pKernelFlcn, addr);
}

static inline NvU32 kflcnGetEccInterruptMask_DISPATCH(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    return pKernelFlcn->__kflcnGetEccInterruptMask__(pGpu, pKernelFlcn);
}

static inline NvBool kflcnConfigured_DISPATCH(struct KernelFalcon *arg_this) {
    return arg_this->__nvoc_metadata_ptr->vtable.__kflcnConfigured__(arg_this);
}

static inline void kflcnUnload_DISPATCH(struct KernelFalcon *arg_this) {
    arg_this->__nvoc_metadata_ptr->vtable.__kflcnUnload__(arg_this);
}

static inline void kflcnVprintf_DISPATCH(struct KernelFalcon *arg_this, NvBool bReportStart, const char *fmt, va_list args) {
    arg_this->__nvoc_metadata_ptr->vtable.__kflcnVprintf__(arg_this, bReportStart, fmt, args);
}

static inline NvU32 kflcnPriRead_DISPATCH(struct KernelFalcon *arg_this, NvU32 offset) {
    return arg_this->__nvoc_metadata_ptr->vtable.__kflcnPriRead__(arg_this, offset);
}

static inline void kflcnPriWrite_DISPATCH(struct KernelFalcon *arg_this, NvU32 offset, NvU32 data) {
    arg_this->__nvoc_metadata_ptr->vtable.__kflcnPriWrite__(arg_this, offset, data);
}

static inline void * kflcnMapBufferDescriptor_DISPATCH(struct KernelFalcon *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    return arg_this->__nvoc_metadata_ptr->vtable.__kflcnMapBufferDescriptor__(arg_this, pBufDesc);
}

static inline void kflcnUnmapBufferDescriptor_DISPATCH(struct KernelFalcon *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    arg_this->__nvoc_metadata_ptr->vtable.__kflcnUnmapBufferDescriptor__(arg_this, pBufDesc);
}

static inline void kflcnSyncBufferDescriptor_DISPATCH(struct KernelFalcon *arg_this, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size) {
    arg_this->__nvoc_metadata_ptr->vtable.__kflcnSyncBufferDescriptor__(arg_this, pBufDesc, offset, size);
}

static inline void kflcnReadDmem_DISPATCH(struct KernelFalcon *arg_this, NvU32 offset, NvU32 size, void *pBuf) {
    arg_this->__nvoc_metadata_ptr->vtable.__kflcnReadDmem__(arg_this, offset, size, pBuf);
}

static inline void kflcnReadEmem_DISPATCH(struct KernelFalcon *arg_this, NvU64 offset, NvU64 size, void *pBuf) {
    arg_this->__nvoc_metadata_ptr->vtable.__kflcnReadEmem__(arg_this, offset, size, pBuf);
}

static inline const NvU32 * kflcnGetScratchOffsets_DISPATCH(struct KernelFalcon *arg_this, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId) {
    return arg_this->__nvoc_metadata_ptr->vtable.__kflcnGetScratchOffsets__(arg_this, scratchGroupId);
}

static inline NvU32 kflcnGetWFL0Offset_DISPATCH(struct KernelFalcon *arg_this) {
    return arg_this->__nvoc_metadata_ptr->vtable.__kflcnGetWFL0Offset__(arg_this);
}

NvU32 kflcnRegRead_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 offset);

static inline NvU32 kflcnRegRead_474d46(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 offset) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

void kflcnRegWrite_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 offset, NvU32 data);

static inline void kflcnRegWrite_f2d351(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 offset, NvU32 data) {
    NV_ASSERT_PRECOMP(0);
}

NvU32 kflcnRiscvRegRead_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 offset);

static inline NvU32 kflcnRiscvRegRead_474d46(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 offset) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

void kflcnRiscvRegWrite_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 offset, NvU32 data);

static inline void kflcnRiscvRegWrite_f2d351(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 offset, NvU32 data) {
    NV_ASSERT_PRECOMP(0);
}

NvBool kflcnIsRiscvCpuEnabled_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

static inline NvBool kflcnIsRiscvCpuEnabled_86b752(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_FALSE);
}

NvBool kflcnIsRiscvActive_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

NvBool kflcnIsRiscvActive_GA102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

static inline NvBool kflcnIsRiscvActive_86b752(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_FALSE);
}

NvBool kflcnIsRiscvSelected_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

NvBool kflcnIsRiscvSelected_GA102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

static inline NvBool kflcnIsRiscvSelected_e661f0(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    return NV_TRUE;
}

static inline NvBool kflcnIsRiscvSelected_86b752(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_FALSE);
}

void kflcnRiscvProgramBcr_GA102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvBool bBRFetch);

static inline void kflcnRiscvProgramBcr_f2d351(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvBool bBRFetch) {
    NV_ASSERT_PRECOMP(0);
}

void kflcnSwitchToFalcon_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

void kflcnSwitchToFalcon_GA102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

static inline void kflcnSwitchToFalcon_f2d351(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    NV_ASSERT_PRECOMP(0);
}

NV_STATUS kflcnReset_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

static inline NV_STATUS kflcnReset_5baef9(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS kflcnResetIntoRiscv_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

NV_STATUS kflcnResetIntoRiscv_GA102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

static inline NV_STATUS kflcnResetIntoRiscv_5baef9(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

void kflcnStartCpu_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

static inline void kflcnStartCpu_f2d351(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    NV_ASSERT_PRECOMP(0);
}

void kflcnDisableCtxReq_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

static inline void kflcnDisableCtxReq_f2d351(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    NV_ASSERT_PRECOMP(0);
}

NV_STATUS kflcnPreResetWait_GA102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

static inline NV_STATUS kflcnPreResetWait_56cd7a(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    return NV_OK;
}

static inline NV_STATUS kflcnPreResetWait_5baef9(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS kflcnWaitForResetToFinish_GA102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

NV_STATUS kflcnWaitForResetToFinish_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

static inline NV_STATUS kflcnWaitForResetToFinish_5baef9(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS kflcnWaitForHalt_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 timeoutUs, NvU32 flags);

static inline NV_STATUS kflcnWaitForHalt_5baef9(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 timeoutUs, NvU32 flags) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS kflcnWaitForHaltRiscv_GA102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 timeoutUs, NvU32 flags);

static inline NV_STATUS kflcnWaitForHaltRiscv_5baef9(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 timeoutUs, NvU32 flags) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NvU32 kflcnReadIntrStatus_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKerneFlcn);

static inline NvU32 kflcnReadIntrStatus_474d46(struct OBJGPU *pGpu, struct KernelFalcon *pKerneFlcn) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

NvU32 kflcnRiscvReadIntrStatus_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

NvU32 kflcnRiscvReadIntrStatus_GA102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

static inline NvU32 kflcnRiscvReadIntrStatus_474d46(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

void kflcnIntrRetrigger_GA100(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

static inline void kflcnIntrRetrigger_b3696a(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    return;
}

static inline void kflcnIntrRetrigger_f2d351(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    NV_ASSERT_PRECOMP(0);
}

NvU32 kflcnMaskImemAddr_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 addr);

NvU32 kflcnMaskImemAddr_GA100(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 addr);

static inline NvU32 kflcnMaskImemAddr_474d46(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 addr) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

NvU32 kflcnMaskDmemAddr_TU102(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 addr);

NvU32 kflcnMaskDmemAddr_GA100(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 addr);

static inline NvU32 kflcnMaskDmemAddr_474d46(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn, NvU32 addr) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

NvU32 kflcnGetEccInterruptMask_GB100(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn);

static inline NvU32 kflcnGetEccInterruptMask_4a4dee(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFlcn) {
    return 0;
}

void kflcnConfigureEngine_IMPL(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFalcon, KernelFalconEngineConfig *pFalconConfig);

#ifdef __nvoc_kernel_falcon_h_disabled
static inline void kflcnConfigureEngine(struct OBJGPU *pGpu, struct KernelFalcon *pKernelFalcon, KernelFalconEngineConfig *pFalconConfig) {
    NV_ASSERT_FAILED_PRECOMP("KernelFalcon was disabled!");
}
#else //__nvoc_kernel_falcon_h_disabled
#define kflcnConfigureEngine(pGpu, pKernelFalcon, pFalconConfig) kflcnConfigureEngine_IMPL(pGpu, pKernelFalcon, pFalconConfig)
#endif //__nvoc_kernel_falcon_h_disabled

NvU32 kflcnGetPendingHostInterrupts_IMPL(struct OBJGPU *arg1, struct KernelFalcon *arg2);

#ifdef __nvoc_kernel_falcon_h_disabled
static inline NvU32 kflcnGetPendingHostInterrupts(struct OBJGPU *arg1, struct KernelFalcon *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelFalcon was disabled!");
    return 0;
}
#else //__nvoc_kernel_falcon_h_disabled
#define kflcnGetPendingHostInterrupts(arg1, arg2) kflcnGetPendingHostInterrupts_IMPL(arg1, arg2)
#endif //__nvoc_kernel_falcon_h_disabled

NV_STATUS kflcnAllocContext_IMPL(struct OBJGPU *arg1, struct KernelFalcon *arg2, struct KernelChannel *arg3, NvU32 arg4);

#ifdef __nvoc_kernel_falcon_h_disabled
static inline NV_STATUS kflcnAllocContext(struct OBJGPU *arg1, struct KernelFalcon *arg2, struct KernelChannel *arg3, NvU32 arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelFalcon was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_falcon_h_disabled
#define kflcnAllocContext(arg1, arg2, arg3, arg4) kflcnAllocContext_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_falcon_h_disabled

NV_STATUS kflcnFreeContext_IMPL(struct OBJGPU *arg1, struct KernelFalcon *arg2, struct KernelChannel *arg3, NvU32 arg4);

#ifdef __nvoc_kernel_falcon_h_disabled
static inline NV_STATUS kflcnFreeContext(struct OBJGPU *arg1, struct KernelFalcon *arg2, struct KernelChannel *arg3, NvU32 arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelFalcon was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_falcon_h_disabled
#define kflcnFreeContext(arg1, arg2, arg3, arg4) kflcnFreeContext_IMPL(arg1, arg2, arg3, arg4)
#endif //__nvoc_kernel_falcon_h_disabled

struct KernelFalcon *kflcnGetKernelFalconForEngine_IMPL(struct OBJGPU *pGpu, ENGDESCRIPTOR physEngDesc);

#define kflcnGetKernelFalconForEngine(pGpu, physEngDesc) kflcnGetKernelFalconForEngine_IMPL(pGpu, physEngDesc)
#undef PRIVATE_FIELD


static inline void kflcnSetRiscvMode(KernelFalcon *pKernelFlcn, NvBool bRiscvMode)
{
    pKernelFlcn->riscvMode = (NvTristate)bRiscvMode;
}

/*! Caches and returns the value of kflcnIsRiscvSelected_HAL() */
static inline NvBool kflcnIsRiscvMode(OBJGPU *pGpu, KernelFalcon *pKernelFlcn)
{
    if (pKernelFlcn->riscvMode == NV_TRISTATE_INDETERMINATE)
        kflcnSetRiscvMode(pKernelFlcn, kflcnIsRiscvSelected_HAL(pGpu, pKernelFlcn));

    return (NvBool)pKernelFlcn->riscvMode;
}

// Basic implementation of KernelFalcon that can be instantiated.

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_FALCON_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__GenericKernelFalcon;
struct NVOC_METADATA__KernelFalcon;
struct NVOC_METADATA__IntrService;
struct NVOC_METADATA__Object;
struct NVOC_VTABLE__GenericKernelFalcon;


struct GenericKernelFalcon {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GenericKernelFalcon *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct KernelFalcon __nvoc_base_KernelFalcon;
    struct IntrService __nvoc_base_IntrService;
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct CrashCatEngine *__nvoc_pbase_CrashCatEngine;    // crashcatEngine super^3
    struct KernelCrashCatEngine *__nvoc_pbase_KernelCrashCatEngine;    // kcrashcatEngine super^2
    struct KernelFalcon *__nvoc_pbase_KernelFalcon;    // kflcn super
    struct IntrService *__nvoc_pbase_IntrService;    // intrserv super
    struct Object *__nvoc_pbase_Object;    // obj super
    struct GenericKernelFalcon *__nvoc_pbase_GenericKernelFalcon;    // gkflcn

    // Vtable with 3 per-object function pointers
    NvU32 (*__gkflcnRegRead__)(struct OBJGPU *, struct GenericKernelFalcon * /*this*/, NvU32);  // virtual halified (2 hals) inherited (kflcn) base (kflcn) body
    void (*__gkflcnRegWrite__)(struct OBJGPU *, struct GenericKernelFalcon * /*this*/, NvU32, NvU32);  // virtual halified (2 hals) inherited (kflcn) base (kflcn) body
    NvU32 (*__gkflcnMaskDmemAddr__)(struct OBJGPU *, struct GenericKernelFalcon * /*this*/, NvU32);  // virtual halified (3 hals) inherited (kflcn) base (kflcn) body
};


// Vtable with 17 per-class function pointers
struct NVOC_VTABLE__GenericKernelFalcon {
    NV_STATUS (*__gkflcnResetHw__)(struct OBJGPU *, struct GenericKernelFalcon * /*this*/);  // virtual override (kflcn) base (kflcn)
    void (*__gkflcnRegisterIntrService__)(struct OBJGPU *, struct GenericKernelFalcon * /*this*/, IntrServiceRecord *);  // virtual override (intrserv) base (intrserv)
    NV_STATUS (*__gkflcnServiceNotificationInterrupt__)(struct OBJGPU *, struct GenericKernelFalcon * /*this*/, IntrServiceServiceNotificationInterruptArguments *);  // virtual override (intrserv) base (intrserv)
    NvBool (*__gkflcnConfigured__)(struct GenericKernelFalcon * /*this*/);  // virtual inherited (kcrashcatEngine) base (kflcn)
    void (*__gkflcnUnload__)(struct GenericKernelFalcon * /*this*/);  // virtual inherited (kcrashcatEngine) base (kflcn)
    void (*__gkflcnVprintf__)(struct GenericKernelFalcon * /*this*/, NvBool, const char *, va_list);  // virtual inherited (kcrashcatEngine) base (kflcn)
    NvU32 (*__gkflcnPriRead__)(struct GenericKernelFalcon * /*this*/, NvU32);  // virtual inherited (kcrashcatEngine) base (kflcn)
    void (*__gkflcnPriWrite__)(struct GenericKernelFalcon * /*this*/, NvU32, NvU32);  // virtual inherited (kcrashcatEngine) base (kflcn)
    void * (*__gkflcnMapBufferDescriptor__)(struct GenericKernelFalcon * /*this*/, CrashCatBufferDescriptor *);  // virtual inherited (kcrashcatEngine) base (kflcn)
    void (*__gkflcnUnmapBufferDescriptor__)(struct GenericKernelFalcon * /*this*/, CrashCatBufferDescriptor *);  // virtual inherited (kcrashcatEngine) base (kflcn)
    void (*__gkflcnSyncBufferDescriptor__)(struct GenericKernelFalcon * /*this*/, CrashCatBufferDescriptor *, NvU32, NvU32);  // virtual inherited (kcrashcatEngine) base (kflcn)
    void (*__gkflcnReadDmem__)(struct GenericKernelFalcon * /*this*/, NvU32, NvU32, void *);  // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
    void (*__gkflcnReadEmem__)(struct GenericKernelFalcon * /*this*/, NvU64, NvU64, void *);  // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
    const NvU32 * (*__gkflcnGetScratchOffsets__)(struct GenericKernelFalcon * /*this*/, NV_CRASHCAT_SCRATCH_GROUP_ID);  // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
    NvU32 (*__gkflcnGetWFL0Offset__)(struct GenericKernelFalcon * /*this*/);  // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
    NvBool (*__gkflcnClearInterrupt__)(struct OBJGPU *, struct GenericKernelFalcon * /*this*/, IntrServiceClearInterruptArguments *);  // virtual inherited (intrserv) base (intrserv)
    NvU32 (*__gkflcnServiceInterrupt__)(struct OBJGPU *, struct GenericKernelFalcon * /*this*/, IntrServiceServiceInterruptArguments *);  // virtual inherited (intrserv) base (intrserv)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__GenericKernelFalcon {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__KernelFalcon metadata__KernelFalcon;
    const struct NVOC_METADATA__IntrService metadata__IntrService;
    const struct NVOC_METADATA__Object metadata__Object;
    const struct NVOC_VTABLE__GenericKernelFalcon vtable;
};

#ifndef __NVOC_CLASS_GenericKernelFalcon_TYPEDEF__
#define __NVOC_CLASS_GenericKernelFalcon_TYPEDEF__
typedef struct GenericKernelFalcon GenericKernelFalcon;
#endif /* __NVOC_CLASS_GenericKernelFalcon_TYPEDEF__ */

#ifndef __nvoc_class_id_GenericKernelFalcon
#define __nvoc_class_id_GenericKernelFalcon 0xabcf08
#endif /* __nvoc_class_id_GenericKernelFalcon */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GenericKernelFalcon;

#define __staticCast_GenericKernelFalcon(pThis) \
    ((pThis)->__nvoc_pbase_GenericKernelFalcon)

#ifdef __nvoc_kernel_falcon_h_disabled
#define __dynamicCast_GenericKernelFalcon(pThis) ((GenericKernelFalcon*) NULL)
#else //__nvoc_kernel_falcon_h_disabled
#define __dynamicCast_GenericKernelFalcon(pThis) \
    ((GenericKernelFalcon*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GenericKernelFalcon)))
#endif //__nvoc_kernel_falcon_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GenericKernelFalcon(GenericKernelFalcon**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GenericKernelFalcon(GenericKernelFalcon**, Dynamic*, NvU32, struct OBJGPU *arg_pGpu, KernelFalconEngineConfig *arg_pFalconConfig);
#define __objCreate_GenericKernelFalcon(ppNewObj, pParent, createFlags, arg_pGpu, arg_pFalconConfig) \
    __nvoc_objCreate_GenericKernelFalcon((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pGpu, arg_pFalconConfig)


// Wrapper macros
#define gkflcnResetHw_FNPTR(pGenKernFlcn) pGenKernFlcn->__nvoc_metadata_ptr->vtable.__gkflcnResetHw__
#define gkflcnResetHw(pGpu, pGenKernFlcn) gkflcnResetHw_DISPATCH(pGpu, pGenKernFlcn)
#define gkflcnRegisterIntrService_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__gkflcnRegisterIntrService__
#define gkflcnRegisterIntrService(arg1, arg_this, arg3) gkflcnRegisterIntrService_DISPATCH(arg1, arg_this, arg3)
#define gkflcnServiceNotificationInterrupt_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__gkflcnServiceNotificationInterrupt__
#define gkflcnServiceNotificationInterrupt(arg1, arg_this, arg3) gkflcnServiceNotificationInterrupt_DISPATCH(arg1, arg_this, arg3)
#define gkflcnRegRead_FNPTR(pKernelFlcn) pKernelFlcn->__nvoc_base_KernelFalcon.__kflcnRegRead__
#define gkflcnRegRead(pGpu, pKernelFlcn, offset) gkflcnRegRead_DISPATCH(pGpu, pKernelFlcn, offset)
#define gkflcnRegRead_HAL(pGpu, pKernelFlcn, offset) gkflcnRegRead_DISPATCH(pGpu, pKernelFlcn, offset)
#define gkflcnRegWrite_FNPTR(pKernelFlcn) pKernelFlcn->__nvoc_base_KernelFalcon.__kflcnRegWrite__
#define gkflcnRegWrite(pGpu, pKernelFlcn, offset, data) gkflcnRegWrite_DISPATCH(pGpu, pKernelFlcn, offset, data)
#define gkflcnRegWrite_HAL(pGpu, pKernelFlcn, offset, data) gkflcnRegWrite_DISPATCH(pGpu, pKernelFlcn, offset, data)
#define gkflcnMaskDmemAddr_FNPTR(pKernelFlcn) pKernelFlcn->__nvoc_base_KernelFalcon.__kflcnMaskDmemAddr__
#define gkflcnMaskDmemAddr(pGpu, pKernelFlcn, addr) gkflcnMaskDmemAddr_DISPATCH(pGpu, pKernelFlcn, addr)
#define gkflcnMaskDmemAddr_HAL(pGpu, pKernelFlcn, addr) gkflcnMaskDmemAddr_DISPATCH(pGpu, pKernelFlcn, addr)
#define gkflcnConfigured_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr->vtable.__kcrashcatEngineConfigured__
#define gkflcnConfigured(arg_this) gkflcnConfigured_DISPATCH(arg_this)
#define gkflcnUnload_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr->vtable.__kcrashcatEngineUnload__
#define gkflcnUnload(arg_this) gkflcnUnload_DISPATCH(arg_this)
#define gkflcnVprintf_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr->vtable.__kcrashcatEngineVprintf__
#define gkflcnVprintf(arg_this, bReportStart, fmt, args) gkflcnVprintf_DISPATCH(arg_this, bReportStart, fmt, args)
#define gkflcnPriRead_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr->vtable.__kcrashcatEnginePriRead__
#define gkflcnPriRead(arg_this, offset) gkflcnPriRead_DISPATCH(arg_this, offset)
#define gkflcnPriWrite_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr->vtable.__kcrashcatEnginePriWrite__
#define gkflcnPriWrite(arg_this, offset, data) gkflcnPriWrite_DISPATCH(arg_this, offset, data)
#define gkflcnMapBufferDescriptor_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr->vtable.__kcrashcatEngineMapBufferDescriptor__
#define gkflcnMapBufferDescriptor(arg_this, pBufDesc) gkflcnMapBufferDescriptor_DISPATCH(arg_this, pBufDesc)
#define gkflcnUnmapBufferDescriptor_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr->vtable.__kcrashcatEngineUnmapBufferDescriptor__
#define gkflcnUnmapBufferDescriptor(arg_this, pBufDesc) gkflcnUnmapBufferDescriptor_DISPATCH(arg_this, pBufDesc)
#define gkflcnSyncBufferDescriptor_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr->vtable.__kcrashcatEngineSyncBufferDescriptor__
#define gkflcnSyncBufferDescriptor(arg_this, pBufDesc, offset, size) gkflcnSyncBufferDescriptor_DISPATCH(arg_this, pBufDesc, offset, size)
#define gkflcnReadDmem_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr->vtable.__kcrashcatEngineReadDmem__
#define gkflcnReadDmem(arg_this, offset, size, pBuf) gkflcnReadDmem_DISPATCH(arg_this, offset, size, pBuf)
#define gkflcnReadDmem_HAL(arg_this, offset, size, pBuf) gkflcnReadDmem_DISPATCH(arg_this, offset, size, pBuf)
#define gkflcnReadEmem_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr->vtable.__kcrashcatEngineReadEmem__
#define gkflcnReadEmem(arg_this, offset, size, pBuf) gkflcnReadEmem_DISPATCH(arg_this, offset, size, pBuf)
#define gkflcnReadEmem_HAL(arg_this, offset, size, pBuf) gkflcnReadEmem_DISPATCH(arg_this, offset, size, pBuf)
#define gkflcnGetScratchOffsets_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr->vtable.__kcrashcatEngineGetScratchOffsets__
#define gkflcnGetScratchOffsets(arg_this, scratchGroupId) gkflcnGetScratchOffsets_DISPATCH(arg_this, scratchGroupId)
#define gkflcnGetScratchOffsets_HAL(arg_this, scratchGroupId) gkflcnGetScratchOffsets_DISPATCH(arg_this, scratchGroupId)
#define gkflcnGetWFL0Offset_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr->vtable.__kcrashcatEngineGetWFL0Offset__
#define gkflcnGetWFL0Offset(arg_this) gkflcnGetWFL0Offset_DISPATCH(arg_this)
#define gkflcnGetWFL0Offset_HAL(arg_this) gkflcnGetWFL0Offset_DISPATCH(arg_this)
#define gkflcnClearInterrupt_FNPTR(pIntrService) pIntrService->__nvoc_base_IntrService.__nvoc_metadata_ptr->vtable.__intrservClearInterrupt__
#define gkflcnClearInterrupt(pGpu, pIntrService, pParams) gkflcnClearInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define gkflcnServiceInterrupt_FNPTR(pIntrService) pIntrService->__nvoc_base_IntrService.__nvoc_metadata_ptr->vtable.__intrservServiceInterrupt__
#define gkflcnServiceInterrupt(pGpu, pIntrService, pParams) gkflcnServiceInterrupt_DISPATCH(pGpu, pIntrService, pParams)

// Dispatch functions
static inline NV_STATUS gkflcnResetHw_DISPATCH(struct OBJGPU *pGpu, struct GenericKernelFalcon *pGenKernFlcn) {
    return pGenKernFlcn->__nvoc_metadata_ptr->vtable.__gkflcnResetHw__(pGpu, pGenKernFlcn);
}

static inline void gkflcnRegisterIntrService_DISPATCH(struct OBJGPU *arg1, struct GenericKernelFalcon *arg_this, IntrServiceRecord arg3[179]) {
    arg_this->__nvoc_metadata_ptr->vtable.__gkflcnRegisterIntrService__(arg1, arg_this, arg3);
}

static inline NV_STATUS gkflcnServiceNotificationInterrupt_DISPATCH(struct OBJGPU *arg1, struct GenericKernelFalcon *arg_this, IntrServiceServiceNotificationInterruptArguments *arg3) {
    return arg_this->__nvoc_metadata_ptr->vtable.__gkflcnServiceNotificationInterrupt__(arg1, arg_this, arg3);
}

static inline NvU32 gkflcnRegRead_DISPATCH(struct OBJGPU *pGpu, struct GenericKernelFalcon *pKernelFlcn, NvU32 offset) {
    return pKernelFlcn->__gkflcnRegRead__(pGpu, pKernelFlcn, offset);
}

static inline void gkflcnRegWrite_DISPATCH(struct OBJGPU *pGpu, struct GenericKernelFalcon *pKernelFlcn, NvU32 offset, NvU32 data) {
    pKernelFlcn->__gkflcnRegWrite__(pGpu, pKernelFlcn, offset, data);
}

static inline NvU32 gkflcnMaskDmemAddr_DISPATCH(struct OBJGPU *pGpu, struct GenericKernelFalcon *pKernelFlcn, NvU32 addr) {
    return pKernelFlcn->__gkflcnMaskDmemAddr__(pGpu, pKernelFlcn, addr);
}

static inline NvBool gkflcnConfigured_DISPATCH(struct GenericKernelFalcon *arg_this) {
    return arg_this->__nvoc_metadata_ptr->vtable.__gkflcnConfigured__(arg_this);
}

static inline void gkflcnUnload_DISPATCH(struct GenericKernelFalcon *arg_this) {
    arg_this->__nvoc_metadata_ptr->vtable.__gkflcnUnload__(arg_this);
}

static inline void gkflcnVprintf_DISPATCH(struct GenericKernelFalcon *arg_this, NvBool bReportStart, const char *fmt, va_list args) {
    arg_this->__nvoc_metadata_ptr->vtable.__gkflcnVprintf__(arg_this, bReportStart, fmt, args);
}

static inline NvU32 gkflcnPriRead_DISPATCH(struct GenericKernelFalcon *arg_this, NvU32 offset) {
    return arg_this->__nvoc_metadata_ptr->vtable.__gkflcnPriRead__(arg_this, offset);
}

static inline void gkflcnPriWrite_DISPATCH(struct GenericKernelFalcon *arg_this, NvU32 offset, NvU32 data) {
    arg_this->__nvoc_metadata_ptr->vtable.__gkflcnPriWrite__(arg_this, offset, data);
}

static inline void * gkflcnMapBufferDescriptor_DISPATCH(struct GenericKernelFalcon *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    return arg_this->__nvoc_metadata_ptr->vtable.__gkflcnMapBufferDescriptor__(arg_this, pBufDesc);
}

static inline void gkflcnUnmapBufferDescriptor_DISPATCH(struct GenericKernelFalcon *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    arg_this->__nvoc_metadata_ptr->vtable.__gkflcnUnmapBufferDescriptor__(arg_this, pBufDesc);
}

static inline void gkflcnSyncBufferDescriptor_DISPATCH(struct GenericKernelFalcon *arg_this, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size) {
    arg_this->__nvoc_metadata_ptr->vtable.__gkflcnSyncBufferDescriptor__(arg_this, pBufDesc, offset, size);
}

static inline void gkflcnReadDmem_DISPATCH(struct GenericKernelFalcon *arg_this, NvU32 offset, NvU32 size, void *pBuf) {
    arg_this->__nvoc_metadata_ptr->vtable.__gkflcnReadDmem__(arg_this, offset, size, pBuf);
}

static inline void gkflcnReadEmem_DISPATCH(struct GenericKernelFalcon *arg_this, NvU64 offset, NvU64 size, void *pBuf) {
    arg_this->__nvoc_metadata_ptr->vtable.__gkflcnReadEmem__(arg_this, offset, size, pBuf);
}

static inline const NvU32 * gkflcnGetScratchOffsets_DISPATCH(struct GenericKernelFalcon *arg_this, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId) {
    return arg_this->__nvoc_metadata_ptr->vtable.__gkflcnGetScratchOffsets__(arg_this, scratchGroupId);
}

static inline NvU32 gkflcnGetWFL0Offset_DISPATCH(struct GenericKernelFalcon *arg_this) {
    return arg_this->__nvoc_metadata_ptr->vtable.__gkflcnGetWFL0Offset__(arg_this);
}

static inline NvBool gkflcnClearInterrupt_DISPATCH(struct OBJGPU *pGpu, struct GenericKernelFalcon *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return pIntrService->__nvoc_metadata_ptr->vtable.__gkflcnClearInterrupt__(pGpu, pIntrService, pParams);
}

static inline NvU32 gkflcnServiceInterrupt_DISPATCH(struct OBJGPU *pGpu, struct GenericKernelFalcon *pIntrService, IntrServiceServiceInterruptArguments *pParams) {
    return pIntrService->__nvoc_metadata_ptr->vtable.__gkflcnServiceInterrupt__(pGpu, pIntrService, pParams);
}

NV_STATUS gkflcnResetHw_IMPL(struct OBJGPU *pGpu, struct GenericKernelFalcon *pGenKernFlcn);

void gkflcnRegisterIntrService_IMPL(struct OBJGPU *arg1, struct GenericKernelFalcon *arg2, IntrServiceRecord arg3[179]);

NV_STATUS gkflcnServiceNotificationInterrupt_IMPL(struct OBJGPU *arg1, struct GenericKernelFalcon *arg2, IntrServiceServiceNotificationInterruptArguments *arg3);

NV_STATUS gkflcnConstruct_IMPL(struct GenericKernelFalcon *arg_pGenKernFlcn, struct OBJGPU *arg_pGpu, KernelFalconEngineConfig *arg_pFalconConfig);

#define __nvoc_gkflcnConstruct(arg_pGenKernFlcn, arg_pGpu, arg_pFalconConfig) gkflcnConstruct_IMPL(arg_pGenKernFlcn, arg_pGpu, arg_pFalconConfig)
#undef PRIVATE_FIELD


#endif // KERNEL_FALCON_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_FALCON_NVOC_H_
