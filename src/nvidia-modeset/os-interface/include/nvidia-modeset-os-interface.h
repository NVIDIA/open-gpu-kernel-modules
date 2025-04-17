/*
 * SPDX-FileCopyrightText: Copyright (c) 2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * Define the entry points which the NVKMS kernel interface layer
 * provides to core NVKMS.
 */

#if !defined(_NVIDIA_MODESET_OS_INTERFACE_H_)
#define _NVIDIA_MODESET_OS_INTERFACE_H_

#if defined(NV_KERNEL_INTERFACE_LAYER) && defined(NV_LINUX)
#include <linux/stddef.h>  /* size_t */
#else
#include <stddef.h>        /* size_t */
#endif
#include "nvtypes.h" /* NvU8 */

#include "nvkms.h"
#include "nv_stdarg.h"

enum NvKmsSyncPtOp {
    /*
     * Call into Tegra's kernel nvhost driver, and allocate a syncpoint that can
     * be exclusively used by the caller. Internally, this operation will call
     * get() to set the initial refcount of the syncpoint to 1.
     */
    NVKMS_SYNCPT_OP_ALLOC,
    /*
     * Decrease the refcount of an already allocated syncpoint. Once the
     * refcount drops to 0, the syncpoint will be returned to the free pool that
     * nvhost manages, so PUT can also be used to balance out an ALLOC.
     */
    NVKMS_SYNCPT_OP_PUT,
    /*
     * Extract syncpt id and thresh from the sync-file file descriptor
     */
    NVKMS_SYNCPT_OP_FD_TO_ID_AND_THRESH,
    /*
     * Create dma-fence from syncpt id and thresh value and create sync_file
     * file descriptor for the dma-fence handle created.
     */
    NVKMS_SYNCPT_OP_ID_AND_THRESH_TO_FD,
    /*
     * read syncpt minimum value of given syncpt
     */
    NVKMS_SYNCPT_OP_READ_MINVAL,
};

enum NvKmsDebugForceColorSpace {
    NVKMS_DEBUG_FORCE_COLOR_SPACE_NONE,
    NVKMS_DEBUG_FORCE_COLOR_SPACE_RGB,
    NVKMS_DEBUG_FORCE_COLOR_SPACE_YUV444,
    NVKMS_DEBUG_FORCE_COLOR_SPACE_YUV422,
    NVKMS_DEBUG_FORCE_COLOR_SPACE_MAX,
};

typedef struct {

    struct {
        const char *syncpt_name;        /*  in   */
        NvU32 id;                       /*  out  */
    } alloc;

    struct {
        NvU32 id;                       /*  in   */
    } put;

    struct {
        NvS32 fd;                       /*  in   */
        NvU32 id;                       /*  out  */
        NvU32 thresh;                   /*  out  */
    } fd_to_id_and_thresh;

    struct {
        NvU32 id;                       /*  in   */
        NvU32 thresh;                   /*  in   */
        NvS32 fd;                       /*  out  */
    } id_and_thresh_to_fd;

    struct {
        NvU32 id;                       /*  in   */
        NvU32 minval;                   /*  out  */
    } read_minval;
} NvKmsSyncPtOpParams;

enum FailAllocCoreChannelMethod {
    FAIL_ALLOC_CORE_CHANNEL_RM_SETUP_CORE_CHANNEL = 0,
    FAIL_ALLOC_CORE_CHANNEL_RESTORE_CONSOLE = 1,
};

NvBool nvkms_test_fail_alloc_core_channel(enum FailAllocCoreChannelMethod method);
NvBool nvkms_conceal_vrr_caps(void);
NvBool nvkms_output_rounding_fix(void);
NvBool nvkms_disable_hdmi_frl(void);
NvBool nvkms_disable_vrr_memclk_switch(void);
NvBool nvkms_hdmi_deepcolor(void);
NvBool nvkms_vblank_sem_control(void);
NvBool nvkms_opportunistic_display_sync(void);
enum NvKmsDebugForceColorSpace nvkms_debug_force_color_space(void);
NvBool nvkms_enable_overlay_layers(void);

void   nvkms_call_rm    (void *ops);
void*  nvkms_alloc      (size_t size,
                         NvBool zero);
void   nvkms_free       (void *ptr,
                         size_t size);
void*  nvkms_memset     (void *ptr,
                         NvU8 c,
                         size_t size);
