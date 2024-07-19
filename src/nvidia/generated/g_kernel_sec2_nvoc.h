
#ifndef _G_KERNEL_SEC2_NVOC_H_
#define _G_KERNEL_SEC2_NVOC_H_
#include "nvoc/runtime.h"

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 0

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_kernel_sec2_nvoc.h"

#ifndef KERNEL_SEC2_H
#define KERNEL_SEC2_H

#include "core/bin_data.h"
#include "core/core.h"
#include "gpu/eng_state.h"
#include "gpu/falcon/kernel_falcon.h"
#include "gpu/gpu.h"

// forward declaration of RM_FLCN_BL_DESC from rmflcnbl.h
struct _def_rm_flcn_bl_desc;
typedef struct _def_rm_flcn_bl_desc RM_FLCN_BL_DESC;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_SEC2_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct KernelSec2 {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct IntrService __nvoc_base_IntrService;
    struct KernelFalcon __nvoc_base_KernelFalcon;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct IntrService *__nvoc_pbase_IntrService;    // intrserv super
    struct CrashCatEngine *__nvoc_pbase_CrashCatEngine;    // crashcatEngine super^3
    struct KernelCrashCatEngine *__nvoc_pbase_KernelCrashCatEngine;    // kcrashcatEngine super^2
    struct KernelFalcon *__nvoc_pbase_KernelFalcon;    // kflcn super
    struct KernelSec2 *__nvoc_pbase_KernelSec2;    // ksec2

    // Vtable with 39 per-object function pointers
    NV_STATUS (*__ksec2ConstructEngine__)(struct OBJGPU *, struct KernelSec2 * /*this*/, ENGDESCRIPTOR);  // virtual halified (singleton optimized) override (engstate) base (engstate) body
    void (*__ksec2RegisterIntrService__)(struct OBJGPU *, struct KernelSec2 * /*this*/, IntrServiceRecord *);  // virtual halified (singleton optimized) override (intrserv) base (intrserv) body
    NV_STATUS (*__ksec2ServiceNotificationInterrupt__)(struct OBJGPU *, struct KernelSec2 * /*this*/, IntrServiceServiceNotificationInterruptArguments *);  // virtual halified (singleton optimized) override (intrserv) base (intrserv)
    NV_STATUS (*__ksec2ConfigureFalcon__)(struct OBJGPU *, struct KernelSec2 * /*this*/);  // halified (3 hals) body
    NV_STATUS (*__ksec2ResetHw__)(struct OBJGPU *, struct KernelSec2 * /*this*/);  // virtual halified (2 hals) override (kflcn) base (kflcn) body
    NV_STATUS (*__ksec2StateLoad__)(struct OBJGPU *, struct KernelSec2 * /*this*/, NvU32);  // virtual halified (3 hals) override (engstate) base (engstate) body
    NvU32 (*__ksec2ReadUcodeFuseVersion__)(struct OBJGPU *, struct KernelSec2 * /*this*/, NvU32);  // halified (3 hals) body
    const BINDATA_ARCHIVE * (*__ksec2GetBinArchiveBlUcode__)(struct OBJGPU *, struct KernelSec2 * /*this*/);  // halified (3 hals) body
    NV_STATUS (*__ksec2GetGenericBlUcode__)(struct OBJGPU *, struct KernelSec2 * /*this*/, const RM_FLCN_BL_DESC **, const NvU8 **);  // halified (3 hals) body
    const BINDATA_ARCHIVE * (*__ksec2GetBinArchiveSecurescrubUcode__)(struct OBJGPU *, struct KernelSec2 * /*this*/);  // halified (3 hals) body
    void (*__ksec2InitMissing__)(struct OBJGPU *, struct KernelSec2 * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__ksec2StatePreInitLocked__)(struct OBJGPU *, struct KernelSec2 * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__ksec2StatePreInitUnlocked__)(struct OBJGPU *, struct KernelSec2 * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__ksec2StateInitLocked__)(struct OBJGPU *, struct KernelSec2 * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__ksec2StateInitUnlocked__)(struct OBJGPU *, struct KernelSec2 * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__ksec2StatePreLoad__)(struct OBJGPU *, struct KernelSec2 * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__ksec2StatePostLoad__)(struct OBJGPU *, struct KernelSec2 * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__ksec2StatePreUnload__)(struct OBJGPU *, struct KernelSec2 * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__ksec2StateUnload__)(struct OBJGPU *, struct KernelSec2 * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__ksec2StatePostUnload__)(struct OBJGPU *, struct KernelSec2 * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    void (*__ksec2StateDestroy__)(struct OBJGPU *, struct KernelSec2 * /*this*/);  // virtual inherited (engstate) base (engstate)
    NvBool (*__ksec2IsPresent__)(struct OBJGPU *, struct KernelSec2 * /*this*/);  // virtual inherited (engstate) base (engstate)
    NvBool (*__ksec2ClearInterrupt__)(struct OBJGPU *, struct KernelSec2 * /*this*/, IntrServiceClearInterruptArguments *);  // virtual inherited (intrserv) base (intrserv)
    NvU32 (*__ksec2ServiceInterrupt__)(struct OBJGPU *, struct KernelSec2 * /*this*/, IntrServiceServiceInterruptArguments *);  // virtual inherited (intrserv) base (intrserv)
    NvU32 (*__ksec2RegRead__)(struct OBJGPU *, struct KernelSec2 * /*this*/, NvU32);  // virtual halified (2 hals) inherited (kflcn) base (kflcn) body
    void (*__ksec2RegWrite__)(struct OBJGPU *, struct KernelSec2 * /*this*/, NvU32, NvU32);  // virtual halified (2 hals) inherited (kflcn) base (kflcn) body
    NvU32 (*__ksec2MaskDmemAddr__)(struct OBJGPU *, struct KernelSec2 * /*this*/, NvU32);  // virtual halified (3 hals) inherited (kflcn) base (kflcn) body
    NvBool (*__ksec2Configured__)(struct KernelSec2 * /*this*/);  // virtual inherited (kcrashcatEngine) base (kflcn)
    void (*__ksec2Unload__)(struct KernelSec2 * /*this*/);  // virtual inherited (kcrashcatEngine) base (kflcn)
    void (*__ksec2Vprintf__)(struct KernelSec2 * /*this*/, NvBool, const char *, va_list);  // virtual inherited (kcrashcatEngine) base (kflcn)
    NvU32 (*__ksec2PriRead__)(struct KernelSec2 * /*this*/, NvU32);  // virtual inherited (kcrashcatEngine) base (kflcn)
    void (*__ksec2PriWrite__)(struct KernelSec2 * /*this*/, NvU32, NvU32);  // virtual inherited (kcrashcatEngine) base (kflcn)
    void * (*__ksec2MapBufferDescriptor__)(struct KernelSec2 * /*this*/, CrashCatBufferDescriptor *);  // virtual inherited (kcrashcatEngine) base (kflcn)
    void (*__ksec2UnmapBufferDescriptor__)(struct KernelSec2 * /*this*/, CrashCatBufferDescriptor *);  // virtual inherited (kcrashcatEngine) base (kflcn)
    void (*__ksec2SyncBufferDescriptor__)(struct KernelSec2 * /*this*/, CrashCatBufferDescriptor *, NvU32, NvU32);  // virtual inherited (kcrashcatEngine) base (kflcn)
    void (*__ksec2ReadDmem__)(struct KernelSec2 * /*this*/, NvU32, NvU32, void *);  // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
    void (*__ksec2ReadEmem__)(struct KernelSec2 * /*this*/, NvU64, NvU64, void *);  // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
    const NvU32 * (*__ksec2GetScratchOffsets__)(struct KernelSec2 * /*this*/, NV_CRASHCAT_SCRATCH_GROUP_ID);  // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
    NvU32 (*__ksec2GetWFL0Offset__)(struct KernelSec2 * /*this*/);  // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)

    // Data members
    const RM_FLCN_BL_DESC *pGenericBlUcodeDesc;
    const NvU8 *pGenericBlUcodeImg;
};

#ifndef __NVOC_CLASS_KernelSec2_TYPEDEF__
#define __NVOC_CLASS_KernelSec2_TYPEDEF__
typedef struct KernelSec2 KernelSec2;
#endif /* __NVOC_CLASS_KernelSec2_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelSec2
#define __nvoc_class_id_KernelSec2 0x2f36c9
#endif /* __nvoc_class_id_KernelSec2 */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelSec2;

#define __staticCast_KernelSec2(pThis) \
    ((pThis)->__nvoc_pbase_KernelSec2)

#ifdef __nvoc_kernel_sec2_h_disabled
#define __dynamicCast_KernelSec2(pThis) ((KernelSec2*)NULL)
#else //__nvoc_kernel_sec2_h_disabled
#define __dynamicCast_KernelSec2(pThis) \
    ((KernelSec2*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelSec2)))
