#ifndef _G_THIRD_PARTY_P2P_NVOC_H_
#define _G_THIRD_PARTY_P2P_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2009-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_third_party_p2p_nvoc.h"

#ifndef _THIRD_PARTY_P2P_H_
#define _THIRD_PARTY_P2P_H_

#include "rmapi/client.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/gpu_resource.h"

#include <ctrl/ctrl503c.h>

//
// The third-party P2P token is a 64bit integer.
//
// The value may be passed by the RM client enabling the third-party P2P access.
// Otherwise the P2P token format is:
//
//   fn  [  *GPU ID*  ]  [ *UNUSED*  ]  [  *PEER INDEX*  ]
//   bit 63          32  31          8  7                0
//
// The third-party P2P token is a globally unique identifier for one
// of an attached GPU's several P2P slots.  It is passed, as an
// opaque handle, to third-party driver stacks to allow the setup
// of a P2P mapping between a given GPU and a third-party device with
// NVIDIA P2P capabilities.
//

#define CLI_ENCODEP2PTOKEN(gpuId, peerIndex) (((NvU64)gpuId << 32) | peerIndex)

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

    NvU64                    fbApertureOffset;
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


#ifdef NVOC_THIRD_PARTY_P2P_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct ThirdPartyP2P {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct ThirdPartyP2P *__nvoc_pbase_ThirdPartyP2P;
    NV_STATUS (*__thirdpartyp2pCtrlCmdRegisterVaSpace__)(struct ThirdPartyP2P *, NV503C_CTRL_REGISTER_VA_SPACE_PARAMS *);
    NV_STATUS (*__thirdpartyp2pCtrlCmdUnregisterVaSpace__)(struct ThirdPartyP2P *, NV503C_CTRL_UNREGISTER_VA_SPACE_PARAMS *);
    NV_STATUS (*__thirdpartyp2pCtrlCmdRegisterVidmem__)(struct ThirdPartyP2P *, NV503C_CTRL_REGISTER_VIDMEM_PARAMS *);
    NV_STATUS (*__thirdpartyp2pCtrlCmdUnregisterVidmem__)(struct ThirdPartyP2P *, NV503C_CTRL_UNREGISTER_VIDMEM_PARAMS *);
    NV_STATUS (*__thirdpartyp2pCtrlCmdRegisterPid__)(struct ThirdPartyP2P *, NV503C_CTRL_REGISTER_PID_PARAMS *);
    NvBool (*__thirdpartyp2pShareCallback__)(struct ThirdPartyP2P *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__thirdpartyp2pControl__)(struct ThirdPartyP2P *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__thirdpartyp2pUnmap__)(struct ThirdPartyP2P *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__thirdpartyp2pGetMemInterMapParams__)(struct ThirdPartyP2P *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__thirdpartyp2pGetMemoryMappingDescriptor__)(struct ThirdPartyP2P *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__thirdpartyp2pGetMapAddrSpace__)(struct ThirdPartyP2P *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvHandle (*__thirdpartyp2pGetInternalObjectHandle__)(struct ThirdPartyP2P *);
    NV_STATUS (*__thirdpartyp2pControlFilter__)(struct ThirdPartyP2P *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__thirdpartyp2pAddAdditionalDependants__)(struct RsClient *, struct ThirdPartyP2P *, RsResourceRef *);
    NvU32 (*__thirdpartyp2pGetRefCount__)(struct ThirdPartyP2P *);
    NV_STATUS (*__thirdpartyp2pCheckMemInterUnmap__)(struct ThirdPartyP2P *, NvBool);
    NV_STATUS (*__thirdpartyp2pMapTo__)(struct ThirdPartyP2P *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__thirdpartyp2pControl_Prologue__)(struct ThirdPartyP2P *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__thirdpartyp2pGetRegBaseOffsetAndSize__)(struct ThirdPartyP2P *, struct OBJGPU *, NvU32 *, NvU32 *);
    NvBool (*__thirdpartyp2pCanCopy__)(struct ThirdPartyP2P *);
    NV_STATUS (*__thirdpartyp2pInternalControlForward__)(struct ThirdPartyP2P *, NvU32, void *, NvU32);
    void (*__thirdpartyp2pPreDestruct__)(struct ThirdPartyP2P *);
    NV_STATUS (*__thirdpartyp2pUnmapFrom__)(struct ThirdPartyP2P *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__thirdpartyp2pControl_Epilogue__)(struct ThirdPartyP2P *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__thirdpartyp2pControlLookup__)(struct ThirdPartyP2P *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__thirdpartyp2pMap__)(struct ThirdPartyP2P *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__thirdpartyp2pAccessCallback__)(struct ThirdPartyP2P *, struct RsClient *, void *, RsAccessRight);
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
    PNODE pAddressRangeTree;
};

