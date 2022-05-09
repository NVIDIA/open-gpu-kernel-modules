/*
 * Copyright (c) 2016, NVIDIA CORPORATION. All rights reserved.
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
#include "nvidia-drm-prime-fence.h"
#include "nvidia-dma-resv-helper.h"

#if defined(NV_DRM_FENCE_AVAILABLE)

#include "nvidia-dma-fence-helper.h"

struct nv_drm_fence_context {
    struct nv_drm_device *nv_dev;

    uint32_t context;

    NvU64 fenceSemIndex; /* Index into semaphore surface */

    /* Mapped semaphore surface */
    struct NvKmsKapiMemory *pSemSurface;
    NvU32 *pLinearAddress;

    /* Protects nv_drm_fence_context::{pending, last_seqno} */
    spinlock_t lock;

    /*
     * Software signaling structures. __nv_drm_fence_context_new()
     * allocates channel event and __nv_drm_fence_context_destroy() frees it.
     * There are no simultaneous read/write access to 'cb', therefore it does
     * not require spin-lock protection.
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
nv_drm_gem_prime_fence_op_get_driver_name(nv_dma_fence_t *fence)
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
    .get_driver_name = nv_drm_gem_prime_fence_op_get_driver_name,
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
    struct nv_drm_fence_context *nv_fence_context)
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
    struct nv_drm_fence_context *nv_fence_context = dataPtr;

    spin_lock(&nv_fence_context->lock);

    while (!list_empty(&nv_fence_context->pending)) {
        struct nv_drm_prime_fence *nv_fence = list_first_entry(
            &nv_fence_context->pending,
            typeof(*nv_fence),
            list_entry);

        /* Index into surface with 16 byte stride */
        unsigned int seqno = *((nv_fence_context->pLinearAddress) +
                               (nv_fence_context->fenceSemIndex * 4));

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

static inline struct nv_drm_fence_context *__nv_drm_fence_context_new(
    struct nv_drm_device *nv_dev,
    struct drm_nvidia_fence_context_create_params *p)
{
    struct nv_drm_fence_context *nv_fence_context;
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

    if ((nv_fence_context = nv_drm_calloc(
                    1,
                    sizeof(*nv_fence_context))) == NULL) {
        goto failed_alloc_fence_context;
    }

    /*
     * nv_dma_fence_context_alloc() cannot fail, so we do not need
     * to check a return value.
     */

    *nv_fence_context = (struct nv_drm_fence_context) {
        .nv_dev = nv_dev,
        .context = nv_dma_fence_context_alloc(1),
        .pSemSurface = pSemSurface,
        .pLinearAddress = pLinearAddress,
        .fenceSemIndex = p->index,
    };

    INIT_LIST_HEAD(&nv_fence_context->pending);

    spin_lock_init(&nv_fence_context->lock);

    /*
     * Except 'cb', the fence context should be completely initialized
     * before channel event allocation because the fence context may start
     * receiving events immediately after allocation.
     *
     * There are no simultaneous read/write access to 'cb', therefore it does
     * not require spin-lock protection.
     */
    nv_fence_context->cb =
        nvKms->allocateChannelEvent(nv_dev->pDevice,
                                    nv_drm_gem_prime_fence_event,
                                    nv_fence_context,
                                    p->event_nvkms_params_ptr,
                                    p->event_nvkms_params_size);
    if (!nv_fence_context->cb) {
        NV_DRM_DEV_LOG_ERR(nv_dev,
                           "Failed to allocate fence signaling event");
        goto failed_to_allocate_channel_event;
    }

    return nv_fence_context;

failed_to_allocate_channel_event:
    nv_drm_free(nv_fence_context);

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

static void __nv_drm_fence_context_destroy(
    struct nv_drm_fence_context *nv_fence_context)
{
    struct nv_drm_device *nv_dev = nv_fence_context->nv_dev;

    /*
     * Free channel event before destroying the fence context, otherwise event
     * callback continue to get called.
     */
    nvKms->freeChannelEvent(nv_dev->pDevice, nv_fence_context->cb);

    /* Force signal all pending fences and empty pending list */
    spin_lock(&nv_fence_context->lock);

    nv_drm_gem_prime_force_fence_signal(nv_fence_context);

    spin_unlock(&nv_fence_context->lock);

    /* Free nvkms resources */

    nvKms->unmapMemory(nv_dev->pDevice,
                       nv_fence_context->pSemSurface,
                       NVKMS_KAPI_MAPPING_TYPE_KERNEL,
                       (void *) nv_fence_context->pLinearAddress);

    nvKms->freeMemory(nv_dev->pDevice, nv_fence_context->pSemSurface);