#endif //__nvoc_kernel_sec2_h_disabled

// Property macros
#define PDB_PROP_KSEC2_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KSEC2_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelSec2(KernelSec2**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelSec2(KernelSec2**, Dynamic*, NvU32);
#define __objCreate_KernelSec2(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelSec2((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define ksec2ConstructEngine_FNPTR(pKernelSec2) pKernelSec2->__ksec2ConstructEngine__
#define ksec2ConstructEngine(pGpu, pKernelSec2, arg3) ksec2ConstructEngine_DISPATCH(pGpu, pKernelSec2, arg3)
#define ksec2ConstructEngine_HAL(pGpu, pKernelSec2, arg3) ksec2ConstructEngine_DISPATCH(pGpu, pKernelSec2, arg3)
#define ksec2RegisterIntrService_FNPTR(pKernelSec2) pKernelSec2->__ksec2RegisterIntrService__
#define ksec2RegisterIntrService(pGpu, pKernelSec2, pRecords) ksec2RegisterIntrService_DISPATCH(pGpu, pKernelSec2, pRecords)
#define ksec2RegisterIntrService_HAL(pGpu, pKernelSec2, pRecords) ksec2RegisterIntrService_DISPATCH(pGpu, pKernelSec2, pRecords)
#define ksec2ServiceNotificationInterrupt_FNPTR(arg_this) arg_this->__ksec2ServiceNotificationInterrupt__
#define ksec2ServiceNotificationInterrupt(arg1, arg_this, arg3) ksec2ServiceNotificationInterrupt_DISPATCH(arg1, arg_this, arg3)
#define ksec2ServiceNotificationInterrupt_HAL(arg1, arg_this, arg3) ksec2ServiceNotificationInterrupt_DISPATCH(arg1, arg_this, arg3)
#define ksec2ConfigureFalcon_FNPTR(pKernelSec2) pKernelSec2->__ksec2ConfigureFalcon__
#define ksec2ConfigureFalcon(pGpu, pKernelSec2) ksec2ConfigureFalcon_DISPATCH(pGpu, pKernelSec2)
#define ksec2ConfigureFalcon_HAL(pGpu, pKernelSec2) ksec2ConfigureFalcon_DISPATCH(pGpu, pKernelSec2)
#define ksec2ResetHw_FNPTR(pKernelSec2) pKernelSec2->__ksec2ResetHw__
#define ksec2ResetHw(pGpu, pKernelSec2) ksec2ResetHw_DISPATCH(pGpu, pKernelSec2)
#define ksec2ResetHw_HAL(pGpu, pKernelSec2) ksec2ResetHw_DISPATCH(pGpu, pKernelSec2)
#define ksec2StateLoad_FNPTR(pKernelSec2) pKernelSec2->__ksec2StateLoad__
#define ksec2StateLoad(pGpu, pKernelSec2, arg3) ksec2StateLoad_DISPATCH(pGpu, pKernelSec2, arg3)
#define ksec2StateLoad_HAL(pGpu, pKernelSec2, arg3) ksec2StateLoad_DISPATCH(pGpu, pKernelSec2, arg3)
#define ksec2ReadUcodeFuseVersion_FNPTR(pKernelSec2) pKernelSec2->__ksec2ReadUcodeFuseVersion__
#define ksec2ReadUcodeFuseVersion(pGpu, pKernelSec2, ucodeId) ksec2ReadUcodeFuseVersion_DISPATCH(pGpu, pKernelSec2, ucodeId)
#define ksec2ReadUcodeFuseVersion_HAL(pGpu, pKernelSec2, ucodeId) ksec2ReadUcodeFuseVersion_DISPATCH(pGpu, pKernelSec2, ucodeId)
#define ksec2GetBinArchiveBlUcode_FNPTR(pKernelSec2) pKernelSec2->__ksec2GetBinArchiveBlUcode__
#define ksec2GetBinArchiveBlUcode(pGpu, pKernelSec2) ksec2GetBinArchiveBlUcode_DISPATCH(pGpu, pKernelSec2)
#define ksec2GetBinArchiveBlUcode_HAL(pGpu, pKernelSec2) ksec2GetBinArchiveBlUcode_DISPATCH(pGpu, pKernelSec2)
#define ksec2GetGenericBlUcode_FNPTR(pKernelSec2) pKernelSec2->__ksec2GetGenericBlUcode__
#define ksec2GetGenericBlUcode(pGpu, pKernelSec2, ppDesc, ppImg) ksec2GetGenericBlUcode_DISPATCH(pGpu, pKernelSec2, ppDesc, ppImg)
#define ksec2GetGenericBlUcode_HAL(pGpu, pKernelSec2, ppDesc, ppImg) ksec2GetGenericBlUcode_DISPATCH(pGpu, pKernelSec2, ppDesc, ppImg)
#define ksec2GetBinArchiveSecurescrubUcode_FNPTR(pKernelSec2) pKernelSec2->__ksec2GetBinArchiveSecurescrubUcode__
#define ksec2GetBinArchiveSecurescrubUcode(pGpu, pKernelSec2) ksec2GetBinArchiveSecurescrubUcode_DISPATCH(pGpu, pKernelSec2)
#define ksec2GetBinArchiveSecurescrubUcode_HAL(pGpu, pKernelSec2) ksec2GetBinArchiveSecurescrubUcode_DISPATCH(pGpu, pKernelSec2)
#define ksec2InitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateInitMissing__
#define ksec2InitMissing(pGpu, pEngstate) ksec2InitMissing_DISPATCH(pGpu, pEngstate)
#define ksec2StatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreInitLocked__
#define ksec2StatePreInitLocked(pGpu, pEngstate) ksec2StatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define ksec2StatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreInitUnlocked__
#define ksec2StatePreInitUnlocked(pGpu, pEngstate) ksec2StatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define ksec2StateInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__
#define ksec2StateInitLocked(pGpu, pEngstate) ksec2StateInitLocked_DISPATCH(pGpu, pEngstate)
#define ksec2StateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateInitUnlocked__
#define ksec2StateInitUnlocked(pGpu, pEngstate) ksec2StateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define ksec2StatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreLoad__
#define ksec2StatePreLoad(pGpu, pEngstate, arg3) ksec2StatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define ksec2StatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePostLoad__
#define ksec2StatePostLoad(pGpu, pEngstate, arg3) ksec2StatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define ksec2StatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreUnload__
#define ksec2StatePreUnload(pGpu, pEngstate, arg3) ksec2StatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define ksec2StateUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateUnload__
#define ksec2StateUnload(pGpu, pEngstate, arg3) ksec2StateUnload_DISPATCH(pGpu, pEngstate, arg3)
#define ksec2StatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePostUnload__
#define ksec2StatePostUnload(pGpu, pEngstate, arg3) ksec2StatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define ksec2StateDestroy_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__
#define ksec2StateDestroy(pGpu, pEngstate) ksec2StateDestroy_DISPATCH(pGpu, pEngstate)
#define ksec2IsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateIsPresent__
#define ksec2IsPresent(pGpu, pEngstate) ksec2IsPresent_DISPATCH(pGpu, pEngstate)
#define ksec2ClearInterrupt_FNPTR(pIntrService) pIntrService->__nvoc_base_IntrService.__intrservClearInterrupt__
#define ksec2ClearInterrupt(pGpu, pIntrService, pParams) ksec2ClearInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define ksec2ServiceInterrupt_FNPTR(pIntrService) pIntrService->__nvoc_base_IntrService.__intrservServiceInterrupt__
#define ksec2ServiceInterrupt(pGpu, pIntrService, pParams) ksec2ServiceInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define ksec2RegRead_FNPTR(pKernelFlcn) pKernelFlcn->__nvoc_base_KernelFalcon.__kflcnRegRead__
#define ksec2RegRead(pGpu, pKernelFlcn, offset) ksec2RegRead_DISPATCH(pGpu, pKernelFlcn, offset)
#define ksec2RegRead_HAL(pGpu, pKernelFlcn, offset) ksec2RegRead_DISPATCH(pGpu, pKernelFlcn, offset)
#define ksec2RegWrite_FNPTR(pKernelFlcn) pKernelFlcn->__nvoc_base_KernelFalcon.__kflcnRegWrite__
#define ksec2RegWrite(pGpu, pKernelFlcn, offset, data) ksec2RegWrite_DISPATCH(pGpu, pKernelFlcn, offset, data)
#define ksec2RegWrite_HAL(pGpu, pKernelFlcn, offset, data) ksec2RegWrite_DISPATCH(pGpu, pKernelFlcn, offset, data)
#define ksec2MaskDmemAddr_FNPTR(pKernelFlcn) pKernelFlcn->__nvoc_base_KernelFalcon.__kflcnMaskDmemAddr__
#define ksec2MaskDmemAddr(pGpu, pKernelFlcn, addr) ksec2MaskDmemAddr_DISPATCH(pGpu, pKernelFlcn, addr)
#define ksec2MaskDmemAddr_HAL(pGpu, pKernelFlcn, addr) ksec2MaskDmemAddr_DISPATCH(pGpu, pKernelFlcn, addr)
#define ksec2Configured_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__kcrashcatEngineConfigured__
#define ksec2Configured(arg_this) ksec2Configured_DISPATCH(arg_this)
#define ksec2Unload_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__kcrashcatEngineUnload__
#define ksec2Unload(arg_this) ksec2Unload_DISPATCH(arg_this)
#define ksec2Vprintf_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__kcrashcatEngineVprintf__
#define ksec2Vprintf(arg_this, bReportStart, fmt, args) ksec2Vprintf_DISPATCH(arg_this, bReportStart, fmt, args)
#define ksec2PriRead_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__kcrashcatEnginePriRead__
#define ksec2PriRead(arg_this, offset) ksec2PriRead_DISPATCH(arg_this, offset)
#define ksec2PriWrite_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__kcrashcatEnginePriWrite__
#define ksec2PriWrite(arg_this, offset, data) ksec2PriWrite_DISPATCH(arg_this, offset, data)
#define ksec2MapBufferDescriptor_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__kcrashcatEngineMapBufferDescriptor__
#define ksec2MapBufferDescriptor(arg_this, pBufDesc) ksec2MapBufferDescriptor_DISPATCH(arg_this, pBufDesc)
#define ksec2UnmapBufferDescriptor_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__kcrashcatEngineUnmapBufferDescriptor__
#define ksec2UnmapBufferDescriptor(arg_this, pBufDesc) ksec2UnmapBufferDescriptor_DISPATCH(arg_this, pBufDesc)
#define ksec2SyncBufferDescriptor_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__kcrashcatEngineSyncBufferDescriptor__
#define ksec2SyncBufferDescriptor(arg_this, pBufDesc, offset, size) ksec2SyncBufferDescriptor_DISPATCH(arg_this, pBufDesc, offset, size)
#define ksec2ReadDmem_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__kcrashcatEngineReadDmem__
#define ksec2ReadDmem(arg_this, offset, size, pBuf) ksec2ReadDmem_DISPATCH(arg_this, offset, size, pBuf)
#define ksec2ReadDmem_HAL(arg_this, offset, size, pBuf) ksec2ReadDmem_DISPATCH(arg_this, offset, size, pBuf)
#define ksec2ReadEmem_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__kcrashcatEngineReadEmem__
#define ksec2ReadEmem(arg_this, offset, size, pBuf) ksec2ReadEmem_DISPATCH(arg_this, offset, size, pBuf)
#define ksec2ReadEmem_HAL(arg_this, offset, size, pBuf) ksec2ReadEmem_DISPATCH(arg_this, offset, size, pBuf)
#define ksec2GetScratchOffsets_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__kcrashcatEngineGetScratchOffsets__
#define ksec2GetScratchOffsets(arg_this, scratchGroupId) ksec2GetScratchOffsets_DISPATCH(arg_this, scratchGroupId)
#define ksec2GetScratchOffsets_HAL(arg_this, scratchGroupId) ksec2GetScratchOffsets_DISPATCH(arg_this, scratchGroupId)
#define ksec2GetWFL0Offset_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__kcrashcatEngineGetWFL0Offset__
#define ksec2GetWFL0Offset(arg_this) ksec2GetWFL0Offset_DISPATCH(arg_this)
#define ksec2GetWFL0Offset_HAL(arg_this) ksec2GetWFL0Offset_DISPATCH(arg_this)

