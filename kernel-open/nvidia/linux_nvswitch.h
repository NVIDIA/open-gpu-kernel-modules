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

#ifndef LINUX_NVSWITCH_H
#define LINUX_NVSWITCH_H

#include "nvmisc.h"
#include "nv-linux.h"
#include "nv-kthread-q.h"
#include "export_nvswitch.h"

#define NVSWITCH_SHORT_NAME "nvswi"

#define NVSWITCH_IRQ_NONE 0
#define NVSWITCH_IRQ_MSIX 1
#define NVSWITCH_IRQ_MSI  2
#define NVSWITCH_IRQ_PIN  3

#define NVSWITCH_OS_ASSERT(_cond)                                                       \
    do {                                                                                \
        if (!(_cond)) {                                                                 \
            nvswitch_os_assert_log("NVSwitch: Assertion failed in %s() at %s:%d\n",     \
            __FUNCTION__ , __FILE__, __LINE__);                                         \
        }                                                                               \
    } while(0)

#define NVSWITCH_KMALLOC_LIMIT (128 * 1024)

#define nvswitch_os_malloc(_size)        nvswitch_os_malloc_trace(_size, __FILE__, __LINE__)

typedef struct
{
    struct list_head entry;
    struct i2c_adapter *adapter;
} nvswitch_i2c_adapter_entry;

// Per-chip driver state
typedef struct
{
    char name[sizeof(NVSWITCH_DRIVER_NAME) + 4];
    char sname[sizeof(NVSWITCH_SHORT_NAME) + 4];  /* short name */
    int minor;
    NvUuid uuid;
    struct mutex device_mutex;
    nvswitch_device *lib_device;                  /* nvswitch library device */
    wait_queue_head_t wait_q_errors;
    void *bar0;
    struct nv_kthread_q task_q;                   /* Background task queue */
    struct nv_kthread_q_item task_item;           /* Background dispatch task */
    atomic_t task_q_ready;
    wait_queue_head_t wait_q_shutdown;
    struct pci_dev *pci_dev;
    atomic_t ref_count;
    struct list_head list_node;
    NvBool unusable;
    NvU32 phys_id;
    NvU64 bios_ver;
#if defined(CONFIG_PROC_FS)
    struct proc_dir_entry *procfs_dir;
#endif
    NvU8 irq_mechanism;
    struct list_head i2c_adapter_list;
} NVSWITCH_DEV;


int nvswitch_map_status(NvlStatus status);
int nvswitch_procfs_init(void);
void nvswitch_procfs_exit(void);
int nvswitch_procfs_device_add(NVSWITCH_DEV *nvswitch_dev);
void nvswitch_procfs_device_remove(NVSWITCH_DEV *nvswitch_dev);
struct i2c_adapter *nvswitch_i2c_add_adapter(NVSWITCH_DEV *nvswitch_dev, NvU32 port);
void nvswitch_i2c_del_adapter(struct i2c_adapter *adapter);

#endif // LINUX_NVSWITCH_H
