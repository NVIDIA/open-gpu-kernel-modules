
#ifndef _G_KERN_FSP_NVOC_H_
#define _G_KERN_FSP_NVOC_H_
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
#include "g_kern_fsp_nvoc.h"

#ifndef KERN_FSP_H
#define KERN_FSP_H

#include "core/core.h"
#include "gpu/eng_state.h"
#include "gpu/timer/objtmr.h"

#include "kernel/gpu/mem_mgr/mem_desc.h"
#include "kernel/gpu/gpu_halspec.h"
#include "nvoc/utility.h"

#pragma pack(1)
typedef struct
{
    NvU32 constBlob;
    NvU8 msgType;
    NvU16 vendorId;
} MCTP_HEADER;

// Needed to remove unnecessary padding
#pragma pack(1)
typedef struct
{
    NvU16 version;
    NvU16 size;
    NvU64 gspFmcSysmemOffset;
    NvU64 frtsSysmemOffset;
    NvU32 frtsSysmemSize;

    // Note this is an offset from the end of FB
    NvU64 frtsVidmemOffset;
    NvU32 frtsVidmemSize;

    // Authentication related fields
    NvU32 hash384[12];
    NvU32 publicKey[96];
    NvU32 signature[96];

    NvU64 gspBootArgsSysmemOffset;
} NVDM_PAYLOAD_COT;
#pragma pack()

// State required to handle the async FSP RPC response
typedef void (*AsyncRpcCallback)(NV_STATUS status, void *pArgs);
typedef struct
{
    AsyncRpcCallback  callback;
    void             *pCallbackArgs;
    NvU8             *pResponseBuffer;
    NvU32             responseBufferSize;
} FSP_RPC_STATE;

