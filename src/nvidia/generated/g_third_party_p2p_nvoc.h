
#ifndef _G_THIRD_PARTY_P2P_NVOC_H_
#define _G_THIRD_PARTY_P2P_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2009-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_third_party_p2p_nvoc.h"

#ifndef _THIRD_PARTY_P2P_H_
#define _THIRD_PARTY_P2P_H_

#include "rmapi/client.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/gpu_resource.h"

#include <ctrl/ctrl503c.h>


struct Subdevice;

#ifndef __NVOC_CLASS_Subdevice_TYPEDEF__
#define __NVOC_CLASS_Subdevice_TYPEDEF__
typedef struct Subdevice Subdevice;
#endif /* __NVOC_CLASS_Subdevice_TYPEDEF__ */

#ifndef __nvoc_class_id_Subdevice
#define __nvoc_class_id_Subdevice 0x4b01b3
#endif /* __nvoc_class_id_Subdevice */



struct Memory;

#ifndef __NVOC_CLASS_Memory_TYPEDEF__
#define __NVOC_CLASS_Memory_TYPEDEF__
typedef struct Memory Memory;
#endif /* __NVOC_CLASS_Memory_TYPEDEF__ */

#ifndef __nvoc_class_id_Memory
#define __nvoc_class_id_Memory 0x4789f2
#endif /* __nvoc_class_id_Memory */



#define CLI_THIRD_PARTY_P2P_FLAGS_INITIALIZED   NVBIT(0)

#define CLI_THIRD_PARTY_P2P_MAX_CLIENT 256

//
// CUDA tools has requested that the third-party P2P code reserve at least
// 32MB of BAR1 address space for RM clients. Pad this reservation by
// 4MB to account for miscellaneous RM mappings.
//
#define CLI_THIRD_PARTY_P2P_BAR1_RESERVE (36 << 20)

// ****************************************************************************
//                          Type definitions
// ****************************************************************************

//
// third-party p2p support types
//
struct _def_client_third_party_p2p_vaspace_info
{
    NvHandle                 hClient;
    NvHandle                 hThirdPartyP2P;
    NvHandle                 hVASpace;
    NvU32                    vaSpaceToken;
};
typedef struct _def_client_third_party_p2p_vaspace_info CLI_THIRD_PARTY_P2P_VASPACE_INFO, *PCLI_THIRD_PARTY_P2P_VASPACE_INFO;

MAKE_MAP(CLI_THIRD_PARTY_P2P_VASPACE_INFO_MAP, CLI_THIRD_PARTY_P2P_VASPACE_INFO);

typedef void (THIRD_PARTY_P2P_VIDMEM_FREE_CALLBACK)(void *);

struct _def_client_third_party_p2p_mapping_info
{
    NODE                                                     Node;
    THIRD_PARTY_P2P_VIDMEM_FREE_CALLBACK                    *pFreeCallback;
    void                                                    *pData;

    // Address and length describe a subrange of the parent vidmem info address range
    NvU64                                                    address;
    NvU64                                                    length;

    struct _def_client_third_party_p2p_mapping_extent_info  *pStart;
};
typedef struct _def_client_third_party_p2p_mapping_info CLI_THIRD_PARTY_P2P_MAPPING_INFO, *PCLI_THIRD_PARTY_P2P_MAPPING_INFO;

struct _def_client_third_party_p2p_mapping_extent_info
{
    // Address and length describe a subrange of the parent vidmem info address range
    NvU64                    address;
    NvU64                    length;

    // Memory Area scatterlist of BAR1 PAs
    MemoryArea               memArea;

    //
    // VGPU RPC returns contiguous range, so we preallocate range instead of portMemAlloc
    // TODO: look into unifying VGPU/non-vgpu cases
    //
    MemoryRange              vgpuRange;

    PMEMORY_DESCRIPTOR       pMemDesc;
    NvU32                    refCount;
    ListNode                 listNode;  // Node in the list.
};
typedef struct _def_client_third_party_p2p_mapping_extent_info CLI_THIRD_PARTY_P2P_MAPPING_EXTENT_INFO, *PCLI_THIRD_PARTY_P2P_MAPPING_EXTENT_INFO;

MAKE_INTRUSIVE_LIST(CLI_THIRD_PARTY_P2P_MAPPING_EXTENT_INFO_LIST, CLI_THIRD_PARTY_P2P_MAPPING_EXTENT_INFO, listNode);

struct _def_client_third_party_p2p_vidmem_info
{
    NvHandle                   hClient;
    NvHandle                   hThirdPartyP2P;
    NvHandle                   hMemory;

