/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "conftest.h"

#include "nvlink_os.h"
#include "nvlink_linux.h"
#include "nvlink_errors.h"
#include "nvlink_export.h"
#include "nvlink_proto.h"
#include "nv-linux.h"
#include "nv-procfs.h"
#include "nv-time.h"
#include "nvlink_caps.h"

#include <linux/module.h>
#include <linux/major.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/mutex.h>

#define MAX_ERROR_STRING           512

typedef struct nvlink_file_private
{
    struct
    {
        /* A duped file descriptor for fabric_mgmt capability */
        int fabric_mgmt;
    } capability_fds;
} nvlink_file_private_t;

#define NVLINK_SET_FILE_PRIVATE(filp, data) ((filp)->private_data = (data))
#define NVLINK_GET_FILE_PRIVATE(filp) ((nvlink_file_private_t *)(filp)->private_data)

typedef struct
{
    struct mutex    lock;
    NvBool          initialized;
    struct cdev     cdev;
    dev_t           devno;
    int             opened;
    int             major_devnum;
} _nvlink_drvctx;


// nvlink driver local state
static _nvlink_drvctx nvlink_drvctx;

#if defined(CONFIG_PROC_FS)
#define NV_DEFINE_SINGLE_NVLINK_PROCFS_FILE(name) \
    NV_DEFINE_SINGLE_PROCFS_FILE_READ_ONLY(name, nv_system_pm_lock)
#endif

#define NVLINK_PROCFS_DIR "driver/nvidia-nvlink"

static struct proc_dir_entry *nvlink_procfs_dir = NULL;

#if defined(CONFIG_PROC_FS)
    static int nvlink_is_procfs_available = 1;
#else
    static int nvlink_is_procfs_available = 0;
#endif

static struct proc_dir_entry *nvlink_permissions = NULL;

static int nv_procfs_read_permissions(struct seq_file *s, void *v)
{
    // Restrict device node permissions - 0666.
    seq_printf(s, "%s: %u\n", "DeviceFileMode", 438);

    return 0;
}

NV_DEFINE_SINGLE_NVLINK_PROCFS_FILE(permissions);

static void nvlink_permissions_exit(void)
{
    if (!nvlink_permissions)
    {
        return;
    }

    proc_remove(nvlink_permissions);
    nvlink_permissions = NULL;
}

static int nvlink_permissions_init(void)
{
    if (!nvlink_procfs_dir)
    {
        return -EACCES;
    }

    nvlink_permissions = NV_CREATE_PROC_FILE("permissions",
                                             nvlink_procfs_dir,
                                             permissions,
                                             NULL);
    if (!nvlink_permissions)
    {
        return -EACCES;
    }

    return 0;
}

static void nvlink_procfs_exit(void)
{
    nvlink_permissions_exit();

    if (!nvlink_procfs_dir)
    {
        return;
    }

    proc_remove(nvlink_procfs_dir);
    nvlink_procfs_dir = NULL;
}

static int nvlink_procfs_init(void)
{
    int rc = 0;

    if (!nvlink_is_procfs_available)
    {
        return -EACCES;
    }

    nvlink_procfs_dir = NV_CREATE_PROC_DIR(NVLINK_PROCFS_DIR, NULL);
    if (!nvlink_procfs_dir)
    {
        return -EACCES;
    }

    rc = nvlink_permissions_init();
    if (rc < 0)
    {
        goto cleanup;
    }

    return 0;

cleanup:

    nvlink_procfs_exit();

    return rc;
}

static int nvlink_fops_open(struct inode *inode, struct file *filp)
{
    int rc = 0;
    nvlink_file_private_t *private = NULL;

    nvlink_print(NVLINK_DBG_INFO, "nvlink driver open\n");

    mutex_lock(&nvlink_drvctx.lock);

    // nvlink lib driver is currently exclusive open.
    if (nvlink_drvctx.opened)
    {
        rc = -EBUSY;
        goto open_error;
    }

    private = (nvlink_file_private_t *)nvlink_malloc(sizeof(*private));
    if (private == NULL)
    {
        rc = -ENOMEM;
        goto open_error;
    }

    private->capability_fds.fabric_mgmt = -1;
    NVLINK_SET_FILE_PRIVATE(filp, private);

    // mark our state as opened
    nvlink_drvctx.opened = NV_TRUE;

open_error:
    mutex_unlock(&nvlink_drvctx.lock);
    return rc;
}