// Type of packet, can either be SOM, EOM, neither, or both (1-packet messages)
typedef enum
{
    MCTP_PACKET_STATE_START,
    MCTP_PACKET_STATE_INTERMEDIATE,
    MCTP_PACKET_STATE_END,
    MCTP_PACKET_STATE_SINGLE_PACKET
} MCTP_PACKET_STATE;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERN_FSP_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct KernelFsp {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct KernelFsp *__nvoc_pbase_KernelFsp;    // kfsp

    // Vtable with 44 per-object function pointers
    NV_STATUS (*__kfspConstructEngine__)(OBJGPU *, struct KernelFsp * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    void (*__kfspStateDestroy__)(OBJGPU *, struct KernelFsp * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kfspSendAndReadMessageAsync__)(OBJGPU *, struct KernelFsp * /*this*/, NvU8 *, NvU32, NvU32, NvU8 *, NvU32, AsyncRpcCallback, void *);  // virtual
    NvBool (*__kfspGspFmcIsEnforced__)(OBJGPU *, struct KernelFsp * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kfspPrepareBootCommands__)(OBJGPU *, struct KernelFsp * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kfspSendBootCommands__)(OBJGPU *, struct KernelFsp * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kfspPrepareAndSendBootCommands__)(OBJGPU *, struct KernelFsp * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kfspWaitForSecureBoot__)(OBJGPU *, struct KernelFsp * /*this*/);  // halified (3 hals) body
    NvU32 (*__kfspGetRmChannelSize__)(OBJGPU *, struct KernelFsp * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kfspConfigEmemc__)(OBJGPU *, struct KernelFsp * /*this*/, NvU32, NvBool, NvBool);  // halified (2 hals) body
    void (*__kfspUpdateQueueHeadTail__)(OBJGPU *, struct KernelFsp * /*this*/, NvU32, NvU32);  // halified (2 hals) body
    void (*__kfspGetQueueHeadTail__)(OBJGPU *, struct KernelFsp * /*this*/, NvU32 *, NvU32 *);  // halified (2 hals) body
    void (*__kfspUpdateMsgQueueHeadTail__)(OBJGPU *, struct KernelFsp * /*this*/, NvU32, NvU32);  // halified (2 hals) body
    void (*__kfspGetMsgQueueHeadTail__)(OBJGPU *, struct KernelFsp * /*this*/, NvU32 *, NvU32 *);  // halified (2 hals) body
    NvU8 (*__kfspNvdmToSeid__)(OBJGPU *, struct KernelFsp * /*this*/, NvU8);  // halified (2 hals) body
    NvU32 (*__kfspCreateMctpHeader__)(OBJGPU *, struct KernelFsp * /*this*/, NvU8, NvU8, NvU8, NvU8);  // halified (2 hals) body
    NvU32 (*__kfspCreateNvdmHeader__)(OBJGPU *, struct KernelFsp * /*this*/, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kfspWriteToEmem__)(OBJGPU *, struct KernelFsp * /*this*/, NvU8 *, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kfspReadFromEmem__)(OBJGPU *, struct KernelFsp * /*this*/, NvU8 *, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kfspGetPacketInfo__)(OBJGPU *, struct KernelFsp * /*this*/, NvU8 *, NvU32, MCTP_PACKET_STATE *, NvU8 *);  // halified (2 hals) body
    NV_STATUS (*__kfspValidateMctpPayloadHeader__)(OBJGPU *, struct KernelFsp * /*this*/, NvU8 *, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kfspProcessNvdmMessage__)(OBJGPU *, struct KernelFsp * /*this*/, NvU8 *, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kfspProcessCommandResponse__)(OBJGPU *, struct KernelFsp * /*this*/, NvU8 *, NvU32);  // halified (2 hals) body
    void (*__kfspDumpDebugState__)(OBJGPU *, struct KernelFsp * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kfspErrorCode2NvStatusMap__)(OBJGPU *, struct KernelFsp * /*this*/, NvU32);  // halified (2 hals) body
    NvU64 (*__kfspGetExtraReservedMemorySize__)(OBJGPU *, struct KernelFsp * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kfspWaitForGspTargetMaskReleased__)(OBJGPU *, struct KernelFsp * /*this*/);  // halified (2 hals) body
    NvBool (*__kfspRequiresBug3957833WAR__)(OBJGPU *, struct KernelFsp * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kfspFrtsSysmemLocationProgram__)(OBJGPU *, struct KernelFsp * /*this*/);  // halified (2 hals) body
    void (*__kfspFrtsSysmemLocationClear__)(OBJGPU *, struct KernelFsp * /*this*/);  // halified (2 hals) body
    void (*__kfspCheckForClockBoostCapability__)(OBJGPU *, struct KernelFsp * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kfspSendClockBoostRpc__)(OBJGPU *, struct KernelFsp * /*this*/, NvU8);  // halified (2 hals) body
    void (*__kfspInitMissing__)(struct OBJGPU *, struct KernelFsp * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kfspStatePreInitLocked__)(struct OBJGPU *, struct KernelFsp * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kfspStatePreInitUnlocked__)(struct OBJGPU *, struct KernelFsp * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kfspStateInitLocked__)(struct OBJGPU *, struct KernelFsp * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kfspStateInitUnlocked__)(struct OBJGPU *, struct KernelFsp * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kfspStatePreLoad__)(struct OBJGPU *, struct KernelFsp * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kfspStateLoad__)(struct OBJGPU *, struct KernelFsp * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kfspStatePostLoad__)(struct OBJGPU *, struct KernelFsp * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kfspStatePreUnload__)(struct OBJGPU *, struct KernelFsp * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kfspStateUnload__)(struct OBJGPU *, struct KernelFsp * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kfspStatePostUnload__)(struct OBJGPU *, struct KernelFsp * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NvBool (*__kfspIsPresent__)(struct OBJGPU *, struct KernelFsp * /*this*/);  // virtual inherited (engstate) base (engstate)

    // 7 PDB properties
    NvBool PDB_PROP_KFSP_FSP_FUSE_ERROR_CHECK_ENABLED;
    NvBool PDB_PROP_KFSP_BOOT_COMMAND_OK;
    NvBool PDB_PROP_KFSP_GSP_MODE_GSPRM;
    NvBool PDB_PROP_KFSP_DISABLE_FRTS_SYSMEM;
    NvBool PDB_PROP_KFSP_DISABLE_FRTS_VIDMEM;
    NvBool PDB_PROP_KFSP_DISABLE_GSPFMC;

    // Data members
    MEMORY_DESCRIPTOR *pSysmemFrtsMemdesc;
    MEMORY_DESCRIPTOR *pVidmemFrtsMemdesc;
    MEMORY_DESCRIPTOR *pGspFmcMemdesc;
    MEMORY_DESCRIPTOR *pGspBootArgsMemdesc;
    NVDM_PAYLOAD_COT *pCotPayload;
    NvU32 cotPayloadSignatureSize;
    NvU32 cotPayloadPublicKeySize;
    NvU32 cotPayloadVersion;
    NvBool bGspDebugBufferInitialized;
    NvBool bBusy;
    struct TMR_EVENT *pPollEvent;
    RMTIMEOUT rpcTimeout;
    FSP_RPC_STATE rpcState;
    NvBool bClockBoostSupported;
    NvBool bClockBoostDisabledViaRegkey;
};

#ifndef __NVOC_CLASS_KernelFsp_TYPEDEF__
#define __NVOC_CLASS_KernelFsp_TYPEDEF__
typedef struct KernelFsp KernelFsp;
#endif /* __NVOC_CLASS_KernelFsp_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelFsp
#define __nvoc_class_id_KernelFsp 0x87fb96
#endif /* __nvoc_class_id_KernelFsp */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFsp;

#define __staticCast_KernelFsp(pThis) \
    ((pThis)->__nvoc_pbase_KernelFsp)

#ifdef __nvoc_kern_fsp_h_disabled
#define __dynamicCast_KernelFsp(pThis) ((KernelFsp*)NULL)
#else //__nvoc_kern_fsp_h_disabled
#define __dynamicCast_KernelFsp(pThis) \
    ((KernelFsp*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelFsp)))
#endif //__nvoc_kern_fsp_h_disabled

// Property macros
#define PDB_PROP_KFSP_DISABLE_FRTS_VIDMEM_BASE_CAST
#define PDB_PROP_KFSP_DISABLE_FRTS_VIDMEM_BASE_NAME PDB_PROP_KFSP_DISABLE_FRTS_VIDMEM
#define PDB_PROP_KFSP_DISABLE_GSPFMC_BASE_CAST
#define PDB_PROP_KFSP_DISABLE_GSPFMC_BASE_NAME PDB_PROP_KFSP_DISABLE_GSPFMC
#define PDB_PROP_KFSP_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KFSP_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING
#define PDB_PROP_KFSP_GSP_MODE_GSPRM_BASE_CAST
#define PDB_PROP_KFSP_GSP_MODE_GSPRM_BASE_NAME PDB_PROP_KFSP_GSP_MODE_GSPRM
#define PDB_PROP_KFSP_FSP_FUSE_ERROR_CHECK_ENABLED_BASE_CAST
#define PDB_PROP_KFSP_FSP_FUSE_ERROR_CHECK_ENABLED_BASE_NAME PDB_PROP_KFSP_FSP_FUSE_ERROR_CHECK_ENABLED
#define PDB_PROP_KFSP_BOOT_COMMAND_OK_BASE_CAST
#define PDB_PROP_KFSP_BOOT_COMMAND_OK_BASE_NAME PDB_PROP_KFSP_BOOT_COMMAND_OK
#define PDB_PROP_KFSP_DISABLE_FRTS_SYSMEM_BASE_CAST
#define PDB_PROP_KFSP_DISABLE_FRTS_SYSMEM_BASE_NAME PDB_PROP_KFSP_DISABLE_FRTS_SYSMEM

NV_STATUS __nvoc_objCreateDynamic_KernelFsp(KernelFsp**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelFsp(KernelFsp**, Dynamic*, NvU32);
#define __objCreate_KernelFsp(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelFsp((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define kfspConstructEngine_FNPTR(pKernelFsp) pKernelFsp->__kfspConstructEngine__
#define kfspConstructEngine(pGpu, pKernelFsp, arg3) kfspConstructEngine_DISPATCH(pGpu, pKernelFsp, arg3)
#define kfspStateDestroy_FNPTR(pKernelFsp) pKernelFsp->__kfspStateDestroy__
#define kfspStateDestroy(pGpu, pKernelFsp) kfspStateDestroy_DISPATCH(pGpu, pKernelFsp)
#define kfspSendAndReadMessageAsync_FNPTR(pKernelFsp) pKernelFsp->__kfspSendAndReadMessageAsync__
#define kfspSendAndReadMessageAsync(pGpu, pKernelFsp, pPayload, size, nvdmType, pResponsePayload, responseBufferSize, callback, pCallbackArgs) kfspSendAndReadMessageAsync_DISPATCH(pGpu, pKernelFsp, pPayload, size, nvdmType, pResponsePayload, responseBufferSize, callback, pCallbackArgs)
#define kfspGspFmcIsEnforced_FNPTR(pKernelFsp) pKernelFsp->__kfspGspFmcIsEnforced__
#define kfspGspFmcIsEnforced(pGpu, pKernelFsp) kfspGspFmcIsEnforced_DISPATCH(pGpu, pKernelFsp)
#define kfspGspFmcIsEnforced_HAL(pGpu, pKernelFsp) kfspGspFmcIsEnforced_DISPATCH(pGpu, pKernelFsp)
#define kfspPrepareBootCommands_FNPTR(pKernelFsp) pKernelFsp->__kfspPrepareBootCommands__
#define kfspPrepareBootCommands(pGpu, pKernelFsp) kfspPrepareBootCommands_DISPATCH(pGpu, pKernelFsp)
#define kfspPrepareBootCommands_HAL(pGpu, pKernelFsp) kfspPrepareBootCommands_DISPATCH(pGpu, pKernelFsp)
#define kfspSendBootCommands_FNPTR(pKernelFsp) pKernelFsp->__kfspSendBootCommands__
#define kfspSendBootCommands(pGpu, pKernelFsp) kfspSendBootCommands_DISPATCH(pGpu, pKernelFsp)
#define kfspSendBootCommands_HAL(pGpu, pKernelFsp) kfspSendBootCommands_DISPATCH(pGpu, pKernelFsp)
#define kfspPrepareAndSendBootCommands_FNPTR(pKernelFsp) pKernelFsp->__kfspPrepareAndSendBootCommands__
#define kfspPrepareAndSendBootCommands(pGpu, pKernelFsp) kfspPrepareAndSendBootCommands_DISPATCH(pGpu, pKernelFsp)
#define kfspPrepareAndSendBootCommands_HAL(pGpu, pKernelFsp) kfspPrepareAndSendBootCommands_DISPATCH(pGpu, pKernelFsp)
#define kfspWaitForSecureBoot_FNPTR(pKernelFsp) pKernelFsp->__kfspWaitForSecureBoot__
#define kfspWaitForSecureBoot(pGpu, pKernelFsp) kfspWaitForSecureBoot_DISPATCH(pGpu, pKernelFsp)
#define kfspWaitForSecureBoot_HAL(pGpu, pKernelFsp) kfspWaitForSecureBoot_DISPATCH(pGpu, pKernelFsp)
#define kfspGetRmChannelSize_FNPTR(pKernelFsp) pKernelFsp->__kfspGetRmChannelSize__
#define kfspGetRmChannelSize(pGpu, pKernelFsp) kfspGetRmChannelSize_DISPATCH(pGpu, pKernelFsp)
#define kfspGetRmChannelSize_HAL(pGpu, pKernelFsp) kfspGetRmChannelSize_DISPATCH(pGpu, pKernelFsp)
#define kfspConfigEmemc_FNPTR(pKernelFsp) pKernelFsp->__kfspConfigEmemc__
#define kfspConfigEmemc(pGpu, pKernelFsp, offset, bAincw, bAincr) kfspConfigEmemc_DISPATCH(pGpu, pKernelFsp, offset, bAincw, bAincr)
#define kfspConfigEmemc_HAL(pGpu, pKernelFsp, offset, bAincw, bAincr) kfspConfigEmemc_DISPATCH(pGpu, pKernelFsp, offset, bAincw, bAincr)
#define kfspUpdateQueueHeadTail_FNPTR(pKernelFsp) pKernelFsp->__kfspUpdateQueueHeadTail__
#define kfspUpdateQueueHeadTail(pGpu, pKernelFsp, queueHead, queueTail) kfspUpdateQueueHeadTail_DISPATCH(pGpu, pKernelFsp, queueHead, queueTail)
#define kfspUpdateQueueHeadTail_HAL(pGpu, pKernelFsp, queueHead, queueTail) kfspUpdateQueueHeadTail_DISPATCH(pGpu, pKernelFsp, queueHead, queueTail)
#define kfspGetQueueHeadTail_FNPTR(pKernelFsp) pKernelFsp->__kfspGetQueueHeadTail__
#define kfspGetQueueHeadTail(pGpu, pKernelFsp, pQueueHead, pQueueTail) kfspGetQueueHeadTail_DISPATCH(pGpu, pKernelFsp, pQueueHead, pQueueTail)
#define kfspGetQueueHeadTail_HAL(pGpu, pKernelFsp, pQueueHead, pQueueTail) kfspGetQueueHeadTail_DISPATCH(pGpu, pKernelFsp, pQueueHead, pQueueTail)
#define kfspUpdateMsgQueueHeadTail_FNPTR(pKernelFsp) pKernelFsp->__kfspUpdateMsgQueueHeadTail__
#define kfspUpdateMsgQueueHeadTail(pGpu, pKernelFsp, msgqHead, msgqTail) kfspUpdateMsgQueueHeadTail_DISPATCH(pGpu, pKernelFsp, msgqHead, msgqTail)
#define kfspUpdateMsgQueueHeadTail_HAL(pGpu, pKernelFsp, msgqHead, msgqTail) kfspUpdateMsgQueueHeadTail_DISPATCH(pGpu, pKernelFsp, msgqHead, msgqTail)
#define kfspGetMsgQueueHeadTail_FNPTR(pKernelFsp) pKernelFsp->__kfspGetMsgQueueHeadTail__
#define kfspGetMsgQueueHeadTail(pGpu, pKernelFsp, pMsgqHead, pMsgqTail) kfspGetMsgQueueHeadTail_DISPATCH(pGpu, pKernelFsp, pMsgqHead, pMsgqTail)
#define kfspGetMsgQueueHeadTail_HAL(pGpu, pKernelFsp, pMsgqHead, pMsgqTail) kfspGetMsgQueueHeadTail_DISPATCH(pGpu, pKernelFsp, pMsgqHead, pMsgqTail)
#define kfspNvdmToSeid_FNPTR(pKernelFsp) pKernelFsp->__kfspNvdmToSeid__
#define kfspNvdmToSeid(pGpu, pKernelFsp, nvdmType) kfspNvdmToSeid_DISPATCH(pGpu, pKernelFsp, nvdmType)
#define kfspNvdmToSeid_HAL(pGpu, pKernelFsp, nvdmType) kfspNvdmToSeid_DISPATCH(pGpu, pKernelFsp, nvdmType)
#define kfspCreateMctpHeader_FNPTR(pKernelFsp) pKernelFsp->__kfspCreateMctpHeader__
#define kfspCreateMctpHeader(pGpu, pKernelFsp, som, eom, seid, seq) kfspCreateMctpHeader_DISPATCH(pGpu, pKernelFsp, som, eom, seid, seq)
#define kfspCreateMctpHeader_HAL(pGpu, pKernelFsp, som, eom, seid, seq) kfspCreateMctpHeader_DISPATCH(pGpu, pKernelFsp, som, eom, seid, seq)
#define kfspCreateNvdmHeader_FNPTR(pKernelFsp) pKernelFsp->__kfspCreateNvdmHeader__
#define kfspCreateNvdmHeader(pGpu, pKernelFsp, nvdmType) kfspCreateNvdmHeader_DISPATCH(pGpu, pKernelFsp, nvdmType)
#define kfspCreateNvdmHeader_HAL(pGpu, pKernelFsp, nvdmType) kfspCreateNvdmHeader_DISPATCH(pGpu, pKernelFsp, nvdmType)
#define kfspWriteToEmem_FNPTR(pKernelFsp) pKernelFsp->__kfspWriteToEmem__
#define kfspWriteToEmem(pGpu, pKernelFsp, pBuffer, size) kfspWriteToEmem_DISPATCH(pGpu, pKernelFsp, pBuffer, size)
#define kfspWriteToEmem_HAL(pGpu, pKernelFsp, pBuffer, size) kfspWriteToEmem_DISPATCH(pGpu, pKernelFsp, pBuffer, size)
#define kfspReadFromEmem_FNPTR(pKernelFsp) pKernelFsp->__kfspReadFromEmem__
#define kfspReadFromEmem(pGpu, pKernelFsp, pBuffer, size) kfspReadFromEmem_DISPATCH(pGpu, pKernelFsp, pBuffer, size)
#define kfspReadFromEmem_HAL(pGpu, pKernelFsp, pBuffer, size) kfspReadFromEmem_DISPATCH(pGpu, pKernelFsp, pBuffer, size)
#define kfspGetPacketInfo_FNPTR(pKernelFsp) pKernelFsp->__kfspGetPacketInfo__
#define kfspGetPacketInfo(pGpu, pKernelFsp, pBuffer, size, pPacketState, pTag) kfspGetPacketInfo_DISPATCH(pGpu, pKernelFsp, pBuffer, size, pPacketState, pTag)
#define kfspGetPacketInfo_HAL(pGpu, pKernelFsp, pBuffer, size, pPacketState, pTag) kfspGetPacketInfo_DISPATCH(pGpu, pKernelFsp, pBuffer, size, pPacketState, pTag)
#define kfspValidateMctpPayloadHeader_FNPTR(pKernelFsp) pKernelFsp->__kfspValidateMctpPayloadHeader__
#define kfspValidateMctpPayloadHeader(pGpu, pKernelFsp, pBuffer, size) kfspValidateMctpPayloadHeader_DISPATCH(pGpu, pKernelFsp, pBuffer, size)
#define kfspValidateMctpPayloadHeader_HAL(pGpu, pKernelFsp, pBuffer, size) kfspValidateMctpPayloadHeader_DISPATCH(pGpu, pKernelFsp, pBuffer, size)
#define kfspProcessNvdmMessage_FNPTR(pKernelFsp) pKernelFsp->__kfspProcessNvdmMessage__
#define kfspProcessNvdmMessage(pGpu, pKernelFsp, pBuffer, size) kfspProcessNvdmMessage_DISPATCH(pGpu, pKernelFsp, pBuffer, size)
#define kfspProcessNvdmMessage_HAL(pGpu, pKernelFsp, pBuffer, size) kfspProcessNvdmMessage_DISPATCH(pGpu, pKernelFsp, pBuffer, size)
#define kfspProcessCommandResponse_FNPTR(pKernelFsp) pKernelFsp->__kfspProcessCommandResponse__
#define kfspProcessCommandResponse(pGpu, pKernelFsp, pBuffer, size) kfspProcessCommandResponse_DISPATCH(pGpu, pKernelFsp, pBuffer, size)
#define kfspProcessCommandResponse_HAL(pGpu, pKernelFsp, pBuffer, size) kfspProcessCommandResponse_DISPATCH(pGpu, pKernelFsp, pBuffer, size)
#define kfspDumpDebugState_FNPTR(pKernelFsp) pKernelFsp->__kfspDumpDebugState__
#define kfspDumpDebugState(pGpu, pKernelFsp) kfspDumpDebugState_DISPATCH(pGpu, pKernelFsp)
#define kfspDumpDebugState_HAL(pGpu, pKernelFsp) kfspDumpDebugState_DISPATCH(pGpu, pKernelFsp)
#define kfspErrorCode2NvStatusMap_FNPTR(pKernelFsp) pKernelFsp->__kfspErrorCode2NvStatusMap__
#define kfspErrorCode2NvStatusMap(pGpu, pKernelFsp, errorCode) kfspErrorCode2NvStatusMap_DISPATCH(pGpu, pKernelFsp, errorCode)
#define kfspErrorCode2NvStatusMap_HAL(pGpu, pKernelFsp, errorCode) kfspErrorCode2NvStatusMap_DISPATCH(pGpu, pKernelFsp, errorCode)
#define kfspGetExtraReservedMemorySize_FNPTR(pKernelFsp) pKernelFsp->__kfspGetExtraReservedMemorySize__
#define kfspGetExtraReservedMemorySize(pGpu, pKernelFsp) kfspGetExtraReservedMemorySize_DISPATCH(pGpu, pKernelFsp)
#define kfspGetExtraReservedMemorySize_HAL(pGpu, pKernelFsp) kfspGetExtraReservedMemorySize_DISPATCH(pGpu, pKernelFsp)
#define kfspWaitForGspTargetMaskReleased_FNPTR(pKernelFsp) pKernelFsp->__kfspWaitForGspTargetMaskReleased__
#define kfspWaitForGspTargetMaskReleased(pGpu, pKernelFsp) kfspWaitForGspTargetMaskReleased_DISPATCH(pGpu, pKernelFsp)
#define kfspWaitForGspTargetMaskReleased_HAL(pGpu, pKernelFsp) kfspWaitForGspTargetMaskReleased_DISPATCH(pGpu, pKernelFsp)
#define kfspRequiresBug3957833WAR_FNPTR(pKernelFsp) pKernelFsp->__kfspRequiresBug3957833WAR__
#define kfspRequiresBug3957833WAR(pGpu, pKernelFsp) kfspRequiresBug3957833WAR_DISPATCH(pGpu, pKernelFsp)
#define kfspRequiresBug3957833WAR_HAL(pGpu, pKernelFsp) kfspRequiresBug3957833WAR_DISPATCH(pGpu, pKernelFsp)
#define kfspFrtsSysmemLocationProgram_FNPTR(pKernelFsp) pKernelFsp->__kfspFrtsSysmemLocationProgram__
#define kfspFrtsSysmemLocationProgram(pGpu, pKernelFsp) kfspFrtsSysmemLocationProgram_DISPATCH(pGpu, pKernelFsp)
#define kfspFrtsSysmemLocationProgram_HAL(pGpu, pKernelFsp) kfspFrtsSysmemLocationProgram_DISPATCH(pGpu, pKernelFsp)
#define kfspFrtsSysmemLocationClear_FNPTR(pKernelFsp) pKernelFsp->__kfspFrtsSysmemLocationClear__
#define kfspFrtsSysmemLocationClear(pGpu, pKernelFsp) kfspFrtsSysmemLocationClear_DISPATCH(pGpu, pKernelFsp)
#define kfspFrtsSysmemLocationClear_HAL(pGpu, pKernelFsp) kfspFrtsSysmemLocationClear_DISPATCH(pGpu, pKernelFsp)
#define kfspCheckForClockBoostCapability_FNPTR(pKernelFsp) pKernelFsp->__kfspCheckForClockBoostCapability__
#define kfspCheckForClockBoostCapability(pGpu, pKernelFsp) kfspCheckForClockBoostCapability_DISPATCH(pGpu, pKernelFsp)
#define kfspCheckForClockBoostCapability_HAL(pGpu, pKernelFsp) kfspCheckForClockBoostCapability_DISPATCH(pGpu, pKernelFsp)
#define kfspSendClockBoostRpc_FNPTR(pKernelFsp) pKernelFsp->__kfspSendClockBoostRpc__
#define kfspSendClockBoostRpc(pGpu, pKernelFsp, cmd) kfspSendClockBoostRpc_DISPATCH(pGpu, pKernelFsp, cmd)
#define kfspSendClockBoostRpc_HAL(pGpu, pKernelFsp, cmd) kfspSendClockBoostRpc_DISPATCH(pGpu, pKernelFsp, cmd)
#define kfspInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateInitMissing__
#define kfspInitMissing(pGpu, pEngstate) kfspInitMissing_DISPATCH(pGpu, pEngstate)
#define kfspStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreInitLocked__
#define kfspStatePreInitLocked(pGpu, pEngstate) kfspStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kfspStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreInitUnlocked__
#define kfspStatePreInitUnlocked(pGpu, pEngstate) kfspStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kfspStateInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__
#define kfspStateInitLocked(pGpu, pEngstate) kfspStateInitLocked_DISPATCH(pGpu, pEngstate)
#define kfspStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateInitUnlocked__
#define kfspStateInitUnlocked(pGpu, pEngstate) kfspStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kfspStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreLoad__
#define kfspStatePreLoad(pGpu, pEngstate, arg3) kfspStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kfspStateLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateLoad__
#define kfspStateLoad(pGpu, pEngstate, arg3) kfspStateLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kfspStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePostLoad__
#define kfspStatePostLoad(pGpu, pEngstate, arg3) kfspStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kfspStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreUnload__
#define kfspStatePreUnload(pGpu, pEngstate, arg3) kfspStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kfspStateUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateUnload__
#define kfspStateUnload(pGpu, pEngstate, arg3) kfspStateUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kfspStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePostUnload__
#define kfspStatePostUnload(pGpu, pEngstate, arg3) kfspStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kfspIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateIsPresent__
#define kfspIsPresent(pGpu, pEngstate) kfspIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS kfspConstructEngine_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, ENGDESCRIPTOR arg3) {
    return pKernelFsp->__kfspConstructEngine__(pGpu, pKernelFsp, arg3);
}

static inline void kfspStateDestroy_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    pKernelFsp->__kfspStateDestroy__(pGpu, pKernelFsp);
}

