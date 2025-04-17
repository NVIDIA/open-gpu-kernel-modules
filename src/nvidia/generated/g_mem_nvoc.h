
#ifndef _G_MEM_NVOC_H_
#define _G_MEM_NVOC_H_

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

#pragma once
#include "g_mem_nvoc.h"

#ifndef _MEMORY_API_H_
#define _MEMORY_API_H_

#include "core/core.h"
#include "resserv/rs_resource.h"
#include "rmapi/rmapi.h"
#include "rmapi/resource.h"

#include "containers/btree.h"

#include "ctrl/ctrl0041.h"


struct Device;

#ifndef __NVOC_CLASS_Device_TYPEDEF__
#define __NVOC_CLASS_Device_TYPEDEF__
typedef struct Device Device;
#endif /* __NVOC_CLASS_Device_TYPEDEF__ */

#ifndef __nvoc_class_id_Device
#define __nvoc_class_id_Device 0xe0ac20
#endif /* __nvoc_class_id_Device */



struct Subdevice;

#ifndef __NVOC_CLASS_Subdevice_TYPEDEF__
#define __NVOC_CLASS_Subdevice_TYPEDEF__
typedef struct Subdevice Subdevice;
#endif /* __NVOC_CLASS_Subdevice_TYPEDEF__ */

#ifndef __nvoc_class_id_Subdevice
#define __nvoc_class_id_Subdevice 0x4b01b3
#endif /* __nvoc_class_id_Subdevice */



struct RsClient;

#ifndef __NVOC_CLASS_RsClient_TYPEDEF__
#define __NVOC_CLASS_RsClient_TYPEDEF__
typedef struct RsClient RsClient;
#endif /* __NVOC_CLASS_RsClient_TYPEDEF__ */

#ifndef __nvoc_class_id_RsClient
#define __nvoc_class_id_RsClient 0x8f87e5
#endif /* __nvoc_class_id_RsClient */



struct Heap;

#ifndef __NVOC_CLASS_Heap_TYPEDEF__
#define __NVOC_CLASS_Heap_TYPEDEF__
typedef struct Heap Heap;
#endif /* __NVOC_CLASS_Heap_TYPEDEF__ */

#ifndef __nvoc_class_id_Heap
#define __nvoc_class_id_Heap 0x556e9a
#endif /* __nvoc_class_id_Heap */



struct OBJGPU;

#ifndef __NVOC_CLASS_OBJGPU_TYPEDEF__
#define __NVOC_CLASS_OBJGPU_TYPEDEF__
typedef struct OBJGPU OBJGPU;
#endif /* __NVOC_CLASS_OBJGPU_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPU
#define __nvoc_class_id_OBJGPU 0x7ef3cb
#endif /* __nvoc_class_id_OBJGPU */


typedef struct MEMORY_DESCRIPTOR MEMORY_DESCRIPTOR;
typedef struct PmuMapping PmuMapping;
typedef struct HWRESOURCE_INFO HWRESOURCE_INFO;

//
// vGPU non-stall interrupt info
//
typedef struct _def_client_vgpu_ns_intr
{
    NvU32                   nsSemValue;  // Non stall interrupt semaphore value
    NvU32                   nsSemOffset; // Non stall interrupt semaphore offset. Currently it is always 0.
    NvBool                  isSemaMemValidationEnabled; // Enable change in Non stall interrupt sema value check
                                                        // while generating event
    NvU64                   guestDomainId; // guest ID that we need to use to inject interrupt
    NvU64                   guestMSIAddr; // MSI address allocated by guest OS
    NvU32                   guestMSIData; // MSI data value set by guest OS
    void                    *pEventDpc; // DPC event to pass the interrupt
} VGPU_NS_INTR;

typedef struct
{
  struct Memory *pNext;
  struct Memory *pPrev;
} memCircularListItem;

