#ifndef _G_MEM_DESC_NVOC_H_
#define _G_MEM_DESC_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_mem_desc_nvoc.h"

#ifndef _MEMDESC_H_
#define _MEMDESC_H_

#include "core/prelude.h"
#include "poolalloc.h"


struct OBJVASPACE;

#ifndef __NVOC_CLASS_OBJVASPACE_TYPEDEF__
#define __NVOC_CLASS_OBJVASPACE_TYPEDEF__
typedef struct OBJVASPACE OBJVASPACE;
#endif /* __NVOC_CLASS_OBJVASPACE_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJVASPACE
#define __nvoc_class_id_OBJVASPACE 0x6c347f
#endif /* __nvoc_class_id_OBJVASPACE */


struct OBJGPU;

#ifndef __NVOC_CLASS_OBJGPU_TYPEDEF__
#define __NVOC_CLASS_OBJGPU_TYPEDEF__
typedef struct OBJGPU OBJGPU;
#endif /* __NVOC_CLASS_OBJGPU_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPU
#define __nvoc_class_id_OBJGPU 0x7ef3cb
#endif /* __nvoc_class_id_OBJGPU */


struct Heap;

#ifndef __NVOC_CLASS_Heap_TYPEDEF__
#define __NVOC_CLASS_Heap_TYPEDEF__
typedef struct Heap Heap;
#endif /* __NVOC_CLASS_Heap_TYPEDEF__ */

#ifndef __nvoc_class_id_Heap
#define __nvoc_class_id_Heap 0x556e9a
#endif /* __nvoc_class_id_Heap */


struct MEMORY_DESCRIPTOR;

typedef struct CTX_BUF_POOL_INFO CTX_BUF_POOL_INFO;
typedef struct COMPR_INFO COMPR_INFO;

//
// Address space identifiers.
//
typedef NvU32      NV_ADDRESS_SPACE;
#define ADDR_UNKNOWN    0         // Address space is unknown
#define ADDR_SYSMEM     1         // System memory (PCI)
#define ADDR_FBMEM      2         // Frame buffer memory space
#define ADDR_REGMEM     3         // NV register memory space
#define ADDR_VIRTUAL    4         // Virtual address space only
#define ADDR_FABRIC     5         // Fabric address space for the GPA based addressing.
#define ADDR_FABRIC_V2  6         // Fabric address space for the FLA based addressing. Will replace ADDR_FABRIC.

//
// Address translation identifiers:
//
// Memory descriptors are used to describe physical block(s) of memory.
// That memory can be described at various levels of address translation
// using the address translation (AT) enumerates. The levels of translation
// supported is illustrated below.
//
// The diagram is drawn for system memory with SR-IOV but the translations
// are similar for video memory (replace IOMMU with VMMU). VGPU pre-SR-IOV
// is also different.
//
// +-------------------+           +-------------------+
// |       CPU         |           |     GPU Engine    |
// +-------------------+           +-------------------+
//          |                               |
//          |                               | GPU VA
//          |                               V
//          |                      +-------------------+
//          | CPU VA               |       GMMU        |
//          |                      +-------------------+
//          |                               |
//          |                               | GPU GPA (AT_GPU)
//          v                               v
// +-------------------+           +-------------------+
// |  MMU (1st level)| |           | IOMMU (1st level) |
// +-------------------+           +-------------------+
//          |                               |
//          | CPU GPA (AT_CPU)              |                   <---- AT_PA for VGPU guest
//          v                               v
// +-------------------+           +-------------------+
// |  MMU (2nd level)  |           | IOMMU (2nd level) |
// +-------------------+           +-------------------+
//          |                               |
//          | SPA                           | SPA               <---- AT_PA for bare metal
//          v                               v                         or VGPU host
// +---------------------------------------------------+
// |                System Memory                      |
// +---------------------------------------------------+
//
//
// Descriptions for *physical* address translation levels:
//
// AT_CPU - CPU physical address or guest physical address (GPA)
// AT_GPU - GPU physical address or guest physical address (GPA)
// AT_PA  - When running in host RM or bare metal this is the system physical address. When
//          running inside a VGPU guest environment, this is the last level of translation
//          visible to the OS context that RM is running in.
//
// AT_CPU should typically == AT_PA, but there might be cases such as IBM P9 where vidmem
// might be 0-based on GPU but exposed elsewhere in the CPU address space.
//
// Descriptions for *virtual* address translation levels:
//
// AT_GPU_VA - Memory descriptors can also describe virtual memory allocations. AT_GPU_VA
//             represents a GMMU virtual address.
//
#define AT_CPU      AT_VARIANT(0)
#define AT_GPU      AT_VARIANT(1)
#define AT_PA       AT_VARIANT(2)

#define AT_GPU_VA   AT_VARIANT(3)

//
// TODO - switch to using numeric values for AT_XYZ. Using pointers for
// typesafety after initial split from using class IDs/mmuContext
//
typedef struct ADDRESS_TRANSLATION_ *ADDRESS_TRANSLATION;
#define AT_VARIANT(x)  ((struct ADDRESS_TRANSLATION_ *)x)
#define AT_VALUE(x)    ((NvU64)(NvUPtr)(x))

//
// Overrides address translation in SR-IOV enabled usecases
//
// In SRIOV systems, an access from guest has to go through the following
// translations:
//
// GVA -> GPA -> SPA
//
// Given HOST manages channel/memory management for guest, there are certain
// code paths that expects VA -> GPA translations and some may need GPA -> SPA
// translations. We use address translation to differentiate between these
// cases.
//
// We use AT_PA to force GPA -> SPA translation for vidmem. In case of non-SRIOV systems,
// using IO_VASPACE_A will fall back to FERMI_VASPACE_A or default context.
//
#define FORCE_VMMU_TRANSLATION(pMemDesc, curAddressTranslation) \
    ((memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM) ? AT_PA : curAddressTranslation)

typedef struct _memdescDestroyCallback MEM_DESC_DESTROY_CALLBACK;

typedef void (MEM_DATA_RELEASE_CALL_BACK)(struct MEMORY_DESCRIPTOR *);