static inline NV_STATUS kfspSendAndReadMessageAsync_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pPayload, NvU32 size, NvU32 nvdmType, NvU8 *pResponsePayload, NvU32 responseBufferSize, AsyncRpcCallback callback, void *pCallbackArgs) {
    return pKernelFsp->__kfspSendAndReadMessageAsync__(pGpu, pKernelFsp, pPayload, size, nvdmType, pResponsePayload, responseBufferSize, callback, pCallbackArgs);
}

static inline NvBool kfspGspFmcIsEnforced_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return pKernelFsp->__kfspGspFmcIsEnforced__(pGpu, pKernelFsp);
}

static inline NV_STATUS kfspPrepareBootCommands_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return pKernelFsp->__kfspPrepareBootCommands__(pGpu, pKernelFsp);
}

static inline NV_STATUS kfspSendBootCommands_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return pKernelFsp->__kfspSendBootCommands__(pGpu, pKernelFsp);
}

static inline NV_STATUS kfspPrepareAndSendBootCommands_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return pKernelFsp->__kfspPrepareAndSendBootCommands__(pGpu, pKernelFsp);
}

static inline NV_STATUS kfspWaitForSecureBoot_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return pKernelFsp->__kfspWaitForSecureBoot__(pGpu, pKernelFsp);
}

