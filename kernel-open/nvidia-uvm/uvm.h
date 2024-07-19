/*******************************************************************************
    Copyright (c) 2013-2024 NVIDIA Corporation

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be
        included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*******************************************************************************/

//
// uvm.h
//
// This file contains the UVM API declarations, for the userspace-to-kernel
// calls. For legacy API definitions that are in use on Windows, see
// uvm_legacy.h.
//

// UVM API signature modification steps
// In order to change API signature for any of the APIs defined in this file, a
// particular sequence of steps has to be followed since the consumer of this
// API (i.e. CUDA) belongs to a different module branch than the one for this
// file. Here are the steps to change the signature for a hypothetical API named
// UvmExampleApi. The assumption being made here is that this file is being
// modified in chips_a.
// 1) Increment the value of UVM_API_LATEST_REVISION defined in this file.
// 2) Use the macro UVM_API_REV_IS_AT_MOST to define the two revisions of the
//    API as follows:
//     #if UVM_API_REV_IS_AT_MOST(<previous_value_of_UVM_API_LATEST_REVISION>)
//     // Old UvmExampleApi declaration
//     #else
//     // New UvmExampleApi declaration
//     #endif
// 3) Do the same thing for the function definition, and for any structs that
//    are taken as arguments to these functions.
// 4) Let this change propagate over to cuda_a and dev_a, so that the CUDA and
//    nvidia-cfg libraries can start using the new API by bumping up the API
//    version number it's using.
//    Places where UVM_API_REVISION is defined are:
//      drivers/gpgpu/cuda/cuda.nvmk (cuda_a)
//      drivers/setup/linux/nvidia-cfg/makefile.nvmk (dev_a)
// 5) Once the dev_a and cuda_a changes have made it back into chips_a,
//    remove the old API declaration, definition, and any old structs that were
//    in use.

#ifndef _UVM_H_
#define _UVM_H_

#define UVM_API_LATEST_REVISION 12

#if !defined(UVM_API_REVISION)
#error "please define UVM_API_REVISION macro to a desired version number or UVM_API_LATEST_REVISION macro"
#endif

#define UVM_API_REV_IS_AT_MOST(rev) (UVM_API_REVISION <= rev)

#include "uvm_types.h"
#include "uvm_user_types.h"
#include "uvm_legacy.h"

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// UvmSetDriverVersion
//
// Informs the user-mode layer which kernel driver version is running. The user-
// mode layer uses this information to know what flavor to use when calling
// kernel APIs.
//
// If this API is not called, the user-mode layer assumes that the kernel
// version is the same as the user-mode layer version.
//
// The last UvmDeinitialize will reset this state.
//
// If this API is called, it must be called before UvmInitialize. It is an error
// to call this API after UvmInitialize and before the last UvmDeinitialize, or
// to call this API more than once before the last UvmDeinitialize.
//
// Arguments:
//     major: (INPUT)
//         The kernel driver's major version number, such as 384.
//
//     changelist: (INPUT)
//         The changelist at which the kernel driver was built.
//
// Error codes:
//     NV_ERR_INVALID_STATE:
//         UvmInitialize or UvmSetDriverVersion has already been called.
//
//------------------------------------------------------------------------------
NV_STATUS UvmSetDriverVersion(NvU32 major, NvU32 changelist);

//------------------------------------------------------------------------------
// UvmInitialize
//
// This must be called before any other UVM functions except for
// UvmSetDriverVersion. Repeated calls to UvmInitialize increment a refcount,
// which is decremented by calls to UvmDeinitialize. UVM deinitilization occurs
// when the refcount reaches zero.
//
// The UVM file descriptor passed in can either be UVM_AUTO_FD or a valid file
// descriptor created during a prior call to UvmInitialize. If UVM_AUTO_FD is
// passed and the refcount is zero, a new file descriptor is created. Subsequent
// calls must either also specify UVM_AUTO_FD or use the current file
// descriptor. If the first call to UvmInitialize did not specify UVM_AUTO_FD,
// all subsequent calls must use the same file descriptor used in the initial
// call. The file descriptor that is currently in use can be retrieved using
// UvmGetFileDescriptor.
//
// If flags does not contain UVM_INIT_FLAGS_MULTI_PROCESS_SHARING_MODE, the
// UvmInitialize call which creates the file descriptor will associate the
// calling process with that file descriptor when the Operating System can
// support such an association. In that case UvmInitialize may be called using
// the same file in other processes, but internally the file remains associated
// with the original process.
//
// Arguments:
//     fd: (INPUT)
//         The UVM file descriptor to initialize UVM with. Passing in
//         UVM_AUTO_FD creates a new file descriptor on the first call to
//         UvmInitialize.
//
//     flags: (INPUT)
//         Must be a combination of 0 or more of following flags:
//
//         - UVM_INIT_FLAGS_DISABLE_HMM
//             Specifying this flag will only have an effect if the system
//             allows GPUs to read/write system (CPU) pageable memory and the
//             GPUs do not have hardware support to do it transparently, and the
//             UVM_INIT_FLAGS_MULTI_PROCESS_SHARING_MODE flag is not specified.
//             In such cases pageable access from the GPU will be disabled.
//
//             Pageable memory here refers to memory allocated by the Operating
//             System for the process's globals, stack variables, heap
//             allocations, etc. that has not been registered for CUDA access
//             using cudaHostRegister.
//
//         - UVM_INIT_FLAGS_MULTI_PROCESS_SHARING_MODE
//             Specifying this flag will prevent UVM from creating any
//             association between this process and the UVM file descriptor.
//             Pageable memory access of any kind will be disabled (regardless
//             of whether UVM_INIT_FLAGS_DISABLE_HMM was specified) and the GPU
//             resources used by the UVM file descriptor will be freed when the
//             last reference to the file is dropped rather than when this
//             process exits.
//
//             If this flag is not specified, calling UvmMemMap or
//             UvmAllocSemaphorePool on the same file from a different process
//             may return an error.
//
//         If UvmInitialize is called multiple times on the same file, even from
//         different processes, the flags to each call must match.
//
// Error codes:
//     NV_ERR_NOT_SUPPORTED:
//         The kernel is not able to support UVM. This could be because
//         the kernel is too old, or because it lacks a feature that UVM
//         requires. The kernel log will have details.
//
//     NV_ERR_INVALID_ARGUMENT:
//         The file descriptor passed in is neither UVM_AUTO_FD nor a valid file
//         descriptor created during a prior call to UvmInitialize, or the flags
//         do not match a prior call to UvmInitialize.
//
//     NV_ERR_NO_MEMORY:
//         Internal memory allocation failed.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmInitialize(UvmFileDescriptor fd,
                        NvU64             flags);

//------------------------------------------------------------------------------
// UvmDeinitialize
//
// Releases the reference implicitly obtained by UvmInitialize. If the refcount
// reaches zero, cleans up all UVM resources associated with the calling
// process. Any channels that are still registered will be unregistered prior to
// unmapping any managed allocations. Any resources that have been shared with
// other processes and are still being used will continue to remain valid.
//
// Error codes:
//     NV_ERR_INVALID_STATE:
//         Refcount is zero.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmDeinitialize(void);

//------------------------------------------------------------------------------
// UvmReopen
//
// Reinitializes the UVM driver after checking for minimal user-mode state.
// Before calling this function, all GPUs must be unregistered with
// UvmUnregisterGpu() and all allocated VA ranges must be freed with UvmFree().
// Note that it is not required to release VA ranges that were reserved with
// UvmReserveVa().
//
// This is useful for per-process checkpoint and restore, where kernel-mode
// state needs to be reconfigured to match the expectations of a pre-existing
// user-mode process.
//
// UvmReopen() closes the open file returned by UvmGetFileDescriptor() and
// replaces it with a new open file with the same name.
//
// Arguments:
//     flags: (INPUT)
//         Must be zero.  UVM will be reinitialized with the
//         Same flags that were passed to UvmInitialize() originally.
//
// Error codes:
//     NV_ERR_INVALID_STATE:
//         UVM was not initialized before calling this function.
//
//     NV_ERR_UVM_ADDRESS_IN_USE:
//         Not all allocated VA ranges were freed before calling this function.
//
//     NV_ERR_IN_USE:
//         Not all GPUs were unregistered before calling this function.
//
//     NV_ERR_INVALID_FLAGS:
//         Flags is not zero.
//
//     NV_ERR_OPERATING_SYSTEM:
//         Replacing the original UVM file descriptor failed.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmReopen(NvU64 flags);

//------------------------------------------------------------------------------
// UvmIsPageableMemoryAccessSupported
//
// Returns true only if pageable memory access from GPUs is supported by the
// system and that support was not explicitly disabled via UvmInitialize.
//
// Pageable memory here refers to memory allocated by the Operating System for
// the process's globals, stack variables, heap allocations, etc that has not
// been registered for CUDA access using cudaHostRegister.
//
// Note that this does not check whether GPUs are present which can make use of
// this feature, just whether system support exists. If
// UvmIsPageableMemoryAccessSupported reports that pageable memory access is
// supported, UvmIsPageableMemoryAccessSupportedOnGpu can be used for querying
// per-GPU support.
//
// Arguments:
//     pageableMemAccess: (OUTPUT)
//         Returns true (non-zero) if the system supports pageable memory access
//         from GPUs and that support was not explicitly disabled via
//         UvmInitialize, and false (zero) otherwise.
//
// Error codes:
//     NV_ERR_INVALID_STATE:
//         UVM was not initialized.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmIsPageableMemoryAccessSupported(NvBool *pageableMemAccess);

//------------------------------------------------------------------------------
// UvmIsPageableMemoryAccessSupportedOnGpu
//
// Returns whether pageable memory access is supported from the given GPU on
// this system and that support was not explicitly disabled via UvmInitialize.
// The GPU must have been previously registered with UvmRegisterGpu first.
//
// Pageable memory here refers to memory allocated by the Operating System for
// the process's globals, stack variables, heap allocations, etc that has not
// been registered for CUDA access using cudaHostRegister.
//
// Arguments:
//     gpuUuid: (INPUT)
//         UUID of the physical GPU if the GPU is not SMC capable or SMC
//         enabled, or the GPU instance UUID of the partition for which
//         pageable memory access support is queried.
//
//     pageableMemAccess: (OUTPUT)
//         Returns true (non-zero) if the GPU represented by gpuUuid supports
//         pageable memory access and that support was not explicitly disabled
//         via UvmInitialize, and false (zero) otherwise.
//
// Error codes:
//     NV_ERR_INVALID_STATE:
//         UVM was not initialized.
//
//     NV_ERR_INVALID_DEVICE:
//         The given GPU has not been registered.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmIsPageableMemoryAccessSupportedOnGpu(const NvProcessorUuid *gpuUuid,
                                                  NvBool *pageableMemAccess);

//------------------------------------------------------------------------------
// UvmRegisterGpu
//
// Registers a GPU with UVM. If this is the first process to register this GPU,
// the UVM driver initializes resources on the GPU and prepares it for CUDA
// usage. Calling UvmRegisterGpu multiple times on the same GPU from the same
// process results in an error.
//
// After successfully registering a GPU partition, all subsequent API calls
// which take a NvProcessorUuid argument (including UvmGpuMappingAttributes),
// must use the GI partition UUID which can be obtained with
// NvRmControl(NVC637_CTRL_CMD_GET_UUID). Otherwise, if the GPU is not SMC
// capable or SMC enabled, the physical GPU UUID must be used.
//
// Arguments:
//     gpuUuid: (INPUT)
//         UUID of the physical GPU to register.
//
//     platformParams: (INPUT)
//         User handles identifying the GPU partition to register.
//         This should be NULL if the GPU is not SMC capable or SMC enabled.
//
// Error codes:
//     NV_ERR_NO_MEMORY:
//         Internal memory allocation failed.
//
//     NV_ERR_INSUFFICIENT_RESOURCES
//         Internal client or object allocation failed.
//
//     NV_ERR_INVALID_DEVICE:
//         The GPU referred to by pGpuUuid has already been registered by this
//         process.
//
//         The GPU referred to by pGpuUuid doesn't have a NVLINK2 link to the
//         CPU but a GPU with such a link has already been registered by this
//         process, or vice-versa.
//
//     NV_ERR_NOT_SUPPORTED:
//         The GPU referred to by pGpuUuid is not supported by UVM or the GPU
//         is configured to run in virtualization mode without SRIOV support.
//
//     NV_ERR_GPU_UUID_NOT_FOUND:
//         The GPU referred to by pGpuUuid was not found.
//
//     NV_ERR_PAGE_TABLE_NOT_AVAIL:
//         The system requires that the UVM file descriptor be associated with a
//         single process, and that process has exited.
//
//     NV_ERR_INVALID_ARGUMENT:
//         OS state required to register the GPU is not present.
//
//     NV_ERR_OBJECT_NOT_FOUND:
//         OS state required to register the GPU is not present.
//
//     NV_ERR_INVALID_STATE:
//         OS state required to register the GPU is malformed, or the partition
//         identified by the user handles or its configuration changed.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
#if UVM_API_REV_IS_AT_MOST(8)
NV_STATUS UvmRegisterGpu(const NvProcessorUuid *gpuUuid);
#else
NV_STATUS UvmRegisterGpu(const NvProcessorUuid *gpuUuid,
                         const UvmGpuPlatformParams *platformParams);
#endif

#if UVM_API_REV_IS_AT_MOST(8)
//------------------------------------------------------------------------------
// UvmRegisterGpuSmc
//
// The same as UvmRegisterGpu, but takes additional parameters to specify the
// GPU partition being registered if SMC is enabled.
//
// Arguments:
//     gpuUuid: (INPUT)
//         UUID of the physical GPU of the SMC partition to register.
//
//     platformParams: (INPUT)
//         User handles identifying the partition to register.
//
// Error codes (see UvmRegisterGpu also):
//
//     NV_ERR_INVALID_STATE:
//         SMC was not enabled, or the partition identified by the user
//         handles or its configuration changed.
//
NV_STATUS UvmRegisterGpuSmc(const NvProcessorUuid *gpuUuid,
                            const UvmGpuPlatformParams *platformParams);
#endif

//------------------------------------------------------------------------------
// UvmUnregisterGpu
//
// Unregisters a GPU from UVM. If this is the last process to unregister this
// GPU, the UVM driver frees all resources allocated on the GPU when the GPU
// was first registered. Any pages on the GPU allocated by the UVM driver will
// be migrated to CPU memory before the GPU resources are freed.
//
// Any GPU VA spaces or channels that were registered on this GPU using
// UvmRegisterGpuVaSpace or UvmRegisterChannel respectively, will be
// unregistered. Any state that was set by calling UvmSetPreferredLocation or
// UvmSetAccessedBy for this GPU will be cleared. Any pages that were associated
// with a non-migratable range group and had this GPU as their preferred
// location will have their range group association changed to
// UVM_RANGE_GROUP_ID_NONE.
//
// If the Confidential Computing feature is enabled in the system, any VA
// ranges allocated using UvmAllocSemaphorePool and owned by this GPU will be
// unmapped from all GPUs and the CPU. UvmFree must still be called on those
// ranges to reclaim the VA. See UvmAllocSemaphorePool to determine which GPU
// is considered the owner.
//
// Arguments:
//     gpuUuid: (INPUT)
//         UUID of the physical GPU if the GPU is not SMC capable or SMC
//         enabled, or the GPU instance UUID of the partition to unregister.
//
// Error codes:
//     NV_ERR_INVALID_DEVICE:
//         The GPU referred to by pGpuUuid was not registered by this process.
//
//     NV_ERR_GPU_UUID_NOT_FOUND:
//         The GPU referred to by pGpuUuid was not found.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmUnregisterGpu(const NvProcessorUuid *gpuUuid);

//------------------------------------------------------------------------------
// UvmRegisterGpuVaSpace
//
// Registers a GPU's VA (virtual address) space for use with UVM. Only one GPU
// VA space can be registered for a given GPU at a time. Once a VA space has
// been registered for a GPU, all page table updates for that VA space on that
// GPU will be managed by the UVM driver.
//
// The GPU must have been registered using UvmRegisterGpu prior to making this
// call.
//
// On systems with GPUs that support transparent access to pageable memory, this
// feature is enabled per GPU VA space. This setting must match for all
// registered GPU VA spaces.
//
// Any VA ranges that were allocated using UvmAllocSemaphorePool will be mapped
// on this GPU with the mapping and caching attributes as specified during that
// call, or with default attributes if none were specified.
//
// Any VA ranges that had a preferred location set to this GPU will be mapped on
// this GPU only if this GPU is not fault-capable and the VA range belongs to a
// non-migratable range group. If such a mapping cannot be established, an error
// is returned.
//
// Any VA ranges which have accessed-by set for this GPU will be mapped on this
// GPU. If that VA range resides in a PCIe peer GPU's memory and P2P support
// between the two GPUs has not been enabled via UvmEnablePeerAccess, then a
// mapping won't be established. Also, if read duplication is enabled for this
// VA range, or its preferred location is set to this GPU, and this GPU is a
// fault-capable GPU, then a mapping will not be established.  If this is a
// non-fault-capable GPU and a mapping cannot be established, then an error is
// returned.
//
// If P2P support has been enabled between this GPU and another GPU that also
// has a GPU VA space registered, then the two GPU VA spaces must support the
// same set of page sizes for GPU mappings. Otherwise, an error is returned.
//
// Note that all the aforementioned VA ranges must lie within the largest
// possible virtual address supported by this GPU.
//
// Arguments:
//     gpuUuid: (INPUT)
//         UUID of the physical GPU if the GPU is not SMC capable or SMC
//         enabled, or the GPU instance UUID of the partition to register.
//
//     platformParams: (INPUT)
//         On Linux: RM ctrl fd, hClient and hVaSpace.
//
// Error codes:
//     NV_ERR_NO_MEMORY:
//         Internal memory allocation failed.
//
//     NV_ERR_OUT_OF_RANGE:
//         A VA range that needs to be mapped on this GPU exceeds the largest
//         virtual address supported by the GPU.
//
//     NV_ERR_INVALID_DEVICE:
//         The GPU referred to by gpuUuid was not registered or a VA space has
//         already been registered for this GPU. Or this is a non-fault-capable
//         GPU that is present in the accessed-by list of a VA range that
//         resides on another non-fault-capable GPU, and P2P support between
//         both GPUs is not enabled.
//
//     NV_ERR_OTHER_DEVICE_FOUND:
//         The UUID does not match the UUID of the device that is associated
//         with the VA space handles in the platformParams argument.
//
//     NV_ERR_INVALID_FLAGS:
//         The VA space was originally allocated with UVM-incompatible flags.
//         This includes the case in which the value for the setting to enable
//         transparent access to pageable memory for the given GPU VA space does
//         not match the value in previously-registered GPU VA spaces, or that
//         value is set but pageable memory access has been disabled via
//         UvmInitialize.
//
//     NV_ERR_NOT_COMPATIBLE:
//         The GPU referred to by gpuUuid has P2P support enabled with another
//         GPU and the set of page sizes supported by the specified VA space
//         doesn't match that of the VA space registered on the peer GPU.
//
//     NV_ERR_INVALID_ARGUMENT:
//         Some problem with the platform specific arguments was detected.
//
//     NV_ERR_NOT_SUPPORTED:
//         A GPU VA space has already been registered using a different UVM file
//         descriptor in this process and this platform does not support that
//         operation, or a GPU VA space has already been registered on this UVM
//         file descriptor by a different process and this platform does not
//         support that operation.
//
//     NV_ERR_PAGE_TABLE_NOT_AVAIL:
//         The system requires that the UVM file descriptor be associated with a
//         single process, and that process has exited.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmRegisterGpuVaSpace(const NvProcessorUuid             *gpuUuid,
                                const UvmGpuVaSpacePlatformParams *platformParams);