/*!
 * RM internal class representing NV01_MEMORY_XXX
 *
 * @note Memory cannot be a GpuResource because NoDeviceMemory
 *       subclass is not allocated under a device.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_MEM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__Memory;
struct NVOC_METADATA__RmResource;
struct NVOC_VTABLE__Memory;


struct Memory {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__Memory *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RmResource __nvoc_base_RmResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct RsResource *__nvoc_pbase_RsResource;    // res super^2
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^2
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super
    struct Memory *__nvoc_pbase_Memory;    // mem

    // Vtable with 7 per-object function pointers
    NV_STATUS (*__memCtrlCmdGetSurfaceCompressionCoverageLvm__)(struct Memory * /*this*/, NV0041_CTRL_GET_SURFACE_COMPRESSION_COVERAGE_PARAMS *);  // exported (id=0x410112)
    NV_STATUS (*__memCtrlCmdSurfaceFlushGpuCache__)(struct Memory * /*this*/, NV0041_CTRL_SURFACE_FLUSH_GPU_CACHE_PARAMS *);  // exported (id=0x410116)
    NV_STATUS (*__memCtrlCmdGetMemPageSize__)(struct Memory * /*this*/, NV0041_CTRL_GET_MEM_PAGE_SIZE_PARAMS *);  // exported (id=0x410118)
    NV_STATUS (*__memCtrlCmdSetTag__)(struct Memory * /*this*/, NV0041_CTRL_CMD_SET_TAG_PARAMS *);  // exported (id=0x410120)
    NV_STATUS (*__memCtrlCmdGetTag__)(struct Memory * /*this*/, NV0041_CTRL_CMD_GET_TAG_PARAMS *);  // exported (id=0x410121)
    NV_STATUS (*__memCtrlCmdGetSurfacePhysAttrLvm__)(struct Memory * /*this*/, NV0041_CTRL_GET_SURFACE_PHYS_ATTR_PARAMS *);  // exported (id=0x410103)
    NV_STATUS (*__memCtrlCmdGetSurfaceInfoLvm__)(struct Memory * /*this*/, NV0041_CTRL_GET_SURFACE_INFO_PARAMS *);  // exported (id=0x410110)

    // Data members
    NvBool bConstructed;
    struct Device *pDevice;
    struct Subdevice *pSubDevice;
    struct OBJGPU *pGpu;
    NvBool bBcResource;
    NvU32 categoryClassId;
    NvU64 Length;
    NvU32 HeapOwner;
    NvU32 RefCount;
    struct Heap *pHeap;
    MEMORY_DESCRIPTOR *pMemDesc;
    NvBool isMemDescOwner;
    memCircularListItem dupListItem;
    NvP64 KernelVAddr;
    NvP64 KernelMapPriv;
    PmuMapping *pPmuMappingList;
    NODE Node;
    NvU32 Attr;
    NvU32 Attr2;
    NvU32 Pitch;
    NvU32 Type;
    NvU32 Flags;
    NvU32 tag;
    NvU64 osDeviceHandle;
    HWRESOURCE_INFO *pHwResource;
    NvBool bRpcAlloc;
    NvBool bRegisteredWithGsp;
    VGPU_NS_INTR vgpuNsIntr;
};


