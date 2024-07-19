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

#ifndef __UVM_HAL_H__
#define __UVM_HAL_H__

#include "uvm_types.h"
#include "uvm_common.h"
#include "uvm_forward_decl.h"
#include "uvm_hal_types.h"
#include "uvm_push.h"
#include "uvm_gpu.h"
#include "uvm_test_ioctl.h"

// A dummy method validation that always returns true; it can be used to skip
// CE/Host/SW method validations for a given architecture
bool uvm_hal_method_is_valid_stub(uvm_push_t *push, NvU32 method_address, NvU32 method_data);

typedef void (*uvm_hal_init_t)(uvm_push_t *push);
void uvm_hal_maxwell_ce_init(uvm_push_t *push);
void uvm_hal_maxwell_host_init_noop(uvm_push_t *push);
void uvm_hal_pascal_host_init(uvm_push_t *push);
void uvm_hal_maxwell_sec2_init_noop(uvm_push_t *push);
void uvm_hal_hopper_sec2_init(uvm_push_t *push);

// Host method validation
typedef bool (*uvm_hal_host_method_is_valid)(uvm_push_t *push, NvU32 method_address, NvU32 method_data);
bool uvm_hal_ampere_host_method_is_valid(uvm_push_t *push, NvU32 method_address, NvU32 method_data);

// SW method validation
typedef bool (*uvm_hal_host_sw_method_is_valid)(uvm_push_t *push, NvU32 method_address, NvU32 method_data);
bool uvm_hal_ampere_host_sw_method_is_valid(uvm_push_t *push, NvU32 method_address, NvU32 method_data);

// Wait for idle
typedef void (*uvm_hal_wait_for_idle_t)(uvm_push_t *push);
void uvm_hal_maxwell_host_wait_for_idle(uvm_push_t *push);

// Membar SYS
typedef void (*uvm_hal_membar_sys_t)(uvm_push_t *push);
void uvm_hal_maxwell_host_membar_sys(uvm_push_t *push);
void uvm_hal_pascal_host_membar_sys(uvm_push_t *push);

// Membar GPU
typedef void (*uvm_hal_membar_gpu_t)(uvm_push_t *push);
void uvm_hal_pascal_host_membar_gpu(uvm_push_t *push);

// Put a noop in the pushbuffer of the given size in bytes.
// The size needs to be a multiple of 4.
typedef void (*uvm_hal_noop_t)(uvm_push_t *push, NvU32 size);
void uvm_hal_maxwell_host_noop(uvm_push_t *push, NvU32 size);

// Host-generated interrupt method. This will generate a call to
// uvm_isr_top_half_entry.
//
// This is a non-stalling interrupt, which means that it's fire-and-forget. Host
// will not stall method processing nor stop channel switching, which means that
// we cannot directly identify in software which channel generated the
// interrupt.
//
// We must set up software state before pushing the interrupt, and check any
// possible interrupt condition on receiving an interrupt callback.
typedef void (*uvm_hal_interrupt_t)(uvm_push_t *push);
void uvm_hal_maxwell_host_interrupt(uvm_push_t *push);

// Issue a TLB invalidate applying to all VAs in a PDB.
//
// The PTE caches (TLBs) are always invalidated. The PDE caches for all VAs in
// the PDB are invalidated from the specified depth down to the PTEs. This
// allows for optimizations if the caller isn't writing all levels of the PDEs.
// Depth follows the MMU code convention where depth 0 is the top level and here
// means to invalidate everything. See uvm_pascal_mmu.c for an example of depth
// mapping to HW PDE levels. Notably 2M PTEs are considered PDEs as far as the
// TLBs are concerned and hence on Pascal the depth needs to be at most 3 for
// them to be included in the invalidation.
//
// If the membar parameter is not UVM_MEMBAR_NONE, the specified membar is
// performed logically after the TLB invalidate such that all physical memory
// accesses using the old translations are ordered to the scope of the membar.
typedef void (*uvm_hal_host_tlb_invalidate_all_t)(uvm_push_t *push,
                                                  uvm_gpu_phys_address_t pdb,
                                                  NvU32 depth,
                                                  uvm_membar_t membar);
void uvm_hal_maxwell_host_tlb_invalidate_all_a16f(uvm_push_t *push,
                                                  uvm_gpu_phys_address_t pdb,
                                                  NvU32 depth,
                                                  uvm_membar_t membar);
void uvm_hal_maxwell_host_tlb_invalidate_all_b06f(uvm_push_t *push,
                                                  uvm_gpu_phys_address_t pdb,
                                                  NvU32 depth,
                                                  uvm_membar_t membar);
void uvm_hal_pascal_host_tlb_invalidate_all(uvm_push_t *push,
                                            uvm_gpu_phys_address_t pdb,
                                            NvU32 depth,
                                            uvm_membar_t membar);
void uvm_hal_turing_host_tlb_invalidate_all(uvm_push_t *push,
                                            uvm_gpu_phys_address_t pdb,
                                            NvU32 depth,
                                            uvm_membar_t membar);
void uvm_hal_ampere_host_tlb_invalidate_all(uvm_push_t *push,
                                            uvm_gpu_phys_address_t pdb,
                                            NvU32 depth,
                                            uvm_membar_t membar);
void uvm_hal_hopper_host_tlb_invalidate_all(uvm_push_t *push,
                                            uvm_gpu_phys_address_t pdb,
                                            NvU32 depth,
                                            uvm_membar_t membar);
void uvm_hal_blackwell_host_tlb_invalidate_all(uvm_push_t *push,
                                               uvm_gpu_phys_address_t pdb,
                                               NvU32 depth,
                                               uvm_membar_t membar);

// Issue a TLB invalidate applying to the specified VA range in a PDB.
//
// The PTE caches (TLBs) for each page size aligned VA within the VA range
// are always invalidated. The PDE caches covering the specified VA
// range in the PDB are invalidated from the specified depth down to the PTEs.
// Specifying the depth allows for optimizations if the caller isn't writing all
// levels of the PDEs. Specifying the page size allows for optimizations if
// the caller can guarantee caches for smaller page sizes don't need to be
// invalidated.
//
// Depth follows the MMU code convention where depth 0 is the top level and here
// means to invalidate all levels. See uvm_pascal_mmu.c for an example of depth
// mapping to HW PDE levels. Notably 2M PTEs are considered PDEs as far as the
// TLBs are concerned and hence on Pascal the depth needs to be at most 3 for
// them to be included in the invalidation.
//
// If the membar parameter is not UVM_MEMBAR_NONE, the specified membar is
// performed logically after the TLB invalidate such that all physical memory
// accesses using the old translations are ordered to the scope of the membar.
//
// Note that this can end up pushing a lot of methods for big ranges so it's
// better not to use it directly. Instead, uvm_tlb_batch* APIs should be used
// that automatically switch between targeted VA invalidates and invalidate all.
typedef void (*uvm_hal_host_tlb_invalidate_va_t)(uvm_push_t *push,
                                                 uvm_gpu_phys_address_t pdb,
                                                 NvU32 depth,
                                                 NvU64 base,
                                                 NvU64 size,
                                                 NvU64 page_size,
                                                 uvm_membar_t membar);
void uvm_hal_maxwell_host_tlb_invalidate_va(uvm_push_t *push,
                                            uvm_gpu_phys_address_t pdb,
                                            NvU32 depth,
                                            NvU64 base,
                                            NvU64 size,
                                            NvU64 page_size,
                                            uvm_membar_t membar);