// Dispatch functions
static inline NV_STATUS ksec2ConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, ENGDESCRIPTOR arg3) {
    return pKernelSec2->__ksec2ConstructEngine__(pGpu, pKernelSec2, arg3);
}

static inline void ksec2RegisterIntrService_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, IntrServiceRecord pRecords[175]) {
    pKernelSec2->__ksec2RegisterIntrService__(pGpu, pKernelSec2, pRecords);
}

static inline NV_STATUS ksec2ServiceNotificationInterrupt_DISPATCH(struct OBJGPU *arg1, struct KernelSec2 *arg_this, IntrServiceServiceNotificationInterruptArguments *arg3) {
    return arg_this->__ksec2ServiceNotificationInterrupt__(arg1, arg_this, arg3);
}

static inline NV_STATUS ksec2ConfigureFalcon_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2) {
    return pKernelSec2->__ksec2ConfigureFalcon__(pGpu, pKernelSec2);
}

static inline NV_STATUS ksec2ResetHw_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2) {
    return pKernelSec2->__ksec2ResetHw__(pGpu, pKernelSec2);
}

static inline NV_STATUS ksec2StateLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, NvU32 arg3) {
    return pKernelSec2->__ksec2StateLoad__(pGpu, pKernelSec2, arg3);
}

static inline NvU32 ksec2ReadUcodeFuseVersion_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, NvU32 ucodeId) {
    return pKernelSec2->__ksec2ReadUcodeFuseVersion__(pGpu, pKernelSec2, ucodeId);
}

