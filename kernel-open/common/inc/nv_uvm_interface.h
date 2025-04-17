/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//
// This file provides the interface that RM exposes to UVM.
//

#ifndef _NV_UVM_INTERFACE_H_
#define _NV_UVM_INTERFACE_H_

// Forward references, to break circular header file dependencies:
struct UvmOpsUvmEvents;

#if defined(NVIDIA_UVM_ENABLED)

// We are in the UVM build system, for a Linux target.
#include "uvm_linux.h"

#else

// We are in the RM build system, for a Linux target:
#include "nv-linux.h"

#endif // NVIDIA_UVM_ENABLED

#include "nvgputypes.h"
#include "nvstatus.h"
#include "nv_uvm_types.h"


// Define the type here as it's Linux specific, used only by the Linux specific
// nvUvmInterfaceRegisterGpu() API.
typedef struct
{
    struct pci_dev *pci_dev;

    // DMA addressable range of the device, mirrors fields in nv_state_t.
    NvU64 dma_addressable_start;
    NvU64 dma_addressable_limit;
} UvmGpuPlatformInfo;

/*******************************************************************************
    nvUvmInterfaceRegisterGpu

    Registers the GPU with the provided physical UUID for use. A GPU must be
    registered before its UUID can be used with any other API. This call is
    ref-counted so every nvUvmInterfaceRegisterGpu must be paired with a
    corresponding nvUvmInterfaceUnregisterGpu.

    You don't need to call nvUvmInterfaceSessionCreate before calling this.

    Error codes:
        NV_ERR_GPU_UUID_NOT_FOUND
        NV_ERR_NO_MEMORY
        NV_ERR_GENERIC
*/
NV_STATUS nvUvmInterfaceRegisterGpu(const NvProcessorUuid *gpuUuid, UvmGpuPlatformInfo *gpuInfo);

/*******************************************************************************
    nvUvmInterfaceUnregisterGpu

    Unregisters the GPU with the provided physical UUID. This drops the ref
    count from nvUvmInterfaceRegisterGpu. Once the reference count goes to 0
    the device may no longer be accessible until the next
    nvUvmInterfaceRegisterGpu call. No automatic resource freeing is performed,
    so only make the last unregister call after destroying all your allocations
    associated with that UUID (such as those from
    nvUvmInterfaceAddressSpaceCreate).

    If the UUID is not found, no operation is performed.
*/
void nvUvmInterfaceUnregisterGpu(const NvProcessorUuid *gpuUuid);

/*******************************************************************************
    nvUvmInterfaceSessionCreate

    TODO: Creates session object.  All allocations are tied to the session.

    The platformInfo parameter is filled by the callee with miscellaneous system
    information. Refer to the UvmPlatformInfo struct for details.

    Error codes:
      NV_ERR_GENERIC
      NV_ERR_NO_MEMORY
*/
NV_STATUS nvUvmInterfaceSessionCreate(uvmGpuSessionHandle *session,
                                      UvmPlatformInfo *platformInfo);

/*******************************************************************************
    nvUvmInterfaceSessionDestroy

    Destroys a session object.  All allocations are tied to the session will
    be destroyed.

    Error codes:
      NV_ERR_GENERIC
      NV_ERR_NO_MEMORY
*/
NV_STATUS nvUvmInterfaceSessionDestroy(uvmGpuSessionHandle session);

/*******************************************************************************
    nvUvmInterfaceDeviceCreate

    Creates a device object under the given session for the GPU with the given
    physical UUID. Also creates a partition object for the device iff
    bCreateSmcPartition is true and pGpuInfo->smcEnabled is true.
    pGpuInfo->smcUserClientInfo will be used to determine the SMC partition in
    this case. A device handle is returned in the device output parameter.

    Error codes:
      NV_ERR_GENERIC
      NV_ERR_NO_MEMORY
      NV_ERR_INVALID_ARGUMENT
      NV_ERR_INSUFFICIENT_RESOURCES
      NV_ERR_OBJECT_NOT_FOUND
*/
NV_STATUS nvUvmInterfaceDeviceCreate(uvmGpuSessionHandle session,
                                     const UvmGpuInfo *pGpuInfo,
                                     const NvProcessorUuid *gpuUuid,
                                     uvmGpuDeviceHandle *device,
                                     NvBool bCreateSmcPartition);

/*******************************************************************************
    nvUvmInterfaceDeviceDestroy

    Destroys the device object for the given handle. The handle must have been
    obtained in a prior call to nvUvmInterfaceDeviceCreate.
*/
void nvUvmInterfaceDeviceDestroy(uvmGpuDeviceHandle device);

/*******************************************************************************
    nvUvmInterfaceAddressSpaceCreate

    This function creates an address space.
    This virtual address space is created on the GPU specified
    by device.

    Error codes:
      NV_ERR_GENERIC
      NV_ERR_NO_MEMORY
*/
NV_STATUS nvUvmInterfaceAddressSpaceCreate(uvmGpuDeviceHandle device,
                                           unsigned long long vaBase,
                                           unsigned long long vaSize,
                                           NvBool enableAts,
                                           uvmGpuAddressSpaceHandle *vaSpace,
                                           UvmGpuAddressSpaceInfo *vaSpaceInfo);

/*******************************************************************************
    nvUvmInterfaceDupAddressSpace

    This function will dup the given vaspace from the users client to the
    kernel client was created as an ops session.

    By duping the vaspace it is guaranteed that RM will refcount the vaspace object.

    Error codes:
      NV_ERR_GENERIC
*/
NV_STATUS nvUvmInterfaceDupAddressSpace(uvmGpuDeviceHandle device,
                                        NvHandle hUserClient,
                                        NvHandle hUserVASpace,
                                        uvmGpuAddressSpaceHandle *vaSpace,
                                        UvmGpuAddressSpaceInfo *vaSpaceInfo);

/*******************************************************************************
    nvUvmInterfaceAddressSpaceDestroy

    Destroys an address space that was previously created via
    nvUvmInterfaceAddressSpaceCreate.
*/

void nvUvmInterfaceAddressSpaceDestroy(uvmGpuAddressSpaceHandle vaSpace);

/*******************************************************************************
    nvUvmInterfaceMemoryAllocFB

    This function will allocate video memory and provide a mapped Gpu
    virtual address to this allocation. It also returns the Gpu physical
    offset if contiguous allocations are requested.

    This function will allocate a minimum page size if the length provided is 0
    and will return a unique GPU virtual address.

    The default page size will be the small page size (as returned by query
    caps). The physical alignment will also be enforced to small page
    size(64K/128K).

    Arguments:
        vaSpace[IN]          - Pointer to vaSpace object
        length [IN]          - Length of the allocation
        gpuPointer[OUT]      - GPU VA mapping
        allocInfo[IN/OUT]    - Pointer to allocation info structure which
                               contains below given fields

        allocInfo Members:
        gpuPhysOffset[OUT]         - Physical offset of allocation returned only
                                     if contiguous allocation is requested.
        pageSize[IN]               - Override the default page size (see above).
        alignment[IN]              - gpuPointer GPU VA alignment. 0 means 4KB
                                     alignment.
        bContiguousPhysAlloc[IN]   - Flag to request contiguous allocation. Default
                                     will follow the vidHeapControl default policy.
        bMemGrowsDown[IN]
        bPersistentVidmem[IN]      - Allocate persistent vidmem.
        hPhysHandle[IN/OUT]        - The handle will be used in allocation if provided.
                                     If not provided; allocator will return the handle
                                     it used eventually.
    Error codes:
        NV_ERR_INVALID_ARGUMENT
        NV_ERR_NO_MEMORY              - Not enough physical memory to service
                                        allocation request with provided constraints
        NV_ERR_INSUFFICIENT_RESOURCES - Not enough available resources to satisfy allocation request
        NV_ERR_INVALID_OWNER          - Target memory not accessible by specified owner
        NV_ERR_NOT_SUPPORTED          - Operation not supported on broken FB

*/
NV_STATUS nvUvmInterfaceMemoryAllocFB(uvmGpuAddressSpaceHandle vaSpace,
                                      NvLength length,
                                      UvmGpuPointer * gpuPointer,
                                      UvmGpuAllocInfo * allocInfo);

/*******************************************************************************
    nvUvmInterfaceMemoryAllocSys

    This function will allocate system memory and provide a mapped Gpu
    virtual address to this allocation.

    This function will allocate a minimum page size if the length provided is 0
    and will return a unique GPU virtual address.

    The default page size will be the small page size (as returned by query caps)

    Arguments:
        vaSpace[IN]          - Pointer to vaSpace object
        length [IN]          - Length of the allocation
        gpuPointer[OUT]      - GPU VA mapping
        allocInfo[IN/OUT]    - Pointer to allocation info structure which
                               contains below given fields

        allocInfo Members:
        gpuPhysOffset[OUT]         - Physical offset of allocation returned only
                                     if contiguous allocation is requested.
        pageSize[IN]               - Override the default page size (see above).
        alignment[IN]              - gpuPointer GPU VA alignment. 0 means 4KB
                                     alignment.
        bContiguousPhysAlloc[IN]   - Flag to request contiguous allocation. Default
                                     will follow the vidHeapControl default policy.
        bMemGrowsDown[IN]
        bPersistentVidmem[IN]      - Allocate persistent vidmem.
        hPhysHandle[IN/OUT]        - The handle will be used in allocation if provided.
                                     If not provided; allocator will return the handle
                                     it used eventually.
    Error codes:
        NV_ERR_INVALID_ARGUMENT
        NV_ERR_NO_MEMORY              - Not enough physical memory to service
                                        allocation request with provided constraints
        NV_ERR_INSUFFICIENT_RESOURCES - Not enough available resources to satisfy allocation request
        NV_ERR_INVALID_OWNER          - Target memory not accessible by specified owner
        NV_ERR_NOT_SUPPORTED          - Operation not supported
*/
NV_STATUS nvUvmInterfaceMemoryAllocSys(uvmGpuAddressSpaceHandle vaSpace,
                                       NvLength length,
                                       UvmGpuPointer * gpuPointer,
                                       UvmGpuAllocInfo * allocInfo);

