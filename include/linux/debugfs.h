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

#ifndef _DEBUGFS_H_
#define _DEBUGFS_H_

#include <linux/fs.h>
#include <linux/seq_file.h>

#include <linux/types.h>
#include <linux/compiler.h>

struct device;
struct file_operations;

struct debugfs_blob_wrapper {
	void *data;
	unsigned long size;
};

struct debugfs_reg32 {
	char *name;
	unsigned long offset;
};

struct debugfs_regset32 {
	const struct debugfs_reg32 *regs;
	int nregs;
	void __iomem *base;
	struct device *dev;	/* Optional device for Runtime PM */
};

struct debugfs_u32_array {
	u32 *array;
	u32 n_elements;
};

extern struct dentry *arch_debugfs_dir;

#define DEFINE_DEBUGFS_ATTRIBUTE(__fops, __get, __set, __fmt)		\
static int __fops ## _open(struct inode *inode, struct file *file)	\
{									\
	__simple_attr_check_format(__fmt, 0ull);			\
	return simple_attr_open(inode, file, __get, __set, __fmt);	\
}									\
static const struct file_operations __fops = {				\
	.owner	 = THIS_MODULE,						\
	.open	 = __fops ## _open,					\
	.release = simple_attr_release,					\
	.read	 = debugfs_attr_read,					\
	.write	 = debugfs_attr_write,					\
	.llseek  = no_llseek,						\
}

typedef struct vfsmount *(*debugfs_automount_t)(struct dentry *, void *);

#if defined(CONFIG_DEBUG_FS)

struct dentry *debugfs_lookup(const char *name, struct dentry *parent);

struct dentry *debugfs_create_file(const char *name, umode_t mode,
				   struct dentry *parent, void *data,
				   const struct file_operations *fops);
struct dentry *debugfs_create_file_unsafe(const char *name, umode_t mode,
				   struct dentry *parent, void *data,
				   const struct file_operations *fops);

void debugfs_create_file_size(const char *name, umode_t mode,
			      struct dentry *parent, void *data,
			      const struct file_operations *fops,
			      loff_t file_size);

struct dentry *debugfs_create_dir(const char *name, struct dentry *parent);

struct dentry *debugfs_create_symlink(const char *name, struct dentry *parent,
				      const char *dest);

struct dentry *debugfs_create_automount(const char *name,
					struct dentry *parent,
					debugfs_automount_t f,
					void *data);

void debugfs_remove(struct dentry *dentry);
#define debugfs_remove_recursive debugfs_remove

const struct file_operations *debugfs_real_fops(const struct file *filp);

int debugfs_file_get(struct dentry *dentry);
void debugfs_file_put(struct dentry *dentry);

ssize_t debugfs_attr_read(struct file *file, char __user *buf,
			size_t len, loff_t *ppos);
ssize_t debugfs_attr_write(struct file *file, const char __user *buf,
			size_t len, loff_t *ppos);

struct dentry *debugfs_rename(struct dentry *old_dir, struct dentry *old_dentry,
                struct dentry *new_dir, const char *new_name);

void debugfs_create_u8(const char *name, umode_t mode, struct dentry *parent,
		       u8 *value);
void debugfs_create_u16(const char *name, umode_t mode, struct dentry *parent,
			u16 *value);
void debugfs_create_u32(const char *name, umode_t mode, struct dentry *parent,
			u32 *value);
void debugfs_create_u64(const char *name, umode_t mode, struct dentry *parent,
			u64 *value);
struct dentry *debugfs_create_ulong(const char *name, umode_t mode,
				    struct dentry *parent, unsigned long *value);
void debugfs_create_x8(const char *name, umode_t mode, struct dentry *parent,
		       u8 *value);
void debugfs_create_x16(const char *name, umode_t mode, struct dentry *parent,
			u16 *value);
void debugfs_create_x32(const char *name, umode_t mode, struct dentry *parent,
			u32 *value);
void debugfs_create_x64(const char *name, umode_t mode, struct dentry *parent,
			u64 *value);
