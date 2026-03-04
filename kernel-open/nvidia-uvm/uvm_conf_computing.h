/*******************************************************************************
    Copyright (c) 2021-2024 NVIDIA Corporation

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

#ifndef __UVM_CONF_COMPUTING_H__
#define __UVM_CONF_COMPUTING_H__

#include "nv_uvm_types.h"
#include "uvm_forward_decl.h"
#include "uvm_lock.h"
#include "uvm_tracker.h"
#include "uvm_va_block_types.h"

#include "linux/list.h"

#define UVM_CONF_COMPUTING_AUTH_TAG_SIZE (UVM_CSL_CRYPT_AUTH_TAG_SIZE_BYTES)

// An authentication tag pointer is required by HW to be 16-bytes aligned.
#define UVM_CONF_COMPUTING_AUTH_TAG_ALIGNMENT 16

// An IV pointer is required by HW to be 16-bytes aligned.
//
// Use sizeof(UvmCslIv) to refer to the IV size.
#define UVM_CONF_COMPUTING_IV_ALIGNMENT 16

// SEC2 decrypt operation buffers are required to be 16-bytes aligned.
#define UVM_CONF_COMPUTING_SEC2_BUF_ALIGNMENT 16

// CE encrypt/decrypt can be unaligned if the entire buffer lies in a single
// 32B segment. Otherwise, it needs to be 32B aligned.
#define UVM_CONF_COMPUTING_BUF_ALIGNMENT 32

#define UVM_CONF_COMPUTING_DMA_BUFFER_SIZE UVM_VA_BLOCK_SIZE

// SEC2 supports at most a stream of 64 entries in the method stream for
// signing. Each entry is made of the method address and method data, therefore
// the maximum buffer size is: UVM_METHOD_SIZE * 2 * 64 = 512.
// UVM, however, won't use this amount of entries, in the worst case scenario,
// we push a semaphore_releases or a decrypt. A SEC2 semaphore_release uses 6 1U
// entries, whereas a SEC2 decrypt uses 10 1U entries. For 10 entries,
// UVM_METHOD_SIZE * 2 * 10 = 80.
#define UVM_CONF_COMPUTING_SIGN_BUF_MAX_SIZE 80

void uvm_conf_computing_check_parent_gpu(const uvm_parent_gpu_t *parent);

typedef struct
{
    // List of free DMA buffers (uvm_conf_computing_dma_buffer_t).
    // A free DMA buffer can be grabbed anytime, though the tracker
    // inside it may still have pending work.
    struct list_head free_dma_buffers;

    // Used to grow the pool when full.
    size_t num_dma_buffers;

    // Lock protecting the dma_buffer_pool
    uvm_mutex_t lock;
} uvm_conf_computing_dma_buffer_pool_t;

typedef struct
{
    // Backing DMA allocation
    uvm_mem_t *alloc;

    // Used internally by the pool management code to track the state of
    // a free buffer.
    uvm_tracker_t tracker;

    // When the DMA buffer is used as the destination of a GPU encryption, the
    // engine (CE or SEC2) writes the authentication tag here. When the buffer
    // is decrypted on the CPU the authentication tag is used by CSL to verify
    // the authenticity. The allocation is big enough for one authentication
    // tag per PAGE_SIZE page in the alloc buffer.
    uvm_mem_t *auth_tag;

    // CSL supports out-of-order decryption, the decrypt IV is used similarly
    // to the authentication tag. The allocation is big enough for one IV per
    // PAGE_SIZE page in the alloc buffer. The granularity between the decrypt
    // IV and authentication tag must match.
    UvmCslIv decrypt_iv[UVM_CONF_COMPUTING_DMA_BUFFER_SIZE / PAGE_SIZE];

    // When the DMA buffer is used as the destination of a GPU encryption, the
    // key version used during GPU encryption of each PAGE_SIZE page can be
    // saved here, so CPU decryption uses the correct decryption key.
    NvU32 key_version[UVM_CONF_COMPUTING_DMA_BUFFER_SIZE / PAGE_SIZE];

    // Bitmap of the encrypted pages in the backing allocation
    uvm_page_mask_t encrypted_page_mask;

    // See uvm_conf_computing_dma_pool lists
    struct list_head node;
} uvm_conf_computing_dma_buffer_t;

// Retrieve a DMA buffer from the given DMA allocation pool.
// NV_OK                Stage buffer successfully retrieved
// NV_ERR_NO_MEMORY     No free DMA buffers are available for grab, and
//                      expanding the memory pool to get new ones failed.
//
// out_dma_buffer is only valid if NV_OK is returned. The caller is responsible
// for calling uvm_conf_computing_dma_buffer_free once the operations on this
// buffer are done.
// When out_tracker is passed to the function, the buffer's dependencies are
// added to the tracker. The caller is guaranteed that all pending tracker
// entries come from the same GPU as the pool's owner. Before being able to use
// the DMA buffer, the caller is responsible for either acquiring or waiting
// on out_tracker. If out_tracker is NULL, the wait happens in the allocation
// itself.
// Upon success the encrypted_page_mask is cleared as part of the allocation.
NV_STATUS uvm_conf_computing_dma_buffer_alloc(uvm_conf_computing_dma_buffer_pool_t *dma_buffer_pool,
                                              uvm_conf_computing_dma_buffer_t **out_dma_buffer,
                                              uvm_tracker_t *out_tracker);

// Free a DMA buffer to the DMA allocation pool. All DMA buffers must be freed
// prior to GPU deinit.
//
// The tracker is optional and a NULL tracker indicates that no new operation
// has been pushed for the buffer. A non-NULL tracker indicates any additional
// pending operations on the buffer pushed by the caller that need to be
// synchronized before freeing or re-using the buffer.
void uvm_conf_computing_dma_buffer_free(uvm_conf_computing_dma_buffer_pool_t *dma_buffer_pool,
                                        uvm_conf_computing_dma_buffer_t *dma_buffer,
                                        uvm_tracker_t *tracker);

// Synchronize trackers in all entries in the GPU's DMA pool
void uvm_conf_computing_dma_buffer_pool_sync(uvm_conf_computing_dma_buffer_pool_t *dma_buffer_pool);


// Initialization and deinitialization of Confidential Computing data structures
// for the given GPU.
NV_STATUS uvm_conf_computing_gpu_init(uvm_gpu_t *gpu);
void uvm_conf_computing_gpu_deinit(uvm_gpu_t *gpu);

// Logs encryption information from the GPU and returns the IV.
void uvm_conf_computing_log_gpu_encryption(uvm_channel_t *channel, size_t size, UvmCslIv *iv);

// Acquires next CPU encryption IV and returns it.
void uvm_conf_computing_acquire_encryption_iv(uvm_channel_t *channel, UvmCslIv *iv);

// CPU side encryption helper with explicit IV, which is obtained from
// uvm_conf_computing_acquire_encryption_iv. Without an explicit IV
// the function uses the next IV in order. Encrypts data in src_plain and
// write the cipher text in dst_cipher. src_plain and dst_cipher can't overlap.
// The IV is invalidated and can't be used again after this operation.
void uvm_conf_computing_cpu_encrypt(uvm_channel_t *channel,
                                    void *dst_cipher,
                                    const void *src_plain,
                                    UvmCslIv *encrypt_iv,
                                    size_t size,
                                    void *auth_tag_buffer);

// CPU side decryption helper. Decrypts data from src_cipher and writes the
// plain text in dst_plain. src_cipher and dst_plain can't overlap. IV obtained
// from uvm_conf_computing_log_gpu_encryption() needs to be be passed to src_iv.
//
// The caller must indicate which key to use for decryption by passing the
// appropiate key version number.
NV_STATUS uvm_conf_computing_cpu_decrypt(uvm_channel_t *channel,
                                         void *dst_plain,
                                         const void *src_cipher,
                                         const UvmCslIv *src_iv,
                                         NvU32 key_version,
                                         size_t size,
                                         const void *auth_tag_buffer);

// CPU decryption of a single replayable fault, encrypted by GSP-RM.
//
// Replayable fault decryption depends not only on the encrypted fault contents,
// and the authentication tag, but also on the plaintext valid bit associated
// with the fault.
//
// When decrypting data previously encrypted by the Copy Engine, use
// uvm_conf_computing_cpu_decrypt instead.
//
// Locking: this function must be invoked while holding the replayable ISR lock.
NV_STATUS uvm_conf_computing_fault_decrypt(uvm_parent_gpu_t *parent_gpu,
                                           void *dst_plain,
                                           const void *src_cipher,
                                           const void *auth_tag_buffer,
                                           NvU8 valid);

// Increment the CPU-side decrypt IV of the CSL context associated with
// replayable faults.
//
// The IV associated with a fault CSL context is a 64-bit counter.
//
// Locking: this function must be invoked while holding the replayable ISR lock.
void uvm_conf_computing_fault_increment_decrypt_iv(uvm_parent_gpu_t *parent_gpu);

// Query the number of remaining messages before IV needs to be rotated.
void uvm_conf_computing_query_message_pools(uvm_channel_t *channel,
                                            NvU64 *remaining_encryptions,
                                            NvU64 *remaining_decryptions);

// Check if there are more than uvm_conf_computing_channel_iv_rotation_limit
// messages available in the channel and try to rotate if not.
NV_STATUS uvm_conf_computing_maybe_rotate_channel_ivs(uvm_channel_t *channel);

// Check if there are more than uvm_conf_computing_channel_iv_rotation_limit
// messages available in the channel and rotate if not.
NV_STATUS uvm_conf_computing_maybe_rotate_channel_ivs_retry_busy(uvm_channel_t *channel);

// Check if there are fewer than 'limit' messages available in either direction
// and rotate if not.
NV_STATUS uvm_conf_computing_rotate_channel_ivs_below_limit(uvm_channel_t *channel, NvU64 limit, bool retry_if_busy);

// Rotate the engine key associated with the given channel pool.
NV_STATUS uvm_conf_computing_rotate_pool_key(uvm_channel_pool_t *pool);

// Returns true if key rotation is allowed in the channel pool.
bool uvm_conf_computing_is_key_rotation_enabled_in_pool(uvm_channel_pool_t *pool);

// Returns true if key rotation is pending in the channel pool.
bool uvm_conf_computing_is_key_rotation_pending_in_pool(uvm_channel_pool_t *pool);

// Enable/disable key rotation in the passed GPU. Note that UVM enablement is
// dependent on RM enablement: key rotation may still be disabled upon calling
// this function, if it is disabled in RM. On the other hand, key rotation can
// be disabled in UVM, even if it is enabled in RM.
//
// Enablement/Disablement affects only kernel key rotation in keys owned by UVM.
// It doesn't affect user key rotation (CUDA, Video...), nor it affects RM
// kernel key rotation.
void uvm_conf_computing_enable_key_rotation(uvm_gpu_t *gpu);
void uvm_conf_computing_disable_key_rotation(uvm_gpu_t *gpu);

// Returns true if key rotation is enabled on UVM in the given GPU. Key rotation
// can be enabled on the GPU but disabled on some of GPU engines (LCEs or SEC2),
// see uvm_conf_computing_is_key_rotation_enabled_in_pool.
bool uvm_conf_computing_is_key_rotation_enabled(uvm_gpu_t *gpu);

// Launch a synchronous, encrypted copy between CPU and GPU.
//
// The maximum copy size allowed is UVM_CONF_COMPUTING_DMA_BUFFER_SIZE.
//
// The source CPU buffer pointed by src_plain contains the unencrypted (plain
// text) contents; the function internally performs a CPU-side encryption step
// before launching the GPU-side CE decryption. The source buffer can be in
// protected or unprotected sysmem, while the destination buffer must be in
// protected vidmem.
//
// The input tracker, if not NULL, is internally acquired by the push
// responsible for the encrypted copy.
__attribute__ ((format(printf, 6, 7)))
NV_STATUS uvm_conf_computing_util_memcopy_cpu_to_gpu(uvm_gpu_t *gpu,
                                                     uvm_gpu_address_t dst_gpu_address,
                                                     void *src_plain,
                                                     size_t size,
                                                     uvm_tracker_t *tracker,
                                                     const char *format,
                                                     ...);

// Launch a synchronous, encrypted copy between CPU and GPU.
//
// The maximum copy size allowed is UVM_CONF_COMPUTING_DMA_BUFFER_SIZE.
//
// The source CPU buffer pointed by src_plain contains the unencrypted (plain
// text) contents; the function internally performs a CPU-side encryption step
// before launching the GPU-side CE decryption. The source buffer can be in
// protected or unprotected sysmem, while the destination buffer must be in
// protected vidmem.
//
// The input tracker, if not NULL, is internally acquired by the push
// responsible for the encrypted copy.
__attribute__ ((format(printf, 6, 7)))
NV_STATUS uvm_conf_computing_util_memcopy_gpu_to_cpu(uvm_gpu_t *gpu,
                                                     void *dst_plain,
                                                     uvm_gpu_address_t src_gpu_address,
                                                     size_t size,
                                                     uvm_tracker_t *tracker,
                                                     const char *format,
                                                     ...);
#endif // __UVM_CONF_COMPUTING_H__