static inline NvU32 kfspGetRmChannelSize_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return pKernelFsp->__kfspGetRmChannelSize__(pGpu, pKernelFsp);
}

static inline NV_STATUS kfspConfigEmemc_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 offset, NvBool bAincw, NvBool bAincr) {
    return pKernelFsp->__kfspConfigEmemc__(pGpu, pKernelFsp, offset, bAincw, bAincr);
}

static inline void kfspUpdateQueueHeadTail_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 queueHead, NvU32 queueTail) {
    pKernelFsp->__kfspUpdateQueueHeadTail__(pGpu, pKernelFsp, queueHead, queueTail);
}

static inline void kfspGetQueueHeadTail_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 *pQueueHead, NvU32 *pQueueTail) {
    pKernelFsp->__kfspGetQueueHeadTail__(pGpu, pKernelFsp, pQueueHead, pQueueTail);
}

static inline void kfspUpdateMsgQueueHeadTail_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 msgqHead, NvU32 msgqTail) {
    pKernelFsp->__kfspUpdateMsgQueueHeadTail__(pGpu, pKernelFsp, msgqHead, msgqTail);
}

static inline void kfspGetMsgQueueHeadTail_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 *pMsgqHead, NvU32 *pMsgqTail) {
    pKernelFsp->__kfspGetMsgQueueHeadTail__(pGpu, pKernelFsp, pMsgqHead, pMsgqTail);
}

