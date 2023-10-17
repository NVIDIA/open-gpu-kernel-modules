#ifndef _G_KERNEL_SEC2_NVOC_H_
#define _G_KERNEL_SEC2_NVOC_H_
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

#ifdef NVOC_KERNEL_SEC2_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelSec2 {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct IntrService __nvoc_base_IntrService;
    struct KernelFalcon __nvoc_base_KernelFalcon;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct IntrService *__nvoc_pbase_IntrService;
    struct CrashCatEngine *__nvoc_pbase_CrashCatEngine;
    struct KernelCrashCatEngine *__nvoc_pbase_KernelCrashCatEngine;
    struct KernelFalcon *__nvoc_pbase_KernelFalcon;
    struct KernelSec2 *__nvoc_pbase_KernelSec2;
    NV_STATUS (*__ksec2ConstructEngine__)(struct OBJGPU *, struct KernelSec2 *, ENGDESCRIPTOR);
    void (*__ksec2RegisterIntrService__)(struct OBJGPU *, struct KernelSec2 *, IntrServiceRecord *);
    NV_STATUS (*__ksec2ServiceNotificationInterrupt__)(struct OBJGPU *, struct KernelSec2 *, IntrServiceServiceNotificationInterruptArguments *);
    void (*__ksec2ConfigureFalcon__)(struct OBJGPU *, struct KernelSec2 *);
    NV_STATUS (*__ksec2ResetHw__)(struct OBJGPU *, struct KernelSec2 *);
    NV_STATUS (*__ksec2StateLoad__)(struct OBJGPU *, struct KernelSec2 *, NvU32);
    NvU32 (*__ksec2ReadUcodeFuseVersion__)(struct OBJGPU *, struct KernelSec2 *, NvU32);
    const BINDATA_ARCHIVE *(*__ksec2GetBinArchiveBlUcode__)(struct OBJGPU *, struct KernelSec2 *);
    NV_STATUS (*__ksec2GetGenericBlUcode__)(struct OBJGPU *, struct KernelSec2 *, const RM_FLCN_BL_DESC **, const NvU8 **);
    const BINDATA_ARCHIVE *(*__ksec2GetBinArchiveSecurescrubUcode__)(struct OBJGPU *, struct KernelSec2 *);
    NvBool (*__ksec2Configured__)(struct KernelSec2 *);
    NvU32 (*__ksec2PriRead__)(struct KernelSec2 *, NvU32);
    void (*__ksec2RegWrite__)(struct OBJGPU *, struct KernelSec2 *, NvU32, NvU32);
    NvU32 (*__ksec2MaskDmemAddr__)(struct OBJGPU *, struct KernelSec2 *, NvU32);
    void (*__ksec2StateDestroy__)(POBJGPU, struct KernelSec2 *);
    void (*__ksec2Vprintf__)(struct KernelSec2 *, NvBool, const char *, va_list);
    NvBool (*__ksec2ClearInterrupt__)(struct OBJGPU *, struct KernelSec2 *, IntrServiceClearInterruptArguments *);
    void (*__ksec2PriWrite__)(struct KernelSec2 *, NvU32, NvU32);
    void *(*__ksec2MapBufferDescriptor__)(struct KernelSec2 *, CrashCatBufferDescriptor *);
    void (*__ksec2SyncBufferDescriptor__)(struct KernelSec2 *, CrashCatBufferDescriptor *, NvU32, NvU32);
    NvU32 (*__ksec2RegRead__)(struct OBJGPU *, struct KernelSec2 *, NvU32);
    NvBool (*__ksec2IsPresent__)(POBJGPU, struct KernelSec2 *);
    NvU32 (*__ksec2ServiceInterrupt__)(struct OBJGPU *, struct KernelSec2 *, IntrServiceServiceInterruptArguments *);
    void (*__ksec2ReadEmem__)(struct KernelSec2 *, NvU64, NvU64, void *);
    const NvU32 *(*__ksec2GetScratchOffsets__)(struct KernelSec2 *, NV_CRASHCAT_SCRATCH_GROUP_ID);
    void (*__ksec2Unload__)(struct KernelSec2 *);
    NV_STATUS (*__ksec2StateUnload__)(POBJGPU, struct KernelSec2 *, NvU32);
    NvU32 (*__ksec2GetWFL0Offset__)(struct KernelSec2 *);
    NV_STATUS (*__ksec2StateInitLocked__)(POBJGPU, struct KernelSec2 *);
    NV_STATUS (*__ksec2StatePreLoad__)(POBJGPU, struct KernelSec2 *, NvU32);
    NV_STATUS (*__ksec2StatePostUnload__)(POBJGPU, struct KernelSec2 *, NvU32);
    NV_STATUS (*__ksec2StatePreUnload__)(POBJGPU, struct KernelSec2 *, NvU32);
    NV_STATUS (*__ksec2StateInitUnlocked__)(POBJGPU, struct KernelSec2 *);
    void (*__ksec2InitMissing__)(POBJGPU, struct KernelSec2 *);
    NV_STATUS (*__ksec2StatePreInitLocked__)(POBJGPU, struct KernelSec2 *);
    NV_STATUS (*__ksec2StatePreInitUnlocked__)(POBJGPU, struct KernelSec2 *);
    NV_STATUS (*__ksec2StatePostLoad__)(POBJGPU, struct KernelSec2 *, NvU32);
    void (*__ksec2UnmapBufferDescriptor__)(struct KernelSec2 *, CrashCatBufferDescriptor *);
    void (*__ksec2ReadDmem__)(struct KernelSec2 *, NvU32, NvU32, void *);
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

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelSec2;

