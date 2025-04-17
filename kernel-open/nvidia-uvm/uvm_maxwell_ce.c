/*******************************************************************************
    Copyright (c) 2021-2023 NVIDIA Corporation

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
#include "clb06f.h"
#include "clb0b5.h"

void uvm_hal_maxwell_ce_init(uvm_push_t *push)
{
    // Notably this sends SET_OBJECT with the CE class on subchannel 0 instead
    // of the recommended by HW subchannel 4 (subchannel 4 is required to
    // match CE usage on GRCE). For the UVM driver using subchannel 0 has the
    // benefit of also verifying that we ended up on the right CE engine type
    // though as SET_OBJECT with CE class on subchannel 0 would fail on GRCE.
    NV_PUSH_1U(B06F, SET_OBJECT, uvm_push_get_gpu(push)->parent->rm_info.ceClass);
}

void uvm_hal_maxwell_ce_offset_out(uvm_push_t *push, NvU64 offset_out)
{
    NV_PUSH_2U(B0B5, OFFSET_OUT_UPPER, HWVALUE(B0B5, OFFSET_OUT_UPPER, UPPER, NvOffset_HI32(offset_out)),
                     OFFSET_OUT_LOWER, HWVALUE(B0B5, OFFSET_OUT_LOWER, VALUE, NvOffset_LO32(offset_out)));
}

void uvm_hal_maxwell_ce_offset_in_out(uvm_push_t *push, NvU64 offset_in, NvU64 offset_out)
{
    NV_PUSH_4U(B0B5, OFFSET_IN_UPPER,  HWVALUE(B0B5, OFFSET_IN_UPPER,  UPPER, NvOffset_HI32(offset_in)),
                     OFFSET_IN_LOWER,  HWVALUE(B0B5, OFFSET_IN_LOWER,  VALUE, NvOffset_LO32(offset_in)),
                     OFFSET_OUT_UPPER, HWVALUE(B0B5, OFFSET_OUT_UPPER, UPPER, NvOffset_HI32(offset_out)),
                     OFFSET_OUT_LOWER, HWVALUE(B0B5, OFFSET_OUT_LOWER, VALUE, NvOffset_LO32(offset_out)));
}

bool uvm_hal_maxwell_semaphore_target_is_valid(uvm_push_t *push, NvU64 gpu_va)
{
    if (uvm_gpu_address_is_peer(uvm_push_get_gpu(push), uvm_gpu_address_virtual(gpu_va))) {
        UVM_ERR_PRINT("Semaphore operation targetting peer addresses is not allowed!");
        return false;
    }

    return true;
}

void uvm_hal_maxwell_ce_semaphore_release(uvm_push_t *push, NvU64 gpu_va, NvU32 payload)
{
    NvU32 flush_value;
    bool use_flush;

    UVM_ASSERT_MSG(uvm_push_get_gpu(push)->parent->ce_hal->semaphore_target_is_valid(push, gpu_va),
                   "Semaphore target validation failed in channel %s, GPU %s.\n",
                   push->channel->name,
                   uvm_gpu_name(uvm_push_get_gpu(push)));

    use_flush = uvm_hal_membar_before_semaphore(push);

    if (use_flush)
        flush_value = HWCONST(B0B5, LAUNCH_DMA, FLUSH_ENABLE, TRUE);
    else
        flush_value = HWCONST(B0B5, LAUNCH_DMA, FLUSH_ENABLE, FALSE);

    NV_PUSH_3U(B0B5, SET_SEMAPHORE_A, HWVALUE(B0B5, SET_SEMAPHORE_A, UPPER, NvOffset_HI32(gpu_va)),
                     SET_SEMAPHORE_B, HWVALUE(B0B5, SET_SEMAPHORE_B, LOWER, NvOffset_LO32(gpu_va)),
                     SET_SEMAPHORE_PAYLOAD, payload);

    NV_PUSH_1U(B0B5, LAUNCH_DMA, flush_value |
       HWCONST(B0B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, NONE) |
       HWCONST(B0B5, LAUNCH_DMA, SEMAPHORE_TYPE, RELEASE_ONE_WORD_SEMAPHORE));
}

void uvm_hal_maxwell_ce_semaphore_reduction_inc(uvm_push_t *push, NvU64 gpu_va, NvU32 payload)
{
    NvU32 flush_value;
    bool use_flush;

    UVM_ASSERT_MSG(uvm_push_get_gpu(push)->parent->ce_hal->semaphore_target_is_valid(push, gpu_va),
                   "Semaphore target validation failed in channel %s, GPU %s.\n",
                   push->channel->name,
                   uvm_gpu_name(uvm_push_get_gpu(push)));

    use_flush = uvm_hal_membar_before_semaphore(push);

    if (use_flush)
        flush_value = HWCONST(B0B5, LAUNCH_DMA, FLUSH_ENABLE, TRUE);
    else
        flush_value = HWCONST(B0B5, LAUNCH_DMA, FLUSH_ENABLE, FALSE);

    NV_PUSH_3U(B0B5, SET_SEMAPHORE_A, HWVALUE(B0B5, SET_SEMAPHORE_A, UPPER, NvOffset_HI32(gpu_va)),
                     SET_SEMAPHORE_B, HWVALUE(B0B5, SET_SEMAPHORE_B, LOWER, NvOffset_LO32(gpu_va)),
                     SET_SEMAPHORE_PAYLOAD, payload);

    NV_PUSH_1U(B0B5, LAUNCH_DMA, flush_value |
       HWCONST(B0B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, NONE) |
       HWCONST(B0B5, LAUNCH_DMA, SEMAPHORE_TYPE, RELEASE_ONE_WORD_SEMAPHORE) |
       HWCONST(B0B5, LAUNCH_DMA, SEMAPHORE_REDUCTION, INC) |
       HWCONST(B0B5, LAUNCH_DMA, SEMAPHORE_REDUCTION_SIGN, UNSIGNED) |
       HWCONST(B0B5, LAUNCH_DMA, SEMAPHORE_REDUCTION_ENABLE, TRUE));
}

void uvm_hal_maxwell_ce_semaphore_timestamp(uvm_push_t *push, NvU64 gpu_va)
{
    NvU32 flush_value;
    bool use_flush;

    UVM_ASSERT_MSG(uvm_push_get_gpu(push)->parent->ce_hal->semaphore_target_is_valid(push, gpu_va),
                   "Semaphore target validation failed in channel %s, GPU %s.\n",
                   push->channel->name,
                   uvm_gpu_name(uvm_push_get_gpu(push)));

    use_flush = uvm_hal_membar_before_semaphore(push);

    if (use_flush)
        flush_value = HWCONST(B0B5, LAUNCH_DMA, FLUSH_ENABLE, TRUE);
    else
        flush_value = HWCONST(B0B5, LAUNCH_DMA, FLUSH_ENABLE, FALSE);

    NV_PUSH_3U(B0B5, SET_SEMAPHORE_A, HWVALUE(B0B5, SET_SEMAPHORE_A, UPPER, NvOffset_HI32(gpu_va)),
                     SET_SEMAPHORE_B, HWVALUE(B0B5, SET_SEMAPHORE_B, LOWER, NvOffset_LO32(gpu_va)),
                     SET_SEMAPHORE_PAYLOAD, 0xdeadbeef);

    NV_PUSH_1U(B0B5, LAUNCH_DMA, flush_value |
       HWCONST(B0B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, NONE) |
       HWCONST(B0B5, LAUNCH_DMA, SEMAPHORE_TYPE, RELEASE_FOUR_WORD_SEMAPHORE));
}

static void maxwell_membar_after_transfer(uvm_push_t *push)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);

    if (uvm_push_get_and_reset_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE))
        return;

    // Flush on transfers only works when paired with a semaphore release. Use a
    // host WFI + MEMBAR.
    // Bug 1709888
    gpu->parent->host_hal->wait_for_idle(push);

    if (uvm_push_get_and_reset_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_GPU))
        gpu->parent->host_hal->membar_gpu(push);
    else
        gpu->parent->host_hal->membar_sys(push);
}

static NvU32 ce_aperture(uvm_aperture_t aperture)
{
    BUILD_BUG_ON(HWCONST(B0B5, SET_SRC_PHYS_MODE, TARGET, LOCAL_FB) !=
                 HWCONST(B0B5, SET_DST_PHYS_MODE, TARGET, LOCAL_FB));
    BUILD_BUG_ON(HWCONST(B0B5, SET_SRC_PHYS_MODE, TARGET, COHERENT_SYSMEM) !=
                 HWCONST(B0B5, SET_DST_PHYS_MODE, TARGET, COHERENT_SYSMEM));

    UVM_ASSERT_MSG(aperture == UVM_APERTURE_VID || aperture == UVM_APERTURE_SYS, "aperture 0x%x\n", aperture);

    if (aperture == UVM_APERTURE_SYS)
        return HWCONST(B0B5, SET_SRC_PHYS_MODE, TARGET, COHERENT_SYSMEM);
    else
        return HWCONST(B0B5, SET_SRC_PHYS_MODE, TARGET, LOCAL_FB);
}

// Push SET_{SRC,DST}_PHYS mode if needed and return LAUNCH_DMA_{SRC,DST}_TYPE
// flags
NvU32 uvm_hal_maxwell_ce_phys_mode(uvm_push_t *push, uvm_gpu_address_t dst, uvm_gpu_address_t src)
{
    NvU32 launch_dma_src_dst_type = 0;

    if (src.is_virtual)
        launch_dma_src_dst_type |= HWCONST(B0B5, LAUNCH_DMA, SRC_TYPE, VIRTUAL);
    else
        launch_dma_src_dst_type |= HWCONST(B0B5, LAUNCH_DMA, SRC_TYPE, PHYSICAL);

    if (dst.is_virtual)
        launch_dma_src_dst_type |= HWCONST(B0B5, LAUNCH_DMA, DST_TYPE, VIRTUAL);
    else
        launch_dma_src_dst_type |= HWCONST(B0B5, LAUNCH_DMA, DST_TYPE, PHYSICAL);

    if (!src.is_virtual && !dst.is_virtual) {
        NV_PUSH_2U(B0B5, SET_SRC_PHYS_MODE, ce_aperture(src.aperture),
                         SET_DST_PHYS_MODE, ce_aperture(dst.aperture));
    }
    else if (!src.is_virtual) {
        NV_PUSH_1U(B0B5, SET_SRC_PHYS_MODE, ce_aperture(src.aperture));
    }
    else if (!dst.is_virtual) {
        NV_PUSH_1U(B0B5, SET_DST_PHYS_MODE, ce_aperture(dst.aperture));
    }

    return launch_dma_src_dst_type;
}

// Noop, since DISABLE_PLC doesn't exist in Maxwell.
NvU32 uvm_hal_maxwell_ce_plc_mode(void)
{
    return 0;
}

bool uvm_hal_maxwell_ce_memset_is_valid(uvm_push_t *push,
                                        uvm_gpu_address_t dst,
                                        size_t num_elements,
                                        size_t element_size)
{
    if (uvm_gpu_address_is_peer(uvm_push_get_gpu(push), dst)) {
        UVM_ERR_PRINT("Memset to peer address (0x%llx) is not allowed!", dst.address);
        return false;
    }

    return true;
}

bool uvm_hal_maxwell_ce_memcopy_is_valid(uvm_push_t *push, uvm_gpu_address_t dst, uvm_gpu_address_t src)
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

    return true;
}

// Noop, since COPY_TYPE doesn't exist in Maxwell.
NvU32 uvm_hal_maxwell_ce_memcopy_copy_type(uvm_gpu_address_t dst, uvm_gpu_address_t src)
{
    return 0;
}

void uvm_hal_maxwell_ce_memcopy(uvm_push_t *push, uvm_gpu_address_t dst, uvm_gpu_address_t src, size_t size)
{
    // If >4GB copies ever become an important use case, this function should
    // use multi-line transfers so we don't have to iterate (bug 1766588).
    static const size_t max_single_copy_size = 0xFFFFFFFF;
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);

    NvU32 pipelined_value;
    NvU32 launch_dma_src_dst_type;
    NvU32 launch_dma_plc_mode;
    NvU32 copy_type_value;

    UVM_ASSERT_MSG(gpu->parent->ce_hal->memcopy_is_valid(push, dst, src),
                   "Memcopy validation failed in channel %s, GPU %s.\n",
                   push->channel->name,
                   uvm_gpu_name(gpu));

    // Check if the copy is over NVLINK and simulate dropped traffic if there's
    // an NVLINK error.
    // Src address cannot be peer as that wouldn't pass the valid check above.
    if (uvm_gpu_address_is_peer(gpu, dst) && uvm_gpu_get_injected_nvlink_error(gpu) != NV_OK)
        size = 0;

    gpu->parent->ce_hal->memcopy_patch_src(push, &src);

    launch_dma_src_dst_type = gpu->parent->ce_hal->phys_mode(push, dst, src);
    launch_dma_plc_mode = gpu->parent->ce_hal->plc_mode();
    copy_type_value = gpu->parent->ce_hal->memcopy_copy_type(dst, src);

    if (uvm_push_get_and_reset_flag(push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED))
        pipelined_value = HWCONST(B0B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, PIPELINED);
    else
        pipelined_value = HWCONST(B0B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, NON_PIPELINED);

    do {
        NvU32 copy_this_time = (NvU32)min(size, max_single_copy_size);

        gpu->parent->ce_hal->offset_in_out(push, src.address, dst.address);

        NV_PUSH_1U(B0B5, LINE_LENGTH_IN, copy_this_time);

        NV_PUSH_1U(B0B5, LAUNCH_DMA,
           HWCONST(B0B5, LAUNCH_DMA, SRC_MEMORY_LAYOUT, PITCH) |
           HWCONST(B0B5, LAUNCH_DMA, DST_MEMORY_LAYOUT, PITCH) |
           HWCONST(B0B5, LAUNCH_DMA, MULTI_LINE_ENABLE, FALSE) |
           HWCONST(B0B5, LAUNCH_DMA, REMAP_ENABLE, FALSE) |
           HWCONST(B0B5, LAUNCH_DMA, FLUSH_ENABLE, FALSE) |
           launch_dma_src_dst_type |
           launch_dma_plc_mode |
           copy_type_value |
           pipelined_value);

        pipelined_value = HWCONST(B0B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, PIPELINED);
        dst.address += copy_this_time;
        src.address += copy_this_time;
        size -= copy_this_time;
    } while (size > 0);

    maxwell_membar_after_transfer(push);
}

void uvm_hal_maxwell_ce_memcopy_v_to_v(uvm_push_t *push, NvU64 dst_va, NvU64 src_va, size_t size)
{
    uvm_push_get_gpu(push)->parent->ce_hal->memcopy(push,
                                                    uvm_gpu_address_virtual(dst_va),
                                                    uvm_gpu_address_virtual(src_va),
                                                    size);
}

// Push SET_DST_PHYS mode if needed and return LAUNCH_DMA_DST_TYPE flags
static NvU32 maxwell_memset_push_phys_mode(uvm_push_t *push, uvm_gpu_address_t dst)
{
    if (dst.is_virtual)
        return HWCONST(B0B5, LAUNCH_DMA, DST_TYPE, VIRTUAL);

    NV_PUSH_1U(B0B5, SET_DST_PHYS_MODE, ce_aperture(dst.aperture));
    return HWCONST(B0B5, LAUNCH_DMA, DST_TYPE, PHYSICAL);
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

    UVM_ASSERT_MSG(gpu->parent->ce_hal->memset_is_valid(push, dst, size, memset_element_size),
                   "Memset validation failed in channel %s, GPU %s.\n",
                   push->channel->name,
                   uvm_gpu_name(gpu));

    launch_dma_dst_type = maxwell_memset_push_phys_mode(push, dst);
    launch_dma_plc_mode = gpu->parent->ce_hal->plc_mode();

    if (uvm_push_get_and_reset_flag(push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED))
        pipelined_value = HWCONST(B0B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, PIPELINED);
    else
        pipelined_value = HWCONST(B0B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, NON_PIPELINED);

    do {
        NvU32 memset_this_time = (NvU32)min(size, max_single_memset_size);

        gpu->parent->ce_hal->offset_out(push, dst.address);

        NV_PUSH_1U(B0B5, LINE_LENGTH_IN, memset_this_time);

        NV_PUSH_1U(B0B5, LAUNCH_DMA,
           HWCONST(B0B5, LAUNCH_DMA, SRC_MEMORY_LAYOUT, PITCH) |
           HWCONST(B0B5, LAUNCH_DMA, DST_MEMORY_LAYOUT, PITCH) |
           HWCONST(B0B5, LAUNCH_DMA, MULTI_LINE_ENABLE, FALSE) |
           HWCONST(B0B5, LAUNCH_DMA, REMAP_ENABLE, TRUE) |
           HWCONST(B0B5, LAUNCH_DMA, FLUSH_ENABLE, FALSE) |
           launch_dma_dst_type |
           launch_dma_plc_mode |
           pipelined_value);

        dst.address += memset_this_time * memset_element_size;
        size -= memset_this_time;
        pipelined_value = HWCONST(B0B5, LAUNCH_DMA, DATA_TRANSFER_TYPE, PIPELINED);
    } while (size > 0);

    maxwell_membar_after_transfer(push);
}

void uvm_hal_maxwell_ce_memset_1(uvm_push_t *push, uvm_gpu_address_t dst, NvU8 value, size_t size)
{
    NV_PUSH_2U(B0B5, SET_REMAP_CONST_B,    (NvU32)value,
                     SET_REMAP_COMPONENTS,
       HWCONST(B0B5, SET_REMAP_COMPONENTS, DST_X,               CONST_B) |
       HWCONST(B0B5, SET_REMAP_COMPONENTS, COMPONENT_SIZE,      ONE)     |
       HWCONST(B0B5, SET_REMAP_COMPONENTS, NUM_DST_COMPONENTS,  ONE));

    memset_common(push, dst, size, 1);
}

void uvm_hal_maxwell_ce_memset_4(uvm_push_t *push, uvm_gpu_address_t dst, NvU32 value, size_t size)
{
    UVM_ASSERT_MSG(size % 4 == 0, "size: %zd\n", size);

    size /= 4;

    NV_PUSH_2U(B0B5, SET_REMAP_CONST_B,    value,
                     SET_REMAP_COMPONENTS,
       HWCONST(B0B5, SET_REMAP_COMPONENTS, DST_X,               CONST_B) |
       HWCONST(B0B5, SET_REMAP_COMPONENTS, COMPONENT_SIZE,      FOUR)    |
       HWCONST(B0B5, SET_REMAP_COMPONENTS, NUM_DST_COMPONENTS,  ONE));

    memset_common(push, dst, size, 4);
}

void uvm_hal_maxwell_ce_memset_8(uvm_push_t *push, uvm_gpu_address_t dst, NvU64 value, size_t size)
{
    UVM_ASSERT_MSG(size % 8 == 0, "size: %zd\n", size);

    size /= 8;

    NV_PUSH_3U(B0B5, SET_REMAP_CONST_A, (NvU32)value,
                     SET_REMAP_CONST_B, (NvU32)(value >> 32),
                     SET_REMAP_COMPONENTS,
       HWCONST(B0B5, SET_REMAP_COMPONENTS, DST_X,               CONST_A) |
       HWCONST(B0B5, SET_REMAP_COMPONENTS, DST_Y,               CONST_B) |
       HWCONST(B0B5, SET_REMAP_COMPONENTS, COMPONENT_SIZE,      FOUR)    |
       HWCONST(B0B5, SET_REMAP_COMPONENTS, NUM_DST_COMPONENTS,  TWO));

    memset_common(push, dst, size, 8);
}

void uvm_hal_maxwell_ce_memset_v_4(uvm_push_t *push, NvU64 dst_va, NvU32 value, size_t size)
{
    uvm_push_get_gpu(push)->parent->ce_hal->memset_4(push, uvm_gpu_address_virtual(dst_va), value, size);
}

void uvm_hal_maxwell_ce_encrypt_unsupported(uvm_push_t *push,
                                            uvm_gpu_address_t dst,
                                            uvm_gpu_address_t src,
                                            NvU32 size,
                                            uvm_gpu_address_t auth_tag)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);

    UVM_ASSERT_MSG(false, "CE encrypt is not supported on GPU: %s.\n", uvm_gpu_name(gpu));
}

void uvm_hal_maxwell_ce_decrypt_unsupported(uvm_push_t *push,
                                            uvm_gpu_address_t dst,
                                            uvm_gpu_address_t src,
                                            NvU32 size,
                                            uvm_gpu_address_t auth_tag)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);

    UVM_ASSERT_MSG(false, "CE decrypt is not supported on GPU: %s.\n", uvm_gpu_name(gpu));
}
