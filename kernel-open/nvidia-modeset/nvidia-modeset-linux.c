/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/vmalloc.h>
#include <asm/div64.h> /* do_div() */
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/random.h>
#include <linux/file.h>
#include <linux/list.h>
#include <linux/rwsem.h>
#include <linux/freezer.h>
#include <linux/poll.h>
#include <linux/cdev.h>

#include <acpi/video.h>

#include "nvstatus.h"

#include "nv-modeset-interface.h"
#include "nv-kref.h"

#include "nvidia-modeset-os-interface.h"
#include "nvkms.h"
#include "nvkms-ioctl.h"

#include "conftest.h"
#include "nv-procfs.h"
#include "nv-kthread-q.h"
#include "nv-time.h"
#include "nv-timer.h"
#include "nv-lock.h"
#include "nv-chardev-numbers.h"

/*
 * Commit aefb2f2e619b ("x86/bugs: Rename CONFIG_RETPOLINE =>
 * CONFIG_MITIGATION_RETPOLINE) in v6.8 renamed CONFIG_RETPOLINE.
 */
#if !defined(CONFIG_RETPOLINE) && !defined(CONFIG_MITIGATION_RETPOLINE)
#include "nv-retpoline.h"
#endif

#include <linux/backlight.h>

#define NVKMS_LOG_PREFIX "nvidia-modeset: "

static bool output_rounding_fix = true;
module_param_named(output_rounding_fix, output_rounding_fix, bool, 0400);

static bool disable_hdmi_frl = false;
module_param_named(disable_hdmi_frl, disable_hdmi_frl, bool, 0400);

static bool disable_vrr_memclk_switch = false;
module_param_named(disable_vrr_memclk_switch, disable_vrr_memclk_switch, bool, 0400);

static bool hdmi_deepcolor = true;
module_param_named(hdmi_deepcolor, hdmi_deepcolor, bool, 0400);

static bool vblank_sem_control = true;
module_param_named(vblank_sem_control, vblank_sem_control, bool, 0400);

static bool opportunistic_display_sync = true;
module_param_named(opportunistic_display_sync, opportunistic_display_sync, bool, 0400);

static enum NvKmsDebugForceColorSpace debug_force_color_space = NVKMS_DEBUG_FORCE_COLOR_SPACE_NONE;
module_param_named(debug_force_color_space, debug_force_color_space, uint, 0400);

static bool enable_overlay_layers = true;
module_param_named(enable_overlay_layers, enable_overlay_layers, bool, 0400);

/* These parameters are used for fault injection tests.  Normally the defaults
 * should be used. */
MODULE_PARM_DESC(fail_malloc, "Fail the Nth call to nvkms_alloc");
static int fail_malloc_num = -1;
module_param_named(fail_malloc, fail_malloc_num, int, 0400);

MODULE_PARM_DESC(malloc_verbose, "Report information about malloc calls on module unload");
static bool malloc_verbose = false;
module_param_named(malloc_verbose, malloc_verbose, bool, 0400);

MODULE_PARM_DESC(conceal_vrr_caps, 
                 "Conceal all display VRR capabilities");
static bool conceal_vrr_caps = false;
module_param_named(conceal_vrr_caps, conceal_vrr_caps, bool, 0400);

/* Fail allocating the RM core channel for NVKMS using the i-th method (see
 * FailAllocCoreChannelMethod). Failures not using the i-th method are ignored. */
MODULE_PARM_DESC(fail_alloc_core_channel, "Control testing for hardware core channel allocation failure");
static int fail_alloc_core_channel_method = -1;
module_param_named(fail_alloc_core_channel, fail_alloc_core_channel_method, int, 0400);

#if NVKMS_CONFIG_FILE_SUPPORTED
/* This parameter is used to find the dpy override conf file */
#define NVKMS_CONF_FILE_SPECIFIED (nvkms_conf != NULL)

MODULE_PARM_DESC(config_file,
                 "Path to the nvidia-modeset configuration file (default: disabled)");
static char *nvkms_conf = NULL;
module_param_named(config_file, nvkms_conf, charp, 0400);
#endif

static atomic_t nvkms_alloc_called_count;

NvBool nvkms_test_fail_alloc_core_channel(
    enum FailAllocCoreChannelMethod method
)
{
    if (method != fail_alloc_core_channel_method) {
        // don't fail if it's not the currently specified method
        return NV_FALSE;
    } 

    printk(KERN_INFO NVKMS_LOG_PREFIX 
        "Failing core channel allocation using method %d", 
        fail_alloc_core_channel_method);    

    return NV_TRUE;
}

NvBool nvkms_conceal_vrr_caps(void)
{
    return conceal_vrr_caps;
}

NvBool nvkms_output_rounding_fix(void)
{
    return output_rounding_fix;
}

NvBool nvkms_disable_hdmi_frl(void)
{
    return disable_hdmi_frl;
}

NvBool nvkms_disable_vrr_memclk_switch(void)
{
    return disable_vrr_memclk_switch;
}

NvBool nvkms_hdmi_deepcolor(void)
{
    return hdmi_deepcolor;
}

NvBool nvkms_vblank_sem_control(void)
{
    return vblank_sem_control;
}

NvBool nvkms_opportunistic_display_sync(void)
{
    return opportunistic_display_sync;
}

enum NvKmsDebugForceColorSpace nvkms_debug_force_color_space(void)
{
    if (debug_force_color_space >= NVKMS_DEBUG_FORCE_COLOR_SPACE_MAX) {
        return NVKMS_DEBUG_FORCE_COLOR_SPACE_NONE;
    }
    return debug_force_color_space;
}

NvBool nvkms_enable_overlay_layers(void)
{
    return enable_overlay_layers;
}

NvBool nvkms_kernel_supports_syncpts(void)
{
/*
 * Note this only checks that the kernel has the prerequisite
 * support for syncpts; callers must also check that the hardware
 * supports syncpts.
 */
#if (defined(CONFIG_TEGRA_GRHOST) || defined(NV_LINUX_HOST1X_NEXT_H_PRESENT))
    return NV_TRUE;
#else
    return NV_FALSE;
#endif
}

#define NVKMS_SYNCPT_STUBS_NEEDED

/*************************************************************************
 * NVKMS interface for nvhost unit for sync point APIs.
 *************************************************************************/

#ifdef NVKMS_SYNCPT_STUBS_NEEDED
/* Unsupported STUB for nvkms_syncpt APIs */
NvBool nvkms_syncpt_op(
    enum NvKmsSyncPtOp op,
    NvKmsSyncPtOpParams *params)
{
    return NV_FALSE;
}
#endif

#define NVKMS_MAJOR_DEVICE_NUMBER 195
#define NVKMS_MINOR_DEVICE_NUMBER 254

/*
 * Convert from microseconds to jiffies.  The conversion is:
 * ((usec) * HZ / 1000000)
 *
 * Use do_div() to avoid gcc-generated references to __udivdi3().
 * Note that the do_div() macro divides the first argument in place.
 */
static inline unsigned long NVKMS_USECS_TO_JIFFIES(NvU64 usec)
{
    unsigned long result = usec * HZ;
    do_div(result, 1000000);
    return result;
}