#define __staticCast_KernelSec2(pThis) \
    ((pThis)->__nvoc_pbase_KernelSec2)

#ifdef __nvoc_kernel_sec2_h_disabled
#define __dynamicCast_KernelSec2(pThis) ((KernelSec2*)NULL)
#else //__nvoc_kernel_sec2_h_disabled
#define __dynamicCast_KernelSec2(pThis) \
    ((KernelSec2*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelSec2)))
#endif //__nvoc_kernel_sec2_h_disabled

#define PDB_PROP_KSEC2_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KSEC2_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelSec2(KernelSec2**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelSec2(KernelSec2**, Dynamic*, NvU32);
#define __objCreate_KernelSec2(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelSec2((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define ksec2ConstructEngine(pGpu, pKernelSec2, arg0) ksec2ConstructEngine_DISPATCH(pGpu, pKernelSec2, arg0)
#define ksec2ConstructEngine_HAL(pGpu, pKernelSec2, arg0) ksec2ConstructEngine_DISPATCH(pGpu, pKernelSec2, arg0)
#define ksec2RegisterIntrService(pGpu, pKernelSec2, pRecords) ksec2RegisterIntrService_DISPATCH(pGpu, pKernelSec2, pRecords)
#define ksec2RegisterIntrService_HAL(pGpu, pKernelSec2, pRecords) ksec2RegisterIntrService_DISPATCH(pGpu, pKernelSec2, pRecords)
#define ksec2ServiceNotificationInterrupt(arg0, arg1, arg2) ksec2ServiceNotificationInterrupt_DISPATCH(arg0, arg1, arg2)
#define ksec2ServiceNotificationInterrupt_HAL(arg0, arg1, arg2) ksec2ServiceNotificationInterrupt_DISPATCH(arg0, arg1, arg2)
#define ksec2ConfigureFalcon(pGpu, pKernelSec2) ksec2ConfigureFalcon_DISPATCH(pGpu, pKernelSec2)
#define ksec2ConfigureFalcon_HAL(pGpu, pKernelSec2) ksec2ConfigureFalcon_DISPATCH(pGpu, pKernelSec2)
#define ksec2ResetHw(pGpu, pKernelSec2) ksec2ResetHw_DISPATCH(pGpu, pKernelSec2)
#define ksec2ResetHw_HAL(pGpu, pKernelSec2) ksec2ResetHw_DISPATCH(pGpu, pKernelSec2)
#define ksec2StateLoad(pGpu, pKernelSec2, arg0) ksec2StateLoad_DISPATCH(pGpu, pKernelSec2, arg0)
#define ksec2StateLoad_HAL(pGpu, pKernelSec2, arg0) ksec2StateLoad_DISPATCH(pGpu, pKernelSec2, arg0)
#define ksec2ReadUcodeFuseVersion(pGpu, pKernelSec2, ucodeId) ksec2ReadUcodeFuseVersion_DISPATCH(pGpu, pKernelSec2, ucodeId)
#define ksec2ReadUcodeFuseVersion_HAL(pGpu, pKernelSec2, ucodeId) ksec2ReadUcodeFuseVersion_DISPATCH(pGpu, pKernelSec2, ucodeId)
#define ksec2GetBinArchiveBlUcode(pGpu, pKernelSec2) ksec2GetBinArchiveBlUcode_DISPATCH(pGpu, pKernelSec2)
#define ksec2GetBinArchiveBlUcode_HAL(pGpu, pKernelSec2) ksec2GetBinArchiveBlUcode_DISPATCH(pGpu, pKernelSec2)
#define ksec2GetGenericBlUcode(pGpu, pKernelSec2, ppDesc, ppImg) ksec2GetGenericBlUcode_DISPATCH(pGpu, pKernelSec2, ppDesc, ppImg)
#define ksec2GetGenericBlUcode_HAL(pGpu, pKernelSec2, ppDesc, ppImg) ksec2GetGenericBlUcode_DISPATCH(pGpu, pKernelSec2, ppDesc, ppImg)
#define ksec2GetBinArchiveSecurescrubUcode(pGpu, pKernelSec2) ksec2GetBinArchiveSecurescrubUcode_DISPATCH(pGpu, pKernelSec2)
#define ksec2GetBinArchiveSecurescrubUcode_HAL(pGpu, pKernelSec2) ksec2GetBinArchiveSecurescrubUcode_DISPATCH(pGpu, pKernelSec2)
#define ksec2Configured(arg0) ksec2Configured_DISPATCH(arg0)
#define ksec2PriRead(arg0, offset) ksec2PriRead_DISPATCH(arg0, offset)
#define ksec2RegWrite(pGpu, pKernelFlcn, offset, data) ksec2RegWrite_DISPATCH(pGpu, pKernelFlcn, offset, data)
#define ksec2MaskDmemAddr(pGpu, pKernelFlcn, addr) ksec2MaskDmemAddr_DISPATCH(pGpu, pKernelFlcn, addr)
#define ksec2StateDestroy(pGpu, pEngstate) ksec2StateDestroy_DISPATCH(pGpu, pEngstate)
#define ksec2Vprintf(arg0, bReportStart, fmt, args) ksec2Vprintf_DISPATCH(arg0, bReportStart, fmt, args)
#define ksec2ClearInterrupt(pGpu, pIntrService, pParams) ksec2ClearInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define ksec2PriWrite(arg0, offset, data) ksec2PriWrite_DISPATCH(arg0, offset, data)
#define ksec2MapBufferDescriptor(arg0, pBufDesc) ksec2MapBufferDescriptor_DISPATCH(arg0, pBufDesc)
#define ksec2SyncBufferDescriptor(arg0, pBufDesc, offset, size) ksec2SyncBufferDescriptor_DISPATCH(arg0, pBufDesc, offset, size)
#define ksec2RegRead(pGpu, pKernelFlcn, offset) ksec2RegRead_DISPATCH(pGpu, pKernelFlcn, offset)
#define ksec2IsPresent(pGpu, pEngstate) ksec2IsPresent_DISPATCH(pGpu, pEngstate)
#define ksec2ServiceInterrupt(pGpu, pIntrService, pParams) ksec2ServiceInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define ksec2ReadEmem(arg0, offset, size, pBuf) ksec2ReadEmem_DISPATCH(arg0, offset, size, pBuf)
#define ksec2GetScratchOffsets(arg0, scratchGroupId) ksec2GetScratchOffsets_DISPATCH(arg0, scratchGroupId)
#define ksec2Unload(arg0) ksec2Unload_DISPATCH(arg0)
#define ksec2StateUnload(pGpu, pEngstate, arg0) ksec2StateUnload_DISPATCH(pGpu, pEngstate, arg0)
#define ksec2GetWFL0Offset(arg0) ksec2GetWFL0Offset_DISPATCH(arg0)
#define ksec2StateInitLocked(pGpu, pEngstate) ksec2StateInitLocked_DISPATCH(pGpu, pEngstate)
#define ksec2StatePreLoad(pGpu, pEngstate, arg0) ksec2StatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define ksec2StatePostUnload(pGpu, pEngstate, arg0) ksec2StatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define ksec2StatePreUnload(pGpu, pEngstate, arg0) ksec2StatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define ksec2StateInitUnlocked(pGpu, pEngstate) ksec2StateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define ksec2InitMissing(pGpu, pEngstate) ksec2InitMissing_DISPATCH(pGpu, pEngstate)
#define ksec2StatePreInitLocked(pGpu, pEngstate) ksec2StatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define ksec2StatePreInitUnlocked(pGpu, pEngstate) ksec2StatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define ksec2StatePostLoad(pGpu, pEngstate, arg0) ksec2StatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define ksec2UnmapBufferDescriptor(arg0, pBufDesc) ksec2UnmapBufferDescriptor_DISPATCH(arg0, pBufDesc)
#define ksec2ReadDmem(arg0, offset, size, pBuf) ksec2ReadDmem_DISPATCH(arg0, offset, size, pBuf)
NV_STATUS ksec2ConstructEngine_IMPL(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, ENGDESCRIPTOR arg0);