    //
    // A node in the PCLI_THIRD_PARTY_P2P_INFO::pAddressRangeTree btree tracking
    // the address range registered for this vidmem allocation.
    //
    // Notably the address ranges come from the user and are not enforced to
    // be mapped in any GPU VA space. It's up to the user to pick them and they
    // are used purely for exposing the registered vidmem allocations through
    // the third-party P2P APIs at the specified ranges. See documentation of
    // NV503C_CTRL_CMD_REGISTER_VIDMEM for more details.
    //
    // Commonly clients will map the allocation at the same address as it's
    // registered with for third-party P2P and the third-party P2P APIs still
    // call the address parameter a virtual address, but the implementation
    // refers to them just as addresses to make it clear they are not enforced
    // to be actually mapped.
    //
    // Notably in the past the addresses have been indeed looked up in the GPU
    // VA spaces directly, but that became challenging with the introduction of
    // externally managed VA spaces and now it's up to the clients to register
    // them explicitly.
    //
    NODE                                         addressRangeNode;
    MapNode                                      mapNode;
    // Offset at which the address range starts in the vidmem allocation.
    NvU64                                        offset;

    // VidmemInfo ID used for persistent mappings
    NvU64                                        id;

    PMEMORY_DESCRIPTOR                           pMemDesc;
    PNODE                                        pMappingInfoList;
    CLI_THIRD_PARTY_P2P_MAPPING_EXTENT_INFO_LIST mappingExtentList;
};
typedef struct _def_client_third_party_p2p_vidmem_info  CLI_THIRD_PARTY_P2P_VIDMEM_INFO, *PCLI_THIRD_PARTY_P2P_VIDMEM_INFO;

MAKE_INTRUSIVE_MAP(CLI_THIRD_PARTY_P2P_VIDMEM_INFO_MAP, CLI_THIRD_PARTY_P2P_VIDMEM_INFO, mapNode);

typedef void (THIRD_PARTY_P2P_DESTROY_CALLBACK)(void *);

enum _def_client_third_party_p2p_type
{
    CLI_THIRD_PARTY_P2P_TYPE_PROPRIETARY,
    CLI_THIRD_PARTY_P2P_TYPE_BAR1,
    CLI_THIRD_PARTY_P2P_TYPE_NVLINK
};
typedef enum   _def_client_third_party_p2p_type     CLI_THIRD_PARTY_P2P_TYPE;

struct _def_client_third_party_p2p_pid_client_mapping_info
{
    NvU32       pid;
    NvHandle    hClient;
};
typedef struct _def_client_third_party_p2p_pid_client_mapping_info CLI_THIRD_PARTY_P2P_PID_CLIENT_INFO, *PCLI_THIRD_PARTY_P2P_PID_CLIENT_INFO;


struct ThirdPartyP2P;

#ifndef __NVOC_CLASS_ThirdPartyP2P_TYPEDEF__
#define __NVOC_CLASS_ThirdPartyP2P_TYPEDEF__
typedef struct ThirdPartyP2P ThirdPartyP2P;
#endif /* __NVOC_CLASS_ThirdPartyP2P_TYPEDEF__ */

#ifndef __nvoc_class_id_ThirdPartyP2P
#define __nvoc_class_id_ThirdPartyP2P 0x34d08b
#endif /* __nvoc_class_id_ThirdPartyP2P */



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_THIRD_PARTY_P2P_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__P2PTokenShare;
struct NVOC_METADATA__RsShared;


struct P2PTokenShare {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__P2PTokenShare *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RsShared __nvoc_base_RsShared;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct RsShared *__nvoc_pbase_RsShared;    // shr super
    struct P2PTokenShare *__nvoc_pbase_P2PTokenShare;    // shrp2p

