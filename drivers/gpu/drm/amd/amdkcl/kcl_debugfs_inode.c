// SPDX-License-Identifier: GPL-2.0
/*
 *  inode.c - part of debugfs, a tiny little debug file system
 *
 *  Copyright (C) 2004,2019 Greg Kroah-Hartman <greg@kroah.com>
 *  Copyright (C) 2004 IBM Inc.
 *  Copyright (C) 2019 Linux Foundation <gregkh@linuxfoundation.org>
 *
 *  debugfs is for people to use instead of /proc or /sys.
 *  See ./Documentation/core-api/kernel-api.rst for more details.
 */

#include <linux/fs.h>
#include <linux/debugfs.h>

/* Copied from fs/debugfs/inode.c */
#ifndef HAVE_DEBUGFS_CREATE_FILE_SIZE
void debugfs_create_file_size(const char *name, umode_t mode,
			      struct dentry *parent, void *data,
			      const struct file_operations *fops,
			      loff_t file_size)
{
	struct dentry *de = debugfs_create_file(name, mode, parent, data, fops);

	if (de)
		d_inode(de)->i_size = file_size;
}
EXPORT_SYMBOL_GPL(debugfs_create_file_size);
#endif