static inline NV_STATUS ksec2ConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, ENGDESCRIPTOR arg0) {
    return pKernelSec2->__ksec2ConstructEngine__(pGpu, pKernelSec2, arg0);
}

void ksec2RegisterIntrService_IMPL(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, IntrServiceRecord pRecords[168]);

static inline void ksec2RegisterIntrService_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, IntrServiceRecord pRecords[168]) {
    pKernelSec2->__ksec2RegisterIntrService__(pGpu, pKernelSec2, pRecords);
}

NV_STATUS ksec2ServiceNotificationInterrupt_IMPL(struct OBJGPU *arg0, struct KernelSec2 *arg1, IntrServiceServiceNotificationInterruptArguments *arg2);

static inline NV_STATUS ksec2ServiceNotificationInterrupt_DISPATCH(struct OBJGPU *arg0, struct KernelSec2 *arg1, IntrServiceServiceNotificationInterruptArguments *arg2) {
    return arg1->__ksec2ServiceNotificationInterrupt__(arg0, arg1, arg2);
}

void ksec2ConfigureFalcon_TU102(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2);

void ksec2ConfigureFalcon_GA100(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2);

void ksec2ConfigureFalcon_GA102(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2);

static inline void ksec2ConfigureFalcon_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2) {
    pKernelSec2->__ksec2ConfigureFalcon__(pGpu, pKernelSec2);
}