void uvm_hal_pascal_host_tlb_invalidate_va(uvm_push_t *push,
                                           uvm_gpu_phys_address_t pdb,
                                           NvU32 depth,
                                           NvU64 base,
                                           NvU64 size,
                                           NvU64 page_size,
                                           uvm_membar_t membar);
void uvm_hal_volta_host_tlb_invalidate_va(uvm_push_t *push,
                                          uvm_gpu_phys_address_t pdb,
                                          NvU32 depth,
                                          NvU64 base,
                                          NvU64 size,
                                          NvU64 page_size,
                                          uvm_membar_t membar);
void uvm_hal_turing_host_tlb_invalidate_va(uvm_push_t *push,
                                           uvm_gpu_phys_address_t pdb,
                                           NvU32 depth,
                                           NvU64 base,
                                           NvU64 size,
                                           NvU64 page_size,
                                           uvm_membar_t membar);
void uvm_hal_ampere_host_tlb_invalidate_va(uvm_push_t *push,
                                           uvm_gpu_phys_address_t pdb,
                                           NvU32 depth,
                                           NvU64 base,
                                           NvU64 size,
                                           NvU64 page_size,
                                           uvm_membar_t membar);
void uvm_hal_hopper_host_tlb_invalidate_va(uvm_push_t *push,
                                           uvm_gpu_phys_address_t pdb,
                                           NvU32 depth,
                                           NvU64 base,
                                           NvU64 size,
                                           NvU64 page_size,
                                           uvm_membar_t membar);
void uvm_hal_blackwell_host_tlb_invalidate_va(uvm_push_t *push,
                                              uvm_gpu_phys_address_t pdb,
                                              NvU32 depth,
                                              NvU64 base,
                                              NvU64 size,
                                              NvU64 page_size,
                                              uvm_membar_t membar);

typedef void (*uvm_hal_host_tlb_invalidate_test_t)(uvm_push_t *push,
                                                   uvm_gpu_phys_address_t pdb,
                                                   UVM_TEST_INVALIDATE_TLB_PARAMS *params);
void uvm_hal_maxwell_host_tlb_invalidate_test(uvm_push_t *push,
                                              uvm_gpu_phys_address_t pdb,
                                              UVM_TEST_INVALIDATE_TLB_PARAMS *params);
void uvm_hal_pascal_host_tlb_invalidate_test(uvm_push_t *push,
                                             uvm_gpu_phys_address_t pdb,
                                             UVM_TEST_INVALIDATE_TLB_PARAMS *params);
void uvm_hal_turing_host_tlb_invalidate_test(uvm_push_t *push,
                                             uvm_gpu_phys_address_t pdb,
                                             UVM_TEST_INVALIDATE_TLB_PARAMS *params);
void uvm_hal_ampere_host_tlb_invalidate_test(uvm_push_t *push,
                                             uvm_gpu_phys_address_t pdb,
                                             UVM_TEST_INVALIDATE_TLB_PARAMS *params);
void uvm_hal_hopper_host_tlb_invalidate_test(uvm_push_t *push,
                                             uvm_gpu_phys_address_t pdb,
                                             UVM_TEST_INVALIDATE_TLB_PARAMS *params);
void uvm_hal_blackwell_host_tlb_invalidate_test(uvm_push_t *push,
                                                uvm_gpu_phys_address_t pdb,
                                                UVM_TEST_INVALIDATE_TLB_PARAMS *params);

// By default all semaphore release operations include a membar sys before the
// operation. This can be affected by using UVM_PUSH_FLAG_NEXT_* flags with
// uvm_push_set_flag().
typedef void (*uvm_hal_semaphore_release_t)(uvm_push_t *push, NvU64 gpu_va, NvU32 payload);
void uvm_hal_maxwell_host_semaphore_release(uvm_push_t *push, NvU64 gpu_va, NvU32 payload);
void uvm_hal_maxwell_ce_semaphore_release(uvm_push_t *push, NvU64 gpu_va, NvU32 payload);
void uvm_hal_maxwell_sec2_semaphore_release_unsupported(uvm_push_t *push, NvU64 gpu_va, NvU32 payload);
void uvm_hal_pascal_ce_semaphore_release(uvm_push_t *push, NvU64 gpu_va, NvU32 payload);
void uvm_hal_volta_ce_semaphore_release(uvm_push_t *push, NvU64 gpu_va, NvU32 payload);
void uvm_hal_turing_host_semaphore_release(uvm_push_t *push, NvU64 gpu_va, NvU32 payload);
void uvm_hal_hopper_sec2_semaphore_release(uvm_push_t *push, NvU64 gpu_va, NvU32 payload);
void uvm_hal_hopper_ce_semaphore_release(uvm_push_t *push, NvU64 gpu_va, NvU32 payload);
void uvm_hal_hopper_host_semaphore_release(uvm_push_t *push, NvU64 gpu_va, NvU32 payload);

// Release a semaphore including a timestamp at the specific GPU VA.
//
// This operation writes 16 bytes of memory and the VA needs to be 16-byte
// aligned. The value of the released payload is unspecified and shouldn't be
// relied on, only the timestamp should be of interest.
typedef void (*uvm_hal_semaphore_timestamp_t)(uvm_push_t *push, NvU64 gpu_va);
void uvm_hal_maxwell_ce_semaphore_timestamp(uvm_push_t *push, NvU64 gpu_va);
void uvm_hal_pascal_ce_semaphore_timestamp(uvm_push_t *push, NvU64 gpu_va);
void uvm_hal_volta_ce_semaphore_timestamp(uvm_push_t *push, NvU64 gpu_va);
void uvm_hal_hopper_ce_semaphore_timestamp(uvm_push_t *push, NvU64 gpu_va);

void uvm_hal_maxwell_host_semaphore_timestamp(uvm_push_t *push, NvU64 gpu_va);
void uvm_hal_volta_host_semaphore_timestamp(uvm_push_t *push, NvU64 gpu_va);
void uvm_hal_hopper_host_semaphore_timestamp(uvm_push_t *push, NvU64 gpu_va);

void uvm_hal_maxwell_sec2_semaphore_timestamp_unsupported(uvm_push_t *push, NvU64 gpu_va);
void uvm_hal_hopper_sec2_semaphore_timestamp_unsupported(uvm_push_t *push, NvU64 gpu_va);

typedef void (*uvm_hal_semaphore_acquire_t)(uvm_push_t *push, NvU64 gpu_va, NvU32 payload);
void uvm_hal_maxwell_host_semaphore_acquire(uvm_push_t *push, NvU64 gpu_va, NvU32 payload);
void uvm_hal_turing_host_semaphore_acquire(uvm_push_t *push, NvU64 gpu_va, NvU32 payload);
void uvm_hal_hopper_host_semaphore_acquire(uvm_push_t *push, NvU64 gpu_va, NvU32 payload);

typedef void (*uvm_hal_host_set_gpfifo_entry_t)(NvU64 *fifo_entry,
                                                NvU64 pushbuffer_va,
                                                NvU32 pushbuffer_length,
                                                uvm_gpfifo_sync_t sync_flag);
void uvm_hal_maxwell_host_set_gpfifo_entry(NvU64 *fifo_entry,
                                           NvU64 pushbuffer_va,
                                           NvU32 pushbuffer_length,
                                           uvm_gpfifo_sync_t sync_flag);