//------------------------------------------------------------------------------
// UvmUnregisterGpuVaSpace
//
// Unregisters the GPU VA space that was previously registered via a call to
// UvmRegisterGpuVaSpace.
//
// Any page table mappings created by UVM on that GPU for that VA space will be
// unmapped. Any channels that were registered on this GPU using
// UvmRegisterChannel will be unregistered.
//
// Arguments:
//     gpuUuid: (INPUT)
//         UUID of the physical GPU if the GPU is not SMC capable or SMC
//         enabled, or the GPU instance UUID of the partition whose VA space
//         should be unregistered.
//
// Error codes:
//     NV_ERR_INVALID_DEVICE:
//         The GPU referred to by gpuUuid was not registered or no VA space has
//         been registered for this GPU.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmUnregisterGpuVaSpace(const NvProcessorUuid *gpuUuid);

//------------------------------------------------------------------------------
// UvmEnablePeerAccess
//
// Enables P2P (peer to peer) support in the UVM driver between two GPUs
// connected via PCIe. NVLink peers are automatically discovered/enabled in the
// driver at UvmRegisterGpu time. Enabling P2P support between two GPUs allows
// peer mappings to be created as part of fault servicing, memory allocation,
// etc. The P2P support is bidirectional i.e. enabling P2P between GPU A and
// GPU B also enables P2P support between GPU B and GPU A.
//
// The two GPUs must have been registered via UvmRegisterGpu prior to making
// this call. An error is returned if P2P support has already been enabled
// between these two GPUs in this process.
//
// The two GPUs must be connected via PCIe. An error is returned if the GPUs are
// not connected or are connected over an interconnect different than PCIe
// (NVLink or SMC partitions, for example).
//
// If both GPUs have GPU VA spaces registered for them, the two GPU VA spaces
// must support the same set of page sizes for GPU mappings.
//
// If any VA range resides in one GPU's memory, and the peer GPU is in the
// accessed-by list of that VA range, then a peer mapping will be established
// unless the VA space for the peer GPU has not been registered, or read
// duplication is enabled for the VA range, or the preferred location of the VA
// range is the peer GPU.
//
// Arguments:
//     gpuUuidA: (INPUT)
//         UUID of the physical GPU if the GPU is not SMC capable or SMC
//         enabled, or the GPU instance UUID of the partition A.
//
//     gpuUuidB: (INPUT)
//         UUID of the physical GPU if the GPU is not SMC capable or SMC
//         enabled, or the GPU instance UUID of the partition B.
//
// Error codes:
//     NV_ERR_NO_MEMORY:
//         Internal memory allocation failed.
//
//     NV_ERR_INVALID_DEVICE:
//         At least one GPU has not been registered, P2P support has already
//         been enabled between the two GPUs, or the GPUs are connected via an
//         interconnect other than PCIe.
//
//     NV_ERR_NOT_SUPPORTED:
//         The two GPUs are not peer capable.
//
//     NV_ERR_NOT_COMPATIBLE:
//         Both GPUs have a GPU VA space registered for them and the two VA
//         spaces don't support the same set of page sizes for GPU mappings.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmEnablePeerAccess(const NvProcessorUuid *gpuUuidA,
                              const NvProcessorUuid *gpuUuidB);

//------------------------------------------------------------------------------
// UvmDisablePeerAccess
//
// Disables P2P (peer to peer) support in the UVM driver between two GPUs.
// connected via PCIe. NVLink peers are automatically disabled in the driver
// at UvmUnregisterGpu time. Disabling P2P support between two GPUs removes all
// existing peer mappings from either GPU to the other, and also prevents new
// peer mappings from being established between the two GPUs.
//
// The two GPUs must be connected via PCIe. An error is returned if the GPUs are
// not connected or are connected over an interconnect different than PCIe
// (NVLink, for example).
//
// If one of the two GPUs is present in the accessed-by list of a non-migratable
// VA range that has a preferred location set to the other GPU, and the two GPUs
// are not fault-capable, then the GPU is removed from the accessed-by list of
// the range.
//
// Arguments:
//     gpuUuidA: (INPUT)
//         UUID of the physical GPU if the GPU is not SMC capable or SMC
//         enabled, or the GPU instance UUID of the partition A.
//
//     gpuUuidB: (INPUT)
//         UUID of the physical GPU if the GPU is not SMC capable or SMC
//         enabled, or the GPU instance UUID of the partition B.
//
// Error codes:
//     NV_ERR_INVALID_DEVICE:
//         At least one GPU has not been registered, or P2P support has not been
//         enabled between the two GPUs, or the GPUs are connected via an
//         interconnect other than PCIe.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmDisablePeerAccess(const NvProcessorUuid *gpuUuidA,
                               const NvProcessorUuid *gpuUuidB);

//------------------------------------------------------------------------------
// UvmRegisterChannel
//
// Register a channel for use with UVM. Any faults that occur on this channel
// will be handled by the UVM driver.
//
// A GPU VA space must have been registered on this GPU via
// UvmRegisterGpuVaSpace prior to making this call.
//
// For channels that require associated mappings, the base and length of a
// virtual address region that was reserved via UvmReserveVa must be supplied to
// this call in order to map those allocations. The size and alignment of this
// region can be obtained by calling the appropriate platform specific API. For
// example, on RM, an RM control call has to be made with the control type as
// NV2080_CTRL_CMD_GR_GET_CTX_BUFFER_SIZE. If no region needs to be reserved for
// this channel, the base and length arguments are ignored.
//
// Using the same VA region for multiple UvmRegisterChannel calls is allowed,
// provided all allocations required by all of those calls fit within the
// region.
//
// Registering the same channel on multiple subdevices of an SLI group is
// disallowed.
//
// On any errors, the channel may be reset, thereby terminating any pending
// work on that channel.
//
// Arguments:
//     gpuUuid: (INPUT)
//        UUID of the physical GPU if the GPU is not SMC capable or SMC
//        enabled, or the GPU instance UUID of the partition that the channel is
//        associated with.
//
//     platformParams: (INPUT)
//         On Linux: RM ctrl fd, hClient and hChannel.
//
//     base: (INPUT)
//         Base address (starting point) of the VA (virtual address) range
//         reserved for mapping the allocations associated with this channel.
//         If this channel does not have associated allocations, this argument
//         is ignored.
//
//     length: (INPUT)
//         Length, in bytes, of the range. If this channel does not have
//         associated allocations, this argument is ignored.
//
// Error codes:
//     NV_ERR_NO_MEMORY:
//         Internal memory allocation failed.
//
//     NV_ERR_OTHER_DEVICE_FOUND:
//         The UUID does not match the UUID of the device that is associated
//         with the channel identifier in the platformParams argument.
//
//     NV_ERR_GPU_INVALID_DEVICE:
//         The GPU referred to by pGpuUuid was not registered or no VA space
//         has been registered for this GPU.
//
//     NV_ERR_INVALID_CHANNEL:
//         The given channel identifier is invalid or has already been
//         registered.
//
//     NV_ERR_INVALID_ADDRESS:
//         The channel has allocations which need to be mapped but the base
//         address is invalid, or the VA range specified by base and length
//         is too small.
//
//     NV_ERR_INVALID_ARGUMENT:
//         Either some problem with the platform-specific arguments was detected
//         or the channel has allocations which need to be mapped but length is
//         invalid.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmRegisterChannel(const NvProcessorUuid          *gpuUuid,
                             const UvmChannelPlatformParams *platformParams,
                             void                           *base,
                             NvLength                        length);

//------------------------------------------------------------------------------
// UvmUnregisterChannel
//
// Unregisters a channel from UVM. The channel must have been previously
// registered via a call to UvmRegisterChannel. The channel will be reset,
// thereby terminating any pending work on that channel.
//
// Since channels may share virtual mappings, a call to UvmUnregisterChannel is
// not guaranteed to unmap the VA range passed into the corresponding
// UvmRegisterChannel call because other still-registered channels may be using
// allocations in that VA range. Only channels which share the same TSG can
// share allocations, so a channel's VA range can only be considered released
// after UvmUnregisterChannel has been called on all channels under that TSG.
//
// Arguments:
//     platformParams: (INPUT)
//         On Linux: RM ctrl fd, hClient and hChannel.
//
// Error codes:
//     NV_ERR_INVALID_CHANNEL:
//         The given channel identifier was not registered.
//
//     NV_ERR_INVALID_ARGUMENT:
//         Some problem with the platform specific arguments was detected.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmUnregisterChannel(const UvmChannelPlatformParams *platformParams);

//------------------------------------------------------------------------------
// UvmReserveVa
//
// Reserves VA space on the CPU for future use. Multiple, non-contiguous VA
// ranges can be reserved via this API.
//
// The starting address for the VA reservation can be either explicitly
// specified or left NULL to let the API implementation select one. When the
// starting address is specified, it must be aligned to the smallest CPU page
// size. When the starting address is not specified, the bounds of the search
// space within which the VA range should be reserved must be specified. The
// specified lower bound of the search space is rounded up to the nearest
// non-zero multiple of the requested alignment. The total size of the search
// space taking into consideration the rounded up lower bound cannot be less
// than the requested length for the VA reservation. The starting address chosen
// by the API implementation is guaranteed to be aligned to the requested
// alignment.
//
// The requested alignment must be either a power of two that is at least the
// smallest CPU page size or left zero to indicate default alignment which is
// the smallest CPU page size.
//
// The length of the VA reservation must be a multiple of the smallest CPU page
// size.
//
// Arguments:
//     base: (INPUT/OUTPUT)
//         Contains the starting address of the VA reservation when the call
//         returns successfully. If *base is NULL when this API is invoked, a VA
//         range that falls within the requested bounds is reserved. Note that
//         the lower bound will be rounded up to the nearest non-zero multiple
//         of the requested alignment. If *base is non-NULL when this API
//         is invoked, then that address is chosen as the starting address of
//         the VA reservation.
//
//     length: (INPUT)
//         Length in bytes of the region. Must be a multiple of the smallest CPU
//         page size.
//
//     minVa: (INPUT)
//         Lower limit for the search space within which the VA range must be
//         reserved. Will be rounded up to the nearest non-zero multiple of the
//         requested alignment. Ignored if *base is non-NULL when the API is
//         invoked.
//
//     maxVa: (INPUT)
//         Upper limit for the search space within which the VA range must be
//         reserved. Ignored if *base is non-NULL when the API is invoked.
//
//     alignment: (INPUT)
//         Alignment required for the starting address of the reservation. Must
//         either be zero to indicate default alignment which is smallest CPU
//         page size or a power of two that is at least the smallest CPU page
//         size. Ignored if *base is non-NULL when the API is invoked.
//
// Error codes:
//     NV_ERR_NO_MEMORY:
//         Either *base is NULL and no suitable VA reservation could be made or
//         some other internal memory allocation failed.
//
//     NV_ERR_UVM_ADDRESS_IN_USE:
//         *base is non-NULL and reserving the VA range at that address failed.
//
//     NV_ERR_INVALID_ADDRESS:
//         One of the following occurred:
//         - base is NULL.
//         - *base is non-NULL and is not aligned to the smallest CPU page size.
//         - *base is NULL and one of the following occurred:
//             - the rounded up minVa is not less than maxVa.
//             - the region covered by the rounded up minVa and maxVa is not big
//               enough to contain a VA reservation of the requested length.
//         - alignment is non-zero and is either not a power of two or is less
//           than the smallest CPU size.
//         - length is zero or is not a multiple of the smallest CPU page size.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmReserveVa(void     **base,
                       NvLength   length,
                       void      *minVa,
                       void      *maxVa,
                       NvLength   alignment);

//------------------------------------------------------------------------------
// UvmReleaseVa
//
// Releases all pages within the VA range. If any of the pages were committed,
// they are automatically decomitted as well.
//
// The release may encompass more than a single reserve VA or commit call, but
// must not partially release any regions that were either reserved or
// committed previously.
//
// Arguments:
//     base: (INPUT)
//         Base address (starting point) of the VA (virtual address) range.
//
//     length: (INPUT)
//         Length, in bytes, of the range.
//
// Error codes:
//     NV_ERR_NO_MEMORY:
//         Internal memory allocation failed. There is likely more than one
//         possible cause of this error.
//
//     NV_ERR_INVALID_ADDRESS:
//         base and length are not properly aligned or the range was not
//         previously reserved via UvmReserveVa.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmReleaseVa(void     *base,
                       NvLength  length);

//------------------------------------------------------------------------------
// UvmCreateRangeGroup
//
// Creates a new range group. Virtual address ranges can be associated with
// this range group as outlined in UvmSetRangeGroup.
//
// Arguments:
//     rangeGroupId: (OUTPUT)
//         Id of the newly created range group.
//
// Error codes:
//     NV_ERR_NO_MEMORY:
//         Internal memory allocation failed.
//
//     NV_ERR_INVALID_ARGUMENT:
//         A NULL pointer was passed in the rangeGroupId argument.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmCreateRangeGroup(NvU64 *rangeGroupId);

//------------------------------------------------------------------------------
// UvmDestroyRangeGroup
//
// Destroys a previously created range group. If there are any pages associated
// with this range group, that association is cleared. i.e. the behavior is the
// same as associating those pages with UVM_RANGE_GROUP_ID_NONE via a call to
// UvmSetRangeGroup.
//
// Arguments:
//     rangeGroupId: (INPUT)
//         Id of the range group to be destroyed.
//
// Error codes:
//     NV_ERR_OBJECT_NOT_FOUND:
//         rangeGroupId was not created by a previous call to
//         UvmCreateRangeGroup.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmDestroyRangeGroup(NvU64 rangeGroupId);

//------------------------------------------------------------------------------
// UvmSetRangeGroup
//
// Associates the pages in a virtual address (VA) range with the specified
// range group. The base address and length of the VA range must be aligned to
// the smallest page size supported by the CPU. If any pages in that VA range
// were associated with another range group, that association is changed to
// this range group. The VA range must have been allocated via either UvmAlloc
// or UvmMemMap.
//
// If the range group was made non-migratable by a previous call to
// UvmPreventMigrationRangeGroups, then all pages in the VA range are migrated
// to their preferred location if they are not already located there. If any
// page does not have a preferred location or if the preferred location is a
// fault-capable GPU, an error is returned.
//
// If rangeGroupId is UVM_RANGE_GROUP_ID_NONE, then all pages in the VA range
// will have their range group association removed.
//
// Arguments:
//     base: (INPUT)
//         Base address of the virtual address range.
//
//     length: (INPUT)
//         Length, in bytes, of the range.
//
//     rangeGroupId: (INPUT)
//         Id of the range group to associate the VA range with.
//
// Errors:
//     NV_ERR_NO_MEMORY:
//         Internal memory allocation failed.
//
//     NV_ERR_INVALID_ADDRESS:
//         base and length are not properly aligned or don't represent a valid
//         address range.
//
//     NV_ERR_INVALID_DEVICE:
//         The range group is non-migratable and at least one page in the VA
//         range either does not have a preferred location or its preferred
//         location is a fault-capable GPU.
//
//     NV_ERR_OBJECT_NOT_FOUND:
//         rangeGroupId was not created by a previous call to
//         UvmCreateRangeGroup.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmSetRangeGroup(void     *base,
                           NvLength  length,
                           NvU64     rangeGroupId);