/*************************************************************************
 * NVKMS uses a global lock, nvkms_lock.  The lock is taken in the
 * file operation callback functions when calling into core NVKMS.
 *************************************************************************/

static struct semaphore nvkms_lock;

/*************************************************************************
 * User clients of NVKMS may need to be synchronized with suspend/resume
 * operations.  This depends on the state of the system when the NVKMS
 * suspend/resume callbacks are invoked.  NVKMS uses a single
 * RW lock, nvkms_pm_lock, for this synchronization.
 *************************************************************************/

static struct rw_semaphore nvkms_pm_lock;

/*************************************************************************
 * NVKMS executes almost all of its queued work items on a single
 * kthread.  The exception are deferred close() handlers, which typically
 * block for long periods of time and stall their queue.
 *************************************************************************/

static struct nv_kthread_q nvkms_kthread_q;
static struct nv_kthread_q nvkms_deferred_close_kthread_q;

/*************************************************************************
 * The nvkms_per_open structure tracks data that is specific to a
 * single open.
 *************************************************************************/

struct nvkms_per_open {
    void *data;

    enum NvKmsClientType type;

    union {
        struct {
            struct {
                atomic_t available;
                wait_queue_head_t wait_queue;
            } events;
        } user;

        struct {
            struct {
                nv_kthread_q_item_t nv_kthread_q_item;
            } events;
        } kernel;
    } u;

    nv_kthread_q_item_t deferred_close_q_item;
};

/*************************************************************************
 * nvkms_pm_lock helper functions.  Since no down_read_interruptible()
 * or equivalent interface is available, it needs to be approximated with
 * down_read_trylock() to enable the kernel's freezer to round up user
 * threads going into suspend.
 *************************************************************************/

static inline int nvkms_read_trylock_pm_lock(void)
{
    return !down_read_trylock(&nvkms_pm_lock);
}

static inline void nvkms_read_lock_pm_lock(void)
{
    if ((current->flags & PF_NOFREEZE)) {
        /*
         * Non-freezable tasks (i.e. kthreads in this case) don't have to worry
         * about being frozen during system suspend, but do need to block so
         * that the CPU can go idle during s2idle. Do a normal uninterruptible
         * blocking wait for the PM lock.
         */
        down_read(&nvkms_pm_lock);
    } else {
        /*
         * For freezable tasks, make sure we give the kernel an opportunity to
         * freeze if taking the PM lock fails.
         */
        while (!down_read_trylock(&nvkms_pm_lock)) {
            try_to_freeze();
            cond_resched();
        }
    }
}

static inline void nvkms_read_unlock_pm_lock(void)
{
    up_read(&nvkms_pm_lock);
}

static inline void nvkms_write_lock_pm_lock(void)
{
    down_write(&nvkms_pm_lock);
}

static inline void nvkms_write_unlock_pm_lock(void)
{
    up_write(&nvkms_pm_lock);
}

/*************************************************************************
 * nvidia-modeset-os-interface.h functions.  It is assumed that these
 * are called while nvkms_lock is held.
 *************************************************************************/

/* Don't use kmalloc for allocations larger than one page */
#define KMALLOC_LIMIT PAGE_SIZE

void* nvkms_alloc(size_t size, NvBool zero)
{
    void *p;

    if (malloc_verbose || fail_malloc_num >= 0) {
        int this_alloc = atomic_inc_return(&nvkms_alloc_called_count) - 1;
        if (fail_malloc_num >= 0 && fail_malloc_num == this_alloc) {
            printk(KERN_WARNING NVKMS_LOG_PREFIX "Failing alloc %d\n",
                   fail_malloc_num);
            return NULL;
        }
    }

    if (size <= KMALLOC_LIMIT) {
        p = kmalloc(size, GFP_KERNEL);
    } else {
        p = vmalloc(size);
    }

    if (zero && (p != NULL)) {
        memset(p, 0, size);
    }

    return p;
}

void nvkms_free(void *ptr, size_t size)
{
    if (size <= KMALLOC_LIMIT) {
        kfree(ptr);
    } else {
        vfree(ptr);
    }
}

void* nvkms_memset(void *ptr, NvU8 c, size_t size)
{
    return memset(ptr, c, size);
}

void* nvkms_memcpy(void *dest, const void *src, size_t n)
{
    return memcpy(dest, src, n);
}

void* nvkms_memmove(void *dest, const void *src, size_t n)
{
    return memmove(dest, src, n);
}

int nvkms_memcmp(const void *s1, const void *s2, size_t n)
{
    return memcmp(s1, s2, n);
}

size_t nvkms_strlen(const char *s)
{
    return strlen(s);
}

int nvkms_strcmp(const char *s1, const char *s2)
{
    return strcmp(s1, s2);
}

char* nvkms_strncpy(char *dest, const char *src, size_t n)
{
    return strncpy(dest, src, n);
}

void nvkms_usleep(NvU64 usec)
{
    if (usec < 1000) {
        /*
         * If the period to wait is less than one millisecond, sleep
         * using udelay(); note this is a busy wait.
         */
        udelay(usec);
    } else {
        /*
         * Otherwise, sleep with millisecond precision.  Clamp the
         * time to ~4 seconds (0xFFF/1000 => 4.09 seconds).
         *
         * Note that the do_div() macro divides the first argument in
         * place.
         */

        int msec;
        NvU64 tmp = usec + 500;
        do_div(tmp, 1000);
        msec = (int) (tmp & 0xFFF);

        /*
         * XXX NVKMS TODO: this may need to be msleep_interruptible(),
         * though the callers would need to be made to handle
         * returning early.
         */
        msleep(msec);
    }
}

NvU64 nvkms_get_usec(void)
{
    struct timespec64 ts;
    NvU64 ns;

    ktime_get_raw_ts64(&ts);

    ns = timespec64_to_ns(&ts);
    return ns / 1000;
}

int nvkms_copyin(void *kptr, NvU64 uaddr, size_t n)
{
    if (!nvKmsNvU64AddressIsSafe(uaddr)) {
        return -EINVAL;
    }

    if (copy_from_user(kptr, nvKmsNvU64ToPointer(uaddr), n) != 0) {
        return -EFAULT;
    }

    return 0;
}

int nvkms_copyout(NvU64 uaddr, const void *kptr, size_t n)
{
    if (!nvKmsNvU64AddressIsSafe(uaddr)) {
        return -EINVAL;
    }

    if (copy_to_user(nvKmsNvU64ToPointer(uaddr), kptr, n) != 0) {
        return -EFAULT;
    }

    return 0;
}

void nvkms_yield(void)
{
    schedule();
}

void nvkms_dump_stack(void)
{
    dump_stack();
}

int nvkms_snprintf(char *str, size_t size, const char *format, ...)
{
    int ret;
    va_list ap;

    va_start(ap, format);
    ret = vsnprintf(str, size, format, ap);
    va_end(ap);

    return ret;
}

int nvkms_vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
    return vsnprintf(str, size, format, ap);
}