/*******************************************************************************
    nvUvmInterfaceGetP2PCaps

    Obtain the P2P capabilities between two devices.

    Arguments:
        device1[IN]         - Device handle of the first GPU (required)
        device2[IN]         - Device handle of the second GPU (required)
        p2pCapsParams [OUT] - P2P capabilities between the two GPUs

    Error codes:
        NV_ERR_INVALID_ARGUMENT
        NV_ERR_GENERIC:
          Unexpected error. We try hard to avoid returning this error
          code,because it is not very informative.

*/
NV_STATUS nvUvmInterfaceGetP2PCaps(uvmGpuDeviceHandle device1,
                                   uvmGpuDeviceHandle device2,
                                   UvmGpuP2PCapsParams * p2pCapsParams);

/*******************************************************************************
    nvUvmInterfaceGetPmaObject

    This function will return pointer to PMA object for the given GPU. This
    PMA object handle is required for page allocation.

    Arguments:
        device [IN]         - Device handle allocated in
                              nvUvmInterfaceDeviceCreate
        pPma [OUT]          - Pointer to PMA object
        pPmaPubStats [OUT]  - Pointer to UvmPmaStatistics object

    Error codes:
        NV_ERR_NOT_SUPPORTED          - Operation not supported on broken FB
        NV_ERR_GENERIC:
          Unexpected error. We try hard to avoid returning this error
          code,because it is not very informative.
*/
NV_STATUS nvUvmInterfaceGetPmaObject(uvmGpuDeviceHandle device,
                                     void **pPma,
                                     const UvmPmaStatistics **pPmaPubStats);

// Mirrors pmaEvictPagesCb_t, see its documentation in pma.h.
typedef NV_STATUS (*uvmPmaEvictPagesCallback)(void *callbackData,
                                              NvU64 pageSize,
                                              NvU64 *pPages,
                                              NvU32 count,
                                              NvU64 physBegin,
                                              NvU64 physEnd,
                                              UVM_PMA_GPU_MEMORY_TYPE mem_type);

// Mirrors pmaEvictRangeCb_t, see its documentation in pma.h.
typedef NV_STATUS (*uvmPmaEvictRangeCallback)(void *callbackData,
                                              NvU64 physBegin,
                                              NvU64 physEnd,
                                              UVM_PMA_GPU_MEMORY_TYPE mem_type);

/*******************************************************************************
    nvUvmInterfacePmaRegisterEvictionCallbacks

    Simple wrapper for pmaRegisterEvictionCb(), see its documentation in pma.h.
*/
NV_STATUS nvUvmInterfacePmaRegisterEvictionCallbacks(void *pPma,
                                                     uvmPmaEvictPagesCallback evictPages,
                                                     uvmPmaEvictRangeCallback evictRange,
                                                     void *callbackData);

/******************************************************************************
    nvUvmInterfacePmaUnregisterEvictionCallbacks

    Simple wrapper for pmaUnregisterEvictionCb(), see its documentation in pma.h.
*/
void nvUvmInterfacePmaUnregisterEvictionCallbacks(void *pPma);

/*******************************************************************************
    nvUvmInterfacePmaAllocPages

    @brief Synchronous API for allocating pages from the PMA.
    PMA will decide which pma regions to allocate from based on the provided
    flags.  PMA will also initiate UVM evictions to make room for this
    allocation unless prohibited by PMA_FLAGS_DONT_EVICT.  UVM callers must pass
    this flag to avoid deadlock.  Only UVM may allocated unpinned memory from
    this API.

    For broadcast methods, PMA will guarantee the same physical frames are
    allocated on multiple GPUs, specified by the PMA objects passed in.

    If allocation is contiguous, only one page in pPages will be filled.
    Also, contiguous flag must be passed later to nvUvmInterfacePmaFreePages.

    Arguments:
        pPma[IN]             - Pointer to PMA object
        pageCount [IN]       - Number of pages required to be allocated.
        pageSize [IN]        - 64kb, 128kb or 2mb.  No other values are permissible.
        pPmaAllocOptions[IN] - Pointer to PMA allocation info structure.
        pPages[OUT]          - Array of pointers, containing the PA base
                               address of each page.

    Error codes:
        NV_ERR_NO_MEMORY:
          Internal memory allocation failed.
        NV_ERR_GENERIC:
          Unexpected error. We try hard to avoid returning this error
          code,because it is not very informative.
*/
NV_STATUS nvUvmInterfacePmaAllocPages(void *pPma,
                                      NvLength pageCount,
                                      NvU64 pageSize,
                                      UvmPmaAllocationOptions *pPmaAllocOptions,
                                      NvU64 *pPages);

/*******************************************************************************
    nvUvmInterfacePmaPinPages

    This function will pin the physical memory allocated using PMA. The pages
    passed as input must be unpinned else this function will return an error and
    rollback any change if any page is not previously marked "unpinned".

    Arguments:
        pPma[IN]             - Pointer to PMA object.
        pPages[IN]           - Array of pointers, containing the PA base
                               address of each page to be pinned.
        pageCount [IN]       - Number of pages required to be pinned.
        pageSize [IN]        - Page size of each page to be pinned.
        flags [IN]           - UVM_PMA_CALLED_FROM_PMA_EVICTION if called from
                               PMA eviction, 0 otherwise.
    Error codes:
        NV_ERR_INVALID_ARGUMENT       - Invalid input arguments.
        NV_ERR_GENERIC                - Unexpected error. We try hard to avoid
                                        returning this error code as is not very
                                        informative.
        NV_ERR_NOT_SUPPORTED          - Operation not supported on broken FB
*/
NV_STATUS nvUvmInterfacePmaPinPages(void *pPma,
                                    NvU64 *pPages,
                                    NvLength pageCount,
                                    NvU64 pageSize,
                                    NvU32 flags);

/*******************************************************************************
    nvUvmInterfaceMemoryFree

    Free up a GPU allocation
*/
void nvUvmInterfaceMemoryFree(uvmGpuAddressSpaceHandle vaSpace,
                              UvmGpuPointer gpuPointer);

/*******************************************************************************
    nvUvmInterfacePmaFreePages

    This function will free physical memory allocated using PMA.  It marks a list
    of pages as free. This operation is also used by RM to mark pages as "scrubbed"
    for the initial ECC sweep. This function does not fail.

    When allocation was contiguous, an appropriate flag needs to be passed.

    Arguments:
        pPma[IN]             - Pointer to PMA object
        pPages[IN]           - Array of pointers, containing the PA base
                               address of each page.
        pageCount [IN]       - Number of pages required to be allocated.
        pageSize [IN]        - Page size of each page
        flags [IN]           - Flags with information about allocation type
                               with the same meaning as flags in options for
                               nvUvmInterfacePmaAllocPages. When called from PMA
                               eviction, UVM_PMA_CALLED_FROM_PMA_EVICTION needs
                               to be added to flags.
    Error codes:
        NV_ERR_INVALID_ARGUMENT
        NV_ERR_NO_MEMORY              - Not enough physical memory to service
                                        allocation request with provided constraints
        NV_ERR_INSUFFICIENT_RESOURCES - Not enough available resources to satisfy allocation request
        NV_ERR_INVALID_OWNER          - Target memory not accessible by specified owner
        NV_ERR_NOT_SUPPORTED          - Operation not supported on broken FB
*/
void nvUvmInterfacePmaFreePages(void *pPma,
                                NvU64 *pPages,
                                NvLength pageCount,
                                NvU64 pageSize,
                                NvU32 flags);

/*******************************************************************************
    nvUvmInterfaceMemoryCpuMap

    This function creates a CPU mapping to the provided GPU address.
    If the address is not the same as what is returned by the Alloc
    function, then the function will map it from the address provided.
    This offset will be relative to the gpu offset obtained from the
    memory alloc functions.

    Error codes:
      NV_ERR_GENERIC
      NV_ERR_NO_MEMORY
*/
NV_STATUS nvUvmInterfaceMemoryCpuMap(uvmGpuAddressSpaceHandle vaSpace,
                                     UvmGpuPointer gpuPointer,
                                     NvLength length, void **cpuPtr,
                                     NvU64 pageSize);

/*******************************************************************************
    uvmGpuMemoryCpuUnmap

    Unmaps the cpuPtr provided from the process virtual address space.
*/
void nvUvmInterfaceMemoryCpuUnMap(uvmGpuAddressSpaceHandle vaSpace,
                                  void *cpuPtr);