void debugfs_create_size_t(const char *name, umode_t mode,
			   struct dentry *parent, size_t *value);
void debugfs_create_atomic_t(const char *name, umode_t mode,
			     struct dentry *parent, atomic_t *value);
struct dentry *debugfs_create_bool(const char *name, umode_t mode,
				  struct dentry *parent, bool *value);
void debugfs_create_str(const char *name, umode_t mode,
			struct dentry *parent, char **value);

struct dentry *debugfs_create_blob(const char *name, umode_t mode,
				  struct dentry *parent,
				  struct debugfs_blob_wrapper *blob);

void debugfs_create_regset32(const char *name, umode_t mode,
			     struct dentry *parent,
			     struct debugfs_regset32 *regset);

void debugfs_print_regs32(struct seq_file *s, const struct debugfs_reg32 *regs,
			  int nregs, void __iomem *base, char *prefix);

void debugfs_create_u32_array(const char *name, umode_t mode,
			      struct dentry *parent,
			      struct debugfs_u32_array *array);

void debugfs_create_devm_seqfile(struct device *dev, const char *name,
				 struct dentry *parent,
				 int (*read_fn)(struct seq_file *s, void *data));

bool debugfs_initialized(void);

ssize_t debugfs_read_file_bool(struct file *file, char __user *user_buf,
			       size_t count, loff_t *ppos);

ssize_t debugfs_write_file_bool(struct file *file, const char __user *user_buf,
				size_t count, loff_t *ppos);

ssize_t debugfs_read_file_str(struct file *file, char __user *user_buf,
			      size_t count, loff_t *ppos);

#else

#include <linux/err.h>

/*
 * We do not return NULL from these functions if CONFIG_DEBUG_FS is not enabled
 * so users have a chance to detect if there was a real error or not.  We don't
 * want to duplicate the design decision mistakes of procfs and devfs again.
 */

static inline struct dentry *debugfs_lookup(const char *name,
					    struct dentry *parent)
{
	return ERR_PTR(-ENODEV);
}

static inline struct dentry *debugfs_create_file(const char *name, umode_t mode,
					struct dentry *parent, void *data,
					const struct file_operations *fops)
{
	return ERR_PTR(-ENODEV);
}

static inline struct dentry *debugfs_create_file_unsafe(const char *name,
					umode_t mode, struct dentry *parent,
					void *data,
					const struct file_operations *fops)
{
	return ERR_PTR(-ENODEV);
}

static inline void debugfs_create_file_size(const char *name, umode_t mode,
					    struct dentry *parent, void *data,
					    const struct file_operations *fops,
					    loff_t file_size)
{ }

static inline struct dentry *debugfs_create_dir(const char *name,
						struct dentry *parent)
{
	return ERR_PTR(-ENODEV);
}

static inline struct dentry *debugfs_create_symlink(const char *name,
						    struct dentry *parent,
						    const char *dest)
{
	return ERR_PTR(-ENODEV);
}

static inline struct dentry *debugfs_create_automount(const char *name,
					struct dentry *parent,
					debugfs_automount_t f,
					void *data)
{
	return ERR_PTR(-ENODEV);
}

static inline void debugfs_remove(struct dentry *dentry)
{ }

static inline void debugfs_remove_recursive(struct dentry *dentry)
{ }

const struct file_operations *debugfs_real_fops(const struct file *filp);

static inline int debugfs_file_get(struct dentry *dentry)
{
	return 0;
}

static inline void debugfs_file_put(struct dentry *dentry)
{ }

static inline ssize_t debugfs_attr_read(struct file *file, char __user *buf,
					size_t len, loff_t *ppos)
{
	return -ENODEV;
}

static inline ssize_t debugfs_attr_write(struct file *file,
					const char __user *buf,
					size_t len, loff_t *ppos)
{
	return -ENODEV;
}

static inline struct dentry *debugfs_rename(struct dentry *old_dir, struct dentry *old_dentry,
                struct dentry *new_dir, char *new_name)
{
	return ERR_PTR(-ENODEV);
}

