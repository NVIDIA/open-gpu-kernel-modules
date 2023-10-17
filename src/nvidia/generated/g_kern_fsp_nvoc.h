#ifndef _G_KERN_FSP_NVOC_H_
#define _G_KERN_FSP_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_kern_fsp_nvoc.h"

#ifndef KERN_FSP_H
#define KERN_FSP_H

#include "core/core.h"
#include "gpu/eng_state.h"
#include "kernel/gpu/fifo/channel_descendant.h"
#include "kernel/gpu/intr/intr_service.h"
#include "fsp/nvdm_payload_cmd_response.h"

//
// VBIOS ucode descriptor v4 format. Used for the CPU command to FSP to boot
// GSP FMC.
//
typedef struct
{
    NvU8  flags;
    NvU8  version;
    NvU16 structSize;
    NvU32 storeSize;
    NvU32 IMEMOffset;
    NvU32 IMEMSize;
    NvU32 DMEMOffset;
    NvU32 DMEMSize;
    NvU8  ucodeId;
    NvU8  ucodeVersion;
    NvU16 payloadCount;
    NvU16 payloadSize;
    NvU8  ucodeVerMajor;
    NvU8  ucodeVerMinor;
    NvU32 cryptoData[10]; // unused
} FSP_UCODE_DESC_V4;

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

typedef struct
{
    NvU8 nvdmType;

    // We can make this a union when adding more NVDM payloads
    NVDM_PAYLOAD_COT cotPayload;
} NVDM_PACKET;

// The structure cannot have embedded pointers to send as byte stream
typedef struct
{
    MCTP_HEADER header;
    NVDM_PACKET nvdmPacket;
} MCTP_PACKET, *PMCTP_PACKET;

// Type of packet, can either be SOM, EOM, neither, or both (1-packet messages)
typedef enum
{
    MCTP_PACKET_STATE_START,
    MCTP_PACKET_STATE_INTERMEDIATE,
    MCTP_PACKET_STATE_END,
    MCTP_PACKET_STATE_SINGLE_PACKET
} MCTP_PACKET_STATE;

