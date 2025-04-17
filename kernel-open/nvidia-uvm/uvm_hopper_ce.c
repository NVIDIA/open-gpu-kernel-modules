/*******************************************************************************
    Copyright (c) 2020-2024 NVIDIA Corporation

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

#include "uvm_hal.h"
#include "uvm_global.h"
#include "uvm_push.h"
#include "uvm_mem.h"
#include "uvm_conf_computing.h"
#include "clc8b5.h"

static NvU32 ce_aperture(uvm_aperture_t aperture)
{
    BUILD_BUG_ON(HWCONST(C8B5, SET_SRC_PHYS_MODE, TARGET, LOCAL_FB) !=
                 HWCONST(C8B5, SET_DST_PHYS_MODE, TARGET, LOCAL_FB));
    BUILD_BUG_ON(HWCONST(C8B5, SET_SRC_PHYS_MODE, TARGET, COHERENT_SYSMEM) !=
                 HWCONST(C8B5, SET_DST_PHYS_MODE, TARGET, COHERENT_SYSMEM));
    BUILD_BUG_ON(HWCONST(C8B5, SET_SRC_PHYS_MODE, TARGET, PEERMEM) !=
                 HWCONST(C8B5, SET_DST_PHYS_MODE, TARGET, PEERMEM));

    if (aperture == UVM_APERTURE_SYS) {
        return HWCONST(C8B5, SET_SRC_PHYS_MODE, TARGET, COHERENT_SYSMEM);
    }
    else if (aperture == UVM_APERTURE_VID) {
        return HWCONST(C8B5, SET_SRC_PHYS_MODE, TARGET, LOCAL_FB);
    }
    else {
        return HWCONST(C8B5, SET_SRC_PHYS_MODE, TARGET, PEERMEM) |
               HWVALUE(C8B5, SET_SRC_PHYS_MODE, FLA, 0) |
               HWVALUE(C8B5, SET_SRC_PHYS_MODE, PEER_ID, UVM_APERTURE_PEER_ID(aperture));
    }
}

void uvm_hal_hopper_ce_offset_out(uvm_push_t *push, NvU64 offset_out)
{
    NV_PUSH_2U(C8B5, OFFSET_OUT_UPPER, HWVALUE(C8B5, OFFSET_OUT_UPPER, UPPER, NvOffset_HI32(offset_out)),
                     OFFSET_OUT_LOWER, HWVALUE(C8B5, OFFSET_OUT_LOWER, VALUE, NvOffset_LO32(offset_out)));
}

void uvm_hal_hopper_ce_offset_in_out(uvm_push_t *push, NvU64 offset_in, NvU64 offset_out)
{
    NV_PUSH_4U(C8B5, OFFSET_IN_UPPER,  HWVALUE(C8B5, OFFSET_IN_UPPER,  UPPER, NvOffset_HI32(offset_in)),
                     OFFSET_IN_LOWER,  HWVALUE(C8B5, OFFSET_IN_LOWER,  VALUE, NvOffset_LO32(offset_in)),
                     OFFSET_OUT_UPPER, HWVALUE(C8B5, OFFSET_OUT_UPPER, UPPER, NvOffset_HI32(offset_out)),
                     OFFSET_OUT_LOWER, HWVALUE(C8B5, OFFSET_OUT_LOWER, VALUE, NvOffset_LO32(offset_out)));
}

// Return the flush type and the flush enablement.
static NvU32 hopper_get_flush_value(uvm_push_t *push)
{
    NvU32 flush_value;
    uvm_membar_t membar = uvm_push_get_and_reset_membar_flag(push);

    if (membar == UVM_MEMBAR_NONE) {
        // No MEMBAR requested, don't use a flush.
        flush_value = HWCONST(C8B5, LAUNCH_DMA, FLUSH_ENABLE, FALSE);
    }
    else {
        flush_value = HWCONST(C8B5, LAUNCH_DMA, FLUSH_ENABLE, TRUE);

        if (membar == UVM_MEMBAR_GPU)
            flush_value |= HWCONST(C8B5, LAUNCH_DMA, FLUSH_TYPE, GL);
        else
            flush_value |= HWCONST(C8B5, LAUNCH_DMA, FLUSH_TYPE, SYS);
    }

    return flush_value;
}

void uvm_hal_hopper_ce_semaphore_release(uvm_push_t *push, NvU64 gpu_va, NvU32 payload)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    NvU32 launch_dma_plc_mode;

    UVM_ASSERT_MSG(gpu->parent->ce_hal->semaphore_target_is_valid(push, gpu_va),
                   "Semaphore target validation failed in channel %s, GPU %s.\n",
                   push->channel ? push->channel->name : "'fake'",
                   uvm_gpu_name(gpu));

    NV_PUSH_3U(C8B5, SET_SEMAPHORE_A, HWVALUE(C8B5, SET_SEMAPHORE_A, UPPER, NvOffset_HI32(gpu_va)),
                     SET_SEMAPHORE_B, HWVALUE(C8B5, SET_SEMAPHORE_B, LOWER, NvOffset_LO32(gpu_va)),
                     SET_SEMAPHORE_PAYLOAD, payload);

    launch_dma_plc_mode = gpu->parent->ce_hal->plc_mode();

    NV_PUSH_1U(C8B5, LAUNCH_DMA, hopper_get_flush_value(push) |
       HWCONST(C8B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, NONE) |
       HWCONST(C8B5, LAUNCH_DMA, SEMAPHORE_PAYLOAD_SIZE, ONE_WORD) |
       HWCONST(C8B5, LAUNCH_DMA, SEMAPHORE_TYPE, RELEASE_SEMAPHORE_NO_TIMESTAMP) |
       launch_dma_plc_mode);
}

void uvm_hal_hopper_ce_semaphore_reduction_inc(uvm_push_t *push, NvU64 gpu_va, NvU32 payload)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    NvU32 launch_dma_plc_mode;

    UVM_ASSERT_MSG(gpu->parent->ce_hal->semaphore_target_is_valid(push, gpu_va),
                   "Semaphore target validation failed in channel %s, GPU %s.\n",
                   push->channel ? push->channel->name : "'fake'",
                   uvm_gpu_name(gpu));

    NV_PUSH_3U(C8B5, SET_SEMAPHORE_A, HWVALUE(C8B5, SET_SEMAPHORE_A, UPPER, NvOffset_HI32(gpu_va)),
                     SET_SEMAPHORE_B, HWVALUE(C8B5, SET_SEMAPHORE_B, LOWER, NvOffset_LO32(gpu_va)),
                     SET_SEMAPHORE_PAYLOAD, payload);

    launch_dma_plc_mode = gpu->parent->ce_hal->plc_mode();

    NV_PUSH_1U(C8B5, LAUNCH_DMA, hopper_get_flush_value(push) |
       HWCONST(C8B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, NONE) |
       HWCONST(C8B5, LAUNCH_DMA, SEMAPHORE_PAYLOAD_SIZE, ONE_WORD) |
       HWCONST(C8B5, LAUNCH_DMA, SEMAPHORE_TYPE, RELEASE_SEMAPHORE_NO_TIMESTAMP) |
       HWCONST(C8B5, LAUNCH_DMA, SEMAPHORE_REDUCTION, INC) |
       HWCONST(C8B5, LAUNCH_DMA, SEMAPHORE_REDUCTION_SIGN, UNSIGNED) |
       HWCONST(C8B5, LAUNCH_DMA, SEMAPHORE_REDUCTION_ENABLE, TRUE) |
       launch_dma_plc_mode);
}

void uvm_hal_hopper_ce_semaphore_timestamp(uvm_push_t *push, NvU64 gpu_va)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    NvU32 launch_dma_plc_mode;

    UVM_ASSERT_MSG(gpu->parent->ce_hal->semaphore_target_is_valid(push, gpu_va),
                   "Semaphore target validation failed in channel %s, GPU %s.\n",
                   push->channel ? push->channel->name : "'fake'",
                   uvm_gpu_name(gpu));

    NV_PUSH_3U(C8B5, SET_SEMAPHORE_A, HWVALUE(C8B5, SET_SEMAPHORE_A, UPPER, NvOffset_HI32(gpu_va)),
                     SET_SEMAPHORE_B, HWVALUE(C8B5, SET_SEMAPHORE_B, LOWER, NvOffset_LO32(gpu_va)),
                     SET_SEMAPHORE_PAYLOAD, 0xdeadbeef);

    launch_dma_plc_mode = gpu->parent->ce_hal->plc_mode();

    NV_PUSH_1U(C8B5, LAUNCH_DMA, hopper_get_flush_value(push) |
       HWCONST(C8B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, NONE) |
       HWCONST(C8B5, LAUNCH_DMA, SEMAPHORE_PAYLOAD_SIZE, ONE_WORD) |
       HWCONST(C8B5, LAUNCH_DMA, SEMAPHORE_TYPE, RELEASE_SEMAPHORE_WITH_TIMESTAMP) |
       launch_dma_plc_mode);
}

static NvU32 hopper_memset_push_phys_mode(uvm_push_t *push, uvm_gpu_address_t dst)
{
    if (dst.is_virtual)
        return HWCONST(C8B5, LAUNCH_DMA, DST_TYPE, VIRTUAL);

    NV_PUSH_1U(C8B5, SET_DST_PHYS_MODE, ce_aperture(dst.aperture));
    return HWCONST(C8B5, LAUNCH_DMA, DST_TYPE, PHYSICAL);
}

static bool va_is_flat_vidmem(uvm_gpu_t *gpu, NvU64 va)
{
    return (uvm_mmu_parent_gpu_needs_static_vidmem_mapping(gpu->parent) ||
            uvm_mmu_parent_gpu_needs_dynamic_vidmem_mapping(gpu->parent)) &&
           va >= gpu->parent->flat_vidmem_va_base &&
           va < gpu->parent->flat_vidmem_va_base + UVM_GPU_MAX_PHYS_MEM;
}

// Return whether a memset should use the fast scrubber. If so, convert dst to
// the address needed by the fast scrubber.
static bool hopper_scrub_enable(uvm_gpu_t *gpu, uvm_gpu_address_t *dst, size_t size)
{
    if (!IS_ALIGNED(dst->address, UVM_PAGE_SIZE_4K) || !IS_ALIGNED(size, UVM_PAGE_SIZE_4K))
        return false;

    // When CE physical writes are disallowed, higher layers will convert
    // physical memsets to virtual using the flat mapping. Those layers are
    // unaware of the fast scrubber, which is safe to use specifically when CE
    // physical access is disallowed. Detect such memsets within the flat vidmem
    // region and convert them back to physical, since the fast scrubber only
    // works with physical addressing.
    if (dst->is_virtual && !gpu->parent->ce_phys_vidmem_write_supported && va_is_flat_vidmem(gpu, dst->address)) {
        *dst = uvm_gpu_address_physical(UVM_APERTURE_VID, dst->address - gpu->parent->flat_vidmem_va_base);
        return true;
    }

    return !dst->is_virtual && dst->aperture == UVM_APERTURE_VID;
}

static NvU32 hopper_memset_copy_type(uvm_gpu_address_t dst)
{
    if (g_uvm_global.conf_computing_enabled && dst.is_unprotected)
        return HWCONST(C8B5, LAUNCH_DMA, COPY_TYPE, NONPROT2NONPROT);

    return HWCONST(C8B5, LAUNCH_DMA, COPY_TYPE, DEFAULT);
}

NvU32 uvm_hal_hopper_ce_memcopy_copy_type(uvm_gpu_address_t dst, uvm_gpu_address_t src)
{
    if (g_uvm_global.conf_computing_enabled && dst.is_unprotected && src.is_unprotected)
        return HWCONST(C8B5, LAUNCH_DMA, COPY_TYPE, NONPROT2NONPROT);

    return HWCONST(C8B5, LAUNCH_DMA, COPY_TYPE, DEFAULT);
}

static void hopper_memset_common(uvm_push_t *push,
                                 uvm_gpu_address_t dst,
                                 size_t num_elements,
                                 size_t memset_element_size)
{
    // If >4GB memsets ever become an important use case, this function should
    // use multi-line transfers so we don't have to iterate (bug 1766588).
    static const size_t max_single_memset = 0xFFFFFFFF;

    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    NvU32 pipelined_value;
    NvU32 launch_dma_dst_type;
    NvU32 launch_dma_plc_mode;
    NvU32 launch_dma_remap_enable;
    NvU32 launch_dma_scrub_enable;
    NvU32 flush_value = HWCONST(C8B5, LAUNCH_DMA, FLUSH_ENABLE, FALSE);
    NvU32 copy_type_value = hopper_memset_copy_type(dst);
    bool is_scrub = hopper_scrub_enable(gpu, &dst, num_elements * memset_element_size);

    UVM_ASSERT_MSG(gpu->parent->ce_hal->memset_is_valid(push, dst, num_elements, memset_element_size),
                   "Memset validation failed in channel %s, GPU %s",
                   push->channel->name,
                   uvm_gpu_name(gpu));

    launch_dma_dst_type = hopper_memset_push_phys_mode(push, dst);
    launch_dma_plc_mode = gpu->parent->ce_hal->plc_mode();

    if (uvm_push_get_and_reset_flag(push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED))
        pipelined_value = HWCONST(C8B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, PIPELINED);
    else
        pipelined_value = HWCONST(C8B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, NON_PIPELINED);

    if (memset_element_size == 8 && is_scrub) {
        launch_dma_remap_enable = HWCONST(C8B5, LAUNCH_DMA, REMAP_ENABLE, FALSE);
        launch_dma_scrub_enable = HWCONST(C8B5, LAUNCH_DMA, MEMORY_SCRUB_ENABLE, TRUE);

        NV_PUSH_1U(C8B5, SET_MEMORY_SCRUB_PARAMETERS,
           HWCONST(C8B5, SET_MEMORY_SCRUB_PARAMETERS, DISCARDABLE, FALSE));

        // Scrub requires disabling remap, and with remap disabled the element
        // size is 1.
        num_elements *= memset_element_size;
        memset_element_size = 1;
    }
    else {
        launch_dma_remap_enable = HWCONST(C8B5, LAUNCH_DMA, REMAP_ENABLE, TRUE);
        launch_dma_scrub_enable = HWCONST(C8B5, LAUNCH_DMA, MEMORY_SCRUB_ENABLE, FALSE);
    }

    do {
        NvU32 memset_this_time = (NvU32)min(num_elements, max_single_memset);

        // In the last operation, a flush/membar may be issued after the memset.
        if (num_elements == memset_this_time)
            flush_value = hopper_get_flush_value(push);

        gpu->parent->ce_hal->offset_out(push, dst.address);

        NV_PUSH_1U(C8B5, LINE_LENGTH_IN, memset_this_time);

        NV_PUSH_1U(C8B5, LAUNCH_DMA,
           HWCONST(C8B5, LAUNCH_DMA, SRC_MEMORY_LAYOUT, PITCH) |
           HWCONST(C8B5, LAUNCH_DMA, DST_MEMORY_LAYOUT, PITCH) |
           HWCONST(C8B5, LAUNCH_DMA, MULTI_LINE_ENABLE, FALSE) |
           flush_value |
           launch_dma_remap_enable |
           launch_dma_scrub_enable |
           launch_dma_dst_type |
           launch_dma_plc_mode |
           copy_type_value |
           pipelined_value);

        dst.address += memset_this_time * memset_element_size;
        num_elements -= memset_this_time;
        pipelined_value = HWCONST(C8B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, PIPELINED);
    } while (num_elements > 0);
}

void uvm_hal_hopper_ce_memset_8(uvm_push_t *push, uvm_gpu_address_t dst, NvU64 value, size_t size)
{
    UVM_ASSERT_MSG(size % 8 == 0, "size: %zd\n", size);

    size /= 8;

    NV_PUSH_3U(C8B5, SET_REMAP_CONST_A, (NvU32)value,
                     SET_REMAP_CONST_B, (NvU32)(value >> 32),
                     SET_REMAP_COMPONENTS,
       HWCONST(C8B5, SET_REMAP_COMPONENTS, DST_X,               CONST_A) |
       HWCONST(C8B5, SET_REMAP_COMPONENTS, DST_Y,               CONST_B) |
       HWCONST(C8B5, SET_REMAP_COMPONENTS, COMPONENT_SIZE,      FOUR)    |
       HWCONST(C8B5, SET_REMAP_COMPONENTS, NUM_DST_COMPONENTS,  TWO));

    hopper_memset_common(push, dst, size, 8);
}

void uvm_hal_hopper_ce_memset_1(uvm_push_t *push, uvm_gpu_address_t dst, NvU8 value, size_t size)
{
    if (hopper_scrub_enable(uvm_push_get_gpu(push), &dst, size)) {
        NvU64 value64 = value;

        value64 |= value64 << 8;
        value64 |= value64 << 16;
        value64 |= value64 << 32;

        uvm_hal_hopper_ce_memset_8(push, dst, value64, size);
        return;
    }

    NV_PUSH_2U(C8B5, SET_REMAP_CONST_B,    (NvU32)value,
                     SET_REMAP_COMPONENTS,
       HWCONST(C8B5, SET_REMAP_COMPONENTS, DST_X,               CONST_B) |
       HWCONST(C8B5, SET_REMAP_COMPONENTS, COMPONENT_SIZE,      ONE)     |
       HWCONST(C8B5, SET_REMAP_COMPONENTS, NUM_DST_COMPONENTS,  ONE));

    hopper_memset_common(push, dst, size, 1);
}

void uvm_hal_hopper_ce_memset_4(uvm_push_t *push, uvm_gpu_address_t dst, NvU32 value, size_t size)
{
    UVM_ASSERT_MSG(size % 4 == 0, "size: %zd\n", size);

    if (hopper_scrub_enable(uvm_push_get_gpu(push), &dst, size)) {
        NvU64 value64 = value;

        value64 |= value64 << 32;

        uvm_hal_hopper_ce_memset_8(push, dst, value64, size);
        return;
    }

    size /= 4;

    NV_PUSH_2U(C8B5, SET_REMAP_CONST_B,    value,
                     SET_REMAP_COMPONENTS,
       HWCONST(C8B5, SET_REMAP_COMPONENTS, DST_X,               CONST_B) |
       HWCONST(C8B5, SET_REMAP_COMPONENTS, COMPONENT_SIZE,      FOUR)    |
       HWCONST(C8B5, SET_REMAP_COMPONENTS, NUM_DST_COMPONENTS,  ONE));

    hopper_memset_common(push, dst, size, 4);
}

bool uvm_hal_hopper_ce_memset_is_valid(uvm_push_t *push,
                                       uvm_gpu_address_t dst,
                                       size_t num_elements,
                                       size_t element_size)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);

    // In Confidential Computing, if a memset uses physical addressing for the
    // destination, then it must write to (protected) vidmem. If the memset uses
    // virtual addressing, and the backing storage is not vidmem, the access is
    // only legal if the copy type is NONPROT2NONPROT, and the destination is
    // unprotected sysmem, but the validation does not detect it.
    if (g_uvm_global.conf_computing_enabled && !dst.is_virtual && dst.aperture != UVM_APERTURE_VID)
        return false;

    if (uvm_gpu_address_is_peer(gpu, dst)) {
        UVM_ERR_PRINT("Memset to peer address (0x%llx) is not allowed!", dst.address);
        return false;
    }

    if (!gpu->parent->ce_phys_vidmem_write_supported) {
        size_t size = num_elements * element_size;
        uvm_gpu_address_t temp = dst;

        // Physical vidmem writes are disallowed, unless using the scrubber
        if (!dst.is_virtual && dst.aperture == UVM_APERTURE_VID && !hopper_scrub_enable(gpu, &temp, size)) {
            UVM_ERR_PRINT("Destination address of vidmem memset must be virtual, not physical: {%s, 0x%llx} size %zu\n",
                          uvm_gpu_address_aperture_string(dst),
                          dst.address,
                          size);
            return false;
        }
    }

    return true;
}

bool uvm_hal_hopper_ce_memcopy_is_valid(uvm_push_t *push, uvm_gpu_address_t dst, uvm_gpu_address_t src)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);

    if (uvm_gpu_address_is_peer(gpu, src)) {
        UVM_ERR_PRINT("Peer copy from peer address (0x%llx) is not allowed!", src.address);
        return false;
    }

    if (push->channel && uvm_gpu_address_is_peer(gpu, dst) && !uvm_channel_is_p2p(push->channel)) {
        UVM_ERR_PRINT("Peer copy from address (0x%llx) to address (0x%llx) should use designated p2p channels!",
                      src.address,
                      dst.address);
        return false;
    }

    if (g_uvm_global.conf_computing_enabled) {
        // In Confidential Computing, if a memcopy uses physical addressing for
        // either the destination or the source, then the corresponding aperture
        // must be vidmem. If virtual addressing is used, and the backing
        // storage is sysmem the access is only legal if the copy type is
        // NONPROT2NONPROT, but the validation does not detect it. In other
        // words the copy source and destination is unprotected sysmem.
        if (!src.is_virtual && (src.aperture != UVM_APERTURE_VID))
            return false;

        if (!dst.is_virtual && (dst.aperture != UVM_APERTURE_VID))
            return false;

        if (dst.is_unprotected != src.is_unprotected)
            return false;
    }

    if (!gpu->parent->ce_phys_vidmem_write_supported && !dst.is_virtual && dst.aperture == UVM_APERTURE_VID) {
        UVM_ERR_PRINT("Destination address of vidmem memcopy must be virtual, not physical: {%s, 0x%llx}\n",
                      uvm_gpu_address_aperture_string(dst),
                      dst.address);
        return false;
    }

    return true;
}

// Specialized version of uvm_hal_volta_ce_memcopy used for encryption and
// decryption. Pre-Hopper functionality, such as validation or address patching,
// has been removed.
static void encrypt_or_decrypt(uvm_push_t *push, uvm_gpu_address_t dst, uvm_gpu_address_t src, NvU32 size)
{
    NvU32 pipelined_value;
    NvU32 launch_dma_src_dst_type;
    NvU32 launch_dma_plc_mode;
    NvU32 flush_value;
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);

    // HW allows unaligned operations only if the entire buffer is in one 32B
    // sector. Operations on buffers larger than 32B have to be aligned.
    if (size > UVM_CONF_COMPUTING_BUF_ALIGNMENT) {
        UVM_ASSERT(IS_ALIGNED(src.address, UVM_CONF_COMPUTING_BUF_ALIGNMENT));
        UVM_ASSERT(IS_ALIGNED(dst.address, UVM_CONF_COMPUTING_BUF_ALIGNMENT));
    }
    else {
        UVM_ASSERT((dst.address >> UVM_CONF_COMPUTING_BUF_ALIGNMENT) ==
                   ((dst.address + size - 1) >> UVM_CONF_COMPUTING_BUF_ALIGNMENT));
        UVM_ASSERT((src.address >> UVM_CONF_COMPUTING_BUF_ALIGNMENT) ==
                   ((src.address + size - 1) >> UVM_CONF_COMPUTING_BUF_ALIGNMENT));
    }

    launch_dma_src_dst_type = gpu->parent->ce_hal->phys_mode(push, dst, src);
    launch_dma_plc_mode = gpu->parent->ce_hal->plc_mode();

    if (uvm_push_get_and_reset_flag(push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED))
        pipelined_value = HWCONST(C8B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, PIPELINED);
    else
        pipelined_value = HWCONST(C8B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, NON_PIPELINED);

    flush_value = hopper_get_flush_value(push);

    gpu->parent->ce_hal->offset_in_out(push, src.address, dst.address);

    NV_PUSH_1U(C8B5, LINE_LENGTH_IN, size);

    NV_PUSH_1U(C8B5, LAUNCH_DMA, HWCONST(C8B5, LAUNCH_DMA, SRC_MEMORY_LAYOUT, PITCH) |
                                 HWCONST(C8B5, LAUNCH_DMA, DST_MEMORY_LAYOUT, PITCH) |
                                 HWCONST(C8B5, LAUNCH_DMA, MULTI_LINE_ENABLE, FALSE) |
                                 HWCONST(C8B5, LAUNCH_DMA, REMAP_ENABLE, FALSE) |
                                 HWCONST(C8B5, LAUNCH_DMA, COPY_TYPE, SECURE) |
                                 flush_value |
                                 launch_dma_src_dst_type |
                                 launch_dma_plc_mode |
                                 pipelined_value);
}

// The GPU CE encrypt operation requires clients to pass a valid
// address where the used IV will be written. But this requirement is
// unnecessary, because UVM should instead rely on the CSL
// nvUvmInterfaceCslLogDeviceEncryption API to independently track
// the expected IV.
//
// To satisfy the HW requirement the same unprotected sysmem address is
// passed to all GPU-side encryptions. This dummy buffer is allocated at
// GPU initialization time.
static NvU64 encrypt_iv_address(uvm_push_t *push, uvm_gpu_address_t dst)
{
    NvU64 iv_address;
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);

    // Match addressing mode of destination and IV
    if (dst.is_virtual) {
        iv_address = uvm_rm_mem_get_gpu_va(gpu->conf_computing.iv_rm_mem, gpu, false).address;
    }
    else {
        iv_address = uvm_mem_gpu_physical(gpu->conf_computing.iv_mem,
                                          gpu,
                                          0,
                                          gpu->conf_computing.iv_mem->size).address;
    }

    UVM_ASSERT(IS_ALIGNED(iv_address, UVM_CONF_COMPUTING_IV_ALIGNMENT));

    return iv_address;
}

void uvm_hal_hopper_ce_encrypt(uvm_push_t *push,
                               uvm_gpu_address_t dst,
                               uvm_gpu_address_t src,
                               NvU32 size,
                               uvm_gpu_address_t auth_tag)
{

    NvU32 auth_tag_address_hi32, auth_tag_address_lo32;
    NvU64 iv_address;
    NvU32 iv_address_hi32, iv_address_lo32;

    UVM_ASSERT(g_uvm_global.conf_computing_enabled);
    UVM_ASSERT(IS_ALIGNED(auth_tag.address, UVM_CONF_COMPUTING_AUTH_TAG_ALIGNMENT));

    if (!src.is_virtual)
        UVM_ASSERT(src.aperture == UVM_APERTURE_VID);

    // The addressing mode (and aperture, if applicable) of the destination
    // pointer determines the addressing mode and aperture used by the
    // encryption to reference the other two addresses written by it:
    // authentication tag, and IV. If the client passes a sysmem physical
    // address as destination, then the authentication tag must also be a sysmem
    // physical address.
    UVM_ASSERT(dst.is_virtual == auth_tag.is_virtual);

    if (!dst.is_virtual) {
        UVM_ASSERT(dst.aperture == UVM_APERTURE_SYS);
        UVM_ASSERT(auth_tag.aperture == UVM_APERTURE_SYS);
    }

    NV_PUSH_1U(C8B5, SET_SECURE_COPY_MODE, HWCONST(C8B5, SET_SECURE_COPY_MODE, MODE, ENCRYPT));

    auth_tag_address_hi32 = HWVALUE(C8B5, SET_ENCRYPT_AUTH_TAG_ADDR_UPPER, UPPER, NvU64_HI32(auth_tag.address));
    auth_tag_address_lo32 = HWVALUE(C8B5, SET_ENCRYPT_AUTH_TAG_ADDR_LOWER, LOWER, NvU64_LO32(auth_tag.address));

    iv_address = encrypt_iv_address(push, dst);

    iv_address_hi32 = HWVALUE(C8B5, SET_ENCRYPT_IV_ADDR_UPPER, UPPER, NvU64_HI32(iv_address));
    iv_address_lo32 = HWVALUE(C8B5, SET_ENCRYPT_IV_ADDR_LOWER, LOWER, NvU64_LO32(iv_address));

    NV_PUSH_4U(C8B5, SET_ENCRYPT_AUTH_TAG_ADDR_UPPER, auth_tag_address_hi32,
                     SET_ENCRYPT_AUTH_TAG_ADDR_LOWER, auth_tag_address_lo32,
                     SET_ENCRYPT_IV_ADDR_UPPER, iv_address_hi32,
                     SET_ENCRYPT_IV_ADDR_LOWER, iv_address_lo32);

    encrypt_or_decrypt(push, dst, src, size);
}

void uvm_hal_hopper_ce_decrypt(uvm_push_t *push,
                               uvm_gpu_address_t dst,
                               uvm_gpu_address_t src,
                               NvU32 size,
                               uvm_gpu_address_t auth_tag)
{

    NvU32 auth_tag_address_hi32, auth_tag_address_lo32;

    UVM_ASSERT(g_uvm_global.conf_computing_enabled);
    UVM_ASSERT(IS_ALIGNED(auth_tag.address, UVM_CONF_COMPUTING_AUTH_TAG_ALIGNMENT));

    // The addressing mode (and aperture, if applicable) of the source and
    // authentication pointers should match. But unlike in the encryption case,
    // clients are not forced to pass a valid IV address.
    UVM_ASSERT(src.is_virtual == auth_tag.is_virtual);

    if (!src.is_virtual) {
        UVM_ASSERT(src.aperture == UVM_APERTURE_SYS);
        UVM_ASSERT(auth_tag.aperture == UVM_APERTURE_SYS);
    }

    if (!dst.is_virtual)
        UVM_ASSERT(dst.aperture == UVM_APERTURE_VID);

    NV_PUSH_1U(C8B5, SET_SECURE_COPY_MODE, HWCONST(C8B5, SET_SECURE_COPY_MODE, MODE, DECRYPT));

    auth_tag_address_hi32 = HWVALUE(C8B5, SET_DECRYPT_AUTH_TAG_COMPARE_ADDR_UPPER, UPPER, NvU64_HI32(auth_tag.address));
    auth_tag_address_lo32 = HWVALUE(C8B5, SET_DECRYPT_AUTH_TAG_COMPARE_ADDR_LOWER, LOWER, NvU64_LO32(auth_tag.address));

    NV_PUSH_2U(C8B5, SET_DECRYPT_AUTH_TAG_COMPARE_ADDR_UPPER, auth_tag_address_hi32,
                     SET_DECRYPT_AUTH_TAG_COMPARE_ADDR_LOWER, auth_tag_address_lo32);

    encrypt_or_decrypt(push, dst, src, size);
}