static inline NvU8 kfspNvdmToSeid_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 nvdmType) {
    return pKernelFsp->__kfspNvdmToSeid__(pGpu, pKernelFsp, nvdmType);
}

static inline NvU32 kfspCreateMctpHeader_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 som, NvU8 eom, NvU8 seid, NvU8 seq) {
    return pKernelFsp->__kfspCreateMctpHeader__(pGpu, pKernelFsp, som, eom, seid, seq);
}

static inline NvU32 kfspCreateNvdmHeader_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 nvdmType) {
    return pKernelFsp->__kfspCreateNvdmHeader__(pGpu, pKernelFsp, nvdmType);
}

static inline NV_STATUS kfspWriteToEmem_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size) {
    return pKernelFsp->__kfspWriteToEmem__(pGpu, pKernelFsp, pBuffer, size);
}

static inline NV_STATUS kfspReadFromEmem_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size) {
    return pKernelFsp->__kfspReadFromEmem__(pGpu, pKernelFsp, pBuffer, size);
}

static inline NV_STATUS kfspGetPacketInfo_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size, MCTP_PACKET_STATE *pPacketState, NvU8 *pTag) {
    return pKernelFsp->__kfspGetPacketInfo__(pGpu, pKernelFsp, pBuffer, size, pPacketState, pTag);
}