    // Data members
    struct ThirdPartyP2P *pThirdPartyP2P;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__P2PTokenShare {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RsShared metadata__RsShared;
};

#ifndef __NVOC_CLASS_P2PTokenShare_TYPEDEF__
#define __NVOC_CLASS_P2PTokenShare_TYPEDEF__
typedef struct P2PTokenShare P2PTokenShare;
#endif /* __NVOC_CLASS_P2PTokenShare_TYPEDEF__ */

#ifndef __nvoc_class_id_P2PTokenShare
#define __nvoc_class_id_P2PTokenShare 0x3e3a6a
#endif /* __nvoc_class_id_P2PTokenShare */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_P2PTokenShare;

#define __staticCast_P2PTokenShare(pThis) \
    ((pThis)->__nvoc_pbase_P2PTokenShare)

#ifdef __nvoc_third_party_p2p_h_disabled
#define __dynamicCast_P2PTokenShare(pThis) ((P2PTokenShare*) NULL)
#else //__nvoc_third_party_p2p_h_disabled
#define __dynamicCast_P2PTokenShare(pThis) \
    ((P2PTokenShare*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(P2PTokenShare)))
#endif //__nvoc_third_party_p2p_h_disabled

NV_STATUS __nvoc_objCreateDynamic_P2PTokenShare(P2PTokenShare**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_P2PTokenShare(P2PTokenShare**, Dynamic*, NvU32);
#define __objCreate_P2PTokenShare(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_P2PTokenShare((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
NV_STATUS shrp2pConstruct_IMPL(struct P2PTokenShare *arg_pP2PTokenShare);

#define __nvoc_shrp2pConstruct(arg_pP2PTokenShare) shrp2pConstruct_IMPL(arg_pP2PTokenShare)
void shrp2pDestruct_IMPL(struct P2PTokenShare *pP2PTokenShare);

#define __nvoc_shrp2pDestruct(pP2PTokenShare) shrp2pDestruct_IMPL(pP2PTokenShare)
#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_THIRD_PARTY_P2P_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__ThirdPartyP2P;
struct NVOC_METADATA__GpuResource;
struct NVOC_VTABLE__ThirdPartyP2P;


struct ThirdPartyP2P {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__ThirdPartyP2P *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResource __nvoc_base_GpuResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super
    struct ThirdPartyP2P *__nvoc_pbase_ThirdPartyP2P;    // thirdpartyp2p

    // Vtable with 5 per-object function pointers
    NV_STATUS (*__thirdpartyp2pCtrlCmdRegisterVaSpace__)(struct ThirdPartyP2P * /*this*/, NV503C_CTRL_REGISTER_VA_SPACE_PARAMS *);  // exported (id=0x503c0102)
    NV_STATUS (*__thirdpartyp2pCtrlCmdUnregisterVaSpace__)(struct ThirdPartyP2P * /*this*/, NV503C_CTRL_UNREGISTER_VA_SPACE_PARAMS *);  // exported (id=0x503c0103)
    NV_STATUS (*__thirdpartyp2pCtrlCmdRegisterVidmem__)(struct ThirdPartyP2P * /*this*/, NV503C_CTRL_REGISTER_VIDMEM_PARAMS *);  // exported (id=0x503c0104)
    NV_STATUS (*__thirdpartyp2pCtrlCmdUnregisterVidmem__)(struct ThirdPartyP2P * /*this*/, NV503C_CTRL_UNREGISTER_VIDMEM_PARAMS *);  // exported (id=0x503c0105)
    NV_STATUS (*__thirdpartyp2pCtrlCmdRegisterPid__)(struct ThirdPartyP2P * /*this*/, NV503C_CTRL_REGISTER_PID_PARAMS *);  // exported (id=0x503c0106)

    // Data members
    NODE Node;
    NvHandle hClient;
    NvHandle hThirdPartyP2P;
    CLI_THIRD_PARTY_P2P_TYPE type;
    struct Subdevice *pSubdevice;
    NvU32 peerIndex;
    NvU32 flags;
    NvU64 p2pToken;
    CLI_THIRD_PARTY_P2P_PID_CLIENT_INFO pidClientList[256];
    CLI_THIRD_PARTY_P2P_VASPACE_INFO_MAP vaSpaceInfoMap;
    THIRD_PARTY_P2P_DESTROY_CALLBACK *pDestroyCallback;
    void *pData;
    CLI_THIRD_PARTY_P2P_VIDMEM_INFO_MAP vidmemInfoMap;
    struct P2PTokenShare *pTokenShare;
    PNODE pAddressRangeTree;
};


// Vtable with 25 per-class function pointers
struct NVOC_VTABLE__ThirdPartyP2P {
    NV_STATUS (*__thirdpartyp2pControl__)(struct ThirdPartyP2P * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__thirdpartyp2pMap__)(struct ThirdPartyP2P * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__thirdpartyp2pUnmap__)(struct ThirdPartyP2P * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__thirdpartyp2pShareCallback__)(struct ThirdPartyP2P * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__thirdpartyp2pGetRegBaseOffsetAndSize__)(struct ThirdPartyP2P * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__thirdpartyp2pGetMapAddrSpace__)(struct ThirdPartyP2P * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__thirdpartyp2pInternalControlForward__)(struct ThirdPartyP2P * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__thirdpartyp2pGetInternalObjectHandle__)(struct ThirdPartyP2P * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__thirdpartyp2pAccessCallback__)(struct ThirdPartyP2P * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__thirdpartyp2pGetMemInterMapParams__)(struct ThirdPartyP2P * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__thirdpartyp2pCheckMemInterUnmap__)(struct ThirdPartyP2P * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__thirdpartyp2pGetMemoryMappingDescriptor__)(struct ThirdPartyP2P * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__thirdpartyp2pControlSerialization_Prologue__)(struct ThirdPartyP2P * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__thirdpartyp2pControlSerialization_Epilogue__)(struct ThirdPartyP2P * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__thirdpartyp2pControl_Prologue__)(struct ThirdPartyP2P * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__thirdpartyp2pControl_Epilogue__)(struct ThirdPartyP2P * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__thirdpartyp2pCanCopy__)(struct ThirdPartyP2P * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__thirdpartyp2pIsDuplicate__)(struct ThirdPartyP2P * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__thirdpartyp2pPreDestruct__)(struct ThirdPartyP2P * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__thirdpartyp2pControlFilter__)(struct ThirdPartyP2P * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__thirdpartyp2pIsPartialUnmapSupported__)(struct ThirdPartyP2P * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__thirdpartyp2pMapTo__)(struct ThirdPartyP2P * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__thirdpartyp2pUnmapFrom__)(struct ThirdPartyP2P * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__thirdpartyp2pGetRefCount__)(struct ThirdPartyP2P * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__thirdpartyp2pAddAdditionalDependants__)(struct RsClient *, struct ThirdPartyP2P * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__ThirdPartyP2P {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_VTABLE__ThirdPartyP2P vtable;
};

#ifndef __NVOC_CLASS_ThirdPartyP2P_TYPEDEF__
#define __NVOC_CLASS_ThirdPartyP2P_TYPEDEF__
typedef struct ThirdPartyP2P ThirdPartyP2P;
#endif /* __NVOC_CLASS_ThirdPartyP2P_TYPEDEF__ */

#ifndef __nvoc_class_id_ThirdPartyP2P
#define __nvoc_class_id_ThirdPartyP2P 0x34d08b
#endif /* __nvoc_class_id_ThirdPartyP2P */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_ThirdPartyP2P;

#define __staticCast_ThirdPartyP2P(pThis) \
    ((pThis)->__nvoc_pbase_ThirdPartyP2P)

#ifdef __nvoc_third_party_p2p_h_disabled
#define __dynamicCast_ThirdPartyP2P(pThis) ((ThirdPartyP2P*) NULL)
#else //__nvoc_third_party_p2p_h_disabled
#define __dynamicCast_ThirdPartyP2P(pThis) \
    ((ThirdPartyP2P*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(ThirdPartyP2P)))
#endif //__nvoc_third_party_p2p_h_disabled

NV_STATUS __nvoc_objCreateDynamic_ThirdPartyP2P(ThirdPartyP2P**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_ThirdPartyP2P(ThirdPartyP2P**, Dynamic*, NvU32, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_ThirdPartyP2P(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_ThirdPartyP2P((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define thirdpartyp2pCtrlCmdRegisterVaSpace_FNPTR(pThirdPartyP2P) pThirdPartyP2P->__thirdpartyp2pCtrlCmdRegisterVaSpace__
#define thirdpartyp2pCtrlCmdRegisterVaSpace(pThirdPartyP2P, pRegisterVaSpaceParams) thirdpartyp2pCtrlCmdRegisterVaSpace_DISPATCH(pThirdPartyP2P, pRegisterVaSpaceParams)
#define thirdpartyp2pCtrlCmdUnregisterVaSpace_FNPTR(pThirdPartyP2P) pThirdPartyP2P->__thirdpartyp2pCtrlCmdUnregisterVaSpace__
#define thirdpartyp2pCtrlCmdUnregisterVaSpace(pThirdPartyP2P, pUnregisterVaSpaceParams) thirdpartyp2pCtrlCmdUnregisterVaSpace_DISPATCH(pThirdPartyP2P, pUnregisterVaSpaceParams)
#define thirdpartyp2pCtrlCmdRegisterVidmem_FNPTR(pThirdPartyP2P) pThirdPartyP2P->__thirdpartyp2pCtrlCmdRegisterVidmem__
#define thirdpartyp2pCtrlCmdRegisterVidmem(pThirdPartyP2P, pRegisterVidmemParams) thirdpartyp2pCtrlCmdRegisterVidmem_DISPATCH(pThirdPartyP2P, pRegisterVidmemParams)
#define thirdpartyp2pCtrlCmdUnregisterVidmem_FNPTR(pThirdPartyP2P) pThirdPartyP2P->__thirdpartyp2pCtrlCmdUnregisterVidmem__
#define thirdpartyp2pCtrlCmdUnregisterVidmem(pThirdPartyP2P, pUnregisterVidmemParams) thirdpartyp2pCtrlCmdUnregisterVidmem_DISPATCH(pThirdPartyP2P, pUnregisterVidmemParams)
#define thirdpartyp2pCtrlCmdRegisterPid_FNPTR(pThirdPartyP2P) pThirdPartyP2P->__thirdpartyp2pCtrlCmdRegisterPid__
#define thirdpartyp2pCtrlCmdRegisterPid(pThirdPartyP2P, pParams) thirdpartyp2pCtrlCmdRegisterPid_DISPATCH(pThirdPartyP2P, pParams)
#define thirdpartyp2pControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresControl__
#define thirdpartyp2pControl(pGpuResource, pCallContext, pParams) thirdpartyp2pControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define thirdpartyp2pMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresMap__
#define thirdpartyp2pMap(pGpuResource, pCallContext, pParams, pCpuMapping) thirdpartyp2pMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define thirdpartyp2pUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define thirdpartyp2pUnmap(pGpuResource, pCallContext, pCpuMapping) thirdpartyp2pUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define thirdpartyp2pShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define thirdpartyp2pShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) thirdpartyp2pShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define thirdpartyp2pGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define thirdpartyp2pGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) thirdpartyp2pGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define thirdpartyp2pGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define thirdpartyp2pGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) thirdpartyp2pGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define thirdpartyp2pInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define thirdpartyp2pInternalControlForward(pGpuResource, command, pParams, size) thirdpartyp2pInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define thirdpartyp2pGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define thirdpartyp2pGetInternalObjectHandle(pGpuResource) thirdpartyp2pGetInternalObjectHandle_DISPATCH(pGpuResource)
#define thirdpartyp2pAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define thirdpartyp2pAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) thirdpartyp2pAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define thirdpartyp2pGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define thirdpartyp2pGetMemInterMapParams(pRmResource, pParams) thirdpartyp2pGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define thirdpartyp2pCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define thirdpartyp2pCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) thirdpartyp2pCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define thirdpartyp2pGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define thirdpartyp2pGetMemoryMappingDescriptor(pRmResource, ppMemDesc) thirdpartyp2pGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define thirdpartyp2pControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define thirdpartyp2pControlSerialization_Prologue(pResource, pCallContext, pParams) thirdpartyp2pControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define thirdpartyp2pControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define thirdpartyp2pControlSerialization_Epilogue(pResource, pCallContext, pParams) thirdpartyp2pControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define thirdpartyp2pControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define thirdpartyp2pControl_Prologue(pResource, pCallContext, pParams) thirdpartyp2pControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define thirdpartyp2pControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define thirdpartyp2pControl_Epilogue(pResource, pCallContext, pParams) thirdpartyp2pControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define thirdpartyp2pCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define thirdpartyp2pCanCopy(pResource) thirdpartyp2pCanCopy_DISPATCH(pResource)
#define thirdpartyp2pIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define thirdpartyp2pIsDuplicate(pResource, hMemory, pDuplicate) thirdpartyp2pIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define thirdpartyp2pPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define thirdpartyp2pPreDestruct(pResource) thirdpartyp2pPreDestruct_DISPATCH(pResource)
#define thirdpartyp2pControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define thirdpartyp2pControlFilter(pResource, pCallContext, pParams) thirdpartyp2pControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define thirdpartyp2pIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define thirdpartyp2pIsPartialUnmapSupported(pResource) thirdpartyp2pIsPartialUnmapSupported_DISPATCH(pResource)
#define thirdpartyp2pMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define thirdpartyp2pMapTo(pResource, pParams) thirdpartyp2pMapTo_DISPATCH(pResource, pParams)
#define thirdpartyp2pUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define thirdpartyp2pUnmapFrom(pResource, pParams) thirdpartyp2pUnmapFrom_DISPATCH(pResource, pParams)
#define thirdpartyp2pGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define thirdpartyp2pGetRefCount(pResource) thirdpartyp2pGetRefCount_DISPATCH(pResource)
#define thirdpartyp2pAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define thirdpartyp2pAddAdditionalDependants(pClient, pResource, pReference) thirdpartyp2pAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS thirdpartyp2pCtrlCmdRegisterVaSpace_DISPATCH(struct ThirdPartyP2P *pThirdPartyP2P, NV503C_CTRL_REGISTER_VA_SPACE_PARAMS *pRegisterVaSpaceParams) {
    return pThirdPartyP2P->__thirdpartyp2pCtrlCmdRegisterVaSpace__(pThirdPartyP2P, pRegisterVaSpaceParams);
}

static inline NV_STATUS thirdpartyp2pCtrlCmdUnregisterVaSpace_DISPATCH(struct ThirdPartyP2P *pThirdPartyP2P, NV503C_CTRL_UNREGISTER_VA_SPACE_PARAMS *pUnregisterVaSpaceParams) {
    return pThirdPartyP2P->__thirdpartyp2pCtrlCmdUnregisterVaSpace__(pThirdPartyP2P, pUnregisterVaSpaceParams);
}

static inline NV_STATUS thirdpartyp2pCtrlCmdRegisterVidmem_DISPATCH(struct ThirdPartyP2P *pThirdPartyP2P, NV503C_CTRL_REGISTER_VIDMEM_PARAMS *pRegisterVidmemParams) {
    return pThirdPartyP2P->__thirdpartyp2pCtrlCmdRegisterVidmem__(pThirdPartyP2P, pRegisterVidmemParams);
}

static inline NV_STATUS thirdpartyp2pCtrlCmdUnregisterVidmem_DISPATCH(struct ThirdPartyP2P *pThirdPartyP2P, NV503C_CTRL_UNREGISTER_VIDMEM_PARAMS *pUnregisterVidmemParams) {
    return pThirdPartyP2P->__thirdpartyp2pCtrlCmdUnregisterVidmem__(pThirdPartyP2P, pUnregisterVidmemParams);
}

static inline NV_STATUS thirdpartyp2pCtrlCmdRegisterPid_DISPATCH(struct ThirdPartyP2P *pThirdPartyP2P, NV503C_CTRL_REGISTER_PID_PARAMS *pParams) {
    return pThirdPartyP2P->__thirdpartyp2pCtrlCmdRegisterPid__(pThirdPartyP2P, pParams);
}

static inline NV_STATUS thirdpartyp2pControl_DISPATCH(struct ThirdPartyP2P *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__thirdpartyp2pControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS thirdpartyp2pMap_DISPATCH(struct ThirdPartyP2P *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__thirdpartyp2pMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS thirdpartyp2pUnmap_DISPATCH(struct ThirdPartyP2P *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__thirdpartyp2pUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool thirdpartyp2pShareCallback_DISPATCH(struct ThirdPartyP2P *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__thirdpartyp2pShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS thirdpartyp2pGetRegBaseOffsetAndSize_DISPATCH(struct ThirdPartyP2P *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__thirdpartyp2pGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS thirdpartyp2pGetMapAddrSpace_DISPATCH(struct ThirdPartyP2P *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__thirdpartyp2pGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS thirdpartyp2pInternalControlForward_DISPATCH(struct ThirdPartyP2P *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__thirdpartyp2pInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle thirdpartyp2pGetInternalObjectHandle_DISPATCH(struct ThirdPartyP2P *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__thirdpartyp2pGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool thirdpartyp2pAccessCallback_DISPATCH(struct ThirdPartyP2P *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__thirdpartyp2pAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS thirdpartyp2pGetMemInterMapParams_DISPATCH(struct ThirdPartyP2P *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__thirdpartyp2pGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS thirdpartyp2pCheckMemInterUnmap_DISPATCH(struct ThirdPartyP2P *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__thirdpartyp2pCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS thirdpartyp2pGetMemoryMappingDescriptor_DISPATCH(struct ThirdPartyP2P *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__thirdpartyp2pGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS thirdpartyp2pControlSerialization_Prologue_DISPATCH(struct ThirdPartyP2P *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__thirdpartyp2pControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void thirdpartyp2pControlSerialization_Epilogue_DISPATCH(struct ThirdPartyP2P *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__thirdpartyp2pControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS thirdpartyp2pControl_Prologue_DISPATCH(struct ThirdPartyP2P *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__thirdpartyp2pControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void thirdpartyp2pControl_Epilogue_DISPATCH(struct ThirdPartyP2P *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__thirdpartyp2pControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool thirdpartyp2pCanCopy_DISPATCH(struct ThirdPartyP2P *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__thirdpartyp2pCanCopy__(pResource);
}

static inline NV_STATUS thirdpartyp2pIsDuplicate_DISPATCH(struct ThirdPartyP2P *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__thirdpartyp2pIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void thirdpartyp2pPreDestruct_DISPATCH(struct ThirdPartyP2P *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__thirdpartyp2pPreDestruct__(pResource);
}

static inline NV_STATUS thirdpartyp2pControlFilter_DISPATCH(struct ThirdPartyP2P *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__thirdpartyp2pControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool thirdpartyp2pIsPartialUnmapSupported_DISPATCH(struct ThirdPartyP2P *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__thirdpartyp2pIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS thirdpartyp2pMapTo_DISPATCH(struct ThirdPartyP2P *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__thirdpartyp2pMapTo__(pResource, pParams);
}

static inline NV_STATUS thirdpartyp2pUnmapFrom_DISPATCH(struct ThirdPartyP2P *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__thirdpartyp2pUnmapFrom__(pResource, pParams);
}

static inline NvU32 thirdpartyp2pGetRefCount_DISPATCH(struct ThirdPartyP2P *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__thirdpartyp2pGetRefCount__(pResource);
}

static inline void thirdpartyp2pAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct ThirdPartyP2P *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__thirdpartyp2pAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS thirdpartyp2pCtrlCmdRegisterVaSpace_IMPL(struct ThirdPartyP2P *pThirdPartyP2P, NV503C_CTRL_REGISTER_VA_SPACE_PARAMS *pRegisterVaSpaceParams);

NV_STATUS thirdpartyp2pCtrlCmdUnregisterVaSpace_IMPL(struct ThirdPartyP2P *pThirdPartyP2P, NV503C_CTRL_UNREGISTER_VA_SPACE_PARAMS *pUnregisterVaSpaceParams);

NV_STATUS thirdpartyp2pCtrlCmdRegisterVidmem_IMPL(struct ThirdPartyP2P *pThirdPartyP2P, NV503C_CTRL_REGISTER_VIDMEM_PARAMS *pRegisterVidmemParams);

NV_STATUS thirdpartyp2pCtrlCmdUnregisterVidmem_IMPL(struct ThirdPartyP2P *pThirdPartyP2P, NV503C_CTRL_UNREGISTER_VIDMEM_PARAMS *pUnregisterVidmemParams);

NV_STATUS thirdpartyp2pCtrlCmdRegisterPid_IMPL(struct ThirdPartyP2P *pThirdPartyP2P, NV503C_CTRL_REGISTER_PID_PARAMS *pParams);

NV_STATUS thirdpartyp2pConstruct_IMPL(struct ThirdPartyP2P *arg_pResource, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_thirdpartyp2pConstruct(arg_pResource, arg_pCallContext, arg_pParams) thirdpartyp2pConstruct_IMPL(arg_pResource, arg_pCallContext, arg_pParams)
void thirdpartyp2pDestruct_IMPL(struct ThirdPartyP2P *pResource);

#define __nvoc_thirdpartyp2pDestruct(pResource) thirdpartyp2pDestruct_IMPL(pResource)
NvBool thirdpartyp2pIsValidClientPid_IMPL(struct ThirdPartyP2P *pThirdPartyP2P, NvU32 pid, NvHandle hClient);

#ifdef __nvoc_third_party_p2p_h_disabled
static inline NvBool thirdpartyp2pIsValidClientPid(struct ThirdPartyP2P *pThirdPartyP2P, NvU32 pid, NvHandle hClient) {
    NV_ASSERT_FAILED_PRECOMP("ThirdPartyP2P was disabled!");
    return NV_FALSE;
}
#else //__nvoc_third_party_p2p_h_disabled
#define thirdpartyp2pIsValidClientPid(pThirdPartyP2P, pid, hClient) thirdpartyp2pIsValidClientPid_IMPL(pThirdPartyP2P, pid, hClient)
#endif //__nvoc_third_party_p2p_h_disabled

NV_STATUS thirdpartyp2pDelMappingInfoByKey_IMPL(struct ThirdPartyP2P *pThirdPartyP2P, void *pKey);

#ifdef __nvoc_third_party_p2p_h_disabled
static inline NV_STATUS thirdpartyp2pDelMappingInfoByKey(struct ThirdPartyP2P *pThirdPartyP2P, void *pKey) {
    NV_ASSERT_FAILED_PRECOMP("ThirdPartyP2P was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_third_party_p2p_h_disabled
#define thirdpartyp2pDelMappingInfoByKey(pThirdPartyP2P, pKey) thirdpartyp2pDelMappingInfoByKey_IMPL(pThirdPartyP2P, pKey)
#endif //__nvoc_third_party_p2p_h_disabled

NV_STATUS thirdpartyp2pDelPersistentMappingInfoByKey_IMPL(struct ThirdPartyP2P *pThirdPartyP2P, void *pKey);

#ifdef __nvoc_third_party_p2p_h_disabled
static inline NV_STATUS thirdpartyp2pDelPersistentMappingInfoByKey(struct ThirdPartyP2P *pThirdPartyP2P, void *pKey) {
    NV_ASSERT_FAILED_PRECOMP("ThirdPartyP2P was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_third_party_p2p_h_disabled
#define thirdpartyp2pDelPersistentMappingInfoByKey(pThirdPartyP2P, pKey) thirdpartyp2pDelPersistentMappingInfoByKey_IMPL(pThirdPartyP2P, pKey)
#endif //__nvoc_third_party_p2p_h_disabled

NV_STATUS thirdpartyp2pGetVASpaceInfoFromToken_IMPL(struct ThirdPartyP2P *pThirdPartyP2P, NvU32 vaSpaceToken, PCLI_THIRD_PARTY_P2P_VASPACE_INFO *ppVASpaceInfo);

#ifdef __nvoc_third_party_p2p_h_disabled
static inline NV_STATUS thirdpartyp2pGetVASpaceInfoFromToken(struct ThirdPartyP2P *pThirdPartyP2P, NvU32 vaSpaceToken, PCLI_THIRD_PARTY_P2P_VASPACE_INFO *ppVASpaceInfo) {
    NV_ASSERT_FAILED_PRECOMP("ThirdPartyP2P was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_third_party_p2p_h_disabled
#define thirdpartyp2pGetVASpaceInfoFromToken(pThirdPartyP2P, vaSpaceToken, ppVASpaceInfo) thirdpartyp2pGetVASpaceInfoFromToken_IMPL(pThirdPartyP2P, vaSpaceToken, ppVASpaceInfo)
#endif //__nvoc_third_party_p2p_h_disabled

NV_STATUS thirdpartyp2pGetNextVASpaceInfo_IMPL(struct ThirdPartyP2P *pThirdPartyP2P, PCLI_THIRD_PARTY_P2P_VASPACE_INFO *arg2);

#ifdef __nvoc_third_party_p2p_h_disabled
static inline NV_STATUS thirdpartyp2pGetNextVASpaceInfo(struct ThirdPartyP2P *pThirdPartyP2P, PCLI_THIRD_PARTY_P2P_VASPACE_INFO *arg2) {
    NV_ASSERT_FAILED_PRECOMP("ThirdPartyP2P was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_third_party_p2p_h_disabled
#define thirdpartyp2pGetNextVASpaceInfo(pThirdPartyP2P, arg2) thirdpartyp2pGetNextVASpaceInfo_IMPL(pThirdPartyP2P, arg2)
#endif //__nvoc_third_party_p2p_h_disabled

#undef PRIVATE_FIELD


typedef struct ThirdPartyP2P *PCLI_THIRD_PARTY_P2P_INFO; // RS-TODO: Remove

// ****************************************************************************
//                          Functions prototypes
// ****************************************************************************

// Get registered third-party P2P info from pid. Also match the provided client handle if provided.
NV_STATUS           CliNextThirdPartyP2PInfoWithPid (struct OBJGPU *, NvU32, NvHandle, struct RmClient**, PCLI_THIRD_PARTY_P2P_INFO *);

// Get third-party P2P info given a P2P token
NV_STATUS           CliGetThirdPartyP2PInfoFromToken (NvU64, PCLI_THIRD_PARTY_P2P_INFO *);

// Register an address space with a third-party P2P object
NV_STATUS           CliAddThirdPartyP2PVASpace   (struct ThirdPartyP2P*, NvHandle, NvU32 *);

// Unregister an address space from a third-party P2P object
NV_STATUS           CliDelThirdPartyP2PVASpace   (struct ThirdPartyP2P*, NvHandle);

// Register video memory with a third-party P2P object
NV_STATUS           CliAddThirdPartyP2PVidmemInfo (struct ThirdPartyP2P*, NvHandle, NvU64, NvU64, NvU64, struct Memory *);

// Unregister video memory from a third-party P2P object
NV_STATUS           CliDelThirdPartyP2PVidmemInfo (struct ThirdPartyP2P*, NvHandle);

// Unregister video memory from a third-party P2P object only if the VidmemInfo is not used
void                CliDelThirdPartyP2PVidmemInfoPersistent (struct ThirdPartyP2P*, CLI_THIRD_PARTY_P2P_VIDMEM_INFO*);

// Find registered video memory given an address
NV_STATUS           CliGetThirdPartyP2PVidmemInfoFromAddress (struct ThirdPartyP2P*, NvU64, NvU64, NvU64 *, PCLI_THIRD_PARTY_P2P_VIDMEM_INFO *);

// Find registered video memory given a VidmemInfo ID
NV_STATUS           CliGetThirdPartyP2PVidmemInfoFromId(struct ThirdPartyP2P*, NvU64, CLI_THIRD_PARTY_P2P_VIDMEM_INFO **);

// Find platformData given a P2PInfo object
NV_STATUS           CliGetThirdPartyP2PPlatformData (PCLI_THIRD_PARTY_P2P_INFO, void *);

// Associate a P2P mapping with registered video memory
NV_STATUS           CliAddThirdPartyP2PMappingInfo (struct ThirdPartyP2P*, NvHandle, void *, THIRD_PARTY_P2P_VIDMEM_FREE_CALLBACK *, void *, PCLI_THIRD_PARTY_P2P_MAPPING_INFO *);

// Find a P2P mapping given its platform specific data
NV_STATUS           CliGetThirdPartyP2PMappingInfoFromKey (struct ThirdPartyP2P*, NvHandle, void *, PCLI_THIRD_PARTY_P2P_MAPPING_INFO *);

// Register pid & client with a third-party P2P Info object
NV_STATUS           CliAddThirdPartyP2PClientPid (struct ThirdPartyP2P*, NvU32, NvHandle);

// Unregister pid & client from a third-party P2P Info object
NV_STATUS           CliDelThirdPartyP2PClientPid (struct RmClient*, NvHandle, NvU32, NvU32);

// Remove association of a client from any existing third-Party P2P Info object
NV_STATUS           CliUnregisterFromThirdPartyP2P (struct RmClient*);

// Register a free callback
NV_STATUS           CliRegisterThirdPartyP2PMappingCallback (struct ThirdPartyP2P*, NvHandle, void *, THIRD_PARTY_P2P_VIDMEM_FREE_CALLBACK *, void *);

// Unregister memory from a third-party P2P Info object
void                CliUnregisterMemoryFromThirdPartyP2P(struct Memory *pMemory);

#endif // _THIRD_PARTY_P2P_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_THIRD_PARTY_P2P_NVOC_H_