//
// A memory descriptor is an object that describes and can be used to manipulate
// a block of memory.  The memory can be video or system memory; it can be
// contiguous or noncontiguous; it can be tiled, block linear, etc.  However,
// regardless of what type of memory it is, clients can use a standard set of
// APIs to manipulate it.
//
DECLARE_INTRUSIVE_LIST(MEMORY_DESCRIPTOR_LIST);

typedef struct MEMORY_DESCRIPTOR
{
    // The GPU that this memory belongs to
    OBJGPU *pGpu;

    // Flags field for optional behavior
    NvU64 _flags;

    // Size of mapping used for this allocation.  Multiple mappings on Fermi must always use the same page size.
    NvU32 _pageSize;

    // Size of the memory allocation in pages
    NvU64 PageCount;

    // Alignment of the memory allocation as size in bytes
    // XXX: would 32b work here?
    NvU64 Alignment;

    // Size of the memory allocation requested in bytes
    NvU64 Size;

    // Actual size of memory allocated to satisfy alignment.
    // We report the requested size, not the actual size. A number of callers
    // depend on this.
    NvU64 ActualSize;

    // The information returned from osAllocPages
    NvP64 _address;
    void *_pMemData;
    MEM_DATA_RELEASE_CALL_BACK *_pMemDataReleaseCallback;

    // When memory is allocated by a guest Virtual Machine (VM)
    // it is aliased by the host RM. We store a unique guest ID
    // for each piece of aliased memory to facilitate host RM mappings
    // to these pages (only in case of system memory).
    // XXX: would 32b work here?
    NvU64 _guestId;

    // To keep track of the offset from parent memdesc
    NvU64 subMemOffset;

    //
    // The byte offset at which the memory allocation begins within the first
    // PTE.  To locate the physical address of the byte at offset i in the memory
    // allocation, use the following logic:
    //   i += PteAdjust;
    //   if (PhysicallyContiguous)
    //       PhysAddr = PteArray[0] + i;
    //   else
    //       PhysAddr = PteArray[i >> RM_PAGE_SHIFT] + (i & RM_PAGE_MASK);
    //
    NvU32 PteAdjust;

    // Has the memory been allocated yet?
    NvBool Allocated;

    //
    // Marks that a request to deallocate memory has been called on this memdesc while it had multiple references
    // NV_TRUE denotes that memFree will be called when refcount reaches 0.
    //
    NvBool bDeferredFree;

    // Does this use SUBALLOCATOR?
    NvBool bUsingSuballocator;

    // Where does the memory live?  Video, system, other
    NV_ADDRESS_SPACE _addressSpace;

    // Attributes reflecting GPU caching of this memory.
    NvU32 _gpuCacheAttrib;

    // Peer vid mem cacheability
    NvU32 _gpuP2PCacheAttrib;

    // One of NV_MEMORY_CACHED, NV_MEMORY_UNCACHED, NV_MEMORY_WRITECOMBINED
    NvU32 _cpuCacheAttrib;

    // The page kind of this memory
    NvU32 _pteKind;
    NvU32 _pteKindCompressed;

    //
    // Scale memory allocation by this value
    //
    NvU32 _subDeviceAllocCount;

    //
    // Reference count for the object.
    //
    NvU32 RefCount;

    // Reference count for duplication of memory object via RmDupObject.
    NvU32 DupCount;

    //
    // The HwResId is used by the device dependent HAL to keep track of
    // resources attached to the memory (e.g.: compression tags, zcull).
    //
    NvU32 _hwResId;

    //
    // Keep track which heap is actually used for this allocation
    //
    struct Heap *pHeap;

    //
    // GFID that this memory allocation belongs to
    //
    NvU32    gfid;

    //
    // Keep track of the PMA_ALLOC_INFO data.
    //
    struct PMA_ALLOC_INFO *pPmaAllocInfo;

    // Serve as head node in a list of page handles
    PoolPageHandleList *pPageHandleList;

    //
    // List of callbacks to call when destroying memory descriptor
    //
    MEM_DESC_DESTROY_CALLBACK *_pMemDestroyCallbackList;

    // pointer to descriptor which was used to subset current descriptor
    struct MEMORY_DESCRIPTOR *_pParentDescriptor;

    // Count used for sanity check
    NvU32 childDescriptorCnt;

    // Next memory descriptor in subdevice list
    struct MEMORY_DESCRIPTOR *_pNext;

    // Pointer to system Memory descriptor which used to back some FB content across S3/S4.
    struct MEMORY_DESCRIPTOR *_pStandbyBuffer;

    // Serve as a head node in a list of submemdescs
    MEMORY_DESCRIPTOR_LIST *pSubMemDescList;

    // If strung in a intrusive linked list
    ListNode   node;

    //
    // Pointer to IOVA mappings used to back the IOMMU VAs for different IOVA spaces
    // Submemory descriptors only have on mapping, but the root descriptor will have
    // one per IOVA space that the memory is mapped into.
    //
    struct IOVAMAPPING *_pIommuMappings;

    // Kernel mapping of the memory
    NvP64 _kernelMapping;
    NvP64 _kernelMappingPriv;

    // Internal mapping
    void *_pInternalMapping;
    void *_pInternalMappingPriv;
    NvU32 _internalMappingRefCount;

    // Array to hold SPA addresses when memdesc is allocated from GPA. Valid only for SRIOV cases
    RmPhysAddr *pPteSpaMappings;

    //
    // context buffer pool from which this memdesc is to be allocated.
    // This is controlled by PDB_PROP_GPU_MOVE_RM_BUFFERS_TO_PMA which is
    // enabled only for SMC today
    //
    CTX_BUF_POOL_INFO *pCtxBufPool;

    // Max physical address width to be override
    NvU32 _overridenAddressWidth;

    // We verified that memdesc is safe to be mapped as large pages
    NvBool bForceHugePages;

    //
    // If PhysicallyContiguous is NV_TRUE, this array consists of one element.
    // If PhysicallyContiguous is NV_FALSE, this array is actually larger and has
    // one entry for each physical page in the memory allocation.  As a result,
    // this structure must be allocated from the heap.
    // If the AddressSpace is ADDR_FBMEM, each entry is an FB offset.
    // Otherwise, each entry is a physical address on the system bus.
    // TBD: for now, the array will be sized at one entry for every 4KB, but
    // we probably want to optimize this later to support 64KB pages.
    //
    RmPhysAddr _pteArray[1];
    //!!! Place nothing behind PteArray!!!
} MEMORY_DESCRIPTOR, *PMEMORY_DESCRIPTOR;