static inline NV_STATUS kfspValidateMctpPayloadHeader_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size) {
    return pKernelFsp->__kfspValidateMctpPayloadHeader__(pGpu, pKernelFsp, pBuffer, size);
}

static inline NV_STATUS kfspProcessNvdmMessage_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size) {
    return pKernelFsp->__kfspProcessNvdmMessage__(pGpu, pKernelFsp, pBuffer, size);
}

static inline NV_STATUS kfspProcessCommandResponse_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size) {
    return pKernelFsp->__kfspProcessCommandResponse__(pGpu, pKernelFsp, pBuffer, size);
}

static inline void kfspDumpDebugState_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    pKernelFsp->__kfspDumpDebugState__(pGpu, pKernelFsp);
}

static inline NV_STATUS kfspErrorCode2NvStatusMap_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 errorCode) {
    return pKernelFsp->__kfspErrorCode2NvStatusMap__(pGpu, pKernelFsp, errorCode);
}

static inline NvU64 kfspGetExtraReservedMemorySize_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return pKernelFsp->__kfspGetExtraReservedMemorySize__(pGpu, pKernelFsp);
}

static inline NV_STATUS kfspWaitForGspTargetMaskReleased_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return pKernelFsp->__kfspWaitForGspTargetMaskReleased__(pGpu, pKernelFsp);
}