static inline const BINDATA_ARCHIVE * ksec2GetBinArchiveBlUcode_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2) {
    return pKernelSec2->__ksec2GetBinArchiveBlUcode__(pGpu, pKernelSec2);
}

static inline NV_STATUS ksec2GetGenericBlUcode_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, const RM_FLCN_BL_DESC **ppDesc, const NvU8 **ppImg) {
    return pKernelSec2->__ksec2GetGenericBlUcode__(pGpu, pKernelSec2, ppDesc, ppImg);
}

static inline const BINDATA_ARCHIVE * ksec2GetBinArchiveSecurescrubUcode_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2) {
    return pKernelSec2->__ksec2GetBinArchiveSecurescrubUcode__(pGpu, pKernelSec2);
}

static inline void ksec2InitMissing_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pEngstate) {
    pEngstate->__ksec2InitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS ksec2StatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pEngstate) {
    return pEngstate->__ksec2StatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS ksec2StatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pEngstate) {
    return pEngstate->__ksec2StatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS ksec2StateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pEngstate) {
    return pEngstate->__ksec2StateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS ksec2StateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pEngstate) {
    return pEngstate->__ksec2StateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS ksec2StatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pEngstate, NvU32 arg3) {
    return pEngstate->__ksec2StatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS ksec2StatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pEngstate, NvU32 arg3) {
    return pEngstate->__ksec2StatePostLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS ksec2StatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pEngstate, NvU32 arg3) {
    return pEngstate->__ksec2StatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS ksec2StateUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pEngstate, NvU32 arg3) {
    return pEngstate->__ksec2StateUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS ksec2StatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pEngstate, NvU32 arg3) {
    return pEngstate->__ksec2StatePostUnload__(pGpu, pEngstate, arg3);
}

