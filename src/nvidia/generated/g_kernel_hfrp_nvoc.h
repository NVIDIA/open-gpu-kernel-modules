
#ifndef _G_KERNEL_HFRP_NVOC_H_
#define _G_KERNEL_HFRP_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2024-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_kernel_hfrp_nvoc.h"

#ifndef _KERNELHFRP_H_
#define _KERNELHFRP_H_

#include "nvtypes.h"
#include "nvstatus.h"
#include "nvmisc.h"
#include "utils/nvprintf.h"

#include "os/os.h"
#include "gpu/eng_state.h"
#include "gpu/gpu.h"
#include "gpu/hfrp/kern_hfrp_common.h"

// Total number of HFRP Mailboxes available for the interface
#define HFRP_NUMBER_OF_MAILBOXES            2U

// Maximum Payload size for a message
#define HFRP_MAX_PAYLOAD_SIZE               50U

#define HFRP_COMMAND_MAILBOX_INDEX          HFRP_COMMAND_MAILBOX_INDEX_DISPLAY
#define HFRP_RESPONSE_MAILBOX_INDEX         HFRP_RESPONSE_MAILBOX_INDEX_DISPLAY

//
// Maximum values of Sequence Id index and Sequence Id Array index (each
// Sequence Id array element has 32 bits that represent 32 Sequence Ids)
//
#define HFRP_NUMBER_OF_SEQUENCEID_INDEX             0x400
#define HFRP_NUMBER_OF_SEQUENCEID_ARRAY_INDEX       (HFRP_NUMBER_OF_SEQUENCEID_INDEX / 32U)
#define HFRP_ASYNC_NOTIFICATION_SEQUENCEID_INDEX    0x3FF


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_HFRP_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelHFRP;
struct NVOC_METADATA__OBJENGSTATE;
struct NVOC_VTABLE__KernelHFRP;


struct KernelHFRP {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelHFRP *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct KernelHFRP *__nvoc_pbase_KernelHFRP;    // khfrp

    // 3 PDB properties
    NvBool PDB_PROP_KHFRP_IS_ENABLED;
    NvBool PDB_PROP_KHFRP_HDA_IS_ENABLED;
//  NvBool PDB_PROP_KHFRP_IS_MISSING inherited from OBJENGSTATE

    // Data members
    NvU32 khfrpPrivBase[5];
    NvU32 khfrpIntrCtrlReg[5];
    struct IoAperture *pAperture[2];
    HFRP_INFO khfrpInfo;
};


