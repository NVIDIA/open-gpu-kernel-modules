/*******************************************************************************
    Copyright (c) 2015-2023 NVIDIA Corporation

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

#include "uvm_api.h"
#include "uvm_global.h"
#include "uvm_gpu_replayable_faults.h"
#include "uvm_tools_init.h"
#include "uvm_lock.h"
#include "uvm_test.h"
#include "uvm_va_space.h"
#include "uvm_va_space_mm.h"
#include "uvm_va_range.h"
#include "uvm_va_block.h"
#include "uvm_tools.h"
#include "uvm_common.h"
#include "uvm_linux_ioctl.h"
#include "uvm_hmm.h"
#include "uvm_mem.h"
#include "uvm_kvmalloc.h"

#define NVIDIA_UVM_DEVICE_NAME          "nvidia-uvm"

static dev_t g_uvm_base_dev;
static struct cdev g_uvm_cdev;
static const struct file_operations uvm_fops;

bool uvm_file_is_nvidia_uvm(struct file *filp)
{
    return (filp != NULL) && (filp->f_op == &uvm_fops);
}

uvm_fd_type_t uvm_fd_type(struct file *filp, void **ptr_val)
{
    unsigned long uptr;
    uvm_fd_type_t type;
    void *ptr;

    UVM_ASSERT(uvm_file_is_nvidia_uvm(filp));

    uptr = atomic_long_read_acquire((atomic_long_t *) (&filp->private_data));
    type = (uvm_fd_type_t)(uptr & UVM_FD_TYPE_MASK);
    ptr = (void *)(uptr & ~UVM_FD_TYPE_MASK);
    BUILD_BUG_ON(UVM_FD_COUNT > UVM_FD_TYPE_MASK + 1);

    switch (type) {
        case UVM_FD_UNINITIALIZED:
        case UVM_FD_INITIALIZING:
            UVM_ASSERT(!ptr);
            break;

        case UVM_FD_VA_SPACE:
            UVM_ASSERT(ptr);
            BUILD_BUG_ON(__alignof__(uvm_va_space_t) < (1UL << UVM_FD_TYPE_BITS));
            break;

        case UVM_FD_MM:
            UVM_ASSERT(ptr);
            BUILD_BUG_ON(__alignof__(struct file) < (1UL << UVM_FD_TYPE_BITS));
            break;

        default:
            UVM_ASSERT(0);
    }

    if (ptr_val)
        *ptr_val = ptr;

    return type;
}

void *uvm_fd_get_type(struct file *filp, uvm_fd_type_t type)
{
    void *ptr;

    UVM_ASSERT(uvm_file_is_nvidia_uvm(filp));

    if (uvm_fd_type(filp, &ptr) == type)
        return ptr;
    else
        return NULL;
}

static NV_STATUS uvm_api_mm_initialize(UVM_MM_INITIALIZE_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space;
    uvm_va_space_mm_t *va_space_mm;
    struct file *uvm_file;
    uvm_fd_type_t old_fd_type;
    struct mm_struct *mm;
    NV_STATUS status;

    uvm_file = fget(params->uvmFd);
    if (!uvm_file_is_nvidia_uvm(uvm_file)) {
        status = NV_ERR_INVALID_ARGUMENT;
        goto err;
    }

    if (uvm_fd_type(uvm_file, (void **)&va_space) != UVM_FD_VA_SPACE) {
        status = NV_ERR_INVALID_ARGUMENT;
        goto err;
    }

    // Tell userspace the MM FD is not required and it may be released
    // with no loss of functionality.
    if (!uvm_va_space_mm_enabled(va_space)) {
        status = NV_WARN_NOTHING_TO_DO;
        goto err;
    }

    old_fd_type = nv_atomic_long_cmpxchg((atomic_long_t *)&filp->private_data,
                                         UVM_FD_UNINITIALIZED,
                                         UVM_FD_INITIALIZING);
    old_fd_type &= UVM_FD_TYPE_MASK;
    if (old_fd_type != UVM_FD_UNINITIALIZED) {
        status = NV_ERR_IN_USE;
        goto err;
    }

    va_space_mm = &va_space->va_space_mm;
    uvm_spin_lock(&va_space_mm->lock);
    switch (va_space->va_space_mm.state) {
        // We only allow the va_space_mm to be initialised once. If
        // userspace passed the UVM FD to another process it is up to
        // userspace to ensure it also passes the UVM MM FD that
        // initialised the va_space_mm or arranges some other way to keep
        // a reference on the FD.
        case UVM_VA_SPACE_MM_STATE_ALIVE:
            status = NV_ERR_INVALID_STATE;
            goto err_release_unlock;
            break;

        // Once userspace has released the va_space_mm the GPU is
        // effectively dead and no new work can be started. We don't
        // support re-initializing once userspace has closed the FD.
        case UVM_VA_SPACE_MM_STATE_RELEASED:
            status = NV_ERR_PAGE_TABLE_NOT_AVAIL;
            goto err_release_unlock;
            break;

        // Keep the warnings at bay
        case UVM_VA_SPACE_MM_STATE_UNINITIALIZED:
            mm = va_space->va_space_mm.mm;
            if (!mm || !mmget_not_zero(mm)) {
                status = NV_ERR_PAGE_TABLE_NOT_AVAIL;
                goto err_release_unlock;
            }

            va_space_mm->state = UVM_VA_SPACE_MM_STATE_ALIVE;
            break;

        default:
            UVM_ASSERT(0);
            break;
    }
    uvm_spin_unlock(&va_space_mm->lock);
    atomic_long_set_release((atomic_long_t *)&filp->private_data, (long)uvm_file | UVM_FD_MM);

    return NV_OK;

err_release_unlock:
    uvm_spin_unlock(&va_space_mm->lock);
    atomic_long_set_release((atomic_long_t *)&filp->private_data, UVM_FD_UNINITIALIZED);

err:
    if (uvm_file)
        fput(uvm_file);

    return status;
}

// Called when opening /dev/nvidia-uvm. This code doesn't take any UVM locks, so
// there's no need to acquire g_uvm_global.pm.lock, but if that changes the PM
// lock will need to be taken.
static int uvm_open(struct inode *inode, struct file *filp)
{
    struct address_space *mapping;
    NV_STATUS status = uvm_global_get_status();

    if (status != NV_OK)
        return -nv_status_to_errno(status);

    mapping = uvm_kvmalloc(sizeof(*mapping));
    if (!mapping)
        return -ENOMEM;

    // By default all struct files on the same inode share the same
    // address_space structure (the inode's) across all processes. This means
    // unmap_mapping_range would unmap virtual mappings across all processes on
    // that inode.
    //
    // Since the UVM driver uses the mapping offset as the VA of the file's
    // process, we need to isolate the mappings to each process.
    address_space_init_once(mapping);
    mapping->host = inode;

    // Some paths in the kernel, for example force_page_cache_readahead which
    // can be invoked from user-space via madvise MADV_WILLNEED and fadvise
    // POSIX_FADV_WILLNEED, check the function pointers within
    // file->f_mapping->a_ops for validity. However, those paths assume that a_ops
    // itself is always valid. Handle that by using the inode's a_ops pointer,
    // which is what f_mapping->a_ops would point to anyway if we weren't re-
    // assigning f_mapping.
    mapping->a_ops = inode->i_mapping->a_ops;

#if defined(NV_ADDRESS_SPACE_HAS_BACKING_DEV_INFO)
    mapping->backing_dev_info = inode->i_mapping->backing_dev_info;
#endif

    filp->private_data = NULL;
    filp->f_mapping = mapping;

    return NV_OK;
}

static int uvm_open_entry(struct inode *inode, struct file *filp)
{
   UVM_ENTRY_RET(uvm_open(inode, filp));
}

static void uvm_release_deferred(void *data)
{
    uvm_va_space_t *va_space = data;

    // Since this function is only scheduled to run when uvm_release() fails
    // to trylock-acquire the pm.lock, the following acquisition attempt
    // is expected to block this thread, and cause it to remain blocked until
    // uvm_resume() releases the lock.  As a result, the deferred release
    // kthread queue may stall for long periods of time.
    uvm_down_read(&g_uvm_global.pm.lock);

    uvm_va_space_destroy(va_space);

    uvm_up_read(&g_uvm_global.pm.lock);
}

static void uvm_mm_release(struct file *filp, struct file *uvm_file)
{
    uvm_va_space_t *va_space = uvm_va_space_get(uvm_file);
    uvm_va_space_mm_t *va_space_mm = &va_space->va_space_mm;
    struct mm_struct *mm = va_space_mm->mm;

    if (uvm_va_space_mm_enabled(va_space)) {
        uvm_va_space_mm_unregister(va_space);

        if (uvm_va_space_mm_enabled(va_space))
            uvm_mmput(mm);

        va_space_mm->mm = NULL;
        fput(uvm_file);
    }
}

static int uvm_release(struct inode *inode, struct file *filp)
{
    void *ptr;
    uvm_va_space_t *va_space;
    uvm_fd_type_t fd_type;
    int ret;

    fd_type = uvm_fd_type(filp, &ptr);
    UVM_ASSERT(fd_type != UVM_FD_INITIALIZING);
    if (fd_type == UVM_FD_UNINITIALIZED) {
        uvm_kvfree(filp->f_mapping);
        return 0;
    }
    else if (fd_type == UVM_FD_MM) {
        uvm_kvfree(filp->f_mapping);
        uvm_mm_release(filp, (struct file *)ptr);
        return 0;
    }

    UVM_ASSERT(fd_type == UVM_FD_VA_SPACE);
    va_space = (uvm_va_space_t *)ptr;
    filp->private_data = NULL;
    filp->f_mapping = NULL;

    // Because the kernel discards the status code returned from this release
    // callback, early exit in case of a pm.lock acquisition failure is not
    // an option.  Instead, the teardown work normally performed synchronously
    // needs to be scheduled to run after uvm_resume() releases the lock.
    if (uvm_down_read_trylock(&g_uvm_global.pm.lock)) {
        uvm_va_space_destroy(va_space);
        uvm_up_read(&g_uvm_global.pm.lock);
    }
    else {
        // Remove references to this inode from the address_space.  This isn't
        // strictly necessary, as any CPU mappings of this file have already
        // been destroyed, and va_space->mapping won't be used again. Still,
        // the va_space survives the inode if its destruction is deferred, in
        // which case the references are rendered stale.
        address_space_init_once(va_space->mapping);

        nv_kthread_q_item_init(&va_space->deferred_release_q_item, uvm_release_deferred, va_space);
        ret = nv_kthread_q_schedule_q_item(&g_uvm_global.deferred_release_q, &va_space->deferred_release_q_item);
        UVM_ASSERT(ret != 0);
    }

    return 0;
}

static int uvm_release_entry(struct inode *inode, struct file *filp)
{
   UVM_ENTRY_RET(uvm_release(inode, filp));
}

static void uvm_destroy_vma_managed(struct vm_area_struct *vma, bool make_zombie)
{
    uvm_va_range_t *va_range, *va_range_next;
    NvU64 size = 0;

    uvm_assert_rwsem_locked_write(&uvm_va_space_get(vma->vm_file)->lock);
    uvm_for_each_va_range_in_vma_safe(va_range, va_range_next, vma) {
        // On exit_mmap (process teardown), current->mm is cleared so
        // uvm_va_range_vma_current would return NULL.
        UVM_ASSERT(uvm_va_range_vma(va_range) == vma);
        UVM_ASSERT(va_range->node.start >= vma->vm_start);
        UVM_ASSERT(va_range->node.end   <  vma->vm_end);
        size += uvm_va_range_size(va_range);
        if (make_zombie)
            uvm_va_range_zombify(va_range);
        else
            uvm_va_range_destroy(va_range, NULL);
    }

    if (vma->vm_private_data) {
        uvm_vma_wrapper_destroy(vma->vm_private_data);
        vma->vm_private_data = NULL;
    }
    UVM_ASSERT(size == vma->vm_end - vma->vm_start);
}

static void uvm_destroy_vma_semaphore_pool(struct vm_area_struct *vma)
{
    uvm_va_space_t *va_space;
    uvm_va_range_t *va_range;

    va_space = uvm_va_space_get(vma->vm_file);
    uvm_assert_rwsem_locked(&va_space->lock);
    va_range = uvm_va_range_find(va_space, vma->vm_start);
    UVM_ASSERT(va_range &&
               va_range->node.start   == vma->vm_start &&
               va_range->node.end + 1 == vma->vm_end &&
               va_range->type == UVM_VA_RANGE_TYPE_SEMAPHORE_POOL);

    uvm_mem_unmap_cpu_user(va_range->semaphore_pool.mem);
}

// If a fault handler is not set, paths like handle_pte_fault in older kernels
// assume the memory is anonymous. That would make debugging this failure harder
// so we force it to fail instead.
static vm_fault_t uvm_vm_fault_sigbus(struct vm_area_struct *vma, struct vm_fault *vmf)
{
    UVM_DBG_PRINT_RL("Fault to address 0x%lx in disabled vma\n", nv_page_fault_va(vmf));
    return VM_FAULT_SIGBUS;
}

static vm_fault_t uvm_vm_fault_sigbus_entry(struct vm_area_struct *vma, struct vm_fault *vmf)
{
    UVM_ENTRY_RET(uvm_vm_fault_sigbus(vma, vmf));
}

static vm_fault_t uvm_vm_fault_sigbus_wrapper(struct vm_fault *vmf)
{
#if defined(NV_VM_OPS_FAULT_REMOVED_VMA_ARG)
    return uvm_vm_fault_sigbus(vmf->vma, vmf);
#else
    return uvm_vm_fault_sigbus(NULL, vmf);
#endif
}

static vm_fault_t uvm_vm_fault_sigbus_wrapper_entry(struct vm_fault *vmf)
{
    UVM_ENTRY_RET(uvm_vm_fault_sigbus_wrapper(vmf));
}

static struct vm_operations_struct uvm_vm_ops_disabled =
{
#if defined(NV_VM_OPS_FAULT_REMOVED_VMA_ARG)
    .fault = uvm_vm_fault_sigbus_wrapper_entry
#else
    .fault = uvm_vm_fault_sigbus_entry
#endif
};

static void uvm_disable_vma(struct vm_area_struct *vma)
{
    // In the case of fork, the kernel has already copied the old PTEs over to
    // the child process, so an access in the child might succeed instead of
    // causing a fault. To force a fault we'll unmap it directly here.
    //
    // Note that since the unmap works on file offset, not virtual address, this
    // unmaps both the old and new vmas.
    //
    // In the case of a move (mremap), the kernel will copy the PTEs over later,
    // so it doesn't matter if we unmap here. However, the new vma's open will
    // immediately be followed by a close on the old vma. We call
    // unmap_mapping_range for the close, which also unmaps the new vma because
    // they have the same file offset.
    unmap_mapping_range(vma->vm_file->f_mapping,
                        vma->vm_pgoff << PAGE_SHIFT,
                        vma->vm_end - vma->vm_start,
                        1);

    vma->vm_ops = &uvm_vm_ops_disabled;

    if (vma->vm_private_data) {
        uvm_vma_wrapper_destroy(vma->vm_private_data);
        vma->vm_private_data = NULL;
    }
}

// We can't return an error from uvm_vm_open so on failed splits
// we'll disable *both* vmas. This isn't great behavior for the
// user, but we don't have many options. We could leave the old VA
// range in place but that breaks the model of vmas always
// completely covering VA ranges. We'd have to be very careful
// handling later splits and closes of both that partially-covered
// VA range, and of the vmas which might or might not cover it any
// more.
//
// A failure likely means we're in OOM territory, so this should not
// be common by any means, and the process might die anyway.
static void uvm_vm_open_failure(struct vm_area_struct *original,
                                struct vm_area_struct *new)
{
    uvm_va_space_t *va_space = uvm_va_space_get(new->vm_file);
    static const bool make_zombie = false;

    UVM_ASSERT(va_space == uvm_va_space_get(original->vm_file));
    uvm_assert_rwsem_locked_write(&va_space->lock);

    uvm_destroy_vma_managed(original, make_zombie);
    uvm_disable_vma(original);
    uvm_disable_vma(new);
}

// vm_ops->open cases:
//
// 1) Parent vma is dup'd (fork)
//    This is undefined behavior in the UVM Programming Model. For convenience
//    the parent will continue operating properly, but the child is not
//    guaranteed access to the range.
//
// 2) Original vma is split (munmap, mprotect, mremap, mbind, etc)
//    The UVM Programming Model supports mbind always and supports mprotect if
//    HMM is present. Supporting either of those means all such splitting cases
//    must be handled. This involves splitting the va_range covering the split
//    location. Note that the kernel will never merge us back on two counts: we
//    set VM_MIXEDMAP and we have a ->close callback.
//
// 3) Original vma is moved (mremap)
//    This is undefined behavior in the UVM Programming Model. We'll get an open
//    on the new vma in which we disable operations on the new vma, then a close
//    on the old vma.
//
// Note that since we set VM_DONTEXPAND on the vma we're guaranteed that the vma
// will never increase in size, only shrink/split.
static void uvm_vm_open_managed(struct vm_area_struct *vma)
{
    uvm_va_space_t *va_space = uvm_va_space_get(vma->vm_file);
    uvm_va_range_t *va_range;
    struct vm_area_struct *original;
    NV_STATUS status;
    NvU64 new_end;

    // This is slightly ugly. We need to know the parent vma of this new one,
    // but we can't use the range tree to look up the original because that
    // doesn't handle a vma move operation.
    //
    // However, all of the old vma's fields have been copied into the new vma,
    // and open of the new vma is always called before close of the old (in
    // cases where close will be called immediately afterwards, like move).
    // vma->vm_private_data will thus still point to the original vma that we
    // set in mmap or open.
    //
    // Things to watch out for here:
    // - For splits, the old vma hasn't been adjusted yet so its vm_start and
    //   vm_end region will overlap with this vma's start and end.
    //
    // - For splits and moves, the new vma has not yet been inserted into the
    //   mm's list so vma->vm_prev and vma->vm_next cannot be used, nor will
    //   the new vma show up in find_vma and friends.
    original = ((uvm_vma_wrapper_t*)vma->vm_private_data)->vma;
    vma->vm_private_data = NULL;
    // On fork or move we want to simply disable the new vma
    if (vma->vm_mm != original->vm_mm ||
        (vma->vm_start != original->vm_start && vma->vm_end != original->vm_end)) {
        uvm_disable_vma(vma);
        return;
    }

    // At this point we are guaranteed that the mmap_lock is held in write
    // mode.
    uvm_record_lock_mmap_lock_write(current->mm);

    // Split vmas should always fall entirely within the old one, and be on one
    // side.
    UVM_ASSERT(vma->vm_start >= original->vm_start && vma->vm_end <= original->vm_end);
    UVM_ASSERT(vma->vm_start == original->vm_start || vma->vm_end == original->vm_end);

    // The vma is splitting, so create a new range under this vma if necessary.
    // The kernel handles splits in the middle of the vma by doing two separate
    // splits so we just have to handle one vma splitting in two here.
    if (vma->vm_start == original->vm_start)
        new_end = vma->vm_end - 1; // Left split (new_end is inclusive)
    else
        new_end = vma->vm_start - 1; // Right split (new_end is inclusive)

    uvm_va_space_down_write(va_space);

    vma->vm_private_data = uvm_vma_wrapper_alloc(vma);
    if (!vma->vm_private_data) {
        uvm_vm_open_failure(original, vma);
        goto out;
    }

    // There can be multiple va_ranges under the vma already. Check if one spans
    // the new split boundary. If so, split it.
    va_range = uvm_va_range_find(va_space, new_end);
    UVM_ASSERT(va_range);
    UVM_ASSERT(uvm_va_range_vma_current(va_range) == original);
    if (va_range->node.end != new_end) {
        status = uvm_va_range_split(va_range, new_end, NULL);
        if (status != NV_OK) {
            UVM_DBG_PRINT("Failed to split VA range, destroying both: %s. "
                          "original vma [0x%lx, 0x%lx) new vma [0x%lx, 0x%lx)\n",
                          nvstatusToString(status),
                          original->vm_start, original->vm_end,
                          vma->vm_start, vma->vm_end);
            uvm_vm_open_failure(original, vma);
            goto out;
        }
    }

    // Point va_ranges to the new vma
    uvm_for_each_va_range_in_vma(va_range, vma) {
        UVM_ASSERT(uvm_va_range_vma_current(va_range) == original);
        va_range->managed.vma_wrapper = vma->vm_private_data;
    }

out:
    uvm_va_space_up_write(va_space);
    uvm_record_unlock_mmap_lock_write(current->mm);
}

static void uvm_vm_open_managed_entry(struct vm_area_struct *vma)
{
   UVM_ENTRY_VOID(uvm_vm_open_managed(vma));
}

static void uvm_vm_close_managed(struct vm_area_struct *vma)
{
    uvm_va_space_t *va_space = uvm_va_space_get(vma->vm_file);
    bool make_zombie = false;

    if (current->mm != NULL)
        uvm_record_lock_mmap_lock_write(current->mm);

    // current->mm will be NULL on process teardown, in which case we have
    // special handling.
    if (current->mm == NULL) {
        make_zombie = (va_space->initialization_flags & UVM_INIT_FLAGS_MULTI_PROCESS_SHARING_MODE);
        if (!make_zombie) {
            // If we're not in multi-process mode, then we want to stop all user
            // channels before unmapping the managed allocations to avoid
            // spurious MMU faults in the system log. If we have a va_space_mm
            // then this must've already happened as part of
            // uvm_va_space_mm_shutdown. Otherwise we need to handle it here.
            if (uvm_va_space_mm_enabled(va_space) && current->mm == va_space->va_space_mm.mm) {
                UVM_ASSERT(atomic_read(&va_space->user_channels_stopped));
            }
            else {
                // Stopping channels involves making RM calls, so we have to do
                // that with the VA space lock in read mode.
                uvm_va_space_down_read_rm(va_space);
                if (!atomic_read(&va_space->user_channels_stopped))
                    uvm_va_space_stop_all_user_channels(va_space);
                uvm_va_space_up_read_rm(va_space);
            }
        }
    }

    // See uvm_mmap for why we need this in addition to mmap_lock
    uvm_va_space_down_write(va_space);

    uvm_destroy_vma_managed(vma, make_zombie);

    uvm_va_space_up_write(va_space);

    if (current->mm != NULL)
        uvm_record_unlock_mmap_lock_write(current->mm);
}

static void uvm_vm_close_managed_entry(struct vm_area_struct *vma)
{
    UVM_ENTRY_VOID(uvm_vm_close_managed(vma));
}

static vm_fault_t uvm_vm_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
{
    uvm_va_space_t *va_space = uvm_va_space_get(vma->vm_file);

    return uvm_va_space_cpu_fault_managed(va_space, vma, vmf);
}

static vm_fault_t uvm_vm_fault_entry(struct vm_area_struct *vma, struct vm_fault *vmf)
{
    UVM_ENTRY_RET(uvm_vm_fault(vma, vmf));
}

static vm_fault_t uvm_vm_fault_wrapper(struct vm_fault *vmf)
{
#if defined(NV_VM_OPS_FAULT_REMOVED_VMA_ARG)
    return uvm_vm_fault(vmf->vma, vmf);
#else
    return uvm_vm_fault(NULL, vmf);
#endif
}

static vm_fault_t uvm_vm_fault_wrapper_entry(struct vm_fault *vmf)
{
    UVM_ENTRY_RET(uvm_vm_fault_wrapper(vmf));
}

static struct vm_operations_struct uvm_vm_ops_managed =
{
    .open         = uvm_vm_open_managed_entry,
    .close        = uvm_vm_close_managed_entry,

#if defined(NV_VM_OPS_FAULT_REMOVED_VMA_ARG)
    .fault        = uvm_vm_fault_wrapper_entry,
    .page_mkwrite = uvm_vm_fault_wrapper_entry,
#else
    .fault        = uvm_vm_fault_entry,
    .page_mkwrite = uvm_vm_fault_entry,
#endif
};

// vm operations on semaphore pool allocations only control CPU mappings. Unmapping GPUs,
// freeing the allocation, and destroying the va_range are handled by UVM_FREE.
static void uvm_vm_open_semaphore_pool(struct vm_area_struct *vma)
{
    struct vm_area_struct *origin_vma = (struct vm_area_struct *)vma->vm_private_data;
    uvm_va_space_t *va_space = uvm_va_space_get(origin_vma->vm_file);
    uvm_va_range_t *va_range;
    bool is_fork = (vma->vm_mm != origin_vma->vm_mm);
    NV_STATUS status;

    uvm_record_lock_mmap_lock_write(current->mm);

    uvm_va_space_down_write(va_space);

    va_range = uvm_va_range_find(va_space, origin_vma->vm_start);
    UVM_ASSERT(va_range);
    UVM_ASSERT_MSG(va_range->type == UVM_VA_RANGE_TYPE_SEMAPHORE_POOL &&
                   va_range->node.start == origin_vma->vm_start &&
                   va_range->node.end + 1 == origin_vma->vm_end,
                   "origin vma [0x%llx, 0x%llx); va_range [0x%llx, 0x%llx) type %d\n",
                   (NvU64)origin_vma->vm_start, (NvU64)origin_vma->vm_end, va_range->node.start,
                   va_range->node.end + 1, va_range->type);

    // Semaphore pool vmas do not have vma wrappers, but some functions will
    // assume vm_private_data is a wrapper.
    vma->vm_private_data = NULL;

    if (is_fork) {
        // If we forked, leave the parent vma alone.
        uvm_disable_vma(vma);

        // uvm_disable_vma unmaps in the parent as well; clear the uvm_mem CPU
        // user mapping metadata and then remap.
        uvm_mem_unmap_cpu_user(va_range->semaphore_pool.mem);

        status = uvm_mem_map_cpu_user(va_range->semaphore_pool.mem, va_range->va_space, origin_vma);
        if (status != NV_OK) {
            UVM_DBG_PRINT("Failed to remap semaphore pool to CPU for parent after fork; status = %d (%s)",
                    status, nvstatusToString(status));
            origin_vma->vm_ops = &uvm_vm_ops_disabled;
        }
    }
    else {
        origin_vma->vm_private_data = NULL;
        origin_vma->vm_ops = &uvm_vm_ops_disabled;
        vma->vm_ops = &uvm_vm_ops_disabled;
        uvm_mem_unmap_cpu_user(va_range->semaphore_pool.mem);
    }

    uvm_va_space_up_write(va_space);

    uvm_record_unlock_mmap_lock_write(current->mm);
}

static void uvm_vm_open_semaphore_pool_entry(struct vm_area_struct *vma)
{
   UVM_ENTRY_VOID(uvm_vm_open_semaphore_pool(vma));
}

// vm operations on semaphore pool allocations only control CPU mappings. Unmapping GPUs,
// freeing the allocation, and destroying the va_range are handled by UVM_FREE.
static void uvm_vm_close_semaphore_pool(struct vm_area_struct *vma)
{
    uvm_va_space_t *va_space = uvm_va_space_get(vma->vm_file);

    if (current->mm != NULL)
        uvm_record_lock_mmap_lock_write(current->mm);

    uvm_va_space_down_read(va_space);

    uvm_destroy_vma_semaphore_pool(vma);

    uvm_va_space_up_read(va_space);

    if (current->mm != NULL)
        uvm_record_unlock_mmap_lock_write(current->mm);
}

static void uvm_vm_close_semaphore_pool_entry(struct vm_area_struct *vma)
{
   UVM_ENTRY_VOID(uvm_vm_close_semaphore_pool(vma));
}

static struct vm_operations_struct uvm_vm_ops_semaphore_pool =
{
    .open         = uvm_vm_open_semaphore_pool_entry,
    .close        = uvm_vm_close_semaphore_pool_entry,

#if defined(NV_VM_OPS_FAULT_REMOVED_VMA_ARG)
    .fault        = uvm_vm_fault_sigbus_wrapper_entry,
#else
    .fault        = uvm_vm_fault_sigbus_entry,
#endif
};

static int uvm_mmap(struct file *filp, struct vm_area_struct *vma)
{
    uvm_va_space_t *va_space;
    uvm_va_range_t *va_range;
    NV_STATUS status = uvm_global_get_status();
    int ret = 0;
    bool vma_wrapper_allocated = false;

    if (status != NV_OK)
        return -nv_status_to_errno(status);

    va_space = uvm_fd_va_space(filp);
    if (!va_space)
        return -EBADFD;

    // When the VA space is associated with an mm, all vmas under the VA space
    // must come from that mm.
    if (uvm_va_space_mm_enabled(va_space)) {
        UVM_ASSERT(va_space->va_space_mm.mm);
        if (va_space->va_space_mm.mm != current->mm)
            return -EOPNOTSUPP;
    }

    // UVM mappings are required to set offset == VA. This simplifies things
    // since we don't have to worry about address aliasing (except for fork,
    // handled separately) and it makes unmap_mapping_range simpler.
    if (vma->vm_start != (vma->vm_pgoff << PAGE_SHIFT)) {
        UVM_DBG_PRINT_RL("vm_start 0x%lx != vm_pgoff 0x%lx\n", vma->vm_start, vma->vm_pgoff << PAGE_SHIFT);
        return -EINVAL;
    }

    // Enforce shared read/writable mappings so we get all fault callbacks
    // without the kernel doing COW behind our backs. The user can still call
    // mprotect to change protections, but that will only hurt user space.
    if ((vma->vm_flags & (VM_SHARED|VM_READ|VM_WRITE)) !=
                         (VM_SHARED|VM_READ|VM_WRITE)) {
        UVM_DBG_PRINT_RL("User requested non-shared or non-writable mapping\n");
        return -EINVAL;
    }

    // If the PM lock cannot be acquired, disable the VMA and report success
    // to the caller.  The caller is expected to determine whether the
    // map operation succeeded via an ioctl() call.  This is necessary to
    // safely handle MAP_FIXED, which needs to complete atomically to prevent
    // the loss of the virtual address range.
    if (!uvm_down_read_trylock(&g_uvm_global.pm.lock)) {
        uvm_disable_vma(vma);
        return 0;
    }

    uvm_record_lock_mmap_lock_write(current->mm);

    // VM_MIXEDMAP      Required to use vm_insert_page
    //
    // VM_DONTEXPAND    mremap can grow a vma in place without giving us any
    //                  callback. We need to prevent this so our ranges stay
    //                  up-to-date with the vma. This flag doesn't prevent
    //                  mremap from moving the mapping elsewhere, nor from
    //                  shrinking it. We can detect both of those cases however
    //                  with vm_ops->open() and vm_ops->close() callbacks.
    //
    // Using VM_DONTCOPY would be nice, but madvise(MADV_DOFORK) can reset that
    // so we have to handle vm_open on fork anyway. We could disable MADV_DOFORK
    // with VM_IO, but that causes other mapping issues.
    // Make the default behavior be VM_DONTCOPY to avoid the performance impact
    // of removing CPU mappings in the parent on fork()+exec(). Users can call
    // madvise(MDV_DOFORK) if the child process requires access to the
    // allocation.
    nv_vm_flags_set(vma, VM_MIXEDMAP | VM_DONTEXPAND | VM_DONTCOPY);

    vma->vm_ops = &uvm_vm_ops_managed;

    // This identity assignment is needed so uvm_vm_open can find its parent vma
    vma->vm_private_data = uvm_vma_wrapper_alloc(vma);
    if (!vma->vm_private_data) {
        ret = -ENOMEM;
        goto out;
    }
    vma_wrapper_allocated = true;

    // The kernel has taken mmap_lock in write mode, but that doesn't prevent
    // this va_space from being modified by the GPU fault path or from the ioctl
    // path where we don't have this mm for sure, so we have to lock the VA
    // space directly.
    uvm_va_space_down_write(va_space);

    // uvm_va_range_create_mmap will catch collisions. Below are some example
    // cases which can cause collisions. There may be others.
    // 1) An overlapping range was previously created with an ioctl, for example
    //    for an external mapping.
    // 2) This file was passed to another process via a UNIX domain socket
    status = uvm_va_range_create_mmap(va_space, current->mm, vma->vm_private_data, NULL);

    if (status == NV_ERR_UVM_ADDRESS_IN_USE) {
        // If the mmap is for a semaphore pool, the VA range will have been
        // allocated by a previous ioctl, and the mmap just creates the CPU
        // mapping.
        va_range = uvm_va_range_find(va_space, vma->vm_start);
        if (va_range && va_range->node.start == vma->vm_start &&
                va_range->node.end + 1 == vma->vm_end &&
                va_range->type == UVM_VA_RANGE_TYPE_SEMAPHORE_POOL) {
            uvm_vma_wrapper_destroy(vma->vm_private_data);
            vma_wrapper_allocated = false;
            vma->vm_private_data = vma;
            vma->vm_ops = &uvm_vm_ops_semaphore_pool;
            status = uvm_mem_map_cpu_user(va_range->semaphore_pool.mem, va_range->va_space, vma);
        }
    }

    if (status != NV_OK) {
        UVM_DBG_PRINT_RL("Failed to create or map VA range for vma [0x%lx, 0x%lx): %s\n",
                         vma->vm_start, vma->vm_end, nvstatusToString(status));
        ret = -nv_status_to_errno(status);
    }

    uvm_va_space_up_write(va_space);

out:
    if (ret != 0 && vma_wrapper_allocated)
        uvm_vma_wrapper_destroy(vma->vm_private_data);

    uvm_record_unlock_mmap_lock_write(current->mm);

    uvm_up_read(&g_uvm_global.pm.lock);

    return ret;
}

bool uvm_vma_is_managed(struct vm_area_struct *vma)
{
    return vma->vm_ops == &uvm_vm_ops_disabled ||
           vma->vm_ops == &uvm_vm_ops_managed ||
           vma->vm_ops == &uvm_vm_ops_semaphore_pool;
}

static int uvm_mmap_entry(struct file *filp, struct vm_area_struct *vma)
{
   UVM_ENTRY_RET(uvm_mmap(filp, vma));
}

static NV_STATUS uvm_api_initialize(UVM_INITIALIZE_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space;
    NV_STATUS status;
    uvm_fd_type_t old_fd_type;

    // Normally we expect private_data == UVM_FD_UNINITIALIZED. However multiple
    // threads may call this ioctl concurrently so we have to be careful to
    // avoid initializing multiple va_spaces and/or leaking memory. To do this
    // we do an atomic compare and swap. Only one thread will observe
    // UVM_FD_UNINITIALIZED and that thread will allocate and setup the
    // va_space.
    //
    // Other threads will either see UVM_FD_INITIALIZING or UVM_FD_VA_SPACE. In
    // the case of UVM_FD_VA_SPACE we return success if and only if the
    // initialization flags match. If another thread is still initializing the
    // va_space we return NV_ERR_BUSY_RETRY.
    //
    // If va_space initialization fails we return the failure code and reset the
    // FD state back to UVM_FD_UNINITIALIZED to allow another initialization
    // attempt to be made. This is safe because other threads will have only had
    // a chance to observe UVM_FD_INITIALIZING and not UVM_FD_VA_SPACE in this
    // case.
    old_fd_type = nv_atomic_long_cmpxchg((atomic_long_t *)&filp->private_data,
                                         UVM_FD_UNINITIALIZED, UVM_FD_INITIALIZING);
    old_fd_type &= UVM_FD_TYPE_MASK;
    if (old_fd_type == UVM_FD_UNINITIALIZED) {
        status = uvm_va_space_create(filp->f_mapping, &va_space, params->flags);
        if (status != NV_OK) {
            atomic_long_set_release((atomic_long_t *)&filp->private_data, UVM_FD_UNINITIALIZED);
            return status;
        }

        atomic_long_set_release((atomic_long_t *)&filp->private_data, (long)va_space | UVM_FD_VA_SPACE);
    }
    else if (old_fd_type == UVM_FD_VA_SPACE) {
        va_space = uvm_va_space_get(filp);

        if (params->flags != va_space->initialization_flags)
            status = NV_ERR_INVALID_ARGUMENT;
        else
            status = NV_OK;
    }
    else if (old_fd_type == UVM_FD_MM) {
        status = NV_ERR_INVALID_ARGUMENT;
    }
    else {
        UVM_ASSERT(old_fd_type == UVM_FD_INITIALIZING);
        status = NV_ERR_BUSY_RETRY;
    }

    return status;
}

static NV_STATUS uvm_api_pageable_mem_access(UVM_PAGEABLE_MEM_ACCESS_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    params->pageableMemAccess = uvm_va_space_pageable_mem_access_supported(va_space) ? NV_TRUE : NV_FALSE;
    return NV_OK;
}

static long uvm_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    switch (cmd)
    {
        case UVM_DEINITIALIZE:
            return 0;

        UVM_ROUTE_CMD_STACK_NO_INIT_CHECK(UVM_INITIALIZE,                  uvm_api_initialize);
        UVM_ROUTE_CMD_STACK_NO_INIT_CHECK(UVM_MM_INITIALIZE,               uvm_api_mm_initialize);

        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_PAGEABLE_MEM_ACCESS,            uvm_api_pageable_mem_access);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_PAGEABLE_MEM_ACCESS_ON_GPU,     uvm_api_pageable_mem_access_on_gpu);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_REGISTER_GPU,                   uvm_api_register_gpu);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_UNREGISTER_GPU,                 uvm_api_unregister_gpu);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_CREATE_RANGE_GROUP,             uvm_api_create_range_group);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_DESTROY_RANGE_GROUP,            uvm_api_destroy_range_group);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_ENABLE_PEER_ACCESS,             uvm_api_enable_peer_access);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_DISABLE_PEER_ACCESS,            uvm_api_disable_peer_access);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_SET_RANGE_GROUP,                uvm_api_set_range_group);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_CREATE_EXTERNAL_RANGE,          uvm_api_create_external_range);
        UVM_ROUTE_CMD_ALLOC_INIT_CHECK(UVM_MAP_EXTERNAL_ALLOCATION,        uvm_api_map_external_allocation);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_MAP_EXTERNAL_SPARSE,            uvm_api_map_external_sparse);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_FREE,                           uvm_api_free);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_PREVENT_MIGRATION_RANGE_GROUPS, uvm_api_prevent_migration_range_groups);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_ALLOW_MIGRATION_RANGE_GROUPS,   uvm_api_allow_migration_range_groups);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_SET_PREFERRED_LOCATION,         uvm_api_set_preferred_location);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_UNSET_PREFERRED_LOCATION,       uvm_api_unset_preferred_location);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_SET_ACCESSED_BY,                uvm_api_set_accessed_by);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_UNSET_ACCESSED_BY,              uvm_api_unset_accessed_by);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_REGISTER_GPU_VASPACE,           uvm_api_register_gpu_va_space);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_UNREGISTER_GPU_VASPACE,         uvm_api_unregister_gpu_va_space);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_REGISTER_CHANNEL,               uvm_api_register_channel);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_UNREGISTER_CHANNEL,             uvm_api_unregister_channel);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_ENABLE_READ_DUPLICATION,        uvm_api_enable_read_duplication);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_DISABLE_READ_DUPLICATION,       uvm_api_disable_read_duplication);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_MIGRATE,                        uvm_api_migrate);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_ENABLE_SYSTEM_WIDE_ATOMICS,     uvm_api_enable_system_wide_atomics);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_DISABLE_SYSTEM_WIDE_ATOMICS,    uvm_api_disable_system_wide_atomics);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_TOOLS_READ_PROCESS_MEMORY,      uvm_api_tools_read_process_memory);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_TOOLS_WRITE_PROCESS_MEMORY,     uvm_api_tools_write_process_memory);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_TOOLS_GET_PROCESSOR_UUID_TABLE, uvm_api_tools_get_processor_uuid_table);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_MAP_DYNAMIC_PARALLELISM_REGION, uvm_api_map_dynamic_parallelism_region);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_UNMAP_EXTERNAL,                 uvm_api_unmap_external);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_MIGRATE_RANGE_GROUP,            uvm_api_migrate_range_group);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_TOOLS_FLUSH_EVENTS,             uvm_api_tools_flush_events);
        UVM_ROUTE_CMD_ALLOC_INIT_CHECK(UVM_ALLOC_SEMAPHORE_POOL,           uvm_api_alloc_semaphore_pool);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_CLEAN_UP_ZOMBIE_RESOURCES,      uvm_api_clean_up_zombie_resources);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_POPULATE_PAGEABLE,              uvm_api_populate_pageable);
        UVM_ROUTE_CMD_STACK_INIT_CHECK(UVM_VALIDATE_VA_RANGE,              uvm_api_validate_va_range);
    }

    // Try the test ioctls if none of the above matched
    return uvm_test_ioctl(filp, cmd, arg);
}

static long uvm_unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    long ret;

    if (!uvm_down_read_trylock(&g_uvm_global.pm.lock))
        return -EAGAIN;

    ret = uvm_ioctl(filp, cmd, arg);

    uvm_up_read(&g_uvm_global.pm.lock);

    uvm_thread_assert_all_unlocked();

    return ret;
}

static long uvm_unlocked_ioctl_entry(struct file *filp, unsigned int cmd, unsigned long arg)
{
   UVM_ENTRY_RET(uvm_unlocked_ioctl(filp, cmd, arg));
}

static const struct file_operations uvm_fops =
{
    .open            = uvm_open_entry,
    .release         = uvm_release_entry,
    .mmap            = uvm_mmap_entry,
    .unlocked_ioctl  = uvm_unlocked_ioctl_entry,
#if NVCPU_IS_X86_64
    .compat_ioctl    = uvm_unlocked_ioctl_entry,
#endif
    .owner           = THIS_MODULE,
};

NV_STATUS uvm_test_register_unload_state_buffer(UVM_TEST_REGISTER_UNLOAD_STATE_BUFFER_PARAMS *params, struct file *filp)
{
    long ret;
    struct page *page;
    NV_STATUS status = NV_OK;

    if (!IS_ALIGNED(params->unload_state_buf, sizeof(NvU64)))
        return NV_ERR_INVALID_ADDRESS;

    // Hold mmap_lock to call get_user_pages(), the UVM locking helper functions
    // are not used because unload_state_buf may be a managed memory pointer and
    // therefore a locking assertion from the CPU fault handler could be fired.
    nv_mmap_read_lock(current->mm);
    ret = NV_PIN_USER_PAGES(params->unload_state_buf, 1, FOLL_WRITE, &page);
    nv_mmap_read_unlock(current->mm);

    if (ret < 0)
        return errno_to_nv_status(ret);
    UVM_ASSERT(ret == 1);

    uvm_mutex_lock(&g_uvm_global.global_lock);

    if (g_uvm_global.unload_state.ptr) {
        NV_UNPIN_USER_PAGE(page);
        status = NV_ERR_IN_USE;
        goto error;
    }

    g_uvm_global.unload_state.page = page;
    g_uvm_global.unload_state.ptr = (NvU64 *)((char *)kmap(page) + (params->unload_state_buf & ~PAGE_MASK));
    *g_uvm_global.unload_state.ptr = 0;

error:
    uvm_mutex_unlock(&g_uvm_global.global_lock);

    return status;
}

static void uvm_test_unload_state_exit(void)
{
    if (g_uvm_global.unload_state.ptr) {
        kunmap(g_uvm_global.unload_state.page);
        NV_UNPIN_USER_PAGE(g_uvm_global.unload_state.page);
    }
}

static int uvm_chardev_create(void)
{
    dev_t uvm_dev;

    int ret = alloc_chrdev_region(&g_uvm_base_dev,
                                  0,
                                  NVIDIA_UVM_NUM_MINOR_DEVICES,
                                  NVIDIA_UVM_DEVICE_NAME);
    if (ret != 0) {
        UVM_ERR_PRINT("alloc_chrdev_region failed: %d\n", ret);
        return ret;
    }
    uvm_dev = MKDEV(MAJOR(g_uvm_base_dev), NVIDIA_UVM_PRIMARY_MINOR_NUMBER);

    uvm_init_character_device(&g_uvm_cdev, &uvm_fops);
    ret = cdev_add(&g_uvm_cdev, uvm_dev, 1);
    if (ret != 0) {
        UVM_ERR_PRINT("cdev_add (major %u, minor %u) failed: %d\n", MAJOR(uvm_dev), MINOR(uvm_dev), ret);
        unregister_chrdev_region(g_uvm_base_dev, NVIDIA_UVM_NUM_MINOR_DEVICES);
        return ret;
    }

    return 0;
}

static void uvm_chardev_exit(void)
{
    cdev_del(&g_uvm_cdev);
    unregister_chrdev_region(g_uvm_base_dev, NVIDIA_UVM_NUM_MINOR_DEVICES);
}

static int uvm_init(void)
{
    bool initialized_globals = false;
    bool added_device = false;
    int ret;

    NV_STATUS status = uvm_global_init();
    if (status != NV_OK) {
        UVM_ERR_PRINT("uvm_global_init() failed: %s\n", nvstatusToString(status));
        ret = -ENODEV;
        goto error;
    }
    initialized_globals = true;

    ret = uvm_chardev_create();
    if (ret != 0) {
        UVM_ERR_PRINT("uvm_chardev_create failed: %d\n", ret);
        goto error;
    }
    added_device = true;

    ret = uvm_tools_init(g_uvm_base_dev);
    if (ret != 0) {
        UVM_ERR_PRINT("uvm_tools_init() failed: %d\n", ret);
        goto error;
    }

    pr_info("Loaded the UVM driver, major device number %d.\n", MAJOR(g_uvm_base_dev));

    if (uvm_enable_builtin_tests)
        pr_info("Built-in UVM tests are enabled. This is a security risk.\n");

    // After Open RM is released, both the enclosing "#if" and this comment
    // block should be removed, because the uvm_hmm_is_enabled_system_wide()
    // check is both necessary and sufficient for reporting functionality.
    // Until that time, however, we need to avoid advertisting UVM's ability to
    // enable HMM functionality.

    if (uvm_hmm_is_enabled_system_wide())
        UVM_INFO_PRINT("HMM (Heterogeneous Memory Management) is enabled in the UVM driver.\n");

    return 0;

error:
    if (added_device)
        uvm_chardev_exit();

    if (initialized_globals)
        uvm_global_exit();

    UVM_ERR_PRINT("uvm init failed: %d\n", ret);

    return ret;
}

static int __init uvm_init_entry(void)
{
   UVM_ENTRY_RET(uvm_init());
}

static void uvm_exit(void)
{
    uvm_tools_exit();
    uvm_chardev_exit();

    uvm_global_exit();

    uvm_test_unload_state_exit();

    pr_info("Unloaded the UVM driver.\n");
}

static void __exit uvm_exit_entry(void)
{
   UVM_ENTRY_VOID(uvm_exit());
}

module_init(uvm_init_entry);
module_exit(uvm_exit_entry);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_INFO(supported, "external");
MODULE_VERSION(NV_VERSION_STRING);
