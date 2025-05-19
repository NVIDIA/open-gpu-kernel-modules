/*
 * Copyright (c) 2016-2025, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "nvidia-drm-conftest.h"

#if defined(NV_DRM_AVAILABLE)

#if defined(NV_DRM_DRMP_H_PRESENT)
#include <drm/drmP.h>
#endif

#include "nvidia-drm-priv.h"
#include "nvidia-drm-ioctl.h"
#include "nvidia-drm-gem.h"
#include "nvidia-drm-fence.h"
#include "nvidia-dma-resv-helper.h"

#if defined(NV_DRM_FENCE_AVAILABLE)

#include "nvidia-dma-fence-helper.h"

#define NV_DRM_SEMAPHORE_SURFACE_FENCE_MAX_TIMEOUT_MS 5000

struct nv_drm_fence_context;

struct nv_drm_fence_context_ops {
    void (*destroy)(struct nv_drm_fence_context *nv_fence_context);
};

struct nv_drm_fence_context {
    struct nv_drm_gem_object base;

    const struct nv_drm_fence_context_ops *ops;

    struct nv_drm_device *nv_dev;
    uint64_t context;

    NvU64 fenceSemIndex; /* Index into semaphore surface */
};

struct nv_drm_prime_fence_context {
    struct nv_drm_fence_context base;

    /* Mapped semaphore surface */
    struct NvKmsKapiMemory *pSemSurface;
    NvU32 *pLinearAddress;

    /* Protects nv_drm_fence_context::{pending, last_seqno} */
    spinlock_t lock;

    /*
     * Software signaling structures. __nv_drm_prime_fence_context_new()
     * allocates channel event and __nv_drm_prime_fence_context_destroy() frees
     * it. There are no simultaneous read/write access to 'cb', therefore it
     * does not require spin-lock protection.
     */
    struct NvKmsKapiChannelEvent *cb;

    /* List of pending fences which are not yet signaled */
    struct list_head pending;

    unsigned last_seqno;
};

struct nv_drm_prime_fence {
    struct list_head list_entry;
    nv_dma_fence_t base;
    spinlock_t lock;
};

static inline
struct nv_drm_prime_fence *to_nv_drm_prime_fence(nv_dma_fence_t *fence)
{
    return container_of(fence, struct nv_drm_prime_fence, base);
}

static const char*
nv_drm_gem_fence_op_get_driver_name(nv_dma_fence_t *fence)
{
    return "NVIDIA";
}

static const char*
nv_drm_gem_prime_fence_op_get_timeline_name(nv_dma_fence_t *fence)
{
    return "nvidia.prime";
}

static bool nv_drm_gem_prime_fence_op_enable_signaling(nv_dma_fence_t *fence)
{
    // DO NOTHING
    return true;
}

static void nv_drm_gem_prime_fence_op_release(nv_dma_fence_t *fence)
{
    struct nv_drm_prime_fence *nv_fence = to_nv_drm_prime_fence(fence);
    nv_drm_free(nv_fence);
}

static signed long
nv_drm_gem_prime_fence_op_wait(nv_dma_fence_t *fence,
                               bool intr, signed long timeout)
{
    /*
     * If the waiter requests to wait with no timeout, force a timeout to ensure
     * that it won't get stuck forever in the kernel if something were to go
     * wrong with signaling, such as a malicious userspace not releasing the
     * semaphore.
     *
     * 96 ms (roughly 6 frames @ 60 Hz) is arbitrarily chosen to be long enough
     * that it should never get hit during normal operation, but not so long
     * that the system becomes unresponsive.
     */
    return nv_dma_fence_default_wait(fence, intr,
                              (timeout == MAX_SCHEDULE_TIMEOUT) ?
                                  msecs_to_jiffies(96) : timeout);
}

static const nv_dma_fence_ops_t nv_drm_gem_prime_fence_ops = {
    .get_driver_name = nv_drm_gem_fence_op_get_driver_name,
    .get_timeline_name = nv_drm_gem_prime_fence_op_get_timeline_name,
    .enable_signaling = nv_drm_gem_prime_fence_op_enable_signaling,
    .release = nv_drm_gem_prime_fence_op_release,
    .wait = nv_drm_gem_prime_fence_op_wait,
};

static inline void
__nv_drm_prime_fence_signal(struct nv_drm_prime_fence *nv_fence)
{
    list_del(&nv_fence->list_entry);
    nv_dma_fence_signal(&nv_fence->base);
    nv_dma_fence_put(&nv_fence->base);
}

static void nv_drm_gem_prime_force_fence_signal(
    struct nv_drm_prime_fence_context *nv_fence_context)
{
    WARN_ON(!spin_is_locked(&nv_fence_context->lock));

    while (!list_empty(&nv_fence_context->pending)) {
        struct nv_drm_prime_fence *nv_fence = list_first_entry(
            &nv_fence_context->pending,
            typeof(*nv_fence),
            list_entry);

        __nv_drm_prime_fence_signal(nv_fence);
    }
}

static void nv_drm_gem_prime_fence_event
(
    void *dataPtr,
    NvU32 dataU32
)
{
    struct nv_drm_prime_fence_context *nv_fence_context = dataPtr;

    spin_lock(&nv_fence_context->lock);

    while (!list_empty(&nv_fence_context->pending)) {
        struct nv_drm_prime_fence *nv_fence = list_first_entry(
            &nv_fence_context->pending,
            typeof(*nv_fence),
            list_entry);

        /* Index into surface with 16 byte stride */
        unsigned int seqno = *((nv_fence_context->pLinearAddress) +
                               (nv_fence_context->base.fenceSemIndex * 4));

        if (nv_fence->base.seqno > seqno) {
            /*
             * Fences in list are placed in increasing order of sequence
             * number, breaks a loop once found first fence not
             * ready to signal.
             */
            break;
        }

        __nv_drm_prime_fence_signal(nv_fence);
    }

    spin_unlock(&nv_fence_context->lock);
}

static inline struct nv_drm_prime_fence_context*
to_nv_prime_fence_context(struct nv_drm_fence_context *nv_fence_context) {
    return container_of(nv_fence_context, struct nv_drm_prime_fence_context, base);
}

static void __nv_drm_prime_fence_context_destroy(
    struct nv_drm_fence_context *nv_fence_context)
{
    struct nv_drm_device *nv_dev = nv_fence_context->nv_dev;
    struct nv_drm_prime_fence_context *nv_prime_fence_context =
        to_nv_prime_fence_context(nv_fence_context);

    /*
     * Free channel event before destroying the fence context, otherwise event
     * callback continue to get called.
     */
    nvKms->freeChannelEvent(nv_dev->pDevice, nv_prime_fence_context->cb);

    /* Force signal all pending fences and empty pending list */
    spin_lock(&nv_prime_fence_context->lock);

    nv_drm_gem_prime_force_fence_signal(nv_prime_fence_context);

    spin_unlock(&nv_prime_fence_context->lock);

    /* Free nvkms resources */

    nvKms->unmapMemory(nv_dev->pDevice,
                       nv_prime_fence_context->pSemSurface,
                       NVKMS_KAPI_MAPPING_TYPE_KERNEL,
                       (void *) nv_prime_fence_context->pLinearAddress);

    nvKms->freeMemory(nv_dev->pDevice, nv_prime_fence_context->pSemSurface);

    nv_drm_free(nv_fence_context);
}

