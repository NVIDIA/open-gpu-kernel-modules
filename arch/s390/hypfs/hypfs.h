/* SPDX-License-Identifier: GPL-2.0 */
/*
 *    Hypervisor filesystem for Linux on s390.
 *
 *    Copyright IBM Corp. 2006
 *    Author(s): Michael Holzheu <holzheu@de.ibm.com>
 */

#ifndef _HYPFS_H_
#define _HYPFS_H_

#include <linux/fs.h>
#include <linux/types.h>
#include <linux/debugfs.h>
#include <linux/workqueue.h>
#include <linux/kref.h>
#include <asm/hypfs.h>

#define REG_FILE_MODE    0440
#define UPDATE_FILE_MODE 0220
#define DIR_MODE         0550

extern struct dentry *hypfs_mkdir(struct dentry *parent, const char *name);

extern struct dentry *hypfs_create_u64(struct dentry *dir, const char *name,
				       __u64 value);

extern struct dentry *hypfs_create_str(struct dentry *dir, const char *name,
				       char *string);

/* LPAR Hypervisor */
extern int hypfs_diag_init(void);
extern void hypfs_diag_exit(void);
extern int hypfs_diag_create_files(struct dentry *root);

/* VM Hypervisor */
extern int hypfs_vm_init(void);
extern void hypfs_vm_exit(void);
extern int hypfs_vm_create_files(struct dentry *root);

/* VM diagnose 0c */
int hypfs_diag0c_init(void);
void hypfs_diag0c_exit(void);

/* Set Partition-Resource Parameter */
void hypfs_sprp_init(void);
void hypfs_sprp_exit(void);

/* debugfs interface */
struct hypfs_dbfs_file;

struct hypfs_dbfs_data {
	void			*buf;
	void			*buf_free_ptr;
	size_t			size;
	struct hypfs_dbfs_file	*dbfs_file;
};

struct hypfs_dbfs_file {
	const char	*name;
	int		(*data_create)(void **data, void **data_free_ptr,
				       size_t *size);
	void		(*data_free)(const void *buf_free_ptr);
	long		(*unlocked_ioctl) (struct file *, unsigned int,
					   unsigned long);

	/* Private data for hypfs_dbfs.c */
	struct mutex		lock;
	struct dentry		*dentry;
};

extern void hypfs_dbfs_init(void);
extern void hypfs_dbfs_exit(void);
extern void hypfs_dbfs_create_file(struct hypfs_dbfs_file *df);
extern void hypfs_dbfs_remove_file(struct hypfs_dbfs_file *df);

#endif /* _HYPFS_H_ */