MAKE_INTRUSIVE_LIST(MEMORY_DESCRIPTOR_LIST, MEMORY_DESCRIPTOR, node);

//
// Common address space lists
//
extern const NV_ADDRESS_SPACE ADDRLIST_FBMEM_PREFERRED[];
extern const NV_ADDRESS_SPACE ADDRLIST_SYSMEM_PREFERRED[];
extern const NV_ADDRESS_SPACE ADDRLIST_FBMEM_ONLY[];
extern const NV_ADDRESS_SPACE ADDRLIST_SYSMEM_ONLY[];

NvU32 memdescAddrSpaceListToU32(const NV_ADDRESS_SPACE *addrlist);
const NV_ADDRESS_SPACE *memdescU32ToAddrSpaceList(NvU32 index);

NV_STATUS _memdescUpdateSpaArray(PMEMORY_DESCRIPTOR   pMemDesc);
// Create a memory descriptor data structure (without allocating any physical
// storage).
NV_STATUS memdescCreate(MEMORY_DESCRIPTOR **ppMemDesc, OBJGPU *pGpu, NvU64 Size,
                        NvU64 alignment, NvBool PhysicallyContiguous,
                        NV_ADDRESS_SPACE AddressSpace, NvU32 CpuCacheAttrib, NvU64 Flags);

#define MEMDESC_FLAGS_ALLOC_PER_SUBDEVICE_FB_BC_ONLY(pGpu, addressSpace) \
    ((gpumgrGetBcEnabledStatus(pGpu) && (pGpu != NULL) && (addressSpace == ADDR_FBMEM)) ?  MEMDESC_FLAGS_ALLOC_PER_SUBDEVICE : MEMDESC_FLAGS_NONE)

// Initialize a caller supplied memory descriptor for use with memdescDescribe()
void memdescCreateExisting(MEMORY_DESCRIPTOR *pMemDesc, OBJGPU *pGpu, NvU64 Size,
                           NV_ADDRESS_SPACE AddressSpace,
                           NvU32 CpuCacheAttrib, NvU64 Flags);

// Increment reference count
void memdescAddRef(MEMORY_DESCRIPTOR *pMemDesc);

// Decrement reference count
void memdescRemoveRef(MEMORY_DESCRIPTOR *pMemDesc);

// Decrement reference count and reclaim any resources when possible
void memdescDestroy(MEMORY_DESCRIPTOR *pMemDesc);

//
// The destroy callback is called when the memory descriptor is
// destroyed with memdescDestroy().
//
// The caller is responsible for managing the memory used
// containing the callback.
//
typedef void (MemDescDestroyCallBack)(OBJGPU *, void *pObject, MEMORY_DESCRIPTOR *);
struct _memdescDestroyCallback
{
    MemDescDestroyCallBack *destroyCallback;
    void *pObject;
    MEM_DESC_DESTROY_CALLBACK *pNext;
};
void memdescAddDestroyCallback(MEMORY_DESCRIPTOR *pMemDesc, MEM_DESC_DESTROY_CALLBACK *);
void memdescRemoveDestroyCallback(MEMORY_DESCRIPTOR *pMemDesc, MEM_DESC_DESTROY_CALLBACK *);

// Allocate physical storage for a memory descriptor and fill in its PteArray
NV_STATUS memdescAlloc(MEMORY_DESCRIPTOR *pMemDesc);

// Allocate memory from one of the possible locations specified in pList.
NV_STATUS memdescAllocList(MEMORY_DESCRIPTOR *pMemDesc, const NV_ADDRESS_SPACE *pList);

// Free physical storage for a memory descriptor
void memdescFree(MEMORY_DESCRIPTOR *pMemDesc);

// Lock the paged virtual memory
NV_STATUS memdescLock(MEMORY_DESCRIPTOR *pMemDesc);

// Unlock the paged virtual memory
NV_STATUS memdescUnlock(MEMORY_DESCRIPTOR *pMemDesc);

// Allocate a CPU mapping of an arbitrary subrange of the memory.
// 64-bit clean (mac can have a 32-bit kernel pointer and 64-bit client pointers)
NV_STATUS memdescMap(MEMORY_DESCRIPTOR *pMemDesc, NvU64 Offset, NvU64 Size,
                     NvBool Kernel, NvU32 Protect, NvP64 *pAddress, NvP64 *pPriv);

// Free a CPU mapping of an arbitrary subrange of the memory.
void memdescUnmap(MEMORY_DESCRIPTOR *pMemDesc, NvBool Kernel, NvU32 ProcessId,
                  NvP64 Address, NvP64 Priv);

// Allocate a CPU mapping of an arbitrary subrange of the memory.
// fails unless Kernel == NV_TRUE
NV_STATUS memdescMapOld(MEMORY_DESCRIPTOR *pMemDesc, NvU64 Offset, NvU64 Size,
                        NvBool Kernel, NvU32 Protect, void **pAddress, void **pPriv);

// Free a CPU mapping of an arbitrary subrange of the memory.
void memdescUnmapOld(MEMORY_DESCRIPTOR *pMemDesc, NvBool Kernel, NvU32 ProcessId,
                     void *Address, void *Priv);

// Fill in a MEMORY_DESCRIPTOR with a description of a preexisting contiguous
// memory allocation.  It should already be initialized with
// memdescCreate*().
void memdescDescribe(MEMORY_DESCRIPTOR *pMemDesc,
                     NV_ADDRESS_SPACE AddressSpace,
                     RmPhysAddr Base, NvU64 Size);

// Fill in a MEMORY_DESCRIPTOR with the physical page addresses returned by PMA.
// It should already be initialized with memdescCreate*().
void memdescFillPages(MEMORY_DESCRIPTOR *pMemDesc, NvU32 offset,
                      NvU64 *pPages, NvU32 pageCount, NvU32 pageSize);