// Vtable with 14 per-class function pointers
struct NVOC_VTABLE__KernelHFRP {
    NV_STATUS (*__khfrpStatePreInitLocked__)(struct OBJGPU *, struct KernelHFRP * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__khfrpConstructEngine__)(struct OBJGPU *, struct KernelHFRP * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    void (*__khfrpInitMissing__)(struct OBJGPU *, struct KernelHFRP * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__khfrpStatePreInitUnlocked__)(struct OBJGPU *, struct KernelHFRP * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__khfrpStateInitLocked__)(struct OBJGPU *, struct KernelHFRP * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__khfrpStateInitUnlocked__)(struct OBJGPU *, struct KernelHFRP * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__khfrpStatePreLoad__)(struct OBJGPU *, struct KernelHFRP * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__khfrpStateLoad__)(struct OBJGPU *, struct KernelHFRP * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__khfrpStatePostLoad__)(struct OBJGPU *, struct KernelHFRP * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__khfrpStatePreUnload__)(struct OBJGPU *, struct KernelHFRP * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__khfrpStateUnload__)(struct OBJGPU *, struct KernelHFRP * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__khfrpStatePostUnload__)(struct OBJGPU *, struct KernelHFRP * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    void (*__khfrpStateDestroy__)(struct OBJGPU *, struct KernelHFRP * /*this*/);  // virtual inherited (engstate) base (engstate)
    NvBool (*__khfrpIsPresent__)(struct OBJGPU *, struct KernelHFRP * /*this*/);  // virtual inherited (engstate) base (engstate)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelHFRP {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
    const struct NVOC_VTABLE__KernelHFRP vtable;
};

#ifndef __NVOC_CLASS_KernelHFRP_TYPEDEF__
#define __NVOC_CLASS_KernelHFRP_TYPEDEF__
typedef struct KernelHFRP KernelHFRP;
#endif /* __NVOC_CLASS_KernelHFRP_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelHFRP
#define __nvoc_class_id_KernelHFRP 0xa9fc13
#endif /* __nvoc_class_id_KernelHFRP */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelHFRP;

#define __staticCast_KernelHFRP(pThis) \
    ((pThis)->__nvoc_pbase_KernelHFRP)

#ifdef __nvoc_kernel_hfrp_h_disabled
#define __dynamicCast_KernelHFRP(pThis) ((KernelHFRP*) NULL)
#else //__nvoc_kernel_hfrp_h_disabled
#define __dynamicCast_KernelHFRP(pThis) \
    ((KernelHFRP*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelHFRP)))
#endif //__nvoc_kernel_hfrp_h_disabled

// Property macros
#define PDB_PROP_KHFRP_IS_ENABLED_BASE_CAST
#define PDB_PROP_KHFRP_IS_ENABLED_BASE_NAME PDB_PROP_KHFRP_IS_ENABLED
#define PDB_PROP_KHFRP_HDA_IS_ENABLED_BASE_CAST
#define PDB_PROP_KHFRP_HDA_IS_ENABLED_BASE_NAME PDB_PROP_KHFRP_HDA_IS_ENABLED
#define PDB_PROP_KHFRP_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KHFRP_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING


NV_STATUS __nvoc_objCreateDynamic_KernelHFRP(KernelHFRP**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelHFRP(KernelHFRP**, Dynamic*, NvU32);
#define __objCreate_KernelHFRP(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelHFRP((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros for implementation functions
void khfrpDestruct_IMPL(struct KernelHFRP *pHfrp);
#define __nvoc_khfrpDestruct(pHfrp) khfrpDestruct_IMPL(pHfrp)

void khfrpCommonConstruct_IMPL(struct KernelHFRP *pHfrp);
#ifdef __nvoc_kernel_hfrp_h_disabled
static inline void khfrpCommonConstruct(struct KernelHFRP *pHfrp) {
    NV_ASSERT_FAILED_PRECOMP("KernelHFRP was disabled!");
}
#else // __nvoc_kernel_hfrp_h_disabled
#define khfrpCommonConstruct(pHfrp) khfrpCommonConstruct_IMPL(pHfrp)
#endif // __nvoc_kernel_hfrp_h_disabled

NV_STATUS khfrpIoApertureConstruct_IMPL(struct OBJGPU *pGpu, struct KernelHFRP *pHfrp);
#ifdef __nvoc_kernel_hfrp_h_disabled
static inline NV_STATUS khfrpIoApertureConstruct(struct OBJGPU *pGpu, struct KernelHFRP *pHfrp) {
    NV_ASSERT_FAILED_PRECOMP("KernelHFRP was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_hfrp_h_disabled
#define khfrpIoApertureConstruct(pGpu, pHfrp) khfrpIoApertureConstruct_IMPL(pGpu, pHfrp)
#endif // __nvoc_kernel_hfrp_h_disabled

void khfrpIoApertureDestruct_IMPL(struct KernelHFRP *pHfrp, NvU32 index);
#ifdef __nvoc_kernel_hfrp_h_disabled
static inline void khfrpIoApertureDestruct(struct KernelHFRP *pHfrp, NvU32 index) {
    NV_ASSERT_FAILED_PRECOMP("KernelHFRP was disabled!");
}
#else // __nvoc_kernel_hfrp_h_disabled
#define khfrpIoApertureDestruct(pHfrp, index) khfrpIoApertureDestruct_IMPL(pHfrp, index)
#endif // __nvoc_kernel_hfrp_h_disabled

NvU32 khfrpReadBit_IMPL(struct KernelHFRP *pHfrp, NvU32 virtualAddr, NvU32 bitIndex);
#ifdef __nvoc_kernel_hfrp_h_disabled
static inline NvU32 khfrpReadBit(struct KernelHFRP *pHfrp, NvU32 virtualAddr, NvU32 bitIndex) {
    NV_ASSERT_FAILED_PRECOMP("KernelHFRP was disabled!");
    return 0;
}
#else // __nvoc_kernel_hfrp_h_disabled
#define khfrpReadBit(pHfrp, virtualAddr, bitIndex) khfrpReadBit_IMPL(pHfrp, virtualAddr, bitIndex)
#endif // __nvoc_kernel_hfrp_h_disabled

void khfrpWriteBit_IMPL(struct KernelHFRP *pHfrp, NvU32 virtualAddr, NvU32 bitIndex, NvU32 data);
#ifdef __nvoc_kernel_hfrp_h_disabled
static inline void khfrpWriteBit(struct KernelHFRP *pHfrp, NvU32 virtualAddr, NvU32 bitIndex, NvU32 data) {
    NV_ASSERT_FAILED_PRECOMP("KernelHFRP was disabled!");
}
#else // __nvoc_kernel_hfrp_h_disabled
#define khfrpWriteBit(pHfrp, virtualAddr, bitIndex, data) khfrpWriteBit_IMPL(pHfrp, virtualAddr, bitIndex, data)
#endif // __nvoc_kernel_hfrp_h_disabled

NV_STATUS khfrpMailboxQueueMessage_IMPL(struct KernelHFRP *pHfrp, NvU32 messageHeader, NvU8 *pPayloadArray, NvU32 payloadSize, NvU32 mailboxFlag);
#ifdef __nvoc_kernel_hfrp_h_disabled
static inline NV_STATUS khfrpMailboxQueueMessage(struct KernelHFRP *pHfrp, NvU32 messageHeader, NvU8 *pPayloadArray, NvU32 payloadSize, NvU32 mailboxFlag) {
    NV_ASSERT_FAILED_PRECOMP("KernelHFRP was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_hfrp_h_disabled
#define khfrpMailboxQueueMessage(pHfrp, messageHeader, pPayloadArray, payloadSize, mailboxFlag) khfrpMailboxQueueMessage_IMPL(pHfrp, messageHeader, pPayloadArray, payloadSize, mailboxFlag)
#endif // __nvoc_kernel_hfrp_h_disabled

NV_STATUS khfrpServiceEvent_IMPL(struct KernelHFRP *pHfrp);
#ifdef __nvoc_kernel_hfrp_h_disabled
static inline NV_STATUS khfrpServiceEvent(struct KernelHFRP *pHfrp) {
    NV_ASSERT_FAILED_PRECOMP("KernelHFRP was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_hfrp_h_disabled
#define khfrpServiceEvent(pHfrp) khfrpServiceEvent_IMPL(pHfrp)
#endif // __nvoc_kernel_hfrp_h_disabled

NvU32 khfrpAllocateSequenceId_IMPL(struct KernelHFRP *pHfrp, NvU16 *pResponseStatus, void *pResponsePayload, NvU32 *pResponsePayloadSize, NV_STATUS *pStatus, NvU32 *pSequenceId);
#ifdef __nvoc_kernel_hfrp_h_disabled
static inline NvU32 khfrpAllocateSequenceId(struct KernelHFRP *pHfrp, NvU16 *pResponseStatus, void *pResponsePayload, NvU32 *pResponsePayloadSize, NV_STATUS *pStatus, NvU32 *pSequenceId) {
    NV_ASSERT_FAILED_PRECOMP("KernelHFRP was disabled!");
    return 0;
}
#else // __nvoc_kernel_hfrp_h_disabled
#define khfrpAllocateSequenceId(pHfrp, pResponseStatus, pResponsePayload, pResponsePayloadSize, pStatus, pSequenceId) khfrpAllocateSequenceId_IMPL(pHfrp, pResponseStatus, pResponsePayload, pResponsePayloadSize, pStatus, pSequenceId)
#endif // __nvoc_kernel_hfrp_h_disabled

void khfrpFreeSequenceId_IMPL(struct KernelHFRP *pHfrp, NvU32 index);
#ifdef __nvoc_kernel_hfrp_h_disabled
static inline void khfrpFreeSequenceId(struct KernelHFRP *pHfrp, NvU32 index) {
    NV_ASSERT_FAILED_PRECOMP("KernelHFRP was disabled!");
}
#else // __nvoc_kernel_hfrp_h_disabled
#define khfrpFreeSequenceId(pHfrp, index) khfrpFreeSequenceId_IMPL(pHfrp, index)
#endif // __nvoc_kernel_hfrp_h_disabled

NvBool khfrpIsSequenceIdFree_IMPL(struct KernelHFRP *pHfrp, NvU32 index);
#ifdef __nvoc_kernel_hfrp_h_disabled
static inline NvBool khfrpIsSequenceIdFree(struct KernelHFRP *pHfrp, NvU32 index) {
    NV_ASSERT_FAILED_PRECOMP("KernelHFRP was disabled!");
    return NV_FALSE;
}
#else // __nvoc_kernel_hfrp_h_disabled
#define khfrpIsSequenceIdFree(pHfrp, index) khfrpIsSequenceIdFree_IMPL(pHfrp, index)
#endif // __nvoc_kernel_hfrp_h_disabled

NV_STATUS khfrpPollOnIrqWrapper_IMPL(struct KernelHFRP *pHfrp, NvU32 irqRegAddr, NvU32 bitIndex, NvBool bData);
#ifdef __nvoc_kernel_hfrp_h_disabled
static inline NV_STATUS khfrpPollOnIrqWrapper(struct KernelHFRP *pHfrp, NvU32 irqRegAddr, NvU32 bitIndex, NvBool bData) {
    NV_ASSERT_FAILED_PRECOMP("KernelHFRP was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_hfrp_h_disabled
#define khfrpPollOnIrqWrapper(pHfrp, irqRegAddr, bitIndex, bData) khfrpPollOnIrqWrapper_IMPL(pHfrp, irqRegAddr, bitIndex, bData)
#endif // __nvoc_kernel_hfrp_h_disabled

NV_STATUS khfrpPollOnIrqRm_IMPL(struct KernelHFRP *pHfrp, NvU32 irqRegAddr, NvU32 bitIndex, NvBool bData);
#ifdef __nvoc_kernel_hfrp_h_disabled
static inline NV_STATUS khfrpPollOnIrqRm(struct KernelHFRP *pHfrp, NvU32 irqRegAddr, NvU32 bitIndex, NvBool bData) {
    NV_ASSERT_FAILED_PRECOMP("KernelHFRP was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_hfrp_h_disabled
#define khfrpPollOnIrqRm(pHfrp, irqRegAddr, bitIndex, bData) khfrpPollOnIrqRm_IMPL(pHfrp, irqRegAddr, bitIndex, bData)
#endif // __nvoc_kernel_hfrp_h_disabled

NV_STATUS khfrpPostCommandBlocking_IMPL(struct KernelHFRP *pHfrp, NvU16 commandIndex, void *pCommandPayload, NvU32 commandPayloadSize, NvU16 *pResponseStatus, void *pResponsePayload, NvU32 *pResponsePayloadSize);
#ifdef __nvoc_kernel_hfrp_h_disabled
static inline NV_STATUS khfrpPostCommandBlocking(struct KernelHFRP *pHfrp, NvU16 commandIndex, void *pCommandPayload, NvU32 commandPayloadSize, NvU16 *pResponseStatus, void *pResponsePayload, NvU32 *pResponsePayloadSize) {
    NV_ASSERT_FAILED_PRECOMP("KernelHFRP was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_hfrp_h_disabled
#define khfrpPostCommandBlocking(pHfrp, commandIndex, pCommandPayload, commandPayloadSize, pResponseStatus, pResponsePayload, pResponsePayloadSize) khfrpPostCommandBlocking_IMPL(pHfrp, commandIndex, pCommandPayload, commandPayloadSize, pResponseStatus, pResponsePayload, pResponsePayloadSize)
#endif // __nvoc_kernel_hfrp_h_disabled

NV_STATUS khfrpInterfaceReset_IMPL(struct KernelHFRP *pHfrp);
#ifdef __nvoc_kernel_hfrp_h_disabled
static inline NV_STATUS khfrpInterfaceReset(struct KernelHFRP *pHfrp) {
    NV_ASSERT_FAILED_PRECOMP("KernelHFRP was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_hfrp_h_disabled
#define khfrpInterfaceReset(pHfrp) khfrpInterfaceReset_IMPL(pHfrp)
#endif // __nvoc_kernel_hfrp_h_disabled


// Wrapper macros for halified functions
#define khfrpStatePreInitLocked_FNPTR(pHfrp) pHfrp->__nvoc_metadata_ptr->vtable.__khfrpStatePreInitLocked__
#define khfrpStatePreInitLocked(pGpu, pHfrp) khfrpStatePreInitLocked_DISPATCH(pGpu, pHfrp)
#define khfrpConstructEngine_FNPTR(pHfrp) pHfrp->__nvoc_metadata_ptr->vtable.__khfrpConstructEngine__
#define khfrpConstructEngine(pGpu, pHfrp, engDesc) khfrpConstructEngine_DISPATCH(pGpu, pHfrp, engDesc)
#define khfrpInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define khfrpInitMissing(pGpu, pEngstate) khfrpInitMissing_DISPATCH(pGpu, pEngstate)
#define khfrpStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define khfrpStatePreInitUnlocked(pGpu, pEngstate) khfrpStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define khfrpStateInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitLocked__
#define khfrpStateInitLocked(pGpu, pEngstate) khfrpStateInitLocked_DISPATCH(pGpu, pEngstate)
#define khfrpStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__
#define khfrpStateInitUnlocked(pGpu, pEngstate) khfrpStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define khfrpStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__
#define khfrpStatePreLoad(pGpu, pEngstate, arg3) khfrpStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define khfrpStateLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateLoad__
#define khfrpStateLoad(pGpu, pEngstate, arg3) khfrpStateLoad_DISPATCH(pGpu, pEngstate, arg3)
#define khfrpStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostLoad__
#define khfrpStatePostLoad(pGpu, pEngstate, arg3) khfrpStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define khfrpStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__
#define khfrpStatePreUnload(pGpu, pEngstate, arg3) khfrpStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define khfrpStateUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateUnload__
#define khfrpStateUnload(pGpu, pEngstate, arg3) khfrpStateUnload_DISPATCH(pGpu, pEngstate, arg3)
#define khfrpStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__
#define khfrpStatePostUnload(pGpu, pEngstate, arg3) khfrpStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define khfrpStateDestroy_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateDestroy__
#define khfrpStateDestroy(pGpu, pEngstate) khfrpStateDestroy_DISPATCH(pGpu, pEngstate)
#define khfrpIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__
#define khfrpIsPresent(pGpu, pEngstate) khfrpIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS khfrpStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelHFRP *pHfrp) {
    return pHfrp->__nvoc_metadata_ptr->vtable.__khfrpStatePreInitLocked__(pGpu, pHfrp);
}

static inline NV_STATUS khfrpConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelHFRP *pHfrp, ENGDESCRIPTOR engDesc) {
    return pHfrp->__nvoc_metadata_ptr->vtable.__khfrpConstructEngine__(pGpu, pHfrp, engDesc);
}

static inline void khfrpInitMissing_DISPATCH(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__khfrpInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS khfrpStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__khfrpStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS khfrpStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__khfrpStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS khfrpStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__khfrpStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS khfrpStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__khfrpStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS khfrpStateLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__khfrpStateLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS khfrpStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__khfrpStatePostLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS khfrpStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__khfrpStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS khfrpStateUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__khfrpStateUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS khfrpStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__khfrpStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline void khfrpStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__khfrpStateDestroy__(pGpu, pEngstate);
}

static inline NvBool khfrpIsPresent_DISPATCH(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__khfrpIsPresent__(pGpu, pEngstate);
}

NV_STATUS khfrpStatePreInitLocked_IMPL(struct OBJGPU *pGpu, struct KernelHFRP *pHfrp);

NV_STATUS khfrpConstructEngine_IMPL(struct OBJGPU *pGpu, struct KernelHFRP *pHfrp, ENGDESCRIPTOR engDesc);

#undef PRIVATE_FIELD


#define HFRP_REG_RD32(pKernelHfrp, virtualAddr)                                \
    REG_RD32(pKernelHfrp->pAperture[virtualAddr / HFRP_MAILBOX_ACCESS_RANGE],  \
             virtualAddr % HFRP_MAILBOX_ACCESS_RANGE)

#define HFRP_REG_WR32(pKernelHfrp, virtualAddr, data32)                        \
    REG_WR32(pKernelHfrp->pAperture[virtualAddr / HFRP_MAILBOX_ACCESS_RANGE],  \
             virtualAddr % HFRP_MAILBOX_ACCESS_RANGE, data32)

#define HFRP_POLL_ON_IRQ(pKernelHfrp, irqRegAddr, bitIndex, bData)             \
    khfrpPollOnIrqRm(pKernelHfrp, irqRegAddr, bitIndex, bData)

#endif  // _KernelHFRP_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_HFRP_NVOC_H_