//------------------------------------------------------------------------------
// UvmPreventMigrationRangeGroups
//
// Migrates all pages associated with the specified range groups to their
// preferred location and prevents them from being migrated on faults from
// either the CPU or the GPU. Any unpopulated pages are populated at the
// preferred location. If any page does not have a preferred location or if the
// preferred location is a fault-capable GPU, an error is returned. All the
// specified range groups must be valid range groups allocated using
// UvmCreateRangeGroup.
//
// All pages associated with the specified range groups are mapped at the
// preferred location and from all the GPUs present in the accessed-by list of
// those pages, provided establishing a mapping is possible. If any page
// associated with any of the specified range groups has a preferred location
// set to a non-fault-capable GPU, and another non-fault-capable GPU is in the
// accessed-by list of the page but P2P support between both GPUs is not
// enabled, an error is returned.
//
// GPUs are allowed to map any pages belonging to these range groups on faults.
// If establishing such a mapping is not possible, the fault is fatal.
//
// Existing CPU mappings to any pages belonging to these range groups are
// revoked, even if the pages are in system memory and even if the CPU is in
// the accessed-by list of those pages. The CPU is not allowed to map these
// pages on faults even if they are located in system memory and so, CPU faults
// to these pages are always fatal.
//
// Multiple calls to UvmPreventMigrationRangeGroups are not refcounted. i.e.
// calling UvmPreventMigrationRangeGroups on a range group on which
// UvmPreventMigrationRangeGroups has already been called results in a no-op.
//
// Arguments:
//     rangeGroupIds: (INPUT)
//         An array of range group IDs.
//
//     numGroupIds: (INPUT)
//         Number of items in the rangeGroupIds array.
//
// Errors:
//     NV_ERR_NO_MEMORY:
//         Internal memory allocation failed.
//
//     NV_ERR_OBJECT_NOT_FOUND:
//         One or more rangeGroupIds was not found.
//
//     NV_ERR_INVALID_ARGUMENT:
//         A NULL pointer was passed in for rangeGroupIds or numGroupIds was
//         zero.
//
//     NV_ERR_INVALID_DEVICE:
//         At least one page in one of the VA ranges associated with these range
//         groups does not have a preferred location or its preferred location
//         is a fault-capable GPU. Or the preferred location has been set to a
//         non-fault-capable GPU, and another non-fault-capable GPU is present
//         in the accessed-by list of a page but P2P support between both GPUs
//         has not been enabled.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmPreventMigrationRangeGroups(const NvU64 *rangeGroupIds,
                                         NvLength     numGroupIds);

//------------------------------------------------------------------------------
// UvmAllowMigrationRangeGroups
//
// Undoes the effect of UvmPreventMigrationRangeGroups. Pages associated with
// these range groups are now allowed to migrate at any time, and CPU or GPU
// faults to these pages are no longer fatal. All the specified range groups
// must be valid range groups allocated using UvmCreateRangeGroup.
//
// Multiple calls to UvmAllowMigrationRangeGroups are not refcounted. i.e.
// calling UvmAllowMigrationRangeGroups on a range group on which
// UvmAllowMigrationRangeGroups has already been called results in a no-op.
//
// Arguments:
//     rangeGroupIds: (INPUT)
//         An array of range group IDs.
//
//     numGroupIds: (INPUT)
//         Number of items in the rangeGroupIds array.
//
// Errors:
//     NV_ERR_OBJECT_NOT_FOUND:
//         One or more rangeGroupIds was not found.
//
//     NV_ERR_INVALID_ARGUMENT:
//         A NULL pointer was passed in for rangeGroupIds or numGroupIds was
//         zero.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmAllowMigrationRangeGroups(const NvU64 *rangeGroupIds,
                                       NvLength     numGroupIds);

//------------------------------------------------------------------------------
// UvmAlloc
//
// Creates a new mapping in the virtual address space of the process, populates
// it at the specified preferred location, maps it on the provided list of
// processors if feasible and associates the range with the given range group.
// If the preferredLocationUuid is the UUID of the CPU, preferred location is
// set to all CPU nodes allowed by the global and thread memory policies.
//
// This API is equivalent to the following code sequence:
//     UvmMemMap(base, length);
//     UvmSetPreferredLocation(base, length, preferredLocationUuid, -1);
//     for (i = 0; i < accessedByCount; i++) {
//         UvmSetAccessedBy(base, length, &accessedByUuids[i]);
//     }
//     UvmSetRangeGroup(base, length, rangeGroupId);
//     UvmMigrate(base, length, preferredLocationUuid, 0);
//
// Please see those APIs for further details on their behavior. If an error is
// encountered during any part of the sequence, the completed portion will be
// undone.
//
// The VA range can be unmapped and freed via a call to UvmFree.
//
// Arguments:
//     base: (INPUT)
//         Base address of the virtual address range.
//
//     length: (INPUT)
//         Length, in bytes, of the range.
//
//     preferredLocationUuid: (INPUT)
//         UUID of the CPU, UUID of the physical GPU if the GPU is not SMC
//         capable or SMC enabled, or the GPU instance UUID of the partition of
//         the preferred location for this VA range.
//
//     accessedByUuids: (INPUT)
//         UUID of the CPU, UUID of the physical GPUs if the GPUs are not SMC
//         capable or SMC enabled, or the GPU instance UUID of the partitions
//         that should have persistent mappings to this VA range.
//
//     accessedByCount: (INPUT)
//         Number of elements in the accessedByUuids array.
//
//     rangeGroupId: (INPUT)
//         ID of the range group to associate this VA range with.
//
// Errors:
//     NV_ERR_UVM_ADDRESS_IN_USE:
//         The requested address range overlaps with an existing allocation.
//
//     NV_ERR_INVALID_ADDRESS:
//         base and length are not properly aligned or the range was not
//         previously reserved via UvmReserveVa.
//
//     NV_ERR_INVALID_DEVICE:
//         Either preferredLocationUuid or one of the UUIDs in the
//         accessedByUuids array was not registered or the UUID represents a GPU
//         that has no VA space registered for it.
//
//     NV_ERR_OBJECT_NOT_FOUND:
//         rangeGroupId was not found.
//
//     NV_ERR_NO_MEMORY:
//         Internal memory allocation failed.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmAlloc(void                  *base,
                   NvLength               length,
                   const NvProcessorUuid *preferredLocationUuid,
                   const NvProcessorUuid *accessedByUuids,
                   NvLength               accessedByCount,
                   NvU64                  rangeGroupId);

//------------------------------------------------------------------------------
// UvmFree
//
// Frees a VA range previously allocated via one of the UVM allocator APIs,
// namely either UvmAlloc, UvmMemMap, UvmCreateExternalRange,
// UvmMapDynamicParallelismRegion or UvmAllocSemaphorePool.
//
// For VA ranges allocated via UvmAlloc, UvmMemMap or UvmAllocSemaphorePool, all
// CPU and GPU page table mappings are cleared and all allocated pages are
// freed.
//
// For VA ranges allocated via UvmCreateExternalRange, all GPU page table
// mappings are cleared. No CPU page table mappings for this range are affected,
// and no physical pages for this range are freed.
//
// For VA ranges allocated via UvmMapDynamicParallelismRegion, all GPU page
// table mappings are cleared. No CPU page table mappings for this range are
// affected.
//
// The base address of the VA range to be freed must match the base address used
// when allocating the range. If the VA range came from a region previously
// reserved via UvmReserveVa, then this VA range is put back in the reserved
// state.
//
// Note that the reason this API does not take a length argument is because this
// API is modeled after the C library free() API. Partial frees are not allowed
// and the UVM usermode layer tracks the base and length of each allocated
// range, so having a length argument would be redundant. This also eliminates
// the need for the caller to track the length of each allocation.
//
// Arguments:
//     base: (INPUT)
//         Starting address of the range to be freed. This must be match an
//         address that was obtained via a UVM allocator API.
//
// Errors:
//     NV_ERR_INVALID_ADDRESS:
//         base does not match an address that was passed into a UVM allocator
//         API.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmFree(void *base);

//------------------------------------------------------------------------------
// UvmCleanUpZombieResources
//
// Clean up resources left by processes that specify
// UVM_INIT_FLAGS_MULTI_PROCESS_SHARING_MODE. Resources not freed before
// termination by such processes are not immediately freed by UVM if another
// processes is using the same UVM file.
//
// Errors:
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmCleanUpZombieResources(void);

//------------------------------------------------------------------------------
// UvmAllocSemaphorePool
//
// Allocates memory from which semaphores can be suballocated and used to order
// work between UVM and CUDA as described in UvmMigrateAsync.
//
// The virtual address range specified by (base, length) must have been
// previously reserved via a call to UvmReserveVa. Both base and length must be
// aligned to the smallest page size supported by the CPU.
//
// The pages are populated in CPU memory and zero initialized. They are mapped
// on the CPU and in all registered GPU VA spaces. They will also be mapped in
// any GPU VA spaces registered after this call. The pages are non-migratable
// and the GPU mappings are persistent, which makes them safe to access from
// non-fault-capable HW engines.
//
// By default, all mappings to this VA range have read, write and atomic access
// and are uncached. This behavior can be overridden for GPUs by explicitly
// specifying the mapping and caching attributes through this API. At most one
// GPU may cache the allocation, in which case no other processor should write
// to it. These GPUs must have been registered via UvmRegisterGpu. These GPUs
// do not need to have a GPU VA space registered at the time of this API call.
// Overriding default mapping and caching attributes for the CPU is disallowed.
// If a new GPU is registered or a currently registered GPU is unregistered via
// UvmUnregisterGpu and then re-registered, default mapping and caching
// attributes will be applied for that GPU.
//
// The VA range must lie within the largest possible virtual address supported
// by all GPUs that currently have a GPU VA space registered for them. Also, if
// a GPU VA space is registered in the future for a GPU which is unable to map
// this allocation, that GPU VA space registration will fail.
//
// The pages in this VA range cannot be associated with range groups, cannot be
// the target for read duplication, cannot have a preferred location set, and
// cannot have any accessed-by processors.
//
// The VA range can be unmapped and freed via a call to UvmFree.
//
// If the Confidential Computing feature is enabled in the system, at least one
// GPU must be provided in the perGpuAttribs array. The first GPU in the array
// is considered the owning GPU. If the owning GPU is unregistered via
// UvmUnregisterGpu, this allocation will no longer be usable.
// See UvmUnregisterGpu.
//
// Arguments:
//     base: (INPUT)
//         Base address of the virtual address range.
//
//     length: (INPUT)
//         Length, in bytes, of the range.
//
//     perGpuAttribs: (INPUT)
//         List of per GPU mapping and caching attributes. GPUs not in the list
//         are mapped with default attributes.
//
//     gpuAttribsCount: (INPUT)
//         Number of entries in the perGpuAttribs array.
//
// Errors:
//     NV_ERR_UVM_ADDRESS_IN_USE:
//         The requested address range overlaps with an existing allocation.
//
//     NV_ERR_INVALID_ADDRESS:
//         base and length are not properly aligned or the range was not
//         previously reserved via UvmReserveVa.
//
//     NV_ERR_OUT_OF_RANGE:
//         The VA range exceeds the largest virtual address supported by one or
//         more registered GPUs.
//
//     NV_ERR_INVALID_DEVICE:
//         At least one of the UUIDs in the perGpuAttribs list was either not
//         registered or is the UUID of the CPU.
//
//     NV_ERR_NO_MEMORY:
//         Internal memory allocation failed.
//
//     NV_ERR_INVALID_ARGUMENT:
//         perGpuAttribs is NULL but gpuAttribsCount is non-zero or vice-versa,
//         or caching is requested on more than one GPU.
//         The Confidential Computing feature is enabled and the perGpuAttribs
//         list is empty.
//
//     NV_ERR_NOT_SUPPORTED:
//         The current process is not the one which called UvmInitialize, and
//         UVM_INIT_FLAGS_MULTI_PROCESS_SHARING_MODE was not specified to
//         UvmInitialize.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmAllocSemaphorePool(void                          *base,
                                NvLength                       length,
                                const UvmGpuMappingAttributes *perGpuAttribs,
                                NvLength                       gpuAttribsCount);

//------------------------------------------------------------------------------
// UvmMigrate
//
// Migrates the backing of a given virtual address range to the specified
// destination processor. If any page in the VA range is unpopulated, it is
// populated at the destination processor. The migrated pages in the VA range
// are also mapped on the destination processor.
//
// Both base and length must be aligned to the smallest page size supported by
// the CPU. The VA range must lie within the largest possible virtual address
// supported by the specified processor.
//
// The virtual address range specified by (base, length) must have been
// allocated via a call to either UvmAlloc or UvmMemMap, or be supported
// system-allocated pageable memory.
//
// If the input virtual range corresponds to system-allocated pageable memory,
// and there is at least one GPU in the system that supports transparent access
// to pageable memory, the behavior described in the next paragraphs does not
// take effect. Instead, the driver will first populate any unpopulated pages
// according to the memory policy defined by the calling process and address
// range. Then, pages will be migrated to the requested processor. If the
// destination processor is the CPU, and the memory policy has not defined
// preferred CPU memory nodes or the given preferredCpuMemoryNode is in the
// mask of preferred memory nodes, the driver will try to migrate memory to
// preferredCpuMemoryNode first, and will fallback to the rest of CPU the nodes
// if it doesn't succeed. If pages were already resident on any CPU memory node,
// they will not be migrated.
//
// If the input virtual range corresponds to system-allocated pageable memory,
// and UvmIsPageableMemoryAccessSupported reports that pageable memory access
// is supported, then the driver will populate any unpopulated pages at the
// destination processor and migrate the data from any source location to the
// destination. Pages in the VA range are migrated even if their preferred
// location is set to a processor other than the destination processor.
// If the accessed-by list of any of the pages in the VA range is not empty,
// then mappings to those pages from all the appropriate processors are updated
// to refer to the new location if establishing such a mapping is possible.
// Otherwise, those mappings are cleared.
// Note that in this case, software managed pageable memory does not support
// migration of MAP_SHARED, file-backed, or PROT_NONE mappings.
//
// If any pages in the given VA range are associated with a range group which
// has been made non-migratable via UvmPreventMigrationRangeGroups, then those
// pages are not migrated and the mappings on the destination processor for
// those pages are left unmodified. If the VA range is associated with a
// migratable range group and the destination processor is a non-fault-capable
// GPU, then an error is returned if that GPU is in the accessed-by list of the
// VA range but that GPU is not the preferred location.
//
// If read duplication is enabled on any pages in the VA range, then those pages
// are read duplicated at the destination processor, leaving the source copy, if
// present, intact with only its mapping changed to read-only if it wasn't
// already mapped that way.
//
// Pages in the VA range are migrated even if their preferred location is set to
// a processor other than the destination processor.
//
// If the accessed-by list of any of the pages in the VA range is not empty,
// then mappings to those pages from all the appropriate processors are updated
// to refer to the new location if establishing such a mapping is possible.
// Otherwise, those mappings are cleared.
//
// If fewer than the number of requested pages were migrated,
// NV_WARN_MORE_PROCESSING_REQUIRED is returned. An example scenario where this
// could happen is when UvmPreventMigrationRangeGroups has been called on a
// range group associated with some pages in this range. If fewer than the
// number of requested pages were migrated due to insufficient memory to
// allocate physical pages or page tables, then NV_ERR_NO_MEMORY is returned.
//
// Arguments:
//     base: (INPUT)
//         Base address of the virtual address range.
//
//     length: (INPUT)
//         Length, in bytes, of the range.
//
//     destinationUuid: (INPUT)
//         UUID of the physical GPU if the GPU is not SMC capable or SMC
//         enabled, the GPU instance UUID of the partition, or the CPU UUID to
//         migrate pages to.
//
//     preferredCpuMemoryNode: (INPUT)
//         Preferred CPU NUMA memory node used if the destination processor is
//         the CPU. -1 indicates no preference, in which case the pages used
//         can be on any of the available CPU NUMA nodes. If NUMA is disabled
//         only 0 and -1 are allowed.
//
// Error codes:
//     NV_ERR_INVALID_ADDRESS:
//         base and length are not properly aligned, or the range does not
//         represent a migratable allocation created via UvmMemMap, or the
//         range is pageable memory and the system does not support accessing
//         pageable memory, or the range does not represent a supported
//         Operating System allocation.
//
//     NV_ERR_OUT_OF_RANGE:
//         The VA range exceeds the largest virtual address supported by the
//         destination processor.
//
//     NV_ERR_INVALID_ARGUMENT:
//         preferredCpuMemoryNode is not a valid CPU NUMA node or it corresponds
//         to a NUMA node ID for a registered GPU. If NUMA is disabled, it
//         indicates that preferredCpuMemoryNode was not either 0 or -1.
//
//     NV_ERR_INVALID_DEVICE:
//         destinationUuid does not represent a valid processor such as a CPU or
//         a GPU with a GPU VA space registered for it. Or destinationUuid is a
//         non-fault-capable GPU, and that GPU is present in the accessed-by
//         list of the VA range but that GPU is not the preferred location.
//
//     NV_ERR_NO_MEMORY:
//         There was insufficient memory to allocate physical pages or page
//         tables to complete the migration. Or internal memory allocation
//         failed.
//
//     NV_ERR_NOT_SUPPORTED:
//         The UVM file descriptor is associated with another process and the
//         input virtual range corresponds to system-allocated pageable memory
//         that cannot be migrated from this process.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//     NV_WARN_MORE_PROCESSING_REQUIRED:
//         Fewer than the number of requested pages were migrated because some
//         pages were associated with a non-migratable range group.
//
//------------------------------------------------------------------------------
NV_STATUS UvmMigrate(void                  *base,
                     NvLength               length,
                     const NvProcessorUuid *destinationUuid,
                     NvS32                  preferredCpuMemoryNode);