/*******************************************************************************
    nvUvmInterfaceTsgAllocate

    This function allocates a Time-Slice Group (TSG).

    allocParams must contain an engineIndex as TSGs need to be bound to an
    engine type at allocation time. The possible values are [0,
    UVM_COPY_ENGINE_COUNT_MAX) for CE engine type. Notably only the copy engines
    that have UvmGpuCopyEngineCaps::supported set to true can be allocated.

    Note that TSG is not supported on all GPU architectures for all engine
    types, e.g., pre-Volta GPUs only support TSG for the GR/Compute engine type.
    On devices that do not support HW TSGs on the requested engine, this API is
    still required, i.e., a TSG handle is required in
    nvUvmInterfaceChannelAllocate(), due to information stored in it necessary
    for channel allocation. However, when HW TSGs aren't supported, a TSG handle
    is essentially a "fake" TSG with no HW scheduling impact.

    tsg is filled with the address of the corresponding TSG handle.

    Arguments:
        vaSpace[IN]      - VA space linked to a client and a device under which
                           the TSG is allocated.
        allocParams[IN]  - structure with allocation settings.
        tsg[OUT]         - pointer to the new TSG handle.

    Error codes:
      NV_ERR_GENERIC
      NV_ERR_INVALID_ARGUMENT
      NV_ERR_NO_MEMORY
      NV_ERR_NOT_SUPPORTED
*/
NV_STATUS nvUvmInterfaceTsgAllocate(uvmGpuAddressSpaceHandle vaSpace,
                                    const UvmGpuTsgAllocParams *allocParams,
                                    uvmGpuTsgHandle *tsg);

/*******************************************************************************
    nvUvmInterfaceTsgDestroy

    This function destroys a given TSG.

    Arguments:
        tsg[IN]         - Tsg handle
*/
void nvUvmInterfaceTsgDestroy(uvmGpuTsgHandle tsg);

/*******************************************************************************
    nvUvmInterfaceChannelAllocate

    This function will allocate a channel bound to a copy engine(CE) or a SEC2
    engine.

    allocParams contains information relative to GPFIFO and GPPut.

    channel is filled with the address of the corresponding channel handle.

    channelInfo is filled out with channel get/put. The errorNotifier is filled
    out when the channel hits an RC error. On Volta+ devices, it also computes
    the work submission token and the work submission offset to be used in the
    Host channel submission doorbell.

    Arguments:
        tsg[IN]          - Time-Slice Group that the channel will be a member.
        allocParams[IN]  - structure with allocation settings
        channel[OUT]     - pointer to the new channel handle
        channelInfo[OUT] - structure filled with channel information

    Error codes:
      NV_ERR_GENERIC
      NV_ERR_INVALID_ARGUMENT
      NV_ERR_NO_MEMORY
      NV_ERR_NOT_SUPPORTED
*/
NV_STATUS nvUvmInterfaceChannelAllocate(const uvmGpuTsgHandle tsg,
                                        const UvmGpuChannelAllocParams *allocParams,
                                        uvmGpuChannelHandle *channel,
                                        UvmGpuChannelInfo *channelInfo);

/*******************************************************************************
    nvUvmInterfaceChannelDestroy

    This function destroys a given channel.

    Arguments:
        channel[IN]     - channel handle
*/
void nvUvmInterfaceChannelDestroy(uvmGpuChannelHandle channel);

/*******************************************************************************
    nvUvmInterfaceQueryCaps

    Return capabilities for the provided GPU.
    If GPU does not exist, an error will be returned.

    If the client is only interested in the capabilities of the Copy Engines of
    the given GPU, use nvUvmInterfaceQueryCopyEnginesCaps instead.

    Error codes:
      NV_ERR_GENERIC
      NV_ERR_NO_MEMORY
      NV_ERR_INVALID_STATE
      NV_ERR_NOT_SUPPORTED
      NV_ERR_NOT_READY
      NV_ERR_INVALID_LOCK_STATE
      NV_ERR_INVALID_STATE
      NV_ERR_NVLINK_FABRIC_NOT_READY
      NV_ERR_NVLINK_FABRIC_FAILURE
      NV_ERR_GPU_MEMORY_ONLINING_FAILURE
*/
NV_STATUS nvUvmInterfaceQueryCaps(uvmGpuDeviceHandle device,
                                  UvmGpuCaps *caps);

/*******************************************************************************
    nvUvmInterfaceQueryCopyEnginesCaps

    Return the capabilities of all the Copy Engines for the provided GPU.
    If the GPU does not exist, an error will be returned.

    Error codes:
      NV_ERR_GENERIC
      NV_ERR_NO_MEMORY
*/
NV_STATUS nvUvmInterfaceQueryCopyEnginesCaps(uvmGpuDeviceHandle device,
                                             UvmGpuCopyEnginesCaps *caps);

/*******************************************************************************
    nvUvmInterfaceGetGpuInfo

    Return various gpu info, refer to the UvmGpuInfo struct for details.
    The input UUID is for the physical GPU and the pGpuClientInfo identifies
    the SMC partition if SMC is enabled and the partition exists.
    If no gpu matching the uuid is found, an error will be returned.

    On Ampere+ GPUs, pGpuClientInfo contains SMC information provided by the
    client regarding the partition targeted in this operation.

    Error codes:
      NV_ERR_GENERIC
      NV_ERR_NO_MEMORY
      NV_ERR_GPU_UUID_NOT_FOUND
      NV_ERR_INSUFFICIENT_PERMISSIONS
      NV_ERR_INSUFFICIENT_RESOURCES
*/
NV_STATUS nvUvmInterfaceGetGpuInfo(const NvProcessorUuid *gpuUuid,
                                   const UvmGpuClientInfo *pGpuClientInfo,
                                   UvmGpuInfo *pGpuInfo);

/*******************************************************************************
    nvUvmInterfaceServiceDeviceInterruptsRM

    Tells RM to service all pending interrupts. This is helpful in ECC error
    conditions when ECC error interrupt is set & error can be determined only
    after ECC notifier will be set or reset.

    Error codes:
      NV_ERR_GENERIC
      UVM_INVALID_ARGUMENTS
*/
NV_STATUS nvUvmInterfaceServiceDeviceInterruptsRM(uvmGpuDeviceHandle device);

/*******************************************************************************
    nvUvmInterfaceSetPageDirectory
    Sets pageDirectory in the provided location. Also moves the existing PDE to
    the provided pageDirectory.

    RM will propagate the update to all channels using the provided VA space.
    All channels must be idle when this call is made.

    If the pageDirectory is in system memory then a CPU physical address must be
    provided. RM will establish and manage the DMA mapping for the
    pageDirectory.

    Arguments:
      vaSpace[IN}         - VASpace Object
      physAddress[IN]     - Physical address of new page directory. If
                            !bVidMemAperture this is a CPU physical address.
      numEntries[IN]      - Number of entries including previous PDE which will be copied
      bVidMemAperture[IN] - If set pageDirectory will reside in VidMem aperture else sysmem
      pasid[IN]           - PASID (Process Address Space IDentifier) of the process
                            corresponding to the VA space. Ignored unless the VA space
                            object has ATS enabled.
      dmaAddress[OUT]     - DMA mapping created for physAddress.

    Error codes:
      NV_ERR_GENERIC
      NV_ERR_INVALID_ARGUMENT
*/
NV_STATUS nvUvmInterfaceSetPageDirectory(uvmGpuAddressSpaceHandle vaSpace,
                                         NvU64 physAddress, unsigned numEntries,
                                         NvBool bVidMemAperture, NvU32 pasid,
                                         NvU64 *dmaAddress);

/*******************************************************************************
    nvUvmInterfaceUnsetPageDirectory
    Unsets/Restores pageDirectory to RM's defined location.

    Arguments:
      vaSpace[IN}         - VASpace Object

    Error codes:
      NV_ERR_GENERIC
      NV_ERR_INVALID_ARGUMENT
*/
NV_STATUS nvUvmInterfaceUnsetPageDirectory(uvmGpuAddressSpaceHandle vaSpace);

/*******************************************************************************
    nvUvmInterfaceDupAllocation

    Duplicate the given allocation in a different VA space.

    The physical handle backing the source allocation is duplicated in
    the GPU device associated with the destination VA space, and a new mapping
    is created in that VA space.

    The input allocation can be located in sysmem (i.e. allocated using
    nvUvmInterfaceMemoryAllocSys) or vidmem (i.e. allocated using
    nvUvmInterfaceMemoryAllocFB). If located in vidmem, duplication across
    GPUs is not supported.

    For duplication of physical memory use nvUvmInterfaceDupMemory.

    Arguments:
        srcVaSpace[IN]     - Source VA space.
        srcAddress[IN]     - GPU VA in the source VA space. The provided address
                             should match one previously returned by
                             nvUvmInterfaceMemoryAllocFB or
                             nvUvmInterfaceMemoryAllocSys.
        dstVaSpace[IN]     - Destination VA space where the new mapping will be
                             created.
        dstVaAlignment[IN] - Alignment of the GPU VA in the destination VA
                             space. 0 means 4KB alignment.
        dstAddress[OUT]    - Pointer to the GPU VA in the destination VA space.

    Error codes:
      NV_ERR_INVALID_ARGUMENT - If any of the inputs is invalid, or the source
                                and destination VA spaces are identical.
      NV_ERR_OBJECT_NOT_FOUND - If the input allocation is not found in under
                                the provided VA space.
      NV_ERR_NO_MEMORY        - If there is no memory to back the duplicate,
                                or the associated metadata.
      NV_ERR_NOT_SUPPORTED    - If trying to duplicate vidmem across GPUs.
*/
NV_STATUS nvUvmInterfaceDupAllocation(uvmGpuAddressSpaceHandle srcVaSpace,
                                      NvU64 srcAddress,
                                      uvmGpuAddressSpaceHandle dstVaSpace,
                                      NvU64 dstVaAlignment,
                                      NvU64 *dstAddress);