#ifdef NVOC_KERN_FSP_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelFsp {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct KernelFsp *__nvoc_pbase_KernelFsp;
    NV_STATUS (*__kfspConstructEngine__)(struct OBJGPU *, struct KernelFsp *, ENGDESCRIPTOR);
    void (*__kfspStateDestroy__)(struct OBJGPU *, struct KernelFsp *);
    void (*__kfspSecureReset__)(struct OBJGPU *, struct KernelFsp *);
    NV_STATUS (*__kfspSendPacket__)(struct OBJGPU *, struct KernelFsp *, NvU8 *, NvU32);
    NV_STATUS (*__kfspSendAndReadMessage__)(struct OBJGPU *, struct KernelFsp *, NvU8 *, NvU32, NvU32, NvU8 *, NvU32);
    NvBool (*__kfspIsQueueEmpty__)(struct OBJGPU *, struct KernelFsp *);
    NV_STATUS (*__kfspPollForQueueEmpty__)(struct OBJGPU *, struct KernelFsp *);
    NvBool (*__kfspIsMsgQueueEmpty__)(struct OBJGPU *, struct KernelFsp *);
    NV_STATUS (*__kfspPollForResponse__)(struct OBJGPU *, struct KernelFsp *);
    NvBool (*__kfspGspFmcIsEnforced__)(struct OBJGPU *, struct KernelFsp *);
    NV_STATUS (*__kfspSendBootCommands__)(struct OBJGPU *, struct KernelFsp *);
    NV_STATUS (*__kfspWaitForSecureBoot__)(struct OBJGPU *, struct KernelFsp *);
    NvU32 (*__kfspGetRmChannelSize__)(struct OBJGPU *, struct KernelFsp *);
    NV_STATUS (*__kfspConfigEmemc__)(struct OBJGPU *, struct KernelFsp *, NvU32, NvBool, NvBool);
    void (*__kfspUpdateQueueHeadTail__)(struct OBJGPU *, struct KernelFsp *, NvU32, NvU32);
    void (*__kfspGetQueueHeadTail__)(struct OBJGPU *, struct KernelFsp *, NvU32 *, NvU32 *);
    void (*__kfspUpdateMsgQueueHeadTail__)(struct OBJGPU *, struct KernelFsp *, NvU32, NvU32);
    void (*__kfspGetMsgQueueHeadTail__)(struct OBJGPU *, struct KernelFsp *, NvU32 *, NvU32 *);
    NvU8 (*__kfspNvdmToSeid__)(struct OBJGPU *, struct KernelFsp *, NvU8);
    NvU32 (*__kfspCreateMctpHeader__)(struct OBJGPU *, struct KernelFsp *, NvU8, NvU8, NvU8, NvU8);
    NvU32 (*__kfspCreateNvdmHeader__)(struct OBJGPU *, struct KernelFsp *, NvU32);
    NV_STATUS (*__kfspWriteToEmem__)(struct OBJGPU *, struct KernelFsp *, NvU8 *, NvU32);
    NV_STATUS (*__kfspReadFromEmem__)(struct OBJGPU *, struct KernelFsp *, NvU8 *, NvU32);
    NV_STATUS (*__kfspGetPacketInfo__)(struct OBJGPU *, struct KernelFsp *, NvU8 *, NvU32, MCTP_PACKET_STATE *, NvU8 *);
    NV_STATUS (*__kfspValidateMctpPayloadHeader__)(struct OBJGPU *, struct KernelFsp *, NvU8 *, NvU32);
    NV_STATUS (*__kfspProcessNvdmMessage__)(struct OBJGPU *, struct KernelFsp *, NvU8 *, NvU32);
    NV_STATUS (*__kfspProcessCommandResponse__)(struct OBJGPU *, struct KernelFsp *, NvU8 *, NvU32);
    void (*__kfspDumpDebugState__)(struct OBJGPU *, struct KernelFsp *);
    NV_STATUS (*__kfspErrorCode2NvStatusMap__)(struct OBJGPU *, struct KernelFsp *, NvU32);
    NvU64 (*__kfspGetExtraReservedMemorySize__)(struct OBJGPU *, struct KernelFsp *);
    NvBool (*__kfspCheckGspSecureScratch__)(struct OBJGPU *, struct KernelFsp *);
    NV_STATUS (*__kfspWaitForGspTargetMaskReleased__)(struct OBJGPU *, struct KernelFsp *);
    NvBool (*__kfspRequiresBug3957833WAR__)(struct OBJGPU *, struct KernelFsp *);
    NV_STATUS (*__kfspStateLoad__)(POBJGPU, struct KernelFsp *, NvU32);
    NV_STATUS (*__kfspStateUnload__)(POBJGPU, struct KernelFsp *, NvU32);
    NV_STATUS (*__kfspStateInitLocked__)(POBJGPU, struct KernelFsp *);
    NV_STATUS (*__kfspStatePreLoad__)(POBJGPU, struct KernelFsp *, NvU32);
    NV_STATUS (*__kfspStatePostUnload__)(POBJGPU, struct KernelFsp *, NvU32);
    NV_STATUS (*__kfspStatePreUnload__)(POBJGPU, struct KernelFsp *, NvU32);
    NV_STATUS (*__kfspStateInitUnlocked__)(POBJGPU, struct KernelFsp *);
    void (*__kfspInitMissing__)(POBJGPU, struct KernelFsp *);
    NV_STATUS (*__kfspStatePreInitLocked__)(POBJGPU, struct KernelFsp *);
    NV_STATUS (*__kfspStatePreInitUnlocked__)(POBJGPU, struct KernelFsp *);
    NV_STATUS (*__kfspStatePostLoad__)(POBJGPU, struct KernelFsp *, NvU32);
    NvBool (*__kfspIsPresent__)(POBJGPU, struct KernelFsp *);
    NvBool PDB_PROP_KFSP_BOOT_COMMAND_OK;
    NvBool PDB_PROP_KFSP_GSP_MODE_GSPRM;
    NvBool PDB_PROP_KFSP_DISABLE_FRTS_SYSMEM;
    NvBool PDB_PROP_KFSP_DISABLE_FRTS_VIDMEM;
    NvBool PDB_PROP_KFSP_DISABLE_GSPFMC;
    MEMORY_DESCRIPTOR *pSysmemFrtsMemdesc;
    MEMORY_DESCRIPTOR *pVidmemFrtsMemdesc;
    MEMORY_DESCRIPTOR *pGspFmcMemdesc;
    MEMORY_DESCRIPTOR *pGspBootArgsMemdesc;
    NVDM_PAYLOAD_COT *pCotPayload;
    NvBool bGspDebugBufferInitialized;
};

#ifndef __NVOC_CLASS_KernelFsp_TYPEDEF__
#define __NVOC_CLASS_KernelFsp_TYPEDEF__
typedef struct KernelFsp KernelFsp;
#endif /* __NVOC_CLASS_KernelFsp_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelFsp
#define __nvoc_class_id_KernelFsp 0x87fb96
#endif /* __nvoc_class_id_KernelFsp */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFsp;

#define __staticCast_KernelFsp(pThis) \
    ((pThis)->__nvoc_pbase_KernelFsp)