static struct nv_drm_fence_context_ops nv_drm_prime_fence_context_ops = {
    .destroy = __nv_drm_prime_fence_context_destroy,
};

static inline struct nv_drm_prime_fence_context *
__nv_drm_prime_fence_context_new(
    struct nv_drm_device *nv_dev,
    struct drm_nvidia_prime_fence_context_create_params *p)
{
    struct nv_drm_prime_fence_context *nv_prime_fence_context;
    struct NvKmsKapiMemory *pSemSurface;
    NvU32 *pLinearAddress;

    /* Allocate backup nvkms resources */

    pSemSurface = nvKms->importMemory(nv_dev->pDevice,
                                      p->size,
                                      p->import_mem_nvkms_params_ptr,
                                      p->import_mem_nvkms_params_size);
    if (!pSemSurface) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to import fence semaphore surface");

        goto failed;
    }

    if (!nvKms->mapMemory(nv_dev->pDevice,
                          pSemSurface,
                          NVKMS_KAPI_MAPPING_TYPE_KERNEL,
                          (void **) &pLinearAddress)) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to map fence semaphore surface");

        goto failed_to_map_memory;
    }

    /*
     * Allocate a fence context object, initialize it and allocate channel
     * event for it.
     */

    if ((nv_prime_fence_context = nv_drm_calloc(
                    1,
                    sizeof(*nv_prime_fence_context))) == NULL) {
        goto failed_alloc_fence_context;
    }

    /*
     * nv_dma_fence_context_alloc() cannot fail, so we do not need
     * to check a return value.
     */

    nv_prime_fence_context->base.ops = &nv_drm_prime_fence_context_ops;
    nv_prime_fence_context->base.nv_dev = nv_dev;
    nv_prime_fence_context->base.context = nv_dma_fence_context_alloc(1);
    nv_prime_fence_context->base.fenceSemIndex = p->index;
    nv_prime_fence_context->pSemSurface = pSemSurface;
    nv_prime_fence_context->pLinearAddress = pLinearAddress;

    INIT_LIST_HEAD(&nv_prime_fence_context->pending);

    spin_lock_init(&nv_prime_fence_context->lock);

    /*
     * Except 'cb', the fence context should be completely initialized
     * before channel event allocation because the fence context may start
     * receiving events immediately after allocation.
     *
     * There are no simultaneous read/write access to 'cb', therefore it does
     * not require spin-lock protection.
     */
    nv_prime_fence_context->cb =
        nvKms->allocateChannelEvent(nv_dev->pDevice,
                                    nv_drm_gem_prime_fence_event,
                                    nv_prime_fence_context,
                                    p->event_nvkms_params_ptr,
                                    p->event_nvkms_params_size);
    if (!nv_prime_fence_context->cb) {
        NV_DRM_DEV_LOG_ERR(nv_dev,
                           "Failed to allocate fence signaling event");
        goto failed_to_allocate_channel_event;
    }

    return nv_prime_fence_context;

failed_to_allocate_channel_event:
    nv_drm_free(nv_prime_fence_context);

failed_alloc_fence_context:

    nvKms->unmapMemory(nv_dev->pDevice,
                       pSemSurface,
                       NVKMS_KAPI_MAPPING_TYPE_KERNEL,
                       (void *) pLinearAddress);

failed_to_map_memory:
    nvKms->freeMemory(nv_dev->pDevice, pSemSurface);

failed:
    return NULL;
}

static nv_dma_fence_t *__nv_drm_prime_fence_context_create_fence(
    struct nv_drm_prime_fence_context *nv_prime_fence_context,
    unsigned int seqno)
{
    struct nv_drm_prime_fence *nv_fence;
    int ret = 0;

    if ((nv_fence = nv_drm_calloc(1, sizeof(*nv_fence))) == NULL) {
        ret = -ENOMEM;
        goto out;
    }

    spin_lock(&nv_prime_fence_context->lock);

    /*
     * If seqno wrapped, force signal fences to make sure none of them
     * get stuck.
     */
    if (seqno < nv_prime_fence_context->last_seqno) {
        nv_drm_gem_prime_force_fence_signal(nv_prime_fence_context);
    }

    INIT_LIST_HEAD(&nv_fence->list_entry);

    spin_lock_init(&nv_fence->lock);

    nv_dma_fence_init(&nv_fence->base, &nv_drm_gem_prime_fence_ops,
                      &nv_fence->lock, nv_prime_fence_context->base.context,
                      seqno);

    /* The context maintains a reference to any pending fences. */
    nv_dma_fence_get(&nv_fence->base);

    list_add_tail(&nv_fence->list_entry, &nv_prime_fence_context->pending);

    nv_prime_fence_context->last_seqno = seqno;

    spin_unlock(&nv_prime_fence_context->lock);

out:
    return ret != 0 ? ERR_PTR(ret) : &nv_fence->base;
}

int nv_drm_fence_supported_ioctl(struct drm_device *dev,
                                 void *data, struct drm_file *filep)
{
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    return nv_dev->pDevice ? 0 : -EINVAL;
}

static inline struct nv_drm_fence_context *to_nv_fence_context(
    struct nv_drm_gem_object *nv_gem)
{
    if (nv_gem != NULL) {
        return container_of(nv_gem, struct nv_drm_fence_context, base);
    }

    return NULL;
}

/*
 * Tear down of the 'struct nv_drm_fence_context' object is not expected
 * to be happen from any worker thread, if that happen it causes dead-lock
 * because tear down sequence calls to flush all existing
 * worker thread.
 */
static void
__nv_drm_fence_context_gem_free(struct nv_drm_gem_object *nv_gem)
{
    struct nv_drm_fence_context *nv_fence_context = to_nv_fence_context(nv_gem);

    nv_fence_context->ops->destroy(nv_fence_context);
}

const struct nv_drm_gem_object_funcs nv_fence_context_gem_ops = {
    .free = __nv_drm_fence_context_gem_free,
};

static inline
struct nv_drm_fence_context *
__nv_drm_fence_context_lookup(
    struct drm_device *dev,
    struct drm_file *filp,
    u32 handle)
{
    struct nv_drm_gem_object *nv_gem =
            nv_drm_gem_object_lookup(dev, filp, handle);

    if (nv_gem != NULL && nv_gem->ops != &nv_fence_context_gem_ops) {
        nv_drm_gem_object_unreference_unlocked(nv_gem);
        return NULL;
    }

    return to_nv_fence_context(nv_gem);
}

static int
__nv_drm_fence_context_gem_init(struct drm_device *dev,
                                struct nv_drm_fence_context *nv_fence_context,
                                u32 *handle,
                                struct drm_file *filep)
{
    struct nv_drm_device *nv_dev = to_nv_device(dev);

    nv_drm_gem_object_init(nv_dev,
                           &nv_fence_context->base,
                           &nv_fence_context_gem_ops,
                           0 /* size */,
                           NULL /* pMemory */);

    return nv_drm_gem_handle_create_drop_reference(filep,
                                                   &nv_fence_context->base,
                                                   handle);
}

int nv_drm_prime_fence_context_create_ioctl(struct drm_device *dev,
                                            void *data, struct drm_file *filep)
{
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    struct drm_nvidia_prime_fence_context_create_params *p = data;
    struct nv_drm_prime_fence_context *nv_prime_fence_context;
    int err;

    if (nv_dev->pDevice == NULL) {
        return -EOPNOTSUPP;
    }

    nv_prime_fence_context = __nv_drm_prime_fence_context_new(nv_dev, p);

    if (!nv_prime_fence_context) {
        goto done;
    }

    err = __nv_drm_fence_context_gem_init(dev,
                                          &nv_prime_fence_context->base,
                                          &p->handle,
                                          filep);
    if (err) {
        __nv_drm_prime_fence_context_destroy(&nv_prime_fence_context->base);
    }

    return err;

done:
    return -ENOMEM;
}