void*  nvkms_memcpy     (void *dest,
                         const void *src,
                         size_t n);
void*  nvkms_memmove    (void *dest,
                         const void *src,
                         size_t n);
int    nvkms_memcmp     (const void *s1,
                         const void *s2,
                         size_t n);
size_t nvkms_strlen     (const char *s);
int    nvkms_strcmp     (const char *s1,
                         const char *s2);
char*  nvkms_strncpy    (char *dest,
                         const char *src,
                         size_t n);
void   nvkms_usleep     (NvU64 usec);
NvU64  nvkms_get_usec   (void);
int    nvkms_copyin     (void *kptr,
                         NvU64 uaddr,
                         size_t n);
int    nvkms_copyout    (NvU64 uaddr,
                         const void *kptr,
                         size_t n);
void   nvkms_yield      (void);
void   nvkms_dump_stack (void);
NvBool nvkms_syncpt_op  (enum NvKmsSyncPtOp op,
                         NvKmsSyncPtOpParams *params);
int    nvkms_snprintf   (char *str,
                         size_t size,
                         const char *format, ...)
    __attribute__((format (printf, 3, 4)));

int    nvkms_vsnprintf  (char *str,
                         size_t size,
                         const char *format,
                         va_list ap);

#define NVKMS_LOG_LEVEL_INFO  0
#define NVKMS_LOG_LEVEL_WARN  1
#define NVKMS_LOG_LEVEL_ERROR 2

void   nvkms_log        (const int level,
                         const char *gpuPrefix,
                         const char *msg);

/*!
 * Refcounted pointer to an object that may be freed while references still
 * exist.
 *
 * This structure is intended to be used for nvkms timers to refer to objects
 * that may be freed while timers with references to the object are still
 * pending.
 *
 * When the owner of an nvkms_ref_ptr is freed, the teardown code should call
 * nvkms_free_ref_ptr().  That marks the pointer as invalid so that later calls
 * to nvkms_dec_ref() (i.e. from a workqueue callback) return NULL rather than
 * the pointer originally passed to nvkms_alloc_ref_ptr().
 */
struct nvkms_ref_ptr;

/*!
 * Allocate and initialize a ref_ptr.
 *
 * The pointer stored in the ref_ptr is initialized to ptr, and its refcount is
 * initialized to 1.
 */
struct nvkms_ref_ptr* nvkms_alloc_ref_ptr(void *ptr);

/*!
 * Clear a ref_ptr.
 *
 * This function sets the pointer stored in the ref_ptr to NULL and drops the
 * reference created by nvkms_alloc_ref_ptr().  This function should be called
 * when the object pointed to by the ref_ptr is freed.
 *
 * A caller should make sure that no code that can call nvkms_inc_ref() can
 * execute after nvkms_free_ref_ptr() is called.
 */
void nvkms_free_ref_ptr(struct nvkms_ref_ptr *ref_ptr);

/*!
 * Increment the refcount of a ref_ptr.
 *
 * This function should be used when a pointer to the ref_ptr is stored
 * somewhere.  For example, when the ref_ptr is used as the argument to
 * nvkms_alloc_timer.
 *
 * This may be called outside of the nvkms_lock, for example by an RM callback.
 */
void nvkms_inc_ref(struct nvkms_ref_ptr *ref_ptr);

/*!
 * Decrement the refcount of a ref_ptr and extract the embedded pointer.
 *
 * This should be used by code that needs to atomically determine whether the
 * object pointed to by the ref_ptr still exists.  To prevent the object from
 * being destroyed while the current thread is executing, this should be called
 * from inside the nvkms_lock.
 */
void* nvkms_dec_ref(struct nvkms_ref_ptr *ref_ptr);

typedef void nvkms_timer_proc_t(void *dataPtr, NvU32 dataU32);
typedef struct nvkms_timer_t nvkms_timer_handle_t;

/*!
 * Schedule a callback function to be called in the future.
 *
 * The callback function 'proc' will be called with the arguments
 * 'dataPtr' and 'dataU32' at 'usec' (or later) microseconds from now.
 * If usec==0, the callback will be scheduled to be called as soon as
 * possible.
 *
 * The callback function is guaranteed to be called back with the
 * nvkms_lock held, and in process context.
 *
 * Returns an opaque handle, nvkms_timer_handle_t*, or NULL on
 * failure.  If non-NULL, the caller is responsible for caching the
 * handle and eventually calling nvkms_free_timer() to free the
 * memory.
 *
 * The nvkms_lock may be held when nvkms_alloc_timer() is called, but
 * the nvkms_lock is not required.
 */
