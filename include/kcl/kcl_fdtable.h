/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _KCL_FDTABLE_H
#define _KCL_FDTABLE_H

#ifndef HAVE_KERNEL_CLOSE_FD
#include <linux/syscalls.h>
#ifdef HAVE_KSYS_CLOSE_FD
#define close_fd ksys_close
#else
#define close_fd sys_close
#endif
#endif

#endif