void uvm_hal_turing_host_set_gpfifo_entry(NvU64 *fifo_entry,
                                          NvU64 pushbuffer_va,
                                          NvU32 pushbuffer_length,
                                          uvm_gpfifo_sync_t sync_flag);
void uvm_hal_hopper_host_set_gpfifo_entry(NvU64 *fifo_entry,
                                          NvU64 pushbuffer_va,
                                          NvU32 pushbuffer_length,
                                          uvm_gpfifo_sync_t sync_flag);

typedef void (*uvm_hal_host_set_gpfifo_noop_t)(NvU64 *fifo_entry);
void uvm_hal_maxwell_host_set_gpfifo_noop(NvU64 *fifo_entry);

typedef void (*uvm_hal_host_set_gpfifo_pushbuffer_segment_base_t)(NvU64 *fifo_entry, NvU64 pushbuffer_va);
void uvm_hal_maxwell_host_set_gpfifo_pushbuffer_segment_base_unsupported(NvU64 *fifo_entry, NvU64 pushbuffer_va);
void uvm_hal_hopper_host_set_gpfifo_pushbuffer_segment_base(NvU64 *fifo_entry, NvU64 pushbuffer_va);

typedef void (*uvm_hal_host_write_gpu_put_t)(uvm_channel_t *channel, NvU32 gpu_put);
void uvm_hal_maxwell_host_write_gpu_put(uvm_channel_t *channel, NvU32 gpu_put);
void uvm_hal_volta_host_write_gpu_put(uvm_channel_t *channel, NvU32 gpu_put);

// Return the current GPU time in nanoseconds
typedef NvU64 (*uvm_hal_get_time_t)(uvm_gpu_t *gpu);
NvU64 uvm_hal_maxwell_get_time(uvm_gpu_t *gpu);

// Internal helpers used by the CE hal
// Used to handle the offset encoding differences between architectures
typedef void (*uvm_hal_ce_offset_out_t)(uvm_push_t *push, NvU64 offset);
void uvm_hal_maxwell_ce_offset_out(uvm_push_t *push, NvU64 offset);
void uvm_hal_pascal_ce_offset_out(uvm_push_t *push, NvU64 offset);
void uvm_hal_hopper_ce_offset_out(uvm_push_t *push, NvU64 offset);

typedef void (*uvm_hal_ce_offset_in_out_t)(uvm_push_t *push, NvU64 offset_in, NvU64 offset_out);
void uvm_hal_maxwell_ce_offset_in_out(uvm_push_t *push, NvU64 offset_in, NvU64 offset_out);
void uvm_hal_pascal_ce_offset_in_out(uvm_push_t *push, NvU64 offset_in, NvU64 offset_out);
void uvm_hal_hopper_ce_offset_in_out(uvm_push_t *push, NvU64 offset_in, NvU64 offset_out);

typedef NvU32 (*uvm_hal_ce_phys_mode_t)(uvm_push_t *push, uvm_gpu_address_t dst, uvm_gpu_address_t src);
NvU32 uvm_hal_maxwell_ce_phys_mode(uvm_push_t *push, uvm_gpu_address_t dst, uvm_gpu_address_t src);
NvU32 uvm_hal_ampere_ce_phys_mode(uvm_push_t *push, uvm_gpu_address_t dst, uvm_gpu_address_t src);

typedef NvU32 (*uvm_hal_ce_plc_mode_t)(void);
NvU32 uvm_hal_maxwell_ce_plc_mode(void);
NvU32 uvm_hal_ampere_ce_plc_mode_c7b5(void);

typedef NvU32 (*uvm_hal_ce_memcopy_type_t)(uvm_gpu_address_t dst, uvm_gpu_address_t src);
NvU32 uvm_hal_maxwell_ce_memcopy_copy_type(uvm_gpu_address_t dst, uvm_gpu_address_t src);
NvU32 uvm_hal_hopper_ce_memcopy_copy_type(uvm_gpu_address_t dst, uvm_gpu_address_t src);

// CE method validation
typedef bool (*uvm_hal_ce_method_is_valid)(uvm_push_t *push, NvU32 method_address, NvU32 method_data);
bool uvm_hal_ampere_ce_method_is_valid_c6b5(uvm_push_t *push, NvU32 method_address, NvU32 method_data);

// Memcopy validation.
// The validation happens at the start of the memcopy (uvm_hal_memcopy_t)
// execution. Use uvm_hal_ce_memcopy_is_valid_stub to skip the validation for
// a given architecture.
typedef bool (*uvm_hal_ce_memcopy_is_valid)(uvm_push_t *push, uvm_gpu_address_t dst, uvm_gpu_address_t src);
bool uvm_hal_ce_memcopy_is_valid_stub(uvm_push_t *push, uvm_gpu_address_t dst, uvm_gpu_address_t src);
bool uvm_hal_ampere_ce_memcopy_is_valid_c6b5(uvm_push_t *push, uvm_gpu_address_t dst, uvm_gpu_address_t src);
bool uvm_hal_hopper_ce_memcopy_is_valid(uvm_push_t *push, uvm_gpu_address_t dst, uvm_gpu_address_t src);

// Patching of the memcopy source; if not needed for a given architecture use
// the (empty) uvm_hal_ce_memcopy_patch_src_stub implementation
typedef void (*uvm_hal_ce_memcopy_patch_src)(uvm_push_t *push, uvm_gpu_address_t *src);
void uvm_hal_ce_memcopy_patch_src_stub(uvm_push_t *push, uvm_gpu_address_t *src);
void uvm_hal_ampere_ce_memcopy_patch_src_c6b5(uvm_push_t *push, uvm_gpu_address_t *src);

// Memcopy size bytes from src to dst.
//
// By default all CE transfer operations include a membar sys after the
// operation and are not pipelined. This can be affected by using
// UVM_PUSH_FLAG_NEXT_CE_* flags with uvm_push_set_flag().
typedef void (*uvm_hal_memcopy_t)(uvm_push_t *push, uvm_gpu_address_t dst, uvm_gpu_address_t src, size_t size);
void uvm_hal_maxwell_ce_memcopy(uvm_push_t *push, uvm_gpu_address_t dst, uvm_gpu_address_t src, size_t size);
void uvm_hal_volta_ce_memcopy(uvm_push_t *push, uvm_gpu_address_t dst, uvm_gpu_address_t src, size_t size);

// Simple wrapper for uvm_hal_memcopy_t with both addresses being virtual
typedef void (*uvm_hal_memcopy_v_to_v_t)(uvm_push_t *push, NvU64 dst, NvU64 src, size_t size);
void uvm_hal_maxwell_ce_memcopy_v_to_v(uvm_push_t *push, NvU64 dst, NvU64 src, size_t size);

// Memset validation.
// The validation happens at the start of the memset (uvm_hal_memset_*_t)
// execution. Use uvm_hal_ce_memset_is_valid_stub to skip the validation for
// a given architecture.
typedef bool (*uvm_hal_ce_memset_is_valid)(uvm_push_t *push,
                                           uvm_gpu_address_t dst,
                                           size_t num_elements,
                                           size_t element_size);