// Create a MEMORY_DESCRIPTOR for a subset of an existing memory allocation.
// The new MEMORY_DESCRIPTOR must be freed with memdescDestroy.
NV_STATUS memdescCreateSubMem(MEMORY_DESCRIPTOR **ppMemDescNew,
                              MEMORY_DESCRIPTOR *pMemDesc,
                              OBJGPU *pGpu, NvU64 Offset, NvU64 Size);

// Compute the physical address of a byte within a MEMORY_DESCRIPTOR
RmPhysAddr memdescGetPhysAddr(MEMORY_DESCRIPTOR *pMemDesc, ADDRESS_TRANSLATION addressTranslation, NvU64 offset);

// Compute count physical addresses within a MEMORY_DESCRIPTOR. Starting at the
// given offset and advancing it by stride for each consecutive address.
void memdescGetPhysAddrs(MEMORY_DESCRIPTOR *pMemDesc,
                         ADDRESS_TRANSLATION addressTranslation,
                         NvU64 offset,
                         NvU64 stride,
                         NvU64 count,
                         RmPhysAddr *pAddresses);

// Compute count physical addresses within a MEMORY_DESCRIPTOR for a specific
// GPU. Starting at the given offset and advancing it by stride for each
// consecutive address.
void memdescGetPhysAddrsForGpu(MEMORY_DESCRIPTOR *pMemDesc,
                               OBJGPU *pGpu,
                               ADDRESS_TRANSLATION addressTranslation,
                               NvU64 offset,
                               NvU64 stride,
                               NvU64 count,
                               RmPhysAddr *pAddresses);

// Obtains one of the PTEs from the MEMORY_DESCRIPTOR.  Assumes 4KB pages,
// and works for either contiguous or noncontiguous descriptors.
RmPhysAddr memdescGetPte(MEMORY_DESCRIPTOR *pMemDesc, ADDRESS_TRANSLATION addressTranslation, NvU32 PteIndex);

void memdescSetPte(PMEMORY_DESCRIPTOR pMemDesc, ADDRESS_TRANSLATION addressTranslation, NvU32 PteIndex, RmPhysAddr PhysAddr);

// Obtains the PteArray from the MEMORY_DESCRIPTOR for the specified GPU.
RmPhysAddr * memdescGetPteArrayForGpu(MEMORY_DESCRIPTOR *pMemDesc, OBJGPU *pGpu, ADDRESS_TRANSLATION addressTranslation);

/*!
 *  @brief Obtains the PteArray from the MEMORY_DESCRIPTOR.
 *
 *  @param[in]  pMemDesc           Memory descriptor to use
 *  @param[in]  addressTranslation Address translation identifier
 *
 *  @returns PageArray
 */
static inline RmPhysAddr *
memdescGetPteArray(PMEMORY_DESCRIPTOR  pMemDesc, ADDRESS_TRANSLATION addressTranslation)
{
    return memdescGetPteArrayForGpu(pMemDesc, pMemDesc->pGpu, addressTranslation);
}

// Obtains the PteArray size from the MEMORY_DESCRIPTOR based on the mmuContext.
NvU32 memdescGetPteArraySize(MEMORY_DESCRIPTOR *pMemDesc, ADDRESS_TRANSLATION addressTranslation);

// Return the aperture of the NV_ADDRESS_SPACE as a null terminated string.
// Useful for print statements.
const char* memdescGetApertureString(NV_ADDRESS_SPACE addrSpace);

// Return true if two MEMORY_DESCRIPTOR are equal
NvBool memdescDescIsEqual(MEMORY_DESCRIPTOR *pMemDescOne, MEMORY_DESCRIPTOR *pMemDescTwo);

// Retrieve the per-GPU memory descriptor for a subdevice
MEMORY_DESCRIPTOR *memdescGetMemDescFromSubDeviceInst(MEMORY_DESCRIPTOR *pMemDesc, NvU32 subDeviceInst);

// Retrieve the per-GPU memory descriptor for a GPU
MEMORY_DESCRIPTOR *memdescGetMemDescFromGpu(MEMORY_DESCRIPTOR *pMemDesc, OBJGPU *pGpu);

// Retrieve the per-GPU memory descriptor at an index
MEMORY_DESCRIPTOR *memdescGetMemDescFromIndex(MEMORY_DESCRIPTOR *pMemDesc, NvU32 index);

// Print information on memory descriptor
void memdescPrintMemdesc(MEMORY_DESCRIPTOR *pMemDesc, NvBool bPrintIndividualPages, const char *pPrefixMessage);

// Get the page offset for an arbitrary power of two page size
NvU64 memdescGetPageOffset(MEMORY_DESCRIPTOR *pMemDesc, NvU32 pageSize);

//
// Internal APIs for the IOVASPACE to manage IOMMU mappings in a memdesc.
//
// Note that the external APIs are memdescMapIommu(),
// memdescUnmapIommu() and memdescGetIommuMap().
//
NV_STATUS memdescAddIommuMap(PMEMORY_DESCRIPTOR pMemDesc, struct IOVAMAPPING *pIommuMap);
void memdescRemoveIommuMap(PMEMORY_DESCRIPTOR pMemDesc, struct IOVAMAPPING *pIommuMap);

//
// Map and unmap IOMMU for the specified VA space
//
// Each memdescUnmapIommu() call has to be paired with a previous successful
// memdescMapIommu() call for the same VA space. The calls are refcounted for
// each VA space and only the last Unmap will remove the mappings.
//
// The caller has to guarantee that before the VA space is destroyed, either the
// mapping is explicitly unmapped with memdescUnmapIommu() or the memdesc is
// freed (or destroyed for memdescs that are not memdescFree()d).
//
NV_STATUS memdescMapIommu(PMEMORY_DESCRIPTOR pMemDesc, NvU32 vaspaceId);
void memdescUnmapIommu(PMEMORY_DESCRIPTOR pMemDesc, NvU32 vaspaceId);

// Returns the IOVA mapping created by memdescMapIommu().
struct IOVAMAPPING *memdescGetIommuMap(PMEMORY_DESCRIPTOR pMemDesc, NvU32 vaspaceId);

