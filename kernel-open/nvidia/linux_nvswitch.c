/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "linux_nvswitch.h"

#include <linux/version.h>

#include "conftest.h"
#include "nvlink_errors.h"
#include "nvlink_linux.h"
#include "nvCpuUuid.h"
#include "nv-time.h"
#include "nvlink_caps.h"

#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/string.h>
#include <linux/moduleparam.h>
#include <linux/ctype.h>
#include <linux/wait.h>
#include <linux/jiffies.h>

#include "ioctl_nvswitch.h"

static const struct
{
    NvlStatus status;
    int err;
} nvswitch_status_map[] = {
    { NVL_ERR_GENERIC,                  -EIO        },
    { NVL_NO_MEM,                       -ENOMEM     },
    { NVL_BAD_ARGS,                     -EINVAL     },
    { NVL_ERR_INVALID_STATE,            -EIO        },
    { NVL_ERR_NOT_SUPPORTED,            -EOPNOTSUPP },
    { NVL_NOT_FOUND,                    -EINVAL     },
    { NVL_ERR_STATE_IN_USE,             -EBUSY      },
    { NVL_ERR_NOT_IMPLEMENTED,          -ENOSYS     },
    { NVL_ERR_INSUFFICIENT_PERMISSIONS, -EPERM      },
    { NVL_ERR_OPERATING_SYSTEM,         -EIO        },
    { NVL_MORE_PROCESSING_REQUIRED,     -EAGAIN     },
    { NVL_SUCCESS,                       0          },
};

int
nvswitch_map_status
(
    NvlStatus status
)
{
    int err = -EIO;
    NvU32 i;
    NvU32 limit = sizeof(nvswitch_status_map) / sizeof(nvswitch_status_map[0]);

    for (i = 0; i < limit; i++)
    {
        if (nvswitch_status_map[i].status == status ||
            nvswitch_status_map[i].status == -status)
        {
            err = nvswitch_status_map[i].err;
            break;
        }
    }

    return err;
}

#if !defined(IRQF_SHARED)
#define IRQF_SHARED SA_SHIRQ
#endif

#define NV_FILE_INODE(file) (file)->f_inode

static int nvswitch_probe(struct pci_dev *, const struct pci_device_id *);
static void nvswitch_remove(struct pci_dev *);

static struct pci_device_id nvswitch_pci_table[] =
{
    {
        .vendor      = PCI_VENDOR_ID_NVIDIA,
        .device      = PCI_ANY_ID,
        .subvendor   = PCI_ANY_ID,
        .subdevice   = PCI_ANY_ID,
        .class       = (PCI_CLASS_BRIDGE_OTHER << 8),
        .class_mask  = ~0
    },
    {}
};

static struct pci_driver nvswitch_pci_driver =
{
    .name           = NVSWITCH_DRIVER_NAME,
    .id_table       = nvswitch_pci_table,
    .probe          = nvswitch_probe,
    .remove         = nvswitch_remove,
    .shutdown       = nvswitch_remove
};

//
// nvidia_nvswitch_mknod uses minor number 255 to create nvidia-nvswitchctl
// node. Hence, if NVSWITCH_CTL_MINOR is changed, then NV_NVSWITCH_CTL_MINOR
// should be updated. See nvdia-modprobe-utils.h
//
#define NVSWITCH_CTL_MINOR 255
#define NVSWITCH_MINOR_COUNT (NVSWITCH_CTL_MINOR + 1)

// 32 bit hex value - including 0x prefix. (10 chars)
#define NVSWITCH_REGKEY_VALUE_LEN 10

static char *NvSwitchRegDwords;
module_param(NvSwitchRegDwords, charp, 0);
MODULE_PARM_DESC(NvSwitchRegDwords, "NvSwitch regkey");

static char *NvSwitchBlacklist;
module_param(NvSwitchBlacklist, charp, 0);
MODULE_PARM_DESC(NvSwitchBlacklist, "NvSwitchBlacklist=uuid[,uuid...]");

//
// Locking:
//   We handle nvswitch driver locking in the OS layer. The nvswitch lib
//   layer does not have its own locking. It relies on the OS layer for
//   atomicity.
//
//   All locking is done with sleep locks. We use threaded MSI interrupts to
//   facilitate this.
//
//   When handling a request from a user context we use the interruptible
//   version to enable a quick ^C return if there is lock contention.
//
//   nvswitch.driver_mutex is used to protect driver's global state, "struct
//   NVSWITCH". The driver_mutex is taken during .probe, .remove, .open,
//   .close, and nvswitch-ctl .ioctl operations.
//
//   nvswitch_dev.device_mutex is used to protect per-device state, "struct
//   NVSWITCH_DEV", once a device is opened. The device_mutex is taken during
//   .ioctl, .poll and other background tasks.
//
//   The kernel guarantees that .close won't happen while .ioctl and .poll
//   are going on and without successful .open one can't execute any file ops.
//   This behavior guarantees correctness of the locking model.
//
//   If .close is invoked and holding the lock which is also used by threaded
//   tasks such as interrupt, driver will deadlock while trying to stop such
//   tasks. For example, when threaded interrupts are enabled, free_irq() calls
//   kthread_stop() to flush pending interrupt tasks. The locking model
//   makes sure that such deadlock cases don't happen.
//
// Lock ordering:
//   nvswitch.driver_mutex
//   nvswitch_dev.device_mutex
//
// Note:
//   Due to bug 2856314, nvswitch_dev.device_mutex is taken when calling
//   nvswitch_post_init_device() in nvswitch_probe().
//

// Per-chip driver state is defined in linux_nvswitch.h

// Global driver state
typedef struct
{
    NvBool initialized;
    struct cdev cdev;
    struct cdev cdev_ctl;
    dev_t devno;
    atomic_t count;
    struct mutex driver_mutex;
    struct list_head devices;
} NVSWITCH;

static NVSWITCH nvswitch = {0};

// NvSwitch event
typedef struct nvswitch_event_t
{
    wait_queue_head_t wait_q_event;
    NvBool            event_pending;
} nvswitch_event_t;

typedef struct nvswitch_file_private
{
    NVSWITCH_DEV     *nvswitch_dev;
    nvswitch_event_t file_event;
    struct
    {
        /* A duped file descriptor for fabric_mgmt capability */
        int fabric_mgmt;
    } capability_fds;
} nvswitch_file_private_t;

#define NVSWITCH_SET_FILE_PRIVATE(filp, data) ((filp)->private_data = (data))
#define NVSWITCH_GET_FILE_PRIVATE(filp) ((nvswitch_file_private_t *)(filp)->private_data)

static int nvswitch_device_open(struct inode *inode, struct file *file);
static int nvswitch_device_release(struct inode *inode, struct file *file);
static unsigned int nvswitch_device_poll(struct file *file, poll_table *wait);
static int nvswitch_device_ioctl(struct inode *inode,
                                 struct file *file,
                                 unsigned int cmd,
                                 unsigned long arg);
static long nvswitch_device_unlocked_ioctl(struct file *file,
                                           unsigned int cmd,
                                           unsigned long arg);

static int nvswitch_ctl_ioctl(struct inode *inode,
                              struct file *file,
                              unsigned int cmd,
                              unsigned long arg);
static long nvswitch_ctl_unlocked_ioctl(struct file *file,
                                        unsigned int cmd,
                                        unsigned long arg);

struct file_operations device_fops =
{
    .owner = THIS_MODULE,
    .unlocked_ioctl = nvswitch_device_unlocked_ioctl,
    .open    = nvswitch_device_open,
    .release = nvswitch_device_release,
    .poll    = nvswitch_device_poll
};

struct file_operations ctl_fops =
{
    .owner = THIS_MODULE,
    .unlocked_ioctl = nvswitch_ctl_unlocked_ioctl,
};

static int nvswitch_initialize_device_interrupt(NVSWITCH_DEV *nvswitch_dev);
static void nvswitch_shutdown_device_interrupt(NVSWITCH_DEV *nvswitch_dev);
static void nvswitch_load_bar_info(NVSWITCH_DEV *nvswitch_dev);
static void nvswitch_task_dispatch(NVSWITCH_DEV *nvswitch_dev);

static NvBool
nvswitch_is_device_blacklisted
(
    NVSWITCH_DEV *nvswitch_dev
)
{
    NVSWITCH_DEVICE_FABRIC_STATE device_fabric_state = 0;
    NvlStatus status;

    status = nvswitch_lib_read_fabric_state(nvswitch_dev->lib_device, 
                                            &device_fabric_state, NULL, NULL);

    if (status != NVL_SUCCESS)
    {
        printk(KERN_INFO "%s: Failed to read fabric state, %x\n", nvswitch_dev->name, status);
        return NV_FALSE;
    }

    return device_fabric_state == NVSWITCH_DEVICE_FABRIC_STATE_BLACKLISTED;
}

static void
nvswitch_deinit_background_tasks
(
    NVSWITCH_DEV *nvswitch_dev
)
{
    NV_ATOMIC_SET(nvswitch_dev->task_q_ready, 0);

    wake_up(&nvswitch_dev->wait_q_shutdown);

    nv_kthread_q_stop(&nvswitch_dev->task_q);
}