nvkms_timer_handle_t* nvkms_alloc_timer (nvkms_timer_proc_t *proc,
                                         void *dataPtr, NvU32 dataU32,
                                         NvU64 usec);

/*!
 * Schedule a callback function to be called in the future.
 *
 * This function is like nvkms_alloc_timer() except that instead of returning a
 * pointer to a structure that the caller should free later, the timer will free
 * itself after executing the callback function.  This is only intended for
 * cases where the caller cannot cache the nvkms_alloc_timer() return value.
 */
NvBool
nvkms_alloc_timer_with_ref_ptr(nvkms_timer_proc_t *proc,
                               struct nvkms_ref_ptr *ref_ptr,
                               NvU32 dataU32, NvU64 usec);

/*!
 * Free the nvkms_timer_t object.  If the callback function has not
 * yet been called, freeing the nvkms_timer_handle_t will guarantee
 * that it is not called.
 *
 * The nvkms_lock must be held when calling nvkms_free_timer().
 */
void nvkms_free_timer  (nvkms_timer_handle_t *handle);



/*!
 * Notify the NVKMS kernel interface that the event queue has changed.
 *
 * \param[in]  pOpenKernel      This indicates the file descriptor
 *                              ("per-open") of the client whose event queue
 *                              has been updated.  This is the pointer
 *                              passed by the kernel interface to nvKmsOpen().
 * \param[in]  eventsAvailable  If TRUE, a new event has been added to the
 *                              event queue.  If FALSE, the last event has
 *                              been removed from the event queue.
 */
void
nvkms_event_queue_changed(nvkms_per_open_handle_t *pOpenKernel,
                          NvBool eventsAvailable);


/*!
 * Get the "per-open" data (the pointer returned by nvKmsOpen())
 * associated with this fd.
 */
void* nvkms_get_per_open_data(int fd);


/*!
 * Raise and lower the reference count of the specified GPU.
 */
NvBool nvkms_open_gpu(NvU32 gpuId);
void nvkms_close_gpu(NvU32 gpuId);


/*!
 * Enumerate nvidia gpus.
 */

NvU32 nvkms_enumerate_gpus(nv_gpu_info_t *gpu_info);

/*!
 * Availability of write combining support for video memory.
 */

NvBool nvkms_allow_write_combining(void);

/*!
 * Check if OS supports syncpoints.
 */
NvBool nvkms_kernel_supports_syncpts(void);

/*!
 * Checks whether the fd is associated with an nvidia character device.
 */
NvBool nvkms_fd_is_nvidia_chardev(int fd);

/*!
 * NVKMS interface for kernel space NVKMS clients like KAPI
 */

struct nvkms_per_open;

struct nvkms_per_open* nvkms_open_from_kapi
(
    struct NvKmsKapiDevice *device
);

void nvkms_close_from_kapi(struct nvkms_per_open *popen);

NvBool nvkms_ioctl_from_kapi
(
    struct nvkms_per_open *popen,
    NvU32 cmd, void *params_address, const size_t params_size
);

/*!
 * Like nvkms_ioctl_from_kapi, but return NV_FALSE instead of waiting if the
 * power management read lock cannot be acquired.
 */
NvBool nvkms_ioctl_from_kapi_try_pmlock
(
    struct nvkms_per_open *popen,
    NvU32 cmd, void *params_address, const size_t params_size
);

/*!
 * APIs for locking.
 */

typedef struct nvkms_sema_t nvkms_sema_handle_t;

nvkms_sema_handle_t*
     nvkms_sema_alloc    (void);
void nvkms_sema_free     (nvkms_sema_handle_t *sema);
void nvkms_sema_down     (nvkms_sema_handle_t *sema);
void nvkms_sema_up       (nvkms_sema_handle_t *sema);

/*!
 * APIs to register/unregister backlight device.
 */
struct nvkms_backlight_device;

struct nvkms_backlight_device*
nvkms_register_backlight(NvU32 gpu_id, NvU32 display_id, void *drv_priv,
                         NvU32 current_brightness);

void nvkms_unregister_backlight(struct nvkms_backlight_device *nvkms_bd);

#endif /* _NVIDIA_MODESET_OS_INTERFACE_H_ */