//
// Check subdevice consistency functions
//
void memdescCheckSubDevicePageSizeConsistency(OBJGPU *pGpu, PMEMORY_DESCRIPTOR pMemDesc, struct OBJVASPACE *pVAS,
                                              NvU64 pageSize, NvU64 pageOffset);
void memdescCheckSubDeviceMemContiguityConsistency(OBJGPU *pGpu, PMEMORY_DESCRIPTOR pMemDesc, struct OBJVASPACE *pVAS,
                                                   NvBool bIsMemContiguous);
NV_STATUS memdescCheckSubDeviceKindComprConsistency(OBJGPU *pGpu, PMEMORY_DESCRIPTOR pMemDesc, struct OBJVASPACE *pVAS,
                                                    NvU32 kind, COMPR_INFO *pComprInfo);

//
// Accessor functions
//
void memdescSetHeapOffset(MEMORY_DESCRIPTOR *pMemDesc, RmPhysAddr fbOffset);
void memdescSetCpuCacheAttrib(MEMORY_DESCRIPTOR *pMemDesc, NvU32 cpuCacheAttrib);
void memdescSetGpuCacheAttrib(MEMORY_DESCRIPTOR *pMemDesc, NvU32 GpuCacheAttrib);
NvU32 memdescGetGpuP2PCacheAttrib(MEMORY_DESCRIPTOR *pMemDesc);
void memdescSetGpuP2PCacheAttrib(MEMORY_DESCRIPTOR *pMemDesc, NvU32 GpuCacheAttrib);
NvU32 memdescGetPteKindForGpu(MEMORY_DESCRIPTOR *pMemDesc, OBJGPU *pGpu);
void  memdescSetPteKindForGpu(MEMORY_DESCRIPTOR *pMemDesc, OBJGPU *pGpu, NvU32 pteKind);
NvU32 memdescGetPteKindCompressed(MEMORY_DESCRIPTOR *pMemDesc);
void  memdescSetPteKindCompressed(MEMORY_DESCRIPTOR *pMemDesc, NvU32 pteKindCmpr);
NvP64 memdescGetKernelMapping(MEMORY_DESCRIPTOR *pMemDesc);
void  memdescSetKernelMapping(MEMORY_DESCRIPTOR *pMemDesc, NvP64 kernelMapping);
NvP64 memdescGetKernelMappingPriv(MEMORY_DESCRIPTOR *pMemDesc);
void  memdescSetKernelMappingPriv(MEMORY_DESCRIPTOR *pMemDesc, NvP64 kernelMappingPriv);
MEMORY_DESCRIPTOR *memdescGetStandbyBuffer(MEMORY_DESCRIPTOR *pMemDesc);
void memdescSetStandbyBuffer(MEMORY_DESCRIPTOR *pMemDesc, MEMORY_DESCRIPTOR *pStandbyBuffer);
void memdescSetDestroyCallbackList(MEMORY_DESCRIPTOR *pMemDesc, MEM_DESC_DESTROY_CALLBACK *pCb);
NvU64 memdescGetGuestId(MEMORY_DESCRIPTOR *pMemDesc);
void memdescSetGuestId(MEMORY_DESCRIPTOR *pMemDesc, NvU64 guestId);
NvBool memdescGetFlag(MEMORY_DESCRIPTOR *pMemDesc, NvU64 flag);
void memdescSetFlag(MEMORY_DESCRIPTOR *pMemDesc, NvU64 flag, NvBool bValue);
NvP64 memdescGetAddress(MEMORY_DESCRIPTOR *pMemDesc);
void memdescSetAddress(MEMORY_DESCRIPTOR *pMemDesc, NvP64 pAddress);
void *memdescGetMemData(MEMORY_DESCRIPTOR *pMemDesc);
void memdescSetMemData(MEMORY_DESCRIPTOR *pMemDesc, void *pMemData, MEM_DATA_RELEASE_CALL_BACK *pMemDataReleaseCallback);
NvBool memdescGetVolatility(MEMORY_DESCRIPTOR *pMemDesc);
NvBool memdescGetContiguity(PMEMORY_DESCRIPTOR pMemDesc, ADDRESS_TRANSLATION addressTranslation);
void memdescSetContiguity(PMEMORY_DESCRIPTOR pMemDesc, ADDRESS_TRANSLATION addressTranslation, NvBool isContiguous);
NvBool memdescCheckContiguity(PMEMORY_DESCRIPTOR pMemDesc, ADDRESS_TRANSLATION addressTranslation);
NV_ADDRESS_SPACE memdescGetAddressSpace(PMEMORY_DESCRIPTOR pMemDesc);
NvU32 memdescGetPageSize(MEMORY_DESCRIPTOR *pMemDesc, ADDRESS_TRANSLATION addressTranslation);
void  memdescSetPageSize(MEMORY_DESCRIPTOR *pMemDesc, ADDRESS_TRANSLATION addressTranslation, NvU32 pageSize);
PMEMORY_DESCRIPTOR memdescGetRootMemDesc(PMEMORY_DESCRIPTOR pMemDesc, NvU64 *pRootOffset);
void memdescSetCustomHeap(PMEMORY_DESCRIPTOR);
NvBool memdescGetCustomHeap(PMEMORY_DESCRIPTOR);

/*!
 *  @brief Get PTE kind
 *
 *  @param[in]  pMemDesc           Memory descriptor pointer
 *  @param[in]  addressTranslation Address translation identifier
 *
 *  @returns Current PTE kind value.
 */
static inline NvU32
memdescGetPteKind(PMEMORY_DESCRIPTOR pMemDesc)
{
    return memdescGetPteKindForGpu(pMemDesc,  pMemDesc->pGpu);
}

/*!
 *  @brief Set PTE kind.
 *
 *  @param[in]  pMemDesc           Memory descriptor pointer
 *  @param[in]  pteKind            New PTE kind
 *
 *  @returns nothing
 */
static inline void
memdescSetPteKind(PMEMORY_DESCRIPTOR pMemDesc, NvU32 pteKind)
{
    memdescSetPteKindForGpu(pMemDesc, pMemDesc->pGpu, pteKind);
}

