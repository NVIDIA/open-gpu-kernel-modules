/*******************************************************************************
    Copyright (c) 2015-2023 NVIDIA Corporation

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

#ifndef __UVM_RM_MEM_H__
#define __UVM_RM_MEM_H__

#include "uvm_forward_decl.h"
#include "uvm_processors.h"
#include "uvm_test_ioctl.h"
#include "uvm_hal_types.h"

typedef enum
{
    UVM_RM_MEM_TYPE_GPU,
    UVM_RM_MEM_TYPE_SYS,
} uvm_rm_mem_type_t;

// Abstraction for memory allocations done through the UVM-RM interface
struct uvm_rm_mem_struct
{
    // Type of the memory
    uvm_rm_mem_type_t type;

    // Mask of processors the memory is mapped on
    uvm_processor_mask_t mapped_on;

    // VA of the memory on the UVM internal address space of each processor. If
    // the memory has not been mapped on a given processor, the VA is zero.
    NvU64 vas[UVM_ID_MAX_PROCESSORS];

    // VA of the memory in the proxy address space of each processor. If
    // the memory has not been mapped on a given processor, the VA is zero.
    // If the memory is mapped on the proxy address space of a processor, then
    // it must be mapped on UVM's internal address space.
    //
    // This array is only allocated in SR-IOV heavy. It is sized, and indexed,
    // as the 'vas' array.
    NvU64 *proxy_vas;

    // The GPU the allocation originated from
    uvm_gpu_t *gpu_owner;

    // Size of the allocation
    NvLength size;
};

// Allocate memory of the given type and size in the GPU's UVM internal address
// space, and (in SR-IOV heavy) map it on the proxy address space as well.
//
// The GPU cannot be NULL and the memory is going to mapped on the GPU for the
// lifetime of the allocation. For sysmem allocations other GPUs can have a
// mapping created and removed dynamically with the uvm_rm_mem_(un)map_gpu()
// functions.
//
// Alignment affects only the GPU VA mapping. If gpu_alignment is 0, then 4K
// alignment is enforced.
//
// Locking:
//  - Internally acquires:
//    - RM API lock
//    - RM GPUs lock
NV_STATUS uvm_rm_mem_alloc(uvm_gpu_t *gpu,
                           uvm_rm_mem_type_t type,
                           NvLength size,
                           NvU64 gpu_alignment,
                           uvm_rm_mem_t **rm_mem_out);

// Free the memory.
// Clear all mappings and free the memory
//
// Locking same as uvm_rm_mem_alloc()
void uvm_rm_mem_free(uvm_rm_mem_t *rm_mem);

// Map/Unmap on the CPU
// Locking same as uvm_rm_mem_alloc()
NV_STATUS uvm_rm_mem_map_cpu(uvm_rm_mem_t *rm_mem);
void uvm_rm_mem_unmap_cpu(uvm_rm_mem_t *rm_mem);

// Shortcut for uvm_rm_mem_alloc() + uvm_rm_mem_map_cpu().
// The function fails and nothing is allocated if any of the intermediate steps
// fail.
//
// Locking same as uvm_rm_mem_alloc()
NV_STATUS uvm_rm_mem_alloc_and_map_cpu(uvm_gpu_t *gpu,
                                       uvm_rm_mem_type_t type,
                                       NvLength size,
                                       NvU64 gpu_alignment,
                                       uvm_rm_mem_t **rm_mem_out);

// Shortcut for uvm_rm_mem_alloc_and_map_cpu() + uvm_rm_mem_map_all_gpus()
// The function fails and nothing is allocated if any of the intermediate steps
// fail.
//
// Locking same as uvm_rm_mem_alloc()
NV_STATUS uvm_rm_mem_alloc_and_map_all(uvm_gpu_t *gpu,
                                       uvm_rm_mem_type_t type,
                                       NvLength size,
                                       NvU64 gpu_alignment,
                                       uvm_rm_mem_t **rm_mem_out);

// Map/Unmap on UVM's internal address space of a GPU. In SR-IOV heavy the
// operation is also applied on the GPU's proxy address space.
//
// Mapping/unmapping on the GPU owner, or mapping on an already mapped GPU, are
// no-ops. Mapping/unmapping on a GPU different from the owner is only supported
// for system memory.
//
// Locking same as uvm_rm_mem_alloc()
NV_STATUS uvm_rm_mem_map_gpu(uvm_rm_mem_t *rm_mem, uvm_gpu_t *gpu, NvU64 gpu_alignment);
void uvm_rm_mem_unmap_gpu(uvm_rm_mem_t *rm_mem, uvm_gpu_t *gpu);

// Map on UVM's internal address space of all GPUs retained by the UVM driver
// that do not yet have this allocation mapped. In SR-IOV heavy the memory is
// also mapped on the proxy address space of all GPUs.
//
// Locking same as uvm_rm_mem_alloc()
NV_STATUS uvm_rm_mem_map_all_gpus(uvm_rm_mem_t *rm_mem, NvU64 gpu_alignment);

// Get the CPU VA, GPU VA (UVM internal/kernel address space), or GPU (proxy
// address space)
void *uvm_rm_mem_get_cpu_va(uvm_rm_mem_t *rm_mem);
NvU64 uvm_rm_mem_get_gpu_uvm_va(uvm_rm_mem_t *rm_mem, uvm_gpu_t *gpu);
NvU64 uvm_rm_mem_get_gpu_proxy_va(uvm_rm_mem_t *rm_mem, uvm_gpu_t *gpu);

// Get the GPU VA of the given memory in UVM's internal address space (if the
// flag is false), or proxy address space (if flag is true).
uvm_gpu_address_t uvm_rm_mem_get_gpu_va(uvm_rm_mem_t *rm_mem, uvm_gpu_t *gpu, bool is_proxy_va_space);

// Query if the memory is mapped on the CPU, GPU (UVM internal/kernel address
// space), or GPU (proxy address space)
bool uvm_rm_mem_mapped_on_cpu(uvm_rm_mem_t *rm_mem);
bool uvm_rm_mem_mapped_on_gpu(uvm_rm_mem_t *rm_mem, uvm_gpu_t *gpu);
bool uvm_rm_mem_mapped_on_gpu_proxy(uvm_rm_mem_t *rm_mem, uvm_gpu_t *gpu);

#endif // __UVM_RM_MEM_H__
