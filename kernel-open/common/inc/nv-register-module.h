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


#ifndef _NV_REGISTER_MODULE_H_
#define _NV_REGISTER_MODULE_H_

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/poll.h>

#include "nvtypes.h"

typedef struct nvidia_module_s {
    struct module *owner;

    /* nvidia0, nvidia1 ..*/
    const char *module_name;

    /* module instance */
    NvU32 instance;

    /* file operations */
    int (*open)(struct inode *, struct file *filp);
    int (*close)(struct inode *, struct file *filp);
    int (*mmap)(struct file *filp, struct vm_area_struct *vma);
    int (*ioctl)(struct inode *, struct file * file, unsigned int cmd, unsigned long arg);
    unsigned int (*poll)(struct file * file, poll_table *wait);

} nvidia_module_t;

int nvidia_register_module(nvidia_module_t *);
int nvidia_unregister_module(nvidia_module_t *);

#endif