/*******************************************************************************
    nvUvmInterfaceDupMemory

    Duplicates a physical memory allocation. If requested, provides information
    about the allocation.

    Arguments:
        device[IN]                      - Device linked to a client under which
                                          the phys memory needs to be duped.
        hClient[IN]                     - Client owning the memory.
        hPhysMemory[IN]                 - Phys memory which is to be duped.
        hDupedHandle[OUT]               - Handle of the duped memory object.
        pGpuMemoryInfo[OUT]             - see nv_uvm_types.h for more information.
                                          This parameter can be NULL. (optional)
    Error codes:
      NV_ERR_INVALID_ARGUMENT   - If the parameter/s is invalid.
      NV_ERR_NOT_SUPPORTED      - If the allocation is not a physical allocation.
      NV_ERR_OBJECT_NOT_FOUND   - If the allocation is not found in under the provided client.
*/
NV_STATUS nvUvmInterfaceDupMemory(uvmGpuDeviceHandle device,
                                  NvHandle hClient,
                                  NvHandle hPhysMemory,
                                  NvHandle *hDupMemory,
                                  UvmGpuMemoryInfo *pGpuMemoryInfo);

/*******************************************************************************
    nvUvmInterfaceFreeDupedAllocation

    Free the allocation represented by the physical handle used to create the
    duped allocation.

    Arguments:
        device[IN]               - Device handle used to dup the memory.
        hPhysHandle[IN]          - Handle representing the phys allocation.

    Error codes:
      NV_ERROR
      NV_ERR_INVALID_ARGUMENT
*/
NV_STATUS nvUvmInterfaceFreeDupedHandle(uvmGpuDeviceHandle device,
                                        NvHandle hPhysHandle);

/*******************************************************************************
    nvUvmInterfaceGetFbInfo

    Gets FB information from RM.

    Arguments:
        device[IN]        - GPU device handle
        fbInfo [OUT]      - Pointer to FbInfo structure which contains
                            reservedHeapSize & heapSize
    Error codes:
      NV_ERROR
      NV_ERR_INVALID_ARGUMENT
*/
NV_STATUS nvUvmInterfaceGetFbInfo(uvmGpuDeviceHandle device,
                                  UvmGpuFbInfo * fbInfo);

/*******************************************************************************
    nvUvmInterfaceGetEccInfo

    Gets ECC information from RM.

    Arguments:
        device[IN]        - GPU device handle
        eccInfo [OUT]     - Pointer to EccInfo structure

    Error codes:
      NV_ERROR
      NV_ERR_INVALID_ARGUMENT
*/
NV_STATUS nvUvmInterfaceGetEccInfo(uvmGpuDeviceHandle device,
                                   UvmGpuEccInfo * eccInfo);

/*******************************************************************************
    nvUvmInterfaceOwnPageFaultIntr

    This function transfers ownership of the replayable page fault interrupt,
    between RM and UVM, for a particular GPU.

    bOwnInterrupts == NV_TRUE: UVM is taking ownership from the RM. This causes
    the following: RM will not service, enable or disable this interrupt and it
    is up to the UVM driver to handle this interrupt. In this case, replayable
    page fault interrupts are disabled by this function, before it returns.

    bOwnInterrupts == NV_FALSE: UVM is returning ownership to the RM: in this
    case, replayable page fault interrupts MUST BE DISABLED BEFORE CALLING this
    function.

    The cases above both result in transferring ownership of a GPU that has its
    replayable page fault interrupts disabled. Doing otherwise would make it
    very difficult to control which driver handles any interrupts that build up
    during the hand-off.

    The calling pattern should look like this:

    UVM setting up a new GPU for operation:
        UVM GPU LOCK
           nvUvmInterfaceOwnPageFaultIntr(..., NV_TRUE)
        UVM GPU UNLOCK

        Enable replayable page faults for that GPU

    UVM tearing down a GPU:

        Disable replayable page faults for that GPU

        UVM GPU GPU LOCK
           nvUvmInterfaceOwnPageFaultIntr(..., NV_FALSE)
        UVM GPU UNLOCK

    Arguments:
        device[IN]           - Device handle associated with the gpu
        bOwnInterrupts       - Set to NV_TRUE for UVM to take ownership of the
                               replayable page fault interrupts. Set to NV_FALSE
                               to return ownership of the page fault interrupts
                               to RM.
    Error codes:
      NV_ERR_GENERIC
      NV_ERR_INVALID_ARGUMENT
*/
NV_STATUS nvUvmInterfaceOwnPageFaultIntr(uvmGpuDeviceHandle device, NvBool bOwnInterrupts);

/*******************************************************************************
    nvUvmInterfaceInitFaultInfo

    This function obtains fault buffer address, size and a few register mappings
    for replayable faults, and creates a shadow buffer to store non-replayable
    faults if the GPU supports it.

    Arguments:
        device[IN]        - Device handle associated with the gpu
        pFaultInfo[OUT]   - information provided by RM for fault handling

    Error codes:
      NV_ERR_GENERIC
      NV_ERR_NO_MEMORY
      NV_ERR_INVALID_ARGUMENT
*/
NV_STATUS nvUvmInterfaceInitFaultInfo(uvmGpuDeviceHandle device,
                                      UvmGpuFaultInfo *pFaultInfo);

/*******************************************************************************
    nvUvmInterfaceDestroyFaultInfo

    This function obtains destroys unmaps the fault buffer and clears faultInfo
    for replayable faults, and frees the shadow buffer for non-replayable faults.

    Arguments:
        device[IN]        - Device handle associated with the gpu
        pFaultInfo[OUT]   - information provided by RM for fault handling

    Error codes:
      NV_ERR_GENERIC
      NV_ERR_INVALID_ARGUMENT
*/
NV_STATUS nvUvmInterfaceDestroyFaultInfo(uvmGpuDeviceHandle device,
                                         UvmGpuFaultInfo *pFaultInfo);

/*******************************************************************************
    nvUvmInterfaceHasPendingNonReplayableFaults

    This function tells whether there are pending non-replayable faults in the
    client shadow fault buffer ready to be consumed.

    NOTES:
    - This function uses a pre-allocated stack per GPU (stored in the
    UvmGpuFaultInfo object) for calls related to non-replayable faults from the
    top half.
    - Concurrent calls to this function using the same pFaultInfo are not
    thread-safe due to pre-allocated stack. Therefore, locking is the caller's
    responsibility.
    - This function DOES NOT acquire the RM API or GPU locks. That is because
    it is called during fault servicing, which could produce deadlocks.

    Arguments:
        pFaultInfo[IN]        - information provided by RM for fault handling.
                                Contains a pointer to the shadow fault buffer
        hasPendingFaults[OUT] - return value that tells if there are
                                non-replayable faults ready to be consumed by
                                the client

    Error codes:
      NV_ERR_INVALID_ARGUMENT
*/
NV_STATUS nvUvmInterfaceHasPendingNonReplayableFaults(UvmGpuFaultInfo *pFaultInfo,
                                                      NvBool *hasPendingFaults);

/*******************************************************************************
    nvUvmInterfaceGetNonReplayableFaults

    This function consumes all the non-replayable fault packets in the client
    shadow fault buffer and copies them to the given buffer. It also returns the
    number of faults that have been copied

    NOTES:
    - This function uses a pre-allocated stack per GPU (stored in the
    UvmGpuFaultInfo object) for calls from the bottom half that handles
    non-replayable faults.
    - See nvUvmInterfaceHasPendingNonReplayableFaults for the implications of
    using a shared stack.
    - This function DOES NOT acquire the RM API or GPU locks. That is because
    it is called during fault servicing, which could produce deadlocks.

    Arguments:
        pFaultInfo[IN]    - information provided by RM for fault handling.
                            Contains a pointer to the shadow fault buffer
        pFaultBuffer[OUT] - buffer provided by the client where fault buffers
                            are copied when they are popped out of the shadow
                            fault buffer (which is a circular queue).
        numFaults[OUT]    - return value that tells the number of faults copied
                            to the client's buffer

    Error codes:
      NV_ERR_INVALID_ARGUMENT
*/
NV_STATUS nvUvmInterfaceGetNonReplayableFaults(UvmGpuFaultInfo *pFaultInfo,
                                               void *pFaultBuffer,
                                               NvU32 *numFaults);

/*******************************************************************************
    nvUvmInterfaceFlushReplayableFaultBuffer

    This function sends an RPC to GSP in order to flush the HW replayable fault buffer.

    NOTES:
    - This function DOES NOT acquire the RM API or GPU locks. That is because
    it is called during fault servicing, which could produce deadlocks.
    - This function should not be called when interrupts are disabled.

    Arguments:
        pFaultInfo[IN]        - information provided by RM for fault handling.
                                used for obtaining the device handle without locks.
        bCopyAndFlush[IN]     - Instructs RM to perform the flush in the Copy+Flush mode.
                                In this mode, RM will perform a copy of the packets from
                                the HW buffer to UVM's SW buffer as part of performing
                                the flush. This mode gives UVM the opportunity to observe
                                the packets contained within the HW buffer at the time
                                of issuing the call.

    Error codes:
      NV_ERR_INVALID_ARGUMENT
*/
NV_STATUS nvUvmInterfaceFlushReplayableFaultBuffer(UvmGpuFaultInfo *pFaultInfo,
                                                   NvBool bCopyAndFlush);