bool uvm_hal_ce_memset_is_valid_stub(uvm_push_t *push, uvm_gpu_address_t dst, size_t num_elements, size_t element_size);
bool uvm_hal_ampere_ce_memset_is_valid_c6b5(uvm_push_t *push,
                                            uvm_gpu_address_t dst,
                                            size_t num_elements,
                                            size_t element_size);
bool uvm_hal_hopper_ce_memset_is_valid(uvm_push_t *push,
                                       uvm_gpu_address_t dst,
                                       size_t num_elements,
                                       size_t element_size);

// Memset size bytes at dst to a given N-byte input value.
//
// Size has to be a multiple of the element size. For example, the size passed
// to uvm_hal_memset_4_t must be a multiple of 4 bytes.
//
// By default all CE transfer operations include a membar sys after the
// operation and are not pipelined. This can be affected by using
// UVM_PUSH_FLAG_NEXT_CE_* flags with uvm_push_set_flag().
typedef void (*uvm_hal_memset_1_t)(uvm_push_t *push, uvm_gpu_address_t dst, NvU8 value, size_t size);
typedef void (*uvm_hal_memset_4_t)(uvm_push_t *push, uvm_gpu_address_t dst, NvU32 value, size_t size);
typedef void (*uvm_hal_memset_8_t)(uvm_push_t *push, uvm_gpu_address_t dst, NvU64 value, size_t size);

// Simple wrapper for uvm_hal_memset_4_t with the address being virtual.
typedef void (*uvm_hal_memset_v_4_t)(uvm_push_t *push, NvU64 dst_va, NvU32 value, size_t size);

void uvm_hal_maxwell_ce_memset_1(uvm_push_t *push, uvm_gpu_address_t dst, NvU8 value, size_t size);
void uvm_hal_maxwell_ce_memset_4(uvm_push_t *push, uvm_gpu_address_t dst, NvU32 value, size_t size);
void uvm_hal_maxwell_ce_memset_8(uvm_push_t *push, uvm_gpu_address_t dst, NvU64 value, size_t size);
void uvm_hal_maxwell_ce_memset_v_4(uvm_push_t *push, NvU64 dst_va, NvU32 value, size_t size);

void uvm_hal_volta_ce_memset_1(uvm_push_t *push, uvm_gpu_address_t dst, NvU8 value, size_t size);
void uvm_hal_volta_ce_memset_4(uvm_push_t *push, uvm_gpu_address_t dst, NvU32 value, size_t size);
void uvm_hal_volta_ce_memset_8(uvm_push_t *push, uvm_gpu_address_t dst, NvU64 value, size_t size);

void uvm_hal_hopper_ce_memset_1(uvm_push_t *push, uvm_gpu_address_t dst, NvU8 value, size_t size);
void uvm_hal_hopper_ce_memset_4(uvm_push_t *push, uvm_gpu_address_t dst, NvU32 value, size_t size);
void uvm_hal_hopper_ce_memset_8(uvm_push_t *push, uvm_gpu_address_t dst, NvU64 value, size_t size);

// Encrypts the contents of the source buffer into the destination buffer, up to
// the given size. The authentication tag of the encrypted contents is written
// to auth_tag, so it can be verified later on by a decrypt operation.
//
// The addressing modes of the destination and authentication tag addresses
// should match. If the addressing mode is physical, then the address apertures
// should also match.
typedef void (*uvm_hal_ce_encrypt_t)(uvm_push_t *push,
                                     uvm_gpu_address_t dst,
                                     uvm_gpu_address_t src,
                                     NvU32 size,
                                     uvm_gpu_address_t auth_tag);

// Decrypts the contents of the source buffer into the destination buffer, up to
// the given size. The method also verifies the integrity of the encrypted
// buffer by calculating its authentication tag, and comparing it with the one
// provided as argument.
//
// The addressing modes of the source and authentication tag addresses should
// match. If the addressing mode is physical, then the address apertures should
// also match.
typedef void (*uvm_hal_ce_decrypt_t)(uvm_push_t *push,
                                     uvm_gpu_address_t dst,
                                     uvm_gpu_address_t src,
                                     NvU32 size,
                                     uvm_gpu_address_t auth_tag);

void uvm_hal_maxwell_ce_encrypt_unsupported(uvm_push_t *push,
                                            uvm_gpu_address_t dst,
                                            uvm_gpu_address_t src,
                                            NvU32 size,
                                            uvm_gpu_address_t auth_tag);
void uvm_hal_maxwell_ce_decrypt_unsupported(uvm_push_t *push,
                                            uvm_gpu_address_t dst,
                                            uvm_gpu_address_t src,
                                            NvU32 size,
                                            uvm_gpu_address_t auth_tag);
void uvm_hal_hopper_ce_encrypt(uvm_push_t *push,
                               uvm_gpu_address_t dst,
                               uvm_gpu_address_t src,
                               NvU32 size,
                               uvm_gpu_address_t auth_tag);
void uvm_hal_hopper_ce_decrypt(uvm_push_t *push,
                               uvm_gpu_address_t dst,
                               uvm_gpu_address_t src,
                               NvU32 size,
                               uvm_gpu_address_t auth_tag);

// Increments the semaphore by 1, or resets to 0 if the incremented value would
// exceed the payload.
//
// By default all CE semaphore operations include a membar sys before the
// semaphore operation. This can be affected by using UVM_PUSH_FLAG_NEXT_CE_*
// flags with uvm_push_set_flag().
typedef void (*uvm_hal_semaphore_reduction_inc_t)(uvm_push_t *push, NvU64 gpu_va, NvU32 payload);
void uvm_hal_maxwell_ce_semaphore_reduction_inc(uvm_push_t *push, NvU64 gpu_va, NvU32 payload);
void uvm_hal_pascal_ce_semaphore_reduction_inc(uvm_push_t *push, NvU64 gpu_va, NvU32 payload);
void uvm_hal_volta_ce_semaphore_reduction_inc(uvm_push_t *push, NvU64 gpu_va, NvU32 payload);
void uvm_hal_hopper_ce_semaphore_reduction_inc(uvm_push_t *push, NvU64 gpu_va, NvU32 payload);