#ifndef __NVOC_CLASS_ThirdPartyP2P_TYPEDEF__
#define __NVOC_CLASS_ThirdPartyP2P_TYPEDEF__
typedef struct ThirdPartyP2P ThirdPartyP2P;
#endif /* __NVOC_CLASS_ThirdPartyP2P_TYPEDEF__ */

#ifndef __nvoc_class_id_ThirdPartyP2P
#define __nvoc_class_id_ThirdPartyP2P 0x34d08b
#endif /* __nvoc_class_id_ThirdPartyP2P */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ThirdPartyP2P;

#define __staticCast_ThirdPartyP2P(pThis) \
    ((pThis)->__nvoc_pbase_ThirdPartyP2P)

#ifdef __nvoc_third_party_p2p_h_disabled
#define __dynamicCast_ThirdPartyP2P(pThis) ((ThirdPartyP2P*)NULL)
#else //__nvoc_third_party_p2p_h_disabled
#define __dynamicCast_ThirdPartyP2P(pThis) \
    ((ThirdPartyP2P*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(ThirdPartyP2P)))
#endif //__nvoc_third_party_p2p_h_disabled


NV_STATUS __nvoc_objCreateDynamic_ThirdPartyP2P(ThirdPartyP2P**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_ThirdPartyP2P(ThirdPartyP2P**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_ThirdPartyP2P(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_ThirdPartyP2P((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define thirdpartyp2pCtrlCmdRegisterVaSpace(pThirdPartyP2P, pRegisterVaSpaceParams) thirdpartyp2pCtrlCmdRegisterVaSpace_DISPATCH(pThirdPartyP2P, pRegisterVaSpaceParams)
#define thirdpartyp2pCtrlCmdUnregisterVaSpace(pThirdPartyP2P, pUnregisterVaSpaceParams) thirdpartyp2pCtrlCmdUnregisterVaSpace_DISPATCH(pThirdPartyP2P, pUnregisterVaSpaceParams)
#define thirdpartyp2pCtrlCmdRegisterVidmem(pThirdPartyP2P, pRegisterVidmemParams) thirdpartyp2pCtrlCmdRegisterVidmem_DISPATCH(pThirdPartyP2P, pRegisterVidmemParams)
#define thirdpartyp2pCtrlCmdUnregisterVidmem(pThirdPartyP2P, pUnregisterVidmemParams) thirdpartyp2pCtrlCmdUnregisterVidmem_DISPATCH(pThirdPartyP2P, pUnregisterVidmemParams)
#define thirdpartyp2pCtrlCmdRegisterPid(pThirdPartyP2P, pParams) thirdpartyp2pCtrlCmdRegisterPid_DISPATCH(pThirdPartyP2P, pParams)
#define thirdpartyp2pShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) thirdpartyp2pShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define thirdpartyp2pControl(pGpuResource, pCallContext, pParams) thirdpartyp2pControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define thirdpartyp2pUnmap(pGpuResource, pCallContext, pCpuMapping) thirdpartyp2pUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define thirdpartyp2pGetMemInterMapParams(pRmResource, pParams) thirdpartyp2pGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define thirdpartyp2pGetMemoryMappingDescriptor(pRmResource, ppMemDesc) thirdpartyp2pGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define thirdpartyp2pGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) thirdpartyp2pGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define thirdpartyp2pGetInternalObjectHandle(pGpuResource) thirdpartyp2pGetInternalObjectHandle_DISPATCH(pGpuResource)
#define thirdpartyp2pControlFilter(pResource, pCallContext, pParams) thirdpartyp2pControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define thirdpartyp2pAddAdditionalDependants(pClient, pResource, pReference) thirdpartyp2pAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define thirdpartyp2pGetRefCount(pResource) thirdpartyp2pGetRefCount_DISPATCH(pResource)
#define thirdpartyp2pCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) thirdpartyp2pCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define thirdpartyp2pMapTo(pResource, pParams) thirdpartyp2pMapTo_DISPATCH(pResource, pParams)
#define thirdpartyp2pControl_Prologue(pResource, pCallContext, pParams) thirdpartyp2pControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define thirdpartyp2pGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) thirdpartyp2pGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define thirdpartyp2pCanCopy(pResource) thirdpartyp2pCanCopy_DISPATCH(pResource)
#define thirdpartyp2pInternalControlForward(pGpuResource, command, pParams, size) thirdpartyp2pInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define thirdpartyp2pPreDestruct(pResource) thirdpartyp2pPreDestruct_DISPATCH(pResource)
#define thirdpartyp2pUnmapFrom(pResource, pParams) thirdpartyp2pUnmapFrom_DISPATCH(pResource, pParams)
#define thirdpartyp2pControl_Epilogue(pResource, pCallContext, pParams) thirdpartyp2pControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define thirdpartyp2pControlLookup(pResource, pParams, ppEntry) thirdpartyp2pControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define thirdpartyp2pMap(pGpuResource, pCallContext, pParams, pCpuMapping) thirdpartyp2pMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define thirdpartyp2pAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) thirdpartyp2pAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS thirdpartyp2pCtrlCmdRegisterVaSpace_IMPL(struct ThirdPartyP2P *pThirdPartyP2P, NV503C_CTRL_REGISTER_VA_SPACE_PARAMS *pRegisterVaSpaceParams);

static inline NV_STATUS thirdpartyp2pCtrlCmdRegisterVaSpace_DISPATCH(struct ThirdPartyP2P *pThirdPartyP2P, NV503C_CTRL_REGISTER_VA_SPACE_PARAMS *pRegisterVaSpaceParams) {
    return pThirdPartyP2P->__thirdpartyp2pCtrlCmdRegisterVaSpace__(pThirdPartyP2P, pRegisterVaSpaceParams);
}

NV_STATUS thirdpartyp2pCtrlCmdUnregisterVaSpace_IMPL(struct ThirdPartyP2P *pThirdPartyP2P, NV503C_CTRL_UNREGISTER_VA_SPACE_PARAMS *pUnregisterVaSpaceParams);

static inline NV_STATUS thirdpartyp2pCtrlCmdUnregisterVaSpace_DISPATCH(struct ThirdPartyP2P *pThirdPartyP2P, NV503C_CTRL_UNREGISTER_VA_SPACE_PARAMS *pUnregisterVaSpaceParams) {
    return pThirdPartyP2P->__thirdpartyp2pCtrlCmdUnregisterVaSpace__(pThirdPartyP2P, pUnregisterVaSpaceParams);
}

NV_STATUS thirdpartyp2pCtrlCmdRegisterVidmem_IMPL(struct ThirdPartyP2P *pThirdPartyP2P, NV503C_CTRL_REGISTER_VIDMEM_PARAMS *pRegisterVidmemParams);

static inline NV_STATUS thirdpartyp2pCtrlCmdRegisterVidmem_DISPATCH(struct ThirdPartyP2P *pThirdPartyP2P, NV503C_CTRL_REGISTER_VIDMEM_PARAMS *pRegisterVidmemParams) {
    return pThirdPartyP2P->__thirdpartyp2pCtrlCmdRegisterVidmem__(pThirdPartyP2P, pRegisterVidmemParams);
}

NV_STATUS thirdpartyp2pCtrlCmdUnregisterVidmem_IMPL(struct ThirdPartyP2P *pThirdPartyP2P, NV503C_CTRL_UNREGISTER_VIDMEM_PARAMS *pUnregisterVidmemParams);

static inline NV_STATUS thirdpartyp2pCtrlCmdUnregisterVidmem_DISPATCH(struct ThirdPartyP2P *pThirdPartyP2P, NV503C_CTRL_UNREGISTER_VIDMEM_PARAMS *pUnregisterVidmemParams) {
    return pThirdPartyP2P->__thirdpartyp2pCtrlCmdUnregisterVidmem__(pThirdPartyP2P, pUnregisterVidmemParams);
}

NV_STATUS thirdpartyp2pCtrlCmdRegisterPid_IMPL(struct ThirdPartyP2P *pThirdPartyP2P, NV503C_CTRL_REGISTER_PID_PARAMS *pParams);

static inline NV_STATUS thirdpartyp2pCtrlCmdRegisterPid_DISPATCH(struct ThirdPartyP2P *pThirdPartyP2P, NV503C_CTRL_REGISTER_PID_PARAMS *pParams) {
    return pThirdPartyP2P->__thirdpartyp2pCtrlCmdRegisterPid__(pThirdPartyP2P, pParams);
}

static inline NvBool thirdpartyp2pShareCallback_DISPATCH(struct ThirdPartyP2P *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__thirdpartyp2pShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS thirdpartyp2pControl_DISPATCH(struct ThirdPartyP2P *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__thirdpartyp2pControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS thirdpartyp2pUnmap_DISPATCH(struct ThirdPartyP2P *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__thirdpartyp2pUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS thirdpartyp2pGetMemInterMapParams_DISPATCH(struct ThirdPartyP2P *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__thirdpartyp2pGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS thirdpartyp2pGetMemoryMappingDescriptor_DISPATCH(struct ThirdPartyP2P *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__thirdpartyp2pGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS thirdpartyp2pGetMapAddrSpace_DISPATCH(struct ThirdPartyP2P *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__thirdpartyp2pGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NvHandle thirdpartyp2pGetInternalObjectHandle_DISPATCH(struct ThirdPartyP2P *pGpuResource) {
    return pGpuResource->__thirdpartyp2pGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS thirdpartyp2pControlFilter_DISPATCH(struct ThirdPartyP2P *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__thirdpartyp2pControlFilter__(pResource, pCallContext, pParams);
}

static inline void thirdpartyp2pAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct ThirdPartyP2P *pResource, RsResourceRef *pReference) {
    pResource->__thirdpartyp2pAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NvU32 thirdpartyp2pGetRefCount_DISPATCH(struct ThirdPartyP2P *pResource) {
    return pResource->__thirdpartyp2pGetRefCount__(pResource);
}

static inline NV_STATUS thirdpartyp2pCheckMemInterUnmap_DISPATCH(struct ThirdPartyP2P *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__thirdpartyp2pCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS thirdpartyp2pMapTo_DISPATCH(struct ThirdPartyP2P *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__thirdpartyp2pMapTo__(pResource, pParams);
}

static inline NV_STATUS thirdpartyp2pControl_Prologue_DISPATCH(struct ThirdPartyP2P *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__thirdpartyp2pControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS thirdpartyp2pGetRegBaseOffsetAndSize_DISPATCH(struct ThirdPartyP2P *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__thirdpartyp2pGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NvBool thirdpartyp2pCanCopy_DISPATCH(struct ThirdPartyP2P *pResource) {
    return pResource->__thirdpartyp2pCanCopy__(pResource);
}

static inline NV_STATUS thirdpartyp2pInternalControlForward_DISPATCH(struct ThirdPartyP2P *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__thirdpartyp2pInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline void thirdpartyp2pPreDestruct_DISPATCH(struct ThirdPartyP2P *pResource) {
    pResource->__thirdpartyp2pPreDestruct__(pResource);
}

static inline NV_STATUS thirdpartyp2pUnmapFrom_DISPATCH(struct ThirdPartyP2P *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__thirdpartyp2pUnmapFrom__(pResource, pParams);
}

static inline void thirdpartyp2pControl_Epilogue_DISPATCH(struct ThirdPartyP2P *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__thirdpartyp2pControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS thirdpartyp2pControlLookup_DISPATCH(struct ThirdPartyP2P *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__thirdpartyp2pControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS thirdpartyp2pMap_DISPATCH(struct ThirdPartyP2P *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__thirdpartyp2pMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool thirdpartyp2pAccessCallback_DISPATCH(struct ThirdPartyP2P *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__thirdpartyp2pAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

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

NV_STATUS thirdpartyp2pDelMappingInfoByKey_IMPL(struct ThirdPartyP2P *pThirdPartyP2P, void *pKey, NvBool bIsRsyncNeeded);
#ifdef __nvoc_third_party_p2p_h_disabled
static inline NV_STATUS thirdpartyp2pDelMappingInfoByKey(struct ThirdPartyP2P *pThirdPartyP2P, void *pKey, NvBool bIsRsyncNeeded) {
    NV_ASSERT_FAILED_PRECOMP("ThirdPartyP2P was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_third_party_p2p_h_disabled
#define thirdpartyp2pDelMappingInfoByKey(pThirdPartyP2P, pKey, bIsRsyncNeeded) thirdpartyp2pDelMappingInfoByKey_IMPL(pThirdPartyP2P, pKey, bIsRsyncNeeded)
#endif //__nvoc_third_party_p2p_h_disabled

NV_STATUS thirdpartyp2pDelPersistentMappingInfoByKey_IMPL(struct ThirdPartyP2P *pThirdPartyP2P, void *pKey, NvBool bIsRsyncNeeded);
#ifdef __nvoc_third_party_p2p_h_disabled
static inline NV_STATUS thirdpartyp2pDelPersistentMappingInfoByKey(struct ThirdPartyP2P *pThirdPartyP2P, void *pKey, NvBool bIsRsyncNeeded) {
    NV_ASSERT_FAILED_PRECOMP("ThirdPartyP2P was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_third_party_p2p_h_disabled
#define thirdpartyp2pDelPersistentMappingInfoByKey(pThirdPartyP2P, pKey, bIsRsyncNeeded) thirdpartyp2pDelPersistentMappingInfoByKey_IMPL(pThirdPartyP2P, pKey, bIsRsyncNeeded)
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

NV_STATUS thirdpartyp2pGetNextVASpaceInfo_IMPL(struct ThirdPartyP2P *pThirdPartyP2P, PCLI_THIRD_PARTY_P2P_VASPACE_INFO *arg0);
#ifdef __nvoc_third_party_p2p_h_disabled
static inline NV_STATUS thirdpartyp2pGetNextVASpaceInfo(struct ThirdPartyP2P *pThirdPartyP2P, PCLI_THIRD_PARTY_P2P_VASPACE_INFO *arg0) {
    NV_ASSERT_FAILED_PRECOMP("ThirdPartyP2P was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_third_party_p2p_h_disabled
#define thirdpartyp2pGetNextVASpaceInfo(pThirdPartyP2P, arg0) thirdpartyp2pGetNextVASpaceInfo_IMPL(pThirdPartyP2P, arg0)
#endif //__nvoc_third_party_p2p_h_disabled

#undef PRIVATE_FIELD


typedef struct ThirdPartyP2P *PCLI_THIRD_PARTY_P2P_INFO; // RS-TODO: Remove

// ****************************************************************************
//                          Functions prototypes
// ****************************************************************************

// Create and add/register a third-party P2P object
NV_STATUS           CliAddThirdPartyP2P          (NvHandle, NvHandle, struct Subdevice *, NvU32, NvU64);

// Delete the specified third-party P2P object
NV_STATUS           CliDelThirdPartyP2P          (NvHandle, NvHandle);

// Get third-party P2P info given client/object handles
NV_STATUS           CliGetThirdPartyP2PInfo      (NvHandle, NvHandle, PCLI_THIRD_PARTY_P2P_INFO *);

// Get registered third-party P2P info from pid. Also match the provided client handle if provided.
NV_STATUS           CliNextThirdPartyP2PInfoWithPid (struct OBJGPU *, NvU32, NvHandle, struct RmClient**, PCLI_THIRD_PARTY_P2P_INFO *);

// Get third-party P2P info given a P2P token
NV_STATUS           CliGetThirdPartyP2PInfoFromToken (NvU64, PCLI_THIRD_PARTY_P2P_INFO *);

// Register an address space with a third-party P2P object
NV_STATUS           CliAddThirdPartyP2PVASpace   (NvHandle, NvHandle, NvHandle, NvU32 *);

// Unregister an address space from a third-party P2P object
NV_STATUS           CliDelThirdPartyP2PVASpace   (struct ThirdPartyP2P*, NvHandle);

// Register video memory with a third-party P2P object
NV_STATUS           CliAddThirdPartyP2PVidmemInfo (NvHandle, NvHandle, NvHandle, NvU64, NvU64, NvU64, struct Memory *);

// Unregister video memory from a third-party P2P object
NV_STATUS           CliDelThirdPartyP2PVidmemInfo (struct ThirdPartyP2P*, NvHandle);

// Unregister video memory from a third-party P2P object only if the VidmemInfo is not used
void                CliDelThirdPartyP2PVidmemInfoPersistent (struct ThirdPartyP2P*, CLI_THIRD_PARTY_P2P_VIDMEM_INFO*);

// Find registered video memory given an address
NV_STATUS           CliGetThirdPartyP2PVidmemInfoFromAddress (NvHandle, NvHandle, NvU64, NvU64, NvU64 *, PCLI_THIRD_PARTY_P2P_VIDMEM_INFO *);

// Find registered video memory given a VidmemInfo ID
NV_STATUS           CliGetThirdPartyP2PVidmemInfoFromId(NvHandle, NvHandle, NvU64, CLI_THIRD_PARTY_P2P_VIDMEM_INFO **);

// Find platformData given a P2PInfo object
NV_STATUS           CliGetThirdPartyP2PPlatformData (PCLI_THIRD_PARTY_P2P_INFO, void *);

// Associate a P2P mapping with registered video memory
NV_STATUS           CliAddThirdPartyP2PMappingInfo (NvHandle, NvHandle, NvHandle, void *, THIRD_PARTY_P2P_VIDMEM_FREE_CALLBACK *, void *, PCLI_THIRD_PARTY_P2P_MAPPING_INFO *);

// Find a P2P mapping given its platform specific data
NV_STATUS           CliGetThirdPartyP2PMappingInfoFromKey (NvHandle, NvHandle, NvHandle, void *, PCLI_THIRD_PARTY_P2P_MAPPING_INFO *);

// Register pid & client with a third-party P2P Info object
NV_STATUS           CliAddThirdPartyP2PClientPid (NvHandle, NvHandle, NvU32, NvU32);

// Unregister pid & client from a third-party P2P Info object
NV_STATUS           CliDelThirdPartyP2PClientPid (struct RmClient*, NvHandle, NvU32, NvU32);

// Remove association of a client from any existing third-Party P2P Info object
NV_STATUS           CliUnregisterFromThirdPartyP2P (struct RmClient*);

// Register a free callback
NV_STATUS           CliRegisterThirdPartyP2PMappingCallback (NvHandle, NvHandle, NvHandle, void *, THIRD_PARTY_P2P_VIDMEM_FREE_CALLBACK *, void *);

// Unregister memory from a third-party P2P Info object
void                CliUnregisterMemoryFromThirdPartyP2P(struct Memory *pMemory);

#endif // _THIRD_PARTY_P2P_H_

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_THIRD_PARTY_P2P_NVOC_H_
