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
#include <linux/string.h>
#include <linux/uaccess.h>
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
#define NV_PROC_OPS_RELEASE proc_release
#if defined(NV_PROC_OPS_HAS_PROC_WRITE_BUF)
#define NV_PROC_OPS_WRITE   proc_write_buf
#else
#define NV_PROC_OPS_READ    proc_read
#define NV_PROC_OPS_WRITE   proc_write
#define NV_PROC_OPS_LSEEK   proc_lseek
#endif
#else
typedef struct file_operations nv_proc_ops_t;

#define NV_PROC_OPS_SET_OWNER() .owner = THIS_MODULE,

#define NV_PROC_OPS_OPEN    open
#define NV_PROC_OPS_READ    read
#define NV_PROC_OPS_WRITE   write
#define NV_PROC_OPS_LSEEK   llseek
#define NV_PROC_OPS_RELEASE release
#endif

/*
 * Helpers for seq_file based procfs entries.
 *
 * Without NV_PROC_OPS_HAS_PROC_WRITE_BUF, an entry's ->open() creates the
 * seq_file with single_open(), reads and seeks go through seq_read() and
 * seq_lseek(), ->release() frees the seq_file with single_release(), and
 * write handlers copy the data in from userspace.
 *
 * With it, procfs owns the seq_file of an entry that has ->proc_show(): it
 * creates the seq_file before calling ->proc_open(), reads and seeks
 * through it itself, and frees it after ->proc_release(). ->proc_open()
 * only attaches per-open data to the seq_file, ->proc_release() only frees
 * that data, and ->proc_lseek() must not be set. Writes reach
 * ->proc_write_buf() in a NUL-terminated kernel buffer of at most
 * ->proc_write_size bytes (PAGE_SIZE - 1 if that is 0), and the position
 * pointer is NULL for a stream.
 *
 *   NV_PROC_OPS_SEQ_SHOW(show)     read side of a readable seq_file entry
 *   NV_PROC_OPS_SEQ_PRIVATE(show)  seq_file of a write-only entry that only
 *                                  carries per-open data
 *   NV_PROC_OPS_SEQ_RELEASE        ->release() of an entry whose ->open()
 *                                  only calls nv_procfs_single_open()
 *   NV_PROC_OPS_WRITE_SIZE(size)   largest single write an entry accepts
 *   NV_PROC_WRITE_BUF              buffer type of a write handler
 */
#if defined(NV_PROC_OPS_HAS_PROC_WRITE_BUF)
#define NV_PROC_OPS_SEQ_SHOW(show)      .proc_show = (show),
#define NV_PROC_OPS_SEQ_PRIVATE(show)   .proc_show = (show),
#define NV_PROC_OPS_SEQ_RELEASE
#define NV_PROC_OPS_WRITE_SIZE(size)    .proc_write_size = (size),
#define NV_PROC_WRITE_BUF               char *
#else
#define NV_PROC_OPS_SEQ_SHOW(show)      .NV_PROC_OPS_READ  = seq_read, \
                                        .NV_PROC_OPS_LSEEK = seq_lseek,
#define NV_PROC_OPS_SEQ_PRIVATE(show)
#define NV_PROC_OPS_SEQ_RELEASE         .NV_PROC_OPS_RELEASE = single_release,
#define NV_PROC_OPS_WRITE_SIZE(size)
#define NV_PROC_WRITE_BUF               const char __user *
#endif

static inline int nv_procfs_single_open(
    struct file *file,
    int (*show)(struct seq_file *, void *),
    void *data
)
{
#if defined(NV_PROC_OPS_HAS_PROC_WRITE_BUF)
    ((struct seq_file *)file->private_data)->private = data;
    return 0;
#else
    return single_open(file, show, data);
#endif
}

static inline int nv_procfs_single_release(
    struct inode *inode,
    struct file *file
)
{
#if defined(NV_PROC_OPS_HAS_PROC_WRITE_BUF)
    return 0;
#else
    return single_release(inode, file);
#endif
}

static inline unsigned long nv_procfs_copy_from_write_buf(
    void *to,
    NV_PROC_WRITE_BUF from,
    size_t count
)
{
#if defined(NV_PROC_OPS_HAS_PROC_WRITE_BUF)
    memcpy(to, from, count);
    return 0;
#else
    return copy_from_user(to, from, count);
#endif
}

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

#if defined(NV_PROC_OPS_HAS_PROC_WRITE_BUF)
/*
 * procfs passes NV_PDE_DATA(inode) to ->proc_show() as the seq_file's
 * private data, like single_open() did. The lock is taken around the show
 * function instead of around the whole read.
 */
#define NV_DEFINE_SINGLE_PROCFS_FILE_HELPER(name, lock)                     \
    static int nv_procfs_show_lock_##name(                                  \
        struct seq_file *s,                                                 \
        void *v                                                             \
    )                                                                       \
    {                                                                       \
        int ret;                                                            \
        ret = nv_down_read_interruptible(&lock);                            \
        if (ret < 0)                                                        \
        {                                                                   \
            return ret;                                                     \
        }                                                                   \
        ret = nv_procfs_read_##name(s, v);                                  \
        up_read(&lock);                                                     \
        return ret;                                                         \
    }

#define NV_DEFINE_SINGLE_PROCFS_FILE_READ_ONLY(name, lock)                  \
    NV_DEFINE_SINGLE_PROCFS_FILE_HELPER(name, lock)                         \
                                                                            \
    static const nv_proc_ops_t nv_procfs_##name##_fops = {                  \
        .proc_show = nv_procfs_show_lock_##name,                            \
    };

#define NV_DEFINE_SINGLE_PROCFS_FILE_READ_ONLY_WITHOUT_LOCK(name)           \
    static const nv_proc_ops_t nv_procfs_##name##_fops = {                  \
        .proc_show = nv_procfs_read_##name,                                 \
    };

#else
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

#endif

#endif  /* CONFIG_PROC_FS */

#endif /* _NV_PROCFS_UTILS_H */