//------------------------------------------------------------------------------
// UvmMigrateAsync
//
// Migrates the backing of a given virtual address range to the specified
// destination processor. The behavior of this API is exactly the same as that
// of UvmMigrate except for the differences outlined below.
//
// When this call returns NV_OK, the migration operation is considered to be
// in-flight and can be synchronized upon by waiting for the specified payload
// to be written at the given semaphore address. The semaphore address must be
// 4-byte aligned and must fall within a VA range allocated using
// UvmAllocSemaphorePool. It is up to the caller to ensure that the payload has
// been written before reusing the address in a subsequent UvmMigrateAsync call.
// Specifying a semaphore address is optional. If the semaphore address is NULL
// the payload must be zero.
//
// The API makes no guarantees about how many pages will be migrated, and there
// is no provision to detect errors that occur during the in-flight operations.
// However, the API does guarantee that the semaphore will eventually be
// released regardless of errors during in-flight operations, as long as the API
// call itself returned NV_OK.
//
// Arguments:
//     base: (INPUT)
//         Base address of the virtual address range.
//
//     length: (INPUT)
//         Length, in bytes, of the range.
//
//     destinationUuid: (INPUT)
//         UUID of the physical GPU if the GPU is not SMC capable or SMC
//         enabled, the GPU instance UUID of the partition, or the CPU UUID to
//         migrate pages to.
//
//     preferredCpuMemoryNode: (INPUT)
//         Preferred CPU NUMA memory node used if the destination processor is
//         the CPU. -1 indicates no preference, in which case the pages used
//         can be on any of the available CPU NUMA nodes. If NUMA is disabled
//         only 0 and -1 are allowed.
//
//     semaphoreAddress: (INPUT)
//         Base address of the semaphore.
//
//     semaphorePayload: (INPUT)
//         Payload to be written at semaphoreAddress when the operation
//         completes. Must be zero if semaphoreAddress is NULL.
//
// Error codes:
//     NV_ERR_INVALID_ADDRESS:
//         base and length are not properly aligned, or the range does not
//         represent a migratable allocation created via UvmMemMap, or the
//         range is pageable memory and the system does not support accessing
//         pageable memory, or the range does not represent a supported
//         Operating System allocation, or the semaphoreAddress isn't properly
//         aligned, or isn't suballocated from a semaphore pool.
//
//     NV_ERR_OUT_OF_RANGE:
//         The VA range exceeds the largest virtual address supported by the
//         destination processor.
//
//     NV_ERR_INVALID_DEVICE:
//         destinationUuid does not represent a valid processor such as a CPU or
//         a GPU with a GPU VA space registered for it. Or destinationUuid is a
//         non-fault-capable GPU, and that GPU is present in the accessed-by
//         list of the VA range but that GPU is not the preferred location.
//
//     NV_ERR_INVALID_ARGUMENT:
//         semaphoreAddress is NULL and semaphorePayload is not zero.
//
//     NV_ERR_NO_MEMORY:
//         There was insufficient memory to allocate physical pages or page
//         tables to complete the migration. Or internal memory allocation
//         failed.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//     NV_WARN_MORE_PROCESSING_REQUIRED:
//         Fewer than the number of requested pages were migrated because some
//         pages were associated with a non-migratable range group.
//
//------------------------------------------------------------------------------
NV_STATUS UvmMigrateAsync(void                  *base,
                          NvLength               length,
                          const NvProcessorUuid *destinationUuid,
                          NvS32                  preferredCpuMemoryNode,
                          void                  *semaphoreAddress,
                          NvU32                  semaphorePayload);

//------------------------------------------------------------------------------
// UvmMigrateRangeGroup
//
// Migrates the backing of all virtual address ranges associated with the given
// range group to the specified destination processor. The behavior of this API
// is equivalent to calling UvmMigrate with preferredCpuMemoryNode = -1 on each
// VA range associated with this range group.
//
// Any errors encountered during migration are returned immediately. No attempt
// is made to migrate the remaining unmigrated ranges and the ranges that are
// already migrated are not rolled back to their previous location.
//
// The range group id specified must have been allocated via
// UvmCreateRangeGroup.
//
// Arguments:
//     rangeGroupId: (INPUT)
//         Id of the range group whose associated VA ranges have to be migrated.
//
//     destinationUuid: (INPUT)
//         UUID of the physical GPU if the GPU is not SMC capable or SMC
//         enabled, the GPU instance UUID of the partition, or the CPU UUID to
//         migrate pages to.
//
// Error codes:
//     NV_ERR_OBJECT_NOT_FOUND:
//         Either UVM_RANGE_GROUP_ID_NONE was specified or the rangeGroupId was
//         not found.
//
//     NV_ERR_INVALID_DEVICE:
//         destinationUuid does not represent a valid processor such as a CPU or
//         a GPU with a GPU VA space registered for it.
//
//     NV_ERR_NO_MEMORY:
//         Internal memory allocation failed.
//
//     NV_ERR_OUT_OF_RANGE:
//         One or more of the VA ranges exceeds the largest virtual address
//         supported by the destination processor.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//     NV_WARN_MORE_PROCESSING_REQUIRED:
//         Fewer than requested pages were migrated because for example, the
//         range group was non-migratable.
//
//------------------------------------------------------------------------------
NV_STATUS UvmMigrateRangeGroup(NvU64                  rangeGroupId,
                               const NvProcessorUuid *destinationUuid);

//------------------------------------------------------------------------------
// UvmPopulatePageable
//
// Forces the population of the given virtual address range. Memory will be
// populated by the system according to the memory policy defined by the calling
// process and address range.
//
// This function only supports pageable memory. None of the pages within the
// virtual address range specified by (base, length) may belong to a virtual
// address range allocated or registered using any of the UVM
// allocation/mapping APIs. Also, all pages must be mapped with at least read
// permissions.
//
// If fewer than the number of requested pages were populated, NV_ERR_NO_MEMORY
// is returned.
//
// Arguments:
//     base: (INPUT)
//         Base address of the virtual address range.
//
//     length: (INPUT)
//         Length, in bytes, of the range.
//
// Errors:
//     NV_ERR_INVALID_ADDRESS:
//         base and length are not properly aligned, the range does not
//         represent a supported Operating System allocation, or the range
//         contains pages not mapped with at least read permissions.
//
//     NV_ERR_NO_MEMORY:
//         Fewer than the number of requested pages were populated, likely
//         because the system ran out of memory.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmPopulatePageable(void     *base,
                              NvLength  length);

//------------------------------------------------------------------------------
// UvmMemMap
//
// Creates a new mapping in the virtual address space of the process that is
// valid for access from any fault-capable CPU or GPU.
//
// The virtual address range specified by (base, length) must have been
// previously reserved via a call to UvmReserveVa. Both base and length must be
// aligned to the smallest page size supported by the CPU. Note that using a
// larger alignment for base and length, such as the largest GPU page size, may
// result in higher performance.
//
// The pages in the VA range are zero initialized. They are typically populated
// on demand, for example, through CPU or GPU faults.
//
// The VA range can be unmapped and freed via a call to UvmFree.
//
// Arguments:
//     base: (INPUT)
//         Base address of the virtual address range.
//
//     length: (INPUT)
//         Length, in bytes, of the range.
//
// Errors:
//     NV_ERR_UVM_ADDRESS_IN_USE:
//         The requested address range overlaps with an existing allocation.
//
//     NV_ERR_INVALID_ADDRESS:
//         base and length are not properly aligned or the range was not
//         previously reserved via UvmReserveVa.
//
//     NV_ERR_NOT_SUPPORTED:
//         The current process is not the one which called UvmInitialize, and
//         UVM_INIT_FLAGS_MULTI_PROCESS_SHARING_MODE was not specified to
//         UvmInitialize.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmMemMap(void     *base,
                    NvLength  length);

//------------------------------------------------------------------------------
// UvmCreateExternalRange
//
// Create a VA range within the process's address space reserved for external
// allocations. The VA range is not mapped to any physical allocation at the
// time of creation. Once an external VA range has been created using this API,
// the user is free to map any number of physical allocations within the VA
// range (see UvmMapExternalAllocation and UvmMapExternalSparse for more
// details).
//
// The virtual address range, itself, does not impose any restrictions on the
// alignment of the physical allocations mapped within it. However, both base
// and length must be aligned to 4K.
//
// The VA range must not overlap with an existing VA range, irrespective of
// whether the existing range corresponds to a UVM allocation or an external
// allocation.
//
// It is allowed (but not required) for the VA range to come from a region
// previously reserved via UvmReserveVa.
//
// Any mappings created within this VA range are considered non-migratable.
// Consequently, pages cannot be associated with range groups, cannot be
// the target for read duplication, cannot have a preferred location set,
// cannot have any accessed-by processors, and any GPU faults within this range
// are fatal.
//
// Mappings within this range neither create nor modify any CPU mappings, even
// if the mappings came from a region previously reserved via UvmReserveVa.
// This implies that CPU accesses to any mappings within this range will cause
// a fatal fault if it's not mapped.
//
// The VA range is not reclaimed until UvmFree is called on it even if it is
// fully unmapped from all GPUs either explicitly via UvmUnmapExternal or
// implicitly via APIs such as UvmUnregisterGpu, UvmUnregisterGpuVaSpace,
// UvmDisablePeerAccess, etc.
//
// Arguments:
//     base: (INPUT)
//         Base address of the virtual address range.
//
//     length: (INPUT)
//         Length, in bytes, of the range.
//
// Errors:
//     NV_ERR_INVALID_ADDRESS:
//         base is NULL or length is zero or at least one of base and length is
//         not aligned to 4K.
//
//     NV_ERR_UVM_ADDRESS_IN_USE:
//         The requested address range overlaps with an existing allocation.
//
//     NV_ERR_NO_MEMORY:
//         Internal memory allocation failed.
//
//------------------------------------------------------------------------------
NV_STATUS UvmCreateExternalRange(void     *base,
                                 NvLength  length);

//------------------------------------------------------------------------------
// UvmMapExternalAllocation
//
// Maps an allocation that was allocated outside of UVM on the specified list of
// GPUs. The external allocation can be unmapped from a specific GPU using
// UvmUnmapExternal or from all GPUs using UvmFree.
//
// The virtual address range specified by (base, length) must fall within a VA
// range previously created with UvmCreateExternalRange. A GPU VA space must
// have been registered for each GPU in the list. The (base, length) range must
// lie within the largest possible virtual address supported by the specified
// GPUs.
//
// The page size used for the mapping is the largest supported page size less
// than or equal to the alignments of base, length, offset, and the allocation
// page size.
//
// If the range specified by (base, length) falls within any existing mappings,
// the behavior is the same as if UvmUnmapExternal with the range specified by
// (base, length) had been called first, provided that base and length are
// aligned to the page size used for the existing one.
//
// If the allocation resides in GPU memory, that GPU must have been registered
// via UvmRegisterGpu. If the allocation resides in GPU memory and a mapping is
// requested for a different GPU, then P2P support should have been enabled via
// UvmEnablePeerAccess between the two GPUs if connected by PCIe.
//
// The allocation can be mapped with different access permissions and
// cacheability settings on different GPUs. The settings to use for each GPU are
// specified in the perGpuAttribs array. It is also legal to map the allocation
// multiple times on the same GPU with different access permissions and
// cacheability settings as long as all of the mappings are fully contained
// within the VA range. Calling this API with the same GPU appearing multiple
// times in the list is equivalent to calling the API multiple times on the same
// GPU.
//
// Access permissions control which of 3 types of accesses (reads, writes and
// atomics) are allowed for this VA range. Any GPU accesses of a disallowed kind
// result in a fatal fault. If UvmGpuMappingTypeDefault is specified, the UVM
// driver chooses the appropriate access permissions. On non-fault-capable GPUs,
// specifying either UvmGpuMappingTypeReadOnly or UvmGpuMappingTypeReadWrite is
// disallowed.
//
// Caching can be forced on or off, or can be left to the UVM driver to manage
// by specifying UvmGpuCachingTypeDefault. Specifying UvmGpuCachingTypeDefault
// will result in a cached mapping only if the allocation is physically located
// in that GPU's memory. Note that caching here only refers to GPU L2 caching
// and not GPU L1 caching as the latter is controlled via instruction opcode
// modifiers and not through page table attributes.
//
// Format and element bits can be forced, or can be left to the UVM driver to
// manage by specifying UvmGpuFormatTypeDefault and
// UvmGpuFormatElementBitsDefault respectively. UvmGpuFormatTypeDefault and
// UvmGpuFormatElementBitsDefault are mutually inclusive, meaning that if one
// of them is specified then the other one must be specified too.
//
// Compression type of the specified virtual address range can be specified with
// UvmGpuCompressionType mapping attribute.
//
// The UVM driver retains a reference on the external allocation as long as at
// least one GPU has any portion of that allocation mapped.
//
// The pages in this mapping are not zero initialized or modified in any way.
//
// Note that calling UvmUnregisterGpuVaSpace will also unmap all mappings
// created via this API on the GPU that the GPU VA space is associated with.
// Also, if a mapping has to be created on a GPU for a physical allocation that
// resides on a PCIe peer GPU, then peer-to-peer support must have been enabled
// between those two GPUs via UvmEnablePeerAccess. Disabling peer-to-peer
// support via UvmDisablePeerAccess will tear down all peer mappings between the
// two GPUs.
//
// Arguments:
//     base: (INPUT)
//         Base address of the virtual address range.
//
//     length: (INPUT)
//         Length, in bytes, of the range.
//
//     offset: (INPUT)
//         Offset, in bytes, in the physical allocation at which the VA range
//         must be mapped.
//
//     perGpuAttribs: (INPUT)
//         List of per GPU mapping and caching attributes. GPUs not in the list
//         are not affected.
//
//     gpuAttribsCount: (INPUT)
//         Number of entries in the perGpuAttribs array.
//
//     platformParams: (INPUT)
//         Platform specific parameters that identify the allocation.
//         On Linux: RM ctrl fd, hClient and hMemory.
//
// Errors:
//     NV_ERR_INVALID_ADDRESS:
//         One of the following occurred:
//         - base is NULL.
//         - length is zero.
//         - The requested address range does not fall entirely within an
//           existing external VA range created with a single call to
//           UvmCreateExternalRange.
//         - The mapping page size allowed by the alignments of base, length,
//           and offset is smaller than the minimum supported page size on the
//           GPU.
//         - base or base + length fall within an existing mapping but are not
//           aligned to that mapping's page size.
//
//     NV_ERR_OUT_OF_RANGE:
//         The range specified by (base, length) exceeds the largest virtual
//         address supported by one or more of the specified GPUs.
//
//     NV_ERR_INVALID_OFFSET:
//         - offset+length exceeds the allocation size.
//
//     NV_ERR_INVALID_DEVICE:
//         One of the following occurred:
//         - The allocation resides in GPU memory whose UUID was not registered.
//         - One or more of the UUIDs in the perGpuAttribs list was either not
//           registered or has no GPU VA space registered for it.
//         - The allocation resides in GPU memory and a mapping was requested
//           for a different GPU and P2P support was not enabled between them.
//         - The UUID of the CPU was specified in the perGpuAttribs list.
//         - UvmGpuCompressionTypeEnabledNoPlc compression type was used on one
//           or more GPUs that don't support PLC.
//
//     NV_ERR_NO_MEMORY:
//         Internal memory allocation failed.
//
//     NV_ERR_INVALID_ARGUMENT:
//         One of the following occurred:
//         - perGpuAttribs is NULL.
//         - gpuAttribsCount is zero.
//         - an invalid mapping type was specified.
//         - an invalid caching type was specified.
//         - an invalid format/element bits combination was specified.
//         - an invalid compression type was specified.
//         - UvmGpuCompressionTypeEnabledNoPlc compression type was used with a
//           non-compressible physical allocation.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmMapExternalAllocation(void                              *base,
                                   NvLength                           length,
                                   NvLength                           offset,
                                   const UvmGpuMappingAttributes     *perGpuAttribs,
                                   NvLength                           gpuAttribsCount,
                                   const UvmAllocationPlatformParams *platformParams);

//------------------------------------------------------------------------------
// UvmMapExternalSparse
//
// Create a Sparse mapping for the virtual address range specified by (base,
// length). The mapping does not have any physical backing, rather the PTEs use
// a special pattern. The virtual address range specified by (base, length) must
// be fully contained within a virtual address range previously created with
// UvmCreateExternalRange.
//
// Virtual address ranges with Sparse mappings will not generate any faults when
// accessed. Instead, writes will be discarded and reads will return 0.
//
// Sparse mappings are supported only on fault-capable GPUs and only for 64K
// pages, so the virtual address range specified by (base, length) must be
// aligned to 64K.
//
// If the range specified by (base, length) falls within any existing mappings,
// the behavior is the same as if UvmUnmapExternal with the range specified by
// (base, length) had been called first.
//
// Note that calling UvmUnregisterGpuVaSpace will also unmap all mappings
// created via this API on the GPU that the GPU VA space is associated with.
// Notably the mappings won't be recreated when the GPU VA space is
// re-registered.
//
// Arguments:
//     base: (INPUT)
//         Base address of the virtual address range. The address must be
//         aligned on a 64K boundary.
//
//     length: (INPUT)
//         Length, in bytes, of the range. The length must be 64K aligned.
//
//
//     gpuUuid: (INPUT)
//         UUID of the physical GPU if the GPU is not SMC capable or SMC
//         enabled, or the GPU instance UUID of the partition to map the sparse
//         region on.
//
// Errors:
//     NV_ERR_INVALID_ADDRESS:
//         One of the following occurred:
//         - base is NULL.
//         - length is zero.
//         - The requested address range does not fall entirely within an
//           existing external VA range created with a single call to
//           UvmCreateExternalRange.
//         - At least one of base and length is not aligned to a 64K
//           boundary.
//
//     NV_ERR_OUT_OF_RANGE:
//         The range specified by (base, length) exceeds the largest virtual
//         address supported by the specified GPU.
//
//     NV_ERR_INVALID_DEVICE:
//         One of the following occurred:
//         - The specified GPU was not registered.
//         - The GPU specified has no VA space registered for it.
//         - The UUID of the CPU was specified.
//         - Sparse mappings are not supported on the specified GPU.
//
//     NV_ERR_NO_MEMORY:
//         Internal memory allocation failed.
//------------------------------------------------------------------------------
NV_STATUS UvmMapExternalSparse(void                  *base,
                               NvLength               length,
                               const NvProcessorUuid *gpuUuid);