static int
nvswitch_init_background_tasks
(
    NVSWITCH_DEV *nvswitch_dev
)
{
    int rc;

    rc = nv_kthread_q_init(&nvswitch_dev->task_q, nvswitch_dev->sname);
    if (rc)
    {
        printk(KERN_ERR "%s: Failed to create task queue\n", nvswitch_dev->name);
        return rc;
    }

    NV_ATOMIC_SET(nvswitch_dev->task_q_ready, 1);

    nv_kthread_q_item_init(&nvswitch_dev->task_item,
                           (nv_q_func_t) &nvswitch_task_dispatch,
                           nvswitch_dev);

    if (!nv_kthread_q_schedule_q_item(&nvswitch_dev->task_q,
                                      &nvswitch_dev->task_item))
    {
        printk(KERN_ERR "%s: Failed to schedule an item\n",nvswitch_dev->name);
        rc = -ENODEV;
        goto init_background_task_failed;
    }

    return 0;

init_background_task_failed:
    nvswitch_deinit_background_tasks(nvswitch_dev);

    return rc;
}

static NVSWITCH_DEV*
nvswitch_find_device(int minor)
{
    struct list_head *cur;
    NVSWITCH_DEV *nvswitch_dev = NULL;

    list_for_each(cur, &nvswitch.devices)
    {
        nvswitch_dev = list_entry(cur, NVSWITCH_DEV, list_node);
        if (nvswitch_dev->minor == minor)
        {
            return nvswitch_dev;
        }
    }

    return NULL;
}

static int
nvswitch_find_minor(void)
{
    struct list_head *cur;
    NVSWITCH_DEV *nvswitch_dev;
    int minor;
    int minor_in_use;

    for (minor = 0; minor < NVSWITCH_DEVICE_INSTANCE_MAX; minor++)
    {
        minor_in_use = 0;

        list_for_each(cur, &nvswitch.devices)
        {
            nvswitch_dev = list_entry(cur, NVSWITCH_DEV, list_node);
            if (nvswitch_dev->minor == minor)
            {
                minor_in_use = 1;
                break;
            }
        }

        if (!minor_in_use)
        {
            return minor;
        }
    }

    return NVSWITCH_DEVICE_INSTANCE_MAX;
}