// Vtable with 26 per-class function pointers
struct NVOC_VTABLE__Memory {
    NV_STATUS (*__memIsDuplicate__)(struct Memory * /*this*/, NvHandle, NvBool *);  // virtual override (res) base (rmres)
    NV_STATUS (*__memGetMapAddrSpace__)(struct Memory * /*this*/, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual
    NV_STATUS (*__memControl__)(struct Memory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual override (res) base (rmres)
    NV_STATUS (*__memMap__)(struct Memory * /*this*/, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual override (res) base (rmres)
    NV_STATUS (*__memUnmap__)(struct Memory * /*this*/, CALL_CONTEXT *, RsCpuMapping *);  // virtual override (res) base (rmres)
    NV_STATUS (*__memGetMemInterMapParams__)(struct Memory * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual override (rmres) base (rmres)
    NV_STATUS (*__memCheckMemInterUnmap__)(struct Memory * /*this*/, NvBool);  // inline virtual override (rmres) base (rmres) body
    NV_STATUS (*__memGetMemoryMappingDescriptor__)(struct Memory * /*this*/, MEMORY_DESCRIPTOR **);  // virtual override (rmres) base (rmres)
    NV_STATUS (*__memCheckCopyPermissions__)(struct Memory * /*this*/, struct OBJGPU *, struct Device *);  // inline virtual body
    NV_STATUS (*__memIsReady__)(struct Memory * /*this*/, NvBool);  // virtual
    NvBool (*__memIsGpuMapAllowed__)(struct Memory * /*this*/, struct OBJGPU *);  // inline virtual body
    NvBool (*__memIsExportAllowed__)(struct Memory * /*this*/);  // inline virtual body
    NvBool (*__memAccessCallback__)(struct Memory * /*this*/, RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (rmres)
    NvBool (*__memShareCallback__)(struct Memory * /*this*/, RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__memControlSerialization_Prologue__)(struct Memory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__memControlSerialization_Epilogue__)(struct Memory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__memControl_Prologue__)(struct Memory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__memControl_Epilogue__)(struct Memory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NvBool (*__memCanCopy__)(struct Memory * /*this*/);  // virtual inherited (res) base (rmres)
    void (*__memPreDestruct__)(struct Memory * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__memControlFilter__)(struct Memory * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (rmres)
    NvBool (*__memIsPartialUnmapSupported__)(struct Memory * /*this*/);  // inline virtual inherited (res) base (rmres) body
    NV_STATUS (*__memMapTo__)(struct Memory * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__memUnmapFrom__)(struct Memory * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (rmres)
    NvU32 (*__memGetRefCount__)(struct Memory * /*this*/);  // virtual inherited (res) base (rmres)
    void (*__memAddAdditionalDependants__)(struct RsClient *, struct Memory * /*this*/, RsResourceRef *);  // virtual inherited (res) base (rmres)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__Memory {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RmResource metadata__RmResource;
    const struct NVOC_VTABLE__Memory vtable;
};

#ifndef __NVOC_CLASS_Memory_TYPEDEF__
#define __NVOC_CLASS_Memory_TYPEDEF__
typedef struct Memory Memory;
#endif /* __NVOC_CLASS_Memory_TYPEDEF__ */

#ifndef __nvoc_class_id_Memory
#define __nvoc_class_id_Memory 0x4789f2
#endif /* __nvoc_class_id_Memory */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

#define __staticCast_Memory(pThis) \
    ((pThis)->__nvoc_pbase_Memory)

#ifdef __nvoc_mem_h_disabled
#define __dynamicCast_Memory(pThis) ((Memory*) NULL)
#else //__nvoc_mem_h_disabled
#define __dynamicCast_Memory(pThis) \
    ((Memory*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Memory)))
#endif //__nvoc_mem_h_disabled

NV_STATUS __nvoc_objCreateDynamic_Memory(Memory**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Memory(Memory**, Dynamic*, NvU32, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_Memory(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_Memory((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define memIsDuplicate_FNPTR(pMemory) pMemory->__nvoc_metadata_ptr->vtable.__memIsDuplicate__
#define memIsDuplicate(pMemory, hMemory, pDuplicate) memIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define memGetMapAddrSpace_FNPTR(pMemory) pMemory->__nvoc_metadata_ptr->vtable.__memGetMapAddrSpace__
#define memGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) memGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define memControl_FNPTR(pMemory) pMemory->__nvoc_metadata_ptr->vtable.__memControl__
#define memControl(pMemory, pCallContext, pParams) memControl_DISPATCH(pMemory, pCallContext, pParams)
#define memMap_FNPTR(pMemory) pMemory->__nvoc_metadata_ptr->vtable.__memMap__
#define memMap(pMemory, pCallContext, pParams, pCpuMapping) memMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define memUnmap_FNPTR(pMemory) pMemory->__nvoc_metadata_ptr->vtable.__memUnmap__
#define memUnmap(pMemory, pCallContext, pCpuMapping) memUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define memGetMemInterMapParams_FNPTR(pMemory) pMemory->__nvoc_metadata_ptr->vtable.__memGetMemInterMapParams__
#define memGetMemInterMapParams(pMemory, pParams) memGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define memCheckMemInterUnmap_FNPTR(pMemory) pMemory->__nvoc_metadata_ptr->vtable.__memCheckMemInterUnmap__
#define memCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) memCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define memGetMemoryMappingDescriptor_FNPTR(pMemory) pMemory->__nvoc_metadata_ptr->vtable.__memGetMemoryMappingDescriptor__
#define memGetMemoryMappingDescriptor(pMemory, ppMemDesc) memGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define memCheckCopyPermissions_FNPTR(pMemory) pMemory->__nvoc_metadata_ptr->vtable.__memCheckCopyPermissions__
#define memCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) memCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define memIsReady_FNPTR(pMemory) pMemory->__nvoc_metadata_ptr->vtable.__memIsReady__
#define memIsReady(pMemory, bCopyConstructorContext) memIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define memIsGpuMapAllowed_FNPTR(pMemory) pMemory->__nvoc_metadata_ptr->vtable.__memIsGpuMapAllowed__
#define memIsGpuMapAllowed(pMemory, pGpu) memIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define memIsExportAllowed_FNPTR(pMemory) pMemory->__nvoc_metadata_ptr->vtable.__memIsExportAllowed__
#define memIsExportAllowed(pMemory) memIsExportAllowed_DISPATCH(pMemory)
#define memCtrlCmdGetSurfaceCompressionCoverageLvm_FNPTR(pMemory) pMemory->__memCtrlCmdGetSurfaceCompressionCoverageLvm__
#define memCtrlCmdGetSurfaceCompressionCoverageLvm(pMemory, pParams) memCtrlCmdGetSurfaceCompressionCoverageLvm_DISPATCH(pMemory, pParams)
#define memCtrlCmdSurfaceFlushGpuCache_FNPTR(pMemory) pMemory->__memCtrlCmdSurfaceFlushGpuCache__
#define memCtrlCmdSurfaceFlushGpuCache(pMemory, pCacheFlushParams) memCtrlCmdSurfaceFlushGpuCache_DISPATCH(pMemory, pCacheFlushParams)
#define memCtrlCmdGetMemPageSize_FNPTR(pMemory) pMemory->__memCtrlCmdGetMemPageSize__
#define memCtrlCmdGetMemPageSize(pMemory, pPageSizeParams) memCtrlCmdGetMemPageSize_DISPATCH(pMemory, pPageSizeParams)
#define memCtrlCmdSetTag_FNPTR(pMemory) pMemory->__memCtrlCmdSetTag__
#define memCtrlCmdSetTag(pMemory, pParams) memCtrlCmdSetTag_DISPATCH(pMemory, pParams)
#define memCtrlCmdGetTag_FNPTR(pMemory) pMemory->__memCtrlCmdGetTag__
#define memCtrlCmdGetTag(pMemory, pParams) memCtrlCmdGetTag_DISPATCH(pMemory, pParams)
#define memCtrlCmdGetSurfacePhysAttrLvm_FNPTR(pMemory) pMemory->__memCtrlCmdGetSurfacePhysAttrLvm__
#define memCtrlCmdGetSurfacePhysAttrLvm(pMemory, pGPAP) memCtrlCmdGetSurfacePhysAttrLvm_DISPATCH(pMemory, pGPAP)
#define memCtrlCmdGetSurfaceInfoLvm_FNPTR(pMemory) pMemory->__memCtrlCmdGetSurfaceInfoLvm__
#define memCtrlCmdGetSurfaceInfoLvm(pMemory, pSurfaceInfoParams) memCtrlCmdGetSurfaceInfoLvm_DISPATCH(pMemory, pSurfaceInfoParams)
#define memAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define memAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) memAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define memShareCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresShareCallback__
#define memShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) memShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define memControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define memControlSerialization_Prologue(pResource, pCallContext, pParams) memControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define memControlSerialization_Epilogue(pResource, pCallContext, pParams) memControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define memControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define memControl_Prologue(pResource, pCallContext, pParams) memControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define memControl_Epilogue(pResource, pCallContext, pParams) memControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define memCanCopy_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define memCanCopy(pResource) memCanCopy_DISPATCH(pResource)
#define memPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define memPreDestruct(pResource) memPreDestruct_DISPATCH(pResource)
#define memControlFilter_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define memControlFilter(pResource, pCallContext, pParams) memControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define memIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define memIsPartialUnmapSupported(pResource) memIsPartialUnmapSupported_DISPATCH(pResource)
#define memMapTo_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define memMapTo(pResource, pParams) memMapTo_DISPATCH(pResource, pParams)
#define memUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define memUnmapFrom(pResource, pParams) memUnmapFrom_DISPATCH(pResource, pParams)
#define memGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define memGetRefCount(pResource) memGetRefCount_DISPATCH(pResource)
#define memAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define memAddAdditionalDependants(pClient, pResource, pReference) memAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS memIsDuplicate_DISPATCH(struct Memory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__nvoc_metadata_ptr->vtable.__memIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline NV_STATUS memGetMapAddrSpace_DISPATCH(struct Memory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__nvoc_metadata_ptr->vtable.__memGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS memControl_DISPATCH(struct Memory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__nvoc_metadata_ptr->vtable.__memControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS memMap_DISPATCH(struct Memory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_metadata_ptr->vtable.__memMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS memUnmap_DISPATCH(struct Memory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_metadata_ptr->vtable.__memUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS memGetMemInterMapParams_DISPATCH(struct Memory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__nvoc_metadata_ptr->vtable.__memGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS memCheckMemInterUnmap_DISPATCH(struct Memory *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__nvoc_metadata_ptr->vtable.__memCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NV_STATUS memGetMemoryMappingDescriptor_DISPATCH(struct Memory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__nvoc_metadata_ptr->vtable.__memGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS memCheckCopyPermissions_DISPATCH(struct Memory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__nvoc_metadata_ptr->vtable.__memCheckCopyPermissions__(pMemory, pDstGpu, pDstDevice);
}

static inline NV_STATUS memIsReady_DISPATCH(struct Memory *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__nvoc_metadata_ptr->vtable.__memIsReady__(pMemory, bCopyConstructorContext);
}

static inline NvBool memIsGpuMapAllowed_DISPATCH(struct Memory *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__nvoc_metadata_ptr->vtable.__memIsGpuMapAllowed__(pMemory, pGpu);
}

static inline NvBool memIsExportAllowed_DISPATCH(struct Memory *pMemory) {
    return pMemory->__nvoc_metadata_ptr->vtable.__memIsExportAllowed__(pMemory);
}

static inline NV_STATUS memCtrlCmdGetSurfaceCompressionCoverageLvm_DISPATCH(struct Memory *pMemory, NV0041_CTRL_GET_SURFACE_COMPRESSION_COVERAGE_PARAMS *pParams) {
    return pMemory->__memCtrlCmdGetSurfaceCompressionCoverageLvm__(pMemory, pParams);
}

static inline NV_STATUS memCtrlCmdSurfaceFlushGpuCache_DISPATCH(struct Memory *pMemory, NV0041_CTRL_SURFACE_FLUSH_GPU_CACHE_PARAMS *pCacheFlushParams) {
    return pMemory->__memCtrlCmdSurfaceFlushGpuCache__(pMemory, pCacheFlushParams);
}

static inline NV_STATUS memCtrlCmdGetMemPageSize_DISPATCH(struct Memory *pMemory, NV0041_CTRL_GET_MEM_PAGE_SIZE_PARAMS *pPageSizeParams) {
    return pMemory->__memCtrlCmdGetMemPageSize__(pMemory, pPageSizeParams);
}

static inline NV_STATUS memCtrlCmdSetTag_DISPATCH(struct Memory *pMemory, NV0041_CTRL_CMD_SET_TAG_PARAMS *pParams) {
    return pMemory->__memCtrlCmdSetTag__(pMemory, pParams);
}

static inline NV_STATUS memCtrlCmdGetTag_DISPATCH(struct Memory *pMemory, NV0041_CTRL_CMD_GET_TAG_PARAMS *pParams) {
    return pMemory->__memCtrlCmdGetTag__(pMemory, pParams);
}

static inline NV_STATUS memCtrlCmdGetSurfacePhysAttrLvm_DISPATCH(struct Memory *pMemory, NV0041_CTRL_GET_SURFACE_PHYS_ATTR_PARAMS *pGPAP) {
    return pMemory->__memCtrlCmdGetSurfacePhysAttrLvm__(pMemory, pGPAP);
}

static inline NV_STATUS memCtrlCmdGetSurfaceInfoLvm_DISPATCH(struct Memory *pMemory, NV0041_CTRL_GET_SURFACE_INFO_PARAMS *pSurfaceInfoParams) {
    return pMemory->__memCtrlCmdGetSurfaceInfoLvm__(pMemory, pSurfaceInfoParams);
}

static inline NvBool memAccessCallback_DISPATCH(struct Memory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__memAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool memShareCallback_DISPATCH(struct Memory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_metadata_ptr->vtable.__memShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS memControlSerialization_Prologue_DISPATCH(struct Memory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__memControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void memControlSerialization_Epilogue_DISPATCH(struct Memory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__memControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memControl_Prologue_DISPATCH(struct Memory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__memControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void memControl_Epilogue_DISPATCH(struct Memory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__memControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool memCanCopy_DISPATCH(struct Memory *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__memCanCopy__(pResource);
}

static inline void memPreDestruct_DISPATCH(struct Memory *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__memPreDestruct__(pResource);
}

static inline NV_STATUS memControlFilter_DISPATCH(struct Memory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__memControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool memIsPartialUnmapSupported_DISPATCH(struct Memory *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__memIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS memMapTo_DISPATCH(struct Memory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__memMapTo__(pResource, pParams);
}

static inline NV_STATUS memUnmapFrom_DISPATCH(struct Memory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__memUnmapFrom__(pResource, pParams);
}

static inline NvU32 memGetRefCount_DISPATCH(struct Memory *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__memGetRefCount__(pResource);
}

static inline void memAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct Memory *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__memAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS memIsDuplicate_IMPL(struct Memory *pMemory, NvHandle hMemory, NvBool *pDuplicate);

NV_STATUS memGetMapAddrSpace_IMPL(struct Memory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);

NV_STATUS memControl_IMPL(struct Memory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

NV_STATUS memMap_IMPL(struct Memory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);

NV_STATUS memUnmap_IMPL(struct Memory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);

NV_STATUS memGetMemInterMapParams_IMPL(struct Memory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams);

static inline NV_STATUS memCheckMemInterUnmap_ac1694(struct Memory *pMemory, NvBool bSubdeviceHandleProvided) {
    return NV_OK;
}

NV_STATUS memGetMemoryMappingDescriptor_IMPL(struct Memory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc);

static inline NV_STATUS memCheckCopyPermissions_ac1694(struct Memory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return NV_OK;
}

NV_STATUS memIsReady_IMPL(struct Memory *pMemory, NvBool bCopyConstructorContext);

static inline NvBool memIsGpuMapAllowed_e661f0(struct Memory *pMemory, struct OBJGPU *pGpu) {
    return NV_TRUE;
}

static inline NvBool memIsExportAllowed_e661f0(struct Memory *pMemory) {
    return NV_TRUE;
}

NV_STATUS memCtrlCmdGetSurfaceCompressionCoverageLvm_IMPL(struct Memory *pMemory, NV0041_CTRL_GET_SURFACE_COMPRESSION_COVERAGE_PARAMS *pParams);

NV_STATUS memCtrlCmdSurfaceFlushGpuCache_IMPL(struct Memory *pMemory, NV0041_CTRL_SURFACE_FLUSH_GPU_CACHE_PARAMS *pCacheFlushParams);

NV_STATUS memCtrlCmdGetMemPageSize_IMPL(struct Memory *pMemory, NV0041_CTRL_GET_MEM_PAGE_SIZE_PARAMS *pPageSizeParams);

NV_STATUS memCtrlCmdSetTag_IMPL(struct Memory *pMemory, NV0041_CTRL_CMD_SET_TAG_PARAMS *pParams);

NV_STATUS memCtrlCmdGetTag_IMPL(struct Memory *pMemory, NV0041_CTRL_CMD_GET_TAG_PARAMS *pParams);

NV_STATUS memCtrlCmdGetSurfacePhysAttrLvm_IMPL(struct Memory *pMemory, NV0041_CTRL_GET_SURFACE_PHYS_ATTR_PARAMS *pGPAP);

NV_STATUS memCtrlCmdGetSurfaceInfoLvm_IMPL(struct Memory *pMemory, NV0041_CTRL_GET_SURFACE_INFO_PARAMS *pSurfaceInfoParams);

NV_STATUS memConstruct_IMPL(struct Memory *arg_pMemory, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_memConstruct(arg_pMemory, arg_pCallContext, arg_pParams) memConstruct_IMPL(arg_pMemory, arg_pCallContext, arg_pParams)
NV_STATUS memCopyConstruct_IMPL(struct Memory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);

#ifdef __nvoc_mem_h_disabled
static inline NV_STATUS memCopyConstruct(struct Memory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Memory was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_h_disabled
#define memCopyConstruct(pMemory, pCallContext, pParams) memCopyConstruct_IMPL(pMemory, pCallContext, pParams)
#endif //__nvoc_mem_h_disabled

void memDestruct_IMPL(struct Memory *pMemory);

#define __nvoc_memDestruct(pMemory) memDestruct_IMPL(pMemory)
NV_STATUS memConstructCommon_IMPL(struct Memory *pMemory, NvU32 categoryClassId, NvU32 flags, MEMORY_DESCRIPTOR *pMemDesc, NvU32 heapOwner, struct Heap *pHeap, NvU32 attr, NvU32 attr2, NvU32 Pitch, NvU32 type, NvU32 tag, HWRESOURCE_INFO *pHwResource);

#ifdef __nvoc_mem_h_disabled
static inline NV_STATUS memConstructCommon(struct Memory *pMemory, NvU32 categoryClassId, NvU32 flags, MEMORY_DESCRIPTOR *pMemDesc, NvU32 heapOwner, struct Heap *pHeap, NvU32 attr, NvU32 attr2, NvU32 Pitch, NvU32 type, NvU32 tag, HWRESOURCE_INFO *pHwResource) {
    NV_ASSERT_FAILED_PRECOMP("Memory was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_h_disabled
#define memConstructCommon(pMemory, categoryClassId, flags, pMemDesc, heapOwner, pHeap, attr, attr2, Pitch, type, tag, pHwResource) memConstructCommon_IMPL(pMemory, categoryClassId, flags, pMemDesc, heapOwner, pHeap, attr, attr2, Pitch, type, tag, pHwResource)
#endif //__nvoc_mem_h_disabled

void memDestructCommon_IMPL(struct Memory *pMemory);

#ifdef __nvoc_mem_h_disabled
static inline void memDestructCommon(struct Memory *pMemory) {
    NV_ASSERT_FAILED_PRECOMP("Memory was disabled!");
}
#else //__nvoc_mem_h_disabled
#define memDestructCommon(pMemory) memDestructCommon_IMPL(pMemory)
#endif //__nvoc_mem_h_disabled

NV_STATUS memCreateMemDesc_IMPL(struct OBJGPU *pGpu, MEMORY_DESCRIPTOR **ppMemDesc, NV_ADDRESS_SPACE addrSpace, NvU64 FBOffset, NvU64 length, NvU32 attr, NvU32 attr2);

#define memCreateMemDesc(pGpu, ppMemDesc, addrSpace, FBOffset, length, attr, attr2) memCreateMemDesc_IMPL(pGpu, ppMemDesc, addrSpace, FBOffset, length, attr, attr2)
NV_STATUS memCreateKernelMapping_IMPL(struct Memory *pMemory, NvU32 Protect, NvBool bClear);

#ifdef __nvoc_mem_h_disabled
static inline NV_STATUS memCreateKernelMapping(struct Memory *pMemory, NvU32 Protect, NvBool bClear) {
    NV_ASSERT_FAILED_PRECOMP("Memory was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_mem_h_disabled
#define memCreateKernelMapping(pMemory, Protect, bClear) memCreateKernelMapping_IMPL(pMemory, Protect, bClear)
#endif //__nvoc_mem_h_disabled

NV_STATUS memGetByHandle_IMPL(struct RsClient *pClient, NvHandle hMemory, struct Memory **ppMemory);

#define memGetByHandle(pClient, hMemory, ppMemory) memGetByHandle_IMPL(pClient, hMemory, ppMemory)
NV_STATUS memGetByHandleAndDevice_IMPL(struct RsClient *pClient, NvHandle hMemory, NvHandle hDevice, struct Memory **ppMemory);

#define memGetByHandleAndDevice(pClient, hMemory, hDevice, ppMemory) memGetByHandleAndDevice_IMPL(pClient, hMemory, hDevice, ppMemory)
NV_STATUS memGetByHandleAndGroupedGpu_IMPL(struct RsClient *pClient, NvHandle hMemory, struct OBJGPU *pGpu, struct Memory **ppMemory);

#define memGetByHandleAndGroupedGpu(pClient, hMemory, pGpu, ppMemory) memGetByHandleAndGroupedGpu_IMPL(pClient, hMemory, pGpu, ppMemory)
NV_STATUS memRegisterWithGsp_IMPL(struct OBJGPU *pGpu, struct RsClient *pClient, NvHandle hParent, NvHandle hMemory);

#define memRegisterWithGsp(pGpu, pClient, hParent, hMemory) memRegisterWithGsp_IMPL(pGpu, pClient, hParent, hMemory)
void memSetSysmemCacheAttrib_IMPL(struct OBJGPU *pGpu, NV_MEMORY_ALLOCATION_PARAMS *pAllocData, NvU32 *pCpuCacheAttrib, NvU32 *pGpuCacheAttrib);

#define memSetSysmemCacheAttrib(pGpu, pAllocData, pCpuCacheAttrib, pGpuCacheAttrib) memSetSysmemCacheAttrib_IMPL(pGpu, pAllocData, pCpuCacheAttrib, pGpuCacheAttrib)
#undef PRIVATE_FIELD


#endif


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_MEM_NVOC_H_