static int __nv_drm_gem_attach_fence(struct nv_drm_gem_object *nv_gem,
                                     nv_dma_fence_t *fence,
                                     bool shared)
{
    nv_dma_resv_t *resv = nv_drm_gem_res_obj(nv_gem);
    int ret;

    nv_dma_resv_lock(resv, NULL);

    ret = nv_dma_resv_reserve_fences(resv, 1, shared);
    if (ret == 0) {
        if (shared) {
            nv_dma_resv_add_shared_fence(resv, fence);
        } else {
            nv_dma_resv_add_excl_fence(resv, fence);
        }
    } else {
        NV_DRM_LOG_ERR("Failed to reserve fence. Error code: %d", ret);
    }

    nv_dma_resv_unlock(resv);

    return ret;
}

int nv_drm_gem_prime_fence_attach_ioctl(struct drm_device *dev,
                                        void *data, struct drm_file *filep)
{
    int ret = -EINVAL;
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    struct drm_nvidia_gem_prime_fence_attach_params *p = data;

    struct nv_drm_gem_object *nv_gem;
    struct nv_drm_fence_context *nv_fence_context;
    nv_dma_fence_t *fence;

    if (nv_dev->pDevice == NULL) {
        ret = -EOPNOTSUPP;
        goto done;
    }

    if (p->__pad != 0) {
        NV_DRM_DEV_LOG_ERR(nv_dev, "Padding fields must be zeroed");
        goto done;
    }

    nv_gem = nv_drm_gem_object_lookup(nv_dev->dev, filep, p->handle);

    if (!nv_gem) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to lookup gem object for fence attach: 0x%08x",
            p->handle);

        goto done;
    }

    if((nv_fence_context = __nv_drm_fence_context_lookup(
                nv_dev->dev,
                filep,
                p->fence_context_handle)) == NULL) {

        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to lookup gem object for fence context: 0x%08x",
            p->fence_context_handle);

        goto fence_context_lookup_failed;
    }

    if (nv_fence_context->ops !=
        &nv_drm_prime_fence_context_ops) {

        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Wrong fence context type: 0x%08x",
            p->fence_context_handle);

        goto fence_context_create_fence_failed;
    }

    fence = __nv_drm_prime_fence_context_create_fence(
                to_nv_prime_fence_context(nv_fence_context),
                p->sem_thresh);

    if (IS_ERR(fence)) {
        ret = PTR_ERR(fence);

        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to allocate fence: 0x%08x", p->handle);

        goto fence_context_create_fence_failed;
    }

    ret = __nv_drm_gem_attach_fence(nv_gem, fence, true /* exclusive */);

    nv_dma_fence_put(fence);

fence_context_create_fence_failed:
    nv_drm_gem_object_unreference_unlocked(&nv_fence_context->base);

fence_context_lookup_failed:
    nv_drm_gem_object_unreference_unlocked(nv_gem);

done:
    return ret;
}

struct nv_drm_semsurf_fence {
    nv_dma_fence_t base;
    spinlock_t lock;

    /*
     * When unsignaled, node in the associated fence context's pending fence
     * list. The list holds a reference to the fence
     */
    struct list_head pending_node;

#if !defined(NV_DMA_FENCE_OPS_HAS_USE_64BIT_SEQNO)
    /* 64-bit version of base.seqno on kernels with 32-bit fence seqno */
    NvU64 wait_value;
#endif

    /*
     * Raw absolute kernel time (time domain and scale are treated as opaque)
     * when this fence times out.
     */
    unsigned long timeout;
};

struct nv_drm_semsurf_fence_callback {
    struct nv_drm_semsurf_fence_ctx *ctx;
    nv_drm_work work;
    NvU64 wait_value;
};

struct nv_drm_sync_fd_wait_data {
    nv_dma_fence_cb_t dma_fence_cb;
    struct nv_drm_semsurf_fence_ctx *ctx;
    nv_drm_work work; /* Deferred second half of fence wait callback */

    /* Could use a lockless list data structure here instead */
    struct list_head pending_node;

    NvU64 pre_wait_value;
    NvU64 post_wait_value;
};

struct nv_drm_semsurf_fence_ctx {
    struct nv_drm_fence_context base;

    /* The NVKMS KAPI reference to the context's semaphore surface */
    struct NvKmsKapiSemaphoreSurface *pSemSurface;

    /* CPU mapping of the semaphore slot values */
    union {
        volatile void *pVoid;
        volatile NvU32 *p32;
        volatile NvU64 *p64;
    } pSemMapping;
    volatile NvU64 *pMaxSubmittedMapping;

    /* work thread for fence timeouts and waits */
    nv_drm_workthread worker;

    /* Timeout timer and associated workthread work */
    nv_drm_timer timer;
    nv_drm_work timeout_work;

    /* Protects access to everything below */
    spinlock_t lock;

    /* List of pending fences which are not yet signaled */
    struct list_head pending_fences;

    /* List of pending fence wait operations */
    struct list_head pending_waits;

    /*
     * Tracking data for the single in-flight callback associated with this
     * context. Either both pointers will be valid, or both will be NULL.
     *
     * Note it is not safe to dereference these values outside of the context
     * lock unless it is certain the associated callback is not yet active,
     * or has been canceled. Their memory is owned by the callback itself as
     * soon as it is registered. Subtly, this means these variables can not
     * be used as output parameters to the function that registers the callback.
     */
    struct {
        struct nv_drm_semsurf_fence_callback *local;
        struct NvKmsKapiSemaphoreSurfaceCallback *nvKms;
    } callback;

    /*
     * Wait value associated with either the above or a being-registered
     * callback. May differ from callback->local->wait_value if it is the
     * latter. Zero if no callback is currently needed.
     */
    NvU64 current_wait_value;
};

static inline struct nv_drm_semsurf_fence_ctx*
to_semsurf_fence_ctx(
    struct nv_drm_fence_context *nv_fence_context
)
{
    return container_of(nv_fence_context,
                        struct nv_drm_semsurf_fence_ctx,
                        base);
}

static inline NvU64
__nv_drm_get_semsurf_fence_seqno(const struct nv_drm_semsurf_fence *nv_fence)
{
#if defined(NV_DMA_FENCE_OPS_HAS_USE_64BIT_SEQNO)
    return nv_fence->base.seqno;
#else
    return nv_fence->wait_value;
#endif
}

#ifndef READ_ONCE
#define READ_ONCE(x) ACCESS_ONCE(x)
#endif

static inline NvU64
__nv_drm_get_semsurf_ctx_seqno(struct nv_drm_semsurf_fence_ctx *ctx)
{
    NvU64 semVal;

    if (ctx->pMaxSubmittedMapping) {
        /* 32-bit GPU semaphores */
        NvU64 maxSubmitted = READ_ONCE(*ctx->pMaxSubmittedMapping);

        /*
         * Must happen after the max submitted read! See
         * NvTimeSemFermiGetPayload() for full details.
         */
        semVal = READ_ONCE(*ctx->pSemMapping.p32);

        if ((maxSubmitted & 0xFFFFFFFFull) < semVal) {
            maxSubmitted -= 0x100000000ull;
        }

        semVal |= (maxSubmitted & 0xffffffff00000000ull);
    } else {
        /* 64-bit GPU semaphores */
        semVal = READ_ONCE(*ctx->pSemMapping.p64);
    }

    return semVal;
}