/*!
 *  @brief Get HW resource identifier (HwResId)
 *
 *  TODO: Need to ensure this is checked per subdevice only.
 *
 *  @param[in]  pMemDesc           Memory descriptor pointer
 *
 *  @returns Current HW resource identifier
 */
static inline NvU32
memdescGetHwResId(PMEMORY_DESCRIPTOR pMemDesc)
{
    return pMemDesc->_hwResId;
}

/*!
 *  @brief Set HW resource identifier (HwResId)
 *
 *  @param[in]  pMemDesc           Memory descriptor pointer
 *  @param[in]  hwResId            New HW resource identifier
 *
 *  @returns nothing
 */
static inline void
memdescSetHwResId(PMEMORY_DESCRIPTOR pMemDesc, NvU32 hwResId)
{
    pMemDesc->_hwResId = hwResId;
}

/*!
 *  @brief Get mem destroy callback list pointer
 *
 *  @param[in]  pMemDesc    Memory descriptor pointer
 *
 *  @returns Pointer to mem destroy callback list
 */
static inline MEM_DESC_DESTROY_CALLBACK *
memdescGetDestroyCallbackList(MEMORY_DESCRIPTOR *pMemDesc)
{
    return pMemDesc->_pMemDestroyCallbackList;
}

/*!
 *  @brief Get the byte offset relative to the root memory descriptor.
 *
 *  Root memory descriptor is the Top level memory descriptor with no parent,
 *  from which this memory descriptor was derived.
 *
 *  @param[in]  pMemDesc  Return pointer to memory descriptor.
 *
 *  @returns the byte offset relative to Root memory descriptor.
 */
static inline NvU64
memdescGetRootOffset(PMEMORY_DESCRIPTOR pMemDesc)
{
    NvU64 rootOffset = 0;
    (void)memdescGetRootMemDesc(pMemDesc, &rootOffset);
    return rootOffset;
}

/*!
 *  @brief Get CPU cache attributes
 *
 *  @param[in]  pMemDesc    Memory descriptor pointer
 *
 *  @returns Current CPU cache attributes
 */
static inline NvU32
memdescGetCpuCacheAttrib(MEMORY_DESCRIPTOR *pMemDesc)
{
    return pMemDesc->_cpuCacheAttrib;
}

/*!
 *  @brief Get GPU cache attributes
 *
 *  @param[in]  pMemDesc    Memory descriptor pointer
 *
 *  @returns Current GPU cache attributes
 */
static inline NvU32
memdescGetGpuCacheAttrib(MEMORY_DESCRIPTOR *pMemDesc)
{
    return pMemDesc->_gpuCacheAttrib;
}

/*!
 *  @brief Return pte adjust
 *
 *  PteAdjust is zero whenever the memory is allocated as allocations are always
 *  going to be page-size aligned. However, we can have memory descriptors
 *  created on pre-allocated addresses + offset that aren't page aligned.
 *  PteAdjust is non-zero in such cases. We do not allow memdescDescribe operation
 *  (i.e. memory descriptors created on pre-allocated address) for subdevice
 *  memdesc and hence top level memdesc is always used to access pte adjust.
 *
 *  @param[in]  pMemDesc   Memory descriptor to use
 *
 *  @returns PteAdjust
 */
static inline NvU32
memdescGetPteAdjust(PMEMORY_DESCRIPTOR pMemDesc)
{
    return pMemDesc->PteAdjust;
}

/*!
 *  @brief Get subdevice allocation count.
 *
 *  @param[in]  pMemDesc    Memory descriptor pointer
 *
 *  @returns Current subdevice allocation count value.
 */
static inline NvU32
memdescGetSubDeviceAllocCount (MEMORY_DESCRIPTOR *pMemDesc)
{
    return pMemDesc->_subDeviceAllocCount;
}

/*!
 *  @brief Get memory descriptor of parent
 *
 *  @param[in]  pMemDesc    Memory descriptor pointer
 *
 *  @returns Memory descriptor of parent
 */
static inline MEMORY_DESCRIPTOR *
memdescGetParentDescriptor(MEMORY_DESCRIPTOR *pMemDesc)
{
    return pMemDesc->_pParentDescriptor;
}

/*!
 *  @brief Set the address space of the memory descriptor
 *
 *  @param[in]  pMemDesc           Memory descriptor used
 *  @param[in]  addressTranslation Address translation identifier
 *  @param[in]  addressSpace       Address Space
 *
 *  @returns nothing
 */
static inline void
memdescSetAddressSpace(PMEMORY_DESCRIPTOR pMemDesc, NV_ADDRESS_SPACE addressSpace)
{
    pMemDesc->_addressSpace = addressSpace;
}

/*!
 *  @brief Return size
 *
 *  @param[in]  pMemDesc   Memory descriptor to use
 *
 *  @returns Size
 */
static inline NvU64
memdescGetSize(PMEMORY_DESCRIPTOR pMemDesc)
{
    return pMemDesc->Size;
}

/*!
 *  @brief Checks if subdevice memory descriptors are present
 *
 *  See memdescGetMemDescFromSubDeviceInst for an explanation of subdevice memory
 *  descriptors
 *
 *  @param[in]  pMemDesc  Memory descriptor to query
 *
 *  @returns NV_TRUE if subdevice memory descriptors exist
 */
static NV_INLINE NvBool
memdescHasSubDeviceMemDescs(MEMORY_DESCRIPTOR *pMemDesc)
{
    return (pMemDesc->_subDeviceAllocCount > 1);
}

/*!
 *  @brief Checks if memory descriptor describes memory that is submemory
 *
 *  @param[in]  pMemDesc  Memory descriptor to query
 *
 *  @returns NV_TRUE if it is a submemory desc, NV_FALSE otherwise.
 */
static NV_INLINE NvBool
memdescIsSubMemoryMemDesc(MEMORY_DESCRIPTOR *pMemDesc)
{
    return pMemDesc->_pParentDescriptor != NULL ? NV_TRUE : NV_FALSE;
}

NV_STATUS memdescGetNvLinkGpa(OBJGPU *pGpu, NvU64 pageCount, RmPhysAddr *pGpa);