static int nvlink_fops_release(struct inode *inode, struct file *filp)
{
    nvlink_file_private_t *private = NVLINK_GET_FILE_PRIVATE(filp);

    nvlink_print(NVLINK_DBG_INFO, "nvlink driver close\n");

    if (private == NULL)
        return -ENOMEM;

    mutex_lock(&nvlink_drvctx.lock);

    if (private->capability_fds.fabric_mgmt > 0)
    {
        nvlink_cap_release(private->capability_fds.fabric_mgmt);
        private->capability_fds.fabric_mgmt = -1;
    }

    nvlink_free(filp->private_data);
    NVLINK_SET_FILE_PRIVATE(filp, NULL);

    // mark the device as not opened
    nvlink_drvctx.opened = NV_FALSE;

    mutex_unlock(&nvlink_drvctx.lock);

    return 0;
}

static int nvlink_fops_ioctl(struct inode *inode,
                             struct file *filp,
                             unsigned int cmd,
                             unsigned long arg)
{
    nvlink_ioctrl_params ctrl_params = {0};
    int param_size = _IOC_SIZE(cmd);
    void *param_buf = NULL;
    NvlStatus ret_val = 0;
    int rc = 0;

    // no buffer for simple _IO types
    if (param_size)
    {
        // allocate a buffer to hold user input
        param_buf = kzalloc(param_size, GFP_KERNEL);
        if (NULL == param_buf) 
        {
            rc = -ENOMEM;
            goto nvlink_ioctl_fail;
        }

        // copy user input to kernel buffers. Simple _IOR() ioctls can skip this step.
        if (_IOC_DIR(cmd) & _IOC_WRITE)
        {
            // copy user input to local buffer
            if (copy_from_user(param_buf, (const void *)arg, param_size))
            {
                rc = -EFAULT;
                goto nvlink_ioctl_fail;
            }
        }
    }

    ctrl_params.osPrivate = filp->private_data;
    ctrl_params.cmd = _IOC_NR(cmd);
    ctrl_params.buf = param_buf;
    ctrl_params.size = param_size;

    ret_val = nvlink_lib_ioctl_ctrl(&ctrl_params);
    if (NVL_SUCCESS != ret_val)
    {
        rc = -EINVAL;
        goto nvlink_ioctl_fail;
    }

    //  no copy for write-only ioctl
    if ((param_size) && (_IOC_DIR(cmd) & _IOC_READ))
    {
        if (copy_to_user((void *)arg, ctrl_params.buf, ctrl_params.size))
        {
            rc = -EFAULT;
            goto nvlink_ioctl_fail;
        }
    }

nvlink_ioctl_fail:
    if (param_buf) 
    {
        kfree(param_buf);
    }
    return rc;
}

#define NV_FILE_INODE(file) (file)->f_inode

static long nvlink_fops_unlocked_ioctl(struct file *file,
                                       unsigned int cmd,
                                       unsigned long arg)
{
    return nvlink_fops_ioctl(NV_FILE_INODE(file), file, cmd, arg);
}


static const struct file_operations nvlink_fops = {
    .owner           = THIS_MODULE,
    .open            = nvlink_fops_open,
    .release         = nvlink_fops_release,
    .unlocked_ioctl  = nvlink_fops_unlocked_ioctl,
};