#ifdef __nvoc_kern_fsp_h_disabled
#define __dynamicCast_KernelFsp(pThis) ((KernelFsp*)NULL)
#else //__nvoc_kern_fsp_h_disabled
#define __dynamicCast_KernelFsp(pThis) \
    ((KernelFsp*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelFsp)))
#endif //__nvoc_kern_fsp_h_disabled

#define PDB_PROP_KFSP_DISABLE_FRTS_VIDMEM_BASE_CAST
#define PDB_PROP_KFSP_DISABLE_FRTS_VIDMEM_BASE_NAME PDB_PROP_KFSP_DISABLE_FRTS_VIDMEM
#define PDB_PROP_KFSP_DISABLE_GSPFMC_BASE_CAST
#define PDB_PROP_KFSP_DISABLE_GSPFMC_BASE_NAME PDB_PROP_KFSP_DISABLE_GSPFMC
#define PDB_PROP_KFSP_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KFSP_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING
#define PDB_PROP_KFSP_GSP_MODE_GSPRM_BASE_CAST
#define PDB_PROP_KFSP_GSP_MODE_GSPRM_BASE_NAME PDB_PROP_KFSP_GSP_MODE_GSPRM
#define PDB_PROP_KFSP_BOOT_COMMAND_OK_BASE_CAST
#define PDB_PROP_KFSP_BOOT_COMMAND_OK_BASE_NAME PDB_PROP_KFSP_BOOT_COMMAND_OK
#define PDB_PROP_KFSP_DISABLE_FRTS_SYSMEM_BASE_CAST
#define PDB_PROP_KFSP_DISABLE_FRTS_SYSMEM_BASE_NAME PDB_PROP_KFSP_DISABLE_FRTS_SYSMEM