NV_STATUS ksec2ResetHw_TU102(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2);

static inline NV_STATUS ksec2ResetHw_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2) {
    return pKernelSec2->__ksec2ResetHw__(pGpu, pKernelSec2);
}

NV_STATUS ksec2StateLoad_GH100(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, NvU32 arg0);

static inline NV_STATUS ksec2StateLoad_56cd7a(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, NvU32 arg0) {
    return NV_OK;
}

static inline NV_STATUS ksec2StateLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, NvU32 arg0) {
    return pKernelSec2->__ksec2StateLoad__(pGpu, pKernelSec2, arg0);
}

static inline NvU32 ksec2ReadUcodeFuseVersion_b2b553(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, NvU32 ucodeId) {
    return 0;
}

NvU32 ksec2ReadUcodeFuseVersion_GA100(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, NvU32 ucodeId);

static inline NvU32 ksec2ReadUcodeFuseVersion_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, NvU32 ucodeId) {
    return pKernelSec2->__ksec2ReadUcodeFuseVersion__(pGpu, pKernelSec2, ucodeId);
}

const BINDATA_ARCHIVE *ksec2GetBinArchiveBlUcode_TU102(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2);

static inline const BINDATA_ARCHIVE *ksec2GetBinArchiveBlUcode_80f438(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((void *)0));
}

static inline const BINDATA_ARCHIVE *ksec2GetBinArchiveBlUcode_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2) {
    return pKernelSec2->__ksec2GetBinArchiveBlUcode__(pGpu, pKernelSec2);
}

NV_STATUS ksec2GetGenericBlUcode_TU102(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, const RM_FLCN_BL_DESC **ppDesc, const NvU8 **ppImg);

static inline NV_STATUS ksec2GetGenericBlUcode_5baef9(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, const RM_FLCN_BL_DESC **ppDesc, const NvU8 **ppImg) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

static inline NV_STATUS ksec2GetGenericBlUcode_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, const RM_FLCN_BL_DESC **ppDesc, const NvU8 **ppImg) {
    return pKernelSec2->__ksec2GetGenericBlUcode__(pGpu, pKernelSec2, ppDesc, ppImg);
}

const BINDATA_ARCHIVE *ksec2GetBinArchiveSecurescrubUcode_AD10X(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2);

static inline const BINDATA_ARCHIVE *ksec2GetBinArchiveSecurescrubUcode_80f438(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((void *)0));
}

static inline const BINDATA_ARCHIVE *ksec2GetBinArchiveSecurescrubUcode_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2) {
    return pKernelSec2->__ksec2GetBinArchiveSecurescrubUcode__(pGpu, pKernelSec2);
}

static inline NvBool ksec2Configured_DISPATCH(struct KernelSec2 *arg0) {
    return arg0->__ksec2Configured__(arg0);
}

static inline NvU32 ksec2PriRead_DISPATCH(struct KernelSec2 *arg0, NvU32 offset) {
    return arg0->__ksec2PriRead__(arg0, offset);
}

static inline void ksec2RegWrite_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelFlcn, NvU32 offset, NvU32 data) {
    pKernelFlcn->__ksec2RegWrite__(pGpu, pKernelFlcn, offset, data);
}

