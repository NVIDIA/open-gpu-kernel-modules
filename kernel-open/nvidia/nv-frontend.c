/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2013 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "os-interface.h"
#include "nv-linux.h"
#include "nv-reg.h"
#include "nv-frontend.h"

#if defined(MODULE_LICENSE)

MODULE_LICENSE("Dual MIT/GPL");



#endif
#if defined(MODULE_INFO)
MODULE_INFO(supported, "external");
#endif
#if defined(MODULE_VERSION)
MODULE_VERSION(NV_VERSION_STRING);
#endif

#ifdef MODULE_ALIAS_CHARDEV_MAJOR
MODULE_ALIAS_CHARDEV_MAJOR(NV_MAJOR_DEVICE_NUMBER);
#endif

/*
 * MODULE_IMPORT_NS() is added by commit id 8651ec01daeda
 * ("module: add support for symbol namespaces") in 5.4
 */
#if defined(MODULE_IMPORT_NS)


/*
 * DMA_BUF namespace is added by commit id 16b0314aa746
 * ("dma-buf: move dma-buf symbols into the DMA_BUF module namespace") in 5.16
 */
MODULE_IMPORT_NS(DMA_BUF);


#endif

static NvU32 nv_num_instances;

// lock required to protect table.
struct semaphore nv_module_table_lock;

// minor number table
nvidia_module_t *nv_minor_num_table[NV_FRONTEND_CONTROL_DEVICE_MINOR_MAX + 1];

int nvidia_init_module(void);
void nvidia_exit_module(void);

/* EXPORTS to Linux Kernel */

int          nvidia_frontend_open(struct inode *, struct file *);
int          nvidia_frontend_close(struct inode *, struct file *);
unsigned int nvidia_frontend_poll(struct file *, poll_table *);
int          nvidia_frontend_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
long         nvidia_frontend_unlocked_ioctl(struct file *, unsigned int, unsigned long);
long         nvidia_frontend_compat_ioctl(struct file *, unsigned int, unsigned long);
int          nvidia_frontend_mmap(struct file *, struct vm_area_struct *);

/* character driver entry points */
static struct file_operations nv_frontend_fops = {
    .owner     = THIS_MODULE,
    .poll      = nvidia_frontend_poll,
#if defined(NV_FILE_OPERATIONS_HAS_IOCTL)
    .ioctl     = nvidia_frontend_ioctl,
#endif
    .unlocked_ioctl = nvidia_frontend_unlocked_ioctl,
#if NVCPU_IS_X86_64 || NVCPU_IS_AARCH64
    .compat_ioctl = nvidia_frontend_compat_ioctl,
#endif
    .mmap      = nvidia_frontend_mmap,
    .open      = nvidia_frontend_open,
    .release   = nvidia_frontend_close,
};

/* Helper functions */

static int add_device(nvidia_module_t *module, nv_linux_state_t *device, NvBool all)
{
    NvU32 i;
    int rc = -1;

    // look for free a minor number and assign unique minor number to this device
    for (i = 0; i <= NV_FRONTEND_CONTROL_DEVICE_MINOR_MIN; i++)
    {
        if (nv_minor_num_table[i] == NULL)
        {
            nv_minor_num_table[i] = module;
            device->minor_num = i;
            if (all == NV_TRUE)
            {
                device = device->next;
                if (device == NULL)
                {
                    rc = 0;
                    break;
                }
            }
            else
            {
                rc = 0;
                break;
            }
        }
    }
    return rc;
}

static int remove_device(nvidia_module_t *module, nv_linux_state_t *device)
{
    int rc = -1;

    // remove this device from minor_number table
    if ((device != NULL) && (nv_minor_num_table[device->minor_num] != NULL))
    {
        nv_minor_num_table[device->minor_num] = NULL;
        device->minor_num = 0;
        rc = 0;
    }
    return rc;
}

/* Export functions */

int nvidia_register_module(nvidia_module_t *module)
{
    int rc = 0;
    NvU32 ctrl_minor_num;

    down(&nv_module_table_lock);
    if (module->instance >= NV_MAX_MODULE_INSTANCES)
    {
        printk("NVRM: NVIDIA module instance %d registration failed.\n",
                module->instance);
        rc = -EINVAL;
        goto done;
    }

    ctrl_minor_num = NV_FRONTEND_CONTROL_DEVICE_MINOR_MAX - module->instance;
    nv_minor_num_table[ctrl_minor_num] = module;
    nv_num_instances++;
done:
    up(&nv_module_table_lock);

    return rc;
}
EXPORT_SYMBOL(nvidia_register_module);

int nvidia_unregister_module(nvidia_module_t *module)
{
    int rc = 0;
    NvU32 ctrl_minor_num;

    down(&nv_module_table_lock);

    ctrl_minor_num = NV_FRONTEND_CONTROL_DEVICE_MINOR_MAX - module->instance;
    if (nv_minor_num_table[ctrl_minor_num] == NULL)
    {
        printk("NVRM: NVIDIA module for %d instance does not exist\n",
                module->instance);
        rc = -1;
    }
    else
    {
        nv_minor_num_table[ctrl_minor_num] = NULL;
        nv_num_instances--;
    }

    up(&nv_module_table_lock);

    return rc;
}
EXPORT_SYMBOL(nvidia_unregister_module);