static void
__nv_drm_semsurf_force_complete_pending(struct nv_drm_semsurf_fence_ctx *ctx)
{
    unsigned long flags;

    /*
     * No locks are needed for the pending_fences list. This code runs after all
     * other possible references to the fence context have been removed. The
     * fences have their own individual locks to protect themselves.
     */
    while (!list_empty(&ctx->pending_fences)) {
        struct nv_drm_semsurf_fence *nv_fence = list_first_entry(
            &ctx->pending_fences,
            typeof(*nv_fence),
            pending_node);
        nv_dma_fence_t *fence = &nv_fence->base;

        list_del(&nv_fence->pending_node);

        nv_dma_fence_set_error(fence, -ETIMEDOUT);
        nv_dma_fence_signal(fence);

        /* Remove the pending list's reference */
        nv_dma_fence_put(fence);
    }

    /*
     * The pending waits are also referenced by the fences they are waiting on,
     * but those fences are guaranteed to complete in finite time. Just keep the
     * the context alive until they do so.
     */
    spin_lock_irqsave(&ctx->lock, flags);
    while (!list_empty(&ctx->pending_waits)) {
        spin_unlock_irqrestore(&ctx->lock, flags);
        nv_drm_yield();
        spin_lock_irqsave(&ctx->lock, flags);
    }
    spin_unlock_irqrestore(&ctx->lock, flags);
}

/* Forward declaration */
static void
__nv_drm_semsurf_ctx_reg_callbacks(struct nv_drm_semsurf_fence_ctx *ctx);

static void
__nv_drm_semsurf_ctx_fence_callback_work(void *data)
{
    struct nv_drm_semsurf_fence_callback *callback = data;

    __nv_drm_semsurf_ctx_reg_callbacks(callback->ctx);

    nv_drm_free(callback);
}

static struct nv_drm_semsurf_fence_callback*
__nv_drm_semsurf_new_callback(struct nv_drm_semsurf_fence_ctx *ctx)
{
    struct nv_drm_semsurf_fence_callback *newCallback =
        nv_drm_calloc(1, sizeof(*newCallback));

    if (!newCallback) {
        return NULL;
    }

    newCallback->ctx = ctx;
    nv_drm_workthread_work_init(&newCallback->work,
                                __nv_drm_semsurf_ctx_fence_callback_work,
                                newCallback);

    return newCallback;
}

static void
__nv_drm_semsurf_ctx_process_completed(struct nv_drm_semsurf_fence_ctx *ctx,
                                       NvU64 *newWaitValueOut,
                                       unsigned long *newTimeoutOut)
{
    struct list_head finished;
    struct list_head timed_out;
    struct nv_drm_semsurf_fence *nv_fence;
    nv_dma_fence_t *fence;
    NvU64 currentSeqno = __nv_drm_get_semsurf_ctx_seqno(ctx);
    NvU64 fenceSeqno = 0;
    unsigned long flags;
    unsigned long fenceTimeout = 0;
    unsigned long now = nv_drm_timer_now();

    INIT_LIST_HEAD(&finished);
    INIT_LIST_HEAD(&timed_out);

    spin_lock_irqsave(&ctx->lock, flags);

    while (!list_empty(&ctx->pending_fences)) {
        nv_fence = list_first_entry(&ctx->pending_fences,
                                    typeof(*nv_fence),
                                    pending_node);

        fenceSeqno = __nv_drm_get_semsurf_fence_seqno(nv_fence);
        fenceTimeout = nv_fence->timeout;

        if (fenceSeqno <= currentSeqno) {
            list_move_tail(&nv_fence->pending_node, &finished);
        } else if (fenceTimeout <= now) {
            list_move_tail(&nv_fence->pending_node, &timed_out);
        } else {
            break;
        }
    }

    /*
     * If the caller passes non-NULL newWaitValueOut and newTimeoutOut
     * parameters, it establishes a contract. If the returned values are
     * non-zero, the caller must attempt to register a callback associated with
     * the new wait value and reset the context's timer to the specified
     * timeout.
     */
    if (newWaitValueOut && newTimeoutOut) {
        if (list_empty(&ctx->pending_fences)) {
            /* No pending fences, so no waiter is needed. */
            ctx->current_wait_value = fenceSeqno = 0;
            fenceTimeout = 0;
        } else if (fenceSeqno == ctx->current_wait_value) {
            /*
             * The context already has a waiter registered, or in the process of
             * being registered, for this fence. Indicate to the caller no new
             * waiter registration is needed, and leave the ctx state alone.
             */
            fenceSeqno = 0;
            fenceTimeout = 0;
        } else {
            /* A new waiter must be registered. Prep the context */
            ctx->current_wait_value = fenceSeqno;
        }

        *newWaitValueOut = fenceSeqno;
        *newTimeoutOut = fenceTimeout;
    }

    spin_unlock_irqrestore(&ctx->lock, flags);

    while (!list_empty(&finished)) {
        nv_fence = list_first_entry(&finished, typeof(*nv_fence), pending_node);
        list_del_init(&nv_fence->pending_node);
        fence = &nv_fence->base;
        nv_dma_fence_signal(fence);
        nv_dma_fence_put(fence); /* Drops the pending list's reference */
    }

    while (!list_empty(&timed_out)) {
        nv_fence = list_first_entry(&timed_out, typeof(*nv_fence),
                                    pending_node);
        list_del_init(&nv_fence->pending_node);
        fence = &nv_fence->base;
        nv_dma_fence_set_error(fence, -ETIMEDOUT);
        nv_dma_fence_signal(fence);
        nv_dma_fence_put(fence); /* Drops the pending list's reference */
    }
}

static void
__nv_drm_semsurf_ctx_callback(void *data)
{
    struct nv_drm_semsurf_fence_callback *callback = data;
    struct nv_drm_semsurf_fence_ctx *ctx = callback->ctx;
    unsigned long flags;

    spin_lock_irqsave(&ctx->lock, flags);
    /* If this was the context's currently registered callback, clear it. */
    if (ctx->callback.local == callback) {
        ctx->callback.local = NULL;
        ctx->callback.nvKms = NULL;
    }
    /* If storing of this callback may have been pending, prevent it. */
    if (ctx->current_wait_value == callback->wait_value) {
        ctx->current_wait_value = 0;
    }
    spin_unlock_irqrestore(&ctx->lock, flags);

    /*
     * This is redundant with the __nv_drm_semsurf_ctx_reg_callbacks() call from
     * __nv_drm_semsurf_ctx_fence_callback_work(), which will be called by the
     * work enqueued below, but calling it here as well allows unblocking
     * waiters with less latency.
     */
    __nv_drm_semsurf_ctx_process_completed(ctx, NULL, NULL);

    if (!nv_drm_workthread_add_work(&ctx->worker, &callback->work)) {
        /*
         * The context is shutting down. It will force-signal all fences when
         * doing so, so there's no need for any more callback handling.
         */
        nv_drm_free(callback);
    }
}

/*
 * Take spin lock, attempt to stash newNvKmsCallback/newCallback in ctx.
 * If current_wait_value in fence context != new_wait_value, we raced with
 * someone registering a newer waiter. Release spin lock, and unregister our
 * waiter. It isn't needed anymore.
 */
