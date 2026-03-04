/*******************************************************************************
    Copyright (c) 2016-2024 NVIDIA Corporation

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

#ifndef __UVM_MEM_H__
#define __UVM_MEM_H__

#include "uvm_forward_decl.h"
#include "uvm_processors.h"
#include "uvm_hal_types.h"
#include "uvm_pmm_gpu.h"
#include "uvm_range_allocator.h"

//
// This module provides an abstraction for UVM-managed allocations, both sysmem
// and vidmem, which can be mapped on GPUs in internal or user VA spaces or on
// the CPU, or accessed physically.
//
// As opposed to the uvm_rm_mem_* abstraction, this module has no dependencies
// on the UVM-RM interface and implements all the functionality on top of other
// UVM abstractions. Specifically, vidmem is allocated from PMM and sysmem is
// allocated directly from the kernel (in the future PMM will support sysmem as
// well and then this module can switch over). And GPU mappings are created
// through the page table range vector (uvm_page_table_range_vec_t) and CPU
// mappings (only sysmem) use vmap directly.
//
// The module currently allows the following:
//  - sysmem allocation and mapping on all GPUs and the CPU
//  - vidmem allocation and mapping on the GPU backing the allocation
//
// Additionally, helpers for accessing the allocations physically are provided,
// which allows skipping virtual mappings if not necessary (e.g. allocating a
// single CPU page and accessing it from the GPU).
//
// For internal mappings, GPU VA ranges used for mapping the allocations are
// allocated from a global range allocator (uvm_range_allocator_t) and are
// currently offset by a GPU specific offset (gpu->uvm_mem_va_base). This would
// change if the first limitation below is lifted and UVM can control the VA
// starting at 0. For user mappings, a fixed VA is provided externally.
//
// Allocation lifetimes:
// - Vidmem allocations cannot exceed the lifetime of the GPU on which they are
//   allocated (backing_gpu).
// - Sysmem allocations without a DMA owner have no lifetime restrictions.
// - Sysmem allocations with a DMA owner cannot exceed the lifetime of the
//   dma_owner GPU.
//
// Future additions:
//  - Per processor caching attributes (longer term, the envisioned use-case is
//    for GPU semaphore caching, which requires the first limitation below to be
//    lifted)
//
// Limitations:
//  - On Pascal+ limited to VAs over 40bit due to how the internal VA is shared
//    with RM. This implies it cannot be used for e.g. pushbuffer nor sempahores
//    currently. At some point in the future UVM should be able
//    to take full control of the VA (or at least the bottom 40bits of it)
//    and this limitation would be lifted. See comments around
//    gpu->rm_va_base for more details.
//  - Mapping vidmem on the CPU is only allowed on GPU-coherent systems. The
//    problem with lifting this limitation in other systems is that the BAR1
//    space (that's used for such mappings) is limited and controlled by RM and
//    may not be easy to interop with vidmem allocations from PMM.
//


// The size of the VA used for mapping uvm_mem_t allocations
// 128 GBs should be plenty for internal allocations and fits easily on all
// supported architectures.
#define UVM_MEM_VA_SIZE (128 * UVM_SIZE_1GB)

typedef struct
{
    // The GPU to allocate memory from, or NULL for sysmem.
    uvm_gpu_t *backing_gpu;

    // For DMA allocations, the accessing GPU needs to be known at alloc
    // time for sysmem allocations. Setting the DMA owner has consequences on
    // the lifetime of the allocation, which are described in the block
    // commment at the top of the file.
    // The mapping is bound to the allocation, hence, one can assume that the
    // mappings on dma_owner are done when uvm_mem_alloc() returns.
    uvm_gpu_t *dma_owner;

    // Size of the allocation, in bytes.
    // The only restriction is for it to be non-0.
    NvU64 size;

    // mm owning the memory allocation.
    // The mm is used to charge the mm's memory cgroup for the allocation.
    // If mm is NULL, the allocation will not be charged.
    struct mm_struct *mm;

    // Desired page size to use, in bytes.
    //
    // If this is a DMA allocation, the physical allocation chunk must be
    // equal to PAGE_SIZE
    //
    // If this is a CPU allocation, the physical allocation chunk has to be
    // aligned to PAGE_SIZE and the allocation will be mapped with the largest
    // PTEs possible on the GPUs. If set to UVM_PAGE_SIZE_DEFAULT, PAGE_SIZE
    // size will be used.
    //
    // For a GPU allocation, if set to UVM_PAGE_SIZE_DEFAULT, GPU mappings will
    // use the largest page size supported by the backing GPU which is not
    // larger than size. Otherwise, the desired page size will be used.
    //
    // CPU mappings will always use PAGE_SIZE, so the physical allocation chunk
    // has to be aligned to PAGE_SIZE.
    NvU64 page_size;

    // If true, the allocation is zeroed (scrubbed).
    bool zero;
} uvm_mem_alloc_params_t;

typedef struct
{
    uvm_prot_t protection;
    bool is_cacheable;
} uvm_mem_gpu_mapping_attrs_t;

// Information specific to allocations mapped in a user VA space.
typedef struct
{
    // Mask of processors the memory is virtually mapped on
    uvm_processor_mask_t mapped_on;

    // Page table ranges for all GPUs
    uvm_page_table_range_vec_t *range_vecs[UVM_ID_MAX_GPUS];

    uvm_va_space_t *va_space;

    // The VA to map the allocation at on all processors
    void *addr;
} uvm_mem_user_mapping_t;

struct uvm_mem_struct
{
    // The GPU the physical memory is allocated on. Or NULL for sysmem.
    //
    // For GPU allocations, the lifetime of the allocation cannot extend the
    // lifetime of the GPU. For CPU allocations there is no lifetime limitation.
    uvm_gpu_t *backing_gpu;

    // In Confidential Computing, the accessing GPU needs to be known at alloc
    // time for sysmem allocations.
    uvm_gpu_t *dma_owner;

    union
    {
        struct
        {
            uvm_gpu_chunk_t **chunks;
        } vidmem;

        struct
        {
            // Mask of processors the memory is physically mapped on.
            //
            // There is no equivalent mask for vidmem, because only the backing
            // GPU can physical access the memory
            //
            // TODO: Bug 3723779: Share DMA mappings within a single parent GPU
            uvm_processor_mask_t mapped_on_phys;

            struct page **pages;
            void **va;

            // Per GPU IOMMU mappings of the pages
            NvU64 *dma_addrs[UVM_ID_MAX_GPUS];
        } sysmem;
    };

    // Count of chunks (vidmem) or CPU pages (sysmem) above
    size_t chunks_count;

    // Size of each physical chunk (vidmem) or CPU page (sysmem)
    NvU64 chunk_size;

    // Size of the allocation
    NvU64 size;

    uvm_mem_user_mapping_t *user;

    // Information specific to allocations mapped in UVM internal VA space.
    struct
    {
        // Mask of processors the memory is virtually mapped on
        uvm_processor_mask_t mapped_on;

        // Page table ranges for all GPUs
        uvm_page_table_range_vec_t *range_vecs[UVM_ID_MAX_GPUS];

        // Range allocation for the GPU VA
        uvm_range_allocation_t range_alloc;

        // CPU address of the allocation if mapped on the CPU
        void *cpu_addr;
    } kernel;
};

NV_STATUS uvm_mem_global_init(void);
void uvm_mem_global_exit(void);

// Fill out attrs_out from attrs.  attrs_out must not be null.  attrs_out may be
// prepopulated with default values, which are not overwritten if the
// corresponding field in attrs has a default value.  The gpu corresponding to
// attrs->gpuUuid is optionally returned in gpu_out if it is not NULL.
//
// Returns an error if attrs is invalid.
NV_STATUS uvm_mem_translate_gpu_attributes(const UvmGpuMappingAttributes *attrs,
                                           uvm_va_space_t *va_space,
                                           uvm_gpu_t **gpu_out,
                                           uvm_mem_gpu_mapping_attrs_t *attrs_out);

uvm_chunk_sizes_mask_t uvm_mem_kernel_chunk_sizes(uvm_gpu_t *gpu);

// Size of all the physical allocations backing the given memory.
static inline NvU64 uvm_mem_physical_size(const uvm_mem_t *mem)
{
    NvU64 physical_size = mem->chunks_count * mem->chunk_size;

    UVM_ASSERT(mem->size <= physical_size);

    return physical_size;
}

// Returns true if the memory is physically contiguous in the
// [offset, offset + size) interval.
bool uvm_mem_is_physically_contiguous(uvm_mem_t *mem, NvU64 offset, NvU64 size);

// Allocate memory according to the given allocation parameters.
//
// In the case of sysmem, the memory is immediately physically accessible from
// the GPU DMA owner, if any. Vidmem is accessible from the GPU backing the
// allocation.
//
// Unless a specific page size is needed, or the physical pages need to be
// zeroed, the caller can use the appropriate uvm_mem_alloc* helper instead.
//
// If an error is returned, mem_out is guaranteed to not have been modified.
NV_STATUS uvm_mem_alloc(const uvm_mem_alloc_params_t *params, uvm_mem_t **mem_out);

// Clear all mappings and free the memory
void uvm_mem_free(uvm_mem_t *mem);

// Map/unmap sysmem for physical access on a GPU.
// A physical unmap implies a virtual user and kernel unmap i.e. it clears all
// mappings in the given GPU.
NV_STATUS uvm_mem_map_gpu_phys(uvm_mem_t *mem, uvm_gpu_t *gpu);
void uvm_mem_unmap_gpu_phys(uvm_mem_t *mem, uvm_gpu_t *gpu);

// Map/unmap on a user VA space.
//
// For GPU mappings, the caller passes the user VA space to map the allocation,
// the address to map at, and the mapping attributes. CPU mappings pass the user
// VA space, and the affected VM area; the memory is mapped at the VM area
// starting address.
//
// The user VA space and address values must be identical to those used in prior
// mappings (if any) on the same memory object.
NV_STATUS uvm_mem_map_gpu_user(uvm_mem_t *mem,
                               uvm_gpu_t *gpu,
                               uvm_va_space_t *user_va_space,
                               void *user_addr,
                               const uvm_mem_gpu_mapping_attrs_t *attrs);
NV_STATUS uvm_mem_map_cpu_user(uvm_mem_t *mem,
                               uvm_va_space_t *user_va_space,
                               struct vm_area_struct *vma);
void uvm_mem_unmap_gpu_user(uvm_mem_t *mem, uvm_gpu_t *gpu);
void uvm_mem_unmap_cpu_user(uvm_mem_t *mem);

// Map/unmap on UVM's internal VA space.
//
// TODO: Bug 1812419: uvm_mem_map_gpu_kernel should accept GPU mapping
// attributes the way uvm_mem_map_gpu_user does.
NV_STATUS uvm_mem_map_gpu_kernel(uvm_mem_t *mem, uvm_gpu_t *gpu);
NV_STATUS uvm_mem_map_cpu_kernel(uvm_mem_t *mem);
void uvm_mem_unmap_gpu_kernel(uvm_mem_t *mem, uvm_gpu_t *gpu);
void uvm_mem_unmap_cpu_kernel(uvm_mem_t *mem);

// Check if a user or kernel mapping exists on a given device.
bool uvm_mem_mapped_on_gpu_kernel(uvm_mem_t *mem, uvm_gpu_t *gpu);
bool uvm_mem_mapped_on_gpu_user(uvm_mem_t *mem, uvm_gpu_t *gpu);
bool uvm_mem_mapped_on_cpu_kernel(uvm_mem_t *mem);
bool uvm_mem_mapped_on_cpu_user(uvm_mem_t *mem);

// Get the CPU address
//
// The allocation has to be mapped on the CPU prior to calling this function.
void *uvm_mem_get_cpu_addr_kernel(uvm_mem_t *mem);

// Get the GPU VA
//
// The allocation has to be internally mapped on the given GPU prior to calling
// this function.
NvU64 uvm_mem_get_gpu_va_kernel(uvm_mem_t *mem, uvm_gpu_t *gpu);

// Helper for getting a virtual uvm_gpu_address_t
uvm_gpu_address_t uvm_mem_gpu_address_virtual_kernel(uvm_mem_t *mem, uvm_gpu_t *gpu);

// Helpers for getting both types of GPU physical addresses.
//
// Offset and size are used to return the address of the correct physical chunk
// and check that the allocation is physically contiguous for the given range.
uvm_gpu_phys_address_t uvm_mem_gpu_physical(uvm_mem_t *mem, uvm_gpu_t *gpu, NvU64 offset, NvU64 size);
uvm_gpu_address_t uvm_mem_gpu_address_physical(uvm_mem_t *mem, uvm_gpu_t *gpu, NvU64 offset, NvU64 size);

// Helper to get an address suitable for accessing_gpu (which may be the backing
// GPU) to access with CE.
uvm_gpu_address_t uvm_mem_gpu_address_copy(uvm_mem_t *mem, uvm_gpu_t *accessing_gpu, NvU64 offset, NvU64 size);

static bool uvm_mem_is_sysmem(uvm_mem_t *mem)
{
    return mem->backing_gpu == NULL;
}

static bool uvm_mem_is_vidmem(uvm_mem_t *mem)
{
    return !uvm_mem_is_sysmem(mem);
}

static bool uvm_mem_is_local_vidmem(uvm_mem_t *mem, uvm_gpu_t *gpu)
{
    return uvm_mem_is_vidmem(mem) && (mem->backing_gpu == gpu);
}

static bool uvm_mem_is_sysmem_dma(uvm_mem_t *mem)
{
    return uvm_mem_is_sysmem(mem) && !!mem->dma_owner;
}

// Helper for allocating sysmem using the default page size. The backing pages
// are not zeroed.
static NV_STATUS uvm_mem_alloc_sysmem(NvU64 size, struct mm_struct *mm, uvm_mem_t **mem_out)
{
    uvm_mem_alloc_params_t params = { 0 };
    params.size = size;
    params.backing_gpu = NULL;
    params.page_size = UVM_PAGE_SIZE_DEFAULT;
    params.mm = mm;

    return uvm_mem_alloc(&params, mem_out);
}

// Helper for allocating sysmem in DMA zone using the default page size. The
// backing pages are not zeroed.
static NV_STATUS uvm_mem_alloc_sysmem_dma(NvU64 size, uvm_gpu_t *dma_owner, struct mm_struct *mm, uvm_mem_t **mem_out)
{
    uvm_mem_alloc_params_t params = { 0 };
    params.size = size;
    params.backing_gpu = NULL;
    params.dma_owner = dma_owner;
    params.page_size = UVM_PAGE_SIZE_DEFAULT;
    params.mm = mm;

    return uvm_mem_alloc(&params, mem_out);
}

// Helper for allocating vidmem with the default page size. The backing pages
// are not zeroed.
static NV_STATUS uvm_mem_alloc_vidmem(NvU64 size, uvm_gpu_t *gpu, uvm_mem_t **mem_out)
{
    uvm_mem_alloc_params_t params = { 0 };
    params.size = size;
    params.backing_gpu = gpu;
    params.page_size = UVM_PAGE_SIZE_DEFAULT;

    return uvm_mem_alloc(&params, mem_out);
}

// Helper for allocating sysmem and mapping it on the CPU
static NV_STATUS uvm_mem_alloc_sysmem_and_map_cpu_kernel(NvU64 size, struct mm_struct *mm, uvm_mem_t **mem_out)
{
    NV_STATUS status;
    uvm_mem_t *mem;

    status = uvm_mem_alloc_sysmem(size, mm, &mem);
    if (status != NV_OK)
        return status;

    status = uvm_mem_map_cpu_kernel(mem);
    if (status != NV_OK) {
        uvm_mem_free(mem);
        return status;
    }

    *mem_out = mem;
    return NV_OK;
}

// Helper for allocating sysmem DMA and mapping it on the CPU. This is useful
// for certain systems where the main system memory is encrypted
// (e.g., AMD SEV) and cannot be read from IO devices unless specially
// allocated using the DMA APIs.
//
// See uvm_mem_alloc()
static NV_STATUS uvm_mem_alloc_sysmem_dma_and_map_cpu_kernel(NvU64 size,
                                                             uvm_gpu_t *gpu,
                                                             struct mm_struct *mm,
                                                             uvm_mem_t **mem_out)
{
    NV_STATUS status;
    uvm_mem_t *mem;

    status = uvm_mem_alloc_sysmem_dma(size, gpu, mm, &mem);
    if (status != NV_OK)
        return status;

    status = uvm_mem_map_cpu_kernel(mem);
    if (status != NV_OK) {
        uvm_mem_free(mem);
        return status;
    }

    *mem_out = mem;
    return NV_OK;
}

// Helper to map an allocation on the specified processors in the UVM VA space.
NV_STATUS uvm_mem_map_kernel(uvm_mem_t *mem, const uvm_processor_mask_t *mask);

#endif // __UVM_MEM_H__