void nvkms_log(const int level, const char *gpuPrefix, const char *msg)
{
    const char *levelString;
    const char *levelPrefix;

    switch (level) {
    default:
    case NVKMS_LOG_LEVEL_INFO:
        levelPrefix = "";
        levelString = KERN_INFO;
        break;
    case NVKMS_LOG_LEVEL_WARN:
        levelPrefix = "WARNING: ";
        levelString = KERN_WARNING;
        break;
    case NVKMS_LOG_LEVEL_ERROR:
        levelPrefix = "ERROR: ";
        levelString = KERN_ERR;
        break;
    }

    printk("%s%s%s%s%s\n",
           levelString, NVKMS_LOG_PREFIX, levelPrefix, gpuPrefix, msg);
}

void
nvkms_event_queue_changed(nvkms_per_open_handle_t *pOpenKernel,
                          NvBool eventsAvailable)
{
    struct nvkms_per_open *popen = pOpenKernel;

    switch (popen->type) {
        case NVKMS_CLIENT_USER_SPACE:
            /*
             * Write popen->events.available atomically, to avoid any races or
             * memory barrier issues interacting with nvkms_poll().
             */
            atomic_set(&popen->u.user.events.available, eventsAvailable);

            wake_up_interruptible(&popen->u.user.events.wait_queue);

            break;
        case NVKMS_CLIENT_KERNEL_SPACE:
            if (eventsAvailable) {
                nv_kthread_q_schedule_q_item(
                    &nvkms_kthread_q,
                    &popen->u.kernel.events.nv_kthread_q_item);
            }

            break;
    }
}

static void nvkms_suspend(NvU32 gpuId)
{
    nvKmsKapiSuspendResume(NV_TRUE /* suspend */);

    if (gpuId == 0) {
        nvkms_write_lock_pm_lock();
    }

    down(&nvkms_lock);
    nvKmsSuspend(gpuId);
    up(&nvkms_lock);
}

static void nvkms_resume(NvU32 gpuId)
{
    down(&nvkms_lock);
    nvKmsResume(gpuId);
    up(&nvkms_lock);

    if (gpuId == 0) {
        nvkms_write_unlock_pm_lock();
    }

    nvKmsKapiSuspendResume(NV_FALSE /* suspend */);
}


/*************************************************************************
 * Interface with resman.
 *************************************************************************/

static nvidia_modeset_rm_ops_t __rm_ops = { 0 };
static nvidia_modeset_callbacks_t nvkms_rm_callbacks = {
    .suspend = nvkms_suspend,
    .resume  = nvkms_resume
};

static int nvkms_alloc_rm(void)
{
    NV_STATUS nvstatus;
    int ret;

    __rm_ops.version_string = NV_VERSION_STRING;

    nvstatus = nvidia_get_rm_ops(&__rm_ops);

    if (nvstatus != NV_OK) {
        printk(KERN_ERR NVKMS_LOG_PREFIX "Version mismatch: "
               "nvidia.ko(%s) nvidia-modeset.ko(%s)\n",
               __rm_ops.version_string, NV_VERSION_STRING);
        return -EINVAL;
    }

    ret = __rm_ops.set_callbacks(&nvkms_rm_callbacks);
    if (ret < 0) {
        printk(KERN_ERR NVKMS_LOG_PREFIX "Failed to register callbacks\n");
        return ret;
    }

    return 0;
}

static void nvkms_free_rm(void)
{
    __rm_ops.set_callbacks(NULL);
}

void nvkms_call_rm(void *ops)
{
    nvidia_modeset_stack_ptr stack = NULL;

    if (__rm_ops.alloc_stack(&stack) != 0) {
        return;
    }

    __rm_ops.op(stack, ops);

    __rm_ops.free_stack(stack);
}

/*************************************************************************
 * ref_ptr implementation.
 *************************************************************************/

struct nvkms_ref_ptr {
    nv_kref_t refcnt;
    // Access to ptr is guarded by the nvkms_lock.
    void *ptr;
};

struct nvkms_ref_ptr* nvkms_alloc_ref_ptr(void *ptr)
{
    struct nvkms_ref_ptr *ref_ptr = nvkms_alloc(sizeof(*ref_ptr), NV_FALSE);
    if (ref_ptr) {
        // The ref_ptr owner counts as a reference on the ref_ptr itself.
        nv_kref_init(&ref_ptr->refcnt);
        ref_ptr->ptr = ptr;
    }
    return ref_ptr;
}

void nvkms_free_ref_ptr(struct nvkms_ref_ptr *ref_ptr)
{
    if (ref_ptr) {
        ref_ptr->ptr = NULL;
        // Release the owner's reference of the ref_ptr.
        nvkms_dec_ref(ref_ptr);
    }
}

void nvkms_inc_ref(struct nvkms_ref_ptr *ref_ptr)
{
    nv_kref_get(&ref_ptr->refcnt);
}

static void ref_ptr_free(nv_kref_t *ref)
{
    struct nvkms_ref_ptr *ref_ptr = container_of(ref, struct nvkms_ref_ptr,
                                                 refcnt);
    nvkms_free(ref_ptr, sizeof(*ref_ptr));
}

void* nvkms_dec_ref(struct nvkms_ref_ptr *ref_ptr)
{
    void *ptr = ref_ptr->ptr;
    nv_kref_put(&ref_ptr->refcnt, ref_ptr_free);
    return ptr;
}

/*************************************************************************
 * Timer support
 *
 * Core NVKMS needs to be able to schedule work to execute in the
 * future, within process context.
 *
 * To achieve this, use struct timer_list to schedule a timer
 * callback, nvkms_timer_callback().  This will execute in softirq
 * context, so from there schedule an nv_kthread_q item,
 * nvkms_kthread_q_callback(), which will execute in process context.
 *************************************************************************/

struct nvkms_timer_t {
    nv_kthread_q_item_t nv_kthread_q_item;
    struct timer_list kernel_timer;
    NvBool cancel;
    NvBool complete;
    NvBool isRefPtr;
    NvBool kernel_timer_created;
    nvkms_timer_proc_t *proc;
    void *dataPtr;
    NvU32 dataU32;
    struct list_head timers_list;
};

/*
 * Global list with pending timers, any change requires acquiring lock
 */
static struct {
    spinlock_t lock;
    struct list_head list;
} nvkms_timers;

static void nvkms_kthread_q_callback(void *arg)
{
    struct nvkms_timer_t *timer = arg;
    void *dataPtr;
    unsigned long flags = 0;

    /*
     * We can delete this timer from pending timers list - it's being
     * processed now.
     */
    spin_lock_irqsave(&nvkms_timers.lock, flags);
    list_del(&timer->timers_list);
    spin_unlock_irqrestore(&nvkms_timers.lock, flags);

    /*
     * After kthread_q_callback we want to be sure that timer_callback
     * for this timer also have finished. It's important during module
     * unload - this way we can safely unload this module by first deleting
     * pending timers and than waiting for workqueue callbacks.
     */
    if (timer->kernel_timer_created) {
        nv_timer_delete_sync(&timer->kernel_timer);
    }

    /*
     * Block the kthread during system suspend & resume in order to defer
     * handling of events such as DP_IRQ and hotplugs until after resume.
     */
    nvkms_read_lock_pm_lock();

    down(&nvkms_lock);

    if (timer->isRefPtr) {
        // If the object this timer refers to was destroyed, treat the timer as
        // canceled.
        dataPtr = nvkms_dec_ref(timer->dataPtr);
        if (!dataPtr) {
            timer->cancel = NV_TRUE;
        }
    } else {
        dataPtr = timer->dataPtr;
    }

    if (!timer->cancel) {
        timer->proc(dataPtr, timer->dataU32);
        timer->complete = NV_TRUE;
    }

    if (timer->isRefPtr) {
        // ref_ptr-based timers are allocated with kmalloc(GFP_ATOMIC).
        kfree(timer);
    } else if (timer->cancel) {
        nvkms_free(timer, sizeof(*timer));
    }

    up(&nvkms_lock);

    nvkms_read_unlock_pm_lock();
}