//------------------------------------------------------------------------------
// UvmUnmapExternal
//
// Unmaps a virtual address range that was mapped using UvmMapExternalAllocation
// or UvmMapExternalSparse from the specified GPU. The range specified by (base,
// length) must be fully contained within a single External VA range created
// with UvmCreateExternalRange.
//
// If the range specified by (base, length) range partially overlaps existing
// mappings, the overlapping portion of the existing mappings will be unmapped
// provided that the split points are aligned to the mappings' respective page
// sizes. Otherwise, the overlapping portions of the existing mappings will be
// left in an undefined state.
//
// Note that the VA range is not reclaimed until UvmFree is called on it even if
// all mappings in the created range have been unmapped from all GPUs via this
// API.
//
// Arguments:
//     base: (INPUT)
//         Base address of the virtual address range.
//
//     length: (INPUT)
//         The length of the virtual address range.
//
//     gpuUuid: (INPUT)
//         UUID of the physical GPU if the GPU is not SMC capable or SMC
//         enabled, or the GPU instance UUID of the partition to unmap the VA
//         range from.
//
// Errors:
//     NV_ERR_INVALID_ADDRESS:
//         One of the following has occurred:
//         - base is NULL.
//         - The requested address range does not fall entirely within an
//           existing external VA range created with a single call to
//           UvmCreateExternalRange.
//         - base or base + length fall within an existing mapping but are not
//           aligned to that mapping's page size.
//
//     NV_ERR_INVALID_DEVICE:
//         Either gpuUuid does not represent a valid registered GPU or the VA
//         range corresponding to the given base address is not mapped on the
//         specified GPU.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmUnmapExternal(void                  *base,
                           NvLength               length,
                           const NvProcessorUuid *gpuUuid);

// TODO: Bug 2732305: Remove this declaration when the new external APIs have
//       been implemented.
NV_STATUS UvmUnmapExternalAllocation(void                  *base,
                                     const NvProcessorUuid *gpuUuid);

//------------------------------------------------------------------------------
// UvmMapDynamicParallelismRegion
//
// Creates a special mapping required for dynamic parallelism. The mapping
// doesn't have any physical backing, it's just a PTE with a special kind.
//
// The virtual address range specified by (base, length) must cover exactly one
// GPU page, so length must be a page size supported by the GPU and base must be
// aligned to that page size. The VA range must not overlap with an existing
// mapping for the GPU. A GPU VA space must have been registered for the GPU and
// the GPU must support dynamic parallelism.
//
// The mapping is created immediately and not modified until a call to UvmFree
// Calling UvmFree frees the GPU page table mapping. The range cannot be
// associated with range groups and any GPU faults within this range are fatal.
// Also, the pages cannot be the target for read duplication, cannot have a
// preferred location set, and cannot have any accessed-by processors.
//
// Note that calling UvmUnregisterGpuVaSpace will also unmap all mappings
// created via this API on the GPU that the GPU VA space is associated with.
// Notably the mappings won't be recreated when the GPU VA space is
// re-registered, but the range should still be destroyed with UvmFree.
//
// This call neither creates nor modifies any CPU mappings, even if the VA range
// came from a region previously reserved via UvmReserveVa. This implies that
// CPU accesses to this range will cause a fatal fault if it's not mapped.
//
// Arguments:
//     base: (INPUT)
//         Base address of the virtual address range.
//
//     length: (INPUT)
//         Length, in bytes, of the range. Must be equal to a page size
//         supported by the GPU.
//
//     gpuUuid: (INPUT)
//         UUID of the physical GPU if the GPU is not SMC capable or SMC
//         enabled, or the GPU instance UUID of the partition to map the
//         dynamic parallelism region on.
//
// Errors:
//     NV_ERR_UVM_ADDRESS_IN_USE:
//         The requested address range overlaps with an existing allocation.
//
//     NV_ERR_INVALID_ADDRESS:
//         base is NULL or not aligned to length or length is not a page size
//         supported by the GPU.
//
//     NV_ERR_OUT_OF_RANGE:
//         The VA range exceeds the largest virtual address supported by one or
//         more of the specified GPUs.
//
//     NV_ERR_INVALID_DEVICE:
//         The gpuUuid was either not registered, has no GPU VA space
//         registered for it, or the GPU doesn't support dynamic parallelism.
//
//     NV_ERR_NO_MEMORY:
//         Internal memory allocation failed.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmMapDynamicParallelismRegion(void                  *base,
                                         NvLength               length,
                                         const NvProcessorUuid *gpuUuid);

//------------------------------------------------------------------------------
// UvmEnableReadDuplication
//
// Enables read duplication on the specified virtual address range, overriding
// the UVM driver's default migration and mapping policy on read faults.
//
// The virtual address range specified by (base, length) must have been
// allocated via a call to either UvmAlloc or UvmMemMap, or be supported
// system-allocated pageable memory. If the input virtual range corresponds to
// system-allocated pageable memory and UvmIsPageableMemoryAccessSupported
// reports that pageable memory access is supported, the behavior described
// below does not take effect, and read duplication will not be enabled for
// the input range.
//
// Both base and length must be aligned to the smallest page size supported by
// the CPU.
//
// On a read fault from a processor on a page in this range, any existing
// mapping to that page from all other processors will be made read-only. If the
// page does not reside in the faulting processor's memory, a duplicate copy of
// the page will be created there. The copy of the page in the faulting
// processor's memory will then be mapped as read-only on that processor. Note
// that a write to this page from any processor will collapse the duplicated
// copies.
//
// If UvmMigrate, UvmMigrateAsync or UvmMigrateRangeGroup is called on any pages
// in this VA range, then those pages will also be read duplicated on the
// destination processor for the migration.
//
// Enabling read duplication on a VA range requires the CPU and all GPUs with
// registered VA spaces to be fault-capable. Otherwise, the migration and
// mapping policies outlined above are not applied until all the
// non-fault-capable GPUs are unregistered via UvmUnregisterGpu. If a
// non-fault-capable GPU is registered after a page has already been
// read-duplicated, then the copies of that page will be collapsed into a single
// page.
//
// If UvmPreventMigrationRangeGroups has been called on the range group that
// this VA range is associated with, then the migration and mapping policies
// outlined above don't take effect until UvmAllowMigrationRangeGroups is called
// for that range group.
//
// If any page in the VA range has a preferred location, then the migration and
// mapping policies associated with this API take precedence over those related
// to the preferred location. If the preferred location is a specific CPU NUMA
// node, that NUMA node will be used for a CPU-resident copy of the page.
//
// If any pages in this VA range have any processors present in their
// accessed-by list, the migration and mapping policies associated with this
// API override those associated with the accessed-by list.
//
// Multiple calls to this API for the same VA range and the same processor are
// not refcounted, i.e. calling this API on a VA range after it has already been
// called for that same VA range results in a no-op.
//
// Arguments:
//     base: (INPUT)
//         Base address of the virtual address range.
//
//     length: (INPUT)
//         Length, in bytes, of the range.
//
// Errors:
//     NV_ERR_INVALID_ADDRESS:
//         base and length are not properly aligned, or the range does not
//         represent a valid UVM allocation, or the range is pageable memory and
//         the system does not support accessing pageable memory, or the range
//         does not represent a supported Operating System allocation.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmEnableReadDuplication(void     *base,
                                   NvLength  length);

//------------------------------------------------------------------------------
// UvmDisableReadDuplication
//
// Disables read duplication on the specified virtual address range, and reverts
// the associated policies. This also disables any default read duplication
// heuristics employed by the kernel driver.
//
// The virtual address range specified by (base, length) must have been
// allocated via a call to either UvmAlloc or UvmMemMap, or be supported
// system-allocated pageable memory. If the input virtual range corresponds to
// system-allocated pageable memory and UvmIsPageableMemoryAccessSupported
// reports that pageable memory access is supported, the behavior described
// below does not take effect, and read duplication will not be enabled for
// the input range.
//
// Both base and length must be aligned to the smallest page size supported by
// the CPU.
//
// Any pages in the VA range that are currently read duplicated will be
// collapsed into a single copy. The location for the collapsed copy will be the
// preferred location if the page has a preferred location and was resident at
// that location when this API was called. Otherwise, the location will be
// chosen arbitrarily.
//
// It is ok to call this API only on a subset of the VA range on which
// UvmEnableReadDuplication was called or for a VA range on which
// UvmEnableReadDuplication was never called.
//
// Arguments:
//     base: (INPUT)
//         Base address of the virtual address range.
//
//     length: (INPUT)
//         Length, in bytes, of the range.
//
// Errors:
//     NV_ERR_INVALID_ADDRESS:
//         base and length are not properly aligned, or the range does not
//         represent a valid UVM allocation, or the range is pageable memory and
//         the system does not support accessing pageable memory, or the range
//         does not represent a supported Operating System allocation.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//-----------------------------------------------------------------------------
NV_STATUS UvmDisableReadDuplication(void     *base,
                                    NvLength  length);

//------------------------------------------------------------------------------
// UvmSetPreferredLocation
//
// Sets the preferred location for the given virtual address range to be the
// specified processor's memory.
//
// Both base and length must be aligned to the smallest page size supported by
// the CPU. The VA range must lie within the largest possible virtual address
// supported by the specified processor.
//
// The virtual address range specified by (base, length) must have been
// allocated via a call to either UvmAlloc or UvmMemMap (managed memory), or be
// supported system-allocated pageable memory. If the input range corresponds to
// a file backed shared mapping and least one GPU in the system supports
// transparent access to pageable memory, the behavior below is not guaranteed.
//
// If any pages in the VA range are associated with a range group that was made
// non-migratable via UvmPreventMigrationRangeGroups, then those pages are
// migrated immediately to the specified preferred location and mapped according
// to policies specified in UvmPreventMigrationRangeGroups. Otherwise, this API
// neither migrates pages nor does it populate unpopulated pages. Note that if
// the specified preferred location is a fault-capable GPU and at least one page
// in the VA range is associated with a non-migratable range group, then an
// error is returned. Additionally, if the specified preferred location is a
// non-fault capable GPU and at least one page in the VA range is associated
// with a non-migratable range group, an error is returned if another
// non-fault-capable GPU is present in the accessed-by list of that page but P2P
// support has not been enabled between both GPUs.
//
// When a page is in its preferred location, a fault from another processor will
// not cause a migration if a mapping for that page from that processor can be
// established without migrating the page.
//
// When a page that was allocated via either UvmAlloc or UvmMemMap migrates away
// from its preferred location, the mapping on the preferred location's
// processor is cleared so that the next access from that processor will cause a
// fault and migrate the page back to its preferred location. In other words, a
// page is mapped on the preferred location's processor only if the page is in
// its preferred location. Thus, when the preferred location changes, mappings
// to pages in the given range are removed from the new preferred location if
// the pages are resident in a different processor. Note that if the preferred
// location's processor is a GPU, then a mapping from that GPU to a page in the
// VA range is only created if a GPU VA space has been registered for that GPU
// and the page is in its preferred location.
//
// If read duplication has been enabled for any pages in this VA range and
// UvmPreventMigrationRangeGroups has not been called on the range group that
// those pages are associated with, then the migration and mapping policies
// associated with UvmEnableReadDuplication override the policies outlined
// above. Note that enabling read duplication on any pages in this VA range
// does not clear the state set by this API for those pages. It merely overrides
// the policies associated with this state until read duplication is disabled
// for those pages.
//
// If the preferred location processor is present in the accessed-by list of any
// of the pages in this VA range, then the migration and mapping policies
// associated with this API override those associated with the accessed-by list.
//
// The state set by this API can be cleared either by calling
// UvmUnsetPreferredLocation for the same VA range or by calling
// UvmUnregisterGpu on this processor if the processor is a GPU. Note that
// calling UvmUnregisterGpuVaSpace will not clear the state set by this API.
// Multiple calls to this API for the same VA range and the same processor are
// not refcounted, i.e. calling this API on a VA range and processor after it
// has already been called for that same VA range and processor results in a
// no-op.
//
// Arguments:
//     base: (INPUT)
//         Base address of the virtual address range.
//
//     length: (INPUT)
//         Length, in bytes, of the range.
//
//     preferredLocationUuid: (INPUT)
//         UUID of the physical GPU if the GPU is not SMC capable or SMC
//         enabled, the GPU instance UUID of the partition, or the CPU UUID
//         preferred location.
//
//     preferredCpuMemoryNode: (INPUT)
//         Preferred CPU NUMA memory node used if preferredLocationUuid is the
//         UUID of the CPU. -1 is a special value which indicates all CPU nodes
//         allowed by the global and thread memory policies. If NUMA is disabled
//         only 0 and -1 are allowed.
//
// Errors:
//     NV_ERR_INVALID_ADDRESS:
//         One of the following occurred:
//         - base and length are not properly aligned.
//         - The range does not represent a valid UVM allocation.
//         - The range is pageable memory and the system does not support
//           accessing pageable memory.
//         - The range does not represent a supported Operating System
//           allocation.
//
//     NV_ERR_OUT_OF_RANGE:
//         The VA range exceeds the largest virtual address supported by the
//         specified processor.
//
//     NV_ERR_INVALID_DEVICE:
//         One of the following occurred:
//         - preferredLocationUuid is neither the UUID of the CPU nor the UUID
//           of a GPU that was registered by this process.
//         - At least one page in VA range belongs to a non-migratable range
//           group and the specified UUID represents a fault-capable GPU.
//         - preferredLocationUuid is the UUID of a non-fault-capable GPU and at
//           least one page in the VA range belongs to a non-migratable range
//           group and another non-fault-capable GPU is in the accessed-by list
//           of the same page but P2P support between both GPUs has not been
//           enabled.
//
//      NV_ERR_INVALID_ARGUMENT:
//         One of the following occured:
//         - preferredLocationUuid is the UUID of the CPU and
//           preferredCpuMemoryNode is either:
//              - not a valid NUMA node,
//              - not a possible NUMA node, or
//              - a NUMA node ID corresponding to a registered GPU.
//
//     NV_ERR_NOT_SUPPORTED:
//         The UVM file descriptor is associated with another process and the
//         input virtual range corresponds to system-allocated pageable memory.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmSetPreferredLocation(void                  *base,
                                  NvLength               length,
                                  const NvProcessorUuid *preferredLocationUuid,
                                  NvS32                  preferredCpuMemoryNode);

//------------------------------------------------------------------------------
// UvmUnsetPreferredLocation
//
// Unsets the preferred location associated with all pages in the specified
// virtual address range, reverting the migration and mapping policies outlined
// in UvmSetPreferredLocation.
//
// Both base and length must be aligned to the smallest page size supported by
// the CPU.
//
// The virtual address range specified by (base, length) must have been
// allocated via a call to either UvmAlloc or UvmMemMap, or be supported
// system-allocated pageable memory. If the input range corresponds to a file
// backed shared mapping and least one GPU in the system supports transparent
// access to pageable memory, the behavior below is not guaranteed.
//
// If the VA range is associated with a non-migratable range group, then that
// association is cleared. i.e. the pages in this VA range have their range
// group association changed to UVM_RANGE_GROUP_ID_NONE.
//
// It is ok to call this API only on a subset of the VA range on which
// UvmSetPreferredLocation was called or for a VA range on which
// UvmSetPreferredLocation was never called.
//
// Arguments:
//     base: (INPUT)
//         Base address of the virtual address range.
//
//     length: (INPUT)
//         Length, in bytes, of the range.
//
// Errors:
//     NV_ERR_INVALID_ADDRESS:
//         One of the following occured:
//         - base and length are not properly aligned or the range does not
//           represent a valid UVM allocation.
//         - The range is pageable memory and the system does not support
//           accessing pageable memory.
//         - The range does not represent a supported Operating System
//           allocation.
//         - The range contains both managed and pageable memory allocations.
//
//     NV_ERR_NOT_SUPPORTED:
//         The UVM file descriptor is associated with another process and the
//         input virtual range corresponds to system-allocated pageable memory.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmUnsetPreferredLocation(void     *base,
                                    NvLength  length);

//------------------------------------------------------------------------------
// UvmSetAccessedBy
//
// Indicates to the UVM driver that the pages in the given virtual address range
// should be mapped on the specified processor whenever establishing such a
// mapping is possible. The purpose of this API is to prevent faults from the
// specified processor to the given VA range as much as possible.
//
// Both base and length must be aligned to the smallest page size supported by
// the CPU. The VA range must lie within the largest possible virtual address
// supported by the specified processor.
//
// The virtual address range specified by (base, length) must have been
// allocated via a call to either UvmAlloc or UvmMemMap, or be supported
// system-allocated pageable memory. If the input range is pageable memory and
// at least one GPU in the system supports transparent access to pageable
// memory, the behavior described below does not take effect and the accessed-by
// processor list of the VA range does not change.
//
// If a page in the VA range is not populated or its current location doesn't
// permit a mapping to be established, then no mapping is created for that page.
// If a page in the VA range migrates to a new location, then the mapping is
// updated to point to the new location if establishing such a mapping is
// possible. If a page in the VA range is associated with a non-migratable range
// group and the specified processor is a non-fault-capable GPU, then an error
// is returned if the mapping cannot be established.
//
// If the specified processor is a GPU and no GPU VA space has been registered
// for it or if the registered GPU VA space gets unregistered, then the policies
// outlined above will take effect the next time a GPU VA space gets registered
// for this GPU.
//
// If read duplication is enabled in any pages in this VA range, then the page
// mapping policy associated with read duplication overrides the mapping policy
// associated with this API.
//
// Similarly, if any page in this VA range has a preferred location, and the
// UUID of the preferred location is the same as the UUID passed in to this API,
// then the mapping policy associated with having a preferred location overrides
// the mapping policy associated with this API.
//
// Note that enabling read duplication or setting a preferred location on any
// pages in this VA range does not clear the state set by this API for those
// pages. It merely overrides the policies associated with this state until read
// duplication is disabled on those pages or their preferred location is
// cleared.
//
// The state set by this API can be cleared either by calling UvmUnsetAccessedBy
// for the same VA range and processor or by calling UvmUnregisterGpu on this
// processor if the processor is a GPU. It is also cleared if the processor is a
// non-fault-capable GPU and the VA range has a preferred location set to a peer
// GPU and peer access is disabled via UvmDisablePeerAccess. Note however that
// calling UvmUnregisterGpuVaSpace will not clear the state set by this API.
//
// Multiple calls to this API for the same VA range and the same processor are
// not refcounted. i.e. calling this API on a VA range and processor after it
// has already been called for that same VA range and processor results in a
// no-op.
//
// Arguments:
//     base: (INPUT)
//         Base address of the virtual address range.
//
//     length: (INPUT)
//         Length, in bytes, of the range.
//
//     accessedByUuid: (INPUT)
//         UUID of the physical GPU if the GPU is not SMC capable or SMC
//         enabled, the GPU instance UUID of the partition, or the CPU UUID
//         that should have pages in the VA range mapped when possible.
//
// Errors:
//     NV_ERR_INVALID_ADDRESS:
//         base and length are not properly aligned or the range does not
//         represent a valid UVM allocation, or the range is pageable memory and
//         the system does not support accessing pageable memory, or the range
//         does not represent a supported Operating System allocation.
//
//     NV_ERR_OUT_OF_RANGE:
//         The VA range exceeds the largest virtual address supported by the
//         specified processor.
//
//     NV_ERR_INVALID_DEVICE:
//         accessedByUuid is neither the UUID of the CPU nor the UUID of a GPU
//         that was registered by this process. Or accessedByUuid is the UUID of
//         a non-fault-capable GPU and the VA range is associated with a
//         non-migratable range group with a preferred location set to another
//         non-fault-capable GPU that doesn't have P2P support enabled with this
//         GPU.
//
//     NV_ERR_NO_MEMORY:
//         accessedByUuid is a non-fault-capable GPU and there was insufficient
//         memory to create the mapping.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmSetAccessedBy(void                  *base,
                           NvLength               length,
                           const NvProcessorUuid *accessedByUuid);

