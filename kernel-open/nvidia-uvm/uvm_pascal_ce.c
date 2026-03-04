/*******************************************************************************
    Copyright (c) 2016-2020 NVIDIA Corporation

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
#include "clc0b5.h"

void uvm_hal_pascal_ce_offset_out(uvm_push_t *push, NvU64 offset_out)
{
    NV_PUSH_2U(C0B5, OFFSET_OUT_UPPER, HWVALUE(C0B5, OFFSET_OUT_UPPER, UPPER, NvOffset_HI32(offset_out)),
                     OFFSET_OUT_LOWER, HWVALUE(C0B5, OFFSET_OUT_LOWER, VALUE, NvOffset_LO32(offset_out)));
}

void uvm_hal_pascal_ce_offset_in_out(uvm_push_t *push, NvU64 offset_in, NvU64 offset_out)
{
    NV_PUSH_4U(C0B5, OFFSET_IN_UPPER,  HWVALUE(C0B5, OFFSET_IN_UPPER,  UPPER, NvOffset_HI32(offset_in)),
                     OFFSET_IN_LOWER,  HWVALUE(C0B5, OFFSET_IN_LOWER,  VALUE, NvOffset_LO32(offset_in)),
                     OFFSET_OUT_UPPER, HWVALUE(C0B5, OFFSET_OUT_UPPER, UPPER, NvOffset_HI32(offset_out)),
                     OFFSET_OUT_LOWER, HWVALUE(C0B5, OFFSET_OUT_LOWER, VALUE, NvOffset_LO32(offset_out)));
}

void uvm_hal_pascal_ce_semaphore_release(uvm_push_t *push, NvU64 gpu_va, NvU32 payload)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    NvU32 flush_value;
    NvU32 launch_dma_plc_mode;
    bool use_flush;

    UVM_ASSERT_MSG(gpu->parent->ce_hal->semaphore_target_is_valid(push, gpu_va),
                   "Semaphore target validation failed in channel %s, GPU %s.\n",
                   push->channel->name,
                   uvm_gpu_name(gpu));

    use_flush = uvm_hal_membar_before_semaphore(push);

    if (use_flush)
        flush_value = HWCONST(C0B5, LAUNCH_DMA, FLUSH_ENABLE, TRUE);
    else
        flush_value = HWCONST(C0B5, LAUNCH_DMA, FLUSH_ENABLE, FALSE);

    NV_PUSH_3U(C0B5, SET_SEMAPHORE_A, HWVALUE(C0B5, SET_SEMAPHORE_A, UPPER, NvOffset_HI32(gpu_va)),
                     SET_SEMAPHORE_B, HWVALUE(C0B5, SET_SEMAPHORE_B, LOWER, NvOffset_LO32(gpu_va)),
                     SET_SEMAPHORE_PAYLOAD, payload);

    launch_dma_plc_mode = gpu->parent->ce_hal->plc_mode();

    NV_PUSH_1U(C0B5, LAUNCH_DMA, flush_value |
       HWCONST(C0B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, NONE) |
       HWCONST(C0B5, LAUNCH_DMA, SEMAPHORE_TYPE, RELEASE_ONE_WORD_SEMAPHORE) |
       launch_dma_plc_mode);
}

void uvm_hal_pascal_ce_semaphore_reduction_inc(uvm_push_t *push, NvU64 gpu_va, NvU32 payload)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    NvU32 flush_value;
    NvU32 launch_dma_plc_mode;
    bool use_flush;

    UVM_ASSERT_MSG(gpu->parent->ce_hal->semaphore_target_is_valid(push, gpu_va),
                   "Semaphore target validation failed in channel %s, GPU %s.\n",
                   push->channel->name,
                   uvm_gpu_name(gpu));

    use_flush = uvm_hal_membar_before_semaphore(push);

    if (use_flush)
        flush_value = HWCONST(C0B5, LAUNCH_DMA, FLUSH_ENABLE, TRUE);
    else
        flush_value = HWCONST(C0B5, LAUNCH_DMA, FLUSH_ENABLE, FALSE);

    NV_PUSH_3U(C0B5, SET_SEMAPHORE_A, HWVALUE(C0B5, SET_SEMAPHORE_A, UPPER, NvOffset_HI32(gpu_va)),
                     SET_SEMAPHORE_B, HWVALUE(C0B5, SET_SEMAPHORE_B, LOWER, NvOffset_LO32(gpu_va)),
                     SET_SEMAPHORE_PAYLOAD, payload);

    launch_dma_plc_mode = gpu->parent->ce_hal->plc_mode();

    NV_PUSH_1U(C0B5, LAUNCH_DMA, flush_value |
       HWCONST(C0B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, NONE) |
       HWCONST(C0B5, LAUNCH_DMA, SEMAPHORE_TYPE, RELEASE_ONE_WORD_SEMAPHORE) |
       HWCONST(C0B5, LAUNCH_DMA, SEMAPHORE_REDUCTION, INC) |
       HWCONST(C0B5, LAUNCH_DMA, SEMAPHORE_REDUCTION_SIGN, UNSIGNED) |
       HWCONST(C0B5, LAUNCH_DMA, SEMAPHORE_REDUCTION_ENABLE, TRUE) |
       launch_dma_plc_mode);
}

void uvm_hal_pascal_ce_semaphore_timestamp(uvm_push_t *push, NvU64 gpu_va)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    NvU32 flush_value;
    NvU32 launch_dma_plc_mode;
    bool use_flush;

    UVM_ASSERT_MSG(gpu->parent->ce_hal->semaphore_target_is_valid(push, gpu_va),
                   "Semaphore target validation failed in channel %s, GPU %s.\n",
                   push->channel->name,
                   uvm_gpu_name(gpu));

    use_flush = uvm_hal_membar_before_semaphore(push);

    if (use_flush)
        flush_value = HWCONST(C0B5, LAUNCH_DMA, FLUSH_ENABLE, TRUE);
    else
        flush_value = HWCONST(C0B5, LAUNCH_DMA, FLUSH_ENABLE, FALSE);

    NV_PUSH_3U(C0B5, SET_SEMAPHORE_A, HWVALUE(C0B5, SET_SEMAPHORE_A, UPPER, NvOffset_HI32(gpu_va)),
                     SET_SEMAPHORE_B, HWVALUE(C0B5, SET_SEMAPHORE_B, LOWER, NvOffset_LO32(gpu_va)),
                     SET_SEMAPHORE_PAYLOAD, 0xdeadbeef);

    launch_dma_plc_mode = gpu->parent->ce_hal->plc_mode();

    NV_PUSH_1U(C0B5, LAUNCH_DMA, flush_value |
       HWCONST(C0B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, NONE) |
       HWCONST(C0B5, LAUNCH_DMA, SEMAPHORE_TYPE, RELEASE_FOUR_WORD_SEMAPHORE) |
       launch_dma_plc_mode);
}