static inline NvU32 ksec2MaskDmemAddr_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelFlcn, NvU32 addr) {
    return pKernelFlcn->__ksec2MaskDmemAddr__(pGpu, pKernelFlcn, addr);
}

static inline void ksec2StateDestroy_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    pEngstate->__ksec2StateDestroy__(pGpu, pEngstate);
}

static inline void ksec2Vprintf_DISPATCH(struct KernelSec2 *arg0, NvBool bReportStart, const char *fmt, va_list args) {
    arg0->__ksec2Vprintf__(arg0, bReportStart, fmt, args);
}

static inline NvBool ksec2ClearInterrupt_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return pIntrService->__ksec2ClearInterrupt__(pGpu, pIntrService, pParams);
}

static inline void ksec2PriWrite_DISPATCH(struct KernelSec2 *arg0, NvU32 offset, NvU32 data) {
    arg0->__ksec2PriWrite__(arg0, offset, data);
}

static inline void *ksec2MapBufferDescriptor_DISPATCH(struct KernelSec2 *arg0, CrashCatBufferDescriptor *pBufDesc) {
    return arg0->__ksec2MapBufferDescriptor__(arg0, pBufDesc);
}

static inline void ksec2SyncBufferDescriptor_DISPATCH(struct KernelSec2 *arg0, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size) {
    arg0->__ksec2SyncBufferDescriptor__(arg0, pBufDesc, offset, size);
}

static inline NvU32 ksec2RegRead_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelFlcn, NvU32 offset) {
    return pKernelFlcn->__ksec2RegRead__(pGpu, pKernelFlcn, offset);
}

static inline NvBool ksec2IsPresent_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    return pEngstate->__ksec2IsPresent__(pGpu, pEngstate);
}

static inline NvU32 ksec2ServiceInterrupt_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pIntrService, IntrServiceServiceInterruptArguments *pParams) {
    return pIntrService->__ksec2ServiceInterrupt__(pGpu, pIntrService, pParams);
}

static inline void ksec2ReadEmem_DISPATCH(struct KernelSec2 *arg0, NvU64 offset, NvU64 size, void *pBuf) {
    arg0->__ksec2ReadEmem__(arg0, offset, size, pBuf);
}

static inline const NvU32 *ksec2GetScratchOffsets_DISPATCH(struct KernelSec2 *arg0, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId) {
    return arg0->__ksec2GetScratchOffsets__(arg0, scratchGroupId);
}

static inline void ksec2Unload_DISPATCH(struct KernelSec2 *arg0) {
    arg0->__ksec2Unload__(arg0);
}

static inline NV_STATUS ksec2StateUnload_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate, NvU32 arg0) {
    return pEngstate->__ksec2StateUnload__(pGpu, pEngstate, arg0);
}

static inline NvU32 ksec2GetWFL0Offset_DISPATCH(struct KernelSec2 *arg0) {
    return arg0->__ksec2GetWFL0Offset__(arg0);
}

static inline NV_STATUS ksec2StateInitLocked_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    return pEngstate->__ksec2StateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS ksec2StatePreLoad_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate, NvU32 arg0) {
    return pEngstate->__ksec2StatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS ksec2StatePostUnload_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate, NvU32 arg0) {
    return pEngstate->__ksec2StatePostUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS ksec2StatePreUnload_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate, NvU32 arg0) {
    return pEngstate->__ksec2StatePreUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS ksec2StateInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    return pEngstate->__ksec2StateInitUnlocked__(pGpu, pEngstate);
}

static inline void ksec2InitMissing_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    pEngstate->__ksec2InitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS ksec2StatePreInitLocked_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    return pEngstate->__ksec2StatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS ksec2StatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    return pEngstate->__ksec2StatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS ksec2StatePostLoad_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate, NvU32 arg0) {
    return pEngstate->__ksec2StatePostLoad__(pGpu, pEngstate, arg0);
}

static inline void ksec2UnmapBufferDescriptor_DISPATCH(struct KernelSec2 *arg0, CrashCatBufferDescriptor *pBufDesc) {
    arg0->__ksec2UnmapBufferDescriptor__(arg0, pBufDesc);
}

static inline void ksec2ReadDmem_DISPATCH(struct KernelSec2 *arg0, NvU32 offset, NvU32 size, void *pBuf) {
    arg0->__ksec2ReadDmem__(arg0, offset, size, pBuf);
}

void ksec2Destruct_IMPL(struct KernelSec2 *pKernelSec2);

#define __nvoc_ksec2Destruct(pKernelSec2) ksec2Destruct_IMPL(pKernelSec2)
#undef PRIVATE_FIELD


#endif  // KERNEL_SEC2_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_SEC2_NVOC_H_