static inline void debugfs_create_u8(const char *name, umode_t mode,
				     struct dentry *parent, u8 *value) { }

static inline void debugfs_create_u16(const char *name, umode_t mode,
				      struct dentry *parent, u16 *value) { }

static inline void debugfs_create_u32(const char *name, umode_t mode,
				      struct dentry *parent, u32 *value) { }

static inline void debugfs_create_u64(const char *name, umode_t mode,
				      struct dentry *parent, u64 *value) { }

static inline struct dentry *debugfs_create_ulong(const char *name,
						umode_t mode,
						struct dentry *parent,
						unsigned long *value)
{
	return ERR_PTR(-ENODEV);
}

static inline void debugfs_create_x8(const char *name, umode_t mode,
				     struct dentry *parent, u8 *value) { }

static inline void debugfs_create_x16(const char *name, umode_t mode,
				      struct dentry *parent, u16 *value) { }

static inline void debugfs_create_x32(const char *name, umode_t mode,
				      struct dentry *parent, u32 *value) { }

static inline void debugfs_create_x64(const char *name, umode_t mode,
				      struct dentry *parent, u64 *value) { }

static inline void debugfs_create_size_t(const char *name, umode_t mode,
					 struct dentry *parent, size_t *value)
{ }

static inline void debugfs_create_atomic_t(const char *name, umode_t mode,
					   struct dentry *parent,
					   atomic_t *value)
{ }

static inline struct dentry *debugfs_create_bool(const char *name, umode_t mode,
						 struct dentry *parent,
						 bool *value)
{
	return ERR_PTR(-ENODEV);
}

static inline void debugfs_create_str(const char *name, umode_t mode,
				      struct dentry *parent,
				      char **value)
{ }

static inline struct dentry *debugfs_create_blob(const char *name, umode_t mode,
				  struct dentry *parent,
				  struct debugfs_blob_wrapper *blob)
{
	return ERR_PTR(-ENODEV);
}

static inline void debugfs_create_regset32(const char *name, umode_t mode,
					   struct dentry *parent,
					   struct debugfs_regset32 *regset)
{
}

static inline void debugfs_print_regs32(struct seq_file *s, const struct debugfs_reg32 *regs,
			 int nregs, void __iomem *base, char *prefix)
{
}

static inline bool debugfs_initialized(void)
{
	return false;
}

static inline void debugfs_create_u32_array(const char *name, umode_t mode,
					    struct dentry *parent,
					    struct debugfs_u32_array *array)
{
}

static inline void debugfs_create_devm_seqfile(struct device *dev,
					       const char *name,
					       struct dentry *parent,
					       int (*read_fn)(struct seq_file *s,
							      void *data))
{
}

static inline ssize_t debugfs_read_file_bool(struct file *file,
					     char __user *user_buf,
					     size_t count, loff_t *ppos)
{
	return -ENODEV;
}

static inline ssize_t debugfs_write_file_bool(struct file *file,
					      const char __user *user_buf,
					      size_t count, loff_t *ppos)
{
	return -ENODEV;
}

static inline ssize_t debugfs_read_file_str(struct file *file,
					    char __user *user_buf,
					    size_t count, loff_t *ppos)
{
	return -ENODEV;
}

#endif

/**
 * debugfs_create_xul - create a debugfs file that is used to read and write an
 * unsigned long value, formatted in hexadecimal
 * @name: a pointer to a string containing the name of the file to create.
 * @mode: the permission that the file should have
 * @parent: a pointer to the parent dentry for this file.  This should be a
 *          directory dentry if set.  If this parameter is %NULL, then the
 *          file will be created in the root of the debugfs filesystem.
 * @value: a pointer to the variable that the file should read to and write
 *         from.
 */
static inline void debugfs_create_xul(const char *name, umode_t mode,
				      struct dentry *parent,
				      unsigned long *value)
{
	if (sizeof(*value) == sizeof(u32))
		debugfs_create_x32(name, mode, parent, (u32 *)value);
	else
		debugfs_create_x64(name, mode, parent, (u64 *)value);
}

#endif