static void nvkms_queue_work(nv_kthread_q_t *q, nv_kthread_q_item_t *q_item)
{
    int ret = nv_kthread_q_schedule_q_item(q, q_item);
    /*
     * nv_kthread_q_schedule_q_item should only fail (which it indicates by
     * returning false) if the item is already scheduled or the queue is
     * stopped. Neither of those should happen in NVKMS.
     */
    WARN_ON(!ret);
}

static void _nvkms_timer_callback_internal(struct nvkms_timer_t *nvkms_timer)
{
    /* In softirq context, so schedule nvkms_kthread_q_callback(). */
    nvkms_queue_work(&nvkms_kthread_q, &nvkms_timer->nv_kthread_q_item);
}

/*
 * Why the "inline" keyword? Because only one of these next two functions will
 * be used, thus leading to a "defined but not used function" warning. The
 * "inline" keyword is redefined in the Kbuild system
 * (see: <kernel>/include/linux/compiler-gcc.h) so as to suppress that warning.
 */
inline static void nvkms_timer_callback_typed_data(struct timer_list *timer)
{
    struct nvkms_timer_t *nvkms_timer =
            container_of(timer, struct nvkms_timer_t, kernel_timer);

    _nvkms_timer_callback_internal(nvkms_timer);
}

inline static void nvkms_timer_callback_anon_data(unsigned long arg)
{
    struct nvkms_timer_t *nvkms_timer = (struct nvkms_timer_t *) arg;
    _nvkms_timer_callback_internal(nvkms_timer);
}

static void
nvkms_init_timer(struct nvkms_timer_t *timer, nvkms_timer_proc_t *proc,
                 void *dataPtr, NvU32 dataU32, NvBool isRefPtr, NvU64 usec)
{
    unsigned long flags = 0;

    memset(timer, 0, sizeof(*timer));
    timer->cancel = NV_FALSE;
    timer->complete = NV_FALSE;
    timer->isRefPtr = isRefPtr;

    timer->proc = proc;
    timer->dataPtr = dataPtr;
    timer->dataU32 = dataU32;

    nv_kthread_q_item_init(&timer->nv_kthread_q_item, nvkms_kthread_q_callback,
                           timer);

    /*
     * After adding timer to timers_list we need to finish referencing it
     * (calling nvkms_queue_work() or mod_timer()) before releasing the lock.
     * Otherwise, if the code to free the timer were ever updated to
     * run in parallel with this, it could race against nvkms_init_timer()
     * and free the timer before its initialization is complete.
     */
    spin_lock_irqsave(&nvkms_timers.lock, flags);
    list_add(&timer->timers_list, &nvkms_timers.list);

    if (usec == 0) {
        timer->kernel_timer_created = NV_FALSE;
        nvkms_queue_work(&nvkms_kthread_q, &timer->nv_kthread_q_item);
    } else {
#if defined(NV_TIMER_SETUP_PRESENT)
        timer_setup(&timer->kernel_timer, nvkms_timer_callback_typed_data, 0);
#else
        init_timer(&timer->kernel_timer);
        timer->kernel_timer.function = nvkms_timer_callback_anon_data;
        timer->kernel_timer.data = (unsigned long) timer;
#endif

        timer->kernel_timer_created = NV_TRUE;
        mod_timer(&timer->kernel_timer, jiffies + NVKMS_USECS_TO_JIFFIES(usec));
    }
    spin_unlock_irqrestore(&nvkms_timers.lock, flags);
}

nvkms_timer_handle_t*
nvkms_alloc_timer(nvkms_timer_proc_t *proc,
                  void *dataPtr, NvU32 dataU32,
                  NvU64 usec)
{
    // nvkms_alloc_timer cannot be called from an interrupt context.
    struct nvkms_timer_t *timer = nvkms_alloc(sizeof(*timer), NV_FALSE);
    if (timer) {
        nvkms_init_timer(timer, proc, dataPtr, dataU32, NV_FALSE, usec);
    }
    return timer;
}

NvBool
nvkms_alloc_timer_with_ref_ptr(nvkms_timer_proc_t *proc,
                               struct nvkms_ref_ptr *ref_ptr,
                               NvU32 dataU32, NvU64 usec)
{
    // nvkms_alloc_timer_with_ref_ptr is called from an interrupt bottom half
    // handler, which runs in a tasklet (i.e. atomic) context.
    struct nvkms_timer_t *timer = kmalloc(sizeof(*timer), GFP_ATOMIC);
    if (timer) {
        // Reference the ref_ptr to make sure that it doesn't get freed before
        // the timer fires.
        nvkms_inc_ref(ref_ptr);
        nvkms_init_timer(timer, proc, ref_ptr, dataU32, NV_TRUE, usec);
    }

    return timer != NULL;
}

void nvkms_free_timer(nvkms_timer_handle_t *handle)
{
    struct nvkms_timer_t *timer = handle;

    if (timer == NULL) {
        return;
    }

    if (timer->complete) {
        nvkms_free(timer, sizeof(*timer));
        return;
    }

    timer->cancel = NV_TRUE;
}

NvBool nvkms_fd_is_nvidia_chardev(int fd)
{
    struct file *filp = fget(fd);
    dev_t rdev = 0;
    NvBool ret = NV_FALSE;

    if (filp == NULL) {
        return ret;
    }

    if (filp->f_inode == NULL) {
        goto done;
    }
    rdev = filp->f_inode->i_rdev;

    if (MAJOR(rdev) == NVKMS_MAJOR_DEVICE_NUMBER) {
        ret = NV_TRUE;
    }

done:
    fput(filp);

    return ret;
}

NvBool nvkms_open_gpu(NvU32 gpuId)
{
    nvidia_modeset_stack_ptr stack = NULL;
    NvBool ret;

    if (__rm_ops.alloc_stack(&stack) != 0) {
        return NV_FALSE;
    }

    ret = __rm_ops.open_gpu(gpuId, stack) == 0;

    __rm_ops.free_stack(stack);

    return ret;
}

void nvkms_close_gpu(NvU32 gpuId)
{
    nvidia_modeset_stack_ptr stack = NULL;

    if (__rm_ops.alloc_stack(&stack) != 0) {
        return;
    }

    __rm_ops.close_gpu(gpuId, stack);

    __rm_ops.free_stack(stack);
}

NvU32 nvkms_enumerate_gpus(nv_gpu_info_t *gpu_info)
{
    return __rm_ops.enumerate_gpus(gpu_info);
}