NV_STATUS memdescSetCtxBufPool(PMEMORY_DESCRIPTOR pMemDesc, CTX_BUF_POOL_INFO* pCtxBufPool);
CTX_BUF_POOL_INFO* memdescGetCtxBufPool(PMEMORY_DESCRIPTOR pMemDesc);

/*!
 * @brief Override the registry INST_LOC two-bit enum to an aperture (list) + cpu attr.
 *
 * loc parameters uses NV_REG_STR_RM_INST_LOC defines.
 * Caller must set initial default values.
 */
void memdescOverrideInstLoc(NvU32 loc, const char *name, NV_ADDRESS_SPACE *pAddrSpace, NvU32 *pCpuMappingAttr);
void memdescOverrideInstLocList(NvU32 loc, const char *name, const NV_ADDRESS_SPACE **ppAllocList, NvU32 *pCpuMappingAttr);

/*!
* @brief Override the physical system address limit.
*
*/
void memdescOverridePhysicalAddressWidthWindowsWAR(OBJGPU *pGpu, MEMORY_DESCRIPTOR *pMemDesc, NvU32 addressWidth);

/*!
* @brief Register memory descriptor referenced by hMemory in CPU-RM to GSP
*
* @param[in]  pGpu          OBJGPU pointer
* @param[in]  hClient       client handled
* @param[in]  hSubDevice    subdevice handle
* @param[in]  hMemory       memory handle
*
* @returns NV_STATUS
*/
NV_STATUS memdescRegisterToGSP(OBJGPU *pGpu, NvHandle hClient, NvHandle hParent, NvHandle hMemory);

/*!
* @brief Deregister memory descriptor referenced by hMemory in CPU-RM from GSP
*
* @param[in]  pGpu          OBJGPU pointer
* @param[in]  hClient       client handled
* @param[in]  hSubDevice    subdevice handle
* @param[in]  hMemory       memory handle
*
* @returns NV_STATUS
*/

NV_STATUS memdescDeregisterFromGSP(OBJGPU *pGpu, NvHandle hClient, NvHandle hParent, NvHandle hMemory);

// cache maintenance functions
void memdescFlushGpuCaches(OBJGPU *pGpu, MEMORY_DESCRIPTOR *pMemDesc);
void memdescFlushCpuCaches(OBJGPU *pGpu, MEMORY_DESCRIPTOR *pMemDesc);

// Map memory descriptor for RM internal access
void* memdescMapInternal(OBJGPU *pGpu, MEMORY_DESCRIPTOR *pMemDesc, NvU32 flags);
void memdescUnmapInternal(OBJGPU *pGpu, MEMORY_DESCRIPTOR *pMemDesc, NvU32 flags);

//
// External flags:
//   ALLOC_PER_SUBDEVICE    Allocate independent system memory for each GPU
//   LOST_ON_SUSPEND        PM code will skip this allocation during S/R
//   LOCKLESS_SYSMEM_ALLOC  System memory should be allocated unprotected by
//                          the  RM lock
//   GPU_PRIVILEGED         This memory will be marked as privileged in the GPU
//                          page tables.  When set only GPU requestors who are
//                          "privileged" are allowed to access this memory.
//                          This can be used for mapping sensitive memory into
//                          a user's GPU address space (like context buffers).
//                          Note support for this in our GPUs is limited, so
//                          only use it if you know the HW accessing the memory
//                          makes privileged requests.
//
// Internal flags:
//   SET_KIND               Whether or not the kind was set a different value
//                          than default.
//   PRE_ALLOCATED          Caller provided memory descriptor memory
//   FIXED_ADDRESS_ALLOCATE Allocate from the heap with a fixed address
//   ALLOCATED              Has the memory been allocated yet?
//   GUEST_ALLOCATED        Is the memory allocated by a guest VM?
//                          We make aliased memory descriptors to guest
//                          allocated memory and mark it so, so that we know
//                          how to deal with it in memdescMap() etc.
//   KERNEL_MODE            Is the memory for a user or kernel context?
//                          XXX This is lame, and it would be best if we could
//                          get rid of it.  Memory *storage* isn't either user
//                          or kernel -- only mappings are user or kernel.
//                          Unfortunately, osAllocPages requires that we
//                          provide this information.
//  PHYSICALLY_CONTIGUOUS   Are the underlying physical pages of this memory
//                          allocation contiguous?
//  ENCRYPTED               TurboCipher allocations need a bit in the PTE to
//                          indicate encrypted
//  UNICAST                 Memory descriptor was created via UC path
//  PAGED_SYSMEM            Allocate the memory from paged system memory. When
//                          this flag is used, memdescLock() should be called
//                          to lock the memory in physical pages before we
//                          access this memory descriptor.
//  CPU_ONLY                Allocate memory only accessed by CPU.
//
#define MEMDESC_FLAGS_NONE                         ((NvU64)0x0)
#define MEMDESC_FLAGS_ALLOC_PER_SUBDEVICE          NVBIT64(0)
#define MEMDESC_FLAGS_SET_KIND                     NVBIT64(1)
#define MEMDESC_FLAGS_LOST_ON_SUSPEND              NVBIT64(2)
#define MEMDESC_FLAGS_PRE_ALLOCATED                NVBIT64(3)
#define MEMDESC_FLAGS_FIXED_ADDRESS_ALLOCATE       NVBIT64(4)
#define MEMDESC_FLAGS_LOCKLESS_SYSMEM_ALLOC        NVBIT64(5)
#define MEMDESC_FLAGS_GPU_IN_RESET                 NVBIT64(6)
#define MEMDESC_ALLOC_FLAGS_PROTECTED              NVBIT64(7)
#define MEMDESC_FLAGS_GUEST_ALLOCATED              NVBIT64(8)
#define MEMDESC_FLAGS_KERNEL_MODE                  NVBIT64(9)
#define MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS        NVBIT64(10)
#define MEMDESC_FLAGS_ENCRYPTED                    NVBIT64(11)
#define MEMDESC_FLAGS_PAGED_SYSMEM                 NVBIT64(12)
#define MEMDESC_FLAGS_GPU_PRIVILEGED               NVBIT64(13)
#define MEMDESC_FLAGS_PRESERVE_CONTENT_ON_SUSPEND  NVBIT64(14)
#define MEMDESC_FLAGS_DUMMY_TOPLEVEL               NVBIT64(15)

