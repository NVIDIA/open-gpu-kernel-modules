/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "nv-procfs.h"

#include <linux/fs.h>

#if defined(CONFIG_PROC_FS)

#define NV_DEFINE_SINGLE_NVSWITCH_PROCFS_FILE(name) \
    NV_DEFINE_SINGLE_PROCFS_FILE_READ_ONLY(name, nv_system_pm_lock)

#define NVSWITCH_PROCFS_DIR "driver/nvidia-nvswitch"

static struct proc_dir_entry *nvswitch_procfs_dir;
static struct proc_dir_entry *nvswitch_permissions;
static struct proc_dir_entry *nvswitch_procfs_devices;

static int
nv_procfs_read_permissions
(
    struct seq_file *s,
    void *v
)
{
    // Restrict device node permissions - 0666. Used by nvidia-modprobe.
    seq_printf(s, "%s: %u\n", "DeviceFileMode", 438);

    return 0;
}

NV_DEFINE_SINGLE_NVSWITCH_PROCFS_FILE(permissions);

static int
nv_procfs_read_device_info
(
    struct seq_file *s,
    void *v
)
{
    NVSWITCH_DEV *nvswitch_dev = s->private;

    if (!nvswitch_dev)
    {
        NVSWITCH_OS_ASSERT(0);
        return -EFAULT;
    }

    seq_printf(s, "BIOS Version: ");

    if (nvswitch_dev->bios_ver)
    {
        seq_printf(s, "%02llx.%02llx.%02llx.%02llx.%02llx\n",
                       nvswitch_dev->bios_ver >> 32,
                       (nvswitch_dev->bios_ver >> 24) & 0xFF,
                       (nvswitch_dev->bios_ver >> 16) & 0xFF,
                       (nvswitch_dev->bios_ver >> 8) & 0xFF,
                       nvswitch_dev->bios_ver & 0xFF);
    }
    else
    {
        seq_printf(s, "N/A\n");
    }

    return 0;
}

NV_DEFINE_SINGLE_NVSWITCH_PROCFS_FILE(device_info);

void
nvswitch_procfs_device_remove
(
    NVSWITCH_DEV *nvswitch_dev
)
{
    if (!nvswitch_dev || !nvswitch_dev->procfs_dir)
    {
        NVSWITCH_OS_ASSERT(0);
        return;
    }

    proc_remove(nvswitch_dev->procfs_dir);
    nvswitch_dev->procfs_dir = NULL;
}

int
nvswitch_procfs_device_add
(
    NVSWITCH_DEV *nvswitch_dev
)
{
    struct pci_dev *pci_dev;
    struct proc_dir_entry *device_dir, *entry;
    char name[32];

    if (!nvswitch_dev || !nvswitch_dev->pci_dev)
    {
        NVSWITCH_OS_ASSERT(0);
        return -1;
    }

    pci_dev = nvswitch_dev->pci_dev;

    snprintf(name, sizeof(name), "%04x:%02x:%02x.%1x",
             NV_PCI_DOMAIN_NUMBER(pci_dev), NV_PCI_BUS_NUMBER(pci_dev),
             NV_PCI_SLOT_NUMBER(pci_dev), PCI_FUNC(pci_dev->devfn));

    device_dir = NV_CREATE_PROC_DIR(name, nvswitch_procfs_devices);
    if (!device_dir)
        return -1;

    nvswitch_dev->procfs_dir = device_dir;

    entry = NV_CREATE_PROC_FILE("information", device_dir, device_info,
                                nvswitch_dev);
    if (!entry)
        goto failed;

    return 0;

failed:
    nvswitch_procfs_device_remove(nvswitch_dev);
    return -1;
}

void
nvswitch_procfs_exit
(
    void
)
{
    if (!nvswitch_procfs_dir)
    {
        return;
    }

    proc_remove(nvswitch_procfs_dir);
    nvswitch_procfs_dir = NULL;
}

int
nvswitch_procfs_init
(
    void
)
{
    nvswitch_procfs_dir = NV_CREATE_PROC_DIR(NVSWITCH_PROCFS_DIR, NULL);
    if (!nvswitch_procfs_dir)
    {
        return -EACCES;
    }

    nvswitch_permissions = NV_CREATE_PROC_FILE("permissions",
                                               nvswitch_procfs_dir,
                                               permissions,
                                               NULL);
    if (!nvswitch_permissions)
    {
        goto cleanup;
    }

    nvswitch_procfs_devices = NV_CREATE_PROC_DIR("devices", nvswitch_procfs_dir);
    if (!nvswitch_procfs_devices)
    {
        goto cleanup;
    }

    return 0;

cleanup:

    nvswitch_procfs_exit();

    return -EACCES;
}

#else // !CONFIG_PROC_FS

int nvswitch_procfs_init(void) { return 0; }
void nvswitch_procfs_exit(void) { }
int nvswitch_procfs_device_add(NVSWITCH_DEV *nvswitch_dev) { return 0; }
void nvswitch_procfs_device_remove(NVSWITCH_DEV *nvswitch_dev) { }

#endif // CONFIG_PROC_FS
