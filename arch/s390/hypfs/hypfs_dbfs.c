// SPDX-License-Identifier: GPL-2.0
/*
 * Hypervisor filesystem for Linux on s390 - debugfs interface
 *
 * Copyright IBM Corp. 2010
 * Author(s): Michael Holzheu <holzheu@linux.vnet.ibm.com>
 */

#include <linux/slab.h>
#include "hypfs.h"

static struct dentry *dbfs_dir;

static struct hypfs_dbfs_data *hypfs_dbfs_data_alloc(struct hypfs_dbfs_file *f)
{
	struct hypfs_dbfs_data *data;

	data = kmalloc(sizeof(*data), GFP_KERNEL);
	if (!data)
		return NULL;
	data->dbfs_file = f;
	return data;
}

static void hypfs_dbfs_data_free(struct hypfs_dbfs_data *data)
{
	data->dbfs_file->data_free(data->buf_free_ptr);
	kfree(data);
}

static ssize_t dbfs_read(struct file *file, char __user *buf,
			 size_t size, loff_t *ppos)
{
	struct hypfs_dbfs_data *data;
	struct hypfs_dbfs_file *df;
	ssize_t rc;

	if (*ppos != 0)
		return 0;

	df = file_inode(file)->i_private;
	mutex_lock(&df->lock);
	data = hypfs_dbfs_data_alloc(df);
	if (!data) {
		mutex_unlock(&df->lock);
		return -ENOMEM;
	}
	rc = df->data_create(&data->buf, &data->buf_free_ptr, &data->size);
	if (rc) {
		mutex_unlock(&df->lock);
		kfree(data);
		return rc;
	}
	mutex_unlock(&df->lock);

	rc = simple_read_from_buffer(buf, size, ppos, data->buf, data->size);
	hypfs_dbfs_data_free(data);
	return rc;
}

static long dbfs_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct hypfs_dbfs_file *df = file_inode(file)->i_private;
	long rc;

	mutex_lock(&df->lock);
	if (df->unlocked_ioctl)
		rc = df->unlocked_ioctl(file, cmd, arg);
	else
		rc = -ENOTTY;
	mutex_unlock(&df->lock);
	return rc;
}

static const struct file_operations dbfs_ops = {
	.read		= dbfs_read,
	.llseek		= no_llseek,
	.unlocked_ioctl = dbfs_ioctl,
};

void hypfs_dbfs_create_file(struct hypfs_dbfs_file *df)
{
	df->dentry = debugfs_create_file(df->name, 0400, dbfs_dir, df,
					 &dbfs_ops);
	mutex_init(&df->lock);
}

void hypfs_dbfs_remove_file(struct hypfs_dbfs_file *df)
{
	debugfs_remove(df->dentry);
}

void hypfs_dbfs_init(void)
{
	dbfs_dir = debugfs_create_dir("s390_hypfs", NULL);
}

void hypfs_dbfs_exit(void)
{
	debugfs_remove(dbfs_dir);
}