NvBool nvkms_allow_write_combining(void)
{
    return __rm_ops.system_info.allow_write_combining;
}

#if IS_ENABLED(CONFIG_BACKLIGHT_CLASS_DEVICE)
/*************************************************************************
 * Implementation of sysfs interface to control backlight
 *************************************************************************/

struct nvkms_backlight_device {
    NvU32 gpu_id;
    NvU32 display_id;

    void *drv_priv;

    struct backlight_device * dev;
};

static int nvkms_update_backlight_status(struct backlight_device *bd)
{
    struct nvkms_backlight_device *nvkms_bd = bl_get_data(bd);
    NvBool status;
    int ret;

    ret = down_interruptible(&nvkms_lock);

    if (ret != 0) {
        return ret;
    }

    status = nvKmsSetBacklight(nvkms_bd->display_id, nvkms_bd->drv_priv,
                               bd->props.brightness);

    up(&nvkms_lock);

    return status ? 0 : -EINVAL;
}

static int nvkms_get_backlight_brightness(struct backlight_device *bd)
{
    struct nvkms_backlight_device *nvkms_bd = bl_get_data(bd);
    NvU32 brightness = 0;
    NvBool status;
    int ret;

    ret = down_interruptible(&nvkms_lock);

    if (ret != 0) {
        return ret;
    }

    status = nvKmsGetBacklight(nvkms_bd->display_id, nvkms_bd->drv_priv,
                               &brightness);

    up(&nvkms_lock);

    return  status ? brightness : -1;
}

static const struct backlight_ops nvkms_backlight_ops = {
    .update_status = nvkms_update_backlight_status,
    .get_brightness = nvkms_get_backlight_brightness,
};
#endif /* IS_ENABLED(CONFIG_BACKLIGHT_CLASS_DEVICE) */

struct nvkms_backlight_device*
nvkms_register_backlight(NvU32 gpu_id, NvU32 display_id, void *drv_priv,
                         NvU32 current_brightness)
{
#if IS_ENABLED(CONFIG_BACKLIGHT_CLASS_DEVICE)
    char name[18];
    struct backlight_properties props = {
        .brightness = current_brightness,
        .max_brightness = 100,
        .type = BACKLIGHT_RAW,
    };
    nv_gpu_info_t *gpu_info = NULL;
    NvU32 gpu_count = 0;
    struct nvkms_backlight_device *nvkms_bd = NULL;
    int i;

#if defined(NV_ACPI_VIDEO_BACKLIGHT_USE_NATIVE)
    if (!acpi_video_backlight_use_native()) {
#if defined(NV_ACPI_VIDEO_REGISTER_BACKLIGHT)
        nvkms_log(NVKMS_LOG_LEVEL_INFO, NVKMS_LOG_PREFIX,
                  "ACPI reported no NVIDIA native backlight available; attempting to use ACPI backlight.");
        acpi_video_register_backlight();
#endif
        return NULL;
    }
#endif

    gpu_info = nvkms_alloc(NV_MAX_GPUS * sizeof(*gpu_info), NV_TRUE);
    if (gpu_info == NULL) {
        return NULL;
    }

    gpu_count = __rm_ops.enumerate_gpus(gpu_info);
    if (gpu_count == 0) {
        goto done;
    }

    for (i = 0; i < gpu_count; i++) {
        if (gpu_info[i].gpu_id == gpu_id) {
            break;
        }
    }

    if (i == gpu_count) {
        goto done;
    }

    nvkms_bd = nvkms_alloc(sizeof(*nvkms_bd), NV_TRUE);
    if (nvkms_bd == NULL) {
        goto done;
    }

    snprintf(name, sizeof(name), "nvidia_%d", i);
    name[sizeof(name) - 1] = '\0';

    nvkms_bd->gpu_id = gpu_id;
    nvkms_bd->display_id = display_id;
    nvkms_bd->drv_priv = drv_priv;

    nvkms_bd->dev =
        backlight_device_register(name,
                                  gpu_info[i].os_device_ptr,
                                  nvkms_bd,
                                  &nvkms_backlight_ops,
                                  &props);

done:
    nvkms_free(gpu_info, NV_MAX_GPUS * sizeof(*gpu_info));

    return nvkms_bd;
#else
    return NULL;
#endif /* IS_ENABLED(CONFIG_BACKLIGHT_CLASS_DEVICE) */
}

void nvkms_unregister_backlight(struct nvkms_backlight_device *nvkms_bd)
{
#if IS_ENABLED(CONFIG_BACKLIGHT_CLASS_DEVICE)
    if (nvkms_bd->dev) {
        backlight_device_unregister(nvkms_bd->dev);
    }

    nvkms_free(nvkms_bd, sizeof(*nvkms_bd));
#endif /* IS_ENABLED(CONFIG_BACKLIGHT_CLASS_DEVICE) */
}

/*************************************************************************
 * Common to both user-space and kapi NVKMS interfaces
 *************************************************************************/

static void nvkms_kapi_event_kthread_q_callback(void *arg)
{
    struct NvKmsKapiDevice *device = arg;

    nvKmsKapiHandleEventQueueChange(device);
}

static struct nvkms_per_open *nvkms_open_common(enum NvKmsClientType type,
                                         struct NvKmsKapiDevice *device,
                                         int *status)
{
    struct nvkms_per_open *popen = NULL;

    popen = nvkms_alloc(sizeof(*popen), NV_TRUE);

    if (popen == NULL) {
        *status = -ENOMEM;
        goto failed;
    }

    popen->type = type;

    *status = down_interruptible(&nvkms_lock);

    if (*status != 0) {
        goto failed;
    }

    popen->data = nvKmsOpen(current->tgid, type, popen);

    up(&nvkms_lock);

    if (popen->data == NULL) {
        *status = -EPERM;
        goto failed;
    }

    switch (popen->type) {
        case NVKMS_CLIENT_USER_SPACE:
            init_waitqueue_head(&popen->u.user.events.wait_queue);
            break;
        case NVKMS_CLIENT_KERNEL_SPACE:
            nv_kthread_q_item_init(&popen->u.kernel.events.nv_kthread_q_item,
                                   nvkms_kapi_event_kthread_q_callback,
                                   device);
            break;
    }

    *status = 0;

    return popen;

failed:

    nvkms_free(popen, sizeof(*popen));

    return NULL;
}

static void nvkms_close_pm_locked(struct nvkms_per_open *popen)
{
    /*
     * Don't use down_interruptible(): we need to free resources
     * during close, so we have no choice but to wait to take the
     * mutex.
     */

    down(&nvkms_lock);

    nvKmsClose(popen->data);

    popen->data = NULL;

    up(&nvkms_lock);

    if (popen->type == NVKMS_CLIENT_KERNEL_SPACE) {
        /*
         * Flush any outstanding nvkms_kapi_event_kthread_q_callback() work
         * items before freeing popen.
         *
         * Note that this must be done after the above nvKmsClose() call, to
         * guarantee that no more nvkms_kapi_event_kthread_q_callback() work
         * items get scheduled.
         *
         * Also, note that though popen->data is freed above, any subsequent
         * nvkms_kapi_event_kthread_q_callback()'s for this popen should be
         * safe: if any nvkms_kapi_event_kthread_q_callback()-initiated work
         * attempts to call back into NVKMS, the popen->data==NULL check in
         * nvkms_ioctl_common() should reject the request.
         */

        nv_kthread_q_flush(&nvkms_kthread_q);
    }

    nvkms_free(popen, sizeof(*popen));
}