/*******************************************************************************
    nvUvmInterfaceTogglePrefetchFaults

    This function sends an RPC to GSP in order to toggle the prefetch fault PRI.

    NOTES:
    - This function DOES NOT acquire the RM API or GPU locks. That is because
    it is called during fault servicing, which could produce deadlocks.
    - This function should not be called when interrupts are disabled.

    Arguments:
        pFaultInfo[IN]        - Information provided by RM for fault handling.
                                Used for obtaining the device handle without locks.
        bEnable[IN]           - Instructs RM whether to toggle generating faults on
                                prefetch on/off.

    Error codes:
      NV_ERR_INVALID_ARGUMENT
*/
NV_STATUS nvUvmInterfaceTogglePrefetchFaults(UvmGpuFaultInfo *pFaultInfo,
                                             NvBool bEnable);

/*******************************************************************************
    nvUvmInterfaceInitAccessCntrInfo

    This function obtains access counter buffer address, size and a few register mappings

    Arguments:
        device[IN]           - Device handle associated with the gpu
        pAccessCntrInfo[OUT] - Information provided by RM for access counter handling
        accessCntrIndex[IN]  - Access counter index

    Error codes:
      NV_ERR_GENERIC
      NV_ERR_INVALID_ARGUMENT
*/
NV_STATUS nvUvmInterfaceInitAccessCntrInfo(uvmGpuDeviceHandle device,
                                           UvmGpuAccessCntrInfo *pAccessCntrInfo,
                                           NvU32 accessCntrIndex);

/*******************************************************************************
    nvUvmInterfaceDestroyAccessCntrInfo

    This function obtains, destroys, unmaps the access counter buffer and clears accessCntrInfo

    Arguments:
        device[IN]          - Device handle associated with the gpu
        pAccessCntrInfo[IN] - Information provided by RM for access counter handling

    Error codes:
      NV_ERR_GENERIC
      NV_ERR_INVALID_ARGUMENT
*/
NV_STATUS nvUvmInterfaceDestroyAccessCntrInfo(uvmGpuDeviceHandle device,
                                              UvmGpuAccessCntrInfo *pAccessCntrInfo);

/*******************************************************************************
    nvUvmInterfaceEnableAccessCntr

    This function enables access counters using the given configuration
    UVM is also taking ownership from the RM.
    This causes the following: RM will not service, enable or disable this
    interrupt and it is up to the UVM driver to handle this interrupt. In
    this case, access counter notificaion interrupts are enabled by this
    function before it returns.

    Arguments:
        device[IN]            - Device handle associated with the gpu
        pAccessCntrInfo[IN]   - Pointer to structure filled out by nvUvmInterfaceInitAccessCntrInfo
        pAccessCntrConfig[IN] - Configuration for access counters

    Error codes:
      NV_ERR_GENERIC
      NV_ERR_INVALID_ARGUMENT
*/
NV_STATUS nvUvmInterfaceEnableAccessCntr(uvmGpuDeviceHandle device,
                                         UvmGpuAccessCntrInfo *pAccessCntrInfo,
                                         const UvmGpuAccessCntrConfig *pAccessCntrConfig);

/*******************************************************************************
    nvUvmInterfaceDisableAccessCntr

    This function disables acccess counters
    UVM is also returning ownership to the RM: RM can service, enable or
    disable this interrupt. In this case, access counter notificaion interrupts
    are disabled by this function before it returns.

    Arguments:
        device[IN]           - Device handle associated with the gpu
        pAccessCntrInfo[IN]  - Pointer to structure filled out by nvUvmInterfaceInitAccessCntrInfo

    Error codes:
      NV_ERR_GENERIC
      NV_ERR_INVALID_ARGUMENT
*/
NV_STATUS nvUvmInterfaceDisableAccessCntr(uvmGpuDeviceHandle device,
                                          UvmGpuAccessCntrInfo *pAccessCntrInfo);

//
// Called by the UVM driver to register operations with RM. Only one set of
// callbacks can be registered by any driver at a time. If another set of
// callbacks was already registered, NV_ERR_IN_USE is returned.
//
NV_STATUS nvUvmInterfaceRegisterUvmCallbacks(struct UvmOpsUvmEvents *importedUvmOps);

//
// Counterpart to nvUvmInterfaceRegisterUvmCallbacks. This must only be called
// if nvUvmInterfaceRegisterUvmCallbacks returned NV_OK.
//
// Upon return, the caller is guaranteed that any outstanding callbacks are done
// and no new ones will be invoked.
//
void nvUvmInterfaceDeRegisterUvmOps(void);

/*******************************************************************************
    nvUvmInterfaceGetNvlinkInfo

    Gets NVLINK information from RM.

    Arguments:
        device[IN]        - GPU device handle
        nvlinkInfo [OUT]     - Pointer to NvlinkInfo structure

    Error codes:
      NV_ERROR
      NV_ERR_INVALID_ARGUMENT
*/
NV_STATUS nvUvmInterfaceGetNvlinkInfo(uvmGpuDeviceHandle device,
                                      UvmGpuNvlinkInfo *nvlinkInfo);

/*******************************************************************************
    nvUvmInterfaceP2pObjectCreate

    This API creates an NV50_P2P object for the GPUs with the given device
    handles, and returns the handle to the object.

    Arguments:
        device1[IN]        - first GPU device handle
        device2[IN]        - second GPU device handle
        hP2pObject[OUT]    - handle to the created P2p object.

    Error codes:
      NV_ERR_INVALID_ARGUMENT
      NV_ERR_OBJECT_NOT_FOUND : If device object associated with the device
                                handles isn't found.
*/
NV_STATUS nvUvmInterfaceP2pObjectCreate(uvmGpuDeviceHandle device1,
                                        uvmGpuDeviceHandle device2,
                                        NvHandle *hP2pObject);

/*******************************************************************************
    nvUvmInterfaceP2pObjectDestroy

    This API destroys the NV50_P2P associated with the passed handle.

    Arguments:
        session[IN]        - Session handle.
        hP2pObject[IN]     - handle to an P2p object.

    Error codes: NONE
*/
void nvUvmInterfaceP2pObjectDestroy(uvmGpuSessionHandle session,
                                    NvHandle hP2pObject);

/*******************************************************************************
    nvUvmInterfaceGetExternalAllocPtes

    The interface builds the RM PTEs using the provided input parameters.

    Arguments:
        vaSpace[IN]                     -  vaSpace handle.
        hMemory[IN]                     -  Memory handle.
        offset [IN]                     -  Offset from the beginning of the allocation
                                           where PTE mappings should begin.
                                           Should be aligned with mappingPagesize
                                           in gpuExternalMappingInfo associated
                                           with the allocation.
        size [IN]                       -  Length of the allocation for which PTEs
                                           should be built.
                                           Should be aligned with mappingPagesize
                                           in gpuExternalMappingInfo associated
                                           with the allocation.
                                           size = 0 will be interpreted as the total size
                                           of the allocation.
        gpuExternalMappingInfo[IN/OUT]  -  See nv_uvm_types.h for more information.

   Error codes:
        NV_ERR_INVALID_ARGUMENT         - Invalid parameter/s is passed.
        NV_ERR_INVALID_OBJECT_HANDLE    - Invalid memory handle is passed.
        NV_ERR_NOT_SUPPORTED            - Functionality is not supported (see comments in nv_gpu_ops.c)
        NV_ERR_INVALID_BASE             - offset is beyond the allocation size
        NV_ERR_INVALID_LIMIT            - (offset + size) is beyond the allocation size.
        NV_ERR_BUFFER_TOO_SMALL         - gpuExternalMappingInfo.pteBufferSize is insufficient to
                                          store single PTE.
        NV_ERR_NOT_READY                - Returned when querying the PTEs requires a deferred setup
                                          which has not yet completed. It is expected that the caller
                                          will reattempt the call until a different code is returned.
                                          As an example, multi-node systems which require querying
                                          PTEs from the Fabric Manager may return this code.
*/
NV_STATUS nvUvmInterfaceGetExternalAllocPtes(uvmGpuAddressSpaceHandle vaSpace,
                                             NvHandle hMemory,
                                             NvU64 offset,
                                             NvU64 size,
                                             UvmGpuExternalMappingInfo *gpuExternalMappingInfo);

