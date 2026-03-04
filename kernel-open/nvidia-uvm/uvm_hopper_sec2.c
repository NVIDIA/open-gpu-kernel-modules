/*******************************************************************************
    Copyright (c) 2022-2023 NVIDIA Corporation

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
#include "uvm_hal_types.h"
#include "uvm_push.h"
#include "uvm_push_macros.h"
#include "nv_uvm_types.h"
#include "nv_uvm_interface.h"
#include "clcba2.h"
#include "clc86f.h"
#include "clb06f.h"

#define UVM_CSL_SIGN_AUTH_TAG_ALIGNMENT_BYTES (1 << HWSHIFT(CBA2, METHOD_STREAM_AUTH_TAG_ADDR_LO, DATA))

static void sign_push(uvm_push_t *push, NvU32 *init_method, NvU8 *auth_tag)
{
    NvU32 *sign_input_buf = push->begin + UVM_METHOD_SIZE / sizeof(*push->begin);
    NvU32 sign_size = 0;
    NV_STATUS status;

    UVM_ASSERT(init_method < push->next);

    while (init_method < push->next) {
        NvU8 subch = READ_HWVALUE(*init_method, B06F, DMA, METHOD_SUBCHANNEL);
        NvU32 count = READ_HWVALUE(*init_method, B06F, DMA, METHOD_COUNT);

        if (subch == UVM_SUBCHANNEL_CBA2) {
            NvU32 method_addr = READ_HWVALUE(*init_method, B06F, DMA, METHOD_ADDRESS) << 2;

            UVM_ASSERT(count == 1);
            UVM_ASSERT((sign_size + 2) * UVM_METHOD_SIZE <= UVM_CONF_COMPUTING_SIGN_BUF_MAX_SIZE);

            sign_input_buf[sign_size] = method_addr;
            sign_input_buf[sign_size + 1] = init_method[1];

            // We consume the method address and the method data from the input,
            // we advance the sign_input_buf by 2.
            sign_size += 2;
        }

        init_method += (count + 1);
    }

    UVM_ASSERT(sign_size > 0);

    status = nvUvmInterfaceCslSign(&push->channel->csl.ctx,
                                   sign_size * UVM_METHOD_SIZE,
                                   (NvU8 *)sign_input_buf,
                                   auth_tag);

    UVM_ASSERT_MSG(status == NV_OK,
                   "Failure to sign method stream auth tag, err: %s, GPU: %s.\n",
                   nvstatusToString(status),
                   uvm_gpu_name(uvm_push_get_gpu(push)));
}

void uvm_hal_hopper_sec2_init(uvm_push_t *push)
{
    // Commonly, we would push a SET_OBJECT HOPPER_SEC2_WORK_LAUNCH_A in the
    // init function. During channel initialization, this method would be sent
    // to ESCHED to notify the expected SEC2 class ID. ESCHED forwards this
    // method to the SEC2 engine. SEC2 is not guaranteed to support the
    // SET_OBJECT method, so we shouldn't submit it.
}

void uvm_hal_hopper_sec2_semaphore_release(uvm_push_t *push, NvU64 gpu_va, NvU32 payload)
{
    NvU32 sem_lo;
    NvU32 flush_value;
    NvU8 *sign_auth_tag_ptr;
    NvU32 sign_auth_tag_addr_lo;
    uvm_gpu_address_t sign_auth_tag_gpu_va;
    NvU32 *csl_sign_init = push->next;

    UVM_ASSERT_MSG(uvm_push_get_gpu(push)->parent->sec2_hal->semaphore_target_is_valid(push, gpu_va),
                   "Semaphore target validation failed in channel %s, GPU %s.\n",
                   push->channel->name,
                   uvm_gpu_name(uvm_push_get_gpu(push)));

    UVM_ASSERT(IS_ALIGNED(NvU64_LO32(gpu_va), 1 << HWSHIFT(CBA2, SEMAPHORE_B, LOWER)));

    sem_lo = READ_HWVALUE(NvU64_LO32(gpu_va), CBA2, SEMAPHORE_B, LOWER);

    flush_value = uvm_hal_membar_before_semaphore(push) ? HWCONST(CBA2, SEMAPHORE_D, FLUSH_DISABLE, FALSE) :
                                                          HWCONST(CBA2, SEMAPHORE_D, FLUSH_DISABLE, TRUE);

    // The push and the method stream signature have the same lifetime, we
    // reserve space in the pushbuffer to store the signature. After the push is
    // processed by the GPU, the pushbuffer is recycled entirely, including the
    // signature buffer.
    sign_auth_tag_ptr = uvm_push_get_single_inline_buffer(push,
                                                          UVM_CSL_SIGN_AUTH_TAG_SIZE_BYTES,
                                                          UVM_CSL_SIGN_AUTH_TAG_ALIGNMENT_BYTES,
                                                          &sign_auth_tag_gpu_va);

    NV_PUSH_1U(CBA2,
               METHOD_STREAM_AUTH_TAG_ADDR_HI,
               HWVALUE(CBA2, METHOD_STREAM_AUTH_TAG_ADDR_HI, DATA, NvU64_HI32(sign_auth_tag_gpu_va.address)));

    sign_auth_tag_addr_lo = READ_HWVALUE(NvU64_LO32(sign_auth_tag_gpu_va.address),
                                         CBA2,
                                         METHOD_STREAM_AUTH_TAG_ADDR_LO,
                                         DATA);
    NV_PUSH_1U(CBA2,
               METHOD_STREAM_AUTH_TAG_ADDR_LO,
               HWVALUE(CBA2, METHOD_STREAM_AUTH_TAG_ADDR_LO, DATA, sign_auth_tag_addr_lo));

    NV_PUSH_1U(CBA2, SEMAPHORE_A, HWVALUE(CBA2, SEMAPHORE_A, UPPER, NvU64_HI32(gpu_va)));
    NV_PUSH_1U(CBA2, SEMAPHORE_B, HWVALUE(CBA2, SEMAPHORE_B, LOWER, sem_lo));
    NV_PUSH_1U(CBA2, SET_SEMAPHORE_PAYLOAD_LOWER, payload);

    NV_PUSH_1U(CBA2, SEMAPHORE_D, HWCONST(CBA2, SEMAPHORE_D, TIMESTAMP, DISABLE) |
                                  HWCONST(CBA2, SEMAPHORE_D, PAYLOAD_SIZE, 32_BIT) |
                                  flush_value);

    sign_push(push, csl_sign_init, sign_auth_tag_ptr);
}

void uvm_hal_hopper_sec2_semaphore_timestamp_unsupported(uvm_push_t *push, NvU64 gpu_va)
{
    // TODO: Bug 3804752: [uvm][HCC] Add support for Hopper SEC2 HAL in UVM.
    // Semaphore_timestamp is not implemented in the SEC2 engine yet. We will
    // add support in UVM when they become available.
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    UVM_ASSERT_MSG(false, "SEC2 semaphore_timestamp is not supported on GPU: %s.\n", uvm_gpu_name(gpu));
}

static void execute_with_membar(uvm_push_t *push)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    uvm_membar_t membar = uvm_push_get_and_reset_membar_flag(push);

    NvU32 flush_value = (membar == UVM_MEMBAR_SYS) ? HWCONST(CBA2, EXECUTE, FLUSH_DISABLE, FALSE) :
                                                     HWCONST(CBA2, EXECUTE, FLUSH_DISABLE, TRUE);

    NV_PUSH_1U(CBA2, EXECUTE, flush_value |
                              HWCONST(CBA2, EXECUTE, NOTIFY, DISABLE));

    if (membar == UVM_MEMBAR_GPU) {
        gpu->parent->host_hal->wait_for_idle(push);
        gpu->parent->host_hal->membar_gpu(push);
    }
}

void uvm_hal_hopper_sec2_decrypt(uvm_push_t *push, NvU64 dst_va, NvU64 src_va, NvU32 size, NvU64 auth_tag_va)
{
    NvU8 *sign_auth_tag_ptr;
    NvU32 sign_auth_tag_addr_lo;
    uvm_gpu_address_t sign_auth_tag_gpu_va;
    NvU32 *csl_sign_init = push->next;

    // Check that the provided alignment matches HW
    BUILD_BUG_ON(UVM_CONF_COMPUTING_SEC2_BUF_ALIGNMENT != (1 << HWSHIFT(CBA2, DECRYPT_COPY_DST_ADDR_LO, DATA)));
    BUILD_BUG_ON(UVM_CONF_COMPUTING_BUF_ALIGNMENT < (1 << HWSHIFT(CBA2, DECRYPT_COPY_DST_ADDR_LO, DATA)));
    BUILD_BUG_ON(UVM_CONF_COMPUTING_BUF_ALIGNMENT % (1 << HWSHIFT(CBA2, DECRYPT_COPY_DST_ADDR_LO, DATA)) != 0);

    // No overlapping.
    UVM_ASSERT(!uvm_ranges_overlap(src_va, src_va + size - 1, dst_va, dst_va + size - 1));

    // Alignment requirements.
    UVM_ASSERT(IS_ALIGNED(NvU64_LO32(src_va), 1 <<  HWSHIFT(CBA2, DECRYPT_COPY_SRC_ADDR_LO, DATA)));
    UVM_ASSERT(IS_ALIGNED(NvU64_LO32(dst_va), 1 << HWSHIFT(CBA2, DECRYPT_COPY_DST_ADDR_LO, DATA)));
    UVM_ASSERT(IS_ALIGNED(NvU64_LO32(auth_tag_va), 1 << HWSHIFT(CBA2, DECRYPT_COPY_AUTH_TAG_ADDR_LO, DATA)));
    UVM_ASSERT(IS_ALIGNED(size, 1 << HWSHIFT(CBA2, DECRYPT_COPY_SIZE, DATA)));

    // See comments in SEC2 semaphore_release.
    sign_auth_tag_ptr = uvm_push_get_single_inline_buffer(push,
                                                          UVM_CSL_SIGN_AUTH_TAG_SIZE_BYTES,
                                                          UVM_CSL_SIGN_AUTH_TAG_ALIGNMENT_BYTES,
                                                          &sign_auth_tag_gpu_va);

    NV_PUSH_1U(CBA2, DECRYPT_COPY_SRC_ADDR_HI, NvU64_HI32(src_va));
    NV_PUSH_1U(CBA2, DECRYPT_COPY_SRC_ADDR_LO, NvU64_LO32(src_va));

    NV_PUSH_1U(CBA2, DECRYPT_COPY_DST_ADDR_HI, NvU64_HI32(dst_va));
    NV_PUSH_1U(CBA2, DECRYPT_COPY_DST_ADDR_LO, NvU64_LO32(dst_va));

    NV_PUSH_1U(CBA2, DECRYPT_COPY_SIZE, size);
    NV_PUSH_1U(CBA2, DECRYPT_COPY_AUTH_TAG_ADDR_HI, NvU64_HI32(auth_tag_va));
    NV_PUSH_1U(CBA2, DECRYPT_COPY_AUTH_TAG_ADDR_LO, NvU64_LO32(auth_tag_va));

    NV_PUSH_1U(CBA2,
               METHOD_STREAM_AUTH_TAG_ADDR_HI,
               HWVALUE(CBA2, METHOD_STREAM_AUTH_TAG_ADDR_HI, DATA, NvU64_HI32(sign_auth_tag_gpu_va.address)));

    sign_auth_tag_addr_lo = READ_HWVALUE(NvU64_LO32(sign_auth_tag_gpu_va.address),
                                         CBA2,
                                         METHOD_STREAM_AUTH_TAG_ADDR_LO,
                                         DATA);
    NV_PUSH_1U(CBA2,
               METHOD_STREAM_AUTH_TAG_ADDR_LO,
               HWVALUE(CBA2, METHOD_STREAM_AUTH_TAG_ADDR_LO, DATA, sign_auth_tag_addr_lo));

    execute_with_membar(push);

    sign_push(push, csl_sign_init, sign_auth_tag_ptr);
}