static inline void ksec2StateDestroy_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pEngstate) {
    pEngstate->__ksec2StateDestroy__(pGpu, pEngstate);
}

static inline NvBool ksec2IsPresent_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pEngstate) {
    return pEngstate->__ksec2IsPresent__(pGpu, pEngstate);
}

static inline NvBool ksec2ClearInterrupt_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return pIntrService->__ksec2ClearInterrupt__(pGpu, pIntrService, pParams);
}

static inline NvU32 ksec2ServiceInterrupt_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pIntrService, IntrServiceServiceInterruptArguments *pParams) {
    return pIntrService->__ksec2ServiceInterrupt__(pGpu, pIntrService, pParams);
}

static inline NvU32 ksec2RegRead_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelFlcn, NvU32 offset) {
    return pKernelFlcn->__ksec2RegRead__(pGpu, pKernelFlcn, offset);
}

static inline void ksec2RegWrite_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelFlcn, NvU32 offset, NvU32 data) {
    pKernelFlcn->__ksec2RegWrite__(pGpu, pKernelFlcn, offset, data);
}

static inline NvU32 ksec2MaskDmemAddr_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelFlcn, NvU32 addr) {
    return pKernelFlcn->__ksec2MaskDmemAddr__(pGpu, pKernelFlcn, addr);
}