/*******************************************************************************
    nvUvmInterfaceGetExternalAllocPhysAddrs

    The interface builds the RM physical addrs using the provided input parameters.

    Arguments:
        vaSpace[IN]                     -  vaSpace handle.
        hMemory[IN]                     -  Memory handle.
        offset [IN]                     -  Offset from the beginning of the allocation
                                           where PTE mappings should begin.
                                           Should be aligned with mappingPagesize
                                           in gpuExternalMappingInfo associated
                                           with the allocation.
        size [IN]                       -  Length of the allocation for which PhysAddrs
                                           should be built.
                                           Should be aligned with mappingPagesize
                                           in gpuExternalMappingInfo associated
                                           with the allocation.
                                           size = 0 will be interpreted as the total size
                                           of the allocation.
        gpuExternalMappingInfo[IN/OUT]  -  See nv_uvm_types.h for more information.

   Error codes:
        NV_ERR_INVALID_ARGUMENT         - Invalid parameter/s is passed.
        NV_ERR_INVALID_OBJECT_HANDLE    - Invalid memory handle is passed.
        NV_ERR_NOT_SUPPORTED            - Functionality is not supported (see comments in nv_gpu_ops.c)
        NV_ERR_INVALID_BASE             - offset is beyond the allocation size
        NV_ERR_INVALID_LIMIT            - (offset + size) is beyond the allocation size.
        NV_ERR_BUFFER_TOO_SMALL         - gpuExternalMappingInfo.physAddrBufferSize is insufficient to
                                          store single physAddr.
        NV_ERR_NOT_READY                - Returned when querying the physAddrs requires a deferred setup
                                          which has not yet completed. It is expected that the caller
                                          will reattempt the call until a different code is returned.
                                          As an example, multi-node systems which require querying
                                          physAddrs from the Fabric Manager may return this code.
*/
NV_STATUS nvUvmInterfaceGetExternalAllocPhysAddrs(uvmGpuAddressSpaceHandle vaSpace,
                                                  NvHandle hMemory,
                                                  NvU64 offset,
                                                  NvU64 size,
                                                  UvmGpuExternalPhysAddrInfo *gpuExternalPhysAddrsInfo);

/*******************************************************************************
    nvUvmInterfaceRetainChannel

    Validates and returns information about the user's channel and its resources
    (local CTX buffers + global CTX buffers). The state is refcounted and must be
    released by calling nvUvmInterfaceReleaseChannel.

    Arguments:
        vaSpace[IN]               - vaSpace handle.
        hClient[IN]               - Client handle
        hChannel[IN]              - Channel handle
        retainedChannel[OUT]      - Opaque pointer to use to refer to this
                                    channel in other nvUvmInterface APIs.
        channelInstanceInfo[OUT]  - Channel instance information to be filled out.
                                    See nv_uvm_types.h for details.

    Error codes:
        NV_ERR_INVALID_ARGUMENT : If the parameter/s are invalid.
        NV_ERR_OBJECT_NOT_FOUND : If the object associated with the handle isn't found.
        NV_ERR_INVALID_CHANNEL : If the channel verification fails.
        NV_ERR_INSUFFICIENT_RESOURCES : If no memory available to store the resource information.
 */
NV_STATUS nvUvmInterfaceRetainChannel(uvmGpuAddressSpaceHandle vaSpace,
                                      NvHandle hClient,
                                      NvHandle hChannel,
                                      void **retainedChannel,
                                      UvmGpuChannelInstanceInfo *channelInstanceInfo);

/*******************************************************************************
    nvUvmInterfaceBindChannelResources

    Associates the mapping address of the channel resources (VAs) provided by the
    caller with the channel.

    Arguments:
        retainedChannel[IN]           - Channel pointer returned by nvUvmInterfaceRetainChannel
        channelResourceBindParams[IN] - Buffer of initialized UvmGpuChannelInstanceInfo::resourceCount
                                        entries. See nv_uvm_types.h for details.

    Error codes:
        NV_ERR_INVALID_ARGUMENT : If the parameter/s are invalid.
        NV_ERR_OBJECT_NOT_FOUND : If the object associated with the handle aren't found.
        NV_ERR_INSUFFICIENT_RESOURCES : If no memory available to store the resource information.
 */
NV_STATUS nvUvmInterfaceBindChannelResources(void *retainedChannel,
                                             UvmGpuChannelResourceBindParams *channelResourceBindParams);

/*******************************************************************************
    nvUvmInterfaceReleaseChannel

    Releases state retained by nvUvmInterfaceRetainChannel.
 */
void nvUvmInterfaceReleaseChannel(void *retainedChannel);

/*******************************************************************************
    nvUvmInterfaceStopChannel

    Idles the channel and takes it off the runlist.

    Arguments:
        retainedChannel[IN]           - Channel pointer returned by nvUvmInterfaceRetainChannel
        bImmediate[IN]                - If true, kill the channel without attempting to wait for it to go idle.
*/
void nvUvmInterfaceStopChannel(void *retainedChannel, NvBool bImmediate);

/*******************************************************************************
    nvUvmInterfaceGetChannelResourcePtes

    The interface builds the RM PTEs using the provided input parameters.

    Arguments:
        vaSpace[IN]                     -  vaSpace handle.
        resourceDescriptor[IN]          -  The channel resource descriptor returned by returned by
                                           nvUvmInterfaceRetainChannelResources.
        offset[IN]                      -  Offset from the beginning of the allocation
                                           where PTE mappings should begin.
                                           Should be aligned with pagesize associated
                                           with the allocation.
        size[IN]                        -  Length of the allocation for which PTEs
                                           should be built.
                                           Should be aligned with pagesize associated
                                           with the allocation.
                                           size = 0 will be interpreted as the total size
                                           of the allocation.
        gpuExternalMappingInfo[IN/OUT]  -  See nv_uvm_types.h for more information.

   Error codes:
        NV_ERR_INVALID_ARGUMENT         - Invalid parameter/s is passed.
        NV_ERR_INVALID_OBJECT_HANDLE    - Invalid memory handle is passed.
        NV_ERR_NOT_SUPPORTED            - Functionality is not supported.
        NV_ERR_INVALID_BASE             - offset is beyond the allocation size
        NV_ERR_INVALID_LIMIT            - (offset + size) is beyond the allocation size.
        NV_ERR_BUFFER_TOO_SMALL         - gpuExternalMappingInfo.pteBufferSize is insufficient to
                                          store single PTE.
*/
NV_STATUS nvUvmInterfaceGetChannelResourcePtes(uvmGpuAddressSpaceHandle vaSpace,
                                               NvP64 resourceDescriptor,
                                               NvU64 offset,
                                               NvU64 size,
                                               UvmGpuExternalMappingInfo *externalMappingInfo);

/*******************************************************************************
    nvUvmInterfaceReportNonReplayableFault

    The interface communicates a nonreplayable fault packet from UVM to RM, which
    will log the fault, notify the clients and then trigger RC on the channel.

    Arguments:
        device[IN]                     -  The device where the fault happened.
        pFaultPacket[IN]               -  The opaque pointer from UVM that will be later
                                          converted to a MMU_FAULT_PACKET type.
    Error codes:
        NV_ERR_INVALID_ARGUMENT         - Invalid parameter/s is passed.
        NV_ERR_NOT_SUPPORTED            - Functionality is not supported.
*/
NV_STATUS nvUvmInterfaceReportNonReplayableFault(uvmGpuDeviceHandle device,
                                                 const void *pFaultPacket);

/*******************************************************************************
    nvUvmInterfacePagingChannelAllocate

    In SR-IOV heavy, this function requests the allocation of a paging channel
    (i.e. a privileged CE channel) bound to a specified copy engine. Unlike
    channels allocated via nvUvmInterfaceChannelAllocate, the caller cannot push
    methods to a paging channel directly, but instead relies on the
    nvUvmInterfacePagingChannelPushStream API to do so.

    SR-IOV heavy only. The implementation of this interface can acquire
    RM or GPU locks.

    Arguments:
        device[IN]       - device under which the paging channel will be allocated
        allocParams[IN]  - structure with allocation settings
        channel[OUT]     - pointer to the allocated paging channel handle
        channelInfo[OUT] - structure filled with channel information

   Error codes:
        NV_ERR_INVALID_ARGUMENT         - Invalid parameter/s is passed.
        NV_ERR_NO_MEMORY                - Not enough memory to allocate
                                          paging channel/shadow notifier.
        NV_ERR_NOT_SUPPORTED            - SR-IOV heavy mode is disabled.

 */
NV_STATUS nvUvmInterfacePagingChannelAllocate(uvmGpuDeviceHandle device,
                                              const UvmGpuPagingChannelAllocParams *allocParams,
                                              UvmGpuPagingChannelHandle *channel,
                                              UvmGpuPagingChannelInfo *channelInfo);

/*******************************************************************************
    nvUvmInterfacePagingChannelDestroy

    This function destroys a given paging channel.

    SR-IOV heavy only. The implementation of this interface can acquire
    RM or GPU locks.

    Arguments:
        channel[IN] - paging channel handle. If the passed handle is
                      the NULL pointer, the function returns immediately.
*/
void nvUvmInterfacePagingChannelDestroy(UvmGpuPagingChannelHandle channel);

/*******************************************************************************

    nvUvmInterfacePagingChannelsMap

    Map a guest allocation in the address space associated with all the paging
    channels allocated under the given device.

    SR-IOV heavy only. The implementation of this interface can acquire
    RM or GPU locks.

    Arguments:
        srcVaSpace[IN]  - VA space handle used to allocate the input pointer
                          srcAddress.
        srcAddress[IN]  - virtual address returned by nvUvmInterfaceMemoryAllocFB
                          or nvUvmInterfaceMemoryAllocSys. The entire allocation
                          backing this guest VA is mapped.
        device[IN]      - device under which paging channels were allocated
        dstAddress[OUT] - a virtual address that is valid (i.e. is mapped) in
                          all the paging channels allocated under the given vaSpace.

   Error codes:
        NV_ERR_INVALID_ARGUMENT         - Invalid parameter/s is passed.
        NV_ERR_NOT_SUPPORTED            - SR-IOV heavy mode is disabled.
*/
NV_STATUS nvUvmInterfacePagingChannelsMap(uvmGpuAddressSpaceHandle srcVaSpace,
                                          UvmGpuPointer srcAddress,
                                          uvmGpuDeviceHandle device,
                                          NvU64 *dstAddress);