static inline NvBool kfspRequiresBug3957833WAR_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return pKernelFsp->__kfspRequiresBug3957833WAR__(pGpu, pKernelFsp);
}

static inline NV_STATUS kfspFrtsSysmemLocationProgram_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return pKernelFsp->__kfspFrtsSysmemLocationProgram__(pGpu, pKernelFsp);
}

static inline void kfspFrtsSysmemLocationClear_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    pKernelFsp->__kfspFrtsSysmemLocationClear__(pGpu, pKernelFsp);
}

static inline void kfspCheckForClockBoostCapability_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    pKernelFsp->__kfspCheckForClockBoostCapability__(pGpu, pKernelFsp);
}

static inline NV_STATUS kfspSendClockBoostRpc_DISPATCH(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 cmd) {
    return pKernelFsp->__kfspSendClockBoostRpc__(pGpu, pKernelFsp, cmd);
}

static inline void kfspInitMissing_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pEngstate) {
    pEngstate->__kfspInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kfspStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pEngstate) {
    return pEngstate->__kfspStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kfspStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pEngstate) {
    return pEngstate->__kfspStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kfspStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pEngstate) {
    return pEngstate->__kfspStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kfspStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pEngstate) {
    return pEngstate->__kfspStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kfspStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pEngstate, NvU32 arg3) {
    return pEngstate->__kfspStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kfspStateLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pEngstate, NvU32 arg3) {
    return pEngstate->__kfspStateLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kfspStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pEngstate, NvU32 arg3) {
    return pEngstate->__kfspStatePostLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kfspStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pEngstate, NvU32 arg3) {
    return pEngstate->__kfspStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kfspStateUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pEngstate, NvU32 arg3) {
    return pEngstate->__kfspStateUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kfspStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pEngstate, NvU32 arg3) {
    return pEngstate->__kfspStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline NvBool kfspIsPresent_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pEngstate) {
    return pEngstate->__kfspIsPresent__(pGpu, pEngstate);
}

NV_STATUS kfspConstructEngine_IMPL(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, ENGDESCRIPTOR arg3);

void kfspStateDestroy_IMPL(OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

NV_STATUS kfspSendAndReadMessageAsync_IMPL(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pPayload, NvU32 size, NvU32 nvdmType, NvU8 *pResponsePayload, NvU32 responseBufferSize, AsyncRpcCallback callback, void *pCallbackArgs);

NvBool kfspGspFmcIsEnforced_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline NvBool kfspGspFmcIsEnforced_491d52(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return ((NvBool)(0 != 0));
}

NV_STATUS kfspPrepareBootCommands_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline NV_STATUS kfspPrepareBootCommands_ac1694(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return NV_OK;
}

NV_STATUS kfspSendBootCommands_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline NV_STATUS kfspSendBootCommands_ac1694(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return NV_OK;
}

NV_STATUS kfspPrepareAndSendBootCommands_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline NV_STATUS kfspPrepareAndSendBootCommands_ac1694(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return NV_OK;
}

NV_STATUS kfspWaitForSecureBoot_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

NV_STATUS kfspWaitForSecureBoot_GB100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline NV_STATUS kfspWaitForSecureBoot_46f6a7(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return NV_ERR_NOT_SUPPORTED;
}

NvU32 kfspGetRmChannelSize_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline NvU32 kfspGetRmChannelSize_b2b553(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return 0;
}

NV_STATUS kfspConfigEmemc_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 offset, NvBool bAincw, NvBool bAincr);

static inline NV_STATUS kfspConfigEmemc_395e98(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 offset, NvBool bAincw, NvBool bAincr) {
    return NV_ERR_NOT_SUPPORTED;
}

void kfspUpdateQueueHeadTail_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 queueHead, NvU32 queueTail);

static inline void kfspUpdateQueueHeadTail_d44104(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 queueHead, NvU32 queueTail) {
    return;
}

void kfspGetQueueHeadTail_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 *pQueueHead, NvU32 *pQueueTail);

static inline void kfspGetQueueHeadTail_d44104(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 *pQueueHead, NvU32 *pQueueTail) {
    return;
}

void kfspUpdateMsgQueueHeadTail_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 msgqHead, NvU32 msgqTail);

static inline void kfspUpdateMsgQueueHeadTail_d44104(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 msgqHead, NvU32 msgqTail) {
    return;
}

void kfspGetMsgQueueHeadTail_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 *pMsgqHead, NvU32 *pMsgqTail);

static inline void kfspGetMsgQueueHeadTail_d44104(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 *pMsgqHead, NvU32 *pMsgqTail) {
    return;
}

NvU8 kfspNvdmToSeid_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 nvdmType);

static inline NvU8 kfspNvdmToSeid_b2b553(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 nvdmType) {
    return 0;
}

NvU32 kfspCreateMctpHeader_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 som, NvU8 eom, NvU8 seid, NvU8 seq);

static inline NvU32 kfspCreateMctpHeader_b2b553(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 som, NvU8 eom, NvU8 seid, NvU8 seq) {
    return 0;
}

NvU32 kfspCreateNvdmHeader_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 nvdmType);

static inline NvU32 kfspCreateNvdmHeader_b2b553(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 nvdmType) {
    return 0;
}

NV_STATUS kfspWriteToEmem_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size);

static inline NV_STATUS kfspWriteToEmem_395e98(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kfspReadFromEmem_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size);

static inline NV_STATUS kfspReadFromEmem_395e98(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kfspGetPacketInfo_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size, MCTP_PACKET_STATE *pPacketState, NvU8 *pTag);

static inline NV_STATUS kfspGetPacketInfo_395e98(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size, MCTP_PACKET_STATE *pPacketState, NvU8 *pTag) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kfspValidateMctpPayloadHeader_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size);

static inline NV_STATUS kfspValidateMctpPayloadHeader_395e98(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kfspProcessNvdmMessage_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size);

