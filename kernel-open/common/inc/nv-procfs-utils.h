/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _NV_PROCFS_UTILS_H
#define _NV_PROCFS_UTILS_H

#include "conftest.h"

#ifdef CONFIG_PROC_FS
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

/*
 * Allow procfs to create file to exercise error forwarding.
 * This is supported by CRAY platforms.
 */
#if defined(CONFIG_CRAY_XT)
#define EXERCISE_ERROR_FORWARDING NV_TRUE
#else
#define EXERCISE_ERROR_FORWARDING NV_FALSE
#endif

#define IS_EXERCISE_ERROR_FORWARDING_ENABLED() (EXERCISE_ERROR_FORWARDING)

#if defined(NV_PROC_OPS_PRESENT)
typedef struct proc_ops nv_proc_ops_t;

#define NV_PROC_OPS_SET_OWNER()

#define NV_PROC_OPS_OPEN    proc_open
#define NV_PROC_OPS_READ    proc_read
#define NV_PROC_OPS_WRITE   proc_write
#define NV_PROC_OPS_LSEEK   proc_lseek
#define NV_PROC_OPS_RELEASE proc_release
#else
typedef struct file_operations nv_proc_ops_t;

#define NV_PROC_OPS_SET_OWNER() .owner = THIS_MODULE,

#define NV_PROC_OPS_OPEN    open
#define NV_PROC_OPS_READ    read
#define NV_PROC_OPS_WRITE   write
#define NV_PROC_OPS_LSEEK   llseek
#define NV_PROC_OPS_RELEASE release
#endif

#define NV_CREATE_PROC_FILE(filename,parent,__name,__data)               \
   ({                                                                    \
        struct proc_dir_entry *__entry;                                  \
        int mode = (S_IFREG | S_IRUGO);                                  \
        const nv_proc_ops_t *fops = &nv_procfs_##__name##_fops;          \
        if (fops->NV_PROC_OPS_WRITE != 0)                                \
            mode |= S_IWUSR;                                             \
        __entry = proc_create_data(filename, mode, parent, fops, __data);\
        __entry;                                                         \
    })

# define NV_PROC_MKDIR_MODE(name, mode, parent)                \
    proc_mkdir_mode(name, mode, parent)

#define NV_CREATE_PROC_DIR(name,parent)                        \
   ({                                                          \
        struct proc_dir_entry *__entry;                        \
        int mode = (S_IFDIR | S_IRUGO | S_IXUGO);              \
        __entry = NV_PROC_MKDIR_MODE(name, mode, parent);      \
        __entry;                                               \
    })

#if defined(NV_PDE_DATA_LOWER_CASE_PRESENT)
#define NV_PDE_DATA(inode) pde_data(inode)
#else
#define NV_PDE_DATA(inode) PDE_DATA(inode)
#endif

#define NV_DEFINE_SINGLE_PROCFS_FILE_HELPER(name, lock)                     \
    static ssize_t nv_procfs_read_lock_##name(                              \
        struct file *file,                                                  \
        char __user *buf,                                                   \
        size_t size,                                                        \
        loff_t *ppos                                                        \
    )                                                                       \
    {                                                                       \
        int ret;                                                            \
        ret = nv_down_read_interruptible(&lock);                            \
        if (ret < 0)                                                        \
        {                                                                   \
            return ret;                                                     \
        }                                                                   \
        size = seq_read(file, buf, size, ppos);                             \
        up_read(&lock);                                                     \
        return size;                                                        \
    }                                                                       \
                                                                            \
    static int nv_procfs_open_##name(                                       \
        struct inode *inode,                                                \
        struct file *filep                                                  \
    )                                                                       \
    {                                                                       \
        int ret;                                                            \
        ret = single_open(filep, nv_procfs_read_##name,                     \
                          NV_PDE_DATA(inode));                              \
        if (ret < 0)                                                        \
        {                                                                   \
            return ret;                                                     \
        }                                                                   \
        return ret;                                                         \
    }                                                                       \
                                                                            \
    static int nv_procfs_release_##name(                                    \
        struct inode *inode,                                                \
        struct file *filep                                                  \
    )                                                                       \
    {                                                                       \
        return single_release(inode, filep);                                \
    }

#define NV_DEFINE_SINGLE_PROCFS_FILE_READ_ONLY(name, lock)                  \
    NV_DEFINE_SINGLE_PROCFS_FILE_HELPER(name, lock)                         \
                                                                            \
    static const nv_proc_ops_t nv_procfs_##name##_fops = {                  \
        NV_PROC_OPS_SET_OWNER()                                             \
        .NV_PROC_OPS_OPEN    = nv_procfs_open_##name,                       \
        .NV_PROC_OPS_READ    = nv_procfs_read_lock_##name,                  \
        .NV_PROC_OPS_LSEEK   = seq_lseek,                                   \
        .NV_PROC_OPS_RELEASE = nv_procfs_release_##name,                    \
    };

#define NV_DEFINE_SINGLE_PROCFS_FILE_READ_ONLY_WITHOUT_LOCK(name)           \
    static int nv_procfs_open_##name(                                       \
        struct inode *inode,                                                \
        struct file *filep                                                  \
    )                                                                       \
    {                                                                       \
        int ret;                                                            \
        ret = single_open(filep, nv_procfs_read_##name,                     \
                          NV_PDE_DATA(inode));                              \
        return ret;                                                         \
    }                                                                       \
                                                                            \
    static int nv_procfs_release_##name(                                    \
        struct inode *inode,                                                \
        struct file *filep                                                  \
    )                                                                       \
    {                                                                       \
        return single_release(inode, filep);                                \
    }                                                                       \
                                                                            \
    static const nv_proc_ops_t nv_procfs_##name##_fops = {                  \
        NV_PROC_OPS_SET_OWNER()                                             \
        .NV_PROC_OPS_OPEN    = nv_procfs_open_##name,                       \
        .NV_PROC_OPS_READ    = seq_read,                                    \
        .NV_PROC_OPS_LSEEK   = seq_lseek,                                   \
        .NV_PROC_OPS_RELEASE = nv_procfs_release_##name,                    \
    };

#endif  /* CONFIG_PROC_FS */

#endif /* _NV_PROCFS_UTILS_H */
