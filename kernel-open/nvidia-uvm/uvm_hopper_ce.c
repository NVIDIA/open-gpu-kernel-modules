/*******************************************************************************
    Copyright (c) 2020 NVIDIA Corporation

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
#include "uvm_push.h"
#include "clc8b5.h"

static void hopper_membar_after_transfer(uvm_push_t *push)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);

    if (uvm_push_get_and_reset_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE))
        return;

    // TODO: [UVM-Volta] Remove Host WFI + Membar WAR for CE flush-only bug
    // http://nvbugs/1734761
    gpu->parent->host_hal->wait_for_idle(push);

    if (uvm_push_get_and_reset_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_GPU))
        gpu->parent->host_hal->membar_gpu(push);
    else
        gpu->parent->host_hal->membar_sys(push);
}

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

// Perform an appropriate membar before a semaphore operation. Returns whether
// the semaphore operation should include a flush.
static bool hopper_membar_before_semaphore(uvm_push_t *push)
{
    uvm_gpu_t *gpu;

    if (uvm_push_get_and_reset_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE)) {
        // No MEMBAR requested, don't use a flush.
        return false;
    }

    if (!uvm_push_get_and_reset_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_GPU)) {
        // By default do a MEMBAR SYS and for that we can just use flush on the
        // semaphore operation.
        return true;
    }

    // TODO: Bug 1734761: Remove the HOST WFI+membar WAR, i.e, perform the CE
    // flush when MEMBAR GPU is requested.
    gpu = uvm_push_get_gpu(push);
    gpu->parent->host_hal->wait_for_idle(push);
    gpu->parent->host_hal->membar_gpu(push);

    return false;
}

void uvm_hal_hopper_ce_semaphore_release(uvm_push_t *push, NvU64 gpu_va, NvU32 payload)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    NvU32 flush_value;
    NvU32 launch_dma_plc_mode;
    bool use_flush;

    use_flush = hopper_membar_before_semaphore(push);

    if (use_flush)
        flush_value = HWCONST(C8B5, LAUNCH_DMA, FLUSH_ENABLE, TRUE);
    else
        flush_value = HWCONST(C8B5, LAUNCH_DMA, FLUSH_ENABLE, FALSE);

    NV_PUSH_3U(C8B5, SET_SEMAPHORE_A, HWVALUE(C8B5, SET_SEMAPHORE_A, UPPER, NvOffset_HI32(gpu_va)),
                     SET_SEMAPHORE_B, HWVALUE(C8B5, SET_SEMAPHORE_B, LOWER, NvOffset_LO32(gpu_va)),
                     SET_SEMAPHORE_PAYLOAD, payload);

    launch_dma_plc_mode = gpu->parent->ce_hal->plc_mode();

    NV_PUSH_1U(C8B5, LAUNCH_DMA, flush_value |
       HWCONST(C8B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, NONE) |
       HWCONST(C8B5, LAUNCH_DMA, SEMAPHORE_TYPE, RELEASE_ONE_WORD_SEMAPHORE) |
       launch_dma_plc_mode);
}

void uvm_hal_hopper_ce_semaphore_reduction_inc(uvm_push_t *push, NvU64 gpu_va, NvU32 payload)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    NvU32 flush_value;
    NvU32 launch_dma_plc_mode;
    bool use_flush;

    use_flush = hopper_membar_before_semaphore(push);

    if (use_flush)
        flush_value = HWCONST(C8B5, LAUNCH_DMA, FLUSH_ENABLE, TRUE);
    else
        flush_value = HWCONST(C8B5, LAUNCH_DMA, FLUSH_ENABLE, FALSE);

    NV_PUSH_3U(C8B5, SET_SEMAPHORE_A, HWVALUE(C8B5, SET_SEMAPHORE_A, UPPER, NvOffset_HI32(gpu_va)),
                     SET_SEMAPHORE_B, HWVALUE(C8B5, SET_SEMAPHORE_B, LOWER, NvOffset_LO32(gpu_va)),
                     SET_SEMAPHORE_PAYLOAD, payload);

    launch_dma_plc_mode = gpu->parent->ce_hal->plc_mode();

    NV_PUSH_1U(C8B5, LAUNCH_DMA, flush_value |
       HWCONST(C8B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, NONE) |
       HWCONST(C8B5, LAUNCH_DMA, SEMAPHORE_TYPE, RELEASE_ONE_WORD_SEMAPHORE) |
       HWCONST(C8B5, LAUNCH_DMA, SEMAPHORE_REDUCTION, INC) |
       HWCONST(C8B5, LAUNCH_DMA, SEMAPHORE_REDUCTION_SIGN, UNSIGNED) |
       HWCONST(C8B5, LAUNCH_DMA, SEMAPHORE_REDUCTION_ENABLE, TRUE) |
       launch_dma_plc_mode);
}

void uvm_hal_hopper_ce_semaphore_timestamp(uvm_push_t *push, NvU64 gpu_va)
{
    uvm_gpu_t *gpu;
    NvU32 flush_value;
    NvU32 launch_dma_plc_mode;
    bool use_flush;

    use_flush = hopper_membar_before_semaphore(push);

    if (use_flush)
        flush_value = HWCONST(C8B5, LAUNCH_DMA, FLUSH_ENABLE, TRUE);
    else
        flush_value = HWCONST(C8B5, LAUNCH_DMA, FLUSH_ENABLE, FALSE);

    NV_PUSH_3U(C8B5, SET_SEMAPHORE_A, HWVALUE(C8B5, SET_SEMAPHORE_A, UPPER, NvOffset_HI32(gpu_va)),
                     SET_SEMAPHORE_B, HWVALUE(C8B5, SET_SEMAPHORE_B, LOWER, NvOffset_LO32(gpu_va)),
                     SET_SEMAPHORE_PAYLOAD, 0xdeadbeef);

    gpu = uvm_push_get_gpu(push);
    launch_dma_plc_mode = gpu->parent->ce_hal->plc_mode();

    NV_PUSH_1U(C8B5, LAUNCH_DMA, flush_value |
       HWCONST(C8B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, NONE) |
       HWCONST(C8B5, LAUNCH_DMA, SEMAPHORE_TYPE, RELEASE_FOUR_WORD_SEMAPHORE) |
       launch_dma_plc_mode);
}

static NvU32 hopper_memset_push_phys_mode(uvm_push_t *push, uvm_gpu_address_t dst)
{
    if (dst.is_virtual)
        return HWCONST(C8B5, LAUNCH_DMA, DST_TYPE, VIRTUAL);

    NV_PUSH_1U(C8B5, SET_DST_PHYS_MODE, ce_aperture(dst.aperture));
    return HWCONST(C8B5, LAUNCH_DMA, DST_TYPE, PHYSICAL);
}

static bool hopper_scrub_enable(uvm_gpu_address_t dst, size_t size)
{
    return !dst.is_virtual &&
           dst.aperture == UVM_APERTURE_VID &&
           IS_ALIGNED(dst.address, UVM_PAGE_SIZE_4K) &&
           IS_ALIGNED(size, UVM_PAGE_SIZE_4K);
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

    UVM_ASSERT_MSG(gpu->parent->ce_hal->memset_validate(push, dst, memset_element_size),
                   "Memset validation failed in channel %s, GPU %s",
                   push->channel->name,
                   uvm_gpu_name(gpu));

    launch_dma_dst_type = hopper_memset_push_phys_mode(push, dst);
    launch_dma_plc_mode = gpu->parent->ce_hal->plc_mode();

    if (uvm_push_get_and_reset_flag(push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED))
        pipelined_value = HWCONST(C8B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, PIPELINED);
    else
        pipelined_value = HWCONST(C8B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, NON_PIPELINED);

    if (memset_element_size == 8 && hopper_scrub_enable(dst, num_elements * memset_element_size)) {
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

        gpu->parent->ce_hal->offset_out(push, dst.address);

        NV_PUSH_1U(C8B5, LINE_LENGTH_IN, memset_this_time);

        NV_PUSH_1U(C8B5, LAUNCH_DMA,
           HWCONST(C8B5, LAUNCH_DMA, SRC_MEMORY_LAYOUT, PITCH) |
           HWCONST(C8B5, LAUNCH_DMA, DST_MEMORY_LAYOUT, PITCH) |
           HWCONST(C8B5, LAUNCH_DMA, MULTI_LINE_ENABLE, FALSE) |
           HWCONST(C8B5, LAUNCH_DMA, FLUSH_ENABLE, FALSE) |
           launch_dma_remap_enable |
           launch_dma_scrub_enable |
           launch_dma_dst_type |
           launch_dma_plc_mode |
           pipelined_value);

        dst.address += memset_this_time * memset_element_size;
        num_elements -= memset_this_time;
        pipelined_value = HWCONST(C8B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, NON_PIPELINED);
    } while (num_elements > 0);

    hopper_membar_after_transfer(push);
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
    if (hopper_scrub_enable(dst, size)) {
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

    if (hopper_scrub_enable(dst, size)) {
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