// Don't use the below two flags. For memdesc internal use only.
// These flags will be removed on memory allocation refactoring in RM
#define MEMDESC_FLAGS_PROVIDE_IOMMU_MAP            NVBIT64(16)
#define MEMDESC_FLAGS_SKIP_RESOURCE_COMPUTE        NVBIT64(17)

#define MEMDESC_FLAGS_CUSTOM_HEAP_ACR              NVBIT64(18)

// Allocate in "fast" or "slow" memory, if there are multiple grades of memory (like mixed density)
#define MEMDESC_FLAGS_HIGH_PRIORITY                NVBIT64(19)
#define MEMDESC_FLAGS_LOW_PRIORITY                 NVBIT64(20)

// Flag to specify if requested size should be rounded to page size
#define MEMDESC_FLAGS_PAGE_SIZE_ALIGN_IGNORE       NVBIT64(21)

#define MEMDESC_FLAGS_CPU_ONLY                     NVBIT64(22)

// This flags is used for a special SYSMEM descriptor that points to a memory
// region allocated externally (e.g. malloc, kmalloc etc.)
#define MEMDESC_FLAGS_EXT_PAGE_ARRAY_MEM           NVBIT64(23)

// Owned by Physical Memory Allocator (PMA).
#define MEMDESC_FLAGS_ALLOC_PMA_OWNED              NVBIT64(24)

// This flag is added as part of Sub-Allocator feature meant to be used by VGPU clients.
// Once VGPU clients allocate a large block of memory for their use, they carve-out a small
// portion of it to be used for RM internal allocations originating from a given client. Each
// allocation can choose to use this carved-out memory owned by client or be part of global heap.
// This flag has to be used in RM internal allocation only when a particular allocation is tied to
// the life-time of this client and will be freed before client gets destroyed.
#define MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE      NVBIT64(25)

// This flag is used to specify the pages are pinned using other kernel module or API
// Currently, this flag is used for vGPU on KVM where RM calls vfio APIs to pin and unpin pages
// instead of using os_lock_user_pages() and os_unlock_user_pages().
#define MEMDESC_FLAGS_FOREIGN_PAGE                 NVBIT64(26)

// These flags are used for SYSMEM descriptors that point to a physical BAR
// range and do not take the usual memory mapping paths. Currently, these are used for vGPU.
#define MEMDESC_FLAGS_BAR0_REFLECT                 NVBIT64(27)
#define MEMDESC_FLAGS_BAR1_REFLECT                 NVBIT64(28)

// This flag is used to create shared memory required for vGPU operation.
// During RPC and all other shared memory allocations, VF RM will set this flag to instruct mods
// layer to create shared memory between VF process and PF process.
#define MEMDESC_FLAGS_MODS_SHARED_MEM              NVBIT64(29)

// This flag is set in memdescs that describe client (currently MODS) managed VPR allocations.
#define MEMDESC_FLAGS_VPR_REGION_CLIENT_MANAGED    NVBIT64(30)

// This flags is used for a special SYSMEM descriptor that points to physical BAR
// range of a third party device.
#define MEMDESC_FLAGS_PEER_IO_MEM                  NVBIT64(31)

// If the flag is set, the RM will only allow read-only CPU user-mappings
// to the descriptor.
#define MEMDESC_FLAGS_USER_READ_ONLY               NVBIT64(32)

// If the flag is set, the RM will only allow read-only DMA mappings
// to the descriptor.
#define MEMDESC_FLAGS_DEVICE_READ_ONLY             NVBIT64(33)

// This flag is used to denote the memory descriptor that is part of larger memory descriptor;
// created using NV01_MEMORY_LIST_SYSTEM, NV01_MEMORY_LIST_FBMEM or NV01_MEMORY_LIST_OBJECT.
#define MEMDESC_FLAGS_LIST_MEMORY                  NVBIT64(34)

// This flag is used to denote that this memdesc is allocated from
// a context buffer pool. When this flag is set, we expect a pointer
// to this context buffer pool to be cached in memdesc.
#define MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL        NVBIT64(36)

//
// This flag is used to skip privilege checks for the ADDR_REGMEM mapping type.
// This flag is useful for cases like UserModeApi where we want to use this memory type
// in a non-privileged user context
#define MEMDESC_FLAGS_SKIP_REGMEM_PRIV_CHECK       NVBIT64(37)

// This flag denotes the memory descriptor of type Display non iso
#define MEMDESC_FLAGS_MEMORY_TYPE_DISPLAY_NISO             NVBIT64(38)

// This flag is used to force mapping of coherent sysmem through
// the GMMU over BAR1. This is useful when we need some form
// of special translation of the SYSMEM_COH aperture by the GMMU.
#define MEMDESC_FLAGS_MAP_SYSCOH_OVER_BAR1         NVBIT64(39)

// This flag is used to override system memory limit to be allocated
// within override address width.
#define MEMDESC_FLAGS_OVERRIDE_SYSTEM_ADDRESS_LIMIT   NVBIT64(40)

//
// If this flag is set, Linux RM will ensure that the allocated memory is
// 32-bit addressable.
#define MEMDESC_FLAGS_ALLOC_32BIT_ADDRESSABLE      NVBIT64(41)

//
// If this flag is set, the memory is registered in GSP
//
#define MEMDESC_FLAGS_REGISTERED_TO_GSP      NVBIT64(42)

//
// Indicates that this memdesc is tracking client sysmem allocation as
// against RM internal sysmem allocation
//
#define MEMDESC_FLAGS_SYSMEM_OWNED_BY_CLIENT        NVBIT64(44)

//
// The following is a special use case for sharing memory between
// the GPU and a WSL client. There is no IOMMU-compliant support
// currently for this, so a WAR is required for r515. The intent
// is to remove this by r525.
//
#define MEMDESC_FLAGS_WSL_SHARED_MEMORY             NVBIT64(46)

#endif // _MEMDESC_H_

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_MEM_DESC_NVOC_H_