//------------------------------------------------------------------------------
// UvmUnsetAccessedBy
//
// Undoes the effect of UvmSetAccessedBy for the given virtual address range on
// the specified processor, thereby reverting the mapping policies imposed by
// UvmSetAccessedBy.
//
// Both base and length must be aligned to the smallest page size supported by
// the CPU
//
// The virtual address range specified by (base, length) must have been
// allocated via a call to either UvmAlloc or UvmMemMap, or be supported
// system-allocated pageable memory. If the input range is pageable memory and
// at least one GPU in the system supports transparent access to pageable
// memory, the behavior described below does not take effect and the accessed-by
// processor list of the VA range does not change.

//
// Existing mappings to this VA range from the given processor are not affected.
// If any page in the VA range migrates to a different location however, the
// mapping may be cleared or updated based on other mapping policies that are in
// effect.
//
// It is ok to call this API for a subset of a VA range with a accessed-by list
// containing this processor, or for a VA range with an empty accessed-by list.
//
// Arguments:
//     base: (INPUT)
//         Base address of the virtual address range.
//
//     length: (INPUT)
//         Length, in bytes, of the range.
//
//     accessedByUuid: (INPUT)
//         UUID of the physical GPU if the GPU is not SMC capable or SMC
//         enabled, the GPU instance UUID of the partition, or the CPU UUID
//         from which any policies set by UvmSetAccessedBy should be revoked
//         for the given VA range.
//
// Errors:
//     NV_ERR_INVALID_ADDRESS:
//         base and length are not properly aligned or the range does not
//         represent a valid UVM allocation, or the range is pageable memory and
//         the system does not support accessing pageable memory, or the range
//         does not represent a supported Operating System allocation.
//
//     NV_ERR_INVALID_DEVICE:
//         accessedByUuid is neither the UUID of the CPU nor the UUID of a GPU
//         that was registered by this process.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmUnsetAccessedBy(void                  *base,
                             NvLength               length,
                             const NvProcessorUuid *accessedByUuid);

//------------------------------------------------------------------------------
// UvmEnableSystemWideAtomics
//
// Enables software-assisted system-wide atomics support on the specified GPU.
// Any system-wide atomic operation issued from this GPU is now guaranteed to be
// atomic with respect to all accesses from other processors that also support
// system-wide atomics regardless of whether that support is enabled on those
// other processors or not.
//
// The class of atomic operations from the GPU that are considered system-wide
// is GPU architecture dependent. All atomic operations from the CPU are always
// considered to be system-wide and support for system-wide atomics on the CPU
// is always considered to be enabled.
//
// System-wide atomics which cannot be natively supported in hardware are
// emulated using virtual mappings and page faults. For example, assume a
// virtual address which is resident in CPU memory and has CPU memory as its
// preferred location. A GPU with system-wide atomics enabled but without native
// atomics support to CPU memory will not have atomics enabled in its virtual
// mapping of the page that contains that address. If that GPU performs an
// atomic operation, the access will fault, all other processors' mappings to
// that page will have their write permissions revoked, the faulting GPU will be
// granted atomic permissions in its virtual mapping, and the faulting GPU will
// retry its access. Further atomic accesses from that GPU will not cause page
// faults until another processor attempts a write access to the same page.
//
// Multiple calls to this API for the same GPU are not refcounted, i.e. calling
// this API for a GPU for which software-assisted system-wide atomics support
// has already been enabled results in a no-op.
//
// The GPU must have been registered using UvmRegisterGpu prior to making this
// call. By default, software-assisted system-wide atomics support is enabled
// when a GPU is registered.
//
// Arguments:
//     gpuUuid: (INPUT)
//         UUID of the physical GPU if the GPU is not SMC capable or SMC
//         enabled, or the GPU instance UUID of the partition to enable
//         software-assisted system-wide atomics on.
//
// Error codes:
//     NV_ERR_NO_MEMORY:
//         Internal memory allocation failed.
//
//     NV_ERR_INVALID_DEVICE:
//         The GPU referred to by gpuUuid was not registered.
//
//     NV_ERR_NOT_SUPPORTED:
//         The GPU does not support system-wide atomic operations, or the GPU
//         has hardware support for scoped atomic operations.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmEnableSystemWideAtomics(const NvProcessorUuid *gpuUuid);

//------------------------------------------------------------------------------
// UvmDisableSystemWideAtomics
//
// Disables software-assisted system-wide atomics support on the specified GPU.
// Any atomic operation from this GPU is no longer guaranteed to be atomic with
// respect to accesses from other processors in the system, even if the
// operation has system-wide scope at the instruction level.
//
// The GPU must have been registered using UvmRegisterGpu prior to making this
// call. It is however ok to call this API for GPUs that do not have support for
// system-wide atomic operations enabled. If the GPU is unregistered via
// UvmUnregisterGpu and then registered again via UvmRegisterGpu, support for
// software-assisted system-wide atomics will be enabled.
//
// Arguments:
//     gpuUuid: (INPUT)
//         UUID of the physical GPU if the GPU is not SMC capable or SMC
//         enabled, or the GPU instance UUID of the partition to disable
//         software-assisted system-wide atomics on.
//
// Error codes:
//     NV_ERR_INVALID_DEVICE:
//         The GPU referred to by gpuUuid was not registered.
//
//     NV_ERR_NOT_SUPPORTED:
//         The GPU does not support system-wide atomic operations, or the GPU
//         has hardware support for scoped atomic operations.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmDisableSystemWideAtomics(const NvProcessorUuid *gpuUuid);

//------------------------------------------------------------------------------
// UvmGetFileDescriptor
//
// Returns the UVM file descriptor currently being used to call into the UVM
// kernel mode driver. The data type of the returned file descriptor is platform
// specific.
//
// If UvmInitialize has not yet been called, an error is returned. If
// UvmInitialize was called with UVM_AUTO_FD, then the file created during
// UvmInitialize is returned. If UvmInitialize was called with an existing UVM
// file descriptor, then that file descriptor is returned.
//
// Arguments:
//     returnedFd: (OUTPUT)
//         A platform specific file descriptor.
//
// Error codes:
//     NV_ERR_INVALID_ARGUMENT:
//         returnedFd is NULL.
//
//     NV_ERR_INVALID_STATE:
//         UVM was not initialized before calling this function.
//
//------------------------------------------------------------------------------
NV_STATUS UvmGetFileDescriptor(UvmFileDescriptor *returnedFd);

//------------------------------------------------------------------------------
// UvmGetMmFileDescriptor
//
// Returns the UVM file descriptor currently being used to keep the
// memory management context valid. The data type of the returned file
// descriptor is platform specific.
//
// If UvmInitialize has not yet been called, an error is returned.
//
// Arguments:
//     returnedFd: (OUTPUT)
//         A platform specific file descriptor.
//
// Error codes:
//     NV_ERR_INVALID_ARGUMENT:
//         returnedFd is NULL.
//
//     NV_ERR_INVALID_STATE:
//         UVM was not initialized before calling this function.
//
//     NV_ERR_NOT_SUPPORTED:
//         This file descriptor is not required on this platform.
//------------------------------------------------------------------------------
NV_STATUS UvmGetMmFileDescriptor(UvmFileDescriptor *returnedFd);

//------------------------------------------------------------------------------
// UvmIs8Supported
//
// Returns whether the kernel driver has been loaded in UVM 8 mode or not.
//
// Argument:
//     is8Supported: (OUTPUT)
//         Will be set to true (nonzero) if the driver was loaded as UVM 8, or
//         false (zero) if it was loaded as UVM Lite.
//
// Error codes:
//     NV_ERR_INVALID_ARGUMENT:
//         is8Supported is NULL.
//
//     NV_ERR_GENERIC:
//         Unexpected error. We try hard to avoid returning this error code,
//         because it is not very informative.
//
//------------------------------------------------------------------------------
NV_STATUS UvmIs8Supported(NvU32 *is8Supported);

//------------------------------------------------------------------------------
//    Tools API
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// UvmDebugGetVersion
//
// Returns the version number of the UVM debug library
// See uvm_types.h for valid verion numbers, e.g. UVM_DEBUG_V1
//
//------------------------------------------------------------------------------
unsigned UvmDebugVersion(void);

//------------------------------------------------------------------------------
// UvmDebugCreateSession
//
// Creates a handle for a debugging session.
//
// When the client initializes, it will pass in a process handle and get a
// session ID for itself. Subsequent calls to the UVM API will take in that
// session ID.
//
// There are security requirements to this call.
// One of the following must be true:
// 1.  The session owner must be running as an elevated user
// 2.  The session owner and target must belong to the same user and the
//     session owner is at least as privileged as the target.
//
// For CUDA 6.0 we can create at most 64 sessions per debugger process.
//
// Arguments:
//     pid: (INPUT)
//         Process id for which the debugging session will be created
//
//     session: (OUTPUT)
//         Handle to the debugging session associated to that pid.
//
// Error codes:
//     NV_ERR_PID_NOT_FOUND:
//         pid is invalid/ not associated with UVM.
//
//     NV_ERR_INSUFFICIENT_PERMISSIONS:
//         Function fails the security check.
//
//     NV_ERR_INSUFFICIENT_RESOURCES:
//         Attempt is made to allocate more than 64 sessions per process.
//
//     NV_ERR_BUSY_RETRY:
//         internal resources are blocked by other threads.
//
//------------------------------------------------------------------------------
NV_STATUS UvmDebugCreateSession(unsigned         pid,
                                UvmDebugSession *session);

//------------------------------------------------------------------------------
// UvmDebugDestroySession
//
// Destroys a debugging session.
//
// Arguments:
//     session: (INPUT)
//         Handle to the debugging session associated to that pid.
//
// Error codes:
//     NV_ERR_INVALID_ARGUMENT:
//         session is invalid.
//
//     NV_ERR_BUSY_RETRY:
//         ebug session is in use by some other thread.
//
//------------------------------------------------------------------------------
NV_STATUS UvmDebugDestroySession(UvmDebugSession session);

//------------------------------------------------------------------------------
// UvmDebugCountersEnable
//
// Enables the counters following the user specified configuration.
//
// The user must fill a list with the configuration of the counters it needs to
// either enable or disable. It can only enable one counter per line.
//
// The structure (UvmCounterConfig) has several fields:
//  - scope: Please see the UvmCounterScope  enum (above), for details.
//  - name: Name of the counter. Please check UvmCounterName for list.
//  - gpuid: Identifies the GPU for which the counter will be enabled/disabled
//           This parameter is ignored in AllGpu scopes.
//  - state: A value of 0 will disable the counter, a value of 1 will enable
//           the counter.
//
//  Note: All counters are refcounted, that means that a counter will only be
//  disable when its refcount reached zero.
//
// Arguments:
//     session: (INPUT)
//         Handle to the debugging session.
//
//     config: (INPUT)
//         pointer to configuration list as per above.
//
//     count: (INPUT)
//         number of entries in the config list.
//
// Error codes:
//     NV_ERR_INSUFFICIENT_PERMISSIONS:
//         Function fails the security check
//
//     RM_INVALID_ARGUMENT:
//         debugging session is invalid or one of the counter lines is invalid.
//         If call returns this value, no action specified by the config list
//         will have taken effect.
//
//     NV_ERR_NOT_SUPPORTED:
//         UvmCounterScopeGlobalSingleGpu is not supported for CUDA 6.0
//
//     NV_ERR_BUSY_RETRY:
//         the debug session is in use by some other thread.
//
//------------------------------------------------------------------------------
NV_STATUS UvmDebugCountersEnable(UvmDebugSession   session,
                                 UvmCounterConfig *config,
                                 unsigned          count);

//------------------------------------------------------------------------------
// UvmDebugGetCounterHandle
//
// Returns handle to a particular counter. This is an opaque handle that the
// implementation uses in order to find your counter, later. This handle can be
// used in subsequent calls to UvmDebugGetCounterVal().
//
// Arguments:
//     session: (INPUT)
//         Handle to the debugging session.
//
//     scope: (INPUT)
//         Scope that will be mapped.
//
//     counterName: (INPUT)
//         Name of the counter in that scope.
//
//     gpu: (INPUT)
//         UUID of the physical GPU if the GPU is not SMC capable or SMC
//         enabled, or the GPU instance UUID of the partition of the scoped GPU.
//         This parameter is ignored in AllGpu scopes.
//
//     pCounterHandle: (OUTPUT)
//         Handle to the counter address.
//
// Error codes:
//     NV_ERR_INVALID_ARGUMENT:
//         Specified scope/gpu pair or session id is invalid
//
//     NV_ERR_NOT_SUPPORTED:
//         UvmCounterScopeGlobalSingleGpu is not supported for CUDA 6.0
//
//     NV_ERR_BUSY_RETRY:
//         debug session is in use by some other thread.
//
//------------------------------------------------------------------------------
NV_STATUS UvmDebugGetCounterHandle(UvmDebugSession  session,
                                   UvmCounterScope  scope,
                                   UvmCounterName   counterName,
                                   NvProcessorUuid  gpu,
                                   NvUPtr          *pCounterHandle);

//------------------------------------------------------------------------------
// UvmDebugGetCounterVal
//
// Returns the counter value specified by the counter name.
//
// Arguments:
//     session: (INPUT)
//         Handle to the debugging session.
//
//     counterHandleArray: (INPUT)
//         Array of counter handles
//
//     handleCount: (INPUT)
//         Number of handles in the pPCounterHandle array.
//
//     counterValArray: (OUTPUT)
//         Array of counter values corresponding to the handles.
//
// Error codes:
//     NV_ERR_INVALID_ARGUMENT:
//         one of the specified handles is invalid.
//
//------------------------------------------------------------------------------
NV_STATUS UvmDebugGetCounterVal(UvmDebugSession     session,
                                NvUPtr             *counterHandleArray,
                                unsigned            handleCount,
                                unsigned long long *counterValArray);

//------------------------------------------------------------------------------
// UvmEventQueueCreate
//
// This call creates an event queue of the given size.
// No events are added in the queue until they are enabled by the user.
// Event queue data is visible to the user even after the target process dies
// if the session is active and queue is not freed.
//
// User doesn't need to serialize multiple UvmEventQueueCreate calls as
// each call creates a new queue state associated with the returned queue
// handle.
//
// Arguments:
//     sessionHandle: (INPUT)
//         Handle to the debugging session.
//
//     queueHandle: (OUTPUT)
//         Handle to created queue.
//
//     queueSize: (INPUT)
//         Size of the event queue buffer in units of UvmEventEntry's.
//         This quantity must be > 1.
//
//     notificationCount: (INPUT)
//         Number of entries after which the user should be notified that
//         there are events to fetch.
//         User is notified when queueEntries >= notification count.
//
// Error codes:
//     NV_ERR_INSUFFICIENT_PERMISSIONS:
//         Function fails the security check.
//
//     NV_ERR_INVALID_ARGUMENT:
//         One of the arguments is invalid.
//
//     NV_ERR_INSUFFICIENT_RESOURCES:
//         it's not possible to allocate a queue of requested size.
//
//     NV_ERR_BUSY_RETRY:
//         internal resources are blocked by other threads.
//
//     NV_ERR_PID_NOT_FOUND:
//         queue create call is made on a session after the target dies.
//
//------------------------------------------------------------------------------
NV_STATUS UvmEventQueueCreate(UvmDebugSession        sessionHandle,
                              UvmEventQueueHandle   *queueHandle,
                              NvS64                  queueSize,
                              NvU64                  notificationCount,
                              UvmEventTimeStampType  timeStampType);

//------------------------------------------------------------------------------
// UvmEventQueueDestroy
//
// This call frees all interal resources associated with the queue, including
// unpinning of the memory associated with that queue. Freeing user buffer is
// responsibility of a caller. Event queue might be also destroyed as a side
// effect of destroying a session associated with this queue.
//
// User needs to ensure that a queue handle is not deleted while some other
// thread is using the same queue handle.
//
// Arguments:
//     sessionHandle: (INPUT)
//         Handle to the debugging session.
//
//     queueHandle: (INPUT)
//         Handle to the queue which is to be freed
//
// Error codes:
//     RM_ERR_NOT_PERMITTED:
//         Function fails the security check.
//
//     NV_ERR_INVALID_ARGUMENT:
//         One of the arguments is invalid.
//
//     NV_ERR_BUSY_RETRY:
//         internal resources are blocked by other threads.
//
//------------------------------------------------------------------------------
NV_STATUS UvmEventQueueDestroy(UvmDebugSession     sessionHandle,
                               UvmEventQueueHandle queueHandle);

