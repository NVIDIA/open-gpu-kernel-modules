// SPDX-License-Identifier: GPL-2.0
/*
 *  debugfs.h - a tiny little debug file system
 *
 *  Copyright (C) 2004 Greg Kroah-Hartman <greg@kroah.com>
 *  Copyright (C) 2004 IBM Inc.
 *
 *  debugfs is for people to use instead of /proc or /sys.
 *  See Documentation/filesystems/ for more details.
 */
#include <linux/fs.h>
#include <linux/debugfs.h>

#ifndef HAVE_DEBUGFS_CREATE_FILE_SIZE
#ifdef CONFIG_DEBUG_FS
void debugfs_create_file_size(const char *name, umode_t mode,
			      struct dentry *parent, void *data,
			      const struct file_operations *fops,
			      loff_t file_size);
#else
static inline void debugfs_create_file_size(const char *name, umode_t mode,
					    struct dentry *parent, void *data,
					    const struct file_operations *fops,
					    loff_t file_size)
{ }
#endif
#endif