NV_STATUS __nvoc_objCreateDynamic_KernelFsp(KernelFsp**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelFsp(KernelFsp**, Dynamic*, NvU32);
#define __objCreate_KernelFsp(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelFsp((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define kfspConstructEngine(pGpu, pKernelFsp, arg0) kfspConstructEngine_DISPATCH(pGpu, pKernelFsp, arg0)
#define kfspStateDestroy(pGpu, pKernelFsp) kfspStateDestroy_DISPATCH(pGpu, pKernelFsp)
#define kfspSecureReset(pGpu, pKernelFsp) kfspSecureReset_DISPATCH(pGpu, pKernelFsp)
#define kfspSendPacket(pGpu, pKernelFsp, pPacket, packetSize) kfspSendPacket_DISPATCH(pGpu, pKernelFsp, pPacket, packetSize)
#define kfspSendAndReadMessage(pGpu, pKernelFsp, pPayload, size, nvdmType, pResponsePayload, responseBufferSize) kfspSendAndReadMessage_DISPATCH(pGpu, pKernelFsp, pPayload, size, nvdmType, pResponsePayload, responseBufferSize)
#define kfspIsQueueEmpty(pGpu, pKernelFsp) kfspIsQueueEmpty_DISPATCH(pGpu, pKernelFsp)
#define kfspPollForQueueEmpty(pGpu, pKernelFsp) kfspPollForQueueEmpty_DISPATCH(pGpu, pKernelFsp)
#define kfspIsMsgQueueEmpty(pGpu, pKernelFsp) kfspIsMsgQueueEmpty_DISPATCH(pGpu, pKernelFsp)
#define kfspPollForResponse(pGpu, pKernelFsp) kfspPollForResponse_DISPATCH(pGpu, pKernelFsp)
#define kfspGspFmcIsEnforced(pGpu, pKernelFsp) kfspGspFmcIsEnforced_DISPATCH(pGpu, pKernelFsp)
#define kfspGspFmcIsEnforced_HAL(pGpu, pKernelFsp) kfspGspFmcIsEnforced_DISPATCH(pGpu, pKernelFsp)
#define kfspSendBootCommands(pGpu, pKernelFsp) kfspSendBootCommands_DISPATCH(pGpu, pKernelFsp)
#define kfspSendBootCommands_HAL(pGpu, pKernelFsp) kfspSendBootCommands_DISPATCH(pGpu, pKernelFsp)
#define kfspWaitForSecureBoot(pGpu, pKernelFsp) kfspWaitForSecureBoot_DISPATCH(pGpu, pKernelFsp)
#define kfspWaitForSecureBoot_HAL(pGpu, pKernelFsp) kfspWaitForSecureBoot_DISPATCH(pGpu, pKernelFsp)
#define kfspGetRmChannelSize(pGpu, pKernelFsp) kfspGetRmChannelSize_DISPATCH(pGpu, pKernelFsp)
#define kfspGetRmChannelSize_HAL(pGpu, pKernelFsp) kfspGetRmChannelSize_DISPATCH(pGpu, pKernelFsp)
#define kfspConfigEmemc(pGpu, pKernelFsp, offset, bAincw, bAincr) kfspConfigEmemc_DISPATCH(pGpu, pKernelFsp, offset, bAincw, bAincr)
#define kfspConfigEmemc_HAL(pGpu, pKernelFsp, offset, bAincw, bAincr) kfspConfigEmemc_DISPATCH(pGpu, pKernelFsp, offset, bAincw, bAincr)
#define kfspUpdateQueueHeadTail(pGpu, pKernelFsp, queueHead, queueTail) kfspUpdateQueueHeadTail_DISPATCH(pGpu, pKernelFsp, queueHead, queueTail)
#define kfspUpdateQueueHeadTail_HAL(pGpu, pKernelFsp, queueHead, queueTail) kfspUpdateQueueHeadTail_DISPATCH(pGpu, pKernelFsp, queueHead, queueTail)
#define kfspGetQueueHeadTail(pGpu, pKernelFsp, pQueueHead, pQueueTail) kfspGetQueueHeadTail_DISPATCH(pGpu, pKernelFsp, pQueueHead, pQueueTail)
#define kfspGetQueueHeadTail_HAL(pGpu, pKernelFsp, pQueueHead, pQueueTail) kfspGetQueueHeadTail_DISPATCH(pGpu, pKernelFsp, pQueueHead, pQueueTail)
#define kfspUpdateMsgQueueHeadTail(pGpu, pKernelFsp, msgqHead, msgqTail) kfspUpdateMsgQueueHeadTail_DISPATCH(pGpu, pKernelFsp, msgqHead, msgqTail)
#define kfspUpdateMsgQueueHeadTail_HAL(pGpu, pKernelFsp, msgqHead, msgqTail) kfspUpdateMsgQueueHeadTail_DISPATCH(pGpu, pKernelFsp, msgqHead, msgqTail)
#define kfspGetMsgQueueHeadTail(pGpu, pKernelFsp, pMsgqHead, pMsgqTail) kfspGetMsgQueueHeadTail_DISPATCH(pGpu, pKernelFsp, pMsgqHead, pMsgqTail)
#define kfspGetMsgQueueHeadTail_HAL(pGpu, pKernelFsp, pMsgqHead, pMsgqTail) kfspGetMsgQueueHeadTail_DISPATCH(pGpu, pKernelFsp, pMsgqHead, pMsgqTail)
#define kfspNvdmToSeid(pGpu, pKernelFsp, nvdmType) kfspNvdmToSeid_DISPATCH(pGpu, pKernelFsp, nvdmType)
#define kfspNvdmToSeid_HAL(pGpu, pKernelFsp, nvdmType) kfspNvdmToSeid_DISPATCH(pGpu, pKernelFsp, nvdmType)
#define kfspCreateMctpHeader(pGpu, pKernelFsp, som, eom, seid, seq) kfspCreateMctpHeader_DISPATCH(pGpu, pKernelFsp, som, eom, seid, seq)
#define kfspCreateMctpHeader_HAL(pGpu, pKernelFsp, som, eom, seid, seq) kfspCreateMctpHeader_DISPATCH(pGpu, pKernelFsp, som, eom, seid, seq)
#define kfspCreateNvdmHeader(pGpu, pKernelFsp, nvdmType) kfspCreateNvdmHeader_DISPATCH(pGpu, pKernelFsp, nvdmType)
#define kfspCreateNvdmHeader_HAL(pGpu, pKernelFsp, nvdmType) kfspCreateNvdmHeader_DISPATCH(pGpu, pKernelFsp, nvdmType)
#define kfspWriteToEmem(pGpu, pKernelFsp, pBuffer, size) kfspWriteToEmem_DISPATCH(pGpu, pKernelFsp, pBuffer, size)
#define kfspWriteToEmem_HAL(pGpu, pKernelFsp, pBuffer, size) kfspWriteToEmem_DISPATCH(pGpu, pKernelFsp, pBuffer, size)
#define kfspReadFromEmem(pGpu, pKernelFsp, pBuffer, size) kfspReadFromEmem_DISPATCH(pGpu, pKernelFsp, pBuffer, size)
#define kfspReadFromEmem_HAL(pGpu, pKernelFsp, pBuffer, size) kfspReadFromEmem_DISPATCH(pGpu, pKernelFsp, pBuffer, size)
#define kfspGetPacketInfo(pGpu, pKernelFsp, pBuffer, size, pPacketState, pTag) kfspGetPacketInfo_DISPATCH(pGpu, pKernelFsp, pBuffer, size, pPacketState, pTag)
#define kfspGetPacketInfo_HAL(pGpu, pKernelFsp, pBuffer, size, pPacketState, pTag) kfspGetPacketInfo_DISPATCH(pGpu, pKernelFsp, pBuffer, size, pPacketState, pTag)
#define kfspValidateMctpPayloadHeader(pGpu, pKernelFsp, pBuffer, size) kfspValidateMctpPayloadHeader_DISPATCH(pGpu, pKernelFsp, pBuffer, size)
#define kfspValidateMctpPayloadHeader_HAL(pGpu, pKernelFsp, pBuffer, size) kfspValidateMctpPayloadHeader_DISPATCH(pGpu, pKernelFsp, pBuffer, size)
#define kfspProcessNvdmMessage(pGpu, pKernelFsp, pBuffer, size) kfspProcessNvdmMessage_DISPATCH(pGpu, pKernelFsp, pBuffer, size)
#define kfspProcessNvdmMessage_HAL(pGpu, pKernelFsp, pBuffer, size) kfspProcessNvdmMessage_DISPATCH(pGpu, pKernelFsp, pBuffer, size)
#define kfspProcessCommandResponse(pGpu, pKernelFsp, pBuffer, size) kfspProcessCommandResponse_DISPATCH(pGpu, pKernelFsp, pBuffer, size)
#define kfspProcessCommandResponse_HAL(pGpu, pKernelFsp, pBuffer, size) kfspProcessCommandResponse_DISPATCH(pGpu, pKernelFsp, pBuffer, size)
#define kfspDumpDebugState(pGpu, pKernelFsp) kfspDumpDebugState_DISPATCH(pGpu, pKernelFsp)
#define kfspDumpDebugState_HAL(pGpu, pKernelFsp) kfspDumpDebugState_DISPATCH(pGpu, pKernelFsp)
#define kfspErrorCode2NvStatusMap(pGpu, pKernelFsp, errorCode) kfspErrorCode2NvStatusMap_DISPATCH(pGpu, pKernelFsp, errorCode)
#define kfspErrorCode2NvStatusMap_HAL(pGpu, pKernelFsp, errorCode) kfspErrorCode2NvStatusMap_DISPATCH(pGpu, pKernelFsp, errorCode)
#define kfspGetExtraReservedMemorySize(pGpu, pKernelFsp) kfspGetExtraReservedMemorySize_DISPATCH(pGpu, pKernelFsp)
#define kfspGetExtraReservedMemorySize_HAL(pGpu, pKernelFsp) kfspGetExtraReservedMemorySize_DISPATCH(pGpu, pKernelFsp)
#define kfspCheckGspSecureScratch(pGpu, pKernelFsp) kfspCheckGspSecureScratch_DISPATCH(pGpu, pKernelFsp)
#define kfspCheckGspSecureScratch_HAL(pGpu, pKernelFsp) kfspCheckGspSecureScratch_DISPATCH(pGpu, pKernelFsp)
#define kfspWaitForGspTargetMaskReleased(pGpu, pKernelFsp) kfspWaitForGspTargetMaskReleased_DISPATCH(pGpu, pKernelFsp)
#define kfspWaitForGspTargetMaskReleased_HAL(pGpu, pKernelFsp) kfspWaitForGspTargetMaskReleased_DISPATCH(pGpu, pKernelFsp)
#define kfspRequiresBug3957833WAR(pGpu, pKernelFsp) kfspRequiresBug3957833WAR_DISPATCH(pGpu, pKernelFsp)
#define kfspRequiresBug3957833WAR_HAL(pGpu, pKernelFsp) kfspRequiresBug3957833WAR_DISPATCH(pGpu, pKernelFsp)
#define kfspStateLoad(pGpu, pEngstate, arg0) kfspStateLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kfspStateUnload(pGpu, pEngstate, arg0) kfspStateUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kfspStateInitLocked(pGpu, pEngstate) kfspStateInitLocked_DISPATCH(pGpu, pEngstate)
#define kfspStatePreLoad(pGpu, pEngstate, arg0) kfspStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kfspStatePostUnload(pGpu, pEngstate, arg0) kfspStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kfspStatePreUnload(pGpu, pEngstate, arg0) kfspStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kfspStateInitUnlocked(pGpu, pEngstate) kfspStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kfspInitMissing(pGpu, pEngstate) kfspInitMissing_DISPATCH(pGpu, pEngstate)
#define kfspStatePreInitLocked(pGpu, pEngstate) kfspStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kfspStatePreInitUnlocked(pGpu, pEngstate) kfspStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kfspStatePostLoad(pGpu, pEngstate, arg0) kfspStatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kfspIsPresent(pGpu, pEngstate) kfspIsPresent_DISPATCH(pGpu, pEngstate)
NV_STATUS kfspConstructEngine_IMPL(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, ENGDESCRIPTOR arg0);

static inline NV_STATUS kfspConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, ENGDESCRIPTOR arg0) {
    return pKernelFsp->__kfspConstructEngine__(pGpu, pKernelFsp, arg0);
}