    nv_drm_free(nv_fence_context);
}

static nv_dma_fence_t *__nv_drm_fence_context_create_fence(
    struct nv_drm_fence_context *nv_fence_context,
    unsigned int seqno)
{
    struct nv_drm_prime_fence *nv_fence;
    int ret = 0;

    if ((nv_fence = nv_drm_calloc(1, sizeof(*nv_fence))) == NULL) {
        ret = -ENOMEM;
        goto out;
    }

    spin_lock(&nv_fence_context->lock);

    /*
     * If seqno wrapped, force signal fences to make sure none of them
     * get stuck.
     */
    if (seqno < nv_fence_context->last_seqno) {
        nv_drm_gem_prime_force_fence_signal(nv_fence_context);
    }

    INIT_LIST_HEAD(&nv_fence->list_entry);

    spin_lock_init(&nv_fence->lock);

    nv_dma_fence_init(&nv_fence->base, &nv_drm_gem_prime_fence_ops,
                      &nv_fence->lock, nv_fence_context->context,
                      seqno);

    list_add_tail(&nv_fence->list_entry, &nv_fence_context->pending);

    nv_fence_context->last_seqno = seqno;

    spin_unlock(&nv_fence_context->lock);

out:
    return ret != 0 ? ERR_PTR(ret) : &nv_fence->base;
}

int nv_drm_fence_supported_ioctl(struct drm_device *dev,
                                 void *data, struct drm_file *filep)
{
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    return nv_dev->pDevice ? 0 : -EINVAL;
}

struct nv_drm_gem_fence_context {
    struct nv_drm_gem_object base;
    struct nv_drm_fence_context *nv_fence_context;
};

static inline struct nv_drm_gem_fence_context *to_gem_fence_context(
    struct nv_drm_gem_object *nv_gem)
{
    if (nv_gem != NULL) {
        return container_of(nv_gem, struct nv_drm_gem_fence_context, base);
    }

    return NULL;
}

/*
 * Tear down of the 'struct nv_drm_gem_fence_context' object is not expected
 * to be happen from any worker thread, if that happen it causes dead-lock
 * because tear down sequence calls to flush all existing
 * worker thread.
 */
static void __nv_drm_gem_fence_context_free(struct nv_drm_gem_object *nv_gem)
{
    struct nv_drm_gem_fence_context *nv_gem_fence_context =
        to_gem_fence_context(nv_gem);

    __nv_drm_fence_context_destroy(nv_gem_fence_context->nv_fence_context);

    nv_drm_free(nv_gem_fence_context);
}

const struct nv_drm_gem_object_funcs nv_gem_fence_context_ops = {
    .free = __nv_drm_gem_fence_context_free,
};

static inline
struct nv_drm_gem_fence_context *__nv_drm_gem_object_fence_context_lookup(
    struct drm_device *dev,
    struct drm_file *filp,
    u32 handle)
{
    struct nv_drm_gem_object *nv_gem =
            nv_drm_gem_object_lookup(dev, filp, handle);

    if (nv_gem != NULL && nv_gem->ops != &nv_gem_fence_context_ops) {
        nv_drm_gem_object_unreference_unlocked(nv_gem);
        return NULL;
    }

    return to_gem_fence_context(nv_gem);
}

int nv_drm_fence_context_create_ioctl(struct drm_device *dev,
                                      void *data, struct drm_file *filep)
{
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    struct drm_nvidia_fence_context_create_params *p = data;
    struct nv_drm_gem_fence_context *nv_gem_fence_context = NULL;

    if ((nv_gem_fence_context = nv_drm_calloc(
                1,
                sizeof(struct nv_drm_gem_fence_context))) == NULL) {
        goto done;
    }

    if ((nv_gem_fence_context->nv_fence_context =
                __nv_drm_fence_context_new(nv_dev, p)) == NULL) {
        goto fence_context_new_failed;
    }

    nv_drm_gem_object_init(nv_dev,
                           &nv_gem_fence_context->base,
                           &nv_gem_fence_context_ops,
                           0 /* size */,
                           NULL /* pMemory */);

    return nv_drm_gem_handle_create_drop_reference(filep,
                                                   &nv_gem_fence_context->base,
                                                   &p->handle);

fence_context_new_failed:
    nv_drm_free(nv_gem_fence_context);

done:
    return -ENOMEM;
}

int nv_drm_gem_fence_attach_ioctl(struct drm_device *dev,
                                  void *data, struct drm_file *filep)
{
    int ret = -EINVAL;
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    struct drm_nvidia_gem_fence_attach_params *p = data;

    struct nv_drm_gem_object *nv_gem;
    struct nv_drm_gem_fence_context *nv_gem_fence_context;

    nv_dma_fence_t *fence;

    nv_gem = nv_drm_gem_object_lookup(nv_dev->dev, filep, p->handle);

    if (!nv_gem) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to lookup gem object for fence attach: 0x%08x",
            p->handle);

        goto done;
    }

    if((nv_gem_fence_context = __nv_drm_gem_object_fence_context_lookup(
                nv_dev->dev,
                filep,
                p->fence_context_handle)) == NULL) {

        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to lookup gem object for fence context: 0x%08x",
            p->fence_context_handle);

        goto fence_context_lookup_failed;
    }

    if (IS_ERR(fence = __nv_drm_fence_context_create_fence(
                            nv_gem_fence_context->nv_fence_context,
                            p->sem_thresh))) {
        ret = PTR_ERR(fence);

        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to allocate fence: 0x%08x", p->handle);

        goto fence_context_create_fence_failed;
    }

    nv_dma_resv_add_excl_fence(&nv_gem->resv, fence);

    ret = 0;

fence_context_create_fence_failed:
    nv_drm_gem_object_unreference_unlocked(&nv_gem_fence_context->base);

fence_context_lookup_failed:
    nv_drm_gem_object_unreference_unlocked(nv_gem);

done:
    return ret;
}

#endif /* NV_DRM_FENCE_AVAILABLE */

#endif /* NV_DRM_AVAILABLE */