static inline NvBool ksec2Configured_DISPATCH(struct KernelSec2 *arg_this) {
    return arg_this->__ksec2Configured__(arg_this);
}

static inline void ksec2Unload_DISPATCH(struct KernelSec2 *arg_this) {
    arg_this->__ksec2Unload__(arg_this);
}

static inline void ksec2Vprintf_DISPATCH(struct KernelSec2 *arg_this, NvBool bReportStart, const char *fmt, va_list args) {
    arg_this->__ksec2Vprintf__(arg_this, bReportStart, fmt, args);
}

static inline NvU32 ksec2PriRead_DISPATCH(struct KernelSec2 *arg_this, NvU32 offset) {
    return arg_this->__ksec2PriRead__(arg_this, offset);
}

static inline void ksec2PriWrite_DISPATCH(struct KernelSec2 *arg_this, NvU32 offset, NvU32 data) {
    arg_this->__ksec2PriWrite__(arg_this, offset, data);
}

static inline void * ksec2MapBufferDescriptor_DISPATCH(struct KernelSec2 *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    return arg_this->__ksec2MapBufferDescriptor__(arg_this, pBufDesc);
}

static inline void ksec2UnmapBufferDescriptor_DISPATCH(struct KernelSec2 *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    arg_this->__ksec2UnmapBufferDescriptor__(arg_this, pBufDesc);
}