void kfspStateDestroy_IMPL(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline void kfspStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    pKernelFsp->__kfspStateDestroy__(pGpu, pKernelFsp);
}

void kfspSecureReset_IMPL(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline void kfspSecureReset_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    pKernelFsp->__kfspSecureReset__(pGpu, pKernelFsp);
}

NV_STATUS kfspSendPacket_IMPL(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pPacket, NvU32 packetSize);

static inline NV_STATUS kfspSendPacket_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pPacket, NvU32 packetSize) {
    return pKernelFsp->__kfspSendPacket__(pGpu, pKernelFsp, pPacket, packetSize);
}

NV_STATUS kfspSendAndReadMessage_IMPL(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pPayload, NvU32 size, NvU32 nvdmType, NvU8 *pResponsePayload, NvU32 responseBufferSize);

static inline NV_STATUS kfspSendAndReadMessage_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pPayload, NvU32 size, NvU32 nvdmType, NvU8 *pResponsePayload, NvU32 responseBufferSize) {
    return pKernelFsp->__kfspSendAndReadMessage__(pGpu, pKernelFsp, pPayload, size, nvdmType, pResponsePayload, responseBufferSize);
}

NvBool kfspIsQueueEmpty_IMPL(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline NvBool kfspIsQueueEmpty_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return pKernelFsp->__kfspIsQueueEmpty__(pGpu, pKernelFsp);
}