static bool
__nv_drm_semsurf_ctx_store_callback(
    struct nv_drm_semsurf_fence_ctx *ctx,
    NvU64 new_wait_value,
    struct NvKmsKapiSemaphoreSurfaceCallback *newNvKmsCallback,
    struct nv_drm_semsurf_fence_callback *newCallback)
{
    struct nv_drm_device *nv_dev = ctx->base.nv_dev;
    struct NvKmsKapiSemaphoreSurfaceCallback *oldNvKmsCallback;
    struct nv_drm_semsurf_fence_callback *oldCallback = NULL;
    NvU64 oldWaitValue;
    unsigned long flags;
    bool installed = false;

    spin_lock_irqsave(&ctx->lock, flags);
    if (ctx->current_wait_value == new_wait_value) {
        oldCallback = ctx->callback.local;
        oldNvKmsCallback = ctx->callback.nvKms;
        oldWaitValue = oldCallback ? oldCallback->wait_value : 0;
        ctx->callback.local = newCallback;
        ctx->callback.nvKms = newNvKmsCallback;
        installed = true;
    }
    spin_unlock_irqrestore(&ctx->lock, flags);

    if (oldCallback) {
        if (nvKms->unregisterSemaphoreSurfaceCallback(nv_dev->pDevice,
                                                      ctx->pSemSurface,
                                                      ctx->base.fenceSemIndex,
                                                      oldWaitValue,
                                                      oldNvKmsCallback)) {
            /*
             * The old callback was successfully canceled, and its NVKMS and RM
             * resources have been freed. Free its local tracking data.
             */
            nv_drm_free(oldCallback);
        } else {
            /*
             * The new callback is already running. It will do no harm, and free
             * itself.
             */
        }
    }

    return installed;
}

/*
 * Processes completed fences and registers an RM callback and a timeout timer
 * for the next incomplete fence, if any. To avoid calling in to RM while
 * holding a spinlock, this is done in a loop until the state settles.
 *
 * Can NOT be called from in an atomic context/interrupt handler.
 */
static void
__nv_drm_semsurf_ctx_reg_callbacks(struct nv_drm_semsurf_fence_ctx *ctx)

{
    struct nv_drm_device *nv_dev = ctx->base.nv_dev;
    struct nv_drm_semsurf_fence_callback *newCallback =
        __nv_drm_semsurf_new_callback(ctx);
    struct NvKmsKapiSemaphoreSurfaceCallback *newNvKmsCallback;
    NvU64 newWaitValue;
    unsigned long newTimeout;
    NvKmsKapiRegisterWaiterResult kapiRet;

    if (!newCallback) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to allocate new fence signal callback data");
        return;
    }

    do {
        /*
         * Process any completed or timed out fences. This returns the wait
         * value and timeout of the first remaining pending fence, or 0/0
         * if no pending fences remain. It will also tag the context as
         * waiting for the value returned.
         */
        __nv_drm_semsurf_ctx_process_completed(ctx,
                                               &newWaitValue,
                                               &newTimeout);

        if (newWaitValue == 0) {
            /* No fences remain, so no callback is needed. */
            nv_drm_free(newCallback);
            newCallback = NULL;
            return;
        }

        newCallback->wait_value = newWaitValue;

        /*
         * Attempt to register a callback for the remaining fences. Note this
         * code may be running concurrently in multiple places, attempting to
         * register a callback for the same value, a value greater than
         * newWaitValue if more fences have since completed, or a value less
         * than newWaitValue if new fences have been created tracking lower
         * values than the previously lowest pending one. Hence, even if this
         * registration succeeds, the callback may be discarded
         */
        kapiRet =
            nvKms->registerSemaphoreSurfaceCallback(nv_dev->pDevice,
                                                    ctx->pSemSurface,
                                                    __nv_drm_semsurf_ctx_callback,
                                                    newCallback,
                                                    ctx->base.fenceSemIndex,
                                                    newWaitValue,
                                                    0,
                                                    &newNvKmsCallback);
    } while (kapiRet == NVKMS_KAPI_REG_WAITER_ALREADY_SIGNALLED);

    /* Can't deref newCallback at this point unless kapiRet indicates failure */

    if (kapiRet != NVKMS_KAPI_REG_WAITER_SUCCESS) {
        /*
         * This is expected if another thread concurrently registered a callback
         * for the same value, which is fine. That thread's callback will do the
         * same work this thread's would have. Clean this one up and return.
         *
         * Another possibility is that an allocation or some other low-level
         * operation that can spuriously fail has caused this failure, or of
         * course a bug resulting in invalid usage of the
         * registerSemaphoreSurfaceCallback() API. There is no good way to
         * handle such failures, so the fence timeout will be relied upon to
         * guarantee forward progress in those cases.
         */
        nv_drm_free(newCallback);
        return;
    }

    nv_drm_mod_timer(&ctx->timer, newTimeout);

    if (!__nv_drm_semsurf_ctx_store_callback(ctx,
                                             newWaitValue,
                                             newNvKmsCallback,
                                             newCallback)) {
        /*
         * Another thread registered a callback for a different value before
         * this thread's callback could be stored in the context, or the
         * callback is already running. That's OK. One of the following is true:
         *
         * -A new fence with a lower value has been registered, and the callback
         *  associated with that fence is now active and associated with the
         *  context.
         *
         * -This fence has already completed, and a new callback associated with
         *  a higher value has been registered and associated with the context.
         *  This lower-value callback is no longer needed, as any fences
         *  associated with it must have been marked completed before
         *  registering the higher-value callback.
         *
         * -The callback started running and cleared ctx->current_wait_value
         *  before the callback could be stored in the context. Work to signal
         *  the fence is now pending.
         *
         * Hence, it is safe to request cancellation of the callback and free
         * the associated data if cancellation succeeds.
         */
        if (nvKms->unregisterSemaphoreSurfaceCallback(nv_dev->pDevice,
                                                      ctx->pSemSurface,
                                                      ctx->base.fenceSemIndex,
                                                      newWaitValue,
                                                      newNvKmsCallback)) {
            /* RM callback successfully canceled. Free local tracking data */
            nv_drm_free(newCallback);
        }
    }
}

static void __nv_drm_semsurf_fence_ctx_destroy(
    struct nv_drm_fence_context *nv_fence_context)
{
    struct nv_drm_device *nv_dev = nv_fence_context->nv_dev;
    struct nv_drm_semsurf_fence_ctx *ctx =
        to_semsurf_fence_ctx(nv_fence_context);
    struct NvKmsKapiSemaphoreSurfaceCallback *pendingNvKmsCallback;
    NvU64 pendingWaitValue;
    unsigned long flags;

    /*
     * The workthread must be shut down before the timer is stopped to ensure
     * the timer does not queue work that restarts itself.
     */
    nv_drm_workthread_shutdown(&ctx->worker);

    nv_timer_delete_sync(&ctx->timer.kernel_timer);

    /*
     * The semaphore surface could still be sending callbacks, so it is still
     * not safe to dereference the ctx->callback pointers. However,
     * unregistering a callback via its handle is safe, as that code in NVKMS
     * takes care to avoid dereferencing the handle until it knows the callback
     * has been canceled in RM. This unregistration must be done to ensure the
     * callback data is not leaked in NVKMS if it is still pending, as freeing
     * the semaphore surface only cleans up RM's callback data.
     */
    spin_lock_irqsave(&ctx->lock, flags);
    pendingNvKmsCallback = ctx->callback.nvKms;
    pendingWaitValue = ctx->callback.local ?
        ctx->callback.local->wait_value : 0;
    spin_unlock_irqrestore(&ctx->lock, flags);

