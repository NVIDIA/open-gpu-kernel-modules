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

#include <linux/slab.h>

#include "nvidia-drm-os-interface.h"

#if defined(NV_DRM_AVAILABLE)

#if defined(NV_LINUX_SYNC_FILE_H_PRESENT)
#include <linux/file.h>
#include <linux/sync_file.h>
#endif

#include <linux/vmalloc.h>
#include <linux/sched.h>
#include <linux/device.h>

#include "nv-mm.h"

#if defined(NV_DRM_DRMP_H_PRESENT)
#include <drm/drmP.h>
#endif

bool nv_drm_modeset_module_param = false;
bool nv_drm_fbdev_module_param = true;

void *nv_drm_calloc(size_t nmemb, size_t size)
{
    size_t total_size = nmemb * size;
    //
    // Check for overflow.
    //
    if ((nmemb != 0) && ((total_size / nmemb) != size))
    {
        return NULL;
    }
    return kzalloc(nmemb * size, GFP_KERNEL);
}

void nv_drm_free(void *ptr)
{
    if (IS_ERR(ptr)) {
        return;
    }

    kfree(ptr);
}

char *nv_drm_asprintf(const char *fmt, ...)
{
    va_list ap;
    char *p;

    va_start(ap, fmt);
    p = kvasprintf(GFP_KERNEL, fmt, ap);
    va_end(ap);

    return p;
}

#if defined(NVCPU_X86) || defined(NVCPU_X86_64)
  #define WRITE_COMBINE_FLUSH()    asm volatile("sfence":::"memory")
#elif defined(NVCPU_PPC64LE)
  #define WRITE_COMBINE_FLUSH()    asm volatile("sync":::"memory")
#else
  #define WRITE_COMBINE_FLUSH()    mb()
#endif

void nv_drm_write_combine_flush(void)
{
    WRITE_COMBINE_FLUSH();
}

int nv_drm_lock_user_pages(unsigned long address,
                           unsigned long pages_count, struct page ***pages)
{
    struct mm_struct *mm = current->mm;
    struct page **user_pages;
    int pages_pinned;

    user_pages = nv_drm_calloc(pages_count, sizeof(*user_pages));

    if (user_pages == NULL) {
        return -ENOMEM;
    }

    nv_mmap_read_lock(mm);

    pages_pinned = NV_PIN_USER_PAGES(address, pages_count, FOLL_WRITE,
                                     user_pages);
    nv_mmap_read_unlock(mm);

    if (pages_pinned < 0 || (unsigned)pages_pinned < pages_count) {
        goto failed;
    }

    *pages = user_pages;

    return 0;

failed:

    if (pages_pinned > 0) {
        int i;

        for (i = 0; i < pages_pinned; i++) {
           NV_UNPIN_USER_PAGE(user_pages[i]);
        }
    }

    nv_drm_free(user_pages);

    return (pages_pinned < 0) ? pages_pinned : -EINVAL;
}

void nv_drm_unlock_user_pages(unsigned long  pages_count, struct page **pages)
{
    unsigned long i;

    for (i = 0; i < pages_count; i++) {
        set_page_dirty_lock(pages[i]);
        NV_UNPIN_USER_PAGE(pages[i]);
    }

    nv_drm_free(pages);
}

/*
 * linuxkpi vmap doesn't use the flags argument as it
 * doesn't seem to be needed. Define VM_USERMAP to 0
 * to make errors go away
 *
 * vmap: sys/compat/linuxkpi/common/src/linux_compat.c
 */
#if defined(NV_BSD)
#define VM_USERMAP 0
#endif

void *nv_drm_vmap(struct page **pages, unsigned long pages_count, bool cached)
{
    pgprot_t prot = PAGE_KERNEL;

    if (!cached) {
        prot = pgprot_noncached(PAGE_KERNEL);
    }

    return vmap(pages, pages_count, VM_USERMAP, prot);
}

void nv_drm_vunmap(void *address)
{
    vunmap(address);
}

bool nv_drm_workthread_init(nv_drm_workthread *worker, const char *name)
{
    worker->shutting_down = false;
    if (nv_kthread_q_init(&worker->q, name)) {
        return false;
    }

    spin_lock_init(&worker->lock);

    return true;
}

void nv_drm_workthread_shutdown(nv_drm_workthread *worker)
{
    unsigned long flags;

    spin_lock_irqsave(&worker->lock, flags);
    worker->shutting_down = true;
    spin_unlock_irqrestore(&worker->lock, flags);

    nv_kthread_q_stop(&worker->q);
}

void nv_drm_workthread_work_init(nv_drm_work *work,
                                 void (*callback)(void *),
                                 void *arg)
{
    nv_kthread_q_item_init(work, callback, arg);
}

int nv_drm_workthread_add_work(nv_drm_workthread *worker, nv_drm_work *work)
{
    unsigned long flags;
    int ret = 0;

    spin_lock_irqsave(&worker->lock, flags);
    if (!worker->shutting_down) {
        ret = nv_kthread_q_schedule_q_item(&worker->q, work);
    }
    spin_unlock_irqrestore(&worker->lock, flags);

    return ret;
}

void nv_drm_timer_setup(nv_drm_timer *timer, void (*callback)(nv_drm_timer *nv_drm_timer))
{
    nv_timer_setup(timer, callback);
}

void nv_drm_mod_timer(nv_drm_timer *timer, unsigned long timeout_native)
{
    mod_timer(&timer->kernel_timer, timeout_native);
}

unsigned long nv_drm_timer_now(void)
{
    return jiffies;
}

unsigned long nv_drm_timeout_from_ms(NvU64 relative_timeout_ms)
{
    return jiffies + msecs_to_jiffies(relative_timeout_ms);
}

#if defined(NV_DRM_FENCE_AVAILABLE)
int nv_drm_create_sync_file(nv_dma_fence_t *fence)
{
#if defined(NV_LINUX_SYNC_FILE_H_PRESENT)
    struct sync_file *sync;
    int fd = get_unused_fd_flags(O_CLOEXEC);

    if (fd < 0) {
        return fd;
    }

    /* sync_file_create() generates its own reference to the fence */
    sync = sync_file_create(fence);

    if (IS_ERR(sync)) {
        put_unused_fd(fd);
        return PTR_ERR(sync);
    }

    fd_install(fd, sync->file);

    return fd;
#else /* defined(NV_LINUX_SYNC_FILE_H_PRESENT) */
    return -EINVAL;
#endif  /* defined(NV_LINUX_SYNC_FILE_H_PRESENT) */
}

nv_dma_fence_t *nv_drm_sync_file_get_fence(int fd)
{
#if defined(NV_SYNC_FILE_GET_FENCE_PRESENT)
    return sync_file_get_fence(fd);
#else /* defined(NV_SYNC_FILE_GET_FENCE_PRESENT) */
    return NULL;
#endif  /* defined(NV_SYNC_FILE_GET_FENCE_PRESENT) */
}
#endif /* defined(NV_DRM_FENCE_AVAILABLE) */

void nv_drm_yield(void)
{
    set_current_state(TASK_INTERRUPTIBLE);
    schedule_timeout(1);
}

#endif /* NV_DRM_AVAILABLE */
