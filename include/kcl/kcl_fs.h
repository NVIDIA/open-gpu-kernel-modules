/* SPDX-License-Identifier: GPL-2.0 */
#ifndef AMDKCL_FS_H
#define AMDKCL_FS_H

#include <linux/fs.h>
#include <asm/compat.h>

/* Copied from v5.4-rc2-1-g2952db0fd51b linux/fs.h */
#ifndef HAVE_COMPAT_PTR_IOCTL
#ifdef CONFIG_COMPAT
extern long _kcl_compat_ptr_ioctl(struct file *file, unsigned int cmd,
							unsigned long arg);
static inline long compat_ptr_ioctl(struct file *file, unsigned int cmd,
							unsigned long arg)
{
	return _kcl_compat_ptr_ioctl(file, cmd, arg);
}
#else
#define compat_ptr_ioctl NULL
#endif /* CONFIG_COMPAT */
#endif /* HAVE_COMPAT_PTR_IOCTL */

#ifndef HAVE_KERNEL_WRITE_PPOS
ssize_t _kcl_kernel_write(struct file *file, const void *buf, size_t count,
                            loff_t *pos);
#endif

#endif