    if (pendingNvKmsCallback) {
        WARN_ON(pendingWaitValue == 0);
        nvKms->unregisterSemaphoreSurfaceCallback(nv_dev->pDevice,
                                                  ctx->pSemSurface,
                                                  ctx->base.fenceSemIndex,
                                                  pendingWaitValue,
                                                  pendingNvKmsCallback);
    }

    nvKms->freeSemaphoreSurface(nv_dev->pDevice, ctx->pSemSurface);

    /*
     * Now that the semaphore surface, the timer, and the workthread are gone:
     *
     * -No more RM/NVKMS callbacks will arrive, nor are any in progress. Freeing
     *  the semaphore surface cancels all its callbacks associated with this
     *  instance of it, and idles any pending callbacks.
     *
     * -No more timer callbacks will arrive, nor are any in flight.
     *
     * -The workthread has been idled and is no longer running.
     *
     * Further, given the destructor is running, no other references to the
     * fence context exist, so this code can assume no concurrent access to the
     * fence context's data will happen from here on out.
     */

    if (ctx->callback.local) {
        nv_drm_free(ctx->callback.local);
        ctx->callback.local = NULL;
        ctx->callback.nvKms = NULL;
    }

    __nv_drm_semsurf_force_complete_pending(ctx);

    nv_drm_free(nv_fence_context);
}

static void
__nv_drm_semsurf_ctx_timeout_work(void *data)
{
    struct nv_drm_semsurf_fence_ctx *ctx = data;

    __nv_drm_semsurf_ctx_reg_callbacks(ctx);
}

static void
__nv_drm_semsurf_ctx_timeout_callback(nv_drm_timer *timer)
{
    struct nv_drm_semsurf_fence_ctx *ctx =
        container_of(timer, typeof(*ctx), timer);

    /*
     * Schedule work to register new waiter & timer on a worker thread.
     *
     * It does not matter if this fails. There are two possible failure cases:
     *
     * - ctx->timeout_work is already scheduled. That existing scheduled work
     *   will do at least as much as work scheduled right now and executed
     *   immediately, which is sufficient.
     *
     * - The context is shutting down. In this case, all fences will be force-
     *   signalled, so no further callbacks or timeouts are needed.
     *
     * Note this work may schedule a new timeout timer. To ensure that doesn't
     * happen while context shutdown is shutting down and idling the timer, the
     * the worker thread must be shut down before the timer is stopped.
     */
    nv_drm_workthread_add_work(&ctx->worker, &ctx->timeout_work);
}

static struct nv_drm_fence_context_ops
nv_drm_semsurf_fence_ctx_ops = {
    .destroy = __nv_drm_semsurf_fence_ctx_destroy,
};

static struct nv_drm_semsurf_fence_ctx*
__nv_drm_semsurf_fence_ctx_new(
    struct nv_drm_device *nv_dev,
    struct drm_nvidia_semsurf_fence_ctx_create_params *p
)
{
    struct nv_drm_semsurf_fence_ctx *ctx;
    struct NvKmsKapiSemaphoreSurface *pSemSurface;
    uint8_t *semMapping;
    uint8_t *maxSubmittedMapping;
    char worker_name[20+16+1]; /* strlen(nvidia-drm/timeline-) + 16 for %llx + NUL */

    pSemSurface = nvKms->importSemaphoreSurface(nv_dev->pDevice,
                                                p->nvkms_params_ptr,
                                                p->nvkms_params_size,
                                                (void **)&semMapping,
                                                (void **)&maxSubmittedMapping);
    if (!pSemSurface) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to import semaphore surface");

        goto failed;
    }

    /*
     * Allocate a fence context object and initialize it.
     */

    if ((ctx = nv_drm_calloc(1, sizeof(*ctx))) == NULL) {
        goto failed_alloc_fence_context;
    }

    semMapping += (p->index * nv_dev->semsurf_stride);
    if (maxSubmittedMapping) {
        maxSubmittedMapping += (p->index * nv_dev->semsurf_stride) +
            nv_dev->semsurf_max_submitted_offset;
    }

    /*
     * nv_dma_fence_context_alloc() cannot fail, so we do not need
     * to check a return value.
     */

    ctx->base.ops = &nv_drm_semsurf_fence_ctx_ops;
    ctx->base.nv_dev = nv_dev;
    ctx->base.context = nv_dma_fence_context_alloc(1);
    ctx->base.fenceSemIndex = p->index;
    ctx->pSemSurface = pSemSurface;
    ctx->pSemMapping.pVoid = semMapping;
    ctx->pMaxSubmittedMapping = (volatile NvU64 *)maxSubmittedMapping;
    ctx->callback.local = NULL;
    ctx->callback.nvKms = NULL;
    ctx->current_wait_value = 0;

    spin_lock_init(&ctx->lock);
    INIT_LIST_HEAD(&ctx->pending_fences);
    INIT_LIST_HEAD(&ctx->pending_waits);

    sprintf(worker_name, "nvidia-drm/timeline-%llx",
            (long long unsigned)ctx->base.context);
    if (!nv_drm_workthread_init(&ctx->worker, worker_name)) {
        goto failed_alloc_worker;
    }

    nv_drm_workthread_work_init(&ctx->timeout_work,
                                __nv_drm_semsurf_ctx_timeout_work,
                                ctx);

    nv_drm_timer_setup(&ctx->timer, __nv_drm_semsurf_ctx_timeout_callback);

    return ctx;

failed_alloc_worker:
    nv_drm_free(ctx);

failed_alloc_fence_context:
    nvKms->freeSemaphoreSurface(nv_dev->pDevice, pSemSurface);

failed:
    return NULL;

}

int nv_drm_semsurf_fence_ctx_create_ioctl(struct drm_device *dev,
                                          void *data,
                                          struct drm_file *filep)
{
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    struct drm_nvidia_semsurf_fence_ctx_create_params *p = data;
    struct nv_drm_semsurf_fence_ctx *ctx;
    int err;

    if (nv_dev->pDevice == NULL) {
        return -EOPNOTSUPP;
    }

    if (p->__pad != 0) {
        NV_DRM_DEV_LOG_ERR(nv_dev, "Padding fields must be zeroed");
        return -EINVAL;
    }

    ctx = __nv_drm_semsurf_fence_ctx_new(nv_dev, p);

    if (!ctx) {
        return -ENOMEM;
    }

    err = __nv_drm_fence_context_gem_init(dev, &ctx->base, &p->handle, filep);

    if (err) {
        __nv_drm_semsurf_fence_ctx_destroy(&ctx->base);
    }

    return err;
}

static inline struct nv_drm_semsurf_fence*
to_nv_drm_semsurf_fence(nv_dma_fence_t *fence)
{
    return container_of(fence, struct nv_drm_semsurf_fence, base);
}

static const char*
__nv_drm_semsurf_fence_op_get_timeline_name(nv_dma_fence_t *fence)
{
    return "nvidia.semaphore_surface";
}

static bool
__nv_drm_semsurf_fence_op_enable_signaling(nv_dma_fence_t *fence)
{
    // DO NOTHING - Could defer RM callback registration until this point
    return true;
}