static inline void ksec2SyncBufferDescriptor_DISPATCH(struct KernelSec2 *arg_this, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size) {
    arg_this->__ksec2SyncBufferDescriptor__(arg_this, pBufDesc, offset, size);
}

static inline void ksec2ReadDmem_DISPATCH(struct KernelSec2 *arg_this, NvU32 offset, NvU32 size, void *pBuf) {
    arg_this->__ksec2ReadDmem__(arg_this, offset, size, pBuf);
}

static inline void ksec2ReadEmem_DISPATCH(struct KernelSec2 *arg_this, NvU64 offset, NvU64 size, void *pBuf) {
    arg_this->__ksec2ReadEmem__(arg_this, offset, size, pBuf);
}

static inline const NvU32 * ksec2GetScratchOffsets_DISPATCH(struct KernelSec2 *arg_this, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId) {
    return arg_this->__ksec2GetScratchOffsets__(arg_this, scratchGroupId);
}

static inline NvU32 ksec2GetWFL0Offset_DISPATCH(struct KernelSec2 *arg_this) {
    return arg_this->__ksec2GetWFL0Offset__(arg_this);
}

NV_STATUS ksec2ConstructEngine_IMPL(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, ENGDESCRIPTOR arg3);

void ksec2RegisterIntrService_IMPL(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, IntrServiceRecord pRecords[175]);

