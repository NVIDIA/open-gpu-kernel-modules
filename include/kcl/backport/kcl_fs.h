/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _KCL_BACKPORT_KCL_FS_H
#define _KCL_BACKPORT_KCL_FS_H

#include <kcl/kcl_fs.h>
#include <linux/fs.h>

#ifndef HAVE_KERNEL_WRITE_PPOS
#define kernel_write _kcl_kernel_write
#endif

#endif
