/*******************************************************************************
    Copyright (c) 2022 NVIDIA Corporation

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
#include "clc3b5.h"

// Return the flush type and the flush enablement.
static NvU32 volta_get_flush_value(uvm_push_t *push)
{
    NvU32 flush_value;
    uvm_membar_t membar = uvm_push_get_and_reset_membar_flag(push);

    if (membar == UVM_MEMBAR_NONE) {
        // No MEMBAR requested, don't use a flush.
        flush_value = HWCONST(C3B5, LAUNCH_DMA, FLUSH_ENABLE, FALSE);
    }
    else {
        flush_value = HWCONST(C3B5, LAUNCH_DMA, FLUSH_ENABLE, TRUE);

        if (membar == UVM_MEMBAR_GPU)
            flush_value |= HWCONST(C3B5, LAUNCH_DMA, FLUSH_TYPE, GL);
        else
            flush_value |= HWCONST(C3B5, LAUNCH_DMA, FLUSH_TYPE, SYS);
    }

    return flush_value;
}

void uvm_hal_volta_ce_semaphore_release(uvm_push_t *push, NvU64 gpu_va, NvU32 payload)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    NvU32 launch_dma_plc_mode;

    NV_PUSH_3U(C3B5, SET_SEMAPHORE_A, HWVALUE(C3B5, SET_SEMAPHORE_A, UPPER, NvOffset_HI32(gpu_va)),
                     SET_SEMAPHORE_B, HWVALUE(C3B5, SET_SEMAPHORE_B, LOWER, NvOffset_LO32(gpu_va)),
                     SET_SEMAPHORE_PAYLOAD, payload);

    launch_dma_plc_mode = gpu->parent->ce_hal->plc_mode();

    NV_PUSH_1U(C3B5, LAUNCH_DMA, volta_get_flush_value(push) |
       HWCONST(C3B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, NONE) |
       HWCONST(C3B5, LAUNCH_DMA, SEMAPHORE_TYPE, RELEASE_ONE_WORD_SEMAPHORE) |
       launch_dma_plc_mode);
}

void uvm_hal_volta_ce_semaphore_reduction_inc(uvm_push_t *push, NvU64 gpu_va, NvU32 payload)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    NvU32 launch_dma_plc_mode;

    NV_PUSH_3U(C3B5, SET_SEMAPHORE_A, HWVALUE(C3B5, SET_SEMAPHORE_A, UPPER, NvOffset_HI32(gpu_va)),
                     SET_SEMAPHORE_B, HWVALUE(C3B5, SET_SEMAPHORE_B, LOWER, NvOffset_LO32(gpu_va)),
                     SET_SEMAPHORE_PAYLOAD, payload);

    launch_dma_plc_mode = gpu->parent->ce_hal->plc_mode();

    NV_PUSH_1U(C3B5, LAUNCH_DMA, volta_get_flush_value(push) |
       HWCONST(C3B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, NONE) |
       HWCONST(C3B5, LAUNCH_DMA, SEMAPHORE_TYPE, RELEASE_ONE_WORD_SEMAPHORE) |
       HWCONST(C3B5, LAUNCH_DMA, SEMAPHORE_REDUCTION, INC) |
       HWCONST(C3B5, LAUNCH_DMA, SEMAPHORE_REDUCTION_SIGN, UNSIGNED) |
       HWCONST(C3B5, LAUNCH_DMA, SEMAPHORE_REDUCTION_ENABLE, TRUE) |
       launch_dma_plc_mode);
}

void uvm_hal_volta_ce_semaphore_timestamp(uvm_push_t *push, NvU64 gpu_va)
{
    uvm_gpu_t *gpu;
    NvU32 launch_dma_plc_mode;

    NV_PUSH_3U(C3B5, SET_SEMAPHORE_A, HWVALUE(C3B5, SET_SEMAPHORE_A, UPPER, NvOffset_HI32(gpu_va)),
                     SET_SEMAPHORE_B, HWVALUE(C3B5, SET_SEMAPHORE_B, LOWER, NvOffset_LO32(gpu_va)),
                     SET_SEMAPHORE_PAYLOAD, 0xdeadbeef);

    gpu = uvm_push_get_gpu(push);
    launch_dma_plc_mode = gpu->parent->ce_hal->plc_mode();

    NV_PUSH_1U(C3B5, LAUNCH_DMA, volta_get_flush_value(push) |
       HWCONST(C3B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, NONE) |
       HWCONST(C3B5, LAUNCH_DMA, SEMAPHORE_TYPE, RELEASE_FOUR_WORD_SEMAPHORE) |
       launch_dma_plc_mode);
}

void uvm_hal_volta_ce_memcopy(uvm_push_t *push, uvm_gpu_address_t dst, uvm_gpu_address_t src, size_t size)
{
    // If >4GB copies ever become an important use case, this function should
    // use multi-line transfers so we don't have to iterate (bug 1766588).
    static const size_t max_single_copy_size = 0xFFFFFFFF;
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);

    NvU32 pipelined_value;
    NvU32 launch_dma_src_dst_type;
    NvU32 launch_dma_plc_mode;
    NvU32 flush_value = HWCONST(C3B5, LAUNCH_DMA, FLUSH_ENABLE, FALSE);
    NvU32 copy_type_value = gpu->parent->ce_hal->memcopy_copy_type(dst, src);

    UVM_ASSERT_MSG(gpu->parent->ce_hal->memcopy_is_valid(push, dst, src),
                   "Memcopy validation failed in channel %s, GPU %s.\n",
                   push->channel->name,
                   uvm_gpu_name(gpu));

    gpu->parent->ce_hal->memcopy_patch_src(push, &src);

    launch_dma_src_dst_type = gpu->parent->ce_hal->phys_mode(push, dst, src);
    launch_dma_plc_mode = gpu->parent->ce_hal->plc_mode();

    if (uvm_push_get_and_reset_flag(push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED))
        pipelined_value = HWCONST(C3B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, PIPELINED);
    else
        pipelined_value = HWCONST(C3B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, NON_PIPELINED);

    do {
        NvU32 copy_this_time = (NvU32)min(size, max_single_copy_size);

        // In the last operation, a flush/membar may be issued after the copy.
        if (size == copy_this_time)
            flush_value = volta_get_flush_value(push);

        gpu->parent->ce_hal->offset_in_out(push, src.address, dst.address);

        NV_PUSH_1U(C3B5, LINE_LENGTH_IN, copy_this_time);

        NV_PUSH_1U(C3B5, LAUNCH_DMA,
           HWCONST(C3B5, LAUNCH_DMA, SRC_MEMORY_LAYOUT, PITCH) |
           HWCONST(C3B5, LAUNCH_DMA, DST_MEMORY_LAYOUT, PITCH) |
           HWCONST(C3B5, LAUNCH_DMA, MULTI_LINE_ENABLE, FALSE) |
           HWCONST(C3B5, LAUNCH_DMA, REMAP_ENABLE, FALSE) |
           flush_value |
           launch_dma_src_dst_type |
           launch_dma_plc_mode |
           copy_type_value |
           pipelined_value);

        pipelined_value = HWCONST(C3B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, PIPELINED);
        dst.address += copy_this_time;
        src.address += copy_this_time;
        size -= copy_this_time;
    } while (size > 0);
}

static NvU32 ce_aperture(uvm_aperture_t aperture)
{
    BUILD_BUG_ON(HWCONST(C3B5, SET_SRC_PHYS_MODE, TARGET, LOCAL_FB) !=
                 HWCONST(C3B5, SET_DST_PHYS_MODE, TARGET, LOCAL_FB));
    BUILD_BUG_ON(HWCONST(C3B5, SET_SRC_PHYS_MODE, TARGET, COHERENT_SYSMEM) !=
                 HWCONST(C3B5, SET_DST_PHYS_MODE, TARGET, COHERENT_SYSMEM));

    UVM_ASSERT_MSG(aperture == UVM_APERTURE_VID || aperture == UVM_APERTURE_SYS, "aperture 0x%x\n", aperture);

    if (aperture == UVM_APERTURE_SYS)
        return HWCONST(C3B5, SET_SRC_PHYS_MODE, TARGET, COHERENT_SYSMEM);
    else
        return HWCONST(C3B5, SET_SRC_PHYS_MODE, TARGET, LOCAL_FB);
}

static NvU32 volta_memset_push_phys_mode(uvm_push_t *push, uvm_gpu_address_t dst)
{
    if (dst.is_virtual)
        return HWCONST(C3B5, LAUNCH_DMA, DST_TYPE, VIRTUAL);

    NV_PUSH_1U(C3B5, SET_DST_PHYS_MODE, ce_aperture(dst.aperture));
    return HWCONST(C3B5, LAUNCH_DMA, DST_TYPE, PHYSICAL);
}

static void memset_common(uvm_push_t *push, uvm_gpu_address_t dst, size_t size, size_t memset_element_size)
{
    // If >4GB memsets ever become an important use case, this function should
    // use multi-line transfers so we don't have to iterate (bug 1766588).
    static const size_t max_single_memset_size = 0xFFFFFFFF;

    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    NvU32 pipelined_value;
    NvU32 launch_dma_dst_type;
    NvU32 launch_dma_plc_mode;
    NvU32 flush_value = HWCONST(C3B5, LAUNCH_DMA, FLUSH_ENABLE, FALSE);

    UVM_ASSERT_MSG(gpu->parent->ce_hal->memset_is_valid(push, dst, size, memset_element_size),
                   "Memset validation failed in channel %s, GPU %s.\n",
                   push->channel->name,
                   uvm_gpu_name(gpu));

    launch_dma_dst_type = volta_memset_push_phys_mode(push, dst);
    launch_dma_plc_mode = gpu->parent->ce_hal->plc_mode();

    if (uvm_push_get_and_reset_flag(push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED))
        pipelined_value = HWCONST(C3B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, PIPELINED);
    else
        pipelined_value = HWCONST(C3B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, NON_PIPELINED);

    do {
        NvU32 memset_this_time = (NvU32)min(size, max_single_memset_size);

        // In the last operation, a flush/membar may be issue after the memset.
        if (size == memset_this_time)
            flush_value = volta_get_flush_value(push);

        gpu->parent->ce_hal->offset_out(push, dst.address);

        NV_PUSH_1U(C3B5, LINE_LENGTH_IN, memset_this_time);

        NV_PUSH_1U(C3B5, LAUNCH_DMA,
           HWCONST(C3B5, LAUNCH_DMA, SRC_MEMORY_LAYOUT, PITCH) |
           HWCONST(C3B5, LAUNCH_DMA, DST_MEMORY_LAYOUT, PITCH) |
           HWCONST(C3B5, LAUNCH_DMA, MULTI_LINE_ENABLE, FALSE) |
           HWCONST(C3B5, LAUNCH_DMA, REMAP_ENABLE, TRUE) |
           flush_value |
           launch_dma_dst_type |
           launch_dma_plc_mode |
           pipelined_value);

        dst.address += memset_this_time * memset_element_size;
        size -= memset_this_time;
        pipelined_value = HWCONST(C3B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, NON_PIPELINED);
    } while (size > 0);
}

void uvm_hal_volta_ce_memset_1(uvm_push_t *push, uvm_gpu_address_t dst, NvU8 value, size_t size)
{
    NV_PUSH_2U(C3B5, SET_REMAP_CONST_B,    (NvU32)value,
                     SET_REMAP_COMPONENTS,
       HWCONST(C3B5, SET_REMAP_COMPONENTS, DST_X,               CONST_B) |
       HWCONST(C3B5, SET_REMAP_COMPONENTS, COMPONENT_SIZE,      ONE)     |
       HWCONST(C3B5, SET_REMAP_COMPONENTS, NUM_DST_COMPONENTS,  ONE));

    memset_common(push, dst, size, 1);
}

void uvm_hal_volta_ce_memset_4(uvm_push_t *push, uvm_gpu_address_t dst, NvU32 value, size_t size)
{
    UVM_ASSERT_MSG(size % 4 == 0, "size: %zd\n", size);

    size /= 4;

    NV_PUSH_2U(C3B5, SET_REMAP_CONST_B,    value,
                     SET_REMAP_COMPONENTS,
       HWCONST(C3B5, SET_REMAP_COMPONENTS, DST_X,               CONST_B) |
       HWCONST(C3B5, SET_REMAP_COMPONENTS, COMPONENT_SIZE,      FOUR)    |
       HWCONST(C3B5, SET_REMAP_COMPONENTS, NUM_DST_COMPONENTS,  ONE));

    memset_common(push, dst, size, 4);
}

void uvm_hal_volta_ce_memset_8(uvm_push_t *push, uvm_gpu_address_t dst, NvU64 value, size_t size)
{
    UVM_ASSERT_MSG(size % 8 == 0, "size: %zd\n", size);

    size /= 8;

    NV_PUSH_3U(C3B5, SET_REMAP_CONST_A, (NvU32)value,
                     SET_REMAP_CONST_B, (NvU32)(value >> 32),
                     SET_REMAP_COMPONENTS,
       HWCONST(C3B5, SET_REMAP_COMPONENTS, DST_X,               CONST_A) |
       HWCONST(C3B5, SET_REMAP_COMPONENTS, DST_Y,               CONST_B) |
       HWCONST(C3B5, SET_REMAP_COMPONENTS, COMPONENT_SIZE,      FOUR)    |
       HWCONST(C3B5, SET_REMAP_COMPONENTS, NUM_DST_COMPONENTS,  TWO));

    memset_common(push, dst, size, 8);
}