NV_STATUS ksec2ServiceNotificationInterrupt_IMPL(struct OBJGPU *arg1, struct KernelSec2 *arg2, IntrServiceServiceNotificationInterruptArguments *arg3);

NV_STATUS ksec2ConfigureFalcon_TU102(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2);

NV_STATUS ksec2ConfigureFalcon_GA100(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2);

NV_STATUS ksec2ConfigureFalcon_GA102(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2);

NV_STATUS ksec2ResetHw_TU102(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2);

static inline NV_STATUS ksec2ResetHw_5baef9(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS ksec2StateLoad_GH100(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, NvU32 arg3);

static inline NV_STATUS ksec2StateLoad_56cd7a(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, NvU32 arg3) {
    return NV_OK;
}

static inline NvU32 ksec2ReadUcodeFuseVersion_b2b553(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, NvU32 ucodeId) {
    return 0;
}

NvU32 ksec2ReadUcodeFuseVersion_GA100(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, NvU32 ucodeId);

static inline NvU32 ksec2ReadUcodeFuseVersion_474d46(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, NvU32 ucodeId) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

const BINDATA_ARCHIVE *ksec2GetBinArchiveBlUcode_TU102(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2);

static inline const BINDATA_ARCHIVE *ksec2GetBinArchiveBlUcode_80f438(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((void *)0));
}

NV_STATUS ksec2GetGenericBlUcode_TU102(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, const RM_FLCN_BL_DESC **ppDesc, const NvU8 **ppImg);

static inline NV_STATUS ksec2GetGenericBlUcode_5baef9(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, const RM_FLCN_BL_DESC **ppDesc, const NvU8 **ppImg) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

const BINDATA_ARCHIVE *ksec2GetBinArchiveSecurescrubUcode_AD10X(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2);

static inline const BINDATA_ARCHIVE *ksec2GetBinArchiveSecurescrubUcode_80f438(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((void *)0));
}

void ksec2Destruct_IMPL(struct KernelSec2 *pKernelSec2);

#define __nvoc_ksec2Destruct(pKernelSec2) ksec2Destruct_IMPL(pKernelSec2)
#undef PRIVATE_FIELD


#endif  // KERNEL_SEC2_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_SEC2_NVOC_H_