static int
nvswitch_init_i2c_adapters
(
    NVSWITCH_DEV *nvswitch_dev
)
{
    NvlStatus retval;
    NvU32 i, valid_ports_mask;
    struct i2c_adapter *adapter;
    nvswitch_i2c_adapter_entry *adapter_entry;

    if (!nvswitch_lib_is_i2c_supported(nvswitch_dev->lib_device))
    {
        return 0;
    }

    retval = nvswitch_lib_get_valid_ports_mask(nvswitch_dev->lib_device,
                                               &valid_ports_mask);
    if (retval != NVL_SUCCESS)
    {
        printk(KERN_ERR "Failed to get valid I2C ports mask.\n");
        return -ENODEV;
    }

    FOR_EACH_INDEX_IN_MASK(32, i, valid_ports_mask)
    {
        adapter = nvswitch_i2c_add_adapter(nvswitch_dev, i);
        if (adapter == NULL)
        {
            continue;
        }

        adapter_entry = nvswitch_os_malloc(sizeof(*adapter_entry));
        if (adapter_entry == NULL)
        {
            printk(KERN_ERR "Failed to create I2C adapter entry.\n");
            nvswitch_i2c_del_adapter(adapter);
            continue;
        }

        adapter_entry->adapter = adapter;

        list_add_tail(&adapter_entry->entry, &nvswitch_dev->i2c_adapter_list);
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return 0;
}

static void
nvswitch_deinit_i2c_adapters
(
    NVSWITCH_DEV *nvswitch_dev
)
{
    nvswitch_i2c_adapter_entry *curr;
    nvswitch_i2c_adapter_entry *next;

    list_for_each_entry_safe(curr,
                             next,
                             &nvswitch_dev->i2c_adapter_list,
                             entry)
    {
        nvswitch_i2c_del_adapter(curr->adapter);
        list_del(&curr->entry);
        nvswitch_os_free(curr);
    }
}

static int
nvswitch_init_device
(
    NVSWITCH_DEV *nvswitch_dev
)
{
    struct pci_dev *pci_dev = nvswitch_dev->pci_dev;
    NvlStatus retval;
    int rc;

    INIT_LIST_HEAD(&nvswitch_dev->i2c_adapter_list);

    retval = nvswitch_lib_register_device(NV_PCI_DOMAIN_NUMBER(pci_dev),
                                          NV_PCI_BUS_NUMBER(pci_dev),
                                          NV_PCI_SLOT_NUMBER(pci_dev),
                                          PCI_FUNC(pci_dev->devfn),
                                          pci_dev->device,
                                          pci_dev,
                                          nvswitch_dev->minor,
                                          &nvswitch_dev->lib_device);
    if (NVL_SUCCESS != retval)
    {
        printk(KERN_ERR "%s: Failed to register device : %d\n",
               nvswitch_dev->name,
               retval);
        return -ENODEV;
    }

    nvswitch_load_bar_info(nvswitch_dev);

    retval = nvswitch_lib_initialize_device(nvswitch_dev->lib_device);
    if (NVL_SUCCESS != retval)
    {
        printk(KERN_ERR "%s: Failed to initialize device : %d\n",
               nvswitch_dev->name,
               retval);
        rc = -ENODEV;
        goto init_device_failed;
    }

    nvswitch_lib_get_uuid(nvswitch_dev->lib_device, &nvswitch_dev->uuid);

    if (nvswitch_lib_get_bios_version(nvswitch_dev->lib_device,
                                      &nvswitch_dev->bios_ver) != NVL_SUCCESS)
    {
        nvswitch_dev->bios_ver = 0;
    }

    if (nvswitch_lib_get_physid(nvswitch_dev->lib_device,
                                &nvswitch_dev->phys_id) != NVL_SUCCESS)
    {
        nvswitch_dev->phys_id = NVSWITCH_INVALID_PHYS_ID;
    }

    rc = nvswitch_initialize_device_interrupt(nvswitch_dev);
    if (rc)
    {
        printk(KERN_ERR "%s: Failed to initialize interrupt : %d\n",
               nvswitch_dev->name,
               rc);
        goto init_intr_failed;
    }

    if (nvswitch_is_device_blacklisted(nvswitch_dev))
    {
        printk(KERN_ERR "%s: Blacklisted nvswitch device\n", nvswitch_dev->name);
        // Keep device registered for HAL access and Fabric State updates
        return 0;
    }

    nvswitch_lib_enable_interrupts(nvswitch_dev->lib_device);

    return 0;

init_intr_failed:
    nvswitch_lib_shutdown_device(nvswitch_dev->lib_device);

init_device_failed:
    nvswitch_lib_unregister_device(nvswitch_dev->lib_device);
    nvswitch_dev->lib_device = NULL;

    return rc;
}

static int
nvswitch_post_init_device
(
    NVSWITCH_DEV *nvswitch_dev
)
{
    int rc;
    NvlStatus retval;

    rc = nvswitch_init_i2c_adapters(nvswitch_dev);
    if (rc < 0)
    {
       return rc;
    }

    retval = nvswitch_lib_post_init_device(nvswitch_dev->lib_device);
    if (retval != NVL_SUCCESS)
    {
        return -ENODEV;
    }

    return 0;
}

static void
nvswitch_post_init_blacklisted
(
    NVSWITCH_DEV *nvswitch_dev
)
{
    nvswitch_lib_post_init_blacklist_device(nvswitch_dev->lib_device);
}

static void
nvswitch_deinit_device
(
    NVSWITCH_DEV *nvswitch_dev
)
{
    nvswitch_deinit_i2c_adapters(nvswitch_dev);

    nvswitch_lib_disable_interrupts(nvswitch_dev->lib_device);

    nvswitch_shutdown_device_interrupt(nvswitch_dev);

    nvswitch_lib_shutdown_device(nvswitch_dev->lib_device);

    nvswitch_lib_unregister_device(nvswitch_dev->lib_device);
    nvswitch_dev->lib_device = NULL;
}

static void
nvswitch_init_file_event
(
    nvswitch_file_private_t *private
)
{
    init_waitqueue_head(&private->file_event.wait_q_event);
    private->file_event.event_pending = NV_FALSE;
}

//
// Basic device open to support IOCTL interface
//
static int
nvswitch_device_open
(
    struct inode *inode,
    struct file *file
)
{
    NVSWITCH_DEV *nvswitch_dev;
    int rc = 0;
    nvswitch_file_private_t *private = NULL;

    //
    // Get the major/minor device
    // We might want this for routing requests to multiple nvswitches
    //
    printk(KERN_INFO "nvidia-nvswitch%d: open (major=%d)\n",
           MINOR(inode->i_rdev),
           MAJOR(inode->i_rdev));

    rc = mutex_lock_interruptible(&nvswitch.driver_mutex);
    if (rc)
    {
        return rc;
    }

    nvswitch_dev = nvswitch_find_device(MINOR(inode->i_rdev));
    if (!nvswitch_dev)
    {
        rc = -ENODEV;
        goto done;
    }

    if (nvswitch_is_device_blacklisted(nvswitch_dev))
    {
        rc = -ENODEV;
        goto done;
    }

    private = nvswitch_os_malloc(sizeof(*private));
    if (private == NULL)
    {
        rc = -ENOMEM;
        goto done;
    }

    private->nvswitch_dev = nvswitch_dev;

    nvswitch_init_file_event(private);

    private->capability_fds.fabric_mgmt = -1;
    NVSWITCH_SET_FILE_PRIVATE(file, private);

    NV_ATOMIC_INC(nvswitch_dev->ref_count);

done:
    mutex_unlock(&nvswitch.driver_mutex);

    return rc;
}

//
// Basic device release to support IOCTL interface
//
static int
nvswitch_device_release
(
    struct inode *inode,
    struct file *file
)
{
    nvswitch_file_private_t *private = NVSWITCH_GET_FILE_PRIVATE(file);
    NVSWITCH_DEV *nvswitch_dev = private->nvswitch_dev;

    printk(KERN_INFO "nvidia-nvswitch%d: release (major=%d)\n",
           MINOR(inode->i_rdev),
           MAJOR(inode->i_rdev));

    mutex_lock(&nvswitch.driver_mutex);

    nvswitch_lib_remove_client_events(nvswitch_dev->lib_device, (void *)private);

    //
    // If there are no outstanding references and the device is marked
    // unusable, free it.
    //
    if (NV_ATOMIC_DEC_AND_TEST(nvswitch_dev->ref_count) &&
        nvswitch_dev->unusable)
    {
        kfree(nvswitch_dev);
    }

    if (private->capability_fds.fabric_mgmt > 0)
    {
        nvlink_cap_release(private->capability_fds.fabric_mgmt);
        private->capability_fds.fabric_mgmt = -1;
    }

    nvswitch_os_free(file->private_data);
    NVSWITCH_SET_FILE_PRIVATE(file, NULL);

    mutex_unlock(&nvswitch.driver_mutex);

    return 0;
}

static unsigned int
nvswitch_device_poll
(
    struct file *file,
    poll_table *wait
)
{
    nvswitch_file_private_t *private = NVSWITCH_GET_FILE_PRIVATE(file);
    NVSWITCH_DEV *nvswitch_dev = private->nvswitch_dev;
    int rc = 0;
    NvlStatus status;
    struct NVSWITCH_CLIENT_EVENT *client_event;

    rc = mutex_lock_interruptible(&nvswitch_dev->device_mutex);
    if (rc)
    {
        return rc;
    }

    if (nvswitch_dev->unusable)
    {
        printk(KERN_INFO "%s: a stale fd detected\n", nvswitch_dev->name);
        rc = POLLHUP;
        goto done;
    }

    status = nvswitch_lib_get_client_event(nvswitch_dev->lib_device,
                                           (void *) private, &client_event);
    if (status != NVL_SUCCESS)
    {
        printk(KERN_INFO "%s: no events registered for fd\n", nvswitch_dev->name);
        rc = POLLERR;
        goto done;
    }

    poll_wait(file, &private->file_event.wait_q_event, wait);

    if (private->file_event.event_pending)
    {
        rc = POLLPRI | POLLIN;
        private->file_event.event_pending = NV_FALSE;
    }

done:
    mutex_unlock(&nvswitch_dev->device_mutex);

    return rc;
}

typedef struct {
    void *kernel_params;                // Kernel copy of ioctl parameters
    unsigned long kernel_params_size;   // Size of ioctl params according to user
} IOCTL_STATE;

//
// Clean up any dynamically allocated memory for ioctl state
//
static void
nvswitch_ioctl_state_cleanup
(
    IOCTL_STATE *state
)
{
    kfree(state->kernel_params);
    state->kernel_params = NULL;
}

//
// Initialize buffer state for ioctl.
//
// This handles allocating memory and copying user data into kernel space.  The
// ioctl params structure only is supported. Nested data pointers are not handled.
//
// State is maintained in the IOCTL_STATE struct for use by the ioctl, _sync and
// _cleanup calls.
//
static int
nvswitch_ioctl_state_start(IOCTL_STATE *state, int cmd, unsigned long user_arg)
{
    int rc;

    state->kernel_params = NULL;
    state->kernel_params_size = _IOC_SIZE(cmd);

    if (0 == state->kernel_params_size)
    {
        return 0;
    }

    state->kernel_params = kzalloc(state->kernel_params_size, GFP_KERNEL);
    if (NULL == state->kernel_params)
    {
        rc = -ENOMEM;
        goto nvswitch_ioctl_state_start_fail;
    }

    // Copy params to kernel buffers.  Simple _IOR() ioctls can skip this step.
    if (_IOC_DIR(cmd) & _IOC_WRITE)
    {
        rc = copy_from_user(state->kernel_params,
                            (const void *)user_arg,
                            state->kernel_params_size);
        if (rc)
        {
            rc = -EFAULT;
            goto nvswitch_ioctl_state_start_fail;
        }
    }

    return 0;

nvswitch_ioctl_state_start_fail:
    nvswitch_ioctl_state_cleanup(state);
    return rc;
}

//
// Synchronize any ioctl output in the kernel buffers to the user mode buffers.
//
static int
nvswitch_ioctl_state_sync
(
    IOCTL_STATE *state,
    int cmd,
    unsigned long user_arg
)
{
    int rc;

    // Nothing to do if no buffer or write-only ioctl
    if ((0 == state->kernel_params_size) || (0 == (_IOC_DIR(cmd) & _IOC_READ)))
    {
        return 0;
    }

    // Copy params structure back to user mode
    rc = copy_to_user((void *)user_arg,
                      state->kernel_params,
                      state->kernel_params_size);
    if (rc)
    {
        rc = -EFAULT;
    }

    return rc;
}

static int
nvswitch_device_ioctl
(
    struct inode *inode,
    struct file *file,
    unsigned int cmd,
    unsigned long arg
)
{
    nvswitch_file_private_t *private = NVSWITCH_GET_FILE_PRIVATE(file);
    NVSWITCH_DEV *nvswitch_dev = private->nvswitch_dev;
    IOCTL_STATE state = {0};
    NvlStatus retval;
    int rc = 0;

    if (_IOC_TYPE(cmd) != NVSWITCH_DEV_IO_TYPE)
    {
        return -EINVAL;
    }

    rc = mutex_lock_interruptible(&nvswitch_dev->device_mutex);
    if (rc)
    {
        return rc;
    }

    if (nvswitch_dev->unusable)
    {
        printk(KERN_INFO "%s: a stale fd detected\n", nvswitch_dev->name);
        rc = -ENODEV;
        goto nvswitch_device_ioctl_exit;
    }

    if (nvswitch_is_device_blacklisted(nvswitch_dev))
    {
        printk(KERN_INFO "%s: ioctl attempted on blacklisted device\n", nvswitch_dev->name);
        rc = -ENODEV;
        goto nvswitch_device_ioctl_exit;
    }

    rc = nvswitch_ioctl_state_start(&state, cmd, arg);
    if (rc)
    {
        goto nvswitch_device_ioctl_exit;
    }

    retval = nvswitch_lib_ctrl(nvswitch_dev->lib_device,
                               _IOC_NR(cmd),
                               state.kernel_params,
                               state.kernel_params_size,
                               file->private_data);
    rc = nvswitch_map_status(retval);
    if (!rc)
    {
        rc = nvswitch_ioctl_state_sync(&state, cmd, arg);
    }

    nvswitch_ioctl_state_cleanup(&state);

nvswitch_device_ioctl_exit:
    mutex_unlock(&nvswitch_dev->device_mutex);

    return rc;
}

static long
nvswitch_device_unlocked_ioctl
(
    struct file *file,
    unsigned int cmd,
    unsigned long arg
)
{
    return nvswitch_device_ioctl(NV_FILE_INODE(file), file, cmd, arg);
}

static int
nvswitch_ctl_check_version(NVSWITCH_CHECK_VERSION_PARAMS *p)
{
    NvlStatus retval;

    p->is_compatible = 0;
    p->user.version[NVSWITCH_VERSION_STRING_LENGTH - 1] = '\0';

    retval = nvswitch_lib_check_api_version(p->user.version, p->kernel.version,
                                            NVSWITCH_VERSION_STRING_LENGTH);
    if (retval == NVL_SUCCESS)
    {
        p->is_compatible = 1;
    }
    else if (retval == -NVL_ERR_NOT_SUPPORTED)
    {
        printk(KERN_ERR "nvidia-nvswitch: Version mismatch, "
               "kernel version %s user version %s\n",
               p->kernel.version, p->user.version);
    }
    else
    {
        // An unexpected failure
        return nvswitch_map_status(retval);
    }

    return 0;
}

static void
nvswitch_ctl_get_devices(NVSWITCH_GET_DEVICES_PARAMS *p)
{
    int index = 0;
    NVSWITCH_DEV *nvswitch_dev;
    struct list_head *cur;

    BUILD_BUG_ON(NVSWITCH_DEVICE_INSTANCE_MAX != NVSWITCH_MAX_DEVICES);

    list_for_each(cur, &nvswitch.devices)
    {
        nvswitch_dev = list_entry(cur, NVSWITCH_DEV, list_node);
        p->info[index].deviceInstance = nvswitch_dev->minor;
        p->info[index].pciDomain = NV_PCI_DOMAIN_NUMBER(nvswitch_dev->pci_dev);
        p->info[index].pciBus = NV_PCI_BUS_NUMBER(nvswitch_dev->pci_dev);
        p->info[index].pciDevice = NV_PCI_SLOT_NUMBER(nvswitch_dev->pci_dev);
        p->info[index].pciFunction = PCI_FUNC(nvswitch_dev->pci_dev->devfn);
        index++;
    }

    p->deviceCount = index;
}

static void
nvswitch_ctl_get_devices_v2(NVSWITCH_GET_DEVICES_V2_PARAMS *p)
{
    int index = 0;
    NVSWITCH_DEV *nvswitch_dev;
    struct list_head *cur;

    BUILD_BUG_ON(NVSWITCH_DEVICE_INSTANCE_MAX != NVSWITCH_MAX_DEVICES);

    list_for_each(cur, &nvswitch.devices)
    {
        nvswitch_dev = list_entry(cur, NVSWITCH_DEV, list_node);
        p->info[index].deviceInstance = nvswitch_dev->minor;
        memcpy(&p->info[index].uuid, &nvswitch_dev->uuid, sizeof(nvswitch_dev->uuid));
        p->info[index].pciDomain = NV_PCI_DOMAIN_NUMBER(nvswitch_dev->pci_dev);
        p->info[index].pciBus = NV_PCI_BUS_NUMBER(nvswitch_dev->pci_dev);
        p->info[index].pciDevice = NV_PCI_SLOT_NUMBER(nvswitch_dev->pci_dev);
        p->info[index].pciFunction = PCI_FUNC(nvswitch_dev->pci_dev->devfn);
        p->info[index].physId = nvswitch_dev->phys_id;

        if (nvswitch_dev->lib_device != NULL)
        {
            mutex_lock(&nvswitch_dev->device_mutex);
            (void)nvswitch_lib_read_fabric_state(nvswitch_dev->lib_device,
                                                 &p->info[index].deviceState,
                                                 &p->info[index].deviceReason,
                                                 &p->info[index].driverState);

            p->info[index].bTnvlEnabled = nvswitch_lib_is_tnvl_enabled(nvswitch_dev->lib_device);
            mutex_unlock(&nvswitch_dev->device_mutex);
        }
        index++;
    }

    p->deviceCount = index;
}

#define NVSWITCH_CTL_CHECK_PARAMS(type, size) (sizeof(type) == size ? 0 : -EINVAL)

static int
nvswitch_ctl_cmd_dispatch
(
    unsigned int cmd,
    void *params,
    unsigned int param_size
)
{
    int rc;

    switch(cmd)
    {
        case CTRL_NVSWITCH_CHECK_VERSION:
            rc = NVSWITCH_CTL_CHECK_PARAMS(NVSWITCH_CHECK_VERSION_PARAMS,
                                           param_size);
            if (!rc)
            {
                rc = nvswitch_ctl_check_version(params);
            }
            break;
        case CTRL_NVSWITCH_GET_DEVICES:
            rc = NVSWITCH_CTL_CHECK_PARAMS(NVSWITCH_GET_DEVICES_PARAMS,
                                           param_size);
            if (!rc)
            {
                nvswitch_ctl_get_devices(params);
            }
            break;
        case CTRL_NVSWITCH_GET_DEVICES_V2:
            rc = NVSWITCH_CTL_CHECK_PARAMS(NVSWITCH_GET_DEVICES_V2_PARAMS,
                                           param_size);
            if (!rc)
            {
                nvswitch_ctl_get_devices_v2(params);
            }
            break;

        default:
            rc = -EINVAL;
            break;
    }

    return rc;
}

static int
nvswitch_ctl_ioctl
(
    struct inode *inode,
    struct file *file,
    unsigned int cmd,
    unsigned long arg
)
{
    int rc = 0;
    IOCTL_STATE state = {0};

    if (_IOC_TYPE(cmd) != NVSWITCH_CTL_IO_TYPE)
    {
        return -EINVAL;
    }

    rc = mutex_lock_interruptible(&nvswitch.driver_mutex);
    if (rc)
    {
        return rc;
    }

    rc = nvswitch_ioctl_state_start(&state, cmd, arg);
    if (rc)
    {
        goto nvswitch_ctl_ioctl_exit;
    }

    rc = nvswitch_ctl_cmd_dispatch(_IOC_NR(cmd),
                                   state.kernel_params,
                                   state.kernel_params_size);
    if (!rc)
    {
        rc = nvswitch_ioctl_state_sync(&state, cmd, arg);
    }

    nvswitch_ioctl_state_cleanup(&state);

nvswitch_ctl_ioctl_exit:
    mutex_unlock(&nvswitch.driver_mutex);

    return rc;
}

static long
nvswitch_ctl_unlocked_ioctl
(
    struct file *file,
    unsigned int cmd,
    unsigned long arg
)
{
    return nvswitch_ctl_ioctl(NV_FILE_INODE(file), file, cmd, arg);
}

static irqreturn_t
nvswitch_isr_pending
(
    int   irq,
    void *arg
)
{

    NVSWITCH_DEV *nvswitch_dev = (NVSWITCH_DEV *)arg;
    NvlStatus retval;

    //
    // On silicon MSI must be enabled.  Since interrupts will not be shared
    // with MSI, we can simply signal the thread.
    //
    if (nvswitch_dev->irq_mechanism == NVSWITCH_IRQ_MSI)
    {
        return IRQ_WAKE_THREAD;
    }

    if (nvswitch_dev->irq_mechanism == NVSWITCH_IRQ_PIN)
    {
        //
        // We do not take mutex in the interrupt context. The interrupt
        // check is safe to driver state.
        //
        retval = nvswitch_lib_check_interrupts(nvswitch_dev->lib_device);

        // Wake interrupt thread if there is an interrupt pending
        if (-NVL_MORE_PROCESSING_REQUIRED == retval)
        {
            nvswitch_lib_disable_interrupts(nvswitch_dev->lib_device);
            return IRQ_WAKE_THREAD;
        }

        // PCI errors are handled else where.
        if (-NVL_PCI_ERROR == retval)
        {
            return IRQ_NONE;
        }

        if (NVL_SUCCESS != retval)
        {
            pr_err("nvidia-nvswitch: unrecoverable error in ISR\n");
            NVSWITCH_OS_ASSERT(0);
        }
        return IRQ_NONE;
    }

    pr_err("nvidia-nvswitch: unsupported IRQ mechanism in ISR\n");
    NVSWITCH_OS_ASSERT(0);

    return IRQ_NONE;
}

static irqreturn_t
nvswitch_isr_thread
(
    int   irq,
    void *arg
)
{
    NVSWITCH_DEV *nvswitch_dev = (NVSWITCH_DEV *)arg;
    NvlStatus retval;

    mutex_lock(&nvswitch_dev->device_mutex);

    retval = nvswitch_lib_service_interrupts(nvswitch_dev->lib_device);

    wake_up(&nvswitch_dev->wait_q_errors);

    if (nvswitch_dev->irq_mechanism == NVSWITCH_IRQ_PIN)
    {
        nvswitch_lib_enable_interrupts(nvswitch_dev->lib_device);
    }

    mutex_unlock(&nvswitch_dev->device_mutex);

    if (WARN_ON(retval != NVL_SUCCESS))
    {
        printk(KERN_ERR "%s: Interrupts disabled to avoid a storm\n",
               nvswitch_dev->name);
    }

    return IRQ_HANDLED;
}

static void
nvswitch_task_dispatch
(
    NVSWITCH_DEV *nvswitch_dev
)
{
    NvU64 nsec;
    NvU64 timeout;
    NvS64 rc;

    if (NV_ATOMIC_READ(nvswitch_dev->task_q_ready) == 0)
    {
        return;
    }

    mutex_lock(&nvswitch_dev->device_mutex);

    nsec = nvswitch_lib_deferred_task_dispatcher(nvswitch_dev->lib_device);

    mutex_unlock(&nvswitch_dev->device_mutex);

    timeout = usecs_to_jiffies(nsec / NSEC_PER_USEC);

    rc = wait_event_interruptible_timeout(nvswitch_dev->wait_q_shutdown,
                              (NV_ATOMIC_READ(nvswitch_dev->task_q_ready) == 0),
                              timeout);

    //
    // These background tasks should rarely, if ever, get interrupted. We use
    // the "interruptible" variant of wait_event in order to avoid contributing
    // to the system load average (/proc/loadavg), and to avoid softlockup
    // warnings that can occur if a kernel thread lingers too long in an
    // uninterruptible state. If this does get interrupted, we'd like to debug
    // and find out why, so WARN in that case.
    //
    WARN_ON(rc < 0);

    //
    // Schedule a work item only if the above actually timed out or got
    // interrupted, without the condition becoming true.
    //
    if (rc <= 0)
    {
        if (!nv_kthread_q_schedule_q_item(&nvswitch_dev->task_q,
                                          &nvswitch_dev->task_item))
        {
            printk(KERN_ERR "%s: Failed to re-schedule background task\n",
                   nvswitch_dev->name);
        }
    }
}

static int
nvswitch_probe
(
    struct pci_dev *pci_dev,
    const struct pci_device_id *id_table
)
{
    NVSWITCH_DEV *nvswitch_dev = NULL;
    int rc = 0;
    int minor;

    if (!nvswitch_lib_validate_device_id(pci_dev->device))
    {
        return -EINVAL;
    }

    printk(KERN_INFO "nvidia-nvswitch: Probing device %04x:%02x:%02x.%x, "
           "Vendor Id = 0x%x, Device Id = 0x%x, Class = 0x%x \n",
           NV_PCI_DOMAIN_NUMBER(pci_dev),
           NV_PCI_BUS_NUMBER(pci_dev),
           NV_PCI_SLOT_NUMBER(pci_dev),
           PCI_FUNC(pci_dev->devfn),
           pci_dev->vendor,
           pci_dev->device,
           pci_dev->class);

    mutex_lock(&nvswitch.driver_mutex);

    minor = nvswitch_find_minor();
    if (minor >= NVSWITCH_DEVICE_INSTANCE_MAX)
    {
        rc = -ERANGE;
        goto find_minor_failed;
    }

    nvswitch_dev = kzalloc(sizeof(*nvswitch_dev), GFP_KERNEL);
    if (NULL == nvswitch_dev)
    {
        rc = -ENOMEM;
        goto kzalloc_failed;
    }

    mutex_init(&nvswitch_dev->device_mutex);
    init_waitqueue_head(&nvswitch_dev->wait_q_errors);
    init_waitqueue_head(&nvswitch_dev->wait_q_shutdown);

    snprintf(nvswitch_dev->name, sizeof(nvswitch_dev->name),
        NVSWITCH_DRIVER_NAME "%d", minor);

    snprintf(nvswitch_dev->sname, sizeof(nvswitch_dev->sname),
        NVSWITCH_SHORT_NAME "%d", minor);

    rc = pci_enable_device(pci_dev);
    if (rc)
    {
        printk(KERN_ERR "%s: Failed to enable PCI device : %d\n",
               nvswitch_dev->name,
               rc);
        goto pci_enable_device_failed;
    }

    pci_set_master(pci_dev);

    rc = pci_request_regions(pci_dev, nvswitch_dev->name);
    if (rc)
    {
        printk(KERN_ERR "%s: Failed to request memory regions : %d\n",
               nvswitch_dev->name,
               rc);
        goto pci_request_regions_failed;
    }

    nvswitch_dev->bar0 = pci_iomap(pci_dev, 0, 0);
    if (!nvswitch_dev->bar0)
    {
        rc = -ENOMEM;
        printk(KERN_ERR "%s: Failed to map BAR0 region : %d\n",
               nvswitch_dev->name,
               rc);
        goto pci_iomap_failed;
    }

    nvswitch_dev->pci_dev = pci_dev;
    nvswitch_dev->minor = minor;

    rc = nvswitch_init_device(nvswitch_dev);
    if (rc)
    {
        printk(KERN_ERR "%s: Failed to initialize device : %d\n",
               nvswitch_dev->name,
               rc);
        goto init_device_failed;
    }

    if (nvswitch_is_device_blacklisted(nvswitch_dev))
    {
        nvswitch_post_init_blacklisted(nvswitch_dev);
        goto blacklisted;
    }

    //
    // device_mutex held here because post_init entries may call soeService_HAL()
    // with IRQs on. see bug 2856314 for more info
    //
    mutex_lock(&nvswitch_dev->device_mutex);
    rc = nvswitch_post_init_device(nvswitch_dev);
    mutex_unlock(&nvswitch_dev->device_mutex);
    if (rc)
    {
        printk(KERN_ERR "%s:Failed during device post init : %d\n",
               nvswitch_dev->name, rc);
        goto post_init_device_failed;
    }

blacklisted:
    rc = nvswitch_init_background_tasks(nvswitch_dev);
    if (rc)
    {
        printk(KERN_ERR "%s: Failed to initialize background tasks : %d\n",
               nvswitch_dev->name,
               rc);
        goto init_background_task_failed;
    }

    pci_set_drvdata(pci_dev, nvswitch_dev);

    nvswitch_procfs_device_add(nvswitch_dev);

    list_add_tail(&nvswitch_dev->list_node, &nvswitch.devices);

    NV_ATOMIC_INC(nvswitch.count);

    mutex_unlock(&nvswitch.driver_mutex);

    return 0;

init_background_task_failed:
post_init_device_failed:
    nvswitch_deinit_device(nvswitch_dev);

init_device_failed:
    pci_iounmap(pci_dev, nvswitch_dev->bar0);

pci_iomap_failed:
    pci_release_regions(pci_dev);

pci_request_regions_failed:
#ifdef CONFIG_PCI
    pci_clear_master(pci_dev);
#endif
    pci_disable_device(pci_dev);

pci_enable_device_failed:
    kfree(nvswitch_dev);

kzalloc_failed:
find_minor_failed:
    mutex_unlock(&nvswitch.driver_mutex);

    return rc;
}

void
nvswitch_remove
(
    struct pci_dev *pci_dev
)
{
    NVSWITCH_DEV *nvswitch_dev;

    mutex_lock(&nvswitch.driver_mutex);

    nvswitch_dev = pci_get_drvdata(pci_dev);

    if (nvswitch_dev == NULL)
    {
        goto done;
    }

    printk(KERN_INFO "%s: removing device %04x:%02x:%02x.%x\n",
           nvswitch_dev->name,
           NV_PCI_DOMAIN_NUMBER(pci_dev),
           NV_PCI_BUS_NUMBER(pci_dev),
           NV_PCI_SLOT_NUMBER(pci_dev),
           PCI_FUNC(pci_dev->devfn));

    //
    // Synchronize with device operations such as .ioctls/.poll, and then mark
    // the device unusable.
    //
    mutex_lock(&nvswitch_dev->device_mutex);
    nvswitch_dev->unusable = NV_TRUE;
    mutex_unlock(&nvswitch_dev->device_mutex);

    NV_ATOMIC_DEC(nvswitch.count);

    list_del(&nvswitch_dev->list_node);

    nvswitch_deinit_background_tasks(nvswitch_dev);

    nvswitch_deinit_device(nvswitch_dev);

    pci_set_drvdata(pci_dev, NULL);

    pci_iounmap(pci_dev, nvswitch_dev->bar0);

    pci_release_regions(pci_dev);

#ifdef CONFIG_PCI
    pci_clear_master(pci_dev);
#endif

    pci_disable_device(pci_dev);

    nvswitch_procfs_device_remove(nvswitch_dev);

    // Free nvswitch_dev only if it is not in use.
    if (NV_ATOMIC_READ(nvswitch_dev->ref_count) == 0)
    {
        kfree(nvswitch_dev);
    }

done:
    mutex_unlock(&nvswitch.driver_mutex);

    return;
}

static void
nvswitch_load_bar_info
(
    NVSWITCH_DEV *nvswitch_dev
)
{
    struct pci_dev *pci_dev = nvswitch_dev->pci_dev;
    nvlink_pci_info *info;
    NvU32 bar = 0;

    nvswitch_lib_get_device_info(nvswitch_dev->lib_device, &info);

    info->bars[0].offset = NVRM_PCICFG_BAR_OFFSET(0);
    pci_read_config_dword(pci_dev, info->bars[0].offset, &bar);

    info->bars[0].busAddress = (bar & PCI_BASE_ADDRESS_MEM_MASK);
    if (NV_PCI_RESOURCE_FLAGS(pci_dev, 0) & PCI_BASE_ADDRESS_MEM_TYPE_64)
    {
        pci_read_config_dword(pci_dev, info->bars[0].offset + 4, &bar);
        info->bars[0].busAddress |= (((NvU64)bar) << 32);
    }

    info->bars[0].baseAddr = NV_PCI_RESOURCE_START(pci_dev, 0);

    info->bars[0].barSize = NV_PCI_RESOURCE_SIZE(pci_dev, 0);

    info->bars[0].pBar = nvswitch_dev->bar0;
}

static int
_nvswitch_initialize_msix_interrupt
(
    NVSWITCH_DEV *nvswitch_dev
)
{
    // Not supported (bug 3018806)
    return -EINVAL;
}

static int
_nvswitch_initialize_msi_interrupt
(
    NVSWITCH_DEV *nvswitch_dev
)
{
#ifdef CONFIG_PCI_MSI
    struct pci_dev *pci_dev = nvswitch_dev->pci_dev;
    int rc;

    rc = pci_enable_msi(pci_dev);
    if (rc)
    {
        return rc;
    }

    return 0;
#else
    return -EINVAL;
#endif
}

static int
_nvswitch_get_irq_caps(NVSWITCH_DEV *nvswitch_dev, unsigned long *irq_caps)
{
    struct pci_dev *pci_dev;

    if (!nvswitch_dev || !irq_caps)
        return -EINVAL;

    pci_dev = nvswitch_dev->pci_dev;

    if (pci_find_capability(pci_dev, PCI_CAP_ID_MSIX))
        set_bit(NVSWITCH_IRQ_MSIX, irq_caps);

    if (pci_find_capability(pci_dev, PCI_CAP_ID_MSI))
        set_bit(NVSWITCH_IRQ_MSI, irq_caps);

    if (nvswitch_lib_use_pin_irq(nvswitch_dev->lib_device))
        set_bit(NVSWITCH_IRQ_PIN, irq_caps);

    return 0;
}

static int
nvswitch_initialize_device_interrupt
(
    NVSWITCH_DEV *nvswitch_dev
)
{
    struct pci_dev *pci_dev = nvswitch_dev->pci_dev;
    int flags = 0;
    unsigned long irq_caps = 0;
    int rc;

    if (_nvswitch_get_irq_caps(nvswitch_dev, &irq_caps))
    {
        pr_err("%s: failed to retrieve device interrupt capabilities\n",
               nvswitch_dev->name);
        return -EINVAL;
    }

    nvswitch_dev->irq_mechanism = NVSWITCH_IRQ_NONE;

    if (test_bit(NVSWITCH_IRQ_MSIX, &irq_caps))
    {
        rc = _nvswitch_initialize_msix_interrupt(nvswitch_dev);
        if (!rc)
        {
            nvswitch_dev->irq_mechanism = NVSWITCH_IRQ_MSIX;
            pr_info("%s: using MSI-X\n", nvswitch_dev->name);
        }
    }

    if (nvswitch_dev->irq_mechanism == NVSWITCH_IRQ_NONE
        && test_bit(NVSWITCH_IRQ_MSI, &irq_caps))
    {
        rc = _nvswitch_initialize_msi_interrupt(nvswitch_dev);
        if (!rc)
        {
            nvswitch_dev->irq_mechanism = NVSWITCH_IRQ_MSI;
            pr_info("%s: using MSI\n", nvswitch_dev->name);
        }
    }

    if (nvswitch_dev->irq_mechanism == NVSWITCH_IRQ_NONE
        && test_bit(NVSWITCH_IRQ_PIN, &irq_caps))
    {
        flags |= IRQF_SHARED;
        nvswitch_dev->irq_mechanism = NVSWITCH_IRQ_PIN;
        pr_info("%s: using PCI pin\n", nvswitch_dev->name);
    }

    if (nvswitch_dev->irq_mechanism == NVSWITCH_IRQ_NONE)
    {
        pr_err("%s: No supported interrupt mechanism was found. This device supports:\n",
               nvswitch_dev->name);

        if (test_bit(NVSWITCH_IRQ_MSIX, &irq_caps))
            pr_err("%s: MSI-X\n", nvswitch_dev->name);
        if (test_bit(NVSWITCH_IRQ_MSI, &irq_caps))
            pr_err("%s: MSI\n", nvswitch_dev->name);
        if (test_bit(NVSWITCH_IRQ_PIN, &irq_caps))
             pr_err("%s: PCI Pin\n", nvswitch_dev->name);

        return -EINVAL;
    }

    rc = request_threaded_irq(pci_dev->irq,
                              nvswitch_isr_pending,
                              nvswitch_isr_thread,
                              flags, nvswitch_dev->sname,
                              nvswitch_dev);
    if (rc)
    {
#ifdef CONFIG_PCI_MSI
        if (nvswitch_dev->irq_mechanism == NVSWITCH_IRQ_MSI)
        {
            pci_disable_msi(pci_dev);
        }
#endif
        printk(KERN_ERR "%s: failed to get IRQ\n",
               nvswitch_dev->name);

        return rc;
    }

    return 0;
}

void
nvswitch_shutdown_device_interrupt
(
    NVSWITCH_DEV *nvswitch_dev
)
{
    struct pci_dev *pci_dev = nvswitch_dev->pci_dev;

    free_irq(pci_dev->irq, nvswitch_dev);
#ifdef CONFIG_PCI_MSI
    if (nvswitch_dev->irq_mechanism == NVSWITCH_IRQ_MSI)
    {
        pci_disable_msi(pci_dev);
    }
#endif
}

static void
nvswitch_ctl_exit
(
    void
)
{
    cdev_del(&nvswitch.cdev_ctl);
}

static int
nvswitch_ctl_init
(
    int major
)
{
    int rc = 0;
    dev_t nvswitch_ctl = MKDEV(major, NVSWITCH_CTL_MINOR);

    cdev_init(&nvswitch.cdev_ctl, &ctl_fops);

    nvswitch.cdev_ctl.owner = THIS_MODULE;

    rc = cdev_add(&nvswitch.cdev_ctl, nvswitch_ctl, 1);
    if (rc < 0)
    {
        printk(KERN_ERR "nvidia-nvswitch: Unable to create cdev ctl\n");
        return rc;
    }

    return 0;
}

//
// Initialize nvswitch driver SW state.  This is currently called
// from the RM as a backdoor interface, and not by the Linux device
// manager
//
int
nvswitch_init
(
    void
)
{
    int rc;

    if (nvswitch.initialized)
    {
        printk(KERN_ERR "nvidia-nvswitch: Interface already initialized\n");
        return -EBUSY;
    }

    BUILD_BUG_ON(NVSWITCH_DEVICE_INSTANCE_MAX >= NVSWITCH_MINOR_COUNT);

    mutex_init(&nvswitch.driver_mutex);

    INIT_LIST_HEAD(&nvswitch.devices);

    rc = alloc_chrdev_region(&nvswitch.devno,
                             0,
                             NVSWITCH_MINOR_COUNT,
                             NVSWITCH_DRIVER_NAME);
    if (rc < 0)
    {
        printk(KERN_ERR "nvidia-nvswitch: Unable to create cdev region\n");
        goto alloc_chrdev_region_fail;
    }

    printk(KERN_ERR, "nvidia-nvswitch: Major: %d Minor: %d\n",
           MAJOR(nvswitch.devno),
           MINOR(nvswitch.devno));

    cdev_init(&nvswitch.cdev, &device_fops);
    nvswitch.cdev.owner = THIS_MODULE;
    rc = cdev_add(&nvswitch.cdev, nvswitch.devno, NVSWITCH_DEVICE_INSTANCE_MAX);
    if (rc < 0)
    {
        printk(KERN_ERR "nvidia-nvswitch: Unable to create cdev\n");
        goto cdev_add_fail;
    }

    rc = nvswitch_procfs_init();
    if (rc < 0)
    {
        goto nvswitch_procfs_init_fail;
    }

    rc = pci_register_driver(&nvswitch_pci_driver);
    if (rc < 0)
    {
        printk(KERN_ERR "nvidia-nvswitch: Failed to register driver : %d\n", rc);
        goto pci_register_driver_fail;
    }

    rc = nvswitch_ctl_init(MAJOR(nvswitch.devno));
    if (rc < 0)
    {
        goto nvswitch_ctl_init_fail;
    }

    nvswitch.initialized = NV_TRUE;

    return 0;

nvswitch_ctl_init_fail:
    pci_unregister_driver(&nvswitch_pci_driver);

pci_register_driver_fail:
nvswitch_procfs_init_fail:
    cdev_del(&nvswitch.cdev);

cdev_add_fail:
    unregister_chrdev_region(nvswitch.devno, NVSWITCH_MINOR_COUNT);

alloc_chrdev_region_fail:

    return rc;
}

//
// Clean up driver state on exit.  Currently called from RM backdoor call,
// and not by the Linux device manager.
//
void
nvswitch_exit
(
    void
)
{
    if (NV_FALSE == nvswitch.initialized)
    {
        return;
    }

    nvswitch_ctl_exit();

    pci_unregister_driver(&nvswitch_pci_driver);

    nvswitch_procfs_exit();

    cdev_del(&nvswitch.cdev);

    unregister_chrdev_region(nvswitch.devno, NVSWITCH_MINOR_COUNT);

    WARN_ON(!list_empty(&nvswitch.devices));

    nvswitch.initialized = NV_FALSE;
}

//
// Get current time in seconds.nanoseconds
// In this implementation, the time is monotonic time
//
NvU64
nvswitch_os_get_platform_time
(
    void
)
{
    struct timespec64 ts;

    ktime_get_raw_ts64(&ts);
    return (NvU64) timespec64_to_ns(&ts);
}

//
// Get current time in seconds.nanoseconds
// In this implementation, the time is from epoch time
// (midnight UTC of January 1, 1970).
// This implementation cannot be used for polling loops
// due to clock skew during system startup (bug 3302382,
// 3297170, 3273847, 3277478, 200693329).
// Instead, nvswitch_os_get_platform_time() is used
// for polling loops
//
NvU64
nvswitch_os_get_platform_time_epoch
(
    void
)
{
    struct timespec64 ts;

    ktime_get_real_ts64(&ts);
    return (NvU64) timespec64_to_ns(&ts);
}

void
nvswitch_os_print
(
    const int  log_level,
    const char *fmt,
    ...
)
{
    va_list arglist;
    char   *kern_level;
    char    fmt_printk[NVSWITCH_LOG_BUFFER_SIZE];

    switch (log_level)
    {
        case NVSWITCH_DBG_LEVEL_MMIO:
        case NVSWITCH_DBG_LEVEL_NOISY:
            kern_level = KERN_DEBUG;
            break;
        case NVSWITCH_DBG_LEVEL_INFO:
        case NVSWITCH_DBG_LEVEL_SETUP:
            kern_level = KERN_INFO;
            break;
        case NVSWITCH_DBG_LEVEL_WARN:
            kern_level = KERN_WARNING;
            break;
        case NVSWITCH_DBG_LEVEL_ERROR:
            kern_level = KERN_ERR;
            break;
        default:
            kern_level = KERN_DEFAULT;
            break;
    }

    va_start(arglist, fmt);
    snprintf(fmt_printk, sizeof(fmt_printk), "%s%s", kern_level, fmt);
    vprintk(fmt_printk, arglist);
    va_end(arglist);
}

void
nvswitch_os_override_platform
(
    void *os_handle,
    NvBool *rtlsim
)
{
    // Never run on RTL
    *rtlsim = NV_FALSE;
}

NvlStatus
nvswitch_os_read_registery_binary
(
    void *os_handle,
    const char *name,
    NvU8 *data,
    NvU32 length
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvU32
nvswitch_os_get_device_count
(
    void
)
{
    return NV_ATOMIC_READ(nvswitch.count);
}

//
// A helper to convert a string to an unsigned int.
//
// The string should be NULL terminated.
// Only works with base16 values.
//
static int
nvswitch_os_strtouint
(
    char *str,
    unsigned int *data
)
{
    char *p;
    unsigned long long val;

    if (!str || !data)
    {
        return -EINVAL;
    }

    *data = 0;
    val = 0;
    p = str;

    while (*p != '\0')
    {
        if ((tolower(*p) == 'x') && (*str == '0') && (p == str + 1))
        {
            p++;
        }
        else if (*p >='0' && *p <= '9')
        {
            val = val * 16 + (*p - '0');
            p++;
        }
        else if (tolower(*p) >= 'a' && tolower(*p) <= 'f')
        {
            val = val * 16 + (tolower(*p) - 'a' + 10);
            p++;
        }
        else
        {
            return -EINVAL;
        }
    }

    if (val > 0xFFFFFFFF)
    {
        return -EINVAL;
    }

    *data = (unsigned int)val;

    return 0;
}

NvlStatus
nvswitch_os_read_registry_dword
(
    void *os_handle,
    const char *name,
    NvU32 *data
)
{
    char *regkey, *regkey_val_start, *regkey_val_end;
    char regkey_val[NVSWITCH_REGKEY_VALUE_LEN + 1];
    NvU32 regkey_val_len = 0;

    *data = 0;

    if (!NvSwitchRegDwords)
    {
        return -NVL_ERR_GENERIC;
    }

    regkey = strstr(NvSwitchRegDwords, name);
    if (!regkey)
    {
        return -NVL_ERR_GENERIC;
    }

    regkey = strchr(regkey, '=');
    if (!regkey)
    {
        return -NVL_ERR_GENERIC;
    }

    regkey_val_start = regkey + 1;

    regkey_val_end = strchr(regkey, ';');
    if (!regkey_val_end)
    {
        regkey_val_end = strchr(regkey, '\0');
    }

    regkey_val_len = regkey_val_end - regkey_val_start;
    if (regkey_val_len > NVSWITCH_REGKEY_VALUE_LEN || regkey_val_len == 0)
    {
        return -NVL_ERR_GENERIC;
    }

    strncpy(regkey_val, regkey_val_start, regkey_val_len);
    regkey_val[regkey_val_len] = '\0';

    if (nvswitch_os_strtouint(regkey_val, data) != 0)
    {
        return -NVL_ERR_GENERIC;
    }

    return NVL_SUCCESS;
}

static NvBool
_nvswitch_is_space(const char ch)
{
    return ((ch == ' ') || ((ch >= '\t') && (ch <= '\r')));
}

static char *
_nvswitch_remove_spaces(const char *in)
{
    unsigned int len = nvswitch_os_strlen(in) + 1;
    const char *in_ptr;
    char *out, *out_ptr;

    out = nvswitch_os_malloc(len);
    if (out == NULL)
        return NULL;

    in_ptr = in;
    out_ptr = out;

    while (*in_ptr != '\0')
    {
        if (!_nvswitch_is_space(*in_ptr))
            *out_ptr++ = *in_ptr;
        in_ptr++;
    }
    *out_ptr = '\0';

    return out;
}

/*
 * Compare given string UUID with the NvSwitchBlacklist registry parameter string and
 * return whether the UUID is in the NvSwitch blacklist
 */
NvBool
nvswitch_os_is_uuid_in_blacklist
(
    NvUuid *uuid
)
{
    char *list;
    char *ptr;
    char *token;
    NvU8 uuid_string[NVSWITCH_UUID_STRING_LENGTH];

    if (NvSwitchBlacklist == NULL)
        return NV_FALSE;

    if (nvswitch_uuid_to_string(uuid, uuid_string, NVSWITCH_UUID_STRING_LENGTH) == 0)
        return NV_FALSE;

    if ((list = _nvswitch_remove_spaces(NvSwitchBlacklist)) == NULL)
        return NV_FALSE;

    ptr = list;

    while ((token = strsep(&ptr, ",")) != NULL)
    {
        if (strcmp(token, uuid_string) == 0)
        {
            nvswitch_os_free(list);
            return NV_TRUE;
        }
    }
    nvswitch_os_free(list);
    return NV_FALSE;
}


NvlStatus
nvswitch_os_alloc_contig_memory
(
    void *os_handle,
    void **virt_addr,
    NvU32 size,
    NvBool force_dma32
)
{
    NvU32 gfp_flags;
    unsigned long nv_gfp_addr = 0;

    if (!virt_addr)
        return -NVL_BAD_ARGS;

    gfp_flags = GFP_KERNEL | (force_dma32 ? GFP_DMA32 : 0);
    NV_GET_FREE_PAGES(nv_gfp_addr, get_order(size), gfp_flags);

    if(!nv_gfp_addr)
    {
        pr_err("nvidia-nvswitch: unable to allocate kernel memory\n");
        return -NVL_NO_MEM;
    }

    *virt_addr = (void *)nv_gfp_addr;

    return NVL_SUCCESS;
}

void
nvswitch_os_free_contig_memory
(
    void *os_handle,
    void *virt_addr,
    NvU32 size
)
{
    NV_FREE_PAGES((unsigned long)virt_addr, get_order(size));
}

static inline int
_nvswitch_to_pci_dma_direction
(
    NvU32 direction
)
{
    if (direction == NVSWITCH_DMA_DIR_TO_SYSMEM)
        return DMA_FROM_DEVICE;
    else if (direction == NVSWITCH_DMA_DIR_FROM_SYSMEM)
        return DMA_TO_DEVICE;
    else
        return DMA_BIDIRECTIONAL;
}

NvlStatus
nvswitch_os_map_dma_region
(
    void *os_handle,
    void *cpu_addr,
    NvU64 *dma_handle,
    NvU32 size,
    NvU32 direction
)
{
    int dma_dir;
    struct pci_dev *pdev = (struct pci_dev *)os_handle;

    if (!pdev || !cpu_addr || !dma_handle)
        return -NVL_BAD_ARGS;

    dma_dir = _nvswitch_to_pci_dma_direction(direction);

    *dma_handle = (NvU64)dma_map_single(&pdev->dev, cpu_addr, size, dma_dir);

    if (dma_mapping_error(&pdev->dev, *dma_handle))
    {
        pr_err("nvidia-nvswitch: unable to create PCI DMA mapping\n");
        return -NVL_ERR_GENERIC;
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_os_unmap_dma_region
(
    void *os_handle,
    void *cpu_addr,
    NvU64 dma_handle,
    NvU32 size,
    NvU32 direction
)
{
    int dma_dir;
    struct pci_dev *pdev = (struct pci_dev *)os_handle;

    if (!pdev || !cpu_addr)
        return -NVL_BAD_ARGS;

    dma_dir = _nvswitch_to_pci_dma_direction(direction);

    dma_unmap_single(&pdev->dev, dma_handle, size, dma_dir);

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_os_set_dma_mask
(
    void *os_handle,
    NvU32 dma_addr_width
)
{
    struct pci_dev *pdev = (struct pci_dev *)os_handle;

    if (!pdev)
        return -NVL_BAD_ARGS;

    if (dma_set_mask(&pdev->dev, DMA_BIT_MASK(dma_addr_width)))
        return -NVL_ERR_GENERIC;

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_os_sync_dma_region_for_cpu
(
    void *os_handle,
    NvU64 dma_handle,
    NvU32 size,
    NvU32 direction
)
{
    int dma_dir;
    struct pci_dev *pdev = (struct pci_dev *)os_handle;

    if (!pdev)
        return -NVL_BAD_ARGS;

    dma_dir = _nvswitch_to_pci_dma_direction(direction);

    dma_sync_single_for_cpu(&pdev->dev, dma_handle, size, dma_dir);

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_os_sync_dma_region_for_device
(
    void *os_handle,
    NvU64 dma_handle,
    NvU32 size,
    NvU32 direction
)
{
    int dma_dir;
    struct pci_dev *pdev = (struct pci_dev *)os_handle;

    if (!pdev)
        return -NVL_BAD_ARGS;

    dma_dir = _nvswitch_to_pci_dma_direction(direction);

    dma_sync_single_for_device(&pdev->dev, dma_handle, size, dma_dir);

    return NVL_SUCCESS;
}

static inline void *
_nvswitch_os_malloc
(
    NvLength size
)
{
    void *ptr = NULL;

    if (!NV_MAY_SLEEP())
    {
        if (size <= NVSWITCH_KMALLOC_LIMIT)
        {
            ptr = kmalloc(size, NV_GFP_ATOMIC);
        }
    }
    else
    {
        if (size <= NVSWITCH_KMALLOC_LIMIT)
        {
            ptr = kmalloc(size, NV_GFP_NO_OOM);
        }

        if (ptr == NULL)
        {
            ptr = vmalloc(size);
        }
    }

    return ptr;
}

void *
nvswitch_os_malloc_trace
(
    NvLength size,
    const char *file,
    NvU32 line
)
{
#if defined(NV_MEM_LOGGER)
    void *ptr = _nvswitch_os_malloc(size);
    if (ptr)
    {
        nv_memdbg_add(ptr, size, file, line);
    }

    return ptr;
#else
    return _nvswitch_os_malloc(size);
#endif
}

static inline void
_nvswitch_os_free
(
    void *ptr
)
{
    if (!ptr)
        return;

    if (is_vmalloc_addr(ptr))
    {
        vfree(ptr);
    }
    else
    {
        kfree(ptr);
    }
}

void
nvswitch_os_free
(
    void *ptr
)
{
#if defined (NV_MEM_LOGGER)
    if (ptr == NULL)
        return;

    nv_memdbg_remove(ptr, 0, NULL, 0);

    return _nvswitch_os_free(ptr);
#else
    return _nvswitch_os_free(ptr);
#endif
}

NvLength
nvswitch_os_strlen
(
    const char *str
)
{
    return strlen(str);
}

char*
nvswitch_os_strncpy
(
    char *dest,
    const char *src,
    NvLength length
)
{
    return strncpy(dest, src, length);
}

int
nvswitch_os_strncmp
(
    const char *s1,
    const char *s2,
    NvLength length
)
{
    return strncmp(s1, s2, length);
}

char*
nvswitch_os_strncat
(
    char *s1,
    const char *s2,
    NvLength length
)
{
    return strncat(s1, s2, length);
}

void *
nvswitch_os_memset
(
    void *dest,
    int value,
    NvLength size
)
{
     return memset(dest, value, size);
}

void *
nvswitch_os_memcpy
(
    void *dest,
    const void *src,
    NvLength size
)
{
    return memcpy(dest, src, size);
}

int
nvswitch_os_memcmp
(
    const void *s1,
    const void *s2,
    NvLength size
)
{
    return memcmp(s1, s2, size);
}

NvU32
nvswitch_os_mem_read32
(
    const volatile void * address
)
{
    return (*(const volatile NvU32*)(address));
}

void
nvswitch_os_mem_write32
(
    volatile void *address,
    NvU32 data
)
{
    (*(volatile NvU32 *)(address)) = data;
}

NvU64
nvswitch_os_mem_read64
(
    const volatile void * address
)
{
    return (*(const volatile NvU64 *)(address));
}

void
nvswitch_os_mem_write64
(
    volatile void *address,
    NvU64 data
)
{
    (*(volatile NvU64 *)(address)) = data;
}

int
nvswitch_os_snprintf
(
    char *dest,
    NvLength size,
    const char *fmt,
    ...
)
{
    va_list arglist;
    int chars_written;

    va_start(arglist, fmt);
    chars_written = vsnprintf(dest, size, fmt, arglist);
    va_end(arglist);

    return chars_written;
}

int
nvswitch_os_vsnprintf
(
    char *buf,
    NvLength size,
    const char *fmt,
    va_list arglist
)
{
    return vsnprintf(buf, size, fmt, arglist);
}

void
nvswitch_os_assert_log
(
    const char *fmt,
    ...
)
{
    if (printk_ratelimit())
    {
        va_list arglist;
        char fmt_printk[NVSWITCH_LOG_BUFFER_SIZE];

        va_start(arglist, fmt);
        vsnprintf(fmt_printk, sizeof(fmt_printk), fmt, arglist);
        va_end(arglist);
        nvswitch_os_print(NVSWITCH_DBG_LEVEL_ERROR, fmt_printk);
        WARN_ON(1);
     }
     dbg_breakpoint();
}

/*
 * Sleep for specified milliseconds. Yields the CPU to scheduler.
 */
void
nvswitch_os_sleep
(
    unsigned int ms
)
{
    NV_STATUS status;
    status = nv_sleep_ms(ms);

    if (status != NV_OK)
    {
        if (printk_ratelimit())
        {
            nvswitch_os_print(NVSWITCH_DBG_LEVEL_ERROR, "NVSwitch: requested"
                              " sleep duration %d msec exceeded %d msec\n",
                              ms, NV_MAX_ISR_DELAY_MS);
            WARN_ON(1);
        }
    }
}

NvlStatus
nvswitch_os_acquire_fabric_mgmt_cap
(
    void *osPrivate,
    NvU64 capDescriptor
)
{
    int dup_fd = -1;
    nvswitch_file_private_t *private_data = (nvswitch_file_private_t *)osPrivate;

    if (private_data == NULL)
    {
        return -NVL_BAD_ARGS;
    }

    dup_fd = nvlink_cap_acquire((int)capDescriptor,
                                NVLINK_CAP_FABRIC_MANAGEMENT);
    if (dup_fd < 0)
    {
        return -NVL_ERR_OPERATING_SYSTEM;
    }

    private_data->capability_fds.fabric_mgmt = dup_fd;
    return NVL_SUCCESS;
}

int
nvswitch_os_is_fabric_manager
(
    void *osPrivate
)
{
    nvswitch_file_private_t *private_data = (nvswitch_file_private_t *)osPrivate;

    /* Make sure that fabric mgmt capbaility fd is valid */
    if ((private_data == NULL) ||
        (private_data->capability_fds.fabric_mgmt < 0))
    {
        return 0;
    }

    return 1;
}

int
nvswitch_os_is_admin
(
    void
)
{
    return NV_IS_SUSER();
}

#define NV_KERNEL_RELEASE    ((LINUX_VERSION_CODE >> 16) & 0x0ff)
#define NV_KERNEL_VERSION    ((LINUX_VERSION_CODE >> 8)  & 0x0ff)
#define NV_KERNEL_SUBVERSION ((LINUX_VERSION_CODE)       & 0x0ff)

NvlStatus
nvswitch_os_get_os_version
(
    NvU32 *pMajorVer,
    NvU32 *pMinorVer,
    NvU32 *pBuildNum
)
{
    if (pMajorVer)
        *pMajorVer = NV_KERNEL_RELEASE;
    if (pMinorVer)
        *pMinorVer = NV_KERNEL_VERSION;
    if (pBuildNum)
        *pBuildNum = NV_KERNEL_SUBVERSION;

    return NVL_SUCCESS;
}

/*!
 * @brief: OS specific handling to add an event.
 */
NvlStatus
nvswitch_os_add_client_event
(
    void            *osHandle,
    void            *osPrivate,
    NvU32           eventId
)
{
    return NVL_SUCCESS;
}

/*!
 * @brief: OS specific handling to remove all events corresponding to osPrivate.
 */
NvlStatus
nvswitch_os_remove_client_event
(
    void            *osHandle,
    void            *osPrivate
)
{
    return NVL_SUCCESS;
}

/*!
 * @brief: OS specific handling to notify an event.
 */
NvlStatus
nvswitch_os_notify_client_event
(
    void *osHandle,
    void *osPrivate,
    NvU32 eventId
)
{
    nvswitch_file_private_t *private_data = (nvswitch_file_private_t *)osPrivate;

    if (private_data == NULL)
    {
        return -NVL_BAD_ARGS;
    }

    private_data->file_event.event_pending = NV_TRUE;
    wake_up_interruptible(&private_data->file_event.wait_q_event);

    return NVL_SUCCESS;
}

/*!
 * @brief: Gets OS specific support for the REGISTER_EVENTS ioctl
 */
NvlStatus
nvswitch_os_get_supported_register_events_params
(
    NvBool *many_events,
    NvBool *os_descriptor
)
{
    *many_events   = NV_FALSE;
    *os_descriptor = NV_FALSE;
    return NVL_SUCCESS;
}

NvlStatus
nvswitch_os_get_pid
(
    NvU32 *pPid
)
{
    if (pPid != NULL)
    {
        *pPid = task_pid_nr(current);
    }
    
    return NVL_SUCCESS;
}