int __init nvlink_core_init(void)
{
    NvlStatus ret_val;
    int rc;

    if (NV_TRUE == nvlink_drvctx.initialized)
    {
        nvlink_print(NVLINK_DBG_ERRORS, "nvlink core interface already initialized\n");
        return -EBUSY;
    }

    mutex_init(&nvlink_drvctx.lock);

    ret_val = nvlink_lib_initialize();
    if (NVL_SUCCESS != ret_val)
    {
        nvlink_print(NVLINK_DBG_ERRORS,  "Failed to initialize driver : %d\n", ret_val);
        rc = -ENODEV;
        goto nvlink_lib_initialize_fail;
    }

    rc = alloc_chrdev_region(&nvlink_drvctx.devno, 0, NVLINK_NUM_MINOR_DEVICES,
                             NVLINK_DEVICE_NAME);
    if (rc < 0)
    {
        nvlink_print(NVLINK_DBG_ERRORS, "alloc_chrdev_region failed: %d\n", rc);
        goto alloc_chrdev_region_fail;
    }

    nvlink_drvctx.major_devnum = MAJOR(nvlink_drvctx.devno);
    nvlink_print(NVLINK_DBG_INFO, "Nvlink Core is being initialized, major device number %d\n",
                 nvlink_drvctx.major_devnum);

    cdev_init(&nvlink_drvctx.cdev, &nvlink_fops);
    nvlink_drvctx.cdev.owner = THIS_MODULE;
    rc = cdev_add(&nvlink_drvctx.cdev, nvlink_drvctx.devno, NVLINK_NUM_MINOR_DEVICES);
    if (rc < 0)
    {
        nvlink_print(NVLINK_DBG_ERRORS, " Unable to create cdev\n");
        goto cdev_add_fail;
    }

    rc = nvlink_procfs_init();
    if (rc < 0)
    {
        goto procfs_init_fail;
    }

    rc = nvlink_cap_init(NVLINK_PROCFS_DIR);
    if (rc < 0)
    {
        nvlink_print(NVLINK_DBG_ERRORS, " Unable to create capability\n");
        goto cap_init_fail;
    }

    nvlink_drvctx.initialized = NV_TRUE;

    return 0;

cap_init_fail:
    nvlink_procfs_exit();

procfs_init_fail:
    cdev_del(&nvlink_drvctx.cdev);

cdev_add_fail:
    unregister_chrdev_region(nvlink_drvctx.devno, NVLINK_NUM_MINOR_DEVICES);

alloc_chrdev_region_fail:
    nvlink_lib_unload();

nvlink_lib_initialize_fail:
    nv_mutex_destroy(&nvlink_drvctx.lock);
    return rc;
}

void nvlink_core_exit(void)
{
    if (NV_FALSE == nvlink_drvctx.initialized)
    {
        return;
    }

    nvlink_cap_exit();

    nvlink_procfs_exit();

    cdev_del(&nvlink_drvctx.cdev);

    unregister_chrdev_region(nvlink_drvctx.devno, NVLINK_NUM_MINOR_DEVICES);

    nvlink_lib_unload();

    nv_mutex_destroy(&nvlink_drvctx.lock);

    nvlink_print(NVLINK_DBG_INFO, "Unregistered Nvlink Core, major device number %d\n",
                 nvlink_drvctx.major_devnum);
}

void
nvlink_print
(
    const char *file,
    int         line,
    const char *function,
    int         log_level,
    const char *fmt,
    ...
)
{
    va_list arglist;
    char    nv_string[MAX_ERROR_STRING];
    char   *sys_log_level;

    switch (log_level) {
    case NVLINK_DBG_LEVEL_INFO:
        sys_log_level = KERN_INFO;
        break;
    case NVLINK_DBG_LEVEL_SETUP:
        sys_log_level = KERN_DEBUG;
        break;
    case NVLINK_DBG_LEVEL_USERERRORS:
        sys_log_level = KERN_NOTICE;
        break;
    case NVLINK_DBG_LEVEL_WARNINGS:
        sys_log_level = KERN_WARNING;
        break;
    case NVLINK_DBG_LEVEL_ERRORS:
        sys_log_level = KERN_ERR;
        break;
    default:
        sys_log_level = KERN_INFO;
        break;
    }

    va_start(arglist, fmt);
    vsnprintf(nv_string, sizeof(nv_string), fmt, arglist);
    va_end(arglist);

    nv_string[sizeof(nv_string) - 1] = '\0';
    printk("%snvidia-nvlink: %s", sys_log_level, nv_string);
}

void * nvlink_malloc(NvLength size)
{
   return kmalloc(size, GFP_KERNEL);
}

void nvlink_free(void *ptr)
{
    return kfree(ptr);
}

char * nvlink_strcpy(char *dest, const char *src)
{
    return strcpy(dest, src);
}

