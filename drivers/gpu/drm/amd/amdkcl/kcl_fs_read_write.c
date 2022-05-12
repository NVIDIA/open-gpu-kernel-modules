/*
 *  linux/fs/read_write.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */
#include <kcl/kcl_fs.h>
#include <linux/uaccess.h>

/* Copied from v4.13-rc7-6-ge13ec939e96b:fs/read_write.c */
#ifndef HAVE_KERNEL_WRITE_PPOS
ssize_t _kcl_kernel_write(struct file *file, const void *buf, size_t count,
                            loff_t *pos)
{
        mm_segment_t old_fs;
        ssize_t res;

        old_fs = get_fs();
        set_fs(get_ds());
        /* The cast to a user pointer is valid due to the set_fs() */
        res = vfs_write(file, (__force const char __user *)buf, count, pos);
        set_fs(old_fs);

        return res;
}
EXPORT_SYMBOL(_kcl_kernel_write);
#endif