//------------------------------------------------------------------------------
// UvmEventEnable
//
// This call enables a particular event type in the event queue.
// All events are disabled by default when a queue is created.
//
// This API does not access the queue state maintained in the user
// library so the user doesn't need to acquire a lock to protect the queue
// state.
//
// Arguments:
//     sessionHandle: (INPUT)
//         Handle to the debugging session.
//
//     queueHandle: (INPUT)
//         Handle to the queue where events are to be enabled
//
//     eventTypeFlags: (INPUT)
//         This field specifies the event types to be enabled. For example:
//         To enable migration events and memory violations: pass flags
//         "UVM_EVENT_ENABLE_MEMORY_VIOLATION |UVM_EVENT_ENABLE_MIGRATION"
//
// Error codes:
//     RM_ERR_NOT_PERMITTED:
//         Function fails the security check.
//
//     NV_ERR_INVALID_ARGUMENT:
//         One of the arguments is invalid.
//
//     NV_ERR_PID_NOT_FOUND:
//         this call is made after the target process dies
//
//     NV_ERR_BUSY_RETRY:
//         internal resources are blocked by other threads.
//
//------------------------------------------------------------------------------
NV_STATUS UvmEventEnable(UvmDebugSession     sessionHandle,
                         UvmEventQueueHandle queueHandle,
                         unsigned            eventTypeFlags);

//------------------------------------------------------------------------------
// UvmEventDisable
//
// This call disables a particular event type in the queue.
//
// This API does not access the queue state maintained in the user
// library so the user doesn't need to acquire a lock to protect the queue
// state.
//
// Arguments:
//     sessionHandle: (INPUT)
//         Handle to the debugging session.
//
//     queueHandle: (INPUT)
//         Handle to the queue where events are to be enabled
//
//     eventTypeFlags: (INPUT)
//         This field specifies the event types to be enabled
//         For example: To enable migration events and memory violations:
//         pass "UVM_EVENT_ENABLE_MEMORY_VIOLATION |UVM_EVENT_ENABLE_MIGRATION"
//         as flags
//
// Error codes:
//     RM_ERR_NOT_PERMITTED:
//         Function fails the security check.
//
//     NV_ERR_INVALID_ARGUMENT:
//         One of the arguments is invalid.
//
//     NV_ERR_PID_NOT_FOUND:
//         this call is made after the target process dies
//
//     NV_ERR_BUSY_RETRY:
//         internal resources are blocked by other threads.
//
//------------------------------------------------------------------------------
NV_STATUS UvmEventDisable(UvmDebugSession     sessionHandle,
                          UvmEventQueueHandle queueHandle,
                          unsigned            eventTypeFlags);

//------------------------------------------------------------------------------
// UvmEventWaitOnQueueHandles
//
// User is notified when queueEntries >= notification count.
// This call does a blocking wait for this notification. It returns when
// at least one of the queue handles has events to be fetched or if it timeouts
//
//     This API accesses constant data maintained in the queue state. Hence,
//     the user doesn't need to acquire a lock to protect the queue state.
//
// Arguments:
//     queueHandles: (INPUT)
//         array of queue handles.
//
//     arraySize: (INPUT)
//         number of handles in array.
//
//     timeout: (INPUT)
//         timeout in msec
//
//     pNotificationFlags: (OUTPUT)
//         If a particular queue handle in the input array is notified then
//         the respective bit flag is set in pNotificationFlags.
//
// Error codes:
//     NV_ERR_INVALID_ARGUMENT:
//         one of the queueHandles is invalid.
//
//------------------------------------------------------------------------------
NV_STATUS UvmEventWaitOnQueueHandles(UvmEventQueueHandle *queueHandleArray,
                                     unsigned             arraySize,
                                     NvU64                timeout,
                                     unsigned            *pNotificationFlags);

//------------------------------------------------------------------------------
// UvmEventGetNotificationHandles
//
// User is notified when queueEntries >= notification count.
// The user can directly get the queue notification handles rather than using
// a UVM API to wait on queue handles. This helps the user to wait on other
// objects (apart from queue notification) along with queue notification
// handles in the same thread. The user can safely use this call along with the
// library supported wait call UvmEventWaitOnQueueHandles.
//
// This API reads constant data maintained in the queue state. Hence,
// the user doesn't need to acquire a lock to protect the queue state.
//
// Arguments:
//     queueHandles: (INPUT)
//         array of queue handles.
//
//     arraySize: (INPUT)
//         number of handles in array.
//
//     notificationHandles: (OUTPUT)
//         Windows: Output of this call contains an array of 'windows event
//             handles' corresponding to the queue handles passes as input.
//         Linux: All queues belonging to the same process share the same
//             file descriptor(fd) for notification. If the user chooses to use
//             UvmEventGetNotificationHandles then he should check all queues
//             for new events (by calling UvmEventFetch) when notified on
//             the fd.
//
// Error codes:
//     NV_ERR_INVALID_ARGUMENT:
//         One of the arguments is invalid.
//
//------------------------------------------------------------------------------
NV_STATUS UvmEventGetNotificationHandles(UvmEventQueueHandle  *queueHandleArray,
                                         unsigned              arraySize,
                                         void                **notificationHandleArray);

//------------------------------------------------------------------------------
// UvmEventGetGpuUuidTable
//
// Each migration event entry contains the gpu index to/from where data is
// migrated. This index maps to a corresponding physical gpu UUID in the
// gpuUuidTable. Using indices saves on the size of each event entry. This API
// provides the gpuIndex to gpuUuid relation to the user.
//
// This API does not access the queue state maintained in the user
// library and so the user doesn't need to acquire a lock to protect the
// queue state.
//
// Arguments:
//     gpuUuidTable: (OUTPUT)
//         The return value is an array of physical GPU UUIDs. The array index
//         is the corresponding gpuIndex. There can be at max 32 GPUs
//         associated with UVM, so array size is 32.
//
//     validCount: (OUTPUT)
//         The system doesn't normally contain 32 GPUs. This field gives the
//         count of entries that are valid in the returned gpuUuidTable.
//
// Error codes:
//     NV_ERR_BUSY_RETRY:
//         internal resources are blocked by other threads.
//
//------------------------------------------------------------------------------
NV_STATUS UvmEventGetGpuUuidTable(NvProcessorUuid *gpuUuidTable,
                                  unsigned        *validCount);

//------------------------------------------------------------------------------
// UvmEventFetch
//
// This call is used to fetch the queue entries in a user buffer.
//
// This API updates the queue state. Hence simultaneous calls to fetch/skip
// events should be avoided as that might corrupt the queue state.
//
// Arguments:
//     sessionHandle: (INPUT)
//         Handle to the debugging session.
//
//     queueHandle: (INPUT)
//         queue from where to fetch the events.
//
//     pBuffer: (OUTPUT)
//         Pointer to the buffer where the API will copy the events. User
//         shall ensure the size is enough.
//
//     nEntries: (INPUT/OUTPUT)
//         It provides the maximum number of entries that will be fetched
//         from the queue. If this number is larger than the size of the
//         queue it will be internally capped to that value.
//         As output it returns the actual number of entries copies to the
//         buffer.
//
// Error codes:
//     RM_ERR_NOT_PERMITTED:
//         Function fails the security check.
//
//     NV_ERR_INVALID_ARGUMENT:
//         One of the arguments is invalid.
//
//     NV_ERR_INVALID_INDEX:
//         The indices of the queue have been corrupted.
//
//     NV_ERR_BUFFER_TOO_SMALL:
//         The event queue buffer provided by the caller was too small to
//         contain all of the events that occurred during this run.
//         Events were therefore dropped (not recorded).
//         Please re-run with a larger buffer.
//
//------------------------------------------------------------------------------
NV_STATUS UvmEventFetch(UvmDebugSession      sessionHandle,
                        UvmEventQueueHandle  queueHandle,
                        UvmEventEntry_V1    *pBuffer,
                        NvU64               *nEntries);

//------------------------------------------------------------------------------
// UvmEventSkipAll
//
// This API drops all event entries from the queue.
//
// This API updates the queue state. Hence simultaneous calls to fetch/
// skip events should be avoided as that might corrupt the queue state.
//
// Arguments:
//     sessionHandle: (INPUT)
//         Handle to the debugging session.
//
//     queueHandle: (INPUT)
//         target queue.
//
// Error codes:
//     RM_ERR_NOT_PERMITTED:
//         Function fails the security check.
//
//     NV_ERR_INVALID_ARGUMENT:
//         One of the arguments is invalid.
//
//------------------------------------------------------------------------------
NV_STATUS UvmEventSkipAll(UvmDebugSession     sessionHandle,
                          UvmEventQueueHandle queueHandle);

//------------------------------------------------------------------------------
// UvmEventQueryTimeStampType
//
// This API returns the type of time stamp used in an event entry for a given
// queue.
//
// This API reads constant data maintained in the queue state. Hence,
// the user doesn't need to acquire a lock to protect the queue state.
//
// Arguments:
//     sessionHandle: (INPUT)
//         Handle to the debugging session.
//
//     queueHandle: (INPUT)
//         target queue.
//
//     timeStampType: (OUTPUT)
//         type of time stamp used in event entry. See UvmEventTimestampType
//         for supported types of time stamps.
//
// Error codes:
//     RM_ERR_NOT_PERMITTED:
//         Function fails the security check.
//
//     NV_ERR_INVALID_ARGUMENT:
//         One of the arguments is invalid.
//
//------------------------------------------------------------------------------
NV_STATUS UvmEventQueryTimeStampType(UvmDebugSession        sessionHandle,
                                     UvmEventQueueHandle    queueHandle,
                                     UvmEventTimeStampType *timeStampType);

//------------------------------------------------------------------------------
// UvmDebugAccessMemory
//
// This call can be used by the debugger to read/write memory range. UVM driver
// may not be aware of all the pages in this range. A bit per page is set by the
// driver if it is read/written by UVM.
//
// Arguments:
//     session: (INPUT)
//         Handle to the debugging session.
//
//     baseAddress: (INPUT)
//         base address from where memory is to be accessed
//
//     sizeInBytes: (INPUT)
//         Number of bytes to be accessed
//
//     accessType: (INPUT)
//         Read or write access request
//
//     buffer: (INPUT/OUTPUT)
//         This buffer would be read or written to by the driver.
//         User needs to allocate a big enough buffer to fit sizeInBytes.
//
//     isBitmaskSet: (INPUT/OUTPUT)
//         Set to 1, if any field in bitmask is set
//         NULL(INPUT) if unused
//
//     bitmask: (INPUT/OUTPUT)
//         One bit per page is set if UVM reads or writes to it.
//         User should allocate a bitmask big enough to fit one bit per page
//         covered by baseAddress + sizeInBytes:
//         (baseAlignmentBytes + sizeInBytes + pageSize - 1)/pageSize number
//         of bits.
//         NULL(IN) if unused.
//
// Error codes:
//     NV_ERR_INVALID_ARGUMENT:
//         One of the arguments is invalid.
//
//------------------------------------------------------------------------------
NV_STATUS UvmDebugAccessMemory(UvmDebugSession     session,
                               void               *baseAddress,
                               NvU64               sizeInBytes,
                               UvmDebugAccessType  accessType,
                               void               *buffer,
                               NvBool             *isBitmaskSet,
                               NvU64              *bitmask);

//
// Uvm Tools uvm API
//


//------------------------------------------------------------------------------
// UvmToolsCreateSession
//
// Creates a handle for a tools session.
//
// When the client initializes, it will pass a duplicated Uvm file handle from
// target's process UvmGetFileDescriptor API, e.g. by calling DuplicateHandle,
// dup2, share file descriptor over Unix Socket Domains. Returned session
// handle is required to create other Tool's objects, e.g. events, counters.
//
// In order to guarantee that session persists the lifetime of a target process,
// callee is responsible for passing a duplicate file descriptor. This is also
// required for correctness in case of out-of-process session.
//
// Passing non duplicated Uvm file handle results in undefined behaviour. The
// least that you should expect is that all your session related objects will
// become useless once target process closes Uvm file handle.
//
//
// There are security requirements for this call to be successful. Fortunately,
// after validating a file descriptor, one of the following conditions must
// hold:
// 1.  The session owner is running as an elevated user
// 2.  The session owner and target belong to the same user and the
//     session owner is at least as privileged as the target.
//
// Arguments:
//     fd: (INPUT)
//         Duplicated file handle from target process.
//
//     session: (OUTPUT)
//         Handle to the tools session associated to fd above.
//
// Error codes:
//     NV_ERR_INVALID_ARGUMENT:
//         fd is either closed or points to non uvm device.
//
//     NV_ERR_NO_MEMORY:
//         Internal memory allocation failed.
//
//------------------------------------------------------------------------------
NV_STATUS UvmToolsCreateSession(UvmFileDescriptor      fd,
                                UvmToolsSessionHandle *session);

//------------------------------------------------------------------------------
// UvmToolsDestroySession
//
// Destroys a tools session. This also has a side-effect of closing fd
// associated with this session during UvmToolsCreateSession.
//
// All resources associated with this session (counters, event queues) are also
// destroyed.
//
// Arguments:
//     session: (INPUT)
//         Handle associated with a Tool's session.
//
// Error codes:
//     NV_ERR_INVALID_ARGUMENT:
//         session handle does not refer to a valid session.
//
//------------------------------------------------------------------------------
NV_STATUS UvmToolsDestroySession(UvmToolsSessionHandle session);

//
// Events subsystem
//
// Events subsystem is useful for a tools process to track target process
// behaviour. Every event refers to a single process using Unified memory.
//
// The most typical use case is as follows:
// 1. Create event Queue using UvmToolsCreateEventQueue
// 2. Start capture of interesting event types using
//    UvmToolsEventQueueEnableEvents
// 3. poll / Loop using Get/Put pointer
// - Consume existing events from user's buffer
// - exit loop based on some condition (e.g. timeout, target process exit)
// - pause (Stop) capture of some of the events
// 4. Destroy event Queue using UvmToolsDestroyEventQueue
//

#if UVM_API_REV_IS_AT_MOST(10)
// This is deprecated and replaced by sizeof(UvmToolsEventControlData).
NvLength UvmToolsGetEventControlSize(void);

// This is deprecated and replaced by sizeof(UvmEventEntry_V1) or
// sizeof(UvmEventEntry_V2).
NvLength UvmToolsGetEventEntrySize(void);
#endif

NvLength UvmToolsGetNumberOfCounters(void);

//------------------------------------------------------------------------------
// UvmToolsCreateEventQueue
//
// This call creates an event queue that can hold the given number of events.
// All events are disabled by default. Event queue data persists lifetime of the
// target process.
//
// Arguments:
//     session: (INPUT)
//         Handle to the tools session.
//
//     version: (INPUT)
//         Requested version for events or counters.
//         See UvmToolsEventQueueVersion.
//
//     event_buffer: (INPUT)
//         User allocated buffer. Must be page-aligned. Must be large enough to
//         hold at least event_buffer_size events. Gets pinned until queue is
//         destroyed.
//
//     event_buffer_size: (INPUT)
//         Size of the event queue buffer in units of UvmEventEntry's. Must be
//         a power of two, and greater than 1.
//
//     event_control (INPUT)
//         User allocated buffer. Must be page-aligned. Must be large enough to
//         hold UvmToolsEventControlData (although single page-size allocation
//         should be more than enough). Gets pinned until queue is destroyed.
//
//     queue: (OUTPUT)
//         Handle to the created queue.
//
// Error codes:
//     NV_ERR_INSUFFICIENT_PERMISSIONS:
//         Session handle does not refer to a valid session
//
//     NV_ERR_INVALID_ARGUMENT:
//         The version is not UvmToolsEventQueueVersion_V1 or
//         UvmToolsEventQueueVersion_V2.
//         One of the parameters: event_buffer, event_buffer_size, event_control
//         is not valid
//
//     NV_ERR_NOT_SUPPORTED:
//         The requested version queue could not be created
//         (i.e., the UVM kernel driver is older and doesn't support
//         UvmToolsEventQueueVersion_V2).
//
//     NV_ERR_INSUFFICIENT_RESOURCES:
//         There could be multiple reasons for this error. One would be that
//         it's not possible to allocate a queue of requested size. Another
//         would be either event_buffer or event_control memory couldn't be
//         pinned (e.g. because of OS limitation of pinnable memory). Also it
//         could not have been possible to create UvmToolsEventQueueDescriptor.
//
//------------------------------------------------------------------------------
#if UVM_API_REV_IS_AT_MOST(10)
NV_STATUS UvmToolsCreateEventQueue(UvmToolsSessionHandle     session,
                                   void                     *event_buffer,
                                   NvLength                  event_buffer_size,
                                   void                     *event_control,
                                   UvmToolsEventQueueHandle *queue);
#else
NV_STATUS UvmToolsCreateEventQueue(UvmToolsSessionHandle        session,
                                   UvmToolsEventQueueVersion    version,
                                   void                        *event_buffer,
                                   NvLength                     event_buffer_size,
                                   void                        *event_control,
                                   UvmToolsEventQueueHandle    *queue);
#endif

UvmToolsEventQueueDescriptor UvmToolsGetEventQueueDescriptor(UvmToolsEventQueueHandle queue);


//------------------------------------------------------------------------------
// UvmToolsSetNotificationThreshold
//
// Sets a custom notification threshold in number of events for a given queue.
// Polling subsystem will notify user about this queue if and only if number
// of unconsumed events is greater or equal notification_threshold. Default
// threshold upon creating an event queue is floor(N / 2), where N represents
// maximum number of events that this queue can fit.
//
// Consequently, if notifications_threshold is greater than queue size, there
// will be no notification.
//
// Arguments:
//     queue: (INPUT)
//         Handle to the queue, for which events are supposed to be enabled
//
//     notification_threshold: (INPUT)
//         A new threshold, in number of events, to be set for this queue.
//
// Error codes:
//     NV_ERR_INSUFFICIENT_PERMISSIONS:
//         Event Queue might be corrupted (associated session is not valid).
//
//     NV_ERR_INVALID_ARGUMENT:
//         Queue handle does not refer to a valid queue.
//------------------------------------------------------------------------------
NV_STATUS UvmToolsSetNotificationThreshold(UvmToolsEventQueueHandle queue,
                                           NvLength notification_threshold);