static inline NV_STATUS kfspProcessNvdmMessage_395e98(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kfspProcessCommandResponse_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size);

static inline NV_STATUS kfspProcessCommandResponse_395e98(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size) {
    return NV_ERR_NOT_SUPPORTED;
}

void kfspDumpDebugState_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline void kfspDumpDebugState_b3696a(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return;
}

NV_STATUS kfspErrorCode2NvStatusMap_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 errorCode);

static inline NV_STATUS kfspErrorCode2NvStatusMap_395e98(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 errorCode) {
    return NV_ERR_NOT_SUPPORTED;
}

NvU64 kfspGetExtraReservedMemorySize_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline NvU64 kfspGetExtraReservedMemorySize_4a4dee(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return 0;
}

NV_STATUS kfspWaitForGspTargetMaskReleased_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline NV_STATUS kfspWaitForGspTargetMaskReleased_395e98(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return NV_ERR_NOT_SUPPORTED;
}

NvBool kfspRequiresBug3957833WAR_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline NvBool kfspRequiresBug3957833WAR_491d52(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return ((NvBool)(0 != 0));
}

NV_STATUS kfspFrtsSysmemLocationProgram_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline NV_STATUS kfspFrtsSysmemLocationProgram_395e98(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return NV_ERR_NOT_SUPPORTED;
}

void kfspFrtsSysmemLocationClear_GH100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline void kfspFrtsSysmemLocationClear_d44104(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return;
}

void kfspCheckForClockBoostCapability_GB100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline void kfspCheckForClockBoostCapability_b3696a(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return;
}

NV_STATUS kfspSendClockBoostRpc_GB100(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 cmd);

static inline NV_STATUS kfspSendClockBoostRpc_56cd7a(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 cmd) {
    return NV_OK;
}

void kfspCleanupBootState_IMPL(OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

#ifdef __nvoc_kern_fsp_h_disabled
static inline void kfspCleanupBootState(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    NV_ASSERT_FAILED_PRECOMP("KernelFsp was disabled!");
}
#else //__nvoc_kern_fsp_h_disabled
#define kfspCleanupBootState(pGpu, pKernelFsp) kfspCleanupBootState_IMPL(pGpu, pKernelFsp)
#endif //__nvoc_kern_fsp_h_disabled

NV_STATUS kfspSendPacket_IMPL(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pPacket, NvU32 packetSize);

#ifdef __nvoc_kern_fsp_h_disabled
static inline NV_STATUS kfspSendPacket(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pPacket, NvU32 packetSize) {
    NV_ASSERT_FAILED_PRECOMP("KernelFsp was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_fsp_h_disabled
#define kfspSendPacket(pGpu, pKernelFsp, pPacket, packetSize) kfspSendPacket_IMPL(pGpu, pKernelFsp, pPacket, packetSize)
#endif //__nvoc_kern_fsp_h_disabled

NV_STATUS kfspSendAndReadMessage_IMPL(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pPayload, NvU32 size, NvU32 nvdmType, NvU8 *pResponsePayload, NvU32 responseBufferSize);

#ifdef __nvoc_kern_fsp_h_disabled
static inline NV_STATUS kfspSendAndReadMessage(OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pPayload, NvU32 size, NvU32 nvdmType, NvU8 *pResponsePayload, NvU32 responseBufferSize) {
    NV_ASSERT_FAILED_PRECOMP("KernelFsp was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_fsp_h_disabled
#define kfspSendAndReadMessage(pGpu, pKernelFsp, pPayload, size, nvdmType, pResponsePayload, responseBufferSize) kfspSendAndReadMessage_IMPL(pGpu, pKernelFsp, pPayload, size, nvdmType, pResponsePayload, responseBufferSize)
#endif //__nvoc_kern_fsp_h_disabled

NvBool kfspIsQueueEmpty_IMPL(OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

#ifdef __nvoc_kern_fsp_h_disabled
static inline NvBool kfspIsQueueEmpty(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    NV_ASSERT_FAILED_PRECOMP("KernelFsp was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kern_fsp_h_disabled
#define kfspIsQueueEmpty(pGpu, pKernelFsp) kfspIsQueueEmpty_IMPL(pGpu, pKernelFsp)
#endif //__nvoc_kern_fsp_h_disabled

NV_STATUS kfspPollForQueueEmpty_IMPL(OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

#ifdef __nvoc_kern_fsp_h_disabled
static inline NV_STATUS kfspPollForQueueEmpty(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    NV_ASSERT_FAILED_PRECOMP("KernelFsp was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_fsp_h_disabled
#define kfspPollForQueueEmpty(pGpu, pKernelFsp) kfspPollForQueueEmpty_IMPL(pGpu, pKernelFsp)
#endif //__nvoc_kern_fsp_h_disabled

NvBool kfspIsMsgQueueEmpty_IMPL(OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

#ifdef __nvoc_kern_fsp_h_disabled
static inline NvBool kfspIsMsgQueueEmpty(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    NV_ASSERT_FAILED_PRECOMP("KernelFsp was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kern_fsp_h_disabled
#define kfspIsMsgQueueEmpty(pGpu, pKernelFsp) kfspIsMsgQueueEmpty_IMPL(pGpu, pKernelFsp)
#endif //__nvoc_kern_fsp_h_disabled

NV_STATUS kfspPollForResponse_IMPL(OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

#ifdef __nvoc_kern_fsp_h_disabled
static inline NV_STATUS kfspPollForResponse(OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    NV_ASSERT_FAILED_PRECOMP("KernelFsp was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_fsp_h_disabled
#define kfspPollForResponse(pGpu, pKernelFsp) kfspPollForResponse_IMPL(pGpu, pKernelFsp)
#endif //__nvoc_kern_fsp_h_disabled

#undef PRIVATE_FIELD


#endif // KERN_FSP_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERN_FSP_NVOC_H_