NV_STATUS kfspPollForQueueEmpty_IMPL(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline NV_STATUS kfspPollForQueueEmpty_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return pKernelFsp->__kfspPollForQueueEmpty__(pGpu, pKernelFsp);
}

NvBool kfspIsMsgQueueEmpty_IMPL(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline NvBool kfspIsMsgQueueEmpty_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return pKernelFsp->__kfspIsMsgQueueEmpty__(pGpu, pKernelFsp);
}

NV_STATUS kfspPollForResponse_IMPL(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline NV_STATUS kfspPollForResponse_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return pKernelFsp->__kfspPollForResponse__(pGpu, pKernelFsp);
}

NvBool kfspGspFmcIsEnforced_GH100(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline NvBool kfspGspFmcIsEnforced_491d52(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return ((NvBool)(0 != 0));
}

static inline NvBool kfspGspFmcIsEnforced_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return pKernelFsp->__kfspGspFmcIsEnforced__(pGpu, pKernelFsp);
}

NV_STATUS kfspSendBootCommands_GH100(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline NV_STATUS kfspSendBootCommands_ac1694(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return NV_OK;
}

static inline NV_STATUS kfspSendBootCommands_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return pKernelFsp->__kfspSendBootCommands__(pGpu, pKernelFsp);
}

NV_STATUS kfspWaitForSecureBoot_GH100(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline NV_STATUS kfspWaitForSecureBoot_395e98(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kfspWaitForSecureBoot_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return pKernelFsp->__kfspWaitForSecureBoot__(pGpu, pKernelFsp);
}

NvU32 kfspGetRmChannelSize_GH100(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline NvU32 kfspGetRmChannelSize_b2b553(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return 0;
}

static inline NvU32 kfspGetRmChannelSize_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return pKernelFsp->__kfspGetRmChannelSize__(pGpu, pKernelFsp);
}

NV_STATUS kfspConfigEmemc_GH100(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 offset, NvBool bAincw, NvBool bAincr);

static inline NV_STATUS kfspConfigEmemc_395e98(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 offset, NvBool bAincw, NvBool bAincr) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kfspConfigEmemc_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 offset, NvBool bAincw, NvBool bAincr) {
    return pKernelFsp->__kfspConfigEmemc__(pGpu, pKernelFsp, offset, bAincw, bAincr);
}

void kfspUpdateQueueHeadTail_GH100(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 queueHead, NvU32 queueTail);

static inline void kfspUpdateQueueHeadTail_d44104(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 queueHead, NvU32 queueTail) {
    return;
}

static inline void kfspUpdateQueueHeadTail_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 queueHead, NvU32 queueTail) {
    pKernelFsp->__kfspUpdateQueueHeadTail__(pGpu, pKernelFsp, queueHead, queueTail);
}