int nvidia_frontend_add_device(nvidia_module_t *module, nv_linux_state_t * device)
{
    int rc = -1;
    NvU32 ctrl_minor_num;

    down(&nv_module_table_lock);
    ctrl_minor_num = NV_FRONTEND_CONTROL_DEVICE_MINOR_MAX - module->instance;
    if (nv_minor_num_table[ctrl_minor_num] == NULL)
    {
        printk("NVRM: NVIDIA module for %d instance does not exist\n",
                module->instance);
        rc = -1;
    }
    else
    {
        rc = add_device(module, device, NV_FALSE);
    }
    up(&nv_module_table_lock);

    return rc;
}
EXPORT_SYMBOL(nvidia_frontend_add_device);

int nvidia_frontend_remove_device(nvidia_module_t *module, nv_linux_state_t * device)
{
    int rc = 0;
    NvU32 ctrl_minor_num;

    down(&nv_module_table_lock);
    ctrl_minor_num = NV_FRONTEND_CONTROL_DEVICE_MINOR_MAX - module->instance;
    if (nv_minor_num_table[ctrl_minor_num] == NULL)
    {
        printk("NVRM: NVIDIA module for %d instance does not exist\n",
                module->instance);
        rc = -1;
    }
    else
    {
        rc = remove_device(module, device);
    }
    up(&nv_module_table_lock);

    return rc;
}
EXPORT_SYMBOL(nvidia_frontend_remove_device);

int nvidia_frontend_open(
    struct inode *inode,
    struct file *file
)
{
    int rc = -ENODEV;
    nvidia_module_t *module = NULL;

    NvU32 minor_num = NV_FRONTEND_MINOR_NUMBER(inode);

    down(&nv_module_table_lock);
    module = nv_minor_num_table[minor_num];

    if ((module != NULL) && (module->open != NULL))
    {
        // Increment the reference count of module to ensure that module does
        // not get unloaded if its corresponding device file is open, for
        // example nvidiaN.ko should not get unloaded if /dev/nvidiaN is open.
        if (!try_module_get(module->owner))
        {
            up(&nv_module_table_lock);
            return -ENODEV;
        }
        rc = module->open(inode, file);
        if (rc < 0)
        {
            module_put(module->owner);
        }
    }

    up(&nv_module_table_lock);
    return rc;
}

int nvidia_frontend_close(
    struct inode *inode,
    struct file *file
)
{
    int rc = -ENODEV;
    nvidia_module_t *module = NULL;

    NvU32 minor_num = NV_FRONTEND_MINOR_NUMBER(inode);

    module = nv_minor_num_table[minor_num];

    if ((module != NULL) && (module->close != NULL))
    {
        rc = module->close(inode, file);

        // Decrement the reference count of module.
        module_put(module->owner);
    }

    return rc;
}

unsigned int nvidia_frontend_poll(
    struct file *file,
    poll_table *wait
)
{
    unsigned int mask = 0;
    struct inode *inode = NV_FILE_INODE(file);
    NvU32 minor_num = NV_FRONTEND_MINOR_NUMBER(inode);
    nvidia_module_t *module = nv_minor_num_table[minor_num];

    if ((module != NULL) && (module->poll != NULL))
        mask = module->poll(file, wait);

    return mask;
}

int nvidia_frontend_ioctl(
    struct inode *inode,
    struct file *file,
    unsigned int cmd,
    unsigned long i_arg)
{
    int rc = -ENODEV;
    nvidia_module_t *module = NULL;

    NvU32 minor_num = NV_FRONTEND_MINOR_NUMBER(inode);
    module = nv_minor_num_table[minor_num];

    if ((module != NULL) && (module->ioctl != NULL))
        rc = module->ioctl(inode, file, cmd, i_arg);

    return rc;
}

long nvidia_frontend_unlocked_ioctl(
    struct file *file,
    unsigned int cmd,
    unsigned long i_arg
)
{
    return nvidia_frontend_ioctl(NV_FILE_INODE(file), file, cmd, i_arg);
}

long nvidia_frontend_compat_ioctl(
    struct file *file,
    unsigned int cmd,
    unsigned long i_arg
)
{
    return nvidia_frontend_ioctl(NV_FILE_INODE(file), file, cmd, i_arg);
}

int nvidia_frontend_mmap(
    struct file *file,
    struct vm_area_struct *vma
)
{
    int rc = -ENODEV;
    struct inode *inode = NV_FILE_INODE(file);
    NvU32 minor_num = NV_FRONTEND_MINOR_NUMBER(inode);
    nvidia_module_t *module = nv_minor_num_table[minor_num];

    if ((module != NULL) && (module->mmap != NULL))
        rc = module->mmap(file, vma);

    return rc;
}

static int __init nvidia_frontend_init_module(void)
{
    int status = 0;

    // initialise nvidia module table;
    nv_num_instances = 0;
    memset(nv_minor_num_table, 0, sizeof(nv_minor_num_table));
    NV_INIT_MUTEX(&nv_module_table_lock);

    status = nvidia_init_module();
    if (status < 0)
    {
        return status;
    }

    // register char device
    status = register_chrdev(NV_MAJOR_DEVICE_NUMBER, "nvidia-frontend", &nv_frontend_fops);
    if (status < 0)
    {
        printk("NVRM: register_chrdev() failed!\n");
        nvidia_exit_module();
    }

    return status;
}

static void __exit nvidia_frontend_exit_module(void)
{
    /*
     * If this is the last nvidia_module to be unregistered, cleanup and
     * unregister char dev
     */
    if (nv_num_instances == 1)
    {
        unregister_chrdev(NV_MAJOR_DEVICE_NUMBER, "nvidia-frontend");
    }

    nvidia_exit_module();
}

module_init(nvidia_frontend_init_module);
module_exit(nvidia_frontend_exit_module);