//------------------------------------------------------------------------------
// UvmToolsDestroyEventQueue
//
// Destroys all internal resources associated with the queue. It unpins the
// buffers provided in UvmToolsCreateEventQueue. Event Queue is also auto
// destroyed when corresponding session gets destroyed.
//
// Arguments:
//     queue: (INPUT)
//         Handle to the queue to be destroyed
//
// Error codes:
//     NV_ERR_INSUFFICIENT_PERMISSIONS:
//         Event Queue might be corrupted (associated session is not valid).
//
//     NV_ERR_INVALID_ARGUMENT:
//         Queue handle does not refer to a valid queue.
//
//------------------------------------------------------------------------------
NV_STATUS UvmToolsDestroyEventQueue(UvmToolsEventQueueHandle queue);

//------------------------------------------------------------------------------
// UvmEventQueueEnableEvents
//
// This call enables a particular event type in the event queue. All events are
// disabled by default. Any event type is considered listed if and only if its
// corresponding value is equal to 1 (in other words, bit is set). Disabled
// events listed in eventTypeFlags are going to be enabled. Enabled events and
// events not listed in eventTypeFlags are not affected by this call.
//
// It is not an error to call this function multiple times with the same
// arguments.
//
// Arguments:
//     queue: (INPUT)
//         Handle to the queue, for which events are supposed to be enabled
//
//     eventTypeFlags: (INPUT)
//         This bit field specifies the event types to be enabled. Events not
//         specified in this field do not change their state. For example to
//         enable migration and memory violations events pass flags
//         "UVM_EVENT_ENABLE_MEMORY_VIOLATION | UVM_EVENT_ENABLE_MIGRATION"
//
// Error codes:
//     NV_ERR_INSUFFICIENT_PERMISSIONS:
//         Event Queue might be corrupted (associated session is not valid).
//
//     NV_ERR_INVALID_ARGUMENT:
//         Queue handle does not refer to a valid queue.
//
//------------------------------------------------------------------------------
NV_STATUS UvmToolsEventQueueEnableEvents(UvmToolsEventQueueHandle queue,
                                         NvU64                    eventTypeFlags);

//------------------------------------------------------------------------------
// UvmToolsEventQueueDisableEvents
//
// This call disables a particular event type in the event queue. Any event type
// is considered listed if and only if its corresponding value is equal to 1
// (in other words, bit is set). Enabled events listed in eventTypeFlags are
// going to be disabled. Disabled events and events not listed in eventTypeFlags
// are not affected by this call.
//
// It is not an error to call this function multiple times with the same
// arguments.
//
// Arguments:
//     queue: (INPUT)
//         Handle to the queue, for which events are supposed to be enabled
//
//     eventTypeFlags: (INPUT)
//         This bit field specifies the event types to be disabled. Events not
//         specified in this field do not change their state. For example to
//         disable migration and memory violations events pass flags
//         "UVM_EVENT_ENABLE_MEMORY_VIOLATION | UVM_EVENT_ENABLE_MIGRATION"
//
// Error codes:
//     NV_ERR_INSUFFICIENT_PERMISSIONS:
//         Event Queue might be corrupted (associated session is not valid).
//
//     NV_ERR_INVALID_ARGUMENT:
//         Queue handle does not refer to a valid event queue.
//
//------------------------------------------------------------------------------
NV_STATUS UvmToolsEventQueueDisableEvents(UvmToolsEventQueueHandle queue,
                                          NvU64                    eventTypeFlags);


//------------------------------------------------------------------------------
// UvmToolsCreateProcessAggregateCounters
//
// Creates the counters structure for tracking aggregate process counters.
// These counters are enabled by default.
//
// Counters position follows the layout of the memory that UVM driver decides to
// use. To obtain particular counter value, user should perform consecutive
// atomic reads at a given buffer + offset address.
//
// It is not defined what is the initial value of a counter. User should rely on
// a difference between each snapshot.
//
// Arguments:
//     session: (INPUT)
//         Handle to the tools session.
//
//     counters_buffer : (INPUT)
//         User allocated buffer. Must be aligned to the OS's page aligned. Must
//         be large enough to hold all possible counter types. In practice, 4kB
//         system page (minimal granurality) should be sufficent. This memory
//         gets pinned until counters are destroyed.
//
//     counters: (OUTPUT)
//         Handle to the created counters.
//
// Error codes:
//     NV_ERR_INSUFFICIENT_PERMISSIONS:
//         Provided session is not valid
//
//     NV_ERR_INSUFFICIENT_RESOURCES
//         There could be multiple reasons for this error. One would be that
//         it's not possible to allocate counters structure. Another would be
//         that either event_buffer or event_control memory couldn't be pinned
//         (e.g. because of OS limitation of pinnable memory)
//
//------------------------------------------------------------------------------
NV_STATUS UvmToolsCreateProcessAggregateCounters(UvmToolsSessionHandle   session,
                                                 void                   *counters_buffer,
                                                 UvmToolsCountersHandle *counters);

//------------------------------------------------------------------------------
// UvmToolsCreateProcessorCounters
//
// Creates the counters structure for tracking per-processor counters.
// These counters are disabled by default.
//
// Counters position follows the layout of the memory that UVM driver decides to
// use. To obtain particular counter value, user should perform consecutive
// atomic reads at a given buffer + offset address.
//
// It is not defined what is the initial value of a counter. User should rely on
// a difference between each snapshot.
//
// Arguments:
//     session: (INPUT)
//         Handle to the tools session.
//
//     counters_buffer : (INPUT)
//         User allocated buffer. Must be aligned to the OS's page aligned. Must
//         be large enough to hold all possible counter types. In practice, 4kB
//         system page should be sufficent. This memory gets pinned until
//         counters are destroyed.
//
//     processorUuid: (INPUT)
//         UUID of the physical GPU if the GPU is not SMC capable or SMC
//         enabled, the GPU instance UUID of the partition, or the CPU UUID of
//         the resource, for which counters will provide statistic data.
//
//     counters: (OUTPUT)
//         Handle to the created counters.
//
// Error codes:
//     NV_ERR_INSUFFICIENT_PERMISSIONS:
//         session handle does not refer to a valid tools session
//
//     NV_ERR_INSUFFICIENT_RESOURCES
//         There could be multiple reasons for this error. One would be that
//         it's not possible to allocate counters structure. Another would be
//         that either event_buffer or event_control memory couldn't be pinned
//         (e.g. because of OS limitation of pinnable memory)
//
//     NV_ERR_INVALID_ARGUMENT
//         processorUuid does not refer to any known resource in UVM driver
//
//------------------------------------------------------------------------------
NV_STATUS UvmToolsCreateProcessorCounters(UvmToolsSessionHandle   session,
                                          void                   *counters_buffer,
                                          const NvProcessorUuid  *processorUuid,
                                          UvmToolsCountersHandle *counters);

//------------------------------------------------------------------------------
// UvmToolsDestroyCounters
//
// Destroys all internal resources associated with this counters structure.
// It unpins the buffer provided in UvmToolsCreate*Counters. Counters structure
// also gest destroyed when corresponding session is destroyed.
//
// Arguments:
//     counters: (INPUT)
//         Handle to the counters structure.
//
// Error codes:
//     NV_ERR_INSUFFICIENT_PERMISSIONS:
//         State of the counters has been corrupted.
//
//     NV_ERR_INVALID_ARGUMENT:
//         Counters handle does not refer to a valid Counters structure.
//
//------------------------------------------------------------------------------
NV_STATUS UvmToolsDestroyCounters(UvmToolsCountersHandle counters);

//------------------------------------------------------------------------------
// UvmToolsEnableCounters
//
// This call enables certain counter types in the counters structure. Any
// counter type is considered listed if and only if its corresponding value is
// equal to 1 (in other words, bit is set). Disabled counter types listed in
// counterTypeFlags are going to be enabled. Already enabled counter types and
// counter types not listed in counterTypeFlags are not affected by this call.
//
// It is not an error to call this function multiple times with the same
// arguments.
//
// Arguments:
//     counters: (INPUT)
//         Handle to the counters structure.
//
//     counterTypeFlags: (INPUT)
//         This bit field specifies the counter types to be enabled.
//         For example, to enable faults number accounting and number of bytes
//         transferred into a given resource (or aggregate) pass flags
//         "UVM_COUNTER_ENABLE_FAULTS_NUMBER |
//          UVM_COUNTER_ENABLE_BYTES_TRANSFERRED_IN"
//
// Error codes:
//     NV_ERR_INSUFFICIENT_PERMISSIONS:
//         Counters structure mighe be corrupted (associated session is not
//         valid).
//
//     NV_ERR_INVALID_ARGUMENT:
//         Counters handle does not refer to a valid counters structure.
//------------------------------------------------------------------------------
NV_STATUS UvmToolsEnableCounters(UvmToolsCountersHandle counters,
                                 NvU64                  counterTypeFlags);

//------------------------------------------------------------------------------
// UvmToolsDisableCounters
//
// This call disables certain counter types in the counters structure. Any
// counter type is considered listed if and only if its corresponding value is
// equal to 1 (in other words, bit is set). Enabled counter types listed in
// counterTypeFlags are going to be disabled. Already disabled counter types and
// counter types not listed in counterTypeFlags are not affected by this call.
//
// It is not an error to call this function multiple times with the same
// arguments.
//
// Arguments:
//     counters: (INPUT)
//         Handle to the counters structure.
//
//     counterTypeFlags: (INPUT)
//         This bit field specifies the counter types to be disabled.
//         For example, to disable faults number accounting and number of bytes
//         transferred into a given resource (or aggregate) pass flags
//         "UVM_COUNTER_ENABLE_FAULTS_NUMBER |
//          UVM_COUNTER_ENABLE_BYTES_TRANSFERRED_IN"
//
// Error codes:
//     NV_ERR_INSUFFICIENT_PERMISSIONS:
//         Counters structure mighe be corrupted (associated session is not
//         valid).
//
//     NV_ERR_INVALID_ARGUMENT:
//         Counters handle does not refer to a valid counters structure.
//------------------------------------------------------------------------------
NV_STATUS UvmToolsDisableCounters(UvmToolsCountersHandle counters,
                                  NvU64                  counterTypeFlags);

//------------------------------------------------------------------------------
// UvmToolsReadProcessMemory
//
// Reads up to size bytes from a given target process's virtual address.
// If size is 0, function should successfully return size of the largest size
// that can be read starting at a given target process's virtual memory. This
// might be used to discover size of user's allocation.
//
// Upon successful execution and size greater than 0, user should have a copy of
// target's process memory in a given buffer. Result is unspecified in case of
// In-process scenario when targetVa address + size overlaps with buffer + size.
//
// This is essentially a UVM version of RM ctrl call
// NV83DE_CTRL_CMD_DEBUG_READ_MEMORY. For implementation constraints (and more
// information), please refer to the documentation:
// //sw/docs/resman/components/compute/UVM/subsystems/UVM_8_Tools_API_Design.docx
//
// Arguments:
//     session: (INPUT)
//         Handle to the tools session.
//
//     buffer: (INPUT)
//         User buffer (destination) address, where requested memory shall be
//         copied.
//
//     size: (INPUT)
//         Number of bytes requested to be copied. If user's buffer is not large
//         enough to fit size bytes, result is unspecified. If this is 0,
//         function should return largest chunk of memory available to read.
//
//     targetVa: (INPUT)
//         Target process's (source) address, from which memory should be
//         copied.
//
//     bytes_read: (OUTPUT)
//         Either number of bytes successfully read or the largest chunk of
//         memory available to read, depending on size parameter.
//
// Error codes:
//     NV_ERR_INSUFFICIENT_PERMISSIONS:
//         session handle does not refer to a valid tools session
//
//     NV_ERR_INVALID_ADDRESS:
//         UVM driver has no knowledge of targetVa address.
//
//     NV_ERR_INVALID_ARGUMENT:
//         Read spans more than a single target process allocation.
//
//------------------------------------------------------------------------------
NV_STATUS UvmToolsReadProcessMemory(UvmToolsSessionHandle  session,
                                    void                  *buffer,
                                    NvLength               size,
                                    void                  *targetVa,
                                    NvLength              *bytes_read);

//------------------------------------------------------------------------------
// UvmToolsWriteProcessMemory
//
// Writes up to size bytes from a given target process's virtual address.
// If size is 0, function should successfully return size of the largest size
// that can be written starting at a given target process's virtual address.
// This might be used to discover size of user's allocation.
//
// Upon successful execution and size greater than 0, target process should have
// a copy of buffer starting at targetVa address. Result is unspecified in case
// of In-process scenario when targetVa address + size overlaps with
// buffer + size.
//
// This is essentially a UVM version of RM ctrl call
// NV83DE_CTRL_CMD_DEBUG_READ_MEMORY. For implementation constraints (and more
// information), please refer to the documentation:
// //sw/docs/resman/components/compute/UVM/subsystems/UVM_8_Tools_API_Design.docx
//
// Arguments:
//     session: (INPUT)
//         Handle to the tools session.
//
//     buffer: (INPUT)
//         User buffer (source) address, from which requested memory shall be
//         copied.
//
//     size: (INPUT)
//         Number of bytes requested to be copied. If user's buffer is not large
//         enough to fit size bytes, result is unspecified. If this is 0,
//         function should return largest chunk of memory available to write.
//
//     targetVa: (INPUT)
//         Target process's (destination) address, where memory should be
//         copied.
//
//     bytes_read: (OUTPUT)
//         Either number of bytes successfully written or the largest chunk of
//         memory available to write, depending on size parameter.
//
// Error codes:
//     NV_ERR_INSUFFICIENT_PERMISSIONS:
//         session handle does not refer to a valid tools session
//
//     NV_ERR_INVALID_ADDRESS:
//         UVM driver has no knowledge of targetVa address.
//
//     NV_ERR_INVALID_ARGUMENT:
//         Write spans more than a single target process allocation.
//
//------------------------------------------------------------------------------
NV_STATUS UvmToolsWriteProcessMemory(UvmToolsSessionHandle  session,
                                     void                  *buffer,
                                     NvLength               size,
                                     void                  *targetVa,
                                     NvLength              *bytes_read);

//------------------------------------------------------------------------------
// UvmToolsGetProcessorUuidTable
//
// Populate a table with the UUIDs of all the currently registered processors
// in the target process. When a GPU is registered, it is added to the table.
// When a GPU is unregistered, it is removed. As long as a GPU remains
// registered, its index in the table does not change.
// Note that the index in the table corresponds to the processor ID reported
// in UvmEventEntry event records and that the table is not contiguously packed
// with non-zero UUIDs even with no GPU unregistrations.
//
// Arguments:
//     session: (INPUT)
//         Handle to the tools session.
//
//     version: (INPUT)
//         Requested version for the UUID table returned. The version must
//         match the requested version of the event queue created with
//         UvmToolsCreateEventQueue(). See UvmToolsEventQueueVersion.
//         If the version of the event queue does not match the version of the
//         UUID table, the behavior is undefined.
//
//     table: (OUTPUT)
//         Array of processor UUIDs, including the CPU's UUID which is always
//         at index zero. The number of elements in the array must be greater
//         or equal to UVM_MAX_PROCESSORS_V1 if the version is
//         UvmToolsEventQueueVersion_V1 and UVM_MAX_PROCESSORS if the version is
//         UvmToolsEventQueueVersion_V2.
//         The srcIndex and dstIndex fields of the UvmEventMigrationInfo struct
//         index this array. Unused indices will have a UUID of zero.
//         If version is UvmToolsEventQueueVersion_V1 then the reported UUID
//         will be that of the corresponding physical GPU, even if multiple SMC
//         partitions are registered under that physical GPU. If version is
//         UvmToolsEventQueueVersion_V2 then the reported UUID will be the GPU
//         instance UUID if SMC is enabled, otherwise it will be the UUID of
//         the physical GPU.
//
// Error codes:
//     NV_ERR_INVALID_ADDRESS:
//         writing to table failed.
//
//     NV_ERR_INVALID_ARGUMENT:
//         The version is not UvmToolsEventQueueVersion_V1 or
//         UvmToolsEventQueueVersion_V2.
//
//     NV_ERR_NOT_SUPPORTED:
//         The kernel is not able to support the requested version
//         (i.e., the UVM kernel driver is older and doesn't support
//         UvmToolsEventQueueVersion_V2).
//
//     NV_ERR_NO_MEMORY:
//         Internal memory allocation failed.
//------------------------------------------------------------------------------
#if UVM_API_REV_IS_AT_MOST(11)
NV_STATUS UvmToolsGetProcessorUuidTable(UvmToolsSessionHandle      session,
                                        UvmToolsEventQueueVersion  version,
                                        NvProcessorUuid           *table,
                                        NvLength                   table_size,
                                        NvLength                  *count);
#else
NV_STATUS UvmToolsGetProcessorUuidTable(UvmToolsSessionHandle     session,
                                        UvmToolsEventQueueVersion version,
                                        NvProcessorUuid          *table);
#endif

//------------------------------------------------------------------------------
// UvmToolsFlushEvents
//
// Some events, like migrations, which have end timestamps are not immediately
// submitted to queues when they are completed.  This call enqueues any
// completed but unenqueued events associated with the session.
//
// Arguments:
//     session: (INPUT)
//         Handle to the tools session.
//
// Error codes:
//     NV_ERR_INSUFFICIENT_PERMISSIONS:
//         Session handle does not refer to a valid session
//------------------------------------------------------------------------------
NV_STATUS UvmToolsFlushEvents(UvmToolsSessionHandle session);

#ifdef __cplusplus
}
#endif

#endif // _UVM_H_
