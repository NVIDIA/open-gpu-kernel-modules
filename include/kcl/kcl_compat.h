/* SPDX-License-Identifier: GPL-2.0 */
#ifndef AMDKCL_COMPATE_H
#define AMDKCL_COMPATE_H

#include <linux/compat.h>

#if !defined(HAVE_IN_COMPAT_SYSCALL)
#ifdef CONFIG_COMPAT
static inline bool in_compat_syscall(void) { return is_compat_task(); }
#else
static inline bool in_compat_syscall(void) { return false; }
#endif
#endif

#endif /* AMDKCL_COMPATE_H */