/*******************************************************************************

    nvUvmInterfacePagingChannelsUnmap

    Unmap a VA returned by nvUvmInterfacePagingChannelsMap.

    SR-IOV heavy only. The implementation of this interface can acquire
    RM or GPU locks.

    Arguments:
        srcVaSpace[IN] - VA space handle that was passed to prevous mapping.
        srcAddress[IN] - virtual address that was passed to prevous mapping.
        device[IN]     - device under which paging channels were allocated.
 */
void nvUvmInterfacePagingChannelsUnmap(uvmGpuAddressSpaceHandle srcVaSpace,
                                       UvmGpuPointer srcAddress,
                                       uvmGpuDeviceHandle device);


/*******************************************************************************
    nvUvmInterfacePagingChannelPushStream

    Used for remote execution of the passed methods; the UVM driver uses this
    interface to ask the vGPU plugin to execute certain HW methods on its
    behalf. The callee should push the methods in the specified order i.e. is
    not allowed to do any reordering.

    The API is asynchronous. The UVM driver can wait on the remote execution by
    inserting a semaphore release method at the end of the method stream, and
    then loop until the semaphore value reaches the completion value indicated
    in the release method.

    The valid HW methods that can be passed by the UVM driver follow; the source
    functions listed contain the exact formatting (encoding) of the HW method
    used by the UVM driver for Ampere.

      - TLB invalidation targeting a VA range. See
        uvm_hal_volta_host_tlb_invalidate_va.

      - TLB invalidation targeting certain levels in the page tree (including
        the possibility of invalidating everything).
        See uvm_hal_pascal_host_tlb_invalidate_all.

      - Replayable fault replay. See uvm_hal_volta_replay_faults.

      - Replayable fault cancellation targeting a guest virtual address. See
        uvm_hal_volta_cancel_faults_va

      - Membar, scoped to device or to the entire system. See
        uvm_hal_pascal_host_membar_gpu and uvm_hal_pascal_host_membar_sys

      - Host semaphore acquire, see uvm_hal_turing_host_semaphore_acquire. The
        virtual address specified in the semaphore operation must lie within a
        buffer previously mapped by nvUvmInterfacePagingChannelsMap.

      - CE semaphore release, see uvm_hal_pascal_ce_semaphore_release. The
        virtual address specified in the semaphore operation must lie within a
        buffer previously mapped by nvUvmInterfacePagingChannelsMap.

      - 64 bits-wide memset, see uvm_hal_kepler_ce_memset_8. The destination
        address is a physical address in vidmem.

      - No-op, see uvm_hal_kepler_host_noop. Used to store the source buffer
        of a memcopy method within the input stream itself.

      - Memcopy, see uvm_hal_kepler_ce_memcopy. The destination address is a
        physical address in vidmem. The source address is an offset within
        methodStream, in bytes, indicating the location of the (inlined) source
        buffer. The copy size does not exceed 4KB.

      - CE semaphore release with timestamp, see
        uvm_hal_kepler_ce_semaphore_timestamp. The virtual address specified in
        the semaphore operation must lie within a buffer previously mapped by
        nvUvmInterfacePagingChannelsMap.

      - CE semaphore reduction, see uvm_hal_kepler_ce_semaphore_reduction_inc.
        The virtual address specified in the semaphore operation must lie within
        a buffer previously mapped by nvUvmInterfacePagingChannelsMap.

    Only invoked in SR-IOV heavy mode.

    NOTES:
      - This function uses a pre-allocated stack per paging channel
        (stored in the UvmGpuPagingChannel object)
      - This function DOES NOT acquire the RM API or GPU locks. That is because
        it is called during fault servicing, which could produce deadlocks.
      - Concurrent calls to this function using channels under same device are not
        allowed due to:
          a. pre-allocated stack
          b. the fact that internal RPC infrastructure doesn't acquire GPU lock.
        Therefore, locking is the caller's responsibility.

    Arguments:
        channel[IN]          - paging channel handle obtained via
                               nvUvmInterfacePagingChannelAllocate

        methodStream[IN]     - HW methods to be pushed to the paging channel.

        methodStreamSize[IN] - Size of methodStream, in bytes. The maximum push
                               size is 128KB.


   Error codes:
        NV_ERR_INVALID_ARGUMENT         - Invalid parameter/s is passed.
        NV_ERR_NOT_SUPPORTED            - SR-IOV heavy mode is disabled.
*/
NV_STATUS nvUvmInterfacePagingChannelPushStream(UvmGpuPagingChannelHandle channel,
                                                char *methodStream,
                                                NvU32 methodStreamSize);

/*******************************************************************************
    nvUvmInterfaceReportFatalError

    Reports a global fatal error so RM can inform the clients that a node reboot
    is necessary to recover from this error. This function can be called from
    any lock environment, bottom half or non-interrupt context.

*/
void nvUvmInterfaceReportFatalError(NV_STATUS error);

/*******************************************************************************
    Cryptography Services Library (CSL) Interface
*/

/*******************************************************************************
    nvUvmInterfaceCslInitContext

    Allocates and initializes a CSL context for a given secure channel.

    The lifetime of the context is the same as the lifetime of the secure channel
    it is paired with.

    Locking: This function acquires an API lock.
    Memory : This function dynamically allocates memory.

    Arguments:
        uvmCslContext[IN/OUT] - The CSL context associated with a channel.
        channel[IN]           - Handle to a secure channel.

    Error codes:
      NV_ERR_INVALID_STATE   - The system is not operating in Confidential Compute mode.
      NV_ERR_INVALID_CHANNEL - The associated channel is not a secure channel.
      NV_ERR_IN_USE          - The context has already been initialized.
*/
NV_STATUS nvUvmInterfaceCslInitContext(UvmCslContext *uvmCslContext,
                                       uvmGpuChannelHandle channel);

/*******************************************************************************
    nvUvmInterfaceDeinitCslContext

    Securely deinitializes and clears the contents of a context.

    If context is already deinitialized then function returns immediately.

    Locking: This function does not acquire an API or GPU lock.
    Memory : This function may free memory.

    Arguments:
        uvmCslContext[IN] - The CSL context associated with a channel.
*/
void nvUvmInterfaceDeinitCslContext(UvmCslContext *uvmCslContext);

/*******************************************************************************
    nvUvmInterfaceCslRotateKey

    Disables channels and rotates keys.

    This function disables channels and rotates associated keys. The channels
    associated with the given CSL contexts must be idled before this function is
    called. To trigger key rotation all allocated channels for a given key must
    be present in the list. If the function returns successfully then the CSL
    contexts have been updated with the new key.

    Locking: This function attempts to acquire the GPU lock. In case of failure
             to acquire the return code is NV_ERR_STATE_IN_USE. The caller must
             guarantee that no CSL function, including this one, is invoked
             concurrently with the CSL contexts in contextList.
    Memory : This function dynamically allocates memory.

    Arguments:
        contextList[IN/OUT]  - An array of pointers to CSL contexts.
        contextListCount[IN] - Number of CSL contexts in contextList. Its value
                               must be greater than 0.
    Error codes:
        NV_ERR_INVALID_ARGUMENT - contextList is NULL or contextListCount is 0.
        NV_ERR_STATE_IN_USE     - Unable to acquire lock / resource. Caller
                                  can retry at a later time.
        NV_ERR_GENERIC          - A failure other than _STATE_IN_USE occurred
                                  when attempting to acquire a lock.
*/
NV_STATUS nvUvmInterfaceCslRotateKey(UvmCslContext *contextList[],
                                     NvU32 contextListCount);

/*******************************************************************************
    nvUvmInterfaceCslRotateIv

    Rotates the IV for a given channel and operation.

    This function will rotate the IV on both the CPU and the GPU.
    For a given operation the channel must be idle before calling this function.
    This function can be called regardless of the value of the IV's message counter.

    Locking: This function attempts to acquire the GPU lock. In case of failure to
             acquire the return code is NV_ERR_STATE_IN_USE. The caller must guarantee
             that no CSL function, including this one, is invoked concurrently with
             the same CSL context.
    Memory : This function does not dynamically allocate memory.

Arguments:
        uvmCslContext[IN/OUT] - The CSL context associated with a channel.
        operation[IN]         - Either
                                - UVM_CSL_OPERATION_ENCRYPT
                                - UVM_CSL_OPERATION_DECRYPT

    Error codes:
      NV_ERR_INSUFFICIENT_RESOURCES - The rotate operation would cause a counter
                                      to overflow.
      NV_ERR_STATE_IN_USE           - Unable to acquire lock / resource. Caller
                                      can retry at a later time.
      NV_ERR_INVALID_ARGUMENT       - Invalid value for operation.
      NV_ERR_GENERIC                - A failure other than _STATE_IN_USE occurred
                                      when attempting to acquire a lock.
*/
NV_STATUS nvUvmInterfaceCslRotateIv(UvmCslContext *uvmCslContext,
                                    UvmCslOperation operation);