void kfspGetQueueHeadTail_GH100(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 *pQueueHead, NvU32 *pQueueTail);

static inline void kfspGetQueueHeadTail_d44104(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 *pQueueHead, NvU32 *pQueueTail) {
    return;
}

static inline void kfspGetQueueHeadTail_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 *pQueueHead, NvU32 *pQueueTail) {
    pKernelFsp->__kfspGetQueueHeadTail__(pGpu, pKernelFsp, pQueueHead, pQueueTail);
}

void kfspUpdateMsgQueueHeadTail_GH100(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 msgqHead, NvU32 msgqTail);

static inline void kfspUpdateMsgQueueHeadTail_d44104(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 msgqHead, NvU32 msgqTail) {
    return;
}

static inline void kfspUpdateMsgQueueHeadTail_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 msgqHead, NvU32 msgqTail) {
    pKernelFsp->__kfspUpdateMsgQueueHeadTail__(pGpu, pKernelFsp, msgqHead, msgqTail);
}

void kfspGetMsgQueueHeadTail_GH100(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 *pMsgqHead, NvU32 *pMsgqTail);

static inline void kfspGetMsgQueueHeadTail_d44104(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 *pMsgqHead, NvU32 *pMsgqTail) {
    return;
}

static inline void kfspGetMsgQueueHeadTail_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 *pMsgqHead, NvU32 *pMsgqTail) {
    pKernelFsp->__kfspGetMsgQueueHeadTail__(pGpu, pKernelFsp, pMsgqHead, pMsgqTail);
}

NvU8 kfspNvdmToSeid_GH100(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 nvdmType);

static inline NvU8 kfspNvdmToSeid_b2b553(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 nvdmType) {
    return 0;
}

static inline NvU8 kfspNvdmToSeid_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 nvdmType) {
    return pKernelFsp->__kfspNvdmToSeid__(pGpu, pKernelFsp, nvdmType);
}

NvU32 kfspCreateMctpHeader_GH100(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 som, NvU8 eom, NvU8 seid, NvU8 seq);

static inline NvU32 kfspCreateMctpHeader_b2b553(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 som, NvU8 eom, NvU8 seid, NvU8 seq) {
    return 0;
}

static inline NvU32 kfspCreateMctpHeader_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 som, NvU8 eom, NvU8 seid, NvU8 seq) {
    return pKernelFsp->__kfspCreateMctpHeader__(pGpu, pKernelFsp, som, eom, seid, seq);
}

NvU32 kfspCreateNvdmHeader_GH100(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 nvdmType);

static inline NvU32 kfspCreateNvdmHeader_b2b553(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 nvdmType) {
    return 0;
}

static inline NvU32 kfspCreateNvdmHeader_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 nvdmType) {
    return pKernelFsp->__kfspCreateNvdmHeader__(pGpu, pKernelFsp, nvdmType);
}

NV_STATUS kfspWriteToEmem_GH100(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size);

static inline NV_STATUS kfspWriteToEmem_395e98(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kfspWriteToEmem_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size) {
    return pKernelFsp->__kfspWriteToEmem__(pGpu, pKernelFsp, pBuffer, size);
}

NV_STATUS kfspReadFromEmem_GH100(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size);

static inline NV_STATUS kfspReadFromEmem_395e98(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kfspReadFromEmem_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size) {
    return pKernelFsp->__kfspReadFromEmem__(pGpu, pKernelFsp, pBuffer, size);
}

NV_STATUS kfspGetPacketInfo_GH100(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size, MCTP_PACKET_STATE *pPacketState, NvU8 *pTag);

static inline NV_STATUS kfspGetPacketInfo_395e98(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size, MCTP_PACKET_STATE *pPacketState, NvU8 *pTag) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kfspGetPacketInfo_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size, MCTP_PACKET_STATE *pPacketState, NvU8 *pTag) {
    return pKernelFsp->__kfspGetPacketInfo__(pGpu, pKernelFsp, pBuffer, size, pPacketState, pTag);
}

NV_STATUS kfspValidateMctpPayloadHeader_GH100(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size);

static inline NV_STATUS kfspValidateMctpPayloadHeader_395e98(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kfspValidateMctpPayloadHeader_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size) {
    return pKernelFsp->__kfspValidateMctpPayloadHeader__(pGpu, pKernelFsp, pBuffer, size);
}

NV_STATUS kfspProcessNvdmMessage_GH100(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size);

static inline NV_STATUS kfspProcessNvdmMessage_395e98(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kfspProcessNvdmMessage_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size) {
    return pKernelFsp->__kfspProcessNvdmMessage__(pGpu, pKernelFsp, pBuffer, size);
}