static void
__nv_drm_semsurf_fence_op_release(nv_dma_fence_t *fence)
{
    struct nv_drm_semsurf_fence *nv_fence =
        to_nv_drm_semsurf_fence(fence);

    nv_drm_free(nv_fence);
}

static const nv_dma_fence_ops_t nv_drm_semsurf_fence_ops = {
    .get_driver_name = nv_drm_gem_fence_op_get_driver_name,
    .get_timeline_name = __nv_drm_semsurf_fence_op_get_timeline_name,
    .enable_signaling = __nv_drm_semsurf_fence_op_enable_signaling,
    .release = __nv_drm_semsurf_fence_op_release,
    .wait = nv_dma_fence_default_wait,
#if defined(NV_DMA_FENCE_OPS_HAS_USE_64BIT_SEQNO)
    .use_64bit_seqno = true,
#endif
};

/*
 * Completes fence initialization, places a new reference to the fence in the
 * context's pending fence list, and updates/registers any RM callbacks and
 * timeout timers if necessary.
 *
 * Can NOT be called from in an atomic context/interrupt handler.
 */
static void
__nv_drm_semsurf_ctx_add_pending(struct nv_drm_semsurf_fence_ctx *ctx,
                                 struct nv_drm_semsurf_fence *nv_fence,
                                 NvU64 timeoutMS)
{
    struct list_head *pending;
    unsigned long flags;

    if (timeoutMS > NV_DRM_SEMAPHORE_SURFACE_FENCE_MAX_TIMEOUT_MS) {
        timeoutMS = NV_DRM_SEMAPHORE_SURFACE_FENCE_MAX_TIMEOUT_MS;
    }

    /* Add a reference to the fence for the list */
    nv_dma_fence_get(&nv_fence->base);
    INIT_LIST_HEAD(&nv_fence->pending_node);

    nv_fence->timeout = nv_drm_timeout_from_ms(timeoutMS);

    spin_lock_irqsave(&ctx->lock, flags);

    list_for_each(pending, &ctx->pending_fences) {
        struct nv_drm_semsurf_fence *pending_fence =
            list_entry(pending, typeof(*pending_fence), pending_node);
        if (__nv_drm_get_semsurf_fence_seqno(pending_fence) >
            __nv_drm_get_semsurf_fence_seqno(nv_fence)) {
            /* Inserts 'nv_fence->pending_node' before 'pending' */
            list_add_tail(&nv_fence->pending_node, pending);
            break;
        }
    }

    if (list_empty(&nv_fence->pending_node)) {
        /*
         * Inserts 'fence->pending_node' at the end of 'ctx->pending_fences',
         * or as the head if the list is empty
         */
        list_add_tail(&nv_fence->pending_node, &ctx->pending_fences);
    }

    /* Fence is live starting... now! */
    spin_unlock_irqrestore(&ctx->lock, flags);

    /* Register new wait and timeout callbacks, if necessary */
    __nv_drm_semsurf_ctx_reg_callbacks(ctx);
}

static nv_dma_fence_t *__nv_drm_semsurf_fence_ctx_create_fence(
    struct nv_drm_device *nv_dev,
    struct nv_drm_semsurf_fence_ctx *ctx,
    NvU64 wait_value,
    NvU64 timeout_value_ms)
{
    struct nv_drm_semsurf_fence *nv_fence;
    nv_dma_fence_t *fence;
    int ret = 0;

    if (timeout_value_ms == 0 ||
        timeout_value_ms > NV_DRM_SEMAPHORE_SURFACE_FENCE_MAX_TIMEOUT_MS) {
        timeout_value_ms = NV_DRM_SEMAPHORE_SURFACE_FENCE_MAX_TIMEOUT_MS;
    }

    if ((nv_fence = nv_drm_calloc(1, sizeof(*nv_fence))) == NULL) {
        ret = -ENOMEM;
        goto out;
    }

    fence = &nv_fence->base;
    spin_lock_init(&nv_fence->lock);
#if !defined(NV_DMA_FENCE_OPS_HAS_USE_64BIT_SEQNO)
    nv_fence->wait_value = wait_value;
#endif

    /* Initializes the fence with one reference (for the caller) */
    nv_dma_fence_init(fence, &nv_drm_semsurf_fence_ops,
                      &nv_fence->lock,
                      ctx->base.context, wait_value);

    __nv_drm_semsurf_ctx_add_pending(ctx, nv_fence, timeout_value_ms);

out:
    /* Returned fence has one reference reserved for the caller. */
    return ret != 0 ? ERR_PTR(ret) : &nv_fence->base;
}

int nv_drm_semsurf_fence_create_ioctl(struct drm_device *dev,
                                      void *data,
                                      struct drm_file *filep)
{
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    struct drm_nvidia_semsurf_fence_create_params *p = data;
    struct nv_drm_fence_context *nv_fence_context;
    nv_dma_fence_t *fence;
    int ret = -EINVAL;
    int fd;

    if (nv_dev->pDevice == NULL) {
        ret = -EOPNOTSUPP;
        goto done;
    }

    if (p->__pad != 0) {
        NV_DRM_DEV_LOG_ERR(nv_dev, "Padding fields must be zeroed");
        goto done;
    }

    if ((nv_fence_context = __nv_drm_fence_context_lookup(
                                nv_dev->dev,
                                filep,
                                p->fence_context_handle)) == NULL) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to lookup gem object for fence context: 0x%08x",
            p->fence_context_handle);

        goto done;
    }

    if (nv_fence_context->ops != &nv_drm_semsurf_fence_ctx_ops) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Wrong fence context type: 0x%08x",
            p->fence_context_handle);

        goto fence_context_create_fence_failed;
    }

    fence = __nv_drm_semsurf_fence_ctx_create_fence(
        nv_dev,
        to_semsurf_fence_ctx(nv_fence_context),
        p->wait_value,
        p->timeout_value_ms);

    if (IS_ERR(fence)) {
        ret = PTR_ERR(fence);

        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to allocate fence: 0x%08x", p->fence_context_handle);

        goto fence_context_create_fence_failed;
    }

    if ((fd = nv_drm_create_sync_file(fence)) < 0) {
        ret = fd;

        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to create sync file from fence on ctx 0x%08x",
            p->fence_context_handle);

        goto fence_context_create_sync_failed;
    }

    p->fd = fd;
    ret = 0;

fence_context_create_sync_failed:
    /*
     * Release this function's reference to the fence.  If successful, the sync
     * FD will still hold a reference, and the pending list (if the fence hasn't
     * already been signaled) will also retain a reference.
     */
    nv_dma_fence_put(fence);

fence_context_create_fence_failed:
    nv_drm_gem_object_unreference_unlocked(&nv_fence_context->base);

done:
    return ret;
}

static void
__nv_drm_semsurf_free_wait_data(struct nv_drm_sync_fd_wait_data *wait_data)
{
    struct nv_drm_semsurf_fence_ctx *ctx = wait_data->ctx;
    unsigned long flags;

    spin_lock_irqsave(&ctx->lock, flags);
    list_del(&wait_data->pending_node);
    spin_unlock_irqrestore(&ctx->lock, flags);

    nv_drm_free(wait_data);
}