/*******************************************************************************
    nvUvmInterfaceCslEncrypt

    Encrypts data and produces an authentication tag.

    Auth, input, and output buffers must not overlap. If they do then calling
    this function produces undefined behavior. Performance is typically
    maximized when the input and output buffers are 16-byte aligned. This is
    natural alignment for AES block.
    The encryptIV can be obtained from nvUvmInterfaceCslIncrementIv.
    However, it is optional. If it is NULL, the next IV in line will be used.

    Locking: This function does not acquire an API or GPU lock.
             The caller must guarantee that no CSL function, including this one,
             is invoked concurrently with the same CSL context.
    Memory : This function does not dynamically allocate memory.

Arguments:
        uvmCslContext[IN/OUT] - The CSL context associated with a channel.
        bufferSize[IN]        - Size of the input and output buffers in
                                units of bytes. Value can range from 1 byte
                                to (2^32) - 1 bytes.
        inputBuffer[IN]       - Address of plaintext input buffer.
        encryptIv[IN/OUT]     - IV to use for encryption. Can be NULL.
        outputBuffer[OUT]     - Address of ciphertext output buffer.
        authTagBuffer[OUT]    - Address of authentication tag buffer.
                                Its size is UVM_CSL_CRYPT_AUTH_TAG_SIZE_BYTES.

    Error codes:
      NV_ERR_INVALID_ARGUMENT - The CSL context is not associated with a channel.
                              - The size of the data is 0 bytes.
                              - The encryptIv has already been used.
*/
NV_STATUS nvUvmInterfaceCslEncrypt(UvmCslContext *uvmCslContext,
                                   NvU32 bufferSize,
                                   NvU8 const *inputBuffer,
                                   UvmCslIv *encryptIv,
                                   NvU8 *outputBuffer,
                                   NvU8 *authTagBuffer);

/*******************************************************************************
    nvUvmInterfaceCslDecrypt

    Verifies the authentication tag and decrypts data.

    Auth, input, and output buffers must not overlap. If they do then calling
    this function produces undefined behavior. Performance is typically
    maximized when the input and output buffers are 16-byte aligned. This is
    natural alignment for AES block.

    During a key rotation event the previous key is stored in the CSL context.
    This allows data encrypted by the GPU to be decrypted with the previous key.
    The keyRotationId parameter identifies which key is used. The first key rotation
    ID has a value of 0 that increments by one for each key rotation event.

    Locking: This function does not acquire an API or GPU lock.
             The caller must guarantee that no CSL function, including this one,
             is invoked concurrently with the same CSL context.
    Memory : This function does not dynamically allocate memory.

    Arguments:
        uvmCslContext[IN/OUT] - The CSL context.
        bufferSize[IN]        - Size of the input and output buffers in units of bytes.
                                Value can range from 1 byte to (2^32) - 1 bytes.
        decryptIv[IN]         - IV used to decrypt the ciphertext. Its value can either be given by
                                nvUvmInterfaceCslIncrementIv, or, if NULL, the CSL context's
                                internal counter is used.
        keyRotationId[IN]     - Specifies the key that is used for decryption.
                                A value of NV_U32_MAX specifies the current key.
        inputBuffer[IN]       - Address of ciphertext input buffer.
        outputBuffer[OUT]     - Address of plaintext output buffer.
        addAuthData[IN]       - Address of the plaintext additional authenticated data used to
                                calculate the authentication tag. Can be NULL.
        addAuthDataSize[IN]   - Size of the additional authenticated data in units of bytes.
                                Value can range from 1 byte to (2^32) - 1 bytes.
                                This parameter is ignored if addAuthData is NULL.
        authTagBuffer[IN]     - Address of authentication tag buffer.
                                Its size is UVM_CSL_CRYPT_AUTH_TAG_SIZE_BYTES.

    Error codes:
      NV_ERR_INSUFFICIENT_RESOURCES - The decryption operation would cause a
                                      counter overflow to occur.
      NV_ERR_INVALID_ARGUMENT       - The size of the data is 0 bytes.
      NV_ERR_INVALID_DATA           - Verification of the authentication tag fails.
*/
NV_STATUS nvUvmInterfaceCslDecrypt(UvmCslContext *uvmCslContext,
                                   NvU32 bufferSize,
                                   NvU8 const *inputBuffer,
                                   UvmCslIv const *decryptIv,
                                   NvU32 keyRotationId,
                                   NvU8 *outputBuffer,
                                   NvU8 const *addAuthData,
                                   NvU32 addAuthDataSize,
                                   NvU8 const *authTagBuffer);

/*******************************************************************************
    nvUvmInterfaceCslSign

    Generates an authentication tag for secure work launch.

    Auth and input buffers must not overlap. If they do then calling this function produces
    undefined behavior.

    Locking: This function does not acquire an API or GPU lock.
             The caller must guarantee that no CSL function, including this one,
             is invoked concurrently with the same CSL context.
    Memory : This function does not dynamically allocate memory.

    Arguments:
        uvmCslContext[IN/OUT] - The CSL context associated with a channel.
        bufferSize[IN]        - Size of the input buffer in units of bytes.
                                Value can range from 1 byte to (2^32) - 1 bytes.
        inputBuffer[IN]       - Address of plaintext input buffer.
        authTagBuffer[OUT]    - Address of authentication tag buffer.
                                Its size is UVM_CSL_SIGN_AUTH_TAG_SIZE_BYTES.

    Error codes:
      NV_ERR_INSUFFICIENT_RESOURCES - The signing operation would cause a counter overflow to occur.
      NV_ERR_INVALID_ARGUMENT       - The CSL context is not associated with a channel.
                                    - The size of the data is 0 bytes.
*/
NV_STATUS nvUvmInterfaceCslSign(UvmCslContext *uvmCslContext,
                                NvU32 bufferSize,
                                NvU8 const *inputBuffer,
                                NvU8 *authTagBuffer);

/*******************************************************************************
    nvUvmInterfaceCslQueryMessagePool

    Returns the number of messages that can be encrypted before the message counter will overflow.

    Locking: This function does not acquire an API or GPU lock.
    Memory : This function does not dynamically allocate memory.
             The caller must guarantee that no CSL function, including this one,
             is invoked concurrently with the same CSL context.

    Arguments:
        uvmCslContext[IN/OUT] - The CSL context.
        operation[IN]         - Either UVM_CSL_OPERATION_ENCRYPT or UVM_CSL_OPERATION_DECRYPT.
        messageNum[OUT]       - Number of messages left before overflow.

    Error codes:
      NV_ERR_INVALID_ARGUMENT - The value of the operation parameter is illegal.
*/
NV_STATUS nvUvmInterfaceCslQueryMessagePool(UvmCslContext *uvmCslContext,
                                            UvmCslOperation operation,
                                            NvU64 *messageNum);

/*******************************************************************************
    nvUvmInterfaceCslIncrementIv

    Increments the message counter by the specified amount.

    If iv is non-NULL then the incremented value is returned.
    If operation is UVM_CSL_OPERATION_ENCRYPT then the returned IV's "freshness" bit is set and
    can be used in nvUvmInterfaceCslEncrypt. If operation is UVM_CSL_OPERATION_DECRYPT then
    the returned IV can be used in nvUvmInterfaceCslDecrypt.

    Locking: This function does not acquire an API or GPU lock.
             The caller must guarantee that no CSL function, including this one,
             is invoked concurrently with the same CSL context.
    Memory : This function does not dynamically allocate memory.

Arguments:
        uvmCslContext[IN/OUT] - The CSL context.
        operation[IN]         - Either
                                - UVM_CSL_OPERATION_ENCRYPT
                                - UVM_CSL_OPERATION_DECRYPT
        increment[IN]         - The amount by which the IV is incremented. Can be 0.
        iv[OUT]               - If non-NULL, a buffer to store the incremented IV.

    Error codes:
      NV_ERR_INVALID_ARGUMENT       - The value of the operation parameter is illegal.
      NV_ERR_INSUFFICIENT_RESOURCES - Incrementing the message counter would result
                                      in an overflow.
*/
NV_STATUS nvUvmInterfaceCslIncrementIv(UvmCslContext *uvmCslContext,
                                       UvmCslOperation operation,
                                       NvU64 increment,
                                       UvmCslIv *iv);

/*******************************************************************************
    nvUvmInterfaceCslLogEncryption

    Checks and logs information about encryptions associated with the given
    CSL context.

    For contexts associated with channels, this function does not modify elements of
    the UvmCslContext, and must be called for every CPU/GPU encryption.

    For the context associated with fault buffers, bufferSize can encompass multiple
    encryption invocations, and the UvmCslContext will be updated following a key
    rotation event.

    In either case the IV remains unmodified after this function is called.

    Locking: This function does not acquire an API or GPU lock.
    Memory : This function does not dynamically allocate memory.
             The caller must guarantee that no CSL function, including this one,
             is invoked concurrently with the same CSL context.

    Arguments:
        uvmCslContext[IN/OUT] - The CSL context.
        operation[IN]         - If the CSL context is associated with a fault
                                buffer, this argument is ignored. If it is
                                associated with a channel, it must be either
                                - UVM_CSL_OPERATION_ENCRYPT
                                - UVM_CSL_OPERATION_DECRYPT
        bufferSize[IN]        - The size of the buffer(s) encrypted by the
                                external entity in units of bytes.

    Error codes:
      NV_ERR_INSUFFICIENT_RESOURCES - The encryption would cause a counter
                                      to overflow.
*/
NV_STATUS nvUvmInterfaceCslLogEncryption(UvmCslContext *uvmCslContext,
                                         UvmCslOperation operation,
                                         NvU32 bufferSize);
#endif // _NV_UVM_INTERFACE_H_