// Initialize GPU architecture dependent properties
typedef void (*uvm_hal_arch_init_properties_t)(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_maxwell_arch_init_properties(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_pascal_arch_init_properties(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_volta_arch_init_properties(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_turing_arch_init_properties(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_ampere_arch_init_properties(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_ada_arch_init_properties(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_hopper_arch_init_properties(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_blackwell_arch_init_properties(uvm_parent_gpu_t *parent_gpu);

// Retrieve the page-tree HAL for a given big page size
typedef uvm_mmu_mode_hal_t *(*uvm_hal_lookup_mode_hal_t)(NvU64 big_page_size);
typedef void (*uvm_hal_mmu_enable_prefetch_faults_t)(uvm_parent_gpu_t *parent_gpu);
typedef void (*uvm_hal_mmu_disable_prefetch_faults_t)(uvm_parent_gpu_t *parent_gpu);
uvm_mmu_mode_hal_t *uvm_hal_mmu_mode_maxwell(NvU64 big_page_size);
uvm_mmu_mode_hal_t *uvm_hal_mmu_mode_pascal(NvU64 big_page_size);
uvm_mmu_mode_hal_t *uvm_hal_mmu_mode_volta(NvU64 big_page_size);
uvm_mmu_mode_hal_t *uvm_hal_mmu_mode_turing(NvU64 big_page_size);
uvm_mmu_mode_hal_t *uvm_hal_mmu_mode_ampere(NvU64 big_page_size);
uvm_mmu_mode_hal_t *uvm_hal_mmu_mode_hopper(NvU64 big_page_size);
uvm_mmu_mode_hal_t *uvm_hal_mmu_mode_blackwell(NvU64 big_page_size);
void uvm_hal_maxwell_mmu_enable_prefetch_faults_unsupported(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_maxwell_mmu_disable_prefetch_faults_unsupported(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_pascal_mmu_enable_prefetch_faults(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_pascal_mmu_disable_prefetch_faults(uvm_parent_gpu_t *parent_gpu);

typedef NvU16 (*uvm_hal_mmu_client_id_to_utlb_id_t)(NvU16 client_id);
NvU16 uvm_hal_maxwell_mmu_client_id_to_utlb_id_unsupported(NvU16 client_id);
NvU16 uvm_hal_pascal_mmu_client_id_to_utlb_id(NvU16 client_id);
NvU16 uvm_hal_volta_mmu_client_id_to_utlb_id(NvU16 client_id);
NvU16 uvm_hal_ampere_mmu_client_id_to_utlb_id(NvU16 client_id);
NvU16 uvm_hal_hopper_mmu_client_id_to_utlb_id(NvU16 client_id);
NvU16 uvm_hal_blackwell_mmu_client_id_to_utlb_id(NvU16 client_id);

// Replayable faults
typedef void (*uvm_hal_enable_replayable_faults_t)(uvm_parent_gpu_t *parent_gpu);
typedef void (*uvm_hal_disable_replayable_faults_t)(uvm_parent_gpu_t *parent_gpu);
typedef void (*uvm_hal_clear_replayable_faults_t)(uvm_parent_gpu_t *parent_gpu, NvU32 get);
typedef NvU32 (*uvm_hal_fault_buffer_read_put_t)(uvm_parent_gpu_t *parent_gpu);
typedef NvU32 (*uvm_hal_fault_buffer_read_get_t)(uvm_parent_gpu_t *parent_gpu);
typedef void (*uvm_hal_fault_buffer_write_get_t)(uvm_parent_gpu_t *parent_gpu, NvU32 get);
typedef NvU8 (*uvm_hal_fault_buffer_get_ve_id_t)(NvU16 mmu_engine_id, uvm_mmu_engine_type_t mmu_engine_type);
typedef uvm_mmu_engine_type_t (*uvm_hal_fault_buffer_get_mmu_engine_type_t)(NvU16 mmu_engine_id,
                                                                            uvm_fault_client_type_t client_type,
                                                                            NvU16 client_id);

// Parse the replayable entry at the given buffer index. This also clears the
// valid bit of the entry in the buffer.
typedef NV_STATUS (*uvm_hal_fault_buffer_parse_replayable_entry_t)(uvm_parent_gpu_t *gpu,
                                                                   NvU32 index,
                                                                   uvm_fault_buffer_entry_t *buffer_entry);

NV_STATUS uvm_hal_maxwell_fault_buffer_parse_replayable_entry_unsupported(uvm_parent_gpu_t *parent_gpu,
                                                                          NvU32 index,
                                                                          uvm_fault_buffer_entry_t *buffer_entry);

NV_STATUS uvm_hal_pascal_fault_buffer_parse_replayable_entry(uvm_parent_gpu_t *parent_gpu,
                                                             NvU32 index,
                                                             uvm_fault_buffer_entry_t *buffer_entry);

NV_STATUS uvm_hal_volta_fault_buffer_parse_replayable_entry(uvm_parent_gpu_t *parent_gpu,
                                                            NvU32 index,
                                                            uvm_fault_buffer_entry_t *buffer_entry);

typedef bool (*uvm_hal_fault_buffer_entry_is_valid_t)(uvm_parent_gpu_t *parent_gpu, NvU32 index);
typedef void (*uvm_hal_fault_buffer_entry_clear_valid_t)(uvm_parent_gpu_t *parent_gpu, NvU32 index);
typedef NvU32 (*uvm_hal_fault_buffer_entry_size_t)(uvm_parent_gpu_t *parent_gpu);
typedef void (*uvm_hal_fault_buffer_replay_t)(uvm_push_t *push, uvm_fault_replay_type_t type);
typedef uvm_fault_type_t (*uvm_hal_fault_buffer_get_fault_type_t)(const NvU32 *fault_entry);
typedef void (*uvm_hal_fault_cancel_global_t)(uvm_push_t *push, uvm_gpu_phys_address_t instance_ptr);
typedef void (*uvm_hal_fault_cancel_targeted_t)(uvm_push_t *push,
                                                uvm_gpu_phys_address_t instance_ptr,
                                                NvU32 gpc_id,
                                                NvU32 client_id);

void uvm_hal_maxwell_enable_replayable_faults_unsupported(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_maxwell_disable_replayable_faults_unsupported(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_maxwell_clear_replayable_faults_unsupported(uvm_parent_gpu_t *parent_gpu, NvU32 get);
NvU32 uvm_hal_maxwell_fault_buffer_read_put_unsupported(uvm_parent_gpu_t *parent_gpu);
NvU32 uvm_hal_maxwell_fault_buffer_read_get_unsupported(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_maxwell_fault_buffer_write_get_unsupported(uvm_parent_gpu_t *parent_gpu, NvU32 index);
NvU8 uvm_hal_maxwell_fault_buffer_get_ve_id_unsupported(NvU16 mmu_engine_id, uvm_mmu_engine_type_t mmu_engine_type);
uvm_mmu_engine_type_t uvm_hal_maxwell_fault_buffer_get_mmu_engine_type_unsupported(NvU16 mmu_engine_id,
                                                                                   uvm_fault_client_type_t client_type,
                                                                                   NvU16 client_id);
uvm_fault_type_t uvm_hal_maxwell_fault_buffer_get_fault_type_unsupported(const NvU32 *fault_entry);

void uvm_hal_pascal_enable_replayable_faults(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_pascal_disable_replayable_faults(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_pascal_clear_replayable_faults(uvm_parent_gpu_t *parent_gpu, NvU32 get);
NvU32 uvm_hal_pascal_fault_buffer_read_put(uvm_parent_gpu_t *parent_gpu);
NvU32 uvm_hal_pascal_fault_buffer_read_get(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_pascal_fault_buffer_write_get(uvm_parent_gpu_t *parent_gpu, NvU32 index);

uvm_fault_type_t uvm_hal_pascal_fault_buffer_get_fault_type(const NvU32 *fault_entry);

NvU32 uvm_hal_volta_fault_buffer_read_put(uvm_parent_gpu_t *parent_gpu);
NvU32 uvm_hal_volta_fault_buffer_read_get(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_volta_fault_buffer_write_get(uvm_parent_gpu_t *parent_gpu, NvU32 index);
NvU8 uvm_hal_volta_fault_buffer_get_ve_id(NvU16 mmu_engine_id, uvm_mmu_engine_type_t mmu_engine_type);
uvm_mmu_engine_type_t uvm_hal_volta_fault_buffer_get_mmu_engine_type(NvU16 mmu_engine_id,
                                                                     uvm_fault_client_type_t client_type,
                                                                     NvU16 client_id);

uvm_fault_type_t uvm_hal_volta_fault_buffer_get_fault_type(const NvU32 *fault_entry);

void uvm_hal_turing_disable_replayable_faults(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_turing_clear_replayable_faults(uvm_parent_gpu_t *parent_gpu, NvU32 get);
uvm_mmu_engine_type_t uvm_hal_turing_fault_buffer_get_mmu_engine_type(NvU16 mmu_engine_id,
                                                                      uvm_fault_client_type_t client_type,
                                                                      NvU16 client_id);

uvm_mmu_engine_type_t uvm_hal_ampere_fault_buffer_get_mmu_engine_type(NvU16 mmu_engine_id,
                                                                      uvm_fault_client_type_t client_type,
                                                                      NvU16 client_id);

NvU8 uvm_hal_hopper_fault_buffer_get_ve_id(NvU16 mmu_engine_id, uvm_mmu_engine_type_t mmu_engine_type);
uvm_mmu_engine_type_t uvm_hal_hopper_fault_buffer_get_mmu_engine_type(NvU16 mmu_engine_id,
                                                                      uvm_fault_client_type_t client_type,
                                                                      NvU16 client_id);

uvm_mmu_engine_type_t uvm_hal_blackwell_fault_buffer_get_mmu_engine_type(NvU16 mmu_engine_id,
                                                                         uvm_fault_client_type_t client_type,
                                                                         NvU16 client_id);
uvm_fault_type_t uvm_hal_blackwell_fault_buffer_get_fault_type(const NvU32 *fault_entry);

bool uvm_hal_maxwell_fault_buffer_entry_is_valid_unsupported(uvm_parent_gpu_t *parent_gpu, NvU32 index);
void uvm_hal_maxwell_fault_buffer_entry_clear_valid_unsupported(uvm_parent_gpu_t *parent_gpu, NvU32 index);
NvU32 uvm_hal_maxwell_fault_buffer_entry_size_unsupported(uvm_parent_gpu_t *parent_gpu);
bool uvm_hal_pascal_fault_buffer_entry_is_valid(uvm_parent_gpu_t *parent_gpu, NvU32 index);
void uvm_hal_pascal_fault_buffer_entry_clear_valid(uvm_parent_gpu_t *parent_gpu, NvU32 index);
NvU32 uvm_hal_pascal_fault_buffer_entry_size(uvm_parent_gpu_t *parent_gpu);

typedef void (*uvm_hal_fault_buffer_parse_non_replayable_entry_t)(uvm_parent_gpu_t *parent_gpu,
                                                                  void *fault_packet,
                                                                  uvm_fault_buffer_entry_t *buffer_entry);
void uvm_hal_maxwell_fault_buffer_parse_non_replayable_entry_unsupported(uvm_parent_gpu_t *parent_gpu,
                                                                         void *fault_packet,
                                                                         uvm_fault_buffer_entry_t *buffer_entry);
void uvm_hal_volta_fault_buffer_parse_non_replayable_entry(uvm_parent_gpu_t *parent_gpu,
                                                           void *fault_packet,
                                                           uvm_fault_buffer_entry_t *buffer_entry);

void uvm_hal_maxwell_cancel_faults_global_unsupported(uvm_push_t *push, uvm_gpu_phys_address_t instance_ptr);
void uvm_hal_pascal_cancel_faults_global(uvm_push_t *push, uvm_gpu_phys_address_t instance_ptr);

// Trigger fault replay on the GPU where the given pushbuffer is located.
void uvm_hal_maxwell_replay_faults_unsupported(uvm_push_t *push, uvm_fault_replay_type_t type);
void uvm_hal_maxwell_cancel_faults_targeted_unsupported(uvm_push_t *push,
                                                        uvm_gpu_phys_address_t instance_ptr,
                                                        NvU32 gpc_id,
                                                        NvU32 client_id);
void uvm_hal_pascal_replay_faults(uvm_push_t *push, uvm_fault_replay_type_t type);
void uvm_hal_pascal_cancel_faults_targeted(uvm_push_t *push,
                                           uvm_gpu_phys_address_t instance_ptr,
                                           NvU32 gpc_id,
                                           NvU32 client_id);

typedef void (*uvm_hal_fault_cancel_va_t)(uvm_push_t *push,
                                          uvm_gpu_phys_address_t pdb,
                                          const uvm_fault_buffer_entry_t *fault_entry,
                                          uvm_fault_cancel_va_mode_t cancel_va_mode);

void uvm_hal_maxwell_cancel_faults_va_unsupported(uvm_push_t *push,
                                                  uvm_gpu_phys_address_t pdb,
                                                  const uvm_fault_buffer_entry_t *fault_entry,
                                                  uvm_fault_cancel_va_mode_t cancel_va_mode);

void uvm_hal_volta_replay_faults(uvm_push_t *push, uvm_fault_replay_type_t type);
void uvm_hal_volta_cancel_faults_va(uvm_push_t *push,
                                    uvm_gpu_phys_address_t pdb,
                                    const uvm_fault_buffer_entry_t *fault_entry,
                                    uvm_fault_cancel_va_mode_t cancel_va_mode);

void uvm_hal_hopper_cancel_faults_va(uvm_push_t *push,
                                     uvm_gpu_phys_address_t pdb,
                                     const uvm_fault_buffer_entry_t *fault_entry,
                                     uvm_fault_cancel_va_mode_t cancel_va_mode);

typedef void (*uvm_hal_host_clear_faulted_channel_method_t)(uvm_push_t *push,
                                                            uvm_user_channel_t *user_channel,
                                                            const uvm_fault_buffer_entry_t *buffer_entry);

void uvm_hal_maxwell_host_clear_faulted_channel_method_unsupported(uvm_push_t *push,
                                                                   uvm_user_channel_t *user_channel,
                                                                   const uvm_fault_buffer_entry_t *buffer_entry);
void uvm_hal_volta_host_clear_faulted_channel_method(uvm_push_t *push,
                                                     uvm_user_channel_t *user_channel,
                                                     const uvm_fault_buffer_entry_t *buffer_entry);
void uvm_hal_turing_host_clear_faulted_channel_method(uvm_push_t *push,
                                                      uvm_user_channel_t *user_channel,
                                                      const uvm_fault_buffer_entry_t *buffer_entry);
typedef void (*uvm_hal_host_clear_faulted_channel_register_t)(uvm_user_channel_t *user_channel,
                                                              const uvm_fault_buffer_entry_t *buffer_entry);
void uvm_hal_maxwell_host_clear_faulted_channel_register_unsupported(uvm_user_channel_t *user_channel,
                                                                     const uvm_fault_buffer_entry_t *buffer_entry);
void uvm_hal_ampere_host_clear_faulted_channel_register(uvm_user_channel_t *user_channel,
                                                        const uvm_fault_buffer_entry_t *buffer_entry);

typedef void (*uvm_hal_host_clear_faulted_channel_sw_method_t)(uvm_push_t *push,
                                                               uvm_user_channel_t *user_channel,
                                                               const uvm_fault_buffer_entry_t *buffer_entry);
void uvm_hal_maxwell_host_clear_faulted_channel_sw_method_unsupported(uvm_push_t *push,
                                                                     uvm_user_channel_t *user_channel,
                                                                     const uvm_fault_buffer_entry_t *buffer_entry);
void uvm_hal_ampere_host_clear_faulted_channel_sw_method(uvm_push_t *push,
                                                         uvm_user_channel_t *user_channel,
                                                         const uvm_fault_buffer_entry_t *buffer_entry);

void uvm_hal_print_fault_entry(const uvm_fault_buffer_entry_t *entry);
void uvm_hal_print_access_counter_buffer_entry(const uvm_access_counter_buffer_entry_t *entry);

// Access counters
typedef void (*uvm_hal_enable_access_counter_notifications_t)(uvm_parent_gpu_t *parent_gpu);
typedef void (*uvm_hal_disable_access_counter_notifications_t)(uvm_parent_gpu_t *parent_gpu);
typedef void (*uvm_hal_clear_access_counter_notifications_t)(uvm_parent_gpu_t *parent_gpu, NvU32 get);

// Parse the entry on the given buffer index. This also clears the valid bit of
// the entry in the buffer.
typedef void (*uvm_hal_access_counter_buffer_parse_entry_t)(uvm_parent_gpu_t *parent_gpu,
                                                            NvU32 index,
                                                            uvm_access_counter_buffer_entry_t *buffer_entry);
typedef bool (*uvm_hal_access_counter_buffer_entry_is_valid_t)(uvm_parent_gpu_t *parent_gpu, NvU32 index);
typedef void (*uvm_hal_access_counter_buffer_entry_clear_valid_t)(uvm_parent_gpu_t *parent_gpu, NvU32 index);
typedef NvU32 (*uvm_hal_access_counter_buffer_entry_size_t)(uvm_parent_gpu_t *parent_gpu);
typedef void (*uvm_hal_access_counter_clear_all_t)(uvm_push_t *push);
typedef void (*uvm_hal_access_counter_clear_type_t)(uvm_push_t *push, uvm_access_counter_type_t type);
typedef void (*uvm_hal_access_counter_clear_targeted_t)(uvm_push_t *push,
                                                        const uvm_access_counter_buffer_entry_t *buffer_entry);

void uvm_hal_maxwell_enable_access_counter_notifications_unsupported(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_maxwell_disable_access_counter_notifications_unsupported(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_maxwell_clear_access_counter_notifications_unsupported(uvm_parent_gpu_t *parent_gpu, NvU32 get);
void uvm_hal_maxwell_access_counter_buffer_parse_entry_unsupported(uvm_parent_gpu_t *parent_gpu,
                                                                   NvU32 index,
                                                                   uvm_access_counter_buffer_entry_t *buffer_entry);
bool uvm_hal_maxwell_access_counter_buffer_entry_is_valid_unsupported(uvm_parent_gpu_t *parent_gpu, NvU32 index);
void uvm_hal_maxwell_access_counter_buffer_entry_clear_valid_unsupported(uvm_parent_gpu_t *parent_gpu, NvU32 index);
NvU32 uvm_hal_maxwell_access_counter_buffer_entry_size_unsupported(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_maxwell_access_counter_clear_all_unsupported(uvm_push_t *push);
void uvm_hal_maxwell_access_counter_clear_type_unsupported(uvm_push_t *push, uvm_access_counter_type_t type);
void uvm_hal_maxwell_access_counter_clear_targeted_unsupported(uvm_push_t *push,
                                                               const uvm_access_counter_buffer_entry_t *buffer_entry);

void uvm_hal_volta_enable_access_counter_notifications(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_volta_disable_access_counter_notifications(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_volta_clear_access_counter_notifications(uvm_parent_gpu_t *parent_gpu, NvU32 get);
void uvm_hal_volta_access_counter_buffer_parse_entry(uvm_parent_gpu_t *parent_gpu,
                                                     NvU32 index,
                                                     uvm_access_counter_buffer_entry_t *buffer_entry);
bool uvm_hal_volta_access_counter_buffer_entry_is_valid(uvm_parent_gpu_t *parent_gpu, NvU32 index);
void uvm_hal_volta_access_counter_buffer_entry_clear_valid(uvm_parent_gpu_t *parent_gpu, NvU32 index);
NvU32 uvm_hal_volta_access_counter_buffer_entry_size(uvm_parent_gpu_t *parent_gpu);

void uvm_hal_volta_access_counter_clear_all(uvm_push_t *push);
void uvm_hal_volta_access_counter_clear_type(uvm_push_t *push, uvm_access_counter_type_t type);
void uvm_hal_volta_access_counter_clear_targeted(uvm_push_t *push,
                                                 const uvm_access_counter_buffer_entry_t *buffer_entry);

void uvm_hal_turing_disable_access_counter_notifications(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_turing_clear_access_counter_notifications(uvm_parent_gpu_t *parent_gpu, NvU32 get);

// The source and destination addresses must be 16-byte aligned. Note that the
// best performance is achieved with 256-byte alignment. The decrypt size must
// be larger than 0, and a multiple of 4 bytes.
//
// The authentication tag address must also be 16-byte aligned.
// The authentication tag buffer size is UVM_CONF_COMPUTING_AUTH_TAG_SIZE bytes
// defined in uvm_conf_computing.h.
//
// Decrypts the src buffer into the dst buffer of the given size.
// The method also verifies integrity of the src buffer by calculating its
// authentication tag and comparing it with the provided one.
//
// Note: SEC2 does not support encryption.
typedef void (*uvm_hal_sec2_decrypt_t)(uvm_push_t *push, NvU64 dst_va, NvU64 src_va, NvU32 size, NvU64 auth_tag_va);

void uvm_hal_maxwell_sec2_decrypt_unsupported(uvm_push_t *push,
                                              NvU64 dst_va,
                                              NvU64 src_va,
                                              NvU32 size,
                                              NvU64 auth_tag_va);
void uvm_hal_hopper_sec2_decrypt(uvm_push_t *push, NvU64 dst_va, NvU64 src_va, NvU32 size, NvU64 auth_tag_va);

struct uvm_host_hal_struct
{
    uvm_hal_init_t init;
    uvm_hal_host_method_is_valid method_is_valid;
    uvm_hal_host_sw_method_is_valid sw_method_is_valid;
    uvm_hal_wait_for_idle_t wait_for_idle;
    uvm_hal_membar_sys_t membar_sys;
    uvm_hal_membar_gpu_t membar_gpu;
    uvm_hal_noop_t noop;
    uvm_hal_interrupt_t interrupt;
    uvm_hal_semaphore_release_t semaphore_release;
    uvm_hal_semaphore_acquire_t semaphore_acquire;
    uvm_hal_semaphore_timestamp_t semaphore_timestamp;
    uvm_hal_host_set_gpfifo_entry_t set_gpfifo_entry;
    uvm_hal_host_set_gpfifo_noop_t set_gpfifo_noop;
    uvm_hal_host_set_gpfifo_pushbuffer_segment_base_t set_gpfifo_pushbuffer_segment_base;
    uvm_hal_host_write_gpu_put_t write_gpu_put;
    uvm_hal_host_tlb_invalidate_all_t tlb_invalidate_all;
    uvm_hal_host_tlb_invalidate_va_t tlb_invalidate_va;
    uvm_hal_host_tlb_invalidate_test_t tlb_invalidate_test;
    uvm_hal_fault_buffer_replay_t replay_faults;
    uvm_hal_fault_cancel_global_t cancel_faults_global;
    uvm_hal_fault_cancel_targeted_t cancel_faults_targeted;
    uvm_hal_fault_cancel_va_t cancel_faults_va;
    uvm_hal_host_clear_faulted_channel_method_t clear_faulted_channel_sw_method;
    uvm_hal_host_clear_faulted_channel_method_t clear_faulted_channel_method;
    uvm_hal_host_clear_faulted_channel_register_t clear_faulted_channel_register;
    uvm_hal_access_counter_clear_all_t access_counter_clear_all;
    uvm_hal_access_counter_clear_type_t access_counter_clear_type;
    uvm_hal_access_counter_clear_targeted_t access_counter_clear_targeted;
    uvm_hal_get_time_t get_time;
};

struct uvm_ce_hal_struct
{
    uvm_hal_init_t init;
    uvm_hal_ce_method_is_valid method_is_valid;
    uvm_hal_semaphore_release_t semaphore_release;
    uvm_hal_semaphore_timestamp_t semaphore_timestamp;
    uvm_hal_ce_offset_out_t offset_out;
    uvm_hal_ce_offset_in_out_t offset_in_out;
    uvm_hal_ce_phys_mode_t phys_mode;
    uvm_hal_ce_plc_mode_t plc_mode;
    uvm_hal_ce_memcopy_type_t memcopy_copy_type;
    uvm_hal_ce_memcopy_is_valid memcopy_is_valid;
    uvm_hal_ce_memcopy_patch_src memcopy_patch_src;
    uvm_hal_memcopy_t memcopy;
    uvm_hal_memcopy_v_to_v_t memcopy_v_to_v;
    uvm_hal_ce_memset_is_valid memset_is_valid;
    uvm_hal_memset_1_t memset_1;
    uvm_hal_memset_4_t memset_4;
    uvm_hal_memset_8_t memset_8;
    uvm_hal_memset_v_4_t memset_v_4;
    uvm_hal_semaphore_reduction_inc_t semaphore_reduction_inc;
    uvm_hal_ce_encrypt_t encrypt;
    uvm_hal_ce_decrypt_t decrypt;
};

struct uvm_arch_hal_struct
{
    uvm_hal_arch_init_properties_t init_properties;
    uvm_hal_lookup_mode_hal_t mmu_mode_hal;
    uvm_hal_mmu_enable_prefetch_faults_t enable_prefetch_faults;
    uvm_hal_mmu_disable_prefetch_faults_t disable_prefetch_faults;
    uvm_hal_mmu_client_id_to_utlb_id_t mmu_client_id_to_utlb_id;
};

struct uvm_fault_buffer_hal_struct
{
    uvm_hal_enable_replayable_faults_t enable_replayable_faults;
    uvm_hal_disable_replayable_faults_t disable_replayable_faults;
    uvm_hal_clear_replayable_faults_t clear_replayable_faults;
    uvm_hal_fault_buffer_read_put_t read_put;
    uvm_hal_fault_buffer_read_get_t read_get;
    uvm_hal_fault_buffer_write_get_t write_get;
    uvm_hal_fault_buffer_get_ve_id_t get_ve_id;
    uvm_hal_fault_buffer_get_mmu_engine_type_t get_mmu_engine_type;
    uvm_hal_fault_buffer_parse_replayable_entry_t parse_replayable_entry;
    uvm_hal_fault_buffer_entry_is_valid_t entry_is_valid;
    uvm_hal_fault_buffer_entry_clear_valid_t entry_clear_valid;
    uvm_hal_fault_buffer_entry_size_t entry_size;
    uvm_hal_fault_buffer_parse_non_replayable_entry_t parse_non_replayable_entry;
    uvm_hal_fault_buffer_get_fault_type_t get_fault_type;
};

struct uvm_access_counter_buffer_hal_struct
{
    uvm_hal_enable_access_counter_notifications_t enable_access_counter_notifications;
    uvm_hal_disable_access_counter_notifications_t disable_access_counter_notifications;
    uvm_hal_clear_access_counter_notifications_t clear_access_counter_notifications;
    uvm_hal_access_counter_buffer_parse_entry_t parse_entry;
    uvm_hal_access_counter_buffer_entry_is_valid_t entry_is_valid;
    uvm_hal_access_counter_buffer_entry_clear_valid_t entry_clear_valid;
    uvm_hal_access_counter_buffer_entry_size_t entry_size;
};

struct uvm_sec2_hal_struct
{
    uvm_hal_init_t init;
    uvm_hal_sec2_decrypt_t decrypt;
    uvm_hal_semaphore_release_t semaphore_release;
    uvm_hal_semaphore_timestamp_t semaphore_timestamp;
};

typedef struct
{
    // id is either a hardware class or GPU architecture
    NvU32 id;
    NvU32 parent_id;
    union
    {
        // host_ops: id is a hardware class
        uvm_host_hal_t host_ops;

        // ce_ops: id is a hardware class
        uvm_ce_hal_t ce_ops;

        // arch_ops: id is an architecture
        uvm_arch_hal_t arch_ops;

        // fault_buffer_ops: id is an architecture
        uvm_fault_buffer_hal_t fault_buffer_ops;

        // access_counter_buffer_ops: id is an architecture
        uvm_access_counter_buffer_hal_t access_counter_buffer_ops;

        // sec2_ops: id is an architecture
        uvm_sec2_hal_t sec2_ops;
    } u;
} uvm_hal_class_ops_t;

NV_STATUS uvm_hal_init_table(void);
NV_STATUS uvm_hal_init_gpu(uvm_parent_gpu_t *parent_gpu);
void uvm_hal_init_properties(uvm_parent_gpu_t *parent_gpu);

// Helper to push a SYS or GPU membar based on the membar type
//
// Notably this doesn't just get the GPU from the push object to support the
// test mode of the page tree code that doesn't do real pushes.
static void uvm_hal_membar(uvm_gpu_t *gpu, uvm_push_t *push, uvm_membar_t membar)
{
    switch (membar) {
        case UVM_MEMBAR_SYS:
            gpu->parent->host_hal->membar_sys(push);
            break;
        case UVM_MEMBAR_GPU:
            gpu->parent->host_hal->membar_gpu(push);
            break;
        case UVM_MEMBAR_NONE:
            break;
    }
}

static void uvm_hal_wfi_membar(uvm_push_t *push, uvm_membar_t membar)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    gpu->parent->host_hal->wait_for_idle(push);
    uvm_hal_membar(gpu, push, membar);
}

// Internal helper used by the TLB invalidate hal functions. This issues the
// appropriate Host membar(s) after a TLB invalidate.
void uvm_hal_tlb_invalidate_membar(uvm_push_t *push, uvm_membar_t membar);

// Internal helper used by architectures/engines that don't support a FLUSH
// operation with a FLUSH_TYPE on the semaphore release method, e.g., pre-Volta
// CE. It inspects and clears the MEMBAR push flags, issues a Host WFI +
// membar.gpu for MEMBAR_GPU or returns true to indicate the caller to use the
// engine's FLUSH for MEMBAR_SYS.
bool uvm_hal_membar_before_semaphore(uvm_push_t *push);

// Determine the appropriate membar to use on TLB invalidates for GPU PTE
// permissions downgrades.
//
// gpu is the GPU on which the TLB invalidate is happening.
//
// is_local_vidmem indicates whether all mappings being invalidated pointed to
// the local GPU's memory.
uvm_membar_t uvm_hal_downgrade_membar_type(uvm_gpu_t *gpu, bool is_local_vidmem);

#endif // __UVM_HAL_H__
