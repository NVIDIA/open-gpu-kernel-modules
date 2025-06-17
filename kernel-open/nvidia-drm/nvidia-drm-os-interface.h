/*
 * Copyright (c) 2015-2025, NVIDIA CORPORATION. All rights reserved.
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

#ifndef __NVIDIA_DRM_OS_INTERFACE_H__
#define __NVIDIA_DRM_OS_INTERFACE_H__

#include "nvidia-drm-conftest.h" /* NV_DRM_AVAILABLE */

#include "nvtypes.h"

#if defined(NV_DRM_AVAILABLE)

#if defined(NV_DRM_FENCE_AVAILABLE)
#include "nvidia-dma-fence-helper.h"
#endif

#if defined(NV_LINUX) || defined(NV_BSD)
#include "nv-kthread-q.h"
#include "linux/spinlock.h"

typedef struct nv_drm_workthread {
    spinlock_t lock;
    struct nv_kthread_q q;
    bool shutting_down;
} nv_drm_workthread;

typedef nv_kthread_q_item_t nv_drm_work;

#else
#error "Need to define deferred work primitives for this OS"
#endif

#if defined(NV_LINUX) || defined(NV_BSD)
#include "nv-timer.h"

typedef struct nv_timer nv_drm_timer;

#else
#error "Need to define kernel timer callback primitives for this OS"
#endif

struct page;

/* Set to true when the atomic modeset feature is enabled. */
extern bool nv_drm_modeset_module_param;
#if defined(NV_DRM_FBDEV_AVAILABLE)
/* Set to true when the nvidia-drm driver should install a framebuffer device */
extern bool nv_drm_fbdev_module_param;
#endif

void *nv_drm_calloc(size_t nmemb, size_t size);

void nv_drm_free(void *ptr);

char *nv_drm_asprintf(const char *fmt, ...);

void nv_drm_write_combine_flush(void);

int nv_drm_lock_user_pages(unsigned long address,
                           unsigned long pages_count, struct page ***pages);

void nv_drm_unlock_user_pages(unsigned long  pages_count, struct page **pages);

void *nv_drm_vmap(struct page **pages, unsigned long pages_count, bool cached);

void nv_drm_vunmap(void *address);

bool nv_drm_workthread_init(nv_drm_workthread *worker, const char *name);

/* Can be called concurrently with nv_drm_workthread_add_work() */
void nv_drm_workthread_shutdown(nv_drm_workthread *worker);

void nv_drm_workthread_work_init(nv_drm_work *work,
                                 void (*callback)(void *),
                                 void *arg);

/* Can be called concurrently with nv_drm_workthread_shutdown() */
int nv_drm_workthread_add_work(nv_drm_workthread *worker, nv_drm_work *work);

void nv_drm_timer_setup(nv_drm_timer *timer,
                        void (*callback)(nv_drm_timer *nv_drm_timer));

void nv_drm_mod_timer(nv_drm_timer *timer, unsigned long relative_timeout_ms);

unsigned long nv_drm_timer_now(void);

unsigned long nv_drm_timeout_from_ms(NvU64 relative_timeout_ms);

#if defined(NV_DRM_FENCE_AVAILABLE)
int nv_drm_create_sync_file(nv_dma_fence_t *fence);

nv_dma_fence_t *nv_drm_sync_file_get_fence(int fd);
#endif /* defined(NV_DRM_FENCE_AVAILABLE) */

void nv_drm_yield(void);

#endif /* defined(NV_DRM_AVAILABLE) */

#endif /* __NVIDIA_DRM_OS_INTERFACE_H__ */