static void nvkms_close_pm_unlocked(void *data)
{
    struct nvkms_per_open *popen = data;

    nvkms_read_lock_pm_lock();

    nvkms_close_pm_locked(popen);

    nvkms_read_unlock_pm_lock();
}

static void nvkms_close_popen(struct nvkms_per_open *popen)
{
    if (nvkms_read_trylock_pm_lock() == 0) {
        nvkms_close_pm_locked(popen);
        nvkms_read_unlock_pm_lock();
    } else {
        nv_kthread_q_item_init(&popen->deferred_close_q_item,
                               nvkms_close_pm_unlocked,
                               popen);
        nvkms_queue_work(&nvkms_deferred_close_kthread_q,
                         &popen->deferred_close_q_item);
    }
}

static int nvkms_ioctl_common
(
    struct nvkms_per_open *popen,
    NvU32 cmd, NvU64 address, const size_t size
)
{
    int status;
    NvBool ret;

    status = down_interruptible(&nvkms_lock);
    if (status != 0) {
        return status;
    }

    if (popen->data != NULL) {
        ret = nvKmsIoctl(popen->data, cmd, address, size);
    } else {
        ret = NV_FALSE;
    }

    up(&nvkms_lock);

    return ret ? 0 : -EPERM;
}

/*************************************************************************
 * NVKMS interface for kernel space NVKMS clients like KAPI
 *************************************************************************/

struct nvkms_per_open* nvkms_open_from_kapi
(
    struct NvKmsKapiDevice *device
)
{
    int status = 0;
    struct nvkms_per_open *ret;

    nvkms_read_lock_pm_lock();
    ret = nvkms_open_common(NVKMS_CLIENT_KERNEL_SPACE, device, &status);
    nvkms_read_unlock_pm_lock();

    return ret;
}

void nvkms_close_from_kapi(struct nvkms_per_open *popen)
{
    nvkms_close_pm_unlocked(popen);
}

NvBool nvkms_ioctl_from_kapi_try_pmlock
(
    struct nvkms_per_open *popen,
    NvU32 cmd, void *params_address, const size_t param_size
)
{
    NvBool ret;

    if (nvkms_read_trylock_pm_lock()) {
        return NV_FALSE;
    }

    ret = nvkms_ioctl_common(popen,
                             cmd,
                             (NvU64)(NvUPtr)params_address, param_size) == 0;
    nvkms_read_unlock_pm_lock();

    return ret;
}

NvBool nvkms_ioctl_from_kapi
(
    struct nvkms_per_open *popen,
    NvU32 cmd, void *params_address, const size_t param_size
)
{
    NvBool ret;

    nvkms_read_lock_pm_lock();
    ret = nvkms_ioctl_common(popen,
                             cmd,
                             (NvU64)(NvUPtr)params_address, param_size) == 0;
    nvkms_read_unlock_pm_lock();

    return ret;
}

/*************************************************************************
 * APIs for locking.
 *************************************************************************/

struct nvkms_sema_t {
    struct semaphore os_sema;
};

nvkms_sema_handle_t* nvkms_sema_alloc(void)
{
    nvkms_sema_handle_t *sema = nvkms_alloc(sizeof(*sema), NV_TRUE);

    if (sema != NULL) {
        sema_init(&sema->os_sema, 1);
    }

    return sema;
}

void nvkms_sema_free(nvkms_sema_handle_t *sema)
{
    nvkms_free(sema, sizeof(*sema));
}

void nvkms_sema_down(nvkms_sema_handle_t *sema)
{
    down(&sema->os_sema);
}

void nvkms_sema_up(nvkms_sema_handle_t *sema)
{
    up(&sema->os_sema);
}

/*************************************************************************
 * Procfs files support code.
 *************************************************************************/

#if defined(CONFIG_PROC_FS)

#define NV_DEFINE_SINGLE_NVKMS_PROCFS_FILE(name) \
    NV_DEFINE_SINGLE_PROCFS_FILE_READ_ONLY(name, nvkms_pm_lock)

#define NVKMS_PROCFS_FOLDER "driver/nvidia-modeset"

struct proc_dir_entry *nvkms_proc_dir;

static void nv_procfs_out_string(void *data, const char *str)
{
    struct seq_file *s = data;

    seq_puts(s, str);
}

static int nv_procfs_read_nvkms_proc(struct seq_file *s, void *arg)
{
    char *buffer;
    nvkms_procfs_proc_t *func;

#define NVKMS_PROCFS_STRING_SIZE 8192

    func = s->private;
    if (func == NULL) {
        return 0;
    }

    buffer = nvkms_alloc(NVKMS_PROCFS_STRING_SIZE, NV_TRUE);

    if (buffer != NULL) {
        int status = down_interruptible(&nvkms_lock);

        if (status != 0) {
            nvkms_free(buffer, NVKMS_PROCFS_STRING_SIZE);
            return status;
        }

        func(s, buffer, NVKMS_PROCFS_STRING_SIZE, &nv_procfs_out_string);

        up(&nvkms_lock);

        nvkms_free(buffer, NVKMS_PROCFS_STRING_SIZE);
    }

    return 0;
}

NV_DEFINE_SINGLE_NVKMS_PROCFS_FILE(nvkms_proc);

static NvBool
nvkms_add_proc_file(const nvkms_procfs_file_t *file)
{
    struct proc_dir_entry *new_proc_dir;

    if (nvkms_proc_dir == NULL) {
        return NV_FALSE;
    }

    new_proc_dir = proc_create_data(file->name, 0, nvkms_proc_dir,
                                    &nv_procfs_nvkms_proc_fops, file->func);
    return (new_proc_dir != NULL);
}

#endif /* defined(CONFIG_PROC_FS) */

static void nvkms_proc_init(void)
{
#if defined(CONFIG_PROC_FS)
    const nvkms_procfs_file_t *file;

    nvkms_proc_dir = NULL;
    nvKmsGetProcFiles(&file);

    if (file == NULL || file->name == NULL) {
        return;
    }

    nvkms_proc_dir = NV_CREATE_PROC_DIR(NVKMS_PROCFS_FOLDER, NULL);
    if (nvkms_proc_dir == NULL) {
        return;
    }

    while (file->name != NULL) {
        if (!nvkms_add_proc_file(file)) {
            nvkms_log(NVKMS_LOG_LEVEL_WARN, NVKMS_LOG_PREFIX,
                      "Failed to create proc file");
            break;
        }
        file++;
    }
#endif
}

static void nvkms_proc_exit(void)
{
#if defined(CONFIG_PROC_FS)
    if (nvkms_proc_dir == NULL) {
        return;
    }

    proc_remove(nvkms_proc_dir);
#endif /* CONFIG_PROC_FS */
}

/*************************************************************************
 * NVKMS Config File Read
 ************************************************************************/
