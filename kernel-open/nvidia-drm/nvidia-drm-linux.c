/*
 * Copyright (c) 2015, NVIDIA CORPORATION. All rights reserved.
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

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/err.h>

#include "nvidia-drm-os-interface.h"
#include "nvidia-drm.h"

#include "nvidia-drm-conftest.h"

#if defined(NV_DRM_AVAILABLE)

#if defined(NV_DRM_DRMP_H_PRESENT)
#include <drm/drmP.h>
#endif

#include <linux/vmalloc.h>

#include "nv-mm.h"

MODULE_PARM_DESC(
    modeset,
    "Enable atomic kernel modesetting (1 = enable, 0 = disable (default))");
bool nv_drm_modeset_module_param = false;
module_param_named(modeset, nv_drm_modeset_module_param, bool, 0400);

void *nv_drm_calloc(size_t nmemb, size_t size)
{
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
#elif defined(NVCPU_FAMILY_ARM)
  #if defined(NVCPU_ARM)
    #define WRITE_COMBINE_FLUSH()  { dsb(); outer_sync(); }
  #elif defined(NVCPU_AARCH64)
    #define WRITE_COMBINE_FLUSH()  mb()
  #endif
#elif defined(NVCPU_PPC64LE)
  #define WRITE_COMBINE_FLUSH()    asm volatile("sync":::"memory")
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
    const int write = 1;
    const int force = 0;
    int pages_pinned;

    user_pages = nv_drm_calloc(pages_count, sizeof(*user_pages));

    if (user_pages == NULL) {
        return -ENOMEM;
    }

    nv_mmap_read_lock(mm);

    pages_pinned = NV_GET_USER_PAGES(address, pages_count, write, force,
                                     user_pages, NULL);
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
            put_page(user_pages[i]);
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

        put_page(pages[i]);
    }

    nv_drm_free(pages);
}

void *nv_drm_vmap(struct page **pages, unsigned long pages_count)
{
    return vmap(pages, pages_count, VM_USERMAP, PAGE_KERNEL);
}

void nv_drm_vunmap(void *address)
{
    vunmap(address);
}

#endif /* NV_DRM_AVAILABLE */

/*************************************************************************
 * Linux loading support code.
 *************************************************************************/

static int __init nv_linux_drm_init(void)
{
    return nv_drm_init();
}

static void __exit nv_linux_drm_exit(void)
{
    nv_drm_exit();
}

module_init(nv_linux_drm_init);
module_exit(nv_linux_drm_exit);

#if defined(MODULE_LICENSE)

  MODULE_LICENSE("Dual MIT/GPL");



#endif
#if defined(MODULE_INFO)
  MODULE_INFO(supported, "external");
#endif
#if defined(MODULE_VERSION)
  MODULE_VERSION(NV_VERSION_STRING);
#endif
