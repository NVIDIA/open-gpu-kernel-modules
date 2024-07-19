/*******************************************************************************
    Copyright (c) 2015-2024 NVIDIA Corporation

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

#ifndef __UVM_MMU_H__
#define __UVM_MMU_H__

#include "uvm_forward_decl.h"
#include "uvm_hal_types.h"
#include "uvm_pmm_gpu.h"
#include "uvm_types.h"
#include "uvm_common.h"
#include "uvm_tracker.h"
#include "uvm_test_ioctl.h"

// Used when the page size isn't known and should not matter.
#define UVM_PAGE_SIZE_AGNOSTIC 0

// Memory layout of UVM's kernel VA space.
// The following memory regions are not to scale. The memory layout is linear,
// i.e., no canonical form address conversion.
//
// Hopper-Blackwell:
// +----------------+ 128PB
// |                |
// |   (not used)*  | * See note(1)
// |                |
// ------------------
// |uvm_mem_t(128GB)| (uvm_mem_va_size)
// ------------------ 64PB + 384TB (uvm_mem_va_base)
// |                |
// |   (not used)   |
// |                |
// ------------------ 64PB + 33TB (UVM_GPU_MAX_PHYS_MEM)
// |     vidmem     |
// |  flat mapping  | ==> UVM_GPU_MAX_PHYS_MEM
// |  (up to 1TB)   |
// ------------------ 64PB + 32TB (flat_vidmem_va_base)
// |peer ident. maps|
// |32 * 1TB = 32TB | ==> NV_MAX_DEVICES * UVM_PEER_IDENTITY_VA_SIZE
// ------------------ 64PB
// |                |
// |  rm_mem(64PB)  | (rm_va_size)
// |                |
// +----------------+ 0 (rm_va_base)
//
// Pascal-Ada:
// +----------------+ 512TB
// |                |
// |   (not used)*  | * See note(1)
// |                |
// ------------------
// |uvm_mem_t(128GB)| (uvm_mem_va_size)
// ------------------ 384TB (uvm_mem_va_base)
// |     sysmem     |
// |  flat mapping  | ==> 1ULL << NV_CHIP_EXTENDED_SYSTEM_PHYSICAL_ADDRESS_BITS
// |  (up to 128TB) |
// ------------------ 256TB (flat_sysmem_va_base)
// |                |
// |   (not used)   |
// |                |
// ------------------ 161TB
// |     vidmem     |
// |  flat mapping  | ==> UVM_GPU_MAX_PHYS_MEM
// |  (up to 1TB)   |
// ------------------ 160TB (flat_vidmem_va_base)
// |peer ident. maps|
// |32 * 1TB = 32TB | ==> NV_MAX_DEVICES * UVM_PEER_IDENTITY_VA_SIZE
// ------------------ 128TB
// |                |
// | rm_mem(128TB)  | (rm_va_size)
// |                |
// +----------------+ 0 (rm_va_base)
//
// Maxwell:
// +----------------+ 1TB
// |                |
// |   (not used)   |
// |                |
// ------------------ 896GB
// |uvm_mem_t(128GB)| (uvm_mem_va_size)
// ------------------ 768GB (uvm_mem_va_base)
// |                |
// |   (not used)   |
// |                |
// ------------------ 128GB
// |                |
// | rm_mem(128GB)  | (rm_va_size)
// |                |
// +----------------+ 0 (rm_va_base)
//
// Note (1): This region is used in unit tests, see
// tests/uvm_mem_test.c:test_huge_pages().

// Maximum memory of any GPU.
#define UVM_GPU_MAX_PHYS_MEM (UVM_SIZE_1TB)

// The size of VA that should be reserved per peer identity mapping.
// This should be at least the maximum amount of memory of any GPU.
#define UVM_PEER_IDENTITY_VA_SIZE UVM_GPU_MAX_PHYS_MEM

// GPUs which support ATS perform a parallel lookup on both ATS and GMMU page
// tables. The ATS lookup can be disabled by setting a bit in the GMMU page
// tables. All GPUs which support ATS use the same mechanism (a bit in PDE1),
// and have the same PDE1 coverage (512MB).
//
// If the PTE format changes, this will need to move to the HAL.
#define UVM_GMMU_ATS_GRANULARITY (512ull * 1024 * 1024)

// This represents an allocation containing either a page table or page
// directory.
typedef struct
{
    uvm_gpu_phys_address_t addr;

    NvU64 size;
    union
    {
        struct page *page;
        uvm_gpu_chunk_t *chunk;
    } handle;
} uvm_mmu_page_table_alloc_t;

// This structure in general refers to a page directory
// although it is also used to represent a page table, in which case entries is
// not allocated.
typedef struct uvm_page_directory_struct uvm_page_directory_t;

struct uvm_page_directory_struct
{
    // parent directory
    uvm_page_directory_t *host_parent;

    // index of this entry in the parent directory
    NvU32 index_in_parent;

    // allocation that holds actual page table used by device
    uvm_mmu_page_table_alloc_t phys_alloc;

    // count of references to all entries
    NvU32 ref_count;

    // depth from the root
    NvU32 depth;

    // pointers to child directories on the host.
    // this array is variable length, so it needs to be last to allow it to
    // take up extra space
    uvm_page_directory_t *entries[];
};

enum
{
    UVM_MMU_PTE_FLAGS_NONE                     = 0,

    // data from the page pointed by the PTE may be cached incoherently by the
    // GPU L2. This option may result in accessing stale data. For vidmem
    // aperture, however, it is safe to use _CACHED, i.e., no memory request
    // accesses stale data because L2 is in the datapath of the GPU memory.
    UVM_MMU_PTE_FLAGS_CACHED                   = (1 << 0),

    // Disable access counters to the page pointed by the PTE.
    UVM_MMU_PTE_FLAGS_ACCESS_COUNTERS_DISABLED = (1 << 1),
    UVM_MMU_PTE_FLAGS_MASK                     = (1 << 2) - 1
};

struct uvm_mmu_mode_hal_struct
{
    // bit pattern of a valid PTE. flags is a bitwise-or of UVM_MMU_PTE_FLAGS_*.
    NvU64 (*make_pte)(uvm_aperture_t aperture, NvU64 address, uvm_prot_t prot, NvU64 flags);

    // bit pattern of a sked reflected PTE
    NvU64 (*make_sked_reflected_pte)(void);

    // bit pattern of sparse PTE
    // Sparse PTEs will indicate to MMU to route all reads and writes to the
    // debug pages. Therefore, accesses to sparse mappings do not generate
    // faults.
    NvU64 (*make_sparse_pte)(void);

    // Bit pattern of an "unmapped" PTE. The GPU MMU recognizes two flavors of
    // empty PTEs:
    // 1) Invalid: Bit pattern of all 0s. There is no HAL function for this.
    // 2) Unmapped: This pattern.
    //
    // The subtle difference is for big PTEs. Invalid big PTEs indicate to the
    // GPU MMU that there might be 4k PTEs present instead, and that those 4k
    // entries should be read and cached. Unmapped big PTEs indicate that there
    // are no 4k PTEs below the unmapped big entry, so MMU should stop its walk
    // and not cache any 4k entries which may be in memory.
    //
    // This is an optimization which reduces TLB pressure, reduces the number of
    // TLB invalidates we must issue, and means we don't have to initialize the
    // 4k PTEs which are covered by big PTEs since the MMU will never read them.
    NvU64 (*unmapped_pte)(NvU64 page_size);

    // Bit pattern used for debug purposes to clobber PTEs which ought to be
    // unused. In practice this will generate a PRIV violation or a physical
    // memory out-of-range error so we can immediately identify bad PTE usage.
    NvU64 (*poisoned_pte)(void);

    // Write a PDE bit-pattern to entry based on the data in allocs (which may
    // point to two items for dual PDEs).
    // Any of allocs are allowed to be NULL, in which case they are to be
    // treated as empty. make_pde() uses dir and child_index to compute the
    // mapping PDE VA. On ATS-enabled systems, we may set PDE's PCF as
    // ATS_ALLOWED or ATS_NOT_ALLOWED based on the mapping PDE VA, even for
    // invalid/clean PDE entries.
    void (*make_pde)(void *entry, uvm_mmu_page_table_alloc_t **allocs, uvm_page_directory_t *dir, NvU32 child_index);

    // size of an entry in a directory/table.  Generally either 8 or 16 bytes.
    // (in the case of Pascal dual PDEs)
    NvLength (*entry_size)(NvU32 depth);

    // Two for dual PDEs, one otherwise.
    NvU32 (*entries_per_index)(NvU32 depth);

    // For dual PDEs, this is ether 1 or 0, depending on the page size.
    // This is used to index the host copy only. GPU PDEs are always entirely
    // re-written using make_pde.
    NvLength (*entry_offset)(NvU32 depth, NvU64 page_size);

    // number of virtual address bits used to index the directory/table at a
    // given depth
    NvU32 (*index_bits)(NvU32 depth, NvU64 page_size);

    // total number of bits that represent the virtual address space
    NvU32 (*num_va_bits)(void);

    // the size, in bytes, of a directory/table at a given depth.
    NvLength (*allocation_size)(NvU32 depth, NvU64 page_size);

    // the depth which corresponds to the page tables
    NvU32 (*page_table_depth)(NvU64 page_size);

    // bitwise-or of supported page sizes
    NvU64 (*page_sizes)(void);
};

struct uvm_page_table_range_struct
{
    uvm_page_directory_t *table;
    NvU32 start_index;
    NvU32 entry_count;
    NvU64 page_size;
};

typedef enum
{
    UVM_PAGE_TREE_TYPE_USER,
    UVM_PAGE_TREE_TYPE_KERNEL,
    UVM_PAGE_TREE_TYPE_COUNT
} uvm_page_tree_type_t;

struct uvm_page_tree_struct
{
    uvm_mutex_t lock;
    uvm_gpu_t *gpu;
    uvm_page_directory_t *root;
    uvm_mmu_mode_hal_t *hal;
    uvm_page_tree_type_t type;
    NvU64 big_page_size;

    // Pointer to the GPU VA space containing the page tree.
    // This pointer is set only for page trees of type
    // UVM_PAGE_TREE_TYPE_USER and is used to get to the
    // VA space mm_struct when CGroup accounting is enabled.
    uvm_gpu_va_space_t *gpu_va_space;

    // Location of the physical pages backing the page directories and tables in
    // the tree. If the location is UVM_APERTURE_SYS, all the pages are in
    // sysmem. If it is UVM_APERTURE_VID, all the pages are in vidmem unless
    // location_sys_fallback is true, in which case a sysmem fallback is allowed
    // so the pages can be placed in either aperture.
    uvm_aperture_t location;
    bool location_sys_fallback;

    struct
    {
        // Page table where all entries are invalid small-page entries.
        uvm_mmu_page_table_alloc_t ptes_invalid_4k;

        // PDE0 where all big-page entries are invalid, and small-page entries
        // point to ptes_invalid_4k.
        // pde0 is used on Pascal+ GPUs, i.e., they have the same PDE format.
        uvm_page_directory_t *pde0;
    } map_remap;

    // On ATS-enabled systems where the CPU VA width is smaller than the GPU VA
    // width, the excess address range is set with ATS_NOT_ALLOWED on all  leaf
    // PDEs covering that range. We have at most 2 no_ats_ranges, due to
    // canonical form address systems.
    uvm_page_table_range_t no_ats_ranges[2];

    // Tracker for all GPU operations on the tree
    uvm_tracker_t tracker;
};

// A vector of page table ranges
struct uvm_page_table_range_vec_struct
{
    // The tree the range vector is from
    uvm_page_tree_t *tree;

    // Start of the covered VA in bytes, always page_size aligned
    NvU64 start;

    // Size of the covered VA in bytes, always page_size aligned
    NvU64 size;

    // Page size used for all the page table ranges
    NvU64 page_size;

    // Page table ranges covering the VA
    uvm_page_table_range_t *ranges;

    // Number of allocated ranges
    size_t range_count;
};

// Called at module init
NV_STATUS uvm_mmu_init(void);

// Initialize MMU-specific information for the GPU/sub-processor
void uvm_mmu_init_gpu_chunk_sizes(uvm_parent_gpu_t *parent_gpu);
void uvm_mmu_init_gpu_peer_addresses(uvm_gpu_t *gpu);

// Create a page tree structure and allocate the root directory. location
// behavior:
// - UVM_APERTURE_VID       Force all page table allocations into vidmem
// - UVM_APERTURE_SYS       Force all page table allocations into sysmem
// - UVM_APERTURE_DEFAULT   Let the implementation decide
//
// On failure the caller must call uvm_page_tree_deinit() iff tree_out->root is
// valid.
NV_STATUS uvm_page_tree_init(uvm_gpu_t *gpu,
                             uvm_gpu_va_space_t *gpu_va_space,
                             uvm_page_tree_type_t type,
                             NvU64 big_page_size,
                             uvm_aperture_t location,
                             uvm_page_tree_t *tree_out);

// Destroy the root directory of the page tree.
// This function asserts that there are no other elements left in the tree.
// All PTEs should have been put with uvm_page_tree_put_ptes before this
// function is called.
// uvm_page_tree_deinit() must be called when tree->root is valid.
void uvm_page_tree_deinit(uvm_page_tree_t *tree);

// Returns the range of PTEs that correspond to [start, start + size).
// It is the caller's responsibility to ensure that this range falls within the
// same table. This function asserts that start and size are multiples of
// page_size because sub-page alignment information is not represented in
// *range PTE ranges may overlap. Each get_ptes (and friends) call must be
// paired with a put_ptes call on the same range. get_ptes may overwrite
// existing PTE values, so do not call get_ptes on overlapping ranges with
// the same page size without an intervening put_ptes. To duplicate a subset of
// an existing range or change the size of an existing range, use
// uvm_page_table_range_get_upper() and/or uvm_page_table_range_shrink().
NV_STATUS uvm_page_tree_get_ptes(uvm_page_tree_t *tree,
                                 NvU64 page_size,
                                 NvU64 start,
                                 NvU64 size,
                                 uvm_pmm_alloc_flags_t pmm_flags,
                                 uvm_page_table_range_t *range);

// Same as uvm_page_tree_get_ptes(), but doesn't synchronize the GPU work.
//
// All pending operations can be waited on with uvm_page_tree_wait().
NV_STATUS uvm_page_tree_get_ptes_async(uvm_page_tree_t *tree,
                                       NvU64 page_size,
                                       NvU64 start,
                                       NvU64 size,
                                       uvm_pmm_alloc_flags_t pmm_flags,
                                       uvm_page_table_range_t *range);

// Returns a single-entry page table range for the addresses passed.
// The size parameter must be a page size supported by this tree.
// This is equivalent to calling uvm_page_tree_get_ptes() with size equal to
// page_size.
NV_STATUS uvm_page_tree_get_entry(uvm_page_tree_t *tree,
                                  NvU64 page_size,
                                  NvU64 start,
                                  uvm_pmm_alloc_flags_t pmm_flags,
                                  uvm_page_table_range_t *single);

// For a single-entry page table range, write the PDE (which could be a dual
// PDE) to the GPU.
// This is useful when the GPU currently has a PTE but that entry can also
// contain a PDE.
// It is an error to call this on a PTE-only range.
// The parameter single can otherwise be any single-entry range such as those
// allocated from get_entry() or get_ptes().
// This function performs no TLB invalidations.
void uvm_page_tree_write_pde(uvm_page_tree_t *tree, uvm_page_table_range_t *single, uvm_push_t *push);

// For a single-entry page table range, clear the PDE on the GPU.
// It is an error to call this on a PTE-only range.
// The parameter single can otherwise be any single-entry range such as those
// allocated from get_entry() or get_ptes().
// This function performs no TLB invalidations.
void uvm_page_tree_clear_pde(uvm_page_tree_t *tree, uvm_page_table_range_t *single, uvm_push_t *push);

// For a single-entry page table range, allocate a sibling table for a given
// page size. This sibling entry will be inserted into the host cache, but will
// not be written to the GPU page tree. uvm_page_tree_write_pde() can be used to
// submit entries to the GPU page tree, using single. The range returned refers
// to all the PTEs in the sibling directory directly.
//
// It is the caller's responsibility to initialize the returned table before
// calling uvm_page_tree_write_pde.
NV_STATUS uvm_page_tree_alloc_table(uvm_page_tree_t *tree,
                                    NvU64 page_size,
                                    uvm_pmm_alloc_flags_t pmm_flags,
                                    uvm_page_table_range_t *single,
                                    uvm_page_table_range_t *children);

// Gets PTEs from the upper portion of an existing range and returns them in a
// new range. num_upper_pages is the number of pages that should be in the new
// range. It must be in the range [1, existing->entry_count].
//
// The existing range is unmodified.
void uvm_page_table_range_get_upper(uvm_page_tree_t *tree,
                                    uvm_page_table_range_t *existing,
                                    uvm_page_table_range_t *upper,
                                    NvU32 num_upper_pages);

// Releases range's references on its upper pages to shrink the range down to
// to new_page_count, which must be >= range->entry_count. The range start
// remains the same.
//
// new_page_count is allowed to be 0, in which case this is equivalent to
// calling uvm_page_tree_put_ptes.
void uvm_page_table_range_shrink(uvm_page_tree_t *tree, uvm_page_table_range_t *range, NvU32 new_page_count);

// Releases the range of PTEs.
// It is the caller's responsibility to ensure that the empty PTE patterns have
// already been written in the range passed to the function.
void uvm_page_tree_put_ptes(uvm_page_tree_t *tree, uvm_page_table_range_t *range);

// Same as uvm_page_tree_put_ptes(), but doesn't synchronize the GPU work.
//
// All pending operations can be waited on with uvm_page_tree_wait().
void uvm_page_tree_put_ptes_async(uvm_page_tree_t *tree, uvm_page_table_range_t *range);

// Synchronize any pending operations
NV_STATUS uvm_page_tree_wait(uvm_page_tree_t *tree);

// Returns the physical allocation that contains the root directory.
static uvm_mmu_page_table_alloc_t *uvm_page_tree_pdb(uvm_page_tree_t *tree)
{
    return &tree->root->phys_alloc;
}

// Initialize a page table range vector covering the specified VA range
// [start, start + size)
//
// This splits the VA in the minimum amount of page table ranges required to
// cover it and calls uvm_page_tree_get_ptes() for each of them.
//
// The pmm_flags are only used if PTEs are allocated from vidmem
//
// Start and size are in bytes and need to be page_size aligned.
NV_STATUS uvm_page_table_range_vec_init(uvm_page_tree_t *tree,
                                        NvU64 start,
                                        NvU64 size,
                                        NvU64 page_size,
                                        uvm_pmm_alloc_flags_t pmm_flags,
                                        uvm_page_table_range_vec_t *range_vec);

// Allocate and initialize a page table range vector.
// The pmm_flags are only used if PTEs are allocated from vidmem
NV_STATUS uvm_page_table_range_vec_create(uvm_page_tree_t *tree,
                                          NvU64 start,
                                          NvU64 size,
                                          NvU64 page_size,
                                          uvm_pmm_alloc_flags_t pmm_flags,
                                          uvm_page_table_range_vec_t **range_vec_out);

// Split a page table range vector in two using new_end as the split point.
// new_range_vec will contain the upper portion of range_vec, starting at
// new_end + 1.
//
// new_end + 1 is required to be within the address range of range_vec and be
// aligned to range_vec's page_size.
//
// On failure, the original range vector is left unmodified.
NV_STATUS uvm_page_table_range_vec_split_upper(uvm_page_table_range_vec_t *range_vec,
                                               NvU64 new_end,
                                               uvm_page_table_range_vec_t *new_range_vec);

// Deinitialize a page table range vector and set all fields to 0.
//
// Put all the PTEs that the range vector covered.
void uvm_page_table_range_vec_deinit(uvm_page_table_range_vec_t *range_vec);

// Deinitialize and free a page table range vector.
void uvm_page_table_range_vec_destroy(uvm_page_table_range_vec_t *range_vec);

// A PTE making function used by uvm_page_table_range_vec_write_ptes()
//
// The function gets called for each page_size aligned offset within the VA
// covered by the range vector and is supposed to return the desired PTE bits
// for each offset.
// The caller_data pointer is what the caller passed in as caller_data to
// uvm_page_table_range_vec_write_ptes().
typedef NvU64 (*uvm_page_table_range_pte_maker_t)(uvm_page_table_range_vec_t *range_vec,
                                                  NvU64 offset,
                                                  void *caller_data);

// Write all PTEs covered by the range vector using the given PTE making
// function.
//
// After writing all the PTEs a TLB invalidate operation is performed including
// the passed in tlb_membar.
//
// See comments about uvm_page_table_range_pte_maker_t for details about the
// PTE making callback.
NV_STATUS uvm_page_table_range_vec_write_ptes(uvm_page_table_range_vec_t *range_vec,
                                              uvm_membar_t tlb_membar,
                                              uvm_page_table_range_pte_maker_t pte_maker,
                                              void *caller_data);

// Set all PTEs covered by the range vector to an empty PTE
//
// After clearing all PTEs a TLB invalidate is performed including the given
// membar.
NV_STATUS uvm_page_table_range_vec_clear_ptes(uvm_page_table_range_vec_t *range_vec, uvm_membar_t tlb_membar);

// Create peer identity mappings
NV_STATUS uvm_mmu_create_peer_identity_mappings(uvm_gpu_t *gpu, uvm_gpu_t *peer);

// Destroy peer identity mappings
void uvm_mmu_destroy_peer_identity_mappings(uvm_gpu_t *gpu, uvm_gpu_t *peer);

// Create or initialize flat mappings to vidmem or system memories. The
// mappings may cover the entire physical address space, or just parts of it.
// The mappings are used for engines that do not support physical addressing.
NV_STATUS uvm_mmu_create_flat_mappings(uvm_gpu_t *gpu);

// Destroy the flat mappings created by uvm_mmu_create_flat_mappings().
void uvm_mmu_destroy_flat_mappings(uvm_gpu_t *gpu);

// Returns true if a static flat mapping covering the entire vidmem is required
// for the given GPU.
bool uvm_mmu_parent_gpu_needs_static_vidmem_mapping(uvm_parent_gpu_t *parent_gpu);

// Returns true if an on-demand flat mapping partially covering the GPU memory
// is required for the given device.
bool uvm_mmu_parent_gpu_needs_dynamic_vidmem_mapping(uvm_parent_gpu_t *parent_gpu);

// Returns true if an on-demand flat mapping partially covering the sysmem
// address space is required for the given device.
bool uvm_mmu_parent_gpu_needs_dynamic_sysmem_mapping(uvm_parent_gpu_t *parent_gpu);

// Add or remove a (linear) mapping to the root chunk containing the given
// chunk. The mapping is added to UVM's internal address space in the GPU
// associated with the chunk. The virtual address associated with a chunk
// physical address can be queried via uvm_gpu_address_virtual_from_vidmem_phys.
//
// Because the root chunk mapping is referenced counted, a map call may not add
// any actual GPU mappings. Similarly, an unmap call may not remove any GPU
// mappings. The reference counting scheme is based on the size of the input
// chunk requiring that, for a given root chunk, the combined unmap size matches
// the combined map size. For example, the same chunk can be mapped multiple
// times, but then it must be unmapped the same number of times. And if an
// already mapped chunk is split into multiple subchunks, each subchunk is
// expected to be unmapped once.
//
// The map and unmap functions are synchronous. A tracker can be passed to
// the unmap routine to express any input dependencies.
NV_STATUS uvm_mmu_chunk_map(uvm_gpu_chunk_t *chunk);
void uvm_mmu_chunk_unmap(uvm_gpu_chunk_t *chunk, uvm_tracker_t *tracker);

// Map a system physical address interval. The mapping is added to UVM's
// internal address space in the given GPU. The resulting virtual address can be
// queried via uvm_parent_gpu_address_virtual_from_sysmem_phys.
//
// The mapping persists until GPU deinitialization, such that no unmap
// functionality is exposed. The map operation is synchronous, and internally
// uses a large mapping granularity that in the common case exceeds the input
// size.
//
// The input address must be a GPU address as returned by
// uvm_parent_gpu_map_cpu_pages for the given GPU.
NV_STATUS uvm_mmu_sysmem_map(uvm_gpu_t *gpu, NvU64 pa, NvU64 size);

static NvU64 uvm_mmu_page_tree_entries(uvm_page_tree_t *tree, NvU32 depth, NvU64 page_size)
{
    return 1ull << tree->hal->index_bits(depth, page_size);
}

static NvU64 uvm_mmu_pde_coverage(uvm_page_tree_t *tree, NvU64 page_size)
{
    NvU32 depth = tree->hal->page_table_depth(page_size);
    return uvm_mmu_page_tree_entries(tree, depth, page_size) * page_size;
}

// Page sizes supported by the GPU. Use uvm_mmu_biggest_page_size() to retrieve
// the largest page size supported in a given system, which considers the GMMU
// and vMMU page sizes and segment sizes.
static bool uvm_mmu_page_size_supported(uvm_page_tree_t *tree, NvU64 page_size)
{
    UVM_ASSERT_MSG(is_power_of_2(page_size), "0x%llx\n", page_size);

    return (tree->hal->page_sizes() & page_size) != 0;
}

static NvU64 uvm_mmu_biggest_page_size_up_to(uvm_page_tree_t *tree, NvU64 max_page_size)
{
    NvU64 gpu_page_sizes = tree->hal->page_sizes();
    NvU64 smallest_gpu_page_size = gpu_page_sizes & ~(gpu_page_sizes - 1);
    NvU64 page_sizes;
    NvU64 page_size;

    UVM_ASSERT_MSG(is_power_of_2(max_page_size), "0x%llx\n", max_page_size);

    if (max_page_size < smallest_gpu_page_size)
        return 0;

    // Calculate the supported page sizes that are not larger than the max
    page_sizes = gpu_page_sizes & (max_page_size | (max_page_size - 1));

    // And pick the biggest one of them
    page_size = 1ULL << __fls(page_sizes);

    UVM_ASSERT_MSG(uvm_mmu_page_size_supported(tree, page_size), "page_size 0x%llx", page_size);

    return page_size;
}

static NvU32 uvm_mmu_pte_size(uvm_page_tree_t *tree, NvU64 page_size)
{
    return tree->hal->entry_size(tree->hal->page_table_depth(page_size));
}

static NvU64 uvm_page_table_range_size(uvm_page_table_range_t *range)
{
    return ((NvU64)range->entry_count) * range->page_size;
}

// Get the physical address of the entry at entry_index within the range
// (counted from range->start_index).
static uvm_gpu_phys_address_t uvm_page_table_range_entry_address(uvm_page_tree_t *tree,
                                                                 uvm_page_table_range_t *range,
                                                                 size_t entry_index)
{
    NvU32 entry_size = uvm_mmu_pte_size(tree, range->page_size);
    uvm_gpu_phys_address_t entry = range->table->phys_alloc.addr;

    UVM_ASSERT(entry_index < range->entry_count);

    entry.address += (range->start_index + entry_index) * entry_size;
    return entry;
}

static uvm_aperture_t uvm_page_table_range_aperture(uvm_page_table_range_t *range)
{
    return range->table->phys_alloc.addr.aperture;
}

// Given a GPU or CPU physical address that refers to pages tables, retrieve an
// address suitable for CE writes to those page tables. This should be used
// instead of uvm_gpu_address_copy because PTE writes are used to bootstrap the
// various flat virtual mappings, so we usually ensure that PTE writes work even
// if virtual mappings are required for other accesses. This is only needed when
// CE has system-wide physical addressing restrictions.
uvm_gpu_address_t uvm_mmu_gpu_address(uvm_gpu_t *gpu, uvm_gpu_phys_address_t phys_addr);

NV_STATUS uvm_test_invalidate_tlb(UVM_TEST_INVALIDATE_TLB_PARAMS *params, struct file *filp);

#endif