#if NVKMS_CONFIG_FILE_SUPPORTED
static NvBool nvkms_fs_mounted(void)
{
    return current->fs != NULL;
}

static size_t nvkms_config_file_open
(
    char *fname,
    char ** const buff
)
{
    int i = 0;
    struct file *file;
    struct inode *file_inode;
    size_t file_size = 0;
    size_t read_size = 0;
#if defined(NV_KERNEL_READ_HAS_POINTER_POS_ARG)
    loff_t pos = 0;
#endif

    *buff = NULL;
    
    if (!nvkms_fs_mounted()) {
        printk(KERN_ERR NVKMS_LOG_PREFIX "ERROR: Filesystems not mounted\n");
        return 0;
    }

    file = filp_open(fname, O_RDONLY, 0);
    if (file == NULL || IS_ERR(file)) {
        printk(KERN_WARNING NVKMS_LOG_PREFIX "WARNING: Failed to open %s\n",
               fname);
        return 0;
    }

    file_inode = file->f_inode;
    if (file_inode == NULL || IS_ERR(file_inode)) {
        printk(KERN_WARNING NVKMS_LOG_PREFIX "WARNING: Inode is invalid\n");
        goto done;
    }
    file_size = file_inode->i_size;
    if (file_size > NVKMS_READ_FILE_MAX_SIZE) {
        printk(KERN_WARNING NVKMS_LOG_PREFIX "WARNING: File exceeds maximum size\n");
        goto done;
    }

    // Do not alloc a 0 sized buffer
    if (file_size == 0) {
        goto done;
    }

    *buff = nvkms_alloc(file_size, NV_FALSE);
    if (*buff == NULL) {
        printk(KERN_WARNING NVKMS_LOG_PREFIX "WARNING: Out of memory\n");
        goto done;
    }

    /*
     * TODO: Once we have access to GPL symbols, this can be replaced with
     * kernel_read_file for kernels >= 4.6
     */
    while ((read_size < file_size) && (i++ < NVKMS_READ_FILE_MAX_LOOPS)) {
#if defined(NV_KERNEL_READ_HAS_POINTER_POS_ARG)
        ssize_t ret = kernel_read(file, *buff + read_size,
                                  file_size - read_size, &pos);
#else
        ssize_t ret = kernel_read(file, read_size,
                                  *buff + read_size,
                                  file_size - read_size);
#endif
        if (ret <= 0) {
            break;
        }
        read_size += ret;
    }

    if (read_size != file_size) {
        printk(KERN_WARNING NVKMS_LOG_PREFIX "WARNING: Failed to read %s\n",
               fname);
        goto done;
    }

    filp_close(file, current->files);
    return file_size;

done:
    nvkms_free(*buff, file_size);
    filp_close(file, current->files);
    return 0;
}

/* must be called with nvkms_lock locked */
static void nvkms_read_config_file_locked(void)
{
    char *buffer = NULL;
    size_t buf_size = 0;

    /* only read the config file if the kernel parameter is set */
    if (!NVKMS_CONF_FILE_SPECIFIED) {
        return;
    }

    buf_size = nvkms_config_file_open(nvkms_conf, &buffer);

    if (buf_size == 0) {
        return;
    }

    if (nvKmsReadConf(buffer, buf_size, nvkms_config_file_open)) {
        printk(KERN_INFO NVKMS_LOG_PREFIX "Successfully read %s\n",
               nvkms_conf);
    }

    nvkms_free(buffer, buf_size);
}
#else
static void nvkms_read_config_file_locked(void)
{
}
#endif

/*************************************************************************
 * NVKMS KAPI functions
 ************************************************************************/

NvBool nvKmsKapiGetFunctionsTable
(
    struct NvKmsKapiFunctionsTable *funcsTable
)
{
    return nvKmsKapiGetFunctionsTableInternal(funcsTable);
}
EXPORT_SYMBOL(nvKmsKapiGetFunctionsTable);

NvU32 nvKmsKapiF16ToF32(NvU16 a)
{
    return nvKmsKapiF16ToF32Internal(a);
}
EXPORT_SYMBOL(nvKmsKapiF16ToF32);

NvU16 nvKmsKapiF32ToF16(NvU32 a)
{
    return nvKmsKapiF32ToF16Internal(a);
}
EXPORT_SYMBOL(nvKmsKapiF32ToF16);

NvU32 nvKmsKapiF32Mul(NvU32 a, NvU32 b)
{
    return nvKmsKapiF32MulInternal(a, b);
}
EXPORT_SYMBOL(nvKmsKapiF32Mul);

NvU32 nvKmsKapiF32Div(NvU32 a, NvU32 b)
{
    return nvKmsKapiF32DivInternal(a, b);
}
EXPORT_SYMBOL(nvKmsKapiF32Div);

NvU32 nvKmsKapiF32Add(NvU32 a, NvU32 b)
{
    return nvKmsKapiF32AddInternal(a, b);
}
EXPORT_SYMBOL(nvKmsKapiF32Add);

NvU32 nvKmsKapiF32ToUI32RMinMag(NvU32 a, NvBool exact)
{
    return nvKmsKapiF32ToUI32RMinMagInternal(a, exact);
}
EXPORT_SYMBOL(nvKmsKapiF32ToUI32RMinMag);

NvU32 nvKmsKapiUI32ToF32(NvU32 a)
{
    return nvKmsKapiUI32ToF32Internal(a);
}
EXPORT_SYMBOL(nvKmsKapiUI32ToF32);

/*************************************************************************
 * File operation callback functions.
 *************************************************************************/

static int nvkms_open(struct inode *inode, struct file *filp)
{
    int status;

    status = nv_down_read_interruptible(&nvkms_pm_lock);
    if (status != 0) {
        return status;
    }

    filp->private_data =
        nvkms_open_common(NVKMS_CLIENT_USER_SPACE, NULL, &status);

    nvkms_read_unlock_pm_lock();

    return status;
}

static int nvkms_close(struct inode *inode, struct file *filp)
{
    struct nvkms_per_open *popen = filp->private_data;

    if (popen == NULL) {
        return -EINVAL;
    }

    nvkms_close_popen(popen);
    return 0;
}

static int nvkms_mmap(struct file *filp, struct vm_area_struct *vma)
{
    return -EPERM;
}

static int nvkms_ioctl(struct inode *inode, struct file *filp,
                           unsigned int cmd, unsigned long arg)
{
    size_t size;
    unsigned int nr;
    int status;
    struct NvKmsIoctlParams params;
    struct nvkms_per_open *popen = filp->private_data;

    if ((popen == NULL) || (popen->data == NULL)) {
        return -EINVAL;
    }

    size = _IOC_SIZE(cmd);
    nr = _IOC_NR(cmd);

    /* The only supported ioctl is NVKMS_IOCTL_CMD. */

    if ((nr != NVKMS_IOCTL_CMD) || (size != sizeof(struct NvKmsIoctlParams))) {
        return -ENOTTY;
    }

    status = copy_from_user(&params, (void *) arg, size);
    if (status != 0) {
        return -EFAULT;
    }

    status = nv_down_read_interruptible(&nvkms_pm_lock);
    if (status != 0) {
        return status;
    }

    status = nvkms_ioctl_common(popen,
                                params.cmd,
                                params.address,
                                params.size);

    nvkms_read_unlock_pm_lock();

    return status;
}