static void
__nv_drm_semsurf_wait_fence_work_cb
(
    void *arg
)
{
    struct nv_drm_sync_fd_wait_data *wait_data = arg;
    struct nv_drm_semsurf_fence_ctx *ctx = wait_data->ctx;
    struct nv_drm_device *nv_dev = ctx->base.nv_dev;
    NvKmsKapiRegisterWaiterResult ret;

    /*
     * Note this command applies "newValue" immediately if the semaphore has
     * already reached "waitValue." It only returns NVKMS_KAPI_ALREADY_SIGNALLED
     * if a separate notification was requested as well.
     */
    ret = nvKms->registerSemaphoreSurfaceCallback(nv_dev->pDevice,
                                                  ctx->pSemSurface,
                                                  NULL,
                                                  NULL,
                                                  ctx->base.fenceSemIndex,
                                                  wait_data->pre_wait_value,
                                                  wait_data->post_wait_value,
                                                  NULL);

    if (ret != NVKMS_KAPI_REG_WAITER_SUCCESS) {
        NV_DRM_DEV_LOG_ERR(nv_dev,
                           "Failed to register auto-value-update on pre-wait value for sync FD semaphore surface");
    }

    __nv_drm_semsurf_free_wait_data(wait_data);
}

static void
__nv_drm_semsurf_wait_fence_cb
(
    nv_dma_fence_t *fence,
    nv_dma_fence_cb_t *cb
)
{
    struct nv_drm_sync_fd_wait_data *wait_data =
        container_of(cb, typeof(*wait_data), dma_fence_cb);
    struct nv_drm_semsurf_fence_ctx *ctx = wait_data->ctx;

    /*
     * Defer registering the wait with RM to a worker thread, since
     * this function may be called in interrupt context, which
     * could mean arriving here directly from RM's top/bottom half
     * handler when the fence being waited on came from an RM-managed GPU.
     */
    if (!nv_drm_workthread_add_work(&ctx->worker, &wait_data->work)) {
        /*
         * The context is shutting down. RM would likely just drop
         * the wait anyway as part of that, so do nothing. Either the
         * client is exiting uncleanly, or it is a bug in the client
         * in that it didn't consume its wait before destroying the
         * fence context used to instantiate it.
         */
        __nv_drm_semsurf_free_wait_data(wait_data);
    }

    /* Don't need to reference the fence anymore, just the fence context. */
    nv_dma_fence_put(fence);
}

int nv_drm_semsurf_fence_wait_ioctl(struct drm_device *dev,
                                    void *data,
                                    struct drm_file *filep)
{
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    struct drm_nvidia_semsurf_fence_wait_params *p = data;
    struct nv_drm_fence_context *nv_fence_context;
    struct nv_drm_semsurf_fence_ctx *ctx;
    struct nv_drm_sync_fd_wait_data *wait_data = NULL;
    nv_dma_fence_t *fence;
    unsigned long flags;
    int ret = -EINVAL;

    if (nv_dev->pDevice == NULL) {
        return -EOPNOTSUPP;
    }

    if (p->pre_wait_value >= p->post_wait_value) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Non-monotonic wait values specified to fence wait: 0x%" NvU64_fmtu ", 0x%" NvU64_fmtu,
            p->pre_wait_value, p->post_wait_value);
        goto done;
    }

    if ((nv_fence_context = __nv_drm_fence_context_lookup(
                                nv_dev->dev,
                                filep,
                                p->fence_context_handle)) == NULL) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to lookup gem object for fence context: 0x%08x",
            p->fence_context_handle);

        goto done;
    }

    if (nv_fence_context->ops != &nv_drm_semsurf_fence_ctx_ops) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Wrong fence context type: 0x%08x",
            p->fence_context_handle);

        goto fence_context_sync_lookup_failed;
    }

    ctx = to_semsurf_fence_ctx(nv_fence_context);

    wait_data = nv_drm_calloc(1, sizeof(*wait_data));

    if (!wait_data) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to allocate callback data for sync FD wait: %d", p->fd);

        goto fence_context_sync_lookup_failed;
    }

    fence = nv_drm_sync_file_get_fence(p->fd);

    if (!fence) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Attempt to wait on invalid sync FD: %d", p->fd);

        goto fence_context_sync_lookup_failed;
    }

    wait_data->ctx = ctx;
    wait_data->pre_wait_value = p->pre_wait_value;
    wait_data->post_wait_value = p->post_wait_value;
    nv_drm_workthread_work_init(&wait_data->work,
                                __nv_drm_semsurf_wait_fence_work_cb,
                                wait_data);

    spin_lock_irqsave(&ctx->lock, flags);
    list_add(&wait_data->pending_node, &ctx->pending_waits);
    spin_unlock_irqrestore(&ctx->lock, flags);

    ret = nv_dma_fence_add_callback(fence,
                                    &wait_data->dma_fence_cb,
                                    __nv_drm_semsurf_wait_fence_cb);

    if (ret) {
       if (ret == -ENOENT) {
           /* The fence is already signaled */
       } else {
           NV_DRM_LOG_ERR(
               "Failed to add dma_fence callback. Signaling early!");
           /* Proceed as if the fence wait succeeded */
       }

       /* Execute second half of wait immediately, avoiding the worker thread */
       nv_dma_fence_put(fence);
        __nv_drm_semsurf_wait_fence_work_cb(wait_data);
    }

    ret = 0;

fence_context_sync_lookup_failed:
    if (ret && wait_data) {
        /*
         * Do not use __nv_drm_semsurf_free_wait_data() here, as the wait_data
         * has not been added to the pending list yet.
         */
        nv_drm_free(wait_data);
    }

    nv_drm_gem_object_unreference_unlocked(&nv_fence_context->base);

done:
    return 0;
}

int nv_drm_semsurf_fence_attach_ioctl(struct drm_device *dev,
                                      void *data,
                                      struct drm_file *filep)
{
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    struct drm_nvidia_semsurf_fence_attach_params *p = data;
    struct nv_drm_gem_object *nv_gem = NULL;
    struct nv_drm_fence_context *nv_fence_context = NULL;
    nv_dma_fence_t *fence;
    int ret = -EINVAL;

    if (nv_dev->pDevice == NULL) {
        ret = -EOPNOTSUPP;
        goto done;
    }

    nv_gem = nv_drm_gem_object_lookup(nv_dev->dev, filep, p->handle);

    if (!nv_gem) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to lookup gem object for fence attach: 0x%08x",
            p->handle);

        goto done;
    }

    nv_fence_context = __nv_drm_fence_context_lookup(
        nv_dev->dev,
        filep,
        p->fence_context_handle);

    if (!nv_fence_context) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to lookup gem object for fence context: 0x%08x",
            p->fence_context_handle);

        goto done;
    }

    if (nv_fence_context->ops != &nv_drm_semsurf_fence_ctx_ops) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Wrong fence context type: 0x%08x",
            p->fence_context_handle);

        goto done;
    }

    fence = __nv_drm_semsurf_fence_ctx_create_fence(
        nv_dev,
        to_semsurf_fence_ctx(nv_fence_context),
        p->wait_value,
        p->timeout_value_ms);

    if (IS_ERR(fence)) {
        ret = PTR_ERR(fence);

        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to allocate fence: 0x%08x", p->handle);

        goto done;
    }

    ret = __nv_drm_gem_attach_fence(nv_gem, fence, p->shared);

    nv_dma_fence_put(fence);

done:
    if (nv_fence_context) {
        nv_drm_gem_object_unreference_unlocked(&nv_fence_context->base);
    }

    if (nv_gem) {
        nv_drm_gem_object_unreference_unlocked(nv_gem);
    }

    return ret;
}

#endif /* NV_DRM_FENCE_AVAILABLE */

#endif /* NV_DRM_AVAILABLE */