int nvlink_strcmp(const char *dest, const char *src)
{
    return strcmp(dest, src);
}

NvLength nvlink_strlen(const char *s)
{
    return strlen(s);
}

int nvlink_snprintf(char *dest, NvLength size, const char *fmt, ...)
{
    va_list arglist;
    int chars_written;

    va_start(arglist, fmt);
    chars_written = vsnprintf(dest, size, fmt, arglist);
    va_end(arglist);

    return chars_written;
}

NvU32 nvlink_memRd32(const volatile void * address)
{
    return (*(const volatile NvU32*)(address));
}

void nvlink_memWr32(volatile void *address, NvU32 data)
{
    (*(volatile NvU32 *)(address)) = data;
}

NvU64 nvlink_memRd64(const volatile void * address)
{
    return (*(const volatile NvU64 *)(address));
}

void nvlink_memWr64(volatile void *address, NvU64 data)
{
    (*(volatile NvU64 *)(address)) = data;
}

void * nvlink_memset(void *dest, int value, NvLength size)
{
     return memset(dest, value, size);
}

void * nvlink_memcpy(void *dest, const void *src, NvLength size)
{
    return memcpy(dest, src, size);
}

int nvlink_memcmp(const void *s1, const void *s2, NvLength size)
{
    return memcmp(s1, s2, size);
}

/*
 * Sleep for specified milliseconds. Yields the CPU to scheduler.
 */
void nvlink_sleep(unsigned int ms)
{
    NV_STATUS status;

    status = nv_sleep_ms(ms);

    if (status !=  NV_OK)
    {
        if (printk_ratelimit())
        {
            nvlink_print(NVLINK_DBG_ERRORS, "NVLink: requested sleep duration"
                         " %d msec exceeded %d msec\n",
                         ms, NV_MAX_ISR_DELAY_MS);
            WARN_ON(1);
        }
    }
}

void nvlink_assert(int cond)
{
    if ((cond) == 0x0)
    {
        if (printk_ratelimit())
        {
            nvlink_print(NVLINK_DBG_ERRORS, "NVLink: Assertion failed!\n");
            WARN_ON(1);
        }

        dbg_breakpoint();
    }
}

void * nvlink_allocLock(void)
{
    struct semaphore *sema;

    sema = nvlink_malloc(sizeof(*sema));
    if (sema == NULL)
    {
        nvlink_print(NVLINK_DBG_ERRORS, "Failed to allocate sema!\n");
        return NULL;
    }
    sema_init(sema, 1);

    return sema;
}

void nvlink_acquireLock(void *hLock)
{
    down(hLock);
}

void nvlink_releaseLock(void *hLock)
{
    up(hLock);
}

void nvlink_freeLock(void *hLock)
{
    if (NULL == hLock)
    {
        return;
    }

    NVLINK_FREE(hLock);
}

NvBool nvlink_isLockOwner(void *hLock)
{
    return NV_TRUE;
}

NvlStatus nvlink_acquire_fabric_mgmt_cap(void *osPrivate, NvU64 capDescriptor)
{
    int dup_fd = -1;
    nvlink_file_private_t *private_data = (nvlink_file_private_t *)osPrivate;

    if (private_data == NULL)
    {
        return NVL_BAD_ARGS;
    }

    dup_fd = nvlink_cap_acquire((int)capDescriptor,
                                NVLINK_CAP_FABRIC_MANAGEMENT);
    if (dup_fd < 0)
    {
        return NVL_ERR_OPERATING_SYSTEM;
    }

    private_data->capability_fds.fabric_mgmt = dup_fd;
    return NVL_SUCCESS;
}

int nvlink_is_fabric_manager(void *osPrivate)
{
    nvlink_file_private_t *private_data = (nvlink_file_private_t *)osPrivate;

    /* Make sure that fabric mgmt capbaility fd is valid */
    if ((private_data == NULL) ||
        (private_data->capability_fds.fabric_mgmt < 0))
    {
        return 0;
    }

    return 1;
}

int nvlink_is_admin(void)
{
    return NV_IS_SUSER();
}

NvU64 nvlink_get_platform_time(void)
{
    return nv_ktime_get_raw_ns();
}