static long nvkms_unlocked_ioctl(struct file *filp, unsigned int cmd,
                                 unsigned long arg)
{
    return nvkms_ioctl(filp->f_inode, filp, cmd, arg);
}

static unsigned int nvkms_poll(struct file *filp, poll_table *wait)
{
    unsigned int mask = 0;
    struct nvkms_per_open *popen = filp->private_data;

    if ((popen == NULL) || (popen->data == NULL)) {
        return mask;
    }

    BUG_ON(popen->type != NVKMS_CLIENT_USER_SPACE);

    if ((filp->f_flags & O_NONBLOCK) == 0) {
        poll_wait(filp, &popen->u.user.events.wait_queue, wait);
    }

    if (atomic_read(&popen->u.user.events.available)) {
        mask = POLLPRI | POLLIN;
    }

    return mask;
}


/*************************************************************************
 * Module loading support code.
 *************************************************************************/

#define NVKMS_RDEV  (MKDEV(NV_MAJOR_DEVICE_NUMBER, \
                           NV_MINOR_DEVICE_NUMBER_MODESET_DEVICE))

static struct file_operations nvkms_fops = {
    .owner       = THIS_MODULE,
    .poll        = nvkms_poll,
    .unlocked_ioctl = nvkms_unlocked_ioctl,
#if NVCPU_IS_X86_64 || NVCPU_IS_AARCH64
    .compat_ioctl = nvkms_unlocked_ioctl,
#endif
    .mmap        = nvkms_mmap,
    .open        = nvkms_open,
    .release     = nvkms_close,
};

static struct cdev nvkms_device_cdev;

static int __init nvkms_register_chrdev(void)
{
    int ret;

    ret = register_chrdev_region(NVKMS_RDEV, 1, "nvidia-modeset");
    if (ret < 0) {
        return ret;
    }

    cdev_init(&nvkms_device_cdev, &nvkms_fops);
    ret = cdev_add(&nvkms_device_cdev, NVKMS_RDEV, 1);
    if (ret < 0) {
        unregister_chrdev_region(NVKMS_RDEV, 1);
        return ret;
    }

    return ret;
}

static void nvkms_unregister_chrdev(void)
{
    cdev_del(&nvkms_device_cdev);
    unregister_chrdev_region(NVKMS_RDEV, 1);
}

void* nvkms_get_per_open_data(int fd)
{
    struct file *filp = fget(fd);
    void *data = NULL;

    if (filp) {
        if (filp->f_op == &nvkms_fops && filp->private_data) {
            struct nvkms_per_open *popen = filp->private_data;
            data = popen->data;
        }

        /*
         * fget() incremented the struct file's reference count, which needs to
         * be balanced with a call to fput().  It is safe to decrement the
         * reference count before returning filp->private_data because core
         * NVKMS is currently holding the nvkms_lock, which prevents the
         * nvkms_close() => nvKmsClose() call chain from freeing the file out
         * from under the caller of nvkms_get_per_open_data().
         */
        fput(filp);
    }

    return data;
}

static int __init nvkms_init(void)
{
    int ret;

    atomic_set(&nvkms_alloc_called_count, 0);

    ret = nvkms_alloc_rm();

    if (ret != 0) {
        return ret;
    }

    sema_init(&nvkms_lock, 1);
    init_rwsem(&nvkms_pm_lock);

    ret = nv_kthread_q_init(&nvkms_kthread_q,
                            "nvidia-modeset/kthread_q");
    if (ret != 0) {
        goto fail_kthread;
    }

    ret = nv_kthread_q_init(&nvkms_deferred_close_kthread_q,
                            "nvidia-modeset/deferred_close_kthread_q");
    if (ret != 0) {
        goto fail_deferred_close_kthread;
    }

    INIT_LIST_HEAD(&nvkms_timers.list);
    spin_lock_init(&nvkms_timers.lock);

    ret = nvkms_register_chrdev();
    if (ret != 0) {
        goto fail_register_chrdev;
    }

    down(&nvkms_lock);
    if (!nvKmsModuleLoad()) {
        ret = -ENOMEM;
    }
    if (ret != 0) {
        up(&nvkms_lock);
        goto fail_module_load;
    }
    nvkms_read_config_file_locked();
    up(&nvkms_lock);

    nvkms_proc_init();

    return 0;

fail_module_load:
    nvkms_unregister_chrdev();
fail_register_chrdev:
    nv_kthread_q_stop(&nvkms_deferred_close_kthread_q);
fail_deferred_close_kthread:
    nv_kthread_q_stop(&nvkms_kthread_q);
fail_kthread:
    nvkms_free_rm();

    return ret;
}

static void __exit nvkms_exit(void)
{
    struct nvkms_timer_t *timer, *tmp_timer;
    unsigned long flags = 0;

    nvkms_proc_exit();

    down(&nvkms_lock);
    nvKmsModuleUnload();
    up(&nvkms_lock);

    /*
     * At this point, any pending tasks should be marked canceled, but
     * we still need to drain them, so that nvkms_kthread_q_callback() doesn't
     * get called after the module is unloaded.
     */
restart:
    spin_lock_irqsave(&nvkms_timers.lock, flags);

    list_for_each_entry_safe(timer, tmp_timer, &nvkms_timers.list, timers_list) {
        if (timer->kernel_timer_created) {
            /*
             * We delete pending timers and check whether it was being executed
             * (returns 0) or we have deactivated it before execution (returns 1).
             * If it began execution, the kthread_q callback will wait for timer
             * completion, and we wait for queue completion with
             * nv_kthread_q_stop below.
             */
#if !defined(NV_BSD) && NV_IS_EXPORT_SYMBOL_PRESENT_timer_delete_sync
            if (timer_delete_sync(&timer->kernel_timer) == 1) {
#else
            if (del_timer_sync(&timer->kernel_timer) == 1) {
#endif
                /*  We've deactivated timer so we need to clean after it */
                list_del(&timer->timers_list);

                /* We need to unlock spinlock because we are freeing memory which
                 * may sleep */
                spin_unlock_irqrestore(&nvkms_timers.lock, flags);

                if (timer->isRefPtr) {
                    nvkms_dec_ref(timer->dataPtr);
                    kfree(timer);
                } else {
                    nvkms_free(timer, sizeof(*timer));
                }

                /* List could change when we were freeing memory. */
                goto restart;
            }
        }
    }

    spin_unlock_irqrestore(&nvkms_timers.lock, flags);

    nv_kthread_q_stop(&nvkms_deferred_close_kthread_q);
    nv_kthread_q_stop(&nvkms_kthread_q);

    nvkms_unregister_chrdev();
    nvkms_free_rm();

    if (malloc_verbose) {
        printk(KERN_INFO NVKMS_LOG_PREFIX "Total allocations: %d\n",
               atomic_read(&nvkms_alloc_called_count));
    }
}

module_init(nvkms_init);
module_exit(nvkms_exit);

  MODULE_LICENSE("Dual MIT/GPL");

MODULE_INFO(supported, "external");
MODULE_VERSION(NV_VERSION_STRING);