NV_STATUS kfspProcessCommandResponse_GH100(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size);

static inline NV_STATUS kfspProcessCommandResponse_395e98(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kfspProcessCommandResponse_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU8 *pBuffer, NvU32 size) {
    return pKernelFsp->__kfspProcessCommandResponse__(pGpu, pKernelFsp, pBuffer, size);
}

void kfspDumpDebugState_GH100(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline void kfspDumpDebugState_d44104(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return;
}

static inline void kfspDumpDebugState_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    pKernelFsp->__kfspDumpDebugState__(pGpu, pKernelFsp);
}

NV_STATUS kfspErrorCode2NvStatusMap_GH100(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 errorCode);

static inline NV_STATUS kfspErrorCode2NvStatusMap_395e98(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 errorCode) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kfspErrorCode2NvStatusMap_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp, NvU32 errorCode) {
    return pKernelFsp->__kfspErrorCode2NvStatusMap__(pGpu, pKernelFsp, errorCode);
}

NvU64 kfspGetExtraReservedMemorySize_GH100(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline NvU64 kfspGetExtraReservedMemorySize_4a4dee(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return 0;
}

static inline NvU64 kfspGetExtraReservedMemorySize_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return pKernelFsp->__kfspGetExtraReservedMemorySize__(pGpu, pKernelFsp);
}

NvBool kfspCheckGspSecureScratch_GH100(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline NvBool kfspCheckGspSecureScratch_491d52(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return ((NvBool)(0 != 0));
}

static inline NvBool kfspCheckGspSecureScratch_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return pKernelFsp->__kfspCheckGspSecureScratch__(pGpu, pKernelFsp);
}

NV_STATUS kfspWaitForGspTargetMaskReleased_GH100(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline NV_STATUS kfspWaitForGspTargetMaskReleased_395e98(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kfspWaitForGspTargetMaskReleased_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return pKernelFsp->__kfspWaitForGspTargetMaskReleased__(pGpu, pKernelFsp);
}

NvBool kfspRequiresBug3957833WAR_GH100(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp);

static inline NvBool kfspRequiresBug3957833WAR_491d52(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return ((NvBool)(0 != 0));
}

static inline NvBool kfspRequiresBug3957833WAR_DISPATCH(struct OBJGPU *pGpu, struct KernelFsp *pKernelFsp) {
    return pKernelFsp->__kfspRequiresBug3957833WAR__(pGpu, pKernelFsp);
}

static inline NV_STATUS kfspStateLoad_DISPATCH(POBJGPU pGpu, struct KernelFsp *pEngstate, NvU32 arg0) {
    return pEngstate->__kfspStateLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kfspStateUnload_DISPATCH(POBJGPU pGpu, struct KernelFsp *pEngstate, NvU32 arg0) {
    return pEngstate->__kfspStateUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kfspStateInitLocked_DISPATCH(POBJGPU pGpu, struct KernelFsp *pEngstate) {
    return pEngstate->__kfspStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kfspStatePreLoad_DISPATCH(POBJGPU pGpu, struct KernelFsp *pEngstate, NvU32 arg0) {
    return pEngstate->__kfspStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kfspStatePostUnload_DISPATCH(POBJGPU pGpu, struct KernelFsp *pEngstate, NvU32 arg0) {
    return pEngstate->__kfspStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kfspStatePreUnload_DISPATCH(POBJGPU pGpu, struct KernelFsp *pEngstate, NvU32 arg0) {
    return pEngstate->__kfspStatePreUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kfspStateInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelFsp *pEngstate) {
    return pEngstate->__kfspStateInitUnlocked__(pGpu, pEngstate);
}

static inline void kfspInitMissing_DISPATCH(POBJGPU pGpu, struct KernelFsp *pEngstate) {
    pEngstate->__kfspInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kfspStatePreInitLocked_DISPATCH(POBJGPU pGpu, struct KernelFsp *pEngstate) {
    return pEngstate->__kfspStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kfspStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelFsp *pEngstate) {
    return pEngstate->__kfspStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kfspStatePostLoad_DISPATCH(POBJGPU pGpu, struct KernelFsp *pEngstate, NvU32 arg0) {
    return pEngstate->__kfspStatePostLoad__(pGpu, pEngstate, arg0);
}

static inline NvBool kfspIsPresent_DISPATCH(POBJGPU pGpu, struct KernelFsp *pEngstate) {
    return pEngstate->__kfspIsPresent__(pGpu, pEngstate);
}

#undef PRIVATE_FIELD


#endif // KERN_FSP_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERN_FSP_NVOC_H_
