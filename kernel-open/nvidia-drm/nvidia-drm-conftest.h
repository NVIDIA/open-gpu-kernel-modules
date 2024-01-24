/*
 * Copyright (c) 2017, NVIDIA CORPORATION. All rights reserved.
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

#ifndef __NVIDIA_DRM_CONFTEST_H__
#define __NVIDIA_DRM_CONFTEST_H__

#include "conftest.h"
#include "nvtypes.h"

/*
 * NOTE: This file is expected to get included at the top before including any
 * of linux/drm headers.
 *
 * The goal is to redefine refcount_dec_and_test and refcount_inc before
 * including drm header files, so that the drm macro/inline calls to
 * refcount_dec_and_test* and refcount_inc get redirected to
 * alternate implementation in this file.
 */

#if NV_IS_EXPORT_SYMBOL_GPL_refcount_inc

#include <linux/refcount.h>

#define refcount_inc(__ptr)         \
    do {                            \
        atomic_inc(&(__ptr)->refs); \
    } while(0)

#endif

#if NV_IS_EXPORT_SYMBOL_GPL_refcount_dec_and_test

#include <linux/refcount.h>

#define refcount_dec_and_test(__ptr) atomic_dec_and_test(&(__ptr)->refs)

#endif

#if defined(NV_DRM_DRIVER_HAS_GEM_PRIME_RES_OBJ) || \
    defined(NV_DRM_GEM_OBJECT_HAS_RESV)
#define NV_DRM_FENCE_AVAILABLE
#else
#undef NV_DRM_FENCE_AVAILABLE
#endif

/*
 * We can support color management if either drm_helper_crtc_enable_color_mgmt()
 * or drm_crtc_enable_color_mgmt() exist.
 */
#if defined(NV_DRM_HELPER_CRTC_ENABLE_COLOR_MGMT_PRESENT) || \
    defined(NV_DRM_CRTC_ENABLE_COLOR_MGMT_PRESENT)
#define NV_DRM_COLOR_MGMT_AVAILABLE
#else
#undef NV_DRM_COLOR_MGMT_AVAILABLE
#endif

/*
 * Adapt to quirks in FreeBSD's Linux kernel compatibility layer.
 */
#if defined(NV_BSD)

#include <linux/rwsem.h>
#include <sys/param.h>
#include <sys/lock.h>
#include <sys/sx.h>

/* For nv_drm_gem_prime_force_fence_signal */
#ifndef spin_is_locked
#define spin_is_locked(lock) mtx_owned(lock.m)
#endif

#ifndef rwsem_is_locked
#define rwsem_is_locked(sem) (((sem)->sx.sx_lock & (SX_LOCK_SHARED)) \
                              || ((sem)->sx.sx_lock & ~(SX_LOCK_FLAGMASK & ~SX_LOCK_SHARED)))
#endif

/*
 * FreeBSD does not define vm_flags_t in its linuxkpi, since there is already
 * a FreeBSD vm_flags_t (of a different size) and they don't want the names to
 * collide. Temporarily redefine it when including nv-mm.h
 */
#define vm_flags_t unsigned long
#include "nv-mm.h"
#undef vm_flags_t

/*
 * sys/nv.h and nvidia/nv.h have the same header guard
 * we need to clear it for nvlist_t to get loaded
 */
#undef _NV_H_
#include <sys/nv.h>

/*
 * For now just use set_page_dirty as the lock variant
 * is not ported for FreeBSD. (in progress). This calls
 * vm_page_dirty. Used in nv-mm.h
 */
#define set_page_dirty_lock set_page_dirty

/*
 * FreeBSD does not implement drm_atomic_state_free, simply
 * default to drm_atomic_state_put
 */
#define drm_atomic_state_free drm_atomic_state_put

#if __FreeBSD_version < 1300000
/* redefine LIST_HEAD_INIT to the linux version */
#include <linux/list.h>
#define LIST_HEAD_INIT(name) LINUX_LIST_HEAD_INIT(name)
#endif

/*
 * FreeBSD currently has only vmf_insert_pfn_prot defined, and it has a
 * static assert warning not to use it since all of DRM's usages are in
 * loops with the vm obj lock(s) held. Instead we should use the lkpi
 * function itself directly. For us none of this applies so we can just
 * wrap it in our own definition of vmf_insert_pfn
 */
#ifndef NV_VMF_INSERT_PFN_PRESENT
#define NV_VMF_INSERT_PFN_PRESENT 1

#if __FreeBSD_version < 1300000
#define VM_SHARED       (1 << 17)

/* Not present in 12.2 */
static inline vm_fault_t
lkpi_vmf_insert_pfn_prot_locked(struct vm_area_struct *vma, unsigned long addr,
    unsigned long pfn, pgprot_t prot)
{
       vm_object_t vm_obj = vma->vm_obj;
       vm_page_t page;
       vm_pindex_t pindex;

       VM_OBJECT_ASSERT_WLOCKED(vm_obj);
       pindex = OFF_TO_IDX(addr - vma->vm_start);
       if (vma->vm_pfn_count == 0)
               vma->vm_pfn_first = pindex;
       MPASS(pindex <= OFF_TO_IDX(vma->vm_end));

       page = vm_page_grab(vm_obj, pindex, VM_ALLOC_NORMAL);
       if (page == NULL) {
               page = PHYS_TO_VM_PAGE(IDX_TO_OFF(pfn));
               vm_page_xbusy(page);
               if (vm_page_insert(page, vm_obj, pindex)) {
                       vm_page_xunbusy(page);
                       return (VM_FAULT_OOM);
               }
               page->valid = VM_PAGE_BITS_ALL;
       }
       pmap_page_set_memattr(page, pgprot2cachemode(prot));
       vma->vm_pfn_count++;

       return (VM_FAULT_NOPAGE);
}
#endif

static inline vm_fault_t
vmf_insert_pfn(struct vm_area_struct *vma, unsigned long addr,
    unsigned long pfn)
{
       vm_fault_t ret;

       VM_OBJECT_WLOCK(vma->vm_obj);
       ret = lkpi_vmf_insert_pfn_prot_locked(vma, addr, pfn, vma->vm_page_prot);
       VM_OBJECT_WUNLOCK(vma->vm_obj);

       return (ret);
}

#endif

#endif /* defined(NV_BSD) */

#endif /* defined(__NVIDIA_DRM_CONFTEST_H__) */
